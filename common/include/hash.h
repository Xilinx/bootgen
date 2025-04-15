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

#ifndef _HASH_H_
#define _HASH_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <string>
#include <openssl/sha.h>

#include "Keccak-compact.h"
#include "bootgenenum.h"


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define SHA2_LENGTH_BYTES           32 // ~ SHA_256_LENGTH_BYTES
#define SHA3_LENGTH_BYTES           48 // ~ SHA_384_LENGTH_BYTES

//#define SHAKE256_LENGTH_BYTES   32
//#define SHA3_256_LENGTH_BYTES   32

/* BH Hash Block for telluride has 
            6 hash slots of (6 * 48) bytes and 
            a corresponding index of 4 bytes, for each hash, and 
            a total alignment of 8 bytes

   Total            : (6 * (48 + 4)) + 8 = 0x140 bytes
   Order of hashes  : BH Hash, PLM Hash, PMC DATA Hash and Hash Block 1 Hash corresponding indexes of 0,1,2,3. Rest are reserved */

#define BH_HASH_BLOCK_BYTES_TELLURIDE         0x140
#define HASH_BLOCK_ALIGNMENT_BYTES_TELLURIDE  8

/* BH Hash Block for Lassen has
            4 hash slots of (4 * 32) bytes and
            a corresponding index of 4 bytes, for each hash, and
            a total alignment of 0 bytes

   Total            : (4 * (32 + 4)) + 0 = 0x90 bytes
   Order of hashes  : BH Hash, PLM Hash and PMC DATA Hash, corresponding indexes of 0,1,2. Rest are reserved */

#define BH_HASH_BLOCK_BYTES_LASSEN            0x90

/* BH Hash Block for Lassen has
            4 hash slots of (4 * 48) bytes and
            a corresponding index of 4 bytes, for each hash, and
            a total alignment of 0 bytes

   Total            : (4 * (48 + 4)) + 0 = 0xD0 bytes
   Order of hashes  : BH Hash, PLM Hash and PMC DATA Hash, corresponding indexes of 0,1,2. Rest are reserved */

#define BH_HASH_BLOCK_BYTES_LASSEN_DL9        0xD0
#define HASH_BLOCK_ALIGNMENT_BYTES_LASSEN     0

#define HASH_BLOCK_INDEX_BYTES             4

#define HASH_BLOCK_BH_HASH_INDEX           0
#define HASH_BLOCK_PLM_HASH_INDEX          1
#define HASH_BLOCK_PMCDATA_HASH_INDEX      2
#define HASH_BLOCK_HASHBLOCK1_HASH_INDEX   3

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class Hash 
{
public:
    Hash()
    : hashLength(0)
    { };

    virtual ~Hash() {}

    virtual void InitHash(void) = 0;
    virtual uint8_t GetHashLength(void) = 0;
    virtual uint8_t UpdateHash(const void*, size_t) = 0;
    virtual uint8_t FinalHash(uint8_t*) = 0;
    virtual void CalculateHash(bool flag, const uint8_t*, size_t, uint8_t*) = 0;
    virtual void CalculateVersalHash(bool flag, const uint8_t*, size_t, uint8_t*) {}
    virtual std::string GetHashFileExtension(void) = 0;

protected:
    uint8_t hashLength;
};

/******************************************************************************/
class HashSha2 : public Hash
{
public:
    HashSha2()
    {
        ctx.md_len = 0;
        ctx.num = 0;
        ctx.Nl = 0;
        ctx.Nh = 0;
    }

    ~HashSha2() {}

    uint8_t GetHashLength(void);
    void InitHash(void) { SHA256_Init(&ctx); }
    uint8_t UpdateHash(const void*, size_t);
    uint8_t FinalHash(uint8_t*);
    void CalculateHash(bool flag, const uint8_t*, size_t, uint8_t*);
    std::string GetHashFileExtension(void);

private:
    SHA256_CTX ctx;
};

/******************************************************************************/
class HashSha3 : public Hash
{
public:
    HashSha3()
    {
        ctx.bitsInQueue = 0;
    }

    ~HashSha3() {}

    uint8_t GetHashLength(void);
    void InitHash(void) { Init(&ctx); }
    uint8_t UpdateHash(const void*, size_t);
    uint8_t FinalHash(uint8_t*);
    void CalculateHash(bool flag, const uint8_t*, size_t, uint8_t*);
    void CalculateVersalHash(bool flag, const uint8_t*, size_t, uint8_t*);
    std::string GetHashFileExtension(void);

private:
    hashState ctx;
};

/******************************************************************************/
class HashShake256 : public Hash
{
public:
    HashShake256() { }
    ~HashShake256() { }

    uint8_t GetHashLength(void);
    void InitHash(void);
    uint8_t UpdateHash(const void*, size_t);
    uint8_t FinalHash(uint8_t*);
    void CalculateHash(bool flag, const uint8_t*, size_t, uint8_t*);
    std::string GetHashFileExtension(void);
};

/******************************************************************************/
class HashSha3_256 : public Hash
{
public:
    HashSha3_256() { }
    ~HashSha3_256() { }

    uint8_t GetHashLength(void);
    void InitHash(void);
    uint8_t UpdateHash(const void*, size_t);
    uint8_t FinalHash(uint8_t*);
    void CalculateHash(bool flag, const uint8_t*, size_t, uint8_t*);
    std::string GetHashFileExtension(void);
};
#endif
