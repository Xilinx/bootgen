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
        , bhIvFile("")
        , aesFilename("")
    { };

    virtual ~EncryptionContext() { };

    virtual Encryption::Type Type() = 0;
    virtual void Process(BootImage& bi, PartitionHeader* partition) {};
    virtual void WriteEncryptionKeyFile(const std::string& baseFileName, bool useOptionalKey, uint32_t blocks) {};
    virtual void ReadEncryptionKeyFile(const std::string& keyFileName) {};
    virtual void GenerateEncryptionKeyFile(const std::string& baseFileName, Options& options) {};
    virtual void GenerateAesKey(void) {};
    virtual void SetAesKey(const uint8_t* binarykey) {};
    virtual void ReadBhIv(uint8_t* bhIv) {};
    virtual void GenerateGreyKey() {};
    virtual void GenerateMetalKey() {};
    
    void SetAesFileName(std::string);
    void SetMetalKeyFile(std::string file);
    void SetBhIvFile(std::string file);
    void SetDeviceName(const std::string& deviceName0);
    void PackHex(const std::string & hexString, uint8_t * hexData);
    void SetRandomSeed(void);
    void SetAesKeyString(const std::string& ASCIIkey);

    void GetRandomData(uint8_t* randomData, uint32_t randomDatabyteLength);
    std::string GetAesFileName(void);
    std::string GetMetalKeyFile(void);
    std::string GetBhIvFile(void);

    std::string aesFilename;
protected:
    std::string deviceName;
    std::string metalFile;
    std::string bhIvFile;
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
