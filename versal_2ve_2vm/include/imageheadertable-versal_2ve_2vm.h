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
#pragma once

#ifndef _VERSAL_2VE_2VM_IMAGEHEADERTABLE_H_
#define _VERSAL_2VE_2VM_IMAGEHEADERTABLE_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#define MAX_NUM_PARTITIONS_VERSAL2   20
#define MAX_NUM_IMAGES_VERSAL2       10 

#include "imageheadertable.h"
#include "bootheader.h"
extern "C" {
#include "cdo-command.h"
}

class ImageBifOptions;
class SubSysImageHeader;
class BifOptions;
/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/

/* 128MB Linear QSPI range */
#define LQSPI_BASE_ADDR_VERSAL      (0xC0000000)
#define LQSPI_SIZE_VERSAL           (0x8000000) 
#define LQSPI_RANGE_MASK_VERSAL     (0x7FFFFFF)

#define MAX_IHT_RESERVED_VERSAL     8

/* Version Updates :
   v1 : Initial Telluride support */
#define VERSION_v1_00_TELLURIDE    0x00010000


/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
/* totalMetaHdrLength: Includes Authentication and encryption over head (excluding IHT and including AC) */
typedef struct
{
    uint32_t    version;                            // 0x0
    uint32_t    imageTotalCount;                    // 0x4
    uint32_t    firstImageHeaderWordOffset;         // 0x8
    uint32_t    partitionTotalCount;                // 0xc
    uint32_t    firstPartitionHeaderWordOffset;     // 0x10
    uint32_t    secondaryBootDeviceAddress;         // 0x14
    uint32_t    idCode;                             // 0x18
    uint32_t    imageHeaderTableAttributes;         // 0x1c
    uint32_t    pdiId;                              // 0x20
    uint32_t    parentId;                           // 0x24
    uint32_t    identificationString;               // 0x28
    uint32_t    headerSizes;                        // 0x2C
    uint32_t    totalMetaHdrLength;                 // 0x30
    uint32_t    metaHdrSecureHdrIv[IV_LENGTH];      // 0x34
    uint32_t    metaHdrKeySource;                   // 0x40
    uint32_t    extendedIdCode;                     // 0x44
    uint32_t    headerAuthCertificateWordOffset;    // 0x48
    uint32_t    metaHdrGreyOrBlackIV[IV_LENGTH];    // 0x4C
    uint32_t    optionalDataSize;                   // 0x58

    uint32_t authHeader1;                           // 0x5C
    uint32_t hashBlockLength1;                      // 0x60
    uint32_t hashBlockOffset;                       // 0x64
    uint32_t totalppkkSize1;                        // 0x68
    uint32_t actualppkSize1;                        // 0x6C
    uint32_t totalHashBlockSignatureSize1;          // 0x70
    uint32_t actualSignatureSize1;                  // 0x74
    uint32_t reserved;                              // 0x78

    //uint32_t    reserved[MAX_IHT_RESERVED_VERSAL];  // 0x5C - 0x78
    uint32_t    ihtChecksum;                        // 0x7C
} Versal_2ve_2vmImageHeaderTableStructure;

typedef struct
{
    uint32_t partitionHeaderWordOffset;         // 0x00
    uint32_t dataSectionCount;                  // 0x04
    uint32_t metaHdrRevokeId;                   // 0x08
    uint32_t imageAttributes;                   // 0x0C
    char     imageName[16];                     // 0x10 
    uint32_t imageId;                           // 0x20
    uint32_t uniqueId;                          // 0x24
    uint32_t parentUniqueId;                    // 0x28
    uint32_t functionId;                        // 0x2C
    uint32_t memcpyAddressLo;                   // 0x30
    uint32_t memcpyAddressHi;                   // 0x34
    uint16_t pcrNumber;                         // 0x38
    uint16_t pcrMeasurementIndex;               // 0x3A
    uint32_t ihChecksum;                        // 0x3C
} Versal_2ve_2vmImageHeaderStructure;

#define CDO_SSIT_SYNC_MASTER_CMD 0x0E010000
#define CDO_SSIT_SYNC_SLAVES_CMD 0x0F010200
#define CDO_CMD_END 0x00000100
#define CDO_IDENTIFICATION 0x004f4443


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/

class Versal_2ve_2vmImageHeaderTable : public ImageHeaderTable
{
public:
    Versal_2ve_2vmImageHeaderTable();
    Versal_2ve_2vmImageHeaderTable(std::ifstream& ifs);
    ~Versal_2ve_2vmImageHeaderTable();
    
    void Build(BootImage & bi, Binary & cache);
    void Link(BootImage & bi);
    void ValidateSecurityCombinations(Authentication::Type, Encryption::Type, Checksum::Type);
    void RealignSectionDataPtr(void);

    void SetImageHeaderTableVersion(uint32_t version);
    void SetHeaderTablesSize();
    void SetImageCount(uint32_t count);
    void SetFirstImageHeaderOffset(uint32_t offset);
    void SetFirstPartitionHeaderOffset(uint32_t offset);
    void SetPartitionCount(uint32_t count);
    void SetBootDeviceAddress(uint32_t address);
    void SetIds(bool warnIdCode);
    void SetIdentificationString(bool bootloader);
    void SetImageHeaderTableAttributes();
    void SetTotalMetaHdrLength(uint32_t size);
    void SetHashBlockLength(uint32_t size);
    void SetMetaHdrSecureHdrIv(uint8_t* iv);
    void SetMetaHdrKeySrc(KeySource::Type keyType, BifOptions* bifOptions);
    void SetMetaHdrGreyOrBlackIv(std::string);
    void SetHeaderAuthCertificateOffset(uint32_t offset);
    void SetHashBlockOffset(uint32_t address);
    void SetReservedFields(void);
    void SetChecksum(void);
    void SetOptionalDataSize(void);
    void SetOptionalData(uint32_t*, uint32_t);
    void SetUserOptionalData(std::vector<std::pair<std::string, uint32_t>> optionalDataInfo, uint32_t hashTableSize = 0, uint32_t isCorePsm = 0);
    void SetXplmModulesData(BootImage& bi, uint32_t*, uint32_t);

    uint32_t GetImageHeaderTableVersion(void);
    uint32_t GetPartitionCount(void);
    uint32_t GetImageCount(void);
    uint32_t GetFirstImageHeaderOffset(void);
    uint8_t GetMaxNumOfPartitions(void) { return MAX_NUM_PARTITIONS_VERSAL2; }
    uint32_t GetTotalMetaHdrLength();

    void SetCreatorId(uint8_t id);
    void SetPdiId(uint32_t id);
    void SetParentId(uint32_t id);
    std::list<SubSysImageHeader*> subSysImageList;

private:
    Versal_2ve_2vmImageHeaderTableStructure *iHTable;
    uint8_t creatorId;
    uint32_t parentId;
    uint32_t pdiId;
    uint32_t idCode;
    uint32_t extendedIdCode;
    uint8_t bootDevice;
    std::string kekIvFile;
    bool kekIvMust;
    bool bypassIdCode;
    bool prebuilt;
    DpaCM::Type dpacm;
    PufHdLoc::Type pufHDLoc;
};

/******************************************************************************/
class Versal_2ve_2vmImageHeader : public ImageHeader
{
public:
    Versal_2ve_2vmImageHeader(std::string& filename);
    Versal_2ve_2vmImageHeader(uint8_t* data, uint64_t len);
    Versal_2ve_2vmImageHeader(std::ifstream& ifs, bool isBootloader);
    Versal_2ve_2vmImageHeader(std::ifstream& ifs, Versal_2ve_2vmImageHeaderStructure* importedIH, bool isBootloader, uint32_t index);
    ~Versal_2ve_2vmImageHeader();
    
    void ImportElf(BootImage& bi);
    void Build(BootImage& bi, Binary& cache);
    void Link(BootImage &bi, PartitionHeader* partitionHeader, ImageHeader* nextImageheader);
    void SetMetaHdrRevokeId(uint32_t revocId);
    void ParseFileToImport(BootImage& bi);
    void ImportNpi(BootImage& bi);
    void ImportBuffer(BootImage &bi);
    void ImportBin(BootImage& bi);
    void ImportBit(BootImage& bi);
    void ImportCdoSource(BootImage& bi);
    void ImportCdo(BootImage& bi);
    uint8_t* DecodeCdo(std::string file, size_t* size);
    void ImportAieEngineElf(BootImage& bi);
    void CreateAieEnginePartition(BootImage& bi);
    uint64_t ImportAieEngineElfCdo(std::string);
    uint32_t CdoCmdDmaWrite(uint32_t pSize, uint64_t pAddr, uint8_t *databuffer);
    uint32_t CdoCmdWriteImageStore(uint32_t pSize, uint64_t id, uint8_t *databuffer);
    void SetLoadAndExecAddress(PartitionHeader *partHdr);
    std::list<std::string> ParseAieJson(const char* filename);
    std::list <std::string> GetAieFilesPath(std::string);
    Binary::Address_t GetAieEngineGlobalAddress(Binary::Address_t elfAddr);
    void CalculateAieEngineBaseAddress(uint32_t colNum, uint32_t rowNum);
    uint32_t CheckAieEngineDataMemoryBoundary(Binary::Address_t globalAddr, Binary::Length_t pSize);
    void ParseCdos(BootImage& bi, std::vector<std::string> filelist, uint8_t**, size_t*, bool);

    bool PostProcessCdo(const uint8_t* cdo_data, Binary::Length_t cdo_size);

    void SetPartitionHeaderOffset(uint32_t addr);
    void SetDataSectionCount(uint32_t cnt);
    void SetImageHeaderAttributes();
    void SetImageName(void);
    void SetPartitionUid(uint32_t id);
    void SetEncryptionKeySrc(KeySource::Type type);
    void SetImageId();
    void SetPartitionRevocationId(uint32_t id);
    void SetSpkRevocationId(uint32_t id);
    void SetMemCopyAddr();
    void SetChecksum(void);
    void SetAuthBlock(size_t blockSize, bool flag);
    void SetWriteImageStorePartitions(ImageStorePdiInfo*);
    void SetDpacm(DpaCM::Type);
    void SetPufHdLocation(PufHdLoc::Type type);

    uint32_t GetPartitionHeaderOffset(void);
    uint32_t GetDataSectionCount(void);
    uint32_t GetImageHeaderAttributes(void);
    uint32_t GetPartitionUid(void);
    KeySource::Type GetEncryptionKeySrc();
    uint32_t GetPartitionRevocationId();
    uint32_t GetSpkRevocationId();
    DpaCM::Type GetDpacm(void);
    PufHdLoc::Type GetPufHdLocation(void);
    std::string GetKekIV(void);

private:
    void ParseSlrConfigFiles(size_t* slr_total_file_size);
    void CheckIdsInCdo(CdoSequence * cdo_seq, std::string cdo_filename);
    void SetPowerDomains(uint8_t* buf, uint32_t count);
    void CreateWriteImageStorePartition();
    
    Versal_2ve_2vmImageHeaderStructure *imageHeader;
    std::unique_ptr<VersalCdoHeader> cdoHeader;
 
    uint64_t aie_array_base_address;
    Binary::Address_t coreBaseAddr;
    Binary::Address_t southBankBaseAddr;
    Binary::Address_t westBankBaseAddr;
    Binary::Address_t northBankBaseAddr;
    Binary::Address_t eastBankBaseAddr;

protected:
    static std::list<std::unique_ptr<CdoCommandDmaWrite>> cdoSections;  // Smart pointers
    uint8_t num_of_slrs;
};

/******************************************************************************/
class Versal_2ve_2vmSubSysImageHeader : public SubSysImageHeader
{
public:
    Versal_2ve_2vmSubSysImageHeader(ImageBifOptions * imgOptions);
    Versal_2ve_2vmSubSysImageHeader(std::ifstream & ifs);

    void Build(BootImage & bi, Binary & cache);
    void Link(BootImage &bi, SubSysImageHeader* nextheader);

    void SetPCRMeasurementIndex(bool versalnet);
    void SetPCRNumber(bool versalnet);
    void SetChecksum(void);
    void SetPartitionHeaderOffset(uint32_t addr);
    void SetDataSectionCount(void);
    void SetMetaHdrRevokeId(uint32_t id);
    void SetImageHeaderIds(void);
    void SetImageHeaderAttributes(void);
    void SetImageName(void);
    void SetMemCopyAddress(void);

    uint64_t GetMemCopyAddress(void);
    uint32_t GetPartitionHeaderOffset(void);
    uint32_t GetDataSectionCount(void);
    uint32_t GetImageHeaderAttributes(void);

protected:
    Versal_2ve_2vmImageHeaderStructure *versal_2ve_2vmSubSysImageHeaderTable;
};
#endif
