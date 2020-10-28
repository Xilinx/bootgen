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
#include "partitionheadertable-zynq.h"
#include "authentication-zynq.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
ZynqPartitionHeader::ZynqPartitionHeader(ImageHeader* imageheader, int index)
    : PartitionHeader(imageheader, index)
{
    std::string name;
    if (imageHeader)
    {
        name = "PartitionHeader " + imageHeader->GetName() + StringUtils::Format(".%d", index);
    }
    else
    {
        name = "PartitionHeader Null";
    }
    section = new Section(name, sizeof(ZynqPartitionHeaderTableStructure));
    memset(section->Data, 0, section->Length);

    pHTable = (ZynqPartitionHeaderTableStructure*)section->Data;
}

/******************************************************************************/
ZynqPartitionHeader::~ZynqPartitionHeader()
{
    if (section != NULL)
    {
        delete section;
    }
}

/******************************************************************************/
void ZynqPartitionHeader::ReadHeader(std::ifstream& ifs)
{
    ifs.read((char*)pHTable, sizeof(ZynqPartitionHeaderTableStructure));

    uint32_t checksum = ComputeWordChecksum(pHTable, sizeof(ZynqPartitionHeaderTableStructure) - sizeof(uint32_t));

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
    checksumType = GetChecksumType();
    ownerType = GetOwnerType();

    presigned = (authCertPresent != 0);
    if (presigned)
    {
        certificateRelativeByteOffset = (GetAuthCertificateOffset() - GetPartitionWordOffset());
    }
}

/******************************************************************************/
void ZynqPartitionHeader::ReadData(std::ifstream& ifs)
{
    uint32_t dataLen = GetTotalPartitionLength();
    std::string partName = imageHeader->GetName() + StringUtils::Format(".%d", index);
    Section* dsection = new Section(partName, dataLen);
    LOG_INFO("TESTINFO: SecName-%s, SecAdd-0x%x", dsection->Name.c_str(), dsection->Address);
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
void ZynqPartitionHeader::SetEncryptedPartitionLength(uint32_t len)
{
    pHTable->encryptedPartitionLength = len / sizeof(uint32_t);
}


/******************************************************************************/
void ZynqPartitionHeader::SetUnencryptedPartitionLength(uint32_t len)
{
    pHTable->unencryptedPartitionLength = len / sizeof(uint32_t);
}

/******************************************************************************/
void ZynqPartitionHeader::SetTotalPartitionLength(uint32_t len)
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
void ZynqPartitionHeader::SetLoadAddress(uint64_t addr)
{
    pHTable->destinationLoadAddress = (uint32_t)addr;
}

/******************************************************************************/
void ZynqPartitionHeader::SetExecAddress(uint64_t addr)
{
    pHTable->destinationExecAddress = (uint32_t)addr;
}

/******************************************************************************/
void ZynqPartitionHeader::SetPartitionWordOffset(uint32_t addr)
{
    pHTable->partitionWordOffset = addr / sizeof(uint32_t);
}

/******************************************************************************/
void ZynqPartitionHeader::SetPartitionAttributes(void)
{
    if (authCertPresent == 0)
    {
        authCertPresent = imageHeader->GetAuthenticationType() == Authentication::RSA;
    }

    switch (imageHeader->GetDomain())
    {
        case Domain::PL:
            destDevice = DestinationDevice::DEST_DEV_PL;
            break;
        case Domain::PS:
            destDevice = DestinationDevice::DEST_DEV_PS;
            break;
        default:
            LOG_ERROR("Destination domain %d not supported", imageHeader->GetDomain());
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

    pHTable->partitionAttributes = (headAlignment << PH_HEAD_ALIGN_SHIFT) |
                                   (tailAlignment << PH_TAIL_ALIGN_SHIFT) |
                                   (destDevice << PH_DEST_DEVICE_SHIFT) |
                                   (authCertPresent << PH_AC_FLAG_SHIFT) |
                                   (checksumType << PH_CHECKSUM_SHIFT) |
                                   (ownerType << PH_OWNER_SHIFT);
}

/******************************************************************************/
void ZynqPartitionHeader::SetSectionCount(uint32_t cnt)
{
    pHTable->dataSectionCount = cnt;
}

/******************************************************************************/
void ZynqPartitionHeader::SetChecksumOffset(void)
{
    if (pHTable->checksumWordOffset == 0)
    {
        if (checksumSection)
        {
            pHTable->checksumWordOffset = (uint32_t)(checksumSection->Address / sizeof(uint32_t));
        }
        else
        {
            pHTable->checksumWordOffset = 0;
        }
    }
}

/******************************************************************************/
void ZynqPartitionHeader::SetImageHeaderOffset(uint32_t offset)
{
    pHTable->imageHeaderWordOffset = offset / sizeof(uint32_t);
}

/******************************************************************************/
void ZynqPartitionHeader::SetAuthCertificateOffset(void)
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
        if (acs && acs->section) {
            pHTable->authCertificateOffset = (uint32_t)(acs->section->Address / sizeof(uint32_t));
        }
    }
    else if (imageHeader->GetAuthenticationType() == Authentication::RSA)
    {
        /* If the image is not yet signed, partition addr + partition length - cert size */
        AuthenticationContext::SetAuthenticationKeyLength(RSA_2048_KEY_LENGTH);
        pHTable->authCertificateOffset = (uint32_t)((partition->section->Address + partition->section->Length - sizeof(AuthCertificate2048Structure)) / sizeof(uint32_t));
    }
    else
    {
        /* For unauthenticated cases */
        pHTable->authCertificateOffset = 0;
    }
}

/******************************************************************************/
void ZynqPartitionHeader::SetReserved(void)
{
    for (uint8_t i = 0; i<PHT_RESERVED; i++)
    {
        pHTable->reserved[i] = 0x0;
    }
}

/******************************************************************************/
void ZynqPartitionHeader::SetChecksum(void)
{
    pHTable->pHChecksum = ComputeWordChecksum(pHTable, sizeof(ZynqPartitionHeaderTableStructure) - sizeof(uint32_t));
}

/******************************************************************************/
void ZynqPartitionHeader::RealignSectionDataPtr(void)
{
    pHTable = (ZynqPartitionHeaderTableStructure*)section->Data;
}

/******************************************************************************/
uint32_t ZynqPartitionHeader::GetPartitionHeaderSize(void)
{
    return sizeof(ZynqPartitionHeaderTableStructure);
}

/******************************************************************************/
uint32_t ZynqPartitionHeader::GetChecksum(void)
{
    return pHTable->pHChecksum;
}

/******************************************************************************/
uint32_t ZynqPartitionHeader::GetExecAddress(void)
{
    return pHTable->destinationExecAddress;
}

/******************************************************************************/
uint32_t ZynqPartitionHeader::GetLoadAddress(void)
{
    return pHTable->destinationLoadAddress;
}

/******************************************************************************/
uint32_t ZynqPartitionHeader::GetEncryptedPartitionLength(void)
{
    return pHTable->encryptedPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t ZynqPartitionHeader::GetUnencryptedPartitionLength(void)
{
    return pHTable->unencryptedPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t ZynqPartitionHeader::GetTotalPartitionLength(void)
{
    return pHTable->totalPartitionLength * sizeof(uint32_t);
}

/******************************************************************************/
uint32_t ZynqPartitionHeader::GetAuthCertificateOffset(void)
{
    return (pHTable->authCertificateOffset * sizeof(uint32_t));
}

/******************************************************************************/
uint32_t ZynqPartitionHeader::GetPartitionWordOffset(void)
{
    return (pHTable->partitionWordOffset * sizeof(uint32_t));
}

/******************************************************************************/
uint8_t ZynqPartitionHeader::GetDestinationDevice(void)
{
    return ((pHTable->partitionAttributes >> PH_DEST_DEVICE_SHIFT) & PH_DEST_DEVICE_MASK);
}

/******************************************************************************/
uint8_t ZynqPartitionHeader::GetHeadAlignment(void)
{
    return ((pHTable->partitionAttributes >> PH_HEAD_ALIGN_SHIFT) & PH_HEAD_ALIGN_MASK);
}

/******************************************************************************/
uint8_t ZynqPartitionHeader::GetTailAlignment(void)
{
    return ((pHTable->partitionAttributes >> PH_TAIL_ALIGN_SHIFT) & PH_TAIL_ALIGN_MASK);
}

/******************************************************************************/
uint8_t ZynqPartitionHeader::GetAuthCertFlag(void)
{
    return ((pHTable->partitionAttributes >> PH_AC_FLAG_SHIFT) & PH_AC_FLAG_MASK);
}

/******************************************************************************/
uint8_t ZynqPartitionHeader::GetChecksumType(void)
{
    return ((pHTable->partitionAttributes >> PH_CHECKSUM_SHIFT) & PH_CHECKSUM_MASK);
}

/******************************************************************************/
uint8_t ZynqPartitionHeader::GetOwnerType(void)
{
    return ((pHTable->partitionAttributes >> PH_OWNER_SHIFT) & PH_OWNER_MASK);
}

/******************************************************************************/
void ZynqPartitionHeader::SetBitLoadAddress(bool load, uint32_t val)
{
    if (load)
    {
        loadAddress = val;
    }
    else
    {
        loadAddress = 0;
    }
}
