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

#ifndef _BOOTHEADER_H_
#define _BOOTHEADER_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "logger.h"
#include "imageheadertable.h"

/* Forward class references */
class BaseThing;
class Options;
class BootImage;
class Section;
class ImageHeader;

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define ARM_JUMP_TO_SELF_INSTRUCION             0xEAFFFFFE 
#define ARM_JUMP_TO_SELF_INSTRUCION_A53_64      0x14000000
#define WIDTH_DETECTION                         0xAA995566
#define HEADER_ID_WORD                          0x584C4E58
#define BOOTHEADER_VERSION                      0x01010000
#define QSPI_CONFIG_WORD                        0x1
#define MAX_BOOT_VECTORS                        8
#define MAX_REGISTER_INITS                      256
#define BLK_GRY_KEY_LENGTH                      8
#define IV_LENGTH                               3
#define PUF_DATA_LENGTH                         1544
#define INVALID_REGISTER_ADDRESS                0xFFFFFFFF

/* Keys Source */
#define EFUSE_RED_KEY                           0xA5C3C5A3
#define BBRAM_RED_KEY                           0x3A5C3C5A
#define EFUSE_BLK_KEY                           0xA5C3C5A5
#define BH_BLACK_KEY                            0xA35C7C53
#define EFUSE_GRY_KEY                           0xA5C3C5A7
#define BH_GRY_KEY                              0xA35C7CA5
#define BH_KUP_KEY                              0xA3A5C3C5

/* Bit shifts for FSBL config parameters */
#define OPT_KEY_BIT_SHIFT                       2
#define AUTH_ONLY_BIT_SHIFT                     4
#define PUF_HD_BIT_SHIFT                        6
#define BI_HASH_BIT_SHIFT                       8
#define CORE_BIT_SHIFT                          10
#define AUTH_HASH_BIT_SHIFT                     12
#define BH_RSA_BIT_SHIFT                        14
#define BH_PUF_MODE_BIT_SHIFT                   16
#define OPT_KEY_BIT_MASK                        3
#define AUTH_ONLY_BIT_MASK                      3
#define PUF_HD_BIT_MASK                         3
#define BI_HASH_BIT_MASK                        3
#define BH_RSA_BIT_MASK                         3
#define BH_PUF_MODE_BIT_MASK                    3

/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
typedef struct 
{
    uint32_t address;
    uint32_t value;
} RegisterInitPair;


typedef struct 
{
    RegisterInitPair registerInitialization [MAX_REGISTER_INITS];
} RegisterInitTable;

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class RegisterTable 
{
public:
    RegisterTable()
        : regtab(NULL)
        , count(0) 
    {
    }
    
    void Build (Options& option,  RegisterInitTable* regTable0);
    void Add (Options& options, uint32_t address, uint32_t value);
    
    std::string filename;

private:
    RegisterInitTable* regtab;
    uint32_t count;
};


/******************************************************************************/
class BootHeader : public BaseThing
{
public:
    BootHeader();
    virtual ~BootHeader() {}

    virtual void Build(BootImage& bi, Binary& cache) { };
    virtual void Link(BootImage& bi) { };
    virtual void BuildRegInitTable (Options& options) { };
    virtual void ResizeSection(BootImage &bi) {};
    void BootloaderAddressAndSizeCheck(PartitionHeader& hdr);
    void Copy (BootHeader* otherHeader);
    void LoadUdfData (const std::string& udfBHFilename, uint8_t* udfData, uint32_t size);
    bool GetPrebuiltFlag (void);

    virtual uint32_t GetBootHeaderSize(void) { return 0; }
    uint32_t GetRegInitTableSize (void) 
    { 
        return sizeof(RegisterInitTable); 
    }

    RegisterTable regTable;

protected:
    bool prebuilt;
    uint32_t bhSize;
    bool slaveBootSplitMode;
    uint32_t fullBhSize;
    uint32_t allHdrSize;
    
    inline RegisterInitTable* registerInitTable() 
    {
        if (section == NULL)
        {
            LOG_ERROR("Section was not allocated");
        }
        return (RegisterInitTable*)(section->Data + GetBootHeaderSize());
    }
};

#endif
