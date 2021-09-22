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
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <inttypes.h>
#include "cdo-alloc.h"
#include "cdo-command.h"
UserKeys user_keys;

static CdoSequence * default_seq;

#define USER_KEYS_BASE_ADDR 0xF11E0110
#define USER_KEYS_END_ADDR  0xF11E020C
#define USER_KEY_OFFSET     0x20

void cdocmd_free(CdoCommand * cmd) {
    if (!list_is_empty(&cmd->link_all)) list_remove(&cmd->link_all);
    if (cmd->buf) myfree(cmd->buf);
    myfree(cmd);
}

CdoCommand * cdocmd_alloc(CdoCmdType type) {
    CdoCommand * cmd = (CdoCommand *)myalloc_zero(sizeof *cmd);
    cmd->type = type;
    return cmd;
};

CdoSequence * cdocmd_create_sequence(void) {
    CdoSequence * seq = myalloc_zero(sizeof *seq);
    seq->version = DEFAULT_CDO_VERSION;
    list_init(&seq->cmds);
    return seq;
}

void cdocmd_delete_sequence(CdoSequence * seq) {
    LINK * l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        cdocmd_free(cmd);
    }
    myfree(seq);
}

static void * copy_buf(void * buf, uint32_t count, uint32_t be) {
    uint32_t * p = NULL;
    if (buf != NULL) {
        p = myalloc(count * 4);
        memcpy(p, buf, count * 4);
        if (is_be_host() != be) {
            uint32_t i;
            for (i = 0; i < count; i++) {
                p[i] = u32swap(p[i]);
            }
        }
    }
    return p;
}

CdoCommand * cdocmd_duplicate(CdoCommand * origcmd) {
    CdoCommand * cmd = (CdoCommand *)myalloc(sizeof *cmd);
    *cmd = *origcmd;
    list_init(&cmd->link_all)
    cmd->buf = copy_buf(cmd->buf, cmd->count, is_be_host());
    return cmd;
}

void cdocmd_set_default_sequence(CdoSequence * seq) {
    default_seq = seq;
}

static void add_command(CdoSequence * seq, CdoCommand * cmd) {
    /* Append cmd to sequence */
    if (seq == NULL) seq = default_seq;
    list_add_last(&cmd->link_all, &seq->cmds);
}

void cdocmd_append_command(CdoSequence * seq, CdoCommand * newcmd) {
    /* Append newcmd to sequence */
    if (!list_is_empty(&newcmd->link_all)) list_remove(&newcmd->link_all);
    add_command(seq, newcmd);
}

void cdocmd_insert_command(CdoCommand * cmd, CdoCommand * newcmd) {
    /* Insert newcmd before cmd */
    if (!list_is_empty(&newcmd->link_all)) list_remove(&newcmd->link_all);
    list_add_last(&newcmd->link_all, &cmd->link_all);
}

static uint8_t randchar() {
    static char chrs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    return chrs[rand() % (sizeof chrs - 1)];
}

static uint32_t randu32() {
    uint32_t width = rand() & 3;
    uint32_t value = rand() & 255;
    while (width-- > 0) {
        value <<= 8;
        value |= rand() & 255;
    }
    return value;
}

static uint64_t randu64() {
    uint64_t width = rand() & 7;
    uint64_t value = rand() & 255;
    while (width-- > 0) {
        value <<= 8;
        value |= rand() & 255;
    }
    return value;
}

void cdocmd_add_random_command(CdoSequence * seq, uint32_t * levelp) {
    uint32_t level = *levelp;
    CdoCmdType type;
    CdoCommand * cmd;
    do {
        type = randu32() % CdoCmdLast;
    } while (type == CdoCmdGeneric ||
             type == CdoCmdSection ||
             type == CdoCmdInclude ||
             type == CdoCmdComment ||
             type == CdoCmdSetBaseAddress ||
             (type == CdoCmdProc && level != 0) ||
             (type == CdoCmdBegin && level >= (randu32() & 3)) ||
             (type == CdoCmdEnd && level == 0) ||
             (type == CdoCmdBreak && level == 0));
    cmd = cdocmd_alloc(type);
    cmd->id = randu32();
    cmd->dstaddr = randu64();
    cmd->srcaddr = randu64();
    cmd->mask = randu32();
    cmd->value = randu32();
    cmd->count = randu32();
    cmd->flags = randu32();
    if (cmd->type == CdoCmdMaskPoll) {
        if ((randu32() & 3) == 0) cmd->flags = 0;
    }
    if (cmd->type == CdoCmdNpiPreCfg ||
        cmd->type == CdoCmdNpiSeq ||
        cmd->type == CdoCmdNpiWrite ||
        cmd->type == CdoCmdNpiShutdown) {
        cmd->dstaddr &= 0xffffffff;
    }
    if (cmd->type == CdoCmdWrite ||
        cmd->type == CdoCmdWriteKeyhole ||
        cmd->type == CdoCmdDmaXfer ||
        cmd->type == CdoCmdCframeRead ||
        cmd->type == CdoCmdNop ||
        cmd->type == CdoCmdEventLogging ||
        cmd->type == CdoCmdNpiWrite ||
        cmd->type == CdoCmdPmIoctl ||
        cmd->type == CdoCmdPmQueryData ||
        cmd->type == CdoCmdPmDescribeNodes ||
        cmd->type == CdoCmdPmAddNode ||
        cmd->type == CdoCmdPmAddNodeParent ||
        cmd->type == CdoCmdPmAddRequirement ||
        cmd->type == CdoCmdPmInitNode) {
        uint32_t * p;
        uint32_t i;
        cmd->count = (cmd->count & 15) + 1;
        cmd->buf = p = (uint32_t *)myalloc(cmd->count * 4);
        for (i = 0; i < cmd->count; i++) {
            p[i] = randu32();
        }
        if (cmd->type == CdoCmdDmaXfer) {
            if (rand() & 1) cmd->srcaddr = 0;
            if (cmd->srcaddr != 0) {
                free(cmd->buf);
                cmd->buf = NULL;
            }
        }
    } else if (cmd->type == CdoCmdComment ||
               cmd->type == CdoCmdSetBoard ||
               cmd->type == CdoCmdLogString ||
               cmd->type == CdoCmdMarker ||
               cmd->type == CdoCmdBegin ||
               cmd->type == CdoCmdPmAddNodeName) {
        uint8_t * p;
        uint32_t count = (cmd->count & 7) + 1;
        uint32_t i;
        cmd->count = count / 4 + 1;
        cmd->buf = p = (uint8_t *)myalloc(cmd->count * 4);
        for (i = 0; i < count; i++) {
            p[i] = randchar();
        }
        p[i] = '\0';
    } else if (cmd->type == CdoCmdBreak) {
        cmd->value = (cmd->value % level) + 1;
    }
    if (cmd->type == CdoCmdProc) level++;
    if (cmd->type == CdoCmdBegin) level++;
    if (cmd->type == CdoCmdEnd) level--;
    add_command(seq, cmd);
    *levelp = level;
}

void cdocmd_add_generic_command(CdoSequence * seq, uint32_t id, void * buf, uint32_t count, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdGeneric);
    cmd->id = id;
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_comment(CdoSequence * seq, const char * comment, ...) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdComment);
    va_list ap;
    int size = 1;
    cmd->buf = (char *)myalloc(size);
    while (1) {
        int n;
        va_start(ap, comment);
        n = vsnprintf(cmd->buf, size, comment, ap);
        va_end(ap);
        if (n >= 0) {
            if (n < size) break;
            size = n + 1;
        } else {
            size *= 2;
        }
        cmd->buf = myrealloc(cmd->buf, size);
    }
    cmd->count = strlen(cmd->buf) / 4 + 1;
    add_command(seq, cmd);
}

void cdocmd_add_section(CdoSequence * seq, uint32_t id) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdSection);
    cmd->id = id;
    add_command(seq, cmd);
}

void cdocmd_add_include(CdoSequence * seq, const char * name) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdInclude);
    cmd->count = strlen(name) / 4 + 1;
    cmd->buf = strdup(name);
    add_command(seq, cmd);
}

static CdoCommand * build_block_write(uint64_t addr, void * buf, uint32_t count, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdWrite);
    cmd->dstaddr = addr;
    cmd->count = count;

    if (addr  >= USER_KEYS_BASE_ADDR && addr <= USER_KEYS_END_ADDR)
    {
        /* copy user keys*/
        int i, j;
        for (j = 0; j < 8; j++)
        {
            for (i = 0; i < 8; i++)
            {
                if ((addr == USER_KEYS_BASE_ADDR + (USER_KEY_OFFSET * j) + (4 * i)) && (user_keys.user_keys_array[j] != 0))
                {
                    memcpy(buf, &user_keys.user_keys_array[j][7 - i], sizeof(uint32_t));
                }
            }
        }
    }

    cmd->buf = copy_buf(buf, count, be);
    return cmd;
}

static CdoCommand * build_set_block(uint64_t addr, uint32_t count, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdSetBlock);
    cmd->dstaddr = addr;
    cmd->count = count;
    cmd->value = value;
    return cmd;
}

void cdocmd_add_mask_poll(CdoSequence * seq, uint64_t addr, uint32_t mask, uint32_t value, uint32_t count, uint32_t flags) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdMaskPoll);
    cmd->dstaddr = addr;
    cmd->mask = mask;
    cmd->value = value;
    cmd->count = count;
    cmd->flags = flags;
    add_command(seq, cmd);
}

void cdocmd_add_mask_write(CdoSequence * seq, uint64_t addr, uint32_t mask, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdMaskWrite);
    cmd->dstaddr = addr;
    cmd->mask = mask;
    cmd->value = value;
    add_command(seq, cmd);
}

void cdocmd_add_write(CdoSequence * seq, uint64_t addr, uint32_t value) {
    cdocmd_add_block_write(seq, addr, 1, &value, is_be_host());
}

void cdocmd_add_delay(CdoSequence * seq, uint32_t delay) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdDelay);
    cmd->value = delay;
    add_command(seq, cmd);
}

void cdocmd_add_block_write(CdoSequence * seq, uint64_t addr, uint32_t count, void * buf, uint32_t be) {
    CdoCommand * cmd = build_block_write(addr, buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_keyhole_write(CdoSequence * seq, uint64_t addr, uint32_t keyhole_size, uint32_t count, void * buf, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdWriteKeyhole);
    cmd->dstaddr = addr;
    cmd->value = keyhole_size;
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_block_set(CdoSequence * seq, uint64_t addr, uint32_t count, uint32_t value) {
    CdoCommand * cmd = build_set_block(addr, count, value);
    add_command(seq, cmd);
}

void cdocmd_add_dma_xfer(CdoSequence * seq, uint64_t srcaddr, uint64_t dstaddr, uint32_t count, uint32_t flags, void * buf, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdDmaXfer);
    cmd->srcaddr = srcaddr;
    cmd->dstaddr = dstaddr;
    cmd->count = count;
    cmd->flags = flags;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_npi_seq(CdoSequence * seq, uint32_t addr, uint32_t flags) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdNpiSeq);
    cmd->dstaddr = addr;
    cmd->flags = flags;
    add_command(seq, cmd);
}

void cdocmd_add_npi_precfg(CdoSequence * seq, uint32_t addr, uint32_t flags) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdNpiPreCfg);
    cmd->dstaddr = addr;
    cmd->flags = flags;
    add_command(seq, cmd);
}

void cdocmd_add_npi_write(CdoSequence * seq, uint32_t addr, uint32_t flags, uint32_t count, void * buf, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdNpiWrite);
    cmd->dstaddr = addr;
    cmd->flags = flags;
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_npi_shutdown(CdoSequence * seq, uint32_t addr, uint32_t flags) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdNpiShutdown);
    cmd->dstaddr = addr;
    cmd->flags = flags;
    add_command(seq, cmd);
}

void cdocmd_add_pm_get_api_version(CdoSequence * seq) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmGetApiVersion);
    add_command(seq, cmd);
}

void cdocmd_add_pm_get_device_status(CdoSequence * seq, uint32_t nodeid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmGetDeviceStatus);
    cmd->id = nodeid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_request_suspend(CdoSequence * seq, uint32_t nodeid, uint32_t ack, uint32_t latency, uint32_t state) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmRequestSuspend);
    cmd->id = nodeid;
    cmd->flags = ack;
    cmd->count = latency;
    cmd->value = state;
    add_command(seq, cmd);
}

void cdocmd_add_pm_self_suspend(CdoSequence * seq, uint32_t nodeid, uint32_t latency, uint32_t state, uint64_t addr) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmSelfSuspend);
    cmd->id = nodeid;
    cmd->count = latency;
    cmd->value = state;
    cmd->dstaddr = addr;
    add_command(seq, cmd);
}

void cdocmd_add_pm_force_powerdown(CdoSequence * seq, uint32_t nodeid, uint32_t ack) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmForcePowerdown);
    cmd->id = nodeid;
    cmd->flags = ack;
    add_command(seq, cmd);
}

void cdocmd_add_pm_abort_suspend(CdoSequence * seq, uint32_t reason, uint32_t nodeid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmAbortSuspend);
    cmd->value = reason;
    cmd->id = nodeid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_request_wakeup(CdoSequence * seq, uint32_t nodeid, uint64_t addr, uint32_t ack) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmRequestWakeup);
    cmd->id = nodeid;
    cmd->dstaddr = addr;
    cmd->flags = ack;
    add_command(seq, cmd);
}

void cdocmd_add_pm_set_wakeup_source(CdoSequence * seq, uint32_t target, uint32_t source, uint32_t enable) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmSetWakeupSource);
    cmd->id = target;
    cmd->value = source;
    cmd->flags = enable;
    add_command(seq, cmd);
}

void cdocmd_add_pm_system_shutdown(CdoSequence * seq, uint32_t type, uint32_t subtype) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmSystemShutdown);
    cmd->id = type;
    cmd->value = subtype;
    add_command(seq, cmd);
}

void cdocmd_add_pm_request_device(CdoSequence * seq, uint32_t nodeid, uint32_t capabilities, uint32_t qos, uint32_t ack) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmRequestDevice);
    cmd->id = nodeid;
    cmd->value = capabilities;
    cmd->count = qos;
    cmd->flags = ack;
    add_command(seq, cmd);
}

void cdocmd_add_pm_release_device(CdoSequence * seq, uint32_t nodeid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmReleaseDevice);
    cmd->id = nodeid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_set_requirement(CdoSequence * seq, uint32_t nodeid, uint32_t capabilities, uint32_t qos, uint32_t ack) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmSetRequirement);
    cmd->id = nodeid;
    cmd->value = capabilities;
    cmd->count = qos;
    cmd->flags = ack;
    add_command(seq, cmd);
}

void cdocmd_add_pm_set_max_latency(CdoSequence * seq, uint32_t nodeid, uint32_t latency) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmSetMaxLatency);
    cmd->id = nodeid;
    cmd->count = latency;
    add_command(seq, cmd);
}

void cdocmd_add_pm_reset_assert(CdoSequence * seq, uint32_t nodeid, uint32_t action) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmResetAssert);
    cmd->id = nodeid;
    cmd->flags = action;
    add_command(seq, cmd);
}

void cdocmd_add_pm_reset_get_status(CdoSequence * seq, uint32_t nodeid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmResetGetStatus);
    cmd->id = nodeid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_pinctrl_request(CdoSequence * seq, uint32_t pinid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmPinctrlRequest);
    cmd->id = pinid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_pinctrl_release(CdoSequence * seq, uint32_t pinid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmPinctrlRelease);
    cmd->id = pinid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_pinctrl_get_function(CdoSequence * seq, uint32_t pinid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmPinctrlGetFunction);
    cmd->id = pinid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_pinctrl_set_function(CdoSequence * seq, uint32_t pinid, uint32_t funcid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmPinctrlSetFunction);
    cmd->id = pinid;
    cmd->value = funcid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_pinctrl_config_param_get(CdoSequence * seq, uint32_t pinid, uint32_t param) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmPinctrlConfigParamGet);
    cmd->id = pinid;
    cmd->flags = param;
    add_command(seq, cmd);
}

void cdocmd_add_pm_pinctrl_config_param_set(CdoSequence * seq, uint32_t pinid, uint32_t param, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmPinctrlConfigParamSet);
    cmd->id = pinid;
    cmd->flags = param;
    cmd->value = value;
    add_command(seq, cmd);
}

void cdocmd_add_pm_ioctl(CdoSequence * seq, uint32_t nodeid, uint32_t ioctlid, void * buf, uint32_t count, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmIoctl);
    cmd->id = nodeid;
    cmd->flags = ioctlid;
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_pm_query_data(CdoSequence * seq, uint32_t id, void * buf, uint32_t count, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmQueryData);
    cmd->id = id;
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_pm_clock_enable(CdoSequence * seq, uint32_t nodeid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmClockEnable);
    cmd->id = nodeid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_clock_disable(CdoSequence * seq, uint32_t nodeid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmClockDisable);
    cmd->id = nodeid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_clock_getstate(CdoSequence * seq, uint32_t nodeid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmClockGetState);
    cmd->id = nodeid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_clock_setdivider(CdoSequence * seq, uint32_t nodeid, uint32_t div) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmClockSetDivider);
    cmd->id = nodeid;
    cmd->value = div;
    add_command(seq, cmd);
}

void cdocmd_add_pm_clock_getdivider(CdoSequence * seq, uint32_t nodeid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmClockGetDivider);
    cmd->id = nodeid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_clock_setrate(CdoSequence * seq, uint32_t nodeid, uint32_t rate) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmClockSetRate);
    cmd->id = nodeid;
    cmd->value = rate;
    add_command(seq, cmd);
}

void cdocmd_add_pm_clock_getrate(CdoSequence * seq, uint32_t nodeid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmClockGetRate);
    cmd->id = nodeid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_clock_setparent(CdoSequence * seq, uint32_t nodeid, uint32_t parentid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmClockSetParent);
    cmd->id = nodeid;
    cmd->value = parentid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_clock_getparent(CdoSequence * seq, uint32_t nodeid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmClockGetParent);
    cmd->id = nodeid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_pll_set_parameter(CdoSequence * seq, uint32_t nodeid, uint32_t param, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmPllSetParameter);
    cmd->id = nodeid;
    cmd->flags = param;
    cmd->value = value;
    add_command(seq, cmd);
}

void cdocmd_add_pm_pll_get_parameter(CdoSequence * seq, uint32_t nodeid, uint32_t param) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmPllGetParameter);
    cmd->id = nodeid;
    cmd->flags = param;
    add_command(seq, cmd);
}

void cdocmd_add_pm_pll_set_mode(CdoSequence * seq, uint32_t nodeid, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmPllSetMode);
    cmd->id = nodeid;
    cmd->value = value;
    add_command(seq, cmd);
}

void cdocmd_add_pm_pll_get_mode(CdoSequence * seq, uint32_t nodeid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmPllGetMode);
    cmd->id = nodeid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_add_subsystem(CdoSequence * seq, uint32_t ssid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmAddSubsystem);
    cmd->id = ssid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_destroy_subsystem(CdoSequence * seq, uint32_t ssid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmDestroySubsystem);
    cmd->id = ssid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_describe_nodes(CdoSequence * seq, void * buf, uint32_t count, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmDescribeNodes);
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_pm_add_node(CdoSequence * seq, uint32_t nodeid, uint32_t count, void * buf, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmAddNode);
    cmd->id = nodeid;
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_pm_add_node_parent(CdoSequence * seq, uint32_t nodeid, uint32_t count, void * buf, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmAddNodeParent);
    cmd->id = nodeid;
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_pm_add_node_name(CdoSequence * seq, uint32_t nodeid, const char * name) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmAddNodeName);
    cmd->id = nodeid;
    cmd->count = strlen(name) / 4 + 1;
    cmd->buf = strdup(name);
    add_command(seq, cmd);
}

void cdocmd_add_pm_add_requirement(CdoSequence * seq, uint32_t ssid, uint32_t nodeid, uint32_t flags, uint32_t count, void * buf, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmAddRequirement);
    cmd->id = ssid;
    cmd->value = nodeid;
    cmd->flags = flags;
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_pm_set_current_subsystem(CdoSequence * seq, uint32_t ssid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmSetCurrentSubsystem);
    cmd->id = ssid;
    add_command(seq, cmd);
}

void cdocmd_add_pm_init_node(CdoSequence * seq, uint32_t nodeid, uint32_t function, uint32_t count, void * buf, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmInitNode);
    cmd->id = nodeid;
    cmd->value = function;
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_pm_feature_check(CdoSequence * seq, uint32_t id) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmFeatureCheck);
    cmd->id = id;
    add_command(seq, cmd);
}

void cdocmd_add_pm_iso_control(CdoSequence * seq, uint32_t nodeid, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmIsoControl);
    cmd->id = nodeid;
    cmd->value = value;
    add_command(seq, cmd);
}

void cdocmd_add_cfu_set_crc32(CdoSequence * seq, uint32_t type, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdCfuSetCrc32);
    cmd->flags = type;
    cmd->value = value;
    add_command(seq, cmd);
}

void cdocmd_add_cfu_decompress(CdoSequence * seq, uint32_t type) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdCfuDecompress);
    cmd->flags = type;
    add_command(seq, cmd);
}

void cdocmd_add_cfu_cram_rw(CdoSequence * seq, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdCfuCramRW);
    cmd->value = value;
    add_command(seq, cmd);
}

void cdocmd_add_cfu_seu_go(CdoSequence * seq, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdCfuSeuGo);
    cmd->flags = value;
    add_command(seq, cmd);
}

void cdocmd_add_cfu_crc8_dis(CdoSequence * seq, uint32_t type) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdCfuCrc8Dis);
    cmd->flags = type;
    add_command(seq, cmd);
}

void cdocmd_add_cfu_ssi_per_slr_pr(CdoSequence * seq, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdCfuSsiPerSlrPr);
    cmd->flags = value;
    add_command(seq, cmd);
}

void cdocmd_add_cfu_gsr_gsc(CdoSequence * seq, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdCfuGsrGsc);
    cmd->flags = value;
    add_command(seq, cmd);
}

void cdocmd_add_cfu_gcap_clk_en(CdoSequence * seq, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdCfuGcapClkEn);
    cmd->flags = value;
    add_command(seq, cmd);
}

void cdocmd_add_cfu_cfi_type(CdoSequence * seq, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdCfuCfiType);
    cmd->value = value;
    add_command(seq, cmd);
}

void cdocmd_add_cframe_read(CdoSequence * seq, uint32_t param, uint64_t addr, uint32_t read_count, uint32_t count, void * buf, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdCframeRead);
    cmd->flags = param;
    cmd->dstaddr = addr;
    cmd->value = read_count;
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_ssit_sync_master(CdoSequence * seq) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdSsitSyncMaster);
    add_command(seq, cmd);
}

void cdocmd_add_ssit_sync_slaves(CdoSequence * seq, uint32_t mask, uint32_t count) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdSsitSyncSlaves);
    cmd->mask = mask;
    cmd->count = count;
    add_command(seq, cmd);
}

void cdocmd_add_ssit_wait_slaves(CdoSequence * seq, uint32_t mask, uint32_t count) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdSsitWaitSlaves);
    cmd->mask = mask;
    cmd->count = count;
    add_command(seq, cmd);
}

void cdocmd_add_nop(CdoSequence * seq, uint32_t count, void * buf, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdNop);
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_event_logging(CdoSequence * seq, uint32_t subcmd, uint32_t count, void * buf, uint32_t be) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdEventLogging);
    cmd->value = subcmd;
    cmd->count = count;
    cmd->buf = copy_buf(buf, count, be);
    add_command(seq, cmd);
}

void cdocmd_add_set_board(CdoSequence * seq, const char * name) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdSetBoard);
    cmd->count = strlen(name) / 4 + 1;
    cmd->buf = strdup(name);
    add_command(seq, cmd);
}

void cdocmd_add_set_plm_wdt(CdoSequence * seq, uint32_t nodeid, uint32_t periodicity) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdSetPlmWdt);
    cmd->id = nodeid;
    cmd->value = periodicity;
    add_command(seq, cmd);
}

void cdocmd_add_log_string(CdoSequence * seq, const char * name) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdLogString);
    cmd->count = strlen(name) / 4 + 1;
    cmd->buf = strdup(name);
    add_command(seq, cmd);
}

void cdocmd_add_log_address(CdoSequence * seq, uint64_t addr) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdLogAddress);
    cmd->srcaddr = addr;
    add_command(seq, cmd);
}

void cdocmd_add_marker(CdoSequence * seq, uint32_t value, const char * name) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdMarker);
    cmd->value = value;
    cmd->count = strlen(name) / 4 + 1;
    cmd->buf = strdup(name);
    add_command(seq, cmd);
}

void cdocmd_add_proc(CdoSequence * seq, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdProc);
    cmd->value = value;
    add_command(seq, cmd);
}

void cdocmd_add_begin(CdoSequence * seq, const char * name) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdBegin);
    cmd->count = strlen(name) / 4 + 1;
    cmd->buf = strdup(name);
    add_command(seq, cmd);
}

void cdocmd_add_end(CdoSequence * seq) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdEnd);
    add_command(seq, cmd);
}

void cdocmd_add_break(CdoSequence * seq, uint32_t value) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdBreak);
    cmd->value = value;
    add_command(seq, cmd);
}

void cdocmd_add_pm_register_notifier(CdoSequence * seq, uint32_t nodeid, uint32_t mask, uint32_t arg1, uint32_t arg2) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdPmRegisterNotifier);
    cmd->id = nodeid;
    cmd->mask = mask;
    cmd->value = arg1;
    cmd->count = arg2;
    add_command(seq, cmd);
}

void cdocmd_add_em_set_action(CdoSequence * seq, uint32_t nodeid, uint32_t action, uint32_t mask) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdEmSetAction);
    cmd->id = nodeid;
    cmd->value = action;
    cmd->mask = mask;
    add_command(seq, cmd);
}

void cdocmd_add_ldr_set_image_info(CdoSequence * seq, uint32_t nodeid, uint32_t uid, uint32_t puid, uint32_t funcid) {
    CdoCommand * cmd = cdocmd_alloc(CdoCmdLdrSetImageInfo);
    cmd->id = nodeid;
    cmd->value = uid;
    cmd->mask = puid;
    cmd->count = funcid;
    add_command(seq, cmd);
}

void cdocmd_insert_seq(CdoCommand * cmd, CdoSequence * seq) {
    list_concat(&cmd->link_all, &seq->cmds);
    list_init(&seq->cmds);
}

void cdocmd_concat_seq(CdoSequence * seq, CdoSequence * seq2) {
    list_concat(&seq->cmds, &seq2->cmds);
    list_init(&seq2->cmds);
}

void cdocmd_rewrite_address(CdoSequence * seq) {
    LINK * l = seq->cmds.next;
    uint64_t mask = 0x1000 - 1;
    uint64_t baseaddr = 0;
    int basevalid = 0;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case CdoCmdWrite:
        case CdoCmdMaskWrite:
        case CdoCmdMaskPoll:
            if (!basevalid || baseaddr != (cmd->dstaddr & ~mask)) {
                CdoCommand * newcmd = cdocmd_alloc(CdoCmdSetBaseAddress);
                newcmd->dstaddr = baseaddr = cmd->dstaddr & ~mask;
                cdocmd_insert_command(cmd, newcmd);
                basevalid = 1;
            }
            cmd->dstaddr &= mask;
            break;
        default:
            break;
        }
    }
}

void cdocmd_rewrite_block(CdoSequence * seq) {
    LINK * l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case CdoCmdWrite:
            if (cmd->count > 1) {
                uint64_t dstaddr = cmd->dstaddr;
                uint32_t * valuep = (uint32_t *)cmd->buf;
                uint32_t i;
                for (i = 0; i < cmd->count; i++) {
                    CdoCommand * newcmd = build_block_write(dstaddr, valuep, 1, is_be_host());
                    cdocmd_insert_command(cmd, newcmd);
                    dstaddr += 4;
                    valuep++;
                }
                cdocmd_free(cmd);
            }
            break;
        default:
            break;
        }
    }
}

void cdocmd_rewrite_sequential(CdoSequence * seq) {
    LINK * l = seq->cmds.next;
    CdoCommand * prevcmd = NULL;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case CdoCmdWrite:
            if (prevcmd != NULL && prevcmd->dstaddr + prevcmd->count*4 == cmd->dstaddr) {
                prevcmd->buf = myrealloc(prevcmd->buf, (prevcmd->count + cmd->count)*4);
                memcpy((char *)prevcmd->buf + prevcmd->count*4, cmd->buf, cmd->count*4);
                prevcmd->count += cmd->count;
                cdocmd_free(cmd);
            } else {
                prevcmd = cmd;
            }
            break;
        default:
            prevcmd = NULL;
            break;
        }
    }
}

void cdocmd_rewrite_repeat(CdoSequence * seq) {
    LINK * l = seq->cmds.next;
    uint64_t minset = 3;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case CdoCmdWrite: {
            uint64_t addr = cmd->dstaddr;
            uint32_t * start = (uint32_t *)cmd->buf;
            uint32_t * end = start + cmd->count;
            uint32_t * p = start;
            uint32_t value = 0;
            uint32_t repeat = 0;
            while (p < end) {
                uint32_t value2 = *p++;
                if (value2 == value) {
                    repeat++;
                    if (repeat == minset) {
                        uint32_t count = p - start - repeat;
                        if (count > 0) {
                            CdoCommand * newcmd = build_block_write(addr, start, count, is_be_host());
                            cdocmd_insert_command(cmd, newcmd);
                            addr += count * 4;
                            start += count;
                        }
                    }
                    continue;
                }
                if (repeat >= minset) {
                    CdoCommand * newcmd = build_set_block(addr, repeat, u32be(value));
                    cdocmd_insert_command(cmd, newcmd);
                    addr += repeat * 4;
                    start += repeat;
                }
                value = value2;
                repeat = 1;
            }
            if (repeat >= minset) {
                CdoCommand * newcmd = build_set_block(addr, repeat, u32be(value));
                cdocmd_insert_command(cmd, newcmd);
                cdocmd_free(cmd);
                addr += repeat * 4;
                start += repeat;
            } else if (cmd->dstaddr != addr) {
                CdoCommand * newcmd = build_block_write(addr, start, p - start, is_be_host());
                cdocmd_insert_command(cmd, newcmd);
                cdocmd_free(cmd);
            }
            break;
        }
        default:
            break;
        }
    }
}

void cdocmd_remove_comments(CdoSequence * seq) {
    LINK * l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case CdoCmdComment:
            cdocmd_free(cmd);
            break;
        default:
            break;
        }
    }
}
