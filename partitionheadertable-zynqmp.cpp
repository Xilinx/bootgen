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
#include "partitionheadertable-zynqmp.h"
#include "authentication-zynqmp.h"

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
ZynqMpPartitionHeader::ZynqMpPartitionHeader(ImageHeader* imageheader, int index)
    : partitionEncrypted(0)
    , PartitionHeader(imageheader, index)
{
    std::string name;
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
    }
    else
    {
        name = "PartitionHeader Null";
    }
    section = new Section(name, sizeof(ZynqMpPartitionHeaderTableStructure));
    memset(section->Data, 0, section->Length);

    pHTable = (ZynqMpPartitionHeaderTableStructure*)section->Data;
}

/******************************************************************************/
ZynqMpPartitionHeader::~ZynqMpPartitionHeader()
{
    if (section != NULL)
    {
        delete section;
    }
}

/******************************************************************************/
void ZynqMpPartitionHeader::ReadHeader(std::ifstream& ifs)
{
    ifs.read((char*)pHTable, sizeof(ZynqMpPartitionHeaderTableStructure));

    uint32_t checksum = ComputeWordChecksum(pHTable, sizeof(ZynqMpPartitionHeaderTableStructure) - sizeof(uint32_t));

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
    authBlock = GetAuthblock();

    presigned = (authCertPresent != 0);
    if (presigned)
    {
        certificateRelativeByteOffset = (GetAuthCertificateOffset() - GetPartitionWordOffset());
    }
}

/******************************************************************************/
void ZynqMpPartitionHeader::ReadData(std::ifstream& ifs)
{
    uint32_t data_len = GetTotalPartitionLength();
    std::string part_name = imageHeader->GetName() + StringUtils::Format(".%d", index);
    Section* dsection = new Section(part_name, data_len);
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
void ZynqMpPartitionHeader::SetEncryptedPartitionLength(uint32_t len)
{
    pHTable->encryptedPartitionLength = len / sizeof(uint32_t);
}

/******************************************************************************/
void ZynqMpPartitionHeader::SetUnencryptedPartitionLength(uint32_t len)
{
    pHTable->unencryptedPartitionLength = len / sizeof(uint32_t);
}

/******************************************************************************/
void ZynqMpPartitionHeader::SetTotalPartitionLength(uint32_t len)
{
    pHTable->totalPartitionLength = len / sizeof(uint32_t);
    for (std::list<AuthenticationCertificate*>::iterator acs = ac.begin(); acs != ac.end(); acs++)
    {
        if (*acs && (*acs)->section)
        {
            pHTable->totalPartitionLength += (uint32_t)((*acs)->section->Length / sizeof(uint32_t)); // additional size of AC
        }
    }
}

/******************************************************************************/
void ZynqMpPartitionHeader::SetNextPartitionHeaderOffset(void)
{
    static PartitionHeader* prev_part_hdr = NULL;
    if (prev_part_hdr != NULL)
    {
        ZynqMpPartitionHeaderTableStructure* prev_pht = (ZynqMpPartitionHeaderTableStructure*)prev_part_hdr->section->Data;
        prev_pht->nextPartitionHeaderOffset = (uint32_t)(section->Address / sizeof(uint32_t));
        if (slaveBootSplitMode && (prev_pht->nextPartitionHeaderOffset != 0))
        {
            prev_pht->nextPartitionHeaderOffset -= fullBhSize / sizeof(uint32_t);
        }

        prev_part_hdr->SetChecksum();
    }
    prev_part_hdr = this;
}

/******************************************************************************/
void ZynqMpPartitionHeader::SetExecAddress(uint64_t addr)
{
    pHTable->destinationExecAddress = addr;
}

/******************************************************************************/
void ZynqMpPartitionHeader::SetLoadAddress(uint64_t addr)
{
    pHTable->destinationLoadAddress = addr;
}

/******************************************************************************/
void ZynqMpPartitionHeader::SetPartitionWordOffset(uint32_t addr)
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
void ZynqMpPartitionHeader::SetPartitionAttributes(void)
{
    if (authCertPresent == 0)
    {
        authCertPresent = imageHeader->GetAuthenticationType() == Authentication::RSA;
    }

    destDevice = imageHeader->GetDestinationDevice();
    destCpu = imageHeader->GetDestCpu();
    exceptionLevel = imageHeader->GetExceptionLevel();
    trustzone = imageHeader->GetTrustZone();
    early_handoff = imageHeader->GetEarlyHandoff();
    hivec = imageHeader->GetHivec();
    if (hivec) {
        if ((destCpu == DestinationCPU::PMU) || (destDevice == DestinationDevice::DEST_DEV_PL) || (execState == A53ExecState::AARCH64))
        {
            LOG_ERROR("BIF attribute 'hivec' is supported only for a53(32-bit) and r5 applications.");
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

    ownerType = imageHeader->GetPartOwner();

    authBlock = imageHeader->GetAuthBlock();
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
        partitionEncrypted = 1;
    }
    else if (encryptFlag == 1)
    {
        /* Encryption to be enabled in release mode based on preencrypted flag available,
           since Encrption Context will not be AES */
        partitionEncrypted = 1;
    }
    else
    {
        partitionEncrypted = 0;
    }

    uint8_t endian = 0;
    if (elfEndianess == Endianness::LittleEndian)
    {
        endian = 0;
    }
    else if (elfEndianess == Endianness::BigEndian)
    {
        endian = 1;
    }

    pHTable->partitionAttributes = (trustzone << PH_TRUSTZONE_SHIFT_ZYNQMP) |
                                   (exceptionLevel << PH_EXCEPTION_LEVEL_SHIFT_ZYNQMP) |
                                   (execState << PH_EXEC_STATE_SHIFT_ZYNQMP) |
                                   (destDevice << PH_DEST_DEVICE_SHIFT_ZYNQMP) |
                                   (partitionEncrypted << PH_ENCRYPT_SHIFT_ZYNQMP) |
                                   (destCpu << PH_DEST_CPU_SHIFT_ZYNQMP) |
                                   (authCertPresent << PH_AC_FLAG_SHIFT_ZYNQMP) |
                                   (checksumType << PH_CHECKSUM_SHIFT_ZYNQMP) |
                                   (ownerType << PH_OWNER_SHIFT_ZYNQMP) |
                                   (endian << PH_ENDIAN_SHIFT_ZYNQMP) |
                                   (early_handoff << PH_EARLY_HANDOFF_SHIFT_ZYNQMP) |
                                   (authBlock << PH_AUTH_BLOCK_SHIFT_ZYNQMP) |
                                   (hivec << PH_HIVEC_SHIFT_ZYNQMP);
}

/******************************************************************************/
void ZynqMpPartitionHeader::SetSectionCount(uint32_t cnt)
{
    pHTable->dataSectionCount = cnt;
}

/******************************************************************************/
void ZynqMpPartitionHeader::SetChecksumOffset(void)
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
void ZynqMpPartitionHeader::SetImageHeaderOffset(uint32_t offset)
{
    pHTable->imageHeaderWordOffset = offset / sizeof(uint32_t);

    if (slaveBootSplitMode && (pHTable->imageHeaderWordOffset != 0))
    {
        pHTable->imageHeaderWordOffset -= fullBhSize / sizeof(uint32_t);
    }
}

/******************************************************************************/
void ZynqMpPartitionHeader::SetAuthCertificateOffset(void)
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
            pHTable->authCertificateOffset = (uint32_t)(acs->section->Address / sizeof(uint32_t));
        }
    }
    else if (imageHeader->GetAuthenticationType() == Authentication::RSA)
    {
        /* If the image is not yet signed, partition addr + partition length - cert size */
        AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
        pHTable->authCertificateOffset = (uint32_t)((partition->section->Address + partition->section->Length - sizeof(AuthCertificate4096Structure)) / sizeof(uint32_t));
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
void ZynqMpPartitionHeader::SetPartitionNumber(uint32_t partNum)
{
    pHTable->partitionNumber = partNum;
}

/******************************************************************************/
void ZynqMpPartitionHeader::SetChecksum(void)
{
    pHTable->pHChecksum = ComputeWordChecksum(pHTable, sizeof(ZynqMpPartitionHeaderTableStructure) - sizeof(uint32_t));
}

/******************************************************************************/
void ZynqMpPartitionHeader::RealignSectionDataPtr(void)
{
    pHTable = (ZynqMpPartitionHeaderTableStructure*)section->Data;
}

/******************************************************************************/
uint32_t ZynqMpPartitionHeader::GetPartitionHeaderSize(void)
{
    return sizeof(ZynqMpPartitionHeaderTableStructure);
}

/******************************************************************************/
uint32_t ZynqMpPartitionHeader::GetChecksum(void)
{
    return pHTable->pHChecksum;
}

/******************************************************************************/
uint32_t ZynqMpPartitionHeader::GetPartitionNumber(void)
{
    return pHTable->partitionNumber;
}

/******************************************************************************/
uint64_t ZynqMpPartitionHeader::GetExecAddress(void)
{
    return pHTable->destinationExecAddress;
}

/******************************************************************************/
uint64_t ZynqMpPartitionHeader::GetLoadAddress(void)
{
    return pHTable->destinationLoadAddress;
}

/******************************************************************************/
uint32_t ZynqMpPartitionHeader::GetEncryptedPartitionLength(void)
{
    return pHTable->encryptedPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t ZynqMpPartitionHeader::GetUnencryptedPartitionLength(void)
{
    return pHTable->unencryptedPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t ZynqMpPartitionHeader::GetTotalPartitionLength(void)
{
    return pHTable->totalPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetDestinationDevice(void)
{
    return ((pHTable->partitionAttributes >> PH_DEST_DEVICE_SHIFT_ZYNQMP) & PH_DEST_DEVICE_MASK_ZYNQMP);
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetEncryptFlag(void)
{
    return ((pHTable->partitionAttributes >> PH_ENCRYPT_SHIFT_ZYNQMP) & PH_ENCRYPT_MASK_ZYNQMP);
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetAuthCertFlag(void)
{
    return ((pHTable->partitionAttributes >> PH_AC_FLAG_SHIFT_ZYNQMP) & PH_AC_FLAG_MASK_ZYNQMP);
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetChecksumType(void)
{
    return ((pHTable->partitionAttributes >> PH_CHECKSUM_SHIFT_ZYNQMP) & PH_CHECKSUM_MASK_ZYNQMP);
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetOwnerType(void)
{
    return ((pHTable->partitionAttributes >> PH_OWNER_SHIFT_ZYNQMP) & PH_OWNER_MASK_ZYNQMP);
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetDestinationCpu(void)
{
    return ((pHTable->partitionAttributes >> PH_DEST_CPU_SHIFT_ZYNQMP) & PH_DEST_CPU_MASK_ZYNQMP);
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetProcessorExecState(void)
{
    return ((pHTable->partitionAttributes >> PH_EXEC_STATE_SHIFT_ZYNQMP) & PH_EXEC_STATE_MASK_ZYNQMP);
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetElfEndianess(void)
{
    if ((pHTable->partitionAttributes >> PH_ENDIAN_SHIFT_ZYNQMP) & PH_ENDIAN_MASK_ZYNQMP)
    {
        return Endianness::BigEndian;
    }
    else
    {
        return Endianness::LittleEndian;
    }
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetExceptionLevel(void)
{
    return ((pHTable->partitionAttributes >> PH_EXCEPTION_LEVEL_SHIFT_ZYNQMP) & PH_EXCEPTION_LEVEL_MASK_ZYNQMP);
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetTrustZone(void)
{
    return ((pHTable->partitionAttributes >> PH_TRUSTZONE_SHIFT_ZYNQMP) & PH_TRUSTZONE_MASK_ZYNQMP);
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetEarlyHandoff(void)
{
    return ((pHTable->partitionAttributes >> PH_EARLY_HANDOFF_SHIFT_ZYNQMP) & PH_EARLY_HANDOFF_MASK_ZYNQMP);
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetHivec(void)
{
    return ((pHTable->partitionAttributes >> PH_HIVEC_SHIFT_ZYNQMP) & PH_HIVEC_MASK_ZYNQMP);
}

/******************************************************************************/
uint32_t ZynqMpPartitionHeader::GetAuthCertificateOffset(void)
{
    return (pHTable->authCertificateOffset * sizeof(uint32_t));
}

/******************************************************************************/
uint8_t ZynqMpPartitionHeader::GetAuthblock(void)
{
    uint8_t block = ((pHTable->partitionAttributes >> PH_AUTH_BLOCK_SHIFT_ZYNQMP) & PH_AUTH_BLOCK_MASK_ZYNQMP);
    if (block != 0)
    {
        block = 1 << (block + 1);
    }
    return block;
}

/******************************************************************************/
uint32_t ZynqMpPartitionHeader::GetPartitionWordOffset(void)
{
    return (pHTable->partitionWordOffset * sizeof(uint32_t));
}

/******************************************************************************/
uint32_t ZynqMpPartitionHeader::GetPartitionPadSize64bBoundary(Section* sec)
{
    return ((64 - (sec->Length & 63)) & 63);
}

/******************************************************************************/
void ZynqMpPartitionHeader::SetBitLoadAddress(bool load, uint32_t val)
{
    if (load)
    {
        loadAddress = val;
    }
    else
    {
        loadAddress = 0xFFFFFFFF;
    }
}
