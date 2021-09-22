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
#include "imageheadertable-zynqmp.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
ZynqMpImageHeader::ZynqMpImageHeader(std::string& filename) 
    : ImageHeader(filename) 
    , imageHeader(NULL)
{
    Name = StringUtils::BaseName(filename);
    
    /* Include null terminator and round up to word */
    uint32_t size = (uint32_t)Name.length() + 1;
    size = (size + 3) & ~3;
    size += 4;
    size += 16;

    section = new Section("ImageHeader " + Name, size);
    memset(section->Data, 0, size);
}

/******************************************************************************/
ZynqMpImageHeader::ZynqMpImageHeader(std::ifstream& ifs) 
    : ImageHeader(ifs)
    , imageHeader(NULL)
{
    bool firstValidHdr = false;
    ZynqMpImageHeaderStructure importedIH;

    /* Import the Image Header from a boot image file
    Read the image header from the file to extract image name for section
    creation */
    long pos = ifs.tellg();
    ifs.read((char*)&importedIH, 4 * sizeof(uint32_t));

    char buffer[4];
    char name[256];
    uint32_t i = 0;

    do
    {
        ifs.read(buffer, 4);
        name[i + 0] = buffer[3];
        name[i + 1] = buffer[2];
        name[i + 2] = buffer[1];
        name[i + 3] = buffer[0];
        i += 4;
    } while (buffer[0] && buffer[1] && buffer[2] && buffer[3] && i<sizeof(name));

    if (i >= sizeof(name))
    {
        LOG_DEBUG(DEBUG_STAMP, "Image header name too long");
        LOG_ERROR("Failure parsing imported bootimage");
    }

    Name = name;

    /* Include null terminator and round upto word */
    uint32_t size = (uint32_t)Name.length() + 1;
    size = (size + 3) & ~3;
    size += 4;
    size += 16; 

    /* Go to start and read the image header to populate other fields */
    ifs.seekg(pos);
    section = new Section("ImageHeader " + Name, size);
    imageHeader = (ZynqMpImageHeaderStructure*)section->Data;
    ifs.read((char*)imageHeader, size);

    /* This is a work around of a bug in the old bootgen. The partition header counter is stored in
    the wrong field. */
    long count = (importedIH.dataSectionCount == 0) ? importedIH.imageNameLength : importedIH.dataSectionCount;
    long offset = importedIH.partitionHeaderWordOffset * sizeof(uint32_t);

    for (uint8_t index = 0; index<count; index++)
    {
        PartitionHeader* hdr = new ZynqMpPartitionHeader(this, index);
        if (!firstValidHdr)
        {
            hdr->firstValidIndex = true;
            firstValidHdr = true;
        }
        ifs.seekg(offset);
        hdr->ReadHeader(ifs);
        hdr->ReadData(ifs);

        partitionHeaderList.push_back(hdr);

        Bootloader = false;
        Alignment = 0;
        Offset = 0;
        Reserve = 0;

        destCpu = (DestinationCPU::Type)hdr->GetDestinationCpu();
        exceptionLevel = (ExceptionLevel::Type)hdr->GetExceptionLevel();
        trustzone = (TrustZone::Type)hdr->GetTrustZone();
        early_handoff = hdr->GetEarlyHandoff();
        hivec = hdr->GetHivec();
        authBlock = hdr->GetAuthblock();

        switch (hdr->GetDestinationDevice())
        {
            case DestinationDevice::DEST_DEV_PS:
                SetDomain(Domain::PS);
                destDevice = DestinationDevice::DEST_DEV_PS;
                break;

            case DestinationDevice::DEST_DEV_PMU:
                SetDomain(Domain::PL);
                destDevice = DestinationDevice::DEST_DEV_PMU;
                break;

            case DestinationDevice::DEST_DEV_PL:
                domain = Domain::PL;
                destDevice = DestinationDevice::DEST_DEV_PL;
                break;

            case DestinationDevice::DEST_DEV_NONE:
                LOG_DEBUG(DEBUG_STAMP, "Bad destination field in imported partition header - %s", hdr->section->Name.c_str());
                LOG_ERROR("Failure parsing imported bootimage");
        }
        offset += hdr->GetPartitionHeaderSize();
    }
}

/******************************************************************************/
ZynqMpImageHeader::~ZynqMpImageHeader()
{
    if (section != NULL)
    {
        delete section;
    }
}

/******************************************************************************/
void ZynqMpImageHeader::ImportFpgaDataFile(BootImage& bi)
{
    ByteFile data(Filename);

    PartitionHeader* partHdr = new ZynqMpPartitionHeader(this, 0);
    if(updateReserveInPh == true)
    {
        if(Reserve.IsSet())
        {
            if(data.len > Reserve.Value())
            {
                LOG_WARNING("Total Partition length is more than Reserve Length. Hence reserve attribute is ignored.");
            }
            else
            {
                data.len = Reserve.Value();
            }
        }
    }

    partHdr->partition = new Partition(partHdr, data.bytes, data.len);
    partitionHeaderList.push_back(partHdr);
}

/******************************************************************************/
void ZynqMpImageHeader::CreateElfPartitions(BootImage& bi, ElfFormat* elf, uint8_t proc_state)
{
    Binary::Length_t total_size = 0;
    Binary::Address_t load_addr = 0;
    Binary::Address_t exec_addr = 0;
    uint8_t *partition_data = NULL;
    uint32_t hdr_index = 0;

    SetDomain(Domain::PS);

    for (uint8_t iprog = 0; iprog < elf->programHdrEntryCount; iprog++)
    {
        if (Bootloader)
        {
            Binary::Length_t fsbl_size = 0;
            std::string pmu_fw = bi.bifOptions->GetPmuFwImageFile();
            if (pmu_fw != "")
            {
                uint8_t* fsbl_data = CombineElfSections(elf, &fsbl_size, &load_addr);
                fsblFwSize = totalFsblFwSize = total_size = fsbl_size;
                if(updateReserveInPh == true)
                {
                    if(Reserve.IsSet())
                    {
                        if(total_size > Reserve.Value())
                        {
                            LOG_WARNING("Total Partition length is more than Reserve Length. Hence reserve attribute is ignored.");
                        }
                        else
                        {
                            fsblFwSize = totalFsblFwSize = total_size = fsbl_size = Reserve.Value();
                        }
                    }
                }
                partition_data = AttachPmuFw(fsbl_data, &total_size, pmu_fw);
                free(fsbl_data);
            }
            else
            {
                partition_data = CombineElfSections(elf, &total_size, &load_addr);
                if(updateReserveInPh == true)
                {
                    if(Reserve.IsSet())
                    {
                        if(total_size > Reserve.Value())
                        {
                            LOG_WARNING("Total Partition length is more than Reserve Length. Hence reserve attribute is ignored.");
                        }
                        else
                        {
                            fsblFwSize = totalFsblFwSize = total_size = fsbl_size = Reserve.Value();
                        }
                    }
                }
                fsblFwSize = totalFsblFwSize = total_size;
            }
            bi.SetCoreFromDestCpu(destCpu, (A53ExecState::Type)proc_state);
            
            if (bi.XipMode)
            {
                Offset.IsSet() ? SetFsblSourceAddrIh(Offset.Value()) : SetFsblSourceAddrIh(GetExecAddrForXip(load_addr));
            }
        }
        else
        {
            partition_data = GetElfSections(elf, &total_size, &load_addr, iprog);
            if(updateReserveInPh == true)
            {
                if(Reserve.IsSet() && non_zero_elf_sec_count > 1)
                {
                    LOG_WARNING("Multiple sections in elf. Hence reserve attribute is ignored.");
                }
                if(Reserve.IsSet()  && non_zero_elf_sec_count == 1)
                {
                    if(total_size > Reserve.Value())
                    {
                        LOG_WARNING("Total Partition length is more than Reserve Length. Hence reserve attribute is ignored.");
                    }
                    else
                    {
                        total_size = Reserve.Value();
                    }
                }
            }
            if (partition_data == NULL)
            {
                continue;
            }
        }

        /* Override the exec and load address, if startup & load are set through BIF attributes */
        Startup.IsSet() ? (exec_addr = Startup.Value()) : (exec_addr = elf->GetStartUpAddress());
        if (Load.IsSet())
        {
            load_addr = Load.Value();
        }


        PartitionHeader* partHdr = new ZynqMpPartitionHeader(this, hdr_index);
        (hdr_index == 0) ? (partHdr->firstValidIndex = true) : (partHdr->firstValidIndex = false);
        partHdr->elfEndianess = elf->endian;
        partHdr->execAddress = exec_addr;
        partHdr->loadAddress = load_addr;
        partHdr->execState = proc_state;

        partHdr->partition = new Partition(partHdr, partition_data, total_size);
        free(partition_data);
        // This length also includes padding size necessary for 16-byte alignment
        partHdr->partitionSize = partHdr->partition->section->Length;
        hdr_index++;
        partitionHeaderList.push_back(partHdr);

        if (Bootloader)
        {
            /* All the sections are already combined using CombineElfSections, so break*/
            break;
        }
    }
}

/******************************************************************************/
uint8_t* ZynqMpImageHeader::AttachPmuFw(uint8_t* fsbl_data, uint64_t* size, std::string pmu_fw)
{
    uint8_t* partition_data = NULL;

    ByteFile pmu_fw_data(pmu_fw);
    Binary::Length_t pmu_size = 0;
    Binary::Address_t pmu_addr = 0;
    uint8_t pmu_padding = 0;
    
    /* If PMU FW is an ELF file */
    if (StringUtils::EndsWith(pmu_fw, ".elf"))
    {
        ElfFormat32 pmu_elf(pmu_fw_data.bytes);
        partition_data = CombineElfSections((ElfFormat*)&pmu_elf, &pmu_size, &pmu_addr);
    }
    else
    {
        pmu_size = pmu_fw_data.len;
        partition_data = (uint8_t*)malloc(pmu_size);
        memcpy(partition_data, pmu_fw_data.bytes, pmu_size);
    }

    pmu_padding = (4 - (pmu_size & 3)) & 3;
    pmuFwSize = totalPmuFwSize = pmu_size + pmu_padding;

    
    partition_data = (uint8_t*)realloc(partition_data, pmuFwSize + *size);
    memset(partition_data + pmu_size, 0, pmu_padding);
    memcpy(partition_data + pmuFwSize, fsbl_data, *size);
    *size += pmuFwSize;

    return partition_data;
}

/******************************************************************************/
void ZynqMpImageHeader::ImportBit(BootImage& bi)
{
    if (Bootloader)
    {
        LOG_ERROR("Bit stream can not be marked [bootloader]");
    }

    SetDomain(Domain::PL);
    SetDestDevice(DestinationDevice::DEST_DEV_PL);

    std::ifstream stream(Filename.c_str(), std::ios_base::binary);
    if (!stream)
    {
        LOG_ERROR("Cannot read BIT file - %s ", Filename.c_str());
    }
    BitFile *bit = new ZynqMpBitFile(stream);
    bit->ParseBit(bi);
    bit->SetEncryptionType(Encrypt->Type());
    bi.bitFilename = Filename.c_str();
    PartitionHeader* hdr = new ZynqMpPartitionHeader(this, 0);
    hdr->firstValidIndex = true;

    /* The endianess is different (Big Endian) in case of Zynq MP/FPGA
       encryption cases. All other cases the bitstream is copied as
       Little Endian */
    OutputStream *os = bit->GetOutputStreamType();

    /* If the Zynq/FPGA bitstream partition is encrypted, then we need
       to strip the normal BIT header (sync data, etc) before encryption.
       All other cases, just copy the entire bitstream as is */
    if (bit->GetBitStripFlag())
    {
        bit->Strip(os);
    }
    else
    {
        bit->Copy(os);
    }

    /* Bitstream sizes should be word aligned.
       Otherwise bitstream is invalid */
    if (os->Size() % 4)
    {
        LOG_DEBUG(DEBUG_STAMP, "BIT stream %s does not have a word aligned number of bytes (0xX)", Name.c_str(), (uint32_t)os->Size());
        LOG_ERROR("BIT file parsing error !!!");
    }

    /* As bitstreams are configured in the PL section
       There should be no load / start-up (execution) addresses in case of
       bitstreams. */
    if (this->Startup.IsSet())
    {
        LOG_ERROR("[startup=...] attribute not supported for BIT partition - %s", this->Name.c_str());
    }
 
    hdr->execAddress = 0;
    hdr->SetBitLoadAddress(this->Load.IsSet(), this->Load.Value());

    hdr->partitionSize = os->Size();

    if (bit->GetBitPadFlag(bi.options.GetLegacyFlag()))
    {
        /* Pad the bit stream with NOPS to get a chunk useful for DMA
        Bit streams must have the length 32 byte aligned because of a HW bug in the PCAP DMA engine.
        This increases the os to a multiple of 32 bytes */
        while (os->Size() % 32)
        {
            os->WriteLong(BITSTREAM_NOP);
        }
        hdr->partitionSize = os->Size();
    }

    hdr->transferSize = os->Size();
    hdr->preservedBitstreamHdr = os->pHdr;
    hdr->partition = new Partition(hdr, os->Start(), os->Size());
    if(updateReserveInPh == true)
    {
        if(Reserve.IsSet())
        {
            if(hdr->transferSize > Reserve.Value())
            {
                LOG_WARNING("Total Partition length is more than Reserve Length. Hence reserve attribute is ignored.");
            }
            else
            {
                hdr->transferSize = Reserve.Value();
                hdr->partitionSize = Reserve.Value();
                hdr->partition = new Partition(hdr, os->Start(),Reserve.Value());
            }
        }
    }
    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
void ZynqMpImageHeader::ImportBin(BootImage& bi)
{
    SetDomain(Domain::PS);
    ByteFile data(Filename);

    PartitionHeader* hdr = new ZynqMpPartitionHeader(this, 0);
    hdr->firstValidIndex = true;

    hdr->execAddress = Startup.ValueOrDefault(0);
    hdr->loadAddress = 0;
    if (Load.IsSet())
    {
        hdr->loadAddress = Load.Value();
    }
    
    if (bigEndian)
    {
        hdr->elfEndianess = Endianness::BigEndian;
    }

    if (a32Mode)
    {
        hdr->execState = A53ExecState::AARCH32;
    }
    if(updateReserveInPh == true)
    {
        if(Reserve.IsSet())
        {
            if(data.len > Reserve.Value())
            {
                LOG_WARNING("Total Partition length is more than Reserve Length. Hence reserve attribute is ignored.");
            }
            else
            {
                data.len = Reserve.Value();
            }
        }
    }
    hdr->partition = new Partition(hdr, data.bytes, data.len);
    hdr->partitionSize = data.len;
    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
Binary::Address_t ZynqMpImageHeader::GetNextImageHeaderOffset()
{
    return imageHeader->nextImageHeaderWordOffset * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t ZynqMpImageHeader::GetPartitionHeaderOffset(void)
{
    return imageHeader->partitionHeaderWordOffset * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t ZynqMpImageHeader::GetDataSectionCount(void)
{
    return imageHeader->dataSectionCount;
}

/******************************************************************************/
uint32_t ZynqMpImageHeader::GetImageNameLength(void)
{
    return imageHeader->imageNameLength;
}



/******************************************************************************/
void ZynqMpImageHeader::Build(BootImage& bi, Binary& cache)
{
   cache.Sections.push_back(section);

    uint32_t defaultAlignment = bi.options.GetDefaultAlignment();

    if (imageHeader == NULL)
    {
        imageHeader = (ZynqMpImageHeaderStructure*)section->Data;
        SetImageName();
        SetImageNameLength((uint32_t)Name.length());
        SetDataSectionCount(0);
        SetPartitionHeaderOffset(0);
        SetNextImageHeaderOffset(0);

        if (Alignment.IsSet() && Offset.IsSet())
        {
            LOG_ERROR("[alignment] and [offset] attributes are mutually exclusive");
        }
        if (Alignment.IsSet() && (Alignment.Value() & (defaultAlignment - 1)))
        {
            LOG_ERROR("Alignment (0x%X) is not a multiple of %d",
            Alignment.Value(), defaultAlignment);
        }
        if (Reserve.IsSet() && (Reserve.Value()  & (defaultAlignment - 1)))
        {
            LOG_ERROR("Reserve (0x%X) is not a multiple of %d",
            Reserve.Value(), defaultAlignment);
        }
        if (Offset.IsSet() && (Offset.Value() & (defaultAlignment - 1)))
        {
            LOG_ERROR("Offset (0x%X) is not a multiple of %d",
            Offset.Value(), defaultAlignment);
        }

        partitionHeaderList.clear();

        std::string comparison = Filename;
        StringUtils::ToLower(comparison);
 
        if (StringUtils::EndsWith(comparison, ".elf"))
        {
            ImportElf(bi);
        }
        else if (StringUtils::EndsWith(comparison, ".bit") || StringUtils::EndsWith(comparison, ".rbt"))
        {
            ImportBit(bi);
        }
        else
        {
            ImportBin(bi);
        }
        SetDataSectionCount(0);
        SetImageNameLength((uint32_t)partitionHeaderList.size());
    }
    bi.partitionHeaderList.insert(bi.partitionHeaderList.end(),partitionHeaderList.begin(),partitionHeaderList.end());
}

/******************************************************************************/
void ZynqMpImageHeader::Link(BootImage &bi, PartitionHeader* partitionHeader,
    ImageHeader* nextImageHeader)
{
    slaveBootSplitMode = (bi.bifOptions->GetSplitMode() == SplitMode::SlaveMode) ? true : false;
    fullBhSize = bi.options.bootheaderSize;
    allHdrSize = bi.options.allHeaderSize;

    imageHeader = (ZynqMpImageHeaderStructure*)section->Data;
    if (partitionHeader->section != NULL)
    {
        SetPartitionHeaderOffset((uint32_t)partitionHeader->section->Address);
    }

    if (nextImageHeader)
    {
        SetNextImageHeaderOffset((uint32_t)(nextImageHeader->section->Address));
    }
    else
    {
        SetNextImageHeaderOffset(0);
    }
}

/******************************************************************************/
ZynqMpImageHeaderTable::ZynqMpImageHeaderTable()
{
    section = new Section("ImageHeaderTable", sizeof(ZynqMpImageHeaderTableStructure));
    iHTable = (ZynqMpImageHeaderTableStructure*)section->Data;
}

/******************************************************************************/
ZynqMpImageHeaderTable::ZynqMpImageHeaderTable(std::ifstream& src)
{
    section = new Section("ImageHeaderTable", sizeof(ZynqMpImageHeaderTableStructure));
    iHTable = (ZynqMpImageHeaderTableStructure*)section->Data;
    
    /* Import the Image Header Table from a boot image file */
    src.read((char*)section->Data, section->Length);
}

/******************************************************************************/
ZynqMpImageHeaderTable::~ZynqMpImageHeaderTable()
{
    if (section != NULL)
    {
        delete section;
    }
}

/******************************************************************************/
void ZynqMpImageHeaderTable::SetImageHeaderTableVersion(uint32_t version)
{
    iHTable->version = version;
}

/******************************************************************************/
void ZynqMpImageHeaderTable::SetPartitionCount(uint32_t count)
{
    iHTable->partitionTotalCount = count;
}

/******************************************************************************/
void ZynqMpImageHeaderTable::SetFirstPartitionHeaderOffset(uint32_t offset)
{
    iHTable->firstPartitionHeaderWordOffset = offset;
    if (slaveBootSplitMode && (iHTable->firstPartitionHeaderWordOffset != 0))
    {
        iHTable->firstPartitionHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqMpImageHeaderTable::SetFirstImageHeaderOffset(uint32_t offset)
{
    iHTable->firstImageHeaderWordOffset = offset;
    if (slaveBootSplitMode && (iHTable->firstImageHeaderWordOffset != 0))
    {
        iHTable->firstImageHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqMpImageHeaderTable::SetHeaderAuthCertificateOffset(uint32_t offset)
{
    iHTable->headerAuthCertificateWordOffset = offset;
    if (slaveBootSplitMode && (iHTable->headerAuthCertificateWordOffset != 0))
    {
        iHTable->headerAuthCertificateWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqMpImageHeaderTable::SetBootDevice(BootDevice::Type type)
{
    iHTable->bootDevice = type;
}

/******************************************************************************/
void ZynqMpImageHeaderTable::SetReservedFields(void)
{
    memset(&iHTable->reserved, 0x00, MAX_IHT_RESERVED_ZYNQMP * sizeof(uint32_t));
}

/******************************************************************************/
void ZynqMpImageHeaderTable::SetChecksum(void)
{
    iHTable->ihtChecksum = ComputeWordChecksum(iHTable,
        sizeof(ZynqMpImageHeaderTableStructure) - sizeof(iHTable->ihtChecksum) /* bytes*/);
}

/******************************************************************************/
void ZynqMpImageHeaderTable::RealignSectionDataPtr(void)
{
    iHTable = (ZynqMpImageHeaderTableStructure*)section->Data;
}

/******************************************************************************/
uint32_t ZynqMpImageHeaderTable::GetImageHeaderTableVersion(void)
{
    return iHTable->version;
}

/******************************************************************************/
uint32_t ZynqMpImageHeaderTable::GetPartitionCount(void)
{
    return iHTable->partitionTotalCount;
}

/******************************************************************************/
uint32_t ZynqMpImageHeaderTable::GetFirstPartitionHeaderOffset(void)
{
    return iHTable->firstPartitionHeaderWordOffset;
}

/******************************************************************************/
uint32_t ZynqMpImageHeaderTable::GetFirstImageHeaderOffset(void)
{
    return iHTable->firstImageHeaderWordOffset;
}

/******************************************************************************/
uint32_t ZynqMpImageHeaderTable::GetHeaderAuthCertificateOffset(void)
{
    return iHTable->headerAuthCertificateWordOffset;
}

/******************************************************************************/
void ZynqMpImageHeaderTable::ValidateSecurityCombinations(Authentication::Type authType,
                            Encryption::Type encryptType, Checksum::Type cksumType)
{
    if ((authType != Authentication::None) && (cksumType != Checksum::None))
    {
        LOG_ERROR("Checksum is not supported with Authentication");
    }
    if ((encryptType != Encryption::None) && (cksumType != Checksum::None))
    {
        LOG_ERROR("Checksum is not supported with Encryption");
    }
}

/******************************************************************************/
void ZynqMpImageHeader::SetNextImageHeaderOffset(uint32_t addr)
{
    imageHeader->nextImageHeaderWordOffset = addr / sizeof(uint32_t);
    if (slaveBootSplitMode && (imageHeader->nextImageHeaderWordOffset != 0))
    {
        imageHeader->nextImageHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqMpImageHeader::SetPartitionHeaderOffset(uint32_t addr)
{
    imageHeader->partitionHeaderWordOffset = addr / sizeof(uint32_t);
    if (slaveBootSplitMode && (imageHeader->partitionHeaderWordOffset != 0))
    {
        imageHeader->partitionHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqMpImageHeader::SetDataSectionCount(uint32_t cnt)
{
    imageHeader->dataSectionCount = cnt;
}

/******************************************************************************/
void ZynqMpImageHeader::SetImageNameLength(uint32_t len)
{
    imageHeader->imageNameLength = len;
}

/******************************************************************************/
void ZynqMpImageHeader::SetImageName(void)
{
    for (uint32_t i = 0;i<Name.size();i++)
    {
        uint32_t j = (i & ~3) + (3 - (i & 3));
        imageHeader->imageName[j] = Name[i];
    }
}

/******************************************************************************/
void ZynqMpImageHeader::SetBigEndian(bool flag)
{
    bigEndian = flag;
}

/******************************************************************************/
void ZynqMpImageHeader::SetA32ExecMode(bool flag)
{
    a32Mode = flag;
}

/******************************************************************************/
void ZynqMpImageHeader::ValidateEncryptionAttributes(BootImage& bi)
{
    if (bi.bootHeader->GetPrebuiltFlag() == false)
    {
        if (IsBootloader() && (bi.options.GetEncryptedKeySource() != KeySource::None) && (bi.bifOptions->GetAuthOnly() != AuthOnly::Enabled))
        {
            LOG_ERROR("Key Source can be enabled only if bootloader is encrypted or auth_only is used in [fsbl_config] attribute.");
        }
    }

    if (IsBootloader() && bi.bifOptions->GetOptKey() == OptKey::OptKeyinSecHdr)
    {
        LOG_ERROR("Optional key cannot be used, if bootloader is not encrypted\n          Cannot use 'opt_key' in [fsbl_config] attribute when encryption is not used");
    }
}

/******************************************************************************/
void ZynqMpImageHeader::SetAuthBlock(size_t blockSize, bool flag)
{
    if (flag)
    {
        authBlock = 0;
    }
    else
    {
        if (blockSize == 0)
        {
            authBlock = 8;
        }
        else
        {
            authBlock = blockSize;
        }
    }
}

/******************************************************************************/
uint64_t ZynqMpImageHeader::GetExecAddrForXip(uint64_t execAddr)
{
    if ((execAddr < LQSPI_BASE_ADDR_ZYNQMP) || (execAddr >(LQSPI_BASE_ADDR_ZYNQMP + LQSPI_SIZE_ZYNQMP)))
    {
        LOG_DEBUG(DEBUG_STAMP, "LQSPI Range: 0x%x-0x%x, Exec Addr: 0x%x", LQSPI_BASE_ADDR_ZYNQMP,
            LQSPI_BASE_ADDR_ZYNQMP + LQSPI_SIZE_ZYNQMP, execAddr);
        LOG_ERROR("Execution Address of FSBL is out of Linear QSPI range in XIP mode");
    }
    return (execAddr & LQSPI_RANGE_MASK_ZYNQMP);
}
