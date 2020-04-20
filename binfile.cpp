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
#include "outputfile.h"
#include "systemutils.h"
#include <string.h>

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
void BinFile::Open() 
{
    std::string fName1 = filename.c_str();
    std::string fName2 = filename.c_str();
    if ((qspiDualMode == QspiMode::PARALLEL_LQSPI) || (qspiDualMode == QspiMode::PARALLEL_GQSPI) ||
        ((qspiDualMode == QspiMode::STACKED) && (totalImageSize > qspiSizeInBytes)))
    {
        fName1 = fName1.substr(0, fName1.find_last_of("."));  //remove extension
        fName1.append("_1.bin");
        fName2 = fName2.substr(0, fName2.find_last_of("."));  //remove extension
        fName2.append("_2.bin");

        ofs1.open(fName2.c_str(), std::ios::binary);

        if (!ofs1)
        {
            LOG_ERROR("Cannot write output file : %s", fName2.c_str());
        }
    }

    ofs.open(fName1.c_str(), std::ios::binary);

    if (!ofs) 
    {
        LOG_ERROR("Cannot write output file : %s", filename.c_str());
    }
}

/******************************************************************************/
void BinFile::Append() 
{
    ofs.open(filename.c_str(), std::ios::binary | std::ios::app);
}

/******************************************************************************/
void BinFile::WritePreamble()
{
    // do nothing
}

/******************************************************************************/
void BinFile::WritePostscript()
{
    // do nothing
}

/******************************************************************************/
void BinFile::Close()
{
    ofs.close();
}

/******************************************************************************/
void BinFile::Fill(Binary::Address_t start, Binary::Address_t end, bool doFill, uint8_t fillByte)
{
    /* ignore "doFill". Always fill! */
    size_t length = end - start;
    if (qspiDualMode == QspiMode::PARALLEL_GQSPI)
    {
        for (size_t i = 0; i < length/2; i++)
        {
            ofs.put((char)fillByte);
            ofs1.put((char)fillByte);
        }
    }
    else if (qspiDualMode == QspiMode::STACKED)
    {
        Binary::Length_t write_size_1 = 0;
        Binary::Length_t write_size_2 = 0;
        if ((start + length) <= qspiSizeInBytes)
        {
            for (size_t i = 0; i < length; i++)
            {
                ofs.put((char)fillByte);
            }
        }
        else
        {
            if (start <= qspiSizeInBytes)
            {
                write_size_1 = qspiSizeInBytes - start;
                for (size_t i = 0; i < write_size_1; i++)
                {
                    ofs.put((char)fillByte);
                }
            }
            write_size_2 = length - write_size_1;
            for (size_t i = 0; i < write_size_2; i++)
            {
                ofs1.put((char)fillByte);
            }
        }
    }
    else
    {
        for (size_t i = 0; i < length; i++)
        {
            ofs.put((char)fillByte);
        }
    }
    totalByteOutputCount += length;
}
	 
/******************************************************************************/
void BinFile::Write(Binary::Address_t start, Binary::Length_t length, const uint8_t *buffer)
{
    uint8_t* writedata = new uint8_t[length];
    memcpy(writedata, buffer, length);


    if (qspiDualMode == QspiMode::PARALLEL_GQSPI)
    {
        uint8_t* buffer1 = new uint8_t[length / 2];
        uint8_t* buffer2 = new uint8_t[length / 2];
        for (uint64_t i = 0, j = 0; i < length; i += 2, j++)
        {
            buffer1[j] = (writedata[i]);
            buffer2[j] = (writedata[i + 1]);
        }
        ofs.write((const char*)buffer1, length / 2);
        ofs1.write((const char*)buffer2, length / 2);
    }
    else if (qspiDualMode == QspiMode::STACKED)
    {
        Binary::Length_t write_size_1 = 0;
        Binary::Length_t write_size_2 = 0;
        if((start + length) <= qspiSizeInBytes)
        {
            ofs.write((const char*)writedata, length);
        }
        else
        {
            if (start <= qspiSizeInBytes)
            {
                write_size_1 = qspiSizeInBytes - start;
                ofs.write((const char*)writedata, write_size_1);
            }
            write_size_2 = length - write_size_1;
            ofs1.write((const char*)(writedata + write_size_1), write_size_2);
        }
    }
    else
    {
        ofs.write((const char*)writedata, length);
    }
    totalByteOutputCount += length;
    delete [] writedata;
}
