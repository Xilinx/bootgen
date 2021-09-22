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
#include "encryption-versal.h"
#include "bootimage.h"
#include "encryptutils.h"
#include "options.h"
#include "imageheadertable-versal.h"
#include "partitionheadertable-versal.h"
#include <openssl/rand.h>

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
VersalEncryptionContext::VersalEncryptionContext()
    : isBootloader(false)
    , aesSeedexits(false)
    , isPmcData(false)
{
    encryptionAlgorithm = new AesGcmEncryptionContext();
};

/******************************************************************************/
VersalEncryptionContext::VersalEncryptionContext(const EncryptionContext* other)
    : isBootloader(false)
    , aesSeedexits(false)
    , isPmcData(false)
{
    aesFilename = other->aesFilename;
    encryptionAlgorithm = new AesGcmEncryptionContext();
}

/******************************************************************************/
VersalEncryptionContext::~VersalEncryptionContext()
{
    if (encryptionAlgorithm != NULL)
    {
        delete[] encryptionAlgorithm;
    }
}

/******************************************************************************/
void VersalEncryptionContext::SetAesKey(const uint8_t* key)
{
    aesKey = new uint32_t[WORDS_PER_AES_KEY];

    for (uint32_t index = 0; index < WORDS_PER_AES_KEY; index++)
    {
        aesKey[index] = ReadBigEndian32(key);
        key += sizeof(uint32_t);
    }
}

/******************************************************************************/
void VersalEncryptionContext::SetAesSeedString(const std::string& key)
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
void VersalEncryptionContext::GenerateAesKey(void)
{
    uint32_t keysize = WORDS_PER_AES_KEY * sizeof(uint32_t);
    uint8_t newKey[BYTES_PER_AES_KEY];

    RAND_bytes(newKey, keysize);
    SetAesKey(newKey);

    LOG_INFO("AES Key generated successfully");
}

/******************************************************************************/
void VersalEncryptionContext::GetEncryptionKeys(Options& options, uint8_t* aesKey,
    uint8_t* aesOptKey, uint8_t* aesIV)
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
void VersalEncryptionContext::WriteEncryptionKeyFile(const std::string & baseFileName, 
                                                       bool useOptionalKey, uint32_t blocks)
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
    for (x = 0; x<blocks; x++) 
    {
        keyFile << "Key " << std::dec << x;
        keyFile << "        ";
        if (x == 0 && GetAesKey() != NULL) 
        {
            for (index = 0; index < WORDS_PER_AES_KEY; ++index)
            {
                keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << aesKey[index];
            }
        }
        else 
        {
            for (y = 0; y < WORDS_PER_AES_KEY; y++)
            {
                keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << outBufKDF[11 * x + y];
            }
        }
        keyFile << ";\n";
        keyFile << "IV " << std::dec << x;
        keyFile << "         "; 
        for (y = WORDS_PER_AES_KEY; y < WORDS_PER_AES_KEY + WORDS_PER_IV; y++)
        {
            keyFile << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << outBufKDF[11 * x + y];
        }
        keyFile << ";\n";
        keyFile << "\n";
    }
}

/******************************************************************************/
void VersalEncryptionContext::ReadEncryptionKeyFile(const std::string& inputFileName)
{
    LOG_TRACE("Reading the AES key file");

    std::ifstream keyFile(inputFileName.c_str());
    aesKeyVec.clear();
    aesIvVec.clear();
    int aesKeyNum = 0;
    int aesIvNum = 0;

    if (!keyFile)
    {
        LOG_ERROR("Failure reading AES key file - %s", inputFileName.c_str());
    }

    while (keyFile)
    {
        std::string word;
        keyFile >> word;
        if (word == "")
        {
            return;
        }

        char c = ' ';
        if (word == "Device")
        {
            word = "";
            keyFile >> word;
            c = word[word.size() - 1];
            word.erase(word.size() - 1);
            deviceName = word;
        }
        else if (word == "Key")
        {
            keyFile >> word;
            if (word == "Opt")
            {
                LOG_ERROR("The key word 'Key Opt' is not supported in VERSAL architecture");
            }
            if (word != "" && isalnum(word[0]))
            {
                /* Second Word is "0" or "1" or "2" ...*/
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
                if (aesKeyVec.size() == 0)
                {
                    SetAesKeyString(word);
                }
                aesKeyVec.push_back(word);
            }
            else
            {
                /* Second Word other than "0",  - throw error */
                LOG_DEBUG(DEBUG_STAMP, "Unknown key type - '%s' in %s", word.c_str(), inputFileName.c_str());
                LOG_ERROR("Error parsing AES key file - %s", inputFileName.c_str());
            }
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
                    if (aesIvVec.size() == 0)
                    {
                        SetIvString(word);
                    }
                }
            }
            aesIvVec.push_back(word);
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
        else
        {
            /* If the word is neither of the above */
            LOG_DEBUG(DEBUG_STAMP, "'Key' or 'Device' identifier expected, '%s' found instead", word.c_str());
            LOG_ERROR("Error parsing AES key file - %s", inputFileName.c_str());
        }
        if (c != ';')
        {
            /* Semicolons expected at end of every line */
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
void VersalEncryptionContext::PackNextEncryptionKey(uint8_t* aesKeyNext, int aeskeyPtr)
{
    uint8_t hexData[AES_GCM_KEY_SZ];
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
void VersalEncryptionContext::PackNextIv(uint8_t* aesIVNext, int aesIvPtr)
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
void VersalEncryptionContext::GetNextKey(uint8_t * keyNext, int ptr)
{
    PackNextEncryptionKey(keyNext, ptr);
}

/******************************************************************************/
void VersalEncryptionContext::GetNextIv(uint8_t * keyNext, int ptr)
{
    PackNextIv(keyNext, ptr);
}

/******************************************************************************/
void VersalEncryptionContext::SetAesSeed(const uint8_t* key)
{
    aesSeed = new uint32_t[WORDS_PER_AES_KEY];

    for (uint32_t index = 0; index < WORDS_PER_AES_KEY; index++)
    {
        aesSeed[index] = ReadBigEndian32(key);
        key += sizeof(uint32_t);
    }
}

/******************************************************************************/
const uint32_t* VersalEncryptionContext::GetAesSeed(void)
{
    return (uint32_t *)aesSeed;
}

/******************************************************************************/
const uint32_t* VersalEncryptionContext::GetAesKey(void)
{
    return (uint32_t *)aesKey;
}

/******************************************************************************/
void VersalEncryptionContext::SetIv(const uint8_t* iv)
{
    aesIv = new uint32_t[WORDS_PER_IV];

    for (uint32_t index = 0; index < WORDS_PER_IV; index++)
    {
        aesIv[index] = ReadBigEndian32(iv);
        iv += sizeof(uint32_t);
    }
}

/******************************************************************************/
void VersalEncryptionContext::SetIvString(const std::string& IV)
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
void VersalEncryptionContext::GenerateIv(void)
{
    uint8_t newIVData[BYTES_PER_IV];
    RAND_bytes(newIVData, BYTES_PER_IV);
    SetIv(newIVData);
    LOG_INFO("AES IV generated successfully");
}

/******************************************************************************/
const uint32_t* VersalEncryptionContext::GetIv(void)
{
    return (uint32_t *)aesIv;
}

/******************************************************************************/
void VersalEncryptionContext::GenerateRemainingKeys(Options& options)
{
    uint32_t blocks, x = 0;

    if (GetAesKey() == NULL && !aesSeedexits)
    {
        LOG_ERROR("Encryption Error !!!\n           Key 0 does not exist in the AES key file ");
    }

    if (GetIv() == NULL && !aesSeedexits)
    {
        LOG_ERROR("IV 0 does not exist in the AES key file ");
    }

    if (aesKeyVec.size() != aesIvVec.size())
    {
        //LOG_ERROR("Encryption Error !!!\n           Number of Keys in the AES key file are not equal to the number of IVs. ");
    }

    /* If only seed exists in the given in nky file => No key0/iv0 mentioned. +1 to generate key0/IV0 for SH */
    if ((aesKeyVec.size() == 0 || aesIvVec.size() == 0))
    {
        blocks = options.bifOptions->GetEncryptionBlocksList().size() + 1;
    }
    else
    {
        blocks = options.bifOptions->GetEncryptionBlocksList().size();
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

    uint32_t ret = kdf->CounterModeKDF(aesSeed, fixedInputData, fixedInputDataByteLength, outBufKDF, outBufBytes);
    if (ret != 0)
    {
        LOG_ERROR("Error generating encryption keys from Counter Mode KDF.");
    }

    uint8_t aesKeyNext[AES_GCM_KEY_SZ];
    uint8_t aesIvNext[AES_GCM_IV_SZ];
    for (x = 0; x < blocks; x++)
    {
        memcpy(aesKeyNext, &outBufKDF[(x * 11)], AES_GCM_KEY_SZ);
        if (aesKeyVec.size() < (options.bifOptions->GetEncryptionBlocksList().size() + 1))
        {
            aesKeyVec.push_back(ConvertKeyIvToString(aesKeyNext, AES_GCM_KEY_SZ).c_str());
        }
        if (GetAesKey() == NULL)
        {
            SetAesKey(aesKeyNext);
        }

        memcpy(aesIvNext, &outBufKDF[(x * 11) + WORDS_PER_AES_KEY], AES_GCM_IV_SZ);
        if (aesIvVec.size() < (options.bifOptions->GetEncryptionBlocksList().size() + 1))
        {
            aesIvVec.push_back(ConvertKeyIvToString(aesIvNext, AES_GCM_IV_SZ).c_str());
        }
        if (GetIv() == NULL)
        {
            SetIv(aesIvNext);
        }
    }
}

/******************************************************************************/
void VersalEncryptionContext::ChunkifyAndEncrypt(Options& options, const uint8_t *inBuf, uint32_t inLen,
    uint8_t *aad, uint32_t aad_len, uint8_t* outBuf, uint32_t& outLen)
{
    std::vector<uint32_t> blockList = options.bifOptions->GetEncryptionBlocksList();
    uint8_t *aesIv = new uint8_t[AES_GCM_IV_SZ];
    uint8_t *aesKey = new uint8_t[AES_GCM_KEY_SZ];
    uint8_t *aesIVNext = new uint8_t[AES_GCM_IV_SZ];
    uint8_t *aesKeyNext = new uint8_t[AES_GCM_KEY_SZ];

    GetEncryptionKeys(options, aesKey, NULL, aesIv);
    GetNextKey(aesKeyNext, 1);
    GetNextIv(aesIVNext, 1);

    uint8_t secureHdr_in[AES_GCM_KEY_SZ + AES_GCM_IV_SZ + NUM_BYTES_PER_WORD];

    /* Extract the first block size */
    uint32_t nextBlkSize = (blockList.empty()) ? inLen : blockList[0];
    nextBlkSize = (nextBlkSize > inLen) ? inLen : nextBlkSize + 0;

    /* Copy Key,IV used for encrypting next block to Secure Header*/
    memcpy(secureHdr_in, aesKeyNext, AES_GCM_KEY_SZ);
    memcpy(secureHdr_in + AES_GCM_KEY_SZ, aesIVNext, AES_GCM_IV_SZ);

    /* Copy the word Size of Block 0 to Secure Header*/
    WriteLittleEndian32(secureHdr_in + AES_GCM_KEY_SZ + AES_GCM_IV_SZ, nextBlkSize / NUM_BYTES_PER_WORD);

    int ct_len;
    uint8_t gcm_tag[AES_GCM_TAG_SZ];

    /* Encrypt the Secure Header with device key and starting IV */
    LOG_TRACE("Encrypting the Secure Header");
    uint8_t* ptr = outBuf;
    AesGcm256Encrypt(secureHdr_in, SECURE_HDR_SZ, aesKey, aesIv, aad, aad_len, ptr, ct_len, gcm_tag);

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
        AesGcm256Decrypt(secureHdr_out, pt_len, aesKey, aesIv, NULL, 0, outBuf, SECURE_HDR_SZ, gcm_tag);
        int ret = memcmp(secureHdr_in, secureHdr_out, SECURE_HDR_SZ);
        if (ret == 0)
        {
            //VERBOSE_OUT << "        Encrypted secure header was successfully decrypted and tag matched\n";
        }
        else
        {
            //LOG_ERROR("Decrypted Secure Header tag mis-matched");
        }
    }

    uint32_t bytesWritten = 0;
    uint32_t inPtr = 0;
    uint32_t blkPtr = 1;

    /* Take the next generated Key and IV for encrypting next block */
    memcpy(aesIv, aesIVNext, AES_GCM_IV_SZ);
    memcpy(aesKey, aesKeyNext, AES_GCM_KEY_SZ);

    while (bytesWritten < inLen)
    {
        //Update the block size for current block and extract the next block size
        uint32_t currBlkSize = nextBlkSize;
        bytesWritten += currBlkSize;
        nextBlkSize = (blockList.size() > blkPtr) ? blockList[blkPtr] : inLen - bytesWritten;
        nextBlkSize = ((nextBlkSize + bytesWritten) > inLen) ? (inLen - bytesWritten) : (nextBlkSize + 0);

        /* Get next key and IV - store in current block - use for next block */
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
        /* Prepare the buffer for encryption - Actual block data + Next Block Key + Next Block IV + Next Block Word Size */
        memcpy(gcm_pt, inBuf + inPtr, currBlkSize);
        inPtr += currBlkSize;
        memcpy(gcm_pt + currBlkSize, aesKeyNext, AES_GCM_KEY_SZ);
        memcpy(gcm_pt + currBlkSize + AES_GCM_KEY_SZ, aesIVNext, AES_GCM_IV_SZ);
        WriteLittleEndian32(gcm_pt + currBlkSize + AES_GCM_KEY_SZ + AES_GCM_IV_SZ, nextBlkSize / NUM_BYTES_PER_WORD);

        //Encrypt the consolidated block
        LOG_TRACE("Encrypting the block %d of size 0x%x", blkPtr, currBlkSize);
        AesGcm256Encrypt(gcm_pt, currBlkSize + SECURE_HDR_SZ, aesKey, aesIv, NULL, 0, outBuf + outPtr, ct_len, gcm_tag);
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
            AesGcm256Decrypt(inBuf_out, pt_len, aesKey, aesIv, NULL, 0, outBuf + outPtr, ct_len, gcm_tag);
            int ret = memcmp(gcm_pt, inBuf_out, pt_len);
            if (ret == 0)
            {
                //VERBOSE_OUT << std::dec << "        Encrypted block " << currBlk++ << " was successfully decrypted and tag matched\n";
            }
            else
            {
                //LOG_ERROR("Decrypted block %d tag mis-matched", currBlk++);
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
}

/******************************************************************************/
void VersalEncryptionContext::WarnforDPACMImpactonBootTime(bool dpacmKeyRollingEnable, bool dpacmMaskEnable)
{
    static bool dpaKeyrollingWarningGiven = false;
    static bool dpaMaskingWarningGiven = false;

    if (!dpaKeyrollingWarningGiven)
    {
        if (dpacmKeyRollingEnable)
        {
            LOG_WARNING("DPA key rolling countermeasure is enabled.\n\t   Boot time will be impacted, Refer to boot time estimator spreadsheet for calculations.");
            dpaKeyrollingWarningGiven = true;
        }
    }
    if (!dpaMaskingWarningGiven)
    {
        if (dpacmMaskEnable)
        {
            LOG_WARNING("DPA masking countermeasure enabled.\n\t   Boot time will be impacted, Refer to boot time estimator spreadsheet for calculations.");
            dpaMaskingWarningGiven = true;
        }
    }
}

/******************************************************************************/
void VersalEncryptionContext::Process(BootImage& bi, PartitionHeader* partHdr)
{
    Options& options = bi.options;

    LOG_INFO("Encrypting the partition - %s", partHdr->partition->section->Name.c_str());

    std::vector<uint32_t> encrBlocks = partHdr->imageHeader->GetEncrBlocksList();
    uint32_t defEncrBlocks = partHdr->imageHeader->GetDefaultEncrBlockSize();
    Binary::Length_t lastBlock = 0;
    uint32_t totalencrBlocks = 0;

    WarnforDPACMImpactonBootTime(encrBlocks.size() != 0, partHdr->imageHeader->GetDpacm() == DpaCM::DpaCMEnable);

    /* Default key rolling to make a chunk size of 32KB - Valid for any partition other than bootloader */
    if (!partHdr->IsBootloader())
    {
        totalencrBlocks = encrBlocks.size();
        defEncrBlocks = 0;
        uint32_t overhead = (totalencrBlocks) * (SECURE_HDR_SZ + AES_GCM_TAG_SZ);

        /* Due to encryption over head, the actual default size on which the partition needs to be Key rolled is always less than 64KB.
           So first calculate the default key roll data size by substracting the overhead. */
        /* Then calculate the number of such blocks possible on a given partition. */
        /* Note that the last block will always be based on the partition length.*/

        std::vector<uint32_t> secureChunkEncrBlocks;
        uint32_t actualSecureChunkSize = VersalPartition::GetSecureChunkSize() - overhead;
        if (partHdr->imageHeader->GetAuthenticationType() == Authentication::None)
        {
            actualSecureChunkSize += SHA3_LENGTH_BYTES;
        }

        uint32_t totalKeyRollencrBlocks = GetTotalEncryptionBlocks(partHdr->partition->section->Length, secureChunkEncrBlocks, actualSecureChunkSize, &lastBlock);
        secureChunkEncrBlocks.clear();

        for (uint32_t itr = 0; itr < totalKeyRollencrBlocks; itr++)
        {
            if ((itr == totalKeyRollencrBlocks - 1) && (lastBlock != 0))
            {
                secureChunkEncrBlocks.push_back(lastBlock);
                lastBlock = 0;
            }
            else
            {
                secureChunkEncrBlocks.push_back(actualSecureChunkSize);
            }
        }

        /* Now chunk each default key roll data size, based on user encryption blocks.
           Note that the last block will always be based on the partition length.*/
        options.bifOptions->GetEncryptionBlocksList().clear();
        for (uint32_t itr1 = 0; itr1 < totalKeyRollencrBlocks; itr1++)
        {
            if ((itr1 == totalKeyRollencrBlocks - 1) && (secureChunkEncrBlocks[itr1] != actualSecureChunkSize))
            {
                Binary::Length_t encrBlocksSize = 0;
                for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
                {
                    encrBlocksSize += encrBlocks[itr];
                    if (secureChunkEncrBlocks[itr1] > encrBlocksSize)
                    {
                        options.bifOptions->InsertEncryptionBlock(encrBlocks[itr]);
                    }
                    else
                    {
                        options.bifOptions->InsertEncryptionBlock(secureChunkEncrBlocks[itr1] - (encrBlocksSize - encrBlocks[itr]));
                        break;
                    }
                }
            }
            else
            {
                for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
                {
                    options.bifOptions->InsertEncryptionBlock(encrBlocks[itr]);
                }
            }
        }
        totalencrBlocks = options.bifOptions->GetEncryptionBlocksList().size();
    }
    else
    {
        totalencrBlocks = GetTotalEncryptionBlocks(partHdr->partition->section->Length, encrBlocks, defEncrBlocks, &lastBlock);
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
    }

    LOG_TRACE("Total no. of Key/IV pairs needed to encrypt - %d", totalencrBlocks + 1);

    /* Get the key file */
    SetAesFileName(partHdr->partitionAesKeyFile);
    LOG_INFO("Key file - %s", aesFilename.c_str());
    if (partHdr->generateAesKeyFile)
    {
        std::ifstream keyFile(aesFilename);
        bool fileExists = keyFile.good();
        if (!fileExists)
        {
            if (bi.aesKeyandKeySrc.size() != 0)
            {
                for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
                {
                    if (partHdr->imageHeader->GetEncryptionKeySrc() == bi.aesKeyandKeySrc[i].first)
                    {
                        aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                        memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                        break;
                    }
                }
            }
            GenerateEncryptionKeyFile(aesFilename, options);
        }
        else
        {
            LOG_ERROR("Key Generation Error !!!\n           File - %s already exists.", aesFilename.c_str());
        }
    }

    bi.InsertEncryptionKeyFile(aesFilename);
    CheckForSameAesKeyFiles(bi.GetEncryptionKeyFileVec());

    ReadEncryptionKeyFile(aesFilename);

    std::pair<KeySource::Type, uint32_t*> asesKeyandKeySrcPair (partHdr->imageHeader->GetEncryptionKeySrc(), aesKey);
    bi.aesKeyandKeySrc.push_back(asesKeyandKeySrcPair);
    bi.bifOptions->CheckForBadKeyandKeySrcPair(bi.aesKeyandKeySrc, aesFilename);

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
        if (bi.aesKeyandKeySrc.size() != 0)
        {
            for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
            {
                if (partHdr->imageHeader->GetEncryptionKeySrc() == bi.aesKeyandKeySrc[i].first)
                {
                    if (aesKey == NULL)
                    {
                        aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                        memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                    }
                    if (aesKeyVec.size() == 0)
                    {
                        aesKeyVec.push_back(ConvertKeyIvToString((uint8_t *)aesKey, AES_GCM_KEY_SZ).c_str());
                    }
                    break;
                }
            }
        }
        GenerateRemainingKeys(options);
    }

    CheckForRepeatedKeyIVPairs(bi.GetEncryptionKeyFileVec(), false);
    if (bi.aesKeyandKeySrc.size() != 0)
    {
        for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
        {
            if (partHdr->imageHeader->GetEncryptionKeySrc() == bi.aesKeyandKeySrc[i].first)
            {
                if (aesKey != NULL)
                {
                    memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                }
                break;
            }
        }
    }

    // For copying SCR HDR IV into Boot Header
    const uint32_t* tmpIv = GetIv();
    if (tmpIv != NULL)
    {
        if (partHdr->IsBootloader())
        {
            if (options.secHdrIv == NULL)
            {
                options.secHdrIv = (uint8_t*)malloc(BYTES_PER_IV);
            }
            memcpy_be(options.secHdrIv, tmpIv, BYTES_PER_IV);
        }
        if (partHdr->partitionSecHdrIv == NULL)
        {
            partHdr->partitionSecHdrIv = (uint8_t*)malloc(BYTES_PER_IV);
        }
        memcpy_be(partHdr->partitionSecHdrIv, tmpIv, BYTES_PER_IV);
    }

    uint32_t totalBlocksOverhead = (totalencrBlocks + 1) * 64; //64 = AES_GCM_IV_SZ+AES_GCM_KEY_SZ+NUM_BYTES_PER_WORD+AES_GCM_TAG_SZ

    if (!Binary::CheckAddress(partHdr->partition->section->Length))
    {
        LOG_ERROR("Partition too large to encrypt");
    }

    isBootloader = partHdr->imageHeader->IsBootloader();
    if (options.GetEncryptionDumpFlag())
    {
        bi.options.aesLogFile.open(bi.options.aesLogFilename, std::fstream::app);
        VERBOSE_OUT << std::endl << "------------------------------------";
        VERBOSE_OUT << std::endl << " Partition Name : " << partHdr->partition->section->Name;
        VERBOSE_OUT << std::endl << " Key file       : " << StringUtils::BaseName(aesFilename);
        VERBOSE_OUT << std::endl << "------------------------------------";
    }

    /* Encrypt the stream */
    if (isBootloader && partHdr->imageHeader->GetPmcFwSizeIh() != 0)
    {
        // BootLoader Encryption
        uint32_t encrFsblByteLength;
        uint32_t estimatedEncrFsblLength = partHdr->imageHeader->GetFsblFwSizeIh() + totalBlocksOverhead;
        uint8_t* encrFsblDataBuffer = new uint8_t[estimatedEncrFsblLength];
        LOG_INFO("Encrypting Bootloader");
        ChunkifyAndEncrypt(options,
            partHdr->partition->section->Data,
            partHdr->imageHeader->GetFsblFwSizeIh(),
            NULL, 0,
            encrFsblDataBuffer /* out*/,
            encrFsblByteLength /* out */);

        if (estimatedEncrFsblLength < encrFsblByteLength)
        {
            LOG_ERROR("Encryption Error!!! - PmcFw Buffer overflow");
        }

        // PMC Data Encryption
        if (options.bifOptions->pmcDataAesFile != "")
        {
            aesKey = aesIv = aesSeed = NULL;
            fixedInputData = NULL;
            SetAesFileName(options.bifOptions->pmcDataAesFile);
            LOG_INFO("Key file - %s", aesFilename.c_str());
            std::ifstream keyFile(aesFilename);
            bool fileExists = keyFile.good();
            if (!fileExists)
            {
                if (bi.aesKeyandKeySrc.size() != 0)
                {
                    for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
                    {
                        if (partHdr->imageHeader->GetEncryptionKeySrc() == bi.aesKeyandKeySrc[i].first)
                        {
                            aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                            memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                            break;
                        }
                    }
                }
                GenerateEncryptionKeyFile(aesFilename, options);
            }

            bi.InsertEncryptionKeyFile(aesFilename);
            CheckForSameAesKeyFiles(bi.GetEncryptionKeyFileVec());

            ReadEncryptionKeyFile(aesFilename);

            std::pair<KeySource::Type, uint32_t*> asesKeyandKeySrcPair(partHdr->imageHeader->GetEncryptionKeySrc(), aesKey);
            bi.aesKeyandKeySrc.push_back(asesKeyandKeySrcPair);
            bi.bifOptions->CheckForBadKeyandKeySrcPair(bi.aesKeyandKeySrc, aesFilename);

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
                if (bi.aesKeyandKeySrc.size() != 0)
                {
                    for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
                    {
                        if (partHdr->imageHeader->GetEncryptionKeySrc() == bi.aesKeyandKeySrc[i].first)
                        {
                            if (aesKey == NULL)
                            {
                                aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                                memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                            }
                            if (aesKeyVec.size() == 0)
                            {
                                aesKeyVec.push_back(ConvertKeyIvToString((uint8_t *)aesKey, AES_GCM_KEY_SZ).c_str());
                            }
                            break;
                        }
                    }
                }
                GenerateRemainingKeys(options);
            }

            tmpIv = GetIv();
            if (tmpIv != NULL)
            {
                if (options.secHdrIvPmcData == NULL)
                {
                    options.secHdrIvPmcData = (uint8_t*)malloc(BYTES_PER_IV);
                }
                memcpy_be(options.secHdrIvPmcData, tmpIv, BYTES_PER_IV);
            }
        }
        else
        {
            LOG_ERROR("Key Generation Error !!!\n           Key File doesnot exist to encrypt PMC CDO ");
        }

        CheckForRepeatedKeyIVPairs(bi.GetEncryptionKeyFileVec(), false);
        isPmcData = true;
        uint32_t encrPmcByteLength;
        uint32_t estimatedEncrPmcLength = partHdr->imageHeader->GetTotalPmcFwSizeIh() + totalBlocksOverhead;
        uint8_t* encrPmcDataBuffer = new uint8_t[estimatedEncrPmcLength];
        LOG_INFO("Encrypting the PMC Data");

        ChunkifyAndEncrypt(options,
            partHdr->partition->section->Data + partHdr->imageHeader->GetFsblFwSizeIh(),
            partHdr->imageHeader->GetTotalPmcFwSizeIh(),
            NULL, 0,
            encrPmcDataBuffer /* out*/,
            encrPmcByteLength /* out */);

        if (estimatedEncrPmcLength < encrPmcByteLength)
        {
            LOG_ERROR("Encryption Error!!! - PMC Buffer overflow");
        }

        partHdr->partition->section->IncreaseLengthAndPadTo(encrFsblByteLength + encrPmcByteLength, 0x0);
        memset(partHdr->partition->section->Data, 0, encrFsblByteLength + encrPmcByteLength);
        memcpy(partHdr->partition->section->Data, encrFsblDataBuffer, encrFsblByteLength);
        memcpy(partHdr->partition->section->Data + encrFsblByteLength, encrPmcDataBuffer, encrPmcByteLength);

        partHdr->imageHeader->SetTotalPmcFwSizeIh(encrPmcByteLength);
        partHdr->imageHeader->SetTotalFsblFwSizeIh(encrFsblByteLength);
        partHdr->partition->section->Length = encrFsblByteLength + encrPmcByteLength;

        LOG_INFO("Encrypted the partition - %s", partHdr->partition->section->Name.c_str());
        bi.options.CloseEncryptionDumpFile();
        delete[] encrFsblDataBuffer;
        delete[] encrPmcDataBuffer;
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
            NULL, 0,
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
void VersalEncryptionContext::Process(BootImage& bi)
{
    size_t size = bi.imageHeaderTable->metaHeaderLength;
    uint8_t* dataBuffer = new uint8_t[size];

    uint32_t offset = 0;
    for (SectionList::iterator i = bi.headers.begin(); i != bi.headers.end(); i++)
    {
        Section& section(**i);
        memcpy(dataBuffer + offset, section.Data, section.Length);
        offset += section.Length;
    }

    uint32_t totalencrBlocks = bi.options.bifOptions->metaHdrAttributes.encrBlocks.size();
    bi.options.bifOptions->GetEncryptionBlocksList().clear();
    for (uint32_t itr = 0; itr < totalencrBlocks; itr++)
    {
        bi.options.bifOptions->InsertEncryptionBlock(bi.options.bifOptions->metaHdrAttributes.encrBlocks[itr]);
    }

    LOG_TRACE("Total no. of Key/IV pairs needed to encrypt - %d", totalencrBlocks + 1);

    WarnforDPACMImpactonBootTime(bi.options.bifOptions->metaHdrAttributes.encrBlocks.size() != 0, bi.options.bifOptions->metaHdrAttributes.dpaCM == DpaCM::DpaCMEnable);

    /* Get the key file */
    if (bi.options.bifOptions->metaHdrAttributes.encrKeyFile != "")
    {
        SetAesFileName(bi.options.bifOptions->metaHdrAttributes.encrKeyFile);
        LOG_INFO("Key file - %s", aesFilename.c_str());
        std::ifstream keyFile(aesFilename);
        bool fileExists = keyFile.good();
        if (!fileExists)
        {
            if (bi.aesKeyandKeySrc.size() != 0)
            {
                for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
                {
                    if (bi.options.bifOptions->metaHdrAttributes.encrKeySource == bi.aesKeyandKeySrc[i].first)
                    {
                        aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                        memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                        break;
                    }
                }
            }
            GenerateEncryptionKeyFile(aesFilename, bi.options);
        }

        bi.InsertEncryptionKeyFile(aesFilename);
        CheckForSameAesKeyFiles(bi.GetEncryptionKeyFileVec());

        ReadEncryptionKeyFile(aesFilename);

        std::pair<KeySource::Type, uint32_t*> asesKeyandKeySrcPair(bi.options.bifOptions->metaHdrAttributes.encrKeySource, aesKey);
        bi.aesKeyandKeySrc.push_back(asesKeyandKeySrcPair);
        bi.bifOptions->CheckForBadKeyandKeySrcPair(bi.aesKeyandKeySrc, aesFilename);

        bi.options.SetDevicePartName(deviceName);

        CheckForExtraKeyIVPairs(totalencrBlocks, "MetaHeader");

        if (!aesSeedexits && aesKeyVec.size() != 1)
        {
            if (totalencrBlocks + 1 > aesKeyVec.size())
            {
                LOG_ERROR("AES Key file has less keys than the number of blocks to be encrypted in Meta Header.");
            }
            if (totalencrBlocks + 1 > aesIvVec.size())
            {
                LOG_ERROR("AES Key file has less IVs than the number of blocks to be encrypted in Meta Header.");
            }
        }
        else
        {
            if (bi.aesKeyandKeySrc.size() != 0)
            {
                for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
                {
                    if (bi.options.bifOptions->metaHdrAttributes.encrKeySource == bi.aesKeyandKeySrc[i].first)
                    {
                        if (aesKey == NULL)
                        {
                            aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                            memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                        }
                        if (aesKeyVec.size() == 0)
                        {
                            aesKeyVec.push_back(ConvertKeyIvToString((uint8_t *)aesKey, AES_GCM_KEY_SZ).c_str());
                        }
                        break;
                    }
                }
            }
            GenerateRemainingKeys(bi.options);
        }

        CheckForRepeatedKeyIVPairs(bi.GetEncryptionKeyFileVec(), false);
        if (bi.aesKeyandKeySrc.size() != 0)
        {
            for (uint32_t i = 0; i < bi.aesKeyandKeySrc.size(); i++)
            {
                if (bi.options.bifOptions->metaHdrAttributes.encrKeySource == bi.aesKeyandKeySrc[i].first)
                {
                    if (aesKey == NULL)
                    {
                        aesKey = new uint32_t[AES_GCM_KEY_SZ / 4];
                        memcpy(aesKey, bi.aesKeyandKeySrc[i].second, AES_GCM_KEY_SZ);
                    }
                    if (aesKeyVec.size() == 0)
                    {
                        aesKeyVec.push_back(ConvertKeyIvToString((uint8_t *)aesKey, AES_GCM_KEY_SZ).c_str());
                    }
                    break;
                }
            }
        }

        // For copying SCR HDR IV into Boot Header
        const uint32_t* tmpIv = GetIv();
        if (tmpIv != NULL)
        {
            if (bi.imageHeaderTable->metaHdrSecHdrIv == NULL)
            {
                bi.imageHeaderTable->metaHdrSecHdrIv = (uint8_t*)malloc(BYTES_PER_IV);
            }
            memcpy_be(bi.imageHeaderTable->metaHdrSecHdrIv, tmpIv, BYTES_PER_IV);
        }
        bi.imageHeaderTable->SetMetaHdrSecureHdrIv(bi.imageHeaderTable->metaHdrSecHdrIv);
        bi.imageHeaderTable->SetChecksum();
    }
    else
    {
        LOG_ERROR("Key Generation Error !!!\n           File - %s already exists.", aesFilename.c_str());
    }


    uint32_t totalBlocksOverhead = (totalencrBlocks + 1) * 64; //64 = AES_GCM_IV_SZ+AES_GCM_KEY_SZ+NUM_BYTES_PER_WORD+AES_GCM_TAG_SZ

    if (!Binary::CheckAddress(size))
    {
        LOG_ERROR("Partition too large to encrypt");
    }

    if (bi.options.GetEncryptionDumpFlag())
    {
        bi.options.aesLogFile.open(bi.options.aesLogFilename, std::fstream::app);
        VERBOSE_OUT_MH << std::endl << "------------------------------------";
        VERBOSE_OUT_MH << std::endl << " Partition Name : " << "Meta Header";
        VERBOSE_OUT_MH << std::endl << " Key file       : " << StringUtils::BaseName(aesFilename);
        VERBOSE_OUT_MH << std::endl << "------------------------------------";
    }

    /* Encrypt the stream */
    uint32_t encryptedLength;
    uint32_t estimatedEncrLength = size + totalBlocksOverhead;
    if (bi.encryptedHeaders->Length != estimatedEncrLength)
    {
        LOG_ERROR("Internal Error : Encrypted metaheader length calculation error.");
    }

    uint8_t* encryptedDataBuffer = new uint8_t[estimatedEncrLength];

    ChunkifyAndEncrypt(bi.options,
        dataBuffer,
        (uint32_t)size,
        bi.imageHeaderTable->section->Data,
        sizeof(VersalImageHeaderTableStructure),
        encryptedDataBuffer /* out*/,
        encryptedLength /* out */);

    if (bi.encryptedHeaders->Length != encryptedLength)
    {
        LOG_ERROR("Internal Error : Encryption buffer allocation error.");
    }

    memset(bi.encryptedHeaders->Data, bi.options.GetOutputFillByte(), bi.encryptedHeaders->Length);
    memcpy(bi.encryptedHeaders->Data, encryptedDataBuffer, encryptedLength);
    LOG_INFO("Encrypted the partition - %s", bi.encryptedHeaders->Name.c_str());

    bi.options.CloseEncryptionDumpFile();
    delete[] encryptedDataBuffer;
    delete[] dataBuffer;
    return;
}

/******************************************************************************/
void VersalEncryptionContext::AesGcm256Encrypt(unsigned char* gcm_pt, int pt_len,
                                               unsigned char* gcm_key, unsigned char* gcm_iv,
                                               unsigned char* gcm_aad, int aad_len,
                                               unsigned char* gcm_ct, int& ct_len,
                                               unsigned char* gcm_tag)
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
    if ((gcm_aad) && aad_len) {
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
void VersalEncryptionContext::AesGcm256Decrypt(unsigned char* gcm_pt, int& pt_len,
                                               unsigned char* gcm_key, unsigned char* gcm_iv,
                                               unsigned char* gcm_aad, int aad_len,
                                               unsigned char* gcm_ct, int ct_len,
                                               unsigned char* gcm_tag)
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
    * required the tag before any AAD or ciphertext */
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

