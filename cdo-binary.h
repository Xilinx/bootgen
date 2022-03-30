/******************************************************************************
* Copyright 2019-2022 Xilinx, Inc.
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

#ifndef D_cdo_binary
#define D_cdo_binary

#include "cdo-command.h"

void cdobinary_set_verbose(uint32_t verbose);
void cdobinary_set_auto_align(uint32_t enable);
void cdobinary_set_add_offset(uint32_t enable);
CdoSequence * decode_cdo_binary(const void * data, size_t len);
void * cdoseq_to_binary(CdoSequence * seq, size_t * sizep, uint32_t be);
int is_cdo_data(void * data, size_t size);
uint32_t*  get_slr_sync_point_offsets(void);
void search_for_sync_points(void);
size_t get_num_of_sync_points(void);
#endif /* D_cdo_binary */
