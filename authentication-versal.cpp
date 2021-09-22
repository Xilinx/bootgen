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
#include "authentication-versal.h"
#include "authentication.h"
#include "bootgenexception.h"
#include "stringutils.h"
#include "binary.h"
#include "bootimage.h"
#include <iomanip>
#include <iostream>
#include "string.h"
#include "options.h"
#include <limits>
#include "partitionheadertable-versal.h"
#include "imageheadertable-versal.h"
#include "Keccak-compact-versal.h"
#include "encryptutils.h"
#include <openssl/rand.h>

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
VersalAuthenticationContext::VersalAuthenticationContext(Authentication::Type type)
{
    signatureLength = SIGN_LENGTH_VERSAL;
    spksignature = new uint8_t[signatureLength];
    bHsignature = new uint8_t[signatureLength];
    spkSignLoaded = false;
    memset(udf_data, 0, UDF_DATA_SIZE);
    memset(bHsignature, 0, signatureLength);
    bhSignLoaded = false;
    certSize = sizeof(AuthCertificate4096Sha3PaddingStructure);
    hashType = AuthHash::Sha3;
    authAlgorithm = GetAuthenticationAlgorithm(type);
    if (type == Authentication::RSA)
    {
        primaryKey = new Key4096Sha3Padding("Primary Key");
        secondaryKey = new Key4096Sha3Padding("Secondary Key");
        primaryKey->authType = Authentication::RSA;
        secondaryKey->authType = Authentication::RSA;
    }
    else if (type == Authentication::ECDSA)
    {
        primaryKey = new KeyECDSA("Primary Key");
        secondaryKey = new KeyECDSA("Secondary Key");
        primaryKey->authType = Authentication::ECDSA;
        secondaryKey->authType = Authentication::ECDSA;
    }
    else
    {
        primaryKey = new KeyECDSAp521("Primary Key");
        secondaryKey = new KeyECDSAp521("Secondary Key");
        primaryKey->authType = Authentication::ECDSAp521;
        secondaryKey->authType = Authentication::ECDSAp521;
    }
}

/******************************************************************************/
VersalAuthenticationContext::VersalAuthenticationContext(const AuthenticationContext* refAuthContext, Authentication::Type authtype)
{
    signatureLength = SIGN_LENGTH_VERSAL;
    spksignature = new uint8_t[signatureLength];
    bHsignature = new uint8_t[signatureLength];

    authAlgorithm = GetAuthenticationAlgorithm(authtype);
    hashType = AuthHash::Sha3;
    ppkFile = refAuthContext->ppkFile;
    pskFile = refAuthContext->pskFile;
    spkFile = refAuthContext->spkFile;
    sskFile = refAuthContext->sskFile;
    spkSignFile = refAuthContext->spkSignFile;
    bhSignFile = refAuthContext->bhSignFile;
    spkSelect = refAuthContext->spkSelect;
    spkIdentification = refAuthContext->spkIdentification;

    if (authtype == Authentication::RSA)
    {
        primaryKey = new Key4096Sha3Padding("Primary Key");
        secondaryKey = new Key4096Sha3Padding("Secondary Key");
        primaryKey->authType = Authentication :: RSA;
        secondaryKey->authType = Authentication :: RSA;
    }
    else if(authtype == Authentication::ECDSA)
    {
        primaryKey = new KeyECDSA("Primary Key");
        secondaryKey = new KeyECDSA("Secondary Key");
        primaryKey->authType = Authentication :: ECDSA;
        secondaryKey->authType = Authentication :: ECDSA;
    }
    else
    {
        primaryKey = new KeyECDSAp521("Primary Key");
        secondaryKey = new KeyECDSAp521("Secondary Key");
        primaryKey->authType = Authentication :: ECDSAp521;
        secondaryKey->authType = Authentication :: ECDSAp521;
    }

    if (pskFile != "" || ppkFile != "")
    {
        if (pskFile != "")
        {
            primaryKey->ParseSecret(pskFile);
        }
        if (ppkFile != "")
        {
            primaryKey->ParsePublic(ppkFile);
        }
    }
    else
    {
        primaryKey = refAuthContext->primaryKey;
    }

    if (spkFile != "" || sskFile != "")
    {
        if (sskFile != "")
        {
            secondaryKey->ParseSecret(sskFile);
        }
        if (spkFile != "")
        {
            secondaryKey->ParsePublic(spkFile);
        }
    }
    else
    {
        secondaryKey = refAuthContext->secondaryKey;
    }
    if (spkSignFile != "")
    {
        SetSPKSignatureFile(spkSignFile);
    }
    else
    {
        memcpy(spksignature, refAuthContext->spksignature, signatureLength);
    }

    if (bhSignFile != "")
    {
        SetBHSignatureFile(bhSignFile);
    }
    else
    {
        memcpy(bHsignature, refAuthContext->bHsignature, signatureLength);
    }
    memcpy(udf_data, refAuthContext->udf_data, sizeof(udf_data));
    bhSignLoaded = refAuthContext->bhSignLoaded;
    spkSignLoaded = refAuthContext->spkSignLoaded;
    spkSignRequested = refAuthContext->spkSignRequested;
    certSize = sizeof(AuthCertificate4096Sha3PaddingStructure);
    preSigned = refAuthContext->preSigned;
}


/******************************************************************************/
VersalAuthenticationContext::VersalAuthenticationContext(const AuthCertificate4096Sha3PaddingStructure* existingCert, Authentication::Type authtype)
{
    signatureLength = SIGN_LENGTH_VERSAL;
    spksignature = new uint8_t[signatureLength];
    spkSignLoaded = true;
    bHsignature = new uint8_t[signatureLength];
    bhSignLoaded = true;
    authAlgorithm = GetAuthenticationAlgorithm(authtype);

    if (authtype == Authentication::RSA)
    {
        primaryKey = new Key4096Sha3Padding("Primary Key");
        secondaryKey = new Key4096Sha3Padding("Secondary Key");
        primaryKey->authType = Authentication::RSA;
        secondaryKey->authType = Authentication::RSA;
    }
    else if (authtype == Authentication::ECDSA)
    {
        primaryKey = new KeyECDSA("Primary Key");
        secondaryKey = new KeyECDSA("Secondary Key");
        primaryKey->authType = Authentication::ECDSA;
        secondaryKey->authType = Authentication::ECDSA;
    }
    else
    {
        primaryKey = new KeyECDSAp521("Primary Key");
        secondaryKey = new KeyECDSAp521("Secondary Key");
        primaryKey->authType = Authentication::ECDSAp521;
        secondaryKey->authType = Authentication::ECDSAp521;
    }

    primaryKey->Import(&existingCert->acPpk, "Primary Key");
    secondaryKey->Import(&existingCert->acSpk, "Secondary Key");
    hashType = AuthHash::Sha3;

    authAlgorithm->RearrangeEndianess(primaryKey->N, sizeof(existingCert->acPpk.N));
    authAlgorithm->RearrangeEndianess(primaryKey->N_ext, sizeof(existingCert->acPpk.N_extension));
    authAlgorithm->RearrangeEndianess(primaryKey->E, sizeof(existingCert->acPpk.E));

    authAlgorithm->RearrangeEndianess(secondaryKey->N, sizeof(existingCert->acSpk.N));
    authAlgorithm->RearrangeEndianess(secondaryKey->N_ext, sizeof(existingCert->acSpk.N_extension));
    authAlgorithm->RearrangeEndianess(secondaryKey->E, sizeof(existingCert->acSpk.E));
    memcpy(spksignature, existingCert->acSpkSignature.Signature, signatureLength);
    memcpy(bHsignature, existingCert->acHeaderSignature.Signature, signatureLength);
    memcpy(udf_data, existingCert->acUdf, UDF_DATA_SIZE);
    spkIdentification = existingCert->spkId;
    certSize = sizeof(AuthCertificate4096Sha3PaddingStructure);
}

/******************************************************************************/
AuthenticationAlgorithm* VersalAuthenticationContext::GetAuthenticationAlgorithm(Authentication::Type type)
{
    if (type == Authentication::ECDSA)
    {
        SetAuthenticationKeyLength(EC_P384_KEY_LENGTH);
        return new ECDSAAuthenticationAlgorithm();
    }
    else if (type == Authentication::ECDSAp521)
    {
        //SetAuthenticationKeyLength(EC_P521_KEY_LENGTH);
        return new ECDSAP521AuthenticationAlgorithm();
    }
    else 
    {
        SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
        return new RSA4096Sha3PaddingAuthenticationAlgorithm();
    }
}

/******************************************************************************/

uint32_t VersalAuthenticationContext::GetCertificateSize(void)
{ 
    return certSize; 
}

/******************************************************************************/
VersalAuthenticationContext::~VersalAuthenticationContext()
{
    if (spksignature != NULL)
    {
        delete[] spksignature;
        spksignature = nullptr;
    }

    if (bHsignature != NULL)
    {
        delete[] bHsignature;
        bHsignature = nullptr;
    }

    if (primaryKey != NULL)
    {
        delete primaryKey;
        primaryKey = nullptr;
    }

    if (secondaryKey != NULL)
    {
        delete secondaryKey;
        secondaryKey = nullptr;
    }
}

/******************************************************************************/
ECDSAAuthenticationAlgorithm::ECDSAAuthenticationAlgorithm()
{
    certSize = sizeof(AuthCertificateECDSAStructure);
    authType = Authentication::ECDSA;
}

/******************************************************************************/
ECDSAAuthenticationAlgorithm::~ECDSAAuthenticationAlgorithm()
{
}

/******************************************************************************/
ECDSAP521AuthenticationAlgorithm::ECDSAP521AuthenticationAlgorithm()
{
    certSize = sizeof(AuthCertificateECDSAp521Structure);
    authType = Authentication::ECDSAp521;
}

/******************************************************************************/
ECDSAP521AuthenticationAlgorithm::~ECDSAP521AuthenticationAlgorithm()
{
}

/******************************************************************************/
RSA4096Sha3PaddingAuthenticationAlgorithm::RSA4096Sha3PaddingAuthenticationAlgorithm()
{
    certSize = sizeof(AuthCertificate4096Sha3PaddingStructure);
    authType = Authentication::RSA;
}

/******************************************************************************/
RSA4096Sha3PaddingAuthenticationAlgorithm::~RSA4096Sha3PaddingAuthenticationAlgorithm()
{
}

/******************************************************************************/
static void FillSha3Padding(uint8_t* pad, uint32_t sha3PadLength)
{
    uint8_t *sha3 = new uint8_t[sha3PadLength];
    memset(sha3, 0, sha3PadLength);
    sha3[0] = 0x6;
    sha3[(sha3PadLength)-1] |= 0x80;
    memcpy(pad, sha3, sha3PadLength);
    delete[] sha3;
}

/******************************************************************************/
uint8_t* RSA4096Sha3PaddingAuthenticationAlgorithm::AttachSHA3Padding(uint8_t* data, const Binary::Length_t datalength)
{
    uint8_t sha3padding = SHA3_PAD_LENGTH - (datalength % SHA3_PAD_LENGTH);
    uint8_t *dataSha3 = new uint8_t[datalength + sha3padding];
    memset(dataSha3, 0, datalength + sha3padding);
    memcpy(dataSha3, data, datalength);
    dataSha3[datalength] = 0x6;
    dataSha3[(datalength + sha3padding) - 1] |= 0x80;
    return dataSha3;
}

/******************************************************************************/
int RSA4096Sha3PaddingAuthenticationAlgorithm::MaskGenerationFunction(unsigned char *mask, long len, const unsigned char *seed, long seedlen, const EVP_MD *dgst)
{
    long i, outlen = 0;
    unsigned char cnt[4];
    unsigned char md[SHA3_LENGTH_BYTES];
    int mdlen = SHA3_LENGTH_BYTES;
    int rv = -1;
    uint8_t* c = new uint8_t[52];
    uint8_t* cSha3Pad = NULL;
    if (mdlen < 0)
        goto err;
    for (i = 0; outlen < len; i++)
    {
        cnt[0] = (unsigned char)((i >> 24) & 255);
        cnt[1] = (unsigned char)((i >> 16) & 255);
        cnt[2] = (unsigned char)((i >> 8)) & 255;
        cnt[3] = (unsigned char)(i & 255);

        memcpy(c, seed, seedlen);
        memcpy(c + seedlen, cnt, 4);
        cSha3Pad = AttachSHA3Padding(c, (seedlen + 4));
        if (outlen + mdlen <= len)
        {
            Versalcrypto_hash(mask + outlen, cSha3Pad, 104, false);
            outlen += mdlen;
        }
        else
        {
            Versalcrypto_hash(md, cSha3Pad, 104, false);
            memcpy(mask + outlen, md, len - outlen);
            outlen = len;
        }
    }
    rv = 0;
    delete[] c;
    delete[] cSha3Pad;
err:
    return rv;
}

/******************************************************************************/
Section* VersalAuthenticationContext::CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection)
{
    LOG_INFO("Creating Authentication Certificate for section - %s", dataSection->Name.c_str());
    hashType = bi.GetAuthHashAlgo();
    hash = bi.hash;
    hashLength = hash->GetHashLength();
    std::string hashExtension = hash->GetHashFileExtension();

    /* Partition size must be 64-byte aligned */
    if ((dataSection->Length & 0x3F) != 0)
    {
        //LOG_ERROR("Authentication Error !!!\n           Partition %s's length %d bytes - is not 64-byte aligned - %s", dataSection->Name.c_str(), dataSection->Length);
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
        else if (bi.options.GetNonBootingFlag() && (dataSection->Name.find("MetaHeader") != std::string::npos))
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
    Section* acSection = new Section(name + hashExtension, certSize);
    acSection->isCertificate = true;
    acSection->index = dataSection->index;
    cache.Sections.push_back(acSection);
    uint8_t* authCert = acSection->Data;
    LOG_TRACE("Creating new section for certificate - %s", acSection->Name.c_str());

    uint32_t x = sizeof(AuthCertificate4096Sha3PaddingStructure);
    if (x != certSize)
    {
        LOG_DEBUG(DEBUG_STAMP, "Bad Authentication Certificate Size");
        LOG_ERROR("Authentication Error !!!");
    }
    memset(authCert, 0, certSize);
    
    uint32_t acHdr = authAlgorithm->GetAuthHeader();
    acHdr |= ((bi.GetAuthHashAlgo()) ? 0 : 1) << AC_HDR_SHA_2_3_BIT_SHIFT;
    
    WriteLittleEndian32(authCert+AC_HEADER_OFFSET, acHdr);
    WriteLittleEndian32(authCert+AC_SPK_ID_OFFSET, spkIdentification);

    if (udfFile != "")
    {
        LoadUdfData(udfFile, udf_data);
        authAlgorithm->RearrangeEndianess(udf_data, sizeof(udf_data));
        memcpy(authCert+AC_UDF_OFFSET, udf_data, UDF_DATA_SIZE);
    }

    primaryKey->Export(authCert+AC_PPK_KEY_OFFSET);
    authAlgorithm->RearrangeEndianess(authCert+AC_PPK_KEY_OFFSET + RSA_4096_N, RSA_4096_N_SIZE);
    authAlgorithm->RearrangeEndianess(authCert + AC_PPK_KEY_OFFSET + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
    authAlgorithm->RearrangeEndianess(authCert + AC_PPK_KEY_OFFSET + RSA_4096_E, RSA_4096_E_SIZE);
    FillSha3Padding(authCert+AC_PPK_SHA3PAD_OFFSET, AC_PPK_SHA3PAD_SIZE);

    secondaryKey->Export(authCert+AC_SPK_KEY_OFFSET);
    authAlgorithm->RearrangeEndianess(authCert + AC_SPK_KEY_OFFSET + RSA_4096_N, RSA_4096_N_SIZE);
    authAlgorithm->RearrangeEndianess(authCert + AC_SPK_KEY_OFFSET + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
    authAlgorithm->RearrangeEndianess(authCert + AC_SPK_KEY_OFFSET + RSA_4096_E, RSA_4096_E_SIZE);
    FillSha3Padding(authCert+AC_SPK_SHA3PAD_OFFSET, AC_SPK_SHA3PAD_SIZE);
    memset(authCert+AC_ALLIGNMENT_OFFSET, 0, AC_ALLIGNMENT_SIZE);

    CopySPKSignature(authCert+AC_SPK_SIGN_OFFSET);
    certIndex++;
    return acSection;
}

/******************************************************************************/
void VersalAuthenticationContext::Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert)
{
    /* Copy bhSignature when bootloader */
    memset(cert->section->Data + AC_BH_SIGN_OFFSET, 0, signatureLength); /*check*/
    if (sections.front()->isBootloader)
    {
        CopybHSignature(bi, cert->section->Data + AC_BH_SIGN_OFFSET);
    }

    /*  Copy meta header Signature when headers */
    if (sections.front()->Name == "Headers")
    {
        CopyIHTSignature(bi, cert->section->Data + AC_BH_SIGN_OFFSET);
    }

    if (presignFile == "")
    {
        CopyPartitionSignature(bi, sections, cert->section->Data + AC_PARTITION_SIGN_OFFSET, cert->section);
    }
    else
    {
        int index = acIndex;
        if (cert->section->index != 0)
        {
            index = cert->section->index;
        }
        GetPresign(presignFile, cert->section->Data + AC_PARTITION_SIGN_OFFSET, index);
        acIndex++;
    }
}

/******************************************************************************/
void VersalAuthenticationContext::CopybHSignature(BootImage& bi, uint8_t* ptr)
{
    uint8_t* sha_hash_padded = new uint8_t[signatureLength];
    uint8_t* bHsignaturetmp = new uint8_t[signatureLength];
    memset(bHsignaturetmp, 0, signatureLength);
    memset(sha_hash_padded, 0, signatureLength);

    GenerateBHHash(bi, sha_hash_padded);
    if (bi.options.DoGenerateHashes())
    {
        std::string hashfilename = "bootheader" + hash->GetHashFileExtension();
        WritePaddedSHAFile(sha_hash_padded, hashfilename);
    }

    if (primaryKey->Loaded && primaryKey->isSecret)
    {
        LOG_TRACE("Creating Boot Header Signature");
        authAlgorithm->RearrangeEndianess(sha_hash_padded, signatureLength);
        authAlgorithm->CreateSignature(sha_hash_padded, (uint8_t*)secondaryKey, bHsignaturetmp);
        authAlgorithm->RearrangeEndianess(bHsignaturetmp, signatureLength);

        if (bhSignLoaded)
        {
            if (memcmp(bHsignature, bHsignaturetmp, signatureLength) != 0)
            {
                LOG_ERROR("Authentication Error !!!\n           Loaded BH Signature does not match calculated BH Signature");
            }
        }
        memcpy(ptr, bHsignaturetmp, signatureLength);
    }
    else if (bhSignLoaded)
    {
        memcpy(ptr, bHsignature, signatureLength);
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
        LOG_ERROR("Authentication Error !!!\n          Either Secret Key Pair or BH signature file must be specified in BIF file");
    }

    // Clean up
    if (sha_hash_padded)
    {
        delete[] sha_hash_padded;
    }

    if (bHsignaturetmp)
    {
        delete[] bHsignaturetmp;
    }
    LOG_TRACE("Boot Header Signature copied into Authentication Certificate");
}

/******************************************************************************/
void VersalAuthenticationContext::CopyIHTSignature(BootImage & bi, uint8_t * ptr)
{
    uint8_t* sha_hash_padded = new uint8_t[signatureLength];
    uint8_t* signaturetmp = new uint8_t[signatureLength];
    memset(signaturetmp, 0, signatureLength);
    memset(sha_hash_padded, 0, signatureLength);

    GenerateIHTHash(bi, sha_hash_padded);
    if (bi.options.DoGenerateHashes())
    {
        std::string hashfilename = "imageheadertable" + hash->GetHashFileExtension();
        WritePaddedSHAFile(sha_hash_padded, hashfilename);
    }

    if (primaryKey->Loaded && primaryKey->isSecret)
    {
        LOG_TRACE("Creating Image Header Table Signature");
        if (authAlgorithm->Type() != Authentication::ECDSA)
        {
            authAlgorithm->RearrangeEndianess(sha_hash_padded, signatureLength);
        }
        authAlgorithm->CreateSignature(sha_hash_padded, (uint8_t*)secondaryKey, signaturetmp);
        if (authAlgorithm->Type() != Authentication::ECDSA)
        {
            authAlgorithm->RearrangeEndianess(signaturetmp, signatureLength);
        }
        /* if (bhSignLoaded)
        {
        if (memcmp(bHsignature, bHsignaturetmp, rsaKeyLength) != 0)
        {
        LOG_ERROR("Authentication Error !!!\n           Loaded BH Signature does not match calculated BH Signature");
        }
        } */
        memcpy(ptr, signaturetmp, signatureLength);
    }
    else if (bi.bifOptions->GetHeaderSignatureFile() != "")
    {
        GetPresign(bi.bifOptions->GetHeaderSignatureFile(), signaturetmp, 0);
        memcpy(ptr, signaturetmp, signatureLength);
    }
    else if (bi.options.DoGenerateHashes() && (bi.bifOptions->GetHeaderSignatureFile() == ""))
    {
        static bool warningGiven = false;
        if (!warningGiven)
        {
            LOG_WARNING("Either SSK or presign) is needed to authenticate a boot image or generate partition hashes. Because they are not provided, the bootimage and partition hashes will not be usable. However, the boot header hash file generated can be used for offline signing");
            warningGiven = true;
        }
    }
    else
    {
        LOG_ERROR("Authentication Error !!!\n          Either Secret Key Pair or BH signature file must be specified in BIF file");
    }

    // Clean up
    if (sha_hash_padded)
    {
        delete[] sha_hash_padded;
    }
    if (signaturetmp)
    {
        delete[] signaturetmp;
    }
    LOG_TRACE("Image Header Table Signature copied into Authentication Certificate");
}

/******************************************************************************/
void RSA4096Sha3PaddingAuthenticationAlgorithm::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0)
{
    AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
    RSA_Exponentiation(base, ((VersalKey*) primaryKey)->N, ((VersalKey*) primaryKey)->N_ext, ((VersalKey*) primaryKey)->D, result0);
}

/******************************************************************************/
void ECDSAAuthenticationAlgorithm::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0)
{
    ECDSASignature(base, ((VersalKey*)primaryKey)->eckey, result0);
}

/******************************************************************************/
void ECDSAP521AuthenticationAlgorithm::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0)
{
    ECDSASignature(base, ((VersalKey*)primaryKey)->eckey, result0);
}

/******************************************************************************/
void VersalAuthenticationContext::GenerateIHTHash(BootImage& bi, uint8_t* sha_hash_padded)
{
    LOG_TRACE("Calculating Image Header Table Hash");
    uint8_t* tmpIht = bi.imageHeaderTable->section->Data;
    uint8_t* sha_hash = new uint8_t[hashLength];
    hash->CalculateVersalHash(true,tmpIht, bi.imageHeaderTable->section->Length, sha_hash);
    authAlgorithm->CreatePadding(sha_hash_padded, sha_hash, hashLength);
    delete[] sha_hash;
}

/******************************************************************************/
void VersalAuthenticationContext::GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded)
{
    LOG_TRACE("Calculating the Boot Header Hash");
    /* Donot include SMAP data to calculate BH hash */
    uint8_t* tmpBh = bi.bootHeader->section->Data + 0x10;
    uint8_t* sha_hash = new uint8_t[hashLength];
    hash->CalculateVersalHash(false, tmpBh, bi.bootHeader->section->Length, sha_hash);
    authAlgorithm->CreatePadding(sha_hash_padded, sha_hash, hashLength);
    delete[] sha_hash;
}

/******************************************************************************/
void VersalAuthenticationContext::GenerateSPKHash(uint8_t* sha_hash_padded)
{
    LOG_TRACE("Calculating the SPK Hash");
    uint8_t* spkFull = new uint8_t[VERSAL_ACKEY_STRUCT_SIZE];
    uint8_t spkSHA3Padding[4] = { 0,0,0,0 };
    FillSha3Padding(spkSHA3Padding, sizeof(spkSHA3Padding));

    if (!secondaryKey->Loaded)
    {
        ParseSPKeyFile(spkFile);
    }
    secondaryKey->Export(spkFull);
    hashLength = hash->GetHashLength();
    uint8_t* shaHash = new uint8_t[hashLength];

    uint32_t acHdr = authAlgorithm->GetAuthHeader();
    acHdr |= ((hashType == AuthHash::Sha2) ? 0 : 1) << AC_HDR_SHA_2_3_BIT_SHIFT;

    uint8_t* tempBuffer = new uint8_t[VERSAL_ACKEY_STRUCT_SIZE + sizeof(acHdr) + sizeof(spkIdentification) + sizeof(spkSHA3Padding)];

    WriteLittleEndian32(tempBuffer, acHdr);
    WriteLittleEndian32(tempBuffer + sizeof(acHdr), spkIdentification);

    authAlgorithm->RearrangeEndianess(spkFull+ RSA_4096_N, RSA_4096_N_SIZE);
    authAlgorithm->RearrangeEndianess(spkFull+ RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
    authAlgorithm->RearrangeEndianess(spkFull+RSA_4096_E, RSA_4096_E_SIZE);
    memcpy(tempBuffer + sizeof(acHdr) + sizeof(spkIdentification), (uint8_t*)spkFull, VERSAL_ACKEY_STRUCT_SIZE);

    memcpy(tempBuffer + sizeof(acHdr) + sizeof(spkIdentification) + VERSAL_ACKEY_STRUCT_SIZE, spkSHA3Padding, sizeof(spkSHA3Padding));
    hash->CalculateVersalHash(false,(uint8_t*)tempBuffer, VERSAL_ACKEY_STRUCT_SIZE + sizeof(acHdr) + sizeof(spkIdentification) + sizeof(spkSHA3Padding), shaHash);
    // Create PKCS padding
    authAlgorithm->CreatePadding(sha_hash_padded, shaHash, hashLength);
    delete[] shaHash;
    delete[] tempBuffer;
}

/******************************************************************************/
void VersalAuthenticationContext::CopySPKSignature(uint8_t* ptr)
{
    CreateSPKSignature();
    LOG_TRACE("Copying the SPK signature into the Authentication Certificate");
    memcpy(ptr, spksignature, signatureLength);
}

/******************************************************************************/
std::string VersalAuthenticationContext::GetCertificateName(std::string name)
{
    if (certIndex != 0)
    {
        size_t x = name.find(".0");
        if (std::string::npos != name.find(".0"))
        {
            name[x + 1] = (char)(name[x + 1] + certIndex); // nudge the '0' to '1' ... '9'.
        }
    }
    return name;
}

/******************************************************************************/
void VersalAuthenticationContext::GeneratePPKHash(const std::string& filename)
{
    int ppkSize = VERSAL_ACKEY_STRUCT_SIZE;
    uint8_t* ppkTemp = new uint8_t[ppkSize];
    hashLength = hash->GetHashLength();
    primaryKey->Export(ppkTemp);
    authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_N, RSA_4096_N_SIZE);
    authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
    authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_E, RSA_4096_E_SIZE);

    uint8_t ppkSHA3Padding[12] = { 0 };
    FillSha3Padding(ppkSHA3Padding, sizeof(ppkSHA3Padding));

    uint8_t* tempBuffer = new uint8_t[ppkSize + sizeof(ppkSHA3Padding)];
    memcpy(tempBuffer, (uint8_t*)ppkTemp, ppkSize);
    memcpy(tempBuffer + ppkSize, ppkSHA3Padding, sizeof(ppkSHA3Padding));

    uint8_t* ppkHash = new uint8_t[hashLength];

    hash->CalculateVersalHash(false,tempBuffer, ppkSize + sizeof(ppkSHA3Padding), ppkHash);

    FILE* filePtr;
    if ((filePtr = fopen(filename.c_str(), "w")) == NULL)
    {
        LOG_ERROR("-efuseppkbits error !!!           Failure writing to hash file %s", StringUtils::BaseName(filename).c_str());
    }

    /* For Versal, the efuses are available only for 256 bits. 
       So the upper 256 bits(0x20 bytes) of the hash is dumped, which will be programmed to efuse.*/
    for (int index = 0; index < 0x20; index++)
    {
        fprintf(filePtr, "%02X", ppkHash[index]);
    }
    fprintf(filePtr, "\r\n");

    fclose(filePtr);
    LOG_INFO("Efuse PPK bits written to file %s successfully", filename.c_str());

    delete[] tempBuffer;
}

/******************************************************************************/
void RSA4096Sha3PaddingAuthenticationAlgorithm::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    /* RSA PSS Padding */
    char* rsaLocalENV = getenv("TEST_RSA_LOCAL");
    int hLen = hashLength;
    uint8_t padding1[PAD1_LENGTH];
    memset(padding1, 0, PAD1_LENGTH);
    uint8_t padding2[PAD2_LENGTH];
    memset(padding2, 0, PAD2_LENGTH);
    padding2[PAD2_LENGTH - 1] |= 0x01;
    //Set the leftmost 8emLen - emBits bits of the leftmost octet in maskedDB to zero
    int MSBits = ((SIGN_LENGTH_VERSAL * 8) - 1) & 0x7;
    int emLen = SIGN_LENGTH_VERSAL;

    if (MSBits == 0)
    {
        emLen--;
    }
    uint8_t *salt = NULL;
    salt = new uint8_t[SALT_LENGTH];

    if (rsaLocalENV != NULL) {
        uint8_t defsalt[SALT_LENGTH] = { 0x71, 0x5d, 0x74, 0xeb, 0x06, 0xcb, 0x2f, 0xfa, 0xed, 0x09, 0x3d, 0xd3, 0x9b, 0xa0, 0xb8, 0x57, 0xa3, 0x01, 0xcd, 0xd7, 0x52, 0x2a, 0x82, 0xc9, 0x71, 0x06, 0x8a, 0x6b, 0xb1, 0x99, 0x83, 0x52, 0x49, 0x49, 0x2b, 0xdb, 0xc8, 0x50, 0x20, 0x5e, 0x44, 0xb2, 0xc8, 0xbe, 0xbf, 0x77, 0x98, 0x22 };
        memcpy(salt, defsalt, SALT_LENGTH);
    }
    else {
        RAND_bytes(salt, SALT_LENGTH);
    }

    //calculate mHash - hash on message(m)
    uint8_t mHash[48] = { 0 };
    memcpy(mHash, hash, 48);

    //calculate mPad - add padding1 and salt to mHash
    uint8_t mPad[104];
    memset(mPad, 0, 104);
    memcpy(mPad, padding1, PAD1_LENGTH);
    memcpy(mPad + PAD1_LENGTH, mHash, 48);
    memcpy(mPad + PAD1_LENGTH + 48, salt, SALT_LENGTH);

    //calculate hash on mPad
    uint8_t *m1;
    uint8_t mPadHash[48];
    m1 = AttachSHA3Padding(mPad, 104);
    Versalcrypto_hash(mPadHash, m1, 208, false);
    int maskedDBLen = emLen - hLen - 1; //463
    uint8_t mask[463] = { 0 };
    if (MaskGenerationFunction(mask, maskedDBLen, mPadHash, 48, EVP_sha384()) == -1)
    {
        LOG_ERROR("Internal Error : Mask generation failed during authentication.");
    }
    //DB Padding
    uint8_t *DB;
    DB = new uint8_t[463];
    memset(DB, 0x0, 463);
    memcpy(DB, padding2, PAD2_LENGTH);
    memcpy(DB + PAD2_LENGTH, salt, 48);
    uint8_t EM[512] = { 0 };

    for (int i = 0; i < 463; i++)
    {
        EM[i] = (mask[i] ^ DB[i]);
    }

    if (MSBits == 0)
    {
        EM[0] = 0;
    }

    if (MSBits)
        EM[0] &= 0xFF >> (8 - MSBits);
    memcpy(EM + 463, mPadHash, 48);
    EM[512 - 1] = 0xbc;
    memcpy(signature, EM, 512);
    delete[] m1;
    delete[] DB;
    delete[] salt;
}

/******************************************************************************/
void ECDSAAuthenticationAlgorithm::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    memcpy(signature, hash, hashLength);
}

/******************************************************************************/
void ECDSAP521AuthenticationAlgorithm::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    memcpy(signature, hash, hashLength);
}

/******************************************************************************/
void VersalAuthenticationContext::CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection)
{
    LOG_TRACE("Copying the partition (%s) signature into Authentication Certificate", acSection->Name.c_str());
    /* calculate hash first */
    uint8_t* shaHash = new uint8_t[hashLength];
    std::list<Section*>::iterator section = sections.begin();
    size_t hashSecLen = (*section)->Length;

    //If !bootloader AND !headers
    if ((*section)->firstChunkSize != 0 && !(*section)->isBootloader && (*section)->isPartitionData)
    {
        hashSecLen = (*section)->firstChunkSize + hashLength;
    }
    /* Update with AC and then Partition */
    uint8_t *partitionAc = new uint8_t[hashSecLen + (acSection->Length - signatureLength)];

    /* Update with authentication certificate except the last 256 bytes - which is the partition signature that we are calculating now.
    Once calculated, the partition signature will sit there */
    memcpy(partitionAc, acSection->Data, acSection->Length - signatureLength);

    /* Update with Partition */
    memcpy(partitionAc + acSection->Length - signatureLength, (*section)->Data, hashSecLen);

    /* Calculate the final hash */
    Versalcrypto_hash(shaHash, partitionAc, hashSecLen + (acSection->Length - signatureLength), !(*section)->isBootloader);
    LOG_TRACE("Hash of %s (LE):", acSection->Name.c_str());
    LOG_DUMP_BYTES(shaHash, hashLength);
    /* Create the PKCS padding for the hash */
    uint8_t* shaHashPadded = new uint8_t[signatureLength];
    memset(shaHashPadded, 0, signatureLength);
    authAlgorithm->CreatePadding(shaHashPadded, shaHash, hashLength);

    /* Generate hashes, if requested from command line option "-generate_hashes" */
    if (bi.options.DoGenerateHashes())
    {
        std::string hashfilename = acSection->Name;
        WritePaddedSHAFile(shaHashPadded, hashfilename);
    }
    authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);
    /* Now sign the hash */
    authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)secondaryKey, signatureBlock);
    authAlgorithm->RearrangeEndianess(signatureBlock, signatureLength);
    LOG_TRACE("The partition signature is copied into Authentication Certificate");
    /* Delete the temporarily created arrays */
    delete[] shaHash;
    delete[] shaHashPadded;
    acIndex++;
}

/******************************************************************************/
void RSA4096Sha3PaddingAuthenticationAlgorithm::RearrangeEndianess(uint8_t *array, uint32_t size)
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

/******************************************************************************/
void ECDSAAuthenticationAlgorithm::RearrangeEndianess(uint8_t *array, uint32_t size)
{
    uint32_t lastIndex = size - 1;
    char tempInt = 0;

    // If array is NULL, return
    if (!array)
    {
        return;
    }

    if (size != UDF_DATA_SIZE && (size != EC_P384_KEY_LENGTH))
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
void ECDSAP521AuthenticationAlgorithm::RearrangeEndianess(uint8_t *array, uint32_t size)
{
    uint32_t lastIndex = size - 1;
    char tempInt = 0;

    // If array is NULL, return
    if (!array)
    {
        return;
    }

    if (size != UDF_DATA_SIZE && size != EC_P521_KEY_LENGTH1 && size != EC_P521_KEY_LENGTH2) 
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
void ECDSAAuthenticationAlgorithm::ECDSASignature(const uint8_t *base, EC_KEY *eckeyU, uint8_t *result0)
{
    EC_KEY* eckeyN = EC_KEY_new();
    EC_GROUP* ecgroup = EC_GROUP_new_by_curve_name(NID_secp384r1);
    EC_KEY_set_group(eckeyN, ecgroup);
    EC_KEY_set_asn1_flag(eckeyN, OPENSSL_EC_NAMED_CURVE);

    const BIGNUM* prv = BN_new();
    prv = EC_KEY_get0_private_key(eckeyU);
    const EC_POINT *pub = EC_KEY_get0_public_key(eckeyU);

    /* add the private & public key to the EC_KEY structure */
    EC_KEY_set_private_key(eckeyN, prv);
    EC_KEY_set_public_key(eckeyN, pub);

    /* create and verify signature */
    ECDSA_SIG* signature = ECDSA_do_sign(base, 48, eckeyN);
#if 0
    if (1 != ECDSA_do_verify(base, 48, signature, eckeyN)) {
        LOG_ERROR("Failed to verify EC Signature\n");
    }
    else {
        LOG_TRACE("Verified EC Signature\n");
    }
#endif

    if (signature != NULL)
    {
#if OPENSSL_VERSION_NUMBER > 0x10100000L
        const BIGNUM *sig_r = NULL, *sig_s = NULL;
        ECDSA_SIG_get0(signature, &sig_r, &sig_s);

        memcpy(result0, sig_r->d, EC_P384_KEY_LENGTH);
        RearrangeEndianess(result0, EC_P384_KEY_LENGTH);

        memcpy(result0 + EC_P384_KEY_LENGTH, sig_s->d, EC_P384_KEY_LENGTH);
        RearrangeEndianess(result0 + EC_P384_KEY_LENGTH, EC_P384_KEY_LENGTH);
#else

        memcpy(result0, signature->r->d, EC_P384_KEY_LENGTH);
        RearrangeEndianess(result0, EC_P384_KEY_LENGTH);

        memcpy(result0 + EC_P384_KEY_LENGTH, signature->s->d, EC_P384_KEY_LENGTH);
        RearrangeEndianess(result0 + EC_P384_KEY_LENGTH, EC_P384_KEY_LENGTH);
#endif
    }

    EC_GROUP_free(ecgroup);
    EC_KEY_free(eckeyN);
    ECDSA_SIG_free(signature);
}

/******************************************************************************/
void ECDSAP521AuthenticationAlgorithm::ECDSASignature(const uint8_t *base, EC_KEY *eckeyU, uint8_t *result0)
{
    EC_KEY* eckeyN = EC_KEY_new();
    EC_GROUP* ecgroup = EC_GROUP_new_by_curve_name(NID_secp521r1);
    EC_KEY_set_group(eckeyN, ecgroup);
    EC_KEY_set_asn1_flag(eckeyN, OPENSSL_EC_NAMED_CURVE);

    const BIGNUM* prv = BN_new();
    prv = EC_KEY_get0_private_key(eckeyU);
    const EC_POINT *pub = EC_KEY_get0_public_key(eckeyU);
    
    /* add the private & public key to the EC_KEY structure */
    EC_KEY_set_private_key(eckeyN, prv);
    EC_KEY_set_public_key(eckeyN, pub);

    /* create and verify signature */
    ECDSA_SIG* signature = ECDSA_do_sign(base, 48, eckeyN);
#if 0 
    if (1 != ECDSA_do_verify(base, 48, signature, eckeyN)) {
        LOG_ERROR("Failed to verify EC Signature\n");
    }
    else {
        LOG_TRACE("Verified EC Signature\n");
    }
#endif

    if (signature != NULL)
    {
        uint8_t *x1 = new uint8_t[EC_P521_KEY_LENGTH2];
        uint8_t *y1 = new uint8_t[EC_P521_KEY_LENGTH2];
        memset(x1, 0, EC_P521_KEY_LENGTH2);
        memset(y1, 0, EC_P521_KEY_LENGTH2);

        uint32_t signSzR;
        uint32_t signSzS;

#if OPENSSL_VERSION_NUMBER > 0x10100000L
        const BIGNUM *sig_r = NULL, *sig_s = NULL;
        ECDSA_SIG_get0(signature, &sig_r, &sig_s);

        signSzR = BN_num_bytes(sig_r);
        signSzS = BN_num_bytes(sig_s);

        if (signSzR == EC_P521_KEY_LENGTH1)
        {
            memcpy(x1 + 1, sig_r->d, signSzR);
            RearrangeEndianess(x1 + 1, signSzR);
        }
        else
        {
            memcpy(x1, sig_r->d, signSzR);
            RearrangeEndianess(x1, signSzR);
        }
        if (signSzS == EC_P521_KEY_LENGTH1)
        {
            memcpy(y1 + 1, sig_s->d, signSzS);
            RearrangeEndianess(y1 + 1, signSzS);
        }
        else
        {
            memcpy(y1, sig_s->d, signSzS);
            RearrangeEndianess(y1, signSzS);
        }

        memcpy(result0, x1, EC_P521_KEY_LENGTH2);
        memcpy(result0 + EC_P521_KEY_LENGTH2, y1, EC_P521_KEY_LENGTH2);
#else
        signSzR = BN_num_bytes(signature->r);
        signSzS = BN_num_bytes(signature->s);

        if (signSzR == EC_P521_KEY_LENGTH1)
        {
            memcpy(x1 + 1, signature->r->d, signSzR);
            RearrangeEndianess(x1 + 1, signSzR);
        }
        else
        {
            memcpy(x1, signature->r->d, signSzR);
            RearrangeEndianess(x1, signSzR);
        }
        if (signSzS == EC_P521_KEY_LENGTH1)
        {
            memcpy(y1 + 1, signature->s->d, signSzS);
            RearrangeEndianess(y1 + 1, signSzS);
        }
        else
        {
            memcpy(y1, signature->s->d, signSzS);
            RearrangeEndianess(y1, signSzS);
        }

        memcpy(result0, x1, EC_P521_KEY_LENGTH2);
        memcpy(result0 + EC_P521_KEY_LENGTH2, y1, EC_P521_KEY_LENGTH2);
#endif
        if (x1 != NULL)
        {
            delete[] x1;
        }
        if (y1 != NULL)
        {
            delete[] y1;
        }
    }

    EC_GROUP_free(ecgroup);
    EC_KEY_free(eckeyN);
    ECDSA_SIG_free(signature);
}

/******************************************************************************/
void VersalAuthenticationContext::AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header)
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
void VersalAuthenticationContext::GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index)
{
    std::string filename(presignFilename);
    std::string baseFile = StringUtils::BaseName(filename);

    if (index > 9)
    {
        LOG_DEBUG(DEBUG_STAMP, "Partition index count %d too high", index);
        LOG_ERROR("Presign file should have proper index (0-9)");
    }

    if (index != 0)
    {
        size_t x = filename.find(".0.");
        if (x == std::string::npos)
        {
            LOG_ERROR("Presign file %s does not have partition index (*.0.*)", baseFile.c_str());
        }
        filename[x + 1] = (char)(filename[x + 1] + index); // nudge the '0' to '1' ... '9'.
    }
    LOG_TRACE("Reading presign file - %s", filename.c_str());
    FILE* filePtr;
    filePtr = fopen(filename.c_str(), "rb");

    if (filePtr)
    {
        fseek(filePtr, 0, SEEK_END);   // non-portable
        long size = ftell(filePtr);
        fclose(filePtr);
        if (size == SIGN_LENGTH_VERSAL)
        {
            // read binary
            filePtr = fopen(filename.c_str(), "rb");
            long read_size = fread(signature, 1, SIGN_LENGTH_VERSAL, filePtr);
            if (read_size != SIGN_LENGTH_VERSAL)
            {
                LOG_ERROR("Authentication Error !!!\n           Presign file %s should be of %d bytes", baseFile.c_str(), SIGN_LENGTH_VERSAL);
            }
            fclose(filePtr);
        }
        else
        {
            // read ascii
            filePtr = fopen(filename.c_str(), "r");
            for (int i = 0; i<SIGN_LENGTH_VERSAL; i++)
            {
                int x;
                if (fscanf(filePtr, "%2X", &x) != 1)
                {
                    LOG_ERROR("Failure reading presign file - %s", baseFile.c_str());
                }
                signature[i] = x;
            }
            fclose(filePtr);
        }
    }
    else
    {
        LOG_ERROR("Failure opening presign file - %s", baseFile.c_str());
    }
}

/******************************************************************************/
void VersalAuthenticationContext::SetSPKSignatureFile(const std::string& filename)
{
    FILE* filePtr;
    filePtr = fopen(filename.c_str(), "r");

    if (filePtr)
    {
        fclose(filePtr);
        GetPresign(filename, spksignature, 0);
        spkSignLoaded = true;
        spkSignRequested = "";
    }
    else
    {
        LOG_ERROR("Failure reading SPK Signature file - %s", StringUtils::BaseName(filename).c_str());
    }
}

/******************************************************************************/
void VersalAuthenticationContext::SetBHSignatureFile(const std::string& filename)
{
    FILE* filePtr;
    filePtr = fopen(filename.c_str(), "r");

    if (filePtr)
    {
        fclose(filePtr);
        GetPresign(filename, bHsignature, 0);
        bhSignLoaded = true;
    }
    else
    {
        LOG_ERROR("Failure reading Boot Header Signature file - %s", StringUtils::BaseName(filename).c_str());
    }
}

/******************************************************************************/
void VersalAuthenticationContext::GenerateSPKSignature(const std::string& filename)
{
    if (primaryKey->Loaded && primaryKey->isSecret)
    {
        uint8_t* shaHashPadded = new uint8_t[signatureLength];
        uint8_t* spkSignatureTemp = new uint8_t[signatureLength];
        memset(shaHashPadded, 0, signatureLength);
        memset(spkSignatureTemp, 0, signatureLength);
        GenerateSPKHash(shaHashPadded);
        authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);
        authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)primaryKey, spkSignatureTemp);
        authAlgorithm->RearrangeEndianess(spkSignatureTemp, signatureLength);
        LOG_INFO("SPK Signature generated successfully");

        if (filename != "")
        {
            FILE* filePtr;
            filePtr = fopen(filename.c_str(), "w");
            if (filePtr)
            {
                for (uint32_t i = 0; i<signatureLength; i++)
                {
                    fprintf(filePtr, "%02X", spkSignatureTemp[i]);
                    if ((i % 32) == 31)
                    {
                        fprintf(filePtr, "\n");
                    }
                }
                fclose(filePtr);
                LOG_INFO("SPK Signature file - %s generated successfully", filename.c_str());
            }
            else
            {
                LOG_ERROR("-spksignature error !!!           Failure writing the SPK signature file - %s", StringUtils::BaseName(filename).c_str());
            }
        }
        delete[] spkSignatureTemp;
        delete[] shaHashPadded;
    }
    else
    {
        LOG_ERROR("-spksignature error !!!           PSK missing, cannot sign SPK for generating SPK Signature");
    }
}

/******************************************************************************/
void VersalAuthenticationContext::ResizeIfNecessary(Section* section)
{
    // round up to next 64 byte boundary.
    uint32_t dataSizePadded = (section->Length + 63) & 0xFFFFFFC0;
    section->IncreaseLengthAndPadTo(dataSizePadded, 0xFF);
}

/******************************************************************************/
void VersalAuthenticationContext::LoadUdfData(const std::string& udfFilename, uint8_t* signature)
{
    std::string filename(udfFilename);
    std::string baseFile = StringUtils::BaseName(udfFilename);
    FILE* filePtr;
    int datum, ret, nbytes;
    nbytes = 0;
    filePtr = fopen(filename.c_str(), "r");

    if (filePtr)
    {
        while ((ret = fscanf(filePtr, "%2X", &datum)) != EOF)
        {
            if (ret == 1)
            {
                if (datum < 0 || datum > 255)
                {
                    LOG_ERROR("Bad hex conversion %x - %s", datum, baseFile.c_str());
                }

                if (nbytes == UDF_DATA_SIZE)
                {
                    LOG_ERROR("More than %d bytes in the UDF Data file %s is not allowed.", UDF_DATA_SIZE, baseFile.c_str());
                }
                signature[nbytes++] = datum;
            }
            else
            {
                LOG_ERROR("Failure reading UDF file - %s", baseFile.c_str());
            }
        }
    }
    else
    {
        LOG_ERROR("Failure opening UDF file - %s", baseFile.c_str());
    }
    fclose(filePtr);
    LOG_TRACE("UDF data is loaded successfully in the Authentication Certificate from file - %s", udfFilename.c_str());
}

/******************************************************************************/
void VersalAuthenticationContext::CreateSPKSignature(void)
{
    LOG_TRACE("Creating the SPK signature");

    /* SPK is signed with PSK (Primary Secret Key)
    Check if PSK is given */
    if (primaryKey->Loaded && primaryKey->isSecret)
    {
        uint8_t* shaHashPadded = new uint8_t[signatureLength];
        memset(shaHashPadded, 0, signatureLength);
        memset(spksignature, 0, signatureLength);
        // Calulate the SPK hash with PKCS padding
        GenerateSPKHash(shaHashPadded);
        authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);
        // Sign the SPK hash
        authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)primaryKey, spksignature);
        authAlgorithm->RearrangeEndianess(spksignature, signatureLength);
        delete[] shaHashPadded;
    }
    /* If SPK signature file or PSK is not given in BIF file, cannot get SPK signature for the auth certificate
    Throw error */
    else if (!spkSignLoaded)
    {
        LOG_ERROR("Authentication Error !!!\n          PSK or SPK signature missing, must specify atleast one");
    }
    LOG_INFO("SPK signature created successfully");
}

/******************************************************************************/
uint32_t ECDSAAuthenticationAlgorithm::GetAuthHeader(void)
{
    return AUTH_HDR_VERSAL_ECDSA;
}

/******************************************************************************/
uint32_t ECDSAP521AuthenticationAlgorithm::GetAuthHeader(void)
{
    return AUTH_HDR_VERSAL_ECDSA_P521;
}

/******************************************************************************/
uint32_t RSA4096Sha3PaddingAuthenticationAlgorithm::GetAuthHeader(void)
{
    return AUTH_HDR_VERSAL;
}

/******************************************************************************/
void static SetDeviceDNA(const uint8_t* dnaValue, uint32_t* deviceDNA)
{
    for (uint32_t index = 0; index < WORDS_PER_DEVICE_DNA; index++)
    {
        deviceDNA[index] = ReadBigEndian32(dnaValue);
        dnaValue += sizeof(uint32_t);
    }
}

/******************************************************************************/
void VersalAuthenticationContext::CreateAuthJtagImage(uint8_t* buffer, AuthJtagInfo authJtagAttributes)
{
    LOG_TRACE("Creating the Authentication Header signature");
    AuthenticatedJtagImageStructure* authJtagImage = (AuthenticatedJtagImageStructure*)buffer;
    if (primaryKey->Loaded && primaryKey->isSecret)
    {
        hashLength = hash->GetHashLength();
        uint8_t* shaHash = new uint8_t[hashLength];
        uint8_t* shaHashPadded = new uint8_t[signatureLength];
        memset(shaHash, 0, hashLength);
        memset(shaHashPadded, 0, signatureLength);

        uint32_t acHdr = authAlgorithm->GetAuthHeader();
        acHdr |= ((hashType == AuthHash::Sha2) ? 0 : 1) << AC_HDR_SHA_2_3_BIT_SHIFT;

        WriteLittleEndian32(&authJtagImage->acHeader, acHdr);
        if (!authJtagAttributes.userRevokeId)
        {
            LOG_WARNING("revoke_id is not specified in BIF, default revoke id is assigned as '0'.");
        }
        WriteLittleEndian32(&authJtagImage->spkId, authJtagAttributes.revokeId);

        authJtagInfoStructurev2* v2 = (authJtagInfoStructurev2*)authJtagImage->authJtagInfo;
        if (authJtagAttributes.userDeviceDNA)
        {
            WriteLittleEndian32(&v2->attributes, vauthJtagMessagenMask << vauthJtagMessageShift);
        }
        SetDeviceDNA(authJtagAttributes.deviceDNA, (uint32_t*)v2->deviceDNA);
        WriteLittleEndian32(&v2->jtagTimeOut, authJtagAttributes.jtagTimeout);
        FillSha3Padding(v2->SHA3Padding, sizeof(v2->SHA3Padding));

        primaryKey->Export(authJtagImage->acPpk);
        authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_E, RSA_4096_E_SIZE);

        FillSha3Padding(authJtagImage->ppkSHA3Padding, sizeof(authJtagImage->ppkSHA3Padding));

        hash->CalculateVersalHash(false, (uint8_t*)authJtagImage, sizeof(authJtagImage->acHeader) + sizeof(authJtagImage->spkId) + sizeof(authJtagImage->authJtagInfo), shaHash);
        authAlgorithm->CreatePadding(shaHashPadded, shaHash, hashLength);
        authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);

        authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)primaryKey, authJtagImage->authJtagSignature);
        authAlgorithm->RearrangeEndianess(authJtagImage->authJtagSignature, signatureLength);

        delete[] shaHash;
        delete[] shaHashPadded;
    }
    else
    {
        LOG_ERROR("Authentication Error !!!\n          PSK must be specified to generate Authenticated Jtag Image");
    }
}

/******************************************************************************/
void VersalAuthenticationCertificate::Link(BootImage& bi, Section* dataSection)
{
    /* Gather up all the sections that will be used to calculate the authentication hash */
    std::list<Section*> sections;
    Section* headers = NULL;

    /* If the section is a header table section */
    if (isTableHeader)
    {
        size_t size = 0;
        if (bi.options.bifOptions->GetHeaderEncyption())
        {
            size = bi.encryptedHeaders->Length;
            headers = new Section("Headers", size);
            memset(headers->Data, bi.options.GetOutputFillByte(), headers->Length);
            memcpy(headers->Data, bi.encryptedHeaders->Data, bi.encryptedHeaders->Length);
            sections.push_back(headers);
        }
        else
        {
            if (bi.createSubSystemPdis == true)
            {
                for (std::list<SubSysImageHeader*>::iterator image = bi.subSysImageList.begin(); image != bi.subSysImageList.end(); image++)
                {
                    sections.push_back((*image)->section);
                    size += sizeof(VersalImageHeaderStructure);
                }
            }
            else
            {
                for (std::list<ImageHeader*>::iterator img = bi.imageList.begin(); img != bi.imageList.end(); img++)
                {
                    sections.push_back((*img)->section);
                    size += sizeof(VersalImageHeaderStructure);
                }
            }

            // Append all partition headers
            for (std::list<PartitionHeader*>::iterator part = bi.partitionHeaderList.begin(); part != bi.partitionHeaderList.end(); part++)
            {
                sections.push_back((*part)->section);
                size += sizeof(VersalPartitionHeaderTableStructure);
            }

            // Append NULL partition header
            //sections.push_back(bi.nullPartHeaderSection);

            /* Create one new combined section will all the appended sections above */
            headers = new Section("Headers", size);
            //headers->Address = iHT->section->Address; // not really needed, but useful for debug.
            memset(headers->Data, bi.options.GetOutputFillByte(), headers->Length);

            Binary::Address_t start = sections.front()->Address;
            for (SectionList::iterator i = sections.begin(); i != sections.end(); i++)
            {
                Section& section(**i);
                int offset = section.Address - start;
                memcpy(headers->Data + offset, section.Data, section.Length);
            }

            // replace sections list with the combined new section
            sections.clear();
            sections.push_back(headers);
        }
    }
    // If section is a partition section
    else
    {
        Section* bHSec = AttachBootHeaderToFsbl(bi);
        if (bHSec != NULL)
        {
            sections.push_back(bHSec);
        }
        else
        {
            sections.push_back(dataSection);
        }
    }

    // Link the certificate - pass for signing
    this->AuthContext->Link(bi, sections, this);
    if (headers != NULL)
    {
        delete headers;
    }
}

/******************************************************************************/
void VersalAuthenticationContext::SetKeyLength(Authentication::Type type)
{
    if (type == Authentication::RSA)
    {
        AuthenticationContext::authKeyLength = RSA_4096_KEY_LENGTH;
    }
    else if (type == Authentication::ECDSA)
    {
        AuthenticationContext::authKeyLength = EC_P384_KEY_LENGTH;
    }
    else if (type == Authentication::ECDSAp521)
    {
        //AuthenticationContext::authKeyLength = EC_P521_KEY_LENGTH;
    }
}