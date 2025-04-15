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
        uint8_t* sha_hash = new uint8_t[chunkificationHashLength];
        memset(sha_hash, 0, chunkificationHashLength);
        if (header->partition->section->isBootloader)
        {
            //PLM HASH
            size_t length = header->imageHeader->GetTotalFsblFwSizeIh();
            bi.hash->CalculateVersalHash(true, header->partition->section->Data, length, sha_hash);
            LOG_TRACE("PLM_PCR_HASH");
            LOG_DUMP_BYTES(sha_hash, chunkificationHashLength);

            //PMC_DATA HASH
            if (header->imageHeader->GetTotalPmcFwSizeIh() != 0)
            {
                memset(sha_hash, 0, chunkificationHashLength);
                bi.hash->CalculateVersalHash(true, header->partition->section->Data + length, header->imageHeader->GetTotalPmcFwSizeIh(), sha_hash);
                LOG_TRACE("PMC_DATA_PCR_HASH");
                LOG_DUMP_BYTES(sha_hash, chunkificationHashLength);
            }
        }
        else
        {
            bi.hash->CalculateVersalHash(true, header->partition->section->Data, header->partition->section->Length, sha_hash);
        }
        delete sha_hash;
    }
}

/******************************************************************************/
Versal_2ve_2vmPartition::Versal_2ve_2vmPartition(PartitionHeader* header0, Section* section0)
    : Partition(header0, section0)
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
    section = new Section(partName, totallength);
    section->index = header->index;
    section->isPartitionData = true;
    section->isBitStream = (header->imageHeader->GetDomain() == Domain::PL) ? true : false;
    section->isFirstElfSection = (header->firstValidIndex);
    section->isBootloader = header->imageHeader->IsBootloader();
    memcpy(section->Data, data, length);
    memset(section->Data + length, 0, padding);
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
    size_t newSectionLength;

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
    uint8_t* tempBuffer;
    int tempBufferSize = 0;
    /* Get the number/size data chunks and claculate the new section length */
    size_t length = section->Length;
    size_t newLength = 0;
    bool checksum_bootloader = false;
    uint8_t* pmcDataHash;
    pmcDataHash = new uint8_t[chunkificationHashLength];

    uint8_t* partitionHash;
    partitionHash = new uint8_t[chunkificationHashLength];
    memset(partitionHash, 0, chunkificationHashLength);

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

    uint8_t* dataPtr = new uint8_t[length];
    memset(dataPtr, 0, length);
    memcpy(dataPtr, section->Data, length);
    dataPtr += length;

    uint8_t* newDataPtr = new uint8_t[newLength];
    memset(newDataPtr, 0, newLength);
    newDataPtr += newLength;

    uint8_t* shaHash;

    //PMC DATA
    int itr = 0;
    if(!header->imageHeader->GetPrebuilt() || header->imageHeader->GetReplacePmc())
    {
        if (header->imageHeader->GetTotalPmcFwSizeIh() != 0)
        {
            if (pmcdataChunkCount == 1)
            {
                tempBufferSize = dataChunks[itr];
                tempBuffer = new uint8_t[tempBufferSize];
                memset(tempBuffer, 0, tempBufferSize);

                dataPtr -= tempBufferSize;
                memcpy(tempBuffer, dataPtr, tempBufferSize);
                newDataPtr -= tempBufferSize;

                memcpy(newDataPtr, tempBuffer, tempBufferSize);
                delete[] tempBuffer;

                shaHash = new uint8_t[chunkificationHashLength];
                CalculateChunkificationHash(shaHash, newDataPtr, tempBufferSize, true);

                memcpy(pmcDataHash, shaHash, chunkificationHashLength);
                LOG_TRACE("PMC DATA HASH");
                LOG_DUMP_BYTES(pmcDataHash, chunkificationHashLength);
                delete[] shaHash;
            }
            else
            {
                itr = (pmcdataChunkCount - 3);

                /*-------------------------------CHUNK N------------------------------------*/
                /* Insert Data */
                tempBufferSize = dataChunks[0];
                tempBuffer = new uint8_t[tempBufferSize];
                memset(tempBuffer, 0, tempBufferSize);

                dataPtr -= tempBufferSize;
                memcpy(tempBuffer, dataPtr, tempBufferSize);
                newDataPtr -= tempBufferSize;

                memcpy(newDataPtr, tempBuffer, tempBufferSize);
                delete[] tempBuffer;

                /* Calculate hash */
                shaHash = new uint8_t[chunkificationHashLength];
                CalculateChunkificationHash(shaHash, newDataPtr, dataChunks[0], true);
                /*-------------------------------CHUNK N------------------------------------*/

                for (int i = 2; i <= itr; i += 2)
                {
                    /* Insert previous hash */
                    newDataPtr -= chunkificationHashLength;
                    memcpy(newDataPtr, shaHash, chunkificationHashLength);
                    delete[] shaHash;

                    /* Insert Data */
                    tempBufferSize = dataChunks[i];
                    tempBuffer = new uint8_t[tempBufferSize];
                    memset(tempBuffer, 0, tempBufferSize);

                    dataPtr -= tempBufferSize;
                    memcpy(tempBuffer, dataPtr, tempBufferSize);
                    newDataPtr -= tempBufferSize;

                    memcpy(newDataPtr, tempBuffer, tempBufferSize);
                    delete[] tempBuffer;

                    /* Calculate hash */
                    shaHash = new uint8_t[chunkificationHashLength];
                    CalculateChunkificationHash(shaHash, newDataPtr, dataChunks[i] + dataChunks[i - 1], true);
                }

                /*-------------------------------CHUNK 1------------------------------------*/
                itr += 2;

                /* Insert previous hash */
                newDataPtr -= chunkificationHashLength;
                memcpy(newDataPtr, shaHash, chunkificationHashLength);
                delete[] shaHash;

                /* Insert Data */
                tempBufferSize = dataChunks[itr];
                tempBuffer = new uint8_t[tempBufferSize];
                memset(tempBuffer, 0, tempBufferSize);

                dataPtr -= tempBufferSize;
                memcpy(tempBuffer, dataPtr, tempBufferSize);
                newDataPtr -= tempBufferSize;

                memcpy(newDataPtr, tempBuffer, tempBufferSize);
                delete[] tempBuffer;
                /*-------------------------------CHUNK 1------------------------------------*/

                /* Calculate hash of top chunk and previous hash */
                shaHash = new uint8_t[chunkificationHashLength];
                CalculateChunkificationHash(shaHash, newDataPtr, tempBufferSize + chunkificationHashLength, true);

                memcpy(pmcDataHash, shaHash, chunkificationHashLength);
                LOG_TRACE("PMC DATA HASH");
                LOG_DUMP_BYTES(pmcDataHash, chunkificationHashLength);
                delete[] shaHash;
            }
        }
    }
    else
	{
        dataPtr -= header->imageHeader->GetTotalPmcFwSizeIh();
        shaHash = new uint8_t[chunkificationHashLength];
        CalculateChunkificationHash(shaHash, dataPtr, secureChunkSize + chunkificationHashLength, true);
        memcpy(pmcDataHash, shaHash, chunkificationHashLength);
        LOG_TRACE("PMC DATA HASH");
        LOG_DUMP_BYTES(pmcDataHash, chunkificationHashLength);
        dataPtr += header->imageHeader->GetTotalPmcFwSizeIh();

        tempBufferSize = header->imageHeader->GetTotalPmcFwSizeIh();
        //tempBufferSize = header->imageHeader->GetPmcFwSizeIh();
        tempBuffer = new uint8_t[tempBufferSize];
        memset(tempBuffer, 0, tempBufferSize);

        dataPtr -= tempBufferSize;
        memcpy(tempBuffer, dataPtr, tempBufferSize);
        newDataPtr -= tempBufferSize;

        memcpy(newDataPtr, tempBuffer, tempBufferSize);
        delete[] tempBuffer;
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
                tempBuffer = new uint8_t[tempBufferSize];
                memset(tempBuffer, 0, tempBufferSize);

                dataPtr -= tempBufferSize;
                memcpy(tempBuffer, dataPtr, tempBufferSize);
                newDataPtr -= tempBufferSize;

                memcpy(newDataPtr, tempBuffer, tempBufferSize);
                delete[] tempBuffer;

                shaHash = new uint8_t[chunkificationHashLength];
                CalculateChunkificationHash(shaHash, newDataPtr, tempBufferSize, true);

                memcpy(partitionHash, shaHash, chunkificationHashLength);
                if (section->isBootloader)
                {
                    LOG_TRACE("PLM HASH");
                    LOG_DUMP_BYTES(partitionHash, chunkificationHashLength);
                }
                //LOG_TRACE("Partition Data HASHed");
                //LOG_DUMP_BYTES(newDataPtr, tempBufferSize);
                delete[] shaHash;
            }
            else
            {
                itr = (dataChunks.size() - 3);

                /*-------------------------------CHUNK N------------------------------------*/
                /* Insert Data */
                tempBufferSize = dataChunks[0];
                tempBuffer = new uint8_t[tempBufferSize];
                memset(tempBuffer, 0, tempBufferSize);

                dataPtr -= tempBufferSize;
                memcpy(tempBuffer, dataPtr, tempBufferSize);
                newDataPtr -= tempBufferSize;

                memcpy(newDataPtr, tempBuffer, tempBufferSize);
                delete[] tempBuffer;

                /* Calculate hash */
                shaHash = new uint8_t[chunkificationHashLength];
                CalculateChunkificationHash(shaHash, newDataPtr, dataChunks[0], true);
                /*-------------------------------CHUNK N------------------------------------*/

                for (int i = 2; i <= itr; i += 2)
                {
                    /* Insert previous hash */
                    newDataPtr -= chunkificationHashLength;
                    memcpy(newDataPtr, shaHash, chunkificationHashLength);
                    delete[] shaHash;

                    /* Insert Data */
                    tempBufferSize = dataChunks[i];
                    tempBuffer = new uint8_t[tempBufferSize];
                    memset(tempBuffer, 0, tempBufferSize);

                    dataPtr -= tempBufferSize;
                    memcpy(tempBuffer, dataPtr, tempBufferSize);
                    newDataPtr -= tempBufferSize;

                    memcpy(newDataPtr, tempBuffer, tempBufferSize);
                    delete[] tempBuffer;

                    /* Calculate hash */
                    shaHash = new uint8_t[chunkificationHashLength];
                    CalculateChunkificationHash(shaHash, newDataPtr, dataChunks[i] + dataChunks[i - 1], true);
                }

                /*-------------------------------CHUNK 1------------------------------------*/
                itr += 2;

                /* Insert previous hash */
                newDataPtr -= chunkificationHashLength;
                memcpy(newDataPtr, shaHash, chunkificationHashLength);
                delete[] shaHash;

                /* Insert Data */
                tempBufferSize = dataChunks[itr];
                tempBuffer = new uint8_t[tempBufferSize];
                memset(tempBuffer, 0, tempBufferSize);

                dataPtr -= tempBufferSize;
                memcpy(tempBuffer, dataPtr, tempBufferSize);
                newDataPtr -= tempBufferSize;

                memcpy(newDataPtr, tempBuffer, tempBufferSize);
                delete[] tempBuffer;
                /*-------------------------------CHUNK 1------------------------------------*/

                /* Calculate hash of top chunk and previous hash */
                shaHash = new uint8_t[chunkificationHashLength];
                CalculateChunkificationHash(shaHash, newDataPtr, tempBufferSize + chunkificationHashLength, true);

                memcpy(partitionHash, shaHash, chunkificationHashLength);
                if (section->isBootloader)
                {
                    LOG_TRACE("PLM HASH");
                    LOG_DUMP_BYTES(partitionHash, chunkificationHashLength);
                }
                //LOG_TRACE("PLM Data HASHed");
                //LOG_DUMP_BYTES(newDataPtr, tempBufferSize + chunkificationHashLength);
                delete[] shaHash;
            }
        }
    }
    else
	{
        if(section->isBootloader && !header->imageHeader->GetReplacePlm())
        {
            dataPtr -= header->imageHeader->GetTotalFsblFwSizeIh();
            shaHash = new uint8_t[chunkificationHashLength];
            CalculateChunkificationHash(shaHash, dataPtr, secureChunkSize + chunkificationHashLength, true);
            memcpy(partitionHash, shaHash, chunkificationHashLength);
            if (section->isBootloader)
            {
                LOG_TRACE("PLM HASH");
                LOG_DUMP_BYTES(partitionHash, chunkificationHashLength);
            }
            dataPtr += header->imageHeader->GetTotalFsblFwSizeIh();

            tempBufferSize = header->imageHeader->GetTotalFsblFwSizeIh();
            tempBuffer = new uint8_t[tempBufferSize];
            memset(tempBuffer, 0, tempBufferSize);

            dataPtr -= tempBufferSize;
            memcpy(tempBuffer, dataPtr, tempBufferSize);
            newDataPtr -= tempBufferSize;

            memcpy(newDataPtr, tempBuffer, tempBufferSize);
            delete[] tempBuffer;
        }
        else if(!header->imageHeader->GetReplacePsm())
        {
            dataPtr -= length;
            shaHash = new uint8_t[chunkificationHashLength];
            CalculateChunkificationHash(shaHash, dataPtr, secureChunkSize + chunkificationHashLength, true);
            memcpy(partitionHash, shaHash, chunkificationHashLength);
            dataPtr += length;

            tempBufferSize = length;
            tempBuffer = new uint8_t[tempBufferSize];
            memset(tempBuffer, 0, tempBufferSize);

            dataPtr -= tempBufferSize;
            memcpy(tempBuffer, dataPtr, tempBufferSize);
            newDataPtr -= tempBufferSize;

            memcpy(newDataPtr, tempBuffer, tempBufferSize);
            delete[] tempBuffer;
        }
    }
    if (checksum_bootloader)
    {
        /* GCM Tag sits in the end of Hash Block */
        if (encryptionFlag && (header->imageHeader->GetAuthenticationType() == Authentication::None))
        {
            newDataPtr -= AES_GCM_TAG_SZ;
        }

        if (header->imageHeader->GetAuthenticationType() != Authentication::None)
        {
            newDataPtr -= totalHashBlockSignatureLength;
        }

        newDataPtr -= HASH_BLOCK_ALIGNMENT_BYTES_TELLURIDE;
        newDataPtr -= (3 * (chunkificationHashLength + HASH_BLOCK_INDEX_BYTES)); //Reserved Hashes

        uint32_t hashIndex = 0;

        if (header->imageHeader->GetTotalPmcFwSizeIh() != 0)
        {
            /* Place the PMC DATA hash at the third position in Hash Block of PLM partition */
            newDataPtr -= chunkificationHashLength;
            memcpy(newDataPtr, pmcDataHash, chunkificationHashLength);

            hashIndex = HASH_BLOCK_PMCDATA_HASH_INDEX;
            newDataPtr -= HASH_BLOCK_INDEX_BYTES;
            memcpy(newDataPtr, &hashIndex, sizeof(uint32_t));
        }
        else
        {
            newDataPtr -= chunkificationHashLength;
            newDataPtr -= HASH_BLOCK_INDEX_BYTES;
        }

        /* Place the final hash at the second position in Hash Block of PLM partition */
        newDataPtr -= chunkificationHashLength;
        memcpy(newDataPtr, partitionHash, chunkificationHashLength);

        hashIndex = HASH_BLOCK_PLM_HASH_INDEX;
        newDataPtr -= HASH_BLOCK_INDEX_BYTES;
        memcpy(newDataPtr, &hashIndex, sizeof(uint32_t));

        /* Place the BH hash at the start of Hash Block of PLM partition */
        newDataPtr -= chunkificationHashLength;
        newDataPtr -= HASH_BLOCK_INDEX_BYTES;
    }

    delete[] dataPtr;
    delete[] pmcDataHash;
    delete[] partitionHash;

    delete[] section->Data;
    section->Data = newDataPtr;
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
        uint32_t* syncpt_offsets = NULL;
        uint8_t num_of_sync_points = 0;

        CdoSequence * cdo_seq;
        cdo_seq = decode_cdo_binary(header->partition->section->Data, header->partition->section->Length);

        /* Enable the search for sync points - only needs to be done for SSIT devices */
        search_for_sync_points();

        uint8_t* buffer = (uint8_t*)cdoseq_to_binary(cdo_seq, &buffer_size, 0);

        /* Get no. of sync points and sync points offsets */
        num_of_sync_points = get_num_of_sync_points();
        syncpt_offsets = get_slr_sync_point_offsets();

        for (int i = 0; i < num_of_sync_points; i++)
        {
            size_t offset = (*(syncpt_offsets + i) * 4);
            bi.sync_offsets.push_back(offset);
        }
        delete syncpt_offsets;
        delete buffer;
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
                        uint8_t* newDataPtr = new uint8_t[newLength];
                        memset(newDataPtr, 0, newLength);

                        newDataPtr += hashBlockLength;
                        if ((encryptCtx->Type() != Encryption::None) && currentAuthCtx->authAlgorithm->Type() == Authentication::None)
                        {
                            newDataPtr += AES_GCM_TAG_SZ;
                        }
                        if (header->imageHeader->GetAuthenticationType() != Authentication::None)
                        {
                            newDataPtr += totalHashBlockSignatureLength;
                        }

                        memcpy(newDataPtr, section->Data, section->Length);

                        /* Calculate hash */
                        uint8_t* shaHash;
                        shaHash = new uint8_t[chunkificationHashLength];
                        
                        CalculateChunkificationHash(shaHash, newDataPtr, section->Length, true);
                        LOG_TRACE("PLM HASH");
                        LOG_DUMP_BYTES(shaHash, chunkificationHashLength);
                        //LOG_TRACE("PLM Data HASHed");
                        //LOG_DUMP_BYTES(newDataPtr, section->Length);

                        /* GCM Tag sits in the end of Hash Block */
                        if ((encryptCtx->Type() != Encryption::None) && currentAuthCtx->authAlgorithm->Type() == Authentication::None)
                        {
                            newDataPtr -= AES_GCM_TAG_SZ;
                        }

                        /* Place the final hash at the start of PLM partition */
                        if (header->imageHeader->GetAuthenticationType() != Authentication::None)
                        {
                            newDataPtr -= totalHashBlockSignatureLength;
                        }

                        newDataPtr -= HASH_BLOCK_ALIGNMENT_BYTES_TELLURIDE;
                        newDataPtr -= (4 * (chunkificationHashLength + HASH_BLOCK_INDEX_BYTES)); //Reserved Hashes, including pmc data hash

                        newDataPtr -= chunkificationHashLength;
                        memcpy(newDataPtr, shaHash, chunkificationHashLength);

                        uint32_t hashIndex = HASH_BLOCK_PLM_HASH_INDEX;
                        newDataPtr -= HASH_BLOCK_INDEX_BYTES;
                        memcpy(newDataPtr, &hashIndex, sizeof(uint32_t));

                        header->imageHeader->SetTotalFsblFwSizeIh(header->imageHeader->GetTotalFsblFwSizeIh());

                        /* For BH Hash */
                        newDataPtr -= chunkificationHashLength;
                        newDataPtr -= HASH_BLOCK_INDEX_BYTES;

                        delete[] shaHash;
                        delete[] section->Data;
                        section->Data = newDataPtr;
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

                uint8_t* shaHash = new uint8_t[chunkificationHashLength];
                uint8_t* hash = new uint8_t[chunkificationHashLength];
                bi.hashTable.push_back(std::pair<uint32_t, uint8_t*>(header->partitionNum, hash));

                if (dataChunksCount != 1)
                {
                    ChunkifyAndHash(section, (encryptCtx->Type() != Encryption::None || header->preencrypted));
                    if(header->imageHeader->GetPrebuilt())
                        firstChunkSize = secureChunkSize;
                    currentAuthCtx->SetFirstChunkSize(firstChunkSize);
                    header->firstChunkSize = currentAuthCtx->GetFirstChunkSize();
                    header->partition->section->firstChunkSize = header->firstChunkSize;

                    CalculateChunkificationHash(shaHash, section->Data, firstChunkSize + chunkificationHashLength, true);
                }
                if (dataChunksCount == 1)
                {
                    CalculateChunkificationHash(shaHash, section->Data, section->Length, true);
                }

                memcpy(hash, shaHash, chunkificationHashLength);
                LOG_TRACE("%s , Partition Index : %d \n           Hash :", header->section->Name.c_str(), header->partitionNum);
                LOG_DUMP_BYTES(hash, chunkificationHashLength);

                delete[] shaHash;
            }
        }

        AuthenticationCertificate* tempacs;
        tempacs = new Versal_2ve_2vmAuthenticationCertificate(currentAuthCtx);
        tempacs->Build(bi, cache, header->partition->section, imageHeader.IsBootloader(), false);
        header->ac.push_back(tempacs);
    }

    if (imageHeader.IsBootloader() == true)
    {
        if (encryptCtx->Type() != Encryption::None && currentAuthCtx->authAlgorithm->Type() == Authentication::None)
        {
            uint8_t* tmpBh = bi.bootHeader->section->Data + 0x10;
            uint8_t* sha_hash = new uint8_t[chunkificationHashLength];

            Versal_2ve_2vmBootHeaderStructure* bh = (Versal_2ve_2vmBootHeaderStructure*)bi.bootHeader->section->Data;
            bh->sourceOffset = bi.bootHeader->section->Length + hashBlockLength + AES_GCM_TAG_SZ;
            bh->hashBlockLength1 = hashBlockLength;
            bh->totalPlmLength = header->imageHeader->GetTotalFsblFwSizeIh();
            bh->imageHeaderByteOffset = bi.bootHeader->section->Length + hashBlockLength + AES_GCM_TAG_SZ + bh->totalPlmLength + bh->totalPmcCdoLength;
            bh->headerChecksum = bi.bootHeader->ComputeWordChecksum(&bh->widthDetectionWord, bi.bootHeader->GetBHChecksumDataSize());
            bi.hash->CalculateVersalHash(true, tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(Versal_2ve_2vmSmapWidthTable), sha_hash);

            LOG_TRACE("BH before AAD");
            LOG_DUMP_BYTES(tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(Versal_2ve_2vmSmapWidthTable));
            /* Copy BH Hash */
            memcpy(header->partition->section->Data + HASH_BLOCK_INDEX_BYTES, sha_hash, chunkificationHashLength);
            LOG_TRACE("hash block 0");
            LOG_DUMP_BYTES(header->partition->section->Data, hashBlockLength);
            delete[] sha_hash;

            encryptCtx->AesGcm256HashBlockEncrypt(bi.options, header->partition->section->Data,
                hashBlockLength, header->partition->section->Data + hashBlockLength, 2);

            LOG_TRACE("GCM Tag + Hash Block 0");
            LOG_DUMP_BYTES(header->partition->section->Data, hashBlockLength + AES_GCM_TAG_SZ);
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

    /* Push the section alloted into the Main section */
    if (section != NULL)
    {
        cache.Sections.push_back(section);
    }
}

/******************************************************************************/
void Versal_2ve_2vmPartition::Link(BootImage &bi)
{
    for (std::list<AuthenticationCertificate*>::iterator acs = header->ac.begin(); acs != header->ac.end(); acs++)
    {
        if ((*acs))
        {
            (*acs)->Link(bi, header->partition->section);
        }
    }
}

