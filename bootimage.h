/******************************************************************************
* Copyright 2015-2022 Xilinx, Inc.
* Copyright 2022-2023 Advanced Micro Devices, Inc.
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

#ifndef _BOOTIMAGE_H_
#define _BOOTIMAGE_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <list>
#include <string>
#include "bootgenenum.h"
#include "encryption.h"
#include <iostream>
#include "binary.h"
#include "checksum.h"
#include "bootgenexception.h"
#include "logger.h"
#include "authkeys.h"
#include "partitionheadertable.h"
#include "imageheadertable-versal.h"
#include "bifoptions.h"

#define R5_TCM_START_ADDRESS      0x0000
#define R5_BTCM_START_ADDRESS     0x20000
#define R5_TCM_BANK_LENGTH        0x10000
#define PMU_RAM_END_ADDRESS       0xFFDDFFFF

/* Forward class declarations */
class Options;
class BifOptions;
class PartitionBifOptions;
class BootHeader;
class ImageHeaderTable;
class PartitionHeaderTable;
class BootImage;
class Partition;
class ImageHeader;
class ElfFormat;
class Elf32ProgramHeader;
class BitFile;
class ByteFile;
class ChecksumContext;
class MD5ChecksumContext;
class Key;
class ChecksumTable;

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
class BIF_File
{
public:
    BIF_File(std::string& filename0) : biffilename(filename0) {} 
    void Process(Options& options);
    void ParseBifFile(Options& options);
    void ReplaceFiles();
private:
    std::string biffilename;
    void Output(Options& options, uint8_t index);
    std::vector<BifOptions*> bifOptionList;
    std::vector<BifOptions*> includeBifOptionList;
    std::vector<BootImage*> bootImages;
};

/******************************************************************************/
class BootImage 
{
public:
    BootImage(Options& options, uint8_t index);
    ~BootImage();

    virtual void Add(BifOptions* bifoptions) { };
    virtual void DetermineEncryptionDefaults() {};
    virtual void ParseBootImage(PartitionBifOptions * it) = 0;
    virtual void ConfigureProcessingStages(ImageHeader* image, PartitionBifOptions* partitionbifoptions) = 0;
    virtual void OutputOptionalEfuseHash();
    virtual void BuildAndLink(Binary* cache);
    void OutputPartitionFiles(Options& options, Binary& cache);
    void PrintPartitionInformation(void);
    void SetLegacyEncryptionFlag(bool flag);
    void GenerateAuthenticationKeys(void);
    void GenerateGreyKey(void);
    void SetOutputSplitModeFormat(SplitMode::Type splitMode, File::Type fmt);
    void ValidateOutputModes(File::Type split, OutputMode::Type outMode);
    bool IsBootloaderFound();
    virtual void OutputOptionalSecureDebugImage() {};

    void SetAssumeEncryptionFlag (bool);
    void SetCore (Core::Type);
    void SetPmuFwSize (uint32_t);
    void SetFsblFwSize (uint32_t);
    void SetTotalPmuFwSize (uint32_t);
    void SetTotalFsblFwSize (uint32_t);
    void SetFsblSourceAddr (Binary::Address_t);
    void SetOutputBitstreamModeFormat (File::Type);
    void SetCoreFromDestCpu (DestinationCPU::Type type, A53ExecState::Type procType);
    void SetDestCpuFromCore(Core::Type coreType, DestinationCPU::Type cpuType);
    
    static std::vector<std::string> encryptionKeyFileVec ;
    std::vector<std::string>& GetEncryptionKeyFileVec();
    void InsertEncryptionKeyFile(std::string filename);
    std::vector<std::pair<KeySource::Type, uint32_t*>> aesKeyandKeySrc;

    Core::Type GetCore(void);
    uint32_t GetPmuFwSize (void);
    uint32_t GetFsblFwSize (void);
    uint32_t GetTotalPmuFwSize (void);
    uint32_t GetTotalFsblFwSize (void);
    Binary::Address_t GetFsblSourceAddr (void);
    AuthHash::Type GetAuthHashAlgo(void);

    std::string Name;
    BootHeader* bootHeader;
    std::list<Section*> headers;
    Section* encryptedHeaders;
    ImageHeaderTable* imageHeaderTable;
    std::list<ImageHeader*> imageList;
    std::list<SubSysImageHeader*> subSysImageList;
    PartitionHeaderTable* partitionHeaderTable;
    std::list<PartitionHeader*> partitionHeaderList;
    Section* nullPartHeaderSection;
    ChecksumTable* checksumTable;
    AuthenticationCertificate* headerAC;
    AuthenticationContext* currentAuthCtx;
    AuthenticationContext* metaHdrAuthCtx;
    EncryptionContext* currentEncryptCtx;
    MD5ChecksumContext* currentChecksumCtx;
    Hash* hash;
    Options& options;
    BifOptions* bifOptions;
    KeyGenerationStruct* keygen;
    PartitionOutput* partitionOutput;
    bool headerSignature_Loaded;
    bool XipMode;
    uint32_t totalHeadersSize;
    uint32_t partCount;
    Binary* cache;
    bool createSubSystemPdis;
    bool convertAieElfToCdo;
    std::string fsblFilename;
    std::string bitFilename;    
    void * overlayCDO;

    virtual uint64_t GetSecureChunkSize(bool isBootloader) { return 0; }

    std::vector<uint32_t> sync_offsets;
//private:
    bool assumeEncryption;
    uint32_t* deviceKey;
    uint32_t* firstIv;
    uint32_t* firstOptKey;
    Core::Type core;
    AuthHash::Type authHash;
    uint32_t pmuFwSize;
    uint32_t fsblFwSize;
    uint32_t totalPmuFwSize;
    uint32_t totalFsblFwSize;
    Binary::Address_t sourceAddr;
    bool legacyEncryptionEnabled;
    bool bootloaderFound;
    bool bootloaderEncrypt;
    KeySource::Type bootloaderKeySource;
    bool bootloaderAuthenticate;
    uint32_t xplm_modules_data_length;
    uint32_t* xplm_modules_data;
    char globalSlrId;

    uint32_t* iht_optional_data;
    uint32_t iht_optional_data_length;
    std::string pmcDataAesFile;
};
#endif
