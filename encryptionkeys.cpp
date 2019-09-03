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
#include "encryptionkeys.h"
#include "encryption.h"
#include "encryption-zynqmp.h"
#include "encryptutils.h"
#include "systemutils.h"
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

void ZynqMpEncryptionContext::CounterModeKDF(uint32_t blocks, std::string keyFilename, bool encrDump)
{
    uint32_t x, y;
    int number_of_i_bytes = 4;
    uint8_t i[4] = { 0x00, 0x00, 0x00, 0x00 };
    int number_of_L_bytes = 8;
    int number_of_Separator_bytes = 1;
    int number_of_Label_bytes = aesLabelBytes;
    int number_of_Context_bytes = aesContextBytes; //3 to make input 48 - was 6
    static bool kdfLogInit = false;
    static std::ofstream kdfLogFile;

    //input key0 or seed
    if (GetAesSeed() == NULL)
    {
        SetAesSeed((uint8_t*)aesKey);
    }
    if (GetAesLabel() == NULL)
    {
        SetAesLabel(defaultLabel, number_of_Label_bytes);
    }
    if (GetAesContext() == NULL)
    {
        SetAesContext(defaultContext, number_of_Context_bytes);
    }

    uint8_t* cmac_key = new uint8_t[BYTES_PER_AES_KEY];
    memcpy_be(cmac_key,aesSeed, AES_GCM_KEY_SZ);

    uint8_t* label = new uint8_t[number_of_Label_bytes];
    memcpy(label, aesLabel, number_of_Label_bytes);
    
    uint8_t* context = new uint8_t[number_of_Context_bytes];
    memcpy(context, aesContext, number_of_Context_bytes);

    /* L will be 0x580 to generate 4 AES-256-GCM key/IV pairs which is 1408 bits = 4*(32+12) bytes */
    uint8_t* L = new uint8_t[number_of_L_bytes];
    memset(L, 0, number_of_L_bytes);
    uint64_t temp = blocks * 8 * (uint64_t)(AES_GCM_KEY_SZ + AES_GCM_IV_SZ);
    reverse_copy(L, temp, number_of_L_bytes);
    
    /* Due to the CMAC implementation the Input must be evenly divisible by 128-bits; This
    * CMAC implementation should be fixed so that Context, Label can be aribitrary byte
    * lengths defined by the customer */

    int number_of_Input_bytes = number_of_i_bytes + number_of_Label_bytes + number_of_Separator_bytes +
                number_of_Context_bytes + number_of_L_bytes;
    uint8_t* Input = new uint8_t[number_of_Input_bytes];
    
    /* An example generating 4 key/IV pairs will have 4*(32+12) number_of_Ko_bytes.
       This should be flexible based on number of keys or key/iv pair required 
       or it can just pull off key/IV as needed; it is acceptable to discard 
       32-bits and use 384 bits of Ko per 352-bits of key/iv pair. */
    
    uint32_t number_of_Ko_bytes = blocks * (AES_GCM_KEY_SZ + AES_GCM_IV_SZ);
    uint8_t* Ko = new uint8_t[number_of_Ko_bytes];
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
        VERBOSE_OUT_KDF << std::endl << " Generating Key file : " << StringUtils::BaseName(keyFilename);
        VERBOSE_OUT_KDF << std::endl << "------------------------------------" << std::endl;
    }

    gen_cmac_subkeys(cmac_key);
    
    // Build the input
    copy_array_uint8_t(&Input[0], i, number_of_i_bytes);
    copy_array_uint8_t(&Input[number_of_i_bytes], defaultLabel, number_of_Label_bytes);
    copy_array_uint8_t(&Input[number_of_i_bytes + number_of_Label_bytes], &Separator, number_of_Separator_bytes);
    copy_array_uint8_t(&Input[number_of_i_bytes + number_of_Label_bytes + number_of_Separator_bytes],
        defaultContext, number_of_Context_bytes);
    copy_array_uint8_t(&Input[number_of_i_bytes + number_of_Label_bytes + number_of_Separator_bytes + number_of_Context_bytes],
        L, number_of_L_bytes);
    
    uint32_t cmac_iterations = number_of_Ko_bytes / 16;
    cmac_iterations += ((number_of_Ko_bytes % 16) == 0 ? 0 : 1);
    /* Run the KDF for the amount of data needed */
    for (x = 0; x<cmac_iterations; x++) 
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
            memcpy(&Ko[16 * x], &LastKo[0], (number_of_Ko_bytes-(16*x)));
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
            if (encrDump)
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
        for (x = 0; x < blocks; x++)
        {
            VERBOSE_OUT_KDF << std::endl << "  Key " << x << "  ";
            for (y = 0; y < 32; y++)
            {
                VERBOSE_OUT_KDF << std::setfill('0') << std::setw(2) << std::hex << uint32_t(Ko[44 * x + y]);
            }

            VERBOSE_OUT_KDF << std::endl << "  IV  " << x << "  ";
            for (y = 32; y < 44; y++)
            {
                VERBOSE_OUT_KDF << std::setfill('0') << std::setw(2) << std::hex << uint32_t(Ko[44 * x + y]);
            }
            VERBOSE_OUT_KDF << std::endl;
        }
    }

    outBufKDF = new uint32_t[number_of_Ko_bytes/4];
    
    // Copy the key
    for (uint32_t index = 0; index < number_of_Ko_bytes / 4; index++)
    {
        outBufKDF[index] = ReadBigEndian32(Ko + (index * sizeof(uint32_t)));
    }
    delete[] cmac_key;
    delete[] L;
    delete[] label;
    delete[] context;
    delete[] Input;
    delete[] Ko;
}

/******************************************************************************/
void ZynqMpEncryptionContext::ParseKDFTestVectorFile(std::string filename)
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
                koLength = strtoul(word.c_str(), NULL, 10);
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
                fixedInputDataByteLength = strtoul(word.c_str(), NULL, 10);
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
        else
        {
            LOG_ERROR("Error parsing KDF test vector file - %s \n\t Expected 'L','KI','FixedInputDataByteLen', and 'FixedInputData'.", filename.c_str());
        }
    }
}

/******************************************************************************/
void ZynqMpEncryptionContext::CAVPonCounterModeKDF(std::string filename)
{
    LOG_MSG("Generating Ko using Counter-Mode KDF...");
    ParseKDFTestVectorFile(filename);

    uint32_t number_of_i_bytes = 4;
    uint8_t i[4] = { 0x00, 0x00, 0x00, 0x00 };

    uint32_t number_of_Input_bytes = number_of_i_bytes + fixedInputDataByteLength;
    uint8_t* Input = new uint8_t[number_of_Input_bytes];

    uint32_t number_of_Ko_bytes = koLength / 8;
    uint8_t* Ko = new uint8_t[number_of_Ko_bytes];

    gen_cmac_subkeys(kI);

    copy_array_uint8_t(&Input[0], i, number_of_i_bytes);
    copy_array_uint8_t(&Input[number_of_i_bytes], fixedInputData, fixedInputDataByteLength);

    uint32_t cmac_iterations = number_of_Ko_bytes / 16;
    cmac_iterations += ((number_of_Ko_bytes % 16) == 0 ? 0 : 1);

    printf("KO = ");
    /* Run the KDF for the amount of data needed */
    uint32_t x, y;
    for (x = 0; x<cmac_iterations; x++)
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
            for (y = 0; y<16; y++)
            {
                printf("%02X", LastKo[y]);
            }
            delete[] LastKo;
        }
        else
        {
            cmac(&Ko[16 * x], Input, number_of_Input_bytes);
            for (y = 0; y<16; y++)
            {
                printf("%02X", Ko[16 * x + y]);
            }
        }
    }
    printf("\n");
    delete[] Input;
    delete[] Ko;
}


