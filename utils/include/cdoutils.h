/******************************************************************************
* Copyright 2015-2022 Xilinx, Inc.
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

#pragma once

#ifndef _CDOUTIL_H_
#define _CDOUTIL_H_

#include <string>
#include <vector>
#include "bootgenenum.h"
#include "logger.h"
#include <sys/stat.h>

#ifdef ENABLE_WDI
#include <isl/iostreams/filtering_stream.hpp>
#include <isl/iostreams/util.hpp>
#endif

extern "C" {
#include "cdo-command.h"
#include "cdo-load.h"
};

#define CDO_SSIT_SYNC_MASTER_CMD 0x0E010000
#define CDO_SSIT_SYNC_SLAVES_CMD 0x0F010200
#define CDO_CMD_END 0x00000100
#define CDO_IDENTIFICATION 0x004f4443
#define CDO_VERSION 0x00000200
#define CDO_REMAINING_WORDS 0x4
#define SBI_KEYHOLE_SIZE            0x10000

#define PMC_SBI_BUF_ADDR            0xF2100000
#define MASTER_PMC_BASE_ADDR        0xF0000000
#define SLR1_PMC_BASE_ADDR          0x108000000
#define SLR2_PMC_BASE_ADDR          0x110000000
#define SLR3_PMC_BASE_ADDR          0x118000000

#define SLR1_SBI_BUF_BASE_ADDR      (SLR1_PMC_BASE_ADDR + (PMC_SBI_BUF_ADDR - MASTER_PMC_BASE_ADDR))
#define SLR2_SBI_BUF_BASE_ADDR      (SLR2_PMC_BASE_ADDR + (PMC_SBI_BUF_ADDR - MASTER_PMC_BASE_ADDR))
#define SLR3_SBI_BUF_BASE_ADDR      (SLR3_PMC_BASE_ADDR + (PMC_SBI_BUF_ADDR - MASTER_PMC_BASE_ADDR))

static uint64_t slr_sbi_base_array[4] = { PMC_SBI_BUF_ADDR, SLR1_SBI_BUF_BASE_ADDR, SLR2_SBI_BUF_BASE_ADDR, SLR3_SBI_BUF_BASE_ADDR };

typedef struct
{
    uint8_t cmd_id;
    uint8_t handler_id;
    uint8_t length;
    uint8_t reserved;
} CdoCommandHeader;

typedef struct
{
    CdoCommandHeader header;
    uint32_t mask;
    uint32_t timeout_ms;
} CdoSsitSlaves;

typedef struct
{
    uint32_t partition_index;
    Checksum::Type checksum;
    Authentication::Type authentication;
    Encryption::Type encryption;
    bool top_chunk_processed;
} SsitConfigPartitionSecurityInfo;

typedef struct
{
    std::string file;                       /* SLR partition PDI/CDO file */
    SlrId::Type index;                      /* SLR index - master, slr-1, slr-2 etc. */
    uint8_t* data;                          /* SLR partition data */
    size_t size;                            /* SLR partition size */
    uint32_t offset;                        /* To track the offset of data pointer for packing next chunk */
    std::vector<uint32_t> sync_addresses;   /* List of addresses for CDO_SSIT_SYNC_MASTER_CMD/CDO_SSIT_SYNC_SLAVES_CMD */
    std::vector<uint32_t> partition_sizes;  /* Individual partition sizes of each SLR PDI */
    uint32_t partition_index;               /* To track current partition of SLR PDI to chunk and pack */
    uint32_t partition_offset;              /* To track current partition of SLR PDI to chunk and pack */
    uint32_t sync_points;                   /* To track how many sync points are processed within SLR PDI */
    uint32_t num_chunks;                    /* To track no of chunks created or processed */
    std::vector<SsitConfigPartitionSecurityInfo*> security_info;
    bool eof;                               /* To track end of file for each SLR PDI */
} SsitConfigSlrInfo;

typedef struct
{
    uint8_t slr_num;
    size_t offset;
    size_t size;
    size_t sync_points;
} SsitConfigSlrLog;

typedef struct
{
    uint32_t remaining_words;
    uint32_t id_word;
    uint32_t version;
    uint32_t length;
    uint32_t checksum;
} VersalCdoHeader;

typedef struct
{
    uint32_t header;
    uint32_t length;
    uint32_t hi_address;
    uint32_t lo_address;
    uint8_t* data;
} CdoCommandDmaWrite;

typedef struct
{
    std::string file;
    uint32_t id;
} ImageStorePdiInfo;

typedef struct
{
    uint32_t header;
    uint32_t length;
    uint32_t id;
    uint8_t* data;
} CdoCommandWriteImageStore;

#define CDO_CMD_WRITE_IMAGE_STORE_SIZE 12

struct CdoCmds
{
    typedef enum
    {
        END,
        MASK_POLL,
        MASK_WRITE,
        WRITE,
        DELAY,
        DMA_WRITE,
        MASK_POLL_64,
        MASK_WRITE_64,
        WRITE_64,
        DMA_XFER,
        INIT_SEQ,
        CFI_READ,
        SET,
        WRITE_KEYHOLE,
        SSIT_SYNC_MASTER,
        SSIT_SYNC_SLAVES,
        SSIT_WAIT_SLAVES,
        NOP
    } Type;
};

#define CDO_CMD_DMA_WRITE_SIZE 16

typedef struct
{
    CdoCommandHeader header;
    uint32_t length;
    uint32_t hi_address;
    uint32_t lo_address;
    uint32_t keyhole_size;
    uint8_t* data;
} CdoCommandWriteKeyhole;

#define CDO_CMD_WRITE_KEYHOLE_SIZE 20

typedef struct
{
    CdoCommandHeader header;
    uint32_t length;
} CdoCommandNop;
#define CDO_CMD_NOP_SIZE 4

typedef struct
{
    uint32_t header;
    uint32_t value;
} CdoCommandSync;

typedef struct
{
    CdoCommandHeader header;
} CdoSsitSyncMaster;

/******************************************************************************/
inline CdoSsitSyncMaster* CdoCmdWriteSsitSyncMaster()
{
    CdoSsitSyncMaster* cdoCmd = new CdoSsitSyncMaster;
    cdoCmd->header.reserved = 0x00;
    cdoCmd->header.length = 0x0;
    cdoCmd->header.handler_id = 1;
    cdoCmd->header.cmd_id = CdoCmds::SSIT_SYNC_MASTER;
    LOG_TRACE("CDO_CMD_SSIT_SYNC_MASTER");
    return cdoCmd;
}

/******************************************************************************/
inline CdoCommandNop* CdoCmdNoOperation2(size_t size)
{
    CdoCommandNop* cdoCmd = new CdoCommandNop;
    uint32_t length = size - CDO_CMD_NOP_SIZE - 4;  /* consider the NOP command header and length as 2 NOPs */
    cdoCmd->header.reserved = 0x00;
    cdoCmd->header.length = 0xff;
    cdoCmd->header.handler_id = 1;
    cdoCmd->header.cmd_id = CdoCmds::NOP;
    cdoCmd->length = (length / CDO_CMD_NOP_SIZE);
    LOG_TRACE("   CDO_CMD_NOP - %d", cdoCmd->length + 2);
    return cdoCmd;
}

/******************************************************************************/
inline CdoCommandHeader* CdoCmdCdoEnd(void)
{
    CdoCommandHeader* cdoCmd = new CdoCommandHeader;
    cdoCmd->reserved = 0x00;
    cdoCmd->length = 0x0;
    cdoCmd->handler_id = 1;
    cdoCmd->cmd_id = CdoCmds::END;
    LOG_TRACE("CDO_CMD_END");
    return cdoCmd;
}

/******************************************************************************/
inline CdoSsitSlaves* CdoCmdWriteSsitSyncSlaves(uint8_t mask)
{
    CdoSsitSlaves* cdoCmd = new CdoSsitSlaves;
    cdoCmd->header.reserved = 0x00;
    cdoCmd->header.length = 0x2;
    cdoCmd->header.handler_id = 1;
    cdoCmd->header.cmd_id = CdoCmds::SSIT_SYNC_SLAVES;
    cdoCmd->mask = mask;
    cdoCmd->timeout_ms = 0xFFFF;
    LOG_TRACE("CDO_CMD_SSIT_SYNC_SLAVES - %d, %x", cdoCmd->mask, cdoCmd->timeout_ms);
    return cdoCmd;
}

/******************************************************************************/
inline CdoSsitSlaves* CdoCmdWriteSsitWaitSlaves(uint32_t mask)
{
    CdoSsitSlaves* cdoCmd = new CdoSsitSlaves;
    cdoCmd->header.reserved = 0x00;
    cdoCmd->header.length = 0x2;
    cdoCmd->header.handler_id = 1;
    cdoCmd->header.cmd_id = CdoCmds::SSIT_WAIT_SLAVES;
    cdoCmd->mask = mask;
    cdoCmd->timeout_ms = 0xFFFF;
    LOG_TRACE("CDO_CMD_SSIT_WAIT_SLAVES - %d, %x", cdoCmd->mask, cdoCmd->timeout_ms);
    return cdoCmd;
}

/******************************************************************************/
inline CdoCommandNop* CdoCmdNoOperation(size_t size)
{
    CdoCommandNop* cdoCmd = new CdoCommandNop;
    uint8_t length = size - CDO_CMD_NOP_SIZE;
    cdoCmd->header.reserved = 0x00;
    /* If 1 word needs to be ignored, then no need to add length and payload */
    cdoCmd->header.length = length / CDO_CMD_NOP_SIZE;
    cdoCmd->header.handler_id = 1;
    cdoCmd->header.cmd_id = CdoCmds::NOP;
    LOG_TRACE("   CDO_CMD_NOP - %d", cdoCmd->header.length + 1);
    return cdoCmd;
}

/******************************************************************************/
inline CdoCommandWriteKeyhole* CdoCmdWriteKeyHole(uint32_t size, SlrId::Type slr_index)
{
    uint8_t index = slr_index;
    if (slr_index == 4)
    {
        index = 0;
    }
    CdoCommandWriteKeyhole* cdoCmd = new CdoCommandWriteKeyhole;
    cdoCmd->header.reserved = 0x00;
    cdoCmd->header.length = 0xff;
    cdoCmd->header.handler_id = 1;
    cdoCmd->header.cmd_id = CdoCmds::WRITE_KEYHOLE;
    cdoCmd->length = (size / 4) + 3;    /* +3 to accomodate the address lengths and keyhole size*/
    cdoCmd->hi_address = (slr_sbi_base_array[index] >> 32) & 0xFFFFFFFF;
    cdoCmd->lo_address = slr_sbi_base_array[index] & 0xFFFFFFFF;
    cdoCmd->keyhole_size = SBI_KEYHOLE_SIZE / 4;

    LOG_TRACE("   CDO_CMD_WRITE_KEYHOLE: Address-0x%llx, Length-0x%lx (words)", slr_sbi_base_array[index], cdoCmd->length);
    return cdoCmd;
}

/******************************************************************************/
inline bool IsCdoCmdEndFound(const uint8_t* buffer, size_t size)
{
    bool status = false;
    uint32_t end_word = buffer[size - sizeof(CdoCommandHeader)];

    if (end_word == CDO_CMD_END)
    {
        status = true;
    }
    return status;
}

/******************************************************************************/
inline bool IsCdoFile(uint32_t value)
{
    if ((value == 0x584c4e58) || (value == 0x584e4c58) || (value == 0x004f4443) || (value == 0x43444f00))
    {
        return true;
    }
    return false;
}

/******************************************************************************/
inline bool IsCdoFile(std::string file)
{
#ifdef ENABLE_WDI
    isl::iostreams::istream stream(file.c_str(), std::ios_base::binary);
    if (!stream)
    {
        LOG_ERROR("Cannot read file - %s ", (file.c_str()));
    }
    std::string line;
    getline(stream, line);
#else
    std::ifstream stream(file.c_str(), std::ios_base::binary);
    if (!stream)
    {
        LOG_ERROR("Cannot read file - %s ", (file.c_str()));
    }
    std::string line;
    getline(stream, line);
#endif
    if ((line.find("Xilinx ASCII NPI Deviceimage") != std::string::npos) || (line.find("Xilinx ASCII PSAXIMM Deviceimage") != std::string::npos) || (line.find("version") != std::string::npos))
    {
        return true;
    }
    else
    {
        FILE *binFile = fopen(file.c_str(), "rb");

        if (!binFile)
        {
            LOG_ERROR("Cannot read file %s", file.c_str());
        }

        uint32_t value = 0;
        size_t result = fread(&value, 1, 4, binFile);
        if (result == 4)
        {
            if ((value == 0x584c4e58) || (value == 0x584e4c58) || (value == 0x004f4443) || (value == 0x43444f00))
            {
                return true;
            }
        }
        fclose(binFile);
    }
    return false;
}


/******************************************************************************/
inline SlrPdiType GetSlrType(SlrPdiInfo* slr)
{
    if (IsCdoFile(slr->file))
    {
        return SlrPdiType::MASTER_CDO;
    }
    else
    {
        return SlrPdiType::BOOT;
    }
}

/******************************************************************************/
inline uint8_t GetTotalSlrCount(std::list<SlrPdiInfo*> slrPdi)
{
    uint8_t cnt = 0;
    for (std::list<SlrPdiInfo*>::iterator slr_id = slrPdi.begin(); slr_id != slrPdi.end(); slr_id++)
    {
        if (!IsCdoFile((*slr_id)->file))
        {
            cnt++;
        }
    }
    return cnt;
}


/******************************************************************************/
inline bool SortByIndex(SlrPdiInfo* A, SlrPdiInfo* B)
{
    return (A->index < B->index);
}

/* UTILS */

/******************************************************************************/
inline std::vector<std::string> SplitString(std::string strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

/******************************************************************************/
inline void CompareCDOSequences(CdoSequence * user_cdo_seq, std::string golden_cdo_filename, std::string user_cdo_filename)
{
    cdoseq_extract_writes(user_cdo_seq);
    uint32_t * golden_value = NULL;
    uint32_t * user_value = NULL;
    int pos_plus_neg_itr = 2;
    struct stat f_stat;
    bool found = false;
    bool golden_element_found = false;
    std::string golden_cdo;
    std::vector <uint32_t> golden_cdo_write;

    for (int i = 0; i < pos_plus_neg_itr; i++)
    {
        golden_cdo.clear();
        golden_cdo_write.clear();
        golden_value = NULL;
        user_value = NULL;
        if (stat(golden_cdo_filename.c_str(), &f_stat) == 0)
        {
            found = true;
        }
        else
        {
#ifdef _WIN32
            std::string DS = "\\";
#else
            std::string DS = "/";
#endif
            char *s = getenv("HDI_APPROOT");
            if (s != NULL && *s != '\0')
            {
                if (i == 0)
                {
                    golden_cdo = s + DS + "data" + DS + "bootgen" + DS + golden_cdo_filename + "_positive.cdo";
                }
                else
                {
                    golden_cdo = s + DS + "data" + DS + "bootgen" + DS + golden_cdo_filename + "_negative.cdo";
                }
                if (stat(golden_cdo.c_str(), &f_stat) == 0)
                {
                    found = true;
                }
            }
        }
        if (!found)
        {
            LOG_ERROR("Cannot find golden CDO : %s", golden_cdo.c_str());
        }

        CdoSequence * golden_cdo_seq = NULL;
        golden_cdo_seq = cdoseq_load_cdo(golden_cdo.c_str());
        if (golden_cdo_seq == NULL)
        {
            LOG_ERROR("Error parsing CDO file : %s", golden_cdo.c_str());
        }
        if (found)
        {
            LINK * l = golden_cdo_seq->cmds.next;
            while (l != &golden_cdo_seq->cmds) {
                CdoCommand * golden_cmd = all2cmds(l);
                l = l->next;
                golden_value = (uint32_t *)golden_cmd->buf;
                LINK * U = user_cdo_seq->cmds.next;
                while (U != &user_cdo_seq->cmds) {
                    CdoCommand * user_cmd = all2cmds(U);
                    U = U->next;
                    user_value = (uint32_t *)user_cmd->buf;
                    if ((golden_cmd->dstaddr == user_cmd->dstaddr) && (*golden_value == *user_value))
                    {
                        golden_element_found = true;
                        break;
                    }
                }
                if ((i == 0) && (golden_element_found == false))
                {
                    golden_cdo_write.push_back(golden_cmd->dstaddr);
                    golden_cdo_write.push_back(*golden_value);
                }
                else if ((i != 0) && golden_element_found == true)
                {
                    golden_cdo_write.push_back(golden_cmd->dstaddr);
                    golden_cdo_write.push_back(*golden_value);
                }
                golden_element_found = false;
            }
            if (golden_cdo_write.size() != 0)
            {
                LOG_MSG("\n");
                if (i == 0)
                {
                    LOG_WARNING("User Cdo %s has missing write commands with below address and value.\n", user_cdo_filename.c_str());
                }
                else
                {
                    LOG_WARNING("User Cdo %s has invalid write commands with below address and value.\n", user_cdo_filename.c_str());
                }
                for (size_t itr = 0; itr < golden_cdo_write.size(); itr += 2)
                {
                    LOG_MSG(" Address = 0x%x ; Value = 0x%x", golden_cdo_write[itr], golden_cdo_write[itr + 1]);
                }
            }
        }
        found = false;
    }
}

#endif