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
#define SHA2_LENGTH_BYTES   32
#define SHA3_LENGTH_BYTES   48


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

#endif
