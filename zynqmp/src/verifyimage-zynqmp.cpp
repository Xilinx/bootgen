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
#include "readimage-zynqmp.h"
#include "authkeys.h"
#include "Keccak-compact.h"
#define BITSTREAM_AUTH_CHUNK_SIZE  0x800000 //8MB = 8*1024*1024



/*******************************************************************************/
void ZynqMpReadImage::VerifyAuthentication(bool verifyImageOption)
{
    ReadHeaderTableDetails();

    if (iHT->headerAuthCertificateWordOffset != 0)
    {
        VerifyHeaderTableSignature();
    }
    else
    {
        LOG_ERROR("Bootimage %s is not authenticated. Authentication verification cannot be done on this image.", binFilename.c_str());
    }

    VerifyPartitionSignature();
    if (authenticationVerified)
    {
        LOG_MSG("Authentication is verified on bootimage %s", binFilename.c_str());
    }
    else
    {
        LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
    }
    
}

/*******************************************************************************/
bool ZynqMpReadImage::VerifySignature(bool nist, uint8_t * data, size_t dataLength, ACKey4096* acKey, uint8_t * signature)
{
    /* Find SHA-384 hash from data */
    uint8_t hashLength = SHA3_LENGTH_BYTES;
    auto shaHash = std::make_unique<uint8_t[]>(hashLength);
    if (nist)
    {
        crypto_hash_NIST_SHA3(shaHash.get(), data, dataLength);
    }
    else
    {
        crypto_hash(shaHash.get(), data, dataLength);
    }

    LOG_TRACE("Hash from data");
    LOG_DUMP_BYTES(shaHash.get(), hashLength);

    RSA_Class rsaInstance;
    rsaInstance.rsa = RSA_new();
    if (rsaInstance.rsa == NULL)
    {
        LOG_ERROR("Failed to allocate RSA verification key");
    }

    // ZynqMP authentication certificates serialize RSA components in big-endian
    // order, unlike Bootgen's internal key buffers.
    BIGNUM* n = BN_bin2bn(acKey->N, RSA_4096_KEY_LENGTH, NULL);
    BIGNUM* e = BN_bin2bn(acKey->E, sizeof(uint32_t), NULL);
    if (n == NULL || e == NULL)
    {
        BN_free(n);
        BN_free(e);
        LOG_ERROR("Failed to create BIGNUMs from AC key data");
    }
    if (RSA_set0_key(rsaInstance.rsa, n, e, NULL) != 1)
    {
        BN_free(n);
        BN_free(e);
        LOG_ERROR("Failed to configure RSA verification key");
    }

    auto opensslHashPadded = std::make_unique<uint8_t[]>(RSA_4096_KEY_LENGTH);
    
    if (RSA_public_encrypt(RSA_4096_KEY_LENGTH, signature, (unsigned char*)opensslHashPadded.get(), rsaInstance.rsa, RSA_NO_PADDING) < 0)
    {
        LOG_ERROR("RSA_public_encrypt error");
    }

    auto opensslHash = std::make_unique<uint8_t[]>(hashLength);
    memcpy(opensslHash.get(),opensslHashPadded.get() + RSA_4096_KEY_LENGTH - hashLength, hashLength);
    LOG_TRACE("Hash from signature");
    LOG_DUMP_BYTES(opensslHash.get(), hashLength);
    
    /* compare openssl Hash with calculated shaHash */
    if (memcmp(shaHash.get(), opensslHash.get(), hashLength) == 0)
    {
        // Cleanup handled by unique_ptr
        return true;
    }
    else
    {
        // Cleanup handled by unique_ptr
        return false;
    }    
}

/*******************************************************************************/
void ZynqMpReadImage::VerifyHeaderTableSignature()
{
    Separator();
    LOG_MSG("Verifying Header Authentication Certificate");
    uint64_t offset = 0;
    size_t result;

    FILE *binFile;
    binFile = fopen(binFilename.c_str(), "rb");

    AuthCertificate4096Structure* auth_cert = (AuthCertificate4096Structure*)(aCs.front());

    /* Verifying Header SPK Signature */
    VerifySPKSignature(auth_cert);

    /* Partition Signature should not be included for hash calculation. */
    size_t headersSize = bH->sourceOffset - bH->imageHeaderByteOffset - RSA_4096_KEY_LENGTH;
    if(bH->sourceOffset == 0)
    {
        headersSize = (pHT->partitionWordOffset * 4) - bH->imageHeaderByteOffset - RSA_4096_KEY_LENGTH;
    }
    auto tempBuffer = std::make_unique<uint8_t[]>(headersSize);
    memset(tempBuffer.get(), 0, headersSize);
    offset = bH->imageHeaderByteOffset;

    if (!(fseek(binFile, offset, SEEK_SET)))
    {
        result = fread(tempBuffer.get(), 1, headersSize, binFile);
        if (result != headersSize)
        {
            LOG_ERROR("Error reading signature");
        }
    }
    else
    {
        LOG_ERROR("Error parsing Headers from BootImage file %s", binFilename.c_str());
    }

    bool signatureVerified = VerifySignature(true, tempBuffer.get(), headersSize, &auth_cert->acSpk, (unsigned char*)(&auth_cert->acPartitionSignature));
    if (signatureVerified)
    {
        LOG_MSG("    Header Signature Verified");
    }
    else
    {
        LOG_MSG("    Header Signature Verification Failed");
        authenticationVerified = false;
        LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
    }

    fclose(binFile);
    // Cleanup handled by unique_ptr
}

/*******************************************************************************/
void ZynqMpReadImage::VerifySPKSignature(AuthCertificate4096Structure* auth_cert)
{
    bool nist = false;
    size_t size = 0;
    ACKey4096 key;

    if (auth_cert->acHeader & 0x80000)
    {
        nist = true;
    }
    
    // Hash of SPK - AH + SPK ID + SPK FULL + Padding
    size = sizeof(auth_cert->acHeader) + sizeof(auth_cert->spkId) + sizeof(key.N) + sizeof(key.N_extension) + sizeof(key.E) + sizeof(key.Padding);
    auto tempBuffer = std::make_unique<uint8_t[]>(size);
    
    WriteLittleEndian32(tempBuffer.get(), auth_cert->acHeader);
    WriteLittleEndian32(tempBuffer.get() + sizeof(auth_cert->acHeader), auth_cert->spkId);
    memcpy(tempBuffer.get() + sizeof(auth_cert->acHeader) + sizeof(auth_cert->spkId), auth_cert->acSpk.N, sizeof(key.N));
    memcpy(tempBuffer.get() + sizeof(auth_cert->acHeader) + sizeof(auth_cert->spkId) + sizeof(key.N), auth_cert->acSpk.N_extension, sizeof(key.N_extension));
    memcpy(tempBuffer.get() + sizeof(auth_cert->acHeader) + sizeof(auth_cert->spkId) + sizeof(key.N) + sizeof(key.N_extension), auth_cert->acSpk.E, sizeof(key.E));
    memcpy(tempBuffer.get() + sizeof(auth_cert->acHeader) + sizeof(auth_cert->spkId) + sizeof(key.N) + sizeof(key.N_extension) + sizeof(key.E), auth_cert->acSpk.Padding, sizeof(key.Padding));
     
    bool signatureVerified = VerifySignature(nist, tempBuffer.get(), size, &auth_cert->acPpk, (unsigned char*)(&auth_cert->acSpkSignature));
    if (signatureVerified)
    {
        LOG_MSG("    SPK Signature Verified");
    }
    else
    {
        LOG_MSG("    SPK Signature Verification Failed");
        authenticationVerified = false;
        LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
    }
    // Cleanup handled by unique_ptr
}


/*******************************************************************************/
void ZynqMpReadImage::VerifyPartitionSignature(void)
{
    size_t result;
    uint64_t offset = 0;

    std::list<uint8_t*>::iterator authCertificate = aCs.begin();
    authCertificate++;

    FILE *binFile;
    binFile = fopen(binFilename.c_str(), "rb");

    /* Partition Extraction */
    std::list<std::string>::iterator partitionName = pHTNames.begin();
    for (std::list<ZynqMpPartitionHeaderTableStructure*>::iterator partitionHdr = pHTs.begin(); partitionHdr != pHTs.end(); partitionHdr++, authCertificate++, partitionName++)
    {
        if ((*partitionHdr)->authCertificateOffset != 0)
        {
            Separator();
            LOG_MSG("Verifying Partition '%s' Authentication Certificate", (*partitionName).c_str());
            AuthCertificate4096Structure* auth_cert = (AuthCertificate4096Structure*)(*authCertificate);

            bool checkLoadAddrInBhAndPht = ((*partitionHdr)->destinationExecAddress == bH->fsblExecAddress);
            bool isItBootloader = (checkLoadAddrInBhAndPht && (bH->sourceOffset != 0));
            
            if (isItBootloader)
            {
                uint32_t bHLength = sizeof(ZynqMpBootHeaderStructure) + sizeof(RegisterInitTable);
                if (bH->fsblAttributes & 0xC0)
                {
                    bHLength += PUF_DATA_LENGTH;
                }

                auto tempBHBuffer = std::make_unique<uint8_t[]>(bHLength);
                size_t result = fread(tempBHBuffer.get(), 1, bHLength, binFile);
                if (result != bHLength)
                {
                    LOG_ERROR("Error reading boot header while verifying ");
                }

                bool signatureVerified = VerifySignature(false, tempBHBuffer.get(), bHLength, &auth_cert->acSpk, (unsigned char*)(&auth_cert->acBhSignature));
                if (signatureVerified)
                {
                    LOG_MSG("    BootHeader Signature Verified");
                }
                else
                {
                    LOG_MSG("    BootHeader Signature Verification Failed");
                    authenticationVerified = false;
                    LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
                }
                // tempBHBuffer is now unique_ptr, automatically deleted
            }

            /* Verifying Partition SPK Signature */
            VerifySPKSignature(auth_cert);

           /* Partition Signature should not be included for hash calculation. */
            uint32_t bufferLength = ((*partitionHdr)->totalPartitionLength * 4) - RSA_4096_KEY_LENGTH;
            bool signatureVerified = false;
            if (((((*partitionHdr)->partitionAttributes) >> PH_DEST_DEVICE_SHIFT_ZYNQMP) & PH_DEST_DEVICE_MASK_ZYNQMP) == 2)
            {
                bufferLength = ((*partitionHdr)->totalPartitionLength * 4);
            }

            auto tempBuffer = std::make_unique<uint8_t[]>(bufferLength);
            memset(tempBuffer.get(), 0, bufferLength);

            offset = (*partitionHdr)->partitionWordOffset * 4;
            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(tempBuffer.get(), 1, bufferLength, binFile);
                if (result != bufferLength)
                {
                    LOG_ERROR("Error reading partition for hash calculation");
                }
            }
            else
            {
                LOG_ERROR("Error parsing Partitions from BootImage file %s",binFilename.c_str());
            }
           
            if (((((*partitionHdr)->partitionAttributes) >> PH_DEST_DEVICE_SHIFT_ZYNQMP) & PH_DEST_DEVICE_MASK_ZYNQMP) == 2)
            {
                uint32_t blockSize = BITSTREAM_AUTH_CHUNK_SIZE; 
                uint32_t lastBlockSize = ((*partitionHdr)->totalPartitionLength * 4) - (sizeof(AuthCertificate4096Structure) * plAcCount) - (BITSTREAM_AUTH_CHUNK_SIZE * (plAcCount - 1));

                for(int i = 0; i<plAcCount ; i++)
                {
                    if (i == plAcCount - 1 && (lastBlockSize < BITSTREAM_AUTH_CHUNK_SIZE))
                    {
                        blockSize = lastBlockSize;
                    }
                    if(i != 0)
                    {
                        authCertificate++;
                    }
                    AuthCertificate4096Structure* cert = (AuthCertificate4096Structure*)(*authCertificate);
                    auto buffer = std::make_unique<uint8_t[]>(blockSize + sizeof(AuthCertificate4096Structure) - RSA_4096_KEY_LENGTH);
                    memcpy(buffer.get(), tempBuffer.get() + BITSTREAM_AUTH_CHUNK_SIZE * i, blockSize);
                    memcpy(buffer.get() + blockSize, cert, (sizeof(AuthCertificate4096Structure) - RSA_4096_KEY_LENGTH));
                    signatureVerified = VerifySignature(!isItBootloader, buffer.get(), blockSize + (sizeof(AuthCertificate4096Structure) - RSA_4096_KEY_LENGTH), &auth_cert->acSpk, (unsigned char*)(&cert->acPartitionSignature));             
                    if(!signatureVerified)
                    {
                        LOG_MSG("    Partition Signature Verification Failed");
                        authenticationVerified = false;
                        LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
                    }
                    // Cleanup handled by unique_ptr
                }
            }
            else
            {
                signatureVerified = VerifySignature(!isItBootloader, tempBuffer.get(), bufferLength, &auth_cert->acSpk, (unsigned char*)(&auth_cert->acPartitionSignature));
            }
            if (signatureVerified)
            {
                LOG_MSG("    Partition Signature Verified");
            }
            else
            {
                LOG_MSG("    Partition Signature Verification Failed");
                authenticationVerified = false;
                LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
            }
            // Cleanup handled by unique_ptr
        }
        else
        {
            //EXIT
        }
    }
    fclose(binFile);
    Separator();
}
