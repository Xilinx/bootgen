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
#include "imageheadertable-versal.h"
#include "bootimage.h"
#include "bootheader-versal.h"
#include "elftools.h"
#include <string.h>
#include <sstream>
#include "stringutils.h"
#include "fileutils.h"
#include "bitutils.h"
#include "bitutils-versal.h"
#include "partitionheadertable-versal.h"
#include "encryptutils.h"
#include "cJSON.h"
#include <sys/stat.h>
extern "C" {
#include "cdo-npi.h"
#include "cdo-source.h"
#include "cdo-binary.h"
#include "cdo-load.h"
#include "cdo-overlay.h"
};

static uint8_t bufferIndex = 0;
std::list<CdoCommandDmaWrite*> VersalImageHeader::cdoSections;
uint64_t slr_sbi_base_array[4] = { PMC_SBI_BUF_ADDR, SLR1_SBI_BUF_BASE_ADDR, SLR2_SBI_BUF_BASE_ADDR, SLR3_SBI_BUF_BASE_ADDR };

#define POWER_LPD       0x4210002
#define POWER_FPD       0x420c003
#define POWER_PLD       0x4220006
#define POWER_SPD       0x4214004   //POWER_NOC

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
VersalImageHeaderTable::VersalImageHeaderTable()
    : kekIvMust(false)
    , bootDevice(0)
    , creatorId(0)
    , parentId(0)
    , pdiId(0)
    , idCode(0)
    , extendedIdCode(0)
    , bypassIdCode(false)
    , prebuilt(false)
    , dpacm(DpaCM::DpaCMDisable)
{
    section = new Section("MetaHeader", sizeof(VersalImageHeaderTableStructure));
    iHTable = (VersalImageHeaderTableStructure*)section->Data;
}

/******************************************************************************/
VersalImageHeaderTable::VersalImageHeaderTable(std::ifstream& src)
{
    prebuilt = true;
    section = new Section("MetaHeader", sizeof(VersalImageHeaderTableStructure));
    iHTable = (VersalImageHeaderTableStructure*)section->Data;
    
    /* Import the Image Header Table from a boot image file */
    src.read((char*)section->Data, section->Length);
    creatorId = (iHTable->imageHeaderTableAttributes >> vihtImageCreatorIdShift) & vihtImageCreatorIdMask;
    parentId = iHTable->parentId;
    pdiId = iHTable->pdiId;
    idCode = iHTable->idCode;
    extendedIdCode = iHTable->extendedIdCode;
    bypassIdCode = (iHTable->imageHeaderTableAttributes >> vihtIdCodeCheckShift) & vihtIdCodeCheckMask;
    bootDevice = (iHTable->imageHeaderTableAttributes >> vihtSecBootDeviceShift) & vihtSecBootDeviceMask;
    dpacm = ((iHTable->imageHeaderTableAttributes >> vihtDpacmEnableShift) & vihtDpacmEnableMask) == 3 ? DpaCM::DpaCMEnable : DpaCM::DpaCMDisable;
    pufHDLoc = ((iHTable->imageHeaderTableAttributes >> vihtPufHDLocationShift) & vihtPufHDLocationMask) == 3 ? PufHdLoc::PUFinBH : PufHdLoc::PUFinEFuse;
    kekIvMust = false;
}

/******************************************************************************/
VersalImageHeaderTable::~VersalImageHeaderTable()
{
    if (section != NULL)
    {
        delete section;
    }
}

/******************************************************************************/
void VersalImageHeaderTable::Build(BootImage& bi, Binary& cache)
{
    if (section != NULL)
    {
        cache.Sections.push_back(section);
    }

    if (!prebuilt)
    {
        pdiId = bi.bifOptions->GetPdiId();
        parentId = bi.bifOptions->GetParentId();
        idCode = bi.bifOptions->GetIdCode();
        extendedIdCode = bi.bifOptions->GetExtendedIdCode();
        bypassIdCode = bi.bifOptions->GetBypassIdcodeFlag();
        bootDevice = bi.bifOptions->GetBootDevice();

        SetImageHeaderTableVersion(VERSION_v4_00_VERSAL);
        SetHeaderTablesSize();
        SetTotalMetaHdrLength(0);
        SetIdentificationString(bi.IsBootloaderFound());
        SetIds();
        metaHdrKeySrc = bi.options.bifOptions->metaHdrAttributes.encrKeySource;
        SetMetaHdrSecureHdrIv(metaHdrSecHdrIv);
        dpacm = bi.bifOptions->metaHdrAttributes.dpaCM;
        pufHDLoc = bi.bifOptions->metaHdrAttributes.pufHdLoc;
    }
    else
    {
        if (bi.bifOptions->GetPdiId() != 0)
        {
            pdiId = bi.bifOptions->GetPdiId();
        }
        if (bi.bifOptions->GetParentId() != 0)
        {
            parentId = bi.bifOptions->GetParentId();
        }

        if (bi.bifOptions->GetIdCode() != 0)
        {
            idCode = bi.bifOptions->GetIdCode();
        }
        if (bi.bifOptions->GetExtendedIdCode() != 0)
        {
            extendedIdCode = bi.bifOptions->GetExtendedIdCode();
        }

        if (bi.bifOptions->GetBypassIdcodeFlag() != false)
        {
            bypassIdCode = bi.bifOptions->GetBypassIdcodeFlag();
        }
        if (bi.bifOptions->GetBootDevice() != 0)
        {
            bootDevice = bi.bifOptions->GetBootDevice();
        }

        SetIdentificationString(bi.IsBootloaderFound());
        SetIds();

        if (bi.bifOptions->metaHdrAttributes.encrKeySource != KeySource::None)
        {
            metaHdrKeySrc = bi.options.bifOptions->metaHdrAttributes.encrKeySource;
        }
        if (bi.bifOptions->metaHdrAttributes.dpaCM != DpaCM::DpaCMDisable)
        {
            dpacm = bi.bifOptions->metaHdrAttributes.dpaCM;
        }
        if (bi.bifOptions->metaHdrAttributes.pufHdLoc != PufHdLoc::PUFinEFuse)
        {
            pufHDLoc = bi.bifOptions->metaHdrAttributes.pufHdLoc;
        }
    }

    /* Sub system Image Header creation */
    if (bi.createSubSystemPdis == true)
    {
        for (std::list<SubSysImageHeader*>::iterator subsysimage = bi.subSysImageList.begin(); subsysimage != bi.subSysImageList.end(); subsysimage++)
        {
            (*subsysimage)->Build(bi, cache);
            for (std::list<ImageHeader*>::iterator img = (*subsysimage)->imgList.begin(); img != (*subsysimage)->imgList.end(); img++)
            {
                ValidateSecurityCombinations((*img)->GetAuthenticationType(), (*img)->GetEncryptContext()->Type(),
                    (*img)->GetChecksumContext()->Type());
            }
        }
    }
    else
    {
        for (std::list<ImageHeader*>::iterator image = bi.imageList.begin(); image != bi.imageList.end(); image++)
        {
            (*image)->Build(bi, cache);
            ValidateSecurityCombinations((*image)->GetAuthenticationType(), (*image)->GetEncryptContext()->Type(),
                (*image)->GetChecksumContext()->Type());

            imageHeaderList.push_back(*image);
        }
    }

    bi.options.SetPadHeaderTable(false);
    if (bi.options.DoPadHeaderTable())
    {
        if (bi.imageList.size() == 0)
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
void VersalImageHeaderTable::Link(BootImage &bi)
{
    /* Realigning the data ptr is necessary because Section->Data ptr gets changed in the
    RSAAuthenticationContext::ResizeIfNecessary() function call */
    RealignSectionDataPtr();

    slaveBootSplitMode = (bi.bifOptions->GetSplitMode() == SplitMode::SlaveMode) ? true : false;
    fullBhSize = bi.options.bootheaderSize;
    allHdrSize = bi.options.allHeaderSize;

    if (bi.partitionHeaderTable->firstSection != NULL)
    {
        SetFirstPartitionHeaderOffset((uint32_t)bi.partitionHeaderTable->firstSection->WordAddress());
    }

    if (bi.options.bifOptions->GetHeaderEncyption())
    {
        SetFirstImageHeaderOffset(bi.encryptedHeaders->WordAddress());
        SetImageCount((uint32_t)bi.imageList.size());
    }
    else
    {
        if (bi.imageList.size() != 0)
        {
            if (bi.imageList.front()->section != NULL)
            {
                uint32_t size = (bi.imageList.size() > 0) ? (uint32_t)bi.imageList.front()->section->WordAddress() : 0;
                SetFirstImageHeaderOffset(size);
            }
            SetImageCount((uint32_t)bi.imageList.size());
        }
    }

    if (bi.createSubSystemPdis == true)
    {
        if (bi.options.bifOptions->GetHeaderEncyption())
        {
            SetFirstImageHeaderOffset(bi.encryptedHeaders->WordAddress());
            SetImageCount((uint32_t)bi.subSysImageList.size());
        }
        else if (bi.subSysImageList.size() != 0)
        {
            if (bi.subSysImageList.front()->section != NULL)
            {
                uint32_t size = (bi.subSysImageList.size() > 0) ? (uint32_t)bi.subSysImageList.front()->section->WordAddress() : 0;
                SetFirstImageHeaderOffset(size);
            }
            SetImageCount((uint32_t)bi.subSysImageList.size());
        }
    }
    SetPartitionCount((uint32_t)bi.partitionHeaderList.size());

    uint32_t acOffset = (bi.headerAC != 0) ? (bi.headerAC->section->WordAddress()) : 0;
    SetHeaderAuthCertificateOffset(acOffset);

    // Go through image list, to set the next image's offset parameters
    if (bi.createSubSystemPdis == true)
    {
        /* Sub system Image Header creation */
        for (std::list<SubSysImageHeader*>::iterator subsysimage = bi.subSysImageList.begin(); subsysimage != bi.subSysImageList.end();)
        {
            std::list<SubSysImageHeader*>::iterator prevSubSysImage = subsysimage;
            subsysimage++;
            if (subsysimage == bi.subSysImageList.end())
            {
                (*prevSubSysImage)->Link(bi, 0);
            }
            else
            {
                (*prevSubSysImage)->Link(bi, (*subsysimage));
            }
        }
    }
    else
    {
        for (std::list<ImageHeader*>::iterator currentImage = imageHeaderList.begin(); currentImage != imageHeaderList.end(); )
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
    }
    if (prebuilt)
    {
        SetTotalMetaHdrLength(bi.imageHeaderTable->metaHeaderLength);
    }
    SetBootDevice(bi.bifOptions->GetBootDevice());
    SetBootDeviceAddress(bi.bifOptions->GetBootDeviceAddress());
    SetMetaHdrKeySrc(bi.imageHeaderTable->metaHdrKeySrc, bi.bifOptions);
    SetMetaHdrGreyOrBlackIv(kekIvFile);
    SetImageHeaderTableAttributes();
    SetReservedFields();
    SetChecksum();
}

/******************************************************************************/
void VersalImageHeaderTable::SetImageHeaderTableVersion(uint32_t version)
{
    iHTable->version = version;
}

/******************************************************************************/
void VersalImageHeaderTable::SetHeaderTablesSize()
{
    iHTable->headerSizes = (sizeof(VersalImageHeaderTableStructure)/4) | (sizeof(VersalImageHeaderStructure)/4 << 8) | (sizeof(VersalPartitionHeaderTableStructure)/4 << 16);
}

/******************************************************************************/
void VersalImageHeaderTable::SetTotalMetaHdrLength(uint32_t size)
{
    iHTable->totalMetaHdrLength = size / sizeof(uint32_t);
}

/******************************************************************************/
void VersalImageHeaderTable::SetPartitionCount(uint32_t count)
{
    iHTable->partitionTotalCount = count;
}

/******************************************************************************/
void VersalImageHeaderTable::SetImageCount(uint32_t count)
{
    iHTable->imageTotalCount = count;
}

/******************************************************************************/
void VersalImageHeaderTable::SetFirstImageHeaderOffset(uint32_t offset)
{
    iHTable->firstImageHeaderWordOffset = offset;
    if (slaveBootSplitMode && (iHTable->firstImageHeaderWordOffset != 0))
    {
        iHTable->firstImageHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void VersalImageHeaderTable::SetFirstPartitionHeaderOffset(uint32_t offset)
{
    iHTable->firstPartitionHeaderWordOffset = offset;
    if (slaveBootSplitMode && (iHTable->firstPartitionHeaderWordOffset != 0))
    {
        iHTable->firstPartitionHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void VersalImageHeaderTable::SetBootDeviceAddress(uint32_t address)
{
    iHTable->secondaryBootDeviceAddress = address;
}

/******************************************************************************/
void VersalImageHeaderTable::SetIds()
{
    if (idCode == 0)
    {
        LOG_WARNING("id_code is not specified in BIF, default id code is 0x04ca8093 (s80 device)");
        idCode = DEFAULT_ID_CODE_S80;
    }
    iHTable->idCode = idCode;
    iHTable->extendedIdCode = extendedIdCode;
    iHTable->parentId = parentId;
    iHTable->pdiId = pdiId;
}

/******************************************************************************/
void VersalImageHeaderTable::SetImageHeaderTableAttributes()
{
    iHTable->imageHeaderTableAttributes = bypassIdCode;
    iHTable->imageHeaderTableAttributes |= ((creatorId & 0x0F) << 1);
    iHTable->imageHeaderTableAttributes |= ((bootDevice & vihtSecBootDeviceMask) << vihtSecBootDeviceShift);
    iHTable->imageHeaderTableAttributes |= ((dpacm & vihtDpacmEnableMask) << vihtDpacmEnableShift);
    iHTable->imageHeaderTableAttributes |= ((pufHDLoc & vihtPufHDLocationMask) << vihtPufHDLocationShift);
}

/******************************************************************************/
void VersalImageHeaderTable::SetMetaHdrSecureHdrIv(uint8_t* iv)
{
    if (iv == NULL)
    {
        memset(iHTable->metaHdrSecureHdrIv, 0, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }
    else
    {
        memcpy(iHTable->metaHdrSecureHdrIv, iv, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }
}

/******************************************************************************/
void VersalImageHeaderTable::SetMetaHdrKeySrc(KeySource::Type keyType, BifOptions* bifOptions)
{
    //Encryption key source, only key source used for PLM is valid for meta header
    kekIvMust = false;
    switch (keyType)
    {
    case KeySource::EfuseRedKey:
        iHTable->metaHdrKeySource = EFUSE_RED_KEY;
        break;

    case KeySource::BbramRedKey:
        iHTable->metaHdrKeySource = BBRAM_RED_KEY;
        break;

    case KeySource::EfuseBlkKey:
        iHTable->metaHdrKeySource = EFUSE_BLK_KEY;
        kekIvFile = bifOptions->GetEfuseKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'efuse_kek_iv' is mandatory with 'keysrc=efuse_blk_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::BbramBlkKey:
        iHTable->metaHdrKeySource = BBRAM_BLK_KEY;
        kekIvFile = bifOptions->GetBbramKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'bbram_kek_iv' is mandatory with 'keysrc=bbram_blk_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::BhBlkKey:
        iHTable->metaHdrKeySource = BH_BLACK_KEY;
        kekIvFile = bifOptions->GetBHKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'bh_kek_iv' is mandatory with 'keysrc=bh_blk_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::EfuseGryKey:
        iHTable->metaHdrKeySource = EFUSE_GRY_KEY;
        kekIvFile = bifOptions->GetEfuseKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'efuse_kek_iv' is mandatory with 'keysrc=efuse_gry_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::BbramGryKey:
        iHTable->metaHdrKeySource = BBRAM_GRY_KEY;
        kekIvFile = bifOptions->GetBbramKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'bbram_kek_iv' is mandatory with 'keysrc=bbram_gry_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::BhGryKey:
        iHTable->metaHdrKeySource = BH_GRY_KEY;
        kekIvFile = bifOptions->GetBHKekIVFile();
        if (kekIvFile == "")
        {
            LOG_ERROR("'bh_kek_iv' is mandatory with 'keysrc=bh_gry_key'");
        }
        kekIvMust = true;
        break;

    case KeySource::None:
        iHTable->metaHdrKeySource = 0;
        break;

    default:
        LOG_ERROR("BIF attribute error !!!\n\t\tInvalid 'keysrc' mentioned for MetaHeader.");
        break;
    }
}

/******************************************************************************/
void VersalImageHeaderTable::SetMetaHdrGreyOrBlackIv(std::string ivFile)
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

    memcpy(&iHTable->metaHdrGreyOrBlackIV, ivData, IV_LENGTH * 4);
    delete[] ivData;
}

/******************************************************************************/
void VersalImageHeaderTable::SetHeaderAuthCertificateOffset(uint32_t address)
{
    iHTable->headerAuthCertificateWordOffset = address;
}

/******************************************************************************/
void VersalImageHeaderTable::SetPdiId(uint32_t id)
{
    pdiId = id;
}

/******************************************************************************/
void VersalImageHeaderTable::SetParentId(uint32_t id)
{
    parentId = id;
}

/******************************************************************************/
void VersalImageHeaderTable::SetIdentificationString(bool bootloaderFound)
{
    if (bootloaderFound)
    {
        iHTable->identificationString = FPDI;
    }
    else
    {
        iHTable->identificationString = PPDI;
    }
}

/******************************************************************************/
void VersalImageHeaderTable::SetCreatorId(uint8_t id)
{
    creatorId = id;
}

/******************************************************************************/
void VersalImageHeaderTable::SetReservedFields(void)
{
    memset(&iHTable->reserved, 0x00, MAX_IHT_RESERVED_VERSAL * sizeof(uint32_t));
}

/******************************************************************************/
void VersalImageHeaderTable::SetChecksum(void)
{
    iHTable->ihtChecksum = ComputeWordChecksum(iHTable, sizeof(VersalImageHeaderTableStructure) - sizeof(iHTable->ihtChecksum) /* bytes*/);
}

/******************************************************************************/
void VersalImageHeaderTable::RealignSectionDataPtr(void)
{
    iHTable = (VersalImageHeaderTableStructure*)section->Data;
}

/******************************************************************************/
uint32_t VersalImageHeaderTable::GetImageHeaderTableVersion(void)
{
    return iHTable->version;
}

/******************************************************************************/
uint32_t VersalImageHeaderTable::GetPartitionCount(void)
{
    return iHTable->partitionTotalCount;
}

/******************************************************************************/
uint32_t VersalImageHeaderTable::GetImageCount(void)
{
    return iHTable->imageTotalCount;
}

/******************************************************************************/
uint32_t VersalImageHeaderTable::GetFirstImageHeaderOffset(void)
{
    return iHTable->firstImageHeaderWordOffset;
}

/******************************************************************************/
uint32_t VersalImageHeaderTable::GetTotalMetaHdrLength()
{
    return (iHTable->totalMetaHdrLength) * sizeof(uint32_t);
}

/******************************************************************************/
void VersalImageHeaderTable::ValidateSecurityCombinations(Authentication::Type authType, Encryption::Type encryptType,
                                Checksum::Type cksumType)
{
    if (cksumType != Checksum::None)
    {
        if (authType != Authentication::None)
        {
            LOG_ERROR("Checksum is not supported with Authentication");
        }
        if (encryptType != Encryption::None)
        {
            LOG_ERROR("Checksum is not supported with Encryption");
        }
    }
}

/******************************************************************************/
VersalImageHeader::VersalImageHeader(std::string& filename)
    : ImageHeader(filename)
    , imageHeader(NULL)
    , cdoHeader(NULL)
    , aie_array_base_address(AIE_BASE_ADDR)
    , coreBaseAddr(0)
    , southBankBaseAddr(0)
    , westBankBaseAddr(0)
    , northBankBaseAddr(0)
    , eastBankBaseAddr(0)
    , num_of_slrs(0)
{
    Name = StringUtils::BaseName(filename);
    uint32_t size = sizeof(VersalImageHeaderStructure);

    section = new Section("ImageHeader " + Name, size);
    memset(section->Data, 0, size);
}

/******************************************************************************/
VersalImageHeader::VersalImageHeader(uint8_t* data, uint64_t len)
    : ImageHeader(data, len)
    , imageHeader(NULL)
    , cdoHeader(NULL)
    , aie_array_base_address(AIE_BASE_ADDR)
    , coreBaseAddr(0)
    , southBankBaseAddr(0)
    , westBankBaseAddr(0)
    , northBankBaseAddr(0)
    , eastBankBaseAddr(0)
    , num_of_slrs(0)
{
    Name = "Buffer" + StringUtils::Format(".%d", bufferIndex++);
    uint32_t size = sizeof(VersalImageHeaderStructure);

    section = new Section("ImageHeader " + Name, size);
    memset(section->Data, 0, size);
}

/******************************************************************************/
VersalImageHeader::VersalImageHeader(std::ifstream& ifs, bool IsBootloader)
    : ImageHeader(ifs)
    , imageHeader(NULL)
    , cdoHeader(NULL)
    , aie_array_base_address(AIE_BASE_ADDR)
    , coreBaseAddr(0)
    , southBankBaseAddr(0)
    , westBankBaseAddr(0)
    , northBankBaseAddr(0)
    , eastBankBaseAddr(0)
    , num_of_slrs(0)
{
    bool firstValidHdr = false;
    VersalImageHeaderStructure importedIH;

    long pos = ifs.tellg();
    ifs.read((char*)&importedIH, 4 * sizeof(uint32_t));

    char buffer[4];
    char name[256];
    uint32_t i = 0;

    do
    {
        ifs.read(buffer, 4);
        name[i + 0] = buffer[0];
        name[i + 1] = buffer[1];
        name[i + 2] = buffer[2];
        name[i + 3] = buffer[3];
        i += 4;
    } while (buffer[0] && buffer[1] && buffer[2] && buffer[3] && i<sizeof(name));

    if (i >= sizeof(name))
    {
        LOG_DEBUG(DEBUG_STAMP, "Image header name too long");
        LOG_ERROR("Failure parsing imported bootimage");
    }

    Name = name;
    uint32_t size = sizeof(VersalImageHeaderStructure);

    ifs.seekg(pos);
    section = new Section("ImageHeader " + Name, size);
    imageHeader = (VersalImageHeaderStructure*)section->Data;
    ifs.read((char*)imageHeader, size);
    long count = imageHeader->dataSectionCount;
    long offset = imageHeader->partitionHeaderWordOffset * sizeof(uint32_t);

    for (uint8_t index = 0; index<count; index++)
    {
        Bootloader = IsBootloader;

        VersalPartitionHeader* hdr = new VersalPartitionHeader(this, index);
        if (!firstValidHdr)
        {
            hdr->firstValidIndex = true;
            firstValidHdr = true;
        }
        ifs.seekg(offset);
        hdr->ReadHeader(ifs);
        hdr->ReadData(ifs);
        if (hdr->GetPartitionKeySource())
        {
            hdr->preencrypted = true;
        }
        partitionHeaderList.push_back(hdr);

        Alignment = 0;
        Offset = 0;
        Reserve = 0;

        destCpu = (DestinationCPU::Type)hdr->GetDestinationCpu();
        exceptionLevel = (ExceptionLevel::Type)hdr->GetExceptionLevel();
        trustzone = (TrustZone::Type)hdr->GetTrustZone();
        early_handoff = hdr->GetEarlyHandoff();
        hivec = hdr->GetHivec();
        partitionType = hdr->GetPartitionType();
        PartOwner = (PartitionOwner::Type)hdr->GetOwnerType();
        dpacm = hdr->GetDpaCMFlag();
        pufHdLoc = hdr->GetPufHdLocation();
        offset += hdr->GetPartitionHeaderSize();
    }
}

/******************************************************************************/
VersalImageHeader::VersalImageHeader(std::ifstream& ifs, VersalImageHeaderStructure* importedIH, bool IsBootloader, uint32_t img_index)
    : ImageHeader(ifs)
    , imageHeader(NULL)
    , coreBaseAddr(0)
    , southBankBaseAddr(0)
    , westBankBaseAddr(0)
    , northBankBaseAddr(0)
    , eastBankBaseAddr(0)
    , num_of_slrs(0)
{
    bool firstValidHdr = false;

    Name = importedIH->imageName;
    uint32_t size = sizeof(VersalImageHeaderStructure);

    section = new Section("ImageHeader " + Name, size);
    imageHeader = (VersalImageHeaderStructure*)section->Data;
    memcpy(imageHeader, importedIH, size);
    
    long offset =  (imageHeader->partitionHeaderWordOffset * sizeof(uint32_t)) + (img_index * sizeof(VersalPartitionHeaderTableStructure));

    VersalPartitionHeaderTableStructure* tempPHT = new VersalPartitionHeaderTableStructure;
    ifs.seekg(offset);
    ifs.read((char*)tempPHT, sizeof(VersalPartitionHeaderTableStructure));
    uint32_t count = tempPHT->dataSectionCount;
    delete tempPHT;

    for (uint8_t index = 0; index<count; index++)
    {
        Bootloader = IsBootloader;

        VersalPartitionHeader* hdr = new VersalPartitionHeader(this, index);
        if (!firstValidHdr)
        {
            hdr->firstValidIndex = true;
            firstValidHdr = true;
        }
        ifs.seekg(offset);
        hdr->ReadHeader(ifs);
        hdr->ReadData(ifs);
        if (hdr->GetPartitionKeySource())
        {
            hdr->preencrypted = true;
        }
        partitionHeaderList.push_back(hdr);

        Alignment = 0;
        Offset = 0;
        Reserve = 0;

        destCpu = (DestinationCPU::Type)hdr->GetDestinationCpu();
        exceptionLevel = (ExceptionLevel::Type)hdr->GetExceptionLevel();
        trustzone = (TrustZone::Type)hdr->GetTrustZone();
        early_handoff = hdr->GetEarlyHandoff();
        hivec = hdr->GetHivec();
        partitionType = hdr->GetPartitionType();
        PartOwner = (PartitionOwner::Type)hdr->GetOwnerType();
        dpacm = hdr->GetDpaCMFlag();
        pufHdLoc = hdr->GetPufHdLocation();
        offset += hdr->GetPartitionHeaderSize();
    }
}
/******************************************************************************/
VersalImageHeader::~VersalImageHeader()
{
    if (section != NULL)
    {
        delete section;
    }
}

/******************************************************************************/
uint32_t VersalImageHeader::GetImageHeaderAttributes(void)
{
    return imageHeader->imageAttributes;
}

/******************************************************************************/
void VersalImageHeader::SetAuthBlock(size_t blockSize, bool flag)
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
void VersalImageHeader::SetPartitionUid(uint32_t id)
{
    partitionUid = id;
}

/******************************************************************************/
void VersalImageHeader::SetEncryptionKeySrc(KeySource::Type type)
{
    keySrc = type;
    if (type != KeySource::None)
    {
        LOG_TRACE("Setting Encryption Key Source as %d", type);
    }
}

/******************************************************************************/
void VersalImageHeader::SetImageId(void)
{
    /* This is for the non-subsystem flow. BOOTGEN_SUBSYSTEM_PDI = 0
    Place image IDs as per the new subsystem guidelines i.e. 0x1c000001 for PMC subsystem and
    0x1c000000 for default subsystems */
    if (IsBootloader())
    {
        imageHeader->imageId = 0x1c000001;
    }
    else
    {
        imageHeader->imageId = 0x1c000000;
    }
}

/******************************************************************************/
void VersalImageHeader::SetPartitionRevocationId(uint32_t id)
{
    partitionRevokeId = id;
}

/******************************************************************************/
void VersalImageHeader::SetMemCopyAddr()
{
    if (ihMemCpyAddr == 0xFFFFFFFFFFFFFFFF)
    {
        ihMemCpyAddr = 0;
    }
    imageHeader->memcpyAddressLo = ihMemCpyAddr;
    imageHeader->memcpyAddressHi = ihMemCpyAddr >> 32;
}

/******************************************************************************/
uint32_t VersalImageHeader::GetPartitionUid(void)
{
    return partitionUid;
}

/******************************************************************************/
KeySource::Type VersalImageHeader::GetEncryptionKeySrc(void)
{
    return keySrc;
}

/******************************************************************************/
uint32_t VersalImageHeader::GetPartitionRevocationId()
{
    return partitionRevokeId;
}

/******************************************************************************/
DpaCM::Type VersalImageHeader::GetDpacm(void)
{
    return dpacm;
}

/******************************************************************************/
PufHdLoc::Type VersalImageHeader::GetPufHdLocation(void)
{
    return pufHdLoc;
}

/******************************************************************************/
std::string VersalImageHeader::GetKekIV(void)
{
    return kekIvFile;
}

/******************************************************************************/
void VersalImageHeader::Build(BootImage& bi, Binary& cache)
{
    if ((bi.subSysImageList.size() == 0) || (bi.createSubSystemPdis == false))
    {
        if (section != NULL)
        {
            bi.headers.push_back(section);
            bi.imageHeaderTable->metaHeaderLength += section->Length;
        }
    }

    uint32_t defaultAlignment = bi.options.GetDefaultAlignment();

    if (imageHeader == NULL)
    {
        imageHeader = (VersalImageHeaderStructure*)section->Data;
        SetImageName();
        SetImageHeaderAttributes();
        SetDataSectionCount(0);
        SetPartitionHeaderOffset(0);
        SetMetaHdrRevokeId(bi.bifOptions->metaHdrAttributes.revokeId);
        SetImageId();
        SetMemCopyAddr();

        if (Alignment.IsSet() && Offset.IsSet())
        {
            LOG_ERROR("[alignment] and [offset] attributes are mutually exclusive");
        }
        if (Alignment.IsSet() && (Alignment.Value() & (defaultAlignment - 1)))
        {
            LOG_ERROR("Alignment (0x%X) is not a multiple of %d", Alignment.Value(), defaultAlignment);
        }
        if (Reserve.IsSet() && (Reserve.Value()  & (defaultAlignment - 1)))
        {
            LOG_ERROR("Reserve (0x%X) is not a multiple of %d", Reserve.Value(), defaultAlignment);
        }
        if (Offset.IsSet() && (Offset.Value() & (defaultAlignment - 1)))
        {
            LOG_ERROR("Offset (0x%X) is not a multiple of %d", Offset.Value(), defaultAlignment);
        }

        partitionHeaderList.clear();

        ParseFileToImport(bi);

        SetDataSectionCount(partitionHeaderList.size());
    }
    bi.partitionHeaderList.insert(bi.partitionHeaderList.end(), partitionHeaderList.begin(), partitionHeaderList.end());
}

/******************************************************************************/
bool IsElf(std::string line)
{
    bool status = false;
    if ((line.at(0) == 0x7f) && (line.at(1) == 0x45) && (line.at(2) == 0x4C) && (line.at(3) == 0x46))
    {
        status = true;
    }
    return status;
}

/******************************************************************************/
void VersalImageHeader::ImportBin(BootImage& bi)
{
    SetDomain(Domain::PS);
    Binary::Address_t load_addr, exec_addr;

    ByteFile data(Filename);

    if (Load.IsSet())
    {
        load_addr = Load.Value();
    }
    else
    {
        load_addr = 0;
    }

    if (Startup.IsSet())
    {
        exec_addr = Startup.Value();
    }
    else
    {
        exec_addr = 0;
    }

    uint32_t dataValue;
    uint32_t alignlen = data.len + ((4 - (data.len & 3)) & 3);
    uint8_t* tempBuffer = new uint8_t[alignlen];

    if (GetPartitionType() == PartitionType::CONFIG_DATA_OBJ)
    {
        dataValue = ReadBigEndian32(data.bytes + 4);
        bool change_endianness = false;
        if ((dataValue != 0x584c4e58) && (dataValue != 0x584e4c58) && (dataValue != 0x004f4443) && (dataValue != 0x43444f00))
        {
            LOG_ERROR("Invalid CDO format - incorrect identification word (XLNX/CDO) - 0x%x", dataValue);
        }
        /* Read ID value (XNLX/CDO) from CDO Header to identify endianness */
        if (dataValue == 0x584c4e58 || dataValue == 0x004f4443)
        {
            /* Convert BE CDOs to LE */
            change_endianness = true;
            LOG_WARNING("CDO is in Big Endian format. Big Endian CDOs are deprecated. Please use little endian CDOs.");
        }

        for (uint32_t index = 0; index < data.len; index += 4)
        {
            dataValue = ReadBigEndian32(data.bytes + index);
            if (change_endianness)
            {
                WriteLittleEndian32(tempBuffer + index, dataValue);
            }
            else
            {
                WriteBigEndian32(tempBuffer + index, dataValue);
            }
        }
        if (PostProcessCdo(tempBuffer, data.len)) return;
        load_addr = 0xFFFFFFFF;
        exec_addr = 0;
    }
    else if (GetPartitionType() == PartitionType::CFI)
    {
        uint32_t value[4] = { 0 };
        /* Rearrange data in CFI files as follows
        CFI File: D6 00 00 52 BE BA FE C0 BE BA FE C0 BE BA FE C0
        PDI File: C0 FE BA BE C0 FE BA BE C0 FE BA BE 52 00 00 D6
        */

        for (uint32_t index = 0; index < data.len; index += 16)
        {
            /* Read 32-bit word, change endianness
            Swap positions of 4 32-bit words */
            for (uint8_t i = 0; i < 4; i++)
            {
                value[i] = ReadBigEndian32(data.bytes + index + (4 * i));
                WriteLittleEndian32(tempBuffer + index + 4 * (3 - i), value[i]);
            }
        }
        exec_addr = 0;
    }
    else
    {
        memcpy(tempBuffer, data.bytes, alignlen);
    }
    PartitionHeader* hdr = new VersalPartitionHeader(this, 0);
    hdr->firstValidIndex = true;
    hdr->loadAddress = load_addr;
    hdr->execAddress = exec_addr;
    if(updateReserveInPh == true)
    {
        if(Reserve.IsSet())
        {
            if(alignlen > Reserve.Value())
            {
                LOG_WARNING("Total Partition length is more than Reserve Length. Hence reserve attribute is ignored.");
            }
            else
            {
                alignlen = Reserve.Value();
            }
        }
    }
    hdr->partition = new VersalPartition(hdr, tempBuffer, alignlen);
    hdr->partitionSize = alignlen;
    delete[] tempBuffer;
    SetLoadAndExecAddress(hdr);
    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
void VersalImageHeader::ImportBuffer(BootImage& bi)
{
    SetDomain(Domain::PS);

    PartitionHeader* hdr = new VersalPartitionHeader(this, 0);
    hdr->firstValidIndex = true;

    hdr->execAddress = Startup.ValueOrDefault(0);
    if (Load.IsSet())
    {
        hdr->loadAddress = Load.Value();
    }
    else
    {
        hdr->loadAddress = 0;
        if ((GetPartitionType() == PartitionType::CFI) || (GetPartitionType() == PartitionType::NPI) || (GetPartitionType() == PartitionType::CONFIG_DATA_OBJ))
        {
            SetDomain(Domain::PL);
        }
    }
    if(updateReserveInPh)
    {
        if(Reserve.IsSet())
        {
            if(bufferSize > Reserve.Value())
            {
                LOG_WARNING("Total Partition length is more than Reserve Length. Hence reserve attribute is ignored.");
            }
            else
            {
                bufferSize = Reserve.Value();
            }
        }
    }
    hdr->partition = new VersalPartition(hdr, buffer, bufferSize);
    hdr->partitionSize = bufferSize;
    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
void VersalImageHeader::ImportNpi(BootImage& bi)
{
    SetPartitionType(PartitionType::CONFIG_DATA_OBJ);
    if (Bootloader)
    {
        LOG_ERROR("NPI can not be marked [bootloader]");
    }

    /* Parse the bitstream, and set the bit file name for usage by other features
    such as '-process_bitstream' */
    std::ifstream stream(Filename.c_str(), std::ios_base::binary);
    if (!stream)
    {
        LOG_ERROR("Cannot read NPI file - %s ", Filename.c_str());
    }
    BitFile *bit = new VersalBitFile(stream);
    bit->ParseBit(bi);
    bi.bitFilename = Filename.c_str();

    /* The endianess is different (Big Endian) in case of Zynq MP/FPGA encryption cases. All other cases the
    the bitstream is copied as Little Endian */
    OutputStream *os = bit->GetOutputStreamType();
    bit->CopyNpi(os);

    /* Bitstream sizes should be word aligned. Otherwise bitstream is invalid */
    if (os->Size() % 4)
    {
        LOG_DEBUG(DEBUG_STAMP, "BIT stream %s does not have a word aligned number of bytes (0xX)", Name.c_str(), (uint32_t)os->Size());
        LOG_ERROR("BIT file parsing error !!!");
    }

    /* As bitstreams are configured in the PL section
    There should be no load / start-up (execution) addresses in case of bitstreams. */
    if (this->Startup.IsSet())
    {
        LOG_ERROR("[startup=...] attribute not supported for BIT partition - %s", this->Name.c_str());
    }

    if (PostProcessCdo(os->Start(), os->Size())) return;

    PartitionHeader* hdr = new VersalPartitionHeader(this, 0);
    hdr->firstValidIndex = true;
    hdr->execAddress = 0;

    /* Padding of the partition, based on encryption */
    hdr->partitionSize = os->Size();

    // Larger size, if needed.
    hdr->transferSize = os->Size();
    hdr->preservedBitstreamHdr = os->pHdr;

    hdr->partition = new VersalPartition(hdr, os->Start(), os->Size());

    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
void VersalImageHeader::ImportBit(BootImage& bi)
{
    /* Bitstream can never be marked as a bootloader */
    if (Bootloader)
    {
        LOG_ERROR("Bit stream can not be marked [bootloader]");
    }

    static bool cfi_gsc_mask = false;
    if (GetPartitionType() == PartitionType::CFI_GSC)
    {
        if (!cfi_gsc_mask)
        {
            SetPartitionType(PartitionType::CFI_GSC_UNMASK);
            cfi_gsc_mask = true;
        }
        else if (cfi_gsc_mask)
        {
            SetPartitionType(PartitionType::CFI_GSC);
            cfi_gsc_mask = false;
        }
    }
    else
    {
        SetPartitionType(PartitionType::CFI);
    }

    /* Parse the bitstream, and set the bit file name for usage by other features
    such as '-process_bitstream' */
    std::ifstream stream(Filename.c_str(), std::ios_base::binary);
    if (!stream)
    {
        LOG_ERROR("Cannot read BIT file - %s ", Filename.c_str());
    }
    BitFile *bit = new VersalBitFile(stream);
    bit->ParseBit(bi);
    bi.bitFilename = Filename.c_str();

    /* The endianess is different (Big Endian) in case of Zynq MP/FPGA encryption cases. All other cases the
    the bitstream is copied as Little Endian */
    OutputStream *os = bit->GetOutputStreamType();
    bit->Copy(os);

    /* Bitstream sizes should be word aligned. Otherwise bitstream is invalid */
    if (os->Size() % 4)
    {
        LOG_DEBUG(DEBUG_STAMP, "BIT stream %s does not have a word aligned number of bytes (0xX)", Name.c_str(), (uint32_t)os->Size());
        LOG_ERROR("BIT file parsing error !!!");
    }


    /* As bitstreams are configured in the PL section
       There should be no load / start-up (execution) addresses in case of bitstreams. */
    if (this->Startup.IsSet())
    {
        LOG_ERROR("[startup=...] attribute not supported for BIT partition - %s", this->Name.c_str());
    }

    PartitionHeader* hdr = new VersalPartitionHeader(this, 0);
    hdr->firstValidIndex = true;
    hdr->execAddress = 0;

    /* Padding of the partition, based on encryption */
    hdr->partitionSize = os->Size();

    // Larger size, if needed.
    hdr->transferSize = os->Size();
    hdr->preservedBitstreamHdr = os->pHdr;
    SetPartitionType(PartitionType::CFI);
    hdr->partition = new VersalPartition(hdr, os->Start(), os->Size());

    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
void VersalImageHeader::ParseFileToImport(BootImage& bi)
{
    if (destCpu == DestinationCPU::AIE)
    {
        if (bi.convertAieElfToCdo == true)
        {
            CreateAieEnginePartition(bi);
            LOG_INFO("AIE partitions are created by converting AIE elfs to CDOs");
        }
        else
        {
            ImportAieEngineElf(bi);
            LOG_INFO("AIE partitions are created using AIE elfs");
        }
    }
    else if (partitionType == PartitionType::SLR_BOOT)
    {
        CreateSlrBootPartition(bi);
    }
    else if (partitionType == PartitionType::SLR_CONFIG)
    {
        CreateSlrConfigPartition(bi);
    }
    else
    {
        if (Filename != "")
        {
            if (FileUtils::GetFileSize(Filename) == 0)
            {
                LOG_ERROR("Cannot read empty file - %s ", Filename.c_str());
            }
            if ((filelist.size() > 1) && (GetPartitionType() != PartitionType::CONFIG_DATA_OBJ))
            {
                LOG_ERROR("File for merging is not in CDO format - %s", Filename.c_str());
            }
            LOG_INFO("Parsing file - %s", filelist.at(0).c_str());
            std::ifstream stream(Filename.c_str(), std::ios_base::binary);
            if (!stream)
            {
                LOG_ERROR("Cannot read file - %s ", Filename.c_str());
            }
            std::string line;
            while(line == "")
            {
                getline(stream, line);
            }

            if (IsElf(line) && (partitionType != PartitionType::RAW))
            {
                ImportElf(bi);
            }
            else if ((line.find("Xilinx ASCII Bitstream") != std::string::npos) || (line.find("Xilinx ASCII CFI Deviceimage") != std::string::npos))
            {
                ImportBit(bi);
            }
            else if ((line.find("Xilinx ASCII NPI Deviceimage") != std::string::npos) || (line.find("Xilinx ASCII PSAXIMM Deviceimage") != std::string::npos)
                     || (line.find("version") != std::string::npos) || (GetPartitionType() == PartitionType::CONFIG_DATA_OBJ))
            {
                ImportCdo(bi);
            }
            else
            {
                if (IsElf(line))
                {
                    SetPartitionType(PartitionType::RAW_ELF);
                }
                else if (GetPartitionType() != PartitionType::RESERVED)
                {
                    SetPartitionType(GetPartitionType());
                }
                else
                {
                    SetPartitionType(PartitionType::RAW);
                }
                ImportBin(bi);
            }
        }
        else
        {
            ImportBuffer(bi);
        }
    }
}

/******************************************************************************/
void VersalImageHeader::ImportCdoSource(BootImage& bi)
{
    uint8_t* buffer = NULL;
    size_t size = 0;
    buffer = DecodeCdo(Filename, &size);
    if (PostProcessCdo(buffer, size)) return;
    PartitionHeader* hdr = new VersalPartitionHeader(this, 0);
    hdr->firstValidIndex = true;
    hdr->loadAddress = 0xFFFFFFFFFFFFFFFF;
    hdr->execAddress = 0;

    hdr->partition = new VersalPartition(hdr, buffer, size);
    hdr->partitionSize = size;
    delete[] buffer;
    SetLoadAndExecAddress(hdr);
    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
void VersalImageHeader::ImportCdo(BootImage& bi)
{
    uint8_t* buffer = NULL;
    size_t size = 0;

    if (filelist.size() > 0)
    {
        ParseCdos(bi, filelist, &buffer, &size);
    }
    SetPartitionType(PartitionType::CONFIG_DATA_OBJ);
    PartitionHeader* hdr = new VersalPartitionHeader(this, 0);
    hdr->firstValidIndex = true;
    hdr->loadAddress = 0xFFFFFFFFFFFFFFFF;
    hdr->execAddress = 0;
    if(updateReserveInPh == true)
    {
        if(Reserve.IsSet())
        {
            if(size > Reserve.Value())
            {
                LOG_WARNING("Total Partition length is more than Reserve Length. Hence reserve attribute is ignored.");
            }
            else
            {
                size = Reserve.Value();
            }
        }
    }
    hdr->partition = new VersalPartition(hdr, buffer, size);
    hdr->partitionSize = hdr->transferSize = size;
    delete[] buffer;
    SetLoadAndExecAddress(hdr);
    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
uint8_t* VersalImageHeader::DecodeCdo(std::string file, size_t* size)
{
    uint8_t* buffer = NULL;
    FILE* fp = fopen(file.c_str(), "r");
    CdoSequence* seq = cdoseq_from_source(fp);
    fclose(fp);
    if (seq == NULL)
    {
        LOG_ERROR("Error decoding CDO file %s", file.c_str());
    }
    buffer = (uint8_t*)cdoseq_to_binary(seq, size, 0);
    return buffer;
}

/******************************************************************************/
void VersalImageHeader::Link(BootImage &bi, PartitionHeader* partitionHeader, ImageHeader* nextImageHeader)
{
    slaveBootSplitMode = (bi.bifOptions->GetSplitMode() == SplitMode::SlaveMode) ? true : false;
    fullBhSize = bi.options.bootheaderSize;
    allHdrSize = bi.options.allHeaderSize;

    imageHeader = (VersalImageHeaderStructure*)section->Data;
    if (partitionHeader->section != NULL)
    {
        SetPartitionHeaderOffset((uint32_t)partitionHeader->section->Address);
    }

    SetImageId();
    SetReservedFields();
    SetChecksum();
}

/******************************************************************************/
void VersalImageHeader::SetMetaHdrRevokeId(uint32_t revokeId)
{
    imageHeader->metaHdrRevokeId = revokeId;
}

/******************************************************************************/
void VersalImageHeader::SetReservedFields(void)
{
    imageHeader->reserved = 0x00;
}

/******************************************************************************/
void VersalImageHeader::SetChecksum(void)
{
    imageHeader->ihChecksum = ComputeWordChecksum(imageHeader, sizeof(VersalImageHeaderStructure) - sizeof(imageHeader->ihChecksum) /* bytes*/);
}

/******************************************************************************/
void VersalImageHeader::SetPartitionHeaderOffset(uint32_t addr)
{
    imageHeader->partitionHeaderWordOffset = addr / sizeof(uint32_t);
    if (slaveBootSplitMode && (imageHeader->partitionHeaderWordOffset != 0))
    {
        imageHeader->partitionHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void VersalImageHeader::SetDataSectionCount(uint32_t cnt)
{
    imageHeader->dataSectionCount = cnt;
}

/******************************************************************************/
void VersalImageHeader::SetImageHeaderAttributes()
{
    if (GetPartOwner() != PartitionOwner::FSBL)
    {
        imageHeader->imageAttributes = 1 << 3;
    }
    else
    {
        imageHeader->imageAttributes = 0;
    }
    if (ihDelayLoad)
    {
        imageHeader->imageAttributes |= (1 << vihDelayLoadShift);
    }
    if (ihDelayHandoff)
    {
        imageHeader->imageAttributes |= (1 << vihDelayHandoffShift);
    }
    if (ihMemCpyAddr != 0xFFFFFFFFFFFFFFFF)
    {
        imageHeader->imageAttributes |= (1 << vihCopyToMemoryShift);
    }
}

/******************************************************************************/
void VersalImageHeader::SetImageName(void)
{
    /* Only 16-bytes available to store the name
    name length <= 16 bytes - complete name
    name length > 16 bytes - some part of name + file extension
    */
    std::string ext = StringUtils::GetExtension(Name);
    uint32_t i = 0;
    if (Name.size() <= 16)
    {
        for (i = 0; i < Name.size(); i++)
        {
            imageHeader->imageName[i] = Name[i];
        }
    }
    else
    {
        for (i = 0; i < (16 - ext.size()); i++)
        {
            imageHeader->imageName[i] = Name[i];
        }
        uint8_t j = 0;
        for (; i < 16; i++)
        {
            imageHeader->imageName[i] = ext[j++];
        }
    }
}

/******************************************************************************/
uint32_t VersalImageHeader::GetPartitionHeaderOffset(void)
{
    return imageHeader->partitionHeaderWordOffset * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t VersalImageHeader::GetDataSectionCount(void)
{
    return imageHeader->dataSectionCount;
}

/******************************************************************************/
std::vector<std::string> SplitString(std::string strToSplit, char delimeter)
{
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

/******************************************************************************/
void VersalImageHeader::ImportElf(BootImage& bi)
{
    uint8_t proc_state = 0;

    domain = Domain::PS;
    SetPartitionType(PartitionType::ELF);

    /* Convert the file into array of bytes */
    ByteFile data(Filename);

    /* Get the ELF Class format - 32-bit elf vs 64-bit elf */
    elfClass = GetElfClass(data.bytes);
    ElfFormat* elf = ElfFormat::GetElfFormat(elfClass, data.bytes, &proc_state);

    /* Check for no. of executable sections & non-zero size LOAD sections */
    uint8_t exec_count = 0;
    uint8_t non_zero_elf_sec_count = 0;

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

    // Variables for combining sections
    uint32_t num_hdrs = 0;
    uint32_t hdr_index = 0;
    Binary::Length_t size = 0;
    Binary::Length_t total_size = 0;
    Binary::Address_t addr = 0;
    Binary::Address_t prev_end = 0;
    Binary::Address_t offset = 0;

    Binary::Address_t load_addr = 0;
    Binary::Address_t exec_addr = 0;
    uint8_t *partition_data = NULL;

    /* Loop through all the program headers and populate the fields exec, load address etc. */
    for (uint8_t iprog = 0; iprog < elf->programHdrEntryCount; iprog++)
    {
        bool first_index = false;
        size = elf->GetProgramHeaderFileSize(iprog);
        addr = elf->GetPhysicalAddress(iprog);
        exec_addr = elf->GetStartUpAddress();

        /* For bootloader, all the sections are combined by padding the gaps to create only partition */
        if (Bootloader)
        {
            /* Only loadable sections with non-size are considered */
            if ((elf->GetProgramHeaderType(iprog) == xPT_LOAD) && (size > 0))
            {
                num_hdrs++;

                /* Consider load address of first section */
                if (num_hdrs == 1)
                {
                    load_addr = addr;
                }
                else
                {
                    /* Calculate filler bytes and pad to combine different sections */
                    Binary::Length_t filler_bytes = addr - prev_end;
                    if (filler_bytes != 0)
                    {
                        total_size += filler_bytes;
                        partition_data = (uint8_t*)realloc(partition_data, total_size);
                        memset(partition_data + offset, 0, filler_bytes);
                        offset = total_size;
                    }
                }
                total_size += size;
                partition_data = (uint8_t*)realloc(partition_data, total_size);
                memcpy(partition_data + offset, elf->GetProgramHeaderData(iprog), size);
                prev_end = addr + size;
                offset = total_size;
            }
            /* Loop through till last entry */
            if (iprog < elf->programHdrEntryCount - 1)
            {
                continue;
            }

            /* Calculate pad bytes for aligning PMC FW for 16 byte-alignment to add to lengths populated in boot header */
            uint32_t pmc_fw_pad_bytes = 0;
            uint64_t pmc_fw_size = total_size;
            if(updateReserveInPh == true)
            {
                if(Reserve.IsSet())
                {
                    if(pmc_fw_size > Reserve.Value())
                    {
                        LOG_WARNING("Total Partition length is more than Reserve Length. Hence reserve attribute is ignored.");
                    }
                    else
                    {
                        pmc_fw_size = Reserve.Value();
                    }
                }
            }
            if (pmc_fw_size % 16 != 0)
            {
                pmc_fw_pad_bytes = 16 - (pmc_fw_size % 16);
            }
            SetFsblFwSizeIh(pmc_fw_size + pmc_fw_pad_bytes);
            SetTotalFsblFwSizeIh(pmc_fw_size + pmc_fw_pad_bytes);

            if ((bi.bifOptions->GetPmcCdoFileList().size() > 0) || (bi.bifOptions->GetPmcDataBuffer() != NULL))
            {
                uint8_t* cdo_partition = NULL;
                size_t cdo_length = 0;
                if(bi.bifOptions->GetPmcCdoFileList().size() != 0)
                {
                    LOG_INFO("File for merging - %s ", bi.bifOptions->GetPmcCdoFileList().at(0).c_str());
                }

                ParseCdos(bi, bi.bifOptions->GetPmcCdoFileList(), &cdo_partition, &cdo_length);
                /* Calculate pad bytes for aligning PMC CDO for 16 byte-alignment */
                uint32_t total_cdo_pad_bytes = 0;
                if (cdo_length % 16 != 0)
                {
                    total_cdo_pad_bytes = 16 - (cdo_length % 16);
                }

                /* Append PMC CDO to PMC FW to create a single partition */
                pmcdataSize = totalpmcdataSize = cdo_length + total_cdo_pad_bytes;
                total_size = pmc_fw_size + pmc_fw_pad_bytes + pmcdataSize;
                partition_data = (uint8_t*)realloc(partition_data, total_size);
                memset(partition_data + pmc_fw_size, 0, pmc_fw_pad_bytes);
                memcpy(partition_data + pmc_fw_size + pmc_fw_pad_bytes, cdo_partition, cdo_length);
                memset(partition_data + pmc_fw_size + pmc_fw_pad_bytes + cdo_length, 0, total_cdo_pad_bytes);

                delete[] cdo_partition;
            }
        }
        /* For all other partitions add each loadable section as a different partition */
        else
        {
            /* Only loadable sections with non-size are considered */
            if ((elf->GetProgramHeaderType(iprog) == xPT_LOAD) && (size > 0))
            {
                load_addr = addr;
                total_size = size;
                if(updateReserveInPh == true)
                {
                    if(Reserve.IsSet() && non_zero_elf_sec_count > 1)
                    {
                        LOG_WARNING("Multiple sections in elf. Hence reserve attribute is ignored.");
                    }
                    if(Reserve.IsSet() && non_zero_elf_sec_count == 1)
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
                partition_data = (uint8_t*)malloc(total_size);
                memcpy(partition_data, elf->GetProgramHeaderData(iprog), total_size);
            }
        }

        if (partition_data != NULL)
        {
            if (hdr_index == 0)
            {
                first_index = true;
            }

            /* Override the exec and load address, if startup & load are set through BIF attributes */
            if (Startup.IsSet() && iprog == 0)
            {
                exec_addr = Startup.Value();
            }
            if (Load.IsSet() && iprog == 0)
            {
                load_addr = Load.Value();
            }
            PartitionHeader* partHdr = new VersalPartitionHeader(this, hdr_index);
            partHdr->firstValidIndex = first_index;
            partHdr->elfEndianess = elf->endian;
            partHdr->execAddress = exec_addr;
            partHdr->loadAddress = load_addr;
            partHdr->execState = proc_state;

            partHdr->partition = new VersalPartition(partHdr, partition_data, total_size);

            // This length also includes padding size necessary for 16-byte alignment
            partHdr->partitionSize = partHdr->partition->section->Length;

            partitionHeaderList.push_back(partHdr);
            hdr_index++;
            free(partition_data);
            partition_data = NULL;
        }
    }
}

/******************************************************************************/
void VersalImageHeader::ParseCdos(BootImage& bi, std::vector<std::string> filelist, uint8_t** cdo_data, size_t* cdo_size)
{
    uint8_t* total_cdo_data = NULL;
    uint64_t total_cdo_length = 0;
    void *cdo_data_pp = NULL;
    size_t cdo_data_pp_length = 0;

    if (filelist.size() > 0)
    {
        /* Offset and length are set to CDO header size, to take into account the addition of
        merged CDO header which will be added at the end */
        uint64_t offset = sizeof(VersalCdoHeader);
        total_cdo_length = sizeof(VersalCdoHeader);
        for (uint8_t idx = 0; idx != filelist.size(); idx++)
        {
            void* cdo_data = NULL;
            size_t cdo_length = 0;
            uint64_t actual_cdo_size = 0;
            const char* cdo_filename = filelist.at(idx).c_str();
            CdoSequence * cdo_seq;
            cdo_seq = cdoseq_load_cdo(cdo_filename);
            if (cdo_seq == NULL)
            {
                LOG_ERROR("Error parsing CDO file");
            }
            if (bi.overlayCDO && cdooverlay_apply(cdo_seq, (CdoOverlayInfo *)(bi.overlayCDO)))
            {
                LOG_ERROR("Error applying overlay CDO file");
            }
            cdo_data = cdoseq_to_binary(cdo_seq, &cdo_length, 0);
            CheckIdsInCdo(cdo_seq);
            //cdocmd_delete_sequence(cdo_seq);

            if (cdocmd_post_process_cdo(cdo_data, cdo_length, &cdo_data_pp, &cdo_data_pp_length))
            {
                LOG_ERROR("PMC CDO post process error");
            }

            if (cdo_data_pp != NULL)
            {
                //delete cdo_data;
                cdo_data = (uint8_t*)cdo_data_pp;
                cdo_length = cdo_data_pp_length;
            }

            actual_cdo_size = cdo_length - sizeof(VersalCdoHeader);
            total_cdo_length += (actual_cdo_size);
            total_cdo_data = (uint8_t*)realloc(total_cdo_data, total_cdo_length);
            memcpy(total_cdo_data + offset, (uint8_t*)cdo_data + sizeof(VersalCdoHeader), actual_cdo_size);
            offset += actual_cdo_size;
            //delete cdo_data;
        }
        VersalCdoHeader* cdo_header = new VersalCdoHeader;
        cdo_header->remaining_words = 0x04;
        cdo_header->id_word = 0x004f4443; /* CDO */
        cdo_header->version = 0x00000200; /* Version - 2.0 */
        cdo_header->length = (total_cdo_length - sizeof(VersalCdoHeader)) / 4;
        cdo_header->checksum = ~(cdo_header->remaining_words + cdo_header->id_word + cdo_header->version + cdo_header->length);
        memcpy(total_cdo_data, cdo_header, sizeof(VersalCdoHeader));
        delete cdo_header;
    }
    else
    {
        /* If PMC CDO is passed as a buffer 
           or in case PDI is passed as input in BIF, PMC data is read into a buffer from the PDI */
        total_cdo_length = bi.bifOptions->GetTotalpmcdataSize();
        total_cdo_data = new uint8_t[total_cdo_length];
        memcpy(total_cdo_data, bi.bifOptions->GetPmcDataBuffer(), total_cdo_length);
    }

    *cdo_size = total_cdo_length;
    *cdo_data = total_cdo_data;
}

/******************************************************************************/
void VersalImageHeader::CreateAieEnginePartition(BootImage& bi)
{
    bool is_dir = false;
    std::ifstream aie_file(Filename);
    uint64_t size = 0;
    struct stat buf;
    std::list<std::string> aie_elf_list;

    if (stat(Filename.c_str(), &buf) == 0)
    {
        is_dir = S_IFDIR & buf.st_mode;
    }
    if (!is_dir)
    {
        aie_elf_list = bi.bifOptions->aie_elfs;
    }
    else
    {
        aie_elf_list = GetAieFilesPath(Filename);
    }
    for (std::list<std::string>::iterator aie_file = aie_elf_list.begin(); aie_file != aie_elf_list.end(); aie_file++)
    {
        size += ImportAieEngineElfCdo(*aie_file);
    }

    cdoHeader = new VersalCdoHeader;
    cdoHeader->remaining_words = 0x04;
    cdoHeader->id_word = 0x004f4443; /* CDO */
    cdoHeader->version = 0x00000200; /* Version - 2.0 */
    size += sizeof(VersalCdoHeader);
    PartitionHeader* partHdr = new VersalPartitionHeader(this, 0);
    partHdr->execState = 0;
    partHdr->elfEndianess = 0;
    partHdr->firstValidIndex = true;

    uint8_t* pBuffer = new uint8_t[size];
    uint32_t p_offset = 0;
    cdoHeader->length = (size - sizeof(VersalCdoHeader)) / 4;
    cdoHeader->checksum = ~(cdoHeader->remaining_words + cdoHeader->id_word + cdoHeader->version + cdoHeader->length);
    memcpy(pBuffer, cdoHeader, sizeof(VersalCdoHeader));
    p_offset += sizeof(VersalCdoHeader);
    for (std::list<CdoCommandDmaWrite*>::iterator it = cdoSections.begin(); it != cdoSections.end(); it++)
    {
        memcpy(pBuffer + p_offset, (*it), CDO_COMMAND_SIZE);
        p_offset += CDO_COMMAND_SIZE;
        memcpy(pBuffer + p_offset, (*it)->data, ((*it)->length - 2) * 4);
        p_offset += (((*it)->length - 2) * 4);
        delete *it;
    }

    partHdr->partitionSize = size;
    partHdr->partition = new VersalPartition(partHdr, pBuffer, size);
    partitionHeaderList.push_back(partHdr);
    delete[] pBuffer;
    cdoSections.clear();
}

/******************************************************************************/
std::list<std::string> VersalImageHeader::GetAieFilesPath(std::string filename)
{
    std::list <std::string> aie_elf_list;
    std::list <std::string> core_list;
    std::string json_file;
    std::string file_path;
    file_path = filename + "//" + "aie" + "//";
    json_file = file_path + "active_cores.json";
    std::ifstream aie_json(json_file);

    if (!aie_json.good())
    {
        LOG_ERROR("AIE Work \"%s\" directory doesn't have AIE folder", filename.c_str());
    }

    core_list = ParseAieJson(json_file.c_str());
    for (std::list<std::string>::iterator aie_file = core_list.begin(); aie_file != core_list.end(); aie_file++)
    {
        std::string aie_elf = file_path + (*aie_file) + "//" + "Release" + "//" + (*aie_file);
        aie_elf_list.push_back(aie_elf);
    }
    return aie_elf_list;
}

/******************************************************************************/
uint64_t VersalImageHeader::ImportAieEngineElfCdo(std::string aie_file)
{
    uint32_t progHdrCnt = 0;
    uint8_t *newData = NULL;
    uint32_t rowNum = 0;
    uint32_t colNum = 0;

    SetPartitionType(PartitionType::CONFIG_DATA_OBJ);

    /* Convert the file into array of bytes */
    ByteFile data(aie_file);

    /* Get the ELF Class format - 32-bit elf vs 64-bit elf */
    ElfFormat32 elf(data.bytes);

    /* Check for no. of executable sections
    & non-zero size LOAD sections */
    uint8_t execCount = 0;
    uint8_t nonZeroCount = 0;
    for (uint32_t iprog = 0; iprog < elf.programHdrEntryCount; iprog++)
    {
        if (elf.GetProgramHeaderFlags(iprog) & PF_X)
        {
            execCount++;
        }
        if ((elf.GetProgramHeaderFileSize(iprog) > 0) && (elf.GetProgramHeaderType(iprog) == xPT_LOAD))
        {
            nonZeroCount++;
        }
    }

    if (nonZeroCount == 0)
    {
        LOG_ERROR("No non-empty program sections in %s", Filename.c_str());
    }

    // Extract column and row numbers from elf file name (elfname_<colnum>_<rownum>.elf)
    // Example: test_2_4.elf -> AIE core with column-2 and row-4
    std::string temp = StringUtils::RemoveExtension(StringUtils::BaseName(aie_file));
    size_t pos = temp.find_last_of(".");
    if (pos != std::string::npos)
    {
        temp = temp.substr(pos+1);
    }
    std::vector<std::string> coreName = SplitString(temp, '_');
    if (coreName.size() != 2)
    {
        LOG_ERROR("Invalid core name in %s", aie_file.c_str());
    }
    colNum = std::stoi(coreName.at(0));
    rowNum = std::stoi(coreName.at(1));

    /* AIE Tile details in the form of [ColumnNum (16-bits) | RowNum(16-bits)]
    to be put into Exec Address Lo field in case of AIE partition */
    LOG_INFO("Parsing AIE elfs - %s", Filename.c_str());
    CalculateAieEngineBaseAddress(colNum, rowNum);
    /* Loop through all the program headers and populate the fields like, startup address,
    load address etc., which can be overrided by the BIF file options */

    uint32_t numHdrs = 0;
    std::size_t size = 0;
    std::size_t prevSize = 0;
    std::size_t totalSize = 0;
    uint64_t addr = 0;
    uint64_t prevAddr = 0;
    uint64_t offset = 0;
    Binary::Address_t textSecAddr = 0;

    Elf32ProgramHeader* elfPrgHeader = NULL;

    uint64_t total_psize = 0;

    for (uint32_t iprog = 0; iprog < elf.programHdrEntryCount; iprog++)
    {
        elfPrgHeader = elf.programHeaders[iprog];
        // For executable sections (combine all text sections) (addr < 0x20000 corresponds to AIE program memory)
        if ((elfPrgHeader->p_type == xPT_LOAD) && (elfPrgHeader->p_paddr < 0x20000) && (elfPrgHeader->p_filesz > 0) && (elfPrgHeader->p_flags & PF_X))
        {
            size = elfPrgHeader->p_filesz;
            addr = elfPrgHeader->p_paddr;
            // Consider 1st text section for load address in PHT
            if (numHdrs == 0)
            {
                textSecAddr = elf.GetPhysicalAddress(iprog);
            }

            numHdrs++;

            if (numHdrs > 1)
            {
                std::size_t fillerBytes = (std::size_t) (addr - (prevAddr + prevSize));
                totalSize += fillerBytes;
                newData = (uint8_t*)realloc(newData, totalSize);
                memset(newData + offset, 0, fillerBytes);
                offset = totalSize;
            }
            /* Populate the section data */
            totalSize += size;
            newData = (uint8_t *)realloc(newData, totalSize);
            memcpy(newData + offset, elfPrgHeader->data, size);
            prevAddr = addr;
            prevSize = size;
            offset = totalSize;
        }
    }

    total_psize += CdoCmdDmaWrite(totalSize, GetAieEngineGlobalAddress(textSecAddr), newData);
    delete[] newData;

    for (uint8_t iprog = 0; iprog < elf.programHdrEntryCount; iprog++)
    {
        elfPrgHeader = elf.programHeaders[iprog];
        // For data sections (addr >= 0x20000 corresponds to AIE data memory)
        if ((elfPrgHeader->p_type == xPT_LOAD) && !(elfPrgHeader->p_flags & PF_X) && (elfPrgHeader->p_paddr >= 0x20000))
        {
            if (elfPrgHeader->p_filesz > 0)
            {
                Binary::Length_t previousPartitionSize = 0, spill, pSize;
                Binary::Address_t pAddr;
                do
                {
                    spill = CheckAieEngineDataMemoryBoundary((Binary::Address_t)elf.GetPhysicalAddress(iprog) + previousPartitionSize,
                        elfPrgHeader->p_filesz - previousPartitionSize);

                    pSize = elfPrgHeader->p_filesz - spill - previousPartitionSize;
                    pAddr = GetAieEngineGlobalAddress((Binary::Address_t)elf.GetPhysicalAddress(iprog) + previousPartitionSize);
                    total_psize += CdoCmdDmaWrite(pSize, pAddr, elfPrgHeader->data + previousPartitionSize);
                    previousPartitionSize += pSize;
                    progHdrCnt++;
                } while (spill);
            }
            else if (elfPrgHeader->p_memsz > 0)
            {
                Binary::Length_t previousPartitionSize = 0, spill;
                do
                {
                    spill = CheckAieEngineDataMemoryBoundary((Binary::Address_t)elf.GetPhysicalAddress(iprog) + previousPartitionSize,
                        elfPrgHeader->p_memsz - previousPartitionSize);

                    uint32_t pSize = elfPrgHeader->p_memsz - spill - previousPartitionSize;
                    uint64_t pAddr = GetAieEngineGlobalAddress((Binary::Address_t)elf.GetPhysicalAddress(iprog) + previousPartitionSize);
                    uint32_t p_size_pad = pSize + ((4 - (pSize & 3)) & 3);
                    uint8_t *databuffer = new uint8_t[p_size_pad];
                    memset(databuffer, 0, p_size_pad);
                    previousPartitionSize += pSize;
                    total_psize += CdoCmdDmaWrite(pSize, pAddr, databuffer);
                    delete[] databuffer;
                    progHdrCnt++;
                } while (spill);
            }
        }
    }
    return total_psize;
}

/* In physical address space, PM is mapped to 0x20000 and DM is mapped to 0x0,
* whereas in logical address space, PM is mapped to 0x0 and DM is mapped to 0x20000.
* Also, the DM is spread across banks in 4 directions, so adjust the row/tile number,
* based on the bank in which the DM address falls. Below are the details about how to
* adjust the row/col number based on the DM address.
* +----------------------------+---------------+---------------+
* | Bank                       |   Odd Row     |    Even Row   |
* |----------------------------|---------------|---------------|
* | South (0 – 0x7fff)         |   row - 1     |    row  - 1   |
* | West (0x8000 – 0xffff)     |   same tile   |    col - 1    |
* | North (0x10000 – 0x17fff)  |   row + 1     |    row + 1    |
* | East (0x18000 – 0x1ffff)   |   col + 1     |    same tile  |
* +----------------------------+---------------+---------------+
* Row 0 is now assigned to shim, so we should be using row - 1 to check if the row
* is odd or even
*/

/******************************************************************************/
void VersalImageHeader::ImportAieEngineElf(BootImage& bi)
{
    uint32_t progHdrCnt = 0;
    uint8_t procState = 0;
    uint8_t *newData = NULL;
    uint32_t rowNum = 0;
    uint32_t colNum = 0;

    SetPartitionType(PartitionType::ELF);

    /* Convert the file into array of bytes */
    ByteFile data(Filename);

    /* Get the ELF Class format - 32-bit elf vs 64-bit elf */
    ElfFormat32 elf(data.bytes);

    /* Check for no. of executable sections
    & non-zero size LOAD sections */
    uint8_t execCount = 0;
    uint8_t nonZeroCount = 0;
    for (uint32_t iprog = 0; iprog < elf.programHdrEntryCount; iprog++)
    {
        if (elf.GetProgramHeaderFlags(iprog) & PF_X)
        {
            execCount++;
        }
        if ((elf.GetProgramHeaderFileSize(iprog) > 0) && (elf.GetProgramHeaderType(iprog) == xPT_LOAD))
        {
            nonZeroCount++;
        }
    }

    if (nonZeroCount == 0)
    {
        LOG_ERROR("No non-empty program sections in %s", Filename.c_str());
    }

    // Extract column and row numbers from elf file name (elfname_<colnum>_<rownum>.elf)
    // Example: test_2_4.elf -> AIE core with column-2 and row-4
    std::string temp = StringUtils::RemoveExtension(StringUtils::BaseName(Filename));
    std::vector<std::string> coreName = SplitString(temp, '_');
    if (coreName.size() != 2)
    {
        LOG_ERROR("Invalid core name in %s", Filename.c_str());
    }
    colNum = std::stoi(coreName.at(0));
    rowNum = std::stoi(coreName.at(1));

    //colNum = std::stoi(temp.substr((temp.length() - 3), 1));
    //rowNum = std::stoi(temp.substr((temp.length() - 1), 1));

    /* AIE Tile details in the form of [ColumnNum (16-bits) | RowNum(16-bits)]
    to be put into Exec Address Lo field in case of AIE partition */
    uint64_t aieTileDetails = (colNum << 16) | (rowNum);

    CalculateAieEngineBaseAddress(colNum, rowNum);
    /* Loop through all the program headers and populate the fields like, startup address,
    load address etc., which can be overrided by the BIF file options */

    uint32_t numHdrs = 0;
    std::size_t size = 0;
    std::size_t prevSize = 0;
    std::size_t totalSize = 0;
    uint64_t addr = 0;
    uint64_t prevAddr = 0;
    uint64_t offset = 0;
    uint8_t padding = 0;
    Binary::Address_t textSecAddr = 0;

    Elf32ProgramHeader* elfPrgHeader = NULL;
    PartitionHeader* partHdr = new VersalPartitionHeader(this, progHdrCnt);
    partHdr->execState = procState;
    partHdr->elfEndianess = elf.endian;

    for (uint32_t iprog = 0; iprog < elf.programHdrEntryCount; iprog++)
    {
        elfPrgHeader = elf.programHeaders[iprog];
        // For executable sections (combine all text sections) (addr < 0x20000 corresponds to AIE program memory)
        if ((elfPrgHeader->p_type == xPT_LOAD) && (elfPrgHeader->p_paddr < 0x20000) && (elfPrgHeader->p_filesz > 0) && (elfPrgHeader->p_flags & PF_X))
        {
            size = elfPrgHeader->p_filesz;
            addr = elfPrgHeader->p_paddr;
            // Consider 1st text section for load address in PHT
            if (numHdrs == 0)
            {
                textSecAddr = elf.GetPhysicalAddress(iprog);
            }

            numHdrs++;

            if (numHdrs > 1)
            {
                std::size_t fillerBytes = (std::size_t) (addr - (prevAddr + prevSize));
                totalSize += fillerBytes;
                newData = (uint8_t*)realloc(newData, totalSize);
                memset(newData + offset, 0, fillerBytes);
                offset = totalSize;
            }
            /* Populate the section data */
            totalSize += size;
            newData = (uint8_t *)realloc(newData, totalSize);
            memcpy(newData + offset, elfPrgHeader->data, size);
            prevAddr = addr;
            prevSize = size;
            offset = totalSize;
            partHdr->firstValidIndex = true;
        }
    }
    //padding = (4 - (totalSize & 3) & 3);
    partHdr->partitionSize = totalSize + padding;

    /* AIE Tile details in Exec address in case of AIE partitions
    Load address & execution address will be same for AIE elfs, so no need of separate exec addr field*/
    //partHdr->execAddress = GetAieEngineGlobalAddress((Binary::Address_t)elf.GetStartUpAddress());
    partHdr->execAddress = aieTileDetails;

    // consider addr of first header
    partHdr->loadAddress = GetAieEngineGlobalAddress(textSecAddr);
    /* Create a new partition out of each valid program header */
    partHdr->partition = new VersalPartition(partHdr, newData, partHdr->partitionSize);
    progHdrCnt++;
    partitionHeaderList.push_back(partHdr);

    for (uint8_t iprog = 0; iprog < elf.programHdrEntryCount; iprog++)
    {
        elfPrgHeader = elf.programHeaders[iprog];
        // For data sections (addr >= 0x20000 corresponds to AIE data memory)
        if ((elfPrgHeader->p_type == xPT_LOAD) && !(elfPrgHeader->p_flags & PF_X) && (elfPrgHeader->p_paddr >= 0x20000))
        {
            if (elfPrgHeader->p_filesz > 0)
            {
                Binary::Length_t previousPartitionSize = 0, spill;
                do
                {
                    spill = CheckAieEngineDataMemoryBoundary((Binary::Address_t)elf.GetPhysicalAddress(iprog) + previousPartitionSize,
                        elfPrgHeader->p_filesz - previousPartitionSize);

                    PartitionHeader* partHdr = new VersalPartitionHeader(this, progHdrCnt);
                    partHdr->partitionType = PartitionType::ELF;
                    partHdr->partitionSize = elfPrgHeader->p_filesz - spill - previousPartitionSize;
                    partHdr->loadAddress = GetAieEngineGlobalAddress((Binary::Address_t)elf.GetPhysicalAddress(iprog) + previousPartitionSize);
                    partHdr->partition = new VersalPartition(partHdr, elfPrgHeader->data + previousPartitionSize, partHdr->partitionSize);
                    previousPartitionSize += partHdr->partitionSize;
                    partitionHeaderList.push_back(partHdr);
                    progHdrCnt++;
                } while (spill);
            }
            else if (elfPrgHeader->p_memsz > 0)
            {
                Binary::Length_t previousPartitionSize = 0, spill;
                do
                {
                    spill = CheckAieEngineDataMemoryBoundary((Binary::Address_t)elf.GetPhysicalAddress(iprog) + previousPartitionSize,
                        elfPrgHeader->p_memsz - previousPartitionSize);

                    PartitionHeader* partHdr = new VersalPartitionHeader(this, progHdrCnt);
                    partHdr->partitionType = PartitionType::ELF;
                    partHdr->partitionSize = elfPrgHeader->p_memsz - spill - previousPartitionSize;
                    partHdr->loadAddress = GetAieEngineGlobalAddress((Binary::Address_t)elf.GetPhysicalAddress(iprog) + previousPartitionSize);
                    previousPartitionSize += partHdr->partitionSize;
                    uint8_t *databuffer = new uint8_t[partHdr->partitionSize];
                    memset(databuffer, 0, partHdr->partitionSize);
                    partHdr->partition = new VersalPartition(partHdr, databuffer, partHdr->partitionSize);
                    partitionHeaderList.push_back(partHdr);
                    delete[] databuffer;
                    progHdrCnt++;
                } while (spill);
            }
        }
    }
    delete[] newData;
}

/******************************************************************************/
uint32_t VersalImageHeader::CdoCmdDmaWrite(uint32_t pSize, uint64_t pAddr, uint8_t *databuffer)
{
    uint32_t total_size;
    CdoCommandDmaWrite* cdoDataSec = new CdoCommandDmaWrite;
    uint32_t p_size_pad = pSize + ((4 - (pSize & 3)) & 3);
    cdoDataSec->header = 0x00ff0105;
    cdoDataSec->length = (p_size_pad / 4) + 2;
    cdoDataSec->hi_address = ((pAddr) >> 32) & 0xFFFFFFFF;
    cdoDataSec->lo_address = (pAddr) & 0xFFFFFFFF;
    cdoDataSec->data = new uint8_t[p_size_pad];
    memset(cdoDataSec->data, 0, p_size_pad);
    memcpy(cdoDataSec->data, databuffer, pSize);
    cdoSections.push_back(cdoDataSec);
    total_size = (CDO_COMMAND_SIZE + p_size_pad);
    LOG_TRACE("AIE ELF CDO DMA Write Command: Address-0x%x%08x, Size-%x", cdoDataSec->hi_address, cdoDataSec->lo_address, p_size_pad);
    return total_size;
}

/******************************************************************************/
void VersalImageHeader::CalculateAieEngineBaseAddress(uint32_t colNum, uint32_t rowNum)
{

#define AIE_ARRAY_TILE_STRIDE 0x40000
#define AIE_ARRAY_ROW_WIDTH 32

    // Row-0 is always used for SHIM, so actual rows start from 1
    rowNum++;
    uint32_t coreIndex = AIE_ARRAY_ROW_WIDTH * colNum + rowNum;

    coreBaseAddr = aie_array_base_address + (coreIndex * AIE_ARRAY_TILE_STRIDE);

    // South Tile
    uint32_t southCoreIndex = ((AIE_ARRAY_ROW_WIDTH * colNum) + (rowNum - 1));
    southBankBaseAddr = aie_array_base_address + (southCoreIndex * AIE_ARRAY_TILE_STRIDE);

    // West Tile
    uint32_t westCoreIndex = ((rowNum - 1) % 2) ? coreIndex : ((AIE_ARRAY_ROW_WIDTH * (colNum - 1)) + rowNum);
    westBankBaseAddr = aie_array_base_address + (westCoreIndex * AIE_ARRAY_TILE_STRIDE);

    // North Tile
    uint32_t northCoreIndex = ((AIE_ARRAY_ROW_WIDTH * colNum) + (rowNum + 1));
    northBankBaseAddr = aie_array_base_address + (northCoreIndex * AIE_ARRAY_TILE_STRIDE);

    // East Tile
    uint32_t eastCoreIndex = ((rowNum - 1) % 2) ? ((AIE_ARRAY_ROW_WIDTH * (colNum + 1)) + rowNum) : coreIndex;
    eastBankBaseAddr = aie_array_base_address + (eastCoreIndex * AIE_ARRAY_TILE_STRIDE);
}

/******************************************************************************/
Binary::Address_t VersalImageHeader::GetAieEngineGlobalAddress(Binary::Address_t elfAddr)
{
    Binary::Address_t globalAddr = elfAddr;
    if (globalAddr < 0x20000)
    {
        globalAddr += coreBaseAddr + 0x20000;
    }
    else if (globalAddr < 0x40000)
    {
        globalAddr -= 0x20000;
        if (globalAddr < 0x8000)
        {
            globalAddr += southBankBaseAddr;
        }
        else if (globalAddr < 0x10000)
        {
            globalAddr += westBankBaseAddr - 0x8000;
        }
        else if (globalAddr < 0x18000)
        {
            globalAddr += northBankBaseAddr - 0x10000;
        }
        else
        {
            globalAddr += eastBankBaseAddr - 0x18000;
        }
    }
    return globalAddr;
}

/******************************************************************************/
uint32_t VersalImageHeader::CheckAieEngineDataMemoryBoundary(Binary::Address_t globalAddr, Binary::Length_t pSize)
{
    Binary::Address_t startAddr = globalAddr;
    Binary::Address_t endAddr;
    uint64_t spillSize = 0;

    if (startAddr < 0x40000)
    {
        startAddr -= 0x20000;
        endAddr = startAddr + pSize - 1;

        if (((startAddr >= 0 && startAddr < 0x8000) && (endAddr >= 0 && endAddr < 0x8000)) ||
            ((startAddr >= 0x8000 && startAddr < 0x10000) && (endAddr >= 0x8000 && endAddr < 0x10000)) ||
            ((startAddr >= 0x10000 && startAddr < 0x18000) && (endAddr >= 0x10000 && endAddr < 0x18000)) ||
            ((startAddr >= 0x18000 && startAddr < 0x20000) && (endAddr >= 0x18000 && endAddr < 0x20000)))
        {
            // do nothing
        }
        else
        {
            if (startAddr < 0x8000)
            {
                spillSize = endAddr + 1 - 0x8000;
            }
            else if (startAddr < 0x10000)
            {
                spillSize = endAddr + 1 - 0x10000;
            }
            else if (startAddr < 0x18000)
            {
                spillSize = endAddr + 1 - 0x18000;
            }
            else
            {
                spillSize = endAddr + 1 - 0x20000;
            }
        }
    }
    return spillSize;
}

/******************************************************************************/
void VersalImageHeader::SetLoadAndExecAddress(PartitionHeader* partHdr)
{
    if (destCpu == DestinationCPU::AIE)
    {
        // Extract column and row numbers from elf file name (elfname_<colnum>_<rownum>.elf)
        // Example: test_2_4.elf -> AIE core with column-2 and row-4
        std::string temp = StringUtils::RemoveExtension(StringUtils::BaseName(Filename));
        uint32_t colNum = std::stoi(temp.substr((temp.length() - 3), 1));
        uint32_t rowNum = std::stoi(temp.substr((temp.length() - 1), 1));

        /* AIE Tile details in the form of [ColumnNum (16-bits) | RowNum(16-bits)]
        to be put into Exec Address Lo field in case of AIE partition */
        uint64_t aieTileDetails = (colNum << 16) | (rowNum);

        /* AIE Tile details in Exec address in case of AIE partitions
        Load address & execution address will be same for AIE elfs, so no need of separate exec addr field*/
        //partHdr->execAddress = GetAieEngineGlobalAddress((Binary::Address_t)elf.GetStartUpAddress());
        partHdr->execAddress = aieTileDetails;

        CalculateAieEngineBaseAddress(colNum, rowNum);
        partHdr->loadAddress = GetAieEngineGlobalAddress(0);
    }
}

/******************************************************************************/
std::list<std::string> VersalImageHeader::ParseAieJson(const char * filename)
{
    std::list<std::string> core_list;
    cJSON *Jfile = parse_file(filename);
    cJSON *JsonStr = cJSON_GetObjectItem(Jfile, "ActiveCores");
    int active_cores = cJSON_GetArraySize(JsonStr);
    for (int core_no = 0; core_no < active_cores; core_no++) {
        cJSON *core_array = cJSON_GetArrayItem(JsonStr, core_no);
        auto arrayStr = StringUtils::Split_jsonString((std::string) cJSON_Print(core_array));
        auto key = StringUtils::Trim_quotes(arrayStr.first);
        auto value = StringUtils::Trim_quotes(arrayStr.second);
        //auto path = "/Release/" + key;
        //path_full.push_back(value + path);
        core_list.push_back(key);
    }
    return core_list;
}

/******************************************************************************/
CdoCommandHeader* CdoCmdCdoEnd(void)
{
    CdoCommandHeader* cdoCmd = new CdoCommandHeader;
    cdoCmd->reserved = 0x00;
    cdoCmd->length = 0x0;
    cdoCmd->handler_id = 1;
    cdoCmd->cmd_id = CdoCmds::END;
    LOG_TRACE("CDO_CMD_END");
    return cdoCmd;
}

/******************************************************************************/
CdoSsitSlaves* CdoCmdWriteSsitSyncSlaves(uint8_t num_slrs)
{
    CdoSsitSlaves* cdoCmd = new CdoSsitSlaves;
    cdoCmd->header.reserved = 0x00;
    cdoCmd->header.length = 0x2;
    cdoCmd->header.handler_id = 1;
    cdoCmd->header.cmd_id = CdoCmds::SSIT_SYNC_SLAVES;
    cdoCmd->mask = 0x7 >> (MAX_SLAVE_SLRS - num_slrs);
    cdoCmd->timeout_ms = 0xFFFF;
    LOG_TRACE("CDO_CMD_SSIT_SYNC_SLAVES - %d, %x", cdoCmd->mask, cdoCmd->timeout_ms);
    return cdoCmd;
}

/******************************************************************************/
CdoSsitSlaves* CdoCmdWriteSsitWaitSlaves(uint32_t mask)
{
    CdoSsitSlaves* cdoCmd = new CdoSsitSlaves;
    cdoCmd->header.reserved = 0x00;
    cdoCmd->header.length = 0x2;
    cdoCmd->header.handler_id = 1;
    cdoCmd->header.cmd_id = CdoCmds::SSIT_WAIT_SLAVES;
    cdoCmd->mask = mask;
    cdoCmd->timeout_ms = 0xFFFF;
    LOG_TRACE("CDO_CMD_SSIT_WAIT_SLAVES - %d, %x", cdoCmd->mask, cdoCmd->timeout_ms);
    return cdoCmd;
}

/******************************************************************************/
CdoCommandWriteKeyhole* CdoCmdWriteKeyHole(uint32_t size, SlrId::Type slr_index)
{
    uint8_t index = slr_index;
    if (slr_index == 4)
    {
        index = 0;
    }
    CdoCommandWriteKeyhole* cdoCmd = new CdoCommandWriteKeyhole;
    cdoCmd->header.reserved = 0x00;
    cdoCmd->header.length = 0xff;
    cdoCmd->header.handler_id = 1;
    cdoCmd->header.cmd_id = CdoCmds::WRITE_KEYHOLE;
    cdoCmd->length = (size / 4) + 3;    /* +3 to accomodate the address lengths and keyhole size*/
    cdoCmd->hi_address = (slr_sbi_base_array[index] >> 32) & 0xFFFFFFFF;
    cdoCmd->lo_address = slr_sbi_base_array[index] & 0xFFFFFFFF;
    cdoCmd->keyhole_size = SBI_KEYHOLE_SIZE / 4;

    LOG_TRACE("   CDO_CMD_WRITE_KEYHOLE: Address-0x%llx, Length-0x%lx (words)", slr_sbi_base_array[index], cdoCmd->length);
    return cdoCmd;
}

/******************************************************************************/
CdoCommandNop* CdoCmdNoOperation(size_t size)
{
    CdoCommandNop* cdoCmd = new CdoCommandNop;
    uint8_t length = size - CDO_CMD_NOP_SIZE;
    cdoCmd->header.reserved = 0x00;
    /* If 1 word needs to be ignored, then no need to add length and payload */
    cdoCmd->header.length = length / CDO_CMD_NOP_SIZE;
    cdoCmd->header.handler_id = 1;
    cdoCmd->header.cmd_id = CdoCmds::NOP;
    LOG_TRACE("   CDO_CMD_NOP - %d", cdoCmd->header.length + 1);
    return cdoCmd;
}

/******************************************************************************/
bool IsCdoCmdEndFound(const uint8_t* buffer, size_t size)
{
    bool status = false;
    uint32_t end_word = buffer[size - sizeof(CdoCommandHeader)];

    if (end_word == CDO_CMD_END)
    {
        status = true;
    }
    return status;
}

/******************************************************************************/
bool IsCdoFile(uint32_t value)
{
    if ((value == 0x584c4e58) || (value == 0x584e4c58) || (value == 0x004f4443) || (value == 0x43444f00))
    {
        return true;
    }
    return false;
}

/******************************************************************************/
static bool CompareCDOSequences(CdoSequence * user_cdo_seq, std::string golden_cdo_filename)
{
    cdoseq_extract_writes(user_cdo_seq);
    size_t user_cdo_size = 0;
    uint8_t* user_cdo_buffer = (uint8_t*)cdoseq_to_binary(user_cdo_seq, &user_cdo_size, 0);

    struct stat f_stat;
    bool found = false;
    if (stat(golden_cdo_filename.c_str(), &f_stat) == 0)
    {
        found = true;
    }
    else
    {
#ifdef _WIN32
        std::string DS = "\\";
#else
        std::string DS = "/";
#endif
        char * s = getenv("RDI_DATADIR");
        if (s != NULL && *s != '\0')
        {
            golden_cdo_filename = s + DS + "bootgen" + DS + golden_cdo_filename;
            if (stat(golden_cdo_filename.c_str(), &f_stat) == 0)
            {
                found = true;
            }
        }
    }
    if (!found)
    {
        LOG_WARNING("Cannot find golden CDO : %s", golden_cdo_filename.c_str());
        return true;
    }

    CdoSequence * golden_cdo_seq;
    golden_cdo_seq = cdoseq_load_cdo(golden_cdo_filename.c_str());
    if (golden_cdo_seq == NULL)
    {
        LOG_ERROR("Error parsing CDO file : %s", golden_cdo_filename.c_str());
    }
    cdoseq_extract_writes(golden_cdo_seq);

    uint8_t* golden_cdo_buffer = NULL;
    size_t golden_cdo_size = 0;
    golden_cdo_buffer = (uint8_t*)cdoseq_to_binary(golden_cdo_seq, &golden_cdo_size, 0);
    //cdocmd_delete_sequence(golden_cdo_seq);

    const size_t header_size = 20;
    if (user_cdo_size == golden_cdo_size && memcmp(user_cdo_buffer + header_size, golden_cdo_buffer + header_size, golden_cdo_size - header_size) == 0)
    {
        return true;
    }

    return false;
}

/******************************************************************************/
bool IsCdoFile(std::string file)
{
    std::ifstream stream(file.c_str(), std::ios_base::binary);
    if (!stream)
    {
        LOG_ERROR("Cannot read file - %s ", (file.c_str()));
    }
    std::string line;
    getline(stream, line);
    if ((line.find("Xilinx ASCII NPI Deviceimage") != std::string::npos) || (line.find("Xilinx ASCII PSAXIMM Deviceimage") != std::string::npos) || (line.find("version") != std::string::npos))
    {
        return true;
    }
    else
    {
        FILE *binFile = fopen(file.c_str(), "rb");

        if (!binFile)
        {
            LOG_ERROR("Cannot read file %s", file.c_str());
        }

        uint32_t value = 0;
        size_t result = fread(&value, 1, 4, binFile);
        if (result == 4)
        {
            if ((value == 0x584c4e58) || (value == 0x584e4c58) || (value == 0x004f4443) || (value == 0x43444f00))
            {
                return true;
            }
        }
        fclose(binFile);
    }
    return false;
}


/******************************************************************************/
SlrPdiType GetSlrType(SlrPdiInfo* slr)
{
    if (IsCdoFile(slr->file))
    {
        return SlrPdiType::MASTER_CDO;
    }
    else
    {
        return SlrPdiType::BOOT;
    }
}

/******************************************************************************/
uint8_t GetTotalSlrCount(std::list<SlrPdiInfo*> slrPdi)
{
    uint8_t cnt = 0;
    for (std::list<SlrPdiInfo*>::iterator slr_id = slrPdi.begin(); slr_id != slrPdi.end(); slr_id++)
    {
        if (!IsCdoFile((*slr_id)->file))
        {
            cnt++;
        }
    }
    return cnt;
}


/******************************************************************************/
bool SortByIndex(SlrPdiInfo* A, SlrPdiInfo* B)
{
    return (A->index < B->index);
}

/******************************************************************************/
/* 
Total SLR boot partition:
+-------------------+
|    CDO Header     |              CDO header size
+-------------------+                     +
|                   |              WRITE KEYHOLE CDO command size
|     SLR1 PDI      |                     +
|                   |              SLR1 PDI size
+-------------------+                     +
|       SYNC        |              SYNC CDO command size
+-------------------+                     +
|                   |              WRITE KEYHOLE CDO command size
|     SLR2 PDI      |                     +
|                   |              SLR2 PDI size
+-------------------+                     +
|       SYNC        |              SYNC CDO command size
+-------------------+                     +
|                   |              WRITE KEYHOLE CDO command size
|     SLR3 PDI      |                     +
|                   |              SLR3 PDI size
+-------------------+                     +
|       SYNC        |              SYNC CDO command size
+-------------------+                     +
|    NoC Freq &     |
|     reconfig      |              NoC CDO size
|       CDO         |
+-------------------+                     +
|       SYNC        |              SYNC CDO command size
+-------------------+
*/

void VersalImageHeader::CreateSlrBootPartition(BootImage& bi)
{
    LOG_INFO("Creating SLR Boot CDO partition");

    if ((slrBootPdiInfo.size() == 1) && GetSlrType(slrBootPdiInfo.front()) == MASTER_CDO)
    {
        SetPartitionType(PartitionType::CONFIG_DATA_OBJ);
        PartitionHeader* partHdr = new VersalPartitionHeader(this, 0);
        partHdr->execState = 0;
        partHdr->elfEndianess = 0;
        partHdr->firstValidIndex = true;
        uint8_t* cdo_buffer = NULL;
        size_t size = 0;
        const char* cdo_filename = slrBootPdiInfo.front()->file.c_str();
        CdoSequence * cdo_seq;
        cdo_seq = cdoseq_load_cdo(cdo_filename);
        if (cdo_seq == NULL)
        {
            LOG_ERROR("Error parsing CDO file");
        }
        cdo_buffer = (uint8_t*)cdoseq_to_binary(cdo_seq, &size, 0);
        partHdr->partitionSize = size;
        partHdr->partition = new VersalPartition(partHdr, cdo_buffer, size);
        partitionHeaderList.push_back(partHdr);
        delete cdo_buffer;
        delete cdoHeader;
        cdoSections.clear();
    }
    else
    {
        uint32_t p_offset = 0;
        uint64_t size = 0;
        uint8_t slr_cnt = 0;

        /* Add CDO Header */
        cdoHeader = new VersalCdoHeader;
        cdoHeader->remaining_words = CDO_REMAINING_WORDS;
        cdoHeader->id_word = CDO_IDENTIFICATION; /* CDO */
        cdoHeader->version = CDO_VERSION; /* Version - 2.0 */
        cdoHeader->length = 0;
        cdoHeader->checksum = 0;

        size += sizeof(VersalCdoHeader);
        uint8_t* p_buffer = new uint8_t[size];
        memcpy(p_buffer, cdoHeader, sizeof(VersalCdoHeader));
        p_offset += sizeof(VersalCdoHeader);


        slrBootPdiInfo.sort(SortByIndex);
        num_of_slrs = GetTotalSlrCount(slrBootPdiInfo);
        std::string device_name = "";
        switch (bi.bifOptions->idCode) {
        case 0x04d14093: /* h50 */
            device_name = "xcvp1802";
            break;
        default:
            LOG_WARNING("Unknown SSIT device IDCODE 0x%08x, skipping verification of NoC configuration", bi.bifOptions->idCode);
        }

        /* Add CDO Write Keyhole commands */
        char * do_ssit_check = getenv("BOOTGEN_DO_SSIT_NOC_CHECK");
        bool master_cdo_verified = 0;
        for (std::list<SlrPdiInfo*>::iterator slr_id = slrBootPdiInfo.begin(); slr_id != slrBootPdiInfo.end(); slr_id++)
        {
            uint32_t file_size;
            uint32_t pad_size;
            std::ifstream stream((*slr_id)->file.c_str(), std::ios_base::binary);
            if (!stream)
            {
                LOG_ERROR("Cannot read file - %s ", (*slr_id)->file.c_str());
            }
            
            if (GetSlrType(*slr_id) == SlrPdiType::BOOT)
            {
                slr_cnt++;
                ByteFile slr_boot_data((*slr_id)->file);
                file_size = slr_boot_data.len;
                pad_size = file_size + ((4 - (file_size & 3)) & 3);
                uint32_t bh_offset = 0;
                /* Remove the 16-bytes of SMAP bus width from start of PDI */
                uint32_t smap_data = (slr_boot_data.bytes[0]) + (slr_boot_data.bytes[1] << 8) + (slr_boot_data.bytes[2] << 16) + (slr_boot_data.bytes[3] << 24);
                if ((smap_data == 0xDD000000) || (smap_data == 0x00DD0000) || (smap_data == 0x000000DD))
                {
                    bh_offset = (SMAP_BUS_WIDTH * 4);
                    file_size -= (SMAP_BUS_WIDTH * 4);
                    pad_size -= (SMAP_BUS_WIDTH * 4);
                }

                if ((device_name != "") && (do_ssit_check != NULL))
                {
                    /* Get the partition offsets from the PDI */
                    uint32_t partition_offset = 0;
                    uint32_t partition_size = 0;
                    //VersalBootHeaderStructure* bH = (VersalBootHeaderStructure*)(slr_boot_data.bytes);
                    uint32_t *id_offset = (uint32_t *)(slr_boot_data.bytes + bh_offset + 0x4);
                    if (*id_offset == 0x584c4e58)
                    {
                        uint32_t *ih_byte_offset = (uint32_t *)(slr_boot_data.bytes + bh_offset + 0xb4);
                        VersalImageHeaderTableStructure* iHT = (VersalImageHeaderTableStructure*)(slr_boot_data.bytes + *ih_byte_offset);
                        size_t offset = iHT->firstPartitionHeaderWordOffset * 4;
                        offset += sizeof(VersalPartitionHeaderTableStructure);
                        for (uint8_t index = 1; index < iHT->partitionTotalCount; index++)
                        {
                            VersalPartitionHeaderTableStructure* pHT = (VersalPartitionHeaderTableStructure*)(slr_boot_data.bytes + offset);
                            if (((pHT->partitionAttributes >> vphtPartitionTypeShift) & vphtPartitionTypeMask) == PartitionType::CONFIG_DATA_OBJ)
                            {
                                partition_size = pHT->totalPartitionLength * 4;
                                partition_offset = pHT->partitionWordOffset * 4;
                                break;
                            }
                            offset += sizeof(VersalPartitionHeaderTableStructure);
                        }

                        CdoSequence* cdo_seq = decode_cdo_binary(slr_boot_data.bytes + partition_offset, partition_size);
                        if (cdo_seq == NULL) {
                            LOG_WARNING("Cannot decode binary cdo data for slr %d, skipping verification of NoC configuration", (*slr_id)->index);
                        }
                        else
                        {
                            std::string golden_cdo_filename = device_name + "_boot_" + std::to_string((*slr_id)->index) + ".rnpi";
                            bool ret = CompareCDOSequences(cdo_seq, golden_cdo_filename);
                            //cdocmd_delete_sequence(cdo_seq);
                            if (ret == false)
                            {
                                LOG_ERROR("NoC configuration for slr %d doesn't match golden configuration", (*slr_id)->index);
                            }
                        }
                    }
                }

                /* For DMA alignment - add nop commands to align it to 128-bit (16-byte) */
                size_t pad_bytes = ((16 - ((p_offset + CDO_CMD_WRITE_KEYHOLE_SIZE) & 15)) & 15);
                size += (CDO_CMD_WRITE_KEYHOLE_SIZE + pad_size + pad_bytes);
                p_buffer = (uint8_t*)realloc(p_buffer, size);

                if (pad_bytes != 0)
                {
                    CdoCommandNop* cdoCmd = CdoCmdNoOperation(pad_bytes);
                    LOG_TRACE("NOP - 0x%x", p_offset);
                    memcpy(p_buffer + p_offset, cdoCmd, CDO_CMD_NOP_SIZE);
                    p_offset += CDO_CMD_NOP_SIZE;
                    if (cdoCmd->header.length > 0)
                    {
                        memset(p_buffer + p_offset, 0, cdoCmd->header.length * sizeof(uint32_t));
                        p_offset += (cdoCmd->header.length * sizeof(uint32_t));
                    }
                }

                /* Add Write Key Hole command with SLR Boot PDI data */
                CdoCommandWriteKeyhole* cdoCmd = CdoCmdWriteKeyHole(pad_size, (*slr_id)->index);
                memcpy(p_buffer + p_offset, cdoCmd, CDO_CMD_WRITE_KEYHOLE_SIZE);
                p_offset += CDO_CMD_WRITE_KEYHOLE_SIZE;
                delete cdoCmd;
                memcpy(p_buffer + p_offset, slr_boot_data.bytes + bh_offset, file_size);
                p_offset += file_size;
                if ((pad_size - file_size) != 0)
                {
                    memset(p_buffer + p_offset, 0, pad_size - file_size);
                    p_offset += pad_size;
                }

                /* Add SSIT Wait Slave command */
                size += sizeof(CdoSsitSlaves);
                p_buffer = (uint8_t*)realloc(p_buffer, size);
                CdoSsitSlaves *ssit_wait_slaves_cmd = CdoCmdWriteSsitWaitSlaves(1 << ((*slr_id)->index - 1));
                memcpy(p_buffer + p_offset, ssit_wait_slaves_cmd, sizeof(CdoSsitSlaves));
                p_offset += sizeof(CdoSsitSlaves);
                delete ssit_wait_slaves_cmd;

                
                if (slr_cnt == num_of_slrs)
                {
                    /* Add SSIT Sync Slave command */
                    size += sizeof(CdoSsitSlaves);
                    p_buffer = (uint8_t*)realloc(p_buffer, size);
                    CdoSsitSlaves *ssit_sync_slaves_cmd = CdoCmdWriteSsitSyncSlaves(num_of_slrs);
                    memcpy(p_buffer + p_offset, ssit_sync_slaves_cmd, sizeof(CdoSsitSlaves));
                    p_offset += sizeof(CdoSsitSlaves);
                    delete ssit_sync_slaves_cmd;
                }
                if (bi.options.GetDumpOption() != DumpOption::SLAVE_PDIS)
                {
                    remove ((*slr_id)->file.c_str());
                }
            }
            else if (GetSlrType(*slr_id) == SlrPdiType::MASTER_CDO)
            {
                /* Add Master Boot NPI and NoC freq CDO commands by parsing the CDO file */
                const char* cdo_filename = (*slr_id)->file.c_str();
                CdoSequence* cdo_seq = cdoseq_load_cdo(cdo_filename);
                if (cdo_seq == NULL)
                {
                    LOG_ERROR("Error parsing CDO file : %s", cdo_filename);
                }

                if ((master_cdo_verified == 0) && (device_name != "") && (do_ssit_check != NULL))
                {
                    std::string golden_cdo_filename = device_name + "_boot_0.rnpi";
                    master_cdo_verified = 1;
                    bool ret = CompareCDOSequences(cdo_seq, golden_cdo_filename);
                    //cdocmd_delete_sequence(cdo_seq);
                    if (ret == false)
                    {
                        LOG_ERROR("NoC configuration for master slr doesn't match golden configuration");
                    }
                    cdo_seq = cdoseq_load_cdo(cdo_filename);
                    if (cdo_seq == NULL)
                    {
                        LOG_ERROR("Error parsing CDO file : %s", cdo_filename);
                    }
                }
                size_t cdo_size = 0;
                uint8_t* cdo_buffer = (uint8_t*)cdoseq_to_binary(cdo_seq, &cdo_size, 0);
                file_size = cdo_size;

                pad_size = file_size + ((4 - (file_size & 3)) & 3);
                file_size -= sizeof(VersalCdoHeader);
                pad_size -= sizeof(VersalCdoHeader);
                if (IsCdoCmdEndFound(cdo_buffer, file_size))
                {
                    file_size -= +sizeof(CdoCommandHeader);
                    pad_size -= +sizeof(CdoCommandHeader);
                }
                size += pad_size;
                p_buffer = (uint8_t*)realloc(p_buffer, size);

                memcpy(p_buffer + p_offset, cdo_buffer + sizeof(VersalCdoHeader), file_size);
                if ((pad_size - file_size) != 0)
                {
                    memset(p_buffer + p_offset + file_size, 0, pad_size - file_size);
                }
                delete[] cdo_buffer;
                p_offset += pad_size;
                if ((*slr_id)->index == slrBootPdiInfo.size())
                {
                    /* Add SSIT Sync Slave command */
                    size += sizeof(CdoSsitSlaves);
                    p_buffer = (uint8_t*)realloc(p_buffer, size);

                    /* Add SSIT Sync Slave command */
                    CdoSsitSlaves *ssit_sync_slaves_cmd = CdoCmdWriteSsitSyncSlaves(num_of_slrs);
                    memcpy(p_buffer + p_offset, ssit_sync_slaves_cmd, sizeof(CdoSsitSlaves));
                    p_offset += sizeof(CdoSsitSlaves);
                    delete ssit_sync_slaves_cmd;
                }
            }
        }

        size += sizeof(CdoCommandHeader);
        p_buffer = (uint8_t*)realloc(p_buffer, size);
        CdoCommandHeader* cmd_end = CdoCmdCdoEnd();
        memcpy(p_buffer + p_offset, cmd_end, sizeof(CdoCommandHeader));
        delete cmd_end;

        /* Update CDO header lengths and checksum */
        cdoHeader->length = (size - sizeof(VersalCdoHeader)) / 4;
        cdoHeader->checksum = ~(cdoHeader->remaining_words + cdoHeader->id_word + cdoHeader->version + cdoHeader->length);
        memcpy(p_buffer, cdoHeader, sizeof(VersalCdoHeader));

        SetPartitionType(PartitionType::CONFIG_DATA_OBJ);
        PartitionHeader* partHdr = new VersalPartitionHeader(this, 0);
        partHdr->execState = 0;
        partHdr->elfEndianess = 0;
        partHdr->firstValidIndex = true;
        partHdr->partitionSize = size;
        partHdr->partition = new VersalPartition(partHdr, p_buffer, size);
        partitionHeaderList.push_back(partHdr);
        delete[] p_buffer;
        delete cdoHeader;
        cdoSections.clear();
    }
}

/******************************************************************************/
size_t GetActualChunkSize(SsitConfigSlrInfo* slr_info)
{
    size_t num_bytes = 0;
    static bool chunk_size_info_printed = false;
    /* Default chunk size - 32KB */
    uint64_t chunk_size = 0x8000;
    char * ssit_chunk_size = getenv("BOOTGEN_SSIT_CHUNK_SIZE");
    if (ssit_chunk_size != NULL)
    {
        chunk_size = strtoull(ssit_chunk_size, NULL, 16);
    }
    if (!chunk_size_info_printed)
    {
        LOG_INFO("SSIT chunk size = 0x%x", chunk_size);
        chunk_size_info_printed = true;
    }

    /* Create chunks such that each partition within a SLR PDI starts as a new chunk */
    size_t partition_size = slr_info->partition_sizes.at(slr_info->partition_index);

    if((partition_size - slr_info->partition_offset) <= chunk_size)
    {
        /* If remaining bytes in partition are less than chunk size, create a chunk with remaining bytes */
        num_bytes = partition_size - slr_info->partition_offset;
        if (slr_info->partition_index < (slr_info->partition_sizes.size() - 1))
        {
            slr_info->partition_index++;
            slr_info->partition_offset = 0;
        }
        else
        {
            slr_info->partition_offset += num_bytes;
        }
    }
    else
    {
        /* If partition size is greater than chunk size, create a chunk with chunk size */
        num_bytes = chunk_size;
        slr_info->partition_offset += num_bytes;
    }

    return num_bytes;
}

/******************************************************************************/
void VersalImageHeader::LogConfigSlrDetails(size_t chunk_num, uint8_t slr_num, size_t offset, size_t chunk_size, size_t sync_points)
{
    SsitConfigSlrLog* log_details = new SsitConfigSlrLog;
    log_details->slr_num = slr_num;
    log_details->offset = offset;
    log_details->size = chunk_size;
    log_details->sync_points = sync_points;
    configSlrLog.push_back(log_details);
}

/******************************************************************************/
void VersalImageHeader::PrintConfigSlrSummary(void)
{
    LOG_TRACE("Note: Offset in below table is from the start of the config CDO partition and not start of PDI");
    LOG_TRACE("---------------------------------------------------------------");
    LOG_TRACE("                        SSIT Summary                           ");
    LOG_TRACE("---------------------------------------------------------------");
    LOG_TRACE(" Chunks   SLR     Offset        Size    Cumulative Sync Points");
    LOG_TRACE("---------------------------------------------------------------");
    uint32_t index = 1;
    for (std::vector<SsitConfigSlrLog*>::iterator slr_info = configSlrLog.begin(); slr_info != configSlrLog.end(); slr_info++)
    {
        std::string slr = ((*slr_info)->slr_num == 4) ? "master" : ("slr-" + std::to_string((*slr_info)->slr_num));
        LOG_TRACE("%4d  %8s    0x%-8x    0x%-8x    %3d", index++, slr.c_str(), (*slr_info)->offset, (*slr_info)->size, (*slr_info)->sync_points);
    }
    LOG_TRACE("---------------------------------------------------------------");
}

/******************************************************************************/
/* SLR Slave config partition CDO with round robin chunking
        +--------------+--------------+--------------+--------------+
        |     SLR1     |     SLR2     |     SLR3     |  Master SLR  |
        +--------------+--------------+--------------+--------------+
        |       1      |     2 (S)    |       3      |              |
        +--------------+--------------+--------------+--------------+
        |   4 (S) (S)  |              |       5      |              |
        +--------------+--------------+--------------+--------------+
        |              |              |       6      |              |
        +--------------+--------------+--------------+--------------+
        |              |              |       7      |              |
        +--------------+--------------+--------------+--------------+
        |              |              |     8 (S)    |     9 (S)    |
        +--------------+--------------+--------------+--------------+
        |              |  10 (S) (S)  |      11      |              |
        +--------------+--------------+--------------+--------------+
        |              |      -       |      12      |              |
        +--------------+--------------+--------------+--------------+
        |              |      -       |      13      |              |
        +--------------+--------------+--------------+--------------+
        |              |      -       |     14 (S)   |     15 (S)   |
        +--------------+--------------+--------------+--------------+
*/

void VersalImageHeader::CreateSlrConfigPartition(BootImage& bi)
{
    size_t size = 0;
    size_t p_offset = 0;
    size_t common_sync_point = 0, current_sync_point = 0, last_sync_point = 0;
    size_t total_slr_chunk_size = 0;
    size_t master_file_size = 0;
    size_t slr_total_file_size = 0;
    uint32_t chunk_num = 1;
    bool master_slr_available = false;
    LOG_INFO("Creating SLR Config CDO partition");

    /* CDO Header */
    cdoHeader = new VersalCdoHeader;
    cdoHeader->remaining_words = CDO_REMAINING_WORDS;
    cdoHeader->id_word = CDO_IDENTIFICATION; /* CDO */
    cdoHeader->version = CDO_VERSION; /* Version - 2.0 */

    /* Populate length and checksum later, as we can't estimate the exact size of CDO */
    cdoHeader->length = 0;
    cdoHeader->checksum = 0;
    size += sizeof(VersalCdoHeader);
    uint8_t* p_buffer = new uint8_t[size];
    memcpy(p_buffer, cdoHeader, sizeof(VersalCdoHeader));
    p_offset += sizeof(VersalCdoHeader);

    /* Initialize the individual SLR config structures */
    slrConfigPdiInfo.sort(SortByIndex);
    for (std::list<SlrPdiInfo*>::iterator slr_id = slrConfigPdiInfo.begin(); slr_id != slrConfigPdiInfo.end(); slr_id++)
    {
        SsitConfigSlrInfo* configSlr = new SsitConfigSlrInfo;
        configSlr->file = (*slr_id)->file;
        configSlr->index = (*slr_id)->index;
        configSlr->offset = 0;
        configSlr->size = 0;
        configSlr->sync_points = 0;
        configSlr->data = NULL;
        configSlr->partition_index = 0;
        configSlr->partition_offset = 0;
        configSlr->num_chunks = 0;
        configSlr->eof = false;
        configSlrsInfo.push_back(configSlr);
        if (configSlr->index ==SlrId::MASTER)
        {
            master_slr_available = true;
        }
    }

    /* Parse slave SLR config files to get SLR data, identify sync points, file sizes and get total file size */
    ParseSlrConfigFiles(&slr_total_file_size);

    /* 1. Create DMA write keyhole command for chunk from each slave SLR in a round robin fashion.
       2. Continue the above process till a sync point is found in the current chunk.
       3. Once a sync point is reached for a slave SLRx, don't include this slave SLRx chunks in the round robin.
       4. Continue the above steps for remaining slave SLRs.
       5. Once all the slave SLRs reach a common sync point, create a master SLR chunk.
       6. Master SLR chunk is data between two sync points (CDO_SSIT_SYNC_SLAVES_CMD).
       7. Repeat the above steps, till all the sync points are processed or EOF for each slave SLR PDI.
    */
    while (total_slr_chunk_size < slr_total_file_size)
    {
        for (std::vector<SsitConfigSlrInfo*>::iterator slr_info = configSlrsInfo.begin(); slr_info != configSlrsInfo.end(); slr_info++)
        {
            /* Process slave SLR data only if it has not yet reached the common sync point */
            if (((*slr_info)->sync_points == common_sync_point) && ((*slr_info)->index != SlrId::MASTER) && (last_sync_point == current_sync_point))
            {
                /* Slave SLRs processing */
                size_t chunk_size = GetActualChunkSize(*slr_info);
                
                if(chunk_size != 0)
                {
                    (*slr_info)->num_chunks++;
                    /* For DMA alignment - add nop commands to align it to 128-bit (16-byte) */
                    size_t pad_bytes = ((16 - ((p_offset + CDO_CMD_WRITE_KEYHOLE_SIZE) & 15)) & 15);
                    size += (chunk_size + CDO_CMD_WRITE_KEYHOLE_SIZE + pad_bytes);
                    p_buffer = (uint8_t*)realloc(p_buffer, size);

                    if (pad_bytes != 0)
                    {
                        CdoCommandNop* cdoCmd = CdoCmdNoOperation(pad_bytes);
                        memcpy(p_buffer + p_offset, cdoCmd, CDO_CMD_NOP_SIZE);
                        p_offset += CDO_CMD_NOP_SIZE;
                        if (cdoCmd->header.length > 0)
                        {
                            memset(p_buffer + p_offset, 0, cdoCmd->header.length * sizeof(uint32_t));
                            p_offset += (cdoCmd->header.length * sizeof(uint32_t));
                        }
                    }

                    LOG_TRACE("Chunk-%d | SLR-%d", chunk_num, (*slr_info)->index);
                    /* Check for sync points in the current chunk */
                    CheckSyncPointInChunk(*slr_info, chunk_size);
                    LogConfigSlrDetails(chunk_num++, (((*slr_info)->index == 4) ? 0 : (*slr_info)->index), p_offset, chunk_size, (*slr_info)->sync_points);

                    /* Add write keyhole command for slave SLRs and master config */
                    CdoCommandWriteKeyhole* cdoCmd = CdoCmdWriteKeyHole(chunk_size, (*slr_info)->index);
                    memcpy(p_buffer + p_offset, cdoCmd, CDO_CMD_WRITE_KEYHOLE_SIZE);
                    delete cdoCmd;
                    p_offset += CDO_CMD_WRITE_KEYHOLE_SIZE;
                    memcpy(p_buffer + p_offset, (*slr_info)->data + (*slr_info)->offset, chunk_size);

                    p_offset += chunk_size;
                    total_slr_chunk_size += chunk_size;
                    (*slr_info)->offset += chunk_size;
                }
                else
                {
                    /* If EOF reached, don't consider this SLR for finding common sync points */
                    (*slr_info)->eof = true;
                }
            }

            /* Get the common sync point - sync point which all the slave SLRs have serviced */
            current_sync_point = FindCurrentSyncPoint();
            
            if ((last_sync_point != current_sync_point))
            {
                if (master_slr_available)
                {
                    /* Do not process any other SLR data, until the master data is processed */
                    if (((*slr_info)->index == SlrId::MASTER) && (master_file_size < (*slr_info)->size))
                    {
                        static uint8_t master_index = 0;
                        size_t master_chunk_size = 0;

                        /* Chunk size in master is different from the chunk size of slave SLR chunk sizes
                           Master SLRs chunk size is data size between two sync points (CDO_SSIT_SYNC_SLAVES_CMD )in Master SLR CDO */
                        (*slr_info)->num_chunks++;

                        const char* cdo_filename = (*slr_info)->file.c_str();
                        CdoSequence * master_cdo_seq;
                        master_cdo_seq = cdoseq_load_cdo(cdo_filename);
                        if (master_cdo_seq == NULL)
                        {
                            LOG_ERROR("Error parsing CDO file");
                        }
                        size_t part_size = 0;

                        /* Pass the complete CDO sequence and SYNC point index */
                        /* If SYNC point 1 is passed, it will extract the data between start & 1st SSIT_SYNC_SLAVES command in RCDO
                           If SYNC point 2 is passed, it will extract the data between 1st & 2nd SSIT_SYNC_SLAVES command in RCDO */
                        CdoSequence* part_seq = cdoseq_extract_cdo_till_ssit_sync_slaves(master_cdo_seq, master_index + 1);
                        uint8_t* part_data = (uint8_t*)cdoseq_to_binary(part_seq, &part_size, 0);

                        size_t pad_bytes = 0;
                        master_chunk_size = part_size;
                        pad_bytes = ((16 - (p_offset & 15)) & 15);

                        size += (master_chunk_size+ pad_bytes);
                        p_buffer = (uint8_t*)realloc(p_buffer, size);

                        /* For master SLR, just copy the CDO contents to chunk. No need to create seperate DMA commands like slave SLRs */
                        if (pad_bytes != 0)
                        {
                            CdoCommandNop* cdoCmd = CdoCmdNoOperation(pad_bytes);
                            memcpy(p_buffer + p_offset, cdoCmd, CDO_CMD_NOP_SIZE);
                            p_offset += CDO_CMD_NOP_SIZE;
                            if (cdoCmd->header.length > 0)
                            {
                                memset(p_buffer + p_offset, 0, cdoCmd->header.length * sizeof(uint32_t));
                                p_offset += (cdoCmd->header.length * sizeof(uint32_t));
                            }
                        }
                        LOG_TRACE("Chunk-%d | SLR-master", chunk_num);
                        if(master_index < (*slr_info)->sync_addresses.size())
                        {
                            master_index++;
                        }
                        LogConfigSlrDetails(chunk_num++, SlrId::MASTER, p_offset, master_chunk_size, master_index);

                        /* As we strip the CDO header, we are replacing that with NOP commands to ensure other alignments are not disturbed for only the first chunk */
                        CdoCommandNop* cdoCmd = CdoCmdNoOperation(sizeof(VersalCdoHeader));
                        memcpy(p_buffer + p_offset, cdoCmd, CDO_CMD_NOP_SIZE);
                        memset(p_buffer + p_offset + CDO_CMD_NOP_SIZE, 0, sizeof(VersalCdoHeader) - CDO_CMD_NOP_SIZE);
                        memcpy(p_buffer + p_offset + sizeof(VersalCdoHeader), part_data + sizeof(VersalCdoHeader), part_size - sizeof(VersalCdoHeader));

                        p_offset += (master_chunk_size);
                        (*slr_info)->offset += (master_chunk_size);
                        master_file_size += master_chunk_size;
                        total_slr_chunk_size += master_chunk_size;
                        last_sync_point++;
                        if (current_sync_point == 0xFFFFFFFF)
                        {
                            break;
                        }
                    }
                }
                else
                {
                     last_sync_point++;
                }
            }
        }
        common_sync_point = current_sync_point;
        if (current_sync_point == 0xFFFFFFFF)
        {
            break;
        }
    }

    size += sizeof(CdoCommandHeader);
    p_buffer = (uint8_t*)realloc(p_buffer, size);
    CdoCommandHeader* cmd_end = CdoCmdCdoEnd();
    memcpy(p_buffer + p_offset, cmd_end, sizeof(CdoCommandHeader));

    /* Update CDO header lengths and checksum */
    cdoHeader->length = (size - sizeof(VersalCdoHeader)) / 4;
    cdoHeader->checksum = ~(cdoHeader->remaining_words + cdoHeader->id_word + cdoHeader->version + cdoHeader->length);
    memcpy(p_buffer, cdoHeader, sizeof(VersalCdoHeader));

    SetPartitionType(PartitionType::CONFIG_DATA_OBJ);
    PartitionHeader* partHdr =  new VersalPartitionHeader(this, 0);
    partHdr->execState = 0;
    partHdr->elfEndianess = 0;
    partHdr->firstValidIndex = true;
    partHdr->partitionSize = size;
    partHdr->partition = new VersalPartition(partHdr, p_buffer, size);
    partitionHeaderList.push_back(partHdr);

    delete[] p_buffer;
    delete cmd_end;

    for (std::vector<SsitConfigSlrInfo*>::iterator slr_info = configSlrsInfo.begin(); slr_info != configSlrsInfo.end(); slr_info++)
    {
        if ((*slr_info)->index != SlrId::MASTER)
        {
            LOG_TRACE("SSIT: total slr-%d chunks  = %d", (*slr_info)->index, (*slr_info)->num_chunks);
            if (bi.options.GetDumpOption() != DumpOption::SLAVE_PDIS)
            {
                remove ((*slr_info)->file.c_str());
            }
        }
        else
        {
            LOG_TRACE("SSIT: total master chunks = %d", (*slr_info)->num_chunks);
        }
    }

    PrintConfigSlrSummary();
}

/******************************************************************************/
void GetPartitionOffsets(SsitConfigSlrInfo* slr_info, uint8_t* data, size_t size)
{
    uint32_t* syncpt_offsets = NULL;
    uint8_t num_of_sync_points = 0;
    bool info_display = true;
    VersalImageHeaderTableStructure* iHT = (VersalImageHeaderTableStructure*)data;
    size_t offset = iHT->firstPartitionHeaderWordOffset * 4;
    slr_info->partition_sizes.push_back(sizeof(VersalImageHeaderTableStructure) + (iHT->totalMetaHdrLength * 4));
    for (uint8_t index = 0; index < iHT->partitionTotalCount; index++)
    {
        VersalPartitionHeaderTableStructure* pHT = (VersalPartitionHeaderTableStructure*)(data + offset);
        slr_info->partition_sizes.push_back(pHT->totalPartitionLength * 4);
        offset += sizeof(VersalPartitionHeaderTableStructure);
        if (((pHT->partitionAttributes >> vphtPartitionTypeShift) & vphtPartitionTypeMask) == PartitionType::CONFIG_DATA_OBJ)
        {
            CdoSequence * cdo_seq;
            cdo_seq = decode_cdo_binary(data + pHT->partitionWordOffset*4, pHT->totalPartitionLength*4);
            if (cdo_seq == NULL)
            {
                LOG_ERROR("decode_cdo_binary failed - %s", slr_info->file.c_str());
            }
            size_t buffer_size = 0;

            /* Enable the search for sync points - only needs to be done for SSIT devices */
            search_for_sync_points();

            uint8_t* buffer = (uint8_t*)cdoseq_to_binary(cdo_seq, &buffer_size, 0);

            /* Get no. of sync points and sync points offsets */
            num_of_sync_points = get_num_of_sync_points();
            syncpt_offsets = get_slr_sync_point_offsets();
            if (num_of_sync_points > 0)
            {
                if (info_display)
                {
                    LOG_TRACE("SSIT_SYNC_MASTER command detected at following offsets:");
                    LOG_TRACE("   slr_%d:", slr_info->index);
                    info_display = false;
                }
                for (int i = 0; i < num_of_sync_points; i++)
                {
                    size_t offset = (pHT->partitionWordOffset * 4) + (*(syncpt_offsets + i) * 4);
                    slr_info->sync_addresses.push_back(offset);
                    LOG_TRACE("       offset = 0x%x", offset);
                }
                delete syncpt_offsets;
                delete buffer;
            }
        }
    }
}

/******************************************************************************/
void VersalImageHeader::ParseSlrConfigFiles(size_t* slr_total_file_size)
{
    uint32_t slr_sync_points[4] = { 0, 0, 0, 0 };
    uint32_t* syncpt_offsets = NULL;
    uint8_t num_of_sync_points = 0;
    for (std::vector<SsitConfigSlrInfo*>::iterator slr_info = configSlrsInfo.begin(); slr_info != configSlrsInfo.end(); slr_info++)
    {
        if (IsCdoFile((*slr_info)->file))
        {
            /* For CDO files - master CDO */
            const char* cdo_filename = (*slr_info)->file.c_str();
            CdoSequence * cdo_seq;
            cdo_seq = cdoseq_load_cdo(cdo_filename);
            if (cdo_seq == NULL)
            {
                LOG_ERROR("Error parsing CDO file - %s", (*slr_info)->file.c_str());
            }
            search_for_sync_points();
            (*slr_info)->data = (uint8_t*)cdoseq_to_binary(cdo_seq, &(*slr_info)->size, 0);
            num_of_sync_points = get_num_of_sync_points();
            syncpt_offsets = get_slr_sync_point_offsets();
            LOG_TRACE("SSIT_SYNC_SLAVES command detected at following offsets:");
            slr_sync_points[0] = num_of_sync_points;
            for (int i = 0; i < num_of_sync_points; i++)
            {
                size_t offset = (*(syncpt_offsets + i) * 4);
                (*slr_info)->sync_addresses.push_back(offset);
                LOG_TRACE("       offset = 0x%x", offset);
            }
            /* As we strip the CDO HEADER, we are replacing that with NOP commands to ensure other alignments are not disturbed */
            CdoCommandNop* cdoCmd = CdoCmdNoOperation(sizeof(VersalCdoHeader));
            memcpy((*slr_info)->data, cdoCmd, CDO_CMD_NOP_SIZE);
            memset((*slr_info)->data + CDO_CMD_NOP_SIZE, 0, sizeof(VersalCdoHeader) - CDO_CMD_NOP_SIZE);
            delete syncpt_offsets;

            /* Add the size to accomodate the NOPs added at the start of each master chunk
               Since the master chunks are read using the cdoseq_to_binary, each chunk comes with a CDO header
               This CDO header is replaced with NOPs to not disturb the alignments of the rest of the commands in CDO
               No. of CDO headers replaced is equal to the number of master chunks which is equal to the number of sync points */
            (*slr_info)->size += ((num_of_sync_points) * sizeof(VersalCdoHeader));
        }
        else
        {
            /* For PDI files - slave PDIs */
            std::ifstream fl((*slr_info)->file.c_str(), std::ios::binary);
            fl.seekg(0, std::ios::end);
            if (fl.bad() || fl.fail())
            {
                LOG_ERROR("Cannot seek to end of file - %s", (*slr_info)->file.c_str());
            }
            (*slr_info)->size = fl.tellg();
            (*slr_info)->data = new uint8_t[(*slr_info)->size];
            fl.seekg(0, std::ios::beg);
            fl.read((char*)(*slr_info)->data, (*slr_info)->size);
            fl.close();
            GetPartitionOffsets((*slr_info), (*slr_info)->data, (*slr_info)->size);
            slr_sync_points[(*slr_info)->index] = (*slr_info)->sync_addresses.size();

            num_of_slrs++;
        }

        *slr_total_file_size += (*slr_info)->size;
    }

    /* Check for mismatch in sync points between different slaves and master */
    bool sync_point_mismatch_error = false;
    for (uint8_t i = 1; i < num_of_slrs; i++)
    {
        if (slr_sync_points[i] != slr_sync_points[0])
        {
            sync_point_mismatch_error = true;
        }
    }
    char * env_ss = getenv("BOOTGEN_SKIP_SSIT_SYNCPOINT_CHECK");
    if (env_ss != NULL)
    {
        LOG_WARNING("BOOTGEN_SKIP_SSIT_SYNCPOINT_CHECK is enabled. Skipping the check for sync point mismatch across SLRs.");
        if (sync_point_mismatch_error)
        {
            LOG_WARNING("The number of SYNC commands are not same across slave SLRs and master SLR.");
        }
    }
    else
    {
        if (sync_point_mismatch_error)
        {
            LOG_WARNING("Set env variable BOOTGEN_SKIP_SSIT_SYNCPOINT_CHECK to 1, to skip the check for sync point mismatch across SLRs.");
            LOG_ERROR("The number of SYNC commands are not same across slave SLRs and master SLR.");
        }
    }
}

/******************************************************************************/
uint32_t VersalImageHeader::FindCurrentSyncPoint(void)
{
    uint32_t temp = 0xFFFFFFFF;

    /* Check  */
    for (std::vector<SsitConfigSlrInfo*>::iterator slr_info = configSlrsInfo.begin(); slr_info != configSlrsInfo.end(); slr_info++)
    {
        /* Don't check sync points in MASTER */
        if ((*slr_info)->index != SlrId::MASTER)
        {
            if (((*slr_info)->sync_points < temp) && ((*slr_info)->eof != true))
            {
                temp = (*slr_info)->sync_points;
            }
        }
    }
    return temp;
}

/******************************************************************************/
void VersalImageHeader::CheckSyncPointInChunk(SsitConfigSlrInfo* slr_info, size_t size)
{
    /* Check if the current chunk has the identified sync point for the SLR
       If yes, increment the sync point.
       "sync_point" points to the no. of sync points processed
    */
    for (uint32_t ix = 0; ix < slr_info->sync_addresses.size(); ix++)
    {
        if ((slr_info->sync_addresses[ix] > slr_info->offset) && (slr_info->sync_addresses[ix] < (slr_info->offset + size)))
        {
            slr_info->sync_points++;
            LOG_TRACE("        sync point - %d",slr_info->sync_points);
        }
    }
}

/******************************************************************************/
void VersalImageHeader::SetSlrBootPartitions(std::list<SlrPdiInfo*> bootPdis)
{
    slrBootPdiInfo = bootPdis;
}

/******************************************************************************/
void VersalImageHeader::SetSlrConfigPartitions(std::list<SlrPdiInfo*> configPdis)
{
    slrConfigPdiInfo = configPdis;
}

/******************************************************************************/
void VersalImageHeader::SetDpacm(DpaCM::Type type)
{
    dpacm = type;
}

/******************************************************************************/
void VersalImageHeader::SetPufHdLocation(PufHdLoc::Type type)
{
    pufHdLoc = type;
}

/******************************************************************************/
void SubSysImageHeader::Build(BootImage& bi, Binary& cache)
{
    if (section != NULL)
    {
        bi.headers.push_back(section);
        bi.imageHeaderTable->metaHeaderLength += section->Length;
    }

    if (imgList.size() == 0)
    {
        LOG_ERROR("Partitions not specified in subsystem - %s", imageName.c_str());
    }
    bool uid_updated = false;
    for (std::list<ImageHeader*>::iterator image = imgList.begin(); image != imgList.end(); image++)
    {
        (*image)->Build(bi, cache);
        if(((*image)->GetImageId() != 0) && ((*image)->IsUidInfoFoundInCdo() == true) && (uid_updated == false))
        {
            if (uniqueId == 0xFFFFFFFF)
            {
                uniqueId = (*image)->GetUniqueId();
            }
            if (parentUniqueId == 0xFFFFFFFF)
            {
                parentUniqueId = (*image)->GetParentUniqueId();
            }
            if (functionId == 0xFFFFFFFF)
            {
                functionId = (*image)->GetFunctionId();
            }
            uid_updated = true;
        }

        if (!isPLPowerDomain)
        {
            isPLPowerDomain = (*image)->GetPLPowerDomainFlag();
        }
        if (!isFullPowerDomain)
        {
            isFullPowerDomain = (*image)->GetFullPowerDomainFlag();
        }
        if (!isLowPowerDomain)
        {
            isLowPowerDomain = (*image)->GetLowPowerDomainFlag();
        }
        if (!isSystemPowerDomain)
        {
            isSystemPowerDomain = (*image)->GetSystemPowerDomainFlag();
        }
    }

    if (subSysImageHeaderTable == NULL)
    {
        subSysImageHeaderTable = (VersalImageHeaderStructure*)section->Data;
        SetImageName();
        SetImageHeaderAttributes();
        SetPartitionHeaderOffset(0);
        SetMetaHdrRevokeId(bi.bifOptions->metaHdrAttributes.revokeId);
        SetMemCopyAddress();
        SetImageHeaderIds();
    }
}

/******************************************************************************/
void SubSysImageHeader::Link(BootImage &bi, SubSysImageHeader* nextHeader)
{
    subSysImageHeaderTable = (VersalImageHeaderStructure*)section->Data;
    if (imgList.front()->GetPartitionHeaderList().front()->section != NULL)
    {
        SetPartitionHeaderOffset((uint32_t)imgList.front()->GetPartitionHeaderList().front()->section->Address);
    }

    SetDataSectionCount();
    SetImageHeaderIds();
    SetReservedFields();
    SetChecksum();
}


/******************************************************************************/
void SubSysImageHeader::SetReservedFields(void)
{
    subSysImageHeaderTable->reserved = 0x0;
}

/******************************************************************************/
void SubSysImageHeader::SetChecksum(void)
{
    subSysImageHeaderTable->ihChecksum = ComputeWordChecksum(subSysImageHeaderTable, sizeof(VersalImageHeaderStructure) - sizeof(subSysImageHeaderTable->ihChecksum) /* bytes*/);
}

/******************************************************************************/
void SubSysImageHeader::SetPartitionHeaderOffset(uint32_t addr)
{
    subSysImageHeaderTable->partitionHeaderWordOffset = addr / sizeof(uint32_t);
}

/******************************************************************************/
void SubSysImageHeader::SetDataSectionCount(void)
{
    subSysImageHeaderTable->dataSectionCount = 0;
    for (std::list<ImageHeader*>::iterator imageItr = imgList.begin(); imageItr != imgList.end(); imageItr++)
    {
        subSysImageHeaderTable->dataSectionCount += (*imageItr)->NumPartitions();
    }
}

/******************************************************************************/
void SubSysImageHeader::SetMetaHdrRevokeId(uint32_t id)
{
    subSysImageHeaderTable->metaHdrRevokeId = id;
}

/******************************************************************************/
void SubSysImageHeader::SetImageHeaderIds(void)
{
    subSysImageHeaderTable->imageId = imageId;
    subSysImageHeaderTable->uniqueId = (uniqueId != 0xFFFFFFFF) ? uniqueId : 0;
    subSysImageHeaderTable->parentUniqueId = (parentUniqueId != 0xFFFFFFFF) ? parentUniqueId : 0;
    subSysImageHeaderTable->functionId = (functionId != 0xFFFFFFFF) ? functionId : 0;
}

/******************************************************************************/
void SubSysImageHeader::SetImageHeaderAttributes(void)
{
    if (delayLoad)
    {
        subSysImageHeaderTable->imageAttributes |= (1 << vihDelayLoadShift);
    }
    if (delayHandoff)
    {
        subSysImageHeaderTable->imageAttributes |= (1 << vihDelayHandoffShift);
    }
    if (memCopyAddr != 0xFFFFFFFFFFFFFFFF)
    {
        subSysImageHeaderTable->imageAttributes |= (1 << vihCopyToMemoryShift);
    }
    if (isFullPowerDomain)
    {
        subSysImageHeaderTable->imageAttributes |= (1 << vihLowPowerDomainShift);
    }
    if (isLowPowerDomain)
    {
        subSysImageHeaderTable->imageAttributes |= (1 << vihFullPowerDomainShift);
    }
    if (isSystemPowerDomain)
    {
        subSysImageHeaderTable->imageAttributes |= (1 << vihSystemPowerDomainShift);
    }
    if (isPLPowerDomain)
    {
        subSysImageHeaderTable->imageAttributes |= (1 << vihPLPowerDomainShift);
    }
}

/******************************************************************************/
void SubSysImageHeader::SetImageName(void)
{
    for (uint32_t i = 0; i<imageName.size(); i++)
    {
        subSysImageHeaderTable->imageName[i] = imageName[i];
    }
}

/******************************************************************************/
void SubSysImageHeader::SetMemCopyAddress(void)
{
    if (memCopyAddr == 0xFFFFFFFFFFFFFFFF)
    {
        memCopyAddr = 0;
    }
    subSysImageHeaderTable->memcpyAddressLo = memCopyAddr;
    subSysImageHeaderTable->memcpyAddressHi = memCopyAddr >> 32;
}

/******************************************************************************/
uint64_t SubSysImageHeader::GetMemCopyAddress(void)
{
    uint64_t mem_cpy_addr = 0;
    mem_cpy_addr = subSysImageHeaderTable->memcpyAddressHi;
    mem_cpy_addr = (mem_cpy_addr << 32) | (subSysImageHeaderTable->memcpyAddressLo);
    return mem_cpy_addr;
}

/******************************************************************************/
uint32_t SubSysImageHeader::GetPartitionHeaderOffset(void)
{
    return subSysImageHeaderTable->partitionHeaderWordOffset * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t SubSysImageHeader::GetDataSectionCount(void)
{
    return subSysImageHeaderTable->dataSectionCount;
}

/******************************************************************************/
uint32_t SubSysImageHeader::GetImageHeaderAttributes(void)
{
    return subSysImageHeaderTable->imageAttributes;
}

/******************************************************************************/
void SubSysImageHeader::SetSubSystemName(std::string name)
{
    imageName = name;
}

/******************************************************************************/
void SubSysImageHeader::SetSubSystemId(uint32_t id)
{
    imageId = id;
}

/******************************************************************************/
void SubSysImageHeader::SetSubSystemType(PartitionType::Type type)
{
    imageType = type;
}

/******************************************************************************/
void SubSysImageHeader::SetDelayHandoffMode(bool flag)
{
    delayHandoff = flag;
}

/******************************************************************************/
void SubSysImageHeader::SetDelayLoadMode(bool flag)
{
    delayLoad = flag;
}

/******************************************************************************/
void SubSysImageHeader::SetSubSysMemCopyAddress(uint64_t addr)
{
    memCopyAddr = addr;
}

/******************************************************************************/
uint64_t SubSysImageHeader::GetSubSysMemCopyAddress(void)
{
    return memCopyAddr;
}

/******************************************************************************/
bool SubSysImageHeader::GetDelayLoadMode(void)
{
    return delayLoad;
}

/******************************************************************************/
bool SubSysImageHeader::GetDelayHandoffMode(void)
{
    return delayHandoff;
}

/******************************************************************************/
PartitionType::Type SubSysImageHeader::GetSubSystemType(void)
{
    return imageType;
}

/******************************************************************************/
std::string SubSysImageHeader::GetSubSystemName(void)
{
    return imageName;
}

/******************************************************************************/
uint32_t SubSysImageHeader::GetSubSystemId(void)
{
    return imageId;
}

/******************************************************************************/
SubSysImageHeader::SubSysImageHeader(ImageBifOptions *imgOptions)
{
    subSysImageHeaderTable = NULL;

    imageName = imgOptions->GetImageName();
    imageId = imgOptions->GetImageId();
    delayLoad = imgOptions->GetDelayLoad();
    delayHandoff = imgOptions->GetDelayHandoff();
    memCopyAddr = imgOptions->GetMemCopyAddress();
    imageType = imgOptions->GetImageType();
    uniqueId = imgOptions->GetUniqueId();
    parentUniqueId = imgOptions->GetParentUniqueId();
    functionId = imgOptions->GetFunctionId();

    isFullPowerDomain = false;
    isLowPowerDomain = false;
    isSystemPowerDomain = false;
    isPLPowerDomain = false;

    std::string name = "ImageHeader " + imageName;
    uint32_t size = sizeof(VersalImageHeaderStructure);
    section = new Section(name, size);
    memset(section->Data, 0, size);
    imgList.clear();
    num_of_images = 0;
}

/******************************************************************************/
SubSysImageHeader::SubSysImageHeader(std::ifstream& ifs)
{
    VersalImageHeaderStructure importedSubSysHeaderTable;

    long pos = ifs.tellg();
    ifs.read((char*)&importedSubSysHeaderTable, 4 * sizeof(uint32_t));

    char buffer[4];
    char name[256];
    uint32_t i = 0;

    do
    {
        ifs.read(buffer, 4);
        name[i + 0] = buffer[0];
        name[i + 1] = buffer[1];
        name[i + 2] = buffer[2];
        name[i + 3] = buffer[3];
        i += 4;
    } while (buffer[0] && buffer[1] && buffer[2] && buffer[3] && i<sizeof(name));

    if (i >= sizeof(name))
    {
        LOG_DEBUG(DEBUG_STAMP, "Image header name too long");
        LOG_ERROR("Failure parsing imported bootimage");
    }
    imageName = name;
    uint32_t size = sizeof(VersalImageHeaderStructure);

    ifs.seekg(pos);
    section = new Section("ImageHeader " + imageName, size);
    subSysImageHeaderTable = (VersalImageHeaderStructure*)section->Data;
    ifs.read((char*)subSysImageHeaderTable, size);

    imageId = subSysImageHeaderTable->imageId;
    uniqueId = subSysImageHeaderTable->uniqueId;
    parentUniqueId = subSysImageHeaderTable->parentUniqueId;
    functionId = subSysImageHeaderTable->functionId;
    delayLoad = ((subSysImageHeaderTable->imageAttributes >> vihDelayLoadShift) & vihDelayLoadMask);
    delayHandoff = ((subSysImageHeaderTable->imageAttributes >> vihDelayHandoffShift) & vihDelayHandoffMask);
    memCopyAddr = (uint64_t) (subSysImageHeaderTable->memcpyAddressLo) >> 32;
    memCopyAddr |= subSysImageHeaderTable->memcpyAddressHi;
    isPLPowerDomain = ((subSysImageHeaderTable->imageAttributes >> vihPLPowerDomainShift) & vihPLPowerDomainMask);
    isFullPowerDomain = ((subSysImageHeaderTable->imageAttributes >> vihFullPowerDomainShift) & vihFullPowerDomainMask);
    isLowPowerDomain = ((subSysImageHeaderTable->imageAttributes >> vihLowPowerDomainShift) & vihLowPowerDomainMask);
    isSystemPowerDomain = ((subSysImageHeaderTable->imageAttributes >> vihSystemPowerDomainShift) & vihSystemPowerDomainMask);

    /* Find the no. of image headers to be created */
    num_of_images = 0;
    uint32_t num_of_sections = 0;
    uint32_t p_count = subSysImageHeaderTable->dataSectionCount;
    uint32_t p_offset = subSysImageHeaderTable->partitionHeaderWordOffset * sizeof(uint32_t);
    p_offset += 0x28;
    for (uint32_t i = 0; i < p_count; i++)
    {
        ifs.seekg(p_offset);
        ifs.read((char*)&num_of_sections, 4);
        if (num_of_sections != 0)
        {
            num_of_images++;
        }
        p_offset += sizeof(VersalPartitionHeaderTableStructure);
    }
}

/******************************************************************************/
void VersalImageHeader::CheckIdsInCdo(CdoSequence * seq)
{
    LINK * l = seq->cmds.next;
    while (l != &seq->cmds) 
    {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if(cmd->type == CdoCmdPmInitNode)
        {
            imageId = u32le(cmd->id);
            if (cmd->value == 0)
            {
                SetPowerDomains((uint8_t*)cmd->buf,cmd->count);
            }
        }
        if(cmd->type == CdoCmdLdrSetImageInfo)
        {
            if (imageId == u32le(cmd->id))
            {
                uniqueId = u32le(cmd->value);
                parentUniqueId = u32le(cmd->mask);
                functionId = u32le(cmd->count);
                uidInfoFoundInCdo = true;
            }
            break;
        }
    }
}

/******************************************************************************/
void VersalImageHeader::SetPowerDomains(uint8_t* buf, uint32_t count)
{
    uint32_t id = 0;
    for (uint32_t index = 0; index < count ; index++)
    {
        id = ReadLittleEndian32(buf);
        switch (id)
        {
        case POWER_FPD:
            isIhFullPowerDomain = true;
            break;
        case POWER_LPD:
            isIhLowPowerDomain = true;
            break;
        case POWER_SPD:
            isIhSystemPowerDomain = true;
            break;
        case POWER_PLD:
            isIhPLPowerDomain = true;
            break;
        default:
            break;
        }
        buf += sizeof(uint32_t);
    }
}
