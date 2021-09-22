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

/*
-------------------------------------------------------------------------------
************************************************* H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "imageheadertable-zynq.h"

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
ZynqImageHeader::ZynqImageHeader(std::string& filename)
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
ZynqImageHeader::ZynqImageHeader(std::ifstream& ifs)
    : ImageHeader(ifs)
    , imageHeader(NULL)
{
    bool firstValidHdr = false;
    
    /* Import the Image Header from a boot image file 
       Read the image header from the file to extract image name for section
       creation */
    ZynqImageHeaderStructure importedIH;
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
    imageHeader = (ZynqImageHeaderStructure*)section->Data;
    ifs.read((char*)imageHeader, size);

    /* This is a work around of a bug in the old bootgen. The partition header counter is stored in
    the wrong field. */
    long count = (importedIH.dataSectionCount == 0) ? importedIH.imageNameLength : importedIH.dataSectionCount;
    long offset = importedIH.partitionHeaderWordOffset * sizeof(uint32_t);

    for (uint8_t index = 0; index<count; index++)
    {
        PartitionHeader* hdr = new ZynqPartitionHeader(this, index);
        if (!firstValidHdr)
        {
            hdr->firstValidIndex = true;
            firstValidHdr = true;
        }
        ifs.seekg(offset);
        hdr->ReadHeader(ifs);
        hdr->ReadData(ifs);

        partitionHeaderList.push_back(hdr);

        /* This is just a default value.
        It may be true when the partition load address is compared to the bootheader */
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

            case DestinationDevice::DEST_DEV_PL:
                SetDomain(Domain::PL);
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
ZynqImageHeader::~ZynqImageHeader()
{
    if (section != NULL)
    {
        delete section;
    }
}

/******************************************************************************/
void ZynqImageHeader::ImportFpgaDataFile(BootImage& bi)
{
    ByteFile data(Filename);

    PartitionHeader* partHdr = new ZynqPartitionHeader(this, 0);
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
void ZynqImageHeader::CreateElfPartitions(BootImage& bi, ElfFormat* elf, uint8_t proc_state)
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
                        fsblFwSize = totalFsblFwSize = total_size = Reserve.Value();
                    }
                }
            }
            fsblFwSize = totalFsblFwSize = total_size;
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

        
        PartitionHeader* partHdr = new ZynqPartitionHeader(this, hdr_index);
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
void ZynqImageHeader::ImportBit(BootImage& bi)
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
    BitFile *bit = new ZynqBitFile(stream);
    bit->ParseBit(bi);
    bit->SetEncryptionType(Encrypt->Type());
    bi.bitFilename = Filename.c_str();
    PartitionHeader* hdr = new ZynqPartitionHeader(this, 0);
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

    /* Bitstream sizes should be word aligned, otherwise bitstream is invalid */
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
void ZynqImageHeader::ImportBin(BootImage& bi)
{
    SetDomain(Domain::PS);

    ByteFile data(Filename);

    PartitionHeader* hdr = new ZynqPartitionHeader(this, 0);
    hdr->firstValidIndex = true;

    hdr->execAddress = Startup.ValueOrDefault(0);
    if (Load.IsSet())
    {
        hdr->loadAddress = Load.Value();
    }
    else
    {
        hdr->loadAddress = 0;
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
Binary::Address_t ZynqImageHeader::GetNextImageHeaderOffset() 
{
    return imageHeader->nextImageHeaderWordOffset * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t ZynqImageHeader::GetPartitionHeaderOffset(void)
{
    return imageHeader->partitionHeaderWordOffset * sizeof(uint32_t);
}
 
/******************************************************************************/
uint32_t ZynqImageHeader::GetDataSectionCount(void)
{
    return imageHeader->dataSectionCount;
}

/******************************************************************************/
uint32_t ZynqImageHeader::GetImageNameLength(void)
{
    return imageHeader->imageNameLength;
}

/******************************************************************************/
void ZynqImageHeader::Build(BootImage& bi, Binary& cache)
{
    cache.Sections.push_back(section);
    
    uint32_t defaultAlignment = bi.options.GetDefaultAlignment();

    if (imageHeader == NULL)
    {
        imageHeader = (ZynqImageHeaderStructure*)section->Data;
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
            if (bi.partitionOutput->Type() == PartitionArch::FPGA)
            {
                ImportFpgaDataFile(bi);
            }
            else
            {
                ImportElf(bi);
            }
        }
        else if (StringUtils::EndsWith(comparison, ".srec"))
        {
            ImportFpgaDataFile(bi);
        }
        else if (StringUtils::EndsWith(comparison, ".bit") || StringUtils::EndsWith(comparison, ".rbt"))
        {
            ImportBit(bi);
        }
        else
        {
            ImportBin(bi);
        }

        /* Legacy operation for backward compatibility. This is a bug in the old bootgen */
        SetDataSectionCount(0);
        SetImageNameLength((uint32_t)partitionHeaderList.size());
    }
    bi.partitionHeaderList.insert(bi.partitionHeaderList.end(),
    partitionHeaderList.begin(),
    partitionHeaderList.end());
}

/******************************************************************************/
void ZynqImageHeader::Link(BootImage &bi, PartitionHeader* partitionHeader,
    ImageHeader* nextImageHeader)
{
    slaveBootSplitMode = (bi.bifOptions->GetSplitMode() == SplitMode::SlaveMode) ? true : false;
    fullBhSize = bi.options.bootheaderSize;
    allHdrSize = bi.options.allHeaderSize;

    imageHeader = (ZynqImageHeaderStructure*)section->Data;
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
ZynqImageHeaderTable::ZynqImageHeaderTable()
{
    section = new Section("ImageHeaderTable", sizeof(ZynqImageHeaderTableStructure));
    iHTable = (ZynqImageHeaderTableStructure*)section->Data;
}

/******************************************************************************/
ZynqImageHeaderTable::ZynqImageHeaderTable(std::ifstream& src)
{
    section = new Section("ImageHeaderTable", sizeof(ZynqImageHeaderTableStructure));
    iHTable = (ZynqImageHeaderTableStructure*)section->Data;
   
    /* Import the Image Header Table from a boot image file */
    src.read((char*)section->Data, section->Length);
}

/******************************************************************************/
ZynqImageHeaderTable::~ZynqImageHeaderTable()
{
    if (section != NULL)
    {
        delete section;
    }
}

/******************************************************************************/
void ZynqImageHeaderTable::SetImageHeaderTableVersion(uint32_t version)
{
    iHTable->version = version;
}

/******************************************************************************/
void ZynqImageHeaderTable::SetPartitionCount(uint32_t count)
{
    iHTable->partitionTotalCount = count;
}

/******************************************************************************/
void ZynqImageHeaderTable::SetFirstPartitionHeaderOffset(uint32_t offset)
{
    iHTable->firstPartitionHeaderWordOffset = offset;
}

/******************************************************************************/
void ZynqImageHeaderTable::SetFirstImageHeaderOffset(uint32_t offset)
{
    iHTable->firstImageHeaderWordOffset = offset;
}

/******************************************************************************/
void ZynqImageHeaderTable::SetHeaderAuthCertificateOffset(uint32_t offset)
{
    iHTable->headerAuthCertificateWordOffset = offset;
}

/******************************************************************************/
void ZynqImageHeaderTable::SetReservedFields(void)
{
    /* For Zynq Image Header Table, reserved fields are set to 0xFF */
    memset(&iHTable->reserved, 0xFF, MAX_IHT_RESERVED_ZYNQ * sizeof(uint32_t));
}

/******************************************************************************/
void ZynqImageHeaderTable::RealignSectionDataPtr(void)
{
    iHTable = (ZynqImageHeaderTableStructure*)section->Data;
}

/******************************************************************************/
uint32_t ZynqImageHeaderTable::GetImageHeaderTableVersion(void)
{
    return iHTable->version;
}

/******************************************************************************/
uint32_t ZynqImageHeaderTable::GetPartitionCount(void)
{
    return iHTable->partitionTotalCount;
}

/******************************************************************************/
uint32_t ZynqImageHeaderTable::GetFirstPartitionHeaderOffset(void)
{
    return iHTable->firstPartitionHeaderWordOffset;
}

/******************************************************************************/
uint32_t ZynqImageHeaderTable::GetFirstImageHeaderOffset(void)
{
    return iHTable->firstImageHeaderWordOffset;
}

/******************************************************************************/
uint32_t ZynqImageHeaderTable::GetHeaderAuthCertificateOffset(void)
{
    return iHTable->headerAuthCertificateWordOffset;
}

/******************************************************************************/
void ZynqImageHeader::SetNextImageHeaderOffset(uint32_t addr)
{
    imageHeader->nextImageHeaderWordOffset = addr / sizeof(uint32_t);
    if (slaveBootSplitMode && (imageHeader->nextImageHeaderWordOffset != 0))
    {
        imageHeader->nextImageHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqImageHeader::SetPartitionHeaderOffset(uint32_t addr)
{
    imageHeader->partitionHeaderWordOffset = addr / sizeof(uint32_t);
    if (slaveBootSplitMode && (imageHeader->partitionHeaderWordOffset != 0))
    {
        imageHeader->partitionHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqImageHeader::SetDataSectionCount(uint32_t cnt)
{
    imageHeader->dataSectionCount = cnt;
}

/******************************************************************************/
void ZynqImageHeader::SetImageNameLength(uint32_t len)
{
    imageHeader->imageNameLength = len;
}

/******************************************************************************/
void ZynqImageHeader::SetImageName(void)
{
    /* This is legacy ordering, the name is written with each set of 4 characters
       in reversed order */
    for (uint32_t i = 0;i<Name.size();i++)
    {
        uint32_t j = (i & ~3) + (3 - (i & 3));
        imageHeader->imageName[j] = Name[i];
    }
}

/******************************************************************************/
void ZynqImageHeader::ValidateEncryptionAttributes(BootImage& bi)
{
    if (bi.bootHeader->GetPrebuiltFlag() == false)
    {
        if (IsBootloader() && (bi.options.GetEncryptedKeySource() != KeySource::None))
        {
            LOG_ERROR("Key Source can be enabled only if bootloader is encrypted.");
        }
    }
}

/******************************************************************************/
uint64_t ZynqImageHeader::GetExecAddrForXip(uint64_t execAddr)
{
    if ((execAddr >= LQSPI_BASE_ADDR_ZYNQ) && (execAddr < LQSPI_BASE_ADDR_ZYNQ + LQSPI_SIZE_ZYNQ))
    {
        return (execAddr & LQSPI_RANGE_MASK_ZYNQ);
    }
    else if ((execAddr >= NOR_BASE_ADDR_ZYNQ) && (execAddr < NOR_BASE_ADDR_ZYNQ + NOR_SIZE_ZYNQ))
    {
        LOG_INFO("Exec Addr - 0x%x, NOR Mask - 0x%x", execAddr, NOR_RANGE_MASK_ZYNQ);
        return (execAddr - NOR_BASE_ADDR_ZYNQ);
    }
    else
    {
        LOG_DEBUG(DEBUG_STAMP, "LQSPI Range: 0x%x-0x%x, NOR Range: 0x%x-0x%x, Exec Addr: 0x%x",
                    LQSPI_BASE_ADDR_ZYNQ, LQSPI_BASE_ADDR_ZYNQ + LQSPI_SIZE_ZYNQ,
                    NOR_BASE_ADDR_ZYNQ, NOR_BASE_ADDR_ZYNQ + NOR_SIZE_ZYNQ, execAddr);
        LOG_ERROR("Execution Address of FSBL is out of Linear QSPI range in XIP mode");
    }
    return 0;
}
