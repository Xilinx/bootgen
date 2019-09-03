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

#ifndef _ENCRYPTION_ZYNQ_CONTEXT_
#define _ENCRYPTION_ZYNQ_CONTEXT_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "encryption.h"
#include "encryptutils.h"

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class ZynqEncryptionContext : public EncryptionContext
{
public:
    ZynqEncryptionContext();
    ZynqEncryptionContext(const EncryptionContext* other);
    ZynqEncryptionContext(uint32_t wordsPerAesBlock, uint32_t words_per_AES_key0);

    ~ZynqEncryptionContext();
    
    Encryption::Type Type()
    {
        return Encryption::AES;
    }

    void ReadEncryptionKeyFile(const std::string& keyFileName);
    void GenerateEncryptionKeyFile(const std::string& baseFileName, Options& options);
    void WriteEncryptionKeyFile(const std::string& baseFileName, bool useOptionalKey = false, uint32_t blocks = 0);
    void Process(BootImage& bi, PartitionHeader* partition);

    void GenerateTempKeys(void);
    void EncryptKeys(const uint8_t* inputData, uint32_t inputDataByteLength, uint8_t* encryptedData);

    void ClearKeySchedule(void);
    void InitKeySchedule(void);
    void AES_Encrypt(const uint8_t* inputData, uint32_t inputDataByteLength, uint8_t* encryptedData);
    void ComputeHMACDigest(const uint8_t* data, const uint32_t dataByteLength, bool initializeNewDigest, uint8_t* digest);

    void GenerateAesKey(void);
    void SetAesKey(const uint8_t* binarykey);
    const uint8_t* GetAesKey(void);

    void GenerateCbc(void);
    void setCbcString(const std::string& ASCIIinitialVector);
    void SetCbc(const uint8_t * binaryinitialVector);
    const uint8_t* GetCbc(void);

    void GenerateHmacKey(void);
    void SetHmacKeyString(const std::string& asciiHMACKey);
    void SetHmacKey(const uint8_t * binaryHMACKey);
    const uint8_t* GetHmacKey(void);

    DataMover* dataMover;
    bool maskEfuseFlag;

private:
    void AddRoundKey(uint32_t* int_array, uint32_t num_round);
    void ByteSubTransformation(uint32_t* int_array);
    void ShiftRowTransformation(uint32_t* int_array);
    void MixColumnTransformation(uint32_t* int_array);

    uint32_t CvtChars2Int(uint8_t value1, uint8_t value2, uint8_t value3, uint8_t value4);
    uint32_t Transform(uint32_t orig_col, uint32_t col_num, bool rotate_rcon);

    bool first_AES_block;
    uint32_t d_NumRounds;
    uint32_t wordsPerAesKey;
    uint32_t wordsPerAesBlock;
    uint32_t* working_AES_block;
    uint32_t* keySchedule;
    uint32_t* aesKey;
    uint8_t ivCbc[16];
    uint32_t* hmacKey;
};

/******************************************************************************/
class DataStreamEncryption : public ZynqEncryptionContext
{
public:
    DataStreamEncryption(uint32_t wordsPerAesBlock, uint32_t words_per_AES_key0);
    ~DataStreamEncryption(void);

    void AESDataStreamEncrypt(KeySource::Type  keySource, bool isFPGAData, const uint8_t* inputData,
    uint32_t inputDataByteLength, uint8_t*& encryptedDataStream,
    uint32_t&	encryptedBufferByteLength, uint32_t* presBitHdr, uint32_t presBitHdrSize);

private:
    void SetDataStreamHeader(KeySource::Type keySource, uint32_t encryptedStreamByteLength, uint32_t* presBitHdr, uint32_t presBitHdrSize);
    void InitializeInputDataStream(const uint8_t* inputData, uint32_t	inputDataByteLength, bool isFPGAData);
    void InitializeHMAC_ioPadData(uint8_t padByte, uint8_t * padBuffer);
    void InitializePad1Data(uint32_t pad1DataByteLength);
    void InitializePad2Data(uint32_t pad2DataByteLength);
    void EncryptDataStream(uint8_t* encryptedOutputData);

    uint8_t* stream_header;
    uint8_t ipad_data[BYTES_PER_IPAD];
    uint8_t* input_data_Buffer;
    uint32_t input_data_ByteLength;
    uint8_t pad1_data[BYTES_PER_PAD1];
    uint8_t gap_2048[BYTES_PER_2048_GAP];
    uint8_t opad_data[BYTES_PER_OPAD];
    uint8_t first_digest_gap[BYTES_PER_FIRST_DIGEST];
    uint8_t pad2_data[BYTES_PER_PAD2];
    uint8_t HMAC_digest[BYTES_PER_HMAC_DIGEST];

    uint8_t calculated_first_digest[BYTES_PER_FIRST_DIGEST + BYTES_PER_PAD2];
};
#endif
