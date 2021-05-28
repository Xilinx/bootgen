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
#include "bootheader-zynqmp.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
ZynqMpBootHeader::ZynqMpBootHeader(void)
{
    pufData = new uint8_t[PUF_DATA_LENGTH];
    bhKeyData = new uint8_t[BLK_GRY_KEY_LENGTH * 4];
    ivData = new uint8_t[IV_LENGTH * 4];
    keyIvMust = false;

    memset(pufData, 0, PUF_DATA_LENGTH);
    memset(bhKeyData, 0, BLK_GRY_KEY_LENGTH * 4);
    memset(ivData, 0, IV_LENGTH * 4);

    section = new Section("BootHeader", sizeof(ZynqMpBootHeaderStructure) + sizeof(RegisterInitTable));
    bHTable = (ZynqMpBootHeaderStructure*)section->Data;
}

/******************************************************************************/
ZynqMpBootHeader::ZynqMpBootHeader(std::ifstream& src)
{
    prebuilt = true;
    pufData = new uint8_t[PUF_DATA_LENGTH];
    bhKeyData = new uint8_t[BLK_GRY_KEY_LENGTH * 4];
    ivData = new uint8_t[IV_LENGTH * 4];

    memset(pufData, 0, PUF_DATA_LENGTH);
    memset(bhKeyData, 0, BLK_GRY_KEY_LENGTH * 4);
    memset(ivData, 0, IV_LENGTH * 4);
    keyIvMust = false;

    /* Import the Boot Header from a boot image file */
    section = new Section("BootHeader", sizeof(ZynqMpBootHeaderStructure) + sizeof(RegisterInitTable));
    if (!src.read((char*)section->Data, section->Length).good())
    {
        LOG_ERROR("Failed to read bootheader from imported image");
    }
    bHTable = (ZynqMpBootHeaderStructure*)section->Data;

    /* If PUF is present in BH, increase the BH size */
    if (((bHTable->fsblAttributes >> PUF_HD_BIT_SHIFT) & PUF_HD_BIT_MASK) == 0x3)
    {
        uint32_t newBhSize = sizeof(ZynqMpBootHeaderStructure) + sizeof(RegisterInitTable) + PUF_DATA_LENGTH;
        uint8_t* newDataPtr = new uint8_t[newBhSize];
        memcpy(newDataPtr, section->Data, section->Length);
        if (!src.read((char*)newDataPtr + section->Length, newBhSize - section->Length).good())
        {
            LOG_ERROR("Failed to read bootheader from imported image");
        }

        delete[] section->Data;
        section->Data = newDataPtr;
        section->Length = newBhSize;
        bHTable = (ZynqMpBootHeaderStructure*)section->Data;
    }
}

/******************************************************************************/
ZynqMpBootHeader::~ZynqMpBootHeader(void)
{
    if (pufData != NULL)
    {
        delete[] pufData;
    }

    if (bhKeyData != NULL)
    {
        delete[] bhKeyData;
    }

    if (ivData != NULL)
    {
        delete[] ivData;
    }

    if (section != NULL)
    {
        delete[] section;
    }
}

/******************************************************************************/
void ZynqMpBootHeader::Build(BootImage& bi, Binary& cache)
{
    ResizeSection(bi);
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
    SetShutterValue(bi.bifOptions->GetShutterValue());
    SetUdf(bi.options.bifOptions->GetUdfBhFile());
    SetGreyOrBlackKey(bi.bifOptions->GetBhKeyFile());
    SetGreyOrBlackKekIV(bi.bifOptions->GetBHKekIVFile());
    SetSecureHdrIv(bi.options.secHdrIv);
    BuildRegInitTable(bi.options);
    SetPufData(bi);
}

/******************************************************************************/
void ZynqMpBootHeader::Link(BootImage& bi)
{
    if (prebuilt)
    {
        AddAcSizeToTotalFsblSize(bi);
        return;
    }

    ImageHeaderTable* iHT = bi.imageHeaderTable;
    ImageHeader* fsbl = iHT->GetFSBLImageHeader();

    slaveBootSplitMode = (bi.bifOptions->GetSplitMode() == SplitMode::SlaveMode) ? true : false;
    fullBhSize = bi.options.bootheaderSize;
    allHdrSize = bi.options.allHeaderSize;
    SetFsblAttributes(bi);
    SetBootVectors(bi);

    if (fsbl == NULL)
    {
        SetBHForSinglePartitionImage(bi);
    }
    else
    {
        PartitionHeader& bootloaderHdr(*(fsbl->GetPartitionHeaderList().front()));
        BootloaderAddressAndSizeCheck(bootloaderHdr);
        SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address);

        SetFsblLength((uint32_t)bootloaderHdr.GetPartitionSize());
        SetFsblExecAddress((uint32_t)bootloaderHdr.GetExecAddress());

        SetBHForXIP(bi);
        
        SetPmuFwLength(bi.GetPmuFwSize());
        SetTotalPmuFwLength(bi.GetTotalPmuFwSize());
        SetSecureHdrIv(bi.options.secHdrIv);

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
void ZynqMpBootHeader::ResizeSection(BootImage &bi)
{
    /* If PUF is present in BH, increase the BH size */
    if (bi.bifOptions->GetPufHdLoc() == PufHdLoc::PUFinBH)
    {
        uint32_t newBhSize = sizeof(ZynqMpBootHeaderStructure) + sizeof(RegisterInitTable) + PUF_DATA_LENGTH;
        section->IncreaseLengthAndPadTo(newBhSize, 0);
    }
    bHTable = (ZynqMpBootHeaderStructure*)section->Data;
}

/******************************************************************************/
void ZynqMpBootHeader::SetBootVectors(BootImage &bi)
{
    std::vector<uint32_t> bootvectors = bi.bifOptions->GetBootVectorArray();
    uint32_t core = bi.GetCore();

    if (bootvectors.size() == 0)
    {
        for (int i = 0; i < MAX_BOOT_VECTORS; i++)
        {
            bHTable->bootVectors[i] = ((core == Core::A53Singlex64) ? ARM_JUMP_TO_SELF_INSTRUCION_A53_64 : ARM_JUMP_TO_SELF_INSTRUCION);
        }
    }
    else 
    {
        for (uint32_t i = 0; i < MAX_BOOT_VECTORS; i++)
        {
            bHTable->bootVectors[i] = ((i < bootvectors.size()) ? bootvectors[i] : (((core == Core::A53Singlex64) ? ARM_JUMP_TO_SELF_INSTRUCION_A53_64 : ARM_JUMP_TO_SELF_INSTRUCION)));
        }
    }
}

/******************************************************************************/
void ZynqMpBootHeader::SetWidthDetectionWord(void)
{
    bHTable->widthDetectionWord = WIDTH_DETECTION;
}

/******************************************************************************/
void ZynqMpBootHeader::SetIdentificationWord(void)
{
    bHTable->identificationWord = HEADER_ID_WORD;
}

/******************************************************************************/
void ZynqMpBootHeader::SetEncryptionKeySource(KeySource::Type keyType)
{
    keyIvMust = false;
    switch (keyType)
    {
        case KeySource::EfuseRedKey:
            bHTable->encryptionKeySource = EFUSE_RED_KEY;
            break;

        case KeySource::BbramRedKey:
            bHTable->encryptionKeySource = BBRAM_RED_KEY;
            break;

        case KeySource::EfuseBlkKey:
            bHTable->encryptionKeySource = EFUSE_BLK_KEY;
            keyIvMust = true;
            break;

        case KeySource::BhBlkKey:
            bHTable->encryptionKeySource = BH_BLACK_KEY;
            keyIvMust = true;
            break;

        case KeySource::EfuseGryKey:
            bHTable->encryptionKeySource = EFUSE_GRY_KEY;
            keyIvMust = true;
            break;

        case KeySource::BhGryKey:
            bHTable->encryptionKeySource = BH_GRY_KEY;
            keyIvMust = true;
            break;

        case KeySource::BhKupKey:
            bHTable->encryptionKeySource = BH_KUP_KEY;
            break;

        case KeySource::None:
        default:
            bHTable->encryptionKeySource = 0;
            break;
    }
}

/******************************************************************************/
void ZynqMpBootHeader::SetFsblExecAddress(uint32_t address)
{
    bHTable->fsblExecAddress = address;
}

/******************************************************************************/
void ZynqMpBootHeader::SetSourceOffset(uint32_t offset)
{
    bHTable->sourceOffset = offset;
    if (slaveBootSplitMode)
    {
        bHTable->sourceOffset -= allHdrSize;
    }
}

/******************************************************************************/
void ZynqMpBootHeader::SetPmuFwLength(uint32_t size)
{
    bHTable->pmuFwLength = size;
}

/******************************************************************************/
void ZynqMpBootHeader::SetTotalPmuFwLength(uint32_t size)
{
    if (size > PMU_MAX_SIZE)
    {
        LOG_ERROR("Total PMU Firmware size %dKB > maximum allowable size (128KB)", size / 1024);
    }
    bHTable->totalPmuFwLength = size;
}

/******************************************************************************/
void ZynqMpBootHeader::SetFsblLength(uint32_t size)
{
    bHTable->fsblLength = size;
}

/******************************************************************************/
void ZynqMpBootHeader::SetTotalFsblLength(uint32_t size)
{
    bHTable->totalFsblLength = size;
}

/******************************************************************************/
void ZynqMpBootHeader::AddAcSizeToTotalFsblSize(BootImage & bi)
{
    /* If authentication is enabled, and the image is not already authenticated, i.e if plain/encrypted
    image is provided through release mode flow, then add AC size to FSBL size */
    if ((bi.imageList.front()->GetAuthenticationType() != Authentication::None) && (bi.imageList.front()->IsBootloader()))
    {
        if (!(bi.imageList.front()->GetAuthContext()->preSigned))
        {
            /* The AC should be appended to a 64-byte aligned FSBL + PMU partition
            So find the pad length required for alignment and then add AC size */
            uint32_t padLength = (64 - (((bHTable->totalFsblLength + bHTable->totalPmuFwLength) & 63) & 63));
            bHTable->totalFsblLength += (bi.headerAC->section->Length + padLength);
            SetHeaderChecksum();
        }
    }
}

/******************************************************************************/
void ZynqMpBootHeader::SetBHForSinglePartitionImage(BootImage& bi)
{
    SetSourceOffset(0);
    SetFsblExecAddress(0);
    SetPmuFwLength(0);
    SetTotalPmuFwLength(0);
    SetFsblLength(0);
    SetSecureHdrIv(bi.options.secHdrIv);
    SetTotalFsblLength(0);
}

/******************************************************************************/
void ZynqMpBootHeader::SetBHForXIP(BootImage& bi)
{
    if (bi.bifOptions->GetXipMode())
    {
        SetFsblLength(0);
        SetTotalFsblLength(0);
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
void ZynqMpBootHeader::SetFsblAttributes(BootImage& bi)
{
    if (bi.imageList.size() != 0)
    {
        uint8_t cksumType = 0;
        cksumType = bi.partitionHeaderList.front()->imageHeader->GetChecksumContext()->Type();

        bHTable->fsblAttributes = bi.bifOptions->GetOptKey() << OPT_KEY_BIT_SHIFT;
        bHTable->fsblAttributes |= bi.bifOptions->GetAuthOnly() << AUTH_ONLY_BIT_SHIFT;
        bHTable->fsblAttributes |= bi.bifOptions->GetPufHdLoc() << PUF_HD_BIT_SHIFT;
        bHTable->fsblAttributes |= cksumType << BI_HASH_BIT_SHIFT;
        bHTable->fsblAttributes |= bi.GetCore() << CORE_BIT_SHIFT;
        bHTable->fsblAttributes |= bi.GetAuthHashAlgo() << AUTH_HASH_BIT_SHIFT;
        bHTable->fsblAttributes |= bi.bifOptions->GetBhRsa() << BH_RSA_BIT_SHIFT;
        bHTable->fsblAttributes |= bi.bifOptions->GetPufMode() << BH_PUF_MODE_BIT_SHIFT;
    }
}

/******************************************************************************/
void ZynqMpBootHeader::SetFsblAttributes(uint32_t fsblattributes)
{
    bHTable->fsblAttributes = fsblattributes;
}

/******************************************************************************/
void ZynqMpBootHeader::SetHeaderChecksum(void)
{
    /* Set the simple header checksum value by adding up all of the 32 bit
       values from the Width Detection Word (0x20) to the fsbl attributes (0x44)
       (10 words, 40 bytes) */
    bHTable->headerChecksum = ComputeWordChecksum(&bHTable->widthDetectionWord, 40);
}

/******************************************************************************/
void ZynqMpBootHeader::SetGreyOrBlackKey(std::string keyFile)
{
    uint8_t* bhKeyData = new uint8_t[BLK_GRY_KEY_LENGTH * 4];
    memset(bhKeyData, 0, BLK_GRY_KEY_LENGTH * 4);

    if (keyFile != "")
    {
        FileImport fileReader;
        if (!fileReader.LoadHexData(keyFile, bhKeyData, BLK_GRY_KEY_LENGTH * 4))
        {
            LOG_ERROR("Invalid no. of data bytes for Grey/Black key in BootHeader.\n           Expected length for Grey/Black key is 32 bytes");
        }
    }
    memcpy(&bHTable->greyOrBlackKey, bhKeyData, BLK_GRY_KEY_LENGTH * 4);

    delete[] bhKeyData;
}

/******************************************************************************/
void ZynqMpBootHeader::SetShutterValue(uint32_t value)
{
    bHTable->shutterValue = value;
}

/******************************************************************************/
void ZynqMpBootHeader::SetUdf(std::string udfFile)
{
    uint8_t bhUdfData[UDF_BH_SIZE_ZYNQMP * sizeof(uint32_t)];

    memset(bhUdfData, 0, UDF_BH_SIZE_ZYNQMP * 4);

    if (udfFile != "")
    {
        LoadUdfData(udfFile, bhUdfData, UDF_BH_SIZE_ZYNQMP);
    }
    memcpy((uint8_t*)bHTable->udf, bhUdfData, UDF_BH_SIZE_ZYNQMP * sizeof(uint32_t));
}

/******************************************************************************/
void ZynqMpBootHeader::SetImageHeaderByteOffset(uint32_t address)
{
    bHTable->imageHeaderByteOffset = address;
    if (slaveBootSplitMode)
    {
        bHTable->imageHeaderByteOffset = 0;
    }
}

/******************************************************************************/
void ZynqMpBootHeader::SetPartitionHeaderByteOffset(uint32_t address)
{
    bHTable->partitionHeaderByteOffset = address;
    if (slaveBootSplitMode)
    {
        bHTable->partitionHeaderByteOffset = 0;
    }
}

/******************************************************************************/
void ZynqMpBootHeader::SetSecureHdrIv(uint8_t* iv)
{
    if (iv == NULL)
    {
        memset(bHTable->secureHdrIv, 0, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }
    else
    {
        memcpy(bHTable->secureHdrIv, iv, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }
}

/******************************************************************************/
void ZynqMpBootHeader::SetGreyOrBlackKekIV(std::string ivFile)
{
    uint8_t* ivData = new uint8_t[IV_LENGTH * 4];
    memset(ivData, 0, IV_LENGTH * 4);

    if (ivFile != "")
    {
        FileImport fileReader;
        if (!fileReader.LoadHexData(ivFile, ivData, IV_LENGTH * 4))
        {
            LOG_ERROR("Invalid no. of data bytes for Black/Grey Key IV.\n           Expected length for Grey/Black IV is 12 bytes");
        }
    }
    else
    {
        if (keyIvMust)
        {
            LOG_ERROR("Black/Grey IV is mandatory in case of Black/Grey key sources\n          Please use [bh_key_iv] to specify the IV in BIF file");
        }
    }
    memcpy(&bHTable->greyOrBlackIV, ivData, IV_LENGTH * 4);
    delete[] ivData;
}

/******************************************************************************/
void ZynqMpBootHeader::SetPufData(BootImage &bi)
{
    if (bi.bifOptions->GetPufHdLoc() == PufHdLoc::PUFinBH)
    {
        uint8_t* pufData = new uint8_t[PUF_DATA_LENGTH];
        memset(pufData, 0, PUF_DATA_LENGTH);

        if (bi.bifOptions->GetPufHelperFile() != "")
        {
            FileImport fileReader;
            if (!fileReader.LoadHexData(bi.bifOptions->GetPufHelperFile(), pufData, PUF_DATA_LENGTH))
            {
                LOG_ERROR("Invalid no. of data bytes for PUF Helper Data.\n           Expected length for PUF Helper Data is 1544 bytes");
            }
        }

        memcpy((uint8_t*)bHTable + GetBootHeaderSize() + GetRegInitTableSize(), pufData, PUF_DATA_LENGTH);
        delete[] pufData;
    }
}

/******************************************************************************/
uint32_t* ZynqMpBootHeader::GetChecksumStartPtr(void)
{
    return &bHTable->widthDetectionWord;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetBootHeaderSize(void)
{
    return sizeof(ZynqMpBootHeaderStructure);
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetWidthDetectionWord(void)
{
    return bHTable->widthDetectionWord;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetIdentificationWord(void)
{
    return bHTable->identificationWord;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetSourceOffset(void)
{
    return bHTable->sourceOffset;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetHeaderChecksum(void)
{
    return bHTable->headerChecksum;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetFsblLength(void)
{
    return bHTable->fsblLength;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetTotalFsblLength(void)
{
    return bHTable->totalFsblLength;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetFsblExecAddress(void)
{
    return bHTable->fsblExecAddress;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetImageHeaderByteOffset(void)
{
    return bHTable->imageHeaderByteOffset;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetPartitionHeaderByteOffset(void)
{
    return bHTable->partitionHeaderByteOffset;
}

/******************************************************************************/
KeySource::Type ZynqMpBootHeader::GetEncryptionKeySource(void)
{
    switch (bHTable->encryptionKeySource)
    {
        case BBRAM_RED_KEY:
            return KeySource::BbramRedKey;
            break;

        case EFUSE_RED_KEY:
            return KeySource::EfuseRedKey;
            break;

        case EFUSE_BLK_KEY:
            return KeySource::EfuseBlkKey;
            break;

        case BH_BLACK_KEY:
            return KeySource::BhBlkKey;
            break;

        case EFUSE_GRY_KEY:
            return KeySource::EfuseGryKey;
            break;

        case BH_GRY_KEY:
            return KeySource::BhGryKey;
            break;

        case BH_KUP_KEY:
            return KeySource::BhKupKey;
            break;

        default:
            return KeySource::None;
            break;
     }
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetPmuFwLength(void)
{
    return bHTable->pmuFwLength;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetTotalPmuFwLength(void)
{
    return bHTable->totalPmuFwLength;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetFsblAttributes(void)
{
    return bHTable->fsblAttributes;
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::CalcHeaderChecksum(void* addr, size_t len)
{
    return ComputeWordChecksum(addr, len);
}

/******************************************************************************/
void ZynqMpBootHeader::BuildRegInitTable(Options& options)
{
    regTable.Build(options, registerInitTable());
}

/******************************************************************************/
uint32_t ZynqMpBootHeader::GetXipBaseAddr(void)
{
    return LQSPI_BASE_ADDR_ZYNQMP;
}

