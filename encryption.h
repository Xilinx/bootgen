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

#ifndef _ENCRYPTION_CONTEXT_
#define _ENCRYPTION_CONTEXT_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <string>
#include <stdint.h>
#include <sstream>
#include <vector>
#include <string>

#include "kdf.h"

#include "bootgenenum.h"
#include "options.h"

/* Forward Class References */
class PartitionHeader;
class Options;
class DataMover;
class BootImage;

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define WORDS_PER_AES_BLOCK                 4
#define WORDS_PER_AES_KEY                   8
#define BYTES_PER_AES_KEY                   32

#define BYTES_PER_FID                       60
#define WORDS_PER_FID                       15

#define ENCRYPTION_ENABLE                   0x00000040
#define EFUSE_KEY_SOURCE                    0x80000000
#define BBRAM_KEY_SOURCE                    0x00000000

#define ENCRYPTION_KEY_SRC_OFFSET           36
#define ENCRYPTION_ENABLE_WORD_OFFSET       44
#define CBC_WORD_OFFSET                     116
#define ENCRYPTION_DATA_LENGTH_OFFSET       136

#define FPGA_ENCRYPTION_KEY_SRC_OFFSET      4
#define FPGA_ENCRYPTION_ENABLE_WORD_OFFSET  12
#define FPGA_CBC_WORD_OFFSET                84
#define FPGA_ENCRYPTION_DATA_LENGTH_OFFSET  104
#define WORDS_PER_IV                        3
#define BYTES_PER_IV                        12


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class EncryptionContext
{
public:
    EncryptionContext()
        : deviceName("")
        , metalFile("")
        , bhKekIVFile("")
        , bbramKekIVFile("")
        , efuseKekIVFile("")
        , efuseUserKek0IVFile("")
        , efuseUserKek1IVFile("")
        , aesFilename("")
        , aesKey(NULL)
        , aesOptKey(NULL)
        , aesIv(NULL)
        , aesSeed(NULL)
        , outBufKDF(NULL)
        , fixedInputDataByteLength(0)
        , fixedInputData(NULL)
        , fixedInputDataExits(false)
    {
        kdf = new Kdf();
        LOG_TRACE("KDF Version : 0x%X", kdf->GetVersion());
    };

    virtual ~EncryptionContext()
    {
        if(kdf)
        {
            delete kdf;
        }
    };

    virtual Encryption::Type Type() 
    {
        return Encryption::None;
    }
    virtual void Process(BootImage& bi, PartitionHeader* partition) {};
    virtual void Process(BootImage & bi, PartitionHeader * partHdr, bool chunkbootloader) {};
    virtual void Process(BootImage& bi) {};
    virtual void WriteEncryptionKeyFile(const std::string& baseFileName, bool useOptionalKey, uint32_t blocks) {};
    virtual void ReadEncryptionKeyFile(const std::string& keyFileName) {};
    virtual void GenerateEncryptionKeyFile(const std::string& baseFileName, Options& options);
    virtual void GenerateAesKey(void) {};
    virtual void SetAesKey(const uint8_t* binarykey) {};
    virtual void ReadBhIv(uint8_t* bhIv) {};
    virtual void GenerateGreyKey() {};
    virtual void GenerateRemainingKeys() {};
    void SetAesFileName(std::string);
    void SetMetalKeyFile(std::string file);
    void SetBHKekIVFile(std::string file);
    void SetBbramKekIVFile(std::string file);
    void SetEfuseKekIVFile(std::string file);
    void SetEfuseUserKek0IVFile(std::string file);
    void SetEfuseUserKek1IVFile(std::string file);
    void SetDeviceName(const std::string& deviceName0);
    void PackHex(const std::string & hexString, uint8_t * hexData);
    void SetRandomSeed(void);
    void SetAesKeyString(const std::string& ASCIIkey);
    std::string ConvertKeyIvToString(uint8_t *keyIv, uint8_t size);

    void SetKdfLogFile(bool encrdump);
    void GetRandomData(uint8_t* randomData, uint32_t randomDatabyteLength);
    uint32_t static GetTotalEncryptionBlocks(Binary::Length_t partitionSize, std::vector<uint32_t> encrBlocks, uint32_t defEncrBlockSize, Binary::Length_t * lastBlock);

    void CheckForSameAesKeyFiles(std::vector<std::string>);
    void CheckForExtraKeyIVPairs(uint32_t, std::string name);
    void CheckForRepeatedKeyIVPairs(std::vector<std::string> aesKeyFileVec, bool maskKey0IV0);

    std::string GetAesFileName(void);
    std::string GetMetalKeyFile(void);
    std::string GetBHKekIVFile(void);
    std::string GetBbramKekIVFile(void);
    std::string GetEfuseKekIVFile(void);
    std::string GetEfuseUserKek0IVFile(void);
    std::string GetEfuseUserKek1IVFile(void);

    virtual void SetAesSeed(const uint8_t * key) {};
    void GenerateAesSeed(void);
    virtual const uint32_t* GetAesSeed(void) { return NULL; };

    void SetAesFixedInputDataString(const std::string & key);
    void SetAesFixedInputData(const uint8_t * key, uint32_t bytes);
    void GenerateAesFixedInputData(void);
    uint32_t* GetFixedInputData(void) { return fixedInputData; };

    std::string aesFilename;
    Kdf* kdf;

protected:
    uint32_t* aesKey;
    uint32_t* aesOptKey;
    uint32_t* aesIv;
    std::vector<std::string> aesKeyVec;
    std::vector<std::string> aesIvVec;
    uint32_t* aesSeed;
    uint32_t* outBufKDF;
    uint32_t* fixedInputData;
    uint32_t fixedInputDataByteLength;
    std::string deviceName;
    std::string metalFile;
    std::string bhKekIVFile;
    std::string bbramKekIVFile;
    std::string efuseKekIVFile;
    std::string efuseUserKek0IVFile;
    std::string efuseUserKek1IVFile;
    bool fixedInputDataExits;
};

/******************************************************************************/
class NoneEncryptionContext : public EncryptionContext 
{
public:
    Encryption::Type Type()
    {
        return Encryption::None;
    }
};

/******************************************************************************/
class AesGcmEncryptionContext
{
public:
    AesGcmEncryptionContext() { };
    ~AesGcmEncryptionContext() { };

    void AesGcm256Encrypt(unsigned char* gcm_pt, int pt_len, unsigned char* gcm_key, unsigned char* gcm_iv, unsigned char* gcm_aad, int aad_len,
        unsigned char* gcm_ct, int& ct_len, unsigned char* gcm_tag);
    void AesGcm256Decrypt(unsigned char* gcm_pt, int& pt_len, unsigned char* gcm_key, unsigned char* gcm_iv, unsigned char* gcm_aad, int aad_len,
        unsigned char* gcm_ct, int ct_len, unsigned char* gcm_tag);
};
#endif
