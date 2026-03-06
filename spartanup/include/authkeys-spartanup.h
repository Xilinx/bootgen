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

#ifndef _AUTH_KEYS_SPARTANUP_H_
#define _AUTH_KEYS_SPARTANUP_H_

/*
-------------------------------------------------------------------------------
************************************************************ HEADER FILES *****
-------------------------------------------------------------------------------
*/
#include <memory>
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
class SpartanupKey : public Key
{
public:
    SpartanupKey(const std::string& name0);
    SpartanupKey(const Key& otherKey);
    ~SpartanupKey();

    void Parse(const std::string& filename, bool isSecret0);
    uint8_t ParseECDSAOpenSSLKey(const std::string& filename);

    EC_KEY *eckey;
    std::unique_ptr<uint8_t[]> x;    // x co-ordinate (384 bits)
    std::unique_ptr<uint8_t[]> y;    // y co-ordinate (384 bits)

    // PM-TODO Create a separate class
    std::unique_ptr<HssPublicKey> public_key;
    std::unique_ptr<HssPrivateKey> private_key;
    void ParseLmsKey(const std::string& filename); 
};

/******************************************************************************/
class Key4096Sha3Padding_spartanup : public SpartanupKey
{
public:
    Key4096Sha3Padding_spartanup(const std::string& name0) : SpartanupKey(name0) { }
    // Copy constructor deleted because unique_ptr members cannot be copied
    Key4096Sha3Padding_spartanup(const Key4096Sha3Padding_spartanup& otherKey) = delete;
    Key4096Sha3Padding_spartanup& operator=(const Key4096Sha3Padding_spartanup& otherKey) = delete;

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};


/******************************************************************************/
class KeyECDSA_spartanup : public SpartanupKey
{
public:
    KeyECDSA_spartanup(const std::string& name0) : SpartanupKey(name0) { }
    // Copy constructor deleted because unique_ptr members cannot be copied
    KeyECDSA_spartanup(const KeyECDSA_spartanup& otherKey) = delete;
    KeyECDSA_spartanup& operator=(const KeyECDSA_spartanup& otherKey) = delete;

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

/******************************************************************************/
class KeyECDSAp521_spartanup : public SpartanupKey
{
public:
    KeyECDSAp521_spartanup(const std::string& name0) : SpartanupKey(name0) { }
    // Copy constructor deleted because unique_ptr members cannot be copied
    KeyECDSAp521_spartanup(const KeyECDSAp521_spartanup& otherKey) = delete;
    KeyECDSAp521_spartanup& operator=(const KeyECDSAp521_spartanup& otherKey) = delete;

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

/******************************************************************************/
class KeyLMS_spartanup : public SpartanupKey
{
public:
    KeyLMS_spartanup(const std::string& name0) : SpartanupKey(name0) { }
    // Copy constructor deleted because unique_ptr members cannot be copied
    KeyLMS_spartanup(const KeyLMS_spartanup& otherKey) = delete;
    KeyLMS_spartanup& operator=(const KeyLMS_spartanup& otherKey) = delete;

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

#endif
