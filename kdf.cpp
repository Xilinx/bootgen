// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
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

#include "kdf.h"
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <sstream>
#include <vector>
#include <cstring>
using namespace std;

/************************************************************************
Example of NIST SP 800-108 in Counter Mode
i is chosen to be 32 bits. This allows the 128(2^32 -1) bits of keying material can be generated.
L will be 40 bits which allows for the full amount of i. So Ko of 128(2^32 -1) bits of data can
be and is limited by the size of i.

If Ko is being used to generate both the 256-bit AES key and the 96-bit IV for AES-GCM, then
this allows for about 1.56 x 10^9 key/IV pairs. Looking at worst case scenarios,
key rolling every 32 AES encryption blocks would allow us to encrypt ~800 GB of data with a
single root key.

Because the CMAC implementation provided only handles even 128-bit blocks,
the length of [i]2 || Label || 0x00 || Context || [L]2 must always be
evenly divisible by 128 bits. The CMAC implementation needs fixed so the customer can
provide any Context and Label. Just as an example, these values are being used

i = 32 bits
Label = 256 bits
Separator = 8 bits
Context = 48 bits
L = 40 bits

These are just arbitrary values. Really anything can be used and anything should be
permitted for the Context and Label. The spec defines the separator. The i and L lengths
are defined to sensible values for our implementation.
divisible by 128.

The same cmac key is used as the earlier cmac example. This would be the customer provided
secret seed or root key.
*************************************************************************/

#define VERBOSE_OUT_KDF kdfLogFile

/* Read a big endian 32 bit word from a byte stream. */
#define read_big_endian_32( addr ) \
    ( ( (uint32_t)( *( (uint8_t *)(addr) + 0 ) ) << 24 ) | \
      ( (uint32_t)( *( (uint8_t *)(addr) + 1 ) ) << 16 ) | \
      ( (uint32_t)( *( (uint8_t *)(addr) + 2 ) ) << 8 ) |  \
        (uint32_t)( *( (uint8_t *)(addr) + 3 ) ) )

/* Read a little endian 32 bit word from a byte stream. */
#define read_little_endian_32( addr ) \
    ( ( (uint32_t)( *( (uint8_t *)(addr) + 3 ) ) << 24 ) | \
      ( (uint32_t)( *( (uint8_t *)(addr) + 2 ) ) << 16 ) | \
      ( (uint32_t)( *( (uint8_t *)(addr) + 1 ) ) << 8 ) |  \
        (uint32_t)( *( (uint8_t *)(addr) + 0 ) ) )

/* Write a big endian 32 bit word to a byte stream. */
#define write_big_endian_32( addr, value ) \
    *( (uint8_t *)(addr) + 0 ) = ( ( (value) >> 24 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 1 ) = ( ( (value) >> 16 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 2 ) = ( ( (value) >> 8 ) & 0xFF );  \
    *( (uint8_t *)(addr) + 3 ) =   ( (value) & 0xFF );


static const uint8_t sbox[256] = {
    //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

static const uint8_t rsbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

static const uint8_t Rcon[11] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };

uint8_t expanded_key[240];

uint8_t cmac_key[32];
uint8_t K1[16];
uint8_t K2[16];

/******************************************************************************/
static void memcpy_be(uint8_t* dst, const uint32_t* src, size_t size)
{
    uint32_t value = 0;
    uint8_t *addr;
    for (uint32_t i = 0; i<size / 4; i++)
    {
        addr = dst + (4 * i);
        value = *(src + i);
        write_big_endian_32(addr, value);
    }
}

/******************************************************************************/
void copy_array_uint64_t(uint64_t *dst, uint64_t *src, int length)
{
    int index;
    for (index = 0; index<length; index++)
    {
        dst[index] = src[index];
    }
}

/******************************************************************************/
void copy_array_uint8_t(uint8_t *dst, uint8_t *src, int size)
{
    int index;

    for (index = 0; index<size; index++)
    {
        dst[index] = src[index];
    }
}

/******************************************************************************/
void copy_uint8_t_uint64_t(uint64_t *dst, uint8_t *src, int size)
{
    int index8;
    int index64;

    index64 = 0;
    for (index8 = 0; index8<size; index8++)
    {
        if (index8 % 8 == 0)
        {
            dst[index64] = 0;
        }
        dst[index64] = (dst[index64] << 8) | src[index8];
        if (index8 % 8 == 7)
        {
            index64++;
        }
    }
}

/******************************************************************************/
void xor_array_uint64_t(uint64_t *dst, uint64_t *src, int size)
{
    int index;

    /* Xor the source onto the destination */
    for (index = 0; index<size; index++)
    {
        dst[index] ^= src[index];
    }
}

/******************************************************************************/
void xor_array_uint8_t(uint8_t *dst, uint8_t *src, int size)
{
    int index;

    /* Xor the source onto the destination */
    for (index = 0; index<size; index++)
    {
        dst[index] ^= src[index];
    }
}

/******************************************************************************/
void add_array_uint64_t(uint64_t *dst, uint64_t *src, int length)
{
    int index;

    for (index = 0; index<length; index++)
    {
        dst[index] += src[index];
    }
}

/******************************************************************************/
void str_2_uint8_t(uint8_t *dst, char *str, int char_count)
{
    int char_index;
    int byte_index;
    uint8_t nibble;

    byte_index = 0;
    for (char_index = 0; char_index<char_count; char_index++)
    {
        /* Initialize the byte */
        if (char_index % 2 == 0)
        {
            dst[byte_index] = 0;
        }

        /* Calculate the nibble */
        if ((str[char_index] >= '0') && (str[char_index] <= '9'))
        {
            nibble = (str[char_index] - '0');
        }
        else if ((str[char_index] >= 'a') && (str[char_index] <= 'f'))
        {
            nibble = (str[char_index] - 'a' + 10);
        }
        else if ((str[char_index] >= 'A') && (str[char_index] <= 'F'))
        {
            nibble = (str[char_index] - 'F' + 10);
        }
        else
        {
            nibble = 0;
        }

        /* Shift in the nibble */
        dst[byte_index] = (dst[byte_index] << 4) | nibble;

        /* Increment the byte pointer as needed */
        if (char_index % 2 == 1)
        {
            byte_index++;
        }
    }
}

/******************************************************************************/
void uint64_t_2_str(char *str, uint64_t *src, int word_count)
{
    int word_index;
    int nibble_count;
    int  mychar;

    for (word_index = 0; word_index<word_count; word_index++)
    {
        for (nibble_count = 0; nibble_count<16; nibble_count++)
        {
            mychar = (int)(src[word_index] >> (60 - 4 * (nibble_count))) & 0xF;
#ifdef __GNUC__
            sprintf(&str[16 * word_index + nibble_count], "%x", mychar);
#else
            sprintf_s(&str[16 * word_index + nibble_count], 64, "%x", mychar);
#endif
        }
    }
}

/******************************************************************************/
static void rotate_word(uint8_t *t, int size, int stride, int offset)
{
    uint8_t temp;
    int index;
    int shift_count;

    /* Count how many shifts happen */
    for (shift_count = 0; shift_count<offset; shift_count++)
    {
        /* Rotate the word */
        temp = t[0];
        for (index = 1; index<size; index++)
        {
            t[stride*(index - 1)] = t[stride*index];
        }
        t[stride*(size - 1)] = temp;
    }
}

/******************************************************************************/
static void sbox_word(uint8_t *t, int size)
{
    int index;

    /* Apply sbox to all the bytes */
    for (index = 0; index<size; index++)
    {
        t[index] = sbox[t[index]];
    }
}

/******************************************************************************/
static void expand_key(uint8_t *k)
{
    int word_index, byte_index, iteration;

    /* Copy the 1st 8 words */
    for (word_index = 0; word_index<8; word_index++)
    {
        for (byte_index = 0; byte_index<4; byte_index++)
        {
            expanded_key[4 * word_index + byte_index] = k[4 * word_index + byte_index];
        }
    }

    /* Expand the rest of the key */
    iteration = 1;
    for (; word_index<60; word_index++)
    {
        /* Assign the previous four bytes to the next 4 bytes */
        copy_array_uint8_t(&expanded_key[4 * word_index], &expanded_key[4 * (word_index - 1)], 4);

        /* For the 1st word of this 8 word portion of the expanded key */
        if (word_index % 8 == 0)
        {
            rotate_word(&expanded_key[4 * word_index], 4, 1, 1);
        }

        /* For the 1st word and 4th word of this 8 word portion do substition */
        if ((word_index % 8 == 0) || (word_index % 8 == 4))
        {
            sbox_word(&expanded_key[4 * word_index], 4);
        }

        /* For the 1st word of this 8 word portion of the expanded key */
        if (word_index % 8 == 0)
        {
            expanded_key[4 * word_index] ^= Rcon[iteration];
            iteration++;
        }

        /* Xor these expanded key bytes with earlier portions */
        xor_array_uint8_t(&expanded_key[4 * word_index], &expanded_key[4 * (word_index - 8)], 4);
    }
}

/******************************************************************************/
static void add_round_key(uint8_t *state, int rnd)
{
    xor_array_uint8_t(state, &expanded_key[16 * rnd], 16);
}

/******************************************************************************/
static void sub_bytes(uint8_t *state)
{
    sbox_word(state, 16);
}

/******************************************************************************/
static void shift_rows(uint8_t *state)
{
    rotate_word(&state[1], 4, 4, 1);
    rotate_word(&state[2], 4, 4, 2);
    rotate_word(&state[3], 4, 4, 3);
}

/******************************************************************************/
static void mix_column(uint8_t *state)
{
    uint8_t a[4];
    uint8_t b[4];
    uint8_t index;
    uint8_t h;

    /* The array 'a' is simply a copy of the input array 'r'
    * The array 'b' is each element of the array 'a' multiplied by 2
    * in Rijndael's Galois field
    * a[n] ^ b[n] is element n multiplied by 3 in Rijndael's Galois field */
    for (index = 0; index<4; index++)
    {
        a[index] = state[index];
        h = (0x80 & state[index]) ? 0xff : 0x00;
        b[index] = state[index] << 1;
        b[index] ^= 0x1b & h;
    }
    state[0] = b[0] ^ a[3] ^ a[2] ^ b[1] ^ a[1]; /* 2 * a0 + a3 + a2 + 3 * a1 */
    state[1] = b[1] ^ a[0] ^ a[3] ^ b[2] ^ a[2]; /* 2 * a1 + a0 + a3 + 3 * a2 */
    state[2] = b[2] ^ a[1] ^ a[0] ^ b[3] ^ a[3]; /* 2 * a2 + a1 + a0 + 3 * a3 */
    state[3] = b[3] ^ a[2] ^ a[1] ^ b[0] ^ a[0]; /* 2 * a3 + a2 + a1 + 3 * a0 */
}

/******************************************************************************/
static void mix_columns(uint8_t *state)
{
    int column;

    for (column = 0; column<4; column++)
    {
        mix_column(&state[4 * column]);
    }
}

/******************************************************************************/
static void block_cipher(uint8_t *pt, uint8_t *ct)
{
    int rnd;

    copy_array_uint8_t(ct, pt, 16);
    add_round_key(ct, 0);
    for (rnd = 1; rnd<14; rnd++)
    {
        sub_bytes(ct);
        shift_rows(ct);
        mix_columns(ct);
        add_round_key(ct, rnd);
    }
    sub_bytes(ct);
    shift_rows(ct);
    add_round_key(ct, rnd);
}

/******************************************************************************/
void ecb_encrypt(uint8_t *key, uint8_t *pt, uint8_t *ct, int blocks)
{
    int block_count;

    expand_key(key);

    for (block_count = 0; block_count<blocks; block_count++)
    {
        block_cipher(&pt[16 * block_count], &ct[16 * block_count]);
    }
}

/******************************************************************************/
void cbc_encrypt(uint8_t *iv, uint8_t *key, uint8_t *pt, uint8_t *ct, int blocks)
{
    int block_count;
    uint8_t cipher_input[16];

    expand_key(key);
    copy_array_uint8_t(cipher_input, iv, 16);

    for (block_count = 0; block_count<blocks; block_count++)
    {
        xor_array_uint8_t(cipher_input, &pt[16 * block_count], 16);
        block_cipher(cipher_input, &ct[16 * block_count]);
        copy_array_uint8_t(cipher_input, &ct[16 * block_count], 16);
    }
}

/******************************************************************************/
void gen_cmac_subkeys(uint8_t *key)
{
    uint8_t pt[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0 };
    uint8_t Rb[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0x87 };
    uint8_t L[16];
    int i;

    /* NIST SP 800-38B 6.1 Step 1 */
    copy_array_uint8_t(cmac_key, key, 32);
    ecb_encrypt(cmac_key, pt, L, 1);

    /* NIST SP 800-38B 6.1 Step 2 */
    for (i = 0; i<15; i = i + 1)
    {
        K1[i] = (L[i] << 1) | (L[i + 1] >> 7);
    }
    K1[15] = L[i] << 1;
    if ((L[0] & 0x80) != 0)
    {
        for (i = 0; i<16; i = i + 1)
        {
            K1[i] ^= Rb[i];
        }
    }

    /* NIST SP 800-38B 6.1 Step 3 */
    for (i = 0; i<15; i = i + 1)
    {
        K2[i] = (K1[i] << 1) | (K1[i + 1] >> 7);
    }
    K2[15] = K1[i] << 1;
    if ((K1[0] & 0x80) != 0)
    {
        for (i = 0; i<16; i = i + 1)
        {
            K2[i] ^= Rb[i];
        }
    }
}

/******************************************************************************/
void cmac(uint8_t *mac, uint8_t *msg, int total_bytes)
{
    int block;
    uint8_t ct[16];
    uint8_t pt[16];

    int lastBlockSize = total_bytes - ((total_bytes / 16) * 16);
    int total_blocks = (total_bytes / 16) + ((total_bytes % 16) == 0 ? 0 : 1);

    for (block = 0; block < total_blocks; block++)
    {
        copy_array_uint8_t(pt, &msg[16 * block], 16);
        if (block == total_blocks - 1)
        {
            if (lastBlockSize == 0)
            {
                xor_array_uint8_t(pt, K1, 16);
            }
            else
            {
                pt[lastBlockSize++] = 0x80;
                while (lastBlockSize & 15)
                    pt[lastBlockSize++] = 0x00;
                xor_array_uint8_t(pt, K2, 16);
            }
        }
        if (block != 0)
        {
            xor_array_uint8_t(pt, ct, 16);
        }
        ecb_encrypt(cmac_key, pt, ct, 1);
    }
    copy_array_uint8_t(mac, ct, 16);
}

/*******************************************************************************************************************/
static void pack_hex(const std::string& hex_string, uint8_t* hex_data)
{
    uint32_t hex_string_length = (uint32_t)hex_string.size();

    if (hex_string_length & 1)
    {
        std::cout << "Hex String - " << hex_string.c_str() << " - does not have even no. of hex digits" << std::endl;
        exit(KDF_CAVP_HEXKEY_IMPROPER);
    }

    for (uint32_t i = 0; i < hex_string_length; i += 2)
    {
        std::string byte = hex_string.substr(i, 2);
        if (!isxdigit(byte[0]) || !isxdigit(byte[1]))
        {
            std::cout << "Hex String - " << hex_string.c_str() << " - is has a non hex digit" << std::endl;
            exit(KDF_CAVP_HEXKEY_IMPROPER);
        }
        *hex_data++ = (uint8_t)strtoul(byte.c_str(), NULL, 16);
    }
}

/******************************************************************************/
void Kdf::SetKdfLogFilename(std::string filename)
{
    kdf_log_file = filename;
}

/******************************************************************************/
std::string Kdf::GetKdfLogFilename(void)
{
    return kdf_log_file;
}

/******************************************************************************/
uint32_t Kdf::GetVersion(void)
{
    return VERSION;
}

/******************************************************************************/
uint32_t Kdf::CounterModeKDF(uint32_t* k_in, uint32_t* fid, uint32_t fid_byte_length, uint32_t* ko_buf, uint32_t ko_bytes, bool reset_cntr)
{
    if (fid_byte_length == 0)
    {
        std::cout << "Fixed Input Data is not available. Seed and Fixed Input Data are mandatory for KDF." << std::endl;
        return KDF_INPUT_FID_UNAVAILABLE;
    }

    if (k_in == NULL)
    {
        std::cout << "Seed is not available. Seed and Fixed Input Data are mandatory for KDF." << std::endl;
        return KDF_INPUT_SEED_UNAVAILABLE;
    }

    if((fixed_input_data_byte_length != fid_byte_length) || (fixed_input_data == NULL))
    {
        fixed_input_data_byte_length = fid_byte_length;
        delete[] fixed_input_data;
        fixed_input_data = new uint8_t[fixed_input_data_byte_length];
    }
    memcpy_be(fixed_input_data, fid, fixed_input_data_byte_length);

    /* This KDF accepts a seed of fixed length - 32bytes */
    if (key_seed == NULL)
    {
        key_seed = new uint8_t[BYTES_PER_AES_SEED];
    }
    memcpy_be(key_seed, k_in, BYTES_PER_AES_SEED);

    /* An example generating 4 key/IV pairs will have 4*(32+12) number_of_Ko_bytes.
    This should be flexible based on number of keys or key/iv pair required
    or it can just pull off key/IV as needed; it is acceptable to discard
    32-bits and use 384 bits of Ko per 352-bits of key/iv pair. */

    KDF(ko_bytes, reset_cntr);

    for (uint32_t index = 0; index < (ko_bytes/sizeof(uint32_t)); index++)
    {
        ko_buf[index] = read_little_endian_32( (key_out + (index * sizeof(uint32_t))) );
    }

    return 0;
}

/******************************************************************************/
uint32_t Kdf::ParseKDFTestVectorFile(std::string filename)
{
    std::cout << "Reading the Counter Mode KDF test file" << std::endl;
    std::ifstream testFile(filename.c_str());
    if (!testFile)
    {
        std::cout << "Failure reading Counter Mode KDF test file - " << filename.c_str() << std::endl;
        return KDF_CAVP_TESTFILE_UNAVAILABLE;
    }
    
    while (testFile)
    {
        std::string word;
        testFile >> word;
        if (word == "")
        {
            return 0;
        }

        if (word == "L")
        {
            testFile >> word;
            if (word == "=")
            {
                testFile >> word;
                key_out_length = (uint32_t)strtoul(word.c_str(), NULL, 10);
            }
            else
            {
                std::cout << "Error parsing KDF test vector file - " << filename.c_str() << "\n\t Expected '=''." << std::endl;
                return KDF_CAVP_TESTFILE_IMPROPER;
            }
        }
        else if (word == "KI")
        {
            testFile >> word;
            if (word == "=")
            {
                testFile >> word;
                if (word.size() != BYTES_PER_AES_SEED * 2)
                {
                    std::cout << "KI must be 256 bits long - " << word.c_str() << std::endl;
                    exit(KDF_CAVP_KI_LENGTH_IMPROPER);
                }
                key_seed = new uint8_t[(uint32_t)word.size()];
                pack_hex(word, key_seed);
            }
            else
            {
                std::cout << "Error parsing KDF test vector file - " << filename.c_str() << "\n\t Expected '=''." << std::endl;
                return KDF_CAVP_TESTFILE_IMPROPER;
            }
        }
        else if (word == "FixedInputDataByteLen")
        {
            testFile >> word;
            if (word == "=")
            {
                testFile >> word;
                fixed_input_data_byte_length = (uint32_t)strtoul(word.c_str(), NULL, 10);
            }
            else
            {
                std::cout << "Error parsing KDF test vector file - " << filename.c_str() << "\n\t Expected '=''." << std::endl;
                return KDF_CAVP_TESTFILE_IMPROPER;
            }
        }
        else if (word == "FixedInputData")
        {
            testFile >> word;
            if (word == "=")
            {
                testFile >> word;
                fixed_input_data = new uint8_t[fixed_input_data_byte_length];
                pack_hex(word, fixed_input_data);
            }
            else
            {
                std::cout << "Error parsing KDF test vector file - " << filename.c_str() << "\n\t Expected '=''." << std::endl;
                return KDF_CAVP_TESTFILE_IMPROPER;
            }
        }
        else if (word == "KO")
        {
            testFile >> word;
            if (word == "=")
            {
                testFile >> word;
                verify_ko = new uint8_t[key_out_length / 8];
                pack_hex(word, verify_ko);
            }
            else
            {
                std::cout << "Error parsing KDF test vector file - " << filename.c_str() << "\n\t Expected '=''." << std::endl;
                return KDF_CAVP_TESTFILE_IMPROPER;
            }
        }
        else
        {
            std::cout << "Error parsing KDF test vector file - " << filename.c_str() << "\n\t   Expected 'L','KI','FixedInputDataByteLen', and 'FixedInputData'." << std::endl;
            return KDF_CAVP_TESTFILE_IMPROPER;
        }
    }
    return 0;
}

/******************************************************************************/
uint32_t Kdf::CAVPonCounterModeKDF(std::string filename)
{
    uint32_t ret_value = ParseKDFTestVectorFile(filename);
    if (ret_value != 0)
    {
        return ret_value;
    }

    bool verified = false;
    std::cout << "Generating Ko using Counter-Mode KDF..." << std::endl;
    std::cout << "KO = ";
    uint32_t number_of_Ko_bytes = key_out_length / 8;

    KDF(number_of_Ko_bytes);

    for (uint32_t y = 0; y < number_of_Ko_bytes; y++)
    {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << uint32_t(key_out[y]);
    }
    std::cout << std::endl << std::endl;
    
    if (verify_ko != NULL)
    {
        if (memcmp(key_out, verify_ko, number_of_Ko_bytes) == 0)
        {
            std::cout << "Verified with given Ko" << std::endl;
            verified = true;
        }
        else
        {
            std::cout << "Failed to verify with given Ko" << std::endl;
        }
    }

    std::cout << std::endl;
    if (verified)
    {
        return 0;
    }
    else
    {
        return KDF_CAVP_VERIFY_KO_FAILED;
    }
}

/******************************************************************************/
void Kdf::ResetKdfCounter()
{
    for(int32_t i = 0; i < KDF_COUNTER_BYTES; ++i)
    {
        kdf_counter[i] = 0x00;
    }
}

/******************************************************************************/
bool Kdf::IsKdfCounterMax()
{
    for(int32_t i = 0; i < KDF_COUNTER_BYTES; ++i)
    {
        if(kdf_counter[i] != 0xFF)
        {
            return false;
        }
    }
    /* This function should practically never return 'true'
       since the num of keys generated by KDF will always be less than 2^32 keys.i */
    return true;
}

/******************************************************************************/
void Kdf::KDF(uint32_t ko_bytes, bool reset_cntr)
{
    static std::ofstream kdfLogFile;

    if (kdf_log_file != "")
    {
        kdfLogFile.open(kdf_log_file.c_str(), std::fstream::app);
        VERBOSE_OUT_KDF << std::endl << "------------------------------------";
        VERBOSE_OUT_KDF << std::endl << " Generating Ko of " << ko_bytes << " bytes";
        VERBOSE_OUT_KDF << std::endl << "------------------------------------" << std::endl;
    }

    if (reset_cntr || IsKdfCounterMax()) 
    {
        ResetKdfCounter();
    }

    uint64_t number_of_input_bytes = KDF_COUNTER_BYTES + fixed_input_data_byte_length;
    uint8_t* input = new uint8_t[number_of_input_bytes];

    if((ko_bytes != key_out_length) || (key_out == NULL)) 
    {
        key_out_length = ko_bytes;
        delete[] key_out;
        key_out = new uint8_t[key_out_length];
    }

    gen_cmac_subkeys(key_seed);

    copy_array_uint8_t(&input[0], kdf_counter, KDF_COUNTER_BYTES);
    copy_array_uint8_t(&input[KDF_COUNTER_BYTES], fixed_input_data, fixed_input_data_byte_length);

    uint32_t cmac_iterations = ko_bytes / 16;
    cmac_iterations += ((ko_bytes % 16) == 0 ? 0 : 1);

    /* Run the KDF for the amount of data needed */
    uint32_t x;
    int32_t y;
    for (x = 0; x < cmac_iterations; x++)
    {
        /* Increment kdf_counter. The first value used is 1 */
        y = 3;
        do {
            kdf_counter[y]++;
            y--;
        } while (kdf_counter[y + 1] == 0 && y >= 0);
        copy_array_uint8_t(&input[0], kdf_counter, KDF_COUNTER_BYTES);

        /* Run the PRF */
        if (ko_bytes % 16 != 0 && x == cmac_iterations - 1)
        {
            uint8_t* last_ko = new uint8_t[16];
            cmac(&last_ko[0], input, number_of_input_bytes);
            memcpy(&key_out[16 * x], &last_ko[0], (ko_bytes - (16 * x)));
            if (kdf_log_file != "")
            {
                for (y = 0; y < 16; y++)
                {
                    VERBOSE_OUT_KDF << std::setfill('0') << std::setw(2) << std::hex << uint32_t(last_ko[y]);
                }
                VERBOSE_OUT_KDF << std::endl;
            }
            delete[] last_ko;
        }
        else
        {
            cmac(&key_out[16 * x], input, number_of_input_bytes);
            if (kdf_log_file != "")
            {
                for (y = 0; y < 16; y++)
                {
                    VERBOSE_OUT_KDF << std::setfill('0') << std::setw(2) << std::hex << uint32_t(key_out[16 * x + y]);
                }
                VERBOSE_OUT_KDF << std::endl;
            }
        }
    }

    if (kdf_log_file != "")
    {
        kdfLogFile.close();
    }

    delete[] input;
}
