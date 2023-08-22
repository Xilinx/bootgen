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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>

#include "cdo-alloc.h"
#include "cdo-overlay.h"

#define OVERLAY_START 100
#define OVERLAY_END 101

#define all2action(A)  list_item_type((A), OverlayAction, link_all)

typedef struct OverlayAction OverlayAction;

struct OverlayAction {
    LINK link_all;
    LINK children;
    CdoCommand * start;
    CdoCommand * end;
    char name[1];
};

static void free_action(OverlayAction * action);

#ifndef PRINT_OVERLAY_ACTIONS
#define PRINT_OVERLAY_ACTIONS 0
#endif

#if PRINT_OVERLAY_ACTIONS
static void print_overlay_actions(LINK * head, unsigned int level) {
    LINK * l = head->next;
    while (l != head) {
        OverlayAction * action = all2action(l);
        l = l->next;
        printf("%*s%s %p {\n", level*2, "", action->name, action->ovlentry);
        print_overlay_actions(&action->children, level+1);
        printf("%*s}\n", level*2, "");
    }
}
#endif

static void free_action_list(LINK * head) {
    LINK * l = head->next;
    while (l != head) {
        OverlayAction * action = all2action(l);
        l = l->next;
        free_action(action);
    }
}

static void free_action(OverlayAction * action) {
    free_action_list(&action->children);
    list_remove(&action->link_all);
    myfree(action);
}

static CdoCommand * find_matching_end(CdoCommand * start, LINK * lend) {
    LINK * l = start->link_all.next;
    while (l != lend) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if (cmd->type == CdoCmdMarker) {
            if (cmd->value == OVERLAY_START) {
                CdoCommand * end = find_matching_end(cmd, lend);
                if (end == NULL) return NULL;
                l = end->link_all.next;
                continue;
            }
            if (cmd->value == OVERLAY_END) {
                if (strcmp(start->buf, cmd->buf) == 0) {
                    return cmd;
                }
                fprintf(stderr, "unexpected end marker \"%s\", expected  \"%s\"\n", (char *)cmd->buf, (char *)start->buf);
                return NULL;
            }
        }
    }
    fprintf(stderr, "missing end marker \"%s\"\n", (char *)start->buf);
    return NULL;
}

static OverlayAction * add_overlay_action(LINK * head, CdoCommand * startmarker, CdoCommand * endmarker, const char * name, int * enter_scope) {
    LINK * l = head->next;
    OverlayAction * action = NULL;
    int append_action = 1;
    const char * s = strchr(name, '/');
    if (s == NULL) s = name + strlen(name);
    if (s == name) {
        fprintf(stderr, "invalid overlay name: \"%s\"\n", (const char *)startmarker->buf);
        return NULL;
    }
    action = myalloc_zero(sizeof *action + (s - name));
    list_init(&action->link_all);
    list_init(&action->children);
    if (*s == '/') {
        action->start = NULL;
        action->end = NULL;
    } else {
        action->start = startmarker;
        action->end = endmarker;
    }
    memcpy(action->name, name, s - name);
    while (l != head) {
        OverlayAction * action2 = all2action(l);
        l = l->next;
        int cmp = strcmp(action->name, action2->name);
        if (cmp <= 0) {
            if (cmp == 0) {
                if (action->start != NULL) {
                    /* Use new action and free existing. */
                    list_add_last(&action->link_all, &action2->link_all);
                    free_action(action2);
                } else {
                    /* Use existing action and free new. */
                    free_action(action);
                    action = action2;
                }
            } else {
                /* Insert new action before existing. */
                list_add_last(&action->link_all, &action2->link_all);
            }
            append_action = 0;          /* Action handled. */
            break;
        }
    }
    if (append_action) {
        /* Add new action at then end of the list. */
        list_add_last(&action->link_all, head);
    }
    if (*s == '\0') return action;
    s++;                                /* Skip '/' */
    if (*s == '\0') {
        *enter_scope = 1;
        return action;
    }
    return add_overlay_action(&action->children, startmarker, endmarker, s, enter_scope);
}

/*
 * Build a tree of overlay actions.  Actions selects a scope and can
 * optionally replace the content of the scope. Child actions do the
 * same on inner scopes.
 */
static int create_overlay_actions(LINK * actions, LINK * l, LINK * lend) {
    while (l != lend) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if (cmd->type == CdoCmdMarker) {
            if (cmd->value == OVERLAY_START) {
                /* Marker start */
                CdoCommand * end = find_matching_end(cmd, lend);
                if (end == NULL) return 1;
                int enter_scope = 0;
                OverlayAction * action = add_overlay_action(actions, cmd, end, cmd->buf, &enter_scope);
                if (action == NULL) {
                    return 1;
                }
                if (enter_scope) {
                    if (create_overlay_actions(&action->children, cmd->link_all.next, &end->link_all)) {
                        return 1;
                    }
                } else {
                    l = cmd->link_all.next;
                    while (l != &end->link_all) {
                        CdoCommand * cmd2 = all2cmds(l);
                        l = l->next;
                        if (cmd2->type == CdoCmdMarker && cmd2->value == OVERLAY_START) {
                            const char * name = cmd2->buf;
                            const char * s = strchr(name, '/');
                            if (s != NULL) {
                                fprintf(stderr, "markers with path cannot be nested in overlay: \"%s\"\n", name);
                                return 1;
                            }
                        }
                    }
                }
                l = end->link_all.next;
            } else if (cmd->value == OVERLAY_END) {
                /* Marker end */
                fprintf(stderr, "missing start marker for \"%s\"\n", (char *)cmd->buf);
                goto error;
            }
        }
    }
    return 0;

error:
    return 1;
}

static OverlayAction * find_action(LINK * head, const char * name) {
    LINK * l = head->next;
    while (l != head) {
        OverlayAction * action = all2action(l);
        l = l->next;
        if (strcmp(action->name, name) == 0) return action;
    }
    return NULL;
}

static void remove_commands_between(CdoCommand * start, CdoCommand * end) {
    for (;;) {
        CdoCommand * cmd = all2cmds(start->link_all.next);
        if (cmd == end) break;
        cdocmd_free(cmd);
    }
}

static void insert_overlay_commands(CdoCommand * cmd, CdoCommand * start, CdoCommand * end) {
    CdoSequence * seq = cdocmd_create_sequence();
    CdoCommand * ovlcmd = start;
    for (;;) {
        ovlcmd = all2cmds(ovlcmd->link_all.next);
        if (ovlcmd == end) break;
        cdocmd_append_command(seq, cdocmd_duplicate(ovlcmd));
    }
    cdocmd_insert_seq(cmd, seq);
    cdocmd_delete_sequence(seq);
}

static void insert_overlay_marker(CdoCommand * cmd, const char * name) {
    CdoSequence * seq = cdocmd_create_sequence();
    cdocmd_add_marker(seq, OVERLAY_START, name);
    cdocmd_add_marker(seq, OVERLAY_END, name);
    cdocmd_insert_seq(cmd, seq);
    cdocmd_delete_sequence(seq);
}

static int apply_nested_overlay(CdoCommand * startmarker, CdoCommand * endmarker, LINK * actions) {
    LINK * l = startmarker->link_all.next;
    LINK * lact = actions->next;
    while (l != &endmarker->link_all) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if (cmd->type == CdoCmdMarker) {
            if (cmd->value == OVERLAY_START) {
                /* Marker start */
                CdoCommand * end = find_matching_end(cmd, &endmarker->link_all);
                if (end == NULL) return 1;
                l = end->link_all.next;
                while (lact != actions) {
                    OverlayAction * action = all2action(lact);
                    int cmp = strcmp(action->name, cmd->buf);
                    if (cmp > 0) break;
                    if (cmp < 0) {
                        /* Insert new submarker before current marker */
                        insert_overlay_marker(cmd, action->name);
                        end = all2cmds(cmd->link_all.prev);
                        cmd = all2cmds(end->link_all.prev);
                        if (action->start != NULL) {
                            insert_overlay_commands(end, action->start, action->end);
                        }
                    } else if (action->start != NULL) {
                        remove_commands_between(cmd, end);
                        insert_overlay_commands(end, action->start, action->end);
                    }
                    if (apply_nested_overlay(cmd, end, &action->children)) goto error;
                    l = end->link_all.next;
                    lact = lact->next;
                }
            } else if (cmd->value == OVERLAY_END) {
                /* Marker end */
                fprintf(stderr, "missing start marker for \"%s\"\n", (char *)cmd->buf);
                goto error;
            }
        }
    }
    while (lact != actions) {
        OverlayAction * action = all2action(lact);
        /* Insert new submarker before current marker */
        insert_overlay_marker(endmarker, action->name);
        CdoCommand * end = all2cmds(endmarker->link_all.prev);
        CdoCommand * cmd = all2cmds(end->link_all.prev);
        if (action->start != NULL) {
            insert_overlay_commands(end, action->start, action->end);
        }
        if (apply_nested_overlay(cmd, end, &action->children)) goto error;
        lact = lact->next;
    }
    return 0;

error:
    return 1;
}

CdoOverlayInfo * cdooverlay_open(CdoSequence * seq) {
    CdoOverlayInfo * info = myalloc_zero(sizeof *info);
    list_init(&info->actions);
    if (create_overlay_actions(&info->actions, seq->cmds.next, &seq->cmds)) {
        cdooverlay_close(info);
        return NULL;
    }
#if PRINT_OVERLAY_ACTIONS
    print_overlay_actions(&info->actions, 0);
#endif
    return info;
}

void cdooverlay_close(CdoOverlayInfo * info) {
    if (info->seq) cdocmd_delete_sequence(info->seq);
    free_action_list(&info->actions);
    myfree(info);
}

int cdooverlay_apply(CdoSequence * seq, CdoOverlayInfo * ovl) {
    LINK * l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if (cmd->type == CdoCmdMarker) {
            if (cmd->value == OVERLAY_START) {
                /* Marker start */
                CdoCommand * end = find_matching_end(cmd, &seq->cmds);
                if (end == NULL) return 1;
                OverlayAction * action = find_action(&ovl->actions, cmd->buf);
                if (action != NULL) {
                    if (action->start != NULL) {
                        remove_commands_between(cmd, end);
                        insert_overlay_commands(end, action->start, action->end);
                    }
                    if (apply_nested_overlay(cmd, end, &action->children)) goto error;
                }
                l = end->link_all.next;
            } else if (cmd->value == OVERLAY_END) {
                /* Marker end */
                fprintf(stderr, "missing start marker for \"%s\"\n", (char *)cmd->buf);
                goto error;
            }
        }
    }
    return 0;

error:
    return 1;
}
