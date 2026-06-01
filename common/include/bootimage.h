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
#include <map>
#include <list>
#include <string>
#include <tuple>
#include "bootgenenum.h"
#include "encryption.h"
#include <iostream>
#include "binary.h"
#include "checksum.h"
#include "bootgenexception.h"
#include "logger.h"
//#include "authkeys.h"
#include "partitionheadertable.h"
#include "imageheadertable-versal.h"
#include "imageheadertable-spartanup.h"
#include "imageheadertable-versal_2ve_2vm.h"
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
    ~BIF_File(); // Destructor to clean up bootImages
    void Process(Options& options);
    void ParseBifFile(Options& options);
    void AppendAndReplaceFilesinBIF();
private:
    std::string biffilename;
    void Output(Options& options, uint8_t index);
    std::vector<BifOptions*> bifOptionList;
    std::vector<BifOptions*> includeBifOptionList;
    std::vector<std::unique_ptr<BootImage>> bootImages;
};

/******************************************************************************/
class BootImage 
{
public:
    BootImage(Options& options, uint8_t index);
    virtual ~BootImage();  // Made virtual for proper polymorphic deletion

    virtual void Add(BifOptions* bifoptions) { };
    virtual void DetermineEncryptionDefaults() {};
    virtual void ParseBootImage(PartitionBifOptions * it) = 0;
    virtual void ConfigureProcessingStages(ImageHeader* image, PartitionBifOptions* partitionbifoptions) = 0;
    virtual void OutputOptionalEfuseHash();
    virtual void OutputOptionalEfusePufHash() {};
    virtual void OutputOptionalPufPDI() {};
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
    void SetDeviceArchitecture();

    static std::vector<std::string> encryptionKeyFileVec ;
    std::vector<std::string>& GetEncryptionKeyFileVec();
    void InsertEncryptionKeyFile(std::string filename);
    std::vector<std::pair<KeySource::Type, std::unique_ptr<uint32_t[]>>> aesKeyandKeySrc;  // Smart pointers for AES keys
    std::map<std::tuple<std::string, std::string, uint32_t>, std::vector<uint8_t>> spkFileAndSpkSignature;

    Core::Type GetCore(void);
    uint32_t GetPmuFwSize (void);
    uint32_t GetFsblFwSize (void);
    uint32_t GetTotalPmuFwSize (void);
    uint32_t GetTotalFsblFwSize (void);
    Binary::Address_t GetFsblSourceAddr (void);
    AuthHash::Type GetAuthHashAlgo(void);

    std::string Name;
    std::unique_ptr<BootHeader> bootHeader;
    std::list<Section*> headers;
    Section* encryptedHeaders;
    std::unique_ptr<ImageHeaderTable> imageHeaderTable;
    std::list<ImageHeader*> imageList;
    std::list<SubSysImageHeader*> subSysImageList;
    std::unique_ptr<PartitionHeaderTable> partitionHeaderTable;
    std::list<PartitionHeader*> partitionHeaderList;
    Section* nullPartHeaderSection;
    std::unique_ptr<ChecksumTable> checksumTable;
    std::unique_ptr<AuthenticationCertificate> headerAC;
    std::unique_ptr<AuthenticationContext> currentAuthCtx;
    std::unique_ptr<AuthenticationContext> metaHdrAuthCtx;
    std::unique_ptr<EncryptionContext> currentEncryptCtx;
    // Container to store all authentication contexts created during build phase
    // This ensures contexts stay alive through Link phase (avoid use-after-free)
    std::vector<std::unique_ptr<AuthenticationContext>> authContexts;
    MD5ChecksumContext* currentChecksumCtx;
    std::unique_ptr<Hash> hash;
    Options& options;
    BifOptions* bifOptions;
    std::unique_ptr<KeyGenerationStruct> keygen;
    std::unique_ptr<PartitionOutput> partitionOutput;
    bool headerSignature_Loaded;
    bool XipMode;
    uint32_t totalHeadersSize;
    uint32_t partCount;
    std::unique_ptr<Binary> cache;
    bool createSubSystemPdis;
    bool convertAieElfToCdo;
    std::string fsblFilename;
    std::string bitFilename;    
    void * overlayCDO;
    virtual uint64_t GetSecureChunkSize(bool isBootloader) { return 0; }
    Arch::Type GetDeviceArchitecture(void) { return arch; }

    std::vector<uint32_t> sync_offsets;
//private:
    //check-R Earlier in options - Now imported to bi, since they are not actually from cmd line
    bool assumeEncryption;
    std::unique_ptr<uint32_t[]> deviceKey;
    std::unique_ptr<uint32_t[]> firstIv;
    std::unique_ptr<uint32_t[]> firstOptKey;
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
    std::unique_ptr<uint32_t[]> xplm_modules_data;  // Smart pointer - auto cleanup
    char globalSlrId;
    bool authOnPartitionFound;

    std::unique_ptr<uint32_t[]> iht_optional_data;  // Changed from raw pointer to smart pointer for automatic memory management
    uint32_t iht_optional_data_length;
    uint32_t copied_iht_optional_data_length;
    std::vector<std::pair<uint32_t, std::unique_ptr<uint8_t[]>>> hashTable;
    uint32_t numHashTableEntries;
    std::vector<std::pair<uint32_t, uint32_t>> hashNumMap;
    std::string pmcDataAesFile;

    std::unique_ptr<uint32_t[]> hashBlock;  // Smart pointer - auto cleanup
    uint32_t hashBlockLength;

    Arch::Type arch;
};
#endif