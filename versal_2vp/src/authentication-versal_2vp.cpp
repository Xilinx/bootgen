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
#include "authentication-versal_2vp.h"
#include "authentication-versal_2vp-hybrid.h"
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
#include "partitionheadertable-versal_2vp.h"
#include "imageheadertable-versal_2vp.h"
#include "Keccak-compact-versal_2vp.h"
#include "bootimage-versal_2vp.h"
#include "encryptutils.h"
#include <openssl/rand.h>
#include "bootheader-versal_2vp.h"

extern "C" {
#include "lms-utils.h"
#include "hss_verify.h"
};

#include "ml_dsa/ml_dsa_87.hpp"
#include "randomshake/randomshake.hpp"
#include <span>  // For variable-length ML-DSA message signing

#include "slh_dsa.h"
#include <stdlib.h>
/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

// Helper function to populate certificate header from .kmd metadata (if available) or defaults
static void PopulateHeaderFromKmdMetadata(
    HeaderAuthCert_versal_2vp* header,
    const KeyMetadata& kmdMetadata,
    uint32_t defaultAlgorithm,
    uint32_t defaultRevocationID,
    uint32_t defaultHybrid,
    uint32_t defaultAuthority,
    uint32_t defaultPermission)
{
    if (kmdMetadata.isValid) {
        // Use metadata from .kmd file
        header->Algorithm = kmdMetadata.Algorithm;
        header->RevocationID = kmdMetadata.RevocationID;
        header->Hybrid = kmdMetadata.Hybrid;
        header->Authority = kmdMetadata.Authority;
        header->Permission = kmdMetadata.Permission;
        memcpy(header->Reserved, kmdMetadata.Reserved, sizeof(header->Reserved));
        LOG_TRACE("Using .kmd metadata: Algorithm=0x%08x, RevID=0x%08x, Hybrid=0x%08x, Authority=0x%08x, Permission=0x%08x",
                 kmdMetadata.Algorithm, kmdMetadata.RevocationID, kmdMetadata.Hybrid,
                 kmdMetadata.Authority, kmdMetadata.Permission);
    } else {
        // Use default values (backward compatibility)
        header->Algorithm = defaultAlgorithm;
        header->RevocationID = defaultRevocationID;
        header->Hybrid = defaultHybrid;
        header->Authority = defaultAuthority;
        header->Permission = defaultPermission;
        memset(header->Reserved, 0, sizeof(header->Reserved));
        LOG_TRACE("Using default values: Algorithm=0x%08x, RevID=0x%08x, Hybrid=0x%08x, Authority=0x%08x, Permission=0x%08x",
                 defaultAlgorithm, defaultRevocationID, defaultHybrid, defaultAuthority, defaultPermission);
    }
}

// Helper function to get algorithm name from Authentication::Type
static std::string GetAlgorithmName(Authentication::Type authType)
{
    switch (authType) {
        case Authentication::RSA: return "RSA-4096";
        case Authentication::ECDSA: return "ECDSA-P384";
        case Authentication::ECDSAp521: return "ECDSA-P521";
        case Authentication::LMS_SHA2_256: return "LMS-SHA2_256";
        case Authentication::LMS_SHAKE256: return "LMS-SHAKE256";
        case Authentication::MLDSA: return "ML-DSA-87";
        case Authentication::SLH_SHAKE256: return "SLH-DSA-SHAKE-256f";
        default: return "Unknown";
    }
}

/******************************************************************************/
Versal_2vpAuthenticationContext::Versal_2vpAuthenticationContext(Authentication::Type type)
{
    signatureLength = SIGN_LENGTH_VERSAL;
    spkSignLoaded = false;
    memset(udf_data, 0, UDF_DATA_SIZE);
    bhSignLoaded = false;
    hashType = AuthHash::Sha3;
    ownsPrimaryKey = true;
    ownsSecondaryKey = true;
    ownsAuthAlgorithm = true;
    authAlgorithm = GetAuthenticationAlgorithm(type);
    if (type == Authentication::RSA)
    {
        primaryKey = std::make_unique<Key4096Sha3Padding_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<Key4096Sha3Padding_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::RSA;
        secondaryKey->authType = Authentication::RSA;

        certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure_versal2vp);
        signatureLength = SIGN_LENGTH_VERSAL;
    }
    else if (type == Authentication::ECDSA)
    {
        primaryKey = std::make_unique<KeyECDSA_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSA_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::ECDSA;
        secondaryKey->authType = Authentication::ECDSA;

        certSize = sizeof(AuthCertificateECDSAHBStructure_versal2vp);
        signatureLength = EC_P384_KEY_LENGTH * 2;
    }
    else if (type == Authentication::ECDSAp521)
    {
        primaryKey = std::make_unique<KeyECDSAp521_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSAp521_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::ECDSAp521;
        secondaryKey->authType = Authentication::ECDSAp521;

        certSize = sizeof(AuthCertificateECDSAp521HBStructure_versal2vp);
        signatureLength = EC_P521_KEY_LENGTH2 * 2;
    }
    else if ((type == Authentication::LMS_SHA2_256) || (type == Authentication::LMS_SHAKE256))
    {
        primaryKey = std::make_unique<KeyLMS_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyLMS_versal_2vp>("Secondary Key");
        primaryKey->lmsOnly = secondaryKey->lmsOnly = lmsOnly;
        if (type == Authentication::LMS_SHA2_256)
        {
            primaryKey->authType = Authentication::LMS_SHA2_256;
            secondaryKey->authType = Authentication::LMS_SHA2_256;
            hashType = AuthHash::Sha2;
            hash = new HashSha2();
            ownsHash = true;
        }
        else
        {
            primaryKey->authType = Authentication::LMS_SHAKE256;
            secondaryKey->authType = Authentication::LMS_SHAKE256;
            hashType = AuthHash::Shake256;
            hash = new HashShake256();
            ownsHash = true;
        }
        signatureLength = GetLmsSignLength(pskFile.c_str(),lmsOnly);
        certSize = GetCertificateSize();
    }
    else if (type == Authentication::MLDSA)
    {
        primaryKey = std::make_unique<KeyMLDSA_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyMLDSA_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::MLDSA;
        secondaryKey->authType = Authentication::MLDSA;
        hash = new HashSha2_512();
        ownsHash = true;
        hashType = AuthHash::Sha2_512;

        certSize = sizeof(AuthCertificateMLDSAStructure_versal2vp);
        signatureLength = MLDSA_TOTAL_SIGN_LEN;
    }
    else if (type == Authentication::SLH_SHAKE256)
    {
        primaryKey = std::make_unique<KeySLH_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeySLH_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::SLH_SHAKE256;
        secondaryKey->authType = Authentication::SLH_SHAKE256;
        hashType = AuthHash::Shake256;
        hash = new HashShake256();
        ownsHash = true;

        signatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
        certSize = GetCertificateSize();
    }
    spksignature = std::make_unique<uint8_t[]>(signatureLength);
    bHsignature = std::make_unique<uint8_t[]>(signatureLength);
    memset(bHsignature.get(), 0, signatureLength);
    
    // Initialize new members
    spkSignatureCreated = false;
    currentDataSection = nullptr;
    bypassHybridDetection = false;
}

/******************************************************************************/
Versal_2vpAuthenticationContext::Versal_2vpAuthenticationContext(const AuthenticationContext* refAuthContext, Authentication::Type authtype)
{
    signatureLength = SIGN_LENGTH_VERSAL;
    bHsignature = std::make_unique<uint8_t[]>(signatureLength);

    ownsPrimaryKey = true;
    ownsSecondaryKey = true;
    ownsAuthAlgorithm = true;
    authAlgorithm = GetAuthenticationAlgorithm(authtype);
    hashType = AuthHash::Sha3;
    
    const Versal_2vpHybridAuthenticationContext* hybridParent = 
        dynamic_cast<const Versal_2vpHybridAuthenticationContext*>(refAuthContext);
    
    if (hybridParent != nullptr) {
        ppkFile = "";
        pskFile = "";
        spkFile = "";
        sskFile = "";
        spkSignFile = "";
        bhSignFile = "";
    } else {
        ppkFile = refAuthContext->ppkFile;
        pskFile = refAuthContext->pskFile;
        spkFile = refAuthContext->spkFile;
        sskFile = refAuthContext->sskFile;
        spkSignFile = refAuthContext->spkSignFile;
        bhSignFile = refAuthContext->bhSignFile;
    }
    
    spkSelect = refAuthContext->spkSelect;
    spkIdentification = refAuthContext->spkIdentification;
    lmsOnly = refAuthContext->lmsOnly;

    if (authtype == Authentication::RSA)
    {
        primaryKey = std::make_unique<Key4096Sha3Padding_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<Key4096Sha3Padding_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication :: RSA;
        secondaryKey->authType = Authentication :: RSA;

        certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure_versal2vp);
        signatureLength = SIGN_LENGTH_VERSAL;
        hash = new HashSha3();
        ownsHash = true;
        hashType = AuthHash::Sha3;
    }
    else if(authtype == Authentication::ECDSA)
    {
        primaryKey = std::make_unique<KeyECDSA_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSA_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication :: ECDSA;
        secondaryKey->authType = Authentication :: ECDSA;

        certSize = sizeof(AuthCertificateECDSAHBStructure_versal2vp);
        signatureLength = EC_P384_KEY_LENGTH * 2;
        hash = new HashSha3();
        ownsHash = true;
        hashType = AuthHash::Sha3;
    }
    else if (authtype == Authentication::ECDSAp521)
    {
        primaryKey = std::make_unique<KeyECDSAp521_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSAp521_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication :: ECDSAp521;
        secondaryKey->authType = Authentication :: ECDSAp521;

        certSize = sizeof(AuthCertificateECDSAp521HBStructure_versal2vp);
        signatureLength = EC_P521_KEY_LENGTH2 * 2;
        hash = new HashSha3();
        ownsHash = true;
        hashType = AuthHash::Sha3;
    }
    else if ((authtype == Authentication::LMS_SHA2_256) || (authtype == Authentication::LMS_SHAKE256))
    {
        primaryKey = std::make_unique<KeyLMS_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyLMS_versal_2vp>("Secondary Key");
        primaryKey->lmsOnly = secondaryKey->lmsOnly = lmsOnly;
        if (authtype == Authentication::LMS_SHA2_256)
        {
            primaryKey->authType = Authentication::LMS_SHA2_256;
            secondaryKey->authType = Authentication::LMS_SHA2_256;
            hashType = AuthHash::Sha2;
            hash = new HashSha2();
            ownsHash = true;
        }
        else
        {
            primaryKey->authType = Authentication::LMS_SHAKE256;
            secondaryKey->authType = Authentication::LMS_SHAKE256;
            hashType = AuthHash::Shake256;
            hash = new HashShake256();
            ownsHash = true;
        }
        if (!pskFile.empty()) {
            signatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);
            if (signatureLength == 0) {
                LOG_WARNING("GetLmsSignLength returned 0 for key file: %s - using default", pskFile.c_str());
                signatureLength = SIGN_LENGTH_VERSAL;
            }
        }
        certSize = GetCertificateSize();
    }
    else if (authtype == Authentication::MLDSA)
    {
        primaryKey = std::make_unique<KeyMLDSA_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyMLDSA_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::MLDSA;
        secondaryKey->authType = Authentication::MLDSA;
        hash = new HashSha2_512();
        ownsHash = true;
        hashType = AuthHash::Sha2_512;

        certSize = sizeof(AuthCertificateMLDSAStructure_versal2vp);
        signatureLength = MLDSA_TOTAL_SIGN_LEN;
    }
    else if (authtype == Authentication::SLH_SHAKE256)
    {
        primaryKey = std::make_unique<KeySLH_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeySLH_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::SLH_SHAKE256;
        secondaryKey->authType = Authentication::SLH_SHAKE256;
        hashType = AuthHash::Shake256;
        hash = new HashShake256();
        ownsHash = true;

        signatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
        certSize = GetCertificateSize();
    }

    spksignature = std::make_unique<uint8_t[]>(signatureLength);
    bHsignature = std::make_unique<uint8_t[]>(signatureLength);
    
    if (hash != nullptr) {
        hashLength = hash->GetHashLength();
    }

    if (pskFile != "" || ppkFile != "")
    {
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
        primaryKey.reset(refAuthContext->primaryKey.get());
        ownsPrimaryKey = false;
    }

    if (spkFile != "" || sskFile != "")
    {
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
        secondaryKey.reset(refAuthContext->secondaryKey.get());
        ownsSecondaryKey = false;
    }
    /* The reference context may use a shorter signature than this one (e.g. a
       512-byte RSA context feeding a 4640-byte ML-DSA one), so copy only what
       it actually holds; the remainder stays zero-initialised. */
    uint16_t copyLength = (refAuthContext->signatureLength < signatureLength)
                            ? refAuthContext->signatureLength : signatureLength;

    if (spkSignFile != "")
    {
        SetSPKSignatureFile(spkSignFile);
    }
    else
    {
        memcpy(spksignature.get(), refAuthContext->spksignature.get(), copyLength);
    }

    if (bhSignFile != "")
    {
        SetBHSignatureFile(bhSignFile);
    }
    else
    {
        memcpy(bHsignature.get(), refAuthContext->bHsignature.get(), copyLength);
    }
    memcpy(udf_data, refAuthContext->udf_data, sizeof(udf_data));
    bhSignLoaded = refAuthContext->bhSignLoaded;
    spkSignLoaded = refAuthContext->spkSignLoaded;
    spkSignRequested = refAuthContext->spkSignRequested;
    preSigned = refAuthContext->preSigned;
    
    spkSignatureCreated = false;
    currentDataSection = nullptr;
    bypassHybridDetection = false;
}


/******************************************************************************/
Versal_2vpAuthenticationContext::Versal_2vpAuthenticationContext(const AuthCertificate4096Sha3PaddingHBStructure_versal2vp* existingCert, Authentication::Type authtype)
{
    signatureLength = SIGN_LENGTH_VERSAL;
    spkSignLoaded = true;
    bhSignLoaded = true;
    ownsPrimaryKey = true;
    ownsSecondaryKey = true;
    ownsAuthAlgorithm = true;
    authAlgorithm = GetAuthenticationAlgorithm(authtype);

    if (authtype == Authentication::RSA)
    {
        primaryKey = std::make_unique<Key4096Sha3Padding_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<Key4096Sha3Padding_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::RSA;
        secondaryKey->authType = Authentication::RSA;

        certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure_versal2vp);
        signatureLength = SIGN_LENGTH_VERSAL;
    }
    else if (authtype == Authentication::ECDSA)
    {
        primaryKey = std::make_unique<KeyECDSA_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSA_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::ECDSA;
        secondaryKey->authType = Authentication::ECDSA;

        certSize = sizeof(AuthCertificateECDSAHBStructure_versal2vp);
        signatureLength = EC_P384_KEY_LENGTH * 2;
    }
    else if (authtype == Authentication::ECDSAp521)
    {
        primaryKey = std::make_unique<KeyECDSAp521_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyECDSAp521_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::ECDSAp521;
        secondaryKey->authType = Authentication::ECDSAp521;
        hashType = AuthHash::Sha3;
        hash = new HashSha3();
        ownsHash = true;
        certSize = sizeof(AuthCertificateECDSAp521HBStructure_versal2vp);
        signatureLength = EC_P521_KEY_LENGTH2 * 2;
    }
    else if((authtype == Authentication::LMS_SHA2_256) || (authtype == Authentication::LMS_SHAKE256))
    {
        primaryKey = std::make_unique<KeyLMS_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyLMS_versal_2vp>("Secondary Key");
        primaryKey->lmsOnly = secondaryKey->lmsOnly = lmsOnly;
        if (authtype == Authentication::LMS_SHA2_256)
        {
            primaryKey->authType = Authentication::LMS_SHA2_256;
            secondaryKey->authType = Authentication::LMS_SHA2_256;
            hashType = AuthHash::Sha2;
            hash = new HashSha2();
            ownsHash = true;
        }
        else
        {
            primaryKey->authType = Authentication::LMS_SHAKE256;
            secondaryKey->authType = Authentication::LMS_SHAKE256;
            hashType = AuthHash::Shake256;
            hash = new HashShake256();
            ownsHash = true;
        }
        signatureLength = GetLmsSignLength(pskFile.c_str(),lmsOnly);
        certSize = GetCertificateSize();
    }
    else if (authtype == Authentication::MLDSA)
    {
        primaryKey = std::make_unique<KeyMLDSA_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeyMLDSA_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::MLDSA;
        secondaryKey->authType = Authentication::MLDSA;
        hash = new HashSha2_512();
        ownsHash = true;
        hashType = AuthHash::Sha2_512;

        certSize = sizeof(AuthCertificateMLDSAStructure_versal2vp);
        signatureLength = MLDSA_TOTAL_SIGN_LEN;
    }
    else if (authtype == Authentication::SLH_SHAKE256)
    {
        primaryKey = std::make_unique<KeySLH_versal_2vp>("Primary Key");
        secondaryKey = std::make_unique<KeySLH_versal_2vp>("Secondary Key");
        primaryKey->authType = Authentication::SLH_SHAKE256;
        secondaryKey->authType = Authentication::SLH_SHAKE256;
        hashType = AuthHash::Shake256;
        hash = new HashShake256();
        ownsHash = true;

        signatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
        certSize = GetCertificateSize();
    }
    else {
    }

    spksignature = std::make_unique<uint8_t[]>(signatureLength);
    bHsignature = std::make_unique<uint8_t[]>(signatureLength);
    memset(bHsignature.get(), 0, signatureLength);
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
    
    // Initialize new members
    spkSignatureCreated = false;
    currentDataSection = nullptr;
    bypassHybridDetection = false;
}

/******************************************************************************/
std::unique_ptr<AuthenticationAlgorithm> Versal_2vpAuthenticationContext::GetAuthenticationAlgorithm(Authentication::Type type)
{
    if (type == Authentication::ECDSA)
    {
        SetAuthenticationKeyLength(EC_P384_KEY_LENGTH);
        return std::make_unique<ECDSAHBAuthenticationAlgorithm_versal_2vp>();
    }
    else if (type == Authentication::ECDSAp521)
    {
        return std::make_unique<ECDSAP521HBAuthenticationAlgorithm_versal_2vp>();
    }
    else if(type == Authentication::RSA)
    {
        SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
        return std::make_unique<RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp>();
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
        return std::make_unique<LMSAuthenticationAlgorithm_versal_2vp>(type);
    }
    else if(type == Authentication::MLDSA)
    {
        return std::make_unique<MLDSAAuthenticationAlgorithm_versal_2vp>(type);
    }
    else if (type == Authentication::SLH_SHAKE256)
    {
        hashType = AuthHash::Shake256;
        return std::make_unique<SLHAuthenticationAlgorithm_versal_2vp>(type);
    }
    else
    {
        return nullptr;
    }
}

/******************************************************************************/
uint32_t Versal_2vpAuthenticationContext::GetCertificateSize(void)
{
    if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256) || (authAlgorithm->Type() == Authentication::LMS_SHAKE256))
    {
        /* 
        New versal_2vp AC Structure for LMS/HSS:
        --- PPK Header
        HeaderAuthCert_versal_2vp ppkheader;         // (0x00)   : 24 bytes (6 * 4 bytes)
        --- PPK
        HssPublicKey        acPpk;                   // (0x18)   : 60 bytes
        uint32_t            acPpkAlignment;          // (0x54)   : 12 bytes padding to 16-byte align
        --- SPK Header  
        HeaderAuthCert_versal_2vp spkheader;         // (0x60)   : 24 bytes (6 * 4 bytes)
        --- SPK
        HssPublicKey        acSpk;                   // (0x78)   : 60 bytes
        --- SPK Signature Length
        uint32_t            acSpkSignatureLength;    // (0xB4)   : 4 bytes
        uint8_t             acSpkSignatureLengthAlignment[12]; // 12 bytes padding
        --- SPK Signature
        HssSignature        acSpkSignature;          // (0xC4)   : variable bytes
        --- Hashblock Signature Length
        uint32_t            acHashblockSignatureLength; // 4 bytes  
        uint8_t             acHashblockSignatureLengthAlignment[12]; // 12 bytes padding
        --- Hash Block Signature : This is placed after Hash Block
        //HssSignature        acHashBlockSignature;    // (??)     : variable bytes (separate, not part of AC)
        */

        // PPK header size
        size_t ppkHeaderLength = sizeof(HeaderAuthCert_versal_2vp);
        
        // Get lmsOnly flag from primaryKey (or secondaryKey) to determine HSS vs LMS mode
        bool lmsOnlyForSize = true;  // Default to LMS (single-level)
        if (primaryKey != nullptr) {
            lmsOnlyForSize = primaryKey->lmsOnly;
        } else if (secondaryKey != nullptr) {
            lmsOnlyForSize = secondaryKey->lmsOnly;
        }
        
        // PPK size with padding
        size_t ppkLength = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnlyForSize);
        ppkLength += PADDING_16B(ppkLength);
        
        // SPK header size  
        size_t spkHeaderLength = sizeof(HeaderAuthCert_versal_2vp);
        
        // SPK size with padding
        size_t spkLength = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnlyForSize);
        spkLength += PADDING_16B(spkLength);
        
        // Calculate OLD size (without signature length fields)
        size_t spkSignLength = GetLmsSignLength(pskFile.c_str(), lmsOnlyForSize);
        spkSignLength += PADDING_16B(spkSignLength);
        size_t hashblockSignLength = GetTotalHashBlockSignSize();
        
        // SPK signature length field with padding
        size_t spkSignLengthField = sizeof(uint32_t) + 12; // 4 bytes + 12 bytes padding
        
        // Hashblock signature length field with padding
        size_t hashblockSignLengthField = sizeof(uint32_t) + 12; // 4 bytes + 12 bytes padding
        
        size_t newSize = ppkHeaderLength + ppkLength + spkHeaderLength + spkLength + 
                        spkSignLengthField + spkSignLength + hashblockSignLengthField + 
                        hashblockSignLength;
        
        return newSize;
    }
    else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
    {
        /* 
        New versal_2vp AC Structure for SLH-DSA:
        --- PPK Header
        HeaderAuthCert_versal_2vp ppkheader;         // (0x00)   : 24 bytes (6 * 4 bytes)
        --- PPK
        uint8_t             acPpk[32];               // (0x18)   : 32 bytes
        uint8_t             acPpkAlignment[8];       // (0x38)   : 8 bytes padding to 16-byte align
        --- SPK Header  
        HeaderAuthCert_versal_2vp spkheader;         // (0x40)   : 24 bytes (6 * 4 bytes)
        --- SPK
        uint8_t             acSpk[32];               // (0x58)   : 32 bytes
        uint8_t             acSpkAlignment[8];       // (0x78)   : 8 bytes padding to 16-byte align
        --- SPK Signature Length
        uint32_t            acSpkSignatureLength;    // (0x80)   : 4 bytes
        uint8_t             acSpkSignatureLengthAlignment[12]; // 12 bytes padding
        --- SPK Signature
        uint8_t             acSpkSignature[29792];   // (0x90)   : 29792 bytes (already 16-byte aligned)
        --- Hashblock Signature Length
        uint32_t            acHashblockSignatureLength; // 4 bytes  
        uint8_t             acHashblockSignatureLengthAlignment[12]; // 12 bytes padding
        --- Hash Block Signature : This is placed after Hash Block
        //uint8_t             acHashBlockSignature[29792]; // 29792 bytes (separate, not part of AC)
        */

        // Get SLH-DSA parameters
        const slh_param_t *slh_params = &slh_dsa_shake_256s;

        // PPK header size
        size_t ppkHeaderLength = sizeof(HeaderAuthCert_versal_2vp);
        
        // PPK size with padding
        size_t ppkLength = slh_pk_sz(slh_params);
        ppkLength += PADDING_16B(ppkLength);
        
        // SPK header size  
        size_t spkHeaderLength = sizeof(HeaderAuthCert_versal_2vp);
        
        // SPK size with padding
        size_t spkLength = slh_pk_sz(slh_params);
        spkLength += PADDING_16B(spkLength);
        
        // Calculate OLD size (without signature length fields)
        size_t spkSignLength = slh_sig_sz(slh_params);
        spkSignLength += PADDING_16B(spkSignLength);
        size_t hashblockSignLength = GetTotalHashBlockSignSize();
        
        size_t oldSize = ppkHeaderLength + ppkLength + spkHeaderLength + spkLength + 
                        spkSignLength + hashblockSignLength;
        LOG_INFO("SLH-DSA AC Size BEFORE adding signature length fields: %zu bytes", oldSize);
        
        // SPK signature length field with padding
        size_t spkSignLengthField = sizeof(uint32_t) + 12; // 4 bytes + 12 bytes padding
        
        // Hashblock signature length field with padding
        size_t hashblockSignLengthField = sizeof(uint32_t) + 12; // 4 bytes + 12 bytes padding
        
        size_t newSize = ppkHeaderLength + ppkLength + spkHeaderLength + spkLength + 
                        spkSignLengthField + spkSignLength + hashblockSignLengthField + 
                        hashblockSignLength;

        return newSize;
    }
    else
    {
        return certSize;
    }
}

/******************************************************************************/
uint32_t Versal_2vpAuthenticationContext::GetTotalHashBlockSignSize(void)
{
    // Handle LMS/HSS algorithms - they have variable signature lengths
    // Note: HSS uses same type (LMS_SHAKE256/LMS_SHA2_256) with lmsOnly=false flag
    if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256) || 
        (authAlgorithm->Type() == Authentication::LMS_SHAKE256))
    {
        // Get lmsOnly flag from secondaryKey (or primaryKey) to determine HSS vs LMS mode
        bool lmsOnlyForHashBlock = true;  // Default to LMS (single-level)
        if (secondaryKey != nullptr) {
            lmsOnlyForHashBlock = secondaryKey->lmsOnly;
        } else if (primaryKey != nullptr) {
            lmsOnlyForHashBlock = primaryKey->lmsOnly;
        }
        
        size_t totalHashBlockSignatureLength = GetLmsSignLength(sskFile.c_str(), lmsOnlyForHashBlock);
        totalHashBlockSignatureLength += PADDING_16B(totalHashBlockSignatureLength);
        
        return totalHashBlockSignatureLength;
    }
    else
    {
        return signatureLength;
    }
}

/******************************************************************************/
Versal_2vpAuthenticationContext::~Versal_2vpAuthenticationContext()
{
}

/******************************************************************************/
ECDSAHBAuthenticationAlgorithm_versal_2vp::ECDSAHBAuthenticationAlgorithm_versal_2vp()
{
    certSize = sizeof(AuthCertificateECDSAHBStructure_versal2vp);
    authType = Authentication::ECDSA;
}

/******************************************************************************/
ECDSAHBAuthenticationAlgorithm_versal_2vp::~ECDSAHBAuthenticationAlgorithm_versal_2vp()
{
}

/******************************************************************************/
ECDSAP521HBAuthenticationAlgorithm_versal_2vp::ECDSAP521HBAuthenticationAlgorithm_versal_2vp()
{
    certSize = sizeof(AuthCertificateECDSAp521HBStructure_versal2vp);
    authType = Authentication::ECDSAp521;
}

/******************************************************************************/
ECDSAP521HBAuthenticationAlgorithm_versal_2vp::~ECDSAP521HBAuthenticationAlgorithm_versal_2vp()
{
}

/******************************************************************************/
RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp::RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp()
{
    certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure_versal2vp);
    authType = Authentication::RSA;
}

/******************************************************************************/
RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp::~RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp()
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
uint8_t* RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp::AttachSHA3Padding(uint8_t* data, const Binary::Length_t datalength)
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
int RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp::MaskGenerationFunction(unsigned char *mask, long len, const unsigned char *seed, long seedlen, const EVP_MD *dgst)
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
            Versal_2vpcrypto_hash(mask + outlen, cSha3Pad, 104, false);
            outlen += mdlen;
        }
        else
        {
            Versal_2vpcrypto_hash(md, cSha3Pad, 104, false);
            memcpy(mask + outlen, md, len - outlen);
            outlen = len;
        }
        delete[] cSha3Pad;
        cSha3Pad = NULL;
    }
    rv = 0;
    delete[] c;
err:
    return rv;
}

/******************************************************************************/
PartitionBifOptions* Versal_2vpAuthenticationContext::FindOwningPartitionBifOptions(BootImage& bi, Section* dataSection)
{
    PartitionBifOptions* owner = nullptr;
    auto keyFilesMatch = [this](const PartitionBifOptions* p) -> bool {
        bool anyMatch = false;
        auto cmp = [&](const std::string& a, const std::string& b) -> bool {
            if (!a.empty() && !b.empty()) {
                if (a != b) return false;   // conflicting key -> not this partition
                anyMatch = true;
            }
            return true;
        };
        if (!cmp(this->ppkFile, p->ppkFile)) return false;
        if (!cmp(this->spkFile, p->spkFile)) return false;
        if (!cmp(this->pskFile, p->pskFile)) return false;
        if (!cmp(this->sskFile, p->sskFile)) return false;
        return anyMatch;
    };
    for (std::list<ImageBifOptions*>::iterator imgIt = bi.bifOptions->imageBifOptionList.begin();
         imgIt != bi.bifOptions->imageBifOptionList.end() && owner == nullptr; ++imgIt) {
        for (std::list<PartitionBifOptions*>::iterator partIt = (*imgIt)->partitionBifOptionsList.begin();
             partIt != (*imgIt)->partitionBifOptionsList.end(); ++partIt) {
            if (keyFilesMatch(*partIt)) {
                owner = *partIt;
                LOG_TRACE("Found PartitionBifOptions for metadata retrieval by key-file match");
                break;
            }
        }
    }

    // Fallback: locate by section index (single-key / legacy behavior)
    if (dataSection != nullptr) {
        size_t partIndexForFlags = 0;
        for (std::list<ImageBifOptions*>::iterator imgIt = bi.bifOptions->imageBifOptionList.begin();
             imgIt != bi.bifOptions->imageBifOptionList.end() && owner == nullptr; ++imgIt) {
            for (std::list<PartitionBifOptions*>::iterator partIt = (*imgIt)->partitionBifOptionsList.begin();
                 partIt != (*imgIt)->partitionBifOptionsList.end(); ++partIt, ++partIndexForFlags) {
                if (partIndexForFlags == static_cast<size_t>(dataSection->index)) {
                    owner = *partIt;
                    LOG_TRACE("Partition[%zu]: Found PartitionBifOptions for metadata retrieval", partIndexForFlags);
                    break;
                }
            }
        }

        // Fallback to legacy BIF format
        if (owner == nullptr && static_cast<size_t>(dataSection->index) < bi.bifOptions->partitionBifOptionList.size()) {
            auto partBifIt = bi.bifOptions->partitionBifOptionList.begin();
            std::advance(partBifIt, dataSection->index);
            if (partBifIt != bi.bifOptions->partitionBifOptionList.end()) {
                owner = *partBifIt;
                LOG_TRACE("Partition[%zu]: Found PartitionBifOptions using legacy BIF format",
                          static_cast<size_t>(dataSection->index));
            }
        }
    }

    return owner;
}

/******************************************************************************/
Section* Versal_2vpAuthenticationContext::CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection, bool isBootloader)
{
    LOG_INFO("Creating Authentication Certificate for section - %s", dataSection->Name.c_str());

    SetCurrentDataSection(dataSection);
    /* PM-TODO - Check this and update */
    if ( (authAlgorithm->Type() != Authentication::MLDSA) && (authAlgorithm->Type() != Authentication::LMS_SHA2_256) && 
         (authAlgorithm->Type() != Authentication::LMS_SHAKE256) && (authAlgorithm->Type() != Authentication::SLH_SHAKE256) )
    {
        hashType = bi.GetAuthHashAlgo();
        if (ownsHash && hash != nullptr) {
            delete hash;
        }
        hash = bi.hash.get();
        ownsHash = false;
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
    
    Section* acSection = new Section(name + hashExtension, certSize);
    acSection->isCertificate = true;
    acSection->index = dataSection->index;
    cache.Sections.push_back(std::unique_ptr<Section>(acSection));
    uint8_t* authCert = acSection->Data.get();
    LOG_TRACE("Creating new section for certificate - %s", acSection->Name.c_str());

    uint32_t x = sizeof(AuthCertificate4096Sha3PaddingHBStructure_versal2vp);
    if (authAlgorithm->Type() == Authentication::RSA)
    {
        x = certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure_versal2vp);
    }
    else if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        x = certSize = sizeof(AuthCertificateECDSAHBStructure_versal2vp);
    }
    else if (authAlgorithm->Type() == Authentication::ECDSAp521)
    {
        x = certSize = sizeof(AuthCertificateECDSAp521HBStructure_versal2vp);
    }
    else if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256) || (authAlgorithm->Type() == Authentication::LMS_SHAKE256) || (authAlgorithm->Type() == Authentication::SLH_SHAKE256))
    {
        x = certSize = GetCertificateSize();
    }
    else if (authAlgorithm->Type() == Authentication::MLDSA)
    {
        x = certSize = GetCertificateSize();  
    }

    if (x != certSize)
    {
        LOG_DEBUG(DEBUG_STAMP, "Bad Authentication Certificate Size");
        LOG_ERROR("Authentication Error !!!");
    }
    memset(authCert, 0, certSize);

    // Retrieve .kmd metadata for the owning partition. The same helper is used by
    // CreateSPKSignature so the emitted and signed SPK/PPK headers cannot diverge.
    KeyMetadata primaryMeta, secondaryMeta;
    PartitionBifOptions* partBifOptionsForFlags = FindOwningPartitionBifOptions(bi, dataSection);

    if (partBifOptionsForFlags != nullptr) {
        primaryMeta = partBifOptionsForFlags->primaryMetadata;
        secondaryMeta = partBifOptionsForFlags->secondaryMetadata;
        LOG_TRACE("Retrieved .kmd metadata - primaryMeta.isValid=%d, secondaryMeta.isValid=%d",
                  primaryMeta.isValid, secondaryMeta.isValid);
    } else {
        LOG_TRACE("No PartitionBifOptions available, using default values");
        primaryMeta.isValid = false;
        secondaryMeta.isValid = false;
    }

    LOG_TRACE("Authentication Certificate size1****** - %x %x bytes", certSize,x);
    uint32_t acHdr = authAlgorithm->GetAuthHeader();
  
    // Remove PPK header from being updated in boot header as it moved to AC for versal_2vp
    uint8_t* headerData = bi.bootHeader->section->Data.get();
   /*
    uint32_t authHeader1Offset = BH_AC_HEADER_OFFSET_V2;
    uint32_t totalppkkSize1Offset = BH_TOTAL_PPK_SIZE1_OFFSET_V2;
    uint32_t actualppkSize1Offset = BH_ACTUAL_PPK_SIZE1_OFFSET_V2;
    uint32_t totalHashBlockSignatureSize1Offset = BH_TOTAL_SIGN_SIZE1_OFFSET_V2;
    uint32_t actualSignatureSize1Offset = BH_ACTUAL_SIGN_SIZE1_OFFSET_V2;
  */
    
    if (name == "MetaHeader")
    {
         // Remove PPK header from being updated in boot header as it moved to AC for versal_2vp
    
        headerData = bi.imageHeaderTable->section->Data.get();
     /*   authHeader1Offset = IHT_AC_HEADER_OFFSET;
        totalppkkSize1Offset = IHT_TOTAL_PPK_SIZE1_OFFSET;
        actualppkSize1Offset = IHT_ACTUAL_PPK_SIZE1_OFFSET;
        totalHashBlockSignatureSize1Offset = IHT_TOTAL_SIGN_SIZE1_OFFSET;
        actualSignatureSize1Offset = IHT_ACTUAL_SIGN_SIZE1_OFFSET;
        */
        
    }
    
    if (udfFile != "")
    {
        LoadUdfData(udfFile, udf_data);
        authAlgorithm->RearrangeEndianess(udf_data, sizeof(udf_data));
        memcpy(headerData + BH_UDF_OFFSET_TELLURIDE, udf_data, UDF_BH_VERSAL_2VP);
    }

   // WriteLittleEndian32(headerData + authHeader1Offset, acHdr);
    if (authAlgorithm->Type() == Authentication::RSA)
    {
        // // Remove PPK header from being updated in boot header as it moved to AC for versal_2vp
      /*  WriteLittleEndian32(headerData + totalppkkSize1Offset, RSA_4096_N_SIZE + RSA_4096_N_EXT_SIZE + RSA_4096_E_SIZE + TELLURIDE_RSA_AC_PPK_SPK_ALIGNMENT);
        WriteLittleEndian32(headerData + actualppkSize1Offset, RSA_4096_N_SIZE + RSA_4096_N_EXT_SIZE + RSA_4096_E_SIZE);
        WriteLittleEndian32(headerData + totalHashBlockSignatureSize1Offset, signatureLength);
        WriteLittleEndian32(headerData + actualSignatureSize1Offset, signatureLength);
        */

       // Telluride format way of updating AC   
        /*
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
        */


        // Cast the buffer to your new structure
        AuthCertificate4096Sha3PaddingHBStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificate4096Sha3PaddingHBStructure_versal2vp*>(authCert);

        // Fill PPK header using .kmd metadata (if available) or defaults (backward compatibility)
        PopulateHeaderFromKmdMetadata(
            &certStruct->ppkheader,
            primaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(),  // default algorithm
            0x0,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x0   // default permission
        );

        // Export PPK key
        primaryKey->Export(reinterpret_cast<uint8_t*>(&certStruct->acPpk));

        // Rearrange endianess for each RSA field inside the structure for PPK 
        authAlgorithm->RearrangeEndianess(certStruct->acPpk.N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acPpk.N_extension, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acPpk.E, RSA_4096_E_SIZE);

        // Optionally set alignment if needed
        memset(certStruct->acPpkAlignment, 0, sizeof(certStruct->acPpkAlignment));

        // Fill SPK header using .kmd metadata (if available) or defaults (backward compatibility)
        PopulateHeaderFromKmdMetadata(
            &certStruct->spkheader,
            secondaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(),  // default algorithm
            spkIdentification,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x1  // default permission
        );

        // Export SPK key
        secondaryKey->Export(reinterpret_cast<uint8_t*>(&certStruct->acSpk));

        // Optionally set alignment if needed
        memset(certStruct->acSpkAlignment, 0, sizeof(certStruct->acSpkAlignment));

        // Rearrange endianess for each RSA field inside the structure for SPK
        authAlgorithm->RearrangeEndianess(certStruct->acSpk.N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acSpk.N_extension, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acSpk.E, RSA_4096_E_SIZE);

        // Set SPK signature length
        certStruct->acSpkSignatureLength = RSA_4096_KEY_LENGTH;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));

        // Copy SPK signature
        CopySPKSignature(bi, reinterpret_cast<uint8_t*>(&certStruct->acSpkSignature));
        
        // Set hashblock signature length
        certStruct->acHashblockSignatureLength = RSA_4096_KEY_LENGTH;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }
    else if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        /*
        WriteLittleEndian32(headerData + totalppkkSize1Offset, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(headerData + actualppkSize1Offset, EC_P384_KEY_LENGTH * 2);

        WriteLittleEndian32(headerData + totalHashBlockSignatureSize1Offset, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(headerData + actualSignatureSize1Offset, EC_P384_KEY_LENGTH * 2);

        primaryKey->Export(authCert + TELLURIDE_EC_P384_AC_PPK_OFFSET);

        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_TOTAL_SPK_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_ACTUAL_SPK_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_TOTAL_SPK_SIGN_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_ACTUAL_SPK_SIGN_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_SPK_ID_OFFSET, spkIdentification);

        secondaryKey->Export(authCert + TELLURIDE_EC_P384_AC_SPK_OFFSET);

        CopySPKSignature(bi, authCert + TELLURIDE_EC_P384_AC_SPK_SIGN_OFFSET);
        */

        // Cast the buffer to your new structure
        AuthCertificateECDSAHBStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificateECDSAHBStructure_versal2vp*>(authCert);

        // Fill PPK header using .kmd metadata (if available) or defaults (backward compatibility)
        PopulateHeaderFromKmdMetadata(
            &certStruct->ppkheader,
            primaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(),  // default algorithm
            0x0,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x0   // default permission
        );

        // Export PPK key
        primaryKey->Export(reinterpret_cast<uint8_t*>(&certStruct->acPpk));

        // Fill SPK header using .kmd metadata (if available) or defaults (backward compatibility)
        PopulateHeaderFromKmdMetadata(
            &certStruct->spkheader,
            secondaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(),  // default algorithm
            spkIdentification,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x1  // default permission
        );

        // Export SPK key
        secondaryKey->Export(reinterpret_cast<uint8_t*>(&certStruct->acSpk));

        // Set SPK signature length
        certStruct->acSpkSignatureLength = EC_P384_KEY_LENGTH * 2;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));

        // Copy SPK signature
        CopySPKSignature(bi, reinterpret_cast<uint8_t*>(&certStruct->acSpkSignature));
        
        // Set hashblock signature length
        certStruct->acHashblockSignatureLength = EC_P384_KEY_LENGTH * 2;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }
    else if (authAlgorithm->Type() == Authentication::ECDSAp521)
    {
        /*
        //Update to ECDSAp521
        WriteLittleEndian32(headerData + totalppkkSize1Offset, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(headerData + actualppkSize1Offset, EC_P384_KEY_LENGTH * 2);

        WriteLittleEndian32(headerData + totalHashBlockSignatureSize1Offset, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(headerData + actualSignatureSize1Offset, EC_P384_KEY_LENGTH * 2);

        primaryKey->Export(authCert + TELLURIDE_EC_P384_AC_PPK_OFFSET);

        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_TOTAL_SPK_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_ACTUAL_SPK_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_TOTAL_SPK_SIGN_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_ACTUAL_SPK_SIGN_SIZE_OFFSET, EC_P384_KEY_LENGTH * 2);
        WriteLittleEndian32(authCert + TELLURIDE_EC_P384_AC_SPK_ID_OFFSET, spkIdentification);

        secondaryKey->Export(authCert + TELLURIDE_EC_P384_AC_SPK_OFFSET);

        CopySPKSignature(bi, authCert + TELLURIDE_EC_P384_AC_SPK_SIGN_OFFSET);
        */
       // Cast the buffer to your new structure
        AuthCertificateECDSAp521HBStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificateECDSAp521HBStructure_versal2vp*>(authCert);

        // Fill PPK header using .kmd metadata (if available) or defaults (backward compatibility)
        PopulateHeaderFromKmdMetadata(
            &certStruct->ppkheader,
            primaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(),  // default algorithm
            0x0,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x0   // default permission
        );

        // Export PPK key
        primaryKey->Export(reinterpret_cast<uint8_t*>(&certStruct->acPpk));

        // Optionally set alignment if needed
        memset(certStruct->acPpkAlignment, 0, sizeof(certStruct->acPpkAlignment));

        // Fill SPK header using .kmd metadata (if available) or defaults (backward compatibility)
        PopulateHeaderFromKmdMetadata(
            &certStruct->spkheader,
            secondaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(),  // default algorithm
            spkIdentification,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x1  // default permission
        );
        certStruct->spkheader.Permission   = 0x1; // BOOT or Others
        memset(certStruct->spkheader.Reserved, 0, sizeof(certStruct->spkheader.Reserved));

        // Export SPK key
        secondaryKey->Export(reinterpret_cast<uint8_t*>(&certStruct->acSpk));

        // Set SPK signature length
        certStruct->acSpkSignatureLength = EC_P521_KEY_LENGTH2 * 2;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));

        // Copy SPK signature
        CopySPKSignature(bi, reinterpret_cast<uint8_t*>(&certStruct->acSpkSignature));

        // Optionally set alignment if needed
        memset(certStruct->acSpkAlignment, 0, sizeof(certStruct->acSpkAlignment));
        
        // Set hashblock signature length
        certStruct->acHashblockSignatureLength = EC_P521_KEY_LENGTH2 * 2;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }
    else if (authAlgorithm->Type() == Authentication::MLDSA)
    {
	  #if 0
        WriteLittleEndian32(headerData + totalppkkSize1Offset, MLDSA_PUB_KEY_LENGTH);
        WriteLittleEndian32(headerData + actualppkSize1Offset, MLDSA_PUB_KEY_LENGTH);

        WriteLittleEndian32(headerData + totalHashBlockSignatureSize1Offset, MLDSA_TOTAL_SIGN_LEN);
        WriteLittleEndian32(headerData + actualSignatureSize1Offset, MLDSA_ACTUAL_SIGN_LEN);

        primaryKey->Export(authCert + TELLURIDE_MLDSA_AC_PPK_OFFSET);

        WriteLittleEndian32(authCert + TELLURIDE_MLDSA_AC_TOTAL_SPK_SIZE_OFFSET, MLDSA_PUB_KEY_LENGTH);
        WriteLittleEndian32(authCert + TELLURIDE_MLDSA_AC_ACTUAL_SPK_SIZE_OFFSET, MLDSA_PUB_KEY_LENGTH);
        WriteLittleEndian32(authCert + TELLURIDE_MLDSA_AC_TOTAL_SPK_SIGN_SIZE_OFFSET, MLDSA_TOTAL_SIGN_LEN);
        WriteLittleEndian32(authCert + TELLURIDE_MLDSA_AC_ACTUAL_SPK_SIGN_SIZE_OFFSET, MLDSA_ACTUAL_SIGN_LEN);
        WriteLittleEndian32(authCert + TELLURIDE_MLDSA_AC_SPK_ID_OFFSET, spkIdentification);

        secondaryKey->Export(authCert + TELLURIDE_MLDSA_AC_SPK_OFFSET);

        CopySPKSignature(bi, authCert + TELLURIDE_MLDSA_AC_SPK_SIGN_OFFSET);
		
		#endif
        AuthCertificateMLDSAStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificateMLDSAStructure_versal2vp*>(authCert);

        // Fill PPK header using .kmd metadata (if available) or defaults (backward compatibility)
        memset(&certStruct->ppkheader, 0, sizeof(HeaderAuthCert_versal_2vp));
        PopulateHeaderFromKmdMetadata(
            &certStruct->ppkheader,
            primaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(),  // default algorithm
            0x0,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x0   // default permission
        );

        // Export PPK key
        primaryKey->Export(certStruct->acPpk);

        // Fill SPK header using .kmd metadata (if available) or defaults (backward compatibility)
        memset(&certStruct->spkheader, 0, sizeof(HeaderAuthCert_versal_2vp));
        PopulateHeaderFromKmdMetadata(
            &certStruct->spkheader,
            secondaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(),  // default algorithm
            spkIdentification,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x1  // default permission
        );

        // Export SPK key
        secondaryKey->Export(certStruct->acSpk);

        // Set SPK signature length
        certStruct->acSpkSignatureLength = MLDSA_TOTAL_SIGN_LEN;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));

        // Copy SPK signature
        CopySPKSignature(bi, certStruct->acSpkSignature);
        
        // Set hashblock signature length  
        certStruct->acHashblockSignatureLength = MLDSA_TOTAL_SIGN_LEN;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
		
    }
    else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
    {

        AuthCertificateSLHDSAStructure_versal2vp* certStruct =
        reinterpret_cast<AuthCertificateSLHDSAStructure_versal2vp*>(authCert);

        // Fill PPK header using .kmd metadata (if available) or defaults (backward compatibility)
        memset(&certStruct->ppkheader, 0, sizeof(HeaderAuthCert_versal_2vp));
        PopulateHeaderFromKmdMetadata(
            &certStruct->ppkheader,
            primaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(),  // default algorithm
            0x0,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x0   // default permission
        );

        // Export PPK key
        primaryKey->Export(certStruct->acPpk);

        // Fill SPK header using .kmd metadata (if available) or defaults (backward compatibility)
        memset(&certStruct->spkheader, 0, sizeof(HeaderAuthCert_versal_2vp));
        PopulateHeaderFromKmdMetadata(
            &certStruct->spkheader,
            secondaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(),  // default algorithm
            spkIdentification,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x1  // default permission
        );

        // Export SPK key
        secondaryKey->Export(certStruct->acSpk);

        // Set SPK signature length
        certStruct->acSpkSignatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));

        // Copy SPK signature
        CopySPKSignature(bi, certStruct->acSpkSignature);
        
        // Set hashblock signature length
        certStruct->acHashblockSignatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }	
    else 
    {
        // LMS_SHA256 || LMS_SHAKE256
        /*
        acHdr = authAlgorithm->GetAuthHeader(lmsOnly, false, bi.options.IsDl9Series());

		size_t actualSpkSignatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);
		size_t actualHashBlockSignatureLength = GetLmsSignLength(sskFile.c_str(), lmsOnly);
        size_t actualLmsPpkSize1 = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly);
        size_t actualLmsSpkSize1 = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);

        WriteLittleEndian32(headerData + authHeader1Offset, acHdr);
        WriteLittleEndian32(headerData + totalppkkSize1Offset, actualLmsPpkSize1 + PADDING_16B(actualLmsPpkSize1));
        WriteLittleEndian32(headerData + actualppkSize1Offset, actualLmsPpkSize1);
        WriteLittleEndian32(headerData + totalHashBlockSignatureSize1Offset, actualHashBlockSignatureLength + PADDING_16B(actualHashBlockSignatureLength));
        WriteLittleEndian32(headerData + actualSignatureSize1Offset, actualHashBlockSignatureLength);
    

        primaryKey->Export(authCert + TELLURIDE_LMS_AC_PPK_OFFSET);
        WriteLittleEndian32(authCert + TELLURIDE_LMS_AC_TOTAL_SPK_SIZE_OFFSET(lmsOnly), actualLmsSpkSize1 + PADDING_16B(actualLmsSpkSize1));
        WriteLittleEndian32(authCert + TELLURIDE_LMS_AC_ACTUAL_SPK_SIZE_OFFSET(lmsOnly), actualLmsSpkSize1);
        WriteLittleEndian32(authCert + TELLURIDE_LMS_AC_TOTAL_SPK_SIGN_SIZE_OFFSET(lmsOnly), actualSpkSignatureLength + PADDING_16B(actualSpkSignatureLength));
        WriteLittleEndian32(authCert + TELLURIDE_LMS_AC_ACTUAL_SPK_SIGN_SIZE_OFFSET(lmsOnly), actualSpkSignatureLength);

        WriteLittleEndian32(authCert + TELLURIDE_LMS_AC_SPK_ID_OFFSET(lmsOnly), spkIdentification);

        secondaryKey->Export(authCert + TELLURIDE_LMS_AC_SPK_OFFSET(lmsOnly));
        CopySPKSignature(bi, authCert + TELLURIDE_LMS_AC_SPK_SIGN_OFFSET(lmsOnly));
        */
        HeaderAuthCert_versal_2vp ppkHeader, spkHeader;
        memset(&ppkHeader, 0, sizeof(ppkHeader));
        memset(&spkHeader, 0, sizeof(spkHeader));

        // Fill PPK header using .kmd metadata (if available) or defaults (backward compatibility)
        PopulateHeaderFromKmdMetadata(
            &ppkHeader,
            primaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(lmsOnly, false, bi.options.IsDl9Series()),  // default algorithm
            0x0,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x0   // default permission
        );

        // Fill SPK header using .kmd metadata (if available) or defaults (backward compatibility)
        PopulateHeaderFromKmdMetadata(
            &spkHeader,
            secondaryMeta,  // .kmd metadata for single signing
            authAlgorithm->GetAuthHeader(lmsOnly, false, bi.options.IsDl9Series()),  // default algorithm
            spkIdentification,  // default revocation ID
            0x0,  // default hybrid
            0x0,  // default authority
            0x1  // default permission
        );

        // Now copy these headers into your authCert buffer at the correct offsets
        uint8_t* ptr = authCert;
        
        // Get signature lengths for the new versal_2vp format fields
        size_t actualSpkSignatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);
        size_t actualHashBlockSignatureLength = GetLmsSignLength(sskFile.c_str(), lmsOnly);
        
        // Calculate total lengths (actual + padding) for consistency with other algorithms
        size_t totalSpkSignatureLength = actualSpkSignatureLength + PADDING_16B(actualSpkSignatureLength);
        size_t totalHashBlockSignatureLength = actualHashBlockSignatureLength + PADDING_16B(actualHashBlockSignatureLength);

        LOG_TRACE("for LMS Total SPK Signature Length: %x, Total Hash Block Signature Length: %x",
                  totalSpkSignatureLength, totalHashBlockSignatureLength);

        // 1. Write PPK header
        memcpy(ptr, &ppkHeader, sizeof(HeaderAuthCert_versal_2vp));
        ptr += sizeof(HeaderAuthCert_versal_2vp);

        // 2. Write PPK key
        primaryKey->Export(ptr);
        size_t ppkLen = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly);
        size_t ppkPad = PADDING_16B(ppkLen); //lms-hss key 16 bytes aligned
        ptr += ppkLen;
        if (ppkPad) {
            memset(ptr, 0, ppkPad);
            ptr += ppkPad;
        }
        // 3. Write SPK header
        memcpy(ptr, &spkHeader, sizeof(HeaderAuthCert_versal_2vp));
        ptr += sizeof(HeaderAuthCert_versal_2vp);

        // 4. Write SPK key
        secondaryKey->Export(ptr);
        size_t spkLen = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);
        size_t spkPad = PADDING_16B(spkLen); //lms-hss key 16 bytes aligned
        ptr += spkLen;
        if (spkPad) {
            memset(ptr, 0, spkPad);
            ptr += spkPad;
        }
        
        // 5. Write SPK signature length (new versal_2vp field)
        WriteLittleEndian32(ptr, totalSpkSignatureLength);
        ptr += sizeof(uint32_t);
        
        // Add 16-byte alignment padding for SPK signature length field
        size_t spkSigLenPad = 12; // 4 bytes length + 12 bytes padding = 16 bytes aligned
        memset(ptr, 0, spkSigLenPad);
        ptr += spkSigLenPad;
        
        // 6. Write SPK signature
        CopySPKSignature(bi, ptr);
        size_t spkSigLen = actualSpkSignatureLength;
        size_t spkSigPad = PADDING_16B(spkSigLen);
        ptr += spkSigLen;
        if (spkSigPad) {
            memset(ptr, 0, spkSigPad);
            ptr += spkSigPad;
        }
        
        // 7. Write hash block signature length (new versal_2vp field)
        WriteLittleEndian32(ptr, totalHashBlockSignatureLength);
        ptr += sizeof(uint32_t);
        
        // Add 16-byte alignment padding for hash block signature length field
        size_t hashSigLenPad = 12; // 4 bytes length + 12 bytes padding = 16 bytes aligned
        memset(ptr, 0, hashSigLenPad);
        ptr += hashSigLenPad;
        
        // 8. Reserve space for hash block signature (will be filled during Link phase)
        // The actual signature is written to this location during CopyPartitionSignature
        // Store the pointer offset for later use during Link
        size_t hashSigLen = actualHashBlockSignatureLength;
        size_t hashSigPad = PADDING_16B(hashSigLen);
        LOG_TRACE("LMS AC: Reserving %zu bytes for hashblock signature at AC offset 0x%lx", 
                 hashSigLen + hashSigPad, (unsigned long)(ptr - authCert));
        memset(ptr, 0, hashSigLen + hashSigPad);  // Initialize to zeros, will be filled during Link
        ptr += hashSigLen + hashSigPad;
    }

    certIndex++;
    return acSection;
}
/*******************************************************************************/
bool Versal_2vpAuthenticationContext::IsSectionEncrypted(BootImage& bi, Section* section)
{
    for (auto& p : bi.partitionHeaderList) {
        if (p->partition->section == section) {
            return p->encryptFlag;
        }
    }
    return false;
}

/*******************************************************************************/
static void BuildMetaheaderBufferForHash(BootImage& bi, uint8_t*& buffer, size_t& bufferSize) {
    std::list<Section*> sections;
    size_t size = 0;
    ImageHeaderTable* iHT = bi.imageHeaderTable.get();

    // Always add the image header table section
    sections.push_back(iHT->section);
    size += iHT->section->Length;

    if (bi.options.bifOptions->GetHeaderEncyption()) {
        // If encrypted, add the encrypted metaheader section
        sections.push_back(bi.encryptedHeaders);
        size += bi.encryptedHeaders->Length;
    } else {
        // Otherwise, add all image and partition headers
        for (auto& image : bi.subSysImageList) {
            sections.push_back(image->section);
            size += sizeof(VersalImageHeaderStructure);
        }
        for (auto& part : bi.partitionHeaderList) {
            sections.push_back(part->section);
            size += sizeof(Versal_2vpPartitionHeaderTableStructure);
        }
    }

    buffer = new uint8_t[size];
    memset(buffer, bi.options.GetOutputFillByte(), size);

    Binary::Address_t start = sections.front()->Address;
    for (auto& section : sections) {
        int offset = section->Address - start;
        memcpy(buffer + offset, section->Data.get(), section->Length);
    }
    bufferSize = size;
}

/*******************************************************************************/
void PopulateBootloaderHashforBHandMetaheader(BootImage& bi, Section* bootloaderSection, Section* hashBlockSection, ImageHeader* imageHeader, Section* metaSection)
{
    size_t hashLen = bi.hash->GetHashLength();
    uint8_t* hashBlock = hashBlockSection->Data.get();
    size_t offset = 0;

     // 1. BH hash (index 0) -- MATCHING THE LOGIC IN Link()
    uint8_t* bhData = bi.bootHeader->section->Data.get() + 0x10;
    uint8_t bhHash[SHA3_LENGTH_BYTES] = {0};
    bi.hash->CalculateVersalHash(true, bhData, bi.bootHeader->GetBootHeaderSize() - sizeof(Versal_2vpSmapWidthTable), bhHash);
    uint32_t bhIndex = HASH_BLOCK_BH_HASH_INDEX;
    memcpy(hashBlock + offset, &bhIndex, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(hashBlock + offset, bhHash, hashLen);
    offset += hashLen;
    LOG_TRACE("BH hash added to hashblock0.");
    LOG_DUMP_BYTES(bhHash, hashLen);


    // 2. Skip PLM hash (index 1) as it already got populated in parition-versal.cpp Build fun
    offset += sizeof(uint32_t) + hashLen;
    // 3. Skip PMC hash (index 2) as it already got populated in parition-versal.cpp Build fun
    offset += sizeof(uint32_t) + hashLen;

    // 4. Metaheader hash (index 3, if present)
    if (metaSection)
    {
        uint8_t* headersBuffer = nullptr;
        size_t metaheaderSize = 0;
        BuildMetaheaderBufferForHash(bi, headersBuffer, metaheaderSize);

        uint8_t metaHash[SHA3_LENGTH_BYTES] = {0};
        bi.hash->CalculateVersalHash(true, headersBuffer, metaheaderSize, metaHash);

        LOG_INFO("Final metaheader hash (combined headers):");
        LOG_DUMP_BYTES(metaHash, hashLen);

        uint32_t metaIndex = HASH_BLOCK_HASHBLOCK1_HASH_INDEX;
        memcpy(hashBlock + offset, &metaIndex, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(hashBlock + offset, metaHash, hashLen);
        offset += hashLen;

        delete[] headersBuffer;
    }

    // Pad remaining bytes in hashblock0 with zeros
    if (offset < hashBlockSection->Length) {
        memset(hashBlock + offset, 0, hashBlockSection->Length - offset);
        LOG_TRACE("Padding hashblock0 with zeros from offset %zu to %zu", offset, hashBlockSection->Length);
    }

     // Print the final hashblock0 content for verification
    LOG_INFO("Final hashblock0 content after PopulateBootloaderHashBlock:");
    LOG_DUMP_BYTES(hashBlock, hashBlockSection->Length);
}

/*******************************************************************************/
void Versal_2vpAuthenticationContext::Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert)
{
    /* Copy bhSignature when bootloader */
    //memset(cert->section->Data + AC_BH_SIGN_OFFSET, 0, signatureLength); /*check*/
    if (sections.front()->isBootloader)
    {
        //CopybHSignature(bi, cert->section->Data + AC_BH_SIGN_OFFSET);

        //CopyBhHash
        LOG_TRACE("Calculating the Boot Header Hash");
        /* Donot include SMAP data to calculate BH hash */
        uint8_t* tmpBh = bi.bootHeader->section->Data.get() + 0x10;
        
        /* Calculate the BH hash with SHA3 rather than LMS hashing algo */
        uint8_t* sha_hash = new uint8_t[bi.hash->GetHashLength()];
        bi.hash->CalculateVersalHash(true, tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(Versal_2vpSmapWidthTable), sha_hash);
        
        
        /*if (bi.options.DoGenerateHashes())
        {
            std::string hashfilename = "bootheader" + hash->GetHashFileExtension();
            WritePaddedSHAFile(sha_hash_padded, hashfilename);
        }*/

        // For versal_2vp devices, BH hash should NOT be copied into PLM section data
        // PLM section contains only PLM+PMC content, hash blocks are in separate sections
        bool isVersal2vpNative = (bi.options.GetArchType() == Arch::VERSAL_2VP);
        if (!isVersal2vpNative) {
            memcpy(bi.partitionHeaderList.front()->partition->section->Data.get() + HASH_BLOCK_INDEX_BYTES, sha_hash, bi.hash->GetHashLength());
            LOG_TRACE("BH hash copied to PLM section data at offset %d (non-native device)", HASH_BLOCK_INDEX_BYTES);
        } else {
            LOG_TRACE("versal_2vp: Skipping BH hash copy to PLM section data - hash blocks are in separate sections");
        }
        delete[] sha_hash;
    }

    /*  Copy meta header Signature when headers */
    if (sections.front()->Name == "Headers")
    {
        //CopyIHTSignature(bi, cert->section->Data + AC_BH_SIGN_OFFSET);
    }

    // === FIX START: Club hashblock0 signature to end of certificate for authenticated bootloader ===
    if (sections.front()->isBootloader
        && authAlgorithm->Type() != Authentication::None
        && !(authAlgorithm->Type() == Authentication::None && IsSectionEncrypted(bi, sections.front())))
    {
        // Calculate where to write the hashblock0 signature in the certificate section
        size_t certSize = cert->section->Length - GetTotalHashBlockSignSize();
        LOG_TRACE("For LMS single singing lms size %x",GetTotalHashBlockSignSize());
        uint8_t* signaturePtr = cert->section->Data.get() + certSize; // Offset to end of certificate(offset of PLM singture)

        // Generate the signature for hashblock0 and write it to signaturePtr
        Versal_2vpBootImage& vbi = static_cast<Versal_2vpBootImage&>(bi);
        Section* hashBlockSection = vbi.bootloaderHashBlockSection;
        if (hashBlockSection)
        {
            // Populate hashblock0 with all hashes
            Section* metaSection = nullptr;
            if (bi.imageHeaderTable && bi.imageHeaderTable->section)
                metaSection = bi.imageHeaderTable->section;

            // Find the bootloader partition header and image header
            PartitionHeader* bootloaderPH = nullptr;
            ImageHeader* bootloaderIH = nullptr;
            for (auto& ph : bi.partitionHeaderList) {
                if (ph->partition->section->isBootloader) {
                    bootloaderPH = ph;
                    bootloaderIH = ph->imageHeader;
                    break;
                }
            }
            if (bootloaderPH && bootloaderIH) {
                LOG_INFO("Passing section to PopulateBootloaderHashBlock: %s, address: 0x%lx, length: %zu",
                bootloaderPH->partition->section->Name.c_str(),
                (unsigned long)bootloaderPH->partition->section->Address,
                bootloaderPH->partition->section->Length);
                 LOG_DUMP_BYTES(bootloaderPH->partition->section->Data.get(), 64); // Dump first 64 bytes for inspection
                PopulateBootloaderHashforBHandMetaheader(bi, bootloaderPH->partition->section, hashBlockSection, bootloaderIH, metaSection);
            }

            // Now hashBlockSection->Data is ready for signing
            LOG_INFO("########################### Hashblock0 (populated)");
            LOG_DUMP_BYTES(hashBlockSection->Data.get(), hashBlockSection->Length);

            std::list<Section*> hashSections;
            hashSections.push_back(hashBlockSection);
            if (presignFile != "")
            {
                /* HSM: Inject the externally generated bootloader (HashBlock0)
                   signature instead of signing internally. */
                int hbIndex = acIndex;
                if (cert->section->index != 0)
                {
                    hbIndex = cert->section->index;
                }
                LOG_INFO("versal_2vp HSM: Injecting presigned HashBlock0 signature from %s", presignFile.c_str());
                GetPresign(presignFile, (uint16_t)GetTotalHashBlockSignSize(), signaturePtr, hbIndex);
            }
            else
            {
                CopyPartitionSignature(bi, hashSections, signaturePtr, cert->section);
            }
        }
        LOG_INFO("##################################################### Hashblock0 Singnature");
        LOG_DUMP_BYTES(signaturePtr, GetTotalHashBlockSignSize());
       
        LOG_TRACE("singnature length : %x", cert->section->Length);
        LOG_TRACE("Hashblock length : %x", GetTotalHashBlockSignSize());
        LOG_INFO("Hashblock0 signature clubbed to end of certificate section: %s", cert->section->Name.c_str());
    }
    // === FIX END ===
    if (presignFile == "")
    {
        LOG_INFO("No presign file - generating signature");

        if (sections.front()->Name == "Headers" || sections.front()->Name == "HashBlock")
        {
            /* versal_2vp CORRECTION: Hash Block 1 signature goes ONLY in the AC section,
               NOT in the Hash Block itself. Hash Block contains only hash entries. */
            
            // Calculate AC signature offset (signature is at end of AC)
            uint32_t totalSigLen = GetTotalHashBlockSignSize();
            size_t acSigOffset = cert->section->Length - totalSigLen;
            
            LOG_INFO("=== Generating Hash Block 1 Signature (Single Signing: %s) for AC ===", 
                     authAlgorithm->Type() == Authentication::LMS_SHAKE256 ? "LMS-SHAKE256" :
                     authAlgorithm->Type() == Authentication::LMS_SHA2_256 ? "LMS-SHA2-256" :
                     authAlgorithm->Type() == Authentication::HSS_SHAKE256 ? "HSS-SHAKE256" :
                     authAlgorithm->Type() == Authentication::ECDSA ? "ECDSA-P384" :
                     authAlgorithm->Type() == Authentication::ECDSAp521 ? "ECDSA-P521" :
                     authAlgorithm->Type() == Authentication::RSA ? "RSA" :
                     authAlgorithm->Type() == Authentication::MLDSA ? "MLDSA" :
                     authAlgorithm->Type() == Authentication::SLH_SHAKE256 ? "SLH-SHAKE256" : "Unknown");
            LOG_INFO("  AC section: %s, address: 0x%lX, length: %zu", 
                     cert->section->Name.c_str(), (unsigned long)cert->section->Address, cert->section->Length);
            LOG_INFO("  AC signature dest offset: 0x%zX (absolute PDI addr: 0x%lX)", 
                     acSigOffset, (unsigned long)(cert->section->Address + acSigOffset));
            LOG_INFO("  Signature length: %u bytes, totalSigLen (with padding): %u bytes", 
                     signatureLength, totalSigLen);

            // Generate signature directly into AC section (NOT into Hash Block)
            CopyPartitionSignature(bi, sections, cert->section->Data.get() + acSigOffset, cert->section);
            
            // Log the generated signature
            LOG_INFO("  AC signature after generation (first 32 bytes at offset 0x%zX):", acSigOffset);
            LOG_DUMP_BYTES(cert->section->Data.get() + acSigOffset, 32);
            LOG_INFO("  AC signature generation complete - %u bytes written to AC", signatureLength);
            
            // Print Hash Block 1 summary (hash entries only, NO signatures)
            LOG_INFO("=== Hash Block 1 Summary (Meta Header - Single Signing) ===");
            LOG_INFO("  Hash Data ONLY: Offset=0x0, Length=%u bytes", bi.imageHeaderTable->hashBlockSectionLength);
            LOG_INFO("  Total HashBlock Size: %zu bytes (hash entries only)", bi.imageHeaderTable->hashBlockSection->Length);
            LOG_INFO("  Signature is in AC, NOT in Hash Block");
        } 
        else 
        {
            // For versal_2vp devices, do NOT copy signature into PLM section after hash block
            if (bi.options.GetArchType() == Arch::VERSAL_2VP && sections.front()->isBootloader) {
                LOG_INFO("versal_2vp: Skipping CopyPartitionSignature for PLM section - preserving pure PLM+PMC content");
            } else {
                LOG_INFO("Calling CopyPartitionSignature for partition: %s", sections.front()->Name.c_str());
                LOG_INFO("Target signature location: %p (offset 0x%x in section data)", 
                    sections.front()->Data.get() + bi.hashBlockLength, bi.hashBlockLength);
                CopyPartitionSignature(bi, sections, sections.front()->Data.get() + bi.hashBlockLength, cert->section);
            }
        }

        //CopyPartitionSignature(bi, sections, cert->section->Data + AC_PARTITION_SIGN_OFFSET, cert->section);
    }
    else
    {
        int index = acIndex;
        if (cert->section->index != 0)
        {
            index = cert->section->index;
        }
        if (sections.front()->Name == "Headers" || sections.front()->Name == "HashBlock")
        {
            /* HSM: inject the external MetaHeader/HashBlock signature into
               the AC section */
            uint32_t totalSigLen = GetTotalHashBlockSignSize();
            size_t acSigOffset = cert->section->Length - totalSigLen;
            LOG_INFO("versal_2vp HSM: Injecting presigned %s signature from %s at AC offset 0x%zX",
                     sections.front()->Name.c_str(), presignFile.c_str(), acSigOffset);
            GetPresign(presignFile, (uint16_t)totalSigLen, cert->section->Data.get() + acSigOffset, index);
        }
        // For versal_2vp bootloader partitions, skip GetPresign to preserve pure PLM+PMC content
        else if (bi.options.GetArchType() == Arch::VERSAL_2VP && sections.front()->isBootloader) {
            LOG_INFO("versal_2vp: Skipping GetPresign for PLM section in presignFile branch - preserving pure PLM+PMC content");
        } else {  // TBD : cross check if it has to handle for presign cases
            if (authAlgorithm->Type() == Authentication::RSA)
                GetPresign(presignFile, sections.front()->Data.get() + bi.hashBlockLength, index);
            else if (authAlgorithm->Type() == Authentication::ECDSA)
                GetPresign(presignFile, sections.front()->Data.get() + bi.hashBlockLength, index);
            else {
                //EC-p521 
                //LMS
            }
        }
        acIndex++;
    }
    // At the very end of Versal_2vpAuthenticationContext::Link
    if (sections.front()->isBootloader) {
        LOG_INFO("Bootloader certificate (final AC + hashblock0 signature): %s", cert->section->Name.c_str());
        LOG_DUMP_BYTES(cert->section->Data.get(), cert->section->Length);
        LOG_INFO("Bootloader certificate address: 0x%08lx, length: 0x%x", (unsigned long)cert->section->Address, cert->section->Length);
    }
    
    // Print final Hash Block 1 content for Meta Header
    if (sections.front()->Name == "Headers") {
        LOG_INFO("=== Final Hash Block 1 Content (Meta Header - Single Signing) ===");
        LOG_INFO("  Total size: %zu bytes (Hash=%u + Signature=%u)", 
                 bi.imageHeaderTable->hashBlockSection->Length,
                 bi.imageHeaderTable->hashBlockSectionLength,
                 signatureLength);
        LOG_INFO("  Hash Block 1 data (first 512 bytes or full if smaller):");
        size_t printLen = bi.imageHeaderTable->hashBlockSection->Length;
        if (printLen > 512) printLen = 512;
        LOG_DUMP_BYTES(bi.imageHeaderTable->hashBlockSection->Data.get(), printLen);
        if (bi.imageHeaderTable->hashBlockSection->Length > 512) {
            LOG_INFO("  ... (truncated, showing first 512 of %zu bytes)", bi.imageHeaderTable->hashBlockSection->Length);
        }
    }
}

/******************************************************************************/
void Versal_2vpAuthenticationContext::CopybHSignature(BootImage& bi, uint8_t* ptr)
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
        if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
        {
            authAlgorithm->CreateSignature(sha_hash_padded, hashLength, sskFile.c_str(), 
                bHsignaturetmp, signatureLength, lmsOnly, spkFile.c_str());
        }
        else if (authAlgorithm->Type() == Authentication::MLDSA)
        {
            authAlgorithm->CreateSignature(sha_hash_padded, (uint8_t*)secondaryKey.get(), bHsignaturetmp);
        }
        else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
        {
            size_t sig = signatureLength;
            authAlgorithm->CreateSignature(sha_hash_padded, hashLength, sskFile.c_str(), bHsignaturetmp, sig);
            signatureLength = sig;
        }
        else
        {
            authAlgorithm->CreateSignature(sha_hash_padded, (uint8_t*)secondaryKey.get(), bHsignaturetmp);
        }
        authAlgorithm->RearrangeEndianess(bHsignaturetmp, signatureLength);

        if (bhSignLoaded)
        {
            if (memcmp(bHsignature.get(), bHsignaturetmp, signatureLength) != 0)
            {
                LOG_ERROR("Authentication Error !!!\n           Loaded BH Signature does not match calculated BH Signature");
            }
        }
        memcpy(ptr, bHsignaturetmp, signatureLength);
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
void Versal_2vpAuthenticationContext::CopyIHTSignature(BootImage & bi, uint8_t * ptr)
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
        if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
        { 
            authAlgorithm->CreateSignature(sha_hash_padded, hashLength, sskFile.c_str(), 
                signaturetmp, signatureLength, lmsOnly, spkFile.c_str());
        }
        else if (authAlgorithm->Type() == Authentication::MLDSA)
        {
            authAlgorithm->CreateSignature(sha_hash_padded, (uint8_t*)secondaryKey.get(), signaturetmp);
        }
        else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
        {
            size_t sig = signatureLength;
            authAlgorithm->CreateSignature(sha_hash_padded, hashLength, sskFile.c_str(), signaturetmp, sig);
            signatureLength = sig;
        }
        else
        {
            if (authAlgorithm->Type() != Authentication::ECDSA)
            {
                authAlgorithm->RearrangeEndianess(sha_hash_padded, signatureLength);
            }
            authAlgorithm->CreateSignature(sha_hash_padded, (uint8_t*)secondaryKey.get(), signaturetmp);
            if (authAlgorithm->Type() != Authentication::ECDSA)
            {
                authAlgorithm->RearrangeEndianess(signaturetmp, signatureLength);
            }
        }
        
        /* if (bhSignLoaded)
        {
        if (memcmp(bHsignature.get(), bHsignaturetmp, rsaKeyLength) != 0)
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
void RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0)
{
    AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
    RSA_Exponentiation(base, ((Versal_2vpKey*) primaryKey)->N.get(), ((Versal_2vpKey*) primaryKey)->N_ext.get(), ((Versal_2vpKey*) primaryKey)->D.get(), result0);
}

/******************************************************************************/
void ECDSAHBAuthenticationAlgorithm_versal_2vp::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0)
{
    ECDSASignature(base, ((Versal_2vpKey*)primaryKey)->eckey, result0);
}

/******************************************************************************/
void ECDSAP521HBAuthenticationAlgorithm_versal_2vp::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0)
{
    ECDSASignature(base, ((Versal_2vpKey*)primaryKey)->eckey, result0);
}

/******************************************************************************/
void LMSAuthenticationAlgorithm_versal_2vp::VerifySignature(const uint8_t* buffer, size_t buf_len, const char* keyFile, uint8_t* result0, size_t result0_len, bool lmsOnly)
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
void LMSAuthenticationAlgorithm_versal_2vp::CreateSignature(const uint8_t* buffer, size_t buf_len, const char* keyFile, 
    uint8_t* result0, size_t result0_len, bool lmsOnly, const char* publicKeyFile)
{
    size_t sig_len = 0;
    uint8_t* hss_sign;
    if (lmsOnly)
    {
        hss_sign = new uint8_t[result0_len + 4];
        memset(hss_sign, 0, result0_len + 4);
    }
    else
    {
        hss_sign = new uint8_t[result0_len];
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
void MLDSAAuthenticationAlgorithm_versal_2vp::VerifySignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* sign, bool isSign)
{
    //LOG_TRACE("Verifying MLDSA signature");
    
    // Check for null pointers
    if (!base || !primaryKey || !sign) {
        LOG_ERROR("ML-DSA VerifySignature: NULL pointer detected - base: %p, primaryKey: %p, sign: %p", 
                  base, primaryKey, sign);
        return;
    }
    
    // Cast primaryKey to Versal_2vpKey* to match RSA/ECDSA convention
    Versal_2vpKey* keyStruct = (Versal_2vpKey*)primaryKey;
    
    // Check if the ML-DSA public key is available
    if (!keyStruct->mldsa_public_key) {
        LOG_ERROR("ML-DSA VerifySignature: mldsa_public_key is NULL in key structure");
        return;
    }
    
    bool result = false;

    std::array<uint8_t, ml_dsa_87::PubKeyByteLen> pubkey{};
    for(int i=0; i<ml_dsa_87::PubKeyByteLen; i++)
        pubkey[i] = keyStruct->mldsa_public_key[i];
    
    std::array<uint8_t, ml_dsa_87::SigByteLen> sig{};
    for(int i=0; i<ml_dsa_87::SigByteLen; i++)
        sig[i] = sign[i];

    if(isSign == true)
    {
        std::array<uint8_t, MLDSA_PUB_KEY_LENGTH + TELLURIDE_AC_SPK_HDR_LENGTH> msg{};
        for(int i=0; i<MLDSA_PUB_KEY_LENGTH + TELLURIDE_AC_SPK_HDR_LENGTH; i++)
            msg[i] = base[i];
        result = ml_dsa_87::verify(pubkey, msg, {}, sig);
    }
    else
    {
        std::array<uint8_t,BH_HASH_BLOCK_BYTES_TELLURIDE> msg{};
        for(int i=0; i<BH_HASH_BLOCK_BYTES_TELLURIDE; i++)
            msg[i] = base[i];
        result = ml_dsa_87::verify(pubkey, msg, {}, sig);
    } 
    
    if (result == true)
        LOG_TRACE("Verified MLDSA signature");
    else
        LOG_ERROR("Error verifying MLDSA signature");
    
}

/******************************************************************************/
void MLDSAAuthenticationAlgorithm_versal_2vp::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0, bool isSign)
{
    // Check for null pointers
    if (!base || !primaryKey || !result0) {
        LOG_ERROR("ML-DSA CreateSignature: NULL pointer detected - base: %p, primaryKey: %p, result0: %p", 
                  base, primaryKey, result0);
        return;
    }

    // Cast primaryKey to Versal_2vpKey* to match RSA/ECDSA convention
    Versal_2vpKey* keyStruct = (Versal_2vpKey*)primaryKey;
    
    // Check if the ML-DSA private key is available
    if (!keyStruct->mldsa_private_key) {
        LOG_ERROR("ML-DSA CreateSignature: mldsa_private_key is NULL in key structure");
        return;
    }

    std::array<uint8_t, ml_dsa_87::SigningSeedByteLen> rnd{};
    //randomshake::randomshake_t<128> csprng;
    //csprng.generate(rnd);
    for(int i=0; i<ml_dsa_87::SigningSeedByteLen; i++)
        rnd[i] = 0;

    std::array<uint8_t, ml_dsa_87::SecKeyByteLen> seckey{};
    for(int i=0; i<ml_dsa_87::SecKeyByteLen; i++)
        seckey[i] = keyStruct->mldsa_private_key[i];

    std::array<uint8_t, ml_dsa_87::SigByteLen> sig{};

    if(isSign == true)
    {
        std::array<uint8_t, MLDSA_PUB_KEY_LENGTH + TELLURIDE_AC_SPK_HDR_LENGTH> msg{};
        for(int i=0; i< MLDSA_PUB_KEY_LENGTH + TELLURIDE_AC_SPK_HDR_LENGTH; i++)
            msg[i] = base[i];
        ml_dsa_87::sign(rnd, seckey, msg, {}, sig);
    }
    else
    {
        std::array<uint8_t,BH_HASH_BLOCK_BYTES_TELLURIDE> msg{};
        for(int i=0; i<BH_HASH_BLOCK_BYTES_TELLURIDE; i++)
            msg[i] = base[i];
        ml_dsa_87::sign(rnd, seckey, msg, {}, sig);
    }

    for(int i=0; i<ml_dsa_87::SigByteLen; i++)
        result0[i] = sig[i];
}

/******************************************************************************/
// Variable-length ML-DSA CreateSignature for HashBlock1 (variable size based on partition count)
void MLDSAAuthenticationAlgorithm_versal_2vp::CreateSignature(const uint8_t* base, size_t msg_len, uint8_t* primaryKey, uint8_t* result0)
{
    // Check for null pointers
    if (!base || !primaryKey || !result0) {
        LOG_ERROR("ML-DSA CreateSignature (variable-length): NULL pointer detected - base: %p, primaryKey: %p, result0: %p", 
                  base, primaryKey, result0);
        return;
    }
    
    if (msg_len == 0) {
        LOG_ERROR("ML-DSA CreateSignature (variable-length): msg_len is 0");
        return;
    }

    // Cast primaryKey to Versal_2vpKey* to match RSA/ECDSA convention
    Versal_2vpKey* keyStruct = (Versal_2vpKey*)primaryKey;
    
    // Check if the ML-DSA private key is available
    if (!keyStruct->mldsa_private_key) {
        LOG_ERROR("ML-DSA CreateSignature (variable-length): mldsa_private_key is NULL in key structure");
        return;
    }

    LOG_TRACE("ML-DSA CreateSignature (variable-length): msg_len=%zu bytes", msg_len);

    std::array<uint8_t, ml_dsa_87::SigningSeedByteLen> rnd{};
    for(size_t i=0; i<ml_dsa_87::SigningSeedByteLen; i++)
        rnd[i] = 0;

    std::array<uint8_t, ml_dsa_87::SecKeyByteLen> seckey{};
    for(size_t i=0; i<ml_dsa_87::SecKeyByteLen; i++)
        seckey[i] = keyStruct->mldsa_private_key[i];

    std::array<uint8_t, ml_dsa_87::SigByteLen> sig{};

    // Use std::span for variable-length message (HashBlock1 can have different sizes)
    std::span<const uint8_t> msg(base, msg_len);
    ml_dsa_87::sign(rnd, seckey, msg, {}, sig);

    for(size_t i=0; i<ml_dsa_87::SigByteLen; i++)
        result0[i] = sig[i];
    
    LOG_TRACE("ML-DSA CreateSignature (variable-length): Generated signature of %zu bytes", ml_dsa_87::SigByteLen);
}

/******************************************************************************/
// Variable-length ML-DSA VerifySignature for HashBlock1 (variable size based on partition count)
void MLDSAAuthenticationAlgorithm_versal_2vp::VerifySignature(const uint8_t* base, size_t msg_len, uint8_t* primaryKey, uint8_t* sign)
{
    // Check for null pointers
    if (!base || !primaryKey || !sign) {
        LOG_ERROR("ML-DSA VerifySignature (variable-length): NULL pointer detected - base: %p, primaryKey: %p, sign: %p", 
                  base, primaryKey, sign);
        return;
    }
    
    if (msg_len == 0) {
        LOG_ERROR("ML-DSA VerifySignature (variable-length): msg_len is 0");
        return;
    }
    
    // Cast primaryKey to Versal_2vpKey* to match RSA/ECDSA convention
    Versal_2vpKey* keyStruct = (Versal_2vpKey*)primaryKey;
    
    // Check if the ML-DSA public key is available
    if (!keyStruct->mldsa_public_key) {
        LOG_ERROR("ML-DSA VerifySignature (variable-length): mldsa_public_key is NULL in key structure");
        return;
    }
    
    LOG_TRACE("ML-DSA VerifySignature (variable-length): msg_len=%zu bytes", msg_len);
    
    bool result = false;

    std::array<uint8_t, ml_dsa_87::PubKeyByteLen> pubkey{};
    for(size_t i=0; i<ml_dsa_87::PubKeyByteLen; i++)
        pubkey[i] = keyStruct->mldsa_public_key[i];
    
    std::array<uint8_t, ml_dsa_87::SigByteLen> sig{};
    for(size_t i=0; i<ml_dsa_87::SigByteLen; i++)
        sig[i] = sign[i];

    // Use std::span for variable-length message (HashBlock1 can have different sizes)
    std::span<const uint8_t> msg(base, msg_len);
    result = ml_dsa_87::verify(pubkey, msg, {}, sig);
    
    if (result == true)
        LOG_TRACE("Verified MLDSA signature (variable-length, %zu bytes)", msg_len);
    else
        LOG_ERROR("Error verifying MLDSA signature (variable-length, %zu bytes)", msg_len);
}

/******************************************************************************/
static int demo_randombytes(uint8_t *x, size_t xlen) {
    for (size_t i = 0; i < xlen; i++) {
        x[i] = (uint8_t)rand();
        //x[i] = 0;
    }
    return 0;
}

/******************************************************************************/
void SLHAuthenticationAlgorithm_versal_2vp::CreateSignature(const uint8_t* buffer, size_t buf_len, const char* keyFile, uint8_t* result, size_t& result_len)
{
    // SLH parameter set
    const slh_param_t *slh_params = &slh_dsa_shake_256s;

    // Get signature size and secret key size
    size_t sig_sz = slh_sig_sz(slh_params);
    size_t sk_sz = slh_sk_sz(slh_params);

    // Load secret key from file
    uint8_t* sk = new uint8_t[sk_sz];
    KeySLH_versal_2vp tempKey("temp");
    tempKey.LoadKey(keyFile, sk, sk_sz);

    //Prepend two zero bytes to the buffer to match the M' = 0x00||0x00||M format
    size_t prepended_buf_len = buf_len + 2;
    uint8_t* prepended_buffer = new uint8_t[prepended_buf_len];
    prepended_buffer[0] = 0;
    prepended_buffer[1] = 0;
    memcpy(prepended_buffer + 2, buffer, buf_len);

    // Generate signature using the prepended buffer
    result_len = slh_sign(result, prepended_buffer, prepended_buf_len, sk, demo_randombytes, slh_params);
    //result_len = slh_sign(result, buffer, buf_len, sk, demo_randombytes, slh_params);

    // Validate
    if (result_len == 0 || result_len != sig_sz) {
        LOG_ERROR("SLH signature generation failed: got %zu bytes, expected %zu bytes", result_len, sig_sz);
    }
    
    delete[] prepended_buffer;
    delete[] sk;
}

void SLHAuthenticationAlgorithm_versal_2vp::VerifySignature(const uint8_t* msg, size_t msg_len, const uint8_t* sig, const char* keyFile)
{
    //LOG_TRACE("INSIDE SLH verifysignature ");
    const slh_param_t *slh_params = &slh_dsa_shake_256s;
    size_t pk_sz = SLHDSA_PUB_KEY_LENGTH;
    uint8_t pk[pk_sz];
    KeySLH_versal_2vp tempKey("temp");
    tempKey.LoadKey(keyFile, pk, pk_sz);
    
    //Prepend two zero bytes to the buffer to match the M' = 0x00||0x00||M format
    size_t prepended_msg_len = msg_len + 2;
    uint8_t* prepended_msg = new uint8_t[prepended_msg_len];
    prepended_msg[0] = 0;
    prepended_msg[1] = 0;
    memcpy(prepended_msg + 2, msg, msg_len);

    // verify signature using the prepended buffer
    bool is_valid = slh_verify(prepended_msg, prepended_msg_len, sig, pk, slh_params);
    //bool is_valid = slh_verify(msg, msg_len, sig, pk, slh_params);
    if(is_valid)
    {
        LOG_TRACE("signature verification succeeded");
    }
    else
    {
        LOG_TRACE("signature verification failed");
    }
}

/******************************************************************************/
void Versal_2vpAuthenticationContext::GenerateIHTHash(BootImage& bi, uint8_t* sha_hash_padded)
{
    LOG_TRACE("Calculating Image Header Table Hash");
    uint8_t* tmpIht = bi.imageHeaderTable->section->Data.get();
    uint8_t* sha_hash = new uint8_t[hashLength];
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        hash->CalculateHash(true, tmpIht, bi.imageHeaderTable->section->Length, sha_hash);
    }
    else
    {
        hash->CalculateVersalHash(true,tmpIht, bi.imageHeaderTable->section->Length, sha_hash);
    }
    authAlgorithm->CreatePadding(sha_hash_padded, sha_hash, hashLength);
    delete[] sha_hash;
}

/******************************************************************************/
void Versal_2vpAuthenticationContext::GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded)
{
    LOG_TRACE("Calculating the Boot Header Hash");
    /* Donot include SMAP data to calculate BH hash */
    uint8_t* tmpBh = bi.bootHeader->section->Data.get() + 0x10;
    LOG_TRACE("BH content");
    LOG_DUMP_BYTES(bi.bootHeader->section->Data.get() + 0x10, 64);
    uint8_t* sha_hash = new uint8_t[hashLength];
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        hash->CalculateHash(true, tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(Versal_2vpSmapWidthTable), sha_hash);
    }
    else
    {
        hash->CalculateVersalHash(true, tmpBh, bi.bootHeader->section->Length - sizeof(Versal_2vpSmapWidthTable), sha_hash);
    }
    
    authAlgorithm->CreatePadding(sha_hash_padded, sha_hash, hashLength);
    
    // For versal_2vp devices, don't copy BH hash into PLM section since PLM section contains only PLM+PMC content
    // Hash block is in a separate section (plm.elf.0.HashBlock)
    bool isVersal2vpNative = (bi.options.GetArchType() == Arch::VERSAL_2VP);
    if (!isVersal2vpNative) {
        memcpy(bi.partitionHeaderList.front()->section->Data.get() + HASH_BLOCK_INDEX_BYTES, sha_hash, hashLength);
    }
    
    delete[] sha_hash;
}

/******************************************************************************/
void Versal_2vpAuthenticationContext::GenerateSPKHash(uint8_t* sha_hash_padded)
{
    #if 0
    uint8_t* tempBuffer = NULL;
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
    else if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        actualKeySize = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);
        totalKeySize = actualKeySize + PADDING_16B(actualKeySize);
		signatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);
        totalSignatureLength = signatureLength + PADDING_16B(signatureLength);
    }
    else if (authAlgorithm->Type() == Authentication::MLDSA)
    {
        totalKeySize = actualKeySize = MLDSA_PUB_KEY_LENGTH;
        signatureLength = MLDSA_ACTUAL_SIGN_LEN;
        totalSignatureLength = MLDSA_TOTAL_SIGN_LEN;
    }
    else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
    {
        actualKeySize = totalKeySize = SLHDSA_PUB_KEY_LENGTH;
        signatureLength = SLHDSA_ACTUAL_SIGN_LENGTH;
        totalSignatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
    }

    uint8_t* spkFull = new uint8_t[actualKeySize];

    //uint8_t spkSHA3Padding[4] = { 0,0,0,0 };
    //FillSha3Padding(spkSHA3Padding, sizeof(spkSHA3Padding));

    if (!secondaryKey->Loaded)
    {
        ParseSPKeyFile(spkFile);
    }
    secondaryKey->Export(spkFull);
    hashLength = hash->GetHashLength();
    uint8_t* shaHash = new uint8_t[hashLength];
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        // acTotalSpkSize; acActualSpkSize; acSpkTotalSignatureSize; acSpkActualSignatureSize; acSpkId; acSpkHdrAlignment[3]; , acSpk
        //acSpkSize, acSpkSignatureSize, acSpkId, acSpkHdrAlignment, acSpk
        tempBuffer = new uint8_t[totalKeySize + TELLURIDE_AC_SPK_HDR_LENGTH];
        memset(tempBuffer, 0, totalKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
        
        WriteLittleEndian32(tempBuffer, totalKeySize);
        WriteLittleEndian32(tempBuffer + sizeof(uint32_t), actualKeySize);
        WriteLittleEndian32(tempBuffer + (2 * sizeof(uint32_t)), totalSignatureLength);
        WriteLittleEndian32(tempBuffer + (3 * sizeof(uint32_t)), signatureLength);
        WriteLittleEndian32(tempBuffer + (4 * sizeof(uint32_t)), spkIdentification);
        memcpy(tempBuffer + TELLURIDE_AC_SPK_HDR_LENGTH, (uint8_t*)spkFull, actualKeySize);
#ifdef DEBUG
        LOG_TRACE("DATA being Hashed for SPK Sign");
        LOG_DUMP_BYTES(tempBuffer, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
#endif
        hash->CalculateHash(true, (uint8_t*)tempBuffer, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, shaHash);
    }
    else if (authAlgorithm->Type() == Authentication::MLDSA)
    {
        // acTotalSpkSize; acActualSpkSize; acSpkTotalSignatureSize; acSpkActualSignatureSize; acSpkId; acSpkHdrAlignment[3]; , acSpk
        //acSpkSize, acSpkSignatureSize, acSpkId, acSpkHdrAlignment, acSpk
        tempBuffer = new uint8_t[totalKeySize + TELLURIDE_AC_SPK_HDR_LENGTH];
        memset(tempBuffer, 0, totalKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
        
        WriteLittleEndian32(tempBuffer, totalKeySize);
        WriteLittleEndian32(tempBuffer + sizeof(uint32_t), actualKeySize);
        WriteLittleEndian32(tempBuffer + (2 * sizeof(uint32_t)), totalSignatureLength);
        WriteLittleEndian32(tempBuffer + (3 * sizeof(uint32_t)), signatureLength);
        WriteLittleEndian32(tempBuffer + (4 * sizeof(uint32_t)), spkIdentification);
        memcpy(tempBuffer + TELLURIDE_AC_SPK_HDR_LENGTH, (uint8_t*)spkFull, actualKeySize);
#ifdef DEBUG
        LOG_TRACE("DATA being Hashed for SPK Sign");
        LOG_DUMP_BYTES(tempBuffer, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
#endif
        hash->CalculateHash(true, (uint8_t*)tempBuffer, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, shaHash);
        LOG_TRACE("DATA being Hashed for SPK Sign");
        LOG_DUMP_BYTES((uint8_t*)tempBuffer, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
        LOG_TRACE("SPK Hash");
        LOG_DUMP_BYTES(shaHash, hashLength);
    }
    else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
    {
        tempBuffer = new uint8_t[totalKeySize + TELLURIDE_AC_SPK_HDR_LENGTH];
        memset(tempBuffer, 0, totalKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
        
        WriteLittleEndian32(tempBuffer, totalKeySize);
        WriteLittleEndian32(tempBuffer + sizeof(uint32_t), actualKeySize);
        WriteLittleEndian32(tempBuffer + (2 * sizeof(uint32_t)), totalSignatureLength);
        WriteLittleEndian32(tempBuffer + (3 * sizeof(uint32_t)), signatureLength);
        WriteLittleEndian32(tempBuffer + (4 * sizeof(uint32_t)), spkIdentification);
        memcpy(tempBuffer + TELLURIDE_AC_SPK_HDR_LENGTH, (uint8_t*)spkFull, actualKeySize);

        hash->CalculateHash(true, (uint8_t*)tempBuffer, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, shaHash);
        LOG_TRACE("DATA being Hashed for SPK Sign");
        LOG_DUMP_BYTES((uint8_t*)tempBuffer, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
        LOG_TRACE("SPK Hash");
        LOG_DUMP_BYTES(shaHash, hashLength);
    }
    else
    {
        // acTotalSpkSize; acActualSpkSize; acSpkTotalSignatureSize; acSpkActualSignatureSize; acSpkId; acSpkHdrAlignment[3]; , acSpk
        //acSpkSize, acSpkSignatureSize, acSpkId, acSpkHdrAlignment, acSpk
        tempBuffer = new uint8_t[actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH];
        memset(tempBuffer, 0, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);

        WriteLittleEndian32(tempBuffer, totalKeySize);
        WriteLittleEndian32(tempBuffer + sizeof(uint32_t), actualKeySize);
        WriteLittleEndian32(tempBuffer + (2 * sizeof(uint32_t)), signatureLength);
        WriteLittleEndian32(tempBuffer + (3 * sizeof(uint32_t)), signatureLength);
        WriteLittleEndian32(tempBuffer + (4 * sizeof(uint32_t)), spkIdentification);

        authAlgorithm->RearrangeEndianess(spkFull + RSA_4096_N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(spkFull + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(spkFull + RSA_4096_E, RSA_4096_E_SIZE);
        memcpy(tempBuffer + TELLURIDE_AC_SPK_HDR_LENGTH, (uint8_t*)spkFull, actualKeySize);
#ifdef DEBUG
        LOG_TRACE("DATA being Hashed for SPK Sign");
        LOG_DUMP_BYTES(tempBuffer, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH);
#endif
        hash->CalculateVersalHash(true, (uint8_t*)tempBuffer, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, shaHash);
    }
#ifdef DEBUG
    LOG_TRACE("Hash");
    LOG_DUMP_BYTES(shaHash, hashLength);
    LOG_TRACE("Hash with PKCS Padding");
#endif
    // Create PKCS padding
    authAlgorithm->CreatePadding(sha_hash_padded, shaHash, hashLength);
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        LOG_DUMP_BYTES(sha_hash_padded, hashLength);
    }
    else
    {
        LOG_DUMP_BYTES(sha_hash_padded, signatureLength);
    }
    delete[] shaHash;
    delete[] tempBuffer;
    #endif

    LOG_TRACE("Calculating the SPK Hash");
    
    // Get the current data section from class member
    Section* dataSection = GetCurrentDataSection();
    
    // Create NEW header format for Versal_2VP
    HeaderAuthCert_versal_2vp spkHeader;
    memset(&spkHeader, 0, sizeof(spkHeader));
    
    spkHeader.Algorithm    = authAlgorithm->GetAuthHeader();
    spkHeader.RevocationID = spkIdentification;
    spkHeader.Hybrid       = 0x0;
    spkHeader.Authority    = 0x0;
    
    // Set SPK permission based on partition type
    if (dataSection && dataSection->isBootloader)
    {
        spkHeader.Permission = 0x1; // BOOT permission - required by ROM for booting
    }
    else
    {
        spkHeader.Permission = 0x8; // Others permission - used by firmware for general signing
    }
    
    // Get key sizes based on algorithm - FIXED TO INCLUDE ALL ALGORITHMS
    size_t actualKeySize = 0;
    size_t totalKeySize = 0;


    if (authAlgorithm->Type() == Authentication::RSA)
    {
        actualKeySize = VERSAL_ACKEY_STRUCT_SIZE;
        totalKeySize = actualKeySize; // RSA doesn't need extra padding
    }
    else if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        actualKeySize = 2 * EC_P384_KEY_LENGTH;
        totalKeySize = actualKeySize; // ECDSA doesn't need extra padding
    }
    else if (authAlgorithm->Type() == Authentication::ECDSAp521)
    {
        actualKeySize = 2 * EC_P521_KEY_LENGTH2;
        totalKeySize = actualKeySize; // ECDSAp521 doesn't need extra padding
    }
    else if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        actualKeySize = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);
        totalKeySize = actualKeySize + PADDING_16B(actualKeySize); // LMS/HSS needs 16-byte alignment
    }
    else if (authAlgorithm->Type() == Authentication::MLDSA)
    {
        totalKeySize = actualKeySize = MLDSA_PUB_KEY_LENGTH;
    }
    else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
    {
        actualKeySize = SLHDSA_PUB_KEY_LENGTH;
        totalKeySize = actualKeySize; // SLH_SHAKE256 doesn't need extra padding
    }
    else
    {
        LOG_ERROR("Unsupported authentication algorithm for SPK hash generation");
        return;
    }
    
    // Export SPK key data
    uint8_t* spkFull = new uint8_t[actualKeySize]; // Use totalKeySize for buffer allocation
    memset(spkFull, 0, actualKeySize); // Zero the entire buffer including padding
    
    if (!secondaryKey->Loaded)
    {
        ParseSPKeyFile(spkFile);
    }
    secondaryKey->Export(spkFull); // Export actual key (padding area remains zero)
    
    // For RSA, rearrange endianness
    if (authAlgorithm->Type() == Authentication::RSA)
    {
        authAlgorithm->RearrangeEndianess(spkFull + RSA_4096_N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(spkFull + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(spkFull + RSA_4096_E, RSA_4096_E_SIZE);
    }
    
    // Build buffer with NEW SPK header + SPK key (with padding for LMS/HSS)
    uint8_t* tempBuffer = new uint8_t[sizeof(HeaderAuthCert_versal_2vp) + actualKeySize];
    memcpy(tempBuffer, &spkHeader, sizeof(HeaderAuthCert_versal_2vp));
    memcpy(tempBuffer + sizeof(HeaderAuthCert_versal_2vp), spkFull, actualKeySize);
    
    // Calculate hash on header + padded key
    uint8_t* shaHash = new uint8_t[hashLength];
    
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        // Use algorithm-specific hash function
        hash->CalculateHash(true, tempBuffer, sizeof(HeaderAuthCert_versal_2vp) + actualKeySize, shaHash);
    }
    else
    {
        // Use Versal hash function for RSA/ECDSA/ECDSAp521
        hash->CalculateVersalHash(true, tempBuffer, sizeof(HeaderAuthCert_versal_2vp) + actualKeySize, shaHash);
    }
    
    LOG_TRACE("SPK Hash (Header + Key with padding):");
    LOG_DUMP_BYTES(shaHash, hashLength);
    LOG_TRACE("SPK Header:");
    LOG_DUMP_BYTES((uint8_t*)&spkHeader, sizeof(HeaderAuthCert_versal_2vp));
    LOG_TRACE("SPK Key (with padding):");
    LOG_DUMP_BYTES(spkFull, actualKeySize);
    
    // Create PKCS padding
    authAlgorithm->CreatePadding(sha_hash_padded, shaHash, hashLength);
    
    // Clean up
    delete[] shaHash;
    delete[] spkFull;
    delete[] tempBuffer;

}

/******************************************************************************/
void Versal_2vpAuthenticationContext::CopySPKSignature(BootImage& bi, uint8_t* ptr)
{
    // Determine the hybrid mask for the current algorithm (only for hybrid contexts)
    uint32_t hybridMask = 0x0;
    
    // Check if this is a hybrid context by trying to cast to hybrid type
    Versal_2vpHybridAuthenticationContext* hybridContext = dynamic_cast<Versal_2vpHybridAuthenticationContext*>(this);
    if (hybridContext && hybridContext->IsHybrid()) {
        // This is a hybrid context, determine the appropriate hybrid mask
        std::vector<Authentication::Type> hybridAlgorithms = hybridContext->GetHybridAlgorithms();
        Authentication::Type currentAlgorithm = authAlgorithm->Type();
        
        // Find the current algorithm in the hybrid algorithms list
        for (size_t i = 0; i < hybridAlgorithms.size(); ++i) {
            if (hybridAlgorithms[i] == currentAlgorithm) {
                // Set the hybrid mask to the OTHER algorithm's mask
                // If current is algorithm[0], set mask to algorithm[1]
                // If current is algorithm[1], set mask to algorithm[0]
                size_t otherIndex = (i == 0) ? 1 : 0;
                if (otherIndex < hybridAlgorithms.size()) {
                    hybridMask = hybridContext->GetHybridMask(hybridAlgorithms[otherIndex]);
                    LOG_INFO("Hybrid Mode: Setting hybrid mask for SPK signature: current algorithm %s, hybrid mask 0x%08x (for %s)", 
                             hybridContext->GetAlgorithmName(currentAlgorithm), hybridMask, hybridContext->GetAlgorithmName(hybridAlgorithms[otherIndex]));
                }
                break;
            }
        }
    }
    // For single signing contexts, hybridMask remains 0x0
    
    // CreateSPKSignature will auto-detect which algorithm it is (Algorithm 1 or 2) based on authType/authType1
    CreateSPKSignature(bi, hybridMask);
    LOG_TRACE("Copying the SPK signature into the Authentication Certificate");
    memcpy(ptr, spksignature.get(), signatureLength);
}

/******************************************************************************/
std::string Versal_2vpAuthenticationContext::GetCertificateName(std::string name)
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
void Versal_2vpAuthenticationContext::GeneratePPKHash(const std::string& filename)
{
    int ppkSize = VERSAL_ACKEY_STRUCT_SIZE;
    
    if (authAlgorithm->Type() == Authentication::ECDSA)
        ppkSize = 2 * EC_P384_KEY_LENGTH;
    if (authAlgorithm->Type() == Authentication::ECDSAp521)
        ppkSize = 2 * EC_P521_KEY_LENGTH2;
    if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256) || (authAlgorithm->Type() == Authentication::LMS_SHAKE256))
    {
        hash = new HashSha3();
        ppkSize = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly);
    }
    if (authAlgorithm->Type() == Authentication::MLDSA)
    {
       // hash = new HashSha2_512();
        hash = new HashSha3();
        ppkSize = MLDSA_PUB_KEY_LENGTH;
    }
    if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
    {
        //hash = new HashShake256();
        hash = new HashSha3();
        ppkSize = SLHDSA_PUB_KEY_LENGTH;
    }
    hashLength = hash->GetHashLength();
    
    // For versal_2vp devices, PPK hash generation needs to consider both PPK header and PPK key
    // similar to how SPK hash is generated
    uint8_t* ppkTemp = new uint8_t[ppkSize];
    memset(ppkTemp, 0, ppkSize);
    primaryKey->Export(ppkTemp);
    
    if ((authAlgorithm->Type() != Authentication::LMS_SHA2_256) && 
        (authAlgorithm->Type() != Authentication::LMS_SHAKE256) &&
        (authAlgorithm->Type() != Authentication::MLDSA) &&
        (authAlgorithm->Type() != Authentication::SLH_SHAKE256))
    {
        authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_E, RSA_4096_E_SIZE);
    }

    uint8_t* ppkHash = new uint8_t[hashLength];
    
    // Create PPK header for versal_2vp device format (similar to SPK header in GenerateSPKHash)
    HeaderAuthCert_versal_2vp ppkHeader;
    memset(&ppkHeader, 0, sizeof(ppkHeader));
    
    // For LMS/HSS, GetAuthHeader needs parameters to determine correct algorithm ID
    // lmsOnly flag determines: true = LMS (0x10), false = HSS (0x08)
    if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256) || 
        (authAlgorithm->Type() == Authentication::LMS_SHAKE256))
    {
        // For versal_2vp: IsLassenSeries=false, IsDl9Series=false (not applicable)
        ppkHeader.Algorithm = authAlgorithm->GetAuthHeader(lmsOnly, false, false);
    }
    else
    {
        ppkHeader.Algorithm = authAlgorithm->GetAuthHeader();
    }
    ppkHeader.RevocationID = 0x0;  // PPK revocation ID is always 0x0 (not spkIdentification!)
    ppkHeader.Hybrid       = 0x0;  // No hybrid algorithms for PPK
    ppkHeader.Authority    = 0x0;  // Reserved for CA/PA
    ppkHeader.Permission   = 0x0;  // BOOT permission - PPK has boot permission
    // ppkHeader.Reserved[3] is already zeroed by memset
    
    // Build buffer with PPK header + PPK key for versal_2vp devices
    uint8_t* tempBuffer = new uint8_t[sizeof(HeaderAuthCert_versal_2vp) + ppkSize];
    memcpy(tempBuffer, &ppkHeader, sizeof(HeaderAuthCert_versal_2vp));
    memcpy(tempBuffer + sizeof(HeaderAuthCert_versal_2vp), ppkTemp, ppkSize);
    
    // Calculate hash on header + key
    hash->CalculateVersalHash(true, tempBuffer, sizeof(HeaderAuthCert_versal_2vp) + ppkSize, ppkHash);
    
    LOG_TRACE("versal_2vp PPK HASH (Header + Key)");
    LOG_DUMP_BYTES(ppkHash, hashLength);
    LOG_TRACE("PPK Header:");
    LOG_DUMP_BYTES((uint8_t*)&ppkHeader, sizeof(HeaderAuthCert_versal_2vp));
    LOG_TRACE("PPK Key:");
    LOG_DUMP_BYTES(ppkTemp, ppkSize);
    LOG_TRACE("Final PPK plain content:");
    LOG_DUMP_BYTES(tempBuffer, sizeof(HeaderAuthCert_versal_2vp) + ppkSize);

    FILE* filePtr;
    if ((filePtr = fopen(filename.c_str(), "w")) == NULL)
    {
        LOG_ERROR("-efuseppkbits error !!!           Failure writing to hash file %s", StringUtils::BaseName(filename).c_str());
    }

    /* No Truncation on PPK Hash for Telluride, after ADR-278 */
    for (int index = 0; index < hashLength; index++)
    {
        fprintf(filePtr, "%02X", ppkHash[index]);
    }
    fprintf(filePtr, "\r\n");

    fclose(filePtr);
    LOG_INFO("Efuse PPK bits written to file %s successfully", filename.c_str());

    delete[] ppkTemp;
    delete[] tempBuffer;
}

/******************************************************************************/
void Versal_2vpAuthenticationContext::GeneratePPKHashWithMetadata(const std::string& filename, const KeyMetadata& metadata, const std::string& keyFile)
{
    LOG_INFO("Generating PPK Hash using .kmd metadata");
    
    // Determine PPK size based on algorithm type from metadata
    int ppkSize = VERSAL_ACKEY_STRUCT_SIZE;
    
    LOG_TRACE("Metadata Algorithm = 0x%x", metadata.Algorithm);
    
    // Check algorithm type from metadata and set appropriate ppkSize
    if (metadata.Algorithm == AUTH_HDR_TELLURIDE_ECDSA_P384)
        ppkSize = 2 * EC_P384_KEY_LENGTH;
    if (metadata.Algorithm == AUTH_HDR_TELLURIDE_ECDSA_P521)
        ppkSize = 2 * EC_P521_KEY_LENGTH2;
    if ((metadata.Algorithm == AUTH_HDR_TELLURIDE_HSS_LMS) || (metadata.Algorithm == AUTH_HDR_TELLURIDE_LMS))
    {
        hash = new HashSha3();
        // Use provided keyFile for hybrid case, otherwise use ppkFile member variable
        const char* keyPath = keyFile.empty() ? ppkFile.c_str() : keyFile.c_str();
        // Derive lmsOnly from metadata.Algorithm: HSS (0x08) = false, LMS (0x10) = true
        bool lmsOnlyFromMeta = (metadata.Algorithm == AUTH_HDR_TELLURIDE_LMS);
        ppkSize = GetLmsPublicKeyLength(keyPath, lmsOnlyFromMeta);
    }
    if (metadata.Algorithm == AUTH_HDR_TELLURIDE_ML_DSA87)
    {
        hash = new HashSha3();
        ppkSize = MLDSA_PUB_KEY_LENGTH;
    }
    if (metadata.Algorithm == AUTH_HDR_TELLURIDE_SLH_DSA)
    {
        hash = new HashSha3();
        ppkSize = SLHDSA_PUB_KEY_LENGTH;
    }
    hashLength = hash->GetHashLength();
    
    // Export PPK key - load the key based on algorithm type
    uint8_t* ppkTemp = new uint8_t[ppkSize];
    memset(ppkTemp, 0, ppkSize);
    
    // Use keyFile for hybrid (second algorithm), ppkFile for first algorithm
    std::string keyPath = keyFile.empty() ? ppkFile : keyFile;
    
    // For ECDSA and RSA, we need to create a temporary authentication context of the right type
    // to get a properly initialized Key object (with correct keySize, etc.)
    // For PQC algorithms (MLDSA/LMS/SLHDSA), create new key objects directly
    Key* loadedKey = NULL;
    bool useExistingKey = false;
    AuthenticationContext* tempAuthCtx = NULL;
    
    if (metadata.Algorithm == AUTH_HDR_TELLURIDE_ECDSA_P384)
    {
        // Create temporary auth context to get properly initialized ECDSA key
        LOG_TRACE("Creating temporary ECDSA P384 authentication context for key loading");
        tempAuthCtx = new Versal_2vpAuthenticationContext(Authentication::ECDSA);
        loadedKey = tempAuthCtx->primaryKey.get();
        LOG_TRACE("Calling ParseSecret for ECDSA P384 key from .pem file: %s", keyPath.c_str());
        loadedKey->ParseSecret(keyPath);  // Parse private key file, public key will be extracted automatically
        LOG_TRACE("ParseSecret completed, Loaded=%s", loadedKey->Loaded ? "true" : "false");
        useExistingKey = true;  // Will be deleted with tempAuthCtx
    }
    else if (metadata.Algorithm == AUTH_HDR_TELLURIDE_ECDSA_P521)
    {
        // Create temporary auth context to get properly initialized ECDSA P521 key
        LOG_TRACE("Creating temporary ECDSA P521 authentication context for key loading");
        tempAuthCtx = new Versal_2vpAuthenticationContext(Authentication::ECDSAp521);
        loadedKey = tempAuthCtx->primaryKey.get();
        LOG_TRACE("Calling ParseSecret for ECDSA P521 key from .pem file: %s", keyPath.c_str());
        loadedKey->ParseSecret(keyPath);  // Parse private key file, public key will be extracted automatically
        LOG_TRACE("ParseSecret completed, Loaded=%s", loadedKey->Loaded ? "true" : "false");
        useExistingKey = true;  // Will be deleted with tempAuthCtx
    }
    else if (metadata.Algorithm == AUTH_HDR_TELLURIDE_RSA_4096)
    {
        // Create temporary auth context to get properly initialized RSA key
        LOG_TRACE("Creating temporary RSA authentication context for key loading");
        tempAuthCtx = new Versal_2vpAuthenticationContext(Authentication::RSA);
        loadedKey = tempAuthCtx->primaryKey.get();
        LOG_TRACE("Calling ParseSecret for RSA key from .pem file: %s", keyPath.c_str());
        loadedKey->ParseSecret(keyPath);  // Parse private key file, public key will be extracted automatically
        LOG_TRACE("ParseSecret completed, Loaded=%s", loadedKey->Loaded ? "true" : "false");
        useExistingKey = true;  // Will be deleted with tempAuthCtx
    }
    else if (metadata.Algorithm == AUTH_HDR_TELLURIDE_ML_DSA87)
    {
        // Direct pointer usage - no casting needed
        KeyMLDSA_versal_2vp* mldsaKey = new KeyMLDSA_versal_2vp("Primary Key");
        mldsaKey->authType = Authentication::MLDSA;
        mldsaKey->isSecret = false;
        mldsaKey->ParseMldsaKey(keyPath);
        mldsaKey->Loaded = true;
        
        loadedKey = mldsaKey;  // Assign to base pointer
    }
    else if (metadata.Algorithm == AUTH_HDR_TELLURIDE_HSS_LMS || metadata.Algorithm == AUTH_HDR_TELLURIDE_LMS)
    {
        // Direct pointer usage - no casting needed
        KeyLMS_versal_2vp* lmsKey = new KeyLMS_versal_2vp("Primary Key");
        lmsKey->authType = (metadata.Algorithm == AUTH_HDR_TELLURIDE_LMS) ? 
                           Authentication::LMS_SHA2_256 : Authentication::LMS_SHAKE256;
        // Set lmsOnly based on metadata.Algorithm: LMS (0x10) = true, HSS (0x08) = false
        lmsKey->lmsOnly = (metadata.Algorithm == AUTH_HDR_TELLURIDE_LMS);
        lmsKey->isSecret = false;
        lmsKey->ParseLmsKey(keyPath);
        lmsKey->Loaded = true;
        
        loadedKey = lmsKey;  // Assign to base pointer
    }
    else if (metadata.Algorithm == AUTH_HDR_TELLURIDE_SLH_DSA)
    {
        // Direct pointer usage - no casting needed
        KeySLH_versal_2vp* slhKey = new KeySLH_versal_2vp("Primary Key");
        slhKey->authType = Authentication::SLH_SHAKE256;
        slhKey->isSecret = false;
        slhKey->ParseSLHdsaKey(keyPath);
        slhKey->Loaded = true;
        
        loadedKey = slhKey;  // Assign to base pointer
    }
    else
        LOG_ERROR("Unknown algorithm type in metadata: 0x%x", metadata.Algorithm);
    
    if (loadedKey)
    {
        // Verify key was loaded
        if (!loadedKey->Loaded) {
            LOG_ERROR("Failed to load key from %s (Algorithm 0x%x)", keyPath.c_str(), metadata.Algorithm);
        }
        
        LOG_TRACE("About to call Export() on loaded key, ppkTemp=%p, ppkSize=%d", ppkTemp, ppkSize);
        loadedKey->Export(ppkTemp);
        LOG_TRACE("Export() completed successfully");
        
        // Clean up: delete temporary auth context (which will delete its keys)
        // or delete standalone PQC key objects
        if (tempAuthCtx) {
            delete tempAuthCtx;  // This deletes primaryKey/secondaryKey too
        } else if (!useExistingKey) {
            delete loadedKey;
        }
    }
    
    // Rearrange endianness ONLY for RSA (not for ECDSA/LMS/MLDSA/SLHDSA)
    if (metadata.Algorithm == AUTH_HDR_TELLURIDE_RSA_4096)
    {
        authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_E, RSA_4096_E_SIZE);
    }

    uint8_t* ppkHash = new uint8_t[hashLength];
    
    // Create PPK header using metadata from .kmd file
    HeaderAuthCert_versal_2vp ppkHeader;
    memset(&ppkHeader, 0, sizeof(ppkHeader));
    
    ppkHeader.Algorithm = metadata.Algorithm;
    ppkHeader.RevocationID = metadata.RevocationID;
    ppkHeader.Hybrid = metadata.Hybrid;
    ppkHeader.Authority = metadata.Authority;
    ppkHeader.Permission = metadata.Permission;
    ppkHeader.Reserved[0] = metadata.Reserved[0];
    ppkHeader.Reserved[1] = metadata.Reserved[1];
    ppkHeader.Reserved[2] = metadata.Reserved[2];
    
    // Build buffer with PPK header + PPK key
    uint8_t* tempBuffer = new uint8_t[sizeof(HeaderAuthCert_versal_2vp) + ppkSize];
    memcpy(tempBuffer, &ppkHeader, sizeof(HeaderAuthCert_versal_2vp));
    memcpy(tempBuffer + sizeof(HeaderAuthCert_versal_2vp), ppkTemp, ppkSize);
    // Calculate hash on header + key
    hash->CalculateVersalHash(true, tempBuffer, sizeof(HeaderAuthCert_versal_2vp) + ppkSize, ppkHash);
    LOG_TRACE("PPK HASH (Header + Key) with metadata");
    LOG_DUMP_BYTES(ppkHash, hashLength);
    LOG_TRACE("PPK Header (from .kmd):");
    LOG_DUMP_BYTES((uint8_t*)&ppkHeader, sizeof(HeaderAuthCert_versal_2vp));
    LOG_TRACE("PPK Key:");
    LOG_DUMP_BYTES(ppkTemp, ppkSize);
    LOG_TRACE("Final PPK plain content:");
    LOG_DUMP_BYTES(tempBuffer, sizeof(HeaderAuthCert_versal_2vp) + ppkSize);

    FILE* filePtr;
    // Open in append mode for hybrid case (second algorithm)
    const char* mode = "w";
    if (!keyFile.empty())
    {
        // This is the second key (hybrid mode), append to existing file
        mode = "a";
    }

    if ((filePtr = fopen(filename.c_str(), mode)) == NULL)
    {
        LOG_ERROR("-efuseppkbits error !!!           Failure writing to hash file %s", StringUtils::BaseName(filename).c_str());
    }

    /* No Truncation on PPK Hash for Telluride, after ADR-278 */
    for (uint32_t index = 0; index < hashLength; index++)
    {
        fprintf(filePtr, "%02X", ppkHash[index]);
    }
    fprintf(filePtr, "\r\n");

    fclose(filePtr);
    LOG_INFO("Efuse PPK bits (with metadata) written to file %s successfully", filename.c_str());

    delete[] ppkTemp;
    delete[] tempBuffer;
    delete[] ppkHash;
}

/******************************************************************************/
void RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
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
    uint8_t *salt = NULL;
    salt = new uint8_t[SALT_LENGTH];

    #ifdef DEBUG
    if (rsaLocalENV != NULL) {
        uint8_t defsalt[SALT_LENGTH] = { 0x71, 0x5d, 0x74, 0xeb, 0x06, 0xcb, 0x2f, 0xfa, 0xed, 0x09, 0x3d, 0xd3, 0x9b, 0xa0, 0xb8, 0x57, 0xa3, 0x01, 0xcd, 0xd7, 0x52, 0x2a, 0x82, 0xc9, 0x71, 0x06, 0x8a, 0x6b, 0xb1, 0x99, 0x83, 0x52, 0x49, 0x49, 0x2b, 0xdb, 0xc8, 0x50, 0x20, 0x5e, 0x44, 0xb2, 0xc8, 0xbe, 0xbf, 0x77, 0x98, 0x22 };
        memcpy(salt, defsalt, SALT_LENGTH);
    }
    else 
    #endif
    {
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
    Versal_2vpcrypto_hash(mPadHash, m1, 208, false);
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
    delete[] m1;
    delete[] DB;
    delete[] salt;
}

/******************************************************************************/
void ECDSAHBAuthenticationAlgorithm_versal_2vp::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    memcpy(signature, hash, hashLength);
}

/******************************************************************************/
void ECDSAP521HBAuthenticationAlgorithm_versal_2vp::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    memcpy(signature, hash, hashLength);
}

/******************************************************************************/
void LMSAuthenticationAlgorithm_versal_2vp::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    memcpy(signature, hash, hashLength);
}
/******************************************************************************/
void MLDSAAuthenticationAlgorithm_versal_2vp::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    memcpy(signature, hash, hashLength);
}

/******************************************************************************/
void SLHAuthenticationAlgorithm_versal_2vp::CreatePadding(uint8_t * signature, uint8_t * hash, uint8_t hashLength)
{
    memcpy(signature, hash, hashLength);
}

/******************************************************************************/
void Versal_2vpAuthenticationContext::CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection)
{
    LOG_TRACE("Copying the partition (%s) signature into Authentication Certificate", acSection->Name.c_str());
    LOG_TRACE("CopyPartitionSignature: signatureBlock=%p, hashLength=%u, signatureLength=%u", 
             signatureBlock, hashLength, signatureLength);

    /* calculate hash first */
    if (hashLength == 0 || hashLength > 1024) {
        LOG_ERROR("Invalid hashLength: %u", hashLength);
        return;
    }
    uint8_t* shaHash = new uint8_t[hashLength];
    std::list<Section*>::iterator section = sections.begin();

    /* Calculate the final hash */
    size_t hashBlockLength = bi.hashBlockLength;
    if (sections.front()->Name == "Headers")
    {
        hashBlockLength = bi.imageHeaderTable->hashBlockSectionLength;
        LOG_TRACE("Headers: hashBlockSectionLength=%u, hashBlockSection->Length=%zu", 
                 bi.imageHeaderTable->hashBlockSectionLength, bi.imageHeaderTable->hashBlockSection->Length);
    }
    else if (sections.front()->Name == "HashBlock")
    {
        hashBlockLength = sections.front()->Length;
        LOG_TRACE("Owner HashBlock: signing own length=%zu bytes", hashBlockLength);
    }
    LOG_TRACE("Allocating hashBlock with size %zu", hashBlockLength);

    uint8_t *hashBlock = new uint8_t[hashBlockLength];
    memset(hashBlock, 0, hashBlockLength);

    if (sections.front()->Name == "Headers")
        memcpy(hashBlock, bi.imageHeaderTable->hashBlockSection->Data.get(), hashBlockLength);
    else
        memcpy(hashBlock, (*section)->Data.get(), hashBlockLength);
        
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        // Get lmsOnly flag from secondaryKey (which stores the correct HSS/LMS mode)
        bool lmsOnlyForPartSig = true;  // Default to LMS (single-level)
        if (secondaryKey != nullptr) {
            lmsOnlyForPartSig = secondaryKey->lmsOnly;
            LOG_TRACE("Retrieved lmsOnly flag from secondaryKey for partition signature: %s (true=LMS/2640, false=HSS/2644)", 
                     lmsOnlyForPartSig ? "true" : "false");
        } else if (primaryKey != nullptr) {
            lmsOnlyForPartSig = primaryKey->lmsOnly;
            LOG_TRACE("Retrieved lmsOnly flag from primaryKey for partition signature: %s (true=LMS/2640, false=HSS/2644)", 
                     lmsOnlyForPartSig ? "true" : "false");
        } else {
            LOG_WARNING("No key available to retrieve lmsOnly flag, using default lmsOnly=true for partition signature");
        }
        
        signatureLength = GetLmsSignLength(sskFile.c_str(), lmsOnlyForPartSig);
        LOG_TRACE("LMS signature length with key %s - %d, -%d", sskFile.c_str(), 
                 (int)GetLmsSignLength(sskFile.c_str(), true), -(int)GetLmsSignLength(sskFile.c_str(), lmsOnlyForPartSig));
    }
    else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
    {
        signatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
    }
    else
    {
        Versal_2vpcrypto_hash(shaHash, hashBlock, hashBlockLength, true);
    }
    LOG_TRACE("Hash Block Data for %s (length=%zu):", acSection->Name.c_str(), hashBlockLength);
    LOG_DUMP_BYTES(hashBlock, hashBlockLength);
    LOG_TRACE("Creating PKCS padding: signatureLength=%u", signatureLength);
    /* Create the PKCS padding for the hash */
    uint8_t* shaHashPadded = new uint8_t[signatureLength];
    memset(shaHashPadded, 0, signatureLength);
    authAlgorithm->CreatePadding(shaHashPadded, shaHash, hashLength);

    /* Generate hashes, if requested from command line option "-generate_hashes" */
    if (bi.options.DoGenerateHashes())
    {
        std::string hashfilename = acSection->Name;
        if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 ||
            authAlgorithm->Type() == Authentication::LMS_SHAKE256 ||
            authAlgorithm->Type() == Authentication::MLDSA ||
            authAlgorithm->Type() == Authentication::SLH_SHAKE256)
        {
            WritePaddedSHAFile(hashBlock, hashBlockLength, hashfilename);   // raw message
        }
        else
        {
            WritePaddedSHAFile(shaHashPadded, hashfilename);                // padded SHA (RSA/ECDSA)
        }
    }
    authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);
#ifdef DEBUG
    LOG_TRACE("Hash Block Data for %s (length=%zu):", acSection->Name.c_str(), hashBlockLength);
    LOG_DUMP_BYTES(hashBlock, hashBlockLength);
#endif
    /* Now sign the hash */
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        // Get lmsOnly flag from secondaryKey for signing (already retrieved above)
        bool lmsOnlyForSigning = true;
        if (secondaryKey != nullptr) {
            lmsOnlyForSigning = secondaryKey->lmsOnly;
        } else if (primaryKey != nullptr) {
            lmsOnlyForSigning = primaryKey->lmsOnly;
        }
        
        authAlgorithm->CreateSignature(hashBlock, hashBlockLength, sskFile.c_str(), 
                signatureBlock, signatureLength, lmsOnlyForSigning, spkFile.c_str());
        authAlgorithm->VerifySignature(hashBlock, hashBlockLength, spkFile.c_str(), signatureBlock, signatureLength, lmsOnlyForSigning);
    }
    if (authAlgorithm->Type() == Authentication::MLDSA)
    {
        // Use the variable-length version that passes hashBlockLength
        // This is critical for HashBlock1 which has variable size based on partition count
        LOG_TRACE("ML-DSA: Using variable-length signature for hashBlockLength=%zu bytes", hashBlockLength);
        
        // Cast to MLDSAAuthenticationAlgorithm_versal_2vp to access the variable-length methods
        MLDSAAuthenticationAlgorithm_versal_2vp* mldsaAlg = dynamic_cast<MLDSAAuthenticationAlgorithm_versal_2vp*>(authAlgorithm.get());
        if (mldsaAlg) {
            mldsaAlg->CreateSignature(hashBlock, hashBlockLength, (uint8_t*)secondaryKey.get(), signatureBlock);
            mldsaAlg->VerifySignature(hashBlock, hashBlockLength, (uint8_t*)secondaryKey.get(), signatureBlock);
        } else {
            LOG_ERROR("Failed to cast authAlgorithm to MLDSAAuthenticationAlgorithm_versal_2vp for variable-length signature");
            // Fall back to fixed-length (will be incorrect for HashBlock1 with >6 partitions)
            authAlgorithm->CreateSignature(hashBlock, (uint8_t*)secondaryKey.get(), signatureBlock, false);
            authAlgorithm->VerifySignature(hashBlock, (uint8_t*)secondaryKey.get(), signatureBlock, false);
        }
    }
    else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
    {
        size_t sig_len = signatureLength;
        authAlgorithm->CreateSignature(hashBlock, hashBlockLength, sskFile.c_str(), signatureBlock, sig_len);
        signatureLength = sig_len;

        authAlgorithm->VerifySignature(hashBlock, hashBlockLength, signatureBlock, spkFile.c_str());
    }
    else
    {
        authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)secondaryKey.get(), signatureBlock);
    }
    
    authAlgorithm->RearrangeEndianess(signatureBlock, signatureLength);
#ifdef DEBUG
    LOG_TRACE("The partition signature is copied into Authentication Certificate");
    LOG_DUMP_BYTES(signatureBlock, signatureLength);
#endif
    /* Delete the temporarily created arrays */
    delete[] shaHash;
    delete[] shaHashPadded;
    acIndex++;
}

/******************************************************************************/
void RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp::RearrangeEndianess(uint8_t *array, uint32_t size)
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
void ECDSAHBAuthenticationAlgorithm_versal_2vp::RearrangeEndianess(uint8_t *array, uint32_t size)
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
void ECDSAP521HBAuthenticationAlgorithm_versal_2vp::RearrangeEndianess(uint8_t *array, uint32_t size)
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
void ECDSAHBAuthenticationAlgorithm_versal_2vp::ECDSASignature(const uint8_t *base, EC_KEY *eckeyU, uint8_t *result0)
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
void ECDSAP521HBAuthenticationAlgorithm_versal_2vp::ECDSASignature(const uint8_t *base, EC_KEY *eckeyU, uint8_t *result0)
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
void Versal_2vpAuthenticationContext::AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header)
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
void Versal_2vpAuthenticationContext::GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index)
{
    /* signature lengths are different for differnt alogrithms */
    GetPresign(presignFilename, SIGN_LENGTH_VERSAL, signature, index);
}

/******************************************************************************/
void Versal_2vpAuthenticationContext::GetPresign(const std::string& presignFilename, uint16_t signatureLength, uint8_t* signature, uint32_t index)
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
                LOG_ERROR("Authentication Error !!!\n           Presign file %s should be of %d bytes", baseFile.c_str(), signatureLength);
            }
            fclose(filePtr);
        }
        else
        {
            // read ascii
            filePtr = fopen(filename.c_str(), "r");
            for (int i = 0; i < signatureLength; i++)
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
void Versal_2vpAuthenticationContext::SetSPKSignatureFile(const std::string& filename)
{
    spkSignFile = filename;
    if (filename.empty())
    {
        return;
    }

    FILE* filePtr;
    filePtr = fopen(filename.c_str(), "r");

    if (filePtr)
    {
        fclose(filePtr);
        GetPresign(filename, spksignature.get(), 0);
        spkSignLoaded = true;
        spkSignRequested = "";
    }
    else
    {
        LOG_ERROR("Failure reading SPK Signature file - %s", StringUtils::BaseName(filename).c_str());
    }
}

/******************************************************************************/
void Versal_2vpAuthenticationContext::SetBHSignatureFile(const std::string& filename)
{
    bhSignFile = filename;
    if (filename.empty())
    {
        return;
    }

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
void Versal_2vpAuthenticationContext::GenerateSPKSignature(const std::string& filename)
{
    if (primaryKey->Loaded && primaryKey->isSecret)
    {
        uint8_t* shaHashPadded = new uint8_t[signatureLength];
        uint8_t* spkSignatureTemp = new uint8_t[signatureLength];
        memset(shaHashPadded, 0, signatureLength);
        memset(spkSignatureTemp, 0, signatureLength);
        GenerateSPKHash(shaHashPadded);
        authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);
        authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)primaryKey.get(), spkSignatureTemp);
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
void Versal_2vpAuthenticationContext::ResizeIfNecessary(Section* section)
{
    // round up to next 64 byte boundary.
    uint32_t dataSizePadded = (section->Length + 63) & 0xFFFFFFC0;
    section->IncreaseLengthAndPadTo(dataSizePadded, 0xFF);
}

/******************************************************************************/
void Versal_2vpAuthenticationContext::LoadUdfData(const std::string& udfFilename, uint8_t* signature)
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
void Versal_2vpAuthenticationContext::CreateSPKSignature(BootImage& bi)
{
    CreateSPKSignature(bi, 0x0);  // Default to no hybrid for backward compatibility
}

/******************************************************************************/
void Versal_2vpAuthenticationContext::CreateSPKSignature(BootImage& bi, uint32_t hybridMask)
{
    LOG_TRACE("Creating the SPK signature");

    // Note : for SPK singnature generation we should pass only SPK header + SPK key(actual key) without padding as inputs 
    Section* dataSection = GetCurrentDataSection();
    uint8_t* shaHashPadded = NULL;

    // Retrieve .kmd metadata for the owning partition. MUST use the same lookup as
    // Auto-detect which algorithm we are (Algorithm 1 or Algorithm 2) based on authType/authType1
    KeyMetadata secondaryMeta;
    PartitionBifOptions* partBifOptionsForFlags = FindOwningPartitionBifOptions(bi, dataSection);

    if (partBifOptionsForFlags != nullptr) {
        // Determine which metadata to use based on the current authentication algorithm
        // In hybrid mode, we need to check if current algorithm matches hybridAlgorithms[0] or [1]
        Authentication::Type currentAuthType = authAlgorithm->Type();
        
        // Get hybrid algorithms array
        std::vector<Authentication::Type> hybridAlgorithms = partBifOptionsForFlags->GetHybridAlgorithms();
        bool isHybridPartition = (hybridAlgorithms.size() == 2);
        
        if (isHybridPartition) {
            // Hybrid mode: determine if we're Algorithm 1 or Algorithm 2
            if (currentAuthType == hybridAlgorithms[0]) {
                // Current algorithm matches hybridAlgorithms[0] → use secondaryMetadata (Algorithm 1)
                secondaryMeta = partBifOptionsForFlags->secondaryMetadata;
                LOG_TRACE("Hybrid Mode: Retrieved SPK .kmd metadata for Algorithm 1 (%s) - secondaryMeta.isValid=%d", 
                         GetAlgorithmName(currentAuthType).c_str(), secondaryMeta.isValid);
            } else if (currentAuthType == hybridAlgorithms[1]) {
                // Current algorithm matches hybridAlgorithms[1] → use secondaryMetadata1 (Algorithm 2)
                secondaryMeta = partBifOptionsForFlags->secondaryMetadata1;
                LOG_TRACE("Hybrid Mode: Retrieved SPK .kmd metadata for Algorithm 2 (%s) - secondaryMeta1.isValid=%d", 
                         GetAlgorithmName(hybridAlgorithms[1]).c_str(), secondaryMeta.isValid);
            } else {
                // Shouldn't happen, but fallback to secondaryMetadata
                LOG_WARNING("Hybrid Mode: Current algorithm (%s) doesn't match hybridAlgorithms[0] (%s) or [1] (%s), using default secondaryMetadata", 
                           GetAlgorithmName(currentAuthType).c_str(), 
                           GetAlgorithmName(hybridAlgorithms[0]).c_str(),
                           GetAlgorithmName(hybridAlgorithms[1]).c_str());
                secondaryMeta = partBifOptionsForFlags->secondaryMetadata;
            }
        } else {
            // Single signing mode: use secondaryMetadata
            secondaryMeta = partBifOptionsForFlags->secondaryMetadata;
            LOG_TRACE("Single Mode: Retrieved SPK .kmd metadata - secondaryMeta.isValid=%d", secondaryMeta.isValid);
        }
    } else {
        secondaryMeta.isValid = false;
        LOG_TRACE("No PartitionBifOptions found - using default SPK header values");
    }

    if (primaryKey->Loaded && primaryKey->isSecret)
    {
        if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
        {
            // Get lmsOnly flag from primaryKey (which stores the correct HSS/LMS mode)
            bool lmsOnlyForSPK = true;  // Default to LMS (single-level)
            if (primaryKey != nullptr) {
                lmsOnlyForSPK = primaryKey->lmsOnly;
                LOG_TRACE("Retrieved lmsOnly flag from primaryKey for SPK signature: %s (true=LMS/2640, false=HSS/2644)", 
                         lmsOnlyForSPK ? "true" : "false");
            } else {
                LOG_WARNING("primaryKey is null, using default lmsOnly=true for SPK signature");
            }
            
            // LMS/HSS - Use NEW header format with proper padding
            HeaderAuthCert_versal_2vp spkHeader;
            memset(&spkHeader, 0, sizeof(spkHeader));
            
            // Fill SPK header using .kmd metadata (if available) or defaults (backward compatibility)
            PopulateHeaderFromKmdMetadata(
                &spkHeader,
                secondaryMeta,  // .kmd metadata for SPK signature
                authAlgorithm->GetAuthHeader(lmsOnlyForSPK, false, bi.options.IsDl9Series()),  // default algorithm
                spkIdentification,  // default revocation ID
                hybridMask,  // default hybrid mask (from parameter)
                0x0,  // default authority
                0x1  // default permission
            );
            
            // Use totalKeySize for proper alignment (matching GenerateSPKHash)
            size_t actualKeySize = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnlyForSPK);
            size_t totalKeySize = actualKeySize + PADDING_16B(actualKeySize); // 16-byte alignment
            
            uint8_t* spkFull = new uint8_t[actualKeySize];
            memset(spkFull, 0, actualKeySize); // Zero entire buffer including padding
            
            if (!secondaryKey->Loaded)
            {
                ParseSPKeyFile(spkFile);
            }
            secondaryKey->Export(spkFull); // Export actual key (padding remains zero)
            
            // Build buffer with NEW SPK header + SPK key (with padding)
            uint8_t* tempBuffer = new uint8_t[sizeof(HeaderAuthCert_versal_2vp) + actualKeySize];
            
            // Write NEW SPK header
            memcpy(tempBuffer, &spkHeader, sizeof(HeaderAuthCert_versal_2vp));
            
            // Write SPK key data (with padding)
            memcpy(tempBuffer + sizeof(HeaderAuthCert_versal_2vp), spkFull, actualKeySize);
            
            // Update signatureLength to actual value from key file before signing
            signatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnlyForSPK);
            LOG_TRACE("LMS signature length with key %s - %d", pskFile.c_str(), signatureLength);
            
            // Sign the combined buffer (NEW header + padded key)
            authAlgorithm->CreateSignature(tempBuffer, sizeof(HeaderAuthCert_versal_2vp) + actualKeySize, 
                                        pskFile.c_str(), spksignature.get(), signatureLength, lmsOnlyForSPK, ppkFile.c_str());
                                        
            // Add verification
            authAlgorithm->VerifySignature(tempBuffer, sizeof(HeaderAuthCert_versal_2vp) + actualKeySize, ppkFile.c_str(),
                                        spksignature.get(), signatureLength, lmsOnlyForSPK);
            delete[] spkFull;
            delete[] tempBuffer;
        }
		else if (authAlgorithm->Type() == Authentication::MLDSA)
        {
              // 1. Prepare SPK header using .kmd metadata (if available) or defaults (backward compatibility)
            HeaderAuthCert_versal_2vp spkHeader;
            memset(&spkHeader, 0, sizeof(spkHeader));
            PopulateHeaderFromKmdMetadata(
                &spkHeader,
                secondaryMeta,  // .kmd metadata for SPK signature
                authAlgorithm->GetAuthHeader(),  // default algorithm
                spkIdentification,  // default revocation ID
                hybridMask,  // default hybrid mask (from parameter)
                0x0,  // default authority
                0x1  // default permission
            );

            // 2. Prepare SPK key
            size_t spkKeyLen = MLDSA_PUB_KEY_LENGTH;
            uint8_t* spkKey = new uint8_t[spkKeyLen];
            memset(spkKey, 0, spkKeyLen);
            if (!secondaryKey->Loaded)
            {
                ParseSPKeyFile(spkFile);
            }
            secondaryKey->Export(spkKey);

            // 3. Build buffer: [SPK header][SPK key]
            size_t bufferLen = sizeof(HeaderAuthCert_versal_2vp) + spkKeyLen;
            uint8_t* buffer = new uint8_t[bufferLen];
            memcpy(buffer, &spkHeader, sizeof(HeaderAuthCert_versal_2vp));
            memcpy(buffer + sizeof(HeaderAuthCert_versal_2vp), spkKey, spkKeyLen);

            LOG_TRACE("SPK Header + SPK Key for MLDSA signature:");
            LOG_DUMP_BYTES(buffer, bufferLen);

            authAlgorithm->CreateSignature(buffer, (uint8_t*)primaryKey.get(), spksignature.get(), true);
            
            // Verify the signature using the primary public key (PPK) since we signed with PSK
            authAlgorithm->VerifySignature(buffer, (uint8_t*)primaryKey.get(), spksignature.get(), true);

            delete[] spkKey;
            delete[] buffer;
        }
        else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
        {
            // 1. Prepare SPK header using .kmd metadata (if available) or defaults (backward compatibility)
            HeaderAuthCert_versal_2vp spkHeader;
            memset(&spkHeader, 0, sizeof(spkHeader));
            PopulateHeaderFromKmdMetadata(
                &spkHeader,
                secondaryMeta,  // .kmd metadata for SPK signature
                authAlgorithm->GetAuthHeader(),  // default algorithm
                spkIdentification,  // default revocation ID
                hybridMask,  // default hybrid mask (from parameter)
                0x0,  // default authority
                0x1  // default permission
            );

            // 2. Prepare SPK key
            size_t spkKeyLen = SLHDSA_PUB_KEY_LENGTH;
            uint8_t* spkKey = new uint8_t[spkKeyLen];
            memset(spkKey, 0, spkKeyLen);
            if (!secondaryKey->Loaded)
            {
                ParseSPKeyFile(spkFile);
            }
            secondaryKey->Export(spkKey);

            // 3. Build buffer: [SPK header][SPK key]
            size_t bufferLen = sizeof(HeaderAuthCert_versal_2vp) + spkKeyLen;
            uint8_t* buffer = new uint8_t[bufferLen];
            memcpy(buffer, &spkHeader, sizeof(HeaderAuthCert_versal_2vp));
            memcpy(buffer + sizeof(HeaderAuthCert_versal_2vp), spkKey, spkKeyLen);

            LOG_TRACE("SPK Header + SPK Key for SLHDSA signature:");
            LOG_DUMP_BYTES(buffer, bufferLen);

            size_t sig_len = signatureLength;
            authAlgorithm->CreateSignature(buffer, bufferLen, pskFile.c_str(), spksignature.get(), sig_len);
            signatureLength = sig_len;

            authAlgorithm->VerifySignature(buffer, bufferLen, spksignature.get(), ppkFile.c_str());
            
            delete[] spkKey;
            delete[] buffer;
        }
        else
        {
   
            // RSA/ECDSA/ECDSAp521 - Use NEW header format with .kmd metadata (if available) or defaults
            HeaderAuthCert_versal_2vp spkHeader;
            memset(&spkHeader, 0, sizeof(spkHeader));
            PopulateHeaderFromKmdMetadata(
                &spkHeader,
                secondaryMeta,  // .kmd metadata for SPK signature
                authAlgorithm->GetAuthHeader(),  // default algorithm
                spkIdentification,  // default revocation ID
                hybridMask,  // default hybrid mask (from parameter)
                0x0,  // default authority
                0x1  // default permission
            );
            
            // Get key sizes based on algorithm (matching GenerateSPKHash pattern)
            size_t actualKeySize = 0;
            size_t totalKeySize = 0;
            
            if (authAlgorithm->Type() == Authentication::RSA)
            {
                actualKeySize = VERSAL_ACKEY_STRUCT_SIZE;
                totalKeySize = actualKeySize; // RSA doesn't need extra padding
            }
            else if (authAlgorithm->Type() == Authentication::ECDSA)
            {
                actualKeySize = 2 * EC_P384_KEY_LENGTH;
                totalKeySize = actualKeySize; // ECDSA doesn't need extra padding
            }
            else if (authAlgorithm->Type() == Authentication::ECDSAp521)
            {
                actualKeySize = 2 * EC_P521_KEY_LENGTH2;
                totalKeySize = actualKeySize; // ECDSAp521 doesn't need extra padding
            }
            else
            {
                LOG_ERROR("Unsupported authentication algorithm for SPK signature creation");
                return;
            }
            
            uint8_t* spkFull = new uint8_t[actualKeySize];
            memset(spkFull, 0, actualKeySize);
            
            if (!secondaryKey->Loaded)
            {
                ParseSPKeyFile(spkFile);
            }
            secondaryKey->Export(spkFull);
            
            // For RSA, rearrange endianness
            if (authAlgorithm->Type() == Authentication::RSA)
            {
                authAlgorithm->RearrangeEndianess(spkFull + RSA_4096_N, RSA_4096_N_SIZE);
                authAlgorithm->RearrangeEndianess(spkFull + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
                authAlgorithm->RearrangeEndianess(spkFull + RSA_4096_E, RSA_4096_E_SIZE);
            }
            
            // Build buffer with NEW SPK header + SPK key
            uint8_t* tempBuffer = new uint8_t[sizeof(HeaderAuthCert_versal_2vp) + actualKeySize];
            
            // Write NEW SPK header
            memcpy(tempBuffer, &spkHeader, sizeof(HeaderAuthCert_versal_2vp));
            
            // Write SPK key data
            memcpy(tempBuffer + sizeof(HeaderAuthCert_versal_2vp), spkFull, actualKeySize);
            
            // Calculate hash of the combined buffer
            uint8_t* shaHash = new uint8_t[hashLength];
            hash->CalculateVersalHash(true, tempBuffer, sizeof(HeaderAuthCert_versal_2vp) + actualKeySize, shaHash);
            LOG_INFO("Calculating the SPK hash (before PKCS padding):");
            LOG_DUMP_BYTES(shaHash, hashLength);
            
            // Create PKCS padding and sign
            shaHashPadded = new uint8_t[signatureLength];
            memset(shaHashPadded, 0, signatureLength);
            authAlgorithm->CreatePadding(shaHashPadded, shaHash, hashLength);
            authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);
            authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)primaryKey.get(), spksignature.get());
            
            delete[] shaHash;
            delete[] spkFull;
            delete[] tempBuffer;
        }

        authAlgorithm->RearrangeEndianess(spksignature.get(), signatureLength);
        if (shaHashPadded) 
            delete[] shaHashPadded;
    }
    else if (!spkSignLoaded)
    {
        LOG_ERROR("Authentication Error !!!\n          PSK or SPK signature missing, must specify atleast one");
    }
    LOG_TRACE("SPK Signature:");
    LOG_DUMP_BYTES(spksignature.get(), signatureLength);
    LOG_INFO("SPK signature created successfully");    

}

/******************************************************************************/
uint32_t ECDSAHBAuthenticationAlgorithm_versal_2vp::GetAuthHeader(void)
{
    return AUTH_HDR_TELLURIDE_ECDSA_P384;
}

/******************************************************************************/
uint32_t ECDSAP521HBAuthenticationAlgorithm_versal_2vp::GetAuthHeader(void)
{
    return AUTH_HDR_TELLURIDE_ECDSA_P521;
}

/******************************************************************************/
uint32_t RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp::GetAuthHeader(void)
{
    return AUTH_HDR_TELLURIDE_RSA_4096;
}

/******************************************************************************/
uint32_t MLDSAAuthenticationAlgorithm_versal_2vp::GetAuthHeader(void)
{
    return AUTH_HDR_TELLURIDE_ML_DSA87;
}

/******************************************************************************/
uint32_t SLHAuthenticationAlgorithm_versal_2vp::GetAuthHeader(void)
{
    return AUTH_HDR_TELLURIDE_SLH_DSA;
}

/******************************************************************************/
uint32_t LMSAuthenticationAlgorithm_versal_2vp::GetAuthHeader(bool lmsOnly, bool IsLassenSeries, bool IsDl9Series)
{   
    if(!IsLassenSeries)
    {
        if (lmsOnly)
            return  AUTH_HDR_TELLURIDE_LMS;
        else
            return AUTH_HDR_TELLURIDE_HSS_LMS;
    }
    else
    {
        if(IsDl9Series)
            if (lmsOnly)
                return  AUTH_HDR_TELLURIDE_LMS;
            else
                return AUTH_HDR_TELLURIDE_HSS_LMS;
        else
            return AUTH_HDR_LASSEN_HSS_LMS;
    }
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
/* Writes the message-header fields common to every algorithm: id word, and the
   secure-debug inputs (device-DNA attributes/value and jtag timeout). The
   algorithm id and revocation id are carried in the ppkheader, not the header. */
static void WriteAuthJtagCommonHeader(uint8_t* buffer, const AuthJtagInfo& authJtagAttributes)
{
    WriteLittleEndian32(buffer + AUTH_JTAG_V2VP_ID_WORD_OFFSET, AUTH_JTAG_IMAGE_IDENTIFICATION_WORD);

    if (!authJtagAttributes.userRevokeId)
    {
        LOG_WARNING("revoke_id is not specified in BIF, default revoke id is assigned as '0'.");
    }

    if (authJtagAttributes.userDeviceDNA)
    {
        WriteLittleEndian32(buffer + AUTH_JTAG_V2VP_ATTRIBUTES_OFFSET, authJtagMessagenMask << authJtagMessageShift);
    }
    SetDeviceDNA(authJtagAttributes.deviceDNA, (uint32_t*)(buffer + AUTH_JTAG_V2VP_DEVICE_DNA_OFFSET));
    WriteLittleEndian32(buffer + AUTH_JTAG_V2VP_JTAG_TIMEOUT_OFFSET, authJtagAttributes.jtagTimeout);
}

/******************************************************************************/
/* Populates the versal_2vp PPK header (HeaderAuthCert): Hybrid/Authority = 0, no KMD. */
static void WriteAuthJtagPpkHeader(HeaderAuthCert_versal_2vp* hdr, uint32_t algorithm,
                                   uint32_t revocationId, uint32_t permission)
{
    WriteLittleEndian32(&hdr->Algorithm, algorithm);
    WriteLittleEndian32(&hdr->RevocationID, revocationId);
    WriteLittleEndian32(&hdr->Hybrid, 0);
    WriteLittleEndian32(&hdr->Authority, 0);
    WriteLittleEndian32(&hdr->Permission, permission);
    memset(hdr->Reserved, 0, sizeof(hdr->Reserved));
}

/******************************************************************************/
/* Reads a pre-signed signature of arbitrary length (binary, with ascii-hex
   fallback) for offline signing via the jtag_signature file. */
static void ReadAuthJtagPresign(const std::string& presignFilename, uint8_t* signature, size_t length)
{
    std::string baseFile = StringUtils::BaseName(presignFilename);
    FILE* filePtr = fopen(presignFilename.c_str(), "rb");
    if (!filePtr)
    {
        LOG_ERROR("Failure opening presign file - %s", baseFile.c_str());
        return;
    }
    fseek(filePtr, 0, SEEK_END);
    long size = ftell(filePtr);
    fseek(filePtr, 0, SEEK_SET);
    if (size == (long)length)
    {
        long read_size = fread(signature, 1, length, filePtr);
        fclose(filePtr);
        if (read_size != (long)length)
        {
            LOG_ERROR("Authentication Error !!!\n           Presign file %s should be of %lu bytes", baseFile.c_str(), (unsigned long)length);
        }
    }
    else
    {
        fclose(filePtr);
        filePtr = fopen(presignFilename.c_str(), "r");
        if (!filePtr)
        {
            LOG_ERROR("Failure opening presign file - %s", baseFile.c_str());
            return;
        }
        for (size_t i = 0; i < length; i++)
        {
            int x;
            if (fscanf(filePtr, "%2X", &x) != 1)
            {
                fclose(filePtr);
                LOG_ERROR("Failure reading presign file - %s", baseFile.c_str());
            }
            signature[i] = (uint8_t)x;
        }
        fclose(filePtr);
    }
}

/******************************************************************************/
uint32_t Versal_2vpAuthenticationContext::GetAuthJtagImageSize(void) const
{
    /* versal_2vp secure-debug is PPK-only: message header + HeaderAuthCert ppkheader +
       PPK + a single PSK signature (no SPK / SPK signature). */
    if (authAlgorithm->Type() == Authentication::RSA)
    {
        return sizeof(AuthJtagRSAImageStructure_versal2vp);
    }
    else if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        return sizeof(AuthJtagECP384ImageStructure_versal2vp);
    }
    else if (authAlgorithm->Type() == Authentication::ECDSAp521)
    {
        return sizeof(AuthJtagECP521ImageStructure_versal2vp);
    }
    else if (authAlgorithm->Type() == Authentication::MLDSA)
    {
        return sizeof(AuthJtagMLDSAImageStructure_versal2vp);
    }
    else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
    {
        return sizeof(AuthJtagSLHDSAImageStructure_versal2vp);
    }
    else if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        size_t actualppkSize = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly);
        size_t authJtagSignLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);

        size_t totalSize = AUTH_JTAG_V2VP_FIXED_HEADER_SIZE
            + sizeof(HeaderAuthCert_versal_2vp)
            + actualppkSize + PADDING_16B(actualppkSize)
            + authJtagSignLength;

        return (uint32_t)totalSize;
    }
    return sizeof(AuthJtagRSAImageStructure_versal2vp);
}

/******************************************************************************/
/* Shared body for the fixed-layout ECDSA auth-JTAG variants (P-384 / P-521).
   The two curves differ only by the image struct type and the key length
   (2*curve size), so both are handled here to avoid duplicating the signing flow. */
template <typename ImageStruct>
void Versal_2vpAuthenticationContext::CreateEcdsaAuthJtagImage(Options& options, uint8_t* buffer, const AuthJtagInfo& authJtagAttributes, uint32_t keyBytes)
{
    ImageStruct* authJtagImage = (ImageStruct*)buffer;
    if (!primaryKey->Loaded)
    {
        LOG_ERROR("Authentication Error !!!\n          PSK must be specified to generate Authenticated Jtag Image");
        return;
    }

    WriteAuthJtagCommonHeader(buffer, authJtagAttributes);

    hashLength = hash->GetHashLength();
    auto shaHash = std::make_unique<uint8_t[]>(hashLength);
    auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
    memset(shaHash.get(), 0, hashLength);
    memset(shaHashPadded.get(), 0, signatureLength);

    WriteAuthJtagPpkHeader(&authJtagImage->ppkheader, authAlgorithm->GetAuthHeader(),
                           authJtagAttributes.partitionRevokeId, AUTH_JTAG_PPK_PERMISSION);

    /* ECDSA keys are exported directly (Qx||Qy); no RSA-style N/N_ext/E endianness rearrange needed. */
    primaryKey->Export(authJtagImage->acPpk);

    /* PPK-only: PSK signs the whole message minus the trailing signature. */
    hash->CalculateVersalHash(true, (uint8_t*)authJtagImage, sizeof(ImageStruct) - signatureLength, shaHash.get());
    authAlgorithm->CreatePadding(shaHashPadded.get(), shaHash.get(), hashLength);

    if (primaryKey->isSecret)
    {
        authAlgorithm->RearrangeEndianess(shaHashPadded.get(), signatureLength);
        authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)primaryKey.get(), authJtagImage->authJtagSignature);
        authAlgorithm->RearrangeEndianess(authJtagImage->authJtagSignature, signatureLength);
    }
    else if (authJtagAttributes.jtagSignatureFile != "")
    {
        ReadAuthJtagPresign(authJtagAttributes.jtagSignatureFile, authJtagImage->authJtagSignature, signatureLength);
    }
    else if (options.DoGenerateHashes())
    {
        WritePaddedSHAFile(shaHashPadded.get(), "authJtag" + hash->GetHashFileExtension());
    }
}

/******************************************************************************/
void Versal_2vpAuthenticationContext::CreateAuthJtagImage(Options& options, uint8_t* buffer, const AuthJtagInfo& authJtagAttributes)
{
    LOG_TRACE("Creating the Authentication Header signature");
    /* versal_2vp secure-debug message is PPK-only: message header + versal_2vp HeaderAuthCert
       ppkheader + PPK + a single PSK signature over the whole message minus the
       trailing signature. No SPK, no SPK sub-header, no SPK signature. */
    if (authAlgorithm->Type() == Authentication::RSA)
    {
        AuthJtagRSAImageStructure_versal2vp* authJtagImage = (AuthJtagRSAImageStructure_versal2vp*)buffer;
        if (primaryKey->Loaded)
        {
            WriteAuthJtagCommonHeader(buffer, authJtagAttributes);

            hashLength = hash->GetHashLength();
            auto shaHash = std::make_unique<uint8_t[]>(hashLength);
            auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
            memset(shaHash.get(), 0, hashLength);
            memset(shaHashPadded.get(), 0, signatureLength);

            WriteAuthJtagPpkHeader(&authJtagImage->ppkheader, authAlgorithm->GetAuthHeader(),
                                   authJtagAttributes.partitionRevokeId, AUTH_JTAG_PPK_PERMISSION);

            primaryKey->Export(authJtagImage->acPpk);
            authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_N, RSA_4096_N_SIZE);
            authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
            authAlgorithm->RearrangeEndianess(authJtagImage->acPpk + RSA_4096_E, RSA_4096_E_SIZE);

            //Auth Jtag Msg Sign - PSK signs whole message minus the trailing signature
            hash->CalculateVersalHash(true, (uint8_t*)authJtagImage, sizeof(AuthJtagRSAImageStructure_versal2vp) - signatureLength, shaHash.get());
            authAlgorithm->CreatePadding(shaHashPadded.get(), shaHash.get(), hashLength);

            if (primaryKey->isSecret)
            {
                authAlgorithm->RearrangeEndianess(shaHashPadded.get(), signatureLength);
                authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)primaryKey.get(), authJtagImage->authJtagSignature);
                authAlgorithm->RearrangeEndianess(authJtagImage->authJtagSignature, signatureLength);
            }
            else if (authJtagAttributes.jtagSignatureFile != "")
            {
                ReadAuthJtagPresign(authJtagAttributes.jtagSignatureFile, authJtagImage->authJtagSignature, signatureLength);
            }
            else if (options.DoGenerateHashes())
            {
                std::string hashfilename = "authJtag" + hash->GetHashFileExtension();
                WritePaddedSHAFile(shaHashPadded.get(), hashfilename);
            }
        }
        else
        {
            LOG_ERROR("Authentication Error !!!\n          PSK must be specified to generate Authenticated Jtag Image");
        }
    }
    else if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        CreateEcdsaAuthJtagImage<AuthJtagECP384ImageStructure_versal2vp>(options, buffer, authJtagAttributes, 2 * EC_P384_KEY_LENGTH);
    }
    else if (authAlgorithm->Type() == Authentication::ECDSAp521)
    {
        CreateEcdsaAuthJtagImage<AuthJtagECP521ImageStructure_versal2vp>(options, buffer, authJtagAttributes, 2 * EC_P521_KEY_LENGTH2);
    }
    else if (authAlgorithm->Type() == Authentication::MLDSA)
    {
        AuthJtagMLDSAImageStructure_versal2vp* authJtagImage = (AuthJtagMLDSAImageStructure_versal2vp*)buffer;
        if (primaryKey->Loaded)
        {
            WriteAuthJtagCommonHeader(buffer, authJtagAttributes);
            WriteAuthJtagPpkHeader(&authJtagImage->ppkheader, authAlgorithm->GetAuthHeader(),
                                   authJtagAttributes.partitionRevokeId, AUTH_JTAG_PPK_PERMISSION);

            primaryKey->Export(authJtagImage->acPpk);

            MLDSAAuthenticationAlgorithm_versal_2vp* mldsaAlg = dynamic_cast<MLDSAAuthenticationAlgorithm_versal_2vp*>(authAlgorithm.get());
            size_t msgLen = sizeof(AuthJtagMLDSAImageStructure_versal2vp) - MLDSA_TOTAL_SIGN_LEN;
            if (primaryKey->isSecret && mldsaAlg)
            {
                mldsaAlg->CreateSignature((uint8_t*)authJtagImage, msgLen, (uint8_t*)primaryKey.get(), authJtagImage->authJtagSignature);
                mldsaAlg->VerifySignature((uint8_t*)authJtagImage, msgLen, (uint8_t*)primaryKey.get(), authJtagImage->authJtagSignature);
            }
            else if (authJtagAttributes.jtagSignatureFile != "")
            {
                ReadAuthJtagPresign(authJtagAttributes.jtagSignatureFile, authJtagImage->authJtagSignature, MLDSA_TOTAL_SIGN_LEN);
            }
            else
            {
                LOG_ERROR("Authentication Error !!!\n          Either a secret PSK or a jtag_signature file must be provided to sign the Authenticated Jtag Image");
            }
        }
        else
        {
            LOG_ERROR("Authentication Error !!!\n          PSK must be specified to generate Authenticated Jtag Image");
        }
    }
    else if (authAlgorithm->Type() == Authentication::SLH_SHAKE256)
    {
        AuthJtagSLHDSAImageStructure_versal2vp* authJtagImage = (AuthJtagSLHDSAImageStructure_versal2vp*)buffer;
        if (primaryKey->Loaded)
        {
            WriteAuthJtagCommonHeader(buffer, authJtagAttributes);
            WriteAuthJtagPpkHeader(&authJtagImage->ppkheader, authAlgorithm->GetAuthHeader(),
                                   authJtagAttributes.partitionRevokeId, AUTH_JTAG_PPK_PERMISSION);

            primaryKey->Export(authJtagImage->acPpk);

            size_t msgLen = sizeof(AuthJtagSLHDSAImageStructure_versal2vp) - SLHDSA_TOTAL_SIGN_LENGTH;
            if (primaryKey->isSecret)
            {
                size_t sig_len = SLHDSA_TOTAL_SIGN_LENGTH;
                authAlgorithm->CreateSignature((uint8_t*)authJtagImage, msgLen, pskFile.c_str(), authJtagImage->authJtagSignature, sig_len);
                authAlgorithm->VerifySignature((uint8_t*)authJtagImage, msgLen, authJtagImage->authJtagSignature, ppkFile.c_str());
            }
            else if (authJtagAttributes.jtagSignatureFile != "")
            {
                ReadAuthJtagPresign(authJtagAttributes.jtagSignatureFile, authJtagImage->authJtagSignature, SLHDSA_TOTAL_SIGN_LENGTH);
            }
            else
            {
                LOG_ERROR("Authentication Error !!!\n          Either a secret PSK or a jtag_signature file must be provided to sign the Authenticated Jtag Image");
            }
        }
        else
        {
            LOG_ERROR("Authentication Error !!!\n          PSK must be specified to generate Authenticated Jtag Image");
        }
    }
    else if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        uint8_t* authJtagImage = buffer;
        if (primaryKey->Loaded)
        {
            size_t actualppkSize = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly);
            size_t authJtagSignLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);

            if (actualppkSize > AUTH_JTAG_LMS_PUBLIC_KEY_SIZE)
            {
                LOG_ERROR("Authentication Error !!!\n          PPK size exceeds AUTH_JTAG_LMS_PUBLIC_KEY_SIZE (60) for Authenticated Jtag Image");
            }
            else
            {
                /* PPK-only versal_2vp layout: 32-byte header (0x20) + HeaderAuthCert ppkheader (0x20) + PPK + PSK signature.
                   Algorithm/revoke id live in the ppkheader; no message-level size fields. */
                size_t ppkHeaderOffset = AUTH_JTAG_V2VP_FIXED_HEADER_SIZE;
                size_t ppkOffset = ppkHeaderOffset + sizeof(HeaderAuthCert_versal_2vp);
                size_t authSignOffset = ppkOffset + actualppkSize + PADDING_16B(actualppkSize);

                WriteAuthJtagCommonHeader(authJtagImage, authJtagAttributes);

                uint32_t acHdr = authAlgorithm->GetAuthHeader(lmsOnly, false, options.IsDl9Series());
                WriteAuthJtagPpkHeader((HeaderAuthCert_versal_2vp*)(authJtagImage + ppkHeaderOffset), acHdr,
                                       authJtagAttributes.partitionRevokeId, AUTH_JTAG_PPK_PERMISSION);

                primaryKey->Export(authJtagImage + ppkOffset);

                /* PSK signs the whole message minus the trailing signature. */
                if (primaryKey->isSecret)
                {
                    authAlgorithm->CreateSignature(authJtagImage, (size_t)authSignOffset, pskFile.c_str(),
                        authJtagImage + authSignOffset, authJtagSignLength, lmsOnly, ppkFile.c_str());
                    authAlgorithm->VerifySignature(authJtagImage, (size_t)authSignOffset, ppkFile.c_str(),
                        authJtagImage + authSignOffset, authJtagSignLength, lmsOnly);
                }
                else if (authJtagAttributes.jtagSignatureFile != "")
                {
                    ReadAuthJtagPresign(authJtagAttributes.jtagSignatureFile, authJtagImage + authSignOffset, authJtagSignLength);
                }
                else if (options.DoGenerateHashes())
                {
                    std::string hashfilename = "authJtag" + hash->GetHashFileExtension();
                    WritePaddedSHAFile(authJtagImage, (size_t)authSignOffset, hashfilename);
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
void Versal_2vpAuthenticationCertificate::Link(BootImage& bi, Section* dataSection)
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
            memset(headers->Data.get(), bi.options.GetOutputFillByte(), headers->Length);
            memcpy(headers->Data.get(), bi.encryptedHeaders->Data.get(), bi.encryptedHeaders->Length);
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
                size += sizeof(Versal_2vpPartitionHeaderTableStructure);
            }

            // Append NULL partition header
            //sections.push_back(bi.nullPartHeaderSection);

            /* Create one new combined section will all the appended sections above */
            headers = new Section("Headers", size);
            //headers->Address = iHT->section->Address; // not really needed, but useful for debug.
            memset(headers->Data.get(), bi.options.GetOutputFillByte(), headers->Length);

            Binary::Address_t start = sections.front()->Address;
            for (std::list<Section*>::iterator i = sections.begin(); i != sections.end(); i++)
            {
                Section* section = *i;
                int offset = section->Address - start;
                memcpy(headers->Data.get() + offset, section->Data.get(), section->Length);
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
void Versal_2vpAuthenticationContext::SetKeyLength(Authentication::Type type)
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
