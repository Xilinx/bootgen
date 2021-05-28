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
        std::string partition_name = "";
        if ((imageHeader->GetFileList().size() > 0) && (imageHeader->IsSlrPartition() == false))
        {
            for (size_t i = 0; i < imageHeader->GetFileList().size(); i++)
            {
                partition_name += StringUtils::BaseName(imageHeader->GetFileList().at(i));
                if (i != (imageHeader->GetFileList().size() - 1))
                {
                    partition_name += "_";
                }
            }
        }
        else
        {
            partition_name = imageHeader->GetName();
        }
        name = "PartitionHeader " + partition_name + StringUtils::Format(".%d", index);
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
    authCertPresent = GetAuthCertFlag();
    encryptFlag = GetEncryptFlag();
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
        certificateRelativeByteOffset = (GetPartitionWordOffset() - GetAuthCertificateOffset());
    }
}

/******************************************************************************/
void VersalPartitionHeader::ReadData(std::ifstream& ifs)
{
    uint32_t dataLen = GetTotalPartitionLength();
    std::string partName = imageHeader->GetName() + "_" + std::to_string(partitionUid) + StringUtils::Format(".%d", index);
    Section* dsection = new Section(partName, dataLen);
    if (presigned)
    {
        ifs.seekg(GetAuthCertificateOffset());
    }
    else
    {
        ifs.seekg(GetPartitionWordOffset());
    }
    ifs.read((char*)dsection->Data, dsection->Length);
    dsection->isPartitionData = true;
    partition = new VersalPartition(this, dsection);

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
    if (!preencrypted)
    {
        SetPartitionSecureHdrIv(partitionSecHdrIv);
        SetPartitionKeySrc(partitionKeySrc, bi.bifOptions);
        SetPartitionGreyOrBlackIv(kekIvFile);
    }
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
    kekIvMust = false;
    switch (keyType)
    {
        case KeySource::EfuseRedKey:
            pHTable->partitionKeySource = EFUSE_RED_KEY;
            break;

        case KeySource::BbramRedKey:
            pHTable->partitionKeySource = BBRAM_RED_KEY;
            break;

        case KeySource::EfuseBlkKey:
            pHTable->partitionKeySource = EFUSE_BLK_KEY;
            kekIvFile = bifOptions->GetEfuseKekIVFile();
            if (kekIvFile == "")
            {
                LOG_ERROR("'efuse_kek_iv' is mandatory with 'keysrc=efuse_blk_key'");
            }
            kekIvMust = true;
            break;

        case KeySource::BbramBlkKey:
            pHTable->partitionKeySource = BBRAM_BLK_KEY;
            kekIvFile = bifOptions->GetBbramKekIVFile();
            if (kekIvFile == "")
            {
                LOG_ERROR("'bbram_kek_iv' is mandatory with 'keysrc=bbram_blk_key'");
            }
            kekIvMust = true;
            break;

        case KeySource::BhBlkKey:
            pHTable->partitionKeySource = BH_BLACK_KEY;
            kekIvFile = bifOptions->GetBHKekIVFile();
            if (kekIvFile == "")
            {
                LOG_ERROR("'bh_kek_iv' is mandatory with 'keysrc=bh_blk_key'");
            }
            kekIvMust = true;
            break;

        case KeySource::EfuseGryKey:
            pHTable->partitionKeySource = EFUSE_GRY_KEY;
            kekIvFile = bifOptions->GetEfuseKekIVFile();
            if (kekIvFile == "")
            {
                LOG_ERROR("'efuse_kek_iv' is mandatory with 'keysrc=efuse_gry_key'");
            }
            kekIvMust = true;
            break;

        case KeySource::BbramGryKey:
            pHTable->partitionKeySource = BBRAM_GRY_KEY;
            kekIvFile = bifOptions->GetBbramKekIVFile();
            if (kekIvFile == "")
            {
                LOG_ERROR("'bbram_kek_iv' is mandatory with 'keysrc=bbram_gry_key'");
            }
            kekIvMust = true;
            break;

        case KeySource::BhGryKey:
            pHTable->partitionKeySource = BH_GRY_KEY;
            kekIvFile = bifOptions->GetBHKekIVFile();
            if (kekIvFile == "")
            {
                LOG_ERROR("'bh_kek_iv' is mandatory with 'keysrc=bh_gry_key'");
            }
            kekIvMust = true;
            break;

        case KeySource::UserKey0:
            pHTable->partitionKeySource = USER_KEY0;
            break;

        case KeySource::UserKey1:
            pHTable->partitionKeySource = USER_KEY1;
            break;

        case KeySource::UserKey2:
            pHTable->partitionKeySource = USER_KEY2;
            break;

        case KeySource::UserKey3:
            pHTable->partitionKeySource = USER_KEY3;
            break;

        case KeySource::UserKey4:
            pHTable->partitionKeySource = USER_KEY4;
            break;

        case KeySource::UserKey5:
            pHTable->partitionKeySource = USER_KEY5;
            break;

        case KeySource::UserKey6:
            pHTable->partitionKeySource = USER_KEY6;
            break;

        case KeySource::UserKey7:
            pHTable->partitionKeySource = USER_KEY7;
            break;

        case KeySource::EfuseUserKey0:
            pHTable->partitionKeySource = EFUSE_USER_KEY0;
            break;

        case KeySource::EfuseUserBlkKey0:
            pHTable->partitionKeySource = EFUSE_USER_BLK_KEY0;
            kekIvFile = bifOptions->GetEfuseUserKek0IVFile();
            if (kekIvFile == "")
            {
                LOG_ERROR("'efuse_user_kek0_iv' is mandatory with 'keysrc=efuse_user_blk_key0'");
            }
            kekIvMust = true;
            break;

        case KeySource::EfuseUserGryKey0:
            pHTable->partitionKeySource = EFUSE_USER_GRY_KEY0;
            kekIvFile = bifOptions->GetEfuseUserKek0IVFile();
            if (kekIvFile == "")
            {
                LOG_ERROR("'efuse_user_kek0_iv' is mandatory with 'keysrc=efuse_user_gry_key0'");
            }
            kekIvMust = true;
            break;

        case KeySource::EfuseUserKey1:
            pHTable->partitionKeySource = EFUSE_USER_KEY1;
            break;

        case KeySource::EfuseUserBlkKey1:
            pHTable->partitionKeySource = EFUSE_USER_BLK_KEY1;
            kekIvFile = bifOptions->GetEfuseUserKek1IVFile();
            if (kekIvFile == "")
            {
                LOG_ERROR("'efuse_user_kek1_iv' is mandatory with 'keysrc=efuse_user_blk_key1'");
            }
            kekIvMust = true;
            break;

        case KeySource::EfuseUserGryKey1:
            pHTable->partitionKeySource = EFUSE_USER_BLK_KEY1;
            kekIvFile = bifOptions->GetEfuseUserKek1IVFile();
            if (kekIvFile == "")
            {
                LOG_ERROR("'efuse_user_kek1_iv' is mandatory with 'keysrc=efuse_user_gry_key1'");
            }
            kekIvMust = true;
            break;

        case KeySource::None:
        default:
            pHTable->partitionKeySource = 0;
            break;
    }
}

/******************************************************************************/
void VersalPartitionHeader::SetPartitionGreyOrBlackIv(std::string ivFile)
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
    if (presigned)
    {
        pHTable->partitionWordOffset = (addr+sizeof(AuthCertificate4096Sha3PaddingStructure)) / sizeof(uint32_t);
    }
    else
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
        pHTable->authCertificateOffset = (uint32_t)((partition->section->Address) / sizeof(uint32_t));
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
        AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
        AuthenticationContext* auth = (VersalAuthenticationContext*)new VersalAuthenticationContext(Authentication::RSA);
        pHTable->authCertificateOffset = (uint32_t)((partition->section->Address + partition->section->Length - auth->GetCertificateSize()) / sizeof(uint32_t));
    }
    else if (imageHeader->GetAuthenticationType() == Authentication::ECDSA)
    {
        AuthenticationContext::SetAuthenticationKeyLength(EC_P384_KEY_LENGTH);
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
DpaCM::Type VersalPartitionHeader::GetDpaCMFlag(void)
{
    return (DpaCM::Type)((pHTable->partitionAttributes >> vphtDpaCMShift) & vphtDpaCMMask);
}

/******************************************************************************/
PufHdLoc::Type VersalPartitionHeader::GetPufHdLocation(void)
{
    return (PufHdLoc::Type)((pHTable->partitionAttributes >> vphtPufHDLocationShift) & vphtPufHDLocationMask);
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
KeySource::Type VersalPartitionHeader::GetPartitionKeySource(void)
{
    return (KeySource::Type)pHTable->partitionKeySource;
}

/******************************************************************************/
uint8_t VersalPartitionHeader::GetEncryptFlag(void)
{
    if (pHTable->partitionKeySource != KeySource::None)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************/
uint8_t VersalPartitionHeader::GetAuthCertFlag(void)
{
    if (pHTable->authCertificateOffset != 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
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

    if ((bi.bifOptions->aHwrot == true) && (bi.options.bifOptions->metaHdrAttributes.authenticate == Authentication::None))
    {
        LOG_ERROR("Meta Header must be authenticated when 'a_hwrot' is enabled");
    }
    if ((bi.bifOptions->sHwrot == true) && (bi.options.bifOptions->metaHdrAttributes.encrypt == Encryption::None))
    {
        LOG_ERROR("Meta Header must be encrypted with 'keysrc=efuse_blk_key', when 's_hwrot' is enabled");
    }
    if ((bi.bifOptions->sHwrot == true) && (bi.options.bifOptions->metaHdrAttributes.encrKeySource != KeySource::EfuseBlkKey))
    {
        LOG_ERROR("Meta Header must be encrypted with 'keysrc=efuse_blk_key', when 's_hwrot' is enabled");
    }

    if (bi.bifOptions->GetHeaderAC())
    {
        LOG_INFO("Creating Header Authentication Certificate");
        ConfigureMetaHdrAuthenticationContext(bi);
        bi.headerAC = new VersalAuthenticationCertificate(bi.metaHdrAuthCtx);
        bi.headerAC->Build(bi, cache, bi.imageHeaderTable->section, false, true);
    }

    bi.imageHeaderTable->SetTotalMetaHdrLength(bi.imageHeaderTable->metaHeaderLength);
    if (bi.options.bifOptions->GetHeaderEncyption())
    {
        if (bi.bootloaderFound && !(bi.bootloaderAuthenticate) && !(bi.bootloaderEncrypt))
        {
            //LOG_ERROR("Bootloader must be encrypted or atleast authenticated to encrypt the Meta Header");
        }
        if (bi.options.bifOptions->metaHdrAttributes.encrKeySource == KeySource::None) 
        {
            bi.imageHeaderTable->metaHdrKeySrc = bi.options.cmdEncryptOptions->encryptedKeySource;
        }
        if (bi.options.bifOptions->metaHdrAttributes.encrKeyFile == "")
        {
            bi.options.bifOptions->metaHdrAttributes.encrKeyFile = "meta_header.nky";
        }

        std::vector<uint32_t> encrBlocks = bi.options.bifOptions->metaHdrAttributes.encrBlocks;
        int32_t defaultEncrBlockSize = bi.options.bifOptions->metaHdrAttributes.defEncrBlockSize;
        Binary::Length_t encrBlocksSize = 0;
        Binary::Length_t encrOverhead = 0;
        Binary::Length_t secureChunkSize = VersalPartition::GetSecureChunkSize();

        bi.options.bifOptions->metaHdrAttributes.encrBlocks.clear();

        /* Creating encryption blocks for 64KB from user specified blocks. Consider encryption overhead as well */
        for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
        {
            encrBlocksSize += encrBlocks[itr];
            encrOverhead += (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
            /* Push all the user blocks till the sum of user specified blocks and overhead is less than 64KB. */
            if ((encrBlocksSize + encrOverhead) < secureChunkSize)
            {
                bi.options.bifOptions->metaHdrAttributes.encrBlocks.push_back((encrBlocks[itr]));
            }
            /* When the sum of user specified blocks and overhead reaches 64KB, push that block and break. */
            else if ((encrBlocksSize + encrOverhead) == secureChunkSize)
            {
                bi.options.bifOptions->metaHdrAttributes.encrBlocks.push_back((encrBlocks[itr]));
                break;
            }
            /* If the sum of user specified blocks and overhead exceeds 64KB, truncate that block, .push and break.*/
            else
            {
                Binary::Length_t lastBlock = secureChunkSize - (encrBlocksSize - encrBlocks[itr] + encrOverhead);
                encrBlocksSize += (lastBlock - encrBlocks[itr]);
                bi.options.bifOptions->metaHdrAttributes.encrBlocks.push_back((lastBlock));
                LOG_WARNING("The last encryption block size is truncated to %d to fit into the secure chunk of 32KB.", lastBlock);
                break;
            }
        }

        /* If the user specified blocks, does not make a chunk of 64KB, then calculate the rest and push. */
        if (encrBlocksSize + encrOverhead < secureChunkSize)
        {
            /* If a default size(using (*)) is mentioned, */
            if (defaultEncrBlockSize != 0)
            {
                encrBlocksSize += defaultEncrBlockSize;
                encrOverhead += (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                /* then push the default size untill the sum of encr blocks and overhead is < or = 64KB.*/
                while (encrBlocksSize + encrOverhead < secureChunkSize)
                {
                    bi.options.bifOptions->metaHdrAttributes.encrBlocks.push_back(defaultEncrBlockSize);
                    encrOverhead += (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                    encrBlocksSize += defaultEncrBlockSize;
                }

                Binary::Length_t lastBlock = defaultEncrBlockSize;
                /* When the sum of encr blocks and overhead goes beyond 64KB, truncate the default size and push.*/
                if ((encrBlocksSize + encrOverhead) > secureChunkSize)
                {
                    lastBlock = secureChunkSize - (encrBlocksSize + encrOverhead - defaultEncrBlockSize);
                    encrBlocksSize += (lastBlock - defaultEncrBlockSize);
                    LOG_WARNING("The last encryption block size is truncated to %d to fit into the secure chunk of 32KB.", lastBlock);
                }
                bi.options.bifOptions->metaHdrAttributes.encrBlocks.push_back(lastBlock);
            }
            /* If a default size(using (*)) is not mentioned,
            then calculate the last block that makes sum of encr blocks and overhead = 64KB and push.*/
            else
            {
                encrOverhead += (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                bi.options.bifOptions->metaHdrAttributes.encrBlocks.push_back(secureChunkSize - (encrBlocksSize + encrOverhead));
            }
        }

        Binary::Length_t lastBlock = 0;
        uint32_t totalencrBlocks = bi.options.bifOptions->metaHdrAttributes.encrBlocks.size();
        uint32_t overhead = (totalencrBlocks) * (SECURE_HDR_SZ + AES_GCM_TAG_SZ);

        encrBlocks.clear();
        encrBlocks = bi.options.bifOptions->metaHdrAttributes.encrBlocks;
        bi.options.bifOptions->metaHdrAttributes.encrBlocks.clear();
        
        /* Due to encryption over head, the actual default size on which the partition needs to be Key rolled is always less than 64KB.
        So first calculate the default key roll data size by substracting the overhead. */
        /* Then calculate the number of such blocks possible on a given partition. */
        /* Note that the last block will always be based on the partition length.*/

        std::vector<uint32_t> secureChunkEncrBlocks;
        uint32_t actualSecureChunkSize = VersalPartition::GetSecureChunkSize() - overhead;
        uint32_t totalKeyRollencrBlocks = EncryptionContext::GetTotalEncryptionBlocks(bi.imageHeaderTable->metaHeaderLength, secureChunkEncrBlocks, actualSecureChunkSize, &lastBlock);
        secureChunkEncrBlocks.clear();

        for (uint32_t itr = 0; itr < totalKeyRollencrBlocks; itr++)
        {
            if ((itr == totalKeyRollencrBlocks - 1) && (lastBlock != 0))
            {
                secureChunkEncrBlocks.push_back(lastBlock);
                lastBlock = 0;
            }
            else
            {
                secureChunkEncrBlocks.push_back(actualSecureChunkSize);
            }
        }

        /* Now chunk each default key roll data size, based on user encryption blocks.
        Note that the last block will always be based on the partition length.*/
        bi.options.bifOptions->GetEncryptionBlocksList().clear();
        for (uint32_t itr1 = 0; itr1 < totalKeyRollencrBlocks; itr1++)
        {
            if ((itr1 == totalKeyRollencrBlocks - 1) && (secureChunkEncrBlocks[itr1] != actualSecureChunkSize))
            {
                Binary::Length_t encrBlocksSize = 0;
                for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
                {
                    encrBlocksSize += encrBlocks[itr];
                    if (secureChunkEncrBlocks[itr1] > encrBlocksSize)
                    {
                        bi.options.bifOptions->metaHdrAttributes.encrBlocks.push_back(encrBlocks[itr]);
                    }
                    else
                    {
                        bi.options.bifOptions->metaHdrAttributes.encrBlocks.push_back(secureChunkEncrBlocks[itr1] - (encrBlocksSize - encrBlocks[itr]));
                        break;
                    }
                }
            }
            else
            {
                for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
                {
                    bi.options.bifOptions->metaHdrAttributes.encrBlocks.push_back(encrBlocks[itr]);
                }
            }
        }

        totalencrBlocks = bi.options.bifOptions->metaHdrAttributes.encrBlocks.size();
        uint32_t totalBlocksOverhead = (totalencrBlocks + 1) * 64;
        bi.encryptedHeaders = new Section("EncryptedMetaHeader", bi.imageHeaderTable->metaHeaderLength + totalBlocksOverhead);
        cache.Sections.push_back(bi.encryptedHeaders);
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
void VersalPartitionHeaderTable::ConfigureMetaHdrAuthenticationContext(BootImage & bi)
{
    AuthenticationContext* biAuth = NULL;
    for (std::list<ImageHeader*>::iterator image = bi.imageList.begin(); image != bi.imageList.end(); image++)
    {
        if (((*image)->IsBootloader()) && ((*image)->GetAuthenticationType() == Authentication::None))
        {
            //LOG_ERROR("Bootloader must be authenticated to authenticate Meta Header.");
        }
    }

    biAuth = (AuthenticationContext*) new VersalAuthenticationContext(bi.options.bifOptions->metaHdrAttributes.authenticate);
    biAuth->hashType = bi.GetAuthHashAlgo();

    if (bi.bifOptions->metaHdrAttributes.ppk != "")
    {
        biAuth->SetPPKeyFile(bi.bifOptions->metaHdrAttributes.ppk);
    }
    else if (bi.bifOptions->GetPPKFileName() != "")
    {
        biAuth->SetPPKeyFile(bi.bifOptions->GetPPKFileName());
    }

    if (bi.bifOptions->metaHdrAttributes.psk != "")
    {
        biAuth->SetPSKeyFile(bi.bifOptions->metaHdrAttributes.psk);
    }
    else if (bi.bifOptions->GetPSKFileName() != "")
    {
        biAuth->SetPSKeyFile(bi.bifOptions->GetPSKFileName());
    }

    if (bi.bifOptions->metaHdrAttributes.spk != "")
    {
        biAuth->SetSPKeyFile(bi.bifOptions->metaHdrAttributes.spk);
    }
    else if (bi.bifOptions->GetSPKFileName() != "")
    {
        biAuth->SetSPKeyFile(bi.bifOptions->GetSPKFileName());
    }

    if (bi.bifOptions->metaHdrAttributes.ssk != "")
    {
        biAuth->SetSSKeyFile(bi.bifOptions->metaHdrAttributes.ssk);
    }
    else if (bi.bifOptions->GetSSKFileName() != "")
    {
        biAuth->SetSSKeyFile(bi.bifOptions->GetSSKFileName());
    }

    if (bi.bifOptions->metaHdrAttributes.spkSignature != "")
    {
        biAuth->SetSPKSignatureFile(bi.bifOptions->metaHdrAttributes.spkSignature);
    }
    biAuth->spkIdentification = bi.bifOptions->metaHdrAttributes.revokeId;

    //biAuth->SetPresignFile(bi.bifOptions->GetHeaderSignatureFile());
    if (bi.bifOptions->metaHdrAttributes.presign != "")
    {
        biAuth->SetPresignFile(bi.bifOptions->metaHdrAttributes.presign);
    }
    AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
    if (bi.bifOptions->metaHdrAttributes.authenticate == Authentication::ECDSA)
    {
        AuthenticationContext::SetAuthenticationKeyLength(EC_P384_KEY_LENGTH);
    }

    ImageHeaderTable* iht = bi.imageHeaderTable;
    biAuth->ResizeIfNecessary(iht->section);
    for (std::list<ImageHeader*>::iterator ih = bi.imageList.begin(); ih != bi.imageList.end(); ih++)
    {
        biAuth->ResizeIfNecessary((*ih)->section);
    }
    for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        biAuth->ResizeIfNecessary((*partHdr)->section);
    }

    /* Header table authentication */
    bi.metaHdrAuthCtx = (AuthenticationContext*)new VersalAuthenticationContext(biAuth, bi.bifOptions->metaHdrAttributes.authenticate);
    
    if (bi.bifOptions->metaHdrAttributes.presign != "")
    {
        bi.metaHdrAuthCtx->SetPresignFile(bi.bifOptions->metaHdrAttributes.presign);
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

    if (bi.bifOptions->GetHeaderAC())
    {
        bi.imageHeaderTable->SetTotalMetaHdrLength(bi.imageHeaderTable->metaHeaderLength + sizeof(AuthCertificate4096Sha3PaddingStructure));
        bi.imageHeaderTable->SetChecksum();
    }

    if (bi.options.bifOptions->GetHeaderEncyption())
    {
        LOG_INFO("Encrypting the Meta Header");
        EncryptionContext* encryptCtx = bi.imageHeaderTable->GetEncryptContext();

        bi.imageHeaderTable->SetTotalMetaHdrLength(bi.encryptedHeaders->Length);
        if (bi.bifOptions->GetHeaderAC())
        {
            bi.imageHeaderTable->SetTotalMetaHdrLength(bi.encryptedHeaders->Length + sizeof(AuthCertificate4096Sha3PaddingStructure));
        }
        bi.imageHeaderTable->SetChecksum();

        encryptCtx->Process(bi);
        bi.imageHeaderTable->metaHeaderLength = bi.imageHeaderTable->GetTotalMetaHdrLength();
    }

    if (bi.bifOptions->GetHeaderAC())
    {
        bi.headerAC->Link(bi, bi.imageHeaderTable->section);
    }

    bi.bifOptions->CheckForSameKeyandKeySrcPair(bi.aesKeyandKeySrc);
}
