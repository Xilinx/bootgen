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

#pragma once

#ifndef _ENCRYPTUTILS_H_
#define _ENCRYPTUTILS_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <string>

#include "encryption.h"


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define BYTES_PER_STREAM_HEADER         140
#define BYTES_PER_FPGA_STREAM_HEADER    108
#define BYTES_PER_IPAD                  64
#define BYTES_PER_PAD1                  64
#define BYTES_PER_2048_GAP              256
#define BYTES_PER_OPAD                  64
#define BYTES_PER_FIRST_DIGEST          32
#define BYTES_PER_PAD2                  32
#define BYTES_PER_HMAC_DIGEST           32

#define AES_GCM_IV_SZ                   12
#define AES_GCM_KEY_SZ                  32
#define WORDS_PER_AES_BLOCK             4
#define WORDS_PER_AES_KEY               8
#define SECURE_HDR_SZ                   48
#define AES_GCM_TAG_SZ                  16
#define NUM_BYTES_PER_WORD              4
#define VERBOSE_FILE                    1

#ifdef VERBOSE_FILE
#define VERBOSE_OUT options.aesLogFile
#define VERBOSE_OUT_MH bi.options.aesLogFile
#else
#define VERBOSE_OUT std::cout
#define VERBOSE_OUT_MH std::cout
#endif


static const uint32_t defaultStreamHeader [BYTES_PER_STREAM_HEADER / sizeof(uint32_t)] =
{
    0xFFFFFFFF,     //   0  Dummy Word
    0xFFFFFFFF,     //   4  Dummy Word
    0x000000BB,     //   8  BusWidth Word
    0x11220044,     //  12  32/08/16 bits
    0xFFFFFFFF,     //  16  Dummy Word
    0xFFFFFFFF,     //  20  Dummy Word
    0xAA995566,     //  24  Sync Word
    0x20000000,     //  28  Type1 NO OP
    0x3000C001,     //  32  Type1 write 1 words to MASK
    0x00000040,     //  36  data word 1
    0x3000A001,     //  40  Type1 write 1 words to CTL
    0x00000040,     //  44  dec
    0x20000000,     //  48  Type1 NO OP
    0x20000000,     //  52  Type1 NO OP
    0x20000000,     //  56  Type1 NO OP
    0x20000000,     //  60  Type1 NO OP
    0x20000000,     //  64  Type1 NO OP
    0x20000000,     //  68  Type1 NO OP
    0x20000000,     //  72  Type1 NO OP
    0x20000000,     //  76  Type1 NO OP
    0x20000000,     //  80  Type1 NO OP
    0x20000000,     //  84  Type1 NO OP
    0x20000000,     //  88  Type1 NO OP
    0x20000000,     //  92  Type1 NO OP
    0x20000000,     //  96  Type1 NO OP
    0x20000000,     // 100  Type1 NO OP
    0x20000000,     // 104  Type1 NO OP
    0x20000000,     // 108  Type1 NO OP
    0x30016004,     // 112  Type1 write 4 words to CBC
    0x00000000,     // 116  data word 1
    0x00000000,     // 120  data word 2
    0x00000000,     // 124  data word 3
    0x00000000,     // 128  data word 4
    0x30034001,     // 132  Type1 write 1 words to DWC
    0x00000000      // 136  data word 1
};

static const uint32_t defaultFpgaStreamHeader [BYTES_PER_FPGA_STREAM_HEADER / sizeof(uint32_t)] =
{
    0x3000C001,     //   0  Type1 write 1 words to MASK
    0x00000040,     //   4  data word 1
    0x3000A001,     //   8  Type1 write 1 words to CTL
    0x00000040,     //  12  dec
    0x3001C001,     //  16  Type1 NO OP
    0x00000000,     //  20  Type1 NO OP
    0x20000000,     //  24  Type1 NO OP
    0x20000000,     //  28  Type1 NO OP
    0x20000000,     //  32  Type1 NO OP
    0x20000000,     //  36  Type1 NO OP
    0x20000000,     //  40  Type1 NO OP
    0x20000000,     //  44  Type1 NO OP
    0x20000000,     //  48  Type1 NO OP
    0x20000000,     //  52  Type1 NO OP
    0x20000000,     //  56  Type1 NO OP
    0x20000000,     //  60  Type1 NO OP
    0x20000000,     //  64  Type1 NO OP
    0x20000000,     //  68  Type1 NO OP
    0x20000000,     //  72  Type1 NO OP
    0x20000000,     //  76  Type1 NO OP
    0x30016004,     //  80  Type1 write 4 words to CBC
    0x00000000,     //  84  data word 1
    0x00000000,     //  88  data word 2
    0x00000000,     //  92  data word 3
    0x00000000,     //  96  data word 4
    0x30034001,     // 100  Type1 write 1 words to DWC
    0x00000000      // 104  data word 1
};

static const uint32_t defaultPcapHeader[] =
{
    0x3001E000,     //  56  Type1 write of 0 words to PCAP reg
    0x50000000      //  60  Type2 write of length word to PCAP reg
};


void memcpy_be(uint8_t* dst, const uint32_t* src, size_t size);
uint32_t reverseDataBits(uint32_t value);
void reverse_copy(uint8_t* dst, uint64_t value, size_t size);

#endif
