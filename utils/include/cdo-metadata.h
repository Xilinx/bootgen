/******************************************************************************
* Copyright 2022 Xilinx, Inc.
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

#ifndef D_cdo_metadata
#define D_cdo_metadata

#include "cdo-command.h"

typedef struct CdoMetadata CdoMetadata;

struct CdoMetadata {
    char creator[256];
    char design[128];
    char arch[128];
    char part[64];
    char slr[32];
    char date[32];
};

void cdometa_extract(CdoMetadata * meta, CdoSequence * seq);
void cdometa_add_markers(CdoMetadata * meta, CdoSequence * seq);

#endif /* D_cdo_metadata */
