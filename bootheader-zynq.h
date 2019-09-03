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

#ifndef _BOOTHEADER_ZYNQ_H_
#define _BOOTHEADER_ZYNQ_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootheader.h"
#include "imageheadertable-zynq.h"
/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define UDF_BH_SIZE_ZYNQ    19

/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
typedef struct 
{
    uint32_t bootVectors [MAX_BOOT_VECTORS];    //  (0x00) 
    uint32_t widthDetectionWord;                //  (0x20) 
    uint32_t identificationWord;                //  (0x24) 
    uint32_t encryptionKeySource;               //  (0x28) 
    uint32_t headerVersion;                     //  (0x2C) 
    uint32_t sourceOffset;                      //  (0x30) 
    uint32_t fsblLength;                        //  (0x34) 
    uint32_t fsblLoadAddress;                   //  (0x38) 
    uint32_t fsblExecAddress;                   //  (0x3C) 
    uint32_t totalFsblLength;                   //  (0x40) 
    uint32_t qspiConfigWord;                    //  (0x44) 
    uint32_t headerChecksum;                    //  (0x48) 
    uint32_t udf [UDF_BH_SIZE_ZYNQ];            //  (0x4C) 
    uint32_t imageHeaderByteOffset;             //  (0x98) 
    uint32_t partitionHeaderByteOffset;         //  (0x9C) 
} ZynqBootHeaderStructure;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
class ZynqBootHeader : public BootHeader
{
public:
    ZynqBootHeader();
    ZynqBootHeader(std::ifstream& src);
    
    ~ZynqBootHeader();

    void Build(BootImage& bi, Binary& cache);
    void Link(BootImage& bi);
    void SetBHForSinglePartitionImage();
    void SetBHForXIP(BootImage & bi);
    void BuildRegInitTable (Options& options);
    uint32_t CalcHeaderChecksum(void* addr, size_t len);

    void SetBootVectors(BootImage &bi);
    void SetWidthDetectionWord(void);
    void SetIdentificationWord(void);
    void SetSourceOffset(uint32_t offset);
    void SetHeaderChecksum(void);
    void SetFsblLength(uint32_t size);
    void SetTotalFsblLength(uint32_t size);
    void SetFsblExecAddress(uint32_t address);
    void SetUdf(std::string udfFile);
    void SetImageHeaderByteOffset(uint32_t address);
    void SetPartitionHeaderByteOffset(uint32_t address);
    void SetEncryptionKeySource(KeySource::Type keyType);
    void SetHeaderVersion(void);
    void SetFsblLoadAddress(uint32_t address); 
    void SetQspiConfigWord(void);
   
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
    uint32_t GetHeaderVersion(void);
    uint32_t GetFsblLoadAddress(void);
    uint32_t GetQspiConfigWord(void);
    uint32_t GetXipBaseAddr(void);
    uint32_t* GetChecksumStartPtr(void);
    KeySource::Type GetEncryptionKeySource(void);

private:
    ZynqBootHeaderStructure *bHTable;
};

#endif

