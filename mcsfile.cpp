/******************************************************************************
* Copyright 2015-2021 Xilinx, Inc.
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
#include <iostream>
#include <iomanip>
#include <string.h>

#include "outputfile.h"
#include "systemutils.h"


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define DATA_RECORD                      0x00
#define END_OF_FILE_RECORD               0x01
#define EXTENDED_SEGMENT_ADDRESS_RECORD  0x02
#define START_SEGMENT_ADDRESS_RECORD     0x03
#define EXTENDED_LINEAR_ADDRESS_RECORD   0x04
#define START_LINEAR_ADDRESS_RECORD      0x05

#define LINEAR_ADDRESS_RECORD_BIGENDIAN
/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
McsFile::McsFile()
  : pageAddress(0)
  , lineLength(16)
  , eofrWritten(false)
  , newFile(false)
  , shiftLength(0)
  , maskByte(0)
{
}

/******************************************************************************/
void McsFile::Open() 
{
    std::string fName1 = filename.c_str();
    std::string fName2 = filename.c_str();
    
    /* QSPI Mode: It is useful for generating the output file in a split format, so that the 
        files can directly be put in the repsective flashes 
        DUAL PARALLEL Mode: 2 files are generated everytime
        DUAL STACKED Mode: 2 files are generated in case the image size > QSPI flash size
    */
    if((qspiDualMode == QspiMode::PARALLEL_LQSPI) || (qspiDualMode == QspiMode::PARALLEL_GQSPI) || 
       ((qspiDualMode == QspiMode::STACKED) && (totalImageSize > qspiSizeInBytes)))
    {
        fName1 = fName1.substr(0, fName1.find_last_of("."));  //remove extension
        fName1.append("_1.mcs");
        fName2 = fName2.substr(0, fName2.find_last_of("."));  //remove extension
        fName2.append("_2.mcs");

        ofs1.open(fName2.c_str());

        if (!ofs1) 
        {
            LOG_ERROR("MCS output file writing error !!! - %s", fName2.c_str());
        }
    }

    ofs.open(fName1.c_str());

    if (!ofs) 
    {
        LOG_ERROR("MCS output file writing error !!! - %s", fName1.c_str());
    }
    pageAddress = 0xFFFFFFFF;
    for(uint8_t i=0; i<NO_SPLIT_SLAVE_FILES; i++)
    {
        filePageAddr[i] = 0xFFFFFFFF;
    }
    lineLength = ((qspiDualMode == QspiMode::PARALLEL_LQSPI)|| (qspiDualMode == QspiMode::PARALLEL_GQSPI)) ? 32 : 16;
    shiftLength = ((qspiDualMode == QspiMode::PARALLEL_LQSPI)|| (qspiDualMode == QspiMode::PARALLEL_GQSPI)) ? 17 : 16;
    maskByte = ((qspiDualMode == QspiMode::PARALLEL_LQSPI)|| (qspiDualMode == QspiMode::PARALLEL_GQSPI)) ? 0x3FFFF : 0xFFFF;
}

/******************************************************************************/
void McsFile::Append()
{
    std::string fName = filename.c_str();
    ofs.open(fName.c_str(), std::ios::app);

    /* As in Split Slave mode, there will be 3 files, the page address will be different for each
       file, so retrieving the page address of the particular file being opened. */
    pageAddress = filePageAddr[fileIndex]; 
}

/******************************************************************************/
void McsFile::WritePreamble()
{
    // no preamble needed
}

/******************************************************************************/
void McsFile::WritePostscript() 
{
    Write(0, END_OF_FILE_RECORD, NULL,0);
}

/******************************************************************************/
void McsFile::Close() 
{
    ofs.close();
    
    /* As in Split Slave mode, there will be 3 files, the page address will be different for each
       file, so saving the page address of the particular file being closed. */
    filePageAddr[fileIndex] = pageAddress;
}

/******************************************************************************/
void McsFile::Fill(Binary::Address_t start, Binary::Address_t end, bool doFill, uint8_t fillByte) 
{
    /* start changes as there are 3 diff files, offset changes for every file in split slave mode */
    start = start - splitOffset;
    if (!doFill) 
        return;
    
    uint8_t buffer[32];
    if (lineLength > sizeof(buffer)) 
    {
        LOG_DEBUG(DEBUG_STAMP, "Internal Error : LineLength (%d) > sizeof(buffer) (%d)", lineLength, sizeof(buffer));
        LOG_ERROR("Internal Error : Line Length > sizeof(buffer)");
    }

    memset(buffer,fillByte,sizeof(buffer));

    Binary::Address_t lineAddress = start;
    while(lineAddress < end) 
    {
        if(qspiDualMode == QspiMode::STACKED)
        {
            if(lineAddress >= qspiSizeInBytes)
            {
                lineAddress -= qspiSizeInBytes;
                end -= qspiSizeInBytes;
                if(!eofrWritten)
                {
                    Write(0, END_OF_FILE_RECORD, NULL,0);
                    eofrWritten = true;
                }
                newFile = true;
            }
        }
        Binary::Length_t bytesToWrite = (end - lineAddress);
        if (bytesToWrite > lineLength) 
            bytesToWrite = lineLength;

        if(qspiDualMode == QspiMode::STACKED)
        {
            if ((bytesToWrite+lineAddress) > qspiSizeInBytes)
            {
                bytesToWrite = qspiSizeInBytes - lineAddress;
            }
        }

        Binary::Address_t newPage = lineAddress >> shiftLength;
        if (newPage != pageAddress) 
        {
            uint8_t addr[2];
            // Is the extended record big-endian? or little-endian?
            // The record address is big-endian, always.
            // The extended record is not well defined.
            // It looks like the old bootgen did little endian.
#ifdef LINEAR_ADDRESS_RECORD_BIGENDIAN
            addr[0] = (uint8_t)(newPage >> 8);
            addr[1] = (uint8_t)newPage;
#else
            addr[0] = (uint8_t)(newPage);
            addr[1] = (uint8_t)(newPage>>8);
#endif
            Write(0,EXTENDED_LINEAR_ADDRESS_RECORD,addr,2);
            pageAddress = newPage;
        }
        Write(lineAddress & maskByte, DATA_RECORD, buffer, (int) bytesToWrite); 
        lineAddress += bytesToWrite;
    }
}

/******************************************************************************/
void McsFile::Write(Binary::Address_t start, Binary::Length_t length, const uint8_t *buffer) 
{
    start = start - splitOffset;
    Binary::Address_t lineAddress = start;
    Binary::Address_t endAddress = start + length;
   
    while(lineAddress < endAddress) 
    {
        if(qspiDualMode == QspiMode::STACKED)
        {
            if(lineAddress >= qspiSizeInBytes)
            {
                lineAddress -= qspiSizeInBytes;
                endAddress = start + length - qspiSizeInBytes;
                if(!eofrWritten)
                {
                    Write(0, END_OF_FILE_RECORD, NULL,0);
                    eofrWritten = true;
                }
                newFile = true;
            }
        }
        Binary::Length_t bytesToWrite = length;
        
        if (bytesToWrite > lineLength) 
            bytesToWrite = lineLength;

        if(qspiDualMode == QspiMode::STACKED)
        {
            if ((bytesToWrite+lineAddress) > qspiSizeInBytes)
            {
                bytesToWrite = qspiSizeInBytes - lineAddress;
            }
        }

        Binary::Address_t newPage = lineAddress >> shiftLength;
        if (newPage != pageAddress) 
        {
            uint8_t addr[2];
            // Is the extended record big-endian? or little-endian?
            // The record address is big-endian, always.
            // The extended record is not well defined.
            // It looks like the old bootgen did little endian.
#ifdef LINEAR_ADDRESS_RECORD_BIGENDIAN
            addr[0] = (uint8_t)(newPage >> 8);
            addr[1] = (uint8_t)newPage;
#else
            addr[0] = (uint8_t)(newPage);
            addr[1] = (uint8_t)(newPage>>8);
#endif
            Write(0,EXTENDED_LINEAR_ADDRESS_RECORD,addr,2);
            pageAddress = newPage;
        }
        Write(lineAddress & maskByte, DATA_RECORD, buffer, (int) bytesToWrite); 
        lineAddress += bytesToWrite;
        buffer += bytesToWrite;
        length -= bytesToWrite;
    }
}

/******************************************************************************/
void McsFile::Write(uint32_t address, uint8_t command, const uint8_t* data, int len) 
{
    uint8_t buffer[128];
    uint8_t writedata[128];
    
    if((qspiDualMode != QspiMode::PARALLEL_LQSPI) && (qspiDualMode != QspiMode::PARALLEL_GQSPI))
    {
        buffer[0] = len;
        buffer[1] = (uint8_t)(address >> 8);
        buffer[2] = (uint8_t)(address     );
        buffer[3] = command;
        
        memcpy(writedata, data, len);

        /* Only applicable to Data Records
           To match the MCS file generation in different interface formats
           according to write_cfgmem of Vivado */
        if(command == DATA_RECORD)
        {
        }

        memcpy(buffer+4,writedata,len);
        uint8_t checksum = 0;

        for(int i=0;i<len+4;i++) {
            checksum += buffer[i];
        }
        buffer[4+len] = -checksum;
        
        if(!newFile)
        {
            ofs << ':';
            for(int i=0;i<len+5;i++) 
            {
                ofs << std::uppercase <<  std::hex << std::setfill('0') << std::setw(2) << (unsigned int) buffer[i];
                totalByteOutputCount ++;
            }
            ofs << "\n";
        }
        else
        {
            ofs1 << ':';
            for(int i=0;i<len+5;i++) 
            {
                ofs1 << std::uppercase <<  std::hex << std::setfill('0') << std::setw(2) << (unsigned int) buffer[i];
                totalByteOutputCount ++;
            }
            ofs1 << "\n";
        }
    }
    else
    {
        uint8_t buffer2[128];
        uint8_t length;
        uint32_t add = address/2;

        length = (command == DATA_RECORD) ? (len/2) : len;
        buffer[0] = buffer2[0] = length;
        buffer[1] = buffer2[1] = (uint8_t)(add >> 8);
        buffer[2] = buffer2[2] = (uint8_t)(add     );
        buffer[3] = buffer2[3] = command;
        
        if (command == DATA_RECORD)
        {
            if (qspiDualMode == QspiMode::PARALLEL_LQSPI)
            {
                int k = 0;
                for (int i = 0; i < len; i += 2, k++)
                {
                    //buffer[4+j] = (data[i] & 0xF0) | ((data[i+1] & 0xF0) >> 4);
                    //buffer2[4+j] = ((data[i] & 0x0F) << 4) | (data[i+1] & 0x0F);

                    uint8_t lsb = 0, lsbNxt = 0;
                    uint8_t msb = 0, msbNxt = 0;
                    uint8_t tempData = data[i];
                    uint8_t tempDataNxt = data[i + 1];
                    for (int j = 0; j < 4; j++)
                    {
                        lsb = lsb | ((tempData & 0x1) << j);
                        msb = msb | (((tempData & 0x2) >> 1) << j);
                        tempData = tempData >> 2;
                        lsbNxt = lsbNxt | ((tempDataNxt & 0x1) << j);
                        msbNxt = msbNxt | (((tempDataNxt & 0x2) >> 1) << j);
                        tempDataNxt = tempDataNxt >> 2;
                    }
                    buffer[4 + k] = (lsb << 4) | lsbNxt;
                    buffer2[4 + k] = (msb << 4) | msbNxt;
                }
            }

            else if (qspiDualMode == QspiMode::PARALLEL_GQSPI)
            {
                for (int i = 0, j = 0; i < len; i += 2, j++)
                {
                    buffer[4 + j] = (data[i]);
                    buffer2[4 + j] = (data[i + 1]);
                }
            }
        }
        else
        {
            memcpy(buffer + 4, data, length);
            memcpy(buffer2 + 4, data, length);
        }
        uint8_t checksum = 0;
        uint8_t checksum2 = 0;
        for(int i=0;i<length+4;i++) 
        {
            checksum += buffer[i];
            checksum2 += buffer2[i];
        }
        buffer[4+length] = -checksum;
        buffer2[4+length] = -checksum2;

        ofs << ':';
        ofs1 << ':';
        for(int i=0;i<length+5;i++) 
        {
            ofs << std::uppercase <<  std::hex << std::setfill('0') << std::setw(2) << (unsigned int) buffer[i];
            ofs1 << std::uppercase <<  std::hex << std::setfill('0') << std::setw(2) << (unsigned int) buffer2[i];
            totalByteOutputCount ++;
        }
        ofs << "\n";
        ofs1 << "\n";
    }
}
