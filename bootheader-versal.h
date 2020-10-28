/******************************************************************************
* Copyright 2015-2020 Xilinx, Inc.
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

#ifndef _BOOTHEADER_VERSAL_H_
#define _BOOTHEADER_VERSAL_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootheader.h"
#include "bootimage.h"
#include "bifoptions.h"
#include "fileutils.h"
#include "imageheadertable-versal.h"


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
/* Keys Source */
#define BBRAM_GRY_KEY       0x3A5C3C57
#define BBRAM_BLK_KEY       0x3A5C3C59

#define DEFAULT_RESERVED_VERSAL 0
#define SMAP_BUS_WIDTH          4
#define ROM_BH_RESERVED         17
#define PLM_BH_RESERVED         24
#define SHA3_PAD_SIZE_VERSAL    19
#define MAX_REG_INIT_VERSAL     512

#define DPA_CM_BIT_SHIFT        10
#define DPA_CM_BIT_MASK         3


/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
typedef struct
{
    uint32_t smapWords[SMAP_BUS_WIDTH];          //  (0x00)
} VersalSmapWidthTable;

typedef struct
{
    uint32_t smapWords[SMAP_BUS_WIDTH];          //  (0x00)
    uint32_t widthDetectionWord;                 //  (0x10)
    uint32_t identificationWord;                 //  (0x14)
    uint32_t encryptionKeySource;                //  (0x18)
    uint32_t sourceOffset;                       //  (0x1C)
    uint32_t pmcCdoLoadAddress;                  //  (0x20)
    uint32_t pmcCdoLength;                       //  (0x24)
    uint32_t totalPmcCdoLength;                  //  (0x28)
    uint32_t plmLength;                          //  (0x2C)
    uint32_t totalPlmLength;                     //  (0x30)
    uint32_t bhAttributes;                       //  (0x34)
    uint32_t greyOrBlackKey[BLK_GRY_KEY_LENGTH]; //  (0x38)
    uint32_t greyOrBlackIV[IV_LENGTH];           //  (0x58)
    uint32_t plmSecureHdrIv[IV_LENGTH];          //  (0x64)
    uint32_t shutterValue;                       //  (0x70)
    uint32_t pmcCdoSecureHdrIv[IV_LENGTH];       //  (0x74)
    uint32_t romReserved[ROM_BH_RESERVED];       //  (0x80)
    uint32_t imageHeaderByteOffset;              //  (0xC4)
    uint32_t plmReserved[PLM_BH_RESERVED];       //  (0xC8)
    uint32_t reginit[MAX_REG_INIT_VERSAL];       //  (0x128)
    uint32_t puf[PUF_DATA_LENGTH / 4];           //  (0x928)
    uint32_t headerChecksum;                     //  (0xF30)
    uint32_t sha3Padding[SHA3_PAD_SIZE_VERSAL];  //  (0xF34)
} VersalBootHeaderStructure;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class VersalBootHeader : public BootHeader
{
public:
    VersalBootHeader();
    VersalBootHeader(std::ifstream& src);
    ~VersalBootHeader();

    void Build(BootImage & bi, Binary & cache);
    void Link(BootImage& bi);
    void SetBHForPartialBootImage();
    void SetBHForXIP(BootImage & bi);
    void BuildRegInitTable(Options& options);
    uint32_t CalcHeaderChecksum(void* addr, size_t len);
    void LinkPrebuiltBH(BootImage& bi);

    void AddAcSizeToTotalFSBLSize(uint32_t size) {};
    void SetPartitionHeaderByteOffset(uint32_t address) {};
    void SetWidthDetectionWord(void);
    void SetIdentificationWord(void);
    void SetSourceOffset(uint32_t offset);
    void SetHeaderChecksum(void);
    void SetShutterValue(uint32_t value);
    void SetImageHeaderByteOffset(uint32_t address);
    void SetEncryptionKeySource(KeySource::Type keyType, BifOptions* bifOptions);
    void SetGreyOrBlackKey(std::string keyFile);
    void SetGreyOrBlackIv(std::string ivFile);
    void SetPlmSecureHdrIv(uint8_t* iv);
    void SetPmcDataSecureHdrIv(uint8_t* iv);
    void SetPufData(BootImage &bi);
    void SetSmapBusWidthWords(uint32_t width);
    void SetPlmLength(uint32_t size);
    void SetTotalPlmLength(uint32_t size);
    void SetPmcCdoLength(uint32_t size);
    void SetPmcCdoLoadAddress(uint32_t address);
    void SetTotalPmcCdoLength(uint32_t size);
    void SetBHAttributes(BootImage& bi);
    void SetBHAttributes(uint32_t attributes);
    void SetRomReserved();
    void SetPmcFwReserved();
    void SetSHA3Padding();

    uint32_t GetBootHeaderSize(void);
    uint32_t GetWidthDetectionWord(void);
    uint32_t GetIdentificationWord(void);
    uint32_t GetSourceOffset(void);
    uint32_t GetHeaderChecksum(void);
    uint32_t GetPmcFwLength(void);
    uint32_t GetTotalPmcFwLength(void);
    uint32_t GetPmcCdoLoadAddress(void);
    uint32_t GetImageHeaderByteOffset(void);
    uint32_t GetPmcCdoLength(void);
    uint32_t GetTotalPmcCdoLength(void);
    uint32_t GetBHAttributes(void);
    uint32_t GetXipBaseAddr(void);
    uint32_t* GetChecksumStartPtr(void);
    KeySource::Type GetEncryptionKeySource(void);

    /*To suppress the vesion check warning in parse boot image for zynqmp*/
    uint32_t GetHeaderVersion(void) { return 0xFFFFFFFF; }

private:
    VersalBootHeaderStructure *bHTable;
    VersalSmapWidthTable *smapTable;
    bool kekIvMust;
    std::string kekIvFile;
    uint8_t* pufData;
    uint8_t* bhKeyData;
    uint8_t* ivData;
};
#endif
