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

#ifndef _GENERATEPDI_H_
#define _GENERATEPDI_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <list>
#include <string>
#include "bootgenenum.h"
#include "options.h"
#include "bootimage.h"


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
enum PdiReturnStatus
{
    PASS,
    ERR_READ_PROPERTY,
    ERR_ELF_PARSING,
    ERR_ENCRYPTION,
    ERR_AUTHENTICATION,
};

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class PdiPartition
{
public:
    PdiPartition()
    {

    }

    /* Constructor for partitions like CFI/NPI */
    PdiPartition(PartitionType::Type xtype, uint8_t* buffer, uint32_t size, uint32_t id = 0)
        : encryption(Encryption::None)
        , authentication(Authentication::None)
        , checksum(Checksum::None)
        , loadAddr(0x0)
        , aesKeyFile("")
        , ppkFile("")
        , pskFile("")
        , spkFile("")
        , sskFile("")
        , partitionFile("")
        , imageName("")
        , imageId(0)
    {
        partitionType = xtype;
        partitionBuffer = new uint8_t[size];
        memcpy(partitionBuffer, buffer, size);
        partitionSize = size;
        puid = id;
        switch (xtype)
        {
            case PartitionType::CFI:
                type = PartitionType::CFI; break;
            case PartitionType::CFI_GSC:
                type = PartitionType::CFI_GSC; break;
            case PartitionType::CDO:
            case PartitionType::PMC_CDO:
            case PartitionType::NPI:
                type = PartitionType::CONFIG_DATA_OBJ; break;
            default:
                type = PartitionType::RESERVED; break;
        }
    }

    /* Constructor for partitions like elfs */
    PdiPartition(PartitionType::Type xtype, std::string filepath, uint32_t id = 0)
        : encryption(Encryption::None)
        , authentication(Authentication::None)
        , checksum(Checksum::None)
        , loadAddr(0x0)
        , aesKeyFile("")
        , ppkFile("")
        , pskFile("")
        , spkFile("")
        , sskFile("")
        , partitionBuffer(NULL)
        , partitionSize(0)
        , imageName("")
        , imageId(0)
    {
        partitionType = xtype;
        partitionFile = filepath;
        puid = id;
        switch (xtype)
        {
            case PartitionType::CFI:
                type = PartitionType::CFI; break;
            case PartitionType::CFI_GSC:
                type = PartitionType::CFI_GSC; break;
            case PartitionType::CDO:
            case PartitionType::PMC_CDO:
            case PartitionType::NPI:
                type = PartitionType::CONFIG_DATA_OBJ; break;
            default:
                type = PartitionType::RESERVED; break;
        }
    }

    virtual ~PdiPartition()
    {
        if (partitionBuffer != NULL)
            delete[] partitionBuffer;
    }

    virtual void SetEncryptionAttributes(Encryption::Type type, std::string keyFile)
    {
        encryption = type;
        aesKeyFile = keyFile;
    }

    virtual void SetAuthenticationAttributes(Authentication::Type type, std::string ppk, std::string psk, std::string spk, std::string ssk)
    {
        authentication = type;
        ppkFile = ppk;
        pskFile = psk;
        spkFile = spk;
        sskFile = ssk;
    }

    void SetType(PartitionType::Type p_type)
    {
        type = p_type;
    }

    PartitionType::Type GetPartitionType(void)
    {
        return partitionType;
    }

    virtual std::string GetType(void)
    {
        return "";
    }

    std::string GetPartitionFile(void)
    {
        return partitionFile;
    }

    uint8_t* GetPartitionBuffer(void)
    {
        return partitionBuffer;
    }

    uint32_t GetPartitionSize(void)
    {
        return partitionSize;
    }

    uint32_t GetPartitionUid(void)
    {
        return puid;
    }

    std::string GetImageName(void)
    {
        return imageName;
    }

    uint32_t GetImageId(void)
    {
        return imageId;
    }

    void SetPartitionSize(uint32_t size)
    {
        partitionSize = size;
    }

    void SetPartitionBuffer(uint8_t* ptr)
    {
        partitionBuffer = ptr;
    }

    void SetImageName(std::string name)
    {
        imageName = name;
    }

    void SetImageId(uint32_t id)
    {
        imageId = id;
    }

    virtual PdiReturnStatus SetProperty(const std::string name, const std::string &value) = 0;
    virtual PdiReturnStatus GetProperty(const std::string name, std::string &value) = 0;

protected:
    PartitionType::Type partitionType;
    PartitionType::Type type;
    Encryption::Type encryption;
    Authentication::Type authentication;
    Checksum::Type checksum;
    std::string partitionFile;
    uint8_t* partitionBuffer;
    uint32_t partitionSize;
    uint64_t loadAddr;
    std::string aesKeyFile;
    uint32_t puid;
    std::string imageName;
    uint32_t imageId;
    std::string ppkFile;
    std::string pskFile;
    std::string spkFile;
    std::string sskFile;
};

/******************************************************************************/
class VersalPdiPartition : public PdiPartition
{
public:
    VersalPdiPartition(PartitionType::Type type, uint8_t* buffer, uint32_t size, uint32_t id = 0) 
        : PdiPartition(type, buffer, size, id)
    {  }

    VersalPdiPartition(PartitionType::Type type, std::string filepath, uint32_t id = 0) 
        : PdiPartition(type, filepath, id)
    {  }

    VersalPdiPartition(const std::string& type, uint8_t* buffer, uint32_t size, uint32_t id = 0) 
        : PdiPartition(PartitionType::RAW, buffer, size, id)
    {
        ConvertStringToPartitionType(type);
    }

    VersalPdiPartition(const std::string& type, std::string filepath, uint32_t id = 0) 
        : PdiPartition(PartitionType::RAW, filepath, id)
    {
        ConvertStringToPartitionType(type);
    }

    ~VersalPdiPartition()
    {  }

    std::string GetType(void)
    {
        return ConvertPTypeToString(type);
    }

    void SetEncryptionAttributes(Encryption::Type type, std::string keyFile);
    void SetAuthenticationAttributes(Authentication::Type type, std::string ppk, std::string psk, std::string spk, std::string ssk);
    PdiReturnStatus SetProperty(const std::string name, const std::string &value);
    PdiReturnStatus GetProperty(const std::string name, std::string &value);

private:
    void ConvertStringToPartitionType(std::string type)
    {
        if (type == "BOOTLOADER") {
            partitionType = PartitionType::BOOTLOADER;
        }
        else if (type == "CDO") {
            partitionType = PartitionType::CDO;
        }
        else if (type == "PMC_CDO") {
            partitionType = PartitionType::PMC_CDO;
        }
        else if (type == "REG_INIT") {
            partitionType = PartitionType::REG_INIT;
        }
        else if (type == "RAW") {
            partitionType = PartitionType::RAW;
        }
        else if (type == "CFI") {
            partitionType = PartitionType::CFI;
        }
        else if (type == "NPI") {
            partitionType = PartitionType::NPI;
        }
        else if (type == "CFI_GSC") {
            partitionType = PartitionType::CFI_GSC;
        }
        else {
            LOG_ERROR("Incorrect partition type %s", type.c_str());
        }
    }

    std::string ConvertPTypeToString(PartitionType::Type type)
    {
        switch (type)
        {
        case PartitionType::RESERVED:
            return "RESERVED";
            break;
        case PartitionType::ELF:
            return "ELF";
            break;
        case PartitionType::CONFIG_DATA_OBJ:
            return "CDO";
            break;
        case PartitionType::CFI:
            return "CFI";
            break;
        case PartitionType::RAW:
            return "RAW";
            break;
        case PartitionType::RAW_ELF:
            return "RAW_ELF";
            break;
        case PartitionType::CFI_GSC:
            return "CFI_GSC";
            break;
        case PartitionType::CFI_GSC_UNMASK:
            return "CFI_GSC_UNMASK";
            break;
        default:
            return "Unknown partition type";
            break;
        }
    }
};

/******************************************************************************/
class PdiImage
{
public:
    PdiImage(std::string subsys_name, uint32_t subsys_id)
    {
        /* Maximum length of subsys_name is 16 characters */
        name = subsys_name;
        id = subsys_id;
    }

    uint32_t GetId(void)
    {
        return id;
    }

    std::string GetName(void)
    {
        return name;
    }

    std::list<PdiPartition*> GetPdiPartitions(void)
    {
        return partitionList;
    }

    void AddPartition(PdiPartition* part)
    {
        partitionList.push_back(part);
    }

    ~PdiImage()
    {
    }

protected:
    uint32_t id;
    std::string name;
    std::list<PdiPartition*> partitionList;
};

/******************************************************************************/
class VersalPdiImage : public PdiImage
{
public:
    VersalPdiImage(std::string name, uint32_t id) : PdiImage(name, id)
    {  }
    ~VersalPdiImage()
    {  }
};

/******************************************************************************/
class Pdi
{
public:
    Pdi(uint8_t xcreatorId = 0, uint32_t xpdiId = 0, uint32_t xparentId = 0)
    {
        encryptionKeySource = KeySource::None;
        pdiPartitions.clear();
        regInitBuffer = NULL;
        regInitSize = 0;
        readPdi = false;
        pdiId = xpdiId;
        parentId = xparentId;
        creatorId = xcreatorId;
    }

    virtual ~Pdi()
    {
        while (!pdiPartitions.empty())
        {
            delete pdiPartitions.front();
            pdiPartitions.pop_front();
        }
        while (!pdiImages.empty())
        {
            delete pdiImages.front();
            pdiImages.pop_front();
        }
    }

    virtual void SetEncryptionKeySource(KeySource::Type type) { }

    virtual PdiReturnStatus AddPartition(PdiPartition* partition)
    {
        return PdiReturnStatus::PASS;
    }

    virtual PdiReturnStatus Generate(std::string fileName, bool overwrite)
    {
        return PdiReturnStatus::PASS;
    }

    virtual PdiReturnStatus Read(std::string fileName)
    {
        return PdiReturnStatus::PASS;
    }

    std::list<PdiPartition*>  GetPdiPartitions(void)
    {
        return pdiPartitions;
    }

    std::list<PdiImage*>  GetPdiImages(void)
    {
        return pdiImages;
    }

    static std::string GetErrorMessage(void)
    {
        return errorMessage;
    }

    uint32_t GetPdiId(void)
    {
        return pdiId;
    }

    uint32_t GetParentId(void)
    {
        return parentId;
    }

    uint32_t GetCreatorId(void)
    {
        return creatorId;
    }

    virtual PdiReturnStatus SetProperty(const std::string name, const std::string &value) = 0;
    virtual PdiReturnStatus GetProperty(const std::string name, std::string &value) = 0;

    void CreateFileSpec(PartitionBifOptions* partitionBifOptions, PdiPartition* partition);
protected:
    KeySource::Type encryptionKeySource;
    std::string regInitFile;
    uint8_t* regInitBuffer;
    uint32_t regInitSize;
    std::list <PdiPartition*> pdiPartitions;
    std::list <PdiImage*> pdiImages;
    bool readPdi;
    static std::string errorMessage;
    uint32_t pdiId;
    uint32_t parentId;
    uint8_t creatorId;
};

/******************************************************************************/
class VersalPdi : public Pdi
{
public:
    VersalPdi(uint8_t creatorId = 0, uint32_t pdiId = 0, uint32_t parentId = 0)
        : Pdi(creatorId, pdiId, parentId)
    {
        pmcDataBuffer = NULL;
        pmcDataSize = eFsblSize = pmcDataLoadAddress = 0;
    }

    ~VersalPdi()
    {  }

    void SetEncryptionKeySource(KeySource::Type type);
    PdiReturnStatus AddPartition(PdiPartition* partition);
    PdiReturnStatus Generate(std::string fileName, bool overwrite);
    PdiReturnStatus Read(std::string fileName);
    PdiReturnStatus SetProperty(const std::string name, const std::string &value);
    PdiReturnStatus GetProperty(const std::string name, std::string &value);

private:
    std::string pmcDataFile;
    uint8_t* pmcDataBuffer;
    uint32_t pmcDataSize;
    uint32_t eFsblSize;
    uint64_t pmcDataLoadAddress;
};
#endif
