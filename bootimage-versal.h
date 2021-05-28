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

#ifndef _BOOTIMAGE_VERSAL_H_
#define _BOOTIMAGE_VERSAL_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootimage.h"
#include "bootheader-versal.h"
#include "encryption-versal.h"
#include "binary-versal.h"

#define ENCR_BLOCK_IN_BYTES    16  //= 128 bits

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class VersalBootImage : public BootImage
{
public:
    VersalBootImage(Options& options, uint8_t index);
    ~VersalBootImage();

    void BuildAndLink(Binary* cache);
    void Add(BifOptions* bifoptions);
    void ReplaceImages(void);
    void ReplaceImagesNoSubSys(void);
    void AppendImagesInSubsystems(void);
    void OutputOptionalEfuseHash();
    void OutputPartitionFiles(Options& options, Binary& cache) {};
    void GenerateAuthenticationKeys(void) {};

    void ConfigureEncryptionContext(ImageHeader * image, Encryption::Type encryptType);
    void ConfigureAuthenticationContext(ImageHeader * image, Authentication::Type authType, PartitionBifOptions * partitionbifoptions);
    void ConfigureChecksumContext(ImageHeader * image, Checksum::Type checksumType);
    void ConfigureProcessingStages(ImageHeader* image, PartitionBifOptions* partitionbifoptions);

    void CheckForIhtAttributes(std::string base_file);
    void ParseBootImage(PartitionBifOptions * it);
    void ValidateSecureAttributes(ImageHeader * image, BifOptions* bifoptions, PartitionBifOptions* partitionBifOptions);
    ImageHeader* ParsePartitionDataToImage(BifOptions* bifoptions, PartitionBifOptions* it);
    void OutputOptionalSecureDebugImage();
    void ConfigureEncryptionBlocks(ImageHeader * image, PartitionBifOptions* partitionBifOptions);
    void SetPmcdataFile(const std::string & filename);
    void SetPmcDataLoadAddress(Binary::Address_t addr);
};
#endif
