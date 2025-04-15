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

#include "checksum-versal_2ve_2vm.h"
#include "bootimage.h"
#include "binary.h"
#include "Keccak-compact-versal_2ve_2vm.h"
#include "encryptutils.h"
#ifdef USE_ISE_OPENSSL
#include "OpenSSL/export/opensslconf.h"
#include "OpenSSL/export/md5.h"
#else
#include <openssl/opensslconf.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#endif

/******************************************************************************/
Section* Versal_2ve_2vmSHA3ChecksumContext::Build(Binary& cache, const uint8_t* data, const Binary::Length_t length)
{
    Section* section = new Section("sha3checksum", Size());
    if (section != NULL)
    {
        cache.Sections.push_back(section);
    }
    return section;
}


/******************************************************************************/
Section* Versal_2ve_2vmSHA3ChecksumContext::Build(std::string name)
{
    Section* section = new Section(name + ".sha3checksum", Size());

    return section;
}


/******************************************************************************/
void Versal_2ve_2vmSHA3ChecksumContext::Link(bool bootloader, const uint8_t* data, const Binary::Length_t length, Section* section) {
    uint32_t size = Size();
    uint8_t* cksum_data = new uint8_t[size];
    Versal_2ve_2vmcrypto_hash(cksum_data, data, length, !bootloader);

    if (bootloader)
    {
        memcpy(section->Data + length, cksum_data, size);
    }
    else
    {
        memcpy(section->Data, cksum_data, size);
    }
    delete[] cksum_data;
}


/******************************************************************************/
void Versal_2ve_2vmChecksumTable::Build(BootImage& bi, Binary& cache)
{
    // allocate space for checksum data
    for (std::list<ImageHeader*>::iterator i = bi.imageList.begin(); i != bi.imageList.end(); i++)
    {
        ImageHeader hdr(**i);
        std::list<PartitionHeader*> pHList = hdr.GetPartitionHeaderList();
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
                    partHdr.partition->section->IncreaseLengthAndPadTo(partHdr.partition->section->Length + hdr.GetChecksumContext()->Size(), 0);
                }
            }
            else
            {
                    // For other partitions, a new checksum section is created and added at the end of image.
                    Section* s = hdr.GetChecksumContext()->Build(partHdr.partition->section->Name);
                    partHdr.checksumSection = s;
            }
        }
    }
}


/******************************************************************************/
void Versal_2ve_2vmChecksumTable::Link(BootImage& bi)
{
    for (std::list<ImageHeader*>::iterator i = bi.imageList.begin(); i != bi.imageList.end(); i++)
    {
        ImageHeader hdr(**i);
        std::list<PartitionHeader*> pHList = hdr.GetPartitionHeaderList();
        for (std::list<PartitionHeader*>::iterator j = pHList.begin(); j != pHList.end(); j++)
        {
            PartitionHeader& partHdr(**j);
            /* Do not calculate checksum on entire partition for VersalNet bootloader - hashing is calculated on the first chunk + hash of previous chunk in VersalNet
               This is handled while doing the chunking of the partition. So need need to calculate again */
            if (partHdr.IsBootloader())
            {
                //LOG_TRACE("Calculating the Boot Header Hash");
                /* Donot include SMAP data to calculate BH hash */
                uint8_t* tmpBh = bi.bootHeader->section->Data + 0x10;
                uint8_t* sha_hash = new uint8_t[bi.hash->GetHashLength()];
                bi.hash->CalculateVersalHash(true, tmpBh, bi.bootHeader->section->Length - 0x10, sha_hash);
                //LOG_TRACE("BH checksum");
                //LOG_DUMP_BYTES(tmpBh, bi.bootHeader->GetBootHeaderSize() - 0x10);
                //LOG_TRACE("hash");
                //LOG_DUMP_BYTES(sha_hash, bi.hash->GetHashLength());
                if (bi.partitionHeaderList.front()->imageHeader->GetEncryptContext()->Type() != Encryption::None)
                {
                    //do Nothing, Already hash and GCM Tag are copied.
                }
                else
                {
                    memcpy(bi.partitionHeaderList.front()->partition->section->Data + HASH_BLOCK_INDEX_BYTES, sha_hash, bi.hash->GetHashLength());
                }
                delete[] sha_hash;
            }
            else
            {
                for (std::list<AuthenticationCertificate*>::iterator acs = partHdr.ac.begin(); acs != partHdr.ac.end(); acs++)
                {
                    uint32_t len = 0;
                    if ((*acs) && (*acs)->section)
                    {
                        len = partHdr.partition->section->Length + (*acs)->section->Length;
                        uint8_t* buffer = new uint8_t[len];
                        memcpy(buffer, partHdr.partition->section->Data, partHdr.partition->section->Length);
                        memcpy(buffer + partHdr.partition->section->Length, (*acs)->section->Data, (*acs)->section->Length);
                        hdr.GetChecksumContext()->Link(partHdr.IsBootloader(), buffer, len, partHdr.checksumSection);
                        delete[] buffer;
                    }
                    else
                    {
                        if (partHdr.firstChunkSize != 0)
                        {
                            len = partHdr.firstChunkSize + bi.hash->GetHashLength();
                        }
                        else
                        {
                            len = partHdr.partition->section->Length;
                        }
                        hdr.GetChecksumContext()->Link(partHdr.IsBootloader(), partHdr.partition->section->Data, len, partHdr.checksumSection);
                    }
                }
            }
        }
    }
}

