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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include "cdo-binary.h"
#include "cdo-source.h"
#include "cdo-npi.h"

uint32_t cdocmd_post_process_cdo_seq(const void * data, size_t size, CdoSequence ** seqp) {
    CdoSequence * seq = decode_cdo_binary(data, size);
    *seqp = NULL;
    if (seq == NULL) return 1;
    *seqp = seq;
    return 0;
}

uint32_t cdocmd_post_process_cdo(const void * data, size_t size, void ** new_data, size_t * new_size) {
    CdoSequence * seq;
    *new_data = NULL;
    *new_size = 0;
    cdobinary_set_auto_align(1);
    if (cdocmd_post_process_cdo_seq(data, size, &seq)) return 1;
    if (seq == NULL) return 0;
    *new_data = cdoseq_to_binary(seq, new_size, 0);
    cdocmd_delete_sequence(seq);
    return 0;
}
