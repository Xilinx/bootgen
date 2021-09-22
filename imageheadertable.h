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

#pragma once

#ifndef _IMAGEHEADERTABLE_H_
#define _IMAGEHEADERTABLE_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <vector>
#include "authentication.h"
#include "elftools.h"


typedef enum
{
    BOOT,
    CONFIG,
    MASTER_CDO,
} SlrPdiType;

struct SlrId
{
    typedef enum
    {
        INVALID,
        SLR_1,
        SLR_2,
        SLR_3,
        MASTER,
    } Type;
};

typedef struct
{
    std::string file;
    SlrId::Type index;
    SlrPdiType type;
} SlrPdiInfo;

/* Forward Class References */
class BaseThing;
class Options;
class BootImage;
class Section;
class PartitionHeader;
class ChecksumContext;
class MD5ChecksumContext;
class AuthenticationContext;
class EncryptionContext;
class BifOptions;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class ImageHeader : public BaseThing
{
public:
    ImageHeader() {}
    ImageHeader(std::string& filename);
    ImageHeader(std::ifstream& f);
    ImageHeader(uint8_t* data, uint64_t len);
    
    virtual ~ImageHeader() {}

    virtual void Build(BootImage& bi, Binary& cache) { return; }
    virtual void Link(BootImage &bi, PartitionHeader* partitionHeader, ImageHeader* nextImageheader) { return; }
    virtual void ValidateEncryptionAttributes(BootImage & bi) {};
    void Relink();

    virtual void SetNextImageHeaderOffset(uint32_t addr) { }
    virtual void SetPartitionHeaderOffset(uint32_t addr) { }
    virtual void SetDataSectionCount(uint32_t cnt) { }
    virtual void SetImageNameLength(uint32_t len) { }
    virtual void SetImageName(void) { }

    virtual Binary::Address_t GetNextImageHeaderOffset(void) { return 0;  }
    virtual uint32_t GetPartitionHeaderOffset(void) { return 0; }
    virtual uint32_t GetDataSectionCount(void) { return 0; }
    virtual uint32_t GetImageNameLength(void) { return 0; }
    virtual uint32_t GetPartitionUid(void) { return 0; }
    virtual KeySource::Type GetEncryptionKeySrc() { return KeySource::None; }

    int NumPartitions()
    {
        return (int)partitionHeaderList.size();
    }

    void SetDomain(Domain::Type type) { domain = type; }
    void SetName(std::string name) { Name = name; }
    void SetFilename(std::string name) { Filename = name; }
    void SetBootloader(bool flag) { Bootloader = flag; }
    void SetBootFlag(bool flag) { Boot = flag; }
    void SetMultibootFlag(bool flag) { Multiboot = flag; }
    void SetNoAutoStartFlag(bool flag) { Noautostart = flag; }
    void SetProtectedFlag(bool flag) { Protected = flag; }
    void SetStaticFlag(bool flag) { Static = flag; }
    void SetUserFlag(bool flag) { User = flag; }
    void SetEarlyHandoff(bool flag) { early_handoff = flag; }
    void SetHivec(bool flag) { hivec = flag; }
    void SetSlrPartition(bool flag) { isSlrPartition = flag; }
    void SetAuthenticationType(Authentication::Type type) { authType = type; }
    void SetAuthContext(AuthenticationContext* ctx) { Auth = ctx; }
    void SetEncryptContext(EncryptionContext* ctx) { Encrypt = ctx; }
    void SetChecksumContext(ChecksumContext* ctx) { Checksum = ctx; }
    void SetPartOwner(PartitionOwner::Type type) { PartOwner = type; }
    void SetPartitionType(PartitionType::Type type) { partitionType = type; }
    void SetDestCpu(DestinationCPU::Type type);
    void SetDestDevice(DestinationDevice::Type type);
    void SetExceptionLevel(ExceptionLevel::Type type) { exceptionLevel = type; }
    void SetTrustZone(TrustZone::Type type) { trustzone = type; }
    void SetAlignment(Override<int> data) { Alignment = data; }
    void SetOffset(Override<Binary::Length_t> data) { Offset = data; }
    void SetReserve(Override<Binary::Length_t> data, bool updateReserveFlag) { Reserve = data; updateReserveInPh = updateReserveFlag;}
    void SetLoad(Override<Binary::Address_t> data) { Load = data; }
    void SetStartup(Override<Binary::Address_t> data) { Startup = data; }
    void InsertPartitionHeaderList(PartitionHeader* ph) { partitionHeaderList.push_back(ph); }
    void SetFileList(std::vector<std::string> files) { filelist = files; }
    virtual void SetAuthBlock(size_t blockSize, bool flag) { };
    virtual void SetPartitionUid(uint32_t id) { };
    virtual void SetBigEndian(bool) { };
    virtual void SetA32ExecMode(bool) { };
    virtual void SetDpacm(DpaCM::Type) { };
    virtual void SetPufHdLocation(PufHdLoc::Type type) { };

    virtual void SetEncryptionKeySrc(KeySource::Type type) { };
    virtual void SetPartitionRevocationId(uint32_t id) { };
    virtual uint32_t GetPartitionRevocationId() { return 0; }
    virtual void SetSlrBootPartitions(std::list<SlrPdiInfo*>) { };
    virtual void SetSlrConfigPartitions(std::list<SlrPdiInfo*>) { };
    void SetMemCopyAddress(uint64_t addr) { ihMemCpyAddr = addr; }
    void SetDelayLoadHandOffFlags(bool load_flag, bool handoff_flag) { ihDelayLoad = load_flag; ihDelayHandoff = handoff_flag; }

    void SetUserPartitionNum(size_t num) 
    { 
        userPartitionNum = num;
        if (num != 0)
        {
            isUserPartitionNum = true;
        }
    }

    Domain::Type GetDomain(void) { return domain; }
    std::string GetName(void) { return Name; }
    std::string GetFilename(void) { return Filename; }
    bool IsBootloader(void) { return Bootloader; }
    bool IsBootFlagSet(void) { return Boot; }
    bool IsMultibootFlagSet(void) { return Multiboot; }
    bool IsNoAutoStartFlagSet(void) { return Noautostart; }
    bool IsProtectedFlagSet(void) { return Protected; }
    bool IsStaticFlagSet(void) { return Static; }
    bool IsUserFlagSet(void) { return User; }
    bool GetEarlyHandoff(void) { return early_handoff; }
    bool GetHivec(void) { return hivec; }
    bool IsSlrPartition(void) { return isSlrPartition; }

    Authentication::Type GetAuthenticationType(void) { return authType; }
    AuthenticationContext* GetAuthContext(void) { return Auth; }
    EncryptionContext* GetEncryptContext(void) { return Encrypt; }
    ChecksumContext* GetChecksumContext(void) { return Checksum; }
    PartitionOwner::Type GetPartOwner(void) { return PartOwner; }
    PartitionType::Type GetPartitionType(void) { return partitionType; }
    DestinationCPU::Type GetDestCpu(void) { return destCpu; }
    DestinationDevice::Type GetDestinationDevice(void) { return destDevice; }
    ExceptionLevel::Type GetExceptionLevel(void) { return exceptionLevel; }
    TrustZone::Type GetTrustZone(void) { return trustzone; }
    Override<int> GetAlignment(void) { return Alignment; }
    Override<Binary::Length_t> GetOffset(void) { return Offset; }
    Override<Binary::Length_t> GetReserve(void) { return Reserve; }
    Override<Binary::Address_t> GetLoad(void) { return Load; }
    Override<Binary::Address_t> GetStartup(void) { return Startup; }
    std::list<PartitionHeader*>& GetPartitionHeaderList(void) { return partitionHeaderList; }
    ElfClass::Type GetElfClass(uint8_t* elfdata);
    size_t GetAuthBlock(void);
    size_t GetUserPartitionNum(void) { return userPartitionNum; }
    bool IsUserPartitionNumSet(void) { return isUserPartitionNum; }
    uint64_t GetMemCopyAddress(void) { return ihMemCpyAddr; }
    virtual DpaCM::Type GetDpacm(void) { return DpaCM::DpaCMDisable; }
    virtual std::string GetKekIV() { return ""; }
    virtual PufHdLoc::Type GetPufHdLocation(void) { return PufHdLoc::PUFinEFuse; }
    std::vector<std::string> GetFileList(void) { return filelist; }

    // For multiple key files and auth parameters
    void SetAesKeyFile(std::string filename) { aesKeyFile = filename; }
    void SetAesKeyFileGeneration(bool flag) { generateAesKeyFile = flag; }
    void SetPpkFile(std::string filename) { ppkFile = filename; }
    void SetPskFile(std::string filename) { pskFile = filename; }
    void SetSpkFile(std::string filename) { spkFile = filename; }
    void SetSskFile(std::string filename) { sskFile = filename; }
    void SetSpkSignFile(std::string filename) { spkSignFile = filename; }
    void SetBhSignFile(std::string filename) { bhSignFile = filename; }
    void SetPpkSelect(uint32_t id) { ppkSelect = id; }
    void SetSpkSelect(uint32_t id) { spkSelect = id; }
    void SetSpkId(uint32_t id) { spkId = id; }

    std::string GetAesKeyFile (void) { return aesKeyFile; }
    bool GetAesKeyFileGeneration(void) { return generateAesKeyFile; }
    std::string GetPpkFile(void) { return ppkFile; }
    std::string GetPskFile(void) { return pskFile; }
    std::string GetSpkFile (void) { return spkFile; }
    std::string GetSskFile (void) { return sskFile; }
    std::string GetSpkSignFile (void) { return spkSignFile; }
    std::string GetBhSignFile (void) { return bhSignFile; }
    uint32_t GetPpkSelect(void) { return ppkSelect; }
    uint32_t GetSpkSelect(void) { return spkSelect; }
    uint32_t GetSpkId(void) { return spkId; }

    void SetPmuFwSizeIh(uint32_t);
    void SetFsblFwSizeIh(uint32_t);
    void SetTotalPmuFwSizeIh(uint32_t);
    void SetTotalFsblFwSizeIh(uint32_t);
    void SetFsblSourceAddrIh(Binary::Address_t);
    void SetTotalPmcDataSizeIh(uint32_t size);
    void SetPmcDataSizeIh(uint32_t size);
    void SetTotalPmcFwSizeIh(uint32_t size);

    uint32_t GetPmuFwSizeIh(void);
    uint32_t GetFsblFwSizeIh(void);
    uint32_t GetTotalPmuFwSizeIh(void);
    uint32_t GetTotalFsblFwSizeIh(void);
    Binary::Address_t GetFsblSourceAddrIh(void);
    std::vector<uint32_t>&  GetEncrBlocksList(void);
    uint32_t  GetDefaultEncrBlockSize(void);
    uint32_t GetTotalPmcFwSizeIh(void);
    uint32_t GetPmcFwSizeIh(void);
    void InsertEncrBlocksList(uint32_t blk);
    void SetDefaultEncrBlockSize(uint32_t blk);
    uint32_t GetImageId(void);
    uint32_t GetParentUniqueId(void);
    uint32_t GetUniqueId(void);
    uint32_t GetFunctionId(void);
    bool IsUidInfoFoundInCdo(void);

    bool GetPLPowerDomainFlag() { return isIhPLPowerDomain; }
    bool GetLowPowerDomainFlag() { return isIhLowPowerDomain; }
    bool GetFullPowerDomainFlag() { return isIhFullPowerDomain; }
    bool GetSystemPowerDomainFlag() { return isIhSystemPowerDomain; }

protected:
    Domain::Type domain;
    uint8_t* buffer;
    uint64_t bufferSize;
    std::string Name;
    std::string Filename;
    std::vector<std::string> filelist;
    bool Bootloader;
    bool Boot;
    bool Multiboot;
    bool Noautostart;
    bool Protected;
    bool Static;
    bool User;
    bool xipMode;
    bool early_handoff;
    bool hivec;
    uint32_t pmuFwSize;
    uint32_t pmcdataSize;
    uint32_t fsblFwSize;
    uint32_t totalPmuFwSize;
    uint32_t totalpmcdataSize;
    uint32_t totalFsblFwSize;
    uint8_t non_zero_elf_sec_count;
    Binary::Address_t sourceAddr;
    uint64_t ihMemCpyAddr;
    bool ihDelayLoad;
    bool ihDelayHandoff;
    bool isSlrPartition;
    bool isIhFullPowerDomain;
    bool isIhLowPowerDomain;
    bool isIhSystemPowerDomain;
    bool isIhPLPowerDomain;

    // For multiple key files & auth parameters
    std::string aesKeyFile;
    bool generateAesKeyFile;
    std::string ppkFile;
    std::string pskFile;
    std::string spkFile;
    std::string sskFile;
    std::string spkSignFile;
    std::string bhSignFile;
    uint32_t ppkSelect;
    uint32_t spkSelect;
    uint32_t spkId;

    Authentication::Type authType;
    AuthenticationContext* Auth;
    EncryptionContext* Encrypt;
    ChecksumContext* Checksum;
    PartitionType::Type partitionType;
    PartitionOwner::Type PartOwner;
    DestinationCPU::Type destCpu;
    DestinationDevice::Type destDevice;
    ExceptionLevel::Type exceptionLevel;
    TrustZone::Type trustzone;
    uint32_t partitionUid;
    uint32_t partitionRevokeId;
    KeySource::Type keySrc;
    Override<int> Alignment;
    Override<Binary::Length_t> Offset;
    Override<Binary::Length_t> Reserve;
    Override<Binary::Address_t> Load;
    Override<Binary::Address_t> Startup;
    size_t userPartitionNum;
    bool isUserPartitionNum;
    bool a32Mode;
    bool bigEndian;
    bool updateReserveInPh;
    DpaCM::Type dpacm;
    PufHdLoc::Type pufHdLoc;
    std::string kekIvFile;

    std::list<PartitionHeader*> partitionHeaderList;

    ElfClass::Type elfClass;

    std::vector<uint32_t>   encrBlocks;
    uint32_t defEncrBlockSize;
    size_t authBlock;

    virtual void ImportElf(BootImage& bi);
    virtual void ImportBit(BootImage& bi) { };
    virtual void ImportBin(BootImage& bi) { };
    virtual void ImportFpgaDataFile(BootImage& bi) { };
    uint8_t* CombineElfSections(ElfFormat* elf, Binary::Length_t* size, Binary::Address_t* load_addr);
    uint8_t* GetElfSections(ElfFormat* elf, Binary::Length_t* size, Binary::Address_t* load_addr, uint32_t iprog);
    virtual void CreateElfPartitions(BootImage& bi, ElfFormat* elf, uint8_t proc_state) {};

    bool slaveBootSplitMode;
    uint32_t fullBhSize;
    uint32_t allHdrSize;
    std::list<SlrPdiInfo*> slrBootPdiInfo;
    std::list<SlrPdiInfo*> slrConfigPdiInfo;
    uint32_t imageId;
    uint32_t uniqueId;
    uint32_t parentUniqueId;
    uint32_t functionId;
    bool uidInfoFoundInCdo;
};

/******************************************************************************/
class ImageHeaderTable : public BaseThing
{
public:
    ImageHeaderTable()
        : slaveBootSplitMode(false)
        , fullBhSize(0)
        , allHdrSize(0)
        , metaHeaderLength(0)
        , metaHdrSecHdrIv(NULL)
        , metaHdrKeySrc(KeySource::None)
        , encrypt(NULL)
    { }

    virtual ~ImageHeaderTable() {}

    virtual void Build(BootImage& bi, Binary& cache);
    virtual void Link(BootImage& bi);
    virtual void RealignSectionDataPtr(void) = 0;
    ImageHeader* GetFSBLImageHeader();

    std::list<ImageHeader*> imageHeaderList;

    virtual void SetImageHeaderTableVersion(uint32_t version) = 0;
    virtual void SetHeaderTablesSize() {};
    virtual void SetTotalMetaHdrLength(uint32_t size) {};
    virtual void SetMetaHdrSecureHdrIv(uint8_t* iv) {};
    virtual void SetMetaHdrKeySrc(KeySource::Type, BifOptions* bifOptions) {};
    virtual void SetPartitionCount(uint32_t count) = 0;
    virtual void SetFirstPartitionHeaderOffset(uint32_t offset) = 0;
    virtual void SetFirstImageHeaderOffset(uint32_t offset) = 0;
    virtual void SetHeaderAuthCertificateOffset(uint32_t offset) = 0;
    virtual void SetReservedFields(void) = 0;
    virtual void SetChecksum(void) = 0;
    virtual void SetBootDevice(BootDevice::Type type) {};
    virtual void SetBootDeviceAddress(uint32_t address) {};
    virtual void ValidateSecurityCombinations(Authentication::Type, Encryption::Type, Checksum::Type) = 0;

    virtual uint32_t GetImageHeaderTableVersion(void) { return 0; }
    virtual uint32_t GetPartitionCount(void) { return 0; }
    virtual uint32_t GetImageCount(void) { return 0; }
    virtual uint32_t GetFirstPartitionHeaderOffset(void) { return 0; }
    virtual uint32_t GetFirstImageHeaderOffset(void) { return 0; }
    virtual uint32_t GetHeaderAuthCertificateOffset(void) { return 0; }
    virtual uint8_t GetMaxNumOfPartitions(void) { return 0; }
    virtual uint32_t GetTotalMetaHdrLength(void) { return 0; }

    void SetEncryptContext(EncryptionContext* ctx) { encrypt = ctx; }
    EncryptionContext* GetEncryptContext(void) { return encrypt; }

    uint32_t metaHeaderLength;
    KeySource::Type metaHdrKeySrc;
    uint8_t* metaHdrSecHdrIv;

protected:
    bool slaveBootSplitMode;
    uint32_t fullBhSize;
    uint32_t allHdrSize;
    EncryptionContext* encrypt;
};
#endif
