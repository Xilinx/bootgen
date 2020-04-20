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
#ifndef _AUTHENTICATION_ZYNQ_H_
#define _AUTHENTICATION_ZYNQ_H_

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
#define AUTH_HDR_ZYNQ           0x00000101

typedef struct
{
    uint32_t acHeader;                      // 0x000
    uint32_t acSize;                        // 0x004
    uint8_t acUdf[UDF_DATA_SIZE];           // 0x008
    ACKey2048 acPpk;                        // 0x040
    ACKey2048 acSpk;                        // 0x280
    ACSignature2048 acSpkSignature;         // 0x4C0
    ACSignature2048 acPartitionSignature;   // 0x5C0
} AuthCertificate2048Structure;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
class ZynqAuthenticationContext : public AuthenticationContext
{
public:
    ZynqAuthenticationContext();
    ZynqAuthenticationContext(const AuthenticationContext* refAuthContext);
    ZynqAuthenticationContext(const AuthCertificate2048Structure* existingCert);
    ~ZynqAuthenticationContext();

    uint32_t GetCertificateSize(void) { return certSize; }
    Section* CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection);
    void Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert);
    void GeneratePPKHash(const std::string& filename);
    void SetKeyLength(Authentication::Type type);
    void GenerateSPKHash(uint8_t* sha256_hash_padded);
    void CreatePadding(uint8_t* signature, const uint8_t* hash);
    void CopyPartitionSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, Section* acSection);
    void RearrangeEndianess(uint8_t *array, uint32_t size) {};

private:
    uint32_t certSize;
    void CopySPKSignature(ACSignature2048* ptr);
};
#endif
