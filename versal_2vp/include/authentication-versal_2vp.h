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

#ifndef _VERSAL_2VP_AUTHENTICATION_CONTEXT_H_
#define _VERSAL_2VP_AUTHENTICATION_CONTEXT_H_


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
//#include "baseclass.h"
#include "bootgenenum.h"
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#include "authkeys-versal_2vp.h"
//#include "hash.h"
//#include "systemutils.h"
#include "authentication.h"
//#include "bifoptions.h"
//#include "lms.h"

/* Forward class references */
class BaseThing;
class Section;
class BootImage;
class PartitionBifOptions;
class Binary;
class AuthenticationCertificate;
class PartitionHeader;
class Key;
struct KeyMetadata;

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


//versal_2vp authentication header defines
#define AUTH_HDR_TELLURIDE_NONSECURE     0x00       
#define AUTH_HDR_TELLURIDE_RSA_4096      0x01       
#define AUTH_HDR_TELLURIDE_ECDSA_P384    0x02       
#define AUTH_HDR_TELLURIDE_ECDSA_P521    0x04       
#define AUTH_HDR_TELLURIDE_HSS_LMS       0x08       
#define AUTH_HDR_TELLURIDE_LMS           0x10       
#define AUTH_HDR_TELLURIDE_ML_DSA87      0x20       
#define AUTH_HDR_TELLURIDE_SLH_DSA       0x40   


// Hybrid bitmask macros for public key algorithms
#define HYBRID_RSA4096              0x00000001  // Bit 0: RSA4096
#define HYBRID_ECDSA_P384           0x00000002  // Bit 1: ECDSAp384
#define HYBRID_ECDSA_P521           0x00000004  // Bit 2: ECDSAp521
#define HYBRID_LMS_HSS              0x00000008  // Bit 3: LMS with HSS
#define HYBRID_LMS                  0x00000010  // Bit 4: LMS
#define HYBRID_ML_DSA87             0x00000020  // Bit 5: ML-DSA-87
#define HYBRID_SLH_DSA_SHAKE_256S   0x00000040  // Bit 6: SLH-DSA-SHAKE-256s

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

#define ECDSA_AC_PPK_KEY_OFFSET         0x20    /* acPpk (x||y, 96 bytes)                       */
#define ECDSA_AC_SPK_HDR_OFFSET         0x80    /* spkheader start; signed data = spkheader+spk */
#define ECDSA_AC_SPK_KEY_OFFSET         0xA0    /* acSpk (x||y, 96 bytes)                       */
#define ECDSA_AC_SPK_SIGN_OFFSET        0x110   /* acSpkSignature (r||s, 96 bytes)              */
#define ECDSA_AC_HASHBLOCK_SIGN_OFFSET  0x180   /* acHashblockSignature (r||s, 96 bytes)        */

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
    uint32_t Algorithm;      // [0:7] Algorithm, [8:15] Reserved(Represent AH)
    uint32_t RevocationID;   // eFUSE bit for key revocation
    uint32_t Hybrid;         // Bitmask for hybrid signing algorithms
    uint32_t Authority;      // Reserved for CA/PA, set to 0
    uint32_t Permission;     // Key permission (SPK, BOOT, RMA, etc.)
    uint32_t Reserved[3];    // Reserved for future (12 bytes, set to 0)
}HeaderAuthCert_versal_2vp;


typedef struct
{
    HeaderAuthCert_versal_2vp ppkheader;            // 32 bytes
    ACKey4096Sha3Padding  acPpk;                     //(0x00) = 0x1028
    uint32_t              acPpkAlignment[3];         //(0x404) = 0x1040  (acPpk is 16 bytes aligned)

    HeaderAuthCert_versal_2vp spkheader;             // 32 bytes
    ACKey4096Sha3Padding  acSpk;                     //(0x430)
    uint32_t              acSpkAlignment[3];         //(0x834)

    uint32_t              acSpkSignatureLength;      //(0x840) - SPK Signature Length
    uint32_t              acSpkSignatureLengthAlignment[3]; // 16-byte alignment padding
    ACSignature4096       acSpkSignature;            //(0x850)
    uint32_t              acHashblockSignatureLength; //(0xC50) - Hashblock Signature Length  
    uint32_t              acHashblockSignatureLengthAlignment[3]; // 16-byte alignment padding
    ACSignature4096       acHashblockSignature;      //(0xC60) hashblock sig size including in AC
                                                     
} AuthCertificate4096Sha3PaddingHBStructure_versal2vp;      //(0x1060)


typedef struct
{
    HeaderAuthCert_versal_2vp ppkheader;
    ACKeyECDSA          acPpk;                    //(0x00)  : 96bytes : no alignment needed
                                                  //uint32_t            acSpkPrivate;             //(0x74)
    HeaderAuthCert_versal_2vp spkheader;                                                  
    ACKeyECDSA          acSpk;                    //(0x80)  : 96 bytes
    uint32_t            acSpkSignatureLength;     //(0xE0)  : SPK Signature Length
    uint32_t            acSpkSignatureLengthAlignment[3]; // 16-byte alignment padding
    ACSignatureECDSA    acSpkSignature;           //(0xF0)  : 96 bytes
    uint32_t            acHashblockSignatureLength; //(0x150) : Hashblock Signature Length
    uint32_t            acHashblockSignatureLengthAlignment[3]; // 16-byte alignment padding  
    ACSignatureECDSA    acHashblockSignature;     //(0x160) : hashblock sig size including in AC
                                                
} AuthCertificateECDSAHBStructure_versal2vp;              //(0x1C0)

typedef struct
{
    HeaderAuthCert_versal_2vp ppkheader;
    ACKeyECDSAP521      acPpk;                   //(0x00) : 132 bytes
    uint32_t            acPpkAlignment[3];       //(0x84)

    HeaderAuthCert_versal_2vp spkheader;
    ACKeyECDSAP521      acSpk;                   //(0xA0) : 132 bytes
    uint32_t            acSpkSignatureLength;    //(0x124): SPK Signature Length
    uint32_t            acSpkSignatureLengthAlignment[3]; // 16-byte alignment padding
    ACSignatureECDSAP521 acSpkSignature;         //(0x134): 132 bytes
    uint32_t            acSpkAlignment[2];       //(0x1B8)                                            
    uint32_t            acHashblockSignatureLength; //(0x1C0) : Hashblock Signature Length
    uint32_t            acHashblockSignatureLengthAlignment[3]; // 16-byte alignment padding
    ACSignatureECDSAP521 acHashblockSignature;   //(0x1D0) : 132 Bytes
} AuthCertificateECDSAp521HBStructure_versal2vp;            //(0x284)       

typedef struct 
{
    HeaderAuthCert_versal_2vp ppkheader;
    uint8_t acPpk[MLDSA_PUB_KEY_LENGTH];
    HeaderAuthCert_versal_2vp spkheader;
    uint8_t acSpk[MLDSA_PUB_KEY_LENGTH];
    uint32_t acSpkSignatureLength;              // SPK Signature Length
    uint32_t acSpkSignatureLengthAlignment[3];  // 16-byte alignment padding
    uint8_t acSpkSignature[MLDSA_TOTAL_SIGN_LEN];
    uint32_t acHashblockSignatureLength;        // Hashblock Signature Length
    uint32_t acHashblockSignatureLengthAlignment[3]; // 16-byte alignment padding
    uint8_t acHashblockSignature[MLDSA_TOTAL_SIGN_LEN];
} AuthCertificateMLDSAStructure_versal2vp;

typedef struct 
{
    HeaderAuthCert_versal_2vp ppkheader;
    uint8_t acPpk[SLHDSA_PUB_KEY_LENGTH];
    HeaderAuthCert_versal_2vp spkheader;
    uint8_t acSpk[SLHDSA_PUB_KEY_LENGTH];
    uint32_t acSpkSignatureLength;              // SPK Signature Length  
    uint32_t acSpkSignatureLengthAlignment[3];  // 16-byte alignment padding
    uint8_t acSpkSignature[SLHDSA_TOTAL_SIGN_LENGTH];
    uint32_t acHashblockSignatureLength;        // Hashblock Signature Length
    uint32_t acHashblockSignatureLengthAlignment[3]; // 16-byte alignment padding   
    uint8_t acHashblockSignature[SLHDSA_TOTAL_SIGN_LENGTH];
} AuthCertificateSLHDSAStructure_versal2vp;

/*
 * versal_2vp native PPK-only Authenticated-JTAG secure-debug message structures.
 * Layout: message header + versal_2vp HeaderAuthCert (ppkheader) + PPK + PSK signature.
 * The algorithm id and revocation id are carried in the ppkheader (Algorithm /
 * RevocationID); key/signature sizes are implied by the algorithm, so no image
 * length, ac-header or message-level size/revoke fields are stored. These offsets
 * are versal_2vp-specific; the shared AUTH_JTAG_LMS_* offsets in common/authkeys.h
 * describe the different Telluride layout.
 */
#define AUTH_JTAG_PPK_PERMISSION              0x1    /* HeaderAuthCert.Permission = BOOT */
#define AUTH_JTAG_V2VP_ID_WORD_OFFSET         0x00
#define AUTH_JTAG_V2VP_ATTRIBUTES_OFFSET      0x04
#define AUTH_JTAG_V2VP_DEVICE_DNA_OFFSET      0x08
#define AUTH_JTAG_V2VP_JTAG_TIMEOUT_OFFSET    0x18
#define AUTH_JTAG_V2VP_FIXED_HEADER_SIZE      0x20   /* 32-byte header; ppkheader starts here */

typedef struct
{
    uint32_t                  idWord;                 //0x00
    uint32_t                  attributes;             //0x04
    uint8_t                   deviceDNA[16];          //0x08
    uint32_t                  jtagTimeOut;            //0x18
    uint32_t                  alignment;              //0x1C
    HeaderAuthCert_versal_2vp ppkheader;              //0x20 (32B)
    uint8_t                   acPpk[RSA_4096_N_SIZE + RSA_4096_N_EXT_SIZE + RSA_4096_E_SIZE]; //0x40 (1028)
    uint32_t                  ppkAlignment[3];        //0x444
    uint8_t                   authJtagSignature[SIGN_LENGTH_VERSAL]; //0x450 (512)
} AuthJtagRSAImageStructure_versal2vp;   //0x650

typedef struct
{
    uint32_t                  idWord;                 //0x00
    uint32_t                  attributes;             //0x04
    uint8_t                   deviceDNA[16];          //0x08
    uint32_t                  jtagTimeOut;            //0x18
    uint32_t                  alignment;              //0x1C
    HeaderAuthCert_versal_2vp ppkheader;              //0x20 (32B)
    uint8_t                   acPpk[2 * EC_P384_KEY_LENGTH];         //0x40 (96)
    uint8_t                   authJtagSignature[2 * EC_P384_KEY_LENGTH]; //0xA0 (96)
} AuthJtagECP384ImageStructure_versal2vp; //0x100

typedef struct
{
    uint32_t                  idWord;                 //0x00
    uint32_t                  attributes;             //0x04
    uint8_t                   deviceDNA[16];          //0x08
    uint32_t                  jtagTimeOut;            //0x18
    uint32_t                  alignment;              //0x1C
    HeaderAuthCert_versal_2vp ppkheader;              //0x20 (32B)
    uint8_t                   acPpk[2 * EC_P521_KEY_LENGTH2];         //0x40 (132)
    uint8_t                   authJtagSignature[2 * EC_P521_KEY_LENGTH2]; //0xC4 (132)
} AuthJtagECP521ImageStructure_versal2vp; //0x148

typedef struct
{
    uint32_t                  idWord;                 //0x00
    uint32_t                  attributes;             //0x04
    uint8_t                   deviceDNA[16];          //0x08
    uint32_t                  jtagTimeOut;            //0x18
    uint32_t                  alignment;              //0x1C
    HeaderAuthCert_versal_2vp ppkheader;              //0x20 (32B)
    uint8_t                   acPpk[MLDSA_PUB_KEY_LENGTH];           //0x40 (2592)
    uint8_t                   authJtagSignature[MLDSA_TOTAL_SIGN_LEN]; //0xA60
} AuthJtagMLDSAImageStructure_versal2vp;

typedef struct
{
    uint32_t                  idWord;                 //0x00
    uint32_t                  attributes;             //0x04
    uint8_t                   deviceDNA[16];          //0x08
    uint32_t                  jtagTimeOut;            //0x18
    uint32_t                  alignment;              //0x1C
    HeaderAuthCert_versal_2vp ppkheader;              //0x20 (32B)
    uint8_t                   acPpk[SLHDSA_PUB_KEY_LENGTH];          //0x40 (64)
    uint8_t                   authJtagSignature[SLHDSA_TOTAL_SIGN_LENGTH]; //0x80
} AuthJtagSLHDSAImageStructure_versal2vp;

/******************************************************************************/
class ECDSAHBAuthenticationAlgorithm_versal_2vp : public AuthenticationAlgorithm
{
public:
    ECDSAHBAuthenticationAlgorithm_versal_2vp();
    ~ECDSAHBAuthenticationAlgorithm_versal_2vp();

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

protected:
    uint32_t certSize;
};

/******************************************************************************/
class ECDSAP521HBAuthenticationAlgorithm_versal_2vp : public AuthenticationAlgorithm
{
public:
    ECDSAP521HBAuthenticationAlgorithm_versal_2vp();
    ~ECDSAP521HBAuthenticationAlgorithm_versal_2vp();

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

protected:
    uint32_t certSize;
};

/******************************************************************************/
class RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp : public AuthenticationAlgorithm
{
public:
    RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp();

    virtual ~RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp();

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

protected:
    uint32_t certSize;
};

/******************************************************************************/
class LMSAuthenticationAlgorithm_versal_2vp : public AuthenticationAlgorithm
{
public:
    LMSAuthenticationAlgorithm_versal_2vp(Authentication::Type type) 
    { 
        authType = type;  
    }

    virtual ~LMSAuthenticationAlgorithm_versal_2vp() {}

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
    uint32_t GetAuthHeader(bool lmsOnly, bool IsLassenSeries, bool IsDl9Series);

protected:
    uint32_t certSize;
};

/******************************************************************************/
class MLDSAAuthenticationAlgorithm_versal_2vp : public AuthenticationAlgorithm
{
public:
    MLDSAAuthenticationAlgorithm_versal_2vp(Authentication::Type type) 
    { 
        authType = type;  
        certSize = sizeof(AuthCertificateMLDSAStructure_versal2vp);
    }

    virtual ~MLDSAAuthenticationAlgorithm_versal_2vp() {}

    Authentication::Type Type()
    {
        return authType;
    }

    // Fixed-length versions (for HashBlock0/SPK signature with known fixed sizes)
    void CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0, bool isSign);
    void VerifySignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* sign, bool isSign);
    
    // Variable-length versions (for HashBlock1 which has variable size based on partition count)
    void CreateSignature(const uint8_t* base, size_t msg_len, uint8_t* primaryKey, uint8_t* result0);
    void VerifySignature(const uint8_t* base, size_t msg_len, uint8_t* primaryKey, uint8_t* sign);

    void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength);
    uint32_t getCertificateSize(void) { return certSize; }
    Authentication::Type authType;
    void RearrangeEndianess(uint8_t* array, uint32_t size) { };
    uint32_t GetAuthHeader();

protected:
    uint32_t certSize;
};

/******************************************************************************/
class SLHAuthenticationAlgorithm_versal_2vp : public AuthenticationAlgorithm 
{
public:
    //SLHAuthenticationAlgorithm_versal_2vp();
    virtual ~SLHAuthenticationAlgorithm_versal_2vp() {}

    SLHAuthenticationAlgorithm_versal_2vp(Authentication::Type type) 
    { 
        authType = type;  
    }
    Authentication::Type Type()
    {
        return Authentication::SLH_SHAKE256;
    }

    void CreateSignature(const uint8_t* buffer, size_t buf_len, const char* keyFile, 
                        uint8_t* result0, size_t& result0_len);
    void VerifySignature(const uint8_t* msg, size_t msg_len, const uint8_t* sig, const char* keyFile);

    void CreatePadding(uint8_t* signature, uint8_t* hash, uint8_t hashLength);
    uint32_t getCertificateSize(void) { return certSize; }
    void RearrangeEndianess(uint8_t* array, uint32_t size) 
    {
    }
    uint32_t GetAuthHeader(void);

protected:
    Authentication::Type authType;
    uint32_t certSize;
};

/******************************************************************************/
class Versal_2vpAuthenticationContext : public AuthenticationContext
{
public:
    Versal_2vpAuthenticationContext(Authentication::Type type);
    Versal_2vpAuthenticationContext(const AuthenticationContext* refAuthContext, Authentication::Type authtype);
    Versal_2vpAuthenticationContext(const AuthCertificate4096Sha3PaddingHBStructure_versal2vp* existingCert, Authentication::Type authtype);
    ~Versal_2vpAuthenticationContext();

    void Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert);

    uint32_t getCertificateSize(void) { return certSize; }
    void AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header);
    Section* CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection, bool isBootloader);
    PartitionBifOptions* FindOwningPartitionBifOptions(BootImage& bi, Section* dataSection);
    void GenerateIHTHash(BootImage& bi, uint8_t* sha_hash_padded);
    void GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded);
    void GenerateSPKHash(uint8_t * sha_hash_padded);
    void GeneratePPKHash(const std::string& filename);
    void GeneratePPKHashWithMetadata(const std::string& filename, const KeyMetadata& metadata, const std::string& keyFile = "");
    void CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection);
    static void GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index);
    static void GetPresign(const std::string& presignFilename, uint16_t signatureLength, uint8_t* signature, uint32_t index);
    void SetSPKSignatureFile(const std::string& filename);
    void SetBHSignatureFile(const std::string& filename);
    void GenerateSPKSignature(const std::string& filename);
    void ResizeIfNecessary(Section* section);
    void LoadUdfData(const std::string& filename, uint8_t* signature);
    void CreateSPKSignature(BootImage& bi);
    void CreateSPKSignature(BootImage& bi, uint32_t hybridMask);
    void CopySPKSignature(BootImage& bi, uint8_t* ptr);
    void CreateAuthJtagImage(Options& options, uint8_t * buffer, const AuthJtagInfo& authJtagAttributes);
    /* Shared implementation for the fixed-layout ECDSA auth-JTAG variants (P-384/P-521).
       Templated on the image struct type; keyBytes is the Qx||Qy (and r||s) length = 2*curve size. */
    template <typename ImageStruct>
    void CreateEcdsaAuthJtagImage(Options& options, uint8_t* buffer, const AuthJtagInfo& authJtagAttributes, uint32_t keyBytes);
    virtual uint32_t GetAuthJtagSpkEnableMask(void) { return 0; }
    uint32_t GetAuthJtagImageSize(void) const;
    void SetKeyLength(Authentication::Type type);
    std::unique_ptr<AuthenticationAlgorithm> GetAuthenticationAlgorithm(Authentication::Type type);
    uint32_t GetCertificateSize();
    uint32_t GetTotalHashBlockSignSize(void);
    static bool IsSectionEncrypted(BootImage& bi, Section* section);
    void SetCurrentDataSection(Section* dataSection) { currentDataSection = dataSection; }
    Section* GetCurrentDataSection() { return currentDataSection; }
    
    // Public flag to bypass hybrid detection for individual algorithm contexts
    bool bypassHybridDetection;
    
protected:
    uint32_t certSize;
    void CopybHSignature(BootImage& bi, uint8_t* ptr);
    void CopyIHTSignature(BootImage& bi, uint8_t* ptr);
    std::string GetCertificateName(std::string name);
    bool spkSignatureCreated;
    Section* currentDataSection;
};

/******************************************************************************/
class Versal_2vpAuthenticationCertificate : public AuthenticationCertificate
{
public:
    Versal_2vpAuthenticationCertificate(AuthenticationContext* context) : AuthenticationCertificate(context) {}
    Section* AttachBootHeaderToFsbl(BootImage& bi) { return NULL; }
    void Link(BootImage& bi, Section* section);
    //AuthCertificate4096Sha3PaddingStructure *acStructure;
};
#endif
