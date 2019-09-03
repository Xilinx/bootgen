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

#pragma once

#ifndef _BIFOPTIONS_H_
#define _BIFOPTIONS_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "options.h"

/* Forward Class References */
class Options;


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define SHUTTER_VAL                 0x01000020

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

    void SetArchType(Arch::Type type);

    void SetEncryptionBlocks(uint32_t size, uint32_t num = 1);
    void SetDefaultEncryptionBlockSize(uint32_t blk);
    void SetDestCpu(DestinationCPU::Type);
    void SetDestDevice(DestinationDevice::Type);
    void SetAuthType(Authentication::Type);
    void SetEncryptType(Encryption::Type);
    void SetChecksumType(Checksum::Type);
    void SetOwnerType(PartitionOwner::Type);
    void SetExceptionLevel(ExceptionLevel::Type);
    void SetTrustZone(TrustZone::Type);
    void SetEarlyHandoff(bool);
    void SetHivec(bool flag);

    std::vector<uint32_t>& GetEncryptionBlocks(void);
    void SetAuthBlockAttr(size_t blocksizeattr);

    uint32_t GetDefaultEncryptionBlockSize(void);
    void SetEncryptionKeySource(KeySource::Type type);

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
    std::string spkSignatureFile;
    bool bootImage;
    bool pmuFwImage;
    std::string presignFile;
    std::string udfDataFile;
    Encryption::Type encryptType;
    Authentication::Type authType;
    Checksum::Type checksumType;
    PartitionOwner::Type ownerType;
    DestinationCPU::Type destCPUType;
    DestinationDevice::Type destDeviceType;
    ExceptionLevel::Type exceptionLevel;
    TrustZone::Type trustzone;

    //no default declared
    Override<int> alignment;
    Override<Binary::Length_t> offset;
    Override<Binary::Length_t> reserve;
    Override<Binary::Address_t> load;
    Override<Binary::Address_t> startup;
    
    uint32_t partitionId;
    bool boot;
    bool user;
    bool Static;
    bool Protected;
    bool multiboot;
    bool noautostart;
    bool early_handoff;
    bool bootloader;
    bool hivec;
    size_t authblockattr;
    size_t pid;
    std::vector<uint32_t> blocks;
    uint32_t defBlockSize;

    //no default declared
    uint32_t blockSize;
    uint32_t spkSelect;
    bool spkSelLocal;
    uint32_t spkId;
    bool spkIdLocal;
    int  fileType;
    std::string filename;

    Arch::Type arch;
};

/******************************************************************************/
class BifOptions
{
public:
    BifOptions(Arch::Type architecture, const char* name);

    void Add(PartitionBifOptions* currentPartitionBifOptions);

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
    void SetBHKeyIVFileName(std::string filename);
    void SetFamilyKeyFileName(std::string filename);
    void SetAESKeyFileName(std::string filename);
    void SetEncryptionKeySource(KeySource::Type type);
    void SetBootDevice(BootDevice::Type type);
    void SetBootVectorArray(uint32_t vector);
    void SetXipMode();
    void SetFsblFilename(std::string);
    void SetPPKSelection(uint32_t ppkSelection);
    void SetSPKSelection(uint32_t spkSelection);
    void SetSpkId(uint32_t spkId);
    void SetHeaderAuthentication();
    void SetSplitMode(SplitMode::Type type);
    void SetSplitFmt(File::Type type);
    void SetGroupName(std::string name);
    void SetBhRsa(BhRsa::Type value);
    void SetPufHdLoc(PufHdLoc::Type value);
    void SetHeaderAC(bool flag);
    void SetAuthOnly(AuthOnly::Type type);
    void SetOptKey(OptKey::Type type);
    void SetPufMode(PufMode::Type type);
    void SetShutterValue(uint32_t value);
    void InsertEncryptionBlock(uint32_t size);
    void SetCore(Core::Type type);

    std::string GetGroupName(void);
    std::string GetAESKeyFileName(void);
    std::string GetRegInitFileName();
    OptKey::Type GetAesOptKeyFlag(void);
    std::vector<uint32_t>& GetEncryptionBlocksList(void);
    BhRsa::Type GetBhRsa(void);
    BootDevice::Type GetBootDevice(void);
    uint32_t GetShutterValue(void);
    std::string GetHeaderSignatureFile(void);
    PufMode::Type GetPufMode(void);
    PufHdLoc::Type GetPufHdLoc(void);
    OptKey::Type GetOptKey(void);
    AuthOnly::Type GetAuthOnly(void);
    uint32_t GetPpkSelection(void);
    uint32_t GetSpkSelection(void);
    std::string GetPPKFileName(void);
    std::string GetPSKFileName(void);
    std::string GetSPKFileName(void);
    std::string GetSSKFileName(void);
    uint32_t GetSpkId(void);
    bool GetHeaderAC(void);
    bool GetPpkSelectionGlobal(void);
    bool GetSpkSelectionGlobal(void);
    bool GetSpkIdGlobal(void);
    bool GetXipMode(void);
    std::string GetBhKeyFile(void);
    std::string GetPufHelperFile(void);
    std::string GetPmuFwImageFile(void);
    std::string GetBHKeyIVFile(void);
    std::string GetUdfBhFile(void);
    SplitMode::Type GetSplitMode(void);
    File::Type GetSplitFormat(void);
    std::vector<uint32_t>& GetBootVectorArray(void);
    std::string GetSPKSignFileName(void);
    std::string GetBHSignFileName(void);
    std::string GetSpkSignFileName(void);
    std::string GetFamilyKeyFileName();
    std::list<PartitionBifOptions*> partitionBifOptionList;
    Core::Type GetCore(void);

private:
    std::string regInitFile;
    std::string udfBhFile;
    std::string pmuFwImageFile;
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
    std::string bhKeyIVFile;
    std::string familyKeyFile;
    std::string aesKeyFile;
    KeySource::Type keySourceEncryption;
    BootDevice::Type bootDevice;
    std::vector<uint32_t> bootvectors;
    BhRsa::Type bhAuthEnable;
    PufHdLoc::Type pufHdLoc;
    AuthOnly::Type authOnly;
    PufMode::Type pufMode;
    OptKey::Type optKey;
    uint32_t shutterVal;
    bool xipMode;
    uint32_t ppkSelect;
    bool isPPKSelectGlobal;
    uint32_t spkSelect;
    bool isSPKSelectGlobal;
    uint32_t spkId;
    bool isSpkIdGlobal;
    bool headerAuthParam;
    bool createHeaderAC;
    SplitMode::Type splitMode;
    File::Type splitFmt;
    std::string groupname;
    Arch::Type arch;
    std::vector<uint32_t> blocks;
    Core::Type core;
};
#endif
