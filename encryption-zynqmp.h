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
    void GenerateEncryptionKeyFile(const std::string& baseFileName, Options& options);
    void WriteEncryptionKeyFile(const std::string& baseFileName, bool useOptionalKey, uint32_t blocks);
    void Process(BootImage& bi, PartitionHeader* partition);

    void GenerateAesKey(void);

    /* For aesSeed used for Key Generation in ZynqMp. GenerateAesSeed() uses SetRandomSeed(); from the Encryption Contex */
    void GenerateAesSeed(void);
    void SetAesSeedString(const std::string & key);
    void SetAesSeed(const uint8_t * key);
    const uint32_t* GetAesSeed(void);

    void SetAesLabelString(const std::string & key);
    void SetAesLabel(const uint8_t * key, int bytes);
    const uint8_t* GetAesLabel(void);

    void SetAesContextString(const std::string & key);
    void SetAesContext(const uint8_t * key, int bytes);
    const uint8_t* GetAesContext(void);

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

    std::string ConvertKeyIvToString(uint8_t *keyIv, uint8_t size);

    std::vector<std::string> aesKeyVec;
    std::vector<std::string> aesIvVec;

    void CAVPonCounterModeKDF(std::string filename);
    void ParseKDFTestVectorFile(std::string filename);

    uint32_t koLength;
    uint8_t* kI;
    uint32_t fixedInputDataByteLength;
    uint8_t* fixedInputData;

protected:
    uint32_t* aesKey;
    uint32_t* aesOptKey;
    uint32_t* aesIv;
    uint32_t* aesSeed;
    uint8_t* aesContext;
    uint8_t* aesLabel;
    uint32_t* outBufKDF;
    int aesContextBytes;
    int aesLabelBytes;
    bool isBootloader;
    bool aesSeedexits;
    int partNum;

    void ChunkifyAndEncrypt(Options& options, const uint8_t *inBuf, uint32_t inLen, uint8_t* outBuf, uint32_t& outLen);
    void CounterModeKDF(uint32_t blocks, std::string filename, bool dump);
    void SetMetalKeyString(const std::string & key, uint8_t * metalKey);
    void GenerateGreyKey();
    void GenerateMetalKey();
    void ReadBhIv(uint8_t * bhIv);
    AesGcmEncryptionContext* encryptionAlgorithm;
};
#endif
