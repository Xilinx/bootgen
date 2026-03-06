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

#ifndef _AUTH_KEYS_VERSAL_2VE_2VM_H_
#define _AUTH_KEYS_VERSAL_2VE_2VM_H_

/*
-------------------------------------------------------------------------------
************************************************************ HEADER FILES *****
-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <string>
#include "logger.h"
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ec.h>
#include "authkeys.h"
#include "lms.h"



/*
-------------------------------------------------------------------------------
*********************************************************** PREPROCESSORS *****
-------------------------------------------------------------------------------
*/

//AC key related defines
#define RSA_4096_N          0x0
#define RSA_4096_N_EXT      0x200
#define RSA_4096_E          0x400

#define VERSAL_ACKEY_STRUCT_SIZE    1028

#define RSA_4096_N_SIZE             512
#define RSA_4096_N_EXT_SIZE         512
#define RSA_4096_E_SIZE             4
/*
-------------------------------------------------------------------------------
************************************************************** STRUCTURES *****
-------------------------------------------------------------------------------
*/
/*
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
    //uint8_t     pad[932];                       // Padding
} ACKeyECDSA;

typedef struct
{
    uint8_t     x[EC_P521_KEY_LENGTH2];          // x co-ordinate
    uint8_t     y[EC_P521_KEY_LENGTH2];          // y co-ordinate
    //uint8_t     pad[896];                       // Padding
} ACKeyECDSAP521;

typedef struct
{
    uint8_t     Signature[2 * EC_P384_KEY_LENGTH];
} ACSignatureECDSA;

typedef struct
{
    uint8_t     Signature[2 * EC_P521_KEY_LENGTH2];
} ACSignatureECDSAP521;
*/

/*
typedef struct
{
    uint8_t Signature[SIGN_LENGTH_VERSAL];
} ACSignatureECDSA;
*/
//typedef HssSignature ACSignatureLMS;

/*
-------------------------------------------------------------------------------
***************************************************************** CLASSES *****
-------------------------------------------------------------------------------
*/
class Versal_2ve_2vmKey : public Key
{
public:
    Versal_2ve_2vmKey(const std::string& name0);
    Versal_2ve_2vmKey(const Key& otherKey);
    ~Versal_2ve_2vmKey();

    void Parse(const std::string& filename, bool isSecret0);
    uint8_t ParseECDSAOpenSSLKey(const std::string& filename);

    EC_KEY *eckey;  // OpenSSL managed pointer, don't wrap in unique_ptr
    std::unique_ptr<uint8_t[]> x;    // x co-ordinate (384 bits)
    std::unique_ptr<uint8_t[]> y;    // y co-ordinate (384 bits)

    // PM-TODO Create a separate class
    std::unique_ptr<HssPublicKey> public_key;
    std::unique_ptr<HssPrivateKey> private_key;
    void ParseLmsKey(const std::string& filename); 
};

/******************************************************************************/
class Key4096Sha3Padding_versal_2ve_2vm : public Versal_2ve_2vmKey
{
public:
    Key4096Sha3Padding_versal_2ve_2vm(const std::string& name0) : Versal_2ve_2vmKey(name0) { }
    Key4096Sha3Padding_versal_2ve_2vm(const Key4096Sha3Padding_versal_2ve_2vm& otherKey) = delete;  // Disable copy due to unique_ptr

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};


/******************************************************************************/
class KeyECDSA_versal_2ve_2vm : public Versal_2ve_2vmKey
{
public:
    KeyECDSA_versal_2ve_2vm(const std::string& name0) : Versal_2ve_2vmKey(name0) { }
    KeyECDSA_versal_2ve_2vm(const KeyECDSA_versal_2ve_2vm& otherKey) = delete;  // Disable copy due to unique_ptr

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

/******************************************************************************/
class KeyECDSAp521_versal_2ve_2vm : public Versal_2ve_2vmKey
{
public:
    KeyECDSAp521_versal_2ve_2vm(const std::string& name0) : Versal_2ve_2vmKey(name0) { }
    KeyECDSAp521_versal_2ve_2vm(const KeyECDSAp521_versal_2ve_2vm& otherKey) = delete;  // Disable copy due to unique_ptr

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

/******************************************************************************/
class KeyLMS_versal_2ve_2vm : public Versal_2ve_2vmKey
{
public:
    KeyLMS_versal_2ve_2vm(const std::string& name0) : Versal_2ve_2vmKey(name0) { }
    KeyLMS_versal_2ve_2vm(const KeyLMS_versal_2ve_2vm& otherKey) = delete;  // Disable copy due to unique_ptr

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

#endif
