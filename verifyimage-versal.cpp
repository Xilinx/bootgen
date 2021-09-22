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
#include <iomanip>
#include <iostream>
#include "readimage-versal.h"
#include "authkeys-versal.h"
#include "Keccak-compact.h"
#include "authentication-versal.h"
#include "Keccak-compact-versal.h"
#include "partitionheadertable.h"
#include "checksum.h"


/*******************************************************************************/
void VersalReadImage::VerifyAuthentication(bool verifyImageOption)
{
    ReadBinaryFile();

    if (iHT->headerAuthCertificateWordOffset != 0)
    {
        VerifyHeaderTableSignature();
    }
    else
    {
        //LOG_ERROR("Bootimage %s is not authenticated. Authentication verification cannot be done on this image.", binFilename.c_str());
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

/******************************************************************************************************************************/
bool VersalReadImage::VerifyECDSASignature(bool nist, uint8_t * data, size_t dataLength, ACKeyECDSA *eckey, uint8_t* signature)
{
    uint8_t shaHash[SHA3_LENGTH_BYTES] = { 0 };
    Versalcrypto_hash(shaHash, data, dataLength, nist);

    BIGNUM *qx = BN_bin2bn(eckey->x, sizeof(eckey->x), NULL);
    BIGNUM *qy = BN_bin2bn(eckey->y, sizeof(eckey->y), NULL);

    BIGNUM *qr = BN_bin2bn(signature, EC_P384_KEY_LENGTH, NULL);
    BIGNUM *qs = BN_bin2bn(signature + EC_P384_KEY_LENGTH, EC_P384_KEY_LENGTH, NULL);

    EC_KEY *eckeyLocal = NULL;
    eckeyLocal = EC_KEY_new_by_curve_name(NID_secp384r1);
    EC_KEY_set_public_key_affine_coordinates(eckeyLocal, qx, qy);
    EC_KEY_set_asn1_flag(eckeyLocal, OPENSSL_EC_NAMED_CURVE);

    if (!EC_KEY_check_key(eckeyLocal))
    {
        LOG_ERROR("Failed to check EC Key\n");
    }

    ECDSA_SIG* ec_signature = ECDSA_SIG_new();
    if (ec_signature != NULL)
    {
#if OPENSSL_VERSION_NUMBER > 0x10100000L
        ECDSA_SIG_set0(ec_signature, qr, qs);
#else
        ec_signature->r = qr;
        ec_signature->s = qs;
#endif
    }

    if (1 != ECDSA_do_verify(shaHash, SHA3_LENGTH_BYTES, ec_signature, eckeyLocal))
    {
        return false;
    }
    else
    {
        return true;
    }

    BN_free(qx);
    BN_free(qy);
    BN_free(qr);
    BN_free(qs);
    EC_KEY_free(eckeyLocal);
    ECDSA_SIG_free(ec_signature);
}

/******************************************************************************************************/
bool VersalReadImage::VerifyECDSAP521Signature(bool nist, uint8_t * data, size_t dataLength, ACKeyECDSAP521 *eckey, uint8_t* signature)
{
    uint8_t shaHash[SHA3_LENGTH_BYTES] = { 0 };
    Versalcrypto_hash(shaHash, data, dataLength, nist);

    BIGNUM *qx = BN_bin2bn(eckey->x, sizeof(eckey->x), NULL);
    BIGNUM *qy = BN_bin2bn(eckey->y, sizeof(eckey->y), NULL);

    BIGNUM *qr = BN_bin2bn(signature, EC_P521_KEY_LENGTH2, NULL);
    BIGNUM *qs = BN_bin2bn(signature + EC_P521_KEY_LENGTH2, EC_P521_KEY_LENGTH2, NULL);

    EC_KEY *eckeyLocal = NULL;
    eckeyLocal = EC_KEY_new_by_curve_name(NID_secp521r1);
    EC_KEY_set_public_key_affine_coordinates(eckeyLocal, qx, qy);
    EC_KEY_set_asn1_flag(eckeyLocal, OPENSSL_EC_NAMED_CURVE);

    if (!EC_KEY_check_key(eckeyLocal))
    {
        LOG_ERROR("Failed to check EC Key\n");
    }

    ECDSA_SIG* ec_signature = ECDSA_SIG_new();

    if (ec_signature != NULL)
    {
#if OPENSSL_VERSION_NUMBER > 0x10100000L
        ECDSA_SIG_set0(ec_signature, qr, qs);
#else
        ec_signature->r = qr;
        ec_signature->s = qs;
#endif
    }

    if (1 != ECDSA_do_verify(shaHash, 48, ec_signature, eckeyLocal))
    {
        return false;
    }
    else
    {
        return true;
    }

    BN_free(qx);
    BN_free(qy);
    BN_free(qr);
    BN_free(qs);
    EC_KEY_free(eckeyLocal);
    ECDSA_SIG_free(ec_signature);
}

/*******************************************************************************/
bool VersalReadImage::VerifySignature(bool nist, uint8_t * data, size_t dataLength, ACKey4096Sha3Padding * acKey, uint8_t* signature)
{
    /* Find SHA-384 hash from data */
    uint8_t shaHash[SHA3_LENGTH_BYTES] = { 0 };
    Versalcrypto_hash(shaHash, data, dataLength, nist);

    rsa = RSA_new();
    BIGNUM *n = BN_bin2bn(acKey->N, sizeof(acKey->N), NULL);
    BIGNUM *e = BN_bin2bn(acKey->E, sizeof(acKey->E), NULL);

#if OPENSSL_VERSION_NUMBER > 0x10100000L
    BIGNUM *d = NULL;
    RSA_set0_key(rsa, n, e, d);
#else
    rsa->n = n;
    rsa->e = e;
#endif

    /* Find SHA-384 hash from signature */
    uint8_t opensslHashPadded[SIGN_LENGTH_VERSAL] = { 0 };

    int ret = RSA_public_encrypt(SIGN_LENGTH_VERSAL, (const unsigned char*)signature, (unsigned char*)opensslHashPadded, rsa, RSA_NO_PADDING);
    if (ret < 0)
    {
        LOG_ERROR("RSA_public_encrypt error");
    }

    if (opensslHashPadded[512 - 1] != 0xbc)
    {
        LOG_ERROR("Invalid EM");
    }

    int maskedDBLen = 463;
    uint8_t* Buffer = new uint8_t[48];
    uint8_t* maskDB = new uint8_t[maskedDBLen];
    memcpy(Buffer, opensslHashPadded + maskedDBLen, 48);
    uint8_t masK[463] = { 0 };
    RSA4096Sha3PaddingAuthenticationAlgorithm* s = NULL;
    if (s->MaskGenerationFunction(masK, maskedDBLen, Buffer, 48, EVP_sha384()) == -1)
    {
        LOG_ERROR("Authentication internal error");
    }

    memset(maskDB,0x0,maskedDBLen);
    memcpy(maskDB,opensslHashPadded,463);

    uint8_t *DB;
    DB = new uint8_t[maskedDBLen];
    for (int i = 0; i < 463; i++)
    {
        DB[i] = (masK[i] ^ maskDB[i]);
    }

    uint8_t salt[SALT_LENGTH] = { 0 };
    memcpy(salt,DB + 415,SALT_LENGTH);
    uint8_t padding1[PAD1_LENGTH];
    memset(padding1, 0, PAD1_LENGTH);

    //calculate mPad - add padding1 and salt to mHash
    uint8_t mPad[104];
    memset(mPad, 0, 104);
    memcpy(mPad, padding1, PAD1_LENGTH);
    memcpy(mPad + PAD1_LENGTH, shaHash, 48);
    memcpy(mPad + PAD1_LENGTH + 48, salt, SALT_LENGTH);

    uint8_t *m1;
    uint8_t maskPadHash[48];
    m1 = s -> AttachSHA3Padding(mPad, 104);
    Versalcrypto_hash(maskPadHash, m1, 208, false);

    uint8_t opensslHash[SHA3_LENGTH_BYTES] = { 0 };
    memcpy(opensslHash,opensslHashPadded + 463, SHA3_LENGTH_BYTES);

    /* compare openssl Hash with calculated shaHash */
    int compare = memcmp(maskPadHash, opensslHash, SHA3_LENGTH_BYTES);
    delete[] Buffer;
    delete[] maskDB;
    delete[] DB;
    return !compare;
}

/*******************************************************************************/
void VersalReadImage::VerifyHeaderTableSignature()
{
    Separator();
    LOG_MSG("Verifying Header Authentication Certificate");
    uint64_t offset = 0;
    size_t result;
    FILE *binFile;
    binFile = fopen(binFilename.c_str(), "rb");
    std::list<uint8_t*>::iterator auth_cert = aCs.begin();
    bool smap_header_found = false;

    /* Verifying Header SPK Signature */
    VerifySPKSignature(*auth_cert);

    /* Verifying IHT Signature */
    uint32_t iHTLength = sizeof(VersalImageHeaderTableStructure);
    uint8_t* tempIHBuffer = new uint8_t[iHTLength];
    bool signatureVerified = false;
    bH = new VersalBootHeaderStructure;
    result = fread(bH, 1, sizeof(VersalBootHeaderStructure), binFile);
    if ((bH->smapWords[0] == 0xDD000000) || (bH->smapWords[0] == 0x00DD0000) || (bH->smapWords[0] == 0x000000DD))
    {
        smap_header_found = true;
    }
    if (bH->widthDetectionWord != 0xAA995566)
    {
        delete bH;
        bH = NULL;
    }
    
    if(bH != NULL)
    {
        offset = bH->imageHeaderByteOffset;
    }
    else
    {
        if(smap_header_found)
        {
            offset = sizeof(VersalSmapWidthTable);
        }
        else
        {
            offset = 0x00;
        }
    }

    if (!(fseek(binFile, offset, SEEK_SET)))
    {
        result = fread(tempIHBuffer, 1, iHTLength, binFile);
        if (result != iHTLength)
        {
            LOG_ERROR("Error reading signature");
        }
    }

    if ((*(*auth_cert) & 0xF3) == 0x02)
    {
        signatureVerified = VerifyECDSASignature(true,tempIHBuffer,iHTLength,  (ACKeyECDSA *)(*auth_cert +  AC_SPK_KEY_OFFSET), *auth_cert + AC_BH_SIGN_OFFSET);
    }
    else if((*(*auth_cert) & 0xF3) == 0x22)
    {
        LOG_ERROR("Authentication verification of IHT cannot be done with ECDSA-P521 algorithm");
    }
    else if((*(*auth_cert) & 0xF3) == 0x11)
    {
        signatureVerified = VerifySignature(true, tempIHBuffer, iHTLength, (ACKey4096Sha3Padding*)(*auth_cert+ AC_SPK_KEY_OFFSET),*auth_cert + AC_BH_SIGN_OFFSET);
    }
    else
    {
        LOG_ERROR("Invalid Authentication Algorithm");
    }
    if (signatureVerified)
    {
        LOG_MSG("    Image Header Table Signature Verified");
    }
    else
    {
        LOG_MSG("    Image Header Table Signature Verification Failed");
        authenticationVerified = false;
        LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
    }
    delete[] tempIHBuffer;

    /* Partition Signature should not be included for hash calculation. */
    size_t headersSize = (iHT->totalMetaHdrLength * 4) - SIGN_LENGTH_VERSAL;
    size_t headersAcDataSize = sizeof(AuthCertificate4096Sha3PaddingStructure) - SIGN_LENGTH_VERSAL;
    uint8_t* tempBuffer = new uint8_t[headersSize];

    if(bH != NULL)
    {
        offset = bH->imageHeaderByteOffset + sizeof(VersalImageHeaderTableStructure);
    }
    else
    {
        if(smap_header_found)
        {
            offset = sizeof(VersalSmapWidthTable) + sizeof(VersalImageHeaderTableStructure);
        }
        else
        {
            offset = sizeof(VersalImageHeaderTableStructure);
        }
    }
    if (!(fseek(binFile, offset, SEEK_SET)))
    {
        result = fread(tempBuffer, 1, headersAcDataSize, binFile);
        if (result != headersAcDataSize)
        {
            LOG_ERROR("Error reading signature");
        }
    }

    offset = iHT->firstImageHeaderWordOffset * 4;
    if (!(fseek(binFile, offset, SEEK_SET)))
    {
        result = fread(tempBuffer + headersAcDataSize, 1, headersSize - headersAcDataSize, binFile);
        if (result != headersSize - headersAcDataSize)
        {
            LOG_ERROR("Error reading signature");
        }
    }

    if ((*(*auth_cert) & 0xF3) == 0x02)
    {
        signatureVerified = VerifyECDSASignature(true,tempBuffer,headersSize,  (ACKeyECDSA *)(*auth_cert +  AC_SPK_KEY_OFFSET), *auth_cert +  AC_PARTITION_SIGN_OFFSET);
    }
    else if((*(*auth_cert) & 0xF3) == 0x22)
    {
        LOG_ERROR("Authentication verification of Metaheader cannot be done with ECDSA-P521 algorithm");
    }
    else if((*(*auth_cert) & 0xF3) == 0x11)
    {
        signatureVerified = VerifySignature(true, tempBuffer, headersSize, (ACKey4096Sha3Padding *)(*auth_cert + AC_SPK_KEY_OFFSET), *auth_cert + AC_PARTITION_SIGN_OFFSET);
    }
    else
    {
        LOG_ERROR("Invalid Authentication Algorithm");
    }
    delete[] tempBuffer;

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
  }

/*******************************************************************************/
void VersalReadImage::VerifySPKSignature(uint8_t* aC)
{
    size_t size = 0;
    bool signatureVerified = false;

    size = sizeof(uint32_t) + sizeof(uint32_t) + VERSAL_ACKEY_STRUCT_SIZE + AC_SPK_SHA3PAD_SIZE;
    uint8_t* tempBuffer = new uint8_t[size];
    memcpy(tempBuffer, aC, sizeof(uint32_t) + sizeof(uint32_t));
    memcpy(tempBuffer + sizeof(uint32_t) + sizeof(uint32_t), aC + AC_SPK_KEY_OFFSET, VERSAL_ACKEY_STRUCT_SIZE+ AC_SPK_SHA3PAD_SIZE);

    uint8_t temp = 0;
    memcpy(&temp, aC, 1);

    if ((temp & 0xF3) == 0x02)
    {
        signatureVerified = VerifyECDSASignature(false,tempBuffer,size,  (ACKeyECDSA *)(aC +  AC_PPK_KEY_OFFSET),  aC +  AC_SPK_SIGN_OFFSET);
    }
    else if ((temp & 0xF3) == 0x22)
    {
        signatureVerified = VerifyECDSAP521Signature(false,tempBuffer,size,  (ACKeyECDSAP521 *)(aC +  AC_PPK_KEY_OFFSET),  aC +  AC_SPK_SIGN_OFFSET);
    }
    else if ((temp & 0xF3) == 0x11)
    {
        signatureVerified = VerifySignature(false, tempBuffer, size, (ACKey4096Sha3Padding *)(aC +  AC_PPK_KEY_OFFSET), aC +  AC_SPK_SIGN_OFFSET);
    }
    else
    {
        LOG_ERROR("Invalid Authentication Algorithm");
    }
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
void VersalReadImage::VerifyPartitionSignature(void)
{
    size_t result;
    uint64_t offset = 0;
    bool signatureVerified = false;

    std::list<uint8_t*>::iterator auth_cert = aCs.begin();
    auth_cert++;

    FILE *binFile;
    binFile = fopen(binFilename.c_str(), "rb");

    bH = new VersalBootHeaderStructure;
    result = fread(bH, 1, sizeof(VersalBootHeaderStructure), binFile);

    /* Partition Extraction */
    std::list<VersalPartitionHeaderTableStructure*>::iterator partitionHdr = pHTs.begin();
    for (std::list<VersalImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
    {
        for ( uint32_t cnt_index = 0; cnt_index < (*iH)->dataSectionCount; cnt_index++,partitionHdr++,auth_cert++)
        {
            if ((*partitionHdr)->authCertificateOffset != 0)
            {
                Separator();
                std::cout << "Verifying Partition " << (*iH)->imageName << "." << std::dec << cnt_index  << std::endl;
                bool isItBootloader = ((*partitionHdr)->partitionWordOffset) * 4 == bH->sourceOffset ? 1 : 0;
                if (isItBootloader)
                {
                    uint32_t bHLength = sizeof(VersalBootHeaderStructure) - sizeof(VersalSmapWidthTable);
                    uint8_t* tempBHBuffer = new uint8_t[bHLength];
                    offset = sizeof(VersalSmapWidthTable);
                    if (!(fseek(binFile, offset, SEEK_SET)))
                    {
                        size_t result = fread(tempBHBuffer, 1, bHLength, binFile);
                        if (result != bHLength)
                        {
                            LOG_ERROR("Error reading boot header while verifying ");
                        }
                    }

                    if ((*(*auth_cert) & 0xF3) == 0x02)
                    {
                        signatureVerified = VerifyECDSASignature(false,tempBHBuffer,bHLength,  (ACKeyECDSA *)(*auth_cert +  AC_SPK_KEY_OFFSET),  *auth_cert + AC_BH_SIGN_OFFSET);
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x22)
                    {
                        LOG_ERROR("Authentication verification of BootHeader cannot be done with ECDSA-P521 algorithm");
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x11)
                    {
                        signatureVerified = VerifySignature(false, tempBHBuffer, bHLength, (ACKey4096Sha3Padding *)(*auth_cert + AC_SPK_KEY_OFFSET),*auth_cert +AC_BH_SIGN_OFFSET);
                    }
                    else
                    {
                        LOG_ERROR("Invalid Authentication Algorithm");
                    }
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
                VerifySPKSignature(*auth_cert);

                uint32_t actualSecureChunkSize = SECURE_32K_CHUNK - SHA3_LENGTH_BYTES;
                bool nist = true;
                /* Partition Signature should not be included for hash calculation. */
                uint32_t encryptedSize = ((*partitionHdr)->encryptedPartitionLength * 4);
                uint32_t dataBufferLength = ((*partitionHdr)->totalPartitionLength * 4) - SIGN_LENGTH_VERSAL;
                uint32_t acBufferLength = sizeof(AuthCertificate4096Sha3PaddingStructure) - SIGN_LENGTH_VERSAL;
                uint8_t* tempBuffer = new uint8_t[dataBufferLength];

                offset = (*partitionHdr)->authCertificateOffset * 4;
                if (!(fseek(binFile, offset, SEEK_SET)))
                {
                    result = fread(tempBuffer, 1, acBufferLength, binFile);
                    if (result != acBufferLength)
                    {
                        LOG_ERROR("Error reading partition for hash calculation");
                    }
                }
                else
                {
                    LOG_ERROR("Error parsing Partitions from BootImage file %s", binFilename.c_str());
                }

                offset = (*partitionHdr)->partitionWordOffset * 4;
                if (!(fseek(binFile, offset, SEEK_SET)))
                {
                    result = fread(tempBuffer + acBufferLength, 1, dataBufferLength - acBufferLength, binFile);
                    if (result != dataBufferLength - acBufferLength)
                    {
                        LOG_ERROR("Error reading partition for hash calculation");
                    }
                }
                else
                {
                    LOG_ERROR("Error parsing Partitions from BootImage file %s", binFilename.c_str());
                }
                if (encryptedSize <= actualSecureChunkSize || isItBootloader)
                {
                    if (isItBootloader)
                    {
                        nist = false;
                    }
                    if ((*(*auth_cert) & 0xF3) == 0x02)
                    {
                        signatureVerified = VerifyECDSASignature(nist, tempBuffer, dataBufferLength, (ACKeyECDSA *)(*auth_cert + AC_SPK_KEY_OFFSET), *auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x22)
                    {
                        signatureVerified = VerifyECDSAP521Signature(nist, tempBuffer, dataBufferLength, (ACKeyECDSAP521 *)(*auth_cert + AC_SPK_KEY_OFFSET), *auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x11)
                    {
                        signatureVerified = VerifySignature(nist, tempBuffer, dataBufferLength, (ACKey4096Sha3Padding *)(*auth_cert + AC_SPK_KEY_OFFSET), *auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else
                    {
                        LOG_ERROR("Invalid Authentication Algorithm");
                    }
                }
                else
                {
                    uint32_t chunk0Size = acBufferLength + actualSecureChunkSize + SHA3_LENGTH_BYTES;
                    if ((*partitionHdr)->partitionKeySource != KeySource::None)
                    {
                        chunk0Size += SECURE_HDR_SZ + AES_GCM_TAG_SZ;
                    }
                    /* Verify Signature */
                    if ((*(*auth_cert) & 0xF3) == 0x02)
                    {
                        signatureVerified = VerifyECDSASignature(true, tempBuffer, chunk0Size, (ACKeyECDSA *)(*auth_cert + AC_SPK_KEY_OFFSET),*auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x22)
                    {
                        signatureVerified = VerifyECDSAP521Signature(true, tempBuffer, chunk0Size, (ACKeyECDSAP521 *)(*auth_cert + AC_SPK_KEY_OFFSET),*auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x11)
                    {
                        signatureVerified = VerifySignature(true, tempBuffer, chunk0Size, (ACKey4096Sha3Padding *)(*auth_cert + AC_SPK_KEY_OFFSET),*auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else
                    {
                        LOG_ERROR("Invalid Authentication Algorithm");
                    }

                    if (signatureVerified)
                    {
                        uint32_t dataSize = actualSecureChunkSize + SHA3_LENGTH_BYTES;
                        uint8_t* hashBuffer = new uint8_t[SHA3_LENGTH_BYTES];
                        if ((*partitionHdr)->partitionKeySource != KeySource::None)
                        {
                            encryptedSize -= (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                        }
                        int count = encryptedSize / actualSecureChunkSize;
                        if (encryptedSize % actualSecureChunkSize != 0)
                        {
                            count = count + 1;
                        }

                        for (int i = 1;i < count; i++)
                        {
                            uint32_t lastChunkSize = encryptedSize - ((count - 1) * actualSecureChunkSize);
                            if(i == count - 1)
                            {
                                dataSize = lastChunkSize;
                            }
                            uint8_t* dataBuffer = new uint8_t[dataSize];
                            memcpy(hashBuffer, tempBuffer + chunk0Size - SHA3_LENGTH_BYTES + (SECURE_32K_CHUNK) * (i-1), SHA3_LENGTH_BYTES);
                            memcpy(dataBuffer, tempBuffer + chunk0Size + (SECURE_32K_CHUNK) * (i-1), dataSize);
                            uint8_t* shaHash = new uint8_t[SHA3_LENGTH_BYTES];
                            Versalcrypto_hash(shaHash, dataBuffer, dataSize, true);
                            int compare = memcmp(shaHash, hashBuffer, SHA3_LENGTH_BYTES);
                            delete[] shaHash;

                            if (compare != 0)
                            {
                                LOG_ERROR("    Partition Verification Failed at Chunk %d", i);
                            }
                            delete[] dataBuffer;
                        }
                        delete[] hashBuffer;
                    }
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
                delete[] tempBuffer;
      }
      else
      {
          //EXIT
      }
    }
  }
  fclose(binFile);
  Separator();
}

