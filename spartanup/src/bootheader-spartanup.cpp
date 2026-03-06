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
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootheader-spartanup.h"
#include "authentication-spartanup.h"
#include "encryptutils.h"

#define PLM_MAX_SIZE               0x60000 //384KB
#define PMCDATA_MAX_SIZE_VERSAL    0x14000 //80KB
#define PMCDATA_MAX_SIZE_VERSALNET 0x10000 //64KB

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
SpartanupBootHeader::SpartanupBootHeader(Arch::Type archType)
{
    arch = archType;
    uint32_t pufDataLength = GetPufDataLength();

    pufData = std::make_unique<uint8_t[]>(pufDataLength);
    bhKeyData = std::make_unique<uint8_t[]>(BLK_GRY_KEY_LENGTH * 4);
    ivData = std::make_unique<uint8_t[]>(IV_LENGTH * 4);
    kekIvMust = false;

    memset(pufData.get(), 0, pufDataLength);
    memset(bhKeyData.get(), 0, BLK_GRY_KEY_LENGTH * 4);
    memset(ivData.get(), 0, IV_LENGTH * 4);
    auto temp_section = std::make_unique<Section>("BootHeader", sizeof(SpartanupBootHeaderStructure));
    section = temp_section.release();  // Transfer ownership to raw pointer member
    bHTable = (SpartanupBootHeaderStructure*)section->Data.get();
    smapTable = (SpartanupSmapWidthTable*)bHTable->smapWords;
}

/******************************************************************************/
SpartanupBootHeader::SpartanupBootHeader(std::ifstream& src, Arch::Type archType)
{
    prebuilt = true;
    arch = archType;
    uint32_t pufDataLength = GetPufDataLength();

    pufData = std::make_unique<uint8_t[]>(pufDataLength);
    bhKeyData = std::make_unique<uint8_t[]>(BLK_GRY_KEY_LENGTH * 4);
    ivData = std::make_unique<uint8_t[]>(IV_LENGTH * 4);

    memset(pufData.get(), 0, pufDataLength);
    memset(bhKeyData.get(), 0, BLK_GRY_KEY_LENGTH * 4);
    memset(ivData.get(), 0, IV_LENGTH * 4);
    kekIvMust = false;

    /* Import the Boot Header from a boot image file */
    auto temp_section = std::make_unique<Section>("BootHeader", sizeof(SpartanupBootHeaderStructure));
    section = temp_section.release();  // Transfer ownership to raw pointer member
    if (!src.read((char*)section->Data.get(), section->Length).good())
    {
        LOG_ERROR("Failed to read bootheader from imported image");
    }
    bHTable = (SpartanupBootHeaderStructure*)section->Data.get();
    smapTable = (SpartanupSmapWidthTable*)bHTable->smapWords;
}

/******************************************************************************/
SpartanupBootHeader::~SpartanupBootHeader()
{
    // pufData, bhKeyData, ivData are now std::unique_ptr - automatic cleanup via RAII
    // section is owned by cache.Sections (std::list<std::unique_ptr<Section>>)
}

/******************************************************************************/
void SpartanupBootHeader::Build(BootImage& bi, Binary& cache)
{
    ResizeSection(bi);

    SetSmapBusWidthWords(bi.bifOptions->GetSmapWidth());

    // NOTE: Don't push section here - it's pushed later in both prebuilt and non-prebuilt paths

    /* If the boot header is imported from a bootimage file, no need to build */
    if (prebuilt)
    {
        if (bi.bifOptions->GetRegInitFileName() != "")
        {
            BuildRegInitTable(bi.options);
        }
        
        // Move section to cache after using it
        if (section != nullptr)
        {
            cache.Sections.push_back(std::unique_ptr<Section>(section));

        }
        return;
    }

    SetWidthDetectionWord();
    SetIdentificationWord();
    SetEncryptionKeySource(bi.options.GetEncryptedKeySource(), bi.bifOptions);
    SetPmcCdoLoadAddress(bi.bifOptions->GetPmcCdoLoadAddress());
    SetShutterValue(bi.bifOptions->GetShutterValue());
    SetPufRingOscilltorSwapConfigValue(bi.bifOptions->GetPufRingOscilltorSwapConfigValue());
    SetGreyOrBlackKey(bi.bifOptions->GetBhKeyFile());

    if (bi.options.IsVersalNetSeries() && bi.bifOptions->GetBHKekIVFile() != "")
    {
        kekIvFile = bi.bifOptions->GetBHKekIVFile();
    }

    SetGreyOrBlackIv(kekIvFile);
    SetPlmSecureHdrIv(bi.options.secHdrIv.get());
    SetPmcDataSecureHdrIv(bi.options.secHdrIvPmcData.get());
    if (bi.options.IsVersalNetSeries())
    {
        if(bi.imageList.size() != 0)
            SetRevokeId(bi.imageList.front()->GetPartitionRevocationId());
    }
    else
    {
        SetRevokeId(0x00000000);
    }
    SetRomReserved();
    
    SetAuthHeader(bi, 0);
    SetHashBlockSize(bi.hashBlockLength);
    SetPPKSize(0,0);
    SetSignatureSize(0,0);

    SetPufPDIIdentificationWord();
    SetPmcFwReserved();
    BuildRegInitTable(bi.options);
    SetPufData(bi);
    //SetSHA3Padding();
    
    // Move section to cache AFTER we're done using it
    if (section != nullptr)
    {
        cache.Sections.push_back(std::unique_ptr<Section>(section));

    }
}

/******************************************************************************/
void SpartanupBootHeader::Link(BootImage& bi)
{
    // If a bootimage is passed to the BIF
    if (prebuilt)
    {
        LinkPrebuiltBH(bi);
        return;
    }

    SetHashBlockSize(bi.hashBlockLength);

    ImageHeaderTable* iHT = bi.imageHeaderTable.get();
    ImageHeader* fsbl = NULL;
    if (bi.createSubSystemPdis == true)
    {
        if (bi.subSysImageList.size() != 0)
        {
            if (bi.subSysImageList.front()->imgList.front()->IsBootloader())
            {
                fsbl = bi.subSysImageList.front()->imgList.front();
            }
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

        if (bootloaderHdr.imageHeader->GetChecksumContext()->Type() != Checksum::None && bi.options.IsVersalNetSeries())
        {
            if((bootloaderHdr.imageHeader->GetEncryptContext()->Type() != Encryption::None) && (bootloaderHdr.imageHeader->GetAuthenticationType() == Authentication::None))
                SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address + bHTable->hashBlockLength1 + AES_GCM_TAG_SZ);
            else
                SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address + bHTable->hashBlockLength1);
        }
        else
        {
            if ((bootloaderHdr.imageHeader->GetEncryptContext()->Type() != Encryption::None) && (bootloaderHdr.imageHeader->GetAuthenticationType() == Authentication::None))
                SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address + bHTable->hashBlockLength1 + AES_GCM_TAG_SZ);
            else if (bootloaderHdr.imageHeader->GetAuthenticationType() != Authentication::None)
                SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address + bHTable->hashBlockLength1 + bHTable->totalSignatureSize1);
            else
                SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address + bHTable->hashBlockLength1);
        }
        SetPlmLength((uint32_t)bootloaderHdr.GetPartitionSize());

        SetBHForXIP(bi);

        SetPlmLength(bi.bifOptions->GetPmcFwSize());
        SetPmcCdoLength(bi.bifOptions->GetPmcFwSize());
        SetTotalPlmLength(bi.bifOptions->GetTotalPmcFwSize());
        SetTotalPmcCdoLength(bi.bifOptions->GetTotalPmcFwSize());
        SetPlmSecureHdrIv(bi.options.secHdrIv.get());

        if (bi.bifOptions->GetPmcFwSize() != 0)
        {
            SetPmcDataSecureHdrIv(bi.options.secHdrIvPmcData.get());
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

        SetImageHeaderByteOffset(bi.bifOptions->GetPdiId());
    }

    SetHeaderChecksum(bi.options);
    //uint8_t* tmpBh = bi.bootHeader->section->Data.get();
    //LOG_TRACE("BH Link");
    //LOG_DUMP_BYTES(tmpBh, bi.bootHeader->section->Length);

    //Update below checks for SpartanUp
#if 0
    if (bHTable->totalPlmLength > PLM_MAX_SIZE && !bi.options.IsVersalNetSeries())
    {
        LOG_ERROR("Total PLM size : %dKB > maximum allowable size (384KB)", bHTable->totalPlmLength / 1024);
    }
    if (bHTable->totalPmcCdoLength > PMCDATA_MAX_SIZE_VERSAL && !bi.options.IsVersalNetSeries())
    {
        LOG_ERROR("Total PMC DATA size : %dKB > maximum allowable size (80KB)", bHTable->totalPlmLength / 1024);
    }
    if (bHTable->totalPmcCdoLength > PMCDATA_MAX_SIZE_VERSALNET && bi.options.IsVersalNetSeries())
    {
        LOG_ERROR("Total PMC DATA size : %dKB > maximum allowable size (64KB)", bHTable->totalPlmLength / 1024);
    }
#endif
}

/******************************************************************************/
void SpartanupBootHeader::ResizeSection(BootImage &bi)
{
    /* If PUF is present in BH, increase the BH size */
    if (bi.bifOptions->GetPufHdLoc() == PufHdLoc::PUFinBH || bi.bifOptions->GetPufHdinBHFlag())
    {
        uint32_t newBhSize = GetBootHeaderSize() + GetPufDataLength();
        section->IncreaseLengthAndPadTo(newBhSize, 0);

        bHTable = (SpartanupBootHeaderStructure*)section->Data.get();
    }
}

/******************************************************************************/
void SpartanupBootHeader::LinkPrebuiltBH(BootImage& bi)
{
    Authentication::Type authTypeLocal = Authentication::None;
    bool presignedLocal = false;

    if (bi.subSysImageList.size() != 0)
    {
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
    }

    if (authTypeLocal != Authentication::None)
    {
        if (!(presignedLocal))
        {
            uint32_t acSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure);
            if(authTypeLocal != Authentication::ECDSA)
                acSize = sizeof(AuthCertificateECDSAHBStructure);
            else if(authTypeLocal != Authentication::ECDSAp521)
                acSize = sizeof(AuthCertificateECDSAp521HBStructure);

            bHTable->sourceOffset += acSize;
            if (bi.options.IsVersalNetSeries())
            {
                SetTotalPlmLength(bi.GetTotalFsblFwSize());
                SetTotalPmcCdoLength(bi.bifOptions->GetTotalPmcFwSize());
            }
            else
            {
                bHTable->totalPlmLength += acSize;
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
            }
            SetHeaderChecksum(bi.options);
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
    ImageHeaderTable* iHT = bi.imageHeaderTable.get();
    if (iHT->section != NULL)
    {
        if (!Binary::CheckAddress(iHT->section->Address))
        {
            LOG_ERROR("Bootgen does not support addresses larger than 32 bits. Image Header Offset is %llX", iHT->section->Address);
        }

        SetImageHeaderByteOffset(0);
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
    SetHeaderChecksum(bi.options);
}

/******************************************************************************/
void SpartanupBootHeader::SetBHForPartialBootImage()
{
    SetSourceOffset(0);
    SetPlmLength(0);
    SetTotalPlmLength(0);
    SetPmcCdoLoadAddress(0);
    SetPmcCdoLength(0);
    SetTotalPmcCdoLength(0);
}

/******************************************************************************/
void SpartanupBootHeader::SetBHForXIP(BootImage& bi)
{
    if (bi.bifOptions->GetXipMode())
    {
        SetTotalPlmLength(0);
        /* TO-DO: Add XIP execution address checks */
    }
}

/******************************************************************************/
void SpartanupBootHeader::SetSmapBusWidthWords(uint32_t width)
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
void SpartanupBootHeader::SetWidthDetectionWord(void)
{
    bHTable->widthDetectionWord = WIDTH_DETECTION_LASSEN;
}

/******************************************************************************/
void SpartanupBootHeader::SetIdentificationWord(void)
{
    bHTable->identificationWord = HEADER_ID_WORD;
}

/******************************************************************************/
void SpartanupBootHeader::SetEncryptionKeySource(KeySource::Type keyType, BifOptions* bifOptions)
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
void SpartanupBootHeader::SetPmcCdoLoadAddress(uint32_t address)
{
    bHTable->pmcCdoLoadAddress = address;
}

/******************************************************************************/
void SpartanupBootHeader::SetSourceOffset(uint32_t offset)
{
    bHTable->sourceOffset = offset;
}

/******************************************************************************/
void SpartanupBootHeader::SetPmcCdoLength(uint32_t size)
{
    bHTable->pmcCdoLength = size;
}

/******************************************************************************/
void SpartanupBootHeader::SetTotalPmcCdoLength(uint32_t size)
{
    bHTable->totalPmcCdoLength = size;
}

/******************************************************************************/
void SpartanupBootHeader::SetPlmLength(uint32_t size)
{
    bHTable->plmLength = size;
}

/******************************************************************************/
void SpartanupBootHeader::SetTotalPlmLength(uint32_t size)
{
    bHTable->totalPlmLength = size;
}

/******************************************************************************/
void SpartanupBootHeader::SetBHAttributes(BootImage& bi)
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

        if (bi.options.IsVersalNetSeries())
        {
            if (bi.partitionHeaderList.front()->imageHeader->GetAuthenticationType() != Authentication::None)
            {
                bHTable->bhAttributes |= BH_RSA_SINGED_BIT_MASK << BH_RSA_SINGED_BIT_SHIFT;
            }

            bHTable->bhAttributes |= bi.bifOptions->GetDice() << BH_DICE_BIT_SHIFT;
        }
    }
}

/******************************************************************************/
void SpartanupBootHeader::SetBHAttributes(uint32_t attributes)
{
    bHTable->bhAttributes = attributes;
}

/******************************************************************************/
void SpartanupBootHeader::SetHeaderChecksum(Options& options)
{  
    uint32_t checksumLength = GetBootHeaderSize() - sizeof(uint32_t) - sizeof(SpartanupSmapWidthTable);   
    bHTable->headerChecksum = ComputeWordChecksum(&bHTable->widthDetectionWord, checksumLength);
}

/******************************************************************************/
void SpartanupBootHeader::SetGreyOrBlackKey(std::string keyFile)
{
    auto bhKeyData = std::make_unique<uint8_t[]>(BLK_GRY_KEY_LENGTH * 4);
    memset(bhKeyData.get(), 0, BLK_GRY_KEY_LENGTH * 4);

    if (keyFile != "")
    {
        FileImport fileReader;
        if (!fileReader.LoadHexData(keyFile, bhKeyData.get(), BLK_GRY_KEY_LENGTH * 4))
        {
            LOG_ERROR("Invalid no. of data bytes for Grey/Black key in BootHeader.\n           Expected length for Grey/Black key is 32 bytes");
        }
    }
    memcpy(&bHTable->greyOrBlackKey, bhKeyData.get(), BLK_GRY_KEY_LENGTH * 4);
}

/******************************************************************************/
void SpartanupBootHeader::SetShutterValue(uint32_t value)
{
    bHTable->shutterValue = value;
}

/******************************************************************************/
void SpartanupBootHeader::SetRevokeId(uint32_t id)
{
    bHTable->plmRevokeId = id;
}

/******************************************************************************/
void SpartanupBootHeader::SetPufRingOscilltorSwapConfigValue(uint32_t value)
{
    bHTable->pufRoSwapConfigVal = value;
}

/******************************************************************************/
void SpartanupBootHeader::SetImageHeaderByteOffset(uint32_t address)
{
    bHTable->imageHeaderByteOffset = address;
}

/******************************************************************************/
void SpartanupBootHeader::SetPlmSecureHdrIv(uint8_t* iv)
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
void SpartanupBootHeader::SetPmcDataSecureHdrIv(uint8_t* iv)
{
    /* Spartanup does not have pmcCdoSecureHdrIv in boot header structure */
}

/******************************************************************************/
void SpartanupBootHeader::SetGreyOrBlackIv(std::string ivFile)
{
    auto ivData = std::make_unique<uint8_t[]>(IV_LENGTH * 4);
    memset(ivData.get(), 0, IV_LENGTH * 4);

    if (ivFile != "")
    {
        FileImport fileReader;
        if (!fileReader.LoadHexData(ivFile, ivData.get(), IV_LENGTH * 4))
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

    memcpy(&bHTable->greyOrBlackIV, ivData.get(), IV_LENGTH * 4);
}

/******************************************************************************/
void SpartanupBootHeader::SetPufPDIIdentificationWord()
{
    //if(arch == Arch::LASSEN)
        bHTable->pufPDIIdentificationWord = 0;
}

/******************************************************************************/
void SpartanupBootHeader::SetPufData(BootImage &bi)
{
    SetPufHDLength(0);
    uint32_t actualPufHDDataLength = bi.bootHeader->GetPufDataLength();
    actualPufHDDataLength = bi.bootHeader->GetPufDataLength() - PUF_DATA_LENGTH_4K_ALIGNMENT;

    auto pufDataTemp = std::make_unique<uint8_t[]>(bi.bootHeader->GetPufDataLength());
    memset(pufDataTemp.get(), 0, bi.bootHeader->GetPufDataLength());
    if (bi.bifOptions->GetPufHdLoc() == PufHdLoc::PUFinBH || bi.bifOptions->GetPufHdinBHFlag())
    {
        if (bi.bifOptions->GetPufHelperFile() != "")
        {
            FileImport fileReader;
            if (!fileReader.LoadHexData(bi.bifOptions->GetPufHelperFile(), pufDataTemp.get(), actualPufHDDataLength))
            {
                LOG_ERROR("Invalid no. of data bytes for PUF Helper Data.\n           Expected length for PUF Helper Data is %d bytes", actualPufHDDataLength);
            }
            
            SetPufHDLength(bi.bootHeader->GetPufDataLength());
            memcpy((uint8_t*)bHTable + GetBootHeaderSize(), pufDataTemp.get(), GetPufDataLength());
        }
    }
}

/******************************************************************************/
void SpartanupBootHeader::SetPufHDLength(uint32_t value)
{
    bHTable->pufHDLength = value;
}

/******************************************************************************/
void SpartanupBootHeader::SetHashBlockSize(uint32_t value)
{
    bHTable->hashBlockLength1 = value;
}

/******************************************************************************/
void SpartanupBootHeader::SetAuthHeader(BootImage &bi, uint32_t value)
{
    if(!bi.options.IsDl9Series())
        bHTable->authHeader1 = AUTH_HDR_LASSEN_NONSECURE;
    else
        bHTable->authHeader1 = AUTH_HDR_TELLURIDE_NONSECURE;
}

/******************************************************************************/
void SpartanupBootHeader::SetPPKSize(uint32_t actualSize, uint32_t totalSize)
{
    bHTable->actualppkSize1 = actualSize;
    bHTable->totalppkkSize1 = totalSize;
}

/******************************************************************************/
void SpartanupBootHeader::SetSignatureSize(uint32_t actualSize, uint32_t totalSize)
{
    bHTable->actualSignatureSize1 = actualSize;
    bHTable->totalSignatureSize1 = totalSize;
}

/******************************************************************************/
void SpartanupBootHeader::SetRomReserved()
{
    int reservedLength = ROM_BH_RESERVED_LASSEN;

    for (int i = 0; i < reservedLength; i++)
    {
        bHTable->romReserved[i] = DEFAULT_RESERVED_VERSAL;
    }
}

/******************************************************************************/
void SpartanupBootHeader::SetPmcFwReserved()
{
    for (int i = 0; i < PLM_BH_RESERVED; i++)
    {
        bHTable->plmReserved[i] = DEFAULT_RESERVED_VERSAL;
    }
}

/******************************************************************************/
uint32_t* SpartanupBootHeader::GetChecksumStartPtr(void)
{
    return &bHTable->widthDetectionWord;
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetBootHeaderSize(void)
{
    return sizeof(SpartanupBootHeaderStructure);
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetBHChecksumDataSize(void)
{
    return (GetBootHeaderSize() - sizeof(uint32_t) - sizeof(SpartanupSmapWidthTable));
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetWidthDetectionWord(void)
{
    return bHTable->widthDetectionWord;
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetIdentificationWord(void)
{
    return bHTable->identificationWord;
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetSourceOffset(void)
{
    return bHTable->sourceOffset;
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetHeaderChecksum(void)
{
    return bHTable->headerChecksum;
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetPmcFwLength(void)
{
    return bHTable->plmLength;
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetTotalPmcFwLength(void)
{
    return bHTable->totalPlmLength;
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetPmcCdoLoadAddress(void)
{
    return bHTable->pmcCdoLoadAddress;
}

/******************************************************************************/
KeySource::Type SpartanupBootHeader::GetEncryptionKeySource(void)
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
uint32_t SpartanupBootHeader::GetPmcCdoLength(void)
{
    return bHTable->pmcCdoLength;
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetTotalPmcCdoLength(void)
{
    return bHTable->totalPmcCdoLength;
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetBHAttributes(void)
{
    return bHTable->bhAttributes;
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetImageHeaderByteOffset(void)
{
    return bHTable->imageHeaderByteOffset;
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetPufDataLength()
{
    return PUF_DATA_LENGTH_4K;
}

/******************************************************************************/
void SpartanupBootHeader::BuildRegInitTable(Options& options)
{
    uint32_t regTableLoc = GetBootHeaderSize() - sizeof(uint32_t) - (MAX_REG_INIT_LASSEN * 4);

    regTable.SetMaximunRegInitPairs(MAX_REG_INIT_LASSEN / 2);
    regTable.Build(options, (RegisterInitTable*)(section->Data.get() + regTableLoc));
}

/******************************************************************************/
uint32_t SpartanupBootHeader::GetXipBaseAddr(void)
{
    return LQSPI_BASE_ADDR_VERSAL;
}
