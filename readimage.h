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

#ifndef _READIMAGE_H_
#define _READIMAGE_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <list>
#include <string>
#include <iostream>

#include "authentication.h"
#include "bootgenenum.h"
#include "encryption.h"
#include "binary.h"
#include "baseclass.h"
#include "bootgenexception.h"
#include "logger.h"
#include "authkeys.h"

/* Forward Class References */
class BootGenOptions;
class BootHeader;
class ImageHeaderTable;
class BootImage;
class Partition;
class ImageHeader;
class ByteFile;

#define SEPARATOR "--------------------------------------------------------------------------------"
/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
class ReadImage
{
public:
    ReadImage(std::string filename)
    {
        binFilename = filename;
        iHNames.clear();
        pHTNames.clear();
        aCs.clear();
    }
    virtual ~ReadImage() { }

    virtual void ReadBinaryFile(DumpOption::Type dump = DumpOption::NONE, std::string path = "") = 0;
    virtual void DisplayImageDetails(ReadImageOption::Type type, DumpOption::Type dump, std::string path="");
    virtual void DisplayBootHeader(void) = 0;
    virtual void DisplayImageHeaderTable(void) = 0;
    virtual void DisplayImageHeaders(void) = 0;
    virtual void DisplayPartitionHeaderTable(void) = 0;
    virtual void DisplayAuthenicationCertificates(void) = 0;
    virtual void DisplayACFields(uint8_t* rsa_ac, Authentication::Type auth_type) = 0;
    void DisplayValue(std::string name1, uint32_t value1);
    void DisplayValue(std::string name1, uint32_t value1, std::string name2, uint32_t value2);
    void DisplayValue(std::string name1, uint32_t value1, std::string name2, std::string value2);
    void DisplayLongValues(std::string name1, uint8_t* value, uint32_t num_bytes);
    void DisplayAscii(std::string name1, std::string value1);
    void DisplayIV(std::string name, uint32_t* ptr);
    void DisplayKey(std::string name, uint32_t* ptr);
    void DisplayAttributes(std::string name1, std::string value1, std::string name2, std::string value2);
    virtual void DisplayBootVectors(void) {};

    virtual void VerifyAuthentication(bool) {};
    void Separator();
protected:
    std::string binFilename;
    std::list<std::string> iHNames;
    std::list<std::string> pHTNames;
    std::list<std::string> aCNames;
    std::list<uint8_t*> aCs;
    RSA* rsa;
};
#endif
