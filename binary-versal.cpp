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

#include "binary-versal.h"
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
VersalBinary::~VersalBinary()
{
    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        delete *i;
    }
    Sections.clear();
}

/******************************************************************************/
void VersalBinary::StackAndAlign(Options& options)
{
    std::string logmsg;
    Binary::Address_t runningAddress = options.GetBaseAddress();
    Binary::Address_t reservedAddress = 0;
    Binary::Address_t unalignedRunningAddress = 0;

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
            if (options.totalHeadersSize == 0) {
                options.totalHeadersSize = runningAddress;
            }
            if (options.allHeaderSize == 0) {
                options.allHeaderSize = options.totalHeadersSize - options.bootheaderSize;
            }
            if (options.bootloaderSize == 0) {
                options.bootloaderSize = runningAddress;
            }
            else {
                options.bootloaderSize += sec.Length;
            }

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
            options.totalHeadersSize = runningAddress;
            options.allHeaderSize = options.totalHeadersSize - options.bootheaderSize;
            options.bootloaderSize = runningAddress;

        }

        if (sec.isBootloader)
        {
            options.bootloaderSize = runningAddress - options.bootloaderSize;
        }
    }

    if (options.bifOptions->GetHeaderAC())
    {
        /* Substract the size of AC, since AC sits infront of headers */
        options.totalHeadersSize -= VERSAL_AC_SIZE;
        options.allHeaderSize -= VERSAL_AC_SIZE;
        options.bootloaderSize -= VERSAL_AC_SIZE;
    }

    runningAddress = options.GetBaseAddress();
    //Binary::Address_t unalignedRunningAddress = 0;
    std::string       acBootloader = "";
    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        Section& section(**i);

        if (!section.Name.compare("BootHeader"))
        {
            unalignedRunningAddress = section.Address + section.Length;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
        }
        if (section.isBootloader)
        {
            acBootloader = section.Name + ".sha384";
        }
    }

    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        Section& section(**i);
        if (!section.Name.compare(acBootloader))
        {
            section.Address = runningAddress;
            runningAddress = section.Address + section.Length;
            //unalignedRunningAddress = section.Address + section.Length;
            //runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
        }
    }

    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        Section& section(**i);
        if (section.isBootloader)
        {
            section.Address = runningAddress;
            unalignedRunningAddress = section.Address + section.Length;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            if (section.Reserve != 0)
            {
                runningAddress = section.Address + section.Length;
            }
        }
    }

    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        Section& section(**i);
        /* Write to output file */
        if (section.Name.compare("BootHeader") && !(section.isBootloader) && section.Name.compare(acBootloader))
        {
            if (section.Address < runningAddress)
            {
                section.Address = runningAddress;
            }

            unalignedRunningAddress = section.Address + section.Length;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            if (section.Reserve != 0)
            {
                runningAddress = section.Address + section.Reserve;
            }
        }
    }
    TotalSize = runningAddress;
}
