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

#include "encryptionkeys.h"
#include "encryption.h"
#include "encryption-zynqmp.h"
#include "encryption-versal.h"
#include "encryptutils.h"
#include "systemutils.h"
#include <openssl/rand.h>

/************************************************************************
Example of NIST SP 800-108 in Counter Mode
i is chosen to be 32 bits. This allows the 128(2^32 -1) bits of keying material can be generated.
L will be 40 bits which allows for the full amount of i. So Ko of 128(2^32 -1) bits of data can
be and is limited by the size of i.

If Ko is being used to generate both the 256-bit AES key and the 96-bit IV for AES-GCM, then
this allows for about 1.56 x 10^9 key/IV pairs. Looking at worst case scenarios,
key rolling every 32 AES encryption blocks would allow us to encrypt ~800 GB of data with a
single root key.

Because the CMAC implementation provided only handles even 128-bit blocks,
the length of [i]2 || Label || 0x00 || Context || [L]2 must always be
evenly divisible by 128 bits. The CMAC implementation needs fixed so the customer can
provide any Context and Label. Just as an example, these values are being used

i = 32 bits
Label = 256 bits
Separator = 8 bits
Context = 48 bits
L = 40 bits

These are just arbitrary values. Really anything can be used and anything should be
permitted for the Context and Label. The spec defines the separator. The i and L lengths
are defined to sensible values for our implementation.
divisible by 128.

The same cmac key is used as the earlier cmac example. This would be the customer provided
secret seed or root key.
*************************************************************************/

#define VERBOSE_OUT_KDF kdfLogFile

void EncryptionContext::CounterModeKDF(uint32_t blocks, std::string keyFilename, bool encrDump)
{
    uint8_t number_of_L_bytes = 8;
    uint8_t number_of_Separator_bytes = 1;

    if (GetAesSeed() == NULL)
    {
        GenerateAesSeed();
    }
    if (GetAesLabel() == NULL)
    {
        aesLabel = new uint8_t[aesLabelBytes];
        RAND_bytes(aesLabel, aesLabelBytes);
    }
    if (GetAesContext() == NULL)
    {
        aesContext = new uint8_t[aesContextBytes];
        RAND_bytes(aesContext, aesContextBytes);
    }

    kI = new uint8_t[BYTES_PER_AES_KEY];
    memcpy_be(kI, aesSeed, AES_GCM_KEY_SZ);

    /* An example generating 4 key/IV pairs will have 4*(32+12) number_of_Ko_bytes.
    This should be flexible based on number of keys or key/iv pair required
    or it can just pull off key/IV as needed; it is acceptable to discard
    32-bits and use 384 bits of Ko per 352-bits of key/iv pair. */

    uint32_t number_of_Ko_bytes = blocks * (AES_GCM_KEY_SZ + AES_GCM_IV_SZ);
    koLength = (blocks * (AES_GCM_KEY_SZ + AES_GCM_IV_SZ)) * 8;

    /* L will be 0x580 to generate 4 AES-256-GCM key/IV pairs which is 1408 bits = 4*(32+12) bytes */
    uint8_t* L = new uint8_t[number_of_L_bytes];
    memset(L, 0, number_of_L_bytes);
    reverse_copy(L, (uint64_t)koLength, number_of_L_bytes);

    if (!fixedInputDataExits)
    {
        /* Due to the CMAC implementation the Input must be evenly divisible by 128-bits; This
        * CMAC implementation should be fixed so that Context, Label can be aribitrary byte
        * lengths defined by the customer */
        fixedInputDataByteLength = aesLabelBytes + number_of_Separator_bytes + aesContextBytes + number_of_L_bytes;
        fixedInputData = new uint8_t[fixedInputDataByteLength];

        copy_array_uint8_t(&fixedInputData[0], aesLabel, aesLabelBytes);
        copy_array_uint8_t(&fixedInputData[aesLabelBytes], &Separator, number_of_Separator_bytes);
        copy_array_uint8_t(&fixedInputData[aesLabelBytes + number_of_Separator_bytes],
            aesContext, aesContextBytes);
        copy_array_uint8_t(&fixedInputData[aesLabelBytes + number_of_Separator_bytes + aesContextBytes],
            L, number_of_L_bytes);
    }

    KDF(blocks, keyFilename, encrDump);

    outBufKDF = new uint32_t[number_of_Ko_bytes / 4];

    // Copy the key
    for (uint32_t index = 0; index < number_of_Ko_bytes / 4; index++)
    {
        outBufKDF[index] = ReadBigEndian32(Ko + (index * sizeof(uint32_t)));
    }
    delete[] L;
}

/******************************************************************************/
void EncryptionContext::ParseKDFTestVectorFile(std::string filename)
{
    LOG_TRACE("Reading the Counter Mode KDF test file");
    std::ifstream testFile(filename.c_str());
    if (!testFile)
    {
        LOG_ERROR("Failure reading Counter Mode KDF test file - %s", filename.c_str());
    }
    
    while (testFile)
    {
        std::string word;
        testFile >> word;
        if (word == "")
        {
            return;
        }

        if (word == "L")
        {
            testFile >> word;
            if (word == "=")
            {
                testFile >> word;
                koLength = (uint32_t)strtoul(word.c_str(), NULL, 10);
            }
            else
            {
                LOG_ERROR("Error parsing KDF test vector file - %s \n\t Expected '=''.", filename.c_str());
            }
        }
        else if (word == "KI")
        {
            testFile >> word;
            if (word == "=")
            {
                testFile >> word;
                if (word.size() != AES_GCM_KEY_SZ * 2)
                {
                    LOG_ERROR("KI must be 256 bits long - %s", word.c_str());
                }
                kI = new uint8_t[(uint32_t)word.size()];
                PackHex(word, kI);
            }
            else
            {
                LOG_ERROR("Error parsing KDF test vector file - %s \n\t Expected '=''.", filename.c_str());
            }
        }
        else if (word == "FixedInputDataByteLen")
        {
            testFile >> word;
            if (word == "=")
            {
                testFile >> word;
                fixedInputDataByteLength = (uint32_t)strtoul(word.c_str(), NULL, 10);
            }
            else
            {
                LOG_ERROR("Error parsing KDF test vector file - %s \n\t Expected '=''.", filename.c_str());
            }
        }
        else if (word == "FixedInputData")
        {
            testFile >> word;
            if (word == "=")
            {
                testFile >> word;
                fixedInputData = new uint8_t[fixedInputDataByteLength];
                PackHex(word, fixedInputData);
            }
            else
            {
                LOG_ERROR("Error parsing KDF test vector file - %s \n\t Expected '=''.", filename.c_str());
            }
        }
        else if (word == "KO")
        {
            testFile >> word;
            if (word == "=")
            {
                testFile >> word;
                verifyKo = new uint8_t[koLength / 8];
                PackHex(word, verifyKo);
            }
            else
            {
                LOG_ERROR("Error parsing KDF test vector file - %s \n\t Expected '=''.", filename.c_str());
            }
        }
        else
        {
            LOG_ERROR("Error parsing KDF test vector file - %s \n\t   Expected 'L','KI','FixedInputDataByteLen', and 'FixedInputData'.", filename.c_str());
        }
    }
}

/******************************************************************************/
void EncryptionContext::CAVPonCounterModeKDF(std::string filename)
{
    ParseKDFTestVectorFile(filename);

    LOG_MSG("Generating Ko using Counter-Mode KDF...");
    std::cout << "KO = ";
    uint32_t number_of_Ko_bytes = koLength / 8;

    KDF(0,"",false);

    for (uint32_t y = 0; y<number_of_Ko_bytes; y++)
    {
        std::cout << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << uint32_t(Ko[y]);
    }
    std::cout << std::endl << std::endl;
    
    if (verifyKo != NULL)
    {
        if (memcmp(Ko, verifyKo, number_of_Ko_bytes) == 0)
        {
            std::cout << "Verified with given Ko" << std::endl;
        }
        else
        {
            std::cout << "Failed to verify with given Ko" << std::endl;
        }
    }
    std::cout << std::endl;
}

/******************************************************************************/
void EncryptionContext::KDF(uint32_t blocks, std::string keyFilename, bool encrDump)
{
    static bool kdfLogInit = false;
    static std::ofstream kdfLogFile;

    if (encrDump)
    {
        if (!kdfLogInit)
        {
            std::ofstream kdfExists("kdf_log.txt");
            if (kdfExists)
            {
                std::ofstream remove("kdf_log.txt");
            }
            kdfLogFile.open("kdf_log.txt", std::fstream::app);
            kdfLogInit = true;
            VERBOSE_OUT_KDF << std::endl << "Key Generation log from Counter-mode KDF" << std::endl;
        }
        VERBOSE_OUT_KDF << std::endl << "------------------------------------";
        VERBOSE_OUT_KDF << std::endl << " Generating Key/IV pairs for " << StringUtils::BaseName(keyFilename);
        VERBOSE_OUT_KDF << std::endl << "------------------------------------" << std::endl;
    }

    uint8_t number_of_i_bytes = 4;
    uint8_t i[4] = { 0x00, 0x00, 0x00, 0x00 };

    uint64_t number_of_Input_bytes = number_of_i_bytes + fixedInputDataByteLength;
    uint8_t* Input = new uint8_t[number_of_Input_bytes];

    uint32_t number_of_Ko_bytes = koLength / 8;
    Ko = new uint8_t[number_of_Ko_bytes];

    gen_cmac_subkeys(kI);

    copy_array_uint8_t(&Input[0], i, number_of_i_bytes);
    copy_array_uint8_t(&Input[number_of_i_bytes], fixedInputData, fixedInputDataByteLength);

    uint32_t cmac_iterations = number_of_Ko_bytes / 16;
    cmac_iterations += ((number_of_Ko_bytes % 16) == 0 ? 0 : 1);

    /* Run the KDF for the amount of data needed */
    uint32_t x;
    int32_t y;
    for (x = 0; x < cmac_iterations; x++)
    {
        /* Increment i. The first value used is 1 */
        y = 3;
        do {
            i[y]++;
            y--;
        } while (i[y + 1] == 0 && y >= 0);
        copy_array_uint8_t(&Input[0], i, number_of_i_bytes);

        /* Run the PRF */
        if (number_of_Ko_bytes % 16 != 0 && x == cmac_iterations - 1)
        {
            uint8_t* LastKo = new uint8_t[16];
            cmac(&LastKo[0], Input, number_of_Input_bytes);
            memcpy(&Ko[16 * x], &LastKo[0], (number_of_Ko_bytes - (16 * x)));
            if (encrDump)
            {
                for (y = 0; y < 16; y++)
                {
                    VERBOSE_OUT_KDF << std::setfill('0') << std::setw(2) << std::hex << uint32_t(LastKo[y]);
                }
                VERBOSE_OUT_KDF << std::endl;
            }
            delete[] LastKo;
        }
        else
        {
            cmac(&Ko[16 * x], Input, number_of_Input_bytes);
            if(encrDump)
            {
                for (y = 0; y < 16; y++)
                {
                    VERBOSE_OUT_KDF << std::setfill('0') << std::setw(2) << std::hex << uint32_t(Ko[16 * x + y]);
                }
                VERBOSE_OUT_KDF << std::endl;
            }
        }
    }

    if (encrDump)
    {
        /* Ko can then be used for key/iv pairs */
        for (uint32_t x = 0; x < blocks; x++)
        {
            VERBOSE_OUT_KDF << std::endl << "  Key " << std::dec << x << "  ";
            for (uint32_t y = 0; y < 32; y++)
            {
                VERBOSE_OUT_KDF << std::setfill('0') << std::setw(2) << std::hex << uint32_t(Ko[44 * x + y]);
            }

            VERBOSE_OUT_KDF << std::endl << "  IV  " << std::dec << x << "  ";
            for (uint32_t y = 32; y < 44; y++)
            {
                VERBOSE_OUT_KDF << std::setfill('0') << std::setw(2) << std::hex << uint32_t(Ko[44 * x + y]);
            }
            VERBOSE_OUT_KDF << std::endl;
        }
    }

    delete[] Input;
}
