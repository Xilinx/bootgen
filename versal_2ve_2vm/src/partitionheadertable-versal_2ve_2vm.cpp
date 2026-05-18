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
#include "partitionheadertable-versal_2ve_2vm.h"
#include "bootheader-versal_2ve_2vm.h"
#include "authentication-versal_2ve_2vm.h"

extern "C" {
#include "../../lms-hash-sigs/lms-utils.h"
#include "../../lms-hash-sigs/hss_verify.h"
};
/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
Versal_2ve_2vmPartitionHeader::Versal_2ve_2vmPartitionHeader(ImageHeader* imageheader, int index)
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
    auto temp_section = std::make_unique<Section>(name, sizeof(Versal_2ve_2vmPartitionHeaderTableStructure));
    section = temp_section.release();  // Transfer ownership to raw pointer member
    memset(section->Data.get(), 0, section->Length);

    pHTable = (Versal_2ve_2vmPartitionHeaderTableStructure*)section->Data.get();
    pHTable->partitionRevokeId = imageHeader->GetPartitionRevocationId();
}

/******************************************************************************/
Versal_2ve_2vmPartitionHeader::~Versal_2ve_2vmPartitionHeader()
{
    if (section != NULL)
    {
    }
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::ReadHeader(std::ifstream& ifs)
{
    ifs.read((char*)pHTable, sizeof(Versal_2ve_2vmPartitionHeaderTableStructure));
    uint32_t checksum = ComputeWordChecksum(pHTable, sizeof(Versal_2ve_2vmPartitionHeaderTableStructure) - sizeof(uint32_t));

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
void Versal_2ve_2vmPartitionHeader::ReadData(std::ifstream& ifs)
{
    uint32_t dataLen = GetTotalPartitionLength();
    std::string partName = imageHeader->GetName() + "_" + std::to_string(partitionUid) + StringUtils::Format(".%d", index);
    // BUGFIX: Use separate variable for data section - don't overwrite header section pointer!
    auto temp_dsection = std::make_unique<Section>(partName, dataLen);
    Section* dsection = temp_dsection.release();  // Transfer ownership to raw pointer member
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
    partition = std::make_unique<Versal_2ve_2vmPartition>(this, dsection);

    static uint8_t encryptionHeader[] =
    {
        0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBB,0x00,0x00,0x00,
        0x44,0x00,0x22,0x11,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x66,0x55,0x99,0xAA
    };
    preencrypted = (memcmp(dsection->Data.get(), encryptionHeader, sizeof(encryptionHeader)) == 0);
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::Build(BootImage& bi, Binary& cache)
{
    if (section != NULL)
    {
        bi.headers.push_back(section);
        bi.imageHeaderTable->metaHeaderLength += section->Length;
    }
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::Link(BootImage &bi, PartitionHeader* next_part_hdr)
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

    //SetHashBlockWordOffset((uint32_t)partition->section->Address);
    SetHashBlockWordOffset(bi.imageHeaderTable->hashBlockSection->WordAddress());
    SetHashBlockLength(0);

    SetPartitionAttributes();
    SetChecksumOffset();
    SetPartitionId();
    if (!preencrypted)
    {
        SetPartitionSecureHdrIv(partitionSecHdrIv.get());
        SetPartitionKeySrc(partitionKeySrc, bi.bifOptions);
        SetPartitionGreyOrBlackIv(kekIvFile);
    }
    SetReserved();  // Clears ac_offset to 0
    
    if (!isBootloader && ((Versal_2ve_2vmPartition*)partition.get())->hashBlockSection != NULL)
    {
        SetAuthCertificateOffset();
        pHTable->authHeader1 = ac.front()->AuthContext->authAlgorithm->GetAuthHeader();
        uint32_t hashBlockLength = ((Versal_2ve_2vmPartition*)partition.get())->hashBlockSection->Length;
        if (ac.front()->AuthContext->authAlgorithm->Type() != Authentication::None)
        {
            hashBlockLength -= ac.front()->AuthContext->GetTotalHashBlockSignSize();
        }
        
        if (imageHeader->GetEncryptContext()->Type() != Encryption::None && ac.front()->AuthContext->authAlgorithm->Type() == Authentication::None)
        {
            hashBlockLength -= AES_GCM_TAG_SZ;
        }
        pHTable->hashBlockLength1 = hashBlockLength / 4;
        pHTable->hashBlockWordOffset = ((Versal_2ve_2vmPartition*)partition.get())->hashBlockSection->WordAddress();

        if (ac.front()->AuthContext->authAlgorithm->Type() == Authentication::RSA)
        {
            pHTable->totalppkkSize1 = VERSAL_ACKEY_STRUCT_SIZE + TELLURIDE_RSA_AC_PPK_SPK_ALIGNMENT;
            pHTable->actualppkSize1 = VERSAL_ACKEY_STRUCT_SIZE;
            pHTable->totalHashBlockSignatureSize1 = pHTable->actualSignatureSize1 = ac.front()->AuthContext->GetSignatureLength();
        }
        else if (ac.front()->AuthContext->authAlgorithm->Type() == Authentication::ECDSA)
        {
            pHTable->totalppkkSize1 = pHTable->actualppkSize1 = 2 * EC_P384_KEY_LENGTH;
            pHTable->totalHashBlockSignatureSize1 = pHTable->actualSignatureSize1 = ac.front()->AuthContext->GetSignatureLength();
        }
        else if (ac.front()->AuthContext->authAlgorithm->Type() == Authentication::ECDSAp521)
        {
            pHTable->actualppkSize1 = EC_P521_KEY_LENGTH2 * 2;
            pHTable->totalppkkSize1 = EC_P521_KEY_LENGTH2 * 2 + PADDING_16B(pHTable->actualppkSize1);
            pHTable->actualSignatureSize1 = ac.front()->AuthContext->GetSignatureLength();
            pHTable->totalHashBlockSignatureSize1 = pHTable->actualSignatureSize1 + PADDING_16B(pHTable->actualSignatureSize1);
            LOG_TRACE("ecdsa521");
        }
        else if (ac.front()->AuthContext->authAlgorithm->Type() == Authentication::LMS_SHA2_256 ||
                 ac.front()->AuthContext->authAlgorithm->Type() == Authentication::LMS_SHAKE256)
        {
            pHTable->authHeader1 = ac.front()->AuthContext->authAlgorithm->GetAuthHeader(ac.front()->AuthContext->lmsOnly);
            size_t actualLmsPpkSize1 = GetLmsPublicKeyLength(ac.front()->AuthContext->ppkFile.c_str(), ac.front()->AuthContext->lmsOnly);
            size_t actualHashBlockSignatureLength = GetLmsSignLength(ac.front()->AuthContext->sskFile.c_str(), ac.front()->AuthContext->lmsOnly);      

            pHTable->totalppkkSize1 = actualLmsPpkSize1 + PADDING_16B(actualLmsPpkSize1);
            pHTable->actualppkSize1 = actualLmsPpkSize1;
            pHTable->totalHashBlockSignatureSize1 = actualHashBlockSignatureLength + PADDING_16B(actualHashBlockSignatureLength);
            pHTable->actualSignatureSize1 = actualHashBlockSignatureLength;
        }       
    }
    
    SetChecksum();
    
 
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetPartitionSecureHdrIv(uint8_t* iv)
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
void Versal_2ve_2vmPartitionHeader::SetPartitionKeySrc(KeySource::Type keyType, BifOptions* bifOptions)
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
void Versal_2ve_2vmPartitionHeader::SetPartitionGreyOrBlackIv(std::string ivFile)
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

    memcpy(&pHTable->partitionGreyOrBlackIV, ivData.get(), IV_LENGTH * 4);
}


/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetEncryptedPartitionLength(uint32_t len)
{
    pHTable->encryptedPartitionLength = len / sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetUnencryptedPartitionLength(uint32_t len)
{
    pHTable->unencryptedPartitionLength = len / sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetTotalPartitionLength(uint32_t len)
{
    pHTable->totalPartitionLength = len / sizeof(uint32_t);

}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetNextPartitionHeaderOffset(uint32_t addr)
{
	pHTable->nextPartitionHeaderOffset = addr / sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetExecAddress(uint64_t addr)
{
    pHTable->destinationExecAddress = addr;
}

/******************************************************************************/
#define XLOADER_R52_0A_TCMA_BASE_ADDR 	0xEBA00000 /* R52_0A TCMA base address */
#define XLOADER_R52_1A_TCMA_BASE_ADDR 	0xEBA40000 /* R52_1A TCMA base address */
#define XLOADER_R52_0B_TCMA_BASE_ADDR 	0xEBA80000 /* R52_0B TCMA base address */
#define XLOADER_R52_1B_TCMA_BASE_ADDR 	0xEBAC0000 /* R52_1B TCMA base address */
#define XLOADER_R52_TCM_CLUSTER_OFFSET	0x00080000 /* R52_TCM TCM cluster offset */
#define XLOADER_R52_TCMA_LOAD_ADDRESS	0x0        /* R52 TCMA load address */
#define XLOADER_R52_TCM_TOTAL_LENGTH	0x30000    /* R52 TCMA total length */

static uint64_t GetR52LoadAddr(DestinationCPU::Type dstCpu, uint32_t dstCluster, uint64_t loadAddr, uint32_t len)
{
    uint64_t address = loadAddr;

    if ((dstCpu == DestinationCPU::R5_0) &&
        ((address < (XLOADER_R52_TCMA_LOAD_ADDRESS +
            XLOADER_R52_TCM_TOTAL_LENGTH)))) {
        if (((address % XLOADER_R52_TCM_TOTAL_LENGTH) + len) >
            XLOADER_R52_TCM_TOTAL_LENGTH) {
            //Status = XPlmi_UpdateStatus(XLOADER_ERR_TCM_ADDR_OUTOF_RANGE, 0);
        }

        address += XLOADER_R52_0A_TCMA_BASE_ADDR +
            (dstCluster * XLOADER_R52_TCM_CLUSTER_OFFSET);
    }
    else if ((dstCpu == DestinationCPU::R5_1) &&
        ((address < (XLOADER_R52_TCMA_LOAD_ADDRESS +
            XLOADER_R52_TCM_TOTAL_LENGTH)))) {
        if (((address % XLOADER_R52_TCM_TOTAL_LENGTH) + len) >
            XLOADER_R52_TCM_TOTAL_LENGTH) {
            //Status = XPlmi_UpdateStatus(XLOADER_ERR_TCM_ADDR_OUTOF_RANGE, 0);
        }

        address += XLOADER_R52_1A_TCMA_BASE_ADDR +
            (dstCluster * XLOADER_R52_TCM_CLUSTER_OFFSET);
    }
    else {
        /* Do nothing */
    }

    /* Update the load address */
    return address;
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetLoadAddress(uint64_t addr, bool versalNetSeries)
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
            addr = GetR52LoadAddr(imageHeader->GetDestCpu(), imageHeader->GetClusterNum(), addr, GetEncryptedPartitionLength());
    }

    pHTable->destinationLoadAddress = addr;
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetPartitionWordOffset(uint32_t addr)
{
    if (presigned)
    {
        pHTable->partitionWordOffset = (addr+sizeof(AuthCertificate4096Sha3PaddingHBStructure)) / sizeof(uint32_t);
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
void Versal_2ve_2vmPartitionHeader::SetHashBlockWordOffset(uint32_t addr)
{
    pHTable->hashBlockWordOffset = addr / sizeof(uint32_t);
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetHashBlockLength(uint32_t length)
{
    pHTable->hashBlockLength1 = length;
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetAuthHeader1(uint32_t value)
{
    pHTable->authHeader1 = value;
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetTotalPpkkSize1(uint32_t size)
{
    pHTable->totalppkkSize1 = size;
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetActualPpkSize1(uint32_t size)
{
    pHTable->actualppkSize1 = size;
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetTotalHashBlockSignatureSize1(uint32_t size)
{
    pHTable->totalHashBlockSignatureSize1 = size;
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetActualSignatureSize1(uint32_t size)
{
    pHTable->actualSignatureSize1 = size;
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetPartitionAttributes(void)
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
        // checksumType = imageHeader->GetChecksumContext()->Type();
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

    pHTable->partitionAttributes = (trustzone << v2phtTrustzoneShift) |
                                   (exceptionLevel << v2phtExceptionLevelShift) |
                                   (execState << v2phtExecStateShift) |
                                   (destCpu << v2phtDestCpuShift) |
                                   (checksumType << v2phtChecksumTypeShift) |
                                   (pufHdLoc << v2phtPufHDLocationShift) |
                                   (ownerType <<v2phtPartitionOwnerShift) |
                                   (endian << v2phtEndiannessShift) |
                                   (partitionType << v2phtPartitionTypeShift) |
                                   (hivec << v2phtHivecShift) |
                                   (dpaCM << v2phtDpaCMShift) |
                                   (cluster << v2phtClusterShift) |
                                   (lockstep << v2phtlockStepShift) |
                                   (tcmBoot << v2phtTcmBootShift);
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetSectionCount(uint32_t cnt)
{
    pHTable->dataSectionCount = cnt;
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetChecksumOffset(void)
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
void Versal_2ve_2vmPartitionHeader::SetAuthCertificateOffset(void)
{
    LOG_TRACE("authcert");
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
        auto auth = std::make_unique<Versal_2ve_2vmAuthenticationContext>(Authentication::RSA);
        pHTable->authCertificateOffset = (uint32_t)((partition->section->Address + partition->section->Length - auth->GetCertificateSize()) / sizeof(uint32_t));
    }
    else if (imageHeader->GetAuthenticationType() == Authentication::ECDSA)
    {
        AuthenticationContext::SetAuthenticationKeyLength(EC_P384_KEY_LENGTH);
        auto auth = std::make_unique<Versal_2ve_2vmAuthenticationContext>(Authentication::ECDSA);
        pHTable->authCertificateOffset = (uint32_t)((partition->section->Address + partition->section->Length - auth->GetCertificateSize()) / sizeof(uint32_t));
        LOG_TRACE("ECDSA");
    }
    else if (imageHeader->GetAuthenticationType() == Authentication::ECDSAp521)
    {
        AuthenticationContext::SetAuthenticationKeyLength(EC_P521_KEY_LENGTH2);
		auto auth = std::make_unique<Versal_2ve_2vmAuthenticationContext>(Authentication::ECDSAp521);
        pHTable->authCertificateOffset = (uint32_t)((partition->section->Address + partition->section->Length - auth->GetCertificateSize()) / sizeof(uint32_t));
        LOG_TRACE("ECDSA521");
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
void Versal_2ve_2vmPartitionHeader::SetReserved(void)
{
    pHTable->authCertificateOffset = 0;                 // 0x34
    pHTable->measuredBootAddress = 0;                   // 0x58
    pHTable->authHeader1 = 0;                           // 0x5C
    pHTable->totalppkkSize1 = 0;                        // 0x64
    pHTable->actualppkSize1 = 0;                        // 0x6C
    pHTable->totalHashBlockSignatureSize1 = 0;          // 0x70
    pHTable->actualSignatureSize1 = 0;                  // 0x74
    pHTable->reserved = 0;                              // 0x78

    //memset(&pHTable->reserved, 0x00, MAX_PHT_RESERVED_VERSAL * sizeof(uint32_t));
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetPartitionId()
{
    pHTable->puid = partitionUid;
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetChecksum(void)
{
    pHTable->pHChecksum = ComputeWordChecksum(pHTable, sizeof(Versal_2ve_2vmPartitionHeaderTableStructure) - sizeof(uint32_t));
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::RealignSectionDataPtr(void)
{
    pHTable = (Versal_2ve_2vmPartitionHeaderTableStructure*)section->Data.get();
}

/******************************************************************************/
uint32_t Versal_2ve_2vmPartitionHeader::GetPartitionHeaderSize(void)
{
    return sizeof(Versal_2ve_2vmPartitionHeaderTableStructure);
}

/******************************************************************************/
uint32_t Versal_2ve_2vmPartitionHeader::GetSectionCount(void)
{
    return pHTable->dataSectionCount;
}

/******************************************************************************/
uint32_t Versal_2ve_2vmPartitionHeader::GetChecksum(void)
{
    return pHTable->pHChecksum;
}

/******************************************************************************/
uint64_t Versal_2ve_2vmPartitionHeader::GetExecAddress(void)
{
    return pHTable->destinationExecAddress;
}

/******************************************************************************/
uint64_t Versal_2ve_2vmPartitionHeader::GetLoadAddress(void)
{
    return pHTable->destinationLoadAddress;
}

/******************************************************************************/
uint32_t Versal_2ve_2vmPartitionHeader::GetEncryptedPartitionLength(void)
{
    return pHTable->encryptedPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t Versal_2ve_2vmPartitionHeader::GetUnencryptedPartitionLength(void)
{
    return pHTable->unencryptedPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t Versal_2ve_2vmPartitionHeader::GetTotalPartitionLength(void)
{
    return pHTable->totalPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint8_t Versal_2ve_2vmPartitionHeader::GetChecksumType(void)
{
    return ((pHTable->partitionAttributes >> v2phtChecksumTypeShift) & v2phtChecksumTypeMask);
}

/******************************************************************************/
uint8_t Versal_2ve_2vmPartitionHeader::GetOwnerType(void)
{
    return ((pHTable->partitionAttributes >> v2phtPartitionOwnerShift) & v2phtPartitionOwnerMask);
}

/******************************************************************************/
uint8_t Versal_2ve_2vmPartitionHeader::GetDestinationCpu(void)
{
    return ((pHTable->partitionAttributes >> v2phtDestCpuShift) & v2phtDestCpuMask);
}

/******************************************************************************/
uint8_t Versal_2ve_2vmPartitionHeader::GetDestinationCluster(void)
{
    return ((pHTable->partitionAttributes >> v2phtClusterShift) & v2phtClusterMask);
}

/******************************************************************************/
uint8_t Versal_2ve_2vmPartitionHeader::GetProcessorExecState(void)
{
    return ((pHTable->partitionAttributes >> v2phtExecStateShift) & v2phtExecStateMask);
}

/******************************************************************************/
uint8_t Versal_2ve_2vmPartitionHeader::GetElfEndianess(void)
{
    if ((pHTable->partitionAttributes >> v2phtEndiannessShift) & v2phtEndiannessMask)
    {
        return Endianness::BigEndian;
    }
    else
    {
        return Endianness::LittleEndian;
    }
}

/******************************************************************************/
uint8_t Versal_2ve_2vmPartitionHeader::GetExceptionLevel(void)
{
    return ((pHTable->partitionAttributes >> v2phtExceptionLevelShift) & v2phtExceptionLevelMask);
}

/******************************************************************************/
uint8_t Versal_2ve_2vmPartitionHeader::GetTrustZone(void)
{
    return ((pHTable->partitionAttributes >> v2phtTrustzoneShift) & v2phtTrustzoneMask);
}

/******************************************************************************/
uint8_t Versal_2ve_2vmPartitionHeader::GetHivec(void)
{
    return ((pHTable->partitionAttributes >> v2phtHivecShift) & v2phtHivecMask);
}

/******************************************************************************/
uint32_t Versal_2ve_2vmPartitionHeader::GetAuthCertificateOffset(void)
{
    return (pHTable->authCertificateOffset * sizeof(uint32_t));
}

/******************************************************************************/
uint32_t Versal_2ve_2vmPartitionHeader::GetPartitionUid(void)
{
    return pHTable->puid;
}

/******************************************************************************/
uint32_t Versal_2ve_2vmPartitionHeader::GetPartitionWordOffset(void)
{
    return (pHTable->partitionWordOffset * sizeof(uint32_t));
}

/******************************************************************************/
PartitionType::Type Versal_2ve_2vmPartitionHeader::GetPartitionType(void)
{
    return (PartitionType::Type)((pHTable->partitionAttributes >> v2phtPartitionTypeShift) & v2phtPartitionTypeMask);
}

/******************************************************************************/
DpaCM::Type Versal_2ve_2vmPartitionHeader::GetDpaCMFlag(void)
{
    return (DpaCM::Type)((pHTable->partitionAttributes >> v2phtDpaCMShift) & v2phtDpaCMMask);
}

/******************************************************************************/
PufHdLoc::Type Versal_2ve_2vmPartitionHeader::GetPufHdLocation(void)
{
    return (PufHdLoc::Type)((pHTable->partitionAttributes >> v2phtPufHDLocationShift) & v2phtPufHDLocationMask);
}

/******************************************************************************/
Lockstep::Type Versal_2ve_2vmPartitionHeader::GetLockStepFlag (void)
{
    return (Lockstep::Type)((pHTable->partitionAttributes >> v2phtlockStepShift) & v2phtlockStepMask);
}

/******************************************************************************/
uint64_t Versal_2ve_2vmPartitionHeader::GetLQspiExecAddrForXip(uint64_t execAddr)
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
uint32_t Versal_2ve_2vmPartitionHeader::GetPartitionPadSize64bBoundary(Section* sec)
{
    return ((64 - (sec->Length & 63)) & 63);
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeader::SetPartitionRevokeId(uint32_t id)
{
    pHTable->partitionRevokeId = id;
}

/******************************************************************************/
KeySource::Type Versal_2ve_2vmPartitionHeader::GetPartitionKeySource(void)
{
    return (KeySource::Type)pHTable->partitionKeySource;
}

/******************************************************************************/
uint8_t Versal_2ve_2vmPartitionHeader::GetEncryptFlag(void)
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
uint8_t Versal_2ve_2vmPartitionHeader::GetAuthCertFlag(void)
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
TcmBoot::Type Versal_2ve_2vmPartitionHeader::GetTcmBootFlag(void)
{
    return (TcmBoot::Type)((pHTable->partitionAttributes >> v2phtTcmBootShift) & v2phtTcmBootMask);
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeaderTable::Build(BootImage & bi, Binary & cache)
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
        auto encHdr = std::make_unique<Section>("EncryptedMetaHeader", bi.imageHeaderTable->metaHeaderLength + totalBlocksOverhead);
        bi.encryptedHeaders = encHdr.get();  // Store raw pointer for later use
        cache.Sections.push_back(std::move(encHdr));  // Cache owns it
    }
    else
    {
        for (std::list<Section*>::iterator itr = bi.headers.begin(); itr != bi.headers.end(); itr++)
        {
            // Transfer ownership to cache (no deleter needed - cache takes ownership)
            cache.Sections.push_back(std::unique_ptr<Section>(*itr));
        }
        bi.headers.clear();  // Cache owns them now
    }

    if (bi.bifOptions->GetHeaderAC())
    {
        LOG_INFO("Creating Header Authentication Certificate");
        ConfigureMetaHdrAuthenticationContext(bi);
        bi.headerAC = std::make_unique<Versal_2ve_2vmAuthenticationCertificate>(bi.metaHdrAuthCtx.get());
        bi.headerAC->Build(bi, cache, bi.imageHeaderTable->section, false, true);
    }

    //store partitionNumber-hashblock mapping in hashNumMap 
    for(std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
    {
        // Dont need mapping for bootloader as bootloader's hash is stored in BH hashblock
        if((*partHdr)->IsBootloader())
        {
            continue;
        }

        // If partition is not authenticated store its hash in meta header's hashblock 
        if((*partHdr)->imageHeader->GetAuthenticationType() == Authentication::None
            || ( (*partHdr)->imageHeader->GetPskFile().size() == 0
                && (*partHdr)->imageHeader->GetSskFile().size() == 0
                && (*partHdr)->imageHeader->GetPpkFile().size() == 0
                && (*partHdr)->imageHeader->GetSpkFile().size() == 0))
        {
            if(bi.IsBootloaderFound() == false)
            {
                bi.hashNumMap.push_back(std::pair<uint32_t, uint32_t>((*partHdr)->partitionNum+1, 0));
            }
            else
            {
                bi.hashNumMap.push_back(std::pair<uint32_t, uint32_t>((*partHdr)->partitionNum, 0));
            }
        }
        
        else if(bi.metaHdrAuthCtx.get() != NULL
                && (*partHdr)->imageHeader->GetPskFile() == bi.metaHdrAuthCtx->pskFile
                && (*partHdr)->imageHeader->GetSskFile() == bi.metaHdrAuthCtx->sskFile
                && (*partHdr)->imageHeader->GetPpkFile() == bi.metaHdrAuthCtx->ppkFile
                && (*partHdr)->imageHeader->GetSpkFile() == bi.metaHdrAuthCtx->spkFile
                && (*partHdr)->imageHeader->GetSpkRevocationId() == bi.metaHdrAuthCtx->spkIdentification)
        {
            if(bi.IsBootloaderFound() == false)
            {
                bi.hashNumMap.push_back(std::pair<uint32_t, uint32_t>((*partHdr)->partitionNum+1, 0));
            }
            else
            {
                bi.hashNumMap.push_back(std::pair<uint32_t, uint32_t>((*partHdr)->partitionNum, 0));
            }
        }
        else
        {
            for(std::list<PartitionHeader*>::iterator hashBlock = bi.partitionHeaderList.begin(); hashBlock != bi.partitionHeaderList.end(); hashBlock++)
            {
                if(!(*hashBlock)->IsBootloader())
                {
                    if((*partHdr)->imageHeader->GetPskFile() == (*hashBlock)->imageHeader->GetPskFile()
                        && (*partHdr)->imageHeader->GetSskFile() == (*hashBlock)->imageHeader->GetSskFile()
                        && (*partHdr)->imageHeader->GetPpkFile() == (*hashBlock)->imageHeader->GetPpkFile()
                        && (*partHdr)->imageHeader->GetSpkFile() == (*hashBlock)->imageHeader->GetSpkFile()
                        && (*partHdr)->imageHeader->GetSpkRevocationId() == (*hashBlock)->imageHeader->GetSpkRevocationId())
                    {
                        if(bi.IsBootloaderFound() == false)
                        {
                            bi.hashNumMap.push_back(std::pair<uint32_t, uint32_t>((*partHdr)->partitionNum+1, (*hashBlock)->partitionNum+1));
                        }
                        else
                        {
                            bi.hashNumMap.push_back(std::pair<uint32_t, uint32_t>((*partHdr)->partitionNum, (*hashBlock)->partitionNum));
                        }
                        break;
                    }
                }
            }
        }
    }

    uint32_t count = 1;     //1 extra entry for meta header hash & index in hash block 1
    if (bi.hashNumMap.size() != 0)
    {
        for (size_t i = 0; i < bi.hashNumMap.size(); i++)
        {
            if(bi.hashNumMap[i].second == 0)
                count++;
        }
    }
	
    bi.imageHeaderTable->hashBlockSectionLength = count * (bi.hash->GetHashLength() + HASH_BLOCK_INDEX_BYTES);
    bi.imageHeaderTable->hashBlockSectionLength += PADDING_16B(bi.imageHeaderTable->hashBlockSectionLength);
    auto hashBlockSectionPtr = std::make_unique<Section>("HashBlock", bi.imageHeaderTable->hashBlockSectionLength);
    bi.imageHeaderTable->hashBlockSection = hashBlockSectionPtr.release();

	if (bi.options.bifOptions->metaHdrAttributes.authenticate != Authentication::None)
    {
        bi.imageHeaderTable->hashBlockSection->IncreaseLengthAndPadTo(bi.imageHeaderTable->hashBlockSectionLength + bi.metaHdrAuthCtx->GetTotalHashBlockSignSize(), 0);
    }
    if (bi.options.bifOptions->metaHdrAttributes.encrypt != Encryption::None && bi.options.bifOptions->metaHdrAttributes.authenticate == Authentication::None)
    {
        bi.imageHeaderTable->hashBlockSection->IncreaseLengthAndPadTo(bi.imageHeaderTable->hashBlockSectionLength + AES_GCM_TAG_SZ, 0);
    }

    cache.Sections.push_back(std::unique_ptr<Section>(bi.imageHeaderTable->hashBlockSection));
}

/******************************************************************************/
void Versal_2ve_2vmPartitionHeaderTable::ConfigureMetaHdrAuthenticationContext(BootImage & bi)
{
    AuthenticationContext* biAuth = NULL;
    for (std::list<ImageHeader*>::iterator image = bi.imageList.begin(); image != bi.imageList.end(); image++)
    {
        if (((*image)->IsBootloader()) && ((*image)->GetAuthenticationType() == Authentication::None))
        {
            //LOG_ERROR("Bootloader must be authenticated to authenticate Meta Header.");
        }
    }

    auto biAuthPtr = std::make_unique<Versal_2ve_2vmAuthenticationContext>(bi.options.bifOptions->metaHdrAttributes.authenticate);
    biAuth = biAuthPtr.release();
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

    biAuth->lmsOnly = bi.bifOptions->metaHdrAttributes.lmsOnly;

    //Copying primary and secondary LmsKeyParam from bifOptions to authCxt
    std::vector<LmsKeyParam> primaryLmsParams = bi.bifOptions->GetPrimaryLmsParams();
    biAuth->primaryLmsParamsSize = primaryLmsParams.size() * 2;
    biAuth->primaryLmsParams = new int[biAuth->primaryLmsParamsSize];
    for (int i = 0, j = 0; i < biAuth->primaryLmsParamsSize && j < (int)primaryLmsParams.size(); i=i+2, j++)
    {
        biAuth->primaryLmsParams[i] = primaryLmsParams[j].h;
        biAuth->primaryLmsParams[i+1] = primaryLmsParams[j].w;
    }

    std::vector<LmsKeyParam> secondaryLmsParams = bi.bifOptions->GetSecondaryLmsParams();
    biAuth->secondaryLmsParamsSize = secondaryLmsParams.size() * 2;
    biAuth->secondaryLmsParams = new int[biAuth->secondaryLmsParamsSize];
    for (int i = 0, j = 0; i < biAuth->secondaryLmsParamsSize && j < (int)secondaryLmsParams.size(); i=i+2, j++)
    {
        biAuth->secondaryLmsParams[i] = secondaryLmsParams[j].h;
        biAuth->secondaryLmsParams[i+1] = secondaryLmsParams[j].w;
    }

    if((bi.options.bifOptions->metaHdrAttributes.authenticate == Authentication::LMS_SHA2_256) ||
       (bi.options.bifOptions->metaHdrAttributes.authenticate == Authentication::LMS_SHAKE256))
    {
        biAuth->signatureLength = GetLmsSignatureLength(biAuth->primaryLmsParams, biAuth->primaryLmsParamsSize,
                                biAuth->pskFile.c_str(), biAuth->ppkFile.c_str(), biAuth->lmsOnly);
        //biAuth->certSize = biAuth->GetCertificateSize();
    }

    if (bi.bifOptions->metaHdrAttributes.spkSignature != "")
    {
        biAuth->SetSPKSignatureFile(bi.bifOptions->metaHdrAttributes.spkSignature);
    }
    biAuth->spkIdentification = bi.bifOptions->metaHdrAttributes.spkRevokeId;

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
    bi.metaHdrAuthCtx = std::make_unique<Versal_2ve_2vmAuthenticationContext>(biAuth, bi.bifOptions->metaHdrAttributes.authenticate);
    
    if (bi.bifOptions->metaHdrAttributes.presign != "")
    {
        bi.metaHdrAuthCtx->SetPresignFile(bi.bifOptions->metaHdrAttributes.presign);
    }
}
#if 0
/******************************************************************************/
void Versal_2ve_2vmPartitionHeaderTable::UpdateAtfHandoffParams(BootImage & bi)
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
            memcpy((*partHdr)->partition->section->Data.get() + (*partHdr)->atf_handoff_params_offset, &atf_handoff_params, sizeof(atf_handoff_params_struct));
        }
    }
}
#endif
/******************************************************************************/
void Versal_2ve_2vmPartitionHeaderTable::Link(BootImage & bi)
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
        auto sha_hash = std::make_unique<uint8_t[]>(bi.hash->GetHashLength());
        std::list<Section*> sections;
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
                size += sizeof(Versal_2ve_2vmPartitionHeaderTableStructure);
            }
        }

        /* Create one new combined section with all the appended sections above */
        auto headers_ptr = std::make_unique<Section>("Headers", size);
        //headers_ptr->Address = iHT->section->Address; // not really needed, but useful for debug.
        memset(headers_ptr->Data.get(), bi.options.GetOutputFillByte(), headers_ptr->Length);

        Binary::Address_t start = sections.front()->Address;
        for (std::list<Section*>::iterator i = sections.begin(); i != sections.end(); i++)
        {
            Section& section(**i);
            int offset = section.Address - start;
            memcpy(headers_ptr->Data.get() + offset, section.Data.get(), section.Length);
        }
        /* Replace sections list with the combined new section */
        sections.clear();
        Section* headers = headers_ptr.get();  // Get raw pointer before releasing
        sections.push_back(headers_ptr.release());  // Transfer ownership to legacy container



        
        bi.hash->CalculateVersalHash(true, headers->Data.get(), size, sha_hash.get());
        
        fprintf(stderr, "[META-HASH-RESULT] Meta header SHA3 hash (THIS IS THE CRITICAL VALUE):\n");
        for (size_t i = 0; i < bi.hash->GetHashLength(); i++) {
            fprintf(stderr, "%02x", sha_hash.get()[i]);
            if ((i+1) % 16 == 0) fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");

        // headers is now managed by sections container

        /* Copy Meta Header Hash into Hash Block 1 */
        fprintf(stderr, "[LINK-IHT-HB] imageHeaderTable->hashBlockSection=%p\n", (void*)bi.imageHeaderTable->hashBlockSection);
        if (bi.imageHeaderTable->hashBlockSection) {
            fprintf(stderr, "[LINK-IHT-HB] Zeroing and copying meta header hash, sectionLength=%u\n", bi.imageHeaderTable->hashBlockSectionLength);
            memset(bi.imageHeaderTable->hashBlockSection->Data.get(), 0, bi.imageHeaderTable->hashBlockSectionLength);
            memcpy(bi.imageHeaderTable->hashBlockSection->Data.get() + HASH_BLOCK_INDEX_BYTES, sha_hash.get(), bi.hash->GetHashLength());
            fprintf(stderr, "[LINK-IHT-HB] Meta header hash copied to offset %u\n", HASH_BLOCK_INDEX_BYTES);
        }
#ifdef DEBUG
        LOG_TRACE("Meta Header Length %d", bi.imageHeaderTable->metaHeaderLength);
        LOG_TRACE("Meta Header Data");
        LOG_DUMP_BYTES(headers->Data, bi.imageHeaderTable->metaHeaderLength);
        LOG_TRACE("Meta Header Hash - Copied to Hash Block 1");
        LOG_DUMP_BYTES(sha_hash.get(), bi.hash->GetHashLength());
#endif

        /* Copy Partition Hashes for the partitions which are not authenticated into Hash Block 1 */
        uint32_t addr = HASH_BLOCK_INDEX_BYTES + bi.hash->GetHashLength();
        for (size_t i = 0; i < bi.hashNumMap.size(); i++)
        {
            if(bi.hashNumMap[i].second == 0)
            {
                for (size_t j = 0; j < bi.hashTable.size(); j++)
                {
                    if(bi.hashNumMap[i].first == bi.hashTable[j].first)
                    {
                        uint32_t partition_num = bi.hashNumMap[i].first;
                            memcpy(bi.imageHeaderTable->hashBlockSection->Data.get() + addr, &partition_num, HASH_BLOCK_INDEX_BYTES);
                        addr += HASH_BLOCK_INDEX_BYTES;
                            memcpy(bi.imageHeaderTable->hashBlockSection->Data.get() + addr, bi.hashTable[j].second.get(), bi.hash->GetHashLength());
                        addr += bi.hash->GetHashLength();
                        break;
                    }
                }
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
        auto sha_hash = std::make_unique<uint8_t[]>(bi.hash->GetHashLength());
        bi.hash->CalculateVersalHash(true, bi.imageHeaderTable->hashBlockSection->Data.get(), bi.imageHeaderTable->hashBlockSectionLength, sha_hash.get());
#ifdef DEBUG
        LOG_TRACE("Hash Block 1 Data");
        LOG_DUMP_BYTES(bi.imageHeaderTable->hashBlockSection->Data.get(), bi.imageHeaderTable->hashBlockSectionLength);
        LOG_TRACE("Hash Block 1 Hash - Copied to Hash Block 0");
        LOG_DUMP_BYTES(sha_hash.get(), bi.hash->GetHashLength());
#endif
        /* Copy Hash Block 1 Hash into Hash Block 0 */

        for (SectionList::iterator i = bi.cache->Sections.begin(); i != bi.cache->Sections.end(); i++)
        {
            Section& section(**i);
            if (section.isBootloader)
            {
                uint32_t hashIndex = HASH_BLOCK_HASHBLOCK1_HASH_INDEX;
                size_t offset = 3 * (HASH_BLOCK_INDEX_BYTES + bi.hash->GetHashLength());
                memcpy(section.Data.get() + offset, &hashIndex, HASH_BLOCK_INDEX_BYTES);
                memcpy(section.Data.get() + HASH_BLOCK_INDEX_BYTES + offset, sha_hash.get(), bi.hash->GetHashLength());

                break;
            }
        }

        for (std::list<PartitionHeader*>::iterator partHdr = bi.partitionHeaderList.begin(); partHdr != bi.partitionHeaderList.end(); partHdr++)
        {
            if ((*partHdr)->IsBootloader())
            {
                if ((*partHdr)->imageHeader->GetEncryptContext()->Type() != Encryption::None && (*partHdr)->imageHeader->GetAuthContext()->authAlgorithm->Type() == Authentication::None)
                {
                    LOG_TRACE("Hash Block 0");
                    LOG_DUMP_BYTES((*partHdr)->partition->section->Data.get(), bi.hashBlockLength);

                    (*partHdr)->imageHeader->GetEncryptContext()->AesGcm256HashBlockEncrypt(bi.options, (*partHdr)->partition->section->Data.get(),
                        bi.hashBlockLength, (*partHdr)->partition->section->Data.get() + bi.hashBlockLength, 2);

                    LOG_TRACE("GCM Tag + Hash Block 0");
                    LOG_DUMP_BYTES((*partHdr)->partition->section->Data.get(), bi.hashBlockLength + AES_GCM_TAG_SZ);
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
