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
#include "encryption-versal.h"
#include "bootimage.h"
#include "encryptutils.h"
#include "options.h"
#include "imageheadertable-versal.h"
#include "bootheader-versal.h"
#include "partitionheadertable-versal.h"
#include <openssl/rand.h>

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

uint32_t VersalEncryptionContext::ConfigureEncryptionBlocksforPmcData(BootImage& bi, PartitionHeader* partHdr)
{
    uint32_t totalencrBlocks = 0;
    Binary::Length_t lastBlock = 0;
    std::vector<uint32_t> encrBlocks = partHdr->imageHeader->GetEncrBlocksList();

    totalencrBlocks = encrBlocks.size();
    uint32_t overhead = (totalencrBlocks) * (SECURE_HDR_SZ + AES_GCM_TAG_SZ);

    /* Due to encryption over head, the actual default size on which the partition needs to be Key rolled is always less than 64KB.
    So first calculate the default key roll data size by substracting the overhead. */
    /* Then calculate the number of such blocks possible on a given partition. */
    /* Note that the last block will always be based on the partition length.*/

    std::vector<uint32_t> secureChunkEncrBlocks;
    uint32_t actualSecureChunkSize = bi.GetSecureChunkSize(partHdr->imageHeader->IsBootloader()) - overhead;
    if (partHdr->imageHeader->GetAuthenticationType() == Authentication::None && !partHdr->imageHeader->GetDelayAuthFlag())
    {
        actualSecureChunkSize += SHA3_LENGTH_BYTES;
    }

    uint32_t totalKeyRollencrBlocks = GetTotalEncryptionBlocks(partHdr->imageHeader->GetTotalPmcFwSizeIh(), secureChunkEncrBlocks, actualSecureChunkSize, &lastBlock);
    secureChunkEncrBlocks.clear();

    for (uint32_t itr = 0; itr < totalKeyRollencrBlocks; itr++)
    {
        if ((itr == totalKeyRollencrBlocks - 1) && (lastBlock != 0))
        {
            secureChunkEncrBlocks.push_back(lastBlock);
            lastBlock = 0;
        }
        else
        {
            secureChunkEncrBlocks.push_back(actualSecureChunkSize);
        }
    }

    /* Now chunk each default key roll data size, based on user encryption blocks.
    Note that the last block will always be based on the partition length.*/
    //std::vector<uint32_t> blocks;
    for (uint32_t itr1 = 0; itr1 < totalKeyRollencrBlocks; itr1++)
    {
        if ((itr1 == totalKeyRollencrBlocks - 1) && (secureChunkEncrBlocks[itr1] != actualSecureChunkSize))
        {
            Binary::Length_t encrBlocksSize = 0;
            for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
            {
                encrBlocksSize += encrBlocks[itr];
                if (secureChunkEncrBlocks[itr1] > encrBlocksSize)
                {
                    bi.options.bifOptions->pmcdataBlocks.push_back(encrBlocks[itr]);
                }
                else
                {
                    bi.options.bifOptions->pmcdataBlocks.push_back(secureChunkEncrBlocks[itr1] - (encrBlocksSize - encrBlocks[itr]));
                    break;
                }
            }
        }
        else
        {
            for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
            {
                bi.options.bifOptions->pmcdataBlocks.push_back(encrBlocks[itr]);
            }
        }
    }
    totalencrBlocks = bi.options.bifOptions->pmcdataBlocks.size();
    return totalencrBlocks;
}

uint32_t VersalEncryptionContext::ConfigureEncryptionBlocksforPartition(BootImage& bi, PartitionHeader* partHdr)
{
    Options& options = bi.options;
    uint32_t totalencrBlocks = 0;
    Binary::Length_t lastBlock = 0;
    std::vector<uint32_t> encrBlocks = partHdr->imageHeader->GetEncrBlocksList();

    totalencrBlocks = encrBlocks.size();
    uint32_t overhead = (totalencrBlocks) * (SECURE_HDR_SZ + AES_GCM_TAG_SZ);

    /* Due to encryption over head, the actual default size on which the partition needs to be Key rolled is always less than 64KB.
    So first calculate the default key roll data size by substracting the overhead. */
    /* Then calculate the number of such blocks possible on a given partition. */
    /* Note that the last block will always be based on the partition length.*/

    std::vector<uint32_t> secureChunkEncrBlocks;
    uint32_t actualSecureChunkSize = bi.GetSecureChunkSize(partHdr->imageHeader->IsBootloader()) - overhead;
    if (partHdr->imageHeader->GetAuthenticationType() == Authentication::None && !partHdr->imageHeader->GetDelayAuthFlag())
    {
        actualSecureChunkSize += SHA3_LENGTH_BYTES;
    }

    uint32_t totalKeyRollencrBlocks = 0;
    if (partHdr->imageHeader->IsBootloader())
    {
        totalKeyRollencrBlocks = GetTotalEncryptionBlocks(partHdr->imageHeader->GetFsblFwSizeIh(), secureChunkEncrBlocks, actualSecureChunkSize, &lastBlock);
    }
    else
    {
        totalKeyRollencrBlocks = GetTotalEncryptionBlocks(partHdr->partition->section->Length, secureChunkEncrBlocks, actualSecureChunkSize, &lastBlock);
    }
    
    secureChunkEncrBlocks.clear();

    for (uint32_t itr = 0; itr < totalKeyRollencrBlocks; itr++)
    {
        if ((itr == totalKeyRollencrBlocks - 1) && (lastBlock != 0))
        {
            secureChunkEncrBlocks.push_back(lastBlock);
            lastBlock = 0;
        }
        else
        {
            secureChunkEncrBlocks.push_back(actualSecureChunkSize);
        }
    }

    /* Now chunk each default key roll data size, based on user encryption blocks.
    Note that the last block will always be based on the partition length.*/
    options.bifOptions->GetEncryptionBlocksList().clear();
    for (uint32_t itr1 = 0; itr1 < totalKeyRollencrBlocks; itr1++)
    {
        if ((itr1 == totalKeyRollencrBlocks - 1) && (secureChunkEncrBlocks[itr1] != actualSecureChunkSize))
        {
            Binary::Length_t encrBlocksSize = 0;
            for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
            {
                encrBlocksSize += encrBlocks[itr];
                if (secureChunkEncrBlocks[itr1] > encrBlocksSize)
                {
                    options.bifOptions->InsertEncryptionBlock(encrBlocks[itr]);
                }
                else
                {
                    options.bifOptions->InsertEncryptionBlock(secureChunkEncrBlocks[itr1] - (encrBlocksSize - encrBlocks[itr]));
                    break;
                }
            }
        }
        else
        {
            for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
            {
                options.bifOptions->InsertEncryptionBlock(encrBlocks[itr]);
            }
        }
    }
    totalencrBlocks = options.bifOptions->GetEncryptionBlocksList().size();
    return totalencrBlocks;
}

/******************************************************************************/
static std::string ReadIV0(const std::string& inputFileName)
{
    std::ifstream keyFile(inputFileName.c_str());
    int aesKeyNum = 0;
    int aesIvNum = 0;

    if (!keyFile)
    {
        LOG_ERROR("Failure reading AES key file - %s", inputFileName.c_str());
    }

    while (keyFile)
    {
        std::string word;
        keyFile >> word;
        if (word == "")
        {
            return "";
        }

        char c = ' ';
        if (word == "Device")
        {
            word = "";
            keyFile >> word;
            c = word[word.size() - 1];
            word.erase(word.size() - 1);
        }
        else if (word == "Key")
        {
            keyFile >> word;
            if (word == "Opt")
            {
                LOG_ERROR("The key word 'Key Opt' is not supported in VERSAL architecture");
            }
            if (word != "" && isalnum(word[0]))
            {
                /* Second Word is "0" or "1" or "2" ...*/
                int index = std::stoi(word);
                if (aesKeyNum != index)
                {
                    LOG_DEBUG(DEBUG_STAMP, "Key order incorrect.");
                    LOG_ERROR("Error parsing AES key file - %s.", inputFileName.c_str());
                }
                aesKeyNum++;
                word = "";
                while ((keyFile >> c) && isalnum(c))
                {
                    word.push_back(c);
                }
            }
            else
            {
                /* Second Word other than "0",  - throw error */
                LOG_DEBUG(DEBUG_STAMP, "Unknown key type - '%s' in %s", word.c_str(), inputFileName.c_str());
                LOG_ERROR("Error parsing AES key file - %s", inputFileName.c_str());
            }
        }
        else if (word == "IV")
        {
            keyFile >> word;
            if (word != "")
            {
                if (word.size() == ((BYTES_PER_IV * 2) + 1))
                {
                    c = word[word.size() - 1];
                    word.erase(word.size() - 1);
                    return word;
                }
                else
                {
                    int index = std::stoi(word);
                    if (aesIvNum != index)
                    {
                        LOG_DEBUG(DEBUG_STAMP, "Iv order incorrect.");
                        LOG_ERROR("Error parsing AES key file - %s.", inputFileName.c_str());
                    }
                    aesIvNum++;
                    word = "";
                    while ((keyFile >> c) && isalnum(c))
                    {
                        word.push_back(c);
                    }
                    if (index == 0)
                    {
                        return word;
                    }
                }
            }
        }
        else if (word == "Seed")
        {
            word = "";
            while ((keyFile >> c) && isalnum(c))
            {
                word.push_back(c);
            }
        }
        else if (word == "Label")
        {
            word = "";
            while ((keyFile >> c) && isalnum(c))
            {
                word.push_back(c);
            }
        }
        else if (word == "Context")
        {
            word = "";
            while ((keyFile >> c) && isalnum(c))
            {
                word.push_back(c);
            }
        }
        else if (word == "FixedInputData")
        {
            word = "";
            while ((keyFile >> c) && isalnum(c))
            {
                word.push_back(c);
            }
        }
        else
        {
            /* If the word is neither of the above */
            LOG_DEBUG(DEBUG_STAMP, "'Key' or 'Device' identifier expected, '%s' found instead", word.c_str());
            LOG_ERROR("Error parsing AES key file - %s", inputFileName.c_str());
        }
        if (c != ';')
        {
            /* Semicolons expected at end of every line */
            while ((keyFile >> c) && isspace(c))
            {
                word.push_back(c);
            }
        }
        if (c != ';')
        {
            LOG_DEBUG(DEBUG_STAMP, "Terminating ';' expected. Last word read was '%s'", word.c_str());
            LOG_ERROR("Error parsing AES key file - %s", inputFileName.c_str());
        }
    }
    return "";
}


/******************************************************************************/
void VersalEncryptionContext::ChunkifyAndProcess(BootImage& bi, PartitionHeader* partHdr)
{
    Options& options = bi.options;

    LOG_INFO("Encrypting the partition - %s", partHdr->partition->section->Name.c_str());

    WarnforDPACMImpactonBootTime(partHdr->imageHeader->GetEncrBlocksList().size() != 0, partHdr->imageHeader->GetDpacm() == DpaCM::DpaCMEnable);

    uint32_t totalencrBlocks = ConfigureEncryptionBlocksforPartition(bi, partHdr);
    LOG_TRACE("Total no. of Key/IV pairs needed to encrypt - %d", totalencrBlocks + 1);

    /* Get the key file */
    SetAesFileName(partHdr->partitionAesKeyFile);
    LOG_INFO("Key file - %s", aesFilename.c_str());
    if (partHdr->generateAesKeyFile)
    {
        std::ifstream keyFile(aesFilename);
        bool fileExists = keyFile.good();
        if (!fileExists)
        {
            if (bi.aesKeyandKeySrc.size() != 0)
            {
                for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
                {
                    if (partHdr->imageHeader->GetEncryptionKeySrc() == bi.aesKeyandKeySrc[i].first)
                    {
                        aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                        memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                        break;
                    }
                }
            }
            GenerateEncryptionKeyFile(aesFilename, options);
        }
        else
        {
            LOG_ERROR("Key Generation Error !!!\n           File - %s already exists.", aesFilename.c_str());
        }
    }

    bi.InsertEncryptionKeyFile(aesFilename);
    CheckForSameAesKeyFiles(bi.GetEncryptionKeyFileVec());

    ReadEncryptionKeyFile(aesFilename);

    std::pair<KeySource::Type, uint32_t*> asesKeyandKeySrcPair(partHdr->imageHeader->GetEncryptionKeySrc(), aesKey);
    bi.aesKeyandKeySrc.push_back(asesKeyandKeySrcPair);
    bi.bifOptions->CheckForBadKeyandKeySrcPair(bi.aesKeyandKeySrc, aesFilename);

    options.SetDevicePartName(deviceName);

    CheckForExtraKeyIVPairs(totalencrBlocks, partHdr->partition->section->Name);

    if (!aesSeedexits && aesKeyVec.size() != 1)
    {
        if (totalencrBlocks + 1 > aesKeyVec.size())
        {
            LOG_ERROR("AES Key file has less keys than the number of blocks to be encrypted in %s.", partHdr->partition->section->Name.c_str());
        }
        if (totalencrBlocks + 1 > aesIvVec.size())
        {
            LOG_ERROR("AES Key file has less IVs than the number of blocks to be encrypted in %s.", partHdr->partition->section->Name.c_str());
        }
    }
    else
    {
        if (bi.aesKeyandKeySrc.size() != 0)
        {
            for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
            {
                if (partHdr->imageHeader->GetEncryptionKeySrc() == bi.aesKeyandKeySrc[i].first)
                {
                    if (aesKey == NULL)
                    {
                        aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                        memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                    }
                    if (aesKeyVec.size() == 0)
                    {
                        aesKeyVec.push_back(ConvertKeyIvToString((uint8_t *)aesKey, AES_GCM_KEY_SZ).c_str());
                    }
                    break;
                }
            }
        }
        GenerateRemainingKeys(options);
    }

    CheckForRepeatedKeyIVPairs(bi.GetEncryptionKeyFileVec(), false);
    if (bi.aesKeyandKeySrc.size() != 0)
    {
        for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
        {
            if (partHdr->imageHeader->GetEncryptionKeySrc() == bi.aesKeyandKeySrc[i].first)
            {
                if (aesKey != NULL)
                {
                    memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                }
                break;
            }
        }
    }

    // For copying SCR HDR IV into Boot Header
    const uint32_t* tmpIv = GetIv();
    if (tmpIv != NULL)
    {
        if (partHdr->IsBootloader())
        {
            if (options.secHdrIv == NULL)
            {
                options.secHdrIv = (uint8_t*)malloc(BYTES_PER_IV);
            }
            memcpy_be(options.secHdrIv, tmpIv, BYTES_PER_IV);
        }
        if (partHdr->partitionSecHdrIv == NULL)
        {
            partHdr->partitionSecHdrIv = (uint8_t*)malloc(BYTES_PER_IV);
        }
        memcpy_be(partHdr->partitionSecHdrIv, tmpIv, BYTES_PER_IV);
    }

    uint32_t totalBlocksOverhead = (totalencrBlocks + 1) * 64; //64 = AES_GCM_IV_SZ+AES_GCM_KEY_SZ+NUM_BYTES_PER_WORD+AES_GCM_TAG_SZ

    if (!Binary::CheckAddress(partHdr->partition->section->Length))
    {
        LOG_ERROR("Partition too large to encrypt");
    }

    isBootloader = partHdr->imageHeader->IsBootloader();
    if (options.GetEncryptionDumpFlag())
    {
        bi.options.aesLogFile.open(bi.options.aesLogFilename, std::fstream::app);
        VERBOSE_OUT << std::endl << "------------------------------------";
        VERBOSE_OUT << std::endl << " Partition Name : " << partHdr->partition->section->Name;
        VERBOSE_OUT << std::endl << " Key file       : " << StringUtils::BaseName(aesFilename);
        VERBOSE_OUT << std::endl << "------------------------------------";
    }

    /* Encrypt the stream */
    if (isBootloader && partHdr->imageHeader->GetPmcFwSizeIh() != 0)
    {
        // BootLoader Encryption
        uint32_t encrFsblByteLength;
        uint32_t estimatedEncrFsblLength = partHdr->imageHeader->GetFsblFwSizeIh() + totalBlocksOverhead;
        uint32_t estimatedTotalFsblLength = estimatedEncrFsblLength;
        uint8_t* encrFsblDataBuffer = new uint8_t[estimatedEncrFsblLength];

        uint32_t totalpmcdataencrBlocks = ConfigureEncryptionBlocksforPmcData(bi, partHdr);
        uint32_t estimatedtotalPmcCdoLength = partHdr->imageHeader->GetTotalPmcFwSizeIh() + (totalpmcdataencrBlocks + 1) * 64;

        LOG_INFO("Encrypting Bootloader");

        if (partHdr->imageHeader->GetAuthenticationType() != Authentication::None)
        {
            Binary::Length_t dataChunksCount = 0;
            // PLM
            dataChunksCount = ((estimatedEncrFsblLength - (SECURE_HDR_SZ + AES_GCM_TAG_SZ)) / bi.GetSecureChunkSize(partHdr->imageHeader->IsBootloader())) +
                ((((estimatedEncrFsblLength - (SECURE_HDR_SZ + AES_GCM_TAG_SZ)) % bi.GetSecureChunkSize(partHdr->imageHeader->IsBootloader())) == 0 ? 0 : 1));
            // If PMC CDO present, need to calculate the hash, so include hash
            if (partHdr->imageHeader->GetTotalPmcFwSizeIh())
            {
                estimatedTotalFsblLength += SHA3_LENGTH_BYTES;
            }

            if (dataChunksCount != 1)
            {
                for (uint32_t itr = 0; itr < dataChunksCount - 1; itr++)
                {
                    estimatedTotalFsblLength += SHA3_LENGTH_BYTES;
                }
            }

            // PMC DATA
            dataChunksCount = 0;

            dataChunksCount = ((estimatedtotalPmcCdoLength - (SECURE_HDR_SZ + AES_GCM_TAG_SZ)) / bi.GetSecureChunkSize(partHdr->imageHeader->IsBootloader())) +
                ((((estimatedtotalPmcCdoLength - (SECURE_HDR_SZ + AES_GCM_TAG_SZ)) % bi.GetSecureChunkSize(partHdr->imageHeader->IsBootloader())) == 0 ? 0 : 1));

            if (dataChunksCount != 1)
            {
                for (uint32_t itr = 0; itr < dataChunksCount - 1; itr++)
                {
                    estimatedtotalPmcCdoLength += SHA3_LENGTH_BYTES;
                }
            }
        }

        uint32_t* tmpIvPMCDATA = new uint32_t[WORDS_PER_IV];
        memset(tmpIvPMCDATA, 0, WORDS_PER_IV);

        // PMC Data Encryption
        if (bi.pmcDataAesFile != "")
        {
            //aesKey = NULL;
            aesSeedexits = false;
            fixedInputDataExits = false;
            SetAesFileName(bi.pmcDataAesFile);
            LOG_INFO("Key file - %s", aesFilename.c_str());
            std::ifstream keyFile(aesFilename);
            bool fileExists = keyFile.good();
            if (!fileExists)
            {
                if (bi.aesKeyandKeySrc.size() != 0)
                {
                    for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
                    {
                        if (partHdr->imageHeader->GetEncryptionKeySrc() == bi.aesKeyandKeySrc[i].first)
                        {
                            aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                            memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                            break;
                        }
                    }
                }

                std::vector<uint32_t> blocks = options.bifOptions->GetEncryptionBlocksList();
                options.bifOptions->GetEncryptionBlocksList().clear();
                for (uint32_t itr = 0; itr < options.bifOptions->pmcdataBlocks.size(); itr++)
                {
                    options.bifOptions->InsertEncryptionBlock(options.bifOptions->pmcdataBlocks[itr]);
                }

                GenerateEncryptionKeyFile(aesFilename, options);

                options.bifOptions->GetEncryptionBlocksList().clear();
                for (uint32_t itr = 0; itr < blocks.size(); itr++)
                {
                    options.bifOptions->InsertEncryptionBlock(blocks[itr]);
                }
                blocks.clear();
            }

            std::string iv0 = ReadIV0(aesFilename);
            uint8_t hexData[256];

            if (iv0.size() != (BYTES_PER_IV * 2))
            {
                LOG_DEBUG(DEBUG_STAMP, "IV = %s, IV Size = %d", iv0.c_str(), iv0.size());
                LOG_ERROR("Encryption Error !!!\n           An IV key must be 12 bytes long");
            }
            PackHex(iv0, hexData);

            const uint8_t* iv = hexData;
            for (uint32_t index = 0; index < WORDS_PER_IV; index++)
            {
                tmpIvPMCDATA[index] = ReadBigEndian32(iv);
                iv += sizeof(uint32_t);
            }
        }
        else
        {
            LOG_ERROR("Key Generation Error !!!\n           Key File doesnot exist to encrypt PMC CDO ");
        }

        VersalBootHeaderStructure* bh = (VersalBootHeaderStructure*)bi.bootHeader->section->Data;
        bh->plmLength = partHdr->imageHeader->GetFsblFwSizeIh();
        bh->pmcCdoLength = partHdr->imageHeader->GetTotalPmcFwSizeIh();
        bh->totalPlmLength = estimatedTotalFsblLength + partHdr->imageHeader->GetAuthContext()->GetCertificateSize();
        bh->totalPmcCdoLength = estimatedtotalPmcCdoLength;

        memcpy_be((uint8_t*)bh->plmSecureHdrIv, tmpIv, BYTES_PER_IV);
        memcpy_be((uint8_t*)bh->pmcCdoSecureHdrIv, tmpIvPMCDATA, BYTES_PER_IV);

        if (bi.imageList.size() != 0)
        {
            uint8_t cksumType = 0;
            cksumType = bi.partitionHeaderList.front()->imageHeader->GetChecksumContext()->Type();

            bh->bhAttributes = bi.partitionHeaderList.front()->imageHeader->GetPufHdLocation() << PUF_HD_BIT_SHIFT;
            bh->bhAttributes |= cksumType << BI_HASH_BIT_SHIFT;
            bh->bhAttributes |= bi.partitionHeaderList.front()->imageHeader->GetDpacm() << DPA_CM_BIT_SHIFT;
            bh->bhAttributes |= bi.bifOptions->GetBhRsa() << BH_RSA_BIT_SHIFT;
            bh->bhAttributes |= bi.bifOptions->GetPufMode() << BH_PUF_MODE_BIT_SHIFT;

            if (bi.partitionHeaderList.front()->imageHeader->GetAuthenticationType() != Authentication::None)
            {
                bh->bhAttributes |= BH_RSA_SINGED_BIT_MASK << BH_RSA_SINGED_BIT_SHIFT;
            }
            bh->bhAttributes |= bi.bifOptions->GetDice() << BH_DICE_BIT_SHIFT;
        }

        bh->sourceOffset = sizeof(VersalBootHeaderStructure) + partHdr->imageHeader->GetAuthContext()->GetCertificateSize();
        bh->imageHeaderByteOffset = sizeof(VersalBootHeaderStructure) + bh->totalPlmLength + bh->totalPmcCdoLength;
        bh->headerChecksum = bi.bootHeader->ComputeWordChecksum(&bh->widthDetectionWord, 3872);

        ChunkifyAndEncrypt(options,
            partHdr->partition->section->Data,
            partHdr->imageHeader->GetFsblFwSizeIh(),
            (uint8_t*)bh + sizeof(VersalSmapWidthTable),
            sizeof(VersalBootHeaderStructure) - sizeof(VersalSmapWidthTable),
            encrFsblDataBuffer /* out*/,
            encrFsblByteLength /* out */);

        if (estimatedEncrFsblLength < encrFsblByteLength)
        {
            LOG_ERROR("Encryption Error!!! - PmcFw Buffer overflow");
        }

        // PMC Data Encryption
        totalencrBlocks = totalpmcdataencrBlocks;
        totalBlocksOverhead = (totalencrBlocks + 1) * 64; //64 = AES_GCM_IV_SZ+AES_GCM_KEY_SZ+NUM_BYTES_PER_WORD+AES_GCM_TAG_SZ

        options.bifOptions->GetEncryptionBlocksList().clear();
        for (uint32_t itr = 0; itr < options.bifOptions->pmcdataBlocks.size(); itr++)
        {
            options.bifOptions->InsertEncryptionBlock(options.bifOptions->pmcdataBlocks[itr]);
        }

        if (bi.pmcDataAesFile != "")
        {
            aesKey = aesIv = aesSeed = NULL;
            fixedInputData = NULL;
            SetAesFileName(bi.pmcDataAesFile);
            LOG_INFO("Key file - %s", aesFilename.c_str());
            std::ifstream keyFile(aesFilename);
            bool fileExists = keyFile.good();
            if (!fileExists)
            {
                if (bi.aesKeyandKeySrc.size() != 0)
                {
                    for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
                    {
                        if (partHdr->imageHeader->GetEncryptionKeySrc() == bi.aesKeyandKeySrc[i].first)
                        {
                            aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                            memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                            break;
                        }
                    }
                }
                GenerateEncryptionKeyFile(aesFilename, options);
            }

            bi.InsertEncryptionKeyFile(aesFilename);
            CheckForSameAesKeyFiles(bi.GetEncryptionKeyFileVec());

            ReadEncryptionKeyFile(aesFilename);

            std::pair<KeySource::Type, uint32_t*> asesKeyandKeySrcPair(partHdr->imageHeader->GetEncryptionKeySrc(), aesKey);
            bi.aesKeyandKeySrc.push_back(asesKeyandKeySrcPair);
            bi.bifOptions->CheckForBadKeyandKeySrcPair(bi.aesKeyandKeySrc, aesFilename);

            options.SetDevicePartName(deviceName);
            CheckForExtraKeyIVPairs(totalencrBlocks, partHdr->partition->section->Name);

            if (!aesSeedexits && aesKeyVec.size() != 1)
            {
                if (totalencrBlocks + 1 > aesKeyVec.size())
                {
                    LOG_ERROR("AES Key file has less keys than the number of blocks to be encrypted in %s.", partHdr->partition->section->Name.c_str());
                }
                if (totalencrBlocks + 1 > aesIvVec.size())
                {
                    LOG_ERROR("AES Key file has less IVs than the number of blocks to be encrypted in %s.", partHdr->partition->section->Name.c_str());
                }
            }
            else
            {
                if (bi.aesKeyandKeySrc.size() != 0)
                {
                    for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
                    {
                        if (partHdr->imageHeader->GetEncryptionKeySrc() == bi.aesKeyandKeySrc[i].first)
                        {
                            if (aesKey == NULL)
                            {
                                aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                                memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                            }
                            if (aesKeyVec.size() == 0)
                            {
                                aesKeyVec.push_back(ConvertKeyIvToString((uint8_t *)aesKey, AES_GCM_KEY_SZ).c_str());
                            }
                            break;
                        }
                    }
                }
                GenerateRemainingKeys(options);
            }

            tmpIv = GetIv();
            if (tmpIv != NULL)
            {
                if (options.secHdrIvPmcData == NULL)
                {
                    options.secHdrIvPmcData = (uint8_t*)malloc(BYTES_PER_IV);
                }
                memcpy_be(options.secHdrIvPmcData, tmpIv, BYTES_PER_IV);
            }
        }
        else
        {
            LOG_ERROR("Key Generation Error !!!\n           Key File doesnot exist to encrypt PMC CDO ");
        }

        CheckForRepeatedKeyIVPairs(bi.GetEncryptionKeyFileVec(), false);
        isPmcData = true;
        uint32_t encrPmcByteLength;
        uint32_t estimatedEncrPmcLength = partHdr->imageHeader->GetTotalPmcFwSizeIh() + totalBlocksOverhead;
        uint8_t* encrPmcDataBuffer = new uint8_t[estimatedEncrPmcLength];
        LOG_INFO("Encrypting the PMC Data");

        ChunkifyAndEncrypt(options,
            partHdr->partition->section->Data + partHdr->imageHeader->GetFsblFwSizeIh(),
            partHdr->imageHeader->GetTotalPmcFwSizeIh(),
            NULL, 0,
            encrPmcDataBuffer /* out*/,
            encrPmcByteLength /* out */);

        if (estimatedEncrPmcLength < encrPmcByteLength)
        {
            LOG_ERROR("Encryption Error!!! - PMC Buffer overflow");
        }

        partHdr->partition->section->IncreaseLengthAndPadTo(encrFsblByteLength + encrPmcByteLength, 0x0);
        memset(partHdr->partition->section->Data, 0, encrFsblByteLength + encrPmcByteLength);
        memcpy(partHdr->partition->section->Data, encrFsblDataBuffer, encrFsblByteLength);
        memcpy(partHdr->partition->section->Data + encrFsblByteLength, encrPmcDataBuffer, encrPmcByteLength);

        partHdr->imageHeader->SetTotalPmcFwSizeIh(encrPmcByteLength);
        partHdr->imageHeader->SetTotalFsblFwSizeIh(encrFsblByteLength);
        partHdr->partition->section->Length = encrFsblByteLength + encrPmcByteLength;

        LOG_INFO("Encrypted the partition - %s", partHdr->partition->section->Name.c_str());
        bi.options.CloseEncryptionDumpFile();
        delete[] encrFsblDataBuffer;
        delete[] encrPmcDataBuffer;
        return;
    }
    else if(isBootloader && partHdr->imageHeader->GetPmcFwSizeIh() == 0)
    {
        uint32_t encryptedLength;
        uint32_t estimatedEncrLength = partHdr->partition->section->Length + totalBlocksOverhead;
        uint8_t* encryptedDataBuffer = new uint8_t[estimatedEncrLength];


        uint32_t estimatedTotalFsblLength = estimatedEncrLength;
        if (partHdr->imageHeader->GetAuthenticationType() != Authentication::None)
        {
            Binary::Length_t dataChunksCount = 0;

            dataChunksCount = ((estimatedEncrLength - (SECURE_HDR_SZ + AES_GCM_TAG_SZ)) / bi.GetSecureChunkSize(partHdr->imageHeader->IsBootloader())) +
                ((((estimatedEncrLength - (SECURE_HDR_SZ + AES_GCM_TAG_SZ)) % bi.GetSecureChunkSize(partHdr->imageHeader->IsBootloader())) == 0 ? 0 : 1));

            if (dataChunksCount != 1)
            {
                for (uint32_t itr = 0; itr < dataChunksCount - 1; itr++)
                {
                    estimatedTotalFsblLength += SHA3_LENGTH_BYTES;
                }
            }
        }

        VersalBootHeaderStructure* bh = (VersalBootHeaderStructure*)bi.bootHeader->section->Data;
        bh->plmLength = partHdr->imageHeader->GetFsblFwSizeIh();
        bh->pmcCdoLength = 0;
        bh->totalPlmLength = estimatedTotalFsblLength + partHdr->imageHeader->GetAuthContext()->GetCertificateSize();
        bh->totalPmcCdoLength = 0;

        memcpy_be((uint8_t*)bh->plmSecureHdrIv, tmpIv, BYTES_PER_IV);
        memset((uint8_t*)bh->pmcCdoSecureHdrIv, 0, BYTES_PER_IV);

        if (bi.imageList.size() != 0)
        {
            uint8_t cksumType = 0;
            cksumType = bi.partitionHeaderList.front()->imageHeader->GetChecksumContext()->Type();

            bh->bhAttributes = bi.partitionHeaderList.front()->imageHeader->GetPufHdLocation() << PUF_HD_BIT_SHIFT;
            bh->bhAttributes |= cksumType << BI_HASH_BIT_SHIFT;
            bh->bhAttributes |= bi.partitionHeaderList.front()->imageHeader->GetDpacm() << DPA_CM_BIT_SHIFT;
            bh->bhAttributes |= bi.bifOptions->GetBhRsa() << BH_RSA_BIT_SHIFT;
            bh->bhAttributes |= bi.bifOptions->GetPufMode() << BH_PUF_MODE_BIT_SHIFT;

            if (bi.partitionHeaderList.front()->imageHeader->GetAuthenticationType() != Authentication::None)
            {
                bh->bhAttributes |= BH_RSA_SINGED_BIT_MASK << BH_RSA_SINGED_BIT_SHIFT;
            }
            bh->bhAttributes |= bi.bifOptions->GetDice() << BH_DICE_BIT_SHIFT;
        }
        bh->sourceOffset = sizeof(VersalBootHeaderStructure) + partHdr->imageHeader->GetAuthContext()->GetCertificateSize();
        bh->imageHeaderByteOffset = sizeof(VersalBootHeaderStructure) + bh->totalPlmLength + bh->totalPmcCdoLength;
        bh->headerChecksum = bi.bootHeader->ComputeWordChecksum(&bh->widthDetectionWord, 3872);

        ChunkifyAndEncrypt(options,
            partHdr->partition->section->Data,
            partHdr->imageHeader->GetFsblFwSizeIh(),
            bi.bootHeader->section->Data + sizeof(VersalSmapWidthTable),
            sizeof(VersalBootHeaderStructure) - sizeof(VersalSmapWidthTable),
            encryptedDataBuffer /* out*/,
            encryptedLength /* out */);

        partHdr->partition->section->IncreaseLengthAndPadTo(encryptedLength, 0x0);
        memcpy(partHdr->partition->section->Data, encryptedDataBuffer, encryptedLength);
        partHdr->partition->section->Length = encryptedLength;
        partHdr->imageHeader->SetTotalFsblFwSizeIh(encryptedLength);

        LOG_INFO("Encrypted the partition - %s", partHdr->partition->section->Name.c_str());
        bi.options.CloseEncryptionDumpFile();
        delete[] encryptedDataBuffer;
        return;
    }
    else
    {
        uint32_t encryptedLength;
        uint32_t estimatedEncrLength = partHdr->partition->section->Length + totalBlocksOverhead;
        uint8_t* encryptedDataBuffer = new uint8_t[estimatedEncrLength];

        ChunkifyAndEncrypt(options,
            partHdr->partition->section->Data,
            (uint32_t)partHdr->partition->section->Length,
            NULL, 0,
            encryptedDataBuffer /* out*/,
            encryptedLength /* out */);

        partHdr->partition->section->IncreaseLengthAndPadTo(encryptedLength, 0x0);
        memcpy(partHdr->partition->section->Data, encryptedDataBuffer, encryptedLength);
        partHdr->partition->section->Length = encryptedLength;
        partHdr->imageHeader->SetTotalFsblFwSizeIh(encryptedLength);

        LOG_INFO("Encrypted the partition - %s", partHdr->partition->section->Name.c_str());
        bi.options.CloseEncryptionDumpFile();
        delete[] encryptedDataBuffer;
        return;
    }
}