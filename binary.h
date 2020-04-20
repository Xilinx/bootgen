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

#ifndef _BINARY_H_
#define _BINARY_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <list>
#include <string>
#include <stdint.h>
#include <ostream>
#include <iostream>

class Section;
class Options;

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define WORD_SIZE_IN_BYTES      4

typedef std::list<Section*> SectionList;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class Binary 
{
public:
    Binary() : TotalSize(0) { }
    virtual ~Binary();

    typedef uint64_t Address_t;
    typedef uint64_t Length_t;

    virtual void StackAndAlign(Options& options);
    //void Dump(std::ostream& s);

    static bool CheckAddress(uint64_t addr) 
    {
        return addr < 0x100000000ULL;
    }
    SectionList Sections;
    Length_t TotalSize;
};

/******************************************************************************/
class Section 
{
public:
    Section(const std::string& name, Binary::Length_t length);
    ~Section();

    std::string Name;
    Binary::Address_t Address;
    Binary::Length_t Length;
    uint32_t Alignment;
    Binary::Length_t Reserve;
    uint8_t* Data;
    uint64_t firstChunkSize;
    bool continuation;
    bool isPartitionData;
    bool isCertificate;
    bool isFirstElfSection;
    bool isBitStream;
    bool isBootloader;
    bool isPmcdata;
    int index;

    Binary::Address_t WordAddress()
    { 
        return  Address / sizeof(uint32_t);
    }
    void PadToWordAlignment(uint32_t headPad, uint32_t tailPad);
    void IncreaseLengthAndPadTo(Binary::Length_t length, uint8_t padding);
    void Summarize(std::ostream& out);
    void IncreaseLengthAndPadSHA3(Binary::Length_t newLength);
};

#endif
