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
#include <string.h>

#include "bootimage.h"
#include "binary.h"
#include "stringutils.h"
#include "elftools.h"
#include "authentication-zynq.h"
#include "authentication-zynqmp.h"

/* TCM address for R5 */
#define R5_TCM_START_ADDRESS      0x0000
#define R5_BTCM_START_ADDRESS     0x20000
#define R5_TCM_BANK_LENGTH        0x10000
#define PMU_RAM_END_ADDRESS       0xFFDDFFFF

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
Partition::Partition(PartitionHeader* header0, Section* section0) 
: header(header0)
{
    section = section0;
    // Populate bootloader flag here in the section object - needed in StackAndAlign stage
    section->isBootloader = header->imageHeader->IsBootloader();
}

/******************************************************************************/
Partition::Partition(PartitionHeader* header0, const uint8_t* data, Binary::Length_t length) 
: header(header0)
{
    std::string partName = header->imageHeader->GetName() + StringUtils::Format(".%d",header->index);
    
    /* Pad binary data with 0s, to get 32-bit word size. 
       This is because bootgen stores partition sizes in terms of words, so partition data *MUST* be a multiple of 32-bits */
    int padding = (4 - (length & 3)) & 3;
#if 1
    padding = 0;
#endif
    Binary::Length_t totallength = length + padding;

    section = new Section(partName, totallength);
    section->index = header->index;
    section->isPartitionData = true;
    section->isBitStream = (header->imageHeader->GetDomain() == Domain::PL) ? true : false;
    section->isFirstElfSection = (header->firstValidIndex);
    section->isBootloader = header->imageHeader->IsBootloader();
    memcpy(section->Data, data, length);
    memset(section->Data+length, 0, padding);
}

/******************************************************************************/
void Partition::ResizeIfNecessary(Section* section)
{
    int padding = (16 - (section->Length & 15)) & 15;
    section->IncreaseLengthAndPadTo(section->Length + padding, 0x00);
}

/******************************************************************************/
void Partition::Build( BootImage& bi, Binary& cache)
{
    /* Push the section alloted into the Main section */
    if(section != NULL)
    {
        cache.Sections.push_back(section);
    }

    /* Get the image header from this partition header */
    ImageHeader& imageHeader(*this->header->imageHeader);

    /* Get the contexts for Authentication & Encryption */
    AuthenticationContext* currentAuthCtx = imageHeader.GetAuthContext();

    EncryptionContext* encryptCtx = imageHeader.GetEncryptContext();

    /* Set default values
       Set the address for relative placement for subsequence partitions in image
       Don't reserve anything for partition sections */
    if(!bi.XipMode)
    {
        section->Address = 0; 
        section->Reserve = 0;
    }

    /* If the partition is a bootloader & XIP mode is enabled, then address is directly
       populated based on the QSPI execution address.  */
    if(!(imageHeader.IsBootloader() && bi.XipMode))
    {
        /* The offset and reserve attributes only apply to first partition in image
           subsequent partitions in the same image are packed right after another */
        if (header->IsFirstPartitionInImage()) 
        {
            section->Address    = imageHeader.GetOffset().ValueOrDefault(0);
            section->Reserve    = imageHeader.GetReserve().ValueOrDefault(0);
        }
        else
        {
            section->continuation = true;
        }
    }
    section->Alignment = imageHeader.GetAlignment().ValueOrDefault(0);


    /* If the head and tail of the partition destination are not on a word boundary
       we will prepad the data so that DMA can more easily copy the bulk of the data
       a whole word at a time. The head and tail bytes are treated seperately */
    if (!header->prealigned) 
    {
        /* Use the starting load address to get the number of mis-align word boundary unused bytes at the beginning
           of a Partition data.
           Byte Addr        Word Bytes          headAlignment
               0           00 00 00 00                0
               1           XX 00 00 00                1
               2           XX XX 00 00                2
               3           XX XX XX 00                3                  */
        if(header->imageHeader->GetDomain() != Domain::PL)
        {
            header->headAlignment = header->loadAddress % sizeof(uint32_t);
        
            /* Use the ending load address to get the number of mis-align word boundary unused bytes at the end
               of a Partition data.
               Byte Addr       Word Bytes       tailAlignment
                   0    (1)   00 XX XX XX           3
                   1    (2)   00 00 XX XX           2
                   2    (3)   00 00 00 XX           1
                   3    (0)   00 00 00 00           0  */
            uint32_t remainder = (header->loadAddress + section->Length) % sizeof(uint32_t);
            header->tailAlignment = (remainder == 0)?0:(4-remainder);
        }
        section->PadToWordAlignment(header->headAlignment,header->tailAlignment);
    }


    /* Encryption process on the partition */
    if(header->preencrypted && encryptCtx->Type() != Encryption::None)
    {
        LOG_ERROR("Cannot reencrypt a partition that is already encrypted for %s", section->Name.c_str());
    }
    encryptCtx->Process(bi, header);

    if (bi.bootHeader->GetPrebuiltFlag() == false)
    {
        if (header->imageHeader->GetDestCpu() == DestinationCPU::R5_0 || header->imageHeader->GetDestCpu() == DestinationCPU::R5_1)
        {
            if ((header->loadAddress < (R5_TCM_START_ADDRESS + R5_TCM_BANK_LENGTH)) && (header->loadAddress + header->partition->section->Length >= (R5_TCM_START_ADDRESS + R5_TCM_BANK_LENGTH)))
            {
                LOG_ERROR("The length of %s has exceeded TCM and cannot be loaded by FSBL.\n\t   Partition Length : 0x%llx , Partition Load Address : %llX.", header->section->Name.substr(header->section->Name.find(" ") + 1).c_str(), header->partition->section->Length, header->loadAddress);
            }
            else if ((header->loadAddress > R5_BTCM_START_ADDRESS) && (header->loadAddress < (R5_BTCM_START_ADDRESS + R5_TCM_BANK_LENGTH)) && (header->loadAddress + header->partition->section->Length >= (R5_BTCM_START_ADDRESS + R5_TCM_BANK_LENGTH)))
            {
                LOG_ERROR("The length of %s has exceeded TCM and cannot be loaded by FSBL.\n\t   Partition Length : 0x%llx , Partition Load Address : %llX.", header->section->Name.substr(header->section->Name.find(" ") + 1).c_str(), header->partition->section->Length, header->loadAddress);
            }
        }
        else if (header->imageHeader->GetDestCpu() == DestinationCPU::R5_lockstep)
        {
            if ((header->loadAddress < (R5_TCM_START_ADDRESS + R5_TCM_BANK_LENGTH * 4)) && (header->loadAddress + header->partition->section->Length >= (R5_TCM_START_ADDRESS + R5_TCM_BANK_LENGTH * 4)))
            {
                LOG_ERROR("The length of %s has exceeded TCM and cannot be loaded by FSBL.\n\t   Partition Length : 0x%llx , Partition Load Address : %llX.", header->section->Name.substr(header->section->Name.find(" ") + 1).c_str(), header->partition->section->Length, header->loadAddress);
            }
        }
        else if (header->imageHeader->GetDestCpu() == DestinationCPU::PMU)
        {
            if (header->loadAddress + header->partition->section->Length >= PMU_RAM_END_ADDRESS)
            {
                LOG_ERROR("The length of %s has exceeded PMU RAM and cannot be loaded by FSBL.\n\t   Partition Length : 0x%llx, Partition Load Address : %llX.", header->section->Name.substr(header->section->Name.find(" ") + 1).c_str(), header->partition->section->Length, header->loadAddress);
            }
        }
    }
    uint32_t padLength = 0;

    /* Authentication process on the partition */
    if(header->presigned) 
    {
        // do nothing
        for (std::list<AuthenticationCertificate*>::iterator acs = header->ac.begin(); acs != header->ac.end(); acs++)
        {
            *acs = 0;
        }
    }
    else
    {
        header->transferSize = section->Length;
        
        /* Integrity is replaced by Checksum from 2016.3, to maintain backward compatibility, 
           the bootheader lengths should be populated in case of either checksum or integrity. 
           Whenever checksum is enabled for FSBL, then add the checksum size to FSBL length */
        if((imageHeader.IsBootloader() && imageHeader.GetChecksumContext()->Type() != Checksum::None))
        {
            padLength = imageHeader.GetChecksumContext()->Size();
        }
        else
        {
            if(currentAuthCtx->authAlgorithm->Type() != Authentication::None)
            {
                padLength = header->GetPartitionPadSize64bBoundary(section);
            }
            // create AC section and add it to the end of the list.
            currentAuthCtx->ResizeIfNecessary(section);
            size_t hashPartLen = header->partition->section->Length;
            size_t authblock = imageHeader.GetAuthBlock();
            if (authblock != 0)
            {
                hashPartLen = (authblock *1024 *1024);
            }
            size_t len = 0;
            while (len < (header->partition->section->Length))
            {
                len += hashPartLen;
                AuthenticationCertificate* tempacs = NULL;
                if (bi.options.archType == Arch::ZYNQ)
                {
                    tempacs = new RSA2048AuthenticationCertificate(currentAuthCtx);
                }
                else if (bi.options.archType == Arch::ZYNQMP)
                {
                    tempacs = new RSA4096AuthenticationCertificate(currentAuthCtx);
                }
                if(tempacs != NULL)
                {
                    tempacs->Build(bi, cache, header->partition->section, imageHeader.IsBootloader(), false);
                    header->ac.push_back(tempacs);
                }
            }
            currentAuthCtx->AddAuthCertSizeToTotalFSBLSize(header);
            currentAuthCtx->authBlocks = authblock;
        }
    }

    if(imageHeader.IsBootloader() == true)
    {
        bi.SetTotalFsblFwSize(imageHeader.GetTotalFsblFwSizeIh() + padLength);
        bi.SetTotalPmuFwSize(imageHeader.GetTotalPmuFwSizeIh());
        bi.SetPmuFwSize(imageHeader.GetPmuFwSizeIh());
        bi.SetFsblFwSize(imageHeader.GetFsblFwSizeIh());
        if(bi.XipMode)
        {
            // Just to make sure, not to get a negative no. as address
            if(imageHeader.GetFsblSourceAddrIh() > imageHeader.GetTotalPmuFwSizeIh())
            {
                section->Address = imageHeader.GetFsblSourceAddrIh() - imageHeader.GetTotalPmuFwSizeIh();
                bi.SetFsblSourceAddr(section->Address);
            }
        }
    }
}

/******************************************************************************/
void Partition::Link(BootImage &bi) 
{
    for (std::list<AuthenticationCertificate*>::iterator acs = header->ac.begin(); acs != header->ac.end(); acs++)
    {
        if ((*acs))
        {
            (*acs)->Link(bi, header->partition->section);
        }
    }
}
