
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

#include "readimage-versal_2vp.h"
#include "authentication-versal_2vp.h"
#include "lms.h"

#define SEPARATOR "--------------------------------------------------------------------------------"

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
void Versal_2vpReadImage::Separator(void)
{
#ifdef SEPARATOR
    std::cout << SEPARATOR << std::endl;
#else
    std::cout << std::endl;
#endif
}

/******************************************************************************/
Versal_2vpReadImage::~Versal_2vpReadImage()
{
    if (bH != NULL)
    {
        delete[] bH;
    }
    if (iH != NULL)
    {
        delete[] iH;
    }
    if (iHT != NULL)
    {
        delete[] iHT;
    }
    if (pHT != NULL)
    {
        delete[] pHT;
    }
}
void Versal_2vpReadImage::DumpPlainPartition(uint8_t *buffer,uint32_t length, std::string partition_name, bool isBootloader, uint32_t id, uint32_t index)
{
    uint8_t *outputBuffer = new uint8_t[length];
    size_t outputIndex = 0;
    size_t inputSize = length;
    size_t hashSize = 0;
    size_t chunksize = ChunkSizeforParitition(partition_name, isBootloader);
    hashSize = GetHashType();
    for (size_t i = 0; i < inputSize; i += chunksize)
    {
        size_t currentChunkSize = std::min(chunksize, inputSize - i);
        if (i + chunksize < inputSize)
        { // Not the last chunk
            size_t bytesToCopy = currentChunkSize - hashSize;
            std::memcpy(outputBuffer + outputIndex, buffer + i, bytesToCopy);
            outputIndex += bytesToCopy;
        }
        else
        { // Last chunk
            std::memcpy(outputBuffer + outputIndex, buffer + i, currentChunkSize);
            outputIndex += currentChunkSize;
        }
    }
    DumpPartitions(outputBuffer, outputIndex, partition_name, id, index);
    delete[] outputBuffer;
}

/**********************************************************************************************/
void Versal_2vpReadImage::ReadPartitions()
{
    size_t result;
    uint64_t offset = 0;
    uint32_t cnt_index = 0;
    FILE *binFile = fopen(binFilename.c_str(), "rb");

    if (!binFile)
    {
        LOG_ERROR("Cannot read file %s", binFilename.c_str());
    }
   
    std::list<Versal_2vpPartitionHeaderTableStructure*>::iterator pHT = pHTs.begin();
    for (std::list<Versal_2vpImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
    {
        uint32_t part_sec_index = 0;
        uint32_t part_sec_count = 0;
        uint32_t part_index = 0;
        PdiImage* pImg = new PdiImage((*iH)->imageName, (*iH)->imageId);

        for (cnt_index = 0; cnt_index < (*iH)->dataSectionCount; cnt_index++)
        {

            uint32_t length = 0; 
            uint8_t* buffer = NULL;
            uint32_t id = (*pHT)->puid & 0xFFFF;
			bool isBootloader = false; 
            if(bH && iH == iHs.begin())    // TODO FIX: WA added to populate plm offset from BH instead of paritition header
            {
                offset = bH->sourceOffset;
                length = bH->totalPlmLength + bH->totalPmcCdoLength;
            }
            else       
            {
                offset = (*pHT)->partitionWordOffset * 4;  
                length = (*pHT)->encryptedPartitionLength * 4; 
            }
            buffer = new uint8_t[length];
                
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
                result = fread(buffer, 1, length, binFile);
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
                    //DumpPartitions(buffer, length, (*iH)->imageName, part_index, part_sec_index);
                    DumpPlainPartition(buffer, length, (*iH)->imageName, isBootloader, part_index, part_sec_index);
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
                         // DumpPartitions(buffer, length, "plm", part_index, part_sec_index);
                         DumpPlainPartition(buffer, length, "plm", isBootloader, part_index, part_sec_index);
                         if (dumpType == DumpOption::PLM)
                        {
                            delete[] buffer;
                            delete pImg;
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
                            
                            if (!(fseek(binFile, bH->sourceOffset + bH->totalPlmLength, SEEK_SET)))
                            {
                                  result = fread(buffer, 1, bH->totalPmcCdoLength, binFile);
                            }
                            if (result != bH->totalPmcCdoLength)
                            {
                                LOG_ERROR("Error parsing PMC CDO from PDI file");
                            }

                            //DumpPartitions(buffer + bH->totalPlmLength, bH->totalPmcCdoLength, "pmc_cdo");
                            DumpPlainPartition(buffer + bH->totalPlmLength, bH->totalPmcCdoLength, "pmc_cdo");
                            if ((dumpType == DumpOption::PMC_CDO) || (dumpType == DumpOption::BOOT_FILES))
                            {
                                // if dump boot files, the return from here
                                delete[] buffer;
                                delete pImg;
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
            delete[] buffer;
        }
        //pdiReadImages.push_back(pImg);
    }
    fclose(binFile);
}
/******************************************************************************/
uint32_t Versal_2vpReadImage::ChunkSizeforParitition(std::string partition_name,  bool isBootloader)
{
	char * env_SECURE_4K_CHUNK = getenv("SECURE_4K_CHUNK");
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
uint32_t Versal_2vpReadImage::GetHashType()
{
    return SHA3_LENGTH_BYTES;
}

/******************************************************************************/
Authentication::Type Versal_2vpReadImage::DetectAuthTypeFromAC(uint32_t acOffset)
{
    uint32_t algorithm = 0;
    size_t result;
    FILE *binFile = fopen(binFilename.c_str(), "rb");
    if (!binFile)
    {
        LOG_ERROR("Cannot read file %s", binFilename.c_str());
    }

    if (!(fseek(binFile, acOffset, SEEK_SET)))
    {
        result = fread(&algorithm, 1, sizeof(uint32_t), binFile);
        if (result != sizeof(uint32_t))
        {
            fclose(binFile);
            return Authentication::None;
        }
    }
    fclose(binFile);

    switch (algorithm & 0xFF)
    {
        case AUTH_HDR_TELLURIDE_RSA_4096:   return Authentication::RSA;
        case AUTH_HDR_TELLURIDE_ECDSA_P384: return Authentication::ECDSA;
        case AUTH_HDR_TELLURIDE_ECDSA_P521: return Authentication::ECDSAp521;
        case AUTH_HDR_TELLURIDE_HSS_LMS:    return Authentication::HSS_SHAKE256;
        case AUTH_HDR_TELLURIDE_LMS:        return Authentication::LMS_SHAKE256;
        case AUTH_HDR_TELLURIDE_ML_DSA87:   return Authentication::MLDSA;
        case AUTH_HDR_TELLURIDE_SLH_DSA:    return Authentication::SLH_SHAKE256;
        default:                            return Authentication::None;
    }
}

/******************************************************************************/
uint32_t Versal_2vpReadImage::GetACLength(uint32_t AuthOffset, uint32_t ppksize)
{
    Authentication::Type authType = DetectAuthTypeFromAC(AuthOffset);
    uint32_t classicSize = 0;
    bool isClassicType = false;

    if (authType == Authentication::RSA)
    {
        classicSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure_versal2vp);
        isClassicType = true;
    }
    else if (authType == Authentication::ECDSA)
    {
        classicSize = sizeof(AuthCertificateECDSAHBStructure_versal2vp);
        isClassicType = true;
    }
    else if (authType == Authentication::ECDSAp521)
    {
        classicSize = sizeof(AuthCertificateECDSAp521HBStructure_versal2vp);
        isClassicType = true;
    }
    else if (authType == Authentication::MLDSA)
    {
        return sizeof(AuthCertificateMLDSAStructure_versal2vp);
    }
    else if (authType == Authentication::SLH_SHAKE256)
    {
        return sizeof(AuthCertificateSLHDSAStructure_versal2vp);
    }
    else if (authType == Authentication::LMS_SHAKE256 || authType == Authentication::LMS_SHA2_256 ||
             authType == Authentication::HSS_SHAKE256 || authType == Authentication::HSS_SHA2_256)
    {
        bool isLmsOnly = (authType == Authentication::LMS_SHAKE256 || authType == Authentication::LMS_SHA2_256);
        uint32_t ppkHeaderSize = sizeof(HeaderAuthCert_versal_2vp);
        uint32_t lmsKeySize = isLmsOnly ? (sizeof(HssPublicKey) - 4) : sizeof(HssPublicKey);
        uint32_t lmsKeyPadded = lmsKeySize + PADDING_16B(lmsKeySize);

        uint32_t spkHeaderSize = sizeof(HeaderAuthCert_versal_2vp);

        uint32_t spkSignLenFieldOffset = AuthOffset + ppkHeaderSize + lmsKeyPadded + spkHeaderSize + lmsKeyPadded;

        uint32_t spkSignLen = 0;
        FILE *binFile = fopen(binFilename.c_str(), "rb");
        if (!binFile) { LOG_ERROR("Cannot read file %s", binFilename.c_str()); }

        if (!(fseek(binFile, spkSignLenFieldOffset, SEEK_SET)))
        {
            size_t result = fread(&spkSignLen, 1, sizeof(uint32_t), binFile);
            if (result != sizeof(uint32_t)) { fclose(binFile); return 0; }
        }

        uint32_t spkSignPadded = spkSignLen + PADDING_16B(spkSignLen);
        uint32_t spkSignFieldSize = 16;

        uint32_t hbSignLenFieldOffset = spkSignLenFieldOffset + spkSignFieldSize + spkSignPadded;
        uint32_t hbSignLen = 0;
        if (!(fseek(binFile, hbSignLenFieldOffset, SEEK_SET)))
        {
            size_t result = fread(&hbSignLen, 1, sizeof(uint32_t), binFile);
            if (result != sizeof(uint32_t)) { fclose(binFile); return 0; }
        }
        fclose(binFile);

        uint32_t hbSignFieldSize = 16;
        uint32_t Acsize = ppkHeaderSize + lmsKeyPadded + spkHeaderSize + lmsKeyPadded +
                          spkSignFieldSize + spkSignPadded + hbSignFieldSize;
        return Acsize;
    }

    if (classicSize == 0)
    {
        return 0;
    }

    if (isClassicType)
    {
        uint32_t pqcAlgorithm = 0;
        FILE *binFile = fopen(binFilename.c_str(), "rb");
        if (binFile)
        {
            if (!(fseek(binFile, AuthOffset + classicSize, SEEK_SET)))
            {
                fread(&pqcAlgorithm, 1, sizeof(uint32_t), binFile);
            }
            fclose(binFile);
        }

        if (pqcAlgorithm == AUTH_HDR_TELLURIDE_HSS_LMS ||
            pqcAlgorithm == AUTH_HDR_TELLURIDE_LMS ||
            pqcAlgorithm == AUTH_HDR_TELLURIDE_ML_DSA87 ||
            pqcAlgorithm == AUTH_HDR_TELLURIDE_SLH_DSA)
        {
            uint32_t pqcSize = GetACLength(AuthOffset + classicSize, 0);
            return classicSize + pqcSize;
        }
    }

    return classicSize;
}

/******************************************************************************/
void Versal_2vpReadImage::ReadHeaderTableDetails()
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
    bH = new Versal_2vpBootHeaderStructure;
    result = fread(bH, 1, sizeof(Versal_2vpBootHeaderStructure), binFile);
    if ((bH->smapWords[0] == 0xDD000000) || (bH->smapWords[0] == 0x00DD0000) || (bH->smapWords[0] == 0x000000DD))
    {
        smap_header_found = true;
    }

    if(bH->widthDetectionWord != 0xAA995566)
    {
        delete bH;
        bH = NULL;        
    }


    if (bH && bH->reginit[0] != 0xFFFFFFFF)
    {
        /*PdiPartition* pRegInit = new VersalPdiPartition(PartitionType::REG_INIT, (uint8_t*)bH->reginit, MAX_REG_INIT_VERSAL);
        pdiReadPartitions.push_back(pRegInit);*/
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
            result = fwrite(&bH->widthDetectionWord, 1, size_t(sizeof(Versal_2vpBootHeaderStructure) - 16), filePtr);
            if (result != sizeof(Versal_2vpBootHeaderStructure) - 16)
            {
                LOG_ERROR("Error dumping Boot Header to a file");
            }
            fclose(filePtr);
            LOG_INFO("%s generated successfully", StringUtils::BaseName(fName).c_str());
        }
    }

    /* versal_2vp BH area layout: BH → Hash Block 0 → PLM AC → PLM data
       Hash Block 0 starts right after BH, PLM AC follows after Hash Block 0. */
    uint8_t* hash_block = NULL;
    if (bH)
    {
        offset = sizeof(Versal_2vpBootHeaderStructure);

        if (bH->hashBlockLength1 > 0)
        {
            hash_block = new uint8_t[bH->hashBlockLength1];
            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(hash_block, 1, bH->hashBlockLength1, binFile);
                if (result != bH->hashBlockLength1)
                {
                    LOG_ERROR("Error parsing Hash Block0 from PDI file");
                }
            }
            Hashblock_record.push_back(std::make_pair(hash_block, bH->hashBlockLength1));
            DumpPartitions(hash_block, bH->hashBlockLength1, "HashBlock0");
        }

        uint32_t plmAcOffset = sizeof(Versal_2vpBootHeaderStructure) + bH->hashBlockLength1;
        if (bH->sourceOffset > plmAcOffset)
        {
            Authentication::Type plmAuthType = DetectAuthTypeFromAC(plmAcOffset);
            if (plmAuthType != Authentication::None)
            {
                Aclength = GetACLength(plmAcOffset, 0);
                uint8_t* plm_ac = new uint8_t[Aclength];
                if (!(fseek(binFile, plmAcOffset, SEEK_SET)))
                {
                    result = fread(plm_ac, 1, Aclength, binFile);
                    if (result != Aclength)
                    {
                        LOG_ERROR("Error parsing Authentication Certificates for PLM from PDI file");
                    }
                }
                aCs.push_back(plm_ac);
                acSizes.push_back(Aclength);
                authtype.push_back(plmAuthType);
            }
            else
            {
                aCs.push_back(NULL);
                acSizes.push_back(0);
                authtype.push_back(Authentication::None);
            }
        }
        else
        {
            aCs.push_back(NULL);
            acSizes.push_back(0);
            authtype.push_back(Authentication::None);
        }
    }

    if (bH)
    {
        offset = bH->imageHeaderByteOffset;
    }
    else
    {
        if(smap_header_found)
        {
            offset = sizeof(Versal_2vpSmapWidthTable);
        }
        else
        {
            offset = 0;
        }
    }


    if (!(fseek(binFile, offset, SEEK_SET)))
    {
        iHT = new Versal_2vpImageHeaderTableStructure;
        result = fread(iHT, 1, sizeof(Versal_2vpImageHeaderTableStructure), binFile);
        if (iHT->version != VERSION_v1_00_TELLURIDE)
        {
            LOG_ERROR("Improper version (0x%.8x) read from Image Header Table of the PDI file.", iHT->version);
        }

        if (result != sizeof(Versal_2vpImageHeaderTableStructure))
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


    uint8_t*  header_ac = NULL;
    if (iHT && iHT->headerAuthCertificateWordOffset != 0)
    {
            offset = iHT->headerAuthCertificateWordOffset * 4;
            Authentication::Type ihtAuthType = DetectAuthTypeFromAC(offset);
            Aclength = GetACLength(offset, 0);
            header_ac = new uint8_t[Aclength];
            if (!(fseek(binFile, offset, SEEK_SET)))  
            {
                result = fread(header_ac, 1, Aclength, binFile);
                if (result != Aclength)
                {
                    LOG_ERROR("Error parsing Header Authentication Certificate from PDI file");
                }
            }
            aCs.push_back(header_ac);
            acSizes.push_back(Aclength);
            authtype.push_back(ihtAuthType);
    }
    else 
    {
        aCs.push_back(NULL);
        acSizes.push_back(0);
        authtype.push_back(Authentication::None);
    }
         //Record hashblock1
       if((iHT->hashBlockLength)*4 != 0)
       {
            header_ac = new uint8_t[(iHT->hashBlockLength)*4];
            if (!(fseek(binFile, (iHT->hashBlockOffset) * 4, SEEK_SET)))  
            {
                result = fread(header_ac, 1, (iHT->hashBlockLength)*4, binFile);
                if (result != (iHT->hashBlockLength)*4)
                {
                    LOG_ERROR("Error parsing record hashblock1 from PDI file");
                }
            }  
            Hashblock_record.push_back(std::make_pair(header_ac,(iHT->hashBlockLength)*4));
       }

    if (iHT->metaHdrKeySource == KeySource::None)
    {
        offset = iHT->firstImageHeaderWordOffset * 4;
        for (index = 0; index < iHT->imageTotalCount; index++)
        {
            iH = new Versal_2vpImageHeaderStructure;
            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(iH, 1, sizeof(Versal_2vpImageHeaderStructure), binFile);
                if (result != sizeof(Versal_2vpImageHeaderStructure))
                {
                    LOG_ERROR("Error parsing Image Headers from PDI file");
                }
                iHs.push_back(iH);
                offset += sizeof(Versal_2vpImageHeaderStructure);
            }
            else
            {
                LOG_ERROR("Error parsing Image Headers from PDI file");
            }
        }

        offset = (iHT->firstImageHeaderWordOffset * 4) + (sizeof(Versal_2vpImageHeaderStructure) * iHT->imageTotalCount);
        for (index = 0; index < iHT->partitionTotalCount; index++)
        {
            pHT = new Versal_2vpPartitionHeaderTableStructure;

            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(pHT, 1, sizeof(Versal_2vpPartitionHeaderTableStructure), binFile);
                if (result != sizeof(Versal_2vpPartitionHeaderTableStructure))
                {
                    LOG_ERROR("Error parsing Partition Headers from PDI file");
                }
                pHTs.push_back(pHT);
                offset += sizeof(Versal_2vpPartitionHeaderTableStructure);
            }
            else
            {
                LOG_ERROR("Error parsing Partition Headers from PDI file");
            }
        }


         /* versal_2vp architecture: Hash Block 0 (BH area) and Hash Block 1 (IHT area) are
           the only hash blocks. Individual partition headers share the IHT hash block
           and do not have separate hash block data. AC and hash block extraction for
           PLM and IHT is already handled above. */
            for (std::list<Versal_2vpPartitionHeaderTableStructure*>::iterator partitionHdr = pHTs.begin();
             partitionHdr != pHTs.end(); partitionHdr++)
            {
            /* Skip the PLM/bootloader partition; its AC + Hash Block 0 were read
               from the boot-header area above. */
            if (bH && partitionHdr == pHTs.begin())
                continue;

            uint32_t acOff = (*partitionHdr)->authCertificateOffset * 4;
            if (acOff == 0)
                continue;

            Authentication::Type at = DetectAuthTypeFromAC(acOff);
            if (at == Authentication::None)
                continue;

            Aclength = GetACLength(acOff, 0);
            uint8_t* aC = new uint8_t[Aclength];
            if (!(fseek(binFile, acOff, SEEK_SET)))
            {
                result = fread(aC, 1, Aclength, binFile);
                if (result != Aclength)
                {
                    LOG_ERROR("Error parsing Authentication Certificates from PDI file");
                }
            }
            aCs.push_back(aC);
            acSizes.push_back(Aclength);
            authtype.push_back(at);

            /* Record this partition's hash block. This path only runs for owner
               (distinct-key) partitions (acOff != 0); their hash_block_offset (0x60)
               and hash_block_length (0x5C) are word values (Address/4, length/4),
               so both scale by 4 to recover the byte address and byte length. */
            uint32_t hbOff = (*partitionHdr)->hashBlockOffset * 4;
            uint32_t hbLen = (*partitionHdr)->hashBlockLength * 4;
            if (hbOff != 0 && hbLen != 0)
            {
                uint8_t* hb = new uint8_t[hbLen];
                if (!(fseek(binFile, hbOff, SEEK_SET)))
                {
                    result = fread(hb, 1, hbLen, binFile);
                    if (result != hbLen)
                    {
                        LOG_ERROR("Error parsing partition Hash Block from PDI file");
                    }
                }
                Hashblock_record.push_back(std::make_pair(hb, hbLen));
                DumpPartitions(hb, hbLen, "HashBlock");
            }
        }
    }

    fclose(binFile);
    return;
}

/******************************************************************************/
void Versal_2vpReadImage::ReadBinaryFile(DumpOption::Type dump, std::string path)
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
#if 0
/******************************************************************************/
std::list<PdiPartition*> Versal_2vpReadImage::GetPdiPartitions(void)
{
    return pdiReadPartitions;
}

/******************************************************************************/
std::list<PdiImage*> Versal_2vpReadImage::GetPdiImages(void)
{
    return pdiReadImages;
}
#endif
/******************************************************************************/
uint32_t Versal_2vpReadImage::GetPdiId(void)
{
    return iHT->pdiId;
}

/******************************************************************************/
uint32_t Versal_2vpReadImage::GetParentId(void)
{
    return iHT->parentId;
}

/******************************************************************************/
uint8_t Versal_2vpReadImage::GetCreatorId(void)
{
    return ((iHT->imageHeaderTableAttributes) >> 1) & 0xF;
}

/******************************************************************************/
void Versal_2vpReadImage::DisplayHeaderTableDetails(ReadImageOption::Type type)
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
void Versal_2vpReadImage::DisplayImageDetails(ReadImageOption::Type type, DumpOption::Type dump, std::string dump_dir)
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
void Versal_2vpReadImage::DisplayBootHeader(void)
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
        #ifdef BUILD_TELLURIDE
        DisplayLongValues("udf_BH (0x7C) : ", (uint8_t*)bH->udfBH, 516);
        #endif 
        DisplayValue("hash_algo (0x27c) : ", bH->hashAlgo);
        DisplayValue("hash_block_length (0x280) : ", bH->hashBlockLength1);
        DisplayValue("metahdr_offset (0x2d4) : ", bH->imageHeaderByteOffset);


	#ifdef BUILD_TELLURIDE
    DisplayKey("puf_data (0xb34) : ", bH->puf);   // PUF data offset
	#endif
    DisplayValue("checksum (0x113c) : ", bH->headerChecksum);
    std::cout << " attribute list - " << std::endl;
    DisplayBhAttributes(bH->bhAttributes);
}

/******************************************************************************/
void Versal_2vpReadImage::DumpPartitions(uint8_t* buffer, uint32_t length, std::string name, uint32_t id, uint32_t index)
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
void Versal_2vpReadImage::DisplayImageHeaderTable(void)
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
        DisplayValue("hash_block_length (0x5C) : ", iHT->hashBlockLength * 4);
        DisplayValue("hash_block_offset (0x60) : ", iHT->hashBlockOffset * 4);
        /* Per versal_2vp spec: 0x64-0x78 are reserved */
        DisplayValue("checksum(0x7c) : ", iHT->ihtChecksum);
        std::cout << " attribute list - " << std::endl;
        DisplayIhtAttributes(iHT->imageHeaderTableAttributes);
    
}

/******************************************************************************/
void Versal_2vpReadImage::DisplayImageHeaders(void)
{

        #define OWNER(opt) (((opt >> 3) & 7) ? "[plm]" : "[non-plm]")
        int index = 0;
        for (std::list<Versal_2vpImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
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
void Versal_2vpReadImage::DisplayPartitionHeaderTable(void)
{

        uint32_t cnt_index = 0;
        std::list<Versal_2vpPartitionHeaderTableStructure*>::iterator pHT = pHTs.begin();
        for (std::list<Versal_2vpImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
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
                DisplayValue("hash_block_length (0x5C) : ", (*pHT)->hashBlockLength * 4);
                DisplayValue("hash_block_offset (0x60) : ", (*pHT)->hashBlockOffset * 4);
                /* Per versal_2vp spec: 0x64-0x78 are reserved */

                DisplayValue("checksum (0x7c) : ", (*pHT)->pHChecksum);
                std::cout << " attribute list -" << std::endl;
                DisplayPhtAttributes((*pHT)->partitionAttributes);
                pHT++;
            }
        }
}

/******************************************************************************/
void Versal_2vpReadImage::DisplayAuthenicationCertificates(void)
{
    std::list<uint8_t*>::iterator aC_itr = aCs.begin();
    std::list<uint32_t>::iterator auth_itr = authtype.begin();
    std::list<uint32_t>::iterator size_itr = acSizes.begin();
    std::list<std::pair<uint8_t*, uint32_t>>::iterator hb_itr = Hashblock_record.begin();

    auto resolveAuthType = [](std::list<uint32_t>::iterator& it) -> Authentication::Type {
        uint32_t val = *it;
        if (val == Authentication::LMS_SHA2_256 || val == Authentication::LMS_SHAKE256 ||
            val == Authentication::HSS_SHA2_256 || val == Authentication::HSS_SHAKE256)
            return Authentication::LMS_SHA2_256;
        return (Authentication::Type)val;
    };

    /* --- Hash Block 0 (BH area: BH + PLM + PMC CDO + HB1 hashes) --- */
    if (bH && hb_itr != Hashblock_record.end() && hb_itr->second > 0)
    {
        Separator();
        std::cout << "   Hash Block0" << std::endl;
        Separator();

        uint8_t* ptr = hb_itr->first;
        uint32_t remaining = hb_itr->second;
        uint32_t numEntries = remaining / 52;
        uint32_t padding = remaining % 52;

        const char* hb0_labels[] = { "BH hash index          : ", "PLM hash index         : ",
                                     "PMC data index         : ", "hash block1 index      : " };
        const char* hb0_digest_labels[] = { "BH hash digest         : ", "PLM hash digest        : ",
                                            "PMC data digest        : ", "hash block1 digest     : " };

        for (uint32_t i = 0; i < numEntries && i < 4; i++)
        {
            DisplayLongValues(hb0_labels[i], ptr, 4); ptr += 4;
            DisplayLongValues(hb0_digest_labels[i], ptr, 48); ptr += 48;
        }
        for (uint32_t i = 4; i < numEntries; i++)
        {
            DisplayLongValues("hash index             : ", ptr, 4); ptr += 4;
            DisplayLongValues("digest                 : ", ptr, 48); ptr += 48;
        }
        if (padding > 0)
            DisplayLongValues("padding                : ", ptr, padding);

        hb_itr++;
    }

    /* --- PLM Authentication Certificate (AC 0) --- */
    if (aC_itr != aCs.end() && auth_itr != authtype.end() && size_itr != acSizes.end())
    {
        Authentication::Type auth_type = resolveAuthType(auth_itr);
        if (*aC_itr != NULL && auth_type != Authentication::None)
        {
            currentAcSize = *size_itr;
            Separator();
            std::cout << "   AUTHENTICATION CERTIFICATE 0" << std::endl;
            Separator();
            DisplayACFields(*aC_itr, auth_type);
        }
        aC_itr++;
        auth_itr++;
        size_itr++;
    }

    /* --- Hash Block 1 (IHT area: hashes for all non-PLM partitions) --- */
    if (hb_itr != Hashblock_record.end() && hb_itr->second > 0)
    {
        Separator();
        std::cout << "   Hash Block1" << std::endl;
        Separator();

        uint8_t* ptr = hb_itr->first;
        uint32_t remaining = hb_itr->second;
        uint32_t numEntries = remaining / 52;
        uint32_t padding = remaining % 52;

        for (uint32_t i = 0; i < numEntries; i++)
        {
            DisplayLongValues("hash index             : ", ptr, 4); ptr += 4;
            DisplayLongValues("digest                 : ", ptr, 48); ptr += 48;
        }
        if (padding > 0)
            DisplayLongValues("padding                : ", ptr, padding);

        hb_itr++;
    }

    /* --- IHT Authentication Certificate (AC 1) --- */
    if (aC_itr != aCs.end() && auth_itr != authtype.end() && size_itr != acSizes.end())
    {
        Authentication::Type auth_type = resolveAuthType(auth_itr);
        if (*aC_itr != NULL && auth_type != Authentication::None)
        {
            currentAcSize = *size_itr;
            Separator();
            std::cout << "   AUTHENTICATION CERTIFICATE 1" << std::endl;
            Separator();
            DisplayACFields(*aC_itr, auth_type);
        }
        aC_itr++;
        auth_itr++;
        size_itr++;
    }

    /* additional per-group hash blocks + ACs (HB2/AC2, HB3/AC3, ...).
       Each distinct-key partition group contributes one hash block followed by its
       own AC; iterate whatever remains instead of stopping at the meta-header group. */
    for (uint32_t groupIdx = 2; aC_itr != aCs.end(); groupIdx++)
    {
        if (hb_itr != Hashblock_record.end() && hb_itr->second > 0)
        {
            Separator();
            std::cout << "   Hash Block" << groupIdx << std::endl;
            Separator();

            uint8_t* ptr = hb_itr->first;
            uint32_t remaining = hb_itr->second;
            uint32_t numEntries = remaining / 52;
            uint32_t padding = remaining % 52;

            for (uint32_t i = 0; i < numEntries; i++)
            {
                DisplayLongValues("hash index             : ", ptr, 4); ptr += 4;
                DisplayLongValues("digest                 : ", ptr, 48); ptr += 48;
            }
            if (padding > 0)
                DisplayLongValues("padding                : ", ptr, padding);

            hb_itr++;
        }

        if (auth_itr != authtype.end() && size_itr != acSizes.end())
        {
            Authentication::Type auth_type = resolveAuthType(auth_itr);
            if (*aC_itr != NULL && auth_type != Authentication::None)
            {
                currentAcSize = *size_itr;
                Separator();
                std::cout << "   AUTHENTICATION CERTIFICATE " << groupIdx << std::endl;
                Separator();
                DisplayACFields(*aC_itr, auth_type);
            }
            aC_itr++;
            auth_itr++;
            size_itr++;
        }
        else
        {
            aC_itr++;
        }
    }
}

/******************************************************************************/
void Versal_2vpReadImage::DisplayACFields(uint8_t* aC, Authentication::Type auth_type)
{
    uint32_t acSize = currentAcSize;
    uint32_t classicSize = 0;

    if (auth_type == Authentication::RSA)
    {
        classicSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure_versal2vp);
        AuthCertificate4096Sha3PaddingHBStructure_versal2vp* auth_cert = (AuthCertificate4096Sha3PaddingHBStructure_versal2vp*)(aC);
        DisplayValue("ppk_header.algorithm : ", auth_cert->ppkheader.Algorithm);
        DisplayValue("ppk_header.revoke_id : ", auth_cert->ppkheader.RevocationID);
        DisplayValue("ppk_header.hybrid : ", auth_cert->ppkheader.Hybrid);
        DisplayLongValues("ppk_mod : ", (uint8_t*)auth_cert->acPpk.N, 512);
        DisplayLongValues("ppk_mod_ext : ", (uint8_t*)auth_cert->acPpk.N_extension, 512);
        DisplayLongValues("ppk_exponent : ", (uint8_t*)auth_cert->acPpk.E, 4);
        DisplayValue("spk_header.algorithm : ", auth_cert->spkheader.Algorithm);
        DisplayValue("spk_header.revoke_id : ", auth_cert->spkheader.RevocationID);
        DisplayValue("spk_header.permission : ", auth_cert->spkheader.Permission);
        DisplayLongValues("spk_mod : ", (uint8_t*)auth_cert->acSpk.N, 512);
        DisplayLongValues("spk_mod_ext : ", (uint8_t*)auth_cert->acSpk.N_extension, 512);
        DisplayLongValues("spk_exponent : ", (uint8_t*)auth_cert->acSpk.E, 4);
        DisplayValue("spk_sign_length : ", auth_cert->acSpkSignatureLength);
        DisplayLongValues("spk_signature : ", (uint8_t*)(&auth_cert->acSpkSignature), 512);
        DisplayValue("hb_sign_length : ", auth_cert->acHashblockSignatureLength);
    }
    else if(auth_type == Authentication::ECDSA)
    {
        classicSize = sizeof(AuthCertificateECDSAHBStructure_versal2vp);
        AuthCertificateECDSAHBStructure_versal2vp* auth_cert = (AuthCertificateECDSAHBStructure_versal2vp*)(aC);
        DisplayValue("ppk_header.algorithm : ", auth_cert->ppkheader.Algorithm);
        DisplayValue("ppk_header.revoke_id : ", auth_cert->ppkheader.RevocationID);
        DisplayValue("ppk_header.hybrid : ", auth_cert->ppkheader.Hybrid);
        DisplayLongValues("ppk_x : ", (uint8_t*)auth_cert->acPpk.x, 48);
        DisplayLongValues("ppk_y : ", (uint8_t*)auth_cert->acPpk.y, 48);
        DisplayValue("spk_header.algorithm : ", auth_cert->spkheader.Algorithm);
        DisplayValue("spk_header.revoke_id : ", auth_cert->spkheader.RevocationID);
        DisplayValue("spk_header.permission : ", auth_cert->spkheader.Permission);
        DisplayLongValues("spk_x : ", (uint8_t*)auth_cert->acSpk.x, 48);
        DisplayLongValues("spk_y : ", (uint8_t*)auth_cert->acSpk.y, 48);
        DisplayValue("spk_sign_length : ", auth_cert->acSpkSignatureLength);
        DisplayLongValues("spk_signature (r) : ", (uint8_t*)(&auth_cert->acSpkSignature), 48);
        DisplayLongValues("spk_signature (s) : ", ((uint8_t*)(&auth_cert->acSpkSignature) + 48), 48);
        DisplayValue("hb_sign_length : ", auth_cert->acHashblockSignatureLength);
    }
    else if(auth_type == Authentication::ECDSAp521)
    {
        classicSize = sizeof(AuthCertificateECDSAp521HBStructure_versal2vp);
        AuthCertificateECDSAp521HBStructure_versal2vp* auth_cert = (AuthCertificateECDSAp521HBStructure_versal2vp*)(aC);
        DisplayValue("ppk_header.algorithm : ", auth_cert->ppkheader.Algorithm);
        DisplayValue("ppk_header.revoke_id : ", auth_cert->ppkheader.RevocationID);
        DisplayValue("ppk_header.hybrid : ", auth_cert->ppkheader.Hybrid);
        DisplayLongValues("ppk_x : ", (uint8_t*)auth_cert->acPpk.x, 66);
        DisplayLongValues("ppk_y : ", (uint8_t*)auth_cert->acPpk.y, 66);
        DisplayValue("spk_header.algorithm : ", auth_cert->spkheader.Algorithm);
        DisplayValue("spk_header.revoke_id : ", auth_cert->spkheader.RevocationID);
        DisplayValue("spk_header.permission : ", auth_cert->spkheader.Permission);
        DisplayLongValues("spk_x : ", (uint8_t*)auth_cert->acSpk.x, 66);
        DisplayLongValues("spk_y : ", (uint8_t*)auth_cert->acSpk.y, 66);
        DisplayValue("spk_sign_length : ", auth_cert->acSpkSignatureLength);
        DisplayLongValues("spk_signature (r) : ", (uint8_t*)(&auth_cert->acSpkSignature), 66);
        DisplayLongValues("spk_signature (s) : ", ((uint8_t*)(&auth_cert->acSpkSignature) + 66), 66);
        DisplayValue("hb_sign_length : ", auth_cert->acHashblockSignatureLength);
    }
    else if(auth_type == Authentication::MLDSA)
    {
        AuthCertificateMLDSAStructure_versal2vp* auth_cert = (AuthCertificateMLDSAStructure_versal2vp*)(aC);
        DisplayValue("ppk_header.algorithm : ", auth_cert->ppkheader.Algorithm);
        DisplayValue("ppk_header.revoke_id : ", auth_cert->ppkheader.RevocationID);
        DisplayValue("ppk_header.hybrid : ", auth_cert->ppkheader.Hybrid);
        DisplayLongValues("ppk : ", (uint8_t*)auth_cert->acPpk, MLDSA_PUB_KEY_LENGTH);
        DisplayValue("spk_header.algorithm : ", auth_cert->spkheader.Algorithm);
        DisplayValue("spk_header.revoke_id : ", auth_cert->spkheader.RevocationID);
        DisplayValue("spk_header.permission : ", auth_cert->spkheader.Permission);
        DisplayLongValues("spk : ", (uint8_t*)auth_cert->acSpk, MLDSA_PUB_KEY_LENGTH);
        DisplayValue("spk_sign_length : ", auth_cert->acSpkSignatureLength);
        DisplayLongValues("spk_signature : ", (uint8_t*)auth_cert->acSpkSignature, MLDSA_TOTAL_SIGN_LEN);
        DisplayValue("hb_sign_length : ", auth_cert->acHashblockSignatureLength);
    }
    else if(auth_type == Authentication::SLH_SHAKE256)
    {
        AuthCertificateSLHDSAStructure_versal2vp* auth_cert = (AuthCertificateSLHDSAStructure_versal2vp*)(aC);
        DisplayValue("ppk_header.algorithm : ", auth_cert->ppkheader.Algorithm);
        DisplayValue("ppk_header.revoke_id : ", auth_cert->ppkheader.RevocationID);
        DisplayValue("ppk_header.hybrid : ", auth_cert->ppkheader.Hybrid);
        DisplayLongValues("ppk : ", (uint8_t*)auth_cert->acPpk, SLHDSA_PUB_KEY_LENGTH);
        DisplayValue("spk_header.algorithm : ", auth_cert->spkheader.Algorithm);
        DisplayValue("spk_header.revoke_id : ", auth_cert->spkheader.RevocationID);
        DisplayValue("spk_header.permission : ", auth_cert->spkheader.Permission);
        DisplayLongValues("spk : ", (uint8_t*)auth_cert->acSpk, SLHDSA_PUB_KEY_LENGTH);
        DisplayValue("spk_sign_length : ", auth_cert->acSpkSignatureLength);
        DisplayLongValues("spk_signature : ", (uint8_t*)auth_cert->acSpkSignature, SLHDSA_TOTAL_SIGN_LENGTH);
        DisplayValue("hb_sign_length : ", auth_cert->acHashblockSignatureLength);
    }
    else if ((auth_type == Authentication::LMS_SHA2_256) || (auth_type == Authentication::LMS_SHAKE256))
    {
        HeaderAuthCert_versal_2vp* ppkHdr = (HeaderAuthCert_versal_2vp*)aC;
        DisplayValue("ppk_header.algorithm : ", ppkHdr->Algorithm);
        DisplayValue("ppk_header.revoke_id : ", ppkHdr->RevocationID);
        DisplayValue("ppk_header.hybrid : ", ppkHdr->Hybrid);
        uint8_t* ptr = aC + sizeof(HeaderAuthCert_versal_2vp);
        std::cout << "---- ppk ----" << std::endl;
        DisplayLongValues("num_levels : ", ptr, 4); ptr += 4;
        DisplayLongValues("lm_type : ", ptr, 4); ptr += 4;
        DisplayLongValues("lm_ots_type : ", ptr, 4); ptr += 4;
        DisplayLongValues("i : ", ptr, 16); ptr += 16;
        DisplayLongValues("k : ", ptr, 32); ptr += 32;
        std::cout << "------------" << std::endl;

        bool isLmsOnly = (ppkHdr->Algorithm & 0xFF) == AUTH_HDR_TELLURIDE_LMS;
        uint32_t lmsKeySize = isLmsOnly ? (sizeof(HssPublicKey) - 4) : sizeof(HssPublicKey);
        uint32_t lmsKeyPadded = lmsKeySize + PADDING_16B(lmsKeySize);
        ptr = aC + sizeof(HeaderAuthCert_versal_2vp) + lmsKeyPadded;

        HeaderAuthCert_versal_2vp* spkHdr = (HeaderAuthCert_versal_2vp*)ptr;
        DisplayValue("spk_header.algorithm : ", spkHdr->Algorithm);
        DisplayValue("spk_header.revoke_id : ", spkHdr->RevocationID);
        DisplayValue("spk_header.permission : ", spkHdr->Permission);
        ptr += sizeof(HeaderAuthCert_versal_2vp);
        std::cout << "---- spk ----" << std::endl;
        DisplayLongValues("num_levels : ", ptr, 4); ptr += 4;
        DisplayLongValues("lm_type : ", ptr, 4); ptr += 4;
        DisplayLongValues("lm_ots_type : ", ptr, 4); ptr += 4;
        DisplayLongValues("i : ", ptr, 16); ptr += 16;
        DisplayLongValues("k : ", ptr, 32); ptr += 32;
        std::cout << "------------" << std::endl;

        ptr = aC + sizeof(HeaderAuthCert_versal_2vp) + lmsKeyPadded +
              sizeof(HeaderAuthCert_versal_2vp) + lmsKeyPadded;
        uint32_t spkSignLen = *(uint32_t*)ptr;
        DisplayValue("spk_sign_length : ", spkSignLen);
        uint8_t* spkSignData = ptr + 16;
        DisplayLongValues("spk_signature : ", spkSignData, spkSignLen);
        uint32_t spkSignPadded = spkSignLen + PADDING_16B(spkSignLen);
        uint8_t* hbSignLenPtr = spkSignData + spkSignPadded;
        uint32_t hbSignLen = *(uint32_t*)hbSignLenPtr;
        DisplayValue("hb_sign_length : ", hbSignLen);
    }

    if (classicSize > 0 && acSize > classicSize)
    {
        uint8_t* pqcAC = aC + classicSize;
        uint32_t pqcAlgorithm = *(uint32_t*)pqcAC;

        Authentication::Type pqcType = Authentication::None;
        switch (pqcAlgorithm)
        {
            case AUTH_HDR_TELLURIDE_LMS:      pqcType = Authentication::LMS_SHAKE256; break;
            case AUTH_HDR_TELLURIDE_HSS_LMS:  pqcType = Authentication::HSS_SHAKE256; break;
            case AUTH_HDR_TELLURIDE_ML_DSA87: pqcType = Authentication::MLDSA; break;
            case AUTH_HDR_TELLURIDE_SLH_DSA:  pqcType = Authentication::SLH_SHAKE256; break;
            default: break;
        }

        if (pqcType != Authentication::None)
        {
            Authentication::Type resolvedPqcType = pqcType;
            if (pqcType == Authentication::LMS_SHAKE256 || pqcType == Authentication::HSS_SHAKE256)
                resolvedPqcType = Authentication::LMS_SHA2_256;

            std::cout << "---- PQC Part ----" << std::endl;
            currentAcSize = acSize - classicSize;
            DisplayACFields(pqcAC, resolvedPqcType);
        }
    }
}

/******************************************************************************/
void Versal_2vpReadImage::DisplaySmapVectors(void)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << "smap_words (0x00) : ";
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << bH->smapWords[0] << std::setw(8) << std::hex << bH->smapWords[1] << std::setw(8) << std::hex << bH->smapWords[2] << std::setw(8) << std::hex << bH->smapWords[3] << std::endl;
}

/******************************************************************************/
void Versal_2vpReadImage::DisplayImageInfo()
{
    Separator();
    std::cout << "   BOOTIMAGE COMPONENTS" << std::endl;
    Separator();

    uint32_t cnt_index = 0;
    std::list<Versal_2vpPartitionHeaderTableStructure*>::iterator pHT = pHTs.begin();
    for (std::list<Versal_2vpImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
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
void Versal_2vpReadImage::DisplayBhAttributes(uint32_t value)
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

    switch ((value >> BH_RSA_BIT_SHIFT) & BH_RSA_BIT_MASK)
    {
        case 3: val = "[enabled]";      break;
        default: val = "[disabled]";    break;
    }
    DisplayAttributes("bh_auth ", val, "", "");
}

/*********************************************************************************/
void Versal_2vpReadImage::DisplayIhtAttributes(uint32_t value)
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
void Versal_2vpReadImage::DisplayIhAttributes(uint32_t value)
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
void Versal_2vpReadImage::DisplayPhtAttributes(uint32_t value)
{
    uint32_t core = (value >> v2vpphtClusterShift) & v2vpphtClusterMask;
    uint32_t arch = (value >> v2vpphtExecStateShift) & v2vpphtExecStateMask;
    uint32_t is_a72 = (core == 1 || core == 2);
    bool is_armv8 = (arch == 0 && is_a72 == 1);
    bool is_armv7 = ((arch == 1 && is_a72 == 1) || core == 5 || core == 6 || core == 7);
    bool is_elf = (((value >> v2vpphtPartitionTypeShift) & v2vpphtPartitionTypeMask) == 1);

    std::string val, val1, val2, val3;
    switch ((value >> v2vpphtTrustzoneShift) & v2vpphtTrustzoneMask)
    {
        case 0: val = "[non-secure]";     break;
        case 1: val = "[secure]";         break;
    }
    if (!is_armv8)
    {
        val = "[n/a]";
    }
    val1 = val;

    switch ((value >> v2vpphtExceptionLevelShift) & v2vpphtExceptionLevelMask)
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
    
    switch ((value >> v2vpphtExecStateShift) & v2vpphtExecStateMask)
    {
        case 0: val = "[aarch-64]";   break;
        case 1: val = "[aarch-32]";   break;
    }
    if (!is_armv8)
    {
        val = "[n/a]";
    }
    val1 = val;

    switch ((value >> v2vpphtDestCpuShift) & v2vpphtDestCpuMask)
    {
        case 0: val = "[none]";           break;
        case 1: val = "[a72-0]";          break;
        case 2: val = "[a72-1]";          break;
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

    switch ((value >> v2vpphtChecksumTypeShift) & v2vpphtChecksumTypeMask)
    {
        case 0: val = "[none]";          break;
        case 3: val = "[sha-3]";         break;
        default: val = "[invalid]";      break;
    }
    val1 = val;
    switch ((value >> v2vpphtPufHDLocationShift) & v2vpphtPufHDLocationMask)
    {
        case 0: val = "[efuse]";        break;
        case 3: val = "[bh]";           break;
        default: val = "[invalid]";     break;
    }

    DisplayAttributes("checksum ", val1, "puf_hd_source", val);

    switch ((value >> v2vpphtPartitionOwnerShift) & v2vpphtPartitionOwnerMask)
    {
        case 0: val = "[plm]";          break;
        case 1: val = "[non-plm]";      break;
        default: val = "[invalid]";     break;
    }
    val1 = val;

    switch ((value >> v2vpphtEndiannessShift) & v2vpphtEndiannessMask)
    {
        case 0: val = "[little-endian]";    break;
        case 1: val = "[big-endian]";       break;
    }
    if (!is_elf)
    {
        val = "[n/a]";
    }

    DisplayAttributes("owner ", val1, "endianness ", val);

    switch ((value >> v2vpphtHivecShift) & v2vpphtHivecMask)
    {
        case 0: val = "[false]";   break;
        case 1: val = "[true]";    break;
    }
    if (!is_armv7)
    {
        val = "[n/a]";
    }
    val1 = val;

    switch ((value >> v2vpphtPartitionTypeShift) & v2vpphtPartitionTypeMask)
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

    switch ((value >> v2vpphtDpaCMShift) & v2vpphtDpaCMMask)
    {
        case 3: val = "[enabled]";      break;
        default: val = "[disabled]";    break;
    }
    val1 = val;
    DisplayAttributes("dpacm ", val1, " ", "");
}

/*******************************************************************************************************/
std::string Versal_2vpReadImage::GetPartitionCore(uint32_t value)
{
    std::string val;
    bool is_elf = (((value >> v2vpphtPartitionTypeShift) & v2vpphtPartitionTypeMask) == 1);

    switch ((value >> v2vpphtDestCpuShift) & v2vpphtDestCpuMask)
    {
        case 0: val = "none";           break;
        case 1: val = "a72-0";          break;
        case 2: val = "a72-1";          break;
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
std::string Versal_2vpReadImage::GetPartitionType(uint32_t value)
{
    std::string val;
    switch ((value >> v2vpphtPartitionTypeShift) & v2vpphtPartitionTypeMask)
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


