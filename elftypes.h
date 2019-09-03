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

#ifndef _ELFTYPES_H_
#define _ELFTYPES_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdint.h>


/* ELF 32-bit */
typedef uint32_t    Elf32_Addr;
typedef uint16_t    Elf32_Half;
typedef uint32_t    Elf32_Off;
typedef int32_t     Elf32_Sword;
typedef uint32_t    Elf32_Word;

/* ELF 64-bit */
typedef uint64_t    Elf64_Addr;
typedef uint64_t    Elf64_Off;
typedef int64_t     Elf64_Sxword;
typedef uint64_t    Elf64_Xword;
typedef int32_t     Elf32_Sword;
typedef uint32_t    Elf64_Word;
typedef uint16_t    Elf64_Half;
#endif
