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

#ifndef _BITUTILS_VERSAL_H_
#define _BITUTILS_VERSAL_H_
/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bitutils.h"

/* Forward Class Reference */
class OutputStream_LE;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************
 VersalBitFile
******************************************************************************/
class VersalBitFile : public BitFile
{
public:
    VersalBitFile(std::istream& stream);
    ~VersalBitFile();

    OutputStream* GetOutputStreamType(void);
    bool GetBitStripFlag(void);
    bool GetBitPadFlag(bool);
    bool GetPreserveHeaderFlag(void);
    void DummyRead(void) { };
    void ComparePartsDataBase(const std::string&);
    void Copy(OutputStream* os);
    void CopyNpi(OutputStream* os);

    protected:
        Encryption::Type encryptType;
        OutputStream_BE* temp;
};

#endif
