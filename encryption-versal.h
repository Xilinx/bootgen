/******************************************************************************
* Copyright 2015-2021 Xilinx, Inc.
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
#ifndef _ENCRYPTION_VERSAL_CONTEXT_
#define _ENCRYPTION_VERSAL_CONTEXT_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <string>
#include "bootgenenum.h"
#include "options.h"
#include <stdint.h>
#include <sstream>

/* Forward Class References */
class PartitionHeader;
class Options;
class DataMover;

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class VersalEncryptionContext : public EncryptionContext
{
public:
    VersalEncryptionContext();
    VersalEncryptionContext(const EncryptionContext * other);
    ~VersalEncryptionContext();

    Encryption::Type Type()
    {
        return Encryption::AES;
    }

    void Process(BootImage& bi, PartitionHeader* partition);
    void Process(BootImage& bi);

    void SetAesSeedString(const std::string & key);
    void SetAesSeed(const uint8_t * key);
    const uint32_t* GetAesSeed(void);

    void GenerateAesKey(void);

    void WriteEncryptionKeyFile(const std::string& baseFileName, bool useOptionalKey, uint32_t blocks);
    void ReadEncryptionKeyFile(const std::string& keyFileName);

    void SetAesKey(const uint8_t* binarykey);
    const uint32_t* GetAesKey(void);

    void SetIv(const uint8_t* iv);
    void SetIvString(const std::string& IV);
    void GenerateIv(void);
    const uint32_t* GetIv(void);

    void PackNextEncryptionKey(uint8_t* aesKeyNext, int aeskeyPtr);
    void PackNextIv(uint8_t * aesIvNext, int aesIvPtr);
    void GetNextKey(uint8_t* keyNext, int ptr);
    void GetNextIv(uint8_t* keyNext, int ptr);

    void GenerateRemainingKeys(Options& options);

private:
    bool isBootloader;
    bool isPmcData;
    bool aesSeedexits;

    void ChunkifyAndEncrypt(Options& options, const uint8_t *inBuf, uint32_t inLen, uint8_t *aad, uint32_t aad_len, uint8_t* outBuf, uint32_t& outLen);
    void GetEncryptionKeys(Options& options, uint8_t* aesKey, uint8_t* aesOptKey, uint8_t* aesIV);
    void WarnforDPACMImpactonBootTime(bool dpacmKeyRollingEnable, bool dpacmMaskEnable);

    void AesGcm256Encrypt(unsigned char* gcm_pt, int pt_len, unsigned char* gcm_key, unsigned char* gcm_iv, unsigned char* gcm_aad, int aad_len,
        unsigned char* gcm_ct, int& ct_len, unsigned char* gcm_tag);
    void AesGcm256Decrypt(unsigned char* gcm_pt, int& pt_len, unsigned char* gcm_key, unsigned char* gcm_iv, unsigned char* gcm_aad, int aad_len,
        unsigned char* gcm_ct, int ct_len, unsigned char* gcm_tag);

protected:
    AesGcmEncryptionContext* encryptionAlgorithm;
};
#endif
