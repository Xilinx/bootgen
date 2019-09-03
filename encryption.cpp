/******************************************************************************
* Copyright 2015 - 2019 Xilinx, Inc.
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


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
void EncryptionContext::SetDeviceName(const std::string& deviceName0)
{
    deviceName = deviceName0;
}

/******************************************************************************/
void EncryptionContext::PackHex(const std::string&	hexString, uint8_t* hexData)
{
    uint32_t hexStringLength = (uint32_t)hexString.size();

    if (hexStringLength & 1)
    {
        LOG_DEBUG(DEBUG_STAMP, "Hex String - %s - does not have even no. of hex digits", hexString.c_str());
        LOG_ERROR("Error parsing encryption key");
    }

    for (uint32_t i = 0;i<hexStringLength;i += 2)
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
void EncryptionContext::SetBhIvFile(std::string file)
{
    bhIvFile = file;
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
std::string EncryptionContext::GetBhIvFile(void)
{
    return bhIvFile;
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