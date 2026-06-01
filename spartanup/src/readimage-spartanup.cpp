
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
#include <memory>

#include "readimage-spartanup.h"
#include "authentication-spartanup.h"
#include "hash.h"

#define SEPARATOR "--------------------------------------------------------------------------------"

Authentication::Type bl_auth_type_spartanuplus = Authentication::None;
/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
void SpartanupReadImage::Separator(void)
{
#ifdef SEPARATOR
    std::cout << SEPARATOR << std::endl;
#else
    std::cout << std::endl;
#endif
}

/******************************************************************************/
SpartanupReadImage::~SpartanupReadImage()
{
    // Smart pointers automatically clean up memory
}

/**********************************************************************************************/
void SpartanupReadImage::DumpPlainPartition(uint8_t *buffer,uint32_t length, std::string partition_name, bool isBootloader, uint32_t id, uint32_t index)
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
void SpartanupReadImage::ReadPartitions()
{
    size_t result;
    uint64_t offset = 0;
	uint32_t length = 0;
    FILE *binFile = fopen(binFilename.c_str(), "rb");

    if (!binFile)
    {
        LOG_ERROR("Cannot read file %s", binFilename.c_str());
    }
    if (!bH)
    {
        fclose(binFile);
        LOG_ERROR("Boot Header not found in %s", binFilename.c_str());
    }

    offset = bH->sourceOffset;
    length = bH->totalPlmLength;
                
    auto buffer = std::make_unique<uint8_t[]>(length);
    /* Bootloader - compare address offset from BH and PHT */
    if (!(fseek(binFile, offset, SEEK_SET)))
    {
        bool isBootloader = false;
        result = fread(buffer.get(), 1, length, binFile);
        if (result != length)
        {
            LOG_ERROR("Error parsing partitions from PDI file");
        }
        if (bH && bH->sourceOffset == offset)
        {
            length = bH->totalPlmLength;
            isBootloader = true;

        }
        if ((dumpType == DumpOption::PLM) || (dumpType == DumpOption::BOOT_FILES))
        {
            //DumpPartitions(buffer, length, "plm", 0, 0);
            DumpPlainPartition(buffer.get(), length, "plm", isBootloader, 0, 0);
            if (dumpType == DumpOption::PLM)
            {
                fclose(binFile);
                return;
            }
        }
        if (bH && bH->totalPmcCdoLength != 0)
        {
            buffer = std::make_unique<uint8_t[]>(bH->totalPmcCdoLength);
            if ((dumpType == DumpOption::PARTITIONS) || (dumpType == DumpOption::PMC_CDO) || (dumpType == DumpOption::BOOT_FILES))
            {
                if (!(fseek(binFile, bH->sourceOffset + bH->totalPlmLength, SEEK_SET)))
                {
                    result = fread(buffer.get(), 1, bH->totalPmcCdoLength, binFile);
                }
                if (result != bH->totalPmcCdoLength)
                {
                    LOG_ERROR("Error parsing PMC CDO from PDI file");
                }
                //DumpPartitions(buffer, bH->totalPmcCdoLength, "pmc_cdo");
                DumpPlainPartition(buffer.get(), bH->totalPmcCdoLength, "pmc_cdo");//TODO:if ca
            }    
        }       
    }
    fclose(binFile);
}
/******************************************************************************/
uint32_t SpartanupReadImage::ChunkSizeforParitition(std::string partition_name, bool isBootloader)
{
	char * env_SECURE_4K_CHUNK = getenv("SECURE_4K_CHUNK");

	if(partition_name == "plm" || partition_name == "pmc_cdo" || isBootloader == true)
		isBootloader = true;
        if(env_SECURE_4K_CHUNK)
        {
			return SECURE_4K_CHUNK;
        }
        else
        {
			return SECURE_8K_CHUNK;
        }
    
}

/******************************************************************************/
uint32_t SpartanupReadImage::GetHashType(uint32_t authheader)
{
	    uint32_t hashtype;
	    if (dl9Series)
        {
			switch ((authheader & 0xF0) >> 4)
			{
				case 1: hashtype = SHA3_LENGTH_BYTES;
				        break;
				default: hashtype = 0;
				        break;
			}
		}
		else
		{
			switch ((authheader & 0x0C) >> 2)
			{
				case 1: hashtype = SHA2_LENGTH_BYTES;  //SHA3_256(size 32bytes)
				        break;
				default: hashtype = 0;
				        break;
			}
		}
		return hashtype;
}

/******************************************************************************/
uint32_t SpartanupReadImage:: IdentifyAuthtype(uint32_t authheader)
{
    /* Check for non-secure header values first - these indicate no authentication
     * AUTH_HDR_LASSEN_NONSECURE = 0x04 (Lassen devices)
     * AUTH_HDR_TELLURIDE_NONSECURE = 0x110 (Telluride/DL9 devices)
     */
    if (authheader == AUTH_HDR_LASSEN_NONSECURE || authheader == AUTH_HDR_TELLURIDE_NONSECURE)
    {
        bl_auth_type_spartanuplus = Authentication::None;
        return bl_auth_type_spartanuplus;
    }

    if(dl9Series)
    {
        switch (authheader & 0x0F)
        {
            case 2: bl_auth_type_spartanuplus = Authentication::ECDSA; break;
            case 4: bl_auth_type_spartanuplus = Authentication::LMS_SHA2_256; break;
            case 5: bl_auth_type_spartanuplus = Authentication::LMS_SHAKE256; break;
            default : bl_auth_type_spartanuplus = Authentication::None; break;
        }
   }   
   else
   {
       switch (authheader & 0x03)
       {
           case 3: bl_auth_type_spartanuplus = Authentication::LMS_SHA2_256; break;
           default : bl_auth_type_spartanuplus = Authentication::None; break;
       }
   }  
   return bl_auth_type_spartanuplus;
}


/******************************************************************************/
uint32_t SpartanupReadImage::GetACLength(uint32_t AuthOffset, uint32_t ppksize)
{
    auto AC_spkheader = std::make_unique<uint8_t[]>(TELLURIDE_AC_SPK_HDR_LENGTH);
    spkheaderstructure* spkheader  = NULL;
    uint32_t Acsize = 0;
    size_t result;
    FILE *binFile;
    binFile = fopen(binFilename.c_str(), "rb");

    if (!binFile)
    {
        LOG_ERROR("Cannot read file %s", binFilename.c_str());
    }

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
void SpartanupReadImage::ReadHeaderTableDetails()
{
    size_t result;
    uint64_t offset = 0;
    
	uint32_t ac_length = 0;
    FILE *binFile;
    binFile = fopen(binFilename.c_str(), "rb");

    if (!binFile)
    {
        LOG_ERROR("Cannot read file %s", binFilename.c_str());
    }

    // Boot Header Table Extraction
    bH = std::make_unique<SpartanupBootHeaderStructure>();
    result = fread(bH.get(), 1, sizeof(SpartanupBootHeaderStructure), binFile);
    if (bH->widthDetectionWord != 0x665599AA)
    {
        bH.reset();
    }

    /* Auto-detect dl9Series from boot header content.
     * - AUTH_HDR_LASSEN_NONSECURE (0x04) indicates Lassen device (dl9Series=false)
     * - AUTH_HDR_TELLURIDE_NONSECURE (0x110) indicates DL9/Telluride device (dl9Series=true)
     * - Hash block size can also help: 0x90 for Lassen, 0xD0 for DL9
     */
    if (bH)
    {
        if (bH->authHeader1 == AUTH_HDR_LASSEN_NONSECURE ||
            bH->hashBlockLength1 == BH_HASH_BLOCK_BYTES_LASSEN)
        {
            dl9Series = false;
            LOG_WARNING("dl3 detected");
        }
        else if (bH->authHeader1 == AUTH_HDR_TELLURIDE_NONSECURE ||
                 bH->hashBlockLength1 == BH_HASH_BLOCK_BYTES_LASSEN_DL9)
        {
            dl9Series = true;
            LOG_WARNING("dl9 detected");
        }
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
            result = fwrite(&bH->widthDetectionWord, 1, size_t(sizeof(SpartanupBootHeaderStructure) - 16), filePtr);
            if (result != sizeof(SpartanupBootHeaderStructure) - 16)
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
                            
            // Extracting AC for PLM paritition     
            ac_length = GetACLength(sizeof(SpartanupBootHeaderStructure), bH->totalppkkSize1);     //Use ACLength only if Auth is enable
            auto plm_ac = std::make_unique<uint8_t[]>(ac_length);
            if (!(fseek(binFile, sizeof(SpartanupBootHeaderStructure), SEEK_SET)))
            {
                result = fread(plm_ac.get(), 1, ac_length, binFile);
                if (result != ac_length)
                {
                    LOG_ERROR("Error parsing Authentication Certificates for PLM from PDI file");
                }
            }
            aCs.push_back(plm_ac.release());  // Transfer ownership to container
            authtype.push_back(IdentifyAuthtype(bH->authHeader1));
        }
        else{
            aCs.push_back(NULL);
            authtype.push_back(Authentication::None);

        } 
    }

// Hash block0 record
    if(bH && IdentifyAuthtype(bH->authHeader1) != Authentication::None)
        offset = sizeof(SpartanupBootHeaderStructure) + ac_length;
     else 
        offset = sizeof(SpartanupBootHeaderStructure); 
    
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
        Hashblock_record.push_back(std::make_pair(hash_block.release(), bH->hashBlockLength1));  // Transfer ownership 
    }

// Hash Block0 signture 
    if(bH)
        offset = offset + bH->hashBlockLength1;
    if(bH && offset != 0)
    {
        auto hash_block_sign = std::make_unique<uint8_t[]>(bH->totalSignatureSize1);
        if (!(fseek(binFile, offset, SEEK_SET)))
        {
            result = fread(hash_block_sign.get(), 1, bH->totalSignatureSize1, binFile);
            if (result != bH->totalSignatureSize1)
            {
                LOG_ERROR("Error parsing Hash Block0 sign from PDI file");
            }
        }
        Hashblock_record.push_back(std::make_pair(hash_block_sign.release(), bH->totalSignatureSize1));  // Transfer ownership
    }
    fclose(binFile);
    return ;
}

/******************************************************************************/
void SpartanupReadImage::ReadBinaryFile(DumpOption::Type dump, std::string path)
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
uint32_t SpartanupReadImage::GetPdiId(void)
{
    return iHT->pdiId;
}

/******************************************************************************/
uint32_t SpartanupReadImage::GetParentId(void)
{
    return iHT->parentId;
}

/******************************************************************************/
uint8_t SpartanupReadImage::GetCreatorId(void)
{
    return ((iHT->imageHeaderTableAttributes) >> 1) & 0xF;
}

/******************************************************************************/
void SpartanupReadImage::DisplayHeaderTableDetails(ReadImageOption::Type type)
{
    switch (type)
    {
    case ReadImageOption::BH:
        DisplayBootHeader();
        break;

    case ReadImageOption::AC:
        DisplayAuthenicationCertificates();
        break;

    default:
        DisplayBootHeader();
        DisplayAuthenicationCertificates();
        break;          
    }
    Separator();
}

/******************************************************************************/
void SpartanupReadImage::DisplayImageDetails(ReadImageOption::Type type, DumpOption::Type dump, std::string dump_dir)
{
    readType = type;
    ReadBinaryFile(dump, dump_dir);
}

/******************************************************************************/
void SpartanupReadImage::DisplayBootHeader(void)
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
    DisplayValue("revoke_id (0x70) : ", bH->plmRevokeId);
    DisplayValue("auth_header (0x74) : ", bH->authHeader1);
    DisplayValue("hash_block_size (0x78) : ", bH->hashBlockLength1);
    DisplayValue("total_ppk_size (0x7C) : ", bH->totalppkkSize1);
    DisplayValue("actual_ppk_size (0x80) : ", bH->actualppkSize1);
    DisplayValue("total_sign_size (0x84) : ", bH->totalSignatureSize1);
    DisplayValue("actual_sign_size (0x88) : ", bH->actualSignatureSize1);
    DisplayValue("puf_pdi_id (0x8C) : ", bH->pufPDIIdentificationWord);
    DisplayValue("puf_shutter (0x90) : ", bH->shutterValue);
    DisplayValue("puf_ro_swap (0x94) : ", bH->pufRoSwapConfigVal);
    DisplayValue("puf_hd_length (0x98) : ", bH->pufHDLength);
    DisplayValue("pdi_id (0xD8) : ", bH->imageHeaderByteOffset);
    DisplayValue("checksum (0x33C) : ", bH->headerChecksum);
    std::cout << " attribute list - " << std::endl;
    DisplayBhAttributes(bH->bhAttributes);
}

/******************************************************************************/
void SpartanupReadImage::DumpPartitions(uint8_t* buffer, uint32_t length, std::string name, uint32_t id, uint32_t index)
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
void SpartanupReadImage::DisplayAuthenicationCertificates(void)
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
                if(bH && hash_block_itr == Hashblock_record.begin() && dl9Series)
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
                    DisplayLongValues("padding      : ", ((*hash_block_itr).first), 16); 
                    hash_block_itr++;
                    if(auth_type != Authentication::None)
                        DisplayLongValues("HASH Block0 signture   : ", ((*hash_block_itr).first), hash_block_itr->second); 
                    if(hash_block_itr != Hashblock_record.end())    
                        hash_block_itr++;
                    continue;                             
                }
                else if(bH && hash_block_itr == Hashblock_record.begin())
                {
                    /* Lassen (non-dl9) uses 32-byte hashes (SHA3-256) */
                    Separator();
                    std::cout << "   Hash Block0" << std::endl;
                    Separator();
                    DisplayLongValues("BH hash index          : ", ((*hash_block_itr).first), 4);   ((*hash_block_itr).first) += 4; 
                    DisplayLongValues("BH hash digest         : ", ((*hash_block_itr).first) , 32);  ((*hash_block_itr).first) += 32;
                    DisplayLongValues("PLM hash index         : ", ((*hash_block_itr).first), 4);   ((*hash_block_itr).first) += 4;
                    DisplayLongValues("PLM hash digest        : ", ((*hash_block_itr).first), 32);  ((*hash_block_itr).first) += 32;
                    DisplayLongValues("PMC data index         : ", ((*hash_block_itr).first), 4);   ((*hash_block_itr).first) += 4;
                    DisplayLongValues("PMC data digest        : ", ((*hash_block_itr).first), 32);  ((*hash_block_itr).first) += 32;                        
                    DisplayLongValues("padding      : ", ((*hash_block_itr).first), 16); 
                    hash_block_itr++;
                    if(auth_type != Authentication::None)
                        DisplayLongValues("HASH Block0 signture   : ", ((*hash_block_itr).first), hash_block_itr->second); 
                    if(hash_block_itr != Hashblock_record.end())    
                        hash_block_itr++;
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
void SpartanupReadImage::DisplayACFields(uint8_t* aC, Authentication::Type auth_type)
{
    if (auth_type == Authentication::RSA)
    {
        AuthCertificate4096Sha3PaddingHBStructure* auth_cert = (AuthCertificate4096Sha3PaddingHBStructure*)(aC);
        DisplayLongValues("ppk_mod (0x40) : ", (uint8_t*)auth_cert->acPpk.N, 512);
        DisplayLongValues("ppk_mod_ext (0x240) : ", (uint8_t*)auth_cert->acPpk.N_extension, 512);
        DisplayLongValues("ppk_exponent (0x440) : ", (uint8_t*)auth_cert->acPpk.E, 4);
        DisplayLongValues("spk_mod (0x450) : ", (uint8_t*)auth_cert->acSpk.N, 512);
        DisplayLongValues("spk_mod_ext (0x650) : ", (uint8_t*)auth_cert->acSpk.N_extension, 512);
        DisplayLongValues("spk_exponent (0x850) : ", (uint8_t*)auth_cert->acSpk.E, 4);
        DisplayLongValues("spk_signature (0x860) : ", (uint8_t*)(&auth_cert->acSpkSignature), 512);
    }
    else if(auth_type == Authentication::ECDSA)
    {
        AuthCertificateECDSAHBStructure* auth_cert = (AuthCertificateECDSAHBStructure*)(aC);
        DisplayLongValues("ppk_x (0x40) : ", (uint8_t*)auth_cert->acPpk.x, 48);
        DisplayLongValues("ppk_y (0x70) : ", (uint8_t*)auth_cert->acPpk.y, 48);
        DisplayLongValues("spk_x (0x450) : ", (uint8_t*)auth_cert->acSpk.x, 48);
        DisplayLongValues("spk_y (0x480) : ", (uint8_t*)auth_cert->acSpk.y, 48);
        DisplayLongValues("spk_signature (r) (0x860) : ", (uint8_t*)(&auth_cert->acSpkSignature), 48);
        DisplayLongValues("(s) (0x890) : ", ((uint8_t*)(&auth_cert->acSpkSignature) + 48), 48);
    }
    else if(auth_type == Authentication::ECDSAp521)
    {
        AuthCertificateECDSAp521HBStructure* auth_cert = (AuthCertificateECDSAp521HBStructure*)(aC);
        DisplayLongValues("ppk_x (0x40) : ", (uint8_t*)auth_cert->acPpk.x, 66);
        DisplayLongValues("ppk_y (0x70) : ", (uint8_t*)auth_cert->acPpk.y, 66);
        DisplayLongValues("spk_x (0x450) : ", (uint8_t*)auth_cert->acSpk.x, 66);
        DisplayLongValues("spk_y (0x480) : ", (uint8_t*)auth_cert->acSpk.y, 66);
        DisplayLongValues("spk_signature (r) (0x860) : ", (uint8_t*)(&auth_cert->acSpkSignature), 66);
        DisplayLongValues("(s) (0x890) : ", ((uint8_t*)(&auth_cert->acSpkSignature) + 66), 66);
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
void SpartanupReadImage::DisplaySmapVectors(void)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << "smap_words (0x00) : ";
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << bH->smapWords[0] << std::setw(8) << std::hex << bH->smapWords[1] << std::setw(8) << std::hex << bH->smapWords[2] << std::setw(8) << std::hex << bH->smapWords[3] << std::endl;
}

/******************************************************************************/
void SpartanupReadImage::DisplayImageInfo()
{
    Separator();
    std::cout << "   BOOTIMAGE COMPONENTS" << std::endl;
    Separator();

    /*Write valid statements here. Lassen doesn't have PHTs/IHTs - Update display based on PDI format*/
}

/******************************************************************************/
void SpartanupReadImage::DisplayBhAttributes(uint32_t value)
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
        DisplayAttributes("rsa_signed ", val1, "dice ", val);
    }
   
}
