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
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <iomanip>
#include <iostream>

#include "binary.h"
#include "baseclass.h"
#include "bootgenenum.h"
#include "authkeys.h"
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

/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/

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

    void Set(BIGNUM& m) 
    {
        if (!BN_MONT_CTX_set(mont,&m, ctx)) 
        {
            LOG_ERROR("Failed to calculated montgomery reduction in BN_MONT_CTX_set");
        }
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
    ~AuthenticationAlgorithm() { };

    virtual Authentication::Type Type() = 0;
    virtual void CreateSignature(const uint8_t *base, uint8_t* primaryKey, uint8_t *result0) {};
    void RSA_Exponentiation(const uint8_t *base, const uint8_t* modular, const uint8_t *modular_ext, const uint8_t *exponent, uint8_t *result0);
    virtual void RearrangeEndianess(uint8_t *array, uint32_t size) {};
    virtual void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength) {};
    virtual uint32_t GetAuthHeader(void) { return AUTH_HEADER; }
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
        : primaryKey(NULL)
        , secondaryKey(NULL)
        , hash(NULL)
        , spkSignLoaded(false)
        , bhSignLoaded(false)
        , spksignature(NULL)
        , bHsignature(NULL)
        , hashType(AuthHash::Sha2)
        , ppkSelect(0)
        , spkSelect(1)
        , spkIdentification(0)
        , acIndex(0)
        , authBlocks(0)
        , certIndex(0)
        , preSigned(false)
        , isHeaderAuthentication(false)
        , authAlgorithm(NULL)
        , authCertificate (NULL)
        , ppkFile("")
        , pskFile("")
        , spkFile("")
        , sskFile("")
        , firstChunkSize(0)
        , signatureLength(0)
    { };

    virtual ~AuthenticationContext() { };
    virtual uint32_t GetCertificateSize() { return 0; }
    virtual void ResizeIfNecessary(Section* section);
    virtual Section* CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection) { return NULL; }
    virtual void Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert) {  }
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
    void SetSPKSignatureFile(const std::string & filename);
    void ParseSPKSignatureFile(const std::string & filename);
    void SetBHSignatureFile(const std::string & filename);
    void ParseBHSignatureFile(const std::string & filename);
    void GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index);
    void LoadUdfData(const std::string & udfFilename, uint8_t * signature);
    void WritePaddedSHAFile(const uint8_t * shaBuf, const std::string & hashfilename);

    void SetPresignFile(const std::string& filename);
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
    Hash *hash;
    uint8_t udf_data[UDF_DATA_SIZE];
    bool spkSignLoaded;
    bool bhSignLoaded;
    std::string spkSignRequested;
    uint8_t* spksignature;
    uint8_t* bHsignature;
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
    Key *primaryKey;
    Key *secondaryKey;
    AuthenticationAlgorithm *authAlgorithm;
    AuthenticationCertificate *authCertificate;
    uint16_t signatureLength;
    std::string presignFile;
    std::string udfFile;
protected:
    static uint16_t authKeyLength;
    static uint8_t hashLength;
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
    void Build(BootImage& bi, Binary& cache, Section* section, bool fsbl, bool isTableHeader);
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
