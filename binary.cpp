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
#include <iostream>
#include <iomanip>
#include <string.h>

#include "binary.h"
#include "bootgenexception.h"
#include "options.h"
#include "stringutils.h"
#include "bifoptions.h"

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
Binary::~Binary()
{
    for(SectionList::iterator i=Sections.begin(); i!=Sections.end(); i++) 
    {
           delete *i;
    }
    Sections.clear();
}

/******************************************************************************/
void Binary::StackAndAlign(Options& options)
{
    std::string logmsg;
    Binary::Address_t runningAddress = options.GetBaseAddress();
    Binary::Address_t reservedAddress = 0;

    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        Section& sec(**i);

        if (!sec.continuation)
        {
            if (runningAddress < reservedAddress)
            {
                runningAddress = reservedAddress;
            }
        }
        if (sec.isBootloader)
        {
            options.totalHeadersSize = runningAddress;
            options.allHeaderSize = options.totalHeadersSize - options.bootheaderSize;
            options.bootloaderSize = runningAddress;
        }

        if (sec.Length == 0)
        {
            LOG_WARNING("Section %s length is zero");
        }

        if (sec.Address != 0)
        {
            if (runningAddress > sec.Address)
            {
                LOG_DUMP_IMAGE(*this);
                LOG_ERROR("Section %s offset of 0x%X overlaps with prior section end address of %X", sec.Name.c_str(), sec.Address, runningAddress);
            }
        }
        else
        {
            if (sec.Alignment)
            {
                runningAddress = (runningAddress + sec.Alignment - 1) & ~(sec.Alignment - 1);
            }
            sec.Address = runningAddress;
        }

        if (sec.Reserve != 0)
        {
            reservedAddress = sec.Address + sec.Reserve;
        }
        runningAddress = sec.Address + sec.Length;
        runningAddress = (runningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1); // align if necessary

        if (!sec.Name.compare("BootHeader"))
        {
            options.bootheaderSize = runningAddress;
        }

        if ((!StringUtils::GetExtension(sec.Name).compare(".sha384") || !StringUtils::GetExtension(sec.Name).compare(".sha256")) && sec.Name.find(StringUtils::BaseName(options.GetFsblFilename())) != std::string::npos)
        {
            options.bootloaderSize += sec.Length;
        }

        if (sec.isBootloader)
        {
            options.bootloaderSize = runningAddress - options.bootloaderSize;
        }
    }
    TotalSize = runningAddress;
}

/******************************************************************************/
Section::Section(const std::string& name, Binary::Length_t length)
    :  Name(name)
    ,  Address(0)
    ,  Length(length)
    ,  Alignment(0)
    ,  Reserve(0)
    ,  continuation(false)
    ,  isPartitionData(false)
    ,  isCertificate(false)
    ,  isBitStream(false)
    ,  isBootloader(false)
    ,  isPmcdata(false)
    ,  isFirstElfSection(false) 
    ,  index(0)
    , firstChunkSize(0)
{
    std::string logmsg;
    Data = new uint8_t[length];
    if (Data == 0) 
    {
        LOG_ERROR("Memory allocation error, size = %d" , length);
    }
}

/******************************************************************************/
Section::~Section() 
{
    delete[] Data;
}

/******************************************************************************/
void Section::PadToWordAlignment(uint32_t headPad, uint32_t tailPad)
{
    if (headPad == 0 && tailPad == 0) 
    {
        return;
    }

    /* The aligned byte size will be anywhere from 0 to 6 bytes longer than the original section data. */
    Binary::Length_t newLength = Length + headPad + tailPad;
    uint8_t* newData = new uint8_t[newLength];
    
    /* Pad the head */
    memset(newData, 0, headPad);

    /* Move the bulk */
    memmove(newData + headPad, Data, Length );

    /* Pad the tail */
    memset(newData + Length + headPad, 0, tailPad); 
    
    delete[] Data;
    Data = newData;
    Length = newLength;
}

/******************************************************************************/
void Section::IncreaseLengthAndPadTo(Binary::Length_t newLength, uint8_t padding)
{
    if (newLength < Length)
    {
        LOG_DEBUG(DEBUG_STAMP, "Resize length is less than original length");
        LOG_ERROR("Section resize issue for authentication");
    }
    uint8_t* newDataPtr = new uint8_t[newLength];
    memcpy(newDataPtr,Data,Length);
    memset(newDataPtr+Length,padding,newLength - Length);

    delete[] Data;
    Data = newDataPtr;
    Length = newLength;
}

/******************************************************************************/
void Section::Summarize(std::ostream& o)
{
    o << Name << " : " << std::hex << Length;
    uint32_t checksum = 0;
    for(uint32_t i=0; i<Length; i++) 
    {
        checksum += Data[i];
    }
    o << " checksum:" << checksum << std::endl;
}

/******************************************************************************/
void Section::IncreaseLengthAndPadSHA3(Binary::Length_t newLength)
{
    if (newLength < Length)
    {
        LOG_DEBUG(DEBUG_STAMP, "Resize length is less than original length");
        LOG_ERROR("Section resize issue for authentication..");
    }

    uint8_t* newDataPtr = new uint8_t[newLength];
    memset(newDataPtr, 0, newLength);
    memcpy(newDataPtr, Data, Length);
    newDataPtr[Length] = 0x6;
    newDataPtr[newLength - 1] |= 0x80;

    delete[] Data;
    Data = newDataPtr;
    Length = newLength;
}
