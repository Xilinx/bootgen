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
#include "bootimage.h"
#include <memory>
#include "options.h"
#include "fileutils.h"

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
BootHeader::BootHeader()
    : prebuilt(false)
    , bhSize(0)
    , fullBhSize(0)
    , allHdrSize(0)
    , slaveBootSplitMode(false)
{
}

/******************************************************************************/
void BootHeader::BootloaderAddressAndSizeCheck(PartitionHeader & hdr)
{
    if (!Binary::CheckAddress(hdr.partition->section->Address))
    {
        LOG_ERROR("Bootgen does not support addresses larger than 32 bits. Bootloader is placed at %llX", hdr.partition->section->Address);
    }
    if (!Binary::CheckAddress(hdr.GetPartitionSize()))
    {
        LOG_ERROR("Bootgen does not support addresses larger than 32 bits. Bootloader size is %llX", hdr.GetPartitionSize());
    }
    if (!Binary::CheckAddress(hdr.GetLoadAddress()))
    {
        LOG_ERROR("Bootgen does not support addresses larger than 32 bits. Bootloader load address is %llX", hdr.GetLoadAddress());
    }
    if (!Binary::CheckAddress(hdr.GetExecAddress()))
    {
        LOG_ERROR("Bootgen does not support addresses larger than 32 bits. Bootloader exec address is %llX", hdr.GetExecAddress());
    }
    if (!Binary::CheckAddress(hdr.GetTransferSize()))
    {
        LOG_ERROR("Bootgen does not support addresses larger than 32 bits. Bootloader transfer size is %llX", hdr.GetTransferSize());
    }
}

/******************************************************************************/
void BootHeader::Copy(BootHeader* otherHeader) 
{
    if (prebuilt) 
    {
        LOG_ERROR("Cannot copy bootheader, since it has already been imported");
    }

    /* The BH Section Length can vary due to PUF Helper Data */ 
    if (section->Length < otherHeader->section->Length)
    {
        uint32_t newBhSize = otherHeader->section->Length;
        uint8_t* newDataPtr = new uint8_t[newBhSize];
                
        delete[] section->Data;
        section->Data = newDataPtr;
        section->Length = newBhSize;
    }

    memcpy(section->Data, otherHeader->section->Data,section->Length);
    prebuilt = true;
}

/******************************************************************************/
void BootHeader::LoadUdfData(const std::string& udfFilename, uint8_t* udfData, uint32_t size)
{
    FILE* fPtr;
    int datum, ret;
    uint32_t nbytes = 0;
    
    std::string filename(udfFilename);
    std::string baseFile = StringUtils::BaseName(filename);

    fPtr = fopen(filename.c_str(),"r");
    
    if (fPtr) 
    {
        while ((ret = fscanf(fPtr, "%2X", &datum)) != EOF) 
        {
            if (ret == 1)
            {
                if (datum < 0 || datum > 255)
                {
                    LOG_ERROR("Bad hex conversion in %s", baseFile.c_str());
                }
                if (nbytes == size*4)
                {
                    LOG_ERROR("More than %d bytes in the Boot Header UDF Data file - %s is not allowed.", size*4, baseFile.c_str());
                }
                udfData[nbytes++] = datum;
            }
            else
            {
                LOG_ERROR("Error reading hex characters from file - %s ", baseFile.c_str());
            }
        }
    } 
    else
    {
        LOG_ERROR("Failure to open boot header udf file - %s ", baseFile.c_str());
    }
    fclose(fPtr);
}

/******************************************************************************/
bool BootHeader::GetPrebuiltFlag(void)
{
    return prebuilt;
}
