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
#include "authentication-spartanup.h"
#include "authentication-versal_2ve_2vm.h"
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
#include "partitionheadertable-spartanup.h"
#include "imageheadertable-spartanup.h"
#include "Keccak-compact-spartanup.h"
#include "encryptutils.h"
#include <openssl/rand.h>

#include "bootheader-spartanup.h"

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
SpartanupAuthenticationContext::SpartanupAuthenticationContext(Authentication::Type type)
{
    signatureLength = SIGN_LENGTH_VERSAL;
    //spksignature = new uint8_t[signatureLength];
    bHsignature = new uint8_t[signatureLength];
    spkSignLoaded = false;
    memset(udf_data, 0, UDF_DATA_SIZE);
    memset(bHsignature, 0, signatureLength);
    bhSignLoaded = false;
    hashType = AuthHash::Sha3;
    authAlgorithm = GetAuthenticationAlgorithm(type);
    if (type == Authentication::RSA)
    {
        primaryKey = new Key4096Sha3Padding_spartanup("Primary Key");
        secondaryKey = new Key4096Sha3Padding_spartanup("Secondary Key");
        primaryKey->authType = Authentication::RSA;
        secondaryKey->authType = Authentication::RSA;

        certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure);
        signatureLength = SIGN_LENGTH_VERSAL;
    }
    else if (type == Authentication::ECDSA)
    {
        primaryKey = new KeyECDSA_spartanup("Primary Key");
        secondaryKey = new KeyECDSA_spartanup("Secondary Key");
        primaryKey->authType = Authentication::ECDSA;
        secondaryKey->authType = Authentication::ECDSA;

        certSize = sizeof(AuthCertificateECDSAHBStructure);
        signatureLength = EC_P384_KEY_LENGTH * 2;
    }
    else if (type == Authentication::ECDSAp521)
    {
        primaryKey = new KeyECDSAp521_spartanup("Primary Key");
        secondaryKey = new KeyECDSAp521_spartanup("Secondary Key");
        primaryKey->authType = Authentication::ECDSAp521;
        secondaryKey->authType = Authentication::ECDSAp521;

        certSize = sizeof(AuthCertificateECDSAp521HBStructure);
        signatureLength = EC_P521_KEY_LENGTH2 * 2;
    }
    else if ((type == Authentication::LMS_SHA2_256) || (type == Authentication::LMS_SHAKE256))
    {
        primaryKey = new KeyLMS_spartanup("Primary Key");
        secondaryKey = new KeyLMS_spartanup("Secondary Key");
        primaryKey->lmsOnly = secondaryKey->lmsOnly = lmsOnly;
        if (type == Authentication::LMS_SHA2_256)
        {
            primaryKey->authType = Authentication::LMS_SHA2_256;
            secondaryKey->authType = Authentication::LMS_SHA2_256;
            hashType = AuthHash::Sha2;
            hash = new HashSha2();
        }
        else
        {
            primaryKey->authType = Authentication::LMS_SHAKE256;
            secondaryKey->authType = Authentication::LMS_SHAKE256;
            hashType = AuthHash::Shake256;
            hash = new HashShake256();
        }
        signatureLength = GetLmsSignLength(pskFile.c_str(),lmsOnly);
        certSize = GetCertificateSize();
    }
    spksignature = new uint8_t[signatureLength];
}

/******************************************************************************/
SpartanupAuthenticationContext::SpartanupAuthenticationContext(const AuthenticationContext* refAuthContext, Authentication::Type authtype)
{
    signatureLength = SIGN_LENGTH_VERSAL;
    //spksignature = new uint8_t[signatureLength];
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
    lmsOnly = refAuthContext->lmsOnly;

    if (authtype == Authentication::RSA)
    {
        primaryKey = new Key4096Sha3Padding_spartanup("Primary Key");
        secondaryKey = new Key4096Sha3Padding_spartanup("Secondary Key");
        primaryKey->authType = Authentication :: RSA;
        secondaryKey->authType = Authentication :: RSA;

        certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure);
        signatureLength = SIGN_LENGTH_VERSAL;
    }
    else if(authtype == Authentication::ECDSA)
    {
        primaryKey = new KeyECDSA_spartanup("Primary Key");
        secondaryKey = new KeyECDSA_spartanup("Secondary Key");
        primaryKey->authType = Authentication :: ECDSA;
        secondaryKey->authType = Authentication :: ECDSA;

        certSize = sizeof(AuthCertificateECDSAHBStructure);
        signatureLength = EC_P384_KEY_LENGTH * 2;
    }
    else if (authtype == Authentication::ECDSAp521)
    {
        primaryKey = new KeyECDSAp521_spartanup("Primary Key");
        secondaryKey = new KeyECDSAp521_spartanup("Secondary Key");
        primaryKey->authType = Authentication :: ECDSAp521;
        secondaryKey->authType = Authentication :: ECDSAp521;

        certSize = sizeof(AuthCertificateECDSAp521HBStructure);
        signatureLength = EC_P521_KEY_LENGTH2 * 2;
    }
    else if ((authtype == Authentication::LMS_SHA2_256) || (authtype == Authentication::LMS_SHAKE256))
    {
        primaryKey = new KeyLMS_spartanup("Primary Key");
        secondaryKey = new KeyLMS_spartanup("Secondary Key");
        primaryKey->lmsOnly = secondaryKey->lmsOnly = lmsOnly;
        if (authtype == Authentication::LMS_SHA2_256)
        {
            primaryKey->authType = Authentication::LMS_SHA2_256;
            secondaryKey->authType = Authentication::LMS_SHA2_256;
            hashType = AuthHash::Sha2;
            hash = new HashSha2();
        }
        else
        {
            primaryKey->authType = Authentication::LMS_SHAKE256;
            secondaryKey->authType = Authentication::LMS_SHAKE256;
            hashType = AuthHash::Shake256;
            hash = new HashShake256();
        }
        signatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);
        certSize = GetCertificateSize();
    }

    spksignature = new uint8_t[signatureLength];
    bHsignature = new uint8_t[signatureLength];

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
        primaryKey = refAuthContext->primaryKey;
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
    //certSize = sizeof(AuthCertificate4096Sha3PaddingStructure);
    preSigned = refAuthContext->preSigned;
}


/******************************************************************************/
SpartanupAuthenticationContext::SpartanupAuthenticationContext(const AuthCertificate4096Sha3PaddingHBStructure* existingCert, Authentication::Type authtype)
{
    signatureLength = SIGN_LENGTH_VERSAL;
    //spksignature = new uint8_t[signatureLength];
    spkSignLoaded = true;
    bHsignature = new uint8_t[signatureLength];
    bhSignLoaded = true;
    authAlgorithm = GetAuthenticationAlgorithm(authtype);

    if (authtype == Authentication::RSA)
    {
        primaryKey = new Key4096Sha3Padding_spartanup("Primary Key");
        secondaryKey = new Key4096Sha3Padding_spartanup("Secondary Key");
        primaryKey->authType = Authentication::RSA;
        secondaryKey->authType = Authentication::RSA;

        certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure);
        signatureLength = SIGN_LENGTH_VERSAL;
    }
    else if (authtype == Authentication::ECDSA)
    {
        primaryKey = new KeyECDSA_spartanup("Primary Key");
        secondaryKey = new KeyECDSA_spartanup("Secondary Key");
        primaryKey->authType = Authentication::ECDSA;
        secondaryKey->authType = Authentication::ECDSA;

        certSize = sizeof(AuthCertificateECDSAHBStructure);
        signatureLength = EC_P384_KEY_LENGTH * 2;
    }
    else if (authtype == Authentication::ECDSAp521)
    {
        primaryKey = new KeyECDSAp521_spartanup("Primary Key");
        secondaryKey = new KeyECDSAp521_spartanup("Secondary Key");
        primaryKey->authType = Authentication::ECDSAp521;
        secondaryKey->authType = Authentication::ECDSAp521;
        hashType = AuthHash::Sha3;
        hash = new HashSha3();
        certSize = sizeof(AuthCertificateECDSAp521HBStructure);
        signatureLength = EC_P521_KEY_LENGTH2 * 2;
    }
    else if((authtype == Authentication::LMS_SHA2_256) || (authtype == Authentication::LMS_SHAKE256))
    {
        primaryKey = new KeyLMS_spartanup("Primary Key");
        secondaryKey = new KeyLMS_spartanup("Secondary Key");
        primaryKey->lmsOnly = secondaryKey->lmsOnly = lmsOnly;
        if (authtype == Authentication::LMS_SHA2_256)
        {
            primaryKey->authType = Authentication::LMS_SHA2_256;
            secondaryKey->authType = Authentication::LMS_SHA2_256;
            hashType = AuthHash::Sha2;
            hash = new HashSha2();
        }
        else
        {
            primaryKey->authType = Authentication::LMS_SHAKE256;
            secondaryKey->authType = Authentication::LMS_SHAKE256;
            hashType = AuthHash::Shake256;
            hash = new HashShake256();
        }
        signatureLength = GetLmsSignLength(pskFile.c_str(),lmsOnly);
        certSize = GetCertificateSize();
    }
    else {
    }

    spksignature = new uint8_t[signatureLength];
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
    //memcpy(bHsignature, existingCert->acHeaderSignature.Signature, signatureLength);
    //memcpy(udf_data, existingCert->acUdf, UDF_DATA_SIZE);
    //spkIdentification = existingCert->spkId;
    //certSize = sizeof(AuthCertificate4096Sha3PaddingStructure);
}

/******************************************************************************/
AuthenticationAlgorithm* SpartanupAuthenticationContext::GetAuthenticationAlgorithm(Authentication::Type type)
{
    if (type == Authentication::ECDSA)
    {
        SetAuthenticationKeyLength(EC_P384_KEY_LENGTH);
        return new ECDSAHBAuthenticationAlgorithm();
    }
    else if (type == Authentication::ECDSAp521)
    {
        //SetAuthenticationKeyLength(EC_P521_KEY_LENGTH);
        return new ECDSAP521HBAuthenticationAlgorithm();
    }
    else if(type == Authentication::RSA)
    {
        SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
        return new RSA4096Sha3PaddingHBAuthenticationAlgorithm();
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
        return new LMSAuthenticationAlgorithm(type);
    }
    else
    {
        return NULL;
    }
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
uint32_t SpartanupAuthenticationContext::GetCertificateSize(void)
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
        
        size_t spkSignLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);
        spkSignLength += PADDING_16B(spkSignLength);
        
        return (ppkLength + spkHdrLength + spkLength + spkSignLength);
    }
    else
    {
        return certSize;
    }
}

/******************************************************************************/
uint32_t SpartanupAuthenticationContext::GetTotalHashBlockSignSize(void)
{
    if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256) || (authAlgorithm->Type() == Authentication::LMS_SHAKE256))
    {
        size_t totalHashBlockSignatureLength = GetLmsSignLength(sskFile.c_str(), lmsOnly);
        totalHashBlockSignatureLength += PADDING_16B(totalHashBlockSignatureLength);
        
        return totalHashBlockSignatureLength;
    }
    else
    {
        return signatureLength;
    }
}

/******************************************************************************/
SpartanupAuthenticationContext::~SpartanupAuthenticationContext()
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
Section* SpartanupAuthenticationContext::CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection)
{
    LOG_INFO("Creating Authentication Certificate for section - %s", dataSection->Name.c_str());

    /* PM-TODO - Check this and update */
    if ((authAlgorithm->Type() != Authentication::LMS_SHA2_256) && (authAlgorithm->Type() != Authentication::LMS_SHAKE256))
    {
        hashType = bi.GetAuthHashAlgo();
        hash = bi.hash;
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
    cache.Sections.push_back(acSection);
    uint8_t* authCert = acSection->Data;
    LOG_TRACE("Creating new section for certificate - %s", acSection->Name.c_str());

    uint32_t x = sizeof(AuthCertificate4096Sha3PaddingHBStructure);
    if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        x = certSize = sizeof(AuthCertificateECDSAHBStructure);
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
    memset(authCert, 0, certSize);
    
    uint32_t acHdr = authAlgorithm->GetAuthHeader();
    
    uint8_t* headerData = bi.bootHeader->section->Data;
    uint32_t authHeader1Offset = BH_AC_HEADER_OFFSET_SUP;
    uint32_t totalppkkSize1Offset = BH_TOTAL_PPK_SIZE1_OFFSET_SUP;
    uint32_t actualppkSize1Offset = BH_ACTUAL_PPK_SIZE1_OFFSET_SUP;
    uint32_t totalHashBlockSignatureSize1Offset = BH_TOTAL_SIGN_SIZE1_OFFSET_SUP;
    uint32_t actualSignatureSize1Offset = BH_ACTUAL_SIGN_SIZE1_OFFSET_SUP;
    if (name == "MetaHeader")
    {
        headerData = bi.imageHeaderTable->section->Data;
        authHeader1Offset = IHT_AC_HEADER_OFFSET;
        totalppkkSize1Offset = IHT_TOTAL_PPK_SIZE1_OFFSET;
        actualppkSize1Offset = IHT_ACTUAL_PPK_SIZE1_OFFSET;
        totalHashBlockSignatureSize1Offset = IHT_TOTAL_SIGN_SIZE1_OFFSET;
        actualSignatureSize1Offset = IHT_ACTUAL_SIGN_SIZE1_OFFSET;
    }
  
    WriteLittleEndian32(headerData + authHeader1Offset, acHdr);
    if (authAlgorithm->Type() == Authentication::RSA)
    {
        WriteLittleEndian32(headerData + totalppkkSize1Offset, RSA_4096_N_SIZE + RSA_4096_N_EXT_SIZE + RSA_4096_E_SIZE + TELLURIDE_RSA_AC_PPK_SPK_ALIGNMENT);
        WriteLittleEndian32(headerData + actualppkSize1Offset, RSA_4096_N_SIZE + RSA_4096_N_EXT_SIZE + RSA_4096_E_SIZE);
        WriteLittleEndian32(headerData + totalHashBlockSignatureSize1Offset, signatureLength);
        WriteLittleEndian32(headerData + actualSignatureSize1Offset, signatureLength);

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
    }
    else if (authAlgorithm->Type() == Authentication::ECDSAp521)
    {
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
    }
    else 
    {
        // LMS_SHA256 || LMS_SHAKE256
        acHdr = authAlgorithm->GetAuthHeader(lmsOnly, true, bi.options.IsDl9Series());

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
    }


    certIndex++;
    return acSection;
}

/******************************************************************************/
void SpartanupAuthenticationContext::Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert)
{
    /* Copy bhSignature when bootloader */
    //memset(cert->section->Data + AC_BH_SIGN_OFFSET, 0, signatureLength); /*check*/
    if (sections.front()->isBootloader)
    {
        //CopybHSignature(bi, cert->section->Data + AC_BH_SIGN_OFFSET);

        //CopyBhHash
        LOG_TRACE("Calculating the Boot Header Hash");
        /* Donot include SMAP data to calculate BH hash */
        uint8_t* tmpBh = bi.bootHeader->section->Data + 0x10;
        
        /* Calculate the BH hash with SHA3 rather than LMS hashing algo */
        uint8_t* sha_hash = new uint8_t[bi.hash->GetHashLength()];
        if (!bi.options.IsDl9Series())
        {
            bi.hash->CalculateHash(true, tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(SpartanupSmapWidthTable), sha_hash);
        }
        else
        {
            bi.hash->CalculateVersalHash(true, tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(SpartanupSmapWidthTable), sha_hash);
        }
        
        /*if (bi.options.DoGenerateHashes())
        {
            std::string hashfilename = "bootheader" + hash->GetHashFileExtension();
            WritePaddedSHAFile(sha_hash_padded, hashfilename);
        }*/

        memcpy(bi.partitionHeaderList.front()->partition->section->Data + HASH_BLOCK_INDEX_BYTES, sha_hash, bi.hash->GetHashLength());

        delete[] sha_hash;
    }

    /*  Copy meta header Signature when headers */
    if (sections.front()->Name == "Headers")
    {
        //CopyIHTSignature(bi, cert->section->Data + AC_BH_SIGN_OFFSET);
    }

    if (presignFile == "")
    {
        if (sections.front()->Name == "Headers")
            CopyPartitionSignature(bi, sections, bi.imageHeaderTable->hashBlockSection->Data + bi.imageHeaderTable->hashBlockSectionLength, cert->section);
        else
            CopyPartitionSignature(bi, sections, sections.front()->Data + bi.hashBlockLength, cert->section);

        //CopyPartitionSignature(bi, sections, cert->section->Data + AC_PARTITION_SIGN_OFFSET, cert->section);
    }
    else
    {
        int index = acIndex;
        if (cert->section->index != 0)
        {
            index = cert->section->index;
        }
        //GetPresign(presignFile, cert->section->Data + AC_PARTITION_SIGN_OFFSET, index);
        if (authAlgorithm->Type() == Authentication::RSA)
            GetPresign(presignFile, sections.front()->Data + bi.hashBlockLength, index);
        else if (authAlgorithm->Type() == Authentication::ECDSA)
            GetPresign(presignFile, sections.front()->Data + bi.hashBlockLength, index);
        else
        {
            //EC-p521 
            //LMS
        }
        acIndex++;
    }
}

/******************************************************************************/
void SpartanupAuthenticationContext::CopybHSignature(BootImage& bi, uint8_t* ptr)
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
        else
        {
            authAlgorithm->CreateSignature(sha_hash_padded, (uint8_t*)secondaryKey, bHsignaturetmp);
        }
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
void SpartanupAuthenticationContext::CopyIHTSignature(BootImage & bi, uint8_t * ptr)
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
        else
        {
            if (authAlgorithm->Type() != Authentication::ECDSA)
            {
                authAlgorithm->RearrangeEndianess(sha_hash_padded, signatureLength);
            }
            authAlgorithm->CreateSignature(sha_hash_padded, (uint8_t*)secondaryKey, signaturetmp);
            if (authAlgorithm->Type() != Authentication::ECDSA)
            {
                authAlgorithm->RearrangeEndianess(signaturetmp, signatureLength);
            }
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
void SpartanupAuthenticationContext::GenerateIHTHash(BootImage& bi, uint8_t* sha_hash_padded)
{
    LOG_TRACE("Calculating Image Header Table Hash");
    uint8_t* tmpIht = bi.imageHeaderTable->section->Data;
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
void SpartanupAuthenticationContext::GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded)
{
    LOG_TRACE("Calculating the Boot Header Hash");
    /* Donot include SMAP data to calculate BH hash */
    uint8_t* tmpBh = bi.bootHeader->section->Data + 0x10;
    uint8_t* sha_hash = new uint8_t[hashLength];
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        if (!bi.options.IsDl9Series())
        {
            bi.hash->CalculateHash(true, tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(SpartanupSmapWidthTable) , sha_hash);
        }
        else
        {
            hash->CalculateHash(true, tmpBh, bi.bootHeader->GetBootHeaderSize() - sizeof(SpartanupSmapWidthTable), sha_hash);
        }
    }
    else
    {
        hash->CalculateVersalHash(true, tmpBh, bi.bootHeader->section->Length - 0x10, sha_hash);
    }
    
    
    authAlgorithm->CreatePadding(sha_hash_padded, sha_hash, hashLength);

    memcpy(bi.partitionHeaderList.front()->section->Data + HASH_BLOCK_INDEX_BYTES, sha_hash, hashLength);

    delete[] sha_hash;
}

/******************************************************************************/
void SpartanupAuthenticationContext::GenerateSPKHash(uint8_t* sha_hash_padded)
{
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
}

/******************************************************************************/
void SpartanupAuthenticationContext::CopySPKSignature(BootImage& bi, uint8_t* ptr)
{
    CreateSPKSignature(bi);
    LOG_TRACE("Copying the SPK signature into the Authentication Certificate");
    memcpy(ptr, spksignature, signatureLength);
}

/******************************************************************************/
std::string SpartanupAuthenticationContext::GetCertificateName(std::string name)
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
void SpartanupAuthenticationContext::GeneratePPKHash(const std::string& filename)
{
    int ppkSize = VERSAL_ACKEY_STRUCT_SIZE;

    if (authAlgorithm->Type() == Authentication::ECDSA)
        ppkSize = 2 * EC_P384_KEY_LENGTH;
    if (authAlgorithm->Type() == Authentication::ECDSAp521)
        ppkSize = 2 * EC_P521_KEY_LENGTH2;
    if ((authAlgorithm->Type() == Authentication::LMS_SHA2_256) || (authAlgorithm->Type() == Authentication::LMS_SHAKE256))
    {
        hash = new HashSha3_256();
        ppkSize = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly); // sizeof(HssPublicKey);
    }
    hashLength = hash->GetHashLength();
    uint8_t* ppkTemp = new uint8_t[ppkSize];
    memset(ppkTemp, 0, ppkSize);
    primaryKey->Export(ppkTemp);
    if ((authAlgorithm->Type() != Authentication::LMS_SHA2_256) && (authAlgorithm->Type() != Authentication::LMS_SHAKE256))
    {
        authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_N_EXT, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(ppkTemp + RSA_4096_E, RSA_4096_E_SIZE);
    }

    uint8_t* ppkHash = new uint8_t[hashLength];

    hash->CalculateHash(true, ppkTemp, ppkSize, ppkHash);

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

    delete[] ppkTemp;
}

/******************************************************************************/
void SpartanupAuthenticationContext::CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection)
{
    LOG_TRACE("Copying the partition (%s) signature into Authentication Certificate", acSection->Name.c_str());
    /* calculate hash first */
    uint8_t* shaHash = new uint8_t[hashLength];
    std::list<Section*>::iterator section = sections.begin();

    /* Calculate the final hash */
    size_t hashBlockLength = bi.hashBlockLength;
    if (sections.front()->Name == "Headers")
    {
        hashBlockLength = bi.imageHeaderTable->hashBlockSectionLength;
    }

    uint8_t *hashBlock = new uint8_t[hashBlockLength];
    memset(hashBlock, 0, hashBlockLength);

    if (sections.front()->Name == "Headers")
        memcpy(hashBlock, bi.imageHeaderTable->hashBlockSection->Data, hashBlockLength);
    else
        memcpy(hashBlock, (*section)->Data, hashBlockLength);

    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    { 
        if(!bi.options.IsDl9Series())
        {
            hash->CalculateHash(true, hashBlock, hashBlockLength, shaHash);
        }
        signatureLength = GetLmsSignLength(sskFile.c_str(), lmsOnly);
    }
    else
    {
        Spartanupcrypto_hash(shaHash, hashBlock, hashBlockLength, true);
    }
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
#ifdef DEBUG
    LOG_TRACE("Hash Block Data ", acSection->Name.c_str());
    LOG_DUMP_BYTES(hashBlock, hashBlockLength);
#endif
    /* Now sign the hash */
    if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
    {
        if(!bi.options.IsDl9Series())
        {
            //authAlgorithm->CreateSignature(shaHashPadded, hashLength, sskFile.c_str(), signatureBlock);
            authAlgorithm->CreateSignature(shaHashPadded, hashLength, sskFile.c_str(), 
                signatureBlock, signatureLength, lmsOnly, spkFile.c_str());
        }
        else
        {
            authAlgorithm->CreateSignature(hashBlock, hashBlockLength, sskFile.c_str(), 
                signatureBlock, signatureLength, lmsOnly, spkFile.c_str());
            authAlgorithm->VerifySignature(hashBlock, hashBlockLength, spkFile.c_str(), signatureBlock, signatureLength, lmsOnly);
        }
    }
    else
    {
        authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)secondaryKey, signatureBlock);
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
void SpartanupAuthenticationContext::AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header)
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
void SpartanupAuthenticationContext::GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index)
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
void SpartanupAuthenticationContext::SetSPKSignatureFile(const std::string& filename)
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
void SpartanupAuthenticationContext::SetBHSignatureFile(const std::string& filename)
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
void SpartanupAuthenticationContext::GenerateSPKSignature(const std::string& filename)
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
void SpartanupAuthenticationContext::ResizeIfNecessary(Section* section)
{
    // round up to next 64 byte boundary.
    uint32_t dataSizePadded = (section->Length + 63) & 0xFFFFFFC0;
    section->IncreaseLengthAndPadTo(dataSizePadded, 0xFF);
}

/******************************************************************************/
void SpartanupAuthenticationContext::LoadUdfData(const std::string& udfFilename, uint8_t* signature)
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
void SpartanupAuthenticationContext::CreateSPKSignature(BootImage& bi)
{
    LOG_TRACE("Creating the SPK signature");

    uint8_t* shaHashPadded = NULL;
    /* SPK is signed with PSK (Primary Secret Key)
    Check if PSK is given */
    if (primaryKey->Loaded && primaryKey->isSecret)
    {
        // Sign the SPK hash
        if (authAlgorithm->Type() == Authentication::LMS_SHA2_256 || authAlgorithm->Type() == Authentication::LMS_SHAKE256)
        {
            if(!bi.options.IsDl9Series())
            {
                shaHashPadded = new uint8_t[hashLength];
                memset(shaHashPadded, 0, hashLength);
                signatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);
                spksignature = new uint8_t[signatureLength];
                memset(spksignature, 0, signatureLength);
                // Calulate the SPK hash with PKCS padding
                GenerateSPKHash(shaHashPadded);
                authAlgorithm->RearrangeEndianess(shaHashPadded, hashLength);
                //authAlgorithm->CreateSignature(shaHashPadded, hashLength, pskFile.c_str(), spksignature);
                authAlgorithm->CreateSignature(shaHashPadded, hashLength, pskFile.c_str(),
                    spksignature, signatureLength, lmsOnly, ppkFile.c_str());
                authAlgorithm->VerifySignature(shaHashPadded, hashLength, ppkFile.c_str(),
                    spksignature, signatureLength, lmsOnly);
            }
            else
            {
                uint8_t* tempBuffer = NULL;
                uint16_t totalSignatureLength = 0;
                size_t totalKeySize = 0;
                size_t actualKeySize = 0;
                actualKeySize = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);
                totalKeySize = actualKeySize + PADDING_16B(actualKeySize);
                signatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);
                totalSignatureLength = signatureLength + PADDING_16B(signatureLength);

                uint8_t* spkFull = new uint8_t[actualKeySize];

                if (!secondaryKey->Loaded)
                {
                    ParseSPKeyFile(spkFile);
                }
                secondaryKey->Export(spkFull);

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
                authAlgorithm->CreateSignature(tempBuffer, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, pskFile.c_str(),
                    spksignature, signatureLength, lmsOnly, ppkFile.c_str());
    #ifdef DEBUG
                LOG_TRACE("spksignature");
                LOG_DUMP_BYTES(spksignature, signatureLength);
    #endif
                authAlgorithm->VerifySignature(tempBuffer, actualKeySize + TELLURIDE_AC_SPK_HDR_LENGTH, ppkFile.c_str(),
                    spksignature, signatureLength, lmsOnly);
                //authAlgorithm->CreateSignature(shaHashPadded, hashLength, pskFile.c_str(), spksignature, signatureLength, lmsOnly);
            }
        }
        else
        {
            shaHashPadded = new uint8_t[signatureLength];
            memset(shaHashPadded, 0, signatureLength);
            memset(spksignature, 0, signatureLength);
            // Calulate the SPK hash with PKCS padding
            GenerateSPKHash(shaHashPadded);
            authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);
            authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)primaryKey, spksignature);
        }

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
void static SetDeviceDNA(const uint8_t* dnaValue, uint32_t* deviceDNA)
{
    for (uint32_t index = 0; index < WORDS_PER_DEVICE_DNA; index++)
    {
        deviceDNA[WORDS_PER_DEVICE_DNA - (index+1)] = ReadBigEndian32(dnaValue);
        dnaValue += sizeof(uint32_t);
    }
}

/******************************************************************************/
void SpartanupAuthenticationContext::CreateAuthJtagImage(uint8_t* buffer, AuthJtagInfo authJtagAttributes)
{
    LOG_TRACE("Creating the Authentication Header signature");
    if (authAlgorithm->Type() == Authentication::RSA)
    {
        AuthenticatedJtagRSAImageStructure* authJtagImage = (AuthenticatedJtagRSAImageStructure*)buffer;
        if (primaryKey->Loaded && primaryKey->isSecret)
        {
            authJtagImage->idWord = AUTH_JTAG_IMAGE_IDENTIFICATION_WORD;
            WriteLittleEndian32(&authJtagImage->authJtagImageLength, sizeof(AuthenticatedJtagRSAImageStructure));

            hashLength = hash->GetHashLength();
            uint8_t* shaHash = new uint8_t[hashLength];
            uint8_t* shaHashPadded = new uint8_t[signatureLength];
            memset(shaHash, 0, hashLength);
            memset(shaHashPadded, 0, signatureLength);

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
            hash->CalculateVersalHash(true, (uint8_t*)authJtagImage + 0x450, 0x20 + VERSAL_ACKEY_STRUCT_SIZE + TELLURIDE_RSA_AC_PPK_SPK_ALIGNMENT, shaHash);
            authAlgorithm->CreatePadding(shaHashPadded, shaHash, hashLength);
            authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);

            authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)primaryKey, authJtagImage->spkSignature);
            authAlgorithm->RearrangeEndianess(authJtagImage->authJtagSignature, signatureLength);

            //Auth Jtag Msg Sign
            hash->CalculateVersalHash(true, (uint8_t*)authJtagImage, sizeof(AuthenticatedJtagRSAImageStructure) - signatureLength, shaHash);
            authAlgorithm->CreatePadding(shaHashPadded, shaHash, hashLength);
            authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);

            authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)secondaryKey, authJtagImage->authJtagSignature);
            authAlgorithm->RearrangeEndianess(authJtagImage->authJtagSignature, signatureLength);

            delete[] shaHash;
            delete[] shaHashPadded;
        }
        else
        {
            LOG_ERROR("Authentication Error !!!\n          PSK must be specified to generate Authenticated Jtag Image");
        }
    }
    else if (authAlgorithm->Type() == Authentication::ECDSA)
    {
        AuthenticatedJtagECP384ImageStructure* authJtagImage = (AuthenticatedJtagECP384ImageStructure*)buffer;
        if (primaryKey->Loaded && primaryKey->isSecret)
        {
            authJtagImage->idWord = AUTH_JTAG_IMAGE_IDENTIFICATION_WORD;
            WriteLittleEndian32(&authJtagImage->authJtagImageLength, sizeof(AuthenticatedJtagRSAImageStructure));

            hashLength = hash->GetHashLength();
            uint8_t* shaHash = new uint8_t[hashLength];
            uint8_t* shaHashPadded = new uint8_t[signatureLength];
            memset(shaHash, 0, hashLength);
            memset(shaHashPadded, 0, signatureLength);

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
            hash->CalculateVersalHash(true, (uint8_t*)authJtagImage + 0xA0, 0x20 + 2 * EC_P384_KEY_LENGTH, shaHash);
            authAlgorithm->CreatePadding(shaHashPadded, shaHash, hashLength);
            authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);

            authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)primaryKey, authJtagImage->spkSignature);
            authAlgorithm->RearrangeEndianess(authJtagImage->authJtagSignature, signatureLength);

            //Auth Jtag Msg Sign
            hash->CalculateVersalHash(false, (uint8_t*)authJtagImage, sizeof(AuthenticatedJtagRSAImageStructure) - signatureLength, shaHash);
            authAlgorithm->CreatePadding(shaHashPadded, shaHash, hashLength);
            authAlgorithm->RearrangeEndianess(shaHashPadded, signatureLength);

            authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)secondaryKey, authJtagImage->authJtagSignature);
            authAlgorithm->RearrangeEndianess(authJtagImage->authJtagSignature, signatureLength);
            delete[] shaHash;
            delete[] shaHashPadded;
        }
        else
        {
            LOG_ERROR("Authentication Error !!!\n          PSK must be specified to generate Authenticated Jtag Image");
        }
    }
}

/******************************************************************************/
void SpartanupAuthenticationCertificate::Link(BootImage& bi, Section* dataSection)
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
                size += sizeof(SpartanupPartitionHeaderTableStructure);
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
void SpartanupAuthenticationContext::SetKeyLength(Authentication::Type type)
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
