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

#ifndef D_cdo_raw
#define D_cdo_raw

#include "cdo-command.h"
#include "cdo-metadata.h"

typedef struct CdoRawInfo CdoRawInfo;

typedef enum {
    CdoRawUnknown,
    CdoRawCdo,
    CdoRawCfi
} CdoRawType;

struct CdoRawInfo {
    CdoRawType type;
    CdoMetadata meta;
    uint32_t data_free_disable:1;
    size_t size;
    uint32_t *data;
};

CdoRawInfo * cdoraw_create(CdoRawType type);
void cdoraw_delete(CdoRawInfo * raw);
CdoRawInfo * cdoraw_decode(void * data, size_t size);
void cdoraw_encode(CdoRawInfo * raw, FILE * f);
void cdoraw_encode_seq(FILE * f, CdoSequence * seq, uint32_t be);
CdoRawInfo * decode_raw(void * data, size_t size);
void encode_raw(FILE * f, void * data, size_t size);

#endif /* D_cdo_raw */
