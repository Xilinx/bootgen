/******************************************************************************
* Copyright 2015-2020 Xilinx, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <string>
#include <sstream>
#include "bootgenexception.h"
#include "bitutils.h"
#include <assert.h>
#include <iostream>
#include <string.h>
#include "bootimage.h"


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define NO_IO_ERROR     0
#define DATA_SYNC_WORD  0xAA995566
#define TIMER_CMD_WORD  0x30022001


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
BitFile::BitFile(std::istream& stream0) 
    : is(stream0),
      temp(NULL),
      encryptType(Encryption::None)
{ 
}

/******************************************************************************/
BitFile::~BitFile()
{
    if (temp) 
    {
        delete temp;
    }
}

/******************************************************************************/
void BitFile::ParseBit(BootImage& bi)
{
    switch (is.Peek()) 
    {
        case 0x00:
            ReadBITHeader_x4k();
            break;
        case 0x19:
            ReadBITHeader_NeoCAD();
            break;
        case 'X':
        case 'x':
            ReadRbtHeader();
            break;
        default:
            LOG_DEBUG(DEBUG_STAMP, "First Byte in Bitstream - 0x%X", is.Peek());
            LOG_ERROR("Bitstream parsing error !!!           Unsupported BIT file");
    }
    inputPackageName = bi.options.GetDevicePackageName();
    if(bi.partitionOutput->Type() != PartitionArch::FPGA)
    {
        ValidateDeviceName(header.devicePackageName);
    }
}

/******************************************************************************/
void BitFile::ReadBITHeader_x4k( )
{
    header.unknownString    = is.ReadPascalString( );
    header.unknownValue     = is.ReadShort( );
    header.fieldSeparatorA  = is.ReadByte( );
    header.designName       = is.ReadPascalString();
    header.fieldSeparatorB  = is.ReadByte( );
    header.devicePackageName= is.ReadPascalString();
    header.fieldSeparatorC  = is.ReadByte( );
    header.designDate       = is.ReadPascalString();
    header.fieldSeparatorD  = is.ReadByte( );
    header.designTime       = is.ReadPascalString();

    if( ( header.fieldSeparatorA != 'a' ) ||
        ( header.fieldSeparatorB != 'b' ) ||
        ( header.fieldSeparatorC != 'c' ) ||
        ( header.fieldSeparatorD != 'd' ) ||
        ( header.designName.size() == 0 ) ||
        ( header.devicePackageName.size() == 0 ) ||
        ( header.designDate.size() == 0 ) ||
        ( header.designTime.size() == 0 ) ||
        ( header.unknownString != "\x0F\xF0\x0F\xF0\x0F\xF0\x0F\xF0" ) )
        {
            LOG_ERROR("Bitstream parsing error !!!           Invalid/Corrupted Bitstream Header");
        }

    header.designDateAndTime = header.designDate + " " + header.designTime;
    header.fieldSeparatorE  = is.ReadByte();
    header.dataByteLength   = is.ReadLong();
}

/******************************************************************************/
void BitFile::ReadBITHeader_NeoCAD()
{
    header.headerType        = is.ReadByte();
    header.designName        = is.ReadCString();
    header.deviceFamily      = is.ReadCString();
    header.devicePackageName = is.ReadCString();
    header.designDateAndTime = is.ReadCString();

    /* Skip unknown data */
    is.Skip(sizeof(header.unknownData)); 
    header.dataByteLength    = is.ReadLong() / BITS_IN_BYTE;
}

/******************************************************************************/
void BitFile::Copy(OutputStream* os)
{
    os->Alloc(header.dataByteLength + 32);
    if (header.dataByteLength % 4) 
    {
        LOG_ERROR("Bitstream length %d - is not word aligned", header.dataByteLength);
    }
    int words = header.dataByteLength / sizeof(uint32_t);
    for(int i=0;i<words;i++) 
    {
        uint32_t x = is.ReadLong();
        os->WriteLong(x);
    }
}

/******************************************************************************/
void BitFile::Strip(OutputStream* os) 
{
    os->Alloc(header.dataByteLength + 32);
    uint32_t cmdWord;
    
    /* In case of Zynq bitstreams, skip synchronization words, until we get the magic AA995566 */
    do
    {
        cmdWord = is.ReadLong();
    } while (cmdWord != DATA_SYNC_WORD);

    /* Dummy Read is necessary to move to next location where the actual bitstream starts 
       FPGA encryption bitstream does not include this NOP data
       Zynq & ZynqMP encrypted bitstreams contain this NOP data */
    DummyRead();

    /* Now copy all command words, until we get a DESYNCH or REBOOT command */
    BitCommand cmd;
    do
    {
        cmd.ReadWrite(is,os);
    } while (! (cmd.reg == BitCommand::CMD_reg && (cmd.value == BitCommand::DESYNCH_cmd || cmd.value == BitCommand::REBOOT_cmd)));

    /* Don't copy the DESYNCH or REBOOT command. */
    os->Rewind(2*sizeof(uint32_t));
}

/******************************************************************************/
void BitFile::ReadRbtHeader()
{
    std::string line = is.ReadLine();
    if ((line != "Xilinx ASCII Bitstream") && (line != "Xilinx ASCII CFI Deviceimage") && 
        (line != "Xilinx ASCII NPI Deviceimage") && (line != "Xilinx ASCII PSAXIMM Deviceimage"))
    {
        LOG_DEBUG(DEBUG_STAMP, "Wrong Line - %s", line.c_str());
        LOG_ERROR("Bitstream parsing error !!!           First line of RBT has incorrect starting line");
    }

    if (line == "Xilinx ASCII Bitstream")
    {
        is.SetRbt();
    }
    while (true)
    {
        line = is.ReadLine();
        if (line == "")
        {
            break;
        }
        size_t colon = line.find(':');
        if (colon != std::string::npos)
        {
            std::string field = line.substr(0, colon);
            size_t valuePtr = line.find_first_not_of(" \t", colon + 1);
            if (valuePtr != std::string::npos)
            {
                if (field == "Design name")
                {
                    header.designName = line.substr(valuePtr);
                }
                else if (field == "Part")
                {
                    header.devicePackageName = line.substr(valuePtr);
                }
                else if (field == "Date")
                {
                    header.designDateAndTime = line.substr(valuePtr);
                }
                else if (field == "Bits")
                {
                    header.dataByteLengthString = line.substr(valuePtr);
                    uint32_t y;
                    std::stringstream x(header.dataByteLengthString);
                    x >> y;
                    header.dataByteLength = y / BITS_IN_BYTE;
                    break;
                }
            }
        }
    }
    ValidateDeviceName(header.devicePackageName);
}

/******************************************************************************/
void BitFile::ValidateDeviceName(const std::string& packageName)
{
    if (inputPackageName != "")
    {
        if (packageName.compare(0, inputPackageName.length(), inputPackageName) == 0)
        {
            return;
        }
    }
    else
    {
        ComparePartsDataBase(packageName);
    }
}

/******************************************************************************/
void BitFile::SetEncryptionType(Encryption::Type type)
{
    encryptType = type;
}

/******************************************************************************/
OutputStream* ZynqBitFile::GetOutputStreamType(void)
{
    return new OutputStream_LE();
}

/******************************************************************************/
bool ZynqBitFile::GetBitStripFlag(void)
{
    if (encryptType == Encryption::AES)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/******************************************************************************/
bool ZynqBitFile::GetBitPadFlag(bool legacyFlag)
{
    if (legacyFlag && (encryptType == Encryption::None))
    {
        /* Don't pad to 32 bytes */
        return false;
    }
    else
    {
        return true;
    }
}

/******************************************************************************/
void ZynqBitFile::ComparePartsDataBase(const std::string& packageName)
{
    const char* parts[] = { "xc7z", "7z", "xa7z", "a7z", "xq7z", "q7z", NULL };
    for (int i = 0; parts[i]; i++)
    {
        if (packageName.compare(0, strlen(parts[i]), parts[i]) == 0)
        {
            return;
        }
    }
    LOG_DEBUG(DEBUG_STAMP, "BIT file package name - %s", packageName.c_str());
    LOG_ERROR("Partname %s in Bit file is incompatible with Zynq (Zynq 7000) parts", packageName.c_str());
}

/******************************************************************************/
OutputStream* ZynqMpBitFile::GetOutputStreamType(void)
{
    if(encryptType == Encryption::AES)
    {
        return new OutputStream_BE();
    }
    else
    {
        return new OutputStream_LE();
    }
}

/******************************************************************************/
bool ZynqMpBitFile::GetBitStripFlag(void)
{
    return false;
}

/******************************************************************************/
bool ZynqMpBitFile::GetBitPadFlag(bool legacyFlag)
{
    return false;
}

/******************************************************************************/
void ZynqMpBitFile::ComparePartsDataBase(const std::string& packageName)
{
    const char* parts[] = { "xqzu", "xczu", "xazu", "xck", NULL };
    for(int i=0; parts[i]; i++) 
    {
        if (packageName.compare(0,strlen(parts[i]),parts[i]) == 0) 
        {
            return;
        }
    }
    LOG_DEBUG(DEBUG_STAMP, "BIT file package name - %s", packageName.c_str());
    LOG_ERROR("Partname %s in Bit file is incompatible with Zynq MP parts", packageName.c_str());
}

/******************************************************************************/
void BitCommand::ReadWrite(InputStream_BE& is, OutputStream* os)
{
    instruction = is.ReadLong();
    os->WriteLong(instruction);

    word_type = Config_Word_Get_Type();
    reg = Config_Word_Get_Register();

    if (Config_Word_Get_Op() == kConfig_NOP_cmd)
    {
        count = 0;
    }
    else
    {
        if (word_type == cmd_word)
        {
            count = Config_Word_Get_Word_Count();
        }
        else if (word_type == extension_word)
        {
            count = Config_Word_Get_Extension_Count();
        }
        else
        {
            LOG_DEBUG(DEBUG_STAMP, "Bad command word in BIT stream: 0x%X", instruction);
        }
    }
    for (uint32_t i = 0;i<count;i++)
    {
        uint32_t x;
        x = is.ReadLong();
        os->WriteLong(x);

        if (i == 0)
        {
            value = x;
        }
    }
}
