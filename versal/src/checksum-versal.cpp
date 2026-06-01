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

#include <memory>
#include "checksum-versal.h"
#include "bootimage.h"
#include "binary.h"
#include "Keccak-compact-versal.h"
#ifdef USE_ISE_OPENSSL
#include "OpenSSL/export/opensslconf.h"
#include "OpenSSL/export/md5.h"
#else
#include <openssl/opensslconf.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#endif

/******************************************************************************/
Section* VersalSHA3ChecksumContext::Build(Binary& cache, const uint8_t* data, const Binary::Length_t length)
{
    auto section = std::make_unique<Section>("sha3checksum", Size());
    Section* section_ptr = section.get();
    if (section != nullptr)
    {
        cache.Sections.push_back(std::move(section));
    }
    return section_ptr;
}


/******************************************************************************/
Section* VersalSHA3ChecksumContext::Build(std::string name)
{
    auto section = std::make_unique<Section>(name + ".sha3checksum", Size());

    return section.release();  // Transfer ownership to caller
}


/******************************************************************************/
void VersalSHA3ChecksumContext::Link(bool bootloader, const uint8_t* data, const Binary::Length_t length, Section* section) {
    uint32_t size = Size();
    auto cksum_data = std::make_unique<uint8_t[]>(size);
    Versalcrypto_hash(cksum_data.get(), data, length, !bootloader);

    if (bootloader)
    {
        memcpy(section->Data.get() + length, cksum_data.get(), size);
    }
    else
    {
        memcpy(section->Data.get(), cksum_data.get(), size);
    }
}


/******************************************************************************/
void VersalChecksumTable::Build(BootImage& bi, Binary& cache)
{
    // allocate space for checksum data
    for (std::list<ImageHeader*>::iterator i = bi.imageList.begin(); i != bi.imageList.end(); i++)
    {
        ImageHeader* hdr = *i;
        std::list<PartitionHeader*> pHList = hdr->GetPartitionHeaderList();
        for (std::list<PartitionHeader*>::iterator j = pHList.begin(); j != pHList.end(); j++)
        {
            PartitionHeader& partHdr(**j);
            /* Do not calculate checksum on entire partition for VersalNet bootloader - hashing is calculated on the first chunk + hash of previous chunk in VersalNet
            This is handled while doing the chunking of the partition. So need need to calculate again */
            if (partHdr.IsBootloader())
            {
                if (!bi.options.IsVersalNetSeries())
                {
                    // For FSBL, checksum should be like Bootimage Integrity, the checksum should be attached right at the end of the FSBL
                    partHdr.partition->section->IncreaseLengthAndPadTo(partHdr.partition->section->Length + hdr->GetChecksumContext()->Size(), 0);
                }
            }
            else
            {
                    // For other partitions, a new checksum section is created and added at the end of image.
                    Section* s = hdr->GetChecksumContext()->Build(partHdr.partition->section->Name);
                    partHdr.checksumSection = s;
            }
        }
    }
}


/******************************************************************************/
void VersalChecksumTable::Link(BootImage& bi)
{
    for (std::list<ImageHeader*>::iterator i = bi.imageList.begin(); i != bi.imageList.end(); i++)
    {
        ImageHeader* hdr = *i;
        if (hdr->GetChecksumContext() == NULL)
        {
            continue;
        }
        std::list<PartitionHeader*> pHList = hdr->GetPartitionHeaderList();
        for (std::list<PartitionHeader*>::iterator j = pHList.begin(); j != pHList.end(); j++)
        {
            PartitionHeader& partHdr(**j);
            if (partHdr.partition == NULL || partHdr.partition->section == NULL)
            {
                continue;
            }
            if (partHdr.IsBootloader())
            {
                if (!bi.options.IsVersalNetSeries())
                {
                    hdr->GetChecksumContext()->Link(partHdr.IsBootloader(), partHdr.partition->section->Data.get(), partHdr.partition->section->Length - hdr->GetChecksumContext()->Size(), partHdr.partition->section);
                }
            }
            else
            {
                for (std::list<AuthenticationCertificate*>::iterator acs = partHdr.ac.begin(); acs != partHdr.ac.end(); acs++)
                {
                    uint32_t len = 0;
                    if ((*acs) && (*acs)->section)
                    {
                        len = partHdr.partition->section->Length + (*acs)->section->Length;
                        auto buffer = std::make_unique<uint8_t[]>(len);
                        memcpy(buffer.get(), partHdr.partition->section->Data.get(), partHdr.partition->section->Length);
                        memcpy(buffer.get() + partHdr.partition->section->Length, (*acs)->section->Data.get(), (*acs)->section->Length);
                        hdr->GetChecksumContext()->Link(partHdr.IsBootloader(), buffer.get(), len, partHdr.checksumSection);
                    }
                    else
                    {
                        if (partHdr.firstChunkSize != 0)
                        {
                            len = partHdr.firstChunkSize + SHA3_LENGTH_BYTES;
                        }
                        else
                        {
                            len = partHdr.partition->section->Length;
                        }
                        hdr->GetChecksumContext()->Link(partHdr.IsBootloader(), partHdr.partition->section->Data.get(), len, partHdr.checksumSection);
                    }
                }
            }
        }
    }
}
