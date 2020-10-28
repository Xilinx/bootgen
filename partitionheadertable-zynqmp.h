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

#ifndef _PARTITIONHEADERTABLE_ZYNQMP_H_
#define _PARTITIONHEADERTABLE_ZYNQMP_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "partitionheadertable.h"
#include "imageheadertable-zynqmp.h"
#include "encryption.h"
#include "checksum.h"

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define PH_TRUSTZONE_SHIFT_ZYNQMP           0
#define PH_TRUSTZONE_MASK_ZYNQMP            0x1
#define PH_EXCEPTION_LEVEL_SHIFT_ZYNQMP     1
#define PH_EXCEPTION_LEVEL_MASK_ZYNQMP      0x3
#define PH_EXEC_STATE_SHIFT_ZYNQMP          3
#define PH_EXEC_STATE_MASK_ZYNQMP           0x1
#define PH_DEST_DEVICE_SHIFT_ZYNQMP         4
#define PH_DEST_DEVICE_MASK_ZYNQMP          0x7
#define PH_ENCRYPT_SHIFT_ZYNQMP             7
#define PH_ENCRYPT_MASK_ZYNQMP              0x1
#define PH_DEST_CPU_SHIFT_ZYNQMP            8
#define PH_DEST_CPU_MASK_ZYNQMP             0xF
#define PH_CHECKSUM_SHIFT_ZYNQMP            12
#define PH_CHECKSUM_MASK_ZYNQMP             0x7
#define PH_AC_FLAG_SHIFT_ZYNQMP             15
#define PH_AC_FLAG_MASK_ZYNQMP              0x1
#define PH_OWNER_SHIFT_ZYNQMP               16
#define PH_OWNER_MASK_ZYNQMP                0x3
#define PH_ENDIAN_SHIFT_ZYNQMP              18
#define PH_ENDIAN_MASK_ZYNQMP               0x1
#define PH_EARLY_HANDOFF_SHIFT_ZYNQMP       19
#define PH_EARLY_HANDOFF_MASK_ZYNQMP        0x1
#define PH_AUTH_BLOCK_SHIFT_ZYNQMP          20
#define PH_AUTH_BLOCK_MASK_ZYNQMP           0x7
#define PH_HIVEC_SHIFT_ZYNQMP               23
#define PH_HIVEC_MASK_ZYNQMP                0x1


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
    uint32_t nextPartitionHeaderOffset;         // 0x0C
    uint64_t destinationExecAddress;            // 0x10
    uint64_t destinationLoadAddress;            // 0x18
    uint32_t partitionWordOffset;               // 0x20
    uint32_t partitionAttributes;               // 0x24
    uint32_t dataSectionCount;                  // 0x28
    uint32_t checksumWordOffset;                // 0x2C
    uint32_t imageHeaderWordOffset;             // 0x30
    uint32_t authCertificateOffset;             // 0x34
    uint32_t partitionNumber;                   // 0x38
    uint32_t pHChecksum;                        // 0x3C
} ZynqMpPartitionHeaderTableStructure;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class ZynqMpPartitionHeader : public PartitionHeader
{
public:
    ZynqMpPartitionHeader(ImageHeader* imageheader, int index0);
    ~ZynqMpPartitionHeader();

    void ReadHeader(std::ifstream& ifs);
    void ReadData(std::ifstream& ifs);
    void RealignSectionDataPtr(void);
    
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
    void SetPartitionNumber(uint32_t partNum);
    void SetChecksum(void);
    void SetNextPartitionHeaderOffset(void);
    void SetBitLoadAddress(bool load, uint32_t val);
    
    bool IsBootloader(void) { return isBootloader; }
    uint8_t GetDestinationDevice(void);
    uint8_t GetEncryptFlag(void);
    uint8_t GetAuthCertFlag(void);
    uint8_t GetChecksumType(void);
    uint8_t GetOwnerType(void);
    uint8_t GetDestinationCpu(void);
    uint8_t GetProcessorExecState(void);
    uint8_t GetElfEndianess(void);
    uint8_t GetExceptionLevel(void);
    uint8_t GetTrustZone(void);
    uint8_t GetEarlyHandoff(void);
    uint8_t GetHivec(void);
    uint32_t GetPartitionHeaderSize(void);
    uint32_t GetChecksum(void);
    uint32_t GetPartitionNumber(void);
    uint64_t GetExecAddress(void);
    uint64_t GetLoadAddress(void);
    uint32_t GetEncryptedPartitionLength(void);
    uint32_t GetUnencryptedPartitionLength(void);
    uint32_t GetTotalPartitionLength(void);
    uint32_t GetAuthCertificateOffset(void);
    uint8_t  GetAuthblock(void);
    uint32_t GetPartitionWordOffset(void);
    uint32_t GetPartitionPadSize64bBoundary(Section*);

    bool IsFirstPartitionInImage()
    {
        return index == 0;
    }

private:
    uint8_t partitionEncrypted;
    ZynqMpPartitionHeaderTableStructure* pHTable;
};
#endif
