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
#ifndef _AUTHENTICATION_ZYNQMP_H_
#define _AUTHENTICATION_ZYNQMP_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "authentication.h"

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define AUTH_HDR_ZYNQMP                 0x115
#define AC_HDR_SHA_2_3_BIT_SHIFT        2
#define AC_HDR_PPK_SELECT_BIT_SHIFT     16
#define AC_HDR_SPK_SELECT_BIT_SHIFT     18

/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/


/* RSA 4096 Authentication Certificate */
typedef struct
{
    uint32_t acHeader;                      // 0x000
    uint32_t spkId;                         // 0x004
    uint8_t acUdf[UDF_DATA_SIZE];           // 0x008
    ACKey4096 acPpk;                        // 0x040
    ACKey4096 acSpk;                        // 0x480
    ACSignature4096 acSpkSignature;         // 0x8C0
    ACSignature4096 acBhSignature;          // 0xAC0
    ACSignature4096 acPartitionSignature;   // 0xCC0
} AuthCertificate4096Structure;


class ZynqMpAuthenticationContext : public AuthenticationContext
{
public:
    ZynqMpAuthenticationContext();
    ZynqMpAuthenticationContext(const AuthenticationContext* refAuthContext);
    ZynqMpAuthenticationContext(const AuthCertificate4096Structure* existingCert);
    ~ZynqMpAuthenticationContext();

    uint32_t GetCertificateSize(void) { return certSize; }
    Section* CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection);
    void Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert);
    void GeneratePPKHash(const std::string& filename);
    void GenerateSPKHash(uint8_t* sha256_hash_padded);
    void GenerateBHHash(BootImage& bi, uint8_t* sha_hash_padded);
    void CreatePadding(uint8_t* signature, const uint8_t* hash);
    void CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection);
    void RearrangeEndianess(uint8_t *array, uint32_t size);
    void AddAuthCertSizeToTotalFSBLSize(PartitionHeader* header);

    void SetKeyLength(Authentication::Type type);

private:
    uint32_t certSize;
    void CopySPKSignature(ACSignature4096* ptr);
    std::string GetCertificateName(std::string name);
    void CopyBHSignature(BootImage& bi, ACSignature4096* ptr);
};
#endif
