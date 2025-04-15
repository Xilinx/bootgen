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
#include <stdint.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include "xpm-nodeid-cdoutil.h"
#include "crp.h"
#include "cfu_apb.h"
//#include "xpm_node.h"
#include "cdo-binary.h"
#ifdef ENABLE_Slaves
#include "cdo-slaves.h"
#endif
#include "cdo-upgrade.h"
#include "cdo-source.h"
#include "spp-ddr.h"
#include "cdo-npi.h"

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

#define TOMASK32(value) ((uint32_t)1 << (value))
#define DEFAULT_SSIT_TIMEOUT 3000000
#define DEFAULT_POLL_TIMEOUT 1000000
#define TEST_POLL_TIMEOUT 1

/**
 * Npi Parameter Mask
 */
#define XILCDO_NPI_BLK_MASK				(0xFFU)
#define XILCDO_NPI_FABRICEN_MASK				(0x100U)
#define XILCDO_NPI_APBEN_MASK					(0x200U)
#define XILCDO_NPI_DDRMC_PRESENT_MASK			(0x400U)
#define XILCDO_NPI_PR_FREEZE_MASK			(0x800U)
#define XILCDO_NPI_GTY_USED_MASK			(0x1000U)
#define XILCDO_NPI_MEMCLR_MASK				(0x1000000U)
#define XILCDO_NPI_SCANCLR_MASK				(0x2000000U)

#define XILCDO_NPI_FIRST_MASK                           (0x1U)
#define XILCDO_NPI_FABRIC_STARTUP_MASK                  (0x2U)
#define XILCDO_NPI_RECONFIGURATION_MASK                 (0x4U)

/**
 * Block Type values
 */
#define XILCDO_NPI_BLK_DDRMC_MAIN                       (0x0U)
#define XILCDO_NPI_BLK_VREF                    		(0x1U)
#define XILCDO_NPI_BLK_XPIO                             (0x2U)
#define XILCDO_NPI_BLK_XPIO_IOMISC                      (0x3U)
#define XILCDO_NPI_BLK_XPLL                             (0x4U)
#define XILCDO_NPI_BLK_XPHY                             (0x5U)
#define XILCDO_NPI_BLK_DDRMC                            (0x6U)
#define XILCDO_NPI_BLK_XPIPE                            (0x7U)
#define XILCDO_NPI_BLK_GT                               (0x8U)
#define XILCDO_NPI_BLK_NOC_NPS                          (0x9U)
#define XILCDO_NPI_BLK_NOC_NCRB                         (0xAU)
#define XILCDO_NPI_BLK_NOC_NSU                          (0xBU)
#define XILCDO_NPI_BLK_NOC_NMU                          (0xCU)
#define XILCDO_NPI_BLK_NOC_IDB                          (0xDU)
#define XILCDO_NPI_BLK_VERT_TO_HSR                      (0xEU)
#define XILCDO_NPI_BLK_VERT_TO_HSR_GT                   (0xFU)
#define XILCDO_NPI_BLK_REBUF_VRT                        (0x10U)
#define XILCDO_NPI_BLK_REBUF_VRT_GT                     (0x11U)
#define XILCDO_NPI_BLK_HSR_BUFGS                        (0x12U)
#define XILCDO_NPI_BLK_VNOC                             (0x13U)
#define XILCDO_NPI_BLK_VNOC_PS                          (0x14U)
#define XILCDO_NPI_BLK_CLK_GT                           (0x15U)
#define XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME                (0x16U)
#define XILCDO_NPI_BLK_MMCM                             (0x17U)
#define XILCDO_NPI_BLK_DPLL                             (0x18U)
#define XILCDO_NPI_BLK_AMS_SAT                          (0x19U)
#define XILCDO_NPI_BLK_AMS_ROOT                         (0x1AU)
#define XILCDO_NPI_BLK_ME_NPI                           (0x1BU)
#define XILCDO_NPI_BLK_PLL_PHY							(0x1CU)
#define XILCDO_NPI_BLK_NOC_NIR							(0x1DU)
#define XILCDO_TOTAL_NPI_BLKS                           (0x20U)

#define BLKMASK_DDRMC_MAIN           TOMASK32(XILCDO_NPI_BLK_DDRMC_MAIN)
#define BLKMASK_VREF                 TOMASK32(XILCDO_NPI_BLK_VREF)
#define BLKMASK_XPIO                 TOMASK32(XILCDO_NPI_BLK_XPIO)
#define BLKMASK_XPIO_IOMISC          TOMASK32(XILCDO_NPI_BLK_XPIO_IOMISC)
#define BLKMASK_XPLL                 TOMASK32(XILCDO_NPI_BLK_XPLL)
#define BLKMASK_XPHY                 TOMASK32(XILCDO_NPI_BLK_XPHY)
#define BLKMASK_DDRMC                TOMASK32(XILCDO_NPI_BLK_DDRMC)
#define BLKMASK_XPIPE                TOMASK32(XILCDO_NPI_BLK_XPIPE)
#define BLKMASK_GT                   TOMASK32(XILCDO_NPI_BLK_GT)
#define BLKMASK_NOC_NPS              TOMASK32(XILCDO_NPI_BLK_NOC_NPS)
#define BLKMASK_NOC_NCRB             TOMASK32(XILCDO_NPI_BLK_NOC_NCRB)
#define BLKMASK_NOC_NSU              TOMASK32(XILCDO_NPI_BLK_NOC_NSU)
#define BLKMASK_NOC_NMU              TOMASK32(XILCDO_NPI_BLK_NOC_NMU)
#define BLKMASK_NOC_IDB              TOMASK32(XILCDO_NPI_BLK_NOC_IDB)
#define BLKMASK_VERT_TO_HSR          TOMASK32(XILCDO_NPI_BLK_VERT_TO_HSR)
#define BLKMASK_VERT_TO_HSR_GT       TOMASK32(XILCDO_NPI_BLK_VERT_TO_HSR_GT)
#define BLKMASK_REBUF_VRT            TOMASK32(XILCDO_NPI_BLK_REBUF_VRT)
#define BLKMASK_REBUF_VRT_GT         TOMASK32(XILCDO_NPI_BLK_REBUF_VRT_GT)
#define BLKMASK_HSR_BUFGS            TOMASK32(XILCDO_NPI_BLK_HSR_BUFGS)
#define BLKMASK_VNOC                 TOMASK32(XILCDO_NPI_BLK_VNOC)
#define BLKMASK_VNOC_PS              TOMASK32(XILCDO_NPI_BLK_VNOC_PS)
#define BLKMASK_CLK_GT               TOMASK32(XILCDO_NPI_BLK_CLK_GT)
#define BLKMASK_REBUF_HSR_TNOC_ME    TOMASK32(XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME)
#define BLKMASK_MMCM                 TOMASK32(XILCDO_NPI_BLK_MMCM)
#define BLKMASK_DPLL                 TOMASK32(XILCDO_NPI_BLK_DPLL)
#define BLKMASK_AMS_SAT              TOMASK32(XILCDO_NPI_BLK_AMS_SAT)
#define BLKMASK_AMS_ROOT             TOMASK32(XILCDO_NPI_BLK_AMS_ROOT)
#define BLKMASK_ME_NPI               TOMASK32(XILCDO_NPI_BLK_ME_NPI)
#define BLKMASK_PLL_PHY              TOMASK32(XILCDO_NPI_BLK_PLL_PHY)
#define BLKMASK_NOC_NIR              TOMASK32(XILCDO_NPI_BLK_NOC_NIR)

/* Special marker indicating that rule should be done regardless of
 * block type. */
#define BLKMASK_ALWAYS               (~(uint32_t)0)

/*
 * PCSR register offsets
 */
#define XILCDO_NPI_PCSR_MASK_OFFSET				(0x0U)
#define XILCDO_NPI_PCSR_CONTROL_OFFSET			(0x4U)
#define XILCDO_NPI_PCSR_STATUS_OFFSET			(0x8U)
#define XILCDO_NPI_PCSR_LOCK_OFFSET				(0xCU)
#define XILCDO_NPI_PENDING_BURST_OFFSET			(0x264U)
#define XILCDO_NPI_NMU_BUSY_OFFSET				(0x810U)

/**
 * PCSR MASK register bit fields
 */
#define PCSR_MASK_BISR_TRIGGER_MASK				(0x20000000U)
#define PCSR_MASK_SHTDN_MASK    				(0x08000000U)
#define PCSR_MASK_ME_ARRAY_RESET_MASK			(0x04000000U)
#define PCSR_MASK_NSU_PR_MASK					(0x04000000U)
#define PCSR_MASK_AXI_REQ_REJECT_MASK			(0x04000000U)
#define PCSR_MASK_ME_IPOR_MASK					(0x01000000U)
#define PCSR_MASK_PR_FREEZE_MASK				(0x01000000U)
#define PCSR_MASK_DESKEW_MASK					(0x01000000U)
#define PCSR_MASK_UB_INIT_MASK					(0x01000000U)
#define PCSR_MASK_INIT_CTRL_MASK				(0x01000000U)
#define PCSR_MASK_MEM_CLEAR_EN_ALL				(0x00800000U)
#define PCSR_MASK_OD_BIST_SETUP1				(0x00400000U)
#define PCSR_MASK_DCI_OFC_RST_MASK              (0x00200000U)
#define PCSR_MASK_OD_MBIST_ASYNC_RESET			(0x00200000U)
#define PCSR_MASK_STARTBISR_MASK    			(0x00080000U)
#define PCSR_MASK_MBISTCLR_MASK					(0x00040000U)
#define PCSR_MASK_SYS_RST_MASK_MASK 			(0x00038000U)
#define PCSR_MASK_PWRDN_MASK    				(0x00004000U)
#define PCSR_MASK_DISNPICLK_MASK    			(0x00002000U)
#define PCSR_MASK_APBEN_MASK    				(0x00001000U)
#define PCSR_MASK_SCANCLR_MASK    				(0x00000800U)
#define PCSR_MASK_STARTCAL_MASK    				(0x00000400U)
#define PCSR_MASK_SLEEP_MASK    				(0x00000400U)
#define PCSR_MASK_FABRICEN_MASK    				(0x00000200U)
#define PCSR_MASK_TRISTATE_MASK    				(0x00000100U)
#define PCSR_MASK_HOLDSTATE_MASK    			(0x00000080U)
#define PCSR_MASK_INITSTATE_MASK    			(0x00000040U)
#define PCSR_MASK_ODISABLE_MASK    				(0x0000003CU)
#define PCSR_MASK_ODISABLE1_MASK				(0x00000008U)
#define PCSR_MASK_ODISABLE_NPP_MASK				(0x00000008U)
#define PCSR_MASK_ODISABLE0_MASK				(0x00000004U)
#define PCSR_MASK_ODISABLE_AXI_MASK				(0x00000004U)
#define PCSR_MASK_GATEREG_MASK    				(0x00000002U)
#define PCSR_MASK_PCOMPLETE_MASK    			(0x00000001U)

/**
 * Register: PCSR_STATUS
 */
#define PCSR_STATUS_SHUTDN_COMP_MASK			(0x00040000U)
#define PCSR_STATUS_ME_PWR_SUPPLY_MASK			(0x00008000U)
#define PCSR_STATUS_BISR_PASS_MASK    			(0x00000200U)
#define PCSR_STATUS_BISRDONE_MASK    			(0x00000100U)
#define PCSR_STATUS_MBIST_PASS_MASK    			(0x00000080U)
#define PCSR_STATUS_MBISTDONE_MASK    			(0x00000040U)
#define PCSR_STATUS_CALERROR_MASK    			(0x00000020U)
#define PCSR_STATUS_CALDONE_MASK    			(0x00000010U)
#define PCSR_STATUS_INCAL_MASK    			(0x00000008U)
#define PCSR_STATUS_SCAN_PASS_MASK    			(0x00000004U)
#define PCSR_STATUS_SCANDONE_MASK    			(0x00000002U)
#define PCSR_STATUS_PCSRLOCK_MASK    			(0x00000001U)

/**
 * Register: PCSR_LOCK
 */
#define PCSR_LOCK_STATE_MASK    			(0x00000001U)
#define PCSR_UNLOCK_VAL					(0xF9E8D7C6U)

/* NMU busy register */
#define XILCDO_REG_BUSY_OFFSET				(0x810U)
#define XILCDO_REG_BUSY_MASK				(0x1U)

/* NSU pending register */
#define XILCDO_REG_PEND_BURST_OFFSET			(0x264U)
#define XILCDO_REG_PEND_BURST_OFFSET_MASK		(0x3FFFU)
#define XILCDO_NSU_REG_BUSY_OFFSET			(0x268U)
#define XILCDO_NSU_REG_BUSY_MASK			(0x1U)

/** DDRMC_NOC Register */
#define XILCDO_DDRMC_NOC_CLK_MUX_OFFSET			(0x758U)
#define XILCDO_DDRMC_CLK_SRC_SELMASK			(0x1U)

//#define CFU_STREAM_ADDR     (0xF12C0000)
//#define CFU_FDRO_ADDR       (0xF12C2000)
#define CFU_STREAM_ADDR     (0xF1F80000)
#define CFU_FDRO_ADDR       (0xF1FC0000)

#define AMS_ROOT_BASE 0xF1270000
#define AMS_ROOT_TOKEN_MNGR (AMS_ROOT_BASE + 0x104)
#define AMS_ROOT_TOKEN_MNGR_BYPASS_PL_MASK (1 << 20)

enum {
    CFI_TYPE_FULL_CONFIG = 0,
    CFI_TYPE_PR_UNMASK = 1,
    CFI_TYPE_PR_CLEAR = 2,
    CFI_TYPE_PR_CONFIG = 3,
    CFI_TYPE_PR_MASK = 4,
    CFI_TYPE_TANDEM_STAGE1 = 5,
    CFI_TYPE_TANDEM_STAGE2_INCR = 6,
    CFI_TYPE_TANDEM_STAGE2_ONLY = 7,
};

enum {
    NODE_INIT_START = 0,
    NODE_INIT_FINISH = 1,
    NODE_INIT_SCAN_CLEAR = 2,
    NODE_INIT_BISR_TRIM = 3,
    NODE_INIT_LBIST = 4,
    NODE_INIT_MEM_INIT = 5,
    NODE_INIT_MBIST = 6,
    NODE_INIT_HOUSE_CLEAN_PLPD = 7,
};


#define NODEID_NPD  PM_POWER_NOC
#define NODEID_PLPD PM_POWER_PLD

enum {
    NPD_INDEX,
    PLPD_INDEX,
    MAXPD_INDEX
};

#define MAX_PRECFG_CMDS 100
#define MAX_SEQ_CMDS 100
#define MAX_SHUTDOWN_CMDS 100
#define MAX_BIST_BLOCKS 100

typedef struct RewriteInfo RewriteInfo;
typedef struct PdInfo PdInfo;
typedef struct BlockHandler BlockHandler;

struct BlockHandler {
    void (*handler)(CdoSequence * seq, uint32_t BaseAddr);
    uint32_t flagmask;
    uint32_t flagvalue;
    uint32_t typemask;
};

struct PdInfo {
    uint32_t has_seq:1;
    uint32_t has_precfg:1;
    uint32_t has_shutdown:1;
    uint32_t nodeid;
    CdoSequence * precfg_seq;
    CdoCommand * precfg_cmds[MAX_PRECFG_CMDS];
    uint32_t precfg_cnt;
    CdoCommand * seq_cmds[MAX_SEQ_CMDS];
    uint32_t seq_cnt;
    CdoCommand * shutdown_cmds[MAX_SEQ_CMDS];
    uint32_t shutdown_cnt;
};

struct RewriteInfo {
    uint32_t noc_srst_req;
    uint32_t precfg_pdmask;
};


typedef struct CdoCategories CdoCategories;
struct CdoCategories {
    uint32_t generic;
    uint32_t precfg;
    uint32_t seq;
    uint32_t shutdown;
    uint32_t cfu;
    uint32_t npiwrite;
    uint32_t has_startup;
};

#define MAX_CRC32 10

static uint32_t crc32_check[MAX_CRC32];
static uint32_t crc32_value[MAX_CRC32];
static uint32_t crc32_write = 0;
static uint32_t crc32_read = 0;
static uint32_t decompress = 0;
static uint32_t cram_rw_set = 0;
static uint32_t cram_rw_value = 0;
static uint32_t seu_go = 0;
static uint32_t crc8_dis = 0;
static uint32_t ssi_per_slr_pr = 0;
static uint32_t gsr_gsc = 0;
static uint32_t gcap_clk_en = 0;
static uint32_t cfi_type_valid = 0;
static uint32_t cfi_type = 0;
static uint32_t debug_print = 0;
static uint32_t fabric_shutdown = 0;
static uint32_t fabric_startup = 0;
static uint32_t npd_houseclean_done = 0;
static uint32_t plpd_houseclean_done = 0;
static uint32_t spp_ddr_ena = 1;
static size_t cfu_stream_keyhole_size = 256*1024;
static uint32_t fpd_init_finish_move = 0;
static uint32_t init_finish_move_enable = 0;
static uint32_t dma_write_keyhole_enable = 1;
static uint32_t pl_iso_disable = 0;
static uint32_t ddr_hack_enable = 0;
static uint32_t dci_cal_wait_enable = 1;
static uint32_t pll_cal_wait_enable = 0;
static uint32_t pl_clk_activate = 1;
static uint32_t pl_rst_mask = 0;
static uint32_t seq_spec = 4;
static int32_t ssit_mode = 0;
static uint32_t npi_mode = 0;
static uint32_t pcsr_mode = 0;
static uint32_t block_continue_mode = 0;
static uint32_t auto_align = 1;
static uint32_t ddrmc_cal_flags = 0;

typedef struct NpiModule NpiModule;
struct NpiModule {
    uint32_t type;
    uint32_t index;
    uint32_t address;
} s80_npi_modules[] = {
    { XILCDO_NPI_BLK_AMS_SAT,  0,                         0XF61A0000 },
    { XILCDO_NPI_BLK_AMS_SAT,  1,                         0XF6310000 },
    { XILCDO_NPI_BLK_AMS_SAT,  2,                         0XF6480000 },
    { XILCDO_NPI_BLK_AMS_SAT,  3,                         0XF65F0000 },
    { XILCDO_NPI_BLK_AMS_SAT,  4,                         0XF6630000 },
    { XILCDO_NPI_BLK_AMS_SAT,  5,                         0XF6690000 },
    { XILCDO_NPI_BLK_AMS_SAT,  6,                         0XF66E0000 },
    { XILCDO_NPI_BLK_AMS_SAT,  7,                         0XF6770000 },
    { XILCDO_NPI_BLK_AMS_SAT,  8,                         0XF67B0000 },
    { XILCDO_NPI_BLK_AMS_SAT,  9,                         0XF6800000 },
    { XILCDO_NPI_BLK_AMS_SAT, 10,                         0XF6850000 },
    { XILCDO_NPI_BLK_AMS_SAT, 11,                         0XF6870000 },
    { XILCDO_NPI_BLK_AMS_SAT, 12,                         0XF68D0000 },
    { XILCDO_NPI_BLK_AMS_SAT, 13,                         0XF6930000 },
    { XILCDO_NPI_BLK_AMS_SAT, 14,                         0XF6970000 },
    { XILCDO_NPI_BLK_AMS_SAT, 15,                         0XF69D0000 },
    { XILCDO_NPI_BLK_AMS_SAT, 16,                         0XF6A10000 },
    { XILCDO_NPI_BLK_AMS_SAT, 17,                         0XF6A70000 },
    { XILCDO_NPI_BLK_AMS_SAT, 18,                         0XF6AD0000 },
    { XILCDO_NPI_BLK_AMS_SAT, 19,                         0XF6B30000 },
    { XILCDO_NPI_BLK_AMS_SAT, 20,                         0XF6B80000 },
    { XILCDO_NPI_BLK_AMS_SAT, 21,                         0XF6C10000 },
    { XILCDO_NPI_BLK_AMS_SAT, 22,                         0XF6C50000 },
    { XILCDO_NPI_BLK_AMS_SAT, 23,                         0XF6CA0000 },
    { XILCDO_NPI_BLK_AMS_SAT, 24,                         0XF6D00000 },
    { XILCDO_NPI_BLK_AMS_SAT, 25,                         0XF6D50000 },
    { XILCDO_NPI_BLK_AMS_SAT, 26,                         0XF6DE0000 },
    { XILCDO_NPI_BLK_AMS_SAT, 27,                         0XF6E20000 },
    { XILCDO_NPI_BLK_AMS_SAT, 28,                         0XF6E70000 },
    { XILCDO_NPI_BLK_AMS_SAT, 29,                         0XF6ED0000 },
    { XILCDO_NPI_BLK_AMS_SAT, 30,                         0XF6F20000 },
    { XILCDO_NPI_BLK_AMS_SAT, 31,                         0XF6FB0000 },
    { XILCDO_NPI_BLK_AMS_SAT, 32,                         0XF6FF0000 },
    { XILCDO_NPI_BLK_AMS_SAT, 33,                         0XF7040000 },
    { XILCDO_NPI_BLK_AMS_SAT, 34,                         0XF7090000 },
    { XILCDO_NPI_BLK_AMS_SAT, 35,                         0XF70C0000 },
    { XILCDO_NPI_BLK_AMS_SAT, 36,                         0XF7130000 },
    { XILCDO_NPI_BLK_AMS_SAT, 37,                         0XF71A0000 },
    { XILCDO_NPI_BLK_AMS_SAT, 38,                         0XF71F0000 },
    { XILCDO_NPI_BLK_CLK_GT, 0,                      0XF6880000 },
    { XILCDO_NPI_BLK_CLK_GT, 1,                      0XF68E0000 },
    { XILCDO_NPI_BLK_CLK_GT, 2,                      0XF6980000 },
    { XILCDO_NPI_BLK_CLK_GT, 3,                      0XF6A20000 },
    { XILCDO_NPI_BLK_CLK_GT, 4,                      0XF70D0000 },
    { XILCDO_NPI_BLK_CLK_GT, 5,                      0XF7140000 },
    { XILCDO_NPI_BLK_CLK_GT, 6,                      0XF7200000 },
    { XILCDO_NPI_BLK_PLL_PHY,  0,                 0XF6099800 },
    { XILCDO_NPI_BLK_PLL_PHY,  1,                 0XF6169800 },
    { XILCDO_NPI_BLK_PLL_PHY,  2,                 0XF61C9800 },
    { XILCDO_NPI_BLK_PLL_PHY,  3,                 0XF6239800 },
    { XILCDO_NPI_BLK_PLL_PHY,  4,                 0XF62D9800 },
    { XILCDO_NPI_BLK_PLL_PHY,  5,                 0XF6339800 },
    { XILCDO_NPI_BLK_PLL_PHY,  6,                 0XF63A9800 },
    { XILCDO_NPI_BLK_PLL_PHY,  7,                 0XF6449800 },
    { XILCDO_NPI_BLK_PLL_PHY,  8,                 0XF64A9800 },
    { XILCDO_NPI_BLK_PLL_PHY,  9,                 0XF6519800 },
    { XILCDO_NPI_BLK_PLL_PHY, 10,                 0XF65B9800 },
    { XILCDO_NPI_BLK_PLL_PHY, 11,                 0XF6619800 },
    { XILCDO_NPI_BLK_HSR_BUFGS,  0,        0XF6050000 },
    { XILCDO_NPI_BLK_HSR_BUFGS,  1,        0XF60C0000 },
    { XILCDO_NPI_BLK_HSR_BUFGS,  2,        0XF6190000 },
    { XILCDO_NPI_BLK_HSR_BUFGS,  3,        0XF61F0000 },
    { XILCDO_NPI_BLK_HSR_BUFGS,  4,        0XF6260000 },
    { XILCDO_NPI_BLK_HSR_BUFGS,  5,        0XF6300000 },
    { XILCDO_NPI_BLK_HSR_BUFGS,  6,        0XF6360000 },
    { XILCDO_NPI_BLK_HSR_BUFGS,  7,        0XF63D0000 },
    { XILCDO_NPI_BLK_HSR_BUFGS,  8,        0XF6470000 },
    { XILCDO_NPI_BLK_HSR_BUFGS,  9,        0XF64D0000 },
    { XILCDO_NPI_BLK_HSR_BUFGS, 10,        0XF6540000 },
    { XILCDO_NPI_BLK_HSR_BUFGS, 11,        0XF65E0000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME,  0,      0XF6760000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME,  1,      0XF67A0000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME,  2,      0XF67F0000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME,  3,      0XF6840000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME,  4,      0XF6C00000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME,  5,      0XF6C40000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME,  6,      0XF6DD0000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME,  7,      0XF6E10000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME,  8,      0XF6FA0000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME,  9,      0XF6FE0000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME, 10,      0XF7030000 },
    { XILCDO_NPI_BLK_REBUF_HSR_TNOC_ME, 11,      0XF7080000 },
    { XILCDO_NPI_BLK_REBUF_VRT,  0,             0XF6660000 },
    { XILCDO_NPI_BLK_REBUF_VRT,  1,             0XF66B0000 },
    { XILCDO_NPI_BLK_REBUF_VRT,  2,             0XF6700000 },
    { XILCDO_NPI_BLK_REBUF_VRT,  3,             0XF6730000 },
    { XILCDO_NPI_BLK_REBUF_VRT,  4,             0XF6B00000 },
    { XILCDO_NPI_BLK_REBUF_VRT,  5,             0XF6B50000 },
    { XILCDO_NPI_BLK_REBUF_VRT,  6,             0XF6BA0000 },
    { XILCDO_NPI_BLK_REBUF_VRT,  7,             0XF6BD0000 },
    { XILCDO_NPI_BLK_REBUF_VRT,  8,             0XF6CD0000 },
    { XILCDO_NPI_BLK_REBUF_VRT,  9,             0XF6D20000 },
    { XILCDO_NPI_BLK_REBUF_VRT, 10,             0XF6D70000 },
    { XILCDO_NPI_BLK_REBUF_VRT, 11,             0XF6DA0000 },
    { XILCDO_NPI_BLK_REBUF_VRT, 12,             0XF6EA0000 },
    { XILCDO_NPI_BLK_REBUF_VRT, 13,             0XF6EF0000 },
    { XILCDO_NPI_BLK_REBUF_VRT, 14,             0XF6F40000 },
    { XILCDO_NPI_BLK_REBUF_VRT, 15,             0XF6F70000 },
    { XILCDO_NPI_BLK_REBUF_VRT_GT, 0,           0XF68B0000 },
    { XILCDO_NPI_BLK_REBUF_VRT_GT, 1,           0XF6910000 },
    { XILCDO_NPI_BLK_REBUF_VRT_GT, 2,           0XF69B0000 },
    { XILCDO_NPI_BLK_REBUF_VRT_GT, 3,           0XF6A50000 },
    { XILCDO_NPI_BLK_REBUF_VRT_GT, 4,           0XF7110000 },
    { XILCDO_NPI_BLK_REBUF_VRT_GT, 5,           0XF7180000 },
    { XILCDO_NPI_BLK_REBUF_VRT_GT, 6,           0XF7240000 },
    { XILCDO_NPI_BLK_VERT_TO_HSR, 0,             0XF6640000 },
    { XILCDO_NPI_BLK_VERT_TO_HSR, 1,             0XF6AE0000 },
    { XILCDO_NPI_BLK_VERT_TO_HSR, 2,             0XF6CB0000 },
    { XILCDO_NPI_BLK_VERT_TO_HSR, 3,             0XF6E80000 },
    { XILCDO_NPI_BLK_VERT_TO_HSR_GT, 0,          0XF6A80000 },
    { XILCDO_NPI_BLK_VNOC,  0,                   0XF6670000 },
    { XILCDO_NPI_BLK_VNOC,  1,                   0XF66C0000 },
    { XILCDO_NPI_BLK_VNOC,  2,                   0XF6710000 },
    { XILCDO_NPI_BLK_VNOC,  3,                   0XF6740000 },
    { XILCDO_NPI_BLK_VNOC,  4,                   0XF6B10000 },
    { XILCDO_NPI_BLK_VNOC,  5,                   0XF6B60000 },
    { XILCDO_NPI_BLK_VNOC,  6,                   0XF6BB0000 },
    { XILCDO_NPI_BLK_VNOC,  7,                   0XF6BE0000 },
    { XILCDO_NPI_BLK_VNOC,  8,                   0XF6CE0000 },
    { XILCDO_NPI_BLK_VNOC,  9,                   0XF6D30000 },
    { XILCDO_NPI_BLK_VNOC, 10,                   0XF6D80000 },
    { XILCDO_NPI_BLK_VNOC, 11,                   0XF6DB0000 },
    { XILCDO_NPI_BLK_VNOC, 12,                   0XF6F00000 },
    { XILCDO_NPI_BLK_VNOC, 13,                   0XF6F50000 },
    { XILCDO_NPI_BLK_VNOC, 14,                   0XF6F80000 },
    { XILCDO_NPI_BLK_VNOC_PS, 0,                 0XF6EB0000 },
    { XILCDO_NPI_BLK_DPLL,  0,                        0XF6040000 },
    { XILCDO_NPI_BLK_DPLL,  1,                        0XF60B0000 },
    { XILCDO_NPI_BLK_DPLL,  2,                        0XF6180000 },
    { XILCDO_NPI_BLK_DPLL,  3,                        0XF61E0000 },
    { XILCDO_NPI_BLK_DPLL,  4,                        0XF6250000 },
    { XILCDO_NPI_BLK_DPLL,  5,                        0XF62F0000 },
    { XILCDO_NPI_BLK_DPLL,  6,                        0XF6350000 },
    { XILCDO_NPI_BLK_DPLL,  7,                        0XF63C0000 },
    { XILCDO_NPI_BLK_DPLL,  8,                        0XF6460000 },
    { XILCDO_NPI_BLK_DPLL,  9,                        0XF64C0000 },
    { XILCDO_NPI_BLK_DPLL, 10,                        0XF6530000 },
    { XILCDO_NPI_BLK_DPLL, 11,                        0XF65D0000 },
    { XILCDO_NPI_BLK_DPLL, 12,                        0XF68C0000 },
    { XILCDO_NPI_BLK_DPLL, 13,                        0XF6920000 },
    { XILCDO_NPI_BLK_DPLL, 14,                        0XF6960000 },
    { XILCDO_NPI_BLK_DPLL, 15,                        0XF69C0000 },
    { XILCDO_NPI_BLK_DPLL, 16,                        0XF6A00000 },
    { XILCDO_NPI_BLK_DPLL, 17,                        0XF6A60000 },
    { XILCDO_NPI_BLK_DPLL, 18,                        0XF6AB0000 },
    { XILCDO_NPI_BLK_DPLL, 19,                        0XF7120000 },
    { XILCDO_NPI_BLK_DPLL, 20,                        0XF7190000 },
    { XILCDO_NPI_BLK_DPLL, 21,                        0XF71E0000 },
    { XILCDO_NPI_BLK_DPLL, 22,                        0XF7250000 },
    { XILCDO_NPI_BLK_MMCM,  0,                        0XF6044000 },
    { XILCDO_NPI_BLK_MMCM,  1,                        0XF60B4000 },
    { XILCDO_NPI_BLK_MMCM,  2,                        0XF6184000 },
    { XILCDO_NPI_BLK_MMCM,  3,                        0XF61E4000 },
    { XILCDO_NPI_BLK_MMCM,  4,                        0XF6254000 },
    { XILCDO_NPI_BLK_MMCM,  5,                        0XF62F4000 },
    { XILCDO_NPI_BLK_MMCM,  6,                        0XF6354000 },
    { XILCDO_NPI_BLK_MMCM,  7,                        0XF63C4000 },
    { XILCDO_NPI_BLK_MMCM,  8,                        0XF6464000 },
    { XILCDO_NPI_BLK_MMCM,  9,                        0XF64C4000 },
    { XILCDO_NPI_BLK_MMCM, 10,                        0XF6534000 },
    { XILCDO_NPI_BLK_MMCM, 11,                        0XF65D4000 },
    { XILCDO_NPI_BLK_XPLL,  0,                        0XF6099000 },
    { XILCDO_NPI_BLK_XPLL,  1,                        0XF6099400 },
    { XILCDO_NPI_BLK_XPLL,  2,                        0XF6169000 },
    { XILCDO_NPI_BLK_XPLL,  3,                        0XF6169400 },
    { XILCDO_NPI_BLK_XPLL,  4,                        0XF61C9000 },
    { XILCDO_NPI_BLK_XPLL,  5,                        0XF61C9400 },
    { XILCDO_NPI_BLK_XPLL,  6,                        0XF6239000 },
    { XILCDO_NPI_BLK_XPLL,  7,                        0XF6239400 },
    { XILCDO_NPI_BLK_XPLL,  8,                        0XF62D9000 },
    { XILCDO_NPI_BLK_XPLL,  9,                        0XF62D9400 },
    { XILCDO_NPI_BLK_XPLL, 10,                        0XF6339000 },
    { XILCDO_NPI_BLK_XPLL, 11,                        0XF6339400 },
    { XILCDO_NPI_BLK_XPLL, 12,                        0XF63A9000 },
    { XILCDO_NPI_BLK_XPLL, 13,                        0XF63A9400 },
    { XILCDO_NPI_BLK_XPLL, 14,                        0XF6449000 },
    { XILCDO_NPI_BLK_XPLL, 15,                        0XF6449400 },
    { XILCDO_NPI_BLK_XPLL, 16,                        0XF64A9000 },
    { XILCDO_NPI_BLK_XPLL, 17,                        0XF64A9400 },
    { XILCDO_NPI_BLK_XPLL, 18,                        0XF6519000 },
    { XILCDO_NPI_BLK_XPLL, 19,                        0XF6519400 },
    { XILCDO_NPI_BLK_XPLL, 20,                        0XF65B9000 },
    { XILCDO_NPI_BLK_XPLL, 21,                        0XF65B9400 },
    { XILCDO_NPI_BLK_XPLL, 22,                        0XF6619000 },
    { XILCDO_NPI_BLK_XPLL, 23,                        0XF6619400 },
    { XILCDO_NPI_BLK_DDRMC_MAIN, 0,                       0XF6150000 },
    { XILCDO_NPI_BLK_DDRMC_MAIN, 1,                       0XF62C0000 },
    { XILCDO_NPI_BLK_DDRMC_MAIN, 2,                       0XF6430000 },
    { XILCDO_NPI_BLK_DDRMC_MAIN, 3,                       0XF65A0000 },
#if 0
    { DDRMC_NOC, 0,                        0XF6070000 },
    { DDRMC_NOC, 1,                        0XF6210000 },
    { DDRMC_NOC, 2,                        0XF6380000 },
    { DDRMC_NOC, 3,                        0XF64F0000 },
#endif
    { XILCDO_NPI_BLK_DDRMC, 0,                         0XF6110000 },
    { XILCDO_NPI_BLK_DDRMC, 1,                         0XF6280000 },
    { XILCDO_NPI_BLK_DDRMC, 2,                         0XF63F0000 },
    { XILCDO_NPI_BLK_DDRMC, 3,                         0XF6560000 },
    { XILCDO_NPI_BLK_GT,  0,                   0XF6890000 },
    { XILCDO_NPI_BLK_GT,  1,                   0XF68F0000 },
    { XILCDO_NPI_BLK_GT,  2,                   0XF6940000 },
    { XILCDO_NPI_BLK_GT,  3,                   0XF6990000 },
    { XILCDO_NPI_BLK_GT,  4,                   0XF69E0000 },
    { XILCDO_NPI_BLK_GT,  5,                   0XF6A30000 },
    { XILCDO_NPI_BLK_GT,  6,                   0XF6A90000 },
    { XILCDO_NPI_BLK_GT,  7,                   0XF70E0000 },
    { XILCDO_NPI_BLK_GT,  8,                   0XF7150000 },
    { XILCDO_NPI_BLK_GT,  9,                   0XF71B0000 },
    { XILCDO_NPI_BLK_GT, 10,                   0XF7210000 },
    { XILCDO_NPI_BLK_ME_NPI, 0,                           0XF70A0000 },
    { XILCDO_NPI_BLK_NOC_NCRB, 0,                         0XF6C70000 },
    { XILCDO_NPI_BLK_NOC_NCRB, 1,                         0XF6C80000 },
    { XILCDO_NPI_BLK_NOC_NCRB, 2,                         0XF6E40000 },
    { XILCDO_NPI_BLK_NOC_NCRB, 3,                         0XF6E50000 },
    { XILCDO_NPI_BLK_NOC_NCRB, 4,                         0XF7260000 },
    { XILCDO_NPI_BLK_NOC_NCRB, 5,                         0XF7270000 },
    { XILCDO_NPI_BLK_NOC_NMU,  0,                         0XF6010000 },
    { XILCDO_NPI_BLK_NOC_NMU,  1,                         0XF6012000 },
    { XILCDO_NPI_BLK_NOC_NMU,  2,                         0XF6014000 },
    { XILCDO_NPI_BLK_NOC_NMU,  3,                         0XF6016000 },
    { XILCDO_NPI_BLK_NOC_NMU,  4,                         0XF60E0000 },
    { XILCDO_NPI_BLK_NOC_NMU,  5,                         0XF60E2000 },
    { XILCDO_NPI_BLK_NOC_NMU,  6,                         0XF60F0000 },
    { XILCDO_NPI_BLK_NOC_NMU,  7,                         0XF60F2000 },
    { XILCDO_NPI_BLK_NOC_NMU,  8,                         0XF60F4000 },
    { XILCDO_NPI_BLK_NOC_NMU,  9,                         0XF60F6000 },
    { XILCDO_NPI_BLK_NOC_NMU, 10,                         0XF6620000 },
    { XILCDO_NPI_BLK_NOC_NMU, 11,                         0XF6650000 },
    { XILCDO_NPI_BLK_NOC_NMU, 12,                         0XF6680000 },
    { XILCDO_NPI_BLK_NOC_NMU, 13,                         0XF66A0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 14,                         0XF66D0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 15,                         0XF66F0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 16,                         0XF6720000 },
    { XILCDO_NPI_BLK_NOC_NMU, 17,                         0XF6790000 },
    { XILCDO_NPI_BLK_NOC_NMU, 18,                         0XF67C0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 19,                         0XF67E0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 20,                         0XF6810000 },
    { XILCDO_NPI_BLK_NOC_NMU, 21,                         0XF6830000 },
    { XILCDO_NPI_BLK_NOC_NMU, 22,                         0XF6860000 },
    { XILCDO_NPI_BLK_NOC_NMU, 23,                         0XF6AC0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 24,                         0XF6AF0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 25,                         0XF6B20000 },
    { XILCDO_NPI_BLK_NOC_NMU, 26,                         0XF6B40000 },
    { XILCDO_NPI_BLK_NOC_NMU, 27,                         0XF6B70000 },
    { XILCDO_NPI_BLK_NOC_NMU, 28,                         0XF6B90000 },
    { XILCDO_NPI_BLK_NOC_NMU, 29,                         0XF6BC0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 30,                         0XF6C30000 },
    { XILCDO_NPI_BLK_NOC_NMU, 31,                         0XF6C60000 },
    { XILCDO_NPI_BLK_NOC_NMU, 32,                         0XF6C90000 },
    { XILCDO_NPI_BLK_NOC_NMU, 33,                         0XF6CC0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 34,                         0XF6CF0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 35,                         0XF6D10000 },
    { XILCDO_NPI_BLK_NOC_NMU, 36,                         0XF6D40000 },
    { XILCDO_NPI_BLK_NOC_NMU, 37,                         0XF6D60000 },
    { XILCDO_NPI_BLK_NOC_NMU, 38,                         0XF6D90000 },
    { XILCDO_NPI_BLK_NOC_NMU, 39,                         0XF6E00000 },
    { XILCDO_NPI_BLK_NOC_NMU, 40,                         0XF6E30000 },
    { XILCDO_NPI_BLK_NOC_NMU, 41,                         0XF6E60000 },
    { XILCDO_NPI_BLK_NOC_NMU, 42,                         0XF6E90000 },
    { XILCDO_NPI_BLK_NOC_NMU, 43,                         0XF6EC0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 44,                         0XF6EE0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 45,                         0XF6F10000 },
    { XILCDO_NPI_BLK_NOC_NMU, 46,                         0XF6F30000 },
    { XILCDO_NPI_BLK_NOC_NMU, 47,                         0XF6F60000 },
    { XILCDO_NPI_BLK_NOC_NMU, 48,                         0XF6FD0000 },
    { XILCDO_NPI_BLK_NOC_NMU, 49,                         0XF7000000 },
    { XILCDO_NPI_BLK_NOC_NMU, 50,                         0XF7020000 },
    { XILCDO_NPI_BLK_NOC_NMU, 51,                         0XF7050000 },
    { XILCDO_NPI_BLK_NOC_NMU, 52,                         0XF7070000 },
    { XILCDO_NPI_BLK_NOC_NMU, 53,                         0XF70B0000 },
    { XILCDO_NPI_BLK_NOC_NPS,   0,                        0XF6030000 },
    { XILCDO_NPI_BLK_NOC_NPS,   1,                        0XF6032000 },
    { XILCDO_NPI_BLK_NOC_NPS,   2,                        0XF6060000 },
    { XILCDO_NPI_BLK_NOC_NPS,   3,                        0XF6062000 },
    { XILCDO_NPI_BLK_NOC_NPS,   4,                        0XF60A0000 },
    { XILCDO_NPI_BLK_NOC_NPS,   5,                        0XF60A2000 },
    { XILCDO_NPI_BLK_NOC_NPS,   6,                        0XF6100000 },
    { XILCDO_NPI_BLK_NOC_NPS,   7,                        0XF6102000 },
    { XILCDO_NPI_BLK_NOC_NPS,   8,                        0XF6170000 },
    { XILCDO_NPI_BLK_NOC_NPS,   9,                        0XF6172000 },
    { XILCDO_NPI_BLK_NOC_NPS,  10,                        0XF61B0000 },
    { XILCDO_NPI_BLK_NOC_NPS,  11,                        0XF61B2000 },
    { XILCDO_NPI_BLK_NOC_NPS,  12,                        0XF61D0000 },
    { XILCDO_NPI_BLK_NOC_NPS,  13,                        0XF61D2000 },
    { XILCDO_NPI_BLK_NOC_NPS,  14,                        0XF61D4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  15,                        0XF6200000 },
    { XILCDO_NPI_BLK_NOC_NPS,  16,                        0XF6202000 },
    { XILCDO_NPI_BLK_NOC_NPS,  17,                        0XF6204000 },
    { XILCDO_NPI_BLK_NOC_NPS,  18,                        0XF6240000 },
    { XILCDO_NPI_BLK_NOC_NPS,  19,                        0XF6242000 },
    { XILCDO_NPI_BLK_NOC_NPS,  20,                        0XF6244000 },
    { XILCDO_NPI_BLK_NOC_NPS,  21,                        0XF6270000 },
    { XILCDO_NPI_BLK_NOC_NPS,  22,                        0XF6272000 },
    { XILCDO_NPI_BLK_NOC_NPS,  23,                        0XF6274000 },
    { XILCDO_NPI_BLK_NOC_NPS,  24,                        0XF62E0000 },
    { XILCDO_NPI_BLK_NOC_NPS,  25,                        0XF62E2000 },
    { XILCDO_NPI_BLK_NOC_NPS,  26,                        0XF62E4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  27,                        0XF6320000 },
    { XILCDO_NPI_BLK_NOC_NPS,  28,                        0XF6322000 },
    { XILCDO_NPI_BLK_NOC_NPS,  29,                        0XF6324000 },
    { XILCDO_NPI_BLK_NOC_NPS,  30,                        0XF6340000 },
    { XILCDO_NPI_BLK_NOC_NPS,  31,                        0XF6342000 },
    { XILCDO_NPI_BLK_NOC_NPS,  32,                        0XF6344000 },
    { XILCDO_NPI_BLK_NOC_NPS,  33,                        0XF6370000 },
    { XILCDO_NPI_BLK_NOC_NPS,  34,                        0XF6372000 },
    { XILCDO_NPI_BLK_NOC_NPS,  35,                        0XF6374000 },
    { XILCDO_NPI_BLK_NOC_NPS,  36,                        0XF63B0000 },
    { XILCDO_NPI_BLK_NOC_NPS,  37,                        0XF63B2000 },
    { XILCDO_NPI_BLK_NOC_NPS,  38,                        0XF63B4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  39,                        0XF63E0000 },
    { XILCDO_NPI_BLK_NOC_NPS,  40,                        0XF63E2000 },
    { XILCDO_NPI_BLK_NOC_NPS,  41,                        0XF63E4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  42,                        0XF6450000 },
    { XILCDO_NPI_BLK_NOC_NPS,  43,                        0XF6452000 },
    { XILCDO_NPI_BLK_NOC_NPS,  44,                        0XF6454000 },
    { XILCDO_NPI_BLK_NOC_NPS,  45,                        0XF6490000 },
    { XILCDO_NPI_BLK_NOC_NPS,  46,                        0XF6492000 },
    { XILCDO_NPI_BLK_NOC_NPS,  47,                        0XF6494000 },
    { XILCDO_NPI_BLK_NOC_NPS,  48,                        0XF64B0000 },
    { XILCDO_NPI_BLK_NOC_NPS,  49,                        0XF64B2000 },
    { XILCDO_NPI_BLK_NOC_NPS,  50,                        0XF64B4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  51,                        0XF64E0000 },
    { XILCDO_NPI_BLK_NOC_NPS,  52,                        0XF64E2000 },
    { XILCDO_NPI_BLK_NOC_NPS,  53,                        0XF64E4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  54,                        0XF6520000 },
    { XILCDO_NPI_BLK_NOC_NPS,  55,                        0XF6522000 },
    { XILCDO_NPI_BLK_NOC_NPS,  56,                        0XF6524000 },
    { XILCDO_NPI_BLK_NOC_NPS,  57,                        0XF6550000 },
    { XILCDO_NPI_BLK_NOC_NPS,  58,                        0XF6552000 },
    { XILCDO_NPI_BLK_NOC_NPS,  59,                        0XF6554000 },
    { XILCDO_NPI_BLK_NOC_NPS,  60,                        0XF65C0000 },
    { XILCDO_NPI_BLK_NOC_NPS,  61,                        0XF65C2000 },
    { XILCDO_NPI_BLK_NOC_NPS,  62,                        0XF65C4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  63,                        0XF6600000 },
    { XILCDO_NPI_BLK_NOC_NPS,  64,                        0XF6602000 },
    { XILCDO_NPI_BLK_NOC_NPS,  65,                        0XF6604000 },
    { XILCDO_NPI_BLK_NOC_NPS,  66,                        0XF6624000 },
    { XILCDO_NPI_BLK_NOC_NPS,  67,                        0XF6626000 },
    { XILCDO_NPI_BLK_NOC_NPS,  68,                        0XF6654000 },
    { XILCDO_NPI_BLK_NOC_NPS,  69,                        0XF6656000 },
    { XILCDO_NPI_BLK_NOC_NPS,  70,                        0XF6684000 },
    { XILCDO_NPI_BLK_NOC_NPS,  71,                        0XF6686000 },
    { XILCDO_NPI_BLK_NOC_NPS,  72,                        0XF66A4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  73,                        0XF66A6000 },
    { XILCDO_NPI_BLK_NOC_NPS,  74,                        0XF66D4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  75,                        0XF66D6000 },
    { XILCDO_NPI_BLK_NOC_NPS,  76,                        0XF66F4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  77,                        0XF66F6000 },
    { XILCDO_NPI_BLK_NOC_NPS,  78,                        0XF6724000 },
    { XILCDO_NPI_BLK_NOC_NPS,  79,                        0XF6726000 },
    { XILCDO_NPI_BLK_NOC_NPS,  80,                        0XF6750000 },
    { XILCDO_NPI_BLK_NOC_NPS,  81,                        0XF6752000 },
    { XILCDO_NPI_BLK_NOC_NPS,  82,                        0XF6780000 },
    { XILCDO_NPI_BLK_NOC_NPS,  83,                        0XF6782000 },
    { XILCDO_NPI_BLK_NOC_NPS,  84,                        0XF67D0000 },
    { XILCDO_NPI_BLK_NOC_NPS,  85,                        0XF67D2000 },
    { XILCDO_NPI_BLK_NOC_NPS,  86,                        0XF6820000 },
    { XILCDO_NPI_BLK_NOC_NPS,  87,                        0XF6822000 },
    { XILCDO_NPI_BLK_NOC_NPS,  88,                        0XF6AC4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  89,                        0XF6AC6000 },
    { XILCDO_NPI_BLK_NOC_NPS,  90,                        0XF6AF4000 },
    { XILCDO_NPI_BLK_NOC_NPS,  91,                        0XF6AF6000 },
    { XILCDO_NPI_BLK_NOC_NPS,  92,                        0XF6B24000 },
    { XILCDO_NPI_BLK_NOC_NPS,  93,                        0XF6B26000 },
    { XILCDO_NPI_BLK_NOC_NPS,  94,                        0XF6B44000 },
    { XILCDO_NPI_BLK_NOC_NPS,  95,                        0XF6B46000 },
    { XILCDO_NPI_BLK_NOC_NPS,  96,                        0XF6B74000 },
    { XILCDO_NPI_BLK_NOC_NPS,  97,                        0XF6B76000 },
    { XILCDO_NPI_BLK_NOC_NPS,  98,                        0XF6B94000 },
    { XILCDO_NPI_BLK_NOC_NPS,  99,                        0XF6B96000 },
    { XILCDO_NPI_BLK_NOC_NPS, 100,                        0XF6BC4000 },
    { XILCDO_NPI_BLK_NOC_NPS, 101,                        0XF6BC6000 },
    { XILCDO_NPI_BLK_NOC_NPS, 102,                        0XF6BF0000 },
    { XILCDO_NPI_BLK_NOC_NPS, 103,                        0XF6BF2000 },
    { XILCDO_NPI_BLK_NOC_NPS, 104,                        0XF6C20000 },
    { XILCDO_NPI_BLK_NOC_NPS, 105,                        0XF6C22000 },
    { XILCDO_NPI_BLK_NOC_NPS, 106,                        0XF6C94000 },
    { XILCDO_NPI_BLK_NOC_NPS, 107,                        0XF6C96000 },
    { XILCDO_NPI_BLK_NOC_NPS, 108,                        0XF6CC4000 },
    { XILCDO_NPI_BLK_NOC_NPS, 109,                        0XF6CC6000 },
    { XILCDO_NPI_BLK_NOC_NPS, 110,                        0XF6CF4000 },
    { XILCDO_NPI_BLK_NOC_NPS, 111,                        0XF6CF6000 },
    { XILCDO_NPI_BLK_NOC_NPS, 112,                        0XF6D14000 },
    { XILCDO_NPI_BLK_NOC_NPS, 113,                        0XF6D16000 },
    { XILCDO_NPI_BLK_NOC_NPS, 114,                        0XF6D44000 },
    { XILCDO_NPI_BLK_NOC_NPS, 115,                        0XF6D46000 },
    { XILCDO_NPI_BLK_NOC_NPS, 116,                        0XF6D64000 },
    { XILCDO_NPI_BLK_NOC_NPS, 117,                        0XF6D66000 },
    { XILCDO_NPI_BLK_NOC_NPS, 118,                        0XF6D94000 },
    { XILCDO_NPI_BLK_NOC_NPS, 119,                        0XF6D96000 },
    { XILCDO_NPI_BLK_NOC_NPS, 120,                        0XF6DC0000 },
    { XILCDO_NPI_BLK_NOC_NPS, 121,                        0XF6DC2000 },
    { XILCDO_NPI_BLK_NOC_NPS, 122,                        0XF6DF0000 },
    { XILCDO_NPI_BLK_NOC_NPS, 123,                        0XF6DF2000 },
    { XILCDO_NPI_BLK_NOC_NPS, 124,                        0XF6E64000 },
    { XILCDO_NPI_BLK_NOC_NPS, 125,                        0XF6E66000 },
    { XILCDO_NPI_BLK_NOC_NPS, 126,                        0XF6E94000 },
    { XILCDO_NPI_BLK_NOC_NPS, 127,                        0XF6E96000 },
    { XILCDO_NPI_BLK_NOC_NPS, 128,                        0XF6EC4000 },
    { XILCDO_NPI_BLK_NOC_NPS, 129,                        0XF6EC6000 },
    { XILCDO_NPI_BLK_NOC_NPS, 130,                        0XF6EE4000 },
    { XILCDO_NPI_BLK_NOC_NPS, 131,                        0XF6EE6000 },
    { XILCDO_NPI_BLK_NOC_NPS, 132,                        0XF6F14000 },
    { XILCDO_NPI_BLK_NOC_NPS, 133,                        0XF6F16000 },
    { XILCDO_NPI_BLK_NOC_NPS, 134,                        0XF6F34000 },
    { XILCDO_NPI_BLK_NOC_NPS, 135,                        0XF6F36000 },
    { XILCDO_NPI_BLK_NOC_NPS, 136,                        0XF6F64000 },
    { XILCDO_NPI_BLK_NOC_NPS, 137,                        0XF6F66000 },
    { XILCDO_NPI_BLK_NOC_NPS, 138,                        0XF6F90000 },
    { XILCDO_NPI_BLK_NOC_NPS, 139,                        0XF6F92000 },
    { XILCDO_NPI_BLK_NOC_NPS, 140,                        0XF6FC0000 },
    { XILCDO_NPI_BLK_NOC_NPS, 141,                        0XF6FC2000 },
    { XILCDO_NPI_BLK_NOC_NPS, 142,                        0XF7010000 },
    { XILCDO_NPI_BLK_NOC_NPS, 143,                        0XF7012000 },
    { XILCDO_NPI_BLK_NOC_NPS, 144,                        0XF7060000 },
    { XILCDO_NPI_BLK_NOC_NPS, 145,                        0XF7062000 },
    { XILCDO_NPI_BLK_NOC_NSU,  0,                         0XF6020000 },
    { XILCDO_NPI_BLK_NOC_NSU,  1,                         0XF6022000 },
    { XILCDO_NPI_BLK_NOC_NSU,  2,                         0XF60D0000 },
    { XILCDO_NPI_BLK_NOC_NSU,  3,                         0XF60D2000 },
    { XILCDO_NPI_BLK_NOC_NSU,  4,                         0XF60E4000 },
    { XILCDO_NPI_BLK_NOC_NSU,  5,                         0XF60E6000 },
    { XILCDO_NPI_BLK_NOC_NSU,  6,                         0XF6622000 },
    { XILCDO_NPI_BLK_NOC_NSU,  7,                         0XF6652000 },
    { XILCDO_NPI_BLK_NOC_NSU,  8,                         0XF6682000 },
    { XILCDO_NPI_BLK_NOC_NSU,  9,                         0XF66A2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 10,                         0XF66D2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 11,                         0XF66F2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 12,                         0XF6722000 },
    { XILCDO_NPI_BLK_NOC_NSU, 13,                         0XF6792000 },
    { XILCDO_NPI_BLK_NOC_NSU, 14,                         0XF67C2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 15,                         0XF67E2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 16,                         0XF6812000 },
    { XILCDO_NPI_BLK_NOC_NSU, 17,                         0XF6832000 },
    { XILCDO_NPI_BLK_NOC_NSU, 18,                         0XF6862000 },
    { XILCDO_NPI_BLK_NOC_NSU, 19,                         0XF6AC2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 20,                         0XF6AF2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 21,                         0XF6B22000 },
    { XILCDO_NPI_BLK_NOC_NSU, 22,                         0XF6B42000 },
    { XILCDO_NPI_BLK_NOC_NSU, 23,                         0XF6B72000 },
    { XILCDO_NPI_BLK_NOC_NSU, 24,                         0XF6B92000 },
    { XILCDO_NPI_BLK_NOC_NSU, 25,                         0XF6BC2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 26,                         0XF6C32000 },
    { XILCDO_NPI_BLK_NOC_NSU, 27,                         0XF6C62000 },
    { XILCDO_NPI_BLK_NOC_NSU, 28,                         0XF6C92000 },
    { XILCDO_NPI_BLK_NOC_NSU, 29,                         0XF6CC2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 30,                         0XF6CF2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 31,                         0XF6D12000 },
    { XILCDO_NPI_BLK_NOC_NSU, 32,                         0XF6D42000 },
    { XILCDO_NPI_BLK_NOC_NSU, 33,                         0XF6D62000 },
    { XILCDO_NPI_BLK_NOC_NSU, 34,                         0XF6D92000 },
    { XILCDO_NPI_BLK_NOC_NSU, 35,                         0XF6E02000 },
    { XILCDO_NPI_BLK_NOC_NSU, 36,                         0XF6E32000 },
    { XILCDO_NPI_BLK_NOC_NSU, 37,                         0XF6E62000 },
    { XILCDO_NPI_BLK_NOC_NSU, 38,                         0XF6E92000 },
    { XILCDO_NPI_BLK_NOC_NSU, 39,                         0XF6EC2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 40,                         0XF6EE2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 41,                         0XF6F12000 },
    { XILCDO_NPI_BLK_NOC_NSU, 42,                         0XF6F32000 },
    { XILCDO_NPI_BLK_NOC_NSU, 43,                         0XF6F62000 },
    { XILCDO_NPI_BLK_NOC_NSU, 44,                         0XF6FD2000 },
    { XILCDO_NPI_BLK_NOC_NSU, 45,                         0XF7002000 },
    { XILCDO_NPI_BLK_NOC_NSU, 46,                         0XF7022000 },
    { XILCDO_NPI_BLK_NOC_NSU, 47,                         0XF7052000 },
    { XILCDO_NPI_BLK_NOC_NSU, 48,                         0XF7072000 },
    { XILCDO_NPI_BLK_NOC_NSU, 49,                         0XF70B2000 },
    { XILCDO_NPI_BLK_NOC_NIR,  0,                         0XF6000000 },
    { XILCDO_NPI_BLK_XPHY,   0,                   0XF6090000 },
    { XILCDO_NPI_BLK_XPHY,   1,                   0XF6091000 },
    { XILCDO_NPI_BLK_XPHY,   2,                   0XF6092000 },
    { XILCDO_NPI_BLK_XPHY,   3,                   0XF6093000 },
    { XILCDO_NPI_BLK_XPHY,   4,                   0XF6094000 },
    { XILCDO_NPI_BLK_XPHY,   5,                   0XF6095000 },
    { XILCDO_NPI_BLK_XPHY,   6,                   0XF6096000 },
    { XILCDO_NPI_BLK_XPHY,   7,                   0XF6097000 },
    { XILCDO_NPI_BLK_XPHY,   8,                   0XF6098000 },
    { XILCDO_NPI_BLK_XPHY,   9,                   0XF6160000 },
    { XILCDO_NPI_BLK_XPHY,  10,                   0XF6161000 },
    { XILCDO_NPI_BLK_XPHY,  11,                   0XF6162000 },
    { XILCDO_NPI_BLK_XPHY,  12,                   0XF6163000 },
    { XILCDO_NPI_BLK_XPHY,  13,                   0XF6164000 },
    { XILCDO_NPI_BLK_XPHY,  14,                   0XF6165000 },
    { XILCDO_NPI_BLK_XPHY,  15,                   0XF6166000 },
    { XILCDO_NPI_BLK_XPHY,  16,                   0XF6167000 },
    { XILCDO_NPI_BLK_XPHY,  17,                   0XF6168000 },
    { XILCDO_NPI_BLK_XPHY,  18,                   0XF61C0000 },
    { XILCDO_NPI_BLK_XPHY,  19,                   0XF61C1000 },
    { XILCDO_NPI_BLK_XPHY,  20,                   0XF61C2000 },
    { XILCDO_NPI_BLK_XPHY,  21,                   0XF61C3000 },
    { XILCDO_NPI_BLK_XPHY,  22,                   0XF61C4000 },
    { XILCDO_NPI_BLK_XPHY,  23,                   0XF61C5000 },
    { XILCDO_NPI_BLK_XPHY,  24,                   0XF61C6000 },
    { XILCDO_NPI_BLK_XPHY,  25,                   0XF61C7000 },
    { XILCDO_NPI_BLK_XPHY,  26,                   0XF61C8000 },
    { XILCDO_NPI_BLK_XPHY,  27,                   0XF6230000 },
    { XILCDO_NPI_BLK_XPHY,  28,                   0XF6231000 },
    { XILCDO_NPI_BLK_XPHY,  29,                   0XF6232000 },
    { XILCDO_NPI_BLK_XPHY,  30,                   0XF6233000 },
    { XILCDO_NPI_BLK_XPHY,  31,                   0XF6234000 },
    { XILCDO_NPI_BLK_XPHY,  32,                   0XF6235000 },
    { XILCDO_NPI_BLK_XPHY,  33,                   0XF6236000 },
    { XILCDO_NPI_BLK_XPHY,  34,                   0XF6237000 },
    { XILCDO_NPI_BLK_XPHY,  35,                   0XF6238000 },
    { XILCDO_NPI_BLK_XPHY,  36,                   0XF62D0000 },
    { XILCDO_NPI_BLK_XPHY,  37,                   0XF62D1000 },
    { XILCDO_NPI_BLK_XPHY,  38,                   0XF62D2000 },
    { XILCDO_NPI_BLK_XPHY,  39,                   0XF62D3000 },
    { XILCDO_NPI_BLK_XPHY,  40,                   0XF62D4000 },
    { XILCDO_NPI_BLK_XPHY,  41,                   0XF62D5000 },
    { XILCDO_NPI_BLK_XPHY,  42,                   0XF62D6000 },
    { XILCDO_NPI_BLK_XPHY,  43,                   0XF62D7000 },
    { XILCDO_NPI_BLK_XPHY,  44,                   0XF62D8000 },
    { XILCDO_NPI_BLK_XPHY,  45,                   0XF6330000 },
    { XILCDO_NPI_BLK_XPHY,  46,                   0XF6331000 },
    { XILCDO_NPI_BLK_XPHY,  47,                   0XF6332000 },
    { XILCDO_NPI_BLK_XPHY,  48,                   0XF6333000 },
    { XILCDO_NPI_BLK_XPHY,  49,                   0XF6334000 },
    { XILCDO_NPI_BLK_XPHY,  50,                   0XF6335000 },
    { XILCDO_NPI_BLK_XPHY,  51,                   0XF6336000 },
    { XILCDO_NPI_BLK_XPHY,  52,                   0XF6337000 },
    { XILCDO_NPI_BLK_XPHY,  53,                   0XF6338000 },
    { XILCDO_NPI_BLK_XPHY,  54,                   0XF63A0000 },
    { XILCDO_NPI_BLK_XPHY,  55,                   0XF63A1000 },
    { XILCDO_NPI_BLK_XPHY,  56,                   0XF63A2000 },
    { XILCDO_NPI_BLK_XPHY,  57,                   0XF63A3000 },
    { XILCDO_NPI_BLK_XPHY,  58,                   0XF63A4000 },
    { XILCDO_NPI_BLK_XPHY,  59,                   0XF63A5000 },
    { XILCDO_NPI_BLK_XPHY,  60,                   0XF63A6000 },
    { XILCDO_NPI_BLK_XPHY,  61,                   0XF63A7000 },
    { XILCDO_NPI_BLK_XPHY,  62,                   0XF63A8000 },
    { XILCDO_NPI_BLK_XPHY,  63,                   0XF6440000 },
    { XILCDO_NPI_BLK_XPHY,  64,                   0XF6441000 },
    { XILCDO_NPI_BLK_XPHY,  65,                   0XF6442000 },
    { XILCDO_NPI_BLK_XPHY,  66,                   0XF6443000 },
    { XILCDO_NPI_BLK_XPHY,  67,                   0XF6444000 },
    { XILCDO_NPI_BLK_XPHY,  68,                   0XF6445000 },
    { XILCDO_NPI_BLK_XPHY,  69,                   0XF6446000 },
    { XILCDO_NPI_BLK_XPHY,  70,                   0XF6447000 },
    { XILCDO_NPI_BLK_XPHY,  71,                   0XF6448000 },
    { XILCDO_NPI_BLK_XPHY,  72,                   0XF64A0000 },
    { XILCDO_NPI_BLK_XPHY,  73,                   0XF64A1000 },
    { XILCDO_NPI_BLK_XPHY,  74,                   0XF64A2000 },
    { XILCDO_NPI_BLK_XPHY,  75,                   0XF64A3000 },
    { XILCDO_NPI_BLK_XPHY,  76,                   0XF64A4000 },
    { XILCDO_NPI_BLK_XPHY,  77,                   0XF64A5000 },
    { XILCDO_NPI_BLK_XPHY,  78,                   0XF64A6000 },
    { XILCDO_NPI_BLK_XPHY,  79,                   0XF64A7000 },
    { XILCDO_NPI_BLK_XPHY,  80,                   0XF64A8000 },
    { XILCDO_NPI_BLK_XPHY,  81,                   0XF6510000 },
    { XILCDO_NPI_BLK_XPHY,  82,                   0XF6511000 },
    { XILCDO_NPI_BLK_XPHY,  83,                   0XF6512000 },
    { XILCDO_NPI_BLK_XPHY,  84,                   0XF6513000 },
    { XILCDO_NPI_BLK_XPHY,  85,                   0XF6514000 },
    { XILCDO_NPI_BLK_XPHY,  86,                   0XF6515000 },
    { XILCDO_NPI_BLK_XPHY,  87,                   0XF6516000 },
    { XILCDO_NPI_BLK_XPHY,  88,                   0XF6517000 },
    { XILCDO_NPI_BLK_XPHY,  89,                   0XF6518000 },
    { XILCDO_NPI_BLK_XPHY,  90,                   0XF65B0000 },
    { XILCDO_NPI_BLK_XPHY,  91,                   0XF65B1000 },
    { XILCDO_NPI_BLK_XPHY,  92,                   0XF65B2000 },
    { XILCDO_NPI_BLK_XPHY,  93,                   0XF65B3000 },
    { XILCDO_NPI_BLK_XPHY,  94,                   0XF65B4000 },
    { XILCDO_NPI_BLK_XPHY,  95,                   0XF65B5000 },
    { XILCDO_NPI_BLK_XPHY,  96,                   0XF65B6000 },
    { XILCDO_NPI_BLK_XPHY,  97,                   0XF65B7000 },
    { XILCDO_NPI_BLK_XPHY,  98,                   0XF65B8000 },
    { XILCDO_NPI_BLK_XPHY,  99,                   0XF6610000 },
    { XILCDO_NPI_BLK_XPHY, 100,                   0XF6611000 },
    { XILCDO_NPI_BLK_XPHY, 101,                   0XF6612000 },
    { XILCDO_NPI_BLK_XPHY, 102,                   0XF6613000 },
    { XILCDO_NPI_BLK_XPHY, 103,                   0XF6614000 },
    { XILCDO_NPI_BLK_XPHY, 104,                   0XF6615000 },
    { XILCDO_NPI_BLK_XPHY, 105,                   0XF6616000 },
    { XILCDO_NPI_BLK_XPHY, 106,                   0XF6617000 },
    { XILCDO_NPI_BLK_XPHY, 107,                   0XF6618000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC,  0,             0XF609A000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC,  1,             0XF616A000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC,  2,             0XF61CA000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC,  3,             0XF623A000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC,  4,             0XF62DA000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC,  5,             0XF633A000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC,  6,             0XF63AA000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC,  7,             0XF644A000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC,  8,             0XF64AA000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC,  9,             0XF651A000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC, 10,             0XF65BA000 },
    { XILCDO_NPI_BLK_XPIO_IOMISC, 11,             0XF661A000 },
    { XILCDO_NPI_BLK_XPIO,   0,                   0XF6090200 },
    { XILCDO_NPI_BLK_XPIO,   1,                   0XF6090400 },
    { XILCDO_NPI_BLK_XPIO,   2,                   0XF6090600 },
    { XILCDO_NPI_BLK_XPIO,   3,                   0XF6091200 },
    { XILCDO_NPI_BLK_XPIO,   4,                   0XF6091400 },
    { XILCDO_NPI_BLK_XPIO,   5,                   0XF6091600 },
    { XILCDO_NPI_BLK_XPIO,   6,                   0XF6092200 },
    { XILCDO_NPI_BLK_XPIO,   7,                   0XF6092400 },
    { XILCDO_NPI_BLK_XPIO,   8,                   0XF6092600 },
    { XILCDO_NPI_BLK_XPIO,   9,                   0XF6093200 },
    { XILCDO_NPI_BLK_XPIO,  10,                   0XF6093400 },
    { XILCDO_NPI_BLK_XPIO,  11,                   0XF6093600 },
    { XILCDO_NPI_BLK_XPIO,  12,                   0XF6094200 },
    { XILCDO_NPI_BLK_XPIO,  13,                   0XF6094400 },
    { XILCDO_NPI_BLK_XPIO,  14,                   0XF6094600 },
    { XILCDO_NPI_BLK_XPIO,  15,                   0XF6095200 },
    { XILCDO_NPI_BLK_XPIO,  16,                   0XF6095400 },
    { XILCDO_NPI_BLK_XPIO,  17,                   0XF6095600 },
    { XILCDO_NPI_BLK_XPIO,  18,                   0XF6096200 },
    { XILCDO_NPI_BLK_XPIO,  19,                   0XF6096400 },
    { XILCDO_NPI_BLK_XPIO,  20,                   0XF6096600 },
    { XILCDO_NPI_BLK_XPIO,  21,                   0XF6097200 },
    { XILCDO_NPI_BLK_XPIO,  22,                   0XF6097400 },
    { XILCDO_NPI_BLK_XPIO,  23,                   0XF6097600 },
    { XILCDO_NPI_BLK_XPIO,  24,                   0XF6098200 },
    { XILCDO_NPI_BLK_XPIO,  25,                   0XF6098400 },
    { XILCDO_NPI_BLK_XPIO,  26,                   0XF6098600 },
    { XILCDO_NPI_BLK_XPIO,  27,                   0XF6160200 },
    { XILCDO_NPI_BLK_XPIO,  28,                   0XF6160400 },
    { XILCDO_NPI_BLK_XPIO,  29,                   0XF6160600 },
    { XILCDO_NPI_BLK_XPIO,  30,                   0XF6161200 },
    { XILCDO_NPI_BLK_XPIO,  31,                   0XF6161400 },
    { XILCDO_NPI_BLK_XPIO,  32,                   0XF6161600 },
    { XILCDO_NPI_BLK_XPIO,  33,                   0XF6162200 },
    { XILCDO_NPI_BLK_XPIO,  34,                   0XF6162400 },
    { XILCDO_NPI_BLK_XPIO,  35,                   0XF6162600 },
    { XILCDO_NPI_BLK_XPIO,  36,                   0XF6163200 },
    { XILCDO_NPI_BLK_XPIO,  37,                   0XF6163400 },
    { XILCDO_NPI_BLK_XPIO,  38,                   0XF6163600 },
    { XILCDO_NPI_BLK_XPIO,  39,                   0XF6164200 },
    { XILCDO_NPI_BLK_XPIO,  40,                   0XF6164400 },
    { XILCDO_NPI_BLK_XPIO,  41,                   0XF6164600 },
    { XILCDO_NPI_BLK_XPIO,  42,                   0XF6165200 },
    { XILCDO_NPI_BLK_XPIO,  43,                   0XF6165400 },
    { XILCDO_NPI_BLK_XPIO,  44,                   0XF6165600 },
    { XILCDO_NPI_BLK_XPIO,  45,                   0XF6166200 },
    { XILCDO_NPI_BLK_XPIO,  46,                   0XF6166400 },
    { XILCDO_NPI_BLK_XPIO,  47,                   0XF6166600 },
    { XILCDO_NPI_BLK_XPIO,  48,                   0XF6167200 },
    { XILCDO_NPI_BLK_XPIO,  49,                   0XF6167400 },
    { XILCDO_NPI_BLK_XPIO,  50,                   0XF6167600 },
    { XILCDO_NPI_BLK_XPIO,  51,                   0XF6168200 },
    { XILCDO_NPI_BLK_XPIO,  52,                   0XF6168400 },
    { XILCDO_NPI_BLK_XPIO,  53,                   0XF6168600 },
    { XILCDO_NPI_BLK_XPIO,  54,                   0XF61C0200 },
    { XILCDO_NPI_BLK_XPIO,  55,                   0XF61C0400 },
    { XILCDO_NPI_BLK_XPIO,  56,                   0XF61C0600 },
    { XILCDO_NPI_BLK_XPIO,  57,                   0XF61C1200 },
    { XILCDO_NPI_BLK_XPIO,  58,                   0XF61C1400 },
    { XILCDO_NPI_BLK_XPIO,  59,                   0XF61C1600 },
    { XILCDO_NPI_BLK_XPIO,  60,                   0XF61C2200 },
    { XILCDO_NPI_BLK_XPIO,  61,                   0XF61C2400 },
    { XILCDO_NPI_BLK_XPIO,  62,                   0XF61C2600 },
    { XILCDO_NPI_BLK_XPIO,  63,                   0XF61C3200 },
    { XILCDO_NPI_BLK_XPIO,  64,                   0XF61C3400 },
    { XILCDO_NPI_BLK_XPIO,  65,                   0XF61C3600 },
    { XILCDO_NPI_BLK_XPIO,  66,                   0XF61C4200 },
    { XILCDO_NPI_BLK_XPIO,  67,                   0XF61C4400 },
    { XILCDO_NPI_BLK_XPIO,  68,                   0XF61C4600 },
    { XILCDO_NPI_BLK_XPIO,  69,                   0XF61C5200 },
    { XILCDO_NPI_BLK_XPIO,  70,                   0XF61C5400 },
    { XILCDO_NPI_BLK_XPIO,  71,                   0XF61C5600 },
    { XILCDO_NPI_BLK_XPIO,  72,                   0XF61C6200 },
    { XILCDO_NPI_BLK_XPIO,  73,                   0XF61C6400 },
    { XILCDO_NPI_BLK_XPIO,  74,                   0XF61C6600 },
    { XILCDO_NPI_BLK_XPIO,  75,                   0XF61C7200 },
    { XILCDO_NPI_BLK_XPIO,  76,                   0XF61C7400 },
    { XILCDO_NPI_BLK_XPIO,  77,                   0XF61C7600 },
    { XILCDO_NPI_BLK_XPIO,  78,                   0XF61C8200 },
    { XILCDO_NPI_BLK_XPIO,  79,                   0XF61C8400 },
    { XILCDO_NPI_BLK_XPIO,  80,                   0XF61C8600 },
    { XILCDO_NPI_BLK_XPIO,  81,                   0XF6230200 },
    { XILCDO_NPI_BLK_XPIO,  82,                   0XF6230400 },
    { XILCDO_NPI_BLK_XPIO,  83,                   0XF6230600 },
    { XILCDO_NPI_BLK_XPIO,  84,                   0XF6231200 },
    { XILCDO_NPI_BLK_XPIO,  85,                   0XF6231400 },
    { XILCDO_NPI_BLK_XPIO,  86,                   0XF6231600 },
    { XILCDO_NPI_BLK_XPIO,  87,                   0XF6232200 },
    { XILCDO_NPI_BLK_XPIO,  88,                   0XF6232400 },
    { XILCDO_NPI_BLK_XPIO,  89,                   0XF6232600 },
    { XILCDO_NPI_BLK_XPIO,  90,                   0XF6233200 },
    { XILCDO_NPI_BLK_XPIO,  91,                   0XF6233400 },
    { XILCDO_NPI_BLK_XPIO,  92,                   0XF6233600 },
    { XILCDO_NPI_BLK_XPIO,  93,                   0XF6234200 },
    { XILCDO_NPI_BLK_XPIO,  94,                   0XF6234400 },
    { XILCDO_NPI_BLK_XPIO,  95,                   0XF6234600 },
    { XILCDO_NPI_BLK_XPIO,  96,                   0XF6235200 },
    { XILCDO_NPI_BLK_XPIO,  97,                   0XF6235400 },
    { XILCDO_NPI_BLK_XPIO,  98,                   0XF6235600 },
    { XILCDO_NPI_BLK_XPIO,  99,                   0XF6236200 },
    { XILCDO_NPI_BLK_XPIO, 100,                   0XF6236400 },
    { XILCDO_NPI_BLK_XPIO, 101,                   0XF6236600 },
    { XILCDO_NPI_BLK_XPIO, 102,                   0XF6237200 },
    { XILCDO_NPI_BLK_XPIO, 103,                   0XF6237400 },
    { XILCDO_NPI_BLK_XPIO, 104,                   0XF6237600 },
    { XILCDO_NPI_BLK_XPIO, 105,                   0XF6238200 },
    { XILCDO_NPI_BLK_XPIO, 106,                   0XF6238400 },
    { XILCDO_NPI_BLK_XPIO, 107,                   0XF6238600 },
    { XILCDO_NPI_BLK_XPIO, 108,                   0XF62D0200 },
    { XILCDO_NPI_BLK_XPIO, 109,                   0XF62D0400 },
    { XILCDO_NPI_BLK_XPIO, 110,                   0XF62D0600 },
    { XILCDO_NPI_BLK_XPIO, 111,                   0XF62D1200 },
    { XILCDO_NPI_BLK_XPIO, 112,                   0XF62D1400 },
    { XILCDO_NPI_BLK_XPIO, 113,                   0XF62D1600 },
    { XILCDO_NPI_BLK_XPIO, 114,                   0XF62D2200 },
    { XILCDO_NPI_BLK_XPIO, 115,                   0XF62D2400 },
    { XILCDO_NPI_BLK_XPIO, 116,                   0XF62D2600 },
    { XILCDO_NPI_BLK_XPIO, 117,                   0XF62D3200 },
    { XILCDO_NPI_BLK_XPIO, 118,                   0XF62D3400 },
    { XILCDO_NPI_BLK_XPIO, 119,                   0XF62D3600 },
    { XILCDO_NPI_BLK_XPIO, 120,                   0XF62D4200 },
    { XILCDO_NPI_BLK_XPIO, 121,                   0XF62D4400 },
    { XILCDO_NPI_BLK_XPIO, 122,                   0XF62D4600 },
    { XILCDO_NPI_BLK_XPIO, 123,                   0XF62D5200 },
    { XILCDO_NPI_BLK_XPIO, 124,                   0XF62D5400 },
    { XILCDO_NPI_BLK_XPIO, 125,                   0XF62D5600 },
    { XILCDO_NPI_BLK_XPIO, 126,                   0XF62D6200 },
    { XILCDO_NPI_BLK_XPIO, 127,                   0XF62D6400 },
    { XILCDO_NPI_BLK_XPIO, 128,                   0XF62D6600 },
    { XILCDO_NPI_BLK_XPIO, 129,                   0XF62D7200 },
    { XILCDO_NPI_BLK_XPIO, 130,                   0XF62D7400 },
    { XILCDO_NPI_BLK_XPIO, 131,                   0XF62D7600 },
    { XILCDO_NPI_BLK_XPIO, 132,                   0XF62D8200 },
    { XILCDO_NPI_BLK_XPIO, 133,                   0XF62D8400 },
    { XILCDO_NPI_BLK_XPIO, 134,                   0XF62D8600 },
    { XILCDO_NPI_BLK_XPIO, 135,                   0XF6330200 },
    { XILCDO_NPI_BLK_XPIO, 136,                   0XF6330400 },
    { XILCDO_NPI_BLK_XPIO, 137,                   0XF6330600 },
    { XILCDO_NPI_BLK_XPIO, 138,                   0XF6331200 },
    { XILCDO_NPI_BLK_XPIO, 139,                   0XF6331400 },
    { XILCDO_NPI_BLK_XPIO, 140,                   0XF6331600 },
    { XILCDO_NPI_BLK_XPIO, 141,                   0XF6332200 },
    { XILCDO_NPI_BLK_XPIO, 142,                   0XF6332400 },
    { XILCDO_NPI_BLK_XPIO, 143,                   0XF6332600 },
    { XILCDO_NPI_BLK_XPIO, 144,                   0XF6333200 },
    { XILCDO_NPI_BLK_XPIO, 145,                   0XF6333400 },
    { XILCDO_NPI_BLK_XPIO, 146,                   0XF6333600 },
    { XILCDO_NPI_BLK_XPIO, 147,                   0XF6334200 },
    { XILCDO_NPI_BLK_XPIO, 148,                   0XF6334400 },
    { XILCDO_NPI_BLK_XPIO, 149,                   0XF6334600 },
    { XILCDO_NPI_BLK_XPIO, 150,                   0XF6335200 },
    { XILCDO_NPI_BLK_XPIO, 151,                   0XF6335400 },
    { XILCDO_NPI_BLK_XPIO, 152,                   0XF6335600 },
    { XILCDO_NPI_BLK_XPIO, 153,                   0XF6336200 },
    { XILCDO_NPI_BLK_XPIO, 154,                   0XF6336400 },
    { XILCDO_NPI_BLK_XPIO, 155,                   0XF6336600 },
    { XILCDO_NPI_BLK_XPIO, 156,                   0XF6337200 },
    { XILCDO_NPI_BLK_XPIO, 157,                   0XF6337400 },
    { XILCDO_NPI_BLK_XPIO, 158,                   0XF6337600 },
    { XILCDO_NPI_BLK_XPIO, 159,                   0XF6338200 },
    { XILCDO_NPI_BLK_XPIO, 160,                   0XF6338400 },
    { XILCDO_NPI_BLK_XPIO, 161,                   0XF6338600 },
    { XILCDO_NPI_BLK_XPIO, 162,                   0XF63A0200 },
    { XILCDO_NPI_BLK_XPIO, 163,                   0XF63A0400 },
    { XILCDO_NPI_BLK_XPIO, 164,                   0XF63A0600 },
    { XILCDO_NPI_BLK_XPIO, 165,                   0XF63A1200 },
    { XILCDO_NPI_BLK_XPIO, 166,                   0XF63A1400 },
    { XILCDO_NPI_BLK_XPIO, 167,                   0XF63A1600 },
    { XILCDO_NPI_BLK_XPIO, 168,                   0XF63A2200 },
    { XILCDO_NPI_BLK_XPIO, 169,                   0XF63A2400 },
    { XILCDO_NPI_BLK_XPIO, 170,                   0XF63A2600 },
    { XILCDO_NPI_BLK_XPIO, 171,                   0XF63A3200 },
    { XILCDO_NPI_BLK_XPIO, 172,                   0XF63A3400 },
    { XILCDO_NPI_BLK_XPIO, 173,                   0XF63A3600 },
    { XILCDO_NPI_BLK_XPIO, 174,                   0XF63A4200 },
    { XILCDO_NPI_BLK_XPIO, 175,                   0XF63A4400 },
    { XILCDO_NPI_BLK_XPIO, 176,                   0XF63A4600 },
    { XILCDO_NPI_BLK_XPIO, 177,                   0XF63A5200 },
    { XILCDO_NPI_BLK_XPIO, 178,                   0XF63A5400 },
    { XILCDO_NPI_BLK_XPIO, 179,                   0XF63A5600 },
    { XILCDO_NPI_BLK_XPIO, 180,                   0XF63A6200 },
    { XILCDO_NPI_BLK_XPIO, 181,                   0XF63A6400 },
    { XILCDO_NPI_BLK_XPIO, 182,                   0XF63A6600 },
    { XILCDO_NPI_BLK_XPIO, 183,                   0XF63A7200 },
    { XILCDO_NPI_BLK_XPIO, 184,                   0XF63A7400 },
    { XILCDO_NPI_BLK_XPIO, 185,                   0XF63A7600 },
    { XILCDO_NPI_BLK_XPIO, 186,                   0XF63A8200 },
    { XILCDO_NPI_BLK_XPIO, 187,                   0XF63A8400 },
    { XILCDO_NPI_BLK_XPIO, 188,                   0XF63A8600 },
    { XILCDO_NPI_BLK_XPIO, 189,                   0XF6440200 },
    { XILCDO_NPI_BLK_XPIO, 190,                   0XF6440400 },
    { XILCDO_NPI_BLK_XPIO, 191,                   0XF6440600 },
    { XILCDO_NPI_BLK_XPIO, 192,                   0XF6441200 },
    { XILCDO_NPI_BLK_XPIO, 193,                   0XF6441400 },
    { XILCDO_NPI_BLK_XPIO, 194,                   0XF6441600 },
    { XILCDO_NPI_BLK_XPIO, 195,                   0XF6442200 },
    { XILCDO_NPI_BLK_XPIO, 196,                   0XF6442400 },
    { XILCDO_NPI_BLK_XPIO, 197,                   0XF6442600 },
    { XILCDO_NPI_BLK_XPIO, 198,                   0XF6443200 },
    { XILCDO_NPI_BLK_XPIO, 199,                   0XF6443400 },
    { XILCDO_NPI_BLK_XPIO, 200,                   0XF6443600 },
    { XILCDO_NPI_BLK_XPIO, 201,                   0XF6444200 },
    { XILCDO_NPI_BLK_XPIO, 202,                   0XF6444400 },
    { XILCDO_NPI_BLK_XPIO, 203,                   0XF6444600 },
    { XILCDO_NPI_BLK_XPIO, 204,                   0XF6445200 },
    { XILCDO_NPI_BLK_XPIO, 205,                   0XF6445400 },
    { XILCDO_NPI_BLK_XPIO, 206,                   0XF6445600 },
    { XILCDO_NPI_BLK_XPIO, 207,                   0XF6446200 },
    { XILCDO_NPI_BLK_XPIO, 208,                   0XF6446400 },
    { XILCDO_NPI_BLK_XPIO, 209,                   0XF6446600 },
    { XILCDO_NPI_BLK_XPIO, 210,                   0XF6447200 },
    { XILCDO_NPI_BLK_XPIO, 211,                   0XF6447400 },
    { XILCDO_NPI_BLK_XPIO, 212,                   0XF6447600 },
    { XILCDO_NPI_BLK_XPIO, 213,                   0XF6448200 },
    { XILCDO_NPI_BLK_XPIO, 214,                   0XF6448400 },
    { XILCDO_NPI_BLK_XPIO, 215,                   0XF6448600 },
    { XILCDO_NPI_BLK_XPIO, 216,                   0XF64A0200 },
    { XILCDO_NPI_BLK_XPIO, 217,                   0XF64A0400 },
    { XILCDO_NPI_BLK_XPIO, 218,                   0XF64A0600 },
    { XILCDO_NPI_BLK_XPIO, 219,                   0XF64A1200 },
    { XILCDO_NPI_BLK_XPIO, 220,                   0XF64A1400 },
    { XILCDO_NPI_BLK_XPIO, 221,                   0XF64A1600 },
    { XILCDO_NPI_BLK_XPIO, 222,                   0XF64A2200 },
    { XILCDO_NPI_BLK_XPIO, 223,                   0XF64A2400 },
    { XILCDO_NPI_BLK_XPIO, 224,                   0XF64A2600 },
    { XILCDO_NPI_BLK_XPIO, 225,                   0XF64A3200 },
    { XILCDO_NPI_BLK_XPIO, 226,                   0XF64A3400 },
    { XILCDO_NPI_BLK_XPIO, 227,                   0XF64A3600 },
    { XILCDO_NPI_BLK_XPIO, 228,                   0XF64A4200 },
    { XILCDO_NPI_BLK_XPIO, 229,                   0XF64A4400 },
    { XILCDO_NPI_BLK_XPIO, 230,                   0XF64A4600 },
    { XILCDO_NPI_BLK_XPIO, 231,                   0XF64A5200 },
    { XILCDO_NPI_BLK_XPIO, 232,                   0XF64A5400 },
    { XILCDO_NPI_BLK_XPIO, 233,                   0XF64A5600 },
    { XILCDO_NPI_BLK_XPIO, 234,                   0XF64A6200 },
    { XILCDO_NPI_BLK_XPIO, 235,                   0XF64A6400 },
    { XILCDO_NPI_BLK_XPIO, 236,                   0XF64A6600 },
    { XILCDO_NPI_BLK_XPIO, 237,                   0XF64A7200 },
    { XILCDO_NPI_BLK_XPIO, 238,                   0XF64A7400 },
    { XILCDO_NPI_BLK_XPIO, 239,                   0XF64A7600 },
    { XILCDO_NPI_BLK_XPIO, 240,                   0XF64A8200 },
    { XILCDO_NPI_BLK_XPIO, 241,                   0XF64A8400 },
    { XILCDO_NPI_BLK_XPIO, 242,                   0XF64A8600 },
    { XILCDO_NPI_BLK_XPIO, 243,                   0XF6510200 },
    { XILCDO_NPI_BLK_XPIO, 244,                   0XF6510400 },
    { XILCDO_NPI_BLK_XPIO, 245,                   0XF6510600 },
    { XILCDO_NPI_BLK_XPIO, 246,                   0XF6511200 },
    { XILCDO_NPI_BLK_XPIO, 247,                   0XF6511400 },
    { XILCDO_NPI_BLK_XPIO, 248,                   0XF6511600 },
    { XILCDO_NPI_BLK_XPIO, 249,                   0XF6512200 },
    { XILCDO_NPI_BLK_XPIO, 250,                   0XF6512400 },
    { XILCDO_NPI_BLK_XPIO, 251,                   0XF6512600 },
    { XILCDO_NPI_BLK_XPIO, 252,                   0XF6513200 },
    { XILCDO_NPI_BLK_XPIO, 253,                   0XF6513400 },
    { XILCDO_NPI_BLK_XPIO, 254,                   0XF6513600 },
    { XILCDO_NPI_BLK_XPIO, 255,                   0XF6514200 },
    { XILCDO_NPI_BLK_XPIO, 256,                   0XF6514400 },
    { XILCDO_NPI_BLK_XPIO, 257,                   0XF6514600 },
    { XILCDO_NPI_BLK_XPIO, 258,                   0XF6515200 },
    { XILCDO_NPI_BLK_XPIO, 259,                   0XF6515400 },
    { XILCDO_NPI_BLK_XPIO, 260,                   0XF6515600 },
    { XILCDO_NPI_BLK_XPIO, 261,                   0XF6516200 },
    { XILCDO_NPI_BLK_XPIO, 262,                   0XF6516400 },
    { XILCDO_NPI_BLK_XPIO, 263,                   0XF6516600 },
    { XILCDO_NPI_BLK_XPIO, 264,                   0XF6517200 },
    { XILCDO_NPI_BLK_XPIO, 265,                   0XF6517400 },
    { XILCDO_NPI_BLK_XPIO, 266,                   0XF6517600 },
    { XILCDO_NPI_BLK_XPIO, 267,                   0XF6518200 },
    { XILCDO_NPI_BLK_XPIO, 268,                   0XF6518400 },
    { XILCDO_NPI_BLK_XPIO, 269,                   0XF6518600 },
    { XILCDO_NPI_BLK_XPIO, 270,                   0XF65B0200 },
    { XILCDO_NPI_BLK_XPIO, 271,                   0XF65B0400 },
    { XILCDO_NPI_BLK_XPIO, 272,                   0XF65B0600 },
    { XILCDO_NPI_BLK_XPIO, 273,                   0XF65B1200 },
    { XILCDO_NPI_BLK_XPIO, 274,                   0XF65B1400 },
    { XILCDO_NPI_BLK_XPIO, 275,                   0XF65B1600 },
    { XILCDO_NPI_BLK_XPIO, 276,                   0XF65B2200 },
    { XILCDO_NPI_BLK_XPIO, 277,                   0XF65B2400 },
    { XILCDO_NPI_BLK_XPIO, 278,                   0XF65B2600 },
    { XILCDO_NPI_BLK_XPIO, 279,                   0XF65B3200 },
    { XILCDO_NPI_BLK_XPIO, 280,                   0XF65B3400 },
    { XILCDO_NPI_BLK_XPIO, 281,                   0XF65B3600 },
    { XILCDO_NPI_BLK_XPIO, 282,                   0XF65B4200 },
    { XILCDO_NPI_BLK_XPIO, 283,                   0XF65B4400 },
    { XILCDO_NPI_BLK_XPIO, 284,                   0XF65B4600 },
    { XILCDO_NPI_BLK_XPIO, 285,                   0XF65B5200 },
    { XILCDO_NPI_BLK_XPIO, 286,                   0XF65B5400 },
    { XILCDO_NPI_BLK_XPIO, 287,                   0XF65B5600 },
    { XILCDO_NPI_BLK_XPIO, 288,                   0XF65B6200 },
    { XILCDO_NPI_BLK_XPIO, 289,                   0XF65B6400 },
    { XILCDO_NPI_BLK_XPIO, 290,                   0XF65B6600 },
    { XILCDO_NPI_BLK_XPIO, 291,                   0XF65B7200 },
    { XILCDO_NPI_BLK_XPIO, 292,                   0XF65B7400 },
    { XILCDO_NPI_BLK_XPIO, 293,                   0XF65B7600 },
    { XILCDO_NPI_BLK_XPIO, 294,                   0XF65B8200 },
    { XILCDO_NPI_BLK_XPIO, 295,                   0XF65B8400 },
    { XILCDO_NPI_BLK_XPIO, 296,                   0XF65B8600 },
    { XILCDO_NPI_BLK_XPIO, 297,                   0XF6610200 },
    { XILCDO_NPI_BLK_XPIO, 298,                   0XF6610400 },
    { XILCDO_NPI_BLK_XPIO, 299,                   0XF6610600 },
    { XILCDO_NPI_BLK_XPIO, 300,                   0XF6611200 },
    { XILCDO_NPI_BLK_XPIO, 301,                   0XF6611400 },
    { XILCDO_NPI_BLK_XPIO, 302,                   0XF6611600 },
    { XILCDO_NPI_BLK_XPIO, 303,                   0XF6612200 },
    { XILCDO_NPI_BLK_XPIO, 304,                   0XF6612400 },
    { XILCDO_NPI_BLK_XPIO, 305,                   0XF6612600 },
    { XILCDO_NPI_BLK_XPIO, 306,                   0XF6613200 },
    { XILCDO_NPI_BLK_XPIO, 307,                   0XF6613400 },
    { XILCDO_NPI_BLK_XPIO, 308,                   0XF6613600 },
    { XILCDO_NPI_BLK_XPIO, 309,                   0XF6614200 },
    { XILCDO_NPI_BLK_XPIO, 310,                   0XF6614400 },
    { XILCDO_NPI_BLK_XPIO, 311,                   0XF6614600 },
    { XILCDO_NPI_BLK_XPIO, 312,                   0XF6615200 },
    { XILCDO_NPI_BLK_XPIO, 313,                   0XF6615400 },
    { XILCDO_NPI_BLK_XPIO, 314,                   0XF6615600 },
    { XILCDO_NPI_BLK_XPIO, 315,                   0XF6616200 },
    { XILCDO_NPI_BLK_XPIO, 316,                   0XF6616400 },
    { XILCDO_NPI_BLK_XPIO, 317,                   0XF6616600 },
    { XILCDO_NPI_BLK_XPIO, 318,                   0XF6617200 },
    { XILCDO_NPI_BLK_XPIO, 319,                   0XF6617400 },
    { XILCDO_NPI_BLK_XPIO, 320,                   0XF6617600 },
    { XILCDO_NPI_BLK_XPIO, 321,                   0XF6618200 },
    { XILCDO_NPI_BLK_XPIO, 322,                   0XF6618400 },
    { XILCDO_NPI_BLK_XPIO, 323,                   0XF6618600 },
    { XILCDO_NPI_BLK_VREF,   0,                   0XF6090800 },
    { XILCDO_NPI_BLK_VREF,   1,                   0XF6091800 },
    { XILCDO_NPI_BLK_VREF,   2,                   0XF6092800 },
    { XILCDO_NPI_BLK_VREF,   3,                   0XF6093800 },
    { XILCDO_NPI_BLK_VREF,   4,                   0XF6094800 },
    { XILCDO_NPI_BLK_VREF,   5,                   0XF6095800 },
    { XILCDO_NPI_BLK_VREF,   6,                   0XF6096800 },
    { XILCDO_NPI_BLK_VREF,   7,                   0XF6097800 },
    { XILCDO_NPI_BLK_VREF,   8,                   0XF6098800 },
    { XILCDO_NPI_BLK_VREF,   9,                   0XF6160800 },
    { XILCDO_NPI_BLK_VREF,  10,                   0XF6161800 },
    { XILCDO_NPI_BLK_VREF,  11,                   0XF6162800 },
    { XILCDO_NPI_BLK_VREF,  12,                   0XF6163800 },
    { XILCDO_NPI_BLK_VREF,  13,                   0XF6164800 },
    { XILCDO_NPI_BLK_VREF,  14,                   0XF6165800 },
    { XILCDO_NPI_BLK_VREF,  15,                   0XF6166800 },
    { XILCDO_NPI_BLK_VREF,  16,                   0XF6167800 },
    { XILCDO_NPI_BLK_VREF,  17,                   0XF6168800 },
    { XILCDO_NPI_BLK_VREF,  18,                   0XF61C0800 },
    { XILCDO_NPI_BLK_VREF,  19,                   0XF61C1800 },
    { XILCDO_NPI_BLK_VREF,  20,                   0XF61C2800 },
    { XILCDO_NPI_BLK_VREF,  21,                   0XF61C3800 },
    { XILCDO_NPI_BLK_VREF,  22,                   0XF61C4800 },
    { XILCDO_NPI_BLK_VREF,  23,                   0XF61C5800 },
    { XILCDO_NPI_BLK_VREF,  24,                   0XF61C6800 },
    { XILCDO_NPI_BLK_VREF,  25,                   0XF61C7800 },
    { XILCDO_NPI_BLK_VREF,  26,                   0XF61C8800 },
    { XILCDO_NPI_BLK_VREF,  27,                   0XF6230800 },
    { XILCDO_NPI_BLK_VREF,  28,                   0XF6231800 },
    { XILCDO_NPI_BLK_VREF,  29,                   0XF6232800 },
    { XILCDO_NPI_BLK_VREF,  30,                   0XF6233800 },
    { XILCDO_NPI_BLK_VREF,  31,                   0XF6234800 },
    { XILCDO_NPI_BLK_VREF,  32,                   0XF6235800 },
    { XILCDO_NPI_BLK_VREF,  33,                   0XF6236800 },
    { XILCDO_NPI_BLK_VREF,  34,                   0XF6237800 },
    { XILCDO_NPI_BLK_VREF,  35,                   0XF6238800 },
    { XILCDO_NPI_BLK_VREF,  36,                   0XF62D0800 },
    { XILCDO_NPI_BLK_VREF,  37,                   0XF62D1800 },
    { XILCDO_NPI_BLK_VREF,  38,                   0XF62D2800 },
    { XILCDO_NPI_BLK_VREF,  39,                   0XF62D3800 },
    { XILCDO_NPI_BLK_VREF,  40,                   0XF62D4800 },
    { XILCDO_NPI_BLK_VREF,  41,                   0XF62D5800 },
    { XILCDO_NPI_BLK_VREF,  42,                   0XF62D6800 },
    { XILCDO_NPI_BLK_VREF,  43,                   0XF62D7800 },
    { XILCDO_NPI_BLK_VREF,  44,                   0XF62D8800 },
    { XILCDO_NPI_BLK_VREF,  45,                   0XF6330800 },
    { XILCDO_NPI_BLK_VREF,  46,                   0XF6331800 },
    { XILCDO_NPI_BLK_VREF,  47,                   0XF6332800 },
    { XILCDO_NPI_BLK_VREF,  48,                   0XF6333800 },
    { XILCDO_NPI_BLK_VREF,  49,                   0XF6334800 },
    { XILCDO_NPI_BLK_VREF,  50,                   0XF6335800 },
    { XILCDO_NPI_BLK_VREF,  51,                   0XF6336800 },
    { XILCDO_NPI_BLK_VREF,  52,                   0XF6337800 },
    { XILCDO_NPI_BLK_VREF,  53,                   0XF6338800 },
    { XILCDO_NPI_BLK_VREF,  54,                   0XF63A0800 },
    { XILCDO_NPI_BLK_VREF,  55,                   0XF63A1800 },
    { XILCDO_NPI_BLK_VREF,  56,                   0XF63A2800 },
    { XILCDO_NPI_BLK_VREF,  57,                   0XF63A3800 },
    { XILCDO_NPI_BLK_VREF,  58,                   0XF63A4800 },
    { XILCDO_NPI_BLK_VREF,  59,                   0XF63A5800 },
    { XILCDO_NPI_BLK_VREF,  60,                   0XF63A6800 },
    { XILCDO_NPI_BLK_VREF,  61,                   0XF63A7800 },
    { XILCDO_NPI_BLK_VREF,  62,                   0XF63A8800 },
    { XILCDO_NPI_BLK_VREF,  63,                   0XF6440800 },
    { XILCDO_NPI_BLK_VREF,  64,                   0XF6441800 },
    { XILCDO_NPI_BLK_VREF,  65,                   0XF6442800 },
    { XILCDO_NPI_BLK_VREF,  66,                   0XF6443800 },
    { XILCDO_NPI_BLK_VREF,  67,                   0XF6444800 },
    { XILCDO_NPI_BLK_VREF,  68,                   0XF6445800 },
    { XILCDO_NPI_BLK_VREF,  69,                   0XF6446800 },
    { XILCDO_NPI_BLK_VREF,  70,                   0XF6447800 },
    { XILCDO_NPI_BLK_VREF,  71,                   0XF6448800 },
    { XILCDO_NPI_BLK_VREF,  72,                   0XF64A0800 },
    { XILCDO_NPI_BLK_VREF,  73,                   0XF64A1800 },
    { XILCDO_NPI_BLK_VREF,  74,                   0XF64A2800 },
    { XILCDO_NPI_BLK_VREF,  75,                   0XF64A3800 },
    { XILCDO_NPI_BLK_VREF,  76,                   0XF64A4800 },
    { XILCDO_NPI_BLK_VREF,  77,                   0XF64A5800 },
    { XILCDO_NPI_BLK_VREF,  78,                   0XF64A6800 },
    { XILCDO_NPI_BLK_VREF,  79,                   0XF64A7800 },
    { XILCDO_NPI_BLK_VREF,  80,                   0XF64A8800 },
    { XILCDO_NPI_BLK_VREF,  81,                   0XF6510800 },
    { XILCDO_NPI_BLK_VREF,  82,                   0XF6511800 },
    { XILCDO_NPI_BLK_VREF,  83,                   0XF6512800 },
    { XILCDO_NPI_BLK_VREF,  84,                   0XF6513800 },
    { XILCDO_NPI_BLK_VREF,  85,                   0XF6514800 },
    { XILCDO_NPI_BLK_VREF,  86,                   0XF6515800 },
    { XILCDO_NPI_BLK_VREF,  87,                   0XF6516800 },
    { XILCDO_NPI_BLK_VREF,  88,                   0XF6517800 },
    { XILCDO_NPI_BLK_VREF,  89,                   0XF6518800 },
    { XILCDO_NPI_BLK_VREF,  90,                   0XF65B0800 },
    { XILCDO_NPI_BLK_VREF,  91,                   0XF65B1800 },
    { XILCDO_NPI_BLK_VREF,  92,                   0XF65B2800 },
    { XILCDO_NPI_BLK_VREF,  93,                   0XF65B3800 },
    { XILCDO_NPI_BLK_VREF,  94,                   0XF65B4800 },
    { XILCDO_NPI_BLK_VREF,  95,                   0XF65B5800 },
    { XILCDO_NPI_BLK_VREF,  96,                   0XF65B6800 },
    { XILCDO_NPI_BLK_VREF,  97,                   0XF65B7800 },
    { XILCDO_NPI_BLK_VREF,  98,                   0XF65B8800 },
    { XILCDO_NPI_BLK_VREF,  99,                   0XF6610800 },
    { XILCDO_NPI_BLK_VREF, 100,                   0XF6611800 },
    { XILCDO_NPI_BLK_VREF, 101,                   0XF6612800 },
    { XILCDO_NPI_BLK_VREF, 102,                   0XF6613800 },
    { XILCDO_NPI_BLK_VREF, 103,                   0XF6614800 },
    { XILCDO_NPI_BLK_VREF, 104,                   0XF6615800 },
    { XILCDO_NPI_BLK_VREF, 105,                   0XF6616800 },
    { XILCDO_NPI_BLK_VREF, 106,                   0XF6617800 },
    { XILCDO_NPI_BLK_VREF, 107,                   0XF6618800 },
    { XILCDO_NPI_BLK_XPIPE, 0,                    0XF7100000 },
    { XILCDO_NPI_BLK_XPIPE, 1,                    0XF7170000 },
    { XILCDO_NPI_BLK_XPIPE, 2,                    0XF71D0000 },
    { XILCDO_NPI_BLK_XPIPE, 3,                    0XF7230000 },
    { 0, 0, 0 }
};

void set_cfu_stream_keyhole_size(uint64_t size) {
    if (size < 4 || size > 256*1024 || (size & 3) != 0) {
        fprintf(stderr, "invalid cfu stream keyhole size %#"PRIx64"\n", size);
        exit(1);
    }
    cfu_stream_keyhole_size = size;
}

static NpiModule * find_npi_module_addr(uint32_t addr) {
    NpiModule * m = s80_npi_modules;
    while (m->address != 0) {
        if (m->address == addr) return m;
        m++;
    }
    return NULL;
}

static enum {
    PlatformHW,
    PlatformSPP
} platform = PlatformHW;

int is_platform_spp(void) {
    return platform == PlatformSPP;
}

void set_platform_spp(void) {
    platform = PlatformSPP;
}

void set_spp_ddr_enable(uint32_t enable) {
    spp_ddr_ena = enable;
}

static uint32_t pl_bypass;
static uint32_t houseclean_bypass;
static uint32_t sysmon_bypass;

typedef enum {
    PpModeInvalid,
    PpMode1,
    PpMode2,
    PpMode3,
    PpMode4,
    PpModeCfiWrite,
    PpModeCfiDmaWrite,
    PpModeCfiDmaWriteKeyhole,
    PpModeIsoAll,
    PpModeSppDdr,
    PpModeSppDdr2,
    PpModeSppNoDdr,
    PpModePlClkNone,
    PpModePlRstNone,
    PpModePlRstAll,
    PpModeDdrHack,
    PpModeDciCalWaitOff,
    PpModeDciCalWaitOn,
    PpModePllCalWaitOn,
    PpModeInitFinishMoveOn,
    PpModeSeqSpec0,
    PpModeSeqSpec1,
    PpModeSeqSpec2,
    PpModeSeqSpec3,
    PpModeSeqSpec4,
    PpModeSsitMaster1,
    PpModeSsitMaster2,
    PpModeSsitMaster3,
    PpModeSsitSlave,
    PpModeNpiAll,
    PpModeNpiNone,
    PpModePlBypass,
    PpModePcsrNormal,
    PpModePcsrSeparate,
    PpModeAlignOn,
    PpModeAlignOff,
    PpModeDdrCalErr0,
    PpModeDdrCalErr1,
    PpModeDdrCalErr2,
    PpModeSysmonBypass,
} PpMode;

struct {
    const char * name;
    PpMode mode;
} ppModeList[] = {
    { "1",                       PpMode1                  },
    { "2",                       PpMode2                  },
    { "3",                       PpMode3                  },
    { "4",                       PpMode4                  },
    { "cfi-write",               PpModeCfiWrite           },
    { "cfi-dma-write",           PpModeCfiDmaWrite        },
    { "cfi-dma-write-keyhole",   PpModeCfiDmaWriteKeyhole },
    { "iso-all",                 PpModeIsoAll             },
    { "spp-ddr",                 PpModeSppDdr             },
    { "spp-ddr-2",               PpModeSppDdr2            },
    { "spp-no-ddr",              PpModeSppNoDdr           },
    { "pl-clk-none",             PpModePlClkNone          },
    { "pl-rst-none",             PpModePlRstNone          },
    { "pl-rst-all",              PpModePlRstAll           },
    { "ddr-hack",                PpModeDdrHack            },
    { "dci-cal-wait-off",        PpModeDciCalWaitOff      },
    { "dci-cal-wait-on",         PpModeDciCalWaitOn       },
    { "pll-cal-wait-on",         PpModePllCalWaitOn       },
    { "init-finish-move-on",     PpModeInitFinishMoveOn   },
    { "seq-spec-0",              PpModeSeqSpec0           },
    { "seq-spec-1",              PpModeSeqSpec1           },
    { "seq-spec-2",              PpModeSeqSpec2           },
    { "seq-spec-3",              PpModeSeqSpec3           },
    { "seq-spec-4",              PpModeSeqSpec4           },
    { "ssit-master-1",           PpModeSsitMaster1        },
    { "ssit-master-2",           PpModeSsitMaster2        },
    { "ssit-master-3",           PpModeSsitMaster3        },
    { "ssit-slave",              PpModeSsitSlave          },
    { "npi-all",                 PpModeNpiAll             },
    { "npi-none",                PpModeNpiNone            },
    { "pl-bypass",               PpModePlBypass           },
    { "pcsr-normal",             PpModePcsrNormal         },
    { "pcsr-separate",           PpModePcsrSeparate       },
    { "align-on",                PpModeAlignOn            },
    { "align-off",               PpModeAlignOff           },
    { "ddrmc-cal-err-0",         PpModeDdrCalErr0         },
    { "ddrmc-cal-err-1",         PpModeDdrCalErr1         },
    { "ddrmc-cal-err-2",         PpModeDdrCalErr2         },
    { "sysmon-bypass",           PpModeSysmonBypass       },
    { NULL,                      PpModeInvalid            },
};

int cdocmd_post_process_mode(const char * s, uint32_t verbose) {
    if (s == NULL || strcmp(s, "") == 0 ||
        strcmp(s, "0") == 0 || strcmp(s, "false") == 0) {
        return 0;
    }
    for (;;) {
        const char * p = s;
        uint32_t i;
        while (*p != '\0' && *p != ',') p++;
        for (i = 0; ppModeList[i].name != NULL; i++) {
            const char * name = ppModeList[i].name;
            size_t len = strlen(name);
            if (len == p-s && memcmp(s, name, len) == 0) break;
        }
        if (ppModeList[i].name == NULL) {
            fprintf(stderr, "invalid post processing mode: %s\n", s);
            fprintf(stderr, "valid modes are:");
            for (i = 0; ppModeList[i].name != NULL; i++) {
                const char * name = ppModeList[i].name;
                if (i != 0) {
                    fprintf(stderr, ",");
                }
                fprintf(stderr, " %s", name);
            }
            fprintf(stderr, "\n");
            exit(1);

        }
        switch (ppModeList[i].mode) {
        case PpMode1:
            break;

        case PpMode2:
            houseclean_bypass = (houseclean_bypass_lpd |
                                 houseclean_bypass_fpd |
                                 houseclean_bypass_cpm |
                                 houseclean_bypass_npd |
                                 houseclean_bypass_plpd);
            break;

        case PpMode3:
            houseclean_bypass = (houseclean_bypass_lpd |
                                 houseclean_bypass_fpd |
                                 houseclean_bypass_cpm |
                                 houseclean_bypass_npd);
            break;

        case PpMode4:
            houseclean_bypass = (houseclean_bypass_lpd |
                                 houseclean_bypass_fpd |
                                 houseclean_bypass_cpm |
                                 houseclean_bypass_npd);
            dma_write_keyhole_enable = 2;
            break;

        case PpModeCfiWrite:
            dma_write_keyhole_enable = 2;
            break;

        case PpModeCfiDmaWrite:
            dma_write_keyhole_enable = 0;
            break;

        case PpModeCfiDmaWriteKeyhole:
            dma_write_keyhole_enable = 1;
            break;

        case PpModeIsoAll:
            pl_iso_disable = 1;
            break;

        case PpModeSppDdr:
            platform = PlatformSPP;
            set_spp_ddr_enable(1);
            break;

        case PpModeSppDdr2:
            platform = PlatformSPP;
            set_spp_ddr_enable(2);
            break;

        case PpModeSppNoDdr:
            platform = PlatformSPP;
            set_spp_ddr_enable(0);
            break;

        case PpModePlClkNone:
            pl_clk_activate = 0;
            break;

        case PpModePlRstNone:
            pl_rst_mask = 0;
            break;

        case PpModePlRstAll:
            pl_rst_mask = (CRP_RST_PL_RESET0_MASK | CRP_RST_PL_RESET1_MASK |
                               CRP_RST_PL_RESET2_MASK | CRP_RST_PL_RESET3_MASK);
            break;

        case PpModeDdrHack:
            ddr_hack_enable = 1;
            break;

        case PpModeDciCalWaitOff:
            dci_cal_wait_enable = 0;
            break;

        case PpModeDciCalWaitOn:
            dci_cal_wait_enable = 1;
            break;

        case PpModePllCalWaitOn:
            pll_cal_wait_enable = 1;
            break;

        case PpModeInitFinishMoveOn:
            init_finish_move_enable = 1;
            break;

        case PpModeSeqSpec0:
            seq_spec = 0;
            break;

        case PpModeSeqSpec1:
            seq_spec = 1;
            break;

        case PpModeSeqSpec2:
            seq_spec = 2;
            break;

        case PpModeSeqSpec3:
            seq_spec = 3;
            break;

        case PpModeSeqSpec4:
            seq_spec = 4;
            break;

        case PpModeSsitMaster1:
            ssit_mode = 1;
            break;

        case PpModeSsitMaster2:
            ssit_mode = 3;
            break;

        case PpModeSsitMaster3:
            ssit_mode = 7;
            break;

        case PpModeSsitSlave:
            ssit_mode = -1;
            break;

        case PpModeNpiAll:
            npi_mode = 0;
            break;

        case PpModeNpiNone:
            npi_mode = 1;
            break;

        case PpModePlBypass:
            pl_bypass = 1;
            break;

        case PpModePcsrNormal:
            pcsr_mode = 0;
            break;

        case PpModePcsrSeparate:
            pcsr_mode = 1;
            break;

        case PpModeAlignOn:
            auto_align = 1;
            break;

        case PpModeAlignOff:
            auto_align = 0;
            break;

        case PpModeDdrCalErr0:
            ddrmc_cal_flags = 0;
            break;

        case PpModeDdrCalErr1:
            ddrmc_cal_flags = 1;
            break;

        case PpModeDdrCalErr2:
            ddrmc_cal_flags = 2;
            break;

        case PpModeSysmonBypass:
            sysmon_bypass = 1;
            break;

        default:
            break;
        }
        if (*p == '\0') break;
        s = p + 1;
    }
    cdobinary_set_auto_align(auto_align);
    if (verbose) {
        printf("Post Processing Settings:\n");
        printf("    CFI configuration command: ");
        if (dma_write_keyhole_enable == 0) {
            printf("dma_write\n");
        } else if (dma_write_keyhole_enable == 1) {
            printf("dma_write_keyhole\n");
        } else {
            printf("write\n");
        }
        printf("    PL isolation override: ");
        if (pl_iso_disable != 0) {
            printf("disable after PL startup\n");
        } else {
            printf("none\n");
        }
        printf("    PMC->PL clock activate: ");
        if (pl_clk_activate != 0) {
            printf("all\n");
        } else {
            printf("none\n");
        }
        printf("    PMC->PL reset activate: ");
        if (pl_rst_mask != 0) {
            printf("%"PRIu32"\n", pl_rst_mask);
        } else {
            printf("none\n");
        }
        printf("    DCI calibration wait enable: %s\n", dci_cal_wait_enable ? "true" : "false");
        printf("    PLL calibration wait enable: %s\n", pll_cal_wait_enable ? "true" : "false");
        printf("    Startup sequence spec version: %"PRIu32"\n", seq_spec);
        printf("    NPI processing mode: %"PRIu32"\n", npi_mode);
        printf("    PL bypass enable: %s\n", pl_bypass ? "true" : "false");
        printf("    House clean bypass:");
        if (houseclean_bypass == 0) {
            printf(" none\n");
        } else {
            if ((houseclean_bypass & houseclean_bypass_lpd) != 0) {
                printf(" LPD");
            }
            if ((houseclean_bypass & houseclean_bypass_fpd) != 0) {
                printf(" FPD");
            }
            if ((houseclean_bypass & houseclean_bypass_npd) != 0) {
                printf(" NPD");
            }
            if ((houseclean_bypass & houseclean_bypass_cpm) != 0) {
                printf(" CPM");
            }
            if ((houseclean_bypass & houseclean_bypass_plpd) != 0) {
                printf(" PLPD");
            }
            printf("\n");
        }
        printf("    Platform: ");
        if (platform == PlatformSPP) {
            if (spp_ddr_ena) {
                printf("SPP with DDR configuration %"PRIu32"\n", spp_ddr_ena);
            } else {
                printf("SPP without DDR configuration\n");
            }
        } else {
            printf("HW\n");
        }
        printf("    DDR Hack: %s\n", ddr_hack_enable ? "on" : "off");
        printf("    SSIT Mode: ");
        switch (ssit_mode) {
        case 0:
            printf("Monolithic");
            break;
        case 1:
            printf("Master with 1 slave");
            break;
        case 3:
            printf("Master with 2 slaves");
            break;
        case 7:
            printf("Master with 3 slaves");
            break;
        case -1:
            printf("Slave");
            break;
        }
        printf("\n");
        printf("    Align DMA writes: %s\n", auto_align ? "on" : "off");
        printf("    Sysmon bypass enable: %s\n", sysmon_bypass ? "true" : "false");
    }
    return 1;
}

static uint32_t get_pd_index(uint32_t block_type) {
    uint32_t pdindex;
    switch(block_type) {
    case XILCDO_NPI_BLK_GT:
        pdindex = PLPD_INDEX;
        break;

    case XILCDO_NPI_BLK_DDRMC:
    case XILCDO_NPI_BLK_NOC_NPS:
    case XILCDO_NPI_BLK_NOC_NCRB:
    case XILCDO_NPI_BLK_NOC_NSU:
    case XILCDO_NPI_BLK_NOC_IDB:
    case XILCDO_NPI_BLK_NOC_NMU:
        pdindex = NPD_INDEX;
        break;

    default:
        pdindex = MAXPD_INDEX;
    }
    return pdindex;
}

static void ssit_sync(CdoSequence * seq) {
    if (ssit_mode > 0) {
        cdocmd_add_ssit_sync_slaves(seq, ssit_mode, DEFAULT_SSIT_TIMEOUT);
    } else if (ssit_mode < 0) {
        cdocmd_add_ssit_sync_master(seq);
    }
}

static void enableCFUWrite(CdoSequence * seq) {
    cdocmd_add_comment(seq, "enable CFU write");
    cdocmd_add_write(seq, CFU_APB_CFU_PROTECT, 0U);
}

static void disableCFUWrite(CdoSequence * seq) {
    cdocmd_add_comment(seq, "disable CFU write");
    cdocmd_add_write(seq, CFU_APB_CFU_PROTECT, CFU_APB_CFU_PROTECT_ACTIVE_MASK);
}

static void setGRestore(CdoSequence * seq) {
    cdocmd_add_comment(seq, "set GRESTORE");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GRESTORE_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, CFU_APB_CFU_FGCR_GRESTORE_MASK);
}

static void clearGRestore(CdoSequence * seq) {
    cdocmd_add_comment(seq, "clear GRESTORE");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GRESTORE_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, 0U);
}

static void pulseGRestore(CdoSequence * seq) {
    setGRestore(seq);

    cdocmd_add_delay(seq, 1U);

    clearGRestore(seq);
}

static void setGmcB(CdoSequence * seq) {
    cdocmd_add_comment(seq, "set GMC_B");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GMC_B_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, CFU_APB_CFU_FGCR_GMC_B_MASK);
}

static void clearGmcB(CdoSequence * seq) {
    cdocmd_add_comment(seq, "clear GMC_B");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GMC_B_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, 0U);
}

static void setGHighB(CdoSequence * seq) {
    cdocmd_add_comment(seq, "set GHIGH_B");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GHIGH_B_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, CFU_APB_CFU_FGCR_GHIGH_B_MASK);
    ssit_sync(seq);
}

static void clearGHighB(CdoSequence * seq) {
    cdocmd_add_comment(seq, "clear GHIGH_B");
    ssit_sync(seq);
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GHIGH_B_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, 0U);
}

static void setGtsCfgB(CdoSequence * seq) {
    cdocmd_add_comment(seq, "set GTS_CFG_B");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GTS_CFG_B_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, CFU_APB_CFU_FGCR_GTS_CFG_B_MASK);
}

static void clearGtsCfgB(CdoSequence * seq) {
    cdocmd_add_comment(seq, "clear GTS_CFG_B");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GTS_CFG_B_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, 0U);
}

static void setEnGlobB(CdoSequence * seq) {
    cdocmd_add_comment(seq, "set EN_GLOB_B");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_EN_GLOBS_B_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, CFU_APB_CFU_FGCR_EN_GLOBS_B_MASK);
}

/*static void clearEnGlobB(CdoSequence * seq) {
    cdocmd_add_comment(seq, "clear EN_GLOB_B");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_EN_GLOBS_B_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, 0U);
}*/

static void setEOS(CdoSequence * seq) {
    cdocmd_add_comment(seq, "set EOS");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_EOS_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, CFU_APB_CFU_FGCR_EOS_MASK);
    ssit_sync(seq);
}

static void clearEOS(CdoSequence * seq) {
    cdocmd_add_comment(seq, "clear EOS");
    ssit_sync(seq);
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_EOS_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, 0U);
}

static void setGWE(CdoSequence * seq, uint32_t BaseAddr) {
    (void)BaseAddr;
    cdocmd_add_comment(seq, "set GWE");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GWE_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, CFU_APB_CFU_FGCR_GWE_MASK);
    ssit_sync(seq);
}

static void clearGWE(CdoSequence * seq, uint32_t BaseAddr) {
    (void)BaseAddr;
    cdocmd_add_comment(seq, "clear GWE");
    ssit_sync(seq);
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GWE_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, 0U);
}

static void setGclkCal(CdoSequence * seq, uint32_t BaseAddr) {
    (void)BaseAddr;
    cdocmd_add_comment(seq, "set GCLK_CAL");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GCLK_CAL_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, CFU_APB_CFU_FGCR_GCLK_CAL_MASK);
}

static void clearGclkCal(CdoSequence * seq, uint32_t BaseAddr) {
    (void)BaseAddr;
    cdocmd_add_comment(seq, "clear GCLK_CAL");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GCLK_CAL_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, 0U);
}

static void setGSCWE(CdoSequence * seq) {
    cdocmd_add_comment(seq, "set GSCWE");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GSCWE_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, CFU_APB_CFU_FGCR_GSCWE_MASK);
}

static void clearGSCWE(CdoSequence * seq) {
    cdocmd_add_comment(seq, "clear GSCWE");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_FGCR_GSCWE_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_FGCR, 0U);
}

/* Called before NPI startup sequences */
static void setGlobalSignals(CdoSequence * seq) {
    cdocmd_add_comment(seq, "pre fabric startup");

    setGmcB(seq);

    pulseGRestore(seq);

    setGHighB(seq);

    cdocmd_add_mask_poll(seq,
                         CFU_APB_CFU_STATUS,
                         CFU_APB_CFU_STATUS_GHIGH_B_ISHIGH_MASK,
                         CFU_APB_CFU_STATUS_GHIGH_B_ISHIGH_MASK,
                         DEFAULT_POLL_TIMEOUT, 0);

    setGtsCfgB(seq);
}

/* Fabric shutdown sequence */
static void clearGlobalSignals(CdoSequence * seq) {
    cdocmd_add_comment(seq, "fabric shutdown");

#if 0
    /* TODO: GSCWE is used to load of GSC-CFI.  Is this correct? No. */
    setGSCWE(seq);
#endif

    /* GSC must be loaded before this */
    clearEOS(seq);
    clearGWE(seq, 0);

#if 0
    /* TODO: grestore is a short positive pulse in all waveforms in
     * the Eveset Fabric Global Signals document.  Is this
     * correct? Should be removed  */
    clearGRestore(seq);
    setGRestore(seq);
#endif

    clearGtsCfgB(seq);

    clearGHighB(seq);

    cdocmd_add_mask_poll(seq,
                         CFU_APB_CFU_STATUS,
                         CFU_APB_CFU_STATUS_GHIGH_B_ISLOW_MASK,
                         CFU_APB_CFU_STATUS_GHIGH_B_ISLOW_MASK,
                         DEFAULT_POLL_TIMEOUT, 0);

    clearGmcB(seq);
}

enum {
    ISO_NODEID_PL_FPD = 0x20000001,
    ISO_NODEID_PL_LPD = 0x20000006,
    ISO_NODEID_PL_PMC = 0x2000000c,
    ISO_NODEID_PL_SOC = 0x2000000f,
    ISO_NODEID_PL_CPM_PCIEA0_ATTR = 0x20000013,
    ISO_NODEID_PL_CPM_PCIEA1_ATTR =  0x20000014,
    ISO_NODEID_PL_CPM_RST_CPI0 = 0x20000015,
    ISO_NODEID_PL_CPM_RST_CPI1 = 0x20000016,
};

/* Called after NPI startup sequences */
static void assertGlobalSignals(CdoSequence * seq) {
    cdocmd_add_comment(seq, "post fabric startup");
    setEOS(seq);
    setEnGlobB(seq);

    if (pl_iso_disable) {
        cdocmd_add_pm_iso_control(seq, ISO_NODEID_PL_FPD, 0);
        cdocmd_add_pm_iso_control(seq, ISO_NODEID_PL_LPD, 0);
        cdocmd_add_pm_iso_control(seq, ISO_NODEID_PL_PMC, 0);
        cdocmd_add_pm_iso_control(seq, ISO_NODEID_PL_SOC, 0);
        cdocmd_add_pm_iso_control(seq, ISO_NODEID_PL_CPM_PCIEA0_ATTR, 0);
        cdocmd_add_pm_iso_control(seq, ISO_NODEID_PL_CPM_PCIEA1_ATTR, 0);
        cdocmd_add_pm_iso_control(seq, ISO_NODEID_PL_CPM_RST_CPI0, 0);
        cdocmd_add_pm_iso_control(seq, ISO_NODEID_PL_CPM_RST_CPI1, 0);
    }
    if (pl_clk_activate) {
        cdocmd_add_write(seq, CRP_RCLK_CTRL, CRP_RCLK_CTRL_CLKACT_MASK);
    }
    if (pl_rst_mask) {
        cdocmd_add_mask_write(seq, CRP_RST_PL, pl_rst_mask, 0);
    }
}

static void waitForStreamDone(CdoSequence * seq) {
    cdocmd_add_comment(seq, "wait for CFI/CFRAME fifos to drain");
    cdocmd_add_mask_poll(seq,
                         CFU_APB_CFU_STATUS,
                         CFU_APB_CFU_STATUS_CFU_STREAM_BUSY_MASK |
                         CFU_APB_CFU_STATUS_CFI_CFRAME_BUSY_MASK,
                         0U, DEFAULT_POLL_TIMEOUT, 0);
}

static void wait1us(CdoSequence * seq, uint32_t BaseAddr) {
    (void)BaseAddr;
    cdocmd_add_delay(seq, 1U);
}

static void wait10us(CdoSequence * seq, uint32_t BaseAddr) {
    (void)BaseAddr;
    cdocmd_add_delay(seq, 10U);
}

static void wait100us(CdoSequence * seq, uint32_t BaseAddr) {
    (void)BaseAddr;
    cdocmd_add_delay(seq, 100U);
}

static void clearLockState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "PCSR unlock");
    cdocmd_add_write(seq, BaseAddr + XILCDO_NPI_PCSR_LOCK_OFFSET, PCSR_UNLOCK_VAL);
}

static void setLockState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "PCSR lock");
    cdocmd_add_write(seq, BaseAddr + XILCDO_NPI_PCSR_LOCK_OFFSET, 0x0U);
}

static void writePcsrCtrlReg(CdoSequence * seq, uint32_t BaseAddr, uint32_t Mask, uint32_t Value) {
    if (block_continue_mode) {
        CdoCommand * maskcmd = NULL;
        CdoCommand * valuecmd = NULL;
        LINK * l = seq->cmds.prev;
        while (l != &seq->cmds) {
            CdoCommand * cmd = all2cmds(l);
            l = l->prev;
            if (cmd->type == CdoCmdComment) {
                /* Skip */
            } else if (cmd->type == CdoCmdWrite && cmd->count == 1) {
                if (cmd->dstaddr == BaseAddr + XILCDO_NPI_PCSR_CONTROL_OFFSET) {
                    if (valuecmd != NULL) break;
                    valuecmd = cmd;
                } else if (cmd->dstaddr == BaseAddr + XILCDO_NPI_PCSR_MASK_OFFSET) {
                    if (valuecmd == NULL || maskcmd != NULL) break;
                    maskcmd = cmd;
                    break;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
        if (maskcmd != NULL && valuecmd != NULL) {
            uint32_t Mask2 = *(uint32_t *)maskcmd->buf;
            uint32_t Value2 = *(uint32_t *)valuecmd->buf;
            if ((Mask & Mask2) == 0) {
                Mask |= Mask2;
                Value |= Value2 & Mask2;
                cdocmd_free(maskcmd);
                cdocmd_free(valuecmd);
            }
        }
    }
    cdocmd_add_write(seq, BaseAddr + XILCDO_NPI_PCSR_MASK_OFFSET, Mask);
    cdocmd_add_write(seq, BaseAddr + XILCDO_NPI_PCSR_CONTROL_OFFSET, Value);
}

static void clearAmsRootPlBypass(CdoSequence * seq, uint32_t BaseAddr) {
    (void)BaseAddr;
    clearLockState(seq, AMS_ROOT_BASE);
    cdocmd_add_mask_write(seq, AMS_ROOT_TOKEN_MNGR, AMS_ROOT_TOKEN_MNGR_BYPASS_PL_MASK, 0U);
    setLockState(seq, AMS_ROOT_BASE);
}

static void setPRMode(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set PR mode");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_NSU_PR_MASK, PCSR_MASK_NSU_PR_MASK);
}

static void clearPRMode(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear PR mode");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_NSU_PR_MASK, 0x0U);
}

static void setAxiReqRejectMode(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set AXI reject mode");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_AXI_REQ_REJECT_MASK, PCSR_MASK_AXI_REQ_REJECT_MASK);
}

static void clearAxiReqRejectMode(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear AXI reject mode");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_AXI_REQ_REJECT_MASK, 0x0U);
}

static void checkNmuRegBusy(CdoSequence * seq, uint32_t BaseAddr) {
    if (!is_platform_spp()) {
        cdocmd_add_comment(seq, "wait for NMU busy");
        cdocmd_add_mask_poll(
            seq, BaseAddr + XILCDO_REG_BUSY_OFFSET,
            XILCDO_REG_BUSY_MASK, 0, DEFAULT_POLL_TIMEOUT, 0);
    }
}

static void checkNsuRegBusy(CdoSequence * seq, uint32_t BaseAddr) {
    if (!is_platform_spp()) {
        cdocmd_add_comment(seq, "wait for NSU busy");
        cdocmd_add_mask_poll(
            seq, BaseAddr + XILCDO_NSU_REG_BUSY_OFFSET,
            XILCDO_NSU_REG_BUSY_MASK, 0, DEFAULT_POLL_TIMEOUT, 0);
    }
}

static void checkRegPendingBurst(CdoSequence * seq, uint32_t BaseAddr) {
    if (!is_platform_spp()) {
        cdocmd_add_comment(seq, "wait for NSU pending");
        cdocmd_add_mask_poll(
            seq, BaseAddr + XILCDO_REG_PEND_BURST_OFFSET,
            XILCDO_REG_PEND_BURST_OFFSET_MASK, 0, DEFAULT_POLL_TIMEOUT, 0);
    }
}

static void clearInitCtrl(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear init ctrl");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_INIT_CTRL_MASK, 0U);
}

static void setInitCtrl(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set init ctrl");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_INIT_CTRL_MASK, PCSR_MASK_INIT_CTRL_MASK);
}

static void clearUBInitState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear UB init");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_UB_INIT_MASK, 0x0U);
}

static void setUBInitState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set UB init");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_UB_INIT_MASK, PCSR_MASK_UB_INIT_MASK);
}

static void nocSysRst(CdoSequence * seq) {
    cdocmd_add_comment(seq, "clear NoC srst");
    cdocmd_add_mask_write(
        seq, CRP_RST_NONPS,
        CRP_RST_NONPS_NOC_RESET_MASK | CRP_RST_NONPS_SYS_RST_1_MASK |
        CRP_RST_NONPS_SYS_RST_2_MASK | CRP_RST_NONPS_SYS_RST_3_MASK, 0U);
}

static void clearGateReg(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear gate reg");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_GATEREG_MASK, 0x0U);
}

static void setGateReg(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set gate reg");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_GATEREG_MASK, PCSR_MASK_GATEREG_MASK);
}

static void setCompleteState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set complete state");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_PCOMPLETE_MASK, PCSR_MASK_PCOMPLETE_MASK);
}

static void clearCompleteState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear complete state");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_PCOMPLETE_MASK, 0U);
}

static void clearODisable(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear odisable");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_ODISABLE_MASK, 0x0U);
}

static void setODisable(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set odisable");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_ODISABLE_MASK, PCSR_MASK_ODISABLE_MASK);
}

static void clearTriState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear tristate");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_TRISTATE_MASK, 0x0U);
}

static void setTriState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set tristate");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_TRISTATE_MASK, PCSR_MASK_TRISTATE_MASK);
}

static void clearInitState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear init state");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_INITSTATE_MASK, 0x0U);
}

static void setInitState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set init state");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_INITSTATE_MASK, PCSR_MASK_INITSTATE_MASK);
}

static void clearDCIOfcReset(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear dci ofc reset");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_DCI_OFC_RST_MASK, 0x0U);
}

static void setDCIOfcReset(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set dci ofc reset");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_DCI_OFC_RST_MASK, PCSR_MASK_DCI_OFC_RST_MASK);
}

static void setStartCal(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set start cal");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_STARTCAL_MASK, PCSR_MASK_STARTCAL_MASK);
}

static void clearStartCal(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear start cal");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_STARTCAL_MASK, 0x0U);
}

static void setSleepState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set sleep state");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_SLEEP_MASK, PCSR_MASK_SLEEP_MASK);
}

static void clearSleepState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear sleep state");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_SLEEP_MASK, 0x0U);
}

static void setShutDown(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set shutdown");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_SHTDN_MASK, PCSR_MASK_SHTDN_MASK);
}

static void clearShutDown(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear shutdown");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_SHTDN_MASK, 0x0U);
}

static void checkShutDown(CdoSequence * seq, uint32_t BaseAddr) {
    if (!is_platform_spp()) {
        cdocmd_add_comment(seq, "wait for shutdown");
        cdocmd_add_mask_poll(
            seq, BaseAddr + XILCDO_NPI_PCSR_STATUS_OFFSET,
            PCSR_STATUS_SHUTDN_COMP_MASK, PCSR_STATUS_SHUTDN_COMP_MASK,
            DEFAULT_POLL_TIMEOUT, 0);
    }
}

static void setDeskew(CdoSequence * seq, uint32_t BaseAddr)
{
    cdocmd_add_comment(seq, "set deskew");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_DESKEW_MASK, PCSR_MASK_DESKEW_MASK);
}

static void clearDeskew(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear deskew");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_DESKEW_MASK, 0x0U);
}

static void clearHoldState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear hold state");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_HOLDSTATE_MASK, 0x0U);
}

static void setHoldState(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set hold state");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_HOLDSTATE_MASK, PCSR_MASK_HOLDSTATE_MASK);
}

static void setApbEnable(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set APB enable");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_APBEN_MASK, PCSR_MASK_APBEN_MASK);
}

static void clearApbEnable(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear APB enable");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_APBEN_MASK, 0x0U);
}

static void setFabricEnable(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set fabric enable");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_FABRICEN_MASK, PCSR_MASK_FABRICEN_MASK);
}

static void clearFabricEnable(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear fabric enable");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_FABRICEN_MASK, 0x0U);
}

static void setPRFreeze(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set PR freeze");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_PR_FREEZE_MASK, PCSR_MASK_PR_FREEZE_MASK);
}

static void clearPRFreeze(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear PR freeze");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_PR_FREEZE_MASK, 0U);
}

static uint32_t ddrmc_ub_to_noc(uint32_t BaseAddr) {
    switch (BaseAddr) {
    case 0xf6110000: return 0xF6070000;
    case 0xf6280000: return 0xF6210000;
    case 0xf63f0000: return 0xF6380000;
    case 0xf6560000: return 0xF64F0000;
    default:
        fprintf(stderr, "unexpected address for DDRMC_UB %#"PRIx32"\n", BaseAddr);
        exit(1);
    }
}

static void resetClkMux(CdoSequence * seq, uint32_t BaseAddr) {
    uint32_t ba = ddrmc_ub_to_noc(BaseAddr);
    clearLockState(seq, ba);
    cdocmd_add_comment(seq, "reset clock mux");
    cdocmd_add_mask_write(seq, ba + XILCDO_DDRMC_NOC_CLK_MUX_OFFSET,
                          XILCDO_DDRMC_CLK_SRC_SELMASK, 0U);
    setLockState(seq, ba);
}

static void setODisableAXI(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set odisable AXI");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_ODISABLE_AXI_MASK, PCSR_MASK_ODISABLE_AXI_MASK);
}

static void clearODisableAXI(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear odisable AXI");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_ODISABLE_AXI_MASK, 0U);
}

static void setODisableNPP(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "set odisable NPP");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_ODISABLE_NPP_MASK, PCSR_MASK_ODISABLE_NPP_MASK);
}

static void clearODisableNPP(CdoSequence * seq, uint32_t BaseAddr) {
    cdocmd_add_comment(seq, "clear odisable NPP");
    writePcsrCtrlReg(seq, BaseAddr, PCSR_MASK_ODISABLE_NPP_MASK, 0U);
}

static void checkCalibrationNcrb(CdoSequence * seq, uint32_t BaseAddr) {
    if (!is_platform_spp()) {
        cdocmd_add_comment(seq, "wait for NCRB calibration");
        cdocmd_add_mask_poll(
            seq, BaseAddr + XILCDO_NPI_PCSR_STATUS_OFFSET,
            PCSR_STATUS_CALDONE_MASK, PCSR_STATUS_CALDONE_MASK,
            DEFAULT_POLL_TIMEOUT, 0);
    }
    clearStartCal(seq, BaseAddr);
}

static void checkCalibrationNcrb2(CdoSequence * seq, uint32_t BaseAddr) {
    if (!is_platform_spp()) {
        cdocmd_add_comment(seq, "Test NCRB calibration");
        cdocmd_add_mask_poll(
            seq, BaseAddr + XILCDO_NPI_PCSR_STATUS_OFFSET,
            PCSR_STATUS_CALDONE_MASK, PCSR_STATUS_CALDONE_MASK,
            TEST_POLL_TIMEOUT, 0);
    }
}

static void checkCalibrationDdr(CdoSequence * seq, uint32_t BaseAddr) {
    if (!is_platform_spp()) {
        cdocmd_add_comment(seq, "wait for DDR calibration");
        cdocmd_add_mask_poll(
            seq, BaseAddr + XILCDO_NPI_PCSR_STATUS_OFFSET,
            PCSR_STATUS_CALDONE_MASK, PCSR_STATUS_CALDONE_MASK,
            DEFAULT_POLL_TIMEOUT, ddrmc_cal_flags);
    }
}

static void checkCalibrationDci(CdoSequence * seq, uint32_t BaseAddr) {
    if (dci_cal_wait_enable && !is_platform_spp()) {
        cdocmd_add_comment(seq, "wait for DCI calibration");
        cdocmd_add_mask_poll(
            seq, BaseAddr + XILCDO_NPI_PCSR_STATUS_OFFSET,
            PCSR_STATUS_CALDONE_MASK, PCSR_STATUS_CALDONE_MASK,
            DEFAULT_POLL_TIMEOUT, 0);
    }
}

static void checkCalibrationPll(CdoSequence * seq, uint32_t BaseAddr) {
    if (pll_cal_wait_enable && !is_platform_spp()) {
        cdocmd_add_comment(seq, "wait for PLL calibration");
        cdocmd_add_mask_poll(
            seq, BaseAddr + XILCDO_NPI_PCSR_STATUS_OFFSET,
            PCSR_STATUS_CALDONE_MASK, PCSR_STATUS_CALDONE_MASK,
            DEFAULT_POLL_TIMEOUT, 0);
    }
}

static void npd_houseclean(CdoSequence * seq) {
    if (npd_houseclean_done) return;
    npd_houseclean_done = 1;

    cdocmd_add_comment(seq, "house clean NPD");
    cdocmd_add_pm_init_node(seq, NODEID_NPD, NODE_INIT_START, 0, NULL, 0);
    if ((houseclean_bypass & houseclean_bypass_npd) == 0) {
        cdocmd_add_pm_init_node(seq, NODEID_NPD, NODE_INIT_SCAN_CLEAR, 0, NULL, 0);
        cdocmd_add_pm_init_node(seq, NODEID_NPD, NODE_INIT_BISR_TRIM, 0, NULL, 0);
        cdocmd_add_pm_init_node(seq, NODEID_NPD, NODE_INIT_MBIST, 0, NULL, 0);
    }
    cdocmd_add_pm_init_node(seq, NODEID_NPD, NODE_INIT_FINISH, 0, NULL, 0);
}

static void plpd_houseclean(CdoSequence * seq) {
    if (plpd_houseclean_done) return;
    plpd_houseclean_done = 1;
    npd_houseclean(seq);
    cdocmd_add_comment(seq, "house clean PLPD");
    cdocmd_add_pm_init_node(seq, NODEID_PLPD, NODE_INIT_START, 0, NULL, 0);
    if ((houseclean_bypass & houseclean_bypass_plpd) == 0) {
        cdocmd_add_pm_init_node(seq, NODEID_PLPD, NODE_INIT_HOUSE_CLEAN_PLPD, 0, NULL, 0);
    } else {
        uint32_t bypass = 1;
        cdocmd_add_pm_init_node(seq, NODEID_PLPD, NODE_INIT_HOUSE_CLEAN_PLPD, 1, &bypass, is_be_host());
    }
    cdocmd_add_pm_init_node(seq, NODEID_PLPD, NODE_INIT_FINISH, 0, NULL, 0);
}

static void cfi_startup(CdoSequence * seq) {
    setGlobalSignals(seq);
    setGclkCal(seq, 0);
    clearGclkCal(seq, 0);
    setGWE(seq, 0);
    assertGlobalSignals(seq);
}

static void rewrite_precfg_group(RewriteInfo * ri, CdoCommand * firstcmd, LINK * lend) {
    CdoSequence * seq = cdocmd_create_sequence();
    LINK * l = &firstcmd->link_all;
    uint32_t pdmask = 0;
    while (l != lend) {
        CdoCommand * cmd = all2cmds(l);
        uint32_t bt = cmd->flags & XILCDO_NPI_BLK_MASK;
        uint32_t pdindex = get_pd_index(bt);
        pdmask |= 1 << pdindex;
        l = l->next;
    }
    if ((ri->precfg_pdmask & pdmask) != 0) {
        fprintf(stderr, "multiple npi_precfg groups for the same power domain\n");
        exit(1);
    }
    ri->precfg_pdmask |= pdmask;

    if ((pdmask & (1 << NPD_INDEX)) != 0) {
        npd_houseclean(seq);
        cdocmd_insert_seq(firstcmd, seq);
    }

    if ((pdmask & (1 << PLPD_INDEX)) != 0 && !pl_bypass) {
        plpd_houseclean(seq);
        cdocmd_insert_seq(firstcmd, seq);
    }

    l = &firstcmd->link_all;
    while (l != lend) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        cdocmd_free(cmd);
    }
    cdocmd_delete_sequence(seq);
}

#define STARTUP_MODULE_LIST (                   \
        BLKMASK_VREF |                          \
        BLKMASK_XPIO |                          \
        BLKMASK_XPIO_IOMISC |                   \
        BLKMASK_XPLL |                          \
        BLKMASK_XPHY |                          \
        BLKMASK_DDRMC |                         \
        BLKMASK_XPIPE |                         \
        BLKMASK_GT |                            \
        BLKMASK_NOC_NPS |                       \
        BLKMASK_NOC_NCRB |                      \
        BLKMASK_NOC_NMU |                       \
        BLKMASK_NOC_NSU |                       \
        /* BLKMASK_NOC_IDB | */                 \
        BLKMASK_VERT_TO_HSR |                   \
        BLKMASK_VERT_TO_HSR_GT |                \
        BLKMASK_REBUF_VRT |                     \
        BLKMASK_REBUF_VRT_GT |                  \
        BLKMASK_HSR_BUFGS |                     \
        BLKMASK_VNOC |                          \
        BLKMASK_VNOC_PS |                       \
        BLKMASK_CLK_GT |                        \
        BLKMASK_REBUF_HSR_TNOC_ME |             \
        BLKMASK_MMCM |                          \
        BLKMASK_DPLL |                          \
        BLKMASK_AMS_SAT |                       \
        BLKMASK_AMS_ROOT |                      \
        /* BLKMASK_ME_NPI | */                  \
        BLKMASK_PLL_PHY |                       \
        /* BLKMASK_NOC_NIR | */                 \
        0)

static BlockHandler startup_unlock_group[] = {
    /* Unlock */
    {
        clearLockState,
        0, 0,
        STARTUP_MODULE_LIST
    },

    /* End */
    { NULL }
};


static BlockHandler startup_lock_group[] = {
    /* Lock */
    {
        setLockState,
        0, 0,
        STARTUP_MODULE_LIST
    },

    /* End */
    { NULL }
};


static BlockHandler startup_xpio_group[] = {
    /* sequence DCI_SEQ_1 */
    /* This assumes that Vivado generates npi_seq commands for DCI
     * modules in order such that all masters are started before their
     * slaves. */
    {
        clearGateReg,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },
    {
        clearTriState,
        0, 0,
        0                               /* Continue */
    },

    /* sequence VREF_SEQ */
    {
        clearGateReg,
        0, 0,
        BLKMASK_VREF
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        wait10us,
        XILCDO_NPI_FIRST_MASK, XILCDO_NPI_FIRST_MASK,
        BLKMASK_VREF
    },

    /* sequence XPIO_SEQ_1 */
    {
        clearGateReg,
        0, 0,
        BLKMASK_XPIO
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_XPIO
    },

    /* sequence DCI_SEQ_2 */
    {
        clearInitState,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },
    {
        clearDCIOfcReset,
        0, 0,
        0                               /* Continue */
    },
    {
        checkCalibrationDci,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },
    {
        setCompleteState,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },

    /* sequence XPIO_SEQ_2 */
    {
        clearTriState,
        0, 0,
        BLKMASK_XPIO
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },

    /* End */
    { NULL }
};

#define CLK_GATEREG_ODISABLE_LIST (             \
        BLKMASK_HSR_BUFGS |                     \
        BLKMASK_VERT_TO_HSR |                   \
        BLKMASK_VERT_TO_HSR_GT |                \
        BLKMASK_REBUF_HSR_TNOC_ME |             \
        BLKMASK_VNOC |                          \
        BLKMASK_VNOC_PS |                       \
        BLKMASK_CLK_GT |                        \
        BLKMASK_REBUF_VRT |                     \
        BLKMASK_REBUF_VRT_GT)

#define CLK_INITSTATE_LIST (                    \
        BLKMASK_HSR_BUFGS |                     \
        BLKMASK_VNOC |                          \
        BLKMASK_VNOC_PS |                       \
        BLKMASK_CLK_GT)

#define CLK_STARTCAL_LIST (                     \
        BLKMASK_HSR_BUFGS |                     \
        BLKMASK_VNOC_PS |                       \
        BLKMASK_CLK_GT)

#define CLK_DESKEWEN_LIST (                     \
        BLKMASK_VNOC |                          \
        BLKMASK_VNOC_PS |                       \
        BLKMASK_CLK_GT |                        \
        BLKMASK_REBUF_VRT |                     \
        BLKMASK_REBUF_VRT_GT)

#define CLK_HOLDSTATE_LIST (                    \
        BLKMASK_HSR_BUFGS |                     \
        BLKMASK_VNOC_PS |                       \
        BLKMASK_CLK_GT)

#define CLK_PCOMPLETE_LIST (                    \
        BLKMASK_HSR_BUFGS |                     \
        BLKMASK_VERT_TO_HSR |                   \
        BLKMASK_VERT_TO_HSR_GT |                \
        BLKMASK_REBUF_HSR_TNOC_ME |             \
        BLKMASK_VNOC |                          \
        BLKMASK_VNOC_PS |                       \
        BLKMASK_CLK_GT |                        \
        BLKMASK_REBUF_VRT |                     \
        BLKMASK_REBUF_VRT_GT)

#define CMT_GATEREG_LIST (                      \
        BLKMASK_MMCM |                          \
        BLKMASK_DPLL |                          \
        BLKMASK_PLL_PHY)

#define CMT_GATEREG_NODDR_LIST (BLKMASK_XPLL)

#define CMT_CAL_LIST (BLKMASK_MMCM | BLKMASK_DPLL)

#define CMT_CAL_NODDR_LIST (BLKMASK_XPLL)

#define CMT_FABRICEN_LIST (BLKMASK_MMCM | BLKMASK_DPLL)

#define CMT_FABRICEN_NODDR_LIST (BLKMASK_XPLL)


static BlockHandler startup_mmcm_clk_group[] = {
    /* sequence CLK_GATEREG */
    {
        clearGateReg,
        0, 0,
        CLK_GATEREG_ODISABLE_LIST
    },

    /* sequence CLK_INITSTATE */
    {
        clearInitState,
        0, 0,
        CLK_INITSTATE_LIST
    },

    /* sequence CLK_ODISABLE */
    {
        clearODisable,
        0, 0,
        CLK_GATEREG_ODISABLE_LIST
    },

    /* sequence CLK_STARTCAL_A */
    {
        setStartCal,
        0, 0,
        CLK_STARTCAL_LIST
    },
    {
        setGclkCal,
        XILCDO_NPI_FABRIC_STARTUP_MASK, XILCDO_NPI_FABRIC_STARTUP_MASK,
        BLKMASK_ALWAYS
    },

    /* sequence CLK_DESKEWEN_A */
    {
        setDeskew,
        0, 0,
        CLK_DESKEWEN_LIST
    },
    {
        wait100us,
        XILCDO_NPI_FIRST_MASK, XILCDO_NPI_FIRST_MASK,
        CLK_DESKEWEN_LIST
    },

    /* sequence CLK_DESKEWEN_D */
    {
        clearDeskew,
        0, 0,
        CLK_DESKEWEN_LIST
    },

    /* sequence CLK_STARTCAL_D */
    {
        clearStartCal,
        0, 0,
        CLK_STARTCAL_LIST
    },
    {
        clearGclkCal,
        XILCDO_NPI_FABRIC_STARTUP_MASK, XILCDO_NPI_FABRIC_STARTUP_MASK,
        BLKMASK_ALWAYS
    },

    /* sequence CLK_HOLDSTATE */
    {
        clearHoldState,
        0, 0,
        CLK_HOLDSTATE_LIST
    },
    {
        setGWE,
        XILCDO_NPI_FABRIC_STARTUP_MASK, XILCDO_NPI_FABRIC_STARTUP_MASK,
        BLKMASK_ALWAYS
    },

    /* sequence CLK_PCOMPLETE */
    {
        setCompleteState,
        0, 0,
        CLK_PCOMPLETE_LIST
    },

    /* sequence CMT_GATEREG */
    {
        clearGateReg,
        0, 0,
        CMT_GATEREG_LIST
    },
    {
        clearGateReg,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_GATEREG_NODDR_LIST
    },

    /* sequence CMT_CALIBRATE */
    {
        clearInitState,
        0, 0,
        CMT_CAL_LIST
    },
    {
        clearODisable,
        0, 0,
        0                               /* Continue */
    },
    {
        clearInitState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_CAL_NODDR_LIST
    },
    {
        clearODisable,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        0                               /* Continue */
    },
    {
        checkCalibrationPll,
        0, 0,
        CMT_CAL_LIST
    },
    {
        checkCalibrationPll,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_CAL_NODDR_LIST
    },

    /* sequence CMT_FABRICEN */
    {
        setFabricEnable,
        XILCDO_NPI_FABRICEN_MASK, XILCDO_NPI_FABRICEN_MASK,
        CMT_FABRICEN_LIST
    },
    {
        setApbEnable,
        XILCDO_NPI_FABRICEN_MASK | XILCDO_NPI_APBEN_MASK,
        XILCDO_NPI_FABRICEN_MASK | XILCDO_NPI_APBEN_MASK,
        0                               /* Continue */
    },
    {                                   /* TODO */
        setFabricEnable,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        //        XILCDO_NPI_FABRICEN_MASK, XILCDO_NPI_FABRICEN_MASK,
        CMT_FABRICEN_NODDR_LIST
    },
    {                                   /* TODO */
        setApbEnable,
        XILCDO_NPI_DDRMC_PRESENT_MASK | XILCDO_NPI_APBEN_MASK,
        XILCDO_NPI_APBEN_MASK,
        //        XILCDO_NPI_FABRICEN_MASK, XILCDO_NPI_FABRICEN_MASK,
        0                               /* Continue */
    },

    /* sequence CMT_PCOMPLETE */
    {
        setCompleteState,
        0, 0,
        CMT_GATEREG_LIST
    },
    {
        setCompleteState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_GATEREG_NODDR_LIST
    },

    /* End */
    { NULL }
};


#define CPAP_SLV_LIST (BLKMASK_PLL_PHY)
#define CMT_SLV_LIST (BLKMASK_MMCM | BLKMASK_DPLL | BLKMASK_XPLL)

static BlockHandler startup_mmcm_clk_group3[] = {
    /* sequence CLK_GATEREG_D */
    {
        clearGateReg,
        0, 0,
        CLK_GATEREG_ODISABLE_LIST
    },

    /* sequence CLK_INITSTATE */
    {
        clearInitState,
        0, 0,
        CLK_INITSTATE_LIST
    },

    /* sequence CLK_ODISABLE_D */
    {
        clearODisable,
        0, 0,
        CLK_GATEREG_ODISABLE_LIST
    },

    /* sequence CLK_STARTCAL_A */
    {
        setStartCal,
        0, 0,
        CLK_STARTCAL_LIST
    },
    {
        setGclkCal,
        XILCDO_NPI_FABRIC_STARTUP_MASK, XILCDO_NPI_FABRIC_STARTUP_MASK,
        BLKMASK_ALWAYS
    },

    /* sequence CLK_DESKEWEN_A */
    {
        setDeskew,
        0, 0,
        CLK_DESKEWEN_LIST
    },
    {
        wait100us,
        XILCDO_NPI_FIRST_MASK, XILCDO_NPI_FIRST_MASK,
        CLK_DESKEWEN_LIST
    },

    /* sequence CLK_DESKEWEN_D */
    {
        clearDeskew,
        0, 0,
        CLK_DESKEWEN_LIST
    },

    /* sequence CLK_STARTCAL_D */
    {
        clearStartCal,
        0, 0,
        CLK_STARTCAL_LIST
    },
    {
        clearGclkCal,
        XILCDO_NPI_FABRIC_STARTUP_MASK, XILCDO_NPI_FABRIC_STARTUP_MASK,
        BLKMASK_ALWAYS
    },

    /* sequence CLK_HOLDSTATE */
    {
        clearHoldState,
        0, 0,
        CLK_HOLDSTATE_LIST
    },
    {
        setGWE,
        XILCDO_NPI_FABRIC_STARTUP_MASK, XILCDO_NPI_FABRIC_STARTUP_MASK,
        BLKMASK_ALWAYS
    },

    /* sequence CLK_PCOMPLETE */
    {
        setCompleteState,
        0, 0,
        CLK_PCOMPLETE_LIST
    },

    /* sequence CMT_START */
    {
        clearGateReg,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_SLV_LIST
    },
    {
        clearInitState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_SLV_LIST
    },
    {
        clearODisable,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        0                               /* Continue */
    },
    {
        /* TODO: Add test for "lock_wait" parameter when it is added by Vivado */
        checkCalibrationPll,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_SLV_LIST
    },
    {
        setFabricEnable,
        XILCDO_NPI_DDRMC_PRESENT_MASK | XILCDO_NPI_FABRICEN_MASK, XILCDO_NPI_FABRICEN_MASK,
        CMT_SLV_LIST
    },
    {
        setApbEnable,
        XILCDO_NPI_DDRMC_PRESENT_MASK | XILCDO_NPI_FABRICEN_MASK | XILCDO_NPI_APBEN_MASK,
        XILCDO_NPI_FABRICEN_MASK | XILCDO_NPI_APBEN_MASK,
        0                               /* Continue */
    },
    {
        setCompleteState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_SLV_LIST
    },

    /* End */
    { NULL }
};

static BlockHandler startup_mmcm_clk_group4[] = {
    /* sequence CLK_GATEREG_D */
    {
        clearGateReg,
        0, 0,
        CLK_GATEREG_ODISABLE_LIST
    },

    /* sequence CLK_INITSTATE */
    {
        clearInitState,
        0, 0,
        CLK_INITSTATE_LIST
    },

    /* sequence CLK_ODISABLE_D */
    {
        clearODisable,
        0, 0,
        CLK_GATEREG_ODISABLE_LIST
    },

    /* sequence CLK_DESKEWEN_A */
    {
        setDeskew,
        0, 0,
        CLK_DESKEWEN_LIST
    },

    /* sequence CLK_STARTCAL_A */
    {
        setStartCal,
        0, 0,
        CLK_STARTCAL_LIST
    },
    {
        setGclkCal,
        XILCDO_NPI_FABRIC_STARTUP_MASK, XILCDO_NPI_FABRIC_STARTUP_MASK,
        BLKMASK_ALWAYS
    },
    {
        wait100us,
        XILCDO_NPI_FIRST_MASK, XILCDO_NPI_FIRST_MASK,
        CLK_DESKEWEN_LIST
    },

    /* sequence CLK_STARTCAL_D */
    {
        clearStartCal,
        0, 0,
        CLK_STARTCAL_LIST
    },
    {
        clearGclkCal,
        XILCDO_NPI_FABRIC_STARTUP_MASK, XILCDO_NPI_FABRIC_STARTUP_MASK,
        BLKMASK_ALWAYS
    },

    /* sequence CLK_DESKEWEN_D */
    {
        clearDeskew,
        0, 0,
        CLK_DESKEWEN_LIST
    },

    /* sequence CLK_HOLDSTATE */
    {
        clearHoldState,
        0, 0,
        CLK_HOLDSTATE_LIST
    },
    {
        setGWE,
        XILCDO_NPI_FABRIC_STARTUP_MASK, XILCDO_NPI_FABRIC_STARTUP_MASK,
        BLKMASK_ALWAYS
    },

    /* sequence CLK_PCOMPLETE */
    {
        setCompleteState,
        0, 0,
        CLK_PCOMPLETE_LIST
    },

    /* sequence CMT_START */
    {
        clearGateReg,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_SLV_LIST
    },
    {
        clearInitState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_SLV_LIST
    },
    {
        clearODisable,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        0                               /* Continue */
    },
    {
        /* TODO: Add test for "lock_wait" parameter when it is added by Vivado */
        checkCalibrationPll,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_SLV_LIST
    },
    {
        setFabricEnable,
        XILCDO_NPI_DDRMC_PRESENT_MASK | XILCDO_NPI_FABRICEN_MASK, XILCDO_NPI_FABRICEN_MASK,
        CMT_SLV_LIST
    },
    {
        setApbEnable,
        XILCDO_NPI_DDRMC_PRESENT_MASK | XILCDO_NPI_FABRICEN_MASK | XILCDO_NPI_APBEN_MASK,
        XILCDO_NPI_FABRICEN_MASK | XILCDO_NPI_APBEN_MASK,
        0                               /* Continue */
    },
    {
        setCompleteState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        CMT_SLV_LIST
    },

    /* End */
    { NULL }
};


static BlockHandler startup_ddr_and_xpio_group[] = {
    /* sequence CPAP_START */
    {
        clearGateReg,
        0, 0,
        CPAP_SLV_LIST
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },

    /* sequence XPLL_DDR_START */
    {
        clearGateReg,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        BLKMASK_XPLL
    },
    {
        setApbEnable,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        BLKMASK_XPLL
    },
    {
        setCompleteState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        0                               /* Continue */
    },

    /* sequence XPHY_START */
    {
        clearGateReg,
        0, 0,
        BLKMASK_XPHY
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_XPHY
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_XPHY
    },
    {
        clearHoldState,
        0, 0,
        BLKMASK_XPHY
    },
    {
        clearTriState,
        0, 0,
        BLKMASK_XPHY
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },

    /* sequence DDRMC_START */
    {
        clearInitState,                 /* was clearGateReg */
        0, 0,
        BLKMASK_DDRMC
    },
    {
        clearSleepState,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        checkCalibrationDdr,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        setFabricEnable,
        XILCDO_NPI_FABRICEN_MASK, XILCDO_NPI_FABRICEN_MASK,
        BLKMASK_DDRMC
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },

    /* End */
    { NULL }
};


static BlockHandler startup_ams_group[] = {
    /* sequence AMS_SAT_START */
    {
        clearGateReg,
        0, 0,
        BLKMASK_AMS_SAT
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_AMS_SAT
    },
    {
        clearHoldState,
        0, 0,
        BLKMASK_AMS_SAT
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearAmsRootPlBypass,
        XILCDO_NPI_FIRST_MASK, XILCDO_NPI_FIRST_MASK,
        BLKMASK_AMS_SAT
    },

    /* End */
    { NULL }
};


static BlockHandler startup_gt_group[] = {
    /* sequence XPIPE_START */
    {
        clearGateReg,
        0, 0,
        BLKMASK_XPIPE
    },
    {
        setFabricEnable,
        0, 0,
        BLKMASK_XPIPE
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_XPIPE
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },

    /* sequence GT_STARTUP */
    {
        clearGateReg,
        0, 0,
        BLKMASK_GT
    },
    {                                   /* TODO: should be unconditional? */
        setFabricEnable,
        XILCDO_NPI_GTY_USED_MASK, XILCDO_NPI_GTY_USED_MASK,
        BLKMASK_GT
    },
    {
        setApbEnable,
        XILCDO_NPI_GTY_USED_MASK | XILCDO_NPI_APBEN_MASK,
        XILCDO_NPI_GTY_USED_MASK | XILCDO_NPI_APBEN_MASK,
        0                               /* Continue */
    },
    {
        clearTriState,
        XILCDO_NPI_GTY_USED_MASK, XILCDO_NPI_GTY_USED_MASK,
        BLKMASK_GT
    },
    {
        clearInitState,
        XILCDO_NPI_GTY_USED_MASK, XILCDO_NPI_GTY_USED_MASK,
        BLKMASK_GT
    },
    {
        setCompleteState,
        XILCDO_NPI_GTY_USED_MASK, XILCDO_NPI_GTY_USED_MASK,
        0                               /* Continue */
    },

    /* End */
    { NULL }
};


static BlockHandler startup_noc_group[] = {
    /* sequence NPS_PCOMPLETE */
    {
        setCompleteState,
        0, 0,
        BLKMASK_NOC_NPS
    },

    /* sequence NCRB_START */
    {
        clearGateReg,
        XILCDO_NPI_RECONFIGURATION_MASK, XILCDO_NPI_RECONFIGURATION_MASK,
        BLKMASK_NOC_NCRB
    },
    {
        clearInitState,
        XILCDO_NPI_RECONFIGURATION_MASK, XILCDO_NPI_RECONFIGURATION_MASK,
        BLKMASK_NOC_NCRB
    },
    {
        clearHoldState,
        XILCDO_NPI_RECONFIGURATION_MASK, XILCDO_NPI_RECONFIGURATION_MASK,
        0                               /* Continue */
    },

    {
        setStartCal,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {
        wait1us,
        XILCDO_NPI_FIRST_MASK, XILCDO_NPI_FIRST_MASK,
        BLKMASK_NOC_NCRB
    },
    {
        clearStartCal,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {                                   /* TODO: should we wait for 1us instead? */
        checkCalibrationNcrb2,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {
        clearODisableNPP,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {
        clearODisableAXI,
        0, 0,
        0                               /* Continue */
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },

    /* sequence NOC_ODISABLE_ASSERT */
    {
        setODisableNPP,
        0, 0,
        BLKMASK_NOC_NMU | BLKMASK_NOC_NSU
    },
    {					/* TODO: should this be a separate write? */
        setODisableAXI,
        0, 0,
        0                               /* Continue */
    },

    /* sequence NSU_SEQUENCE */
    {
        clearGateReg,                   /* FOR PR */
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        clearHoldState,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        setFabricEnable,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        clearODisableNPP,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        clearODisableAXI,
        0, 0,
        0                               /* Continue */
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },

    /* sequence NMU_SEQUENCE */
    {
        clearGateReg,                   /* FOR PR */
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        clearHoldState,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        setFabricEnable,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        clearODisableNPP,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        clearODisableAXI,
        0, 0,
        0                               /* Continue */
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },

    /* End */
    { NULL }
};


static BlockHandler startup_io_modules1[] = {
#if 0
    {
        clearLockState,
        0, 0,
        BLKMASK_VREF
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_XPIO
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_XPLL
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_PLL_PHY
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_XPHY
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },
#endif
    {
        clearGateReg,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },
    {
        clearTriState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        0                               /* Continue */
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_VREF
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    { NULL }
};

static BlockHandler startup_io_modules2[] = {
    {
        clearGateReg,
        0, 0,
        BLKMASK_XPIO
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_XPIO
    },
    {
        clearTriState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        BLKMASK_XPIO_IOMISC
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },
    {
        clearDCIOfcReset,
        0, 0,
        0                               /* Continue */
    },
    { NULL }
};

static BlockHandler startup_io_modules3[] = {
    {
        checkCalibrationDci,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },
    {
        setCompleteState,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },
    {
        clearTriState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        BLKMASK_XPIO
    },
    {
        setCompleteState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        0                               /* Continue */
    },
    { NULL }
};

static BlockHandler startup_mmcm_modules1[] = {
#if 0
    {
        clearLockState,
        0, 0,
        BLKMASK_VERT_TO_HSR
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_VERT_TO_HSR_GT
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_REBUF_VRT
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_REBUF_VRT_GT
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_REBUF_HSR_TNOC_ME
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_HSR_BUFGS
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_VNOC_PS
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_VNOC
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_CLK_GT
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_MMCM
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_DPLL
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_XPLL
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_PLL_PHY
    },
#endif
    {
        clearGateReg,
        0, 0,
        BLKMASK_VERT_TO_HSR
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_VERT_TO_HSR_GT
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_REBUF_VRT
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_REBUF_VRT_GT
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_REBUF_HSR_TNOC_ME
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_HSR_BUFGS
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_VNOC_PS
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_VNOC
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_CLK_GT
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_HSR_BUFGS
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_VNOC_PS
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_VNOC
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_CLK_GT
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_VERT_TO_HSR
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_VERT_TO_HSR_GT
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_REBUF_VRT
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_REBUF_VRT_GT
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_REBUF_HSR_TNOC_ME
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_HSR_BUFGS
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_VNOC_PS
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_VNOC
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_CLK_GT
    },
    {
        setStartCal,
        0, 0,
        BLKMASK_HSR_BUFGS
    },
    {
        setStartCal,
        0, 0,
        BLKMASK_VNOC_PS
    },
    {
        setStartCal,
        0, 0,
        BLKMASK_CLK_GT
    },
    { NULL }
};

static BlockHandler startup_mmcm_modules2[] = {
    {
        setDeskew,
        0, 0,
        BLKMASK_REBUF_VRT
    },
    {
        setDeskew,
        0, 0,
        BLKMASK_REBUF_VRT_GT
    },
    {
        setDeskew,
        0, 0,
        BLKMASK_VNOC_PS
    },
    {
        setDeskew,
        0, 0,
        BLKMASK_VNOC
    },
    {
        setDeskew,
        0, 0,
        BLKMASK_CLK_GT
    },
    { NULL }
};

static BlockHandler startup_mmcm_modules3[] = {
    {
        clearDeskew,
        0, 0,
        BLKMASK_REBUF_VRT
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearDeskew,
        0, 0,
        BLKMASK_REBUF_VRT_GT
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearDeskew,
        0, 0,
        BLKMASK_VNOC_PS
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearDeskew,
        0, 0,
        BLKMASK_VNOC
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearDeskew,
        0, 0,
        BLKMASK_CLK_GT
    },
    {
        clearStartCal,
        0, 0,
        BLKMASK_HSR_BUFGS
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearStartCal,
        0, 0,
        BLKMASK_VNOC_PS
    },
    {
        clearStartCal,
        0, 0,
        BLKMASK_CLK_GT
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        setCompleteState,
        0, 0,
        BLKMASK_VERT_TO_HSR
    },
    {
        setCompleteState,
        0, 0,
        BLKMASK_VERT_TO_HSR_GT
    },
    {
        setCompleteState,
        0, 0,
        BLKMASK_REBUF_HSR_TNOC_ME
    },
    { NULL }
};

static BlockHandler startup_mmcm_modules4[] = {
    {
        clearHoldState,
        0, 0,
        BLKMASK_HSR_BUFGS
    },
    {
        clearHoldState,
        0, 0,
        BLKMASK_VNOC_PS
    },
    {
        clearHoldState,
        0, 0,
        BLKMASK_CLK_GT
    },
    { NULL }
};

static BlockHandler startup_pll_modules1[] = {
    {
        clearGateReg,
        0, 0,
        BLKMASK_MMCM
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_DPLL
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_PLL_PHY
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_XPLL
    },
    {
        setApbEnable,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        BLKMASK_XPLL
    },
    {
        setCompleteState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        0                               /* Continue */
    },
    {
        clearGateReg,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        BLKMASK_XPHY
    },
    {
        clearInitState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        BLKMASK_XPHY
    },
    {
        clearODisable,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        BLKMASK_XPHY
    },
    {
        clearHoldState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        BLKMASK_XPHY
    },
    {
        clearTriState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        BLKMASK_XPHY
    },
    {
        setCompleteState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, XILCDO_NPI_DDRMC_PRESENT_MASK,
        0                               /* Continue */
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        clearUBInitState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearSleepState,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_MMCM
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_DPLL
    },
    {
        clearODisable,
        0, 0,
        0                               /* Continue */
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_XPLL
    },
    {
        clearODisable,
        0, 0,
        0                               /* Continue */
    },
    { NULL }
};

static BlockHandler startup_pll_modules2[] = {
    {
        checkCalibrationPll,
        0, 0,
        BLKMASK_XPLL
    },
    {
        checkCalibrationPll,
        0, 0,
        BLKMASK_DPLL
    },
    {
        checkCalibrationPll,
        0, 0,
        BLKMASK_MMCM
    },
    {
        setCompleteState,
        0, 0,
        BLKMASK_MMCM
    },
    {
        setCompleteState,
        0, 0,
        BLKMASK_DPLL
    },
    {
        setFabricEnable,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        BLKMASK_XPLL
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        setCompleteState,
        0, 0,
        BLKMASK_PLL_PHY
    },
#if 0
    {
        setLockState,
        0, 0,
        BLKMASK_VERT_TO_HSR
    },
    {
        setLockState,
        0, 0,
        BLKMASK_VERT_TO_HSR_GT
    },
    {
        setLockState,
        0, 0,
        BLKMASK_REBUF_VRT
    },
    {
        setLockState,
        0, 0,
        BLKMASK_REBUF_VRT_GT
    },
    {
        setLockState,
        0, 0,
        BLKMASK_REBUF_HSR_TNOC_ME
    },
    {
        setLockState,
        0, 0,
        BLKMASK_HSR_BUFGS
    },
    {
        setLockState,
        0, 0,
        BLKMASK_VNOC_PS
    },
    {
        setLockState,
        0, 0,
        BLKMASK_VNOC
    },
    {
        setLockState,
        0, 0,
        BLKMASK_CLK_GT
    },
    {
        setLockState,
        0, 0,
        BLKMASK_MMCM
    },
    {
        setLockState,
        0, 0,
        BLKMASK_DPLL
    },
    {
        setLockState,
        0, 0,
        BLKMASK_XPLL
    },
    {
        setLockState,
        0, 0,
        BLKMASK_PLL_PHY
    },
#endif
    { NULL }
};

static BlockHandler startup_ddr_modules[] = {
    {
        clearGateReg,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        BLKMASK_XPHY
    },
    {
        clearInitState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        BLKMASK_XPHY
    },
    {
        clearODisable,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        BLKMASK_XPHY
    },
    {
        clearHoldState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        BLKMASK_XPHY
    },
    {
        clearTriState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        BLKMASK_XPHY
    },
    {
        setCompleteState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        0                               /* Continue */
    },
    {
        checkCalibrationDdr,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearPRFreeze,
        XILCDO_NPI_PR_FREEZE_MASK, XILCDO_NPI_PR_FREEZE_MASK,
        BLKMASK_XPIO
    },
    {
        clearTriState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        BLKMASK_XPIO
    },
    {
        setCompleteState,
        XILCDO_NPI_DDRMC_PRESENT_MASK, 0,
        0                               /* Continue */
    },
#if 0
    {
        setLockState,
        0, 0,
        BLKMASK_VREF
    },
    {
        setLockState,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },
    {
        setLockState,
        0, 0,
        BLKMASK_XPHY
    },
    {
        setLockState,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        setLockState,
        0, 0,
        BLKMASK_XPIO
    },
#endif
    { NULL }
};

static BlockHandler startup_ams_modules[] = {
#if 0
    {
        clearLockState,
        0, 0,
        BLKMASK_AMS_ROOT
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_AMS_SAT
    },
#endif
    {
        clearGateReg,
        0, 0,
        BLKMASK_AMS_SAT
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_AMS_SAT
    },
    {
        clearHoldState,
        0, 0,
        BLKMASK_AMS_SAT
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_AMS_ROOT
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_AMS_ROOT
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_AMS_ROOT
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
#if 0
    {
        setLockState,
        0, 0,
        BLKMASK_AMS_ROOT
    },
    {
        setLockState,
        0, 0,
        BLKMASK_AMS_SAT
    },
#endif
    { NULL }
};

static BlockHandler startup_gt_modules1[] = {
#if 0
    {
        clearLockState,
        0, 0,
        BLKMASK_GT
    },
#endif
    /* TODO: should clearInitCtrl be done in house cleaning?? */
    {
        clearInitCtrl,
        XILCDO_NPI_GTY_USED_MASK, XILCDO_NPI_GTY_USED_MASK,
        BLKMASK_GT
    },
    { NULL }
};

static BlockHandler startup_gt_modules2[] = {
#if 0
    {
        clearLockState,
        0, 0,
        BLKMASK_XPIPE
    },
#endif
    {
        clearGateReg,
        0, 0,
        BLKMASK_XPIPE
    },
    {
        setFabricEnable,
        0, 0,
        BLKMASK_XPIPE
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_XPIPE
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_GT
    },
    {
        setFabricEnable,
        XILCDO_NPI_GTY_USED_MASK, XILCDO_NPI_GTY_USED_MASK,
        BLKMASK_GT
    },
    {
        setApbEnable,
        XILCDO_NPI_GTY_USED_MASK | XILCDO_NPI_APBEN_MASK,
        XILCDO_NPI_GTY_USED_MASK | XILCDO_NPI_APBEN_MASK,
        0                               /* Continue */
    },
    {
        clearTriState,
        XILCDO_NPI_GTY_USED_MASK, XILCDO_NPI_GTY_USED_MASK,
        BLKMASK_GT
    },
    {
        clearInitState,
        XILCDO_NPI_GTY_USED_MASK, XILCDO_NPI_GTY_USED_MASK,
        BLKMASK_GT
    },
    {
        setCompleteState,
        XILCDO_NPI_GTY_USED_MASK, XILCDO_NPI_GTY_USED_MASK,
        0                               /* Continue */
    },
#if 0
    {
        setLockState,
        0, 0,
        BLKMASK_GT
    },
    {
        setLockState,
        0, 0,
        BLKMASK_XPIPE
    },
#endif
    { NULL }
};

static BlockHandler startup_noc_modules1[] = {
#if 0
    {
        clearLockState,
        0, 0,
        BLKMASK_NOC_NPS
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_NOC_IDB
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        clearLockState,
        0, 0,
        BLKMASK_NOC_NMU
    },
#endif
    {
        setODisableNPP,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        setODisableNPP,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        setCompleteState,
        0, 0,
        BLKMASK_NOC_NIR
    },
    {
        setCompleteState,
        0, 0,
        BLKMASK_NOC_NPS
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {
        clearHoldState,
        0, 0,
        0                               /* Continue */
    },
    {
        setStartCal,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    { NULL }
};

static BlockHandler startup_noc_modules2[] = {
    {
        checkCalibrationNcrb,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {
        clearODisable,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearODisableNPP,
        0, 0,
        BLKMASK_NOC_IDB
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        clearHoldState,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        setFabricEnable,
        XILCDO_NPI_FABRICEN_MASK, XILCDO_NPI_FABRICEN_MASK,
        BLKMASK_NOC_NSU
    },
    {
        clearODisableNPP,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        clearODisableAXI,
        0, 0,
        0                               /* Continue */
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearGateReg,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        clearHoldState,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        clearInitState,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        setFabricEnable,
        XILCDO_NPI_FABRICEN_MASK, XILCDO_NPI_FABRICEN_MASK,
        BLKMASK_NOC_NMU
    },
    {
        clearODisableNPP,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        clearODisableAXI,
        0, 0,
        0                               /* Continue */
    },
    {
        setCompleteState,
        0, 0,
        0                               /* Continue */
    },
#if 0
    {
        setLockState,
        0, 0,
        BLKMASK_NOC_NPS
    },
    {
        setLockState,
        0, 0,
        BLKMASK_NOC_IDB
    },
    {
        setLockState,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {
        setLockState,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        setLockState,
        0, 0,
        BLKMASK_NOC_NMU
    },
#endif
    { NULL }
};

static void invoke_block_handlers(CdoSequence * seq, CdoCommand * firstcmd, LINK * lend, BlockHandler * table) {
    while (table->handler != NULL) {
        LINK * l = &firstcmd->link_all;
        uint32_t first = 1;
        if (table->typemask == BLKMASK_ALWAYS) {
            uint32_t flags = 0;
            if (first) flags |= XILCDO_NPI_FIRST_MASK;
            if (fabric_startup) flags |= XILCDO_NPI_FABRIC_STARTUP_MASK;
            if (fabric_shutdown) flags |= XILCDO_NPI_RECONFIGURATION_MASK;
            if ((flags & table->flagmask) == table->flagvalue) {
                table->handler(seq, 0);
            }
            table++;
            continue;
        }
        while (l != lend) {
            CdoCommand * cmd = all2cmds(l);
            uint32_t bt = cmd->flags & XILCDO_NPI_BLK_MASK;
            uint32_t flags = cmd->flags & ~XILCDO_NPI_BLK_MASK;
            if (first) flags |= XILCDO_NPI_FIRST_MASK;
            if (fabric_startup) flags |= XILCDO_NPI_FABRIC_STARTUP_MASK;
            if (fabric_shutdown) flags |= XILCDO_NPI_RECONFIGURATION_MASK;
            l = l->next;
            if ((table->typemask & TOMASK32(bt)) != 0 &&
                (flags & table->flagmask) == table->flagvalue) {
                BlockHandler * table2 = table + 1;
                table->handler(seq, cmd->dstaddr);
                while (table2->handler != NULL && table2->typemask == 0) {
                    if ((flags & table2->flagmask) == table2->flagvalue) {
                        if (pcsr_mode == 0) block_continue_mode = 1;
                        table2->handler(seq, cmd->dstaddr);
                        block_continue_mode = 0;
                    }
                    table2++;
                }
                first = 0;
            }
        }
        table++;
    }
}

static void cdocmd_ddr_hack(CdoSequence * seq, CdoCommand * firstcmd, LINK * lend) {
    LINK * l = &firstcmd->link_all;
    cdocmd_add_comment(seq, "DDR hack start");
    while (l != lend) {
        CdoCommand * cmd = all2cmds(l);
        uint32_t bt = cmd->flags & XILCDO_NPI_BLK_MASK;
        uint32_t ba = cmd->dstaddr;
        l = l->next;
        if (bt == XILCDO_NPI_BLK_XPIO) {
            clearLockState(seq, ba);
#if 1
            cdocmd_add_mask_write(seq, ba + 0x3c, 1 << 17, 0);
            cdocmd_add_mask_write(seq, ba + 0x58, 1 << 17, 0);
            cdocmd_add_write(seq, ba + 0x30, 0);
            cdocmd_add_write(seq, ba + 0x4c, 0);
            cdocmd_add_write(seq, ba + 0x60, 0);
            cdocmd_add_write(seq, ba + 0x64, 0);
#endif
            cdocmd_add_write(seq, ba + 0x40, 1);
            cdocmd_add_write(seq, ba + 0x5c, 1);
        }
    }
    cdocmd_add_comment(seq, "DDR hack end");
}

static void rewrite_seq_group(RewriteInfo * ri, CdoCommand * firstcmd, LINK * lend) {
    CdoSequence * seq = cdocmd_create_sequence();
    LINK * l = NULL;

    if (ri->noc_srst_req) {
        ri->noc_srst_req = 0;
        nocSysRst(seq);
    }

    if (ddr_hack_enable) {
        cdocmd_ddr_hack(seq, firstcmd, lend);
    }

    enableCFUWrite(seq);
    if (seq_spec == 0) {
        if(fabric_startup) {
            /* TODO: this needs review. */
            setGlobalSignals(seq);
        }

        invoke_block_handlers(seq, firstcmd, lend, startup_unlock_group);

        invoke_block_handlers(seq, firstcmd, lend, startup_gt_modules1);
        invoke_block_handlers(seq, firstcmd, lend, startup_io_modules1);
        cdocmd_add_delay(seq, 10);
        invoke_block_handlers(seq, firstcmd, lend, startup_io_modules2);
        cdocmd_add_delay(seq, 100);
        invoke_block_handlers(seq, firstcmd, lend, startup_io_modules3);
        invoke_block_handlers(seq, firstcmd, lend, startup_mmcm_modules1);
        setGclkCal(seq, 0);
        invoke_block_handlers(seq, firstcmd, lend, startup_mmcm_modules2);
        cdocmd_add_delay(seq, 100);
        invoke_block_handlers(seq, firstcmd, lend, startup_mmcm_modules3);
        clearGclkCal(seq, 0);
        invoke_block_handlers(seq, firstcmd, lend, startup_mmcm_modules4);
        if(fabric_startup) {
            setGWE(seq, 0);
        }
        invoke_block_handlers(seq, firstcmd, lend, startup_pll_modules1);
        cdocmd_add_delay(seq, 100);
        invoke_block_handlers(seq, firstcmd, lend, startup_pll_modules2);
        invoke_block_handlers(seq, firstcmd, lend, startup_ddr_modules);
        invoke_block_handlers(seq, firstcmd, lend, startup_ams_modules);
        invoke_block_handlers(seq, firstcmd, lend, startup_gt_modules2);
        invoke_block_handlers(seq, firstcmd, lend, startup_noc_modules1);
        cdocmd_add_delay(seq, 100);
        invoke_block_handlers(seq, firstcmd, lend, startup_noc_modules2);

        invoke_block_handlers(seq, firstcmd, lend, startup_lock_group);

        if(fabric_startup) {
            /* TODO: this needs review. */
            assertGlobalSignals(seq);
            fabric_startup = 0;
        }
    } else if (seq_spec == 1) {
        invoke_block_handlers(seq, firstcmd, lend, startup_unlock_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_xpio_group);
        if(fabric_startup) {
            setGlobalSignals(seq);
        }
        invoke_block_handlers(seq, firstcmd, lend, startup_mmcm_clk_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_ddr_and_xpio_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_ams_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_gt_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_noc_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_lock_group);
        if(fabric_startup) {
            assertGlobalSignals(seq);
            fabric_startup = 0;
        }
    } else if (seq_spec == 2) {
        invoke_block_handlers(seq, firstcmd, lend, startup_unlock_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_xpio_group);
        if(fabric_startup) {
            setGlobalSignals(seq);
        }
        invoke_block_handlers(seq, firstcmd, lend, startup_ddr_and_xpio_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_mmcm_clk_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_ams_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_gt_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_noc_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_lock_group);
        if(fabric_startup) {
            assertGlobalSignals(seq);
            fabric_startup = 0;
        }
    } else if (seq_spec == 3) {
        invoke_block_handlers(seq, firstcmd, lend, startup_unlock_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_xpio_group);
        if(fabric_startup) {
            setGlobalSignals(seq);
        }
        invoke_block_handlers(seq, firstcmd, lend, startup_ddr_and_xpio_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_noc_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_mmcm_clk_group3);
        invoke_block_handlers(seq, firstcmd, lend, startup_ams_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_gt_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_lock_group);
        if(fabric_startup) {
            assertGlobalSignals(seq);
            fabric_startup = 0;
        }
    } else {
        invoke_block_handlers(seq, firstcmd, lend, startup_unlock_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_xpio_group);
        if(fabric_startup) {
            setGlobalSignals(seq);
        }
        invoke_block_handlers(seq, firstcmd, lend, startup_ddr_and_xpio_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_noc_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_mmcm_clk_group4);
        invoke_block_handlers(seq, firstcmd, lend, startup_ams_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_gt_group);
        invoke_block_handlers(seq, firstcmd, lend, startup_lock_group);
        if(fabric_startup) {
            assertGlobalSignals(seq);
            fabric_startup = 0;
        }
    }
    disableCFUWrite(seq);

    cdocmd_insert_seq(firstcmd, seq);

    l = &firstcmd->link_all;
    while (l != lend) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        cdocmd_free(cmd);
    }
    cdocmd_delete_sequence(seq);
}

static BlockHandler shutdown_noc_modules[] = {
    /* sequence NMU_QUIESCE */
    {
        setAxiReqRejectMode,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        checkNmuRegBusy,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        clearCompleteState,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        setODisableAXI,
        0, 0,
        0                               /* Continue */
    },
    {
        setODisableNPP,
        0, 0,
        0                               /* Continue */
    },
    {
        clearFabricEnable,
        0, 0,
        0                               /* Continue */
    },
    {
        setInitState,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        setHoldState,
        0, 0,
        0                               /* Continue */
    },
    {
        setGateReg,
        0, 0,
        BLKMASK_NOC_NMU
    },
    {
        clearAxiReqRejectMode,
        0, 0,
        BLKMASK_NOC_NMU
    },

    /* sequence NSU_QUIESCE */
    {
        setPRMode,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        checkNsuRegBusy,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        checkRegPendingBurst,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        clearCompleteState,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        setODisableAXI,
        0, 0,
        0                               /* Continue */
    },
    {
        setODisableNPP,
        0, 0,
        0                               /* Continue */
    },
    {
        clearFabricEnable,
        0, 0,
        0                               /* Continue */
    },
    {
        setInitState,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        setHoldState,
        0, 0,
        0                               /* Continue */
    },
    {
        setGateReg,
        0, 0,
        BLKMASK_NOC_NSU
    },
    {
        clearPRMode,
        0, 0,
        BLKMASK_NOC_NSU
    },

    /* sequence NCRB_SHUTDOWN */
    /* This is now obsolete.  NCRBs will never be shut down. */
#if 0
    {
        clearCompleteState,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {
        setODisable,
        0, 0,
        0                               /* Continue */
    },
    {
        setInitState,
        0, 0,
        BLKMASK_NOC_NCRB
    },
    {
        setHoldState,
        0, 0,
        0                               /* Continue */
    },
    {
        setGateReg,
        0, 0,
        BLKMASK_NOC_NCRB
    },
#endif

    /* sequence NPS_D_PCOMPLETE */
    {
        clearCompleteState,
        0, 0,
        BLKMASK_NOC_NPS
    },
    { NULL }
};

static BlockHandler shutdown_gt_modules[] = {
    /* sequence GT_SHUTDOWN */
    {
        setShutDown,
        0, 0,
        BLKMASK_GT
    },
    {
        checkShutDown,
        0, 0,
        BLKMASK_GT
    },
    {
        clearCompleteState,
        0, 0,
        BLKMASK_GT
    },
    {
        setTriState,
        0, 0,
        0                               /* Continue */
    },
    {
        clearFabricEnable,
        0, 0,
        BLKMASK_GT
    },
    {
        clearApbEnable,
        0, 0,
        0                               /* Continue */
    },
    {
        setInitState,
        0, 0,
        BLKMASK_GT
    },
    {
        setInitCtrl,
        0, 0,
        0                               /* Continue */
    },
    {
        setGateReg,
        0, 0,
        0                               /* Continue */
    },
    {
        clearShutDown,
        0, 0,
        0                               /* Continue */
    },
    {
        clearInitCtrl,
        0, 0,
        BLKMASK_GT
    },

    /* sequence XPIPE_SHUTDOWN */
    {
        clearCompleteState,
        0, 0,
        BLKMASK_XPIPE
    },
    {
        setInitState,
        0, 0,
        0                               /* Continue */
    },
    {
        setGateReg,
        0, 0,
        BLKMASK_XPIPE
    },
    {
        clearFabricEnable,
        0, 0,
        0                               /* Continue */
    },
    { NULL }
};

static BlockHandler shutdown_ddr_modules[] = {
    /* sequence DDRMC_NOC_SHUTDOWN */
    {
        /* Set DDR Noc clock MUX reg to Noc/2 mode */
        resetClkMux,
        0, 0,
        BLKMASK_DDRMC
    },

    /* sequence DDRMC_SHUTDOWN */
    {
        clearCompleteState,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        setODisable,
        0, 0,
        0                               /* Continue */
    },
    {
        setInitState,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        setUBInitState,
        0, 0,
        0                               /* Continue */
    },
    {
        setSleepState,
        0, 0,
        BLKMASK_DDRMC
    },
    {
        clearUBInitState,
        0, 0,
        BLKMASK_DDRMC
    },

    /* sequence XPHY_SHUTDOWN */
    {
        clearCompleteState,
        0, 0,
        BLKMASK_XPHY
    },
    {
        setODisable,
        0, 0,
        0                               /* Continue */
    },
    {
        setTriState,
        0, 0,
        0                               /* Continue */
    },
    {
        setGateReg,
        0, 0,
        BLKMASK_XPHY
    },
    {
        setInitState,
        0, 0,
        0                               /* Continue */
    },
    {
        setHoldState,
        0, 0,
        0                               /* Continue */
    },
    { NULL }
};

#define CLK_DIST_LIST (                         \
        BLKMASK_VERT_TO_HSR |                   \
        BLKMASK_VERT_TO_HSR_GT |                \
        BLKMASK_REBUF_HSR_TNOC_ME |             \
        BLKMASK_VNOC |                          \
        BLKMASK_REBUF_VRT |                     \
        BLKMASK_REBUF_VRT_GT)

static BlockHandler shutdown_mmcm_modules[] = {
    /* sequence CLK_BUFG_SHUTDOWN */
    {
        clearCompleteState,
        0, 0,
        BLKMASK_HSR_BUFGS | BLKMASK_VNOC_PS | BLKMASK_CLK_GT
    },
    {
        setODisable,
        0, 0,
        0                               /* Continue */
    },
    {
        setHoldState,
        0, 0,
        BLKMASK_HSR_BUFGS | BLKMASK_VNOC_PS | BLKMASK_CLK_GT
    },

    /* sequence CMT_SHUTDOWN */
    {
        clearCompleteState,
        0, 0,
        CMT_SLV_LIST
    },
    {
        setInitState,
        0, 0,
        0                               /* Continue */
    },
    {
        setODisable,
        0, 0,
        0                               /* Continue */
    },
    {
        clearApbEnable,
        0, 0,
        CMT_SLV_LIST
    },
    {
        clearFabricEnable,
        0, 0,
        0                               /* Continue */
    },
    {
        setGateReg,
        0, 0,
        CMT_SLV_LIST
    },

    /* sequence CPAP_SHUTDOWN */
    {
        clearCompleteState,
        0, 0,
        BLKMASK_PLL_PHY
    },
    {
        setGateReg,
        0, 0,
        0                               /* Continue */
    },

    /* sequence CLK_DIST_SHUTDOWN */
    {
        clearCompleteState,
        0, 0,
        CLK_DIST_LIST
    },
    {
        setODisable,
        0, 0,
        0                               /* Continue */
    },
    { NULL }
};

static BlockHandler shutdown_io_modules[] = {
    /* sequence XPIO_SHUTDOWN */
    {
        clearCompleteState,
        0, 0,
        BLKMASK_XPIO
    },
    {
        setTriState,
        0, 0,
        0                               /* Continue */
    },
    {
        setODisable,
        0, 0,
        BLKMASK_XPIO
    },
    {
        setGateReg,
        0, 0,
        0                               /* Continue */
    },

    /* sequence DCI_SHUTDOWN */
    {
        clearCompleteState,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },
    {
        setInitState,
        0, 0,
        0                               /* Continue */
    },
    {
        setDCIOfcReset,
        0, 0,
        0                               /* Continue */
    },
    {
        setGateReg,
        0, 0,
        BLKMASK_XPIO_IOMISC
    },
    {
        setTriState,
        0, 0,
        0                               /* Continue */
    },

    /* sequence VREF_SHUTDOWN */
    {
        clearCompleteState,
        0, 0,
        BLKMASK_VREF
    },
    {
        setGateReg,
        0, 0,
        0                               /* Continue */
    },

    /* sequence XPIO_ASSERT_PR_FREEZE */
    {
        setPRFreeze,
        XILCDO_NPI_PR_FREEZE_MASK, XILCDO_NPI_PR_FREEZE_MASK,
        BLKMASK_XPIO
    },
    { NULL }
};

static void rewrite_shutdown_group(RewriteInfo * ri, CdoCommand * firstcmd, LINK * lend) {
    CdoSequence * seq = cdocmd_create_sequence();
    LINK * l = NULL;

    invoke_block_handlers(seq, firstcmd, lend, startup_unlock_group);
    invoke_block_handlers(seq, firstcmd, lend, shutdown_noc_modules);
    invoke_block_handlers(seq, firstcmd, lend, shutdown_gt_modules);
    invoke_block_handlers(seq, firstcmd, lend, shutdown_ddr_modules);
    invoke_block_handlers(seq, firstcmd, lend, shutdown_mmcm_modules);
    invoke_block_handlers(seq, firstcmd, lend, shutdown_io_modules);
    invoke_block_handlers(seq, firstcmd, lend, startup_lock_group);

    cdocmd_insert_seq(firstcmd, seq);

    l = &firstcmd->link_all;
    while (l != lend) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        cdocmd_free(cmd);
    }
    cdocmd_delete_sequence(seq);
}

static void rewrite_command_group(RewriteInfo * ri, CdoCommand * firstcmd, LINK * lend) {
    switch (firstcmd->type) {
    case CdoCmdNpiPreCfg:
        rewrite_precfg_group(ri, firstcmd, lend);
        break;
    case CdoCmdNpiSeq:
        rewrite_seq_group(ri, firstcmd, lend);
        break;
    case CdoCmdNpiShutdown:
        rewrite_shutdown_group(ri, firstcmd, lend);
        break;
    default:
        assert(!"unexpected command type");
    }
}

static int is_npi_cmd(CdoCommand * cmd) {
    if (cmd->type == CdoCmdNpiPreCfg ||
        cmd->type == CdoCmdNpiSeq ||
        cmd->type == CdoCmdNpiShutdown) return 1;
    return 0;
}

static int is_cfg_cmd(CdoCommand * cmd) {
    if (cmd->type == CdoCmdWrite ||
        cmd->type == CdoCmdSetBlock ||
        cmd->type == CdoCmdWriteKeyhole ||
        cmd->type == CdoCmdMaskWrite ||
        cmd->type == CdoCmdMaskPoll ||
        cmd->type == CdoCmdDelay ||
        cmd->type == CdoCmdDmaXfer) return 1;
    return 0;
}

static void cdocmd_rewrite_npi(CdoSequence * seq) {
    RewriteInfo rewriteinfo;
    RewriteInfo * ri = &rewriteinfo;
    LINK * l;
    CdoCommand * firstcmd = NULL;
    memset(ri, 0, sizeof *ri);
    if (seq->version < 0x200) {
        cdoseq_upgrade(seq, 0x200);
    }
    l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if (firstcmd == NULL) {
            if (is_npi_cmd(cmd)) {
                firstcmd = cmd;
            }
        } else if (firstcmd->type != cmd->type) {
            if (firstcmd->type == CdoCmdNpiSeq && is_cfg_cmd(cmd)) {
                /* npi_seq commands are followed by configuration
                 * commands, find the end of the configuration
                 * commands and move the npi_seq commands to that
                 * spot. */
                CdoCommand * cmd2 = NULL;
                LINK * l2 = l;
                while (l2 != &seq->cmds) {
                    cmd2 = all2cmds(l2);
                    l2 = l2->next;
                    if (!is_cfg_cmd(cmd2)) break;
                    cmd2 = NULL;
                }
                if (cmd2 && cmd2->type == CdoCmdNpiSeq) {
                    /* Combine groups of npi_seq commands.  Don't move
                     * npi_seq commands to the end since newer
                     * versions of Vivado generates isolation commands
                     * after npi_seq commands.  */
                    while (is_npi_cmd(firstcmd)) {
                        l2 = firstcmd->link_all.next;
                        if (cmd2 != NULL) {
                            cdocmd_insert_command(cmd2, firstcmd);
                        } else {
                            cdocmd_append_command(seq, firstcmd);
                        }
                        firstcmd = all2cmds(l2);
                    }
                    l = cmd->link_all.next;
                    firstcmd = NULL;
                    continue;
                }
            }
            rewrite_command_group(ri, firstcmd, &cmd->link_all);
            if (is_npi_cmd(cmd)) {
                firstcmd = cmd;
            } else {
                firstcmd = NULL;
            }
        }
    }
    if (firstcmd != NULL) {
        rewrite_command_group(ri, firstcmd, l);
    }
}

static uint64_t spp_ddr_filter[] = {
    0xf6090000, 0xf6099fff,
    0xf609a000, 0xf609afff,
    0xf6160000, 0xf6169fff,
    0xf616a000, 0xf616afff,
    0xf61c0000, 0xf61c9fff,
    0xf61ca000, 0xf61cafff,
    0xf6110000, 0xf6110fff,
    0xf612c000, 0xf612cfff,
    0xf6130000, 0xf6130fff,
    0xf6150000, 0xf6150fff,
    0xf6280000, 0xf628ffff,
    0xf63f0000, 0xf63fffff,
    0xf6560000, 0xf656ffff,
    0xf6890000, 0xf689ffff,
    0xf68f0000, 0xf68fffff,
    0xf6940000, 0xf694ffff,
    0xf6990000, 0xf699ffff,
    0xf69e0000, 0xf69effff,
    0xf6a30000, 0xf6a3ffff,
    0xf6a90000, 0xf6a9ffff,
    0xf70e0000, 0xf70effff,
    0xf7150000, 0xf715ffff,
    0xf71b0000, 0xf71bffff,
    0xf7210000, 0xf721ffff,
    0xf6071000, 0xf607ffff,
    0xf6b10000, 0xf6b1ffff,
    0xf6b60000, 0xf6b6ffff,
    0xf6ce0000, 0xf6ceffff,
    0xf6d30000, 0xf6d3ffff,
    0xf6d80000, 0xf6d8ffff,
    0xf70d0000, 0xf70dffff,
    0xf7140000, 0xf714ffff,
    0xf7200000, 0xf720ffff,
    //    0xf70a0000, 0xf70affff,
    0, 0
};

static uint32_t filter_range(uint64_t * filter, CdoCommand * cmd) {
    CdoSequence * seq;
    uint64_t laddr;
    uint64_t haddr;
    uint64_t lfilter;
    uint64_t hfilter;
    switch (cmd->type) {
    case CdoCmdWrite:
    case CdoCmdSetBlock:
        laddr = cmd->dstaddr;
        haddr = cmd->dstaddr + cmd->count*4 - 1;
        break;

    case CdoCmdMaskWrite:
    case CdoCmdMaskPoll:
    case CdoCmdNpiPreCfg:
    case CdoCmdNpiSeq:
    case CdoCmdNpiShutdown:
        laddr = cmd->dstaddr;
        haddr = cmd->dstaddr + 4 - 1;
        break;

    default:
        assert(!"unexpected command type");
        return 0;
    }
    for (;;) {
        lfilter = *filter++;
        hfilter = *filter++;
        if (lfilter == 0 && hfilter == 0) return 0;
        if (laddr < hfilter && lfilter < haddr) break;
    }
    seq = cdocmd_create_sequence();
    switch (cmd->type) {
    case CdoCmdWrite:
        if (laddr < lfilter) {
            cdocmd_add_block_write(seq, laddr, (lfilter - laddr)/4, cmd->buf, is_be_host());
        }
        if (haddr > hfilter) {
            cdocmd_add_block_write(seq, hfilter + 1, (haddr - hfilter)/4, (char *)cmd->buf + hfilter + 1 - laddr, is_be_host());
        }
        break;

    case CdoCmdSetBlock:
        if (laddr < lfilter) {
            cdocmd_add_block_set(seq, laddr, (lfilter - laddr)/4, cmd->value);
        }
        if (haddr > hfilter) {
            cdocmd_add_block_set(seq, hfilter + 1, (haddr - hfilter)/4, cmd->value);
        }
        break;

    case CdoCmdMaskWrite:
    case CdoCmdMaskPoll:
    case CdoCmdNpiPreCfg:
    case CdoCmdNpiSeq:
    case CdoCmdNpiShutdown:
        assert(laddr >= lfilter);
        assert(haddr <= hfilter);
        break;

    default:
        assert(!"unexpected command type");
        cdocmd_delete_sequence(seq);
        return 0;
    }
    cdocmd_insert_seq(cmd, seq);
    cdocmd_free(cmd);
    return 1;
}

static uint32_t apply_filter(CdoSequence * seq, uint64_t * filter_list) {
    uint32_t update = 0;

    for (;;) {
        LINK * l = seq->cmds.next;
        uint32_t restart = 0;
        while (l != &seq->cmds) {
            CdoCommand * cmd = all2cmds(l);
            l = l->next;
            switch (cmd->type) {
            case CdoCmdWrite:
            case CdoCmdSetBlock:
            case CdoCmdMaskWrite:
            case CdoCmdMaskPoll:
            case CdoCmdNpiPreCfg:
            case CdoCmdNpiSeq:
            case CdoCmdNpiShutdown:
                restart |= filter_range(filter_list, cmd);
                break;

            default:
                break;
            }
        }
        if (!restart) break;
        update = 1;
    }
    return update;
}

static uint32_t cdocmd_rewrite_pl_bypass(CdoSequence * seq) {
#ifdef ENABLE_Slaves
    uint64_t filter_list[4000];
    uint32_t filter_count = 0;

    addSlaveFilter(filter_list, &filter_count, "xpipe_npi_slave");
    addSlaveFilter(filter_list, &filter_count, "gty_npi_slave");
    addSlaveFilter(filter_list, &filter_count, "clk_vert_to_hsr_core");
    addSlaveFilter(filter_list, &filter_count, "clk_vert_to_hsr_gt_core");
    addSlaveFilter(filter_list, &filter_count, "clk_rebuf_vert_gt_core");
    addSlaveFilter(filter_list, &filter_count, "clk_rebuf_vert_core");
    addSlaveFilter(filter_list, &filter_count, "clk_rebuf_bufgs_hsr_core");
    addSlaveFilter(filter_list, &filter_count, "clk_vnoc_core");
    addSlaveFilter(filter_list, &filter_count, "clk_vnoc_ps_core");
    addSlaveFilter(filter_list, &filter_count, "clk_gt_core");
    addSlaveFilter(filter_list, &filter_count, "clk_rebuf_hsr_me_tnoc_core");
    addSlaveFilter(filter_list, &filter_count, "cmt_mmcm");
    addSlaveFilter(filter_list, &filter_count, "cmt_dpll");
    filter_list[filter_count++] = 0;
    filter_list[filter_count++] = 0;

    return apply_filter(seq, filter_list);
#else
    return 0;
#endif
}

/*static uint32_t parse_u64(char ** sp, uint64_t * valuep) {
    char * s = *sp;
    skipsp(s);
    errno = 0;
    *valuep = (uint64_t)strtoull(s, sp, 0);
    if (s == *sp || errno != 0) return 1;
    return 0;
};*/

void cdocmd_rewrite_address_filter(CdoSequence * seq, const char * address_filter_file) {
#ifdef ENABLE_Slaves
    uint64_t filter_list[4000];
    uint32_t filter_count = 0;
    FILE * f = fopen(address_filter_file, "r");
    char line[1024];
    if (f == NULL) {
        perror("fopen");
        exit(1);
        return;
    }
    while (fgets(line, sizeof line, f) != NULL) {
        char * s = line;
        uint32_t len = strlen(line);
        if (len == 0) break;
        if (line[len-1] == '\n') len--;
        line[len] = '\0';
        skipsp(s);
        if (*s == '#') continue;
        if (*s == '-') {
            uint32_t prev_count = filter_count;
            s++;
            skipsp(s);
            if (*s >= '0' && *s <= '9') {
                uint64_t addr;
                uint64_t len;
                if (parse_u64(&s, &addr) || !isspc(*s)) {
                    fprintf(stderr, "invalid filter address: %s\n", line);
                    exit(1);
                }
                skipsp(s);
                if (parse_u64(&s, &len) || addr + len - 1 < addr) {
                    fprintf(stderr, "invalid filter address: %s\n", line);
                    exit(1);
                }
                filter_list[filter_count++] = addr;
                filter_list[filter_count++] = addr + len - 1;
            } else {
                char * p = s;
                skiptok(s);
                *s++ = '\0';
                addSlaveFilter(filter_list, &filter_count, p);
                if (prev_count == filter_count) {
                    fprintf(stderr, "no module matching: %s\n", p);
                    exit(1);
                }
            }
        } else {
            fprintf(stderr, "invalid line: %s\n", line);
            exit(1);
        }
    }
    fclose(f);
    filter_list[filter_count++] = 0;
    filter_list[filter_count++] = 0;
    apply_filter(seq, filter_list);
#endif
}

static uint32_t cdocmd_rewrite_sysmon_bypass(CdoSequence * seq) {
    uint32_t update = 0;
    LINK * l;
    l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if (cmd->type == CdoCmdPmAddNode) {
            switch (cmd->id) {
            case PM_POWER_VCCINT_PMC:
            case PM_POWER_VCCAUX_PMC:
            case PM_POWER_VCCINT_PSLP:
            case PM_POWER_VCCINT_PSFP:
            case PM_POWER_VCCINT_SOC:
            case PM_POWER_VCCINT_RAM:
            case PM_POWER_VCCAUX:
            case PM_POWER_VCCINT_PL:
                cdocmd_free(cmd);
                update = 1;
                break;
            default:
                break;
            }
        }
    }
    return update;
}

static uint32_t cdocmd_rewrite_houseclean_bypass(CdoSequence * seq) {
    uint32_t update = 0;
    LINK * l;
    l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case CdoCmdPmInitNode:
            switch (cmd->id) {
            case PM_POWER_LPD:
                if ((houseclean_bypass & houseclean_bypass_lpd) == 0) break;
                if (cmd->value == NODE_INIT_SCAN_CLEAR ||
                    cmd->value == NODE_INIT_BISR_TRIM ||
                    cmd->value == NODE_INIT_LBIST ||
                    cmd->value == NODE_INIT_MBIST) {
                    cdocmd_free(cmd);
                    update = 1;
                }
                break;

            case PM_POWER_NOC:
                if ((houseclean_bypass & houseclean_bypass_npd) == 0) break;
                if (cmd->value == NODE_INIT_SCAN_CLEAR ||
                    cmd->value == NODE_INIT_BISR_TRIM ||
                    cmd->value == NODE_INIT_LBIST ||
                    cmd->value == NODE_INIT_MBIST) {
                    cdocmd_free(cmd);
                    update = 1;
                }
                break;

            case PM_POWER_CPM:
                if ((houseclean_bypass & houseclean_bypass_cpm) == 0) break;
                if (cmd->value == NODE_INIT_SCAN_CLEAR ||
                    cmd->value == NODE_INIT_BISR_TRIM ||
                    cmd->value == NODE_INIT_LBIST ||
                    cmd->value == NODE_INIT_MBIST) {
                    cdocmd_free(cmd);
                    update = 1;
                }
                break;

            case PM_POWER_FPD:
                if ((houseclean_bypass & houseclean_bypass_fpd) == 0) break;
                if (cmd->value == NODE_INIT_SCAN_CLEAR ||
                    cmd->value == NODE_INIT_BISR_TRIM ||
                    cmd->value == NODE_INIT_LBIST ||
                    cmd->value == NODE_INIT_MBIST) {
                    if (init_finish_move_enable && !fpd_init_finish_move) {
                        CdoSequence * seq2 = cdocmd_create_sequence();
                        cdocmd_add_pm_init_node(seq2, PM_POWER_FPD, NODE_INIT_FINISH, 0, NULL, 0);
                        cdocmd_insert_seq(cmd, seq2);
                        fpd_init_finish_move = 1;
                        cdocmd_delete_sequence(seq2);
                    }
                    cdocmd_free(cmd);
                    update = 1;
                } else if (cmd->value == NODE_INIT_FINISH && fpd_init_finish_move) {
                    cdocmd_free(cmd);
                    update = 1;
                }
                break;

            case PM_POWER_PLD:
                if ((houseclean_bypass & houseclean_bypass_plpd) == 0) break;
                if (cmd->value == NODE_INIT_HOUSE_CLEAN_PLPD && cmd->count == 0) {
                    uint32_t count = 1;
                    uint32_t * p = (uint32_t *)malloc(count * 4);
                    if (cmd->buf) free(cmd->buf);
                    cmd->count = count;
                    cmd->buf = p;
                    p[0] = 1;
                    update = 1;
                }
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }
    }
    return update;
}

static uint32_t cdocmd_rewrite_npi_block_type(CdoSequence * seq) {
    uint32_t update = 0;
    LINK * l;
    l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case CdoCmdNpiPreCfg:
        case CdoCmdNpiSeq:
        case CdoCmdNpiShutdown: {
            uint32_t bt = cmd->flags & XILCDO_NPI_BLK_MASK;
            NpiModule * m = find_npi_module_addr(cmd->dstaddr);
            if (m == NULL) {
                printf("unknown NPI module address %"PRIx64"\n", cmd->dstaddr);
                break;
            }
            if (bt != m->type) {
                printf("update invalid block for %"PRIx64" from %"PRIu32" to %"PRIu32"\n", cmd->dstaddr, bt, m->type);
                cmd->flags = (cmd->flags & ~XILCDO_NPI_BLK_MASK) | m->type;
                update = 1;
            }
            break;
        }

        default:
            break;
        }
    }
    return update;
}

static void cdocmd_rewrite_cfu(CdoSequence * seq) {
    LINK * l;
    if (seq->version < 0x200) {
        cdoseq_upgrade(seq, 0x200);
    }
    l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case CdoCmdCfuSetCrc32:
            if (crc32_write < MAX_CRC32) {
                crc32_check[crc32_write] = cmd->flags;
                crc32_value[crc32_write] = cmd->value;
                crc32_write++;
            }
            cdocmd_free(cmd);
            break;

        case CdoCmdCfuDecompress:
            decompress = cmd->flags;
            cdocmd_free(cmd);
            break;

        case CdoCmdCfuCramRW:
            cram_rw_set = 1;
            cram_rw_value = cmd->value;
            cdocmd_free(cmd);
            break;

        case CdoCmdCfuSeuGo:
            seu_go = cmd->flags;
            cdocmd_free(cmd);
            break;

        case CdoCmdCfuCrc8Dis:
            crc8_dis = cmd->flags;
            cdocmd_free(cmd);
            break;

        case CdoCmdCfuSsiPerSlrPr:
            ssi_per_slr_pr = cmd->flags;
            cdocmd_free(cmd);
            break;

        case CdoCmdCfuGsrGsc:
            gsr_gsc = cmd->flags;
            cdocmd_free(cmd);
            break;

        case CdoCmdCfuGcapClkEn:
            gcap_clk_en = cmd->flags;
            cdocmd_free(cmd);
            break;

        case CdoCmdCfuCfiType:
            cfi_type_valid = 1;
            cfi_type = cmd->value;
            cdocmd_free(cmd);
            break;

        default:
            assert(!"unexpected command type");
        }
    }
}

static void categorize_seq(CdoSequence * seq, CdoCategories * cats) {
    LINK * l;
    uint32_t cfu_mask = 0;
    l = seq->cmds.next;
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case CdoCmdWrite:
            if (cmd->count < 1) break;
            if (cmd->dstaddr == CFU_APB_CFU_MASK) {
                cfu_mask = *(uint32_t *)cmd->buf;
            } else if (cmd->dstaddr == CFU_APB_CFU_CTL) {
                cfu_mask = 0;
            } else if (cmd->dstaddr == CFU_APB_CFU_FGCR) {
                if ((*(uint32_t *)cmd->buf & cfu_mask & CFU_APB_CFU_FGCR_EOS_MASK) != 0) {
                    cats->has_startup = 1;
                }
                cfu_mask = 0;
            }
            break;

        case CdoCmdSection:
            break;

        case CdoCmdNpiPreCfg: {
            uint32_t bt = cmd->flags & XILCDO_NPI_BLK_MASK;
            cats->precfg |= 1 << bt;
            break;
        }

        case CdoCmdNpiSeq: {
            uint32_t bt = cmd->flags & XILCDO_NPI_BLK_MASK;
            cats->seq |= 1 << bt;
            break;
        }

        case CdoCmdNpiShutdown: {
            uint32_t bt = cmd->flags & XILCDO_NPI_BLK_MASK;
            cats->shutdown |= 1 << bt;
            break;
        }

        case CdoCmdNpiWrite:
            cats->npiwrite |= 1;
            break;

        case CdoCmdCfuSetCrc32:
        case CdoCmdCfuDecompress:
        case CdoCmdCfuCramRW:
        case CdoCmdCfuSeuGo:
        case CdoCmdCfuCrc8Dis:
        case CdoCmdCfuSsiPerSlrPr:
        case CdoCmdCfuGsrGsc:
        case CdoCmdCfuGcapClkEn:
        case CdoCmdCfuCfiType:
            cats->cfu |= 1;
            break;

        default:
            cats->generic |= 1;
            break;
        }
    }
}

void cdocmd_post_process_start() {
    char * s = getenv("POST_PROCESS_DEBUG");
    debug_print = s != NULL && *s != '\0';
    s = getenv("POST_PROCESS_SPP");
    if (s != NULL) {
        int mode = atoi(s);
        if (mode != 0) {
            set_platform_spp();
            if (mode & 2) set_spp_ddr_enable(0);
        }
    }
    crc32_write = 0;
    crc32_read = 0;
    decompress = 0;
    cram_rw_set = 0;
    cram_rw_value = 0;
    seu_go = 0;
    crc8_dis = 0;
    ssi_per_slr_pr = 0;
    gsr_gsc = 0;
    gcap_clk_en = 0;
    cfi_type_valid = 0;
    cfi_type = 0;
    fabric_shutdown = 0;
    fabric_startup = 0;
    plpd_houseclean_done = 0;
}

uint32_t cdocmd_post_process_end_seq(CdoSequence ** seqp) {
    CdoSequence * seq = NULL;
    if (fabric_startup) {
        fabric_startup = 0;
        seq = cdocmd_create_sequence();
        seq->version = 0x200;
        enableCFUWrite(seq);
        cfi_startup(seq);
        disableCFUWrite(seq);
    }
    *seqp = seq;
    return 0;
}

void cdocmd_post_process_end(void ** new_data, size_t * new_size) {
    /* TODO: change treturn type to return pass/fail */
    CdoSequence * seq;
    *new_data = NULL;
    *new_size = 0;
    if (cdocmd_post_process_end_seq(&seq)) return;
    if (seq == NULL) return;
    *new_data = cdoseq_to_binary(seq, new_size, 0);
    cdocmd_delete_sequence(seq);
    return;
}

uint32_t cdocmd_post_process_cdo_seq(const void * data, size_t size, CdoSequence ** seqp) {
    CdoSequence * seq = decode_cdo_binary(data, size);
    CdoCategories cats;
    uint32_t update = 0;
    *seqp = NULL;
    if (seq == NULL) return 1;
    memset(&cats, 0, sizeof cats);
    categorize_seq(seq, &cats);

    if (seq->version < 0x200) {
        cdoseq_upgrade(seq, 0x200);
        update = 1;
    }
    if (cats.precfg || cats.seq || cats.shutdown) {
        if (npi_mode) {
            fprintf(stderr, "unexpected NPI commands while NPI processing is disabled\n");
            goto error;
        }
        update |= cdocmd_rewrite_npi_block_type(seq);
    }
    if (cats.cfu) {
        if (cats.generic || cats.precfg || cats.seq || cats.shutdown) {
            fprintf(stderr, "unexpected commands in CFI-CDO\n");
            goto error;
        }
        cdocmd_rewrite_cfu(seq);
        update = 1;
    } else if (cats.shutdown) {
        if (cats.precfg) {
            fprintf(stderr, "unexpected NPI precfg in NPI shutdown CDO\n");
            goto error;
        }
        cdocmd_rewrite_npi(seq);
        update = 1;
    } else if (cats.precfg || cats.seq) {
        cdocmd_rewrite_npi(seq);
        update = 1;
    } else if (cats.has_startup) {
        /* When WDI does startup sequences (CSL) */
        fabric_startup = 0;
    }
    if (is_platform_spp()) {
        update |= apply_filter(seq, spp_ddr_filter);
        if (spp_ddr_ena && cats.precfg && cats.seq) {
            if (spp_ddr_init(seq, spp_ddr_ena)) {
                fprintf(stderr, "unexpected SPP DDR configuration\n");
                goto error;
            }
            update |= 1;
        }
    }
    if (pl_bypass) {
        update |= cdocmd_rewrite_pl_bypass(seq);
    }
    if (houseclean_bypass) {
        update |= cdocmd_rewrite_houseclean_bypass(seq);
    }
    if (sysmon_bypass) {
        update |= cdocmd_rewrite_sysmon_bypass(seq);
    }
    if (auto_align) {
        /* Always update to ensure alignment */
        update |= 1;
    }
    if (!update) {
        cdocmd_delete_sequence(seq);
        return 0;
    }

    if (debug_print) cdoseq_to_source(stdout, seq);
    *seqp = seq;
    return 0;

error:
    cdocmd_delete_sequence(seq);
    return 1;
}

uint32_t cdocmd_post_process_cdo(const void * data, size_t size, void ** new_data, size_t * new_size) {
    CdoSequence * seq;
    *new_data = NULL;
    *new_size = 0;
    if (cdocmd_post_process_cdo_seq(data, size, &seq)) return 1;
    if (seq == NULL) return 0;
    *new_data = cdoseq_to_binary(seq, new_size, 0);
    cdocmd_delete_sequence(seq);
    return 0;
}

static void write_cfi(CdoSequence * seq, size_t size, const void * data, uint32_t is_gsc) {
    size_t max_count = cfu_stream_keyhole_size;
    size_t offset = 0;
    uint32_t cfu_ctl = 0;
    uint32_t cfi_be = 0;

    cdocmd_add_comment(seq, "configure CFI");

    enableCFUWrite(seq);

    if (size > 0 && u32le(((uint32_t *)data)[0]) == 0xc0febabe) {
        cfi_be = 1;
    }

    if (cfi_type_valid) {
        cfi_type_valid = 0;
        switch (cfi_type) {
        case CFI_TYPE_FULL_CONFIG:
        case CFI_TYPE_TANDEM_STAGE1:
        case CFI_TYPE_TANDEM_STAGE2_ONLY:
            plpd_houseclean(seq);
            fabric_startup = 1;
            is_gsc = 0;
            break;
        case CFI_TYPE_PR_UNMASK:
            plpd_houseclean_done = 1;
            is_gsc = 1;
            break;
        case CFI_TYPE_PR_CLEAR:
            plpd_houseclean_done = 1;
            if (fabric_shutdown != 2) {
                /* Shutdown fabric. */
                fabric_shutdown = 2;
                clearGlobalSignals(seq);
            }
            is_gsc = 0;
            break;
        case CFI_TYPE_PR_CONFIG:
            plpd_houseclean_done = 1;
            fabric_shutdown = 2;
            fabric_startup = 1;
            is_gsc = 0;
            break;
        case CFI_TYPE_PR_MASK:
            fabric_shutdown = 0;
            if (fabric_startup) {
                fabric_startup = 0;
                cfi_startup(seq);
            }
            is_gsc = 1;
            break;
        case CFI_TYPE_TANDEM_STAGE2_INCR:
            if (fabric_shutdown != 2) {
                /* Shutdown fabric now since there is no explicit
                 * unmask partition.  Unmasking was already done in
                 * stage1. */
                fabric_shutdown = 2;
                clearGlobalSignals(seq);
            }
            fabric_startup = 1;
            is_gsc = 0;
            break;
        default:
            fprintf(stderr, "invalid cfu type %"PRIu32"\n", cfi_type);
            exit(1);
        }
    } else if (is_gsc) {
        if (!fabric_shutdown) {
            /* Guess this is an unmask partition, so shutdown after
             * loading this partition */
            fabric_shutdown = 1;
            /* Don't generate PLPD housecleaning. */
            plpd_houseclean_done = 1;
        } else {
            /* Guess this is a mask partition, no shutdown should
             * follow, but startup if that is not already done. */
            fabric_shutdown = 0;
            if (fabric_startup) {
                fabric_startup = 0;
                cfi_startup(seq);
            }
        }
    } else {
        plpd_houseclean(seq);
        fabric_startup = 1;
    }

    if (crc32_read < crc32_write && crc32_check[crc32_read] != 0U) {
        /* Assert CRC32 reset.  Deasserted as part of setting
         * decompression, etc. */
        cdocmd_add_comment(seq, "crc32 reset");
        cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_CTL_CRC32_RESET_MASK);
        cdocmd_add_write(seq, CFU_APB_CFU_CTL, CFU_APB_CFU_CTL_CRC32_RESET_MASK);
    }

    if (decompress != 0U) {
        cdocmd_add_comment(seq, "decompress enable");
        cfu_ctl |= CFU_APB_CFU_CTL_DECOMPRESS_MASK;
    } else {
        cdocmd_add_comment(seq, "decompress disable");
    }

    if (crc8_dis != 0U) {
        cdocmd_add_comment(seq, "crc8 disable");
        cfu_ctl |= CFU_APB_CFU_CTL_CRC8_DISABLE_MASK;
    } else {
        cdocmd_add_comment(seq, "crc8 enable");
    }

    if (gcap_clk_en != 0U) {
        cdocmd_add_comment(seq, "gcap clk enable");
        cfu_ctl |= CFU_APB_CFU_CTL_GCAP_CLK_EN_MASK;
    } else {
        cdocmd_add_comment(seq, "gcap clk disable");
    }

    if (ssi_per_slr_pr != 0U) {
        cdocmd_add_comment(seq, "ssi per slr pr enable");
        cfu_ctl |= CFU_APB_CFU_CTL_SSI_PER_SLR_PR_MASK;
    } else {
        cdocmd_add_comment(seq, "ssi per slr pr disable");
    }

    cdocmd_add_write(seq, CFU_APB_CFU_MASK,
                     CFU_APB_CFU_CTL_CRC32_RESET_MASK |
                     CFU_APB_CFU_CTL_DECOMPRESS_MASK |
                     CFU_APB_CFU_CTL_CRC8_DISABLE_MASK |
                     CFU_APB_CFU_CTL_CRC32_CHECK_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_CTL, cfu_ctl);

    if (cram_rw_set != 0U) {
        cdocmd_add_write(seq, CFU_APB_CFU_CRAM_RW, cram_rw_value);
    }

    if (is_gsc) {
        setGSCWE(seq);
    } else {
        clearGSCWE(seq);
    }

    cdocmd_add_comment(seq, "write cframe data");

    if (dma_write_keyhole_enable == 2) {
        while (offset < size) {
            cdocmd_add_block_write(seq, CFU_STREAM_ADDR + (offset & 15), 1, (char *)data + offset, cfi_be);
            /* TODO: fix byte and word swapping if needed. */
            offset += 4;
        }
    } else if (dma_write_keyhole_enable) {
        cdocmd_add_keyhole_write(seq, CFU_STREAM_ADDR, max_count/4, size/4, (char *)data, cfi_be);
    } else {
        while (offset < size) {
            size_t count = size - offset;
            if (count > max_count)  count = max_count;
            cdocmd_add_block_write(seq, CFU_STREAM_ADDR, count/4, (char *)data + offset, cfi_be);
            /* TODO: fix byte and word swapping if needed. */
            offset += count;
        }
    }

    waitForStreamDone(seq);

    if (crc32_read < crc32_write && crc32_check[crc32_read] != 0U) {
        cdocmd_add_comment(seq, "check cframe crc32");
        cdocmd_add_write(seq, CFU_APB_CFU_CRC_EXPECT, crc32_value[crc32_read]);
        cdocmd_add_write(seq, CFU_APB_CFU_MASK, CFU_APB_CFU_CTL_CRC32_CHECK_MASK);
        cdocmd_add_write(seq, CFU_APB_CFU_CTL, CFU_APB_CFU_CTL_CRC32_CHECK_MASK);
        crc32_read++;
    }

    cdocmd_add_comment(seq, "clear decompress and crc32 check");
    cdocmd_add_write(seq, CFU_APB_CFU_MASK,
                     CFU_APB_CFU_CTL_DECOMPRESS_MASK |
                     CFU_APB_CFU_CTL_CRC32_CHECK_MASK);
    cdocmd_add_write(seq, CFU_APB_CFU_CTL, 0U);

    cdocmd_add_mask_poll(seq,
                         CFU_APB_CFU_ISR,
                         CFU_APB_CFU_ISR_DECOMP_ERROR_MASK |
                         CFU_APB_CFU_ISR_BAD_CFI_PACKET_MASK |
                         CFU_APB_CFU_ISR_AXI_ALIGN_ERROR_MASK |
                         CFU_APB_CFU_ISR_CFI_ROW_ERROR_MASK |
                         CFU_APB_CFU_ISR_CRC32_ERROR_MASK |
                         CFU_APB_CFU_ISR_CRC8_ERROR_MASK,
                         0U,
                         1, 0);

    /* TODO: Check CFRAME interrupt??? */

    if (is_gsc) {
        clearGSCWE(seq);
    }

    if (fabric_shutdown == 1) {
        /* Shutdown fabric. */
        fabric_shutdown = 2;
        clearGlobalSignals(seq);
    }

    disableCFUWrite(seq);
}

uint32_t cdocmd_post_process_cfi_seq(const void * data, size_t size, uint32_t is_gsc, CdoSequence ** seqp) {
    CdoSequence * seq = cdocmd_create_sequence();
    seq->version = 0x200;
    if ((size & 15) != 0) {
        fprintf(stderr, "unaligned CFI-DATA size\n");
        goto error;
    }
    if (!pl_bypass) {
        write_cfi(seq, size, data, is_gsc);
    }
    *seqp = seq;
    if (debug_print) cdoseq_to_source(stdout, seq);
    return 0;

error:
    cdocmd_delete_sequence(seq);
    *seqp = NULL;
    return 1;
}

uint32_t cdocmd_post_process_cfi(const void * data, size_t size, uint32_t is_gsc, void ** new_data, size_t * new_size) {
    CdoSequence * seq;
    *new_data = NULL;
    *new_size = 0;
    if (cdocmd_post_process_cfi_seq(data, size, is_gsc, &seq)) return 1;
    if (seq == NULL) return 0;
    *new_data = cdoseq_to_binary(seq, new_size, 0);
    cdocmd_delete_sequence(seq);
    return 0;
}
