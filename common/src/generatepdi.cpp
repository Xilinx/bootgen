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
#include <sstream>

#include "generatepdi.h"
#include "readimage-versal.h"
#include "readimage-versal_2ve_2vm.h"
#include "readimage-spartanup.h"
#include "logger.h"
#include "bootimage-versal.h"
#include "bifoptions.h"
#include "imageheadertable-versal.h"



/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
void VersalPdiPartition::SetEncryptionAttributes(Encryption::Type type, std::string keyFile)
{
    encryption = type;
    aesKeyFile = keyFile;
}

/******************************************************************************/
void VersalPdiPartition::SetAuthenticationAttributes(Authentication::Type type, std::string ppk, 
                                                std::string psk, std::string spk, std::string ssk)
{
    authentication = type;
    ppkFile = ppk;
    pskFile = psk;
    spkFile = spk;
    sskFile = ssk;
}

/******************************************************************************/
PdiReturnStatus VersalPdiPartition::SetProperty(const std::string name, const std::string &value)
{
    if (name == "loadaddr")
    {
        loadAddr = strtoul(value.c_str(), NULL, 16);
    }
    return PdiReturnStatus::PASS;
}

/******************************************************************************/
PdiReturnStatus VersalPdiPartition::GetProperty(const std::string name, std::string &value)
{
    std::stringstream ss;

    if (name == "encryption")
    {
        ss << encryption;
    }
    else if (name == "authentication")
    {
        ss << authentication;
    }
    else if (name == "ppk")
    {
        ss << ppkFile;
    }
    else if (name == "psk")
    {
        ss << pskFile;
    }
    else if (name == "spk")
    {
        ss << spkFile;
    }
    else if (name == "ssk")
    {
        ss << sskFile;
    }
    else if (name == "checksum")
    {
        ss << checksum;
    }
    else if (name == "aeskeyfile")
    {
        ss << aesKeyFile;
    }
    else if (name == "loadaddr")
    {
        ss << loadAddr;
    }
    else
    {
        return PdiReturnStatus::ERR_READ_PROPERTY;
    }
    value = ss.str();

    return PdiReturnStatus::PASS;
}

/******************************************************************************/
void VersalPdi::SetEncryptionKeySource(KeySource::Type type)
{
    // Add error checking here
    encryptionKeySource = type;
}

/******************************************************************************/
PdiReturnStatus VersalPdi::Generate(std::string fileName, bool overwrite)
{
#if 0
    /* Populate BootGenOptions from the Pdi object
    Corresponding to command line options */
    Options pdiOptions;
    pdiOptions.SetOverwrite(overwrite);
    pdiOptions.InsertOutputFileNames(fileName);
    pdiOptions.SetArchType(Arch::VERSAL);

    /* Populate BootImage from PdiPartition objects related to diff partitions
    Corresponding to BIF partition attributes */
    VersalBootImage* pdiBootImage = new VersalBootImage(pdiOptions);

    /* This part of the code is synonymous to the BootImage::Add function */
    for (std::list<PdiPartition*>::iterator partition = pdiPartitions.begin(); partition != pdiPartitions.end(); partition++)
    {
        /* REG_INIT is not a separate partition, it is just a field in boot header.
        It will not have its own image/partition headers. So, do not add the partitions list */
        if ((*partition)->GetPartitionType() == PartitionType::REG_INIT)
        {
            if ((*partition)->GetPartitionFile() != "")
            {
                regInitFile = (*partition)->GetPartitionFile();
                pdiBootImage->bootHeader->regTable.filename = regInitFile;
            }
            else
            {
                regInitBuffer = (*partition)->GetPartitionBuffer();
                regInitSize = (*partition)->GetPartitionSize();
                if (regInitBuffer != NULL)
                {
                    pdiOptions.bifOptions->SetRegInitBuffer(regInitBuffer);
                }
            }
            continue;
        }

        /* PMC_CDO is not a separate partition, it is just appended to the PMC.
        It will not have its own image/partition headers. So, do not add the partitions list */
        if ((*partition)->GetPartitionType() == PartitionType::PMC_CDO)
        {
            if ((*partition)->GetPartitionFile() != "")
            {
                pmcDataFile = (*partition)->GetPartitionFile();
                pdiBootImage.SetPmcdataFile(pmcDataFile);
            }
            else
            {
                pmcDataBuffer = (*partition)->GetPartitionBuffer();
                pmcDataSize = (*partition)->GetPartitionSize();
                if (pmcDataBuffer != NULL)
                {
                    pdiBootImage->SetPmcDataBuffer(pmcDataBuffer, pmcDataSize);
                }
            }
            pmcDataLoadAddress = DEFAULT_PMCDATA_LOADADDR;
            std::string loadAddr;
            (*partition)->GetProperty("loadaddr", loadAddr);
            if (loadAddr != "0")
            {
                pmcDataLoadAddress = strtoul(loadAddr.c_str(), NULL, 10);
            }
            pdiBootImage->SetPmcDataLoadAddress(pmcDataLoadAddress);
            continue;
        }

        pdiBootImage->imageHeaderTable->SetCreatorId(creatorId);
        pdiBootImage->imageHeaderTable->SetPdiId(pdiId);
        pdiBootImage->imageHeaderTable->SetParentId(parentId);

        FileSpec* filespec = new FileSpec(Arch::VERSAL);
        CreateFileSpec(filespec, *partition);

        if ((*partition)->GetPartitionType() == PartitionType::BOOTLOADER)
        {
            /* Append PMC_CDO buffer to existing Bootloader buffer */
            if ((*partition)->GetPartitionBuffer() != NULL)
            {
                eFsblSize = (*partition)->GetPartitionSize();
                if (pmcDataBuffer != NULL)
                {
                    /* Change the buffer size, original size is only bootloader, now appending PMC_CDO to it */
                    uint8_t* newPartition = (uint8_t*)realloc((*partition)->GetPartitionBuffer(), eFsblSize + pmcDataSize);
                    memcpy(newPartition, (*partition)->GetPartitionBuffer(), eFsblSize);
                    memcpy(newPartition + eFsblSize, pmcDataBuffer, pmcDataSize);
                    (*partition)->SetPartitionBuffer(newPartition);
                    (*partition)->SetPartitionSize(eFsblSize + pmcDataSize);
                }
            }
        }


        // Create ImageHeader objects with the available information
        ImageHeader *image;
        if (filespec->Filename != "")
        {
            image = pdiBootImage->archFactory->getImageHeader(filespec->Filename, pdiBootImage->archFactory);
            image->setBootloader(filespec->Bootloader);
        }
        else
        {
            image = pdiBootImage->archFactory->getImageHeader((*partition)->GetPartitionBuffer(), (*partition)->GetPartitionSize(), pdiBootImage->archFactory);
            image->setBootloader(filespec->Bootloader);
            if (filespec->Bootloader)
            {
                image->setPmcDataSizeIh(pmcDataSize);
                image->setTotalPmcDataSizeIh(pmcDataSize);
                image->setFsblFwSizeIh(eFsblSize);
                image->setTotalFsblFwSizeIh(eFsblSize);
            }
        }
        image->setPartitionUid((*partition)->GetPartitionType() << 16 | (*partition)->GetPartitionUid());

        // Update partition type as per eFSBL - CFI is PL, NPI is CDO
        if ((*partition)->GetPartitionType() == PartitionType::CFI)
        {
            image->setPartitionType(PartitionType::CFI);
        }
        if (((*partition)->GetPartitionType() == PartitionType::NPI) || ((*partition)->GetPartitionType() == PartitionType::CDO))
        {
            image->setPartitionType(PartitionType::CONFIG_DATA_OBJ);
        }

        pdiBootImage->ConfigureProcessingStages(image, filespec);

        // Push each image object to the list
        pdiBootImage->imageList.push_back(image);

        if (filespec != NULL)
        {
            delete filespec;
        }
    }

    Binary cache;

    // Build and link all PDI headers
    pdiBootImage->BuildAndLink(cache);

    // Output the PDI, write out the file
    pdiBootImage->partitionOutput->Factory(fileName);
    pdiBootImage->partitionOutput->ProcessOutput(pdiOptions, cache);

    // Destroy locally created objects
    for (std::list<ImageHeader*>::iterator currentImage = pdiBootImage->imageList.begin(); currentImage != pdiBootImage->imageList.end(); )
    {
        if ((*currentImage) != NULL)
        {
            delete *currentImage;
        }
        currentImage++;
    }
    if (pdiBootImage != NULL)
    {
        delete pdiBootImage;
    }
#endif
    return PdiReturnStatus::PASS;
}

void Pdi::CreateFileSpec(PartitionBifOptions* partitionBifOptions, PdiPartition* partition)
{
    partitionBifOptions->filename = partition->GetPartitionFile();

    if (partition->GetPartitionType() == PartitionType::BOOTLOADER)
    {
        partitionBifOptions->bootloader = true;
    }

    std::string value;
    if (partition->GetProperty("authentication", value) == PdiReturnStatus::PASS)
    {
        partitionBifOptions->authType = Authentication::Type(std::stoi(value));
    }
    else
    {
        LOG_ERROR("Internal Error : Incorrect authentication property set.");
    }

    if (partition->GetProperty("encryption", value) == PdiReturnStatus::PASS)
    {
        partitionBifOptions->encryptType = Encryption::Type(std::stoi(value));
    }
    else
    {
        LOG_ERROR("Internal Error : Incorrect encryption property set.");
    }

    if (partition->GetProperty("checksum", value) == PdiReturnStatus::PASS)
    {
        partitionBifOptions->checksumType = Checksum::Type(std::stoi(value));
    }
    else
    {
        LOG_ERROR("Internal Error : Incorrect checksum property set.");
    }
}

/******************************************************************************/
PdiReturnStatus VersalPdi::SetProperty(const std::string name, const std::string &value)
{
    return PdiReturnStatus::PASS;
}

/******************************************************************************/
PdiReturnStatus VersalPdi::GetProperty(const std::string name, std::string &value)
{
    return PdiReturnStatus::PASS;
}

/******************************************************************************/
PdiReturnStatus VersalPdi::AddPartition(PdiPartition* partition)
{
    LOG_INFO("Partition is added");

    pdiPartitions.push_back(partition);

    if (partition->GetPartitionBuffer() == NULL)
    {
        LOG_INFO("File-%s", partition->GetPartitionFile().c_str());
    }
    else
    {
        LOG_INFO("Buffer-%x", partition->GetPartitionBuffer());
        LOG_INFO("Buffer Size-%d", partition->GetPartitionSize());
    }
    return PdiReturnStatus::PASS;
}

/******************************************************************************/
PdiReturnStatus VersalPdi::Read(std::string fileName)
{
   // VersalReadImage* readImage = new VersalReadImage(fileName);
   // TellurideReadImage* readImage = new TellurideReadImage(fileName);
    SpartanupReadImage* readImage = new SpartanupReadImage(fileName);
    readImage->ReadBinaryFile();
    if (pdiPartitions.size() != 0)
    {
        LOG_ERROR("Partition List is not empty");
    }
    pdiPartitions = readImage->GetPdiPartitions();
    pdiImages = readImage->GetPdiImages();
    pdiId = readImage->GetPdiId();
    parentId = readImage->GetParentId();
    creatorId = readImage->GetCreatorId();
    delete readImage;
    return PdiReturnStatus::PASS;
}
