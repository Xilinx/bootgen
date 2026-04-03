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

#ifndef _VERSAL_2VE_2VM_AUTHENTICATION_CONTEXT_H_
#define _VERSAL_2VE_2VM_AUTHENTICATION_CONTEXT_H_


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

#include "authkeys-versal_2ve_2vm.h"
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

#define TELLURIDE_EC_P521_AC_PPK_OFFSET                  (0x00)
#define TELLURIDE_EC_P521_AC_PPK_ALIGNMENT_OFFSET        (0x84)
#define TELLURIDE_EC_P521_AC_TOTAL_SPK_SIZE_OFFSET       (0x90)
#define TELLURIDE_EC_P521_AC_ACTUAL_SPK_SIZE_OFFSET      (0x94)
#define TELLURIDE_EC_P521_AC_TOTAL_SPK_SIGN_SIZE_OFFSET  (0x98)
#define TELLURIDE_EC_P521_AC_ACTUAL_SPK_SIGN_SIZE_OFFSET (0x9C)
#define TELLURIDE_EC_P521_AC_SPK_ID_OFFSET               (0xA0)
#define TELLURIDE_EC_P521_AC_SPK_HDR_ALIGNMENT_OFFSET    (0xA4)
#define TELLURIDE_EC_P521_AC_SPK_OFFSET                  (0xB0)
#define TELLURIDE_EC_P521_AC_SPK_ALIGNMENT_OFFSET        (0x134)
#define TELLURIDE_EC_P521_AC_SPK_SIGN_OFFSET             (0x140)
#define TELLURIDE_EC_P521_AC_SPK_SIGN_ALIGNMENT_OFFSET   (0x1C4)

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
class ECDSAHBAuthenticationAlgorithm : public AuthenticationAlgorithm
{
public:
    ECDSAHBAuthenticationAlgorithm();
    ~ECDSAHBAuthenticationAlgorithm();

    Authentication::Type Type()
    {
        return Authentication::ECDSA;
    }

    int KeySize()
    {
        return sizeof(ACKeyECDSA);
    }

    void CreateSignature(const uint8_t *base, uint8_t* primaryKey, uint8_t *result0);
    //uint32_t getCertificateSize() { return certSize; }
    void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength);
    void ECDSASignature(const uint8_t *base, EC_KEY *eckey, uint8_t *result0);
    Authentication::Type authType;
    void RearrangeEndianess(uint8_t *array, uint32_t size);
    uint32_t GetAuthHeader(void);

private:
    uint32_t certSize;
};

/******************************************************************************/
class ECDSAP521HBAuthenticationAlgorithm : public AuthenticationAlgorithm
{
public:
    ECDSAP521HBAuthenticationAlgorithm();
    ~ECDSAP521HBAuthenticationAlgorithm();

    Authentication::Type Type()
    {
        return Authentication::ECDSAp521;
    }

    int KeySize()
    {
        return sizeof(ACKeyECDSAP521);
    }

    void CreateSignature(const uint8_t *base, uint8_t* primaryKey, uint8_t *result0);
    //uint32_t getCertificateSize() { return certSize; }
    void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength);
    void ECDSASignature(const uint8_t *base, EC_KEY *eckey, uint8_t *result0);
    Authentication::Type authType;
    void RearrangeEndianess(uint8_t *array, uint32_t size);
    uint32_t GetAuthHeader(void);

private:
    uint32_t certSize;
};

/******************************************************************************/
class RSA4096Sha3PaddingHBAuthenticationAlgorithm : public AuthenticationAlgorithm
{
public:
    RSA4096Sha3PaddingHBAuthenticationAlgorithm();

    ~RSA4096Sha3PaddingHBAuthenticationAlgorithm();

    Authentication::Type Type()
    {
        return Authentication::RSA;
    }

    void CreateSignature(const uint8_t *base, uint8_t* primaryKey, uint8_t *result0);
    //uint32_t getCertificateSize(void) { return certSize; }
    void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength);
    Authentication::Type authType;
    void RearrangeEndianess(uint8_t* array, uint32_t size);
    uint8_t* AttachSHA3Padding(uint8_t * data, const Binary::Length_t datalength);
    int MaskGenerationFunction(unsigned char *mask, long len, const unsigned char *seed, long seedlen, const EVP_MD *dgst);
    uint32_t GetAuthHeader(void);

private:
    uint32_t certSize;
};

/******************************************************************************/
class LMSAuthenticationAlgorithm : public AuthenticationAlgorithm
{
public:
    LMSAuthenticationAlgorithm(Authentication::Type type) 
    { 
        authType = type;  
    }

    ~LMSAuthenticationAlgorithm() {}

    Authentication::Type Type()
    {
        return authType;
    }

    void CreateSignature(const uint8_t *buffer, size_t buf_len, const char* keyfile, 
        uint8_t *result0, size_t result0_len, bool lmsOnly, const char* publicKeyFile);
    void VerifySignature(const uint8_t *buffer, size_t buf_len, const char* keyfile, 
        uint8_t *result0, size_t result0_len, bool lmsOnly);

    void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength);
    //uint32_t getCertificateSize(void) { return certSize; }
    Authentication::Type authType;
    void RearrangeEndianess(uint8_t* array, uint32_t size) { };
    //uint32_t GetAuthHeader(void);
    uint32_t GetAuthHeader(bool lmsOnly);

private:
    uint32_t certSize;
};

/******************************************************************************/
class Versal_2ve_2vmAuthenticationContext : public AuthenticationContext
{
public:
    Versal_2ve_2vmAuthenticationContext(Authentication::Type type);
    Versal_2ve_2vmAuthenticationContext(const AuthenticationContext* refAuthContext, Authentication::Type authtype);
    Versal_2ve_2vmAuthenticationContext(const AuthCertificate4096Sha3PaddingHBStructure* existingCert, Authentication::Type authtype);
    ~Versal_2ve_2vmAuthenticationContext();

    void Link(BootImage& bi, void* partition, AuthenticationCertificate* cert);
    void Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert);

    uint32_t getCertificateSize(void) { return certSize; }
    void AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header);
    Section* CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection, bool isBootloader);
    void GenerateIHTHash(BootImage& bi, uint8_t* sha_hash_padded);
    void GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded);
    void GenerateSPKHash(uint8_t * sha_hash_padded);
    void GeneratePPKHash(const std::string& filename);
    void CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, size_t hashBlockLength, uint8_t* signatureBlock, Section* acSection);
    void CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection);
    static void GetPresign(const std::string& presignFilename, uint16_t signatureLength, uint8_t* signature, uint32_t index);
    static void GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index);
    void SetSPKSignatureFile(const std::string& filename);
    void SetBHSignatureFile(const std::string& filename);
    void GenerateSPKSignature(const std::string& filename);
    void ResizeIfNecessary(Section* section);
    void LoadUdfData(const std::string& filename, uint8_t* signature);
    void CreateSPKSignature(BootImage& bi);
    void CreateAuthJtagImage(Options& options, uint8_t * buffer, AuthJtagInfo authJtagAttributes);
    uint32_t GetAuthJtagImageSize(void) const;
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
class Versal_2ve_2vmAuthenticationCertificate : public AuthenticationCertificate
{
public:
    Versal_2ve_2vmAuthenticationCertificate(AuthenticationContext* context) : AuthenticationCertificate(context) {}
    Section* AttachBootHeaderToFsbl(BootImage& bi) { return NULL; }
    void Link(BootImage& bi, void* partition);  // IMAGE_STORE: partition pointer overload
    void Link(BootImage& bi, Section* section);
    //AuthCertificate4096Sha3PaddingStructure *acStructure;
};
#endif
