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
#include <string.h>

#include "bootimage.h"
#include "binary.h"
#include "stringutils.h"
#include "elftools.h"
#include "partitionheadertable-versal.h"
#include "partitionheadertable.h"
#include "checksum-versal.h"
#include "authentication-versal.h"
#include "Keccak-compact-versal.h"

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
VersalPartition::VersalPartition(PartitionHeader* header0, Section* section0)
    : Partition(header0, section0)
    , header(header0)
    , firstChunkSize(0)
{
    section = section0;

    // Populate bootloader flag here in the section object - needed in StackAndAlign stage
    section->isBootloader = header->imageHeader->IsBootloader();
    //    header->PartitionDataSize = section0->Length;
}

/******************************************************************************/
VersalPartition::VersalPartition(PartitionHeader* header0, const uint8_t* data, Binary::Length_t length)
    : Partition(header0, data, length)
    , header(header0)
    , firstChunkSize(0)
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
size_t VersalPartition::GetTotalDataChunks(Binary::Length_t partitionSize, std::vector<uint32_t>& dataChunks, bool encryptionFlag)
{
    size_t newSectionLength = partitionSize;
    size_t chunkOnlength = 0;
    Binary::Length_t dataChunksCount = 0;

    chunkOnlength = partitionSize;
    if (encryptionFlag)
    {
        chunkOnlength -= (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
    }
    dataChunksCount = (chunkOnlength / GetSecureChunkSize()) + (((chunkOnlength % GetSecureChunkSize()) == 0 ? 0 : 1));

    if (chunkOnlength % GetSecureChunkSize() != 0)
    {
        dataChunks.push_back(((chunkOnlength)-((dataChunksCount - 1) * GetSecureChunkSize())));
    }
    else
    {
        dataChunks.push_back(GetSecureChunkSize());
    }

    for (uint32_t itr = 0; itr < dataChunksCount - 1; itr++)
    {
        dataChunks.push_back(SHA3_LENGTH_BYTES);
        newSectionLength += SHA3_LENGTH_BYTES;
        if (itr == dataChunksCount - 2)
        {
            if (encryptionFlag)
            {
                dataChunks.push_back(GetSecureChunkSize() + SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                firstChunkSize = GetSecureChunkSize() + SECURE_HDR_SZ + AES_GCM_TAG_SZ;
            }
            else
            {
                dataChunks.push_back(GetSecureChunkSize());
                firstChunkSize = GetSecureChunkSize();
            }
        }
        else
        {
            dataChunks.push_back(GetSecureChunkSize());
        }
    }

    return newSectionLength;
}

/******************************************************************************/
void VersalPartition::ChunkifyAndHash(Section* section, bool encryptionFlag)
{
    std::vector<uint32_t> dataChunks;
    std::vector<uint8_t*> data;
    uint8_t* tempBuffer;
    int tempBufferSize = 0;
    /* Get the number/size data chunks and claculate the new section length*/
    size_t length = section->Length;
    size_t newLength = GetTotalDataChunks(length, dataChunks, encryptionFlag);

    int itr = (dataChunks.size() - 3);

    /* Form a new section - Divide into data chunks of 64K and add sha pad - calculate hash to each chunk - prepend hash to chunk+shapad to form a complete partition. */
    if (newLength <length)
    {
        LOG_DEBUG(DEBUG_STAMP, "Resize length is less than original length");
        LOG_ERROR("Section resize issue for authentication..");
    }

    uint8_t* dataPtr = new uint8_t[length];
    memset(dataPtr, 0, length);
    memcpy(dataPtr, section->Data, length);
    dataPtr += length;

    uint8_t* newDataPtr = new uint8_t[newLength];
    memset(newDataPtr, 0, newLength);
    newDataPtr += newLength;

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
    uint8_t* shaHash;
    shaHash = new uint8_t[SHA3_LENGTH_BYTES];
    Versalcrypto_hash(shaHash, newDataPtr, dataChunks[0], true);
    /*-------------------------------CHUNK N------------------------------------*/

    for (int i = 2; i <= itr; i += 2)
    {
        /* Insert previous hash */
        newDataPtr -= SHA3_LENGTH_BYTES;
        memcpy(newDataPtr, shaHash, SHA3_LENGTH_BYTES);
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
        shaHash = new uint8_t[SHA3_LENGTH_BYTES];
        Versalcrypto_hash(shaHash, newDataPtr, dataChunks[i] + dataChunks[i - 1], true);
    }

    /*-------------------------------CHUNK 1------------------------------------*/
    itr += 2;

    /* Insert previous hash */
    newDataPtr -= SHA3_LENGTH_BYTES;
    memcpy(newDataPtr, shaHash, SHA3_LENGTH_BYTES);
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

    delete[] dataPtr;

    delete[] section->Data;
    section->Data = newDataPtr;
    section->Length = newLength;

    LOG_TRACE("First Authentication Data Chunk Size 0x%X", firstChunkSize);
}

/******************************************************************************/
void VersalPartition::Build(BootImage& bi, Binary& cache)
{
    /* Get the image header from this partition header */
    ImageHeader& imageHeader(*this->header->imageHeader);
    /* Get the contexts for Authentication & Encryption */
    AuthenticationContext* currentAuthCtx = imageHeader.GetAuthContext();
    EncryptionContext* encryptCtx = imageHeader.GetEncryptContext();

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
    encryptCtx->Process(bi, header);
    
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
        header->transferSize = section->Length;
        if ((imageHeader.GetChecksumContext()->Type() == Checksum::SHA3) || (currentAuthCtx->authAlgorithm->Type() != Authentication::None))
        {
            /* No chunking on bootloader for versal - Data should be alligned to 104 bytes before calculating the hash */
            if (imageHeader.IsBootloader())
            {
                Binary::Length_t shaPadOnLength = header->partition->section->Length;
                if (currentAuthCtx->authAlgorithm->Type() != Authentication::None)
                {
                    /* Include AC but substract the Partition Signature Size, since it is not included in hash calculation */
                    shaPadOnLength += (currentAuthCtx->GetCertificateSize() - SIGN_LENGTH_VERSAL);
                }
                else if (imageHeader.GetChecksumContext()->Type() != Checksum::None)
                {
                    /* Checksum length is added to the partition length, in case of bootloader.
                    Substract the checksum length, since it should not be included in hash calculation */
                    shaPadOnLength -= imageHeader.GetChecksumContext()->Size();
                }
                uint8_t shaPadLength = SHA3_PAD_LENGTH - (shaPadOnLength % SHA3_PAD_LENGTH);

                /* Create the SHA3 Padded partition */
                section->IncreaseLengthAndPadSHA3(header->partition->section->Length + shaPadLength);

                if (bi.bifOptions->GetPmcdataFile() == "")
                {
                    imageHeader.SetTotalFsblFwSizeIh(imageHeader.GetTotalFsblFwSizeIh() + shaPadLength);
                }
                else
                {
                    imageHeader.SetTotalPmcFwSizeIh(imageHeader.GetTotalPmcFwSizeIh() + shaPadLength);
                }
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
                Binary::Length_t dataChunksCount = (chunkOnLength / GetSecureChunkSize()) + ((((chunkOnLength) % GetSecureChunkSize()) == 0 ? 0 : 1));
                if (dataChunksCount != 1)
                {
                    ChunkifyAndHash(section, (encryptCtx->Type() != Encryption::None || header->preencrypted));
                    currentAuthCtx->SetFirstChunkSize(firstChunkSize);
                    header->firstChunkSize = currentAuthCtx->GetFirstChunkSize();
                    header->partition->section->firstChunkSize = header->firstChunkSize;
                }
            }
        }

        if((imageHeader.IsBootloader() && imageHeader.GetChecksumContext()->Type() != Checksum::None))
        {
            padLength = imageHeader.GetChecksumContext()->Size();
        }
        else
        {
            // create AC/checksum section and add it to the end of the list.
            if (header->checksumSection != NULL)
            {
                cache.Sections.push_back(header->checksumSection);
            }
            AuthenticationCertificate* tempacs;
            tempacs = new VersalAuthenticationCertificate(currentAuthCtx);
            tempacs->Build(bi, cache, header->partition->section, imageHeader.IsBootloader(), false);
            header->ac.push_back(tempacs);
            currentAuthCtx->AddAuthCertSizeToTotalFSBLSize(header);
        }
    }

    if (imageHeader.IsBootloader() == true)
    {
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
void VersalPartition::Link(BootImage &bi)
{
    for (std::list<AuthenticationCertificate*>::iterator acs = header->ac.begin(); acs != header->ac.end(); acs++)
    {
        if ((*acs))
        {
            (*acs)->Link(bi, header->partition->section);
        }
    }
}

/******************************************************************************/
uint64_t VersalPartition::GetSecureChunkSize()
{
    return SECURE_32K_CHUNK - SHA3_LENGTH_BYTES;
}
