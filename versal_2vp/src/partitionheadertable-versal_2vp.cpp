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
#include "partitionheadertable-versal_2vp.h"
#include "bootheader-versal_2vp.h"
#include "authentication-versal_2vp.h"
#include "authentication-versal_2vp-hybrid.h"
#include "bootimage-versal_2vp.h"

extern "C" {
#include "lms-utils.h"
}

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
Versal_2vpPartitionHeader::Versal_2vpPartitionHeader(ImageHeader* imageheader, int index)
    : partitionEncrypted(0)
    , partitionUid(0)
    , PartitionHeader(imageheader, index)
    , partitionKeySrc(KeySource::None)
    , dpaCM(DpaCM::DpaCMDisable)
    , pufHdLoc(PufHdLoc::PUFinEFuse)
    , kekIvFile("")
    , lockstep(Lockstep::LockstepDisable)
    , cluster(0)
    , tcmBoot(TcmBoot::TcmBootDisable)
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
    section = new Section(name, sizeof(Versal_2vpPartitionHeaderTableStructure));
    memset(section->Data.get(), 0, section->Length);

    pHTable = (Versal_2vpPartitionHeaderTableStructure*)section->Data.get();
    pHTable->partitionRevokeId = imageHeader->GetPartitionRevocationId();
}

/******************************************************************************/
Versal_2vpPartitionHeader::~Versal_2vpPartitionHeader()
{
}

/******************************************************************************/
void Versal_2vpPartitionHeader::ReadHeader(std::ifstream& ifs)
{
    ifs.read((char*)pHTable, sizeof(Versal_2vpPartitionHeaderTableStructure));
    uint32_t checksum = ComputeWordChecksum(pHTable, sizeof(Versal_2vpPartitionHeaderTableStructure) - sizeof(uint32_t));

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
    checksumType = Checksum::None;
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
    lockstep = GetLockStepFlag();
    cluster = GetDestinationCluster();
    tcmBoot = GetTcmBootFlag();

    presigned = (authCertPresent != 0);
    if (presigned)
    {
        certificateRelativeByteOffset = (GetPartitionWordOffset() - GetAuthCertificateOffset());
    }
}

/******************************************************************************/
void Versal_2vpPartitionHeader::ReadData(std::ifstream& ifs)
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
    ifs.read((char*)dsection->Data.get(), dsection->Length);
    dsection->isPartitionData = true;
    partition = std::make_unique<Versal_2vpPartition>(this, dsection);

    static uint8_t encryptionHeader[] =
    {
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBB,0x00,0x00,0x00,
        0x44,0x00,0x22,0x11,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x66,0x55,0x99,0xAA
    };
    preencrypted = (memcmp(dsection->Data.get(), encryptionHeader, sizeof(encryptionHeader)) == 0);
}

/******************************************************************************/
void Versal_2vpPartitionHeader::Build(BootImage& bi, Binary& cache)
{
    if (section != NULL)
    {
        bi.headers.push_back(section);
        bi.imageHeaderTable->metaHeaderLength += section->Length;
    }
}

/******************************************************************************/
void Versal_2vpPartitionHeader::Link(BootImage &bi, PartitionHeader* next_part_hdr)
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
    SetLoadAddress(loadAddress, bi.options.IsVersalNetSeries());
    SetPartitionWordOffset((uint32_t)partition->section->Address);

    /* PLM loadable partition support for versal_2vp:
       - For bootloader (ROM loadable): Hash Block 0 is in boot header area
       - For other partitions (PLM loadable): Hash Block 1 is after Meta Header
       - Hash block contains partition hashes for integrity verification
    */
    /* pick the hash block this partition belongs to.
       Default is Hash Block 1 (meta-header group). A partition assigned to a
       distinct-key owner (hashNumMap.second != 0) instead points at that
       owner's hash block (HBn). Owners resolve to their own block; sharers to
       their owner's. We also capture the owner's AC section so sharers can
       reference it in SetAuthCertificateOffset(). */
    Section*  grpHashBlock    = bi.imageHeaderTable->hashBlockSection;
    uint32_t  grpHashBlockLen = (bi.imageHeaderTable->hashBlockSection != NULL)
                                ? bi.imageHeaderTable->hashBlockSectionLength : 0;
    groupAcSection = NULL;

    if (!isBootloader && bi.hashNumMap.size() != 0)
    {
        uint32_t myPartNum = partitionNum;
        if (bi.IsBootloaderFound() == false)
            myPartNum = partitionNum + 1;

        uint32_t ownerNum = 0;
        for (size_t i = 0; i < bi.hashNumMap.size(); i++)
        {
            if (bi.hashNumMap[i].first == myPartNum)
            {
                ownerNum = bi.hashNumMap[i].second;
                break;
            }
        }

        if (ownerNum != 0)
        {
            for (std::list<PartitionHeader*>::iterator oh = bi.partitionHeaderList.begin();
                 oh != bi.partitionHeaderList.end(); oh++)
            {
                uint32_t ohNum = (*oh)->partitionNum;
                if (bi.IsBootloaderFound() == false)
                    ohNum = (*oh)->partitionNum + 1;

                if (ohNum == ownerNum)
                {
                    Versal_2vpPartition* ovp =
                        static_cast<Versal_2vpPartition*>((*oh)->partition.get());
                    if (ovp != NULL && ovp->GetHashBlockSection() != NULL)
                    {
                        grpHashBlock    = ovp->GetHashBlockSection();
                        grpHashBlockLen = ovp->GetHashBlockSectionLength();
                    }
                    if ((*oh)->ac.size() != 0 && (*oh)->ac.front() != NULL)
                        groupAcSection = (*oh)->ac.front()->section;
                    break;
                }
            }
        }
    }

    bool isOwnerHashBlock = (grpHashBlock != NULL &&
                             grpHashBlock != bi.imageHeaderTable->hashBlockSection);

    if (grpHashBlock != NULL)
    {
        if (isOwnerHashBlock)
            SetHashBlockWordOffset((uint32_t)grpHashBlock->Address);
        else
            SetHashBlockWordOffset(grpHashBlock->WordAddress());

        // For PLM loadable authenticated/encrypted partitions, set the hash block length
        if (!isBootloader &&
            (imageHeader->GetAuthenticationType() != Authentication::None ||
             imageHeader->GetEncryptContext()->Type() != Encryption::None))
        {
            SetHashBlockLength(isOwnerHashBlock ? (grpHashBlockLen / (uint32_t)sizeof(uint32_t))
                                                : grpHashBlockLen);
            LOG_TRACE("Partition hash block: owner=%d offset=0x%X, length=%d",
                      isOwnerHashBlock, grpHashBlock->WordAddress(), grpHashBlockLen);
        }
        else
        {
            SetHashBlockLength(0);
        }
    }
    else
    {
        SetHashBlockWordOffset(0);
        SetHashBlockLength(0);
    }

    SetPartitionAttributes();
    SetChecksumOffset();
    SetAuthCertificateOffset();
    SetPartitionId();
    if (!preencrypted)
    {
        SetPartitionSecureHdrIv(partitionSecHdrIv.get());
        SetPartitionKeySrc(partitionKeySrc, bi.bifOptions);
        SetPartitionGreyOrBlackIv(kekIvFile);
    }
    SetReserved();
    SetChecksum();
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetPartitionSecureHdrIv(uint8_t* iv)
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
void Versal_2vpPartitionHeader::SetPartitionKeySrc(KeySource::Type keyType, BifOptions* bifOptions)
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
            pHTable->partitionKeySource = EFUSE_USER_GRY_KEY1;
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
void Versal_2vpPartitionHeader::SetPartitionGreyOrBlackIv(std::string ivFile)
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
void Versal_2vpPartitionHeader::SetEncryptedPartitionLength(uint32_t len)
{
    pHTable->encryptedPartitionLength = len / sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetUnencryptedPartitionLength(uint32_t len)
{
    pHTable->unencryptedPartitionLength = len / sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetTotalPartitionLength(uint32_t len)
{
    pHTable->totalPartitionLength = len / sizeof(uint32_t);
    /* Only the ROM-loaded bootloader counts its AC in total_length (the ROM
       fetches AC + PLM/PMC content as one blob). PLM-loadable partitions -
       including multi-SPK owners that now carry their own AC/HBn - must report
       total_length = partition data + chunk hashes only. The device loads such a
       partition from partition_offset while its AC lives separately, so adding
       the AC here overshoots the length (device then mis-sizes / reads 0). */
    if (isBootloader)
    {
        for (std::list<AuthenticationCertificate*>::iterator acs = ac.begin(); acs != ac.end(); acs++)
        {
            if (*acs && (*acs)->section)
            {
                pHTable->totalPartitionLength += (uint32_t)((*acs)->section->Length / sizeof(uint32_t));
            }
        }
    }
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetNextPartitionHeaderOffset(uint32_t addr)
{
	pHTable->nextPartitionHeaderOffset = addr / sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetExecAddress(uint64_t addr)
{
    pHTable->destinationExecAddress = addr;
}

/******************************************************************************/
/* versal_2vp shares the Versal PS/RPU, so its Cortex-R5 TCM is mapped at the
   Versal global TCM addresses, not the Versal_2VE_2VM (R52) addresses. */
#define XLOADER_R5_0_TCMA_BASE_ADDR 	0xFFE00000 /* R5_0 TCM global base address */
#define XLOADER_R5_1_TCMA_BASE_ADDR 	0xFFE90000 /* R5_1 TCM global base address */
#define XLOADER_R5_TCMA_LOAD_ADDRESS	0x0        /* R5 TCM local load address */
#define XLOADER_R5_TCM_BANK_LENGTH  	0x10000    /* R5 TCM per-bank length (64 KB) */
#define XLOADER_R5_TCM_TOTAL_LENGTH 	0x40000    /* R5 TCM total length (256 KB) */

static uint64_t GetR5LoadAddr(DestinationCPU::Type dstCpu, uint64_t loadAddr, uint32_t len)
{
    uint64_t address = loadAddr;

    if ((dstCpu == DestinationCPU::R5_0) &&
        (address < (XLOADER_R5_TCMA_LOAD_ADDRESS + XLOADER_R5_TCM_TOTAL_LENGTH))) {
        if (((address % XLOADER_R5_TCM_BANK_LENGTH) + len) >
            XLOADER_R5_TCM_BANK_LENGTH) {
            //Status = XPlmi_UpdateStatus(XLOADER_ERR_TCM_ADDR_OUTOF_RANGE, 0);
        }

        address += XLOADER_R5_0_TCMA_BASE_ADDR;
    }
    else if ((dstCpu == DestinationCPU::R5_1) &&
        (address < (XLOADER_R5_TCMA_LOAD_ADDRESS + XLOADER_R5_TCM_TOTAL_LENGTH))) {
        if (((address % XLOADER_R5_TCM_BANK_LENGTH) + len) >
            XLOADER_R5_TCM_BANK_LENGTH) {
            //Status = XPlmi_UpdateStatus(XLOADER_ERR_TCM_ADDR_OUTOF_RANGE, 0);
        }

        address += XLOADER_R5_1_TCMA_BASE_ADDR;
    }
    else {
        /* Do nothing */
    }

    /* Update the load address */
    return address;
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetLoadAddress(uint64_t addr, bool versalNetSeries)
{
    if ((imageHeader->GetPartitionType() == PartitionType::CONFIG_DATA_OBJ) ||
        (imageHeader->GetPartitionType() == PartitionType::CFI) || 
        (imageHeader->GetPartitionType() == PartitionType::CFI_GSC) || 
        (imageHeader->GetPartitionType() == PartitionType::CFI_GSC_UNMASK))
    {
        addr = 0xFFFFFFFFFFFFFFFF;
    }
    if (versalNetSeries)
    {
        if((imageHeader->GetDestCpu() == DestinationCPU::R5_0) || (imageHeader->GetDestCpu() == DestinationCPU::R5_1))
            addr = GetR5LoadAddr(imageHeader->GetDestCpu(), addr, GetEncryptedPartitionLength());
    }

    pHTable->destinationLoadAddress = addr;
}

/******************************************************************************/
// ...existing code...

/******************************************************************************/
void Versal_2vpPartitionHeader::SetPartitionWordOffset(uint32_t addr)
{
    if (presigned)
    {
        pHTable->partitionWordOffset = (addr+sizeof(AuthCertificate4096Sha3PaddingHBStructure_versal2vp)) / sizeof(uint32_t);
    }
    else
    {
        pHTable->partitionWordOffset = addr / sizeof(uint32_t);
    }
    
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
void Versal_2vpPartitionHeader::SetHashBlockWordOffset(uint32_t addr)
{
    pHTable->hashBlockOffset = addr / sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetHashBlockLength(uint32_t length)
{
    pHTable->hashBlockLength = length;
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetPartitionAttributes(void)
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
    cluster = imageHeader->GetClusterNum();
    if (imageHeader->GetLockStepFlag() == true)
    {
        lockstep = Lockstep::LockstepEnable;
    }
    if (imageHeader->GetTcmBootFlag() == true)
    {
        tcmBoot = TcmBoot::TcmBootEnable;
    }

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
        /* versal_2vp chunks every partition and stores per-chunk SHA3 hashes in the hash
           block, so the checksum type must always be SHA3. Authenticated partitions
           do not populate the checksum context (its type is None), so relying on it
           would leave the checksum-type field as 0 even though chunk hashing is done. */
        checksumType = Checksum::SHA3;
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

    pHTable->partitionAttributes = (trustzone << v2vpphtTrustzoneShift) |
                                   (exceptionLevel << v2vpphtExceptionLevelShift) |
                                   (execState << v2vpphtExecStateShift) |
                                   (destCpu << v2vpphtDestCpuShift) |
                                   (checksumType << v2vpphtChecksumTypeShift) |
                                   (pufHdLoc << v2vpphtPufHDLocationShift) |
                                   (ownerType <<v2vpphtPartitionOwnerShift) |
                                   (endian << v2vpphtEndiannessShift) |
                                   (partitionType << v2vpphtPartitionTypeShift) |
                                   (hivec << v2vpphtHivecShift) |
                                   (dpaCM << v2vpphtDpaCMShift) |
                                   (cluster << v2vpphtClusterShift) |
                                   (lockstep << v2vpphtlockStepShift) |
                                   (tcmBoot << v2vpphtTcmBootShift);
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetSectionCount(uint32_t cnt)
{
    pHTable->dataSectionCount = cnt;
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetChecksumOffset(void)
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
void Versal_2vpPartitionHeader::SetAuthCertificateOffset(void)
{   
    /* For non-bootloader (PLM loadable) partitions with authentication:
       No individual AC is created - they use the metaheader's AC via Hash Block 1.
       So authCertificateOffset should be 0.
       
       Note: Skip this for presigned partitions (certificateRelativeByteOffset != 0) 
       as they have their own embedded AC. */
    if (!isBootloader &&
        imageHeader->GetAuthenticationType() != Authentication::None &&
        ac.size() == 0 &&
        certificateRelativeByteOffset == 0)
    {
        /* grouped sharer has no AC of its own but belongs to an
           owner's hash block. Point it at the owner's AC. */
        if (groupAcSection != NULL)
        {
            pHTable->authCertificateOffset =
                (uint32_t)(groupAcSection->Address / sizeof(uint32_t));
            LOG_TRACE("PLM loadable partition '%s' - grouped sharer, using owner AC at 0x%X.",
                      imageHeader->GetName().c_str(), pHTable->authCertificateOffset);
            return;
        }

        LOG_TRACE("PLM loadable partition '%s' - no individual AC, using metaheader AC via HB1. Setting authCertificateOffset to 0.",
                  imageHeader->GetName().c_str());
        pHTable->authCertificateOffset = 0;
        return;
    }
    
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
        AuthenticationContext* auth = (Versal_2vpAuthenticationContext*)new Versal_2vpAuthenticationContext(Authentication::RSA);
        pHTable->authCertificateOffset = (uint32_t)((partition->section->Address + partition->section->Length - auth->GetCertificateSize()) / sizeof(uint32_t));
    }
    else if (imageHeader->GetAuthenticationType() == Authentication::ECDSA)
    {
        AuthenticationContext::SetAuthenticationKeyLength(EC_P384_KEY_LENGTH);
        AuthenticationContext* auth = (Versal_2vpAuthenticationContext*)new Versal_2vpAuthenticationContext(Authentication::ECDSA);
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
void Versal_2vpPartitionHeader::SetReserved(void)
{
    pHTable->measuredBootAddress = 0;                   // 0x58
    
    /* Per versal_2vp PDI spec: 
       - 0x5C: hashBlockLength (set separately via SetHashBlockLength)
       - 0x60: hashBlockOffset (set separately via SetHashBlockWordOffset)  
       - 0x64-0x78: Reserved (0)
       For PLM loadable partitions with authentication, the hash block length/offset
       point to Hash Block 1 which contains the partition hashes. */
    memset(pHTable->reserved, 0x00, sizeof(pHTable->reserved));

    if (!isBootloader && imageHeader != NULL && 
        imageHeader->GetAuthenticationType() != Authentication::None)
    {
        LOG_TRACE("PLM loadable partition with authentication: hashBlockLength=%d, hashBlockOffset=0x%X",
                  pHTable->hashBlockLength, pHTable->hashBlockOffset);
    }
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetPartitionId()
{
    pHTable->puid = partitionUid;
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetChecksum(void)
{
    pHTable->pHChecksum = ComputeWordChecksum(pHTable, sizeof(Versal_2vpPartitionHeaderTableStructure) - sizeof(uint32_t));
}

/******************************************************************************/
void Versal_2vpPartitionHeader::RealignSectionDataPtr(void)
{
    pHTable = (Versal_2vpPartitionHeaderTableStructure*)section->Data.get();
}

/******************************************************************************/
uint32_t Versal_2vpPartitionHeader::GetPartitionHeaderSize(void)
{
    return sizeof(Versal_2vpPartitionHeaderTableStructure);
}

/******************************************************************************/
uint32_t Versal_2vpPartitionHeader::GetSectionCount(void)
{
    return pHTable->dataSectionCount;
}

/******************************************************************************/
uint32_t Versal_2vpPartitionHeader::GetChecksum(void)
{
    return pHTable->pHChecksum;
}

/******************************************************************************/
uint64_t Versal_2vpPartitionHeader::GetExecAddress(void)
{
    return pHTable->destinationExecAddress;
}

/******************************************************************************/
uint64_t Versal_2vpPartitionHeader::GetLoadAddress(void)
{
    return pHTable->destinationLoadAddress;
}

/******************************************************************************/
uint32_t Versal_2vpPartitionHeader::GetEncryptedPartitionLength(void)
{
    return pHTable->encryptedPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t Versal_2vpPartitionHeader::GetUnencryptedPartitionLength(void)
{
    return pHTable->unencryptedPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t Versal_2vpPartitionHeader::GetTotalPartitionLength(void)
{
    return pHTable->totalPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint8_t Versal_2vpPartitionHeader::GetChecksumType(void)
{
    return ((pHTable->partitionAttributes >> v2vpphtChecksumTypeShift) & v2vpphtChecksumTypeMask);
}

/******************************************************************************/
uint8_t Versal_2vpPartitionHeader::GetOwnerType(void)
{
    return ((pHTable->partitionAttributes >> v2vpphtPartitionOwnerShift) & v2vpphtPartitionOwnerMask);
}

/******************************************************************************/
uint8_t Versal_2vpPartitionHeader::GetDestinationCpu(void)
{
    return ((pHTable->partitionAttributes >> v2vpphtDestCpuShift) & v2vpphtDestCpuMask);
}

/******************************************************************************/
uint8_t Versal_2vpPartitionHeader::GetDestinationCluster(void)
{
    return ((pHTable->partitionAttributes >> v2vpphtClusterShift) & v2vpphtClusterMask);
}

/******************************************************************************/
uint8_t Versal_2vpPartitionHeader::GetProcessorExecState(void)
{
    return ((pHTable->partitionAttributes >> v2vpphtExecStateShift) & v2vpphtExecStateMask);
}

/******************************************************************************/
uint8_t Versal_2vpPartitionHeader::GetElfEndianess(void)
{
    if ((pHTable->partitionAttributes >> v2vpphtEndiannessShift) & v2vpphtEndiannessMask)
    {
        return Endianness::BigEndian;
    }
    else
    {
        return Endianness::LittleEndian;
    }
}

/******************************************************************************/
uint8_t Versal_2vpPartitionHeader::GetExceptionLevel(void)
{
    return ((pHTable->partitionAttributes >> v2vpphtExceptionLevelShift) & v2vpphtExceptionLevelMask);
}

/******************************************************************************/
uint8_t Versal_2vpPartitionHeader::GetTrustZone(void)
{
    return ((pHTable->partitionAttributes >> v2vpphtTrustzoneShift) & v2vpphtTrustzoneMask);
}

/******************************************************************************/
uint8_t Versal_2vpPartitionHeader::GetHivec(void)
{
    return ((pHTable->partitionAttributes >> v2vpphtHivecShift) & v2vpphtHivecMask);
}

/******************************************************************************/
uint32_t Versal_2vpPartitionHeader::GetAuthCertificateOffset(void)
{
    return (pHTable->authCertificateOffset * sizeof(uint32_t));
}

/******************************************************************************/
uint32_t Versal_2vpPartitionHeader::GetPartitionUid(void)
{
    return pHTable->puid;
}

/******************************************************************************/
uint32_t Versal_2vpPartitionHeader::GetPartitionWordOffset(void)
{
    return (pHTable->partitionWordOffset * sizeof(uint32_t));
}

/******************************************************************************/
PartitionType::Type Versal_2vpPartitionHeader::GetPartitionType(void)
{
    return (PartitionType::Type)((pHTable->partitionAttributes >> v2vpphtPartitionTypeShift) & v2vpphtPartitionTypeMask);
}

/******************************************************************************/
DpaCM::Type Versal_2vpPartitionHeader::GetDpaCMFlag(void)
{
    return (DpaCM::Type)((pHTable->partitionAttributes >> v2vpphtDpaCMShift) & v2vpphtDpaCMMask);
}

/******************************************************************************/
PufHdLoc::Type Versal_2vpPartitionHeader::GetPufHdLocation(void)
{
    return (PufHdLoc::Type)((pHTable->partitionAttributes >> v2vpphtPufHDLocationShift) & v2vpphtPufHDLocationMask);
}

/******************************************************************************/
Lockstep::Type Versal_2vpPartitionHeader::GetLockStepFlag (void)
{
    return (Lockstep::Type)((pHTable->partitionAttributes >> v2vpphtlockStepShift) & v2vpphtlockStepMask);
}

/******************************************************************************/
uint64_t Versal_2vpPartitionHeader::GetLQspiExecAddrForXip(uint64_t execAddr)
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
uint32_t Versal_2vpPartitionHeader::GetPartitionPadSize64bBoundary(Section* sec)
{
    return ((64 - (sec->Length & 63)) & 63);
}

/******************************************************************************/
void Versal_2vpPartitionHeader::SetPartitionRevokeId(uint32_t id)
{
    pHTable->partitionRevokeId = id;
}

/******************************************************************************/
KeySource::Type Versal_2vpPartitionHeader::GetPartitionKeySource(void)
{
    return (KeySource::Type)pHTable->partitionKeySource;
}

/******************************************************************************/
uint8_t Versal_2vpPartitionHeader::GetEncryptFlag(void)
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
uint8_t Versal_2vpPartitionHeader::GetAuthCertFlag(void)
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
TcmBoot::Type Versal_2vpPartitionHeader::GetTcmBootFlag(void)
{
    return (TcmBoot::Type)((pHTable->partitionAttributes >> v2vpphtTcmBootShift) & v2vpphtTcmBootMask);
}

/******************************************************************************/
void Versal_2vpPartitionHeaderTable::Build(BootImage & bi, Binary & cache)
{
    LOG_INFO("Building the Partition Header Table");

    if (!bi.options.IsVersalNetSeries())
    {
        if (getenv("BOOTGEN_SKIP_MAX_PARTITIONS_CHECK") == NULL)
        {
            if (bi.subSysImageList.size() > MAX_NUM_IMAGES_VERSAL)
            {
                LOG_ERROR("The maximum number of images supported for Versal is %d.\n           No. of images found : %d", MAX_NUM_IMAGES_VERSAL, bi.subSysImageList.size());
            }
            if (bi.partitionHeaderList.size() > MAX_NUM_PARTITIONS_VERSAL)
            {
                LOG_ERROR("The maximum number of partitions supported for Versal is %d.\n           No. of partitions found : %d", MAX_NUM_PARTITIONS_VERSAL, bi.partitionHeaderList.size());
            }
        }
        else
        {
            LOG_WARNING("The ENV BOOTGEN_SKIP_MAX_PARTITIONS_CHECK is set. Skipping check for maximum number of partitions/images.");
        }
    }

    for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        (*partHdr)->Build(bi, cache);
    }

    //UpdateAtfHandoffParams(bi);

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
        Binary::Length_t secureChunkSize = bi.GetSecureChunkSize(true);

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
        uint32_t actualSecureChunkSize = bi.GetSecureChunkSize(true) - overhead;
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
        cache.Sections.push_back(std::unique_ptr<Section>(bi.encryptedHeaders));
    }
    else
    {
        for (std::list<Section*>::iterator itr = bi.headers.begin(); itr != bi.headers.end(); itr++)
        {
            cache.Sections.push_back(std::unique_ptr<Section>(*itr));
        }
    }

    /* PDI Section Ordering for versal_2vp (single and hybrid signing):
       The correct order after Meta Header is:
         MH (IHT + IH + PH) → Hash Block 1 → MH AC1 [→ MH AC2 for hybrid]
       
       Hash Block 1 must come BEFORE the Meta Header AC because:
       1. Hash Block 1 contains the Meta Header hash
       2. The AC signs Hash Block 1
       3. PLM reads: MH → HB1 → verifies AC signature on HB1
    */

    /* Check if bootloader is authenticated - if so, inherit settings for meta header */
    bool bootloaderAuthenticated = false;
    bool bootloaderHybridAuth = false;
    ImageHeader* bootloaderImage = nullptr;
    
    for (std::list<ImageHeader*>::iterator image = bi.imageList.begin(); image != bi.imageList.end(); image++)
    {
        if ((*image)->IsBootloader())
        {
            bootloaderImage = *image;
            if ((*image)->GetAuthenticationType() != Authentication::None)
            {
                bootloaderAuthenticated = true;
                /* Check if bootloader uses hybrid authentication */
                AuthenticationContext* blAuthCtx = (*image)->GetAuthContext();
                if (blAuthCtx != nullptr)
                {
                    /* Check if this is a hybrid authentication context */
                    /* Method 1: KMD-based approach - check for second algorithm key files */
                    if (!bi.bifOptions->GetPPKFileName1().empty() || !bi.bifOptions->GetSPKFileName1().empty() ||
                        !bi.bifOptions->GetPKFileName1().empty() || !bi.bifOptions->GetSKFileName1().empty())
                    {
                        bootloaderHybridAuth = true;
                        LOG_INFO("Meta Header: Detected bootloader hybrid authentication (KMD-based: pkfile1=%s, skfile1=%s)",
                                 bi.bifOptions->GetPKFileName1().c_str(), bi.bifOptions->GetSKFileName1().c_str());
                    }
                    /* Method 2: Non-KMD approach - check if auth context is a hybrid context */
                    /* This detects 'authentication = algorithm1, algorithm2' BIF syntax */
                    else 
                    {
                        Versal_2vpHybridAuthenticationContext* hybridCtx = 
                            dynamic_cast<Versal_2vpHybridAuthenticationContext*>(blAuthCtx);
                        if (hybridCtx != nullptr && hybridCtx->IsHybrid()) 
                        {
                            bootloaderHybridAuth = true;
                            std::vector<Authentication::Type> algos = hybridCtx->GetHybridAlgorithms();
                            LOG_INFO("Meta Header: Detected bootloader hybrid authentication (non-KMD: %s + %s)",
                                     hybridCtx->GetAlgorithmName(algos[0]), hybridCtx->GetAlgorithmName(algos[1]));
                        }
                    }
                }
            }
            break;
        }
    }

    /* If bootloader is authenticated but metaheader auth is not explicitly set, inherit from bootloader */
    bool shouldAuthenticateMetaHeader = bi.bifOptions->GetHeaderAC() || bootloaderAuthenticated || 
                                        bi.bifOptions->metaHdrAttributes.authenticate != Authentication::None;
    
    /* Step 1: Configure Meta Header authentication context first (needed for HB1 size calculation) */
    if (shouldAuthenticateMetaHeader)
    {
        LOG_INFO("Configuring Meta Header authentication (bootloaderAuth=%d, hybridAuth=%d)", 
                 bootloaderAuthenticated, bootloaderHybridAuth);
        
        /* If metaHdrAttributes.authenticate is None but bootloader is authenticated, 
           inherit the bootloader's authentication type */
        if (bi.options.bifOptions->metaHdrAttributes.authenticate == Authentication::None && 
            bootloaderImage != nullptr)
        {
            bi.options.bifOptions->metaHdrAttributes.authenticate = bootloaderImage->GetAuthenticationType();
            LOG_INFO("Meta Header: Inherited authentication type from bootloader: %d", 
                     (int)bootloaderImage->GetAuthenticationType());
        }
        
        if (bootloaderHybridAuth)
        {
            /* For hybrid authentication, create a hybrid authentication context for Meta Header */
            LOG_INFO("Creating HYBRID authentication context for Meta Header (inherited from bootloader)");
            ConfigureMetaHdrHybridAuthenticationContext(bi, bootloaderImage);
        }
        else
        {
            /* Single signing - create regular authentication context */
        ConfigureMetaHdrAuthenticationContext(bi);
        }
    }

        /* group non-bootloader partitions by key set
         Decide which hash block each partition's hash belongs to:
         ownerNum == 0        -> rides on Hash Block 1 (meta-header key set)
         ownerNum == partNum  -> this partition owns a new hash block + AC
         ownerNum == other    -> shares the hash block owned by 'other'
         Grouped on the full key tuple (ppk,psk,spk,ssk,spk_revoke_id). */
       bi.hashNumMap.clear();
       for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin();
            partHdr != bi.partitionHeaderList.end(); partHdr++)
       {
           if ((*partHdr)->IsBootloader())
               continue;
   
           uint32_t partNum = (*partHdr)->partitionNum;
           if (bi.IsBootloaderFound() == false)
               partNum = partNum + 1;
   
           ImageHeader* ih = (*partHdr)->imageHeader;
   
           bool noKeys = (ih->GetAuthenticationType() == Authentication::None) ||
                         (ih->GetPskFile().empty() && ih->GetSskFile().empty() &&
                          ih->GetPpkFile().empty() && ih->GetSpkFile().empty());
   
           bool matchesMetaHdr = (bi.metaHdrAuthCtx != nullptr) &&
               ih->GetPpkFile()         == bi.metaHdrAuthCtx->ppkFile &&
               ih->GetPskFile()         == bi.metaHdrAuthCtx->pskFile &&
               ih->GetSpkFile()         == bi.metaHdrAuthCtx->spkFile &&
               ih->GetSskFile()         == bi.metaHdrAuthCtx->sskFile &&
               ih->GetSpkRevocationId() == bi.metaHdrAuthCtx->spkIdentification;
   
           if (noKeys || matchesMetaHdr)
           {
               bi.hashNumMap.push_back(std::make_pair(partNum, 0u));
               continue;
           }
   
           for (std::list<PartitionHeader*>::iterator owner = bi.partitionHeaderList.begin();
                owner != bi.partitionHeaderList.end(); owner++)
           {
               if ((*owner)->IsBootloader())
                   continue;
   
               ImageHeader* oh = (*owner)->imageHeader;
               if (ih->GetPpkFile()         == oh->GetPpkFile() &&
                   ih->GetPskFile()         == oh->GetPskFile() &&
                   ih->GetSpkFile()         == oh->GetSpkFile() &&
                   ih->GetSskFile()         == oh->GetSskFile() &&
                   ih->GetSpkRevocationId() == oh->GetSpkRevocationId())
               {
                   uint32_t ownerNum = (*owner)->partitionNum;
                   if (bi.IsBootloaderFound() == false)
                       ownerNum = ownerNum + 1;
                   bi.hashNumMap.push_back(std::make_pair(partNum, ownerNum));
                   break;
               }
           }
       }



     /* Step 2: Create and push Hash Block 1 BEFORE Meta Header AC */

    /* Hash Block 1 holds the meta-header hash (entry 0) plus only the
       partitions grouped into it (ownerNum == 0). Owners and the partitions
       that share them go into their own hash blocks */
    uint32_t hb1Entries = 1;   /* entry 0 = meta-header hash */
    for (size_t i = 0; i < bi.hashNumMap.size(); i++)
    {
        if (bi.hashNumMap[i].second == 0)
            hb1Entries++;
    }

    if (hb1Entries != 0)
    {
        bi.imageHeaderTable->hashBlockSectionLength = hb1Entries * (bi.hash->GetHashLength() + HASH_BLOCK_INDEX_BYTES);
        bi.imageHeaderTable->hashBlockSectionLength += PADDING_16B(bi.imageHeaderTable->hashBlockSectionLength);

        /* versal_2vp CORRECTION: Hash Block 1 contains ONLY hash entries (no signatures).
           Signatures go in the AC section, NOT in the Hash Block.
           Hash Block 1 = [Entry0: MH hash] + [Entry1..N: Partition hashes] */
        bi.imageHeaderTable->hashBlockSection = new Section("HashBlock", bi.imageHeaderTable->hashBlockSectionLength);
        LOG_INFO("Hash Block 1 created: %u bytes (hash entries only, no signatures)", 
                 bi.imageHeaderTable->hashBlockSectionLength);
        LOG_INFO("  Entries: %d, Entry size: %zu bytes each", 
                 bi.numHashTableEntries, bi.hash->GetHashLength() + HASH_BLOCK_INDEX_BYTES);

        /* For encryption-only (no auth), add GCM tag space */
        if (bi.options.bifOptions->metaHdrAttributes.encrypt != Encryption::None && 
            !shouldAuthenticateMetaHeader)
        {
            bi.imageHeaderTable->hashBlockSection->IncreaseLengthAndPadTo(
                bi.imageHeaderTable->hashBlockSectionLength + AES_GCM_TAG_SZ, 0);
            LOG_INFO("Hash Block 1 increased for encryption GCM tag: %zu bytes", 
                     bi.imageHeaderTable->hashBlockSection->Length);
        }
        cache.Sections.push_back(std::unique_ptr<Section>(bi.imageHeaderTable->hashBlockSection));
        }

    /* Step 3: Create and push Meta Header AC AFTER Hash Block 1 */
    if (shouldAuthenticateMetaHeader && bi.metaHdrAuthCtx != nullptr)
    {
        if (bootloaderHybridAuth)
        {
            /* Create HYBRID authentication certificates for Meta Header */
            LOG_INFO("Creating HYBRID Header Authentication Certificates for Meta Header");
            
            /* bi.metaHdrAuthCtx is now a Versal_2vpHybridAuthenticationContext */
                Versal_2vpHybridAuthenticationContext* hybridCtx = 
                    dynamic_cast<Versal_2vpHybridAuthenticationContext*>(bi.metaHdrAuthCtx.get());
            
            if (hybridCtx != nullptr)
            {
                /* The hybrid context will create both ACs during Build */
                bi.headerAC = std::make_unique<Versal_2vpAuthenticationCertificate>(hybridCtx);
                bi.headerAC->Build(bi, cache, bi.imageHeaderTable->section, false, true);
                
                LOG_INFO("Meta Header HYBRID authentication certificates created");
            }
            else
            {
                LOG_WARNING("Expected hybrid auth context for Meta Header but got regular context");
                bi.headerAC = std::make_unique<Versal_2vpAuthenticationCertificate>(bi.metaHdrAuthCtx.get());
                bi.headerAC->Build(bi, cache, bi.imageHeaderTable->section, false, true);
            }
        }
        else
        {
            /* Single signing - create one authentication certificate */
            LOG_INFO("Creating Header Authentication Certificate");
            bi.headerAC = std::make_unique<Versal_2vpAuthenticationCertificate>(bi.metaHdrAuthCtx.get());
            bi.headerAC->Build(bi, cache, bi.imageHeaderTable->section, false, true);
        }
    }
}

/******************************************************************************/
void Versal_2vpPartitionHeaderTable::BuildBootloaderPartitions(BootImage& bi, Binary& cache)
{
    /* versal_2vp PDI order: BH → HB0 → AC(s) → PLM/PMC → Meta Header...
       This function builds only bootloader partitions (PLM/PMC) with their HB0 and AC(s).
       Must be called BEFORE headers are pushed to cache. */
    for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); 
         partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        if ((*partHdr)->IsBootloader())
        {
            (*partHdr)->partition->Build(bi, cache);
        }
    }
}

/******************************************************************************/
void Versal_2vpPartitionHeaderTable::BuildNonBootloaderPartitions(BootImage& bi, Binary& cache)
{
    /* versal_2vp PDI order: ...Meta Header → HB1 → MH AC(s) → PLM loadable partitions
       This function builds only non-bootloader (PLM loadable) partitions.
       Must be called AFTER headers and HB1/MH AC are pushed to cache. */
    for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); 
         partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        if (!(*partHdr)->IsBootloader())
        {
            (*partHdr)->partition->Build(bi, cache);
        }
    }
}

/******************************************************************************/
void Versal_2vpPartitionHeaderTable::ConfigureMetaHdrAuthenticationContext(BootImage & bi)
{
    AuthenticationContext* biAuth = NULL;
    AuthenticationContext* bootloaderAuthCtx = nullptr;
    
    /* Find bootloader's authentication context to inherit keys from */
    for (std::list<ImageHeader*>::iterator image = bi.imageList.begin(); image != bi.imageList.end(); image++)
    {
        if ((*image)->IsBootloader())
        {
            if ((*image)->GetAuthenticationType() == Authentication::None)
        {
            //LOG_ERROR("Bootloader must be authenticated to authenticate Meta Header.");
            }
            else
            {
                bootloaderAuthCtx = (*image)->GetAuthContext();
            }
            break;
        }
    }

    biAuth = (AuthenticationContext*) new Versal_2vpAuthenticationContext(bi.options.bifOptions->metaHdrAttributes.authenticate);
    biAuth->hashType = bi.GetAuthHashAlgo();

    /* Key inheritance priority:
       1. Explicit metaHdrAttributes (highest priority)
       2. Global bifOptions PPK/PSK/SPK/SSK file names
       3. Bootloader's authentication context (for simplified .kmd syntax)
    */
    
    // PPK
    if (bi.bifOptions->metaHdrAttributes.ppk != "")
    {
        biAuth->SetPPKeyFile(bi.bifOptions->metaHdrAttributes.ppk);
    }
    else if (bi.bifOptions->GetPPKFileName() != "")
    {
        biAuth->SetPPKeyFile(bi.bifOptions->GetPPKFileName());
    }
    else if (bootloaderAuthCtx != nullptr && !bootloaderAuthCtx->ppkFile.empty())
    {
        biAuth->SetPPKeyFile(bootloaderAuthCtx->ppkFile);
        LOG_INFO("Meta Header: Inherited PPK from bootloader: %s", bootloaderAuthCtx->ppkFile.c_str());
    }

    // PSK
    if (bi.bifOptions->metaHdrAttributes.psk != "")
    {
        biAuth->SetPSKeyFile(bi.bifOptions->metaHdrAttributes.psk);
    }
    else if (bi.bifOptions->GetPSKFileName() != "")
    {
        biAuth->SetPSKeyFile(bi.bifOptions->GetPSKFileName());
    }
    else if (bootloaderAuthCtx != nullptr && !bootloaderAuthCtx->pskFile.empty())
    {
        biAuth->SetPSKeyFile(bootloaderAuthCtx->pskFile);
        LOG_INFO("Meta Header: Inherited PSK from bootloader: %s", bootloaderAuthCtx->pskFile.c_str());
    }

    // SPK
    if (bi.bifOptions->metaHdrAttributes.spk != "")
    {
        biAuth->SetSPKeyFile(bi.bifOptions->metaHdrAttributes.spk);
    }
    else if (bi.bifOptions->GetSPKFileName() != "")
    {
        biAuth->SetSPKeyFile(bi.bifOptions->GetSPKFileName());
    }
    else if (bootloaderAuthCtx != nullptr && !bootloaderAuthCtx->spkFile.empty())
    {
        biAuth->SetSPKeyFile(bootloaderAuthCtx->spkFile);
        LOG_INFO("Meta Header: Inherited SPK from bootloader: %s", bootloaderAuthCtx->spkFile.c_str());
    }

    // SSK
    if (bi.bifOptions->metaHdrAttributes.ssk != "")
    {
        biAuth->SetSSKeyFile(bi.bifOptions->metaHdrAttributes.ssk);
    }
    else if (bi.bifOptions->GetSSKFileName() != "")
    {
        biAuth->SetSSKeyFile(bi.bifOptions->GetSSKFileName());
    }
    else if (bootloaderAuthCtx != nullptr && !bootloaderAuthCtx->sskFile.empty())
    {
        biAuth->SetSSKeyFile(bootloaderAuthCtx->sskFile);
        LOG_INFO("Meta Header: Inherited SSK from bootloader: %s", bootloaderAuthCtx->sskFile.c_str());
    }

    if (bi.bifOptions->metaHdrAttributes.spkSignature != "")
    {
        biAuth->SetSPKSignatureFile(bi.bifOptions->metaHdrAttributes.spkSignature);
    }
    biAuth->spkIdentification = bi.bifOptions->metaHdrAttributes.spkRevokeId;
    biAuth->lmsOnly = bi.bifOptions->metaHdrAttributes.lmsOnly;

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

    ImageHeaderTable* iht = bi.imageHeaderTable.get();
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
    bi.metaHdrAuthCtx = std::make_unique<Versal_2vpAuthenticationContext>(biAuth, bi.bifOptions->metaHdrAttributes.authenticate);
    
    if (bi.bifOptions->metaHdrAttributes.presign != "")
    {
        bi.metaHdrAuthCtx->SetPresignFile(bi.bifOptions->metaHdrAttributes.presign);
    }
}

/******************************************************************************/
void Versal_2vpPartitionHeaderTable::ConfigureMetaHdrHybridAuthenticationContext(BootImage & bi, ImageHeader* bootloaderImage)
{
    /* Get the bootloader's authentication context - it should be a hybrid context */
    AuthenticationContext* bootloaderAuthCtx = nullptr;
    Versal_2vpHybridAuthenticationContext* bootloaderHybridCtx = nullptr;
    
    if (bootloaderImage != nullptr)
    {
        bootloaderAuthCtx = bootloaderImage->GetAuthContext();
        bootloaderHybridCtx = dynamic_cast<Versal_2vpHybridAuthenticationContext*>(bootloaderAuthCtx);
    }
    
    /* Get hybrid algorithms from bootloader's hybrid context */
    std::vector<Authentication::Type> hybridAlgorithms;
    if (bootloaderHybridCtx != nullptr)
    {
        hybridAlgorithms = bootloaderHybridCtx->GetHybridAlgorithms();
    }
    
    if (hybridAlgorithms.size() < 2)
    {
        LOG_ERROR("Hybrid authentication requires two algorithms, but bootloader context only has %zu", hybridAlgorithms.size());
        return;
    }
    
    LOG_INFO("Creating Meta Header HYBRID authentication context with algorithms: %d, %d", 
             (int)hybridAlgorithms[0], (int)hybridAlgorithms[1]);
    
    /* Create the hybrid authentication context for Meta Header */
    Versal_2vpHybridAuthenticationContext* hybridAuthCtx = 
        new Versal_2vpHybridAuthenticationContext(bootloaderAuthCtx, hybridAlgorithms);
    
    hybridAuthCtx->hashType = bi.GetAuthHashAlgo();
    
    /* Copy key file paths from bootloader's authentication context or global bifOptions */
    /* Algorithm 1 keys (typically classical like ECDSA/RSA) - from bootloader or bifOptions */
    if (bootloaderAuthCtx != nullptr && !bootloaderAuthCtx->ppkFile.empty())
        hybridAuthCtx->SetPPKeyFile(bootloaderAuthCtx->ppkFile);
    else if (bi.bifOptions->GetPPKFileName() != "")
        hybridAuthCtx->SetPPKeyFile(bi.bifOptions->GetPPKFileName());
        
    if (bootloaderAuthCtx != nullptr && !bootloaderAuthCtx->pskFile.empty())
        hybridAuthCtx->SetPSKeyFile(bootloaderAuthCtx->pskFile);
    else if (bi.bifOptions->GetPSKFileName() != "")
        hybridAuthCtx->SetPSKeyFile(bi.bifOptions->GetPSKFileName());
        
    if (bootloaderAuthCtx != nullptr && !bootloaderAuthCtx->spkFile.empty())
        hybridAuthCtx->SetSPKeyFile(bootloaderAuthCtx->spkFile);
    else if (bi.bifOptions->GetSPKFileName() != "")
        hybridAuthCtx->SetSPKeyFile(bi.bifOptions->GetSPKFileName());
        
    if (bootloaderAuthCtx != nullptr && !bootloaderAuthCtx->sskFile.empty())
        hybridAuthCtx->SetSSKeyFile(bootloaderAuthCtx->sskFile);
    else if (bi.bifOptions->GetSSKFileName() != "")
        hybridAuthCtx->SetSSKeyFile(bi.bifOptions->GetSSKFileName());
    
    /* Algorithm 2 keys (typically PQC like LMS/HSS) - get from bifOptions directly 
     * Note: bootloaderHybridCtx->GetSSKFile1() might be empty at this point because
     * LoadSecondAlgorithmKeys() hasn't been called yet for the bootloader.
     * So we always use bifOptions as the source of truth. */
    {
        /* Get key files from bifOptions for algorithm 2 keys */
        std::string ppk1 = bi.bifOptions->GetPPKFileName1();
        std::string psk1 = bi.bifOptions->GetPSKFileName1();
        std::string spk1 = bi.bifOptions->GetSPKFileName1();
        std::string ssk1 = bi.bifOptions->GetSSKFileName1();
        
        /* If simplified syntax (pkfile1/skfile1), derive the key files */
        if (ppk1.empty() && !bi.bifOptions->GetPKFileName1().empty())
        {
            ppk1 = bi.bifOptions->GetPKFileName1();
            LOG_INFO("Using pkfile1 for ppkFile1: %s", ppk1.c_str());
        }
        if (spk1.empty() && !bi.bifOptions->GetSKFileName1().empty())
        {
            spk1 = bi.bifOptions->GetSKFileName1();
            LOG_INFO("Using skfile1 for spkFile1: %s", spk1.c_str());
        }
        
        /* For LMS/HSS, derive private key file paths from public key file paths */
        if (psk1.empty() && !ppk1.empty())
        {
            /* Check if this looks like an LMS public key file (.lms.pub or .hss.pub) */
            size_t pubPos = ppk1.rfind(".pub");
            if (pubPos != std::string::npos)
            {
                psk1 = ppk1.substr(0, pubPos) + ".prv";
                LOG_INFO("Derived pskFile1 from ppkFile1: %s -> %s", ppk1.c_str(), psk1.c_str());
            }
        }
        if (ssk1.empty() && !spk1.empty())
        {
            /* Check if this looks like an LMS public key file (.lms.pub or .hss.pub) */
            size_t pubPos = spk1.rfind(".pub");
            if (pubPos != std::string::npos)
            {
                ssk1 = spk1.substr(0, pubPos) + ".prv";
                LOG_INFO("Derived sskFile1 from spkFile1: %s -> %s", spk1.c_str(), ssk1.c_str());
            }
        }
        
        hybridAuthCtx->SetPPKFile1(ppk1);
        hybridAuthCtx->SetPSKFile1(psk1);
        hybridAuthCtx->SetSPKFile1(spk1);
        hybridAuthCtx->SetSSKFile1(ssk1);
        
        /* Copy lmsOnly flag from bootloader if available */
        if (bootloaderHybridCtx != nullptr)
        {
            hybridAuthCtx->SetLmsOnlySecondAlg(bootloaderHybridCtx->GetLmsOnlySecondAlg());
        }
    }
    
    /* Copy other settings */
    if (bootloaderAuthCtx != nullptr)
    {
        hybridAuthCtx->spkIdentification = bootloaderAuthCtx->spkIdentification;
        hybridAuthCtx->lmsOnly = bootloaderAuthCtx->lmsOnly;
    }
    
    /* Resize sections for authentication certificates */
    AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
    if (hybridAlgorithms[0] == Authentication::ECDSA)
    {
        AuthenticationContext::SetAuthenticationKeyLength(EC_P384_KEY_LENGTH);
    }
    
    ImageHeaderTable* iht = bi.imageHeaderTable.get();
    hybridAuthCtx->ResizeIfNecessary(iht->section);
    for (std::list<ImageHeader*>::iterator ih = bi.imageList.begin(); ih != bi.imageList.end(); ih++)
    {
        hybridAuthCtx->ResizeIfNecessary((*ih)->section);
    }
    for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        hybridAuthCtx->ResizeIfNecessary((*partHdr)->section);
    }
    
    /* Set as Meta Header auth context */
    bi.metaHdrAuthCtx = std::unique_ptr<AuthenticationContext>(hybridAuthCtx);
    
    if (bi.bifOptions->metaHdrAttributes.presign != "")
    {
        bi.metaHdrAuthCtx->SetPresignFile(bi.bifOptions->metaHdrAttributes.presign);
    }
    
    LOG_INFO("Meta Header HYBRID authentication context created successfully");
}

#if 0
/******************************************************************************/
void Versal_2vpPartitionHeaderTable::UpdateAtfHandoffParams(BootImage & bi)
{
    memset(&atf_handoff_params, 0, sizeof(atf_handoff_params_struct));
    atf_handoff_params.magic[0] = 'X';
    atf_handoff_params.magic[1] = 'L';
    atf_handoff_params.magic[2] = 'N';
    atf_handoff_params.magic[3] = 'X';

    atf_handoff_params.num_entries = 0;

    for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        DestinationCPU::Type core = (*partHdr)->imageHeader->GetDestCpu();
        ExceptionLevel::Type exceptionLevel = (*partHdr)->imageHeader->GetExceptionLevel();
        TrustZone::Type trustzone = (*partHdr)->imageHeader->GetTrustZone();

        //if((core = a72 - 0 or a72 - 1) && ((EL = EL2 && trustzone = non - secure) || (EL = EL1 && trustzone = secure) || (EL = EL1 && trustzone = non - secure)))
        bool valid_core = ((core == DestinationCPU::A53_0) || (core == DestinationCPU::A53_1));
        bool valid_entry1 = ((exceptionLevel == ExceptionLevel::EL2) && (trustzone == TrustZone::NonSecure));
        bool valid_entry2 = (exceptionLevel == ExceptionLevel::EL1);

        if (valid_core && (valid_entry1 || valid_entry2))
        {
            atf_handoff_params.handoff_partition[atf_handoff_params.num_entries].entry_point = (*partHdr)->execAddress;

            uint8_t execState = (*partHdr)->execState;
            uint8_t endian = ((*partHdr)->elfEndianess == Endianness::BigEndian) ? 1 : 0;
            uint8_t destCpu = (core == DestinationCPU::A53_0) ? 0 : 1;

            atf_handoff_params.handoff_partition[atf_handoff_params.num_entries].flags = (execState << atfHandoffExecStateShift) |
                (endian << atfHandoffEndiannessShift) |
                (trustzone << atfHandoffTrustzoneShift) |
                (exceptionLevel << atfHandoffExceptionLevelShift) |
                (destCpu << atfHandoffDestCpuShift);

            atf_handoff_params.num_entries++;
        }
    }

    for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        if ((*partHdr)->update_atf_handoff_params)
        {
            memcpy((*partHdr)->partition->section->Data + (*partHdr)->atf_handoff_params_offset, &atf_handoff_params, sizeof(atf_handoff_params_struct));
        }
    }
}
#endif

/******************************************************************************/
void PopulateBootloaderHashBlockForEncryptionOnly(BootImage& bi, Versal_2vpBootImage& vbi, uint8_t* metaheaderHash)
{
    if (!vbi.bootloaderHashBlockSection) return;

    for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        if ((*partHdr)->IsBootloader())
        {
            bool is_auth = ((*partHdr)->imageHeader->GetAuthenticationType() != Authentication::None);
            bool is_enc = ((*partHdr)->imageHeader->GetEncryptContext()->Type() != Encryption::None);

            if (is_enc && !is_auth)
            {
                uint8_t* hashBlock = vbi.bootloaderHashBlockSection->Data.get();
                const size_t hashLength = bi.hash->GetHashLength();
                size_t offset = 0;

                // 1. Populate BH Hash (Index 0)
                uint32_t bhIndex = HASH_BLOCK_BH_HASH_INDEX;
                memcpy(hashBlock + offset, &bhIndex, sizeof(uint32_t));
                offset += sizeof(uint32_t);

                uint8_t* tmpBh = bi.bootHeader->section->Data.get() + 0x10;
                uint8_t* bhHash = new uint8_t[hashLength];
                bi.hash->CalculateVersalHash(true, tmpBh,
                    bi.bootHeader->GetBootHeaderSize() - sizeof(Versal_2vpSmapWidthTable), bhHash);
                memcpy(hashBlock + offset, bhHash, hashLength);
                offset += hashLength;
                delete[] bhHash;

                // 2. Skip PLM hash (index 1)
                offset += sizeof(uint32_t) + hashLength;
                // 3. Skip PMC hash (index 2)
                offset += sizeof(uint32_t) + hashLength;

                // 4. Populate Metaheader Hash (Index 3)
                uint32_t metaIndex = HASH_BLOCK_HASHBLOCK1_HASH_INDEX;
                memcpy(hashBlock + offset, &metaIndex, sizeof(uint32_t));
                offset += sizeof(uint32_t);
                memcpy(hashBlock + offset, metaheaderHash, hashLength);
                offset += hashLength;

                // Pad remaining bytes in hashblock0 with zeros
                if (offset < vbi.bootloaderHashBlockSection->Length) {
                    memset(hashBlock + offset, 0, vbi.bootloaderHashBlockSection->Length - offset);
                }

                LOG_INFO("Complete hashblock0 populated for encryption-only bootloader:");
                LOG_DUMP_BYTES(hashBlock, vbi.bootloaderHashBlockSection->Length);
            }
            break;
        }
    }
}

/******************************************************************************/
void Versal_2vpPartitionHeaderTable::Link(BootImage & bi)
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
        bi.imageHeaderTable->SetTotalMetaHdrLength(bi.imageHeaderTable->metaHeaderLength);
        bi.imageHeaderTable->SetChecksum();
    }

    if (bi.options.bifOptions->GetHeaderEncyption())
    {
        LOG_INFO("Encrypting the Meta Header");
        EncryptionContext* encryptCtx = bi.imageHeaderTable->GetEncryptContext();

        bi.imageHeaderTable->SetTotalMetaHdrLength(bi.encryptedHeaders->Length);
        bi.imageHeaderTable->SetChecksum();

        encryptCtx->Process(bi);
        bi.imageHeaderTable->metaHeaderLength = bi.imageHeaderTable->GetTotalMetaHdrLength();
    }

    if (bi.imageHeaderTable->hashBlockSection != NULL)
    {
        /* Calculate Headers Hash */
        uint8_t* sha_hash = new uint8_t[bi.hash->GetHashLength()];
        std::list<Section*> sections;
        Section* headers = NULL;
        size_t size = 0;
        ImageHeaderTable* iHT = bi.imageHeaderTable.get();

        /* Header section */
        sections.push_back(iHT->section);
        size += iHT->section->Length;

        if (bi.options.bifOptions->GetHeaderEncyption())
        {
            sections.push_back(bi.encryptedHeaders);
            size += bi.encryptedHeaders->Length;
        }
        else
        {
            /* Append all image and partition headers */
            for (std::list<SubSysImageHeader*>::iterator image = bi.subSysImageList.begin(); image != bi.subSysImageList.end(); image++)
            {
                sections.push_back((*image)->section);
                size += sizeof(VersalImageHeaderStructure);
            }

            for (std::list<PartitionHeader*>::iterator part = bi.partitionHeaderList.begin(); part != bi.partitionHeaderList.end(); part++)
            {
                sections.push_back((*part)->section);
                size += sizeof(Versal_2vpPartitionHeaderTableStructure);
            }
        }

        /* Create one new combined section with all the appended sections above */
        headers = new Section("Headers", size);
        //headers->Address = iHT->section->Address; // not really needed, but useful for debug.
        memset(headers->Data.get(), bi.options.GetOutputFillByte(), headers->Length);

        Binary::Address_t start = sections.front()->Address;
        for (std::list<Section*>::iterator i = sections.begin(); i != sections.end(); i++)
        {
            Section& section(*(*i));
            int offset = section.Address - start;
            memcpy(headers->Data.get() + offset, section.Data.get(), section.Length);
        }
        /* Replace sections list with the combined new section */
        sections.clear();
        sections.push_back(headers);

        bi.hash->CalculateVersalHash(true, headers->Data.get(), size, sha_hash);

        if (headers != NULL)
        {
            delete headers;
        }
        

        /* Copy Meta Header Hash into Hash Block 1 */
        memset(bi.imageHeaderTable->hashBlockSection->Data.get(), 0, bi.imageHeaderTable->hashBlockSectionLength);
        memcpy(bi.imageHeaderTable->hashBlockSection->Data.get() + HASH_BLOCK_INDEX_BYTES, sha_hash, bi.hash->GetHashLength());
#ifdef DEBUG
        LOG_TRACE("Meta Header Length %d", bi.imageHeaderTable->metaHeaderLength);
        LOG_TRACE("Meta Header Data");
        LOG_DUMP_BYTES(headers->Data.get(), bi.imageHeaderTable->metaHeaderLength);
        LOG_TRACE("Meta Header Hash - Copied to Hash Block 1");
        LOG_DUMP_BYTES(sha_hash, bi.hash->GetHashLength());
#endif
        delete[] sha_hash;

        /* Copy Partition Hashes into Hash Block 1.
           include ONLY meta-header-grouped partitions
           (hashNumMap.second == 0), packed sequentially. Owner-grouped
           partitions go into their owner's HBn. */
        {
            size_t slot = 1;   /* slot 0 holds the meta-header hash */
            uint32_t entrySize = HASH_BLOCK_INDEX_BYTES + bi.hash->GetHashLength();
            for (size_t i = 1; i < bi.hashTable.size(); i++)
            {
                uint32_t partition_num = bi.hashTable[i].first;
                bool inMetaGroup = bi.hashNumMap.empty();
                for (size_t k = 0; k < bi.hashNumMap.size(); k++)
                {
                    if (bi.hashNumMap[k].first == partition_num &&
                        bi.hashNumMap[k].second == 0)
                    {
                        inMetaGroup = true;
                        break;
                    }
                }
                if (!inMetaGroup)
                    continue;
                memcpy(bi.imageHeaderTable->hashBlockSection->Data.get() + (slot * entrySize),
                       &partition_num, HASH_BLOCK_INDEX_BYTES);
                memcpy(bi.imageHeaderTable->hashBlockSection->Data.get() + HASH_BLOCK_INDEX_BYTES + (slot * entrySize),
                       bi.hashTable[i].second.get(), bi.hash->GetHashLength());
                slot++;
            }
        }

        /* AAD for HB 1 */
        if (bi.options.bifOptions->metaHdrAttributes.encrypt != Encryption::None && bi.options.bifOptions->metaHdrAttributes.authenticate == Authentication::None)
        {
            LOG_TRACE("Performing AAD on Hash Block 1");
            EncryptionContext* encryptCtx = bi.imageHeaderTable->GetEncryptContext();
            encryptCtx->AesGcm256HashBlockEncrypt(bi.options, bi.imageHeaderTable->hashBlockSection->Data.get(),
                bi.imageHeaderTable->hashBlockSectionLength, bi.imageHeaderTable->hashBlockSection->Data.get() + bi.imageHeaderTable->hashBlockSectionLength, 1);
#ifdef DEBUG
            LOG_TRACE("Hash Block 1 AAD");
            LOG_DUMP_BYTES(bi.imageHeaderTable->hashBlockSection->Data.get() + bi.imageHeaderTable->hashBlockSectionLength, AES_GCM_TAG_SZ);
#endif
        }
    }

    {
        /* Calculate Hash Block 1 Hash */
        uint8_t* sha_hash = new uint8_t[bi.hash->GetHashLength()];
        bi.hash->CalculateVersalHash(true, bi.imageHeaderTable->hashBlockSection->Data.get(), bi.imageHeaderTable->hashBlockSectionLength, sha_hash);
#ifdef DEBUG
        LOG_TRACE("Hash Block 1 Data");
        LOG_DUMP_BYTES(bi.imageHeaderTable->hashBlockSection->Data.get(), bi.imageHeaderTable->hashBlockSectionLength);
        LOG_TRACE("Hash Block 1 Hash - Copied to Hash Block 0");
        LOG_DUMP_BYTES(sha_hash, bi.hash->GetHashLength());
#endif
        /* Copy Hash Block 1 Hash into Hash Block 0 */

        // Populate BH and metaheader hash for encryption-only bootloader flows
        Versal_2vpBootImage& vbi = static_cast<Versal_2vpBootImage&>(bi);
        PopulateBootloaderHashBlockForEncryptionOnly(bi, vbi, sha_hash);
       
        for (SectionList::iterator i = bi.cache->Sections.begin(); i != bi.cache->Sections.end(); i++)
        {
            Section& section(**i);
            if (section.isBootloader)
            {
                 // Legacy flow: embed hash directly in PLM section for bootloader, only if NOT secure
                bool is_auth = false;
                bool is_enc = false;
                PartitionHeader* partHdr = nullptr;
                for (std::list<PartitionHeader*>::iterator it = bi.partitionHeaderList.begin(); it != bi.partitionHeaderList.end(); ++it) {
                    if ((*it)->IsBootloader()) { partHdr = *it; break; }
                }
                if (partHdr) {
                    is_auth = partHdr->imageHeader->GetAuthenticationType() != Authentication::None;
                    is_enc = partHdr->imageHeader->GetEncryptContext()->Type() != Encryption::None;
                }
                if (!(is_auth || is_enc)) {   // TODO : cross check if has to handle if bootloader is non secure
                    uint32_t hashIndex = HASH_BLOCK_HASHBLOCK1_HASH_INDEX;
                    memcpy(section.Data.get() + (3 * (HASH_BLOCK_INDEX_BYTES + bi.hash->GetHashLength())), &hashIndex, HASH_BLOCK_INDEX_BYTES);
                    memcpy(section.Data.get() + HASH_BLOCK_INDEX_BYTES + (3 * (HASH_BLOCK_INDEX_BYTES + bi.hash->GetHashLength())), sha_hash, bi.hash->GetHashLength());
                } else {
                    LOG_INFO("Bootloader secure flow detected - skipping legacy PLM hash embedding in section");
                }
                break;
            }
        }
        delete[] sha_hash;

        for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
        {
            if ((*partHdr)->IsBootloader())
            {
                if ((*partHdr)->imageHeader->GetEncryptContext()->Type() != Encryption::None && (*partHdr)->imageHeader->GetAuthContext()->authAlgorithm->Type() == Authentication::None)
                {
                  //   LOG_TRACE("Hash Block 0 (encryption-only, no GCM tag as per requirement)");
                  //  LOG_DUMP_BYTES((*partHdr)->partition->section->Data, bi.hashBlockLength);

                    //No GCM tag for hashblock0 as per your requirement for versal_2vp
                  //  (*partHdr)->imageHeader->GetEncryptContext()->AesGcm256HashBlockEncrypt(bi.options, (*partHdr)->partition->section->Data,
                  //      bi.hashBlockLength, (*partHdr)->partition->section->Data + bi.hashBlockLength, 2);

                 //   LOG_TRACE("GCM Tag + Hash Block 0");
                  //  LOG_DUMP_BYTES((*partHdr)->partition->section->Data, bi.hashBlockLength + AES_GCM_TAG_SZ);
                }
                break;
            }
        }
    }

    if (bi.bifOptions->GetHeaderAC())
    {
        bi.headerAC->Link(bi, bi.imageHeaderTable->section);
    }

    bi.bifOptions->CheckForSameKeyandKeySrcPair(bi.aesKeyandKeySrc);
}
