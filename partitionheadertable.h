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

#ifndef _PARTITIONHEADERTABLE_H_
#define _PARTITIONHEADERTABLE_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootheader.h"


/*
--------------------------------------------------------------------------------
*****************************************  C L A S S   R E F E R E N C E S   ***
--------------------------------------------------------------------------------
*/
class BaseThing;
class Options;
class BootImage;
class Section;
class PartitionHeader;
class ImageHeader;
class Partition;
class ChecksumContext;
class MD5ChecksumContext;

/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
typedef struct 
{
    uint32_t*   data;
    uint32_t    size;
} PreservedHeader;


/*-----------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class PartitionHeader : public BaseThing 
{
public:
    PartitionHeader(ImageHeader* imageheader, int index);
    virtual ~PartitionHeader();

    virtual void Build(BootImage& bi, Binary& cache);
    virtual void Link(BootImage& bi, PartitionHeader* next_part_hdr);
    virtual void ReadHeader(std::ifstream& ifs) = 0;
    virtual void ReadData(std::ifstream& ifs) = 0;
    virtual void RealignSectionDataPtr(void) {}

    virtual void SetEncryptedPartitionLength(uint32_t len) {}
    virtual void SetUnencryptedPartitionLength(uint32_t len) {}
    virtual void SetTotalPartitionLength(uint32_t len) {}
    virtual void SetLoadAddress(uint64_t addr) {}
    virtual void SetExecAddress(uint64_t addr) {}
    virtual void SetPartitionWordOffset(uint32_t addr) {}
    virtual void SetPartitionAttributes(void) {}
    virtual void SetSectionCount(uint32_t cnt) {}
    virtual void SetChecksumOffset(void) {}
    virtual void SetImageHeaderOffset(uint32_t offset) {}
    virtual void SetAuthCertificateOffset(void) {}
    virtual void SetPartitionNumber(uint32_t partNum) {}
    virtual void SetChecksum(void) {}
    virtual void SetReserved(void) {}
    virtual void SetNextPartitionHeaderOffset(void) {}
    virtual void SetBitLoadAddress(bool load, uint32_t val) {}
    virtual void SetPartitionSize(Binary::Length_t) {}

    virtual bool IsFirstPartitionInImage(void) { return false; }
    virtual bool IsBootloader(void) { return false; }
    virtual uint32_t GetPartitionHeaderSize(void) { return 0; }
    virtual uint32_t GetPartitionNumber(void) { return 0; }
    virtual uint32_t GetChecksum(void) { return 0; };
    virtual uint32_t GetAuthCertificateOffset(void) { return 0; };
    virtual uint8_t GetAuthblock(void) { return 0; };
    virtual uint32_t GetPartitionWordOffset(void) { return 0; };
    virtual uint8_t GetDestinationDevice(void) { return 0; };
    virtual uint8_t GetHeadAlignment(void) { return 0; };
    virtual uint8_t GetTailAlignment(void) { return 0; };
    virtual uint8_t GetEncryptFlag(void) { return 0; };
    virtual uint8_t GetAuthCertFlag(void) { return 0; };
    virtual uint8_t GetChecksumType(void) { return 0; };
    virtual uint8_t GetOwnerType(void) { return 0; };
    virtual uint8_t GetDestinationCpu(void) { return 0; };
    virtual uint8_t GetProcessorExecState(void) { return 0; };
    virtual uint8_t GetElfEndianess(void) { return 0; };
    virtual uint8_t GetExceptionLevel(void) { return 0; };
    virtual uint8_t GetTrustZone(void) { return 0; };
    virtual uint8_t GetEarlyHandoff(void) { return 0; };
    virtual uint8_t GetHivec(void) { return 0; };
    virtual uint32_t GetPartitionPadSize64bBoundary(Section*) { return 0; };
    virtual uint32_t GetEncryptedPartitionLength(void) { return 0; };
    virtual PartitionType::Type GetPartitionType(void) { return PartitionType::RESERVED; }
    virtual uint32_t GetSectionCount(void) { return 0; }
    
    Binary::Address_t GetLoadAddress(void) { return loadAddress; }
    Binary::Address_t GetExecAddress(void) { return execAddress; }
    Binary::Length_t GetPartitionSize(void) { return partitionSize; }
    Binary::Length_t GetTransferSize(void) { return transferSize; }
    uint32_t GetCertificateRelativeByteOffset(void) { return certificateRelativeByteOffset;  }
    uint8_t IsAuthCertPresent(void) { return authCertPresent; }
    uint32_t GetpartitionNum(void) { return partitionNum; }
    std::string GetPartitionAesKeyFile(void) { return partitionAesKeyFile; }
    bool GenerateAesKeyFlag(void) {return generateAesKeyFile; }
    
    ImageHeader* imageHeader;
    Partition* partition;
    std::list<AuthenticationCertificate*> ac;
    Section* checksumSection;
    PreservedHeader preservedBitstreamHdr;
    int index;
    bool prealigned;
    bool preencrypted;
    bool presigned;
    bool firstValidIndex;
    uint8_t execState;
    uint8_t elfEndianess;
    uint32_t certificateRelativeByteOffset;
    uint8_t* partitionSecHdrIv;

    Binary::Address_t loadAddress;
    Binary::Address_t execAddress;
    Binary::Length_t partitionSize;
    Binary::Length_t transferSize;
    uint32_t partitionNum;
    std::string partitionAesKeyFile;
    bool generateAesKeyFile;

    // Attributes 
    uint8_t headAlignment;
    uint8_t tailAlignment;
    uint8_t destDevice;
    uint8_t destCpu;
    uint8_t authCertPresent;
    uint8_t encryptFlag;
    uint8_t checksumType;
    uint8_t partitionType;
    uint8_t ownerType;
    uint8_t exceptionLevel;
    uint8_t trustzone;
    uint8_t early_handoff;
    uint8_t hivec;
    uint8_t authBlock;
    uint64_t firstChunkSize;
    
protected:
    bool slaveBootSplitMode;
    uint32_t fullBhSize;
    uint32_t allHdrSize;
    uint32_t bootloaderSize;
    bool isBootloader;
    bool isPmcdata;
};

/******************************************************************************/
class PartitionHeaderTable
{
public:
    PartitionHeaderTable() {}
    virtual ~PartitionHeaderTable() {}

    virtual void Build(BootImage& bi, Binary& cache);
    virtual void BuildPartitions(BootImage& bi, Binary& cache);
    virtual void Link(BootImage& bi);
    virtual void LinkPartitions(BootImage& bi);
    Section* firstSection;
};

/******************************************************************************/
class Partition : public BaseThing
{
public:
    Partition(PartitionHeader* hdr, Section* section0);
    Partition(PartitionHeader* hdr, const uint8_t* data, Binary::Length_t length);
    virtual ~Partition() {}

    virtual void Build(BootImage& bi, Binary& cache);
    virtual void Link(BootImage& bi);
    void ResizeIfNecessary(Section* section);

private:
    PartitionHeader* header;
};

#endif

// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
