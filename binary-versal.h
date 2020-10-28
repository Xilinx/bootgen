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

#ifndef _VERSAL_BINARY_H_
#define _VERSAL_BINARY_H_

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
#include "binary.h"

class Section;
class Options;
class Binary;

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define WORD_SIZE_IN_BYTES      4
#define VERSAL_AC_SIZE          0xE60

typedef std::list<Section*> SectionList;

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class VersalBinary : public Binary
{
public:
    VersalBinary() {};
    ~VersalBinary();

    void StackAndAlign(Options& options);
};
#endif
