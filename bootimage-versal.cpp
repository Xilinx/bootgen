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
#include "bootimage-versal.h"
#include "partitionheadertable-versal.h"
#include "binary-versal.h"
#include "checksum-versal.h"
#include "authentication-versal.h"
/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
VersalBootImage::VersalBootImage(Options& options) : BootImage(options)
{
    partitionHeaderList.clear();
    options.SetDefaultAlignment(16);
    bootHeader = new VersalBootHeader();
    imageHeaderTable = new VersalImageHeaderTable();
    partitionHeaderTable = new VersalPartitionHeaderTable();
    currentEncryptCtx = new NoneEncryptionContext();
    currentAuthCtx = new NoneAuthenticationContext();
    SetLegacyEncryptionFlag(true);
    partitionOutput = new VersalPartitionOutput();
    hash = new HashSha3();
    cache = new VersalBinary();
    checksumTable = new VersalChecksumTable();
    currentAuthCtx->hashType = AuthHash::Sha3;
    currentAuthCtx->hash = hash;
    partitionHeaderTable->firstSection = NULL;
    convertAieElfToCdo = true;
}

/******************************************************************************/
VersalBootImage::~VersalBootImage()
{
    if (bootHeader)
    {
        delete bootHeader;
    }
    if (imageHeaderTable)
    {
        delete imageHeaderTable;
    }
    if (partitionHeaderTable)
    {
        delete partitionHeaderTable;
    }
    if (currentEncryptCtx)
    {
        delete currentEncryptCtx;
    }
    if (currentAuthCtx)
    {
        delete currentAuthCtx;
    }
    if (partitionOutput)
    {
        delete partitionOutput;
    }
    if (hash)
    {
        delete hash;
    }
}

/******************************************************************************/
void VersalBootImage::ConfigureEncryptionContext(ImageHeader * image, Encryption::Type encryptType)
{
    /* Configure the encryption context */
    bool aesKeyFileExists = false;
    switch (encryptType)
    {
    case Encryption::AES:
        if (image->IsBootloader())
        {
            switch (image->GetEncryptionKeySrc())
            {
            case KeySource::EfuseRedKey:
            case KeySource::BbramRedKey:
            case KeySource::EfuseBlkKey:
            case KeySource::BbramBlkKey:
            case KeySource::BhBlkKey:
            case KeySource::EfuseGryKey:
            case KeySource::BbramGryKey:
            case KeySource::BhGryKey:
            case KeySource::None:
                break;
            default:
                LOG_ERROR("BIF attribute error !!!\n\t\tInvalid 'keysrc' mentioned for Bootloader.");
                break;
            }
            bootloaderEncrypt = true;
            bootloaderKeySource = image->GetEncryptionKeySrc();
        }

        if (bootloaderFound && !bootloaderEncrypt && !bootloaderAuthenticate)
        {
            LOG_ERROR("Bootloader must be encrypted or atleast authenticated to encrypt rest of the partitions");
        }

        if (image->GetEncryptionKeySrc() == KeySource::None)
        {
            //LOG_ERROR("Key Source is mandatory with encryption for partition - %s.", image->GetName().c_str());
        }

        if (image->GetAesKeyFile() != "")
        {
            /* If no key file found in partition specific attributes - ERROR
            If specified key file non-existent - Set generateAesKeyFile flag */
            std::ifstream keyFile(image->GetAesKeyFile());
            aesKeyFileExists = keyFile.good();
            if (!aesKeyFileExists)
            {
                image->SetAesKeyFileGeneration(true);
            }
        }
        options.bifOptions->SetHeaderEncryption(true);
        if (imageHeaderTable->GetEncryptContext() == NULL)
        {
            imageHeaderTable->SetEncryptContext(new VersalEncryptionContext(this->currentEncryptCtx));
        }
        image->SetEncryptContext(new VersalEncryptionContext(this->currentEncryptCtx));
        break;

    case Encryption::None:
        if (image->GetEncryptionKeySrc() != KeySource::None)
        {
            //LOG_ERROR("Key Source cannot be specified without encryption. Check partition - %s.",image->GetName().c_str());
        }

        image->ValidateEncryptionAttributes(*this);
        if (options.bifOptions->metaHdrAttributes.encrypt != Encryption::None)
        {
            if (options.bifOptions->metaHdrAttributes.encrKeySource == KeySource::None)
            {
                LOG_ERROR("Key Source is mandatory when encrypting meta header");
            }
            options.bifOptions->SetHeaderEncryption(true);
            if (imageHeaderTable->GetEncryptContext() == NULL)
            {
                imageHeaderTable->SetEncryptContext(new VersalEncryptionContext());
            }
        }
        image->SetEncryptContext(new NoneEncryptionContext());
        break;

    default:
        image->SetEncryptContext(NULL);
        break;
    }
}

/******************************************************************************/
void VersalBootImage::ConfigureAuthenticationContext(ImageHeader * image, Authentication::Type authType, PartitionBifOptions * partitionbifoptions)
{
    /* Configure the authentication context */
    std::string spkSignFile = options.GetSpkSigFileName();
    switch (authType)
    {
    case Authentication::RSA:
    case Authentication::ECDSA:
    {
        if (image->IsBootloader())
        {
            bootloaderAuthenticate = true;
        }
        if (bootloaderFound && !bootloaderAuthenticate)
        {
            LOG_ERROR("Bootloader must be authenticated to authenticate rest of the partitions");
        }
        image->SetAuthenticationType(authType);
        options.bifOptions->SetHeaderAC(true);
        currentAuthCtx->hashType = GetAuthHashAlgo();
        currentAuthCtx->ppkFile = image->GetPpkFile();
        currentAuthCtx->pskFile = image->GetPskFile();
        currentAuthCtx->spkFile = image->GetSpkFile();
        currentAuthCtx->sskFile = image->GetSskFile();
        currentAuthCtx->spkSignFile = image->GetSpkSignFile();
        currentAuthCtx->spkIdentification = image->GetPartitionRevocationId();

        if (image->GetBhSignFile() != "")
        {
            currentAuthCtx->bhSignFile = image->GetBhSignFile();
        }

        AuthenticationContext::SetRsaKeyLength(RSA_4096_KEY_LENGTH);
        if (authType == Authentication::ECDSA)
        {
            AuthenticationContext::SetRsaKeyLength(EC_P384_KEY_LENGTH);
        }

        image->SetAuthContext(new VersalAuthenticationContext(currentAuthCtx, authType));
        AuthenticationContext* authCtx = image->GetAuthContext();
        authCtx->SetPresignFile(partitionbifoptions->presignFile);
        authCtx->SetUdfFile(partitionbifoptions->udfDataFile);
        if (spkSignFile != "")
        {
            currentAuthCtx->GenerateSPKSignature(spkSignFile);
            currentAuthCtx->spkSignRequested = spkSignFile;
        }
    }
    break;

    case Authentication::None:
    default:
    {
        if (options.bifOptions->metaHdrAttributes.authenticate != Authentication::None)
        {
            if (image->IsBootloader())
            {
                LOG_ERROR("Bootloader must be authenticated to authenticate the Meta Header");
            }
            options.bifOptions->SetHeaderAC(true);
            AuthenticationContext* headerAuthCtx = new VersalAuthenticationContext(currentAuthCtx, options.bifOptions->metaHdrAttributes.authenticate);
            headerAuthCtx->hashType = authHash;
            headerAuthCtx->hash = hash;
            currentAuthCtx = headerAuthCtx;
        }
        image->SetAuthenticationType(Authentication::None);
        image->SetAuthContext(new NoneAuthenticationContext());
        if (spkSignFile != "")
        {
            currentAuthCtx->GenerateSPKSignature(spkSignFile);
            currentAuthCtx->spkSignRequested = spkSignFile;
        }
    }
    break;
    }
}

/******************************************************************************/
void VersalBootImage::ConfigureChecksumContext(ImageHeader * image, Checksum::Type checksumType)
{
    /* Configure the checksum context */
    switch (checksumType)
    {
    case Checksum::MD5:
    case Checksum::SHA2:
        LOG_ERROR("BIF attribute error !!!\n\t\t 'checksum=sha2/md5' is not supported in VERSAL architecture.");
        break;

    case Checksum::SHA3:
        image->SetChecksumContext(new VersalSHA3ChecksumContext());
        break;

    case Checksum::None:
    default:
        image->SetChecksumContext(new NoneChecksumContext());
        break;
    }
}

/******************************************************************************/
void VersalBootImage::ParseBootImage(PartitionBifOptions* it)
{
    LOG_INFO("Importing BootImage...");
    std::string baseFile = StringUtils::BaseName(it->filename);
    std::ifstream src(it->filename.c_str(), std::ios::binary);
    bool full_pdi = true;

    if (!src.good())
    {
        LOG_ERROR("Failure in reading bootimage file for import - %s ", baseFile.c_str());
    }

    VersalBootHeader* importedBh = new VersalBootHeader(src);
    if (importedBh->GetIdentificationWord() != HEADER_ID_WORD)
    {
        LOG_WARNING("File %s is not a full PDI. It is missing the boot header", baseFile.c_str());
        full_pdi = false;
        bootloaderFound = false;
        uint32_t firstWord = *importedBh->section->Data;
        if ((firstWord == 0xDD000000) || (firstWord == 0x00DD0000) || (firstWord == 0x0000DD00) || (firstWord == 0x0000000DD))
        {
            src.seekg(sizeof(VersalSmapWidthTable));
        }
        else
        {
            src.seekg(0);
        }
    }
    else
    {
        if (importedBh->GetHeaderVersion() != 0xFFFFFFFF)
        {
            LOG_WARNING("This version of bootgen may not support the bootimage header in %s ", baseFile.c_str());
            LOG_WARNING("The version of the imported header is 0x%X, and this version of bootgen only supports 0x%X", importedBh->GetHeaderVersion(), BOOTHEADER_VERSION);
        }

        /* Authentication attribues to be appended to the attributes from stage2-Encrypted Image */
        uint32_t attributes = importedBh->GetBHAttributes();
        if (options.bifOptions->GetBhRsa() != BhRsa::BhRsaDisable)
        {
            attributes |= options.bifOptions->GetBhRsa() << BH_RSA_BIT_SHIFT;
        }
        if (authHash != AuthHash::Sha3)
        {
            attributes |= authHash << AUTH_HASH_BIT_SHIFT;
        }
        if (options.bifOptions->GetDpaCM() != DpaCM::DpaCMDisable)
        {
            attributes |= options.bifOptions->GetDpaCM() << DPA_CM_BIT_SHIFT;
        }

        importedBh->SetBHAttributes(attributes);

        /* If imported header has a non-zero source offset, it must be a FSBL header, so it must be copied */
        if (importedBh->GetSourceOffset() != 0)
        {
            LOG_INFO("Copying bootheader from %s ", baseFile.c_str());
            SetAssumeEncryptionFlag(false);
            options.SetEncryptedKeySource(importedBh->GetEncryptionKeySource());
            bootHeader->Copy(importedBh);
        }

        src.seekg(importedBh->GetImageHeaderByteOffset());
    }

    CheckForIhtAttributes(baseFile);
    imageHeaderTable = new VersalImageHeaderTable(src);
    long offset = imageHeaderTable->GetFirstImageHeaderOffset() * sizeof(uint32_t);
    uint32_t imageCount = imageHeaderTable->GetImageCount();

    bool bootloader_found = false;
    do
    {
        imageCount--;
        bool IsBootloader = false;

        /* IsBootloader flag is important to rearrange the sections in the StackAndAlign stage
           This flag needs to be passed on while creating the image header, and from there it is 
           populated in partition headers, sections etc. */
        if (full_pdi)
        {
            if (bootloader_found == false)
            {
                IsBootloader = true;
                bootloader_found = true;
            }
        }
        src.seekg(offset);
        ImageHeader* image = new VersalImageHeader(src, IsBootloader);

        /* Local AES keys has more priority than global AES key, if both specified */
        image->SetAesKeyFile(options.GetEncryptionKeyFile());
        if (it->aesKeyFile != "")
        {
            image->SetAesKeyFile(it->aesKeyFile);
        }
        ConfigureProcessingStages(image, it);
        imageList.push_back(image);

        /* Image is just temporary, we need to get a pointer back to the copied object and relink */
        imageList.back()->Relink();
        ImageHeader* newImage = imageList.back();

        /* For replacing pmcdata */
        if (IsBootloader)
        {
            image->SetTotalFsblFwSizeIh(importedBh->GetTotalPmcFwLength());
            image->SetFsblFwSizeIh(importedBh->GetPmcFwLength());
            if (importedBh->GetTotalPmcCdoLength() != 0 && options.bifOptions->GetPmcdataFile() == "")
            {
                options.bifOptions->SetTotalpmcdataSize(importedBh->GetTotalPmcCdoLength());
                options.bifOptions->pmcdataSize = importedBh->GetPmcCdoLength();
                options.bifOptions->pmcDataBuffer = new uint8_t[options.bifOptions->GetTotalpmcdataSize()];
                memcpy(options.bifOptions->pmcDataBuffer, newImage->GetPartitionHeaderList().front()->partition->section->Data + importedBh->GetTotalPmcFwLength(), options.bifOptions->totalpmcdataSize);
                image->SetPmcDataSizeIh(options.bifOptions->pmcdataSize);
                image->SetTotalPmcDataSizeIh(options.bifOptions->GetTotalpmcdataSize());
            }
        }

        /* Determine if we need to load in previous RSAAuthentication Information */
        for (std::list<PartitionHeader*>::iterator partHdr = newImage->GetPartitionHeaderList().begin(); partHdr != newImage->GetPartitionHeaderList().end(); partHdr++)
        {
            PartitionHeader* ph = (*partHdr);
            uint32_t encrPartLen = ph->GetEncryptedPartitionLength();
            size_t imageAuthBlock = image->GetAuthBlock();
            int acSize = 1;
            if (imageAuthBlock != 0)
            {
                acSize = encrPartLen / (imageAuthBlock * 1024 * 1024);
                if (encrPartLen % imageAuthBlock != 0)
                {
                    acSize++;
                }
            }
            if (ph->IsAuthCertPresent())
            {
                LOG_INFO("Loading AC context for section %s ", ph->section->Name.c_str());
                options.bifOptions->SetHeaderAC(true);

                // load in previous certificate data
                for (int i = 0; i < acSize; i++)
                {
                    void *cert = (ph->partition->section->Data + ph->GetCertificateRelativeByteOffset());
                    VersalAuthenticationContext* auth = new VersalAuthenticationContext((AuthCertificate4096Sha3PaddingStructure*)cert, image->GetAuthenticationType());
                    AuthenticationCertificate* tempac;
                    tempac = new VersalAuthenticationCertificate(auth);
                    auth->preSigned = true;
                    tempac->fsbl = true;
                    ph->ac.push_back(tempac);
                    newImage->SetAuthContext(auth);
                }
            }
        }
        offset += sizeof(VersalImageHeaderStructure);
    } while (imageCount != 0);
}

/******************************************************************************/
void VersalBootImage::CheckForIhtAttributes(std::string base_file)
{
    if (options.bifOptions->GetPdiId() != 0)
    {
        LOG_WARNING("PDI id is taken from base PDI %s, ignoring PDI 'id' specified in the BIF", base_file.c_str());
    }

    if (options.bifOptions->GetParentId())
    {
        LOG_WARNING("Parent id is taken from base PDI %s, ignoring 'parent_id' specified in the BIF", base_file.c_str());
    }

    if (options.bifOptions->GetIdCode())
    {
        LOG_WARNING("ID code is taken from base PDI %s, ignoring 'id_code' specified in the BIF", base_file.c_str());
    }

    if (options.bifOptions->GetExtendedIdCode())
    {
        LOG_WARNING("Extended ID code is taken from base PDI %s, ignoring 'extended_id_code' specified in the BIF", base_file.c_str());
    }

    if (options.bifOptions->GetBypassIdcodeFlag())
    {
        //LOG_WARNING("ID code check is taken from base PDI %s, ignoring 'bypass_idcode_check' specified in the BIF", base_file.c_str());
    }

    if (options.bifOptions->GetBootDevice() != BootDevice::DEFAULT)
    {
        LOG_WARNING("Boot Device is taken from base PDI %s, ignoring 'boot_device' specified in the BIF", base_file.c_str());
    }
}

/******************************************************************************/
void VersalBootImage::ValidateSecureAttributes(ImageHeader * image, BifOptions * bifoptions, PartitionBifOptions * partitionBifOptions)
{
    /* Error checks for diff modes */
    switch (partitionBifOptions->encryptType)
    {
    case Encryption::AES:
        if (XipMode)
        {
            LOG_ERROR("Encryption not supported in XIP Mode");
        }

        if (image->IsBootloader())
        {
            if (bifoptions->GetAuthOnly() == AuthOnly::Enabled)
            {
                LOG_ERROR("Cannot encrypt bootloader, when 'auth_only' attribute is used");
            }
        }
        break;
    default:
        break;
    }

    switch (partitionBifOptions->authType)
    {
    case Authentication::RSA:
    {
        if (XipMode)
        {
            LOG_ERROR("Authentication not supported in XIP Mode");
        }
    }
    break;

    case Authentication::None:
    default:
    {
        if (partitionBifOptions->presignFile != "")
        {
            LOG_ERROR("Cannot specify 'presign' attribute when Authentication is not enabled");
        }
        if ((image->IsBootloader()) && (bifoptions->GetAuthOnly() == AuthOnly::Enabled))
        {
            LOG_ERROR("Bootloader must be authenticated if auth_only is used in {boot_config} attribute.");
        }
    }
    break;
    }
}

/******************************************************************************/
ImageHeader* VersalBootImage::ParsePartitionDataToImage(BifOptions * bifoptions, PartitionBifOptions * partitionBifOptions)
{
    static int aie_elf_cnt = 0;
    static uint8_t slr_boot_cnt = 0;
    static uint8_t slr_cfg_cnt = 0;
    static std::list<SlrPdiInfo*> slrBootPdiInfo;
    static std::list<SlrPdiInfo*> slrConfigPdiInfo;

    ImageHeader *image = new VersalImageHeader(partitionBifOptions->filename);
    image->SetBootloader(partitionBifOptions->bootloader);
    image->SetAlignment(partitionBifOptions->alignment);
    image->SetOffset(partitionBifOptions->offset);
    image->SetReserve(partitionBifOptions->reserve);
    image->SetLoad(partitionBifOptions->load);
    image->SetStartup(partitionBifOptions->startup);

    image->SetBootFlag(partitionBifOptions->boot);
    image->SetMultibootFlag(partitionBifOptions->multiboot);
    image->SetNoAutoStartFlag(partitionBifOptions->noautostart);
    image->SetProtectedFlag(partitionBifOptions->Protected);
    image->SetStaticFlag(partitionBifOptions->Static);
    image->SetUserFlag(partitionBifOptions->user);

    image->SetDestCpu(partitionBifOptions->destCPUType);
    image->SetExceptionLevel(partitionBifOptions->exceptionLevel);
    image->SetTrustZone(partitionBifOptions->trustzone);
    image->SetEarlyHandoff(partitionBifOptions->early_handoff);
    image->SetHivec(partitionBifOptions->hivec);
    image->SetPartitionType(partitionBifOptions->partitionType);
    image->SetPartitionUid(partitionBifOptions->partitionId);
    image->SetEncryptionKeySrc(partitionBifOptions->keySrc);
    image->SetPartitionRevocationId(partitionBifOptions->GetRevokeId());
    image->SetDpacm(partitionBifOptions->dpaCM);
    image->SetPufHdLocation(partitionBifOptions->pufHdLoc);

    if ((bifoptions->GetDpaCM() == DpaCM::DpaCMEnable) && (image->IsBootloader()))
    {
        image->SetDpacm(DpaCM::DpaCMEnable);
    }
    if ((bifoptions->GetPufHdLoc() == PufHdLoc::PUFinBH) && (image->IsBootloader()))
    {
        image->SetPufHdLocation(PufHdLoc::PUFinBH);
    }

    if (image->GetPufHdLocation() == PufHdLoc::PUFinBH)
    {
        bifOptions->SetPufHdinBHFlag();
    }
    /* Local key files/signatures within partition attributes have more priority than global key/signature files,
    if both specified */
    image->SetPpkFile(bifoptions->GetPPKFileName());
    if (partitionBifOptions->ppkFile != "")
    {
        image->SetPpkFile(partitionBifOptions->ppkFile);
    }

    image->SetPskFile(bifoptions->GetPSKFileName());
    if (partitionBifOptions->pskFile != "")
    {
        image->SetPskFile(partitionBifOptions->pskFile);
    }

    image->SetSpkFile(bifoptions->GetSPKFileName());
    if (partitionBifOptions->spkFile != "")
    {
        image->SetSpkFile(partitionBifOptions->spkFile);
    }

    image->SetSskFile(bifoptions->GetSSKFileName());
    if (partitionBifOptions->sskFile != "")
    {
        image->SetSskFile(partitionBifOptions->sskFile);
    }

    image->SetSpkSignFile(bifoptions->GetSPKSignFileName());
    if (partitionBifOptions->spkSignatureFile != "")
    {
        image->SetSpkSignFile(partitionBifOptions->spkSignatureFile);
    }

    /* Local AES key file is mandatory when the partition is encrypted */
    if (partitionBifOptions->aesKeyFile != "")
    {
        image->SetAesKeyFile(partitionBifOptions->aesKeyFile);
    }
    else if ((options.GetEncryptionKeyFile() != "") && (image->IsBootloader()))
    {
        LOG_WARNING("This usage of 'aeskeyfile' is deprecated. \n           Please refer 'bootgen -arch versal -bif_help aeskeyfile' for more info.");
        image->SetAesKeyFile(options.GetEncryptionKeyFile());
    }
    /* If no key file found in partition specific attributes, generate aeskeyfile with partition_name.nky */
    else
    {
        std::string name = StringUtils::RemoveExtension(image->GetName()) + ".nky";
        image->SetAesKeyFile(name);
    }

    if (image->IsBootloader())
    {
        if (partitionBifOptions->keySrc != KeySource::None)
        {
            options.SetEncryptedKeySource(partitionBifOptions->keySrc);
        }

        if (bootloaderFound == true)
        {
            LOG_ERROR("BIF attribute error !!!\n\t\tA bootimage cannot have more than one bootloader.");
        }
        bootloaderFound = true;
        fsblFilename = partitionBifOptions->filename;
        options.fsblFilename = partitionBifOptions->filename;
        if (partitionBifOptions->hivec)
        {
            LOG_ERROR("BIF attribute error !!!\n\t\t'hivec' not supported for bootloader partition");
        }
    }

    ConfigureEncryptionBlocks(image, partitionBifOptions);
    ConfigureProcessingStages(image, partitionBifOptions);
    ValidateSecureAttributes(image, bifoptions, partitionBifOptions);

    if ((partitionBifOptions->partitionType == PartitionType::SLR_BOOT) || (partitionBifOptions->partitionType == PartitionType::SLR_CONFIG))
    {
        /* SSIT devices */
        SlrPdiInfo* slrPdi = new SlrPdiInfo;
        slrPdi->file = partitionBifOptions->filename;
        std::ifstream s(slrPdi->file.c_str());
        if (!s)
        {
            LOG_ERROR("Cannot read file - %s ", slrPdi->file.c_str());
        }
        slrPdi->index = (SlrId::Type) partitionBifOptions->slrNum;
        if (partitionBifOptions->partitionType == PartitionType::SLR_BOOT)
        {
            slr_boot_cnt++;
            if (partitionBifOptions->slrNum == 0xFF)
            {
                slrPdi->index = (SlrId::Type) slr_boot_cnt;
            }
            slrPdi->type = SlrPdiType::BOOT;
            slrBootPdiInfo.push_back(slrPdi);
            if (slr_boot_cnt == bifoptions->slrBootCnt)
            {
                image->SetSlrBootPartitions(slrBootPdiInfo);
                image->SetName("SSIT Boot Partition");
                imageList.push_back(image);
            }
        }
        else
        {
            slrPdi->type = SlrPdiType::CONFIG;
            slr_cfg_cnt++;
            slrPdi->index = (SlrId::Type) partitionBifOptions->slrNum;
            if (partitionBifOptions->slrNum == 0xFF)
            {
                slrPdi->index = (SlrId::Type) slr_cfg_cnt;
                if (slrPdi->index == bifoptions->slrConfigCnt)
                {
                    slrPdi->index = SlrId::MASTER;
                }
            }
            else
            {
                slrPdi->index = (partitionBifOptions->slrNum == 0x0) ? (SlrId::MASTER) : ((SlrId::Type) partitionBifOptions->slrNum);
            }
            slrConfigPdiInfo.push_back(slrPdi);
            if (slr_cfg_cnt == bifoptions->slrConfigCnt)
            {
                image->SetSlrConfigPartitions(slrConfigPdiInfo);
                image->SetName("SSIT Config Partition");
                imageList.push_back(image);
            }
        }
    }
    else if ((convertAieElfToCdo == true) && (partitionBifOptions->destCPUType == DestinationCPU::AIE))
    {
        /* ELF to CDO flow - All ELFs are converted to one CDO
        Push all the AIE elfs to a list */
        aie_elf_cnt++;
        bifoptions->aie_elfs.push_back(partitionBifOptions->filename);
        if (aie_elf_cnt == 1)
        {
            imageList.push_back(image);
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        imageList.push_back(image);
    }

    bool break_outer_loop = false;

    for (std::list<SubSysImageHeader*>::iterator subSysHdr = subSysImageList.begin(); subSysHdr != subSysImageList.end(); subSysHdr++)
    {
        for (std::list<std::string>::iterator partName = (*subSysHdr)->partitionNameList.begin(); partName != (*subSysHdr)->partitionNameList.end(); partName++)
        {
            image->SetMemCopyAddress((*subSysHdr)->GetSubSysMemCopyAddress());
            image->SetDelayLoadHandOffFlags((*subSysHdr)->GetDelayLoadMode(), (*subSysHdr)->GetDelayHandoffMode());
            std::string part(*partName);
            if (!image->GetFilename().compare(part))
            {
                (*subSysHdr)->imgList.push_back(image);
                break_outer_loop = true;
                break;
            }
        }
        if (break_outer_loop)
        {
            break;
        }
    }
    return image;
}

/******************************************************************************/
void VersalBootImage::OutputOptionalSecureDebugImage()
{

}

/******************************************************************************/
void VersalBootImage::SetPmcdataFile(const std::string& filename)
{
    options.bifOptions->SetPmcdataFile(filename);
}

/******************************************************************************/
void VersalBootImage::ConfigureProcessingStages(ImageHeader* image, PartitionBifOptions* partitionbifoptions)
{
    ConfigureEncryptionContext(image, partitionbifoptions->encryptType);
    ConfigureAuthenticationContext(image, partitionbifoptions->authType, partitionbifoptions);
    ConfigureChecksumContext(image, partitionbifoptions->checksumType);
    image->SetPartOwner(partitionbifoptions->ownerType);
}

/******************************************************************************/
void VersalBootImage::SetPmcDataLoadAddress(Binary::Address_t addr)
{
    options.bifOptions->pmcCdoLoadAddress = addr;
}

/******************************************************************************/
void VersalBootImage::Add(BifOptions* bifoptions)
{
    if (bifoptions->GetAESKeyFileName() != "")
    {
        currentEncryptCtx->SetAesFileName(bifoptions->GetAESKeyFileName());
        options.SetEncryptionKeyFile(bifoptions->GetAESKeyFileName());
    }

    currentEncryptCtx->SetMetalKeyFile(bifoptions->GetFamilyKeyFileName());
    currentAuthCtx->SetSpkIdentification(bifoptions->GetSpkId());
    currentAuthCtx->SetHeaderAuthentication(bifoptions->GetHeaderAC());

    if (options.GetAuthKeyGeneration() == GenAuthKeys::None)
    {
        if (bifoptions->GetPPKFileName() != "")
        {
            currentAuthCtx->SetPPKeyFile(bifoptions->GetPPKFileName());
        }

        if (bifoptions->GetPSKFileName() != "")
        {
            currentAuthCtx->SetPSKeyFile(bifoptions->GetPSKFileName());
        }

        if (bifoptions->GetSPKFileName() != "")
        {
            currentAuthCtx->SetSPKeyFile(bifoptions->GetSPKFileName());
            if (options.DoGenerateHashes())
            {
                LOG_INFO("Generating SPK Hash File");
                currentAuthCtx->GenerateSPKHashFile(bifoptions->GetSPKFileName(), hash);
            }
        }

        if (bifoptions->GetSSKFileName() != "")
        {
            currentAuthCtx->SetSSKeyFile(bifoptions->GetSSKFileName());
        }
    }

    if (bifoptions->GetSPKSignFileName() != "")
    {
        LOG_TRACE("Parsing SPK Signature File");
        currentAuthCtx->SetSPKSignatureFile(bifoptions->GetSPKSignFileName());
    }

    if (bifoptions->GetBHSignFileName() != "")
    {
        LOG_TRACE("Parsing BH Signature File");
        currentAuthCtx->SetBHSignatureFile(bifoptions->GetBHSignFileName());
    }

    currentEncryptCtx->SetBHKekIVFile(bifoptions->GetBHKekIVFile());
    XipMode = bifoptions->GetXipMode();

    LOG_INFO("Parsing Partition Data to Image");
    if (bifoptions->imageBifOptionList.size() == 0)
    {
        for (std::list<PartitionBifOptions*>::iterator itr = bifoptions->partitionBifOptionList.begin(); itr != bifoptions->partitionBifOptionList.end(); itr++)
        {
            if ((*itr)->bootImage)
            {
                ParseBootImage((*itr));
            }
            else if ((*itr)->pmcData)
            {
                SetPmcdataFile((*itr)->filename);
                if ((*itr)->load.Value() != 0)
                {
                    bifoptions->pmcCdoLoadAddress = (*itr)->load.Value();
                }
                if ((*itr)->aesKeyFile != "")
                {
                    bifoptions->pmcDataAesFile = (*itr)->aesKeyFile;
                }
                //If no key file found in partition specific attributes - Generate aeskeyfile with partition_name.nky
                else
                {
                    bifoptions->pmcDataAesFile = StringUtils::RemoveExtension(StringUtils::BaseName((*itr)->filename)) + ".nky";
                }
            }
            else
            {
                ParsePartitionDataToImage(bifOptions, *itr);
            }
        }
    } 
    else
    {
        for (std::list<ImageBifOptions*>::iterator imgitr = bifoptions->imageBifOptionList.begin(); imgitr != bifoptions->imageBifOptionList.end(); imgitr++)
        {
            SubSysImageHeader *subSysImage = new SubSysImageHeader(*imgitr);
            bool bootimage_partition = false;
            for (std::list<PartitionBifOptions*>::iterator partitr = (*imgitr)->partitionBifOptionsList.begin(); partitr != (*imgitr)->partitionBifOptionsList.end(); partitr++)
            {
                if ((*partitr)->bootImage)
                {
                    ParseBootImage((*partitr));
                    bootimage_partition = true;
                }
                else if ((*partitr)->pmcData)
                {
                    SetPmcdataFile((*partitr)->filename);
                    if ((*partitr)->load.Value() != 0)
                    {
                        bifoptions->pmcCdoLoadAddress = (*partitr)->load.Value();
                    }
                    if ((*partitr)->aesKeyFile != "")
                    {
                        bifoptions->pmcDataAesFile = (*partitr)->aesKeyFile;
                    }
                    else
                    {
                        //If no key file found in partition specific attributes - Generate aeskeyfile with partition_name.nky
                        bifoptions->pmcDataAesFile = StringUtils::RemoveExtension(StringUtils::BaseName((*partitr)->filename)) + ".nky";
                    }
                }
                else
                {
                    ImageHeader* img = ParsePartitionDataToImage(bifOptions, *partitr);
                    if (img != NULL)
                    {
                        if (subSysImage->GetDelayHandoffMode() || subSysImage->GetDelayLoadMode())
                        {
                            if (img->IsBootloader())
                            {
                                LOG_ERROR("BIF attributes 'delay_load'/'delay_handoff' not supported for PMC subsystem");
                            }
                        }
                        subSysImage->imgList.push_back(img);
                    }
                }
            }
            /* Add to subsys list only if it the partition type is not bootimage. Because bootimage will have its own subsystems */
            if (!bootimage_partition)
            {
                subSysImageList.push_back(subSysImage);
            }
        }
    }
}

/******************************************************************************/
void VersalBootImage::ReplaceImages(void)
{
    std::list<ImageHeader*>::iterator it1, it2, PrevImgloc;
    std::list<SubSysImageHeader*>::iterator subsysIt;
    bool replaceImages = false;
    bool replacePSMImages = false;
    ImageHeader* img1 = NULL;
    ImageHeader* PSMimg = NULL;
    uint32_t tempindx = 0;

    // Check for bootloader, if there are more bootloaders in the bif, other than the one in bootimage, replace it.
    uint8_t bootloaderCnt = 0;
    uint8_t PSMImageCnt = 0;

    for (std::list<SubSysImageHeader*>::iterator subsys = subSysImageList.begin(); subsys != subSysImageList.end(); subsys++)
    {
        for (std::list<ImageHeader*>::iterator img = (*subsys)->imgList.begin(); img != (*subsys)->imgList.end(); img++)
        {
            if ((*img)->GetDestCpu() == DestinationCPU::PMU)
            {
                PSMImageCnt++;
            }
            if (PSMImageCnt > 1)
            {
                replacePSMImages = true;
            }

            if ((*img)->IsBootloader())
            {
                if (bootloaderCnt == 0)
                {
                    it1 = img;
                }
                bootloaderCnt++;
                bootloaderFound = true;
            }
            if (bootloaderCnt > 1)
            {
                replaceImages = true;
                img1 = (*img);
                subsysIt = subsys;
            }
        }
    }
    bootloaderCnt = 0;
    if (replaceImages == true)
    {
        for (std::list<SubSysImageHeader*>::iterator subsys = subSysImageList.begin(); subsys != subSysImageList.end(); subsys++)
        {
            for (std::list<ImageHeader*>::iterator img = (*subsys)->imgList.begin(); img != (*subsys)->imgList.end(); img++)
            {
                tempindx++;
                if ((*img)->IsBootloader())
                {
                    (*subsys)->imgList.erase(it1);
                    (*subsys)->imgList.push_front((img1));
                    break;
                }
            }
            subSysImageList.erase(subsysIt);
            break;
        }
    }

    tempindx = 0;
    PSMImageCnt = 0;
    if (replacePSMImages == true)
    {
        for (std::list<SubSysImageHeader*>::iterator subsys = subSysImageList.begin(); subsys != subSysImageList.end(); subsys++)
        {
            for (std::list<ImageHeader*>::iterator img = (*subsys)->imgList.begin(); img != (*subsys)->imgList.end(); img++)
            {
                tempindx++;
                if ((*img)->GetDestCpu() == DestinationCPU::PMU)
                {
                    PSMImageCnt++;
                    if (PSMImageCnt == 2)
                    {
                        PSMimg = *img;
                        (*subsys)->imgList.remove(*img);
                        break;
                    }
                    else
                    {
                        PrevImgloc = img;
                    }
                }
            }
            (*subsys)->imgList.insert(PrevImgloc, PSMimg);
            (*subsys)->imgList.erase(PrevImgloc);
        }
    }
}

/******************************************************************************/
void VersalBootImage::ReplaceImagesNoSubSys(void)
{
    std::list<ImageHeader*> oldList = imageList;
    std::list<ImageHeader*> newList;
    std::list<ImageHeader*>::iterator it1, it2, PrevImgloc;
    it1 = it2 = imageList.begin();
    bool replaceImages = false;
    bool replacePSMImages = false;
    ImageHeader* tempBl = NULL;
    uint32_t tempindx = 0;
    ImageHeader* PSMimg = NULL;
    // Check for bootloader, if there are more bootloaders in the bif, other than the one in bootimage, replace it.
    uint8_t bootloaderCnt = 0;
    uint8_t PSMImageCnt = 0;
    for (std::list<ImageHeader*>::iterator img = imageList.begin(); img != imageList.end(); img++)
    {
        if ((*img)->GetDestCpu() == DestinationCPU::PMU)
        {
            PSMImageCnt++;
        }
        if (PSMImageCnt > 1)
        {
            replacePSMImages = true;
        }

        if ((*img)->IsBootloader())
        {
            bootloaderCnt++;
            bootloaderFound = true;
        }
        if (bootloaderCnt > 1)
        {
            replaceImages = true;
        }
    }

    bootloaderCnt = 0;
    if (replaceImages == true)
    {
        for (std::list<ImageHeader*>::iterator img = imageList.begin(); img != imageList.end(); img++)
        {
            tempindx++;
            if ((*img)->IsBootloader())
            {
                bootloaderCnt++;
                if (bootloaderCnt == 2)
                {
                    tempBl = *img;
                    imageList.remove(*img);
                    break;
                }
            }
        }
        imageList.erase(it1);
        if(tempBl != NULL)
        {
            imageList.push_front(tempBl);
        }
    }

    tempindx = 0;
    PSMImageCnt = 0;
    if (replacePSMImages == true)
    {
        for (std::list<ImageHeader*>::iterator img = imageList.begin(); img != imageList.end(); img++)
        {
            tempindx++;
            if ((*img)->GetDestCpu() == DestinationCPU::PMU)
            {
                PSMImageCnt++;
                if (PSMImageCnt == 2)
                {
                    PSMimg = *img;
                    imageList.remove(*img);
                    break;
                }
                else {
                    PrevImgloc = img;
                }
            }
        }
        imageList.insert(PrevImgloc, PSMimg);
        imageList.erase(PrevImgloc);
    }
}

/******************************************************************************/
void VersalBootImage::OutputOptionalEfuseHash()
{
    std::string hashFile = options.GetEfuseHashFileName();
    std::string primaryKeyFile;
    if (hashFile != "")
    {
        if (bifOptions->GetPPKFileName() != "" || bifOptions->GetPSKFileName() != "")
        {
            if (currentAuthCtx)
            {
                if (bifOptions->GetPPKFileName() != "")
                {
                    primaryKeyFile = bifOptions->GetPPKFileName();
                }
                if (bifOptions->GetPSKFileName() != "")
                {
                    primaryKeyFile = bifOptions->GetPSKFileName();
                }

                std::ifstream File(primaryKeyFile.c_str());
                std::string word;
                File >> word;
                File >> word;

                VersalAuthenticationContext* authCtx;
                if (word == "EC")
                {
                    authCtx = new VersalAuthenticationContext(currentAuthCtx, Authentication::ECDSA);
                }
                else
                {
                    authCtx = new VersalAuthenticationContext(currentAuthCtx, Authentication::RSA);
                }
                authCtx->hash = hash;
                authCtx->GeneratePPKHash(hashFile);
            }
        }
        else
        {
            LOG_ERROR("Cannot read PPK/PSK. PPK/PSK is mandatory to generate PPK hash bits, using '-efuseppkbits'.");
        }
    }
}

/******************************************************************************/
void VersalBootImage::BuildAndLink(Binary* cache)
{
    if (imageList.size() == 0)
    {
        LOG_WARNING("No partition images given");
    }
 
    DetermineEncryptionDefaults();
    partitionHeaderList.clear();
    ReplaceImagesNoSubSys();

    /* Build stage */
    /* all static fields within the header tables are populated here */
    bootHeader->Build(*this, *cache);
    imageHeaderTable->Build(*this, *cache);
    partitionHeaderTable->Build(*this, *cache);
    checksumTable->Build(*this, *cache);
    partitionHeaderTable->BuildPartitions(*this, *cache);

    LOG_INFO("After build ");
    LOG_DUMP_IMAGE(*cache);

    /* Stack and alignment stage */
    /* Once the header tables are created, stack all the tables and do the necessary alignment */
    cache->StackAndAlign(options);

    LOG_INFO("After align ");
    LOG_DUMP_IMAGE(*cache);

    PrintPartitionInformation();

    /* Link stage - fields which depend on partitions are populated here */
    bootHeader->Link(*this);
    imageHeaderTable->Link(*this);
    partitionHeaderTable->Link(*this);
    partitionHeaderTable->LinkPartitions(*this);
    checksumTable->Link(*this);

    LOG_INFO("After Link ");
    LOG_DUMP_IMAGE(*cache);
}
