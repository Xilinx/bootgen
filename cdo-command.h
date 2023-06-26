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

#ifndef D_cdo_command
#define D_cdo_command

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "link.h"

#define is_be_host() (0)

/* CDO Version 1.50 is using CDOv2 format, but allows CDOv1 commands
 * for NPI and CFU  */
#define CDO_VERSION_1_50 ((1 << 8) | 50)

/* CDO Version 2.00 is using CDOv2 format, but replaces NPI and CFU
 * commands with SEM */
#define CDO_VERSION_2_00 ((2 << 8) | 0)

/* CDO Version 3.00 is not defined and therefore the upper allowed boundary */
#define CDO_VERSION_3_00 ((3 << 8) | 0)

#define DEFAULT_CDO_VERSION CDO_VERSION_2_00

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
    CdoCmdOtCheck,
    CdoCmdPsmSequence,
    CdoCmdScatterWrite,
    CdoCmdScatterWrite2,
    CdoCmdTamperTrigger,
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
    CdoCmdPmActivateSubsystem,
    CdoCmdPmBisr,
    CdoCmdPmApplyTrim,
    CdoCmdPmNocClockEnable,
    CdoCmdPmIfNocClockEnable,
    CdoCmdPmSetNodeAccess,
    CdoCmdPmForceHc,
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
    CdoCmdLdrCframeClearCheck,
    CdoCmdSemNpiTable,

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

CdoSequence * cdocmd_create_sequence(void);
void cdocmd_delete_sequence(CdoSequence * seq);
CdoCommand * cdocmd_alloc(CdoCmdType type);
void cdocmd_free(CdoCommand * cmd);
CdoCommand * cdocmd_duplicate(CdoCommand * cmd);
void cdocmd_set_default_sequence(CdoSequence * seq);
void cdocmd_append_command(CdoSequence * seq, CdoCommand * newcmd);
void cdocmd_insert_command(CdoCommand * cmd, CdoCommand * newcmd);
void cdocmd_add_random_command(CdoSequence * seq, uint32_t * levelp);
void cdocmd_add_generic_command(CdoSequence * seq, uint32_t id, void * buf, uint32_t count, uint32_t be);
void cdocmd_add_comment(CdoSequence * seq, const char * comment, ...);
void cdocmd_add_section(CdoSequence * seq, uint32_t secid);
void cdocmd_add_include(CdoSequence * seq, const char * name);
void cdocmd_add_mask_poll(CdoSequence * seq, uint64_t addr, uint32_t mask, uint32_t value, uint32_t count, uint32_t flags);
void cdocmd_add_mask_write(CdoSequence * seq, uint64_t addr, uint32_t mask, uint32_t value);
void cdocmd_add_write(CdoSequence * seq, uint64_t addr, uint32_t value);
void cdocmd_add_delay(CdoSequence * seq, uint32_t delay);
void cdocmd_add_block_write(CdoSequence * seq, uint64_t addr, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_keyhole_write(CdoSequence * seq, uint64_t addr, uint32_t keyhole_size, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_block_set(CdoSequence * seq, uint64_t addr, uint32_t count, uint32_t value);
void cdocmd_add_dma_xfer(CdoSequence * seq, uint64_t srcaddr, uint64_t dstaddr, uint32_t count, uint32_t flags, void * buf, uint32_t be);
void cdocmd_add_npi_seq(CdoSequence * seq, uint32_t addr, uint32_t flags);
void cdocmd_add_npi_precfg(CdoSequence * seq, uint32_t addr, uint32_t flags);
void cdocmd_add_npi_write(CdoSequence * seq, uint32_t addr, uint32_t flags, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_npi_shutdown(CdoSequence * seq, uint32_t addr, uint32_t flags);

void cdocmd_add_scatter_write(CdoSequence * seq, uint32_t value, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_scatter_write2(CdoSequence * seq, uint32_t value1, uint32_t value2, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_pm_bisr(CdoSequence * seq, uint32_t tagid);
void cdocmd_add_pm_apply_trim(CdoSequence * seq, uint32_t trimtype);

void cdocmd_add_pm_get_api_version(CdoSequence * seq);
void cdocmd_add_pm_get_device_status(CdoSequence * seq, uint32_t nodeid);
void cdocmd_add_pm_register_notifier(CdoSequence * seq, uint32_t nodeid, uint32_t mask, uint32_t arg1, uint32_t arg2);
void cdocmd_add_pm_request_suspend(CdoSequence * seq, uint32_t nodeid, uint32_t ack, uint32_t latency, uint32_t state);
void cdocmd_add_pm_self_suspend(CdoSequence * seq, uint32_t nodeid, uint32_t latency, uint32_t state, uint64_t addr);
void cdocmd_add_pm_force_powerdown(CdoSequence * seq, uint32_t nodeid, uint32_t ack);
void cdocmd_add_pm_abort_suspend(CdoSequence * seq, uint32_t reason, uint32_t nodeid);
void cdocmd_add_pm_request_wakeup(CdoSequence * seq, uint32_t nodeid, uint64_t addr, uint32_t ack);
void cdocmd_add_pm_set_wakeup_source(CdoSequence * seq, uint32_t target, uint32_t source, uint32_t enable);
void cdocmd_add_pm_system_shutdown(CdoSequence * seq, uint32_t type, uint32_t subtype);
void cdocmd_add_pm_request_device(CdoSequence * seq, uint32_t nodeid, uint32_t capabilities, uint32_t qos, uint32_t ack);
void cdocmd_add_pm_release_device(CdoSequence * seq, uint32_t nodeid);
void cdocmd_add_pm_set_requirement(CdoSequence * seq, uint32_t nodeid, uint32_t capabilities, uint32_t qos, uint32_t ack);
void cdocmd_add_pm_set_max_latency(CdoSequence * seq, uint32_t nodeid, uint32_t latency);
void cdocmd_add_pm_reset_assert(CdoSequence * seq, uint32_t nodeid, uint32_t action);
void cdocmd_add_pm_reset_get_status(CdoSequence * seq, uint32_t nodeid);
void cdocmd_add_pm_pinctrl_request(CdoSequence * seq, uint32_t pinid);
void cdocmd_add_pm_pinctrl_release(CdoSequence * seq, uint32_t pinid);
void cdocmd_add_pm_pinctrl_get_function(CdoSequence * seq, uint32_t pinid);
void cdocmd_add_pm_pinctrl_set_function(CdoSequence * seq, uint32_t pinid, uint32_t funcid);
void cdocmd_add_pm_pinctrl_config_param_get(CdoSequence * seq, uint32_t pinid, uint32_t param);
void cdocmd_add_pm_pinctrl_config_param_set(CdoSequence * seq, uint32_t pinid, uint32_t param, uint32_t value);
void cdocmd_add_pm_ioctl(CdoSequence * seq, uint32_t nodeid, uint32_t ioctlid, void * buf, uint32_t count, uint32_t be);
void cdocmd_add_pm_query_data(CdoSequence * seq, uint32_t id, void * buf, uint32_t count, uint32_t be);
void cdocmd_add_pm_clock_enable(CdoSequence * seq, uint32_t nodeid);
void cdocmd_add_pm_clock_disable(CdoSequence * seq, uint32_t nodeid);
void cdocmd_add_pm_clock_getstate(CdoSequence * seq, uint32_t nodeid);
void cdocmd_add_pm_clock_setdivider(CdoSequence * seq, uint32_t nodeid, uint32_t div);
void cdocmd_add_pm_clock_getdivider(CdoSequence * seq, uint32_t nodeid);
void cdocmd_add_pm_clock_setrate(CdoSequence * seq, uint32_t nodeid, uint32_t rate);
void cdocmd_add_pm_clock_getrate(CdoSequence * seq, uint32_t nodeid);
void cdocmd_add_pm_clock_setparent(CdoSequence * seq, uint32_t nodeid, uint32_t parentid);
void cdocmd_add_pm_clock_getparent(CdoSequence * seq, uint32_t nodeid);
void cdocmd_add_pm_pll_set_parameter(CdoSequence * seq, uint32_t nodeid, uint32_t param, uint32_t value);
void cdocmd_add_pm_pll_get_parameter(CdoSequence * seq, uint32_t nodeid, uint32_t param);
void cdocmd_add_pm_pll_set_mode(CdoSequence * seq, uint32_t nodeid, uint32_t val);
void cdocmd_add_pm_pll_get_mode(CdoSequence * seq, uint32_t nodeid);
void cdocmd_add_pm_add_subsystem(CdoSequence * seq, uint32_t ssid);
void cdocmd_add_pm_destroy_subsystem(CdoSequence * seq, uint32_t ssid);
void cdocmd_add_pm_describe_nodes(CdoSequence * seq, void * buf, uint32_t count, uint32_t be);
void cdocmd_add_pm_add_node(CdoSequence * seq, uint32_t nodeid, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_pm_add_node_parent(CdoSequence * seq, uint32_t nodeid, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_pm_add_node_name(CdoSequence * seq, uint32_t nodeid, const char * name);
void cdocmd_add_pm_add_requirement(CdoSequence * seq, uint32_t ssid, uint32_t nodeid, uint32_t flags, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_pm_set_current_subsystem(CdoSequence * seq, uint32_t ssid);
void cdocmd_add_pm_init_node(CdoSequence * seq, uint32_t nodeid, uint32_t function, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_pm_feature_check(CdoSequence * seq, uint32_t id);
void cdocmd_add_pm_iso_control(CdoSequence * seq, uint32_t nodeid, uint32_t value);
void cdocmd_add_pm_activate_subsystem(CdoSequence * seq, uint32_t ssid);
void cdocmd_add_pm_set_node_access(CdoSequence * seq, uint32_t nodeid, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_pm_noc_clock_enable(CdoSequence * seq, uint32_t nodeid, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_pm_if_noc_clock_enable(CdoSequence * seq, uint32_t index, uint32_t state, uint32_t break_level);
void cdocmd_add_pm_force_hc(CdoSequence * seq, uint32_t nodeid);

void cdocmd_add_cfu_set_crc32(CdoSequence * seq, uint32_t type, uint32_t value);
void cdocmd_add_cfu_decompress(CdoSequence * seq, uint32_t type);
void cdocmd_add_cfu_cram_rw(CdoSequence * seq, uint32_t value);
void cdocmd_add_cfu_seu_go(CdoSequence * seq, uint32_t value);
void cdocmd_add_cfu_crc8_dis(CdoSequence * seq, uint32_t type);
void cdocmd_add_cfu_ssi_per_slr_pr(CdoSequence * seq, uint32_t value);
void cdocmd_add_cfu_gsr_gsc(CdoSequence * seq, uint32_t value);
void cdocmd_add_cfu_gcap_clk_en(CdoSequence * seq, uint32_t value);
void cdocmd_add_cfu_cfi_type(CdoSequence * seq, uint32_t value);
void cdocmd_add_cframe_read(CdoSequence * seq, uint32_t param, uint64_t addr, uint32_t read_count, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_ssit_sync_master(CdoSequence * seq);
void cdocmd_add_ssit_sync_slaves(CdoSequence * seq, uint32_t mask, uint32_t count);
void cdocmd_add_ssit_wait_slaves(CdoSequence * seq, uint32_t mask, uint32_t count);
void cdocmd_add_nop(CdoSequence * seq, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_event_logging(CdoSequence * seq, uint32_t subcmd, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_set_board(CdoSequence * seq, const char * name);
void cdocmd_add_set_plm_wdt(CdoSequence * seq, uint32_t nodeid, uint32_t periodicity);
void cdocmd_add_log_string(CdoSequence * seq, const char * str);
void cdocmd_add_log_address(CdoSequence * seq, uint64_t srcaddr);
void cdocmd_add_marker(CdoSequence * seq, uint32_t value, const char * str);
void cdocmd_add_proc(CdoSequence * seq, uint32_t value);
void cdocmd_add_begin(CdoSequence * seq, const char * str);
void cdocmd_add_end(CdoSequence * seq);
void cdocmd_add_break(CdoSequence * seq, uint32_t value);
void cdocmd_add_ot_check(CdoSequence * seq, uint32_t value);
void cdocmd_add_psm_sequence(CdoSequence * seq);
void cdocmd_add_tamper_trigger(CdoSequence * seq, uint32_t value);
void cdocmd_add_sem_npi_table(CdoSequence * seq, uint32_t nodeid, uint32_t flags, uint32_t count, void * buf, uint32_t be);
void cdocmd_add_ldr_set_image_info(CdoSequence * seq, uint32_t nodeid, uint32_t uid, uint32_t puid, uint32_t funcid);
void cdocmd_add_ldr_cframe_clear_check(CdoSequence * seq, uint32_t block_type);
void cdocmd_add_em_set_action(CdoSequence * seq, uint32_t nodeid, uint32_t action, uint32_t mask);
void cdocmd_insert_seq(CdoCommand * cmd, CdoSequence * seq);
void cdocmd_concat_seq(CdoSequence * seq, CdoSequence * seq2);

void cdocmd_rewrite_address(CdoSequence * seq);
void cdocmd_rewrite_block(CdoSequence * seq);
void cdocmd_rewrite_sequential(CdoSequence * seq);
void cdocmd_rewrite_repeat(CdoSequence * seq);
void cdocmd_remove_comments(CdoSequence * seq);

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
