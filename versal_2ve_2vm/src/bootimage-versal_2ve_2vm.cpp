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
#include "bootimage-versal_2ve_2vm.h"
#include "partitionheadertable-versal_2ve_2vm.h"
#include "binary-versal_2ve_2vm.h"
#include "checksum-versal_2ve_2vm.h"
#include "authentication-versal_2ve_2vm.h"
#include "authentication.h"
#include "elftools.h"
#include "fileutils.h"
#include "tlbingeneration.h"
extern "C" {
#include "cdo-command.h"
#include "cdo-overlay.h"
#include "cdo-binary.h"
#include "cdo-load.h"
#include "lms-utils.h"
#include "hss_verify.h"
};


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
Versal_2ve_2vmBootImage::Versal_2ve_2vmBootImage(Options& options, uint8_t index) : BootImage(options, index)
{
    partitionHeaderList.clear();
    options.SetDefaultAlignment(16);
    bootHeader = std::make_unique<Versal_2ve_2vmBootHeader>(arch);
    imageHeaderTable = std::make_unique<Versal_2ve_2vmImageHeaderTable>();
    partitionHeaderTable = std::make_unique<Versal_2ve_2vmPartitionHeaderTable>();
    currentEncryptCtx = std::make_unique<Versal_2ve_2vmEncryptionContext>();
    currentAuthCtx = std::make_unique<Versal_2ve_2vmAuthenticationContext>(Authentication::RSA);
    SetLegacyEncryptionFlag(true);
    partitionOutput = std::make_unique<VersalPartitionOutput>();
    hash = std::make_unique<HashSha3>();
    cache = std::make_unique<Versal_2ve_2vmBinary>();
    checksumTable = std::make_unique<Versal_2ve_2vmChecksumTable>();
    currentAuthCtx->hashType = AuthHash::Sha3;
    currentAuthCtx->hash = hash.get();  // Non-owning reference to BootImage's hash
    partitionHeaderTable->firstSection = NULL;
    convertAieElfToCdo = true;
    current_image_block = 0;
    createSubSystemPdis = true;
}

/******************************************************************************/
Versal_2ve_2vmBootImage::~Versal_2ve_2vmBootImage()
{
}

/******************************************************************************/
void Versal_2ve_2vmBootImage::ConfigureEncryptionContext(ImageHeader * image, Encryption::Type encryptType)
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
            imageHeaderTable->SetEncryptContext(std::make_unique<Versal_2ve_2vmEncryptionContext>(this->currentEncryptCtx.get()));
        }
        image->SetEncryptContext(std::make_unique<Versal_2ve_2vmEncryptionContext>(this->currentEncryptCtx.get()));
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
                imageHeaderTable->SetEncryptContext(std::make_unique<Versal_2ve_2vmEncryptionContext>());
            }
        }
        else if(options.bifOptions->metaHdrAttributes.encrKeySource != KeySource::None)
        {
            LOG_ERROR("BIF attribute error !!!\n\t   'keysrc' can be specified only when 'encryption' is enabled for MetaHeader.");
        }
        image->SetEncryptContext(std::make_unique<NoneEncryptionContext>());
        break;

    default:
        image->SetEncryptContext(NULL);
        break;
    }
}

/******************************************************************************/
void Versal_2ve_2vmBootImage::ConfigureAuthenticationContext(ImageHeader * image, Authentication::Type authType, PartitionBifOptions * partitionbifoptions)
{
    /* Configure the authentication context */
    std::string spkSignFile = options.GetSpkSigFileName();
    switch (authType)
    {
    case Authentication::RSA:
    case Authentication::ECDSA:
    case Authentication::ECDSAp521:
    case Authentication::LMS_SHA2_256:
    case Authentication::LMS_SHAKE256:
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
        authOnPartitionFound = true;
        currentAuthCtx->hashType = GetAuthHashAlgo();
        currentAuthCtx->ppkFile = image->GetPpkFile();
        currentAuthCtx->pskFile = image->GetPskFile();
        currentAuthCtx->spkFile = image->GetSpkFile();
        currentAuthCtx->sskFile = image->GetSskFile();
        currentAuthCtx->spkSignFile = image->GetSpkSignFile();
        currentAuthCtx->spkIdentification = image->GetSpkRevocationId();
        currentAuthCtx->lmsOnly = partitionbifoptions->lmsOnly;

        //Copying primary and secondary LmsKeyParam from bifOptions to authCxt
        std::vector<LmsKeyParam> primaryLmsParams = bifOptions->GetPrimaryLmsParams();
        currentAuthCtx->primaryLmsParamsSize = primaryLmsParams.size() * 2;
        currentAuthCtx->primaryLmsParams = new int[currentAuthCtx->primaryLmsParamsSize];
        for (int i = 0, j = 0; i < currentAuthCtx->primaryLmsParamsSize && j < (int)primaryLmsParams.size(); i=i+2, j++)
        {
            currentAuthCtx->primaryLmsParams[i] = primaryLmsParams[j].h;
            currentAuthCtx->primaryLmsParams[i+1] = primaryLmsParams[j].w;
        }

        std::vector<LmsKeyParam> secondaryLmsParams = bifOptions->GetSecondaryLmsParams();
        currentAuthCtx->secondaryLmsParamsSize = secondaryLmsParams.size() * 2;
        currentAuthCtx->secondaryLmsParams = new int[currentAuthCtx->secondaryLmsParamsSize];
        for (int i = 0, j = 0; i < currentAuthCtx->secondaryLmsParamsSize && j < (int)secondaryLmsParams.size(); i=i+2, j++)
        {
            currentAuthCtx->secondaryLmsParams[i] = secondaryLmsParams[j].h;
            currentAuthCtx->secondaryLmsParams[i+1] = secondaryLmsParams[j].w;
        }

        /*
        for (int i = 0; i < currentAuthCtx->primaryLmsParamsSize; i=i+2)
        {
            LOG_TRACE("Primary Key Parameters :");
            LOG_TRACE("height %d", currentAuthCtx->primaryLmsParams[i]);
            LOG_TRACE("widht %d", currentAuthCtx->primaryLmsParams[i+1]);
        }
        for (int i = 0; i < currentAuthCtx->secondaryLmsParamsSize; i=i+2)
        {
            LOG_TRACE("Secondary Key Parameters :");
            LOG_TRACE("height %d", currentAuthCtx->secondaryLmsParams[i]);
            LOG_TRACE("widht %d", currentAuthCtx->secondaryLmsParams[i+1]);
        }
        */

        if (authType == Authentication::RSA)
            currentAuthCtx->signatureLength = SIGN_LENGTH_VERSAL;
        else if (authType == Authentication::ECDSA)
            currentAuthCtx->signatureLength = EC_P384_KEY_LENGTH * 2;
        else if (authType == Authentication::ECDSAp521)
            currentAuthCtx->signatureLength = EC_P521_KEY_LENGTH2 * 2;
        else if ((authType == Authentication::LMS_SHA2_256) || (authType == Authentication::LMS_SHAKE256))
        {
            currentAuthCtx->signatureLength = GetLmsSignatureLength(currentAuthCtx->primaryLmsParams, currentAuthCtx->primaryLmsParamsSize,
                currentAuthCtx->pskFile.c_str(), currentAuthCtx->ppkFile.c_str(), currentAuthCtx->lmsOnly);
        }
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
        else if (authType == Authentication::ECDSAp521)
        {
            AuthenticationContext::SetAuthenticationKeyLength(EC_P521_KEY_LENGTH2);
            LOG_TRACE("ECDSAP521");
        }

        // Store context in container to keep it alive, pass raw pointer to image
        auto authCtxPtr = std::make_unique<Versal_2ve_2vmAuthenticationContext>(currentAuthCtx.get(), authType);
        AuthenticationContext* authCtx = authCtxPtr.get();
        image->SetAuthContext(authCtx);
        authContexts.push_back(std::move(authCtxPtr));
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
        // Store context in container to keep it alive, pass raw pointer to image
        auto authCtxPtr = std::make_unique<NoneAuthenticationContext>();
        image->SetAuthContext(authCtxPtr.get());
        authContexts.push_back(std::move(authCtxPtr));
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
void Versal_2ve_2vmBootImage::ConfigureChecksumContext(ImageHeader * image, Checksum::Type checksumType)
{
    /* Configure the checksum context */
    switch (checksumType)
    {
    case Checksum::MD5:
    case Checksum::SHA2:
        LOG_ERROR("BIF attribute error !!!\n\t\t 'checksum=sha2/md5' is not supported in VERSAL architecture.");
        break;

    case Checksum::SHA3:
        image->SetChecksumContext(std::make_unique<Versal_2ve_2vmSHA3ChecksumContext>());
        break;

    case Checksum::None:
    default:
        image->SetChecksumContext(std::make_unique<NoneChecksumContext>());
        break;
    }
}

/******************************************************************************/
void Versal_2ve_2vmBootImage::ParseBootImage(PartitionBifOptions* it)
{
    LOG_INFO("Importing BootImage...");
    std::string baseFile = StringUtils::BaseName(it->filename);
    bool full_pdi = true;
    bool smap_exists = false;
    bool this_bootimage = false;
    static uint32_t prev_image_block = 0;
    if (StringUtils::GetExtension(baseFile) == ".mcs")
    {
        LOG_ERROR("Parsing mcs format file is not supported : %s", baseFile.c_str());
    }

    FILE *binFile = fopen(it->filename.c_str(), "rb");

    if (!binFile)
    {
        LOG_ERROR("Cannot read file %s", it->filename.c_str());
    }

    auto bH = std::make_unique<Versal_2ve_2vmBootHeaderStructure>();
    fread(bH.get(), 1, sizeof(Versal_2ve_2vmBootHeaderStructure), binFile);

    if (bH->widthDetectionWord != 0xAA995566)
    {
        full_pdi = false;
    }

    uint32_t firstWord = *bH->smapWords;
    if ((firstWord == 0xDD000000) || (firstWord == 0x00DD0000) || (firstWord == 0x0000DD00) || (firstWord == 0x0000000DD))
    {
        smap_exists = true;
    }
    // Cleanup handled by unique_ptr
    fclose(binFile);

    std::ifstream src(it->filename.c_str(), std::ios::binary);

    if (!src.good())
    {
        LOG_ERROR("Failure in reading bootimage file for import - %s ", baseFile.c_str());
    }

    std::unique_ptr<Versal_2ve_2vmBootHeader> importedBh;
    if (!full_pdi)
    {
        LOG_WARNING("File %s is not a full PDI. It is missing the boot header", baseFile.c_str());
        bootloaderFound = false;
        if (smap_exists)
        {
            src.seekg(sizeof(Versal_2ve_2vmSmapWidthTable));
        }
        else
        {
            src.seekg(0);
        }
    }
    else
    {
        importedBh = std::make_unique<Versal_2ve_2vmBootHeader>(src, arch);
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
        if (options.IsVersalNetSeries())
        {
            if (it->delayAuth || (it->authType != Authentication::None))
            {
                attributes |= BH_RSA_SINGED_BIT_MASK << BH_RSA_SINGED_BIT_SHIFT;
            }
        }
        if (authHash != AuthHash::Sha3)
        {
            attributes |= authHash << AUTH_HASH_BIT_SHIFT;
        }
        if (options.bifOptions->GetDpaCM() != DpaCM::DpaCMDisable)
        {
            attributes |= options.bifOptions->GetDpaCM() << DPA_CM_BIT_SHIFT;
        }
        if (options.bifOptions->GetDice() != DICE::DiceDisable)
        {
            attributes |= options.bifOptions->GetDice() << BH_DICE_BIT_SHIFT;
        }

        importedBh->SetBHAttributes(attributes);

        /* If imported header has a non-zero source offset, it must be a FSBL header, so it must be copied */
        if (importedBh->GetSourceOffset() != 0)
        {
            LOG_INFO("Copying bootheader from %s ", baseFile.c_str());
            SetAssumeEncryptionFlag(false);
            options.SetEncryptedKeySource(importedBh->GetEncryptionKeySource());
            bootHeader->Copy(importedBh.get());
        }
        // Read hash block 0
        if (importedBh->GetHashBlockLength() != 0)
        {
            src.seekg(sizeof(Versal_2ve_2vmBootHeaderStructure));
            hashBlockLength = importedBh->GetHashBlockLength();
            hashBlock = std::make_unique<uint32_t[]>(hashBlockLength / sizeof(uint32_t));
            src.read((char*)hashBlock.get(), hashBlockLength);
        }

        src.seekg(importedBh->GetImageHeaderByteOffset());
    }

    CheckForIhtAttributes(baseFile);
    imageHeaderTable = std::make_unique<Versal_2ve_2vmImageHeaderTable>(src);

    if (imageHeaderTable->iht_optional_data_length != 0)
    {
        if (importedBh != NULL){
        src.seekg(importedBh->GetImageHeaderByteOffset() + sizeof(Versal_2ve_2vmImageHeaderTableStructure));
        }
        else{
            src.seekg(sizeof(Versal_2ve_2vmImageHeaderTableStructure));
        }
        iht_optional_data_length = imageHeaderTable->iht_optional_data_length;
        iht_optional_data = std::make_unique<uint32_t[]>(iht_optional_data_length / sizeof(uint32_t));
        src.read((char*)iht_optional_data.get(), iht_optional_data_length);
    }
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
            auto subsys = std::make_unique<Versal_2ve_2vmSubSysImageHeader>(src);

            for (uint32_t i = 0; i < subsys->num_of_images; i++)
            {
                auto image_ptr = std::make_unique<Versal_2ve_2vmImageHeader>(src, (Versal_2ve_2vmImageHeaderStructure*)subsys->section->Data.get(), IsBootloader, i);
                ImageHeader *image = image_ptr.get();
                image->SetAlignment(it->alignment);
                image->SetOffset(it->offset);
                image->SetReserve(it->reserve, it->updateReserveInPh);
                image->SetLoad(it->load);
                image->SetStartup(it->startup);
                image->SetPartitionRevocationId(it->GetPartitionRevokeId());
                image->SetPrebuilt(true);


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
                imageList.push_back(image_ptr.release());
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
                        options.bifOptions->pmcDataBuffer = std::make_unique<uint8_t[]>(options.bifOptions->GetTotalpmcdataSize()).release();
                        memcpy(options.bifOptions->pmcDataBuffer, newImage->GetPartitionHeaderList().front()->partition->section->Data.get() + importedBh->GetTotalPmcFwLength() + 0x140, options.bifOptions->totalpmcdataSize);
                        image->SetPmcDataSizeIh(options.bifOptions->pmcdataSize);
                        image->SetTotalPmcDataSizeIh(options.bifOptions->GetTotalpmcdataSize());
                    }
                    if (importedBh->GetTotalPmcCdoLength() != 0 && options.bifOptions->GetPmcdataFile() != "")
                    {
                        image->SetReplacePmc(true);
                    }
                }

                /* Determine if we need to load in previous RSAAuthentication Information */
                for (std::list<PartitionHeader*>::iterator partHdr = newImage->GetPartitionHeaderList().begin(); partHdr != newImage->GetPartitionHeaderList().end(); partHdr++)
                {
                    PartitionHeader* ph = (*partHdr);
                    if (ph->GetAuthCertificateOffset() != 0)
                    {
                        authOnPartitionFound = true;
                        LOG_INFO("Loading AC context for section %s ", ph->section->Name.c_str());
                        binFile = fopen(it->filename.c_str(), "rb");
                        if (!binFile)
                        {
                            LOG_ERROR("Cannot read file %s", it->filename.c_str());
                        }

                        auto aC = std::make_unique<uint8_t[]>(sizeof(AuthCertificate4096Sha3PaddingHBStructure));
                        memset(aC.get(), 0, sizeof(AuthCertificate4096Sha3PaddingHBStructure));

                        if (!(fseek(binFile, ph->GetAuthCertificateOffset(), SEEK_SET)))
                        {
                            size_t result = fread(aC.get(), 1, sizeof(AuthCertificate4096Sha3PaddingHBStructure), binFile);
                            if (result != sizeof(AuthCertificate4096Sha3PaddingHBStructure))
                            {
                                LOG_ERROR("Error parsing Authentication Certificates from PDI file");
                            }
                        }
                        fclose(binFile);

                        Authentication::Type authtype = Authentication::None;
                        if (((*aC.get()) & 0xF3) == 0x02)
                        {
                            authtype = Authentication::ECDSA;
                        }
                        else if (((*aC.get()) & 0xF3) == 0x11)
                        {
                            authtype = Authentication::RSA;
                        }
                        else if (((*aC.get()) & 0xF3) == 0x22)
                        {
                            authtype = Authentication::ECDSAp521;
                        }
                        auto auth = std::make_unique<Versal_2ve_2vmAuthenticationContext>((AuthCertificate4096Sha3PaddingHBStructure*)aC.release(), authtype);

                        // load in previous certificate data
                        auto tempac = std::make_unique<Versal_2ve_2vmAuthenticationCertificate>(auth.get());
                        auth->preSigned = true;
                        tempac->fsbl = true;
                        ph->ac.push_back(tempac.release());
                        // Store context in container to keep it alive, pass raw pointer to image
                        newImage->SetAuthContext(auth.get());
                        authContexts.push_back(std::move(auth));
                    }
                }
            }
            offset += sizeof(Versal_2ve_2vmImageHeaderStructure);
            if ((getenv("BOOTGEN_MERGE_IMAGES_WITH_SAME_ID") == NULL))
            {
                if ((prev_image_block != current_image_block) || (this_bootimage == true))
                {
                    subSysImageList.push_back(subsys.release());
                    prev_image_block = current_image_block;
                }
                else
                {
                    for (std::list<SubSysImageHeader*>::iterator subSysHdr = subSysImageList.begin(); subSysHdr != subSysImageList.end(); subSysHdr++)
                    {
                        if ((*subSysHdr)->GetSubSystemId() == subsys->GetSubSystemId())
                        {
                            (*subSysHdr)->imgList.splice((*subSysHdr)->imgList.end(), (subsys->imgList));
                        }
                    }
                }
            }
            else
            {
                subSysImageList.push_back(subsys.release());
            }
        }
        else
        {
            src.seekg(offset);
            auto image_ptr = std::make_unique<Versal_2ve_2vmImageHeader>(src, IsBootloader);
        ImageHeader* image = image_ptr.get();

            image->SetAlignment(it->alignment);
            image->SetOffset(it->offset);
            image->SetReserve(it->reserve, it->updateReserveInPh);
            image->SetLoad(it->load);
            image->SetStartup(it->startup);
            image->SetPartitionRevocationId(it->GetPartitionRevokeId());

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
            imageList.push_back(image_ptr.release());

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
                    options.bifOptions->pmcDataBuffer = std::make_unique<uint8_t[]>(options.bifOptions->GetTotalpmcdataSize()).release();
                    memcpy(options.bifOptions->pmcDataBuffer, newImage->GetPartitionHeaderList().front()->partition->section->Data.get() + importedBh->GetTotalPmcFwLength() + 0x140, options.bifOptions->totalpmcdataSize);
                    image->SetPmcDataSizeIh(options.bifOptions->pmcdataSize);
                    image->SetTotalPmcDataSizeIh(options.bifOptions->GetTotalpmcdataSize());
                }
                if (importedBh->GetTotalPmcCdoLength() != 0 && options.bifOptions->GetPmcdataFile() != "")
                {
                    image->SetReplacePmc(true);
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
                    auto aC = std::make_unique<uint8_t[]>(sizeof(AuthCertificate4096Sha3PaddingHBStructure));
                    memset(aC.get(), 0, sizeof(AuthCertificate4096Sha3PaddingHBStructure));

                    if (!(fseek(binFile, ph->GetAuthCertificateOffset(), SEEK_SET)))
                    {
                        size_t result = fread(aC.get(), 1, sizeof(AuthCertificate4096Sha3PaddingHBStructure), binFile);
                        if (result != sizeof(AuthCertificate4096Sha3PaddingHBStructure))
                        {
                            LOG_ERROR("Error parsing Authentication Certificates from PDI file");
                        }
                    }
                    fclose(binFile);

                    Authentication::Type authtype = Authentication::None;
                    if (((*aC.get()) & 0xF3) == 0x02)
                    {
                        authtype = Authentication::ECDSA;
                    }
                    else if (((*aC.get()) & 0xF3) == 0x11)
                    {
                        authtype = Authentication::RSA;
                    }
                    else if (((*aC.get()) & 0xF3) == 0x22)
                    {
                        authtype = Authentication::ECDSAp521;
                    }
                    auto auth = std::make_unique<Versal_2ve_2vmAuthenticationContext>((AuthCertificate4096Sha3PaddingHBStructure*)aC.release(), authtype);

                    auto tempac = std::make_unique<Versal_2ve_2vmAuthenticationCertificate>(auth.get());
                    auth->preSigned = true;
                    tempac->fsbl = true;
                    ph->ac.push_back(tempac.release());
                    // Store context in container to keep it alive, pass raw pointer to image
                    newImage->SetAuthContext(auth.get());
                    authContexts.push_back(std::move(auth));
                }
            }
            offset += sizeof(Versal_2ve_2vmImageHeaderStructure);
        }
        this_bootimage = true;
    } while (imageCount != 0);
}

/******************************************************************************/
void Versal_2ve_2vmBootImage::CheckForIhtAttributes(std::string base_file)
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
void Versal_2ve_2vmBootImage::ValidateSecureAttributes(ImageHeader * image, BifOptions * bifoptions, PartitionBifOptions * partitionBifOptions)
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
    case Authentication::ECDSA:
    case Authentication::ECDSAp521:
    case Authentication::LMS_SHA2_256:
    case Authentication::LMS_SHAKE256:
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
ImageHeader* Versal_2ve_2vmBootImage::ParsePartitionDataToImage(BifOptions * bifoptions, PartitionBifOptions * partitionBifOptions)
{
    static int aie_elf_cnt = 0;
    static uint8_t slr_boot_cnt = 0;
    static uint8_t slr_cfg_cnt = 0;
    static std::list<SlrPdiInfo*> slrBootPdiInfo;
    static std::list<SlrPdiInfo*> slrConfigPdiInfo;

    auto image_ptr = std::make_unique<Versal_2ve_2vmImageHeader>(partitionBifOptions->filename);
    ImageHeader *image = image_ptr.get();
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
    image->SetPartitionRevocationId(partitionBifOptions->GetPartitionRevokeId());
    image->SetSpkRevocationId(partitionBifOptions->GetSPKRevokeId());
    image->SetDpacm(partitionBifOptions->dpaCM);
    image->SetPufHdLocation(partitionBifOptions->pufHdLoc);
    image->SetClusterNum(partitionBifOptions->clusterNum);
    image->SetLockStepFlag(partitionBifOptions->lockstep);
    image->SetDelayAuthFlag(partitionBifOptions->delayAuth);
    image->SetTcmBoot(partitionBifOptions->tcmBoot);

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
        auto slrPdi = std::make_unique<SlrPdiInfo>();
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
            slrBootPdiInfo.push_back(slrPdi.release());
            if (slr_boot_cnt == bifoptions->slrBootCnt)
            {
                image->SetSlrBootPartitions(slrBootPdiInfo);
                image->SetName("SSIT Boot Partition");
                image->SetSlrPartition(true);
                imageList.push_back(image_ptr.release());
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
            slrConfigPdiInfo.push_back(slrPdi.release());
            if (slr_cfg_cnt == bifoptions->slrConfigCnt)
            {
                image->SetSlrConfigPartitions(slrConfigPdiInfo);
                image->SetName("SSIT Config Partition");
                image->SetSlrPartition(true);
                imageList.push_back(image_ptr.release());
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
            imageList.push_back(image_ptr.release());
        }
        else
        {
            return NULL;
        }
    }
    else if (partitionBifOptions->partitionType == PartitionType::IMAGE_STORE_PDI)
    {
        auto imageStorePdi = std::make_unique<ImageStorePdiInfo>();
        imageStorePdi->file = partitionBifOptions->filename;
        std::ifstream s(imageStorePdi->file.c_str());
        if (!s)
        {
            LOG_ERROR("Cannot read file - %s ", imageStorePdi->file.c_str());
        }
        imageStorePdi->id = partitionBifOptions->imageStoreId;
        image->SetWriteImageStorePartitions(imageStorePdi.release());
        imageList.push_back(image_ptr.release());
    }
    else
    {
        imageList.push_back(image_ptr.release());
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
void Versal_2ve_2vmBootImage::OutputOptionalSecureDebugImage()
{
    std::string secureDebugImageFile = options.GetSecureDebugImageFile();

    if (options.GetSecureDebugAuthType() != Authentication::None)
    {
        Authentication::Type authType = options.GetSecureDebugAuthType();
        bool isHss = (authType == Authentication::HSS_SHA2_256 || authType == Authentication::HSS_SHAKE256);
        if (isHss) {
            authType = (authType == Authentication::HSS_SHA2_256) ? Authentication::LMS_SHA2_256 : Authentication::LMS_SHAKE256;
        }
        auto authCtx_ptr = std::make_unique<Versal_2ve_2vmAuthenticationContext>(this->currentAuthCtx.get(), authType);
        Versal_2ve_2vmAuthenticationContext* authCtx = authCtx_ptr.get();
        if (isHss) {
            authCtx->lmsOnly = false;
        }

        if (authCtx)
        {
            uint32_t authJtagImageSize = authCtx->GetAuthJtagImageSize();
            auto writedata = std::make_unique<uint8_t[]>(authJtagImageSize);
            memset(writedata.get(), 0, authJtagImageSize);

            authCtx->hashType = authHash;
            if (authCtx->ownsHash && authCtx->hash != nullptr) {
                delete authCtx->hash;
            }
            authCtx->hash = hash.get();
            authCtx->ownsHash = false;
            authCtx->CreateAuthJtagImage(options, writedata.get(), bifOptions->authJtagInfo);

            std::ofstream ofs;
            ofs.open(secureDebugImageFile.c_str(), std::ios::binary);

            if (!ofs)
            {
                LOG_ERROR("Cannot write output to file : %s", secureDebugImageFile.c_str());
            }
            else
            {
                uint32_t bytesToWrite = authJtagImageSize;
                Authentication::Type origAuthType = options.GetSecureDebugAuthType();
                if (origAuthType == Authentication::LMS_SHA2_256 || origAuthType == Authentication::LMS_SHAKE256 ||
                    origAuthType == Authentication::HSS_SHA2_256 || origAuthType == Authentication::HSS_SHAKE256)
                {
                    const uint8_t* p = writedata.get() + 4;
                    bytesToWrite = (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
                }
                ofs.write((const char*)writedata.get(), bytesToWrite);
                ofs.close();
                LOG_TRACE("Authenticated Jtag Image : '%s' generated.", secureDebugImageFile.c_str());
            }
        }
    }
}

/******************************************************************************/
void Versal_2ve_2vmBootImage::ConfigureEncryptionBlocks(ImageHeader * image, PartitionBifOptions * partitionBifOptions)
{
    if (partitionBifOptions->encryptType == Encryption::AES)
    {
        std::vector<uint32_t> encrBlocks = partitionBifOptions->GetEncryptionBlocks();
        uint32_t defaultEncrBlockSize = partitionBifOptions->GetDefaultEncryptionBlockSize();
        image->SetDefaultEncrBlockSize(defaultEncrBlockSize);

        if (image->IsBootloader() && !(options.IsVersalNetSeries()))
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
            Binary::Length_t secureChunkSize = GetSecureChunkSize(image->IsBootloader());

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
                            /* leftover (< per-block overhead) is
                             * too small to host another (data+overhead) block,
                             * but the cipher stream requires the chunk to be
                             * packed exactly. Drop the speculative "next
                             * default" (counted in encrBlocksSize/encrOverhead
                             * but never pushed to the list) and grow the most
                             * recently pushed block by the slack. The grown
                             * block is >= defaultEncrBlockSize, so when D
                             * itself satisfies the 5-AES-block
                             * (80 B) minimum, so does the final block.    */
                            std::vector<uint32_t>& list = image->GetEncrBlocksList();
                            if (!list.empty())
                            {
                                encrBlocksSize -= defaultEncrBlockSize;
                                encrOverhead   -= (SECURE_HDR_SZ + AES_GCM_TAG_SZ);
                                Binary::Length_t slack = secureChunkSize - (encrBlocksSize + encrOverhead);
                                list.back()    += slack;
                                encrBlocksSize += slack;
                                LOG_WARNING("The last encryption block size is enlarged to %d to fit into the secure chunk of 32KB.", (uint32_t)list.back());
                                /* The chunk is now exactly full; suppress the
                                 * trailing default-block push below.       */
                                lastBlock = 0;
                            }
                            else
                            {
                                LOG_ERROR("The keyrolling block size '%d' cannot fit into the secure chunk of 32KB. Please choose another block size.\n           For details, refer to the section 'Design Advisories for Bootgen' from UG1283.", defaultEncrBlockSize);
                            }
                        }
                    }
                    if (lastBlock != 0)
                    {
                        image->InsertEncrBlocksList(lastBlock);
                    }
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
void Versal_2ve_2vmBootImage::SetPmcdataFile(const std::string& filename)
{
    options.bifOptions->SetPmcdataFile(filename);
}

/******************************************************************************/
void Versal_2ve_2vmBootImage::ConfigureProcessingStages(ImageHeader* image, PartitionBifOptions* partitionbifoptions)
{
    ConfigureEncryptionContext(image, partitionbifoptions->encryptType);

    // if(!partitionbifoptions->bootloader)
    //     partitionbifoptions->authType = Authentication::None;

    ConfigureAuthenticationContext(image, partitionbifoptions->authType, partitionbifoptions);
    
    if (partitionbifoptions->authType == Authentication::None && partitionbifoptions->encryptType == Encryption::None)
        partitionbifoptions->checksumType = Checksum::SHA3;
    
    ConfigureChecksumContext(image, partitionbifoptions->checksumType);
    image->SetPartOwner(partitionbifoptions->ownerType);
}

/******************************************************************************/
void Versal_2ve_2vmBootImage::Add(BifOptions* bifoptions)
{
    // TlBin generation and swaping the .dtb file with tl.bin
    if(options.GetGenerateTlBin())
    {
        GenerateTlBin(bifoptions);
    }

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
            std::unique_ptr<Versal_2ve_2vmAuthenticationContext> authCtx;
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
                    authCtx = std::make_unique<Versal_2ve_2vmAuthenticationContext>(currentAuthCtx.get(), Authentication::RSA);
                }
                else if(eckeyLocal != NULL)
                {
                    authCtx = std::make_unique<Versal_2ve_2vmAuthenticationContext>(currentAuthCtx.get(), Authentication::ECDSA);
                }
            }

            /*
            if (options.DoGenerateHashes())
            {
                authCtx->hash = hash;
                authCtx->spkIdentification = bifOptions->GetRevokeId();
                LOG_INFO("Generating SPK Hash File");
                authCtx->GenerateSPKHashFile(bifoptions->GetSPKFileName(), hash);
            }
            */

            // authCtx is unique_ptr - automatically cleaned up when going out of scope
        }

        if (bifoptions->GetSSKFileName() != "")
        {
            currentAuthCtx->SetSSKeyFile(bifoptions->GetSSKFileName());
        }
    }

    //Copying primary and secondary LmsKeyParam from bifOptions to authCxt
    std::vector<LmsKeyParam> primaryLmsParams = bifOptions->GetPrimaryLmsParams();
    currentAuthCtx->primaryLmsParamsSize = primaryLmsParams.size() * 2;
    currentAuthCtx->primaryLmsParams = new int[currentAuthCtx->primaryLmsParamsSize];
    for (int i = 0, j = 0; i < currentAuthCtx->primaryLmsParamsSize && j < (int)primaryLmsParams.size(); i=i+2, j++)
    {
        currentAuthCtx->primaryLmsParams[i] = primaryLmsParams[j].h;
        currentAuthCtx->primaryLmsParams[i+1] = primaryLmsParams[j].w;
    }

    std::vector<LmsKeyParam> secondaryLmsParams = bifOptions->GetSecondaryLmsParams();
    currentAuthCtx->secondaryLmsParamsSize = secondaryLmsParams.size() * 2;
    currentAuthCtx->secondaryLmsParams = new int[currentAuthCtx->secondaryLmsParamsSize];
    for (int i = 0, j = 0; i < currentAuthCtx->secondaryLmsParamsSize && j < (int)secondaryLmsParams.size(); i=i+2, j++)
    {
        currentAuthCtx->secondaryLmsParams[i] = secondaryLmsParams[j].h;
        currentAuthCtx->secondaryLmsParams[i+1] = secondaryLmsParams[j].w;
    }

    if (bifoptions->GetSPKSignFileName() != "")
    {
        LOG_TRACE("Parsing SPK Signature File");

        if(options.GetSecureDebugAuthType() == Authentication::LMS_SHA2_256 || options.GetSecureDebugAuthType() == Authentication::LMS_SHAKE256
           || options.GetSecureDebugAuthType() == Authentication::HSS_SHA2_256 || options.GetSecureDebugAuthType() == Authentication::HSS_SHAKE256)
        {
            currentAuthCtx->signatureLength = GetLmsSignatureLength(currentAuthCtx->primaryLmsParams, currentAuthCtx->primaryLmsParamsSize,
                currentAuthCtx->pskFile.c_str(), currentAuthCtx->ppkFile.c_str(), currentAuthCtx->lmsOnly);
        }
        currentAuthCtx->spksignature = std::make_unique<uint8_t[]>(currentAuthCtx->signatureLength);
        memset(currentAuthCtx->spksignature.get(), 0, currentAuthCtx->signatureLength);
        currentAuthCtx->SetSPKSignatureFile(bifoptions->GetSPKSignFileName());
    }

    if (bifoptions->GetBHSignFileName() != "")
    {
        LOG_TRACE("Parsing BH Signature File");
        currentAuthCtx->bHsignature = std::make_unique<uint8_t[]>(currentAuthCtx->signatureLength);
        memset(currentAuthCtx->bHsignature.get(), 0, currentAuthCtx->signatureLength);
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
                    bifoptions->SetPmcDataAesFile((*itr)->aesKeyFile);
                }
                //If no key file found in partition specific attributes - Generate aeskeyfile with partition_name.nky
                else
                {
                    bifoptions->SetPmcDataAesFile(StringUtils::RemoveExtension(StringUtils::BaseName((*itr)->filename)) + ".nky");
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
            auto subSysImage = std::make_unique<Versal_2ve_2vmSubSysImageHeader>(*imgitr);
            bool bootimage_partition = false;
            current_image_block++;
            bool break_outer_loop = false;
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
                        bifoptions->SetPmcDataAesFile((*partitr)->aesKeyFile);
                    }
                    else
                    {
                        //If no key file found in partition specific attributes - Generate aeskeyfile with partition_name.nky
                        bifoptions->SetPmcDataAesFile(StringUtils::RemoveExtension(StringUtils::BaseName((*partitr)->filename)) + ".nky");
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
                        if ((getenv("BOOTGEN_MERGE_IMAGES_WITH_SAME_ID") == NULL))
                        {
                            if (bootimage_partition == true)
                            {
                                for (std::list<SubSysImageHeader*>::iterator ssitr = subSysImageList.begin(); ssitr != subSysImageList.end(); ssitr++)
                                {
                                    if (((*ssitr)->GetSubSystemId() == subSysImage->GetSubSystemId()) && !(img->IsBootloader() || img->GetDestCpu() == DestinationCPU::PMU))
                                    {
                                        (*ssitr)->imgList.push_back(img);
                                        break_outer_loop = true;
                                    }
                                }
                            }
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
                            if (!break_outer_loop)                            
                                subSysImage->imgList.push_back(img);
                        }
                    }
                }
            }
            /* Add to subsys list only if it the partition type is not bootimage. Because bootimage will have its own subsystems */
            if ((!bootimage_partition) || (subSysImage->imgList.size() != 0))
            {
                subSysImageList.push_back(subSysImage.release());
            }
        }
    }
}

/******************************************************************************/
void Versal_2ve_2vmBootImage::ReplaceImages(void)
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
                (*img)->SetReplacePsm(true);
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
                (*img)->SetReplacePlm(true);
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
                    img1->SetPrebuilt((*it1)->GetPrebuilt());
                    img1->SetReplacePmc((*it1)->GetReplacePmc());
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
                    PSMimg->SetPrebuilt((*it2)->GetPrebuilt());
                    (*subsys)->imgList.erase(it2);
                    (*subsys)->imgList.push_back(PSMimg);
                    break;
                    
                }
            }
        }
    }
}

/******************************************************************************/
void Versal_2ve_2vmBootImage::ReplaceImagesNoSubSys(void)
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
void Versal_2ve_2vmBootImage::OutputOptionalEfuseHash()
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

                std::unique_ptr<Versal_2ve_2vmAuthenticationContext> authCtx;
                if (rsa != NULL)
                {
                    authCtx = std::make_unique<Versal_2ve_2vmAuthenticationContext>(currentAuthCtx.get(), Authentication::RSA);
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
                        authCtx = std::make_unique<Versal_2ve_2vmAuthenticationContext>(currentAuthCtx.get(), Authentication::ECDSA);
                    }
                    else if (ecCurveNID == NID_secp521r1)
                    {
                        authCtx = std::make_unique<Versal_2ve_2vmAuthenticationContext>(currentAuthCtx.get(), Authentication::ECDSAp521);
                    }
                    else
                    {
                        LOG_ERROR("Unsupported ECDSA curve read from key file : %s\n           Supported ECDSA curves: P384, P521", primaryKeyFile.c_str());
                    }
                }
                else
                {
                    // Consider LMS key
                    // TO-DO - Check if there is a way to identify LMS key
                    // LOG_ERROR("Cannot read the public key file : %s", filename.c_str());
                    // Decide LMS SHA256/SHAKE256 based on the params read from key file. For timebeing treating as LMS_SHA256
                    authCtx = std::make_unique<Versal_2ve_2vmAuthenticationContext>(currentAuthCtx.get(), Authentication::LMS_SHA2_256);
                }

                if (authCtx->ownsHash && authCtx->hash != nullptr) {
                    delete authCtx->hash;
                }
                authCtx->hash = hash.get();
                authCtx->ownsHash = false;
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

                std::unique_ptr<Versal_2ve_2vmAuthenticationContext> authCtx;
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
                        authCtx = std::make_unique<Versal_2ve_2vmAuthenticationContext>(currentAuthCtx.get(), Authentication::ECDSA);
                    }
                    else if (ecCurveNID == NID_secp521r1)
                    {
                        authCtx = std::make_unique<Versal_2ve_2vmAuthenticationContext>(currentAuthCtx.get(), Authentication::ECDSAp521);
                    }
                    else
                    {
                        LOG_ERROR("Unsupported ECDSA curve read from key file : %s\n           Supported ECDSA curves: P384, P521", primaryKeyFile.c_str());
                    }
                }
                else
                {
                    authCtx = std::make_unique<Versal_2ve_2vmAuthenticationContext>(currentAuthCtx.get(), Authentication::RSA);
                }
                if (authCtx->ownsHash && authCtx->hash != nullptr) {
                    delete authCtx->hash;
                }
                authCtx->hash = hash.get();
                authCtx->ownsHash = false;
                authCtx->GeneratePPKHash(hashFile);
            }
        }
        else
        {
            LOG_ERROR("Cannot read PPK/PSK. PPK/PSK is mandatory to generate PPK hash bits, using '-efuseppkbits'.");
        }
    }
}
#if 0
/******************************************************************************/
void Versal_2ve_2vmBootImage::OutputOptionalEfusePufHash()
{
    std::string hashFile = options.GetEfusePufHashFileName();
    if (hashFile != "")
    {
        if (bifOptions->GetPufHelperFile() != "")
        {
            auto pufDataTemp = std::make_unique<uint8_t[]>(PUF_DATA_LENGTH_4K);
            memset(pufDataTemp.get(), 0, PUF_DATA_LENGTH_4K);

            FileImport fileReader;
            if (!fileReader.LoadHexData(bifOptions->GetPufHelperFile(), pufDataTemp.get(), PUF_DATA_LENGTH_4K - PUF_DATA_LENGTH_4K_ALIGNMENT))
            {
                LOG_ERROR("Invalid no. of data bytes for PUF Helper Data.\n           Expected length for PUF Helper Data is %d bytes", PUF_DATA_LENGTH_4K - PUF_DATA_LENGTH_4K_ALIGNMENT);
            }

            auto pufHash = std::make_unique<uint8_t[]>(hash->GetHashLength());
            hash->CalculateHash(true, pufDataTemp.get(), PUF_DATA_LENGTH_4K, pufHash.get());

            FILE* filePtr;
            if ((filePtr = fopen(hashFile.c_str(), "w")) == NULL)
            {
                LOG_ERROR("-efusepufbits error !!!           Failure writing to hash file %s", StringUtils::BaseName(hashFile).c_str());
            }

            /* For Versal, the efuses are available only for 256 bits.
            So the upper 256 bits(0x20 bytes) of the hash is dumped, which will be programmed to efuse.*/
            for (int index = 0; index < hash->GetHashLength(); index++)
            {
                fprintf(filePtr, "%02X", pufHash.get()[index]);
            }
            fprintf(filePtr, "\r\n");

            fclose(filePtr);
            LOG_INFO("Efuse PPK bits written to file %s successfully", hashFile.c_str());

            delete[] pufDataTemp;
        }
        else
        {
            LOG_ERROR("Cannot read PUF File. PUF Helper Data is mandatory to generate PUF hash bits, using '-efusepufkbits'.");
        }
    }
}

/******************************************************************************/
void Versal_2ve_2vmBootImage::OutputOptionalPufPDI()
{
    if (options.GetDumpOption() == DumpOption::PUF_PDI)
    {
        FILE* filePtr;
        size_t result;
        std::string fName;
        if (!options.GetPufOutputFileName().empty()) 
        {
            fName = options.GetPufOutputFileName();
        } 
        else 
        {
            fName = options.GetDumpDirectory();
            fName += "puf.bin";
        }
        filePtr = fopen(fName.c_str(), "wb");

        auto pufPDITemp = std::make_unique<uint8_t[]>(bootHeader->section->Length);
        memcpy(pufPDITemp.get(), bootHeader->section->Data.get(), bootHeader->section->Length);

        Versal_2ve_2vmBootHeaderStructure *bHTable = (Versal_2ve_2vmBootHeaderStructure*)pufPDITemp.get();
        bHTable->pufPDIIdentificationWord = PUF_IMAGE_ID_WORD;
        bHTable->bhAttributes |= bifOptions->GetPufMode() << BH_PUF_MODE_BIT_SHIFT;
        
        bHTable->headerChecksum = bootHeader->ComputeWordChecksum(&bHTable->widthDetectionWord, bootHeader->GetBootHeaderSize() - sizeof(uint32_t) - sizeof(Versal_2ve_2vmSmapWidthTable));

        if (filePtr != NULL)
        {
            result = fwrite(pufPDITemp.get(), 1, bootHeader->section->Length, filePtr);
            if (result != bootHeader->section->Length)
            {
                LOG_ERROR("Error dumping PUF PDI to a file");
            }
            fclose(filePtr);
        }
        LOG_INFO("PUF PDI file created - %s", fName.c_str());
    }
}
#endif
/******************************************************************************/
void Versal_2ve_2vmBootImage::AppendImagesInSubsystems(void)
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
void Versal_2ve_2vmBootImage::BuildAndLink(Binary* cache)
{
    if (imageList.size() == 0 && options.GetSecureDebugAuthType() == Authentication::None)
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
            auto imgOptions = std::make_unique<ImageBifOptions>();
            /* If Subsystems are not specified in BIF - create one image header for PLM and other image header for subsytem and add all partitions to it */
            imgOptions->SetImageName("default_subsys");
            auto sub_sys_image = std::make_unique<Versal_2ve_2vmSubSysImageHeader>(imgOptions.get());
            for (std::list<ImageHeader*>::iterator image = imageList.begin(); image != imageList.end(); image++)
            {
                if ((*image)->IsBootloader())
                {
                    imgOptions->SetImageName("pmc_subsys");
                    auto plm_header = std::make_unique<Versal_2ve_2vmSubSysImageHeader>(imgOptions.get());
                    plm_header->imgList.push_back((*image));
                    plm_header->SetSubSystemName("pmc_subsys");
                    plm_header->SetSubSystemId(0x1c000001);
                    subSysImageList.push_back(plm_header.release());
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
                subSysImageList.push_back(sub_sys_image.release());
                LOG_INFO("BOOTGEN_SUBSYSTEM_PDI is set, but no subsystems are specified, all partitions are grouped into one default subsystem");
            }
         }
    }

    DetermineEncryptionDefaults();
    partitionHeaderList.clear();
    if (createSubSystemPdis == true)
    {
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
    #ifdef DEBUG
    // DEBUG: Check BootHeader auth fields BEFORE StackAndAlign
    auto* bh_versal = dynamic_cast<Versal_2ve_2vmBootHeader*>(bootHeader.get());
    if (bh_versal) {
        uint32_t* bh_data = (uint32_t*)bh_versal->section->Data.get();
        fprintf(stderr, "[BEFORE-STACK] bh_data=%p, BootHeader auth fields: totalPPK=0x%x, actualPPK=0x%x, totalSig=0x%x, actualSig=0x%x\n",
                (void*)bh_data, bh_data[0x288/4], bh_data[0x28C/4], bh_data[0x290/4], bh_data[0x294/4]);
    }
	#endif
    /* Stack and alignment stage */
    /* Once the header tables are created, stack all the tables and do the necessary alignment */
    cache->StackAndAlign(options);

    LOG_INFO("After align ");
    LOG_DUMP_IMAGE(*cache);
    #ifdef DEBUG
    // DEBUG: Check BootHeader auth fields AFTER StackAndAlign
    if (bh_versal) {
        uint32_t* bh_data = (uint32_t*)bh_versal->section->Data.get();
        fprintf(stderr, "[AFTER-STACK] BootHeader auth fields: totalPPK=0x%x, actualPPK=0x%x, totalSig=0x%x, actualSig=0x%x\n",
                bh_data[0x288/4], bh_data[0x28C/4], bh_data[0x290/4], bh_data[0x294/4]);
    }
	#endif
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

/******************************************************************************/
uint64_t Versal_2ve_2vmBootImage::GetSecureChunkSize(bool isBootloader)
{
    if (isBootloader == true)
    {
        return (SECURE_16K_CHUNK - hash->GetHashLength());
    }
    else
    {
        return (SECURE_32K_CHUNK - hash->GetHashLength());
    }
}

/******************************************************************************/
void Versal_2ve_2vmBootImage::GenerateTlBin(BifOptions* bifoptions)
{
    std::string dtbFilepath;
    uint64_t teeLoadAddr = 0;
    uint64_t ubootExecAddr = 0;
    bool teeTrustZone = false;
    bool ubootTrustZone = false;
    PartitionBifOptions* dtbPartition = nullptr;

    for (auto imageItr = bifoptions->imageBifOptionList.begin(); imageItr != bifoptions->imageBifOptionList.end(); imageItr++)
    {
        if ((*imageItr)->GetImageName() == "apu_ss")
        {
            for (auto partitionItr = (*imageItr)->partitionBifOptionsList.begin(); partitionItr != (*imageItr)->partitionBifOptionsList.end(); partitionItr++)
            {
                std::string filename = (*partitionItr)->filename;
                if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".dtb")
                {
                    dtbFilepath = filename;
                    dtbPartition = *partitionItr;
                }

                if((*partitionItr)->exceptionLevel == ExceptionLevel::EL1)
                {
                    teeTrustZone = (*partitionItr)->trustzone == TrustZone::Secure;
                    if ((*partitionItr)->load.IsSet()) 
                    {
                        teeLoadAddr = (*partitionItr)->load.Value();
                    }
                }

                if((*partitionItr)->exceptionLevel == ExceptionLevel::EL2)
                {
                    // load address from ELF file
                    std::string ubootFilename = (*partitionItr)->filename;
                    ByteFile elfdata(ubootFilename);
                    ElfClass::Type elfclass = (ElfClass::Type)elfdata.bytes[EI_CLASS];
                    uint8_t proc_state = 0;
                    auto elf = ElfFormat::GetElfFormat(elfclass, elfdata.bytes, &proc_state, elfdata.len);
                    ubootExecAddr = elf->GetStartUpAddress();
                    delete elf;

                    ubootTrustZone = (*partitionItr)->trustzone == TrustZone::Secure;
                }
            }
        }
    }

    LOG_TRACE("tl.bin parameters: teeLoadAddr=0x%lx, ubootExecAddr=0x%lx, teeTrustZone=%d, ubootTrustZone=%d, dtbFilepath=%s",
        teeLoadAddr, ubootExecAddr, teeTrustZone, ubootTrustZone, dtbFilepath.c_str());

    std::string yaml;
    yaml += "execution_state: aarch64\n";
    yaml += "has_checksum: true\n";
    yaml += "max_size: 6291456\n";
    yaml += "entries:\n";

    yaml += "        - tag_id: 258\n";
    yaml += "          ep_info:\n";
    yaml += "                  args:\n";
    for (int i = 0; i < 8; i++)
        yaml += "                          - 0\n";
    yaml += "                  h:\n";
    yaml += "                          attr: " + std::to_string(teeTrustZone ? 0 : 1) + "\n";
    yaml += "                          type: " + std::to_string(teeTrustZone ? 0 : 1) + "\n";
    yaml += "                          version: 1\n";
    yaml += "                  pc: " + std::to_string(teeLoadAddr) + "\n";
    yaml += "                  spsr: 0\n";

    yaml += "        - tag_id: 258\n";
    yaml += "          ep_info:\n";
    yaml += "                  args:\n";
    for (int i = 0; i < 8; i++)
        yaml += "                          - 0\n";
    yaml += "                  h:\n";
    yaml += "                          attr: " + std::to_string(ubootTrustZone ? 0 : 1) + "\n";
    yaml += "                          type: " + std::to_string(ubootTrustZone ? 0 : 1) + "\n";
    yaml += "                          version: 1\n";
    yaml += "                  pc: " + std::to_string(ubootExecAddr) + "\n";
    yaml += "                  spsr: 969\n";

    yaml += "        - tag_id: 1\n";
    yaml += "          blob_file_path: \"" + dtbFilepath + "\"\n";

    std::string yamlOutputPath = "tl_generated.yaml";
    std::ofstream yamlFile(yamlOutputPath);
    if (!yamlFile)
    {
        LOG_ERROR("Cannot create YAML file: %s", yamlOutputPath.c_str());
    }
    yamlFile << yaml;
    yamlFile.close();

    std::string tlbinOutputPath = "tl.bin";
    char* tlc_argv[] = {
        const_cast<char*>("tlc"),
        const_cast<char*>("create"),
        const_cast<char*>("--from-yaml"),
        const_cast<char*>(yamlOutputPath.c_str()),
        const_cast<char*>(tlbinOutputPath.c_str())
    };
    int tlc_argc = 5;
    int result = create_tlbin(tlc_argc, tlc_argv);
    if (result != 0)
    {
        LOG_ERROR("Failed to generate TL.BIN");
    }
    else
    {
        LOG_INFO("TL.BIN generated successfully");
    }

    if (dtbPartition != nullptr)
    {
        dtbPartition->filename = tlbinOutputPath;
    }
    LOG_INFO("Replaced %s with %s in the BIF", dtbFilepath.c_str(), dtbPartition->filename.c_str());
}
