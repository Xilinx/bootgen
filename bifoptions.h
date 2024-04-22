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

#pragma once

#ifndef _BIFOPTIONS_H_
#define _BIFOPTIONS_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "options.h"
#include "imageheadertable.h"

/* Forward Class References */
class Options;

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define SHUTTER_VAL                 0x01000020
#define DEFAULT_PMCDATA_LOADADDR    0xF2000000
#define DEFAULT_ID_CODE_S80         0x04ca8093


typedef struct
{
    Encryption::Type encrypt;
    KeySource::Type encrKeySource;
    std::string encrKeyFile;
    std::vector<uint32_t> encrBlocks;
    uint32_t defEncrBlockSize;
    Authentication::Type authenticate;
    std::string ppk;
    std::string psk;
    std::string spk;
    std::string ssk;
    std::string spkSignature;
    std::string presign;
    uint32_t revokeId;
    Checksum::Type checksum;
    DpaCM::Type dpaCM;
    PufHdLoc::Type pufHdLoc;
    std::vector<std::pair<std::string, uint32_t>> ihtOptionalDataInfo;
} MetaHdrInfo;

typedef struct
{
    uint32_t revokeId;
    bool userRevokeId;
    uint8_t deviceDNA[16];
    uint32_t jtagTimeout;
    bool userDeviceDNA;
} AuthJtagInfo;
/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class PartitionBifOptions
{
public:
    PartitionBifOptions();

    void SetArchType(Arch::Type type, bool versalNetSeries);
    void SetEncryptionBlocks(uint32_t size, uint32_t num = 1);
    void SetDefaultEncryptionBlockSize(uint32_t blk);
    void SetDestCpu(DestinationCPU::Type);
    void SetDestDevice(DestinationDevice::Type);
    void SetAuthType(Authentication::Type);
    void SetEncryptType(Encryption::Type);
    void SetChecksumType(Checksum::Type);
    void SetOwnerType(PartitionOwner::Type);
    void SetPartitionType(PartitionType::Type);
    void SetDpaCM(DpaCM::Type);
    void SetExceptionLevel(ExceptionLevel::Type);
    void SetSpkId(uint32_t);
    void SetTrustZone(TrustZone::Type);
    void SetEarlyHandoff(bool);
    void SetHivec(bool);
    void SetRevokeId(uint32_t);
    void SetSlrNum(uint8_t);
    void SetClusterNum(uint8_t id);
    void SetLockStepFlag();
    void SetAesKeyFile(std::string filename);
    void SetUdfDataFile(std::string filename);
    void SetEncryptionKeySource(KeySource::Type type);
    void SetAuthBlockAttr(size_t blocksizeattr);
    void SetPufHdLocation(PufHdLoc::Type);
    void SetReserveLength(uint64_t length, bool flag);
    void SetDelayAuth(bool flag);
    void SetTcmBootFlag();
    void SetTcmARegion(uint64_t tcm_a_region);
    void SetTcmBRegion(uint64_t tcm_b_region);
    void SetTcmCRegion(uint64_t tcm_c_region);

    std::string GetUdfDataFile(void);
    std::vector<uint32_t>& GetEncryptionBlocks(void);
    uint32_t GetDefaultEncryptionBlockSize(void);
    KeySource::Type GetEncryptionKeySource(void);
    DpaCM::Type GetDpaCM(void);
    uint32_t GetRevokeId(void);
    PufHdLoc::Type GetPufHdLocation(void);
    std::string GetOutputFileFromBifSection(std::string, std::string, PartitionType::Type);
    bool IsVersalNetSeries(void) { return versalNetSeries; }

    void Dump(void)
    {
        LOG_INFO("Filename: %s", filename.c_str());
        if (alignment.IsSet())
        {
            LOG_INFO(" Alignment: %x", alignment.Value());
        }
        if (offset.IsSet())
        {
            LOG_INFO(" Offset: %x", offset.Value());
        }
        if (reserve.IsSet())
        {
            LOG_INFO(" Reserve: %x", reserve.Value());
        }
        if (load.IsSet())
        {
            LOG_INFO(" Load: %x", load.Value());
        }
    }
//private:
    std::string aesKeyFile;
    std::string spkFile;
    std::string sskFile;
    std::string ppkFile;
    std::string pskFile;
    std::string spkSignatureFile;
    bool bootImage;
    bool pmuFwImage;
    bool pmcData;
    std::string presignFile;
    std::string acFile;
    std::string udfDataFile;
    Encryption::Type encryptType;
    Authentication::Type authType;
    Checksum::Type checksumType;
    PartitionOwner::Type ownerType;
    PartitionType::Type partitionType;
    DestinationCPU::Type destCPUType;
    DestinationDevice::Type destDeviceType;
    ExceptionLevel::Type exceptionLevel;
    TrustZone::Type trustzone;
    DpaCM::Type dpaCM;
    PufHdLoc::Type pufHdLoc;
    uint8_t slrNum;
    uint8_t clusterNum;
    bool tcmBoot;
    uint64_t tcmARegionAddr;
    uint64_t tcmBRegionAddr;
    uint64_t tcmCRegionAddr;

    //no default declared
    Override<int> alignment;
    Override<Binary::Length_t> offset;
    Override<Binary::Length_t> reserve;
    Override<Binary::Address_t> load;
    Override<Binary::Address_t> startup;

    bool bigEndian;
    bool a32Mode;
    uint32_t partitionId;
    uint32_t imageStoreId;
    bool boot;
    bool user;
    bool Static;
    bool Protected;
    bool multiboot;
    bool noautostart;
    bool early_handoff;
    bool bootloader;
    bool hivec;
    bool lockstep;
    KeySource::Type keySrc;
    size_t authblockattr;
    size_t pid;
    std::vector<uint32_t> blocks;
    uint32_t defBlockSize;
    uint32_t revokeId;

    //no default declared
    uint32_t blockSize;
    uint32_t spkSelect;
    bool spkSelLocal;
    uint32_t spkId;
    bool spkIdLocal;
    int  fileType;
    std::string bifSection;
    std::string filename;
    std::vector<std::string> filelist;
    bool updateReserveInPh;
    Arch::Type arch;
    bool delayAuth;
    bool erasePartition;

private:
    bool versalNetSeries;
};

/******************************************************************************/
class ImageBifOptions
{
public:
    ImageBifOptions()
        : imageId(0x1c000000)
        , imageName("default_subsys")
        , delayHandoff(false)
        , delayLoad(false)
        , memCopyAddr(0xFFFFFFFFFFFFFFFF)
        , imageType(PartitionType::RESERVED)
        , uniqueId(0xFFFFFFFF)
        , parentUniqueId(0xFFFFFFFF)
        , functionId(0xFFFFFFFF)
        , pcrNumber(0xFFFF)
        , pcrMeasurementIndex(0xFFFF)
        , slrConfigCnt(0)
        , slrConfigPartitionIndex(0)
    {
        partitionBifOptionsList.clear();
    }

    void SetDelayHandoff(bool flag);
    void SetDelayLoad(bool flag);
    
    void SetImageId(uint32_t id)
    {
        imageId = id;
    }
    void SetImageName(std::string name)
    {
        imageName = name;
    }
    void SetUniqueId(uint32_t id)
    {
        uniqueId = id;
    }
    void SetParentUniqueId(uint32_t id)
    {
        parentUniqueId = id;
    }
    void SetFunctionId(uint32_t id)
    {
        functionId = id;
    }
    void SetMemCopyAddress(uint64_t addr)
    {
        memCopyAddr = addr;
    }
    void SetImageType(PartitionType::Type type)
    {
        imageType = type;
    }
    void SetPcrNumber(uint16_t num)
    {
        if ((2 > num) || (num > 7))
        {
            LOG_ERROR("'pcr' is specified as '%d' for the Image:'%s'\n\t   The permitted values range from 2 to 7.", num, GetImageName().c_str());
        }
        pcrNumber = num;
    }
    void SetPcrMeasurementIndex(uint16_t pcrIndex)
    {
        pcrMeasurementIndex = pcrIndex;
    }

    uint32_t GetImageId(void)
    {
        return imageId;
    }
    std::string GetImageName(void)
    {
        return imageName;
    }
    uint32_t GetUniqueId(void)
    {
        return uniqueId;
    }
    uint32_t GetParentUniqueId(void)
    {
        return parentUniqueId;
    }
    uint32_t GetFunctionId(void)
    {
        return functionId;
    }
    void GetMemCopyAddress(uint64_t addr)
    {
        memCopyAddr = addr;
    }
    void GetImageType(PartitionType::Type type)
    {
        imageType = type;
    }
    bool GetDelayHandoff(void)
    {
        return delayHandoff;
    }
    bool GetDelayLoad(void)
    {
        return delayLoad;
    }
    uint64_t GetMemCopyAddress(void)
    {
        return memCopyAddr;
    }
    PartitionType::Type GetImageType(void)
    {
        return imageType;
    }
    uint16_t GetPcrNumber(void)
    {
        return pcrNumber;
    }
    uint16_t GetPcrMeasurementIndex(void)
    {
        return pcrMeasurementIndex;
    }
    std::list<PartitionBifOptions*> partitionBifOptionsList;

    uint32_t slrConfigCnt;
    uint32_t slrConfigPartitionIndex;
    std::list<SlrPdiInfo*> slrConfigPdiInfo;

private:
    uint32_t imageId;
    std::string imageName;
    bool delayHandoff;
    bool delayLoad;
    uint64_t memCopyAddr;
    PartitionType::Type imageType;
    uint32_t uniqueId;
    uint32_t parentUniqueId;
    uint32_t functionId;
    uint32_t pcrNumber;
    uint32_t pcrMeasurementIndex;
};

/******************************************************************************/
class BifOptions
{
public:
    BifOptions(Arch::Type architecture, const char* name);

    void Add(PartitionBifOptions* currentPartitionBifOptions, ImageBifOptions* currentImageBifOptions = NULL);
    void AddFiles(int type, std::string filename);
    void CheckForSameKeyandKeySrcPair(std::vector<std::pair<KeySource::Type, uint32_t*>> aesKeyandKeySrc);
    void CheckForBadKeyandKeySrcPair(std::vector<std::pair<KeySource::Type, uint32_t*>> aesKeyandKeySrc, std::string aesFilename);

    void SetRegInitFileName(std::string filename);
    void SetUdfBHFileName(std::string filename);
    void SetPmufwImageFileName(PartitionBifOptions* currentPartitionBifOptions);
    void SetPPKFileName(std::string filename);
    void SetPSKFileName(std::string filename);
    void SetSPKFileName(std::string filename);
    void SetSSKFileName(std::string filename);
    void SetSPKSignFileName(std::string filename);
    void SetBHSignFileName(std::string filename);
    void SetHeaderSignFileName(std::string filename);
    void SetBHKeyFileName(std::string filename);
    void SetPUFHelperFileName(std::string filename);
    void SetBHKekIVFileName(std::string filename);
    void SetBbramKekIVFileName(std::string filename);
    void SetEfuseKekIVFileName(std::string filename);
    void SetEfuseUserKek0IVFileName(std::string filename);
    void SetEfuseUserKek1IVFileName(std::string filename);
    void SetUserKeysFileName(std::string filename);
    void ParseUserKeyFile(std::string filename);
    void SetFamilyKeyFileName(std::string filename);
    void SetAESKeyFileName(std::string filename);
    void SetEncryptionKeySource(KeySource::Type type);
    void SetBootDevice(BootDevice::Type type);
    void SetBootDeviceAddress(uint32_t address);
    void SetBootVectorArray(uint32_t vector);
    void SetXipMode();
    void SetFsblFilename(std::string);
    void SetTotalPmcFwSize(uint32_t);
    void SetPmcFwSize(uint32_t);
    void SetPPKSelection(uint32_t ppkSelection);
    void SetSPKSelection(uint32_t spkSelection);
    void SetSpkId(uint32_t spkId);
    void SetHeaderAuthentication();
    void SetSmapWidth(uint32_t);
    void SetSplitMode(SplitMode::Type type);
    void SetSplitFmt(File::Type type);
    void SetPmcdataFile(const std::string & filename);
    void ClearPmcCdoFileList();
    void SetPmcCdoFileList(const std::string & filename);
    void SetPdiId(uint32_t id);
    void SetGroupName(std::string name);
    void SetParentId(uint32_t id);
    void SetBhRsa(BhRsa::Type value);
    void SetPufHdLoc(PufHdLoc::Type value);
    void SetDpaCM(DpaCM::Type value);
    void SetHeaderAC(bool flag);
    void SetHeaderEncryption(bool flag);
    void SetTotalpmcdataSize(uint32_t size);
    void SetAuthOnly(AuthOnly::Type type);
    void SetOptKey(OptKey::Type type);
    void SetPufMode(PufMode::Type type);
    void SetShutterValue(uint32_t value);
    void SetPufRingOscilltorSwapConfigValue(uint32_t value);
    void SetDiceEnable();
    void InsertEncryptionBlock(uint32_t size);
    void SetCore(Core::Type type);
    void SetMetaHeaderEncryptionKeySource(KeySource::Type type, bool versalNetSeries);
    void SetMetaHeaderEncryptType(Encryption::Type type);
    void SetMetaHeaderEncryptionKeyFile(std::string file);
    void SetMetaHeaderAuthType(Authentication::Type type);
    void SetPufHdinBHFlag();
    void SetAuthJtagRevokeID(uint32_t value);
    void SetAuthJtagDeviceDna(std::string string);
    void SetAuthJtagTimeOut(uint32_t value);
    void SetPmcDataAesFile(std::string filename);
    void SetRevokeId(uint32_t id);

    std::string GetGroupName(void);
    std::string GetAESKeyFileName(void);
    std::string GetRegInitFileName();
    OptKey::Type GetAesOptKeyFlag(void);
    std::vector<uint32_t>& GetEncryptionBlocksList(void);
    BhRsa::Type GetBhRsa(void);
    DpaCM::Type GetDpaCM(void);
    BootDevice::Type GetBootDevice(void);
    uint32_t GetBootDeviceAddress(void);
    uint32_t GetShutterValue(void);
    uint32_t GetPufRingOscilltorSwapConfigValue(void);
    DICE::Type GetDice(void);
    std::string GetHeaderSignatureFile(void);
    PufMode::Type GetPufMode(void);
    PufHdLoc::Type GetPufHdLoc(void);
    OptKey::Type GetOptKey(void);
    void SetIdCode(uint32_t id);
    void SetExtendedIdCode(uint32_t id);
    void SetPdiType(PartitionType::Type type);
    void SetBypassIdcodeFlag(bool flag);
    void SetAHwRoTFlag(bool flag);
    void SetSHwRoTFlag(bool flag);
    AuthOnly::Type GetAuthOnly(void);
    uint32_t GetPpkSelection(void);
    uint32_t GetSpkSelection(void);
    uint32_t GetSmapWidth(void);
    std::string GetPPKFileName(void);
    std::string GetPSKFileName(void);
    std::string GetSPKFileName(void);
    std::string GetSSKFileName(void);
    uint32_t GetSpkId(void);
    bool GetHeaderAC(void);
    bool GetHeaderEncyption(void);
    bool GetPpkSelectionGlobal(void);
    bool GetSpkSelectionGlobal(void);
    bool GetSpkIdGlobal(void);
    bool GetXipMode(void);
    std::string GetBhKeyFile(void);
    std::string GetPufHelperFile(void);
    std::string GetPmuFwImageFile(void);
    std::string GetBHKekIVFile(void);
    std::string GetBbramKekIVFile(void);
    std::string GetEfuseKekIVFile(void);
    std::string GetEfuseUserKek0IVFile(void);
    std::string GetEfuseUserKek1IVFile(void);
    std::string GetUserKeysFileName(void);
    std::string GetUdfBhFile(void);
    SplitMode::Type GetSplitMode(void);
    uint32_t GetIdCode(void);
    uint32_t GetExtendedIdCode(void);
    bool GetBypassIdcodeFlag(void);
    File::Type GetSplitFormat(void);
    std::vector<uint32_t>& GetBootVectorArray(void);
    std::string GetSPKSignFileName(void);
    std::string GetBHSignFileName(void);
    uint32_t GetPmcCdoLoadAddress(void);
    std::string GetPmcdataFile(void);
    std::vector<std::string> GetPmcCdoFileList(void);
    std::string GetPmcDataAesFile(void);
    uint32_t GetTotalPmcFwSize(void);
    uint32_t GetPmcFwSize(void);
    uint8_t* GetPmcDataBuffer(void);
    uint32_t GetTotalpmcdataSize(void);
    uint32_t GetPdiId(void);
    uint32_t GetParentId(void);
    KeySource::Type GetEncryptionKeySource(void);
    std::string GetKeySourceName(KeySource::Type type);
    bool GetPufHdinBHFlag(void);

    std::string GetFamilyKeyFileName();
    std::list<std::string> aie_elfs;
    std::list<ImageBifOptions*> imageBifOptionList;
    std::list<PartitionBifOptions*> partitionBifOptionList;
    Core::Type GetCore(void);
    uint32_t GetRevokeId();

    //Versal
    Binary::Address_t pmcCdoLoadAddress;
    uint32_t idCode;
    uint32_t extendedIdCode;
    PartitionType::Type pdiType;
    bool bypassIdCode;
    bool aHwrot;
    bool sHwrot;
    uint32_t pmcdataSize;
    uint32_t totalpmcdataSize;
    uint8_t* pmcDataBuffer;
    MetaHdrInfo metaHdrAttributes;
    AuthJtagInfo authJtagInfo;
    uint32_t slrBootCnt;
    uint32_t slrConfigCnt;
    PartitionBifOptions* lastPartitionBifOption;
    BootDevice::Type bootDevice;
    std::vector<uint32_t> pmcdataBlocks;
    uint8_t slrNum;
private:
    std::string pmcDataAesFile;
    std::string regInitFile;
    std::string udfBhFile;
    std::string pmuFwImageFile;
    std::string pmcdataFile;
    std::vector<std::string> pmcCdoFileList;
    std::string fsblFilename;
    std::string ppkFile;
    std::string pskFile;
    std::string spkFile;
    std::string sskFile;
    std::string spkSignatureFile;
    std::string bhSignatureFile;
    std::string headerSignatureFile;
    std::string bhKeyFile;
    std::string pufHelperFile;
    std::string bhKekIVFile;
    std::string bbramKekIVFile;
    std::string efuseKekIVFile;
    std::string efuseUserKek0IVFile;
    std::string efuseUserKek1IVFile;
    std::string familyKeyFile;
    std::string aesKeyFile;
    std::string userKeyFile;
    KeySource::Type keySourceEncryption;
    uint32_t bootDeviceAddress;
    std::vector<uint32_t> bootvectors;
    BhRsa::Type bhAuthEnable;
    PufHdLoc::Type pufHdLoc;
    bool pufHdinBHEnable;
    AuthOnly::Type authOnly;
    PufMode::Type pufMode;
    OptKey::Type optKey;
    uint32_t shutterVal;
    uint32_t pufRoSwapConfigVal;
    DpaCM::Type dpaCM;
    DICE::Type dice;
    bool xipMode;
    uint32_t ppkSelect;
    bool isPPKSelectGlobal;
    uint32_t spkSelect;
    bool isSPKSelectGlobal;
    uint32_t spkId;
    bool isSpkIdGlobal;
    bool headerAuthParam;
    bool createHeaderAC;
    bool doHeaderEncryption;
    SplitMode::Type splitMode;
    File::Type splitFmt;
    std::string groupname;
    Arch::Type arch;
    std::vector<uint32_t> blocks;
    Core::Type core;
    // Versal
    uint32_t pdiId;
    uint32_t parentId;
    uint32_t revokeId;
    uint32_t smapWidth;
};
#endif
