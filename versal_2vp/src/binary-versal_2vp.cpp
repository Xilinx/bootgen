/******************************************************************************
 * Copyright 2015-2022 Xilinx, Inc.
 * Copyright 2022-2023 Advanced Micro Devices, Inc.
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
#include <set>

#include "binary-versal_2vp.h"
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
Versal_2vpBinary::~Versal_2vpBinary()
{
}

/******************************************************************************/
void Versal_2vpBinary::StackAndAlign(Options &options)
{
    std::string logmsg;
    Binary::Address_t runningAddress = options.GetBaseAddress();
    Binary::Address_t reservedAddress = 0;
    Binary::Address_t unalignedRunningAddress = 0;
    std::string acBootloader_sha256 = "";
    std::string acBootloader_shake256 = "";
    std::string acBootloader_sha384 = "";

    LOG_INFO("versal_2vp: Using device-specific StackAndAlign logic");

    // First, identify the bootloader sections dynamically (not hardcoded to plm.elf)
    std::string bootloaderName = "";
    std::string bootloaderAuthSection = "";
    std::string bootloaderHashBlockSection = "";

    // Find the bootloader section first
    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        Section &sec(**i);
        if (sec.isBootloader)
        {
            bootloaderName = sec.Name;
            bootloaderAuthSection = sec.Name + ".sha384"; // Try sha384 first
            bootloaderHashBlockSection = sec.Name + ".HashBlock";
            break;
        }
    }

    // Find the actual auth section by checking all possible hash types
    std::vector<std::string> hashTypes = {".sha384", ".sha512", ".sha256", ".shake256"};
    for (const std::string &hashType : hashTypes)
    {
        std::string candidateAuth = bootloaderName + hashType;
        for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
        {
            Section &sec(**i);
            if (sec.Name == candidateAuth)
            {
                bootloaderAuthSection = candidateAuth;
                break;
            }
        }
        if (!bootloaderAuthSection.empty() && bootloaderAuthSection != bootloaderName + ".sha384")
        {
            break; // Found the actual auth section
        }
    }

    // Check for hybrid authentication (second certificate)
    std::string bootloaderAuth2Section = "";
    for (const std::string &hashType : hashTypes)
    {
        std::string candidateAuth2 = bootloaderName + ".auth2" + hashType;
        for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
        {
            Section &sec(**i);
            if (sec.Name == candidateAuth2)
            {
                bootloaderAuth2Section = candidateAuth2;
                break;
            }
        }
        if (!bootloaderAuth2Section.empty())
        {
            break; // Found the second auth section
        }
    }

    if (!bootloaderAuth2Section.empty())
    {
        LOG_INFO("versal_2vp: Identified bootloader sections (HYBRID) - Main: %s, Auth1: %s, Auth2: %s, HashBlock: %s",
                 bootloaderName.c_str(), bootloaderAuthSection.c_str(), bootloaderAuth2Section.c_str(), bootloaderHashBlockSection.c_str());
    }
    else
    {
    LOG_INFO("versal_2vp: Identified bootloader sections - Main: %s, Auth: %s, HashBlock: %s",
             bootloaderName.c_str(), bootloaderAuthSection.c_str(), bootloaderHashBlockSection.c_str());
    }

    // Step 1: Process BootHeader and special sections first to get bootheaderSize
    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        Section &sec(**i);

        // Only process BootHeader and related sections in this pass
        if (sec.Name != "BootHeader")
        {
            continue;
        }

        if (!sec.continuation)
        {
            if (runningAddress < reservedAddress)
            {
                runningAddress = reservedAddress;
            }
        }

        if (sec.Length == 0)
        {
            LOG_WARNING("Section %s length is zero", sec.Name.c_str());
        }

        if (sec.Address != 0)
        {
            if (runningAddress > sec.Address)
            {
                LOG_DUMP_IMAGE(*this);
                LOG_ERROR("Section %s offset of 0x%X overlaps with prior section end address of 0x%X", sec.Name.c_str(), sec.Address, runningAddress);
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
        runningAddress = (runningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);

        if (!sec.Name.compare("BootHeader"))
        {
            options.bootheaderSize = runningAddress;
        }
    }

    // Step 2: Handle bootloader sections sequentially starting at BootHeader size
    Binary::Address_t bootloaderAddress = options.bootheaderSize;

    LOG_INFO("versal_2vp: Processing bootloader sections sequentially starting at 0x%08X", bootloaderAddress);

    // Process in the required order: HashBlock, Auth1, Auth2, Main
    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        Section &sec(**i);
        if (sec.Name == bootloaderHashBlockSection)
        {
            if (sec.Alignment)
            {
                bootloaderAddress = (bootloaderAddress + sec.Alignment - 1) & ~(sec.Alignment - 1);
            }
            sec.Address = bootloaderAddress;
            bootloaderAddress += sec.Length;
            bootloaderAddress = (bootloaderAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            LOG_INFO("versal_2vp: Set %s at 0x%08X (length 0x%X)", sec.Name.c_str(), sec.Address, sec.Length);
            break;
        }
    }

    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        Section &sec(**i);
        if (sec.Name == bootloaderAuthSection)
        {
            if (sec.Alignment)
            {
                bootloaderAddress = (bootloaderAddress + sec.Alignment - 1) & ~(sec.Alignment - 1);
            }
            sec.Address = bootloaderAddress;
            bootloaderAddress += sec.Length;
            bootloaderAddress = (bootloaderAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            LOG_INFO("versal_2vp: Set %s at 0x%08X (length 0x%X)", sec.Name.c_str(), sec.Address, sec.Length);
            break;
        }
    }

    // Handle second authentication certificate for hybrid authentication
    if (!bootloaderAuth2Section.empty())
    {
        for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
        {
            Section &sec(**i);
            if (sec.Name == bootloaderAuth2Section)
            {
                // Ensure we're placing this section right after the first auth section
                if (sec.Alignment)
                {
                    bootloaderAddress = (bootloaderAddress + sec.Alignment - 1) & ~(sec.Alignment - 1);
                }
                sec.Address = bootloaderAddress;
                bootloaderAddress += sec.Length;
                bootloaderAddress = (bootloaderAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
                LOG_INFO("versal_2vp: Set %s at 0x%08X (length 0x%X)", sec.Name.c_str(), sec.Address, sec.Length);
                break;
            }
        }
    }

    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        Section &sec(**i);
        if (sec.Name == bootloaderName)
        {
            if (sec.Alignment)
            {
                bootloaderAddress = (bootloaderAddress + sec.Alignment - 1) & ~(sec.Alignment - 1);
            }
            sec.Address = bootloaderAddress;
            bootloaderAddress += sec.Length;
            bootloaderAddress = (bootloaderAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            LOG_INFO("versal_2vp: Set %s at 0x%08X (length 0x%X)", sec.Name.c_str(), sec.Address, sec.Length);
            break;
        }
    }

    // Step 3: Now process all non-bootloader sections starting after bootloader sections
    // Start from a fresh address after all bootloader sections to avoid validation conflicts
    runningAddress = bootloaderAddress;

    for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
    {
        Section &sec(**i);

        // Skip sections already processed
        if (sec.Name == "BootHeader" ||
            sec.Name == bootloaderHashBlockSection ||
            sec.Name == bootloaderAuthSection ||
            sec.Name == bootloaderAuth2Section ||
            sec.Name == bootloaderName)
        {
            continue;
        }

        if (!sec.continuation)
        {
            if (runningAddress < reservedAddress)
            {
                runningAddress = reservedAddress;
            }
        }

        if (sec.isBootloader)
        {
            if (options.totalHeadersSize == 0)
            {
                options.totalHeadersSize = runningAddress;
            }
            if (options.allHeaderSize == 0)
            {
                options.allHeaderSize = options.totalHeadersSize - options.bootheaderSize;
            }
            if (options.bootloaderSize == 0)
            {
                options.bootloaderSize = runningAddress;
            }
            else
            {
                options.bootloaderSize += sec.Length;
            }
            acBootloader_sha256 = sec.Name + ".sha256";
            acBootloader_shake256 = sec.Name + ".shake256";
            acBootloader_sha384 = sec.Name + ".sha384";
        }

        if (sec.Length == 0)
        {
            LOG_WARNING("Section %s length is zero", sec.Name.c_str());
        }

        // Special handling for versal_2vp bootloader sections to avoid validation conflicts
        bool isVersal2vpNativeBootloaderSection = (sec.Name == bootloaderHashBlockSection ||
                                       sec.Name == bootloaderAuthSection ||
                                       sec.Name == bootloaderAuth2Section ||
                                       sec.Name == bootloaderName);

        if (sec.Address != 0)
        {
            // Skip overlap validation for bootloader sections as they use special layout
            if (!isVersal2vpNativeBootloaderSection && runningAddress > sec.Address)
            {
                LOG_DUMP_IMAGE(*this);
                LOG_ERROR("Section %s offset of 0x%X overlaps with prior section end address of 0x%X", sec.Name.c_str(), sec.Address, runningAddress);
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
        runningAddress = (runningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);

        if ((!StringUtils::GetExtension(sec.Name).compare(".sha384") || !StringUtils::GetExtension(sec.Name).compare(".sha256") || !StringUtils::GetExtension(sec.Name).compare(".shake256") || !StringUtils::GetExtension(sec.Name).compare(".sha512")) && sec.Name.find(StringUtils::BaseName(options.GetFsblFilename())) != std::string::npos)
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

    // Set the required options for versal_2vp bootloader handling
    if (options.totalHeadersSize == 0)
    {
        options.totalHeadersSize = runningAddress;
    }
    if (options.allHeaderSize == 0)
    {
        options.allHeaderSize = options.totalHeadersSize - options.bootheaderSize;
    }
    if (options.bootloaderSize == 0)
    {
        // For versal_2vp, bootloaderSize should be from start to end of all bootloader sections
        for (SectionList::iterator i = Sections.begin(); i != Sections.end(); i++)
        {
            Section &sec(**i);
            if (sec.Name == bootloaderName)
            {
                options.bootloaderSize = sec.Address + sec.Length;
                break;
            }
        }
    }

    TotalSize = runningAddress;

    if (options.bifOptions->GetHeaderAC())
    {
        options.totalHeadersSize -= VERSAL_AC_SIZE;
        options.allHeaderSize -= VERSAL_AC_SIZE;
        options.bootloaderSize -= VERSAL_AC_SIZE;
    }
}
