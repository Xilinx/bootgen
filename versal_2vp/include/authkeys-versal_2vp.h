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

#ifndef _AUTH_KEYS_VERSAL_2VP_H_
#define _AUTH_KEYS_VERSAL_2VP_H_

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
class Versal_2vpKey : public Key
{
public:
    Versal_2vpKey(const std::string& name0);
    Versal_2vpKey(const Key& otherKey);
    ~Versal_2vpKey();

    void Parse(const std::string& filename, bool isSecret0);
    uint8_t ParseECDSAOpenSSLKey(const std::string& filename);

    EC_KEY *eckey;
    uint8_t *x;    // x co-ordinate (384 bits)
    uint8_t *y;    // y co-ordinate (384 bits)

    // PM-TODO Create a separate class
    HssPublicKey *public_key;
    HssPrivateKey *private_key;
    void ParseLmsKey(const std::string& filename); 

    // ML-DSA support
    uint8_t *mldsa_public_key;
    uint8_t *mldsa_private_key;
    void ParseMldsaKey(const std::string& filename);
    void ParseSLHdsaKey(const std::string& filename); 
};

/******************************************************************************/
class Key4096Sha3Padding_versal_2vp : public Versal_2vpKey
{
public:
    Key4096Sha3Padding_versal_2vp(const std::string& name0) : Versal_2vpKey(name0) { }
    Key4096Sha3Padding_versal_2vp(const Key4096Sha3Padding_versal_2vp& otherKey) : Versal_2vpKey(otherKey) { }

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};


/******************************************************************************/
class KeyECDSA_versal_2vp : public Versal_2vpKey
{
public:
    KeyECDSA_versal_2vp(const std::string& name0) : Versal_2vpKey(name0) { }
    KeyECDSA_versal_2vp(const KeyECDSA_versal_2vp& otherKey) : Versal_2vpKey(otherKey) { }

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

/******************************************************************************/
class KeyECDSAp521_versal_2vp : public Versal_2vpKey
{
public:
    KeyECDSAp521_versal_2vp(const std::string& name0) : Versal_2vpKey(name0) { }
    KeyECDSAp521_versal_2vp(const KeyECDSAp521_versal_2vp& otherKey) : Versal_2vpKey(otherKey) { }

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

/******************************************************************************/
class KeyLMS_versal_2vp : public Versal_2vpKey
{
public:
    KeyLMS_versal_2vp(const std::string& name0) : Versal_2vpKey(name0) { }
    KeyLMS_versal_2vp(const KeyLMS_versal_2vp& otherKey) : Versal_2vpKey(otherKey) { }

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

/******************************************************************************/
class KeyMLDSA_versal_2vp : public Versal_2vpKey
{
public:
    KeyMLDSA_versal_2vp(const std::string& name0) : Versal_2vpKey(name0) { }
    KeyMLDSA_versal_2vp(const KeyMLDSA_versal_2vp& otherKey) : Versal_2vpKey(otherKey) { }

    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

/******************************************************************************/
class KeySLH_versal_2vp : public Versal_2vpKey
{
public:
    KeySLH_versal_2vp(const std::string& name0) : Versal_2vpKey(name0) { }
    KeySLH_versal_2vp(const KeySLH_versal_2vp& otherKey) : Versal_2vpKey(otherKey) { }

    void LoadKey(const std::string& filename, uint8_t* buffer, size_t size);
    void Export(void* dst);
    void Import(const void* acKey, const std::string& name0);
};

#endif
