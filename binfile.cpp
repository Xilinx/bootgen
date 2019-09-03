/******************************************************************************
* Copyright 2015-2019 Xilinx, Inc.
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
    if(qspiDualMode != QspiMode::Unknown)
    {
        LOG_ERROR("'dual_qspi_mode' option is not supported for BIN files");
    }
    ofs.open(filename.c_str(), std::ios::binary);

    if (!ofs) 
    {
        LOG_ERROR("Can't write output file : %s", filename.c_str());
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
    size_t count = end - start;
    for(size_t i = 0; i < count ; i++)
    {
        ofs.put((char)fillByte);
    }
    totalByteOutputCount += count;
}

/******************************************************************************/
void BinFile::Write(Binary::Address_t start, Binary::Length_t length, const uint8_t *buffer)
{
    ofs.write((const char*)buffer,length);
    totalByteOutputCount += length;
}
