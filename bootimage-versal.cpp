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
#include "bootimage-versal.h"
#include "partitionheadertable-versal.h"
#include "binary-versal.h"
#include "checksum-versal.h"
#include "authentication-versal.h"
extern "C" {
#include "cdo-command.h"
#include "cdo-overlay.h"
#include "cdo-binary.h"
#include "cdo-load.h"
};
/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
VersalBootImage::VersalBootImage(Options& options, uint8_t index) : BootImage(options, index)
{
    partitionHeaderList.clear();
    options.SetDefaultAlignment(16);
    bootHeader = new VersalBootHeader();
    imageHeaderTable = new VersalImageHeaderTable();
    partitionHeaderTable = new VersalPartitionHeaderTable();
    currentEncryptCtx = new VersalEncryptionContext();
    currentAuthCtx = new VersalAuthenticationContext(Authentication::RSA);
    SetLegacyEncryptionFlag(true);
    partitionOutput = new VersalPartitionOutput();
    hash = new HashSha3();
    cache = new VersalBinary();
    checksumTable = new VersalChecksumTable();
    currentAuthCtx->hashType = AuthHash::Sha3;
    currentAuthCtx->hash = hash;
    partitionHeaderTable->firstSection = NULL;
    convertAieElfToCdo = true;
    createSubSystemPdis = true;
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
        /* Remove if when [keysrc_encryption] id deprecated*/
        if (image->IsBootloader())
        {
            if (image->GetEncryptionKeySrc() == KeySource::None && bifOptions->GetEncryptionKeySource() == KeySource::None)
            {
                LOG_ERROR("BIF attribute error !!!\n\t   'keysrc' must be specified when 'encryption' is enabled for partition : %s.", image->GetName().c_str());
            }
        }
        else
        {
            if (image->GetEncryptionKeySrc() == KeySource::None)
            {
                LOG_ERROR("BIF attribute error !!!\n\t   'keysrc' must be specified when 'encryption' is enabled for partition : %s.", image->GetName().c_str());
            }
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
        /* Remove if when [keysrc_encryption] id deprecated*/
        if (image->IsBootloader())
        {
            if (image->GetEncryptionKeySrc() != KeySource::None || bifOptions->GetEncryptionKeySource() != KeySource::None)
            {
                LOG_ERROR("BIF attribute error !!!\n\t   'keysrc' can be specified only when 'encryption' is enabled for partition : %s.", image->GetName().c_str());
            }
        }
        else
        {
            if (image->GetEncryptionKeySrc() != KeySource::None)
            {
                LOG_ERROR("BIF attribute error !!!\n\t   'keysrc' can be specified only when 'encryption' is enabled for partition : %s.", image->GetName().c_str());
            }
        }

        image->ValidateEncryptionAttributes(*this);
        if (options.bifOptions->metaHdrAttributes.encrypt != Encryption::None)
        {
            if (options.bifOptions->metaHdrAttributes.encrKeySource == KeySource::None)
            {
                LOG_ERROR("BIF attribute error !!!\n\t   'keysrc' must be specified when 'encryption' is enabled for MetaHeader.");
            }
            options.bifOptions->SetHeaderEncryption(true);
            if (imageHeaderTable->GetEncryptContext() == NULL)
            {
                imageHeaderTable->SetEncryptContext(new VersalEncryptionContext());
            }
        }
        else if(options.bifOptions->metaHdrAttributes.encrKeySource != KeySource::None)
        {
            LOG_ERROR("BIF attribute error !!!\n\t   'keysrc' can be specified only when 'encryption' is enabled for MetaHeader.");
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
    case Authentication::ECDSAp521:
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
        currentAuthCtx->SetSPKSignatureFile(image->GetSpkSignFile());

        if (image->GetBhSignFile() != "")
        {
            currentAuthCtx->bhSignFile = image->GetBhSignFile();
        }

        AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
        if (authType == Authentication::ECDSA)
        {
            AuthenticationContext::SetAuthenticationKeyLength(EC_P384_KEY_LENGTH);
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
                //LOG_ERROR("Bootloader must be authenticated to authenticate the Meta Header");
            }
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
    bool full_pdi = true;
    bool smap_exists = true;

    if (StringUtils::GetExtension(baseFile) == ".mcs")
    {
        LOG_ERROR("Parsing mcs format file is not supported : %s", baseFile.c_str());
    }

    FILE *binFile = fopen(it->filename.c_str(), "rb");

    if (!binFile)
    {
        LOG_ERROR("Cannot read file %s", it->filename.c_str());
    }

    VersalBootHeaderStructure* bH = new VersalBootHeaderStructure;
    fread(bH, 1, sizeof(VersalBootHeaderStructure), binFile);

    if (bH->widthDetectionWord != 0xAA995566)
    {
        full_pdi = false;
    }

    uint32_t firstWord = *bH->smapWords;
    if ((firstWord == 0xDD000000) || (firstWord == 0x00DD0000) || (firstWord == 0x0000DD00) || (firstWord == 0x0000000DD))
    {
        smap_exists = true;
    }
    delete bH;
    bH = NULL;
    fclose(binFile);

    std::ifstream src(it->filename.c_str(), std::ios::binary);

    if (!src.good())
    {
        LOG_ERROR("Failure in reading bootimage file for import - %s ", baseFile.c_str());
    }

    VersalBootHeader* importedBh = NULL;
    if (!full_pdi)
    {
        LOG_WARNING("File %s is not a full PDI. It is missing the boot header", baseFile.c_str());
        bootloaderFound = false;
        if (smap_exists)
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
        importedBh = new VersalBootHeader(src);
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
    uint32_t offset = imageHeaderTable->GetFirstImageHeaderOffset() * sizeof(uint32_t);
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

        if (createSubSystemPdis == true)
        {
            src.seekg(offset);
            SubSysImageHeader* subsys = new SubSysImageHeader(src);

            for (uint32_t i = 0; i < subsys->num_of_images; i++)
            {
                ImageHeader* image = new VersalImageHeader(src, (VersalImageHeaderStructure*)subsys->section->Data, IsBootloader, i);
                image->SetAlignment(it->alignment);
                image->SetOffset(it->offset);
                image->SetReserve(it->reserve, it->updateReserveInPh);
                image->SetLoad(it->load);
                image->SetStartup(it->startup);
                image->SetPartitionRevocationId(it->GetRevokeId());

                /* Local key files/signatures within partition attributes have more priority than global key/signature files,
                if both specified */
                image->SetPpkFile(options.bifOptions->GetPPKFileName());
                image->SetPskFile(options.bifOptions->GetPSKFileName());
                if (it->ppkFile != "" || it->pskFile != "")
                {
                    image->SetPpkFile(it->ppkFile);
                    image->SetPskFile(it->pskFile);
                }

                image->SetSpkFile(options.bifOptions->GetSPKFileName());
                image->SetSskFile(options.bifOptions->GetSSKFileName());
                if (it->spkFile != "" || it->sskFile != "")
                {
                    image->SetSpkFile(it->spkFile);
                    image->SetSskFile(it->sskFile);
                }

                image->SetSpkSignFile(it->spkSignatureFile);
                image->SetBhSignFile(options.bifOptions->GetBHSignFileName());

                /* Commenting this func for now, check while doing the HSM mode */
                ConfigureProcessingStages(image, it);
                subsys->imgList.push_back(image);

                /* Image is just temporary, we need to get a pointer back to the copied object and relink */
                subsys->imgList.back()->Relink();
                ImageHeader* newImage = subsys->imgList.back();

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
                    if (ph->GetAuthCertificateOffset() != 0)
                    {
                        LOG_INFO("Loading AC context for section %s ", ph->section->Name.c_str());
                        binFile = fopen(it->filename.c_str(), "rb");
                        if (!binFile)
                        {
                            LOG_ERROR("Cannot read file %s", it->filename.c_str());
                        }

                        uint8_t* aC = new uint8_t[sizeof(AuthCertificate4096Sha3PaddingStructure)];
                        if (!(fseek(binFile, ph->GetAuthCertificateOffset(), SEEK_SET)))
                        {
                            size_t result = fread(aC, 1, sizeof(AuthCertificate4096Sha3PaddingStructure), binFile);
                            if (result != sizeof(AuthCertificate4096Sha3PaddingStructure))
                            {
                                LOG_ERROR("Error parsing Authentication Certificates from PDI file");
                            }
                        }
                        fclose(binFile);

                        Authentication::Type authtype = Authentication::None;
                        if (((*aC) & 0xF3) == 0x02)
                        {
                            authtype = Authentication::ECDSA;
                        }
                        else if (((*aC) & 0xF3) == 0x11)
                        {
                            authtype = Authentication::RSA;
                        }
                        else if (((*aC) & 0xF3) == 0x22)
                        {
                            authtype = Authentication::ECDSAp521;
                        }
                        VersalAuthenticationContext* auth = new VersalAuthenticationContext((AuthCertificate4096Sha3PaddingStructure*)aC, authtype);

                        // load in previous certificate data
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
            subSysImageList.push_back(subsys);
        }
        else
        {
            src.seekg(offset);
            ImageHeader* image = new VersalImageHeader(src, IsBootloader);

            image->SetAlignment(it->alignment);
            image->SetOffset(it->offset);
            image->SetReserve(it->reserve, it->updateReserveInPh);
            image->SetLoad(it->load);
            image->SetStartup(it->startup);
            image->SetPartitionRevocationId(it->GetRevokeId());

            /* Local key files/signatures within partition attributes have more priority than global key/signature files,
            if both specified */
            image->SetPpkFile(options.bifOptions->GetPPKFileName());
            image->SetPskFile(options.bifOptions->GetPSKFileName());
            if (it->ppkFile != "" || it->pskFile != "")
            {
                image->SetPpkFile(it->ppkFile);
                image->SetPskFile(it->pskFile);
            }

            image->SetSpkFile(options.bifOptions->GetSPKFileName());
            image->SetSskFile(options.bifOptions->GetSSKFileName());
            if (it->spkFile != "" || it->sskFile != "")
            {
                image->SetSpkFile(it->spkFile);
                image->SetSskFile(it->sskFile);
            }

            image->SetSpkSignFile(it->spkSignatureFile);
            image->SetBhSignFile(options.bifOptions->GetBHSignFileName());

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
                if(ph->GetAuthCertificateOffset() != 0)
                {
                    LOG_INFO("Loading AC context for section %s ", ph->section->Name.c_str());

                    binFile = fopen(it->filename.c_str(), "rb");
                    if (!binFile)
                    {
                        LOG_ERROR("Cannot read file %s", it->filename.c_str());
                    }
                    uint8_t* aC = new uint8_t[sizeof(AuthCertificate4096Sha3PaddingStructure)];
                    if (!(fseek(binFile, ph->GetAuthCertificateOffset(), SEEK_SET)))
                    {
                        size_t result = fread(aC, 1, sizeof(AuthCertificate4096Sha3PaddingStructure), binFile);
                        if (result != sizeof(AuthCertificate4096Sha3PaddingStructure))
                        {
                            LOG_ERROR("Error parsing Authentication Certificates from PDI file");
                        }
                    }
                    fclose(binFile);

                    Authentication::Type authtype = Authentication::None;
                    if (((*aC) & 0xF3) == 0x02)
                    {
                        authtype = Authentication::ECDSA;
                    }
                    else if (((*aC) & 0xF3) == 0x11)
                    {
                        authtype = Authentication::RSA;
                    }
                    else if (((*aC) & 0xF3) == 0x22)
                    {
                        authtype = Authentication::ECDSAp521;
                    }
                    VersalAuthenticationContext* auth = new VersalAuthenticationContext((AuthCertificate4096Sha3PaddingStructure*)aC, authtype);

                    AuthenticationCertificate* tempac;
                    tempac = new VersalAuthenticationCertificate(auth);
                    auth->preSigned = true;
                    tempac->fsbl = true;
                    ph->ac.push_back(tempac);
                    newImage->SetAuthContext(auth);
                }
            }
            offset += sizeof(VersalImageHeaderStructure);
        }
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

    if (!bootloaderAuthenticate && bootloaderEncrypt)
    {
        switch (bootloaderKeySource)
        {
        case KeySource::EfuseBlkKey:
        case KeySource::EfuseGryKey:
        case KeySource::EfuseRedKey:
        case KeySource::EfuseUserBlkKey0:
        case KeySource::EfuseUserBlkKey1:
        case KeySource::EfuseUserGryKey0:
        case KeySource::EfuseUserGryKey1:
        case KeySource::EfuseUserKey0:
        case KeySource::EfuseUserKey1:
            switch (partitionBifOptions->keySrc)
            {
            case KeySource::BbramBlkKey:
            case KeySource::BbramGryKey:
            case KeySource::BbramRedKey:
                LOG_ERROR("A Bbram key source cannot be used for other partitions when bootloader is not authenticated and uses a Efuse Key Source.");
            default:
                break;
            }
            break;
        case KeySource::BbramBlkKey:
        case KeySource::BbramGryKey:
        case KeySource::BbramRedKey:
            switch (partitionBifOptions->keySrc)
            {
            case KeySource::EfuseBlkKey:
            case KeySource::EfuseGryKey:
            case KeySource::EfuseRedKey:
            case KeySource::EfuseUserBlkKey0:
            case KeySource::EfuseUserBlkKey1:
            case KeySource::EfuseUserGryKey0:
            case KeySource::EfuseUserGryKey1:
            case KeySource::EfuseUserKey0:
            case KeySource::EfuseUserKey1:
                LOG_ERROR("An Efuse key source cannot be used for other partitions when bootloader is not authenticated and uses a Bbram Key Source.");
            default:
                break;
            }
        default:
            break;
        }
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
    image->SetFileList(partitionBifOptions->filelist);
    image->SetBootloader(partitionBifOptions->bootloader);
    image->SetAlignment(partitionBifOptions->alignment);
    image->SetOffset(partitionBifOptions->offset);
    image->SetReserve(partitionBifOptions->reserve, partitionBifOptions->updateReserveInPh);
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
        if ((bifOptions->aHwrot == true) && (partitionBifOptions->authType == Authentication::None))
        {
            LOG_ERROR("Bootloader must be authenticated when 'a_hwrot' is enabled");
        }
        if ((bifOptions->sHwrot == true) && (partitionBifOptions->encryptType == Encryption::None))
        {
            LOG_ERROR("Bootloader must be encrypted with 'keysrc=efuse_blk_key', when 's_hwrot' is enabled");
        }
        if((bifOptions->sHwrot == true) && (partitionBifOptions->keySrc != KeySource::EfuseBlkKey))
        {
            LOG_ERROR("Bootloader must be encrypted with 'keysrc=efuse_blk_key', when 's_hwrot' is enabled");
        }
        if (partitionBifOptions->authType == Authentication::ECDSAp521)
        {
            LOG_ERROR("BIF attribute error !!!\n\t\t  'ecdsa-p521' not supported for bootloader partition");
        }
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
                image->SetSlrPartition(true);
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
                image->SetSlrPartition(true);
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
    std::string secureDebugImageFile = options.GetSecureDebugImageFile();

    uint32_t authJtagImageSize = sizeof(AuthenticatedJtagImageStructure);
    uint8_t* writedata = new uint8_t[authJtagImageSize];
    memset(writedata, 0, authJtagImageSize);

    if (options.GetSecureDebugAuthType() != Authentication::None)
    {
        VersalAuthenticationContext* authCtx = new VersalAuthenticationContext(this->currentAuthCtx, options.GetSecureDebugAuthType());
        if (authCtx)
        {
            authCtx->hashType = authHash;
            authCtx->hash = hash;
            authCtx->CreateAuthJtagImage(writedata, bifOptions->authJtagInfo);
        }

        std::ofstream ofs;
        ofs.open(secureDebugImageFile.c_str(), std::ios::binary);

        if (!ofs)
        {
            LOG_ERROR("Cannot write output to file : %s", secureDebugImageFile.c_str());
        }

        ofs.write((const char*)writedata, authJtagImageSize);
        ofs.close();

        LOG_TRACE("Authenticated Jtag Image : '%s' generated.", secureDebugImageFile.c_str());
    }

    delete[] writedata;
}

/******************************************************************************/
void VersalBootImage::ConfigureEncryptionBlocks(ImageHeader * image, PartitionBifOptions * partitionBifOptions)
{
    if (partitionBifOptions->encryptType == Encryption::AES)
    {
        std::vector<uint32_t> encrBlocks = partitionBifOptions->GetEncryptionBlocks();
        uint32_t defaultEncrBlockSize = partitionBifOptions->GetDefaultEncryptionBlockSize();
        image->SetDefaultEncrBlockSize(defaultEncrBlockSize);

        if (image->IsBootloader())
        {
            for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
            {
                image->InsertEncrBlocksList(encrBlocks[itr]);
            }
        }
        else
        {
            Binary::Length_t encrBlocksSize = 0;
            Binary::Length_t encrOverhead = 0;
            Binary::Length_t secureChunkSize = VersalPartition::GetSecureChunkSize();
            if (partitionBifOptions->authType == Authentication::None)
            {
                secureChunkSize += SHA3_LENGTH_BYTES;
            }

            /* Creating encryption blocks for 64KB from user specified blocks. Consider encryption overhead as well */
            for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
            {
                encrBlocksSize += encrBlocks[itr];
                encrOverhead += (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                /* Push all the user blocks till the sum of user specified blocks and overhead is less than 64KB. */
                if ((encrBlocksSize + encrOverhead) < secureChunkSize)
                {
                    image->InsertEncrBlocksList(encrBlocks[itr]);
                }
                /* When the sum of user specified blocks and overhead reaches 64KB, push that block and break. */
                else if ((encrBlocksSize + encrOverhead) == secureChunkSize)
                {
                    image->InsertEncrBlocksList(encrBlocks[itr]);
                    break;
                }
                /* If the sum of user specified blocks and overhead exceeds 64KB, truncate that block, .push and break.*/
                else
                {
                    Binary::Length_t lastBlock = secureChunkSize - (encrBlocksSize - encrBlocks[itr] + encrOverhead);
                    encrBlocksSize += (lastBlock - encrBlocks[itr]);
                    image->InsertEncrBlocksList(lastBlock);
                    LOG_WARNING("The last encryption block size is truncated to %d to fit into the secure chunk of 32KB.", lastBlock);
                    break;
                }
            }

            /* If the user specified blocks, does not make a chunk of 64KB, then calculate the rest and push. */
            if (encrBlocksSize + encrOverhead < secureChunkSize)
            {
                /* If a default size(using (*)) is mentioned, */
                if (defaultEncrBlockSize != 0)
                {
                    encrBlocksSize += defaultEncrBlockSize;
                    encrOverhead += (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                    /* then push the default size untill the sum of encr blocks and overhead is < or = 64KB.*/
                    while (encrBlocksSize + encrOverhead < secureChunkSize)
                    {
                        image->InsertEncrBlocksList(defaultEncrBlockSize);
                        encrOverhead += (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                        encrBlocksSize += defaultEncrBlockSize;
                    }

                    Binary::Length_t lastBlock = defaultEncrBlockSize;
                    /* When the sum of encr blocks and overhead goes beyond 64KB, truncate the default size and push.*/
                    if ((encrBlocksSize + encrOverhead) > secureChunkSize)
                    {
                        if (secureChunkSize > (encrBlocksSize + encrOverhead - defaultEncrBlockSize))
                        {
                            lastBlock = secureChunkSize - (encrBlocksSize + encrOverhead - defaultEncrBlockSize);
                            encrBlocksSize += (lastBlock - defaultEncrBlockSize);
                            LOG_WARNING("The last encryption block size is truncated to %d to fit into the secure chunk of 32KB.", lastBlock);
                        }
                        else
                        {
                            LOG_ERROR("The keyrolling block size '%d' cannot fit into the secure chunk of 32KB. Please choose another block size.\n           For details, refer to the section 'Design Advisories for Bootgen' from UG1283.", defaultEncrBlockSize);
                        }
                    }
                    image->InsertEncrBlocksList(lastBlock);
                }
                /* If a default size(using (*)) is not mentioned,
                   then calculate the last block that makes sum of encr blocks and overhead = 64KB and push.*/
                else
                {
                    encrOverhead += (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                    image->InsertEncrBlocksList(secureChunkSize - (encrBlocksSize + encrOverhead));
                }
            }
        }

        if (image->GetEncrBlocksList().size() != 0)
        {
            static bool warningGiven = false;
            for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
            {
                if (encrBlocks[itr] > 1024 * 1024 * ENCR_BLOCK_IN_BYTES)
                {
                    if (!warningGiven)
                    {
                        LOG_WARNING("partition - %s, block - %d", partitionBifOptions->filename.c_str(), encrBlocks[itr]);
                        LOG_WARNING("The key rolling rate associated with the partition - %s exceeds 1M traces per key.\n\t   If you are using a device with the AES masking DPA countermeasure enabled,\n\t   you should confirm that the key rolling rate is sufficient.\n\t   For more details on key rolling rates, please see the Versal Security Users Manual(UG1508).", partitionBifOptions->filename.c_str());
                        warningGiven = true;
                    }
                }
            }
        }
    }
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
    uint8_t slr_boot_cnt = 0;
    uint8_t slr_cfg_cnt = 0;
    // Add 'LOG_WARNING("A bootimage cannot be generated on the go, with '-generate_keys'.\n           However, the requested keys will be generated.");'
    if (bifoptions->GetAESKeyFileName() != "")
    {
        LOG_WARNING("This usage of 'aeskeyfile' is deprecated for VERSAL.. \n           Please refer 'bootgen -arch versal -bif_help aeskeyfile' for more info.");
        currentEncryptCtx->SetAesFileName(bifoptions->GetAESKeyFileName());
        options.SetEncryptionKeyFile(bifoptions->GetAESKeyFileName());
    }

    if (bifOptions->GetBhRsa() == BhRsa::BhRsaEnable && bifOptions->aHwrot == true)
    {
        LOG_ERROR("BIF attribute error !!!\n\t\t   'bh_auth_enable' and 'a_hwrot' cannot be used together");
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
            VersalAuthenticationContext* authCtx = NULL;
            if (currentAuthCtx)
            {
                std::string filename = bifoptions->GetSPKFileName();
                FILE* f;
                f = fopen(filename.c_str(), "r");
                if (f == NULL)
                {
                    LOG_ERROR("Cannot open key %s", filename.c_str());
                }
                RSA* rsa = PEM_read_RSA_PUBKEY(f, NULL, NULL, NULL);
                fclose(f);

                f = fopen(filename.c_str(), "r");
                if (f == NULL)
                {
                    LOG_ERROR("Cannot open key %s", filename.c_str());
                }
                EC_KEY *eckeyLocal = PEM_read_EC_PUBKEY(f, NULL, NULL, NULL);
                fclose(f);

                if (rsa != NULL)
                {
                    authCtx = new VersalAuthenticationContext(currentAuthCtx, Authentication::RSA);
                }
                else if(eckeyLocal != NULL)
                {
                    authCtx = new VersalAuthenticationContext(currentAuthCtx, Authentication::ECDSA);
                }
            }

            if (options.DoGenerateHashes())
            {
                authCtx->hash = hash;
                LOG_INFO("Generating SPK Hash File");
                authCtx->GenerateSPKHashFile(bifoptions->GetSPKFileName(), hash);
            }

            if (authCtx != NULL)
            {
                delete authCtx;
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
    currentEncryptCtx->SetBbramKekIVFile(bifoptions->GetBbramKekIVFile());
    currentEncryptCtx->SetEfuseKekIVFile(bifoptions->GetEfuseKekIVFile());
    currentEncryptCtx->SetEfuseUserKek0IVFile(bifoptions->GetEfuseUserKek0IVFile());
    currentEncryptCtx->SetEfuseUserKek1IVFile(bifoptions->GetEfuseUserKek1IVFile());
    XipMode = bifoptions->GetXipMode();

    /* Overlay CDO */
    {
        std::string overlayFile = options.GetOverlayCDOFileName();

        if (overlayFile != "")
        {
            CdoSequence * seq = cdoseq_load_cdo((char *)(overlayFile.c_str()));
            if (seq == NULL)
            {
                LOG_ERROR("Error parsing overlay CDO file");
            }
            overlayCDO = cdooverlay_open(seq);
            if (overlayCDO == NULL)
            {
                LOG_ERROR("Error parsing overlay CDO file");
            }
        }
    }
    LOG_INFO("Parsing Partition Data to Image");

    if (bifoptions->imageBifOptionList.size() == 0)
    {
        if (bifoptions->partitionBifOptionList.size() != 0)
        {
            LOG_ERROR("Legacy BIF format detected. Please update to Versal BIF format. Refer UG1283 for more details.");
        }

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
                        img->SetName(subSysImage->GetSubSystemName());

                        /* PLM and PSM should not have 0x1c000000 as default IDs, otherwise they will get merged with default subsystem 
                           As they should be replaced in their respective subsystems, they should have some non-common id.
                        */
                        if ((bootimage_partition == true) && (subSysImage->GetSubSystemId() == 0x1c000000) &&
                            (img->IsBootloader() || img->GetDestCpu() == DestinationCPU::PMU))
                        {
                            subSysImage->SetSubSystemId(0x0);
                        }
                        if (subSysImage->GetDelayHandoffMode() || subSysImage->GetDelayLoadMode())
                        {
                            if (img->IsBootloader())
                            {
                                LOG_ERROR("BIF attributes 'delay_load'/'delay_handoff' not supported for PMC subsystem");
                            }
                            if (img->GetDestCpu() == DestinationCPU::PMU)
                            {
                                //LOG_WARNING("delay_load/delay_handoff enabled for %s subsystem, this may cause some issues while using PS", subSysImage->GetSubSystemName().c_str());
                            }
                            if ((StringUtils::EndsWith(img->GetFilename(), ".npi")) || (StringUtils::EndsWith(img->GetFilename(), ".rnpi")))
                            {
                                //LOG_WARNING("delay_load/delay_handoff enabled for %s subsystem, this may cause some issues if not handled properly", subSysImage->GetSubSystemName().c_str());
                            }
                        }
                        if (((*partitr)->partitionType == PartitionType::SLR_BOOT) || ((*partitr)->partitionType == PartitionType::SLR_CONFIG))
                        {
                            if ((*partitr)->partitionType == PartitionType::SLR_BOOT)
                            {
                                if (++slr_boot_cnt == bifoptions->slrBootCnt)
                                {
                                    subSysImage->imgList.push_back(img);
                                }
                            }
                            if ((*partitr)->partitionType == PartitionType::SLR_CONFIG)
                            {
                                if (++slr_cfg_cnt == bifoptions->slrConfigCnt)
                                {
                                    subSysImage->imgList.push_back(img);
                                }
                            }
                        }
                        else
                        {
                            subSysImage->imgList.push_back(img);
                        }
                    }
                }
            }
            /* Add to subsys list only if it the partition type is not bootimage. Because bootimage will have its own subsystems */
            if ((!bootimage_partition) || (subSysImage->imgList.size() != 0))
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
                if (PSMImageCnt == 0)
                {
                    it2 = img;
                }
                PSMImageCnt++;
            }
            if ((PSMImageCnt > 1) && (replacePSMImages == false))
            {
                replacePSMImages = true;
                PSMimg = *img;
                subsysIt = subsys;
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
            if ((bootloaderCnt > 1) && (replaceImages == false))
            {
                replaceImages = true;
                img1 = (*img);
                subsysIt = subsys;
            }
        }
    }
    if ((replacePSMImages == true) || (replaceImages == true))
    {
        subSysImageList.erase(subsysIt);
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
                    (*subsys)->imgList.erase(it2);
                    (*subsys)->imgList.push_back(PSMimg);
                    break;
                    
                }
            }
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
        if (bifOptions->GetPPKFileName() != "")
        {
            if (currentAuthCtx)
            {
                primaryKeyFile = bifOptions->GetPPKFileName();

                std::string filename = primaryKeyFile;
                FILE* f = NULL;
                f = fopen(filename.c_str(), "r");
                if (f == NULL)
                {
                    LOG_ERROR("Cannot open key %s", filename.c_str());
                }
                RSA* rsa = PEM_read_RSA_PUBKEY(f, NULL, NULL, NULL);
                fclose(f);

                f = fopen(filename.c_str(), "r");
                if (f == NULL)
                {
                    LOG_ERROR("Cannot open key %s", filename.c_str());
                }
                EC_KEY *eckeyLocal = PEM_read_EC_PUBKEY(f, NULL, NULL, NULL);
                fclose(f);

                VersalAuthenticationContext* authCtx = NULL;
                if (rsa != NULL)
                {
                    authCtx = new VersalAuthenticationContext(currentAuthCtx, Authentication::RSA);
                }
                else if (eckeyLocal != NULL)
                {
                    FILE* f = NULL;
                    f = fopen(primaryKeyFile.c_str(), "r");
                    if (f == NULL)
                    {
                        LOG_ERROR("Cannot open key %s", primaryKeyFile.c_str());
                    }

                    EC_KEY *eckeyLocal = PEM_read_EC_PUBKEY(f, NULL, NULL, NULL);
                    const EC_GROUP* ecgroup = EC_KEY_get0_group(eckeyLocal);
                    int ecCurveNID = EC_GROUP_get_curve_name(ecgroup);
                    fclose(f);

                    if (ecCurveNID == NID_secp384r1)
                    {
                        authCtx = new VersalAuthenticationContext(currentAuthCtx, Authentication::ECDSA);
                    }
                    else if (ecCurveNID == NID_secp521r1)
                    {
                        authCtx = new VersalAuthenticationContext(currentAuthCtx, Authentication::ECDSAp521);
                    }
                    else
                    {
                        LOG_ERROR("Unsupported ECDSA curve read from key file : %s\n           Supported ECDSA curves: P384, P521", primaryKeyFile.c_str());
                    }
                }
                else
                {
                    LOG_ERROR("Cannot read the public key file : %s", filename.c_str());
                }

                authCtx->hash = hash;
                authCtx->GeneratePPKHash(hashFile);
            }
        }
        else if (bifOptions->GetPSKFileName() != "")
        {
            if (currentAuthCtx)
            {
                primaryKeyFile = bifOptions->GetPSKFileName();

                std::ifstream File(primaryKeyFile.c_str());
                std::string word;
                File >> word;
                File >> word;

                VersalAuthenticationContext* authCtx = NULL;
                if (word == "EC")
                {
                    FILE* f = NULL;
                    f = fopen(primaryKeyFile.c_str(), "r");
                    if (f == NULL)
                    {
                        LOG_ERROR("Cannot open key %s", primaryKeyFile.c_str());
                    }

                    EC_KEY *eckeyLocal = PEM_read_ECPrivateKey(f, NULL, NULL, NULL);
                    const EC_GROUP* ecgroup = EC_KEY_get0_group(eckeyLocal);
                    int ecCurveNID = EC_GROUP_get_curve_name(ecgroup);
                    fclose(f);

                    if (ecCurveNID == NID_secp384r1)
                    {
                        authCtx = new VersalAuthenticationContext(currentAuthCtx, Authentication::ECDSA);
                    }
                    else if (ecCurveNID == NID_secp521r1)
                    {
                        authCtx = new VersalAuthenticationContext(currentAuthCtx, Authentication::ECDSAp521);
                    }
                    else
                    {
                        LOG_ERROR("Unsupported ECDSA curve read from key file : %s\n           Supported ECDSA curves: P384, P521", primaryKeyFile.c_str());
                    }
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
void VersalBootImage::AppendImagesInSubsystems(void)
{
    std::list<SubSysImageHeader*> eraseSubSysImageList;

    for (std::list<SubSysImageHeader*>::iterator subsys1 = subSysImageList.begin(); subsys1 != subSysImageList.end(); subsys1++)
    {
        for (std::list<SubSysImageHeader*>::iterator subsys2 = subSysImageList.begin(); subsys2 != subSysImageList.end(); subsys2++)
        {
            if ((*subsys1) != (*subsys2))
            {
                if (((*subsys2)->GetSubSystemId() == (*subsys1)->GetSubSystemId()) && (*subsys1)->GetSubSystemId() != 0 && (*subsys1)->GetSubSystemId() != 0x1c000001)
                {
                    (*subsys1)->imgList.insert((*subsys1)->imgList.end(), (*subsys2)->imgList.begin(), (*subsys2)->imgList.end());
                    eraseSubSysImageList.push_back(*subsys2);
                }
            }
        }
        for (std::list<SubSysImageHeader*>::iterator erasesubsys = eraseSubSysImageList.begin(); erasesubsys != eraseSubSysImageList.end(); erasesubsys++)
        {
            subSysImageList.remove(*erasesubsys);
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
    if (subSysImageList.size() == 0)
    {
        if (createSubSystemPdis == false)
        {
            /* Create old style image headers - for local testing */
            /* No need to do anything here */
            //LOG_WARNING("BOOTGEN_SUBSYSTEM_PDI is not set, PDI has one image header for each input partition file");
        }
        else
        {
            ImageBifOptions *imgOptions = new ImageBifOptions();
            /* If Subsystems are not specified in BIF - create one image header for PLM and other image header for subsytem and add all partitions to it */
            imgOptions->SetImageName("default_subsys");
            SubSysImageHeader* sub_sys_image = new SubSysImageHeader(imgOptions);
            for (std::list<ImageHeader*>::iterator image = imageList.begin(); image != imageList.end(); image++)
            {
                if ((*image)->IsBootloader())
                {
                    imgOptions->SetImageName("pmc_subsys");
                    SubSysImageHeader* plm_header = new SubSysImageHeader(imgOptions);
                    plm_header->imgList.push_back((*image));
                    plm_header->SetSubSystemName("pmc_subsys");
                    plm_header->SetSubSystemId(0x1c000001);
                    subSysImageList.push_back(plm_header);
                }
                else
                {
                    sub_sys_image->imgList.push_back((*image));
                }
            }
            if (sub_sys_image->imgList.size() != 0)
            {
                sub_sys_image->SetSubSystemName("default_subsys");
                sub_sys_image->SetSubSystemId(0x1c000000);
                subSysImageList.push_back(sub_sys_image);
                LOG_INFO("BOOTGEN_SUBSYSTEM_PDI is set, but no subsystems are specified, all partitions are grouped into one default subsystem");
            }
         }
    }

    DetermineEncryptionDefaults();
    partitionHeaderList.clear();
    if (createSubSystemPdis == true)
    {
        AppendImagesInSubsystems();
        ReplaceImages();
    }
    else
    {
        ReplaceImagesNoSubSys();
    }

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
