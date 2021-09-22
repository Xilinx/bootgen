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


/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
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

    bool Loaded;
    bool isSecret;
    uint8_t *N;         // modulus (2048 bits)
    uint8_t *E;         // public (encryption) exponent (32 bits)
    uint8_t *N_ext;     // modular_ext (2048 bits)
    uint8_t *D;         // secret (decryption) exponent (2048 bits).
    Authentication :: Type authType;
protected:
    void Multiply_p_q(uint8_t p[], uint8_t q[], uint8_t n[]);
    void Hex2Byte(FILE* f, uint8_t* data, int count);
    virtual void Parse(const std::string& filename, bool isSecret0);
    uint8_t ParseXilinxRsaKey(FILE* f);
    uint8_t ParseOpenSSLKey(FILE* f);
    static void WriteRsaFile(std::string file, const RSA* rsa, bool secret, uint16_t keyLength);
    static void WritePemFile(std::string file, RSA* rsa, EC_KEY* eckey, bool secret);

    uint8_t *P;
    uint8_t *Q;
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
