/******************************************************************************
* Copyright 2019-2020 Xilinx, Inc.
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

#include "dyn_link.h"

typedef struct CdoRawInfo CdoRawInfo;

typedef enum {
    CdoRawUnknown,
    CdoRawCdo,
    CdoRawCfi
} CdoRawType;

struct CdoRawInfo {
    CdoRawType type;
    size_t size;
    uint32_t data[1];                   /* Must be last */
};

CDOUTIL_EXPORT CdoRawInfo * decode_raw(void * data, size_t size);
CDOUTIL_EXPORT void encode_raw(FILE * f, void * data, size_t size);

#endif /* D_cdo_raw */
