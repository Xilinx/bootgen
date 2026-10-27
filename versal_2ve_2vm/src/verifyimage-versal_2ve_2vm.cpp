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
#include <iomanip>
#include <iostream>
#include "readimage-versal_2ve_2vm.h"
#include "authkeys-versal_2ve_2vm.h"
#include "Keccak-compact.h"
#include "authentication-versal_2ve_2vm.h"
#include "Keccak-compact-versal_2ve_2vm.h"
#include "partitionheadertable.h"
#include "checksum.h"
#include "hash.h"
#include "encryptutils.h"
#include <vector>
#include <cstring>


/*
-------------------------------------------------------------------------------
******   T E L L U R I D E   C O N T E N T - D I G E S T   H E L P E R S   ***
-------------------------------------------------------------------------------
*/

/* NIST SHA3-384 (matches Versalcrypto_hash / CalculateAuthenticationHashDigest,
   which bootgen uses to build every hash-block slot digest). */
static void Versal_2ve_2vmSha3_384(uint8_t* out, const uint8_t* data, size_t length)
{
    Versal_2ve_2vmcrypto_hash(out, data, length, true);
}

/* Read len bytes at file offset into buf. Returns false on any short read so the
   caller fails loudly rather than verifying against stale/zero data. */
static bool Versal_2ve_2vmReadRegion(FILE* f, uint64_t offset, uint8_t* buf, size_t len)
{
    if (fseek(f, (long)offset, SEEK_SET) != 0)
    {
        return false;
    }
    return fread(buf, 1, len, f) == len;
}

/* A hash block is an array of (index:4 bytes)(digest:48 bytes) slots.
   Return a pointer to the digest of the given slot index. */
static const uint8_t* Versal_2ve_2vmHashBlockSlotDigest(const uint8_t* hashBlock, uint32_t index)
{
    return hashBlock + index * (HASH_BLOCK_INDEX_BYTES + SHA3_LENGTH_BYTES) + HASH_BLOCK_INDEX_BYTES;
}

/* Recompute a content region's secure-chunk-chain digest and compare it against
   the expected hash-block slot digest. On disk (bootloader, 16K chunking):
   chunk1 | H2 | chunk2 | H3 | ... | H_n | chunkN, where H_i = SHA3(chunk_i || H_{i+1})
   and H_n = SHA3(chunkN). We rebuild the whole chain from the raw bytes so any
   tamper fails loudly. Encrypted boot loaders chain over the ciphertext (no
   decryption); the first chunk carries an extra SECURE_HDR_SZ + AES_GCM_TAG_SZ
   of AES-GCM material (`encrypted` flag). */
static bool Versal_2ve_2vmVerifyRegionDigest(FILE* f, const char* name,
    uint64_t regionStart, uint32_t actualSize, uint32_t totalSize,
    const uint8_t* expectedDigest, bool encrypted)
{
    const uint32_t stride = SECURE_16K_CHUNK;           /* 0x4000 bootloader chunk       */
    const uint32_t scs    = stride - SHA3_LENGTH_BYTES; /* 0x3FD0 data bytes per chunk   */
    uint8_t digest[SHA3_LENGTH_BYTES] = { 0 };

    if (totalSize == 0)
    {
        LOG_ERROR("%s region has zero length; cannot verify its digest.", name);
        return false;
    }

    if (!encrypted)
    {
        if (totalSize < actualSize || ((totalSize - actualSize) % SHA3_LENGTH_BYTES) != 0)
        {
            LOG_ERROR("%s region has an inconsistent chunk layout (total 0x%X, actual 0x%X); "
                      "cannot verify its digest.", name, (unsigned)totalSize, (unsigned)actualSize);
            return false;
        }

        uint32_t numChunks = (totalSize - actualSize) / SHA3_LENGTH_BYTES + 1;

        if (numChunks == 1)
        {
            /* Whole region fits in a single chunk: digest = SHA3-384(region). */
            auto buf = std::make_unique<uint8_t[]>(actualSize ? actualSize : 1);
            if (!Versal_2ve_2vmReadRegion(f, regionStart, buf.get(), actualSize))
            {
                LOG_ERROR("Error reading %s region for digest verification.", name);
                return false;
            }
            Versal_2ve_2vmSha3_384(digest, buf.get(), actualSize);
        }
        else
        {
            /* Last chunk: h = SHA3-384(chunkN). */
            uint64_t lastOffset = regionStart + (uint64_t)(numChunks - 1) * stride;
            uint32_t lastSize = totalSize - (numChunks - 1) * stride;
            auto last = std::make_unique<uint8_t[]>(lastSize ? lastSize : 1);
            if (!Versal_2ve_2vmReadRegion(f, lastOffset, last.get(), lastSize))
            {
                LOG_ERROR("Error reading %s region (final chunk) for digest verification.", name);
                return false;
            }
            Versal_2ve_2vmSha3_384(digest, last.get(), lastSize);

            /* Chunks n-1 .. 1: h = SHA3-384(chunk_i || h). */
            auto buf = std::make_unique<uint8_t[]>(scs + SHA3_LENGTH_BYTES);
            for (int i = (int)numChunks - 1; i >= 1; --i)
            {
                uint64_t chunkOffset = regionStart + (uint64_t)(i - 1) * stride;
                if (!Versal_2ve_2vmReadRegion(f, chunkOffset, buf.get(), scs))
                {
                    LOG_ERROR("Error reading %s region (chunk %d) for digest verification.", name, i);
                    return false;
                }
                memcpy(buf.get() + scs, digest, SHA3_LENGTH_BYTES);
                Versal_2ve_2vmSha3_384(digest, buf.get(), scs + SHA3_LENGTH_BYTES);
            }
        }

        return memcmp(digest, expectedDigest, SHA3_LENGTH_BYTES) == 0;
    }

    /* Encrypted layout. Split the region into its on-disk chunks using totalSize.
       Each chunk contributes `dataLen` bytes to the chain and (unless it is the
       final chunk) is followed by a 48-byte link that we recompute rather than
       trust. The first chunk is larger by SECURE_HDR_SZ + AES_GCM_TAG_SZ. */
    const uint32_t firstExtra = SECURE_HDR_SZ + AES_GCM_TAG_SZ; /* 48 + 16 = 64 */

    std::vector<std::pair<uint64_t, uint32_t> > chunks; /* (offset, hashed data length) */
    uint64_t pos = regionStart;
    uint64_t remaining = totalSize;
    bool first = true;
    while (remaining > 0)
    {
        uint32_t want = scs + (first ? firstExtra : 0);
        if ((uint64_t)want + SHA3_LENGTH_BYTES >= remaining)
        {
            /* Not enough room for another full chunk plus its link: final chunk. */
            chunks.push_back(std::make_pair(pos, (uint32_t)remaining));
            break;
        }
        chunks.push_back(std::make_pair(pos, want));
        pos       += (uint64_t)want + SHA3_LENGTH_BYTES;
        remaining -= (uint64_t)want + SHA3_LENGTH_BYTES;
        first = false;
    }

    if (chunks.empty())
    {
        LOG_ERROR("%s region is empty; cannot verify its digest.", name);
        return false;
    }

    /* Last chunk: h = SHA3-384(chunkN). */
    {
        uint64_t off = chunks.back().first;
        uint32_t sz  = chunks.back().second;
        auto buf = std::make_unique<uint8_t[]>(sz ? sz : 1);
        if (!Versal_2ve_2vmReadRegion(f, off, buf.get(), sz))
        {
            LOG_ERROR("Error reading %s region (final chunk) for digest verification.", name);
            return false;
        }
        Versal_2ve_2vmSha3_384(digest, buf.get(), sz);
    }

    /* Chunks n-1 .. 1: h = SHA3-384(chunk_i || h). */
    for (int i = (int)chunks.size() - 2; i >= 0; --i)
    {
        uint64_t off = chunks[i].first;
        uint32_t sz  = chunks[i].second;
        auto buf = std::make_unique<uint8_t[]>((size_t)sz + SHA3_LENGTH_BYTES);
        if (!Versal_2ve_2vmReadRegion(f, off, buf.get(), sz))
        {
            LOG_ERROR("Error reading %s region (chunk %d) for digest verification.", name, i + 1);
            return false;
        }
        memcpy(buf.get() + sz, digest, SHA3_LENGTH_BYTES);
        Versal_2ve_2vmSha3_384(digest, buf.get(), (size_t)sz + SHA3_LENGTH_BYTES);
    }

    return memcmp(digest, expectedDigest, SHA3_LENGTH_BYTES) == 0;
}


/*******************************************************************************/
void Versal_2ve_2vmReadImage::VerifyAuthentication(bool verifyImageOption)
{
    ReadBinaryFile();

    /* An image is considered authenticated if either the Image Header Table
       has an authentication certificate, or at least one partition has one
       (e.g. bh_auth_enable images do not sign the IHT but still sign
       per-partition data). If neither is present, the image is unauthenticated
       and -verify must reject it. */
    bool ihtAuthenticated = (iHT->headerAuthCertificateWordOffset != 0);
    bool anyPartitionAuthenticated = false;
    for (std::list<Versal_2ve_2vmPartitionHeaderTableStructure*>::iterator p = pHTs.begin(); p != pHTs.end(); ++p)
    {
        if ((*p) != NULL && (*p)->authCertificateOffset != 0)
        {
            anyPartitionAuthenticated = true;
            break;
        }
    }

    if (!ihtAuthenticated && !anyPartitionAuthenticated)
    {
        LOG_ERROR("Bootimage %s is not authenticated. Authentication verification cannot be done on this image.", binFilename.c_str());
    }

    if (ihtAuthenticated)
    {
        VerifyHeaderTableSignature();
    }
    else
    {
        LOG_WARNING("Bootimage %s header is not authenticated. Skipping header verification, proceeding with partition verification.", binFilename.c_str());
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
bool Versal_2ve_2vmReadImage::VerifyECDSASignature(bool nist, uint8_t * data, size_t dataLength, ACKeyECDSA *eckey, uint8_t* signature)
{
    uint8_t shaHash[SHA3_LENGTH_BYTES] = { 0 };
    Versal_2ve_2vmcrypto_hash(shaHash, data, dataLength, nist);

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
bool Versal_2ve_2vmReadImage::VerifyECDSAP521Signature(bool nist, uint8_t * data, size_t dataLength, ACKeyECDSAP521 *eckey, uint8_t* signature)
{
    uint8_t shaHash[SHA3_LENGTH_BYTES] = { 0 };
    Versal_2ve_2vmcrypto_hash(shaHash, data, dataLength, nist);

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
bool Versal_2ve_2vmReadImage::VerifySignature(bool nist, uint8_t * data, size_t dataLength, ACKey4096Sha3Padding * acKey, uint8_t* signature)
{
    /* Find SHA-384 hash from data */
    uint8_t shaHash[SHA3_LENGTH_BYTES] = { 0 };
    Versal_2ve_2vmcrypto_hash(shaHash, data, dataLength, nist);

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
    RSA4096Sha3PaddingHBAuthenticationAlgorithm* s = NULL;
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
    Versal_2ve_2vmcrypto_hash(maskPadHash, m1, 208, false);

    uint8_t opensslHash[SHA3_LENGTH_BYTES] = { 0 };
    memcpy(opensslHash,opensslHashPadded + 463, SHA3_LENGTH_BYTES);

    /* compare openssl Hash with calculated shaHash */
    int compare = memcmp(maskPadHash, opensslHash, SHA3_LENGTH_BYTES);
    return !compare;
}

/*******************************************************************************/
void Versal_2ve_2vmReadImage::VerifyHeaderTableSignature()
{
    Separator();
    LOG_MSG("Verifying Header Authentication Certificate");
    uint64_t offset = 0;
    size_t result;
    FILE *binFile;
    binFile = fopen(binFilename.c_str(), "rb");
    std::list<uint8_t*>::iterator auth_cert = aCs.begin();
    bool smap_header_found = false;

    /* Auth algorithm for the metaheader AC comes from the IHT auth header
       (hash-block encoding 0x111-0x115), not the first byte of the AC. */
    uint32_t hdrAuthType = IdentifyAuthtype(iHT->authHeader1);

    /* Verifying Header SPK Signature */
    VerifySPKSignature(*auth_cert, hdrAuthType);

    /* Hash block model: the SPK signs SHA3-384 of the meta-header hash
       block (which in turn holds the SHA3 digests of the individual headers).
       There is no separate IHT/Header signature; verify the hash-block signature
       located immediately after the hash block in the file. */
    (void)smap_header_found;
    (void)offset;
    (void)result;
    uint64_t hbOffset = (uint64_t)iHT->hashBlockOffset * 4;
    uint32_t hbLength = iHT->hashBlockLength1 * 4;
    uint64_t hbSigOffset = hbOffset + hbLength;
    bool signatureVerified = VerifyHashBlockSignature(binFile, *auth_cert, hdrAuthType,
                                                      hbOffset, hbLength, hbSigOffset);
    if (signatureVerified)
    {
        LOG_MSG("    Meta Header Hash Block Signature Verified");
    }
    else
    {
        LOG_MSG("    Meta Header Hash Block Signature Verification Failed");
        authenticationVerified = false;
        LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
    }

    fclose(binFile);
  }

/*******************************************************************************/
bool Versal_2ve_2vmReadImage::VerifyHashBlockSignature(FILE* binFile, uint8_t* aC, uint32_t authType,
                                                       uint64_t hashBlockOffset, uint32_t hashBlockLength,
                                                       uint64_t sigOffset)
{
    /* SPK key offset within the hash block AC (per algorithm). */
    uint32_t spkKeyOffset;
    uint32_t sigLen;
    if (authType == Authentication::ECDSA)          { spkKeyOffset = 0x80;  sigLen = 2 * EC_P384_KEY_LENGTH; }
    else if (authType == Authentication::ECDSAp521) { spkKeyOffset = 0xB0;  sigLen = 2 * EC_P521_KEY_LENGTH2; }
    else if (authType == Authentication::RSA)       { spkKeyOffset = 0x430; sigLen = SIGN_LENGTH_VERSAL; }
    else { LOG_ERROR("Invalid Authentication Algorithm"); return false; }

    if (hashBlockLength == 0)
    {
        LOG_ERROR("Hash block length is zero; cannot verify hash block signature");
        return false;
    }

    auto hashBlock = std::make_unique<uint8_t[]>(hashBlockLength);
    if (fseek(binFile, (long)hashBlockOffset, SEEK_SET) != 0 ||
        fread(hashBlock.get(), 1, hashBlockLength, binFile) != hashBlockLength)
    {
        LOG_ERROR("Error reading hash block from bootimage");
        return false;
    }

    auto signature = std::make_unique<uint8_t[]>(sigLen);
    memset(signature.get(), 0, sigLen);
    if (fseek(binFile, (long)sigOffset, SEEK_SET) != 0 ||
        fread(signature.get(), 1, sigLen, binFile) != sigLen)
    {
        LOG_ERROR("Error reading hash block signature from bootimage");
        return false;
    }

    if (authType == Authentication::ECDSA)
    {
        return VerifyECDSASignature(true, hashBlock.get(), hashBlockLength,
                                    (ACKeyECDSA *)(aC + spkKeyOffset), signature.get());
    }
    else if (authType == Authentication::ECDSAp521)
    {
        return VerifyECDSAP521Signature(true, hashBlock.get(), hashBlockLength,
                                        (ACKeyECDSAP521 *)(aC + spkKeyOffset), signature.get());
    }
    return VerifySignature(true, hashBlock.get(), hashBlockLength,
                           (ACKey4096Sha3Padding *)(aC + spkKeyOffset), signature.get());
}

/*******************************************************************************/
void Versal_2ve_2vmReadImage::VerifySPKSignature(uint8_t* aC, uint32_t authType)
{
    bool signatureVerified = false;

    /* Hash block AC layout (per algorithm). The SPK signature is over
       NIST-SHA3-384 of the contiguous AC region starting at the SPK header
       fields {totalSpkSize, actualSpkSize, totalSpkSignSize, actualSpkSignSize,
       spkId} through the (aligned) SPK public key, i.e. AC[spkHdrOffset ..
       spkKeyOffset + keySize).  PPK/SPK/signature offsets differ per algorithm. */
    uint32_t ppkOffset = 0, spkHdrOffset = 0, spkKeyOffset = 0, spkSignOffset = 0, keySize = 0;

    if (authType == Authentication::ECDSA)
    {
        ppkOffset = 0x00; spkHdrOffset = 0x60; spkKeyOffset = 0x80; spkSignOffset = 0xE0;
        keySize = 2 * EC_P384_KEY_LENGTH;
    }
    else if (authType == Authentication::ECDSAp521)
    {
        ppkOffset = 0x00; spkHdrOffset = 0x8C; spkKeyOffset = 0xB0; spkSignOffset = 0x140;
        keySize = 2 * EC_P521_KEY_LENGTH2;
    }
    else if (authType == Authentication::RSA)
    {
        ppkOffset = 0x00; spkHdrOffset = 0x410; spkKeyOffset = 0x430; spkSignOffset = 0x840;
        keySize = VERSAL_ACKEY_STRUCT_SIZE;
    }
    else
    {
        LOG_ERROR("Invalid Authentication Algorithm");
        authenticationVerified = false;
        return;
    }

    size_t size = (spkKeyOffset - spkHdrOffset) + keySize;
    auto tempBuffer = std::make_unique<uint8_t[]>(size);
    memcpy(tempBuffer.get(), aC + spkHdrOffset, size);

    if (authType == Authentication::ECDSA)
    {
        signatureVerified = VerifyECDSASignature(true, tempBuffer.get(), size, (ACKeyECDSA *)(aC + ppkOffset), aC + spkSignOffset);
    }
    else if (authType == Authentication::ECDSAp521)
    {
        signatureVerified = VerifyECDSAP521Signature(true, tempBuffer.get(), size, (ACKeyECDSAP521 *)(aC + ppkOffset), aC + spkSignOffset);
    }
    else if (authType == Authentication::RSA)
    {
        signatureVerified = VerifySignature(true, tempBuffer.get(), size, (ACKey4096Sha3Padding *)(aC + ppkOffset), aC + spkSignOffset);
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
void Versal_2ve_2vmReadImage::VerifyPartitionSignature(void)
{
    size_t result;
    bool signatureVerified = false;

    FILE *binFile = fopen(binFilename.c_str(), "rb");
    if (binFile == NULL)
    {
        LOG_ERROR("Error opening BootImage file %s", binFilename.c_str());
        return;
    }

    /* Re-read the boot header for the BH hash-block offsets. */
    bH = std::make_unique<Versal_2ve_2vmBootHeaderStructure>();
    result = fread(bH.get(), 1, sizeof(Versal_2ve_2vmBootHeaderStructure), binFile);
    (void)result;

    /* aCs order (see ReadBinaryFile): [ BootHeader AC (or NULL),
       Meta Header AC (or NULL), then one AC per authenticated non-bootloader
       partition, in partition-header order ]. */
    std::list<uint8_t*>::iterator auth_cert = aCs.begin();

    /* -------- Bootloader (Boot Header) HashBlock verification -------- */
    uint32_t blAuthType = IdentifyAuthtype(bH->authHeader1);
    if (blAuthType != Authentication::None && auth_cert != aCs.end() && (*auth_cert) != NULL)
    {
        Separator();
        std::cout << "Verifying Boot Header Authentication Certificate" << std::endl;
        uint8_t* bhAC = *auth_cert;

        /* PPK -> SPK */
        VerifySPKSignature(bhAC, blAuthType);

        /* SPK -> Boot Header hash block. The BH AC sits right after the boot
           header; the hash block follows the AC, and its signature follows the
           hash block. */
        uint32_t aclen = GetACLength(sizeof(Versal_2ve_2vmBootHeaderStructure), bH->totalppkkSize1);
        uint64_t hbOffset = (uint64_t)sizeof(Versal_2ve_2vmBootHeaderStructure) + aclen;
        uint32_t hbLength = bH->hashBlockLength1;
        uint64_t hbSigOffset = hbOffset + hbLength;
        signatureVerified = VerifyHashBlockSignature(binFile, bhAC, blAuthType, hbOffset, hbLength, hbSigOffset);
        if (signatureVerified)
        {
            LOG_MSG("    Boot Header Hash Block Signature Verified");
        }
        else
        {
            LOG_MSG("    Boot Header Hash Block Signature Verification Failed");
            authenticationVerified = false;
            LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
        }

        /* -------- Content-digest layer (Hash Block 0 slots) --------
           The signature above proves Hash Block 0 is authentic; now confirm the
           actual on-disk content matches the digests recorded in its slots.
           HB0 slots (index:4 + digest:48, stride 52):
             0 = Boot Header, 1 = PLM, 2 = PMC CDO, 3 = Hash Block 1. */
        if (hbLength >= 4 * (HASH_BLOCK_INDEX_BYTES + SHA3_LENGTH_BYTES))
        {
            auto hashBlock = std::make_unique<uint8_t[]>(hbLength);
            if (Versal_2ve_2vmReadRegion(binFile, hbOffset, hashBlock.get(), hbLength))
            {
                bool encrypted = (bH->encryptionKeySource != 0);

                /* Slot 0: Boot Header digest = SHA3-384 over the boot header
                   excluding the 16-byte SMAP bus-width table. */
                uint32_t bhHashLength = sizeof(Versal_2ve_2vmBootHeaderStructure) - sizeof(Versal_2ve_2vmSmapWidthTable);
                auto bhBuffer = std::make_unique<uint8_t[]>(bhHashLength);
                if (Versal_2ve_2vmReadRegion(binFile, sizeof(Versal_2ve_2vmSmapWidthTable), bhBuffer.get(), bhHashLength))
                {
                    uint8_t bhDigest[SHA3_LENGTH_BYTES] = { 0 };
                    Versal_2ve_2vmSha3_384(bhDigest, bhBuffer.get(), bhHashLength);
                    if (memcmp(bhDigest, Versal_2ve_2vmHashBlockSlotDigest(hashBlock.get(), HASH_BLOCK_BH_HASH_INDEX), SHA3_LENGTH_BYTES) == 0)
                    {
                        LOG_MSG("    Boot Header Digest Verified");
                    }
                    else
                    {
                        LOG_MSG("    Boot Header Digest Verification Failed");
                        authenticationVerified = false;
                        LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
                    }
                }
                else
                {
                    LOG_ERROR("Error reading boot header for digest verification from %s", binFilename.c_str());
                }

                /* Slot 1: PLM digest (secure-chunk chain over the on-disk PLM). */
                if (bH->totalPlmLength != 0)
                {
                    if (Versal_2ve_2vmVerifyRegionDigest(binFile, "PLM", bH->sourceOffset,
                            bH->plmLength, bH->totalPlmLength,
                            Versal_2ve_2vmHashBlockSlotDigest(hashBlock.get(), HASH_BLOCK_PLM_HASH_INDEX), encrypted))
                    {
                        LOG_MSG("    PLM Digest Verified");
                    }
                    else
                    {
                        LOG_MSG("    PLM Digest Verification Failed");
                        authenticationVerified = false;
                        LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
                    }
                }

                /* Slot 2: PMC CDO digest. PMC data follows the PLM on disk. */
                if (bH->totalPmcCdoLength != 0)
                {
                    uint64_t pmcStart = (uint64_t)bH->sourceOffset + bH->totalPlmLength;
                    if (Versal_2ve_2vmVerifyRegionDigest(binFile, "PMC Data", pmcStart,
                            bH->pmcCdoLength, bH->totalPmcCdoLength,
                            Versal_2ve_2vmHashBlockSlotDigest(hashBlock.get(), HASH_BLOCK_PMCDATA_HASH_INDEX), encrypted))
                    {
                        LOG_MSG("    PMC Data Digest Verified");
                    }
                    else
                    {
                        LOG_MSG("    PMC Data Digest Verification Failed");
                        authenticationVerified = false;
                        LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
                    }
                }

                /* Slot 3: Hash Block 1 digest = SHA3-384 over the whole meta-header
                   hash block. This binds the meta-header chain to the bootloader chain. */
                if (iHT != NULL && (iHT->hashBlockLength1 * 4) != 0)
                {
                    uint32_t hb1Length = iHT->hashBlockLength1 * 4;
                    uint64_t hb1Offset = (uint64_t)iHT->hashBlockOffset * 4;
                    auto hb1 = std::make_unique<uint8_t[]>(hb1Length);
                    if (Versal_2ve_2vmReadRegion(binFile, hb1Offset, hb1.get(), hb1Length))
                    {
                        uint8_t hb1Digest[SHA3_LENGTH_BYTES] = { 0 };
                        Versal_2ve_2vmSha3_384(hb1Digest, hb1.get(), hb1Length);
                        if (memcmp(hb1Digest, Versal_2ve_2vmHashBlockSlotDigest(hashBlock.get(), HASH_BLOCK_HASHBLOCK1_HASH_INDEX), SHA3_LENGTH_BYTES) == 0)
                        {
                            LOG_MSG("    Hash Block 1 Digest Verified");
                        }
                        else
                        {
                            LOG_MSG("    Hash Block 1 Digest Verification Failed");
                            authenticationVerified = false;
                            LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
                        }
                    }
                    else
                    {
                        LOG_ERROR("Error reading Hash Block 1 for digest verification from %s", binFilename.c_str());
                    }
                }
            }
            else
            {
                LOG_ERROR("Error reading Hash Block 0 for content-digest verification from %s", binFilename.c_str());
            }
        }
    }

    /* -------- Per-partition HashBlock verification -------- */
    /* Advance to the first per-partition AC (skip BH AC and Meta Header AC). */
    std::list<uint8_t*>::iterator partAcIt = aCs.begin();
    if (partAcIt != aCs.end()) partAcIt++;   /* skip Boot Header AC */
    if (partAcIt != aCs.end()) partAcIt++;   /* skip Meta Header AC */

    std::list<Versal_2ve_2vmPartitionHeaderTableStructure*>::iterator partitionHdr = pHTs.begin();
    for (std::list<Versal_2ve_2vmImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
    {
        for (uint32_t cnt_index = 0; cnt_index < (*iH)->dataSectionCount; cnt_index++, partitionHdr++)
        {
            /* The bootloader/PLM partition is verified via the Boot Header above. */
            bool isItBootloader = ((*partitionHdr)->partitionWordOffset) * 4 == bH->sourceOffset;
            if (isItBootloader)
            {
                continue;
            }

            uint32_t partAuthType = IdentifyAuthtype((*partitionHdr)->authHeader1);
            if (partAuthType == Authentication::None)
            {
                /* No per-partition AC was recorded for this partition. */
                continue;
            }

            /* An AC was recorded for this authenticated partition; consume it to
               stay aligned with ReadBinaryFile's push order. */
            uint8_t* partAC = NULL;
            if (partAcIt != aCs.end())
            {
                partAC = *partAcIt;
                partAcIt++;
            }

            uint64_t partHbOffset = (uint64_t)(*partitionHdr)->hashBlockWordOffset * 4;
            uint32_t partHbLength = (*partitionHdr)->hashBlockLength1 * 4;

            /* Partitions without their own hash block are covered by the Meta
               Header hash block (their header digests live there). */
            if (partAC == NULL || partHbOffset == 0 || partHbLength == 0)
            {
                continue;
            }

            Separator();
            std::cout << "Verifying Partition " << (*iH)->imageName << "." << std::dec << cnt_index << std::endl;

            /* PPK -> SPK */
            VerifySPKSignature(partAC, partAuthType);

            /* SPK -> partition hash block */
            uint64_t partSigOffset = partHbOffset + partHbLength;
            signatureVerified = VerifyHashBlockSignature(binFile, partAC, partAuthType, partHbOffset, partHbLength, partSigOffset);
            if (signatureVerified)
            {
                LOG_MSG("    Partition Hash Block Signature Verified");
            }
            else
            {
                LOG_MSG("    Partition Hash Block Signature Verification Failed");
                authenticationVerified = false;
                LOG_ERROR("Authentication verification failed on bootimage %s", binFilename.c_str());
            }
        }
    }

    fclose(binFile);
    Separator();
}

