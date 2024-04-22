/******************************************************************************
* Copyright 2019-2022 Xilinx, Inc.
* Copyright 2022-2023 Advanced Micro Devices, Inc.
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

#ifndef D_cdo_source
#define D_cdo_source

#include <stdio.h>
#include "cdo-command.h"

CdoSequence * cdoseq_from_source(FILE * f);
void cdoseq_to_source(FILE * f, CdoSequence * seq);
char slr_id_from_source(char ch);
uint32_t idcode_from_source(uint32_t id);
#endif /* D_cdo_source */
