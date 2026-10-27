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
#include <memory>
#include <vector>
#include <iomanip>
#include <iostream>
#include "readimage-versal.h"
#include "authkeys-versal.h"
#include "Keccak-compact.h"
#include "authentication-versal.h"
#include "Keccak-compact-versal.h"
#include "partitionheadertable.h"
#include "checksum.h"
extern "C" {
#include "elftools.h"
}

/******************************************************************************/
static size_t VersalGetMetaHeadersBodySize(const VersalImageHeaderTableStructure* iHTable)
{
    uint32_t ihSize = ((iHTable->headerSizes >> 8) & 0xFF) * sizeof(uint32_t);
    uint32_t phtSize = ((iHTable->headerSizes >> 16) & 0xFF) * sizeof(uint32_t);
    return (iHTable->imageTotalCount * ihSize) + (iHTable->partitionTotalCount * phtSize);
}

/******************************************************************************/
static bool VersalImageUsesAuthOptimization(FILE* binFile, uint64_t ihtByteOffset,
                                            const VersalImageHeaderTableStructure* iHTable)
{
    if (iHTable->optionalDataSize == 0)
    {
        return false;
    }

    uint64_t offset = ihtByteOffset + sizeof(VersalImageHeaderTableStructure);
    uint32_t bytesRead = 0;
    const uint32_t optionalDataBytes = iHTable->optionalDataSize * sizeof(uint32_t);

    while (bytesRead < optionalDataBytes)
    {
        uint32_t sectnSizeId = 0;
        if (fseek(binFile, (long)offset, SEEK_SET) != 0 ||
            fread(&sectnSizeId, 1, sizeof(sectnSizeId), binFile) != sizeof(sectnSizeId))
        {
            return false;
        }

        uint32_t dataId = sectnSizeId & 0xFFFF;
        if (dataId == DATA_ID_PARTITION_HASHES)
        {
            return true;
        }

        uint32_t sectnLength = ((sectnSizeId & 0xFFFF0000) >> 16) * sizeof(uint32_t);
        if (sectnLength == 0)
        {
            return false;
        }

        offset += sectnLength;
        bytesRead += sectnLength;
    }

    return false;
}

/******************************************************************************/
static uint64_t VersalGetImageHeaderTableByteOffset(bool smapHeaderFound, const VersalBootHeaderStructure* bootHeader)
{
    if (bootHeader != nullptr)
    {
        return bootHeader->imageHeaderByteOffset;
    }

    if (smapHeaderFound)
    {
        return sizeof(VersalSmapWidthTable);
    }

    return 0;
}

/******************************************************************************/
/* Rebuild the secure-chunk hash chain and compare against H1 (the digest
 * stored immediately after the first chunk). Matches VersalPartition::StackAndAlign
 * in partition-versal.cpp: chunk0 | H1 | chunk1 | H2 | ... | chunkN, where
 * H_n = SHA3(chunkN) and H_i = SHA3(chunk_i || H_{i+1}). */
static bool VersalVerifyPartitionChunkChain(const uint8_t* regionStart, uint32_t totalSize,
    uint32_t actualPlainSize, bool encrypted, const uint8_t* expectedDigest,
    bool applyEncryptedFirstOverhead)
{
    const uint32_t stride = SECURE_32K_CHUNK;
    const uint32_t scs = stride - SHA3_LENGTH_BYTES;
    uint8_t digest[SHA3_LENGTH_BYTES] = { 0 };

    if (totalSize == 0)
    {
        return memcmp(digest, expectedDigest, SHA3_LENGTH_BYTES) == 0;
    }

    if (!encrypted)
    {
        if (totalSize < actualPlainSize
            || ((totalSize - actualPlainSize) % SHA3_LENGTH_BYTES) != 0)
        {
            return false;
        }

        uint32_t numChunks = (totalSize - actualPlainSize) / SHA3_LENGTH_BYTES + 1;

        if (numChunks == 1)
        {
            Versalcrypto_hash(digest, regionStart, actualPlainSize, true);
        }
        else
        {
            uint32_t lastSize = totalSize - (numChunks - 1) * stride;
            const uint8_t* lastChunk = regionStart + (uint64_t)(numChunks - 1) * stride;
            Versalcrypto_hash(digest, lastChunk, lastSize, true);

            auto buf = std::make_unique<uint8_t[]>(scs + SHA3_LENGTH_BYTES);
            for (int i = (int)numChunks - 1; i >= 1; --i)
            {
                const uint8_t* chunk = regionStart + (uint64_t)(i - 1) * stride;
                memcpy(buf.get(), chunk, scs);
                memcpy(buf.get() + scs, digest, SHA3_LENGTH_BYTES);
                Versalcrypto_hash(digest, buf.get(), scs + SHA3_LENGTH_BYTES, true);
            }
        }
    }
    else
    {
        const uint32_t firstExtra = SECURE_HDR_SZ + AES_GCM_TAG_SZ;
        std::vector<std::pair<uint32_t, uint32_t> > chunks;

        uint32_t pos = 0;
        uint32_t remaining = totalSize;
        bool first = true;
        while (remaining > 0)
        {
            uint32_t want = scs + ((first && applyEncryptedFirstOverhead) ? firstExtra : 0);
            if ((uint64_t)want + SHA3_LENGTH_BYTES >= remaining)
            {
                chunks.push_back(std::make_pair(pos, remaining));
                break;
            }
            chunks.push_back(std::make_pair(pos, want));
            pos += want + SHA3_LENGTH_BYTES;
            remaining -= want + SHA3_LENGTH_BYTES;
            first = false;
        }

        if (chunks.empty())
        {
            return false;
        }

        {
            uint32_t off = chunks.back().first;
            uint32_t sz = chunks.back().second;
            Versalcrypto_hash(digest, regionStart + off, sz, true);
        }

        for (int i = (int)chunks.size() - 2; i >= 0; --i)
        {
            uint32_t off = chunks[i].first;
            uint32_t sz = chunks[i].second;
            auto buf = std::make_unique<uint8_t[]>(sz + SHA3_LENGTH_BYTES);
            memcpy(buf.get(), regionStart + off, sz);
            memcpy(buf.get() + sz, digest, SHA3_LENGTH_BYTES);
            Versalcrypto_hash(digest, buf.get(), sz + SHA3_LENGTH_BYTES, true);
        }
    }

    return memcmp(digest, expectedDigest, SHA3_LENGTH_BYTES) == 0;
}

/*******************************************************************************/
void VersalReadImage::VerifyAuthentication(bool verifyImageOption)
{
    ReadBinaryFile();
    authenticationVerified = false;

    /* An image is considered authenticated if either the Image Header Table
       has an authentication certificate, or at least one partition has one
       (e.g. bh_auth_enable images do not sign the IHT but still sign
       per-partition data). If neither is present, the image is unauthenticated
       and -verify must reject it. */
    bool ihtAuthenticated = (iHT->headerAuthCertificateWordOffset != 0);
    bool anyPartitionAuthenticated = false;
    for (std::list<VersalPartitionHeaderTableStructure*>::iterator p = pHTs.begin(); p != pHTs.end(); ++p)
    {
        if ((*p) != NULL && (*p)->authCertificateOffset != 0)
        {
            anyPartitionAuthenticated = true;
            break;
        }
    }

    bool partitionVerifyPossible = (iHT->metaHdrKeySource == KeySource::None) || !iHs.empty();

    if (!ihtAuthenticated && !anyPartitionAuthenticated)
    {
        LOG_ERROR("Bootimage %s is not authenticated. Authentication verification cannot be done on this image.", binFilename.c_str());
    }
    else
    {
        bool checksPassed = true;

        if (ihtAuthenticated)
        {
            authenticationVerified = true;
            VerifyHeaderTableSignature();
            checksPassed = authenticationVerified;
        }
        else
        {
            LOG_WARNING("Bootimage %s header is not authenticated. Skipping header verification, proceeding with partition verification.", binFilename.c_str());
        }

        if (checksPassed)
        {
            if (!partitionVerifyPossible && (ihtAuthenticated || anyPartitionAuthenticated || iHT->partitionTotalCount > 0))
            {
                LOG_ERROR("Cannot verify partition/boot-header signatures: meta header is encrypted.");
                authenticationVerified = false;
            }
            else
            {
                authenticationVerified = true;
                VerifyPartitionSignature();
            }
        }
        else
        {
            authenticationVerified = false;
        }
    }

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
bool VersalReadImage::VerifyECDSASignature(bool nist, uint8_t * data, size_t dataLength, ACKeyECDSAp *eckey, uint8_t* signature)
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
bool VersalReadImage::VerifyECDSAP521Signature(bool nist, uint8_t * data, size_t dataLength, ACKeyECDSApP521 *eckey, uint8_t* signature)
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
    auto Buffer = std::make_unique<uint8_t[]>(48);
    auto maskDB = std::make_unique<uint8_t[]>(maskedDBLen);
    memcpy(Buffer.get(), opensslHashPadded + maskedDBLen, 48);
    uint8_t masK[463] = { 0 };
    RSA4096Sha3PaddingAuthenticationAlgorithm* s = NULL;
    if (s->MaskGenerationFunction(masK, maskedDBLen, Buffer.get(), 48, EVP_sha384()) == -1)
    {
        LOG_ERROR("Authentication internal error");
    }

    memset(maskDB.get(),0x0,maskedDBLen);
    memcpy(maskDB.get(),opensslHashPadded,463);

    auto DB = std::make_unique<uint8_t[]>(maskedDBLen);
    for (int i = 0; i < 463; i++)
    {
        DB[i] = (masK[i] ^ maskDB[i]);
    }

    uint8_t salt[SALT_LENGTH] = { 0 };
    memcpy(salt,DB.get() + 415,SALT_LENGTH);
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

    /* Verifying IHT Signature - match GenerateIHTHash (authentication-versal.cpp),
     * which hashes imageHeaderTable->section->Length (IHT + optional data). */
    uint32_t ihtOptionalDataLength = iHT->optionalDataSize * 4;
    uint32_t iHTLength = sizeof(VersalImageHeaderTableStructure) + ihtOptionalDataLength;
    auto tempIHBuffer = std::make_unique<uint8_t[]>(iHTLength);
    memset(tempIHBuffer.get(), 0, iHTLength);
    bool signatureVerified = false;
    bH = std::make_unique<VersalBootHeaderStructure>();
    result = fread(bH.get(), 1, sizeof(VersalBootHeaderStructure), binFile);
    if ((bH->smapWords[0] == 0xDD000000) || (bH->smapWords[0] == 0x00DD0000) || (bH->smapWords[0] == 0x000000DD))
    {
        smap_header_found = true;
    }
    if (bH->widthDetectionWord != 0xAA995566)
    {
        bH.reset();
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
        result = fread(tempIHBuffer.get(), 1, iHTLength, binFile);
        if (result != iHTLength)
        {
            LOG_ERROR("Error reading signature");
        }
    }
    else
    {
        LOG_ERROR("Error parsing Headers from BootImage file %s", binFilename.c_str());
    }

    if ((*(*auth_cert) & 0xF3) == 0x02)
    {
        signatureVerified = VerifyECDSASignature(true,tempIHBuffer.get(),iHTLength,  (ACKeyECDSAp *)(*auth_cert +  AC_SPK_KEY_OFFSET), *auth_cert + AC_BH_SIGN_OFFSET);
    }
    else if((*(*auth_cert) & 0xF3) == 0x22)
    {
        LOG_ERROR("Authentication verification of IHT cannot be done with ECDSA-P521 algorithm");
    }
    else if((*(*auth_cert) & 0xF3) == 0x11)
    {
        signatureVerified = VerifySignature(true, tempIHBuffer.get(), iHTLength, (ACKey4096Sha3Padding*)(*auth_cert+ AC_SPK_KEY_OFFSET),*auth_cert + AC_BH_SIGN_OFFSET);
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

    /* Meta-header signature covers [AC bytes used for signing] + [IH/PHT or encrypted
     * meta-header body], matching VersalAuthenticationContext::CopyPartitionSignature
     * for the "Headers" section in authentication-versal.cpp. */
    const uint64_t ihtByteOffset = VersalGetImageHeaderTableByteOffset(smap_header_found, bH.get());
    const bool authOptEnabled = VersalImageUsesAuthOptimization(binFile, ihtByteOffset, iHT.get());
    size_t headersBodySize = 0;
    if (iHT->metaHdrKeySource != KeySource::None)
    {
        headersBodySize = (iHT->totalMetaHdrLength * sizeof(uint32_t)) - sizeof(AuthCertificate4096Sha3PaddingStructure);
    }
    else
    {
        headersBodySize = VersalGetMetaHeadersBodySize(iHT.get());
    }

    size_t headersAcDataSize = authOptEnabled ? AC_BH_SIGN_OFFSET : AC_PARTITION_SIGN_OFFSET;
    size_t headersSize = headersAcDataSize + headersBodySize;
    auto tempBuffer = std::make_unique<uint8_t[]>(headersSize);
    memset(tempBuffer.get(), 0, headersSize);

    memcpy(tempBuffer.get(), *auth_cert, headersAcDataSize);

    offset = iHT->firstImageHeaderWordOffset * 4;
    if (!(fseek(binFile, offset, SEEK_SET)))
    {
        result = fread(tempBuffer.get() + headersAcDataSize, 1, headersBodySize, binFile);
        if (result != headersBodySize)
        {
            LOG_ERROR("Error reading signature");
        }
    }
    else
    {
        LOG_ERROR("Error parsing Headers from BootImage file %s", binFilename.c_str());
    }

    if ((*(*auth_cert) & 0xF3) == 0x02)
    {
        signatureVerified = VerifyECDSASignature(true, tempBuffer.get(), headersSize, (ACKeyECDSAp *)(*auth_cert +  AC_SPK_KEY_OFFSET), *auth_cert +  AC_PARTITION_SIGN_OFFSET);
    }
    else if((*(*auth_cert) & 0xF3) == 0x22)
    {
        LOG_ERROR("Authentication verification of Metaheader cannot be done with ECDSA-P521 algorithm");
    }
    else if((*(*auth_cert) & 0xF3) == 0x11)
    {
        signatureVerified = VerifySignature(true, tempBuffer.get(), headersSize, (ACKey4096Sha3Padding *)(*auth_cert + AC_SPK_KEY_OFFSET), *auth_cert + AC_PARTITION_SIGN_OFFSET);
    }
    else
    {
        LOG_ERROR("Invalid Authentication Algorithm");
    }

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
    auto tempBuffer = std::make_unique<uint8_t[]>(size);
    memcpy(tempBuffer.get(), aC, sizeof(uint32_t) + sizeof(uint32_t));
    memcpy(tempBuffer.get() + sizeof(uint32_t) + sizeof(uint32_t), aC + AC_SPK_KEY_OFFSET, VERSAL_ACKEY_STRUCT_SIZE+ AC_SPK_SHA3PAD_SIZE);

    uint8_t temp = 0;
    memcpy(&temp, aC, 1);

    if ((temp & 0xF3) == 0x02)
    {
        signatureVerified = VerifyECDSASignature(false,tempBuffer.get(),size,  (ACKeyECDSAp *)(aC +  AC_PPK_KEY_OFFSET),  aC +  AC_SPK_SIGN_OFFSET);
    }
    else if ((temp & 0xF3) == 0x22)
    {
        signatureVerified = VerifyECDSAP521Signature(false,tempBuffer.get(),size,  (ACKeyECDSApP521 *)(aC +  AC_PPK_KEY_OFFSET),  aC +  AC_SPK_SIGN_OFFSET);
    }
    else if ((temp & 0xF3) == 0x11)
    {
        signatureVerified = VerifySignature(false, tempBuffer.get(), size, (ACKey4096Sha3Padding *)(aC +  AC_PPK_KEY_OFFSET), aC +  AC_SPK_SIGN_OFFSET);
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

    bH = std::make_unique<VersalBootHeaderStructure>();
    result = fread(bH.get(), 1, sizeof(VersalBootHeaderStructure), binFile);

    /* Partition Extraction */
    std::list<VersalPartitionHeaderTableStructure*>::iterator partitionHdr = pHTs.begin();
    for (std::list<VersalImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
    {
        for ( uint32_t cnt_index = 0; cnt_index < (*iH)->dataSectionCount; cnt_index++,partitionHdr++,auth_cert++)
        {
            if ((*partitionHdr)->authCertificateOffset != 0)
            {
                signatureVerified = false;
                Separator();
                std::cout << "Verifying Partition " << (*iH)->imageName << "." << std::dec << cnt_index  << std::endl;
                bool isItBootloader = ((*partitionHdr)->partitionWordOffset) * 4 == bH->sourceOffset ? 1 : 0;
                if (isItBootloader)
                {
                    uint32_t bHLength = sizeof(VersalBootHeaderStructure) - sizeof(VersalSmapWidthTable);
                    auto tempBHBuffer = std::make_unique<uint8_t[]>(bHLength);
                    offset = sizeof(VersalSmapWidthTable);
                    if (!(fseek(binFile, offset, SEEK_SET)))
                    {
                        size_t result = fread(tempBHBuffer.get(), 1, bHLength, binFile);
                        if (result != bHLength)
                        {
                            LOG_ERROR("Error reading boot header while verifying ");
                        }
                    }
                    else
                    {
                        LOG_ERROR("Error parsing Boot Header from BootImage file %s", binFilename.c_str());
                    }

                    if ((*(*auth_cert) & 0xF3) == 0x02)
                    {
                        signatureVerified = VerifyECDSASignature(false,tempBHBuffer.get(),bHLength,  (ACKeyECDSAp *)(*auth_cert +  AC_SPK_KEY_OFFSET),  *auth_cert + AC_BH_SIGN_OFFSET);
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x22)
                    {
                        LOG_ERROR("Authentication verification of BootHeader cannot be done with ECDSA-P521 algorithm");
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x11)
                    {
                        signatureVerified = VerifySignature(false, tempBHBuffer.get(), bHLength, (ACKey4096Sha3Padding *)(*auth_cert + AC_SPK_KEY_OFFSET),*auth_cert +AC_BH_SIGN_OFFSET);
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
                }

                /* Verifying Partition SPK Signature */
                VerifySPKSignature(*auth_cert);

                /*  Match the data layout that generation actually
                 * signs in VersalAuthenticationContext::CopyPartitionSignature
                 * (authentication-versal.cpp).
                 *
                 *   data = [AC bytes excluding the partition signature slot]
                 *        + [partition body, length = hashSecLen]
                 *   hashSecLen = (section->firstChunkSize != 0 &&
                 *                 !(isBootloader && !IsVersalNetSeries()) &&
                 *                 section->isPartitionData)
                 *              ? section->firstChunkSize + hashLength
                 *              : section->Length;
                 *   nist = !(isBootloader && !IsVersalNetSeries());
                 *
                 * For VersalNet, this means EVERY authenticated partition
                 * (including the bootloader/PLM) is signed with NIST SHA-3
                 * padding over (AC + first chunk + chunk-0 hash). For classic
                 * Versal, the bootloader keeps the legacy Keccak-original
                 * padding over the entire body.
                 *
                 * The pre-existing verify formula
                 *   dataBufferLength = totalPartitionLength*4 - SIGN_LENGTH_VERSAL
                 *   nist = false   (for any bootloader)
                 * was both wrong size and wrong nist flag for the VersalNet
                 * bootloader, so signature verification of PLM (and any other
                 * multi-chunk authenticated partition) on VersalNet always
                 * failed.
                 */
                uint32_t bootloaderChunkSize = SECURE_16K_CHUNK - SHA3_LENGTH_BYTES;
                uint32_t actualSecureChunkSize = SECURE_32K_CHUNK - SHA3_LENGTH_BYTES;
                bool nist = !(isItBootloader && !versalNetSeries);
                uint32_t encryptedSize = ((*partitionHdr)->encryptedPartitionLength * 4);
                /* totalPartitionLength (per SetTotalPartitionLength) =
                 * body_len_words + sum of AC section sizes in words. The body
                 * (partition data only) is therefore the total minus AC.    */
                uint32_t acFullSize = sizeof(AuthCertificate4096Sha3PaddingStructure);
                uint32_t bodyLength = ((*partitionHdr)->totalPartitionLength * 4) - acFullSize;
                uint32_t acBufferLength = acFullSize - SIGN_LENGTH_VERSAL;
                uint32_t dataBufferLength = acBufferLength + bodyLength;
                auto tempBuffer = std::make_unique<uint8_t[]>(dataBufferLength);
                memset(tempBuffer.get(), 0, dataBufferLength);

                offset = (*partitionHdr)->authCertificateOffset * 4;
                if (!(fseek(binFile, offset, SEEK_SET)))
                {
                    result = fread(tempBuffer.get(), 1, acBufferLength, binFile);
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
                    result = fread(tempBuffer.get() + acBufferLength, 1, bodyLength, binFile);
                    if (result != bodyLength)
                    {
                        LOG_ERROR("Error reading partition for hash calculation");
                    }
                }
                else
                {
                    LOG_ERROR("Error parsing Partitions from BootImage file %s", binFilename.c_str());
                }

                /* Pick the slice of [AC|body] that generation signed. */
                uint32_t signedLen = dataBufferLength;
                if (isItBootloader && versalNetSeries)
                {
                    signedLen = acBufferLength + bootloaderChunkSize + SHA3_LENGTH_BYTES;
                    if ((*partitionHdr)->partitionKeySource != KeySource::None)
                    {
                        signedLen += SECURE_HDR_SZ + AES_GCM_TAG_SZ;
                    }
                }
                else if (isItBootloader && !versalNetSeries)
                {
                    /* Classic Versal bootloader: signs entire body, nist=false. */
                    signedLen = acBufferLength + bodyLength;
                }
                else if (encryptedSize > actualSecureChunkSize)
                {
                    /* Multi-chunk non-bootloader: signs first secure chunk only. */
                    signedLen = acBufferLength + actualSecureChunkSize + SHA3_LENGTH_BYTES;
                    if ((*partitionHdr)->partitionKeySource != KeySource::None)
                    {
                        signedLen += SECURE_HDR_SZ + AES_GCM_TAG_SZ;
                    }
                }
                else
                {
                    /* Single-chunk non-bootloader: hashSecLen = section->Length,
                     * i.e. the entire partition body.                          */
                    signedLen = acBufferLength + bodyLength;
                }

                if (signedLen > dataBufferLength)
                {
                    signedLen = dataBufferLength;
                }

                if (encryptedSize <= actualSecureChunkSize || isItBootloader)
                {
                    if ((*(*auth_cert) & 0xF3) == 0x02)
                    {
                        signatureVerified = VerifyECDSASignature(nist, tempBuffer.get(), signedLen, (ACKeyECDSAp *)(*auth_cert + AC_SPK_KEY_OFFSET), *auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x22)
                    {
                        signatureVerified = VerifyECDSAP521Signature(nist, tempBuffer.get(), signedLen, (ACKeyECDSApP521 *)(*auth_cert + AC_SPK_KEY_OFFSET), *auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x11)
                    {
                        signatureVerified = VerifySignature(nist, tempBuffer.get(), signedLen, (ACKey4096Sha3Padding *)(*auth_cert + AC_SPK_KEY_OFFSET), *auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else
                    {
                        LOG_ERROR("Invalid Authentication Algorithm");
                    }
                }
                else
                {
                    /* Multi-chunk non-bootloader: signedLen already covers AC +
                     * first 32K chunk + chunk-1 hash (+ optional SHE/GCM). The
                     * chunk-by-chunk hash chain below still uses the same
                     * actualSecureChunkSize as before.                       */
                    uint32_t chunk0Size = signedLen;
                    if ((*(*auth_cert) & 0xF3) == 0x02)
                    {
                        signatureVerified = VerifyECDSASignature(true, tempBuffer.get(), chunk0Size, (ACKeyECDSAp *)(*auth_cert + AC_SPK_KEY_OFFSET),*auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x22)
                    {
                        signatureVerified = VerifyECDSAP521Signature(true, tempBuffer.get(), chunk0Size, (ACKeyECDSApP521 *)(*auth_cert + AC_SPK_KEY_OFFSET),*auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else if ((*(*auth_cert) & 0xF3) == 0x11)
                    {
                        signatureVerified = VerifySignature(true, tempBuffer.get(), chunk0Size, (ACKey4096Sha3Padding *)(*auth_cert + AC_SPK_KEY_OFFSET),*auth_cert + AC_PARTITION_SIGN_OFFSET);
                    }
                    else
                    {
                        LOG_ERROR("Invalid Authentication Algorithm");
                    }

                    if (signatureVerified)
                    {
                        bool isEncrypted = ((*partitionHdr)->partitionKeySource != KeySource::None);
                        /* Must match the signed first-chunk body bytes (see signedLen above). */
                        uint32_t firstChunkDataSize = signedLen - acBufferLength - SHA3_LENGTH_BYTES;

                        const uint8_t* bodyStart = tempBuffer.get() + acBufferLength;
                        const uint8_t* expectedHeadDigest = bodyStart + firstChunkDataSize;
                        const uint8_t* chainRegion = expectedHeadDigest + SHA3_LENGTH_BYTES;
                        uint32_t chainTotalSize = bodyLength - firstChunkDataSize - SHA3_LENGTH_BYTES;

                        uint32_t chainPlainSize = 0;
                        if (!isEncrypted)
                        {
                            const uint32_t scs = SECURE_32K_CHUNK - SHA3_LENGTH_BYTES;
                            uint32_t chunkOnlength = (*partitionHdr)->encryptedPartitionLength * 4;
                            uint32_t dataChunksCount = (chunkOnlength / scs)
                                + ((chunkOnlength % scs) != 0 ? 1U : 0U);
                            uint32_t hashesInChain = (dataChunksCount > 2U)
                                ? (dataChunksCount - 2U) : 0U;
                            if (chainTotalSize >= hashesInChain * SHA3_LENGTH_BYTES)
                            {
                                chainPlainSize = chainTotalSize - (hashesInChain * SHA3_LENGTH_BYTES);
                            }
                        }

                        if (!VersalVerifyPartitionChunkChain(chainRegion, chainTotalSize,
                                chainPlainSize, isEncrypted, expectedHeadDigest, false))
                        {
                            LOG_ERROR("    Partition chunk hash-chain verification failed");
                            signatureVerified = false;
                        }
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

