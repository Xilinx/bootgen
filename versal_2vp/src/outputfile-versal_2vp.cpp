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
#include "outputfile.h"
#include "stringutils.h"
#include "bootgenexception.h"
#include "binary.h"
#include "options.h"
#include "bootgenenum.h"
#include "bootimage.h"
#include <sstream>
#include <iomanip>

/******************************************************************************/
void OutputFile::OutputVersal_2vp(Options &options, Binary &cache)
{
    bool splitMode = options.GetSplitType() != File::Unknown;

    /* Setup the options for Dual QSPI/OSPI mode */
    qspiDualMode = options.GetDualQspiMode();
    if ((qspiDualMode == QspiMode::PARALLEL_LQSPI) && options.GetArchType() == Arch::VERSAL)
    {
        qspiDualMode = QspiMode::PARALLEL_GQSPI;
    }
    qspiSizeInBytes = options.GetQspiSize() * 1024 * 1024;
    totalImageSize = cache.TotalSize;

    bool overwrite = options.DoOverwrite();
    if (!overwrite)
    {
        if (std::ifstream(filename.c_str()))
        {
            LOG_ERROR("Output File %s already exists in the path\n           Use '-w on' option to force overwrite", filename.c_str());
        }
    }
    uint8_t adjust_len = 0;

    /* Open the file handle for the filename set above & write preamble (if any) */
    Open();
    WritePreamble();
    Binary::Address_t runningAddress = 0;
    std::string acBootloader_sha256 = "";
    std::string acBootloader_shake256 = "";
    std::string acBootloader_sha384 = "";
    // std::string       acBootloader = "";
    Binary::Address_t unalignedRunningAddress = 0;
    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section &section(**i);
        // ProcessEverestImage(section, options);
        /* Write to output file */
        if (!section.Name.compare("BootHeader"))
        {
            if (options.bifOptions->pdiType != PartitionType::SLR_SLAVE_BOOT)
            {
                if (options.bifOptions->GetSmapWidth() == 0)
                {
                    // For SSIT slave devices, the PDI should not have the SMAP bus width
                    adjust_len = 16;
                }
            }
            Write(section.Address, section.Length - adjust_len, section.Data.get() + adjust_len);
            if (options.GetDumpOption() == DumpOption::BH)
            {
                WriteBootHeaderToFile(options.GetOutputFileNames().front(), section.Data.get() + adjust_len, section.Length - adjust_len);
            }
            unalignedRunningAddress = section.Address + section.Length - adjust_len;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
        }
        if (section.isBootloader)
        {
            acBootloader_sha256 = section.Name + ".sha256";
            acBootloader_shake256 = section.Name + ".shake256";
            acBootloader_sha384 = section.Name + ".sha384";
            // acBootloader = section.Name + ".sha384";
        }
    }

    bool isVersal2vpNative = (options.GetArchType() == Arch::VERSAL_2VP);

    // For versal_2vp devices, identify bootloader sections dynamically
    std::string bootloaderName = "";
    std::string bootloaderAuthSection = "";
    std::string bootloaderHashBlockSection = "";

    // Find the bootloader section first
    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section &section(**i);
        if (section.isBootloader)
        {
            bootloaderName = section.Name;
            bootloaderHashBlockSection = section.Name + ".HashBlock";
            break;
        }
    }

    // Find the authentication certificate section and check for hybrid authentication
    std::string bootloaderAuth2Section = "";
    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section &section(**i);
        if (section.Name == bootloaderName + ".sha384" ||
            section.Name == bootloaderName + ".sha256" ||
            section.Name == bootloaderName + ".sha512" ||
            section.Name == bootloaderName + ".shake256")
        {
            bootloaderAuthSection = section.Name;
        }
        // Check for second authentication certificate (hybrid authentication)
        else if (section.Name == bootloaderName + ".auth2.sha384" ||
                 section.Name == bootloaderName + ".auth2.sha256" ||
                 section.Name == bootloaderName + ".auth2.sha512" ||
                 section.Name == bootloaderName + ".auth2.shake256")
        {
            bootloaderAuth2Section = section.Name;
        }
    }

    // Write bootloader sections in the correct physical order

    // 1. First write bootloader hash block
    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section &section(**i);
        if (section.Name == bootloaderHashBlockSection)
        {
            Write(section.Address, section.Length, section.Data.get());
            unalignedRunningAddress = section.Address + section.Length;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            break;
        }
    }

    // 2. Then write the authentication certificate section
    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section &section(**i);
        if (section.Name == bootloaderAuthSection)
        {
            Write(section.Address, section.Length, section.Data.get());
            unalignedRunningAddress = section.Address + section.Length;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            break;
        }
    }

    // 2b. Write the second authentication certificate section for hybrid authentication
    if (!bootloaderAuth2Section.empty())
    {
        bool auth2SectionFound = false;
        for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
        {
            Section &section(**i);
            if (section.Name == bootloaderAuth2Section)
            {
                Write(section.Address, section.Length, section.Data.get());
                unalignedRunningAddress = section.Address + section.Length;
                runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
                auth2SectionFound = true;
                break;
            }
        }
        if (!auth2SectionFound)
        {
            LOG_WARNING("Second auth certificate section %s not found", bootloaderAuth2Section.c_str());
        }
    }

    // 3. Then write bootloader main section
    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section &section(**i);
        if (section.Name == bootloaderName)
        {
            Write(section.Address, section.Length, section.Data.get());
            unalignedRunningAddress = section.Address + section.Length;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            break;
        }
    }

    // After bootloader sections, reset runningAddress to handle high-address sections properly
    // Find the lowest address of remaining sections to set runningAddress correctly
    Binary::Address_t nextSectionAddress = UINT32_MAX;
    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section &section(**i);
        // Skip bootloader sections and BootHeader
        if (section.Name != "BootHeader" && 
            section.Name != bootloaderName &&
            section.Name != bootloaderHashBlockSection &&
            section.Name != bootloaderAuthSection &&
            section.Name != bootloaderAuth2Section &&
            !section.isBootloader)
        {
            if (section.Address < nextSectionAddress)
            {
                nextSectionAddress = section.Address;
            }
        }
    }
    
    if (nextSectionAddress != UINT32_MAX)
    {
        runningAddress = nextSectionAddress;
    }

    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section &section(**i);
        if (section.isBootloader)
        {
            // For versal_2vp devices, skip bootloader sections as they were already written above
            bool isVersal2vpNative = (options.GetArchType() == Arch::VERSAL_2VP);
            if (isVersal2vpNative && (section.Name == bootloaderName ||
                          section.Name == bootloaderHashBlockSection ||
                          section.Name == bootloaderAuthSection ||
                          section.Name == bootloaderAuth2Section))
            {
                continue;
            }

            // For non-native devices, don't override the address - it was already set in StackAndAlign
            if (!isVersal2vpNative)
            {
                section.Address = runningAddress;
            }

            if ((mode == OutputMode::OUT_SPLIT_NORMAL) || splitMode)
            {
                ProcessSplitMode(section, options);
            }
            Write(section.Address, section.Length, section.Data.get());
            unalignedRunningAddress = section.Address + section.Length;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            /* Fill the gap(if any) between sections will fill byte */
            if (runningAddress > unalignedRunningAddress)
            {
                if (options.bifOptionsList.back()->GetSmapWidth() != 0)
                {
                    Fill(unalignedRunningAddress, runningAddress, options.GetDoFill(), options.GetOutputFillByte());
                }
            }
        }
    }

    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section &section(**i);

        /* Write to output file */
        if (section.Name.compare("BootHeader") && !(section.isBootloader) && (section.Name.compare(acBootloader_sha256) && section.Name.compare(acBootloader_shake256) && section.Name.compare(acBootloader_sha384)))
        // if (section.Name.compare("BootHeader") && !(section.isBootloader) && section.Name.compare(acBootloader))
        {
            // For versal_2vp devices, skip bootloader sections as they were already written above
            bool isVersal2vpNative = (options.GetArchType() == Arch::VERSAL_2VP);
            if (isVersal2vpNative && (section.Name == bootloaderName ||
                          section.Name == bootloaderHashBlockSection ||
                          section.Name == bootloaderAuthSection ||
                          section.Name == bootloaderAuth2Section))
            {
                continue;
            }

            if (section.Address < runningAddress)
            {
                LOG_ERROR("Sequential addresses are going backwards!");
            }
            /* Fill the gap(if any) between sections will fill byte */
            if (section.Address > runningAddress)
            {
                if (options.bifOptionsList.back()->GetSmapWidth() != 0)
                {
                    Fill(runningAddress, section.Address, options.GetDoFill(), options.GetOutputFillByte());
                }
            }

            if ((mode == OutputMode::OUT_SPLIT_NORMAL) || splitMode)
            {
                ProcessSplitMode(section, options);
            }

            Write(section.Address, section.Length, section.Data.get());
            unalignedRunningAddress = section.Address + section.Length;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            /* Fill the gap(if any) between sections will fill byte */
            if (runningAddress > unalignedRunningAddress)
            {
                if (options.bifOptionsList.back()->GetSmapWidth() != 0)
                {
                    Fill(unalignedRunningAddress, runningAddress, options.GetDoFill(), options.GetOutputFillByte());
                }
            }
        }
    }

    /* Close the file handle, after writing the post script */
    WritePostscriptAndClose();
}
