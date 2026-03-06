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
#pragma once

#ifndef _SPARTANUP_AUTHENTICATION_CONTEXT_H_
#define _SPARTANUP_AUTHENTICATION_CONTEXT_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <string>
#include <list>
#include <string.h>
#include <memory>
#include "binary.h"
//#include "baseclass.h"
#include "bootgenenum.h"
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#include "authkeys-spartanup.h"
//#include "hash.h"
//#include "systemutils.h"
#include "authentication.h"
//#include "bifoptions.h"
//#include "lms.h"

/* Forward class references */
class BaseThing;
class Section;
class BootImage;
class Binary;
class AuthenticationCertificate;
class PartitionHeader;
class Key;

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define AC_SHA3_PADDING_SIZE        64
#define PAD1_LENGTH                 8
#define PAD2_LENGTH                 415
#define SALT_LENGTH                 48
//#define AUTH_HDR_VERSAL             0x115       //0001 0001 0101
//#define AUTH_HDR_VERSAL_ECDSA       0x106       //0001 0000 0110        
//#define AUTH_HDR_VERSAL_ECDSA_P521  0x126       //0001 0010 0110
#define AUTH_HDR_LASSEN_HSS_LMS          0x10b 


//telluride
#define AUTH_HDR_TELLURIDE_NONSECURE     0x110       //0001 0001 0000
#define AUTH_HDR_TELLURIDE_RSA_4096      0x111       //0001 0001 0001    
#define AUTH_HDR_TELLURIDE_ECDSA_P384    0x112       //0001 0001 0010        
#define AUTH_HDR_TELLURIDE_ECDSA_P521    0x113       //0001 0001 0011
#define AUTH_HDR_TELLURIDE_HSS_LMS       0x114       //0001 0001 0100
#define AUTH_HDR_TELLURIDE_LMS           0x115       //0001 0001 0101

#define AC_HDR_PPK_SELECT_BIT_SHIFT 16

#define AC_HDR_SHA_2_3_BIT_SHIFT    2
#define SHA3_PAD_LENGTH             104
#define WORDS_PER_DEVICE_DNA        4

//Auth Certificate offset defines
#define AC_HEADER_OFFSET            0x0
#define AC_SPK_ID_OFFSET            0x4
#define AC_UDF_OFFSET               0x8
#define AC_PPK_KEY_OFFSET           0x40
#define AC_PPK_SHA3PAD_OFFSET       0x444
#define AC_SPK_KEY_OFFSET           0x450
#define AC_SPK_SHA3PAD_OFFSET       0x854
#define AC_ALLIGNMENT_OFFSET        0x858
#define AC_SPK_SIGN_OFFSET          0x860
#define AC_BH_SIGN_OFFSET           0xa60
#define AC_PARTITION_SIGN_OFFSET    0xc60

#define AC_PPK_SHA3PAD_SIZE 12
#define AC_SPK_SHA3PAD_SIZE 4
#define AC_ALLIGNMENT_SIZE 12

/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
class SpartanupAuthenticationContext : public AuthenticationContext
{
public:
    SpartanupAuthenticationContext(Authentication::Type type);
    SpartanupAuthenticationContext(const AuthenticationContext* refAuthContext, Authentication::Type authtype);
    SpartanupAuthenticationContext(const AuthCertificate4096Sha3PaddingHBStructure* existingCert, Authentication::Type authtype);
    ~SpartanupAuthenticationContext();

    void Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert);

    uint32_t getCertificateSize(void) { return certSize; }
    void AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header);
    Section* CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection, bool isBootloader);
    void GenerateIHTHash(BootImage& bi, uint8_t* sha_hash_padded);
    void GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded);
    void GenerateSPKHash(uint8_t * sha_hash_padded);
    void GeneratePPKHash(const std::string& filename);
    void CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection);
    static void GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index);
    static void GetPresign(const std::string& presignFilename, uint16_t signatureLength, uint8_t* signature, uint32_t index);
    void SetSPKSignatureFile(const std::string& filename);
    void SetBHSignatureFile(const std::string& filename);
    void GenerateSPKSignature(const std::string& filename);
    void ResizeIfNecessary(Section* section);
    void LoadUdfData(const std::string& filename, uint8_t* signature);
    void CreateSPKSignature(BootImage& bi);
    void CreateAuthJtagImage(uint8_t * buffer, AuthJtagInfo authJtagAttributes);
    void SetKeyLength(Authentication::Type type);
    std::unique_ptr<AuthenticationAlgorithm> GetAuthenticationAlgorithm(Authentication::Type type);
    uint32_t GetCertificateSize();
    uint32_t GetTotalHashBlockSignSize(void);
private:
    void CopybHSignature(BootImage& bi, uint8_t* ptr);
    void CopyIHTSignature(BootImage& bi, uint8_t* ptr);
    uint32_t certSize;
    void CopySPKSignature(BootImage& bi, uint8_t* ptr);
    std::string GetCertificateName(std::string name);
    bool spkSignatureCreated;
};

/******************************************************************************/
class SpartanupAuthenticationCertificate : public AuthenticationCertificate
{
public:
    SpartanupAuthenticationCertificate(AuthenticationContext* context) : AuthenticationCertificate(context) {}
    Section* AttachBootHeaderToFsbl(BootImage& bi) { return NULL; }
    void Link(BootImage& bi, Section* section);
    //AuthCertificate4096Sha3PaddingStructure *acStructure;
};
#endif
