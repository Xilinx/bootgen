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
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootheader-zynq.h"
#include "bootimage.h"
#include "bifoptions.h"

class BootImage;

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
ZynqBootHeader::ZynqBootHeader(void)
{
    section = new Section("BootHeader", sizeof(ZynqBootHeaderStructure) + sizeof(RegisterInitTable));
    bHTable = (ZynqBootHeaderStructure*)section->Data;
}

/******************************************************************************/
ZynqBootHeader::ZynqBootHeader(std::ifstream& src)
{
    prebuilt = true;
    section = new Section("BootHeader", sizeof(ZynqBootHeaderStructure) + sizeof(RegisterInitTable));
    
    /* Import the Boot Header from a boot image file */
    if (!src.read((char*)section->Data, section->Length).good())
    {
        LOG_ERROR("Failed to read bootheader from imported image");
    }
    bHTable = (ZynqBootHeaderStructure*)section->Data;
}


/******************************************************************************/
ZynqBootHeader::~ZynqBootHeader(void)
{
    if (section != NULL)
    {
        delete section;
    }
}

/******************************************************************************/
void ZynqBootHeader::Build(BootImage& bi, Binary& cache)
{
    if (section != NULL)
    {
        cache.Sections.push_back(section);
    }

    /* If the boot header is imported from a bootimage file, no need to build */
    if (prebuilt)
    {
        return;
    }

    SetWidthDetectionWord();
    SetIdentificationWord();
    SetEncryptionKeySource(bi.options.GetEncryptedKeySource());
    SetHeaderVersion();
    SetQspiConfigWord();
    SetUdf(bi.options.bifOptions->GetUdfBhFile());
    BuildRegInitTable(bi.options);
}

/******************************************************************************/
void ZynqBootHeader::Link(BootImage& bi)
{
    if (prebuilt)
    {
        return;
    }

    ImageHeaderTable* iHT = bi.imageHeaderTable;
    ImageHeader* fsbl = iHT->GetFSBLImageHeader();

    slaveBootSplitMode = (bi.bifOptions->GetSplitMode() == SplitMode::SlaveMode) ? true : false;
    fullBhSize = bi.options.bootheaderSize;
    allHdrSize = bi.options.allHeaderSize;
    SetBootVectors(bi);

    if (fsbl == NULL)
    {
        SetBHForSinglePartitionImage();
    }
    else
    {
        PartitionHeader& bootloaderHdr(*(fsbl->GetPartitionHeaderList().front()));
        BootloaderAddressAndSizeCheck(bootloaderHdr);

        SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address);
        SetFsblLength((uint32_t)bootloaderHdr.GetPartitionSize());
        SetFsblLoadAddress((uint32_t)bootloaderHdr.GetLoadAddress());
        SetFsblExecAddress((uint32_t)bootloaderHdr.GetExecAddress());

        SetBHForXIP(bi);

        if (fsbl->IsStaticFlagSet() || bi.bifOptions->GetXipMode())
        {
            SetFsblLength(0);
            SetTotalFsblLength(0);
        }
        else
        {
            SetFsblLength(bi.GetFsblFwSize());
            SetTotalFsblLength(bi.GetTotalFsblFwSize());
        }
    }

    if (iHT->section != NULL)
    {
        if (!Binary::CheckAddress(iHT->section->Address))
        {
            LOG_ERROR("Bootgen does not support addresses larger than 32 bits. Image Header Offset is %llX", iHT->section->Address);
        }
        SetImageHeaderByteOffset((uint32_t)iHT->section->Address);
    }

    if (bi.partitionHeaderTable->firstSection != NULL)
    {
        if (!Binary::CheckAddress(bi.partitionHeaderTable->firstSection->Address))
        {
            LOG_ERROR("Bootgen does not support addresses larger than 32 bits. Partition Header Offset is %llX", bi.partitionHeaderList.front()->section->Address);
        }
        SetPartitionHeaderByteOffset((uint32_t)bi.partitionHeaderTable->firstSection->Address);
    }

    SetHeaderChecksum();
}

/******************************************************************************/
void ZynqBootHeader::SetBHForSinglePartitionImage()
{
    SetSourceOffset(0);
    SetFsblLoadAddress(0);
    SetFsblExecAddress(0);
    SetFsblLength(0);
    SetTotalFsblLength(0);
}

/******************************************************************************/
void ZynqBootHeader::SetBHForXIP(BootImage & bi)
{
    if (bi.XipMode)
    {
        SetFsblLength(0);
        SetTotalFsblLength(0);
        SetFsblLoadAddress(0);
        if (GetFsblExecAddress() < GetXipBaseAddr() + bi.options.totalHeadersSize + bi.GetTotalPmuFwSize())
        {
            LOG_ERROR("XIP Execution address is overlapping the Boot Header and/or other regions of the image", GetFsblExecAddress());
        }
        if (GetTotalFsblLength() == 0)
        {
            SetSourceOffset(GetSourceOffset());
        }
    }
}

/******************************************************************************/
void ZynqBootHeader::SetBootVectors(BootImage &bi)
{
    for (int i=0; i<MAX_BOOT_VECTORS; i++)
    {
        bHTable->bootVectors[i] = ARM_JUMP_TO_SELF_INSTRUCION;
    }
}

/******************************************************************************/
void ZynqBootHeader::SetWidthDetectionWord(void)
{
    bHTable->widthDetectionWord = WIDTH_DETECTION;
}

/******************************************************************************/
void ZynqBootHeader::SetIdentificationWord(void)
{
    bHTable->identificationWord = HEADER_ID_WORD;
}

/******************************************************************************/
void ZynqBootHeader::SetEncryptionKeySource(KeySource::Type keyType)
{
    switch (keyType)
    {
        case KeySource::BbramRedKey:
            bHTable->encryptionKeySource = BBRAM_RED_KEY;
            break;
        case KeySource::EfuseRedKey:
            bHTable->encryptionKeySource = EFUSE_RED_KEY;
            break;
        case KeySource::None:
        default:
            bHTable->encryptionKeySource = 0;
            break;
    }
}

/******************************************************************************/
void ZynqBootHeader::SetHeaderVersion(void)
{
    bHTable->headerVersion = BOOTHEADER_VERSION;
}

/******************************************************************************/
void ZynqBootHeader::SetSourceOffset(uint32_t offset)
{
    bHTable->sourceOffset = offset;
}

/******************************************************************************/
void ZynqBootHeader::SetFsblLength(uint32_t size)
{
    bHTable->fsblLength = size;
}

/******************************************************************************/
void ZynqBootHeader::SetFsblLoadAddress(uint32_t address)
{
    bHTable->fsblLoadAddress = address;
}

/******************************************************************************/
void ZynqBootHeader::SetFsblExecAddress(uint32_t address)
{
    bHTable->fsblExecAddress = address;
}

/******************************************************************************/
void ZynqBootHeader::SetTotalFsblLength(uint32_t size)
{
    bHTable->totalFsblLength = size;
}

/******************************************************************************/
void ZynqBootHeader::SetQspiConfigWord(void)
{
    bHTable->qspiConfigWord = QSPI_CONFIG_WORD;
}

/******************************************************************************/
void ZynqBootHeader::SetHeaderChecksum(void)
{
    /* Set the simple header checksum value by adding up all of the 32 bit
       values from the Width Detection Word (0x20) to the QUAD-SPI Config Word 
       (0x44) (10 words, 40 bytes) */
    bHTable->headerChecksum = ComputeWordChecksum(&bHTable->widthDetectionWord, 40);
}

/******************************************************************************/
void ZynqBootHeader::SetUdf(std::string udfFile)
{
    uint8_t bhUdfData[UDF_BH_SIZE_ZYNQ * sizeof(uint32_t)];

    memset(bhUdfData, 0, UDF_BH_SIZE_ZYNQ * 4);

    if (udfFile != "")
    {
        LoadUdfData(udfFile, bhUdfData, UDF_BH_SIZE_ZYNQ);
    }
    memcpy((uint8_t*)bHTable->udf, bhUdfData, UDF_BH_SIZE_ZYNQ * sizeof(uint32_t));
}

/******************************************************************************/
void ZynqBootHeader::SetImageHeaderByteOffset(uint32_t address)
{
    bHTable->imageHeaderByteOffset = address;
}

/******************************************************************************/
void ZynqBootHeader::SetPartitionHeaderByteOffset(uint32_t address)
{
    bHTable->partitionHeaderByteOffset = address;
}

/******************************************************************************/
uint32_t* ZynqBootHeader::GetChecksumStartPtr(void)
{
    return &bHTable->widthDetectionWord;
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetBootHeaderSize(void)
{
    return sizeof(ZynqBootHeaderStructure);
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetWidthDetectionWord(void)
{
    return bHTable->widthDetectionWord;
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetIdentificationWord(void)
{
    return bHTable->identificationWord;
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetSourceOffset(void)
{
    return bHTable->sourceOffset;
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetHeaderChecksum(void)
{
    return bHTable->headerChecksum;
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetFsblLength(void)
{
    return bHTable->fsblLength;
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetTotalFsblLength(void)
{
    return bHTable->totalFsblLength;
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetFsblExecAddress(void)
{
    return bHTable->fsblExecAddress;
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetImageHeaderByteOffset(void)
{
    return bHTable->imageHeaderByteOffset;
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetPartitionHeaderByteOffset(void)
{
    return bHTable->partitionHeaderByteOffset;
}

/******************************************************************************/
KeySource::Type ZynqBootHeader::GetEncryptionKeySource(void)
{
    switch (bHTable->encryptionKeySource)
    {
        case BBRAM_RED_KEY:
            return KeySource::BbramRedKey;
            break;
        case EFUSE_RED_KEY:
            return KeySource::EfuseRedKey;
            break;
        default:
            return KeySource::None;
            break;
    }
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetHeaderVersion(void)
{
    return bHTable->headerVersion;
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetQspiConfigWord(void)
{
    return bHTable->qspiConfigWord;
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetFsblLoadAddress(void)
{
    return bHTable->fsblLoadAddress;
}

/******************************************************************************/
uint32_t ZynqBootHeader::CalcHeaderChecksum(void* addr, size_t len)
{
    return ComputeWordChecksum(addr, len);
}

/******************************************************************************/
void ZynqBootHeader::BuildRegInitTable(Options& options)
{
    regTable.Build(options, registerInitTable());
}

/******************************************************************************/
uint32_t ZynqBootHeader::GetXipBaseAddr(void)
{
    /* LQSPI XIP */
    if ((GetFsblExecAddress() >= LQSPI_BASE_ADDR_ZYNQ) && (GetFsblExecAddress() < LQSPI_BASE_ADDR_ZYNQ + LQSPI_SIZE_ZYNQ))
    {
        return LQSPI_BASE_ADDR_ZYNQ;
    }

    /* NOR XIP */
    if ((GetFsblExecAddress() >= NOR_BASE_ADDR_ZYNQ) && (GetFsblExecAddress() < NOR_BASE_ADDR_ZYNQ + NOR_SIZE_ZYNQ))
    {
        return NOR_BASE_ADDR_ZYNQ;
    }
    return 0;
}
