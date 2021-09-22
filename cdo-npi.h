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

#ifndef D_cdo_npi
#define D_cdo_npi

#include "cdo-command.h"

CDOUTIL_EXPORT int cdocmd_post_process_mode(const char * s, uint32_t verbose);
CDOUTIL_EXPORT void cdocmd_post_process_start(void);
CDOUTIL_EXPORT uint32_t cdocmd_post_process_end_seq(CdoSequence ** seqp);
CDOUTIL_EXPORT void cdocmd_post_process_end(void ** new_data, size_t * new_size);
CDOUTIL_EXPORT uint32_t cdocmd_post_process_cdo_seq(const void * data, size_t size, CdoSequence ** seqp);
CDOUTIL_EXPORT uint32_t cdocmd_post_process_cdo(const void * data, size_t size, void ** new_data, size_t * new_size);

#endif /* D_cdo_npi */
