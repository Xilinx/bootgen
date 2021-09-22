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

#ifndef D_cdo_command
#define D_cdo_command

#include <stdio.h>
#include <stdint.h>
#include "link.h"

#include "dyn_link.h"

#define is_be_host() (0)
#define DEFAULT_CDO_VERSION 0x200

typedef struct CdoSequence CdoSequence;

struct CdoSequence {
    LINK cmds;
    uint32_t version;
};

typedef struct UserKeys {
    uint32_t user_keys_array[8][8];
}UserKeys;

#ifdef __cplusplus
extern "C" {
#endif
    extern UserKeys user_keys;
#ifdef __cplusplus
}
#endif

#define all2cmds(A)  list_item_type((A), CdoCommand, link_all)

typedef struct CdoCommand CdoCommand;

typedef enum CdoCmdType {
    CdoCmdGeneric,
    CdoCmdSection,
    CdoCmdInclude,
    CdoCmdComment,
    CdoCmdSetBaseAddress,
    CdoCmdWrite,
    CdoCmdSetBlock,
    CdoCmdWriteKeyhole,
    CdoCmdMaskWrite,
    CdoCmdMaskPoll,
    CdoCmdDelay,
    CdoCmdDmaXfer,
    CdoCmdCframeRead,
    CdoCmdSsitSyncMaster,
    CdoCmdSsitSyncSlaves,
    CdoCmdSsitWaitSlaves,
    CdoCmdNop,
    CdoCmdEventLogging,
    CdoCmdSetBoard,
    CdoCmdSetPlmWdt,
    CdoCmdLogString,
    CdoCmdLogAddress,
    CdoCmdMarker,
    CdoCmdProc,
    CdoCmdBegin,
    CdoCmdEnd,
    CdoCmdBreak,
    CdoCmdNpiSeq,
    CdoCmdNpiPreCfg,
    CdoCmdNpiWrite,
    CdoCmdNpiShutdown,
    CdoCmdPmGetApiVersion,
    CdoCmdPmGetDeviceStatus,
    CdoCmdPmRegisterNotifier,
    CdoCmdPmRequestSuspend,
    CdoCmdPmSelfSuspend,
    CdoCmdPmForcePowerdown,
    CdoCmdPmAbortSuspend,
    CdoCmdPmRequestWakeup,
    CdoCmdPmSetWakeupSource,
    CdoCmdPmSystemShutdown,
    CdoCmdPmRequestDevice,
    CdoCmdPmReleaseDevice,
    CdoCmdPmSetRequirement,
    CdoCmdPmSetMaxLatency,
    CdoCmdPmResetAssert,
    CdoCmdPmResetGetStatus,
    CdoCmdPmPinctrlRequest,
    CdoCmdPmPinctrlRelease,
    CdoCmdPmPinctrlGetFunction,
    CdoCmdPmPinctrlSetFunction,
    CdoCmdPmPinctrlConfigParamGet,
    CdoCmdPmPinctrlConfigParamSet,
    CdoCmdPmIoctl,
    CdoCmdPmQueryData,
    CdoCmdPmClockEnable,
    CdoCmdPmClockDisable,
    CdoCmdPmClockGetState,
    CdoCmdPmClockSetDivider,
    CdoCmdPmClockGetDivider,
    CdoCmdPmClockSetRate,
    CdoCmdPmClockGetRate,
    CdoCmdPmClockSetParent,
    CdoCmdPmClockGetParent,
    CdoCmdPmPllSetParameter,
    CdoCmdPmPllGetParameter,
    CdoCmdPmPllSetMode,
    CdoCmdPmPllGetMode,
    CdoCmdPmAddSubsystem,
    CdoCmdPmDestroySubsystem,
    CdoCmdPmDescribeNodes,
    CdoCmdPmAddNode,
    CdoCmdPmAddNodeParent,
    CdoCmdPmAddNodeName,
    CdoCmdPmAddRequirement,
    CdoCmdPmSetCurrentSubsystem,
    CdoCmdPmInitNode,
    CdoCmdPmFeatureCheck,
    CdoCmdPmIsoControl,
    CdoCmdCfuSetCrc32,
    CdoCmdCfuDecompress,
    CdoCmdCfuCramRW,
    CdoCmdCfuSeuGo,
    CdoCmdCfuCrc8Dis,
    CdoCmdCfuSsiPerSlrPr,
    CdoCmdCfuGsrGsc,
    CdoCmdCfuGcapClkEn,
    CdoCmdCfuCfiType,
    CdoCmdEmSetAction,
    CdoCmdLdrSetImageInfo,

    /* The following line must be last */
    CdoCmdLast
} CdoCmdType;

struct CdoCommand {
    LINK link_all;
    CdoCmdType type;
    uint32_t id;
    uint64_t dstaddr;
    uint64_t srcaddr;
    uint32_t mask;
    uint32_t value;
    uint32_t count;
    uint32_t period;
    uint32_t flags;
    void * buf;
};

CDOUTIL_EXPORT CdoSequence * cdocmd_create_sequence(void);
CDOUTIL_EXPORT void cdocmd_delete_sequence(CdoSequence * seq);
CDOUTIL_EXPORT CdoCommand * cdocmd_alloc(CdoCmdType type);
CDOUTIL_EXPORT void cdocmd_free(CdoCommand * cmd);
CDOUTIL_EXPORT CdoCommand * cdocmd_duplicate(CdoCommand * cmd);
CDOUTIL_EXPORT void cdocmd_set_default_sequence(CdoSequence * seq);
CDOUTIL_EXPORT void cdocmd_append_command(CdoSequence * seq, CdoCommand * newcmd);
CDOUTIL_EXPORT void cdocmd_insert_command(CdoCommand * cmd, CdoCommand * newcmd);
CDOUTIL_EXPORT void cdocmd_add_random_command(CdoSequence * seq, uint32_t * levelp);
CDOUTIL_EXPORT void cdocmd_add_generic_command(CdoSequence * seq, uint32_t id, void * buf, uint32_t count, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_comment(CdoSequence * seq, const char * comment, ...);
CDOUTIL_EXPORT void cdocmd_add_section(CdoSequence * seq, uint32_t secid);
CDOUTIL_EXPORT void cdocmd_add_include(CdoSequence * seq, const char * name);
CDOUTIL_EXPORT void cdocmd_add_mask_poll(CdoSequence * seq, uint64_t addr, uint32_t mask, uint32_t value, uint32_t count, uint32_t flags);
CDOUTIL_EXPORT void cdocmd_add_mask_write(CdoSequence * seq, uint64_t addr, uint32_t mask, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_write(CdoSequence * seq, uint64_t addr, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_delay(CdoSequence * seq, uint32_t delay);
CDOUTIL_EXPORT void cdocmd_add_block_write(CdoSequence * seq, uint64_t addr, uint32_t count, void * buf, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_keyhole_write(CdoSequence * seq, uint64_t addr, uint32_t keyhole_size, uint32_t count, void * buf, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_block_set(CdoSequence * seq, uint64_t addr, uint32_t count, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_dma_xfer(CdoSequence * seq, uint64_t srcaddr, uint64_t dstaddr, uint32_t count, uint32_t flags, void * buf, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_npi_seq(CdoSequence * seq, uint32_t addr, uint32_t flags);
CDOUTIL_EXPORT void cdocmd_add_npi_precfg(CdoSequence * seq, uint32_t addr, uint32_t flags);
CDOUTIL_EXPORT void cdocmd_add_npi_write(CdoSequence * seq, uint32_t addr, uint32_t flags, uint32_t count, void * buf, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_npi_shutdown(CdoSequence * seq, uint32_t addr, uint32_t flags);

CDOUTIL_EXPORT void cdocmd_add_pm_get_api_version(CdoSequence * seq);
CDOUTIL_EXPORT void cdocmd_add_pm_get_device_status(CdoSequence * seq, uint32_t nodeid);
CDOUTIL_EXPORT void cdocmd_add_pm_register_notifier(CdoSequence * seq, uint32_t nodeid, uint32_t mask, uint32_t arg1, uint32_t arg2);
CDOUTIL_EXPORT void cdocmd_add_pm_request_suspend(CdoSequence * seq, uint32_t nodeid, uint32_t ack, uint32_t latency, uint32_t state);
CDOUTIL_EXPORT void cdocmd_add_pm_self_suspend(CdoSequence * seq, uint32_t nodeid, uint32_t latency, uint32_t state, uint64_t addr);
CDOUTIL_EXPORT void cdocmd_add_pm_force_powerdown(CdoSequence * seq, uint32_t nodeid, uint32_t ack);
CDOUTIL_EXPORT void cdocmd_add_pm_abort_suspend(CdoSequence * seq, uint32_t reason, uint32_t nodeid);
CDOUTIL_EXPORT void cdocmd_add_pm_request_wakeup(CdoSequence * seq, uint32_t nodeid, uint64_t addr, uint32_t ack);
CDOUTIL_EXPORT void cdocmd_add_pm_set_wakeup_source(CdoSequence * seq, uint32_t target, uint32_t source, uint32_t enable);
CDOUTIL_EXPORT void cdocmd_add_pm_system_shutdown(CdoSequence * seq, uint32_t type, uint32_t subtype);
CDOUTIL_EXPORT void cdocmd_add_pm_request_device(CdoSequence * seq, uint32_t nodeid, uint32_t capabilities, uint32_t qos, uint32_t ack);
CDOUTIL_EXPORT void cdocmd_add_pm_release_device(CdoSequence * seq, uint32_t nodeid);
CDOUTIL_EXPORT void cdocmd_add_pm_set_requirement(CdoSequence * seq, uint32_t nodeid, uint32_t capabilities, uint32_t qos, uint32_t ack);
CDOUTIL_EXPORT void cdocmd_add_pm_set_max_latency(CdoSequence * seq, uint32_t nodeid, uint32_t latency);
CDOUTIL_EXPORT void cdocmd_add_pm_reset_assert(CdoSequence * seq, uint32_t nodeid, uint32_t action);
CDOUTIL_EXPORT void cdocmd_add_pm_reset_get_status(CdoSequence * seq, uint32_t nodeid);
CDOUTIL_EXPORT void cdocmd_add_pm_pinctrl_request(CdoSequence * seq, uint32_t pinid);
CDOUTIL_EXPORT void cdocmd_add_pm_pinctrl_release(CdoSequence * seq, uint32_t pinid);
CDOUTIL_EXPORT void cdocmd_add_pm_pinctrl_get_function(CdoSequence * seq, uint32_t pinid);
CDOUTIL_EXPORT void cdocmd_add_pm_pinctrl_set_function(CdoSequence * seq, uint32_t pinid, uint32_t funcid);
CDOUTIL_EXPORT void cdocmd_add_pm_pinctrl_config_param_get(CdoSequence * seq, uint32_t pinid, uint32_t param);
CDOUTIL_EXPORT void cdocmd_add_pm_pinctrl_config_param_set(CdoSequence * seq, uint32_t pinid, uint32_t param, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_pm_ioctl(CdoSequence * seq, uint32_t nodeid, uint32_t ioctlid, void * buf, uint32_t count, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_pm_query_data(CdoSequence * seq, uint32_t id, void * buf, uint32_t count, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_pm_clock_enable(CdoSequence * seq, uint32_t nodeid);
CDOUTIL_EXPORT void cdocmd_add_pm_clock_disable(CdoSequence * seq, uint32_t nodeid);
CDOUTIL_EXPORT void cdocmd_add_pm_clock_getstate(CdoSequence * seq, uint32_t nodeid);
CDOUTIL_EXPORT void cdocmd_add_pm_clock_setdivider(CdoSequence * seq, uint32_t nodeid, uint32_t div);
CDOUTIL_EXPORT void cdocmd_add_pm_clock_getdivider(CdoSequence * seq, uint32_t nodeid);
CDOUTIL_EXPORT void cdocmd_add_pm_clock_setrate(CdoSequence * seq, uint32_t nodeid, uint32_t rate);
CDOUTIL_EXPORT void cdocmd_add_pm_clock_getrate(CdoSequence * seq, uint32_t nodeid);
CDOUTIL_EXPORT void cdocmd_add_pm_clock_setparent(CdoSequence * seq, uint32_t nodeid, uint32_t parentid);
CDOUTIL_EXPORT void cdocmd_add_pm_clock_getparent(CdoSequence * seq, uint32_t nodeid);
CDOUTIL_EXPORT void cdocmd_add_pm_pll_set_parameter(CdoSequence * seq, uint32_t nodeid, uint32_t param, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_pm_pll_get_parameter(CdoSequence * seq, uint32_t nodeid, uint32_t param);
CDOUTIL_EXPORT void cdocmd_add_pm_pll_set_mode(CdoSequence * seq, uint32_t nodeid, uint32_t val);
CDOUTIL_EXPORT void cdocmd_add_pm_pll_get_mode(CdoSequence * seq, uint32_t nodeid);
CDOUTIL_EXPORT void cdocmd_add_pm_add_subsystem(CdoSequence * seq, uint32_t ssid);
CDOUTIL_EXPORT void cdocmd_add_pm_destroy_subsystem(CdoSequence * seq, uint32_t ssid);
CDOUTIL_EXPORT void cdocmd_add_pm_describe_nodes(CdoSequence * seq, void * buf, uint32_t count, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_pm_add_node(CdoSequence * seq, uint32_t nodeid, uint32_t count, void * buf, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_pm_add_node_parent(CdoSequence * seq, uint32_t nodeid, uint32_t count, void * buf, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_pm_add_node_name(CdoSequence * seq, uint32_t nodeid, const char * name);
CDOUTIL_EXPORT void cdocmd_add_pm_add_requirement(CdoSequence * seq, uint32_t ssid, uint32_t nodeid, uint32_t flags, uint32_t count, void * buf, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_pm_set_current_subsystem(CdoSequence * seq, uint32_t ssid);
CDOUTIL_EXPORT void cdocmd_add_pm_init_node(CdoSequence * seq, uint32_t nodeid, uint32_t function, uint32_t count, void * buf, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_pm_feature_check(CdoSequence * seq, uint32_t id);
CDOUTIL_EXPORT void cdocmd_add_pm_iso_control(CdoSequence * seq, uint32_t nodeid, uint32_t value);

CDOUTIL_EXPORT void cdocmd_add_cfu_set_crc32(CdoSequence * seq, uint32_t type, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_cfu_decompress(CdoSequence * seq, uint32_t type);
CDOUTIL_EXPORT void cdocmd_add_cfu_cram_rw(CdoSequence * seq, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_cfu_seu_go(CdoSequence * seq, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_cfu_crc8_dis(CdoSequence * seq, uint32_t type);
CDOUTIL_EXPORT void cdocmd_add_cfu_ssi_per_slr_pr(CdoSequence * seq, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_cfu_gsr_gsc(CdoSequence * seq, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_cfu_gcap_clk_en(CdoSequence * seq, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_cfu_cfi_type(CdoSequence * seq, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_cframe_read(CdoSequence * seq, uint32_t param, uint64_t addr, uint32_t read_count, uint32_t count, void * buf, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_ssit_sync_master(CdoSequence * seq);
CDOUTIL_EXPORT void cdocmd_add_ssit_sync_slaves(CdoSequence * seq, uint32_t mask, uint32_t count);
CDOUTIL_EXPORT void cdocmd_add_ssit_wait_slaves(CdoSequence * seq, uint32_t mask, uint32_t count);
CDOUTIL_EXPORT void cdocmd_add_nop(CdoSequence * seq, uint32_t count, void * buf, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_event_logging(CdoSequence * seq, uint32_t subcmd, uint32_t count, void * buf, uint32_t be);
CDOUTIL_EXPORT void cdocmd_add_set_board(CdoSequence * seq, const char * name);
CDOUTIL_EXPORT void cdocmd_add_set_plm_wdt(CdoSequence * seq, uint32_t nodeid, uint32_t periodicity);
CDOUTIL_EXPORT void cdocmd_add_log_string(CdoSequence * seq, const char * str);
CDOUTIL_EXPORT void cdocmd_add_log_address(CdoSequence * seq, uint64_t srcaddr);
CDOUTIL_EXPORT void cdocmd_add_marker(CdoSequence * seq, uint32_t value, const char * str);
CDOUTIL_EXPORT void cdocmd_add_proc(CdoSequence * seq, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_begin(CdoSequence * seq, const char * str);
CDOUTIL_EXPORT void cdocmd_add_end(CdoSequence * seq);
CDOUTIL_EXPORT void cdocmd_add_break(CdoSequence * seq, uint32_t value);
CDOUTIL_EXPORT void cdocmd_add_ldr_set_image_info(CdoSequence * seq, uint32_t nodeid, uint32_t uid, uint32_t puid, uint32_t funcid);
CDOUTIL_EXPORT void cdocmd_add_em_set_action(CdoSequence * seq, uint32_t nodeid, uint32_t action, uint32_t mask);
CDOUTIL_EXPORT void cdocmd_insert_seq(CdoCommand * cmd, CdoSequence * seq);
CDOUTIL_EXPORT void cdocmd_concat_seq(CdoSequence * seq, CdoSequence * seq2);

CDOUTIL_EXPORT void cdocmd_rewrite_address(CdoSequence * seq);
CDOUTIL_EXPORT void cdocmd_rewrite_block(CdoSequence * seq);
CDOUTIL_EXPORT void cdocmd_rewrite_sequential(CdoSequence * seq);
CDOUTIL_EXPORT void cdocmd_rewrite_repeat(CdoSequence * seq);
CDOUTIL_EXPORT void cdocmd_remove_comments(CdoSequence * seq);

static inline uint16_t u16swap(uint16_t v) {
    return ((v >> 8) & 0xff) | ((v << 8) & 0xff00);
}

static inline uint32_t u32swap(uint32_t v) {
    return u16swap(v >> 16) | (u16swap(v) << 16);
}

#if is_be_host()
static inline uint32_t u32be(uint32_t v) {
    return v;
}

static inline uint32_t u32le(uint32_t v) {
    return u32swap(v);
}
#else
static inline uint32_t u32be(uint32_t v) {
    return u32swap(v);
}

static inline uint32_t u32le(uint32_t v) {
    return v;
}
#endif

#endif /* D_cdo_command */
