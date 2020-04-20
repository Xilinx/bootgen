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
#include "stringutils.h"
#include "bootgenexception.h"
#include "binary.h"
#include "options.h"
#include "bootgenenum.h"
#include "bootimage.h"


/******************************************************************************/
void OutputFile::OutputVersal(Options& options, Binary& cache)
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
    std::string       acBootloader = "";
    Binary::Address_t unalignedRunningAddress = 0;
    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section& section(**i);
        //ProcessEverestImage(section, options);
        /* Write to output file */
        if (!section.Name.compare("BootHeader"))
        {
            if (options.bifOptions->GetSmapWidth() == 0)
            {
                // For SSIT slave devices, the PDI should not have the SMAP bus width
                adjust_len = 16;
            }
            Write(section.Address, section.Length - adjust_len, section.Data + adjust_len);
            if (options.GetDumpOption() == DumpOption::BH)
            {
                WriteBootHeaderToFile(options.GetOutputFileNames().front(), section.Data + adjust_len, section.Length - adjust_len);
            }
            unalignedRunningAddress = section.Address + section.Length - adjust_len;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
        }
        if (section.isBootloader)
        {
            acBootloader = section.Name + ".sha384";
        }
    }

    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section& section(**i);
        if (!section.Name.compare(acBootloader)) {
            section.Address = runningAddress;
            Write(section.Address, section.Length, section.Data);
            unalignedRunningAddress = section.Address + section.Length;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
        }
    }

    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section& section(**i);
        if (section.isBootloader)
        {
            section.Address = runningAddress;
            if ((mode == OutputMode::OUT_SPLIT_NORMAL) || splitMode)
            {
                ProcessSplitMode(section, options);
            }

            Write(section.Address, section.Length, section.Data);
            unalignedRunningAddress = section.Address + section.Length;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            /* Fill the gap(if any) between sections will fill byte */
            if (runningAddress > unalignedRunningAddress)
            {
                if (options.bifOptions->GetSmapWidth() != 0)
                {
                    Fill(unalignedRunningAddress, runningAddress, options.GetDoFill(), options.GetOutputFillByte());
                }
            }
        }
    }

    for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section& section(**i);

        /* Write to output file */
        if (section.Name.compare("BootHeader") && !(section.isBootloader) && section.Name.compare(acBootloader))
        {
            if (section.Address < runningAddress)
            {
                LOG_ERROR("Sequential addresses are going backwards!");
            }
            /* Fill the gap(if any) between sections will fill byte */
            if (section.Address > runningAddress)
            {
                if (options.bifOptions->GetSmapWidth() != 0)
                {
                    Fill(runningAddress, section.Address, options.GetDoFill(), options.GetOutputFillByte());
                }
            }

            if ((mode == OutputMode::OUT_SPLIT_NORMAL) || splitMode)
            {
                ProcessSplitMode(section, options);
            }
            Write(section.Address, section.Length, section.Data);
            unalignedRunningAddress = section.Address + section.Length;
            runningAddress = (unalignedRunningAddress + (options.GetDefaultAlignment() - 1)) & ~(options.GetDefaultAlignment() - 1);
            /* Fill the gap(if any) between sections will fill byte */
            if (runningAddress > unalignedRunningAddress)
            {
                if (options.bifOptions->GetSmapWidth() != 0)
                {
                    Fill(unalignedRunningAddress, runningAddress, options.GetDoFill(), options.GetOutputFillByte());
                }
            }
        }
    }
    /* Close the file handle, after writing the post script */
    WritePostscriptAndClose();
}
