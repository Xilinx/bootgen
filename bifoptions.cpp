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
************************************************************ HEADER FILES *****
-------------------------------------------------------------------------------
*/
#include <fstream>
#include <algorithm>
#include <string>
#include "bifoptions.h"
#include "bifscanner.h"

/* Forward Class Declaration */
class BootImage;

/*
-------------------------------------------------------------------------------
*************************************************************** FUNCTIONS *****
-------------------------------------------------------------------------------
*/
/******************************************************************************/
BifOptions::BifOptions(Arch::Type architecture, const char* name)
    : groupname("")
    , regInitFile("")
    , udfBhFile("")
    , ppkFile("")
    , pskFile("")
    , headerSignatureFile("")
    , bhKeyFile("")
    , pufHelperFile("")
    , bhKeyIVFile("")
    , familyKeyFile("")
    , keySourceEncryption(KeySource::None)
    , bootDevice(BootDevice::DEFAULT)
    , bhAuthEnable(BhRsa::BhRsaDisable)
    , pufHdLoc(PufHdLoc::PUFinEFuse)
    , authOnly(AuthOnly::None)
    , pufMode(PufMode::PUF12K)
    , optKey(OptKey::None)
    , shutterVal(SHUTTER_VAL)
    , ppkSelect(0)
    , spkSelect(1)
    , spkId(0x00000000)
    , headerAuthParam(false)
    , createHeaderAC(false)
    , splitMode(SplitMode::None)
    , splitFmt(File::Unknown)
    , xipMode(false)
    , core(Core::A53Singlex64)
    , isPPKSelectGlobal(false)
    , isSPKSelectGlobal(false)
    , isSpkIdGlobal(false)
{
    arch = architecture;
    SetGroupName(name);
}

/******************************************************************************
 PartitionBifOptions
******************************************************************************/
PartitionBifOptions::PartitionBifOptions()
    : aesKeyFile("")
    , spkFile("")
    , sskFile("")
    , spkSignatureFile("")
    , bootImage(false)
    , pmuFwImage(false)
    , presignFile("")
    , udfDataFile("")
    , encryptType(Encryption::None)
    , authType(Authentication::None)
    , checksumType(Checksum::None)
    , ownerType(PartitionOwner::FSBL)
    , destCPUType(DestinationCPU::NONE)
    , destDeviceType(DestinationDevice::DEST_DEV_PS)
    , exceptionLevel(ExceptionLevel::EL3)
    , trustzone(TrustZone::NonSecure)
    , bootloader(false)
    , early_handoff(false)
    , hivec(false)
    , authblockattr(0)
    , pid(0)
    , blocks(0)
    , defBlockSize(0)
    , spkSelect(1)
    , spkSelLocal(false)
    , spkId(0x00000000)
    , spkIdLocal(false)
    , fileType(0)
    , filename("")
    , arch (Arch::ZYNQ)
    , partitionId(0)
    , boot(false)
    , user(false)
    , Static(false)
    , Protected(false)
    , multiboot(false)
    , noautostart(false)
    , blockSize(0)
{ }

/******************************************************************************/
void BifOptions::SetHeaderAC(bool flag)
{
    createHeaderAC = flag;
}

/******************************************************************************/
void BifOptions::Add(PartitionBifOptions* currentPartitionBifOptions)
{
    //filespec->Dump();
    switch (currentPartitionBifOptions->fileType)
    {
    case BIF::BisonParser::token::INIT:
        SetRegInitFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::UDF_BH:
        SetUdfBHFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::AES_KEY_FILE:
        SetAESKeyFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::FAMILY_KEY:
        SetFamilyKeyFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::PPK_FILE:
        SetPPKFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::PSK_FILE:
        SetPSKFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::SPK_FILE:
        SetSPKFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::SSK_FILE:
        SetSSKFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::SPK_SIGNATURE_FILE:
        SetSPKSignFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::BH_SIGNATURE_FILE:
        SetBHSignFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::HEADER_SIGNATURE_FILE:
        SetHeaderSignFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::BH_KEY_FILE:
        SetBHKeyFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::PUF_HELPER_FILE:
        SetPUFHelperFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::BH_KEY_IV:
        SetBHKeyIVFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::PMUFW_IMAGE:
        SetPmufwImageFileName(currentPartitionBifOptions);
        break;

    case 0:
        partitionBifOptionList.push_back(currentPartitionBifOptions);
    }
}


/******************************************************************************/
void BifOptions::SetRegInitFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f) 
    {
        LOG_ERROR("Can't read file - %s", regInitFile.c_str());
    }
    regInitFile = filename;
    LOG_TRACE("Setting Register initialization file as %s", regInitFile.c_str());
}

/******************************************************************************/
std::string BifOptions::GetRegInitFileName()
{
    return regInitFile;
}

/******************************************************************************/
std::string BifOptions::GetGroupName()
{
    return groupname;
}

/******************************************************************************/
std::string BifOptions::GetAESKeyFileName(void)
{
    return aesKeyFile;
}

/******************************************************************************/
void BifOptions::SetUdfBHFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Can't read file - %s", regInitFile.c_str());
    }
    udfBhFile = filename;
    LOG_TRACE("Setting UDF of BH as %s", udfBhFile.c_str());
}

/******************************************************************************/
void BifOptions::SetPmufwImageFileName(PartitionBifOptions* currentPartitionBifOptions)
{
    if (arch != Arch::ZYNQMP)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'[pmufw_image]' is supported only in ZYNQMP architecture");
    }
    std::ifstream f(currentPartitionBifOptions->filename.c_str());
    if (!f)
    {
        LOG_ERROR("Can't read file - %s", regInitFile.c_str());
    }

    if (currentPartitionBifOptions->authType != Authentication::None)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\tBif option 'authentication' is not supported with [pmufw_image].\n\t\tpmufw will be signed along with bootloader, if authentication is enabled for bootloader.");
    }
    if (currentPartitionBifOptions->encryptType != Encryption::None)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\tBif option 'encryption' is not supported with [pmufw_image].\n\t\tpmufw will be encrypted if encryption is enabled for bootloader.");
    }
    if (currentPartitionBifOptions->checksumType != Checksum::None)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\tBif option 'checksum' is not supported with [pmufw_image].\n\t\tpmufw will be checksummed if checksum is enabled for bootloader.");
    }
    if (currentPartitionBifOptions->destCPUType != DestinationCPU::NONE)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\tBif option 'destination_cpu' is not supported with [pmufw_image].");
    }

    pmuFwImageFile = currentPartitionBifOptions->filename;
    LOG_TRACE("Setting PMU FW Image file as %s", pmuFwImageFile.c_str());
}

/******************************************************************************/
void BifOptions::SetGroupName(std::string name)
{
   groupname = name;
}

/******************************************************************************/
void BifOptions::SetPPKFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    ppkFile = filename;
}

/******************************************************************************/
void BifOptions::SetPSKFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    pskFile = filename;
}

/******************************************************************************/
void BifOptions::SetSPKFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    spkFile = filename;
}

/******************************************************************************/
void BifOptions::SetSSKFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    sskFile = filename;
}

/******************************************************************************/
void BifOptions::SetSPKSignFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Can't read file - %s", filename.c_str());
    }
    spkSignatureFile = filename;
}

/******************************************************************************/
void BifOptions::SetBHSignFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Can't read file - %s", filename.c_str());
    }
    bhSignatureFile = filename;
}

/******************************************************************************/
void BifOptions::SetHeaderSignFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Can't read file - %s", filename.c_str());
    }
    headerSignatureFile = filename;
}

/******************************************************************************/
void BifOptions::SetBHKeyFileName(std::string filename)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'[bh_keyfile]' not supported in ZYNQ architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Can't read file - %s", filename.c_str());
    }

    bhKeyFile = filename;
    LOG_TRACE("Setting BH Key file as %s", bhKeyFile.c_str());
}

/******************************************************************************/
void BifOptions::SetOptKey(OptKey::Type type)
{
    optKey = type;
}

/******************************************************************************/
void BifOptions::SetPufMode(PufMode::Type type)
{
    pufMode = type;
}

/******************************************************************************/
void BifOptions::SetShutterValue(uint32_t value)
{
    shutterVal = value;
}

/******************************************************************************/
void BifOptions::SetPUFHelperFileName(std::string filename)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'[puf_file]' not supported in ZYNQ architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Can't read file - %s", filename.c_str());
    }

    pufHelperFile = filename;
    LOG_TRACE("Setting PUF Helper Data file as %s", pufHelperFile.c_str());
}

/******************************************************************************/
void BifOptions::SetBHKeyIVFileName(std::string filename)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'[bh_key_iv]' not supported in ZYNQ architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Can't read file - %s", filename.c_str());
    }

    bhKeyIVFile = filename;
    LOG_TRACE("Setting BH IV file as %s", bhKeyIVFile.c_str());
}

/******************************************************************************/
void BifOptions::SetFamilyKeyFileName(std::string filename)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'[familykey]' not supported in ZYNQ architecture");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Can't read file - %s", filename.c_str());
    }

    familyKeyFile = filename;
    LOG_TRACE("Setting Family Key file as %s", familyKeyFile.c_str());
}

/******************************************************************************/
void BifOptions::SetAESKeyFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    aesKeyFile = filename;
}

/******************************************************************************/
void BifOptions::SetBootVectorArray(uint32_t vector)
{
    static bool warningGiven = false;
    if (!warningGiven)
    {
        LOG_TRACE("Setting bootvector array");
    }
    warningGiven = true;

    bootvectors.push_back(vector);
    if (bootvectors.size() > 8)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] cannot take more than 8 vector addresses");
    }
}

/******************************************************************************/
void BifOptions::SetXipMode()
{
    xipMode = true;
}

/******************************************************************************/
bool BifOptions::GetXipMode(void)
{
    return xipMode;
}

/******************************************************************************/
void BifOptions::SetSplitMode(SplitMode::Type type)
{
    LOG_TRACE("Setting Split Mode as %d", type );
    splitMode = type;
}

/******************************************************************************/
void BifOptions::SetSplitFmt(File::Type type)
{
    LOG_TRACE("Setting Split Format as %d", type);
    splitFmt = type;
}

/******************************************************************************/
void BifOptions::SetEncryptionKeySource(KeySource::Type type)
{
    if (arch == Arch::ZYNQ)
    {
        if ((type != KeySource::BbramRedKey) && (type != KeySource::EfuseRedKey) && (type != KeySource::None))
        {
            LOG_DEBUG(DEBUG_STAMP, "Key Source = %d", type);
            LOG_ERROR("BIF attribute error !!!\n\t\tUnknown option for 'keysrc_encryption' in BIF file\n\t\tZynq Architecture supports 'efuse' & 'bbram' key sources");
        }
    }
    keySourceEncryption = type;
}

/******************************************************************************/
void BifOptions::SetBootDevice(BootDevice::Type type)
{
    LOG_TRACE("Setting Boot Device as %d", type);
    bootDevice = type;
}

/******************************************************************************/
OptKey::Type BifOptions::GetAesOptKeyFlag(void)
{
    return optKey;
}

/******************************************************************************/
void PartitionBifOptions::SetArchType(Arch::Type type)
{
    arch = type;
}

/******************************************************************************/
void PartitionBifOptions::SetEncryptionBlocks(uint32_t size, uint32_t num)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'blocks' not supported in ZYNQ architecture");
    }
    if (num == 0)
    {
        SetDefaultEncryptionBlockSize(size);
    }
    else if (defBlockSize == 0)
    {
        for (uint32_t i = 0; i < num; i++)
        {
            blocks.push_back(size);
        }
    }
    else
    {
        LOG_ERROR("cannot choose block size after choosing def block size");
    }
}

/******************************************************************************/
void PartitionBifOptions::SetDefaultEncryptionBlockSize(uint32_t blk)
{
    defBlockSize = blk;
}

/******************************************************************************/
void BifOptions::InsertEncryptionBlock(uint32_t blk)
{
    blocks.push_back(blk);
}

/******************************************************************************/
void PartitionBifOptions::SetDestCpu(DestinationCPU::Type type)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'destination_cpu' not supported in ZYNQ architecture");
    }
    destCPUType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetDestDevice(DestinationDevice::Type type)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'destination_device' not supported in ZYNQ architecture");
    }

    if (type == DestinationDevice::DEST_DEV_PMU)
    {
        LOG_ERROR("BIF attribute 'destination_device=pmufw' is not supported.\
                    Please use 'destination_cpu=pmu'");
    }

    if (type == DestinationDevice::DEST_DEV_XIP)
    {
        LOG_ERROR("BIF attribute 'destination_device=xip' is not supported.\
                     Please use the attribute 'xip_mode'. \
                     For more info, see 'bootgen -bif_help xip_mode'");
    }

    destDeviceType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetAuthType(Authentication::Type type)
{
    authType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetEncryptType(Encryption::Type type)
{
    encryptType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetChecksumType(Checksum::Type type)
{
    if (type == Checksum::SHA2)
    {
        LOG_ERROR("BIF cksum BIF attribute error !!!\n\t\t'checksum=sha2' is not supported.");
    }
    if ((arch == Arch::ZYNQ) && (type == Checksum::SHA3))
    {
        
        LOG_ERROR("BIF attribute error !!!\n\t\t'checksum=sha3' not supported in ZYNQ architecture");
    }
    if ((arch == Arch::ZYNQMP) && (type == Checksum::MD5))
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'checksum=md5' not supported in ZYNQMP architecture");
    }
    checksumType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetOwnerType(PartitionOwner::Type type)
{
    ownerType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetExceptionLevel(ExceptionLevel::Type type)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF el BIF attribute error !!!\n\t\t'exception_level' not supported in ZYNQ architecture");
    }
    exceptionLevel = type;
}

/******************************************************************************/
void BifOptions::SetFsblFilename(std::string filename)
{
    fsblFilename = filename;
}

/******************************************************************************/
void PartitionBifOptions::SetTrustZone(TrustZone::Type type)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF trust BIF attribute error !!!\n\t\t'trustzone' not supported in ZYNQ architecture");
    }
    trustzone = type;
}

/******************************************************************************/
void PartitionBifOptions::SetEarlyHandoff(bool flag)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF eh BIF attribute error !!!\n\t\t'early_handoff' not supported in ZYNQ architecture");
    }
    if (bootloader == true)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'early_handoff' not supported for bootloader partitions");
    }
    early_handoff = flag;
}

/******************************************************************************/
void PartitionBifOptions::SetHivec(bool flag)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'hivec' not supported in ZYNQ architecture");
    }
    hivec = flag;
}

/******************************************************************************/
std::vector<uint32_t>& PartitionBifOptions::GetEncryptionBlocks(void)
{
    return blocks;
}

/******************************************************************************/
std::vector<uint32_t>& BifOptions::GetEncryptionBlocksList(void)
{
    return blocks;
}

/******************************************************************************/
void PartitionBifOptions::SetAuthBlockAttr(size_t authBlockAttr)
{
    if (authBlockAttr != 4 && authBlockAttr != 8 && authBlockAttr != 16 && authBlockAttr != 32 && authBlockAttr != 64)
    {
        LOG_ERROR("'-authblock' option supports only 4,8,16,32,64 sizes (taken in MB)");
    }
    if (arch != Arch::ZYNQMP)
    {
        LOG_ERROR("'-authblock' option supported only for ZYNQMP architecture '-arch zynqmp'");
    }
    authblockattr = authBlockAttr;
}

/******************************************************************************/
uint32_t PartitionBifOptions::GetDefaultEncryptionBlockSize(void)
{
    return defBlockSize;
}

/******************************************************************************/
void BifOptions::SetPPKSelection(uint32_t ppkSelection)
{
    if ((ppkSelection == 0) | (ppkSelection == 1))
    {
        ppkSelect = ppkSelection;
        isPPKSelectGlobal = true;
        LOG_TRACE("Setting PPK Selection in Auth Certificate as %d", ppkSelect);
    }
    else
    {
        LOG_DEBUG(DEBUG_STAMP, "PPK Select - %d", ppkSelect);
        LOG_ERROR("BIF attribute error !!!\n\t\t'ppk_select' can either be 0 or 1");
    }
}

/******************************************************************************/
void BifOptions::SetSPKSelection(uint32_t spkSelection)
{
    spkSelect = spkSelection;
    isSPKSelectGlobal = true;
    LOG_TRACE("Setting SPK Selection in Auth Certificate as %d", spkSelect);
}

/******************************************************************************/
void BifOptions::SetSpkId(uint32_t id)
{
    spkId = id;
    isSpkIdGlobal = true;
    LOG_TRACE("Setting SPK ID in Auth Certificate as 0x%x", id);
}

/******************************************************************************/
void BifOptions::SetHeaderAuthentication()
{
    headerAuthParam = true;
    SetHeaderAC(true);
    LOG_TRACE("Setting Header Authentication");
}

/******************************************************************************/
std::string BifOptions::GetFamilyKeyFileName()
{
    return familyKeyFile;
}

/******************************************************************************/
BhRsa::Type BifOptions::GetBhRsa(void)
{
    return bhAuthEnable;
}

/******************************************************************************/
BootDevice::Type BifOptions::GetBootDevice(void)
{
    return bootDevice;
}

/******************************************************************************/
PufMode::Type BifOptions::GetPufMode(void)
{
    return pufMode;
}

/******************************************************************************/
PufHdLoc::Type BifOptions::GetPufHdLoc(void)
{
    return pufHdLoc;
}

/******************************************************************************/
OptKey::Type BifOptions::GetOptKey(void)
{
    return optKey;
}

/******************************************************************************/
AuthOnly::Type BifOptions::GetAuthOnly(void)
{
    return authOnly;
}

/******************************************************************************/
uint32_t BifOptions::GetPpkSelection(void)
{
    return ppkSelect;
}

/******************************************************************************/
bool BifOptions::GetPpkSelectionGlobal()
{
    return isPPKSelectGlobal;
}

/******************************************************************************/
std::string BifOptions::GetPPKFileName(void)
{
    return ppkFile;
}
/******************************************************************************/
std::string BifOptions::GetPSKFileName(void)
{
    return pskFile;
}

/******************************************************************************/
uint32_t BifOptions::GetSpkSelection(void)
{
    return spkSelect;
}

/******************************************************************************/
bool BifOptions::GetSpkSelectionGlobal()
{
    return isSPKSelectGlobal;
}

/******************************************************************************/
std::string BifOptions::GetSPKFileName(void)
{
    return spkFile;
}

/******************************************************************************/
std::string BifOptions::GetSSKFileName(void)
{
    return sskFile;
}

/******************************************************************************/
uint32_t BifOptions::GetSpkId(void)
{
    return spkId;
}

/******************************************************************************/
bool BifOptions::GetSpkIdGlobal()
{
    return isSpkIdGlobal;
}

/******************************************************************************/
bool BifOptions::GetHeaderAC()
{
    return createHeaderAC;
}

/******************************************************************************/
std::string BifOptions::GetBhKeyFile(void)
{
    return bhKeyFile;
}

/******************************************************************************/
uint32_t BifOptions::GetShutterValue(void)
{
    return shutterVal;
}

/******************************************************************************/
std::string BifOptions::GetHeaderSignatureFile(void)
{
    return headerSignatureFile;
}

/******************************************************************************/
std::string BifOptions::GetPufHelperFile(void)
{
    return pufHelperFile;
}

/******************************************************************************/
std::string BifOptions::GetPmuFwImageFile(void)
{
    return pmuFwImageFile;
}

/******************************************************************************/
std::string BifOptions::GetBHKeyIVFile(void)
{
    return bhKeyIVFile;
}

/******************************************************************************/
std::string BifOptions::GetUdfBhFile(void)
{
    return udfBhFile;
}

/******************************************************************************/
SplitMode::Type BifOptions::GetSplitMode(void)
{
    return splitMode;
}

/******************************************************************************/
File::Type BifOptions::GetSplitFormat(void)
{
    return splitFmt;
}

/******************************************************************************/
std::vector<uint32_t>& BifOptions::GetBootVectorArray(void)
{
    return bootvectors;
}

/******************************************************************************/
std::string BifOptions::GetSPKSignFileName(void)
{
    return spkSignatureFile;
}

/******************************************************************************/
std::string BifOptions::GetBHSignFileName(void)
{
    return bhSignatureFile;
}

/******************************************************************************/
void BifOptions::SetAuthOnly(AuthOnly::Type type)
{
    authOnly = type;
}

/******************************************************************************/
void BifOptions::SetBhRsa(BhRsa::Type value)
{
    bhAuthEnable = value;
}

/******************************************************************************/
void BifOptions::SetPufHdLoc(PufHdLoc::Type value)
{
    pufHdLoc= value;
}

/******************************************************************************/
void BifOptions::SetCore(Core::Type type)
{
    core = type;
}

/******************************************************************************/
Core::Type BifOptions::GetCore(void)
{
    return core;
}
