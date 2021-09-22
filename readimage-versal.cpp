/******************************************************************************
* Copyright 2015-2021 Xilinx, Inc.
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

#include "readimage-versal.h"
#include "authentication-versal.h"

#define SEPARATOR "--------------------------------------------------------------------------------"

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
void VersalReadImage::Separator(void)
{
#ifdef SEPARATOR
    std::cout << SEPARATOR << std::endl;
#else
    std::cout << std::endl;
#endif
}

/******************************************************************************/
VersalReadImage::~VersalReadImage()
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

/******************************************************************************/
void VersalReadImage::ReadBinaryFile(DumpOption::Type dump, std::string path)
{
    size_t result;
    uint64_t offset = 0;
    uint32_t index = 0;
    bool smap_header_found = false;
    dumpType = dump;
    dumpPath = path;

    if (StringUtils::GetExtension(binFilename) == ".mcs")
    {
        LOG_ERROR("The option '-read' is not supported on mcs format file : %s", binFilename.c_str());
    }

    FILE *binFile;
    binFile = fopen(binFilename.c_str(), "rb");

    if (!binFile)
    {
        LOG_ERROR("Cannot read file %s", binFilename.c_str());
    }

    // Boot Header Table Extraction
    bH = new VersalBootHeaderStructure;
    result = fread(bH, 1, sizeof(VersalBootHeaderStructure), binFile);
    if ((bH->smapWords[0] == 0xDD000000) || (bH->smapWords[0] == 0x00DD0000) || (bH->smapWords[0] == 0x000000DD))
    {
        smap_header_found = true;
    }
    if (bH->widthDetectionWord != 0xAA995566)
    {
        delete bH;
        bH = NULL;
    }

    if ((dump == DumpOption::BH) || (dump == DumpOption::BOOT_FILES))
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
            result = fwrite(&bH->widthDetectionWord, 1, size_t(sizeof(VersalBootHeaderStructure) - 16), filePtr);
            if (result != sizeof(VersalBootHeaderStructure) - 16)
            {
                LOG_ERROR("Error dumping Boot Header to a file");
            }
            fclose(filePtr);
            LOG_INFO("%s generated successfully", StringUtils::BaseName(fName).c_str());
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
            offset = sizeof(VersalSmapWidthTable);
        }
        else
        {
            offset = 0;
        }
    }
    if (!(fseek(binFile, offset, SEEK_SET)))
    {
        iHT = new VersalImageHeaderTableStructure;
        result = fread(iHT, 1, sizeof(VersalImageHeaderTableStructure), binFile);
        if ((iHT->version != VERSION_v1_00_VERSAL) && (iHT->version != VERSION_v2_00_VERSAL) && (iHT->version != VERSION_v3_00_VERSAL) && (iHT->version != VERSION_v4_00_VERSAL))
        {
            LOG_ERROR("Improper version (0x%.8x) read from Image Header Table of the PDI file.",iHT->version);
        }
        if (result != sizeof(VersalImageHeaderTableStructure))
        {
            LOG_ERROR("Error parsing Image Header Table from PDI file");
        }
    }
    else
    {
        LOG_ERROR("Error parsing Image Header Table from PDI file");
    }

    uint8_t*  header_ac = NULL;
    if (iHT && iHT->headerAuthCertificateWordOffset != 0)
    {
        header_ac = new uint8_t[sizeof(AuthCertificateECDSAStructure)];
        if (!(fseek(binFile, iHT->headerAuthCertificateWordOffset * 4, SEEK_SET)))
        {
            result = fread(header_ac, 1, sizeof(AuthCertificateECDSAStructure), binFile);
            if (result != sizeof(AuthCertificateECDSAStructure))
            {
                LOG_ERROR("Error parsing Header Authentication Certificate from PDI file");
            }
        }
    }
    aCs.push_back(header_ac);
    if (iHT->metaHdrKeySource == KeySource::None)
    {
        offset = iHT->firstImageHeaderWordOffset * 4;
        for (index = 0; index < iHT->imageTotalCount; index++)
        {
            iH = new VersalImageHeaderStructure;
            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(iH, 1, sizeof(VersalImageHeaderStructure), binFile);
                if (result != sizeof(VersalImageHeaderStructure))
                {
                    LOG_ERROR("Error parsing Image Headers from PDI file");
                }
                iHs.push_back(iH);
                offset += sizeof(VersalImageHeaderStructure);
            }
            else
            {
                LOG_ERROR("Error parsing Image Headers from PDI file");
            }
        }

        offset = (iHT->firstImageHeaderWordOffset * 4) + (sizeof(VersalImageHeaderStructure) * iHT->imageTotalCount);
        for (index = 0; index < iHT->partitionTotalCount; index++)
        {
            pHT = new VersalPartitionHeaderTableStructure;

            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(pHT, 1, sizeof(VersalPartitionHeaderTableStructure), binFile);
                if (result != sizeof(VersalPartitionHeaderTableStructure))
                {
                    LOG_ERROR("Error parsing Partition Headers from PDI file");
                }
                pHTs.push_back(pHT);
                offset += sizeof(VersalPartitionHeaderTableStructure);
            }
            else
            {
                LOG_ERROR("Error parsing Partition Headers from PDI file");
            }
        }

        for (std::list<VersalPartitionHeaderTableStructure*>::iterator partitionHdr = pHTs.begin(); partitionHdr != pHTs.end(); partitionHdr++)
        {
            uint8_t* aC = NULL;

            offset = (*partitionHdr)->authCertificateOffset * 4;
            if (offset != 0)
            {
                aC = new uint8_t[sizeof(AuthCertificateECDSAStructure)];
                if (!(fseek(binFile, offset, SEEK_SET)))
                {
                    result = fread(aC, 1, sizeof(AuthCertificateECDSAStructure), binFile);
                    if (result != sizeof(AuthCertificateECDSAStructure))
                    {
                        LOG_ERROR("Error parsing Authentication Certificates from PDI file");
                    }
                }
            }
            aCs.push_back(aC);
        }
    }
    else
    {
        fclose(binFile);
        return;
    }

    uint32_t cnt_index = 0;
    std::list<VersalPartitionHeaderTableStructure*>::iterator pHT = pHTs.begin();
    for (std::list<VersalImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
    {
        uint32_t prev_id = 0xffffffff;
        uint32_t section_count = 0;

        for (cnt_index = 0; cnt_index < (*iH)->dataSectionCount; cnt_index++)
        {
            uint32_t length = (*pHT)->encryptedPartitionLength * 4;
            uint8_t* buffer = new uint8_t[length];
            uint32_t id = (*pHT)->puid & 0xFFFF;
            offset = (*pHT)->partitionWordOffset * 4;
            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(buffer, 1, length, binFile);
                if (result != length)
                {
                    LOG_ERROR("Error parsing partitions from PDI file");
                }
                if (dump == DumpOption::PARTITIONS)
                {
                    if (bH && bH->sourceOffset == offset)
                    {
                        length = bH->totalPlmLength;
                    }
                    if (prev_id == id)
                    {
                        section_count++;
                    }
                    DumpPartitions(buffer, length, (*iH)->imageName, id, section_count);
                }
                else
                {
                    /* Bootloader - compare address offset from BH and PHT */
                    if (bH && bH->sourceOffset == offset)
                    {
                        length = bH->totalPlmLength;
                    }
                    if ((dump == DumpOption::PLM) || (dump == DumpOption::BOOT_FILES))
                    {
                        DumpPartitions(buffer, length, "plm", id, section_count);
                        if (dump == DumpOption::PLM)
                        {
                            delete[] buffer;
                            fclose(binFile);
                            return;
                        }
                    }
                }
                /* For extracting PMC DATA
                Bootloader - compare address offset from BH and PHT */
                if (bH && bH->sourceOffset == offset)
                {
                    if (bH && bH->totalPmcCdoLength != 0)
                    {
                        if ((dump == DumpOption::PARTITIONS) || (dump == DumpOption::PMC_CDO) || (dump == DumpOption::BOOT_FILES))
                        {
                            DumpPartitions(buffer + bH->plmLength, bH->totalPmcCdoLength, "pmc_cdo");
                            if ((dump == DumpOption::PMC_CDO) || (dump == DumpOption::BOOT_FILES))
                            {
                                // if dump boot files, the return from here
                                delete[] buffer;
                                fclose(binFile);
                                return;
                            }
                        }
                    }
                    if (bH && bH->totalPmcCdoLength == 0 && dump == DumpOption::PMC_CDO)
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
            prev_id = id;
            delete[] buffer;
        }
    }
    fclose(binFile);
}

/******************************************************************************/
uint32_t VersalReadImage::GetPdiId(void)
{
    return iHT->pdiId;
}

/******************************************************************************/
uint32_t VersalReadImage::GetParentId(void)
{
    return iHT->parentId;
}

/******************************************************************************/
uint8_t VersalReadImage::GetCreatorId(void)
{
    return ((iHT->imageHeaderTableAttributes) >> 1) & 0xF;
}

/******************************************************************************/
void VersalReadImage::DisplayImageDetails(ReadImageOption::Type type, DumpOption::Type dump, std::string dump_dir)
{
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
        if (iHT->metaHdrKeySource == KeySource::None)
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
void VersalReadImage::DisplayBootHeader(void)
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
    DisplayIV("pmccdo_sec_hdr_iv (0x74) : ", bH->pmcCdoSecureHdrIv);
    DisplayValue("metahdr_offset (0xc4) : ", bH->imageHeaderByteOffset);
    DisplayKey("puf_data (0x928) : ", bH->puf);
    DisplayValue("checksum (0xf30) : ", bH->headerChecksum);
    std::cout << " attribute list - " << std::endl;
    DisplayBhAttributes(bH->bhAttributes);
}

/******************************************************************************/
void VersalReadImage::DumpPartitions(uint8_t* buffer, uint32_t length, std::string name, uint32_t id, uint32_t index)
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
        fName += StringUtils::Format("_%x", id);
        if (index != 0)
        {
            fName += StringUtils::Format(".%x", index);
        }
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
void VersalReadImage::DisplayImageHeaderTable(void)
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
    DisplayValue("hdr_ac (0x48) : ", iHT->headerAuthCertificateWordOffset, "checksum (0x7c) : ", iHT->ihtChecksum);
    DisplayIV("grey/black_iv (0x4C) : ", iHT->metaHdrGreyOrBlackIV);
    std::cout << " attribute list - " << std::endl;
    DisplayIhtAttributes(iHT->imageHeaderTableAttributes);
}

/******************************************************************************/
void VersalReadImage::DisplayImageHeaders(void)
{
#define OWNER(opt) (((opt >> 3) & 7) ? "[plm]" : "[non-plm]")
    int index = 0;
    for (std::list<VersalImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
    {
        index++;
        Separator();
        std::cout << "   IMAGE HEADER " << "(" << (*iH)->imageName << ")" << std::endl;
        Separator();
        DisplayValue("pht_offset (0x00) : ", (*iH)->partitionHeaderWordOffset, "section_count (0x04) : ", (*iH)->dataSectionCount);
        DisplayValue("mHdr_revoke_id (0x08) : ", (*iH)->metaHdrRevokeId, "attributes (0x0c) : ", ((*iH)->imageAttributes));
        DisplayAscii("name (0x10) : ", (*iH)->imageName);
        DisplayValue("id (0x18) : ", (*iH)->imageId, "unique_id (0x24) : ", (*iH)->uniqueId);
        DisplayValue("parent_unique_id (0x28) : ", (*iH)->parentUniqueId, "function_id (0x2c) : ", (*iH)->functionId);
        DisplayValue("memcpy_address_lo (0x30) : ", (*iH)->memcpyAddressLo, "memcpy_address_hi (0x34) : ", (*iH)->memcpyAddressHi);
        DisplayValue("checksum (0x3c) : ", (*iH)->ihChecksum);
        std::cout << " attribute list -" << std::endl;
        DisplayIhAttributes((*iH)->imageAttributes);
    }
}

/******************************************************************************/
void VersalReadImage::DisplayPartitionHeaderTable(void)
{
    uint32_t cnt_index = 0;
    std::list<VersalPartitionHeaderTableStructure*>::iterator pHT = pHTs.begin();
    for (std::list<VersalImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
    {
        for (cnt_index = 0; cnt_index < (*iH)->dataSectionCount; cnt_index++)
        {
            Separator();
            std::cout << "   PARTITION HEADER TABLE " << "(" << (*iH)->imageName << "." << std::dec << cnt_index << ")" << std::endl;
            Separator();
            DisplayValue("encrypted_length (0x00) : ", (*pHT)->encryptedPartitionLength, "unencrypted_length (0x04) : ", (*pHT)->unencryptedPartitionLength);
            DisplayValue("total_length (0x08) : ", (*pHT)->totalPartitionLength, "next_pht (0x0c) : ", (*pHT)->nextPartitionHeaderOffset);
            DisplayValue("exec_addr_lo (0x10) : ", (uint32_t)(*pHT)->destinationExecAddress, "exec_addr_hi (0x14) : ", (uint32_t)((*pHT)->destinationExecAddress >> 32));
            DisplayValue("load_addr_lo (0x18) : ", (uint32_t)(*pHT)->destinationLoadAddress, "load_addr_hi (0x1c) : ", (uint32_t)((*pHT)->destinationLoadAddress >> 32));
            DisplayValue("partition_offset (0x20) : ", (*pHT)->partitionWordOffset, "attributes (0x24) : ", (*pHT)->partitionAttributes);
            DisplayValue("section_count (0x28) : ", (*pHT)->dataSectionCount, "checksum_offset (0x2c) : ", (*pHT)->checksumWordOffset);
            DisplayValue("id (0x30) : ", (*pHT)->puid, "ac_offset (0x34) : ", (*pHT)->authCertificateOffset);
            DisplayIV("sec_hdr_iv (0x38) : ", (*pHT)->partitionSecureHdrIv);
            DisplayValue("encryption_keystore (0x44) : ", (*pHT)->partitionKeySource);
            DisplayIV("grey/black_iv (0x48) : ", (*pHT)->partitionGreyOrBlackIV);
            DisplayValue("revoke_id (0x54) : ", (*pHT)->partitionRevokeId, "checksum (0x7c) : ", (*pHT)->pHChecksum);
            std::cout << " attribute list -" << std::endl;
            DisplayPhtAttributes((*pHT)->partitionAttributes);
            pHT++;
        }
    }
}

/******************************************************************************/
void VersalReadImage::DisplayAuthenicationCertificates(void)
{
    uint32_t cnt_index = 0;
    Authentication::Type auth_type = Authentication::None;
    std::list<uint8_t*>::iterator aC = aCs.begin();

    /* Header AC */
    if ((*aC) != NULL)
    {
        Separator();
        std::cout << "   AUTHENTICATION CERTIFICATE " << "(Meta Header)" << std::endl;
        Separator();
        if ((*(*aC) & 0xF3) == 0x02)
        {
            auth_type = Authentication::ECDSA;
        }
        else if ((*(*aC) & 0xF3) == 0x11)
        {
            auth_type = Authentication::RSA;
        }
        else if ((*(*aC) & 0xF3) == 0x22)
        {
            auth_type = Authentication::ECDSAp521;
        }
        DisplayACFields(*aC, auth_type);
    }
    aC++;

    /* Partition AC */
    for (std::list<VersalImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++)
    {
        for (cnt_index = 0; cnt_index < (*iH)->dataSectionCount; cnt_index++)
        {
            if ((*aC) != NULL)
            {
                Separator();
                std::cout << "   AUTHENTICATION CERTIFICATE " << "(" << (*iH)->imageName << "." << std::dec << cnt_index << ")" << std::endl;
                Separator();
                if ((*(*aC) & 0xF3) == 0x02)
                {
                    auth_type = Authentication::ECDSA;
                }
                else if ((*(*aC) & 0xF3) == 0x11)
                {
                    auth_type = Authentication::RSA;
                }
                else if ((*(*aC) & 0xF3) == 0x22)
                {
                    auth_type = Authentication::ECDSAp521;
                }
                DisplayACFields(*aC, auth_type);
            }
            aC++;
        }
    }
}

/******************************************************************************/
void VersalReadImage::DisplayACFields(uint8_t* aC, Authentication::Type auth_type)
{
    if (auth_type == Authentication::RSA)
    {
        AuthCertificate4096Sha3PaddingStructure* auth_cert = (AuthCertificate4096Sha3PaddingStructure*)(aC);
        DisplayValue("auth_header (0x00) : ", auth_cert->acHeader);
        DisplayValue("revoke_id (0x04) : ", auth_cert->spkId);
        DisplayLongValues("udf (0x08) : ", (uint8_t*)auth_cert->acUdf, 56);
        DisplayLongValues("ppk_mod (0x40) : ", (uint8_t*)auth_cert->acPpk.N, 512);
        DisplayLongValues("ppk_mod_ext (0x240) : ", (uint8_t*)auth_cert->acPpk.N_extension, 512);
        DisplayLongValues("ppk_exponent (0x440) : ", (uint8_t*)auth_cert->acPpk.E, 4);
        DisplayLongValues("ppk_padding (0x444) : ", (uint8_t*)auth_cert->ppkSHA3Padding, 12);
        DisplayLongValues("spk_mod (0x450) : ", (uint8_t*)auth_cert->acSpk.N, 512);
        DisplayLongValues("spk_mod_ext (0x650) : ", (uint8_t*)auth_cert->acSpk.N_extension, 512);
        DisplayLongValues("spk_exponent (0x850) : ", (uint8_t*)auth_cert->acSpk.E, 4);
        DisplayLongValues("spk_padding (0x854) : ", (uint8_t*)auth_cert->spkSHA3Padding, 4);
        DisplayLongValues("spk_signature (0x860) : ", (uint8_t*)(&auth_cert->acSpkSignature), 512);
        DisplayLongValues("bh_signature (0xa60) : ", (uint8_t*)(&auth_cert->acHeaderSignature), 512);
        DisplayLongValues("part_signature (0xc60) : ", (uint8_t*)(&auth_cert->acPartitionSignature), 512);
    }
    else if(auth_type == Authentication::ECDSA)
    {
        AuthCertificateECDSAStructure* auth_cert = (AuthCertificateECDSAStructure*)(aC);
        DisplayValue("auth_header (0x00) : ", auth_cert->acHeader);
        DisplayValue("revoke_id (0x04) : ", auth_cert->spkId);
        DisplayLongValues("udf (0x08) : ", (uint8_t*)auth_cert->acUdf, 56);
        DisplayLongValues("ppk_x (0x40) : ", (uint8_t*)auth_cert->acPpk.x, 48);
        DisplayLongValues("ppk_y (0x70) : ", (uint8_t*)auth_cert->acPpk.y, 48);
        DisplayLongValues("ppk_padding (0x444) : ", (uint8_t*)auth_cert->ppkSHA3Padding, 12);
        DisplayLongValues("spk_x (0x450) : ", (uint8_t*)auth_cert->acSpk.x, 48);
        DisplayLongValues("spk_y (0x480) : ", (uint8_t*)auth_cert->acSpk.y, 48);
        DisplayLongValues("spk_padding (0x854) : ", (uint8_t*)auth_cert->spkSHA3Padding, 4);
        DisplayLongValues("spk_signature (r) (0x860) : ", (uint8_t*)(&auth_cert->acSpkSignature), 48);
        DisplayLongValues("(s) (0x890) : ", ((uint8_t*)(&auth_cert->acSpkSignature) + 48), 48);
        DisplayLongValues("bh_signature (r) (0xa60) : ", (uint8_t*)(&auth_cert->acHeaderSignature), 48);
        DisplayLongValues("(s) (0xa90) : ", ((uint8_t*)(&auth_cert->acHeaderSignature) + 48), 48);
        DisplayLongValues("part_signature (r) (0xc60) : ", (uint8_t*)(&auth_cert->acPartitionSignature), 48);
        DisplayLongValues("(s) (0xc90) : ", ((uint8_t*)(&auth_cert->acPartitionSignature) + 48), 48);
    }
    else
    {
        AuthCertificateECDSAp521Structure* auth_cert = (AuthCertificateECDSAp521Structure*)(aC);
        DisplayValue("auth_header (0x00) : ", auth_cert->acHeader);
        DisplayValue("revoke_id (0x04) : ", auth_cert->spkId);
        DisplayLongValues("udf (0x08) : ", (uint8_t*)auth_cert->acUdf, 56);
        DisplayLongValues("ppk_x (0x40) : ", (uint8_t*)auth_cert->acPpk.x, 66);
        DisplayLongValues("ppk_y (0x70) : ", (uint8_t*)auth_cert->acPpk.y, 66);
        DisplayLongValues("ppk_padding (0x444) : ", (uint8_t*)auth_cert->ppkSHA3Padding, 12);
        DisplayLongValues("spk_x (0x450) : ", (uint8_t*)auth_cert->acSpk.x, 66);
        DisplayLongValues("spk_y (0x480) : ", (uint8_t*)auth_cert->acSpk.y, 66);
        DisplayLongValues("spk_padding (0x854) : ", (uint8_t*)auth_cert->spkSHA3Padding, 4);
        DisplayLongValues("spk_signature (r) (0x860) : ", (uint8_t*)(&auth_cert->acSpkSignature), 66);
        DisplayLongValues("(s) (0x890) : ", ((uint8_t*)(&auth_cert->acSpkSignature) + 66), 66);
        DisplayLongValues("bh_signature (r) (0xa60) : ", (uint8_t*)(&auth_cert->acHeaderSignature), 66);
        DisplayLongValues("(s) (0xa90) : ", ((uint8_t*)(&auth_cert->acHeaderSignature) + 66), 66);
        DisplayLongValues("part_signature (r) (0xc60) : ", (uint8_t*)(&auth_cert->acPartitionSignature), 66);
        DisplayLongValues("(s) (0xc90) : ", ((uint8_t*)(&auth_cert->acPartitionSignature) + 66), 66);
    }
}

/******************************************************************************/
void VersalReadImage::DisplaySmapVectors(void)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << "smap_words (0x00) : ";
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << bH->smapWords[0] << std::setw(8) << std::hex << bH->smapWords[1] << std::setw(8) << std::hex << bH->smapWords[2] << std::setw(8) << std::hex << bH->smapWords[3] << std::endl;
}

/******************************************************************************/
void VersalReadImage::DisplayBhAttributes(uint32_t value)
{
    std::string val, val1;
    
    switch ((value >> PUF_HD_BIT_SHIFT) & PUF_HD_BIT_MASK)
    {
        case 3: val = "[bh]";           break;
        default: val = "[efuse]";       break;
    }
    val1 = val;
    DisplayAttributes("puf_hd_source ", val1,"","");

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
    val1 = val;
    
    switch ((value >> BH_PUF_MODE_BIT_SHIFT) & BH_PUF_MODE_BIT_MASK)
    {
        case 0: val = "[puf-12k]";      break;
        case 3: val = "[puf-4k]";       break;
        default: val = "[invalid]";     break;
    }
    DisplayAttributes("bh_auth ", val1, "puf_mode ", val);
}

/*********************************************************************************/
void VersalReadImage::DisplayIhtAttributes(uint32_t value)
{
    std::string val, val1;

    switch ((value >> vihtIdCodeCheckShift) & vihtIdCodeCheckMask)
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
    DisplayAttributes("id_code_check ", val1, "image_creator ", val);

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
}

/************************************************************************************************/
void VersalReadImage::DisplayIhAttributes(uint32_t value)
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
void VersalReadImage::DisplayPhtAttributes(uint32_t value)
{
    uint32_t core = (value >> vphtDestCpuShift) & vphtDestCpuMask;
    uint32_t arch = (value >> vphtExecStateShift) & vphtExecStateMask;
    uint32_t is_a72 = (core == 1 || core == 2 || core == 3 || core == 4);
    bool is_armv8 = (arch == 0 && is_a72 == 1);
    bool is_armv7 = ((arch == 1 && is_a72 == 1) || core == 5 || core == 6 || core == 7);
    bool is_elf = (((value >> vphtPartitionTypeShift) & vphtPartitionTypeMask) == 1);

    std::string val, val1, val2, val3;
    switch ((value >> vphtTrustzoneShift) & vphtTrustzoneMask)
    {
        case 0: val = "[non-secure]";     break;
        case 1: val = "[secure]";         break;
    }
    if (!is_armv8)
    {
        val = "[n/a]";
    }
    val1 = val;

    switch ((value >> vphtExceptionLevelShift) & vphtExceptionLevelMask)
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
    
    switch ((value >> vphtExecStateShift) & vphtExecStateMask)
    {
        case 0: val = "[aarch-64]";   break;
        case 1: val = "[aarch-32]";   break;
    }
    if (!is_armv8)
    {
        val = "[n/a]";
    }
    val1 = val;

    switch ((value >> vphtDestCpuShift) & vphtDestCpuMask)
    {
        case 0: val = "[none]";           break;
        case 1: val = "[a72-0]";          break;
        case 2: val = "[a72-1]";          break;
        case 3: val = "[a72-2]";          break;
        case 4: val = "[a72-3]";          break;
        case 5: val = "[r5-0]";           break;
        case 6: val = "[r5-1]";           break;
        case 7: val = "[r5-lockstep]";    break;
        case 8: val = "[psm]";            break;
        case 9: val = "[me]";             break;
        default: val = "[invalid]";       break;
    }
    if (!is_elf)
    {
        val = "[n/a]";
    }

    DisplayAttributes("exec_state ", val1, "core ", val);

    switch ((value >> vphtChecksumTypeShift) & vphtChecksumTypeMask)
    {
        case 0: val = "[none]";          break;
        case 3: val = "[sha-3]";         break;
        default: val = "[invalid]";      break;
    }
    val1 = val;
    switch ((value >> vphtPufHDLocationShift) & vphtPufHDLocationMask)
    {
        case 0: val = "[efuse]";        break;
        case 3: val = "[bh]";           break;
        default: val = "[invalid]";     break;
    }

    DisplayAttributes("checksum ", val1, "puf_hd_source", val);

    switch ((value >> vphtPartitionOwnerShift) & vphtPartitionOwnerMask)
    {
        case 0: val = "[plm]";          break;
        case 1: val = "[non-plm]";      break;
        default: val = "[invalid]";     break;
    }
    val1 = val;

    switch ((value >> vphtEndiannessShift) & vphtEndiannessMask)
    {
        case 0: val = "[little-endian]";    break;
        case 1: val = "[big-endian]";       break;
    }
    if (!is_elf)
    {
        val = "[n/a]";
    }

    DisplayAttributes("owner ", val1, "endianness ", val);

    switch ((value >> vphtHivecShift) & vphtHivecMask)
    {
        case 0: val = "[false]";   break;
        case 1: val = "[true]";    break;
    }
    if (!is_armv7)
    {
        val = "[n/a]";
    }
    val1 = val;

    switch ((value >> vphtPartitionTypeShift) & vphtPartitionTypeMask)
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

    switch ((value >> vphtDpaCMShift) & vphtDpaCMMask)
    {
        case 3: val = "[enabled]";      break;
        default: val = "[disabled]";    break;
    }
    val1 = val;
    DisplayAttributes("dpacm ", val1, " ", "");
}
