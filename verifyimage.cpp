/******************************************************************************
* Copyright 2015-2019 Xilinx, Inc.
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


/*******************************************************************************/
static void RearrangeEndianess(char *array, uint32_t size)
{
    uint32_t lastIndex = size - 1;
    char tempInt = 0;

    // If array is NULL, return
    if (!array)
    {
        return;
    }

    for (uint32_t loop = 0; loop <= (lastIndex / 2); loop++)
    {
        tempInt = array[loop];
        array[loop] = array[lastIndex - loop];
        array[lastIndex - loop] = tempInt;
    }
}

/*******************************************************************************/
void ZynqMpReadImage::VerifyAuthentication(bool verifyImageOption)
{
    ReadBinaryFile();

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
    uint8_t* shaHash = new uint8_t[hashLength];
    if (nist)
    {
        crypto_hash_NIST_SHA3(shaHash, data, dataLength);
    }
    else
    {
        crypto_hash(shaHash, data, dataLength);
    }

    LOG_TRACE("Hash from data");
    LOG_DUMP_BYTES(shaHash, hashLength);

    rsa = RSA_new();

    BIGNUM* n = BN_new();
    n->d = (BN_ULONG*)acKey->N;
    n->dmax = RSA_KEY_LENGTH_ZYNQMP / sizeof(BN_ULONG);
    n->top = RSA_KEY_LENGTH_ZYNQMP / sizeof(BN_ULONG);
    n->flags = 0;
    n->neg = 0;
    
    BIGNUM* e = BN_new();
    e->d = (BN_ULONG*)acKey->E;
    e->dmax = 1;
    e->top = 1;
    e->flags = 0;
    e->neg = 0;
    
#if OPENSSL_VERSION_NUMBER > 0x10100000L
    BIGNUM *d = NULL;
    RSA_set0_key(rsa, n, e, d);
    RearrangeEndianess((char*)RSA_get0_e(rsa)->d, sizeof(uint32_t));
    RearrangeEndianess((char*)RSA_get0_n(rsa)->d, RSA_KEY_LENGTH_ZYNQMP);
#else
    rsa->n = n;
    rsa->e = e;
    RearrangeEndianess((char*)rsa->e->d, sizeof(uint32_t));
    RearrangeEndianess((char*)rsa->n->d, RSA_KEY_LENGTH_ZYNQMP);
#endif

    /* Find SHA-384 hash from signature */    
    uint8_t* opensslHashPadded = new uint8_t[RSA_KEY_LENGTH_ZYNQMP]; //chnage key length to signature length
    
    if (RSA_public_encrypt(RSA_KEY_LENGTH_ZYNQMP, signature, (unsigned char*)opensslHashPadded, rsa, RSA_NO_PADDING) < 0)
    {
        LOG_ERROR("RSA_public_encrypt error");
    }
    /* comment */
#if OPENSSL_VERSION_NUMBER > 0x10100000L
    RearrangeEndianess((char*)RSA_get0_n(rsa)->d, RSA_KEY_LENGTH_ZYNQMP);
    RearrangeEndianess((char*)RSA_get0_e(rsa)->d, sizeof(uint32_t));
#else
    RearrangeEndianess((char*)rsa->n->d, RSA_KEY_LENGTH_ZYNQMP);
    RearrangeEndianess((char*)rsa->e->d, sizeof(uint32_t));
#endif

    uint8_t* opensslHash = new uint8_t[hashLength];
    memcpy(opensslHash,opensslHashPadded + RSA_KEY_LENGTH_ZYNQMP - hashLength, hashLength);
    LOG_TRACE("Hash from signature");
    LOG_DUMP_BYTES(opensslHash, hashLength);
    
    /* compare openssl Hash with calculated shaHash */
    if (memcmp(shaHash, opensslHash, hashLength) == 0)
    {
        delete[] shaHash;
        delete[] opensslHash;
        delete[] opensslHashPadded;
        return true;
    }
    else
    {
        delete[] shaHash;
        delete[] opensslHash;
        delete[] opensslHashPadded;
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
    size_t headersSize = bH->sourceOffset - bH->imageHeaderByteOffset - RSA_KEY_LENGTH_ZYNQMP;
    uint8_t* tempBuffer = new uint8_t[headersSize];

    offset = bH->imageHeaderByteOffset;
    if (!(fseek(binFile, offset, SEEK_SET)))
    {
        result = fread(tempBuffer, 1, headersSize, binFile);
        if (result != headersSize)
        {
            LOG_ERROR("Error reading signature");
        }
    }

    bool signatureVerified = VerifySignature(true, tempBuffer, headersSize, &auth_cert->acSpk, (unsigned char*)(&auth_cert->acPartitionSignature));
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
    delete[] tempBuffer;
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
    uint8_t* tempBuffer = new uint8_t[size];
    
    WriteLittleEndian32(tempBuffer, auth_cert->acHeader);
    WriteLittleEndian32(tempBuffer + sizeof(auth_cert->acHeader), auth_cert->spkId);
    memcpy(tempBuffer + sizeof(auth_cert->acHeader) + sizeof(auth_cert->spkId), auth_cert->acSpk.N, sizeof(key.N));
    memcpy(tempBuffer + sizeof(auth_cert->acHeader) + sizeof(auth_cert->spkId) + sizeof(key.N), auth_cert->acSpk.N_extension, sizeof(key.N_extension));
    memcpy(tempBuffer + sizeof(auth_cert->acHeader) + sizeof(auth_cert->spkId) + sizeof(key.N) + sizeof(key.N_extension), auth_cert->acSpk.E, sizeof(key.E));
    memcpy(tempBuffer + sizeof(auth_cert->acHeader) + sizeof(auth_cert->spkId) + sizeof(key.N) + sizeof(key.N_extension) + sizeof(key.E), auth_cert->acSpk.Padding, sizeof(key.Padding));
    
    bool signatureVerified = VerifySignature(nist, tempBuffer, size, &auth_cert->acPpk, (unsigned char*)(&auth_cert->acSpkSignature));
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
    delete[] tempBuffer;
}


/*******************************************************************************/
void ZynqMpReadImage::VerifyPartitionSignature(void)
{
    size_t result;
    uint64_t offset = 0;
    
    /* Insert Partition Names from Iamge Header based on Section Count */
    std::list<std::string>::iterator name = iHNames.begin();
    for (std::list<ZynqMpImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++, name++)
    {
        for (uint32_t sectionIndex = 0; sectionIndex < (*iH)->imageNameLength; sectionIndex++)
        {
            std::stringstream ss;
            ss << sectionIndex;
            pHTNames.push_back(*name + "." + ss.str());
        }
    }

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

                uint8_t* tempBHBuffer = new uint8_t[bHLength];
                size_t result = fread(tempBHBuffer, 1, bHLength, binFile);
                if (result != bHLength)
                {
                    LOG_ERROR("Error reading boot header while verifying ");
                }

                bool signatureVerified = VerifySignature(false, tempBHBuffer, bHLength, &auth_cert->acSpk, (unsigned char*)(&auth_cert->acBhSignature));
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

                delete[] tempBHBuffer;
            }

            /* Verifying Partition SPK Signature */
            VerifySPKSignature(auth_cert);

            /* Partition Signature should not be included for hash calculation. */
            uint32_t bufferLength = ((*partitionHdr)->totalPartitionLength * 4) - RSA_KEY_LENGTH_ZYNQMP;
            uint8_t* tempBuffer = new uint8_t[bufferLength];
            offset = (*partitionHdr)->partitionWordOffset * 4;
            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(tempBuffer, 1, bufferLength, binFile);
                if (result != bufferLength)
                {
                    LOG_ERROR("Error reading partition for hash calculation");
                }
            }
            else
            {
                LOG_ERROR("Error parsing Partitions from BootImage file %s",binFilename.c_str());
            }
            
            
            bool signatureVerified = VerifySignature(!isItBootloader, tempBuffer, bufferLength, &auth_cert->acSpk, (unsigned char*)(&auth_cert->acPartitionSignature));
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

            delete[] tempBuffer;
        }
        else
        {
            //EXIT
        }
    }
    fclose(binFile);
    Separator();
}
