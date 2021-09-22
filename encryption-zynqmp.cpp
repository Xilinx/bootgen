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

/*
------------------------------------------------------------------------------------------------
****************************************************************   H E A D E R   F I L E S   ***
------------------------------------------------------------------------------------------------
*/
#include "encryption-zynqmp.h"
#include "bootimage.h"
#include "encryptutils.h"
#include "options.h"
#include "encryptutils.h"
#include "fileutils.h"
#include <openssl/rand.h>
#include "obfskutil.h"

/*
------------------------------------------------------------------------------------------------
**********************************************************************   F U N C T I O N S   ***
------------------------------------------------------------------------------------------------
*/

/******************************************************************************/
ZynqMpEncryptionContext::ZynqMpEncryptionContext()
    : isBootloader(false)
    , aesSeedexits(false)
    , partNum(0)
{
    encryptionAlgorithm = new AesGcmEncryptionContext();
};

/******************************************************************************/
ZynqMpEncryptionContext::ZynqMpEncryptionContext(const EncryptionContext* other)
    : isBootloader(false)
    , aesSeedexits(false)
    , partNum(0)
{
    aesFilename = other->aesFilename;
    encryptionAlgorithm = new AesGcmEncryptionContext();
}

/******************************************************************************/
ZynqMpEncryptionContext::~ZynqMpEncryptionContext()
{
    if (encryptionAlgorithm)
    {
        delete encryptionAlgorithm;
    }
    if (aesKey)
    {
        delete aesKey;
    }
    if (aesIv)
    {
        delete aesIv;
    }
    if (aesSeed)
    {
        delete aesSeed;
    }
    if (outBufKDF)
    {
        delete outBufKDF;
    }
    if (fixedInputData)
    {
        delete fixedInputData;
    }
};

/******************************************************************************/
void ZynqMpEncryptionContext::SetAesKey(const uint8_t* key)
{
    aesKey = new uint32_t[WORDS_PER_AES_KEY];
    for (uint32_t index = 0; index < WORDS_PER_AES_KEY; index++)
    {
        aesKey[index] = ReadBigEndian32(key);
        key += sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqMpEncryptionContext::SetAesSeedString(const std::string& key)
{
    uint8_t  hexData[256];

    if (key.size() != (WORDS_PER_AES_KEY * 8))
    {
        LOG_DEBUG(DEBUG_STAMP, "Seed size - %d", key.size());
        LOG_ERROR("An AES Seed must be 256 bits long - %s", key.c_str());
    }
    PackHex(key, hexData);
    SetAesSeed(hexData);
}

/******************************************************************************/
void ZynqMpEncryptionContext::GenerateAesKey(void)
{
    uint32_t keysize = WORDS_PER_AES_KEY * sizeof(uint32_t);
    uint8_t newKey[BYTES_PER_AES_KEY];

    RAND_bytes(newKey, keysize);
    SetAesKey(newKey);

    LOG_INFO("AES Key generated successfully");
}

/******************************************************************************/
void ZynqMpEncryptionContext::GetEncryptionKeys(Options& options, uint8_t* aesKey, uint8_t* aesOptKey, uint8_t* aesIV)
{
    const uint32_t* tmpKey = GetAesKey();
    if (tmpKey != NULL)
    {
        memcpy_be(aesKey, tmpKey, AES_GCM_KEY_SZ);
    }
    else
    {
        LOG_ERROR("Encryption Error !!!\n           Key 0 does not exist in the AES key file ");
    }

    if (options.bifOptions->GetAesOptKeyFlag())
    {
        const uint32_t* tmpOptKey = GetAesOptKey();
        if (tmpOptKey != NULL)
        {
            memcpy_be(aesOptKey, tmpOptKey, AES_GCM_KEY_SZ);
        }
        else
        {
            LOG_ERROR("Encryption Error !!!\n           Operational Key 'Key Opt' does not exist in the AES key file ");
        }
    }

    const uint32_t* tmpIv = GetIv();
    if (tmpIv != NULL)
    {
        memcpy_be(aesIV, tmpIv, AES_GCM_IV_SZ);
    }
    else
    {
        LOG_ERROR("IV does not exist in the AES key file ");
    }
}

/******************************************************************************/
void ZynqMpEncryptionContext::WriteEncryptionKeyFile(const std::string & baseFileName, bool useOptionalKey, uint32_t blocks)
{
    uint32_t x, y, index;

    /* Setup the file for writing */
    std::string ext = StringUtils::GetExtension(baseFileName);
    std::string filename;
    if (ext == "")
    {
        filename = baseFileName + ".nky";
    }
    else
    {
        filename = baseFileName;
    }
    std::ofstream keyFile(filename.c_str());

    if (!keyFile)
    {
        LOG_ERROR("Failure writing AES key file", filename.c_str());
    }


    /* Write device name */
    if (deviceName != "")
    {
        keyFile << "Device       " << deviceName << ";\n";
        keyFile << "\n";
    }

    /* Ko can then be used for key/iv pairs */
    for (x = 0; x < blocks; x++) 
    {
        keyFile << "Key " << std::dec << x;
        keyFile << "        ";
        if (x == 0 && GetAesKey() != NULL) {
            for (index = 0; index < WORDS_PER_AES_KEY; ++index)
            {
                keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << aesKey[index];
            }
        }
        else {
            for (y = 0; y < WORDS_PER_AES_KEY; y++)
            {
                keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << outBufKDF[11 * x + y];
            }
        }
        keyFile << ";\n";
        keyFile << "IV " << std::dec << x;
        keyFile << "         ";
        if (x == 0 && GetIv() != NULL) {
            for (y = 0; y < WORDS_PER_IV; ++y)
            {
                keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << aesIv[y];
            }
        }
        else {
            for (y = WORDS_PER_AES_KEY; y < WORDS_PER_AES_KEY + WORDS_PER_IV; y++)
            {
                keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << outBufKDF[11 * x + y];
            }
        }
        keyFile << ";\n";
        keyFile << "\n";
    }
    if (useOptionalKey == true)
    {
        keyFile << "Key Opt      ";
        if (GetAesOptKey() != NULL) {
            for (y = 0; y < WORDS_PER_AES_KEY; ++y)
            {
                keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << aesOptKey[y];
            }
        }
        else {
            for (y = 0; y < WORDS_PER_AES_KEY; y++)
            {
                keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << outBufKDF[11 * x + y];
            }
        }
        keyFile << ";\n";
    }
}

/******************************************************************************/
void ZynqMpEncryptionContext::ReadEncryptionKeyFile(const std::string&	inputFileName)
{
    LOG_TRACE("Reading the AES key file");
    int aesKeyNum = 0;
    int aesIvNum = 0;
    // Get the encryption key file name
    std::ifstream keyFile(inputFileName.c_str());
    aesKeyVec.clear();
    aesIvVec.clear();
    if (!keyFile)
    {
        LOG_ERROR("Failure reading AES key file - %s", inputFileName.c_str());
    }

    while (keyFile)
    {
        std::string word;
        keyFile >> word;
        // If file vacant
        if (word == "")
        {
            return;
        }

        char c = ' ';
        // First Word is "Key"
        if (word == "Key")
        {
            keyFile >> word;
            // Second Word is "Opt"
            if (word == "Opt")
            {
                word = "";
                while ((keyFile >> c) && isalnum(c))
                {
                    word.push_back(c);
                }
                if (word == "pick")
                {
                    GenerateAesOptKey();
                }
                else
                {
                    SetAesOptKeyString(word);
                }
            }
            // Second Word is "0"
            else if (word != "" && isalnum(word[0]))
            {
                int index = std::stoi(word);
                if (aesKeyNum != index)
                {
                    LOG_DEBUG(DEBUG_STAMP, "Key order incorrect.");
                    LOG_ERROR("Error parsing AES key file - %s.", inputFileName.c_str());
                }
                aesKeyNum++;
                word = "";
                while ((keyFile >> c) && isalnum(c))
                {
                    word.push_back(c);
                }
                // Third Word is "pick" - then generate AES key, otherwise set the key specified
                if (word == "pick")
                {
                    GenerateAesKey();
                }
                else
                {
                    if (aesKeyVec.size() == 0)
                    {
                        SetAesKeyString(word);
                    }
                    aesKeyVec.push_back(word);
                }
            }

            // Second Word other than "0", "StartCBC" or "HMAC" - throw error
            else
            {
                LOG_DEBUG(DEBUG_STAMP, "Unknown key type - '%s' in %s", word.c_str(), inputFileName.c_str());
                LOG_ERROR("Error parsing AES key file - %s", inputFileName.c_str());
            }
        }

        // If first word is not "Key" but "Device"
        else if (word == "Device")
        {
            word = "";
            keyFile >> word;
            /*while( (keyFile >> c) && (isalnum(c) || c=='-' || c=='_') )
            {
            word.push_back(c);
            }*/
            c = word[word.size() - 1];
            word.erase(word.size() - 1);
            deviceName = word;
        }

        else if (word == "IV")
        {
            keyFile >> word;
            if (word != "")
            {
                if (word.size() == ((BYTES_PER_IV * 2) + 1))
                {
                    c = word[word.size() - 1];
                    word.erase(word.size() - 1);
                    SetIvString(word);
                }
                else
                {
                    int index = std::stoi(word);
                    if (aesIvNum != index)
                    {
                        LOG_DEBUG(DEBUG_STAMP, "Iv order incorrect.");
                        LOG_ERROR("Error parsing AES key file - %s.", inputFileName.c_str());
                    }
                    aesIvNum++;
                    word = "";
                    while ((keyFile >> c) && isalnum(c))
                    {
                        word.push_back(c);
                    }
                    if (word == "pick")
                    {
                        GenerateIv();
                    }
                    else
                    {
                        if (aesIvVec.size() == 0)
                        {
                            SetIvString(word);
                        }
                    }
                }
                aesIvVec.push_back(word);
            }
        }

        else if (word == "Seed")
        {
            word = "";
            while ((keyFile >> c) && isalnum(c))
            {
                word.push_back(c);
            }
            SetAesSeedString(word);
            aesSeedexits = true;
        }

        else if (word == "Label")
        {
            LOG_ERROR("The input 'Label' is deprecated.\n\t    Please construct a FixedInputData of 60 Bytes instead and provide the same in the nky file.This will be used along with Seed in KDF.");
        }
        else if (word == "Context")
        {
            LOG_ERROR("The input 'Context' is deprecated.\n\t    Please construct a FixedInputData of 60 Bytes instead and provide the same in the nky file.This will be used along with Seed in KDF.");
        }
        else if (word == "FixedInputData")
        {
            word = "";
            while ((keyFile >> c) && isalnum(c))
            {
                word.push_back(c);
            }
            SetAesFixedInputDataString(word);
            fixedInputDataExits = true;
        }
        // If first word is neither "Key" nor "Device"
        else
        {
            LOG_DEBUG(DEBUG_STAMP, "'Key' or 'Device' identifier expected, '%s' found instead", word.c_str());
            LOG_ERROR("Error parsing AES key file - %s", inputFileName.c_str());
        }

        // Semicolons expected at end of every line
        if (c != ';')
        {
            while ((keyFile >> c) && isspace(c))
            {
                word.push_back(c);
            }
        }

        if (c != ';')
        {
            LOG_DEBUG(DEBUG_STAMP, "Terminating ';' expected. Last word read was '%s'", word.c_str());
            LOG_ERROR("Error parsing AES key file - %s", inputFileName.c_str());
        }

        if (deviceName == "")
        {
            LOG_DEBUG(DEBUG_STAMP, "Partname 'Device' missing in the key file - %s", inputFileName.c_str());
            LOG_ERROR("Error parsing AES key file - %s", inputFileName.c_str());
        }

        if ((aesKeyVec.size() > 1 || aesIvVec.size() > 1) && aesSeedexits)
        {
            LOG_ERROR("Seed is not expected with multiple keys/Iv.");
        }
        if (fixedInputDataExits && !aesSeedexits)
        {
            LOG_ERROR("Seed must be specified along with FixedInputData.");
        }
    }
}

/******************************************************************************/
void ZynqMpEncryptionContext::PackNextEncryptionKey(uint8_t* aesKeyNext, int aeskeyPtr)
{
    uint8_t  hexData[AES_GCM_KEY_SZ];
    uint8_t* hexDataPtr = hexData;

    if (aesKeyVec[aeskeyPtr].length() != (WORDS_PER_AES_KEY * 8))
    {
        LOG_DEBUG(DEBUG_STAMP, "AES key %d size - %d", aeskeyPtr, aesKeyVec[aeskeyPtr].length());
        LOG_ERROR("An AES key must be 256 bits long - %s", aesKeyVec[aeskeyPtr].c_str());
    }

    if (aesKeyVec[aeskeyPtr].length() & 1)
    {
        LOG_DEBUG(DEBUG_STAMP, "Hex String - %s - does not have even no. of hex digits", aesKeyVec[aeskeyPtr].c_str());
        LOG_ERROR("Error parsing encryption key");
    }

    for (uint32_t i = 0; i<aesKeyVec[aeskeyPtr].length(); i += 2)
    {
        std::string byte = aesKeyVec[aeskeyPtr].substr(i, 2);
        if (!isxdigit(byte[0]) || !isxdigit(byte[1]))
        {
            LOG_DEBUG(DEBUG_STAMP, "Hex String - %s - is has a non hex digit", aesKeyVec[aeskeyPtr].c_str());
            LOG_ERROR("Error parsing encryption key");
        }
        *hexDataPtr++ = (uint8_t)strtoul(byte.c_str(), NULL, 16);
    }

    hexDataPtr = hexData;
    memcpy(aesKeyNext, hexDataPtr, AES_GCM_KEY_SZ);
}

/******************************************************************************/
void ZynqMpEncryptionContext::PackNextIv(uint8_t* aesIVNext, int aesIvPtr)
{
    uint8_t  hexData[AES_GCM_IV_SZ];
    uint8_t* hexDataPtr = hexData;

    if (aesIvVec[aesIvPtr].length() != (BYTES_PER_IV * 2))
    {
        LOG_DEBUG(DEBUG_STAMP, "IV[%d] Size = %d", aesIvPtr, aesIvVec[aesIvPtr].length());
        LOG_ERROR("Encryption Error !!!\n           An IV key must be 12 bytes long");
    }

    if (aesIvVec[aesIvPtr].length() & 1)
    {
        LOG_DEBUG(DEBUG_STAMP, "Hex String - %s - does not have even no. of hex digits", aesIvVec[aesIvPtr].c_str());
        LOG_ERROR("Error parsing encryption key");
    }

    for (uint32_t i = 0; i<aesIvVec[aesIvPtr].length(); i += 2)
    {
        std::string byte = aesIvVec[aesIvPtr].substr(i, 2);
        if (!isxdigit(byte[0]) || !isxdigit(byte[1]))
        {
            LOG_DEBUG(DEBUG_STAMP, "Hex String - %s - is has a non hex digit", aesIvVec[aesIvPtr].c_str());
            LOG_ERROR("Error parsing encryption key");
        }
        *hexDataPtr++ = (uint8_t)strtoul(byte.c_str(), NULL, 16);
    }

    hexDataPtr = hexData;
    memcpy(aesIVNext, hexDataPtr, BYTES_PER_IV);
}

/******************************************************************************/
void ZynqMpEncryptionContext::GetNextKey(uint8_t * keyNext, int ptr)
{
    PackNextEncryptionKey(keyNext, ptr);
}

/******************************************************************************/
void ZynqMpEncryptionContext::GetNextIv(uint8_t * keyNext, int ptr)
{
    PackNextIv(keyNext, ptr);
}

/******************************************************************************/
void ZynqMpEncryptionContext::SetAesSeed(const uint8_t* key)
{
    aesSeed = new uint32_t[WORDS_PER_AES_KEY];

    for (uint32_t index = 0; index < WORDS_PER_AES_KEY; index++)
    {
        aesSeed[index] = ReadBigEndian32(key);
        key += sizeof(uint32_t);
    }
}

/******************************************************************************/
const uint32_t* ZynqMpEncryptionContext::GetAesSeed(void)
{
    return (uint32_t *)aesSeed;
}

/******************************************************************************/
const uint32_t * ZynqMpEncryptionContext::GetAesKey(void)
{
    return (uint32_t *)aesKey;
}

/******************************************************************************/
const uint32_t* ZynqMpEncryptionContext::GetAesOptKey(void)
{
    return (uint32_t *)aesOptKey;
}

/******************************************************************************/
void ZynqMpEncryptionContext::SetAesOptKey(const uint8_t* key)
{
    aesOptKey = new uint32_t[WORDS_PER_AES_KEY];

    for (uint32_t index = 0; index < WORDS_PER_AES_KEY; index++)
    {
        aesOptKey[index] = ReadBigEndian32(key);
        key += sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqMpEncryptionContext::SetAesOptKeyString(const std::string& key)
{
    uint8_t  hexData[256];

    if (key.size() != (WORDS_PER_AES_KEY * 8))
    {
        LOG_DEBUG(DEBUG_STAMP, "AES key size - %d", key.size());
        LOG_ERROR("An AES key must be 256 bits long - %s", key.c_str());
    }

    PackHex(key, hexData);
    SetAesOptKey(hexData);
}

/******************************************************************************/
void ZynqMpEncryptionContext::GenerateAesOptKey(void)
{
    uint32_t keysize = WORDS_PER_AES_KEY * sizeof(uint32_t);
    uint8_t newKeyData[BYTES_PER_AES_KEY];

    // Get temp data to encrypt as the newKey.
    RAND_bytes(newKeyData, keysize);

    // Set the new key.
    SetAesOptKey(newKeyData);

    // For logging purpose
    LOG_INFO("AES Key generated successfully");
}

/******************************************************************************/
void ZynqMpEncryptionContext::SetIv(const uint8_t* iv)
{
    aesIv = new uint32_t[WORDS_PER_IV];

    // Copy the key
    for (uint32_t index = 0; index < WORDS_PER_IV; index++)
    {
        aesIv[index] = ReadBigEndian32(iv);
        iv += sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqMpEncryptionContext::SetIvString(const std::string& IV)
{
    uint8_t  hexData[256];

    if (IV.size() != (BYTES_PER_IV * 2))
    {
        LOG_DEBUG(DEBUG_STAMP, "IV = %s, IV Size = %d", IV.c_str(), IV.size());
        LOG_ERROR("Encryption Error !!!\n           An IV key must be 12 bytes long");
    }
    PackHex(IV, hexData);
    SetIv(hexData);
}

/******************************************************************************/
void ZynqMpEncryptionContext::GenerateIv(void)
{
    // Get temp data to encrypt as the newCBC.
    uint8_t newIVData[BYTES_PER_IV];
    RAND_bytes(newIVData, BYTES_PER_IV);

    SetIv(newIVData);

    // For logging purpose
    LOG_INFO("AES IV generated successfully");
}

/******************************************************************************/
const uint32_t* ZynqMpEncryptionContext::GetIv(void)
{
    return (uint32_t *)aesIv;
}

/******************************************************************************/
void ZynqMpEncryptionContext::GenerateRemainingKeys(Options& options, std::string aesFilename)
{
    uint32_t blocks, x;
    bool useOptionalKey = false;
    if (GetAesOptKey() == NULL)
    {
        useOptionalKey = options.bifOptions->GetAesOptKeyFlag();
    }

    if (GetAesKey() == NULL && !aesSeedexits)
    {
        LOG_ERROR("Encryption Error !!!\n           Key 0 does not exist in the AES key file ");
    }

    if (options.bifOptions->GetAesOptKeyFlag())
    {
        if (GetAesOptKey() == NULL && !aesSeedexits)
        {
            LOG_ERROR("Encryption Error !!!\n           Operational Key 'Key Opt' does not exist in the AES key file ");
        }
    }

    if (GetIv() == NULL && !aesSeedexits)
    {
        LOG_ERROR("IV does not exist in the AES key file ");
    }

    if (aesKeyVec.size() != aesIvVec.size())
    {
        LOG_ERROR("Encryption Error !!!\n           Number of Keys in the AES key file are not equal to the number of IVs. ");
    }
    // If only seed exists in the given in nky file => No key0/iv0 mentioned. +1 to generate key0/IV0 for SH
    if (GetAesKey() == NULL && aesSeedexits)
    {
        blocks = options.bifOptions->GetEncryptionBlocksList().size() + 1 + useOptionalKey;
        LOG_TRACE("Keys for encryption will be generated based on the seed");
    }
    else
    {
        blocks = options.bifOptions->GetEncryptionBlocksList().size() + useOptionalKey;
        if (aesSeedexits)
        {
            LOG_TRACE("Keys for encryption will be generated based on the seed");
        }
        else
        {
            LOG_TRACE("Keys for encryption will be generated based on Key 0");
        }
    }

    if (GetAesSeed() == NULL)
    {
        aesSeed = new uint32_t[WORDS_PER_AES_KEY];
        GenerateAesSeed();
    }

    if (GetFixedInputData() == NULL)
    {
        fixedInputData = new uint32_t[WORDS_PER_FID];
        GenerateAesFixedInputData();
    }

    uint32_t outBufBytes = blocks * (AES_GCM_KEY_SZ + AES_GCM_IV_SZ);
    outBufKDF = new uint32_t[outBufBytes];
    SetKdfLogFile(options.GetEncryptionDumpFlag());

    uint32_t ret  = kdf->CounterModeKDF(aesSeed, fixedInputData, fixedInputDataByteLength, outBufKDF, outBufBytes);
    if (ret != 0)
    {
        LOG_ERROR("Error generating encryption keys from Counter Mode KDF.");
    }

    uint8_t aesKeyNext[AES_GCM_KEY_SZ];
    uint8_t aesIvNext[AES_GCM_IV_SZ];
    for (x = 0; x<blocks; x++)
    {
        memcpy(aesKeyNext, &outBufKDF[(x * 11)], AES_GCM_KEY_SZ);
        aesKeyVec.push_back(ConvertKeyIvToString(aesKeyNext, AES_GCM_KEY_SZ).c_str());
        if (GetAesKey() == NULL)
        {
            SetAesKey(aesKeyNext);
        }

        memcpy(aesIvNext, &outBufKDF[(x * 11) + WORDS_PER_AES_KEY], AES_GCM_IV_SZ);
        aesIvVec.push_back(ConvertKeyIvToString(aesIvNext, AES_GCM_IV_SZ).c_str());
        if (GetIv() == NULL)
        {
            SetIv(aesIvNext);
        }
    }

    if (useOptionalKey == true)
    {
        if (GetAesOptKey() == NULL)
        {
            SetAesOptKey(aesKeyNext);
        }
    }
}

/******************************************************************************/
void ZynqMpEncryptionContext::ChunkifyAndEncrypt(Options& options, const uint8_t *inBuf, uint32_t inLen, uint8_t* outBuf, uint32_t& outLen)
{
    std::vector<uint32_t> blockList = options.bifOptions->GetEncryptionBlocksList();
    uint8_t *aesIv = new uint8_t[AES_GCM_IV_SZ];
    uint8_t *aesKey = new uint8_t[AES_GCM_KEY_SZ];
    uint8_t *aesOptKey = new uint8_t[AES_GCM_KEY_SZ];
    uint8_t *aesIVNext = new uint8_t[AES_GCM_IV_SZ];
    uint8_t *aesKeyNext = new uint8_t[AES_GCM_KEY_SZ];

    GetEncryptionKeys(options, aesKey, aesOptKey, aesIv);

    GetNextKey(aesKeyNext, 1);
    GetNextIv(aesIVNext, 1);

    if (partNum > 0) {
        uint8_t *x = new uint8_t[AES_GCM_IV_SZ];
        memset(x, 0, AES_GCM_IV_SZ);
        *(x + AES_GCM_IV_SZ - 1) = partNum;
        *(aesIv + AES_GCM_IV_SZ - 1) = *(aesIv + AES_GCM_IV_SZ - 1) + *(x + AES_GCM_IV_SZ - 1);
        delete[] x;
    }

    // Extract the first block size
    uint32_t nextBlkSize = (blockList.empty()) ? inLen : blockList[0];
    nextBlkSize = (nextBlkSize > inLen) ? inLen : nextBlkSize + 0;

    uint8_t secureHdr_in[AES_GCM_KEY_SZ + AES_GCM_IV_SZ + NUM_BYTES_PER_WORD];


    uint32_t charZero = 0;
    if (options.bifOptions->GetAesOptKeyFlag())
    {
        //Write the optional key from .nky file
        memcpy(secureHdr_in, aesOptKey, AES_GCM_KEY_SZ);
        if (isBootloader == false)
        {
            /* If optional key is enabled, then aesKey (boot key) needs to be used only encrypt the secure header
            Next block is encrypted using Opt Key.
            For all partitions, other than bootloader, don't use aesKey (bootkey), always use optional key
            Reason: The main idea to use optional key is to minimize the time of bootkey usage during booting process */
            memcpy(aesKey, aesOptKey, AES_GCM_KEY_SZ);
            memset(secureHdr_in, 0, AES_GCM_KEY_SZ);
            memcpy(secureHdr_in, aesKeyNext, AES_GCM_KEY_SZ);
        }
    }
    else
    {
        /*Security Vulnerability : Reusing same AES / IV Key Pair*/
        //Write Dummy key
        memset(secureHdr_in, charZero, AES_GCM_KEY_SZ);
        //Write Next key
        if (isBootloader == false)
        {
            memcpy(secureHdr_in, aesKeyNext, AES_GCM_KEY_SZ);
        }
    }

    // Block 0 IV(random generated), and the Block0 word size
    memcpy(secureHdr_in + AES_GCM_KEY_SZ, aesIVNext, AES_GCM_IV_SZ);
    WriteLittleEndian32(secureHdr_in + AES_GCM_KEY_SZ + AES_GCM_IV_SZ, nextBlkSize / NUM_BYTES_PER_WORD);

    int ct_len;
    uint8_t gcm_tag[AES_GCM_TAG_SZ];

    /* Encrypt the Secure Header with device key and starting IV */
    LOG_TRACE("Encrypting the Secure Header");
    uint8_t* ptr = outBuf;
    encryptionAlgorithm->AesGcm256Encrypt(secureHdr_in, SECURE_HDR_SZ, aesKey, aesIv, NULL, 0, ptr, ct_len, gcm_tag);

    /* Attach the AES-GCM generated Hash Tag to end of the block */
    memcpy(outBuf + ct_len, gcm_tag, AES_GCM_TAG_SZ);

    uint32_t outPtr = ct_len + AES_GCM_TAG_SZ;

    uint8_t secureHdr_out[1024];
    int pt_len;
    uint32_t length = 0;
    if (options.GetEncryptionDumpFlag())
    {
        uint32_t i = 0;
        VERBOSE_OUT << std::endl << "    Secure Header";
        VERBOSE_OUT << std::endl << "        AES Key : ";
        for (i = 0; i<AES_GCM_KEY_SZ; i++)
            VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << uint32_t(aesKey[i]);

        VERBOSE_OUT << std::endl << "        AES IV  : ";
        for (i = 0; i<AES_GCM_IV_SZ; i++)
            VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << uint32_t(aesIv[i]);

        VERBOSE_OUT << std::endl << "        Length  : ";
        VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << SECURE_HDR_SZ;

        VERBOSE_OUT << std::endl << "        Next Key, IV and length stored in Secure Header: ";
        VERBOSE_OUT << std::endl << "            Next Key : ";
        for (i = 0; i<AES_GCM_KEY_SZ; i++)
            VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << uint32_t(secureHdr_in[i]);

        VERBOSE_OUT << std::endl << "            Next IV  : ";
        for (i = 0; i<AES_GCM_IV_SZ; i++)
            VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << uint32_t(secureHdr_in[i + AES_GCM_KEY_SZ]);

        VERBOSE_OUT << std::endl << "            Length   : ";
        length = ReadLittleEndian32(secureHdr_in + AES_GCM_KEY_SZ + AES_GCM_IV_SZ);
        VERBOSE_OUT << std::hex << length * 4;
        /*
        VERBOSE_OUT << std::endl << "        GCM Tag : ";
        for (i = 0; i<AES_GCM_TAG_SZ; i++)
        VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << uint32_t(gcm_tag[i]);
        VERBOSE_OUT << std::endl;
        */

        //AES-GCM Decryption for verification purpose 
        encryptionAlgorithm->AesGcm256Decrypt(secureHdr_out, pt_len, aesKey, aesIv, NULL, 0, outBuf, SECURE_HDR_SZ, gcm_tag);
        int ret = memcmp(secureHdr_in, secureHdr_out, SECURE_HDR_SZ);
        if (ret == 0)
        {
            //VERBOSE_OUT << "        Encrypted secure header was successfully decrypted and tag matched\n";
        }
    }

    uint32_t bytesWritten = 0;
    uint32_t inPtr = 0;
    uint32_t blkPtr = 1;

    //Take the next random generated IV for encryption
    /* Secure Header and Block 0 will be using the same IV, to have equal number of Keys
    and IVs from Key File. */
    /*Security Vulnerability : Reusing same AES / IV Key Pair*/
    memcpy(aesIv, aesIVNext, AES_GCM_IV_SZ);
    if (options.bifOptions->GetAesOptKeyFlag())
    {
        if (isBootloader == false)
        {
            memcpy(aesKey, aesKeyNext, AES_GCM_KEY_SZ);
        }
        else
        {
            memcpy(aesKey, aesOptKey, AES_GCM_KEY_SZ);
        }
    }
    else
    {
        if (isBootloader == false)
        {
            memcpy(aesKey, aesKeyNext, AES_GCM_KEY_SZ);
        }
    }

    while (bytesWritten < inLen)
    {
        //Update the block size for current block and extract the next block size
        uint32_t currBlkSize = nextBlkSize;
        bytesWritten += currBlkSize;
        nextBlkSize = (blockList.size() > blkPtr) ? blockList[blkPtr] : inLen - bytesWritten;
        nextBlkSize = ((nextBlkSize + bytesWritten) > inLen) ? (inLen - bytesWritten) : (nextBlkSize + 0);

        /* Get next key and IV(random for next key from keyfile) for next block */
        if (nextBlkSize == 0)
        {
            memset(aesKeyNext, 0, AES_GCM_KEY_SZ);
            memset(aesIVNext, 0, AES_GCM_IV_SZ);
        }
        else
        {
            GetNextKey(aesKeyNext, blkPtr + 1);
            GetNextIv(aesIVNext, blkPtr + 1);
        }

        uint8_t *gcm_pt = new uint8_t[currBlkSize + AES_GCM_KEY_SZ + AES_GCM_IV_SZ + NUM_BYTES_PER_WORD];
        //Prepare the buffer for encryption
        //Actual block data + Next Block Key + Next Block IV + Next Block Word Size
        memcpy(gcm_pt, inBuf + inPtr, currBlkSize);
        inPtr += currBlkSize;
        memcpy(gcm_pt + currBlkSize, aesKeyNext, AES_GCM_KEY_SZ);
        memcpy(gcm_pt + currBlkSize + AES_GCM_KEY_SZ, aesIVNext, AES_GCM_IV_SZ);
        WriteLittleEndian32(gcm_pt + currBlkSize + AES_GCM_KEY_SZ + AES_GCM_IV_SZ, nextBlkSize / NUM_BYTES_PER_WORD);

        //Encrypt the consolidated block
        LOG_TRACE("Encrypting the block %d of size 0x%x of partition number %d", blkPtr, currBlkSize, partNum);
        encryptionAlgorithm->AesGcm256Encrypt(gcm_pt, currBlkSize + SECURE_HDR_SZ, aesKey, aesIv, NULL, 0, outBuf + outPtr, ct_len, gcm_tag);
        memcpy(outBuf + outPtr + ct_len, gcm_tag, AES_GCM_TAG_SZ);

        uint8_t* inBuf_out = new uint8_t[ct_len + AES_GCM_TAG_SZ];
        if (options.GetEncryptionDumpFlag())
        {
            uint32_t i = 0;
            /*VERBOSE_OUT << std::endl << std::dec << "Unencrypted Bootimage Data - block-" << currBlk << " Length-" << (currBlkSize+SECURE_HDR_SZ) << std::endl;
            int size = currBlkSize+SECURE_HDR_SZ;
            for(i=0; i<(currBlkSize+SECURE_HDR_SZ); i++)
            VERBOSE_OUT << "0x" << std::setfill('0') << std::setw(2) << std::hex << uint32_t(gcm_pt[i]) << " ";
            VERBOSE_OUT << std::endl;
            */
            VERBOSE_OUT << std::endl << "    Block " << blkPtr - 1;
            VERBOSE_OUT << std::endl << "        AES Key : ";
            for (i = 0; i<AES_GCM_KEY_SZ; i++)
                VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << uint32_t(aesKey[i]);

            VERBOSE_OUT << std::endl << "        AES IV  : ";
            for (i = 0; i<AES_GCM_IV_SZ; i++)
                VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << uint32_t(aesIv[i]);

            VERBOSE_OUT << std::endl << "        Length  : ";
            VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << currBlkSize;

            VERBOSE_OUT << std::endl << "        Next Key, IV and length stored in Block " << blkPtr - 1;
            VERBOSE_OUT << std::endl << "            Next Key : ";
            for (i = 0; i<AES_GCM_KEY_SZ; i++)
                VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << uint32_t(gcm_pt[i + currBlkSize]);

            VERBOSE_OUT << std::endl << "            Next IV  : ";
            for (i = 0; i<AES_GCM_IV_SZ; i++)
                VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << uint32_t(gcm_pt[i + currBlkSize + AES_GCM_KEY_SZ]);

            VERBOSE_OUT << std::endl << "            Length   : ";
            length = ReadLittleEndian32(gcm_pt + currBlkSize + AES_GCM_KEY_SZ + AES_GCM_IV_SZ);
            VERBOSE_OUT << std::hex << length * 4;
            /*
            VERBOSE_OUT << std::endl << "        GCM Tag : ";
            for (i = 0; i<AES_GCM_TAG_SZ; i++)
            VERBOSE_OUT << std::setfill('0') << std::setw(2) << std::hex << uint32_t(gcm_tag[i]);
            VERBOSE_OUT << std::endl;
            */
            encryptionAlgorithm->AesGcm256Decrypt(inBuf_out, pt_len, aesKey, aesIv, NULL, 0, outBuf + outPtr, ct_len, gcm_tag);
            int ret = memcmp(gcm_pt, inBuf_out, pt_len);
            if (ret == 0)
            {
                //VERBOSE_OUT << std::dec << "        Encrypted block " << currBlk++ << " was successfully decrypted and tag matched\n";
            }
            else
            {
                //VERBOSE_OUT << std::dec << "        Encrypted block " << currBlk++ << " was failed in decryption and tag matching\n";
            }
        }

        //Update the current key & IV 
        outPtr += ct_len + AES_GCM_TAG_SZ;
        memcpy(aesIv, aesIVNext, AES_GCM_IV_SZ);
        memcpy(aesKey, aesKeyNext, AES_GCM_KEY_SZ);

        delete[] gcm_pt;
        delete[] inBuf_out;
        blkPtr++;
    }

    outLen = outPtr;
    delete[] aesIv;
    delete[] aesKey;
    delete[] aesIVNext;
    delete[] aesKeyNext;
    delete[] aesOptKey;
}

/******************************************************************************/
void ZynqMpEncryptionContext::Process(BootImage& bi, PartitionHeader* partHdr)
{
    Options& options = bi.options;
    static bool deviceKeyStored = false;

    LOG_INFO("Encrypting the partition - %s", partHdr->partition->section->Name.c_str());

    std::vector<uint32_t> encrBlocks = partHdr->imageHeader->GetEncrBlocksList();
    uint32_t defEncrBlocks = partHdr->imageHeader->GetDefaultEncrBlockSize();
    Binary::Length_t lastBlock = 0;
    uint32_t totalencrBlocks = GetTotalEncryptionBlocks(partHdr->partition->section->Length, encrBlocks, defEncrBlocks, &lastBlock);

    options.bifOptions->GetEncryptionBlocksList().clear();

    for (uint32_t itr = 0; itr < totalencrBlocks; itr++)
    {
        if (itr < encrBlocks.size())
        {
            options.bifOptions->InsertEncryptionBlock(encrBlocks[itr]);
        }
        else if (defEncrBlocks != 0)
        {
            options.bifOptions->InsertEncryptionBlock(defEncrBlocks);
        }
        else if ((itr == totalencrBlocks - 1) && (lastBlock != 0))
        {
            options.bifOptions->InsertEncryptionBlock(lastBlock);
        }
    }

    /* Get the key file or keys (Key0, StartCBC, HMAC) passed from command line */
    SetAesFileName(partHdr->GetPartitionAesKeyFile());
    LOG_INFO("Key file - %s", aesFilename.c_str());
    if (partHdr->GenerateAesKeyFlag())
    {
        std::ifstream keyFile(aesFilename);
        bool fileExists = keyFile.good();
        if (!fileExists)
        {
            if (deviceKeyStored)
            {
                aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                aesIv = new uint32_t[AES_GCM_IV_SZ / 4];
                aesOptKey = new uint32_t[AES_GCM_KEY_SZ / 4];

                memcpy(aesKey, bi.deviceKey, AES_GCM_KEY_SZ);
                memcpy(aesIv, bi.firstIv, AES_GCM_IV_SZ);
                memcpy(aesOptKey, bi.firstOptKey, AES_GCM_KEY_SZ);
            }
            GenerateEncryptionKeyFile(aesFilename, options);
        }
    }

    bi.InsertEncryptionKeyFile(aesFilename);
    CheckForSameAesKeyFiles(bi.GetEncryptionKeyFileVec());

    ReadEncryptionKeyFile(aesFilename);
    options.SetDevicePartName(deviceName);

    CheckForExtraKeyIVPairs(totalencrBlocks, partHdr->partition->section->Name);

    if (!aesSeedexits && aesKeyVec.size() != 1)
    {
        if (totalencrBlocks + 1 > aesKeyVec.size())
        {
            LOG_ERROR("AES Key file has less keys than the number of blocks to be encrypted in %s.", partHdr->partition->section->Name.c_str());
        }
        if (totalencrBlocks + 1 > aesIvVec.size())
        {
            LOG_ERROR("AES Key file has less IVs than the number of blocks to be encrypted in %s.", partHdr->partition->section->Name.c_str());
        }
    }
    else
    {
        if (deviceKeyStored)
        {
            if (aesKey == NULL)
            {
                aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                memcpy(aesKey, bi.deviceKey, AES_GCM_KEY_SZ);
            }
            if (aesKeyVec.size() == 0)
            {
                aesKeyVec.push_back(ConvertKeyIvToString((uint8_t *)aesKey, AES_GCM_KEY_SZ).c_str());
            }
            if (aesIv == NULL)
            {
                aesIv = new uint32_t[AES_GCM_IV_SZ / 4];
                memcpy(aesIv, bi.firstIv, AES_GCM_IV_SZ);
            }
            if (aesIvVec.size() == 0)
            {
                aesIvVec.push_back(ConvertKeyIvToString((uint8_t *)aesIv, AES_GCM_IV_SZ).c_str());
            }
            if (aesOptKey == NULL)
            {
                aesOptKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                memcpy(aesOptKey, bi.firstOptKey, AES_GCM_KEY_SZ);
            }
        }
        GenerateRemainingKeys(options, aesFilename);
    }

    CheckForRepeatedKeyIVPairs(bi.GetEncryptionKeyFileVec(), true);

    if (!deviceKeyStored)
    {
        bi.deviceKey = new uint32_t[AES_GCM_KEY_SZ / 4];
        bi.firstIv = new uint32_t[AES_GCM_IV_SZ / 4];
        bi.firstOptKey = new uint32_t[AES_GCM_KEY_SZ / 4];
        if (aesKey != NULL)
        {
            memcpy(bi.deviceKey, aesKey, AES_GCM_KEY_SZ);
        }
        if (aesIv != NULL)
        {
            memcpy(bi.firstIv, aesIv, AES_GCM_IV_SZ);
        }
        if (aesOptKey != NULL)
        {
            memcpy(bi.firstOptKey, aesOptKey, AES_GCM_KEY_SZ);
        }
        deviceKeyStored = true;
    }
    else
    {
        if ((aesKey != NULL) && memcmp(bi.deviceKey, aesKey, AES_GCM_KEY_SZ))
        {
            LOG_ERROR("Key 0 should be same across all nky files - see 'bootgen -bif_help aeskeyfile' for more info.");
        }
        if ((aesIv != NULL) && memcmp(bi.firstIv, aesIv, AES_GCM_IV_SZ))
        {
            LOG_ERROR("IV 0 should be same across all nky files - see 'bootgen -bif_help aeskeyfile' for more info.");
        }
        if ((aesOptKey != NULL) && memcmp(bi.firstOptKey, aesOptKey, AES_GCM_KEY_SZ))
        {
            LOG_ERROR("Key Opt should be same across all nky files - see 'bootgen -bif_help aeskeyfile' for more info.");
        }
    }

    // For copying SCR HDR IV into Boot Header
    const uint32_t* tmpIv = GetIv();
    if (tmpIv != NULL)
    {
        if (options.secHdrIv == NULL)
        {
            options.secHdrIv = (uint8_t*)malloc(BYTES_PER_IV);
        }
        memcpy_be(options.secHdrIv, tmpIv, BYTES_PER_IV);
    }

    uint32_t totalBlocksOverhead = (totalencrBlocks + 1) * 64;

    if (!Binary::CheckAddress(partHdr->partition->section->Length))
    {
        LOG_ERROR("Partition too large to encrypt");
    }

    isBootloader = partHdr->imageHeader->IsBootloader();
    partNum = partHdr->GetpartitionNum();
    if (options.GetEncryptionDumpFlag())
    {
        bi.options.aesLogFile.open(bi.options.aesLogFilename, std::fstream::app);
        VERBOSE_OUT << std::endl << "------------------------------------";
        VERBOSE_OUT << std::endl << " Partition No.  : " << partNum;
        VERBOSE_OUT << std::endl << " Partition Name : " << partHdr->partition->section->Name;
        VERBOSE_OUT << std::endl << " Key file       : " << StringUtils::BaseName(aesFilename);
        VERBOSE_OUT << std::endl << "------------------------------------";
    }
    /* Encrypt the stream */
    if (isBootloader && partHdr->imageHeader->GetPmuFwSizeIh() != 0)
    {
        // PMU Encryption
        uint32_t encrPmuByteLength;
        uint32_t estimatedEncrPmuLength = partHdr->imageHeader->GetPmuFwSizeIh() + totalBlocksOverhead;
        uint8_t* encrPmuDataBuffer = new uint8_t[estimatedEncrPmuLength];
        LOG_INFO("Encrypting the PMU Firmware");
        ChunkifyAndEncrypt(options,
            partHdr->partition->section->Data,
            partHdr->imageHeader->GetPmuFwSizeIh(),
            encrPmuDataBuffer /* out*/,
            encrPmuByteLength /* out */);

        if (estimatedEncrPmuLength < encrPmuByteLength)
        {
            LOG_ERROR("Encryption Error!!! - PMU Buffer overflow");
        }

        // FSBL Encryption
        uint32_t encrFsblByteLength;
        uint32_t estimatedEncrFsblLength = partHdr->imageHeader->GetFsblFwSizeIh() + totalBlocksOverhead;
        uint8_t* encrFsblDataBuffer = new uint8_t[estimatedEncrFsblLength];
        LOG_INFO("Encrypting the FSBL");
        ChunkifyAndEncrypt(options,
            partHdr->partition->section->Data + partHdr->imageHeader->GetPmuFwSizeIh(),
            partHdr->imageHeader->GetFsblFwSizeIh(),
            encrFsblDataBuffer /* out*/,
            encrFsblByteLength /* out */);

        if (estimatedEncrFsblLength < encrFsblByteLength)
        {
            LOG_ERROR("Encryption Error!!! - FSBL Buffer overflow");
        }

        partHdr->partition->section->IncreaseLengthAndPadTo(encrPmuByteLength + encrFsblByteLength, 0x0);

        memcpy(partHdr->partition->section->Data, encrPmuDataBuffer, encrPmuByteLength);
        memcpy(partHdr->partition->section->Data + encrPmuByteLength, encrFsblDataBuffer, encrFsblByteLength);

        partHdr->imageHeader->SetTotalPmuFwSizeIh(encrPmuByteLength);
        partHdr->imageHeader->SetTotalFsblFwSizeIh(encrFsblByteLength);
        partHdr->partition->section->Length = encrFsblByteLength + encrPmuByteLength;

        LOG_INFO("Encrypted the partition - %s", partHdr->partition->section->Name.c_str());
        bi.options.CloseEncryptionDumpFile();
        delete[] encrFsblDataBuffer;
        delete[] encrPmuDataBuffer;
        return;
    }
    else
    {
        uint32_t encryptedLength;
        uint32_t estimatedEncrLength = partHdr->partition->section->Length + totalBlocksOverhead;
        uint8_t* encryptedDataBuffer = new uint8_t[estimatedEncrLength];

        ChunkifyAndEncrypt(options,
            partHdr->partition->section->Data,
            (uint32_t)partHdr->partition->section->Length,
            encryptedDataBuffer /* out*/,
            encryptedLength /* out */);

        partHdr->partition->section->IncreaseLengthAndPadTo(encryptedLength, 0x0);
        memcpy(partHdr->partition->section->Data, encryptedDataBuffer, encryptedLength);
        partHdr->partition->section->Length = encryptedLength;
        partHdr->imageHeader->SetTotalFsblFwSizeIh(encryptedLength);

        LOG_INFO("Encrypted the partition - %s", partHdr->partition->section->Name.c_str());
        bi.options.CloseEncryptionDumpFile();
        delete[] encryptedDataBuffer;
        return;
    }
}

/******************************************************************************/
void ZynqMpEncryptionContext::GenerateGreyKey()
{
#ifdef ENABLE_OBFUSCATED_KEY
    ReadEncryptionKeyFile(aesFilename);
    uint8_t *bhIv = new uint8_t[AES_GCM_IV_SZ];
    uint8_t *redKey = new uint8_t[AES_GCM_KEY_SZ];

    if (aesKey != NULL)
    {
        memcpy_be(redKey, aesKey, AES_GCM_KEY_SZ);
    }
    else
    {
        LOG_ERROR("Encryption Error !!!\n           Key 0 does not exist in the AES key file ");
    }

    ReadBhIv(bhIv);
    std::string filename = "obfuscatedkey.txt";
    obfs key(redKey, bhIv, metalFile.c_str(), filename.c_str());
    obfsk((void*)&key);
    LOG_TRACE("Obfuscated key file : '%s' generated successfully", filename.c_str());
    LOG_INFO("Obfuscated Key generated successfully");

    delete[] bhIv;
    delete[] redKey;
#else
    LOG_ERROR("Obfuscated Key generation is not supported. Please refer to README on how to enable the same.");
#endif
}

/******************************************************************************/
void ZynqMpEncryptionContext::ReadBhIv(uint8_t* bhIv)
{
    FileImport fileReader;
    uint8_t* ivData = new uint8_t[AES_GCM_IV_SZ];
    memset(ivData, 0, AES_GCM_IV_SZ);

    if (bhKekIVFile != "")
    {
        if (!fileReader.LoadHexData(bhKekIVFile, ivData, AES_GCM_IV_SZ))
        {
            LOG_ERROR("Invalid data bytes for BH IV.\n           Expected length for BH IV is 12 bytes");
        }
        memcpy(bhIv, ivData, AES_GCM_IV_SZ);
    }
    else {
        LOG_ERROR("Key Generation Error !!!\n          BH IV must be specified in BIF file for obfuscated key generation. Use '[bh_key_iv]'");
    }

    delete[] ivData;
}
