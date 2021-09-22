/******************************************************************************
* Copyright 2015-2020 Xilinx, Inc.
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

#ifndef _BOOTGENENUM_H_
#define _BOOTGENENUM_H_

/* Encryption Key Source */
struct KeySource 
{
    typedef enum 
    {
        None,
        BbramRedKey,
        EfuseRedKey,
        EfuseBlkKey,
        BhBlkKey,
        EfuseGryKey,
        BhGryKey,
        BhKupKey,
        BbramBlkKey,
        BbramGryKey,
        UserKey0,
        UserKey1,
        UserKey2,
        UserKey3,
        UserKey4,
        UserKey5,
        UserKey6,
        UserKey7,
        EfuseUserKey0,
        EfuseUserBlkKey0,
        EfuseUserGryKey0,
        EfuseUserKey1,
        EfuseUserBlkKey1,
        EfuseUserGryKey1,
    } Type;
};

/* Elf endianness */
struct Endianness 
{
    typedef enum  
    {
        NotEndian,
        LittleEndian,
        BigEndian,
        NumEndian,
    } Type;
};

/* Authentication Selection */
struct Authentication 
{
    typedef enum 
    {
        None,
        RSA, 
        ECDSA,
        ECDSAp521,
    } Type;
};

/* Checksum Selection */
struct Checksum 
{
    typedef enum 
    {
        None,
        MD5,
        SHA2,
        SHA3,
    } Type;
};

/* Encryption Selection */
struct Encryption 
{
    typedef enum 
    {
        None,
        AES
    } Type;
};

/* Output File types */
struct File 
{
    typedef enum 
    {
        Unknown,
        MCS,
        MEM,
        BIN,
        HEX,
        BIT,
        RBT,
    } Type;
};

/* Partition Owner */
struct PartitionOwner
{
    typedef enum
    {
        FSBL,
        UBOOT, 
        Reserved1,
        Reserved2,
    } Type;
};

/* Qspi Mode for generating output files */
struct QspiMode 
{
    typedef enum 
    {
        Unknown,
        PARALLEL_LQSPI,
        PARALLEL_GQSPI,
        STACKED,
    } Type;
};

/* Secondary boot device */
struct BootDevice 
{
    typedef enum 
    {
       DEFAULT,
       QSPI32,
       QSPI24,
       NAND,
       SD0,
       SD1,
       SDLS,
       MMC,
       USB,
       ETHERNET,
       PCIE,
       SATA,
       OSPI,
       SMAP,
       SBI,
       SD0RAW,
       SD1RAW,
       SDLSRAW,
       MMCRAW,
       MMC0,
       MMC0RAW
    } Type;
};

/* Domain PS or PL */
struct Domain 
{
    typedef enum 
    {
        PS, 
        PL 
    } Type;
};

/* ELF class 32 vs 64 bit */
struct ElfClass 
{
    typedef enum 
    {
        ELFCLASSNONE, 
        ELFCLASS32,
        ELFCLASS64
    } Type;
};

/* Processor Core */
struct Core 
{
    typedef enum 
    {
        R5Single,
        A53Singlex32,
        A53Singlex64,
        R5Dual,
    } Type;
};

/* Boot Header authentication */
struct BhRsa 
{
    typedef enum 
    {
        BhRsaDisable = 0,
        BhRsaEnable = 3,
    }Type;
};

/* DPA Counter measure */
struct DpaCM
{
    typedef enum
    {
        DpaCMDisable = 0,
        DpaCMEnable = 3,
    } Type;
};

/* Authentication Hash Selection */
struct AuthHash
{
    typedef enum 
    {
        Sha3 = 0,
        Sha2 = 3,
    }Type;
};

/* PUF Helper Data location - efuse vs BH */
struct PufHdLoc
{
    typedef enum 
    {
        PUFinEFuse = 0,
        PUFinBH = 3,
    }Type;
};

/* PUF Mode - 12K vs 4K */
struct PufMode
{
    typedef enum 
    {
        PUF12K = 0,
        PUF4K = 3,
    }Type;
};

/* Optional Key */
struct OptKey
{
    typedef enum 
    {
        None = 0,
        OptKeyinSecHdr = 3,
    }Type;
};

/* Authentication only */
struct AuthOnly
{
    typedef enum 
    {
        None = 0,
        Enabled = 3,
    }Type;
};

/* Generate Authentication Keys */
struct GenAuthKeys
{
    typedef enum 
    {
        None,
        PEM,
        RSA,
        ECDSA,
        ECDSAP521,
    }Type;
};

/* Exception Level for ARMv8 cores */
struct ExceptionLevel
{
    typedef enum 
    {
        EL0,
        EL1,
        EL2,
        EL3
    }Type;
};

/* Trustzone for ARMv8 cores */
struct TrustZone
{
    typedef enum 
    {
        NonSecure,
        Secure
    }Type;
};

/* Destination Device */
struct DestinationDevice
{
    typedef enum
    {
        DEST_DEV_NONE,
        DEST_DEV_PS,
        DEST_DEV_PL,
        DEST_DEV_PMU,
        DEST_DEV_XIP,
    } Type;
};

/* A53 execution state 64bit vs 32bit */
struct A53ExecState 
{
    typedef enum 
    {
       AARCH64 = 0,
       AARCH32,
    } Type;
};

/* Destination Processor Cores */
struct DestinationCPU
{
    typedef enum 
    {
        NONE = 0,
        A53_0,
        A53_1,
        A53_2,
        A53_3,
        R5_0,
        R5_1,
        R5_lockstep,
        PMU,
        AIE
    } Type;
};

/* Architecture */
struct PartitionArch
{
    typedef enum
    {
        FPGA,
        NON_FPGA,
        VERSAL,
    } Type;
};

/* Split Mode for generating multiple output files */
struct SplitMode
{
    typedef enum
    {
        None,
        Normal,
        SlaveMode
    } Type;
};

/* SPK select */
struct SpkSelect {
    typedef enum 
    {
        SPK_eFUSE = 1,
        USER_eFUSE = 2,
    } Type;
};

/* Output Mode for generating output files */
struct OutputMode {
    typedef enum 
    {
        OUT_NORMAL,
        OUT_SPLIT_NORMAL,
        OUT_SPLIT_SLAVEBOOT,
        OUT_BITSTREAM,
    } Type;
};

/* Partition Type */
struct PartitionType
{
    typedef enum
    {
        RESERVED,
        ELF,
        CONFIG_DATA_OBJ,
        CFI,
        RAW,
        RAW_ELF,
        CFI_GSC,
        CFI_GSC_UNMASK,
        CDO,
        PMC_CDO,
        NPI,
        REG_INIT,
        BOOTLOADER,
        SLR_BOOT,
        SLR_CONFIG
    } Type;
};

/* Read image options */
struct ReadImageOption
{
    typedef enum
    {
        ALL,
        BH,
        IHT,
        IH,
        PHT,
        AC,
    } Type;
};

/* Dump options */
struct DumpOption
{
    typedef enum
    {
        NONE,
        BH,
        PLM,
        PMC_CDO,
        BOOT_FILES,
        PARTITIONS,
        SLAVE_PDIS,
    } Type;
};
#endif
