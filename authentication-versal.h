/******************************************************************************
* Copyright 2015-2020 Xilinx, Inc.
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
#define AC_HDR_PPK_SELECT_BIT_SHIFT 16
#define EC_SIGN_LENGTH_VERSAL       512

#define AC_HDR_SHA_2_3_BIT_SHIFT    2
#define SHA3_PAD_LENGTH             104

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

typedef struct
{
} AuthCertificate4096Sha3PaddingStructure;

/* ECDSA Authentication Certificate */
typedef struct
{
} AuthCertificateECDSAStructure;

/* RSA Secure-Debug Image Structure */
typedef struct
{
} SecureDebugImage4096Sha3PaddingStructure;

/* ECDSA Secure-Debug Image Structure */
typedef struct
{
} SecureDebugImageECDSAStructure;

/******************************************************************************/
class ECDSAAuthenticationAlgorithm : public AuthenticationAlgorithm
{
public:
    ECDSAAuthenticationAlgorithm() { }
    ~ECDSAAuthenticationAlgorithm() { }

    Authentication::Type Type() { return Authentication::ECDSA; }
    void CreateSignature(const uint8_t *base, uint8_t* primaryKey, uint8_t *result0) { }
    uint32_t getCertificateSize() { return certSize; }
    void ECDSASignature(const uint8_t *base, EC_KEY *eckey, uint8_t *result0) { }
    void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength) {}
    void RearrangeEndianess(char *array, uint32_t size) {}

    uint32_t GetAuthHeader(void) { return 0; }
    Authentication::Type authType;

private:
    uint32_t certSize;
};

/******************************************************************************/
class RSA4096Sha3PaddingAuthenticationAlgorithm : public AuthenticationAlgorithm
{
public:
    RSA4096Sha3PaddingAuthenticationAlgorithm() { }
    ~RSA4096Sha3PaddingAuthenticationAlgorithm() { }

    Authentication::Type Type() { return Authentication::RSA; }
    void CreateSignature(const uint8_t *base, uint8_t* primaryKey, uint8_t *result0) { }
    uint32_t getCertificateSize(void) { return certSize; }
    void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength) { }
    void RearrangeEndianess(char *array, uint32_t size) { }
    uint8_t* AttachSHA3Padding(uint8_t * data, const Binary::Length_t datalength) { return NULL; }
    int mgf(unsigned char *mask, long len, const unsigned char *seed, long seedlen, const EVP_MD *dgst) { return 0; }
    uint32_t GetAuthHeader(void) { return 0; }

    Authentication::Type authType;
private:
    uint32_t certSize;
};

/******************************************************************************/
class VersalAuthenticationContext : public AuthenticationContext
{
public:
    VersalAuthenticationContext(Authentication::Type type)
    {
        LOG_ERROR("Authentication is not supported for Versal ACAP in this version of Bootgen. \n\
           Please use Bootgen from Xilinx install");
    }
    VersalAuthenticationContext(const AuthenticationContext* refAuthContext, Authentication::Type authtype) 
    {
        LOG_ERROR("Authentication is not supported for Versal ACAP in this version of Bootgen. \n\
           Please use Bootgen from Xilinx install");
    }
    VersalAuthenticationContext(const AuthCertificate4096Sha3PaddingStructure* existingCert, Authentication::Type authtype) 
    {
        LOG_ERROR("Authentication is not supported for Versal ACAP in this version of Bootgen. \n\
           Please use Bootgen from Xilinx install");
    }
    VersalAuthenticationContext() { }

    void Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert) { }
    uint32_t getCertificateSize(void) { return certSize; }
    void AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header) { }
    Section* CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection) { return NULL; }
    void GenerateIHTHash(BootImage& bi, uint8_t* sha_hash_padded) { }
    void GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded) { }
    void GenerateSPKHash(uint8_t * sha_hash_padded) { }
    void GeneratePPKHash(const std::string& filename) { }
    void CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection) { }
    static void GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index) { }
    void SetSPKSignatureFile(const std::string& filename) { }
    void SetBHSignatureFile(const std::string& filename) { }
    void GenerateSPKSignature(const std::string& filename) { }
    void ResizeIfNecessary(Section* section) { }
    void LoadUdfData(const std::string& filename, uint8_t* signature) { }
    void CreateSPKSignature(void) { }
    void CalculateAcHdrHash(uint8_t* sha_hash_padded, uint8_t*  buffer) { }
    void CreateAcHdrSignature(uint8_t *buffer) { }
    void SetKeyLength(Authentication::Type type) { }
    AuthenticationAlgorithm* GetAuthenticationAlgorithm(Authentication::Type type) { return NULL; }
    void GenerateSPKHashFile(const std::string& filename, Hash* hashObj) { }
    uint32_t GetCertificateSize() { return 0; }

protected:
    void WritePaddedSHAFile(const uint8_t* sha256, const std::string& hashfilename) { }

private:
    uint32_t certSize;

    void CopybHSignature(BootImage& bi, uint8_t* ptr) { }
    void CopyIHTSignature(BootImage& bi, uint8_t* ptr) { }
    void CopySPKSignature(uint8_t* ptr) { }
    std::string GetCertificateName(std::string name) { return ""; }
};

/******************************************************************************/
class VersalAuthenticationCertificate : public AuthenticationCertificate
{
public:
    VersalAuthenticationCertificate(AuthenticationContext* context) : AuthenticationCertificate(context) { }
    Section* AttachBootHeaderToFsbl(BootImage& bi) { return NULL; }
    void Link(BootImage& bi, Section* section) { }
    //AuthCertificate4096Sha3PaddingStructure *acStructure;
};
#endif
