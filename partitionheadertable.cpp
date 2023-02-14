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

/*
-------------------------------------------------------------------------------
************************************************* H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootimage.h"
#include "stringutils.h"
#include <string.h>
#include <fstream>
#include "options.h"
#include "partitionheadertable-zynq.h"
#include "authentication-zynq.h"
#include "authentication-zynqmp.h"
#include "partitionheadertable-zynqmp.h"
#include "partitionheadertable-versal.h"
#include "logger.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
void PartitionHeaderTable::Build(BootImage &bi, Binary& cache)
{
    LOG_INFO("Building the Partition Header Table");
    
    for(std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++) 
    {
        (*partHdr)->Build(bi,cache);
        bi.partCount++;
        if ((*partHdr)->imageHeader->IsUserPartitionNumSet()) 
        {
            (*partHdr)->partitionNum = ((*partHdr)->imageHeader->GetUserPartitionNum()) + (*partHdr)->index;
        }
        else 
        {
            (*partHdr)->partitionNum = bi.partCount - 1;
        }
    }

    PartitionHeader* nullHeader = NULL;
    if (bi.options.archType == Arch::ZYNQMP)
    {
        nullHeader = new ZynqMpPartitionHeader(NULL, 0);
    }
    else
    {
        nullHeader = new ZynqPartitionHeader(NULL, 0);
    }

    if (nullHeader != NULL)
    {
        nullHeader->SetChecksum();
        nullHeader->Build(bi, cache);
        bi.nullPartHeaderSection = nullHeader->section;

        uint8_t maxPartitions = bi.imageHeaderTable->GetMaxNumOfPartitions();
        if (bi.options.DoPadHeaderTable())
        {
            int paddingsize = maxPartitions - bi.partitionHeaderList.size();
            if (paddingsize > 0)
            {
                nullHeader->section->Reserve = (1 + paddingsize) * 64;
            }
        }

        if (bi.partitionHeaderList.size() > 0)
        {
            firstSection = bi.partitionHeaderList.front()->section;
        }
        else
        {
            firstSection = nullHeader->section;
        }
    }

    if (bi.bifOptions->GetHeaderAC() && ! bi.options.GetLegacyFlag())
    {
        LOG_INFO("Creating Header Authentication Certificate");
        
        AuthenticationContext* biAuth = NULL;
        for (std::list<ImageHeader*>::iterator image = bi.imageList.begin(); image != bi.imageList.end(); image++)
        {
            AuthenticationContext* imageAuth = (AuthenticationContext*)(*image)->GetAuthContext();
            imageAuth->hashType = bi.GetAuthHashAlgo();
            if (((*image)->IsBootloader()) && ((*image)->GetAuthenticationType() == Authentication::None))
            {
                LOG_ERROR("Bootloader must be authenticated to authenticate rest of the partitions");
            }
            if (!biAuth && (*image)->IsBootloader())
            {
                if (bi.options.archType == Arch::ZYNQMP)
                {
                    AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
                    biAuth = (AuthenticationContext*) new ZynqMpAuthenticationContext(imageAuth);
                }
                else
                {
                    AuthenticationContext::SetAuthenticationKeyLength(RSA_2048_KEY_LENGTH);
                    biAuth = (AuthenticationContext*) new ZynqAuthenticationContext(imageAuth);
                }
                biAuth->hashType = bi.GetAuthHashAlgo();
            }
            else if (!biAuth && ((*image)->GetAuthenticationType() != Authentication::None))
            {
                if (bi.options.archType == Arch::ZYNQMP)
                {
                    AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
                    biAuth = (AuthenticationContext*) new ZynqMpAuthenticationContext(imageAuth);
                }
                else
                {
                    AuthenticationContext::SetAuthenticationKeyLength(RSA_2048_KEY_LENGTH);
                    biAuth = (AuthenticationContext*) new ZynqAuthenticationContext(imageAuth);
                }
                biAuth->hashType = bi.GetAuthHashAlgo();
            }
        }
        if(!biAuth && bi.bifOptions->GetHeaderAC())
        {
            if (bi.options.archType == Arch::ZYNQMP)
            {
                AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
                biAuth = (AuthenticationContext*) new ZynqMpAuthenticationContext(bi.currentAuthCtx);
            }
            else
            {
                AuthenticationContext::SetAuthenticationKeyLength(RSA_2048_KEY_LENGTH);
                biAuth = (AuthenticationContext*) new ZynqAuthenticationContext(bi.currentAuthCtx);
            }
            biAuth->hashType = bi.GetAuthHashAlgo();
        }
        if (bi.bifOptions->GetSPKFileName() != "")
        {
            biAuth->SetSPKeyFile(bi.bifOptions->GetSPKFileName());
            biAuth->ParseSPKeyFile(bi.bifOptions->GetSPKFileName());
        }
        if (bi.bifOptions->GetSSKFileName() != "")
        {
            biAuth->SetSSKeyFile(bi.bifOptions->GetSSKFileName());
            biAuth->ParseSSKeyFile(bi.bifOptions->GetSSKFileName());
        }
        if (bi.bifOptions->GetSPKSignFileName() != "")
        {
            biAuth->SetSPKSignatureFile(bi.bifOptions->GetSPKSignFileName());
        }
        if (bi.bifOptions->GetBHSignFileName() != "")
        {
            biAuth->SetBHSignatureFile(bi.bifOptions->GetBHSignFileName());
        }
        if (bi.bifOptions->GetPpkSelectionGlobal())
        {
            biAuth->ppkSelect = bi.bifOptions->GetPpkSelection();
        }
        if (bi.bifOptions->GetSpkSelectionGlobal())
        {
            biAuth->spkSelect = bi.bifOptions->GetSpkSelection();
        }
        if (bi.bifOptions->GetSpkIdGlobal())
        {
            biAuth->spkIdentification = bi.bifOptions->GetSpkId();
        }

        biAuth->SetPresignFile(bi.bifOptions->GetHeaderSignatureFile());
        
        /* Resize sections to guarantee size is mod 64. */
        ImageHeaderTable* iht = bi.imageHeaderTable;
        biAuth->ResizeIfNecessary(iht->section);
        for( std::list<ImageHeader*>::iterator ih = bi.imageList.begin(); ih !=bi.imageList.end(); ih++) 
        {
            biAuth->ResizeIfNecessary((*ih)->section);
        }
        for(std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
        {
            biAuth->ResizeIfNecessary((*partHdr)->section);
        }

        /* Header AC authentication */
        if (bi.options.archType == Arch::ZYNQMP)
        {
            bi.headerAC = new RSA4096AuthenticationCertificate(biAuth);
        }
        else
        {
            bi.headerAC = new RSA2048AuthenticationCertificate(biAuth);
        }

        bi.headerAC->Build(bi, cache, iht->section, false, true);
    } 
    else if (bi.options.DoPadHeaderTable()) 
    {
        if (nullHeader != NULL)
        {
            nullHeader->section->Reserve += bi.currentAuthCtx->GetCertificateSize();
        }
    }
}

/******************************************************************************/
void PartitionHeaderTable::BuildPartitions(BootImage &bi, Binary& cache)
{ 
    for(std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        (*partHdr)->partition->Build(bi, cache);
    }
}

/******************************************************************************/
void PartitionHeaderTable::Link(BootImage &bi)
{
    for(std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        (*partHdr)->Link(bi, NULL);
        (*partHdr)->SetNextPartitionHeaderOffset();
    }

    if (bi.bifOptions->GetHeaderAC() && ! bi.options.GetLegacyFlag())
    {
        bi.headerAC->Link(bi, bi.imageHeaderTable->section);
    }

    std::list<PartitionHeader *>::iterator partHdr = bi.partitionHeaderList.begin();
    for (uint32_t currHdr = 0; currHdr < bi.partitionHeaderList.size(); currHdr++, partHdr++)
    {
        uint8_t currCpu = (*partHdr)->GetDestinationCpu();
        uint64_t currStrtAddr = (*partHdr)->GetLoadAddress();
        uint64_t currEndAddr = currStrtAddr + (*partHdr)->GetPartitionSize();
        if((*partHdr)->imageHeader->IsBootloader())
        {
            currEndAddr = currStrtAddr + (*partHdr)->imageHeader->GetTotalFsblFwSizeIh();
        }
        bool currCpuR5 = (currCpu == DestinationCPU :: R5_0 || currCpu == DestinationCPU :: R5_1 || currCpu == DestinationCPU :: R5_lockstep);
        bool isCurrPartitionOnTcm = false;

        std::list<PartitionHeader *>::iterator nextPartHdr = bi.partitionHeaderList.begin();
        std::advance(nextPartHdr, currHdr + 1);
        for (uint32_t nxtHdr = currHdr + 1; nxtHdr < bi.partitionHeaderList.size(); nxtHdr++)
        {
            uint8_t nxtCpu = (*nextPartHdr)->GetDestinationCpu();
            uint64_t nxtStrtAddr = (*nextPartHdr)->GetLoadAddress();
            uint64_t nxtEndAddr = nxtStrtAddr + (*nextPartHdr)->GetPartitionSize();
            if((*nextPartHdr)->imageHeader->IsBootloader())
            {
                nxtEndAddr = nxtStrtAddr + (*nextPartHdr)->imageHeader->GetTotalFsblFwSizeIh();
            }
            bool nxtCpuR5 = (nxtCpu == DestinationCPU :: R5_0 || nxtCpu == DestinationCPU :: R5_1 || nxtCpu == DestinationCPU :: R5_lockstep);
            bool isNxtPartitionOnTcm = false;

            if (currCpuR5 !=  nxtCpuR5)
            {
                isCurrPartitionOnTcm = IsTcmMemoryRange((currCpu == DestinationCPU :: R5_lockstep) ? true:false, currStrtAddr, currEndAddr);
                isNxtPartitionOnTcm = IsTcmMemoryRange((nxtCpu == DestinationCPU :: R5_lockstep) ? true:false, nxtStrtAddr, nxtEndAddr);
                if (isCurrPartitionOnTcm || isNxtPartitionOnTcm)
                {
                    std::advance(nextPartHdr, 1);
                    continue;
                }
            }

            if ((nxtEndAddr >= currStrtAddr) && (nxtStrtAddr < currEndAddr))
            {
                LOG_WARNING("Partition %s range is overlapped with partition %s memory range", (*partHdr)->partition->section->Name.c_str(), (*nextPartHdr)->partition->section->Name.c_str());
                LOG_TRACE("Current Partition %s Start Address is %X and End Address is %X ,Next Partition %s Start Address is %X and End Address %X",(*partHdr)->partition->section->Name.c_str(), currStrtAddr, currEndAddr, (*nextPartHdr)->partition->section->Name.c_str(),nxtStrtAddr, nxtEndAddr);
            }
            std::advance(nextPartHdr, 1);
        }
    }
}

/******************************************************************************/
void PartitionHeaderTable::LinkPartitions(BootImage &bi)
{
    for(std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++) 
    {
        (*partHdr)->partition->Link(bi);
    }
}

/******************************************************************************/
PartitionHeader::PartitionHeader(ImageHeader* imageheader, int index)
    : imageHeader(imageheader)
    , index(index)
    , partition(NULL)
    , checksumSection(NULL)
    , headAlignment(0)
    , tailAlignment(0)
    , authCertPresent(0)
    , encryptFlag(0)
    , checksumType(0)
    , destDevice(0)
    , ownerType(0)
    , prealigned(false)
    , certificateRelativeByteOffset(0)
    , preencrypted(false)
    , presigned(false)
    , loadAddress(0)
    , execAddress(0)
    , partitionSize(0)
    , transferSize(0)
    , firstValidIndex(false)
    , execState(A53ExecState::AARCH64)
    , trustzone(TrustZone::NonSecure)
    , exceptionLevel(ExceptionLevel::EL3)
    , early_handoff(false)
    , hivec(false)
    , partitionNum(0)
    , slaveBootSplitMode(false)
    , isBootloader(false)
    , destCpu(0)
    , allHdrSize(0)
    , bootloaderSize(0)
    , fullBhSize(0)
    , authBlock(0)
    , elfEndianess(Endianness::LittleEndian)
    , generateAesKeyFile(false)
    , partitionSecHdrIv(NULL)
    , firstChunkSize(0)
    , isPmcdata(false)
    , partitionType(PartitionType::RESERVED)
    , update_atf_handoff_params(false)
    , atf_handoff_params_offset(0)
{
    if(imageheader != NULL)
    {
        isBootloader = imageHeader->IsBootloader();
    }
    preservedBitstreamHdr.data = NULL;
    preservedBitstreamHdr.size = 0;
}

/******************************************************************************/
PartitionHeader::~PartitionHeader()
{
}

/******************************************************************************/
void PartitionHeader::Build(BootImage& bi, Binary& cache) 
{
    if(section != NULL)
    {
        cache.Sections.push_back(section);
    }
}

/******************************************************************************/
void PartitionHeader::Link(BootImage &bi, PartitionHeader* next_part_hdr)
{
    /* Realigning the data ptr is necessary because Section->Data ptr gets changed in the 
       RSAAuthenticationContext::ResizeIfNecessary() function call */
    slaveBootSplitMode = (bi.bifOptions->GetSplitMode() == SplitMode::SlaveMode) ? true : false;
    fullBhSize = bi.options.bootheaderSize;
    allHdrSize = bi.options.allHeaderSize;
    bootloaderSize = bi.options.bootloaderSize;
    isBootloader = partition->section->isBootloader;
    RealignSectionDataPtr();
        
    SetEncryptedPartitionLength((uint32_t)transferSize);
    SetUnencryptedPartitionLength((uint32_t)(partitionSize+3));
    SetTotalPartitionLength((uint32_t)partition->section->Length);
    SetLoadAddress(loadAddress);
    SetExecAddress((firstValidIndex) ? execAddress : 0);
    SetPartitionWordOffset((uint32_t)partition->section->Address);
    SetPartitionAttributes();
    SetSectionCount((uint32_t)((firstValidIndex) ? imageHeader->GetPartitionHeaderList().size() : 0));
    SetChecksumOffset();
    SetImageHeaderOffset((uint32_t)imageHeader->section->Address);
    SetAuthCertificateOffset();
    SetReserved();
    SetPartitionNumber(partitionNum);
    SetChecksum();
}

/****************************************************************************************************************/
bool PartitionHeaderTable::IsTcmMemoryRange(bool isR5LockStep, uint64_t strtAddr, uint64_t endAddr)
{
    bool tcm_R5 = false;
    if (isR5LockStep)
    {
        tcm_R5 = ((strtAddr > R5_BTCM_START_ADDRESS) && (strtAddr < (R5_TCM_START_ADDRESS + R5_TCM_BANK_LENGTH * 4)) && (endAddr <= (R5_TCM_START_ADDRESS + R5_TCM_BANK_LENGTH * 4)));
    }
    else
    {
        bool Tcm = (strtAddr >= R5_TCM_START_ADDRESS) && (strtAddr < (R5_TCM_START_ADDRESS + R5_TCM_BANK_LENGTH)) && (endAddr <= (R5_TCM_START_ADDRESS + R5_TCM_BANK_LENGTH));
        bool Btcm = ((strtAddr > R5_BTCM_START_ADDRESS) && (strtAddr < (R5_BTCM_START_ADDRESS + R5_TCM_BANK_LENGTH)) && (endAddr <= (R5_BTCM_START_ADDRESS + R5_TCM_BANK_LENGTH)));
        tcm_R5 = (Tcm || Btcm);
    }
    return tcm_R5;
}
