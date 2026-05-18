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
#ifndef _AUTHENTICATION_H_
#define _AUTHENTICATION_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <string>
#include <list>
#include <string.h>
#include <vector>
#include <memory>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <iomanip>
#include <iostream>

#include "binary.h"
#include "baseclass.h"
#include "bootgenenum.h"
#include "bifoptions.h"
//#include "authkeys.h"
#include "hash.h"
#include "systemutils.h"

#if OPENSSL_VERSION_NUMBER > 0x10100000L
#include "xil-bignum.h"
#endif

/* Forward class references */
class BaseThing;
class Section;
class BootImage;
class Binary;
class AuthenticationCertificate;
class PartitionHeader;
class Key;
class VersalKey;


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define T_PAD_LENGTH            19
#define UDF_DATA_SIZE           56
#define AUTH_HEADER             0x115

#define RSA_2048_KEY_LENGTH     256
#define RSA_4096_KEY_LENGTH     512
#define EC_P384_KEY_LENGTH      48
#define EC_P521_KEY_LENGTH1     65 /* 520/8 = 65 */
#define EC_P521_KEY_LENGTH2     66 /* 521/8 = 65 + 1bit */

#define SIGN_LENGTH_VERSAL      512 /* for ecdsa: r(48b)+ s(48b)+ pad(416b) */
#define RSA_SIGN_LENGTH_ZYNQ    256
#define RSA_SIGN_LENGTH_ZYNQMP  512

#define RSA_4096_N_SIZE         512
#define RSA_4096_N_EXT_SIZE     512
#define RSA_4096_E_SIZE         4
/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/

#define TELLURIDE_AC_SPK_HDR_ALIGNMENT          12
// acTotalSpkSize; acActualSpkSize; acSpkTotalSignatureSize; acSpkActualSignatureSize; acSpkId; acSpkHdrAlignment[3];
#define TELLURIDE_AC_SPK_HDR_LENGTH    (8 * sizeof(uint32_t))

#define TELLURIDE_RSA_AC_PPK_SPK_ALIGNMENT      12

#define TELLURIDE_RSA_AC_PPK_OFFSET                 (0x00)
#define TELLURIDE_RSA_AC_PPK_ALIGNMENT_OFFSET       (0x404)
#define TELLURIDE_RSA_AC_SPK_TOTAL_SIZE_OFFSET      (0x410)
#define TELLURIDE_RSA_AC_SPK_ACTUAL_SIZE_OFFSET     (0x414)
#define TELLURIDE_RSA_AC_SPK_TOTAL_SIGN_SIZE_OFFSET (0x418)
#define TELLURIDE_RSA_AC_SPK_ACTUAL_SIGN_SIZE_OFFSET (0x41C)
#define TELLURIDE_RSA_AC_SPK_ID_OFFSET              (0x420)
#define TELLURIDE_RSA_AC_SPK_HDR_ALIGNMENT_OFFSET   (0x424)
#define TELLURIDE_RSA_AC_SPK_OFFSET                 (0x430)
#define TELLURIDE_RSA_AC_SPK_ALIGNMENT_OFFSET       (0x834)
#define TELLURIDE_RSA_AC_SPK_SIGN_OFFSET            (0x840)

#define TELLURIDE_EC_P384_AC_PPK_OFFSET                  (0x00)
#define TELLURIDE_EC_P384_AC_TOTAL_SPK_SIZE_OFFSET       (0x60)
#define TELLURIDE_EC_P384_AC_ACTUAL_SPK_SIZE_OFFSET      (0x64)
#define TELLURIDE_EC_P384_AC_TOTAL_SPK_SIGN_SIZE_OFFSET  (0x68)
#define TELLURIDE_EC_P384_AC_ACTUAL_SPK_SIGN_SIZE_OFFSET (0x6C)
#define TELLURIDE_EC_P384_AC_SPK_ID_OFFSET               (0x70)
#define TELLURIDE_EC_P384_AC_SPK_HDR_ALIGNMENT_OFFSET    (0x74)
#define TELLURIDE_EC_P384_AC_SPK_OFFSET                  (0x80)
#define TELLURIDE_EC_P384_AC_SPK_SIGN_OFFSET             (0xE0)

#define PADDING_16B(offset)                                     (((offset % 16) == 0) ? 0 : (16 - (offset % 16)))
#define TELLURIDE_LMS_AC_PPK_OFFSET                             0x00
#define TELLURIDE_LMS_AC_PPK_ALIGN_OFFSET(lmsOnly)              TELLURIDE_LMS_AC_PPK_OFFSET + GetLmsPublicKeyLength(ppkFile.c_str(),lmsOnly) // 0x3c
#define TELLURIDE_LMS_AC_TOTAL_SPK_SIZE_OFFSET(lmsOnly)         TELLURIDE_LMS_AC_PPK_ALIGN_OFFSET(lmsOnly) + PADDING_16B(GetLmsPublicKeyLength(ppkFile.c_str(),lmsOnly))    // 0x40
#define TELLURIDE_LMS_AC_ACTUAL_SPK_SIZE_OFFSET(lmsOnly)        TELLURIDE_LMS_AC_TOTAL_SPK_SIZE_OFFSET(lmsOnly) + sizeof(uint32_t)           // 0x44
#define TELLURIDE_LMS_AC_TOTAL_SPK_SIGN_SIZE_OFFSET(lmsOnly)    TELLURIDE_LMS_AC_ACTUAL_SPK_SIZE_OFFSET(lmsOnly) + sizeof(uint32_t)          // 0x48
#define TELLURIDE_LMS_AC_ACTUAL_SPK_SIGN_SIZE_OFFSET(lmsOnly)   TELLURIDE_LMS_AC_TOTAL_SPK_SIGN_SIZE_OFFSET(lmsOnly) + sizeof(uint32_t)      // 0x4c
#define TELLURIDE_LMS_AC_SPK_ID_OFFSET(lmsOnly)                 TELLURIDE_LMS_AC_ACTUAL_SPK_SIGN_SIZE_OFFSET(lmsOnly) + sizeof(uint32_t)     // 0x50
#define TELLURIDE_LMS_AC_SPK_HDR_ALIGNMENT_OFFSET(lmsOnly)      TELLURIDE_LMS_AC_SPK_ID_OFFSET(lmsOnly) + sizeof(uint32_t)                   // 0x54
#define TELLURIDE_LMS_AC_SPK_OFFSET(lmsOnly)                    TELLURIDE_LMS_AC_SPK_HDR_ALIGNMENT_OFFSET(lmsOnly) + (3*sizeof(uint32_t))    // 0x60
#define TELLURIDE_LMS_AC_SPK_ALIGN_OFFSET(lmsOnly)              TELLURIDE_LMS_AC_SPK_OFFSET(lmsOnly) + GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly)                      // 0x9c
#define TELLURIDE_LMS_AC_SPK_SIGN_OFFSET(lmsOnly)               TELLURIDE_LMS_AC_SPK_ALIGN_OFFSET(lmsOnly) + PADDING_16B(GetLmsPublicKeyLength(spkFile.c_str(),lmsOnly))    // 0xa0
//#define TELLURIDE_LMS_AC_SPK_SIGN_ALIGN_OFFSET          TELLURIDE_LMS_AC_SPK_SIGN_OFFSET + sizeof(HssSignature)             // 0x2774

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class RSA_Class 
{
public:
    RSA_Class() 
    {
        rsa = NULL;
    }
    
    ~RSA_Class() 
    {
        if(rsa != NULL) 
        {
            RSA_free(rsa);
        }
    }
    RSA* rsa;
};

/******************************************************************************/
class BN_CTX_Class
{
public:
    BN_CTX_Class()
    {
        ctx = BN_CTX_new();
        if (ctx == NULL)
        {
            std::string logmsg = "Failed to allocate BN_CTX_new";
            LOG_ERROR(logmsg.c_str());
        }
    }

    ~BN_CTX_Class()
    {
        if (ctx != NULL)
        {
            BN_CTX_free(ctx);
        }
    }
    BN_CTX* ctx;
};

/******************************************************************************/
class BN_MONT_CTX_Class 
{
public:
    BN_MONT_CTX_Class(BN_CTX_Class& ctxInst) 
    {
        mont=BN_MONT_CTX_new();
        if (mont == NULL) 
        {
            LOG_ERROR("Failed to allocate BN_MONT_CTX_new");
        }
        ctx = ctxInst.ctx;
    }

    ~BN_MONT_CTX_Class() 
    {
        if (mont != NULL) 
        {
            BN_MONT_CTX_free(mont);
        }
    }

    /* Pointer overload allows passing BIGNUMs created via public OpenSSL API
       (BN_lebin2bn, BN_bin2bn) where direct struct access is unavailable on MSVC. */
    void Set(BIGNUM* m) 
    {
        if (!BN_MONT_CTX_set(mont, m, ctx)) 
        {
            LOG_ERROR("Failed to calculated montgomery reduction in BN_MONT_CTX_set");
        }
    }

    void Set(BIGNUM& m) 
    {
        Set(&m);
    }

    void GetModulusExtension(uint8_t* ptr, BIGNUM& m, size_t len) 
    {
        if(len == RSA_2048_KEY_LENGTH)
        {
            if (len != mont->RR.top * sizeof(BN_ULONG)) 
            {
                LOG_ERROR("Sanity check in GetModulusExtension Failed");
            }
            memcpy(ptr,mont->RR.d,len);
        }
        else
        {
            BIGNUM *r = BN_new(); 
            BIGNUM *res = BN_new();
            BIGNUM *m_x = BN_new();
            if( r == NULL || res == NULL || m_x == NULL)
            {
                LOG_ERROR("Failed to allocate BN_new");
            }
            BN_one(r);
            BN_lshift(res, r, 4160);
            BN_mod_mul(m_x, res, res, &m, ctx);
            memcpy(ptr, m_x->d, RSA_4096_KEY_LENGTH);
            BN_free(r);
            BN_free(res);
            BN_free(m_x);
        }
    }

    BN_CTX* ctx;
    BN_MONT_CTX* mont;
};

/******************************************************************************/
class AuthenticationAlgorithm
{
public:
    AuthenticationAlgorithm() { };
    virtual ~AuthenticationAlgorithm() { };

    virtual Authentication::Type Type() = 0;
    virtual void CreateSignature(const uint8_t *base, uint8_t* primaryKey, uint8_t *result0) {};
    virtual void CreateSignature(const uint8_t *buffer, size_t buffer_len, const char* keyfile, 
        uint8_t *result0, size_t result0_len, bool lmsOnly, const char* publicKeyfile) {};
    virtual void VerifySignature(const uint8_t *buffer, size_t buf_len, const char* keyfile, 
        uint8_t *result0, size_t result0_len, bool lmsOnly) {};

    void RSA_Exponentiation(const uint8_t *base, const uint8_t* modular, const uint8_t *modular_ext, const uint8_t *exponent, uint8_t *result0);
    virtual void RearrangeEndianess(uint8_t *array, uint32_t size) {};
    virtual void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength) {};
    virtual uint32_t GetAuthHeader(void) { return AUTH_HEADER; }
    virtual uint32_t GetAuthHeader(bool) { return AUTH_HEADER; }
    virtual uint32_t GetAuthHeader(bool, bool, bool) { return AUTH_HEADER; }
    virtual int KeySize() { return 0; }
};

/******************************************************************************/
class RSAAuthenticationAlgorithm : public AuthenticationAlgorithm
{
public:
    RSAAuthenticationAlgorithm() { };
    ~RSAAuthenticationAlgorithm() { };

    Authentication::Type Type()
    {
        return Authentication::RSA;
    }
    void CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0);
};

/******************************************************************************/
class NoneAuthenticationAlgorithm : public AuthenticationAlgorithm
{
public:
    NoneAuthenticationAlgorithm() { };
    ~NoneAuthenticationAlgorithm() { };

    Authentication::Type Type()
    {
        return Authentication::None;
    }
};

/******************************************************************************/
class AuthenticationContext 
{
public:
    AuthenticationContext()
        : primaryKey(nullptr)
        , secondaryKey(nullptr)
        , ownsPrimaryKey(false)
        , ownsSecondaryKey(false)
        , hash(nullptr)
        , ownsHash(false)
        , spkSignLoaded(false)
        , bhSignLoaded(false)
        , spksignature(nullptr)
        , bHsignature(nullptr)
        , hashType(AuthHash::Sha2)
        , ppkSelect(0)
        , spkSelect(1)
        , spkIdentification(0)
        , acIndex(0)
        , authBlocks(0)
        , certIndex(0)
        , preSigned(false)
        , isHeaderAuthentication(false)
        , authAlgorithm(nullptr)
        , ownsAuthAlgorithm(false)
        , authCertificate (nullptr)
        , ppkFile("")
        , pskFile("")
        , spkFile("")
        , sskFile("")
        , firstChunkSize(0)
        , signatureLength(0)
        , lmsOnly(true)
        , primaryLmsParams(NULL)
        , primaryLmsParamsSize(0)
        , secondaryLmsParams(NULL)
        , secondaryLmsParamsSize(0)
    { };

    virtual ~AuthenticationContext() { 
        if (!ownsPrimaryKey && primaryKey != nullptr) {
            primaryKey.release();  // Don't delete non-owned key
        }
        if (!ownsSecondaryKey && secondaryKey != nullptr) {
            secondaryKey.release();  // Don't delete non-owned key
        }
        if (!ownsAuthAlgorithm && authAlgorithm != nullptr) {
            authAlgorithm.release();  // Don't delete non-owned authAlgorithm
        }
        if (ownsHash && hash != nullptr) {
            delete hash;  // Clean up owned hash
            hash = nullptr;
        }
        // spksignature and bHsignature are unique_ptr - automatically cleaned up
    };
    virtual uint32_t GetCertificateSize() { return 0; }
    virtual uint32_t GetTotalHashBlockSignSize(void) { return 0; };
    virtual void ResizeIfNecessary(Section* section);
    virtual Section* CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection, bool isBootloader) { return NULL; }
    virtual void Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert) {  }
    virtual void Link(BootImage& bi, void* partition, AuthenticationCertificate* cert) {  }
    virtual void GeneratePPKHash(const std::string& filename) {}
    virtual void GenerateSPKHash(uint8_t* sha256_hash_padded) {}
    virtual void GenerateSPKHashFile(const std::string& filename, Hash* hashObj);
    virtual void GenerateSPKSignature(const std::string& filename);
    virtual void GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded) {};
    virtual void GenerateBHSignature() {};
    virtual void CreatePadding(uint8_t* signature, const uint8_t* hash) {};
    virtual void CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection) {};
    virtual void RearrangeEndianess(uint8_t *array, uint32_t size) {};
    virtual void AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header) {};
    virtual void CopySPKSignature(uint8_t* ptr) {};
    virtual void SetKeyLength(Authentication::Type type) {};

    void CreateSPKSignature(void);
    virtual void SetSPKSignatureFile(const std::string & filename);
    void ParseSPKSignatureFile(const std::string & filename);
    void SetBHSignatureFile(const std::string & filename);
    void ParseBHSignatureFile(const std::string & filename);
    void GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index);
    void GetAC(const std::string& presignFilename, uint8_t* signature, uint32_t index);
    void LoadUdfData(const std::string & udfFilename, uint8_t * signature);
    void WritePaddedSHAFile(const uint8_t * shaBuf, const std::string & hashfilename);
    void WritePaddedSHAFile(const uint8_t* buffer, size_t bufferLen, const std::string& hashfilename);
    void WriteHashFile(const uint8_t* shaBuf, const std::string& hashfilename, bool isHeader);

    void SetPresignFile(const std::string& filename);
    void SetACFile(const std::string& filename);
    void SetUdfFile(const std::string& filename);
    void SetPSKeyFile(const std::string& filename);
    void ParsePSKeyFile(const std::string& filename);
    void SetPPKeyFile(const std::string& filename);
    void ParsePPKeyFile(const std::string & filename);
    void SetSSKeyFile(const std::string& filename);
    void ParseSSKeyFile(const std::string& filename);
    void SetSPKeyFile(const std::string& filename);
    void ParseSPKeyFile(const std::string& filename);
    void SetPpkSelect(uint32_t ppk);
    void SetSpkSelect(uint32_t spk);
    void SetSpkIdentification(uint32_t spkid);
    void SetHeaderAuthentication(uint32_t headerauth);
    void SetHashType(AuthHash::Type);
    void SetFirstChunkSize(uint64_t);
    AuthHash::Type GetHashType(void);
    uint64_t GetFirstChunkSize(void) { return firstChunkSize; }

    static void SetAuthenticationKeyLength(uint16_t);
    static uint16_t GetRsaKeyLength(void);
    void SetSignatureLength(uint16_t signLength);
    uint16_t GetSignatureLength(void);
    static size_t GetauthBlocks(size_t authblocks);
    static bool GetZynpMpVerEs1Flag(void);
    static void SetZynpMpVerEs1Flag(bool isEs1);

    int acIndex;
    int certIndex;
    Hash* hash;  // Sometimes owned, sometimes non-owning pointer to bi.hash
    bool ownsHash;  // Track if we own the hash (created with new) vs non-owning (bi.hash.get())
    uint8_t udf_data[UDF_DATA_SIZE];
    bool spkSignLoaded;
    bool bhSignLoaded;
    std::string spkSignRequested;
    std::unique_ptr<uint8_t[]> spksignature;
    std::unique_ptr<uint8_t[]> bHsignature;
    AuthHash::Type hashType;
    size_t authBlocks;
    bool preSigned;
    std::string ppkFile;
    std::string pskFile;
    std::string spkFile;
    std::string sskFile;
    uint32_t ppkSelect;
    uint32_t spkSelect;
    uint32_t spkIdentification;
    bool isHeaderAuthentication;
    std::string spkSignFile;
    std::string bhSignFile;
    std::unique_ptr<Key> primaryKey;
    bool ownsPrimaryKey;     // Track if this context owns primaryKey for cleanup
    std::unique_ptr<Key> secondaryKey;
    bool ownsSecondaryKey;   // Track if this context owns secondaryKey for cleanup
    std::unique_ptr<AuthenticationAlgorithm> authAlgorithm;
    bool ownsAuthAlgorithm;  // Track if this context owns authAlgorithm for cleanup
    std::unique_ptr<AuthenticationCertificate> authCertificate;
    uint16_t signatureLength;
    std::string presignFile;
    std::string acFile;
    std::string udfFile;
    bool lmsOnly;
    int* primaryLmsParams;
    int primaryLmsParamsSize;
    int* secondaryLmsParams;
    int secondaryLmsParamsSize;
    uint8_t hashLength;  // CRITICAL: Instance variable, NOT static (each context has its own)
protected:
    static uint16_t authKeyLength;
    static bool zynpmpVerEs1;
    uint64_t firstChunkSize;
};

/******************************************************************************/
class NoneAuthenticationContext  : public AuthenticationContext
{
public:
    NoneAuthenticationContext();
    void ResizeIfNecessary(Section* section) {};
};


/******************************************************************************/
class AuthenticationCertificate : public BaseThing
{
public:
    AuthenticationCertificate(AuthenticationContext* context);
    AuthenticationCertificate() {};
    virtual ~AuthenticationCertificate() {}  // Virtual destructor to avoid warnings
    void Build(BootImage& bi, Binary& cache, Section* section, bool fsbl, bool isTableHeader);
    virtual void Link(BootImage& bi, void* partition);  // IMAGE_STORE: partition pointer overload
    virtual void Link(BootImage& bi, Section* section);
    virtual Section* AttachBootHeaderToFsbl(BootImage& bi) { return NULL; }

    AuthenticationContext* AuthContext;
    bool fsbl;
    bool isTableHeader;
};

/******************************************************************************/
class RSA2048AuthenticationCertificate : public AuthenticationCertificate
{
public:
    RSA2048AuthenticationCertificate(AuthenticationContext* context) : AuthenticationCertificate(context) { }
    RSA2048AuthenticationCertificate() { };
    Section* AttachBootHeaderToFsbl(BootImage& bi);
};

/******************************************************************************/
class RSA4096AuthenticationCertificate : public AuthenticationCertificate
{
public:
    RSA4096AuthenticationCertificate(AuthenticationContext* context) : AuthenticationCertificate(context) { }
    RSA4096AuthenticationCertificate() { };
};
#endif