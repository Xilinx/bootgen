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
#ifndef _AUTHKEYS_H_
#define _AUTHKEYS_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <string>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include "logger.h"
#include <vector>
//#include "bifoptions.h"
#if OPENSSL_VERSION_NUMBER > 0x10100000L
#include "xil-bignum.h"
#endif

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
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

typedef struct
{
    HashFunction::Type hashFunction;
    uint8_t w;
    uint8_t h;
} LmsKeyParam;

typedef struct
{
    AuthKeyLevel::Type type;
    std::string lms_param;
} LmsKeyTypeParam;

typedef struct
{
    uint8_t     N[RSA_4096_KEY_LENGTH];               // Modulus
    uint8_t     N_extension[RSA_4096_KEY_LENGTH];     // Montgomery Modulus extension
    uint8_t     E[4];                                 // Exponent
} ACKey4096Sha3Padding;

typedef struct
{
    uint8_t     x[EC_P384_KEY_LENGTH];          // x co-ordinate
    uint8_t     y[EC_P384_KEY_LENGTH];          // y co-ordinate
//    uint8_t     pad[932];                       // Padding
} ACKeyECDSA;

typedef struct
{
    uint8_t     x[EC_P521_KEY_LENGTH2];          // x co-ordinate
    uint8_t     y[EC_P521_KEY_LENGTH2];          // y co-ordinate
    // uint8_t     pad[896];                       // Padding
} ACKeyECDSAP521;

typedef struct
{
    uint8_t     Signature[2 * EC_P384_KEY_LENGTH];
} ACSignatureECDSA;

typedef struct
{
    uint8_t     Signature[2 * EC_P521_KEY_LENGTH2];
} ACSignatureECDSAP521;

typedef struct
{
    uint8_t N[RSA_2048_KEY_LENGTH];             // Modulus
    uint8_t N_extension[RSA_2048_KEY_LENGTH];   // Montgomery Modulus extension
    uint8_t E[4];                               // Exponent
    uint8_t Padding[60];                        // Padding
} ACKey2048;

typedef struct
{
    uint8_t Signature[RSA_SIGN_LENGTH_ZYNQ];
} ACSignature2048;

typedef struct
{
    uint8_t N[RSA_4096_KEY_LENGTH];           // Modulus
    uint8_t N_extension[RSA_4096_KEY_LENGTH]; // Montgomery Modulus extension
    uint8_t E[4];                               // Exponent
    uint8_t Padding[60];                        // Padding
} ACKey4096;

/* Signature - RSA 4096 bits */
typedef struct
{
    uint8_t Signature[RSA_SIGN_LENGTH_ZYNQMP];
} ACSignature4096;

/* Key Generation Structures */
typedef struct
{
    GenAuthKeys::Type format;
    std::string ppk_file;
    std::string psk_file;
    std::string spk_file;
    std::string ssk_file;
    uint16_t keyLength;
} KeyGenerationStruct;


/* Versal2ve2vm & Spartanup Defines*/
typedef struct
{
    ACKey4096Sha3Padding  acPpk;                     //(0x00) = 1028
    uint32_t              acPpkAlignment[3];         //(0x404)

    uint32_t              acTotalSpkSize;            //(0x410) = 1028
    uint32_t              acActualSpkSize;           //(0x414) = 1040
    uint32_t              acSpkTotalSignatureSize;   //(0x418) = 512
    uint32_t              acSpkActualSignatureSize;  //(0x41C) = 512
    uint32_t              acSpkId;                   //(0x420)
                                                     //uint32_t              acSpkPrivate;              //(0x424)
    uint32_t              acSpkHdrAlignment[3];      //(0x424)

    ACKey4096Sha3Padding  acSpk;                     //(0x430)
    uint32_t              acSpkAlignment[3];         //(0x834)

    ACSignature4096       acSpkSignature;            //(0x840)

                                                     //ACSignature4096       acHashBlockSignature;      //(0xA40)
} AuthCertificate4096Sha3PaddingHBStructure;      //(0xC40)

typedef struct
{
    ACKeyECDSA          acPpk;                    //(0x00)  : 96bytes : no alignment needed

    uint32_t            acTotalSpkSize;           //(0x60)  = 96      
    uint32_t            acActualSpkSize;          //(0x64)  = 96
    uint32_t            acSpkTotalSignatureSize;  //(0x68)  = 96
    uint32_t            acSpkActualSignatureSize; //(0x6C)  = 96
    uint32_t            acSpkId;                  //(0x70)
                                                  //uint32_t            acSpkPrivate;             //(0x74)
    uint32_t            acSpkHdrAlignment[3];     //(0x74)

    ACKeyECDSA          acSpk;                    //(0x80)  : 96 bytes
    ACSignatureECDSA    acSpkSignature;           //(0xE0)  : 96 bytes

                                                  //ACSignatureECDSA    acHashBlockSignature;     //(0x140) : 96 bytes
} AuthCertificateECDSAHBStructure;              //(0x1A0)

typedef struct
{ // struct only specific for telluride and lassen 
    uint32_t            acTotalSpkSize;
    uint32_t            acActualSpkSize;
    uint32_t            acSpkTotalSignatureSize;
    uint32_t            acSpkActualSignatureSize;
    uint32_t            acSpkId;
    //uint32_t            acSpkPrivate;           
    uint32_t            acSpkHdrAlignment[3];
} spkheaderstructure;

typedef struct
{
    ACKeyECDSAP521      acPpk;                   //(0x00) : 132 bytes
    uint32_t            acPpkAlignment[3];       //(0x84)

    uint32_t            acTotalSpkSize;           //(0x90)
    uint32_t            acActualSpkSize;          //(0x94)
    uint32_t            acSpkTotalSignatureSize;  //(0x98)
    uint32_t            acSpkActualSignatureSize; //(0x9C)
    uint32_t            acSpkId;                  //(0xA0)
    uint32_t            acSpkHdrAlignment[3];     //(0xA4)

    ACKeyECDSAP521      acSpk;                   //(0xB0) : 132 bytes
    uint32_t            acSpkAlignment[3];       //(0x134)
    ACSignatureECDSAP521 acSpkSignature;         //(0x140): 132 bytes
    uint32_t            acSpkSignatureAlignment[3]; //(0x1C4
} AuthCertificateECDSAp521HBStructure;            //(0x240)       


#define AUTH_JTAG_IMAGE_IDENTIFICATION_WORD 0x58414A47 //XAJG
                                                  /* Authenticated-Jtag Image Structure */
typedef struct
{
    uint32_t      idWord;                             //0x00         
    uint32_t      authJtagImageLength;                //0x04
    uint32_t      acHeader;                           //0x08   
    uint32_t      totalppkSize;                       //0x0C
    uint32_t      actualppkSize;                      //0x10
    uint32_t      totalAuthJtagSignSize;              //0x14
    uint32_t      actualAuthJtagSignSize;             //0x18
    uint32_t      authJtagMsgrevokeId;                //0x1C
    uint32_t      attributes;                         //0x20
    uint8_t       deviceDNA[16];                      //0x24
    uint32_t      jtagTimeOut;                        //0x34
    uint32_t      alignment[2];                       //0x38

    uint8_t       acPpk[RSA_4096_N_SIZE + RSA_4096_N_EXT_SIZE + RSA_4096_E_SIZE];    //0x40 //1028 Bytes
    uint32_t      ppkAlignment[3];                   //0x444

    uint32_t      totalspkSize;                       //0x450 
    uint32_t      actualspkSize;                      //0x454
    uint32_t      totalspkSignSize;                   //0x458
    uint32_t      actualspkSignSize;                  //0x45C
    uint32_t      spkRevokeId;                        //0x460
    uint32_t      acSpkHdrAlignment[3];               //0x464

    uint8_t       acSpk[RSA_4096_N_SIZE + RSA_4096_N_EXT_SIZE + RSA_4096_E_SIZE];    //0x470 //1028 Bytes
    uint32_t      spkAlignment[3];                   //0x874

    uint8_t       spkSignature[SIGN_LENGTH_VERSAL];   //0x880

    uint8_t       authJtagSignature[SIGN_LENGTH_VERSAL]; //0xA80
} AuthenticatedJtagRSAImageStructure; //0xC80

typedef struct
{
    uint32_t      idWord;                             //0x00
    uint32_t      authJtagImageLength;                //0x04
    uint32_t      acHeader;                           //0x08
    uint32_t      totalppkSize;                       //0x0C
    uint32_t      actualppkSize;                      //0x10
    uint32_t      totalAuthJtagSignSize;              //0x14
    uint32_t      actualAuthJtagSignSize;             //0x18
    uint32_t      authJtagMsgrevokeId;                //0x1C
    uint32_t      attributes;                         //0x20
    uint8_t       deviceDNA[16];                      //0x24
    uint32_t      jtagTimeOut;                        //0x34
    uint32_t      alignment[2];                       //0x38

    uint8_t       acPpk[2 * EC_P384_KEY_LENGTH];      //0x40

    uint32_t      totalspkSize;                       //0xA0
    uint32_t      actualspkSize;                      //0xA4
    uint32_t      totalspkSignSize;                   //0xA8
    uint32_t      actualspkSignSize;                  //0xAC
    uint32_t      spkRevokeId;                        //0xB0
    uint32_t      acSpkHdrAlignment[3];               //0xB4

    uint8_t       acSpk[2 * EC_P384_KEY_LENGTH];      //0xC0

    uint8_t       spkSignature[2 * EC_P384_KEY_LENGTH];   //0x120

    uint8_t       authJtagSignature[2 * EC_P384_KEY_LENGTH]; //0x180
} AuthenticatedJtagECP384ImageStructure; //0x1E0

/* LMS/HSS Authenticated-Jtag Image - layout is variable; PPK/SPK/sign offsets computed at runtime from key/sign sizes */
#define AUTH_JTAG_LMS_PUBLIC_KEY_SIZE       60   /* sizeof(HssPublicKey), max PPK/SPK size */
#define AUTH_JTAG_LMS_PPK_REGION_SIZE       64   /* AUTH_JTAG_LMS_PUBLIC_KEY_SIZE padded to 16B */
#define AUTH_JTAG_LMS_SPK_REGION_SIZE       64   /* same for SPK */
#define AUTH_JTAG_LMS_SPK_HEADER_LENGTH     32   /* TELLURIDE_AC_SPK_HDR_LENGTH */
#define AUTH_JTAG_LMS_MAX_SIGNATURE_SIZE    8192 /* max SPK and auth JTAG signature length supported */
/* Max image size for allocation: fixed header 0x40 + PPK region + SPK header + SPK region + max SPK sign + max auth sign */
#define AUTH_JTAG_LMS_IMAGE_MAX_SIZE        (AUTH_JTAG_LMS_FIXED_HEADER_SIZE + AUTH_JTAG_LMS_PPK_REGION_SIZE + \
                                             AUTH_JTAG_LMS_SPK_HEADER_LENGTH + AUTH_JTAG_LMS_SPK_REGION_SIZE + \
                                             AUTH_JTAG_LMS_MAX_SIGNATURE_SIZE + AUTH_JTAG_LMS_MAX_SIGNATURE_SIZE)

/* Auth JTAG LMS fixed header only (offsets 0x00-0x3F); PPK/SPK/SPK sign/auth sign offsets computed in CreateAuthJtagImage */
#define AUTH_JTAG_LMS_FIXED_HEADER_SIZE     0x40
#define AUTH_JTAG_LMS_ID_WORD_OFFSET                0x00
#define AUTH_JTAG_LMS_IMAGE_LENGTH_OFFSET            0x04
#define AUTH_JTAG_LMS_AC_HEADER_OFFSET               0x08
#define AUTH_JTAG_LMS_TOTAL_PPK_SIZE_OFFSET          0x0C
#define AUTH_JTAG_LMS_ACTUAL_PPK_SIZE_OFFSET         0x10
#define AUTH_JTAG_LMS_TOTAL_AUTH_SIGN_SIZE_OFFSET    0x14
#define AUTH_JTAG_LMS_ACTUAL_AUTH_SIGN_SIZE_OFFSET   0x18
#define AUTH_JTAG_LMS_MSG_REVOKE_ID_OFFSET           0x1C
#define AUTH_JTAG_LMS_ATTRIBUTES_OFFSET              0x20
#define AUTH_JTAG_LMS_DEVICE_DNA_OFFSET              0x24
#define AUTH_JTAG_LMS_JTAG_TIMEOUT_OFFSET            0x34
#define AUTH_JTAG_LMS_PPK_OFFSET                     0x40  /* PPK starts after fixed header */

typedef enum
{
    authJtagMessageShift = 0,
    authJtagMessagenMask = 0x3,
} AuthJtagAttributes;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
class Key 
{
public:
    Key(const std::string& name0);
    Key(const Key& otherKey);
    virtual ~Key();

    virtual void Export(void* dst) = 0;
    virtual void Import(const void* acKey, const std::string& name0) = 0;

    void ParsePublic(const std::string& filename);
    void ParseSecret(const std::string& filename);
    void SetKeyName(std::string);
    static void GenerateRsaKeys(KeyGenerationStruct*);
    static void GenerateEcdsaKeys(KeyGenerationStruct*);
    static void GenerateLmsKeys(KeyGenerationStruct* keygen, std::vector<LmsKeyParam>, std::vector<LmsKeyParam>);

    bool Loaded;
    bool isSecret;
    std::unique_ptr<uint8_t[]> N;         // modulus (2048 bits)
    std::unique_ptr<uint8_t[]> E;         // public (encryption) exponent (32 bits)
    std::unique_ptr<uint8_t[]> N_ext;     // modular_ext (2048 bits)
    std::unique_ptr<uint8_t[]> D;         // secret (decryption) exponent (2048 bits).
    Authentication :: Type authType;
    bool lmsOnly;
    /* Compute R^2 mod N using public OpenSSL API for MSVC compatibility */
    static void ComputeModulusExtension(const uint8_t* modulus, uint8_t* extension, size_t keyLen);
protected:
    void Multiply_p_q(uint8_t p[], uint8_t q[], uint8_t n[]);
    void Hex2Byte(FILE* f, uint8_t* data, int count);
    virtual void Parse(const std::string& filename, bool isSecret0);
    uint8_t ParseAMDRsaKey(FILE* f);
    uint8_t ParseOpenSSLKey(FILE* f);
    static void WriteRsaFile(std::string file, const RSA* rsa, bool secret, uint16_t keyLength);
    static void WritePemFile(std::string file, RSA* rsa, EC_KEY* eckey, bool secret);

    std::unique_ptr<uint8_t[]> P;
    std::unique_ptr<uint8_t[]> Q;
    std::string name;
    uint16_t keySize;
    uint16_t keySizeX;
    uint16_t keySizeY;
};

/******************************************************************************/
class Key2048 : public Key  
{
public:
    Key2048(const std::string& name0) : Key(name0) { }
    Key2048(const Key2048& otherKey) : Key(otherKey) { }

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

/******************************************************************************/
class Key4096 : public Key  
{
public:
    Key4096(const std::string& name0) : Key(name0) { }
    Key4096(const Key4096& otherKey) : Key(otherKey) { }

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

#endif
