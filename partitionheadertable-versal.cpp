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
#include "partitionheadertable-versal.h"
#include "bootheader-versal.h"
#include "authentication-versal.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
VersalPartitionHeader::VersalPartitionHeader(ImageHeader* imageheader, int index)
    : partitionEncrypted(0)
    , partitionUid(0)
    , PartitionHeader(imageheader, index)
    , partitionKeySrc(KeySource::None)
    , dpaCM(DpaCM::DpaCMDisable)
    , pufHdLoc(PufHdLoc::PUFinEFuse)
    , kekIvFile("")
{
    std::string name;
    slr = 0;
    if (imageHeader)
    {
        name = "PartitionHeader " + imageHeader->GetName() + StringUtils::Format(".%d", index);
        partitionAesKeyFile = imageHeader->GetAesKeyFile();
        generateAesKeyFile = imageHeader->GetAesKeyFileGeneration();
        if (index != 0)
        {
            partitionAesKeyFile = StringUtils::RemoveExtension(imageHeader->GetAesKeyFile()) + StringUtils::Format(".%d", index) + ".nky";

            std::ifstream keyFile(partitionAesKeyFile);
            bool exists = keyFile.good();

            if (!generateAesKeyFile && !exists)
            {
                generateAesKeyFile = true;
            }
        }
        partitionUid = imageHeader->GetPartitionUid();
        partitionKeySrc = imageHeader->GetEncryptionKeySrc();
        kekIvFile = imageHeader->GetKekIV();
    }
    else
    {
        name = "PartitionHeader Null";
    }
    section = new Section(name, sizeof(VersalPartitionHeaderTableStructure));
    memset(section->Data, 0, section->Length);

    pHTable = (VersalPartitionHeaderTableStructure*)section->Data;
    pHTable->partitionRevokeId = imageHeader->GetPartitionRevocationId();
}

/******************************************************************************/
VersalPartitionHeader::~VersalPartitionHeader()
{
    if (section != NULL)
    {
        delete section;
    }
}

/******************************************************************************/
void VersalPartitionHeader::ReadHeader(std::ifstream& ifs)
{
    ifs.read((char*)pHTable, sizeof(VersalPartitionHeaderTableStructure));

    uint32_t checksum = ComputeWordChecksum(pHTable, sizeof(VersalPartitionHeaderTableStructure) - sizeof(uint32_t));

    if (checksum != GetChecksum())
    {
        LOG_DEBUG(DEBUG_STAMP, "Calculated Checksum = 0x%8X, Imported Checksum = 0x%8X", checksum, GetChecksum());
        LOG_ERROR("Partition Header Checksum failed for %s", section->Name.c_str());
    }

    execAddress = GetExecAddress();
    loadAddress = GetLoadAddress();
    partitionSize = GetUnencryptedPartitionLength();
    transferSize = GetEncryptedPartitionLength();

    prealigned = true;
    headAlignment = GetHeadAlignment();
    tailAlignment = GetTailAlignment();
    checksumType = GetChecksumType();
    ownerType = GetOwnerType();
    destCpu = GetDestinationCpu();
    destDevice = GetDestinationDevice();
    execState = GetProcessorExecState();
    elfEndianess = GetElfEndianess();
    exceptionLevel = GetExceptionLevel();
    trustzone = GetTrustZone();
    early_handoff = GetEarlyHandoff();
    hivec = GetHivec();
    partitionUid = GetPartitionUid();

    presigned = (authCertPresent != 0);
    if (presigned)
    {
        certificateRelativeByteOffset = (GetAuthCertificateOffset() - GetPartitionWordOffset());
    }
}

/******************************************************************************/
void VersalPartitionHeader::ReadData(std::ifstream& ifs)
{
    uint32_t dataLen = GetTotalPartitionLength();
    std::string partName = imageHeader->GetName() + StringUtils::Format(".%d", index);
    Section* dsection = new Section(partName, dataLen);
    ifs.seekg(GetPartitionWordOffset());
    ifs.read((char*)dsection->Data, dsection->Length);

    partition = new Partition(this, dsection);

    static uint8_t encryptionHeader[] =
    {
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBB,0x00,0x00,0x00,
        0x44,0x00,0x22,0x11,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x66,0x55,0x99,0xAA
    };
    preencrypted = (memcmp(dsection->Data, encryptionHeader, sizeof(encryptionHeader)) == 0);
}

/******************************************************************************/
void VersalPartitionHeader::Build(BootImage& bi, Binary& cache)
{
    if (section != NULL)
    {
        bi.headers.push_back(section);
        bi.imageHeaderTable->metaHeaderLength += section->Length;
    }
}

/******************************************************************************/
void VersalPartitionHeader::Link(BootImage &bi, PartitionHeader* next_part_hdr)
{
    slaveBootSplitMode = (bi.bifOptions->GetSplitMode() == SplitMode::SlaveMode) ? true : false;
    fullBhSize = bi.options.bootheaderSize;
    allHdrSize = bi.options.allHeaderSize;
    bootloaderSize = bi.options.bootloaderSize;
    isBootloader = partition->section->isBootloader;

    /* Realigning the data ptr is necessary because Section->Data ptr gets changed in the
       RSAAuthenticationContext::ResizeIfNecessary() function call */
    RealignSectionDataPtr();

    /* Exec address and section count are populated only for the first of the many partitions 
       that can be created for any elf partition */
    SetExecAddress((firstValidIndex) ? execAddress : 0);
    SetSectionCount((uint32_t)((firstValidIndex) ? imageHeader->GetPartitionHeaderList().size() : 0));

    SetEncryptedPartitionLength((uint32_t)transferSize);
    SetUnencryptedPartitionLength((uint32_t)(partitionSize + 3));
    SetTotalPartitionLength((uint32_t)partition->section->Length);
    uint32_t addr = 0;
    if (next_part_hdr != NULL)
    {
        addr = next_part_hdr->section->Address;
    }
    SetNextPartitionHeaderOffset(addr);
    SetLoadAddress(loadAddress);
    SetPartitionWordOffset((uint32_t)partition->section->Address);
    SetPartitionAttributes();
    SetChecksumOffset();
    SetAuthCertificateOffset();
    SetPartitionId();
    SetPartitionSecureHdrIv(partitionSecHdrIv);
    SetPartitionKeySrc(partitionKeySrc, bi.bifOptions);
    SetPartitionGreyOrBlackIv(kekIvFile);
    SetReserved();
    SetChecksum();
}

/******************************************************************************/
void VersalPartitionHeader::SetPartitionSecureHdrIv(uint8_t* iv)
{
    if (iv == NULL)
    {
        memset(pHTable->partitionSecureHdrIv, 0, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }
    else
    {
        memcpy(pHTable->partitionSecureHdrIv, iv, IV_LENGTH * WORD_SIZE_IN_BYTES);
    }
}

/******************************************************************************/
void VersalPartitionHeader::SetPartitionKeySrc(KeySource::Type keyType, BifOptions* bifOptions)
{
    pHTable->partitionKeySource = 0;
}

/******************************************************************************/
void VersalPartitionHeader::SetPartitionGreyOrBlackIv(std::string ivFile)
{
    uint8_t* ivData = new uint8_t[IV_LENGTH * 4];
    memset(ivData, 0, IV_LENGTH * 4);

    memcpy(&pHTable->partitionGreyOrBlackIV, ivData, IV_LENGTH * 4);
    delete[] ivData;
}


/******************************************************************************/
void VersalPartitionHeader::SetEncryptedPartitionLength(uint32_t len)
{
    pHTable->encryptedPartitionLength = len / sizeof(uint32_t);
}

/******************************************************************************/
void VersalPartitionHeader::SetUnencryptedPartitionLength(uint32_t len)
{
    pHTable->unencryptedPartitionLength = len / sizeof(uint32_t);
}

/******************************************************************************/
void VersalPartitionHeader::SetTotalPartitionLength(uint32_t len)
{
    pHTable->totalPartitionLength = len / sizeof(uint32_t);
    for (std::list<AuthenticationCertificate*>::iterator acs = ac.begin(); acs != ac.end(); acs++)
    {
        if (*acs && (*acs)->section)
        {
            pHTable->totalPartitionLength += (uint32_t)((*acs)->section->Length / sizeof(uint32_t));
        }
    }
}

/******************************************************************************/
void VersalPartitionHeader::SetNextPartitionHeaderOffset(uint32_t addr)
{
	pHTable->nextPartitionHeaderOffset = addr / sizeof(uint32_t);
}

/******************************************************************************/
void VersalPartitionHeader::SetExecAddress(uint64_t addr)
{
    pHTable->destinationExecAddress = addr;
}

/******************************************************************************/
void VersalPartitionHeader::SetLoadAddress(uint64_t addr)
{
    if ((imageHeader->GetPartitionType() == PartitionType::CONFIG_DATA_OBJ) ||
        (imageHeader->GetPartitionType() == PartitionType::CFI) || 
        (imageHeader->GetPartitionType() == PartitionType::CFI_GSC) || 
        (imageHeader->GetPartitionType() == PartitionType::CFI_GSC_UNMASK))
    {
        addr = 0xFFFFFFFFFFFFFFFF;
    }
    pHTable->destinationLoadAddress = addr;
}

/******************************************************************************/
void VersalPartitionHeader::SetPartitionWordOffset(uint32_t addr)
{
    pHTable->partitionWordOffset = addr / sizeof(uint32_t);
    if (slaveBootSplitMode && (pHTable->partitionWordOffset != 0))
    {
        if (IsBootloader())
        {
            pHTable->partitionWordOffset = 0;
        }
        else
        {
            pHTable->partitionWordOffset -= (fullBhSize + allHdrSize + bootloaderSize) / sizeof(uint32_t);
        }
    }
}

/******************************************************************************/
void VersalPartitionHeader::SetPartitionAttributes(void)
{
    if (authCertPresent == 0)
    {
        authCertPresent = (imageHeader->GetAuthenticationType() == Authentication::RSA) || (imageHeader->GetAuthenticationType() == Authentication::ECDSA);
    }

    destCpu = imageHeader->GetDestCpu();
    exceptionLevel = imageHeader->GetExceptionLevel();
    trustzone = imageHeader->GetTrustZone();
    early_handoff = imageHeader->GetEarlyHandoff();
    partitionType = imageHeader->GetPartitionType();
    hivec = imageHeader->GetHivec();
    ownerType = imageHeader->GetPartOwner();
    authBlock = imageHeader->GetAuthBlock();
    dpaCM = imageHeader->GetDpacm();
    pufHdLoc = imageHeader->GetPufHdLocation();

    if (hivec) 
    {
        if(((execState == A53ExecState::AARCH64) && (partitionType == PartitionType::ELF)) 
            || (partitionType != PartitionType::ELF))
        {
            LOG_ERROR("BIF attribute 'hivec' is supported only for a72(32-bit) and r5 applications.");
        }
    }

    if (checksumType != Checksum::None)
    {
        if (imageHeader->GetChecksumContext()->Type() != Checksum::None)
        {
            LOG_ERROR("Cannot rechecksum a partition");
        }
    }
    else
    {
        checksumType = imageHeader->GetChecksumContext()->Type();
    }

    if (authBlock != 0)
    {
        int i = 0;
        while (authBlock != 1)
        {
            if (authBlock % 2 == 0) {
                authBlock = authBlock / 2;
                i++;
            }
        }
        authBlock = (i - 1);
    }

    if ((imageHeader->GetEncryptContext() != NULL) && (imageHeader->GetEncryptContext()->Type() == Encryption::AES))
    {
        /* Encryption to be enabled in release mode based on preencrypted flag avilable,
        since Encrption Context will not be AES */
        partitionEncrypted = 1;
    }
    else if (encryptFlag == 1)
    {
        partitionEncrypted = 1;
    }
    else
    {
        partitionEncrypted = 0;
    }

    uint8_t endian = 0;
    if (elfEndianess == Endianness::BigEndian)
    {
        endian = 1;
    }

    pHTable->partitionAttributes = (trustzone << vphtTrustzoneShift) |
                                   (exceptionLevel << vphtExceptionLevelShift) |
                                   (execState << vphtExecStateShift) |
                                   (destCpu << vphtDestCpuShift) |
                                   (checksumType << vphtChecksumTypeShift) |
                                   (pufHdLoc << vphtPufHDLocationShift) |
                                   (ownerType <<vphtPartitionOwnerShift) |
                                   (endian << vphtEndiannessShift) |
                                   (partitionType << vphtPartitionTypeShift) |
                                   (hivec << vphtHivecShift) |
                                   (dpaCM << vphtDpaCMShift) ;
}

/******************************************************************************/
void VersalPartitionHeader::SetSectionCount(uint32_t cnt)
{
    pHTable->dataSectionCount = cnt;
}

/******************************************************************************/
void VersalPartitionHeader::SetChecksumOffset(void)
{
    if (pHTable->checksumWordOffset == 0)
    {
        if (checksumSection)
        {
            pHTable->checksumWordOffset = (uint32_t)(checksumSection->Address / sizeof(uint32_t));  // 0x20: A word pointer to the Partition Checksum data.
        }
        else
        {
            pHTable->checksumWordOffset = 0;
        }
    }
    else
    {
        if (slaveBootSplitMode)
        {
            pHTable->checksumWordOffset -= (fullBhSize + allHdrSize + bootloaderSize) / sizeof(uint32_t);
        }
    }
}

/******************************************************************************/
void VersalPartitionHeader::SetAuthCertificateOffset(void)
{
    if (certificateRelativeByteOffset != 0)
    {
        /* For presigned images, partition addr + auth cert offset from start of partition */
        pHTable->authCertificateOffset = (uint32_t)((partition->section->Address + certificateRelativeByteOffset) / sizeof(uint32_t));
    }
    else if (ac.size() != 0)
    {
        /* For images signed on the go, get the address from the AC section created */
        AuthenticationCertificate* acs = ac.front();
        if (acs && acs->section)
        {
            pHTable->authCertificateOffset = (uint32_t)(acs->section->Address / sizeof(uint32_t));  // 0x28
        }
    }
    else if (imageHeader->GetAuthenticationType() == Authentication::RSA)
    {
        /* If the image is not yet signed, partition addr + partition length - cert size */
        AuthenticationContext::SetRsaKeyLength(RSA_4096_KEY_LENGTH);
        AuthenticationContext* auth = (VersalAuthenticationContext*)new VersalAuthenticationContext(Authentication::RSA);
        pHTable->authCertificateOffset = (uint32_t)((partition->section->Address + partition->section->Length - auth->GetCertificateSize()) / sizeof(uint32_t));
    }
    else if (imageHeader->GetAuthenticationType() == Authentication::ECDSA)
    {
        AuthenticationContext::SetRsaKeyLength(EC_P384_KEY_LENGTH);
        AuthenticationContext* auth = (VersalAuthenticationContext*)new VersalAuthenticationContext(Authentication::ECDSA);
        pHTable->authCertificateOffset = (uint32_t)((partition->section->Address + partition->section->Length - auth->GetCertificateSize()) / sizeof(uint32_t));
    }
    else
    {
        /* For unauthenticated cases */
        pHTable->authCertificateOffset = 0;
    }

    if (slaveBootSplitMode && (pHTable->authCertificateOffset != 0))
    {
        if (partition->section->isBootloader)
        {
            pHTable->authCertificateOffset = 0;
        }
        else
        {
            pHTable->authCertificateOffset -= (fullBhSize + allHdrSize + bootloaderSize) / sizeof(uint32_t);
        }
    }
}

/******************************************************************************/
void VersalPartitionHeader::SetReserved(void)
{
    memset(&pHTable->reserved, 0x00, MAX_PHT_RESERVED_VERSAL * sizeof(uint32_t));
}

/******************************************************************************/
void VersalPartitionHeader::SetPartitionId()
{
    pHTable->puid = partitionUid;
}

/******************************************************************************/
void VersalPartitionHeader::SetChecksum(void)
{
    pHTable->pHChecksum = ComputeWordChecksum(pHTable, sizeof(VersalPartitionHeaderTableStructure) - sizeof(uint32_t));
}

/******************************************************************************/
void VersalPartitionHeader::RealignSectionDataPtr(void)
{
    pHTable = (VersalPartitionHeaderTableStructure*)section->Data;
}

/******************************************************************************/
uint32_t VersalPartitionHeader::GetPartitionHeaderSize(void)
{
    return sizeof(VersalPartitionHeaderTableStructure);
}

/******************************************************************************/
uint32_t VersalPartitionHeader::GetSectionCount(void)
{
    return pHTable->dataSectionCount;
}

/******************************************************************************/
uint32_t VersalPartitionHeader::GetChecksum(void)
{
    return pHTable->pHChecksum;
}

/******************************************************************************/
uint64_t VersalPartitionHeader::GetExecAddress(void)
{
    return pHTable->destinationExecAddress;
}

/******************************************************************************/
uint64_t VersalPartitionHeader::GetLoadAddress(void)
{
    return pHTable->destinationLoadAddress;
}

/******************************************************************************/
uint32_t VersalPartitionHeader::GetEncryptedPartitionLength(void)
{
    return pHTable->encryptedPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t VersalPartitionHeader::GetUnencryptedPartitionLength(void)
{
    return pHTable->unencryptedPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t VersalPartitionHeader::GetTotalPartitionLength(void)
{
    return pHTable->totalPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint8_t VersalPartitionHeader::GetChecksumType(void)
{
    return ((pHTable->partitionAttributes >> vphtChecksumTypeShift) & vphtChecksumTypeMask);
}

/******************************************************************************/
uint8_t VersalPartitionHeader::GetOwnerType(void)
{
    return ((pHTable->partitionAttributes >> vphtPartitionOwnerShift) & vphtPartitionOwnerMask);
}

/******************************************************************************/
uint8_t VersalPartitionHeader::GetDestinationCpu(void)
{
    return ((pHTable->partitionAttributes >> vphtDestCpuShift) & vphtDestCpuMask);
}

/******************************************************************************/
uint8_t VersalPartitionHeader::GetProcessorExecState(void)
{
    return ((pHTable->partitionAttributes >> vphtExecStateShift) & vphtExecStateMask);
}

/******************************************************************************/
uint8_t VersalPartitionHeader::GetElfEndianess(void)
{
    if ((pHTable->partitionAttributes >> vphtEndiannessShift) & vphtEndiannessMask)
    {
        return Endianness::BigEndian;
    }
    else
    {
        return Endianness::LittleEndian;
    }
}

/******************************************************************************/
uint8_t VersalPartitionHeader::GetExceptionLevel(void)
{
    return ((pHTable->partitionAttributes >> vphtExceptionLevelShift) & vphtExceptionLevelMask);
}

/******************************************************************************/
uint8_t VersalPartitionHeader::GetTrustZone(void)
{
    return ((pHTable->partitionAttributes >> vphtTrustzoneShift) & vphtTrustzoneMask);
}

/******************************************************************************/
uint8_t VersalPartitionHeader::GetHivec(void)
{
    return ((pHTable->partitionAttributes >> vphtHivecShift) & vphtHivecMask);
}

/******************************************************************************/
uint32_t VersalPartitionHeader::GetAuthCertificateOffset(void)
{
    return (pHTable->authCertificateOffset * sizeof(uint32_t));
}

/******************************************************************************/
uint32_t VersalPartitionHeader::GetPartitionUid(void)
{
    return pHTable->puid;
}

/******************************************************************************/
uint32_t VersalPartitionHeader::GetPartitionWordOffset(void)
{
    return (pHTable->partitionWordOffset * sizeof(uint32_t));
}

/******************************************************************************/
PartitionType::Type VersalPartitionHeader::GetPartitionType(void)
{
    return (PartitionType::Type)((pHTable->partitionAttributes >> vphtPartitionTypeShift) & vphtPartitionTypeMask);
}

/******************************************************************************/
uint64_t VersalPartitionHeader::GetLQspiExecAddrForXip(uint64_t execAddr)
{
    //SH -Revisit these addresses needs to be modfied for versal
    if ((execAddr < LQSPI_BASE_ADDR_VERSAL) || (execAddr >(LQSPI_BASE_ADDR_VERSAL + LQSPI_SIZE_VERSAL)))
    {
        LOG_DEBUG(DEBUG_STAMP, "LQSPI Range: 0x%x-0x%x, Exec Addr: 0x%x", LQSPI_BASE_ADDR_VERSAL, LQSPI_BASE_ADDR_VERSAL + LQSPI_SIZE_VERSAL, execAddr);
        LOG_ERROR("Execution Address of FSBL is out of Linear QSPI range in XIP mode");
    }
    return (execAddr & LQSPI_RANGE_MASK_VERSAL);
}

/******************************************************************************/
uint32_t VersalPartitionHeader::GetPartitionPadSize64bBoundary(Section* sec)
{
    return ((64 - (sec->Length & 63)) & 63);
}

/******************************************************************************/
void VersalPartitionHeader::SetPartitionRevokeId(uint32_t id)
{
    pHTable->partitionRevokeId = id;
}

/******************************************************************************/
void VersalPartitionHeaderTable::Build(BootImage & bi, Binary & cache)
{
    LOG_INFO("Building the Partition Header Table");

    for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        (*partHdr)->Build(bi, cache);
    }

    if (bi.partitionHeaderList.size() > 0)
    {
        bi.partitionHeaderTable->firstSection = bi.partitionHeaderList.front()->section;
    }
    else
    {
        bi.partitionHeaderTable->firstSection = NULL;
    }

    if (bi.bifOptions->GetHeaderAC())
    {
        LOG_ERROR("Authentication is not supported for Versal ACAP in this version of Bootgen. \n\
           Please use Bootgen from Xilinx install");
    }

    bi.imageHeaderTable->SetTotalMetaHdrLength(bi.imageHeaderTable->metaHeaderLength);
    if (bi.options.bifOptions->GetHeaderEncyption())
    {
         LOG_ERROR("Encryption is not supported for Versal ACAP in this version of Bootgen. \n\
             Please use Bootgen from Xilinx install");
    }
    else
    {
        for (std::list<Section*>::iterator itr = bi.headers.begin(); itr != bi.headers.end(); itr++)
        {
            cache.Sections.push_back(*itr);
        }
    }
}

/******************************************************************************/
void VersalPartitionHeaderTable::Link(BootImage & bi)
{
    uint32_t numPart = 0;
    for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); )
    {
        PartitionHeader* currentPartHdr = (*partHdr);
        partHdr++;
        numPart++;
        if (numPart == bi.partitionHeaderList.size())
        {
            currentPartHdr->Link(bi, NULL);
        }
        else
        {
            currentPartHdr->Link(bi, (*partHdr));
        }
    }

    if (bi.options.bifOptions->GetHeaderEncyption())
    {
         LOG_ERROR("Encryption is not supported for Versal ACAP in this version of Bootgen. \n\
             Please use Bootgen from Xilinx install");
    }
    if (bi.bifOptions->GetHeaderAC())
    {
         LOG_ERROR("Authentication is not supported for Versal ACAP in this version of Bootgen. \n\
             Please use Bootgen from Xilinx install");
    }
}
