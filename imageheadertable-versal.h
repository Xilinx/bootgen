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
#pragma once

#ifndef _VERSAL_IMAGEHEADERTABLE_H_
#define _VERSAL_IMAGEHEADERTABLE_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/

#include "imageheadertable.h"
#include "bootheader.h"
#include "cdo-command.h"
class ImageBifOptions;
class SubSysImageHeader;
class BifOptions;
/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define MAX_NUM_PARTITIONS_VERSAL   32

/* 128MB Linear QSPI range */
#define LQSPI_BASE_ADDR_VERSAL      (0xC0000000)
#define LQSPI_SIZE_VERSAL           (0x8000000) 
#define LQSPI_RANGE_MASK_VERSAL     (0x7FFFFFF)

#define MAX_IHT_RESERVED_VERSAL     9

#define MAX_SLAVE_SLRS              3
#define CDO_COMMAND_SIZE            16
#define FPDI                        0x46504449
#define PPDI                        0x50504449

#define MASTER_PMC_BASE_ADDR        0xF0000000
#define SLR1_PMC_BASE_ADDR          0x108000000
#define SLR2_PMC_BASE_ADDR          0x110000000
#define SLR3_PMC_BASE_ADDR          0x118000000
#define PMC_SBI_BUF_ADDR            0xF2100000
#define SLR1_SBI_BUF_BASE_ADDR      (SLR1_PMC_BASE_ADDR + (PMC_SBI_BUF_ADDR - MASTER_PMC_BASE_ADDR))
#define SLR2_SBI_BUF_BASE_ADDR      (SLR2_PMC_BASE_ADDR + (PMC_SBI_BUF_ADDR - MASTER_PMC_BASE_ADDR))
#define SLR3_SBI_BUF_BASE_ADDR      (SLR3_PMC_BASE_ADDR + (PMC_SBI_BUF_ADDR - MASTER_PMC_BASE_ADDR))
#define SBI_KEYHOLE_SIZE            0x10000
#define AIE_BASE_ADDR               0x20000000000

typedef enum
{
    vihImageOwnerShift = 3,
    vihImageOwnerMask = 0x7,

    vihCopyToMemoryShift = 6,
    vihCopyToMemoryMask = 0x1,

    vihDelayLoadShift = 7,
    vihDelayLoadMask = 0x1,

    vihDelayHandoffShift = 8,
    vihDelayHandoffMask = 0x1,

    vihLowPowerDomainShift = 9,
    vihLowPowerDomainMask = 0x1,

    vihFullPowerDomainShift = 10,
    vihFullPowerDomainMask = 0x1,

    vihSystemPowerDomainShift = 11,
    vihSystemPowerDomainMask = 0x1,

    vihPLPowerDomainShift = 12,
    vihPLPowerDomainMask = 0x1,
} VersalIHAttributes;

typedef enum
{
    vihtIdCodeCheckShift = 0,
    vihtIdCodeCheckMask = 0x1,

    vihtImageCreatorIdShift = 1,
    vihtImageCreatorIdMask = 0x1F,

    vihtSecBootDeviceShift = 6,
    vihtSecBootDeviceMask = 0x3F,

    vihtDpacmEnableShift = 12,
    vihtDpacmEnableMask = 0x3,

    vihtPufHDLocationShift = 14,
    vihtPufHDLocationMask = 0x3,
} VersalIHTAttributes;


/* Version Updates :
   v1 : Initial versal support
   v2 : IHT,PHT sizes doubled
   v3 : Partition secure chunk size changed to 32k from 64k
   v4 : AAD support added for IHT
      : Hash placemnet updated during chunking
        Hash is included into the 32k secure chunk */
#define VERSION_v1_00_VERSAL    0x01030000
#define VERSION_v2_00_VERSAL    0x00020000
#define VERSION_v3_00_VERSAL    0x00030000
#define VERSION_v4_00_VERSAL    0x00040000


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
    uint32_t    reserved[MAX_IHT_RESERVED_VERSAL];  // Reserved
    uint32_t    ihtChecksum;                        // 0x7C
}VersalImageHeaderTableStructure;

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
    uint32_t reserved;                          // 0x38
    uint32_t ihChecksum;                        // 0x3C
} VersalImageHeaderStructure;

typedef struct
{
    uint32_t header;
    uint32_t length;
    uint32_t hi_address;
    uint32_t lo_address;
    uint8_t* data;
} CdoCommandDmaWrite;

typedef struct
{
    uint8_t cmd_id;
    uint8_t handler_id;
    uint8_t length;
    uint8_t reserved;
} CdoCommandHeader;

struct CdoCmds
{
    typedef enum
    {
        END,
        MASK_POLL,
        MASK_WRITE,
        WRITE,
        DELAY,
        DMA_WRITE,
        MASK_POLL_64,
        MASK_WRITE_64,
        WRITE_64,
        DMA_XFER,
        INIT_SEQ,
        CFI_READ,
        SET,
        WRITE_KEYHOLE,
        SSIT_SYNC_MASTER,
        SSIT_SYNC_SLAVES,
        SSIT_WAIT_SLAVES,
        NOP
    } Type;
};

#define CDO_CMD_DMA_WRITE_SIZE 16

typedef struct
{
    CdoCommandHeader header;
    uint32_t length;
    uint32_t hi_address;
    uint32_t lo_address;
    uint32_t keyhole_size;
    uint8_t* data;
} CdoCommandWriteKeyhole;

#define CDO_CMD_WRITE_KEYHOLE_SIZE 20

typedef struct
{
    CdoCommandHeader header;
} CdoCommandNop;
#define CDO_CMD_NOP_SIZE 4

typedef struct
{
    uint32_t header;
    uint32_t value;
} CdoCommandSync;

typedef struct
{
    CdoCommandHeader header;
} CdoSsitSyncMaster;

#define CDO_SSIT_SYNC_MASTER_CMD 0x0E010000
#define CDO_SSIT_SYNC_SLAVES_CMD 0x0F010200
#define CDO_CMD_END 0x00000100
#define CDO_IDENTIFICATION 0x004f4443
#define CDO_VERSION 0x00000200
#define CDO_REMAINING_WORDS 0x4

typedef struct
{
    CdoCommandHeader header;
    uint32_t mask;
    uint32_t timeout_ms;
} CdoSsitSlaves;

typedef struct
{
    uint32_t remaining_words;
    uint32_t id_word;
    uint32_t version;
    uint32_t length;
    uint32_t checksum;
} VersalCdoHeader;


typedef struct
{
    std::string file;                       /* SLR partition PDI/CDO file */
    SlrId::Type index;                      /* SLR index - master, slr-1, slr-2 etc. */
    uint8_t* data;                          /* SLR partition data */
    size_t size;                            /* SLR partition size */
    uint32_t offset;                        /* To track the offset of data pointer for packing next chunk */
    std::vector<uint32_t> sync_addresses;   /* List of addresses for CDO_SSIT_SYNC_MASTER_CMD/CDO_SSIT_SYNC_SLAVES_CMD */
    std::vector<uint32_t> partition_sizes;  /* Individual partition sizes of each SLR PDI */
    uint32_t partition_index;               /* To track current partition of SLR PDI to chunk and pack */
    uint32_t partition_offset;              /* To track current partition of SLR PDI to chunk and pack */
    uint32_t sync_points;                   /* To track how many sync points are processed within SLR PDI */
    uint32_t num_chunks;                    /* To track no of chunks created or processed */
    bool eof;                               /* To track end of file for each SLR PDI */
} SsitConfigSlrInfo;

typedef struct
{
    uint8_t slr_num;
    size_t offset;
    size_t size;
    size_t sync_points;
} SsitConfigSlrLog;
/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class VersalImageHeaderTable : public ImageHeaderTable
{
public:
    VersalImageHeaderTable();
    VersalImageHeaderTable(std::ifstream& ifs);
    ~VersalImageHeaderTable();
    
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
    void SetIds(void);
    void SetIdentificationString(bool bootloader);
    void SetImageHeaderTableAttributes();
    void SetTotalMetaHdrLength(uint32_t size);
    void SetMetaHdrSecureHdrIv(uint8_t* iv);
    void SetMetaHdrKeySrc(KeySource::Type keyType, BifOptions* bifOptions);
    void SetMetaHdrGreyOrBlackIv(std::string);
    void SetHeaderAuthCertificateOffset(uint32_t offset);
    void SetReservedFields(void);
    void SetChecksum(void);

    uint32_t GetImageHeaderTableVersion(void);
    uint32_t GetPartitionCount(void);
    uint32_t GetImageCount(void);
    uint32_t GetFirstImageHeaderOffset(void);
    uint8_t GetMaxNumOfPartitions(void) { return MAX_NUM_PARTITIONS_VERSAL; }
    uint32_t GetTotalMetaHdrLength();

    void SetCreatorId(uint8_t id);
    void SetPdiId(uint32_t id);
    void SetParentId(uint32_t id);
    std::list<SubSysImageHeader*> subSysImageList;

private:
    VersalImageHeaderTableStructure *iHTable;
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
class VersalImageHeader : public ImageHeader
{
public:
    VersalImageHeader(std::string& filename);
    VersalImageHeader(uint8_t* data, uint64_t len);
    VersalImageHeader(std::ifstream& ifs, bool isBootloader);
    VersalImageHeader(std::ifstream& ifs, VersalImageHeaderStructure* importedIH, bool isBootloader, uint32_t index);
    ~VersalImageHeader();
    
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
    void SetLoadAndExecAddress(PartitionHeader *partHdr);
    std::list<std::string> ParseAieJson(const char* filename);
    std::list <std::string> GetAieFilesPath(std::string);
    Binary::Address_t GetAieEngineGlobalAddress(Binary::Address_t elfAddr);
    void CalculateAieEngineBaseAddress(uint32_t colNum, uint32_t rowNum);
    uint32_t CheckAieEngineDataMemoryBoundary(Binary::Address_t globalAddr, Binary::Length_t pSize);
    void CreateSlrBootPartition(BootImage& bi);
    void CreateSlrConfigPartition(BootImage& bi);
    void ParseCdos(BootImage& bi, std::vector<std::string> filelist, uint8_t**, size_t*);
    //post-processing
    bool PostProcessCdo(const uint8_t* cdo_data, Binary::Length_t cdo_size);
    bool PostProcessCfi(const uint8_t* cdo_data, Binary::Length_t cdo_size);

    void SetPartitionHeaderOffset(uint32_t addr);
    void SetDataSectionCount(uint32_t cnt);
    void SetImageHeaderAttributes();
    void SetImageName(void);
    void SetPartitionUid(uint32_t id);
    void SetEncryptionKeySrc(KeySource::Type type);
    void SetImageId();
    void SetPartitionRevocationId(uint32_t id);
    void SetMemCopyAddr();
    void SetReservedFields(void);
    void SetChecksum(void);
    void SetAuthBlock(size_t blockSize, bool flag);
    void SetSlrBootPartitions(std::list<SlrPdiInfo*>);
    void SetSlrConfigPartitions(std::list<SlrPdiInfo*>);
    void SetDpacm(DpaCM::Type);
    void SetPufHdLocation(PufHdLoc::Type type);

    uint32_t GetPartitionHeaderOffset(void);
    uint32_t GetDataSectionCount(void);
    uint32_t GetImageHeaderAttributes(void);
    uint32_t GetPartitionUid(void);
    KeySource::Type GetEncryptionKeySrc();
    uint32_t GetPartitionRevocationId();
    DpaCM::Type GetDpacm(void);
    PufHdLoc::Type GetPufHdLocation(void);
    std::string GetKekIV(void);

private:
    void ParseSlrConfigFiles(size_t* slr_total_file_size);
    void CheckSyncPointInChunk(SsitConfigSlrInfo* slr_info, size_t size);
    uint32_t FindCurrentSyncPoint(void);
    void CheckIdsInCdo(CdoSequence * cdo_seq);
    void SetPowerDomains(uint8_t* buf, uint32_t count);
    void LogConfigSlrDetails(size_t chunk_num, uint8_t slr_num, size_t offset, size_t chunk_size, size_t sync_points);
    void PrintConfigSlrSummary(void);
    uint64_t slr_total_file_size;

    std::vector<SsitConfigSlrInfo*> configSlrsInfo;
    std::vector<SsitConfigSlrLog*> configSlrLog;
    VersalImageHeaderStructure *imageHeader;
    VersalCdoHeader* cdoHeader;
 
    uint64_t aie_array_base_address;
    Binary::Address_t coreBaseAddr;
    Binary::Address_t southBankBaseAddr;
    Binary::Address_t westBankBaseAddr;
    Binary::Address_t northBankBaseAddr;
    Binary::Address_t eastBankBaseAddr;

protected:
    static std::list<CdoCommandDmaWrite*> cdoSections;
    uint8_t num_of_slrs;
};

/******************************************************************************/
class SubSysImageHeader : public BaseThing
{
public:
    SubSysImageHeader(ImageBifOptions *imgOptions);
    SubSysImageHeader(std::ifstream& ifs);

    ~SubSysImageHeader()
    {
        if (section != NULL)
        {
            delete section;
        }
    }

    void Build(BootImage& bi, Binary& cache);
    void Link(BootImage &bi, SubSysImageHeader* nextheader);
    void SetSubSystemName(std::string);
    void SetSubSystemId(uint32_t);
    void SetSubSystemType(PartitionType::Type);
    void SetDelayHandoffMode(bool);
    void SetDelayLoadMode(bool);
    void SetMemCopyAddress(void);
    void SetSubSysMemCopyAddress(uint64_t);
    void SetPartitionHeaderOffset(uint32_t addr);
    void SetDataSectionCount(void);
    void SetMetaHdrRevokeId(uint32_t id);
    void SetImageHeaderAttributes();
    void SetImageName(void);
    void SetImageHeaderIds();
    void SetReservedFields(void);
    void SetChecksum(void);

    uint32_t GetPartitionHeaderOffset(void);
    uint32_t GetDataSectionCount(void);
    uint32_t GetImageHeaderAttributes(void);
    bool GetDelayLoadMode(void);
    bool GetDelayHandoffMode(void);
    uint64_t GetSubSysMemCopyAddress(void);
    uint64_t GetMemCopyAddress(void);
    PartitionType::Type GetSubSystemType(void);
    std::string GetSubSystemName(void);
    uint32_t GetSubSystemId(void);

    uint32_t num_of_images;
    std::list<ImageHeader*> imgList;
    std::list<std::string> partitionNameList;
protected:
    VersalImageHeaderStructure *subSysImageHeaderTable;
    uint32_t imageId;
    std::string imageName;
    bool delayHandoff;
    bool delayLoad;
    bool isFullPowerDomain;
    bool isLowPowerDomain;
    bool isSystemPowerDomain;
    bool isPLPowerDomain;
    uint64_t memCopyAddr;
    PartitionType::Type imageType;
    uint32_t uniqueId;
    uint32_t parentUniqueId;
    uint32_t functionId;
};

#endif
