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

#ifndef _PARTITIONHEADERTABLE_VERSAL_H_
#define _PARTITIONHEADERTABLE_VERSAL_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "partitionheadertable.h"
#include "imageheadertable-versal.h"
#include "bootimage.h"
#include "stringutils.h"
#include "options.h"
#include "logger.h"
#include "encryptutils.h"
#include "imageheadertable-versal.h"
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
    vphtTrustzoneShift = 0,
    vphtTrustzoneMask = 0x1,

    vphtExceptionLevelShift = 1,
    vphtExceptionLevelMask = 0x3,

    vphtExecStateShift = 3,
    vphtExecStateMask = 0x1,

    vphtDestCpuShift = 8,
    vphtDestCpuMask = 0xF,

    vphtChecksumTypeShift = 12,
    vphtChecksumTypeMask = 0x3,

    vphtPufHDLocationShift = 14,
    vphtPufHDLocationMask = 0x3,

    vphtPartitionOwnerShift = 16,
    vphtPartitionOwnerMask = 0x3,

    vphtEndiannessShift = 18,
    vphtEndiannessMask = 0x1,

    vphtHivecShift = 23,
    vphtHivecMask = 0x1,

    vphtPartitionTypeShift = 24,
    vphtPartitionTypeMask = 0x7,

    vphtDpaCMShift = 27,
    vphtDpaCMMask = 0x3,
} VersalPHTAttributes;


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
    uint32_t reserved[MAX_PHT_RESERVED_VERSAL];     // 0x58
    uint32_t pHChecksum;                            // 0x7C
} VersalPartitionHeaderTableStructure;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class VersalPartitionHeader : public PartitionHeader
{
public:
    VersalPartitionHeader(ImageHeader* imageheader, int index0);
    ~VersalPartitionHeader();

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
    void SetLoadAddress(uint64_t addr);
    void SetExecAddress(uint64_t addr);
    void SetPartitionWordOffset(uint32_t addr);
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
    uint32_t GetPartitionUid(void);
    uint64_t GetLQspiExecAddrForXip(uint64_t addr);
    uint32_t GetSectionCount(void);

private:
    uint8_t partitionEncrypted;
    uint8_t slr;
    uint32_t partitionUid;
    KeySource::Type partitionKeySrc;
    bool kekIvMust;
    std::string kekIvFile;
    DpaCM::Type dpaCM;
    PufHdLoc::Type pufHdLoc;
    VersalPartitionHeaderTableStructure* pHTable;
};

/******************************************************************************/
class VersalPartitionHeaderTable : public PartitionHeaderTable
{
public:
    void Build(BootImage& bi, Binary& cache);
    void ConfigureMetaHdrAuthenticationContext(BootImage& bi);
    void Link(BootImage& bi);
    Section* firstSection;
};

/******************************************************************************/
class VersalPartition : public Partition
{
public:
    VersalPartition(PartitionHeader* hdr, Section* section0);
    VersalPartition(PartitionHeader* hdr, const uint8_t* data, Binary::Length_t length);
    size_t GetTotalDataChunks(Binary::Length_t partitionSize, std::vector<uint32_t>& dataChunks, bool encryptionFlag);
    void ChunkifyAndHash(Section * section, bool encryptionFlag);
    void Build(BootImage& bi, Binary& cache);
    void Link(BootImage& bi);
    static uint64_t GetSecureChunkSize(void);

private:
    PartitionHeader* header;
    uint64_t firstChunkSize;
};
#endif
