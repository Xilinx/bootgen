
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
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstring>
#include <cstdint>
#include <algorithm>

#include "readimage-versal_2ve_2vm.h"
#include "authentication-versal_2ve_2vm.h"

#define SEPARATOR "--------------------------------------------------------------------------------"

Authentication::Type bl_auth_type_versal_2ve_2vm = Authentication::None;
/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
void Versal_2ve_2vmReadImage::Separator(void)
{
#ifdef SEPARATOR
    std::cout << SEPARATOR << std::endl;
#else
    std::cout << std::endl;
#endif
}

/******************************************************************************/
Versal_2ve_2vmReadImage::~Versal_2ve_2vmReadImage()
{
    // Cleanup handled by unique_ptr for bH, iH, iHT, pHT
    
    // Clean up iHs list (wrap in smart pointer for safe deletion)
    for (std::list<Versal_2ve_2vmImageHeaderStructure*>::iterator it = iHs.begin(); it != iHs.end(); ++it)
    {
        std::unique_ptr<Versal_2ve_2vmImageHeaderStructure> iHPtr(*it);  // Wrap for auto-delete
    }
    iHs.clear();
    
    // Clean up pHTs list (wrap in smart pointer for safe deletion)
    for (std::list<Versal_2ve_2vmPartitionHeaderTableStructure*>::iterator it = pHTs.begin(); it != pHTs.end(); ++it)
    {
        std::unique_ptr<Versal_2ve_2vmPartitionHeaderTableStructure> pHTPtr(*it);  // Wrap for auto-delete
    }
    pHTs.clear();
    
    // Clean up aCs list
    for (std::list<uint8_t*>::iterator it = aCs.begin(); it != aCs.end(); ++it)
    {
        delete[] *it;  // These are arrays
    }
    aCs.clear();
}
void Versal_2ve_2vmReadImage::DumpPlainPartition(uint8_t *buffer,uint32_t length, std::string partition_name, bool isBootloader, uint32_t id, uint32_t index)
{
    auto outputBuffer = std::make_unique<uint8_t[]>(length);
    size_t outputIndex = 0;
    size_t inputSize = length;
    size_t hashSize = 0;
    size_t chunksize = ChunkSizeforParitition(partition_name, isBootloader);
	if(bH)
	{
		hashSize = GetHashType(bH->authHeader1);
	}
	else 
	{
		hashSize = GetHashType(iHT->authHeader1);
	}
    for (size_t i = 0; i < inputSize; i += chunksize)
    {
        size_t currentChunkSize = std::min(chunksize, inputSize - i);
        if (i + chunksize < inputSize)
        { // Not the last chunk
            size_t bytesToCopy = currentChunkSize - hashSize;
            std::memcpy(outputBuffer.get() + outputIndex, buffer + i, bytesToCopy);
            outputIndex += bytesToCopy;
        }
        else
        { // Last chunk
            std::memcpy(outputBuffer.get() + outputIndex, buffer + i, currentChunkSize);
            outputIndex += currentChunkSize;
        }
    }
    DumpPartitions(outputBuffer.get(), outputIndex, partition_name, id, index);
}

/**********************************************************************************************/
void Versal_2ve_2vmReadImage::ReadPartitions()
{
    size_t result;
    uint64_t offset = 0;
    uint32_t cnt_index = 0;
    FILE *binFile = fopen(binFilename.c_str(), "rb");

    if (!binFile)
    {
        LOG_ERROR("Cannot read file %s", binFilename.c_str());
    }
   
    fseek(binFile, 0, SEEK_END);
    uint64_t fileSize = ftell(binFile);

    std::list<Versal_2ve_2vmPartitionHeaderTableStructure*>::iterator pHT = pHTs.begin();
    for (std::list<Versal_2ve_2vmImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
    {
        uint32_t part_sec_index = 0;
        uint32_t part_sec_count = 0;
        uint32_t part_index = 0;

        for (cnt_index = 0; cnt_index < (*iH)->dataSectionCount; cnt_index++)
        {

            uint32_t length = 0; 
            std::unique_ptr<uint8_t[]> buffer;  // Smart pointer for auto cleanup
            uint32_t id = (*pHT)->puid & 0xFFFF; (void)id; // Suppress unused warning
			bool isBootloader = false; 
            if(bH && iH == iHs.begin())    // TODO FIX: WA added to populate plm offset from BH instead of paritition header
            {
                offset = bH->sourceOffset;
                length = bH->totalPlmLength + bH->totalPmcCdoLength;
            }
            else       
            {
                offset = (*pHT)->partitionWordOffset * 4;  
                length = (*pHT)->totalPartitionLength * 4; 
            }
            if (offset + length > fileSize)
            {
                length = (uint32_t)(fileSize - offset);
            }
            buffer = std::make_unique<uint8_t[]>(length);  // Allocate directly as unique_ptr
            const uint32_t bufferSize = length;
                
            if ((*pHT)->dataSectionCount > 0)
            {
               part_sec_index = 0;
               part_sec_count = (*pHT)->dataSectionCount;
            }   
            if (((*pHT)->dataSectionCount > 0) && (cnt_index != 0))
            {
               part_index++;
            }
            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(buffer.get(), 1, length, binFile);  // Use .get() for raw pointer access
                if (result != length)
                {
                    LOG_ERROR("Error parsing partitions from PDI file");
                }
                if (dumpType == DumpOption::PARTITIONS)
                {
                    if (bH && bH->sourceOffset == offset)
                    {
                        length = bH->totalPlmLength;
                        isBootloader = true;
                    }
                    if((part_sec_count > 1) && ((*pHT)->dataSectionCount == 0))
                    {
                        part_sec_index++;
                    }
                    //DumpPartitions(buffer.get(), length, (*iH)->imageName, part_index, part_sec_index);
                    DumpPlainPartition(buffer.get(), length, (*iH)->imageName, isBootloader, part_index, part_sec_index);
                }
                else
                {
                    /* Bootloader - compare address offset from BH and PHT */
                    if (bH && bH->sourceOffset == offset)
                    {
                        length = bH->totalPlmLength;
                    }
                    if ((dumpType == DumpOption::PLM) || (dumpType == DumpOption::BOOT_FILES))
                    {
                         // DumpPartitions(buffer.get(), length, "plm", part_index, part_sec_index);
                         DumpPlainPartition(buffer.get(), length, "plm", isBootloader, part_index, part_sec_index);
                         if (dumpType == DumpOption::PLM)
                        {
                            // buffer is unique_ptr, auto-deleted when out of scope
                            fclose(binFile);
                            return;
                        }
                    }
                }
                /* For extracting PMC DATA, Bootloader - compare address offset from BH and PHT */
                if (bH && bH->sourceOffset == offset)
                {
                    if (bH && bH->totalPmcCdoLength != 0)
                    {
                        if ((dumpType == DumpOption::PARTITIONS) || (dumpType == DumpOption::PMC_CDO) || (dumpType == DumpOption::BOOT_FILES))
                        {
                            uint64_t pmcEnd = (uint64_t)bH->totalPlmLength + (uint64_t)bH->totalPmcCdoLength;
                            if (pmcEnd > (uint64_t)bufferSize)
                            {
                                LOG_ERROR("Malformed PDI - PMC CDO range exceeds partition buffer");
                            }

                            if (!(fseek(binFile, bH->sourceOffset + bH->totalPlmLength, SEEK_SET)))
                            {
                                  result = fread(buffer.get(), 1, bH->totalPmcCdoLength, binFile);  // Use .get()
                            }
                            if (result != bH->totalPmcCdoLength)
                            {
                                LOG_ERROR("Error parsing PMC CDO from PDI file");
                            }

                            //DumpPartitions(buffer.get() + bH->totalPlmLength, bH->totalPmcCdoLength, "pmc_cdo");
                            DumpPlainPartition(buffer.get() + bH->totalPlmLength, bH->totalPmcCdoLength, "pmc_cdo");
                            if ((dumpType == DumpOption::PMC_CDO) || (dumpType == DumpOption::BOOT_FILES))
                            {
                                // buffer is unique_ptr, auto-deleted when out of scope
                                fclose(binFile);
                                return;
                            }
                        }
                    }
                    if (bH && bH->totalPmcCdoLength == 0 && dumpType == DumpOption::PMC_CDO)
                    {
                        LOG_ERROR("PMC_CDO partition is not available in the PDI");
                    }
                }
            }
            else
            {
                LOG_ERROR("Error parsing Partition Headers from bin file");
            }
            pHT++;
            // buffer is unique_ptr, auto-deleted at end of loop iteration
        }
    }
    fclose(binFile);
}
/******************************************************************************/
uint32_t Versal_2ve_2vmReadImage::ChunkSizeforParitition(std::string partition_name,  bool isBootloader)
{
	char * env_SECURE_4K_CHUNK = getenv("SECURE_4K_CHUNK"); (void)env_SECURE_4K_CHUNK; // Suppress unused warning
	if(partition_name == "plm" || partition_name == "pmc_cdo" || isBootloader == true)
		isBootloader = true;

	if (isBootloader == true)
    {
		return SECURE_16K_CHUNK;
    }
    else
    {
        return SECURE_32K_CHUNK;
    }
}

/******************************************************************************/
uint32_t Versal_2ve_2vmReadImage::GetHashType(uint32_t authheader)
{
	    uint32_t hashtype;

        switch ((authheader & 0xF0) >> 4)
        {
            case 0 : hashtype = SHA2_LENGTH_BYTES;	
                        break;
            case 1 : hashtype = SHA3_LENGTH_BYTES;
                        break;
            default:
                    hashtype = 0;
                    break;
        }
		return hashtype;
}

/******************************************************************************/

/******************************************************************************/
uint32_t Versal_2ve_2vmReadImage:: IdentifyAuthtype(uint32_t authheader)
{
        switch (authheader & 0x0F)
        {
                case 1: bl_auth_type_versal_2ve_2vm = Authentication::RSA; break;
                case 2: bl_auth_type_versal_2ve_2vm = Authentication::ECDSA; break;
                case 3: bl_auth_type_versal_2ve_2vm = Authentication::ECDSAp521; break;
                case 4: bl_auth_type_versal_2ve_2vm = Authentication::LMS_SHA2_256; break;
                case 5: bl_auth_type_versal_2ve_2vm = Authentication::LMS_SHAKE256; break;
                case 6: bl_auth_type_versal_2ve_2vm = Authentication::HSS_SHA2_256; break;
                case 7: bl_auth_type_versal_2ve_2vm = Authentication::HSS_SHAKE256; break;
                default : bl_auth_type_versal_2ve_2vm = Authentication::None; break;
        }
   
   return bl_auth_type_versal_2ve_2vm;
}


/******************************************************************************/
uint32_t Versal_2ve_2vmReadImage::GetACLength(uint32_t AuthOffset, uint32_t ppksize)
{
    spkheaderstructure* spkheader  = NULL;
    uint32_t Acsize = 0;
    size_t result;
    FILE *binFile;
    binFile = fopen(binFilename.c_str(), "rb");

    if (!binFile)
    {
        LOG_ERROR("Cannot read file %s", binFilename.c_str());
    }

    auto AC_spkheader = std::make_unique<uint8_t[]>(TELLURIDE_AC_SPK_HDR_LENGTH);
    memset(AC_spkheader.get(), 0, TELLURIDE_AC_SPK_HDR_LENGTH);
    if (!(fseek(binFile, AuthOffset + ppksize, SEEK_SET)))
    {
        result = fread(AC_spkheader.get(), 1, TELLURIDE_AC_SPK_HDR_LENGTH, binFile);
        if (result != TELLURIDE_AC_SPK_HDR_LENGTH)
        {
            LOG_ERROR("Error parsing extracting SPK header from PDI file");
        }
    }
    spkheader = (spkheaderstructure*)AC_spkheader.get();
    Acsize = ppksize + TELLURIDE_AC_SPK_HDR_LENGTH + spkheader->acTotalSpkSize + spkheader->acSpkTotalSignatureSize;
    fclose(binFile);
    return Acsize;
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::ReadHeaderTableDetails()
{
    size_t result;
    uint64_t offset = 0;
    uint32_t index = 0;
    bool smap_header_found = false;
	uint32_t Aclength = 0;
    FILE *binFile;
    binFile = fopen(binFilename.c_str(), "rb");

    if (!binFile)
    {
        LOG_ERROR("Cannot read file %s", binFilename.c_str());
    }

    // Boot Header Table Extraction
    bH = std::make_unique<Versal_2ve_2vmBootHeaderStructure>();
    result = fread(bH.get(), 1, sizeof(Versal_2ve_2vmBootHeaderStructure), binFile);
    if ((bH->smapWords[0] == 0xDD000000) || (bH->smapWords[0] == 0x00DD0000) || (bH->smapWords[0] == 0x000000DD))
    {
        smap_header_found = true;
    }

    if(bH->widthDetectionWord != 0xAA995566)
    {
        bH.reset();
    }


    if ((dumpType == DumpOption::BH) || (dumpType == DumpOption::BOOT_FILES))
    {
        if (bH == NULL)
        {
            LOG_ERROR("Boot Header not found");
        }
        FILE* filePtr;
        std::string fName = binFilename;
        if (dumpPath != "")
        {
            fName = dumpPath + "/" + StringUtils::BaseName(binFilename);
        }
        fName = StringUtils::RemoveExtension(fName);
        fName += "_bh.bin";
        filePtr = fopen(fName.c_str(), "wb");
        if (filePtr != NULL)
        {
            /* First 16 bytes are not required by BootRoM/PLM, they should not be part of BH */
            result = fwrite(&bH->widthDetectionWord, 1, size_t(sizeof(Versal_2ve_2vmBootHeaderStructure) - 16), filePtr);
            if (result != sizeof(Versal_2ve_2vmBootHeaderStructure) - 16)
            {
                LOG_ERROR("Error dumping Boot Header to a file");
            }
            fclose(filePtr);
            LOG_INFO("%s generated successfully", StringUtils::BaseName(fName).c_str());
        }
    }

    if (bH)
	{
        if(IdentifyAuthtype(bH->authHeader1) != Authentication::None)
        {
            // extracting SPK header for PLM
            Aclength = GetACLength(sizeof(Versal_2ve_2vmBootHeaderStructure), bH->totalppkkSize1);     //Use ACLength only if Auth is enable
            auto plm_ac = std::make_unique<uint8_t[]>(Aclength);
            if (!(fseek(binFile, sizeof(Versal_2ve_2vmBootHeaderStructure), SEEK_SET)))
            {
                result = fread(plm_ac.get(), 1, Aclength, binFile);
                if (result != Aclength)
                {
                    LOG_ERROR("Error parsing Authentication Certificates for PLM from PDI file");
                }
            }
            aCs.push_back(plm_ac.release());
            authtype.push_back(IdentifyAuthtype(bH->authHeader1));
        }
        else
        {
            aCs.push_back(NULL);
            authtype.push_back(Authentication::None);
        } 
    }
// Hash block0 record
    if(bH && IdentifyAuthtype(bH->authHeader1) != Authentication::None)
        offset = sizeof(Versal_2ve_2vmBootHeaderStructure) + Aclength;
     else 
        offset = sizeof(Versal_2ve_2vmBootHeaderStructure);
	
    // uint8_t* hash_block = NULL;  // Unused variable - commented out 
    if(bH && offset != 0)
    {
        auto hash_block = std::make_unique<uint8_t[]>(bH->hashBlockLength1);
        if (!(fseek(binFile, offset, SEEK_SET)))
        {
            result = fread(hash_block.get(), 1, bH->hashBlockLength1, binFile);
            if (result != bH->hashBlockLength1)
            {
                LOG_ERROR("Error parsing Hash Block0 from PDI file");
            }
        }
        uint8_t* hash_block_ptr = hash_block.release();
        Hashblock_record.push_back(std::make_pair(hash_block_ptr,bH->hashBlockLength1)); 
        DumpPartitions(hash_block_ptr, bH->hashBlockLength1, "HashBlock0"); // dumping hashblock0
    }



// Hash Block0 signture 
    if(bH)
        offset = offset + bH->hashBlockLength1;
    if(bH && offset != 0)
    {
        auto hash_block2 = std::make_unique<uint8_t[]>(bH->totalSignatureSize1);
        if (!(fseek(binFile, offset, SEEK_SET)))
        {
            result = fread(hash_block2.get(), 1, bH->totalSignatureSize1, binFile);
            if (result != bH->totalSignatureSize1)
            {
                LOG_ERROR("Error parsing Hash Block0 sign from PDI file");
            }
        }
        Hashblock_record.push_back(std::make_pair(hash_block2.release(),bH->totalSignatureSize1));
    }
  

    if (bH)
    {
        offset = bH->imageHeaderByteOffset;
    }
    else
    {
        if(smap_header_found)
        {
            offset = sizeof(Versal_2ve_2vmSmapWidthTable);
        }
        else
        {
            offset = 0;
        }
    }


    if (!(fseek(binFile, offset, SEEK_SET)))
    {
        iHT = std::make_unique<Versal_2ve_2vmImageHeaderTableStructure>();
        result = fread(iHT.get(), 1, sizeof(Versal_2ve_2vmImageHeaderTableStructure), binFile);
        if (iHT->version != VERSION_v1_00_TELLURIDE)
        {
            LOG_ERROR("Improper version (0x%.8x) read from Image Header Table of the PDI file.", iHT->version);
        }

        if (result != sizeof(Versal_2ve_2vmImageHeaderTableStructure))
        {
            LOG_ERROR("Error parsing Image Header Table from PDI file");
        }
        if (!((iHT->partitionTotalCount > 0) && (iHT->partitionTotalCount < 0xFF)))
        {
            LOG_ERROR("Number of partitions read from PDI is more than number of supported partititon count.");
        }
    }
    else
    {
        LOG_ERROR("Error parsing Image Header Table from PDI file");
    }


    // uint8_t*  header_ac = NULL;  // Unused variable - commented out
    if (iHT && iHT->headerAuthCertificateWordOffset != 0 && (IdentifyAuthtype(iHT->authHeader1) != Authentication::None))
    {
        // Recording AC for meta header
            offset = iHT->headerAuthCertificateWordOffset * 4 ;
            Aclength = GetACLength(iHT->headerAuthCertificateWordOffset * 4, iHT->totalppkkSize1);     //Use ACLength only if Auth is enable
            auto header_ac = std::make_unique<uint8_t[]>(Aclength);
            if (!(fseek(binFile, offset, SEEK_SET)))  
            {
                result = fread(header_ac.get(), 1, Aclength, binFile);
                if (result != Aclength)
                {
                    LOG_ERROR("Error parsing Header Authentication Certificate from PDI file");
                }
            }
            aCs.push_back(header_ac.release());  
            authtype.push_back(IdentifyAuthtype(iHT->authHeader1));
    }
    else 
    {
        aCs.push_back(NULL);  
    }
         //Record hashblock1
       if((iHT->hashBlockLength1)*4 != 0)
       {
            auto header_ac2 = std::make_unique<uint8_t[]>((iHT->hashBlockLength1)*4);
            if (!(fseek(binFile, (iHT->hashBlockOffset) * 4, SEEK_SET)))  
            {
                result = fread(header_ac2.get(), 1, (iHT->hashBlockLength1)*4, binFile);
                if (result != (iHT->hashBlockLength1)*4)
                {
                    LOG_ERROR("Error parsing record hashblock1 from PDI file");
                }
            }  
            Hashblock_record.push_back(std::make_pair(header_ac2.release(),(iHT->hashBlockLength1)*4));
       }
       
        //Record hashblock1 signture 
        offset = (iHT->hashBlockOffset)*4  + (iHT->hashBlockLength1)*4;
        if(offset != 0)
        {
            auto hash_block3 = std::make_unique<uint8_t[]>(iHT->totalHashBlockSignatureSize1);
            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(hash_block3.get(), 1, iHT->totalHashBlockSignatureSize1, binFile);
                if (result != iHT->totalHashBlockSignatureSize1)
                {
                    LOG_ERROR("Error parsing Hash Block1 sign from PDI file");
                }
            }
            Hashblock_record.push_back(std::make_pair(hash_block3.release(),iHT->totalHashBlockSignatureSize1));
        }    
        

    if (iHT->metaHdrKeySource == KeySource::None)
    {
        offset = iHT->firstImageHeaderWordOffset * 4;
        for (index = 0; index < iHT->imageTotalCount; index++)
        {
            iH = std::make_unique<Versal_2ve_2vmImageHeaderStructure>();
            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(iH.get(), 1, sizeof(Versal_2ve_2vmImageHeaderStructure), binFile);
                if (result != sizeof(Versal_2ve_2vmImageHeaderStructure))
                {
                    LOG_ERROR("Error parsing Image Headers from PDI file");
                }
                iHs.push_back(iH.release());
                offset += sizeof(Versal_2ve_2vmImageHeaderStructure);
            }
            else
            {
                LOG_ERROR("Error parsing Image Headers from PDI file");
            }
        }

        offset = (iHT->firstImageHeaderWordOffset * 4) + (sizeof(Versal_2ve_2vmImageHeaderStructure) * iHT->imageTotalCount);
        for (index = 0; index < iHT->partitionTotalCount; index++)
        {
            pHT = std::make_unique<Versal_2ve_2vmPartitionHeaderTableStructure>();

            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(pHT.get(), 1, sizeof(Versal_2ve_2vmPartitionHeaderTableStructure), binFile);
                if (result != sizeof(Versal_2ve_2vmPartitionHeaderTableStructure))
                {
                    LOG_ERROR("Error parsing Partition Headers from PDI file");
                }
                pHTs.push_back(pHT.release());
                offset += sizeof(Versal_2ve_2vmPartitionHeaderTableStructure);
            }
            else
            {
                LOG_ERROR("Error parsing Partition Headers from PDI file");
            }
        }


        for (std::list<Versal_2ve_2vmPartitionHeaderTableStructure*>::iterator partitionHdr = pHTs.begin(); partitionHdr != pHTs.end(); partitionHdr++)
        {
			 // uint8_t* aC = NULL; // Unused variable
            if(bH && partitionHdr == pHTs.begin())
                 continue;            //incrementing to skip the PLM partition
            
            if ((IdentifyAuthtype((*partitionHdr)->authHeader1) != Authentication::None))
            {              
                Aclength = GetACLength((*partitionHdr)->authCertificateOffset * 4,(*partitionHdr)->totalppkkSize1);     //Use ACLength only if Auth is enable
                offset = (*partitionHdr)->authCertificateOffset * 4;
                auto aC = std::make_unique<uint8_t[]>(Aclength);
                if (!(fseek(binFile, offset, SEEK_SET)))
                {
                    result = fread(aC.get(), 1, Aclength, binFile);
                    if (result != Aclength)
                    {
                        LOG_ERROR("Error parsing Authentication Certificates from PDI file");
                    }
                }
                aCs.push_back(aC.release());
                authtype.push_back(IdentifyAuthtype((*partitionHdr)->authHeader1));
                //delete[] aC;
               // aC = NULL;

                // record hashblock1...n 
                offset = (*partitionHdr)->hashBlockWordOffset * 4 ;
                if(offset != 0)
                {
                    auto hash_block4 = std::make_unique<uint8_t[]>(((*partitionHdr)->hashBlockLength1) * 4); // Hash block1 length with wordoffset
                    if (!(fseek(binFile, offset, SEEK_SET)))
                    {
                        result = fread(hash_block4.get(), 1, ((*partitionHdr)->hashBlockLength1) * 4, binFile);
                        if (result != ((*partitionHdr)->hashBlockLength1) * 4)
                        {
                            LOG_ERROR("Error parsing Hash Block1 from PDI file");
                        }
                    }
                    Hashblock_record.push_back(std::make_pair(hash_block4.release(), (*partitionHdr)->hashBlockLength1 * 4));
                    // delete[] hash_block;
                    // hash_block = NULL;
                }
 
               //record Hashblock1...n signture 
                offset = (*partitionHdr)->hashBlockWordOffset * 4 + (*partitionHdr)->hashBlockLength1 * 4;   //hashblock length comes along with padding for HASH block1 only
                if(offset != 0)
                {
                    auto hash_block5 = std::make_unique<uint8_t[]>((*partitionHdr)->totalHashBlockSignatureSize1);
                    if (!(fseek(binFile, offset, SEEK_SET)))
                    {
                        result = fread(hash_block5.get(), 1, (*partitionHdr)->totalHashBlockSignatureSize1, binFile);
                        if (result != (*partitionHdr)->totalHashBlockSignatureSize1)
                        {
                            LOG_ERROR("Error parsing Hash Block1 sign from PDI file");
                        }
                    }
                    Hashblock_record.push_back(std::make_pair(hash_block5.release(),(*partitionHdr)->totalHashBlockSignatureSize1));    
                    //delete[] hash_block;
                    // hash_block = NULL;   
                }                
            }
        }
    }

    fclose(binFile);
    return;
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::ReadBinaryFile(DumpOption::Type dump, std::string path)
{
    if (StringUtils::GetExtension(binFilename) == ".mcs")
    {
        LOG_ERROR("The option '-read/-dump' is not supported on mcs format file : %s", binFilename.c_str());
    }
    dumpType = dump;
    dumpPath = path;

    ReadHeaderTableDetails();
    if (dumpType == DumpOption::PARTITIONS)
    {
        DisplayImageInfo();
    }
    if (readType != ReadImageOption::NONE)
    {
        DisplayImageInfo();
        DisplayHeaderTableDetails(readType);
    }
    ReadPartitions();
}
/******************************************************************************/
uint32_t Versal_2ve_2vmReadImage::GetPdiId(void)
{
    return iHT->pdiId;
}

/******************************************************************************/
uint32_t Versal_2ve_2vmReadImage::GetParentId(void)
{
    return iHT->parentId;
}

/******************************************************************************/
uint8_t Versal_2ve_2vmReadImage::GetCreatorId(void)
{
    return ((iHT->imageHeaderTableAttributes) >> 1) & 0xF;
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::DisplayHeaderTableDetails(ReadImageOption::Type type)
{
    switch (type)
    {
    case ReadImageOption::BH:
        DisplayBootHeader();
        break;

    case ReadImageOption::IHT:
        DisplayImageHeaderTable();
        break;

    case ReadImageOption::IH:
        if (iHT->metaHdrKeySource != KeySource::None)
        {
            LOG_ERROR("Cannot read IH from an encrypted PDI.");
        }
        DisplayImageHeaders();
        break;

    case ReadImageOption::PHT:
        if (iHT->metaHdrKeySource != KeySource::None)
        {
            LOG_ERROR("Cannot read PHT from an encrypted PDI.");
        }
        DisplayPartitionHeaderTable();
        break;

    case ReadImageOption::AC:
        DisplayAuthenicationCertificates();
        break;

    default:
        DisplayBootHeader();
        DisplayImageHeaderTable();
       
        if(iHT->metaHdrKeySource == KeySource::None)
        {
                DisplayImageHeaders();
                DisplayPartitionHeaderTable();
                DisplayAuthenicationCertificates();
        }   
        else
        {
              Separator();
              LOG_MSG("\n[INFO]   : Cannot read further from an encrypted PDI.");
        }
            break;
                  
    }
    Separator();
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::DisplayImageDetails(ReadImageOption::Type type, DumpOption::Type dump, std::string dump_dir)
{
    readType = type;

    ReadBinaryFile(dump, dump_dir);
    if (dump != DumpOption::NONE)
    {
        if (iHT->metaHdrKeySource != KeySource::None)
        {
            switch (dump)
            {
            case DumpOption::BH:
                break;

            case DumpOption::PLM:
                LOG_ERROR("Cannot dump PLM from an encrypted PDI.");
                break;

            case DumpOption::PMC_CDO:
                LOG_ERROR("Cannot dump PMC_CDO from an encrypted PDI.");
                break;

            case DumpOption::PARTITIONS:
                LOG_ERROR("Cannot dump partitions from an encrypted PDI.");
                break;

            case DumpOption::BOOT_FILES:
                LOG_MSG("\n[INFO]   : Cannot dump PLM/ PMC_CDO/ partitions from an encrypted PDI.");
                break;

            default:
                break;
            }
        }
        return;
    }
}


/******************************************************************************/
void Versal_2ve_2vmReadImage::DisplayBootHeader(void)
{
    Separator();
    if (bH == NULL) 
    {
        std::cout << "   NO BOOT HEADER" << std::endl;
        return;
    }
    std::cout << "   BOOT HEADER" << std::endl;
    Separator();
    /* SMAP Vectors are not part of Boot Header - So don't display */
    //DisplaySmapVectors(); 
    DisplayValue("width_detection (0x10) : ", bH->widthDetectionWord);
    DisplayValue("image_id (0x14) : ", bH->identificationWord);
    DisplayValue("encryption_keystore (0x18) : ", bH->encryptionKeySource);
    DisplayValue("plm_offset (0x1c) : ", bH->sourceOffset);
    DisplayValue("pmccdo_load_addr (0x20) : ", bH->pmcCdoLoadAddress);
    DisplayValue("pmccdo_length (0x24) : ", bH->pmcCdoLength);
    DisplayValue("pmccdo_total_length (0x28) : ", bH->totalPmcCdoLength);
    DisplayValue("plm_length (0x2c) : ", bH->plmLength);
    DisplayValue("plm_total_length (0x30) : ", bH->totalPlmLength);
    DisplayValue("attributes (0x34) : ", bH->bhAttributes);
    DisplayKey("grey/black_key (0x38) : ", bH->greyOrBlackKey);
    DisplayIV("grey/black_iv (0x58) : ", bH->greyOrBlackIV);
    DisplayIV("plm_sec_hdr_iv (0x64) : ", bH->plmSecureHdrIv);
   
 
    
        DisplayValue("puf_shutter (0x70) : ", bH->shutterValue);
        DisplayValue("puf_ro_swap (0x74) : ", bH->pufRoSwapConfigVal);
        DisplayValue("revoke_id (0x78) : ", bH->plmRevokeId);
        DisplayLongValues("udf_BH (0x7C) : ", (uint8_t*)bH->udfBH, 516);
        DisplayValue("auth_header (0x280) : ", bH->authHeader1);
        DisplayValue("hash_block_size (0x284) : ", bH->hashBlockLength1);
        DisplayValue("total_ppk_size (0x288) : ", bH->totalppkkSize1);
        DisplayValue("actual_ppk_size (0x28c) : ", bH->actualppkSize1);
        DisplayValue("total_sign_size (0x290) : ", bH->totalSignatureSize1);
        DisplayValue("actual_sign_size (0x294) : ", bH->actualSignatureSize1);
        DisplayValue("metahdr_offset (0x2d0) : ", bH->imageHeaderByteOffset);
    

    DisplayKey("puf_data (0xb34) : ", bH->puf);   // PUF data offset
    DisplayValue("checksum (0x113c) : ", bH->headerChecksum);
    std::cout << " attribute list - " << std::endl;
    DisplayBhAttributes(bH->bhAttributes,static_cast<Authentication::Type>(IdentifyAuthtype(bH->authHeader1)));
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::DumpPartitions(uint8_t* buffer, uint32_t length, std::string name, uint32_t id, uint32_t index)
{
    FILE* filePtr;
    size_t result;
    std::string extension = ".bin";

    std::string fName = StringUtils::FolderPath(binFilename);
    if (dumpPath != "")
    {
        fName = dumpPath;
    }
    fName = fName + "/" + name;
    if (dumpType == DumpOption::PARTITIONS)
    {
        fName += StringUtils::Format(".%x", id);
        fName += StringUtils::Format(".%x", index);
    }
    fName += extension;
    filePtr = fopen(fName.c_str(), "wb");

    if (filePtr != NULL)
    {
        result = fwrite(buffer, 1, length, filePtr);
        if (result != length)
        {
            LOG_ERROR("Error dumping partition %s to a file", name.c_str());
        }
        fclose(filePtr);
        LOG_INFO("%s generated successfully", StringUtils::BaseName(fName).c_str());
    }
    return;
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::DisplayImageHeaderTable(void)
{

        #define ID_STRING(opt) (opt == FPDI) ? "FPDI" : "PPDI"
        Separator();
        std::cout << "   IMAGE HEADER TABLE" << std::endl;
        Separator();
        DisplayValue("version (0x00) : ", iHT->version, "total_images (0x04) : ", iHT->imageTotalCount);
        DisplayValue("ih_offset (0x08) : ", iHT->firstImageHeaderWordOffset, "total_partitions (0x0c) : ", iHT->partitionTotalCount);
        DisplayValue("pht_offset (0x10) : ", iHT->firstPartitionHeaderWordOffset, "sec_boot_dev_addr (0x14) : ", iHT->secondaryBootDeviceAddress);
        DisplayValue("id_code (0x18) : ", iHT->idCode, "attributes (0x1c) : ", iHT->imageHeaderTableAttributes);
        DisplayValue("pdi_id (0x20) : ", iHT->pdiId, "parent_id (0x24) : ", iHT->parentId);
        DisplayAscii("id_string (0x28) : ", ID_STRING(iHT->identificationString));
        DisplayValue("hdr_sizes (0x2C) : ", iHT->headerSizes, "mhdr_total_length (0x30) : ", iHT->totalMetaHdrLength);
        DisplayIV("mhdr_sec_hdr_iv (0x34) : ", iHT->metaHdrSecureHdrIv);
        DisplayValue("mhdr_encrkey_store (0x40) : ", iHT->metaHdrKeySource, "extended_id_code (0x44) : ", iHT->extendedIdCode);
        DisplayValue("hdr_ac (0x48) : ", iHT->headerAuthCertificateWordOffset);
        DisplayIV("grey/black_iv (0x4C) : ", iHT->metaHdrGreyOrBlackIV);
        DisplayValue("optional_data_size (0x58) : ", iHT->optionalDataSize);
        DisplayValue("auth_header (0x5C) : ", iHT->authHeader1);
        DisplayValue("hash_block_length (0x60) : ", iHT->hashBlockLength1 * 4);
        DisplayValue("hash_block_offset (0x64) : ", iHT->hashBlockOffset * 4);
        DisplayValue("total_ppk_size (0x68) : ", iHT->totalppkkSize1);
        DisplayValue("actual_ppk_size (0x6C) : ", iHT->actualppkSize1);
        DisplayValue("total_hb_sign_size (0x70) : ", iHT->totalHashBlockSignatureSize1);
        DisplayValue("actual_hb_sign_size (0x74) : ", iHT->actualSignatureSize1);
        DisplayValue("checksum(0x7c) : ", iHT->ihtChecksum);
        std::cout << " attribute list - " << std::endl;
        DisplayIhtAttributes(iHT->imageHeaderTableAttributes);
    
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::DisplayImageHeaders(void)
{

        #define OWNER(opt) (((opt >> 3) & 7) ? "[plm]" : "[non-plm]")
        int index = 0;
        for (std::list<Versal_2ve_2vmImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
        {
            index++;
            Separator();
            std::cout << "   IMAGE HEADER " << "(" << (*iH)->imageName << ")" << std::endl;
            Separator();
            DisplayValue("pht_offset (0x00) : ", (*iH)->partitionHeaderWordOffset, "section_count (0x04) : ", (*iH)->dataSectionCount);
            DisplayValue("mHdr_revoke_id (0x08) : ", (*iH)->metaHdrRevokeId, "attributes (0x0c) : ", ((*iH)->imageAttributes));
            DisplayAscii("name (0x10) : ", (*iH)->imageName);
            DisplayValue("id (0x20) : ", (*iH)->imageId, "unique_id (0x24) : ", (*iH)->uniqueId);
            DisplayValue("parent_unique_id (0x28) : ", (*iH)->parentUniqueId, "function_id (0x2c) : ", (*iH)->functionId);
            DisplayValue("memcpy_address_lo (0x30) : ", (*iH)->memcpyAddressLo, "memcpy_address_hi (0x34) : ", (*iH)->memcpyAddressHi);

                DisplayValue("pcr_value (0x38) : ", (*iH)->pcrNumber, "pcr_mindex (0x3A) : ", (*iH)->pcrMeasurementIndex);
             
            
            DisplayValue("checksum (0x3c) : ", (*iH)->ihChecksum);
            std::cout << " attribute list -" << std::endl;
            DisplayIhAttributes((*iH)->imageAttributes);
        }
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::DisplayPartitionHeaderTable(void)
{

        uint32_t cnt_index = 0;
        std::list<Versal_2ve_2vmPartitionHeaderTableStructure*>::iterator pHT = pHTs.begin();
        for (std::list<Versal_2ve_2vmImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
        {
            uint32_t part_sec_index = 0;
            uint32_t part_sec_count = 0;
            uint32_t part_index = 0;
            for (cnt_index = 0; cnt_index < (*iH)->dataSectionCount; cnt_index++)
            {
                if ((*pHT)->dataSectionCount > 0)
                {
                part_sec_index = 0;
                part_sec_count = (*pHT)->dataSectionCount;
                }   
                if (((*pHT)->dataSectionCount > 0) && (cnt_index != 0))
                {
                part_index++;
                }
                if((part_sec_count > 1) && ((*pHT)->dataSectionCount == 0))
                {
                    part_sec_index++;
                }
                Separator();
                std::cout << "   PARTITION HEADER TABLE " << "(" << (*iH)->imageName << "." << std::hex << part_index << "." << std::hex << part_sec_index << ")" << std::endl;
                Separator();
                DisplayValue("encrypted_length (0x00) : ", (*pHT)->encryptedPartitionLength, "unencrypted_length (0x04) : ", (*pHT)->unencryptedPartitionLength);
                DisplayValue("total_length (0x08) : ", (*pHT)->totalPartitionLength, "next_pht (0x0c) : ", (*pHT)->nextPartitionHeaderOffset);
                DisplayValue("exec_addr_lo (0x10) : ", (uint32_t)(*pHT)->destinationExecAddress, "exec_addr_hi (0x14) : ", (uint32_t)((*pHT)->destinationExecAddress >> 32));
                DisplayValue("load_addr_lo (0x18) : ", (uint32_t)(*pHT)->destinationLoadAddress, "load_addr_hi (0x1c) : ", (uint32_t)((*pHT)->destinationLoadAddress >> 32));
                DisplayValue("partition_offset (0x20) : ", (*pHT)->partitionWordOffset * 4, "attributes (0x24) : ", (*pHT)->partitionAttributes);
                DisplayValue("section_count (0x28) : ", (*pHT)->dataSectionCount, "checksum_offset (0x2c) : ", (*pHT)->checksumWordOffset);
                DisplayValue("id (0x30) : ", (*pHT)->puid, "ac_offset (0x34) : ", (*pHT)->authCertificateOffset);
                DisplayIV("sec_hdr_iv (0x38) : ", (*pHT)->partitionSecureHdrIv);
                DisplayValue("encryption_keystore (0x44) : ", (*pHT)->partitionKeySource);
                DisplayIV("grey/black_iv (0x48) : ", (*pHT)->partitionGreyOrBlackIV);
                DisplayValue("revoke_id (0x54) : ", (*pHT)->partitionRevokeId);

                DisplayValue("measure_boot_addr (0x58) : ", (*pHT)->measuredBootAddress);
                DisplayValue("auth_hdr (0x5C) : ", (*pHT)->authHeader1);
                DisplayValue("hash_block_length (0x60) : ", (*pHT)->hashBlockLength1 * 4);
                DisplayValue("hash_block_offset (0x64) : ", (*pHT)->hashBlockWordOffset * 4);
                DisplayValue("total_ppk_size (0x68) : ", (*pHT)->totalppkkSize1);
                DisplayValue("actual_ppk_size (0x6C) : ", (*pHT)->actualppkSize1);
                DisplayValue("total_hb_sign_size (0x70) : ", (*pHT)->totalHashBlockSignatureSize1);
                DisplayValue("actual_hb_sign_size (0x74) : ", (*pHT)->actualSignatureSize1);

                DisplayValue("checksum (0x7c) : ", (*pHT)->pHChecksum);
                std::cout << " attribute list -" << std::endl;
                DisplayPhtAttributes((*pHT)->partitionAttributes);
                pHT++;
            }
        }
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::DisplayAuthenicationCertificates(void)
{
    Authentication::Type auth_type = Authentication::None;
    std::list<std::pair<uint8_t*, uint32_t>>::iterator hash_block_itr = Hashblock_record.begin();
    std::list<uint32_t>::iterator authcheck = authtype.begin();

            
    if (Hashblock_record.empty()) 
    {
        std::cout << "   NO Hash Block0" << std::endl;
        return;
    }
 
        uint32_t index=0;
        for(std::list<uint8_t*>::iterator aC_iterator = aCs.begin(); aC_iterator != aCs.end(); aC_iterator++,++index)
        {
            auth_type = Authentication::None; //resetting auth type to reinit auth type based on config in PDI for each paritition
            if ((*aC_iterator) != NULL)
            {
                    Separator();
                    std::cout << "   AUTHENTICATION CERTIFICATE"<<index << " for Hash Block" <<index<<std::endl;
                    Separator();         
                    if ((*authcheck == Authentication::LMS_SHA2_256) || (*authcheck == Authentication::LMS_SHAKE256))
                    {
                        auth_type = Authentication::LMS_SHA2_256;
                    }
                    else
                    {
                        auth_type = (Authentication::Type)*authcheck; 
                    }
                    DisplayACFields(*aC_iterator, auth_type);    //print AC for each partition 

            }

             if(!Hashblock_record.empty())
             { 
                    
                   if(bH && hash_block_itr == Hashblock_record.begin())
                    {
                        Separator();
                        std::cout << "   Hash Block0" << std::endl;
                        Separator();

                        DisplayLongValues("BH hash index          : ", ((*hash_block_itr).first), 4);   ((*hash_block_itr).first) += 4; 
                        DisplayLongValues("BH hash digest         : ", ((*hash_block_itr).first) , 48);  ((*hash_block_itr).first) += 48;
                        DisplayLongValues("PLM hash index         : ", ((*hash_block_itr).first), 4);   ((*hash_block_itr).first) += 4;
                        DisplayLongValues("PLM hash digest        : ", ((*hash_block_itr).first), 48);  ((*hash_block_itr).first) += 48;
                        DisplayLongValues("PMC data index         : ", ((*hash_block_itr).first), 4);   ((*hash_block_itr).first) += 4;
                        DisplayLongValues("PMC data digest        : ", ((*hash_block_itr).first), 48);  ((*hash_block_itr).first) += 48;
                        DisplayLongValues("hash block1 index      : ", ((*hash_block_itr).first), 4);   ((*hash_block_itr).first) += 4;
                        DisplayLongValues("hash block1 digest     : ", ((*hash_block_itr).first), 48);  ((*hash_block_itr).first) += 152;
                        DisplayLongValues("padding      : ", ((*hash_block_itr).first), 16);   
                        hash_block_itr++;
                        if(auth_type != Authentication::None)
                            DisplayLongValues("HASH Block0 signture   : ", ((*hash_block_itr).first), hash_block_itr->second); 
                        if(hash_block_itr != Hashblock_record.end())    
                            hash_block_itr++;
                        if ((*aC_iterator) != NULL && authcheck != authtype.end())
                            authcheck++;             // authtype checking
                        continue;     
                    }
                    if(hash_block_itr->second != 0)   //hashblock length 
                    { 
                        Separator();
                        std::cout << "   Hash Block" << index<<std::endl;
                        Separator();
                    for(uint32_t limit = 0; limit<(hash_block_itr->second/52);limit++)   // hash_block_itr->second/52 -> to identify number of parititioin in given hash block
                    {
                    DisplayLongValues("hash index          : ", ((*hash_block_itr).first), 4);   ((*hash_block_itr).first) += 4;
                    DisplayLongValues("digest              : ", ((*hash_block_itr).first), 48);  ((*hash_block_itr).first) += 48;
                    }
                    DisplayLongValues("Padding          : ", ((*hash_block_itr).first), hash_block_itr->second%52);   //hash_block_itr->second%52 --> to identify the padding length
                    hash_block_itr++;
                    if(auth_type != Authentication::None)   
                        DisplayLongValues("signture   : ", ((*hash_block_itr).first), hash_block_itr->second);  
                    if(hash_block_itr != Hashblock_record.end())
                        hash_block_itr++;                                              
                    }
                    if ((*aC_iterator) != NULL && authcheck != authtype.end())
                        authcheck++;             // authtype checking
            }      
        }
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::DisplayACFields(uint8_t* aC, Authentication::Type auth_type)
{
    if (auth_type == Authentication::RSA)
    {
        AuthCertificate4096Sha3PaddingHBStructure* auth_cert = (AuthCertificate4096Sha3PaddingHBStructure*)(aC);
        //DisplayValue("auth_header (0x00) : ", auth_cert->acHeader);
        //DisplayValue("revoke_id (0x04) : ", auth_cert->spkId);
        //DisplayLongValues("udf (0x08) : ", (uint8_t*)auth_cert->acUdf, 56);
        DisplayLongValues("ppk_mod (0x40) : ", (uint8_t*)auth_cert->acPpk.N, 512);
        DisplayLongValues("ppk_mod_ext (0x240) : ", (uint8_t*)auth_cert->acPpk.N_extension, 512);
        DisplayLongValues("ppk_exponent (0x440) : ", (uint8_t*)auth_cert->acPpk.E, 4);
        //DisplayLongValues("ppk_padding (0x444) : ", (uint8_t*)auth_cert->ppkSHA3Padding, 12);
        DisplayLongValues("spk_mod (0x450) : ", (uint8_t*)auth_cert->acSpk.N, 512);
        DisplayLongValues("spk_mod_ext (0x650) : ", (uint8_t*)auth_cert->acSpk.N_extension, 512);
        DisplayLongValues("spk_exponent (0x850) : ", (uint8_t*)auth_cert->acSpk.E, 4);
        //DisplayLongValues("spk_padding (0x854) : ", (uint8_t*)auth_cert->spkSHA3Padding, 4);
        DisplayLongValues("spk_signature (0x860) : ", (uint8_t*)(&auth_cert->acSpkSignature), 512);
        //DisplayLongValues("bh_signature (0xa60) : ", (uint8_t*)(&auth_cert->acHeaderSignature), 512);
        //DisplayLongValues("part_signature (0xc60) : ", (uint8_t*)(&auth_cert->acHashBlockSignature), 512);
    }
    else if(auth_type == Authentication::ECDSA)
    {
        AuthCertificateECDSAHBStructure* auth_cert = (AuthCertificateECDSAHBStructure*)(aC);
        //DisplayValue("auth_header (0x00) : ", auth_cert->acHeader);
        //DisplayValue("revoke_id (0x04) : ", auth_cert->spkId);
        //DisplayLongValues("udf (0x08) : ", (uint8_t*)auth_cert->acUdf, 56);
        DisplayLongValues("ppk_x (0x40) : ", (uint8_t*)auth_cert->acPpk.x, 48);
        DisplayLongValues("ppk_y (0x70) : ", (uint8_t*)auth_cert->acPpk.y, 48);
        //DisplayLongValues("ppk_padding (0x444) : ", (uint8_t*)auth_cert->ppkSHA3Padding, 12);
        DisplayLongValues("spk_x (0x450) : ", (uint8_t*)auth_cert->acSpk.x, 48);
        DisplayLongValues("spk_y (0x480) : ", (uint8_t*)auth_cert->acSpk.y, 48);
        //DisplayLongValues("spk_padding (0x854) : ", (uint8_t*)auth_cert->spkSHA3Padding, 4);
        DisplayLongValues("spk_signature (r) (0x860) : ", (uint8_t*)(&auth_cert->acSpkSignature), 48);
        DisplayLongValues("(s) (0x890) : ", ((uint8_t*)(&auth_cert->acSpkSignature) + 48), 48);
        //DisplayLongValues("bh_signature (r) (0xa60) : ", (uint8_t*)(&auth_cert->acHeaderSignature), 48);
        //DisplayLongValues("(s) (0xa90) : ", ((uint8_t*)(&auth_cert->acHeaderSignature) + 48), 48);
        //DisplayLongValues("part_signature (r) (0xc60) : ", (uint8_t*)(&auth_cert->acHashBlockSignature), 48);
        //DisplayLongValues("(s) (0xc90) : ", ((uint8_t*)(&auth_cert->acHashBlockSignature) + 48), 48);
    }
    else if(auth_type == Authentication::ECDSAp521)
    {
        AuthCertificateECDSAp521HBStructure* auth_cert = (AuthCertificateECDSAp521HBStructure*)(aC);
        //DisplayValue("auth_header (0x00) : ", auth_cert->acHeader);
        //DisplayValue("revoke_id (0x04) : ", auth_cert->spkId);
        //DisplayLongValues("udf (0x08) : ", (uint8_t*)auth_cert->acUdf, 56);
        DisplayLongValues("ppk_x (0x40) : ", (uint8_t*)auth_cert->acPpk.x, 66);
        DisplayLongValues("ppk_y (0x70) : ", (uint8_t*)auth_cert->acPpk.y, 66);
        //DisplayLongValues("ppk_padding (0x444) : ", (uint8_t*)auth_cert->ppkSHA3Padding, 12);
        DisplayLongValues("spk_x (0x450) : ", (uint8_t*)auth_cert->acSpk.x, 66);
        DisplayLongValues("spk_y (0x480) : ", (uint8_t*)auth_cert->acSpk.y, 66);
        //DisplayLongValues("spk_padding (0x854) : ", (uint8_t*)auth_cert->spkSHA3Padding, 4);
        DisplayLongValues("spk_signature (r) (0x860) : ", (uint8_t*)(&auth_cert->acSpkSignature), 66);
        DisplayLongValues("(s) (0x890) : ", ((uint8_t*)(&auth_cert->acSpkSignature) + 66), 66);
        //DisplayLongValues("bh_signature (r) (0xa60) : ", (uint8_t*)(&auth_cert->acHeaderSignature), 66);
        //DisplayLongValues("(s) (0xa90) : ", ((uint8_t*)(&auth_cert->acHeaderSignature) + 66), 66);
        //DisplayLongValues("part_signature (r) (0xc60) : ", (uint8_t*)(&auth_cert->acHashBlockSignature), 66);
        //DisplayLongValues("(s) (0xc90) : ", ((uint8_t*)(&auth_cert->acHashBlockSignature) + 66), 66);
    }
    else
    {
        // LMS
        if ((auth_type == Authentication::LMS_SHA2_256) || (auth_type == Authentication::LMS_SHAKE256))
        {
            std::cout << "---- ppk ----" << std::endl;
            DisplayLongValues("num_levels (0x0) : ", aC, 4); aC += 4;
            DisplayLongValues("lm_type (0x4) : ", aC, 4); aC += 4;
            DisplayLongValues("lm_ots_type (0x8) : ", aC, 4); aC += 4;
            DisplayLongValues("i (0xc) : ", aC, 16); aC += 16;
            DisplayLongValues("k (0x1c) : ", aC, 32); aC += 32;
            std::cout << "------------" << std::endl;
            aC += 4;
            
            DisplayLongValues("total_spk_size (0x40) : ", aC, 4); aC += 4;
            DisplayLongValues("actual_spk_size (0x44) : ", aC, 4); aC += 4;
            DisplayLongValues("total_sign_size (0x48) : ", aC, 4); aC += 4;
            DisplayLongValues("actual_sign_size (0x4c) : ", aC, 4); aC += 4;
            DisplayLongValues("spk_id (0x50) : ", aC, 4); aC += 4;
            aC += 12;

            std::cout << "---- spk ----" << std::endl;
            DisplayLongValues("num_levels (0x0) : ", aC, 4); aC += 4;
            DisplayLongValues("lm_type (0x4) : ", aC, 4); aC += 4;
            DisplayLongValues("lm_ots_type (0x8) : ", aC, 4); aC += 4;
            DisplayLongValues("i (0xc) : ", aC, 16); aC += 16;
            DisplayLongValues("k (0x1c) : ", aC, 32); aC += 32;
            std::cout << "------------" << std::endl;
            aC += 4;
            
            DisplayLongValues("spk_signature (0xa0) : ", aC, bH->totalSignatureSize1);
            aC += bH->totalSignatureSize1;
            DisplayLongValues("hash_signature (0x2780) : ", aC, bH->totalSignatureSize1);
        }
    }
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::DisplaySmapVectors(void)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << "smap_words (0x00) : ";
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << bH->smapWords[0] << std::setw(8) << std::hex << bH->smapWords[1] << std::setw(8) << std::hex << bH->smapWords[2] << std::setw(8) << std::hex << bH->smapWords[3] << std::endl;
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::DisplayImageInfo()
{
    Separator();
    std::cout << "   BOOTIMAGE COMPONENTS" << std::endl;
    Separator();

    uint32_t cnt_index = 0;
    std::list<Versal_2ve_2vmPartitionHeaderTableStructure*>::iterator pHT = pHTs.begin();
    for (std::list<Versal_2ve_2vmImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
    {
        uint32_t part_sec_index = 0;
        uint32_t part_sec_count = 0;
        uint32_t part_index = 0;
        if ((dumpType == DumpOption::PARTITIONS) || readType != ReadImageOption::NONE)
        {
           LOG_MSG("+---Image: %s [id:0x%x]",(*iH)->imageName, (*iH)->imageId);
        }
        for (cnt_index = 0; cnt_index < (*iH)->dataSectionCount; cnt_index++)
        {
            std::string partitionType = GetPartitionType((*pHT)->partitionAttributes);
            std::string partitionCore = GetPartitionCore((*pHT)->partitionAttributes);
            if ((*pHT)->dataSectionCount > 0)
            {
               part_sec_index = 0;
               part_sec_count = (*pHT)->dataSectionCount;
            }
            if (((*pHT)->dataSectionCount > 0) && (cnt_index != 0))
            {
               part_index++;
            }
            if((part_sec_count > 1) && ((*pHT)->dataSectionCount == 0))
            {
                part_sec_index++;
            }
            std::string fName = (*iH)->imageName;
            fName += StringUtils::Format(".%x", part_index);
            fName += StringUtils::Format(".%x", part_sec_index);
            LOG_MSG("    |__ %s [core: %s, type: %s]",fName.c_str(), partitionCore.c_str(), partitionType.c_str());
            pHT++;
        }
        LOG_MSG("");
    }
}

/******************************************************************************/
static const char* BhNetSeriesSignedFieldName(Authentication::Type authType)
{
    switch (authType)
    {
    case Authentication::RSA: return "rsa_signed ";
    case Authentication::ECDSA: return "ecdsa_signed ";
    case Authentication::ECDSAp521: return "ecdsap521_signed ";
    case Authentication::LMS_SHA2_256:
    case Authentication::LMS_SHAKE256: return "lms_signed ";
    case Authentication::HSS_SHA2_256:
    case Authentication::HSS_SHAKE256: return "hss_signed ";
    case Authentication::None:
    default: return "signed ";
    }
}

/******************************************************************************/
void Versal_2ve_2vmReadImage::DisplayBhAttributes(uint32_t value, Authentication::Type authType)
{
    std::string val, val1;
    
    switch ((value >> PUF_HD_BIT_SHIFT) & PUF_HD_BIT_MASK)
    {
        case 3: val = "[bh]";           break;
        default: val = "[efuse]";       break;
    }
    val1 = val;

    switch ((value >> BH_PUF_MODE_BIT_SHIFT) & BH_PUF_MODE_BIT_MASK)
    {
        case 0: val = "[puf-12k]";      break;
        case 3: val = "[puf-4k]";       break;
        default: val = "[invalid]";     break;
    }
    DisplayAttributes("puf_hd_source ", val1, "puf_mode ", val);

    switch ((value >> BI_HASH_BIT_SHIFT) & BI_HASH_BIT_MASK)
    {
        case 3: val = "[sha-3]";        break;
        default: val = "[none]";        break;
    }
    val1 = val;
    
    switch ((value >> DPA_CM_BIT_SHIFT) & DPA_CM_BIT_MASK)
    {
        case 3: val = "[enabled]";      break;
        default: val = "[disabled]";    break;
    }
    DisplayAttributes("integrity ", val1, "dpa_cm ", val);

    if (!versalNetSeries)
    {
        switch ((value >> BH_RSA_BIT_SHIFT) & BH_RSA_BIT_MASK)
        {
            case 3: val = "[enabled]";      break;
            default: val = "[disabled]";    break;
        }
        DisplayAttributes("bh_auth ", val, "", "");
    }

    if (versalNetSeries)
    {
        switch ((value >> BH_RSA_SINGED_BIT_SHIFT) & BH_RSA_SINGED_BIT_MASK)
        {
            case 3: val = "[yes]";      break;
            default: val = "[no]";    break;
        }
        val1 = val;

        switch ((value >> BH_DICE_BIT_SHIFT) & BH_DICE_BIT_MASK)
        {
            case 3: val = "[enabled]";      break;
            default: val = "[disabled]";    break;
        }
        DisplayAttributes(BhNetSeriesSignedFieldName(authType), val1, "dice ", val);
    }
   
}

/*********************************************************************************/
void Versal_2ve_2vmReadImage::DisplayIhtAttributes(uint32_t value)
{
    std::string val, val1;

    switch ((value >> vihtSiliconRevisionIdCodeCheckShift) & vihtSiliconRevisionIdCodeCheckMask)
    {
        case 1: val = "[true]";         break;
        default: val = "[false]";       break;
    }
    val1 = val;
    switch ((value >> vihtImageCreatorIdShift) & vihtImageCreatorIdMask)
    {
        case 1: val = "[wdi]";          break;
        case 2: val = "[sdk]";          break;
        default: val = "[rsvd]";        break;
    }
    DisplayAttributes("silicon_rev_id_code_check ", val1, "image_creator ", val);

    switch ((value >> vihtSecBootDeviceShift) & vihtSecBootDeviceMask)
    {
        case 0: val = "[same-dev]";     break;
        case 1: val = "[qspi32]";       break;
        case 2: val = "[qspi24]";       break;
        case 3: val = "[nand]";         break;
        case 4: val = "[sd0]";          break;
        case 5: val = "[sd1]";          break;
        case 6: val = "[sd-ls]";        break;
        case 7: val = "[mmc]";          break;
        case 8: val = "[usb]";          break;
        case 9: val = "[ethernet]";     break;
        case 10: val = "[pcie]";        break;
        case 11: val = "[sata]";        break;
        case 12: val = "[ospi]";        break;
        case 13: val = "[smap]";        break;
        case 14: val = "[sbi]";         break;
        case 15: val = "[sd0-raw]";     break;
        case 16: val = "[sd1-raw]";     break;
        case 17: val = "[sd-ls-raw]";   break;
        case 18: val = "[mmc-raw]";     break;
        case 19: val = "[mmc0]";        break;
        case 20: val = "[mmc0-raw]";    break;
        case 21: val = "[imagestore]";  break;
        case 22: val = "[ufs]";         break;
        default: val = "[n/a]";         break;
    }
    val1 = val;
    switch ((value >> vihtDpacmEnableShift) & vihtDpacmEnableMask)
    {
        case 3: val = "[enabled]";      break;
        default: val = "[disabled]";    break;
    }
    DisplayAttributes("boot_device ", val1, "dpacm ", val);

    switch ((value >> vihtPufHDLocationShift) & vihtPufHDLocationMask)
    {
        case 3: val = "[bh]";           break;
        default: val = "[efuse]";       break;
    }
    val1 = val;
    DisplayAttributes("puf_hd_source", val1, "", "");

    switch ((value >> vihtIdCodeCheckShift) & vihtIdCodeCheckMask)
    {
        case 3: val = "[true]";           break;
        default: val = "[false]";         break;
    }
    val1 = val;
    DisplayAttributes("skip_id_code_check", val1, "", "");
}

/************************************************************************************************/
void Versal_2ve_2vmReadImage::DisplayIhAttributes(uint32_t value)
{
    std::string val, val1, val2, val3;
    switch ((value >> vihImageOwnerShift) & vihImageOwnerMask)
    {
        case 0: val = "[plm]";      break;
        case 1: val = "[non-plm]";  break;
        default: val = "[invalid]"; break;
    }
    val1 = val;
    switch ((value >> vihCopyToMemoryShift) & vihCopyToMemoryMask)
    {
        case 1: val = "[yes]";     break;
        default: val = "[no]";     break;
    }
    DisplayAttributes("owner ", val1, "memcpy ", val);

    switch ((value >> vihDelayLoadShift) & vihDelayLoadMask)
    {
        case 1: val = "[delay]";     break;
        default: val = "[now]";      break;
    }
    val1 = val;
    switch ((value >> vihDelayHandoffShift) & vihDelayHandoffMask)
    {
        case 1: val = "[delay]";     break;
        default: val = "[now]";      break;
    }
    DisplayAttributes("load ", val1, "handoff ", val);

    std::string powerDomains = "";
    switch ((value >> vihLowPowerDomainShift) & vihLowPowerDomainMask)
    {
        case 1: powerDomains += "[lpd]";    break;
        default:                            break;
    }

    switch ((value >> vihFullPowerDomainShift) & vihFullPowerDomainMask)
    {
        case 1: powerDomains += "[fpd]";    break;
        default:                            break;
    }

    switch ((value >> vihSystemPowerDomainShift) & vihSystemPowerDomainMask)
    {
        case 1: powerDomains += "[spd]";    break;
        default:                            break;
    }
    val1 = val;
    switch ((value >> vihPLPowerDomainShift) & vihPLPowerDomainMask)
    {
        case 1: powerDomains += "[pld]";    break;
        default:                            break;
    }
    if (powerDomains == "") powerDomains = "[none]";
    DisplayAttributes("dependentPowerDomains ", powerDomains, " ", "");
}

/************************************************************************************************/
void Versal_2ve_2vmReadImage::DisplayPhtAttributes(uint32_t value)
{
    uint32_t core = (value >> v2phtDestCpuShift) & v2phtDestCpuMask;
    uint32_t arch = (value >> v2phtExecStateShift) & v2phtExecStateMask;
    uint32_t is_a72 = (core == 1 || core == 2);
    bool is_armv8 = (arch == 0 && is_a72 == 1);
    bool is_armv7 = ((arch == 1 && is_a72 == 1) || core == 5 || core == 6 || core == 7);
    bool is_elf = (((value >> v2phtPartitionTypeShift) & v2phtPartitionTypeMask) == 1);

    std::string val, val1, val2, val3;
    switch ((value >> v2phtTrustzoneShift) & v2phtTrustzoneMask)
    {
        case 0: val = "[non-secure]";     break;
        case 1: val = "[secure]";         break;
    }
    if (!is_armv8)
    {
        val = "[n/a]";
    }
    val1 = val;

    switch ((value >> v2phtExceptionLevelShift) & v2phtExceptionLevelMask)
    {
        case 0: val = "[el-0]";     break;
        case 1: val = "[el-1]";     break;
        case 2: val = "[el-2]";     break;
        case 3: val = "[el-3]";     break;
    }
    if (!is_armv8)
    {
        val = "[n/a]";
    }
    DisplayAttributes("trustzone ", val1, "el ", val);
    
    switch ((value >> v2phtExecStateShift) & v2phtExecStateMask)
    {
        case 0: val = "[aarch-64]";   break;
        case 1: val = "[aarch-32]";   break;
    }
    if (!is_armv8)
    {
        val = "[n/a]";
    }
    val1 = val;

    switch ((value >> v2phtDestCpuShift) & v2phtDestCpuMask)
    {
        case 0: val = "[none]";           break;
        case 1: 
            if (versalNetSeries) val = "[a78-0]"; 
            else val = "[a72-0]";         
            break;
        case 2: 
            if (versalNetSeries) val = "[a78-1]";
            else val = "[a72-1]";         
            break;
        if (versalNetSeries)
        {
            case 3: val = "[a78-2]";      break;
            case 4: val = "[a78-3]";      break;
        }
        case 5: val = "[r5-0]";           break;
        case 6: val = "[r5-1]";           break;
        case 7: val = "[r5-lockstep]";    break;
        case 8: val = "[psm]";            break;
        case 9: val = "[aie]";            break;
        default: val = "[invalid]";       break;
    }
    if (!is_elf)
    {
        val = "[n/a]";
    }

    DisplayAttributes("exec_state ", val1, "core ", val);

    if (versalNetSeries)
    {
        switch ((value >> v2phtlockStepShift) & v2phtlockStepMask)
        {
            case 0: val = "[disabled]";      break;
            case 3: val = "[enabled]";       break;
            default: val = "[invalid]";      break;
        }
        val1 = val;
        switch ((value >> v2phtClusterShift) & v2phtClusterMask)
        {
            case 0: val = "[0]";            break;
            case 1: val = "[1]";            break;
            case 2: val = "[2]";            break;
            case 3: val = "[3]";            break;
            case 4: val = "[4]";            break;
            default: val = "[invalid]";     break;
        }
        DisplayAttributes("lockstep ", val1, "cluster", val);
        switch ((value >> v2phtTcmBootShift) & v2phtTcmBootMask)
        {
            case 3: val = "[enabled]";      break;
            default: val = "[disabled]";    break;
        }
        val1 = val;
        DisplayAttributes("tcmboot ", val1, " ", "");
    }
    switch ((value >> v2phtChecksumTypeShift) & v2phtChecksumTypeMask)
    {
        case 0: val = "[none]";          break;
        case 3: val = "[sha-3]";         break;
        default: val = "[invalid]";      break;
    }
    val1 = val;
    switch ((value >> v2phtPufHDLocationShift) & v2phtPufHDLocationMask)
    {
        case 0: val = "[efuse]";        break;
        case 3: val = "[bh]";           break;
        default: val = "[invalid]";     break;
    }

    DisplayAttributes("checksum ", val1, "puf_hd_source", val);

    switch ((value >> v2phtPartitionOwnerShift) & v2phtPartitionOwnerMask)
    {
        case 0: val = "[plm]";          break;
        case 1: val = "[non-plm]";      break;
        default: val = "[invalid]";     break;
    }
    val1 = val;

    switch ((value >> v2phtEndiannessShift) & v2phtEndiannessMask)
    {
        case 0: val = "[little-endian]";    break;
        case 1: val = "[big-endian]";       break;
    }
    if (!is_elf)
    {
        val = "[n/a]";
    }

    DisplayAttributes("owner ", val1, "endianness ", val);

    switch ((value >> v2phtHivecShift) & v2phtHivecMask)
    {
        case 0: val = "[false]";   break;
        case 1: val = "[true]";    break;
    }
    if (!is_armv7)
    {
        val = "[n/a]";
    }
    val1 = val;

    switch ((value >> v2phtPartitionTypeShift) & v2phtPartitionTypeMask)
    {
        case 0: val = "[none]";             break;
        case 1: val = "[elf]";              break;
        case 2: val = "[cdo]";              break;
        case 3: val = "[cfi]";              break;
        case 4: val = "[raw]";              break;
        case 5: val = "[raw-elf]";          break;
        case 6: val = "[cfi-gsc-mask]";     break;
        case 7: val = "[cfi-gsc-unmask]";   break;
        default: val = "[invalid]";         break;
    }

    DisplayAttributes("hi-vec ", val1, "partition_type", val);

    switch ((value >> v2phtDpaCMShift) & v2phtDpaCMMask)
    {
        case 3: val = "[enabled]";      break;
        default: val = "[disabled]";    break;
    }
    val1 = val;
    DisplayAttributes("dpacm ", val1, " ", "");
}

/*******************************************************************************************************/
std::string Versal_2ve_2vmReadImage::GetPartitionCore(uint32_t value)
{
    std::string val;
    bool is_elf = (((value >> v2phtPartitionTypeShift) & v2phtPartitionTypeMask) == 1);

    switch ((value >> v2phtDestCpuShift) & v2phtDestCpuMask)
    {
        case 0: val = "none";           break;
        case 1: 
            if (versalNetSeries) val = "a78-0"; 
            else val = "a72-0";         
            break;
        case 2: 
            if (versalNetSeries) val = "a78-1";
            else val = "a72-1";         
            break;
        if (versalNetSeries)
        {
            case 3: val = "a78-2";      break;
            case 4: val = "a78-3";      break;
        }
        case 5: val = "r5-0";           break;
        case 6: val = "r5-1";           break;
        case 7: val = "r5-lockstep";    break;
        case 8: val = "psm";            break;
        case 9: val = "aie";            break;
        default: val = "invalid";       break;
    }
    if (!is_elf)
    {
        val = "n/a";
    }
    return val;
}

/*******************************************************************************************************/
std::string Versal_2ve_2vmReadImage::GetPartitionType(uint32_t value)
{
    std::string val;
    switch ((value >> v2phtPartitionTypeShift) & v2phtPartitionTypeMask)
    {
        case 0: val = "none";             break;
        case 1: val = "elf";              break;
        case 2: val = "cdo";              break;
        case 3: val = "cfi";              break;
        case 4: val = "raw";              break;
        case 5: val = "raw-elf";          break;
        case 6: val = "cfi-gsc-mask";     break;
        case 7: val = "cfi-gsc-unmask";   break;
        default: val = "invalid";         break;
    }
    return val;
}


