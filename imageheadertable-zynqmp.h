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

#pragma once

#ifndef _IMAGEHEADERTABLE_ZYNQMP_H_
#define _IMAGEHEADERTABLE_ZYNQMP_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "imageheadertable.h"
#include "partitionheadertable-zynqmp.h"
#include "bootimage.h"
#include "fileutils.h"
#include "elftools.h"
#include "bitutils.h"


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define MAX_IHT_RESERVED_ZYNQMP     9
#define MAX_NUM_PARTITIONS_ZYNQMP   32

/* 128MB Linear QSPI range */
#define LQSPI_BASE_ADDR_ZYNQMP      (0xC0000000)
#define LQSPI_SIZE_ZYNQMP           (0x8000000) 
#define LQSPI_RANGE_MASK_ZYNQMP     (0x7FFFFFF)


/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
typedef struct
{
    uint32_t version;                           // 0x00
    uint32_t partitionTotalCount;               // 0x04
    uint32_t firstPartitionHeaderWordOffset;    // 0x08
    uint32_t firstImageHeaderWordOffset;        // 0x0C
    uint32_t headerAuthCertificateWordOffset;   // 0x10
    uint32_t bootDevice;                        // 0x14
    uint32_t reserved[MAX_IHT_RESERVED_ZYNQMP]; // 0x18
    uint32_t ihtChecksum;                       // 0x3C
} ZynqMpImageHeaderTableStructure;


typedef struct
{
    uint32_t nextImageHeaderWordOffset;         // 0x00
    uint32_t partitionHeaderWordOffset;         // 0x04
    uint32_t dataSectionCount;                  // 0x08
    uint32_t imageNameLength;                   // 0x10
    char     imageName[1];                      // 0x14
} ZynqMpImageHeaderStructure;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class ZynqMpImageHeaderTable : public ImageHeaderTable
{
public:
    ZynqMpImageHeaderTable();
    ZynqMpImageHeaderTable(std::ifstream& ifs);
    ~ZynqMpImageHeaderTable();

    void ValidateSecurityCombinations(Authentication::Type, Encryption::Type, Checksum::Type);
    void RealignSectionDataPtr(void);

    void SetImageHeaderTableVersion(uint32_t version);
    void SetPartitionCount(uint32_t count);
    void SetFirstPartitionHeaderOffset(uint32_t offset);
    void SetFirstImageHeaderOffset(uint32_t offset);
    void SetHeaderAuthCertificateOffset(uint32_t offset);
    void SetReservedFields(void);
    void SetChecksum(void);
    void SetBootDevice(BootDevice::Type type);
    
    uint32_t GetImageHeaderTableVersion(void);
    uint32_t GetPartitionCount(void);
    uint32_t GetFirstPartitionHeaderOffset(void);
    uint32_t GetFirstImageHeaderOffset(void);
    uint32_t GetHeaderAuthCertificateOffset(void);
    uint8_t	GetMaxNumOfPartitions(void) { return MAX_NUM_PARTITIONS_ZYNQMP; }

private:
    ZynqMpImageHeaderTableStructure *iHTable;
};

/******************************************************************************/
class ZynqMpImageHeader : public ImageHeader
{
public:
    ZynqMpImageHeader(std::string& filename);
    ZynqMpImageHeader(std::ifstream& f);
    ~ZynqMpImageHeader();

    void Build(BootImage& bi, Binary& cache);
    void Link(BootImage &bi, PartitionHeader* partitionHeader, ImageHeader* nextImageheader);
    void ValidateEncryptionAttributes(BootImage & bi);
    void SetAuthBlock(size_t blockSize, bool flag);

    void ImportFpgaDataFile(BootImage & bi);
    void ImportBit(BootImage & bi);
    void ImportBin(BootImage & bi);
    void CreateElfPartitions(BootImage& bi, ElfFormat* elf, uint8_t proc_state);
    uint8_t* AttachPmuFw(uint8_t* partition_data, uint64_t* total_size, std::string pmu_fw);
    uint64_t GetExecAddrForXip(uint64_t execAddr);
    Binary::Address_t GetNextImageHeaderOffset();
    uint32_t GetPartitionHeaderOffset(void);
    uint32_t GetDataSectionCount(void);
    uint32_t GetImageNameLength(void);
    void SetNextImageHeaderOffset(uint32_t addr);
    void SetPartitionHeaderOffset(uint32_t addr);
    void SetDataSectionCount(uint32_t cnt);
    void SetImageNameLength(uint32_t len);
    void SetImageName(void);
    void SetBigEndian(bool);
    void SetA32ExecMode(bool);

private:
    ZynqMpImageHeaderStructure *imageHeader;
};

#endif
