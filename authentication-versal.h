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
#pragma once

#ifndef _VERSAL_AUTHENTICATION_CONTEXT_H_
#define _VERSAL_AUTHENTICATION_CONTEXT_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <string>
#include <list>
#include <string.h>
#include "binary.h"
#include "baseclass.h"
#include "bootgenenum.h"
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#include "authkeys-versal.h"
#include "hash.h"
#include "systemutils.h"
#include "authentication.h"
#include "bifoptions.h"

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
#define AUTH_HDR_VERSAL             0x115
#define AUTH_HDR_VERSAL_ECDSA       0x106
#define AUTH_HDR_VERSAL_ECDSA_P521  0x126
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

typedef enum
{
    vauthJtagMessageShift = 0,
    vauthJtagMessagenMask = 0x3,
} AuthJtagAttributes;

typedef struct
{
    uint32_t              acHeader;                // 0x000
    uint32_t              spkId;                   // 0x004
    uint8_t               acUdf[UDF_DATA_SIZE];    // 0x008
    ACKey4096Sha3Padding  acPpk;                   // 0x040
    uint8_t               ppkSHA3Padding[12];      // 0x444
    ACKey4096Sha3Padding  acSpk;                   // 0x450
    uint8_t               spkSHA3Padding[4];       // 0x854
    uint8_t               allignment[8];           // 0x858
    ACSignature4096       acSpkSignature;          // 0x860
    ACSignature4096       acHeaderSignature;       // 0xA60
    ACSignature4096       acPartitionSignature;    // 0xC60
} AuthCertificate4096Sha3PaddingStructure;

/* ECDSA Authentication Certificate */
typedef struct
{
    uint32_t            acHeader;                  // 0x000
    uint32_t            spkId;                     // 0x004
    uint8_t             acUdf[UDF_DATA_SIZE];      // 0x008
    ACKeyECDSA          acPpk;                     // 0x040
    uint8_t             ppkSHA3Padding[12];        // 0x444
    ACKeyECDSA          acSpk;                     // 0x450
    uint8_t             spkSHA3Padding[4];         // 0x854
    uint8_t             allignment[8];             // 0x858
    ACSignatureECDSA    acSpkSignature;            // 0x860
    ACSignatureECDSA    acHeaderSignature;         // 0xA60
    ACSignatureECDSA    acPartitionSignature;      // 0xC60
} AuthCertificateECDSAStructure;

typedef struct
{
    uint32_t            acHeader;                  // 0x000
    uint32_t            spkId;                     // 0x004
    uint8_t             acUdf[UDF_DATA_SIZE];      // 0x008
    ACKeyECDSAP521      acPpk;                     // 0x040
    uint8_t             ppkSHA3Padding[12];        // 0x444
    ACKeyECDSAP521      acSpk;                     // 0x450
    uint8_t             spkSHA3Padding[4];         // 0x854
    uint8_t             allignment[8];             // 0x858
    ACSignatureECDSA    acSpkSignature;            // 0x860
    ACSignatureECDSA    acHeaderSignature;         // 0xA60
    ACSignatureECDSA    acPartitionSignature;      // 0xC60
} AuthCertificateECDSAp521Structure;

/* Authenticated-Jtag Image Structure */
typedef struct
{
    uint32_t       acHeader;                                //0x00
    uint32_t       spkId;                                   //0x04
    uint8_t        authJtagInfo[96];                        //0x08
    uint8_t        acPpk[VERSAL_ACKEY_STRUCT_SIZE];         //0x68
    uint8_t        ppkSHA3Padding[12];                      //0x46C
    uint8_t        authJtagSignature[SIGN_LENGTH_VERSAL];   //0x478
} AuthenticatedJtagImageStructure;                          //0x678

typedef struct
{
    uint32_t             attributes;        //0x08
    uint8_t              deviceDNA[16];     //0x0C
    uint32_t             jtagTimeOut;       //0x1C
    uint8_t              SHA3Padding[72];   //0x20
} authJtagInfoStructurev2;

typedef struct
{
    uint8_t              SHA3Padding[96];
} authJtagInfoStructurev1;

/******************************************************************************/
class ECDSAAuthenticationAlgorithm : public AuthenticationAlgorithm
{
public:
    ECDSAAuthenticationAlgorithm();
    ~ECDSAAuthenticationAlgorithm();

    Authentication::Type Type()
    {
        return Authentication::ECDSA;
    }

    int KeySize()
    {
        return sizeof(ACKeyECDSA);
    }

    void CreateSignature(const uint8_t *base, uint8_t* primaryKey, uint8_t *result0);
    uint32_t getCertificateSize() { return certSize; }
    void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength);
    void ECDSASignature(const uint8_t *base, EC_KEY *eckey, uint8_t *result0);
    Authentication::Type authType;
    void RearrangeEndianess(uint8_t *array, uint32_t size);
    uint32_t GetAuthHeader(void);

private:
    uint32_t certSize;
};

/******************************************************************************/
class ECDSAP521AuthenticationAlgorithm : public AuthenticationAlgorithm
{
public:
    ECDSAP521AuthenticationAlgorithm();
    ~ECDSAP521AuthenticationAlgorithm();

    Authentication::Type Type()
    {
        return Authentication::ECDSAp521;
    }

    int KeySize()
    {
        return sizeof(ACKeyECDSAP521);
    }

    void CreateSignature(const uint8_t *base, uint8_t* primaryKey, uint8_t *result0);
    uint32_t getCertificateSize() { return certSize; }
    void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength);
    void ECDSASignature(const uint8_t *base, EC_KEY *eckey, uint8_t *result0);
    Authentication::Type authType;
    void RearrangeEndianess(uint8_t *array, uint32_t size);
    uint32_t GetAuthHeader(void);

private:
    uint32_t certSize;
};

/******************************************************************************/
class RSA4096Sha3PaddingAuthenticationAlgorithm : public AuthenticationAlgorithm
{
public:
    RSA4096Sha3PaddingAuthenticationAlgorithm();

    ~RSA4096Sha3PaddingAuthenticationAlgorithm();

    Authentication::Type Type()
    {
        return Authentication::RSA;
    }

    void CreateSignature(const uint8_t *base, uint8_t* primaryKey, uint8_t *result0);
    uint32_t getCertificateSize(void) { return certSize; }
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
class VersalAuthenticationContext : public AuthenticationContext
{
public:
    VersalAuthenticationContext(Authentication::Type type);
    VersalAuthenticationContext(const AuthenticationContext* refAuthContext, Authentication::Type authtype);
    VersalAuthenticationContext(const AuthCertificate4096Sha3PaddingStructure* existingCert, Authentication::Type authtype);
    ~VersalAuthenticationContext();

    void Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert);

    uint32_t getCertificateSize(void) { return certSize; }
    void AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header);
    Section* CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection);
    void GenerateIHTHash(BootImage& bi, uint8_t* sha_hash_padded);
    void GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded);
    void GenerateSPKHash(uint8_t * sha_hash_padded);
    void GeneratePPKHash(const std::string& filename);
    void CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection);
    static void GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index);
    void SetSPKSignatureFile(const std::string& filename);
    void SetBHSignatureFile(const std::string& filename);
    void GenerateSPKSignature(const std::string& filename);
    void ResizeIfNecessary(Section* section);
    void LoadUdfData(const std::string& filename, uint8_t* signature);
    void CreateSPKSignature(void);
    void CreateAuthJtagImage(uint8_t * buffer, AuthJtagInfo authJtagAttributes);
    void SetKeyLength(Authentication::Type type);
    AuthenticationAlgorithm* GetAuthenticationAlgorithm(Authentication::Type type);
    uint32_t GetCertificateSize();

private:
    void CopybHSignature(BootImage& bi, uint8_t* ptr);
    void CopyIHTSignature(BootImage& bi, uint8_t* ptr);
    uint32_t certSize;
    void CopySPKSignature(uint8_t* ptr);
    std::string GetCertificateName(std::string name);
};

/******************************************************************************/
class VersalAuthenticationCertificate : public AuthenticationCertificate
{
public:
    VersalAuthenticationCertificate(AuthenticationContext* context) : AuthenticationCertificate(context) {}
    Section* AttachBootHeaderToFsbl(BootImage& bi) { return NULL; }
    void Link(BootImage& bi, Section* section);
    //AuthCertificate4096Sha3PaddingStructure *acStructure;
};
#endif
