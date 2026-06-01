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

#ifndef _READIMAGE_VERSAL_2VE_2VM_H_
#define _READIMAGE_VERSAL_2VE_2VM_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <list>
#include <string>
#include <utility>
#include <iostream>
#include "readimage.h"

#include "bootheader-versal_2ve_2vm.h"
#include "imageheadertable-versal_2ve_2vm.h"
#include "partitionheadertable-versal_2ve_2vm.h"
#include "authentication-versal_2ve_2vm.h"


/* Forward Class References */
class BootGenOptions;
class BootHeader;
class ImageHeaderTable;
class BootImage;
class Partition;
class ImageHeader;
class ByteFile;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class Versal_2ve_2vmReadImage :public ReadImage
{
public:
    Versal_2ve_2vmReadImage(std::string filename) : ReadImage(filename)
    {
        binFilename = filename;
        bH = nullptr;
        iHT = nullptr;
        iHs.clear();
        pHTs.clear();
        aCs.clear();
        partitionBuffers.clear();
        authenticationVerified = true;
        versalNetSeries = false;
    }
    ~Versal_2ve_2vmReadImage();

    void ReadBinaryFile(DumpOption::Type dump=DumpOption::NONE, std::string path="");
    void ReadHeaderTableDetails();
    void ReadPartitions();
    void DisplayImageDetails(ReadImageOption::Type type, DumpOption::Type dump, std::string path="");
    void DisplayHeaderTableDetails(ReadImageOption::Type type);
    void DisplayBootHeader(void);
    void DisplayImageHeaderTable(void);
    void DisplayImageHeaders(void);
    void DisplayPartitionHeaderTable(void);
    void DisplayAuthenicationCertificates(void);
    void DisplayACFields(uint8_t* rsa_ac, Authentication::Type auth_type);
    void DisplayBhAttributes(uint32_t value, Authentication::Type authType);
    void DisplayIhtAttributes(uint32_t value);
    void DisplayIhAttributes(uint32_t value);
    void DisplayPhtAttributes(uint32_t value);
    void DisplaySmapVectors(void);
    void VerifyAuthentication(bool);
    void VerifyHeaderTableSignature();
    void VerifySPKSignature(uint8_t* aC);
    void VerifyPartitionSignature(void);
    bool VerifySignature(bool nist, uint8_t * data, size_t dataLength, ACKey4096Sha3Padding * acKey, uint8_t* signature);
    bool VerifyECDSASignature(bool nist, uint8_t * data, size_t dataLength,  ACKeyECDSA *eckey, uint8_t* signature);
    bool VerifyECDSAP521Signature(bool nist, uint8_t * data, size_t dataLength, ACKeyECDSAP521 *eckey, uint8_t* signature);
    void Separator(void);
    void DumpPartitions(uint8_t* buffer, uint32_t length, std::string name, uint32_t id = 0, uint32_t index = 0);
    void DumpPlainPartition(uint8_t *buffer, uint32_t length, std::string partition_name, bool isBootloader = false, uint32_t id = 0, uint32_t index = 0);
	uint32_t ChunkSizeforParitition(std::string partition_name, bool isBootloader);
	uint32_t GetHashType(uint32_t authheader);
    void DisplayImageInfo();
    uint32_t GetPdiId(void);
    uint32_t GetParentId(void);
    uint8_t GetCreatorId(void);
    uint32_t IdentifyAuthtype(uint32_t); 
	uint32_t GetACLength(uint32_t, uint32_t);
    std::string GetPartitionType(uint32_t);
    std::string GetPartitionCore(uint32_t);

protected:
    std::string binFilename;
    std::unique_ptr<Versal_2ve_2vmBootHeaderStructure> bH;
    std::unique_ptr<Versal_2ve_2vmImageHeaderTableStructure> iHT;
    std::unique_ptr<Versal_2ve_2vmImageHeaderStructure> iH;
    std::unique_ptr<Versal_2ve_2vmPartitionHeaderTableStructure> pHT;
    std::list<Versal_2ve_2vmImageHeaderStructure*> iHs;
    std::list<Versal_2ve_2vmPartitionHeaderTableStructure*> pHTs;
    std::list<uint8_t*> aCs;
    std::list<std::pair<uint8_t*, uint32_t>> Hashblock_record;  
    std::list<uint32_t> authtype;  
    std::list<uint8_t*> partitionBuffers;
    bool authenticationVerified;
};

#endif
