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
************************************************************ HEADER FILES *****
-------------------------------------------------------------------------------
*/
#include <fstream>
#include <algorithm>
#include <string>
#include <sys/stat.h>
#include "bifoptions.h"
#include "authkeys.h"
#include "bifscanner.h"
#include "encryptutils.h"
#include "cdo-command.h"
#include "fileutils.h"
#include <map>

/*
-------------------------------------------------------------------------------
************************************************ DEVICE-SPECIFIC FUNCTION POINTERS
-------------------------------------------------------------------------------
Device-specific functions are implemented in device-specific directories
(e.g., versal_2vp/src/bifoptions-versal_2vp.cpp) to avoid polluting common
code with device-specific headers and constants.

These function pointers are set at runtime based on the architecture type.
This ensures:
  1. Common code has NO device-specific header dependencies
  2. Future devices can add support by creating their own bifoptions-<device>.cpp
  3. Each device's authentication constants stay isolated in device directories
*/
namespace DeviceSpecificKmd
{
    typedef uint32_t (*ConvertAlgorithmStringToHeaderValueFunc)(const std::string&);
    typedef uint32_t (*ConvertHybridStringToBitmaskFunc)(const std::string&);
    typedef bool (*IsClassicalAlgorithmFunc)(uint32_t);
    typedef bool (*IsPQCAlgorithmFunc)(uint32_t);
    typedef std::string (*GetAlgorithmNameFunc)(uint32_t);
    typedef bool (*IsAlgorithmInHybridMaskFunc)(uint32_t, uint32_t);
    typedef Authentication::Type (*ConvertKmdAlgorithmToAuthTypeFunc)(uint32_t);
    typedef bool (*HybridMaskContainsClassicalFunc)(uint32_t);
    typedef bool (*HybridMaskContainsPQCFunc)(uint32_t);
    
    // Global function pointers (set based on architecture)
    static ConvertAlgorithmStringToHeaderValueFunc ConvertAlgorithmStringToHeaderValue = NULL;
    static ConvertHybridStringToBitmaskFunc ConvertHybridStringToBitmask = NULL;
    static IsClassicalAlgorithmFunc IsClassicalAlgorithm = NULL;
    static IsPQCAlgorithmFunc IsPQCAlgorithm = NULL;
    static GetAlgorithmNameFunc GetAlgorithmName = NULL;
    static IsAlgorithmInHybridMaskFunc IsAlgorithmInHybridMask = NULL;
    static ConvertKmdAlgorithmToAuthTypeFunc ConvertKmdAlgorithmToAuthType = NULL;
    static HybridMaskContainsClassicalFunc HybridMaskContainsClassical = NULL;
    static HybridMaskContainsPQCFunc HybridMaskContainsPQC = NULL;
}

// Forward declarations
void InitializeDeviceSpecificKmdFunctions(Arch::Type arch);

#ifndef SKIP_VERSAL_2VP_NATIVE
// Device-specific namespace declarations (included only when needed)
namespace BifOptionsVersal2VP
{
    extern uint32_t ConvertAlgorithmStringToHeaderValue(const std::string& algStr);
    extern uint32_t ConvertHybridStringToBitmask(const std::string& hybridStr);
    extern bool IsClassicalAlgorithm(uint32_t algorithm);
    extern bool IsPQCAlgorithm(uint32_t algorithm);
    extern std::string GetAlgorithmName(uint32_t algorithm);
    extern bool IsAlgorithmInHybridMask(uint32_t algorithm, uint32_t hybridMask);
    extern Authentication::Type ConvertKmdAlgorithmToAuthType(uint32_t kmdAlgorithm);
    extern bool HybridMaskContainsClassical(uint32_t hybridMask);
    extern bool HybridMaskContainsPQC(uint32_t hybridMask);
}
#endif

/* Forward Class Declaration */
class BootImage;
static bool delay_load_warning_given;
static bool delay_handoff_warning_given;

/*
-------------------------------------------------------------------------------
*************************************************************** FUNCTIONS *****
-------------------------------------------------------------------------------
*/
/******************************************************************************/
BifOptions::BifOptions(Arch::Type architecture, bool versalNet, bool dl9, const char* name)
    : groupname("")
    , regInitFile("")
    , udfBhFile("")
    , ppkFile("")
    , pskFile("")
    , headerSignatureFile("")
    , bhKeyFile("")
    , pufHelperFile("")
    , bhKekIVFile("")
    , bbramKekIVFile("")
    , efuseKekIVFile("")
    , efuseUserKek0IVFile("")
    , efuseUserKek1IVFile("")
    , familyKeyFile("")
    , keySourceEncryption(KeySource::None)
    , bootDevice(BootDevice::DEFAULT)
    , bootDeviceAddress(0)
    , bhAuthEnable(BhRsa::BhRsaDisable)
    , pufHdLoc(PufHdLoc::PUFinEFuse)
    , pufHdinBHEnable(false)
    , authOnly(AuthOnly::None)
    , pufMode(PufMode::PUF12K)
    , optKey(OptKey::None)
    , shutterVal(SHUTTER_VAL)
    , pufRoSwapConfigVal(0)
    , dpaCM(DpaCM::DpaCMDisable)
    , ppkSelect(0)
    , spkSelect(1)
    , spkId(0x00000000)
    , headerAuthParam(false)
    , createHeaderAC(false)
    , doHeaderEncryption(false)
    , splitMode(SplitMode::None)
    , splitFmt(File::Unknown)
    , xipMode(false)
    , pmcCdoLoadAddress(DEFAULT_PMCDATA_LOADADDR)
    , pmcdataSize(0)
    , totalpmcdataSize(0)
    , pmcDataBuffer(NULL)
    , pdiId(0)
    , parentId(0)
    , idCode(0)
    , extendedIdCode(0)
    , bypassIdCode(false)
    , smapWidth(32)
    , core(Core::A53Singlex64)
    , isPPKSelectGlobal(false)
    , isSPKSelectGlobal(false)
    , isSpkIdGlobal(false)
    , slrBootCnt(0)
    , slrConfigCnt(0)
    , aHwrot(false)
    , sHwrot(false)
    , pmcdataBlocks(0)
    , dice(DICE::DiceDisable)
    , slrNum(0xFF)
    , pdiType(PartitionType::RESERVED)
    , spkRevokeId(0x00000000)
{
    arch = architecture;
    versalNetSeries = versalNet;
    dl9Series = dl9;
    SetGroupName(name);
    delay_handoff_warning_given = false;
    delay_load_warning_given = false;
    metaHdrAttributes.encrypt = Encryption::None;
    metaHdrAttributes.encrKeySource = KeySource::None;
    metaHdrAttributes.encrKeyFile = "";
    metaHdrAttributes.authenticate = Authentication::None;
    metaHdrAttributes.ppk = metaHdrAttributes.spk = metaHdrAttributes.psk = metaHdrAttributes.ssk = "";
    metaHdrAttributes.presign = metaHdrAttributes.spkSignature = "";
    metaHdrAttributes.spkRevokeId = 0;
    metaHdrAttributes.checksum = Checksum::None;
    metaHdrAttributes.dpaCM = DpaCM::DpaCMDisable;
    metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinEFuse;

    authJtagInfo.jtagTimeout = 0;
    authJtagInfo.userDeviceDNA = false;
    memset(authJtagInfo.deviceDNA, 0, sizeof(authJtagInfo.deviceDNA));
    authJtagInfo.spkRevokeId = 0;
    authJtagInfo.partitionRevokeId = 0;
    authJtagInfo.userRevokeId = false;
}

/******************************************************************************/
PartitionBifOptions::PartitionBifOptions()
    : aesKeyFile("")
    , ppkFile("")
    , pskFile("")
    , spkFile("")
    , sskFile("")
    , spkSignatureFile("")
    , bootImage(false)
    , pmuFwImage(false)
    , pmcData(false)
    , presignFile("")
    , acFile("")
    , udfDataFile("")
    , encryptType(Encryption::None)
    , authType(Authentication::None)
    , checksumType(Checksum::None)
    , ownerType(PartitionOwner::FSBL)
    , partitionType(PartitionType::RESERVED)
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
    , keySrc(KeySource::None)
    , boot(false)
    , user(false)
    , Static(false)
    , Protected(false)
    , multiboot(false)
    , noautostart(false)
    , blockSize(0)
    , bigEndian(false)
    , a32Mode(false)
    , dpaCM(DpaCM::DpaCMDisable)
    , spkRevokeId(0x00000000)
    , partitionRevokeId(0x00000000)
    , slrNum(0xFF)
    , pufHdLoc(PufHdLoc::PUFinEFuse)
    , versalNetSeries(false)
    , dl9Series(false)
    , clusterNum(0)
    , lockstep(false)
    , updateReserveInPh(false)
    , bifSection("")
    , delayAuth(false)
    , imageStoreId(0xFF)
    , erasePartition(false)
    , tcmBoot(false)
    , tcmARegionAddr(0)
    , tcmBRegionAddr(0)
    , tcmCRegionAddr(0)
    , lmsOnly(true)
    , hybridAuth(false)
{ }

/******************************************************************************/
void BifOptions::SetHeaderAC(bool flag)
{
    createHeaderAC = flag;
}

/******************************************************************************/
void BifOptions::SetHeaderEncryption(bool flag)
{
    doHeaderEncryption = flag;
}

/******************************************************************************/
void BifOptions::Add(PartitionBifOptions* currentPartitionBifOptions, ImageBifOptions* currentImageBifOptions)
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
        SetBHKekIVFileName(currentPartitionBifOptions->filename);
        break;

    case BIF::BisonParser::token::PMUFW_IMAGE:
        SetPmufwImageFileName(currentPartitionBifOptions);
        break;

    case BIF::BisonParser::token::PMCDATA:
        SetPmcdataFile(currentPartitionBifOptions->filename);
        SetPmcCdoFileList(currentPartitionBifOptions->filename);
        currentPartitionBifOptions->filename = currentPartitionBifOptions->filelist.at(0);
        if ((currentPartitionBifOptions)->load.Value() != 0)
        {
            pmcCdoLoadAddress = (currentPartitionBifOptions)->load.Value();
        }
        if ((currentPartitionBifOptions)->aesKeyFile != "")
        {
            SetPmcDataAesFile((currentPartitionBifOptions)->aesKeyFile);
        }
        //If no key file found in partition specific attributes - Generate aeskeyfile with partition_name.nky
        else
        {
            SetPmcDataAesFile(StringUtils::RemoveExtension(StringUtils::BaseName((currentPartitionBifOptions)->filename)) + ".nky");
        }

        if ((currentImageBifOptions != NULL) && (parentId != 0))
        {
            currentPartitionBifOptions->partitionType = PartitionType::CONFIG_DATA_OBJ;
            currentImageBifOptions->partitionBifOptionsList.push_back(currentPartitionBifOptions);
        }
        else if ((currentImageBifOptions != NULL) && (currentPartitionBifOptions->arch == Arch::SPARTANUP) && 
                 (currentImageBifOptions->partitionBifOptionsList.size() == 0))
        {
            currentPartitionBifOptions->partitionType = PartitionType::CONFIG_DATA_OBJ;
            currentPartitionBifOptions->bootloader = true;
            currentPartitionBifOptions->pmcData = true;
            currentImageBifOptions->partitionBifOptionsList.push_back(currentPartitionBifOptions);
        }
        else
        {
            if(!(currentPartitionBifOptions->pmcData))
                partitionBifOptionList.push_back(currentPartitionBifOptions);
            currentPartitionBifOptions->pmcData = true;
        }
        break;

    case 0:
        if (currentImageBifOptions != NULL)
        {
            if (currentImageBifOptions->GetImageType() == PartitionType::SLR_BOOT)
            {
                slrBootCnt++;
                currentPartitionBifOptions->partitionType = PartitionType::SLR_BOOT;
            }
            if (currentImageBifOptions->GetImageType() == PartitionType::SLR_CONFIG)
            {
                slrConfigCnt++;
                currentImageBifOptions->slrConfigCnt++;
                currentPartitionBifOptions->partitionType = PartitionType::SLR_CONFIG;
            }
            if (currentPartitionBifOptions->partitionType == PartitionType::RESERVED)
            {
                currentPartitionBifOptions->partitionType = currentImageBifOptions->GetImageType();
            }
            currentImageBifOptions->partitionBifOptionsList.push_back(currentPartitionBifOptions);
            if (lastPartitionBifOption == currentPartitionBifOptions)
            {
                currentImageBifOptions->partitionBifOptionsList.pop_back();
            }
        }
        if (lastPartitionBifOption == currentPartitionBifOptions)
        {
            partitionBifOptionList.pop_back();
            currentPartitionBifOptions->filename = currentPartitionBifOptions->filelist.front();
        }
        partitionBifOptionList.push_back(currentPartitionBifOptions);
        lastPartitionBifOption = currentPartitionBifOptions;
    }
}

/******************************************************************************/
void BifOptions::AddFiles(int type, std::string filename)
{
    //filespec->Dump();
    switch (type)
    {
        case BIF::BisonParser::token::INIT: 
            SetRegInitFileName(filename);
            break;

        case BIF::BisonParser::token::UDF_BH:
            SetUdfBHFileName(filename);
            break;

        case BIF::BisonParser::token::AES_KEY_FILE:
            SetAESKeyFileName(filename);
            break;

        case BIF::BisonParser::token::FAMILY_KEY:
            SetFamilyKeyFileName(filename);
            break;

        case BIF::BisonParser::token::PPK_FILE:
            SetPPKFileName(filename);
            break;

        case BIF::BisonParser::token::PSK_FILE:
            SetPSKFileName(filename);
            break;

        case BIF::BisonParser::token::SPK_FILE:
            SetSPKFileName(filename);
            break;

        case BIF::BisonParser::token::SSK_FILE:
            SetSSKFileName(filename);
            break;

        case BIF::BisonParser::token::PPK_FILE1:
            SetPPKFileName1(filename);
            break;

        case BIF::BisonParser::token::PSK_FILE1:
            SetPSKFileName1(filename);
            break;

        case BIF::BisonParser::token::SPK_FILE1:
            SetSPKFileName1(filename);
            break;

        case BIF::BisonParser::token::SSK_FILE1:
            SetSSKFileName1(filename);
            break;

        case BIF::BisonParser::token::SPK_SIGNATURE_FILE:
            SetSPKSignFileName(filename);
            break;

        case BIF::BisonParser::token::BH_SIGNATURE_FILE:
            SetBHSignFileName(filename);
            break;

        case BIF::BisonParser::token::HEADER_SIGNATURE_FILE:
            SetHeaderSignFileName(filename);
            break;

        case BIF::BisonParser::token::BH_KEY_FILE:
            SetBHKeyFileName(filename);
            break;

        case BIF::BisonParser::token::PUF_HELPER_FILE:
            SetPUFHelperFileName(filename);
            break;

        case BIF::BisonParser::token::BH_KEK_IV:
            SetBHKekIVFileName(filename);
            break;

        case BIF::BisonParser::token::BH_KEY_IV:
            SetBHKekIVFileName(filename);
            break;

        case BIF::BisonParser::token::BBRAM_KEK_IV:
            SetBbramKekIVFileName(filename);
            break;

        case BIF::BisonParser::token::EFUSE_KEK_IV:
            SetEfuseKekIVFileName(filename);
            break;

        case BIF::BisonParser::token::EFUSE_USER_KEK0_IV:
            SetEfuseUserKek0IVFileName(filename);
            break;

        case BIF::BisonParser::token::EFUSE_USER_KEK1_IV:
            SetEfuseUserKek1IVFileName(filename);
            break;

        case BIF::BisonParser::token::USER_KEYS:
            SetUserKeysFileName(filename);
            break;
    }
}

/******************************************************************************/
void BifOptions::SetRegInitFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f) 
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
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
        LOG_ERROR("Cannot read file - %s", filename.c_str());
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
        LOG_ERROR("Cannot read file - %s", currentPartitionBifOptions->filename.c_str());
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
void BifOptions::SetPmcdataFile(const std::string& filename)
{
    if (arch == Arch::ZYNQ || arch == Arch::ZYNQMP )//todo: add error check for pmc data is used for spartanup set pmc as pldat in bif.l(arch == Arch::SPARTANUP)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t[pmcdata] not supported in zynq and zynqmp architectures");
    }
    pmcdataFile = filename;
}

/******************************************************************************/
void BifOptions::ClearPmcCdoFileList()
{
    pmcCdoFileList.clear();
}

/******************************************************************************/
void BifOptions::SetPmcCdoFileList(const std::string& filename)
{
    pmcCdoFileList.push_back(filename);
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
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }
    spkSignatureFile = filename;
}

/******************************************************************************/
void BifOptions::SetBHSignFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }
    bhSignatureFile = filename;
}

/******************************************************************************/
void BifOptions::SetHeaderSignFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
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
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    bhKeyFile = filename;
    LOG_TRACE("Setting BH Key file as %s", bhKeyFile.c_str());
}

/******************************************************************************/
void BifOptions::SetTotalpmcdataSize(uint32_t size)
{
    totalpmcdataSize = size;
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
    if (arch == Arch::VERSAL)
    {
        if ((shutterVal & 0x80000000) == 0)
        {
            LOG_ERROR("The PUF shutter value specified in the BIF file indicates that the Global Variation Filter was not enabled during PUF registration/provisioning.\n\t   The Global Variation Filter must be used during PUF registration/provisioning to avoid PUF key encryption keys with lower than expected entropy ");
        }
    }
}

/******************************************************************************/
void BifOptions::SetPufRingOscilltorSwapConfigValue(uint32_t value)
{
    if (arch == Arch::ZYNQ || arch == Arch::ZYNQMP || (arch == Arch::VERSAL && !versalNetSeries))
        LOG_ERROR("BIF attribute error !!!\n\t  'puf_ro_swap' is not supported with the mentioned -arch.");

    pufRoSwapConfigVal = value;
}

/******************************************************************************/
void BifOptions::SetDiceEnable()
{
    if (arch == Arch::ZYNQ || arch == Arch::ZYNQMP || (arch == Arch::VERSAL && !versalNetSeries))
        LOG_ERROR("BIF attribute error !!!\n\t  'dice_enable' is not supported with the mentioned -arch.");

    dice = DICE::DiceEnable;
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
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    pufHelperFile = filename;
    LOG_TRACE("Setting PUF Helper Data file as %s", pufHelperFile.c_str());
}

/******************************************************************************/
void BifOptions::SetBHKekIVFileName(std::string filename)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'bh_key_iv/bh_kek_iv' is not supported with the mentioned -arch.");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    bhKekIVFile = filename;
    LOG_TRACE("Setting BH Kek IV file as %s", bhKekIVFile.c_str());
}

/******************************************************************************/
void BifOptions::SetBbramKekIVFileName(std::string filename)
{
    if (arch == Arch::ZYNQ || arch == Arch::ZYNQMP)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'bbram_kek_iv' is not supported with the mentioned -arch.");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    bbramKekIVFile = filename;
    LOG_TRACE("Setting Bbram Kek IV file as %s", bbramKekIVFile.c_str());
}

/******************************************************************************/
void BifOptions::SetEfuseKekIVFileName(std::string filename)
{
    if (arch == Arch::ZYNQ || arch == Arch::ZYNQMP)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'efuse_kek_iv' is not supported with the mentioned -arch.");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    efuseKekIVFile = filename;
    LOG_TRACE("Setting Efuse Kek IV file as %s", efuseKekIVFile.c_str());
}

/******************************************************************************/
void BifOptions::SetEfuseUserKek0IVFileName(std::string filename)
{
    if (arch == Arch::ZYNQ || arch == Arch::ZYNQMP)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'efuse_user_kek0_iv' is not supported with the mentioned -arch.");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    efuseUserKek0IVFile = filename;
    /* CDO build_block_write injection (F201 user KEK IV range) is Versal 2VE-2VM only */
    if (arch == Arch::VERSALGEN2) {
        uint8_t iv[EFUSE_USER_KEK_IV_BYTES];
        FileImport fileReader;
        if (!fileReader.LoadHexData(filename, iv, EFUSE_USER_KEK_IV_BYTES))
        {
            LOG_ERROR("Invalid data bytes for efuse user KEK0 IV.\n           Expected length is 12 bytes in %s",
                filename.c_str());
        }
        cdocmd_set_efuse_user_kek0_iv(iv);
    }
    LOG_TRACE("Setting Efuse User Kek0 IV file as %s", efuseUserKek0IVFile.c_str());
}

/******************************************************************************/
void BifOptions::SetEfuseUserKek1IVFileName(std::string filename)
{
    if (arch == Arch::ZYNQ || arch == Arch::ZYNQMP)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'efuse_user_kek1_iv' is not supported with the mentioned -arch.");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    efuseUserKek1IVFile = filename;
    if (arch == Arch::VERSALGEN2) {
        uint8_t iv[EFUSE_USER_KEK_IV_BYTES];
        FileImport fileReader;
        if (!fileReader.LoadHexData(filename, iv, EFUSE_USER_KEK_IV_BYTES))
        {
            LOG_ERROR("Invalid data bytes for efuse user KEK1 IV.\n           Expected length is 12 bytes in %s",
                filename.c_str());
        }
        cdocmd_set_efuse_user_kek1_iv(iv);
    }
    LOG_TRACE("Setting Efuse User Kek1 IV file as %s", efuseUserKek1IVFile.c_str());
}

/******************************************************************************/
void BifOptions::SetUserKeysFileName(std::string filename)
{
    if (arch == Arch::ZYNQ  || arch == Arch::ZYNQMP)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'userkeys' is not supported with the mentioned -arch.");
    }

    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Cannot read file - %s", filename.c_str());
    }

    userKeyFile = filename;
    ParseUserKeyFile(userKeyFile);
}

/******************************************************************************/
void static SetUserKey(const uint8_t* key, uint32_t* userKey)
{
    for (uint32_t index = 0; index < WORDS_PER_AES_KEY; index++)
    {
        userKey[index] = ReadBigEndian32(key);
        key += sizeof(uint32_t);
    }
}

/******************************************************************************/
void BifOptions::ParseUserKeyFile(std::string inputFileName)
{
    LOG_TRACE("Reading the user key file %s",inputFileName.c_str());
    std::ifstream keyFile(inputFileName.c_str());

    if (!keyFile)
    {
        LOG_ERROR("Failure reading user key file - %s", inputFileName.c_str());
    }

    while (keyFile)
    {
        std::string word;
        keyFile >> word;
        if (word == "")
        {
            return;
        }

        //char c = ' ';
        int c = (int)word.back() - 48;
        word.pop_back();
        if (word == "user_key")
        {
            if (c < 0 || c > 7)
            {
                LOG_ERROR("The AES user keys available are from 0 to 7. user_key%d is not supported", c);
            }

            word = "";
            keyFile >> word;
            uint8_t hexData[256] = { 0 };
            if (!(word.size() == (WORDS_PER_AES_KEY * 8) || word.size() == (WORDS_PER_AES_KEY * 4)))
            {
                LOG_ERROR("An AES user key must be 128/256 bits long - %s", word.c_str());
            }

            if (word.size() & 1)
            {
                LOG_ERROR("Error parsing AES user key \n\t\t Hex String - %s - does not have even no.of hex digits", word.c_str());
            }

            for (uint32_t i = 0, j = 0; i < word.size(); i += 2, j++)
            {
                std::string byte = word.substr(i, 2);
                if (!isxdigit(byte[0]) || !isxdigit(byte[1]))
                {
                    LOG_ERROR("Error parsing AES user key\n\t\t Hex String - %s - is has a non hex digit", word.c_str());
                }
                if (word.size() == WORDS_PER_AES_KEY * 4)
                {
                    hexData[j+16] = (uint8_t)strtoul(byte.c_str(), NULL, 16);
                }
                else
                {
                    hexData[j] = (uint8_t)strtoul(byte.c_str(), NULL, 16);
                }
            }
            SetUserKey(hexData, (uint32_t*)&(user_keys.user_keys_array[c][0]));
            user_keys.loaded_mask |= (1u << c);
        }
        else
        {
            /* If the word is neither of the above */
            LOG_DEBUG(DEBUG_STAMP, "'user_key' identifier expected, '%s' found instead", word.c_str());
            LOG_ERROR("Error parsing User key file - %s", inputFileName.c_str());
        }
    }
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
        LOG_ERROR("Cannot read file - %s", filename.c_str());
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
void BifOptions::SetBootDeviceAddress(uint32_t address)
{
    bootDeviceAddress = address;
}

/******************************************************************************/
OptKey::Type BifOptions::GetAesOptKeyFlag(void)
{
    return optKey;
}

/******************************************************************************/
std::string BifOptions::GetKeySourceName(KeySource::Type type)
{
    switch (type)
    {
    case KeySource::None: return "None"; break;
    case KeySource::BbramRedKey: return "bbram_red_key"; break;
    case KeySource::EfuseRedKey: return "efuse_red_key"; break;
    case KeySource::EfuseBlkKey: return "efuse_blk_key"; break;
    case KeySource::BhBlkKey: return "bh_blk_key"; break;
    case KeySource::EfuseGryKey: return "efuse_gry_key"; break;
    case KeySource::BhGryKey: return "bh_gry_key"; break;
    case KeySource::BhKupKey: return "kup_key"; break;
    case KeySource::BbramBlkKey: return "bbram_blk_key"; break;
    case KeySource::BbramGryKey: return "bbram_gry_key"; break;
    case KeySource::UserKey0: return "user_key0"; break;
    case KeySource::UserKey1: return "user_key1"; break;
    case KeySource::UserKey2: return "user_key2"; break;
    case KeySource::UserKey3: return "user_key3"; break;
    case KeySource::UserKey4: return "user_key4"; break;
    case KeySource::UserKey5: return "user_key5"; break;
    case KeySource::UserKey6: return "user_key6"; break;
    case KeySource::UserKey7: return "user_key7"; break;
    case KeySource::EfuseUserKey0: return "efuse_user_key0"; break;
    case KeySource::EfuseUserBlkKey0: return "efuse_user_blk_key0"; break;
    case KeySource::EfuseUserGryKey0: return "efuse_user_gry_key0"; break;
    case KeySource::EfuseUserKey1: return "efuse_user_key1"; break;
    case KeySource::EfuseUserBlkKey1: return "efuse_user_blk_key1"; break;
    case KeySource::EfuseUserGryKey1: return "efuse_user_gry_key1"; break;
    default: return "None"; break;
    }
}

/******************************************************************************/
void BifOptions::CheckForSameKeyandKeySrcPair(std::vector<std::pair<KeySource::Type, std::unique_ptr<uint32_t[]>>>& aesKeyandKeySrc)
{
    static bool warningGiven = false;
    std::multimap<std::string, std::string> key0_keysrc;
    for (uint32_t i = 0; i < aesKeyandKeySrc.size(); i++)
    {
        std::stringstream key0String;
        for (uint32_t j = 0; j < WORDS_PER_AES_KEY; j++)
        {
            key0String << std::setfill('0') << std::setw(2) << std::hex << aesKeyandKeySrc[i].second.get()[j];
        }
        key0_keysrc.insert(std::pair<std::string, std::string>(key0String.str(), GetKeySourceName(aesKeyandKeySrc[i].first)));
    }

    for (auto unique_key0s = key0_keysrc.begin(), end = key0_keysrc.end(); unique_key0s != end; unique_key0s = key0_keysrc.upper_bound((unique_key0s->first)))
    {
        std::string warnKeySrc = "";
        int count = 0;
        std::multimap<std::string, std::string>::iterator keysrc_itr = key0_keysrc.find(unique_key0s->first);
        for (uint32_t i = 0; i < key0_keysrc.count(unique_key0s->first); i++)
        {
            if (warnKeySrc.find(keysrc_itr->second.c_str()) == std::string::npos)
            {
                warnKeySrc += keysrc_itr->second.c_str();
                warnKeySrc += ", ";
                count++;
            }
            ++keysrc_itr;
        }
        if (warnKeySrc != "" && count > 1)
        {
            warnKeySrc.erase(warnKeySrc.size() - 2);
            if (!warningGiven)
            {
                LOG_MSG("[WARNING]: Same Key0 is used for different KeySources.\n");
                warningGiven = true;
            }
            LOG_MSG("\t   Key Sources : %s", warnKeySrc.c_str());
            LOG_MSG("\t   Key0 Used   : %s\n", unique_key0s->first.c_str());
        }
    }
}

/******************************************************************************/
void BifOptions::CheckForBadKeyandKeySrcPair(std::vector<std::pair<KeySource::Type, std::unique_ptr<uint32_t[]>>>& aesKeyandKeySrc, std::string aesFilename)
{
    std::string errorKeySrc = "";
    for (uint32_t i = 0; i < aesKeyandKeySrc.size(); i++)
    {
        uint32_t j = aesKeyandKeySrc.size() - 1;
        if (i != j)
        {
            if (aesKeyandKeySrc[i].first == aesKeyandKeySrc[j].first)
            {
                if (memcmp(aesKeyandKeySrc[i].second.get(), aesKeyandKeySrc[j].second.get(), AES_GCM_KEY_SZ) != 0)
                {
                    if (errorKeySrc.find(GetKeySourceName(aesKeyandKeySrc[i].first)) == std::string::npos)
                    {
                        errorKeySrc += GetKeySourceName(aesKeyandKeySrc[i].first);
                        errorKeySrc += ", ";
                    }
                }
            }
        }
    }
    if (errorKeySrc != "")
    {
        errorKeySrc.erase(errorKeySrc.size() - 2);
        LOG_ERROR("Check %s\n\t   Key0 must be same across the BIF for a given KeySource : %s", aesFilename.c_str(), errorKeySrc.c_str());
    }
}

/******************************************************************************/
static void ValidateEncryptionKeySource(KeySource::Type type, bool versal)
{
    if (versal)
    {
        if ((type == KeySource::EfuseGryKey) || (type == KeySource::BhGryKey) || (type == KeySource::BbramGryKey)
            || (type == KeySource::EfuseUserGryKey0) || (type == KeySource::EfuseUserGryKey1))
        {
            LOG_ERROR("The usage of obfuscated keys is deprecated in Versal.\n\t   Refer 'bootgen -arch versal -bif_help keysrc' for valid key sources.");
        }
    }

    static bool bhBlkKek = false;
    static bool bhGryKek = false;
    if ((type == KeySource::BhBlkKey && bhGryKek) || (type == KeySource::BhGryKey && bhBlkKek))
    {
        LOG_ERROR("'bh_blk_key and bh_gry_key' cannot be used in a single boot image.");
    }
    else if (type == KeySource::BhBlkKey)
    {
        bhBlkKek = true;
    }
    else if (type == KeySource::BhGryKey)
    {
        bhGryKek = true;
    }
    static bool bbramBlkKek = false;
    static bool bbramGryKek = false;
    if ((type == KeySource::BbramBlkKey && bbramGryKek) || (type == KeySource::BbramGryKey && bbramBlkKek))
    {
        LOG_ERROR("'bbram_blk_key and bbram_gry_key' cannot be used in a single boot image.");
    }
    else if (type == KeySource::BbramBlkKey)
    {
        bbramBlkKek = true;
    }
    else if (type == KeySource::BbramGryKey)
    {
        bbramGryKek = true;
    }

    static bool efuseBlkKek = false;
    static bool efuseGryKek = false;
    if ((type == KeySource::EfuseBlkKey && efuseGryKek) || (type == KeySource::EfuseGryKey && efuseBlkKek))
    {
        LOG_ERROR("'efuse_blk_key and efuse_gry_key' cannot be used in a single boot image.");
    }
    else if (type == KeySource::EfuseBlkKey)
    {
        efuseBlkKek = true;
    }
    else if (type == KeySource::EfuseGryKey)
    {
        efuseGryKek = true;
    }

    static bool efuseUserBlkKek0 = false;
    static bool efuseUserGryKek0 = false;
    if ((type == KeySource::EfuseUserBlkKey0 && efuseUserGryKek0) || (type == KeySource::EfuseUserGryKey0 && efuseUserBlkKek0))
    {
        LOG_ERROR("'efuse_user_blk_key0 and efuse_user_gry_key0' cannot be used in a single boot image.");
    }
    else if (type == KeySource::EfuseUserBlkKey0)
    {
        efuseUserBlkKek0 = true;
    }
    else if (type == KeySource::EfuseUserGryKey0)
    {
        efuseUserGryKek0 = true;
    }

    static bool efuseUserBlkKek1 = false;
    static bool efuseUserGryKek1 = false;
    if ((type == KeySource::EfuseUserBlkKey1 && efuseUserGryKek1) || (type == KeySource::EfuseUserGryKey1 && efuseUserBlkKek1))
    {
        LOG_ERROR("'efuse_user_blk_key1 and efuse_user_gry_key1' cannot be used in a single boot image.");
    }
    else if (type == KeySource::EfuseUserBlkKey1)
    {
        efuseUserBlkKek1 = true;
    }
    else if (type == KeySource::EfuseUserGryKey1)
    {
        efuseUserGryKek1 = true;
    }
}

/******************************************************************************/
void BifOptions::SetMetaHeaderEncryptionKeySource(KeySource::Type type)
{
    ValidateEncryptionKeySource(type, (arch == Arch::VERSAL && !versalNetSeries));
    metaHdrAttributes.encrKeySource = type;
}

/******************************************************************************/
void BifOptions::SetMetaHeaderEncryptType(Encryption::Type type)
{
    metaHdrAttributes.encrypt = type;
}

/******************************************************************************/
void BifOptions::SetMetaHeaderEncryptionKeyFile(std::string file)
{
    metaHdrAttributes.encrKeyFile = file;
}

/******************************************************************************/
void BifOptions::SetMetaHeaderAuthType(Authentication::Type type)
{
    if (type == Authentication::LMS_SHA2_256 || type == Authentication::LMS_SHAKE256)
    {
        metaHdrAttributes.lmsOnly = true;
    }
    else if (type == Authentication::HSS_SHA2_256 || type == Authentication::HSS_SHAKE256)
    {
        metaHdrAttributes.lmsOnly = false;
        if (type == Authentication::HSS_SHA2_256)
        {
            type = Authentication::LMS_SHA2_256;
        }
        else if (type == Authentication::HSS_SHAKE256)
        {
            type = Authentication::LMS_SHAKE256;
        }
    }

    if (type == Authentication::ECDSAp521)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t  'ecdsa-p521' not supported for Meta Header");
    }
    metaHdrAttributes.authenticate = type;
}

/******************************************************************************/
void BifOptions::SetPufHdinBHFlag()
{
    pufHdinBHEnable = true;
}

/******************************************************************************/
void BifOptions::SetAuthJtagRevokeID(uint32_t value)
{
    if (arch == Arch::SPARTANUP && value > 95)
    {
        LOG_ERROR("revoke_id can only take values from 0x0 to 0x5F for spartanup.");
    }
    else if (value > 0xFF)
    {
        LOG_ERROR("revoke_id can only take values from 0x0 to 0xFF.");
    }
    authJtagInfo.partitionRevokeId = value;
    authJtagInfo.userRevokeId = true;
}

/******************************************************************************/
void BifOptions::SetAuthJtagSPKRevokeID(uint32_t value)
{
    if (arch == Arch::SPARTANUP && value > 95)
    {
        LOG_ERROR("spk_revoke_id can only take values from 0x0 to 0x5F for spartanup.");
    }
    else if (value > 0xFF)
    {
        LOG_ERROR("revoke_id can only take values from 0x0 to 0xFF.");
    }
    authJtagInfo.spkRevokeId = value;
    authJtagInfo.userRevokeId = true;
}

/******************************************************************************/
void BifOptions::SetAuthJtagDeviceDna(std::string hexString)
{
    authJtagInfo.userDeviceDNA = true;
    uint32_t hexStringLength = (uint32_t)hexString.size();
    if (hexStringLength & 1)
    {
        LOG_DEBUG(DEBUG_STAMP, "Hex String - %s - does not have even no. of hex digits", hexString.c_str());
        LOG_ERROR("Error parsing Device DNA");
    }

    for (uint32_t i = 0,j = 0; i < hexStringLength; i += 2,j++)
    {
        std::string byte = hexString.substr(i, 2);
        if (!isxdigit(byte[0]) || !isxdigit(byte[1]))
        {
            LOG_DEBUG(DEBUG_STAMP, "Hex String - %s - is has a non hex digit", hexString.c_str());
            LOG_ERROR("Error parsing Device DNA");
        }
        authJtagInfo.deviceDNA[j] = (uint8_t)strtoul(byte.c_str(), NULL, 16);
    }
}

/******************************************************************************/
void BifOptions::SetAuthJtagTimeOut(uint32_t value)
{
    authJtagInfo.jtagTimeout = value;
}

/******************************************************************************/
void BifOptions::SetAuthJtagSignatureFile(std::string value)
{
    authJtagInfo.jtagSignatureFile = value;
}

/******************************************************************************/
void PartitionBifOptions::SetEncryptionKeySource(KeySource::Type type)
{
    ValidateEncryptionKeySource(type, (arch == Arch::VERSAL && !versalNetSeries));
    keySrc = type;
}

/******************************************************************************/
void PartitionBifOptions::SetArchType(Arch::Type type, bool versalNet,  uint32_t idcode)
{
    arch = type;
    versalNetSeries = versalNet;
    IdCodeManager manager;
    if(!manager.findIdCode(idcode))
    {
        dl9Series = true;
    }else
    {
        dl9Series = false;
    }
}

/******************************************************************************/
void PartitionBifOptions::SetEncryptionBlocks(uint32_t size, uint32_t num)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'blocks' not supported in ZYNQ architecture");
    }

    /* The size of each block is considered in bytes */
    if ((size & 0x3) != 0)
    {
        LOG_DEBUG(DEBUG_STAMP, "Encryption Block Size Error - Block Size - %d", size);
        LOG_ERROR("BIF attribute 'blocks' must specify sizes which are multiples of 4, for word alignment.");
    }

    if((arch == Arch::VERSALGEN2 || (arch == Arch::VERSAL && versalNetSeries) || arch == Arch::SPARTANUP) && size < 80)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\tThe minimum block size allowed is 5 AES encryption blocks (i.e., 80 bytes)");
    }

    if (arch == Arch::VERSAL || arch == Arch::VERSALNET || arch == Arch::VERSALGEN2)
    {
        if (size < 64)
        {
            LOG_ERROR("BIF attribute error !!!\n\t\tThe minimum block size allowed is 4 AES encryption blocks (i.e., 64 bytes)");
        }
        if(size % 16 != 0)
        {
            LOG_ERROR("BIF attribute error !!!\n\t\tThe block size specified must be 16 byte aligned. Block size - %d",size);
        }
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
        LOG_ERROR("Cannot choose block size after choosing a default block size.");
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
    if (((arch == Arch::SPARTANUP) && IsDl9Series()) && (type != Authentication::LMS_SHAKE256 && type != Authentication::HSS_SHAKE256 && type != Authentication::ECDSA))
    {
        LOG_ERROR("BIF attribute error !!!\n           'authentication= lms-shake256/hss-shake256/ecdsa-p384' is not supported with the mentioned -arch.");
    }

    if ((arch == Arch::SPARTANUP) && !IsDl9Series() && type != Authentication::HSS_SHAKE256)
    {
        LOG_ERROR("BIF attribute error !!!\n           'authentication=hss-shake256' only is supported with the mentioned -arch..");
    }

    if (type == Authentication::HSS_SHA2_256 || type == Authentication::HSS_SHAKE256)
    {
        lmsOnly = false;
        if (type == Authentication::HSS_SHA2_256)
        {
            type = Authentication::LMS_SHA2_256;
        }
        else if (type == Authentication::HSS_SHAKE256)
        {
            type = Authentication::LMS_SHAKE256;
        }
    }

    authType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetHybridAuthType(std::string hybridSpec)
{
    // Validate architecture support for hybrid authentication
    if (arch != Arch::VERSAL_2VP)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'hybrid' authentication is supported only in VERSAL_2VP architecture");
    }

    // Parse hybrid specification: "rsa,mldsa" or "ecdsa,mldsa" etc.
    hybridAlgorithms.clear();
    hybridAuth = true;
    
    // Flag to track if any HSS algorithm is used (need to set lmsOnly=false)
    bool hssDetected = false;
    
    // Split by comma
    std::string::size_type start = 0;
    std::string::size_type end = 0;
    
    while ((end = hybridSpec.find(',', start)) != std::string::npos) 
    {
        std::string alg = hybridSpec.substr(start, end - start);
        
        // Trim whitespace
        alg.erase(0, alg.find_first_not_of(" \t"));
        alg.erase(alg.find_last_not_of(" \t") + 1);
        
        // Convert to Authentication::Type
        Authentication::Type algType = Authentication::None;
        if (alg == "rsa") {
            algType = Authentication::RSA;
        } else if (alg == "ecdsa" || alg == "ecdsa-p384") {
            algType = Authentication::ECDSA;
        } else if (alg == "ecdsa-p521") {
            algType = Authentication::ECDSAp521;
        } else if (alg == "mldsa") {
            algType = Authentication::MLDSA;
        } else if (alg == "slh-shake256") {
            algType = Authentication::SLH_SHAKE256;
        } else if (alg == "lms-sha256") {
            algType = Authentication::LMS_SHA2_256;
        } else if (alg == "lms-shake256") {
            algType = Authentication::LMS_SHAKE256;
        } else if (alg == "hss-sha256") {
            // Convert HSS to LMS internally, but set lmsOnly=false
            algType = Authentication::LMS_SHA2_256;
            hssDetected = true;
        } else if (alg == "hss-shake256") {
            // Convert HSS to LMS internally, but set lmsOnly=false
            algType = Authentication::LMS_SHAKE256;
            hssDetected = true;
        } else {
            LOG_ERROR("BIF attribute error !!!\n\t\tUnsupported hybrid algorithm: '%s'\n\t\tSupported algorithms: rsa, ecdsa, ecdsa-p521, mldsa, slh-shake256, lms-sha256, lms-shake256, hss-sha256, hss-shake256", alg.c_str());
        }
        
        hybridAlgorithms.push_back(algType);
        start = end + 1;
    }
    
    // Handle the last algorithm
    if (start < hybridSpec.length()) 
    {
        std::string alg = hybridSpec.substr(start);
        
        // Trim whitespace
        alg.erase(0, alg.find_first_not_of(" \t"));
        alg.erase(alg.find_last_not_of(" \t") + 1);
        
        // Convert to Authentication::Type
        Authentication::Type algType = Authentication::None;
        if (alg == "rsa") {
            algType = Authentication::RSA;
        } else if (alg == "ecdsa" || alg == "ecdsa-p384") {
            algType = Authentication::ECDSA;
        } else if (alg == "ecdsa-p521") {
            algType = Authentication::ECDSAp521;
        } else if (alg == "mldsa") {
            algType = Authentication::MLDSA;
        } else if (alg == "slh-shake256") {
            algType = Authentication::SLH_SHAKE256;
        } else if (alg == "lms-sha256") {
            algType = Authentication::LMS_SHA2_256;
        } else if (alg == "lms-shake256") {
            algType = Authentication::LMS_SHAKE256;
        } else if (alg == "hss-sha256") {
            // Convert HSS to LMS internally, but set lmsOnly=false
            algType = Authentication::LMS_SHA2_256;
            hssDetected = true;
        } else if (alg == "hss-shake256") {
            // Convert HSS to LMS internally, but set lmsOnly=false
            algType = Authentication::LMS_SHAKE256;
            hssDetected = true;
        } else {
            LOG_ERROR("BIF attribute error !!!\n\t\tUnsupported hybrid algorithm: '%s'\n\t\tSupported algorithms: rsa, ecdsa, ecdsa-p521, mldsa, slh-shake256, lms-sha256, lms-shake256, hss-sha256, hss-shake256", alg.c_str());
        }
        
        hybridAlgorithms.push_back(algType);
    }
    
    // Set lmsOnly flag based on whether HSS was detected
    // HSS means hierarchical signature scheme (lmsOnly = false)
    // LMS means single-level signature scheme (lmsOnly = true, default)
    if (hssDetected) {
        lmsOnly = false;
        LOG_INFO("HSS algorithm detected in hybrid mode, setting lmsOnly = false");
    } else {
        // Check if any LMS algorithm is present (default is true)
        lmsOnly = true;
    }
    
    // Validate hybrid combination
    if (hybridAlgorithms.size() != 2) 
    {
        LOG_ERROR("BIF attribute error !!!\n\t\tHybrid authentication requires exactly 2 algorithms, got %d", (int)hybridAlgorithms.size());
    }
    
    // Validate supported combinations - ONLY allow one PQC + one non-PQC combination
    bool validCombination = false;
    if (hybridAlgorithms.size() == 2) 
    {
        Authentication::Type first = hybridAlgorithms[0];
        Authentication::Type second = hybridAlgorithms[1];
        
        // Helper function to check if algorithm is Post-Quantum Cryptography
        auto isPQC = [](Authentication::Type alg) -> bool {
            return (alg == Authentication::MLDSA || 
                    alg == Authentication::SLH_SHAKE256 ||
                    alg == Authentication::LMS_SHA2_256 || 
                    alg == Authentication::LMS_SHAKE256 ||
                    alg == Authentication::HSS_SHA2_256 ||
                    alg == Authentication::HSS_SHAKE256);
        };
        
        // Helper function to check if algorithm is Classical (non-PQC)
        auto isClassical = [](Authentication::Type alg) -> bool {
            return (alg == Authentication::RSA || 
                    alg == Authentication::ECDSA || 
                    alg == Authentication::ECDSAp521);
        };
        
        // Check if first is Classical and second is PQC
        bool firstClassicalSecondPQC = isClassical(first) && isPQC(second);
        
        // Check if first is PQC and second is Classical  
        bool firstPQCSecondClassical = isPQC(first) && isClassical(second);
        
        // ONLY allow Classical + PQC combinations (in either order)
        if (firstClassicalSecondPQC || firstPQCSecondClassical) {
            validCombination = true;
        } else {
            // Helper function to get algorithm name for error messages
            auto getAlgorithmName = [](Authentication::Type alg) -> std::string {
                switch(alg) {
                    case Authentication::RSA: return "rsa";
                    case Authentication::ECDSA: return "ecdsa";
                    case Authentication::ECDSAp521: return "ecdsa-p521";
                    case Authentication::MLDSA: return "mldsa";
                    case Authentication::SLH_SHAKE256: return "slh-shake256";
                    case Authentication::LMS_SHA2_256: return "lms-sha256";
                    case Authentication::LMS_SHAKE256: return "lms-shake256";
                    default: return "unknown";
                }
            };
            
            std::string firstAlg = getAlgorithmName(first);
            std::string secondAlg = getAlgorithmName(second);
            
            // All other combinations are invalid - provide specific error messages
            if (first == second) {
                LOG_ERROR("BIF attribute error !!!\n\t\tYou have specified: %s,%s\n\t\tHybrid authentication error: Same algorithm specified twice\n\t\tSupported combinations: One Classical + One PQC only (rsa+mldsa, ecdsa+slh-shake256, etc.)", 
                         firstAlg.c_str(), secondAlg.c_str());
            } else if (isClassical(first) && isClassical(second)) {
                LOG_ERROR("BIF attribute error !!!\n\t\tYou have specified: %s,%s\n\t\tHybrid authentication error: Classical+Classical combination not allowed\n\t\tSupported combinations: One Classical + One PQC only (rsa+mldsa, ecdsa+slh-shake256, etc.)", 
                         firstAlg.c_str(), secondAlg.c_str());
            } else if (isPQC(first) && isPQC(second)) {
                LOG_ERROR("BIF attribute error !!!\n\t\tYou have specified: %s,%s\n\t\tHybrid authentication error: PQC+PQC combination not allowed\n\t\tSupported combinations: One Classical + One PQC only (rsa+mldsa, ecdsa+slh-shake256, etc.)", 
                         firstAlg.c_str(), secondAlg.c_str());
            } else {
                LOG_ERROR("BIF attribute error !!!\n\t\tYou have specified: %s,%s\n\t\tHybrid authentication error: Unsupported algorithm combination\n\t\tSupported combinations: One Classical + One PQC only (rsa+mldsa, ecdsa+slh-shake256, etc.)", 
                         firstAlg.c_str(), secondAlg.c_str());
            }
        }
    }
    
    // Final validation - ensure we have a valid combination
    if (!validCombination) {
        LOG_ERROR("BIF attribute error !!!\n\t\tUnsupported hybrid combination\n\t\tSupported combinations: Classical+PQC only (rsa+mldsa, rsa+slh-shake256, ecdsa+mldsa, ecdsa+slh-shake256, ecdsa-p521+mldsa, ecdsa-p521+slh-shake256, rsa+lms-sha256, rsa+lms-shake256, ecdsa+lms-sha256, ecdsa+lms-shake256, ecdsa-p521+lms-sha256, ecdsa-p521+lms-shake256)");
    }
    
    // Set primary authType to first algorithm for compatibility
    authType = hybridAlgorithms[0];
    
    LOG_INFO("Hybrid authentication enabled with algorithms: %s", hybridSpec.c_str());
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
        LOG_ERROR("BIF attribute error !!!\n\t\t'checksum=sha2' is not supported.");
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
void PartitionBifOptions::SetPartitionType(PartitionType::Type type)
{
    partitionType = type;
}

/******************************************************************************/
void PartitionBifOptions::SetDpaCM(DpaCM::Type value)
{
    if (arch == Arch::ZYNQ || arch == Arch::ZYNQMP)
        LOG_ERROR("BIF attribute error !!!\n\t\t'dpacm_enable' is not supported with the mentioned -arch. Refer 'bootgen -help' for more details.");

    dpaCM = value;
}

/******************************************************************************/
void PartitionBifOptions::SetExceptionLevel(ExceptionLevel::Type type)
{
    if (arch == Arch::ZYNQ)
        LOG_ERROR("BIF attribute error !!!\n\t\t'exception_level' not supported with the mentioned -arch. Refer 'bootgen -help' for more details.");

    exceptionLevel = type;
}

/******************************************************************************/
void PartitionBifOptions::SetSpkId(uint32_t id)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'spk_id' is not supported with the mentioned -arch.");
    }
    if (arch == Arch::SPARTANUP && id > 95)
    {
        LOG_ERROR("spk_id can only take values from 0x0 to 0x5F for spartanup.");
    }
    spkId = id;
    spkIdLocal = true;
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
        LOG_ERROR("BIF attribute error !!!\n\t\t'trustzone' not supported in ZYNQ architecture");
    }
    trustzone = type;
}

/******************************************************************************/
void PartitionBifOptions::SetEarlyHandoff(bool flag)
{
    if (arch == Arch::ZYNQ)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'early_handoff' not supported in ZYNQ architecture");
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
void PartitionBifOptions::SetSPKRevokeId(uint32_t id)
{
    if (arch == Arch::SPARTANUP && id > 95)
    {
        LOG_ERROR("spk_revoke_id can only take values from 0x0 to 0x5F for spartanup.");
    }
    else if (id > 0xFF)
    {
        LOG_ERROR("revoke_id can only take values from 0x0 to 0xFF.");
    }
    spkRevokeId = id;
}

/******************************************************************************/
void PartitionBifOptions::SetPartitionRevokeId(uint32_t id)
{
    if (arch == Arch::SPARTANUP && id > 95)
    {
        LOG_ERROR("partition_revoke_id can only take values from 0x0 to 0x5F for spartanup.");
    }
    else if (id > 0xFF)
    {
        LOG_ERROR("partition_revoke_id can only take values from 0x0 to 0xFF.");
    }
    partitionRevokeId = id;
}

/******************************************************************************/
void PartitionBifOptions::SetSlrNum(uint8_t id)
{
    slrNum = id;
}

/******************************************************************************/
void PartitionBifOptions::SetClusterNum(uint8_t id)
{
    if (arch == Arch::VERSAL && versalNetSeries)
    {
        if (id != 0 && id != 1 && id != 2 && id != 3)
        {
            LOG_ERROR("cluster can only take values from 0 to 3.");
        }
        clusterNum = id;
    }
    else if (arch == Arch :: VERSALGEN2 || arch == Arch :: VERSAL_2VP)
    {
        if (id != 0 && id != 1 && id != 2 && id != 3 && id != 4)
        {
            LOG_ERROR("cluster can only take values from 0 to 4 in versal_2ve_2vm & versal_2vp.");
        }
        clusterNum = id;
    }
    else
    {
        LOG_ERROR("BIF attribute error !!!\n\t  'cluster' is supported only in VersalNet and above architecture");
    }
}

/******************************************************************************/
void BifOptions::SetRevokeId(uint32_t id)
{
    if (arch == Arch::SPARTANUP && id > 95)
    {
        LOG_ERROR("revoke_id can only take values from 0x0 to 0x5F for spartanup.");
    }
    else if (id > 0xFF)
    {
        LOG_ERROR("revoke_id can only take values from 0x0 to 0xFF.");
    }
    spkRevokeId = id;
}


/******************************************************************************/
void LmsParameterError(std::string param)
{
    LOG_ERROR("Unknown LMS parameter string\nValid options are: lms_sha256_h5_w2, \
lms_sha256_h5_w4, lms_sha256_h5_w8, lms_sha256_h10_w2, lms_sha256_h10_w4, lms_sha256_h10_w8, \
lms_sha256_h15_w2, lms_sha256_h15_w4, lms_sha256_h15_w8, lms_sha256_h20_w2, lms_sha256_h20_w4, \
lms_sha256_h20_w8, lms_shake256_h5_w2, lms_shake256_h5_w4, lms_shake256_h5_w8, lms_shake256_h10_w2, \
lms_shake256_h10_w4, lms_shake256_h10_w8, lms_shake256_h15_w2, lms_shake256_h15_w4, lms_shake256_h15_w8, \
lms_shake256_h20_w2, lms_shake256_h20_w4, lms_shake256_h20_w8");
}

/******************************************************************************/
void BifOptions::SetLmsParameters(LmsKeyTypeParam param)
{
    LOG_INFO("Parsing LMS parameter string - %s", param.lms_param.c_str());
    std::stringstream ss(param.lms_param);
    std::string item;
    
    int i = 0;
    LmsKeyParam temp;
    while (std::getline(ss, item, '_'))
    {
        switch (i++)
        {
        case 0:
            if (item != "lms")
            {
                LmsParameterError(param.lms_param);
            }
            break;
        case 1:
            if (item == "sha256")
            {
                temp.hashFunction = HashFunction::SHA2_256;
            }
            else if (item == "shake256")
            {
                temp.hashFunction = HashFunction::SHAKE256;
            }
            else
            {
                LOG_INFO("Unsupported hash function - %s\n\t   Supported hash functions - sha256, shake256", item.c_str());
                LmsParameterError(param.lms_param);
            }
            break;
        case 2:
        case 3:
            switch (item.front())
            {
                case 'h':
                    item.erase(item.begin());
                    temp.h = strtoul(item.c_str(), NULL, 10);
                    switch (temp.h)
                    {
                        case 5:
                        case 10:
                        case 15:
                        case 20: break;
                        default:
                            LOG_INFO("Unsupported Merkle tree height (h) - %d\n\t   Supported heights - 5, 10, 15, 20", temp.h);
                            LmsParameterError(param.lms_param);
                    }
                    break;
                case 'w':
                    item.erase(item.begin());
                    temp.w = strtoul(item.c_str(), NULL, 10);
                    switch (temp.w)
                    {
                        case 2:
                        case 4:
                        case 8: break;
                        default:
                            LOG_INFO("Unsupported Winternitz parameter (w) - %d\n\t   Supported parameters - 2, 4, 8", temp.w);
                            LmsParameterError(param.lms_param);
                    }
                    break;
                default:
                    LmsParameterError(param.lms_param);
                    break;
            }
            break;
        }
    }
    LOG_TRACE("Parsed LMS parameters: hash=%s, height(h)=%d, Winternitz(w)=%d", temp.hashFunction?"SHAKE256":"SHA256", temp.h, temp.w);
    if (param.type == AuthKeyLevel::PRIMARY)
    {
        primaryLmsParams.push_back(temp);
    }
    if (param.type == AuthKeyLevel::SECONDARY)
    {
        secondaryLmsParams.push_back(temp);
    }
}


/******************************************************************************/
uint32_t BifOptions::GetRevokeId()
{
    return spkRevokeId;
}

/******************************************************************************/
void PartitionBifOptions::SetLockStepFlag()
{
    if (arch == Arch::ZYNQ || arch == Arch::ZYNQMP || (arch == Arch::VERSAL && !versalNetSeries))
    {
        LOG_ERROR("BIF attribute error !!!\n\t  'lockstep' is not supported with the mentioned -arch.");
    }
 
    lockstep = true;
}

/******************************************************************************/
void PartitionBifOptions::SetTcmARegion(uint64_t address)
{
    if ((arch == Arch::VERSAL && versalNetSeries) && (destCPUType == DestinationCPU::R5_0 || destCPUType == DestinationCPU::R5_1 || destCPUType == DestinationCPU::R5_lockstep))
    {
        tcmARegionAddr = address;
    }
    else
    {
        LOG_ERROR("BIF attribute error !!!\n\t  'tcm_a_region' is supported only in VersalNet architecture");
    }
}

/******************************************************************************/
void PartitionBifOptions::SetTcmBRegion(uint64_t address)
{
    if ((arch == Arch::VERSAL && versalNetSeries) && (destCPUType == DestinationCPU::R5_0 || destCPUType == DestinationCPU::R5_1 || destCPUType == DestinationCPU::R5_lockstep))
    {
        tcmBRegionAddr = address;
    }
    else
    {
        LOG_ERROR("BIF attribute error !!!\n\t  'tcm_b_region' is supported only in VersalNet architecture");
    }
}

/******************************************************************************/
void PartitionBifOptions::SetTcmCRegion(uint64_t address)
{
    if ((arch == Arch::VERSAL && versalNetSeries) && (destCPUType == DestinationCPU::R5_0 || destCPUType == DestinationCPU::R5_1 || destCPUType == DestinationCPU::R5_lockstep))
    {
        tcmCRegionAddr = address;
    }
    else
    {
        LOG_ERROR("BIF attribute error !!!\n\t  'tcm_c_region' is supported only in VersalNet architecture");
    }
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
        LOG_ERROR("BIF attribute error !!!\n\t\t'-authblock' option supported only for ZYNQMP architecture '-arch zynqmp'");
    }
    authblockattr = authBlockAttr;
}

/******************************************************************************/
void PartitionBifOptions::SetPufHdLocation(PufHdLoc::Type type)
{
    pufHdLoc = type;
}

/******************************************************************************/
void PartitionBifOptions::SetReserveLength(uint64_t length, bool flag)
{
    reserve = length;
    updateReserveInPh = flag;
    if (updateReserveInPh)
    {
        static bool warningGiven = false;
        if (!warningGiven)
        {
            LOG_WARNING("The existing reserve functionality is updated.\n\t   Please refer the section 'reserve' under 'Appendix A BIF Attribute Reference' in UG1283 for more details.");
            warningGiven = true;
        }
    }
}

/******************************************************************************/
void PartitionBifOptions::SetDelayAuth(bool flag)
{
    delayAuth = flag;
}

/******************************************************************************/
void PartitionBifOptions::SetTcmBootFlag()
{
    if (((arch == Arch::VERSAL && versalNetSeries) || (arch == Arch::VERSALGEN2) || (arch == Arch::VERSAL_2VP)) && (destCPUType == DestinationCPU::R5_0 || destCPUType == DestinationCPU::R5_1 || destCPUType == DestinationCPU::R5_lockstep))
    {
        tcmBoot = true;
    }
    else
    {
        LOG_ERROR("BIF attribute error !!!\n\t  'tcmboot' is supported only in VersalNet architecture");
    }
}

/******************************************************************************/
uint32_t PartitionBifOptions::GetDefaultEncryptionBlockSize(void)
{
    return defBlockSize;
}

/******************************************************************************/
KeySource::Type PartitionBifOptions::GetEncryptionKeySource(void)
{
    return keySrc;
}

/******************************************************************************/
DpaCM::Type PartitionBifOptions::GetDpaCM(void)
{
    return dpaCM;
}

/******************************************************************************/
uint32_t PartitionBifOptions::GetSPKRevokeId(void)
{
    return spkRevokeId;
}

/******************************************************************************/
uint32_t PartitionBifOptions::GetPartitionRevokeId(void)
{
    return partitionRevokeId;
}

/******************************************************************************/
PufHdLoc::Type PartitionBifOptions::GetPufHdLocation(void)
{
    return pufHdLoc;
}

/******************************************************************************/
std::string PartitionBifOptions::GetOutputFileFromBifSection(std::string out_file, std::string bif_section, PartitionType::Type part_type)
{
    std::string filename = StringUtils::RemoveExtension(out_file) + "_" + bif_section + StringUtils::GetExtension(out_file);
    if (part_type == PartitionType::SLR_BOOT || part_type == PartitionType::SLR_CONFIG)
    {
        filename = StringUtils::RemoveExtension(out_file) + "_" + bif_section + ".bin";
    }
    return filename;
}

/******************************************************************************/
void BifOptions::SetTotalPmcFwSize(uint32_t size)
{
    totalpmcdataSize = size;
}

/******************************************************************************/
void BifOptions::SetPmcFwSize(uint32_t size)
{
    pmcdataSize = size;
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
    if (arch == Arch::SPARTANUP && id > 95)
    {
        LOG_ERROR("spk_id can only take values from 0x0 to 0x5F for spartanup.");
    }
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
void BifOptions::SetSmapWidth(uint32_t value)
{
    smapWidth = value;
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
uint32_t BifOptions::GetBootDeviceAddress(void)
{
    return bootDeviceAddress;
}

/******************************************************************************/
uint32_t BifOptions::GetSmapWidth(void)
{
    return smapWidth;
}

/******************************************************************************/
uint32_t BifOptions::GetPmcCdoLoadAddress(void)
{
    return pmcCdoLoadAddress;
}

/******************************************************************************/
uint32_t BifOptions::GetPmcFwSize(void)
{
    return pmcdataSize;
}

/******************************************************************************/
uint32_t BifOptions::GetTotalPmcFwSize(void)
{
    return totalpmcdataSize;
}

/******************************************************************************/
uint32_t BifOptions::GetPdiId(void)
{
    return pdiId;
}

/******************************************************************************/
uint32_t BifOptions::GetParentId(void)
{
    return parentId;
}

/******************************************************************************/
KeySource::Type BifOptions::GetEncryptionKeySource(void)
{
    return keySourceEncryption;
}

/******************************************************************************/
bool BifOptions::GetPufHdinBHFlag(void)
{
    return pufHdinBHEnable;
}

/******************************************************************************/
uint8_t* BifOptions::GetPmcDataBuffer(void)
{
    return pmcDataBuffer;
}

/******************************************************************************/
uint32_t BifOptions::GetTotalpmcdataSize(void)
{
    return totalpmcdataSize;
}

/******************************************************************************/
std::string BifOptions::GetPmcdataFile(void)
{
    return pmcdataFile;
}

/******************************************************************************/
std::vector<std::string> BifOptions::GetPmcCdoFileList(void)
{
    return pmcCdoFileList;
}

/******************************************************************************/
std::vector<LmsKeyParam> BifOptions::GetPrimaryLmsParams(void)
{
    return primaryLmsParams;
}

/******************************************************************************/
std::vector<LmsKeyParam> BifOptions::GetSecondaryLmsParams(void)
{
    return secondaryLmsParams;
}

/******************************************************************************/
void BifOptions::SetPmcDataAesFile(std::string filename)
{
    pmcDataAesFile = filename;
}

/******************************************************************************/
std::string BifOptions::GetPmcDataAesFile(void)
{
    return pmcDataAesFile;
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
    if (arch == Arch::VERSAL || arch == Arch::VERSALGEN2 || arch == Arch::SPARTANUP || arch == Arch::VERSAL_2VP)
    {
        createHeaderAC = false;
        if (metaHdrAttributes.authenticate != Authentication::None)
            createHeaderAC = true;
    }
    return createHeaderAC;
}

/******************************************************************************/
bool BifOptions::GetHeaderEncyption()
{
    if (arch == Arch::VERSAL || arch == Arch::VERSALGEN2 || arch == Arch::SPARTANUP || arch == Arch::VERSAL_2VP)
    {
        doHeaderEncryption = false;
        if (metaHdrAttributes.encrypt != Encryption::None)
            doHeaderEncryption = true;
    }
    return doHeaderEncryption;
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
uint32_t BifOptions::GetPufRingOscilltorSwapConfigValue(void)
{
    return pufRoSwapConfigVal;
}

/******************************************************************************/
DICE::Type BifOptions::GetDice(void)
{
    return dice;
}

/******************************************************************************/
DpaCM::Type BifOptions::GetDpaCM(void)
{
    return dpaCM;
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
std::string BifOptions::GetBHKekIVFile(void)
{
    return bhKekIVFile;
}

/******************************************************************************/
std::string BifOptions::GetBbramKekIVFile(void)
{
    return bbramKekIVFile;
}

/******************************************************************************/
std::string BifOptions::GetEfuseKekIVFile(void)
{
    return efuseKekIVFile;
}

/******************************************************************************/
std::string BifOptions::GetEfuseUserKek0IVFile(void)
{
    return efuseUserKek0IVFile;
}

/******************************************************************************/
std::string BifOptions::GetEfuseUserKek1IVFile(void)
{
    return efuseUserKek1IVFile;
}

/******************************************************************************/
std::string BifOptions::GetUserKeysFileName(void)
{
    return userKeyFile;
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
void BifOptions::SetPdiId(uint32_t id)
{
    pdiId = id;
}

/******************************************************************************/
void BifOptions::SetIdCode(uint32_t id)
{
    idCode = id;
}

/******************************************************************************/
void BifOptions::SetExtendedIdCode(uint32_t id)
{
    if (id > 0x3F)
    {
        LOG_ERROR("Invalid extended ID code. Maximum length of extended_id_code is 6-bits");
    }
    extendedIdCode = id;
}

/******************************************************************************/
void BifOptions::SetPdiType(PartitionType::Type type)
{
    pdiType = type;
}

/******************************************************************************/
void BifOptions::SetBypassIdcodeFlag(bool flag)
{
    bypassIdCode = flag;
}

/******************************************************************************/
void BifOptions::SetAHwRoTFlag(bool flag)
{
    aHwrot = flag;
}

/******************************************************************************/
void BifOptions::SetSHwRoTFlag(bool flag)
{
    sHwrot = flag;
}

/******************************************************************************/
void BifOptions::SetParentId(uint32_t id)
{
    parentId = id;
}

/******************************************************************************/
void BifOptions::SetBhRsa(BhRsa::Type value)
{
    if ((arch == Arch::VERSAL && versalNetSeries) || arch == Arch::VERSALGEN2 || arch == Arch::SPARTANUP || arch == Arch::VERSAL_2VP)
        LOG_ERROR("BIF attribute error !!! 'bh_auth_enable' is not supported with the mentioned '-arch'.\n\t   Bootheader or eFuse authentication will be chosen based on eFuse bits.");
    
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

/******************************************************************************/
void BifOptions::SetDpaCM(DpaCM::Type value)
{
    LOG_WARNING("boot_config { dpacm_enable } will be deprecated. 'dpacm_enable' should be specified along with the partition. \n            See 'bootgen -bif_help dpacm_enable' for more info.");
    dpaCM = value;
}

/******************************************************************************/
uint32_t BifOptions::GetIdCode(void)
{
    return idCode;
}

/******************************************************************************/
uint32_t BifOptions::GetExtendedIdCode(void)
{
    return extendedIdCode;
}

/******************************************************************************/
bool BifOptions::GetBypassIdcodeFlag(void)
{
    return bypassIdCode;
}

/******************************************************************************/
void PartitionBifOptions::SetAesKeyFile(std::string filename)
{
    aesKeyFile = filename;
}

/******************************************************************************/
void PartitionBifOptions::SetUdfDataFile(std::string filename)
{
    if (arch == Arch::VERSAL)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t'udf_data' is not supported in VERSAL architecture");
    }
    udfDataFile = filename;
}

/******************************************************************************/
std::string PartitionBifOptions::GetUdfDataFile(void)
{
    return udfDataFile;
}

/******************************************************************************/
void ImageBifOptions::SetDelayHandoff(bool flag)
{
    if (!delay_handoff_warning_given)
    {
        delay_handoff_warning_given = true;
        LOG_WARNING("delay_handoff is specified, this may cause some issues if not handled properly");
    }
    delayHandoff = flag;
}

/******************************************************************************/
void ImageBifOptions::SetDelayLoad(bool flag)
{
    if (!delay_load_warning_given)
    {
        delay_load_warning_given = true;
        LOG_WARNING("delay_load is specified, this may cause some issues if not handled properly");
    }
    delayLoad = flag;
}

/******************************************************************************/
void BifOptions::SetPPKFileName1(std::string filename)
{
    std::ifstream f(filename.c_str());
    ppkFile1 = filename;
}

/******************************************************************************/
void BifOptions::SetPSKFileName1(std::string filename)
{
    std::ifstream f(filename.c_str());
    pskFile1 = filename;
}

/******************************************************************************/
void BifOptions::SetSPKFileName1(std::string filename)
{
    std::ifstream f(filename.c_str());
    spkFile1 = filename;
}

/******************************************************************************/
void BifOptions::SetSSKFileName1(std::string filename)
{
    std::ifstream f(filename.c_str());
    sskFile1 = filename;
}

/******************************************************************************/
std::string BifOptions::GetPPKFileName1(void)
{
    return ppkFile1;
}

/******************************************************************************/
std::string BifOptions::GetPSKFileName1(void)
{
    return pskFile1;
}

/******************************************************************************/
std::string BifOptions::GetSPKFileName1(void)
{
    return spkFile1;
}

/******************************************************************************/
std::string BifOptions::GetSSKFileName1(void)
{
    return sskFile1;
}

/******************************************************************************/
void BifOptions::SetPKFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    pkFile = filename;
}

/******************************************************************************/
void BifOptions::SetSKFileName(std::string filename)
{
    std::ifstream f(filename.c_str());
    skFile = filename;
}

/******************************************************************************/
void BifOptions::SetPKFileName1(std::string filename)
{
    std::ifstream f(filename.c_str());
    pkFile1 = filename;
}

/******************************************************************************/
void BifOptions::SetSKFileName1(std::string filename)
{
    std::ifstream f(filename.c_str());
    skFile1 = filename;
}

/******************************************************************************/
std::string BifOptions::GetPKFileName(void)
{
    return pkFile;
}

/******************************************************************************/
std::string BifOptions::GetSKFileName(void)
{
    return skFile;
}

/******************************************************************************/
std::string BifOptions::GetPKFileName1(void)
{
    return pkFile1;
}

/******************************************************************************/
std::string BifOptions::GetSKFileName1(void)
{
    return skFile1;
}

/******************************************************************************/
const KeyMetadata& BifOptions::GetPrimaryMetadata(void) const
{
    return primaryMetadata;
}

/******************************************************************************/
const KeyMetadata& BifOptions::GetSecondaryMetadata(void) const
{
    return secondaryMetadata;
}

/******************************************************************************/
const KeyMetadata& BifOptions::GetPrimaryMetadata1(void) const
{
    return primaryMetadata1;
}

/******************************************************************************/
const KeyMetadata& BifOptions::GetSecondaryMetadata1(void) const
{
    return secondaryMetadata1;
}

/******************************************************************************/
/* Helper function to check if algorithm is Classical (RSA/ECDSA)           */
/******************************************************************************/
static bool IsClassicalAlgorithm(Authentication::Type alg)
{
    return (alg == Authentication::RSA || 
            alg == Authentication::ECDSA || 
            alg == Authentication::ECDSAp521);
}

/******************************************************************************/
/* Helper function to check if algorithm is LMS or HSS                      */
/******************************************************************************/
static bool IsLMSOrHSS(Authentication::Type alg)
{
    return (alg == Authentication::LMS_SHA2_256 || 
            alg == Authentication::LMS_SHAKE256 ||
            alg == Authentication::HSS_SHA2_256 || 
            alg == Authentication::HSS_SHAKE256);
}

/******************************************************************************/
/* Helper function to check if algorithm is MLDSA or SLH-DSA                */
/******************************************************************************/
static bool IsMLDSAOrSLH(Authentication::Type alg)
{
    return (alg == Authentication::MLDSA || 
            alg == Authentication::SLH_SHAKE256);
}

/******************************************************************************/
/* Helper function to check if file exists                                  */
/******************************************************************************/
static bool FileExistsForKeyDetection(const std::string& filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

/******************************************************************************/
/* Helper function to extract keyword from filename                         */
/* Examples:                                                                 */
/*   primary.lms.pub -> "primary"                                           */
/*   secondary.mldsa.pk -> "secondary"                                      */
/******************************************************************************/
static std::string ExtractKeyword(const std::string& filename)
{
    // Extract filename without directory
    size_t lastSlash = filename.find_last_of("/\\");
    std::string filenameOnly = (lastSlash != std::string::npos) ? 
                                filename.substr(lastSlash + 1) : filename;
    
    // Remove all extensions (handle cases like primary.lms.pub)
    size_t firstDot = filenameOnly.find_first_of(".");
    std::string keyword = (firstDot != std::string::npos) ? 
                          filenameOnly.substr(0, firstDot) : filenameOnly;
    
    return keyword;
}

/******************************************************************************/
/* Helper function to derive .kmd metadata file from key file               */
/* Examples:                                                                 */
/*   primary.lms.pub -> "primary.kmd"                                       */
/*   secondary.lms.pub -> "secondary.kmd"                                   */
/*   private_1.pem -> "private_1.kmd"                                       */
/******************************************************************************/
static std::string DeriveKmdFile(const std::string& keyFile)
{
    // Extract directory
    size_t lastSlash = keyFile.find_last_of("/\\");
    std::string dir = (lastSlash != std::string::npos) ? keyFile.substr(0, lastSlash + 1) : "";
    
    // Extract base keyword
    std::string keyword = ExtractKeyword(keyFile);
    
    // Build .kmd filename
    std::string kmdFile = dir + keyword + ".kmd";
    
    LOG_TRACE("Derived .kmd file: %s from key file: %s", kmdFile.c_str(), keyFile.c_str());
    return kmdFile;
}

/******************************************************************************/
/* Helper function to trim whitespace from string                           */
/******************************************************************************/
static std::string Trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

/******************************************************************************/
/* Convert algorithm string to device-specific header value                 */
/* Dispatches to device-specific implementation based on architecture       */
/******************************************************************************/
static uint32_t ConvertAlgorithmStringToHeaderValue(const std::string& algStr)
{
    if (DeviceSpecificKmd::ConvertAlgorithmStringToHeaderValue == NULL) {
        LOG_ERROR("Device-specific KMD functions not initialized. Call InitializeDeviceSpecificKmdFunctions() first.");
        return 0;
    }
    return DeviceSpecificKmd::ConvertAlgorithmStringToHeaderValue(algStr);
}

/******************************************************************************/
/* Convert hybrid string to device-specific bitmask                         */
/* Dispatches to device-specific implementation based on architecture       */
/******************************************************************************/
static uint32_t ConvertHybridStringToBitmask(const std::string& hybridStr)
{
    if (DeviceSpecificKmd::ConvertHybridStringToBitmask == NULL) {
        LOG_ERROR("Device-specific KMD functions not initialized. Call InitializeDeviceSpecificKmdFunctions() first.");
        return 0;
    }
    return DeviceSpecificKmd::ConvertHybridStringToBitmask(hybridStr);
}

/******************************************************************************/
/* Check if algorithm is classical (device-specific)                        */
/* Dispatches to device-specific implementation based on architecture       */
/******************************************************************************/
static bool IsClassicalAlgorithm(uint32_t algorithm)
{
    if (DeviceSpecificKmd::IsClassicalAlgorithm == NULL) {
        LOG_ERROR("Device-specific KMD functions not initialized.");
        return false;
    }
    return DeviceSpecificKmd::IsClassicalAlgorithm(algorithm);
}

/******************************************************************************/
/* Check if algorithm is PQC (device-specific)                              */
/* Dispatches to device-specific implementation based on architecture       */
/******************************************************************************/
static bool IsPQCAlgorithm(uint32_t algorithm)
{
    if (DeviceSpecificKmd::IsPQCAlgorithm == NULL) {
        LOG_ERROR("Device-specific KMD functions not initialized.");
        return false;
    }
    return DeviceSpecificKmd::IsPQCAlgorithm(algorithm);
}

/******************************************************************************/
/* Get algorithm name (device-specific)                                     */
/* Dispatches to device-specific implementation based on architecture       */
/******************************************************************************/
static std::string GetAlgorithmName(uint32_t algorithm)
{
    if (DeviceSpecificKmd::GetAlgorithmName == NULL) {
        LOG_ERROR("Device-specific KMD functions not initialized.");
        return "Unknown";
    }
    return DeviceSpecificKmd::GetAlgorithmName(algorithm);
}

/******************************************************************************/
/* Check if algorithm is in hybrid mask (device-specific)                   */
/* Dispatches to device-specific implementation based on architecture       */
/******************************************************************************/
static bool IsAlgorithmInHybridMask(uint32_t algorithm, uint32_t hybridMask)
{
    if (DeviceSpecificKmd::IsAlgorithmInHybridMask == NULL) {
        LOG_ERROR("Device-specific KMD functions not initialized.");
        return false;
    }
    return DeviceSpecificKmd::IsAlgorithmInHybridMask(algorithm, hybridMask);
}

/******************************************************************************/
/* Convert KMD algorithm to Authentication::Type (device-specific)          */
/* Dispatches to device-specific implementation based on architecture       */
/******************************************************************************/
static Authentication::Type ConvertKmdAlgorithmToAuthType(uint32_t kmdAlgorithm)
{
    if (DeviceSpecificKmd::ConvertKmdAlgorithmToAuthType == NULL) {
        LOG_ERROR("Device-specific KMD functions not initialized.");
        return Authentication::None;
    }
    return DeviceSpecificKmd::ConvertKmdAlgorithmToAuthType(kmdAlgorithm);
}

/******************************************************************************/
/* Check if hybrid mask contains classical algorithms (device-specific)     */
/* Dispatches to device-specific implementation based on architecture       */
/******************************************************************************/
static bool HybridMaskContainsClassical(uint32_t hybridMask)
{
    if (DeviceSpecificKmd::HybridMaskContainsClassical == NULL) {
        LOG_ERROR("Device-specific KMD functions not initialized.");
        return false;
    }
    return DeviceSpecificKmd::HybridMaskContainsClassical(hybridMask);
}

/******************************************************************************/
/* Check if hybrid mask contains PQC algorithms (device-specific)           */
/* Dispatches to device-specific implementation based on architecture       */
/******************************************************************************/
static bool HybridMaskContainsPQC(uint32_t hybridMask)
{
    if (DeviceSpecificKmd::HybridMaskContainsPQC == NULL) {
        LOG_ERROR("Device-specific KMD functions not initialized.");
        return false;
    }
    return DeviceSpecificKmd::HybridMaskContainsPQC(hybridMask);
}

/******************************************************************************/
/* Initialize device-specific KMD function pointers based on architecture   */
/* Must be called before any .kmd file parsing operations                   */
/******************************************************************************/
void InitializeDeviceSpecificKmdFunctions(Arch::Type arch)
{
    static bool initialized = false;
    static Arch::Type currentArch = Arch::ZYNQ;
    
    // Only reinitialize if architecture changes
    if (initialized && currentArch == arch) {
        return;
    }
    
    LOG_TRACE("Initializing device-specific KMD functions for architecture: %d", (int)arch);
    
#ifndef SKIP_VERSAL_2VP_NATIVE
    if (arch == Arch::VERSAL_2VP) {
        DeviceSpecificKmd::ConvertAlgorithmStringToHeaderValue = 
            BifOptionsVersal2VP::ConvertAlgorithmStringToHeaderValue;
        DeviceSpecificKmd::ConvertHybridStringToBitmask = 
            BifOptionsVersal2VP::ConvertHybridStringToBitmask;
        DeviceSpecificKmd::IsClassicalAlgorithm = 
            BifOptionsVersal2VP::IsClassicalAlgorithm;
        DeviceSpecificKmd::IsPQCAlgorithm = 
            BifOptionsVersal2VP::IsPQCAlgorithm;
        DeviceSpecificKmd::GetAlgorithmName = 
            BifOptionsVersal2VP::GetAlgorithmName;
        DeviceSpecificKmd::IsAlgorithmInHybridMask = 
            BifOptionsVersal2VP::IsAlgorithmInHybridMask;
        DeviceSpecificKmd::ConvertKmdAlgorithmToAuthType = 
            BifOptionsVersal2VP::ConvertKmdAlgorithmToAuthType;
        DeviceSpecificKmd::HybridMaskContainsClassical = 
            BifOptionsVersal2VP::HybridMaskContainsClassical;
        DeviceSpecificKmd::HybridMaskContainsPQC = 
            BifOptionsVersal2VP::HybridMaskContainsPQC;
        
        LOG_INFO("Loaded Versal 2VP device-specific KMD functions");
    }
    else
#endif
    if (arch == Arch::VERSAL || arch == Arch::VERSALGEN2 || arch == Arch::SPARTANUP) {
        // Future: Add device-specific implementations for other architectures
        // For now, set to NULL (will error only if .kmd parsing is actually attempted)
        LOG_TRACE("Architecture '%s' does not have device-specific KMD functions implemented yet", Arch::ToString(arch));
        DeviceSpecificKmd::ConvertAlgorithmStringToHeaderValue = NULL;
        DeviceSpecificKmd::ConvertHybridStringToBitmask = NULL;
        DeviceSpecificKmd::IsClassicalAlgorithm = NULL;
        DeviceSpecificKmd::IsPQCAlgorithm = NULL;
        DeviceSpecificKmd::GetAlgorithmName = NULL;
        DeviceSpecificKmd::IsAlgorithmInHybridMask = NULL;
        DeviceSpecificKmd::ConvertKmdAlgorithmToAuthType = NULL;
        DeviceSpecificKmd::HybridMaskContainsClassical = NULL;
        DeviceSpecificKmd::HybridMaskContainsPQC = NULL;
    }
    else {
        // ZYNQ, ZYNQMP, FPGA don't use .kmd files
        LOG_TRACE("Architecture '%s' does not use .kmd files", Arch::ToString(arch));
        DeviceSpecificKmd::ConvertAlgorithmStringToHeaderValue = NULL;
        DeviceSpecificKmd::ConvertHybridStringToBitmask = NULL;
        DeviceSpecificKmd::IsClassicalAlgorithm = NULL;
        DeviceSpecificKmd::IsPQCAlgorithm = NULL;
        DeviceSpecificKmd::GetAlgorithmName = NULL;
        DeviceSpecificKmd::IsAlgorithmInHybridMask = NULL;
        DeviceSpecificKmd::ConvertKmdAlgorithmToAuthType = NULL;
        DeviceSpecificKmd::HybridMaskContainsClassical = NULL;
        DeviceSpecificKmd::HybridMaskContainsPQC = NULL;
    }
    
    initialized = true;
    currentArch = arch;
}

/******************************************************************************/
/* Helper function to parse .kmd metadata file in YAML format               */
/* File format (YAML key-value pairs):                                      */
/*   algorithm: hss-shake256                                                */
/*   revoke_id: 0x0                                                         */
/*   permission: 0x0                                                        */
/*   Authority: 0x0                                                         */
/*   hybrid: ecdsa-p384,rsa                                                 */
/*                                                                           */
/* Features:                                                                */
/*   - YAML format: key: value (colon separator)                           */
/*   - Case-insensitive keys (Authority/authority both work)               */
/*   - Flexible whitespace (spaces/tabs around colon)                      */
/*   - Comment support (lines starting with #)                             */
/*   - Empty line support                                                   */
/*   - Hex (0x0) or decimal (0) numeric values                             */
/*                                                                           */
/* Note: Converts string values to match HeaderAuthCert_versal_2vp format  */
/******************************************************************************/
static bool ParseKmdFile(const std::string& kmdFile, KeyMetadata& metadata)
{
    LOG_TRACE("Parsing .kmd file: %s", kmdFile.c_str());
    
    // Check if file exists
    if (!FileExistsForKeyDetection(kmdFile)) {
        LOG_ERROR(".kmd metadata file not found: %s", kmdFile.c_str());
        LOG_ERROR("Please provide .kmd files for all key files");
        return false;
    }
    
    // Open file
    std::ifstream file(kmdFile.c_str());
    if (!file.is_open()) {
        LOG_ERROR("Failed to open .kmd file: %s", kmdFile.c_str());
        return false;
    }
    
    // Temporary storage for string values before conversion
    std::string algorithmStr;
    std::string hybridStr;
    
    // Parse line by line
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = Trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Find colon separator
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            LOG_WARNING(".kmd file %s line %d: Invalid format (missing ':'), skipping", 
                       kmdFile.c_str(), lineNumber);
            continue;
        }
        
        // Extract key and value
        std::string key = Trim(line.substr(0, colonPos));
        std::string value = Trim(line.substr(colonPos + 1));
        
        // Convert key to lowercase for case-insensitive comparison
        std::string keyLower = key;
        std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);
        
        // Parse based on key
        if (keyLower == "algorithm") {
            algorithmStr = value;  // Store for conversion later
            LOG_TRACE("  algorithm string = %s", value.c_str());
        }
        else if (keyLower == "revoke_id" || keyLower == "revocation_id" || keyLower == "revocationid") {
            // Parse hex or decimal - maps to RevocationID field
            char* endptr;
            metadata.RevocationID = strtoul(value.c_str(), &endptr, 0);
            LOG_TRACE("  RevocationID = 0x%x", metadata.RevocationID);
        }
        else if (keyLower == "permission") {
            // Parse hex or decimal - maps to Permission field
            metadata.Permission = strtoul(value.c_str(), NULL, 0);
            LOG_TRACE("  Permission = 0x%x", metadata.Permission);
        }
        else if (keyLower == "authority") {
            // Parse hex or decimal - maps to Authority field
            metadata.Authority = strtoul(value.c_str(), NULL, 0);
            LOG_TRACE("  Authority = 0x%x", metadata.Authority);
        }
        else if (keyLower == "hybrid") {
            hybridStr = value;  // Store for conversion later
            LOG_TRACE("  hybrid string = %s", value.c_str());
        }
        else {
            LOG_WARNING(".kmd file %s line %d: Unknown key '%s', skipping", 
                       kmdFile.c_str(), lineNumber, key.c_str());
        }
    }
    
    file.close();
    
    // Convert algorithm string to AUTH_HDR_TELLURIDE header value
    if (!algorithmStr.empty()) {
        uint32_t headerValue = ConvertAlgorithmStringToHeaderValue(algorithmStr);
        // Algorithm field: [0:7] = AUTH_HDR_TELLURIDE value, [8:15] = reserved
        metadata.Algorithm = headerValue;  // Store directly (already in bits [0:7])
        LOG_TRACE("  Converted Algorithm = 0x%x (AUTH_HDR_TELLURIDE value)", metadata.Algorithm);
    }
    
    // Convert hybrid string to bitmask using HYBRID_* values
    if (!hybridStr.empty()) {
        metadata.Hybrid = ConvertHybridStringToBitmask(hybridStr);
        LOG_TRACE("  Converted Hybrid bitmask = 0x%x", metadata.Hybrid);
    }
    
    // Mark as valid
    metadata.isValid = true;
    LOG_INFO("Successfully parsed .kmd file: %s", kmdFile.c_str());
    LOG_INFO("  Algorithm=0x%x, RevocationID=0x%x, Hybrid=0x%x, Authority=0x%x, Permission=0x%x", 
             metadata.Algorithm, metadata.RevocationID, metadata.Hybrid, metadata.Authority, metadata.Permission);
    
    return true;
}

/******************************************************************************/
/* NOTE: Device-specific helper functions (Is Classical Algorithm, IsPQCAlgorithm, etc.)
 * have been moved to device-specific files (e.g., versal_2vp/src/bifoptions-versal_2vp.cpp)
 * and are accessed via function pointers initialized by InitializeDeviceSpecificKmdFunctions().
 * This ensures common code has no device-specific header dependencies.
 */

/******************************************************************************/
/* Helper function to validate .kmd metadata files according to rules:      */
/*   1. Primary and Secondary algorithms must match (single and hybrid)     */
/*   2. Algorithm 1 must be in Algorithm 2's hybrid field and vice versa    */
/*   3. Only Classical + PQC combinations allowed                           */
/*   4. Compute final hybrid mask for both algorithms                       */
/******************************************************************************/
static bool ValidateKmdMetadata(
    const KeyMetadata& primary,      // PPK metadata for Algorithm 1
    const KeyMetadata& secondary,    // SPK metadata for Algorithm 1
    const KeyMetadata& primary1,     // PPK metadata for Algorithm 2 (hybrid only)
    const KeyMetadata& secondary1,   // SPK metadata for Algorithm 2 (hybrid only)
    bool isHybrid,
    const std::string& primaryKmdFile = "PPK.kmd",
    const std::string& secondaryKmdFile = "SPK.kmd",
    const std::string& primary1KmdFile = "PPK1.kmd",
    const std::string& secondary1KmdFile = "SPK1.kmd")
{
    LOG_INFO("=== Validating .kmd metadata files ===");
    
    // RULE 1: Primary and Secondary algorithms must match for Algorithm 1
    if (primary.Algorithm != secondary.Algorithm) {
        LOG_WARNING("Algorithm mismatch between PPK and SPK .kmd files for Algorithm 1");
        LOG_WARNING("  %s algorithm: %s (0x%x)", 
                  primaryKmdFile.c_str(),
                  GetAlgorithmName(primary.Algorithm).c_str(), primary.Algorithm);
        LOG_WARNING("  %s algorithm: %s (0x%x)", 
                  secondaryKmdFile.c_str(),
                  GetAlgorithmName(secondary.Algorithm).c_str(), secondary.Algorithm);
        LOG_ERROR("Both PPK and SPK metadata files for Algorithm 1 must specify the same algorithm");
        return false;
    }
    LOG_INFO("✓ Rule 1: Algorithm 1 consistency check passed");
    LOG_INFO("  Primary/Secondary algorithm: %s (0x%x)", 
             GetAlgorithmName(primary.Algorithm).c_str(), primary.Algorithm);
    
    if (!isHybrid) {
        LOG_INFO("✓ Single algorithm mode - validation complete");
        return true;
    }
    
    // === HYBRID MODE VALIDATION ===
    LOG_INFO("Hybrid mode detected - performing additional validation...");
    
    // RULE 1 (continued): Primary1 and Secondary1 algorithms must match for Algorithm 2
    if (primary1.Algorithm != secondary1.Algorithm) {
        LOG_WARNING("Algorithm mismatch between PPK and SPK .kmd files for Algorithm 2");
        LOG_WARNING("  %s algorithm: %s (0x%x)", 
                  primary1KmdFile.c_str(),
                  GetAlgorithmName(primary1.Algorithm).c_str(), primary1.Algorithm);
        LOG_WARNING("  %s algorithm: %s (0x%x)", 
                  secondary1KmdFile.c_str(),
                  GetAlgorithmName(secondary1.Algorithm).c_str(), secondary1.Algorithm);
        LOG_ERROR("Both PPK and SPK metadata files for Algorithm 2 must specify the same algorithm");
        return false;
    }
    LOG_INFO("✓ Rule 1: Algorithm 2 consistency check passed");
    LOG_INFO("  Primary1/Secondary1 algorithm: %s (0x%x)", 
             GetAlgorithmName(primary1.Algorithm).c_str(), primary1.Algorithm);
    
    // RULE 2: Cross-validation of algorithms in hybrid fields
    // All 4 .kmd files must have consistent hybrid masks that include both algorithms
    
    // 2a. Check if Algorithm 1 is present in Algorithm 2's PPK hybrid field (primary1)
    if (!IsAlgorithmInHybridMask(primary.Algorithm, primary1.Hybrid)) {
        LOG_WARNING("Algorithm 1 (%s) not found in Algorithm 2's PPK hybrid field", 
                  GetAlgorithmName(primary.Algorithm).c_str());
        LOG_WARNING("  %s specifies algorithm: %s (0x%x)", 
                  primaryKmdFile.c_str(),
                  GetAlgorithmName(primary.Algorithm).c_str(), primary.Algorithm);
        LOG_WARNING("  %s hybrid mask: 0x%x (does not include Algorithm 1)", 
                  primary1KmdFile.c_str(), primary1.Hybrid);
        LOG_ERROR("Algorithm 1 must be listed in Algorithm 2's 'hybrid' field in %s", 
                  primary1KmdFile.c_str());
        return false;
    }
    
    // 2b. Check if Algorithm 1 is present in Algorithm 2's SPK hybrid field (secondary1)
    if (!IsAlgorithmInHybridMask(secondary.Algorithm, secondary1.Hybrid)) {
        LOG_WARNING("Algorithm 1 (%s) not found in Algorithm 2's SPK hybrid field", 
                  GetAlgorithmName(secondary.Algorithm).c_str());
        LOG_WARNING("  %s specifies algorithm: %s (0x%x)", 
                  secondaryKmdFile.c_str(),
                  GetAlgorithmName(secondary.Algorithm).c_str(), secondary.Algorithm);
        LOG_WARNING("  %s hybrid mask: 0x%x (does not include Algorithm 1)", 
                  secondary1KmdFile.c_str(), secondary1.Hybrid);
        LOG_ERROR("Algorithm 1 must be listed in Algorithm 2's 'hybrid' field in %s", 
                  secondary1KmdFile.c_str());
        return false;
    }
    
    // 2c. Check if Algorithm 2 is present in Algorithm 1's PPK hybrid field (primary)
    if (!IsAlgorithmInHybridMask(primary1.Algorithm, primary.Hybrid)) {
        LOG_WARNING("Algorithm 2 (%s) not found in Algorithm 1's PPK hybrid field", 
                  GetAlgorithmName(primary1.Algorithm).c_str());
        LOG_WARNING("  %s specifies algorithm: %s (0x%x)", 
                  primary1KmdFile.c_str(),
                  GetAlgorithmName(primary1.Algorithm).c_str(), primary1.Algorithm);
        LOG_WARNING("  %s hybrid mask: 0x%x (does not include Algorithm 2)", 
                  primaryKmdFile.c_str(), primary.Hybrid);
        LOG_ERROR("Algorithm 2 must be listed in Algorithm 1's 'hybrid' field in %s", 
                  primaryKmdFile.c_str());
        return false;
    }
    
    // 2d. Check if Algorithm 2 is present in Algorithm 1's SPK hybrid field (secondary)
    if (!IsAlgorithmInHybridMask(secondary1.Algorithm, secondary.Hybrid)) {
        LOG_WARNING("Algorithm 2 (%s) not found in Algorithm 1's SPK hybrid field", 
                  GetAlgorithmName(secondary1.Algorithm).c_str());
        LOG_WARNING("  %s specifies algorithm: %s (0x%x)", 
                  secondary1KmdFile.c_str(),
                  GetAlgorithmName(secondary1.Algorithm).c_str(), secondary1.Algorithm);
        LOG_WARNING("  %s hybrid mask: 0x%x (does not include Algorithm 2)", 
                  secondaryKmdFile.c_str(), secondary.Hybrid);
        LOG_ERROR("Algorithm 2 must be listed in Algorithm 1's 'hybrid' field in %s", 
                  secondaryKmdFile.c_str());
        return false;
    }
    
    LOG_INFO("✓ Rule 2: Cross-validation check passed");
    LOG_INFO("  Algorithm 1 found in all Algorithm 2 hybrid fields");
    LOG_INFO("  Algorithm 2 found in all Algorithm 1 hybrid fields");
    
    // RULE 2.5: Validate that each .kmd file's algorithm is NOT in the same category as its hybrid field
    // STRICT REQUIREMENT: No mixing allowed in hybrid field
    // - If algorithm is Classical → hybrid field must contain ONLY PQC algorithms
    // - If algorithm is PQC → hybrid field must contain ONLY Classical algorithms
    
    // Validate primary.kmd: algorithm category must be opposite of hybrid field category (no mixing)
    if (primary.Hybrid != 0) {
        bool algorithmIsClassical = IsClassicalAlgorithm(primary.Algorithm);
        bool algorithmIsPQC = IsPQCAlgorithm(primary.Algorithm);
        bool hybridHasClassical = HybridMaskContainsClassical(primary.Hybrid);
        bool hybridHasPQC = HybridMaskContainsPQC(primary.Hybrid);
        
        // If algorithm is Classical, hybrid MUST contain ONLY PQC (no Classical at all)
        if (algorithmIsClassical && hybridHasClassical) {
            LOG_WARNING("Invalid hybrid field in %s: algorithm is Classical but hybrid contains Classical algorithms", 
                      primaryKmdFile.c_str());
            LOG_WARNING("  Algorithm: %s (Classical, 0x%x)", 
                      GetAlgorithmName(primary.Algorithm).c_str(), primary.Algorithm);
            LOG_WARNING("  Hybrid mask: 0x%x (contains Classical algorithms)", primary.Hybrid);
            LOG_ERROR("If algorithm is Classical, hybrid field must contain ONLY PQC algorithms");
            return false;
        }
        
        // If algorithm is PQC, hybrid MUST contain ONLY Classical (no PQC at all)
        if (algorithmIsPQC && hybridHasPQC) {
            LOG_WARNING("Invalid hybrid field in %s: algorithm is PQC but hybrid contains PQC algorithms", 
                      primaryKmdFile.c_str());
            LOG_WARNING("  Algorithm: %s (PQC, 0x%x)", 
                      GetAlgorithmName(primary.Algorithm).c_str(), primary.Algorithm);
            LOG_WARNING("  Hybrid mask: 0x%x (contains PQC algorithms)", primary.Hybrid);
            LOG_ERROR("If algorithm is PQC, hybrid field must contain ONLY Classical algorithms");
            return false;
        }
    }
    
    // Validate secondary.kmd: algorithm category must be opposite of hybrid field category (no mixing)
    if (secondary.Hybrid != 0) {
        bool algorithmIsClassical = IsClassicalAlgorithm(secondary.Algorithm);
        bool algorithmIsPQC = IsPQCAlgorithm(secondary.Algorithm);
        bool hybridHasClassical = HybridMaskContainsClassical(secondary.Hybrid);
        bool hybridHasPQC = HybridMaskContainsPQC(secondary.Hybrid);
        
        if (algorithmIsClassical && hybridHasClassical) {
            LOG_WARNING("Invalid hybrid field in %s: algorithm is Classical but hybrid contains Classical algorithms", 
                      secondaryKmdFile.c_str());
            LOG_WARNING("  Algorithm: %s (Classical, 0x%x)", 
                      GetAlgorithmName(secondary.Algorithm).c_str(), secondary.Algorithm);
            LOG_WARNING("  Hybrid mask: 0x%x (contains Classical algorithms)", secondary.Hybrid);
            LOG_ERROR("If algorithm is Classical, hybrid field must contain ONLY PQC algorithms");
            return false;
        }
        
        if (algorithmIsPQC && hybridHasPQC) {
            LOG_WARNING("Invalid hybrid field in %s: algorithm is PQC but hybrid contains PQC algorithms", 
                      secondaryKmdFile.c_str());
            LOG_WARNING("  Algorithm: %s (PQC, 0x%x)", 
                      GetAlgorithmName(secondary.Algorithm).c_str(), secondary.Algorithm);
            LOG_WARNING("  Hybrid mask: 0x%x (contains PQC algorithms)", secondary.Hybrid);
            LOG_ERROR("If algorithm is PQC, hybrid field must contain ONLY Classical algorithms");
            return false;
        }
    }
    
    // Validate private_1.kmd: algorithm category must be opposite of hybrid field category (no mixing)
    if (primary1.Hybrid != 0) {
        bool algorithmIsClassical = IsClassicalAlgorithm(primary1.Algorithm);
        bool algorithmIsPQC = IsPQCAlgorithm(primary1.Algorithm);
        bool hybridHasClassical = HybridMaskContainsClassical(primary1.Hybrid);
        bool hybridHasPQC = HybridMaskContainsPQC(primary1.Hybrid);
        
        if (algorithmIsClassical && hybridHasClassical) {
            LOG_WARNING("Invalid hybrid field in %s: algorithm is Classical but hybrid contains Classical algorithms", 
                      primary1KmdFile.c_str());
            LOG_WARNING("  Algorithm: %s (Classical, 0x%x)", 
                      GetAlgorithmName(primary1.Algorithm).c_str(), primary1.Algorithm);
            LOG_WARNING("  Hybrid mask: 0x%x (contains Classical algorithms)", primary1.Hybrid);
            LOG_ERROR("If algorithm is Classical, hybrid field must contain ONLY PQC algorithms");
            return false;
        }
        
        if (algorithmIsPQC && hybridHasPQC) {
            LOG_WARNING("Invalid hybrid field in %s: algorithm is PQC but hybrid contains PQC algorithms", 
                      primary1KmdFile.c_str());
            LOG_WARNING("  Algorithm: %s (PQC, 0x%x)", 
                      GetAlgorithmName(primary1.Algorithm).c_str(), primary1.Algorithm);
            LOG_WARNING("  Hybrid mask: 0x%x (contains PQC algorithms)", primary1.Hybrid);
            LOG_ERROR("If algorithm is PQC, hybrid field must contain ONLY Classical algorithms");
            return false;
        }
    }
    
    // Validate private_2.kmd: algorithm category must be opposite of hybrid field category (no mixing)
    if (secondary1.Hybrid != 0) {
        bool algorithmIsClassical = IsClassicalAlgorithm(secondary1.Algorithm);
        bool algorithmIsPQC = IsPQCAlgorithm(secondary1.Algorithm);
        bool hybridHasClassical = HybridMaskContainsClassical(secondary1.Hybrid);
        bool hybridHasPQC = HybridMaskContainsPQC(secondary1.Hybrid);
        
        if (algorithmIsClassical && hybridHasClassical) {
            LOG_WARNING("Invalid hybrid field in %s: algorithm is Classical but hybrid contains Classical algorithms", 
                      secondary1KmdFile.c_str());
            LOG_WARNING("  Algorithm: %s (Classical, 0x%x)", 
                      GetAlgorithmName(secondary1.Algorithm).c_str(), secondary1.Algorithm);
            LOG_WARNING("  Hybrid mask: 0x%x (contains Classical algorithms)", secondary1.Hybrid);
            LOG_ERROR("If algorithm is Classical, hybrid field must contain ONLY PQC algorithms");
            return false;
        }
        
        if (algorithmIsPQC && hybridHasPQC) {
            LOG_WARNING("Invalid hybrid field in %s: algorithm is PQC but hybrid contains PQC algorithms", 
                      secondary1KmdFile.c_str());
            LOG_WARNING("  Algorithm: %s (PQC, 0x%x)", 
                      GetAlgorithmName(secondary1.Algorithm).c_str(), secondary1.Algorithm);
            LOG_WARNING("  Hybrid mask: 0x%x (contains PQC algorithms)", secondary1.Hybrid);
            LOG_ERROR("If algorithm is PQC, hybrid field must contain ONLY Classical algorithms");
            return false;
        }
    }
    
    LOG_INFO("✓ Rule 2.5: Algorithm-Hybrid category validation passed");
    LOG_INFO("  All .kmd files have algorithm in opposite category from hybrid field (strict separation)");
    
    // RULE 3: Only Classical + PQC combinations allowed
    bool alg1IsClassical = IsClassicalAlgorithm(primary.Algorithm);
    bool alg1IsPQC = IsPQCAlgorithm(primary.Algorithm);
    bool alg2IsClassical = IsClassicalAlgorithm(primary1.Algorithm);
    bool alg2IsPQC = IsPQCAlgorithm(primary1.Algorithm);
    
    // Check for invalid combinations
    if (alg1IsClassical && alg2IsClassical) {
        LOG_WARNING("Invalid hybrid combination: Classical + Classical");
        LOG_WARNING("  %s: %s (Classical)", 
                  primaryKmdFile.c_str(), GetAlgorithmName(primary.Algorithm).c_str());
        LOG_WARNING("  %s: %s (Classical)", 
                  primary1KmdFile.c_str(), GetAlgorithmName(primary1.Algorithm).c_str());
        LOG_ERROR("Hybrid signing requires one Classical algorithm and one PQC algorithm");
        return false;
    }
    
    if (alg1IsPQC && alg2IsPQC) {
        LOG_WARNING("Invalid hybrid combination: PQC + PQC");
        LOG_WARNING("  %s: %s (PQC)", 
                  primaryKmdFile.c_str(), GetAlgorithmName(primary.Algorithm).c_str());
        LOG_WARNING("  %s: %s (PQC)", 
                  primary1KmdFile.c_str(), GetAlgorithmName(primary1.Algorithm).c_str());
        LOG_ERROR("Hybrid signing requires one Classical algorithm and one PQC algorithm");
        return false;
    }
    
    LOG_INFO("✓ Rule 3: Classical + PQC combination check passed");
    if (alg1IsClassical) {
        LOG_INFO("  Algorithm 1: %s (Classical)", GetAlgorithmName(primary.Algorithm).c_str());
        LOG_INFO("  Algorithm 2: %s (PQC)", GetAlgorithmName(primary1.Algorithm).c_str());
    } else {
        LOG_INFO("  Algorithm 1: %s (PQC)", GetAlgorithmName(primary.Algorithm).c_str());
        LOG_INFO("  Algorithm 2: %s (Classical)", GetAlgorithmName(primary1.Algorithm).c_str());
    }
    
    // RULE 4: Compute final hybrid mask by OR-ing the hybrid fields
    // This will be used later when populating PPK/SPK headers
    LOG_INFO("✓ Rule 4: Final hybrid mask computation");
    uint32_t finalHybridMask = primary.Hybrid | primary1.Hybrid;
    LOG_INFO("  Algorithm 1 hybrid mask: 0x%08x", primary.Hybrid);
    LOG_INFO("  Algorithm 2 hybrid mask: 0x%08x", primary1.Hybrid);
    LOG_INFO("  Final combined hybrid mask: 0x%08x (will be used in both PPK/SPK headers)", 
             finalHybridMask);
    
    LOG_INFO("=== All validation rules passed! ===");
    LOG_INFO("Final hybrid algorithm pair: %s + %s", 
             GetAlgorithmName(primary.Algorithm).c_str(),
             GetAlgorithmName(primary1.Algorithm).c_str());
    
    return true;
}

/******************************************************************************/
/* Helper function to derive key file based on algorithm type               */
/* Uses the ACTUAL algorithm from authentication attribute, not guessing!   */
/*                                                                           */
/* Strategy: Replace the extension while preserving the base filename       */
/* For RSA/ECDSA (.pem):  baseFile is used as-is for both PPK and PSK      */
/* For LMS/HSS:           Replace extension: .pub -> PPK, .prv -> PSK       */
/* For MLDSA/SLH-DSA:     Replace extension: .pk -> PPK, .sk -> PSK        */
/******************************************************************************/
static std::string DeriveKeyFileForAlgorithm(const std::string& baseFile, 
                                              const std::string& keyType,
                                              Authentication::Type algorithm)
{
    // Extract directory
    size_t lastSlash = baseFile.find_last_of("/\\");
    std::string dir = (lastSlash != std::string::npos) ? baseFile.substr(0, lastSlash + 1) : "";
    
    // Extract filename without directory
    std::string filenameOnly = (lastSlash != std::string::npos) ? 
                                baseFile.substr(lastSlash + 1) : baseFile;
    
    std::string derivedFile;
    
    // Based on algorithm type, determine the correct extension pattern
    if (IsClassicalAlgorithm(algorithm)) {
        // RSA/ECDSA: .pem files contain both public and private keys
        // Use the same file for PPK and PSK
        derivedFile = baseFile;
        LOG_TRACE("Classical (RSA/ECDSA) derivation: keyType=%s, algorithm=%d -> %s (same file)", 
                  keyType.c_str(), algorithm, derivedFile.c_str());
    }
    else if (IsLMSOrHSS(algorithm)) {
        // LMS/HSS: Separate .pub (public) and .prv (private) files
        // Strategy: Replace the last extension with appropriate one
        // Example: primary.lms.pub -> primary.lms.prv (for PSK)
        //          mykey.pub -> mykey.prv (for PSK)
        
        size_t lastDot = filenameOnly.find_last_of(".");
        if (lastDot == std::string::npos) {
            // No extension found - add one
            std::string baseOnly = filenameOnly;
            if (keyType == "ppk" || keyType == "spk") {
                derivedFile = dir + baseOnly + ".pub";
            } else { // psk or ssk
                derivedFile = dir + baseOnly + ".prv";
            }
        } else {
            // Has extension - replace it
            std::string baseOnly = filenameOnly.substr(0, lastDot);
            if (keyType == "ppk" || keyType == "spk") {
                // Public key - replace with .pub
                derivedFile = dir + baseOnly + ".pub";
            } else { // psk or ssk
                // Private key - replace with .prv
                derivedFile = dir + baseOnly + ".prv";
            }
        }
        LOG_TRACE("LMS/HSS derivation: keyType=%s, algorithm=%d, base=%s -> %s", 
                  keyType.c_str(), algorithm, baseFile.c_str(), derivedFile.c_str());
    }
    else if (IsMLDSAOrSLH(algorithm)) {
        // MLDSA/SLH-DSA: Separate .pk (public) and .sk (signing) files
        // Strategy: Replace the last extension with appropriate one
        // Example: primary.mldsa.pk -> primary.mldsa.sk (for PSK)
        //          mykey.pk -> mykey.sk (for PSK)
        
        size_t lastDot = filenameOnly.find_last_of(".");
        if (lastDot == std::string::npos) {
            // No extension found - add one
            std::string baseOnly = filenameOnly;
            if (keyType == "ppk" || keyType == "spk") {
                derivedFile = dir + baseOnly + ".pk";
            } else { // psk or ssk
                derivedFile = dir + baseOnly + ".sk";
            }
        } else {
            // Has extension - replace it
            std::string baseOnly = filenameOnly.substr(0, lastDot);
            if (keyType == "ppk" || keyType == "spk") {
                // Public key - replace with .pk
                derivedFile = dir + baseOnly + ".pk";
            } else { // psk or ssk
                // Signing key - replace with .sk
                derivedFile = dir + baseOnly + ".sk";
            }
        }
        LOG_TRACE("MLDSA/SLH-DSA derivation: keyType=%s, algorithm=%d, base=%s -> %s", 
                  keyType.c_str(), algorithm, baseFile.c_str(), derivedFile.c_str());
    }
    else {
        LOG_ERROR("Unsupported algorithm type for auto-detection: %d", algorithm);
        derivedFile = baseFile; // Fallback
    }
    
    return derivedFile;
}

/******************************************************************************/
/* Function to populate PPK and PSK from pkfile using ACTUAL algorithm     */
/******************************************************************************/
static bool PopulatePrimaryKeys(const std::string& pkFile, 
                         std::string& ppk, std::string& psk,
                         Authentication::Type actualAlgorithm)
{
    LOG_TRACE("PopulatePrimaryKeys: pkfile=%s, algorithm=%d", pkFile.c_str(), actualAlgorithm);
    
    // Derive PPK and PSK based on actual algorithm type
    ppk = DeriveKeyFileForAlgorithm(pkFile, "ppk", actualAlgorithm);
    psk = DeriveKeyFileForAlgorithm(pkFile, "psk", actualAlgorithm);
    
    LOG_TRACE("Derived primary key files:");
    LOG_TRACE("  PPK: %s", ppk.c_str());
    LOG_TRACE("  PSK: %s", psk.c_str());
    
    // Verify files exist
    if (!FileExistsForKeyDetection(ppk)) {
        LOG_ERROR("Derived PPK file not found: %s", ppk.c_str());
        LOG_ERROR("Please ensure key file exists in the same directory");
        return false;
    }
    if (!FileExistsForKeyDetection(psk)) {
        LOG_ERROR("Derived PSK file not found: %s", psk.c_str());
        LOG_ERROR("Please ensure key file exists in the same directory");
        return false;
    }
    
    LOG_INFO("Successfully auto-detected primary keys from pkfile: %s", pkFile.c_str());
    LOG_INFO("  PPK: %s", ppk.c_str());
    LOG_INFO("  PSK: %s", psk.c_str());
    return true;
}

/******************************************************************************/
/* Function to populate SPK and SSK from skfile using ACTUAL algorithm     */
/******************************************************************************/
static bool PopulateSecondaryKeys(const std::string& skFile, 
                           std::string& spk, std::string& ssk,
                           Authentication::Type actualAlgorithm)
{
    LOG_TRACE("PopulateSecondaryKeys: skfile=%s, algorithm=%d", skFile.c_str(), actualAlgorithm);
    
    // Derive SPK and SSK based on actual algorithm type
    spk = DeriveKeyFileForAlgorithm(skFile, "spk", actualAlgorithm);
    ssk = DeriveKeyFileForAlgorithm(skFile, "ssk", actualAlgorithm);
    
    LOG_TRACE("Derived secondary key files:");
    LOG_TRACE("  SPK: %s", spk.c_str());
    LOG_TRACE("  SSK: %s", ssk.c_str());
    
    // Verify files exist
    if (!FileExistsForKeyDetection(spk)) {
        LOG_ERROR("Derived SPK file not found: %s", spk.c_str());
        LOG_ERROR("Please ensure key file exists in the same directory");
        return false;
    }
    if (!FileExistsForKeyDetection(ssk)) {
        LOG_ERROR("Derived SSK file not found: %s", ssk.c_str());
        LOG_ERROR("Please ensure key file exists in the same directory");
        return false;
    }
    
    LOG_INFO("Successfully auto-detected secondary keys from skfile: %s", skFile.c_str());
    LOG_INFO("  SPK: %s", spk.c_str());
    LOG_INFO("  SSK: %s", ssk.c_str());
    return true;
}

/******************************************************************************/
/* Partition-level auto-detection function                                   */
/* USES AUTHENTICATION TYPE FROM BIF, NOT FILE EXTENSION!                   */
/* Supports inheritance: Falls back to global keys if partition keys not set */
/******************************************************************************/
void PartitionBifOptions::AutoDetectAndPopulateKeys()
{
    LOG_TRACE("AutoDetectAndPopulateKeys: Partition-level auto-detection");
    
    // Initialize device-specific KMD functions for this architecture
    // This must be called before any .kmd file parsing operations
    InitializeDeviceSpecificKmdFunctions(arch);
    
    // Check if user provided simplified syntax at partition level
    bool hasPartitionSimplifiedSyntax = !pkFile.empty() || !skFile.empty() || !pkFile1.empty() || !skFile1.empty();
    bool hasPartitionExplicitSyntax = !ppkFile.empty() || !pskFile.empty() || !spkFile.empty() || !sskFile.empty() ||
                                      !ppkFile1.empty() || !pskFile1.empty() || !spkFile1.empty() || !sskFile1.empty();
    
    // RULE 1: Cannot mix simplified and explicit syntax at partition level
    if (hasPartitionSimplifiedSyntax && hasPartitionExplicitSyntax) {
        LOG_ERROR("Cannot mix simplified syntax (pkfile/skfile) with explicit syntax (ppkfile/pskfile/spkfile/sskfile)");
        LOG_ERROR("Please use either simplified OR explicit syntax, not both");
        return;
    }
    
    // If no partition-level keys specified, check if global keys exist
    if (!hasPartitionSimplifiedSyntax && !hasPartitionExplicitSyntax) {
        // Check for global-level simplified syntax
        // Note: Global keys are managed by BifOptions parent class
        // We'll just skip auto-detection here and let the system use global keys
        LOG_TRACE("No partition-level keys specified, will inherit from global level if available");
        return;
    }
    
    // If explicit syntax is used at partition level, still validate .kmd files
    if (hasPartitionExplicitSyntax) {
        LOG_TRACE("Explicit syntax detected at partition level, validating .kmd files");
        
        // Parse .kmd files for validation (even in explicit syntax mode)
        KeyMetadata primaryMetadata;
        KeyMetadata secondaryMetadata;
        KeyMetadata primaryMetadata1;
        KeyMetadata secondaryMetadata1;
        
        int kmdFileCount = 0;
        
        // Store .kmd filenames for validation error messages
        std::string primaryKmdFile;
        std::string secondaryKmdFile;
        std::string primary1KmdFile;
        std::string secondary1KmdFile;
        
        // Try to find and parse .kmd files based on ppkfile/spkfile names
        if (!ppkFile.empty()) {
            primaryKmdFile = DeriveKmdFile(ppkFile);
            if (FileExistsForKeyDetection(primaryKmdFile)) {
                if (ParseKmdFile(primaryKmdFile, primaryMetadata)) {
                    kmdFileCount++;
                }
            }
        }
        
        if (!spkFile.empty()) {
            secondaryKmdFile = DeriveKmdFile(spkFile);
            if (FileExistsForKeyDetection(secondaryKmdFile)) {
                if (ParseKmdFile(secondaryKmdFile, secondaryMetadata)) {
                    kmdFileCount++;
                }
            }
        }
        
        if (!ppkFile1.empty()) {
            primary1KmdFile = DeriveKmdFile(ppkFile1);
            if (FileExistsForKeyDetection(primary1KmdFile)) {
                if (ParseKmdFile(primary1KmdFile, primaryMetadata1)) {
                    kmdFileCount++;
                }
            }
        }
        
        if (!spkFile1.empty()) {
            secondary1KmdFile = DeriveKmdFile(spkFile1);
            if (FileExistsForKeyDetection(secondary1KmdFile)) {
                if (ParseKmdFile(secondary1KmdFile, secondaryMetadata1)) {
                    kmdFileCount++;
                }
            }
        }
        
        // Validate if .kmd files were found
        if (kmdFileCount > 0) {
            if (hybridAuth) {
                if (kmdFileCount == 4) {
                    if (!ValidateKmdMetadata(primaryMetadata, secondaryMetadata, 
                                             primaryMetadata1, secondaryMetadata1, true,
                                             primaryKmdFile, secondaryKmdFile, primary1KmdFile, secondary1KmdFile)) {
                        LOG_ERROR("✗ .kmd metadata validation failed (explicit syntax mode)");
                        return;
                    }
                    LOG_INFO("✓ .kmd validation passed (explicit syntax, hybrid mode)");
                    // Store validated metadata
                    this->primaryMetadata = primaryMetadata;
                    this->secondaryMetadata = secondaryMetadata;
                    this->primaryMetadata1 = primaryMetadata1;
                    this->secondaryMetadata1 = secondaryMetadata1;
                }
            } else {
                if (kmdFileCount == 2) {
                    if (!ValidateKmdMetadata(primaryMetadata, secondaryMetadata, 
                                             primaryMetadata1, secondaryMetadata1, false,
                                             primaryKmdFile, secondaryKmdFile, primary1KmdFile, secondary1KmdFile)) {
                        LOG_ERROR("✗ .kmd metadata validation failed (explicit syntax mode)");
                        return;
                    }
                    LOG_INFO("✓ .kmd validation passed (explicit syntax, single mode)");
                    // Store validated metadata
                    this->primaryMetadata = primaryMetadata;
                    this->secondaryMetadata = secondaryMetadata;
                }
            }
        }
        
        return;
    }
    
    LOG_INFO("Simplified key syntax detected - initiating auto-detection");
    
    // Initialize metadata structures for .kmd file parsing
    KeyMetadata primaryMetadata;
    KeyMetadata secondaryMetadata;
    KeyMetadata primaryMetadata1;
    KeyMetadata secondaryMetadata1;
    
    // ============================================================================
    // STEP 1: Auto-detect authentication from .kmd files if not specified by user
    // ============================================================================
    bool authAutoDetected = false;
    
    // Check if authentication was NOT specified in BIF
    // NOTE: For non-secure flow (no authentication, no keys), we don't require
    // authentication flag or key files - this is a valid use case!
    if (authType == Authentication::None && !hybridAuth) {
        // Before attempting auto-detection, check if ANY key files were provided
        // If no keys provided at all, this is a non-secure flow - just return
        bool hasAnyKeys = !pkFile.empty() || !skFile.empty() || !pkFile1.empty() || !skFile1.empty();
        
        if (!hasAnyKeys) {
            LOG_TRACE("No authentication and no key files specified - non-secure flow, skipping auto-detection");
            return;  // Non-secure flow: no authentication needed, no keys needed
        }
        
        LOG_INFO("Authentication not specified in BIF - attempting auto-detection from .kmd files");
        
        // Parse .kmd files to determine authentication
        std::string primaryKmdFile;
        std::string secondaryKmdFile;
        std::string primary1KmdFile;
        std::string secondary1KmdFile;
        
        int kmdFileCount = 0;
        bool primaryKmdParsed = false;
        bool secondaryKmdParsed = false;
        bool primary1KmdParsed = false;
        bool secondary1KmdParsed = false;
        
        // Try to parse primary.kmd (from pkFile)
        if (!pkFile.empty()) {
            primaryKmdFile = DeriveKmdFile(pkFile);
            if (FileExistsForKeyDetection(primaryKmdFile)) {
                if (ParseKmdFile(primaryKmdFile, primaryMetadata)) {
                    primaryKmdParsed = true;
                    kmdFileCount++;
                    LOG_TRACE("Auto-detect: Parsed %s - algorithm 0x%x", primaryKmdFile.c_str(), primaryMetadata.Algorithm);
                }
            }
        }
        
        // Try to parse secondary.kmd (from skFile)
        if (!skFile.empty()) {
            secondaryKmdFile = DeriveKmdFile(skFile);
            if (FileExistsForKeyDetection(secondaryKmdFile)) {
                if (ParseKmdFile(secondaryKmdFile, secondaryMetadata)) {
                    secondaryKmdParsed = true;
                    kmdFileCount++;
                    LOG_TRACE("Auto-detect: Parsed %s - algorithm 0x%x", secondaryKmdFile.c_str(), secondaryMetadata.Algorithm);
                }
            }
        }
        
        // Try to parse private_1.kmd (from pkFile1)
        if (!pkFile1.empty()) {
            primary1KmdFile = DeriveKmdFile(pkFile1);
            if (FileExistsForKeyDetection(primary1KmdFile)) {
                if (ParseKmdFile(primary1KmdFile, primaryMetadata1)) {
                    primary1KmdParsed = true;
                    kmdFileCount++;
                    LOG_TRACE("Auto-detect: Parsed %s - algorithm 0x%x", primary1KmdFile.c_str(), primaryMetadata1.Algorithm);
                }
            }
        }
        
        // Try to parse private_2.kmd (from skFile1)
        if (!skFile1.empty()) {
            secondary1KmdFile = DeriveKmdFile(skFile1);
            if (FileExistsForKeyDetection(secondary1KmdFile)) {
                if (ParseKmdFile(secondary1KmdFile, secondaryMetadata1)) {
                    secondary1KmdParsed = true;
                    kmdFileCount++;
                    LOG_TRACE("Auto-detect: Parsed %s - algorithm 0x%x", secondary1KmdFile.c_str(), secondaryMetadata1.Algorithm);
                }
            }
        }
        
        // Now auto-detect based on number of .kmd files found
        if (kmdFileCount == 4 && primaryKmdParsed && primary1KmdParsed) {
            // HYBRID MODE: 4 .kmd files found
            Authentication::Type alg1 = ConvertKmdAlgorithmToAuthType(primaryMetadata.Algorithm);
            Authentication::Type alg2 = ConvertKmdAlgorithmToAuthType(primaryMetadata1.Algorithm);
            
            if (alg1 != Authentication::None && alg2 != Authentication::None) {
                LOG_INFO("✓ Auto-detected HYBRID authentication from .kmd files:");
                LOG_INFO("  Algorithm 1: %s (from %s)", GetAlgorithmName(primaryMetadata.Algorithm).c_str(), primaryKmdFile.c_str());
                LOG_INFO("  Algorithm 2: %s (from %s)", GetAlgorithmName(primaryMetadata1.Algorithm).c_str(), primary1KmdFile.c_str());
                
                // Build hybrid spec string (same format as BIF parser: "alg1,alg2")
                std::string hybridSpec = "";
                
                // Convert Algorithm 1 to string
                switch (alg1) {
                    case Authentication::RSA: hybridSpec += "rsa"; break;
                    case Authentication::ECDSA: hybridSpec += "ecdsa"; break;
                    case Authentication::ECDSAp521: hybridSpec += "ecdsa-p521"; break;
                    case Authentication::MLDSA: hybridSpec += "mldsa"; break;
                    case Authentication::SLH_SHAKE256: hybridSpec += "slh-shake256"; break;
                    case Authentication::LMS_SHA2_256: hybridSpec += "lms-sha256"; break;
                    case Authentication::LMS_SHAKE256: hybridSpec += "lms-shake256"; break;
                    case Authentication::HSS_SHA2_256: hybridSpec += "hss-sha256"; break;
                    case Authentication::HSS_SHAKE256: hybridSpec += "hss-shake256"; break;
                    default: 
                        LOG_ERROR("Failed to auto-detect: Unknown algorithm type for Algorithm 1");
                        return;
                }
                
                hybridSpec += ",";
                
                // Convert Algorithm 2 to string
                switch (alg2) {
                    case Authentication::RSA: hybridSpec += "rsa"; break;
                    case Authentication::ECDSA: hybridSpec += "ecdsa"; break;
                    case Authentication::ECDSAp521: hybridSpec += "ecdsa-p521"; break;
                    case Authentication::MLDSA: hybridSpec += "mldsa"; break;
                    case Authentication::SLH_SHAKE256: hybridSpec += "slh-shake256"; break;
                    case Authentication::LMS_SHA2_256: hybridSpec += "lms-sha256"; break;
                    case Authentication::LMS_SHAKE256: hybridSpec += "lms-shake256"; break;
                    case Authentication::HSS_SHA2_256: hybridSpec += "hss-sha256"; break;
                    case Authentication::HSS_SHAKE256: hybridSpec += "hss-shake256"; break;
                    default: 
                        LOG_ERROR("Failed to auto-detect: Unknown algorithm type for Algorithm 2");
                        return;
                }
                
                // Use SetHybridAuthType() to process the hybrid spec (same as BIF parser)
                // This will populate hybridAlgorithms[] and set hybridAuth=true
                SetHybridAuthType(hybridSpec);
                authAutoDetected = true;
                
                LOG_INFO("  Auto-populated: authentication = %s", hybridSpec.c_str());
                
                // Print all .kmd file contents for HYBRID mode
                LOG_INFO("");
                LOG_INFO("========== HYBRID SIGNING - .KMD FILE CONTENTS ==========");
                LOG_INFO("");
                LOG_INFO("--- Algorithm 1 Primary Key Metadata (%s) ---", primaryKmdFile.c_str());
                LOG_INFO("  Algorithm      : 0x%02X (%s)", primaryMetadata.Algorithm, GetAlgorithmName(primaryMetadata.Algorithm).c_str());
                LOG_INFO("  Revocation ID  : 0x%08X", primaryMetadata.RevocationID);
                LOG_INFO("  Permission     : 0x%08X", primaryMetadata.Permission);
                LOG_INFO("  Authority      : 0x%08X", primaryMetadata.Authority);
                LOG_INFO("  Hybrid         : 0x%08X", primaryMetadata.Hybrid);
                LOG_INFO("  Reserved[0]    : 0x%08X", primaryMetadata.Reserved[0]);
                LOG_INFO("  Reserved[1]    : 0x%08X", primaryMetadata.Reserved[1]);
                LOG_INFO("  Reserved[2]    : 0x%08X", primaryMetadata.Reserved[2]);
                LOG_INFO("");
                LOG_INFO("--- Algorithm 1 Secondary Key Metadata (%s) ---", secondaryKmdFile.c_str());
                LOG_INFO("  Algorithm      : 0x%02X (%s)", secondaryMetadata.Algorithm, GetAlgorithmName(secondaryMetadata.Algorithm).c_str());
                LOG_INFO("  Revocation ID  : 0x%08X", secondaryMetadata.RevocationID);
                LOG_INFO("  Permission     : 0x%08X", secondaryMetadata.Permission);
                LOG_INFO("  Authority      : 0x%08X", secondaryMetadata.Authority);
                LOG_INFO("  Hybrid         : 0x%08X", secondaryMetadata.Hybrid);
                LOG_INFO("  Reserved[0]    : 0x%08X", secondaryMetadata.Reserved[0]);
                LOG_INFO("  Reserved[1]    : 0x%08X", secondaryMetadata.Reserved[1]);
                LOG_INFO("  Reserved[2]    : 0x%08X", secondaryMetadata.Reserved[2]);
                LOG_INFO("");
                LOG_INFO("--- Algorithm 2 Primary Key Metadata (%s) ---", primary1KmdFile.c_str());
                LOG_INFO("  Algorithm      : 0x%02X (%s)", primaryMetadata1.Algorithm, GetAlgorithmName(primaryMetadata1.Algorithm).c_str());
                LOG_INFO("  Revocation ID  : 0x%08X", primaryMetadata1.RevocationID);
                LOG_INFO("  Permission     : 0x%08X", primaryMetadata1.Permission);
                LOG_INFO("  Authority      : 0x%08X", primaryMetadata1.Authority);
                LOG_INFO("  Hybrid         : 0x%08X", primaryMetadata1.Hybrid);
                LOG_INFO("  Reserved[0]    : 0x%08X", primaryMetadata1.Reserved[0]);
                LOG_INFO("  Reserved[1]    : 0x%08X", primaryMetadata1.Reserved[1]);
                LOG_INFO("  Reserved[2]    : 0x%08X", primaryMetadata1.Reserved[2]);
                LOG_INFO("");
                LOG_INFO("--- Algorithm 2 Secondary Key Metadata (%s) ---", secondary1KmdFile.c_str());
                LOG_INFO("  Algorithm      : 0x%02X (%s)", secondaryMetadata1.Algorithm, GetAlgorithmName(secondaryMetadata1.Algorithm).c_str());
                LOG_INFO("  Revocation ID  : 0x%08X", secondaryMetadata1.RevocationID);
                LOG_INFO("  Permission     : 0x%08X", secondaryMetadata1.Permission);
                LOG_INFO("  Authority      : 0x%08X", secondaryMetadata1.Authority);
                LOG_INFO("  Hybrid         : 0x%08X", secondaryMetadata1.Hybrid);
                LOG_INFO("  Reserved[0]    : 0x%08X", secondaryMetadata1.Reserved[0]);
                LOG_INFO("  Reserved[1]    : 0x%08X", secondaryMetadata1.Reserved[1]);
                LOG_INFO("  Reserved[2]    : 0x%08X", secondaryMetadata1.Reserved[2]);
                LOG_INFO("");
                LOG_INFO("=========================================================");
                LOG_INFO("");
                
                // Store parsed metadata
                this->primaryMetadata = primaryMetadata;
                this->secondaryMetadata = secondaryMetadata;
                this->primaryMetadata1 = primaryMetadata1;
                this->secondaryMetadata1 = secondaryMetadata1;
            } else {
                LOG_ERROR("Failed to auto-detect authentication: Invalid algorithm codes in .kmd files");
                return;
            }
        } else if (kmdFileCount == 2 && primaryKmdParsed) {
            // SINGLE MODE: 2 .kmd files found
            Authentication::Type alg1 = ConvertKmdAlgorithmToAuthType(primaryMetadata.Algorithm);
            
            if (alg1 != Authentication::None) {
                LOG_INFO("✓ Auto-detected SINGLE authentication from .kmd files:");
                LOG_INFO("  Algorithm: %s (from %s)", GetAlgorithmName(primaryMetadata.Algorithm).c_str(), primaryKmdFile.c_str());
                
                // Set single authentication using SetAuthType() (same as BIF parser)
                SetAuthType(alg1);
                authAutoDetected = true;
                
                LOG_INFO("  Auto-populated: authentication = %s", GetAlgorithmName(primaryMetadata.Algorithm).c_str());
                
                // Print all .kmd file contents for SINGLE mode
                LOG_INFO("");
                LOG_INFO("========== SINGLE SIGNING - .KMD FILE CONTENTS ==========");
                LOG_INFO("");
                LOG_INFO("--- Primary Key Metadata (%s) ---", primaryKmdFile.c_str());
                LOG_INFO("  Algorithm      : 0x%02X (%s)", primaryMetadata.Algorithm, GetAlgorithmName(primaryMetadata.Algorithm).c_str());
                LOG_INFO("  Revocation ID  : 0x%08X", primaryMetadata.RevocationID);
                LOG_INFO("  Permission     : 0x%08X", primaryMetadata.Permission);
                LOG_INFO("  Authority      : 0x%08X", primaryMetadata.Authority);
                LOG_INFO("  Hybrid         : 0x%08X", primaryMetadata.Hybrid);
                LOG_INFO("  Reserved[0]    : 0x%08X", primaryMetadata.Reserved[0]);
                LOG_INFO("  Reserved[1]    : 0x%08X", primaryMetadata.Reserved[1]);
                LOG_INFO("  Reserved[2]    : 0x%08X", primaryMetadata.Reserved[2]);
                LOG_INFO("");
                LOG_INFO("--- Secondary Key Metadata (%s) ---", secondaryKmdFile.c_str());
                LOG_INFO("  Algorithm      : 0x%02X (%s)", secondaryMetadata.Algorithm, GetAlgorithmName(secondaryMetadata.Algorithm).c_str());
                LOG_INFO("  Revocation ID  : 0x%08X", secondaryMetadata.RevocationID);
                LOG_INFO("  Permission     : 0x%08X", secondaryMetadata.Permission);
                LOG_INFO("  Authority      : 0x%08X", secondaryMetadata.Authority);
                LOG_INFO("  Hybrid         : 0x%08X", secondaryMetadata.Hybrid);
                LOG_INFO("  Reserved[0]    : 0x%08X", secondaryMetadata.Reserved[0]);
                LOG_INFO("  Reserved[1]    : 0x%08X", secondaryMetadata.Reserved[1]);
                LOG_INFO("  Reserved[2]    : 0x%08X", secondaryMetadata.Reserved[2]);
                LOG_INFO("");
                LOG_INFO("=========================================================");
                LOG_INFO("");
                
                // Store parsed metadata
                this->primaryMetadata = primaryMetadata;
                this->secondaryMetadata = secondaryMetadata;
            } else {
                LOG_ERROR("Failed to auto-detect authentication: Invalid algorithm code in .kmd file");
                return;
            }
        } else {
            LOG_ERROR("Failed to auto-detect authentication: Expected 2 .kmd files (single auth) or 4 .kmd files (hybrid auth)");
            LOG_ERROR("Found %d .kmd file(s)", kmdFileCount);
            return;
        }
    }
    
    // ============================================================================
    // STEP 2: Determine algorithms (either from BIF or from auto-detection)
    // ============================================================================
    Authentication::Type algorithm1 = Authentication::None;
    Authentication::Type algorithm2 = Authentication::None;
    
    if (hybridAuth && hybridAlgorithms.size() == 2) {
        // Hybrid authentication: two algorithms
        algorithm1 = hybridAlgorithms[0];
        algorithm2 = hybridAlgorithms[1];
        if (!authAutoDetected) {
            LOG_INFO("Hybrid authentication specified in BIF:");
        }
        LOG_INFO("  Algorithm 1: %d", algorithm1);
        LOG_INFO("  Algorithm 2: %d", algorithm2);
    } else {
        // Single authentication algorithm
        algorithm1 = authType;
        if (!authAutoDetected) {
            LOG_INFO("Single authentication specified in BIF:");
        }
        LOG_INFO("  Algorithm: %d", algorithm1);
    }
    
    // ============================================================================
    // STEP 3: Process key files and parse/validate .kmd files (if not already done)
    // ============================================================================
    
    // Process primary keys for Algorithm 1 (pkFile -> PPK + PSK)
    if (!pkFile.empty()) {
        LOG_TRACE("Processing pkFile (primary keys for Algorithm 1): %s", pkFile.c_str());
        
        if (algorithm1 == Authentication::None) {
            LOG_ERROR("Cannot auto-detect keys: authentication type not determined");
            return;
        }
        
        // Populate PPK and PSK using ACTUAL algorithm
        if (!PopulatePrimaryKeys(pkFile, ppkFile, pskFile, algorithm1)) {
            LOG_ERROR("Failed to auto-populate primary key files from pkfile");
            return;
        }
        
        LOG_INFO("Algorithm 1 primary keys detected:");
        LOG_INFO("  PPK (ppkfile): %s", ppkFile.c_str());
        LOG_INFO("  PSK (pskfile): %s", pskFile.c_str());
    }
    
    // Process secondary keys for Algorithm 1 (skFile -> SPK + SSK)
    if (!skFile.empty()) {
        LOG_TRACE("Processing skFile (secondary keys for Algorithm 1): %s", skFile.c_str());
        
        if (algorithm1 == Authentication::None) {
            LOG_ERROR("Cannot auto-detect keys: authentication type not set in BIF");
            LOG_ERROR("Please specify 'authentication = <type>' before using skfile");
            return;
        }
        
        // Populate SPK and SSK using ACTUAL algorithm from BIF
        if (!PopulateSecondaryKeys(skFile, spkFile, sskFile, algorithm1)) {
            LOG_ERROR("Failed to auto-populate secondary key files from skfile");
            return;
        }
        
        LOG_INFO("Algorithm 1 secondary keys detected:");
        LOG_INFO("  SPK (spkfile): %s", spkFile.c_str());
        LOG_INFO("  SSK (sskfile): %s", sskFile.c_str());
    }
    
    // Process hybrid primary keys for Algorithm 2 (pkFile1 -> PPK1 + PSK1)
    if (!pkFile1.empty()) {
        LOG_TRACE("Processing pkFile1 (hybrid primary keys for Algorithm 2): %s", pkFile1.c_str());
        
        if (!hybridAuth) {
            LOG_ERROR("pkfile1 specified but hybrid authentication not enabled");
            LOG_ERROR("Please use 'authentication = <alg1>,<alg2>' for hybrid authentication");
            return;
        }
        
        if (algorithm2 == Authentication::None) {
            LOG_ERROR("Cannot auto-detect keys: second algorithm not set in hybrid authentication");
            return;
        }
        
        // Populate PPK1 and PSK1 using ACTUAL algorithm2 from BIF
        if (!PopulatePrimaryKeys(pkFile1, ppkFile1, pskFile1, algorithm2)) {
            LOG_ERROR("Failed to auto-populate hybrid primary key files from pkfile1");
            return;
        }
        
        LOG_INFO("Algorithm 2 (hybrid) primary keys detected:");
        LOG_INFO("  PPK1 (ppkfile1): %s", ppkFile1.c_str());
        LOG_INFO("  PSK1 (pskfile1): %s", pskFile1.c_str());
    }
    
    // Process hybrid secondary keys for Algorithm 2 (skFile1 -> SPK1 + SSK1)
    if (!skFile1.empty()) {
        LOG_TRACE("Processing skFile1 (hybrid secondary keys for Algorithm 2): %s", skFile1.c_str());
        
        if (!hybridAuth) {
            LOG_ERROR("skfile1 specified but hybrid authentication not enabled");
            LOG_ERROR("Please use 'authentication = <alg1>,<alg2>' for hybrid authentication");
            return;
        }
        
        if (algorithm2 == Authentication::None) {
            LOG_ERROR("Cannot auto-detect keys: second algorithm not set in hybrid authentication");
            return;
        }
        
        // Populate SPK1 and SSK1 using ACTUAL algorithm2 from BIF
        if (!PopulateSecondaryKeys(skFile1, spkFile1, sskFile1, algorithm2)) {
            LOG_ERROR("Failed to auto-populate hybrid secondary key files from skfile1");
            return;
        }
        
        LOG_INFO("Algorithm 2 (hybrid) secondary keys detected:");
        LOG_INFO("  SPK1 (spkfile1): %s", spkFile1.c_str());
        LOG_INFO("  SSK1 (sskfile1): %s", sskFile1.c_str());
    }
    
    // === STEP 2: Auto-detect and parse .kmd metadata files ===
    LOG_INFO("Detecting .kmd metadata files...");
    
    int kmdFileCount = 0;
    
    // Store .kmd filenames for validation error messages
    std::string primaryKmdFile;
    std::string secondaryKmdFile;
    std::string primary1KmdFile;
    std::string secondary1KmdFile;
    
    // Parse primary.kmd (from pkFile)
    if (!pkFile.empty()) {
        primaryKmdFile = DeriveKmdFile(pkFile);
        if (!ParseKmdFile(primaryKmdFile, primaryMetadata)) {
            LOG_ERROR("Failed to parse primary .kmd file");
            return;
        }
        kmdFileCount++;
    }
    
    // Parse secondary.kmd (from skFile)
    if (!skFile.empty()) {
        secondaryKmdFile = DeriveKmdFile(skFile);
        if (!ParseKmdFile(secondaryKmdFile, secondaryMetadata)) {
            LOG_ERROR("Failed to parse secondary .kmd file");
            return;
        }
        kmdFileCount++;
    }
    
    // Parse private_1.kmd (from pkFile1, hybrid only)
    if (!pkFile1.empty()) {
        primary1KmdFile = DeriveKmdFile(pkFile1);
        if (!ParseKmdFile(primary1KmdFile, primaryMetadata1)) {
            LOG_ERROR("Failed to parse hybrid primary .kmd file");
            return;
        }
        kmdFileCount++;
    }
    
    // Parse private_2.kmd (from skFile1, hybrid only)
    if (!skFile1.empty()) {
        secondary1KmdFile = DeriveKmdFile(skFile1);
        if (!ParseKmdFile(secondary1KmdFile, secondaryMetadata1)) {
            LOG_ERROR("Failed to parse hybrid secondary .kmd file");
            return;
        }
        kmdFileCount++;
    }
    
    // Validate .kmd file count
    if (hybridAuth) {
        // Hybrid: expect 4 .kmd files (primary, secondary, private_1, private_2)
        if (kmdFileCount != 4) {
            LOG_ERROR("Hybrid authentication requires 4 .kmd files, found %d", kmdFileCount);
            LOG_ERROR("Expected: primary.kmd, secondary.kmd, private_1.kmd, private_2.kmd");
            return;
        }
        LOG_INFO("✓ All 4 .kmd files detected and parsed for hybrid authentication");
    } else {
        // Single algorithm: expect 2 .kmd files (primary, secondary)
        if (kmdFileCount != 2) {
            LOG_ERROR("Single authentication requires 2 .kmd files, found %d", kmdFileCount);
            LOG_ERROR("Expected: primary.kmd, secondary.kmd");
            return;
        }
        LOG_INFO("✓ Both .kmd files detected and parsed for single authentication");
    }
    
    // === STEP 3: Validate .kmd metadata according to rules ===
    LOG_INFO("DEBUG: About to call ValidateKmdMetadata with isHybrid=%d", hybridAuth);
    LOG_INFO("DEBUG: primaryMetadata.Algorithm=0x%x, secondaryMetadata.Algorithm=0x%x", 
             primaryMetadata.Algorithm, secondaryMetadata.Algorithm);
    if (hybridAuth) {
        LOG_INFO("DEBUG: primaryMetadata1.Algorithm=0x%x, secondaryMetadata1.Algorithm=0x%x", 
                 primaryMetadata1.Algorithm, secondaryMetadata1.Algorithm);
    }
    
    if (!ValidateKmdMetadata(primaryMetadata, secondaryMetadata, 
                             primaryMetadata1, secondaryMetadata1, hybridAuth,
                             primaryKmdFile, secondaryKmdFile, primary1KmdFile, secondary1KmdFile)) {
        LOG_ERROR("✗ .kmd metadata validation failed");
        LOG_ERROR("Please check your .kmd files and ensure they follow the hybrid signing rules:");
        LOG_ERROR("  1. Primary and Secondary algorithms must match for both Algorithm 1 and 2");
        LOG_ERROR("  2. Each algorithm must be listed in the other's hybrid field");
        LOG_ERROR("  3. Only Classical + PQC combinations are allowed");
        return;
    }
    
    LOG_INFO("✓ Auto-detection completed successfully");
    LOG_INFO("Summary:");
    if (!pkFile.empty()) LOG_INFO("  Primary keys (Alg1) from pkfile: %s", pkFile.c_str());
    if (!skFile.empty()) LOG_INFO("  Secondary keys (Alg1) from skfile: %s", skFile.c_str());
    if (!pkFile1.empty()) LOG_INFO("  Primary keys (Alg2) from pkfile1: %s", pkFile1.c_str());
    if (!skFile1.empty()) LOG_INFO("  Secondary keys (Alg2) from skfile1: %s", skFile1.c_str());
}

/******************************************************************************/
/* Global-level auto-detection function for BifOptions                       */
/******************************************************************************/
void BifOptions::AutoDetectAndPopulateKeys()
{
    LOG_TRACE("AutoDetectAndPopulateKeys: Global-level auto-detection");
    
    // Initialize device-specific KMD functions for this architecture
    // This must be called before any .kmd file parsing operations
    InitializeDeviceSpecificKmdFunctions(arch);
    
    // Register metaheader explicit key files as global ONLY if global keys
    // are not already set. If the BIF specifies both global-level keys and
    // metaheader-level keys, the global keys must not be overwritten.
    if (!metaHdrAttributes.ppk.empty() && GetPPKFileName().empty()) {
        AddFiles(BIF::BisonParser::token::PPK_FILE, metaHdrAttributes.ppk);
    }
    if (!metaHdrAttributes.psk.empty() && GetPSKFileName().empty()) {
        AddFiles(BIF::BisonParser::token::PSK_FILE, metaHdrAttributes.psk);
    }
    if (!metaHdrAttributes.spk.empty() && GetSPKFileName().empty()) {
        AddFiles(BIF::BisonParser::token::SPK_FILE, metaHdrAttributes.spk);
    }
    if (!metaHdrAttributes.ssk.empty() && GetSSKFileName().empty()) {
        AddFiles(BIF::BisonParser::token::SSK_FILE, metaHdrAttributes.ssk);
    }
    
    // Register hybrid explicit syntax files (ppkfile1/pskfile1/spkfile1/sskfile1)
    if (!ppkFile1.empty()) {
        AddFiles(BIF::BisonParser::token::PPK_FILE1, ppkFile1);
    }
    if (!pskFile1.empty()) {
        AddFiles(BIF::BisonParser::token::PSK_FILE1, pskFile1);
    }
    if (!spkFile1.empty()) {
        AddFiles(BIF::BisonParser::token::SPK_FILE1, spkFile1);
    }
    if (!sskFile1.empty()) {
        AddFiles(BIF::BisonParser::token::SSK_FILE1, sskFile1);
    }
    
    // Check if user provided simplified syntax (pkFile/skFile/pkFile1/skFile1)
    bool hasSimplifiedSyntax = !pkFile.empty() || !skFile.empty() || !pkFile1.empty() || !skFile1.empty();
    bool hasExplicitSyntax = !metaHdrAttributes.ppk.empty() || !metaHdrAttributes.psk.empty() || 
                             !metaHdrAttributes.spk.empty() || !metaHdrAttributes.ssk.empty() ||
                             !ppkFile1.empty() || !pskFile1.empty() || !spkFile1.empty() || !sskFile1.empty();
    
    // RULE 1: Cannot mix simplified and explicit syntax
    if (hasSimplifiedSyntax && hasExplicitSyntax) {
        LOG_ERROR("Cannot mix simplified syntax (pkfile/skfile) with explicit syntax (ppkfile/pskfile/spkfile/sskfile)");
        LOG_ERROR("Please use either simplified OR explicit syntax, not both");
        return;
    }
    
    // If no simplified syntax provided, still validate .kmd files if explicit syntax used
    if (!hasSimplifiedSyntax) {
        return;
    }
    
    // If simplified syntax is provided at global level but authentication type is not set,
    // check if we should defer to partition-level authentication or use global auth.
    // STRATEGY: Only defer if partitions have their own keys. If partitions have no keys,
    // we need to use each partition's authentication type to process global keys appropriately.
    if (metaHdrAttributes.authenticate == Authentication::None) {
        LOG_INFO("Simplified key syntax detected at global level");
        LOG_INFO("Global authentication type not set");
        
        // Check if this is PPK hash generation mode (pkfile without skfile)
        bool isPpkHashMode = (!pkFile.empty() || !pkFile1.empty()) && skFile.empty() && skFile1.empty();
        
        if (isPpkHashMode) {
            LOG_INFO("PPK hash generation mode detected - parsing .kmd files without full key derivation");
            
            // Parse .kmd files for PPK hash generation
            if (!pkFile.empty()) {
                std::string primaryKmdFile = DeriveKmdFile(pkFile);
                if (FileExistsForKeyDetection(primaryKmdFile)) {
                    if (ParseKmdFile(primaryKmdFile, this->primaryMetadata)) {
                        LOG_INFO("✓ Parsed primary .kmd file for PPK hash: %s", primaryKmdFile.c_str());
                        
                        // Set ppkFile directly from pkFile for PPK hash generation
                        // No need to derive - just use the provided file
                        metaHdrAttributes.ppk = pkFile;
                        AddFiles(BIF::BisonParser::token::PPK_FILE, pkFile);
                    } else {
                        LOG_ERROR("Failed to parse primary .kmd file: %s", primaryKmdFile.c_str());
                        return;
                    }
                } else {
                    LOG_WARNING("Primary .kmd file not found: %s", primaryKmdFile.c_str());
                    LOG_WARNING("Proceeding without metadata - will use legacy PPK hash generation");
                }
            }
            
            // Parse .kmd for hybrid second algorithm (if pkfile1 exists)
            if (!pkFile1.empty()) {
                std::string primary1KmdFile = DeriveKmdFile(pkFile1);
                if (FileExistsForKeyDetection(primary1KmdFile)) {
                    if (ParseKmdFile(primary1KmdFile, this->primaryMetadata1)) {
                        LOG_INFO("✓ Parsed primary1 .kmd file for PPK hash (hybrid): %s", primary1KmdFile.c_str());
                        
                        // Set ppkFile1 directly from pkFile1 for hybrid PPK hash
                        ppkFile1 = pkFile1;
                        AddFiles(BIF::BisonParser::token::PPK_FILE1, pkFile1);
                    } else {
                        LOG_ERROR("Failed to parse primary1 .kmd file: %s", primary1KmdFile.c_str());
                        return;
                    }
                } else {
                    LOG_WARNING("Primary1 .kmd file not found: %s", primary1KmdFile.c_str());
                    LOG_WARNING("Proceeding without metadata for second algorithm");
                }
            }
            
            LOG_INFO("✓ PPK hash generation metadata loaded successfully");
            return;  // Don't process further - this is PPK hash only mode
        }
        
        // Not PPK hash mode - defer to partition-level authentication
        LOG_INFO("Will use per-partition authentication types to process global keys");
        // Don't return here - we'll handle this in bootimage.cpp by processing
        // global keys with each partition's authentication type
        return;
    }
    
    LOG_INFO("Simplified key syntax detected at global level - initiating auto-detection");
    
    // Initialize metadata structures for .kmd file parsing
    KeyMetadata primaryMetadata;
    KeyMetadata secondaryMetadata;
    KeyMetadata primaryMetadata1;
    KeyMetadata secondaryMetadata1;
    
    // Process primary keys (pkFile -> PPK + PSK)
    if (!pkFile.empty()) {
        LOG_TRACE("Processing global pkFile (primary keys): %s", pkFile.c_str());
        
        // For global level, use metaHdrAttributes.authenticate if set
        // Otherwise infer from EXTENSION ONLY (not filename pattern)
        Authentication::Type algToUse = metaHdrAttributes.authenticate;
        if (algToUse == Authentication::None) {
            LOG_WARNING("Global-level auto-detection: authentication type not set");
            LOG_WARNING("Inferring algorithm from file extension only");
            
            // Infer from extension only
            if (pkFile.find(".pem") != std::string::npos) {
                algToUse = Authentication::RSA;  // Default to RSA for .pem
                LOG_WARNING("Assuming RSA for .pem file (could be ECDSA - use partition-level for accuracy)");
            } else if (pkFile.find(".pub") != std::string::npos || pkFile.find(".prv") != std::string::npos) {
                algToUse = Authentication::LMS_SHA2_256;  // Default to LMS for .pub/.prv
                LOG_WARNING("Assuming LMS-SHA256 for .pub/.prv file (could be LMS-SHAKE256/HSS - use partition-level for accuracy)");
            } else if (pkFile.find(".pk") != std::string::npos || pkFile.find(".sk") != std::string::npos) {
                algToUse = Authentication::MLDSA;  // Default to MLDSA for .pk/.sk
                LOG_WARNING("Assuming MLDSA for .pk/.sk file (could be SLH-DSA - use partition-level for accuracy)");
            } else {
                LOG_ERROR("Cannot infer algorithm from global pkfile: %s", pkFile.c_str());
                LOG_ERROR("Supported extensions: .pem (RSA/ECDSA), .pub/.prv (LMS/HSS), .pk/.sk (MLDSA/SLH-DSA)");
                LOG_ERROR("Please use partition-level pkfile with authentication attribute for accuracy");
                return;
            }
        }
        
        // Populate PPK and PSK from pkFile
        std::string ppk, psk;
        if (!PopulatePrimaryKeys(pkFile, ppk, psk, algToUse)) {
            LOG_ERROR("Failed to auto-populate global primary key files from pkfile");
            return;
        }
        
        // Set global metaHdrAttributes
        metaHdrAttributes.ppk = ppk;
        metaHdrAttributes.psk = psk;
        
        // Also call AddFiles to register with infrastructure (like manual BIF does)
        AddFiles(BIF::BisonParser::token::PPK_FILE, ppk);
        AddFiles(BIF::BisonParser::token::PSK_FILE, psk);
    }
    
    // Process secondary keys (skFile -> SPK + SSK)
    if (!skFile.empty()) {
        LOG_TRACE("Processing global skFile (secondary keys): %s", skFile.c_str());
        
        // For global level, use metaHdrAttributes.authenticate if set
        Authentication::Type algToUse = metaHdrAttributes.authenticate;
        if (algToUse == Authentication::None) {
            // Infer from extension only
            if (skFile.find(".pem") != std::string::npos) {
                algToUse = Authentication::RSA;
            } else if (skFile.find(".pub") != std::string::npos || skFile.find(".prv") != std::string::npos) {
                algToUse = Authentication::LMS_SHA2_256;
            } else if (skFile.find(".pk") != std::string::npos || skFile.find(".sk") != std::string::npos) {
                algToUse = Authentication::MLDSA;
            } else {
                LOG_ERROR("Cannot infer algorithm from global skfile: %s", skFile.c_str());
                return;
            }
        }
        
        // Populate SPK and SSK from skFile
        std::string spk, ssk;
        if (!PopulateSecondaryKeys(skFile, spk, ssk, algToUse)) {
            LOG_ERROR("Failed to auto-populate global secondary key files from skfile");
            return;
        }
        
        // Set global metaHdrAttributes
        metaHdrAttributes.spk = spk;
        metaHdrAttributes.ssk = ssk;
        
        // Also call AddFiles to register with infrastructure
        AddFiles(BIF::BisonParser::token::SPK_FILE, spk);
        AddFiles(BIF::BisonParser::token::SSK_FILE, ssk);
    }
    
    // Process hybrid primary keys (pkFile1 -> PPK1 + PSK1)
    if (!pkFile1.empty()) {
        LOG_TRACE("Processing global pkFile1 (hybrid primary keys): %s", pkFile1.c_str());
        
        // For hybrid at global level, infer from extension only
        Authentication::Type algToUse = Authentication::None;
        if (pkFile1.find(".pem") != std::string::npos) {
            algToUse = Authentication::RSA;
        } else if (pkFile1.find(".pub") != std::string::npos || pkFile1.find(".prv") != std::string::npos) {
            algToUse = Authentication::LMS_SHA2_256;
        } else if (pkFile1.find(".pk") != std::string::npos || pkFile1.find(".sk") != std::string::npos) {
            algToUse = Authentication::MLDSA;
        } else {
            LOG_ERROR("Cannot infer algorithm from global pkfile1: %s", pkFile1.c_str());
            return;
        }
        
        // Populate PPK1 and PSK1 from pkFile1
        if (!PopulatePrimaryKeys(pkFile1, ppkFile1, pskFile1, algToUse)) {
            LOG_ERROR("Failed to auto-populate global hybrid primary key files from pkfile1");
            return;
        }
        
        // Also call AddFiles to register with infrastructure
        AddFiles(BIF::BisonParser::token::PPK_FILE1, ppkFile1);
        AddFiles(BIF::BisonParser::token::PSK_FILE1, pskFile1);
    }
    
    // Process hybrid secondary keys (skFile1 -> SPK1 + SSK1)
    if (!skFile1.empty()) {
        LOG_TRACE("Processing global skFile1 (hybrid secondary keys): %s", skFile1.c_str());
        
        // For hybrid at global level, infer from extension only
        Authentication::Type algToUse = Authentication::None;
        if (skFile1.find(".pem") != std::string::npos) {
            algToUse = Authentication::RSA;
        } else if (skFile1.find(".pub") != std::string::npos || skFile1.find(".prv") != std::string::npos) {
            algToUse = Authentication::LMS_SHA2_256;
        } else if (skFile1.find(".pk") != std::string::npos || skFile1.find(".sk") != std::string::npos) {
            algToUse = Authentication::MLDSA;
        } else {
            LOG_ERROR("Cannot infer algorithm from global skfile1: %s", skFile1.c_str());
            return;
        }
        
        // Populate SPK1 and SSK1 from skFile1
        if (!PopulateSecondaryKeys(skFile1, spkFile1, sskFile1, algToUse)) {
            LOG_ERROR("Failed to auto-populate global hybrid secondary key files from skfile1");
            return;
        }
        
        // Also call AddFiles to register with infrastructure
        AddFiles(BIF::BisonParser::token::SPK_FILE1, spkFile1);
        AddFiles(BIF::BisonParser::token::SSK_FILE1, sskFile1);
    }
    
    // === STEP 2: Auto-detect and parse .kmd metadata files (global level) ===
    LOG_INFO("Detecting .kmd metadata files (global level)...");
    
    int kmdFileCount = 0;
    bool isHybrid = !pkFile1.empty() || !skFile1.empty();
    
    // Store .kmd filenames for validation error messages
    std::string primaryKmdFile;
    std::string secondaryKmdFile;
    std::string primary1KmdFile;
    std::string secondary1KmdFile;
    
    // Parse primary.kmd (from pkFile)
    if (!pkFile.empty()) {
        primaryKmdFile = DeriveKmdFile(pkFile);
        if (!ParseKmdFile(primaryKmdFile, primaryMetadata)) {
            LOG_ERROR("Failed to parse global primary .kmd file");
            return;
        }
        kmdFileCount++;
    }
    
    // Parse secondary.kmd (from skFile)
    if (!skFile.empty()) {
        secondaryKmdFile = DeriveKmdFile(skFile);
        if (!ParseKmdFile(secondaryKmdFile, secondaryMetadata)) {
            LOG_ERROR("Failed to parse global secondary .kmd file");
            return;
        }
        kmdFileCount++;
    }
    
    // Parse private_1.kmd (from pkFile1, hybrid only)
    if (!pkFile1.empty()) {
        primary1KmdFile = DeriveKmdFile(pkFile1);
        if (!ParseKmdFile(primary1KmdFile, primaryMetadata1)) {
            LOG_ERROR("Failed to parse global hybrid primary .kmd file");
            return;
        }
        kmdFileCount++;
    }
    
    // Parse private_2.kmd (from skFile1, hybrid only)
    if (!skFile1.empty()) {
        secondary1KmdFile = DeriveKmdFile(skFile1);
        if (!ParseKmdFile(secondary1KmdFile, secondaryMetadata1)) {
            LOG_ERROR("Failed to parse global hybrid secondary .kmd file");
            return;
        }
        kmdFileCount++;
    }
    
    // Validate .kmd file count
    if (isHybrid) {
        // Hybrid: expect 4 .kmd files (primary, secondary, private_1, private_2)
        if (kmdFileCount != 4) {
            LOG_ERROR("Hybrid authentication requires 4 .kmd files, found %d", kmdFileCount);
            LOG_ERROR("Expected: primary.kmd, secondary.kmd, private_1.kmd, private_2.kmd");
            return;
        }
        LOG_INFO("✓ All 4 .kmd files detected and parsed for hybrid authentication");
    } else {
        // Single algorithm: expect 2 .kmd files (primary, secondary)
        if (kmdFileCount != 2) {
            LOG_ERROR("Single authentication requires 2 .kmd files, found %d", kmdFileCount);
            LOG_ERROR("Expected: primary.kmd, secondary.kmd");
            return;
        }
        LOG_INFO("✓ Both .kmd files detected and parsed for single authentication");
    }
    
    // === STEP 3: Validate .kmd metadata according to rules (global level) ===
    if (!ValidateKmdMetadata(primaryMetadata, secondaryMetadata, 
                             primaryMetadata1, secondaryMetadata1, isHybrid,
                             primaryKmdFile, secondaryKmdFile, primary1KmdFile, secondary1KmdFile)) {
        LOG_ERROR("✗ Global .kmd metadata validation failed");
        LOG_ERROR("Please check your .kmd files and ensure they follow the hybrid signing rules:");
        LOG_ERROR("  1. Primary and Secondary algorithms must match for both Algorithm 1 and 2");
        LOG_ERROR("  2. Each algorithm must be listed in the other's hybrid field");
        LOG_ERROR("  3. Only Classical + PQC combinations are allowed");
        return;
    }
    
    LOG_INFO("Global auto-detection completed successfully");
    if (!pkFile.empty()) LOG_INFO("  Primary keys from pkfile: %s", pkFile.c_str());
    if (!skFile.empty()) LOG_INFO("  Secondary keys from skfile: %s", skFile.c_str());
    if (!pkFile1.empty()) LOG_INFO("  Hybrid primary keys from pkfile1: %s", pkFile1.c_str());
    if (!skFile1.empty()) LOG_INFO("  Hybrid secondary keys from skfile1: %s", skFile1.c_str());
}

