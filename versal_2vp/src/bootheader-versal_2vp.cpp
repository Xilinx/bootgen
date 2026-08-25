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
#include "bootheader-versal_2vp.h"
#include "authentication-versal_2vp.h"
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
Versal_2vpBootHeader::Versal_2vpBootHeader(Arch::Type archType)
{
    arch = archType;
    uint32_t pufDataLength = GetPufDataLength();

    pufData = new uint8_t[pufDataLength];
    bhKeyData = new uint8_t[BLK_GRY_KEY_LENGTH * 4];
    ivData = new uint8_t[IV_LENGTH * 4];
    kekIvMust = false;

    memset(pufData, 0, pufDataLength);
    memset(bhKeyData, 0, BLK_GRY_KEY_LENGTH * 4);
    memset(ivData, 0, IV_LENGTH * 4);
    section = new Section("BootHeader", sizeof(Versal_2vpBootHeaderStructure));
    bHTable = (Versal_2vpBootHeaderStructure*)section->Data.get();
    smapTable = (Versal_2vpSmapWidthTable*)bHTable->smapWords;
}

/******************************************************************************/
Versal_2vpBootHeader::Versal_2vpBootHeader(std::ifstream& src, Arch::Type archType)
{
    prebuilt = true;
    arch = archType;
    uint32_t pufDataLength = GetPufDataLength();

    pufData = new uint8_t[pufDataLength];
    bhKeyData = new uint8_t[BLK_GRY_KEY_LENGTH * 4];
    ivData = new uint8_t[IV_LENGTH * 4];

    memset(pufData, 0, pufDataLength);
    memset(bhKeyData, 0, BLK_GRY_KEY_LENGTH * 4);
    memset(ivData, 0, IV_LENGTH * 4);
    kekIvMust = false;

    /* Import the Boot Header from a boot image file */
    section = new Section("BootHeader", sizeof(Versal_2vpBootHeaderStructure));
    if (!src.read((char*)section->Data.get(), section->Length).good())
    {
        LOG_ERROR("Failed to read bootheader from imported image");
    }
    bHTable = (Versal_2vpBootHeaderStructure*)section->Data.get();
    smapTable = (Versal_2vpSmapWidthTable*)bHTable->smapWords;
}

/******************************************************************************/
Versal_2vpBootHeader::~Versal_2vpBootHeader()
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

}

/******************************************************************************/
void Versal_2vpBootHeader::Build(BootImage& bi, Binary& cache)
{
    ResizeSection(bi);

    SetSmapBusWidthWords(bi.bifOptions->GetSmapWidth());

    if (bi.IsBootloaderFound() == false)
    {
        if (bi.bifOptions->GetSmapWidth() != 0)
        {
            section = new Section("SmapHeader", sizeof(Versal_2vpSmapWidthTable));
            smapTable = (Versal_2vpSmapWidthTable*)section->Data.get();
            memcpy(smapTable, bHTable, SMAP_BUS_WIDTH * sizeof(uint32_t));
            cache.Sections.push_back(std::unique_ptr<Section>(section));
            section = nullptr;
        }
        return;
    }

    /* If the boot header is imported from a bootimage file, no need to build */
    if (prebuilt)
    {
        if (bi.bifOptions->GetRegInitFileName() != "")
        {
            BuildRegInitTable(bi.options);
        }
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
    
    SetUdfBhTelluride(0);
   // SetAuthHeader(bi, 0);
    SetHashAlgo(bi, 0);
    SetHashBlockSize(bi.hashBlockLength);
    //SetPPKSize(0,0);
   // SetSignatureSize(0,0);

    SetPufPDIIdentificationWord();
    SetPmcFwReserved();
    BuildRegInitTable(bi.options);
    SetPufData(bi);
    //SetSHA3Padding();
    
    if (section != nullptr)
    {
        cache.Sections.push_back(std::unique_ptr<Section>(section));
    }
}



/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetAuthCertificateSize(PartitionHeader& bootloaderHdr)
{
    uint32_t acSize = 0;
    Authentication::Type authType = bootloaderHdr.imageHeader->GetAuthenticationType();
    
    switch (authType)
    {
        case Authentication::RSA:
            acSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure_versal2vp);
            break;
            
        case Authentication::ECDSA:
            acSize = sizeof(AuthCertificateECDSAHBStructure_versal2vp);
            break;
            
        case Authentication::ECDSAp521:
            acSize = sizeof(AuthCertificateECDSAp521HBStructure_versal2vp);
            break;
            
        case Authentication::LMS_SHA2_256:
        case Authentication::LMS_SHAKE256:
            // For LMS/HSS, get AC size from authentication context (includes hashblock signature)
            {
                Versal_2vpAuthenticationContext* authContext = 
                    (Versal_2vpAuthenticationContext*)bootloaderHdr.imageHeader->GetAuthContext();
                if (authContext != NULL)
                {
                    acSize = authContext->GetCertificateSize();  // This already includes hashblock signature
                }
            }
            break;
		case Authentication::MLDSA:
		    acSize = sizeof(AuthCertificateMLDSAStructure_versal2vp);
            break;
        case Authentication::SLH_SHAKE256:
            {
                Versal_2vpAuthenticationContext* authContext = 
                        (Versal_2vpAuthenticationContext*)bootloaderHdr.imageHeader->GetAuthContext();
                if (authContext != NULL)
                {
                    acSize = sizeof(authContext->GetCertificateSize());
                }
                break;
            }
            
        default:
            acSize = 0;
            break;
    }
    
    return acSize;
}

/******************************************************************************/
void Versal_2vpBootHeader::Link(BootImage& bi)
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

        // For versal_2vp devices, plm.elf.0 contains only PLM+PMC content (no prepended hash block or AC)
        // Hash block and AC are in separate sections: plm.elf.0.HashBlock and plm.elf.0.sha384/sha256/shake256
        bool isVersal2vpNative = (bi.options.GetArchType() == Arch::VERSAL_2VP);
        
        if (isVersal2vpNative) {
            bool is_auth = bootloaderHdr.imageHeader->GetAuthenticationType() != Authentication::None;
            bool is_enc  = bootloaderHdr.imageHeader->GetEncryptContext()->Type() != Encryption::None;
            uint32_t plmOffset;
            if (is_auth || is_enc) {
                // Secure flow: offset points to pure PLM+PMC
                plmOffset = (uint32_t)bootloaderHdr.partition->section->Address;
                LOG_INFO("versal_2vp: Secure flow - PLM source offset set to pure PLM+PMC at 0x%08X", plmOffset);
            } else {
                // Non-secure flow: offset skips hash block
                plmOffset = (uint32_t)(bootloaderHdr.partition->section->Address + bHTable->hashBlockLength1);
                LOG_INFO("versal_2vp: Non-secure flow - PLM source offset set to PLM+PMC after hash block at 0x%08X", plmOffset);
            }
            SetSourceOffset(plmOffset);
            LOG_INFO("versal_2vp: BootHeader will point to PLM+PMC content at file offset: 0x%08X", plmOffset);
        }
        else if (bootloaderHdr.imageHeader->GetChecksumContext()->Type() != Checksum::None && bi.options.IsVersalNetSeries())
        {
            if (bootloaderHdr.imageHeader->GetAuthenticationType() != Authentication::None)
            {
                // Authentication enabled - add total AC size (includes hashblock signature)
                uint32_t acSize = GetAuthCertificateSize(bootloaderHdr);
                SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address + 
                                bHTable->hashBlockLength1 + 
                                acSize);
            }
            else if ((bootloaderHdr.imageHeader->GetEncryptContext()->Type() != Encryption::None) && 
                    (bootloaderHdr.imageHeader->GetAuthenticationType() == Authentication::None))
            {
                // No authentication, but encrypted - no GCM tag for hashblock0
                SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address + bHTable->hashBlockLength1);
            }
            else
            {
                // No authentication, no encryption
                SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address + bHTable->hashBlockLength1);
            }
        }
        else
        {
            if (bootloaderHdr.imageHeader->GetAuthenticationType() != Authentication::None)
            {
                // Authentication enabled - add total AC size (includes hashblock signature)
                uint32_t acSize = GetAuthCertificateSize(bootloaderHdr);
                SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address + 
                                bHTable->hashBlockLength1 + 
                                acSize);
            }
            else if ((bootloaderHdr.imageHeader->GetEncryptContext()->Type() != Encryption::None) && 
                    (bootloaderHdr.imageHeader->GetAuthenticationType() == Authentication::None))
            {
                // No authentication, but encrypted - no GCM tag for hashblock0
                SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address + bHTable->hashBlockLength1);
            }
            else
            {
                // No authentication, no encryption
                SetSourceOffset((uint32_t)bootloaderHdr.partition->section->Address + bHTable->hashBlockLength1);
            }
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

        SetImageHeaderByteOffset((uint32_t)iHT->section->Address);
    }

    SetHeaderChecksum(bi.options);
    //uint8_t* tmpBh = bi.bootHeader->section->Data;
    //LOG_TRACE("BH Link");
    //LOG_DUMP_BYTES(tmpBh, bi.bootHeader->section->Length);

    //Update below checks for VersalGen2 ?
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
void Versal_2vpBootHeader::LinkPrebuiltBH(BootImage& bi)
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
    SetHeaderChecksum(bi.options);
}

/******************************************************************************/
void Versal_2vpBootHeader::SetBHForPartialBootImage()
{
    SetSourceOffset(0);
    SetPlmLength(0);
    SetTotalPlmLength(0);
    SetPmcCdoLoadAddress(0);
    SetPmcCdoLength(0);
    SetTotalPmcCdoLength(0);
}

/******************************************************************************/
void Versal_2vpBootHeader::SetBHForXIP(BootImage& bi)
{
    if (bi.bifOptions->GetXipMode())
    {
        SetTotalPlmLength(0);
        /* TO-DO: Add XIP execution address checks */
    }
}

/******************************************************************************/
void Versal_2vpBootHeader::SetSmapBusWidthWords(uint32_t width)
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
void Versal_2vpBootHeader::SetWidthDetectionWord(void)
{
    bHTable->widthDetectionWord = WIDTH_DETECTION;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetIdentificationWord(void)
{
    bHTable->identificationWord = HEADER_ID_WORD;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetEncryptionKeySource(KeySource::Type keyType, BifOptions* bifOptions)
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
void Versal_2vpBootHeader::SetPmcCdoLoadAddress(uint32_t address)
{
    bHTable->pmcCdoLoadAddress = address;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetSourceOffset(uint32_t offset)
{
    bHTable->sourceOffset = offset;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetPmcCdoLength(uint32_t size)
{
    bHTable->pmcCdoLength = size;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetTotalPmcCdoLength(uint32_t size)
{
    bHTable->totalPmcCdoLength = size;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetPlmLength(uint32_t size)
{
    bHTable->plmLength = size;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetTotalPlmLength(uint32_t size)
{
    bHTable->totalPlmLength = size;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetBHAttributes(BootImage& bi)
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
void Versal_2vpBootHeader::SetBHAttributes(uint32_t attributes)
{
    bHTable->bhAttributes = attributes;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetHeaderChecksum(Options& options)
{
    /* Set the simple header checksum value by adding up all of the 32 bit
    values from the Width Detection Word (0x10) to the PUF (0xF1F)
    (968 words, 3872 bytes) */
    
    uint32_t checksumLength = GetBootHeaderSize() - sizeof(uint32_t) - sizeof(Versal_2vpSmapWidthTable);
    bHTable->headerChecksum = ComputeWordChecksum(&bHTable->widthDetectionWord, checksumLength);
}

/******************************************************************************/
void Versal_2vpBootHeader::SetGreyOrBlackKey(std::string keyFile)
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
void Versal_2vpBootHeader::SetShutterValue(uint32_t value)
{
    bHTable->shutterValue = value;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetRevokeId(uint32_t id)
{
    bHTable->plmRevokeId = id;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetPufRingOscilltorSwapConfigValue(uint32_t value)
{
    bHTable->pufRoSwapConfigVal = value;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetImageHeaderByteOffset(uint32_t address)
{
    bHTable->imageHeaderByteOffset = address;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetPlmSecureHdrIv(uint8_t* iv)
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
void Versal_2vpBootHeader::SetPmcDataSecureHdrIv(uint8_t* iv)
{
    /*if (iv == NULL)
    {
        memset(bHTable->pmcCdoSecureHdrIv, 0, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }
    else
    {
        memcpy(bHTable->pmcCdoSecureHdrIv, iv, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }*/
}

/******************************************************************************/
void Versal_2vpBootHeader::SetGreyOrBlackIv(std::string ivFile)
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
void Versal_2vpBootHeader::SetPufPDIIdentificationWord()
{
    //if(arch == Arch::LASSEN)
        bHTable->pufPDIIdentificationWord = 0;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetPufData(BootImage &bi)
{
    SetPufHDLength(0);
    uint32_t actualPufHDDataLength = bi.bootHeader->GetPufDataLength();

    uint8_t* pufDataTemp = new uint8_t[bi.bootHeader->GetPufDataLength()];
    memset(pufDataTemp, 0, bi.bootHeader->GetPufDataLength());
    if (bi.bifOptions->GetPufHdLoc() == PufHdLoc::PUFinBH || bi.bifOptions->GetPufHdinBHFlag())
    {
        if (bi.bifOptions->GetPufHelperFile() != "")
        {
            FileImport fileReader;
            if (!fileReader.LoadHexData(bi.bifOptions->GetPufHelperFile(), pufDataTemp, actualPufHDDataLength))
            {
                LOG_ERROR("Invalid no. of data bytes for PUF Helper Data.\n           Expected length for PUF Helper Data is %d bytes", actualPufHDDataLength);
            }
        }
    }
    uint32_t pufLoc = GetBootHeaderSize() - sizeof(uint32_t) - bi.bootHeader->GetPufDataLength();
    memcpy((uint8_t*)bHTable + pufLoc, pufDataTemp, bi.bootHeader->GetPufDataLength());

    delete[] pufDataTemp;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetPufHDLength(uint32_t value)
{
    bHTable->pufHDLength = value;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetHashBlockSize(uint32_t value)
{
    bHTable->hashBlockLength1 = value;
}

/******************************************************************************/
void Versal_2vpBootHeader::SetUdfBhTelluride(uint32_t file)
{
    for (int i = 0; i < UDF_BH_VERSAL_2VP; i++)
    {
        bHTable->udfBH[i] = DEFAULT_RESERVED_VERSAL;
    }
}

/******************************************************************************/
void Versal_2vpBootHeader::SetHashAlgo(BootImage &bi, uint32_t value)
{
    // For versal_2vp devices, dynamically select hash algorithm based on authentication context
    // Requirements: 0 = SHA2-384, 1 = SHA3-384, Rest = Reserved for Future use
    
    uint32_t hashAlgoValue = 0x1; // Default to SHA3-384
    
    // Get hash algorithm type from BootImage 
    AuthHash::Type hashType = bi.GetAuthHashAlgo();
    
    switch (hashType) {
        case AuthHash::Sha2:        // SHA2-256/384 algorithms
            hashAlgoValue = 0x0;    // Set to SHA2-384 for versal_2vp
            LOG_INFO("versal_2vp: Setting hash algorithm to SHA2-384 (0x0) based on auth hash type");
            break;
            
        case AuthHash::Sha3:        // SHA3-384
        case AuthHash::Sha3_256:    // SHA3-256 (treat as SHA3-384 for versal_2vp)
            hashAlgoValue = 0x1;    // Set to SHA3-384 for versal_2vp
            LOG_INFO("versal_2vp: Setting hash algorithm to SHA3-384 (0x1) based on auth hash type");
            break;
            
        case AuthHash::Shake256:    // SHAKE256 - Reserved for future use
        default:
            hashAlgoValue = 0x1;    // Default to SHA3-384
            LOG_WARNING("versal_2vp: Hash type %d not explicitly supported, defaulting to SHA3-384 (0x1)", (int)hashType);
            break;
    }
    
    bHTable->hashAlgo = hashAlgoValue;
}


// Modified : PPK header intilization moved out of BH and will be part of AC
/*

void Versal_2vpBootHeader::SetPPKSize(uint32_t actualSize, uint32_t totalSize)
{
    bHTable->actualppkSize1 = actualSize;
    bHTable->totalppkkSize1 = totalSize;
}


void Versal_2vpBootHeader::SetSignatureSize(uint32_t actualSize, uint32_t totalSize)
{
    bHTable->actualSignatureSize1 = actualSize;
    bHTable->totalSignatureSize1 = totalSize;
}
*/

/******************************************************************************/
void Versal_2vpBootHeader::SetRomReserved()
{
    int reservedLength = ROM_BH_RESERVED_VERSAL_2VP;
    for (int i = 0; i < reservedLength; i++)
    {
        bHTable->romReserved[i] = DEFAULT_RESERVED_VERSAL;
    }
}

/******************************************************************************/
void Versal_2vpBootHeader::SetPmcFwReserved()
{
    for (int i = 0; i < PLM_BH_RESERVED; i++)
    {
        bHTable->plmReserved[i] = DEFAULT_RESERVED_VERSAL;
    }
}

/******************************************************************************/
uint32_t* Versal_2vpBootHeader::GetChecksumStartPtr(void)
{
    return &bHTable->widthDetectionWord;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetHashBlockLength(void)
{
    return bHTable->hashBlockLength1;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetBootHeaderSize(void)
{
    return sizeof(Versal_2vpBootHeaderStructure);
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetBHChecksumDataSize(void)
{
    return (GetBootHeaderSize() - sizeof(uint32_t) - sizeof(Versal_2vpSmapWidthTable));
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetWidthDetectionWord(void)
{
    return bHTable->widthDetectionWord;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetIdentificationWord(void)
{
    return bHTable->identificationWord;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetSourceOffset(void)
{
    return bHTable->sourceOffset;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetHeaderChecksum(void)
{
    return bHTable->headerChecksum;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetPmcFwLength(void)
{
    return bHTable->plmLength;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetTotalPmcFwLength(void)
{
    return bHTable->totalPlmLength;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetPmcCdoLoadAddress(void)
{
    return bHTable->pmcCdoLoadAddress;
}

/******************************************************************************/
KeySource::Type Versal_2vpBootHeader::GetEncryptionKeySource(void)
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
uint32_t Versal_2vpBootHeader::GetPmcCdoLength(void)
{
    return bHTable->pmcCdoLength;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetTotalPmcCdoLength(void)
{
    return bHTable->totalPmcCdoLength;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetBHAttributes(void)
{
    return bHTable->bhAttributes;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetImageHeaderByteOffset(void)
{
    return bHTable->imageHeaderByteOffset;
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetPufDataLength()
{
    return PUF_DATA_LENGTH;
}

/******************************************************************************/
void Versal_2vpBootHeader::BuildRegInitTable(Options& options)
{
    uint32_t regTableLoc = GetBootHeaderSize() - sizeof(uint32_t) - PUF_DATA_LENGTH - (MAX_REG_INIT_VERSAL * 4);

    regTable.Build(options, (RegisterInitTable*)(section->Data.get() + regTableLoc));
}

/******************************************************************************/
uint32_t Versal_2vpBootHeader::GetXipBaseAddr(void)
{
    return LQSPI_BASE_ADDR_VERSAL;
}
