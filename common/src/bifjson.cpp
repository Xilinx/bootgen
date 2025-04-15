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
#include "cJSON.h"
#include "bifjson.h"
std::string includeBifFile;
const char * env = getenv("ENABLE_EXTRA_BIF_ATTRIBUTE");

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
std::string GetPartitionTypeString(PartitionBifOptions* pOptions)
{
    if (pOptions->bootloader)
    {
        return "bootloader";
    }
    else if (pOptions->bootImage)
    {
        return "bootimage";
    }
    else if (pOptions->pmcData)
    {
        return "pmcdata";
    }
    else if (pOptions->partitionType == PartitionType::CONFIG_DATA_OBJ)
    {
        return "cdo";
    }
    else if (pOptions->partitionType == PartitionType::CFI)
    {
        return "cfi";
    }
    else if (pOptions->partitionType == PartitionType::CFI_GSC)
    {
        return "cfi-gsc";
    }
    else if (pOptions->partitionType == PartitionType::ELF)
    {
        return "elf";
    }
    else if (pOptions->partitionType == PartitionType::SLR_BOOT)
    {
        return "slr-boot";
    }
    else if (pOptions->partitionType == PartitionType::SLR_CONFIG)
    {
        return "slr-config";
    }
    else if (pOptions->partitionType == PartitionType::SLR_SLAVE)
    {
        return "slr-slave";
    }
    else if (pOptions->partitionType == PartitionType::SLR_SLAVE_BOOT)
    {
        return "slr-slave-boot";
    }
    else if (pOptions->partitionType == PartitionType::SLR_SLAVE_CONFIG)
    {
        return "slr-slave-config";
    }
    return "";
}

/******************************************************************************/
std::string GetCoreTypeString(DestinationCPU::Type core)
{
    switch (core)
    {
    case DestinationCPU::A53_0:
        return "a72-0"; break;
    case DestinationCPU::A53_1:
        return "a72-1"; break;
    case DestinationCPU::R5_0:
        return "r5-0"; break;
    case DestinationCPU::R5_1:
        return "r5-1"; break;
    case DestinationCPU::R5_lockstep:
        return "r5-lockstep"; break;
    case DestinationCPU::PMU:
        return "psm"; break;
    default:
        return "";
    }
}

/******************************************************************************/
std::string GetAuthTypeString(Authentication::Type type)
{
    switch (type)
    {
    case Authentication::ECDSA:
        return "ecdsa-p384"; break;
    case Authentication::ECDSAp521:
        return "ecdsa-p521"; break;
    case Authentication::RSA:
        return "rsa"; break;
    default:
        return "";
    }
}

/******************************************************************************/
std::string GetKeySourceString(KeySource::Type key)
{
    switch (key)
    {
    case KeySource::EfuseRedKey:
        return "efuse_red_key"; break;
    case KeySource::EfuseBlkKey:
        return "efuse_blk_key"; break;
    case KeySource::BbramRedKey:
        return "bbram_red_key"; break;
    case KeySource::BbramBlkKey:
        return "bbram_blk_key"; break;
    case KeySource::BhBlkKey:
        return "bh_blk_key"; break;
    case KeySource::UserKey0:
        return "user_key0"; break;
    case KeySource::UserKey1:
        return "user_key1"; break;
    case KeySource::UserKey2:
        return "user_key2"; break;
    case KeySource::UserKey3:
        return "user_key3"; break;
    case KeySource::UserKey4:
        return "user_key4"; break;
    case KeySource::UserKey5:
        return "user_key5"; break;
    case KeySource::UserKey6:
        return "user_key6"; break;
    case KeySource::UserKey7:
        return "user_key7"; break;
    case KeySource::EfuseUserKey0:
        return "efuse_user_key0"; break;
    case KeySource::EfuseUserBlkKey0:
        return "efuse_user_blk_key0"; break;
    case KeySource::EfuseUserKey1:
        return "efuse_user_key1"; break;
    case KeySource::EfuseUserBlkKey1:
        return "efuse_user_blk_key1"; break;
    default:
        return "";
    }
}

/******************************************************************************/
std::string GetChecksumTypeString(Checksum::Type type)
{
    switch (type)
    {
    case Checksum::MD5:
        return "md5"; break;
    case Checksum::SHA2:
        return "sha2"; break;
    case Checksum::SHA3:
        return "sha3"; break;
    default:
        return "";
    }
}

/******************************************************************************/
std::string GetOwnerTypeString(PartitionOwner::Type type)
{
    switch (type)
    {
    case PartitionOwner::FSBL:
        return "plm"; break;
    case PartitionOwner::UBOOT:
        return "non-plm"; break;
    default:
        return "";
    }
}

/******************************************************************************/
std::string GetBootDeviceTypeString(BootDevice::Type type)
{
    switch (type)
    {
    case BootDevice::QSPI32:
        return "qspi32"; break;
    case BootDevice::QSPI24:
        return "qspi24"; break;
    case BootDevice::NAND:
        return "nand"; break;
    case BootDevice::SD0:
        return "sd0"; break;
    case BootDevice::SD1:
        return "sd1"; break;
    case BootDevice::SDLS:
        return "sdls"; break;
    case BootDevice::MMC:
        return "mmc"; break;
    case BootDevice::USB:
        return "usb"; break;
    case BootDevice::ETHERNET:
        return "ethernet"; break;
    case BootDevice::PCIE:
        return "pcie"; break;
    case BootDevice::SATA:
        return "sata"; break;
    case BootDevice::OSPI:
        return "ospi"; break;
    case BootDevice::SMAP:
        return "smap"; break;
    case BootDevice::SBI:
        return "sbi"; break;
    case BootDevice::SD0RAW:
        return "SD0RAW"; break;
    case BootDevice::SD1RAW:
        return "sd1raw"; break;
    case BootDevice::SDLSRAW:
        return "sdlsraw"; break;
    case BootDevice::MMCRAW:
        return "mmcraw"; break;
    case BootDevice::MMC0:
        return "mmc0"; break;
    case BootDevice::MMC0RAW:
        return "mmc0raw"; break;
    case BootDevice::IMAGESTORE:
        return "imagestore"; break;
    default:
        return "";
    }
}

/******************************************************************************/
std::string GetSplitModeString(SplitMode::Type type)
{
    switch (type)
    {
    case SplitMode::Normal:
        return "normal"; break;
    case SplitMode::SlaveMode:
        return "slavemode"; break;
    default:
        return "";
    }
}

/******************************************************************************/
std::string GetAesBlocksString(std::vector<uint32_t> blocks, uint32_t def_blk)
{
    std::string block_string = "";
    uint32_t prev_size = 0;
    uint32_t count = 0;
    for (uint32_t ix = 0; ix < blocks.size(); ix++)
    {
        if (prev_size == blocks.at(ix))
        {
            count++;
        }
        else
        {
            if (count != 0)
            {
                block_string.erase(block_string.size() - 1);
                block_string += "(" + std::to_string(count + 1) + ");";
                count = 0;
            }
            block_string += std::to_string(blocks.at(ix));
            if (ix != (blocks.size() - 1))
            {
                block_string += ";";
            }
            prev_size = blocks.at(ix);
        }
    }
    if (count != 0)
    {
        block_string.erase(block_string.size() - 1);
        block_string += "(" + std::to_string(count + 1) + ")";
        count = 0;
    }
    if (def_blk != 0)
    {
        if(block_string != "")
        {
            block_string += ";";
        }
        block_string += std::to_string(def_blk) + "(*)";
    }
    std::cout << std::endl;
    return block_string;
}

/******************************************************************************/
std::string GetDeviceDna(uint32_t deviceDnaCount, uint8_t* deviceDnaVal)
{
   std::string deviceDnaStr;
   std::stringstream outStream;
   int x = 0;
   
   for(uint32_t i = 0; i < deviceDnaCount; i++)
   {
      deviceDnaStr = std::to_string(deviceDnaVal[i]);
      x = atoi(deviceDnaStr.c_str());
      outStream << std::hex << x;
   }

   return outStream.str();
}

/******************************************************************************/
std::string ConvertToHexString(uint64_t val)
{
    std::stringstream ss;
    ss << std::hex << val;
    return ("0x" + ss.str());
}

/******************************************************************************/
cJSON* CreatePartitionBlock(PartitionBifOptions* partition_option, BifOptions* bif_options)
{
    cJSON *files = NULL;
    cJSON *partition = cJSON_CreateObject();
    std::string input_str;
    std::string bif_string;
    cJSON *extra_part_args = NULL;
    extra_part_args = cJSON_CreateArray();

    if (partition_option->partitionId != 0)
    {
        cJSON_AddItemToObject(partition, "id", cJSON_CreateString(ConvertToHexString(partition_option->partitionId).c_str()));
    }
    std::string p_type = GetPartitionTypeString(partition_option);
    if (p_type != "")
    {
        cJSON_AddItemToObject(partition, "type", cJSON_CreateString(p_type.c_str()));
    }
    if (partition_option->destCPUType != DestinationCPU::NONE)
    {
        cJSON_AddItemToObject(partition, "core", cJSON_CreateString(GetCoreTypeString(partition_option->destCPUType).c_str()));
    }
    if (partition_option->load.Value() != 0)
    {
        cJSON_AddItemToObject(partition, "load", cJSON_CreateString(ConvertToHexString(partition_option->load.Value()).c_str()));
    }
    if (partition_option->offset.Value() != 0)
    {
        cJSON_AddItemToObject(partition, "offset" , cJSON_CreateString(ConvertToHexString(partition_option->offset.Value()).c_str()));
    }
    if (partition_option->reserve.Value() != 0)
    {
        input_str = ConvertToHexString(partition_option->reserve.Value()).c_str();
        bif_string = "reserve = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "reserve", cJSON_CreateString(ConvertToHexString(partition_option->reserve.Value()).c_str()));
        }
    }
    if (partition_option->startup.Value() != 0)
    {
        input_str = ConvertToHexString(partition_option->startup.Value()).c_str();
        bif_string = "startup = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "startup", cJSON_CreateString(ConvertToHexString(partition_option->startup.Value()).c_str()));
        }
    }
    if (partition_option->clusterNum != 0)
    {
        input_str = "0";
        input_str = ConvertToHexString(partition_option->clusterNum).c_str();
        bif_string = "cluster = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "cluster", cJSON_CreateString(ConvertToHexString(partition_option->clusterNum).c_str()));
        }
    }
    if (partition_option->slrNum != 0xFF)
    {
        input_str = ConvertToHexString(partition_option->slrNum).c_str();
        bif_string = "slr = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "slr", cJSON_CreateString(ConvertToHexString(partition_option->slrNum).c_str()));
        }
    }
    if (partition_option->imageStoreId != 0xFF)
    {
        input_str = ConvertToHexString(partition_option->imageStoreId).c_str();
        bif_string = "imagestore = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "imagestore", cJSON_CreateString(ConvertToHexString(partition_option->imageStoreId).c_str()));
        }
    }
    if (partition_option->alignment.Value() != 0)
    {
        input_str = ConvertToHexString(partition_option->alignment.Value()).c_str();
        bif_string = "alignment = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "alignment" , cJSON_CreateString(ConvertToHexString(partition_option->alignment.Value()).c_str()));
        }
    }
    if (partition_option->delayAuth != false)
    {
        bif_string = "delay_auth";
        if (env != NULL) {
           cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(partition, "delay_auth", cJSON_CreateString("true")); 
        }
    }
    if (partition_option->bifSection != "")
    {
        bif_string = "section";
        if (env != NULL) {
           cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(partition_option->bifSection.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(partition, "section", cJSON_CreateString(partition_option->bifSection.c_str()));
        }
    }

    /* Encryption */
    if (partition_option->encryptType == Encryption::AES)
    {
        cJSON_AddItemToObject(partition, "encryption", cJSON_CreateString("aes"));
    }
    if (partition_option->keySrc != KeySource::None)
    {
        cJSON_AddItemToObject(partition, "keysrc", cJSON_CreateString(GetKeySourceString(partition_option->keySrc).c_str()));
    }
    if (partition_option->aesKeyFile != "")
    {
        cJSON_AddItemToObject(partition, "aeskeyfile", cJSON_CreateString(partition_option->aesKeyFile.c_str()));
    }
    if (partition_option->dpaCM != DpaCM::DpaCMDisable)
    {
        cJSON_AddItemToObject(partition, "dpacm_enable", cJSON_CreateString("true"));
    }
    if ((partition_option->blocks.size() != 0) || (partition_option->defBlockSize != 0))
    {
        input_str = GetAesBlocksString(partition_option->blocks, partition_option->defBlockSize).c_str();
        bif_string = "blocks = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(partition, "blocks", cJSON_CreateString(GetAesBlocksString(partition_option->blocks, partition_option->defBlockSize).c_str())); 
        }
    }

    /* Authentication */
    if (partition_option->authType != Authentication::None)
    {
        cJSON_AddItemToObject(partition, "authentication", cJSON_CreateString(GetAuthTypeString(partition_option->authType).c_str()));
    }
    if (partition_option->ppkFile != "")
    {
        cJSON_AddItemToObject(partition, "ppkfile", cJSON_CreateString(partition_option->ppkFile.c_str()));
    }
    else if (bif_options->GetPPKFileName() != "")
    {
        cJSON_AddItemToObject(partition, "ppkfile", cJSON_CreateString(bif_options->GetPPKFileName().c_str()));
    }
    if (partition_option->pskFile != "")
    {
        cJSON_AddItemToObject(partition, "pskfile", cJSON_CreateString(partition_option->pskFile.c_str()));
    }
    else if (bif_options->GetPSKFileName() != "")
    {
        cJSON_AddItemToObject(partition, "pskfile", cJSON_CreateString(bif_options->GetPSKFileName().c_str()));
    }
    if (partition_option->spkFile != "")
    {
        cJSON_AddItemToObject(partition, "spkfile", cJSON_CreateString(partition_option->spkFile.c_str()));
    }
    else if (bif_options->GetSPKFileName() != "")
    {
        cJSON_AddItemToObject(partition, "spkfile", cJSON_CreateString(bif_options->GetSPKFileName().c_str()));
    }
    if (partition_option->sskFile != "")
    {
        cJSON_AddItemToObject(partition, "sskfile", cJSON_CreateString(partition_option->sskFile.c_str()));
    }
    else if (bif_options->GetSSKFileName() != "")
    {
        cJSON_AddItemToObject(partition, "sskfile", cJSON_CreateString(bif_options->GetSSKFileName().c_str()));
    }
    if (partition_option->spkRevokeId != 0)
    {
        input_str = ConvertToHexString(partition_option->spkRevokeId).c_str();
        bif_string = "spk_revoke_id = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "revoke_id", cJSON_CreateString(ConvertToHexString(partition_option->spkRevokeId).c_str()));
        }
    }
    if (partition_option->partitionRevokeId != 0)
    {
        input_str = ConvertToHexString(partition_option->partitionRevokeId).c_str();
        bif_string = "revoke_id = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "revoke_id", cJSON_CreateString(ConvertToHexString(partition_option->partitionRevokeId).c_str()));
        }
    }
    if (partition_option->exceptionLevel != 0x3)
    {
        std::string el = "el-3";
        switch (partition_option->exceptionLevel)
        {
        case 0x0: el = "el-0"; break;
        case 0x1: el = "el-1"; break;
        case 0x2: el = "el-2"; break;
        default: break;
        }
        cJSON_AddItemToObject(partition, "exception_level", cJSON_CreateString(el.c_str()));
    }
    if (partition_option->trustzone != 0)
    {
        cJSON_AddItemToObject(partition, "trustzone", cJSON_CreateString("secure"));
    }
    if (partition_option->checksumType != Checksum::None)
    {
        cJSON_AddItemToObject(partition, "checksum", cJSON_CreateString(GetChecksumTypeString(partition_option->checksumType).c_str()));
    }
    if (partition_option->ownerType != 0)
    {
        input_str = GetOwnerTypeString(partition_option->ownerType).c_str();
        bif_string = "owner = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(partition, "owner", cJSON_CreateString(GetOwnerTypeString(partition_option->ownerType).c_str())); 
        }   
    }
    if (partition_option->hivec != false)
    {
        bif_string = "hivec";
        if (env != NULL) {
            cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "hivec", cJSON_CreateString("true"));
        }    
    }
    if (partition_option->a32Mode != false)
    {
        bif_string = "aarch32_mode";
        if (env != NULL) {
            cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "aarch32_mode", cJSON_CreateString("true"));
        } 
    }
    if (partition_option->bigEndian != false)
    {
        bif_string = "big_endian";
        if (env != NULL) {
            cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "big_endian", cJSON_CreateString("true"));
        }    
    }
    if (partition_option->lockstep != false)
    {
        bif_string = "lockstep";
        if (env != NULL) {
            cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "lockstep", cJSON_CreateString("true"));
        }   
    }
    if (partition_option->pufHdLoc != PufHdLoc::PUFinEFuse)
    {
        bif_string = "pufhd_bh";
        if (env != NULL) {
            cJSON_AddItemToArray(extra_part_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(partition, "pufhd_bh", cJSON_CreateString("true"));
        }
    }
    if (partition_option->spkSignatureFile != "")
    {
        cJSON_AddItemToObject(partition, "spksignaturefile", cJSON_CreateString(partition_option->spkSignatureFile.c_str()));
    }
    if (partition_option->presignFile != "")
    {
        cJSON_AddItemToObject(partition, "presign", cJSON_CreateString(partition_option->presignFile.c_str()));
    }
    if (env != NULL) {
       if (bif_string != "")
       {
          cJSON_AddItemToObject(partition, "extra_partition_args", extra_part_args);
          bif_string.clear();
       }
    }
    
    files = cJSON_CreateArray();
    for (size_t index = 0; index < partition_option->filelist.size(); index++)
    {
        cJSON_AddItemToArray(files, cJSON_CreateString(partition_option->filelist.at(index).c_str()));
    }
    cJSON_AddItemToObject(partition, "files", files);
    return partition;
}

/******************************************************************************/
cJSON* CreatePmcDataPartition(BifOptions* bif_options, Encryption::Type encrypt)
{
    cJSON *files = NULL;
    cJSON *partition = cJSON_CreateObject();
    cJSON_AddItemToObject(partition, "type", cJSON_CreateString("pmcdata"));
    if (bif_options->lastPartitionBifOption->partitionId != 0)
    {
        cJSON_AddItemToObject(partition, "id", cJSON_CreateString(ConvertToHexString(bif_options->lastPartitionBifOption->partitionId).c_str()));
    }
    if (bif_options->GetPmcCdoLoadAddress() != 0)
    {
        cJSON_AddItemToObject(partition, "load", cJSON_CreateString(ConvertToHexString(bif_options->GetPmcCdoLoadAddress()).c_str()));
    }
    if ((encrypt == Encryption::AES) && (bif_options->GetPmcDataAesFile() != ""))
    {
        cJSON_AddItemToObject(partition, "aeskeyfile", cJSON_CreateString(bif_options->GetPmcDataAesFile().c_str()));
    }
    files = cJSON_CreateArray();
    for (size_t index = 0; index < bif_options->GetPmcCdoFileList().size(); index++)
    {
        cJSON_AddItemToArray(files, cJSON_CreateString(bif_options->GetPmcCdoFileList().at(index).c_str()));
    }
    cJSON_AddItemToObject(partition, "files", files);
    return partition;
}

/******************************************************************************/
cJSON* CreateImageBlock(ImageBifOptions* img_option, BifOptions* bif_options)
{
    cJSON *image, *partitions;
    std::string bif_string, input_str;
    cJSON *extra_image_args = NULL;
    extra_image_args = cJSON_CreateArray();

    image = cJSON_CreateObject();
    if (!img_option->partitionBifOptionsList.front()->bootImage)
    {
        cJSON_AddItemToObject(image, "name", cJSON_CreateString(img_option->GetImageName().c_str()));
        cJSON_AddItemToObject(image, "id", cJSON_CreateString(ConvertToHexString(img_option->GetImageId()).c_str()));
    }
    if (img_option->GetMemCopyAddress() != 0xFFFFFFFFFFFFFFFF)
    {
        input_str = ConvertToHexString(img_option->GetMemCopyAddress()).c_str();
        bif_string = "copy = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_image_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(image, "copy", cJSON_CreateString(ConvertToHexString(img_option->GetMemCopyAddress()).c_str()));
        }
    }
    if (img_option->GetUniqueId() != 0xFFFFFFFF)
    {
        input_str = ConvertToHexString(img_option->GetUniqueId()).c_str();
        bif_string = "uid = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_image_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(image, "uid", cJSON_CreateString(ConvertToHexString(img_option->GetUniqueId()).c_str()));
        }
    }
    if (img_option->GetParentUniqueId() != 0xFFFFFFFF)
    {
        input_str = ConvertToHexString(img_option->GetParentUniqueId()).c_str();
        bif_string = "parent_uid = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_image_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(image, "parent_uid", cJSON_CreateString(ConvertToHexString(img_option->GetParentUniqueId()).c_str()));
        }
    }
    if (img_option->GetFunctionId() != 0xFFFFFFFF)
    {
        input_str = ConvertToHexString(img_option->GetFunctionId()).c_str();
        bif_string = "function_id = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_image_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(image, "function_id", cJSON_CreateString(ConvertToHexString(img_option->GetFunctionId()).c_str()));
        }    
    }
    if (img_option->GetPcrNumber() != 0xFFFF)
    {
        input_str = ConvertToHexString(img_option->GetPcrNumber()).c_str();
        bif_string = "pcr = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_image_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(image, "pcr", cJSON_CreateString(ConvertToHexString(img_option->GetPcrNumber()).c_str()));
        }    
    }
    if (img_option->GetPcrMeasurementIndex() != 0xFFFF)
    {
        input_str = ConvertToHexString(img_option->GetPcrMeasurementIndex()).c_str();
        bif_string = "pcr_mindex = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_image_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(image, "pcr_mindex", cJSON_CreateString(ConvertToHexString(img_option->GetPcrMeasurementIndex()).c_str()));
        }    
    }    
    if (img_option->GetDelayLoad() != false)
    {
        bif_string = "delay_load";
        if (env != NULL) {
           cJSON_AddItemToArray(extra_image_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(image, "delay_load", cJSON_CreateString("true"));
        }
    }
    if (img_option->GetDelayHandoff() != false)
    {
        bif_string = "delay_handoff";
        if (env != NULL) {
           cJSON_AddItemToArray(extra_image_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(image, "delay_handoff", cJSON_CreateString("true")); 
        }   
    }
    if (env != NULL) {
       if (bif_string != "")
       {
           cJSON_AddItemToObject(image, "extra_image_args", extra_image_args);
           bif_string.clear();
       }
    }

    partitions = cJSON_CreateArray();
    for (std::list<PartitionBifOptions*>::iterator partitr = img_option->partitionBifOptionsList.begin(); partitr != img_option->partitionBifOptionsList.end(); partitr++)
    {
        cJSON *partition = CreatePartitionBlock(*partitr, bif_options);
        cJSON_AddItemToArray(partitions, partition);
        if ((GetPartitionTypeString(*partitr) == "bootloader") && (bif_options->GetPmcCdoFileList().size() > 0))
        {
            cJSON *pmcdata = CreatePmcDataPartition(bif_options, (*partitr)->encryptType);
            cJSON_AddItemToArray(partitions, pmcdata);
        }
    }
    cJSON_AddItemToObject(image, "partitions", partitions);

    return image;
}

/******************************************************************************/
cJSON* CreateBifBlock(BifOptions* bif_options)
{
    cJSON *metahdr = NULL;
    cJSON *bif_block = NULL;
    cJSON *images = NULL;
    cJSON *authjtag_config = NULL;
    cJSON *boot_device = NULL;
    cJSON *boot_config = NULL;

    std::string input_str;
    std::string bif_string;
    std::string sub_string;
    authjtag_config = cJSON_CreateObject();
    boot_device = cJSON_CreateObject();
    boot_config = cJSON_CreateObject();
    cJSON *extra_bif_args = NULL;
    extra_bif_args = cJSON_CreateArray();

    cJSON *extra_meta_args = NULL;
    extra_meta_args = cJSON_CreateArray();

    bif_block = cJSON_CreateObject();
    cJSON_AddItemToObject(bif_block, "name", cJSON_CreateString(bif_options->GetGroupName().c_str()));

    if (bif_options->GetIdCode() != 0)
    {
        cJSON_AddItemToObject(bif_block, "id_code", cJSON_CreateString(ConvertToHexString(bif_options->GetIdCode()).c_str()));
    }
    if (bif_options->GetExtendedIdCode() != 0)
    {
        cJSON_AddItemToObject(bif_block, "extended_id_code", cJSON_CreateString(ConvertToHexString(bif_options->GetExtendedIdCode()).c_str()));
    }
    if (bif_options->GetPdiId() != 0)
    {
        input_str = ConvertToHexString(bif_options->GetPdiId()).c_str();
        bif_string = "id = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(bif_block, "id", cJSON_CreateString(ConvertToHexString(bif_options->GetPdiId()).c_str()));
        }
    }
    if (bif_options->GetParentId() != 0)
    {
        input_str = ConvertToHexString(bif_options->GetParentId()).c_str();
        bif_string = "parent_id = " + input_str;
        if (env != NULL) {
        cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(bif_block, "parent_id", cJSON_CreateString(ConvertToHexString(bif_options->GetParentId()).c_str()));
        }
    }

    /* auth jtag config */
    if (bif_options->authJtagInfo.spkRevokeId != 0)
    {
        input_str = ConvertToHexString(bif_options->authJtagInfo.spkRevokeId).c_str();
        sub_string = "spk_revoke_id = " + input_str + ",";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(authjtag_config, "spk_revoke_id", cJSON_CreateString(ConvertToHexString(bif_options->authJtagInfo.spkRevokeId).c_str()));
        }
    }
    if (bif_options->authJtagInfo.partitionRevokeId != 0)
    {
        input_str = ConvertToHexString(bif_options->authJtagInfo.partitionRevokeId).c_str();
        sub_string = "revoke_id = " + input_str + ",";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(authjtag_config, "revoke_id", cJSON_CreateString(ConvertToHexString(bif_options->authJtagInfo.partitionRevokeId).c_str()));
        }
    }
    if (bif_options->authJtagInfo.deviceDNA[0] != 0)
    {
        input_str = GetDeviceDna(sizeof(bif_options->authJtagInfo.deviceDNA), bif_options->authJtagInfo.deviceDNA).c_str();
        sub_string += "device_dna = " + input_str + ",";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(authjtag_config, "device_dna", cJSON_CreateString(GetDeviceDna(sizeof(bif_options->authJtagInfo.deviceDNA), bif_options->authJtagInfo.deviceDNA).c_str()));
        }
    }
    if (bif_options->authJtagInfo.jtagTimeout != 0)
    {
        input_str = ConvertToHexString(bif_options->authJtagInfo.jtagTimeout).c_str();
        sub_string += "jtag_timeout = " + input_str + ",";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(authjtag_config, "jtag_timeout", cJSON_CreateString(ConvertToHexString(bif_options->authJtagInfo.jtagTimeout).c_str()));
        }
    }
    if (sub_string != "")
    {
        sub_string.pop_back();
        bif_string = "authjtag_config { " + sub_string + " }";
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "authjtag_config", authjtag_config);
        }
        sub_string.clear();
    }

    /* boot device config */
    if (bif_options->GetBootDevice() != BootDevice::DEFAULT)
    {
        input_str = GetBootDeviceTypeString(bif_options->bootDevice).c_str();
        sub_string = input_str + ",";
        if (!(env != NULL)) {
           cJSON_AddItemToObject(boot_device, "option", cJSON_CreateString(GetBootDeviceTypeString(bif_options->bootDevice).c_str()));
        }
    }
    if (bif_options->GetBootDeviceAddress() != 0)
    {
        input_str = ConvertToHexString(bif_options->GetBootDeviceAddress()).c_str();
        sub_string += "address = " + input_str + ",";
        if (!(env != NULL)) {
           cJSON_AddItemToObject(boot_device, "address", cJSON_CreateString(ConvertToHexString(bif_options->GetBootDeviceAddress()).c_str()));
        }
    }
    if (sub_string != "")
    {
        sub_string.pop_back();
        bif_string = "boot_device { " + sub_string + " }";
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "boot_device", boot_device); 
        }
        sub_string.clear();
    }

    /* boot config */
    if (bif_options->GetBhRsa() != BhRsa::BhRsaDisable)
    {
        sub_string += "bh_auth_enable,";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(boot_config, "bh_auth_enable", cJSON_CreateString("true"));
        }
    }
    if (bif_options->GetSmapWidth() != 32)
    {
        input_str = std::to_string(bif_options->GetSmapWidth());
        sub_string += "smap_width = " + input_str + ",";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(boot_config, "smap_width", cJSON_CreateString(input_str.c_str()));
        }
    }
    if (bif_options->GetShutterValue() != SHUTTER_VAL)
    {
        input_str = ConvertToHexString(bif_options->GetShutterValue()).c_str();
        sub_string += "shutter = " + input_str + ",";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(boot_config, "shutter", cJSON_CreateString(ConvertToHexString(bif_options->GetShutterValue()).c_str()));
        }
    }
    if (bif_options->GetDpaCM() != DpaCM::DpaCMDisable)
    {
        sub_string += "dpacm_enable,";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(boot_config, "dpacm_enable", cJSON_CreateString("true"));
        }
    }
    if (bif_options->GetPufMode() != PufMode::PUF12K)
    {
        sub_string += "puf4kmode,";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(boot_config, "puf4kmode", cJSON_CreateString("true"));
        }
    }
    if (bif_options->GetPufHdLoc() != PufHdLoc::PUFinEFuse)
    {
        sub_string += "pufhd_bh,";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(boot_config, "pufhd_bh", cJSON_CreateString("true"));
        }
    }
    if (bif_options->aHwrot != false)
    {
        sub_string += "a_hwrot,";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(boot_config, "a_hwrot", cJSON_CreateString("true"));
        }
    }
    if (bif_options->sHwrot != false)
    {
        sub_string += "s_hwrot,";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(boot_config, "s_hwrot", cJSON_CreateString("true"));
        }
    }
    if (bif_options->GetPufRingOscilltorSwapConfigValue() != 0)
    {
        input_str = ConvertToHexString(bif_options->GetPufRingOscilltorSwapConfigValue()).c_str();
        sub_string += "puf_ro_swap = " + input_str + ",";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(boot_config, "puf_ro_swap", cJSON_CreateString(ConvertToHexString(bif_options->GetPufRingOscilltorSwapConfigValue()).c_str()));
        }
    }
    if (bif_options->GetDice() != DICE::DiceDisable)
    {
        sub_string += "dice_enable,";
        if (!(env != NULL)) {
            cJSON_AddItemToObject(boot_config, "dice_enable", cJSON_CreateString("true"));
        }
    }

    if (sub_string != "")
    {
        sub_string.pop_back();
        bif_string = "boot_config { " + sub_string + " }";
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "boot_config", boot_config); 
        }
        sub_string.clear();
    }

    /* Bif level Inputs*/
    if (includeBifFile != "")
    {
        input_str = includeBifFile.c_str();
        bif_string = "include = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "include", cJSON_CreateString(includeBifFile.c_str()));
        }
    }
    if (bif_options->GetPufHelperFile() != "")
    {
        input_str = bif_options->GetPufHelperFile().c_str();
        bif_string = "puf_file = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "puf_file", cJSON_CreateString(bif_options->GetPufHelperFile().c_str()));
        }
    }
    if (bif_options->GetBHKekIVFile() != "")
    {
        input_str = bif_options->GetBHKekIVFile().c_str();
        bif_string = "bh_kek_iv = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
            cJSON_AddItemToObject(bif_block, "bh_kek_iv", cJSON_CreateString(bif_options->GetBHKekIVFile().c_str())); 
        }
    }
    if (bif_options->GetBbramKekIVFile() != "")
    {
        input_str = bif_options->GetBbramKekIVFile().c_str();
        bif_string = "bbram_kek_iv = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "bbram_kek_iv", cJSON_CreateString(bif_options->GetBbramKekIVFile().c_str()));
        }   
    }
    if (bif_options->GetBhKeyFile() != "")
    {
        input_str = bif_options->GetBhKeyFile().c_str();
        bif_string = "bh_keyfile = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "bh_keyfile", cJSON_CreateString(bif_options->GetBhKeyFile().c_str())); 
        }
    }
    if (bif_options->GetEfuseKekIVFile() != "")
    {
        input_str = bif_options->GetEfuseKekIVFile().c_str();
        bif_string = "efuse_kek_iv = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "efuse_kek_iv", cJSON_CreateString(bif_options->GetEfuseKekIVFile().c_str())); 
        }
    }
    if (bif_options->GetEfuseUserKek0IVFile() != "")
    {
        input_str = bif_options->GetEfuseUserKek0IVFile().c_str();
        bif_string = "efuse_user_kek0_iv = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "efuse_user_kek0_iv", cJSON_CreateString(bif_options->GetEfuseUserKek0IVFile().c_str())); 
        }
    }
    if (bif_options->GetEfuseUserKek1IVFile() != "")
    {
        input_str = bif_options->GetEfuseUserKek1IVFile().c_str();
        bif_string = "efuse_user_kek1_iv = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "efuse_user_kek1_iv", cJSON_CreateString(bif_options->GetEfuseUserKek1IVFile().c_str())); 
        }
    }
    if (bif_options->GetSPKSignFileName() != "")
    {
        input_str = bif_options->GetSPKSignFileName().c_str();
        bif_string = "spksignature = " + input_str;
        if (env != NULL) {
            cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "spksignature", cJSON_CreateString(bif_options->GetSPKSignFileName().c_str())); 
        }
    }
    if (bif_options->GetUserKeysFileName() != "")
    {
        input_str = bif_options->GetUserKeysFileName().c_str();
        bif_string = "userkeys = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "userkeys", cJSON_CreateString(bif_options->GetUserKeysFileName().c_str())); 
        }
    }
    if (bif_options->GetRegInitFileName() != "")
    {
        input_str = bif_options->GetRegInitFileName();
        bif_string = "init = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "init", cJSON_CreateString(bif_options->GetRegInitFileName().c_str())); 
        }
    }
    if (bif_options->GetSplitMode() != SplitMode::None)
    {
        input_str = bif_options->GetSplitMode();
        bif_string = "split = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "split", cJSON_CreateString(GetSplitModeString(bif_options->GetSplitMode()).c_str())); 
        }
    }
    if (env != NULL) {
       if (bif_string != "")
       {
          cJSON_AddItemToObject(bif_block, "extra_bif_args", extra_bif_args);
          bif_string.clear();
       }
    }

    if (bif_options->GetBHSignFileName() != "")
    {
        cJSON_AddItemToObject(bif_block, "bhsignature", cJSON_CreateString(bif_options->GetBHSignFileName().c_str()));
    }
    if (bif_options->GetHeaderSignatureFile() != "")
    {
        cJSON_AddItemToObject(bif_block, "headersignature", cJSON_CreateString(bif_options->GetHeaderSignatureFile().c_str()));
    }
    if (bif_options->GetPPKFileName() != "")
    {
        input_str = bif_options->GetPPKFileName().c_str();
        bif_string = "ppkfile = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "ppkfile", cJSON_CreateString(bif_options->GetPPKFileName().c_str()));
        }
    }
    if (bif_options->GetPSKFileName() != "")
    {
        input_str = bif_options->GetPSKFileName().c_str();
        bif_string = "pskfile = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "pskfile", cJSON_CreateString(bif_options->GetPSKFileName().c_str()));
        }
    }
    if (bif_options->GetSPKFileName() != "")
    {
        input_str = bif_options->GetSPKFileName().c_str();
        bif_string = "spkfile = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "spkfile", cJSON_CreateString(bif_options->GetSPKFileName().c_str()));
        }
    }
    if (bif_options->GetSSKFileName() != "")
    {
        input_str = bif_options->GetSSKFileName().c_str();
        bif_string = "sskfile = " + input_str;
        if (env != NULL) {
           cJSON_AddItemToArray(extra_bif_args, cJSON_CreateString(bif_string.c_str()));
        }
        else
        {
           cJSON_AddItemToObject(bif_block, "sskfile", cJSON_CreateString(bif_options->GetSSKFileName().c_str()));
        }
    }

    /* Meta Header */
    if ((bif_options->metaHdrAttributes.authenticate != Authentication::None) || (bif_options->metaHdrAttributes.encrypt != Encryption::None))
    {
        metahdr = cJSON_CreateObject();
        if (bif_options->metaHdrAttributes.authenticate != Authentication::None)
        {
            cJSON_AddItemToObject(metahdr, "authentication", cJSON_CreateString(GetAuthTypeString(bif_options->metaHdrAttributes.authenticate).c_str()));
        }
        if (bif_options->metaHdrAttributes.ppk != "")
        {
            cJSON_AddItemToObject(metahdr, "ppkfile", cJSON_CreateString(bif_options->metaHdrAttributes.ppk.c_str()));
        }
        else if (bif_options->GetPPKFileName() != "")
        {
            cJSON_AddItemToObject(metahdr, "ppkfile", cJSON_CreateString(bif_options->GetPPKFileName().c_str()));
        }

        if (bif_options->metaHdrAttributes.psk != "")
        {
            cJSON_AddItemToObject(metahdr, "pskfile", cJSON_CreateString(bif_options->metaHdrAttributes.psk.c_str()));
        }
        else if (bif_options->GetPSKFileName() != "")
        {
            cJSON_AddItemToObject(metahdr, "pskfile", cJSON_CreateString(bif_options->GetPSKFileName().c_str()));
        }

        if (bif_options->metaHdrAttributes.spk != "")
        {
            cJSON_AddItemToObject(metahdr, "spkfile", cJSON_CreateString(bif_options->metaHdrAttributes.spk.c_str()));
        }
        else if (bif_options->GetSPKFileName() != "")
        {
            cJSON_AddItemToObject(metahdr, "spkfile", cJSON_CreateString(bif_options->GetSPKFileName().c_str()));
        }

        if (bif_options->metaHdrAttributes.ssk != "")
        {
            cJSON_AddItemToObject(metahdr, "sskfile", cJSON_CreateString(bif_options->metaHdrAttributes.ssk.c_str()));
        }
        else if (bif_options->GetSSKFileName() != "")
        {
            cJSON_AddItemToObject(metahdr, "sskfile", cJSON_CreateString(bif_options->GetSSKFileName().c_str()));
        }

        if (bif_options->metaHdrAttributes.spkSignature != "")
        {
            cJSON_AddItemToObject(metahdr, "spksignature", cJSON_CreateString(bif_options->metaHdrAttributes.spkSignature.c_str()));
        }    
        if (bif_options->metaHdrAttributes.presign != "")
        {
            cJSON_AddItemToObject(metahdr, "presign", cJSON_CreateString(bif_options->metaHdrAttributes.presign.c_str()));
        }

        if (bif_options->metaHdrAttributes.encrypt != Encryption::None)
        {
            cJSON_AddItemToObject(metahdr, "encryption", cJSON_CreateString("aes"));
        }
        if (bif_options->metaHdrAttributes.encrKeySource != KeySource::None)
        {
            cJSON_AddItemToObject(metahdr, "keysrc", cJSON_CreateString(GetKeySourceString(bif_options->metaHdrAttributes.encrKeySource).c_str()));
        }
        if (bif_options->metaHdrAttributes.encrKeyFile != "")
        {
            cJSON_AddItemToObject(metahdr, "aeskeyfile", cJSON_CreateString(bif_options->metaHdrAttributes.encrKeyFile.c_str()));
        }
        if (bif_options->metaHdrAttributes.dpaCM != DpaCM::DpaCMDisable)
        {
            cJSON_AddItemToObject(metahdr, "dpacm_enable", cJSON_CreateString("true"));
        }

        if (bif_options->metaHdrAttributes.spkRevokeId != 0)
        {
            input_str = ConvertToHexString(bif_options->metaHdrAttributes.spkRevokeId).c_str();
            bif_string = "spk_revoke_id = " + input_str;
            if (env != NULL) {
               cJSON_AddItemToArray(extra_meta_args, cJSON_CreateString(bif_string.c_str()));
            }
            else
            {
               cJSON_AddItemToObject(metahdr, "spk_revoke_id", cJSON_CreateString(ConvertToHexString(bif_options->metaHdrAttributes.spkRevokeId).c_str()));
            }
        }
        if (bif_options->metaHdrAttributes.pufHdLoc != PufHdLoc::PUFinEFuse)
        {
            bif_string = "pufhd_bh";
            if (env != NULL) {
                cJSON_AddItemToArray(extra_meta_args, cJSON_CreateString(bif_string.c_str()));
            }
            else
            {
                cJSON_AddItemToObject(metahdr, "pufhd_bh", cJSON_CreateString("true")); 
            }
        }
        if (bif_options->metaHdrAttributes.checksum != Checksum::None)
        {
            input_str = GetChecksumTypeString(bif_options->metaHdrAttributes.checksum).c_str();
            bif_string = "checksum = " + input_str;
            if (env != NULL) {
               cJSON_AddItemToArray(extra_meta_args, cJSON_CreateString(bif_string.c_str()));
            }
            else
            {
               cJSON_AddItemToObject(metahdr, "checksum", cJSON_CreateString(GetChecksumTypeString(bif_options->metaHdrAttributes.checksum).c_str()));
            }
        }
        if ((bif_options->metaHdrAttributes.encrBlocks.size() != 0) || (bif_options->metaHdrAttributes.defEncrBlockSize != 0))
        {
            input_str = GetAesBlocksString(bif_options->metaHdrAttributes.encrBlocks, bif_options->metaHdrAttributes.defEncrBlockSize).c_str();
            bif_string = "blocks = " + input_str;
            if (env != NULL) {
               cJSON_AddItemToArray(extra_meta_args, cJSON_CreateString(bif_string.c_str()));
            }
            else
            {
               cJSON_AddItemToObject(metahdr, "blocks", cJSON_CreateString(GetAesBlocksString(bif_options->metaHdrAttributes.encrBlocks, bif_options->metaHdrAttributes.defEncrBlockSize).c_str()));
            }
        }
        if (env != NULL) {
           if (bif_string != "")
           {
              cJSON_AddItemToObject(metahdr, "extra_meta_args", extra_meta_args);
              bif_string.clear();
           }
        }
        cJSON_AddItemToObject(bif_block, "metaheader", metahdr);
    }

    /* Images */
    images = cJSON_CreateArray();
    for (std::list<ImageBifOptions*>::iterator imgitr = bif_options->imageBifOptionList.begin(); imgitr != bif_options->imageBifOptionList.end(); imgitr++)
    {
        cJSON* image = CreateImageBlock(*imgitr, bif_options);
        cJSON_AddItemToArray(images, image);
    }
    cJSON_AddItemToObject(bif_block, "images", images);
    return bif_block;
}

/******************************************************************************/
void CreateVersalBifJson(Options& options)
{
    std::string jsonfile = StringUtils::ChangeExtension(options.bifFileName, ".json");
    FILE *f = fopen(jsonfile.c_str(), "w");
    if(options.includeBifOptionsList.size() != 0)
        includeBifFile = options.includeBifOptionsList.front();

    cJSON *bif = cJSON_CreateObject();
    if (bif == NULL)
    {
        cJSON_Delete(bif);
        LOG_ERROR("Error generating JSON BIF");
    }

    cJSON * bif_blocks = cJSON_CreateArray();
    for (std::vector<BifOptions*>::iterator bifoptions = options.bifOptionsList.begin(); bifoptions != options.bifOptionsList.end(); bifoptions++)
    {
        cJSON* bif_block = CreateBifBlock((*bifoptions));
        cJSON_AddItemToArray(bif_blocks, bif_block);

    }
    cJSON_AddItemToObject(bif, "bif_blocks", bif_blocks);

    char* out = cJSON_Print(bif);
    if (out == NULL)
    {
        LOG_ERROR("Error writing JSON BIF");
    }
    fprintf(f, "%s\n", out);
    cJSON_Delete(bif);
    fclose(f);
    LOG_INFO("%s generated successfully", jsonfile.c_str());
}