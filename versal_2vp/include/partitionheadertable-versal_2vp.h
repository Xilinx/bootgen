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

#ifndef _PARTITIONHEADERTABLE_VERSAL_2VP_H_
#define _PARTITIONHEADERTABLE_VERSAL_2VP_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "partitionheadertable.h"
#include "imageheadertable-versal_2vp.h"
#include "bootimage.h"
#include "stringutils.h"
#include "options.h"
#include "logger.h"
#include "encryptutils.h"
#include <string.h>
#include <fstream>


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/

#define MAX_PHT_RESERVED_VERSAL             9

typedef enum
{
    v2vpphtTrustzoneShift = 0,
    v2vpphtTrustzoneMask = 0x1,
     
    v2vpphtExceptionLevelShift = 1,
    v2vpphtExceptionLevelMask = 0x3,
     
    v2vpphtExecStateShift = 3,
    v2vpphtExecStateMask = 0x1,
     
    v2vpphtlockStepShift = 4,
    v2vpphtlockStepMask = 0x3,
     
    v2vpphtDestCpuShift = 8,
    v2vpphtDestCpuMask = 0xF,
     
    v2vpphtChecksumTypeShift = 12,
    v2vpphtChecksumTypeMask = 0x3,
     
    v2vpphtPufHDLocationShift = 14,
    v2vpphtPufHDLocationMask = 0x3,
     
    v2vpphtPartitionOwnerShift = 16,
    v2vpphtPartitionOwnerMask = 0x3,
     
    v2vpphtEndiannessShift = 18,
    v2vpphtEndiannessMask = 0x1,
     
    v2vpphtTcmBootShift = 19,
    v2vpphtTcmBootMask = 0x3,

    v2vpphtHivecShift = 23,
    v2vpphtHivecMask = 0x1,

    v2vpphtPartitionTypeShift = 24,
    v2vpphtPartitionTypeMask = 0x7,

    v2vpphtDpaCMShift = 27,
    v2vpphtDpaCMMask = 0x3,

    v2vpphtClusterShift = 29,
    v2vpphtClusterMask = 0x7,
} Versal2vpPHTAttributes;


/* Keys Source */
#define USER_KEY0                       0xC5C3A5A3
#define USER_KEY1                       0xC3A5C5B3
#define USER_KEY2                       0xC5C3A5C3
#define USER_KEY3                       0xC3A5C5D3
#define USER_KEY4                       0xC5C3A5E3
#define USER_KEY5                       0xC3A5C5F3
#define USER_KEY6                       0xC5C3A563
#define USER_KEY7                       0xC3A5C573

#define EFUSE_USER_KEY0                 0x5C3CA5A3
#define EFUSE_USER_BLK_KEY0             0x5C3CA5A5
#define EFUSE_USER_GRY_KEY0             0x5C3CA5A7

#define EFUSE_USER_KEY1                 0xC3A5C5A3
#define EFUSE_USER_BLK_KEY1             0xC3A5C5A5
#define EFUSE_USER_GRY_KEY1             0xC3A5C5A7

#define SECURE_4K_CHUNK              0x1000
#define SECURE_8K_CHUNK              0x2000
#define SECURE_16K_CHUNK             0x4000
#define SECURE_32K_CHUNK             0x8000   /* 32 KB = 32*1024 B */
#define SECURE_64K_CHUNK             0x10000  /* 64 KB = 64*1024 B */

/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
typedef struct
{
    uint32_t encryptedPartitionLength;              // 0x00
    uint32_t unencryptedPartitionLength;            // 0x04
    uint32_t totalPartitionLength;                  // 0x08
    uint32_t nextPartitionHeaderOffset;             // 0x0C
    uint64_t destinationExecAddress;                // 0x10
    uint64_t destinationLoadAddress;                // 0x18
    uint32_t partitionWordOffset;                   // 0x20
    uint32_t partitionAttributes;                   // 0x24
    uint32_t dataSectionCount;                      // 0x28
    uint32_t checksumWordOffset;                    // 0x2C
    uint32_t puid;                                  // 0x30
    uint32_t authCertificateOffset;                 // 0x34
    uint32_t partitionSecureHdrIv[IV_LENGTH];       // 0x38
    uint32_t partitionKeySource;                    // 0x44
    uint32_t partitionGreyOrBlackIV[IV_LENGTH];     // 0x48
    uint32_t partitionRevokeId;                     // 0x54
    uint32_t measuredBootAddress;                   // 0x58
    uint32_t hashBlockLength;                       // 0x5C - Hash block length for Partition (in words)
    uint32_t hashBlockOffset;                       // 0x60 - Partition hash block offset (in words)
    uint32_t reserved[6];                           // 0x64-0x78 - Reserved (0)
    uint32_t pHChecksum;                            // 0x7C
} Versal_2vpPartitionHeaderTableStructure;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class Versal_2vpPartitionHeader : public PartitionHeader
{
public:
    Versal_2vpPartitionHeader(ImageHeader* imageheader, int index0);
    ~Versal_2vpPartitionHeader();

    void ReadHeader(std::ifstream& ifs);
    void ReadData(std::ifstream& ifs);
    void Build(BootImage & bi, Binary & cache);
    void Link(BootImage & bi, PartitionHeader* next_part_hdr);
    void RealignSectionDataPtr(void);

    bool IsFirstPartitionInImage()
    {
        return index == 0;
    }

    void SetPartitionSecureHdrIv(uint8_t * iv);
    void SetPartitionKeySrc(KeySource::Type, BifOptions*);
    void SetPartitionGreyOrBlackIv(std::string);
    void SetEncryptedPartitionLength(uint32_t len);
    void SetUnencryptedPartitionLength(uint32_t len);
    void SetTotalPartitionLength(uint32_t len);
    void SetLoadAddress(uint64_t addr, bool versalNetSeries);
    void SetExecAddress(uint64_t addr);
    void SetPartitionWordOffset(uint32_t addr);
    void SetHashBlockWordOffset(uint32_t addr);
    void SetHashBlockLength(uint32_t length);
    void SetPartitionAttributes(void);
    void SetSectionCount(uint32_t cnt);
    void SetChecksumOffset(void);
    void SetImageHeaderOffset(uint32_t offset) {};
    void SetAuthCertificateOffset(void);
    void SetPartitionId(void);
    void SetChecksum(void);
    void SetReserved(void);
    void SetNextPartitionHeaderOffset(uint32_t addr);
    void SetPartitionRevokeId(uint32_t id);

    KeySource::Type GetPartitionKeySource(void);
    uint8_t GetEncryptFlag(void);
    uint8_t GetAuthCertFlag(void);

    bool IsBootloader(void) { return isBootloader; }
    bool IsPmcdata(void) { return isPmcdata; }
    uint8_t GetChecksumType(void);
    uint8_t GetOwnerType(void);
    uint8_t GetDestinationCpu(void);
    uint8_t GetDestinationCluster(void);
    uint8_t GetProcessorExecState(void);
    uint8_t GetElfEndianess(void);
    uint8_t GetExceptionLevel(void);
    uint8_t GetTrustZone(void);
    uint8_t GetHivec(void);
    uint32_t GetPartitionHeaderSize(void);
    uint32_t GetChecksum(void);
    uint64_t GetExecAddress(void);
    uint64_t GetLoadAddress(void);
    uint32_t GetEncryptedPartitionLength(void);
    uint32_t GetUnencryptedPartitionLength(void);
    uint32_t GetTotalPartitionLength(void);
    uint32_t GetAuthCertificateOffset(void);
    uint32_t GetPartitionWordOffset(void);
    uint32_t GetPartitionPadSize64bBoundary(Section*);
    PartitionType::Type GetPartitionType(void);
    DpaCM::Type GetDpaCMFlag(void);
    PufHdLoc::Type GetPufHdLocation(void);
    Lockstep::Type GetLockStepFlag(void);
    uint32_t GetPartitionUid(void);
    uint64_t GetLQspiExecAddrForXip(uint64_t addr);
    uint32_t GetSectionCount(void);
    TcmBoot::Type GetTcmBootFlag(void);

private:
    uint8_t partitionEncrypted;
    uint8_t slr;
    uint8_t cluster;
    Lockstep::Type lockstep;
    uint32_t partitionUid;
    KeySource::Type partitionKeySrc;
    bool kekIvMust;
    std::string kekIvFile;
    DpaCM::Type dpaCM;
    PufHdLoc::Type pufHdLoc;
    TcmBoot::Type tcmBoot;
    Versal_2vpPartitionHeaderTableStructure* pHTable;

    Section* groupAcSection = NULL;
};

/******************************************************************************/
class Versal_2vpPartitionHeaderTable : public PartitionHeaderTable
{
public:
    void Build(BootImage& bi, Binary& cache);
    void BuildBootloaderPartitions(BootImage& bi, Binary& cache);
    void BuildNonBootloaderPartitions(BootImage& bi, Binary& cache);
    void ConfigureMetaHdrAuthenticationContext(BootImage& bi);
    void ConfigureMetaHdrHybridAuthenticationContext(BootImage& bi, ImageHeader* bootloaderImage);
    void UpdateAtfHandoffParams(BootImage& bi);
    void Link(BootImage& bi);
    Section* firstSection;
};

/******************************************************************************/
class Versal_2vpPartition : public Partition
{
public:
    Versal_2vpPartition(PartitionHeader* hdr, Section* section0);
    Versal_2vpPartition(PartitionHeader* hdr, const uint8_t* data, Binary::Length_t length);
    size_t GetTotalDataChunks(Binary::Length_t partitionSize, std::vector<uint32_t>& dataChunks, bool encryptionFlag);
    size_t GetBootloaderTotalDataChunks(Binary::Length_t partitionSize, std::vector<uint32_t>& dataChunks, bool encryptionFlag);
    void ChunkifyAndHash(Section * section, bool encryptionFlag);
    void Build(BootImage& bi, Binary& cache);
    void Link(BootImage& bi);
    void CalculateChunkificationHash(uint8_t* hash, uint8_t* data, size_t inlen, bool padding);
    void CopyPlmPmcHashesToBootloaderHashBlock(Section* section, BootImage& bi); //Modified
    void DumpPCRHashes(BootImage & bi);
    Section* GetHashBlockSection() const { return hashBlockSection; }
    uint32_t GetHashBlockSectionLength() const { return hashBlockSectionLength; }

private:
    PartitionHeader* header;
    uint64_t firstChunkSize;
    uint64_t secureChunkSize;
    uint64_t hashBlockLength;
    int pmcdataChunkCount;
    uint8_t chunkificationHashLength;
    size_t totalHashBlockSignatureLength;
    Section* hashBlockSection = NULL;
    uint32_t hashBlockSectionLength = 0;
};
#endif
