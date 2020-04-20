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
#include "bitutils.h"

/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
typedef struct 
{
    const char* extension;
    File::Type type;
} fileFormatTableType;

fileFormatTableType fileFormatTable[] =
{
    { ".mem",  File::MEM },
    { ".bit",  File::BIT },
    { ".rbt",  File::RBT },
    { ".bin",  File::BIN },
    { ".hex",  File::HEX },
    { ".mcs",  File::MCS },
    { NULL  },
};


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
File::Type OutputFile::GetType(const std::string& filename)
{
    std::string extension = StringUtils::GetExtension(filename);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    for(fileFormatTableType* ptr = fileFormatTable;ptr->extension;ptr++) 
    {
        if (ptr->extension == extension)
        {
            return ptr->type;
        }
    }
    return File::BIN;
}

/******************************************************************************/
OutputFile* OutputFile::Factory(const std::string& filename) 
 {
    File::Type type = GetType(filename);
    OutputFile* file = NULL;

    switch(type)
    {
        case File::MCS:
            file = new McsFile();
            break;

        case File::BIN:
            file = new BinFile();
            break;
        
        default:
            LOG_ERROR("File extension (type %d) not supported for %s",type,filename.c_str());
    }
    file->filename = filename;
    return file;
}

/******************************************************************************/
static std::string AddSplitModeExtention(std::string fName, File::Type type, bool isFirstSection) 
{
    std::string extension = (type == File::MCS) ? ".mcs" : ".bin";
    
    /* If there are multiple partitions for same elf partition, then add a no. before the extn
       Example: There are 2 partitions in app.elf, then 2 partition files would be
                app.elf.bin
                app.elf.1.bin */
    if(!isFirstSection)
    {
        std::size_t pos = fName.find("elf");
        if(pos != std::string::npos)
        {	
            std::size_t temp = fName.find_last_of(".");
            extension = fName.substr(temp, temp+1) + extension;
        }
    }
    return fName.substr(0, fName.find_last_of(".")) + extension;
}

/******************************************************************************/
void OutputFile::Output(Options& options, Binary& cache) 
{
    if (options.GetArchType() == Arch::VERSAL)
    {
        OutputVersal(options, cache);
    } 
    else
    {
        /* Get all modes */
        bool splitMode = options.GetSplitType() != File::Unknown;
        //bool processBitstream = options.GetProcessBitstreamType() != File::Unknown;
        mode = options.GetOutputMode();

        /* Setup the options for Dual QSPI mode */
        qspiDualMode = options.GetDualQspiMode();
        if ((qspiDualMode == QspiMode::PARALLEL_LQSPI) && options.GetArchType() == Arch::ZYNQMP)
        {
            qspiDualMode = QspiMode::PARALLEL_GQSPI;
        }
        qspiSizeInBytes = options.GetQspiSize() * 1024 * 1024;
        totalImageSize = cache.TotalSize;

        splitOffset = 0;
        encrypted = false;


        /* Create output files as per mode, depending on overwrite flag */
        CreateOutputFiles(options);

        /* Open the file handle for the filename set above & write preamble (if any) */
        Open();
        WritePreamble();

        /* Iterate through all sections and push the section to approprite file,
        depending on mode */
        Binary::Address_t runningAddress = 0;
        for (SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++)
        {
            Section& section(**i);

            /* Process Bitstream Mode
            Only bitstream files will be output to a file */
            if (mode == OutputMode::OUT_BITSTREAM)
            {
                if (section.isBitStream)
                {
                    ProcessBitstreamMode(section);
                }
                else
                {
                    continue;
                }
            }
            else
            {
                if (section.Address < runningAddress)
                {
                    LOG_ERROR("Sequential addresses are going backwards!");
                }

                /* Fill the gap(if any) between sections will fill byte */
                if (section.Address > runningAddress)
                {
                    Fill(runningAddress, section.Address, options.GetDoFill(), options.GetOutputFillByte());
                }

                /* Split Mode
                All headers and FSBL are part of the main output file and all other partitions have one output file each */
                //if (splitMode) 
                if ((mode == OutputMode::OUT_SPLIT_NORMAL) || splitMode)
                {
                    ProcessSplitMode(section, options);
                }

                /* Slave Boot Split Mode
                Bootheader+FSBL are part of the main output file, headers are part of one output file
                and all other partitions to another output file */
                if (mode == OutputMode::OUT_SPLIT_SLAVEBOOT)
                {
                    ProcessSplitSlaveBootMode(section, options);
                }
            }

            /* Write to output file */
            Write(section.Address, section.Length, section.Data);
            runningAddress = section.Address + section.Length;
        }

        /* Close the file handle, after writing the post script */
        WritePostscriptAndClose();
    }
}

/******************************************************************************/
void OutputFile::CreateOutputFiles(Options& options)
{
    bool overwrite = options.DoOverwrite();
    bootFile = filename;

    /* Slave Boot Split mode
       Create two more files other than original file -  for headers & partitions 
       Files are created if overwrite flag is not set, otherwise they are freshly opened (not in append mode) */
    if(mode == OutputMode::OUT_SPLIT_SLAVEBOOT)
    {
        std::string extn = (options.GetOutputFormat() == File::BIN) ? ".bin": ".mcs";
        std::string basename = StringUtils::RemoveExtension(filename.c_str());

        /* Headers file */
        headersFile = basename + "_headers" + extn;
        if(!overwrite && std::ifstream(headersFile.c_str())) 
        {
            LOG_ERROR("Output File %s already exists in the path\n           Use '-w on' option to force overwrite", headersFile.c_str());
        } 
        filename = headersFile;
        Open();
        WritePreamble();
        Close();

        /* Partitions file */
        partitionFile = basename + "_partitions" + extn;
        if(!overwrite && std::ifstream(partitionFile.c_str())) 
        {
            LOG_ERROR("Output File %s already exists in the path\n           Use '-w on' option to force overwrite", partitionFile.c_str());
        }
        filename = partitionFile;
        Open();
        WritePreamble();
        Close();

        this->filename = bootFile;
    }
    else 
    {
        /* Original output file
        If overwrite flag not enabled, throw error if already exists */
        if(!overwrite)
        {
            if (std::ifstream(filename.c_str())) 
            {
                LOG_ERROR("Output File %s already exists in the path\n           Use '-w on' option to force overwrite", filename.c_str());
            }
        }
    }
}

/******************************************************************************/
void OutputFile::ProcessSplitSlaveBootMode(Section& section, Options& options)
{
    /* If the section is a Bootheader, then ignore, as it will be part of the main output 
       file opened in the caller function */
    if(section.Name.compare("BootHeader"))
    {
        /* Close previous file handle
           Don't write the post script, as this is not the end of file */
        Close();

        /* If section is bootloader OR bootloader's authentication certificate 
           Use the orginal output file name */
        if( section.isBootloader || 
            ((section.Name.find(StringUtils::BaseName(options.GetFsblFilename())) != std::string::npos) && section.isCertificate) )
        {
            splitOffset = options.allHeaderSize;
            fileIndex = 0;
            this->filename = bootFile;
        }

        /* All the header sections except bootheader
           Includes authentication certificate(if any) of the all header tables
           Excludes partitions and their authentication certificates(if any)
           Use the <outputfilename>_headers.<ext> */
        if( (section.Name.compare("BootHeader") && !section.isPartitionData && !section.isCertificate) || 
            ((section.Name.find("ImageHeaderTable") != std::string::npos) && section.isCertificate) )
        {
            splitOffset = options.bootheaderSize;
            fileIndex = 1;
            this->filename = headersFile;
        }

        /* All the other partitions except bootloader/fsbl
           Includes their authentication certificates(if any)
           Excludes header's and bootloaders's authentication certificate(if any)
           Use the <outputfilename>_partitions.<ext> */
        if( !section.isBootloader && 
            (section.isPartitionData || section.isCertificate) &&
            !((section.Name.find("ImageHeaderTable") != std::string::npos) && section.isCertificate) &&
            !((section.Name.find(StringUtils::BaseName(options.GetFsblFilename())) != std::string::npos) && section.isCertificate) )
        {
            splitOffset = options.bootheaderSize + options.bootloaderSize + options.allHeaderSize;
            fileIndex = 2;
            this->filename = partitionFile;
        }
        LOG_TRACE("Adding section %s to %s", section.Name.c_str(), StringUtils::BaseName(this->filename.c_str()).c_str());
        
        /* Opens the file handle in append mode */
        Append();
    }
}

/******************************************************************************/
void OutputFile::ProcessSplitMode(Section& section, Options& options)
{
    /* If the section is a partition and not an FSBL partition */
    if( section.isPartitionData && section.Name.find(StringUtils::BaseName(options.GetFsblFilename())) != 0 )
    {
        /* Close the previous file handle */
        WritePostscript();
        Close();

        /* Create the filename, based on the partition name & extension */
        File::Type extn = options.GetSplitType();
        if(extn == File::Unknown)
        {
            extn = options.GetOutputFormat();
        }
        this->filename = AddSplitModeExtention(section.Name, extn, section.isFirstElfSection);
        
        /* Open the new file handle */
        Open(); 
        WritePreamble();

        LOG_TRACE("File %s created for partition %s in Split Mode", filename.c_str(), section.Name.c_str());
    }
}

/******************************************************************************/
void OutputFile::ProcessBitstreamMode(Section& section)
{
    WritePostscript();
    Close(); 
    Open(); 
    WritePreamble();

    /* The 0 section address, means the bitstream data starts from beginning of the file */
    section.Address = 0;
}

/******************************************************************************/
void OutputFile::WritePostscriptAndClose(void)
{
    WritePostscript();
    Close(); 
 
    /* The above functions will write the post script and close the file handle.
       Incase of Spilt Slave mode, the last file written is always partitions file, hence the 
       above functions will write the post script and close the files. Only bootfile & headers file
       need to handled separately and is done in below block */
    if(mode == OutputMode::OUT_SPLIT_SLAVEBOOT)
    {
        this->filename = bootFile;
        Append();
        WritePostscript();
        Close(); 

        this->filename = headersFile;
        Append();
        WritePostscript();
        Close(); 
    }
}

/******************************************************************************/
void FpgaPartitionOutput::GeneratePartitionFiles(BootImage& bi, Binary& cache)
{
    SectionList tempSecList;
    Binary::Address_t *addr = new Binary::Address_t [bi.imageList.size()];

    int i = 0;
    for(std::list<ImageHeader*>::iterator image = bi.imageList.begin(); image != bi.imageList.end();  image++, i++) 
    {
        addr[i] = (Binary::Address_t)(*image)->GetOffset().ValueOrDefault(0);
    }
    int j = 0;
    for(SectionList::iterator i = cache.Sections.begin(); i != cache.Sections.end(); i++) 
    {
        Section& section(**i);

        if(section.isPartitionData)
        {
            tempSecList.push_back(&section);
            section.Address = addr[j++];
        }
    }
    cache.Sections = tempSecList;
    delete[] addr;
}

/******************************************************************************/
void OutputFile::OutputInterface(Options& options, uint8_t* buffer, uint32_t size)
{
    encrypted = false;
    /* If overwrite flag not enabled, throw error if already exists */
    if (!options.DoOverwrite())
    {
        if (std::ifstream(filename.c_str()))
        {
            LOG_ERROR("Output File %s already exists in the path\n           Use '-w on' option to force overwrite", filename.c_str());
        }
    }

    /* Open the file handle for the filename set above & write preamble (if any) */
    Open();
    WritePreamble();

    Write(0, size, buffer);

    /* Close the file handle, after writing the post script */
    WritePostscriptAndClose();
}

/******************************************************************************/
void OutputFile::WriteBootHeaderToFile(std::string fileName, uint8_t* data, uint64_t size)
{
    FILE* filePtr;
    size_t result;
    std::string fName = StringUtils::RemoveExtension(fileName);
    fName += "_bh.bin";
    filePtr = fopen(fName.c_str(), "wb");
    if (filePtr != NULL)
    {
        /* First 16 bytes are not required by BootRoM/PLM, they should not be part of BH */
        result = fwrite(data + 16, 1, size - 16, filePtr);
        if (result != (size - 16))
        {
            LOG_ERROR("Error dumping Boot Header to a file");
        }
        fclose(filePtr);
    }
    LOG_INFO("Boot header binary file created - %s", fName.c_str());
}
