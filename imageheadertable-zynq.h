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

#ifndef _IMAGEHEADERTABLE_ZYNQ_H_
#define _IMAGEHEADERTABLE_ZYNQ_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "imageheadertable.h"
#include "partitionheadertable-zynq.h"
#include "bootimage.h"
#include "fileutils.h"
#include "elftools.h"
#include "bitutils.h"


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define MAX_IHT_RESERVED_ZYNQ       11
#define MAX_NUM_PARTITIONS_ZYNQ     14

/* 32MB Linear QSPI range */
#define LQSPI_BASE_ADDR_ZYNQ        (0xFC000000)
#define LQSPI_SIZE_ZYNQ             (0x2000000) 
#define LQSPI_RANGE_MASK_ZYNQ       (0x1FFFFFF)

/* 32MB + 32MB SMC (NOR0 + NOR1) ranges */
#define NOR_BASE_ADDR_ZYNQ          (0xE2000000)
#define NOR_SIZE_ZYNQ               (0x4000000) 
#define NOR_RANGE_MASK_ZYNQ         (0x3FFFFFF)

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
    uint32_t reserved[MAX_IHT_RESERVED_ZYNQ];   // 0x14
} ZynqImageHeaderTableStructure;

typedef struct
{
    uint32_t nextImageHeaderWordOffset;         // 0x00
    uint32_t partitionHeaderWordOffset;         // 0x04
    uint32_t dataSectionCount;                  // 0x08
    uint32_t imageNameLength;                   // 0x0C
    char     imageName[1];                      // 0x10
} ZynqImageHeaderStructure;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class ZynqImageHeaderTable : public ImageHeaderTable
{
public:
    ZynqImageHeaderTable();
    ZynqImageHeaderTable(std::ifstream& ifs);
    ~ZynqImageHeaderTable();

    void ValidateSecurityCombinations(Authentication::Type, Encryption::Type, Checksum::Type) {};
    void RealignSectionDataPtr(void);
    uint8_t	GetMaxNumOfPartitions(void) { return MAX_NUM_PARTITIONS_ZYNQ; }

    void SetImageHeaderTableVersion(uint32_t version);
    void SetPartitionCount(uint32_t count);
    void SetFirstPartitionHeaderOffset(uint32_t offset);
    void SetFirstImageHeaderOffset(uint32_t offset);
    void SetHeaderAuthCertificateOffset(uint32_t offset);
    void SetReservedFields(void);
    void SetChecksum(void) {}
    void SetBootDevice(BootDevice::Type type) {}
    
    uint32_t GetImageHeaderTableVersion(void);
    uint32_t GetPartitionCount(void);
    uint32_t GetFirstPartitionHeaderOffset(void);
    uint32_t GetFirstImageHeaderOffset(void);
    uint32_t GetHeaderAuthCertificateOffset(void);

private:
    ZynqImageHeaderTableStructure *iHTable;
};

/******************************************************************************/
class ZynqImageHeader : public ImageHeader
{
public:
    ZynqImageHeader(std::string& filename);
    ZynqImageHeader(std::ifstream& f);
    ~ZynqImageHeader();

    void Build(BootImage& bi, Binary& cache);
    void Link(BootImage &bi, PartitionHeader* partitionHeader, ImageHeader* nextImageheader);
    void ValidateEncryptionAttributes(BootImage & bi);
    void ImportFpgaDataFile(BootImage & bi);
    void ImportBit(BootImage & bi);
    void ImportBin(BootImage & bi);
    void CreateElfPartitions(BootImage& bi, ElfFormat* elf, uint8_t proc_state);
    uint64_t GetExecAddrForXip(uint64_t execAddr);
    
    void SetNextImageHeaderOffset(uint32_t addr);
    void SetPartitionHeaderOffset(uint32_t addr);
    void SetDataSectionCount(uint32_t cnt);
    void SetImageNameLength(uint32_t len);
    void SetImageName(void);

    Binary::Address_t GetNextImageHeaderOffset();
    uint32_t GetPartitionHeaderOffset(void);
    uint32_t GetDataSectionCount(void);
    uint32_t GetImageNameLength(void);

private:
    ZynqImageHeaderStructure *imageHeader;
};
#endif
