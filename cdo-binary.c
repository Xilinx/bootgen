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
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include "cdo-binary.h"

static uint32_t verbose;
static uint32_t auto_align;
static uint32_t add_offset;

#define u32xe(v) (be ? u32be(v) : u32le(v))
#define u32xe_lo(v) u32xe((uint32_t)(v))
#define u32xe_hi(v) u32xe((uint32_t)(v >> 32))
#define u32pair(hi, lo) (((uint64_t)(hi) << 32) | (lo))
#define HDR_IDN_WRD		(0x584C4E58U)

static uint32_t* slr_sync_points;
static uint32_t* noc_start_markers;
static uint32_t* noc_end_markers;
static uint8_t num_start_markers = 0;
static uint8_t num_end_markers = 0;
static uint8_t num_of_sync_points = 0;
static uint8_t search_sync_points = 0;

enum {
    /*General Commands */
    CMD1_MASK_POLL	 = 0x101U,
    CMD1_MASK_WRITE	 = 0x102U,
    CMD1_WRITE		 = 0x103U,
    CMD1_DELAY		 = 0x104U,
    CMD1_DMA_WRITE	 = 0x105U,
    CMD1_MASK_POLL64	 = 0x106U,
    CMD1_MASK_WRITE64	 = 0x107U,
    CMD1_WRITE64	 = 0x108U,
    CMD1_DMA_XFER	 = 0x109U,

    /* NPI Commands */
    CMD1_NPI_SEQ	 = 0x201U,
    CMD1_NPI_PRECFG	 = 0x202U,
    CMD1_NPI_WRITE	 = 0x203U,
    CMD1_NPI_SHUTDN	 = 0x204U,

    /* CFU Commands */
    CMD1_CFU_SET_CRC32	 = 0x301U,
    CMD1_CFU_DECOMPRESS	 = 0x302U,
    CMD1_CFU_CRAM_RW	 = 0x303U,
    CMD1_CFU_SEU_GO	 = 0x304U,
    CMD1_CFU_CRC8_DIS	 = 0x305U,
    CMD1_CFU_SSI_PER_SLR_PR = 0x306U,
    CMD1_CFU_GSR_GSC	 = 0x307U,
    CMD1_CFU_GCAP_CLK_EN = 0x308U,
    CMD1_CFU_CFRAME_READ = 0x309U,

    CMD1_END = 0xffffffffU,
};

enum {
    /*General Commands */
    CMD2_END_MARK	 = 0x100U,
    CMD2_MASK_POLL	 = 0x101U,
    CMD2_MASK_WRITE	 = 0x102U,
    CMD2_WRITE		 = 0x103U,
    CMD2_DELAY		 = 0x104U,
    CMD2_DMA_WRITE	 = 0x105U,
    CMD2_MASK_POLL64	 = 0x106U,
    CMD2_MASK_WRITE64	 = 0x107U,
    CMD2_WRITE64	 = 0x108U,
    CMD2_DMA_XFER	 = 0x109U,
    CMD2_INIT_SEQ	 = 0x10aU,
    CMD2_CFRAME_READ	 = 0x10bU,
    CMD2_SET		 = 0x10cU,
    CMD2_DMA_WRITE_KEYHOLE = 0x10dU,
    CMD2_SSIT_SYNC_MASTER = 0x10eU,
    CMD2_SSIT_SYNC_SLAVES = 0x10fU,
    CMD2_SSIT_WAIT_SLAVES = 0x110U,
    CMD2_NOP		 = 0x111U,
    CMD2_GET_DEVICE_ID	 = 0x112U,
    CMD2_EVENT_LOGGING	 = 0x113U,
    CMD2_SET_BOARD	 = 0x114U,
    CMD2_GET_BOARD	 = 0x115U,
    CMD2_SET_PLM_WDT	 = 0x116U,
    CMD2_LOG_STRING	 = 0x117U,
    CMD2_LOG_ADDRESS	 = 0x118U,
    CMD2_MARKER		 = 0x119U,
    CMD2_PROC		 = 0x11aU,
    CMD2_BLOCK_BEGIN	 = 0x11bU,
    CMD2_BLOCK_END	 = 0x11cU,
    CMD2_BREAK		 = 0x11dU,
    CMD2_OT_CHECK	 = 0x11eU,
    CMD2_PSM_SEQUENCE	 = 0x11fU,
    CMD2_PLM_UPDATE	 = 0x120U,
    CMD2_SCATTER_WRITE	 = 0x121U,
    CMD2_SCATTER_WRITE2	 = 0x122U,
    CMD2_TAMPER_TRIGGER  = 0x123U,
    CMD2_SET_IPI_ACCESS  = 0x125U,

    /* PM Commands */
    CMD2_PM_GET_API_VERSION	= 0x201U,
    CMD2_PM_GET_DEVICE_STATUS	= 0x203U,
    CMD2_PM_GET_OP_CHARACTERISTIC = 0x204U,
    CMD2_PM_REGISTER_NOTIFIER	= 0x205U,
    CMD2_PM_REQUEST_SUSPEND	= 0x206U,
    CMD2_PM_SELF_SUSPEND	= 0x207U,
    CMD2_PM_FORCE_POWERDOWN	= 0x208U,
    CMD2_PM_ABORT_SUSPEND	= 0x209U,
    CMD2_PM_REQUEST_WAKEUP	= 0x20aU,
    CMD2_PM_SET_WAKEUP_SOURCE	= 0x20bU,
    CMD2_PM_SYSTEM_SHUTDOWN	= 0x20cU,
    CMD2_PM_REQUEST_DEVICE	= 0x20dU,
    CMD2_PM_RELEASE_DEVICE	= 0x20eU,
    CMD2_PM_SET_REQUIREMENT	= 0x20fU,
    CMD2_PM_SET_MAX_LATENCY	= 0x210U,
    CMD2_PM_RESET_ASSERT	= 0x211U,
    CMD2_PM_RESET_GET_STATUS	= 0x212U,
    CMD2_PM_PINCTRL_REQUEST	= 0x21cU,
    CMD2_PM_PINCTRL_RELEASE	= 0x21dU,
    CMD2_PM_PINCTRL_GET_FUNCTION = 0x21eU,
    CMD2_PM_PINCTRL_SET_FUNCTION = 0x21fU,
    CMD2_PM_PINCTRL_CONFIG_PARAM_GET = 0x220U,
    CMD2_PM_PINCTRL_CONFIG_PARAM_SET = 0x221U,
    CMD2_PM_IOCTL		= 0x222U,
    CMD2_PM_QUERY_DATA		= 0x223U,
    CMD2_PM_CLOCK_ENABLE	= 0x224U,
    CMD2_PM_CLOCK_DISABLE	= 0x225U,
    CMD2_PM_CLOCK_GETSTATE	= 0x226U,
    CMD2_PM_CLOCK_SETDIVIDER	= 0x227U,
    CMD2_PM_CLOCK_GETDIVIDER	= 0x228U,
    CMD2_PM_CLOCK_SETRATE	= 0x229U,
    CMD2_PM_CLOCK_GETRATE	= 0x22aU,
    CMD2_PM_CLOCK_SETPARENT	= 0x22bU,
    CMD2_PM_CLOCK_GETPARENT	= 0x22cU,
    CMD2_PM_PLL_SET_PARAMETER	= 0x230U,
    CMD2_PM_PLL_GET_PARAMETER	= 0x231U,
    CMD2_PM_PLL_SET_MODE	= 0x232U,
    CMD2_PM_PLL_GET_MODE	= 0x233U,
    CMD2_PM_ADD_SUBSYSTEM	= 0x236U,
    CMD2_PM_DESTROY_SUBSYSTEM	= 0x237U,
    CMD2_PM_DESCRIBE_NODES	= 0x238U,
    CMD2_PM_ADD_NODE		= 0x239U,
    CMD2_PM_ADD_NODE_PARENT	= 0x23aU,
    CMD2_PM_ADD_NODE_NAME	= 0x23bU,
    CMD2_PM_ADD_REQUIREMENT	= 0x23cU,
    CMD2_PM_SET_CURRENT_SUBSYSTEM = 0x23dU,
    CMD2_PM_INIT_NODE		= 0x23eU,
    CMD2_PM_FEATURE_CHECK	= 0x23fU,
    CMD2_PM_ISO_CONTROL		= 0x240U,
    CMD2_PM_ACTIVATE_SUBSYSTEM	= 0x241U,
    CMD2_PM_SET_NODE_ACCESS	= 0x242U,
    CMD2_PM_BISR = 0x243,
    CMD2_PM_APPLY_TRIM = 0x244,
    CMD2_PM_NOC_CLOCK_ENABLE	= 0x245,
    CMD2_PM_IF_NOC_CLOCK_ENABLE	= 0x246,
    CMD2_PM_FORCE_HC =0x247,

    /* NPI Commands */
    /* Version 1.50 and later, but not version 2.00 */
    CMD2_NPI_SEQ	 = 0x301U,
    CMD2_NPI_PRECFG	 = 0x302U,
    CMD2_NPI_WRITE	 = 0x303U,
    CMD2_NPI_SHUTDN	 = 0x304U,

    /* SEM Commands */
    /* Version 2.00 and later */
    CMD2_SEM_NPI_TABLE	 = 0x300U,

    /* CFU Commands */
    /* Version 1.50 and later, but not version 2.00 */
    CMD2_CFU_SET_CRC32	 = 0x401U,
    CMD2_CFU_DECOMPRESS	 = 0x402U,
    CMD2_CFU_CRAM_RW	 = 0x403U,
    CMD2_CFU_SEU_GO	 = 0x404U,
    CMD2_CFU_CRC8_DIS	 = 0x405U,
    CMD2_CFU_SSI_PER_SLR_PR = 0x406U,
    CMD2_CFU_GSR_GSC	 = 0x407U,
    CMD2_CFU_GCAP_CLK_EN = 0x408U,
    CMD2_CFU_CFI_TYPE	 = 0x409U,

    /* Loader Commands */
    CMD2_LDR_SET_IMAGE_INFO	 = 0x704U,
    CMD2_LDR_CFRAME_CLEAR_CHECK	 = 0x70cU,

    /* EM Commands */
    CMD2_EM_SET_ACTION		 = 0x801U,

    CMD2_END             = 0xffffffffU
};

void cdobinary_set_verbose(uint32_t level) {
    verbose = level;
}

void cdobinary_set_auto_align(uint32_t enable) {
    auto_align = enable;
}

void cdobinary_set_add_offset(uint32_t enable) {
    add_offset = enable;
}

static void byte_swap_buffer(uint32_t * p, uint32_t count, uint32_t be) {
    if (is_be_host() != be) {
        uint32_t i;
        for (i = 0; i < count; i++) {
            p[i] = u32swap(p[i]);
        }
    }
}

static uint32_t decode_v1(CdoSequence * seq, uint32_t * p, uint32_t l, uint32_t be) {
    uint32_t i = u32xe(p[0]) + 1;
    uint32_t sections = u32xe(p[3]);
    uint32_t secno = 0;
    while (secno < sections && i + 3 <= l) {
        uint32_t secid = u32xe(p[i++]);
        uint32_t seclen = u32xe(p[i++]);
        uint32_t secend = l;
        uint32_t seccmds = u32xe(p[i++]);
        uint32_t cmdno = 0;
        if (verbose) {
            fprintf(stderr, "info: decoding section %#"PRIx32" len %"PRIu32" cmds %"PRIu32" at %"PRIu32"\n", secid, seclen, seccmds, i - 3);
        }
        if (add_offset) {
            cdocmd_add_comment(seq, "offset: %#"PRIx64, (uint64_t)i * 4);
        }
        cdocmd_add_section(seq, secid);
        while (i < l && cmdno < seccmds) {
            uint32_t id = u32xe(p[i++]);
            uint32_t args = 0;
            if (verbose) {
                fprintf(stderr, "info: decoding command %#"PRIx32" at %"PRIu32"\n", id, i - 1);
            }
            if (add_offset) {
                cdocmd_add_comment(seq, "offset: %#"PRIx64, (uint64_t)i * 4);
            }
            switch (id) {
            case CMD1_MASK_POLL:
                args = 4;
                if (secend - i < args) break;
                cdocmd_add_mask_poll(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), u32xe(p[i+3])*1000, 0);
                break;
            case CMD1_MASK_WRITE:
                args = 3;
                if (secend - i < args) break;
                cdocmd_add_mask_write(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]));
                break;
            case CMD1_WRITE:
                args = 2;
                if (secend - i < args) break;
                cdocmd_add_write(seq, u32xe(p[i+0]), u32xe(p[i+1]));
                break;
            case CMD1_DELAY:
                args = 1;
                if (secend - i < args) break;
                cdocmd_add_delay(seq, u32xe(p[i+0])*1000);
                break;
            case CMD1_DMA_WRITE:
                args = 3;
                if (secend - i < args) break;
                args += u32xe(p[i+1]) + u32xe(p[i+2]);
                if (secend - i < args) break;
                cdocmd_add_block_write(seq, u32xe(p[i+0]), u32xe(p[i+1]), &p[i+3+u32xe(p[i+2])], be);
                break;
            case CMD1_MASK_POLL64:
                args = 5;
                if (secend - i < args) break;
                cdocmd_add_mask_poll(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32xe(p[i+2]), u32xe(p[i+3]), u32xe(p[i+4])*1000, 0);
                break;
            case CMD1_MASK_WRITE64:
                args = 4;
                if (secend - i < args) break;
                cdocmd_add_mask_write(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32xe(p[i+2]), u32xe(p[i+3]));
                break;
            case CMD1_WRITE64:
                args = 3;
                if (secend - i < args) break;
                cdocmd_add_write(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32xe(p[i+2]));
                break;
            case CMD1_DMA_XFER:
                args = 6;
                if (secend - i < args) break;
                if (u32xe(p[i+0]) == 0 && u32xe(p[i+1]) == 0) {
                    args += u32xe(p[i+4]);
                    if (secend - i < args) break;
                    cdocmd_add_dma_xfer(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32pair(u32xe(p[i+2]), u32xe(p[i+3])), u32xe(p[i+4]), u32xe(p[i+5]), &p[i+6], be);
                } else {
                    cdocmd_add_dma_xfer(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32pair(u32xe(p[i+2]), u32xe(p[i+3])), u32xe(p[i+4]), u32xe(p[i+5]), NULL, be);
                }
                break;
            case CMD1_NPI_SEQ:
                args = 2;
                if (secend - i < args) break;
                cdocmd_add_npi_seq(seq, u32xe(p[i+0]), u32xe(p[i+1]));
                break;
            case CMD1_NPI_PRECFG:
                args = 2;
                if (secend - i < args) break;
                cdocmd_add_npi_precfg(seq, u32xe(p[i+0]), u32xe(p[i+1]));
                break;
            case CMD1_NPI_WRITE:
                args = 3;
                if (secend - i < args) break;
                args += u32xe(p[i+2]);
                if (secend - i < args) break;
                cdocmd_add_npi_write(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), &p[i+3], be);
                break;
            case CMD1_NPI_SHUTDN:
                args = 2;
                if (secend - i < args) break;
                cdocmd_add_npi_shutdown(seq, u32xe(p[i+0]), u32xe(p[i+1]));
                break;
            case CMD1_CFU_SET_CRC32:
                args = 1;
                if (secend - i < args) break;
                if (u32xe(p[i+0]) == 0) {
                    cdocmd_add_cfu_set_crc32(seq, 0, 0);
                    break;
                }
                args = 2;
                if (secend - i < args) break;
                cdocmd_add_cfu_set_crc32(seq, u32xe(p[i+0]), u32xe(p[i+1]));
                break;
            case CMD1_CFU_DECOMPRESS:
                args = 1;
                if (secend - i < args) break;
                cdocmd_add_cfu_decompress(seq, u32xe(p[i+0]));
                break;
            case CMD1_CFU_CRAM_RW:
                args = 1;
                if (secend - i < args) break;
                cdocmd_add_cfu_cram_rw(seq, u32xe(p[i+0]));
                break;
            case CMD1_CFU_SEU_GO:
                args = 1;
                if (secend - i < args) break;
                cdocmd_add_cfu_seu_go(seq, u32xe(p[i+0]));
                break;
            case CMD1_CFU_CRC8_DIS:
                args = 1;
                if (secend - i < args) break;
                cdocmd_add_cfu_crc8_dis(seq, u32xe(p[i+0]));
                break;
            case CMD1_CFU_SSI_PER_SLR_PR:
                args = 1;
                if (secend - i < args) break;
                cdocmd_add_cfu_ssi_per_slr_pr(seq, u32xe(p[i+0]));
                break;
            case CMD1_CFU_GSR_GSC:
                args = 1;
                if (secend - i < args) break;
                cdocmd_add_cfu_gsr_gsc(seq, u32xe(p[i+0]));
                break;
            case CMD1_CFU_GCAP_CLK_EN:
                args = 1;
                if (secend - i < args) break;
                cdocmd_add_cfu_gcap_clk_en(seq, u32xe(p[i+0]));
                break;
            case CMD1_CFU_CFRAME_READ:
                args = 20;
                if (secend - i < args) break;
                cdocmd_add_cframe_read(seq, u32xe(p[i+0]), u32pair(u32xe(p[i+1]), u32xe(p[i+2])), u32xe(p[i+3]), 16, &p[i+4], be);
                break;
            case CMD1_END:
                goto end_of_section;
            default:
                fprintf(stderr, "unknown command %"PRIx32"\n", id);
                return 1;
            }
            if (secend - i < args) {
                fprintf(stderr, "incomplete command payload %"PRIu32", expected %"PRIu32"\n", secend - i, args);
                return 1;
            }
            i += args;
            cmdno++;
        }
        if (cmdno < seccmds) {
            fprintf(stderr, "incomplete command count %"PRIu32", expected %"PRIu32"\n", cmdno, seccmds);
            return 1;
        }
    end_of_section:
        secno++;
    }
    if (secno < sections) {
        fprintf(stderr, "incomplete section count %"PRIu32", expected %"PRIu32"\n", secno, sections);
        return 1;
    }
    if (i < l) {
        fprintf(stderr, "warning: mismatching CDO word count %"PRIu32", expected %"PRIu32"\n", i, l);
    }
    return 0;
}

static uint32_t decode_v2_cmd(CdoSequence * seq, uint32_t * p, uint32_t * ip, uint32_t l, uint32_t be) {
    uint32_t i = *ip;
    uint32_t cmdpos = 0;
    uint32_t hdr;
    while (i < l) {
        uint32_t args;
        cmdpos = i;
        if (add_offset) {
            cdocmd_add_comment(seq, "offset: %#"PRIx64, (uint64_t)cmdpos * 4);
        }
        hdr = u32xe(p[i++]);
        if (verbose) {
            fprintf(stderr, "info: decoding command %#"PRIx32" at %"PRIu32"\n", hdr, cmdpos);
        }
        args = (hdr >> 16) & 255;
        if (args == 255) {
            if (hdr == CMD2_END) goto found_end_marker;
            if (i >= l) {
                fprintf(stderr, "incomplete CDO command %#"PRIx32" at %"PRIu32"\n", hdr, cmdpos);
                goto error;
            }
            args = u32xe(p[i++]);
        }
        if (l - i < args) {
            fprintf(stderr, "incomplete CDO command %#"PRIx32" at %"PRIu32"\n", hdr, cmdpos);
            goto error;
        }
        switch (hdr & 0xffff) {
        case CMD2_END_MARK:
            goto found_end_marker;
        case CMD2_MASK_POLL:
            if (args == 4) {
                cdocmd_add_mask_poll(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), u32xe(p[i+3]), 0);
            } else if (args == 5) {
                cdocmd_add_mask_poll(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), u32xe(p[i+3]), u32xe(p[i+4]));
            } else if (args == 6) {
                cdocmd_add_mask_poll_w_err(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), u32xe(p[i+3]), u32xe(p[i+4]), u32xe(p[i+5]));
            } else {
                goto unexpected;
            }
            break;
        case CMD2_MASK_WRITE:
            if (args != 3) goto unexpected;
            cdocmd_add_mask_write(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]));
            break;
        case CMD2_WRITE:
            if (args != 2) goto unexpected;
            cdocmd_add_write(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_DELAY:
            if (args != 1) goto unexpected;
            cdocmd_add_delay(seq, u32xe(p[i+0]));
            break;
        case CMD2_DMA_WRITE:
            if (args < 2) goto unexpected;
            cdocmd_add_block_write(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), args - 2, &p[i+2], be);
            break;
        case CMD2_DMA_WRITE_KEYHOLE:
            if (args < 3) goto unexpected;
            cdocmd_add_keyhole_write(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32xe(p[i+2]), args - 3, &p[i+3], be);
            break;
        case CMD2_SSIT_SYNC_MASTER:
            if (args != 0) goto unexpected;
            cdocmd_add_ssit_sync_master(seq);
            break;
        case CMD2_SSIT_SYNC_SLAVES:
            if (args != 2) goto unexpected;
            cdocmd_add_ssit_sync_slaves(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_SSIT_WAIT_SLAVES:
            if (args != 2) goto unexpected;
            cdocmd_add_ssit_wait_slaves(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_MASK_POLL64:
            if (args == 5) {
                cdocmd_add_mask_poll(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32xe(p[i+2]), u32xe(p[i+3]), u32xe(p[i+4]), 0);
            } else if (args == 6) {
                cdocmd_add_mask_poll(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32xe(p[i+2]), u32xe(p[i+3]), u32xe(p[i+4]), u32xe(p[i+5]));
            } else if (args == 7) {
                cdocmd_add_mask_poll_w_err(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32xe(p[i+2]), u32xe(p[i+3]), u32xe(p[i+4]), u32xe(p[i+5]), u32xe(p[i+6]));
            } else {
                goto unexpected;
            }
            break;
        case CMD2_MASK_WRITE64:
            if (args != 4) goto unexpected;
            cdocmd_add_mask_write(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32xe(p[i+2]), u32xe(p[i+3]));
            break;
        case CMD2_WRITE64:
            if (args != 3) goto unexpected;
            cdocmd_add_write(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32xe(p[i+2]));
            break;
        case CMD2_DMA_XFER:
            if (args != 6) goto unexpected;
            cdocmd_add_dma_xfer(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32pair(u32xe(p[i+2]), u32xe(p[i+3])), u32xe(p[i+4]), u32xe(p[i+5]), NULL, be);
            break;
        case CMD2_CFRAME_READ:
            if (args < 4) goto unexpected;
            cdocmd_add_cframe_read(seq, u32xe(p[i+0]), u32pair(u32xe(p[i+1]), u32xe(p[i+2])), u32xe(p[i+3]), args - 4, &p[i+4], be);
            break;
        case CMD2_SET:
            if (args != 4) goto unexpected;
            cdocmd_add_block_set(seq, u32pair(u32xe(p[i+0]), u32xe(p[i+1])), u32xe(p[i+2]), u32xe(p[i+3]));
            break;
        case CMD2_NOP:
            cdocmd_add_nop(seq, args, &p[i+0], be);
            break;
        case CMD2_EVENT_LOGGING:
            if (args < 1) goto unexpected;
            cdocmd_add_event_logging(seq, u32xe(p[i+0]), args - 1, &p[i+1], be);
            break;
        case CMD2_SET_BOARD: {
            uint32_t * tmpbuf;
            tmpbuf = malloc((args + 1) * sizeof *tmpbuf);
            memcpy(tmpbuf, &p[i+0], args * sizeof *tmpbuf);
            tmpbuf[args] = 0;
            byte_swap_buffer(tmpbuf, args, be);
            cdocmd_add_set_board(seq, (char *)tmpbuf);
            free(tmpbuf);
            break;
        }
        case CMD2_SET_PLM_WDT:
            if (args != 2) goto unexpected;
            cdocmd_add_set_plm_wdt(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_LOG_STRING: {
            uint32_t * tmpbuf;
            tmpbuf = malloc((args + 1) * sizeof *tmpbuf);
            memcpy(tmpbuf, &p[i+0], args * sizeof *tmpbuf);
            tmpbuf[args] = 0;
            byte_swap_buffer(tmpbuf, args, be);
            cdocmd_add_log_string(seq, (char *)tmpbuf);
            free(tmpbuf);
            break;
        }
        case CMD2_LOG_ADDRESS:
            if (args == 1) {
                cdocmd_add_log_address(seq, u32xe(p[i+0]));
            } else if (args == 2) {
                cdocmd_add_log_address(seq, u32pair(u32xe(p[i+1]), u32xe(p[i+0])));
            } else {
                goto unexpected;
            }
            break;
        case CMD2_MARKER: {
            uint32_t * tmpbuf;
            if (args < 1) goto unexpected;
            tmpbuf = malloc(args * sizeof *tmpbuf);
            memcpy(tmpbuf, &p[i+1], (args - 1) * sizeof *tmpbuf);
            tmpbuf[args - 1] = 0;
            byte_swap_buffer(tmpbuf, args - 1, be);
            cdocmd_add_marker(seq, u32xe(p[i+0]), (char *)tmpbuf);
            free(tmpbuf);
            break;
        }
        case CMD2_PROC: {
            uint32_t i2 = i + 1;
            if (args < 1) goto unexpected;
            cdocmd_add_proc(seq, u32xe(p[i+0]));
            if (decode_v2_cmd(seq, p, &i2, i2 + args - 1, be)) {
                goto error;
            }
            cdocmd_add_end(seq);
            break;
        }
        case CMD2_BLOCK_BEGIN: {
            uint32_t * tmpbuf;
            if (args < 1) goto unexpected;
            tmpbuf = malloc(args * sizeof *tmpbuf);
            memcpy(tmpbuf, &p[i+1], (args - 1) * sizeof *tmpbuf);
            tmpbuf[args - 1] = 0;
            byte_swap_buffer(tmpbuf, args - 1, be);
            cdocmd_add_begin(seq, (char *)tmpbuf);
            free(tmpbuf);
            break;
        }
        case CMD2_BLOCK_END: {
            if (args != 0) goto unexpected;
            cdocmd_add_end(seq);
            break;
        }
        case CMD2_BREAK: {
            if (args > 1) goto unexpected;
            cdocmd_add_break(seq, args > 0 ? u32xe(p[i+0]) : 1);
            break;
        }
        case CMD2_OT_CHECK:
            if (args != 1) goto unexpected;
            cdocmd_add_ot_check(seq, u32xe(p[i+0]));
            break;
        case CMD2_PSM_SEQUENCE: {
            uint32_t i2 = i;
            cdocmd_add_psm_sequence(seq);
            if (decode_v2_cmd(seq, p, &i2, i2 + args, be)) {
                goto error;
            }
            cdocmd_add_end(seq);
            break;
        }
        case CMD2_SCATTER_WRITE:
            if (args < 2) goto unexpected;
            cdocmd_add_scatter_write(seq, u32xe(p[i+0]), args - 1, &p[i+1], be);
            break;
        case CMD2_SCATTER_WRITE2:
            if (args < 3) goto unexpected;
            cdocmd_add_scatter_write2(seq, u32xe(p[i+0]), u32xe(p[i+1]), args - 2, &p[i+2], be);
            break;

        case CMD2_TAMPER_TRIGGER:
            if (args != 1) goto unexpected;
            cdocmd_add_tamper_trigger(seq, u32xe(p[i+0]));
            break;

        case CMD2_SET_IPI_ACCESS:
            if (args != 2) goto unexpected;
            cdocmd_add_set_ipi_access(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;

        case CMD2_NPI_SEQ:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args != 2) goto unexpected;
            cdocmd_add_npi_seq(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_NPI_PRECFG:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args != 2) goto unexpected;
            cdocmd_add_npi_precfg(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_NPI_WRITE:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args < 2) goto unexpected;
            cdocmd_add_npi_write(seq, u32xe(p[i+0]), u32xe(p[i+1]), args - 2, &p[i+2], be);
            break;
        case CMD2_NPI_SHUTDN:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args != 2) goto unexpected;
            cdocmd_add_npi_shutdown(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_GET_API_VERSION:
            if (args != 0) goto unexpected;
            cdocmd_add_pm_get_api_version(seq);
            break;
        case CMD2_PM_GET_DEVICE_STATUS:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_get_device_status(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_REGISTER_NOTIFIER:
            if (args != 4) goto unexpected;
            cdocmd_add_pm_register_notifier(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), u32xe(p[i+3]));
            break;
        case CMD2_PM_REQUEST_SUSPEND:
            if (args != 4) goto unexpected;
            cdocmd_add_pm_request_suspend(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), u32xe(p[i+3]));
            break;
        case CMD2_PM_SELF_SUSPEND:
            if (args != 5) goto unexpected;
            cdocmd_add_pm_self_suspend(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), u32pair(u32xe(p[i+4]), u32xe(p[i+3])));
            break;
        case CMD2_PM_FORCE_POWERDOWN:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_force_powerdown(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_ABORT_SUSPEND:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_abort_suspend(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_REQUEST_WAKEUP:
            if (args != 4) goto unexpected;
            cdocmd_add_pm_request_wakeup(seq, u32xe(p[i+0]), u32pair(u32xe(p[i+2]), u32xe(p[i+1])), u32xe(p[i+3]));
            break;
        case CMD2_PM_SET_WAKEUP_SOURCE:
            if (args != 3) goto unexpected;
            cdocmd_add_pm_set_wakeup_source(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]));
            break;
        case CMD2_PM_SYSTEM_SHUTDOWN:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_system_shutdown(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_REQUEST_DEVICE:
            if (args != 4) goto unexpected;
            cdocmd_add_pm_request_device(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), u32xe(p[i+3]));
            break;
        case CMD2_PM_RELEASE_DEVICE:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_release_device(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_SET_REQUIREMENT:
            if (args != 4) goto unexpected;
            cdocmd_add_pm_set_requirement(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), u32xe(p[i+3]));
            break;
        case CMD2_PM_SET_MAX_LATENCY:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_set_max_latency(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_RESET_ASSERT:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_reset_assert(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_RESET_GET_STATUS:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_reset_get_status(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_PINCTRL_REQUEST:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_pinctrl_request(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_PINCTRL_RELEASE:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_pinctrl_release(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_PINCTRL_GET_FUNCTION:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_pinctrl_get_function(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_PINCTRL_SET_FUNCTION:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_pinctrl_set_function(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_PINCTRL_CONFIG_PARAM_GET:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_pinctrl_config_param_get(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_PINCTRL_CONFIG_PARAM_SET:
            if (args != 3) goto unexpected;
            cdocmd_add_pm_pinctrl_config_param_set(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]));
            break;
        case CMD2_PM_IOCTL:
            if (args < 2) goto unexpected;
            cdocmd_add_pm_ioctl(seq, u32xe(p[i+0]), u32xe(p[i+1]), &p[i+2], args-2, be);
            break;
        case CMD2_PM_QUERY_DATA:
            if (args < 1) goto unexpected;
            cdocmd_add_pm_query_data(seq, u32xe(p[i+0]), &p[i+1], args-1, be);
            break;
        case CMD2_PM_CLOCK_ENABLE:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_clock_enable(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_CLOCK_DISABLE:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_clock_disable(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_CLOCK_GETSTATE:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_clock_getstate(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_CLOCK_SETDIVIDER:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_clock_setdivider(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_CLOCK_GETDIVIDER:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_clock_getdivider(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_CLOCK_SETRATE:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_clock_setrate(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_CLOCK_GETRATE:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_clock_getrate(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_CLOCK_SETPARENT:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_clock_setparent(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_CLOCK_GETPARENT:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_clock_getparent(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_PLL_SET_PARAMETER:
            if (args != 3) goto unexpected;
            cdocmd_add_pm_pll_set_parameter(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]));
            break;
        case CMD2_PM_PLL_GET_PARAMETER:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_pll_get_parameter(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_PLL_SET_MODE:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_pll_set_mode(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_PLL_GET_MODE:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_pll_get_mode(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_ADD_SUBSYSTEM:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_add_subsystem(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_DESTROY_SUBSYSTEM:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_destroy_subsystem(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_DESCRIBE_NODES:
            cdocmd_add_pm_describe_nodes(seq, &p[i+0], args, be);
            break;
        case CMD2_PM_ADD_NODE:
            if (args < 1) goto unexpected;
            cdocmd_add_pm_add_node(seq, u32xe(p[i+0]), args - 1, &p[i+1], be);
            break;
        case CMD2_PM_ADD_NODE_PARENT:
            if (args < 2) goto unexpected;
            cdocmd_add_pm_add_node_parent(seq, u32xe(p[i+0]), args - 1, &p[i+1], be);
            break;
        case CMD2_PM_ADD_NODE_NAME: {
            uint32_t * tmpbuf;
            if (args < 2) goto unexpected;
            tmpbuf = malloc(args * sizeof *tmpbuf);
            memcpy(tmpbuf, &p[i+1], (args - 1) * sizeof *tmpbuf);
            tmpbuf[args - 1] = 0;
            byte_swap_buffer(tmpbuf, args - 1, be);
            cdocmd_add_pm_add_node_name(seq, u32xe(p[i+0]), (char *)tmpbuf);
            free(tmpbuf);
            break;
        }
        case CMD2_PM_ADD_REQUIREMENT:
            if (args < 3) goto unexpected;
            cdocmd_add_pm_add_requirement(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), args - 3, &p[i+3], be);
            break;
        case CMD2_PM_SET_CURRENT_SUBSYSTEM:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_set_current_subsystem(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_INIT_NODE:
            if (args < 2) goto unexpected;
            cdocmd_add_pm_init_node(seq, u32xe(p[i+0]), u32xe(p[i+1]), args - 2, &p[i+2], be);
            break;
        case CMD2_PM_FEATURE_CHECK:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_feature_check(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_ISO_CONTROL:
            if (args != 2) goto unexpected;
            cdocmd_add_pm_iso_control(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_PM_ACTIVATE_SUBSYSTEM:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_activate_subsystem(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_SET_NODE_ACCESS:
            if (args < 1) goto unexpected;
            cdocmd_add_pm_set_node_access(seq, u32xe(p[i+0]), args - 1, &p[i+1], be);
            break;
        case CMD2_PM_BISR:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_bisr(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_APPLY_TRIM:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_apply_trim(seq, u32xe(p[i+0]));
            break;
        case CMD2_PM_NOC_CLOCK_ENABLE:
            if (args < 1) goto unexpected;
            cdocmd_add_pm_noc_clock_enable(seq, u32xe(p[i+0]), args - 1, &p[i+1], be);
            break;
        case CMD2_PM_IF_NOC_CLOCK_ENABLE:
            if (args != 2 && args != 3) goto unexpected;
            cdocmd_add_pm_if_noc_clock_enable(seq, u32xe(p[i+0]), u32xe(p[i+1]),
                                              args == 3 ? u32xe(p[i+2]) : 1);
            break;
        case CMD2_PM_FORCE_HC:
            if (args != 1) goto unexpected;
            cdocmd_add_pm_force_hc(seq, u32xe(p[i+0]));
            break;
        case CMD2_CFU_SET_CRC32:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args < 1) goto unexpected;
            if (u32xe(p[i+0]) == 0) {
                cdocmd_add_cfu_set_crc32(seq, 0, 0);
                break;
            }
            if (args < 2) goto unexpected;
            cdocmd_add_cfu_set_crc32(seq, u32xe(p[i+0]), u32xe(p[i+1]));
            break;
        case CMD2_CFU_DECOMPRESS:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args < 1) goto unexpected;
            cdocmd_add_cfu_decompress(seq, u32xe(p[i+0]));
            break;
        case CMD2_CFU_CRAM_RW:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args < 1) goto unexpected;
            cdocmd_add_cfu_cram_rw(seq, u32xe(p[i+0]));
            break;
        case CMD2_CFU_SEU_GO:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args < 1) goto unexpected;
            cdocmd_add_cfu_seu_go(seq, u32xe(p[i+0]));
            break;
        case CMD2_CFU_CRC8_DIS:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args < 1) goto unexpected;
            cdocmd_add_cfu_crc8_dis(seq, u32xe(p[i+0]));
            break;
        case CMD2_CFU_SSI_PER_SLR_PR:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args < 1) goto unexpected;
            cdocmd_add_cfu_ssi_per_slr_pr(seq, u32xe(p[i+0]));
            break;
        case CMD2_CFU_GSR_GSC:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args < 1) goto unexpected;
            cdocmd_add_cfu_gsr_gsc(seq, u32xe(p[i+0]));
            break;
        case CMD2_CFU_GCAP_CLK_EN:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args < 1) goto unexpected;
            cdocmd_add_cfu_gcap_clk_en(seq, u32xe(p[i+0]));
            break;
        case CMD2_CFU_CFI_TYPE:
            if (seq->version >= CDO_VERSION_2_00) goto unexpected;
            if (args < 1) goto unexpected;
            cdocmd_add_cfu_cfi_type(seq, u32xe(p[i+0]));
            break;
        case CMD2_EM_SET_ACTION:
            if (args != 3) goto unexpected;
            cdocmd_add_em_set_action(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]));
            break;
        case CMD2_LDR_SET_IMAGE_INFO:
            if (args != 4) goto unexpected;
            cdocmd_add_ldr_set_image_info(seq, u32xe(p[i+0]), u32xe(p[i+1]), u32xe(p[i+2]), u32xe(p[i+3]));
            break;
        case CMD2_LDR_CFRAME_CLEAR_CHECK:
            if (args != 1) goto unexpected;
            cdocmd_add_ldr_cframe_clear_check(seq, u32xe(p[i+0]));
            break;
        case CMD2_SEM_NPI_TABLE:
            if (args < 2) goto unexpected;
            cdocmd_add_sem_npi_table(seq, u32xe(p[i+0]), u32xe(p[i+1]), args - 2, &p[i+2], be);
            break;

        default:
            cdocmd_add_generic_command(seq, hdr & 0xffff, p + i, args, be);
            break;
        }
        i += args;
    }
found_end_marker:
    *ip = i;
    return 0;

unexpected:
    fprintf(stderr, "invalid payload size command %#"PRIx32" at %"PRIu32"\n", hdr, cmdpos);
error:
    *ip = i;
    return 1;
}

int check_cdo_commands(void* data, uint32_t l, uint32_t * xplm_data, uint32_t xplm_length) {
    uint32_t be = 0;
    uint32_t * p = (uint32_t *)data;
    uint32_t hdrlen = u32xe(p[0]);
    uint32_t len = u32xe(p[3]);
    uint32_t total_len = hdrlen + 1 + len;
    uint32_t i = hdrlen + 1;
    uint32_t cmdpos = 0;
    uint32_t hdr;
    uint8_t cmd_id;
    uint8_t module_id;
    size_t index;

    if (l < total_len) {
        fprintf(stderr, "warning: incomplete CDO buffer %"PRIu32", expected %"PRIu32"\n", l, total_len);
        total_len = l;
    } else if (l > total_len) {
        l = total_len;
    }
    while (i < l) {
        uint32_t args;
        int cmd_found;
        cmdpos = i;
        hdr = u32xe(p[i++]);
        if (verbose) {
            fprintf(stderr, "info: decoding command %#"PRIx32" at %"PRIu32"\n", hdr, cmdpos);
        }
        args = (hdr >> 16) & 255;
        if (args == 255) {
            if (hdr == CMD2_END) goto found_end_marker;
            if (i >= l) {
                fprintf(stderr, "incomplete CDO command %#"PRIx32" at %"PRIu32"\n", hdr, cmdpos);
            }
            args = u32xe(p[i++]);
        }
        if (l - i < args) {
            fprintf(stderr, "incomplete CDO command %#"PRIx32" at %"PRIu32"\n", hdr, cmdpos);
        }
        cmd_id = hdr & 0xff;
        module_id = (hdr & 0xff00) >> 0x8;
        // TODO: only PLM commands are supported for now
        if (module_id == 0x1 && xplm_length != 0){
           cmd_found = 0;
           size_t cmdInfo_size = sizeof(XPlmi_CmdInfo);
           for (index = 0; index < (xplm_length/ cmdInfo_size); index++) {
               uint8_t* cmdInfo = (uint8_t*)malloc(cmdInfo_size);
               memset(cmdInfo, 0, cmdInfo_size);
               memcpy(cmdInfo, xplm_data + (index * (cmdInfo_size/4)), cmdInfo_size);
               XPlmi_CmdInfo* cmd_info = (XPlmi_CmdInfo*)(cmdInfo);
               if (cmd_info->cmd_id == cmd_id && cmd_info->module_id == module_id && args >= cmd_info->min_arg_cnt && args <= cmd_info->max_arg_cnt) {
                  cmd_found = 1;
                  break;
               }
            }
            if (cmd_found == 0 && cmd_id != 5 && cmd_id != 13 && cmd_id != 0)
            {
               return 1;
            }
        }
        i += args;
    }
found_end_marker:
return 0;
}

static uint32_t decode_v2(CdoSequence * seq, uint32_t * p, uint32_t l, uint32_t be) {
    uint32_t hdrlen = u32xe(p[0]);
    uint32_t len = u32xe(p[3]);
    uint32_t total_len = hdrlen + 1 + len;
    uint32_t i = hdrlen + 1;
    if (l < total_len) {
        fprintf(stderr, "warning: incomplete CDO buffer %"PRIu32", expected %"PRIu32"\n", l, total_len);
        total_len = l;
    } else if (l > total_len) {
        l = total_len;
    }

    if (decode_v2_cmd(seq, p, &i, l, be)) {
        return 1;
    }

    if (i < total_len) {
        fprintf(stderr, "warning: mismatching CDO word count %"PRIu32", expected %"PRIu32"\n", i, total_len);
    }
    return 0;
}

static uint32_t checksum32(uint32_t * p, uint32_t l, uint32_t be) {
    uint32_t csum = 0;
    while (l-- > 0) csum += u32xe(*p++);
    return ~csum;
}

int is_cdo_data(void * data, size_t size) {
    uint32_t * p = (uint32_t *)data;
    uint32_t hdrlen = 0;
    uint32_t identification;
    uint32_t csum;
    uint32_t be = 0;

    if (size > 4) {
        hdrlen = u32xe(p[0]);
        if (hdrlen >= 0x10000) {
            be = 1;
            hdrlen = u32xe(p[0]);
            if (hdrlen >= 0x10000) return 0;
        }
    }
    if (hdrlen < 4 || hdrlen >= size/4) return 0;
    csum = checksum32(p, hdrlen, be);
    if (u32xe(p[hdrlen]) != csum) return 0;
    identification = u32xe(p[1]);
    if (identification != 0x584C4E58 && identification != 0x004F4443) return 0;
    return 1;
}

CdoSequence * decode_cdo_binary(const void * data, size_t size) {
    CdoSequence * seq;
    uint32_t (*decode)(CdoSequence * seq, uint32_t * p, uint32_t l, uint32_t be) = NULL;
    uint32_t * p = (uint32_t *)data;
    uint32_t hdrlen = 0;
    uint32_t version;
    uint32_t identification;
    uint32_t csum;
    uint32_t be = 0;

    if (size > 4) {
        hdrlen = u32xe(p[0]);
        if (hdrlen >= 0x10000) {
            be = 1;
            hdrlen = u32xe(p[0]);
            if (hdrlen >= 0x10000) {
                fprintf(stderr, "invalid header length %#"PRIx32"\n", hdrlen);
                return NULL;
            }
        }
    }
    if (hdrlen < 4 || hdrlen >= size/4) {
        fprintf(stderr, "invalid header length %"PRIu32"\n", hdrlen);
        return NULL;
    }
    csum = checksum32(p, hdrlen, be);
    if (u32xe(p[hdrlen]) != csum) {
        fprintf(stderr, "invalid header checksum %#"PRIx32"\n", csum);
        return NULL;
    }
    identification = u32xe(p[1]);
    version = u32xe(p[2]);
    if (version < CDO_VERSION_1_50) {
        if (identification != 0x584C4E58) {
            fprintf(stderr, "invalid identification word %#"PRIx32"\n", identification);
            return NULL;
        }
        decode = decode_v1;
    } else if (version < CDO_VERSION_3_00) {
        if (identification != 0x004F4443) {
            fprintf(stderr, "invalid identification word %#"PRIx32"\n", identification);
            return NULL;
        }
        decode = decode_v2;
    } else {
        fprintf(stderr, "unsupported version %#"PRIx32"\n", version);
        return NULL;
    }

    seq = cdocmd_create_sequence();
    seq->version = version;
    if (decode(seq, p, size/4, be) != 0) {
        cdocmd_delete_sequence(seq);
        return NULL;
    }
    return seq;
}

static uint32_t * cdobuf;
static uint32_t cdocap;

static void clear_cdobuf(void) {
    cdobuf = NULL;
    cdocap = 0;
}

static uint32_t * grow_cdobuf(uint32_t len) {
    if (cdocap < len) {
        do {
            cdocap = cdocap ? cdocap * 2 : 1;
        } while (cdocap < len);
        cdobuf = (uint32_t *)realloc(cdobuf, cdocap * sizeof *cdobuf);
    }
    return cdobuf;
}

static void * encode_v1(CdoSequence * seq, size_t * sizep, uint32_t be) {
    LINK * l = seq->cmds.next;
    uint32_t align = 1;                 /* TODO: is this needed? */
    uint32_t hdrlen = 4;
    uint32_t pos = hdrlen + 1;
    uint32_t * p = grow_cdobuf(pos);
    CdoCommand * section_cmd = NULL;
    uint32_t section_pos = 0;
    uint32_t sections = 0;
    uint32_t commands = 0;
    p[0] = u32xe(pos - 1);
    p[1] = u32xe(0x584C4E58);
    p[2] = u32xe(seq->version);
    if (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        if (cmd->type != CdoCmdSection) {
            fprintf(stderr, "missing section information\n");
            clear_cdobuf();
            return NULL;
        }
    }
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case CdoCmdSection:
            if (section_cmd != NULL) {
                p[section_pos+0] = u32xe(section_cmd->id);
                p[section_pos+1] = u32xe(pos - section_pos - 2);
                p[section_pos+2] = u32xe(commands - 1);
            }
            section_cmd = cmd;
            section_pos = pos;
            commands = 0;
            sections++;
            pos += 3;
            break;
#if 0
        case CdoCmdSetBaseAddress:
            fprintf(f, "set_base_address ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, "\n");
            break;
#endif
        case CdoCmdComment:
            continue;
        case CdoCmdWrite:
            if (cmd->count == 1) {
                if ((cmd->dstaddr >> 32) == 0) {
                    p = grow_cdobuf(pos + 3);
                    p[pos++] = u32xe(CMD1_WRITE);
                    p[pos++] = u32xe_lo(cmd->dstaddr);
                } else {
                    p = grow_cdobuf(pos + 4);
                    p[pos++] = u32xe(CMD1_WRITE64);
                    p[pos++] = u32xe_lo(cmd->dstaddr);
                }
            } else {
                uint32_t offset = -(pos + 4) & (align - 1);
                p = grow_cdobuf(pos + 4 + offset + cmd->count);
                p[pos++] = u32xe(CMD1_DMA_WRITE);
                p[pos++] = u32xe_lo(cmd->dstaddr);
                p[pos++] = u32xe(cmd->count);
                p[pos++] = u32xe(offset);
                while (offset-- > 0) p[pos++] = u32xe(0);
            }
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
#if 0
        case CdoCmdSetBlock:
            fprintf(f, "set ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " %"PRIu32" ", cmd->count);
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
#endif
        case CdoCmdMaskWrite:
            if ((cmd->dstaddr >> 32) == 0) {
                p = grow_cdobuf(pos + 4);
                p[pos++] = u32xe(CMD1_MASK_WRITE);
                p[pos++] = u32xe_lo(cmd->dstaddr);
            } else {
                p = grow_cdobuf(pos + 5);
                p[pos++] = u32xe(CMD1_MASK_WRITE64);
                p[pos++] = u32xe_hi(cmd->dstaddr);
                p[pos++] = u32xe_lo(cmd->dstaddr);
            }
            p[pos++] = u32xe(cmd->mask);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdMaskPoll:
            if ((cmd->dstaddr >> 32) == 0) {
                p = grow_cdobuf(pos + 4);
                p[pos++] = u32xe(CMD1_MASK_POLL);
                p[pos++] = u32xe_lo(cmd->dstaddr);
            } else {
                p = grow_cdobuf(pos + 5);
                p[pos++] = u32xe(CMD1_MASK_POLL64);
                p[pos++] = u32xe_hi(cmd->dstaddr);
                p[pos++] = u32xe_lo(cmd->dstaddr);
            }
            p[pos++] = u32xe(cmd->mask);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe((cmd->count + 999)/1000);
            break;
#if 0
        case CdoCmdMaskPollEq:
            fprintf(f, "mask_poll_eq ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->mask);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
#endif
        case CdoCmdDelay:
            p = grow_cdobuf(pos + 2);
            p[pos++] = u32xe(CMD1_DELAY);
            p[pos++] = u32xe((cmd->value + 999)/1000);
            break;
        case CdoCmdDmaXfer:
            p = grow_cdobuf(pos + 8);
            p[pos++] = u32xe(CMD1_DMA_XFER);
            p[pos++] = u32xe_hi(cmd->srcaddr);
            p[pos++] = u32xe_lo(cmd->srcaddr);
            p[pos++] = u32xe_hi(cmd->dstaddr);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->count);
            p[pos++] = u32xe(cmd->flags);
            if (cmd->buf) {
                p = grow_cdobuf(pos + cmd->count);
                memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
                byte_swap_buffer(p + pos, cmd->count, be);
                pos += cmd->count;
            }
            break;
        case CdoCmdNpiSeq:
            p = grow_cdobuf(pos + 3);
            p[pos++] = u32xe(CMD1_NPI_SEQ);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdNpiPreCfg:
            p = grow_cdobuf(pos + 3);
            p[pos++] = u32xe(CMD1_NPI_PRECFG);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdNpiWrite:
            p = grow_cdobuf(pos + 4 + cmd->count);
            p[pos++] = u32xe(CMD1_NPI_WRITE);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->flags);
            p[pos++] = u32xe(cmd->count);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdNpiShutdown:
            p = grow_cdobuf(pos + 3);
            p[pos++] = u32xe(CMD1_NPI_SHUTDN);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuSetCrc32:
            p = grow_cdobuf(pos + 3);
            p[pos++] = u32xe(CMD1_CFU_SET_CRC32);
            p[pos++] = u32xe(cmd->flags);
            if (cmd->flags != 0) {
                p[pos++] = u32xe(cmd->value);
            }
            break;
        case CdoCmdCfuDecompress:
            p = grow_cdobuf(pos + 2);
            p[pos++] = u32xe(CMD1_CFU_DECOMPRESS);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuCramRW:
            p = grow_cdobuf(pos + 2);
            p[pos++] = u32xe(CMD1_CFU_CRAM_RW);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdCfuSeuGo:
            p = grow_cdobuf(pos + 2);
            p[pos++] = u32xe(CMD1_CFU_SEU_GO);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuCrc8Dis:
            p = grow_cdobuf(pos + 2);
            p[pos++] = u32xe(CMD1_CFU_CRC8_DIS);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuSsiPerSlrPr:
            p = grow_cdobuf(pos + 2);
            p[pos++] = u32xe(CMD1_CFU_SSI_PER_SLR_PR);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuGsrGsc:
            p = grow_cdobuf(pos + 2);
            p[pos++] = u32xe(CMD1_CFU_GSR_GSC);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuGcapClkEn:
            p = grow_cdobuf(pos + 2);
            p[pos++] = u32xe(CMD1_CFU_GCAP_CLK_EN);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCframeRead:
            if (cmd->count != 16) {
                fprintf(stderr, "error: cfu_cframe_read must have length 16, got %u\n", cmd->count);
            }
            p = grow_cdobuf(pos + 4 + cmd->count);
            p[pos++] = u32xe(CMD1_CFU_CFRAME_READ);
            p[pos++] = u32xe(cmd->flags);
            p[pos++] = u32xe_hi(cmd->dstaddr);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->value);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        default:
            fprintf(stderr, "unknown command (%u)\n", cmd->type);
            break;
        }
        commands++;
    }
    assert(section_cmd != NULL);
    p[section_pos+0] = u32xe(section_cmd->id);
    p[section_pos+1] = u32xe(pos - section_pos - 2);
    p[section_pos+2] = u32xe(commands - 1);
    p[3] = u32xe(sections);
    p[hdrlen] = u32xe(checksum32(p, hdrlen, be));
    clear_cdobuf();
    *sizep = pos*4;
    return p;
}

static void hdr2l(uint32_t ** bufp, uint32_t * posp, uint32_t cmd, uint32_t len, uint32_t be, uint32_t long_header) {
    uint32_t * p = *bufp;
    uint32_t pos = *posp;
    if (len < 255 && !long_header) {
        p = grow_cdobuf(pos + len + 1);
        p[pos++] = u32xe((len << 16) | cmd);
    } else {
        p = grow_cdobuf(pos + len + 2);
        p[pos++] = u32xe((255 << 16) | cmd);
        p[pos++] = u32xe(len);
    }
    *bufp = p;
    *posp = pos;
}

static void hdr2(uint32_t ** bufp, uint32_t * posp, uint32_t cmd, uint32_t len, uint32_t be) {
    hdr2l(bufp, posp, cmd, len, be, 0);
}

static LINK * find_block_end(LINK * l, LINK * lh) {
    uint32_t level = 0;
    while (l != lh) {
        CdoCommand * cmd = all2cmds(l);
        switch (cmd->type) {
        case CdoCmdProc:
        case CdoCmdPsmSequence:
        case CdoCmdBegin:
            level++;
            break;
        case CdoCmdEnd:
            if (level == 0) return l;
            level--;
            break;
        default:
            break;
        }
        l = l->next;
    }
    return lh;
}

static void * encode_v2_cmd(uint32_t version, LINK * l, LINK * lh, uint32_t * posp, uint32_t be, uint32_t * aligned_commandp) {
    uint32_t pos = *posp;
    uint32_t * p = grow_cdobuf(pos);
    uint32_t aligned_command = 0;
    while (l != lh) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
#if 0
        case CdoCmdSetBaseAddress:
            fprintf(f, "set_base_address ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, "\n");
            break;
#endif
        case CdoCmdGeneric:
            hdr2(&p, &pos, cmd->id, cmd->count, be);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdComment:
            break;
        case CdoCmdWrite:
            if (cmd->count == 1) {
                if ((cmd->dstaddr >> 32) == 0) {
                    hdr2(&p, &pos, CMD2_WRITE, 2, be);
                    p[pos++] = u32xe_lo(cmd->dstaddr);
                } else {
                    hdr2(&p, &pos, CMD2_WRITE64, 3, be);
                    p[pos++] = u32xe_hi(cmd->dstaddr);
                    p[pos++] = u32xe_lo(cmd->dstaddr);
                }
                memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
                byte_swap_buffer(p + pos, cmd->count, be);
                pos += cmd->count;
            } else {
                uint32_t padding = 0;
                uint32_t pos_save = pos;
                uint64_t dstaddr = cmd->dstaddr;
                uint32_t count = cmd->count;
                uint32_t count2 = 0;
                void * buf = cmd->buf;
                if (auto_align && dstaddr >= 0xf6000000 && dstaddr < 0xf8000000) {
                    /* DMA writes to NPI must be 16 byte aligned on
                     * both address and size (EDT-983251) */
                    while (count > 0 && ((dstaddr >> 2) & 3) != 0) {
                        if ((dstaddr >> 32) == 0) {
                            hdr2(&p, &pos, CMD2_WRITE, 2, be);
                            p[pos++] = u32xe_lo(dstaddr);
                        } else {
                            hdr2(&p, &pos, CMD2_WRITE64, 3, be);
                            p[pos++] = u32xe_hi(dstaddr);
                            p[pos++] = u32xe_lo(dstaddr);
                        }
                        memcpy(p + pos, buf, sizeof(uint32_t));
                        byte_swap_buffer(p + pos, 1, be);
                        pos++;
                        dstaddr += sizeof(uint32_t);
                        buf = (char *)buf + sizeof(uint32_t);
                        count--;
                    }
                    count2 = count & 3;
                    count -= count2;
                    pos_save = pos;
                }
                do {
                    assert(padding < 4);
                    if (padding) {
                        hdr2(&p, &pos, CMD2_NOP, padding - 1, be);
                        memset(p + pos, 0, (padding - 1)*4);
                        pos += padding - 1;
                    }
                    hdr2(&p, &pos, CMD2_DMA_WRITE, 2 + count, be);
                    p[pos++] = u32xe_hi(dstaddr);
                    p[pos++] = u32xe_lo(dstaddr);
                    if (!auto_align) break;
                    aligned_command = 1;
                    if ((pos & 3) == ((dstaddr >> 2) & 3)) break;
                    padding++;
                    pos = pos_save;
                } while (1);
                memcpy(p + pos, buf, count * sizeof(uint32_t));
                byte_swap_buffer(p + pos, count, be);
                pos += count;
                if (count2 > 0) {
                    dstaddr += count * sizeof(uint32_t);
                    buf = (char *)buf + count * sizeof(uint32_t);
                    while (count2 > 0) {
                        if ((dstaddr >> 32) == 0) {
                            hdr2(&p, &pos, CMD2_WRITE, 2, be);
                            p[pos++] = u32xe_lo(dstaddr);
                        } else {
                            hdr2(&p, &pos, CMD2_WRITE64, 3, be);
                            p[pos++] = u32xe_hi(dstaddr);
                            p[pos++] = u32xe_lo(dstaddr);
                        }
                        memcpy(p + pos, buf, sizeof(uint32_t));
                        byte_swap_buffer(p + pos, 1, be);
                        pos++;
                        dstaddr += sizeof(uint32_t);
                        buf = (char *)buf + sizeof(uint32_t);
                        count2--;
                    }
                }
            }
            break;
        case CdoCmdWriteKeyhole: {
            uint32_t padding = 0;
            uint32_t pos_save = pos;
            do {
                assert(padding < 4);
                if (padding) {
                    hdr2(&p, &pos, CMD2_NOP, padding - 1, be);
                    memset(p + pos, 0, (padding - 1)*4);
                    pos += padding - 1;
                }
                hdr2(&p, &pos, CMD2_DMA_WRITE_KEYHOLE, 3 + cmd->count, be);
                p[pos++] = u32xe_hi(cmd->dstaddr);
                p[pos++] = u32xe_lo(cmd->dstaddr);
                p[pos++] = u32xe(cmd->value);
                if (!auto_align) break;
                aligned_command = 1;
                if ((pos & 3) == ((cmd->dstaddr >> 2) & 3)) break;
                padding++;
                pos = pos_save;
            } while (1);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        }
        case CdoCmdSetBlock:
            hdr2(&p, &pos, CMD2_SET, 4, be);
            p[pos++] = u32xe_hi(cmd->dstaddr);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->count);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdMaskWrite:
            if ((cmd->dstaddr >> 32) == 0) {
                hdr2(&p, &pos, CMD2_MASK_WRITE, 3, be);
                p[pos++] = u32xe_lo(cmd->dstaddr);
            } else {
                hdr2(&p, &pos, CMD2_MASK_WRITE64, 4, be);
                p[pos++] = u32xe_hi(cmd->dstaddr);
                p[pos++] = u32xe_lo(cmd->dstaddr);
            }
            p[pos++] = u32xe(cmd->mask);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdMaskPoll: {
            uint32_t has_flags = cmd->flags != 0;
            uint32_t has_errorcode = cmd->errorcode != 0;
            if ((cmd->dstaddr >> 32) == 0) {
                hdr2(&p, &pos, CMD2_MASK_POLL, has_errorcode ? 6: (has_flags  ? 5 : 4), be);
                p[pos++] = u32xe_lo(cmd->dstaddr);
            } else {
                hdr2(&p, &pos, CMD2_MASK_POLL64, has_errorcode ? 7: (has_flags  ? 6 : 5), be);
                p[pos++] = u32xe_hi(cmd->dstaddr);
                p[pos++] = u32xe_lo(cmd->dstaddr);
            }
            p[pos++] = u32xe(cmd->mask);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe(cmd->count);
            if (has_flags || has_errorcode) {
                p[pos++] = u32xe(cmd->flags);
            }
            if (has_errorcode) {
                p[pos++] = u32xe(cmd->errorcode);
            }
            break;
        }
#if 0
        case CdoCmdMaskPollEq:
            fprintf(f, "mask_poll_eq ");
            print_x64(f, cmd->dstaddr);
            fprintf(f, " ");
            print_x64(f, cmd->mask);
            fprintf(f, " ");
            print_x64(f, cmd->value);
            fprintf(f, "\n");
            break;
#endif
        case CdoCmdDelay:
            hdr2(&p, &pos, CMD2_DELAY, 1, be);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdDmaXfer:
            hdr2(&p, &pos, CMD2_DMA_XFER, 6, be);
            p[pos++] = u32xe_hi(cmd->srcaddr);
            p[pos++] = u32xe_lo(cmd->srcaddr);
            p[pos++] = u32xe_hi(cmd->dstaddr);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->count);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCframeRead: {
            uint32_t padding = 0;
            uint32_t pos_save = pos;
            do {
                assert(padding < 4);
                if (padding) {
                    hdr2(&p, &pos, CMD2_NOP, padding - 1, be);
                    memset(p + pos, 0, (padding - 1)*4);
                    pos += padding - 1;
                }
                hdr2(&p, &pos, CMD2_CFRAME_READ, 4 + cmd->count, be);
                p[pos++] = u32xe(cmd->flags);
                p[pos++] = u32xe_hi(cmd->dstaddr);
                p[pos++] = u32xe_lo(cmd->dstaddr);
                p[pos++] = u32xe(cmd->value);
                if (!auto_align) break;
                aligned_command = 1;
                if ((pos & 3) == 0) break;
                padding++;
                pos = pos_save;
            } while (1);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        }
        case CdoCmdSsitSyncMaster:
            if (search_sync_points)
            {
                num_of_sync_points++;
                slr_sync_points = (uint32_t *)realloc(slr_sync_points, num_of_sync_points * sizeof(uint32_t));
                memcpy(slr_sync_points + (num_of_sync_points - 1), &pos, 4);
            }
            hdr2(&p, &pos, CMD2_SSIT_SYNC_MASTER, 0, be);
            break;
        case CdoCmdSsitSyncSlaves:
            if (search_sync_points)
            {
                num_of_sync_points++;
                slr_sync_points = (uint32_t *)realloc(slr_sync_points, num_of_sync_points * sizeof(uint32_t));
                memcpy(slr_sync_points + (num_of_sync_points - 1), &pos, 4);
            }
            hdr2(&p, &pos, CMD2_SSIT_SYNC_SLAVES, 2, be);
            p[pos++] = u32xe(cmd->mask);
            p[pos++] = u32xe(cmd->count);
            break;
        case CdoCmdSsitWaitSlaves:
            hdr2(&p, &pos, CMD2_SSIT_WAIT_SLAVES, 2, be);
            p[pos++] = u32xe(cmd->mask);
            p[pos++] = u32xe(cmd->count);
            break;
        case CdoCmdNop:
            hdr2(&p, &pos, CMD2_NOP, cmd->count, be);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdEventLogging:
            hdr2(&p, &pos, CMD2_EVENT_LOGGING, 1 + cmd->count, be);
            p[pos++] = u32xe(cmd->value);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdSetBoard: {
            uint32_t len = strlen((char *)cmd->buf);
            uint32_t count = (len + 3)/4;
            hdr2(&p, &pos, CMD2_SET_BOARD, count, be);
            memset(p + pos, 0, count*4);
            memcpy(p + pos, cmd->buf, len);
            byte_swap_buffer(p + pos, count, be);
            pos += count;
            break;
        }
        case CdoCmdSetPlmWdt:
            hdr2(&p, &pos, CMD2_SET_PLM_WDT, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdLogString: {
            uint32_t len = strlen((char *)cmd->buf);
            uint32_t count = (len + 3)/4;
            hdr2(&p, &pos, CMD2_LOG_STRING, count, be);
            memset(p + pos, 0, count*4);
            memcpy(p + pos, cmd->buf, len);
            byte_swap_buffer(p + pos, count, be);
            pos += count;
            break;
        }
        case CdoCmdLogAddress:
            if ((cmd->srcaddr >> 32) == 0) {
                hdr2(&p, &pos, CMD2_LOG_ADDRESS, 1, be);
                p[pos++] = u32xe_lo(cmd->srcaddr);
            } else {
                hdr2(&p, &pos, CMD2_LOG_ADDRESS, 2, be);
                p[pos++] = u32xe_lo(cmd->srcaddr);
                p[pos++] = u32xe_hi(cmd->srcaddr);
            }
            break;
        case CdoCmdMarker: {
            uint32_t len = strlen((char *)cmd->buf);
            uint32_t count = (len + 3)/4;
            if (cmd->value == MARKER_DEVICE || cmd->value == MARKER_DATE)
            {
                break;
            }
            /* Search for the START markers with string "NOC Start up", only for SSIT devices */
            if (search_sync_points && (cmd->value == 0x64 || cmd->value == 0x65) && (strcmp((char*)cmd->buf, "NOC Startup") == 0)) {
                /* Store the offset of each of the START and END marker with "NOC Start up" string */
                if (cmd->value == 0x64) {
                    num_start_markers++;
                    noc_start_markers = (uint32_t *)realloc(noc_start_markers, num_start_markers * 2 * sizeof(uint32_t));
                    memcpy(noc_start_markers + (num_start_markers - 1), &pos, 4);
                } else {
                    num_end_markers++;
                    noc_end_markers = (uint32_t *)realloc(noc_end_markers, num_end_markers * 2 * sizeof(uint32_t));
                    memcpy(noc_end_markers + (num_end_markers - 1), &pos, 4);
                }
            }
            hdr2(&p, &pos, CMD2_MARKER, 1 + count, be);
            p[pos++] = u32xe(cmd->value);
            memset(p + pos, 0, count*4);
            memcpy(p + pos, cmd->buf, len);
            byte_swap_buffer(p + pos, count, be);
            pos += count;
            break;
        }
        case CdoCmdProc: {
            uint32_t pos_save = pos;
            uint32_t long_header = 0;
            uint32_t aligned_proc_command = 0;
            LINK * blockend = find_block_end(l, lh);
            for (;;) {
                uint32_t hdr_len = long_header ? 2 : 1;
                uint32_t payload_start;
                uint32_t restart = 0;
                uint32_t pos_hdr;
                pos = pos_save;

                if (aligned_proc_command && auto_align && ((pos + hdr_len + 1) & 3) != 0) {
                    /* Align proc if it contains commands that require alignment */
                    uint32_t padding = 4 - ((pos + hdr_len + 1) & 3);
                    hdr2(&p, &pos, CMD2_NOP, padding - 1, be);
                    memset(p + pos, 0, (padding - 1)*4);
                    pos += padding - 1;
                }

                /* Remember header location and reserve space */
                pos_hdr = pos;
                pos += hdr_len;
                payload_start = pos;
                p[pos++] = u32xe(cmd->value);

                /* Store commands as payload */
                p = encode_v2_cmd(version, l, blockend, &pos, be, &aligned_proc_command);

                /* Store header in reserved space. */
                hdr2l(&p, &pos_hdr, CMD2_PROC, pos - payload_start, be, long_header);
                if (pos_hdr != payload_start) {
                    /* Header grew, restart with long header */
                    assert(!long_header);
                    assert(pos_hdr - payload_start == 1);
                    long_header = 1;
                    restart = 1;
                }

                if (aligned_proc_command) {
                    aligned_command = 1;
                    if (auto_align && ((pos_hdr + 1) & 3) != 0) {
                        /* Update padding. */
                        restart = 1;
                    }
                }

                if (!restart) break;
            }
            l = blockend;
            if (l != lh) l = l->next;
            break;
        }
        case CdoCmdBegin: {
            uint32_t len = strlen((char *)cmd->buf);
            uint32_t count = (len + 3)/4;
            LINK * blockend = find_block_end(l, lh);
            uint32_t payload_start;
            hdr2(&p, &pos, CMD2_BLOCK_BEGIN, 1 + count, be);
            payload_start = pos++;
            memset(p + pos, 0, count*4);
            memcpy(p + pos, cmd->buf, len);
            byte_swap_buffer(p + pos, count, be);
            pos += count;
            p = encode_v2_cmd(version, l, blockend, &pos, be, &aligned_command);
            p[payload_start] = u32xe(pos - payload_start - 1);
            l = blockend;
            break;
        }
        case CdoCmdEnd:
            hdr2(&p, &pos, CMD2_BLOCK_END, 0, be);
            break;
        case CdoCmdBreak:
            if (cmd->value == 1) {
                hdr2(&p, &pos, CMD2_BREAK, 0, be);
            } else {
                hdr2(&p, &pos, CMD2_BREAK, 1, be);
                p[pos++] = u32xe(cmd->value);
            }
            break;
        case CdoCmdOtCheck:
            hdr2(&p, &pos, CMD2_OT_CHECK, 1, be);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdPsmSequence: {
            uint32_t pos_save = pos;
            uint32_t payload_start;
            LINK * blockend = find_block_end(l, lh);
            hdr2(&p, &pos, CMD2_PSM_SEQUENCE, 0, be);
            payload_start = pos;
            p = encode_v2_cmd(version, l, blockend, &pos, be, &aligned_command);
            hdr2(&p, &pos_save, CMD2_PSM_SEQUENCE, pos - payload_start, be);
            if (pos_save != payload_start) {
                /* Header grew, regenerate payload */
                pos = pos_save;
                p = encode_v2_cmd(version, l, blockend, &pos, be, &aligned_command);
            }
            l = blockend;
            if (l != lh) l = l->next;
            break;
        }
        case CdoCmdScatterWrite:
            hdr2(&p, &pos, CMD2_SCATTER_WRITE, 1 + cmd->count, be);
            p[pos++] = u32xe(cmd->value);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdScatterWrite2:
            hdr2(&p, &pos, CMD2_SCATTER_WRITE2, 2 + cmd->count, be);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe(cmd->mask);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdTamperTrigger:
            hdr2(&p, &pos, CMD2_TAMPER_TRIGGER, 1, be);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdSetIpiAccess:
            hdr2(&p, &pos, CMD2_SET_IPI_ACCESS, 2, be);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe(cmd->mask);
            break;
        case CdoCmdNpiSeq:
            if (version >= CDO_VERSION_2_00) goto npi_error;
            hdr2(&p, &pos, CMD2_NPI_SEQ, 2, be);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdNpiPreCfg:
            if (version >= CDO_VERSION_2_00) goto npi_error;
            hdr2(&p, &pos, CMD2_NPI_PRECFG, 2, be);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdNpiWrite:
            if (version >= CDO_VERSION_2_00) goto npi_error;
            hdr2(&p, &pos, CMD2_NPI_WRITE, 2 + cmd->count, be);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->flags);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdNpiShutdown:
            if (version >= CDO_VERSION_2_00) goto npi_error;
            hdr2(&p, &pos, CMD2_NPI_SHUTDN, 2, be);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe(cmd->flags);
            break;

        case CdoCmdPmGetApiVersion:
            hdr2(&p, &pos, CMD2_PM_GET_API_VERSION, 0, be);
            break;
        case CdoCmdPmGetDeviceStatus:
            hdr2(&p, &pos, CMD2_PM_GET_DEVICE_STATUS, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmRegisterNotifier:
            hdr2(&p, &pos, CMD2_PM_REGISTER_NOTIFIER, 4, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->mask);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe(cmd->count);
            break;
        case CdoCmdPmRequestSuspend:
            hdr2(&p, &pos, CMD2_PM_REQUEST_SUSPEND, 4, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->flags);
            p[pos++] = u32xe(cmd->count);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdPmSelfSuspend:
            hdr2(&p, &pos, CMD2_PM_SELF_SUSPEND, 5, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->count);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe_hi(cmd->dstaddr);
            break;
        case CdoCmdPmForcePowerdown:
            hdr2(&p, &pos, CMD2_PM_FORCE_POWERDOWN, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdPmAbortSuspend:
            hdr2(&p, &pos, CMD2_PM_ABORT_SUSPEND, 2, be);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmRequestWakeup:
            hdr2(&p, &pos, CMD2_PM_REQUEST_WAKEUP, 4, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe_lo(cmd->dstaddr);
            p[pos++] = u32xe_hi(cmd->dstaddr);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdPmSetWakeupSource:
            hdr2(&p, &pos, CMD2_PM_SET_WAKEUP_SOURCE, 3, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdPmSystemShutdown:
            hdr2(&p, &pos, CMD2_PM_SYSTEM_SHUTDOWN, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdPmRequestDevice:
            hdr2(&p, &pos, CMD2_PM_REQUEST_DEVICE, 4, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe(cmd->count);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdPmReleaseDevice:
            hdr2(&p, &pos, CMD2_PM_RELEASE_DEVICE, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmSetRequirement:
            hdr2(&p, &pos, CMD2_PM_SET_REQUIREMENT, 4, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe(cmd->count);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdPmSetMaxLatency:
            hdr2(&p, &pos, CMD2_PM_SET_MAX_LATENCY, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->count);
            break;
        case CdoCmdPmResetAssert:
            hdr2(&p, &pos, CMD2_PM_RESET_ASSERT, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdPmResetGetStatus:
            hdr2(&p, &pos, CMD2_PM_RESET_GET_STATUS, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmPinctrlRequest:
            hdr2(&p, &pos, CMD2_PM_PINCTRL_REQUEST, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmPinctrlRelease:
            hdr2(&p, &pos, CMD2_PM_PINCTRL_RELEASE, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmPinctrlGetFunction:
            hdr2(&p, &pos, CMD2_PM_PINCTRL_GET_FUNCTION, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmPinctrlSetFunction:
            hdr2(&p, &pos, CMD2_PM_PINCTRL_SET_FUNCTION, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdPmPinctrlConfigParamGet:
            hdr2(&p, &pos, CMD2_PM_PINCTRL_CONFIG_PARAM_GET, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdPmPinctrlConfigParamSet:
            hdr2(&p, &pos, CMD2_PM_PINCTRL_CONFIG_PARAM_SET, 3, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->flags);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdPmIoctl:
            hdr2(&p, &pos, CMD2_PM_IOCTL, 2 + cmd->count, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->flags);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdPmQueryData:
            hdr2(&p, &pos, CMD2_PM_QUERY_DATA, 1 + cmd->count, be);
            p[pos++] = u32xe(cmd->id);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdPmClockEnable:
            hdr2(&p, &pos, CMD2_PM_CLOCK_ENABLE, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmClockDisable:
            hdr2(&p, &pos, CMD2_PM_CLOCK_DISABLE, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmClockGetState:
            hdr2(&p, &pos, CMD2_PM_CLOCK_GETSTATE, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmClockSetDivider:
            hdr2(&p, &pos, CMD2_PM_CLOCK_SETDIVIDER, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdPmClockGetDivider:
            hdr2(&p, &pos, CMD2_PM_CLOCK_GETDIVIDER, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmClockSetRate:
            hdr2(&p, &pos, CMD2_PM_CLOCK_SETRATE, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdPmClockGetRate:
            hdr2(&p, &pos, CMD2_PM_CLOCK_GETRATE, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmClockSetParent:
            hdr2(&p, &pos, CMD2_PM_CLOCK_SETPARENT, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdPmClockGetParent:
            hdr2(&p, &pos, CMD2_PM_CLOCK_GETPARENT, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmPllSetParameter:
            hdr2(&p, &pos, CMD2_PM_PLL_SET_PARAMETER, 3, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->flags);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdPmPllGetParameter:
            hdr2(&p, &pos, CMD2_PM_PLL_GET_PARAMETER, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdPmPllSetMode:
            hdr2(&p, &pos, CMD2_PM_PLL_SET_MODE, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdPmPllGetMode:
            hdr2(&p, &pos, CMD2_PM_PLL_GET_MODE, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmAddSubsystem:
            hdr2(&p, &pos, CMD2_PM_ADD_SUBSYSTEM, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmDestroySubsystem:
            hdr2(&p, &pos, CMD2_PM_DESTROY_SUBSYSTEM, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmDescribeNodes:
            hdr2(&p, &pos, CMD2_PM_DESCRIBE_NODES, 0 + cmd->count, be);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;

        case CdoCmdPmAddNode:
            hdr2(&p, &pos, CMD2_PM_ADD_NODE, 1 + cmd->count, be);
            p[pos++] = u32xe(cmd->id);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdPmAddNodeParent:
            hdr2(&p, &pos, CMD2_PM_ADD_NODE_PARENT, 1 + cmd->count, be);
            p[pos++] = u32xe(cmd->id);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdPmAddNodeName: {
            uint32_t len = strlen((char *)cmd->buf);
            uint32_t count = (len + 3)/4;
            /* This should not be necessary, but its there for now to
             * avoid null termination issues in libpm. */
            if (count < 4) count = 4;
            hdr2(&p, &pos, CMD2_PM_ADD_NODE_NAME, 1 + count, be);
            p[pos++] = u32xe(cmd->id);
            memset(p + pos, 0, count*4);
            memcpy(p + pos, cmd->buf, len);
            byte_swap_buffer(p + pos, count, be);
            pos += count;
            break;
        }
        case CdoCmdPmAddRequirement:
            hdr2(&p, &pos, CMD2_PM_ADD_REQUIREMENT, 3 + cmd->count, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe(cmd->flags);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdPmSetCurrentSubsystem:
            hdr2(&p, &pos, CMD2_PM_SET_CURRENT_SUBSYSTEM, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmInitNode:
            hdr2(&p, &pos, CMD2_PM_INIT_NODE, 2 + cmd->count, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdPmFeatureCheck:
            hdr2(&p, &pos, CMD2_PM_FEATURE_CHECK, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmIsoControl:
            hdr2(&p, &pos, CMD2_PM_ISO_CONTROL, 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdPmBisr:
            hdr2(&p, &pos, CMD2_PM_BISR, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmApplyTrim:
            hdr2(&p, &pos, CMD2_PM_APPLY_TRIM, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmNocClockEnable:
            hdr2(&p, &pos, CMD2_PM_NOC_CLOCK_ENABLE, 1 + cmd->count, be);
            p[pos++] = u32xe(cmd->id);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdPmIfNocClockEnable:
            hdr2(&p, &pos, CMD2_PM_IF_NOC_CLOCK_ENABLE, cmd->flags != 1 ? 3 : 2, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            if (cmd->flags != 1) {
                p[pos++] = u32xe(cmd->flags);
            }
            break;
        case CdoCmdPmActivateSubsystem:
            hdr2(&p, &pos, CMD2_PM_ACTIVATE_SUBSYSTEM, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdPmSetNodeAccess:
            hdr2(&p, &pos, CMD2_PM_SET_NODE_ACCESS, 1 + cmd->count, be);
            p[pos++] = u32xe(cmd->id);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;
        case CdoCmdPmForceHc:
            hdr2(&p, &pos, CMD2_PM_FORCE_HC, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdCfuSetCrc32:
            if (version >= CDO_VERSION_2_00) goto cfu_error;
            if (cmd->flags != 0) {
                hdr2(&p, &pos, CMD2_CFU_SET_CRC32, 2, be);
                p[pos++] = u32xe(cmd->flags);
                p[pos++] = u32xe(cmd->value);
            } else {
                hdr2(&p, &pos, CMD2_CFU_SET_CRC32, 1, be);
                p[pos++] = u32xe(cmd->flags);
            }
            break;
        case CdoCmdCfuDecompress:
            if (version >= CDO_VERSION_2_00) goto cfu_error;
            hdr2(&p, &pos, CMD2_CFU_DECOMPRESS, 1, be);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuCramRW:
            if (version >= CDO_VERSION_2_00) goto cfu_error;
            hdr2(&p, &pos, CMD2_CFU_CRAM_RW, 1, be);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdCfuSeuGo:
            if (version >= CDO_VERSION_2_00) goto cfu_error;
            hdr2(&p, &pos, CMD2_CFU_SEU_GO, 1, be);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuCrc8Dis:
            if (version >= CDO_VERSION_2_00) goto cfu_error;
            hdr2(&p, &pos, CMD2_CFU_CRC8_DIS, 1, be);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuSsiPerSlrPr:
            if (version >= CDO_VERSION_2_00) goto cfu_error;
            hdr2(&p, &pos, CMD2_CFU_SSI_PER_SLR_PR, 1, be);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuGsrGsc:
            if (version >= CDO_VERSION_2_00) goto cfu_error;
            hdr2(&p, &pos, CMD2_CFU_GSR_GSC, 1, be);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuGcapClkEn:
            if (version >= CDO_VERSION_2_00) goto cfu_error;
            hdr2(&p, &pos, CMD2_CFU_GCAP_CLK_EN, 1, be);
            p[pos++] = u32xe(cmd->flags);
            break;
        case CdoCmdCfuCfiType:
            if (version >= CDO_VERSION_2_00) goto cfu_error;
            hdr2(&p, &pos, CMD2_CFU_CFI_TYPE, 1, be);
            p[pos++] = u32xe(cmd->value);
            break;
        case CdoCmdEmSetAction:
            hdr2(&p, &pos, CMD2_EM_SET_ACTION, 3, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe(cmd->mask);
            break;
        case CdoCmdLdrSetImageInfo:
            hdr2(&p, &pos, CMD2_LDR_SET_IMAGE_INFO, 4, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->value);
            p[pos++] = u32xe(cmd->mask);
            p[pos++] = u32xe(cmd->count);
            break;
        case CdoCmdLdrCframeClearCheck:
            hdr2(&p, &pos, CMD2_LDR_CFRAME_CLEAR_CHECK, 1, be);
            p[pos++] = u32xe(cmd->id);
            break;
        case CdoCmdSemNpiTable:
            hdr2(&p, &pos, CMD2_SEM_NPI_TABLE, 2 + cmd->count, be);
            p[pos++] = u32xe(cmd->id);
            p[pos++] = u32xe(cmd->flags);
            memcpy(p + pos, cmd->buf, cmd->count * sizeof(uint32_t));
            byte_swap_buffer(p + pos, cmd->count, be);
            pos += cmd->count;
            break;

        default:
            fprintf(stderr, "unknown command (%u)\n", cmd->type);
            break;
        npi_error:
            fprintf(stderr, "npi related commands are not supported in CDO version 2 or newer\n");
            break;
        cfu_error:
            fprintf(stderr, "cfu related commands are not supported in CDO version 2 or newer\n");
            break;
        }
    }
    if (aligned_commandp) *aligned_commandp = aligned_command;
    *posp = pos;
    return p;
}

static void * encode_v2(CdoSequence * seq, size_t * sizep, uint32_t be) {
    LINK * l = seq->cmds.next;
    uint32_t hdrlen = 4;
    uint32_t pos = hdrlen + 1;
    uint32_t * p = grow_cdobuf(pos);
    p[0] = u32xe(pos - 1);
    p[1] = u32xe(0x004F4443);
    p[2] = u32xe(seq->version);
    p = encode_v2_cmd(seq->version, l, &seq->cmds, &pos, be, NULL);
    p[3] = u32xe(pos - hdrlen - 1);
    p[4] = u32xe(checksum32(p, hdrlen, be));
    clear_cdobuf();
    *sizep = pos*4;
    return p;
}

void cdoseq_write_header_raw(FILE * f, CdoSequence ** seq) {
    LINK * l = (*seq)->cmds.next;
    char * buf;
    fprintf(f, "Xilinx ASCII NPI Deviceimage\n");

    while (l != &((*seq)->cmds)) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if (cmd->type != CdoCmdMarker) {
            /* header markers are at the top */
            break;
        }
        switch (cmd->value) {
        case 1:
            buf = "Created by";
            break;
        case 2:
            buf = "Design name:";
            break;
        case 3:
            buf = "Architecture:";
            break;
        case 4:
            buf = "Part:";
            break;
        case 5:
            buf = "SLR:";
            break;
        case 6:
            buf = "Date:";
            break;
        default:
            buf = NULL;
            break;
        }
        if (buf) {
            fprintf(f, "%s %s\n", buf, (char *)cmd->buf);
            cdocmd_free(cmd);
        }
    }
}

void * cdoseq_to_binary(CdoSequence * seq, size_t * sizep, uint32_t be) {
    void * (*encode)(CdoSequence * seq, size_t * sizep, uint32_t be) = NULL;
    *sizep = 0;
    if (seq->version < CDO_VERSION_1_50) {
        encode = encode_v1;
    } else if (seq->version < CDO_VERSION_3_00) {
        encode = encode_v2;
    } else {
        fprintf(stderr, "unsupported version %#"PRIx32"\n", seq->version);
        return NULL;
    }
    return encode(seq, sizep, be);
}

void search_for_sync_points(void)
{
    num_of_sync_points = 0;
    num_start_markers = 0;
    num_end_markers = 0;
    search_sync_points = 1;
    slr_sync_points = NULL;
    noc_start_markers = NULL;
    noc_end_markers = NULL;
}

uint32_t* get_slr_sync_point_offsets(void)
{
    search_sync_points = 0;
    return slr_sync_points;
}

uint32_t* get_slr_start_marker_offsets(void)
{
    search_sync_points = 0;
    return noc_start_markers;
}

uint32_t* get_slr_end_marker_offsets(void)
{
    search_sync_points = 0;
    return noc_end_markers;
}

size_t get_num_of_sync_points(void)
{
    return num_of_sync_points;
}

size_t get_num_start_markers(void)
{
    return num_start_markers;
}

size_t get_num_end_markers(void)
{
    return num_end_markers;
}

