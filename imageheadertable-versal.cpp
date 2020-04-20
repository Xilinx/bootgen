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
};

static uint8_t bufferIndex = 0;
std::list<CdoCommandDmaWrite*> VersalImageHeader::cdoSections;
uint64_t slr_sbi_base_array[4] = { PMC_SBI_BUF_ADDR, SLR1_SBI_BUF_BASE_ADDR, SLR2_SBI_BUF_BASE_ADDR, SLR3_SBI_BUF_BASE_ADDR };
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
    section = new Section("ImageHeaderTable", sizeof(VersalImageHeaderTableStructure));
    iHTable = (VersalImageHeaderTableStructure*)section->Data;
}

/******************************************************************************/
VersalImageHeaderTable::VersalImageHeaderTable(std::ifstream& src)
{
    prebuilt = true;
    section = new Section("ImageHeaderTable", sizeof(VersalImageHeaderTableStructure));
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

        SetImageHeaderTableVersion(0x00020000);
        SetHeaderTablesSize();
        SetTotalMetaHdrLength(0);
        SetIdentificationString(bi.IsBootloaderFound());
        SetIds();
        metaHdrKeySrc = bi.options.bifOptions->metaHdrAttributes.encrKeySource;
        SetMetaHdrSecureHdrIv(metaHdrSecHdrIv);
        dpacm = bi.bifOptions->metaHdrAttributes.dpaCM;
        pufHDLoc = bi.bifOptions->metaHdrAttributes.pufHdLoc;
    }

    for (std::list<ImageHeader*>::iterator image = bi.imageList.begin(); image != bi.imageList.end(); image++)
    {
        (*image)->Build(bi, cache);
        ValidateSecurityCombinations((*image)->GetAuthenticationType(), (*image)->GetEncryptContext()->Type(),
            (*image)->GetChecksumContext()->Type());

        imageHeaderList.push_back(*image);
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

    SetPartitionCount((uint32_t)bi.partitionHeaderList.size());

    uint32_t acOffset = (bi.headerAC != 0) ? (bi.headerAC->section->WordAddress()) : 0;
    SetHeaderAuthCertificateOffset(acOffset);

    // Go through image list, to set the next image's offset parameters
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
    iHTable->metaHdrKeySource = 0;
}

/******************************************************************************/
void VersalImageHeaderTable::SetMetaHdrGreyOrBlackIv(std::string ivFile)
{
    uint8_t* ivData = new uint8_t[IV_LENGTH * 4];
    memset(ivData, 0, IV_LENGTH * 4);

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
    , master_slr_available(false)
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
    , master_slr_available(false)
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
    , master_slr_available(false)
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

        PartitionHeader* hdr = new VersalPartitionHeader(this, index);
        if (!firstValidHdr)
        {
            hdr->firstValidIndex = true;
            firstValidHdr = true;
        }
        ifs.seekg(offset);
        hdr->ReadHeader(ifs);
        hdr->ReadData(ifs);

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
    , master_slr_available(false)
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

        PartitionHeader* hdr = new VersalPartitionHeader(this, index);
        if (!firstValidHdr)
        {
            hdr->firstValidIndex = true;
            firstValidHdr = true;
        }
        ifs.seekg(offset);
        hdr->ReadHeader(ifs);
        hdr->ReadData(ifs);

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
            LOG_INFO("Parsing file - %s", StringUtils::BaseName(Filename).c_str());
            std::ifstream stream(Filename.c_str(), std::ios_base::binary);
            if (!stream)
            {
                LOG_ERROR("Cannot read file - %s ", Filename.c_str());
            }
            std::string line;
            getline(stream, line);
            if (IsElf(line) && (partitionType != PartitionType::RAW))
            {
                ImportElf(bi);
            }
            else if ((line.find("Xilinx ASCII Bitstream") != std::string::npos) || (line.find("Xilinx ASCII CFI Deviceimage") != std::string::npos))
            {
                ImportBit(bi);
            }
            else if ((line.find("Xilinx ASCII NPI Deviceimage") != std::string::npos) || (line.find("Xilinx ASCII PSAXIMM Deviceimage") != std::string::npos))
            {
                ImportNpi(bi);
            }
            else if (line.find("version") != std::string::npos)
            {
                ImportCdoSource(bi);
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
    memset(imageHeader->reserved, 0x00, MAX_IH_RESERVED_VERSAL * sizeof(uint32_t));
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
    uint8_t non_zero_count = 0;

    for (uint8_t iprog = 0; iprog < elf->programHdrEntryCount; iprog++)
    {
        if (elf->GetProgramHeaderFlags(iprog) & PF_X)
        {
            exec_count++;
        }
        if ((elf->GetProgramHeaderFileSize(iprog) > 0) && (elf->GetProgramHeaderType(iprog) == xPT_LOAD))
        {
            non_zero_count++;
        }
    }

    if (non_zero_count == 0)
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
            if (pmc_fw_size % 16 != 0)
            {
                pmc_fw_pad_bytes = 16 - (pmc_fw_size % 16);
            }
            SetFsblFwSizeIh(pmc_fw_size + pmc_fw_pad_bytes);
            SetTotalFsblFwSizeIh(pmc_fw_size + pmc_fw_pad_bytes);

            /* Attach pmcdata, if available, to PLM to create a single partition */
            std::string pmc_cdo_file = bi.bifOptions->GetPmcdataFile();
            if ((pmc_cdo_file != "") || (bi.bifOptions->GetPmcDataBuffer() != NULL))
            {
                Binary::Length_t pmc_cdo_length = 0;
                uint8_t* pmc_cdo_data = NULL;

                /* If PMC CDO is passed as a file */
                if (pmc_cdo_file != "")
                {
                    std::ifstream stream(pmc_cdo_file.c_str(), std::ios_base::binary);
                    if (!stream)
                    {
                        LOG_ERROR("Cannot read PMC Data file - %s ", pmc_cdo_file.c_str());
                    }
                    std::string line;
                    getline(stream, line);
                    if ((line.find("Xilinx ASCII NPI Deviceimage") != std::string::npos) || (line.find("Xilinx ASCII PSAXIMM Deviceimage") != std::string::npos))
                    {
                        /* If PMC CDO is in ASCII format */
                        stream.seekg(0, std::ios::beg);
                        BitFile *bit = new VersalBitFile(stream);
                        bit->ParseBit(bi);
                        OutputStream *os = bit->GetOutputStreamType();
                        bit->CopyNpi(os);
                        pmc_cdo_length = os->Size();
                        pmc_cdo_data = new uint8_t[pmc_cdo_length];
                        memcpy(pmc_cdo_data, os->Start(), pmc_cdo_length);
                    }
                    else if (line.find("version") != std::string::npos)
                    {
                        size_t len = 0;
                        pmc_cdo_data = DecodeCdo(pmc_cdo_file, &len);
                        pmc_cdo_length = len;
                    }
                    else
                    {
                        /* If PMC CDO is in BIN format */
                        ByteFile pmc_cdo_partition(pmc_cdo_file);
                        pmc_cdo_length = pmc_cdo_partition.len;
                        /* Read ID value (XNLX/CDO) from CDO Header to identify endianness */
                        uint32_t dataValue = ReadBigEndian32(pmc_cdo_partition.bytes + 4);
                        /*if ((dataValue != 0x584c4e58) && (dataValue != 0x584e4c58) && (dataValue != 0x004f4443) && (dataValue != 0x43444f00))
                        {
                        LOG_ERROR("Invalid CDO format - incorrect identification word (XLNX/CDO) - 0x%x", dataValue);
                        }*/
                        bool change_endianness = false;
                        /* Convert BE CDOs to LE */
                        if (dataValue == 0x584c4e58 || dataValue == 0x004f4443)
                        {
                            change_endianness = true;
                        }
                        pmc_cdo_data = new uint8_t[pmc_cdo_length];
                        for (uint32_t index = 0; index < pmc_cdo_partition.len; index += 4)
                        {
                            dataValue = ReadBigEndian32(pmc_cdo_partition.bytes + index);
                            if (change_endianness)
                            {
                                WriteLittleEndian32(pmc_cdo_data + index, dataValue);
                            }
                            else
                            {
                                WriteBigEndian32(pmc_cdo_data + index, dataValue);
                            }
                        }
                    }
                }
                else
                {
                    /* If PMC CDO is passed as a buffer */
                    pmc_cdo_length = bi.bifOptions->GetTotalpmcdataSize();
                    pmc_cdo_data = new uint8_t[pmc_cdo_length];
                    memcpy(pmc_cdo_data, bi.bifOptions->GetPmcDataBuffer(), pmc_cdo_length);
                }
                
                void *pmc_data_pp = NULL;
                size_t pmc_data_pp_length = 0;
                if (cdocmd_post_process_cdo(pmc_cdo_data, pmc_cdo_length, &pmc_data_pp, &pmc_data_pp_length))
                {
                    LOG_ERROR("PMC CDO post process error");
                }
                if (pmc_data_pp != NULL)
                {
                    delete[] pmc_cdo_data;
                    pmc_cdo_data = new uint8_t[pmc_data_pp_length];
                    memcpy(pmc_cdo_data, pmc_data_pp, pmc_data_pp_length);
                    pmc_cdo_length = pmc_data_pp_length;
                }

                /* Calculate pad bytes for aligning PMC CDO for 16 byte-alignment */
                uint32_t pmc_cdo_pad_bytes = 0;
                if (pmc_cdo_length % 16 != 0)
                {
                    pmc_cdo_pad_bytes = 16 - (pmc_cdo_length % 16);
                }
                pmcdataSize = totalpmcdataSize = pmc_cdo_length + pmc_cdo_pad_bytes;

                /* Append PMC CDO to PMC FW to create a single partition */
                total_size = pmc_fw_size + pmc_fw_pad_bytes + pmc_cdo_length + pmc_cdo_pad_bytes;
                partition_data = (uint8_t*)realloc(partition_data, total_size);
                memset(partition_data + pmc_fw_size, 0, pmc_fw_pad_bytes);
                memcpy(partition_data + pmc_fw_size + pmc_fw_pad_bytes, pmc_cdo_data, pmc_cdo_length);
                memset(partition_data + pmc_fw_size + pmc_fw_pad_bytes + pmc_cdo_length, 0, pmc_cdo_pad_bytes);

                delete[] pmc_cdo_data;
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
CdoCommandHeader* CdoCmdEnd(void)
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
    cdoCmd->keyhole_size = SBI_KEYHOLE_SIZE;
    LOG_TRACE("CDO_CMD_WRITE_KEYHOLE: Address-0x%llx, Length-0x%lx (words)", slr_sbi_base_array[index], cdoCmd->length);
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
SlrPdiType GetSlrType(SlrPdiInfo* slr)
{
    SlrPdiType slr_type = SlrPdiType::BOOT;
    std::ifstream stream(slr->file.c_str(), std::ios_base::binary);
    if (!stream)
    {
        LOG_ERROR("Cannot read file - %s ", slr->file.c_str());
    }
    std::string line;
    getline(stream, line);
    if ((line.find("Xilinx ASCII NPI Deviceimage") != std::string::npos) || (line.find("Xilinx ASCII PSAXIMM Deviceimage") != std::string::npos))
    {
        slr_type = SlrPdiType::NOC_FREQ;
    }
    else
    {
        uint32_t dataValue;
        std::ifstream fl(slr->file.c_str(), std::ios::binary);
        fl.read((char*)&dataValue, 4);
        fl.close();
        if ((dataValue == 0x584c4e58) || (dataValue == 0x584e4c58) || (dataValue == 0x004f4443) || (dataValue == 0x43444f00))
        {
            slr_type = SlrPdiType::NOC_FREQ;
        }
    }
    stream.close();
    return slr_type;
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
    SetPartitionType(PartitionType::CONFIG_DATA_OBJ);
    PartitionHeader* partHdr = new VersalPartitionHeader(this, 0);
    partHdr->execState = 0;
    partHdr->elfEndianess = 0;
    partHdr->firstValidIndex = true;

    std::ifstream stream(slrBootPdiInfo.front()->file.c_str(), std::ios_base::binary);
    if (!stream)
    {
        LOG_ERROR("Cannot read file - %s ", slrBootPdiInfo.front()->file.c_str());
    }
    std::string line;
    getline(stream, line);
    uint8_t* cdo_buffer;
    size_t size = 0;

    if ((line.find("Xilinx ASCII NPI Deviceimage") != std::string::npos) || (line.find("Xilinx ASCII PSAXIMM Deviceimage") != std::string::npos))
    {
        /* CDO in ASCII/raw format */
        stream.seekg(0, std::ios::beg);
        BitFile *bit = new VersalBitFile(stream);
        bit->ParseBit(bi);
        OutputStream *os = bit->GetOutputStreamType();
        bit->CopyNpi(os);
        size = os->Size();
        cdo_buffer = new uint8_t[size];
        memcpy(cdo_buffer, os->Start(), size);
        delete os;
    }
    else if (line.find("version") != std::string::npos)
    {
        /* CDO in source format */
        cdo_buffer = DecodeCdo(Filename, &size);
    }
    else
    {
        /* CDO in BIN format */
        ByteFile slr_boot_data(slrBootPdiInfo.front()->file);
        size = slr_boot_data.len;
        cdo_buffer = new uint8_t[size];
        memcpy(cdo_buffer, slr_boot_data.bytes, size);
    }
    partHdr->partitionSize = size;
    partHdr->partition = new Partition(partHdr, cdo_buffer, size);
    partitionHeaderList.push_back(partHdr);
    delete cdo_buffer;
    delete cdoHeader;
    cdoSections.clear();
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
    uint64_t chunk_size = 0x1000;
    uint64_t size = 0;
    uint32_t p_offset = 0;
    uint32_t slr_sync_points[4] = { 0, 0, 0, 0 };
    uint32_t common_sync_point = 0, current_sync_point = 0, last_sync_point = 0;
    uint8_t file_index = 0;
    uint32_t num_chunks[4] = { 0, 0, 0, 0 };
    uint32_t eof_slr[4] = { 0, 0, 0, 0 };
    uint64_t total_slr_chunk_size = 0;
    uint32_t chunk_num = 1;
    uint32_t master_index = 0;
    uint32_t prev_master_slr_offset = 0;
    uint32_t master_file_size = 0;

    for (uint8_t i = 0; i < 4; i++)
    {
        slr_file_size[i] = 0;
        slr_offsets[i] = 0;
    }
    slr_total_file_size = 0;

    LOG_INFO("Creating SLR Config CDO partition");
    LOG_INFO("SSIT chunk size = 0x%x", chunk_size);

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

    /* Parse slave SLR config files and identify sync points, file sizes */
    slrConfigPdiInfo.sort(SortByIndex);
    IdentifySyncPoints(bi);

    while (total_slr_chunk_size < slr_total_file_size)
    {
        file_index = 0;
        for (std::list<SlrPdiInfo*>::iterator slr_id = slrConfigPdiInfo.begin(); slr_id != slrConfigPdiInfo.end(); slr_id++)
        {
            file_index = (*slr_id)->index - 1;
            /* Process slave SLR data only if it has not yet reached the common sync point */
            if ((slr_sync_points[file_index] == common_sync_point) && ((*slr_id)->index != SlrId::MASTER))
            {
                size_t bytes_to_read = chunk_size;
                /* If remaining bytes are less than chunk size */
                if (slr_file_size[file_index] - slr_offsets[file_index] < chunk_size)
                {
                    bytes_to_read = slr_file_size[file_index] - slr_offsets[file_index];
                }
                
                if(bytes_to_read != 0)
                {
                    num_chunks[file_index]++;
                    size += (bytes_to_read + CDO_CMD_WRITE_KEYHOLE_SIZE);
                    p_buffer = (uint8_t*)realloc(p_buffer, size);
                    LOG_TRACE("SSIT: %d. slr_%d, chunk_offset=0x%x", chunk_num++, (((*slr_id)->index == 4) ? 0 : (*slr_id)->index), p_offset);

                    /* Add write keyhole command for slave SLRs and master config */
                    CdoCommandWriteKeyhole* cdoCmd = CdoCmdWriteKeyHole(bytes_to_read, (*slr_id)->index);
                    memcpy(p_buffer + p_offset, cdoCmd, CDO_CMD_WRITE_KEYHOLE_SIZE);
                    delete cdoCmd;
                    p_offset += CDO_CMD_WRITE_KEYHOLE_SIZE;
                    memcpy(p_buffer + p_offset, slr_data[file_index] + slr_offsets[file_index], bytes_to_read);
                    
                    /* Check for sync points in the chunk */
                    CheckSyncPointInChunk(bytes_to_read, file_index, slr_sync_points);
                    p_offset += bytes_to_read;
                    total_slr_chunk_size += bytes_to_read;
                    slr_offsets[file_index] += bytes_to_read;
                }
                else
                {
                    /* If EOF reached, don't consider this SLR for finding common sync points */
                    eof_slr[file_index] = 1;
                }
            }
            /* Get the sync point which all the slave SLRs have serviced */
            current_sync_point = FindCommonSyncPoint(slr_sync_points, eof_slr, num_of_slrs);
            
            if ((last_sync_point != current_sync_point))
            {
                if (master_slr_available)
                {
                    /* Do not process any other SLR data, until the master data is processed */
                    if (((*slr_id)->index == SlrId::MASTER) && (master_file_size < slr_file_size[file_index]))
                    {
                        size_t bytes_to_read = 0;
                        if (master_index < slr_sync_addresses[file_index].size())
                        {
                            bytes_to_read = slr_sync_addresses[file_index][master_index] - prev_master_slr_offset;
                            prev_master_slr_offset = slr_sync_addresses[file_index][master_index];
                            master_index++;
                        }
                        else
                        {
                            bytes_to_read = slr_file_size[file_index] - prev_master_slr_offset;
                        }
                        if (bytes_to_read != 0)
                        {
                            num_chunks[file_index]++;
                            size += bytes_to_read;
                            p_buffer = (uint8_t*)realloc(p_buffer, size);
                            memcpy(p_buffer + p_offset, slr_data[file_index] + slr_offsets[file_index], bytes_to_read);
                            LOG_TRACE("SSIT: %d. slr_master, chunk_offset=0x%x, length=0x%x", chunk_num++, p_offset, bytes_to_read);
                            p_offset += bytes_to_read;
                            slr_offsets[file_index] += bytes_to_read;
                            master_file_size += bytes_to_read;
                            total_slr_chunk_size += bytes_to_read;
                        }
                        last_sync_point++;
                    }
                }
                else
                {
                     last_sync_point++;
                }
            }
        }
        common_sync_point = current_sync_point;
    }

    size += sizeof(CdoCommandHeader);
    p_buffer = (uint8_t*)realloc(p_buffer, size);
    CdoCommandHeader* cmd_end = CdoCmdEnd();
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
    partHdr->partition = new Partition(partHdr, p_buffer, size);
    partitionHeaderList.push_back(partHdr);

    delete[] p_buffer;
    delete cmd_end;

    file_index = 0;
    for (std::list<SlrPdiInfo*>::iterator slr_id = slrConfigPdiInfo.begin(); slr_id != slrConfigPdiInfo.end(); slr_id++)
    {
        file_index = (*slr_id)->index - 1;
        if ((*slr_id)->index != SlrId::MASTER)
        {
            LOG_TRACE("SSIT: total slr_%d chunks = %d", (*slr_id)->index, num_chunks[file_index]);
        }
        else
        {
            LOG_TRACE("SSIT: total slr_master chunks = %d", num_chunks[file_index]);
        }
    }
}

/******************************************************************************/
void VersalImageHeader::IdentifySyncPoints(BootImage& bi)
{
    uint8_t file_index = 0;
    LOG_TRACE("CDO_CMD_SSIT_SYNC_MASTER command detected at following offsets:");
    for (std::list<SlrPdiInfo*>::iterator slr_id = slrConfigPdiInfo.begin(); slr_id != slrConfigPdiInfo.end(); slr_id++)
    {
        file_index = (*slr_id)->index - 1;
        std::ifstream stream((*slr_id)->file.c_str(), std::ios_base::binary);
        if (!stream)
        {
            LOG_ERROR("Cannot read file - %s ", (*slr_id)->file.c_str());
        }
        std::string line;
        getline(stream, line);
        if ((line.find("Xilinx ASCII NPI Deviceimage") != std::string::npos) || (line.find("Xilinx ASCII PSAXIMM Deviceimage") != std::string::npos))
        {
            /* CDO in ASCII/raw format */
            stream.seekg(0, std::ios::beg);
            BitFile *bit = new VersalBitFile(stream);
            bit->ParseBit(bi);
            OutputStream *os = bit->GetOutputStreamType();
            bit->CopyNpi(os);
            uint32_t data_len = os->Size() - sizeof(VersalCdoHeader);
            slr_data[file_index] = (uint8_t*)malloc(data_len);
            memcpy(slr_data[file_index], os->Start() + sizeof(VersalCdoHeader), data_len);
            slr_file_size[file_index] = data_len;
            delete os;
        }
        else if (line.find("version") != std::string::npos)
        {
            /* CDO in source format */
            uint8_t* cdo_buffer;
            size_t size = 0;
            cdo_buffer = DecodeCdo(Filename, &size);
            uint32_t data_len = size - sizeof(VersalCdoHeader);
            slr_data[file_index] = (uint8_t*)malloc(data_len);
            memcpy(slr_data[file_index], cdo_buffer + sizeof(VersalCdoHeader), data_len);
            slr_file_size[file_index] = data_len;
            delete cdo_buffer;
        }
        else
        {
            /* PDI/CDO in BIN format */
            ByteFile slr_boot_data((*slr_id)->file);
            /* If CDO file, strip off the CDO header as this will go into a CDO partition */
            uint32_t dataValue = ReadBigEndian32(slr_boot_data.bytes + 4);
            if (IsCdoFile(dataValue))
            {
                bool change_endianness = false;
                Binary::Length_t data_len = slr_boot_data.len - sizeof(VersalCdoHeader);
                slr_data[file_index] = (uint8_t*)malloc(data_len);
                if (dataValue == 0x584c4e58 || dataValue == 0x004f4443)
                {
                    /* Convert BE CDOs to LE */
                    change_endianness = true;
                    LOG_WARNING("CDO is in Big Endian format. Big Endian CDOs are deprecated. Please use little endian CDOs.");
                }
                for (uint32_t index = 0; index < data_len; index += 4)
                {
                    dataValue = ReadBigEndian32(slr_boot_data.bytes + sizeof(VersalCdoHeader) + index);
                    if (change_endianness)
                    {
                        WriteLittleEndian32(slr_data[file_index] + index, dataValue);
                    }
                    else
                    {
                        WriteBigEndian32(slr_data[file_index] + index, dataValue);
                    }
                }
                slr_file_size[file_index] = data_len;
            }
            else
            {
                slr_data[file_index] = (uint8_t*)malloc(slr_boot_data.len);
                memcpy(slr_data[file_index], slr_boot_data.bytes, slr_boot_data.len);
                slr_file_size[file_index] = slr_boot_data.len;
            }
        }

        std::vector<uint32_t> sync_pt;
        if ((*slr_id)->index != SlrId::MASTER)
        {
            LOG_TRACE("   slr_%d:", (*slr_id)->index);
            for (uint32_t i = 0; i < slr_file_size[file_index]; i = i + 4)
            {
                uint32_t search_cmd = (slr_data[file_index][i] << 24) | (slr_data[file_index][i + 1] << 16) | (slr_data[file_index][i + 2] << 8) | (slr_data[file_index][i + 3]);
                if (search_cmd == CDO_SSIT_SYNC_MASTER_CMD)
                {
                    sync_pt.push_back(i);
                    LOG_TRACE("       offset = 0x%x", i);
                }
            }
            num_of_slrs++;
            slr_total_file_size += slr_file_size[file_index];
        }
        else
        {
            master_slr_available = true;
            LOG_TRACE("   master_slr:", (*slr_id)->index);
            for (uint32_t i = 0; i < slr_file_size[file_index]; i = i + 4)
            {
                uint32_t search_cmd = (slr_data[file_index][i] << 24) | (slr_data[file_index][i + 1] << 16) | (slr_data[file_index][i + 2] << 8) | (slr_data[file_index][i + 3]);
                if (search_cmd == CDO_SSIT_SYNC_SLAVES_CMD)
                {
                    LOG_TRACE("       offset = 0x%x", i);
                    sync_pt.push_back(i + sizeof(CdoSsitSlaves));
                }
            }
            slr_total_file_size += slr_file_size[file_index];
        }
        slr_sync_addresses[file_index] = sync_pt;
    }
}

/******************************************************************************/
uint32_t VersalImageHeader::FindCommonSyncPoint(uint32_t* slr_sync_points, uint32_t* eof_slr, uint8_t num_slrs)
{
    uint32_t temp = 0xFFFFFFFF;
    for (uint8_t i = 0; i < num_slrs; i++)
    {
        if ((slr_sync_points[i] < temp) && (eof_slr[i] != 1))
        {
            temp = slr_sync_points[i];
        }
    }
    return temp;
}

/******************************************************************************/
void VersalImageHeader::CheckSyncPointInChunk(uint8_t* buffer, uint64_t size, uint8_t slr_num, uint32_t* slr_sync)
{
    for (uint32_t i = 0; i < size; i = i + 4)
    {
        uint32_t search_cmd = (buffer[i] << 24) | (buffer[i + 1] << 16) | (buffer[i + 2] << 8) | (buffer[i + 3]);
        if (search_cmd == CDO_SSIT_SYNC_MASTER_CMD)
        {
            slr_sync[slr_num]++;
        }
    }
}

/******************************************************************************/
void VersalImageHeader::CheckSyncPointInChunk(uint64_t size, uint8_t slr_num, uint32_t* slr_sync)
{
    for (uint32_t j = 0; j < slr_sync_addresses[slr_num].size(); j++)
    {
        if ((slr_sync_addresses[slr_num][j] > slr_offsets[slr_num]) && (slr_sync_addresses[slr_num][j] < (slr_offsets[slr_num] + size)))
        {
            slr_sync[slr_num]++;
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

    if (subSysImageHeaderTable == NULL)
    {
        subSysImageHeaderTable = (VersalImageHeaderStructure*)section->Data;
        SetImageName();
        SetImageHeaderAttributes();
        //SetImageNameLength((uint32_t)Name.length()); // this is overwritten later for legacy purpose
        SetPartitionHeaderOffset(0);
        SetMetaHdrRevokeId(bi.bifOptions->metaHdrAttributes.revokeId);
        SetImageHeaderPuid();
        SetMemCopyAddress();
    }

    for (std::list<ImageHeader*>::iterator image = imgList.begin(); image != imgList.end(); image++)
    {
        (*image)->Build(bi, cache);
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
    SetImageHeaderPuid();
    SetReservedFields();
    SetChecksum();
}


/******************************************************************************/
void SubSysImageHeader::SetReservedFields(void)
{
    memset(subSysImageHeaderTable->reserved, 0x00, MAX_IH_RESERVED_VERSAL * sizeof(uint32_t));
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
void SubSysImageHeader::SetImageHeaderPuid(void)
{
    subSysImageHeaderTable->imageId = imageId;
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

std::string SubSysImageHeader::GetSubSystemName(void)
{
    return imageName;
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
    //memset(subSysImageHeaderTable->imageName, 0, sizeof(subSysImageHeaderTable->imageName));
    //strcpy(subSysImageHeaderTable->imageName, imageName.c_str());

    ifs.read((char*)subSysImageHeaderTable, size);

    imageId = subSysImageHeaderTable->imageId;
    delayLoad = ((subSysImageHeaderTable->imageAttributes >> vihDelayLoadShift) & vihDelayLoadMask);
    delayHandoff = ((subSysImageHeaderTable->imageAttributes >> vihDelayHandoffShift) & vihDelayHandoffMask);
    memCopyAddr = (uint64_t) (subSysImageHeaderTable->memcpyAddressLo) >> 32;
    memCopyAddr |= subSysImageHeaderTable->memcpyAddressHi;

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
