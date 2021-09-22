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
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include "cdo-load.h"
#include "cdo-source.h"

#if defined(_WIN32)
#define strcasecmp(x,y) stricmp(x,y)
#define strncasecmp(x,y,z) strnicmp(x,y,z)
#endif

#define isspc(c) (c == ' ' || c == '\t')
#define istok(c) (c != '\0' && !isspc(c))
#define skipsp(s)                               \
    do {                                        \
        int c = *s;                             \
        while (isspc(c)) c = *++s;              \
    } while (0)
#define skiptok(s)                              \
    do {                                        \
        int c = *s;                             \
        while (istok(c)) c = *++s;              \
    } while (0)

typedef struct command_info command_info;
struct command_info {
    char * name;
    uint32_t id;
} command_list[] = {
    { "section", CdoCmdSection },
    { "generic", CdoCmdGeneric },
    { "include", CdoCmdInclude },
    { "set_base_address", CdoCmdSetBaseAddress },
    { "write", CdoCmdWrite },
    { "set", CdoCmdSetBlock },
    { "write_keyhole", CdoCmdWriteKeyhole },
    { "mask_write", CdoCmdMaskWrite },
    { "mask_poll", CdoCmdMaskPoll },
    { "delay", CdoCmdDelay },
    { "dma_xfer", CdoCmdDmaXfer },
    { "cframe_read", CdoCmdCframeRead },
    { "ssit_sync_master", CdoCmdSsitSyncMaster },
    { "ssit_sync_slaves", CdoCmdSsitSyncSlaves },
    { "ssit_wait_slaves", CdoCmdSsitWaitSlaves },
    { "nop", CdoCmdNop },
    { "event_logging", CdoCmdEventLogging },
    { "set_board", CdoCmdSetBoard },
    { "set_plm_wdt", CdoCmdSetPlmWdt },
    { "log_string", CdoCmdLogString },
    { "log_address", CdoCmdLogAddress },
    { "marker", CdoCmdMarker },
    { "proc", CdoCmdProc },
    { "begin", CdoCmdBegin },
    { "end", CdoCmdEnd },
    { "break", CdoCmdBreak },
    { "npi_seq", CdoCmdNpiSeq },
    { "npi_precfg", CdoCmdNpiPreCfg },
    { "npi_write", CdoCmdNpiWrite },
    { "npi_shutdown", CdoCmdNpiShutdown },
    { "pm_get_api_version", CdoCmdPmGetApiVersion },
    { "pm_get_device_status", CdoCmdPmGetDeviceStatus },
    { "pm_register_notifier", CdoCmdPmRegisterNotifier },
    { "pm_request_suspend", CdoCmdPmRequestSuspend },
    { "pm_self_suspend", CdoCmdPmSelfSuspend },
    { "pm_force_powerdown", CdoCmdPmForcePowerdown },
    { "pm_abort_suspend", CdoCmdPmAbortSuspend },
    { "pm_request_wakeup", CdoCmdPmRequestWakeup },
    { "pm_set_wakeup_source", CdoCmdPmSetWakeupSource },
    { "pm_system_shutdown", CdoCmdPmSystemShutdown },
    { "pm_request_device", CdoCmdPmRequestDevice },
    { "pm_release_device", CdoCmdPmReleaseDevice },
    { "pm_set_requirement", CdoCmdPmSetRequirement },
    { "pm_set_max_latency", CdoCmdPmSetMaxLatency },
    { "pm_reset_assert", CdoCmdPmResetAssert },
    { "pm_reset_get_status", CdoCmdPmResetGetStatus },
    { "pm_pinctrl_request", CdoCmdPmPinctrlRequest },
    { "pm_pinctrl_release", CdoCmdPmPinctrlRelease },
    { "pm_pinctrl_get_function", CdoCmdPmPinctrlGetFunction },
    { "pm_pinctrl_set_function", CdoCmdPmPinctrlSetFunction },
    { "pm_pinctrl_config_param_get", CdoCmdPmPinctrlConfigParamGet },
    { "pm_pinctrl_config_param_set", CdoCmdPmPinctrlConfigParamSet },
    { "pm_ioctl", CdoCmdPmIoctl },
    { "pm_query_data", CdoCmdPmQueryData },
    { "pm_clock_enable", CdoCmdPmClockEnable },
    { "pm_clock_disable", CdoCmdPmClockDisable },
    { "pm_clock_get_state", CdoCmdPmClockGetState },
    { "pm_clock_set_divider", CdoCmdPmClockSetDivider },
    { "pm_clock_get_divider", CdoCmdPmClockGetDivider },
    { "pm_clock_set_rate", CdoCmdPmClockSetRate },
    { "pm_clock_get_rate", CdoCmdPmClockGetRate },
    { "pm_clock_set_parent", CdoCmdPmClockSetParent },
    { "pm_clock_get_parent", CdoCmdPmClockGetParent },
    { "pm_pll_set_parameter", CdoCmdPmPllSetParameter },
    { "pm_pll_get_parameter", CdoCmdPmPllGetParameter },
    { "pm_pll_set_mode", CdoCmdPmPllSetMode },
    { "pm_pll_get_mode", CdoCmdPmPllGetMode },
    { "pm_add_subsystem", CdoCmdPmAddSubsystem },
    { "pm_destroy_subsystem", CdoCmdPmDestroySubsystem },
    { "pm_describe_nodes", CdoCmdPmDescribeNodes },
    { "pm_add_node", CdoCmdPmAddNode },
    { "pm_add_node_parent", CdoCmdPmAddNodeParent },
    { "pm_add_node_name", CdoCmdPmAddNodeName },
    { "pm_add_requirement", CdoCmdPmAddRequirement },
    { "pm_set_current_subsystem", CdoCmdPmSetCurrentSubsystem },
    { "pm_init_node", CdoCmdPmInitNode },
    { "pm_feature_check", CdoCmdPmFeatureCheck },
    { "pm_iso_control", CdoCmdPmIsoControl },
    { "cfu_set_crc32", CdoCmdCfuSetCrc32 },
    { "cfu_decompress", CdoCmdCfuDecompress },
    { "cfu_cram_rw", CdoCmdCfuCramRW },
    { "cfu_seu_go", CdoCmdCfuSeuGo },
    { "cfu_crc8_dis", CdoCmdCfuCrc8Dis },
    { "cfu_ssi_per_slr_pr", CdoCmdCfuSsiPerSlrPr },
    { "cfu_gsr_gsc", CdoCmdCfuGsrGsc },
    { "cfu_gcap_clk_en", CdoCmdCfuGcapClkEn },
    { "cfu_cfi_type", CdoCmdCfuCfiType },
    { "em_set_action", CdoCmdEmSetAction },
    { "ldr_set_image_info", CdoCmdLdrSetImageInfo },
    { NULL, 0 }
};

static uint32_t iseol(char ** sp) {
    char * s = *sp;
    skipsp(s);
    *sp = s;
    return !istok(*s) || *s == '#';
}

static command_info * find_command(char * name, uint32_t len) {
    command_info * cmd = command_list;
    while (cmd->name != NULL) {
        if (strncasecmp(cmd->name, name, len) == 0 &&
            cmd->name[len] == '\0') return cmd;
        cmd++;
    }
    return NULL;
}

static uint32_t parse_u32(char ** sp, uint32_t * valuep) {
    char * s = *sp;
    skipsp(s);
    errno = 0;
    *valuep = (uint32_t)strtoull(s, sp, 0);
    if (s == *sp || errno != 0 || istok(**sp)) return 1;
    return 0;
};

static uint32_t parse_u64(char ** sp, uint64_t * valuep) {
    char * s = *sp;
    skipsp(s);
    errno = 0;
    *valuep = (uint64_t)strtoull(s, sp, 0);
    if (s == *sp || errno != 0 || istok(**sp)) return 1;
    return 0;
};

static uint32_t parse_buf(char ** sp, uint32_t ** bufp, uint32_t * countp) {
    char * s = *sp;
    uint32_t * buf = NULL;
    uint32_t count = 0;
    uint32_t capacity = 0;

    for (;;) {
        uint32_t value;
        if (iseol(&s)) break;
        if (parse_u32(&s, &value)) goto error;
        if (count == capacity) {
            capacity = capacity ? capacity*2 : 1;
            buf = (uint32_t *)realloc(buf, capacity * sizeof *buf);
            if (buf == NULL) goto error;
        }
        buf[count++] = value;
    }
    *sp = s;
    *bufp = buf;
    *countp = count;
    return 0;

error:
    free(buf);
    *bufp = NULL;
    *countp = 0;
    return 1;
}

static uint32_t parse_string(char ** sp, char ** strp) {
    char * s = *sp;
    char * str = NULL;
    uint32_t count = 0;
    uint32_t capacity = 0;
    int quote = 0;
    int c;

    skipsp(s);
    if (*s == '"') {
        quote = 1;
        s++;
    }
    while ((c = *s) != '\0') {
        if (quote && c == '"') break;
        s++;
        if (c == '\\') {
            if ((c = *s) == '\0') break;
            s++;
        }
        if (count == capacity) {
            capacity = capacity ? capacity*2 : 1;
            str = (char *)realloc(str, capacity * sizeof *str);
            if (str == NULL) goto error;
        }
        str[count++] = c;
    }
    if (quote) {
        if (c != '"') goto error;
        s++;
    }
    if (count == capacity) {
        capacity++;
        str = (char *)realloc(str, capacity * sizeof *str);
        if (str == NULL) goto error;
    }
    str[count] = '\0';
    *sp = s;
    *strp = str;
    return 0;

error:
    free(str);
    *strp = NULL;
    return 1;
}

CdoSequence * cdoseq_from_source(FILE * f) {
    CdoSequence * seq = cdocmd_create_sequence();
    uint32_t cap = 2;
    char * line = (char *)malloc(cap * sizeof *line);
    uint32_t first = 1;
    uint32_t level = 0;
    for (;;) {
        char * s;
        char * p;
        uint32_t len = 0;
        command_info * cmdinfo;
        for (;;) {
            if (len + 1 >= cap) {
                cap *= 2;
                line = (char *)realloc(line, cap * sizeof *line);
            }
            if (fgets(line + len, cap - len, f) == NULL) break;
            len += strlen(line + len);
            if (len == 0 || line[len-1] == '\n') break;
        }
        if (len == 0) break;
        if (line[len-1] == '\n') len--;
        if (len > 0 && line[len-1] == '\r') len--;
        line[len] = '\0';
        s = line;
        if (iseol(&s)) {
            if (*s == '#') {
                s++;
                skipsp(s);
                cdocmd_add_comment(seq, s);
            }
            continue;
        }
        p = s;
        skiptok(s);
        cmdinfo = find_command(p, s - p);
        if (cmdinfo == NULL) {
            if (s - p == 7 &&
                strncasecmp(p, "version", s - p) == 0) {
                unsigned long major;
                unsigned long minor = 0;
                skipsp(s);
                errno = 0;
                major = strtoul(s, &p, 10);
                if (s != p && errno == 0 && *p == '.') {
                    s = p + 1;
                    minor = strtoul(s, &p, 10);
                }
                if (s == p || errno != 0) goto syntax_error;
                if (!iseol(&p)) goto syntax_error;
                if (!first) goto syntax_error;
                if (major > 0xff) goto syntax_error;
                if (minor > 0xff) goto syntax_error;
                seq->version = (major << 8) | minor;
            } else {
                goto syntax_error;
            }
            first = 0;
            continue;
        }
        switch(cmdinfo->id) {
        case CdoCmdSection: {
            uint32_t id;
            if (seq->version >= 0x200) goto syntax_error;
            if (parse_u32(&s, &id)) goto syntax_error;
            cdocmd_add_section(seq, id);
            break;
        }
        case CdoCmdGeneric: {
            uint32_t id;
            uint32_t * buf;
            uint32_t count;
            if (parse_u32(&s, &id)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_generic_command(seq, id, buf, count, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdInclude: {
            CdoSequence * seq2;
            char * name;
            if (iseol(&s)) goto syntax_error;
            if (parse_string(&s, &name)) goto syntax_error;
            seq2 = cdoseq_load_cdo(name);
            free(name);
            if (seq2 == NULL) goto error;
            cdocmd_concat_seq(seq, seq2);
            cdocmd_delete_sequence(seq2);
            break;
        }
#if 0
        case CdoCmdSetBaseAddress:
            cdocmd_(seq, u32be(p[i+0]), u32be(p[i+1]), u32be(p[i+2]), 0, u32be(p[i+3])*1000);

            fprintf(f, "set_base_address ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, "\n");
            break;
#endif
        case CdoCmdWrite: {
            uint64_t addr;
            uint32_t * buf;
            uint32_t count;
            if (parse_u64(&s, &addr)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_block_write(seq, addr, count, buf, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdSetBlock: {
            uint64_t addr;
            uint32_t count;
            uint32_t value;
            if (parse_u64(&s, &addr)) goto syntax_error;
            if (parse_u32(&s, &count)) goto syntax_error;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_block_set(seq, addr, count, value);
            break;
        }
        case CdoCmdWriteKeyhole: {
            uint64_t addr;
            uint32_t value;
            uint32_t * buf;
            uint32_t count;
            if (parse_u64(&s, &addr)) goto syntax_error;
            if (parse_u32(&s, &value)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_keyhole_write(seq, addr, value, count, buf, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdMaskWrite: {
            uint64_t addr;
            uint32_t mask;
            uint32_t value;
            if (parse_u64(&s, &addr)) goto syntax_error;
            if (parse_u32(&s, &mask)) goto syntax_error;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_mask_write(seq, addr, mask, value);
            break;
        }
        case CdoCmdMaskPoll: {
            uint64_t addr;
            uint32_t mask;
            uint32_t value;
            uint32_t count = 0;
            uint32_t flags = 0;
            if (parse_u64(&s, &addr)) goto syntax_error;
            if (parse_u32(&s, &mask)) goto syntax_error;
            if (parse_u32(&s, &value)) goto syntax_error;
            skipsp(s);
            if (istok(*s) && parse_u32(&s, &count)) goto syntax_error;
            skipsp(s);
            if (istok(*s) && parse_u32(&s, &flags)) goto syntax_error;
            cdocmd_add_mask_poll(seq, addr, mask, value, count, flags);
            break;
        }
        case CdoCmdDelay: {
            uint32_t value;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_delay(seq, value);
            break;
        }
        case CdoCmdDmaXfer: {
            uint64_t srcaddr;
            uint64_t dstaddr;
            uint32_t count;
            uint32_t flags;
            uint32_t * buf = NULL;
            if (parse_u64(&s, &srcaddr)) goto syntax_error;
            if (parse_u64(&s, &dstaddr)) goto syntax_error;
            if (parse_u32(&s, &count)) goto syntax_error;
            if (parse_u32(&s, &flags)) goto syntax_error;
            if (seq->version < 0x200 && srcaddr == 0) {
                uint32_t count2 = 0;
                if (parse_buf(&s, &buf, &count2)) goto syntax_error;
                if (count != count2) {
                    free(buf);
                    goto syntax_error;
                }
            }
            cdocmd_add_dma_xfer(seq, srcaddr, dstaddr, count, flags, buf, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdCframeRead: {
            uint32_t param;
            uint64_t addr;
            uint32_t read_count;
            uint32_t * buf;
            uint32_t count;
            if (parse_u32(&s, &param)) goto syntax_error;
            if (parse_u64(&s, &addr)) goto syntax_error;
            if (parse_u32(&s, &read_count)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_cframe_read(seq, param, addr, read_count, count, buf, is_be_host());
            free(buf);
            if (seq->version < 0x200 && count != 16) goto syntax_error;
            break;
        }
        case CdoCmdSsitSyncMaster: {
            cdocmd_add_ssit_sync_master(seq);
            break;
        }
        case CdoCmdSsitSyncSlaves: {
            uint32_t mask;
            uint32_t count;
            if (parse_u32(&s, &mask)) goto syntax_error;
            if (parse_u32(&s, &count)) goto syntax_error;
            cdocmd_add_ssit_sync_slaves(seq, mask, count);
            break;
        }
        case CdoCmdSsitWaitSlaves: {
            uint32_t mask;
            uint32_t count;
            if (parse_u32(&s, &mask)) goto syntax_error;
            if (parse_u32(&s, &count)) goto syntax_error;
            cdocmd_add_ssit_wait_slaves(seq, mask, count);
            break;
        }
        case CdoCmdNop: {
            uint32_t * buf;
            uint32_t count;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_nop(seq, count, buf, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdEventLogging: {
            uint32_t * buf;
            uint32_t count;
            uint32_t subcmd;
            if (parse_u32(&s, &subcmd)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_event_logging(seq, subcmd, count, buf, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdSetBoard: {
            char * name;
            if (iseol(&s)) goto syntax_error;
            if (parse_string(&s, &name)) goto syntax_error;
            cdocmd_add_set_board(seq, name);
            free(name);
            break;
        }
        case CdoCmdSetPlmWdt: {
            uint32_t nodeid;
            uint32_t periodicity;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &periodicity)) goto syntax_error;
            cdocmd_add_set_plm_wdt(seq, nodeid, periodicity);
            break;
        }
        case CdoCmdLogString: {
            char * name;
            if (iseol(&s)) goto syntax_error;
            if (parse_string(&s, &name)) goto syntax_error;
            cdocmd_add_log_string(seq, name);
            free(name);
            break;
        }
        case CdoCmdLogAddress: {
            uint64_t addr;
            if (parse_u64(&s, &addr)) goto syntax_error;
            cdocmd_add_log_address(seq, addr);
            break;
        }
        case CdoCmdMarker: {
            uint32_t value;
            char * name;
            if (parse_u32(&s, &value)) goto syntax_error;
            if (iseol(&s)) goto syntax_error;
            if (parse_string(&s, &name)) goto syntax_error;
            cdocmd_add_marker(seq, value, name);
            free(name);
            break;
        }
        case CdoCmdProc: {
            uint32_t value;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_proc(seq, value);
            level++;
            break;
        }
        case CdoCmdBegin: {
            char * name;
            if (parse_string(&s, &name)) goto syntax_error;
            cdocmd_add_begin(seq, name);
            free(name);
            level++;
            break;
        }
        case CdoCmdEnd: {
            if (level == 0) goto syntax_error;
            cdocmd_add_end(seq);
            level--;
            break;
        }
        case CdoCmdBreak: {
            uint32_t value = 1;
            skipsp(s);
            if (istok(*s)) {
                if (parse_u32(&s, &value)) goto syntax_error;
                if (value < 1 || value > level) goto syntax_error;
            }
            cdocmd_add_break(seq, value);
            break;
        }
        case CdoCmdNpiSeq:
        case CdoCmdNpiPreCfg:
        case CdoCmdNpiShutdown: {
            uint32_t addr;
            uint32_t flags;
            if (parse_u32(&s, &addr)) goto syntax_error;
            if (parse_u32(&s, &flags)) goto syntax_error;
            if (cmdinfo->id == CdoCmdNpiSeq) {
                cdocmd_add_npi_seq(seq, addr, flags);
            } else if (cmdinfo->id == CdoCmdNpiPreCfg) {
                cdocmd_add_npi_precfg(seq, addr, flags);
            } else {
                cdocmd_add_npi_shutdown(seq, addr, flags);
            }
            break;
        }
        case CdoCmdNpiWrite: {
            uint32_t addr;
            uint32_t flags;
            uint32_t count;
            uint32_t * buf;
            if (parse_u32(&s, &addr)) goto syntax_error;
            if (parse_u32(&s, &flags)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_npi_write(seq, addr, flags, count, buf, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdPmGetApiVersion: {
            cdocmd_add_pm_get_api_version(seq);
            break;
        }
        case CdoCmdPmGetDeviceStatus: {
            uint32_t nodeid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            cdocmd_add_pm_get_device_status(seq, nodeid);
            break;
        }
        case CdoCmdPmRegisterNotifier: {
            uint32_t nodeid;
            uint32_t mask;
            uint32_t arg1;
            uint32_t arg2;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &mask)) goto syntax_error;
            if (parse_u32(&s, &arg1)) goto syntax_error;
            if (parse_u32(&s, &arg2)) goto syntax_error;
            cdocmd_add_pm_register_notifier(seq, nodeid, mask, arg1, arg2);
            break;
        }
        case CdoCmdPmRequestSuspend: {
            uint32_t nodeid;
            uint32_t ack;
            uint32_t latency;
            uint32_t state;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &ack)) goto syntax_error;
            if (parse_u32(&s, &latency)) goto syntax_error;
            if (parse_u32(&s, &state)) goto syntax_error;
            cdocmd_add_pm_request_suspend(seq, nodeid, ack, latency, state);
            break;
        }
        case CdoCmdPmSelfSuspend: {
            uint32_t nodeid;
            uint32_t latency;
            uint32_t state;
            uint64_t addr;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &latency)) goto syntax_error;
            if (parse_u32(&s, &state)) goto syntax_error;
            if (parse_u64(&s, &addr)) goto syntax_error;
            cdocmd_add_pm_self_suspend(seq, nodeid, latency, state, addr);
            break;
        }
        case CdoCmdPmForcePowerdown: {
            uint32_t nodeid;
            uint32_t ack;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &ack)) goto syntax_error;
            cdocmd_add_pm_force_powerdown(seq, nodeid, ack);
            break;
        }
        case CdoCmdPmAbortSuspend: {
            uint32_t nodeid;
            uint32_t reason;
            if (parse_u32(&s, &reason)) goto syntax_error;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            cdocmd_add_pm_abort_suspend(seq, reason, nodeid);
            break;
        }
        case CdoCmdPmRequestWakeup: {
            uint32_t nodeid;
            uint64_t addr;
            uint32_t ack;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u64(&s, &addr)) goto syntax_error;
            if (parse_u32(&s, &ack)) goto syntax_error;
            cdocmd_add_pm_request_wakeup(seq, nodeid, addr, ack);
            break;
        }
        case CdoCmdPmSetWakeupSource: {
            uint32_t target;
            uint32_t source;
            uint32_t enable;
            if (parse_u32(&s, &target)) goto syntax_error;
            if (parse_u32(&s, &source)) goto syntax_error;
            if (parse_u32(&s, &enable)) goto syntax_error;
            cdocmd_add_pm_set_wakeup_source(seq, target, source, enable);
            break;
        }
        case CdoCmdPmSystemShutdown: {
            uint32_t type;
            uint32_t subtype;
            if (parse_u32(&s, &type)) goto syntax_error;
            if (parse_u32(&s, &subtype)) goto syntax_error;
            cdocmd_add_pm_system_shutdown(seq, type, subtype);
            break;
        }
        case CdoCmdPmRequestDevice: {
            uint32_t nodeid;
            uint32_t capabilities;
            uint32_t qos;
            uint32_t ack;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &capabilities)) goto syntax_error;
            if (parse_u32(&s, &qos)) goto syntax_error;
            if (parse_u32(&s, &ack)) goto syntax_error;
            cdocmd_add_pm_request_device(seq, nodeid, capabilities, qos, ack);
            break;
        }
        case CdoCmdPmReleaseDevice: {
            uint32_t nodeid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            cdocmd_add_pm_release_device(seq, nodeid);
            break;
        }
        case CdoCmdPmSetRequirement: {
            uint32_t nodeid;
            uint32_t capabilities;
            uint32_t qos;
            uint32_t ack;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &capabilities)) goto syntax_error;
            if (parse_u32(&s, &qos)) goto syntax_error;
            if (parse_u32(&s, &ack)) goto syntax_error;
            cdocmd_add_pm_set_requirement(seq, nodeid, capabilities, qos, ack);
            break;
        }
        case CdoCmdPmSetMaxLatency: {
            uint32_t nodeid;
            uint32_t latency;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &latency)) goto syntax_error;
            cdocmd_add_pm_set_max_latency(seq, nodeid, latency);
            break;
        }
        case CdoCmdPmResetAssert: {
            uint32_t nodeid;
            uint32_t action;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &action)) goto syntax_error;
            cdocmd_add_pm_reset_assert(seq, nodeid, action);
            break;
        }
        case CdoCmdPmResetGetStatus: {
            uint32_t nodeid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            cdocmd_add_pm_reset_get_status(seq, nodeid);
            break;
        }
        case CdoCmdPmPinctrlRequest: {
            uint32_t pinid;
            if (parse_u32(&s, &pinid)) goto syntax_error;
            cdocmd_add_pm_pinctrl_request(seq, pinid);
            break;
        }
        case CdoCmdPmPinctrlRelease: {
            uint32_t pinid;
            if (parse_u32(&s, &pinid)) goto syntax_error;
            cdocmd_add_pm_pinctrl_release(seq, pinid);
            break;
        }
        case CdoCmdPmPinctrlGetFunction: {
            uint32_t pinid;
            if (parse_u32(&s, &pinid)) goto syntax_error;
            cdocmd_add_pm_pinctrl_get_function(seq, pinid);
            break;
        }
        case CdoCmdPmPinctrlSetFunction: {
            uint32_t pinid;
            uint32_t funcid;
            if (parse_u32(&s, &pinid)) goto syntax_error;
            if (parse_u32(&s, &funcid)) goto syntax_error;
            cdocmd_add_pm_pinctrl_set_function(seq, pinid, funcid);
            break;
        }
        case CdoCmdPmPinctrlConfigParamGet: {
            uint32_t pinid;
            uint32_t param;
            if (parse_u32(&s, &pinid)) goto syntax_error;
            if (parse_u32(&s, &param)) goto syntax_error;
            cdocmd_add_pm_pinctrl_config_param_get(seq, pinid, param);
            break;
        }
        case CdoCmdPmPinctrlConfigParamSet: {
            uint32_t pinid;
            uint32_t param;
            uint32_t value;
            if (parse_u32(&s, &pinid)) goto syntax_error;
            if (parse_u32(&s, &param)) goto syntax_error;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_pm_pinctrl_config_param_set(seq, pinid, param, value);
            break;
        }
        case CdoCmdPmIoctl: {
            uint32_t nodeid;
            uint32_t ioctlid;
            uint32_t * buf;
            uint32_t count;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &ioctlid)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_pm_ioctl(seq, nodeid, ioctlid, buf, count, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdPmQueryData: {
            uint32_t nodeid;
            uint32_t * buf;
            uint32_t count;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_pm_query_data(seq, nodeid, buf, count, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdPmClockEnable: {
            uint32_t nodeid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            cdocmd_add_pm_clock_enable(seq, nodeid);
            break;
        }
        case CdoCmdPmClockDisable: {
            uint32_t nodeid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            cdocmd_add_pm_clock_disable(seq, nodeid);
            break;
        }
        case CdoCmdPmClockGetState: {
            uint32_t nodeid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            cdocmd_add_pm_clock_getstate(seq, nodeid);
            break;
        }
        case CdoCmdPmClockSetDivider: {
            uint32_t nodeid;
            uint32_t div;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &div)) goto syntax_error;
            cdocmd_add_pm_clock_setdivider(seq, nodeid, div);
            break;
        }
        case CdoCmdPmClockGetDivider: {
            uint32_t nodeid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            cdocmd_add_pm_clock_getdivider(seq, nodeid);
            break;
        }
        case CdoCmdPmClockSetRate: {
            uint32_t nodeid;
            uint32_t rate;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &rate)) goto syntax_error;
            cdocmd_add_pm_clock_setrate(seq, nodeid, rate);
            break;
        }
        case CdoCmdPmClockGetRate: {
            uint32_t nodeid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            cdocmd_add_pm_clock_getrate(seq, nodeid);
            break;
        }
        case CdoCmdPmClockSetParent: {
            uint32_t nodeid;
            uint32_t parentid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &parentid)) goto syntax_error;
            cdocmd_add_pm_clock_setparent(seq, nodeid, parentid);
            break;
        }
        case CdoCmdPmClockGetParent: {
            uint32_t nodeid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            cdocmd_add_pm_clock_getparent(seq, nodeid);
            break;
        }
        case CdoCmdPmPllSetParameter: {
            uint32_t nodeid;
            uint32_t param;
            uint32_t value;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &param)) goto syntax_error;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_pm_pll_set_parameter(seq, nodeid, param, value);
            break;
        }
        case CdoCmdPmPllGetParameter: {
            uint32_t nodeid;
            uint32_t param;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &param)) goto syntax_error;
            cdocmd_add_pm_pll_get_parameter(seq, nodeid, param);
            break;
        }
        case CdoCmdPmPllSetMode: {
            uint32_t nodeid;
            uint32_t value;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_pm_pll_set_mode(seq, nodeid, value);
            break;
        }
        case CdoCmdPmPllGetMode: {
            uint32_t nodeid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            cdocmd_add_pm_pll_get_mode(seq, nodeid);
            break;
        }
        case CdoCmdPmAddSubsystem: {
            uint32_t ssid;
            if (parse_u32(&s, &ssid)) goto syntax_error;
            cdocmd_add_pm_add_subsystem(seq, ssid);
            break;
        }
        case CdoCmdPmDestroySubsystem: {
            uint32_t ssid;
            if (parse_u32(&s, &ssid)) goto syntax_error;
            cdocmd_add_pm_destroy_subsystem(seq, ssid);
            break;
        }
        case CdoCmdPmDescribeNodes: {
            uint32_t count;
            uint32_t * buf;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_pm_describe_nodes(seq, buf, count, is_be_host());
            break;
        }
        case CdoCmdPmAddNode: {
            uint32_t id;
            uint32_t count;
            uint32_t * buf;
            if (parse_u32(&s, &id)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_pm_add_node(seq, id, count, buf, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdPmAddNodeParent: {
            uint32_t id;
            uint32_t count;
            uint32_t * buf;
            if (parse_u32(&s, &id)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            if (count < 1) goto syntax_error;
            cdocmd_add_pm_add_node_parent(seq, id, count, buf, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdPmAddNodeName: {
            uint32_t id;
            char * name;
            if (parse_u32(&s, &id)) goto syntax_error;
            if (iseol(&s)) goto syntax_error;
            if (parse_string(&s, &name)) goto syntax_error;
            cdocmd_add_pm_add_node_name(seq, id, name);
            free(name);
            break;
        }
        case CdoCmdPmAddRequirement: {
            uint32_t ssid;
            uint32_t nodeid;
            uint32_t flags;
            uint32_t count;
            uint32_t * buf;
            if (parse_u32(&s, &ssid)) goto syntax_error;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &flags)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_pm_add_requirement(seq, ssid, nodeid, flags, count, buf, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdPmSetCurrentSubsystem: {
            uint32_t ssid;
            if (parse_u32(&s, &ssid)) goto syntax_error;
            cdocmd_add_pm_set_current_subsystem(seq, ssid);
            break;
        }
        case CdoCmdPmInitNode: {
            uint32_t id;
            uint32_t function;
            uint32_t count;
            uint32_t * buf;
            if (parse_u32(&s, &id)) goto syntax_error;
            if (parse_u32(&s, &function)) goto syntax_error;
            if (parse_buf(&s, &buf, &count)) goto syntax_error;
            cdocmd_add_pm_init_node(seq, id, function, count, buf, is_be_host());
            free(buf);
            break;
        }
        case CdoCmdPmFeatureCheck: {
            uint32_t id;
            if (parse_u32(&s, &id)) goto syntax_error;
            cdocmd_add_pm_feature_check(seq, id);
            break;
        }
        case CdoCmdPmIsoControl: {
            uint32_t nodeid;
            uint32_t value;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_pm_iso_control(seq, nodeid, value);
            break;
        }
        case CdoCmdCfuSetCrc32: {
            uint32_t flags;
            uint32_t value;
            if (parse_u32(&s, &flags)) goto syntax_error;
            if (flags != 0) {
                if (parse_u32(&s, &value)) goto syntax_error;
            } else {
                value = 0;
            }
            cdocmd_add_cfu_set_crc32(seq, flags, value);
            break;
        }
        case CdoCmdCfuDecompress: {
            uint32_t flags;
            if (parse_u32(&s, &flags)) goto syntax_error;
            cdocmd_add_cfu_decompress(seq, flags);
            break;
        }
        case CdoCmdCfuCramRW: {
            uint32_t value;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_cfu_cram_rw(seq, value);
            break;
        }
        case CdoCmdCfuSeuGo: {
            uint32_t value;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_cfu_seu_go(seq, value);
            break;
        }
        case CdoCmdCfuCrc8Dis: {
            uint32_t flags;
            if (parse_u32(&s, &flags)) goto syntax_error;
            cdocmd_add_cfu_crc8_dis(seq, flags);
            break;
        }
        case CdoCmdCfuSsiPerSlrPr: {
            uint32_t value;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_cfu_ssi_per_slr_pr(seq, value);
            break;
        }
        case CdoCmdCfuGsrGsc: {
            uint32_t value;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_cfu_gsr_gsc(seq, value);
            break;
        }
        case CdoCmdCfuGcapClkEn: {
            uint32_t value;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_cfu_gcap_clk_en(seq, value);
            break;
        }
        case CdoCmdCfuCfiType: {
            uint32_t value;
            if (parse_u32(&s, &value)) goto syntax_error;
            cdocmd_add_cfu_cfi_type(seq, value);
            break;
        }
        case CdoCmdEmSetAction: {
            uint32_t nodeid;
            uint32_t action;
            uint32_t mask;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &action)) goto syntax_error;
            if (parse_u32(&s, &mask)) goto syntax_error;
            cdocmd_add_em_set_action(seq, nodeid, action, mask);
            break;
        }
        case CdoCmdLdrSetImageInfo: {
            uint32_t nodeid;
            uint32_t uid;
            uint32_t puid;
            uint32_t funcid;
            if (parse_u32(&s, &nodeid)) goto syntax_error;
            if (parse_u32(&s, &uid)) goto syntax_error;
            if (parse_u32(&s, &puid)) goto syntax_error;
            if (parse_u32(&s, &funcid)) goto syntax_error;
            cdocmd_add_ldr_set_image_info(seq, nodeid, uid, puid, funcid);
            break;
        }
        default:
            goto syntax_error;
        }
        if (!iseol(&s)) goto syntax_error;
        first = 0;
    }
    if (level > 0) {
        fprintf(stderr, "missing one or more end statements\n");
        goto error;
    }
    free(line);
    return seq;

syntax_error:
    fprintf(stderr, "syntax error: %s\n", line);
error:
    cdocmd_delete_sequence(seq);
    free(line);
    return NULL;
}

static void print_x64(FILE * f, uint64_t v) {
    if (v == 0) {
        fprintf(f, "0");
    } else {
        fprintf(f, "%#"PRIx64, v);
    }
}

static void print_buf(FILE * f, void * buf, uint32_t count) {
    uint32_t * p = (uint32_t *)buf;
    while (count > 0) {
        uint32_t v = *p++;
        fprintf(f, " ");
        print_x64(f, v);
        count--;
    }
}

static void print_string(FILE * f, char * str) {
    char * s = str;
    fputc('"', f);
    for (;;) {
        int c = *s++;
        if (c == '\0') break;
        if (c == '\\' || c == '"') {
            fputc('\\', f);
        }
        fputc(c, f);
    }
    fputc('"', f);
}

void cdoseq_to_source(FILE * f, CdoSequence * seq) {
    LINK * l = seq->cmds.next;
    uint32_t level = 0;
    fprintf(f, "version %"PRIu32".%"PRIu32"\n",
            seq->version >> 8, seq->version & (uint32_t)0xff);
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if (level > 0 && cmd->type == CdoCmdEnd) level--;
        fprintf(f, "%*s", level*2, "");
        switch (cmd->type) {
        case CdoCmdSection:
            fprintf(f, "section ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdGeneric:
            fprintf(f, "generic ");
            print_x64(f, cmd->id);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdComment:
            fprintf(f, "# %s\n", (char *)cmd->buf);
            break;
        case CdoCmdInclude: {
            fprintf(f, "include ");
            print_string(f, (char *)cmd->buf);
            fprintf(f, "\n");
            break;
        }
        case CdoCmdSetBaseAddress:
            fprintf(f, "set_base_address ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, "\n");
            break;
        case CdoCmdWrite:
            fprintf(f, "write ");
            print_x64(f, cmd->dstaddr);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdSetBlock:
            fprintf(f, "set ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " %"PRIu32" ", cmd->count);
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdWriteKeyhole:
            fprintf(f, "write_keyhole ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdMaskWrite:
            fprintf(f, "mask_write ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->mask);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdMaskPoll:
            fprintf(f, "mask_poll ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->mask);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, " ");
            print_x64(f, cmd->count);
            if (cmd->flags != 0) {
                fprintf(f, " ");
                print_x64(f, cmd->flags);
            }
            fprintf(f, "\n");
            break;
        case CdoCmdDelay:
            fprintf(f, "delay %"PRIu32"\n", cmd->value);
            break;
        case CdoCmdDmaXfer:
            fprintf(f, "dma_xfer ");
            print_x64(f, cmd->srcaddr);
            fprintf(f, " ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->count);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            if (seq->version < 0x200 && cmd->buf) {
                print_buf(f, cmd->buf, cmd->count);
            }
            fprintf(f, "\n");
            break;
        case CdoCmdCframeRead:
            fprintf(f, "cframe_read ");
            print_x64(f, cmd->flags);
            fprintf(f, " ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdSsitSyncMaster:
            fprintf(f, "ssit_sync_master");
            fprintf(f, "\n");
            break;
        case CdoCmdSsitSyncSlaves:
            fprintf(f, "ssit_sync_slaves ");
            print_x64(f, cmd->mask);
            fprintf(f, " ");
            print_x64(f, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdSsitWaitSlaves:
            fprintf(f, "ssit_wait_slaves ");
            print_x64(f, cmd->mask);
            fprintf(f, " ");
            print_x64(f, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdNop:
            fprintf(f, "nop");
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdEventLogging:
            fprintf(f, "event_logging ");
            print_x64(f, cmd->value);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdSetBoard:
            fprintf(f, "set_board ");
            print_string(f, (char *)cmd->buf);
            fprintf(f, "\n");
            break;
        case CdoCmdSetPlmWdt:
            fprintf(f, "set_plm_wdt ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdLogString:
            fprintf(f, "log_string ");
            print_string(f, (char *)cmd->buf);
            fprintf(f, "\n");
            break;
        case CdoCmdLogAddress:
            fprintf(f, "log_address ");
            print_x64(f, cmd->srcaddr);
            fprintf(f, "\n");
            break;
        case CdoCmdMarker:
            fprintf(f, "marker ");
            print_x64(f, cmd->value);
            fprintf(f, " ");
            print_string(f, (char *)cmd->buf);
            fprintf(f, "\n");
            break;
        case CdoCmdProc:
            fprintf(f, "proc ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            level++;
            break;
        case CdoCmdBegin:
            fprintf(f, "begin");
            if (strlen(cmd->buf) != 0) {
                fprintf(f, " ");
                print_string(f, (char *)cmd->buf);
            }
            fprintf(f, "\n");
            level++;
            break;
        case CdoCmdEnd:
            fprintf(f, "end\n");
            break;
        case CdoCmdBreak:
            fprintf(f, "break");
            if (cmd->value != 1) {
                fprintf(f, " ");
                print_x64(f, cmd->value);
            }
            fprintf(f, "\n");
            break;
        case CdoCmdNpiSeq:
            fprintf(f, "npi_seq ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdNpiPreCfg:
            fprintf(f, "npi_precfg ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdNpiWrite:
            fprintf(f, "npi_write ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdNpiShutdown:
            fprintf(f, "npi_shutdown ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdPmGetApiVersion:
            fprintf(f, "pm_get_api_version");
            fprintf(f, "\n");
            break;
        case CdoCmdPmGetDeviceStatus:
            fprintf(f, "pm_get_device_status ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmRegisterNotifier:
            fprintf(f, "pm_register_notifier ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->mask);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, " ");
            print_x64(f, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdPmRequestSuspend:
            fprintf(f, "pm_request_suspend ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, " ");
            print_x64(f, cmd->count);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdPmSelfSuspend:
            fprintf(f, "pm_self_suspend ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->count);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, " ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, "\n");
            break;
        case CdoCmdPmForcePowerdown:
            fprintf(f, "pm_force_powerdown ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdPmAbortSuspend:
            fprintf(f, "pm_abort_suspend ");
            print_x64(f, cmd->value);
            fprintf(f, " ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmRequestWakeup:
            fprintf(f, "pm_request_wakeup ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdPmSetWakeupSource:
            fprintf(f, "pm_set_wakeup_source ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdPmSystemShutdown:
            fprintf(f, "pm_system_shutdown ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdPmRequestDevice:
            fprintf(f, "pm_request_device ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, " ");
            print_x64(f, cmd->count);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdPmReleaseDevice:
            fprintf(f, "pm_release_device ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmSetRequirement:
            fprintf(f, "pm_set_requirement ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, " ");
            print_x64(f, cmd->count);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdPmSetMaxLatency:
            fprintf(f, "pm_set_max_latency ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdPmResetAssert:
            fprintf(f, "pm_reset_assert ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdPmResetGetStatus:
            fprintf(f, "pm_reset_get_status ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmPinctrlRequest:
            fprintf(f, "pm_pinctrl_request ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmPinctrlRelease:
            fprintf(f, "pm_pinctrl_release ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmPinctrlGetFunction:
            fprintf(f, "pm_pinctrl_get_function ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmPinctrlSetFunction:
            fprintf(f, "pm_pinctrl_set_function ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdPmPinctrlConfigParamGet:
            fprintf(f, "pm_pinctrl_config_param_get ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdPmPinctrlConfigParamSet:
            fprintf(f, "pm_pinctrl_config_param_set ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdPmIoctl:
            fprintf(f, "pm_ioctl ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdPmQueryData:
            fprintf(f, "pm_query_data ");
            print_x64(f, cmd->id);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdPmClockEnable:
            fprintf(f, "pm_clock_enable ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmClockDisable:
            fprintf(f, "pm_clock_disable ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmClockGetState:
            fprintf(f, "pm_clock_get_state ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmClockSetDivider:
            fprintf(f, "pm_clock_set_divider ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdPmClockGetDivider:
            fprintf(f, "pm_clock_get_divider ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmClockSetRate:
            fprintf(f, "pm_clock_set_rate ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdPmClockGetRate:
            fprintf(f, "pm_clock_get_rate ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmClockSetParent:
            fprintf(f, "pm_clock_set_parent ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdPmClockGetParent:
            fprintf(f, "pm_clock_get_parent ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmPllSetParameter:
            fprintf(f, "pm_pll_set_parameter ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdPmPllGetParameter:
            fprintf(f, "pm_pll_get_parameter ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdPmPllSetMode:
            fprintf(f, "pm_pll_set_mode ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdPmPllGetMode:
            fprintf(f, "pm_pll_get_mode ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmAddSubsystem:
            fprintf(f, "pm_add_subsystem ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmDestroySubsystem:
            fprintf(f, "pm_destroy_subsystem ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmDescribeNodes:
            fprintf(f, "pm_describe_nodes");
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdPmAddNode:
            fprintf(f, "pm_add_node ");
            print_x64(f, cmd->id);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdPmAddNodeParent:
            fprintf(f, "pm_add_node_parent ");
            print_x64(f, cmd->id);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdPmAddNodeName:
            fprintf(f, "pm_add_node_name ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_string(f, (char *)cmd->buf);
            fprintf(f, "\n");
            break;
        case CdoCmdPmAddRequirement:
            fprintf(f, "pm_add_requirement ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, " ");
            print_x64(f, cmd->flags);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdPmSetCurrentSubsystem:
            fprintf(f, "pm_set_current_subsystem ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmInitNode:
            fprintf(f, "pm_init_node ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            print_buf(f, cmd->buf, cmd->count);
            fprintf(f, "\n");
            break;
        case CdoCmdPmFeatureCheck:
            fprintf(f, "pm_feature_check ");
            print_x64(f, cmd->id);
            fprintf(f, "\n");
            break;
        case CdoCmdPmIsoControl:
            fprintf(f, "pm_iso_control ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdCfuSetCrc32:
            fprintf(f, "cfu_set_crc32 ");
            print_x64(f, cmd->flags);
            if (cmd->flags != 0) {
                fprintf(f, " ");
                print_x64(f, cmd->value);
            }
            fprintf(f, "\n");
            break;
        case CdoCmdCfuDecompress:
            fprintf(f, "cfu_decompress ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdCfuCramRW:
            fprintf(f, "cfu_cram_rw ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdCfuSeuGo:
            fprintf(f, "cfu_seu_go ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdCfuCrc8Dis:
            fprintf(f, "cfu_crc8_dis ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdCfuSsiPerSlrPr:
            fprintf(f, "cfu_ssi_per_slr_pr ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdCfuGsrGsc:
            fprintf(f, "cfu_gsr_gsc ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdCfuGcapClkEn:
            fprintf(f, "cfu_gcap_clk_en ");
            print_x64(f, cmd->flags);
            fprintf(f, "\n");
            break;
        case CdoCmdCfuCfiType:
            fprintf(f, "cfu_cfi_type ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
        case CdoCmdEmSetAction:
            fprintf(f, "em_set_action ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, " ");
            print_x64(f, cmd->mask);
            fprintf(f, "\n");
            break;
        case CdoCmdLdrSetImageInfo:
            fprintf(f, "ldr_set_image_info ");
            print_x64(f, cmd->id);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, " ");
            print_x64(f, cmd->mask);
            fprintf(f, " ");
            print_x64(f, cmd->count);
            fprintf(f, "\n");
            break;
        default:
            fprintf(f, "unknown command (%u)\n", cmd->type);
            break;
        }
    }
}
