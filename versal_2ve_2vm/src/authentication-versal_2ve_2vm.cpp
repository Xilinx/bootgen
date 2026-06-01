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
#include "authentication-versal_2ve_2vm.h"
//#include "authentication.h"
#include "bootgenexception.h"
#include "stringutils.h"
//#include "binary.h"
#include "bootimage.h"
#include <iomanip>
#include <iostream>
#include "string.h"
#include "options.h"
#include <limits>
#include "partitionheadertable-versal_2ve_2vm.h"
#include "imageheadertable-versal_2ve_2vm.h"
#include "Keccak-compact-versal_2ve_2vm.h"
#include "encryptutils.h"
#include <openssl/rand.h>

#include "bootheader-versal_2ve_2vm.h"

extern "C" {
#include "lms-utils.h"
#include "hss_verify.h"
};
/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
Versal_2ve_2vmAuthenticationContext::Versal_2ve_2vmAuthenticationContext(Authentication::Type type)
{
    signatureLength = SIGN_LENGTH_VERSAL;
    bHsignature = std::make_unique<uint8_t[]>(signatureLength);
    spkSignLoaded = false;
    memset(udf_data, 0, UDF_DATA_SIZE);
    memset(bHsignature.get(), 0, signatureLength);
    bhSignLoaded = false;
    hashType = AuthHash::Sha3;
    authAlgorithm = GetAuthenticationAlgorithm(type);
    if (type == Authentication::RSA)
    {
        primaryKey = std::make_unique<Key4096Sha3Padding_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<Key4096Sha3Padding_versal_2ve_2vm>("Secondary Key");
        primaryKey->authType = Authentication::RSA;
        secondaryKey->authType = Authentication::RSA;

        certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure);
        signatureLength = SIGN_LENGTH_VERSAL;
    }
    else if (type == Authentication::ECDSA)
    {
        primaryKey = std::make_unique<KeyECDSA_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSA_versal_2ve_2vm>("Secondary Key");
        primaryKey->authType = Authentication::ECDSA;
        secondaryKey->authType = Authentication::ECDSA;

        certSize = sizeof(AuthCertificateECDSAHBStructure);
        signatureLength = EC_P384_KEY_LENGTH * 2;
    }
    else if (type == Authentication::ECDSAp521)
    {
        LOG_TRACE("ECDSAP521");
        primaryKey = std::make_unique<KeyECDSAp521_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSAp521_versal_2ve_2vm>("Secondary Key");
        primaryKey->authType = Authentication::ECDSAp521;
        secondaryKey->authType = Authentication::ECDSAp521;

        certSize = sizeof(AuthCertificateECDSAp521HBStructure);
        signatureLength = EC_P521_KEY_LENGTH2 * 2;
    }
    else if ((type == Authentication::LMS_SHA2_256) || (type == Authentication::LMS_SHAKE256))
    {
        primaryKey = std::make_unique<KeyLMS_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<KeyLMS_versal_2ve_2vm>("Secondary Key");
        primaryKey->lmsOnly = secondaryKey->lmsOnly = lmsOnly;
        if (type == Authentication::LMS_SHA2_256)
        {
            primaryKey->authType = Authentication::LMS_SHA2_256;
            secondaryKey->authType = Authentication::LMS_SHA2_256;
            hashType = AuthHash::Sha2;
            
        ownsHash = true;  // We own this hash
        }
        else
        {
            primaryKey->authType = Authentication::LMS_SHAKE256;
            secondaryKey->authType = Authentication::LMS_SHAKE256;
            hashType = AuthHash::Shake256;
            
        ownsHash = true;  // We own this hash
        }
        //signatureLength = GetLmsSignLength(pskFile.c_str(),lmsOnly);
        //certSize = GetCertificateSize();
    }
    spksignature = std::make_unique<uint8_t[]>(signatureLength);
}

/******************************************************************************/
Versal_2ve_2vmAuthenticationContext::Versal_2ve_2vmAuthenticationContext(const AuthenticationContext* refAuthContext, Authentication::Type authtype)
{
    // Don't allocate bHsignature here - will be allocated with correct size below
    // after signatureLength is determined for the specific authentication type
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
    lmsOnly = refAuthContext->lmsOnly;
    primaryLmsParams = refAuthContext->primaryLmsParams;
    primaryLmsParamsSize = refAuthContext->primaryLmsParamsSize;
    secondaryLmsParams = refAuthContext->secondaryLmsParams;
    secondaryLmsParamsSize = refAuthContext->secondaryLmsParamsSize;

    if (authtype == Authentication::RSA)
    {
        primaryKey = std::make_unique<Key4096Sha3Padding_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<Key4096Sha3Padding_versal_2ve_2vm>("Secondary Key");
        primaryKey->authType = Authentication :: RSA;
        secondaryKey->authType = Authentication :: RSA;
        
        // RSA doesn't create its own hash - will use bi.hash
        hash = nullptr;
        ownsHash = false;
        hashLength = 48;  // SHA3-384

        certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure);
        signatureLength = SIGN_LENGTH_VERSAL;
    }
    else if(authtype == Authentication::ECDSA)
    {
        primaryKey = std::make_unique<KeyECDSA_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSA_versal_2ve_2vm>("Secondary Key");
        primaryKey->authType = Authentication :: ECDSA;
        secondaryKey->authType = Authentication :: ECDSA;
        
        // ECDSA doesn't create its own hash - will use bi.hash
        hash = nullptr;
        ownsHash = false;
        hashLength = 48;  // SHA3-384

        certSize = sizeof(AuthCertificateECDSAHBStructure);
        signatureLength = EC_P384_KEY_LENGTH * 2;
    }
    else if (authtype == Authentication::ECDSAp521)
    {
        primaryKey = std::make_unique<KeyECDSAp521_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSAp521_versal_2ve_2vm>("Secondary Key");
        primaryKey->authType = Authentication :: ECDSAp521;
        secondaryKey->authType = Authentication :: ECDSAp521;
        
        // ECDSAp521 doesn't create its own hash - will use bi.hash
        hash = nullptr;
        ownsHash = false;
        hashLength = 48;  // SHA3-384

        certSize = sizeof(AuthCertificateECDSAp521HBStructure);
        signatureLength = EC_P521_KEY_LENGTH2 * 2;
    }
    else if ((authtype == Authentication::LMS_SHA2_256) || (authtype == Authentication::LMS_SHAKE256))
    {
        primaryKey = std::make_unique<KeyLMS_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<KeyLMS_versal_2ve_2vm>("Secondary Key");
        primaryKey->lmsOnly = secondaryKey->lmsOnly = lmsOnly;
        if (authtype == Authentication::LMS_SHA2_256)
        {
            primaryKey->authType = Authentication::LMS_SHA2_256;
            secondaryKey->authType = Authentication::LMS_SHA2_256;
            hashType = AuthHash::Sha2;
            hash = new HashSha2();
            ownsHash = true;  // We own this hash
        }
        else
        {
            primaryKey->authType = Authentication::LMS_SHAKE256;
            secondaryKey->authType = Authentication::LMS_SHAKE256;
            hashType = AuthHash::Shake256;
            hash = new HashShake256();
            ownsHash = true;  // We own this hash
        }
        signatureLength = GetLmsSignatureLength(primaryLmsParams, primaryLmsParamsSize, pskFile.c_str(), ppkFile.c_str(), lmsOnly);
        certSize = GetCertificateSize();
    }

    spksignature = std::make_unique<uint8_t[]>(signatureLength);
    bHsignature = std::make_unique<uint8_t[]>(signatureLength);
    
    // Initialize signature buffers to zero
    memset(spksignature.get(), 0, signatureLength);
    memset(bHsignature.get(), 0, signatureLength);
    
    // Initialize ownership flags (will be set false for shared keys below if needed)
    ownsPrimaryKey = true;
    ownsSecondaryKey = true;
    ownsAuthAlgorithm = true;
    // ownsHash set above for LMS

    if (pskFile != "" || ppkFile != "")
    {
        /*PM-TO-DO
           Reversed the parsing of psk and ppk parsing 
           Check if it is ok 
        */
        
        if (ppkFile != "")
        {
            primaryKey->ParsePublic(ppkFile);
        }
        if (pskFile != "")
        {
            primaryKey->ParseSecret(pskFile);
        }
    }
    else
    {
        // Non-owning reference to refAuthContext's primaryKey
        primaryKey.reset(refAuthContext->primaryKey.get());
        ownsPrimaryKey = false;  // Don't delete - shared with reference context
    }

    if (spkFile != "" || sskFile != "")
    {
        /*PM-TO-DO
        Reversed the parsing of ssk and spk parsing
        Check if it is ok
        */
        if (spkFile != "")
        {
            secondaryKey->ParsePublic(spkFile);
        }
        if (sskFile != "")
        {
            secondaryKey->ParseSecret(sskFile);
        }
    }
    else
    {
        // Non-owning reference to refAuthContext's secondaryKey
        secondaryKey.reset(refAuthContext->secondaryKey.get());
        ownsSecondaryKey = false;  // Don't delete - shared with reference context
    }
    if (spkSignFile != "")
    {
        SetSPKSignatureFile(spkSignFile);
    }
    else
    {
        // CRITICAL FIX: Use minimum length to prevent buffer overread
        // refAuthContext may have smaller signature (e.g., RSA=512) than this context (e.g., LMS=9620)
        size_t copyLen = (signatureLength < refAuthContext->signatureLength) ? signatureLength : refAuthContext->signatureLength;
        memcpy(spksignature.get(), refAuthContext->spksignature.get(), copyLen);
        // Zero remaining bytes if destination is larger
        if (signatureLength > copyLen) {
            memset(spksignature.get() + copyLen, 0, signatureLength - copyLen);
        }
    }

    if (bhSignFile != "")
    {
        SetBHSignatureFile(bhSignFile);
    }
    else
    {
        // CRITICAL FIX: Use minimum length to prevent buffer overread
        size_t copyLen = (signatureLength < refAuthContext->signatureLength) ? signatureLength : refAuthContext->signatureLength;
        memcpy(bHsignature.get(), refAuthContext->bHsignature.get(), copyLen);
        // Zero remaining bytes if destination is larger
        if (signatureLength > copyLen) {
            memset(bHsignature.get() + copyLen, 0, signatureLength - copyLen);
        }
    }
    memcpy(udf_data, refAuthContext->udf_data, sizeof(udf_data));
    bhSignLoaded = refAuthContext->bhSignLoaded;
    spkSignLoaded = refAuthContext->spkSignLoaded;
    spkSignRequested = refAuthContext->spkSignRequested;
    //certSize = sizeof(AuthCertificate4096Sha3PaddingStructure);
    preSigned = refAuthContext->preSigned;
}


/******************************************************************************/
Versal_2ve_2vmAuthenticationContext::Versal_2ve_2vmAuthenticationContext(const AuthCertificate4096Sha3PaddingHBStructure* existingCert, Authentication::Type authtype)
{
    signatureLength = SIGN_LENGTH_VERSAL;
    spkSignLoaded = true;
    bHsignature = std::make_unique<uint8_t[]>(signatureLength);
    bhSignLoaded = true;
    authAlgorithm = GetAuthenticationAlgorithm(authtype);

    if (authtype == Authentication::RSA)
    {
        primaryKey = std::make_unique<Key4096Sha3Padding_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<Key4096Sha3Padding_versal_2ve_2vm>("Secondary Key");
        primaryKey->authType = Authentication::RSA;
        secondaryKey->authType = Authentication::RSA;

        certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure);
        signatureLength = SIGN_LENGTH_VERSAL;
    }
    else if (authtype == Authentication::ECDSA)
    {
        primaryKey = std::make_unique<KeyECDSA_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSA_versal_2ve_2vm>("Secondary Key");
        primaryKey->authType = Authentication::ECDSA;
        secondaryKey->authType = Authentication::ECDSA;

        certSize = sizeof(AuthCertificateECDSAHBStructure);
        signatureLength = EC_P384_KEY_LENGTH * 2;
    }
    else if (authtype == Authentication::ECDSAp521)
    {
        primaryKey = std::make_unique<KeyECDSAp521_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSAp521_versal_2ve_2vm>("Secondary Key");
        primaryKey->authType = Authentication::ECDSAp521;
        secondaryKey->authType = Authentication::ECDSAp521;
        hashType = AuthHash::Sha3;
        
        ownsHash = true;  // We own this hash
        certSize = sizeof(AuthCertificateECDSAp521HBStructure);
        signatureLength = EC_P521_KEY_LENGTH2 * 2;
    }
    else if((authtype == Authentication::LMS_SHA2_256) || (authtype == Authentication::LMS_SHAKE256))
    {
        primaryKey = std::make_unique<KeyLMS_versal_2ve_2vm>("Primary Key");
        secondaryKey = std::make_unique<KeyLMS_versal_2ve_2vm>("Secondary Key");
        primaryKey->lmsOnly = secondaryKey->lmsOnly = lmsOnly;
        if (authtype == Authentication::LMS_SHA2_256)
        {
            primaryKey->authType = Authentication::LMS_SHA2_256;
            secondaryKey->authType = Authentication::LMS_SHA2_256;
            hashType = AuthHash::Sha2;
            
        ownsHash = true;  // We own this hash
        }
        else
        {
            primaryKey->authType = Authentication::LMS_SHAKE256;
            secondaryKey->authType = Authentication::LMS_SHAKE256;
            hashType = AuthHash::Shake256;
            
        ownsHash = true;  // We own this hash
        }
        signatureLength = GetLmsSignatureLength(primaryLmsParams, primaryLmsParamsSize, pskFile.c_str(), ppkFile.c_str(), lmsOnly);
        certSize = GetCertificateSize();
    }
    else {
    }

    spksignature = std::make_unique<uint8_t[]>(signatureLength);
    primaryKey->Import(&existingCert->acPpk, "Primary Key");
    secondaryKey->Import(&existingCert->acSpk, "Secondary Key");
    hashType = AuthHash::Sha3;

    authAlgorithm->RearrangeEndianess(primaryKey->N.get(), sizeof(existingCert->acPpk.N));
    authAlgorithm->RearrangeEndianess(primaryKey->N_ext.get(), sizeof(existingCert->acPpk.N_extension));
    authAlgorithm->RearrangeEndianess(primaryKey->E.get(), sizeof(existingCert->acPpk.E));

    authAlgorithm->RearrangeEndianess(secondaryKey->N.get(), sizeof(existingCert->acSpk.N));
    authAlgorithm->RearrangeEndianess(secondaryKey->N_ext.get(), sizeof(existingCert->acSpk.N_extension));
    authAlgorithm->RearrangeEndianess(secondaryKey->E.get(), sizeof(existingCert->acSpk.E));
    memcpy(spksignature.get(), existingCert->acSpkSignature.Signature, signatureLength);
    //memcpy(bHsignature, existingCert->acHeaderSignature.Signature, signatureLength);
    //memcpy(udf_data, existingCert->acUdf, UDF_DATA_SIZE);
    //spkIdentification = existingCert->spkId;
    //certSize = sizeof(AuthCertificate4096Sha3PaddingStructure);
}

/******************************************************************************/
std::unique_ptr<AuthenticationAlgorithm> Versal_2ve_2vmAuthenticationContext::GetAuthenticationAlgorithm(Authentication::Type type)
{
    if (type == Authentication::ECDSA)
    {
        SetAuthenticationKeyLength(EC_P384_KEY_LENGTH);
        return std::make_unique<ECDSAHBAuthenticationAlgorithm>();
    }
    else if (type == Authentication::ECDSAp521)
    {
        SetAuthenticationKeyLength(EC_P521_KEY_LENGTH2);
        return std::make_unique<ECDSAP521HBAuthenticationAlgorithm>();
    }
    else if(type == Authentication::RSA)
    {
        SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
        return std::make_unique<RSA4096Sha3PaddingHBAuthenticationAlgorithm>();
    }
    else if((type == Authentication::LMS_SHA2_256)|| (type == Authentication::LMS_SHAKE256))
    {
        if (type == Authentication::LMS_SHA2_256)
        {
            hashType = AuthHash::Sha2;
        }
        else
        {
            hashType = AuthHash::Shake256;
        }
        return std::make_unique<LMSAuthenticationAlgorithm>(type);
    }
    else
    {
        return nullptr;
    }
}

/******************************************************************************/
uint32_t Versal_2ve_2vmAuthenticationContext::GetCertificateSize(void)
{
    if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256) || (authAlgorithm->Type() == Authentication::LMS_SHAKE256))
    {
        /* 
        --- PPK
        HssPublicKey        acPpk;                      // (0x00)   : 60 bytes
        uint32_t            acPpkAlignment;             // (0x3c)   : 4 bytes 
        --- SPK Header
        uint32_t            acTotalSpkSize;             // (0x40)   : 4 bytes
        uint32_t            acActualSpkSize;            // (0x44)   : 4 bytes
        uint32_t            acSpkTotalSignatureSize;    // (0x48)   : 4 bytes
        uint32_t            acSpkActualSignatureSize;   // (0x4c)   : 4 bytes
        uint32_t            acSpkId;                    // (0x50)   : 4 bytes
        uint32_t            acSpkHdrAlignment[3];       // (0x54)   : 12 bytes
        --- SPK
        HssPublicKey        acSpk;                      // (0x60)   : 60 bytes
        uint32_t            acSpkAlignment;             // (0x9c)   : 4 bytes
        --- SPK Signature
        HssSignature        acSpkSignature;             // (0xa0)   : ?? bytes
        uint32_t            acSpkSignAlignment[3];      // (??)     : ?? bytes
        --- Hash Block Signature : This is placed after Hash Block
        //HssSignature        acHashBlockSignature;       // (??)     : ?? bytes
        //uint32_t            acHashBlockAlignment[3];    // (??)     : ?? bytes
        */

        size_t ppkLength = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly);
        ppkLength += PADDING_16B(ppkLength);
        
        size_t spkHdrLength = 5 * sizeof(uint32_t);
        spkHdrLength += PADDING_16B(spkHdrLength);
        
        size_t spkLength = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);
        spkLength += PADDING_16B(spkLength);
        
        size_t spkSignLength = GetLmsSignatureLength(primaryLmsParams, primaryLmsParamsSize, pskFile.c_str(), ppkFile.c_str(), lmsOnly);
        spkSignLength += PADDING_16B(spkSignLength);
        
        return (ppkLength + spkHdrLength + spkLength + spkSignLength);
    }
    else
    {
        return certSize;
    }
}

/******************************************************************************/
uint32_t Versal_2ve_2vmAuthenticationContext::GetAuthJtagImageSize(void) const
{
    if (authAlgorithm->Type() == Authentication::RSA)
    {
        return sizeof(AuthenticatedJtagRSAImageStructure);
    }
    else if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        return sizeof(AuthenticatedJtagECP384ImageStructure);
    }
    else if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        size_t actualppkSize = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly);
        size_t actualspkSize = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);
        size_t spkSignLength = GetLmsSignatureLength(primaryLmsParams, primaryLmsParamsSize, pskFile.c_str(), ppkFile.c_str(), lmsOnly);
        size_t authJtagSignLength = GetLmsSignatureLength(secondaryLmsParams, secondaryLmsParamsSize, sskFile.c_str(), spkFile.c_str(), lmsOnly);

        size_t totalSize = AUTH_JTAG_LMS_FIXED_HEADER_SIZE
            + actualppkSize + PADDING_16B(actualppkSize)
            + AUTH_JTAG_LMS_SPK_HEADER_LENGTH
            + actualspkSize + PADDING_16B(actualspkSize)
            + spkSignLength + PADDING_16B(spkSignLength)
            + authJtagSignLength + PADDING_16B(authJtagSignLength);

        return (uint32_t)totalSize;
    }
    return sizeof(AuthenticatedJtagRSAImageStructure);
}

/******************************************************************************/
uint32_t Versal_2ve_2vmAuthenticationContext::GetTotalHashBlockSignSize(void)
{
    if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256) || (authAlgorithm->Type() == Authentication::LMS_SHAKE256))
    {
        size_t totalHashBlockSignatureLength = GetLmsSignatureLength(secondaryLmsParams, secondaryLmsParamsSize, sskFile.c_str(), spkFile.c_str(), lmsOnly);
        totalHashBlockSignatureLength += PADDING_16B(totalHashBlockSignatureLength);
        return totalHashBlockSignatureLength;
    }
    else
    {
        return signatureLength;
    }
}

/******************************************************************************/
Versal_2ve_2vmAuthenticationContext::~Versal_2ve_2vmAuthenticationContext()
{
    // spksignature, bHsignature, primaryKey, secondaryKey, hash, authAlgorithm
    // are all automatically destroyed when the object goes out of scope
}

/******************************************************************************/
ECDSAHBAuthenticationAlgorithm::ECDSAHBAuthenticationAlgorithm()
{
    certSize = sizeof(AuthCertificateECDSAHBStructure);
    authType = Authentication::ECDSA;
}

/******************************************************************************/
ECDSAHBAuthenticationAlgorithm::~ECDSAHBAuthenticationAlgorithm()
{
}

/******************************************************************************/
ECDSAP521HBAuthenticationAlgorithm::ECDSAP521HBAuthenticationAlgorithm()
{
    certSize = sizeof(AuthCertificateECDSAp521HBStructure);
    authType = Authentication::ECDSAp521;
}

/******************************************************************************/
ECDSAP521HBAuthenticationAlgorithm::~ECDSAP521HBAuthenticationAlgorithm()
{
}

/******************************************************************************/
RSA4096Sha3PaddingHBAuthenticationAlgorithm::RSA4096Sha3PaddingHBAuthenticationAlgorithm()
{
    certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure);
    authType = Authentication::RSA;
}

/******************************************************************************/
RSA4096Sha3PaddingHBAuthenticationAlgorithm::~RSA4096Sha3PaddingHBAuthenticationAlgorithm()
{
}

/******************************************************************************/
/* Currently unused but kept for potential future use */
[[maybe_unused]] static void FillSha3Padding(uint8_t* pad, uint32_t sha3PadLength)
{
    auto sha3 = std::make_unique<uint8_t[]>(sha3PadLength);
    memset(sha3.get(), 0, sha3PadLength);
    sha3[0] = 0x6;
    sha3[(sha3PadLength)-1] |= 0x80;
    memcpy(pad, sha3.get(), sha3PadLength);
}

/******************************************************************************/
uint8_t* RSA4096Sha3PaddingHBAuthenticationAlgorithm::AttachSHA3Padding(uint8_t* data, const Binary::Length_t datalength)
{
    uint8_t sha3padding = SHA3_PAD_LENGTH - (datalength % SHA3_PAD_LENGTH);
    auto dataSha3 = std::make_unique<uint8_t[]>(datalength + sha3padding);
    memset(dataSha3.get(), 0, datalength + sha3padding);
    memcpy(dataSha3.get(), data, datalength);
    dataSha3[datalength] = 0x6;
    dataSha3[(datalength + sha3padding) - 1] |= 0x80;
    return dataSha3.release();
}

/******************************************************************************/
int RSA4096Sha3PaddingHBAuthenticationAlgorithm::MaskGenerationFunction(unsigned char *mask, long len, const unsigned char *seed, long seedlen, const EVP_MD *dgst)
{
    long i, outlen = 0;
    unsigned char cnt[4];
    unsigned char md[SHA3_LENGTH_BYTES];
    int mdlen = SHA3_LENGTH_BYTES;
    int rv = -1;
    auto c = std::make_unique<uint8_t[]>(52);
    std::unique_ptr<uint8_t[]> cSha3Pad;  // Smart pointer for auto cleanup
    if (mdlen < 0)
        goto err;
    for (i = 0; outlen < len; i++)
    {
        cnt[0] = (unsigned char)((i >> 24) & 255);
        cnt[1] = (unsigned char)((i >> 16) & 255);
        cnt[2] = (unsigned char)((i >> 8)) & 255;
        cnt[3] = (unsigned char)(i & 255);

        memcpy(c.get(), seed, seedlen);
        memcpy(c.get() + seedlen, cnt, 4);
        cSha3Pad.reset(AttachSHA3Padding(c.get(), (seedlen + 4)));  // Wrap C pointer in unique_ptr
        if (outlen + mdlen <= len)
        {
            Versal_2ve_2vmcrypto_hash(mask + outlen, cSha3Pad.get(), 104, false);
            outlen += mdlen;
        }
        else
        {
            Versal_2ve_2vmcrypto_hash(md, cSha3Pad.get(), 104, false);
            memcpy(mask + outlen, md, len - outlen);
            outlen = len;
        }
    }
    rv = 0;
    // Smart pointer auto-deletes
err:
    return rv;
}

/******************************************************************************/
Section* Versal_2ve_2vmAuthenticationContext::CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection, bool isBootloader)
{
    LOG_INFO("Creating Authentication Certificate for section - %s", dataSection->Name.c_str());

    /* PM-TODO - Check this and update */
    Hash* savedHash = nullptr;
    bool savedOwnsHash = false;
    if ((authAlgorithm->Type() != Authentication::LMS_SHA2_256) && (authAlgorithm->Type() != Authentication::LMS_SHAKE256))
    {
        hashType = bi.GetAuthHashAlgo();
        savedHash = hash;
        savedOwnsHash = ownsHash;
        hash = bi.hash.get();
        ownsHash = false;  // Non-owning pointer to bi.hash
    }
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
    // Calculate final certSize BEFORE creating Section
    uint32_t x = sizeof(AuthCertificate4096Sha3PaddingHBStructure);
    if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        x = certSize = sizeof(AuthCertificateECDSAHBStructure);
    }
    else if (authAlgorithm->Type() == Authentication::ECDSAp521)
    {
        x = certSize = sizeof(AuthCertificateECDSAp521HBStructure);
    }
    else if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256) || (authAlgorithm->Type() == Authentication::LMS_SHAKE256))
    {
        x = certSize = GetCertificateSize();
    }

    if (x != certSize)
    {
        LOG_DEBUG(DEBUG_STAMP, "Bad Authentication Certificate Size");
        LOG_ERROR("Authentication Error !!!");
    }

    // Now create Section with correct certSize
    auto acSection_ptr = std::make_unique<Section>(name + hashExtension, certSize);
    Section* acSection = acSection_ptr.get();
    acSection->isCertificate = true;
    acSection->index = dataSection->index;
    cache.Sections.push_back(std::move(acSection_ptr));
    uint8_t* authCert = acSection->Data.get();
    LOG_TRACE("Creating new section for certificate - %s", acSection->Name.c_str());

    memset(authCert, 0, certSize);
    
    uint32_t acHdr = authAlgorithm->GetAuthHeader();
    
    uint8_t* headerData = bi.bootHeader->section->Data.get();
    uint32_t authHeader1Offset = BH_AC_HEADER_OFFSET_V2;
    uint32_t totalppkkSize1Offset = BH_TOTAL_PPK_SIZE1_OFFSET_V2;
    uint32_t actualppkSize1Offset = BH_ACTUAL_PPK_SIZE1_OFFSET_V2;
    uint32_t totalHashBlockSignatureSize1Offset = BH_TOTAL_SIGN_SIZE1_OFFSET_V2;
    uint32_t actualSignatureSize1Offset = BH_ACTUAL_SIGN_SIZE1_OFFSET_V2;
    if (name == "MetaHeader")
    {
        headerData = bi.imageHeaderTable->section->Data.get();
        authHeader1Offset = IHT_AC_HEADER_OFFSET;
        totalppkkSize1Offset = IHT_TOTAL_PPK_SIZE1_OFFSET;
        actualppkSize1Offset = IHT_ACTUAL_PPK_SIZE1_OFFSET;
        totalHashBlockSignatureSize1Offset = IHT_TOTAL_SIGN_SIZE1_OFFSET;
        actualSignatureSize1Offset = IHT_ACTUAL_SIGN_SIZE1_OFFSET;
    }
    
    if (udfFile != "")
    {
        LoadUdfData(udfFile, udf_data);
        authAlgorithm->RearrangeEndianess(udf_data, sizeof(udf_data));
        memcpy(headerData + BH_UDF_OFFSET_TELLURIDE, udf_data, UDF_BH_TELLURIDE);
    }

    if(isBootloader || name == "MetaHeader")
    {
        WriteLittleEndian32(headerData + authHeader1Offset, acHdr);
    }
    if (authAlgorithm->Type() == Authentication::RSA)
    {
        if(isBootloader || name == "MetaHeader")
        {
            WriteLittleEndian32(headerData + totalppkkSize1Offset, RSA_4096_N_SIZE + RSA_4096_N_EXT_SIZE + RSA_4096_E_SIZE + TELLURIDE_RSA_AC_PPK_SPK_ALIGNMENT);
            WriteLittleEndian32(headerData + actualppkSize1Offset, RSA_4096_N_SIZE + RSA_4096_N_EXT_SIZE + RSA_4096_E_SIZE);
            WriteLittleEndian32(headerData + totalHashBlockSignatureSize1Offset, signatureLength);
            WriteLittleEndian32(headerData + actualSignatureSize1Offset, signatureLength);
        }

        primaryKey->Export(authCert + TELLURIDE_RSA_AC_PPK_OFFSET);
        authAlgorithm->RearrangeEndianess(authCert + TELLURIDE_RSA_AC_PPK_OFFSET + RSA_4096_N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(authCert + TELLURIDE_RSA_AC_PPK_OFFSET + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(authCert + TELLURIDE_RSA_AC_PPK_OFFSET + RSA_4096_E, RSA_4096_E_SIZE);

        WriteLittleEndian32(authCert + TELLURIDE_RSA_AC_SPK_TOTAL_SIZE_OFFSET, VERSAL_ACKEY_STRUCT_SIZE + TELLURIDE_RSA_AC_PPK_SPK_ALIGNMENT);
        WriteLittleEndian32(authCert + TELLURIDE_RSA_AC_SPK_ACTUAL_SIZE_OFFSET, VERSAL_ACKEY_STRUCT_SIZE);

        WriteLittleEndian32(authCert + TELLURIDE_RSA_AC_SPK_TOTAL_SIGN_SIZE_OFFSET, signatureLength);
        WriteLittleEndian32(authCert + TELLURIDE_RSA_AC_SPK_ACTUAL_SIGN_SIZE_OFFSET, signatureLength);

        WriteLittleEndian32(authCert + TELLURIDE_RSA_AC_SPK_ID_OFFSET, spkIdentification);

        secondaryKey->Export(authCert + TELLURIDE_RSA_AC_SPK_OFFSET);
        authAlgorithm->RearrangeEndianess(authCert + TELLURIDE_RSA_AC_SPK_OFFSET + RSA_4096_N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(authCert + TELLURIDE_RSA_AC_SPK_OFFSET + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(authCert + TELLURIDE_RSA_AC_SPK_OFFSET + RSA_4096_E, RSA_4096_E_SIZE);

        CopySPKSignature(bi, authCert + TELLURIDE_RSA_AC_SPK_SIGN_OFFSET);
    }
    else if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        if(isBootloader || name == "MetaHeader")
        {
            WriteLittleEndian32(headerData + totalppkkSize1Offset, EC_P384_KEY_LENGTH * 2);
            WriteLittleEndian32(headerData + actualppkSize1Offset, EC_P384_KEY_LENGTH * 2);

            WriteLittleEndian32(headerData + totalHashBlockSignatureSize1Offset, EC_P384_KEY_LENGTH * 2);
            WriteLittleEndian32(headerData + actualSignatureSize1Offset, EC_P384_KEY_LENGTH * 2);
        }

        primaryKey->Export(authCert + TELLURIDE_EC_P384_AC_PPK_OFFSET);

        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_TOTAL_SPK_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_ACTUAL_SPK_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_TOTAL_SPK_SIGN_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_ACTUAL_SPK_SIGN_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_SPK_ID_OFFSET, spkIdentification);

        secondaryKey->Export(authCert + TELLURIDE_EC_P384_AC_SPK_OFFSET);

        CopySPKSignature(bi, authCert + TELLURIDE_EC_P384_AC_SPK_SIGN_OFFSET);
    }
    else if (authAlgorithm->Type() == Authentication::ECDSAp521)
    {
        if(isBootloader || name == "MetaHeader")
        {
            // Implementation of Authentication certificate for ECDSA P-521
            WriteLittleEndian32(headerData + totalppkkSize1Offset, EC_P521_KEY_LENGTH2 * 2 + PADDING_16B(EC_P521_KEY_LENGTH2 * 2));
            WriteLittleEndian32(headerData + actualppkSize1Offset, EC_P521_KEY_LENGTH2 * 2);
            WriteLittleEndian32(headerData + totalHashBlockSignatureSize1Offset, EC_P521_KEY_LENGTH2 * 2 + PADDING_16B(EC_P521_KEY_LENGTH2 * 2));
            WriteLittleEndian32(headerData + actualSignatureSize1Offset, EC_P521_KEY_LENGTH2 * 2);
        }

        primaryKey->Export(authCert + TELLURIDE_EC_P521_AC_PPK_OFFSET);

        WriteLittleEndian32(authCert + TELLURIDE_EC_P521_AC_TOTAL_SPK_SIZE_OFFSET, EC_P521_KEY_LENGTH2 * 2 + PADDING_16B(EC_P521_KEY_LENGTH2 * 2));
        WriteLittleEndian32(authCert + TELLURIDE_EC_P521_AC_ACTUAL_SPK_SIZE_OFFSET, EC_P521_KEY_LENGTH2 * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P521_AC_TOTAL_SPK_SIGN_SIZE_OFFSET, EC_P521_KEY_LENGTH2 * 2 + PADDING_16B(EC_P521_KEY_LENGTH2 * 2));
        WriteLittleEndian32(authCert + TELLURIDE_EC_P521_AC_ACTUAL_SPK_SIGN_SIZE_OFFSET, EC_P521_KEY_LENGTH2 * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P521_AC_SPK_ID_OFFSET, spkIdentification);

        secondaryKey->Export(authCert + TELLURIDE_EC_P521_AC_SPK_OFFSET);

        CopySPKSignature(bi, authCert + TELLURIDE_EC_P521_AC_SPK_SIGN_OFFSET);
    
    }
    else 
    {
        // LMS_SHA256 || LMS_SHAKE256
        acHdr = authAlgorithm->GetAuthHeader(lmsOnly);

        size_t actualSpkSignatureLength = GetLmsSignatureLength(primaryLmsParams, primaryLmsParamsSize, pskFile.c_str(), ppkFile.c_str(), lmsOnly);
        size_t actualHashBlockSignatureLength = GetLmsSignatureLength(secondaryLmsParams, secondaryLmsParamsSize, sskFile.c_str(), spkFile.c_str(), lmsOnly);
        size_t actualLmsPpkSize1 = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly);
        size_t actualLmsSpkSize1 = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);

        // CRITICAL FIX: Only write to BootHeader/IHT for bootloader or MetaHeader
        // Non-bootloader partitions should NOT overwrite these fields!
        if(isBootloader || name == "MetaHeader")
        {
            WriteLittleEndian32(headerData + authHeader1Offset, acHdr);
            WriteLittleEndian32(headerData + totalppkkSize1Offset, actualLmsPpkSize1 + PADDING_16B(actualLmsPpkSize1));
            WriteLittleEndian32(headerData + actualppkSize1Offset, actualLmsPpkSize1);
            WriteLittleEndian32(headerData + totalHashBlockSignatureSize1Offset, actualHashBlockSignatureLength + PADDING_16B(actualHashBlockSignatureLength));
            WriteLittleEndian32(headerData + actualSignatureSize1Offset, actualHashBlockSignatureLength);
        }

        primaryKey->Export(authCert + TELLURIDE_LMS_AC_PPK_OFFSET);
        WriteLittleEndian32(authCert + TELLURIDE_LMS_AC_TOTAL_SPK_SIZE_OFFSET(lmsOnly), actualLmsSpkSize1 + PADDING_16B(actualLmsSpkSize1));
        WriteLittleEndian32(authCert + TELLURIDE_LMS_AC_ACTUAL_SPK_SIZE_OFFSET(lmsOnly), actualLmsSpkSize1);
        WriteLittleEndian32(authCert + TELLURIDE_LMS_AC_TOTAL_SPK_SIGN_SIZE_OFFSET(lmsOnly), actualSpkSignatureLength + PADDING_16B(actualSpkSignatureLength));
        WriteLittleEndian32(authCert + TELLURIDE_LMS_AC_ACTUAL_SPK_SIGN_SIZE_OFFSET(lmsOnly), actualSpkSignatureLength);

        WriteLittleEndian32(authCert + TELLURIDE_LMS_AC_SPK_ID_OFFSET(lmsOnly), spkIdentification);

        secondaryKey->Export(authCert + TELLURIDE_LMS_AC_SPK_OFFSET(lmsOnly));
        CopySPKSignature(bi, authCert + TELLURIDE_LMS_AC_SPK_SIGN_OFFSET(lmsOnly));
    }

    // Restore original hash if we temporarily used bi.hash
    if (savedHash)
    {
        hash = savedHash;
        ownsHash = savedOwnsHash;
    }

    certIndex++;
    return acSection;
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::Link(BootImage& bi, void* partition, AuthenticationCertificate* cert)
{
    // Copy bhSignature when bootloader 
    if (cert->fsbl)
    {
        //CopyBhHash
        LOG_TRACE("Calculating the Boot Header Hash");
        // Donot include SMAP data to calculate BH hash 
        uint8_t* tmpBh = bi.bootHeader->section->Data.get() + 0x10;
        
        // Calculate the BH hash with SHA3 rather than LMS hashing algo 
        auto sha_hash = std::make_unique<uint8_t[]>(bi.hash->GetHashLength());
        bi.hash->CalculateVersalHash(true, tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(Versal_2ve_2vmSmapWidthTable), sha_hash.get());
        memcpy(bi.partitionHeaderList.front()->partition->section->Data.get() + HASH_BLOCK_INDEX_BYTES, sha_hash.get(), bi.hash->GetHashLength());
    }

    std::list<Section*> sections;
    if (presignFile == "")
    {
        if (cert->fsbl)
        {
            sections.push_back(((Versal_2ve_2vmPartition*)partition)->header->partition->section);
            CopyPartitionSignature(bi, sections, bi.hashBlockLength, ((Versal_2ve_2vmPartition*)partition)->header->partition->section->Data.get() + ((Versal_2ve_2vmPartition*)partition)->hashBlockLength, cert->section);
        }
        else if(cert->isTableHeader)
        {
            sections.push_back(bi.imageHeaderTable->hashBlockSection);
            CopyPartitionSignature(bi, sections, bi.imageHeaderTable->hashBlockSectionLength, bi.imageHeaderTable->hashBlockSection->Data.get() + bi.imageHeaderTable->hashBlockSectionLength, cert->section);
        }
        else
        {
            // IMAGE_STORE: Use partition's hashBlockSection and hashBlockSectionLength
            sections.push_back(((Versal_2ve_2vmPartition*)partition)->hashBlockSection);
            CopyPartitionSignature(bi, sections, ((Versal_2ve_2vmPartition*)partition)->hashBlockSectionLength, 
                                  ((Versal_2ve_2vmPartition*)partition)->hashBlockSection->Data.get() + ((Versal_2ve_2vmPartition*)partition)->hashBlockSectionLength, 
                                  cert->section);
        }
    }
    else
    {
        int index = acIndex;
        if (cert->section->index != 0)
        {
            index = cert->section->index;
        }

        if(authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
        {
            signatureLength = GetLmsSignatureLength(secondaryLmsParams, secondaryLmsParamsSize, sskFile.c_str(), spkFile.c_str(), lmsOnly);
        }
        
        if (cert->fsbl)
        {
            GetPresign(presignFile, signatureLength, ((Versal_2ve_2vmPartition*)partition)->header->partition->section->Data.get() + ((Versal_2ve_2vmPartition*)partition)->hashBlockLength, index);
        }
        else if(cert->isTableHeader)
        {
            GetPresign(presignFile, signatureLength, bi.imageHeaderTable->hashBlockSection->Data.get() + bi.imageHeaderTable->hashBlockSectionLength, index);
        }
        else{
            //For all other partitions
            GetPresign(presignFile, signatureLength,((Versal_2ve_2vmPartition*)partition)->hashBlockSection->Data.get() + ((Versal_2ve_2vmPartition*)partition)->hashBlockSectionLength, index);
        }

        acIndex++;
    }
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert)
{
    /* Copy bhSignature when bootloader */
    //memset(cert->section->Data.get() + AC_BH_SIGN_OFFSET, 0, signatureLength); /*check*/
    if (sections.front()->isBootloader)
    {
        //CopybHSignature(bi, cert->section->Data.get() + AC_BH_SIGN_OFFSET);

        //CopyBhHash
        LOG_TRACE("Calculating the Boot Header Hash");
        /* Donot include SMAP data to calculate BH hash */
        uint8_t* tmpBh = bi.bootHeader->section->Data.get() + 0x10;
        
        /* Calculate the BH hash with SHA3 rather than LMS hashing algo */
        auto sha_hash = std::make_unique<uint8_t[]>(bi.hash->GetHashLength());
        bi.hash->CalculateVersalHash(true, tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(Versal_2ve_2vmSmapWidthTable), sha_hash.get());
        
        /*if (bi.options.DoGenerateHashes())
        {
            std::string hashfilename = "bootheader" + hash->GetHashFileExtension();
            WritePaddedSHAFile(sha_hash_padded, hashfilename);
        }*/

        memcpy(bi.partitionHeaderList.front()->partition->section->Data.get() + HASH_BLOCK_INDEX_BYTES, sha_hash.get(), bi.hash->GetHashLength());
    }

    /*  Copy meta header Signature when headers */
    if (sections.front()->Name == "Headers")
    {
        //CopyIHTSignature(bi, cert->section->Data.get() + AC_BH_SIGN_OFFSET);
    }

    if (presignFile == "")
    {
        if (sections.front()->Name == "Headers" && bi.imageHeaderTable->hashBlockSection)
            CopyPartitionSignature(bi, sections, bi.imageHeaderTable->hashBlockSectionLength, bi.imageHeaderTable->hashBlockSection->Data.get() + bi.imageHeaderTable->hashBlockSectionLength, cert->section);
        else
            CopyPartitionSignature(bi, sections, bi.hashBlockLength, sections.front()->Data.get() + bi.hashBlockLength, cert->section);

        //CopyPartitionSignature(bi, sections, cert->section->Data.get() + AC_PARTITION_SIGN_OFFSET, cert->section);
    }
    else
    {
        int index = acIndex;
        if (cert->section->index != 0)
        {
            index = cert->section->index;
        }
        //GetPresign(presignFile, cert->section->Data.get() + AC_PARTITION_SIGN_OFFSET, index);
        if (authAlgorithm->Type() == Authentication::RSA)
            GetPresign(presignFile, sections.front()->Data.get() + bi.hashBlockLength, index);
        else if (authAlgorithm->Type() == Authentication::ECDSA)
            GetPresign(presignFile, sections.front()->Data.get() + bi.hashBlockLength, index);
        else if (authAlgorithm->Type() == Authentication::ECDSAp521)
            GetPresign(presignFile, sections.front()->Data.get() + bi.hashBlockLength, index);
        else
        {
            //LMS
        }
        acIndex++;
    }
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::CopybHSignature(BootImage& bi, uint8_t* ptr)
{
    auto sha_hash_padded = std::make_unique<uint8_t[]>(signatureLength);
    auto bHsignaturetmp = std::make_unique<uint8_t[]>(signatureLength);
    memset(bHsignaturetmp.get(), 0, signatureLength);
    memset(sha_hash_padded.get(), 0, signatureLength);

    GenerateBHHash(bi, sha_hash_padded.get());
    if (bi.options.DoGenerateHashes())
    {
        std::string hashfilename = "bootheader" + hash->GetHashFileExtension();
        WritePaddedSHAFile(sha_hash_padded.get(), hashfilename);
    }

    if (primaryKey->Loaded && primaryKey->isSecret)
    {
        LOG_TRACE("Creating Boot Header Signature");
        authAlgorithm->RearrangeEndianess(sha_hash_padded.get(), signatureLength);
        if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
        {
            authAlgorithm->CreateSignature(sha_hash_padded.get(), hashLength, sskFile.c_str(), 
                bHsignaturetmp.get(), signatureLength, lmsOnly, spkFile.c_str());
        }
        else
        {
            authAlgorithm->CreateSignature(sha_hash_padded.get(), (uint8_t*)secondaryKey.get(), bHsignaturetmp.get());
        }
        authAlgorithm->RearrangeEndianess(bHsignaturetmp.get(), signatureLength);

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
        LOG_ERROR("Authentication Error !!!\n          Either Secret Key Pair or BH signature file must be specified in BIF file");
    }

    LOG_TRACE("Boot Header Signature copied into Authentication Certificate");
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::CopyIHTSignature(BootImage & bi, uint8_t * ptr)
{
    auto sha_hash_padded = std::make_unique<uint8_t[]>(signatureLength);
    auto signaturetmp = std::make_unique<uint8_t[]>(signatureLength);
    memset(signaturetmp.get(), 0, signatureLength);
    memset(sha_hash_padded.get(), 0, signatureLength);

    GenerateIHTHash(bi, sha_hash_padded.get());
    if (bi.options.DoGenerateHashes())
    {
        std::string hashfilename = "imageheadertable" + hash->GetHashFileExtension();
        WritePaddedSHAFile(sha_hash_padded.get(), hashfilename);
    }

    if (primaryKey->Loaded && primaryKey->isSecret)
    {
        LOG_TRACE("Creating Image Header Table Signature");
        if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
        { 
            authAlgorithm->CreateSignature(sha_hash_padded.get(), hashLength, sskFile.c_str(), 
                signaturetmp.get(), signatureLength, lmsOnly, spkFile.c_str());
        }
        else
        {
            if (authAlgorithm->Type() != Authentication::ECDSA)
            {
                authAlgorithm->RearrangeEndianess(sha_hash_padded.get(), signatureLength);
            }
            authAlgorithm->CreateSignature(sha_hash_padded.get(), (uint8_t*)secondaryKey.get(), signaturetmp.get());
            if (authAlgorithm->Type() != Authentication::ECDSA)
            {
                authAlgorithm->RearrangeEndianess(signaturetmp.get(), signatureLength);
            }
        }
        
        /* if (bhSignLoaded)
        {
        if (memcmp(bHsignature.get(), bHsignaturetmp, rsaKeyLength) != 0)
        {
        LOG_ERROR("Authentication Error !!!\n           Loaded BH Signature does not match calculated BH Signature");
        }
        } */
        memcpy(ptr, signaturetmp.get(), signatureLength);
    }
    else if (bi.bifOptions->GetHeaderSignatureFile() != "")
    {
        GetPresign(bi.bifOptions->GetHeaderSignatureFile(), signaturetmp.get(), 0);
        memcpy(ptr, signaturetmp.get(), signatureLength);
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

    LOG_TRACE("Image Header Table Signature copied into Authentication Certificate");
}

/******************************************************************************/
void RSA4096Sha3PaddingHBAuthenticationAlgorithm::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0)
{
    AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
    RSA_Exponentiation(base, ((VersalKey*) primaryKey)->N.get(), ((VersalKey*) primaryKey)->N_ext.get(), ((VersalKey*) primaryKey)->D.get(), result0);
}

/******************************************************************************/
void ECDSAHBAuthenticationAlgorithm::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0)
{
    ECDSASignature(base, ((VersalKey*)primaryKey)->eckey, result0);
}

/******************************************************************************/
void ECDSAP521HBAuthenticationAlgorithm::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0)
{
    ECDSASignature(base, ((VersalKey*)primaryKey)->eckey, result0);
}

/******************************************************************************/
void LMSAuthenticationAlgorithm::VerifySignature(const uint8_t* buffer, size_t buf_len, const char* keyFile, uint8_t* result0, size_t result0_len, bool lmsOnly)
{
    //LOG_TRACE("Verifying LMS signature");
    if (!lmsOnly)
    {
        if (LmsVerify(keyFile, buffer, buf_len, result0, result0_len))
            LOG_TRACE("Verified LMS signature");
        else
            LOG_ERROR("Error verifying LMS signature");
    }
}

/******************************************************************************/
void LMSAuthenticationAlgorithm::CreateSignature(const uint8_t* buffer, size_t buf_len, const char* keyFile, 
    uint8_t* result0, size_t result0_len, bool lmsOnly, const char* publicKeyFile)
{
    size_t sig_len = 0;
    uint8_t* hss_sign = nullptr;  // Initialize to avoid uninitialized warning
    if (lmsOnly)
    {
        auto hss_sign_temp = std::make_unique<uint8_t[]>(result0_len + 4);
        memset(hss_sign_temp.get(), 0, result0_len + 4);
        hss_sign = hss_sign_temp.release();  // Transfer ownership to raw pointer
    }
    else
    {
        hss_sign = std::make_unique<uint8_t[]>(result0_len).release();
        memset(hss_sign, 0, result0_len);
    }
    LOG_WARNING("Be careful when version controlling, backing up, and restoring LMS private keys. The private key file contains state. Re-using private key state leads to loss of authenticity. For more information, see NIST SP800-208 Sec. 9.1 https://csrc.nist.gov/pubs/sp/800/208/final.");
    uint32_t err = 0;
    if (!LmsSign(keyFile, buffer, buf_len, hss_sign, &sig_len, &err))
    {
        if(err == 3)
        {
            LOG_ERROR("Error generating LMS signature!\n\t\t This private key has generated all the signatures it is allowed.");   
        }
        else if(err == 21)
        {
            LOG_ERROR("Error generating LMS signature!\n\t\t The write of the private key failed.");   
        }
        LOG_DEBUG(DEBUG_STAMP, "Error code : %d", err);

        LOG_ERROR("Error generating LMS signature");
    }    
    if (lmsOnly)
    {
        memcpy(result0, hss_sign + 4, result0_len);
    }
    else
    {
        memcpy(result0, hss_sign, result0_len);
    }
    //LOG_TRACE("result0");
    //LOG_DUMP_BYTES(result0, result0_len);

    //LmsVerify(publicKeyFile, buffer, buf_len, result0, result0_len);

	LOG_TRACE("LMS signature length with key %s - %ld, -%ld", keyFile, result0_len, sig_len);
#if 0
    static uint8_t indx = 1;
    std::string filename = "datafile_";

    FILE* filePtr;
    size_t result;
    std::string fName = "datafile_" + std::to_string(indx);
    filePtr = fopen(fName.c_str(), "wb");
    if (filePtr != NULL)
    {
        result = fwrite(buffer, 1, buf_len, filePtr);
        if (result != buf_len)
        {
            LOG_ERROR("Error dumping datafile");
        }
        fclose(filePtr);
    }

    FILE* filePtr1;
    std::string fName1 = "datafile_" + std::to_string(indx) + ".sig";
    filePtr1 = fopen(fName1.c_str(), "wb");
    if (filePtr1 != NULL)
    {
        result = fwrite(result0, 1, sig_len, filePtr1);
        if (result != sig_len)
        {
            LOG_ERROR("Error dumping datafile.sig");
        }
        fclose(filePtr1);
    }
    indx++;
#endif
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::GenerateIHTHash(BootImage& bi, uint8_t* sha_hash_padded)
{
    LOG_TRACE("Calculating Image Header Table Hash");
    uint8_t* tmpIht = bi.imageHeaderTable->section->Data.get();
    auto sha_hash = std::make_unique<uint8_t[]>(hashLength);
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        hash->CalculateHash(true, tmpIht, bi.imageHeaderTable->section->Length, sha_hash.get());
    }
    else
    {
        hash->CalculateVersalHash(true,tmpIht, bi.imageHeaderTable->section->Length, sha_hash.get());
    }
    authAlgorithm->CreatePadding(sha_hash_padded, sha_hash.get(), hashLength);
    // unique_ptr handles deletion
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded)
{
    LOG_TRACE("Calculating the Boot Header Hash");
    /* Donot include SMAP data to calculate BH hash */
    uint8_t* tmpBh = bi.bootHeader->section->Data.get() + 0x10;
    auto sha_hash2 = std::make_unique<uint8_t[]>(hashLength);
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        hash->CalculateHash(true, tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(Versal_2ve_2vmSmapWidthTable), sha_hash2.get());
    }
    else
    {
        hash->CalculateVersalHash(true, tmpBh, bi.bootHeader->section->Length - sizeof(Versal_2ve_2vmSmapWidthTable), sha_hash2.get());
    }
    
    authAlgorithm->CreatePadding(sha_hash_padded, sha_hash2.get(), hashLength);
    memcpy(bi.partitionHeaderList.front()->section->Data.get() + HASH_BLOCK_INDEX_BYTES, sha_hash2.get(), hashLength);
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::GenerateSPKHash(uint8_t* sha_hash_padded)
{
    // uint8_t* tempBuffer = NULL; // Unused variable
    uint16_t totalSignatureLength = 0;
    LOG_TRACE("Calculating the SPK Hash");
    size_t totalKeySize = 0;
    size_t actualKeySize = 0;
    
    if (authAlgorithm->Type() == Authentication::RSA)
    {
        totalKeySize = VERSAL_ACKEY_STRUCT_SIZE + TELLURIDE_RSA_AC_PPK_SPK_ALIGNMENT;
        actualKeySize = VERSAL_ACKEY_STRUCT_SIZE;
    }
    else if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        totalKeySize = actualKeySize = 2 * EC_P384_KEY_LENGTH;
    }
    else if (authAlgorithm->Type() == Authentication::ECDSAp521)
    {
        actualKeySize = 2 * EC_P521_KEY_LENGTH2;
        totalKeySize = actualKeySize + PADDING_16B(actualKeySize);
        signatureLength = 2 * EC_P521_KEY_LENGTH2;
        totalSignatureLength = signatureLength + PADDING_16B(signatureLength);
    }
    else if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        actualKeySize = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);
        totalKeySize = actualKeySize + PADDING_16B(actualKeySize);
        signatureLength = GetLmsSignatureLength(primaryLmsParams, primaryLmsParamsSize, pskFile.c_str(), ppkFile.c_str(), lmsOnly);
        totalSignatureLength = signatureLength + PADDING_16B(signatureLength);
    }

    auto spkFull = std::make_unique<uint8_t[]>(actualKeySize);

    //uint8_t spkSHA3Padding[4] = { 0,0,0,0 };
    //FillSha3Padding(spkSHA3Padding, sizeof(spkSHA3Padding));

    if (!secondaryKey->Loaded)
    {
        ParseSPKeyFile(spkFile);
    }
    secondaryKey->Export(spkFull.get());
    hashLength = hash->GetHashLength();
    auto shaHash = std::make_unique<uint8_t[]>(hashLength);
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        // acTotalSpkSize; acActualSpkSize; acSpkTotalSignatureSize; acSpkActualSignatureSize; acSpkId; acSpkHdrAlignment[3]; , acSpk
        //acSpkSize, acSpkSignatureSize, acSpkId, acSpkHdrAlignment, acSpk
        auto tempBuffer = std::make_unique<uint8_t[]>(totalKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
        memset(tempBuffer.get(), 0, totalKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
        
        WriteLittleEndian32(tempBuffer.get(), totalKeySize);
        WriteLittleEndian32(tempBuffer.get() + sizeof(uint32_t), actualKeySize);
        WriteLittleEndian32(tempBuffer.get() + (2 * sizeof(uint32_t)), totalSignatureLength);
        WriteLittleEndian32(tempBuffer.get() + (3 * sizeof(uint32_t)), signatureLength);
        WriteLittleEndian32(tempBuffer.get() + (4 * sizeof(uint32_t)), spkIdentification);
        memcpy(tempBuffer.get() + TELLURIDE_AC_SPK_HDR_LENGTH, spkFull.get(), actualKeySize);
#ifdef DEBUG
        LOG_TRACE("DATA being Hashed for SPK Sign");
        LOG_DUMP_BYTES(tempBuffer.get(), actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
#endif
        hash->CalculateHash(true, tempBuffer.get(), actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, shaHash.get());
    }
    else if (authAlgorithm->Type() == Authentication::ECDSAp521)
    {
        // SPK hash generation for ECDSA-p521
        auto tempBuffer = std::make_unique<uint8_t[]>(actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
        memset(tempBuffer.get(), 0, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);

        WriteLittleEndian32(tempBuffer.get(), totalKeySize);
        WriteLittleEndian32(tempBuffer.get() + sizeof(uint32_t), actualKeySize);
        WriteLittleEndian32(tempBuffer.get() + (2 * sizeof(uint32_t)), totalSignatureLength);
        WriteLittleEndian32(tempBuffer.get() + (3 * sizeof(uint32_t)), signatureLength);
        WriteLittleEndian32(tempBuffer.get() + (4 * sizeof(uint32_t)), spkIdentification);

        authAlgorithm->RearrangeEndianess(spkFull.get() + RSA_4096_N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(spkFull.get() + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(spkFull.get() + RSA_4096_E, RSA_4096_E_SIZE);
        memcpy(tempBuffer.get() + TELLURIDE_AC_SPK_HDR_LENGTH, spkFull.get(), actualKeySize);
#ifdef DEBUG
        LOG_TRACE("DATA being Hashed for SPK Sign");
        LOG_DUMP_BYTES(tempBuffer.get(), actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
#endif
        hash->CalculateVersalHash(true, tempBuffer.get(), actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, shaHash.get());

    }
    else
    {
        // acTotalSpkSize; acActualSpkSize; acSpkTotalSignatureSize; acSpkActualSignatureSize; acSpkId; acSpkHdrAlignment[3]; , acSpk
        //acSpkSize, acSpkSignatureSize, acSpkId, acSpkHdrAlignment, acSpk
        auto tempBuffer = std::make_unique<uint8_t[]>(actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
        memset(tempBuffer.get(), 0, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);

        WriteLittleEndian32(tempBuffer.get(), totalKeySize);
        WriteLittleEndian32(tempBuffer.get() + sizeof(uint32_t), actualKeySize);
        WriteLittleEndian32(tempBuffer.get() + (2 * sizeof(uint32_t)), signatureLength);
        WriteLittleEndian32(tempBuffer.get() + (3 * sizeof(uint32_t)), signatureLength);
        WriteLittleEndian32(tempBuffer.get() + (4 * sizeof(uint32_t)), spkIdentification);

        authAlgorithm->RearrangeEndianess(spkFull.get() + RSA_4096_N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(spkFull.get() + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(spkFull.get() + RSA_4096_E, RSA_4096_E_SIZE);
        memcpy(tempBuffer.get() + TELLURIDE_AC_SPK_HDR_LENGTH, spkFull.get(), actualKeySize);
#ifdef DEBUG
        LOG_TRACE("DATA being Hashed for SPK Sign");
        LOG_DUMP_BYTES(tempBuffer.get(), actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
#endif
        hash->CalculateVersalHash(true, tempBuffer.get(), actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, shaHash.get());
    }
#ifdef DEBUG
    LOG_TRACE("Hash");
    LOG_DUMP_BYTES(shaHash, hashLength);
    LOG_TRACE("Hash with PKCS Padding");
#endif
    // Create PKCS padding
    authAlgorithm->CreatePadding(sha_hash_padded, shaHash.get(), hashLength);
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        LOG_DUMP_BYTES(sha_hash_padded, hashLength);
    }
    else
    {
        LOG_DUMP_BYTES(sha_hash_padded, signatureLength);
    }

}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::CopySPKSignature(BootImage& bi, uint8_t* ptr)
{
    if (spkSignLoaded)
    {
        uint32_t index = 0;
        if(authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
        {
            signatureLength = GetLmsSignatureLength(primaryLmsParams, primaryLmsParamsSize, pskFile.c_str(), ppkFile.c_str(), lmsOnly);
        }
        GetPresign(spkSignFile, signatureLength, spksignature.get(), index);
    }
    else
    {
        if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
            && bi.spkFileAndSpkSignature.find(std::make_tuple(spkFile, ppkFile, spkIdentification)) != bi.spkFileAndSpkSignature.end())
        {
                 auto& cached = bi.spkFileAndSpkSignature.find(std::make_tuple(spkFile, ppkFile, spkIdentification))->second;
                 memcpy(spksignature.get(), cached.data(), signatureLength);
        }
        else if ((authAlgorithm->Type() == Authentication::RSA || authAlgorithm->Type() == Authentication::ECDSA)
            && bi.spkFileAndSpkSignature.find(std::make_tuple(sskFile, pskFile, spkIdentification)) != bi.spkFileAndSpkSignature.end())
        {
            auto& cached = bi.spkFileAndSpkSignature.find(std::make_tuple(sskFile, pskFile, spkIdentification))->second;
            memcpy(spksignature.get(), cached.data(), signatureLength);
        }
        else
        {
            CreateSPKSignature(bi);
        }
    }
    LOG_TRACE("Copying the SPK signature into the Authentication Certificate");
    memcpy(ptr, spksignature.get(), signatureLength);
}

/******************************************************************************/
std::string Versal_2ve_2vmAuthenticationContext::GetCertificateName(std::string name)
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
void Versal_2ve_2vmAuthenticationContext::GeneratePPKHash(const std::string& filename)
{
    int ppkSize = VERSAL_ACKEY_STRUCT_SIZE;

	if (authAlgorithm->Type() == Authentication::ECDSA)
        ppkSize = 2 * EC_P384_KEY_LENGTH;
    if (authAlgorithm->Type() == Authentication::ECDSAp521)
        ppkSize = 2 * EC_P521_KEY_LENGTH2;
    if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256) || (authAlgorithm->Type() == Authentication::LMS_SHAKE256))
    {
        
        ownsHash = true;  // We own this hash
        ppkSize = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly);
    }
    hashLength = hash->GetHashLength();
    auto ppkTemp = std::make_unique<uint8_t[]>(ppkSize);
    memset(ppkTemp.get(), 0, ppkSize);
    primaryKey->Export(ppkTemp.get());
    if ((authAlgorithm->Type() != Authentication::LMS_SHA2_256) && (authAlgorithm->Type() != Authentication::LMS_SHAKE256))
    {
        authAlgorithm->RearrangeEndianess(ppkTemp.get() + RSA_4096_N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(ppkTemp.get() + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(ppkTemp.get() + RSA_4096_E, RSA_4096_E_SIZE);
    }

    auto ppkHash = std::make_unique<uint8_t[]>(hashLength);

    hash->CalculateVersalHash(true, ppkTemp.get(), ppkSize, ppkHash.get());
    LOG_TRACE("FULL PPK HASH");
    LOG_DUMP_BYTES(ppkHash.get(), hashLength);

    FILE* filePtr;
    if ((filePtr = fopen(filename.c_str(), "w")) == NULL)
    {
        LOG_ERROR("-efuseppkbits error !!!           Failure writing to hash file %s", StringUtils::BaseName(filename).c_str());
    }

    /* No Truncation on PPK Hash for Telluride, after ADR-278 */
    for (int index = 0; index < hashLength; index++)
    {
        fprintf(filePtr, "%02X", ppkHash.get()[index]);
    }
    fprintf(filePtr, "\r\n");

    fclose(filePtr);
    LOG_INFO("Efuse PPK bits written to file %s successfully", filename.c_str());

    
}

/******************************************************************************/
void RSA4096Sha3PaddingHBAuthenticationAlgorithm::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    /* RSA PSS Padding */
    char* rsaLocalENV = getenv("TEST_RSA_LOCAL");
    char* envSetMSBMaskedDb = getenv("BOOTGEN_SET_MSB_MASKED_DB");
    char* envSetUserPadding2 = getenv("BOOTGEN_SET_USER_PADDING2");
    char* envSetUserPadding1 = getenv("BOOTGEN_SET_USER_PADDING1");
    char* envSetEM = getenv("BOOTGEN_SET_EM");

    int hLen = hashLength;
    uint8_t padding1[PAD1_LENGTH];
    memset(padding1, 0, PAD1_LENGTH);
    if (envSetUserPadding1 != NULL)
    {
        static bool envWarnSetUserPadding1 = false;
        FileImport fileReader;
        if (!fileReader.LoadHexData(envSetUserPadding1, padding1, PAD1_LENGTH))
        {
            LOG_ERROR("Invalid no. of data bytes for Padding1.\n           Expected length for Padding1 is 8 bytes");
        }
        if (!envWarnSetUserPadding1)
        {
            envWarnSetUserPadding1 = true;
            LOG_MSG("[WARNING]: RSA-PSS padding1 is set from '%s'", envSetUserPadding1);
        }
    }
    LOG_TRACE("Padding1");
    LOG_DUMP_BYTES(padding1, PAD1_LENGTH);

    uint8_t padding2[PAD2_LENGTH];
    memset(padding2, 0, PAD2_LENGTH);

    if (envSetUserPadding2 != NULL)
    {
        static bool envWarnSetUserPadding2 = false;
        FileImport fileReader;
        if (!fileReader.LoadHexData(envSetUserPadding2, padding2, PAD2_LENGTH))
        {
            LOG_ERROR("Invalid no. of data bytes for Padding2.\n           Expected length for Padding2 is 415 bytes");
        }
        if (!envWarnSetUserPadding2)
        {
            envWarnSetUserPadding2 = true;
            LOG_MSG("[WARNING]: RSA-PSS padding2 is set from '%s'", envSetUserPadding2);
        }
    }
    else
    {
        padding2[PAD2_LENGTH - 1] |= 0x01;
    }
    LOG_TRACE("Padding2");
    LOG_DUMP_BYTES(padding2, PAD2_LENGTH);

    //Set the leftmost 8emLen - emBits bits of the leftmost octet in maskedDB to zero
    int MSBits = ((SIGN_LENGTH_VERSAL * 8) - 1) & 0x7;
    int emLen = SIGN_LENGTH_VERSAL;

    if (MSBits == 0)
    {
        emLen--;
    }
    auto salt = std::make_unique<uint8_t[]>(SALT_LENGTH);


    #ifdef DEBUG
    if (rsaLocalENV != NULL) {
        uint8_t defsalt[SALT_LENGTH] = { 0x71, 0x5d, 0x74, 0xeb, 0x06, 0xcb, 0x2f, 0xfa, 0xed, 0x09, 0x3d, 0xd3, 0x9b, 0xa0, 0xb8, 0x57, 0xa3, 0x01, 0xcd, 0xd7, 0x52, 0x2a, 0x82, 0xc9, 0x71, 0x06, 0x8a, 0x6b, 0xb1, 0x99, 0x83, 0x52, 0x49, 0x49, 0x2b, 0xdb, 0xc8, 0x50, 0x20, 0x5e, 0x44, 0xb2, 0xc8, 0xbe, 0xbf, 0x77, 0x98, 0x22 };
        memcpy(salt.get(), defsalt, SALT_LENGTH);
    }
    else
    #endif 
    {
        RAND_bytes(salt.get(), SALT_LENGTH);
    }

    //calculate mHash - hash on message(m)
    uint8_t mHash[48] = { 0 };
    memcpy(mHash, hash, 48);

    //calculate mPad - add padding1 and salt to mHash
    uint8_t mPad[104];
    memset(mPad, 0, 104);
    memcpy(mPad, padding1, PAD1_LENGTH);
    memcpy(mPad + PAD1_LENGTH, mHash, 48);
    memcpy(mPad + PAD1_LENGTH + 48, salt.get(), SALT_LENGTH);

    //calculate hash on mPad
    std::unique_ptr<uint8_t[]> m1;  // Smart pointer for auto cleanup
    uint8_t mPadHash[48];
    m1.reset(AttachSHA3Padding(mPad, 104));  // Wrap C pointer in unique_ptr
    Versal_2ve_2vmcrypto_hash(mPadHash, m1.get(), 208, false);
    int maskedDBLen = emLen - hLen - 1; //463
    uint8_t mask[463] = { 0 };
    if (MaskGenerationFunction(mask, maskedDBLen, mPadHash, 48, EVP_sha384()) == -1)
    {
        LOG_ERROR("Internal Error : Mask generation failed during authentication.");
    }
    //DB Padding
    auto DB = std::make_unique<uint8_t[]>(463);
    memset(DB.get(), 0x0, 463);
    memcpy(DB.get(), padding2, PAD2_LENGTH);
    memcpy(DB.get() + PAD2_LENGTH, salt.get(), 48);
    uint8_t EM[512] = { 0 };

    for (int i = 0; i < 463; i++)
    {
        EM[i] = (mask[i] ^ DB[i]);
    }

    if (MSBits == 0)
    {
        EM[0] = 0;
    }

    if (envSetMSBMaskedDb != NULL) 
    {
        static bool envWarnSetMSBMaskedDb = false;
        EM[0] |= 0x80;
        if (!envWarnSetMSBMaskedDb)
        {
            envWarnSetMSBMaskedDb = true;
            LOG_MSG("[WARNING]: MSB of Masked DB set to 1. The PDI generated with this setting will not boot.");
        }
    }
    else
    {
        if (MSBits)
        {
            EM[0] &= 0xFF >> (8 - MSBits);
        }
    }
    memcpy(EM + 463, mPadHash, 48);
    EM[512 - 1] = 0xbc;
    memcpy(signature, EM, 512);

    if (envSetEM != NULL)
    {
        static bool envWarnSetEM = false;
        FileImport fileReader;
        if (!fileReader.LoadHexData(envSetEM, signature, 512))
        {
            LOG_ERROR("Invalid no. of data bytes for EM.\n           Expected length for EM is 512 bytes");
        }
        if (!envWarnSetEM)
        {
            envWarnSetEM = true;
            LOG_MSG("[WARNING]: RSA-PSS EM is set from '%s'", envSetEM);
        }
    }

    LOG_TRACE("EM");
    LOG_DUMP_BYTES(signature, 512);
    // Cleanup handled by unique_ptr for m1, DB and salt
}

/******************************************************************************/
void ECDSAHBAuthenticationAlgorithm::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    memcpy(signature, hash, hashLength);
}

/******************************************************************************/
void ECDSAP521HBAuthenticationAlgorithm::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    memcpy(signature, hash, hashLength);
}

/******************************************************************************/
void LMSAuthenticationAlgorithm::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    memcpy(signature, hash, hashLength);
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, size_t hashBlockLength, uint8_t* signatureBlock, Section* acSection)
{
    LOG_TRACE("Copying the partition (%s) signature into Authentication Certificate , hashBlockLength=%zu)", acSection->Name.c_str(), hashBlockLength);
    /* calculate hash first */
    hashLength = hash->GetHashLength();
    auto shaHash = std::make_unique<uint8_t[]>(hashLength);
    std::list<Section*>::iterator section = sections.begin();

    /* Calculate the final hash - hashBlockLength passed as parameter */
    auto hashBlock = std::make_unique<uint8_t[]>(hashBlockLength);
    memset(hashBlock.get(), 0, hashBlockLength);

    if (sections.front()->Name == "Headers")
        memcpy(hashBlock.get(), bi.imageHeaderTable->hashBlockSection->Data.get(), hashBlockLength);
    else
        memcpy(hashBlock.get(), (*section)->Data.get(), hashBlockLength);

    LOG_INFO("##################################################### Hashblock");
    LOG_DUMP_BYTES(hashBlock.get(), hashBlockLength);

    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        signatureLength = GetLmsSignatureLength(secondaryLmsParams, secondaryLmsParamsSize, sskFile.c_str(), spkFile.c_str(), lmsOnly);
    }
    else
    {
        Versal_2ve_2vmcrypto_hash(shaHash.get(), hashBlock.get(), hashBlockLength, true);
    }
    LOG_TRACE("Hash of %s (LE):", acSection->Name.c_str());
    LOG_DUMP_BYTES(shaHash.get(), hashLength);
    /* Create the PKCS padding for the hash */
    auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
    memset(shaHashPadded.get(), 0, signatureLength);
    authAlgorithm->CreatePadding(shaHashPadded.get(), shaHash.get(), hashLength);

    /* Generate hashes, if requested from command line option "-generate_hashes" */
    /*
    if (bi.options.DoGenerateHashes())
    {
        std::string hashfilename = acSection->Name;
        WritePaddedSHAFile(shaHashPadded.get(), hashfilename);
    }
    */
    authAlgorithm->RearrangeEndianess(shaHashPadded.get(), signatureLength);
#ifdef DEBUG
    LOG_TRACE("Hash Block Data ", acSection->Name.c_str());
    LOG_DUMP_BYTES(hashBlock.get(), hashBlockLength);
#endif
    /* Now sign the hash */
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        if (bi.options.DoGenerateHashes())
        {
            std::string hashfilename = acSection->Name;
            WritePaddedSHAFile(hashBlock.get(), hashBlockLength, hashfilename);
        }
        else
        {
            authAlgorithm->CreateSignature(hashBlock.get(), hashBlockLength, sskFile.c_str(),
                signatureBlock, signatureLength, lmsOnly, spkFile.c_str());
            authAlgorithm->VerifySignature(hashBlock.get(), hashBlockLength, spkFile.c_str(), signatureBlock, signatureLength, lmsOnly);
        }
    }
    else
    {
        if (bi.options.DoGenerateHashes())
        {
            std::string hashfilename = acSection->Name;
            WritePaddedSHAFile(shaHashPadded.get(), hashfilename);
        }
        else
        {
            authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)secondaryKey.get(), signatureBlock);
        }
    }

    authAlgorithm->RearrangeEndianess(signatureBlock, signatureLength);
#ifdef DEBUG
    LOG_TRACE("The partition signature is copied into Authentication Certificate");
    LOG_DUMP_BYTES(signatureBlock, signatureLength);
#endif
    /* Delete handled by unique_ptr */
    acIndex++;
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection)
{
    LOG_TRACE("Copying the partition (%s) signature into Authentication Certificate ", acSection->Name.c_str());
    /* calculate hash first */
    auto shaHash = std::make_unique<uint8_t[]>(hashLength);
    std::list<Section*>::iterator section = sections.begin();

    /* Calculate the final hash */
    size_t hashBlockLength = bi.hashBlockLength;
    if (sections.front()->Name == "Headers")
    {
        hashBlockLength = bi.imageHeaderTable->hashBlockSectionLength;
    }

    auto hashBlock = std::make_unique<uint8_t[]>(hashBlockLength);
    memset(hashBlock.get(), 0, hashBlockLength);

    if (sections.front()->Name == "Headers" && bi.imageHeaderTable->hashBlockSection)
        memcpy(hashBlock.get(), bi.imageHeaderTable->hashBlockSection->Data.get(), hashBlockLength);
    else
        memcpy(hashBlock.get(), (*section)->Data.get(), hashBlockLength);

    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        signatureLength = GetLmsSignLength(sskFile.c_str(), lmsOnly);
    }
    else
    {
        Versal_2ve_2vmcrypto_hash(shaHash.get(), hashBlock.get(), hashBlockLength, true);
    }
    LOG_TRACE("Hash of %s (LE):", acSection->Name.c_str());
    LOG_DUMP_BYTES(shaHash.get(), hashLength);
    /* Create the PKCS padding for the hash */
    auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
    memset(shaHashPadded.get(), 0, signatureLength);
    authAlgorithm->CreatePadding(shaHashPadded.get(), shaHash.get(), hashLength);

    /* Generate hashes, if requested from command line option "-generate_hashes" */
    if (bi.options.DoGenerateHashes())
    {
        std::string hashfilename = acSection->Name;
        WritePaddedSHAFile(shaHashPadded.get(), hashfilename);
    }
    authAlgorithm->RearrangeEndianess(shaHashPadded.get(), signatureLength);
#ifdef DEBUG
    LOG_TRACE("Hash Block Data ", acSection->Name.c_str());
    LOG_DUMP_BYTES(hashBlock.get(), hashBlockLength);
#endif
    /* Now sign the hash */
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        authAlgorithm->CreateSignature(hashBlock.get(), hashBlockLength, sskFile.c_str(), 
                signatureBlock, signatureLength, lmsOnly, spkFile.c_str());
        authAlgorithm->VerifySignature(hashBlock.get(), hashBlockLength, spkFile.c_str(), signatureBlock, signatureLength, lmsOnly);
    }
    else
    {
        authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)secondaryKey.get(), signatureBlock);
    }
    
    authAlgorithm->RearrangeEndianess(signatureBlock, signatureLength);
#ifdef DEBUG
    LOG_TRACE("The partition signature is copied into Authentication Certificate");
    LOG_DUMP_BYTES(signatureBlock, signatureLength);
#endif
    /* Delete the temporarily created arrays */
    // unique_ptr handles deletion
    // unique_ptr handles deletion
    acIndex++;
}

/******************************************************************************/
void RSA4096Sha3PaddingHBAuthenticationAlgorithm::RearrangeEndianess(uint8_t *array, uint32_t size)
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
void ECDSAHBAuthenticationAlgorithm::RearrangeEndianess(uint8_t *array, uint32_t size)
{
    uint32_t lastIndex = size - 1;
    char tempInt = 0;

    // If array is NULL, return
    if (!array)
    {
        return;
    }

    //SH - Revisit find a way to correct this for common code calling of ecdsa RearrangeEndianess 
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
void ECDSAP521HBAuthenticationAlgorithm::RearrangeEndianess(uint8_t *array, uint32_t size)
{
    uint32_t lastIndex = size - 1;
    char tempInt = 0;

    // If array is NULL, return
    if (!array)
    {
        return;
    }

    //SH - Revisit find a way to correct this for common code calling of ecdsa RearrangeEndianess 
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
void ECDSAHBAuthenticationAlgorithm::ECDSASignature(const uint8_t *base, EC_KEY *eckeyU, uint8_t *result0)
{
    EC_KEY* eckeyN = EC_KEY_new();
    EC_GROUP* ecgroup = EC_GROUP_new_by_curve_name(NID_secp384r1);
    EC_KEY_set_group(eckeyN, ecgroup);
    EC_KEY_set_asn1_flag(eckeyN, OPENSSL_EC_NAMED_CURVE);

    // Get private/public keys (don't allocate new BIGNUM - would leak!)
    const BIGNUM* prv = EC_KEY_get0_private_key(eckeyU);
    const EC_POINT *pub = EC_KEY_get0_public_key(eckeyU);

    /* add the private & public key to the EC_KEY structure */
    EC_KEY_set_private_key(eckeyN, prv);
    EC_KEY_set_public_key(eckeyN, pub);

    /* create and verify signature */
    ECDSA_SIG* signature = ECDSA_do_sign(base, 48, eckeyN);
    if (1 != ECDSA_do_verify(base, 48, signature, eckeyN)) {
        LOG_ERROR("Failed to verify EC Signature\n");
    }
    else {
        LOG_TRACE("Verified EC Signature\n");
    }

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
void ECDSAP521HBAuthenticationAlgorithm::ECDSASignature(const uint8_t *base, EC_KEY *eckeyU, uint8_t *result0)
{
    EC_KEY* eckeyN = EC_KEY_new();
    EC_GROUP* ecgroup = EC_GROUP_new_by_curve_name(NID_secp521r1);
    EC_KEY_set_group(eckeyN, ecgroup);
    EC_KEY_set_asn1_flag(eckeyN, OPENSSL_EC_NAMED_CURVE);

    // Get private/public keys (don't allocate new BIGNUM - would leak!)
    const BIGNUM* prv = EC_KEY_get0_private_key(eckeyU);
    const EC_POINT *pub = EC_KEY_get0_public_key(eckeyU);
    
    /* add the private & public key to the EC_KEY structure */
    EC_KEY_set_private_key(eckeyN, prv);
    EC_KEY_set_public_key(eckeyN, pub);

    /* create and verify signature */
    ECDSA_SIG* signature = ECDSA_do_sign(base, 48, eckeyN);
    if (1 != ECDSA_do_verify(base, 48, signature, eckeyN)) {
        LOG_ERROR("Failed to verify EC Signature\n");
    }
    else {
        LOG_TRACE("Verified EC521 Signature\n");
    }

    if (signature != NULL)
    {
        auto x1 = std::make_unique<uint8_t[]>(EC_P521_KEY_LENGTH2);
        auto y1 = std::make_unique<uint8_t[]>(EC_P521_KEY_LENGTH2);
        memset(x1.get(), 0, EC_P521_KEY_LENGTH2);
        memset(y1.get(), 0, EC_P521_KEY_LENGTH2);

        uint32_t signSzR;
        uint32_t signSzS;

#if OPENSSL_VERSION_NUMBER > 0x10100000L
        const BIGNUM *sig_r = NULL, *sig_s = NULL;
        ECDSA_SIG_get0(signature, &sig_r, &sig_s);

        signSzR = BN_num_bytes(sig_r);
        signSzS = BN_num_bytes(sig_s);

        if (signSzR == EC_P521_KEY_LENGTH1)
        {
            memcpy(x1.get() + 1, sig_r->d, signSzR);
            RearrangeEndianess(x1.get() + 1, signSzR);
        }
        else
        {
            memcpy(x1.get(), sig_r->d, signSzR);
            RearrangeEndianess(x1.get(), signSzR);
        }
        if (signSzS == EC_P521_KEY_LENGTH1)
        {
            memcpy(y1.get() + 1, sig_s->d, signSzS);
            RearrangeEndianess(y1.get() + 1, signSzS);
        }
        else
        {
            memcpy(y1.get(), sig_s->d, signSzS);
            RearrangeEndianess(y1.get(), signSzS);
        }

        memcpy(result0, x1.get(), EC_P521_KEY_LENGTH2);
        memcpy(result0 + EC_P521_KEY_LENGTH2, y1.get(), EC_P521_KEY_LENGTH2);
#else
        signSzR = BN_num_bytes(signature->r);
        signSzS = BN_num_bytes(signature->s);

        if (signSzR == EC_P521_KEY_LENGTH1)
        {
            memcpy(x1.get() + 1, signature->r->d, signSzR);
            RearrangeEndianess(x1.get() + 1, signSzR);
        }
        else
        {
            memcpy(x1.get(), signature->r->d, signSzR);
            RearrangeEndianess(x1.get(), signSzR);
        }
        if (signSzS == EC_P521_KEY_LENGTH1)
        {
            memcpy(y1.get() + 1, signature->s->d, signSzS);
            RearrangeEndianess(y1.get() + 1, signSzS);
        }
        else
        {
            memcpy(y1.get(), signature->s->d, signSzS);
            RearrangeEndianess(y1.get(), signSzS);
        }

        memcpy(result0, x1.get(), EC_P521_KEY_LENGTH2);
        memcpy(result0 + EC_P521_KEY_LENGTH2, y1.get(), EC_P521_KEY_LENGTH2);
#endif
        if (x1 != NULL)
        {
            // unique_ptr handles deletion
        }
        if (y1 != NULL)
        {
            // unique_ptr handles deletion
        }
    }

    EC_GROUP_free(ecgroup);
    EC_KEY_free(eckeyN);
    ECDSA_SIG_free(signature);
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header)
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
void Versal_2ve_2vmAuthenticationContext::GetPresign(const std::string& presignFilename, uint16_t signatureLength, uint8_t* signature, uint32_t index)
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
        if (size == signatureLength)
        {
            // read binary
            filePtr = fopen(filename.c_str(), "rb");
            long read_size = fread(signature, 1, signatureLength, filePtr);
            if (read_size != signatureLength)
            {
                LOG_ERROR("Authentication Error !!!\n           Presign file %s should be of %d bytes", baseFile.c_str(), SIGN_LENGTH_VERSAL);
            }
            fclose(filePtr);
        }
        else
        {
            // read ascii
            filePtr = fopen(filename.c_str(), "r");
            for (int i = 0; i<signatureLength; i++)
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
void Versal_2ve_2vmAuthenticationContext::GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index)
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
void Versal_2ve_2vmAuthenticationContext::SetSPKSignatureFile(const std::string& filename)
{
    // Handle empty filename gracefully
    if (filename.empty())
    {
        return;
    }

    FILE* filePtr;
    filePtr = fopen(filename.c_str(), "r");

    if (filePtr)
    {
        fclose(filePtr);
        GetPresign(filename, signatureLength, spksignature.get(), 0);
        spkSignLoaded = true;
        spkSignRequested = "";
    }
    else
    {
        LOG_ERROR("Failure reading SPK Signature file - %s", StringUtils::BaseName(filename).c_str());
    }
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::SetBHSignatureFile(const std::string& filename)
{
    FILE* filePtr;
    filePtr = fopen(filename.c_str(), "r");

    if (filePtr)
    {
        fclose(filePtr);
        GetPresign(filename, bHsignature.get(), 0);
        bhSignLoaded = true;
    }
    else
    {
        LOG_ERROR("Failure reading Boot Header Signature file - %s", StringUtils::BaseName(filename).c_str());
    }
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::GenerateSPKSignature(const std::string& filename)
{
    if (primaryKey->Loaded && primaryKey->isSecret)
    {
        auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
        auto spkSignatureTemp = std::make_unique<uint8_t[]>(signatureLength);
        memset(shaHashPadded.get(), 0, signatureLength);
        memset(spkSignatureTemp.get(), 0, signatureLength);
        GenerateSPKHash(shaHashPadded.get());
        authAlgorithm->RearrangeEndianess(shaHashPadded.get(), signatureLength);
        authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)primaryKey.get(), spkSignatureTemp.get());
        authAlgorithm->RearrangeEndianess(spkSignatureTemp.get(), signatureLength);
        LOG_INFO("SPK Signature generated successfully");

        if (filename != "")
        {
            FILE* filePtr;
            filePtr = fopen(filename.c_str(), "w");
            if (filePtr)
            {
                for (uint32_t i = 0; i<signatureLength; i++)
                {
                    fprintf(filePtr, "%02X", spkSignatureTemp.get()[i]);
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
        // unique_ptr handles deletion
        // unique_ptr handles deletion
    }
    else
    {
        LOG_ERROR("-spksignature error !!!           PSK missing, cannot sign SPK for generating SPK Signature");
    }
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::ResizeIfNecessary(Section* section)
{
    // round up to next 64 byte boundary.
    uint32_t dataSizePadded = (section->Length + 63) & 0xFFFFFFC0;
    section->IncreaseLengthAndPadTo(dataSizePadded, 0xFF);
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::LoadUdfData(const std::string& udfFilename, uint8_t* signature)
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
void Versal_2ve_2vmAuthenticationContext::CreateSPKSignature(BootImage& bi)
{
    LOG_TRACE("Creating the SPK signature");

    /* SPK is signed with PSK (Primary Secret Key)
    Check if PSK is given */

    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        uint16_t totalSignatureLength = 0;
        size_t totalKeySize = 0;
        size_t actualKeySize = 0;
        actualKeySize = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);
        totalKeySize = actualKeySize + PADDING_16B(actualKeySize);
        signatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);
        totalSignatureLength = signatureLength + PADDING_16B(signatureLength);

        auto spkFull = std::make_unique<uint8_t[]>(actualKeySize);

        if (!secondaryKey->Loaded)
        {
            ParseSPKeyFile(spkFile);
        }
        secondaryKey->Export(spkFull.get());

        // acTotalSpkSize; acActualSpkSize; acSpkTotalSignatureSize; acSpkActualSignatureSize; acSpkId; acSpkHdrAlignment[3]; , acSpk
        //acSpkSize, acSpkSignatureSize, acSpkId, acSpkHdrAlignment, acSpk
        auto tempBuffer = std::make_unique<uint8_t[]>(totalKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
        memset(tempBuffer.get(), 0, totalKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);

        WriteLittleEndian32(tempBuffer.get(), totalKeySize);
        WriteLittleEndian32(tempBuffer.get() + sizeof(uint32_t), actualKeySize);
        WriteLittleEndian32(tempBuffer.get() + (2 * sizeof(uint32_t)), totalSignatureLength);
        WriteLittleEndian32(tempBuffer.get() + (3 * sizeof(uint32_t)), signatureLength);
        WriteLittleEndian32(tempBuffer.get() + (4 * sizeof(uint32_t)), spkIdentification);
        memcpy(tempBuffer.get() + TELLURIDE_AC_SPK_HDR_LENGTH, spkFull.get(), actualKeySize);

        if(bi.options.DoGenerateHashes())
        {
            std::string hashfilename = bi.bifOptions->GetSPKFileName() + hash->GetHashFileExtension();
            WritePaddedSHAFile(tempBuffer.get(), actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, hashfilename);
        }
        else
        {
        #ifdef DEBUG
            LOG_TRACE("DATA being Hashed for SPK Sign");
            LOG_DUMP_BYTES(tempBuffer.get(), actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
        #endif
            if (primaryKey->Loaded && primaryKey->isSecret)
            {
                authAlgorithm->CreateSignature(tempBuffer.get(), actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, pskFile.c_str(),
                    spksignature.get(), signatureLength, lmsOnly, ppkFile.c_str());
        #ifdef DEBUG
                LOG_TRACE("spksignature");
                LOG_DUMP_BYTES(spksignature.get(), signatureLength);
        #endif
                authAlgorithm->VerifySignature(tempBuffer.get(), actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, ppkFile.c_str(),
                    spksignature.get(), signatureLength, lmsOnly);
            }
            else
            {
                LOG_ERROR("Authentication Error !!!\n          PSK or SPK signature missing, must specify atleast one");
            }
        }

    }
    else
    {
        auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
        memset(shaHashPadded.get(), 0, signatureLength);
        memset(spksignature.get(), 0, signatureLength);
        // Calulate the SPK hash with PKCS padding
        GenerateSPKHash(shaHashPadded.get());

        if(bi.options.DoGenerateHashes())
        {
            std::string hashfilename = bi.bifOptions->GetSPKFileName() + hash->GetHashFileExtension();
            WritePaddedSHAFile(shaHashPadded.get(), hashfilename);
        }
        else
        {
            authAlgorithm->RearrangeEndianess(shaHashPadded.get(), signatureLength);

            if (primaryKey->Loaded && primaryKey->isSecret)
            {
                authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)primaryKey.get(), spksignature.get());
            }
            else
            {
                LOG_ERROR("Authentication Error !!!\n          PSK or SPK signature missing, must specify atleast one");
            }
        }
    }

    authAlgorithm->RearrangeEndianess(spksignature.get(), signatureLength);

    if(authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        auto cacheKey = std::make_tuple(spkFile, ppkFile, spkIdentification);
        if(bi.spkFileAndSpkSignature.find(cacheKey) == bi.spkFileAndSpkSignature.end())
            bi.spkFileAndSpkSignature.emplace(cacheKey, std::vector<uint8_t>(spksignature.get(), spksignature.get() + signatureLength));
    }
    else
    {
        auto cacheKey = std::make_tuple(sskFile, pskFile, spkIdentification);
        if(bi.spkFileAndSpkSignature.find(cacheKey) == bi.spkFileAndSpkSignature.end())
            bi.spkFileAndSpkSignature.emplace(cacheKey, std::vector<uint8_t>(spksignature.get(), spksignature.get() + signatureLength));
    }

    LOG_INFO("SPK signature created successfully");
}

/******************************************************************************/
uint32_t ECDSAHBAuthenticationAlgorithm::GetAuthHeader(void)
{
    return AUTH_HDR_TELLURIDE_ECDSA_P384;
}

/******************************************************************************/
uint32_t ECDSAP521HBAuthenticationAlgorithm::GetAuthHeader(void)
{
    return AUTH_HDR_TELLURIDE_ECDSA_P521;
}

/******************************************************************************/
uint32_t RSA4096Sha3PaddingHBAuthenticationAlgorithm::GetAuthHeader(void)
{
    return AUTH_HDR_TELLURIDE_RSA_4096;
}

/******************************************************************************/
uint32_t LMSAuthenticationAlgorithm::GetAuthHeader(bool lmsOnly)
{   

    if (lmsOnly)
        return  AUTH_HDR_TELLURIDE_LMS;
    else
        return AUTH_HDR_TELLURIDE_HSS_LMS;
}

/******************************************************************************/
void static SetDeviceDNA(const uint8_t* dnaValue, uint32_t* deviceDNA)
{
    for (uint32_t index = 0; index < WORDS_PER_DEVICE_DNA; index++)
    {
        deviceDNA[WORDS_PER_DEVICE_DNA - (index+1)] = ReadBigEndian32(dnaValue);
        dnaValue += sizeof(uint32_t);
    }
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::CreateAuthJtagImage(Options& options, uint8_t* buffer, AuthJtagInfo authJtagAttributes)
{
    LOG_TRACE("Creating the Authentication Header signature");
    if (authAlgorithm->Type() == Authentication::RSA)
    {
        AuthenticatedJtagRSAImageStructure* authJtagImage = (AuthenticatedJtagRSAImageStructure*)buffer;
        if (primaryKey->Loaded)
        {
            authJtagImage->idWord = AUTH_JTAG_IMAGE_IDENTIFICATION_WORD;
            WriteLittleEndian32(&authJtagImage->authJtagImageLength, sizeof(AuthenticatedJtagRSAImageStructure));

            hashLength = hash->GetHashLength();
            auto shaHash = std::make_unique<uint8_t[]>(hashLength);
            auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
            memset(shaHash.get(), 0, hashLength);
            memset(shaHashPadded.get(), 0, signatureLength);

            uint32_t acHdr = authAlgorithm->GetAuthHeader();

            WriteLittleEndian32(&authJtagImage->acHeader, acHdr);
            WriteLittleEndian32(&authJtagImage->totalppkSize, VERSAL_ACKEY_STRUCT_SIZE + TELLURIDE_RSA_AC_PPK_SPK_ALIGNMENT);
            WriteLittleEndian32(&authJtagImage->actualppkSize, VERSAL_ACKEY_STRUCT_SIZE);

            WriteLittleEndian32(&authJtagImage->totalAuthJtagSignSize, signatureLength);
            WriteLittleEndian32(&authJtagImage->actualAuthJtagSignSize, signatureLength);

            if (!authJtagAttributes.userRevokeId)
            {
                LOG_WARNING("revoke_id is not specified in BIF, default revoke id is assigned as '0'.");
            }
            WriteLittleEndian32(&authJtagImage->authJtagMsgrevokeId, authJtagAttributes.partitionRevokeId);
            if (authJtagAttributes.userDeviceDNA)
            {
                WriteLittleEndian32(&authJtagImage->attributes, authJtagMessagenMask << authJtagMessageShift);
            }
            SetDeviceDNA(authJtagAttributes.deviceDNA, (uint32_t*)authJtagImage->deviceDNA);
            WriteLittleEndian32(&authJtagImage->jtagTimeOut, authJtagAttributes.jtagTimeout);

            primaryKey->Export(authJtagImage->acPpk);
            authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_N, RSA_4096_N_SIZE);
            authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
            authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_E, RSA_4096_E_SIZE);

            WriteLittleEndian32(&authJtagImage->totalspkSize, VERSAL_ACKEY_STRUCT_SIZE + TELLURIDE_RSA_AC_PPK_SPK_ALIGNMENT);
            WriteLittleEndian32(&authJtagImage->actualspkSize, VERSAL_ACKEY_STRUCT_SIZE);

            WriteLittleEndian32(&authJtagImage->totalspkSignSize, signatureLength);
            WriteLittleEndian32(&authJtagImage->actualspkSignSize, signatureLength);
            WriteLittleEndian32(&authJtagImage->spkRevokeId, authJtagAttributes.spkRevokeId);

            secondaryKey->Export(authJtagImage->acSpk);
            authAlgorithm->RearrangeEndianess(authJtagImage->acSpk + RSA_4096_N, RSA_4096_N_SIZE);
            authAlgorithm->RearrangeEndianess(authJtagImage->acSpk + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
            authAlgorithm->RearrangeEndianess(authJtagImage->acSpk + RSA_4096_E, RSA_4096_E_SIZE);

            //SPK Sign
            //SPK Header Offset - 0x450, size to be signed - 20 +  
            hash->CalculateHash(true, (uint8_t*)authJtagImage + 0x450, 0x20 + VERSAL_ACKEY_STRUCT_SIZE + TELLURIDE_RSA_AC_PPK_SPK_ALIGNMENT, shaHash.get());
            authAlgorithm->CreatePadding(shaHashPadded.get(), shaHash.get(), hashLength);

            if (primaryKey->isSecret)
            {
                authAlgorithm->RearrangeEndianess(shaHashPadded.get(), signatureLength);
                authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)primaryKey.get(), authJtagImage->spkSignature);
                authAlgorithm->RearrangeEndianess(authJtagImage->spkSignature, signatureLength);
            }
            else
            {
                if (spkSignLoaded)
                {
                    uint32_t index = 0;
                    GetPresign(spkSignFile, signatureLength, authJtagImage->spkSignature, index);
                }
                else
                {
                    if(options.DoGenerateHashes())
                    {
                        std::string hashfilename = "spk" + hash->GetHashFileExtension();
                        WritePaddedSHAFile(shaHashPadded.get(), hashfilename);
                    }
                }
            }
            //Auth Jtag Msg Sign
            hash->CalculateHash(true, (uint8_t*)authJtagImage, sizeof(AuthenticatedJtagRSAImageStructure) - signatureLength, shaHash.get());
            authAlgorithm->CreatePadding(shaHashPadded.get(), shaHash.get(), hashLength);
            
            if (secondaryKey->isSecret)
            {
                authAlgorithm->RearrangeEndianess(shaHashPadded.get(), signatureLength);
                authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)secondaryKey.get(), authJtagImage->authJtagSignature);
                authAlgorithm->RearrangeEndianess(authJtagImage->authJtagSignature, signatureLength);
            }
            else
            {
                if (authJtagAttributes.jtagSignatureFile != "")
                {
                    uint32_t index = 0;
                    GetPresign(authJtagAttributes.jtagSignatureFile, signatureLength, authJtagImage->authJtagSignature, index);
                }
                else
                {
                    if(options.DoGenerateHashes())
                    {
                        std::string hashfilename = "authJtag" + hash->GetHashFileExtension();
                        WritePaddedSHAFile(shaHashPadded.get(), hashfilename);
                    }
                }
            }
            // unique_ptr handles deletion
            // unique_ptr handles deletion
        }
        else
        {
            LOG_ERROR("Authentication Error !!!\n          PSK must be specified to generate Authenticated Jtag Image");
        }
    }
    else if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        AuthenticatedJtagECP384ImageStructure* authJtagImage = (AuthenticatedJtagECP384ImageStructure*)buffer;
        if (primaryKey->Loaded)
        {
            authJtagImage->idWord = AUTH_JTAG_IMAGE_IDENTIFICATION_WORD;
            WriteLittleEndian32(&authJtagImage->authJtagImageLength, sizeof(AuthenticatedJtagECP384ImageStructure));

            hashLength = hash->GetHashLength();
            auto shaHash = std::make_unique<uint8_t[]>(hashLength);
            auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
            memset(shaHash.get(), 0, hashLength);
            memset(shaHashPadded.get(), 0, signatureLength);

            uint32_t acHdr = authAlgorithm->GetAuthHeader();

            WriteLittleEndian32(&authJtagImage->acHeader, acHdr);
            WriteLittleEndian32(&authJtagImage->totalppkSize, 2 * EC_P384_KEY_LENGTH);
            WriteLittleEndian32(&authJtagImage->actualppkSize, 2 * EC_P384_KEY_LENGTH);

            WriteLittleEndian32(&authJtagImage->totalAuthJtagSignSize, signatureLength);
            WriteLittleEndian32(&authJtagImage->actualAuthJtagSignSize, signatureLength);

            if (!authJtagAttributes.userRevokeId)
            {
                LOG_WARNING("revoke_id is not specified in BIF, default revoke id is assigned as '0'.");
            }
            WriteLittleEndian32(&authJtagImage->authJtagMsgrevokeId, authJtagAttributes.partitionRevokeId);

            if (authJtagAttributes.userDeviceDNA)
            {
                WriteLittleEndian32(&authJtagImage->attributes, authJtagMessagenMask << authJtagMessageShift);
            }
            SetDeviceDNA(authJtagAttributes.deviceDNA, (uint32_t*)authJtagImage->deviceDNA);
            WriteLittleEndian32(&authJtagImage->jtagTimeOut, authJtagAttributes.jtagTimeout);

            primaryKey->Export(authJtagImage->acPpk);
            authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_N, RSA_4096_N_SIZE);
            authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
            authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_E, RSA_4096_E_SIZE);

            WriteLittleEndian32(&authJtagImage->totalspkSize, 2 * EC_P384_KEY_LENGTH);
            WriteLittleEndian32(&authJtagImage->actualspkSize, 2 * EC_P384_KEY_LENGTH);

            WriteLittleEndian32(&authJtagImage->totalspkSignSize, signatureLength);
            WriteLittleEndian32(&authJtagImage->actualspkSignSize, signatureLength);
            WriteLittleEndian32(&authJtagImage->spkRevokeId, authJtagAttributes.spkRevokeId);

            secondaryKey->Export(authJtagImage->acSpk);
            authAlgorithm->RearrangeEndianess(authJtagImage->acSpk + RSA_4096_N, RSA_4096_N_SIZE);
            authAlgorithm->RearrangeEndianess(authJtagImage->acSpk + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
            authAlgorithm->RearrangeEndianess(authJtagImage->acSpk + RSA_4096_E, RSA_4096_E_SIZE);

            //SPK Sign
            //SPK Header Offset - 0xA0, size to be signed - 20 +  
            hash->CalculateHash(true, (uint8_t*)authJtagImage + 0xA0, 0x20 + 2 * EC_P384_KEY_LENGTH, shaHash.get());
            authAlgorithm->CreatePadding(shaHashPadded.get(), shaHash.get(), hashLength);

            if (primaryKey->isSecret)
            {
                authAlgorithm->RearrangeEndianess(shaHashPadded.get(), signatureLength);
                authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)primaryKey.get(), authJtagImage->spkSignature);
                authAlgorithm->RearrangeEndianess(authJtagImage->spkSignature, signatureLength);
            }
            else
            {
                if (spkSignLoaded)
                {
                    uint32_t index = 0;
                    GetPresign(spkSignFile, signatureLength, authJtagImage->spkSignature, index);
                }
                else
                {
                    if(options.DoGenerateHashes())
                    {
                        std::string hashfilename = "spk" + hash->GetHashFileExtension();
                        WritePaddedSHAFile(shaHashPadded.get(), hashfilename);
                    }
                }
            }

            //Auth Jtag Msg Sign
            hash->CalculateHash(true, (uint8_t*)authJtagImage, sizeof(AuthenticatedJtagECP384ImageStructure) - signatureLength, shaHash.get());
            authAlgorithm->CreatePadding(shaHashPadded.get(), shaHash.get(), hashLength);

            if (secondaryKey->isSecret)
            {
                authAlgorithm->RearrangeEndianess(shaHashPadded.get(), signatureLength);
                authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)secondaryKey.get(), authJtagImage->authJtagSignature);
                authAlgorithm->RearrangeEndianess(authJtagImage->authJtagSignature, signatureLength);
            }
            else
            {
                if (authJtagAttributes.jtagSignatureFile != "")
                {
                    uint32_t index = 0;
                    GetPresign(authJtagAttributes.jtagSignatureFile, signatureLength, authJtagImage->authJtagSignature, index);
                }
                else
                {
                    if(options.DoGenerateHashes())
                    {
                        std::string hashfilename = "authJtag" + hash->GetHashFileExtension();
                        WritePaddedSHAFile(shaHashPadded.get(), hashfilename);
                    }
                }
            }

        }
    }
    else if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        uint8_t* authJtagImage = buffer;
        if (primaryKey->Loaded)
        {
            size_t actualppkSize = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly);
            size_t actualspkSize = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);
            size_t spkSignLength = GetLmsSignatureLength(primaryLmsParams, primaryLmsParamsSize, pskFile.c_str(), ppkFile.c_str(), lmsOnly);
            size_t authJtagSignLength = GetLmsSignatureLength(secondaryLmsParams, secondaryLmsParamsSize, sskFile.c_str(), spkFile.c_str(), lmsOnly);

            if (actualppkSize > AUTH_JTAG_LMS_PUBLIC_KEY_SIZE || actualspkSize > AUTH_JTAG_LMS_PUBLIC_KEY_SIZE)
            {
                LOG_ERROR("Authentication Error !!!\n          PPK/SPK size exceeds AUTH_JTAG_LMS_PUBLIC_KEY_SIZE (60) for Authenticated Jtag Image");
            }
            else
            {
                /* Compute offsets on the fly from key and signature sizes (like authentication certificate) */
                size_t ppkOffset = AUTH_JTAG_LMS_PPK_OFFSET;
                size_t spkHeaderOffset = AUTH_JTAG_LMS_FIXED_HEADER_SIZE + actualppkSize + PADDING_16B(actualppkSize);
                size_t spkOffset = spkHeaderOffset + AUTH_JTAG_LMS_SPK_HEADER_LENGTH;
                size_t spkSignOffset = spkOffset + actualspkSize + PADDING_16B(actualspkSize);
                size_t authSignOffset = spkSignOffset + spkSignLength + PADDING_16B(spkSignLength);
                
                WriteLittleEndian32(authJtagImage + AUTH_JTAG_LMS_ID_WORD_OFFSET, AUTH_JTAG_IMAGE_IDENTIFICATION_WORD);

                uint32_t authJtagImageLength = (uint32_t)(authSignOffset + authJtagSignLength);
                WriteLittleEndian32(authJtagImage + AUTH_JTAG_LMS_IMAGE_LENGTH_OFFSET, authJtagImageLength);

                uint32_t acHdr = authAlgorithm->GetAuthHeader(lmsOnly, false, false);
                WriteLittleEndian32(authJtagImage + AUTH_JTAG_LMS_AC_HEADER_OFFSET, acHdr);
                WriteLittleEndian32(authJtagImage + AUTH_JTAG_LMS_TOTAL_PPK_SIZE_OFFSET, actualppkSize + PADDING_16B(actualppkSize));
                WriteLittleEndian32(authJtagImage + AUTH_JTAG_LMS_ACTUAL_PPK_SIZE_OFFSET, (uint32_t)actualppkSize);
                WriteLittleEndian32(authJtagImage + AUTH_JTAG_LMS_TOTAL_AUTH_SIGN_SIZE_OFFSET, authJtagSignLength + PADDING_16B(authJtagSignLength));
                WriteLittleEndian32(authJtagImage + AUTH_JTAG_LMS_ACTUAL_AUTH_SIGN_SIZE_OFFSET, (uint32_t)authJtagSignLength);

                if (!authJtagAttributes.userRevokeId)
                {
                    LOG_WARNING("revoke_id is not specified in BIF, default revoke id is assigned as '0'.");
                }
                WriteLittleEndian32(authJtagImage + AUTH_JTAG_LMS_MSG_REVOKE_ID_OFFSET, authJtagAttributes.partitionRevokeId);

                if (authJtagAttributes.userDeviceDNA)
                {
                    WriteLittleEndian32(authJtagImage + AUTH_JTAG_LMS_ATTRIBUTES_OFFSET, authJtagMessagenMask << authJtagMessageShift);
                }
                SetDeviceDNA(authJtagAttributes.deviceDNA, (uint32_t*)(authJtagImage + AUTH_JTAG_LMS_DEVICE_DNA_OFFSET));
                WriteLittleEndian32(authJtagImage + AUTH_JTAG_LMS_JTAG_TIMEOUT_OFFSET, authJtagAttributes.jtagTimeout);

                primaryKey->Export(authJtagImage + ppkOffset);
                if (!secondaryKey->Loaded)
                {
                    ParseSPKeyFile(spkFile);
                }
                secondaryKey->Export(authJtagImage + spkOffset);

                WriteLittleEndian32(authJtagImage + spkHeaderOffset, actualspkSize + PADDING_16B(actualspkSize));
                WriteLittleEndian32(authJtagImage + spkHeaderOffset + sizeof(uint32_t), (uint32_t)actualspkSize);
                WriteLittleEndian32(authJtagImage + spkHeaderOffset + (2 * sizeof(uint32_t)), spkSignLength + PADDING_16B(spkSignLength));
                WriteLittleEndian32(authJtagImage + spkHeaderOffset + (3 * sizeof(uint32_t)), (uint32_t)spkSignLength);
                WriteLittleEndian32(authJtagImage + spkHeaderOffset + (4 * sizeof(uint32_t)), authJtagAttributes.spkRevokeId);

                uint8_t* tempBuffer = new uint8_t[TELLURIDE_AC_SPK_HDR_LENGTH + actualspkSize];
                memset(tempBuffer, 0, TELLURIDE_AC_SPK_HDR_LENGTH + actualspkSize);
                WriteLittleEndian32(tempBuffer, actualspkSize + PADDING_16B(actualspkSize));
                WriteLittleEndian32(tempBuffer + sizeof(uint32_t), actualspkSize);
                WriteLittleEndian32(tempBuffer + (2 * sizeof(uint32_t)), spkSignLength + PADDING_16B(spkSignLength));
                WriteLittleEndian32(tempBuffer + (3 * sizeof(uint32_t)), spkSignLength);
                WriteLittleEndian32(tempBuffer + (4 * sizeof(uint32_t)), authJtagAttributes.spkRevokeId);
                memcpy(tempBuffer + TELLURIDE_AC_SPK_HDR_LENGTH, authJtagImage + spkOffset, actualspkSize);

                if (primaryKey->isSecret)
                {
                    authAlgorithm->CreateSignature(tempBuffer, TELLURIDE_AC_SPK_HDR_LENGTH + actualspkSize, pskFile.c_str(),
                        authJtagImage + spkSignOffset, spkSignLength, lmsOnly, ppkFile.c_str());
                    authAlgorithm->VerifySignature(tempBuffer, TELLURIDE_AC_SPK_HDR_LENGTH + actualspkSize, ppkFile.c_str(),
                        authJtagImage + spkSignOffset, spkSignLength, lmsOnly);
                }
                else
                {
                    if (spkSignLoaded)
                    {
                        uint32_t index = 0;
                        GetPresign(spkSignFile, spkSignLength, authJtagImage + spkSignOffset, index);
                    }
                    else
                    {
                        if(options.DoGenerateHashes())
                        {
                            std::string hashfilename = "spk" + hash->GetHashFileExtension();
                            WritePaddedSHAFile(tempBuffer, TELLURIDE_AC_SPK_HDR_LENGTH + actualspkSize, hashfilename);
                        }
                    }
                }

                delete[] tempBuffer;

                if (secondaryKey->isSecret)
                {
                    authAlgorithm->CreateSignature(authJtagImage, (size_t)authSignOffset, sskFile.c_str(),
                        authJtagImage + authSignOffset, authJtagSignLength, lmsOnly, spkFile.c_str());
                    authAlgorithm->VerifySignature(authJtagImage, (size_t)authSignOffset, spkFile.c_str(),
                        authJtagImage + authSignOffset, authJtagSignLength, lmsOnly);
                }
                else
                {
                    if (authJtagAttributes.jtagSignatureFile != "")
                    {
                        uint32_t index = 0;
                        GetPresign(authJtagAttributes.jtagSignatureFile, authJtagSignLength, authJtagImage + authSignOffset, index);
                    }
                    else
                    {
                        if(options.DoGenerateHashes())
                        {
                            std::string hashfilename = "authJtag" + hash->GetHashFileExtension();
                            WritePaddedSHAFile(authJtagImage, (size_t)authSignOffset, hashfilename);
                        }
                    }
                }
  
            }
        }
        else
        {
            LOG_ERROR("Authentication Error !!!\n          PSK must be specified to generate Authenticated Jtag Image");
        }
    }
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationCertificate::Link(BootImage& bi, void* partition)
{
    LOG_TRACE("[AC-LINK] Using partition pointer overload (image_store)");
    this->AuthContext->Link(bi, partition, this);
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationCertificate::Link(BootImage& bi, Section* dataSection)
{
    LOG_TRACE("[AC-LINK] Using section pointer overload (baseline)");
    /* Gather up all the sections that will be used to calculate the authentication hash */
    std::list<Section*> sections;
    std::unique_ptr<Section> headers;

    /* If the section is a header table section */
    if (isTableHeader)
    {
        size_t size = 0;
        if (bi.options.bifOptions->GetHeaderEncyption())
        {
            size = bi.encryptedHeaders->Length;
            headers = std::make_unique<Section>("Headers", size);
            memset(headers->Data.get(), bi.options.GetOutputFillByte(), headers->Length);
            memcpy(headers->Data.get(), bi.encryptedHeaders->Data.get(), bi.encryptedHeaders->Length);
            sections.push_back(headers.release());
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
                size += sizeof(Versal_2ve_2vmPartitionHeaderTableStructure);
            }

            // Append NULL partition header
            //sections.push_back(bi.nullPartHeaderSection);

            /* Create one new combined section will all the appended sections above */
            headers = std::make_unique<Section>("Headers", size);
            //headers->Address = iHT->section->Address; // not really needed, but useful for debug.
            memset(headers->Data.get(), bi.options.GetOutputFillByte(), headers->Length);

            Binary::Address_t start = sections.front()->Address;
            for (std::list<Section*>::iterator i = sections.begin(); i != sections.end(); i++)
            {
                Section& section(**i);
                int offset = section.Address - start;
                memcpy(headers->Data.get() + offset, section.Data.get(), section.Length);
            }

            // replace sections list with the combined new section
            sections.clear();
            sections.push_back(headers.release());
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
    // Cleanup handled by unique_ptr (headers already released to sections list if used)
}

/******************************************************************************/
void Versal_2ve_2vmAuthenticationContext::SetKeyLength(Authentication::Type type)
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
    else if (type == Authentication::LMS_SHA2_256)
    {
        //AuthenticationContext::authKeyLength = EC_P521_KEY_LENGTH;
    }
}
