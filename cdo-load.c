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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <inttypes.h>
#include "cdo-binary.h"
#include "cdo-source.h"
#include "cdo-raw.h"
#include "cdo-load.h"

void * file_to_buf(const char * path, size_t * sizep) {
    FILE * f = fopen(path, "rb");
    void * buf = NULL;
    size_t capacity = 0;
    size_t size = 0;
    *sizep = 0;
    if (f == NULL) {
        fprintf(stderr, "cannot open file: %s\n", path);
        return NULL;
    }

    for (;;) {
        size_t len;
        if (size == capacity) {
            capacity = capacity ? capacity * 2 : 1;
            buf = realloc(buf, capacity);
            if (buf == NULL) {
                fprintf(stderr, "end of memory while reading file: %s\n", path);
                goto error;
            }
        }
        len = fread((char *)buf + size, 1, capacity - size, f);
        if (len == 0) break;
        size += len;
    }

    if (ferror(f)) {
        fprintf(stderr, "cannot read file: %s\n", path);
        goto error;
    }

    fclose(f);
    *sizep = size;
    return buf;

error:
    if (buf) free(buf);
    fclose(f);
    return NULL;
}

CdoSequence * cdoseq_load_cdo(const char * path) {
    CdoSequence * seq = NULL;
    CdoRawInfo * raw = NULL;
    size_t size;
    void * data = file_to_buf(path, &size);
    if (data == NULL) goto done;
    raw = decode_raw(data, size);
    if (raw != NULL) {
        free(data);
        data = raw->data;
        size = raw->size;
    }
    if (is_cdo_data(data, size)) {
        seq = decode_cdo_binary(data, size);
        if (seq == NULL) {
            fprintf(stderr, "cannot decode binary cdo file: %s\n", path);
            goto done;
        }
    } else {
        FILE * infile = fopen(path, "r");
        if (infile == NULL) {
            fprintf(stderr, "cannot open file: %s\n", path);
            goto done;
        }
        seq = cdoseq_from_source(infile);
        fclose(infile);
        if (seq == NULL) {
            fprintf(stderr, "cannot parse source cdo file: %s\n", path);
            goto done;
        }
    }
done:
    if (raw != NULL) {
        free(raw);
    } else {
        free(data);
    }
    return seq;
}

void cdoseq_extract_writes(CdoSequence * seq) {
    LINK * l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case (CdoCmdWrite):
        case (CdoCmdMaskWrite):
            /* Ignore writes that configure frequency */
            if (cmd->dstaddr == 0xf1260050 || cmd->dstaddr == 0xf1260054 ||
                cmd->dstaddr == 0xf1260104) {
                cdocmd_free(cmd);
            }
            break;
        default:
            cdocmd_free(cmd);
            break;
        }
    }
}

/******************************************************************************/
CdoSequence* cdoseq_extract_cdo_till_ssit_sync_slaves(CdoSequence * seq, uint32_t sync_index)
{
    CdoSequence* temp_seq = seq;

    LINK * l = temp_seq->cmds.next;
    uint32_t num_sync_slave_cmds = 0;
    uint8_t stop = 0;
    uint8_t start = 0;
    if (num_sync_slave_cmds == sync_index - 1)
    {
        start = 1;
    }
    while (l != &temp_seq->cmds)
    {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if ((cmd->type == CdoCmdSsitSyncSlaves) && (stop == 0))
        {
            num_sync_slave_cmds++;
            if ((num_sync_slave_cmds == sync_index - 1) && (start == 0))
            {
                start = 1;
                cdocmd_free(cmd);
                continue;
            }
            if (num_sync_slave_cmds == sync_index)
            {
                stop = 1;
                continue;
            }
        }
        if ((start == 0) || (stop == 1))
        {
            cdocmd_free(cmd);
        }
    }
    return temp_seq;
}
