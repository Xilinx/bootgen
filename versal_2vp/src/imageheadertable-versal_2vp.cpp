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
#include "imageheadertable-versal_2vp.h"
#include "authentication-versal_2vp.h"
#include "bootimage.h"
#include "bootheader-versal_2vp.h"
#include "elftools.h"
#include <string.h>
#include <sstream>
#include "stringutils.h"
#include "fileutils.h"
#include "bitutils.h"
#include "bitutils-versal_2vp.h"
#include "partitionheadertable-versal_2vp.h"
#include "encryptutils.h"
#include "cJSON.h"
extern "C" {
#include "cdo-npi.h"
#include "cdo-source.h"
#include "cdo-command.h"
#include "cdo-binary.h"
#include "cdo-load.h"
#include "cdo-overlay.h"
};

#include "cdoutils.h"
#include <memory>

static uint8_t bufferIndex = 0;
std::list<CdoCommandDmaWrite*> Versal_2vpImageHeader::cdoSections;

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
Versal_2vpImageHeaderTable::Versal_2vpImageHeaderTable()
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
    section = new Section("MetaHeader", sizeof(Versal_2vpImageHeaderTableStructure));
    iHTable = (Versal_2vpImageHeaderTableStructure*)section->Data.get();
}

/******************************************************************************/
Versal_2vpImageHeaderTable::Versal_2vpImageHeaderTable(std::ifstream& src)
{
    prebuilt = true;
    section = new Section("MetaHeader", sizeof(Versal_2vpImageHeaderTableStructure));
    iHTable = (Versal_2vpImageHeaderTableStructure*)section->Data.get();
    
    /* Import the Image Header Table from a boot image file */
    src.read((char*)section->Data.get(), section->Length);
    creatorId = (iHTable->imageHeaderTableAttributes >> vihtImageCreatorIdShift) & vihtImageCreatorIdMask;
    parentId = iHTable->parentId;
    pdiId = iHTable->pdiId;
    idCode = iHTable->idCode;
    extendedIdCode = iHTable->extendedIdCode;
    iht_optional_data_length = iHTable->optionalDataSize * 4;
    bypassIdCode = (iHTable->imageHeaderTableAttributes >> vihtSiliconRevisionIdCodeCheckShift) & vihtSiliconRevisionIdCodeCheckMask;
    bootDevice = (iHTable->imageHeaderTableAttributes >> vihtSecBootDeviceShift) & vihtSecBootDeviceMask;
    dpacm = ((iHTable->imageHeaderTableAttributes >> vihtDpacmEnableShift) & vihtDpacmEnableMask) == 3 ? DpaCM::DpaCMEnable : DpaCM::DpaCMDisable;
    pufHDLoc = ((iHTable->imageHeaderTableAttributes >> vihtPufHDLocationShift) & vihtPufHDLocationMask) == 3 ? PufHdLoc::PUFinBH : PufHdLoc::PUFinEFuse;
    idCodeCheck = (iHTable->imageHeaderTableAttributes >> vihtIdCodeCheckShift) & vihtIdCodeCheckMask;
    kekIvMust = false;

    hashBlockSectionLength = iHTable->hashBlockLength * 4;

    if (hashBlockSectionLength != 0)
    {
        src.seekg(iHTable->hashBlockOffset * 4);
        hashBlockSection = new Section("HashBlock1", hashBlockSectionLength);
        src.read((char*)hashBlockSection->Data.get(), hashBlockSectionLength);
    }

}

/******************************************************************************/
Versal_2vpImageHeaderTable::~Versal_2vpImageHeaderTable()
{
    /* section ownership transferred to bi.headers in Build() - do not delete */
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::Build(BootImage& bi, Binary& cache)
{
    bool isApuOrRpuSubsystem = false;

    /* Push IHT to cache (not bi.headers) so it remains plaintext and is not encrypted.
       This is consistent with versal_2ve_2vm behavior.
       PDI ordering is handled via offsets in the Link function. */
    if (section != NULL)
    {
        cache.Sections.push_back(std::unique_ptr<Section>(section));
    }

    if (!prebuilt)
    {
        pdiId = bi.bifOptions->GetPdiId();
        parentId = bi.bifOptions->GetParentId();
        idCode = bi.bifOptions->GetIdCode();
        extendedIdCode = bi.bifOptions->GetExtendedIdCode();
        bypassIdCode = bi.bifOptions->GetBypassIdcodeFlag();
        bootDevice = bi.bifOptions->GetBootDevice();

        SetImageHeaderTableVersion(VERSION_v1_00_TELLURIDE);

        SetHeaderTablesSize();
        SetTotalMetaHdrLength(0);
        SetIdentificationString(bi.IsBootloaderFound());
        SetIds((bi.imageList.size() != 0) && true);
        metaHdrKeySrc = bi.options.bifOptions->metaHdrAttributes.encrKeySource;
        SetMetaHdrSecureHdrIv(metaHdrSecHdrIv.get());
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
        SetIds((bi.imageList.size() != 0) && true);

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

        SetOptionalData(bi.iht_optional_data.get(), bi.iht_optional_data_length);
    }

    /* Sub system Image Header creation */
    if (bi.createSubSystemPdis == true)
    {
        for (std::list<SubSysImageHeader*>::iterator subsysimage = bi.subSysImageList.begin(); subsysimage != bi.subSysImageList.end(); subsysimage++)
        {
            (*subsysimage)->Build(bi, cache);
            isApuOrRpuSubsystem = false;
            for (std::list<ImageHeader*>::iterator img = (*subsysimage)->imgList.begin(); img != (*subsysimage)->imgList.end(); img++)
            {
                ValidateSecurityCombinations((*img)->GetAuthenticationType(), (*img)->GetEncryptContext()->Type(),
                    (*img)->GetChecksumContext()->Type());
                if ((*img)->GetDestCpu() == DestinationCPU::A53_0 || (*img)->GetDestCpu() == DestinationCPU::A53_1 || (*img)->GetDestCpu() == DestinationCPU::R5_0 || (*img)->GetDestCpu() == DestinationCPU::R5_1 || (*img)->GetDestCpu() == DestinationCPU::R5_lockstep)
                {
                    isApuOrRpuSubsystem = true;
                }    
            }
            if (isApuOrRpuSubsystem == false)
            {
               nonApuSubSysCnt++;
            }
        }
        if (nonApuSubSysCnt == 0)
        {
           idCodeCheck = 3;
        }
        else
        {
           idCodeCheck = 0; 
        }
    }
    else
    {
        for (std::list<ImageHeader*>::iterator image = bi.imageList.begin(); image != bi.imageList.end(); image++)
        {
            (*image)->Build(bi, cache);
            ValidateSecurityCombinations((*image)->GetAuthenticationType(), (*image)->GetEncryptContext()->Type(), (*image)->GetChecksumContext()->Type());
            if ((*image)->GetDestCpu() == DestinationCPU::A53_0 || (*image)->GetDestCpu() == DestinationCPU::A53_1 || (*image)->GetDestCpu() == DestinationCPU::R5_0 || (*image)->GetDestCpu() == DestinationCPU::R5_1 || (*image)->GetDestCpu() == DestinationCPU::R5_lockstep)
            {
               idCodeCheck = 3;
            }
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

    //if ((bi.GetDeviceArchitecture() == Arch::TELLURIDE)) //|| (bi.options.IsAuthOptimizationEnabled()))
    {
        auto hash = std::make_unique<uint8_t[]>(bi.hash->GetHashLength());
        bi.hashTable.push_back(std::make_pair(uint32_t(0), std::move(hash)));
        bi.numHashTableEntries++;

        uint32_t i = 0;
        for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++, i++)
        {
            if (!(*partHdr)->IsBootloader())
            {
                (*partHdr)->partitionNum = i;
                (*partHdr)->section->partitionNum = i;
                bi.numHashTableEntries++;
            }
        }
    }

    bi.imageHeaderTable->SetUserOptionalData(bi.bifOptions->metaHdrAttributes.ihtOptionalDataInfo);
    bi.iht_optional_data_length = iht_optional_data_length;
    bi.copied_iht_optional_data_length = copied_iht_optional_data_length;
    bi.iht_optional_data = std::make_unique<uint32_t[]>(bi.iht_optional_data_length / sizeof(uint32_t));
    memcpy(bi.iht_optional_data.get(), iht_optional_data.get(), iht_optional_data_length);

    SetReservedFields();
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::Link(BootImage &bi)
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

    SetHashBlockLength(bi.imageHeaderTable->hashBlockSectionLength);
    SetHashBlockOffset(bi.imageHeaderTable->hashBlockSection->WordAddress());

    SetBootDevice(bi.bifOptions->GetBootDevice());
    SetBootDeviceAddress(bi.bifOptions->GetBootDeviceAddress());
    SetMetaHdrKeySrc(bi.imageHeaderTable->metaHdrKeySrc, bi.bifOptions);
    SetMetaHdrGreyOrBlackIv(kekIvFile);
    SetImageHeaderTableAttributes();
    SetMetaHdrAuthenticationFields(bi);  // Populate IHT auth fields for Meta Header
    SetOptionalDataSize();
    SetChecksum();
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetImageHeaderTableVersion(uint32_t version)
{
    iHTable->version = version;
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetHeaderTablesSize()
{
    iHTable->headerSizes = (sizeof(Versal_2vpImageHeaderTableStructure)/4) | (sizeof(Versal_2vpImageHeaderStructure)/4 << 8) | (sizeof(Versal_2vpPartitionHeaderTableStructure)/4 << 16);
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetTotalMetaHdrLength(uint32_t size)
{
    iHTable->totalMetaHdrLength = size / sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetHashBlockLength(uint32_t size)
{
    iHTable->hashBlockLength = size / sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetPartitionCount(uint32_t count)
{
    iHTable->partitionTotalCount = count;
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetImageCount(uint32_t count)
{
    iHTable->imageTotalCount = count;
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetFirstImageHeaderOffset(uint32_t offset)
{
    iHTable->firstImageHeaderWordOffset = offset;
    if (slaveBootSplitMode && (iHTable->firstImageHeaderWordOffset != 0))
    {
        iHTable->firstImageHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetFirstPartitionHeaderOffset(uint32_t offset)
{
    iHTable->firstPartitionHeaderWordOffset = offset;
    if (slaveBootSplitMode && (iHTable->firstPartitionHeaderWordOffset != 0))
    {
        iHTable->firstPartitionHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetBootDeviceAddress(uint32_t address)
{
    iHTable->secondaryBootDeviceAddress = address;
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetIds(bool warnIdCode)
{
    if (idCode == 0 && warnIdCode)
    {
        LOG_WARNING("id_code is not specified in BIF, default id code is 0x04ca8093");
        idCode = DEFAULT_ID_CODE_S80;
    }
    iHTable->idCode = idCode;
    iHTable->extendedIdCode = extendedIdCode;
    iHTable->parentId = parentId;
    iHTable->pdiId = pdiId;
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetImageHeaderTableAttributes()
{
    iHTable->imageHeaderTableAttributes = bypassIdCode;
    iHTable->imageHeaderTableAttributes |= ((creatorId & 0x0F) << 1);
    iHTable->imageHeaderTableAttributes |= ((bootDevice & vihtSecBootDeviceMask) << vihtSecBootDeviceShift);
    iHTable->imageHeaderTableAttributes |= ((dpacm & vihtDpacmEnableMask) << vihtDpacmEnableShift);
    iHTable->imageHeaderTableAttributes |= ((pufHDLoc & vihtPufHDLocationMask) << vihtPufHDLocationShift);
    iHTable->imageHeaderTableAttributes |= ((idCodeCheck & vihtIdCodeCheckMask) << vihtIdCodeCheckShift);
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetMetaHdrSecureHdrIv(uint8_t* iv)
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
void Versal_2vpImageHeaderTable::SetMetaHdrKeySrc(KeySource::Type keyType, BifOptions* bifOptions)
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
void Versal_2vpImageHeaderTable::SetMetaHdrGreyOrBlackIv(std::string ivFile)
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
void Versal_2vpImageHeaderTable::SetHeaderAuthCertificateOffset(uint32_t address)
{
    iHTable->headerAuthCertificateWordOffset = address;
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetHashBlockOffset(uint32_t address)
{
    iHTable->hashBlockOffset = address;
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetPdiId(uint32_t id)
{
    pdiId = id;
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetParentId(uint32_t id)
{
    parentId = id;
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetIdentificationString(bool bootloaderFound)
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
void Versal_2vpImageHeaderTable::SetCreatorId(uint8_t id)
{
    creatorId = id;
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetReservedFields(void)
{
    /* Per versal_2vp PDI spec: 0x64-0x78 are reserved and should be 0 */
    memset(iHTable->reserved, 0x00, sizeof(iHTable->reserved));
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetMetaHdrAuthenticationFields(BootImage& bi)
{
    /* Per versal_2vp PDI spec: IHT only has hashBlockLength (0x5C) and hashBlockOffset (0x60)
       for Meta Header authentication. The authentication details are in the Auth Certificate.
       Offsets 0x64-0x78 are reserved and should be 0. */
    
    // hashBlockLength and hashBlockOffset are set separately via SetHashBlockLength/SetHashBlockOffset
    // Reserved fields (0x64-0x78) are set in SetReservedFields()
    
    if (bi.metaHdrAuthCtx != nullptr && bi.metaHdrAuthCtx->authAlgorithm != nullptr)
    {
        uint32_t authHdr = bi.metaHdrAuthCtx->authAlgorithm->GetAuthHeader();
        LOG_INFO("IHT Meta Header: Authentication enabled (authHeader=0x%X)", authHdr);
    }
    else
    {
        LOG_TRACE("IHT Meta Header: Non-secure (no authentication context)");
    }
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetChecksum(void)
{
    iHTable->ihtChecksum = ComputeWordChecksum(iHTable, sizeof(Versal_2vpImageHeaderTableStructure) - sizeof(iHTable->ihtChecksum) /* bytes*/);
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetOptionalDataSize()
{
    iHTable->optionalDataSize = iht_optional_data_length / 4;
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetXplmModulesData(BootImage& bi, uint32_t * data, uint32_t size)
{
    if (size != 0)
    {
        bi.xplm_modules_data_length = size;
        bi.xplm_modules_data = std::make_unique<uint32_t[]>(bi.xplm_modules_data_length / sizeof(uint32_t));
        memcpy(bi.xplm_modules_data.get(), data, bi.xplm_modules_data_length);
    }
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetOptionalData(uint32_t * data, uint32_t size)
{
    iht_optional_data_length = size;
    if (size != 0)
    {
        iht_optional_data = std::make_unique<uint32_t[]>(iht_optional_data_length / sizeof(uint32_t));
        memcpy(iht_optional_data.get(), data, iht_optional_data_length);
    }
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::SetUserOptionalData(std::vector<std::pair<std::string, uint32_t>> optionalDataInfo, uint32_t numHashTableEntries, uint32_t isCorePsm)
{
    for (size_t i = 0; i < optionalDataInfo.size(); i++)
    {
        size_t size = 0;
        void *data = file_to_buf((char *)optionalDataInfo[i].first.c_str(), &size);
        if (size != 0)
        {
            uint32_t sectn_size_id = 0;
            /* Optional Data Header + Optional Data Actual size + Checksum */
            uint16_t sectn_length = sizeof(uint32_t) + size + sizeof(uint32_t);
            sectn_size_id = (uint32_t)((sectn_length / 4) << 16) | (optionalDataInfo[i].second);

            auto new_data = std::make_unique<uint32_t[]>((iht_optional_data_length + sectn_length) / sizeof(uint32_t));
            if (iht_optional_data)
                memcpy(new_data.get(), iht_optional_data.get(), iht_optional_data_length);
            iht_optional_data = std::move(new_data);
            memcpy(iht_optional_data.get() + (iht_optional_data_length / 4), &sectn_size_id, sizeof(uint32_t));
            memcpy(iht_optional_data.get() + (iht_optional_data_length / 4) + sizeof(uint32_t) / 4, data, size);

            uint32_t checksum = ComputeWordChecksum(iht_optional_data.get() + (iht_optional_data_length / 4), sectn_length - sizeof(uint32_t));
            memcpy(iht_optional_data.get() + (iht_optional_data_length / 4) + (sectn_length - sizeof(uint32_t)) / 4, &checksum, sizeof(uint32_t));

            iht_optional_data_length += sectn_length;
        }
    }

    if (iht_optional_data_length != 0)
    {
        uint32_t padLength = (iht_optional_data_length % 64 != 0) ? 64 - (iht_optional_data_length % 64) : 0;
        auto new_data = std::make_unique<uint32_t[]>((iht_optional_data_length + padLength) / sizeof(uint32_t));
        if (iht_optional_data)
            memcpy(new_data.get(), iht_optional_data.get(), iht_optional_data_length);
        iht_optional_data = std::move(new_data);
        memset(iht_optional_data.get() + (iht_optional_data_length / 4), 0xFF, padLength);
        iht_optional_data_length += padLength;

        section->IncreaseLengthAndPadTo(sizeof(Versal_2vpImageHeaderTableStructure) + iht_optional_data_length, 0);
        memcpy(section->Data.get() + sizeof(Versal_2vpImageHeaderTableStructure), iht_optional_data.get(), iht_optional_data_length);

        iHTable = (Versal_2vpImageHeaderTableStructure*)section->Data.get();
        iHTable->optionalDataSize = iht_optional_data_length / 4;
    }
    LOG_TRACE("User optional data is processed");
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::RealignSectionDataPtr(void)
{
    iHTable = (Versal_2vpImageHeaderTableStructure*)section->Data.get();
}

/******************************************************************************/
uint32_t Versal_2vpImageHeaderTable::GetImageHeaderTableVersion(void)
{
    return iHTable->version;
}

/******************************************************************************/
uint32_t Versal_2vpImageHeaderTable::GetPartitionCount(void)
{
    return iHTable->partitionTotalCount;
}

/******************************************************************************/
uint32_t Versal_2vpImageHeaderTable::GetImageCount(void)
{
    return iHTable->imageTotalCount;
}

/******************************************************************************/
uint32_t Versal_2vpImageHeaderTable::GetFirstImageHeaderOffset(void)
{
    return iHTable->firstImageHeaderWordOffset;
}

/******************************************************************************/
uint32_t Versal_2vpImageHeaderTable::GetTotalMetaHdrLength()
{
    return (iHTable->totalMetaHdrLength) * sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2vpImageHeaderTable::ValidateSecurityCombinations(Authentication::Type authType, Encryption::Type encryptType,
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
Versal_2vpImageHeader::Versal_2vpImageHeader(std::string& filename)
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
    uint32_t size = sizeof(Versal_2vpImageHeaderStructure);

    section = new Section("ImageHeader " + Name, size);
    memset(section->Data.get(), 0, size);
}

/******************************************************************************/
Versal_2vpImageHeader::Versal_2vpImageHeader(uint8_t* data, uint64_t len)
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
    uint32_t size = sizeof(Versal_2vpImageHeaderStructure);

    section = new Section("ImageHeader " + Name, size);
    memset(section->Data.get(), 0, size);
}

/******************************************************************************/
Versal_2vpImageHeader::Versal_2vpImageHeader(std::ifstream& ifs, bool IsBootloader)
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
    Versal_2vpImageHeaderStructure importedIH;

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
    uint32_t size = sizeof(Versal_2vpImageHeaderStructure);

    ifs.seekg(pos);
    section = new Section("ImageHeader " + Name, size);
    imageHeader = (Versal_2vpImageHeaderStructure*)section->Data.get();
    ifs.read((char*)imageHeader, size);

    uint32_t count = 0;
    if (imageHeader->dataSectionCount < 32)
    {
        count = imageHeader->dataSectionCount;
    }

    uint32_t offset = imageHeader->partitionHeaderWordOffset * sizeof(uint32_t);

    for (uint8_t index = 0; index < count; index++)
    {
        Bootloader = IsBootloader;

        Versal_2vpPartitionHeader* hdr = new Versal_2vpPartitionHeader(this, index);
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
Versal_2vpImageHeader::Versal_2vpImageHeader(std::ifstream& ifs, Versal_2vpImageHeaderStructure* importedIH, bool IsBootloader, uint32_t img_index)
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
    uint32_t size = sizeof(Versal_2vpImageHeaderStructure);

    section = new Section("ImageHeader " + Name, size);
    imageHeader = (Versal_2vpImageHeaderStructure*)section->Data.get();
    memcpy(imageHeader, importedIH, size);
    
    uint32_t offset =  (imageHeader->partitionHeaderWordOffset * sizeof(uint32_t)) + (img_index * sizeof(Versal_2vpPartitionHeaderTableStructure));

    Versal_2vpPartitionHeaderTableStructure* tempPHT = new Versal_2vpPartitionHeaderTableStructure;
    ifs.seekg(offset);
    ifs.read((char*)tempPHT, sizeof(Versal_2vpPartitionHeaderTableStructure));

    uint32_t count = 0;
    if (imageHeader->dataSectionCount < 32)
    {
        count = tempPHT->dataSectionCount;
    }

    delete tempPHT;

    for (uint8_t index = 0; index < count; index++)
    {
        Bootloader = IsBootloader;

        Versal_2vpPartitionHeader* hdr = new Versal_2vpPartitionHeader(this, index);
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
        cluster = hdr->GetDestinationCluster();
        lockstep = hdr->GetLockStepFlag();
    }
}
/******************************************************************************/
Versal_2vpImageHeader::~Versal_2vpImageHeader()
{
}

/******************************************************************************/
uint32_t Versal_2vpImageHeader::GetImageHeaderAttributes(void)
{
    return imageHeader->imageAttributes;
}

/******************************************************************************/
void Versal_2vpImageHeader::SetAuthBlock(size_t blockSize, bool flag)
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
void Versal_2vpImageHeader::SetPartitionUid(uint32_t id)
{
    partitionUid = id;
}

/******************************************************************************/
void Versal_2vpImageHeader::SetEncryptionKeySrc(KeySource::Type type)
{
    keySrc = type;
    if (type != KeySource::None)
    {
        LOG_TRACE("Setting Encryption Key Source as %d", type);
    }
}

/******************************************************************************/
void Versal_2vpImageHeader::SetImageId(void)
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
void Versal_2vpImageHeader::SetPartitionRevocationId(uint32_t id)
{
    partitionRevokeId = id;
}

/******************************************************************************/
void Versal_2vpImageHeader::SetSpkRevocationId(uint32_t id)
{
    spkRevokeId = id;
}

/******************************************************************************/
void Versal_2vpImageHeader::SetMemCopyAddr()
{
    if (ihMemCpyAddr == 0xFFFFFFFFFFFFFFFF)
    {
        ihMemCpyAddr = 0;
    }
    imageHeader->memcpyAddressLo = ihMemCpyAddr;
    imageHeader->memcpyAddressHi = ihMemCpyAddr >> 32;
}

/******************************************************************************/
uint32_t Versal_2vpImageHeader::GetPartitionUid(void)
{
    return partitionUid;
}

/******************************************************************************/
KeySource::Type Versal_2vpImageHeader::GetEncryptionKeySrc(void)
{
    return keySrc;
}

/******************************************************************************/
uint32_t Versal_2vpImageHeader::GetPartitionRevocationId()
{
    return partitionRevokeId;
}

/******************************************************************************/
uint32_t Versal_2vpImageHeader::GetSpkRevocationId()
{
    return spkRevokeId;
}

/******************************************************************************/
DpaCM::Type Versal_2vpImageHeader::GetDpacm(void)
{
    return dpacm;
}

/******************************************************************************/
PufHdLoc::Type Versal_2vpImageHeader::GetPufHdLocation(void)
{
    return pufHdLoc;
}

/******************************************************************************/
std::string Versal_2vpImageHeader::GetKekIV(void)
{
    return kekIvFile;
}

/******************************************************************************/
void Versal_2vpImageHeader::Build(BootImage& bi, Binary& cache)
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
        imageHeader = (Versal_2vpImageHeaderStructure*)section->Data.get();
        SetImageName();
        SetImageHeaderAttributes();
        SetDataSectionCount(0);
        SetPartitionHeaderOffset(0);
        SetMetaHdrRevokeId(bi.bifOptions->metaHdrAttributes.partitionRevokeId);
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
void Versal_2vpImageHeader::ImportBin(BootImage& bi)
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
        //if (PostProcessCdo(tempBuffer, data.len)) return;
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
        //if (PostProcessCfi(tempBuffer, data.len)) return;
        exec_addr = 0;
    }
    else
    {
        memcpy(tempBuffer, data.bytes, alignlen);
    }
    PartitionHeader* hdr = new Versal_2vpPartitionHeader(this, 0);
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
    hdr->partition = std::make_unique<Versal_2vpPartition>(hdr, tempBuffer, alignlen);
    hdr->partitionSize = alignlen;
    delete[] tempBuffer;
    SetLoadAndExecAddress(hdr);
    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
void Versal_2vpImageHeader::ImportBuffer(BootImage& bi)
{
    SetDomain(Domain::PS);

    PartitionHeader* hdr = new Versal_2vpPartitionHeader(this, 0);
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
    hdr->partition = std::make_unique<Versal_2vpPartition>(hdr, buffer, bufferSize);
    hdr->partitionSize = bufferSize;
    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
void Versal_2vpImageHeader::ImportNpi(BootImage& bi)
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
    BitFile *bit = new Versal_2vpBitFile(stream);
    bit->ParseBit(bi);
    bi.bitFilename = Filename.c_str();

    /* The endianess is different (Big Endian) in case of Zynq MP/FPGA encryption cases. All other cases the
    the bitstream is copied as Little Endian */
    OutputStream *os = bit->GetOutputStreamType().get();
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

    //if (PostProcessCdo(os->Start(), os->Size())) return;

    PartitionHeader* hdr = new Versal_2vpPartitionHeader(this, 0);
    hdr->firstValidIndex = true;
    hdr->execAddress = 0;

    /* Padding of the partition, based on encryption */
    hdr->partitionSize = os->Size();

    // Larger size, if needed.
    hdr->transferSize = os->Size();
    hdr->preservedBitstreamHdr = os->pHdr;

    hdr->partition = std::make_unique<Versal_2vpPartition>(hdr, os->Start(), os->Size());

    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
void Versal_2vpImageHeader::ImportBit(BootImage& bi)
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
    BitFile *bit = new Versal_2vpBitFile(stream);
    bit->ParseBit(bi);
    bi.bitFilename = Filename.c_str();

    /* The endianess is different (Big Endian) in case of Zynq MP/FPGA encryption cases. All other cases the
    the bitstream is copied as Little Endian */
    OutputStream *os = bit->GetOutputStreamType().get();
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

    //if (PostProcessCfi(os->Start(), os->Size())) return;

    PartitionHeader* hdr = new Versal_2vpPartitionHeader(this, 0);
    hdr->firstValidIndex = true;
    hdr->execAddress = 0;

    /* Padding of the partition, based on encryption */
    hdr->partitionSize = os->Size();

    // Larger size, if needed.
    hdr->transferSize = os->Size();
    hdr->preservedBitstreamHdr = os->pHdr;
    SetPartitionType(PartitionType::CFI);
    hdr->partition = std::make_unique<Versal_2vpPartition>(hdr, os->Start(), os->Size());

    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
void Versal_2vpImageHeader::ParseFileToImport(BootImage& bi)
{
    if (destCpu == DestinationCPU::AIE)
    {
        static bool warning_given = false;
        char * bootgen_aie_base_addr = getenv("BOOTGEN_AIE_BASE_ADDR");
        if (bootgen_aie_base_addr != NULL)
        {
            aie_array_base_address = strtoull(bootgen_aie_base_addr, NULL, 16);
            if (!warning_given)
            {
                LOG_WARNING("BOOTGEN_AIE_BASE_ADDR is set to 0x%llx", aie_array_base_address);
                warning_given = true;
            }
        }
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
    else if (partitionType == PartitionType::IMAGE_STORE_PDI)
    {
        CreateWriteImageStorePartition();
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
            else if (!IsElf(line) && (partitionType == PartitionType::RAW))
            {
                ImportBin(bi);
            }
            else if ((line.find("Xilinx ASCII Bitstream") != std::string::npos) || (line.find("Xilinx ASCII CFI Deviceimage") != std::string::npos))
            {
                ImportBit(bi);
            }
            else if ((line.find("Xilinx ASCII NPI Deviceimage") != std::string::npos) || (line.find("Xilinx ASCII PSAXIMM Deviceimage") != std::string::npos)
                || (line.find("Xilinx ASCII CDO") != std::string::npos) || (line.find("version") != std::string::npos) || (GetPartitionType() == PartitionType::CONFIG_DATA_OBJ))
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
            stream.close();
        }
        else
        {
            ImportBuffer(bi);
        }
    }
}

/******************************************************************************/
void Versal_2vpImageHeader::ImportCdoSource(BootImage& bi)
{
    uint8_t* buffer = NULL;
    size_t size = 0;
    buffer = DecodeCdo(Filename, &size);
    //if (PostProcessCdo(buffer, size)) return;
    PartitionHeader* hdr = new Versal_2vpPartitionHeader(this, 0);
    hdr->firstValidIndex = true;
    hdr->loadAddress = 0xFFFFFFFFFFFFFFFF;
    hdr->execAddress = 0;

    hdr->partition = std::make_unique<Versal_2vpPartition>(hdr, buffer, size);
    hdr->partitionSize = size;
    delete[] buffer;
    SetLoadAndExecAddress(hdr);
    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
void Versal_2vpImageHeader::ImportCdo(BootImage& bi)
{
    uint8_t* buffer = NULL;
    size_t size = 0;

    if (filelist.size() > 0)
    {
        ParseCdos(bi, filelist, &buffer, &size, false);
    }
    SetPartitionType(PartitionType::CONFIG_DATA_OBJ);
    PartitionHeader* hdr = new Versal_2vpPartitionHeader(this, 0);
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

    hdr->partition = std::make_unique<Versal_2vpPartition>(hdr, buffer, size);
    hdr->partitionSize = hdr->transferSize = size;
    delete[] buffer;
    SetLoadAndExecAddress(hdr);
    partitionHeaderList.push_back(hdr);
}

/******************************************************************************/
uint8_t* Versal_2vpImageHeader::DecodeCdo(std::string file, size_t* size)
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
void Versal_2vpImageHeader::Link(BootImage &bi, PartitionHeader* partitionHeader, ImageHeader* nextImageHeader)
{
    slaveBootSplitMode = (bi.bifOptions->GetSplitMode() == SplitMode::SlaveMode) ? true : false;
    fullBhSize = bi.options.bootheaderSize;
    allHdrSize = bi.options.allHeaderSize;

    imageHeader = (Versal_2vpImageHeaderStructure*)section->Data.get();
    if (partitionHeader->section != NULL)
    {
        SetPartitionHeaderOffset((uint32_t)partitionHeader->section->Address);
    }

    SetImageId();
    SetChecksum();
}

/******************************************************************************/
void Versal_2vpImageHeader::SetMetaHdrRevokeId(uint32_t revokeId)
{
    imageHeader->metaHdrRevokeId = revokeId;
}

/******************************************************************************/
void Versal_2vpImageHeader::SetChecksum(void)
{
    imageHeader->ihChecksum = ComputeWordChecksum(imageHeader, sizeof(Versal_2vpImageHeaderStructure) - sizeof(imageHeader->ihChecksum) /* bytes*/);
}

/******************************************************************************/
void Versal_2vpImageHeader::SetPartitionHeaderOffset(uint32_t addr)
{
    imageHeader->partitionHeaderWordOffset = addr / sizeof(uint32_t);
    if (slaveBootSplitMode && (imageHeader->partitionHeaderWordOffset != 0))
    {
        imageHeader->partitionHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void Versal_2vpImageHeader::SetDataSectionCount(uint32_t cnt)
{
    imageHeader->dataSectionCount = cnt;
}

/******************************************************************************/
void Versal_2vpImageHeader::SetImageHeaderAttributes()
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
void Versal_2vpImageHeader::SetImageName(void)
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
uint32_t Versal_2vpImageHeader::GetPartitionHeaderOffset(void)
{
    return imageHeader->partitionHeaderWordOffset * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t Versal_2vpImageHeader::GetDataSectionCount(void)
{
    return imageHeader->dataSectionCount;
}

/******************************************************************************/
void Versal_2vpImageHeader::ImportElf(BootImage& bi)
{
    uint8_t proc_state = 0;

    domain = Domain::PS;
    SetPartitionType(PartitionType::ELF);

    /* Convert the file into array of bytes */
    ByteFile data(Filename);

    /* Get the ELF Class format - 32-bit elf vs 64-bit elf */
    elfClass = GetElfClass(data.bytes);
    ElfFormat* elf = ElfFormat::GetElfFormat(elfClass, data.bytes, &proc_state, data.len);

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
            bi.imageHeaderTable->SetOptionalData(elf->iht_optional_data, elf->iht_optional_data_size);
            bi.imageHeaderTable->SetXplmModulesData(bi, elf->xplm_modules_data, elf->xplm_modules_data_size);

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

                bool add_ssit_sync_master_cmd = (bi.bifOptions->pdiType == PartitionType::SLR_SLAVE_BOOT);
                ParseCdos(bi, bi.bifOptions->GetPmcCdoFileList(), &cdo_partition, &cdo_length, add_ssit_sync_master_cmd);
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
                if (bi.bifOptions->GetPmcDataBuffer() != NULL)
                {
                    pmcdataSize = bi.bifOptions->pmcdataSize;
                }
                SetPmcDataSizeIh(pmcdataSize);
                SetTotalPmcDataSizeIh(totalpmcdataSize);
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
            PartitionHeader* partHdr = new Versal_2vpPartitionHeader(this, hdr_index);
            partHdr->firstValidIndex = first_index;
            partHdr->elfEndianess = elf->endian;
            partHdr->execAddress = exec_addr;
            partHdr->loadAddress = load_addr;
            partHdr->execState = proc_state;
            if (elf->atf_handoff_params_prg_hdr_count == iprog && elf->atf_handoff_params_found)
            {
                partHdr->atf_handoff_params_offset = elf->atf_handoff_params_offset;
                partHdr->update_atf_handoff_params = true;
            }

            partHdr->partition = std::make_unique<Versal_2vpPartition>(partHdr, partition_data, total_size);

            // This length also includes padding size necessary for 16-byte alignment
            if(Bootloader)
            {
                partHdr->partitionSize = GetPmcFwSizeIh() + GetFsblFwSizeIh(); //partHdr->partition->section->Length;
            }
            else
            {
                partHdr->partitionSize = partHdr->partition->section->Length;
            }
            
            //partHdr->partitionSize = partHdr->partition->section->Length;

            partitionHeaderList.push_back(partHdr);
            hdr_index++;
            free(partition_data);
            partition_data = NULL;
        }
    }
}

/******************************************************************************/
void Versal_2vpImageHeader::ParseCdos(BootImage& bi, std::vector<std::string> filelist, uint8_t** cdo_data, size_t* cdo_size, bool add_ssit_sync_master)
{
    uint8_t* total_cdo_data = NULL;
    uint64_t total_cdo_length = 0;
    void *cdo_data_pp = NULL;
    size_t cdo_data_pp_length = 0;
    char slrid_from_source = 0;
    char slrid_from_binary = 0;
    char input_ch_souce = 0;
    char input_ch_binary = 0;
    bool is_source_cdo = false;
    bool is_binary_cdo = false;

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
            CdoSequence * cdo_seq1;

            if (add_ssit_sync_master)
            {
                /* Add SSIT Sync Master command */
                cdo_seq1 = cdoseq_load_cdo(cdo_filename);
                if (cdo_seq1 == NULL)
                {
                    LOG_ERROR("Error parsing CDO file");
                }
                cdo_seq = cdocmd_create_sequence();
                cdocmd_add_ssit_sync_master(cdo_seq);
                cdocmd_concat_seq(cdo_seq, cdo_seq1);
                add_ssit_sync_master = false;
            }
            else
            {
                cdo_seq = cdoseq_load_cdo(cdo_filename);
                if (cdo_seq == NULL)
                {
                    LOG_ERROR("Error parsing CDO file");
                }
            }

            slrid_from_source = slr_id_from_source(input_ch_souce);
            if (slrid_from_source != 0)
            {
                if (bi.globalSlrId != 0)
                {
                    is_source_cdo = (bi.globalSlrId == slrid_from_source);
                    if (is_source_cdo == false)
                    {
                        LOG_WARNING("Mismatch between SLR ID of %s and rest of the CDOs. This may cause runtime issues. \n           Please ensure that all the CDOs used for creating the pdi are for the same SLR/device", cdo_filename);
                    }
                    bi.globalSlrId = 0;
                }
                bi.globalSlrId = slrid_from_source;
            }
            else
            {
                slrid_from_binary = slr_id_from_binary(input_ch_binary);
                if (slrid_from_binary != 0)
                {
                   if (bi.globalSlrId != 0)
                   {
                       is_binary_cdo = (bi.globalSlrId == slrid_from_binary);
                       if (is_binary_cdo == false)
                       {
                          LOG_WARNING("Mismatch between SLR ID of %s and rest of the CDOs. This may cause runtime issues. \n           Please ensure that all the CDOs used for creating the pdi are for the same SLR/device", cdo_filename);
                       }
                       bi.globalSlrId = 0;
                   }
                   else
                   {
                       bi.globalSlrId = slrid_from_binary;
                   }
                }
            }
            if (bi.overlayCDO && cdooverlay_apply(cdo_seq, (CdoOverlayInfo *)(bi.overlayCDO)))
            {
                LOG_ERROR("Error applying overlay CDO file");
            }
            cdo_data = cdoseq_to_binary(cdo_seq, &cdo_length, 0);
            CheckIdsInCdo(cdo_seq, cdo_filename);
            // TODO: Call this only for v2
            const char * env = getenv("BOOTGEN_CHECK_CDO_COMMANDS");
            if (env && *env != '\0') {
                if (check_cdo_commands(cdo_data, cdo_length, bi.xplm_modules_data.get(), bi.xplm_modules_data_length) != 0) {
                    LOG_WARNING("Invalid PLM cdo command is found in input cdo file");
                }
            }
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

            if (cdo_length > sizeof(VersalCdoHeader))
            {
                actual_cdo_size = cdo_length - sizeof(VersalCdoHeader);
            }
            else
            {
                LOG_ERROR("Incorrect cdo length read from : %s", cdo_filename);
            }

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
void Versal_2vpImageHeader::CreateAieEnginePartition(BootImage& bi)
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
    PartitionHeader* partHdr = new Versal_2vpPartitionHeader(this, 0);
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
    partHdr->partition = std::make_unique<Versal_2vpPartition>(partHdr, pBuffer, size);
    partitionHeaderList.push_back(partHdr);
    delete[] pBuffer;
    cdoSections.clear();
}

/******************************************************************************/
std::list<std::string> Versal_2vpImageHeader::GetAieFilesPath(std::string filename)
{
    std::list <std::string> aie_elf_list;
    std::list <std::string> core_list;
    std::string json_file;
    std::string file_path;
    std::string aie_dir_names[2] = { "me",  "aie" };
    for (uint8_t index = 0; index <= 1; index++)
    {
        file_path = filename + "//" + aie_dir_names[index] + "//";
        json_file = file_path + "active_cores.json";
        std::ifstream aie_json(json_file);

        if (aie_json.good())
        {
            break;
        }
        if (index == 1)
        {
            LOG_ERROR("AIE Work \"%s\" directory doesn't have AIE or ME folder", filename.c_str());
        }
    }

    core_list = ParseAieJson(json_file.c_str());
    for (std::list<std::string>::iterator aie_file = core_list.begin(); aie_file != core_list.end(); aie_file++)
    {
        std::string aie_elf = file_path + (*aie_file) + "/Release/" + (*aie_file);
        aie_elf_list.push_back(aie_elf);
    }
    return aie_elf_list;
}

/******************************************************************************/
uint64_t Versal_2vpImageHeader::ImportAieEngineElfCdo(std::string aie_file)
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
* | South (0 � 0x7fff)         |   row - 1     |    row  - 1   |
* | West (0x8000 � 0xffff)     |   same tile   |    col - 1    |
* | North (0x10000 � 0x17fff)  |   row + 1     |    row + 1    |
* | East (0x18000 � 0x1ffff)   |   col + 1     |    same tile  |
* +----------------------------+---------------+---------------+
* Row 0 is now assigned to shim, so we should be using row - 1 to check if the row
* is odd or even
*/

/******************************************************************************/
void Versal_2vpImageHeader::ImportAieEngineElf(BootImage& bi)
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
    PartitionHeader* partHdr = new Versal_2vpPartitionHeader(this, progHdrCnt);
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
    partHdr->partition = std::make_unique<Versal_2vpPartition>(partHdr, newData, partHdr->partitionSize);
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

                    PartitionHeader* partHdr = new Versal_2vpPartitionHeader(this, progHdrCnt);
                    partHdr->partitionType = PartitionType::ELF;
                    partHdr->partitionSize = elfPrgHeader->p_filesz - spill - previousPartitionSize;
                    partHdr->loadAddress = GetAieEngineGlobalAddress((Binary::Address_t)elf.GetPhysicalAddress(iprog) + previousPartitionSize);
                    partHdr->partition = std::make_unique<Versal_2vpPartition>(partHdr, elfPrgHeader->data + previousPartitionSize, partHdr->partitionSize);
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

                    PartitionHeader* partHdr = new Versal_2vpPartitionHeader(this, progHdrCnt);
                    partHdr->partitionType = PartitionType::ELF;
                    partHdr->partitionSize = elfPrgHeader->p_memsz - spill - previousPartitionSize;
                    partHdr->loadAddress = GetAieEngineGlobalAddress((Binary::Address_t)elf.GetPhysicalAddress(iprog) + previousPartitionSize);
                    previousPartitionSize += partHdr->partitionSize;
                    uint8_t *databuffer = new uint8_t[partHdr->partitionSize];
                    memset(databuffer, 0, partHdr->partitionSize);
                    partHdr->partition = std::make_unique<Versal_2vpPartition>(partHdr, databuffer, partHdr->partitionSize);
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
uint32_t Versal_2vpImageHeader::CdoCmdDmaWrite(uint32_t pSize, uint64_t pAddr, uint8_t *databuffer)
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
uint32_t Versal_2vpImageHeader::CdoCmdWriteImageStore(uint32_t pSize, uint64_t pdi_id, uint8_t *databuffer)
{
    uint32_t total_size;
    CdoCommandWriteImageStore* cdoDataSec = new CdoCommandWriteImageStore;
    uint32_t p_size_pad = pSize + ((4 - (pSize & 3)) & 3);
    cdoDataSec->header = 0x00ff070D;
    cdoDataSec->length = (p_size_pad / 4) + 2;
    cdoDataSec->id = pdi_id;
    cdoDataSec->data = new uint8_t[p_size_pad];
    memset(cdoDataSec->data, 0, p_size_pad);
    memcpy(cdoDataSec->data, databuffer, pSize);
    total_size = (CDO_COMMAND_SIZE + p_size_pad);
    return total_size;
}

/******************************************************************************/
void Versal_2vpImageHeader::CalculateAieEngineBaseAddress(uint32_t colNum, uint32_t rowNum)
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
Binary::Address_t Versal_2vpImageHeader::GetAieEngineGlobalAddress(Binary::Address_t elfAddr)
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
uint32_t Versal_2vpImageHeader::CheckAieEngineDataMemoryBoundary(Binary::Address_t globalAddr, Binary::Length_t pSize)
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
void Versal_2vpImageHeader::SetLoadAndExecAddress(PartitionHeader* partHdr)
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
std::list<std::string> Versal_2vpImageHeader::ParseAieJson(const char * filename)
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
void Versal_2vpImageHeader::SetWriteImageStorePartitions(ImageStorePdiInfo* imageStorePDIs)
{
    imageStorePdiInfo = imageStorePDIs;
}

/******************************************************************************/
void Versal_2vpImageHeader::SetDpacm(DpaCM::Type type)
{
    dpacm = type;
}

/******************************************************************************/
void Versal_2vpImageHeader::SetPufHdLocation(PufHdLoc::Type type)
{
    pufHdLoc = type;
}

/******************************************************************************/
void Versal_2vpSubSysImageHeader::Build(BootImage& bi, Binary& cache)
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
            if (ss_parentUniqueId == 0xFFFFFFFF)
            {
                ss_parentUniqueId = (*image)->GetParentUniqueId();
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

    if (versal_2vpSubSysImageHeaderTable == NULL)
    {
        versal_2vpSubSysImageHeaderTable = (Versal_2vpImageHeaderStructure*)section->Data.get();
        SetImageName();
        SetImageHeaderAttributes();
        SetPartitionHeaderOffset(0);
        SetMetaHdrRevokeId(bi.bifOptions->metaHdrAttributes.partitionRevokeId);
        SetMemCopyAddress();
        SetImageHeaderIds();
    }
}

/******************************************************************************/
void Versal_2vpSubSysImageHeader::Link(BootImage &bi, SubSysImageHeader* nextHeader)
{
    versal_2vpSubSysImageHeaderTable = (Versal_2vpImageHeaderStructure*)section->Data.get();
    if (imgList.front()->GetPartitionHeaderList().front()->section != NULL)
    {
        SetPartitionHeaderOffset((uint32_t)imgList.front()->GetPartitionHeaderList().front()->section->Address);
    }

    SetDataSectionCount();
    SetImageHeaderIds();
    SetPCRMeasurementIndex(bi.options.IsVersalNetSeries());
    SetPCRNumber(bi.options.IsVersalNetSeries());
    SetChecksum();
}


/******************************************************************************/
void Versal_2vpSubSysImageHeader::SetPCRMeasurementIndex(bool versalnet)
{
    if (versalnet)
    {
        versal_2vpSubSysImageHeaderTable->pcrMeasurementIndex = GetPCRMeasurementIndex();
    }
    else
    {
        versal_2vpSubSysImageHeaderTable->pcrMeasurementIndex = 0x00;
    }
}

/******************************************************************************/
void Versal_2vpSubSysImageHeader::SetPCRNumber(bool versalnet)
{
    if (versalnet)
    {
        versal_2vpSubSysImageHeaderTable->pcrNumber = GetPCRNumber();
    }
    else
    {
        versal_2vpSubSysImageHeaderTable->pcrNumber = 0x00;
    }
}

/******************************************************************************/
void Versal_2vpSubSysImageHeader::SetChecksum(void)
{
    versal_2vpSubSysImageHeaderTable->ihChecksum = ComputeWordChecksum(versal_2vpSubSysImageHeaderTable, sizeof(Versal_2vpImageHeaderStructure) - sizeof(versal_2vpSubSysImageHeaderTable->ihChecksum) /* bytes*/);
}

/******************************************************************************/
void Versal_2vpSubSysImageHeader::SetPartitionHeaderOffset(uint32_t addr)
{
    versal_2vpSubSysImageHeaderTable->partitionHeaderWordOffset = addr / sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2vpSubSysImageHeader::SetDataSectionCount(void)
{
    versal_2vpSubSysImageHeaderTable->dataSectionCount = 0;
    for (std::list<ImageHeader*>::iterator imageItr = imgList.begin(); imageItr != imgList.end(); imageItr++)
    {
        versal_2vpSubSysImageHeaderTable->dataSectionCount += (*imageItr)->NumPartitions();
    }
}

/******************************************************************************/
void Versal_2vpSubSysImageHeader::SetMetaHdrRevokeId(uint32_t id)
{
    versal_2vpSubSysImageHeaderTable->metaHdrRevokeId = id;
}

/******************************************************************************/
void Versal_2vpSubSysImageHeader::SetImageHeaderIds(void)
{
    versal_2vpSubSysImageHeaderTable->imageId = imageId;
    versal_2vpSubSysImageHeaderTable->uniqueId = (uniqueId != 0xFFFFFFFF) ? uniqueId : 0;
    versal_2vpSubSysImageHeaderTable->parentUniqueId = (ss_parentUniqueId != 0xFFFFFFFF) ? ss_parentUniqueId : 0;
    versal_2vpSubSysImageHeaderTable->functionId = (functionId != 0xFFFFFFFF) ? functionId : 0;
}

/******************************************************************************/
void Versal_2vpSubSysImageHeader::SetImageHeaderAttributes(void)
{
    if (delayLoad)
    {
        versal_2vpSubSysImageHeaderTable->imageAttributes |= (1 << vihDelayLoadShift);
    }
    if (delayHandoff)
    {
        versal_2vpSubSysImageHeaderTable->imageAttributes |= (1 << vihDelayHandoffShift);
    }
    if (memCopyAddr != 0xFFFFFFFFFFFFFFFF)
    {
        versal_2vpSubSysImageHeaderTable->imageAttributes |= (1 << vihCopyToMemoryShift);
    }
    if (isFullPowerDomain)
    {
        versal_2vpSubSysImageHeaderTable->imageAttributes |= (1 << vihLowPowerDomainShift);
    }
    if (isLowPowerDomain)
    {
        versal_2vpSubSysImageHeaderTable->imageAttributes |= (1 << vihFullPowerDomainShift);
    }
    if (isSystemPowerDomain)
    {
        versal_2vpSubSysImageHeaderTable->imageAttributes |= (1 << vihSystemPowerDomainShift);
    }
    if (isPLPowerDomain)
    {
        versal_2vpSubSysImageHeaderTable->imageAttributes |= (1 << vihPLPowerDomainShift);
    }
}

/******************************************************************************/
void Versal_2vpSubSysImageHeader::SetImageName(void)
{
    for (uint32_t i = 0; i<imageName.size(); i++)
    {
        versal_2vpSubSysImageHeaderTable->imageName[i] = imageName[i];
    }
}

/******************************************************************************/
void Versal_2vpSubSysImageHeader::SetMemCopyAddress(void)
{
    if (memCopyAddr == 0xFFFFFFFFFFFFFFFF)
    {
        memCopyAddr = 0;
    }
    versal_2vpSubSysImageHeaderTable->memcpyAddressLo = memCopyAddr;
    versal_2vpSubSysImageHeaderTable->memcpyAddressHi = memCopyAddr >> 32;
}

/******************************************************************************/
uint64_t Versal_2vpSubSysImageHeader::GetMemCopyAddress(void)
{
    uint64_t mem_cpy_addr = 0;
    mem_cpy_addr = versal_2vpSubSysImageHeaderTable->memcpyAddressHi;
    mem_cpy_addr = (mem_cpy_addr << 32) | (versal_2vpSubSysImageHeaderTable->memcpyAddressLo);
    return mem_cpy_addr;
}

/******************************************************************************/
uint32_t Versal_2vpSubSysImageHeader::GetPartitionHeaderOffset(void)
{
    return versal_2vpSubSysImageHeaderTable->partitionHeaderWordOffset * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t Versal_2vpSubSysImageHeader::GetDataSectionCount(void)
{
    return versal_2vpSubSysImageHeaderTable->dataSectionCount;
}

/******************************************************************************/
uint32_t Versal_2vpSubSysImageHeader::GetImageHeaderAttributes(void)
{
    return versal_2vpSubSysImageHeaderTable->imageAttributes;
}

/******************************************************************************/
Versal_2vpSubSysImageHeader::Versal_2vpSubSysImageHeader(ImageBifOptions *imgOptions)
{
    versal_2vpSubSysImageHeaderTable = NULL;

    imageName = imgOptions->GetImageName();
    imageId = imgOptions->GetImageId();
    delayLoad = imgOptions->GetDelayLoad();
    delayHandoff = imgOptions->GetDelayHandoff();
    memCopyAddr = imgOptions->GetMemCopyAddress();
    imageType = imgOptions->GetImageType();
    uniqueId = imgOptions->GetUniqueId();
    ss_parentUniqueId = imgOptions->GetParentUniqueId();
    functionId = imgOptions->GetFunctionId();
    pcrNumber = imgOptions->GetPcrNumber();
    pcrMeasurementIndex = imgOptions->GetPcrMeasurementIndex();

    isFullPowerDomain = false;
    isLowPowerDomain = false;
    isSystemPowerDomain = false;
    isPLPowerDomain = false;

    std::string name = "ImageHeader " + imageName;
    uint32_t size = sizeof(Versal_2vpImageHeaderStructure);
    section = new Section(name, size);
    memset(section->Data.get(), 0, size);
    imgList.clear();
    num_of_images = 0;
}

/******************************************************************************/
Versal_2vpSubSysImageHeader::Versal_2vpSubSysImageHeader(std::ifstream& ifs)
{
    Versal_2vpImageHeaderStructure importedSubSysHeaderTable;

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
    uint32_t size = sizeof(Versal_2vpImageHeaderStructure);

    ifs.seekg(pos);
    section = new Section("ImageHeader " + imageName, size);
    versal_2vpSubSysImageHeaderTable = (Versal_2vpImageHeaderStructure*)section->Data.get();
    ifs.read((char*)versal_2vpSubSysImageHeaderTable, size);

    imageId = versal_2vpSubSysImageHeaderTable->imageId;
    uniqueId = versal_2vpSubSysImageHeaderTable->uniqueId;
    ss_parentUniqueId = versal_2vpSubSysImageHeaderTable->parentUniqueId;
    functionId = versal_2vpSubSysImageHeaderTable->functionId;
    delayLoad = ((versal_2vpSubSysImageHeaderTable->imageAttributes >> vihDelayLoadShift) & vihDelayLoadMask);
    delayHandoff = ((versal_2vpSubSysImageHeaderTable->imageAttributes >> vihDelayHandoffShift) & vihDelayHandoffMask);
    memCopyAddr = (uint64_t) (versal_2vpSubSysImageHeaderTable->memcpyAddressLo) >> 32;
    memCopyAddr |= versal_2vpSubSysImageHeaderTable->memcpyAddressHi;
    isPLPowerDomain = ((versal_2vpSubSysImageHeaderTable->imageAttributes >> vihPLPowerDomainShift) & vihPLPowerDomainMask);
    isFullPowerDomain = ((versal_2vpSubSysImageHeaderTable->imageAttributes >> vihFullPowerDomainShift) & vihFullPowerDomainMask);
    isLowPowerDomain = ((versal_2vpSubSysImageHeaderTable->imageAttributes >> vihLowPowerDomainShift) & vihLowPowerDomainMask);
    isSystemPowerDomain = ((versal_2vpSubSysImageHeaderTable->imageAttributes >> vihSystemPowerDomainShift) & vihSystemPowerDomainMask);
    pcrNumber = versal_2vpSubSysImageHeaderTable->pcrNumber;
    pcrMeasurementIndex = versal_2vpSubSysImageHeaderTable->pcrMeasurementIndex;

    /* Find the no. of image headers to be created */
    num_of_images = 0;
    uint32_t num_of_sections = 0;

    uint32_t p_count = 0;
    if (versal_2vpSubSysImageHeaderTable->dataSectionCount < 32)
    {
        p_count = versal_2vpSubSysImageHeaderTable->dataSectionCount;
    }

    uint32_t p_offset = versal_2vpSubSysImageHeaderTable->partitionHeaderWordOffset * sizeof(uint32_t);
    p_offset += 0x28;
    for (uint32_t i = 0; i < p_count; i++)
    {
        ifs.seekg(p_offset);
        ifs.read((char*)&num_of_sections, 4);
        if (num_of_sections != 0)
        {
            num_of_images++;
        }
        p_offset += sizeof(Versal_2vpPartitionHeaderTableStructure);
    }
}

/******************************************************************************/
void Versal_2vpImageHeader::CheckIdsInCdo(CdoSequence * seq, std::string cdo_filename)
{
    LINK * l = seq->cmds.next;
    while (l != &seq->cmds) 
    {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        if(cmd->type == CdoCmdMarker)
        {
            char* marker_arch = (char*)cmd->buf;
            if (cmd->value == MARKER_ARCHITECTURE)
            {
               if (strstr(marker_arch,"versalaiedge2") == 0 && strstr(marker_arch,"versalpremium2") == 0)
               {
                  LOG_WARNING("The architecture(%s) in '%s' does not match '-arch versal_2vp'", marker_arch, cdo_filename.c_str());
               }
            }
        }

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
void Versal_2vpImageHeader::SetPowerDomains(uint8_t* buf, uint32_t count)
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

/******************************************************************************/
void Versal_2vpImageHeader::CreateWriteImageStorePartition()
{
    LOG_INFO("Creating Write Image Store CDO partition");
    std::ifstream s(Filename.c_str());
    if (!s)
    {
        LOG_ERROR("Cannot read file - %s ", Filename.c_str());
    }

    ByteFile data(Filename);
    uint32_t p_size_pad = data.len + ((4 - (data.len & 3)) & 3);
    uint8_t* tempBuffer = new uint8_t[p_size_pad];
    memcpy(tempBuffer, data.bytes, p_size_pad);

    size_t size = 0;
    size_t p_offset = 0;
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

    CdoCommandWriteImageStore* cdoCmd = new CdoCommandWriteImageStore;
    cdoCmd->header = 0x00ff070D;
    cdoCmd->length = (p_size_pad / 4) + 2;
    cdoCmd->id = imageStorePdiInfo->id;
    cdoCmd->data = new uint8_t[p_size_pad];
    memset(cdoCmd->data, 0, p_size_pad);
    memcpy(cdoCmd->data, tempBuffer, p_size_pad);

    size += CDO_CMD_WRITE_IMAGE_STORE_SIZE;
    p_buffer = (uint8_t*)realloc(p_buffer, size);
    memcpy(p_buffer + p_offset, cdoCmd, CDO_CMD_WRITE_IMAGE_STORE_SIZE);
    p_offset += CDO_CMD_WRITE_IMAGE_STORE_SIZE;

    size += p_size_pad;
    p_buffer = (uint8_t*)realloc(p_buffer, size);
    memcpy(p_buffer + p_offset, cdoCmd->data, p_size_pad);
    p_offset += p_size_pad;

    size += sizeof(CdoCommandHeader);
    p_buffer = (uint8_t*)realloc(p_buffer, size);
    CdoCommandHeader* cmd_end = CdoCmdCdoEnd();
    memcpy(p_buffer + p_offset, cmd_end, sizeof(CdoCommandHeader));

    /* Update CDO header lengths and checksum */
    cdoHeader->length = (size - sizeof(VersalCdoHeader)) / 4;
    cdoHeader->checksum = ~(cdoHeader->remaining_words + cdoHeader->id_word + cdoHeader->version + cdoHeader->length);
    memcpy(p_buffer, cdoHeader, sizeof(VersalCdoHeader));

    SetPartitionType(PartitionType::CONFIG_DATA_OBJ);
    PartitionHeader* partHdr = new Versal_2vpPartitionHeader(this, imageStorePdiInfo->id);
    partHdr->execState = 0;
    partHdr->elfEndianess = 0;
    partHdr->firstValidIndex = true;
    partHdr->loadAddress = 0xFFFFFFFFFFFFFFFF;
    partHdr->execAddress = 0;
    partHdr->partitionSize = size;
    partHdr->partition = std::make_unique<Versal_2vpPartition>(partHdr, p_buffer, size);
    partitionHeaderList.push_back(partHdr);
}
