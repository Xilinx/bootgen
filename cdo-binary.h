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

typedef struct
{
    uint8_t cmd_id;
    uint8_t module_id;
    uint16_t reserved1;
    uint8_t min_arg_cnt;
    uint8_t max_arg_cnt;
    uint16_t reserved2;
} XPlmi_CmdInfo;

void cdobinary_set_verbose(uint32_t verbose);
void cdobinary_set_auto_align(uint32_t enable);
void cdobinary_set_add_offset(uint32_t enable);
CdoSequence * decode_cdo_binary(const void * data, size_t len);
void * cdoseq_to_binary(CdoSequence * seq, size_t * sizep, uint32_t be);
uint32_t check_cdo_commands(void* data, uint32_t l, uint32_t * xplm_data, uint32_t xplm_length);
int is_cdo_data(void * data, size_t size);
uint32_t*  get_slr_sync_point_offsets(void);
uint32_t*  get_slr_start_marker_offsets(void);
uint32_t*  get_slr_end_marker_offsets(void);
void search_for_sync_points(void);
size_t get_num_of_sync_points(void);
size_t get_num_start_markers(void);
size_t get_num_end_markers(void);
#endif /* D_cdo_binary */
