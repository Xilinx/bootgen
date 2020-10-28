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

#ifndef _READIMAGE_ZYNQ_H_
#define _READIMAGE_ZYNQ_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/

#include "readimage.h"
#include "bootheader-zynq.h"
#include "imageheadertable-zynq.h"
#include "partitionheadertable-zynq.h"
#include "authentication-zynq.h"

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class ZynqReadImage : public ReadImage
{
public:
    ZynqReadImage(std::string filename) : ReadImage(filename)
    {
        bH = NULL;
        iHT = NULL;
        iHs.clear();
        pHTs.clear();
    }
    ~ZynqReadImage();

    void ReadBinaryFile(DumpOption::Type dump = DumpOption::NONE, std::string path = "");
    void DisplayBootHeader(void);
    void DisplayImageHeaderTable(void);
    void DisplayImageHeaders(void);
    void DisplayPartitionHeaderTable(void);
    void DisplayAuthenicationCertificates(void);
    void DisplayACFields(uint8_t* rsa_ac, Authentication::Type auth_type);
    void DisplayBhAttributes(uint32_t value);
    void DisplayPhtAttributes(uint32_t value);
    void DisplayBootVectors(void);

protected:
    ZynqBootHeaderStructure* bH;
    ZynqImageHeaderTableStructure* iHT;
    ZynqImageHeaderStructure* iH;
    ZynqPartitionHeaderTableStructure* pHT;
    std::list<ZynqImageHeaderStructure*> iHs;
    std::list<ZynqPartitionHeaderTableStructure*> pHTs;
};
#endif
