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

#ifndef _READIMAGE_ZYNQMP_H_
#define _READIMAGE_ZYNQMP_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <list>
#include <string>
#include <iostream>

#include "readimage.h"
#include "bootheader-zynqmp.h"
#include "imageheadertable-zynqmp.h"
#include "partitionheadertable-zynqmp.h"
#include "authentication-zynqmp.h"

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class ZynqMpReadImage : public ReadImage
{
public:
    ZynqMpReadImage(std::string filename) : ReadImage(filename)
    {
        bH = NULL;
        iHT = NULL;
        iHs.clear();
        pHTs.clear();
        authenticationVerified = true;
    }
    ~ZynqMpReadImage();

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

    void VerifyAuthentication(bool);
    void VerifyHeaderTableSignature();
    void VerifySPKSignature(AuthCertificate4096Structure * auth_cert);
    void VerifyPartitionSignature(void);
    bool VerifySignature(bool nist, uint8_t * data, size_t dataLength, ACKey4096 * acKey, uint8_t * signature);

protected:
    ZynqMpBootHeaderStructure* bH;
    ZynqMpImageHeaderTableStructure* iHT;
    ZynqMpImageHeaderStructure* iH;
    ZynqMpPartitionHeaderTableStructure* pHT;
    std::list<ZynqMpImageHeaderStructure*> iHs;
    std::list<ZynqMpPartitionHeaderTableStructure*> pHTs;
    bool authenticationVerified;
    int plAcCount;
};
#endif
