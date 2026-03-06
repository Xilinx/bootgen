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

#ifndef _BOOTHEADER_SPARTANUP_H_
#define _BOOTHEADER_SPARTANUP_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootheader.h"
#include "bootimage.h"
//#include "bifoptions.h"
#include "fileutils.h"
//#include "imageheadertable-spartanup.h"
#include <memory>


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define DEFAULT_RESERVED_VERSAL                 0
#define AUTH_HDR_LASSEN_NONSECURE               0x04
#define WIDTH_DETECTION_LASSEN                  0x665599AA
#define SMAP_BUS_WIDTH                          4
//#define ROM_BH_RESERVED                       17
#define ROM_BH_RESERVED                         15 /* Updated after adding VersalNet */
#define PLM_BH_RESERVED                         24 /* #define PLM_BH_RESERVED_VERSAL_VERSALNET_TELLURIDE_LASSEN   24*/
#define SHA3_PAD_SIZE_VERSAL                    19
#define MAX_REG_INIT_VERSAL                     512 /* #define MAX_REG_INIT_VERSAL_VERSALNET_TELLURIDE_LASSEN   512*/
#define MAX_REG_INIT_LASSEN                     128
#define PUF_IMAGE_ID_WORD                       0x50554649U	/* PUFI */

/* Keys Source */
#define BBRAM_GRY_KEY                           0x3A5C3C57
#define BBRAM_BLK_KEY                           0x3A5C3C59

/* Bit shifts for BH attribute/config parameters */
#define DPA_CM_BIT_SHIFT                        10
#define DPA_CM_BIT_MASK                         3
                                                
/* Bit shifts for BH attribute/config parameters : VersalNet */
#define BH_RSA_SINGED_BIT_SHIFT                 18
#define BH_RSA_SINGED_BIT_MASK                  3
                                                
#define BH_DICE_BIT_SHIFT                       20
#define BH_DICE_BIT_MASK                        3
                                                
/* Telluride */
#define UDF_BH_TELLURIDE                        129 /* 516 Bytes */
#define ROM_BH_RESERVED_TELLURIDE               12 // actual is 14. Added pufPDIIdentificationWord and pufHDLength to compensate Lassen.
#define ROM_BH_RESERVED_LASSEN                  15

#define BH_UDF_OFFSET_TELLURIDE                 0x7C
//#define PLM_BH_RESERVED_TELLURIDE_LASSEN      24
/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
typedef struct
{
    uint32_t smapWords[SMAP_BUS_WIDTH];          //  (0x00)
} SpartanupSmapWidthTable;



/* Lassen A BH Structure */
typedef struct
{
    uint32_t smapWords[SMAP_BUS_WIDTH];                      // (0x00)
    uint32_t widthDetectionWord;                             // (0x10)
    uint32_t identificationWord;                             // (0x14)
    uint32_t encryptionKeySource;                            // (0x18)
    uint32_t sourceOffset;                                   // (0x1C)
    uint32_t pmcCdoLoadAddress;                              // (0x20)
    uint32_t pmcCdoLength;                                   // (0x24)
    uint32_t totalPmcCdoLength;                              // (0x28)
    uint32_t plmLength;                                      // (0x2C)
    uint32_t totalPlmLength;                                 // (0x30)
    uint32_t bhAttributes;                                   // (0x34)
    uint32_t greyOrBlackKey[BLK_GRY_KEY_LENGTH];             // (0x38)
    uint32_t greyOrBlackIV[IV_LENGTH];                       // (0x58)
    uint32_t plmSecureHdrIv[IV_LENGTH];                      // (0x64)
    uint32_t plmRevokeId;                                    // (0x70)
                                                             
    uint32_t authHeader1;                                    // (0x74)
    uint32_t hashBlockLength1;                               // (0x78)
    uint32_t totalppkkSize1;                                 // (0x7C)
    uint32_t actualppkSize1;                                 // (0x80)
    uint32_t totalSignatureSize1;                            // (0x84)
    uint32_t actualSignatureSize1;                           // (0x88)

    uint32_t pufPDIIdentificationWord;                       // (0x8C)
    uint32_t shutterValue;                                   // (0x90)
    uint32_t pufRoSwapConfigVal;                             // (0x94)                                                         
    uint32_t pufHDLength;                                    // (0x98)
                                                             
    uint32_t romReserved[ROM_BH_RESERVED_LASSEN];            // (0x9C)
    uint32_t imageHeaderByteOffset; //pdiID                  // (0xD8)
    uint32_t plmReserved[PLM_BH_RESERVED];                   // (0xDC)
    uint32_t reginit[MAX_REG_INIT_LASSEN];                   // (0x13C)
    uint32_t headerChecksum;                                 // (0x33C)
                                                             // (0x340)

} SpartanupBootHeaderStructure;

#define BH_AC_HEADER_OFFSET_SUP                   0x74
#define BH_TOTAL_PPK_SIZE1_OFFSET_SUP             0x7C
#define BH_ACTUAL_PPK_SIZE1_OFFSET_SUP            0x80
#define BH_TOTAL_SIGN_SIZE1_OFFSET_SUP            0x84
#define BH_ACTUAL_SIGN_SIZE1_OFFSET_SUP           0x88


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class SpartanupBootHeader : public BootHeader
{
public:
    SpartanupBootHeader(Arch::Type arch);
    SpartanupBootHeader(std::ifstream& src, Arch::Type arch);
    ~SpartanupBootHeader();

    void Build(BootImage & bi, Binary & cache);
    void Link(BootImage& bi);
    void ResizeSection(BootImage &bi);
    void SetBHForPartialBootImage();
    void SetBHForXIP(BootImage & bi);
    void BuildRegInitTable(Options& options);
    void LinkPrebuiltBH(BootImage& bi);

    void AddAcSizeToTotalFSBLSize(uint32_t size) {};
    void SetPartitionHeaderByteOffset(uint32_t address) {};
    void SetWidthDetectionWord(void);
    void SetIdentificationWord(void);
    void SetSourceOffset(uint32_t offset);
    void SetHeaderChecksum(Options& options);
    void SetShutterValue(uint32_t value);
    void SetPufRingOscilltorSwapConfigValue(uint32_t value);
    void SetImageHeaderByteOffset(uint32_t address);
    void SetEncryptionKeySource(KeySource::Type keyType, BifOptions* bifOptions);
    void SetGreyOrBlackKey(std::string keyFile);
    void SetGreyOrBlackIv(std::string ivFile);
    void SetPufPDIIdentificationWord();
    void SetPlmSecureHdrIv(uint8_t* iv);
    void SetPmcDataSecureHdrIv(uint8_t* iv);
    void SetPufData(BootImage &bi);
    void SetPufHDLength(uint32_t value);
    void SetHashBlockSize(uint32_t value);
    void SetAuthHeader(BootImage &bi, uint32_t value);
    void SetPPKSize(uint32_t actualSize, uint32_t totalSize);
    void SetSignatureSize(uint32_t actualSize, uint32_t totalSize);
    void SetSmapBusWidthWords(uint32_t width);
    void SetPlmLength(uint32_t size);
    void SetTotalPlmLength(uint32_t size);
    void SetPmcCdoLength(uint32_t size);
    void SetPmcCdoLoadAddress(uint32_t address);
    void SetTotalPmcCdoLength(uint32_t size);
    void SetBHAttributes(BootImage& bi);
    void SetBHAttributes(uint32_t attributes);
    void SetRevokeId(uint32_t id);
    void SetRomReserved();
    void SetPmcFwReserved();
    //void SetSHA3Padding();

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
    uint32_t GetPufDataLength(void);
    uint32_t GetBHChecksumDataSize(void);

    /*To suppress the vesion check warning in parse boot image for zynqmp*/
    uint32_t GetHeaderVersion(void) { return 0xFFFFFFFF; }

    Arch::Type arch;
private:
    SpartanupBootHeaderStructure *bHTable;
    SpartanupSmapWidthTable *smapTable;
    bool kekIvMust;
    std::string kekIvFile;
    std::unique_ptr<uint8_t[]> pufData;
    std::unique_ptr<uint8_t[]> bhKeyData;
    std::unique_ptr<uint8_t[]> ivData;
};
#endif
