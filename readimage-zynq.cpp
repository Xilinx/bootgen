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
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/

#include "readimage-zynq.h"

/*******************************************************************************/
ZynqReadImage::~ZynqReadImage()
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

/*******************************************************************************/
void ZynqReadImage::ReadBinaryFile(DumpOption::Type dump, std::string path)
{
    size_t result;
    uint64_t offset = 0;
    uint32_t index = 0;

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

    /* Boot Header Extraction */
    bH = new ZynqBootHeaderStructure;
    result = fread(bH, 1, sizeof(ZynqBootHeaderStructure), binFile);
    if (result != sizeof(ZynqBootHeaderStructure))
    {
        LOG_ERROR("Error reading Boot header");
    }
    if ((bH->widthDetectionWord != WIDTH_DETECTION) && (bH->identificationWord != HEADER_ID_WORD))
    {
        LOG_ERROR("Boot Header not found in %s", binFilename.c_str());
    }

    /* Image Header Table Extraction */
    offset = bH->imageHeaderByteOffset;
    if (!(fseek(binFile, offset, SEEK_SET)))
    {
        iHT = new ZynqImageHeaderTableStructure;
        result = fread(iHT, 1, sizeof(ZynqImageHeaderTableStructure), binFile);
        if (result != sizeof(ZynqImageHeaderTableStructure))
        {
            LOG_ERROR("Error reading Image header table");
        }
    }
    else
    {
        LOG_ERROR("Error parsing Image Header Table from bin file");
    }

    char buffer[4];
    char name[256];

    /* Image Header Extraction */
    offset = 4 * (iHT->firstImageHeaderWordOffset);
    do
    {
        iH = new ZynqImageHeaderStructure;
        if (!(fseek(binFile, offset, SEEK_SET)))
        {
            result = fread(iH, 1, 4 * sizeof(uint32_t), binFile);
            if (result != (4 * sizeof(uint32_t)))
            {
                LOG_ERROR("Error reading Image header");
            }
            uint32_t i = 0;
            do
            {
                result = fread(buffer, 1, 4, binFile);
                if (result != 4)
                {
                    LOG_ERROR("Error reading Image header");
                }
                name[i + 0] = buffer[3];
                name[i + 1] = buffer[2];
                name[i + 2] = buffer[1];
                name[i + 3] = buffer[0];
                i += 4;
            } while (buffer[0] && buffer[1] && buffer[2] && buffer[3] && i<sizeof(name));

            if (i >= sizeof(name))
            {
                LOG_DEBUG(DEBUG_STAMP, "Image header name too long");
                LOG_ERROR("Failure parsing imported bootimage");
            }

            iHNames.push_back(name);
            iHs.push_back(iH);
        }
        else
        {
            LOG_ERROR("Error parsing Image Headers from bin file");
        }
        offset = 4 * (iH->nextImageHeaderWordOffset);
    } while (offset != 0);

    /* Partition Header Extraction */
    offset = 4 * (iHT->firstPartitionHeaderWordOffset);
    for (index = 0; index < iHT->partitionTotalCount; index++)
    {
        pHT = new ZynqPartitionHeaderTableStructure;
        if (!(fseek(binFile, offset, SEEK_SET)))
        {
            result = fread(pHT, 1, sizeof(ZynqPartitionHeaderTableStructure), binFile);
            if (result != sizeof(ZynqPartitionHeaderTableStructure))
            {
                LOG_ERROR("Error reading Partition header table");
            }
            pHTs.push_back(pHT);
        }
        else
        {
            LOG_ERROR("Error parsing Partition Headers from bin file");
        }
        offset += sizeof(ZynqPartitionHeaderTableStructure);
    }

    /* Authentication Certificates Extraction */
    uint8_t*  header_ac = NULL;
    if (iHT->headerAuthCertificateWordOffset != 0)
    {
        header_ac = new uint8_t[sizeof(AuthCertificate2048Structure)];
        if (!(fseek(binFile, 4 * (iHT->headerAuthCertificateWordOffset), SEEK_SET)))
        {
            result = fread(header_ac, 1, sizeof(AuthCertificate2048Structure), binFile);
            if (result != sizeof(AuthCertificate2048Structure))
            {
                LOG_ERROR("Error reading header authentication certificate");
            }
        }
    }
    aCs.push_back(header_ac);

    for (std::list<ZynqPartitionHeaderTableStructure*>::iterator partitionHdr = pHTs.begin(); partitionHdr != pHTs.end(); partitionHdr++)
    {
        uint8_t* aC = NULL;

        offset = (*partitionHdr)->authCertificateOffset * 4;
        if (offset != 0)
        {
            aC = new uint8_t[sizeof(AuthCertificate2048Structure)];
            if (!(fseek(binFile, offset, SEEK_SET)))
            {
                result = fread(aC, 1, sizeof(AuthCertificate2048Structure), binFile);
                if (result != sizeof(AuthCertificate2048Structure))
                {
                    LOG_ERROR("Error reading partition authentication certificate");
                }
            }
        }
        aCs.push_back(aC);
    }

    if (header_ac != NULL)
    {
        delete[] header_ac;
    }
    fclose(binFile);
}

/*******************************************************************************/
void ZynqReadImage::DisplayBootHeader(void)
{
    Separator();
    std::cout << "   BOOT HEADER" << std::endl;
    Separator();
    DisplayBootVectors();
    DisplayValue("width_detection (0x20) : ", bH->widthDetectionWord);
    DisplayValue("image_id (0x24) : ", bH->identificationWord);
    DisplayValue("encryption_keystore (0x28) : ", bH->encryptionKeySource);
    DisplayValue("header_version (0x2c) : ", bH->headerVersion);
    DisplayValue("fsbl_sourceoffset (0x30) : ", bH->sourceOffset);
    DisplayValue("fsbl_length (0x34) : ", bH->fsblLength);
    DisplayValue("fsbl_load_address (0x38) : ", bH->fsblLoadAddress);
    DisplayValue("fsbl_exec_address (0x3C) : ", bH->fsblExecAddress);
    DisplayValue("fsbl_total_length (0x40) : ", bH->totalFsblLength);
    DisplayValue("qspi_config-word (0x44) : ", bH->qspiConfigWord);
    DisplayValue("checksum (0x48) : ", bH->headerChecksum);
    //DisplayValue("user_defined_feilds (0x4C) : ", bH->udf);
    DisplayValue("iht_offset (0x98) : ", bH->imageHeaderByteOffset);
    DisplayValue("pht_offset (0x9c) : ", bH->partitionHeaderByteOffset);
}

/*******************************************************************************/
void ZynqReadImage::DisplayImageHeaderTable(void)
{
    Separator();
    std::cout << "   IMAGE HEADER TABLE" << std::endl;
    Separator();
    DisplayValue("version (0x00) : ", iHT->version, "total_images (0x04) : ", iHT->partitionTotalCount);
    DisplayValue("pht_offset (0x08) : ", 4 * (iHT->firstPartitionHeaderWordOffset), "ih_offset (0x0c) : ", 4 * (iHT->firstImageHeaderWordOffset));
    DisplayValue("hdr_ac_offset (0x10) : ", 4 * (iHT->headerAuthCertificateWordOffset));
}

/*******************************************************************************/
void ZynqReadImage::DisplayImageHeaders(void)
{
    uint32_t index = 0;
    std::list<std::string>::iterator name = iHNames.begin();
    for (std::list<ZynqImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++, name++)
    {
        index++;
        Separator();
        std::cout << "   IMAGE HEADER " << "(" << *name << ")" << std::endl;
        Separator();
        DisplayValue("next_ih(W) (0x00) : ", (*iH)->nextImageHeaderWordOffset);
        DisplayValue("next_pht(W) (0x04) : ", (*iH)->partitionHeaderWordOffset);
        DisplayValue("total_partitions (0x08) : ", (*iH)->dataSectionCount);
        DisplayValue("total_partitions (0x0c) : ", (*iH)->imageNameLength);
        DisplayAscii("name (0x10) : ", *name);
    }
}

/*******************************************************************************/
void ZynqReadImage::DisplayPartitionHeaderTable(void)
{
    uint32_t cnt_index = 0;
    std::list<ZynqPartitionHeaderTableStructure*>::iterator pHT = pHTs.begin();

    std::list<std::string>::iterator name = iHNames.begin();
    for (std::list<ZynqImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++, name++)
    {
        for (cnt_index = 0; cnt_index < (*iH)->imageNameLength; cnt_index++)
        {
            Separator();
            std::cout << "   PARTITION HEADER TABLE " << "(" << *name << "." << std::dec << cnt_index << ")" << std::endl;
            Separator();
            DisplayValue("encrypted_length (0x00) : ", (*pHT)->encryptedPartitionLength, "unencrypted_length (0x04) : ", (*pHT)->unencryptedPartitionLength);
            DisplayValue("total_length (0x08) : ", (*pHT)->totalPartitionLength, "load_addr (0x0c) : ", (*pHT)->destinationLoadAddress);
            DisplayValue("exec_addr (0x10) : ", (uint32_t)(*pHT)->destinationExecAddress, "partition_offset (0x14) : ", (*pHT)->partitionWordOffset);
            DisplayValue("attributes (0x18) : ", (*pHT)->partitionAttributes, "section_count (0x1C) : ", (*pHT)->dataSectionCount);
            DisplayValue("checksum_offset (0x20) : ", (*pHT)->checksumWordOffset, "iht_offset (0x24) : ", (*pHT)->imageHeaderWordOffset);
            DisplayValue("ac_offset (0x28) : ", (*pHT)->authCertificateOffset, "checksum (0x3c) : ", (*pHT)->pHChecksum);
            std::cout << " attribute list -" << std::endl;
            DisplayPhtAttributes((*pHT)->partitionAttributes);
            pHT++;
        }
    }
}

/*******************************************************************************/
void ZynqReadImage::DisplayAuthenicationCertificates(void)
{
    uint32_t cnt_index = 0;
    Authentication::Type auth_type;
    std::list<uint8_t*>::iterator aC = aCs.begin();

    /* Header AC */
    if ((*aC) != NULL)
    {
        Separator();
        std::cout << "   AUTHENTICATION CERTIFICATE " << "(Headers)" << std::endl;
        Separator();
        auth_type = (Authentication::Type) (*(*aC) & 3);
        DisplayACFields(*aC, auth_type);
    }
    aC++;

    /* Partition ACs */
    std::list<std::string>::iterator name = iHNames.begin();
    for (std::list<ZynqImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++, name++)
    {
        for (cnt_index = 0; cnt_index < (*iH)->imageNameLength; cnt_index++)
        {
            if ((*aC) != NULL)
            {
                Separator();
                std::cout << "   AUTHENTICATION CERTIFICATE " << "(" << *name << "." << std::dec << cnt_index << ")" << std::endl;
                Separator();
                auth_type = (Authentication::Type) (*(*aC) & 3);
                DisplayACFields(*aC, auth_type);
            }
            aC++;
        }
    }
}

/*******************************************************************************/
void ZynqReadImage::DisplayACFields(uint8_t* aC, Authentication::Type auth_type)
{
    if (auth_type == Authentication::RSA)
    {
        AuthCertificate2048Structure* auth_cert = (AuthCertificate2048Structure*)(aC);
        DisplayValue("auth_header (0x00) : ", auth_cert->acHeader);
        DisplayValue("ac_size (0x04) : ", auth_cert->acSize);
        DisplayLongValues("udf (0x08) : ", (uint8_t*)auth_cert->acUdf, UDF_DATA_SIZE);
        DisplayLongValues("ppk_mod (0x40) : ", (uint8_t*)auth_cert->acPpk.N, RSA_2048_KEY_LENGTH);
        DisplayLongValues("ppk_mod_ext (0x140) : ", (uint8_t*)auth_cert->acPpk.N_extension, RSA_2048_KEY_LENGTH);
        DisplayLongValues("ppk_exponent (0x240) : ", (uint8_t*)auth_cert->acPpk.E, 4);
        DisplayLongValues("ppk_padding (0x244) : ", (uint8_t*)auth_cert->acPpk.Padding, 60);
        DisplayLongValues("spk_mod (0x280) : ", (uint8_t*)auth_cert->acSpk.N, RSA_2048_KEY_LENGTH);
        DisplayLongValues("spk_mod_ext (0x380) : ", (uint8_t*)auth_cert->acSpk.N_extension, RSA_2048_KEY_LENGTH);
        DisplayLongValues("spk_exponent (0x480) : ", (uint8_t*)auth_cert->acSpk.E, 4);
        DisplayLongValues("spk_padding (0x484) : ", (uint8_t*)auth_cert->acSpk.Padding, 60);
        DisplayLongValues("spk_signature (0x4c0) : ", (uint8_t*)(&auth_cert->acSpkSignature), RSA_SIGN_LENGTH_ZYNQ);
        DisplayLongValues("part_signature (0x5c0) : ", (uint8_t*)(&auth_cert->acPartitionSignature), RSA_SIGN_LENGTH_ZYNQ);
    }
}

/*******************************************************************************/
void ZynqReadImage::DisplayBootVectors(void)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << "boot_vectors (0x00) : ";
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << bH->bootVectors[0] << std::setw(8) << std::hex << bH->bootVectors[1] << std::setw(8) << std::hex << bH->bootVectors[2] << std::setw(8) << std::hex << bH->bootVectors[3] << bH->bootVectors[4] << std::setw(8) << std::hex << bH->bootVectors[5] << std::setw(8) << std::hex << bH->bootVectors[6] << std::setw(8) << std::hex << bH->bootVectors[7] << std::endl;
}

/*******************************************************************************/
void ZynqReadImage::DisplayPhtAttributes(uint32_t value)
{
    std::string val, val1, val2, val3;
    switch ((value >> PH_TAIL_ALIGN_SHIFT) & PH_TAIL_ALIGN_MASK)
    {
    case 0: val = "[non-secure]";     break;
    case 1: val = "[secure]";         break;
    }
    val1 = val;

    switch ((value >> PH_HEAD_ALIGN_SHIFT) & PH_HEAD_ALIGN_MASK)
    {
    case 0: val = "[el-0]";     break;
    case 1: val = "[el-1]";     break;
    case 2: val = "[el-2]";     break;
    case 3: val = "[el-3]";     break;
    }
    DisplayAttributes("trustzone ", val1, "el ", val);

    switch ((value >> PH_DEST_DEVICE_SHIFT) & PH_DEST_DEVICE_MASK)
    {
    case 0: val = "[aarch-64]";   break;
    case 1: val = "[aarch-32]";   break;
    }
    val1 = val;

    switch ((value >> PH_CHECKSUM_SHIFT) & PH_CHECKSUM_MASK)
    {
    case 0: val = "[none]";         break;
    case 1: val = "[PS]";           break;
    case 2: val = "[PL]";           break;
    default: val = "[invalid]";     break;
    }
    DisplayAttributes("exec_state ", val1, "dest_device ", val);

    switch ((value >> PH_AC_FLAG_SHIFT) & PH_AC_FLAG_MASK)
    {
    case 0: val = "[no]";     break;
    case 1: val = "[yes]";    break;
    }
    val1 = val;

    switch ((value >> PH_OWNER_SHIFT) & PH_OWNER_MASK)
    {
    case 0: val = "[none]";           break;
    case 1: val = "[a53-0]";          break;
    case 2: val = "[a53-1]";          break;
    case 3: val = "[a53-2]";          break;
    case 4: val = "[a53-3]";          break;
    case 5: val = "[r5-0]";           break;
    case 6: val = "[r5-1]";           break;
    case 7: val = "[r5-lockstep]";    break;
    case 8: val = "[pmu]";            break;
    default: val = "[invalid]";       break;
    }
    DisplayAttributes("encryption ", val1, "core ", val);
}