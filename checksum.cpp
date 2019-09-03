/******************************************************************************
* Copyright 2015-2019 Xilinx, Inc.
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
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "bootimage.h"
#include "checksum.h"
#include "binary.h"
#include "Keccak-compact.h"

#ifdef USE_ISE_OPENSSL
  #include "OpenSSL/export/opensslconf.h"
  #include "OpenSSL/export/md5.h"
#else
  #include <openssl/opensslconf.h>
  #include <openssl/md5.h>
  #include <openssl/sha.h>
#endif


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
Section* MD5ChecksumContext::Build(Binary& cache, const uint8_t* data, const Binary::Length_t length) 
{
    Section* section = new Section("md5checksum", Size());
    if(section != NULL)
    {
        cache.Sections.push_back(section);
    }
    return section;
}

/******************************************************************************/
void MD5ChecksumContext::Link(bool bootloader, const uint8_t* data, const Binary::Length_t length, Section* section) 
{
    uint32_t size = Size();
    uint8_t* cksum_data = new uint8_t [size];
    MD5( data, length, cksum_data);

    if(bootloader)
    {
        memcpy(section->Data + length, cksum_data, size);    
    }
    else
    {
        memcpy(section->Data, cksum_data, size);
    }
    delete [] cksum_data;
}

/******************************************************************************/
Section* SHA2ChecksumContext::Build(Binary& cache, const uint8_t* data, const Binary::Length_t length) 
{
    Section* section = new Section("sha2checksum", Size());
    if(section != NULL)
    {
        cache.Sections.push_back(section);
    }
    return section;
}

/******************************************************************************/
void SHA2ChecksumContext::Link(bool bootloader, const uint8_t* data, const Binary::Length_t length, Section* section) 
{
    uint32_t size = Size();
    uint8_t* cksum_data = new uint8_t [size];
    SHA256(data, length, cksum_data);

    if(bootloader)
    {
        memcpy(section->Data + length, cksum_data, size);    
    }
    else
    {
        memcpy(section->Data, cksum_data, size);
    }
    delete [] cksum_data;
}

/******************************************************************************/
Section* SHA3ChecksumContext::Build(Binary& cache, const uint8_t* data, const Binary::Length_t length) 
{
    Section* section = new Section("sha3checksum", Size());
    if(section != NULL)
    {
        cache.Sections.push_back(section);
    }
    return section;
}

/******************************************************************************/
void SHA3ChecksumContext::Link(bool bootloader, const uint8_t* data, const Binary::Length_t length, Section* section) 
{
    uint32_t size = Size();
    uint8_t* cksum_data = new uint8_t [size];
    if(bootloader)
    {
        crypto_hash(cksum_data, data, length);
        memcpy(section->Data + length, cksum_data, size);    
    }
    else
    {
        crypto_hash_NIST_SHA3(cksum_data, data, length);
        memcpy(section->Data, cksum_data, size);
    }
    delete [] cksum_data;
}

/******************************************************************************/
void ChecksumTable::Build(BootImage& bi, Binary& cache) 
{
    for(std::list<ImageHeader*>::iterator i = bi.imageList.begin(); i!= bi.imageList.end(); i++)
    {
        ImageHeader hdr(**i);
        std::list<PartitionHeader*> pHList = hdr.GetPartitionHeaderList();
        for(std::list<PartitionHeader*>::iterator j = pHList.begin(); j != pHList.end(); j++)
        {
            PartitionHeader& partHdr(**j);
            if(partHdr.IsBootloader())
            {
                /* For FSBL, checksum should be like Bootimage Integrity, the checksum should be attached right at the end of the FSBL */
                partHdr.partition->section->IncreaseLengthAndPadTo(partHdr.partition->section->Length + hdr.GetChecksumContext()->Size(), 0);
            }
            else 
            {
                /* For other partitions, a new checksum section is created and added at the end of image. */
                Section* s = hdr.GetChecksumContext()->Build(cache, partHdr.partition->section->Data, partHdr.partition->section->Length);
                partHdr.checksumSection = s;
            }
        }
    }
}

/******************************************************************************/
void ChecksumTable::Link(BootImage& bi) 
{
    for(std::list<ImageHeader*>::iterator i = bi.imageList.begin(); i!= bi.imageList.end(); i++) 
    {
        ImageHeader hdr(**i);
        std::list<PartitionHeader*> pHList = hdr.GetPartitionHeaderList();
        for(std::list<PartitionHeader*>::iterator j = pHList.begin(); j != pHList.end(); j++) 
        {
            PartitionHeader& partHdr(**j);
            if(partHdr.IsBootloader())
            {
                hdr.GetChecksumContext()->Link(partHdr.IsBootloader(), partHdr.partition->section->Data, partHdr.partition->section->Length - hdr.GetChecksumContext()->Size(), partHdr.partition->section);
                
            }
            else 
            {
                for (std::list<AuthenticationCertificate*>::iterator acs = partHdr.ac.begin(); acs != partHdr.ac.end(); acs++)
                {
                    if ((*acs) && (*acs)->section)
                    {
                        uint32_t len = partHdr.partition->section->Length + (*acs)->section->Length;
                        uint8_t* buffer = new uint8_t[len];
                        memcpy(buffer, partHdr.partition->section->Data, partHdr.partition->section->Length);
                        memcpy(buffer + partHdr.partition->section->Length, (*acs)->section->Data, (*acs)->section->Length);
                        hdr.GetChecksumContext()->Link(partHdr.IsBootloader(), buffer, len, partHdr.checksumSection);
                        delete[] buffer;
                    }
                    else
                    {
                        hdr.GetChecksumContext()->Link(partHdr.IsBootloader(), partHdr.partition->section->Data, partHdr.partition->section->Length, partHdr.checksumSection);
                    }
                }
            }
        }
    }
}
