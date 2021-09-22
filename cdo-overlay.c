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
#include <string.h>
#include <inttypes.h>

#include "cdo-alloc.h"
#include "cdo-overlay.h"

static CdoOverlayEntry * find_last_entry(CdoOverlayInfo * info, const char * name, int open) {
    CdoOverlayEntry * match = NULL;
    size_t i;
    for (i = 0; i < info->count; i++) {
        CdoOverlayEntry * entry = info->list + i;
        if ((open == 0 || entry->end == NULL) &&
            (name == NULL || strcmp(entry->start->buf, name) == 0)) {
            match =  entry;
        }
    }
    return match;
}

CdoOverlayInfo * cdooverlay_open(CdoSequence * seq) {
    CdoOverlayInfo * info = myalloc_zero(sizeof *info);
    size_t capacity = 0;
    unsigned int level = 0;

    LINK * l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if (cmd->type == CdoCmdMarker) {
            if (cmd->value == 100) {
                /* Marker start */
                CdoOverlayEntry * entry = find_last_entry(info, cmd->buf, 1);
                if (entry) {
                    fprintf(stderr, "nested overlay marker with same name: \"%s\"\n", (char *)cmd->buf);
                    goto error;
                }
                if (info->count == capacity) {
                    capacity = capacity ? capacity * 2 : 1;
                    info->list = (CdoOverlayEntry *)myrealloc(info->list, capacity * sizeof *info->list);
                }
                entry = info->list + info->count++;
                entry->start = cmd;
                entry->end = NULL;
                entry->level = level++;
            } else if (cmd->value == 101) {
                /* Marker end */
                CdoOverlayEntry * entry = find_last_entry(info, NULL, 1);
                if (entry == NULL) {
                    fprintf(stderr, "missing start marker for \"%s\"\n", (char *)cmd->buf);
                    goto error;
                }
                if (strcmp(entry->start->buf, cmd->buf) != 0) {
                    fprintf(stderr, "unexpected end marker \"%s\", expected \"%s\"\n", (char *)cmd->buf, (char *)entry->start->buf);
                    goto error;
                }
                level--;
                entry->end = cmd;
            }
        }
    }
    if (level != 0) {
        CdoOverlayEntry * entry = info->list;
        int first = 1;
        fprintf(stderr, "missing end marker(s):");
        while (entry < info->list + info->count) {
            if (entry->end == NULL) {
                if (!first) {
                    fprintf(stderr, ",");
                }
                fprintf(stderr, " \"%s\"", (char *)entry->start->buf);
                first = 0;
                entry++;
            }
        }
        fprintf(stderr, "\n");
        goto error;
    }
    return info;

error:
    myfree(info->list);
    myfree(info);
    return NULL;
}

void cdooverlay_close(CdoOverlayInfo * info) {
    if (info->seq) cdocmd_delete_sequence(info->seq);
    myfree(info->list);
    myfree(info);
}

int cdooverlay_apply(CdoSequence * seq, CdoOverlayInfo * ovl) {
    unsigned int level = 0;
    for (;;) {
        CdoOverlayInfo * dst = cdooverlay_open(seq);
        CdoOverlayEntry * dstentry;
        int more = 0;
        if (dst == NULL) return 1;
        for (dstentry = dst->list; dstentry < dst->list + dst->count; dstentry++) {
            CdoOverlayEntry * ovlentry = NULL;
            CdoCommand * ovlcmd = NULL;
            if (dstentry->level != level) continue;
            more = 1;
            fprintf(stdout, "check overlay \"%s\"\n", (char *)dstentry->start->buf);
            ovlentry = find_last_entry(ovl, dstentry->start->buf, 0);
            if (ovlentry == NULL) continue;

            /* Remove original commands */
            for (;;) {
                CdoCommand * cmd = all2cmds(dstentry->start->link_all.next);
                if (cmd == dstentry->end) break;
                cdocmd_free(cmd);
            }

            /* Insert overlay commands */
            ovlcmd = ovlentry->start;
            for (;;) {
                ovlcmd = all2cmds(ovlcmd->link_all.next);
                if (ovlcmd == ovlentry->end) break;
                cdocmd_insert_command(dstentry->end, cdocmd_duplicate(ovlcmd));
            }
        }
        dst->seq = NULL;
        cdooverlay_close(dst);
        if (!more) break;
        level++;
    }
    return 0;
}
