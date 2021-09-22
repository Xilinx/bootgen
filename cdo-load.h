/******************************************************************************
* Copyright 2019-2021 Xilinx, Inc.
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

#ifndef D_cdo_load
#define D_cdo_load

#include <stdlib.h>
#include "cdo-command.h"

CDOUTIL_EXPORT void * file_to_buf(const char * path, size_t * sizep);
CDOUTIL_EXPORT CdoSequence * cdoseq_load_cdo(const char * path);

CDOUTIL_EXPORT void cdoseq_extract_writes(CdoSequence * seq);
CDOUTIL_EXPORT CdoSequence* cdoseq_extract_cdo_till_ssit_sync_slaves(CdoSequence * seq, uint32_t sync_index);
#endif /* D_cdo_load */
