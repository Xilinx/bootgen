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

#include <map>
#include <sstream>
#include <string>

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
    while (getline(stream, line))
    {
        if (!line.empty() && line[0] != '#')
            break;
    }
#else
    std::ifstream stream(file.c_str(), std::ios_base::binary);
    if (!stream)
    {
        LOG_ERROR("Cannot read file - %s ", (file.c_str()));
    }
    std::string line;
    while (getline(stream, line))
    {
        if (!line.empty() && line[0] != '#')
            break;
    }
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
uint32_t inline get_reg_val(uint32_t golden_nps_addr, uint32_t golden_default_val, std::map<uint32_t, std::vector<uint32_t*>> user_cdo_map, std::string user_cdo_filename)
{
    if(user_cdo_map.find(golden_nps_addr) != user_cdo_map.end())
    {
        if(user_cdo_map[golden_nps_addr].size() > 1)
        {
            LOG_WARNING("More than 1 matching value found for address %x in user Cdo %s", golden_nps_addr, user_cdo_filename.c_str());
        }
        return (*(user_cdo_map[golden_nps_addr].back()));

    }
    else
    {
            LOG_WARNING("Address = 0x%x is missing in user cdo  %s", golden_nps_addr, user_cdo_filename.c_str());
    }
    return golden_default_val;
}

/******************************************************************************/
bool inline ValidateCdoOport(uint32_t golden_nps_base_addr, uint32_t golden_nps_iport, uint32_t golden_nps_oport, uint32_t golden_nps_vc,
    uint32_t golden_nps_dest_id, std::map<uint32_t, std::vector<uint32_t*>> user_cdo_map, std::string user_cdo_filename,
    std::vector<std::vector<uint32_t>>& invalid_pos_cdo_write)
{
    const uint32_t REG_HIGH_ID_OFFSET = 0x100;
    const uint32_t REG_MID_ID_OFFSET = 0x300;
    const uint32_t REG_LOW_ID_OFFSET = 0x320;
    const uint32_t REG_ID_OFFSET = 0x3A0;

    // The REG_ID [9:4] represents HIGH_ID.
    const uint32_t REG_ID_HIGH_MASK = 0x3F0;
    const uint32_t REG_ID_HIGH_SHIFT = 0x4;

    // REG_ID[1:0] represents MID_ID.
    const uint32_t REG_ID_MID_MASK = 0x003;
    const uint32_t REG_ID_MID_SHIFT = 0x0;

    // The DST-ID[11:6] represents HIGH-ID.
    const uint32_t DST_ID_HIGH_MASK = 0xFC0;
    const uint32_t DST_ID_HIGH_SHIFT = 0x6;

    // DST-ID[5:4] represents the MID-ID.
    const uint32_t DST_ID_MID_MASK = 0x030;
    const uint32_t DST_ID_MID_SHIFT = 0x4;

    // The DST-ID[3:0] represents LOW-ID.
    const uint32_t DST_ID_LOW_MASK = 0x00F;
    const uint32_t DST_ID_LOW_SHIFT = 0x0;

    const std::vector<uint32_t> DEFAULT_REG_VALUE = {0xFFFFAAAA, 0xFFFFAAAA, 0x55550000, 0x55550000};
    const uint32_t DEFAULT_REG_MASK = 0x3;

    // read NPS regid content (Note the bit offsets are different that for Dest-ID format)
    uint32_t nps_regid = get_reg_val(golden_nps_base_addr + REG_ID_OFFSET, 0, user_cdo_map, user_cdo_filename);
    if (nps_regid == 0) {
        //std::cout << " reg-id write not found (assume default val 0) " << std::endl;
    }

    // calc offset to routing table register
    // WARNING: changed to NEQ as per DID document HIGH offsets are used when HIGH_ID != NPS(HIGH-ID)
    uint32_t Reg_offset;
    if ((((golden_nps_dest_id & DST_ID_HIGH_MASK) >> DST_ID_HIGH_SHIFT) ^ ((nps_regid & REG_ID_HIGH_MASK) >> REG_ID_HIGH_SHIFT)) != 0)
    {
        Reg_offset = REG_HIGH_ID_OFFSET + ((golden_nps_dest_id & DST_ID_HIGH_MASK) >> DST_ID_HIGH_SHIFT) * 8;
    }
    else if ((((golden_nps_dest_id & DST_ID_MID_MASK) >> DST_ID_MID_SHIFT) ^ ((nps_regid & REG_ID_MID_MASK) >> REG_ID_MID_SHIFT)) != 0)
    {
        Reg_offset = REG_MID_ID_OFFSET + ((golden_nps_dest_id & DST_ID_MID_MASK) >> DST_ID_MID_SHIFT) * 8;
    }
    else
    {
        Reg_offset = REG_LOW_ID_OFFSET + ((golden_nps_dest_id & DST_ID_LOW_MASK) >> DST_ID_LOW_SHIFT) * 8;
    }

    uint32_t shift = golden_nps_vc * 2;
    if (golden_nps_iport & 1)
    {
        shift += 16;
    }
    if (golden_nps_iport >= 2)
    {
        Reg_offset += 4;
    }

    uint32_t Reg_value = get_reg_val(golden_nps_base_addr + Reg_offset, DEFAULT_REG_VALUE[golden_nps_iport], user_cdo_map, user_cdo_filename);

    if (((Reg_value >> shift) & DEFAULT_REG_MASK) != golden_nps_oport)
    {
        invalid_pos_cdo_write.push_back({Reg_offset,Reg_value,shift}); //print expected "golden_nps_oport"
        return false;
    }
    return true;
}

/******************************************************************************/
void inline CompareCDOSequences(CdoSequence * user_cdo_seq, std::string device_name, std::string user_cdo_filename, uint32_t slr_id)
{
    cdoseq_extract_writes(user_cdo_seq);
    int pos_plus_neg_itr = 2;
    struct stat f_stat;
    bool found = false;
    std::string golden_cdo;
    std::vector<std::vector<uint32_t>> invalid_pos_cdo_write;
    std::vector<std::vector<uint32_t>> invalid_neg_cdo_write;

    // Read user CDO and store the addr-value mapping in a map
    std::map<uint32_t, std::vector<uint32_t*>> user_cdo_map;
    LINK * U = user_cdo_seq->cmds.next;
    while (U != &user_cdo_seq->cmds) {
        CdoCommand * user_cmd = all2cmds(U);
        U = U->next;
        if(user_cdo_map.find(user_cmd->dstaddr) != user_cdo_map.end())
        {
            std::vector<uint32_t*> myVector = {(uint32_t *)user_cmd->buf};
            user_cdo_map[user_cmd->dstaddr] = myVector;
        }
        else
        {
            user_cdo_map[user_cmd->dstaddr].push_back((uint32_t *)user_cmd->buf);
        }
    }

    bool secure = (user_cdo_map.find(0xf6ea011c) != user_cdo_map.end() ||
                                user_cdo_map.find(0xf6ea0120) != user_cdo_map.end()||
                                user_cdo_map.find(0xf60a011c) != user_cdo_map.end()||
                                user_cdo_map.find(0xf60a0120) != user_cdo_map.end()||
                                user_cdo_map.find(0xf6f1011c) != user_cdo_map.end()  );

    for (int i = 0; i < pos_plus_neg_itr; i++)
    {
        invalid_pos_cdo_write.clear();
        invalid_neg_cdo_write.clear();
        golden_cdo.clear();
        if(i == 0)
            golden_cdo = device_name + "_positive_list_v0.9.csv"; 
        else
            golden_cdo = device_name + "_negative_list_v0.9.csv";

        if (stat(golden_cdo.c_str(), &f_stat) == 0)
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
                if (secure)
                {
                    golden_cdo = s + DS + "secure" + DS + "data" + DS + "bootgen" + DS + golden_cdo;
                }
                else 
                {
                    golden_cdo = s + DS + "non_secure" + DS + "data" + DS + "bootgen" + DS + golden_cdo;
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

        if(found)
        {
            std::fstream fin;
            fin.open(golden_cdo.c_str(), std::ios::in);
            std::vector<std::string> row;
            std::string line, word;

            while (!fin.eof())
            {
                row.clear();
                std::getline(fin, line);
                std::stringstream lineStream(line);

                while (std::getline(lineStream, word, ','))
                {
                    row.push_back(word);
                }

                //Process positive golden Cdo
                if(i == 0)
                {
                    if(row.size()>1 && row[0] != "#SLR")
                    {
                        uint32_t slr_no = std::stoi(row[0]);

                        if(slr_no == slr_id)
                        {
                            std::string node_type = row[1].substr(4,3);

                            if(node_type == "NMU" || node_type == "NSU" || node_type == "NID")
                            //NID refers to NIDB
                            {
                                uint32_t golden_addr;
                                std::string golden_addr_str = row[3].substr(2);
                                std::stringstream ss_addr;
                                ss_addr << std::hex << golden_addr_str;
                                ss_addr >> golden_addr;

                                uint32_t golden_val;
                                //The 0 val is stored as 0 and not as 0x0
                                if(row[4].size() < 2)
                                {
                                    golden_val = 0;
                                }
                                else
                                {
                                    std::string golden_val_str = row[4].substr(2);
                                    std::stringstream ss_val;
                                    ss_val << std::hex << golden_val_str;
                                    ss_val >> golden_val;
                                }

                                if(user_cdo_map.find(golden_addr) != user_cdo_map.end())
                                {
                                    if(user_cdo_map[golden_addr].size() > 1)
                                    {
                                        LOG_WARNING("More than 1 matching value found for address %x in user Cdo %s", golden_addr, user_cdo_filename.c_str());
                                    }

                                    uint32_t userval = *(user_cdo_map[golden_addr].back());

                                    if(secure && row[2].find("SMID") != std::string::npos)
                                    {
                                        uint32_t golden_mask;
                                        std::string golden_mask_str = row[5].substr(2);
                                        std::stringstream ss_mask;
                                        ss_mask << std::hex << golden_mask_str;
                                        ss_mask >> golden_mask;

                                        if((userval & golden_mask) != golden_val)
                                        {
                                            LOG_ERROR("Secure check failed: addr=0x%x, (userval 0x%x & mask 0x%x) = 0x%x, expected 0x%x in %s",
                                                golden_addr, userval, golden_mask, (userval & golden_mask), golden_val, user_cdo_filename.c_str());
                                        }
                                    }
                                    if(userval != golden_val)
                                    {
                                        invalid_pos_cdo_write.push_back({golden_addr, golden_val});
                                    }
                                }
                                else
                                {
					                LOG_ERROR("Address = 0x%x is missing in user cdo  %s", golden_addr, user_cdo_filename.c_str());
				                }

                            }
                            else if(node_type == "NPS")
                            {
                                uint32_t golden_nps_base_addr;
                                std::string golden_addr_str = row[2].substr(2);
                                std::stringstream ss_addr;
                                ss_addr << std::hex << golden_addr_str;
                                ss_addr >> golden_nps_base_addr;

                                uint32_t golden_nps_iport = std::stoi(row[3]);
                                uint32_t golden_nps_oport = std::stoi(row[4]);
                                uint32_t golden_nps_vc = std::stoi(row[5]);
                                uint32_t golden_nps_dest_id = std::stoi(row[6]);

                                ValidateCdoOport(golden_nps_base_addr, golden_nps_iport, golden_nps_oport, golden_nps_vc,
                                    golden_nps_dest_id, user_cdo_map, user_cdo_filename, invalid_pos_cdo_write);
                            }
                        }
                    }
                }
                //Process negative golden Cdo
                else
                {
                    if(row.size()>1 && row[0] != "#SLR")
                    {
                        uint32_t slr_no = std::stoi(row[0]);

                        if(slr_no == slr_id)
                        {
                            std::string node_type = row[1].substr(4,4);

                            if(node_type == "NIDB")
                            {
                                uint32_t golden_addr;
                                std::string golden_addr_str = row[2].substr(2);
                                std::stringstream ss_addr;
                                ss_addr << std::hex << golden_addr_str;
                                ss_addr >> golden_addr;
                                uint32_t addr_mask = 0xFFFFF000;

                                for (const auto& pair : user_cdo_map)
                                    if((pair.first & addr_mask) == golden_addr)
                                        invalid_neg_cdo_write.push_back({pair.first,*pair.second.back()});

                            }
                        }
                    }
                }
            }
            fin.close();
   
            if (i == 0)
            {
                if(invalid_pos_cdo_write.size() != 0)
                {
                    LOG_WARNING("User Cdo %s has missing write commands with below address and value.", user_cdo_filename.c_str());
                    for (size_t j = 0; j < invalid_pos_cdo_write.size(); j++)
                        if(invalid_pos_cdo_write[j].size() == 2)
                            LOG_MSG(" Address = 0x%x; Value = 0x%x ", invalid_pos_cdo_write[j][0], invalid_pos_cdo_write[j][1]);
                        else
                            LOG_MSG(" Address = 0x%x; Value = 0x%x; Shift = %x", invalid_pos_cdo_write[j][0], invalid_pos_cdo_write[j][1], invalid_pos_cdo_write[j][2]);
                }
            }
            else
            {
                if(invalid_neg_cdo_write.size() != 0)
                {
                    LOG_WARNING("User Cdo %s has invalid write commands with below address and value.", user_cdo_filename.c_str());
                    for (size_t j = 0; j < invalid_neg_cdo_write.size(); j++)
                        if(invalid_neg_cdo_write[j].size() == 2)
                            LOG_MSG(" Address = 0x%x; Value = 0x%x ", invalid_neg_cdo_write[j][0], invalid_neg_cdo_write[j][1]);
                        else
                            LOG_MSG(" Address = 0x%x; Value = 0x%x; Shift = %x", invalid_neg_cdo_write[j][0], invalid_neg_cdo_write[j][1], invalid_neg_cdo_write[j][2]);
                }
            }
        }
        found = false;
    }
}

#endif