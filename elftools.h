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

#ifndef _ELFTOOLS_H_
#define _ELFTOOLS_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdint.h>
#include <list>
#include <string>
#include <vector>
#include "bootgenenum.h"
#include "logger.h"

/* This is a "standard" header file */
#include "elf.h"        

/* Forward Class Declarations */
class Elf32SectionHdr_t;
class Elf64SectionHdr_t;

/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
typedef char* StringTableSectionTbl[];
struct DataFileSectionType;

uint32_t DeendianUInt32(Endianness::Type endianData, uint32_t data32);
uint64_t DeendianUInt64(Endianness::Type endianData, uint64_t data64);
Endianness::Type EndianMachine(void);



/*
-------------------------------------------------------------------------------
*****************************   E N U M E R A T E D   D A T A   T Y P E S   ***
-------------------------------------------------------------------------------
*/
/* p-type */
typedef enum 
{
    xPT_NULL = 0,
    xPT_LOAD = 1,
    xPT_DYNAMIC = 2,
    xPT_INTERP = 3,
    xPT_NOTE = 4,
    xPT_SHLIB = 5,
    xPT_PHDR = 6,
    xPT_NUM = 7,
    xPT_LOPROC = 0x70000000,
    xPT_HIPROC = 0x7FFFFFFF
} Program_p_type;

/* p-flags */
typedef enum
{
    xPF_R = 0x4,
    xPF_W = 0x2,
    xPF_X = 0x1,
    xPF_MASKPROC = 0xF0000000
} Program_p_flags;

/* sh_type */
typedef enum Section_sh_s
{
    xSHT_NULL = 0,
    xSHT_PROGBITS = 1,
    xSHT_SYMTAB = 2,
    xSHT_STRTAB = 3,
    xSHT_RELA = 4,
    xSHT_HASH = 5,
    xSHT_DYNAMIC = 6,
    xSHT_NOTE = 7,
    xSHT_NOBITS = 8,
    xSHT_REL = 9,
    xSHT_SHLIB = 10,
    xSHT_DYNSYM = 11,
    xSHT_NUM = 12,
    xSHT_LOSUNW = 0x6FFFFFFD,
    xSHT_SUNW_verdef = 0x6FFFFFFD,
    xSHT_SUNW_verneed = 0x6FFFFFFE,
    xSHT_SUNW_versym = 0x6FFFFFFF,
    xSHT_HISUNW = 0x6FFFFFFF,
    xSHT_LOPROC = 0x70000000,
    xSHT_HIPROC = 0x7FFFFFFF,
    xSHT_LOUSER = 0x80000000,
    xSHT_HIUSER = 0xFFFFFFFF
} Section_sh_type;

/* sh_flags */
typedef enum Section_sh_flags_s
{
    xSHF_WRITE = 0x00000001,
    xSHF_ALLOC = 0x00000002,
    xSHF_EXECINSTR = 0x00000004,
    xSHF_FLAGMASK = 0x00000007,
    xSHF_MASKPROC = 0xF0000000
} Section_sh_flags;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class Elf32ProgramHeader
{
public:
    Elf32ProgramHeader(Endianness::Type endian, uint8_t* ptr) : data(0)
    {
        uint32_t* origData = (uint32_t*) ptr;
        p_type = DeendianUInt32(endian, *origData++);
        p_offset = DeendianUInt32(endian, *origData++);
        p_vaddr = DeendianUInt32(endian, *origData++);
        p_paddr = DeendianUInt32(endian, *origData++);
        p_filesz = DeendianUInt32(endian, *origData++);
        p_memsz = DeendianUInt32(endian, *origData++);
        p_flags = DeendianUInt32(endian, *origData++);
        p_align = DeendianUInt32(endian, *origData++);
    }

    Elf32_Word p_type;          /* entry type */
    Elf32_Off p_offset;         /* file offset */
    Elf32_Addr p_vaddr;         /* virtual address */
    Elf32_Addr p_paddr;         /* physical address */
    Elf32_Word p_filesz;        /* file size */
    Elf32_Word p_memsz;         /* memory size */
    Elf32_Word p_flags;         /* entry flags */
    Elf32_Word p_align;         /* memory/file alignment */

    uint8_t* data;

    Program_p_type Type() 
    { 
        return (Program_p_type) p_type;
    }

    Program_p_flags Flags() 
    { 
        return (Program_p_flags) p_flags;
    }
};

/******************************************************************************/
class Elf64ProgramHeader
{
public:
    Elf64ProgramHeader(Endianness::Type endian, uint8_t* ptr) : data(0)
    {
        uint32_t* origData = (uint32_t*)ptr;
        p_type = DeendianUInt32(endian, *(origData++));
        p_flags = DeendianUInt32(endian, *(origData++));

        uint64_t lsb = (uint64_t)*(origData++);
        p_offset = DeendianUInt64(endian, (lsb | (uint64_t)*(origData++) << 32));

        lsb = (uint64_t)*(origData++);
        p_vaddr = DeendianUInt64(endian, (lsb | (uint64_t)*(origData++) << 32));

        lsb = (uint64_t)*(origData++);
        p_paddr = DeendianUInt64(endian, (lsb | (uint64_t)*(origData++) << 32));

        lsb = (uint64_t)*(origData++);
        p_filesz = DeendianUInt64(endian, (lsb | (uint64_t)*(origData++) << 32));

        lsb = (uint64_t)*(origData++);
        p_memsz = DeendianUInt64(endian, (lsb | (uint64_t)*(origData++) << 32));

        lsb = (uint64_t)*(origData++);
        p_align = DeendianUInt64(endian, (lsb | (uint64_t)*(origData++) << 32));
      }

    Elf64_Word p_type;          /* entry type 32-bit*/
    Elf32_Word p_flags;         /* entry flags 32-bit*/
    Elf64_Off p_offset;         /* file offset 64-bit*/
    Elf64_Addr p_vaddr;         /* virtual address 64-bit*/
    Elf64_Addr p_paddr;         /* physical address 64-bit*/
    Elf64_Xword p_filesz;       /* file size 64-bit*/
    Elf64_Xword p_memsz;        /* memory size 64-bit*/
    Elf64_Xword p_align;        /* memory/file alignment 64-bit*/

    uint8_t*        data;

    Program_p_type Type() 
    { 
        return (Program_p_type) p_type;
    }

    Program_p_flags Flags() 
    { 
        return (Program_p_flags) p_flags;
    }
};

/******************************************************************************/
class ElfFormat 
{
public:
    ElfFormat()
    : elfHdrEntryCount(0)
    , useLogicalLoadAddress(false)
    , programHdrEntrySize(0)
    , programHdrEntryCount(0)
    , sectionHdrEntrySize(0)
    , sectionHdrEntryCount(0)
    , stringTableSection(NULL)
    , stringTableSectionSize(0)
    , lowestProgramAddress(0xFFFFFFFF)
    , highestProgramAddress(0)
    , endian(Endianness::NotEndian)
    { };

    ~ElfFormat() {};
    
    static ElfFormat* GetElfFormat(ElfClass::Type elfClass, uint8_t* start, uint8_t* state);

    virtual Program_p_flags GetProgramHeaderFlags(uint8_t index) = 0;
    virtual Program_p_type GetProgramHeaderType(uint8_t index) = 0;
    virtual Binary::Length_t GetProgramHeaderFileSize(uint8_t index) = 0;
    virtual Binary::Address_t GetStartUpAddress(void) = 0;
    virtual Binary::Address_t GetPhysicalAddress(uint8_t index) = 0;
    virtual uint8_t* GetProgramHeaderData(uint8_t index) = 0;

    Endianness::Type endian;
    uint32_t elfHdrEntryCount;
    bool useLogicalLoadAddress;
    uint32_t programHdrEntrySize;
    uint32_t programHdrEntryCount;
    uint32_t sectionHdrEntrySize;
    uint32_t sectionHdrEntryCount;
    StringTableSectionTbl* stringTableSection;
    uint64_t stringTableSectionSize;
    uint64_t lowestProgramAddress;
    uint64_t highestProgramAddress;
};

/******************************************************************************/
class ElfFormat32 : public ElfFormat
{
public:
    ElfFormat32(uint8_t* start);
    ~ElfFormat32();
    
    Program_p_flags GetProgramHeaderFlags(uint8_t index);
    Program_p_type GetProgramHeaderType(uint8_t index);
    Binary::Length_t GetProgramHeaderFileSize(uint8_t index);
    Binary::Address_t GetStartUpAddress(void);
    Binary::Address_t GetPhysicalAddress(uint8_t index);
    uint8_t* GetProgramHeaderData(uint8_t index);
    uint32_t ELFHdrEntrySize() { return   header.e_ehsize;}

    std::vector<Elf32ProgramHeader*> programHeaders;
    Elf32SectionHdr_t* sectionHdrTbl;
    Elf32_Ehdr header;
private:
    void TrimUnwantedELFHeaders( Elf32ProgramHeader& prgHeader, uint8_t* elfStart );
    void DeendianELFHdr(uint8_t*);
};

/******************************************************************************/
class ElfFormat64 : public ElfFormat
{
public:
    ElfFormat64(uint8_t* start);
    ~ElfFormat64();
    
    Program_p_flags GetProgramHeaderFlags(uint8_t index);
    Program_p_type GetProgramHeaderType(uint8_t index);
    Binary::Length_t GetProgramHeaderFileSize(uint8_t index);
    Binary::Address_t GetStartUpAddress(void);
    Binary::Address_t GetPhysicalAddress(uint8_t index);
    uint8_t* GetProgramHeaderData(uint8_t index);
    uint32_t ELFHdrEntrySize() { return header.e_ehsize;}

    Elf64_Ehdr header;
    std::vector<Elf64ProgramHeader*> programHeaders;
    Elf64SectionHdr_t* sectionHdrTbl;
private:
    void TrimUnwantedELFHeaders( Elf64ProgramHeader& prgHeader, uint8_t* elfStart );
    void DeendianELFHdr(uint8_t*);
};

/******************************************************************************/
class Elf32SectionHdr_t
{
public:
    Elf32SectionHdr_t() {}
    Elf32SectionHdr_t(Elf32SectionHdr_t* secHdr)
    {
        sh_name = secHdr->sh_name;
        sh_type = secHdr->sh_type;
        sh_flags = secHdr->sh_flags;
        sh_addr = secHdr->sh_addr;
        sh_offset = secHdr->sh_offset;
        sh_size = secHdr->sh_size;
        sh_link = secHdr->sh_link;
        sh_info = secHdr->sh_info;
        sh_addralign = secHdr->sh_addralign;
        sh_entsize = secHdr->sh_entsize;
    }
    Elf32_Word sh_name;         /* section name */
    Elf32_Word sh_type;         /* SHT_... */
    Elf32_Word sh_flags;        /* SHF_... */
    Elf32_Addr sh_addr;         /* virtual address */
    Elf32_Off sh_offset;        /* file offset */
    Elf32_Word sh_size;         /* section size */
    Elf32_Word sh_link;         /* misc info */
    Elf32_Word sh_info;         /* misc info */
    Elf32_Word sh_addralign;    /* memory alignment */
    Elf32_Word sh_entsize;      /* entry size if table */

    Section_sh_type Type() 
    { 
        return (Section_sh_type)sh_type; 
    }

    void Deendian(Endianness::Type endian )
    {
        sh_name = DeendianUInt32(endian, sh_name);
        sh_type = DeendianUInt32(endian, sh_type);
        sh_flags = DeendianUInt32(endian, sh_flags);
        sh_addr = DeendianUInt32(endian, sh_addr);
        sh_offset = DeendianUInt32(endian, sh_offset);
        sh_size = DeendianUInt32(endian, sh_size);
        sh_link = DeendianUInt32(endian, sh_link);
        sh_info = DeendianUInt32(endian, sh_info);
        sh_addralign = DeendianUInt32(endian, sh_addralign);
        sh_entsize  = DeendianUInt32(endian, sh_entsize);
    }
};

/******************************************************************************/
class Elf64SectionHdr_t
{
public:
    Elf64SectionHdr_t() {}
    Elf64SectionHdr_t(Elf64SectionHdr_t* secHdr)
    {
        sh_name = secHdr->sh_name;
        sh_type = secHdr->sh_type;
        sh_flags = secHdr->sh_flags;
        sh_addr = secHdr->sh_addr;
        sh_offset = secHdr->sh_offset;
        sh_size = secHdr->sh_size;
        sh_link = secHdr->sh_link;
        sh_info = secHdr->sh_info;
        sh_addralign = secHdr->sh_addralign;
        sh_entsize = secHdr->sh_entsize;
    }
    Elf64_Word sh_name;         /* section name */
    Elf64_Word sh_type;         /* SHT_... */
    Elf64_Xword sh_flags;       /* SHF_... */
    Elf64_Addr sh_addr;         /* virtual address */
    Elf64_Off sh_offset;        /* file offset */
    Elf64_Xword sh_size;        /* section size */
    Elf64_Word sh_link;         /* misc info */
    Elf64_Word sh_info;         /* misc info */
    Elf64_Xword sh_addralign;   /* memory alignment */
    Elf64_Xword sh_entsize;     /* entry size if table */

    Section_sh_type Type() 
    { 
        return (Section_sh_type)sh_type; 
    }

    void Deendian(Endianness::Type endian )
    {
        sh_name = DeendianUInt32(endian, sh_name);
        sh_type = DeendianUInt32(endian, sh_type);
        sh_flags = DeendianUInt64(endian, sh_flags);
        sh_addr = DeendianUInt64(endian, sh_addr);
        sh_offset = DeendianUInt64(endian, sh_offset);
        sh_size = DeendianUInt64(endian, sh_size);
        sh_link = DeendianUInt32(endian, sh_link);
        sh_info = DeendianUInt32(endian, sh_info);
        sh_addralign = DeendianUInt64(endian, sh_addralign);
        sh_entsize= DeendianUInt64(endian, sh_entsize);
    }
};
#endif
