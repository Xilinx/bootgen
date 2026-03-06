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
#include <limits>
#include <iomanip>
#include <iostream>
#include "string.h"

#include "authentication-zynqmp.h"
#include "bootgenexception.h"
#include "stringutils.h"
#include "binary.h"
#include "bootimage.h"
#include "options.h"


/*------------------------------------------------------------------------------
* BH Hash is always Keccak
    - Irrespective of SPK-eFuse/User-eFuse
    - Since BH Sign used by BootRoM
    - BH Hash is same for any partition as its always calculated on same data
* SPK can vary from Partition to Partition
    - Keccak/NIST is based of SPK-eFuse/User-eFuse selected for that partition.
* For Bootloader Partition - always authenticated by BootRoM
    - Always SPK-eFuse is used for Bootloader, so SPK Hash is always Keccak.
* Partition signatures are always authenticated by only FSBL/XilSecure/XilFPGA
    - Partition hashes - always NIST
-----------------------------------------------------------------------------*/

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
void ZynqMpAuthenticationContext::CopyPartitionSignature(BootImage& bi,
                                                         std::list<Section*> sections,
                                                         uint8_t* signatureBlock,
                                                         Section* acSection)
{
    /* Calculate the length of the partiton that is to be hashed,Since bs is authenticated in chunks.
       This can be changed with authblock[default is 8].*/
    LOG_TRACE("Copying the partition signature into Authentication Certificate");
    auto shaHash = std::make_unique<uint8_t[]>(hashLength);
    std::list<Section*>::iterator section = sections.begin();
    size_t hashSecLen = (*section)->Length;
    size_t authblocksize = authBlocks * 1024 * 1024;
    if (authblocksize != 0)
    {
        hashSecLen = authblocksize;
    }
    if (((*section)->Length) - (authblocksize * acIndex) < authblocksize && (authBlocks != 0))
    {
        hashSecLen = ((*section)->Length) - (authblocksize * acIndex);
    }

    /* Update with Partition and then AC */
    auto partitionAc = std::make_unique<uint8_t[]>(hashSecLen + (acSection->Length - signatureLength));

    /* Update with Partition */
    memcpy(partitionAc.get(), (*section)->Data.get() + (authblocksize * acIndex), hashSecLen);

    /* Update with authentication certificate except the last 256 bytes, which is the partition signature, 
       that we are calculating now. Once calculated, the partition signature will sit there */
    memcpy(partitionAc.get() + hashSecLen, acSection->Data.get(), acSection->Length - signatureLength);

    uint32_t start = (*section)->Address;
    uint32_t end = (acSection->Address + acSection->Length) - signatureLength;
    LOG_TRACE("Hashing %s from 0x%x to 0x%x", acSection->Name.c_str(), start, end);
    LOG_DUMP_BYTES((*section)->Data.get() + (authblocksize * acIndex), 32);
    LOG_OUT(" ... ");
    LOG_DUMP_BYTES((*section)->Data.get() + (authblocksize * acIndex) + hashSecLen - hashLength, hashLength);
    LOG_OUT(" ... \n");

    /* Calculate the hash */
    /*  Partition signatures are used by FSBL/XilSecure/XilFPGA except BL Sign-So partition hashes  always NIST except for bootloader
        Bootloader Sign is used by ROM - so Bootloader hash - always Keccak */
    hash->CalculateHash(!(*section)->isBootloader, partitionAc.get(), hashSecLen + (acSection->Length - signatureLength), shaHash.get());

    LOG_TRACE("Hash of %s (LE):", acSection->Name.c_str());
    LOG_DUMP_BYTES(shaHash.get(), hashLength);

    /* Create the PKCS padding for the hash */
    auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
    CreatePadding(shaHashPadded.get(), shaHash.get());

    if (bi.options.DoGenerateHashes())
    {
        std::string hashfilename = acSection->Name;
        WritePaddedSHAFile(shaHashPadded.get(), hashfilename);
    }
    RearrangeEndianess(shaHashPadded.get(), signatureLength);

    /*Sign the hash */
    authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)secondaryKey.get(), signatureBlock);
    RearrangeEndianess(signatureBlock, signatureLength);
    LOG_TRACE("The partition signature is copied into Authentication Certificate");

    /* Delete the temporarily created arrays */
    // Cleanup handled by unique_ptr
    // Cleanup handled by unique_ptr
    // Cleanup handled by unique_ptr
    acIndex++;
}

/******************************************************************************/
ZynqMpAuthenticationContext::ZynqMpAuthenticationContext()
{
    signatureLength = RSA_SIGN_LENGTH_ZYNQMP;
    SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
    hashType = AuthHash::Sha3;
    spksignature = std::make_unique<uint8_t[]>(signatureLength);
    spkSignLoaded = false;
    primaryKey = std::make_unique<Key4096>("Primary Key");
    secondaryKey = std::make_unique<Key4096>("Secondary Key");
    ownsPrimaryKey = true;     // We own these keys
    ownsSecondaryKey = true;   // We own these keys
    memset(spksignature.get(), 0, signatureLength);
    memset(udf_data, 0, UDF_DATA_SIZE);
    bHsignature = std::make_unique<uint8_t[]>(signatureLength);
    memset(bHsignature.get(), 0, signatureLength);
    bhSignLoaded = false;
    authAlgorithm = std::make_unique<RSAAuthenticationAlgorithm>();
    ownsAuthAlgorithm = true;  // We own this
    authCertificate = std::make_unique<RSA4096AuthenticationCertificate>(this);
    certSize = sizeof(AuthCertificate4096Structure);
}

/******************************************************************************/
ZynqMpAuthenticationContext::ZynqMpAuthenticationContext(const AuthenticationContext* refAuthContext)
{
    signatureLength = RSA_SIGN_LENGTH_ZYNQMP;
    SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
    hashType = AuthHash::Sha3;
    spksignature = std::make_unique<uint8_t[]>(signatureLength);
    bHsignature = std::make_unique<uint8_t[]>(signatureLength);
    ppkFile = refAuthContext->ppkFile;
    pskFile = refAuthContext->pskFile;
    spkFile = refAuthContext->spkFile;
    sskFile = refAuthContext->sskFile;
    spkSignFile = refAuthContext->spkSignFile;
    bhSignFile = refAuthContext->bhSignFile;
    ppkSelect = refAuthContext->ppkSelect;
    spkSelect = refAuthContext->spkSelect;
    spkIdentification = refAuthContext->spkIdentification;
    
    // Deep copy keys using Key copy constructor (copies ALL fields including D, P, Q)
    if (refAuthContext->primaryKey) {
        // Use static_cast since we know it's Key4096 (faster and safer than dynamic_cast)
        primaryKey = std::make_unique<Key4096>(*static_cast<Key4096*>(refAuthContext->primaryKey.get()));
    } else {
        primaryKey = std::make_unique<Key4096>("Primary Key");
    }
    ownsPrimaryKey = true;  // We own this copy
    
    if (spkFile != "" || sskFile != "")
    {
        // Create a new key by parsing the file
        secondaryKey = std::make_unique<Key4096>("Secondary Key");
        if (sskFile != "")
        {
            secondaryKey->ParseSecret(sskFile);
        }
        if (spkFile != "")
        {
            secondaryKey->ParsePublic(spkFile);
        }
        ownsSecondaryKey = true;  // We created a new key by parsing
    }
    else
    {
        // Deep copy secondaryKey using Key copy constructor (copies ALL fields)
        if (refAuthContext->secondaryKey) {
            // Use static_cast since we know it's Key4096
            secondaryKey = std::make_unique<Key4096>(*static_cast<Key4096*>(refAuthContext->secondaryKey.get()));
        } else {
            secondaryKey = std::make_unique<Key4096>("Secondary Key");
        }
        ownsSecondaryKey = true;  // We own this copy
    }

    if (spkSignFile != "")
    {
        SetSPKSignatureFile(spkSignFile);
    }
    else
    {
        memcpy(spksignature.get(), refAuthContext->spksignature.get(), signatureLength);
    }

    if (bhSignFile != "")
    {
        SetBHSignatureFile(bhSignFile);
    }
    else
    {
        memcpy(bHsignature.get(), refAuthContext->bHsignature.get(), signatureLength);
    }

    memcpy(udf_data, refAuthContext->udf_data, sizeof(udf_data));
    bhSignLoaded = refAuthContext->bhSignLoaded;
    spkSignLoaded = refAuthContext->spkSignLoaded;
    spkSignRequested = refAuthContext->spkSignRequested;
    certSize = sizeof(AuthCertificate4096Structure);
    preSigned = refAuthContext->preSigned;
    // Create our OWN authAlgorithm (deep copy instead of sharing to avoid use-after-free)
    authAlgorithm = std::make_unique<RSAAuthenticationAlgorithm>();
    ownsAuthAlgorithm = true;  // We own this
    // Create authCertificate with 'this' pointer so it can call back to this context
    authCertificate = std::make_unique<RSA4096AuthenticationCertificate>(this);
}

/******************************************************************************/
ZynqMpAuthenticationContext::ZynqMpAuthenticationContext(const AuthCertificate4096Structure* existingCert)
{
    signatureLength = RSA_SIGN_LENGTH_ZYNQMP;
    SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
    hashType = AuthHash::Sha3;
    spksignature = std::make_unique<uint8_t[]>(signatureLength);
    spkSignLoaded = true;
    bHsignature = std::make_unique<uint8_t[]>(signatureLength);
    bhSignLoaded = true;
    
    primaryKey = std::make_unique<Key4096>("Primary Key");
    secondaryKey = std::make_unique<Key4096>("Secondary Key");
    ownsPrimaryKey = true;     // We own these keys
    ownsSecondaryKey = true;   // We own these keys
    primaryKey->Import(&existingCert->acPpk, "Primary Key");
    secondaryKey->Import(&existingCert->acSpk, "Secondary Key");

    RearrangeEndianess(primaryKey->N.get(), sizeof(existingCert->acPpk.N));
    RearrangeEndianess(primaryKey->N_ext.get(), sizeof(existingCert->acPpk.N_extension));
    RearrangeEndianess(primaryKey->E.get(), sizeof(existingCert->acPpk.E));
    RearrangeEndianess(secondaryKey->N.get(), sizeof(existingCert->acSpk.N));
    RearrangeEndianess(secondaryKey->N_ext.get(), sizeof(existingCert->acSpk.N_extension));
    RearrangeEndianess(secondaryKey->E.get(), sizeof(existingCert->acSpk.E));

    memcpy(spksignature.get(), existingCert->acSpkSignature.Signature, signatureLength);
    memcpy(bHsignature.get(), existingCert->acBhSignature.Signature, signatureLength);
    memcpy(udf_data, existingCert->acUdf, UDF_DATA_SIZE);

    uint32_t acHdr = existingCert->acHeader;
    ppkSelect = 3 & (acHdr >> AC_HDR_PPK_SELECT_BIT_SHIFT);
    spkSelect = acHdr >> AC_HDR_SPK_SELECT_BIT_SHIFT;
    spkIdentification = existingCert->spkId;
    certSize = sizeof(AuthCertificate4096Structure);
    authAlgorithm = std::make_unique<RSAAuthenticationAlgorithm>();
    ownsAuthAlgorithm = true;  // We own this
}

/******************************************************************************/
ZynqMpAuthenticationContext::~ZynqMpAuthenticationContext()
{
    // spksignature, bHsignature, primaryKey, secondaryKey, hash, authAlgorithm
    // are all automatically destroyed when the object goes out of scope
    
    if (authCertificate != nullptr)
    {
        // Cleanup handled by unique_ptr
        authCertificate = nullptr;
    }
}

/******************************************************************************/
void ZynqMpAuthenticationContext::CreatePadding(uint8_t * signature, const uint8_t * hash)
{
    /*  SHA2 PKCS#1v1.5 Padding
        LSB------------------------------------------------------MSB
        0x0 || 0x1 || 0xFF(for 202 bytes) || 0x0 || tPad || SHA Hash

        SHA3 PKCS#1v1.5 Padding
        LSB------------------------------------------------------MSB
        0x0 || 0x1 || 0xFF(for 384 bytes) || 0x0 || tPad || SHA Hash 
    */

    uint8_t tPadSha2[T_PAD_LENGTH] = { 0x30, 0x31, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                       0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 };

    /* Older SHA3 ID used before 2016.1 */
    uint8_t tPadSha3[T_PAD_LENGTH] = { 0x30, 0x41, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                       0x65, 0x03, 0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30 };

    /* New SHA3 ID as per NIST */
    uint8_t tPadSha3new[T_PAD_LENGTH] = { 0x30, 0x41, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                          0x65, 0x03, 0x04, 0x02, 0x09, 0x05, 0x00, 0x04, 0x30 };

    uint8_t* sigPtr;
    uint8_t* tempSigPtr;
    uint32_t index;
    uint8_t* tPad;

    tempSigPtr = signature;
    if (hashType == AuthHash::Sha2)
    {
        tPad = tPadSha2;
    }
    else
    {
        /* If -zynqmpes1 is used, then old sha3 id is used, otherwise new id will be used */
        tPad = tPadSha3new;
        static bool warningGiven = false;
        if (AuthenticationContext::GetZynpMpVerEs1Flag())
        {
            tPad = tPadSha3;
            if (!warningGiven)
            {
                LOG_TRACE("ZynqMp silicon set to (1.0)ES1 using -zynqmpes1");
                LOG_WARNING("Authentication padding scheme is as per silicon (1.0)ES1. The image generated will NOT work for 2.0(ES2) and above.");
                warningGiven = true;
            }
        }
        else
        {
            if (!warningGiven)
            {
                LOG_WARNING("Authentication padding scheme will be as per silicon 2.0(ES2) and above. The image generated will NOT work for 1.0(ES1). \n	   Use '-zynqmpes1' to generate image for 1.0(ES1)");
                warningGiven = true;
            }
        }
    }

    sigPtr = signature;
    *sigPtr++ = 0x00;
    *sigPtr++ = 0x01;

    uint32_t totalFfs = signatureLength - T_PAD_LENGTH - hashLength - 1 - 1 - 1;
    for (index = 0; index < totalFfs; ++index)
    {
        *sigPtr++ = 0xFF;
    }
    *sigPtr++ = 0x00;
    for (index = 0; index < T_PAD_LENGTH; ++index)
    {
        *sigPtr++ = tPad[index];
    }
    for (index = 0; index < hashLength; ++index)
    {
        *sigPtr++ = hash[index];
    }
    signature = tempSigPtr;
}

/******************************************************************************/
Section* ZynqMpAuthenticationContext::CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection, bool isBootloader)
{
    LOG_INFO("Creating Authentication Certificate for section - %s", dataSection->Name.c_str());
    hashType = bi.GetAuthHashAlgo();
    hash = bi.hash.get();
    ownsHash = false;  // Non-owning pointer to bi.hash
    hashLength = hash->GetHashLength();
    std::string hashExtension = hash->GetHashFileExtension();

    /* Partition size must be 64-byte aligned */
    if ((dataSection->Length & 0x3F) != 0)
    {
        LOG_ERROR("Authentication Error !!!\n           Partition %s's length %d bytes - is not 64-byte aligned - %s", dataSection->Name.c_str(), dataSection->Length);
    }

    /* Secondary key is must for authenticating */
    if (!secondaryKey->Loaded)
    {
        LOG_ERROR("Authentication Error !!!\n           Secondary key must be specified in BIF file for %s", dataSection->Name.c_str());
    }

    if (!secondaryKey->isSecret)
    {
        if (presignFile != "")
        {
            // ok
        }
        else if (bi.options.GetNonBootingFlag() && (dataSection->Name.find("ImageHeaderTable") != std::string::npos))
        {
            // ok
        }
        else if (bi.options.DoGenerateHashes() || bi.options.GetNonBootingFlag())
        {
            static bool warningGiven = false;
            std::list<std::string> outFilename = bi.options.GetOutputFileNames();
            if (outFilename.size() > 0)
            {
                if (!warningGiven)
                {
                    LOG_WARNING("SSK is needed to authenticate a boot image. Because the key provided is not secret, the bootimage will not be usable. However, the sha hash files will be generated for offline signing");
                    warningGiven = true;
                }
            }
        }
        else if (bi.currentAuthCtx->spkSignRequested != "")
        {
            LOG_WARNING("SSK is needed to authenticate a boot image. Because the key provided is not secret, the bootimage will not be usable. However, SPK signature will be generated as requested.");
        }
        else
        {
            LOG_ERROR("Authentication Error !!!\n           SSK or partition must have [presign=xxx] attribute in BIF file for section %s", dataSection->Name.c_str());
        }
    }

    if (!(primaryKey->Loaded && primaryKey->isSecret) && !spkSignLoaded)
    {
        if (bi.options.DoGenerateHashes())
        {
            LOG_WARNING("Either PSK or spksignature is needed to authenticate bootimage or generate partition hashes. Because they are not provided, the bootimage and partition hashes will not be usable. However SPK hash and bootheader hash files generated can be used for offline signing.");
        }
        else
        {
            LOG_ERROR("Authentication Error !!!\n           Either PSK or SPK signature file must be specified in BIF file.");
        }
    }

    std::string name = dataSection->Name;
    if (dataSection->index == 0)
    {
        name = GetCertificateName(name);
    }
    auto acSection = std::make_unique<Section>(name + hashExtension, certSize);
    acSection->isCertificate = true;
    acSection->index = dataSection->index;
    AuthCertificate4096Structure* authCert = (AuthCertificate4096Structure*)acSection->Data.get();
    Section* acSectionPtr = acSection.get();
    LOG_TRACE("Creating new section for certificate - %s", acSection->Name.c_str());
    cache.Sections.push_back(std::move(acSection));

    uint32_t x = sizeof(AuthCertificate4096Structure);
    if (x != certSize)
    {
        LOG_DEBUG(DEBUG_STAMP, "Bad Authentication Certificate Size");
        LOG_ERROR("Authentication Error !!!");
    }
    memset(authCert, 0, certSize);

    uint32_t acHdr = AUTH_HDR_ZYNQMP;
    acHdr |= ppkSelect << AC_HDR_PPK_SELECT_BIT_SHIFT;
    acHdr |= spkSelect << AC_HDR_SPK_SELECT_BIT_SHIFT;
    acHdr |= ((bi.GetAuthHashAlgo()) ? 0 : 1) << AC_HDR_SHA_2_3_BIT_SHIFT;

    WriteLittleEndian32(&authCert->acHeader, acHdr);
    WriteLittleEndian32(&authCert->spkId, spkIdentification);

    if (udfFile != "")
    {
        LoadUdfData(udfFile, udf_data);
        RearrangeEndianess(udf_data, sizeof(udf_data));
        memcpy(authCert->acUdf, udf_data, UDF_DATA_SIZE);
    }

    primaryKey->Export(&authCert->acPpk);
    RearrangeEndianess(authCert->acPpk.N, sizeof(authCert->acPpk.N));
    RearrangeEndianess(authCert->acPpk.N_extension, sizeof(authCert->acPpk.N_extension));
    RearrangeEndianess(authCert->acPpk.E, sizeof(authCert->acPpk.E));
    secondaryKey->Export(&authCert->acSpk);
    RearrangeEndianess(authCert->acSpk.N, sizeof(authCert->acSpk.N));
    RearrangeEndianess(authCert->acSpk.N_extension, sizeof(authCert->acSpk.N_extension));
    RearrangeEndianess(authCert->acSpk.E, sizeof(authCert->acSpk.E));

    CopySPKSignature(&authCert->acSpkSignature);
    certIndex++;
    return acSectionPtr;
}

/******************************************************************************/
void ZynqMpAuthenticationContext::Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert)
{
    ::AuthCertificate4096Structure* authCert = (AuthCertificate4096Structure*)cert->section->Data.get();
    uint8_t* signatureBlock = (uint8_t*)&authCert->acPartitionSignature;
    CopyBHSignature(bi, &authCert->acBhSignature);

    if (presignFile == "")
    {
        /* If the parition is not presigned / presign file not present */
        std::list<Section*>::iterator section = sections.begin();
        (*section)->isBootloader = cert->fsbl;
        CopyPartitionSignature(bi, sections, signatureBlock, cert->section);
    }
    else
    {
        /* If the parition is presigned / presign file present */
        int index = acIndex;
        if (cert->section->index != 0)
        {
            index = cert->section->index;
        }
        GetPresign(presignFile, signatureBlock, index);
        acIndex++;
    }
}

/******************************************************************************/
void ZynqMpAuthenticationContext::CopyBHSignature(BootImage& bi, ACSignature4096* ptr)
{
    auto sha_hash_padded = std::make_unique<uint8_t[]>(signatureLength);
    auto bHsignaturetmp = std::make_unique<uint8_t[]>(signatureLength);

    GenerateBHHash(bi, sha_hash_padded.get());
    if (bi.options.DoGenerateHashes())
    {
        std::string hashfilename = "bootheader" + hash->GetHashFileExtension();
        WritePaddedSHAFile(sha_hash_padded.get(), hashfilename);
    }

    if (secondaryKey->Loaded && secondaryKey->isSecret)
    {
        LOG_TRACE("Creating Boot Header Signature");
        RearrangeEndianess(sha_hash_padded.get(), signatureLength);
        authAlgorithm->CreateSignature(sha_hash_padded.get(), (uint8_t*)secondaryKey.get(), bHsignaturetmp.get());
        RearrangeEndianess(bHsignaturetmp.get(), signatureLength);
        if (bhSignLoaded)
        {
            if (memcmp(bHsignature.get(), bHsignaturetmp.get(), signatureLength) != 0)
            {
                LOG_ERROR("Authentication Error !!!\n           Loaded BH Signature does not match calculated BH Signature");
            }
        }
        memcpy(ptr, bHsignaturetmp.get(), signatureLength);
    }
    else if (bhSignLoaded)
    {
        memcpy(ptr, bHsignature.get(), signatureLength);
    }
    else if (bi.options.DoGenerateHashes() && !bhSignLoaded)
    {
        static bool warningGiven = false;
        if (!warningGiven)
        {
            LOG_WARNING("Either SSK or (bhsignature and presign) is needed to authenticate a boot image or generate partition hashes. Because they are not provided, the bootimage and partition hashes will not be usable. However, the boot header hash file generated can be used for offline signing");
            warningGiven = true;
        }
    }
    else
    {
        LOG_ERROR("Authentication Error !!!\n          Either SSK or BH signature file must be specified in the BIF file.");
    }

    // Cleanup handled by unique_ptr
    LOG_TRACE("Boot Header Signature copied into Authentication Certificate");
}

/******************************************************************************/
void ZynqMpAuthenticationContext::GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded)
{
    LOG_TRACE("Calculating the Boot Header Hash");
    uint8_t* tmpBh = bi.bootHeader->section->Data.get();
    auto sha_hash = std::make_unique<uint8_t[]>(hashLength);
    hash->CalculateHash(false, tmpBh, bi.bootHeader->section->Length, sha_hash.get());
    CreatePadding(sha_hash_padded, sha_hash.get());
    // Cleanup handled by unique_ptr
}

/******************************************************************************/
void ZynqMpAuthenticationContext::GenerateSPKHash(uint8_t* shaHashPadded)
{
    LOG_TRACE("Calculating the SPK Hash");
    ACKey4096 spkFull;

    if (!secondaryKey->Loaded)
    {
        ParseSPKeyFile(spkFile);
    }
    secondaryKey->Export(&spkFull);
    hashLength = hash->GetHashLength();
    auto shaHash = std::make_unique<uint8_t[]>(hashLength);

    uint32_t acHdr = AUTH_HDR_ZYNQMP;
    acHdr |= ppkSelect << AC_HDR_PPK_SELECT_BIT_SHIFT;
    acHdr |= spkSelect << AC_HDR_SPK_SELECT_BIT_SHIFT;
    acHdr |= ((hashType == AuthHash::Sha2) ? 0 : 1) << AC_HDR_SHA_2_3_BIT_SHIFT;

    auto tempBuffer = std::make_unique<uint8_t[]>(sizeof(spkFull) + sizeof(acHdr) + sizeof(spkIdentification));
    WriteLittleEndian32(tempBuffer.get(), acHdr);
    WriteLittleEndian32(tempBuffer.get() + sizeof(acHdr), spkIdentification);
    RearrangeEndianess(spkFull.N, sizeof(spkFull.N));
    RearrangeEndianess(spkFull.N_extension, sizeof(spkFull.N_extension));
    RearrangeEndianess(spkFull.E, sizeof(spkFull.E));
    memcpy(tempBuffer.get() + sizeof(acHdr) + sizeof(spkIdentification), (uint8_t*)&spkFull, sizeof(spkFull));

    bool nist = false;
    if (spkSelect == 1)
    {
        nist = false;
    }
    else if (spkSelect == 2)
    {
        nist = true;
    }

    hash->CalculateHash(nist, (uint8_t*)tempBuffer.get(), sizeof(spkFull) + sizeof(acHdr) + sizeof(spkIdentification), shaHash.get());
    CreatePadding(shaHashPadded, shaHash.get());

    // Cleanup handled by unique_ptr
}

/******************************************************************************/
void ZynqMpAuthenticationContext::CopySPKSignature(ACSignature4096* ptr)
{
    CreateSPKSignature();
    LOG_TRACE("Copying the SPK signature into the Authentication Certificate");
    memcpy(ptr, spksignature.get(), signatureLength);
}

/******************************************************************************/
std::string ZynqMpAuthenticationContext::GetCertificateName(std::string name)
{
    if (certIndex != 0)
    {
        size_t x = name.find(".0");
        if (std::string::npos != name.find(".0"))
        {
            name[x + 1] = (char)(name[x + 1] + certIndex);
        }
    }
    return name;
}

/******************************************************************************/
void ZynqMpAuthenticationContext::GeneratePPKHash(const std::string& filename)
{
    ACKey4096 ppkTemp;
    primaryKey->Export(&ppkTemp);
    RearrangeEndianess(ppkTemp.N, sizeof(ppkTemp.N));
    RearrangeEndianess(ppkTemp.N_extension, sizeof(ppkTemp.N_extension));
    RearrangeEndianess(ppkTemp.E, sizeof(ppkTemp.E));

    hashLength = hash->GetHashLength();
    auto rsa_signature = std::make_unique<uint8_t[]>(hashLength);
    hash->CalculateHash(false, (uint8_t*)&ppkTemp, sizeof(ACKey4096), rsa_signature.get());

    FILE* filePtr;
    if ((filePtr = fopen(filename.c_str(), "w")) == NULL)
    {
        LOG_ERROR("-efuseppkbits error !!!           Failure writing to hash file %s", StringUtils::BaseName(filename).c_str());
    }

    for (int index = 0; index < hashLength; index++)
    {
        fprintf(filePtr, "%02X", rsa_signature[index]);
    }

    fprintf(filePtr, "\r\n");

    fclose(filePtr);
    // Cleanup handled by unique_ptr
    LOG_INFO("PPK Hash is written to file %s successfully", filename.c_str());
}

/******************************************************************************/
void ZynqMpAuthenticationContext::RearrangeEndianess(uint8_t *array, uint32_t size)
{
    uint32_t lastIndex = size - 1;
    char tempInt = 0;

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

/******************************************************************************/
void ZynqMpAuthenticationContext::AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header)
{
    for (std::list<AuthenticationCertificate*>::iterator acs = header->ac.begin(); acs != header->ac.end(); acs++)
    {
        if (*acs && (*acs)->section)
        {
            if (header->imageHeader->IsBootloader() == true)
            {
                header->imageHeader->SetTotalFsblFwSizeIh(header->imageHeader->GetTotalFsblFwSizeIh() + (*acs)->section->Length);
            }
        }
    }
}

/******************************************************************************/
void ZynqMpAuthenticationContext::SetKeyLength(Authentication::Type type)
{
    if (type == Authentication::RSA)
    {
        AuthenticationContext::authKeyLength = RSA_4096_KEY_LENGTH;
    }
}
