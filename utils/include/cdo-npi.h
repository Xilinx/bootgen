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

#ifndef D_cdo_npi
#define D_cdo_npi

#include "cdo-command.h"

enum {
    houseclean_bypass_lpd = 1,
    houseclean_bypass_fpd = 2,
    houseclean_bypass_npd = 4,
    houseclean_bypass_plpd = 8,
    houseclean_bypass_cpm = 16,
};

int is_platform_spp(void);
int cdocmd_post_process_mode(const char * s, uint32_t verbose);
void set_cfu_stream_keyhole_size(uint64_t size);
void set_platform_spp(void);
void set_spp_ddr_enable(uint32_t enable);
void cdocmd_post_process_start(void);
uint32_t cdocmd_post_process_end_seq(CdoSequence ** seqp);
void cdocmd_post_process_end(void ** new_data, size_t * new_size);
uint32_t cdocmd_post_process_cdo_seq(const void * data, size_t size, CdoSequence ** seqp);
uint32_t cdocmd_post_process_cfi_seq(const void * data, size_t size, uint32_t is_gsc, CdoSequence ** seqp);
uint32_t cdocmd_post_process_cdo(const void * data, size_t size, void ** new_data, size_t * new_size);
uint32_t cdocmd_post_process_cfi(const void * data, size_t size, uint32_t is_gsc, void ** new_data, size_t * new_size);
void cdocmd_rewrite_address_filter(CdoSequence * seq, const char * address_filter_file);

#endif /* D_cdo_npi */
