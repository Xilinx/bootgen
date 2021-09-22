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

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "encryption.h"
#include "bootimage.h"
#include "encryptutils.h"
#include "options.h"
#include "bifoptions.h"
#include "encryptutils.h"
#include <openssl/rand.h>

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
void EncryptionContext::GenerateEncryptionKeyFile(const std::string & baseFileName, Options & options)
{
    LOG_TRACE("Generating the AES key file");
    std::string devicePartName = options.GetDevicePartName();
    if (devicePartName != "")
    {
        SetDeviceName(devicePartName);
    }
    else
    {
        LOG_ERROR("Partname must be specified with -p <partname> option in command line for generating a key file");
    }

    if (GetAesSeed() == NULL)
    {
        aesSeed = new uint32_t[WORDS_PER_AES_KEY];
    }
    GenerateAesSeed();

    if (GetFixedInputData() == NULL)
    {
        fixedInputData = new uint32_t[WORDS_PER_FID];
    }
    GenerateAesFixedInputData();

    bool useOptionalKey = options.bifOptions->GetAesOptKeyFlag();

    /* The extra 1 Key pair is for Secure Header */

    uint32_t outBufBytes = (options.bifOptions->GetEncryptionBlocksList().size() + useOptionalKey + 1) * (AES_GCM_KEY_SZ + AES_GCM_IV_SZ);
    outBufKDF = new uint32_t[outBufBytes];

    SetKdfLogFile(options.GetEncryptionDumpFlag());
    uint32_t ret = kdf->CounterModeKDF(aesSeed, fixedInputData, fixedInputDataByteLength, outBufKDF, outBufBytes);
    if (ret != 0)
    {
        LOG_ERROR("Error generating encryption keys from Counter Mode KDF.");
    }

    WriteEncryptionKeyFile(baseFileName, useOptionalKey, options.bifOptions->GetEncryptionBlocksList().size() + 1);
}

/******************************************************************************/
void EncryptionContext::SetDeviceName(const std::string& deviceName0)
{
    deviceName = deviceName0;
}

/******************************************************************************/
void EncryptionContext::PackHex(const std::string& hexString, uint8_t* hexData)
{
    uint32_t hexStringLength = (uint32_t)hexString.size();

    if (hexStringLength & 1)
    {
        LOG_DEBUG(DEBUG_STAMP, "Hex String - %s - does not have even no. of hex digits", hexString.c_str());
        LOG_ERROR("Error parsing encryption key");
    }

    for (uint32_t i = 0; i < hexStringLength; i += 2)
    {
        std::string byte = hexString.substr(i, 2);
        if (!isxdigit(byte[0]) || !isxdigit(byte[1]))
        {
            LOG_DEBUG(DEBUG_STAMP, "Hex String - %s - is has a non hex digit", hexString.c_str());
            LOG_ERROR("Error parsing encryption key");
        }
        *hexData++ = (uint8_t)strtoul(byte.c_str(), NULL, 16);
    }
}

/******************************************************************************/
void EncryptionContext::SetAesKeyString(const std::string& key)
{
    uint8_t hexData[256];

    if (key.size() != (WORDS_PER_AES_KEY * 8))
    {
        LOG_DEBUG(DEBUG_STAMP, "AES key size - %d", key.size());
        LOG_ERROR("An AES key must be 256 bits long - %s", key.c_str());
    }

    PackHex(key, hexData);
    SetAesKey(hexData);
}

/******************************************************************************/
std::string EncryptionContext::ConvertKeyIvToString(uint8_t * keyIv, uint8_t size)
{
    std::stringstream keyIvString;
    for (uint32_t i = 0; i < size; i++)
    {
        keyIvString << std::setfill('0') << std::setw(2) << std::hex << uint32_t(keyIv[i]);
    }
    return keyIvString.str();
}

/******************************************************************************/
void EncryptionContext::SetAesFileName(std::string file)
{
    aesFilename = file;
}

/******************************************************************************/
void EncryptionContext::SetMetalKeyFile(std::string file)
{
    metalFile = file;
}

/******************************************************************************/
void EncryptionContext::SetBHKekIVFile(std::string file)
{
    bhKekIVFile = file;
}

/******************************************************************************/
void EncryptionContext::SetBbramKekIVFile(std::string file)
{
    bbramKekIVFile = file;
}

/******************************************************************************/
void EncryptionContext::SetEfuseKekIVFile(std::string file)
{
    efuseKekIVFile = file;
}

/******************************************************************************/
void EncryptionContext::SetEfuseUserKek0IVFile(std::string file)
{
    efuseUserKek0IVFile = file;
}

/******************************************************************************/
void EncryptionContext::SetEfuseUserKek1IVFile(std::string file)
{
    efuseUserKek1IVFile = file;
}

/******************************************************************************/
std::string EncryptionContext::GetAesFileName()
{
    return aesFilename;
}

/******************************************************************************/
std::string EncryptionContext::GetMetalKeyFile(void)
{
    return metalFile;
}

/******************************************************************************/
std::string EncryptionContext::GetBHKekIVFile(void)
{
    return bhKekIVFile;
}

/******************************************************************************/
std::string EncryptionContext::GetBbramKekIVFile(void)
{
    return bbramKekIVFile;
}

/******************************************************************************/
std::string EncryptionContext::GetEfuseKekIVFile(void)
{
    return efuseKekIVFile;
}

/******************************************************************************/
std::string EncryptionContext::GetEfuseUserKek0IVFile(void)
{
   return efuseUserKek0IVFile;
}

/******************************************************************************/
std::string EncryptionContext::GetEfuseUserKek1IVFile(void)
{
    return efuseUserKek1IVFile;
}

/******************************************************************************/
void EncryptionContext::GenerateAesSeed(void)
{
    uint8_t seed[BYTES_PER_AES_KEY];
    RAND_bytes(seed, WORDS_PER_AES_KEY * sizeof(uint32_t));
    SetAesSeed(seed);

    static bool warningGiven = false;
    if (!warningGiven)
    {
        LOG_WARNING("Seed needed for KDF is generated using RAND_Bytes. To have a secure keying material, we encourage the users to provide the Seed. \n\t   Refer to UG1283 for more details");
        warningGiven = true;
    }
}

/******************************************************************************/
void EncryptionContext::SetAesFixedInputData(const uint8_t* key, uint32_t bytes)
{
    fixedInputData = new uint32_t[bytes/4];
    fixedInputDataByteLength = bytes;

    for (uint32_t index = 0; index < bytes/4; index++)
    {
        fixedInputData[index] = ReadBigEndian32(key);
        key += sizeof(uint32_t);
    }
}

/******************************************************************************/
void EncryptionContext::SetAesFixedInputDataString(const std::string& key)
{
    if (key.size() / 2 != 60)
    {
        LOG_DEBUG(DEBUG_STAMP, "Fixed Input Data size - %d", key.size() / 2);
        LOG_ERROR("An AES Fixed Input Data must be 60 Bytes long - %s", key.c_str());
    }

    uint8_t* hexData = new uint8_t[key.size()];
    PackHex(key, hexData);
    SetAesFixedInputData(hexData, key.size() / 2);
    delete[] hexData;
}

/******************************************************************************/
void EncryptionContext::GenerateAesFixedInputData(void)
{
    uint8_t fixedInputData[BYTES_PER_FID];
    RAND_bytes(fixedInputData, BYTES_PER_FID);
    SetAesFixedInputData(fixedInputData, BYTES_PER_FID);

    static bool warningGiven = false;
    if (!warningGiven)
    {
        LOG_WARNING("Fixed Input Data needed for KDF is generated using RAND_Bytes. To have a secure keying material, we encourage the users to provide the Fixed Input Data. \n\t   Refer to UG1283 for more details");
        warningGiven = true;
    }
}

/******************************************************************************/
void EncryptionContext::SetRandomSeed()
{
    /* Start the pseudo random sequence based on the passage of time so that
    we never get the same pseudo random sequence twice.
    This is done by XOR'ing the current time (which increments in seconds)
    with the number of CPU clocks (either microseconds or CPU clock ticks).
    This will create a number that will never repeat again (the date), but
    will be different every time this function is called (based on CPU clocks). */

    static int temp_var = 0;
    clock_t  currentClocks = clock();
    time_t   currentTime = time(NULL);
    uint32_t randomSeed = (uint32_t)(currentTime ^ currentClocks);
    randomSeed += temp_var++;
    srand(randomSeed);
}

/******************************************************************************/
static uint32_t GetRandomValue(uint32_t	maxValue)
{
    uint32_t returnValue;

    do
    {
        returnValue = (rand() / (int)(((unsigned)RAND_MAX + 1) / maxValue));
    } while (returnValue > maxValue);

    return returnValue;
}

/******************************************************************************/
void EncryptionContext::SetKdfLogFile(bool encrdump)
{
    if (encrdump)
    {
        static bool kdfLogInit = false;
        if (!kdfLogInit)
        {
            kdfLogInit = true;
            std::ofstream remove("kdf_log.txt");
            kdf->SetKdfLogFilename("kdf_log.txt");
        }
    }
}

/******************************************************************************/
void EncryptionContext::GetRandomData(uint8_t* randomData, uint32_t randomDataByteLength)
{
    uint8_t randomSequenceNibble;
    uint8_t randomSequenceByte;
    uint8_t lastRandomSequenceByte;
    uint32_t randomByteLocation;
    uint32_t byteCount;
    uint32_t index;

    // Zero out the return buffer;
    for (index = 0; index < randomDataByteLength; index += 1)
    {
        randomData[index] = 0;
    }

    // Get pseudo random byte values to fill the return buffer.
    byteCount = randomDataByteLength;
    while (byteCount > 0)
    {
        // Never get the same byte value twice in a row.
        lastRandomSequenceByte = 0;
        do
        {
            // Combine two pseudo random nibbles into a byte.
            randomSequenceByte = 0;

            // Exclude a byte value of zero.
            while (randomSequenceByte == 0)
            {
                for (index = 0; index < 2; index += 1)
                {
                    // Get a pseudo random nibble between 0x0 and 0xF.
                    randomSequenceNibble = (uint8_t)GetRandomValue(0x10);

                    // Combine the two nibbles together into a bytes.
                    randomSequenceByte <<= 4;
                    randomSequenceByte |= randomSequenceNibble;
                }
            }
        } while (lastRandomSequenceByte == randomSequenceByte);

        lastRandomSequenceByte = randomSequenceByte;

        /* Set the pseudo random bytes values in random byte location order by getting a random byte location
        index of a byte location that hasn't been set yet */
        do
        {
            randomByteLocation = GetRandomValue(randomDataByteLength);
        } while (randomData[randomByteLocation] != 0);

        // Set the pseudo random byte value into the return buffer.
        randomData[randomByteLocation] = randomSequenceByte;

        // Prepare for the next byte loop interation.
        byteCount -= 1;
    }
}

/******************************************************************************/
uint32_t EncryptionContext::GetTotalEncryptionBlocks(Binary::Length_t partitionSize, std::vector<uint32_t> encrBlocks,
                                                     uint32_t defEncrBlockSize, Binary::Length_t* lastBlock)
{
    uint32_t defEncrBlocksCount = 0;

    /* Calculate the Size of partition covered with given blocks */
    Binary::Length_t encrBlocksSize = 0;
    for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
    {
        encrBlocksSize += encrBlocks[itr];
    }

    /* Calculate the number encryption blocks with the def block size given using (*), else would be a single block */
    if (partitionSize > encrBlocksSize)
    {
        if (defEncrBlockSize != 0)
        {
            defEncrBlocksCount = ((partitionSize - encrBlocksSize) / defEncrBlockSize);
            bool lastBlockExists = (((partitionSize - encrBlocksSize) % defEncrBlockSize) == 0 ? 0 : 1);
            if (lastBlockExists)
            {
                defEncrBlocksCount++;
                *lastBlock = (partitionSize - encrBlocksSize - ((defEncrBlocksCount - 1)*defEncrBlockSize));
            }
        }
        else
        {
            defEncrBlocksCount = 1;
            *lastBlock = partitionSize - encrBlocksSize;
        }
    }

    uint32_t totalencrBlocks = defEncrBlocksCount + encrBlocks.size();
    LOG_TRACE("Total no. of blocks to encrypt - %d", totalencrBlocks);

    return totalencrBlocks;
}

/******************************************************************************/
void EncryptionContext::CheckForSameAesKeyFiles(std::vector<std::string> aesKeyFileVec)
{
    uint32_t count = 0;
    for (uint32_t i = 0; i < aesKeyFileVec.size(); i++)
    {
        if (aesFilename.compare(aesKeyFileVec[i]) == 0)
        {
            count++;
        }
    }
    if (count > 1)
    {
        LOG_ERROR("Same .nky file is used to provide keys across multiple partitions.\n           Reuse of keys or key/IV pairs, both of which can create security vulnerability.");
    }
}

/******************************************************************************/
void EncryptionContext::CheckForExtraKeyIVPairs(uint32_t totalencrBlocks, std::string name)
{
    static bool extraKeys = false;
    static bool extraIvs = false;

    if (totalencrBlocks + 1 < aesKeyVec.size())
    {
        extraKeys = true;
        aesKeyVec.resize(totalencrBlocks + 1);
    }
    if (totalencrBlocks + 1 < aesIvVec.size())
    {
        extraIvs = true;
        aesIvVec.resize(totalencrBlocks + 1);
    }
    if (extraKeys && extraIvs)
    {
        LOG_WARNING("AES Key file has more Keys/IVs than the number of blocks to be encrypted in %s. \n           Extra keys/Ivs will be ignored.", name.c_str());
    }
    else if (extraKeys)
    {
        LOG_WARNING("AES Key file has more Keys than the number of blocks to be encrypted in %s. \n           Extra Keys will be ignored.", name.c_str());
    }
    else if (extraIvs)
    {
        LOG_WARNING("AES Key file has more IVs than the number of blocks to be encrypted in %s. \n           Extra Ivs will be ignored.", name.c_str());
    }
}

/******************************************************************************/
void EncryptionContext::CheckForRepeatedKeyIVPairs(std::vector<std::string> aesKeyFileVec, bool maskKey0IV0)
{
    static std::vector<std::vector<std::string> > aesKeyMasterVec;
    static std::vector<std::vector<std::string> > aesIvMasterVec;

    aesKeyMasterVec.push_back(aesKeyVec);
    aesIvMasterVec.push_back(aesIvVec);

    bool repeatedKey = false;
    bool repeatedIV = false;

    for (uint32_t i = 0; i < aesKeyVec.size(); i++)
    {
        for (uint32_t j = i + 1; j < aesKeyVec.size(); j++)
        {
            if (aesKeyVec[i] == aesKeyVec[j])
            {
                repeatedKey = true;
            }
            if (aesIvVec[i] == aesIvVec[j])
            {
                repeatedIV = true;
            }
            if (repeatedKey && repeatedIV)
            {
                LOG_MSG("           Key : %s", aesKeyVec[i].c_str());
                LOG_MSG("           IV : %s", aesIvVec[i].c_str());
                LOG_ERROR("Repeated usage of Key/IV pair is observed in %s.\n           Reuse of Key/IV pair creates security vulnerability.", aesFilename.c_str());
            }
            else
            {
                repeatedKey = repeatedIV = false;
            }
        }
    }

    if (maskKey0IV0)
    {
        aesKeyMasterVec[aesKeyMasterVec.size() - 1].erase(aesKeyMasterVec[aesKeyMasterVec.size() - 1].begin());
        aesIvMasterVec[aesIvMasterVec.size() - 1].erase(aesIvMasterVec[aesIvMasterVec.size() - 1].begin());
    }

    for (uint32_t i = 0; i < aesKeyMasterVec.size(); i++)
    {
        uint32_t j = aesKeyMasterVec.size() - 1;
        if (i != j)
        {
            for (uint32_t x = 0; x < aesKeyMasterVec[i].size(); x++)
            {
                for (uint32_t y = 0; y < aesKeyMasterVec[j].size(); y++)
                {
                    if (aesKeyMasterVec[i][x] == aesKeyMasterVec[j][y])
                    {
                        repeatedKey = true;
                    }
                    if (aesIvMasterVec[i][x] == aesIvMasterVec[j][y])
                    {
                        repeatedIV = true;
                    }
                    if (repeatedKey && repeatedIV)
                    {
                        LOG_MSG("           Key : %s", aesKeyMasterVec[i][x].c_str());
                        LOG_MSG("           IV : %s", aesIvMasterVec[j][y].c_str());
                        LOG_ERROR("Repeated usage of Key/IV pair is observed in %s, %s.\n           Reuse of Key/IV pair creates security vulnerability.", aesKeyFileVec[i].c_str(), aesKeyFileVec[j].c_str());
                    }
                    else
                    {
                        repeatedKey = repeatedIV = false;
                    }
                }
            }
        }
    }
}

/******************************************************************************/
void AesGcmEncryptionContext::AesGcm256Encrypt(unsigned char* gcm_pt, int pt_len, unsigned char* gcm_key,
    unsigned char* gcm_iv, unsigned char* gcm_aad, int aad_len,
    unsigned char* gcm_ct, int& ct_len, unsigned char* gcm_tag)
{
    EVP_CIPHER_CTX *ctx;
    int outlen, tmplen;

    ctx = EVP_CIPHER_CTX_new();

    /* Set cipher type and mode */
    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);

    /* Set IV length to 96 bits or 12 bytes  */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, NULL);

    /* Initialise key and IV */
    EVP_EncryptInit_ex(ctx, NULL, NULL, gcm_key, gcm_iv);

    /* Specify any AAD only if it is non-zero length*/
    if ((gcm_aad) && aad_len)
    {
        EVP_EncryptUpdate(ctx, NULL, &outlen, gcm_aad, aad_len);
    }

    /* Encrypt plaintext */
    EVP_EncryptUpdate(ctx, gcm_ct, &outlen, gcm_pt, pt_len);

    /* Finalise GCM */
    EVP_EncryptFinal_ex(ctx, gcm_ct + outlen, &tmplen);

    /* Get GCM tag */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, gcm_tag);

    EVP_CIPHER_CTX_free(ctx);
    ct_len = outlen + tmplen;
}

/******************************************************************************/
void AesGcmEncryptionContext::AesGcm256Decrypt(unsigned char* gcm_pt, int& pt_len, unsigned char* gcm_key,
    unsigned char* gcm_iv, unsigned char* gcm_aad, int aad_len,
    unsigned char* gcm_ct, int ct_len, unsigned char* gcm_tag)
{
    EVP_CIPHER_CTX *ctx;
    int outlen, tmplen;
    ctx = EVP_CIPHER_CTX_new();

    /* Select cipher */
    EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);

    /* Set IV length, omit for 96 bits */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, NULL);

    /* Specify key and IV */
    EVP_DecryptInit_ex(ctx, NULL, NULL, gcm_key, gcm_iv);

#if 1
    /* Set expected tag value. A restriction in OpenSSL 1.0.1c and earlier
    required the tag before any AAD or ciphertext */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, gcm_tag);
#endif

    /* Specify any AAD only if it is non-zero length*/
    if ((gcm_aad) && aad_len > 0) {
        EVP_DecryptUpdate(ctx, NULL, &outlen, gcm_aad, aad_len);
    }

    /* Decrypt plaintext */
    EVP_DecryptUpdate(ctx, gcm_pt, &outlen, gcm_ct, ct_len);

#if 0
    /* Set expected tag value. Works in OpenSSL 1.0.1d and later */
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, gcm_tag);
#endif

    /* Finalise: GCM */
    (void)EVP_DecryptFinal_ex(ctx, gcm_pt + outlen, &tmplen);

    EVP_CIPHER_CTX_free(ctx);

    pt_len = outlen + tmplen;
}