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

#ifndef _READIMAGE_VERSAL_H_
#define _READIMAGE_VERSAL_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <list>
#include <string>
#include <iostream>
#include "readimage.h"

#include "bootheader-versal.h"
#include "imageheadertable-versal.h"
#include "partitionheadertable-versal.h"
#include "authentication-versal.h"


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
class VersalReadImage :public ReadImage
{
public:
    VersalReadImage(std::string filename) : ReadImage(filename)
    {
        binFilename = filename;
        bH = NULL;
        iHT = NULL;
        dumpType = DumpOption::NONE;
        iHs.clear();
        pHTs.clear();
        partitionBuffers.clear();
        aCs.clear();
        authenticationVerified = true;
    }
    ~VersalReadImage();

    void ReadBinaryFile(DumpOption::Type dump=DumpOption::NONE, std::string path="");
    void DisplayImageDetails(ReadImageOption::Type type, DumpOption::Type dump, std::string path="");
    void DisplayBootHeader(void);
    void DisplayImageHeaderTable(void);
    void DisplayImageHeaders(void);
    void DisplayPartitionHeaderTable(void);
    void DisplayAuthenicationCertificates(void);
    void DisplayACFields(uint8_t* rsa_ac, Authentication::Type auth_type);
    void DisplayBhAttributes(uint32_t value);
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
    void DumpBootHeader(void);
    void DumpPartitions(uint8_t* buffer, uint32_t length, std::string name, uint32_t id = 0, uint32_t index = 0);
    uint32_t GetPdiId(void);
    uint32_t GetParentId(void);
    uint8_t GetCreatorId(void);

protected:
    std::string binFilename;
    DumpOption::Type dumpType;
    std::string dumpPath;
    VersalBootHeaderStructure* bH;
    VersalImageHeaderTableStructure* iHT;
    VersalImageHeaderStructure *iH;
    VersalPartitionHeaderTableStructure *pHT;
    std::list<VersalImageHeaderStructure*> iHs;
    std::list<VersalPartitionHeaderTableStructure*> pHTs;
    std::list<uint8_t*> aCs;
    std::list<uint8_t*> partitionBuffers;
    bool authenticationVerified;
};

#endif
