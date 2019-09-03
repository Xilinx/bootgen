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

#ifndef _CHECKSUM_H_
#define _CHECKSUM_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootgenenum.h"
#include "binary.h"

/* Forward Class References */
class BootImage;
class Section;


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define MD5_LENGTH_BYTES        16
#define SHA2_LENGTH_BYTES       32
#define SHA3_LENGTH_BYTES       48


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class ChecksumTable
{
public:
    ChecksumTable() {}
    virtual ~ChecksumTable() {}

    virtual void Build(BootImage& bi, Binary& cache);
    virtual void Link(BootImage& bi);
};

/******************************************************************************/
class ChecksumContext
{
public:
    ChecksumContext() {}
    virtual ~ChecksumContext() {}

    virtual Checksum::Type Type() = 0;
    
    virtual uint8_t Size()
    {
        return 0;
    }

    virtual Section* Build(Binary& cache, const uint8_t* data, const Binary::Length_t length)
    {
        return NULL;
    }

    virtual Section* Build(std::string name)
    {
        return NULL;
    }

    virtual void Link(bool bootloader, const uint8_t* data, const Binary::Length_t length, Section* section)
    {

    }
};

/******************************************************************************/
class NoneChecksumContext : public ChecksumContext 
{
public:
    virtual Checksum::Type Type() 
    {
        return Checksum::None; 
    }
};

/******************************************************************************/
class MD5ChecksumContext : public ChecksumContext
{
public:
    MD5ChecksumContext() {}
    ~MD5ChecksumContext() {}

    Checksum::Type Type() 
    {
        return Checksum::MD5; 
    }
    
    uint8_t Size() 
    {
        return MD5_LENGTH_BYTES; 
    }

    Section* Build(Binary& cache, const uint8_t* data, const Binary::Length_t length);
    void Link(bool bootloader, const uint8_t* data, const Binary::Length_t length, Section* section);
};

/******************************************************************************/
class SHA2ChecksumContext : public ChecksumContext 
{
public:
    SHA2ChecksumContext() {}
    ~SHA2ChecksumContext() {}

    Checksum::Type Type() 
    {
        return Checksum::SHA2; 
    }

    uint8_t Size() 
    {
        return SHA2_LENGTH_BYTES; 
    }

    Section* Build(Binary& cache, const uint8_t* data, const Binary::Length_t length);
    void Link(bool bootloader, const uint8_t* data, const Binary::Length_t length, Section* section);
};

/******************************************************************************/
class SHA3ChecksumContext : public ChecksumContext 
{
public:
    SHA3ChecksumContext() {}
    ~SHA3ChecksumContext() {}

    Checksum::Type Type() 
    {
        return Checksum::SHA3; 
    }

    uint8_t Size() 
    {
        return SHA3_LENGTH_BYTES; 
    }

    virtual Section* Build(Binary& cache, const uint8_t* data, const Binary::Length_t length) ;
    virtual Section* Build(std::string name) { return NULL; }
    virtual void Link(bool bootloader, const uint8_t* data, const Binary::Length_t length, Section* section);
};
#endif
