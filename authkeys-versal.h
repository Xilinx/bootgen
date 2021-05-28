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

#ifndef _AUTH_KEYS_VERSAL_H_
#define _AUTH_KEYS_VERSAL_H_

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
    uint8_t     pad[932];                       // Padding
} ACKeyECDSA;

typedef struct
{
    uint8_t     x[EC_P521_KEY_LENGTH2];          // x co-ordinate
    uint8_t     y[EC_P521_KEY_LENGTH2];          // y co-ordinate
    uint8_t     pad[896];                       // Padding
} ACKeyECDSAP521;

typedef struct
{
    uint8_t Signature[SIGN_LENGTH_VERSAL];
} ACSignatureECDSA;

/*
-------------------------------------------------------------------------------
***************************************************************** CLASSES *****
-------------------------------------------------------------------------------
*/
class VersalKey : public Key
{
public:
    VersalKey(const std::string& name0);
    VersalKey(const Key& otherKey);
    ~VersalKey();

    void Parse(const std::string& filename, bool isSecret0);
    uint8_t ParseECDSAOpenSSLKey(const std::string& filename);
    EC_KEY *eckey;
    uint8_t *x;    // x co-ordinate (384 bits)
    uint8_t *y;    // y co-ordinate (384 bits)
};

/******************************************************************************/
class Key4096Sha3Padding : public VersalKey
{
public:
    Key4096Sha3Padding(const std::string& name0) : VersalKey(name0) { }
    Key4096Sha3Padding(const Key4096Sha3Padding& otherKey) : VersalKey(otherKey) { }

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};


/******************************************************************************/
class KeyECDSA : public VersalKey
{
public:
    KeyECDSA(const std::string& name0) : VersalKey(name0) { }
    KeyECDSA(const KeyECDSA& otherKey) : VersalKey(otherKey) { }

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

/******************************************************************************/
class KeyECDSAp521 : public VersalKey
{
public:
    KeyECDSAp521(const std::string& name0) : VersalKey(name0) { }
    KeyECDSAp521(const KeyECDSAp521& otherKey) : VersalKey(otherKey) { }

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};
#endif
