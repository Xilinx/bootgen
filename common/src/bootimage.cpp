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
#include <fstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>

#include "bootimage.h"
#include "bootimage-zynq.h"
#include "bootimage-zynqmp.h"
#include "bootimage-versal.h"
#include "bootimage-versal_2ve_2vm.h"
#include "bootimage-versal_2vp.h"
#include "bifoptions-versal_2vp.h"
#include "bootimage-spartanup.h"
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
#include "cdo-command.h"
extern "C" {
#include "cdo-overlay.h"
}

/*
------------------------------------------------------------------------------
****************************************************   F U N C T I O N S   ***
------------------------------------------------------------------------------
*/

std::vector<std::string> BootImage::encryptionKeyFileVec;

/******************************************************************************/
void BIF_File::Process(Options& options)
{
    ParseBifFile(options);

    bifOptionList = options.bifOptionsList;

    // Auto-detect keys and apply global->partition key inheritance.
    // This versal_2vp native multi-SPK key plumbing lives in the device-specific
    // layer; it is a no-op for other architectures (they use no .kmd files).
    BifOptionsVersal2VP::AutoDetectAndInheritKeys(bifOptionList);

    /* Parse the include bif file */
    if (options.includeBifOptionsList.size() > 1)
    {
        LOG_ERROR("Multiple include bif files is not supported.");
    }
    if (options.includeBifOptionsList.size() != 0)
    {
        options.bifOptionsList.clear();
        biffilename = options.includeBifOptionsList.front();
        ParseBifFile(options);

        includeBifOptionList = options.bifOptionsList;
        
        // Auto-detect keys and apply key inheritance for the included BIF.
        BifOptionsVersal2VP::AutoDetectAndInheritKeys(includeBifOptionList);


        if (includeBifOptionList.size() < bifOptionList.size()) {
            bifOptionList.erase(bifOptionList.begin(), bifOptionList.begin() + includeBifOptionList.size());
        }

        // Inherit global keys to partitions for include BIF (safe for all architectures)
        for (std::vector<BifOptions*>::iterator bifoptions = includeBifOptionList.begin(); bifoptions != includeBifOptionList.end(); bifoptions++)
        {
            for (std::list<PartitionBifOptions*>::iterator partitr = (*bifoptions)->partitionBifOptionList.begin(); 
                 partitr != (*bifoptions)->partitionBifOptionList.end(); partitr++)
            {
                bool hasPartitionKeys = !(*partitr)->ppkFile.empty() || !(*partitr)->pskFile.empty() || 
                                       !(*partitr)->spkFile.empty() || !(*partitr)->sskFile.empty();
                
                bool hasGlobalKeys = !(*bifoptions)->GetPPKFileName().empty() || !(*bifoptions)->GetPSKFileName().empty() || 
                                    !(*bifoptions)->GetSPKFileName().empty() || !(*bifoptions)->GetSSKFileName().empty();
                
                if (!hasPartitionKeys && hasGlobalKeys) {
                    LOG_TRACE("Partition has no local keys - inheriting from global keys");
                    
                    if (!(*bifoptions)->GetPPKFileName().empty()) {
                        (*partitr)->ppkFile = (*bifoptions)->GetPPKFileName();
                        LOG_TRACE("Partition inheriting global ppkfile: %s", (*partitr)->ppkFile.c_str());
                    }
                    if (!(*bifoptions)->GetPSKFileName().empty()) {
                        (*partitr)->pskFile = (*bifoptions)->GetPSKFileName();
                        LOG_TRACE("Partition inheriting global pskfile: %s", (*partitr)->pskFile.c_str());
                    }
                    if (!(*bifoptions)->GetSPKFileName().empty()) {
                        (*partitr)->spkFile = (*bifoptions)->GetSPKFileName();
                        LOG_TRACE("Partition inheriting global spkfile: %s", (*partitr)->spkFile.c_str());
                    }
                    if (!(*bifoptions)->GetSSKFileName().empty()) {
                        (*partitr)->sskFile = (*bifoptions)->GetSSKFileName();
                        LOG_TRACE("Partition inheriting global sskfile: %s", (*partitr)->sskFile.c_str());
                    }
                }
            }
        }

        if (options.IsSsitBif())
        {
            includeBifOptionList.back()->slrNum = 0x00;

            /* Back populate the slr number, based on the group name in master bif for SSIT use cases */
            for (std::list<PartitionBifOptions*>::iterator itr = includeBifOptionList.back()->partitionBifOptionList.begin(); itr != includeBifOptionList.back()->partitionBifOptionList.end(); itr++)
            {
                if (((*itr)->slrNum != 0xFF) && ((strcmp((*itr)->bifSection.c_str(), "") != 0)))
                {
                    for (std::size_t j = 0; j < includeBifOptionList.size(); j++)
                    {
                        if (strcmp(includeBifOptionList[j]->GetGroupName().c_str(), (*itr)->bifSection.c_str()) == 0)
                        {
                            includeBifOptionList[j]->slrNum = (*itr)->slrNum;
                            break;
                        }
                    }
                }
            }
        }

        AppendAndReplaceFilesinBIF();
        options.bifOptionsList = bifOptionList = includeBifOptionList;
    }

    std::unique_ptr<BootImage> currentbi = nullptr;
    uint8_t index = 0;
    size_t itr = 0;
    std::string lastPmcDataAesFile = bifOptionList.back()->GetPmcDataAesFile();

    for (std::vector<BifOptions*>::iterator bifoptions = bifOptionList.begin(); bifoptions != bifOptionList.end(); bifoptions++, itr++)
    {
        if (((*bifoptions)->slrBootCnt == 0) && ((*bifoptions)->slrConfigCnt == 0) && options.IsSsitBif())
        {
            (*bifoptions)->pdiType = PartitionType::SLR_SLAVE;
            if ((*bifoptions)->GetSmapWidth() != 0)
            {
                LOG_WARNING("smap_width for SSIT SLAVE PDIs is identified as %d. Setting it to the default value '0'.", (*bifoptions)->GetSmapWidth());
                (*bifoptions)->SetSmapWidth(0);
            }
        }
        else if ((*bifoptions)->pdiType == PartitionType::SLR_SLAVE_CONFIG)
        {
            (*bifoptions)->SetSmapWidth(0);
        }

        /* A bootimage PDI supplies the id, including when the BIF also has one.
           The image-header build copies GetIdCode() into the table, so the
           probed value has to be stored before that copy. */
        if (options.GetBasePdiIdCode() != 0)
        {
            (*bifoptions)->SetIdCode(options.GetBasePdiIdCode());
        }

        if (options.archType == Arch::ZYNQMP)
        {
            currentbi = std::make_unique<ZynqMpBootImage>(options, index);
        }
        else if (options.archType == Arch::VERSAL)
        {
            currentbi = std::make_unique<VersalBootImage>(options, index);
            
            currentbi->pmcDataAesFile = (*bifoptions)->GetPmcDataAesFile();
            if ((itr+1) == bifOptionList.size())
            {
                currentbi->pmcDataAesFile = lastPmcDataAesFile;
            }
        }
        else if (options.archType == Arch::VERSALGEN2)
        {
            currentbi = std::make_unique<Versal_2ve_2vmBootImage>(options, index);

            currentbi->pmcDataAesFile = (*bifoptions)->GetPmcDataAesFile();
            if ((itr + 1) == bifOptionList.size())
            {
                currentbi->pmcDataAesFile = lastPmcDataAesFile;
            }
        }
        else if (options.archType == Arch::VERSAL_2VP)
        {
            uint32_t idCode = (*bifoptions)->GetIdCode();
            /* The base PDI was probed before parsing. Use that id for the flow
               and, when the BIF omitted one, for the image as well. */
            if (options.GetBasePdiIdCode() != 0)
            {
                idCode = options.GetBasePdiIdCode();
                if ((*bifoptions)->GetIdCode() == 0)
                {
                    (*bifoptions)->SetIdCode(idCode);
                }
            }

            if (IdCodeManager::IsVersalFmtIdCode(idCode))
            {
                LOG_INFO("Versal-format variant detected (idcode: 0x%08X).", idCode);
                LOG_INFO("  - Switching to Versal PDI generation flow");
                LOG_INFO("  - PLM max size: 640KB (0xA0000)");
                LOG_INFO("  - PMC max size: 112KB (0x1C000)");
                options.SetVersalFmtVariant(true);
                currentbi = std::make_unique<VersalBootImage>(options, index);
            }
            else
            {
                options.SetVersalFmtVariant(false);
                if (IdCodeManager::IsVersal2vpNativeIdCode(idCode))
                {
                    LOG_INFO("versal_2vp native variant detected (idcode: 0x%08X). Using versal_2vp native PDI generation.", idCode);
                }
                else if (idCode == 0)
                {
                    LOG_WARNING("No idcode specified for versal_2vp arch. Defaulting to versal_2vp native (idcode: 0x%08X).", IDCODE_VERSAL_2VP_NATIVE);
                    if (!options.GetEfuseHashFileName().empty())
                    {
                        LOG_WARNING("No idcode specified for versal_2vp arch. Defaulting to versal_2vp native PPK hash for '-efuseppkbits' (idcode: 0x%08X).", IDCODE_VERSAL_2VP_NATIVE);
                    }
                    (*bifoptions)->SetIdCode(IDCODE_VERSAL_2VP_NATIVE);
                }
                else
                {
                    LOG_WARNING("Unknown idcode (0x%08X) for versal_2vp arch. Defaulting to versal_2vp native PDI generation.", idCode);
                    if (!options.GetEfuseHashFileName().empty())
                    {
                        LOG_WARNING("Unknown idcode (0x%08X) for versal_2vp arch. Defaulting to versal_2vp native PPK hash for '-efuseppkbits'.", idCode);
                    }
                }
                currentbi = std::make_unique<Versal_2vpBootImage>(options, index);
            }

            currentbi->pmcDataAesFile = (*bifoptions)->GetPmcDataAesFile();
            if ((itr + 1) == bifOptionList.size())
            {
                currentbi->pmcDataAesFile = lastPmcDataAesFile;
            }
        }
        else if (options.archType == Arch::SPARTANUP)
        {
            currentbi = std::make_unique<SpartanBootImage>(options, index);

            currentbi->pmcDataAesFile = (*bifoptions)->GetPmcDataAesFile();
            if ((itr + 1) == bifOptionList.size())
            {
                currentbi->pmcDataAesFile = lastPmcDataAesFile;
            }
        }
        else
        {
            currentbi = std::make_unique<ZynqBootImage>(options, index);
        }

        if (currentbi != NULL)
        {
            /* SSIT / multi-image BIF: ensure each image's CDO post-processing
               sees only that image's user keys. The global user_keys is
               populated by BIF parser via BifOptions::ParseUserKeyFile, but
               in a single bootgen run that processes multiple images the
               last-parsed userkeys file would otherwise leak into earlier
               images at CDO assembly time. Reset and re-load per image. */
            cdocmd_clear_user_keys();
            if (!(*bifoptions)->GetUserKeysFileName().empty())
            {
                (*bifoptions)->ParseUserKeyFile((*bifoptions)->GetUserKeysFileName());
            }

            currentbi->Add(*bifoptions);
            if (((*bifoptions)->pdiType != PartitionType::SLR_SLAVE_BOOT) && ((*bifoptions)->pdiType != PartitionType::SLR_SLAVE_CONFIG))
            {
                if ((*bifoptions)->pdiType == PartitionType::SLR_SLAVE)
                {
                    if (currentbi->bootloaderFound)
                    {
                        (*bifoptions)->pdiType = PartitionType::SLR_SLAVE_BOOT;
                    }
                    else
                    {
                        (*bifoptions)->pdiType = PartitionType::SLR_SLAVE_CONFIG;
                    }
                }
            }
            bootImages.push_back(std::move(currentbi));
        }
        Output(options, index);
        index++;
    }
}

/******************************************************************************/
BIF_File::~BIF_File()
{
    // bootImages unique_ptrs auto-delete
    // bifOptionList are non-owning pointers managed by Options
}

/******************************************************************************/
void BIF_File::Output(Options& options, uint8_t index)
{
    File::Type splitFileType = options.GetSplitType();
    std::list<std::string> outFilename = options.GetOutputFileNames();
    std::string fName = "";
    BootImage* bi = bootImages.at(index).get();
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
        bi->BuildAndLink(bi->cache.get());

        bi->SetOutputSplitModeFormat(options.bifOptions->GetSplitMode(), options.bifOptions->GetSplitFormat());
        bi->SetOutputBitstreamModeFormat(options.GetProcessBitstreamType());

        OutputMode::Type outputMode = options.GetOutputMode();
        bi->ValidateOutputModes(splitFileType, outputMode);

        if (options.GetOutType() != File::Unknown)
        {
            if (!outFilename.empty())
            {
                std::string outFile = outFilename.front();
                fName = StringUtils::RemoveExtension(outFile);
                outFilename.clear();
            }
            else
            {
                fName = StringUtils::RemoveExtension(biffilename);
            }
            fName += (options.GetOutType() == File::MCS) ? ".mcs" : ".bin";

        }

        if (outputMode == OutputMode::OUT_SPLIT_NORMAL || outputMode == OutputMode::OUT_SPLIT_SLAVEBOOT || splitFileType != File::Unknown)
        {
            outFilename.clear();
            if (options.GetOutType() != File::Unknown)
            {
                fName.clear();
            }    
            fName = StringUtils::RemoveExtension(biffilename);
            fName = fName.substr(fName.find_last_of("\\")+1, std::string::npos);
            if(splitFileType == File::Unknown)
            {
                splitFileType = options.GetOutputFormat();
            }
            fName += (splitFileType == File::MCS) ? ".mcs" : ".bin";
        }

        /* Process Bitstream Mode
        Create a file with the bitstream filename, add proper extension */
        if (outputMode == OutputMode::OUT_BITSTREAM) 
        {
            outFilename.clear();
            fName = bi->bitFilename;
            if(bi->bitFilename.empty())
            {
                LOG_ERROR("No bitstream in the BIF file - %s ",options.GetBifFilename().c_str());
            }
            fName += (options.GetProcessBitstreamType() == File::MCS) ? ".mcs" : ".bin";
        }

        if (fName != "")
        {
           outFilename.push_back(fName);
        }
        else if (outFilename.empty() && options.IsSsitBif())
        {
            /* section= names each slave PDI from -o. When -o is omitted,
               use the BIF name so those files match the parser. */
            fName = StringUtils::RemoveExtension(biffilename) + ".bin";
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
                if (options.IsSsitBif())
                {
                    out_filename = StringUtils::RemoveExtension(*filename) + "_" + bi->Name + ".bin";
                    if (bi->sync_offsets.size() != 0 && (bi->options.bifOptions->pdiType == PartitionType::SLR_SLAVE_CONFIG))
                    {
                        std::string sync_addresses_filename = StringUtils::RemoveExtension(*filename) + "_" + bi->Name + "_sync_offsets.txt";
                        std::ofstream f(sync_addresses_filename.c_str(), std::ios_base::out | std::ios_base::binary);
                        f << "sync_offsets" << "\n";
                        for (size_t i = 0; i < bi->sync_offsets.size(); i++)
                        {
                            f << bi->sync_offsets[i] << "\n";
                        }

                        f.close();

                        if (f.fail())
                        {
                            LOG_ERROR("Failed to write sync addresses to the file: %s", sync_addresses_filename.c_str());
                        }
                        LOG_TRACE("Sync addresses written to file %s successfully", sync_addresses_filename.c_str());
                    }
                }
            }
            /* Apply dual QSPI/OSPI splitting only to the final stitched image.
               Intermediate per-SLR PDIs are read back to build the master PDI,
               so they must be written un-split. */
            bool isFinalOutput = (index == (options.bifOptionsList.size() - 1));
            QspiMode::Type savedQspiMode = options.GetDualQspiMode();
            if (!isFinalOutput)
            {
                options.SetDualQspiMode(QspiMode::Unknown);
            }
            std::unique_ptr<OutputFile> file(OutputFile::Factory(out_filename));  // Smart pointer
            file->Output(options, *bi->cache);
            // Auto-deleted by unique_ptr
            if (!isFinalOutput)
            {
                options.SetDualQspiMode(savedQspiMode);
            }
        }

        /* Efuse PPK hash for burning the efuse */
        bi->OutputOptionalEfuseHash();

        /* PUF hash for burning the efuse */
        bi->OutputOptionalEfusePufHash();
        bi->OutputOptionalPufPDI();

        /* Secure Debug Image to enable the JTAG during a secure boot scenario */
        bi->OutputOptionalSecureDebugImage();
    }
}

/******************************************************************************/
BootImage::BootImage(Options& options, uint8_t index)
    : options(options)
    , headerSignature_Loaded(false)
    , headerAC(nullptr)
    , XipMode(false)
    , nullPartHeaderSection(NULL)
    , core(Core::A53Singlex64)
    , legacyEncryptionEnabled(true)
    , hash(nullptr)
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
    , keygen(nullptr)
    , partCount(0)
    , authHash(AuthHash::Sha3)
    , assumeEncryption(false)
    , convertAieElfToCdo(false)
    , createSubSystemPdis(false)
    , encryptedHeaders(NULL)
    , bootHeader(nullptr)
    , imageHeaderTable(nullptr)
    , partitionHeaderTable(nullptr)
    , currentAuthCtx(nullptr)
    , metaHdrAuthCtx(nullptr)
    , currentEncryptCtx(nullptr)
    , partitionOutput(nullptr)
    , deviceKey(nullptr)
    , firstIv(nullptr)
    , firstOptKey(nullptr)
    , overlayCDO(NULL)
    , globalSlrId(0)
    , iht_optional_data(nullptr)
    , iht_optional_data_length(0)
    , pmcDataAesFile("")
    , xplm_modules_data_length(0)
    , xplm_modules_data(nullptr)
    , authOnPartitionFound(false)
    , arch(Arch::ZYNQ)
    , hashBlock(nullptr)
    , hashBlockLength(0)
    , numHashTableEntries(0)
{
    options.bifOptions = bifOptions = options.bifOptionsList.at(index);
    Name = bifOptions->GetGroupName();
    cache = std::make_unique<Binary>();
    checksumTable = std::make_unique<ChecksumTable>();

    SetDeviceArchitecture();
    cdocmd_set_user_keys_versal_2ve_2vm(0);
}

/******************************************************************************/
BootImage::~BootImage()
{
    // currentAuthCtx, metaHdrAuthCtx, currentEncryptCtx are now std::unique_ptr - auto-deleted
    // bootHeader, imageHeaderTable, partitionHeaderTable are now std::unique_ptr - auto-deleted
    // partitionOutput, hash are now std::unique_ptr - auto-deleted
    // checksumTable, keygen, cache are std::unique_ptr - auto-deleted
    
    if(currentChecksumCtx)
    {
        std::unique_ptr<ChecksumContext> checksumPtr(currentChecksumCtx);  // Wrap in smart pointer for auto-delete
    }
    // deviceKey, firstIv, firstOptKey are now unique_ptr, automatically deleted
    // nullPartHeaderSection is owned by cache - don't delete it here!
    
    // CRITICAL: imageList OWNS the ImageHeader objects (via image.release())
    // SubSysImageHeader::imgList contains NON-OWNING references to the same objects
    // We must delete imageList items BEFORE deleting SubSysImageHeader objects
    LOG_TRACE("BootImage destructor: Deleting %d ImageHeader objects", imageList.size());
    for (ImageHeader* img : imageList)
    {
        if (img)
        {
            LOG_TRACE("  Deleting ImageHeader: %p", img);
            delete img;
        }
    }
    imageList.clear();
    
    // Clean up subSysImageList - BootImage owns these SubSysImageHeader objects
    // SubSysImageHeader destructor will just clear imgList (NON-OWNING pointers)
    LOG_TRACE("BootImage destructor: Deleting %d SubSysImageHeader objects", subSysImageList.size());
    for (SubSysImageHeader* subsys : subSysImageList)
    {
        if (subsys)
        {
            delete subsys;
        }
    }
    subSysImageList.clear();
    
    // hashTable cleanup handled automatically by unique_ptr
    hashTable.clear();
}

/******************************************************************************/
/******************************************************************************/
void BootImage::OutputOptionalEfuseHash()
{
    std::string hashFile = options.GetEfuseHashFileName();
    if (hashFile == "")
    {
        return; // No efuse PPK hash file specified
    }

    // Determine method based on metadata availability
    if (bifOptions->GetPrimaryMetadata().isValid)
    {
        // NEW APPROACH: .kmd metadata-driven (simplified syntax)
        // Validate primary key exists
        bool hasPrimaryKey = (bifOptions->GetPPKFileName() != "" || bifOptions->GetPKFileName() != "");
        if (!hasPrimaryKey)
        {
            LOG_ERROR("Cannot read PPK. PPK is mandatory to generate PPK hash bits, using '-efuseppkbits'.");
        }

        if (!currentAuthCtx)
        {
            LOG_ERROR("Authentication context not initialized");
        }

        // Uses virtual method so devices can override if needed
        GeneratePPKHashWithKmdMetadata(hashFile);
    }
    else
    {
        // LEGACY APPROACH: Original implementation for Zynq/ZynqMP and explicit syntax
        // This handles:
        //   1. Zynq/ZynqMP devices (no .kmd support, no override)
        //   2. Devices with override will handle their own logic (Versal/Gen2/SpartanUP/Versal2VP)
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
void BootImage::GeneratePPKHashWithKmdMetadata(const std::string& hashFile)
{
    // BASE CLASS IMPLEMENTATION (.kmd metadata-driven PPK hash generation)
    // This works for all devices that have GeneratePPKHashWithMetadata() in their auth context
    // Devices can override this method if they need custom behavior
    
    if (!currentAuthCtx)
    {
        LOG_ERROR("Authentication context not initialized. Cannot generate PPK hash.");
    }
    
    // Primary algorithm PPK hash generation
    LOG_INFO("Generating PPK hash using .kmd metadata (Primary Algorithm)");
    
    std::string primaryKeyFile = !bifOptions->GetPKFileName().empty() ? 
                                  bifOptions->GetPKFileName() : bifOptions->GetPPKFileName();
    
    currentAuthCtx->ppkFile = primaryKeyFile;
    currentAuthCtx->GeneratePPKHashWithMetadata(hashFile, bifOptions->GetPrimaryMetadata());
    
    // HYBRID AUTHENTICATION: Check for secondary algorithm
    bool hasSecondaryKey = (bifOptions->GetPPKFileName1() != "" || bifOptions->GetPKFileName1() != "");
    if (hasSecondaryKey && bifOptions->GetPrimaryMetadata1().isValid)
    {
        LOG_INFO("Generating PPK hash using .kmd metadata (Secondary Algorithm - Hybrid)");
        
        std::string secondaryKeyFile = !bifOptions->GetPKFileName1().empty() ? 
                                        bifOptions->GetPKFileName1() : bifOptions->GetPPKFileName1();
        
        // Secondary hash appends to the same file
        currentAuthCtx->GeneratePPKHashWithMetadata(hashFile, bifOptions->GetPrimaryMetadata1(), secondaryKeyFile);
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
void BootImage::WarnUnappliedOverlayMarkers(void)
{
    if (overlayCDO == NULL)
    {
        return;
    }
    const char* marker = NULL;
    for (unsigned int i = 0; (marker = cdooverlay_get_unapplied_marker((CdoOverlayInfo*)overlayCDO, i)) != NULL; i++)
    {
        LOG_WARNING("Marker '%s' from overlay CDO '%s' is not found in any CDO partition of the BIF. The overlay content for this marker is not applied.",
            marker, options.GetOverlayCDOFileName().c_str());
    }
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
        LOG_INFO("       Partition %d: %s,  Size: %d", index++, (*pH)->partition->GetSectionPtr()->Name.c_str(), (*pH)->GetPartitionSize());
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
        keygen = std::make_unique<KeyGenerationStruct>();
        keygen->format = options.GetAuthKeyGeneration();
        keygen->ppk_file = bifOptions->GetPPKFileName();
        keygen->psk_file = bifOptions->GetPSKFileName();
        keygen->spk_file = bifOptions->GetSPKFileName();
        keygen->ssk_file = bifOptions->GetSSKFileName();
        keygen->keyLength = currentAuthCtx->GetRsaKeyLength();
        if (keygen->format == GenAuthKeys::PEM || keygen->format == GenAuthKeys::RSA)
        {
            Key::GenerateRsaKeys(keygen.get());
        }
        else if (keygen->format == GenAuthKeys::ECDSA || keygen->format == GenAuthKeys::ECDSAP521)
        {
            if ((options.archType == Arch::ZYNQ) || (options.archType == Arch::ZYNQMP))
            {
                LOG_ERROR("'-generate_keys <ecdsa-p384/ecdsa-p521>' is not supported with the mentioned '-arch'.");
            }
            Key::GenerateEcdsaKeys(keygen.get());
        }
        else if (keygen->format == GenAuthKeys::LMS)
        {
            if ((options.archType == Arch::ZYNQ) || (options.archType == Arch::ZYNQMP) || (options.archType == Arch::VERSAL))
            {
                LOG_ERROR("'-generate_keys <lms>' is not supported with the mentioned '-arch'.");
            }
            Key::GenerateLmsKeys(keygen.get(), bifOptions->GetPrimaryLmsParams(), bifOptions->GetSecondaryLmsParams());
        }
        else if (keygen->format == GenAuthKeys::MLDSA)
        {
            Key::GenerateMldsaKeys(keygen.get());
        }
        else if (keygen->format == GenAuthKeys::SLH_SHAKE256)
        {
            if (options.archType != Arch::VERSAL_2VP)
            {
                LOG_ERROR("'-generate_keys <slhdsa>' is not supported with the mentioned '-arch'.");
            }
            Key::GenerateSlhdsaKeys(keygen.get());
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
void BootImage::SetDeviceArchitecture()
{
    if (options.IsVersalNetSeries())
        arch = Arch::VERSALNET;
    else if (options.IsDl9Series())
        arch = Arch::SPARTANUPV2;
    else
        arch = options.archType;
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

/******************************************************************************/
static bool IsBifTokenBoundary(const std::string& code, size_t pos, size_t end)
{
    const bool validBefore = (pos == 0) ||
        (!isalnum(static_cast<unsigned char>(code[pos - 1])) && (code[pos - 1] != '_') &&
         (code[pos - 1] != '/') && (code[pos - 1] != '.'));
    const bool validAfter = (end >= code.size()) ||
        (!isalnum(static_cast<unsigned char>(code[end])) && (code[end] != '_') &&
         (code[end] != '.'));
    return validBefore && validAfter;
}

/******************************************************************************/
/* Comment-stripped BIF text. The architecture must be selected before the
   real parser runs, so generation looks at this text for id_code and for the
   bootimage file. */
static std::string ReadBifWithoutComments(const std::string& bifFileName)
{
    std::ifstream bif(bifFileName.c_str());
    if (!bif)
    {
        return "";
    }

    bool inBlockComment = false;
    std::string line;
    std::string code;
    while (std::getline(bif, line))
    {
        for (size_t i = 0; i < line.size(); ++i)
        {
            if (inBlockComment)
            {
                if ((line[i] == '*') && (i + 1 < line.size()) && (line[i + 1] == '/'))
                {
                    inBlockComment = false;
                    ++i;
                }
                continue;
            }
            if ((line[i] == '/') && (i + 1 < line.size()) && (line[i + 1] == '*'))
            {
                inBlockComment = true;
                ++i;
                continue;
            }
            if ((line[i] == '/') && (i + 1 < line.size()) && (line[i + 1] == '/'))
            {
                break;
            }
            code += line[i];
        }
        code += '\n';
    }
    return code;
}

/******************************************************************************/
/* Pick the first active id_code out of the BIF text. */
static uint32_t ScanBifForIdCode(const std::string& code)
{
    size_t pos = 0;
    while ((pos = code.find("id_code", pos)) != std::string::npos)
    {
        const size_t end = pos + strlen("id_code");
        if (IsBifTokenBoundary(code, pos, end))
        {
            size_t equal = end;
            while ((equal < code.size()) &&
                   isspace(static_cast<unsigned char>(code[equal])))
            {
                ++equal;
            }
            if ((equal < code.size()) && (code[equal] == '='))
            {
                ++equal;
                while ((equal < code.size()) &&
                       isspace(static_cast<unsigned char>(code[equal])))
                {
                    ++equal;
                }
                char* valueEnd = NULL;
                const unsigned long value = strtoul(code.c_str() + equal, &valueEnd, 0);
                if ((valueEnd != (code.c_str() + equal)) &&
                    (value <= std::numeric_limits<uint32_t>::max()))
                {
                    return static_cast<uint32_t>(value);
                }
            }
        }
        pos = end;
    }
    return 0;
}

/******************************************************************************/
static std::string BifPathAfterFileToken(const std::string& code, size_t filePos)
{
    size_t equal = filePos + strlen("file");
    while ((equal < code.size()) && isspace(static_cast<unsigned char>(code[equal])))
    {
        ++equal;
    }
    if ((equal >= code.size()) || (code[equal] != '='))
    {
        return "";
    }
    ++equal;
    while ((equal < code.size()) && isspace(static_cast<unsigned char>(code[equal])))
    {
        ++equal;
    }
    if (equal >= code.size())
    {
        return "";
    }

    char quote = 0;
    if ((code[equal] == '"') || (code[equal] == '\''))
    {
        quote = code[equal];
        ++equal;
    }
    size_t end = equal;
    if (quote != 0)
    {
        while ((end < code.size()) && (code[end] != quote))
        {
            ++end;
        }
    }
    else
    {
        while ((end < code.size()) &&
               !isspace(static_cast<unsigned char>(code[end])) &&
               (code[end] != ',') && (code[end] != '}') && (code[end] != '{'))
        {
            ++end;
        }
    }
    return code.substr(equal, end - equal);
}

/******************************************************************************/
static std::string ResolveBifPath(const std::string& bifFileName, const std::string& path)
{
    if (path.empty() || (path[0] == '/'))
    {
        return path;
    }
    const size_t slash = bifFileName.find_last_of('/');
    if (slash == std::string::npos)
    {
        return path;
    }
    return bifFileName.substr(0, slash + 1) + path;
}

/******************************************************************************/
/* First bootimage partition file in the BIF. Its table id selects the flow
   when the BIF itself has no id_code, and it wins when both are present. */
static std::string ScanBifForBootImageFile(const std::string& bifFileName, const std::string& code)
{
    size_t pos = 0;
    while ((pos = code.find("bootimage", pos)) != std::string::npos)
    {
        const size_t end = pos + strlen("bootimage");
        if (!IsBifTokenBoundary(code, pos, end))
        {
            pos = end;
            continue;
        }

        const size_t blockStart = code.rfind('{', pos);
        size_t blockEnd = code.size();
        if (blockStart != std::string::npos)
        {
            int depth = 0;
            for (size_t i = blockStart; i < code.size(); ++i)
            {
                if (code[i] == '{')
                {
                    ++depth;
                }
                else if (code[i] == '}')
                {
                    --depth;
                    if (depth == 0)
                    {
                        blockEnd = i;
                        break;
                    }
                }
            }
        }
        const size_t searchFrom = (blockStart == std::string::npos) ? pos : blockStart;
        size_t filePos = searchFrom;
        while ((filePos = code.find("file", filePos)) != std::string::npos && (filePos < blockEnd))
        {
            const size_t fileEnd = filePos + strlen("file");
            if (IsBifTokenBoundary(code, filePos, fileEnd))
            {
                const std::string path = BifPathAfterFileToken(code, filePos);
                if (!path.empty())
                {
                    return ResolveBifPath(bifFileName, path);
                }
            }
            filePos = fileEnd;
        }
        pos = end;
    }
    return "";
}

/******************************************************************************/
void BIF_File::ParseBifFile(Options& options)
{
    /* Versal-format variants use the regular Versal PDI flow. Switch the
       architecture before parsing so all downstream validation follows it.
       A bootimage file's table id wins over id_code written in the BIF, except
       when the BIF is versal_2vp native and the base PDI is Versal-format:
       that mix is rejected instead of silently following the base. */
    if (options.GetArchType() == Arch::VERSAL_2VP)
    {
        const std::string bifText = ReadBifWithoutComments(biffilename);
        uint32_t idCode = 0;
        const uint32_t bifIdCode = ScanBifForIdCode(bifText);
        const std::string basePdi = ScanBifForBootImageFile(biffilename, bifText);
        bool idFromBasePdi = false;
        if (!basePdi.empty())
        {
            idCode = ReadPdiTableIdCode(basePdi);
            if (idCode != 0)
            {
                options.SetBasePdiIdCode(idCode);
                idFromBasePdi = true;
            }
        }
        if (IdCodeManager::IsVersal2vpNativeIdCode(bifIdCode) &&
            IdCodeManager::IsVersalFmtIdCode(idCode))
        {
            LOG_ERROR("Base PDI %s belongs to Versal, which is not supported for versal_2vp native (idcode 0x%08X).",
                StringUtils::BaseName(basePdi).c_str(), bifIdCode);
        }
        if (idCode == 0)
        {
            idCode = bifIdCode;
        }
        if (IdCodeManager::IsVersalFmtIdCode(idCode))
        {
            if (idFromBasePdi)
            {
                LOG_INFO("Versal-format variant idcode found in base PDI. Using the Versal flow.");
            }
            else
            {
                LOG_INFO("Versal-format variant idcode found in BIF. Using the Versal flow.");
            }
            options.SetArchType(Arch::VERSAL);
            options.SetVersalNetSeries(false);
            options.SetVersalFmtVariant(true);
        }
    }

    std::string basefile = StringUtils::BaseName(biffilename);

    LOG_TRACE("Parsing BIF file - %s", basefile.c_str());
    BIF::FlexScanner scanner;
    BIF::BisonParser parser(scanner, options);

    scanner.filename = biffilename;
    std::ifstream s(biffilename.c_str());

    if (!s)
    {
        LOG_ERROR("Cannot read BIF file - %s ", basefile.c_str());
    }
    if (s.peek() == EOF)
    {
        LOG_ERROR("BIF file is empty - %s ", basefile.c_str());
    }
    scanner.switch_streams(&s);
    int res = parser.parse();
    if (res)
    {
        LOG_ERROR("BIF file - %s, parsing failed with code %d", basefile.c_str(), res);
    }

    for (const auto& f : scanner.includedFiles) {
        options.includeBifOptionsList.push_back(strdup(f.c_str()));
    }

    LOG_INFO("BIF file parsed successfully - %s ", basefile.c_str());
}

/******************************************************************************/
void BIF_File::AppendAndReplaceFilesinBIF()
{
    /* If slr number matches between partitions - replace the files */
    for (size_t i = 0; i < includeBifOptionList.size(); i++)
    {
        for (size_t j = 0; j < bifOptionList.size(); j++)
        {
            for (std::list<PartitionBifOptions*>::iterator itr1 = bifOptionList[j]->partitionBifOptionList.begin(); itr1 != bifOptionList[j]->partitionBifOptionList.end(); itr1++)
            {
                if (includeBifOptionList[i]->slrNum == (*itr1)->slrNum)
                {
                    if ((*itr1)->bootloader)
                    {
                        for (std::list<PartitionBifOptions*>::iterator itr2 = includeBifOptionList[i]->partitionBifOptionList.begin(); itr2 != includeBifOptionList[i]->partitionBifOptionList.end(); itr2++)
                        {
                            if ((*itr2)->bootloader)
                            {
                                if((*itr1)->slrNum != 0xFF)
                                    LOG_TRACE("Replacing '%s' with '%s' for SLR %d", (*itr2)->filename.c_str(), (*itr1)->filename.c_str(), (*itr1)->slrNum);
                                else
                                    LOG_TRACE("Replacing '%s' with '%s'", (*itr2)->filename.c_str(), (*itr1)->filename.c_str());

                                (*itr2)->filename = (*itr1)->filename;
                                (*itr2)->filelist = (*itr1)->filelist;
                                (*itr1)->erasePartition = true;
                                break;
                            }
                        }
                    }

                    if ((*itr1)->pmcData)
                    {
                        for (std::list<PartitionBifOptions*>::iterator itr2 = includeBifOptionList[i]->partitionBifOptionList.begin(); itr2 != includeBifOptionList[i]->partitionBifOptionList.end(); itr2++)
                        {
                            if ((*itr2)->pmcData)
                            {
                                if ((*itr1)->slrNum != 0xFF)
                                    LOG_TRACE("Replacing '%s' with '%s' for SLR %d", (*itr2)->filename.c_str(), (*itr1)->filename.c_str(), (*itr1)->slrNum);
                                else
                                    LOG_TRACE("Replacing '%s' with '%s'", (*itr2)->filename.c_str(), (*itr1)->filename.c_str());

                                (*itr2)->filename = (*itr1)->filename;
                                includeBifOptionList[i]->SetPmcdataFile((*itr1)->filename);
                                (*itr2)->filelist = (*itr1)->filelist;

                                includeBifOptionList[i]->ClearPmcCdoFileList();
                                for (size_t listSize = 0; listSize < (*itr1)->filelist.size(); listSize++)
                                    includeBifOptionList[i]->SetPmcCdoFileList((*itr1)->filelist[listSize]);
                                (*itr1)->erasePartition = true;
                                break;
                            }
                        }
                    }

                    if ((*itr1)->destCPUType == DestinationCPU::PMU)
                    {
                        for (std::list<PartitionBifOptions*>::iterator itr2 = includeBifOptionList[i]->partitionBifOptionList.begin(); itr2 != includeBifOptionList[i]->partitionBifOptionList.end(); itr2++)
                        {
                            if ((*itr2)->destCPUType == DestinationCPU::PMU)
                            {
                                if ((*itr1)->slrNum != 0xFF)
                                    LOG_TRACE("Replacing '%s' with '%s' for SLR %d", (*itr2)->filename.c_str(), (*itr1)->filename.c_str(), (*itr1)->slrNum);
                                else
                                    LOG_TRACE("Replacing '%s' with '%s'", (*itr2)->filename.c_str(), (*itr1)->filename.c_str());

                                (*itr2)->filename = (*itr1)->filename;
                                (*itr2)->filelist = (*itr1)->filelist;
                                (*itr1)->erasePartition = true;
                                break;
                            }
                        }
                    }
                }
            }
            for (std::list<ImageBifOptions*>::iterator itr1 = bifOptionList[j]->imageBifOptionList.begin(); itr1 != bifOptionList[j]->imageBifOptionList.end(); )
            {
                bool incremented = false;
                for (std::list<PartitionBifOptions*>::iterator itr2 = (*itr1)->partitionBifOptionsList.begin(); itr2 != (*itr1)->partitionBifOptionsList.end(); )
                {
                    if ((*itr2)->erasePartition)
                    {
                        (*itr1)->partitionBifOptionsList.remove(*itr2++);
                        if ((*itr1)->partitionBifOptionsList.size() == 0)
                        {
                            bifOptionList[j]->imageBifOptionList.remove(*itr1++);
                            incremented = true;
                            break;
                        }
                        //break;
                    }
                    else
                    {
                        itr2++;
                    }
                }
                if(!incremented)
                    itr1++;
            }
        }
    }

    for (size_t i = 0; i < includeBifOptionList.size(); i++)
    {
        if ((includeBifOptionList[i]->pdiType == PartitionType::SLR_CONFIG) || (includeBifOptionList[i]->slrNum == 0))
        {
            for (size_t j = 0; j < bifOptionList.size(); j++)
            {
                for (std::list<ImageBifOptions*>::iterator itr1 = bifOptionList[j]->imageBifOptionList.begin(); itr1 != bifOptionList[j]->imageBifOptionList.end(); itr1++)
                {
                    std::list<PartitionBifOptions*>::iterator itr2 = (*itr1)->partitionBifOptionsList.begin();
                    {
                        if (includeBifOptionList[i]->slrNum == (*itr2)->slrNum)
                        {
                            includeBifOptionList[i]->imageBifOptionList.push_back((*itr1));
                            includeBifOptionList[i]->partitionBifOptionList.insert(includeBifOptionList[i]->partitionBifOptionList.end(), (*itr1)->partitionBifOptionsList.begin(), (*itr1)->partitionBifOptionsList.end());
                        }
                    }
                }
            }
        }
        else if(includeBifOptionList[i]->slrNum == 0xFF)
        {
            for (size_t j = 0; j < bifOptionList.size(); j++)
            {
                includeBifOptionList[i]->imageBifOptionList.splice(includeBifOptionList[i]->imageBifOptionList.end(), bifOptionList[j]->imageBifOptionList);
            }
        }
    }
}
