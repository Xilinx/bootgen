/******************************************************************************
* Copyright 2015-2019 Xilinx, Inc.
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#include "encryption-zynq.h"
#include "bootimage.h"
#include "encryptutils.h"
#include "options.h"
#include "bifoptions.h"
#include "encryptutils.h"
#include "fileutils.h"
#include "stringutils.h"
#include "bootgenexception.h"
#include "systemutils.h"

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define BITS_PER_SHA_BLOCK     512
#define BYTE_PER_SHA_BLOCK     64

#define WORDS_PER_CBC_KEY      4
#define BYTES_PER_CBC_KEY      16

#define WORDS_PER_HMAC_KEY     8
#define BYTES_PER_HMAC_KEY     32

#define HMAC_IPAD_BYTE         0x36
#define HMAC_OPAD_BYTE         0x5C

#define BIT_STREAM_NOOP_COMMAND  0x20000000




static const uint32_t round_constant[30] =
{
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
    0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f,
    0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4,
    0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91
};

static const uint8_t s_box[16][16] =
{
    { 0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76 },
    { 0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0 },
    { 0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15 },
    { 0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75 },
    { 0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84 },
    { 0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF },
    { 0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8 },
    { 0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2 },
    { 0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73 },
    { 0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB },
    { 0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79 },
    { 0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08 },
    { 0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A },
    { 0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E },
    { 0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF },
    { 0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16 }
};

static const uint32_t Logtable[256] =
{
    0,   0,  25,   1,  50,   2,  26, 198,  75, 199,  27, 104,  51, 238, 223,   3,
    100,   4, 224,  14,  52, 141, 129, 239,  76, 113,   8, 200, 248, 105,  28, 193,
    125, 194,  29, 181, 249, 185,  39, 106,  77, 228, 166, 114, 154, 201,   9, 120,
    101,  47, 138,   5,  33,  15, 225,  36,  18, 240, 130,  69,  53, 147, 218, 142,
    150, 143, 219, 189,  54, 208, 206, 148,  19,  92, 210, 241,  64,  70, 131,  56,
    102, 221, 253,  48, 191,   6, 139,  98, 179,  37, 226, 152,  34, 136, 145,  16,
    126, 110,  72, 195, 163, 182,  30,  66,  58, 107,  40,  84, 250, 133,  61, 186,
    43, 121,  10,  21, 155, 159,  94, 202,  78, 212, 172, 229, 243, 115, 167,  87,
    175,  88, 168,  80, 244, 234, 214, 116,  79, 174, 233, 213, 231, 230, 173, 232,
    44, 215, 117, 122, 235,  22,  11, 245,  89, 203,  95, 176, 156, 169,  81, 160,
    127,  12, 246, 111,  23, 196,  73, 236, 216,  67,  31,  45, 164, 118, 123, 183,
    204, 187,  62,  90, 251,  96, 177, 134,  59,  82, 161, 108, 170,  85,  41, 157,
    151, 178, 135, 144,  97, 190, 220, 252, 188, 149, 207, 205,  55,  63,  91, 209,
    83,  57, 132,  60,  65, 162, 109,  71,  20,  42, 158,  93,  86, 242, 211, 171,
    68,  17, 146, 217,  35,  32,  46, 137, 180, 124, 184,  38, 119, 153, 227, 165,
    103,  74, 237, 222, 197,  49, 254,  24,  13,  99, 140, 128, 192, 247, 112,   7
};

static const uint32_t Alogtable[256] =
{
    1,   3,   5,  15,  17,  51,  85, 255,  26,  46, 114, 150, 161, 248,  19,  53,
    95, 225,  56,  72, 216, 115, 149, 164, 247,   2,   6,  10,  30,  34, 102, 170,
    229,  52,  92, 228,  55,  89, 235,  38, 106, 190, 217, 112, 144, 171, 230,  49,
    83, 245,   4,  12,  20,  60,  68, 204,  79, 209, 104, 184, 211, 110, 178, 205,
    76, 212, 103, 169, 224,  59,  77, 215,  98, 166, 241,   8,  24,  40, 120, 136,
    131, 158, 185, 208, 107, 189, 220, 127, 129, 152, 179, 206,  73, 219, 118, 154,
    181, 196,  87, 249,  16,  48,  80, 240,  11,  29,  39, 105, 187, 214,  97, 163,
    254,  25,  43, 125, 135, 146, 173, 236,  47, 113, 147, 174, 233,  32,  96, 160,
    251,  22,  58,  78, 210, 109, 183, 194,  93, 231,  50,  86, 250,  21,  63,  65,
    195,  94, 226,  61,  71, 201,  64, 192,  91, 237,  44, 116, 156, 191, 218, 117,
    159, 186, 213, 100, 172, 239,  42, 126, 130, 157, 188, 223, 122, 142, 137, 128,
    155, 182, 193,  88, 232,  35, 101, 175, 234,  37, 111, 177, 200,  67, 197,  84,
    252,  31,  33,  99, 165, 244,   7,   9,  27,  45, 119, 153, 176, 203,  70, 202,
    69, 207,  74, 222, 121, 139, 134, 145, 168, 227,  62,  66, 198,  81, 243,  14,
    18,  54,  90, 238,  41, 123, 141, 140, 143, 138, 133, 148, 167, 242,  13,  23,
    57,  75, 221, 124, 132, 151, 162, 253,  28,  36, 108, 180, 199,  82, 246,   1
};

const uint32_t K_256[64] =
{
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const uint32_t H0_256[8] =
{
    0x6a09e667,
    0xbb67ae85,
    0x3c6ef372,
    0xa54ff53a,
    0x510e527f,
    0x9b05688c,
    0x1f83d9ab,
    0x5be0cd19
};


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
static void loadBar(int i, int n, int r, int w)
{
    // Only update r times.
    if (n > r)
    {
        if (i % (n / r) != 0)
            return;
    }

    // Calculuate the ratio of complete-to-incomplete.
    float ratio = i / (float)n;
    int c = (int)(ratio * w + 0.5);

    // Show the percentage complete.
    std::cout << std::setw(3) << (int)(ratio * 100) << "% [";

    // Show the load bar.
    int x;
    for (x = 0; x<c; x++)
    {
        std::cout << "=";
    }

    for (x = c; x<w; x++)
    {
        std::cout << " ";
    }

    // Move to the first column
    std::cout << "]\r";

    if (i == n)
    {
        std::cout << "\n";
    }
}

/******************************************************************************/
static uint32_t rotate_right(uint32_t x, uint32_t val)
{
    return ((x >> val) | (x << (32 - val)));
}

/******************************************************************************/
static uint32_t shift_right(uint32_t x, uint32_t val)
{
    return (x >> val);
}

/******************************************************************************/
static uint32_t Ch(uint32_t	x, uint32_t	y, uint32_t	z)
{
    return ((x & y) ^ ((~x) & z));
}

/******************************************************************************/
static uint32_t Maj(uint32_t x, uint32_t y, uint32_t z)
{
    return ((x & y) ^ (x & z) ^ (y & z));
}

/******************************************************************************/
static uint32_t c_sigma_0(uint32_t x)
{
    return (rotate_right(x, 2) ^ rotate_right(x, 13) ^ rotate_right(x, 22));
}

/******************************************************************************/
static uint32_t c_sigma_1(uint32_t x)
{
    return (rotate_right(x, 6) ^ rotate_right(x, 11) ^ rotate_right(x, 25));
}

/******************************************************************************/
static uint32_t sigma_0(uint32_t x)
{
    return (rotate_right(x, 7) ^ rotate_right(x, 18) ^ shift_right(x, 3));
}

/******************************************************************************/
static uint32_t sigma_1(uint32_t x)
{
    return (rotate_right(x, 17) ^ rotate_right(x, 19) ^ shift_right(x, 10));
}

/******************************************************************************/
static void SubByte(uint8_t& value1, uint8_t& value2, uint8_t& value3, uint8_t& value4)
{
    uint32_t row;
    uint32_t col;

    row = (value1 >> 4) & 0x0F;
    col = value1 & 0x0F;
    value1 = s_box[row][col];

    row = (value2 >> 4) & 0x0F;
    col = value2 & 0x0F;
    value2 = s_box[row][col];

    row = (value3 >> 4) & 0x0F;
    col = value3 & 0x0F;
    value3 = s_box[row][col];

    row = (value4 >> 4) & 0x0F;
    col = value4 & 0x0F;
    value4 = s_box[row][col];
}

/******************************************************************************/
static uint8_t aes_mul(uint32_t a, uint32_t b)
{
    if (a && b)
    {
        return (uint8_t)Alogtable[(Logtable[a] + Logtable[b]) % 255];
    }
    else
    {
        return 0;
    }
}

/******************************************************************************/
void ZynqEncryptionContext::Process(BootImage& bi, PartitionHeader* partHdr)
{
    Options& options = bi.options;
    /* Setup the AES encryption context */
    DataStreamEncryption aes_encrypt(WORDS_PER_AES_BLOCK, WORDS_PER_AES_KEY);

    /* Data mover for handling endianness across architectures
       7-series FPGA requires the data in Big Endian format
       Zynq & ZynqMp requires the data in Little Endian format */
    aes_encrypt.dataMover = new DataMoverLE(); 
    aes_encrypt.maskEfuseFlag = false; 
    /* Get the key file or keys (Key0, StartCBC, HMAC) passed from command line */
    std::string keyFilename = options.GetEncryptionKeyFile();
    std::string key0 = options.GetKey0();
    std::string startCbc = options.GetStartCbc();
    std::string hmac = options.GetHmac();

    /* If the key file is not given, generate a key file with the BIF file name */
    if (aesFilename == "")
    {
        if (keyFilename == "")
        {
            aesFilename = StringUtils::ChangeExtension(options.GetBifFilename(), ".nky");
        }
        else
        {
            aesFilename = keyFilename;
        }
    }

    bool exists;
    std::ifstream keyFile(aesFilename.c_str());
    exists = keyFile.good();

    /* If the file already exists */
    if (exists)
    {
        // Read the existing key file 
        aes_encrypt.ReadEncryptionKeyFile(aesFilename);

        // Overwrite with the keys, if specified on the command line
        if (key0 != "")
        {
            aes_encrypt.SetAesKeyString(key0);
        }
        if (startCbc != "")
        {
            aes_encrypt.setCbcString(startCbc);
        }
        if (hmac != "")
        {
            aes_encrypt.SetHmacKeyString(hmac);
        }
    }
    /* If the key file is not specified - generate it */
    else
    {
        aes_encrypt.GenerateEncryptionKeyFile(aesFilename, options);
    }

    /* Get the key source */
    KeySource::Type keySource = options.GetEncryptedKeySource();

    if (!Binary::CheckAddress(partHdr->partition->section->Length))
    {
        LOG_ERROR("Partition too large to encrypt");
    }

    bool isFPGA = partHdr->imageHeader->GetDomain() == Domain::PL;

    /* Output buffer & output length */
    uint8_t* encryptedDataBuffer;
    uint32_t encryptedBufferByteLength;

    /* Encrypt the stream */
    aes_encrypt.AESDataStreamEncrypt(keySource, isFPGA, partHdr->partition->section->Data, (uint32_t)partHdr->partition->section->Length,
        encryptedDataBuffer, encryptedBufferByteLength, partHdr->preservedBitstreamHdr.data, partHdr->preservedBitstreamHdr.size);
    delete[] partHdr->partition->section->Data;

    /* Populate the partition headers */
    partHdr->partition->section->Data = encryptedDataBuffer;
    partHdr->partition->section->Length = encryptedBufferByteLength;
    partHdr->imageHeader->SetTotalFsblFwSizeIh(encryptedBufferByteLength);
}

/******************************************************************************/
ZynqEncryptionContext::ZynqEncryptionContext()
{
    wordsPerAesKey = WORDS_PER_AES_KEY;
    wordsPerAesBlock = WORDS_PER_AES_BLOCK;
    keySchedule = NULL;
    aesKey = NULL;
    hmacKey = NULL;
    first_AES_block = false;
    d_NumRounds = 0;
    working_AES_block = NULL;
    dataMover = NULL;
    maskEfuseFlag = false;
}

/******************************************************************************/
ZynqEncryptionContext::ZynqEncryptionContext(const EncryptionContext * other)
    : wordsPerAesKey(WORDS_PER_AES_KEY)
    , wordsPerAesBlock(WORDS_PER_AES_BLOCK)
    , working_AES_block(NULL)
    , keySchedule(NULL)
    , aesKey(NULL)
    , hmacKey(NULL)
    , first_AES_block(false)
    , dataMover(NULL)
    , maskEfuseFlag(false)
    , d_NumRounds(0)
{
    aesFilename = other->aesFilename;
}

/******************************************************************************/
ZynqEncryptionContext::ZynqEncryptionContext(uint32_t words_per_AES_block0, uint32_t words_per_AES_key0)
    : wordsPerAesKey(words_per_AES_key0)
    , wordsPerAesBlock(words_per_AES_block0)
    , keySchedule(NULL)
    , aesKey(NULL)
    , hmacKey(NULL)
    , first_AES_block(false)
    , dataMover(NULL)
    , maskEfuseFlag(false)
{
    memset(ivCbc, 0, 16);

    if ((wordsPerAesBlock == 8) || (wordsPerAesKey == 8))
    {
        d_NumRounds = 14;
    }
    else
    {
        if ((wordsPerAesBlock == 6) || (wordsPerAesKey == 6))
        {
            d_NumRounds = 12;
        }
        else
            d_NumRounds = 10;
    }

    working_AES_block = new uint32_t[wordsPerAesBlock];
}

/******************************************************************************/
ZynqEncryptionContext::~ZynqEncryptionContext(void)
{
    if (aesKey != NULL)
        delete[] aesKey;

    if (keySchedule != NULL)
        delete[] keySchedule;

    if (hmacKey != NULL)
        delete[] hmacKey;

    if (working_AES_block != NULL)
        delete[] working_AES_block;
}

/******************************************************************************/
uint32_t ZynqEncryptionContext::CvtChars2Int(uint8_t	value1, uint8_t	value2, uint8_t	value3, uint8_t	value4)
{
    return (value1 << 24) | (value2 << 16) | (value3 << 8) | value4;
}

/******************************************************************************/
void ZynqEncryptionContext::ClearKeySchedule(void)
{
    // Allocate a key schedule if we don't already have one.
    if (keySchedule == NULL)
        keySchedule = new uint32_t[sizeof(uint32_t) * (d_NumRounds + 1)];

    for (uint32_t index = 0; index < sizeof(uint32_t) * (d_NumRounds + 1); index++)
    {
        keySchedule[index] = 0;
    }
}

/******************************************************************************/
void ZynqEncryptionContext::SetAesKey(const uint8_t* key)
{
    aesKey = new uint32_t[WORDS_PER_AES_KEY];
    for (uint32_t index = 0; index < WORDS_PER_AES_KEY; index++)
    {
        aesKey[index] = ReadBigEndian32(key);
        key += sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqEncryptionContext::GenerateAesKey(void)
{
    uint32_t keysize = WORDS_PER_AES_KEY * sizeof(uint32_t);
    uint8_t newKeyData[BYTES_PER_AES_KEY];
    uint8_t newKey[BYTES_PER_AES_KEY];

    // Get temp data to encrypt as the newKey.
    SetRandomSeed();
    GetRandomData(newKeyData, keysize);
    memcpy(newKey, newKeyData, keysize);

    // Encrypt the newKeyData with the temp key and temp CBC into newKey as the generated new key.
    EncryptKeys(newKeyData, keysize, newKey);
    SetAesKey(newKey);
    LOG_INFO("AES Key generated successfully");
}

/******************************************************************************/
void ZynqEncryptionContext::EncryptKeys(const uint8_t* newKeyData, uint32_t keysize, uint8_t* newKey)
{
    // Use a local encrypt object so that we don't disturb any of the caller's keys.
    ZynqEncryptionContext temp_encrypt(wordsPerAesBlock, wordsPerAesKey);
    temp_encrypt.dataMover = new DataMoverLE();

    temp_encrypt.GenerateTempKeys();
    temp_encrypt.InitKeySchedule();
    temp_encrypt.AES_Encrypt(newKeyData, keysize, newKey);
}

/******************************************************************************/
const uint8_t* ZynqEncryptionContext::GetAesKey(void)
{
    return (uint8_t *)this->aesKey;
}

/******************************************************************************/
void ZynqEncryptionContext::SetCbc(const uint8_t* initialVector)
{
    memcpy(ivCbc, initialVector, WORDS_PER_AES_BLOCK * sizeof(uint32_t));
}

/******************************************************************************/
void ZynqEncryptionContext::setCbcString(const std::string& initialVector)
{
    uint8_t  hexData[256];

    if (initialVector.size() != (BYTES_PER_CBC_KEY * 2))
    {
        LOG_DEBUG(DEBUG_STAMP, "CBC key size - %d", initialVector.size());
        LOG_ERROR("An CBC key must be 128 bits long - %s", initialVector.c_str());
    }
    PackHex(initialVector, hexData);
    SetCbc(hexData);
}

/******************************************************************************/
void ZynqEncryptionContext::GenerateCbc(void)
{
    uint8_t newCBC[BYTES_PER_CBC_KEY];
    uint8_t newCBCData[BYTES_PER_CBC_KEY];

    // Setup a new encrypt object so that we don't disturb any of the caller's keys.
    ZynqEncryptionContext temp_encrypt(WORDS_PER_AES_BLOCK, WORDS_PER_AES_KEY);
    temp_encrypt.dataMover = new DataMoverLE();

    // Get temp data to encrypt as the newCBC.
    SetRandomSeed();
    GetRandomData(newCBCData, BYTES_PER_CBC_KEY);

    /* Encrypt the newKeyData with the temp key and temp CBC into newCBC as the generated
       new CBC. */
    temp_encrypt.GenerateTempKeys();
    temp_encrypt.InitKeySchedule();
    temp_encrypt.AES_Encrypt(newCBCData, BYTES_PER_CBC_KEY, newCBC);

    SetCbc(newCBC);
    LOG_INFO("CBC generated successfully");

}

/******************************************************************************/
const uint8_t * ZynqEncryptionContext::GetCbc(void)
{
#if 1
    static uint8_t be[16];
    ((uint32_t*)be)[0] = ReadBigEndian32(ivCbc);
    ((uint32_t*)be)[1] = ReadBigEndian32(ivCbc + 4);
    ((uint32_t*)be)[2] = ReadBigEndian32(ivCbc + 8);
    ((uint32_t*)be)[3] = ReadBigEndian32(ivCbc + 12);
    return be;
#else
    return (uint8_t *)this->d_InitialVector;
#endif
}

/******************************************************************************/
void ZynqEncryptionContext::SetHmacKey(const uint8_t * HMACKey)
{
    hmacKey = new uint32_t[WORDS_PER_HMAC_KEY];

    for (uint32_t index = 0; index < WORDS_PER_HMAC_KEY; index++)
    {
        hmacKey[index] = ReadBigEndian32(HMACKey);
        HMACKey += sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqEncryptionContext::SetHmacKeyString(const std::string& HMACKey)
{
    uint8_t  hexData[256];

    if (HMACKey.size() != (BYTES_PER_HMAC_KEY * 2))
    {
        LOG_DEBUG(DEBUG_STAMP, "HMAC key size - %d", HMACKey.size());
        LOG_ERROR("An HMAC key must be 128 bits long - %s", HMACKey.c_str());
    }
    PackHex(HMACKey, hexData);
    SetHmacKey(hexData);
}

/******************************************************************************/
void ZynqEncryptionContext::GenerateHmacKey(void)
{
    uint8_t newHMAC[BYTES_PER_HMAC_KEY];
    uint8_t newHMACData[BYTES_PER_HMAC_KEY];

    // Setup a new encrypt object so that we don't disturb any of the caller's keys.
    ZynqEncryptionContext temp_encrypt(WORDS_PER_AES_BLOCK, WORDS_PER_AES_KEY);
    temp_encrypt.dataMover = new DataMoverLE();

    // Get temp data to encrypt as the newHMAC.
    SetRandomSeed();
    GetRandomData(newHMACData, BYTES_PER_HMAC_KEY);

    /* Encrypt the newHMACData with the temp key and temp CBC into newKey as the generated
       new HMAC */
    temp_encrypt.GenerateTempKeys();
    temp_encrypt.InitKeySchedule();
    temp_encrypt.AES_Encrypt(newHMACData, BYTES_PER_HMAC_KEY, newHMAC);

    SetHmacKey(newHMAC);
    LOG_INFO("HMAC generated successfully");
}

/******************************************************************************/
const uint8_t * ZynqEncryptionContext::GetHmacKey(void)
{
    return (uint8_t *)this->hmacKey;
}

/******************************************************************************/
uint32_t ZynqEncryptionContext::Transform(uint32_t orig_col, uint32_t col_num, bool rotate_rcon)
{
    uint8_t element1;
    uint8_t element2;
    uint8_t element3;
    uint8_t element4;


    if (rotate_rcon != 0)
    {
        element1 = (orig_col >> 16) & 0xff;
        element2 = (orig_col >> 8) & 0xff;
        element3 = orig_col & 0xff;
        element4 = orig_col >> 24;
    }
    else
    {
        element1 = orig_col >> 24;
        element2 = (orig_col >> 16) & 0xff;
        element3 = (orig_col >> 8) & 0xff;
        element4 = orig_col & 0xff;
    }
    SubByte(element1, element2, element3, element4);
    if (rotate_rcon != 0)
    {
        element1 ^= round_constant[(col_num / wordsPerAesKey) - 1];
    }

    return CvtChars2Int(element1, element2, element3, element4);
}

/******************************************************************************/
void ZynqEncryptionContext::InitKeySchedule(void)
{
    uint32_t	col;

    // Clear any key schedule, and make sure we
    // start CBC chaining from the beginning.
    ClearKeySchedule();

    first_AES_block = true;

    // copy original key into columns 1 - d_NumKey of key schedule
    for (col = 0; col < wordsPerAesKey; col++)
    {
        keySchedule[col] = aesKey[col];
    }

    // Set the remaining columns of the schedule
    for (col = wordsPerAesKey; col < sizeof(uint32_t) * (d_NumRounds + 1); col++)
    {
        uint32_t T_prev_col;

        // If col is a multiple of wordsPerAesKey, transform
        if ((col % wordsPerAesKey) == 0)
        {
            T_prev_col = Transform(keySchedule[col - 1], col, true);
        }
        else
        {
            if ((wordsPerAesKey == 8) && ((col % 4) == 0))
            {
                // For keysize 256, sub bytes for col multiple of 4
                T_prev_col = Transform(keySchedule[col - 1], col, false);
            }
            else
            {
                T_prev_col = keySchedule[col - 1];
            }
        }
        keySchedule[col] = keySchedule[col - wordsPerAesKey] ^ T_prev_col;
    }
}

/******************************************************************************/
void ZynqEncryptionContext::AddRoundKey(uint32_t *	int_array, uint32_t	num_round)
{
    for (uint32_t index = 0; index < wordsPerAesBlock; index++)
    {
        int_array[index] ^= keySchedule[(sizeof(uint32_t) * num_round) + index];
    }
}

/******************************************************************************/
void ZynqEncryptionContext::ByteSubTransformation(uint32_t *	int_array)
{
    uint8_t element1;
    uint8_t element2;
    uint8_t element3;
    uint8_t element4;

    for (uint32_t index = 0; index < wordsPerAesBlock; index++)
    {
        element1 = int_array[index] >> 24;
        element2 = (int_array[index] >> 16) & 0xff;
        element3 = (int_array[index] >> 8) & 0xff;
        element4 = int_array[index] & 0xff;
        SubByte(element1, element2, element3, element4);
        int_array[index] = CvtChars2Int(element1, element2, element3, element4);
    }
}

/******************************************************************************/
void ZynqEncryptionContext::ShiftRowTransformation(uint32_t *	int_array)
{
    uint32_t num_cols = wordsPerAesBlock;
    uint32_t temp_col0 = int_array[0];
    uint32_t temp_col1 = int_array[1];
    uint32_t temp_col2 = int_array[2];

    for (uint32_t index = 0; index < num_cols - 3; index++)
    {
        int_array[index] = (int_array[index + 0] & 0xff000000) |
                           (int_array[index + 1] & 0x00ff0000) |
                           (int_array[index + 2] & 0x0000ff00) |
                           (int_array[index + 3] & 0x000000ff);
    }

    int_array[num_cols - 3] = (int_array[num_cols - 3] & 0xff000000) |
                              (int_array[num_cols - 2] & 0x00ff0000) |
                              (int_array[num_cols - 1] & 0x0000ff00) |
                              (temp_col0 & 0x000000ff);

    int_array[num_cols - 2] = (int_array[num_cols - 2] & 0xff000000) |
                              (int_array[num_cols - 1] & 0x00ff0000) |
                              (temp_col0 & 0x0000ff00) |
                              (temp_col1 & 0x000000ff);

    int_array[num_cols - 1] = (int_array[num_cols - 1] & 0xff000000) |
                              (temp_col0 & 0x00ff0000) |
                              (temp_col1 & 0x0000ff00) |
                              (temp_col2 & 0x000000ff);
}

/******************************************************************************/
void ZynqEncryptionContext::MixColumnTransformation(uint32_t* int_array)
{
    uint8_t in_byte0;
    uint8_t in_byte1;
    uint8_t in_byte2;
    uint8_t in_byte3;
    uint8_t temp_byte0;
    uint8_t temp_byte1;
    uint8_t temp_byte2;
    uint8_t temp_byte3;

    for (uint32_t index = 0; index < wordsPerAesBlock; index++)
    {

        in_byte0 = int_array[index] >> 24;
        in_byte1 = (int_array[index] >> 16) & 0xff;
        in_byte2 = (int_array[index] >> 8) & 0xff;
        in_byte3 = int_array[index] & 0xff;

        temp_byte0 = aes_mul(2, (uint32_t)in_byte0) ^
                     aes_mul(3, (uint32_t)in_byte1) ^
                     in_byte2 ^ in_byte3;

        temp_byte1 = in_byte0 ^
                     aes_mul(2, (uint32_t)in_byte1) ^
                     aes_mul(3, (uint32_t)in_byte2) ^
                     in_byte3;

        temp_byte2 = in_byte0 ^
                     in_byte1 ^
                     aes_mul(2, (uint32_t)in_byte2) ^
                     aes_mul(3, (uint32_t)in_byte3);

        temp_byte3 = aes_mul(3, (uint32_t)in_byte0) ^
                     in_byte1 ^
                     in_byte2 ^
                     aes_mul(2, (uint32_t)in_byte3);

        int_array[index] = CvtChars2Int(temp_byte0, temp_byte1,
                                        temp_byte2, temp_byte3);
    }
}

/******************************************************************************/
void ZynqEncryptionContext::GenerateTempKeys(void)
{
    uint8_t tempKey[BYTES_PER_AES_KEY];
    SetRandomSeed();
    GetRandomData(tempKey, BYTES_PER_AES_KEY);
    SetAesKey(tempKey);

    uint8_t tempCBC[BYTES_PER_CBC_KEY];
    SetRandomSeed();
    GetRandomData(tempCBC, BYTES_PER_CBC_KEY);
    SetCbc(tempCBC);
}

/******************************************************************************/
void ZynqEncryptionContext::WriteEncryptionKeyFile(const std::string& baseFileName, bool useOptionalKey, uint32_t blocks)
{
    uint32_t index;

    /* Setup the file for writing */
    std::string ext = StringUtils::GetExtension(baseFileName);
    std::string filename;
    if (ext == "")
    {
        filename = baseFileName + ".nky";
    }
    else
    {
        filename = baseFileName;
    }
    std::ofstream keyFile(filename.c_str());

    if (!keyFile)
    {
        LOG_ERROR("Failure writing AES key file", filename.c_str());
    }

    /* Write device name */
    if (deviceName != "")
    {
        keyFile << "Device       " << deviceName << ";\n";
    }

    /* Write AES key */
    keyFile << "Key 0        ";
    for (index = 0; index < wordsPerAesKey; ++index)
    {
        keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << aesKey[index];
    }
    keyFile << ";\n";

    /* Write CBC Initial Vector */
    keyFile << "Key StartCBC ";
    for (index = 0; index < WORDS_PER_CBC_KEY * 4; ++index)
    {
        keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (int)ivCbc[index];
    }
    keyFile << ";\n";

    /* Write HMAC key */
    keyFile << "Key HMAC     ";
    for (index = 0; index < WORDS_PER_HMAC_KEY; ++index)
    {
        keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << hmacKey[index];
    }
    keyFile << ";\n";
}

/******************************************************************************/
void ZynqEncryptionContext::ReadEncryptionKeyFile(const std::string& inputFileName)
{
    std::ifstream keyFile(inputFileName.c_str());
    if (!keyFile)
    {
        LOG_ERROR("Failure reading AES key file - %s", inputFileName.c_str());
    }
    uint8_t noOfSemiColons = 0;

    while (keyFile)
    {
        std::string word;
        keyFile >> word;
        if (word == "")
        {
            break;
        }

        char c;
        if (word == "Key")
        {
            // First Word is "Key"
            keyFile >> word;

            if (word == "0")
            {
                // Second Word is "0"
                word = "";
                while ((keyFile >> c) && isalnum(c))
                {
                    word.push_back(c);
                }
                if (word == "pick")
                {
                    // Third Word is "pick" - then generate AES key, otherwise set the key specified
                    GenerateAesKey();
                }
                else
                {
                    SetAesKeyString(word);
                }
            }
            else if (word == "StartCBC")
            {
                // Second Word is "StartCBC"
                word = "";
                while ((keyFile >> c) && isalnum(c))
                {
                    word.push_back(c);
                }
                if (word == "pick")
                {
                    // Third Word is "pick" - then generate CBC key, otherwise set the key specified
                    GenerateCbc();
                }
                else
                {
                    setCbcString(word);
                }
            }
            else if (word == "HMAC")
            {
                // Second Word is "HMAC"
                word = "";
                while ((keyFile >> c) && isalnum(c))
                {
                    word.push_back(c);
                }
                if (word == "pick")
                {
                    // Third Word is "pick" - then generate HMAC key, otherwise set the key specified
                    GenerateHmacKey();
                }
                else
                {
                    SetHmacKeyString(word);
                }
            }
            else
            {
                // Second Word other than "0", "StartCBC" or "HMAC" - throw error
                LOG_DEBUG(DEBUG_STAMP, "Unknown key type - '%s' in %s", word.c_str(), inputFileName.c_str());
                LOG_ERROR("Error parsing AES key file - %s", inputFileName.c_str());
            }
        }
        else if (word == "Device")
        {
            // If first word is not "Key" but "Device"
            word = "";
            while ((keyFile >> c) && (isalnum(c) || c == '-' || c == '_'))
            {
                word.push_back(c);
            }
            deviceName = word;
        }
        else
        {
            // If first word is neither "Key" nor "Device"
            LOG_DEBUG(DEBUG_STAMP, "'Key' or 'Device' identifier expected, '%s' found instead", word.c_str());
            LOG_ERROR("Error parsing AES key file - %s", inputFileName.c_str());
        }
        if (c == ';')
        {
            // Semicolons expected at end of every line
            noOfSemiColons++;
        }
    }

    if (deviceName == "")
    {
        LOG_DEBUG(DEBUG_STAMP, "Partname 'Device' missing in the key file - %s", inputFileName.c_str());
        LOG_ERROR("Error parsing AES key file - %s. \n           Partname 'Device' missing in the key file", inputFileName.c_str());
    }

    if (noOfSemiColons != 4)
    {
        LOG_DEBUG(DEBUG_STAMP, "Terminating ';' expected at end of each line");
        LOG_ERROR("Error parsing AES key file - %s. \n           Terminating ';' expected at end of each line", inputFileName.c_str());
    }
}

/******************************************************************************/
void ZynqEncryptionContext::GenerateEncryptionKeyFile(const std::string& baseFileName, Options& options)
{
    // Get the device name, AES key, CBC & HMAC
    std::string devicePartName = options.GetDevicePartName();
    std::string key = options.GetKey0();
    std::string initialVector = options.GetStartCbc();
    std::string hmac = options.GetHmac();

    // If device name is mentioned, set it, otherwise throw exception
    if (devicePartName != "")
    {
        SetDeviceName(devicePartName);
    }
    else
    {
        LOG_ERROR("Partname must be specified with -p <partname> option in command line for generating a key file");
    }

    // If AES key is not mentioned, generate it, otherwise set the AES key
    if (key == "")
    {
        GenerateAesKey();
    }
    else
    {
        SetAesKeyString(key);
    }

    // If CBC key is not mentioned, generate it, otherwise set the CBC key
    if (initialVector == "")
    {
        GenerateCbc();
    }
    else
    {
        setCbcString(initialVector);
    }

    // If HMAC key is not mentioned, generate it, otherwise set the HMAC key
    if (hmac == "")
    {
        GenerateHmacKey();
    }
    else
    {
        SetHmacKeyString(hmac);
    }

    // Write all the encryption keys to a file
    WriteEncryptionKeyFile(baseFileName);
}

/******************************************************************************/
void ZynqEncryptionContext::AES_Encrypt(const uint8_t* inputData, uint32_t inputDataByteLength, uint8_t* encryptedData)
{
    uint32_t inputDataAESBlockCount;
    uint8_t * encryptedDataPtr = encryptedData;
    uint32_t dataValue;

    // Check if message has correct block size
    if ((inputDataByteLength > 0) && ((inputDataByteLength % wordsPerAesBlock) != 0))
    {
        LOG_DEBUG(DEBUG_STAMP, "Incorrect message bit length %ld, should be multiple of %ld bytes", inputDataByteLength, wordsPerAesBlock);
        LOG_ERROR("Encryption error !!!");
    }

    // Make sure we have a initialized key schedule.
    if (keySchedule == NULL)
    {
        ClearKeySchedule();
    }

    // Encrypt all AES blocks in the input data.
    inputDataAESBlockCount = inputDataByteLength / (sizeof(uint32_t) * wordsPerAesBlock);

    LOG_INFO("Encrypting %d bytes %d blocks", inputDataByteLength, inputDataAESBlockCount);
    for (uint32_t curr_AES_block_index = 0; curr_AES_block_index < inputDataAESBlockCount; curr_AES_block_index += 1)
    {
        if (inputDataAESBlockCount > 4)
        {
            loadBar(curr_AES_block_index, inputDataAESBlockCount - 1, 60, 60);
        }

        /* Convert incoming chars into array of ints */
        if (first_AES_block == true)
        {
            first_AES_block = false;
            for (uint32_t index = 0; index < wordsPerAesBlock; index++)
            {
                //dataValue  = ReadLittleEndian32( inputData );
                dataValue = dataMover->ReadData32(inputData); // DataMovement
                dataValue ^= ReadBigEndian32(ivCbc + 4 * index);
                working_AES_block[index] = reverseDataBits(dataValue);
                inputData += sizeof(uint32_t);
            }
        }
        else
        {
            for (uint32_t index = 0; index < wordsPerAesBlock; index++)
            {
                //dataValue = ReadLittleEndian32( inputData );
                dataValue = dataMover->ReadData32(inputData); // DataMovement

                /* !!!!!! This has to be done, why !!!!!!
                Reverse the bit order of each 32 bit value of the non-encrypted input data. */
                dataValue = reverseDataBits(dataValue);
                working_AES_block[index] ^= dataValue;
                inputData += sizeof(uint32_t);
            }
        }

        // 0th round key addition
        AddRoundKey(working_AES_block, 0);

        // Nr - 1 rounds of AES (BS, SR, MRC, ARK)
        for (uint32_t round = 1; round < d_NumRounds; round++)
        {
            ByteSubTransformation(working_AES_block);
            ShiftRowTransformation(working_AES_block);
            MixColumnTransformation(working_AES_block);
            AddRoundKey(working_AES_block, round);
        }

        // Last round (BS, SR, ARK) Byte sub transformation
        ByteSubTransformation(working_AES_block);
        ShiftRowTransformation(working_AES_block);
        AddRoundKey(working_AES_block, d_NumRounds);

        // write out the encrypted AES block.
        for (uint32_t index = 0; index < wordsPerAesBlock; index++)
        {
            dataValue = working_AES_block[index];

            /* !!!!!! This has to be done, why !!!!!!
            Reverse the bit order of each 32 bit value of the non-encrypted input data. */
            dataValue = reverseDataBits(dataValue);
            //WriteLittleEndian32( encryptedDataPtr, dataValue );
            dataMover->WriteData32(encryptedDataPtr, dataValue); // DataMovement
            encryptedDataPtr += sizeof(uint32_t);
        }
    }
}

/******************************************************************************/
void ZynqEncryptionContext::ComputeHMACDigest(const uint8_t* data,
                                              const uint32_t dataByteLength,
                                              bool initializeNewDigest,
                                              uint8_t* digest)
{
    uint32_t dataBitLength = dataByteLength * BITS_IN_BYTE;

    // Check if message has correct block size
    if ((dataBitLength > 0) && ((dataBitLength % BITS_PER_SHA_BLOCK) != 0))
    {
        LOG_DEBUG(DEBUG_STAMP, "Incorrect message bit length %ld for SHA256 computation, should be a multiple of %ld bits", (uint32_t)dataBitLength, (uint32_t)BITS_PER_SHA_BLOCK);
        LOG_ERROR("Encryption error !!!\n           Error while calculating HMAC Digest");
    }

    uint32_t a, b, c, d, e, f, g, h;
    uint32_t T1, T2;
    uint32_t Wi[64];
    uint32_t Hi[8];

    uint32_t index;

    /* Initialize the hash if requested. Otherwise continue calculating the hash with the new
    data given. */
    if (initializeNewDigest == true)
    {
        // Start with the inital SHA256 hash values.
        for (index = 0; index < WORDS_PER_HMAC_KEY; ++index)
        {
            Hi[index] = H0_256[index];
        }
    }
    else
    {
        // Start with the previous hash values.
        for (index = 0; index < WORDS_PER_HMAC_KEY; ++index)
        {
            //Hi[ index ] = ReadLittleEndian32( digest );
            Hi[index] = dataMover->ReadData32(digest); // DataMovement
            digest += sizeof(uint32_t);
        }

        // Restore the digest pointer to it's previous value.
        digest -= BYTES_PER_HMAC_KEY;
    }

#if 0
    LOG_TRACE("HMAC Message data of %d bytes", dataByteLength);
#endif

    while (dataBitLength > 0)
    {
        // Prepare message digest. First 16 integers are from the message
        for (index = 0; index < 16; ++index)
        {
            //Wi[ index ] = ReadLittleEndian32( data );
            Wi[index] = dataMover->ReadData32(data); // DataMovement
            data += sizeof(uint32_t);
            dataBitLength -= sizeof(uint32_t) * BITS_IN_BYTE;
        }

        for (index = 16; index < 64; ++index)
        {
            Wi[index] = sigma_1(Wi[index - 2]) + Wi[index - 7] + sigma_0(Wi[index - 15]) + Wi[index - 16];
        }

        // Initialize working variables
        a = Hi[0];
        b = Hi[1];
        c = Hi[2];
        d = Hi[3];
        e = Hi[4];
        f = Hi[5];
        g = Hi[6];
        h = Hi[7];

        // Compute working variables
        for (index = 0; index < 64; ++index)
        {
            T1 = h + c_sigma_1(e) + Ch(e, f, g) + K_256[index] + Wi[index];
            T2 = c_sigma_0(a) + Maj(a, b, c);

            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        // Compute intermediate hash
        Hi[0] += a;
        Hi[1] += b;
        Hi[2] += c;
        Hi[3] += d;
        Hi[4] += e;
        Hi[5] += f;
        Hi[6] += g;
        Hi[7] += h;
    }

    for (index = 0; index < WORDS_PER_HMAC_KEY; ++index)
    {
        //WriteLittleEndian32(digest, Hi[ index ]);
        dataMover->WriteData32(digest, Hi[index]);   // DataMovement
        digest += sizeof(uint32_t);
    }
}

/******************************************************************************/
DataStreamEncryption::DataStreamEncryption(uint32_t words_per_AES_block0, uint32_t words_per_AES_key0)
    : ZynqEncryptionContext(words_per_AES_block0, words_per_AES_key0)
    , input_data_Buffer(NULL)
    , stream_header(NULL)
    , input_data_ByteLength(0)
{
    memset(ipad_data, 0, BYTES_PER_IPAD);
    memset(pad1_data, 0, BYTES_PER_PAD1);
    memset(gap_2048, 0, BYTES_PER_2048_GAP);
    memset(opad_data, 0, BYTES_PER_OPAD);
    memset(first_digest_gap, 0, BYTES_PER_FIRST_DIGEST);
    memset(pad2_data, 0, BYTES_PER_PAD2);
    memset(HMAC_digest, 0, BYTES_PER_HMAC_DIGEST);
    memset(calculated_first_digest, 0, BYTES_PER_FIRST_DIGEST + BYTES_PER_PAD2);
}

/******************************************************************************/
DataStreamEncryption::~DataStreamEncryption(void)
{
    if (input_data_Buffer != NULL)   delete[] input_data_Buffer;
}

/******************************************************************************/
void DataStreamEncryption::SetDataStreamHeader(KeySource::Type keySource, uint32_t encryptedStreamByteLength, uint32_t* presBitHdr,
    uint32_t presBitHdrSize)
{
    uint32_t encryptedKeySource = BBRAM_KEY_SOURCE;
    uint32_t keySourceOffset;
    uint32_t encryptionEnableOffset;
    uint32_t cbcOffset;
    uint32_t encryptLengthOffset;

    std::string test = EncryptionContext::aesFilename;

    if (presBitHdrSize == 0)
    {
        stream_header = (uint8_t*)malloc(BYTES_PER_STREAM_HEADER);
        keySourceOffset = ENCRYPTION_KEY_SRC_OFFSET;
        encryptionEnableOffset = ENCRYPTION_ENABLE_WORD_OFFSET;
        cbcOffset = CBC_WORD_OFFSET;
        encryptLengthOffset = ENCRYPTION_DATA_LENGTH_OFFSET;

        for (uint8_t i = 0, j = 0; i<BYTES_PER_STREAM_HEADER / 4; i++, j = j + 4)
        {
            dataMover->WriteData32(&stream_header[j], defaultStreamHeader[i]);
        }
    }
    else
    {
        uint32_t presBitHdrBytes = presBitHdrSize * sizeof(uint32_t);
        stream_header = (uint8_t*)malloc(BYTES_PER_FPGA_STREAM_HEADER + presBitHdrBytes);
        keySourceOffset = FPGA_ENCRYPTION_KEY_SRC_OFFSET + presBitHdrBytes;
        encryptionEnableOffset = FPGA_ENCRYPTION_ENABLE_WORD_OFFSET + presBitHdrBytes;
        cbcOffset = FPGA_CBC_WORD_OFFSET + presBitHdrBytes;
        encryptLengthOffset = FPGA_ENCRYPTION_DATA_LENGTH_OFFSET + presBitHdrBytes;

        uint32_t j = 0;
        for (uint32_t i = 0; i<presBitHdrSize; i++, j = j + 4)
        {
            dataMover->WriteData32(&stream_header[j], presBitHdr[i]);
        }
        for (uint32_t i = 0; i<BYTES_PER_FPGA_STREAM_HEADER / 4; i++, j = j + 4)
        {
            dataMover->WriteData32(&stream_header[j], defaultFpgaStreamHeader[i]);

        }
    }

    /* Get the correct value for the encryption source. */
    if (keySource == KeySource::EfuseRedKey)
    {
        encryptedKeySource = EFUSE_KEY_SOURCE;
    }
    else if (keySource == KeySource::BbramRedKey)
    {
        encryptedKeySource = BBRAM_KEY_SOURCE;
    }
    else
    {
        LOG_ERROR("Key source %d not supported", (int)keySource);
    }

    /*For bitstreams, the mask flag needs to be set before the actual control word
        0x3000C001, //  32 Type1 write 1 words to MASK
        0x00000040, //  36 data word 1
        0x3000A001, //  40 Type1 write 1 words to CTL
        0x00000040, //  44 dec

        The command to mask (0x3000C001) followed by the data which specifies which
        bits to mask. All the bits needs to be masked which are of significance in the
        next command transaction.
        The command to config (0x3000A001) is followed by the actual data to set.
        For EFUSE, the bit 31 should be set
        For BBRAM, the bit 31 should be clear
        For setting the bit 31 in case of efuse, corresponding mask bit should be set

        Example: For enabling EFUSE,
        0x3000C001,  //  32 Type1 write 1 words to MASK
        0x80000040,  //  36 efuse mask bit31 (0x80000000)
        0x3000A001,  //  40 Type1 write 1 words to CTL
        0x80000040,  //  44 set efuse bits

        Ideally this should be done for both Zynq & FPGAs.
        But as these bits are not of much significance in case of Zynq, as the key
        selection is done by BootROM, and in case of FPGAs the key selection depends
        on these bits.

        Inorder to maintain backward compatibility w.r.t earlier versions of Zynq images
        and not to disturb the current regression test suite, this mask setting is done
        only for FPGA arch and not Zynq arch.

        This flag tells whether to set the mask bits or not */
    if (maskEfuseFlag)
    {
        dataMover->WriteData32(&stream_header[keySourceOffset], encryptedKeySource | ENCRYPTION_ENABLE);
    }

    /* Turn on encryption, and include the decryption key source. */
    dataMover->WriteData32(&stream_header[encryptionEnableOffset], encryptedKeySource | ENCRYPTION_ENABLE);

    /* Update the CBC in the stream header */
    const uint8_t* cbc = GetCbc();
    for (uint8_t i = 0; i<BYTES_PER_CBC_KEY; i = i + 4)
    {
        dataMover->WriteData32(&stream_header[cbcOffset + i], ReadLittleEndian32(cbc + i));
    }

    /* Finally, set the encrypted word length */
    dataMover->WriteData32(&stream_header[encryptLengthOffset], encryptedStreamByteLength / sizeof(uint32_t));
}

/******************************************************************************/
void DataStreamEncryption::InitializeInputDataStream(const uint8_t* inputData,
    uint32_t inputDataByteLength,
    bool isFPGAData)
{
    /* If this is NOT FPGA data, add the special bit stream header that sends data through
    the PCAP "loop back" interface on Zynq. This needs changing when later architectures
    come along. */

    // Add in the header if needed
    input_data_ByteLength = inputDataByteLength;

    if (isFPGAData == false)
    {
        input_data_ByteLength += sizeof(defaultPcapHeader);
    }

    /* Round up the length to the nearest 512 bit boundary for proper AES/HMAC encryption */
    uint32_t misAlignedByteSize = input_data_ByteLength % (512 / BITS_IN_BYTE);
    if (misAlignedByteSize != 0)
    {
        misAlignedByteSize = (512 / BITS_IN_BYTE) - misAlignedByteSize;
        input_data_ByteLength += misAlignedByteSize;
    }

    /* Allocate a buffer to hold the user data plus any header and padding NOOPs */
    input_data_Buffer = new uint8_t[input_data_ByteLength];
    if (input_data_Buffer == NULL)
    {
        LOG_DEBUG(DEBUG_STAMP, "Out of memory");
        LOG_ERROR("Ercryption error !!!");
    }

    uint8_t* inputDataStreamPtr = input_data_Buffer;

    // Add the header if needed.
    if (isFPGAData == false)
    {
        // Set the user's data word length in the write to the PCAP transfer register. Cap it at 27 bits.
        uint32_t inputDataWordLength = (inputDataByteLength / sizeof(uint32_t)) & 0x7FFFFFF;

        // Assemble the Pcap commands
        WriteLittleEndian32(inputDataStreamPtr, defaultPcapHeader[0]);
        inputDataStreamPtr += sizeof(uint32_t);
        WriteLittleEndian32(inputDataStreamPtr, defaultPcapHeader[1] | inputDataWordLength);
        inputDataStreamPtr += sizeof(uint32_t);
    }

    // Copy the user data to the buffer.
    memmove(inputDataStreamPtr, inputData, inputDataByteLength);

    // Point to where pading NOOPs should start.
    inputDataStreamPtr += inputDataByteLength;

    // Add any alignment NOOPs to the end of the buffer, align to a HMAC 512 bit boundary.
    while (misAlignedByteSize > 0)
    {
        // Write the next padding NOOP.
        WriteLittleEndian32(inputDataStreamPtr, BIT_STREAM_NOOP_COMMAND);

        // Go to the next padding NOOP.
        misAlignedByteSize -= sizeof(uint32_t);

        inputDataStreamPtr += sizeof(uint32_t);

    }
}

/******************************************************************************/
void DataStreamEncryption::InitializeHMAC_ioPadData(uint8_t padByte, uint8_t* padBuffer)
{
    /* Setup the ioPad buffer with it's default value.
    ioPad buffers are really 512 bits long, Xilinx HMAC keys are only 256 bits long.
    So use the Xilinx byte length times 2 to initialize the entire buffer.
    This will set the second half of the HMAC key to the default iPad or oPad value. */
    memset(padBuffer, padByte, BYTES_PER_HMAC_KEY * 2);

    /* The HMAC key goes into the first half of the ioPad.
    (Was wrong and used to say "second half"). */
    const uint8_t* HMACKeyPtr = GetHmacKey();
    uint32_t* padBufferWordPtr = (uint32_t *)padBuffer;

    for (uint32_t index = 0; index < WORDS_PER_HMAC_KEY; index += 1)
    {
        // XOR the HMAC key into the ioPad.
        //padBufferWordPtr[ index ] ^= ReadLittleEndian32( HMACKeyPtr );
        uint32_t temp;
        const uint8_t *temp1;
        temp = padBufferWordPtr[index] ^ ReadLittleEndian32(HMACKeyPtr);
        temp1 = (uint8_t*)padBufferWordPtr;
        //WriteBigEndian32(&padBufferWordPtr[index], temp);
        dataMover->WriteData32(&temp1[4 * index], temp);
        HMACKeyPtr += sizeof(uint32_t);
    }
}

/******************************************************************************/
void DataStreamEncryption::InitializePad1Data(uint32_t pad1DataByteLength)
{
    // Make sure it's cleared out.
    memset(pad1_data, 0, BYTES_PER_PAD1);

    // Set the first bit in the pad 1 buffer.
    //WriteLittleEndian32( pad1_data, 0x80000000 );
    dataMover->WriteData32(pad1_data, 0x80000000); // DataMovement

    // Set the length in bits.
    uint64_t length = pad1DataByteLength * BITS_IN_BYTE;
    //WriteLittleEndian32(&pad1_data[BYTES_PER_PAD1-2*sizeof(uint32_t)] ,(length>>32) & 0xFFFFFFFF);
    dataMover->WriteData32(&pad1_data[BYTES_PER_PAD1 - 2 * sizeof(uint32_t)], (length >> 32) & 0xFFFFFFFF); // DataMovement

    //WriteLittleEndian32(&pad1_data[BYTES_PER_PAD1-1*sizeof(uint32_t)] ,(length    ) & 0xFFFFFFFF);
    dataMover->WriteData32(&pad1_data[BYTES_PER_PAD1 - 1 * sizeof(uint32_t)], (length) & 0xFFFFFFFF); // DataMovement
}

/******************************************************************************/
void DataStreamEncryption::InitializePad2Data(uint32_t	pad2DataByteLength)
{
    // Make sure it's cleared out.
    memset(pad2_data, 0, BYTES_PER_PAD2);

    // Set the first bit in the pad 2 buffer.
    //WriteLittleEndian32( pad2_data, 0x80000000 );
    dataMover->WriteData32(pad2_data, 0x80000000); // DataMovement

    // Set the length in bits.
    uint64_t length = pad2DataByteLength * BITS_IN_BYTE;
    //WriteLittleEndian32(&pad2_data[BYTES_PER_PAD2-2*sizeof(uint32_t)], (length>>32) & 0xFFFFFFFF);
    dataMover->WriteData32(&pad2_data[BYTES_PER_PAD2 - 2 * sizeof(uint32_t)], (length >> 32) & 0xFFFFFFFF); // DataMovement

    //WriteLittleEndian32(&pad2_data[BYTES_PER_PAD2-1*sizeof(uint32_t)], (length) & 0xFFFFFFFF);
    dataMover->WriteData32(&pad2_data[BYTES_PER_PAD2 - 1 * sizeof(uint32_t)], (length) & 0xFFFFFFFF); // DataMovement
}

/******************************************************************************/
void DataStreamEncryption::EncryptDataStream(uint8_t* encryptedData)
{
    InitKeySchedule();

    AES_Encrypt(ipad_data, BYTES_PER_IPAD, encryptedData);
    encryptedData += BYTES_PER_IPAD;

    AES_Encrypt(input_data_Buffer, input_data_ByteLength, encryptedData);
    encryptedData += input_data_ByteLength;

    AES_Encrypt(pad1_data, BYTES_PER_PAD1, encryptedData);
    encryptedData += BYTES_PER_PAD1;

    AES_Encrypt(gap_2048, BYTES_PER_2048_GAP, encryptedData);
    encryptedData += BYTES_PER_2048_GAP;

    AES_Encrypt(opad_data, BYTES_PER_OPAD, encryptedData);
    encryptedData += BYTES_PER_OPAD;

    AES_Encrypt(first_digest_gap, BYTES_PER_FIRST_DIGEST, encryptedData);
    encryptedData += BYTES_PER_FIRST_DIGEST;

    AES_Encrypt(pad2_data, BYTES_PER_PAD2, encryptedData);
    encryptedData += BYTES_PER_PAD2;

    AES_Encrypt(HMAC_digest, BYTES_PER_HMAC_DIGEST, encryptedData);
}

/******************************************************************************/
void DataStreamEncryption::AESDataStreamEncrypt(KeySource::Type  keySource,
                                                bool isFPGAData,
                                                const uint8_t *inputData,
                                                uint32_t inputDataByteLength,
                                                uint8_t *& encryptedData,
                                                uint32_t & encryptedDataByteLength,
                                                uint32_t* presBitHdr,
                                                uint32_t presBitHdrSize)
{
    uint32_t streamHdrSize = 0;
    if (presBitHdrSize != 0)
    {
        streamHdrSize = BYTES_PER_FPGA_STREAM_HEADER + presBitHdrSize * sizeof(uint32_t);
    }
    else
    {
        streamHdrSize = BYTES_PER_STREAM_HEADER;
    }

    /* Check if message has correct block size */
    if (((inputDataByteLength * BITS_IN_BYTE) % BITS_IN_A_WORD) != 0)
    {
        LOG_DEBUG(DEBUG_STAMP, "Incorrect message bit length %ld for encryption, needs be a multiple of %ld bits",
            (uint32_t)(inputDataByteLength * BITS_IN_BYTE), (uint32_t)BITS_IN_A_WORD);
        LOG_ERROR("Encryption error !!!");
    }

    /* Prepare the user data to be encrypted, and add any padding that may be needed */
    InitializeInputDataStream(inputData, inputDataByteLength, isFPGAData);


    /* Now that we know the final data length of the encrypted data stream, add that length
    to the stream header */
    uint32_t encryptedStreamByteLength = BYTES_PER_IPAD +
        input_data_ByteLength +
        BYTES_PER_PAD1 +
        BYTES_PER_2048_GAP +
        BYTES_PER_OPAD +
        BYTES_PER_FIRST_DIGEST +
        BYTES_PER_PAD2 +
        BYTES_PER_HMAC_DIGEST;

    SetDataStreamHeader(keySource, encryptedStreamByteLength, presBitHdr, presBitHdrSize);


    /* Initalize the iPad data
    Ipad data is 512 bits long, with each filled with a 0x36, and the last 256
    bit XOR'd with the HMAC key */
    InitializeHMAC_ioPadData(HMAC_IPAD_BYTE, ipad_data);

    /* Default the 2048 bit gap to zero.
    This is to ensure that the first digest round (of the iPad and input data) is
    completed when the HW is calculating it */
    memset(gap_2048, 0, BYTES_PER_2048_GAP);

    /* Initalize the oPad data
    Ipad data is 512 bits long, with each filled with a 0x5C, and the last 256
    bit XOR'd with the HMAC key */
    InitializeHMAC_ioPadData(HMAC_OPAD_BYTE, opad_data);

    /* Default the first digest gap to zero
    This gap repersents the HMAC digest of the iPad and input data stream, and will be
    replaced by the HW with the HW calculated first digest value */
    memset(first_digest_gap, 0, BYTES_PER_FIRST_DIGEST);

    /* Initalize the pad1 data.
    It contains the bit length (from the byte values given here) of the iPad and input
    data stream */
    InitializePad1Data(BYTES_PER_IPAD + input_data_ByteLength);

    /* Calculate the first digest.
    This is the HMAC digest of the iPad, input data stream, and pad1. This is used when
    the final digest is calculated at the same place as were the first_digest_gap is */

    // Start off with the IPAD data.
    ComputeHMACDigest(ipad_data, BYTES_PER_IPAD, true, calculated_first_digest);

    // Add in the user data.
    ComputeHMACDigest(input_data_Buffer, input_data_ByteLength, false, calculated_first_digest);

    // Finally add in the PAD1 data.
    ComputeHMACDigest(pad1_data, BYTES_PER_PAD1, false, calculated_first_digest);

    /* Initalize the pad2 data.
    It contains the bit length (from the byte values given here) of the oPad and first
    digest gap. */
    InitializePad2Data(BYTES_PER_OPAD + BYTES_PER_FIRST_DIGEST);

    /* Calculate the final HMAC digest.
    This is the HMAC digest of the oPad, input data stream, and pad2. This is by the HW
    to compare against to authenticate the entire data stream.
    Note:
    Also, a bit of a hack here. Both the first digest and pad2 are only 265 bits long, but
    for HMAC, the smallest unit can only 512 bits. So we double the size of the calculated
    digest field, and copy the pad2 to the second half so that the digest and pad2 can be HMAC'd
    together. */
    ComputeHMACDigest(opad_data, BYTES_PER_OPAD, true, HMAC_digest);

    memcpy(calculated_first_digest + BYTES_PER_FIRST_DIGEST, pad2_data, BYTES_PER_PAD2);

    ComputeHMACDigest(calculated_first_digest, BYTES_PER_FIRST_DIGEST + BYTES_PER_PAD2, false, HMAC_digest);

    /* Create a new buffer that contains the entire data stream, plus 16 NOP commands at the end,
    and return it */
    encryptedDataByteLength = encryptedStreamByteLength + streamHdrSize + (16 * sizeof(BIT_STREAM_NOOP_COMMAND));

    /* Partitions must have the length 32 byte aligned because of a HW bug in the PCAP DMA engine. */
    uint32_t dataStream32ByteMisalignment = 32 - encryptedDataByteLength % 32;

    /* Get a new buffer for the encrypted data and overhead */
    encryptedData = new uint8_t[encryptedDataByteLength + dataStream32ByteMisalignment];
    if (encryptedData == NULL)
    {
        LOG_DEBUG(DEBUG_STAMP, "Out of memory");
        LOG_ERROR("Ercryption error !!!");
    }
    uint8_t* ptr = encryptedData;

    /* Copy the data stream header to the return */
    memcpy(ptr, stream_header, streamHdrSize);
    ptr += streamHdrSize;

    /* Encrypt the entire data stream from the iPad to the HMAC digest into the return buffer */
    EncryptDataStream(ptr);
    ptr += encryptedStreamByteLength;

    /* Add 16 NOP commands to the end of the encrypted stream to ensure that the HMAC engine finishes
    verifiy the HMAC digest before the user checks the HMAC_Error bit.
    Also, add in any NOOPs needed to align the encrypted data size to 32 bytes */
    uint32_t NOOPLimit = 16 + (dataStream32ByteMisalignment / sizeof(uint32_t));

    for (uint32_t index = 0; index < NOOPLimit; index += 1)
    {
        //WriteLittleEndian32(ptr, BIT_STREAM_NOOP_COMMAND);
        dataMover->WriteData32(ptr, BIT_STREAM_NOOP_COMMAND); // DataMovement
        ptr += sizeof(BIT_STREAM_NOOP_COMMAND);
    }
}
