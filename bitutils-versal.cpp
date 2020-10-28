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
#include "bitutils-versal.h"
#include <assert.h>
#include <iostream>
#include <string.h>
#include "bootimage.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
VersalBitFile::VersalBitFile(std::istream& stream0)
    : BitFile(stream0),
    temp(NULL),
    encryptType(Encryption::None)
{
}

/******************************************************************************/
VersalBitFile::~VersalBitFile()
{
    if (temp)
    {
        delete temp;
    }
}

/******************************************************************************/
void VersalBitFile::Copy(OutputStream* os)
{
    os->swap_cfi_bytes = true;
    os->Alloc(header.dataByteLength + 32);
    if (header.dataByteLength % 4)
    {
        LOG_ERROR("Bitstream length %d - is not word aligned", header.dataByteLength);
    }
    uint32_t bytes = header.dataByteLength / sizeof(uint8_t);
    uint8_t cfiBytes[16] = { 0 };
    uint32_t numCfiLines = bytes / 16;
    for (uint32_t i = 0; i < numCfiLines; i++)
    {
        is.ReadCfiLine(cfiBytes);
        os->WriteCfiLine(cfiBytes);
    }
}

/******************************************************************************/
void VersalBitFile::CopyNpi(OutputStream* os)
{
    os->Alloc(header.dataByteLength + 32);
    if (header.dataByteLength % 4)
    {
        LOG_ERROR("NPI length %d - is not word aligned", header.dataByteLength);
    }
    uint32_t bytes = header.dataByteLength / sizeof(uint8_t);
    uint8_t npiBytes[4] = { 0 };
    uint32_t numnpiLines = bytes / 4;
    bool change_endianness = false;

    /* Read ID value (XNLX/CDO) from CDO Header to identify endianness */
    /* 8 characters (npi line) + one new line character = 9 characters */
    //is.Skip(9);
    is.ReadNpiLine(npiBytes);
    is.ReadNpiLine(npiBytes);
    uint32_t npi_word = ReadBigEndian32(npiBytes);
    if ((npi_word != 0x584c4e58) && (npi_word != 0x584e4c58) && (npi_word != 0x004f4443) && (npi_word != 0x43444f00))
    {
        LOG_ERROR("Invalid CDO format - incorrect identification word (XLNX/CDO) - 0x%x", npi_word);
    }
    if ((npi_word == 0x584c4e58) || (npi_word == 0x004f4443))
    {
        /* Convert BE CDOs to LE */
        change_endianness = true;
        LOG_WARNING("CDO is in Big Endian format. Big Endian CDOs are deprecated. Please use little endian CDOs.");
    }
    /* HACK - Need correction */
    /* Few CDO files have 0x prepended to each word
    So setting the stream to start and reading the banner again to reach the actual CDO word */
    is.Start();
    ReadRbtHeader();

    for (uint32_t i = 0; i < numnpiLines; i++)
    {
        is.ReadNpiLine(npiBytes);
        if (change_endianness)
        {
            npi_word = ReadBigEndian32(npiBytes);
        }
        else
        {
            npi_word = ReadLittleEndian32(npiBytes);
        }
        os->WriteLong(npi_word);
    }
}

/******************************************************************************/
OutputStream* VersalBitFile::GetOutputStreamType(void)
{
    return new OutputStream_LE();
}

/******************************************************************************/
bool VersalBitFile::GetBitStripFlag(void)
{
    return false;
}

/******************************************************************************/
bool VersalBitFile::GetBitPadFlag(bool)
{
    return false;
}

/******************************************************************************/
bool VersalBitFile::GetPreserveHeaderFlag(void)
{
    return false;
}

/******************************************************************************/
void VersalBitFile::ComparePartsDataBase(const std::string&)
{
    /* TO-DO */
    return;
}

