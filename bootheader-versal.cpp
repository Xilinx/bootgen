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
#include "bootheader-versal.h"
#include "authentication-versal.h"

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
VersalBootHeader::VersalBootHeader(void)
{
    pufData = new uint8_t[PUF_DATA_LENGTH];
    bhKeyData = new uint8_t[BLK_GRY_KEY_LENGTH * 4];
    ivData = new uint8_t[IV_LENGTH * 4];
    kekIvMust = false;

    memset(pufData, 0, PUF_DATA_LENGTH);
    memset(bhKeyData, 0, BLK_GRY_KEY_LENGTH * 4);
    memset(ivData, 0, IV_LENGTH * 4);
    section = new Section("BootHeader", sizeof(VersalBootHeaderStructure));
    bHTable = (VersalBootHeaderStructure*)section->Data;
    smapTable = (VersalSmapWidthTable*)bHTable->smapWords;
}

/******************************************************************************/
VersalBootHeader::VersalBootHeader(std::ifstream& src)
{
    prebuilt = true;
    pufData = new uint8_t[PUF_DATA_LENGTH];
    bhKeyData = new uint8_t[BLK_GRY_KEY_LENGTH * 4];
    ivData = new uint8_t[IV_LENGTH * 4];

    memset(pufData, 0, PUF_DATA_LENGTH);
    memset(bhKeyData, 0, BLK_GRY_KEY_LENGTH * 4);
    memset(ivData, 0, IV_LENGTH * 4);
    kekIvMust = false;

    /* Import the Boot Header from a boot image file */
    section = new Section("BootHeader", sizeof(VersalBootHeaderStructure));
    if (!src.read((char*)section->Data, section->Length).good())
    {
        LOG_ERROR("Failed to read bootheader from imported image");
    }
    bHTable = (VersalBootHeaderStructure*)section->Data;
    smapTable = (VersalSmapWidthTable*)bHTable->smapWords;
}

/******************************************************************************/
VersalBootHeader::~VersalBootHeader()
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
        delete section;
    }
}

/******************************************************************************/
void VersalBootHeader::Build(BootImage& bi, Binary& cache)
{
    SetSmapBusWidthWords(bi.bifOptions->GetSmapWidth());

    if (bi.IsBootloaderFound() == false)
    {
        if (bi.bifOptions->GetSmapWidth() != 0)
        {
            section = new Section("SmapHeader", sizeof(VersalSmapWidthTable));
            smapTable = (VersalSmapWidthTable*)section->Data;
            memcpy(smapTable, bHTable, SMAP_BUS_WIDTH * sizeof(uint32_t));
            cache.Sections.push_back(section);
        }
        return;
    }
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
    SetEncryptionKeySource(bi.options.GetEncryptedKeySource(), bi.bifOptions);
    SetPmcCdoLoadAddress(bi.bifOptions->GetPmcCdoLoadAddress());
    SetShutterValue(bi.bifOptions->GetShutterValue());
    SetGreyOrBlackKey(bi.bifOptions->GetBhKeyFile());
    SetGreyOrBlackIv(kekIvFile);
    SetPlmSecureHdrIv(bi.options.secHdrIv);
    SetPmcDataSecureHdrIv(bi.options.secHdrIvPmcData);
    SetRomReserved();
    SetPmcFwReserved();
    BuildRegInitTable(bi.options);
    SetPufData(bi);
    SetSHA3Padding();
}

/******************************************************************************/
void VersalBootHeader::Link(BootImage& bi)
{
    if (bi.IsBootloaderFound() == false)
    {
        return;
    }

    // If a bootimage is passed to the BIF
    if (prebuilt)
    {
        LinkPrebuiltBH(bi);
        return;
    }

    ImageHeaderTable* iHT = bi.imageHeaderTable;
    ImageHeader* fsbl = NULL;
    if (bi.createSubSystemPdis == true)
    {
        if (bi.subSysImageList.front()->imgList.front()->IsBootloader())
        {
            fsbl = bi.subSysImageList.front()->imgList.front();
        }
    }
    else
    {
        fsbl = iHT->GetFSBLImageHeader();
    }

    slaveBootSplitMode = (bi.bifOptions->GetSplitMode() == SplitMode::SlaveMode) ? true : false;
    fullBhSize = bi.options.bootheaderSize;
    allHdrSize = bi.options.allHeaderSize;
    SetBHAttributes(bi);

    if (fsbl == NULL)
    {
        SetBHForPartialBootImage();
    }
    else
    {
        PartitionHeader& bootloaderHdr(*(fsbl->GetPartitionHeaderList().front()));
        BootloaderAddressAndSizeCheck(bootloaderHdr);

        SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address);
        SetPlmLength((uint32_t)bootloaderHdr.GetPartitionSize());

        SetBHForXIP(bi);

        SetPlmLength(bi.bifOptions->GetPmcFwSize());
        SetPmcCdoLength(bi.bifOptions->GetPmcFwSize());
        SetTotalPlmLength(bi.bifOptions->GetTotalPmcFwSize());
        SetTotalPmcCdoLength(bi.bifOptions->GetTotalPmcFwSize());
        SetPlmSecureHdrIv(bi.options.secHdrIv);

        if (bi.bifOptions->GetPmcFwSize() != 0)
        {
            SetPmcDataSecureHdrIv(bi.options.secHdrIvPmcData);
        }

        if (fsbl->IsStaticFlagSet() || bi.bifOptions->GetXipMode())
        {
            SetTotalPlmLength(0);
        }
        else
        {
            SetPlmLength(bi.GetFsblFwSize());
            SetTotalPlmLength(bi.GetTotalFsblFwSize());
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

    SetHeaderChecksum();
}

/******************************************************************************/
void VersalBootHeader::LinkPrebuiltBH(BootImage& bi)
{
    Authentication::Type authTypeLocal = Authentication::None;
    bool presignedLocal = false;

    if (bi.createSubSystemPdis == true)
    {
        authTypeLocal = bi.subSysImageList.front()->imgList.front()->GetAuthenticationType();
        presignedLocal = bi.subSysImageList.front()->imgList.front()->GetAuthContext()->preSigned;
    }
    else
    {
        authTypeLocal = bi.imageList.front()->GetAuthenticationType();
        presignedLocal = bi.imageList.front()->GetAuthContext()->preSigned;
    }

    if (authTypeLocal != Authentication::None)
    {
        if (!(presignedLocal))
        {
            uint32_t acSize = sizeof(AuthCertificate4096Sha3PaddingStructure);
            bHTable->totalPlmLength += acSize;
            bHTable->sourceOffset += acSize;

            /* The AC should be appended to a 64-byte aligned FSBL + PMU partition
               So find the pad length required for alignment and then add AC size */
            //uint32_t padLength = (64 - (((bHTable->totalFsblLength + bHTable->totalPmuFwLength) & 63) & 63));
            Binary::Length_t shaPadOnLength = bHTable->totalPlmLength + bHTable->totalPmcCdoLength - SIGN_LENGTH_VERSAL;
            uint8_t shaPadLength = SHA3_PAD_LENGTH - (shaPadOnLength % SHA3_PAD_LENGTH);
            if (bHTable->totalPmcCdoLength == 0)
            {
                bHTable->totalPlmLength += shaPadLength;
            }
            else
            {
                bHTable->totalPmcCdoLength += shaPadLength;
            }

            SetHeaderChecksum();
        }
        bi.SetTotalFsblFwSize(bHTable->totalPlmLength);
        bi.bifOptions->SetTotalpmcdataSize(bHTable->totalPmcCdoLength);
    }

    SetPlmLength(bi.GetFsblFwSize());
    SetTotalPlmLength(bi.GetTotalFsblFwSize());
    SetPmcCdoLength(bi.bifOptions->GetPmcFwSize());
    SetTotalPmcCdoLength(bi.bifOptions->GetTotalPmcFwSize());
    if (bi.bifOptions->GetPmcdataFile() != "")
    {
        SetPmcCdoLoadAddress(bi.bifOptions->GetPmcCdoLoadAddress());
    }

    /* This is useful for importing the bootimage and appending new partitions.
    Total metaheader length depends on no. of partitions */
    ImageHeaderTable* iHT = bi.imageHeaderTable;
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

    /* Update checksum as metaheader length is updated */
    SetHeaderChecksum();
}

/******************************************************************************/
void VersalBootHeader::SetBHForPartialBootImage()
{
    SetSourceOffset(0);
    SetPlmLength(0);
    SetTotalPlmLength(0);
    SetPmcCdoLoadAddress(0);
    SetPmcCdoLength(0);
    SetTotalPmcCdoLength(0);
}

/******************************************************************************/
void VersalBootHeader::SetBHForXIP(BootImage& bi)
{
    if (bi.bifOptions->GetXipMode())
    {
        SetTotalPlmLength(0);
        /* TO-DO: Add XIP execution address checks */
    }
}

/******************************************************************************/
void VersalBootHeader::SetSmapBusWidthWords(uint32_t width)
{
    LOG_INFO("SMAP bus width is - %d bits", width);
    switch (width)
    {
    case 8:
        bHTable->smapWords[0] = 0xDD000000;
        bHTable->smapWords[1] = 0x44332211;
        bHTable->smapWords[2] = 0x88776655;
        bHTable->smapWords[3] = 0xCCBBAA99;
        break;

    case 16:
        bHTable->smapWords[0] = 0x00DD0000;
        bHTable->smapWords[1] = 0x33441122;
        bHTable->smapWords[2] = 0x77885566;
        bHTable->smapWords[3] = 0xBBCC99AA;
        break;

    case 32:
    default:
        bHTable->smapWords[0] = 0x000000DD;
        bHTable->smapWords[1] = 0x11223344;
        bHTable->smapWords[2] = 0x55667788;
        bHTable->smapWords[3] = 0x99aabbcc;
        break;
    }
}

/******************************************************************************/
void VersalBootHeader::SetWidthDetectionWord(void)
{
    bHTable->widthDetectionWord = WIDTH_DETECTION;
}

/******************************************************************************/
void VersalBootHeader::SetIdentificationWord(void)
{
    bHTable->identificationWord = HEADER_ID_WORD;
}

/******************************************************************************/
void VersalBootHeader::SetEncryptionKeySource(KeySource::Type keyType, BifOptions* bifOptions)
{
    kekIvMust = false;
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
        kekIvFile = bifOptions->GetEfuseKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'efuse_kek_iv' is mandatory with 'keysrc=efuse_blk_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::BbramBlkKey:
        bHTable->encryptionKeySource = BBRAM_BLK_KEY;
        kekIvFile = bifOptions->GetBbramKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'bbram_kek_iv' is mandatory with 'keysrc=bbram_blk_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::BhBlkKey:
        bHTable->encryptionKeySource = BH_BLACK_KEY;
        kekIvFile = bifOptions->GetBHKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'bh_kek_iv' is mandatory with 'keysrc=bh_blk_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::EfuseGryKey:
        bHTable->encryptionKeySource = EFUSE_GRY_KEY;
        kekIvFile = bifOptions->GetEfuseKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'efuse_kek_iv' is mandatory with 'keysrc=efuse_gry_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::BbramGryKey:
        bHTable->encryptionKeySource = BBRAM_GRY_KEY;
        kekIvFile = bifOptions->GetBbramKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'bbram_kek_iv' is mandatory with 'keysrc=bbram_gry_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::BhGryKey:
        bHTable->encryptionKeySource = BH_GRY_KEY;
        kekIvFile = bifOptions->GetBHKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'bh_kek_iv' is mandatory with 'keysrc=bh_gry_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::None:
    default:
        bHTable->encryptionKeySource = 0;
        break;
    }
}

/******************************************************************************/
void VersalBootHeader::SetPmcCdoLoadAddress(uint32_t address)
{
    bHTable->pmcCdoLoadAddress = address;
}

/******************************************************************************/
void VersalBootHeader::SetSourceOffset(uint32_t offset)
{
    bHTable->sourceOffset = offset;
}

/******************************************************************************/
void VersalBootHeader::SetPmcCdoLength(uint32_t size)
{
    bHTable->pmcCdoLength = size;
}

/******************************************************************************/
void VersalBootHeader::SetTotalPmcCdoLength(uint32_t size)
{
    bHTable->totalPmcCdoLength = size;
}

/******************************************************************************/
void VersalBootHeader::SetPlmLength(uint32_t size)
{
    bHTable->plmLength = size;
}

/******************************************************************************/
void VersalBootHeader::SetTotalPlmLength(uint32_t size)
{
    bHTable->totalPlmLength = size;
}

/******************************************************************************/
void VersalBootHeader::SetBHAttributes(BootImage& bi)
{
    if (bi.imageList.size() != 0)
    {
        uint8_t cksumType = 0;
        cksumType = bi.partitionHeaderList.front()->imageHeader->GetChecksumContext()->Type();

        bHTable->bhAttributes = bi.partitionHeaderList.front()->imageHeader->GetPufHdLocation() << PUF_HD_BIT_SHIFT;
        bHTable->bhAttributes |= cksumType << BI_HASH_BIT_SHIFT;
        bHTable->bhAttributes |= bi.partitionHeaderList.front()->imageHeader->GetDpacm() << DPA_CM_BIT_SHIFT;
        bHTable->bhAttributes |= bi.bifOptions->GetBhRsa() << BH_RSA_BIT_SHIFT;
        bHTable->bhAttributes |= bi.bifOptions->GetPufMode() << BH_PUF_MODE_BIT_SHIFT;
    }
}

/******************************************************************************/
void VersalBootHeader::SetBHAttributes(uint32_t attributes)
{
    bHTable->bhAttributes = attributes;
}

/******************************************************************************/
void VersalBootHeader::SetHeaderChecksum(void)
{
    /* Set the simple header checksum value by adding up all of the 32 bit
    values from the Width Detection Word (0x10) to the PUF (0xF1F)
    (968 words, 3872 bytes) */

    bHTable->headerChecksum = ComputeWordChecksum(&bHTable->widthDetectionWord, 3872);
}

/******************************************************************************/
void VersalBootHeader::SetGreyOrBlackKey(std::string keyFile)
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
void VersalBootHeader::SetShutterValue(uint32_t value)
{
    bHTable->shutterValue = value;
}

/******************************************************************************/
void VersalBootHeader::SetImageHeaderByteOffset(uint32_t address)
{
    bHTable->imageHeaderByteOffset = address;
}

/******************************************************************************/
void VersalBootHeader::SetPlmSecureHdrIv(uint8_t* iv)
{
    if (iv == NULL)
    {
        memset(bHTable->plmSecureHdrIv, 0, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }
    else
    {
        memcpy(bHTable->plmSecureHdrIv, iv, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }
}

/******************************************************************************/
void VersalBootHeader::SetPmcDataSecureHdrIv(uint8_t* iv)
{
    if (iv == NULL)
    {
        memset(bHTable->pmcCdoSecureHdrIv, 0, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }
    else
    {
        memcpy(bHTable->pmcCdoSecureHdrIv, iv, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }
}

/******************************************************************************/
void VersalBootHeader::SetGreyOrBlackIv(std::string ivFile)
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
        if (kekIvMust)
        {
            LOG_ERROR("Black/Grey IV is mandatory in case of Black/Grey key sources\n           Please use 'bh_kek_iv' to specify the IV in BIF file");
        }
    }

    memcpy(&bHTable->greyOrBlackIV, ivData, IV_LENGTH * 4);
    delete[] ivData;
}

/******************************************************************************/
void VersalBootHeader::SetPufData(BootImage &bi)
{
    uint8_t* pufData = new uint8_t[PUF_DATA_LENGTH];
    memset(pufData, 0, PUF_DATA_LENGTH);
    if (bi.bifOptions->GetPufHdLoc() == PufHdLoc::PUFinBH || bi.bifOptions->GetPufHdinBHFlag())
    {
        if (bi.bifOptions->GetPufHelperFile() != "")
        {
            FileImport fileReader;
            if (!fileReader.LoadHexData(bi.bifOptions->GetPufHelperFile(), pufData, PUF_DATA_LENGTH))
            {
                LOG_ERROR("Invalid no. of data bytes for PUF Helper Data.\n           Expected length for PUF Helper Data is 1544 bytes");
            }
        }
    }
    uint32_t pufLoc = GetBootHeaderSize() - sizeof(uint32_t) - (SHA3_PAD_SIZE_VERSAL * 4) - PUF_DATA_LENGTH;
    memcpy((uint8_t*)bHTable + pufLoc, pufData, PUF_DATA_LENGTH);

    delete[] pufData;
}

/******************************************************************************/
void VersalBootHeader::SetRomReserved()
{
    for (int i = 0; i<ROM_BH_RESERVED; i++)
    {
        bHTable->romReserved[i] = DEFAULT_RESERVED_VERSAL;
    }
}

/******************************************************************************/
void VersalBootHeader::SetPmcFwReserved()
{
    for (int i = 0; i<PLM_BH_RESERVED; i++)
    {
        bHTable->plmReserved[i] = DEFAULT_RESERVED_VERSAL;
    }
}

/******************************************************************************/
uint32_t* VersalBootHeader::GetChecksumStartPtr(void)
{
    return &bHTable->widthDetectionWord;
}

/******************************************************************************/
uint32_t VersalBootHeader::GetBootHeaderSize(void)
{
    return sizeof(VersalBootHeaderStructure);
}

/******************************************************************************/
uint32_t VersalBootHeader::GetWidthDetectionWord(void)
{
    return bHTable->widthDetectionWord;
}

/******************************************************************************/
uint32_t VersalBootHeader::GetIdentificationWord(void)
{
    return bHTable->identificationWord;
}

/******************************************************************************/
uint32_t VersalBootHeader::GetSourceOffset(void)
{
    return bHTable->sourceOffset;
}

/******************************************************************************/
uint32_t VersalBootHeader::GetHeaderChecksum(void)
{
    return bHTable->headerChecksum;
}

/******************************************************************************/
uint32_t VersalBootHeader::GetPmcFwLength(void)
{
    return bHTable->plmLength;
}

/******************************************************************************/
uint32_t VersalBootHeader::GetTotalPmcFwLength(void)
{
    return bHTable->totalPlmLength;
}

/******************************************************************************/
uint32_t VersalBootHeader::GetPmcCdoLoadAddress(void)
{
    return bHTable->pmcCdoLoadAddress;
}

/******************************************************************************/
KeySource::Type VersalBootHeader::GetEncryptionKeySource(void)
{
    switch (bHTable->encryptionKeySource)
    {
    case BBRAM_RED_KEY:
        return KeySource::BbramRedKey;
        break;

    case EFUSE_RED_KEY:
        return KeySource::EfuseRedKey;
        break;

    case BBRAM_BLK_KEY:
        return KeySource::BbramBlkKey;
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

    case BBRAM_GRY_KEY:
        return KeySource::BbramGryKey;
        break;

    default:
        return KeySource::None;
        break;
    }
}

/******************************************************************************/
uint32_t VersalBootHeader::GetPmcCdoLength(void)
{
    return bHTable->pmcCdoLength;
}

/******************************************************************************/
uint32_t VersalBootHeader::GetTotalPmcCdoLength(void)
{
    return bHTable->totalPmcCdoLength;
}

/******************************************************************************/
uint32_t VersalBootHeader::GetBHAttributes(void)
{
    return bHTable->bhAttributes;
}

/******************************************************************************/
uint32_t VersalBootHeader::GetImageHeaderByteOffset(void)
{
    return bHTable->imageHeaderByteOffset;
}

/******************************************************************************/
uint32_t VersalBootHeader::CalcHeaderChecksum(void* addr, size_t len)
{
    return ComputeWordChecksum(addr, len);
}

/******************************************************************************/
void VersalBootHeader::BuildRegInitTable(Options& options)
{
    uint32_t regTableLoc = GetBootHeaderSize() - sizeof(uint32_t) - (SHA3_PAD_SIZE_VERSAL * 4) - PUF_DATA_LENGTH - (MAX_REGISTER_INITS * 4 * 2);
    regTable.Build(options, (RegisterInitTable*)(section->Data + regTableLoc));
}

/******************************************************************************/
void VersalBootHeader::SetSHA3Padding()
{
    for (int i = 0; i<SHA3_PAD_SIZE_VERSAL; i++)
    {
        bHTable->sha3Padding[i] = DEFAULT_RESERVED_VERSAL;
    }
    bHTable->sha3Padding[0] = 0x6;
    bHTable->sha3Padding[SHA3_PAD_SIZE_VERSAL - 1] = 0x80000000;
}

/******************************************************************************/
uint32_t VersalBootHeader::GetXipBaseAddr(void)
{
    return LQSPI_BASE_ADDR_VERSAL;
}
