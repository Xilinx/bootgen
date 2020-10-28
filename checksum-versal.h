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

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootgenenum.h"
#include "binary.h"
#include "bootimage.h"
#include "checksum.h"

class BootImage;
class Section;


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define MD5_LENGTH_BYTES	16
#define SHA2_LENGTH_BYTES	32
#define SHA3_LENGTH_BYTES	48


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class VersalSHA3ChecksumContext : public ChecksumContext
{
public:
    Checksum::Type Type()
    {
        return Checksum::SHA3;
    }

    uint8_t Size()
    {
        return SHA3_LENGTH_BYTES;
    }

    Section* Build(Binary& cache, const uint8_t* data, const Binary::Length_t length);
    Section* Build(std::string name);

    void Link(bool bootloader, const uint8_t* data, const Binary::Length_t length, Section* section);
};

/******************************************************************************/
class VersalChecksumTable : public ChecksumTable
{
public:
    void Build(BootImage& bi, Binary& cache);
    void Link(BootImage& bi);
};
