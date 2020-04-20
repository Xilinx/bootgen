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

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "hash.h"
#include "Keccak-compact-versal.h"

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
uint8_t HashSha2::GetHashLength(void)
{
    return SHA2_LENGTH_BYTES;
}

/******************************************************************************/
uint8_t HashSha2::UpdateHash(const void* data, size_t length)
{
    uint8_t errorCode = SHA256_Update(&ctx, data, length);

    /* Send the inverted error code, to be consistent with the SHA3 algo */
    return (!errorCode);
}

/******************************************************************************/
uint8_t HashSha2::FinalHash(uint8_t* hashout)
{
    uint8_t errorCode = SHA256_Final(hashout, &ctx);

    /* Send the inverted error code, to be consistent with the SHA3 algo */
    return (!errorCode);
}

/******************************************************************************/
void HashSha2::CalculateHash(bool flag, const uint8_t *data, size_t length, uint8_t* out)
{
    SHA256(data, length, out);
}

/******************************************************************************/
std::string HashSha2::GetHashFileExtension(void)
{
    return ".sha256";
}

/******************************************************************************/
uint8_t HashSha3::GetHashLength(void)
{
    return SHA3_LENGTH_BYTES;
}

/******************************************************************************/
uint8_t HashSha3::UpdateHash(const void* data, size_t length)
{
    size_t length_in_bits = length * 8;
    
    return Update(&ctx, (BitSequence*)data, length_in_bits);
}

/******************************************************************************/
uint8_t HashSha3::FinalHash(uint8_t* hashout)
{
    return Final(&ctx, hashout, SHA3_LENGTH_BYTES);
}

/******************************************************************************/
void HashSha3::CalculateHash(bool nist, const uint8_t *data, size_t length, uint8_t* out)
{
    if (nist) 
    {
        crypto_hash_NIST_SHA3(out, data, length);
    }
    else 
    {
        crypto_hash(out, data, length);
    }
}

/******************************************************************************/
void HashSha3::CalculateVersalHash(bool nist, const uint8_t *data, size_t length, uint8_t* out)
{
    Versalcrypto_hash(out, data, length, nist);
}

/******************************************************************************/
std::string HashSha3::GetHashFileExtension(void)
{
    return ".sha384";
}
