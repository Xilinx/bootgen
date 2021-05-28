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

#include "readimage-zynqmp.h"

#define BITSTREAM_AUTH_CHUNK_SIZE  0x800000 //8MB = 8*1024*1024
/*******************************************************************************/
ZynqMpReadImage::~ZynqMpReadImage()
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
void ZynqMpReadImage::ReadBinaryFile(DumpOption::Type dump, std::string path)
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
    bH = new ZynqMpBootHeaderStructure;
    result = fread(bH, 1, sizeof(ZynqMpBootHeaderStructure), binFile);
    if (result != sizeof(ZynqMpBootHeaderStructure))
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
        iHT = new ZynqMpImageHeaderTableStructure;
        result = fread(iHT, 1, sizeof(ZynqMpImageHeaderTableStructure), binFile);
        if (result != sizeof(ZynqMpImageHeaderTableStructure))
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
        iH = new ZynqMpImageHeaderStructure;
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
        pHT = new ZynqMpPartitionHeaderTableStructure;
        if (!(fseek(binFile, offset, SEEK_SET)))
        {
            result = fread(pHT, 1, sizeof(ZynqMpPartitionHeaderTableStructure), binFile);
            if (result != sizeof(ZynqMpPartitionHeaderTableStructure))
            {
                LOG_ERROR("Error reading Partition header table");
            }
            pHTs.push_back(pHT);
        }
        else
        {
            LOG_ERROR("Error parsing Partition Headers from bin file");
        }
        offset += sizeof(ZynqMpPartitionHeaderTableStructure);
    }

    /* Insert Partition Names from Image Header based on Section Count */
    std::list<std::string>::iterator iHName = iHNames.begin();
    for (std::list<ZynqMpImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++, iHName++)
    {
        for (uint32_t sectionIndex = 0; sectionIndex < (*iH)->imageNameLength; sectionIndex++)
        {
            std::stringstream ss;
            ss << sectionIndex;
            pHTNames.push_back(*iHName + "." + ss.str());
        }
    }

    /* Authentication Certificates Extraction */
    uint8_t*  header_ac = NULL;
    if (iHT->headerAuthCertificateWordOffset != 0)
    {
        header_ac = new uint8_t[sizeof(AuthCertificate4096Structure)];
        if (!(fseek(binFile, 4 * (iHT->headerAuthCertificateWordOffset), SEEK_SET)))
        {
            result = fread(header_ac, 1, sizeof(AuthCertificate4096Structure), binFile);
            if (result != sizeof(AuthCertificate4096Structure))
            {
                LOG_ERROR("Error reading header authentication certificate");
            }
        }
    }
    aCs.push_back(header_ac);
    aCNames.push_back("Headers");

    std::list<std::string>::iterator pHTName = pHTNames.begin();
    for (std::list<ZynqMpPartitionHeaderTableStructure*>::iterator partitionHdr = pHTs.begin(); partitionHdr != pHTs.end(); partitionHdr++, pHTName++)
    {
        uint8_t* aC = NULL;
        offset = (*partitionHdr)->authCertificateOffset * 4;

        if (offset != 0)
        {
            int acCount = 1;
            /* Check if the partition is a PL */
            if (((((*partitionHdr)->partitionAttributes) >> PH_DEST_DEVICE_SHIFT_ZYNQMP) & PH_DEST_DEVICE_MASK_ZYNQMP) == 2)
            {
                acCount = ((*partitionHdr)->encryptedPartitionLength * 4) / BITSTREAM_AUTH_CHUNK_SIZE;
                if (((*partitionHdr)->encryptedPartitionLength * 4) % BITSTREAM_AUTH_CHUNK_SIZE != 0)
                {
                    acCount += 1;
                }
                plAcCount = acCount;
            }

            for (int i = 0; i < acCount; i++)
            {
                aC = new uint8_t[sizeof(AuthCertificate4096Structure)];
                if (!(fseek(binFile, offset, SEEK_SET)))
                {
                    result = fread(aC, 1, sizeof(AuthCertificate4096Structure), binFile);
                    if (result != sizeof(AuthCertificate4096Structure))
                    {
                        LOG_ERROR("Error reading header authentication certificate");
                    }
                }

                offset += sizeof(AuthCertificate4096Structure);
                aCs.push_back(aC);
                aCNames.push_back(*pHTName);
            }
        }
    }
    fclose(binFile);
}

/*******************************************************************************/
void ZynqMpReadImage::DisplayBootHeader(void)
{
    Separator();
    std::cout << "   BOOT HEADER" << std::endl;
    Separator();
    DisplayBootVectors();
    DisplayValue("width_detection (0x20) : ", bH->widthDetectionWord);
    DisplayValue("image_id (0x24) : ", bH->identificationWord);
    DisplayValue("encryption_keystore (0x28) : ", bH->encryptionKeySource);
    DisplayValue("fsbl_exec_address (0x2c) : ", bH->fsblExecAddress);
    DisplayValue("fsbl_sourceoffset (0x30) : ", bH->sourceOffset);
    DisplayValue("pmufw_length (0x34) : ", bH->pmuFwLength);
    DisplayValue("pmufw_total_length (0x38) : ", bH->totalPmuFwLength);
    DisplayValue("fsbl_length (0x3c) : ", bH->fsblLength);
    DisplayValue("fsbl_total_length (0x40) : ", bH->totalFsblLength);
    DisplayValue("attributes (0x44) : ", bH->fsblAttributes);
    DisplayValue("checksum (0x48) : ", bH->headerChecksum);
    DisplayKey("grey/black_key (0x4c) : ", bH->greyOrBlackKey);
    DisplayValue("puf_shutter (0x6c) : ", bH->shutterValue);
    //DisplayValue("user_defined_feilds (0x70) : ", bH->udf);
    DisplayValue("iht_offset (0x98) : ", bH->imageHeaderByteOffset);
    DisplayValue("pht_offset (0x9c) : ", bH->partitionHeaderByteOffset);
    DisplayIV("fsbl_sec_hdr_iv (0xa0) : ", bH->secureHdrIv);
    DisplayIV("grey/black_iv (0xac) : ", bH->greyOrBlackIV);

    std::cout << " attribute list - " << std::endl;
    DisplayBhAttributes(bH->fsblAttributes);
}

/*******************************************************************************/
void ZynqMpReadImage::DisplayImageHeaderTable(void)
{
#define BOOT_DEVICE(opt) (opt ? ((opt==7) ? "[pcie]" : "[invalid]") : "[same-dev]")
    Separator();
    std::cout << "   IMAGE HEADER TABLE" << std::endl;
    Separator();
    DisplayValue("version (0x00) : ", iHT->version, "total_images (0x04) : ", iHT->partitionTotalCount);
    DisplayValue("pht_offset (0x08) : ", 4 * (iHT->firstPartitionHeaderWordOffset), "ih_offset (0x0c) : ", 4 * (iHT->firstImageHeaderWordOffset));
    DisplayValue("hdr_ac_offset (0x10) : ", 4 * (iHT->headerAuthCertificateWordOffset), "boot_device (0x14) : ", BOOT_DEVICE(iHT->bootDevice));
    DisplayValue("checksum (0x3c) : ", iHT->ihtChecksum);
}

/*******************************************************************************/
void ZynqMpReadImage::DisplayImageHeaders(void)
{
#define OWNER(opt) (((opt >> 3) & 7) ? "[efsbl]" : "[non-efsbl]")
    uint32_t index = 0;
    std::list<std::string>::iterator name = iHNames.begin();
    for (std::list<ZynqMpImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++, name++)
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
void ZynqMpReadImage::DisplayPartitionHeaderTable(void)
{
    uint32_t cnt_index = 0;
    std::list<ZynqMpPartitionHeaderTableStructure*>::iterator pHT = pHTs.begin();

    std::list<std::string>::iterator name = iHNames.begin();
    for (std::list<ZynqMpImageHeaderStructure*>::iterator iH = iHs.begin(); iH != iHs.end(); iH++, name++)
    {
        for (cnt_index = 0; cnt_index < (*iH)->imageNameLength; cnt_index++)
        {
            Separator();
            std::cout << "   PARTITION HEADER TABLE " << "(" << *name << "." << std::dec << cnt_index << ")" << std::endl;
            Separator();
            DisplayValue("encrypted_length (0x00) : ", (*pHT)->encryptedPartitionLength, "unencrypted_length (0x04) : ", (*pHT)->unencryptedPartitionLength);
            DisplayValue("total_length (0x08) : ", (*pHT)->totalPartitionLength, "next_pht(W) (0x0c) : ", (*pHT)->nextPartitionHeaderOffset);
            DisplayValue("exec_addr_lo (0x10) : ", (uint32_t)(*pHT)->destinationExecAddress, "exec_addr_hi (0x14) : ", (uint32_t)((*pHT)->destinationExecAddress >> 32));
            DisplayValue("load_addr_lo (0x18) : ", (uint32_t)(*pHT)->destinationLoadAddress, "load_addr_hi (0x1c) : ", (uint32_t)((*pHT)->destinationLoadAddress >> 32));
            DisplayValue("partition_offset (0x20) : ", (*pHT)->partitionWordOffset, "attributes (0x24) : ", (*pHT)->partitionAttributes);
            DisplayValue("section_count (0x28) : ", (*pHT)->dataSectionCount, "checksum_offset (0x2c) : ", (*pHT)->checksumWordOffset);
            DisplayValue("iht_offset (0x30) : ", (*pHT)->imageHeaderWordOffset, "ac_offset (0x34) : ", (*pHT)->authCertificateOffset);
            DisplayValue("partition_num (0x38) : ", (*pHT)->partitionNumber, "checksum (0x3c) : ", (*pHT)->pHChecksum);
            std::cout << " attribute list -" << std::endl;
            DisplayPhtAttributes((*pHT)->partitionAttributes);
            pHT++;
        }
    }
}

/*******************************************************************************/
void ZynqMpReadImage::DisplayAuthenicationCertificates(void)
{
    Authentication::Type auth_type;
    std::list<std::string>::iterator aCName = aCNames.begin();

    for (std::list<uint8_t*>::iterator aC = aCs.begin(); aC != aCs.end(); aC++, aCName++)
    {
        if ((*aC) != NULL)
        {
            Separator();
            std::cout << "   AUTHENTICATION CERTIFICATE " << "(" << *aCName << ")" << std::endl;
            Separator();
            auth_type = (Authentication::Type) (*(*aC) & 3);
            DisplayACFields(*aC, auth_type);
        }

    }
}

/*******************************************************************************/
void ZynqMpReadImage::DisplayACFields(uint8_t* aC, Authentication::Type auth_type)
{
    if (auth_type == Authentication::RSA)
    {
        AuthCertificate4096Structure* auth_cert = (AuthCertificate4096Structure*)(aC);
        DisplayValue("auth_header (0x00) : ", auth_cert->acHeader);
        DisplayValue("spk_id (0x04) : ", auth_cert->spkId);
        DisplayLongValues("udf (0x08) : ", (uint8_t*)auth_cert->acUdf, UDF_DATA_SIZE);
        DisplayLongValues("ppk_mod (0x40) : ", (uint8_t*)auth_cert->acPpk.N, RSA_4096_KEY_LENGTH);
        DisplayLongValues("ppk_mod_ext (0x240) : ", (uint8_t*)auth_cert->acPpk.N_extension, RSA_4096_KEY_LENGTH);
        DisplayLongValues("ppk_exponent (0x440) : ", (uint8_t*)auth_cert->acPpk.E, 4);
        DisplayLongValues("ppk_padding (0x444) : ", (uint8_t*)auth_cert->acPpk.Padding, 60);
        DisplayLongValues("spk_mod (0x480) : ", (uint8_t*)auth_cert->acSpk.N, RSA_4096_KEY_LENGTH);
        DisplayLongValues("spk_mod_ext (0x680) : ", (uint8_t*)auth_cert->acSpk.N_extension, RSA_4096_KEY_LENGTH);
        DisplayLongValues("spk_exponent (0x880) : ", (uint8_t*)auth_cert->acSpk.E, 4);
        DisplayLongValues("spk_padding (0x884) : ", (uint8_t*)auth_cert->acSpk.Padding, 60);
        DisplayLongValues("spk_signature (0x8c0) : ", (uint8_t*)(&auth_cert->acSpkSignature), RSA_SIGN_LENGTH_ZYNQMP);
        DisplayLongValues("bh_signature (0xac0) : ", (uint8_t*)(&auth_cert->acBhSignature), RSA_SIGN_LENGTH_ZYNQMP);
        DisplayLongValues("part_signature (0xcc0) : ", (uint8_t*)(&auth_cert->acPartitionSignature), RSA_SIGN_LENGTH_ZYNQMP);
    }
}

/*******************************************************************************/
void ZynqMpReadImage::DisplayBootVectors(void)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << "boot_vectors (0x00) : ";
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << bH->bootVectors[0] << std::setw(8) << std::hex << bH->bootVectors[1] << std::setw(8) << std::hex << bH->bootVectors[2] << std::setw(8) << std::hex << bH->bootVectors[3] << bH->bootVectors[4] << std::setw(8) << std::hex << bH->bootVectors[5] << std::setw(8) << std::hex << bH->bootVectors[6] << std::setw(8) << std::hex << bH->bootVectors[7] << std::endl;
}

/*******************************************************************************/
void ZynqMpReadImage::DisplayBhAttributes(uint32_t value)
{
    std::string val, val1;

    switch ((value >> AUTH_ONLY_BIT_SHIFT) & OPT_KEY_BIT_MASK)
    {
    case 3: val = "[true]";         break;
    default: val = "[false]";       break;
    }

    switch ((value >> AUTH_ONLY_BIT_SHIFT) & AUTH_ONLY_BIT_MASK)
    {
    case 3: val = "[true]";         break;
    default: val = "[false]";       break;
    }
    val1 = val;

    switch ((value >> PUF_HD_BIT_SHIFT) & PUF_HD_BIT_MASK)
    {
    case 3: val = "[bh]";           break;
    default: val = "[efuse]";       break;
    }
    DisplayAttributes("auth_only ", val1, "puf_hd_source ", val);

    switch ((value >> BI_HASH_BIT_SHIFT) & BI_HASH_BIT_MASK)
    {
    case 3: val = "[sha-3]";        break;
    default: val = "[none]";        break;
    }
    val1 = val;

    switch ((value >> CORE_BIT_SHIFT) & CORE_BIT_MASK)
    {
    case 3: val = "[enabled]";      break;
    default: val = "[disabled]";    break;
    }
    DisplayAttributes("checksum ", val1, "core ", val);

    switch ((value >> AUTH_HASH_BIT_SHIFT) & AUTH_HASH_BIT_MASK)
    {
    case 2: val = "[puf-12k]";      break;
    case 3: val = "[puf-4k]";       break;
    default: val = "[invalid]";     break;
    }

    switch ((value >> BH_RSA_BIT_SHIFT) & BH_RSA_BIT_MASK)
    {
    case 3: val = "[enabled]";      break;
    default: val = "[disabled]";    break;
    }
    val1 = val;

    DisplayAttributes("bh_auth ", val1, "puf_mode ", val);
}

/*******************************************************************************/
void ZynqMpReadImage::DisplayPhtAttributes(uint32_t value)
{
    std::string val, val1, val2, val3;
    switch ((value >> PH_TRUSTZONE_SHIFT_ZYNQMP) & PH_TRUSTZONE_MASK_ZYNQMP)
    {
    case 0: val = "[non-secure]";     break;
    case 1: val = "[secure]";         break;
    }
    val1 = val;

    switch ((value >> PH_EXCEPTION_LEVEL_SHIFT_ZYNQMP) & PH_EXCEPTION_LEVEL_MASK_ZYNQMP)
    {
    case 0: val = "[el-0]";     break;
    case 1: val = "[el-1]";     break;
    case 2: val = "[el-2]";     break;
    case 3: val = "[el-3]";     break;
    }
    DisplayAttributes("trustzone ", val1, "el ", val);

    switch ((value >> PH_EXEC_STATE_SHIFT_ZYNQMP) & PH_EXEC_STATE_MASK_ZYNQMP)
    {
    case 0: val = "[aarch-64]";   break;
    case 1: val = "[aarch-32]";   break;
    }
    val1 = val;

    switch ((value >> PH_DEST_DEVICE_SHIFT_ZYNQMP) & PH_DEST_DEVICE_MASK_ZYNQMP)
    {
    case 0: val = "[none]";         break;
    case 1: val = "[PS]";           break;
    case 2: val = "[PL]";           break;
    default: val = "[invalid]";     break;
    }
    DisplayAttributes("exec_state ", val1, "dest_device ", val);

    switch ((value >> PH_ENCRYPT_SHIFT_ZYNQMP) & PH_ENCRYPT_MASK_ZYNQMP)
    {
    case 0: val = "[no]";     break;
    case 1: val = "[yes]";    break;
    }
    val1 = val;

    switch ((value >> PH_DEST_CPU_SHIFT_ZYNQMP) & PH_DEST_CPU_MASK_ZYNQMP)
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

    switch ((value >> PH_CHECKSUM_SHIFT_ZYNQMP) & PH_CHECKSUM_MASK_ZYNQMP)
    {
    case 0: val = "[none]";           break;
    case 3: val = "[sha-3]";          break;
    default: val = "[invalid]";       break;
    }
    val1 = val;

    switch ((value >> PH_AC_FLAG_SHIFT_ZYNQMP) & PH_AC_FLAG_MASK_ZYNQMP)
    {
    case 0: val = "[none]";       break;
    case 1: val = "[rsa]";        break;
    default: val = "[invalid]";   break;
    }
    DisplayAttributes("checksum ", val1, "authentication ", val);

    switch ((value >> PH_OWNER_SHIFT_ZYNQMP) & PH_OWNER_MASK_ZYNQMP)
    {
    case 0: val = "[efsbl]";      break;
    case 1: val = "[non-efsbl]";  break;
    default: val = "[invalid]";   break;
    }
    val1 = val;

    switch ((value >> PH_ENDIAN_SHIFT_ZYNQMP) & PH_ENDIAN_MASK_ZYNQMP)
    {
    case 0: val = "[little-endian]";    break;
    case 1: val = "[big-endian]";       break;
    }
    DisplayAttributes("owner ", val1, "endianness ", val);

    switch ((value >> PH_EARLY_HANDOFF_SHIFT_ZYNQMP) & PH_EARLY_HANDOFF_MASK_ZYNQMP)
    {
    case 0: val = "[no]";     break;
    case 1: val = "[yes]";    break;
    }
    val1 = val;

    switch ((value >> PH_AUTH_BLOCK_SHIFT_ZYNQMP) & PH_AUTH_BLOCK_MASK_ZYNQMP)
    {
    case 0: val = "[none]";   break;
    case 1: val = "[4kb]";    break;
    case 2: val = "[8kb]";    break;
    case 3: val = "[16kb]";   break;
    case 4: val = "[32kb]";   break;
    case 5: val = "[64kb]";   break;
    default: val = "[invalid]";   break;
    }
    DisplayAttributes("early_handoff ", val, "auth_blocks ", val);

    switch ((value >> PH_HIVEC_SHIFT_ZYNQMP) & PH_HIVEC_MASK_ZYNQMP)
    {
    case 0: val = "[false]";   break;
    case 1: val = "[true]";   break;
    }
    std::string empty = "";
    DisplayAttributes("hi-vec ", val, empty, empty);
}