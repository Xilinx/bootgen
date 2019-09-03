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

#ifndef _PARTITIONHEADERTABLE_ZYNQ_H_
#define _PARTITIONHEADERTABLE_ZYNQ_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "partitionheadertable.h"
#include "imageheadertable-zynq.h"
#include "checksum.h"

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define PHT_RESERVED            4

#define PH_TAIL_ALIGN_SHIFT     0
#define PH_TAIL_ALIGN_MASK      0x3
#define PH_HEAD_ALIGN_SHIFT     2
#define PH_HEAD_ALIGN_MASK      0x3
#define PH_DEST_DEVICE_SHIFT    4
#define PH_DEST_DEVICE_MASK     0xF
#define PH_CHECKSUM_SHIFT       12
#define PH_CHECKSUM_MASK        0x7
#define PH_AC_FLAG_SHIFT        15
#define PH_AC_FLAG_MASK         0x1
#define PH_OWNER_SHIFT          16
#define PH_OWNER_MASK           0x3


/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
typedef struct
{
    uint32_t encryptedPartitionLength;          // 0x00
    uint32_t unencryptedPartitionLength;        // 0x04
    uint32_t totalPartitionLength;              // 0x08
    uint32_t destinationLoadAddress;            // 0x0C
    uint32_t destinationExecAddress;            // 0x10
    uint32_t partitionWordOffset;               // 0x14
    uint32_t partitionAttributes;               // 0x18
    uint32_t dataSectionCount;                  // 0x1C
    uint32_t checksumWordOffset;                // 0x20
    uint32_t imageHeaderWordOffset;             // 0x24
    uint32_t authCertificateOffset;             // 0x28
    uint32_t reserved[PHT_RESERVED];            // 0x2C
    uint32_t pHChecksum;                        // 0x3C
} ZynqPartitionHeaderTableStructure;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
class ZynqPartitionHeader : public PartitionHeader
{
public:
    ZynqPartitionHeader(ImageHeader* imageheader, int index0);
    ~ZynqPartitionHeader();

    void ReadHeader(std::ifstream& ifs);
    void ReadData(std::ifstream& ifs);

    void SetEncryptedPartitionLength(uint32_t len);
    void SetUnencryptedPartitionLength(uint32_t len);
    void SetTotalPartitionLength(uint32_t len);
    void SetLoadAddress(uint64_t addr);
    void SetExecAddress(uint64_t addr);
    void SetPartitionWordOffset(uint32_t addr);
    void SetPartitionAttributes(void);
    void SetSectionCount(uint32_t cnt);
    void SetChecksumOffset(void);
    void SetImageHeaderOffset(uint32_t offset);
    void SetAuthCertificateOffset(void);
    void SetChecksum(void);
    void SetReserved(void);
    void SetBitLoadAddress(bool load, uint32_t val);
    void RealignSectionDataPtr(void);
    bool IsBootloader(void) { return isBootloader; }

    uint8_t GetDestinationDevice(void);
    uint8_t GetHeadAlignment(void);
    uint8_t GetTailAlignment(void);
    uint8_t GetAuthCertFlag(void);
    uint8_t GetChecksumType(void);
    uint8_t GetOwnerType(void);
    uint32_t GetPartitionHeaderSize(void);
    uint32_t GetChecksum(void);
    uint32_t GetExecAddress(void);
    uint32_t GetLoadAddress(void);
    uint32_t GetEncryptedPartitionLength(void);
    uint32_t GetUnencryptedPartitionLength(void);
    uint32_t GetTotalPartitionLength(void);
    uint32_t GetAuthCertificateOffset(void);
    uint32_t GetPartitionWordOffset(void);

    bool IsFirstPartitionInImage()
    {
        return index == 0;
    }

private:
    ZynqPartitionHeaderTableStructure* pHTable;
};
#endif
