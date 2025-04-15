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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "cdo-alloc.h"
#include "cdo-metadata.h"

static void copy_marker_string(char * dst, size_t dstmax, CdoCommand * cmd) {
    size_t len = strlen(cmd->buf);
    if (len >= dstmax) len = dstmax - 1;
    memcpy(dst, cmd->buf, len);
    dst[len] = '\0';
}

void cdometa_extract(CdoMetadata * meta, CdoSequence * seq) {
    LINK * l = seq->cmds.next;

    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if (cmd->type == CdoCmdMarker) {
            switch (cmd->value) {
            case 1:
                if (meta->creator[0] != '\0') break;
                copy_marker_string(meta->creator, sizeof meta->creator, cmd);
                break;
            case 2:
                if (meta->design[0] != '\0') break;
                copy_marker_string(meta->design, sizeof meta->design, cmd);
                break;
            case 3:
                if (meta->arch[0] != '\0') break;
                copy_marker_string(meta->arch, sizeof meta->arch, cmd);
                break;
            case 4:
                if (meta->part[0] != '\0') break;
                copy_marker_string(meta->part, sizeof meta->part, cmd);
                break;
            case 5:
                if (meta->slr[0] != '\0') break;
                copy_marker_string(meta->slr, sizeof meta->slr, cmd);
                break;
            case 6:
                if (meta->date[0] != '\0') break;
                copy_marker_string(meta->date, sizeof meta->date, cmd);
                break;
            default:
                break;
            }
        }
    }
}

void cdometa_add_markers(CdoMetadata * meta, CdoSequence * seq) {
    CdoSequence * newseq = cdocmd_create_sequence();
    CdoMetadata curmeta;

    memset(&curmeta, 0, sizeof curmeta);
    cdometa_extract(&curmeta, seq);
    if (meta->creator[0] != '\0' && curmeta.creator[0] == '\0') {
        cdocmd_add_marker(newseq, 1, meta->creator);
    }
    if (meta->design[0] != '\0' && curmeta.design[0] == '\0') {
        cdocmd_add_marker(newseq, 2, meta->design);
    }
    if (meta->arch[0] != '\0' && curmeta.arch[0] == '\0') {
        cdocmd_add_marker(newseq, 3, meta->arch);
    }
    if (meta->part[0] != '\0' && curmeta.part[0] == '\0') {
        cdocmd_add_marker(newseq, 4, meta->part);
    }
    if (meta->slr[0] != '\0' && curmeta.slr[0] == '\0') {
        cdocmd_add_marker(newseq, 5, meta->slr);
    }
    if (meta->date[0] != '\0' && curmeta.date[0] == '\0') {
        cdocmd_add_marker(newseq, 6, meta->date);
    }

    cdocmd_concat_seq(newseq, seq);
    cdocmd_concat_seq(seq, newseq);
    cdocmd_delete_sequence(newseq);
}
