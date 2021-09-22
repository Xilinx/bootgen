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

#ifndef _ENCRYPTION_ZYNQMP_CONTEXT_
#define _ENCRYPTION_ZYNQMP_CONTEXT_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "encryption.h"


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
class ZynqMpEncryptionContext : public EncryptionContext
{
public:
    ZynqMpEncryptionContext();
    ZynqMpEncryptionContext(const EncryptionContext * other);
    ~ZynqMpEncryptionContext();

    Encryption::Type Type()
    {
        return Encryption::AES;
    }

    void ReadEncryptionKeyFile(const std::string& keyFileName);
    void WriteEncryptionKeyFile(const std::string& baseFileName, bool useOptionalKey, uint32_t blocks);
    void Process(BootImage& bi, PartitionHeader* partition);
    void GenerateAesKey(void);

    void SetAesSeedString(const std::string & key);
    void SetAesSeed(const uint8_t * key);
    const uint32_t* GetAesSeed(void);

    void SetAesKey(const uint8_t* binarykey);
    const uint32_t* GetAesKey(void);

    void SetIv(const uint8_t* iv);
    void SetIvString(const std::string& IV);
    void GenerateIv(void);
    const uint32_t* GetIv(void);

    void SetAesOptKey(const uint8_t* binarykey);
    void SetAesOptKeyString(const std::string& key);
    void GenerateAesOptKey(void);
    const uint32_t* GetAesOptKey(void);

    void PackNextEncryptionKey(uint8_t* aesKeyNext, int aeskeyPtr);
    void PackNextIv(uint8_t * aesIvNext, int aesIvPtr);
    void GetNextKey(uint8_t* keyNext, int ptr);
    void GetNextIv(uint8_t* keyNext, int ptr);

    void GetEncryptionKeys(Options& options, uint8_t* aesKey, uint8_t* aesOptKey, uint8_t* aesIV);
    void GenerateRemainingKeys(Options& options, std::string aesFilename);

protected:
    bool isBootloader;
    bool aesSeedexits;
    int partNum;

    void ChunkifyAndEncrypt(Options& options, const uint8_t *inBuf, uint32_t inLen, uint8_t* outBuf, uint32_t& outLen);
    void GenerateGreyKey();
    void ReadBhIv(uint8_t * bhIv);
    AesGcmEncryptionContext* encryptionAlgorithm;
};
#endif
