/******************************************************************************
* Copyright 2015-2021 Xilinx, Inc.
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

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdexcept>
#include <string.h>
#include "elftools.h"
#include "imageheadertable.h"
#include "bootgenexception.h"
#include "logger.h"
#include "fileutils.h"

static bool gEndianessInitialized = false;


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
Endianness::Type EndianMachine( void )
{
    static Endianness::Type gEndianMachine = Endianness::NotEndian;

    if(!gEndianessInitialized)
    {
        gEndianessInitialized = true;
        union 
        {
            uint8_t bytes[4];
            uint32_t value;
        } data;

        // pack 0xDEADBEEF as big endian
        data.bytes[ 0 ] = 0xDE;
        data.bytes[ 1 ] = 0xAD;
        data.bytes[ 2 ] = 0xBE;
        data.bytes[ 3 ] = 0xEF;

        if( data.value == 0xDEADBEEF ) 
        {
            gEndianMachine = Endianness::BigEndian;
        }
        else if (data.value == 0xEFBEADDE) 
        {
            gEndianMachine = Endianness::LittleEndian;
        }
        else 
        {
            LOG_ERROR("Cannot determine endianess of the ELF file");
        }
    }
    return gEndianMachine;
}

/******************************************************************************/
uint16_t DeendianUInt16(Endianness::Type endian, uint16_t data16)
{
    if(endian != EndianMachine())
    {
        data16 = SwapUInt16Bytes(data16);
    }
    return data16;
}

/******************************************************************************/
uint32_t DeendianUInt32(Endianness::Type endian, uint32_t data32)
{
    if(endian != EndianMachine())
    {
        data32 = SwapUInt32Bytes(data32);
    }
    return data32;
}

/******************************************************************************/
uint64_t DeendianUInt64(Endianness::Type endian, uint64_t data64)
{
    if(endian != EndianMachine())
    {
        data64 = SwapUInt64Bytes(data64);
    }
    return data64;
}

/******************************************************************************/
ElfFormat32::ElfFormat32(uint8_t* start) 
    : sectionHdrTbl(NULL)
{
    endian = (Endianness::Type) ((Elf32_Ehdr*)start)->e_ident[EI_DATA];

    DeendianELFHdr(start);

    /* Get the header's size. For completeness, tell upper layers that
       we only have one record. */
    elfHdrEntryCount = 1;

    /* Basic ELF checks */
    if(ELFHdrEntrySize() != sizeof(Elf32_Ehdr)) 
    {
        LOG_DEBUG(DEBUG_STAMP, "ELF Header size wrong - %d, actual size - %d", ELFHdrEntrySize(), sizeof(Elf64_Ehdr));
        LOG_ERROR("ELF Parsing Error !!!\n           Wrong Header Size");
    }

    if (memcmp(header.e_ident, ELFMAG, 4) != 0)
    {
        LOG_DEBUG(DEBUG_STAMP, "ELF magic identification word wrong");
        LOG_ERROR("ELF Parsing Error !!!\n          ELF magic identification word wrong");
    }

     /* If a Section header is defined, get its pointer and its record size
        count. Otherwise, default it to NULL. */
    if(header.e_shoff == 0)
    {
        sectionHdrTbl = NULL;
        sectionHdrEntrySize = 0;
        sectionHdrEntryCount = 0;
    }
    else
    {
        sectionHdrTbl = (Elf32SectionHdr_t *)(start + header.e_shoff);
        sectionHdrEntrySize = header.e_shentsize;
        sectionHdrEntryCount = header.e_shnum;

        /* Now loop through all of the Sections headers and replace indexes with
           properly endianized values. */
        for(unsigned int index=0; index<sectionHdrEntryCount; ++index)
        {
            /* Convert the header's endian type into the native endian type of the
               machine we're running on. */
            sectionHdrTbl[index].Deendian(endian);
        }

        /* Replace the in-file Sections headers, with a out-of-file copy */
        Elf32SectionHdr_t *tempsectionHdrTbl = (Elf32SectionHdr_t*) malloc(sectionHdrEntrySize * sectionHdrEntryCount);
        memmove(tempsectionHdrTbl, sectionHdrTbl, sectionHdrEntrySize * sectionHdrEntryCount);
        sectionHdrTbl = tempsectionHdrTbl;

        /* If we have a String Table Section, save a pointer to the String Table
           Section data. */
        if(header.e_shstrndx == 0)
        {
            stringTableSection = NULL;
            stringTableSectionSize = 0;
        }
        else
        {
            /* Index from the beginning of the ELF file with the offset index
               in the String Table Section. */
            stringTableSection = (StringTableSectionTbl *)(start+ sectionHdrTbl[header.e_shstrndx].sh_offset);

            stringTableSectionSize = sectionHdrTbl[header.e_shstrndx].sh_size;
        }
    }

    /* If a Program header is defined, get its pointer and its record size
       count. Otherwise, default it to NULL. */
    if(header.e_phoff == 0)
    {
        programHdrEntrySize = 0;
        programHdrEntryCount = 0;
    }
    else
    {
        programHdrEntrySize  = header.e_phentsize;
        programHdrEntryCount = header.e_phnum;

        /* Now loop through all of the Program headers and replace indexes with
           properly endianized values. */
        Elf32_Addr currentProgramPhysicalAddresses = 0;
        uint32_t countProgramPhysicalAddresses = 0;

        uint8_t* ptr = (start + header.e_phoff);
        for(unsigned int index = 0; index < programHdrEntryCount; ++index, ptr += header.e_phentsize )
        {
            Elf32ProgramHeader& prgHdr(* new Elf32ProgramHeader(endian, ptr));
            programHeaders.push_back(&prgHdr);

            /* Now, put the Program section's data in a separate buffer to keep
               separate from the in-memory ELF data. */
            prgHdr.data = new uint8_t[prgHdr.p_filesz];

            memcpy(prgHdr.data, (start + prgHdr.p_offset), prgHdr.p_filesz);

             /* Now for a silly little hack.
            
                 Each program section in a ELF file really has two address values, a
                 physical and logical addresses. Before the embedded world came along,
                 these two address were used for dynamic linking of ELF code to any
                 memory mapped address location. The physical address was a relative
                 offset from the beginning of the code module, and the logical address
                 was calculated at load time.
            
                 Along came the embedded world which has traditional ran code only in
                 real mode; i.e., not memory mapped. That means most of the time the
                 physical load address and logical execution address of a program section
                 are the same. Sometimes the physical address is at a different address
                 because the code is loaded into memory at one address, and later copied
                 to the logical for execution. Since Data2MEM is effectively a loader, we
                 (now) use the physical address for the load address.
            
                 Linkers are suppose to set both the physical and logical address even if
                 they are the same. That way, you are guaranteed to always get the correct
                 address for loading. However, some linkers set just the logical address,
                 leaving the physical address value zero (so far, Metaware and Greenhills).
                 This made Data2MEM try to always load everything to zero by mistake.
            
                 Prior to 9.2i, Data2MEM was loading from the logical address. This meant
                 that 90% of the time, loading code produced by all linkers would work
                 correctly. However, it was breaking those designs that wanted to load code
                 at a different address than it was to execute. 9.2i fixed this, but
                 inadvertently broke code produced by these other linkers (again, Metaware
                 and Greenhills).
            
                 The following hack counts the number program sections that have same physical
                 address. This is a non-sense arrangement since that would mean that every
                 program section would be loaded on top of each other. This is used as a
                 signal that the physical address is erroneous and to use the logical address
                 instead. This will work as long as nobody tries to create such a silly ELF
                 file; at which time a better hack may be needed. */

            if(index == 0)
            {
                // Capture the first physical address.
                currentProgramPhysicalAddresses = prgHdr.p_paddr;
                countProgramPhysicalAddresses   = 1;
            }
            else
            {
                // Count the remaining physical address that are the same as the first one.
                if(currentProgramPhysicalAddresses == prgHdr.p_paddr)
                {
                    countProgramPhysicalAddresses += 1;
                }
            }
        }

#if 1
        if(countProgramPhysicalAddresses == programHdrEntryCount) 
        {
            useLogicalLoadAddress = true;

            // replace the physical address values with the virtual addresses.
            for( unsigned int index = 0; index < programHdrEntryCount; ++index )
            {
                programHeaders[index]->p_paddr = programHeaders[index]->p_vaddr;
            }
        }
#else
        /* If the number of physical addresses of all the program section the same, signal
           that the physical addresses should be ignored, and logical address should be
           used instead. However, if we're doing an extended dump, don't "fix" the addresses
           so that we can see what they were actually set to. */

        if(((dumpMode & DATA_DUMP_EXTENDED ) == false) &&
            ( countProgramPhysicalAddresses == programHdrEntryCount ) )
        {

            useLogicalLoadAddress = true;

            // Loop through all of the Program headers and replace the physical address
            // values with the virtual addresses.

            for( index = 0; index < programHdrEntryCount; ++index )
                INDEXED_PROGRAM_STRUCT_ADDRESS( index )->p_paddr = INDEXED_PROGRAM_STRUCT_ADDRESS( index )->p_vaddr;

        }
#endif

        for(unsigned int index = 0; index < programHdrEntryCount; ++index)
        {
            Elf32ProgramHeader& prgHdr(*programHeaders[index]);

            /* The GNU linkers sometimes put an extraneous ELF header into the
               beginning the of some Program segments. This messes up what is
               put in memory, so get rid of those headers by changing the Program
               header info. */
            TrimUnwantedELFHeaders(prgHdr, start);

            /* Record the lowest and highest program section address for this
               ELF file. */
            if(prgHdr.p_filesz > 0)
            {
                if(prgHdr.p_paddr < lowestProgramAddress)
                {
                    lowestProgramAddress = prgHdr.p_paddr;
                }

                if( ((prgHdr.p_paddr + prgHdr.p_filesz) - 1) > highestProgramAddress )
                {
                    highestProgramAddress = (prgHdr.p_paddr + prgHdr.p_filesz - 1);
                }
            }
        }
    }
}

/******************************************************************************/
ElfFormat32::~ElfFormat32()
{
    uint32_t index;

    if(sectionHdrTbl)
    {
        free (sectionHdrTbl);
        sectionHdrTbl = 0;
    }

    for(index = 0; index < programHdrEntryCount; ++index)
    {
        if (programHeaders[index]->data) 
        {
            delete[] programHeaders[index]->data;
        }
    }
}

/******************************************************************************/
void ElfFormat32::DeendianELFHdr( uint8_t* start)
{
    Elf32_Ehdr* src = (Elf32_Ehdr*) start;

    memcpy(header.e_ident, src->e_ident, 16);
    header.e_type = DeendianUInt16(endian, src->e_type);
    header.e_machine = DeendianUInt16(endian, src->e_machine);
    header.e_version = DeendianUInt32(endian, src->e_version);
    header.e_entry = DeendianUInt32(endian, src->e_entry);
    header.e_phoff = DeendianUInt32(endian, src->e_phoff);
    header.e_shoff = DeendianUInt32(endian, src->e_shoff);
    header.e_flags = DeendianUInt32(endian, src->e_flags);
    header.e_ehsize = DeendianUInt16(endian, src->e_ehsize);
    header.e_phentsize = DeendianUInt16(endian, src->e_phentsize);
    header.e_phnum = DeendianUInt16(endian, src->e_phnum);
    header.e_shentsize = DeendianUInt16(endian, src->e_shentsize);
    header.e_shnum = DeendianUInt16(endian, src->e_shnum);
    header.e_shstrndx = DeendianUInt16(endian, src->e_shstrndx);
}

/******************************************************************************/
void ElfFormat32::TrimUnwantedELFHeaders( Elf32ProgramHeader& prgHeader, uint8_t* elfStart)
{
    /* The GNU linkers sometimes put an extraneous ELF header into the beginning the
        of some Program segments.This messes up what is put in memory, so get rid of
        those headers by changing the Program header info. */

    // Ignore any zero length program sections.
    if( prgHeader.p_filesz == 0 ) return;

    // Ignore if program section does not have a redundant copy if the ELF file header
    if( memcmp( prgHeader.data, ELFMAG, 4 ) ) return;


    Elf32SectionHdr_t foundSection;
    bool isFoundSection = false;

    /* Now loop through all of the Sections headers looking for the lowest
       addressed one the fits within the program section. */

    for(uint32_t  index = 0; index < sectionHdrEntryCount; ++index )
    {
        Elf32SectionHdr_t sectionHdr(sectionHdrTbl[index]);

        /* Does this section have bits defined by the program (i.e., the
          GNU tools), and the bit will actually occupy space in the target
          memory? */

        if( ( sectionHdr.sh_type == xSHT_PROGBITS ) && ( sectionHdr.sh_flags & xSHF_ALLOC ) )
        {
            // If this section is mapped to within this program section, then keep it
            if( ( sectionHdr.sh_addr >= prgHeader.p_vaddr ) &&
                ( sectionHdr.sh_addr + sectionHdr.sh_size <= prgHeader.p_paddr + prgHeader.p_filesz ) )
            {
                // Save section if this is the first, or it is lowest in memory order.
                if( isFoundSection == false || sectionHdr.sh_addr < foundSection.sh_addr)
                {
                    foundSection = sectionHdr;
                    isFoundSection = true;
                }
            }
        }
    }

    // Did we NOT find any sections that fit with this program section?
    if( isFoundSection == false )
    {
        /* This is a completely superfluous program section that the
           idiot compiler tools decided to add! Trim this program
           section down to nothing. */

        prgHeader.p_filesz = 0;
        prgHeader.p_memsz  = 0;
    }
    else
    {
        /* The superfluous ELF header may overlap part of the saved
           section. So, copy the contents of the saved section back
           into the program section. */

        uint32_t skipByteCount = foundSection.sh_addr - prgHeader.p_vaddr;

        //memmove( prgHeader.data, elfStart + foundSection.sh_offset, foundSection.sh_size );

        /* Trim off the beginning of the program section, up to the
           first real data byte from the saved section. */

        prgHeader.p_offset  += skipByteCount;
        prgHeader.p_paddr   += skipByteCount;
        prgHeader.p_vaddr   += skipByteCount;
        prgHeader.p_filesz  -= skipByteCount;
        prgHeader.p_memsz   -= skipByteCount;

        memmove( prgHeader.data, elfStart + prgHeader.p_offset, prgHeader.p_filesz );
    }
}

/******************************************************************************/
ElfFormat64::ElfFormat64(uint8_t* start) 
    : sectionHdrTbl(NULL)
{
    endian =  (Endianness::Type) ((Elf64_Ehdr*)start)->e_ident[EI_DATA];

    DeendianELFHdr(start);

    /* Get the header's size. For completeness, tell upper layers that
       we only have one record. */
    elfHdrEntryCount = 1;

    if( ELFHdrEntrySize() != sizeof( Elf64_Ehdr ) ) 
    {
        LOG_DEBUG(DEBUG_STAMP, "ELF Header size wrong - %d, actual size - %d", ELFHdrEntrySize(), sizeof(Elf64_Ehdr));
        LOG_ERROR("ELF Parsing Error !!!\n           Wrong Header Size");
    }

    if ( memcmp( header.e_ident, ELFMAG, 4 ) != 0 )
    {
        LOG_DEBUG(DEBUG_STAMP, "ELF magic identification word wrong");
        LOG_ERROR("ELF Parsing Error !!!\n          ELF magic identification word wrong");
    }

     /* If a Section header is defined, get its pointer and its record size
        count. Otherwise, default it to NULL. */

    if( header.e_shoff == 0 )
    {
        sectionHdrTbl        = NULL;
        sectionHdrEntrySize  = 0;
        sectionHdrEntryCount = 0;
    }
    else
    {
        sectionHdrTbl        = (Elf64SectionHdr_t *)(start + header.e_shoff );
        sectionHdrEntrySize  = header.e_shentsize;
        sectionHdrEntryCount = header.e_shnum;

        /* Now loop through all of the Sections headers and replace indexes with
           properly endianized values. */

        for( unsigned int index = 0; index < sectionHdrEntryCount; ++index )
        {
            /* Convert the header's endian type into the native endian type of the
               machine we're running on. */

            sectionHdrTbl[index].Deendian(endian);
        }

        // Replace the in-file Sections headers, with a out-of-file copy.

        Elf64SectionHdr_t *tempsectionHdrTbl = (Elf64SectionHdr_t*) malloc( sectionHdrEntrySize *sectionHdrEntryCount );
        memmove( tempsectionHdrTbl, sectionHdrTbl, sectionHdrEntrySize * sectionHdrEntryCount );
        sectionHdrTbl = tempsectionHdrTbl;

        // If we have a String Table Section, save a pointer to the String Table Section data.
        if( header.e_shstrndx == 0 )
        {
            stringTableSection     = NULL;
            stringTableSectionSize = 0;
        }
        else
        {
            /* Index from the beginning of the ELF file with the offset index
               in the String Table Section. */

            stringTableSection = (StringTableSectionTbl *)(start+ sectionHdrTbl[header.e_shstrndx].sh_offset );

            stringTableSectionSize = sectionHdrTbl[header.e_shstrndx].sh_size;
        }
    }

    /* If a Program header is defined, get its pointer and its record size
       count. Otherwise, default it to NULL. */
    if( header.e_phoff == 0 )
    {
        programHdrEntrySize  = 0;
        programHdrEntryCount = 0;

    }
    else
    {
        programHdrEntrySize  = header.e_phentsize;
        programHdrEntryCount = header.e_phnum;

        /* Now loop through all of the Program headers and replace indexes with
           properly endianized values. */

        Elf64_Addr currentProgramPhysicalAddresses = 0;
        uint64_t countProgramPhysicalAddresses = 0;

        uint8_t* ptr = (start + header.e_phoff );
        for(unsigned int index = 0; index < programHdrEntryCount; ++index, ptr += header.e_phentsize )
        {
            Elf64ProgramHeader& prgHdr(* new Elf64ProgramHeader(endian, ptr));
            programHeaders.push_back(&prgHdr);

            /* Now, put the Program section's data in a separate buffer to keep
               separate from the in-memory ELF data. */

            prgHdr.data = new uint8_t[prgHdr.p_filesz];

            memcpy( prgHdr.data,( start +  prgHdr.p_offset ),prgHdr.p_filesz);

            if( index == 0 )
            {
                // Capture the first physical address.
                currentProgramPhysicalAddresses = prgHdr.p_paddr;
                countProgramPhysicalAddresses   = 1;
            }
            else
            {
                // Count the remaining physical address that are the same as the first one.
                if( currentProgramPhysicalAddresses == prgHdr.p_paddr )
                {
                    countProgramPhysicalAddresses += 1;
                }
            }
        }

        if( countProgramPhysicalAddresses == programHdrEntryCount ) 
        {
            useLogicalLoadAddress = true;

            // replace the physical address values with the virtual addresses.
            for( unsigned int index = 0; index < programHdrEntryCount; ++index )
            {
                programHeaders[index]->p_paddr = programHeaders[index]->p_vaddr;
            }
        }

        for(unsigned int index = 0; index < programHdrEntryCount; ++index )
        {
            Elf64ProgramHeader& prgHdr(*programHeaders[index]);

            TrimUnwantedELFHeaders( prgHdr, start );

            if( prgHdr.p_filesz > 0 )
            {
                if( prgHdr.p_paddr < lowestProgramAddress )
                    lowestProgramAddress = prgHdr.p_paddr;

                if( ( ( prgHdr.p_paddr + prgHdr.p_filesz ) - 1 ) > highestProgramAddress )
                    highestProgramAddress = ( prgHdr.p_paddr + prgHdr.p_filesz - 1 );
            }
        }
    }
}

/******************************************************************************/
ElfFormat64::~ElfFormat64()
{
    uint32_t index;

    if(sectionHdrTbl) 
    {
        free (sectionHdrTbl);
        sectionHdrTbl = 0;
    }

    for( index = 0; index < programHdrEntryCount; ++index )
    {
        if (programHeaders[index]->data) 
        {
            delete[] programHeaders[index]->data;
        }
    }
}

/******************************************************************************/
void ElfFormat64::DeendianELFHdr( uint8_t* start)
{
    Elf64_Ehdr* src = (Elf64_Ehdr*) start;

    memcpy(header.e_ident, src->e_ident, 16);
    header.e_type = DeendianUInt16(endian, src->e_type);
    header.e_machine = DeendianUInt16(endian, src->e_machine);
    header.e_version = DeendianUInt32(endian, src->e_version);
    header.e_entry = DeendianUInt64(endian, src->e_entry);
    header.e_phoff = DeendianUInt64(endian, src->e_phoff);
    header.e_shoff = DeendianUInt64(endian, src->e_shoff);
    header.e_flags = DeendianUInt32(endian, src->e_flags);
    header.e_ehsize = DeendianUInt16(endian, src->e_ehsize);
    header.e_phentsize = DeendianUInt16(endian, src->e_phentsize);
    header.e_phnum = DeendianUInt16(endian, src->e_phnum);
    header.e_shentsize = DeendianUInt16(endian, src->e_shentsize);
    header.e_shnum = DeendianUInt16(endian, src->e_shnum);
    header.e_shstrndx = DeendianUInt16(endian, src->e_shstrndx);
}

/******************************************************************************/
void ElfFormat64::TrimUnwantedELFHeaders(Elf64ProgramHeader& prgHeader, uint8_t* elfStart)
{
    /* The GNU linkers sometimes put an extraneous ELF header into the beginning the 
       of some Program segments. This messes up what is put in memory, so get rid of 
       those headers by changing the Program header info. */

    // Ignore any zero length program sections.
    if( prgHeader.p_filesz == 0 ) return;

    // Ignore if program section does not have a redundant copy if the ELF file header
    if( memcmp( prgHeader.data, ELFMAG, 4 ) ) return;

    Elf64SectionHdr_t foundSection;
    bool isFoundSection = false;

    /* Now loop through all of the Sections headers looking for the lowest
       addressed one the fits within the program section. */

    for(uint32_t  index = 0; index < sectionHdrEntryCount; ++index )
    {
        Elf64SectionHdr_t sectionHdr(sectionHdrTbl[index]);

        /* Does this section have bits defined by the program (i.e., the
           GNU tools), and the bit will actually occupy space in the target
           memory? */

        if( ( sectionHdr.sh_type == xSHT_PROGBITS ) && ( sectionHdr.sh_flags & xSHF_ALLOC ) )
        {
            // If this section is mapped to within this program section, then keep it
            if( ( sectionHdr.sh_addr                          >= prgHeader.p_vaddr ) &&
                ( sectionHdr.sh_addr + sectionHdr.sh_size     <= prgHeader.p_paddr + prgHeader.p_filesz ) )
            {
                // Save section if this is the first, or it is lowest in memory order.
                if( isFoundSection == false || sectionHdr.sh_addr < foundSection.sh_addr)
                {
                    foundSection = sectionHdr;
                    isFoundSection = true;
                }
            }
        }
    }

    // Did we NOT find any sections that fit with this program section?
    if( isFoundSection == false )
    {
        /* This is a completely superfluous program section that the
           idiot compiler tools decided to add! Trim this program
           section down to nothing. */

        prgHeader.p_filesz = 0;
        prgHeader.p_memsz  = 0;
    }
    else
    {
        /* The superfluous ELF header may overlap part of the saved
           section. So, copy the contents of the saved section back
           into the program section. */

        uint64_t skipByteCount = foundSection.sh_addr - prgHeader.p_vaddr;

        /* Trim off the beginning of the program section, up to the
           first real data byte from the saved section. */

        prgHeader.p_offset  += skipByteCount;
        prgHeader.p_paddr   += skipByteCount;
        prgHeader.p_vaddr   += skipByteCount;
        prgHeader.p_filesz  -= skipByteCount;
        prgHeader.p_memsz   -= skipByteCount;

        memmove( prgHeader.data, elfStart + prgHeader.p_offset, prgHeader.p_filesz );
    }
}

/******************************************************************************/
ElfFormat* ElfFormat::GetElfFormat(ElfClass::Type elfClass, uint8_t* start, uint8_t* state)
{
    if(elfClass == ElfClass::ELFCLASS64)
    {
        *state = (A53ExecState::Type)A53ExecState::AARCH64;
        return new ElfFormat64(start);
    }
    /* ELF32 may contain either ARMv7/AArch32 or AArch64 using the ILP32 data model */
    else if (((Elf32_Ehdr*)start)->e_machine != 0x28)
    {
        *state = (A53ExecState::Type)A53ExecState::AARCH64;
        return new ElfFormat32(start);
    }
    else
    {
        *state = (A53ExecState::Type)A53ExecState::AARCH32;
        return new ElfFormat32(start);
    }
}

/******************************************************************************/
Program_p_flags ElfFormat32::GetProgramHeaderFlags(uint8_t index)
{
    return programHeaders[index]->Flags();
}

/******************************************************************************/
Program_p_type ElfFormat32::GetProgramHeaderType(uint8_t index)
{
    return programHeaders[index]->Type();
}

/******************************************************************************/
Binary::Length_t ElfFormat32::GetProgramHeaderFileSize(uint8_t index)
{
    return (uint32_t)programHeaders[index]->p_filesz;
}

/******************************************************************************/
Binary::Address_t ElfFormat32::GetStartUpAddress(void)
{
    return (Binary::Address_t)header.e_entry; 
}

/******************************************************************************/
Binary::Address_t ElfFormat32::GetPhysicalAddress(uint8_t index)
{
    return (Binary::Address_t)programHeaders[index]->p_paddr;
}

/******************************************************************************/
uint8_t* ElfFormat32::GetProgramHeaderData(uint8_t index)
{
    return programHeaders[index]->data;
}

/******************************************************************************/
Program_p_flags ElfFormat64::GetProgramHeaderFlags(uint8_t index)
{
    return programHeaders[index]->Flags();
}

/******************************************************************************/
Program_p_type ElfFormat64::GetProgramHeaderType(uint8_t index)
{
    return programHeaders[index]->Type();
}

/******************************************************************************/
Binary::Length_t ElfFormat64::GetProgramHeaderFileSize(uint8_t index)
{
    return (uint32_t)programHeaders[index]->p_filesz;
}

/******************************************************************************/
Binary::Address_t ElfFormat64::GetStartUpAddress(void)
{
    return (Binary::Address_t)header.e_entry; 
}

/******************************************************************************/
Binary::Address_t ElfFormat64::GetPhysicalAddress(uint8_t index)
{
    return (Binary::Address_t)programHeaders[index]->p_paddr;
}

/******************************************************************************/
uint8_t* ElfFormat64::GetProgramHeaderData(uint8_t index)
{
    return programHeaders[index]->data;
}
