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
************************************************* H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootimage.h"
#include "elftools.h"
#include "stringutils.h"
#include "fileutils.h"
#include "options.h"
#include "bifoptions.h"
#include "bitutils.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
void ImageHeaderTable::Build(BootImage& bi, Binary& cache) 
{
    if(section != NULL)
    {
        cache.Sections.push_back(section);
    }

    SetImageHeaderTableVersion(0x01020000);

    for(std::list<ImageHeader*>::iterator image = bi.imageList.begin();
        image != bi.imageList.end();  image++) 
    {
        (*image)->Build(bi,cache);
        
        /* Validate the security combinations, like checksum is not supported
           with encryption & authentication in ZynqMP */
        ValidateSecurityCombinations((*image)->GetAuthenticationType(),
                                     (*image)->GetEncryptContext()->Type(),
                                     (*image)->GetChecksumContext()->Type());

        imageHeaderList.push_back(*image);
    }

    if (bi.options.DoPadHeaderTable()) 
    {
        if ( bi.imageList.size() == 0) 
        {
            std::list<std::string> outFileNames = bi.options.GetOutputFileNames();
            if (outFileNames.size() > 0)
            {
                std::cerr << "WARNING: -padimageheader was requested, but no images were specified." << std::endl;
            }
        } 
        else 
        {
            int paddingsize = GetMaxNumOfPartitions() - bi.imageList.size();
            if (paddingsize > 0) 
            {
                bi.imageList.back()->section->Reserve = (1 + paddingsize) * 64;
            }
        }
    }
}

/******************************************************************************/
void ImageHeaderTable::Link(BootImage &bi) 
{
    /* Realigning the data ptr is necessary because Section->Data ptr gets 
       changed in the RSAAuthenticationContext::ResizeIfNecessary() function
       call */
    RealignSectionDataPtr();
    
    slaveBootSplitMode = (bi.bifOptions->GetSplitMode() == SplitMode::SlaveMode) ? true : false;
    fullBhSize = bi.options.bootheaderSize;
    allHdrSize = bi.options.allHeaderSize;

    if(bi.partitionHeaderTable->firstSection != NULL)
    {
        SetFirstPartitionHeaderOffset((uint32_t)bi.partitionHeaderTable->firstSection->WordAddress());
    }
    
    if(bi.imageList.size() != 0)
    {
        if(bi.imageList.front()->section != NULL)
        {
            uint32_t size = (bi.imageList.size() > 0) ? (uint32_t) bi.imageList.front()->section->WordAddress() : 0;
            SetFirstImageHeaderOffset(size);
        }
    }

    SetPartitionCount((uint32_t)bi.partitionHeaderList.size());
    
    uint32_t acOffset = (bi.headerAC != 0) ? (bi.headerAC->section->WordAddress()) : 0;
    SetHeaderAuthCertificateOffset(acOffset);

    for( std::list<ImageHeader*>::iterator currentImage = imageHeaderList.begin(); currentImage != imageHeaderList.end(); ) 
    {
        std::list<ImageHeader*>::iterator prevImage = currentImage;
        currentImage++;
                
        if (currentImage == imageHeaderList.end()) 
        {
            (*prevImage)->Link(bi, (*prevImage)->GetPartitionHeaderList().front(), 0);
        }
        else
        {
            (*prevImage)->Link(bi, (*prevImage)->GetPartitionHeaderList().front(), (*currentImage));
        }
    }

    SetBootDevice(bi.bifOptions->GetBootDevice());
    SetReservedFields();
    SetChecksum();
}

/******************************************************************************/
bool IsBootloaderx(ImageHeader* x)
{
    return x->IsBootloader();
}

/******************************************************************************/
ImageHeader* ImageHeaderTable::GetFSBLImageHeader()
{
    std::list<ImageHeader*>::iterator x = std::find_if(imageHeaderList.begin(),imageHeaderList.end(),IsBootloaderx);
    if (x == imageHeaderList.end()) 
    {
        return NULL;
    }
    else
    {
        return (*x);
    }
}

/******************************************************************************/
ImageHeader::ImageHeader(std::string& filename)
    : Filename(filename)
    , domain(Domain::PS)
    , Auth(0)
    , Encrypt(0)
    , Checksum(0)
    , Bootloader(false)
    , Boot(false)
    , Multiboot(false)
    , Noautostart(false)
    , Protected(false)
    , Static(false)
    , User(false)
    , PartOwner(PartitionOwner::FSBL)
    , pmuFwSize(0)
    , fsblFwSize(0)
    , totalPmuFwSize(0)
    , totalFsblFwSize(0)
    , sourceAddr(0)
    , trustzone(TrustZone::NonSecure)
    , exceptionLevel(ExceptionLevel::EL3)
    , early_handoff(false)
    , hivec(false)
    , xipMode(false)
    , destCpu(DestinationCPU::NONE)
    , destDevice(DestinationDevice::DEST_DEV_PS)
    , elfClass(ElfClass::ELFCLASS32)
    , slaveBootSplitMode(false)
    , fullBhSize(0)
    , allHdrSize(0)
    , defEncrBlockSize(0)
    , authBlock(0)
    , isUserPartitionNum(false)
    , userPartitionNum(0)
    , spkId(0x000000000)
    , spkSelect(1)
    , ppkSelect(0)
    , generateAesKeyFile(false)
    , partitionUid(0)
    , pmcdataSize(0)
    , bufferSize(0)
    , buffer(NULL)
    , totalpmcdataSize(0)
    , ihMemCpyAddr(0xFFFFFFFFFFFFFFFF)
    , ihDelayLoad(false)
    , ihDelayHandoff(false)
    , authType(Authentication::None)
    , bigEndian(false)
    , a32Mode(false)
    , updateReserveInPh(false)
    , keySrc(KeySource::None)
    , partitionType(PartitionType::RESERVED)
    , partitionRevokeId(0)
    , dpacm(DpaCM::DpaCMDisable)
    , isSlrPartition(false)
    , pufHdLoc(PufHdLoc::PUFinEFuse)
    , imageId(0)
    , uniqueId(0)
    , parentUniqueId(0)
    , functionId(0)
    , uidInfoFoundInCdo(false)
    , isIhFullPowerDomain(false)
    , isIhLowPowerDomain(false)
    , isIhPLPowerDomain(false)
    , isIhSystemPowerDomain(false)
{ }

/******************************************************************************/
ImageHeader::ImageHeader(std::ifstream& ifs) 
    : Filename("")
    , domain(Domain::PS)
    , Auth(0)
    , Encrypt(0)
    , Checksum(0)
    , Bootloader(false)
    , Boot(false)
    , Multiboot(false)
    , Noautostart(false)
    , Protected(false)
    , Static(false)
    , User(false)
    , PartOwner(PartitionOwner::FSBL)
    , pmuFwSize(0)
    , fsblFwSize(0)
    , totalPmuFwSize(0)
    , totalFsblFwSize(0)
    , sourceAddr(0)
    , trustzone(TrustZone::NonSecure)
    , exceptionLevel(ExceptionLevel::EL3)
    , early_handoff(false)
    , hivec(false)
    , xipMode(false)
    , destCpu(DestinationCPU::NONE)
    , destDevice(DestinationDevice::DEST_DEV_PS)
    , elfClass(ElfClass::ELFCLASS32)
    , slaveBootSplitMode(false)
    , fullBhSize(0)
    , allHdrSize(0)
    , defEncrBlockSize(0)
    , authBlock(0)
    , isUserPartitionNum(false)
    , userPartitionNum(0)
    , spkId(0x000000000)
    , spkSelect(1)
    , ppkSelect(0)
    , generateAesKeyFile(false)
    , partitionUid(0)
    , pmcdataSize(0)
    , bufferSize(0)
    , buffer(NULL)
    , totalpmcdataSize(0)
    , ihMemCpyAddr(0xFFFFFFFFFFFFFFFF)
    , ihDelayLoad(false)
    , ihDelayHandoff(false)
    , authType(Authentication::None)
    , bigEndian(false)
    , a32Mode(false)
    , updateReserveInPh(false)
    , keySrc(KeySource::None)
    , partitionType(PartitionType::RESERVED)
    , partitionRevokeId(0)
    , dpacm(DpaCM::DpaCMDisable)
    , isSlrPartition(false)
    , pufHdLoc(PufHdLoc::PUFinEFuse)
    , imageId(0)
    , uniqueId(0)
    , parentUniqueId(0)
    , functionId(0)
    , uidInfoFoundInCdo(false)
    , isIhFullPowerDomain(false)
    , isIhLowPowerDomain(false)
    , isIhPLPowerDomain(false)
    , isIhSystemPowerDomain(false)
{ }

/******************************************************************************/
ImageHeader::ImageHeader(uint8_t* data, uint64_t len)
    : Filename("")
    , domain(Domain::PS)
    , Auth(0)
    , Encrypt(0)
    , Checksum(0)
    , Bootloader(false)
    , Boot(false)
    , Multiboot(false)
    , Noautostart(false)
    , Protected(false)
    , Static(false)
    , User(false)
    , PartOwner(PartitionOwner::FSBL)
    , pmuFwSize(0)
    , fsblFwSize(0)
    , totalPmuFwSize(0)
    , totalFsblFwSize(0)
    , sourceAddr(0)
    , trustzone(TrustZone::NonSecure)
    , exceptionLevel(ExceptionLevel::EL3)
    , early_handoff(false)
    , hivec(false)
    , xipMode(false)
    , destCpu(DestinationCPU::NONE)
    , destDevice(DestinationDevice::DEST_DEV_PS)
    , partitionUid(0)
    , elfClass(ElfClass::ELFCLASS32)
    , slaveBootSplitMode(false)
    , fullBhSize(0)
    , allHdrSize(0)
    , authBlock(0)
    , pmcdataSize(0)
    , totalpmcdataSize(0)
    , buffer(data)
    , bufferSize(len)
    , generateAesKeyFile(false)
    , ihMemCpyAddr(0xFFFFFFFFFFFFFFFF)
    , ihDelayLoad(false)
    , ihDelayHandoff(false)
    , authType(Authentication::None)
    , isUserPartitionNum(false)
    , userPartitionNum(0)
    , spkId(0x000000000)
    , spkSelect(1)
    , ppkSelect(0)
    , defEncrBlockSize(0)
    , bigEndian(false)
    , a32Mode(false)
    , updateReserveInPh(false)
    , keySrc(KeySource::None)
    , partitionType(PartitionType::RESERVED)
    , partitionRevokeId(0)
    , dpacm(DpaCM::DpaCMDisable)
    , isSlrPartition(false)
    , pufHdLoc(PufHdLoc::PUFinEFuse)
    , imageId(0)
    , uniqueId(0)
    , parentUniqueId(0)
    , functionId(0)
    , uidInfoFoundInCdo(false)
    , isIhFullPowerDomain(false)
    , isIhLowPowerDomain(false)
    , isIhPLPowerDomain(false)
    , isIhSystemPowerDomain(false)
{
}

/******************************************************************************/
void ImageHeader::Relink()
{
    for(std::list<PartitionHeader*>::iterator i = partitionHeaderList.begin(); i!=partitionHeaderList.end();i++)
    {
        (*i)->imageHeader = this;
    }
}

/******************************************************************************/
void ImageHeader::ImportElf(BootImage& bi)
{
    uint8_t proc_state = 0;
    ByteFile data(Filename);

    /* Get the ELF Class format - 32-bit elf vs 64-bit elf */
    elfClass = GetElfClass(data.bytes);
    ElfFormat* elf = ElfFormat::GetElfFormat(elfClass, data.bytes, &proc_state);

    /* Check for no. of executable sections & non-zero size LOAD sections */
    uint8_t exec_count = 0;
    non_zero_elf_sec_count = 0;

    for (uint8_t iprog = 0; iprog < elf->programHdrEntryCount; iprog++)
    {
        if (elf->GetProgramHeaderFlags(iprog) & PF_X)
        {
            exec_count++;
        }
        if ((elf->GetProgramHeaderFileSize(iprog) > 0) && (elf->GetProgramHeaderType(iprog) == xPT_LOAD))
        {
            non_zero_elf_sec_count++;
        }
    }
    if (non_zero_elf_sec_count == 0)
    {
        LOG_ERROR("No non-empty program sections in %s", Filename.c_str());
    }
    CreateElfPartitions(bi, elf, proc_state);
}

/******************************************************************************/
uint8_t* ImageHeader::CombineElfSections(ElfFormat* elf, Binary::Length_t* p_size, Binary::Address_t* p_addr)
{
    // Variables for combining sections
    uint32_t num_hdrs = 0;
    Binary::Length_t size = 0;
    Binary::Length_t prev_end = 0;
    Binary::Length_t total_size = 0;
    Binary::Address_t addr = 0;
    Binary::Address_t offset = 0;
    uint8_t* p_data = NULL;

    /* Loop through all the program headers and populate the fields exec, load address etc. */
    for (uint8_t iprog = 0; iprog < elf->programHdrEntryCount; iprog++)
    {
        size = elf->GetProgramHeaderFileSize(iprog);
        addr = elf->GetPhysicalAddress(iprog);

        /* Only loadable sections with non-zero size are considered */
        if ((elf->GetProgramHeaderType(iprog) == xPT_LOAD) && (size > 0))
        {
            num_hdrs++;

            /* Consider load address of first section */
            if (num_hdrs == 1)
            {
                *p_addr = addr;
            }
            else
            {
                /* Calculate filler bytes and pad to combine different sections */
                Binary::Length_t filler_bytes = addr - prev_end;
                if (filler_bytes != 0)
                {
                    total_size += filler_bytes;
                    p_data = (uint8_t*)realloc(p_data, total_size);
                    memset(p_data + offset, 0, filler_bytes);
                    offset = total_size;
                }
            }
            total_size += size;
            p_data = (uint8_t*)realloc(p_data, total_size);
            memcpy(p_data + offset, elf->GetProgramHeaderData(iprog), size);
            prev_end = addr + size;
            *p_size = offset = total_size;

            /* Loop through till last entry */
            if (iprog < elf->programHdrEntryCount - 1)
            {
                continue;
            }
        }
    }
    return p_data;
}

/******************************************************************************/
uint8_t* ImageHeader::GetElfSections(ElfFormat* elf, Binary::Length_t* tsize, Binary::Address_t* addr, uint32_t iprog)
{
    /* Only loadable sections with non-size are considered */
    if ((elf->GetProgramHeaderType(iprog) == xPT_LOAD) && (elf->GetProgramHeaderFileSize(iprog) > 0))
    {
        *tsize = elf->GetProgramHeaderFileSize(iprog);
        *addr = elf->GetPhysicalAddress(iprog);
        return elf->GetProgramHeaderData(iprog);
    }
    return NULL;
}

/******************************************************************************/
void ImageHeader::SetPmuFwSizeIh(uint32_t size)
{
    pmuFwSize = size;
}

/******************************************************************************/
void ImageHeader::SetFsblFwSizeIh(uint32_t size)
{
    fsblFwSize = size;
}

/******************************************************************************/
void ImageHeader::SetTotalPmuFwSizeIh(uint32_t size)
{
    totalPmuFwSize = size;
}

/******************************************************************************/
void ImageHeader::SetTotalPmcDataSizeIh(uint32_t size)
{
    totalpmcdataSize = size;
}

/******************************************************************************/
void ImageHeader::SetPmcDataSizeIh(uint32_t size)
{
    pmcdataSize = size;
}

/******************************************************************************/
void ImageHeader::SetTotalFsblFwSizeIh(uint32_t size)
{
    if(IsBootloader())
    {
        totalFsblFwSize = size;
    }
}

/******************************************************************************/
void ImageHeader::SetTotalPmcFwSizeIh(uint32_t size)
{
    totalpmcdataSize = size;
}

/******************************************************************************/
void ImageHeader::SetFsblSourceAddrIh(Binary::Address_t addr)
{
    if(IsBootloader())
    {
        sourceAddr = addr;
    }
}

/******************************************************************************/
uint32_t ImageHeader::GetPmuFwSizeIh(void)
{
    return pmuFwSize;
}

/******************************************************************************/
uint32_t ImageHeader::GetFsblFwSizeIh(void)
{
    if(IsBootloader())
    {
        return fsblFwSize;
    }
    return 0;
}

/******************************************************************************/
uint32_t ImageHeader::GetTotalPmuFwSizeIh(void)
{
    if(IsBootloader())
    {
        return totalPmuFwSize;
    }
    return 0;
}

/******************************************************************************/
uint32_t ImageHeader::GetTotalFsblFwSizeIh(void)
{
    return totalFsblFwSize;
}

/******************************************************************************/
Binary::Address_t ImageHeader::GetFsblSourceAddrIh(void)
{
    return sourceAddr; 
}

/******************************************************************************/
std::vector<uint32_t>&  ImageHeader::GetEncrBlocksList(void)
{
    return encrBlocks;
}

/******************************************************************************/
uint32_t ImageHeader::GetDefaultEncrBlockSize(void)
{
    return defEncrBlockSize;
}

/******************************************************************************/
void ImageHeader::InsertEncrBlocksList(uint32_t blk)
{
    encrBlocks.push_back(blk);
}

/******************************************************************************/
void ImageHeader::SetDefaultEncrBlockSize(uint32_t blk)
{
    defEncrBlockSize = blk;
}

/******************************************************************************/
ElfClass::Type ImageHeader::GetElfClass(uint8_t* elfdata)
{
    return (ElfClass::Type)elfdata[EI_CLASS];
}

/******************************************************************************/
void ImageHeader::SetDestDevice(DestinationDevice::Type type)
{
    destDevice = type;
}

/******************************************************************************/
void ImageHeader::SetDestCpu(DestinationCPU::Type type)
{ 
    static bool destCpuPmuExists = false;
    /* Destination Device will be deprecated in future, so set it to
    dest_cpu = pmu */
    destCpu = type;
    if (destCpu == DestinationCPU::PMU)
    {
        destDevice = DestinationDevice::DEST_DEV_PMU;
    } 
    else if (destDevice == DestinationDevice::DEST_DEV_PMU)
    {
        destCpu = DestinationCPU::PMU;
    }
    if (destCpu == DestinationCPU::PMU && destCpuPmuExists)
    {
        LOG_ERROR("Bif attribute Error!!!'core=psm/pmu' cannot be specified on multiple partitions.");
    }
    else if (destCpu == DestinationCPU::PMU)
    {
        destCpuPmuExists = true;
    }
}

/******************************************************************************/
size_t ImageHeader::GetAuthBlock(void)
{
    return (authBlock);
}

/******************************************************************************/
uint32_t ImageHeader::GetTotalPmcFwSizeIh(void)
{
    if (IsBootloader())
    {
        return totalpmcdataSize;
    }
    return 0;
}

/******************************************************************************/
uint32_t ImageHeader::GetPmcFwSizeIh(void)
{
    return pmcdataSize;
}

/******************************************************************************/
uint32_t ImageHeader::GetImageId(void)
{
    return imageId;
}

/******************************************************************************/
uint32_t ImageHeader::GetParentUniqueId(void)
{
    return parentUniqueId;
}

/******************************************************************************/
uint32_t ImageHeader::GetUniqueId(void)
{
    return uniqueId;
}

/******************************************************************************/
uint32_t ImageHeader::GetFunctionId(void)
{
    return functionId;
}

/******************************************************************************/
bool ImageHeader::IsUidInfoFoundInCdo(void)
{
    return uidInfoFoundInCdo;
}