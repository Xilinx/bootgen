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

#pragma once
#ifndef _BITUTILS_H_
#define _BITUTILS_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <assert.h>
#include <iostream>
#include <string.h>
#include <istream>
#include <limits>
#include "logger.h"
#include "bootimage.h"

/* Forward class references */
class OutputStream_LE;


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define BITS_IN_BYTE    8
#define BITSTREAM_NOP   0x20000000


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class InputStream_BE
{
public:
    InputStream_BE(std::istream& stream0) : rbt(false),stream(stream0) {}

    void Read(uint8_t* dataBuffer, uint32_t dataByteLength) 
    {
        stream.read((char*)dataBuffer,dataByteLength);
    }
    
    void Skip(int count) 
    {
        stream.seekg(count,std::ios_base::cur);
    }

    void Back(int count)
    {
        count *= -1;
        stream.seekg(count, std::ios_base::cur);
    }
    
    uint8_t ReadByte() 
    {
        uint8_t x;
        stream.read((char*)&x,1);
        return x;
    }

    int Peek() 
    {
        return stream.peek();
    }
    
    /* Read 2 bytes (16 bits) from the stream (big endian) */
    uint16_t ReadShort() 
    {
        uint16_t x;
        x = ((uint16_t)ReadByte()) << 8;
        x |= ReadByte();
        return x;
    }

    /* Read 4 bytes (32 bits) from the stream (big endian) */
    uint32_t ReadLong() 
    {
        uint32_t x = 0;
        if (rbt)
        {
            std::string token = ReadLine();
            if (token != "")
            {
                x = DecodeBinary(token);
            }
        }
        else
        {
            x = ((uint32_t)ReadByte()) << 24;
            x |= ((uint32_t)ReadByte()) << 16;
            x |= ((uint32_t)ReadByte()) << 8;
            x |= ReadByte();
        }
        return x;
    }

    /* Read & Decode the binary data */
    uint32_t DecodeBinary(const std::string &sBin)
    {
        /* Check for a bad string */
        if (sBin.npos != sBin.find_first_not_of("01"))
        {
            LOG_DEBUG(DEBUG_STAMP,"Badly formed input string");
            LOG_ERROR("Failed to read Bitstream !!!");
        }
        /* Check for overflow */
        if (sBin.length() > std::numeric_limits<unsigned>::digits)
        {
            LOG_DEBUG(DEBUG_STAMP, "The binary number is too big to convert to an unsigned int");
            LOG_ERROR("Failed to read Bitstream !!!");
        }
        /* For each binary digit, starting from the least significant digit, 
           set the appropriate bit if the digit is not '0' */
        unsigned nVal = 0;
        unsigned nBit = 0;
        std::string::const_reverse_iterator itr;
        for(itr=sBin.rbegin(); itr!=sBin.rend(); ++itr)
        {
            if (*itr == '1')
            {
                nVal |= (1<<nBit);
            }
            ++nBit;
        }
        return nVal;
    }

    /* Read Pascal string data */
    std::string ReadPascalString() 
    {
        int len = ReadShort();
        if (len == 0) 
        {
            return std::string("");
        }

        // ignore null
        len--; 

        std::string s(len,0);

        for(int i=0;i<len;i++) 
        {
            uint8_t x = ReadByte();
            s[i] = x;
        }

        // skip null
        (void)ReadByte();
        return s;
    }

    /* Read Pascal string data */
    std::string ReadCString() 
    {
        std::string s = "";
        uint8_t x;
        while( (x = ReadByte()) != 0 ) 
        {
            s += x;
        }
        return s;
    }

    void Start(void)
    {
        stream.clear();
        stream.seekg(0, std::ios_base::beg);
    }

    /* Convert string to 16-bytes hex data */
    void PackHex(const std::string&	hexString, uint8_t* hexData)
    {
        uint32_t hexStringLength = (uint32_t)hexString.size();

        if (hexStringLength & 1)
        {
            LOG_DEBUG(DEBUG_STAMP, "Hex String - %s - does not have even no. of hex digits", hexString.c_str());
            LOG_ERROR("Error parsing bitstream file");
        }

        for (uint32_t i = 0; i<hexStringLength; i += 2)
        {
            std::string byte = hexString.substr(i, 2);
            if (!isxdigit(byte[0]) || !isxdigit(byte[1]))
            {
                LOG_DEBUG(DEBUG_STAMP, "Hex String - %s - is has a non hex digit", hexString.c_str());
                LOG_ERROR("Error parsing bitstream file");
            }
            *hexData++ = (uint8_t)strtoul(byte.c_str(), NULL, 16);
        }
    }

    /* Read one line of CFI - 16-bytes */
    void ReadCfiLine(uint8_t* cfiBytes)
    {
        std::string line = ReadLine();
        if (line != "")
        {
            PackHex(line, cfiBytes);
        }
    }

    /* Read one line of NPI - 4-bytes */
    void ReadNpiLine(uint8_t* npiBytes)
    {
        std::string line = ReadLine();
        if (line != "")
        {
            if (line.compare(0, 2, "0x") == 0) {
                line.erase(0, 2);
            }
            PackHex(line, npiBytes);
        }
    }

    /* Read line from bitstream data */
    std::string ReadLine()
    {
        std::string s = "";
        int maxlinelength = 256;
        int count = 0;
        do 
        {
            uint8_t x = ReadByte();
            if (x == 0 || x == '\n')
            {
                return s;
            }
            else if (x == '\r')
            {
                // discard
            }
            else 
            {
                s += x;
            }
            count++;
        } while (count < maxlinelength);
        
        LOG_ERROR("Read failure in bitstream input");
        return s;
    }

    /* Set if the bitstream is RBT file */
    void SetRbt() 
    {
        rbt = true;
    }
    
private:
    bool rbt;
    std::istream& stream;
};

/******************************************************************************/
class OutputStream
{
public:
    OutputStream() : maxsize(0) 
    {
        buffer = 0;
        ptr = 0;
        pHdr.data = NULL;
        pHdr.size = 0;
        swap_cfi_bytes = false;
        changeEndianness = false;
    }

    OutputStream(size_t maxsize0) : maxsize(maxsize0) 
    {
        buffer = new uint8_t[maxsize];
        ptr= buffer;
    }

    virtual ~OutputStream()
    {
        delete[] buffer;
    }
     
    /* Allocate new buffer for the bitstream */
    void Alloc(size_t maxsize0) 
    {
        if (buffer)
        {
            delete[] buffer;
        }

        maxsize = maxsize0;
        buffer = new uint8_t[maxsize];
        ptr = buffer;
    }

    void WriteCfiLine(uint8_t* cfiBytes)
    {
        uint32_t lastIndex = 16 - 1;
        char tempInt = 0;

        // If array is NULL, return
        if (!cfiBytes)
        {
            return;
        }

        if (swap_cfi_bytes == true)
        {
            for (uint32_t loop = 0; loop <= (lastIndex / 2); loop++)
            {
                tempInt = cfiBytes[loop];
                cfiBytes[loop] = cfiBytes[lastIndex - loop];
                cfiBytes[lastIndex - loop] = tempInt;
            }
        }
        for (int i = 0; i <= 3; i++)
        {
            uint32_t word;
            memcpy(&word, &(cfiBytes[4 * i]), sizeof(word));
            WriteLong(word);
        }
    }

    const uint8_t* Start() 
    {
        return buffer;
    }

    size_t Size() 
    {
        return (size_t)(ptr - buffer);
    }

    void Rewind(size_t count) 
    {
        ptr -= count;
    }

    virtual void WriteLong(uint32_t word) = 0;

    PreservedHeader pHdr;
    bool swap_cfi_bytes;

protected:
    size_t maxsize;
    uint8_t* buffer;
    uint8_t* ptr;
    bool changeEndianness;
};

/******************************************************************************/
class OutputStream_LE : public OutputStream 
{
public:
    void WriteLong(uint32_t word) 
    {
        *ptr++ = (word >> (BITS_IN_BYTE * 0)) & 0xFF;
        *ptr++ = (word >> (BITS_IN_BYTE * 1)) & 0xFF;
        *ptr++ = (word >> (BITS_IN_BYTE * 2)) & 0xFF;
        *ptr++ = (word >> (BITS_IN_BYTE * 3)) & 0xFF;
        assert(Size() <= maxsize);
    }
};

/******************************************************************************/
class OutputStream_BE : public OutputStream
{
public:
    OutputStream_BE() { };
    OutputStream_BE(size_t maxsize0) : OutputStream(maxsize0) {}
    ~OutputStream_BE() {}

    void WriteLong(uint32_t word) 
    {
        *ptr++ = (word >> (BITS_IN_BYTE * 3)) & 0xFF;
        *ptr++ = (word >> (BITS_IN_BYTE * 2)) & 0xFF;
        *ptr++ = (word >> (BITS_IN_BYTE * 1)) & 0xFF;
        *ptr++ = (word >> (BITS_IN_BYTE * 0)) & 0xFF;
        assert(Size() <= maxsize);
    }
};

/******************************************************************************/
class BitFileHeaderType
{
public:

    uint8_t headerType;
    std::string unknownString;
    uint32_t unknownValue;
    char fieldSeparatorA;
    std::string designName;
    char fieldSeparatorB;
    std::string devicePackageName;
    char fieldSeparatorC;
    std::string designDate;
    char fieldSeparatorD;
    std::string designTime;
    char fieldSeparatorE;
    uint32_t dataByteLength;
    std::string dataByteLengthString;
    uint8_t * bitStreamBuffer;
    std::string deviceFamily;
    std::string designDateAndTime;
    char unknownData[8];

    BitFileHeaderType() 
        : headerType(0)
        , dataByteLength(0)
        , unknownValue(0)
        , bitStreamBuffer(NULL)
        , unknownString()
        , fieldSeparatorA()
        , fieldSeparatorB()
        , fieldSeparatorC()
        , fieldSeparatorD()
        , fieldSeparatorE()
    {
    }

    void DumpBITHeader( std::ostream& s )
    {
        s << "Xilinx BIT Bitstream\n" ;
        s << "Design name : " << designName << "\n";
        s << "Part        : " << devicePackageName << "\n";
        s << "Date        : " << designDateAndTime << "\n";
        s << "Bits        : " << (dataByteLength * BITS_IN_BYTE ) << "\n";
        s << std::endl;
    }
} ;

/******************************************************************************/
class BitFile
{
public:
    BitFile(std::istream& stream0);
    ~BitFile();

    void ParseBitFpga();
    virtual void ParseBit(BootImage& bi);
    virtual void SetEncryptionType(Encryption::Type);

    /* For Zynq/FPGA encrypted files only */
    virtual void Strip(OutputStream* os);
    
    /* For ZynqMp files and Zynq/FPGA un-encrypted files */
    virtual void Copy(OutputStream* os);

    virtual OutputStream* GetOutputStreamType(void) = 0;
    virtual bool GetBitStripFlag(void) = 0;
    virtual bool GetBitPadFlag(bool) = 0;
    virtual void DummyRead(void) = 0;
    virtual void ComparePartsDataBase(const std::string&) = 0;
    virtual void CopyNpi(OutputStream* os) {};

    std::string inputPackageName;
    BitFileHeaderType header;

protected:
    virtual void ReadBITHeader_x4k();
    virtual void ReadBITHeader_NeoCAD();
    virtual void ReadRbtHeader();
    virtual void ValidateDeviceName(const std::string& packageName);

    Encryption::Type encryptType;
    InputStream_BE is;
    OutputStream_BE* temp;
};

/******************************************************************************/
class ZynqMpBitFile : public BitFile
{
public:
    ZynqMpBitFile(std::istream& stream) : BitFile(stream) { };
    OutputStream* GetOutputStreamType(void);
    bool GetBitStripFlag(void);
    bool GetBitPadFlag(bool);
    void DummyRead(void) { };
    void ComparePartsDataBase(const std::string&);
};

/******************************************************************************/
class ZynqBitFile : public BitFile
{
public:
    ZynqBitFile(std::istream& stream) : BitFile(stream) { };
    OutputStream* GetOutputStreamType(void);
    bool GetBitStripFlag(void);
    bool GetBitPadFlag(bool);
    void DummyRead(void) { };
    void ComparePartsDataBase(const std::string&);
};

/******************************************************************************/
class BitCommand
{
public:
    void ReadWrite(InputStream_BE& is, OutputStream* os);

    uint32_t instruction;
    uint32_t count;
    uint32_t value;

    typedef enum Config_command_type
    {
        Reserve0_cmd = 0x00,
        WCFG_cmd = 0x01,
        MFWR_cmd = 0x02,
        LFRM_cmd = 0x03,
        RCFG_cmd = 0x04,
        START_cmd = 0x05,
        RCAP_cmd = 0x06,
        RCRC_cmd = 0x07,
        AGHIGH_cmd = 0x08,
        SWITCH_cmd = 0x09,
        GRESTORE_cmd = 0x0A,
        SHUTDOWN_cmd = 0x0B,
        GCAPTURE_cmd = 0x0C,
        DESYNCH_cmd = 0x0D,
        REBOOT_cmd = 0x0E,
        Reserve15_cmd = 0x0F
    } Config_command_type;

    typedef enum Config_register_type
    {
        CRC_reg = 0x00,
        FAR_reg = 0x01,
        FDRI_reg = 0x02,
        FDRO_reg = 0x03,
        CMD_reg = 0x04,
        CTL_reg = 0x05,
        MASK_reg = 0x06,
        STAT_reg = 0x07,
        LOUT_reg = 0x08,
        COR_reg = 0x09,
        MFWR_reg = 0x0A,
        CBC_reg = 0x0B,
        IDCODE_reg = 0x0C,
        AXSS_reg = 0x0D,
        COR1_reg = 0x0E,
        WBSTAR_reg = 0x10,
        TIMER_reg = 0x11,
        BOOTSTS_reg = 0x16,
        CTL1_reg = 0x18,
        BSPI_reg = 0x1F,
    } Config_register_type;
    Config_register_type reg;

    typedef enum
    {
        cmd_word = 0x01,
        extension_word = 0x02
    } Config_word_type;
    Config_word_type word_type;

    typedef enum
    {
        kConfig_NOP_cmd = 0x00,
        kConfig_read_cmd = 0x01,
        kConfig_write_cmd = 0x02,
        kConfig_decrypt_cmd = 0x03
    } Config_command_RW_type;

    uint32_t Config_Word_Get_Word_Count()
    {
        /* [10:0] */
        return ((instruction >> 0) & 0x7FF);
    }

    Config_register_type Config_Word_Get_Register()
    {
        /* [18:13] */
        return (Config_register_type)((instruction >> 13) & 0x1F); 
    }

    Config_command_RW_type Config_Word_Get_Op()
    {
        /* [28:27] */
        return (Config_command_RW_type)((instruction >> 27) & 0x3); 
    }

    Config_word_type Config_Word_Get_Type()
    {
        /* [31:29] */
        return (Config_word_type)((instruction >> 29) & 0x7); 
    }

    inline uint32_t Config_Word_Get_Extension_Count()
    {
        return (instruction & 0x07FFFFFF);
    }
};

#endif
