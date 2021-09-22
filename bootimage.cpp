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
#include <fstream>
#include <algorithm>
#include <string>

#include "bootimage.h"
#include "bootimage-zynq.h"
#include "bootimage-zynqmp.h"
#include "bootimage-versal.h"
#include "bifscanner.h"
#include "bootheader.h"
#include "options.h"
#include "bootgenexception.h"
#include "outputfile.h"
#include "binary.h"
#include "stringutils.h"
#include "checksum.h"
#include "logger.h"
#include "bitutils.h"

/*
------------------------------------------------------------------------------
****************************************************   F U N C T I O N S   ***
------------------------------------------------------------------------------
*/

/******************************************************************************/
void BIF_File::Process(Options& options) 
{
    std::string basefile = StringUtils::BaseName(biffilename);
    
    LOG_TRACE("BIF file parsing started");
    
    BIF::FlexScanner scanner;
    BIF::BisonParser parser(scanner, options);
   
    scanner.filename = biffilename;
    std::ifstream s(biffilename.c_str());
    
    if (!s) 
    {
        LOG_ERROR("Cannot read BIF file - %s ", basefile.c_str());
    }
    if(s.peek() == EOF)
    {
        LOG_ERROR("BIF file is empty - %s ", basefile.c_str());
    }
    scanner.switch_streams(&s);
    int res = parser.parse();
    if (res)
    {
        LOG_ERROR("BIF file parsing failed with code %d", res);
    }

    LOG_INFO("BIF file parsing completed successfully");

    //bifOptionList.push_back(options.bifOptions);

    bifOptionList = options.bifOptionsList;
    BootImage* currentbi = NULL;
    uint8_t index = 0;
    for (std::vector<BifOptions*>::iterator bifoptions = bifOptionList.begin(); bifoptions != bifOptionList.end();bifoptions++)
    {
        if (options.archType == Arch::ZYNQMP)
        {
            currentbi = new ZynqMpBootImage(options, index);
        }
        else if (options.archType == Arch::VERSAL)
        {
            currentbi = new VersalBootImage(options, index);
        }
        else
        {
            currentbi = new ZynqBootImage(options, index);
        }

        if (currentbi != NULL)
        {
            currentbi->Add(*bifoptions);
            bootImages.push_back(currentbi);
        }
        Output(options, index);
        index++;
    }
}

/******************************************************************************/
void BIF_File::Output(Options& options, uint8_t index)
{
    File::Type splitFileType = options.GetSplitType();
    std::list<std::string> outFilename = options.GetOutputFileNames();
    BootImage* bi = bootImages.at(index);
    {
        if (options.GetAuthKeyGeneration() != GenAuthKeys::None)
        {
            bi->GenerateAuthenticationKeys();
            return;
        }

        if (options.GetGreyKeyGeneration())
        {
            bi->GenerateGreyKey();
            return;
        }
        
        if (options.GetMetalKeyGeneration())
        {
            LOG_ERROR("The support to generate Family Key is deprecated.");
        }

        LOG_INFO("Building image - %s", bi->Name.c_str());
        bi->BuildAndLink(bi->cache);

        bi->SetOutputSplitModeFormat(options.bifOptions->GetSplitMode(), options.bifOptions->GetSplitFormat());
        bi->SetOutputBitstreamModeFormat(options.GetProcessBitstreamType());

        OutputMode::Type outputMode = options.GetOutputMode();
        bi->ValidateOutputModes(splitFileType, outputMode);

        if (outputMode == OutputMode::OUT_SPLIT_NORMAL || outputMode == OutputMode::OUT_SPLIT_SLAVEBOOT || splitFileType != File::Unknown)
        {
            outFilename.clear();
            std::string fName = StringUtils::RemoveExtension(biffilename);
            fName = fName.substr(fName.find_last_of("\\")+1, std::string::npos);
            if(splitFileType == File::Unknown)
            {
                splitFileType = options.GetOutputFormat();
            }
            fName += (splitFileType == File::MCS) ? ".mcs" : ".bin";
            outFilename.push_back(fName);
        }

        /* Process Bitstream Mode
        Create a file with the bitstream filename, add proper extension */
        if (outputMode == OutputMode::OUT_BITSTREAM) 
        {
            outFilename.clear();
            std::string fName = bi->bitFilename;
            if(bi->bitFilename.empty())
            {
                LOG_ERROR("No bitstream in the BIF file - %s ",options.GetBifFilename().c_str());
            }
            fName += (options.GetProcessBitstreamType() == File::MCS) ? ".mcs" : ".bin";
            outFilename.push_back(fName);
        }

        bi->OutputPartitionFiles(options, *bi->cache);

        /* Create a BIN/MCS file format from the factory 
           Write to the file outputs based on the format */
        for(std::list<std::string>::iterator filename = outFilename.begin();filename != outFilename.end(); filename++) 
        {
            std::string out_filename;
            if (index == (options.bifOptionsList.size() - 1))
            {
                out_filename = *filename;
            }
            else
            {
                out_filename = StringUtils::RemoveExtension(*filename) + "_" + bi->Name + StringUtils::GetExtension(*filename);
            }
            OutputFile* file = OutputFile::Factory(out_filename);
            file->Output(options, *bi->cache);
            delete file;
        }

        /* Efuse PPK hash for burning the efuse */
        bi->OutputOptionalEfuseHash();

        /* Secure Debug Image to enable the JTAG during a secure boot scenario */
        bi->OutputOptionalSecureDebugImage();
    }
}

/******************************************************************************/
BootImage::BootImage(Options& options, uint8_t index)
    : options(options)
    , headerSignature_Loaded(false)
    , headerAC(0)
    , XipMode(false)
    , nullPartHeaderSection(NULL)
    , core(Core::A53Singlex64)
    , legacyEncryptionEnabled(true)
    , hash(NULL)
    , totalHeadersSize(0)
    , currentChecksumCtx(NULL)
    , pmuFwSize(0)
    , fsblFwSize(0)
    , totalPmuFwSize(0)
    , totalFsblFwSize(0)
    , sourceAddr(0)
    , bootloaderFound(false)
    , bootloaderAuthenticate(false)
    , bootloaderEncrypt(false)
    , bootloaderKeySource(KeySource::None)
    , keygen(NULL)
    , partCount(0)
    , authHash(AuthHash::Sha3)
    , assumeEncryption(false)
    , convertAieElfToCdo(false)
    , createSubSystemPdis(false)
    , encryptedHeaders(NULL)
    , bootHeader(NULL)
    , imageHeaderTable(NULL)
    , partitionHeaderTable(NULL)
    , currentAuthCtx(NULL)
    , currentEncryptCtx(NULL)
    , partitionOutput(NULL)
    , deviceKey(NULL)
    , firstIv(NULL)
    , firstOptKey(NULL)
    , overlayCDO(NULL)
{
    bifOptions = options.bifOptionsList.at(index);
    Name = bifOptions->GetGroupName();
    cache = new Binary();
    checksumTable = new ChecksumTable();
}

/******************************************************************************/
BootImage::~BootImage()
{
    if(currentAuthCtx)
    {
        delete currentAuthCtx;
    }
    if(currentEncryptCtx)
    {
        delete currentEncryptCtx;
    }
    if(currentChecksumCtx)
    {
        delete currentChecksumCtx;
    }
    if (checksumTable)
    {
        delete checksumTable;
    }
    if (cache)
    {
        delete cache;
    }
    if (bootHeader)
    {
        delete bootHeader;
    }
    if (imageHeaderTable)
    {
        delete imageHeaderTable;
    }
    if (partitionHeaderTable)
    {
        delete partitionHeaderTable;
    }
    if (partitionOutput)
    {
        delete partitionOutput;
    }
    if (deviceKey)
    {
        delete deviceKey;
    }
    if (firstIv)
    {
        delete firstIv;
    }
    if (firstOptKey)
    {
        delete firstOptKey;
    }
    if (keygen)
    {
        delete keygen;
    }
    if (hash)
    {
        delete hash;
    }
    if (nullPartHeaderSection)
    {
        delete nullPartHeaderSection;
    }
}

/******************************************************************************/
void BootImage::OutputOptionalEfuseHash()
{
    std::string hashFile = options.GetEfuseHashFileName();
    if (hashFile != "")
    {
        if (bifOptions->GetPPKFileName() != "" || bifOptions->GetPSKFileName() != "")
        {
            if (currentAuthCtx)
            {
                currentAuthCtx->GeneratePPKHash(hashFile);
            }
        }
        else
        {
            LOG_ERROR("Cannot read PPK/PSK. PPK/PSK is mandatory to generate PPK hash bits, using '-efuseppkbits'.");
        }
    }
}

/******************************************************************************/
void BootImage::BuildAndLink(Binary* cache) 
{  
    if( imageList.size() == 0 ) 
    {
        LOG_WARNING("No partition images given");
    }
    
    currentAuthCtx->SetPpkSelect(bifOptions->GetPpkSelection());
    currentAuthCtx->SetSpkSelect(bifOptions->GetSpkSelection());
    currentAuthCtx->SetSpkIdentification(bifOptions->GetSpkId());

    DetermineEncryptionDefaults();

    partitionHeaderList.clear();

    /* Build stage */
    /* all static fields within the header tables are populated here */
    bootHeader->Build(*this,*cache);
    imageHeaderTable->Build(*this,*cache);
    partitionHeaderTable->Build(*this,*cache);
    partitionHeaderTable->BuildPartitions(*this,*cache);
    checksumTable->Build(*this,*cache);


    LOG_INFO("After build ");
    LOG_DUMP_IMAGE(*cache);

    /* Stack and alignment stage */
    /* Once the header tables are created, stack all the tables and do the necessary alignment */
    cache->StackAndAlign(options);

    LOG_INFO("After align ");
    LOG_DUMP_IMAGE(*cache);

    PrintPartitionInformation();

    /* Link stage - fields which depend on partitions are populated here */
    bootHeader->Link(*this);
    imageHeaderTable->Link(*this);
    partitionHeaderTable->Link(*this);
    partitionHeaderTable->LinkPartitions(*this);
    checksumTable->Link(*this);

    LOG_INFO("After Link ");
    LOG_DUMP_IMAGE(*cache);
}

/******************************************************************************/
void BootImage::PrintPartitionInformation(void)
{
    LOG_INFO("Partition Information: ");
    std::string lastImageName;
    for (std::list<PartitionHeader*>::iterator pH = partitionHeaderList.begin(); pH != partitionHeaderList.end(); pH++)
    {
        static uint8_t index = 0;
        if ((lastImageName.compare((*pH)->imageHeader->GetName())) != 0)
        {
            LOG_INFO("Image: %s", (*pH)->imageHeader->GetName().c_str());
            lastImageName = (*pH)->imageHeader->GetName();
        }
        LOG_INFO("       Partition %d: %s,  Size: %d", index++, (*pH)->partition->section->Name.c_str(), (*pH)->GetPartitionSize());
    }
}

/******************************************************************************/
void BootImage::ValidateOutputModes(File::Type split, OutputMode::Type outMode)
{
    if(split != File::Unknown && outMode == OutputMode::OUT_SPLIT_SLAVEBOOT)
    {
        LOG_ERROR("Split & Split-Slave modes are mutually exclusive, cannot enable both modes");
    }

    if(outMode == OutputMode::OUT_BITSTREAM && (outMode == OutputMode::OUT_SPLIT_SLAVEBOOT || outMode == OutputMode::OUT_SPLIT_NORMAL))
    {
        LOG_ERROR("Process Bitstream & Split modes are mutually exclusive, cannot enable both modes");
    }

    if(split != File::Unknown && outMode == OutputMode::OUT_BITSTREAM)
    {
        LOG_ERROR("Process Bitstream & Split mode are mutually exclusive, cannot enable both modes");
    }
}

/******************************************************************************/
void BootImage::OutputPartitionFiles(Options& options, Binary& cache)
{
    partitionOutput->GeneratePartitionFiles(*this, cache);
}

/******************************************************************************/
void BootImage::SetLegacyEncryptionFlag(bool flag)
{
    legacyEncryptionEnabled = flag;
}

/******************************************************************************/
AuthHash::Type BootImage::GetAuthHashAlgo(void)
{
    return authHash;
}

/******************************************************************************/
Core::Type BootImage::GetCore(void)
{
    return core;
}

/******************************************************************************/
uint32_t BootImage::GetPmuFwSize(void)
{
    return pmuFwSize;
}

/******************************************************************************/
uint32_t BootImage::GetFsblFwSize(void)
{
    return fsblFwSize;
}

/******************************************************************************/
uint32_t BootImage::GetTotalPmuFwSize(void)
{
    return totalPmuFwSize;
}

/******************************************************************************/
uint32_t BootImage::GetTotalFsblFwSize(void)
{
    return totalFsblFwSize;
}

/******************************************************************************/
Binary::Address_t BootImage::GetFsblSourceAddr(void)
{
    return sourceAddr;
}

/******************************************************************************/
bool BootImage::IsBootloaderFound()
{
    return bootloaderFound;
}

/******************************************************************************/
void BootImage::SetCore(Core::Type type)
{
    core = type;
}

/******************************************************************************/
void BootImage::SetPmuFwSize(uint32_t size)
{
    pmuFwSize = size;
}

/******************************************************************************/
void BootImage::SetFsblFwSize(uint32_t size)
{
    fsblFwSize = size;
}

/******************************************************************************/
void BootImage::SetTotalPmuFwSize(uint32_t size)
{
    totalPmuFwSize = size;
}

/******************************************************************************/
void BootImage::SetTotalFsblFwSize(uint32_t size)
{
    totalFsblFwSize = size;
}

/******************************************************************************/
void BootImage::SetFsblSourceAddr(Binary::Address_t addr)
{
    sourceAddr = addr;
}

/******************************************************************************/
void BootImage::GenerateAuthenticationKeys(void) 
{
    if(options.GetAuthKeyGeneration() != GenAuthKeys::None)
    {
        keygen = new KeyGenerationStruct;
        keygen->format = options.GetAuthKeyGeneration();
        keygen->ppk_file = bifOptions->GetPPKFileName();
        keygen->psk_file = bifOptions->GetPSKFileName();
        keygen->spk_file = bifOptions->GetSPKFileName();
        keygen->ssk_file = bifOptions->GetSSKFileName();
        keygen->keyLength = currentAuthCtx->GetRsaKeyLength();
        if (keygen->format == GenAuthKeys::PEM || keygen->format == GenAuthKeys::RSA)
        {
            Key::GenerateRsaKeys(keygen);
        }
        else if (keygen->format == GenAuthKeys::ECDSA || keygen->format == GenAuthKeys::ECDSAP521)
        {
            if (options.archType != Arch::VERSAL)
            {
                LOG_ERROR("'-generate_keys <ecdsa-p384/ecdsa-p521>' is supported only with '-arch versal'.");
            }
            Key::GenerateEcdsaKeys(keygen);
        }
    }
}

/******************************************************************************/
void BootImage::GenerateGreyKey(void)
{
    if (options.GetGreyKeyGeneration()) 
    {
        currentEncryptCtx->GenerateGreyKey();
    }
}

/******************************************************************************/
void BootImage::SetOutputSplitModeFormat(SplitMode::Type splitMode, File::Type fmt)
{
    if(splitMode == SplitMode::Normal)
    {
        options.SetOutputMode(OutputMode::OUT_SPLIT_NORMAL, fmt);
    }
    else if (splitMode == SplitMode::SlaveMode)
    {
        options.SetOutputMode(OutputMode::OUT_SPLIT_SLAVEBOOT, fmt);
    }
}

/******************************************************************************/
void BootImage::SetOutputBitstreamModeFormat(File::Type type)
{
    if(type != File::Unknown)
    {
        options.SetOutputMode(OutputMode::OUT_BITSTREAM, type);
    }
}

/******************************************************************************/
void BootImage::SetCoreFromDestCpu(DestinationCPU::Type coreType, A53ExecState::Type procType)
{
    switch(coreType)
    {
        case DestinationCPU::A53_0:
            if (procType == A53ExecState::AARCH64)
            {
                SetCore(Core::A53Singlex64);
            }
            else
            {
                SetCore(Core::A53Singlex32);
            }
            break;

        case DestinationCPU::R5_0:
            SetCore(Core::R5Single);
            break;

        case DestinationCPU::R5_lockstep:
            SetCore(Core::R5Dual);
            break;

        case DestinationCPU::A53_1:
        case DestinationCPU::A53_2:
        case DestinationCPU::A53_3:
        case DestinationCPU::R5_1:
        case DestinationCPU::PMU:
            LOG_ERROR("FSBL can run from CPU A53-0 / R5-0 only. Please update destination_cpu accordingly.");
            break;

        default:
        case DestinationCPU::NONE:
            break;
    }
}

/******************************************************************************/
void BootImage::SetDestCpuFromCore(Core::Type coreType, DestinationCPU::Type cpuType)
{
    if (cpuType == DestinationCPU::NONE)
    {
        switch (coreType)
        {
        case Core::A53Singlex64:
        case Core::A53Singlex32:
        {
            cpuType = DestinationCPU::A53_0;
        }
        break;
        case Core::R5Single:
        {
            cpuType = DestinationCPU::R5_0;
        }
        break;
        case Core::R5Dual:
        {
            cpuType = DestinationCPU::R5_lockstep;
        }
        break;
        }
    }
}

/******************************************************************************/
void BootImage::SetAssumeEncryptionFlag(bool flag)
{
    assumeEncryption = flag;
}

/******************************************************************************/
std::vector<std::string>& BootImage::GetEncryptionKeyFileVec()
{ 
    return encryptionKeyFileVec; 
}

/******************************************************************************/
void BootImage::InsertEncryptionKeyFile(std::string filename)
{
    encryptionKeyFileVec.push_back(filename); 
}
