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

#ifndef _BOOTHEADER_ZYNQMP_H_
#define _BOOTHEADER_ZYNQMP_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootheader.h"
#include "bootimage.h"
#include "bifoptions.h"
#include "fileutils.h"
#include "imageheadertable-zynqmp.h"

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define UDF_BH_SIZE_ZYNQMP      10
#define PMU_MAX_SIZE            0x20000 //128KB
#define FSBL_MAX_SIZE           0x3E800 //250KB

/* Masks values for BH attributes */
#define OPT_KEY_BIT_MASK        3
#define AUTH_ONLY_BIT_MASK      3
#define PUF_HD_BIT_MASK         3
#define BI_HASH_BIT_MASK        3
#define BH_RSA_BIT_MASK         3
#define CORE_BIT_MASK           3
#define BH_PUF_MODE_BIT_MASK    3
#define AUTH_HASH_BIT_MASK      3

/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
typedef struct 
{   
    uint32_t bootVectors [MAX_BOOT_VECTORS];    // (0x00)
    uint32_t widthDetectionWord;                // (0x20)
    uint32_t identificationWord;                // (0x24)
    uint32_t encryptionKeySource;               // (0x28)
    uint32_t fsblExecAddress;                   // (0x2C)
    uint32_t sourceOffset;                      // (0x30)
    uint32_t pmuFwLength;                       // (0x34)
    uint32_t totalPmuFwLength;                  // (0x38)
    uint32_t fsblLength;                        // (0x3C)
    uint32_t totalFsblLength;                   // (0x40)
    uint32_t fsblAttributes;                    // (0x44)
    uint32_t headerChecksum;                    // (0x48)
    uint32_t greyOrBlackKey [BLK_GRY_KEY_LENGTH]; // (0x4C-0x68)
    uint32_t shutterValue;                      // (0x6c)
    uint32_t udf [UDF_BH_SIZE_ZYNQMP];          // (0x70-0x94)
    uint32_t imageHeaderByteOffset;             // (0x98)
    uint32_t partitionHeaderByteOffset;         // (0x9C)
    uint32_t secureHdrIv [IV_LENGTH];           // (0xA0-0xA8)
    uint32_t greyOrBlackIV [IV_LENGTH];         // (0xAC-0xB4)
} ZynqMpBootHeaderStructure;

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class ZynqMpBootHeader : public BootHeader
{
public:
    ZynqMpBootHeader();
    ZynqMpBootHeader(std::ifstream& src);
    ~ZynqMpBootHeader();

    void Build(BootImage& bi, Binary& cache);
    void Link(BootImage& bi);
    void ResizeSection(BootImage &bi);
    void AddAcSizeToTotalFsblSize(BootImage& bi);
    void SetBHForSinglePartitionImage(BootImage& bi);
    void SetBHForXIP(BootImage & bi);
    void BuildRegInitTable(Options& options);
    uint32_t CalcHeaderChecksum(void* addr, size_t len);

    void SetBootVectors(BootImage &bi);
    void SetWidthDetectionWord(void);
    void SetIdentificationWord(void);
    void SetSourceOffset(uint32_t offset);
    void SetHeaderChecksum(void);
    void SetFsblLength(uint32_t size);
    void SetTotalFsblLength(uint32_t size);
    void SetFsblExecAddress(uint32_t address);
    void SetShutterValue(uint32_t value);
    void SetUdf(std::string udfFile);
    void SetImageHeaderByteOffset(uint32_t address);
    void SetPartitionHeaderByteOffset(uint32_t address);
    void SetEncryptionKeySource(KeySource::Type keyType);
    void SetPmuFwLength(uint32_t size);
    void SetTotalPmuFwLength(uint32_t size);
    void SetFsblAttributes(BootImage& bi);
    void SetFsblAttributes(uint32_t fsblattributes);
    void SetGreyOrBlackKey(std::string keyFile);
    void SetGreyOrBlackKekIV(std::string ivFile);
    void SetSecureHdrIv(uint8_t* iv);
    void SetPufData(BootImage &bi);

    uint32_t GetBootHeaderSize(void);	
    uint32_t GetWidthDetectionWord(void);
    uint32_t GetIdentificationWord(void);
    uint32_t GetSourceOffset(void);
    uint32_t GetHeaderChecksum(void);
    uint32_t GetFsblLength(void);
    uint32_t GetTotalFsblLength(void);
    uint32_t GetFsblExecAddress(void);
    uint32_t GetImageHeaderByteOffset(void);
    uint32_t GetPartitionHeaderByteOffset(void);
    uint32_t GetPmuFwLength(void);
    uint32_t GetTotalPmuFwLength(void);
    uint32_t GetFsblAttributes(void);
    uint32_t GetXipBaseAddr(void);
    uint32_t* GetChecksumStartPtr(void);
    KeySource::Type GetEncryptionKeySource(void);
    
    /* MP - Revisit - To suppress the vesion check warning in parse boot image for zynqmp */
    uint32_t GetHeaderVersion(void) { return 0xFFFFFFFF; }
    
private:
    ZynqMpBootHeaderStructure *bHTable;
    bool keyIvMust;
    uint8_t* pufData;
    uint8_t* bhKeyData;
    uint8_t* ivData;
};

#endif
