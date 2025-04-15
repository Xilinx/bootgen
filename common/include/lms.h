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
#pragma once

#ifndef _LMS_H_
#define _LMS_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <string>
#include <list>
#include <string.h>
#include "bootgenenum.h"
#include "authkeys-versal.h"
#include "systemutils.h"
#include "authentication.h"
//#include "bifoptions.h"

/* Forward class references */
class AuthenticationCertificate;
class Key;

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/

/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/

/* One Time Signatures (OTS) */
enum LmOtsType {
	LMOTS_RESERVED			= 0x0,
	LMOTS_SHA256_N32_W2		= 0x2,
	LMOTS_SHA256_N32_W4		= 0x3,
	LMOTS_SHA256_N32_W8		= 0x4,
	LMOTS_SHAKE256_N32_W2	= 0xa,
	LMOTS_SHAKE256_N32_W4	= 0xb,
	LMOTS_SHAKE256_N32_W8	= 0xc
};

typedef struct
{
	uint8_t value[32];
} bytestring32;

typedef struct  
{
	uint32_t type;
	bytestring32 C;
	bytestring32 y[265];
} LmOtsSignature_n32p265;

typedef struct
{
	uint32_t type;
	bytestring32 C;
	bytestring32 y[133];
} LmOtsSignature_n32p133;

typedef struct
{
	uint32_t type;
	bytestring32 C;
	bytestring32 y[67];
} LmOtsSignature_n32p67;

typedef struct
{
	uint32_t type;
	bytestring32 C;
	bytestring32 y[34];
} LmOtsSignature_n32p34;

typedef union
{
	//LmOtsSignature_n32p265 sig_n32_p265;
	LmOtsSignature_n32p133 sig_n32_p133;
	LmOtsSignature_n32p67  sig_n32_p67;
	LmOtsSignature_n32p34  sig_n32_p34;
} LmOtsSignature;


/* Hash-Based Signatures (HBS) */

enum LmsType {
	LMS_RESERVED		= 0x0,
	LMS_SHA256_N32_H5	= 0x5,
	LMS_SHA256_N32_H10	= 0x6,
	LMS_SHA256_N32_H15	= 0x7,
	LMS_SHA256_N32_H20	= 0x8,
	LMS_SHAKE256_N32_H5 = 0xf,
	LMS_SHAKE256_N32_H10 = 0x10,
	LMS_SHAKE256_N32_H15 = 0x11,
	LMS_SHAKE256_N32_H20 = 0x12,
};


/* Leighton-Micali Signatures (LMS) */

typedef union 
{
	bytestring32 path_n32_h5[5];
	bytestring32 path_n32_h10[10];
	bytestring32 path_n32_h15[15];
	bytestring32 path_n32_h20[20];
} LmsPath;

typedef struct  
{
	uint32_t q;
	LmOtsSignature lmOtsSign;
	LmsType lmType;
	LmsPath nodes;
} LmsSignature;

typedef struct {
	LmsType lmType;
	LmOtsType otsType;
	uint8_t I[16];
	uint8_t K[32];
} LmsPublicKey;



/* Hierarchical Signature System (HSS) */

typedef struct {
	uint32_t numLevels;
	LmsPublicKey publicKey;
} HssPublicKey;

typedef struct {
	uint8_t numSigDone[8];
	uint8_t compressedDataSet[8];
	uint8_t seed[32];
	uint8_t I[16];
} HssPrivateKey;

inline int GetLmsPublicKeyLength(const char *keyname, bool lmsOnly)
{
    int hss_public_key_size = sizeof(HssPublicKey); //hss_get_public_key_len();

    /*Reducing by 4 bytes, to make 1 Level HSS Sign to LMS Sign*/
    if(lmsOnly)
        return hss_public_key_size - 4;
    else
        return hss_public_key_size;
}
#endif