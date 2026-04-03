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
#include <string.h>

#include "bootimage.h"
#include "binary.h"
#include "stringutils.h"
#include "elftools.h"
#include "partitionheadertable-versal_2ve_2vm.h"
#include "partitionheadertable.h"
#include "checksum-versal.h"
#include "authentication-versal_2ve_2vm.h"
#include "Keccak-compact-versal.h"
#include "bootheader-versal_2ve_2vm.h"

extern "C" {
#include "cdo-npi.h"
#include "cdo-source.h"
#include "cdo-command.h"
#include "cdo-binary.h"
#include "cdo-load.h"
#include "cdo-overlay.h"
#include "fips202.h"
#include "lms-utils.h"
};

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/

/******************************************************************************/
inline void HashSha3_256_CalculateHash(bool flag, const uint8_t *data, size_t length, uint8_t* out)
{
    keccak_state ctx;
    sha3_256_inc_init(ctx);
    sha3_256_inc_absorb(ctx, data, length);
    sha3_256_inc_finalize(out, ctx);
}

/******************************************************************************/
void Versal_2ve_2vmPartition::CalculateChunkificationHash(uint8_t* shaHash, uint8_t* data, size_t inlen, bool padding)
{
    Versalcrypto_hash(shaHash, data, inlen, padding);
}

/******************************************************************************/
void Versal_2ve_2vmPartition::DumpPCRHashes(BootImage & bi)
{
    if (getenv("BOOTGEN_GENERATE_PCR_HASHES") != NULL)
    {
        auto sha_hash = std::make_unique<uint8_t[]>(chunkificationHashLength);
        memset(sha_hash.get(), 0, chunkificationHashLength);
        if (header->partition->section->isBootloader)
        {
            //PLM HASH
            size_t length = header->imageHeader->GetTotalFsblFwSizeIh();
            bi.hash->CalculateVersalHash(true, header->partition->section->Data.get(), length, sha_hash.get());
            LOG_TRACE("PLM_PCR_HASH");
            LOG_DUMP_BYTES(sha_hash.get(), chunkificationHashLength);

            //PMC_DATA HASH
            if (header->imageHeader->GetTotalPmcFwSizeIh() != 0)
            {
                memset(sha_hash.get(), 0, chunkificationHashLength);
                bi.hash->CalculateVersalHash(true, header->partition->section->Data.get() + length, header->imageHeader->GetTotalPmcFwSizeIh(), sha_hash.get());
                LOG_TRACE("PMC_DATA_PCR_HASH");
                LOG_DUMP_BYTES(sha_hash.get(), chunkificationHashLength);
            }
        }
        else
        {
            bi.hash->CalculateVersalHash(true, header->partition->section->Data.get(), header->partition->section->Length, sha_hash.get());
        }
        // sha_hash auto-deletes, no manual delete needed
    }
}

/******************************************************************************/
Versal_2ve_2vmPartition::Versal_2ve_2vmPartition(PartitionHeader* header0, Section* section0)
    : Partition(header0, section0)
    , hashBlockSection(NULL)  // raw pointer, owned by cache
    , hashBlockSectionLength(0)
    , header(header0)
    , firstChunkSize(0)
    , secureChunkSize(SECURE_32K_CHUNK)
    , hashBlockLength(0)
    , pmcdataChunkCount(0)
    , chunkificationHashLength(SHA3_LENGTH_BYTES)
    , totalHashBlockSignatureLength(0)
{
    section = section0;

    // Populate bootloader flag here in the section object - needed in StackAndAlign stage
    section->isBootloader = header->imageHeader->IsBootloader();
    //    header->PartitionDataSize = section0->Length;
}

/******************************************************************************/
Versal_2ve_2vmPartition::Versal_2ve_2vmPartition(PartitionHeader* header0, const uint8_t* data, Binary::Length_t length)
    : Partition(header0, data, length)
    , hashBlockSection(NULL)  // raw pointer, owned by cache
    , hashBlockSectionLength(0)
    , header(header0)
    , firstChunkSize(0)
    , secureChunkSize(SECURE_32K_CHUNK)
    , hashBlockLength(0)
    , pmcdataChunkCount(0)
    , chunkificationHashLength(SHA3_LENGTH_BYTES)
    , totalHashBlockSignatureLength(0)
{
    std::string partition_name = "";
    for (size_t i = 0; i < header->imageHeader->GetFileList().size(); i++)
    {
        partition_name += StringUtils::BaseName(header->imageHeader->GetFileList().at(i));
        if (i != (header->imageHeader->GetFileList().size() - 1))
        {
            partition_name += "_";
        }
    }
    std::string partName = partition_name + StringUtils::Format(".%d", header->index);

    /* Pad binary data with 0s, to get 32-bit word size.
    This is because bootgen stores partition sizes in terms of words, so partition data *MUST* be a multiple of 32-bits */

    /* for everest - Pad the partition with 0's to make it 16 byte alligned */
    int padding = (16 - (length & 15)) & 15;

    Binary::Length_t totallength = length + padding;
    auto temp_section = std::make_unique<Section>(partName, totallength);
    section = temp_section.release();  // Transfer ownership to raw pointer member
    section->index = header->index;
    section->isPartitionData = true;
    section->isBitStream = (header->imageHeader->GetDomain() == Domain::PL) ? true : false;
    section->isFirstElfSection = (header->firstValidIndex);
    section->isBootloader = header->imageHeader->IsBootloader();
    memcpy(section->Data.get(), data, length);
    memset(section->Data.get() + length, 0, padding);
}

/******************************************************************************/
size_t Versal_2ve_2vmPartition::GetTotalDataChunks(Binary::Length_t partitionSize, std::vector<uint32_t>& dataChunks, bool encryptionFlag)
{
    size_t newSectionLength = partitionSize;
    size_t chunkOnlength = 0;
    Binary::Length_t dataChunksCount = 0;

    chunkOnlength = partitionSize;
    if (encryptionFlag)
    {
        chunkOnlength -= (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
    }
    dataChunksCount = (chunkOnlength / secureChunkSize) + (((chunkOnlength % secureChunkSize) == 0 ? 0 : 1));

    if (chunkOnlength % secureChunkSize != 0)
    {
        dataChunks.push_back(((chunkOnlength)-((dataChunksCount - 1) * secureChunkSize)));
    }
    else
    {
        dataChunks.push_back(secureChunkSize);
    }

    for (uint32_t itr = 0; itr < dataChunksCount - 1; itr++)
    {
        dataChunks.push_back(chunkificationHashLength);
        newSectionLength += chunkificationHashLength;
        if (itr == dataChunksCount - 2)
        {
            if (encryptionFlag)
            {
                dataChunks.push_back(secureChunkSize + SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                firstChunkSize = secureChunkSize + SECURE_HDR_SZ + AES_GCM_TAG_SZ;
            }
            else
            {
                dataChunks.push_back(secureChunkSize);
                firstChunkSize = secureChunkSize;
            }
        }
        else
        {
            dataChunks.push_back(secureChunkSize);
        }
    }
    return newSectionLength;
}

/******************************************************************************/
size_t Versal_2ve_2vmPartition::GetBootloaderTotalDataChunks(Binary::Length_t partitionSize, std::vector<uint32_t>& dataChunks, bool encryptionFlag)
{
    size_t newSectionLength = 0;  // Initialize to avoid uninitialized warning

    if(!header->imageHeader->GetPrebuilt()){
        newSectionLength = partitionSize;
    }
    else if (header->imageHeader->GetReplacePmc() || header->imageHeader->GetReplacePlm())
    {
        //newSectionLength = header->imageHeader->GetTotalFsblFwSizeIh() + bi.bifOptions->GetTotalpmcdataSize();
        newSectionLength = header->imageHeader->GetTotalFsblFwSizeIh() + header->imageHeader->GetTotalPmcFwSizeIh();
    }

    size_t chunkOnlength = 0;
    Binary::Length_t dataChunksCount = 0;

    //PMC DATA
    if(!header->imageHeader->GetPrebuilt() || header->imageHeader->GetReplacePmc())
    {
        if (header->imageHeader->GetTotalPmcFwSizeIh() != 0)
        {
            chunkOnlength = header->imageHeader->GetTotalPmcFwSizeIh();
            if (encryptionFlag)
            {
                chunkOnlength -= (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
            }
            dataChunksCount = (chunkOnlength / secureChunkSize) + (((chunkOnlength % secureChunkSize) == 0 ? 0 : 1));
            if (dataChunksCount == 1)
            {
                if (chunkOnlength % secureChunkSize != 0)
                {
                    size_t singleChunklength = (chunkOnlength)-((dataChunksCount - 1) * secureChunkSize);
                    if (encryptionFlag)
                    {
                        dataChunks.push_back(singleChunklength + SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                        firstChunkSize = singleChunklength + SECURE_HDR_SZ + AES_GCM_TAG_SZ;
                        LOG_TRACE("length of pmc data remaining length 0x%X:", singleChunklength + SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                    }
                    else
                    {
                        dataChunks.push_back(singleChunklength);
                        firstChunkSize = singleChunklength;
                        LOG_TRACE("length of pmc data remaining length 0x%X:", singleChunklength);
                    }
                }
                else
                {
                    if (encryptionFlag)
                    {
                        dataChunks.push_back(secureChunkSize + SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                        firstChunkSize = secureChunkSize + SECURE_HDR_SZ + AES_GCM_TAG_SZ;
                    }
                    else
                    {
                        dataChunks.push_back(secureChunkSize);
                        firstChunkSize = secureChunkSize;
                    }
                }
            }
            else
            {
                if (chunkOnlength % secureChunkSize != 0)
                {
                    dataChunks.push_back(((chunkOnlength)-((dataChunksCount - 1) * secureChunkSize)));
                }
                else
                {
                    dataChunks.push_back(secureChunkSize);
                }

                for (uint32_t itr = 0; itr < dataChunksCount - 1; itr++)
                {
                    dataChunks.push_back(chunkificationHashLength);
                    newSectionLength += chunkificationHashLength;
                    header->imageHeader->SetTotalPmcFwSizeIh(header->imageHeader->GetTotalPmcFwSizeIh() + chunkificationHashLength);
                    if (itr == dataChunksCount - 2)
                    {
                        if (encryptionFlag)
                        {
                            dataChunks.push_back(secureChunkSize + SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                            firstChunkSize = secureChunkSize + SECURE_HDR_SZ + AES_GCM_TAG_SZ;
                        }
                        else
                        {
                            dataChunks.push_back(secureChunkSize);
                            firstChunkSize = secureChunkSize;
                        }
                    }
                    else
                    {
                        dataChunks.push_back(secureChunkSize);
                    }
                }
            }
            pmcdataChunkCount = dataChunks.size();
        }
    }
    else{
        //header->imageHeader->SetPmcDataSizeIh(bi.bifOptions->GetPmcFwSize());
        //header->imageHeader->SetTotalPmcFwSizeIh(bi.bifOptions->GetTotalPmcFwSize());
    }

    //PLM
    chunkOnlength = header->imageHeader->GetTotalFsblFwSizeIh();
    if(!header->imageHeader->GetPrebuilt() || header->imageHeader->GetReplacePlm())
    {
        if (chunkOnlength != 0)
        {
            if (encryptionFlag)
            {
                chunkOnlength -= (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
            }
            dataChunksCount = (chunkOnlength / secureChunkSize) + (((chunkOnlength % secureChunkSize) == 0 ? 0 : 1));
            if (dataChunksCount == 1)
            {
                if (chunkOnlength % secureChunkSize != 0)
                {
                    size_t singleChunklength = (chunkOnlength)-((dataChunksCount - 1) * secureChunkSize);
                    if (encryptionFlag)
                    {
                        dataChunks.push_back(singleChunklength + SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                        firstChunkSize = singleChunklength + SECURE_HDR_SZ + AES_GCM_TAG_SZ;
                        LOG_TRACE("length of plm data remaining length 0x%X:", singleChunklength + SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                    }
                    else
                    {
                        dataChunks.push_back(singleChunklength);
                        firstChunkSize = singleChunklength;
                        LOG_TRACE("length of plm data remaining length 0x%X:", singleChunklength);
                    }
                }
                else
                {
                    if (encryptionFlag)
                    {
                        dataChunks.push_back(secureChunkSize + SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                        firstChunkSize = secureChunkSize + SECURE_HDR_SZ + AES_GCM_TAG_SZ;
                        LOG_TRACE("length of plm data remaining length 0x%X:", secureChunkSize + SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                    }
                    else
                    {
                        dataChunks.push_back(secureChunkSize);
                        firstChunkSize = secureChunkSize;
                        LOG_TRACE("length of plm data remaining length 0x%X:", secureChunkSize);
                    }
                }
            }
            else
            {
                if (chunkOnlength % secureChunkSize != 0)
                {
                    dataChunks.push_back(((chunkOnlength)-((dataChunksCount - 1) * secureChunkSize)));
                    LOG_TRACE("length of plm data remaining length 0x%X:", ((chunkOnlength)-((dataChunksCount - 1) * secureChunkSize)));
                }
                else
                {
                    dataChunks.push_back(secureChunkSize);
                    LOG_TRACE("length of plm data remaining length 0x%X:", secureChunkSize);
                }

                for (uint32_t itr = 0; itr < dataChunksCount - 1; itr++)
                {
                    dataChunks.push_back(chunkificationHashLength);
                    newSectionLength += chunkificationHashLength;
                    header->imageHeader->SetTotalFsblFwSizeIh(header->imageHeader->GetTotalFsblFwSizeIh() + chunkificationHashLength);
                    if (itr == dataChunksCount - 2)
                    {
                        if (encryptionFlag)
                        {
                            dataChunks.push_back(secureChunkSize + SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                            firstChunkSize = secureChunkSize + SECURE_HDR_SZ + AES_GCM_TAG_SZ;
                        }
                        else
                        {
                            dataChunks.push_back(secureChunkSize);
                            firstChunkSize = secureChunkSize;
                        }
                    }
                    else
                    {
                        dataChunks.push_back(secureChunkSize);
                    }
                }
            }
        }
    }


    /* Go to BH_HASH_BLOCK_BYTES_* for details */
    hashBlockLength = BH_HASH_BLOCK_BYTES_TELLURIDE;

    // Hash Block length is not added to partition length
    header->imageHeader->SetTotalFsblFwSizeIh(header->imageHeader->GetTotalFsblFwSizeIh());
    
    if (header->imageHeader->GetAuthenticationType() != Authentication::None)
    {
        newSectionLength += totalHashBlockSignatureLength;
    }

    newSectionLength += hashBlockLength;
    
    if (encryptionFlag && (header->imageHeader->GetAuthenticationType() == Authentication::None))
    {
        newSectionLength += AES_GCM_TAG_SZ;
    }

    return newSectionLength;
}

/******************************************************************************/
void Versal_2ve_2vmPartition::ChunkifyAndHash(Section* section, bool encryptionFlag)
{
    if(!header->imageHeader->GetPrebuilt()){
        //continue;
    }
    else if(section->isBootloader && (header->imageHeader->GetReplacePmc() || header->imageHeader->GetReplacePlm())){
        //If partition is bootloader and either plm or pmc needs to be replaced
    }
    else if (!section->isBootloader && header->imageHeader->GetReplacePsm())
    {
        //If partition is psm and needs to be replaced
    }
    else{
        return;
    }
    //Need to bring Hash Context here or bi//
    std::vector<uint32_t> dataChunks;
    std::vector<uint8_t*> data;
    int tempBufferSize = 0;
    /* Get the number/size data chunks and claculate the new section length */
    size_t length = section->Length;
    size_t newLength = 0;
    bool checksum_bootloader = false;
    auto pmcDataHash = std::make_unique<uint8_t[]>(chunkificationHashLength);

    auto partitionHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
    memset(partitionHash.get(), 0, chunkificationHashLength);

    if (section->isBootloader)
    {
        if (header->imageHeader->GetChecksumContext()->Type() != Checksum::None
            || header->imageHeader->GetAuthenticationType() != Authentication::None
            || encryptionFlag != false)
        {
            /* If checksum is enabled for VersalNet bootloader, then add the final checksum here only.
            VersalChecksumTable::Build & VersalChecksumTable::Link will not process anything for bootloader checksum */
            checksum_bootloader = true;
        }
        newLength = GetBootloaderTotalDataChunks(length, dataChunks, encryptionFlag);
    }
    else
    {
        if(!header->imageHeader->GetPrebuilt() || header->imageHeader->GetReplacePsm())
        {
            newLength = GetTotalDataChunks(length, dataChunks, encryptionFlag);
        }
    }

    /* Form a new section - Divide into data chunks of 64K and add sha pad - calculate hash to each chunk - prepend hash to chunk+shapad to form a complete partition. */
    if (newLength < length)
    {
        LOG_DEBUG(DEBUG_STAMP, "Resize length is less than original length");
        LOG_ERROR("Section resize issue for authentication");
    }

    auto dataPtr = std::make_unique<uint8_t[]>(length);
    memset(dataPtr.get(), 0, length);
    memcpy(dataPtr.get(), section->Data.get(), length);
    uint8_t* dataPtrRaw = dataPtr.get() + length;

    auto newDataPtr = std::make_unique<uint8_t[]>(newLength);
    memset(newDataPtr.get(), 0, newLength);
    uint8_t* newDataPtrRaw = newDataPtr.get() + newLength;

    std::unique_ptr<uint8_t[]> shaHash;

    //PMC DATA
    int itr = 0;
    if(!header->imageHeader->GetPrebuilt() || header->imageHeader->GetReplacePmc())
    {
        if (header->imageHeader->GetTotalPmcFwSizeIh() != 0)
        {
            if (pmcdataChunkCount == 1)
            {
                tempBufferSize = dataChunks[itr];
                auto tempBuffer = std::make_unique<uint8_t[]>(tempBufferSize);
                memset(tempBuffer.get(), 0, tempBufferSize);

                dataPtrRaw -= tempBufferSize;
                memcpy(tempBuffer.get(), dataPtrRaw, tempBufferSize);
                newDataPtrRaw -= tempBufferSize;

                memcpy(newDataPtrRaw, tempBuffer.get(), tempBufferSize);
                // unique_ptr handles deletion

                shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
                CalculateChunkificationHash(shaHash.get(), newDataPtrRaw, tempBufferSize, true);

                memcpy(pmcDataHash.get(), shaHash.get(), chunkificationHashLength);
                LOG_TRACE("PMC DATA HASH");
                LOG_DUMP_BYTES(pmcDataHash.get(), chunkificationHashLength);
            }
            else
            {
                itr = (pmcdataChunkCount - 3);

                /*-------------------------------CHUNK N------------------------------------*/
                /* Insert Data */
                tempBufferSize = dataChunks[0];
                auto tempBuffer = std::make_unique<uint8_t[]>(tempBufferSize);
                memset(tempBuffer.get(), 0, tempBufferSize);

                dataPtrRaw -= tempBufferSize;
                memcpy(tempBuffer.get(), dataPtrRaw, tempBufferSize);
                newDataPtrRaw -= tempBufferSize;

                memcpy(newDataPtrRaw, tempBuffer.get(), tempBufferSize);
                // unique_ptr handles deletion

                /* Calculate hash */
                shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
                CalculateChunkificationHash(shaHash.get(), newDataPtrRaw, dataChunks[0], true);
                /*-------------------------------CHUNK N------------------------------------*/

                for (int i = 2; i <= itr; i += 2)
                {
                    /* Insert previous hash */
                    newDataPtrRaw -= chunkificationHashLength;
                    memcpy(newDataPtrRaw, shaHash.get(), chunkificationHashLength);

                    /* Insert Data */
                    tempBufferSize = dataChunks[i];
                    auto tempBuffer = std::make_unique<uint8_t[]>(tempBufferSize);
                    memset(tempBuffer.get(), 0, tempBufferSize);

                    dataPtrRaw -= tempBufferSize;
                    memcpy(tempBuffer.get(), dataPtrRaw, tempBufferSize);
                    newDataPtrRaw -= tempBufferSize;

                    memcpy(newDataPtrRaw, tempBuffer.get(), tempBufferSize);
                    // unique_ptr handles deletion

                    /* Calculate hash */
                    shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
                    CalculateChunkificationHash(shaHash.get(), newDataPtrRaw, dataChunks[i] + dataChunks[i - 1], true);
                }

                /*-------------------------------CHUNK 1------------------------------------*/
                itr += 2;

                /* Insert previous hash */
                newDataPtrRaw -= chunkificationHashLength;
                memcpy(newDataPtrRaw, shaHash.get(), chunkificationHashLength);

                /* Insert Data */
                tempBufferSize = dataChunks[itr];
                {
                    auto tempBuffer = std::make_unique<uint8_t[]>(tempBufferSize);
                    memset(tempBuffer.get(), 0, tempBufferSize);

                    dataPtrRaw -= tempBufferSize;
                    memcpy(tempBuffer.get(), dataPtrRaw, tempBufferSize);
                    newDataPtrRaw -= tempBufferSize;

                    memcpy(newDataPtrRaw, tempBuffer.get(), tempBufferSize);
                    // unique_ptr handles deletion
                }
                /*-------------------------------CHUNK 1------------------------------------*/

                /* Calculate hash of top chunk and previous hash */
                shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
                CalculateChunkificationHash(shaHash.get(), newDataPtrRaw, tempBufferSize + chunkificationHashLength, true);

                memcpy(pmcDataHash.get(), shaHash.get(), chunkificationHashLength);
                LOG_TRACE("PMC DATA HASH");
                LOG_DUMP_BYTES(pmcDataHash.get(), chunkificationHashLength);
            }
        }
    }
    else
	{
        dataPtrRaw -= header->imageHeader->GetTotalPmcFwSizeIh();
        shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
        CalculateChunkificationHash(shaHash.get(), dataPtrRaw, secureChunkSize + chunkificationHashLength, true);
        memcpy(pmcDataHash.get(), shaHash.get(), chunkificationHashLength);
        LOG_TRACE("PMC DATA HASH");
        LOG_DUMP_BYTES(pmcDataHash.get(), chunkificationHashLength);
        dataPtrRaw += header->imageHeader->GetTotalPmcFwSizeIh();

        tempBufferSize = header->imageHeader->GetTotalPmcFwSizeIh();
        //tempBufferSize = header->imageHeader->GetPmcFwSizeIh();
        auto tempBuffer = std::make_unique<uint8_t[]>(tempBufferSize);
        memset(tempBuffer.get(), 0, tempBufferSize);

        dataPtrRaw -= tempBufferSize;
        memcpy(tempBuffer.get(), dataPtrRaw, tempBufferSize);
        newDataPtrRaw -= tempBufferSize;

        memcpy(newDataPtrRaw, tempBuffer.get(), tempBufferSize);
        // unique_ptr handles deletion
    }
    dataChunks.erase(dataChunks.begin(), dataChunks.begin() + pmcdataChunkCount);
    //PLM
    if(!header->imageHeader->GetPrebuilt() || (header->imageHeader->GetReplacePlm() || header->imageHeader->GetReplacePsm()))
    {
        if (dataChunks.size() != 0)
        {
            itr = 0;
            if (dataChunks.size() == 1)
            {
                tempBufferSize = dataChunks[itr];
                auto tempBuffer = std::make_unique<uint8_t[]>(tempBufferSize);
                memset(tempBuffer.get(), 0, tempBufferSize);

                dataPtrRaw -= tempBufferSize;
                memcpy(tempBuffer.get(), dataPtrRaw, tempBufferSize);
                newDataPtrRaw -= tempBufferSize;

                memcpy(newDataPtrRaw, tempBuffer.get(), tempBufferSize);
                // unique_ptr handles deletion

                shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
                CalculateChunkificationHash(shaHash.get(), newDataPtrRaw, tempBufferSize, true);

                memcpy(partitionHash.get(), shaHash.get(), chunkificationHashLength);
                if (section->isBootloader)
                {
                    LOG_TRACE("PLM HASH");
                    LOG_DUMP_BYTES(partitionHash.get(), chunkificationHashLength);
                }
                //LOG_TRACE("Partition Data HASHed");
                //LOG_DUMP_BYTES(newDataPtrRaw, tempBufferSize);
            }
            else
            {
                itr = (dataChunks.size() - 3);

                /*-------------------------------CHUNK N------------------------------------*/
                /* Insert Data */
                tempBufferSize = dataChunks[0];
                auto tempBuffer = std::make_unique<uint8_t[]>(tempBufferSize);
                memset(tempBuffer.get(), 0, tempBufferSize);

                dataPtrRaw -= tempBufferSize;
                memcpy(tempBuffer.get(), dataPtrRaw, tempBufferSize);
                newDataPtrRaw -= tempBufferSize;

                memcpy(newDataPtrRaw, tempBuffer.get(), tempBufferSize);
                // unique_ptr handles deletion

                /* Calculate hash */
                shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
                CalculateChunkificationHash(shaHash.get(), newDataPtrRaw, dataChunks[0], true);
                /*-------------------------------CHUNK N------------------------------------*/

                for (int i = 2; i <= itr; i += 2)
                {
                    /* Insert previous hash */
                    newDataPtrRaw -= chunkificationHashLength;
                    memcpy(newDataPtrRaw, shaHash.get(), chunkificationHashLength);

                    /* Insert Data */
                    tempBufferSize = dataChunks[i];
                    auto tempBuffer = std::make_unique<uint8_t[]>(tempBufferSize);
                    memset(tempBuffer.get(), 0, tempBufferSize);

                    dataPtrRaw -= tempBufferSize;
                    memcpy(tempBuffer.get(), dataPtrRaw, tempBufferSize);
                    newDataPtrRaw -= tempBufferSize;

                    memcpy(newDataPtrRaw, tempBuffer.get(), tempBufferSize);
                    // unique_ptr handles deletion

                    /* Calculate hash */
                    shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
                    CalculateChunkificationHash(shaHash.get(), newDataPtrRaw, dataChunks[i] + dataChunks[i - 1], true);
                }

                /*-------------------------------CHUNK 1------------------------------------*/
                itr += 2;

                /* Insert previous hash */
                newDataPtrRaw -= chunkificationHashLength;
                memcpy(newDataPtrRaw, shaHash.get(), chunkificationHashLength);

                /* Insert Data */
                tempBufferSize = dataChunks[itr];
                {
                    auto tempBuffer = std::make_unique<uint8_t[]>(tempBufferSize);
                    memset(tempBuffer.get(), 0, tempBufferSize);

                    dataPtrRaw -= tempBufferSize;
                    memcpy(tempBuffer.get(), dataPtrRaw, tempBufferSize);
                    newDataPtrRaw -= tempBufferSize;

                    memcpy(newDataPtrRaw, tempBuffer.get(), tempBufferSize);
                    // unique_ptr handles deletion
                }
                /*-------------------------------CHUNK 1------------------------------------*/

                /* Calculate hash of top chunk and previous hash */
                shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
                CalculateChunkificationHash(shaHash.get(), newDataPtrRaw, tempBufferSize + chunkificationHashLength, true);

                memcpy(partitionHash.get(), shaHash.get(), chunkificationHashLength);
                if (section->isBootloader)
                {
                    LOG_TRACE("PLM HASH");
                    LOG_DUMP_BYTES(partitionHash.get(), chunkificationHashLength);
                }
                //LOG_TRACE("PLM Data HASHed");
                //LOG_DUMP_BYTES(newDataPtr, tempBufferSize + chunkificationHashLength);
            }
        }
    }
    else
	{
        if(section->isBootloader && !header->imageHeader->GetReplacePlm())
        {
            dataPtrRaw -= header->imageHeader->GetTotalFsblFwSizeIh();
            shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
            CalculateChunkificationHash(shaHash.get(), dataPtrRaw, secureChunkSize + chunkificationHashLength, true);
            memcpy(partitionHash.get(), shaHash.get(), chunkificationHashLength);
            if (section->isBootloader)
            {
                LOG_TRACE("PLM HASH");
                LOG_DUMP_BYTES(partitionHash.get(), chunkificationHashLength);
            }
            dataPtrRaw += header->imageHeader->GetTotalFsblFwSizeIh();

            tempBufferSize = header->imageHeader->GetTotalFsblFwSizeIh();
            auto tempBuffer = std::make_unique<uint8_t[]>(tempBufferSize);
            memset(tempBuffer.get(), 0, tempBufferSize);

            dataPtrRaw -= tempBufferSize;
            memcpy(tempBuffer.get(), dataPtrRaw, tempBufferSize);
            newDataPtrRaw -= tempBufferSize;

            memcpy(newDataPtrRaw, tempBuffer.get(), tempBufferSize);
            // unique_ptr handles deletion
        }
        else if(!header->imageHeader->GetReplacePsm())
        {
            dataPtrRaw -= length;
            shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
            CalculateChunkificationHash(shaHash.get(), dataPtrRaw, secureChunkSize + chunkificationHashLength, true);
            memcpy(partitionHash.get(), shaHash.get(), chunkificationHashLength);
            dataPtrRaw += length;

            tempBufferSize = length;
            auto tempBuffer = std::make_unique<uint8_t[]>(tempBufferSize);
            memset(tempBuffer.get(), 0, tempBufferSize);

            dataPtrRaw -= tempBufferSize;
            memcpy(tempBuffer.get(), dataPtrRaw, tempBufferSize);
            newDataPtrRaw -= tempBufferSize;

            memcpy(newDataPtrRaw, tempBuffer.get(), tempBufferSize);
            // unique_ptr handles deletion
        }
    }
    if (checksum_bootloader)
    {
        /* GCM Tag sits in the end of Hash Block */
        if (encryptionFlag && (header->imageHeader->GetAuthenticationType() == Authentication::None))
        {
            newDataPtrRaw -= AES_GCM_TAG_SZ;
        }

        if (header->imageHeader->GetAuthenticationType() != Authentication::None)
        {
            newDataPtrRaw -= totalHashBlockSignatureLength;
        }

        newDataPtrRaw -= HASH_BLOCK_ALIGNMENT_BYTES_TELLURIDE;
        newDataPtrRaw -= (3 * (chunkificationHashLength + HASH_BLOCK_INDEX_BYTES)); //Reserved Hashes

        uint32_t hashIndex = 0;

        if (header->imageHeader->GetTotalPmcFwSizeIh() != 0)
        {
            /* Place the PMC DATA hash at the third position in Hash Block of PLM partition */
            newDataPtrRaw -= chunkificationHashLength;
            memcpy(newDataPtrRaw, pmcDataHash.get(), chunkificationHashLength);

            hashIndex = HASH_BLOCK_PMCDATA_HASH_INDEX;
            newDataPtrRaw -= HASH_BLOCK_INDEX_BYTES;
            memcpy(newDataPtrRaw, &hashIndex, sizeof(uint32_t));
        }
        else
        {
            newDataPtrRaw -= chunkificationHashLength;
            newDataPtrRaw -= HASH_BLOCK_INDEX_BYTES;
        }

        /* Place the final hash at the second position in Hash Block of PLM partition */
        newDataPtrRaw -= chunkificationHashLength;
        memcpy(newDataPtrRaw, partitionHash.get(), chunkificationHashLength);

        hashIndex = HASH_BLOCK_PLM_HASH_INDEX;
        newDataPtrRaw -= HASH_BLOCK_INDEX_BYTES;
        memcpy(newDataPtrRaw, &hashIndex, sizeof(uint32_t));

        /* Place the BH hash at the start of Hash Block of PLM partition */
        newDataPtrRaw -= chunkificationHashLength;
        newDataPtrRaw -= HASH_BLOCK_INDEX_BYTES;
    }
    // unique_ptr handles deletion

    section->Data = std::unique_ptr<uint8_t[]>(newDataPtr.release());
    section->Length = newLength;

    LOG_TRACE("First Authentication Data Chunk Size 0x%X", firstChunkSize);
}

/******************************************************************************/
void Versal_2ve_2vmPartition::Build(BootImage& bi, Binary& cache)
{
    chunkificationHashLength = bi.hash->GetHashLength();
    //LOG_TRACE("chunkificationHashLength %d", chunkificationHashLength);

    secureChunkSize = bi.GetSecureChunkSize(this->header->imageHeader->IsBootloader());
    /* Get the image header from this partition header */
    ImageHeader& imageHeader(*this->header->imageHeader);
    /* Get the contexts for Authentication & Encryption */
    AuthenticationContext* currentAuthCtx = imageHeader.GetAuthContext();
    EncryptionContext* encryptCtx = imageHeader.GetEncryptContext();

    totalHashBlockSignatureLength = currentAuthCtx->GetTotalHashBlockSignSize();

    /* Set default values
       Set the address for relative placement for subsequence partitions in image
       Don't reserve anything for partition sections */
    if (!bi.XipMode)
    {
        section->Address = 0;
        section->Reserve = 0;
    }

    /* If the partition is a bootloader & XIP mode is enabled, then address is directly
       populated based on the QSPI execution address.  */
    if (!(imageHeader.IsBootloader() && bi.XipMode))
    {
        /* The offset and reserve attributes only apply to first partition in image
           subsequent partitions in the same image are packed right after another */
        if (header->IsFirstPartitionInImage())
        {
            section->Address = imageHeader.GetOffset().ValueOrDefault(0);
            section->Reserve = imageHeader.GetReserve().ValueOrDefault(0);
        }
        else
        {
            section->continuation = true;
        }
    }
    section->Alignment = imageHeader.GetAlignment().ValueOrDefault(0);

    /* If the head and tail of the partition destination are not on a word boundary
       we will prepad the data so that DMA can more easily copy the bulk of the data
       a whole word at a time. The head and tail bytes are treated seperately */
    if (!header->prealigned)
    {
        /* Use the starting load address to get the number mis-align word boundry unused bytes at the beginning
           of a Partition data.
           Byte Addr        Word Bytes              headAlignment
               0           00 00 00 00                0
               1           XX 00 00 00                1
               2           XX XX 00 00                2
               3           XX XX XX 00                3                  */
        if ((header->imageHeader->GetDomain() != Domain::PL) && (header->imageHeader->GetPartitionType() != PartitionType::CONFIG_DATA_OBJ) &&
            (header->imageHeader->GetPartitionType() != PartitionType::CFI))
        {
            header->headAlignment = header->loadAddress % sizeof(uint32_t);
            /* Use the ending load address to get the number mis-align word boundry unused bytes at the end
               of a Partition data.
               Byte Addr       Word Bytes               tailAlignment
                   0    (1)   00 XX XX XX                   3
                   1    (2)   00 00 XX XX                   2
                   2    (3)   00 00 00 XX                   1
                   3    (0)   00 00 00 00                   0 */
            uint32_t remainder = (header->loadAddress + section->Length) % sizeof(uint32_t);
            header->tailAlignment = (remainder == 0) ? 0 : (4 - remainder);
        }
        section->PadToWordAlignment(header->headAlignment, header->tailAlignment);
    }

    /* Encryption process on the partition */
    if (header->preencrypted && encryptCtx->Type() != Encryption::None)
    {
        LOG_ERROR("Cannot reencrypt a partition that is already encrypted for %s", section->Name.c_str());
    }

    /*******************************************************************************/
    if ((imageHeader.GetPartitionType() == PartitionType::CONFIG_DATA_OBJ) && (encryptCtx->Type() != Encryption::None))
    {
        size_t buffer_size = 0;
        uint8_t num_of_sync_points = 0;

        CdoSequence * cdo_seq;
        cdo_seq = decode_cdo_binary(header->partition->section->Data.get(), header->partition->section->Length);

        /* Enable the search for sync points - only needs to be done for SSIT devices */
        search_for_sync_points();

        // Wrap C-allocated pointers in unique_ptr for automatic cleanup
        std::unique_ptr<uint8_t[]> buffer((uint8_t*)cdoseq_to_binary(cdo_seq, &buffer_size, 0));

        /* Get no. of sync points and sync points offsets */
        num_of_sync_points = get_num_of_sync_points();
        std::unique_ptr<uint32_t[]> syncpt_offsets(get_slr_sync_point_offsets());

        for (int i = 0; i < num_of_sync_points; i++)
        {
            size_t offset = (syncpt_offsets[i] * 4);  // Use array indexing instead of pointer arithmetic
            bi.sync_offsets.push_back(offset);
        }
        // Auto-deleted by unique_ptr destructors
    }
    /*******************************************************************************/
    
    DumpPCRHashes(bi);

    encryptCtx->ChunkifyAndProcess(bi, header);
    
    uint32_t padLength = 0;

    /* Authentication process on the partition */
    if (header->presigned)
    {
        // do nothing
        for (std::list<AuthenticationCertificate*>::iterator acs = header->ac.begin(); acs != header->ac.end(); acs++)
        {
            *acs = 0;
        }
    }
    else
    {
        if(!header->imageHeader->GetPrebuilt() || (header->imageHeader->GetReplacePmc() || header->imageHeader->GetReplacePlm() || header->imageHeader->GetReplacePsm()))
        header->transferSize = section->Length;
        //if ((imageHeader.GetChecksumContext()->Type() == Checksum::SHA3) || (currentAuthCtx->authAlgorithm->Type() != Authentication::None))
        {
            /* No chunking on bootloader for versal - Data should be alligned to 104 bytes before calculating the hash */
            if (imageHeader.IsBootloader())
            {
                if (header->imageHeader->GetTotalPmcFwSizeIh() != 0)
                {
                    ChunkifyAndHash(section, (encryptCtx->Type() != Encryption::None || header->preencrypted));
                    if(header->imageHeader->GetPrebuilt())
                    {
                        firstChunkSize = secureChunkSize;
                        hashBlockLength = BH_HASH_BLOCK_BYTES_TELLURIDE;
                    }
                    currentAuthCtx->SetFirstChunkSize(firstChunkSize);
                    header->firstChunkSize = currentAuthCtx->GetFirstChunkSize();
                    header->partition->section->firstChunkSize = header->firstChunkSize;
                }
                else
                {
                    Binary::Length_t chunkOnLength = header->partition->section->Length;
                    if (encryptCtx->Type() != Encryption::None || header->preencrypted)
                    {
                        chunkOnLength -= (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                    }
                    else if (imageHeader.GetChecksumContext()->Type() != Checksum::None)
                    {
                        /* Checksum length is not added to the partition length, in case of bootloader for VersalNet.
                        Hence don't substract the checksum length, since it should not be included in hash calculation */
                        //chunkOnLength -= imageHeader.GetChecksumContext()->Size();
                    }
                    Binary::Length_t dataChunksCount = (chunkOnLength / secureChunkSize) + ((((chunkOnLength) % secureChunkSize) == 0 ? 0 : 1));
                    if (dataChunksCount != 1)
                    {
                        ChunkifyAndHash(section, (encryptCtx->Type() != Encryption::None || header->preencrypted));
                        if(header->imageHeader->GetPrebuilt())
                        {    
                            firstChunkSize = secureChunkSize;
                        }
                        currentAuthCtx->SetFirstChunkSize(firstChunkSize);
                        header->firstChunkSize = currentAuthCtx->GetFirstChunkSize();
                        header->partition->section->firstChunkSize = header->firstChunkSize;
                    }
                    else if (dataChunksCount == 1 && (imageHeader.GetChecksumContext()->Type() != Checksum::None
                        || header->imageHeader->GetAuthenticationType() != Authentication::None
                        || encryptCtx->Type() != Encryption::None))
                    {
                        hashBlockLength = BH_HASH_BLOCK_BYTES_TELLURIDE;
                        
                        size_t newLength = section->Length + hashBlockLength;
                        if ((encryptCtx->Type() != Encryption::None) && currentAuthCtx->authAlgorithm->Type() == Authentication::None)
                        {
                            newLength += AES_GCM_TAG_SZ;
                        }
                        if (header->imageHeader->GetAuthenticationType() != Authentication::None)
                        {
                            newLength += totalHashBlockSignatureLength;
                        }
                        auto newDataPtr = std::make_unique<uint8_t[]>(newLength);
                        memset(newDataPtr.get(), 0, newLength);

                        uint8_t* newDataPtrRaw = newDataPtr.get() + newLength;
                        newDataPtrRaw += hashBlockLength;
                        if ((encryptCtx->Type() != Encryption::None) && currentAuthCtx->authAlgorithm->Type() == Authentication::None)
                        {
                            newDataPtrRaw += AES_GCM_TAG_SZ;
                        }
                        if (header->imageHeader->GetAuthenticationType() != Authentication::None)
                        {
                            newDataPtrRaw += totalHashBlockSignatureLength;
                        }

                        memcpy(newDataPtrRaw, section->Data.get(), section->Length);

                        /* Calculate hash */
                        auto shaHash_local = std::make_unique<uint8_t[]>(chunkificationHashLength);
                        
                        CalculateChunkificationHash(shaHash_local.get(), newDataPtrRaw, section->Length, true);
                        LOG_TRACE("PLM HASH");
                        LOG_DUMP_BYTES(shaHash_local.get(), chunkificationHashLength);
                        //LOG_TRACE("PLM Data HASHed");
                        //LOG_DUMP_BYTES(newDataPtr, section->Length);

                        /* GCM Tag sits in the end of Hash Block */
                        if ((encryptCtx->Type() != Encryption::None) && currentAuthCtx->authAlgorithm->Type() == Authentication::None)
                        {
                            newDataPtrRaw -= AES_GCM_TAG_SZ;
                        }

                        /* Place the final hash at the start of PLM partition */
                        if (header->imageHeader->GetAuthenticationType() != Authentication::None)
                        {
                            newDataPtrRaw -= totalHashBlockSignatureLength;
                        }

                        newDataPtrRaw -= HASH_BLOCK_ALIGNMENT_BYTES_TELLURIDE;
                        newDataPtrRaw -= (4 * (chunkificationHashLength + HASH_BLOCK_INDEX_BYTES)); //Reserved Hashes, including pmc data hash

                        newDataPtrRaw -= chunkificationHashLength;
                        memcpy(newDataPtrRaw, shaHash_local.get(), chunkificationHashLength);

                        uint32_t hashIndex = HASH_BLOCK_PLM_HASH_INDEX;
                        newDataPtrRaw -= HASH_BLOCK_INDEX_BYTES;
                        memcpy(newDataPtrRaw, &hashIndex, sizeof(uint32_t));

                        header->imageHeader->SetTotalFsblFwSizeIh(header->imageHeader->GetTotalFsblFwSizeIh());

                        /* For BH Hash */
                        newDataPtrRaw -= chunkificationHashLength;
                        newDataPtrRaw -= HASH_BLOCK_INDEX_BYTES;
                        // unique_ptr handles deletion
                        section->Data = std::move(newDataPtr);
                        section->Length = newLength;
                    }
                }

                if (bi.bifOptions->GetPmcdataFile() == "")
                {
                    //imageHeader.SetTotalFsblFwSizeIh(section->Length);
                }
                else
                {
                    //imageHeader.SetTotalPmcFwSizeIh(imageHeader.GetTotalPmcFwSizeIh() + shaPadLength);
                }
                if(!header->imageHeader->GetPrebuilt() || header->imageHeader->GetReplacePmc()
                    || header->imageHeader->GetReplacePlm())
                    header->transferSize = section->Length - imageHeader.GetChecksumContext()->Size();
                /* Checksum length is added to the partition length, in case of bootloader.
                Partiton length should not include checksum length, so substarct*/
            }
            /* Chunk the data into fixed 64KB/32KB */
            else
            {
                Binary::Length_t chunkOnLength = header->partition->section->Length;
                if (encryptCtx->Type() != Encryption::None || header->preencrypted)
                {
                    chunkOnLength -= (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                }
                Binary::Length_t dataChunksCount = (chunkOnLength / secureChunkSize) + ((((chunkOnLength) % secureChunkSize) == 0 ? 0 : 1));

                auto shaHash = std::make_unique<uint8_t[]>(chunkificationHashLength);
                auto hash = std::make_unique<uint8_t[]>(chunkificationHashLength);
                
                // IMAGE_STORE: Use adjusted partition number in hashTable when no bootloader
                uint32_t hashTablePartNum = header->partitionNum;
                if(bi.IsBootloaderFound() == false)
                    hashTablePartNum = header->partitionNum + 1;
                    
                bi.hashTable.push_back(std::make_pair(hashTablePartNum, std::move(hash)));


                if (dataChunksCount != 1)
                {
                    ChunkifyAndHash(section, (encryptCtx->Type() != Encryption::None || header->preencrypted));
                    if(header->imageHeader->GetPrebuilt())
                        firstChunkSize = secureChunkSize;
                    currentAuthCtx->SetFirstChunkSize(firstChunkSize);
                    header->firstChunkSize = currentAuthCtx->GetFirstChunkSize();
                    header->partition->section->firstChunkSize = header->firstChunkSize;

                    CalculateChunkificationHash(shaHash.get(), section->Data.get(), firstChunkSize + chunkificationHashLength, true);
                }
                if (dataChunksCount == 1)
                {
                    CalculateChunkificationHash(shaHash.get(), section->Data.get(), section->Length, true);
                }

                memcpy(bi.hashTable.back().second.get(), shaHash.get(), chunkificationHashLength);
                LOG_TRACE("%s , Partition Index : %d \n           Hash :", header->section->Name.c_str(), header->partitionNum);
                LOG_DUMP_BYTES(bi.hashTable.back().second.get(), chunkificationHashLength);
            }
        }
        
        // AC creation moved to after hashBlockSection creation (line ~1242)
    }

    if (imageHeader.IsBootloader() == true)
    {
        if (encryptCtx->Type() != Encryption::None && currentAuthCtx->authAlgorithm->Type() == Authentication::None)
        {
            uint8_t* tmpBh = bi.bootHeader->section->Data.get() + 0x10;
            auto sha_hash = std::make_unique<uint8_t[]>(chunkificationHashLength);

            Versal_2ve_2vmBootHeaderStructure* bh = (Versal_2ve_2vmBootHeaderStructure*)bi.bootHeader->section->Data.get();
            bh->sourceOffset = bi.bootHeader->section->Length + hashBlockLength + AES_GCM_TAG_SZ;
            bh->hashBlockLength1 = hashBlockLength;
            bh->totalPlmLength = header->imageHeader->GetTotalFsblFwSizeIh();
            bh->imageHeaderByteOffset = bi.bootHeader->section->Length + hashBlockLength + AES_GCM_TAG_SZ + bh->totalPlmLength + bh->totalPmcCdoLength;
            bh->headerChecksum = bi.bootHeader->ComputeWordChecksum(&bh->widthDetectionWord, bi.bootHeader->GetBHChecksumDataSize());
            bi.hash->CalculateVersalHash(true, tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(Versal_2ve_2vmSmapWidthTable), sha_hash.get());

            LOG_TRACE("BH before AAD");
            LOG_DUMP_BYTES(tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(Versal_2ve_2vmSmapWidthTable));
            /* Copy BH Hash */
            memcpy(header->partition->section->Data.get() + HASH_BLOCK_INDEX_BYTES, sha_hash.get(), chunkificationHashLength);
            LOG_TRACE("hash block 0");
            LOG_DUMP_BYTES(header->partition->section->Data.get(), hashBlockLength);

            encryptCtx->AesGcm256HashBlockEncrypt(bi.options, header->partition->section->Data.get(),
                hashBlockLength, header->partition->section->Data.get() + hashBlockLength, 2);

            LOG_TRACE("GCM Tag + Hash Block 0");
            LOG_DUMP_BYTES(header->partition->section->Data.get(), hashBlockLength + AES_GCM_TAG_SZ);
        }
        /* FsblFwSizeIh : PLM */
        /* PmcFwSizeIh : PMC DATA */
        if (bi.bifOptions->GetPmcdataFile() == "")
        {
            bi.SetTotalFsblFwSize(imageHeader.GetTotalFsblFwSizeIh() + padLength);
            //below for everest
            //bi.SetTotalFsblFwSize(imageHeader.getTotalFsblFwSizeIh());
            bi.bifOptions->SetTotalPmcFwSize(imageHeader.GetTotalPmcFwSizeIh());
        }
        else
        {
            bi.SetTotalFsblFwSize(imageHeader.GetTotalFsblFwSizeIh());
            bi.bifOptions->SetTotalPmcFwSize(imageHeader.GetTotalPmcFwSizeIh() + padLength);
            //below for everest
            //bi.SetTotalPmcFwSize(imageHeader.getTotalPmcFwSizeIh());
        }
        //bi.SetTotalFsblFwSize(imageHeader.getTotalFsblFwSizeIh() + padLength);
        bi.SetTotalPmuFwSize(imageHeader.GetTotalPmuFwSizeIh());
        //bi.SetTotalPmcFwSize(imageHeader.getTotalPmcFwSizeIh());
        bi.SetPmuFwSize(imageHeader.GetPmuFwSizeIh());
        bi.bifOptions->SetPmcFwSize(imageHeader.GetPmcFwSizeIh());
        bi.SetFsblFwSize(imageHeader.GetFsblFwSizeIh());
        bi.hashBlockLength = hashBlockLength;
        if (bi.XipMode)
        {
            // Just to make sure, not to get a negative no. as address
            if (imageHeader.GetTotalPmuFwSizeIh() != 0)
            {
                if (imageHeader.GetFsblSourceAddrIh() > imageHeader.GetTotalPmuFwSizeIh())
                {
                    section->Address = imageHeader.GetFsblSourceAddrIh() - imageHeader.GetTotalPmuFwSizeIh();
                    bi.SetFsblSourceAddr(section->Address);
                }
            }

            // Just to make sure, not to get a negative no. as address
            if (imageHeader.GetTotalPmcFwSizeIh() != 0)
            {
                if (imageHeader.GetFsblSourceAddrIh() > imageHeader.GetTotalPmcFwSizeIh())
                {
                    section->Address = imageHeader.GetFsblSourceAddrIh() - imageHeader.GetTotalPmcFwSizeIh();
                    bi.SetFsblSourceAddr(section->Address);
                }
            }
        }
    }

    
    if (!section->isBootloader && (header->imageHeader->GetChecksumContext()->Type() != Checksum::None
                || header->imageHeader->GetAuthenticationType() != Authentication::None
                || (encryptCtx->Type() != Encryption::None || header->preencrypted) != false))
    {
        uint32_t count = 0;
        // Adjust partition number if no bootloader (to match hashNumMap adjustment)
        uint32_t adjustedPartNum = header->partitionNum;
        if (bi.IsBootloaderFound() == false)
            adjustedPartNum = header->partitionNum + 1;
            
        for (size_t i = 0; i < bi.hashNumMap.size(); i++)
        {            
            if(adjustedPartNum == bi.hashNumMap[i].second)
                count++;
        }
        
        
        
        if(count > 0)
        {
            uint32_t totalHashBlockSectionLength = ((sizeof(uint32_t) + chunkificationHashLength) * count);
            totalHashBlockSectionLength += PADDING_16B(totalHashBlockSectionLength);
            
            // Use smart pointer for allocation, then release to raw pointer (owned by cache)
            auto hashBlockPtr = std::make_unique<Section>("HashBlock", totalHashBlockSectionLength);
            memset(hashBlockPtr->Data.get(), 0, hashBlockPtr->Length);
            hashBlockSection = hashBlockPtr.release();
            
            fprintf(stderr, "[BUILD-PART] Created hashBlockSection for partition %d: length=%u, count=%u\n", 
                    header->partitionNum, totalHashBlockSectionLength, count);

            if (header->imageHeader->GetAuthenticationType() != Authentication::None)
            {
                hashBlockSection->IncreaseLengthAndPadTo(hashBlockSection->Length + currentAuthCtx->GetTotalHashBlockSignSize(), 0);
            }
            if (encryptCtx->Type() != Encryption::None && header->imageHeader->GetAuthenticationType() == Authentication::None)
            {
                hashBlockSection->IncreaseLengthAndPadTo(hashBlockSection->Length + AES_GCM_TAG_SZ, 0);
            }
        }
    }
    
    // IMAGE_STORE: Create AC if bootloader OR hashBlockSection exists
    if(imageHeader.IsBootloader() || hashBlockSection != NULL)
    {
        // Use smart pointer locally, then release() for legacy container
        auto tempacs = std::make_unique<Versal_2ve_2vmAuthenticationCertificate>(currentAuthCtx);
        tempacs->Build(bi, cache, header->partition->section, imageHeader.IsBootloader(), false);
        header->ac.push_back(tempacs.release());  // Release ownership to legacy container
    }

    /* Push the section alloted into the Main section */
    if(hashBlockSection != NULL)
    {
        cache.Sections.push_back(std::unique_ptr<Section>(hashBlockSection));  // Transfer ownership to cache
    }
    if (section != NULL)
    {
        cache.Sections.push_back(std::unique_ptr<Section>(section));

    }
}

/******************************************************************************/
void Versal_2ve_2vmPartition::Link(BootImage &bi)
{
    // IMAGE_STORE: Populate hashBlockSection with partition hashes
    if(hashBlockSection != NULL && bi.hashNumMap.size() != 0)
    {
        for (size_t i = 0; i < bi.hashNumMap.size(); i++)
        {
            uint32_t partNum = header->partitionNum;
            if(bi.IsBootloaderFound() == false)
                partNum = partNum+1;

            if(partNum == bi.hashNumMap[i].second)
            {
                //start with index 1 in hashTable because we dont need meta header hash here
                for (size_t j=1; j < bi.hashTable.size(); j++)
                {

                    if(bi.hashNumMap[i].first == bi.hashTable[j].first)
                    {
                        uint32_t hashIndex = bi.hashTable[j].first;
                        
                        memcpy(hashBlockSection->Data.get() + hashBlockSectionLength, &hashIndex, sizeof(uint32_t));
                        hashBlockSectionLength += sizeof(uint32_t);
                        memcpy(hashBlockSection->Data.get() + hashBlockSectionLength, bi.hashTable[j].second.get(), chunkificationHashLength);
                        hashBlockSectionLength += chunkificationHashLength;
                    }
                }
            }
        }        
        hashBlockSectionLength += PADDING_16B(hashBlockSectionLength);
        
    }
    
    for (std::list<AuthenticationCertificate*>::iterator acs = header->ac.begin(); acs != header->ac.end(); acs++)
    {
        if ((*acs))
        {
            // IMAGE_STORE: Pass partition object (not section) so AuthContext can access hashBlockSection
            (*acs)->Link(bi, this);  // Pass partition pointer like lstclone does
        }
    }
    
    // IMAGE_STORE: Encrypt hashBlockSection if needed (no authentication)
    if (hashBlockSection != NULL && header->imageHeader->GetEncryptContext()->Type() != Encryption::None 
        && header->imageHeader->GetAuthenticationType() == Authentication::None)
    {
        LOG_TRACE("Performing AAD on Hash Block");
        header->imageHeader->GetEncryptContext()->AesGcm256HashBlockEncrypt(bi.options, hashBlockSection->Data.get(),
            hashBlockSectionLength, hashBlockSection->Data.get() + hashBlockSectionLength, 1);
#ifdef DEBUG
        LOG_TRACE("Hash Block AAD");
        LOG_DUMP_BYTES(hashBlockSection->Data.get() + hashBlockSectionLength, AES_GCM_TAG_SZ);
#endif
    }
}

