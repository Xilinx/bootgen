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
#include "bootimage-versal_2vp.h"
#include "partitionheadertable-versal_2vp.h"
#include "binary-versal_2vp.h"
#include "checksum-versal_2vp.h"
#include "authentication-versal_2vp.h"
#include "authentication-versal_2vp-hybrid.h"
#include "authentication.h"
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
Versal_2vpBootImage::Versal_2vpBootImage(Options& options, uint8_t index) : BootImage(options, index)
{
    partitionHeaderList.clear();
    options.SetDefaultAlignment(16);
    bootHeader = std::make_unique<Versal_2vpBootHeader>(arch);
    imageHeaderTable = std::make_unique<Versal_2vpImageHeaderTable>();
    partitionHeaderTable = std::make_unique<Versal_2vpPartitionHeaderTable>();
    currentEncryptCtx = std::make_unique<Versal_2vpEncryptionContext>();
    currentAuthCtx = std::make_unique<Versal_2vpAuthenticationContext>(Authentication::RSA);
    SetLegacyEncryptionFlag(true);
    partitionOutput = std::make_unique<VersalPartitionOutput>();
    hash = std::make_unique<HashSha3>();
    hashBlockLength = 0;
    hashBlockSection = nullptr;
    bootloaderHashBlockSection = NULL;
    cache = std::make_unique<Versal_2vpBinary>();
    checksumTable = std::make_unique<Versal_2vpChecksumTable>();
    currentAuthCtx->hashType = AuthHash::Sha3;
    currentAuthCtx->hash = hash.get();  // Non-owning reference to BootImage's hash
    currentAuthCtx->ownsHash = false;
    partitionHeaderTable->firstSection = NULL;
    convertAieElfToCdo = true;
    current_image_block = 0;
    createSubSystemPdis = true;
    char * env_aie = getenv("BOOTGEN_AIE_ELF_FLOW");
    if (env_aie != NULL)
    {
        if ((strcmp(env_aie, "true") == 0 || strcmp(env_aie, "1") == 0))
        {
            convertAieElfToCdo = false;
            LOG_INFO("BOOTGEN_AIE_ELF_FLOW is enabled");
        }
    }
}

/******************************************************************************/
Versal_2vpBootImage::~Versal_2vpBootImage()
{
}

/******************************************************************************/
void Versal_2vpBootImage::ConfigureEncryptionContext(ImageHeader * image, Encryption::Type encryptType)
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
            imageHeaderTable->SetEncryptContext(std::make_unique<Versal_2vpEncryptionContext>(this->currentEncryptCtx.get()));
        }
        image->SetEncryptContext(std::make_unique<Versal_2vpEncryptionContext>(this->currentEncryptCtx.get()));
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
                imageHeaderTable->SetEncryptContext(std::make_unique<Versal_2vpEncryptionContext>());
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
void Versal_2vpBootImage::ConfigureAuthenticationContext(ImageHeader * image, Authentication::Type authType, PartitionBifOptions * partitionbifoptions)
{
    /* Configure the authentication context */
    std::string spkSignFile = options.GetSpkSigFileName();
    
    // Check for hybrid authentication first
    if (partitionbifoptions->IsHybridAuthentication())
    {
        std::vector<Authentication::Type> hybridAlgorithms = partitionbifoptions->GetHybridAlgorithms();
        
        if (image->IsBootloader())
        {
            bootloaderAuthenticate = true;
        }
        if (bootloaderFound && !bootloaderAuthenticate)
        {
            LOG_ERROR("Bootloader must be authenticated to authenticate rest of the partitions");
        }
        
        // Set authentication type to the primary algorithm for compatibility
        image->SetAuthenticationType(hybridAlgorithms[0]);
        options.bifOptions->SetHeaderAC(true);
        authOnPartitionFound = true;
        currentAuthCtx->hashType = GetAuthHashAlgo();
        
        // Set up currentAuthCtx with key information - exactly like single signing
        // BACKWARD COMPATIBILITY: Fall back to global bifOptions keys if partition-level keys are not set
        // This ensures the old approach (pskfile=xxx.pem at BIF global level) still works
        currentAuthCtx->ppkFile = !image->GetPpkFile().empty() ? image->GetPpkFile() : options.bifOptions->GetPPKFileName();
        currentAuthCtx->pskFile = !image->GetPskFile().empty() ? image->GetPskFile() : options.bifOptions->GetPSKFileName();
        currentAuthCtx->spkFile = !image->GetSpkFile().empty() ? image->GetSpkFile() : options.bifOptions->GetSPKFileName();
        currentAuthCtx->sskFile = !image->GetSskFile().empty() ? image->GetSskFile() : options.bifOptions->GetSSKFileName();
        currentAuthCtx->spkSignFile = image->GetSpkSignFile();
        currentAuthCtx->spkIdentification = image->GetSpkRevocationId();
        currentAuthCtx->SetSPKSignatureFile(image->GetSpkSignFile());
        currentAuthCtx->lmsOnly = partitionbifoptions->lmsOnly;

        if (image->GetBhSignFile() != "")
        {
            currentAuthCtx->bhSignFile = image->GetBhSignFile();
        }

        // Set key length based on primary algorithm - exactly like single signing
        AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
        if (hybridAlgorithms[0] == Authentication::ECDSA)
        {
            AuthenticationContext::SetAuthenticationKeyLength(EC_P384_KEY_LENGTH);
        }
        else if (hybridAlgorithms[0] == Authentication::ECDSAp521)
        {
            AuthenticationContext::SetAuthenticationKeyLength(EC_P521_KEY_LENGTH2 * 2);
        }
        else if (hybridAlgorithms[0] == Authentication::MLDSA)
        {
            AuthenticationContext::SetAuthenticationKeyLength(MLDSA_PUB_KEY_LENGTH);
        }
        else if (hybridAlgorithms[0] == Authentication::SLH_SHAKE256)
        {
            AuthenticationContext::SetAuthenticationKeyLength(SLHDSA_PUB_KEY_LENGTH);
        }
        else if (hybridAlgorithms[0] == Authentication::LMS_SHA2_256 || hybridAlgorithms[0] == Authentication::LMS_SHAKE256)
        {
            // For LMS algorithms, use dynamic key length based on the SPK file
            size_t lmsKeyLength = GetLmsPublicKeyLength(currentAuthCtx->spkFile.c_str(), currentAuthCtx->lmsOnly);
            AuthenticationContext::SetAuthenticationKeyLength(lmsKeyLength);
        }

        // Create hybrid authentication context EXACTLY like single signing does
        // This ensures proper initialization using the working constructor pattern
        image->SetAuthContext(new Versal_2vpHybridAuthenticationContext(currentAuthCtx.get(), hybridAlgorithms));
        AuthenticationContext* authCtx = image->GetAuthContext();
        authCtx->SetPresignFile(partitionbifoptions->presignFile);
        authCtx->SetUdfFile(partitionbifoptions->udfDataFile);

        if (spkSignFile != "")
        {
            currentAuthCtx->GenerateSPKSignature(spkSignFile);
            currentAuthCtx->spkSignRequested = spkSignFile;
        }
    }    else
    {
        // Standard single-algorithm authentication
    switch (authType)
    {
    case Authentication::RSA:
    case Authentication::ECDSA:
    case Authentication::ECDSAp521:
    case Authentication::LMS_SHA2_256:
    case Authentication::LMS_SHAKE256:
    case Authentication::MLDSA:
    case Authentication::SLH_SHAKE256:
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
        // BACKWARD COMPATIBILITY: Fall back to global bifOptions keys if partition-level keys are not set
        // This ensures the old approach (pskfile=xxx.pem at BIF global level) still works
        currentAuthCtx->ppkFile = !image->GetPpkFile().empty() ? image->GetPpkFile() : options.bifOptions->GetPPKFileName();
        currentAuthCtx->pskFile = !image->GetPskFile().empty() ? image->GetPskFile() : options.bifOptions->GetPSKFileName();
        currentAuthCtx->spkFile = !image->GetSpkFile().empty() ? image->GetSpkFile() : options.bifOptions->GetSPKFileName();
        currentAuthCtx->sskFile = !image->GetSskFile().empty() ? image->GetSskFile() : options.bifOptions->GetSSKFileName();
        currentAuthCtx->spkSignFile = image->GetSpkSignFile();
        currentAuthCtx->spkIdentification = image->GetSpkRevocationId();
        currentAuthCtx->SetSPKSignatureFile(image->GetSpkSignFile());
        currentAuthCtx->lmsOnly = partitionbifoptions->lmsOnly;

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
            AuthenticationContext::SetAuthenticationKeyLength(EC_P521_KEY_LENGTH2 * 2);
        }
        else if (authType == Authentication::MLDSA)
        {
            AuthenticationContext::SetAuthenticationKeyLength(MLDSA_PUB_KEY_LENGTH);
        }
        else if (authType == Authentication::SLH_SHAKE256)
        {
            AuthenticationContext::SetAuthenticationKeyLength(SLHDSA_PUB_KEY_LENGTH);
        }
        else if (authType == Authentication::LMS_SHA2_256 || authType == Authentication::LMS_SHAKE256)
        {
            // For LMS algorithms, use dynamic key length based on the SPK file
            size_t lmsKeyLength = GetLmsPublicKeyLength(currentAuthCtx->spkFile.c_str(), currentAuthCtx->lmsOnly);
            AuthenticationContext::SetAuthenticationKeyLength(lmsKeyLength);
        }

        image->SetAuthContext(new Versal_2vpAuthenticationContext(currentAuthCtx.get(), authType));
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
        }  // end switch
    }  // end else
}

/******************************************************************************/
void Versal_2vpBootImage::ConfigureChecksumContext(ImageHeader * image, Checksum::Type checksumType)
{
    /* Configure the checksum context */
    switch (checksumType)
    {
    case Checksum::MD5:
    case Checksum::SHA2:
        LOG_ERROR("BIF attribute error !!!\n\t\t 'checksum=sha2/md5' is not supported in VERSAL architecture.");
        break;

    case Checksum::SHA3:
        image->SetChecksumContext(std::make_unique<Versal_2vpSHA3ChecksumContext>());
        break;

    case Checksum::None:
    default:
        image->SetChecksumContext(std::make_unique<NoneChecksumContext>());
        break;
    }
}

/******************************************************************************/
void Versal_2vpBootImage::ParseBootImage(PartitionBifOptions* it)
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

    Versal_2vpBootHeaderStructure* bH = new Versal_2vpBootHeaderStructure;
    fread(bH, 1, sizeof(Versal_2vpBootHeaderStructure), binFile);

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

    Versal_2vpBootHeader* importedBh = NULL;
    if (!full_pdi)
    {
        LOG_WARNING("File %s is not a full PDI. It is missing the boot header", baseFile.c_str());
        bootloaderFound = false;
        if (smap_exists)
        {
            src.seekg(sizeof(Versal_2vpSmapWidthTable));
        }
        else
        {
            src.seekg(0);
        }
    }
    else
    {
        importedBh = new Versal_2vpBootHeader(src, arch);
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
            bootHeader->Copy(importedBh);
        }
        // Read hash block 0
        if (importedBh->GetHashBlockLength() != 0)
        {
            src.seekg(sizeof(Versal_2vpBootHeaderStructure));
            hashBlockLength = importedBh->GetHashBlockLength();
            hashBlock = std::make_unique<uint32_t[]>(hashBlockLength / sizeof(uint32_t));
            src.read((char*)hashBlock.get(), hashBlockLength);
        }

        src.seekg(importedBh->GetImageHeaderByteOffset());
    }

    CheckForIhtAttributes(baseFile);
    imageHeaderTable = std::make_unique<Versal_2vpImageHeaderTable>(src);

    if (imageHeaderTable->iht_optional_data_length != 0)
    {
        src.seekg(importedBh->GetImageHeaderByteOffset() + sizeof(Versal_2vpImageHeaderTableStructure));
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
            Versal_2vpSubSysImageHeader* subsys = new Versal_2vpSubSysImageHeader(src);

            for (uint32_t i = 0; i < subsys->num_of_images; i++)
            {
                ImageHeader* image = new Versal_2vpImageHeader(src, (Versal_2vpImageHeaderStructure*)subsys->section->Data.get(), IsBootloader, i);
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
                        memcpy(options.bifOptions->pmcDataBuffer, newImage->GetPartitionHeaderList().front()->partition->section->Data.get() + importedBh->GetTotalPmcFwLength() + 0x140, options.bifOptions->totalpmcdataSize);
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
                        authOnPartitionFound = true;
                        LOG_INFO("Loading AC context for section %s ", ph->section->Name.c_str());
                        binFile = fopen(it->filename.c_str(), "rb");
                        if (!binFile)
                        {
                            LOG_ERROR("Cannot read file %s", it->filename.c_str());
                        }

                        uint8_t* aC = new uint8_t[sizeof(AuthCertificate4096Sha3PaddingHBStructure)];
                        memset(aC, 0, sizeof(AuthCertificate4096Sha3PaddingHBStructure));

                        if (!(fseek(binFile, ph->GetAuthCertificateOffset(), SEEK_SET)))
                        {
                            size_t result = fread(aC, 1, sizeof(AuthCertificate4096Sha3PaddingHBStructure), binFile);
                            if (result != sizeof(AuthCertificate4096Sha3PaddingHBStructure))
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
                        Versal_2vpAuthenticationContext* auth = new Versal_2vpAuthenticationContext((AuthCertificate4096Sha3PaddingHBStructure_versal2vp*)aC, authtype);

                        // load in previous certificate data
                        AuthenticationCertificate* tempac;
                        tempac = new Versal_2vpAuthenticationCertificate(auth);
                        auth->preSigned = true;
                        tempac->fsbl = true;
                        ph->ac.push_back(tempac);
                        newImage->SetAuthContext(auth);
                    }
                }
            }
            offset += sizeof(Versal_2vpImageHeaderStructure);
            if ((getenv("BOOTGEN_MERGE_IMAGES_WITH_SAME_ID") == NULL))
            {
                if ((prev_image_block != current_image_block) || (this_bootimage == true))
                {
                    subSysImageList.push_back(subsys);
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
                subSysImageList.push_back(subsys);
            }
        }
        else
        {
            src.seekg(offset);
            ImageHeader* image = new Versal_2vpImageHeader(src, IsBootloader);

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
                    uint8_t* aC = new uint8_t[sizeof(AuthCertificate4096Sha3PaddingHBStructure)];
                    memset(aC, 0, sizeof(AuthCertificate4096Sha3PaddingHBStructure));

                    if (!(fseek(binFile, ph->GetAuthCertificateOffset(), SEEK_SET)))
                    {
                        size_t result = fread(aC, 1, sizeof(AuthCertificate4096Sha3PaddingHBStructure), binFile);
                        if (result != sizeof(AuthCertificate4096Sha3PaddingHBStructure))
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
                    Versal_2vpAuthenticationContext* auth = new Versal_2vpAuthenticationContext((AuthCertificate4096Sha3PaddingHBStructure_versal2vp*)aC, authtype);

                    AuthenticationCertificate* tempac;
                    tempac = new Versal_2vpAuthenticationCertificate(auth);
                    auth->preSigned = true;
                    tempac->fsbl = true;
                    ph->ac.push_back(tempac);
                    newImage->SetAuthContext(auth);
                }
            }
            offset += sizeof(Versal_2vpImageHeaderStructure);
        }
        this_bootimage = true;
    } while (imageCount != 0);
}

/******************************************************************************/
void Versal_2vpBootImage::CheckForIhtAttributes(std::string base_file)
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
void Versal_2vpBootImage::ValidateSecureAttributes(ImageHeader * image, BifOptions * bifoptions, PartitionBifOptions * partitionBifOptions)
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
    case Authentication::MLDSA:
    case Authentication::SLH_SHAKE256:
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
ImageHeader* Versal_2vpBootImage::ParsePartitionDataToImage(BifOptions * bifoptions, PartitionBifOptions * partitionBifOptions)
{
    static int aie_elf_cnt = 0;
    static uint8_t slr_boot_cnt = 0;
    static uint8_t slr_cfg_cnt = 0;
    static std::list<SlrPdiInfo*> slrBootPdiInfo;
    static std::list<SlrPdiInfo*> slrConfigPdiInfo;

    ImageHeader *image = new Versal_2vpImageHeader(partitionBifOptions->filename);
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
    else if (partitionBifOptions->partitionType == PartitionType::IMAGE_STORE_PDI)
    {
        ImageStorePdiInfo* imageStorePdi = new ImageStorePdiInfo;
        imageStorePdi->file = partitionBifOptions->filename;
        std::ifstream s(imageStorePdi->file.c_str());
        if (!s)
        {
            LOG_ERROR("Cannot read file - %s ", imageStorePdi->file.c_str());
        }
        imageStorePdi->id = partitionBifOptions->imageStoreId;
        image->SetWriteImageStorePartitions(imageStorePdi);
        imageList.push_back(image);
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
void Versal_2vpBootImage::OutputOptionalSecureDebugImage()
{
    std::string secureDebugImageFile = options.GetSecureDebugImageFile();

    if (options.GetSecureDebugAuthType() == Authentication::None)
    {
        return;
    }

    Authentication::Type origAuthType = options.GetSecureDebugAuthType();
    Authentication::Type authType = origAuthType;
    bool isHss = (authType == Authentication::HSS_SHA2_256 || authType == Authentication::HSS_SHAKE256);
    if (isHss)
    {
        authType = (authType == Authentication::HSS_SHA2_256) ? Authentication::LMS_SHA2_256 : Authentication::LMS_SHAKE256;
    }
    bool isLms = (authType == Authentication::LMS_SHA2_256 || authType == Authentication::LMS_SHAKE256);

    Versal_2vpAuthenticationContext* authCtx = new Versal_2vpAuthenticationContext(this->currentAuthCtx.get(), authType);
    if (isHss)
    {
        authCtx->lmsOnly = false;
    }

    /* For RSA/ECDSA use the boot image (Versal SHA3) hash. For LMS/HSS keep the
       context's own SHA2/SHAKE hash created by the constructor. */
    if (!isLms)
    {
        authCtx->hashType = authHash;
        if (authCtx->ownsHash && authCtx->hash != nullptr)
        {
            delete authCtx->hash;
        }
        authCtx->hash = hash.get();
        authCtx->ownsHash = false;
    }

    uint32_t authJtagImageSize = authCtx->GetAuthJtagImageSize();
    auto writedata = std::make_unique<uint8_t[]>(authJtagImageSize);
    memset(writedata.get(), 0, authJtagImageSize);

    authCtx->CreateAuthJtagImage(options, writedata.get(), bifOptions->authJtagInfo);

    std::ofstream ofs;
    ofs.open(secureDebugImageFile.c_str(), std::ios::binary);

    if (!ofs)
    {
        LOG_ERROR("Cannot write output to file : %s", secureDebugImageFile.c_str());
    }
    else
    {
        ofs.write((const char*)writedata.get(), authJtagImageSize);
        ofs.close();
        LOG_TRACE("Authenticated Jtag Image : '%s' generated.", secureDebugImageFile.c_str());
    }
    /* authCtx is intentionally not deleted here: its primary/secondary keys may
       be shared (non-owning) with currentAuthCtx, and the destructor does not
       honor the ownership flags. Deleting would risk a double-free. */
}

/******************************************************************************/
void Versal_2vpBootImage::ConfigureEncryptionBlocks(ImageHeader * image, PartitionBifOptions * partitionBifOptions)
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
void Versal_2vpBootImage::SetPmcdataFile(const std::string& filename)
{
    options.bifOptions->SetPmcdataFile(filename);
}

/******************************************************************************/
void Versal_2vpBootImage::ConfigureProcessingStages(ImageHeader* image, PartitionBifOptions* partitionbifoptions)
{
    ConfigureEncryptionContext(image, partitionbifoptions->encryptType);

    /* PLM loadable partition support for versal_2vp:
       - Allow authentication for non-bootloader partitions (PLM loadable)
       - ROM handles PLM/PMC authentication using Hash Block 0
       - PLM handles other partition authentication using Hash Block 1
       - Removed restriction: if(!partitionbifoptions->bootloader) partitionbifoptions->authType = Authentication::None;
    */

    ConfigureAuthenticationContext(image, partitionbifoptions->authType, partitionbifoptions);
    
    if (partitionbifoptions->authType == Authentication::None && partitionbifoptions->encryptType == Encryption::None)
        partitionbifoptions->checksumType = Checksum::SHA3;
    
    ConfigureChecksumContext(image, partitionbifoptions->checksumType);
    image->SetPartOwner(partitionbifoptions->ownerType);
}

/******************************************************************************/
void Versal_2vpBootImage::Add(BifOptions* bifoptions)
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
            std::unique_ptr<Versal_2vpAuthenticationContext> authCtx;
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
                    authCtx = std::make_unique<Versal_2vpAuthenticationContext>(currentAuthCtx.get(), Authentication::RSA);
                }
                else if(eckeyLocal != NULL)
                {
                    authCtx = std::make_unique<Versal_2vpAuthenticationContext>(currentAuthCtx.get(), Authentication::ECDSA);
                }
            }

            if (options.DoGenerateHashes())
            {
                authCtx->hash = hash.get();
                authCtx->spkIdentification = bifOptions->GetRevokeId();
                LOG_INFO("Generating SPK Hash File");
                authCtx->GenerateSPKHashFile(bifoptions->GetSPKFileName(), hash.get());
            }

            authCtx.reset();
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
            Versal_2vpSubSysImageHeader *subSysImage = new Versal_2vpSubSysImageHeader(*imgitr);
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
                subSysImageList.push_back(subSysImage);
            }
        }
    }
}

/******************************************************************************/
void Versal_2vpBootImage::ReplaceImages(void)
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
void Versal_2vpBootImage::ReplaceImagesNoSubSys(void)
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
void Versal_2vpBootImage::OutputOptionalEfuseHash()
{
    // First, check if base class can handle this with .kmd metadata (NEW approach)
    // If .kmd metadata exists, base class will handle it and return
    // If no .kmd, base class returns immediately and we handle legacy method here
    
    std::string hashFile = options.GetEfuseHashFileName();
    if (hashFile != "")
    {
        // Check if .kmd metadata is available AND authentication context is initialized
        if (bifOptions->GetPrimaryMetadata().isValid && currentAuthCtx)
        {
            LOG_TRACE("Found .kmd metadata - calling base class to handle new approach");
            // Call base class method to handle .kmd-based approach
            BootImage::OutputOptionalEfuseHash();
            return; // Base class handled it, we're done
        }
        
        // No .kmd metadata or no auth context - this is legacy explicit syntax (ppkfile/pskfile)
        // Handle device-specific initialization for legacy method
        if (!currentAuthCtx)
        {
            LOG_TRACE("No authentication context - skipping PPK hash generation");
            return;
        }
        
        LOG_TRACE("No .kmd metadata - handling legacy method in Versal_2vpBootImage");
    }
    
    // Legacy method continues below...
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

                Versal_2vpAuthenticationContext* authCtx = NULL;
                if (rsa != NULL)
                {
                    authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::RSA);
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
                        authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::ECDSA);
                    }
                    else if (ecCurveNID == NID_secp521r1)
                    {
                        authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::ECDSAp521);
                    }
                    else
                    {
                        LOG_ERROR("Unsupported ECDSA curve read from key file : %s\n           Supported ECDSA curves: P384, P521", primaryKeyFile.c_str());
                    }
                }
                else
                {
                    // Try to detect MLDSA or SLHDSA keys by file size
                    FILE* f = fopen(filename.c_str(), "rb");
                    if (f != NULL)
                    {
                        fseek(f, 0, SEEK_END);
                        long fileSize = ftell(f);
                        fclose(f);

                        if (fileSize == MLDSA_PUB_KEY_LENGTH)
                        {
                            authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::MLDSA);
                        }
                        else if (fileSize == SLHDSA_PUB_KEY_LENGTH)
                        {
                            authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::SLH_SHAKE256);
                        }
                        else
                        {
                            // Default to LMS_SHA2_256 for backward compatibility
                            authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::LMS_SHA2_256);
                        }
                    }
                    else
                    {
                        LOG_ERROR("Cannot read the public key file : %s", filename.c_str());
                    }
                }

                authCtx->hash = hash.get();
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

                Versal_2vpAuthenticationContext* authCtx = NULL;
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
                        authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::ECDSA);
                    }
                    else if (ecCurveNID == NID_secp521r1)
                    {
                        authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::ECDSAp521);
                    }
                    else
                    {
                        LOG_ERROR("Unsupported ECDSA curve read from key file : %s\n           Supported ECDSA curves: P384, P521", primaryKeyFile.c_str());
                    }
                }
                else
                {
                    // Try to detect MLDSA or SLHDSA keys by file size for private keys
                    FILE* f = fopen(primaryKeyFile.c_str(), "rb");
                    if (f != NULL)
                    {
                        fseek(f, 0, SEEK_END);
                        long fileSize = ftell(f);
                        fclose(f);

                        if (fileSize == MLDSA_SEC_KEY_LENGTH)
                        {
                            authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::MLDSA);
                        }
                        else if (fileSize == SLHDSA_PRI_KEY_LENGTH)
                        {
                            authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::SLH_SHAKE256);
                        }
                        else
                        {
                            authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::RSA);
                        }
                    }
                    else
                    {
                        authCtx = new Versal_2vpAuthenticationContext(currentAuthCtx.get(), Authentication::RSA);
                    }
                }
                authCtx->hash = hash.get();
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
void Versal_2vpBootImage::OutputOptionalEfusePufHash()
{
    std::string hashFile = options.GetEfusePufHashFileName();
    if (hashFile != "")
    {
        if (bifOptions->GetPufHelperFile() != "")
        {
            uint8_t* pufDataTemp = new uint8_t[PUF_DATA_LENGTH_4K];
            memset(pufDataTemp, 0, PUF_DATA_LENGTH_4K);

            FileImport fileReader;
            if (!fileReader.LoadHexData(bifOptions->GetPufHelperFile(), pufDataTemp, PUF_DATA_LENGTH_4K - PUF_DATA_LENGTH_4K_ALIGNMENT))
            {
                LOG_ERROR("Invalid no. of data bytes for PUF Helper Data.\n           Expected length for PUF Helper Data is %d bytes", PUF_DATA_LENGTH_4K - PUF_DATA_LENGTH_4K_ALIGNMENT);
            }

            uint8_t* pufHash = new uint8_t[hash->GetHashLength()];
            hash->CalculateHash(true, pufDataTemp, PUF_DATA_LENGTH_4K, pufHash);

            FILE* filePtr;
            if ((filePtr = fopen(hashFile.c_str(), "w")) == NULL)
            {
                LOG_ERROR("-efusepufbits error !!!           Failure writing to hash file %s", StringUtils::BaseName(hashFile).c_str());
            }

            /* For Versal, the efuses are available only for 256 bits.
            So the upper 256 bits(0x20 bytes) of the hash is dumped, which will be programmed to efuse.*/
            for (int index = 0; index < hash->GetHashLength(); index++)
            {
                fprintf(filePtr, "%02X", pufHash[index]);
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
void Versal_2vpBootImage::OutputOptionalPufPDI()
{
    if (options.GetDumpOption() == DumpOption::PUF_PDI)
    {
        FILE* filePtr;
        size_t result;
        std::string fName = options.GetDumpDirectory(); //StringUtils::RemoveExtension(options.GetOutputFileNames().front());
        fName += "puf.bin";
        filePtr = fopen(fName.c_str(), "wb");

        uint8_t* pufPDITemp = new uint8_t[bootHeader->section->Length];
        memcpy(pufPDITemp, bootHeader->section->Data.get(), bootHeader->section->Length);

        Versal_2vpBootHeaderStructure *bHTable = (Versal_2vpBootHeaderStructure*)pufPDITemp;
        bHTable->pufPDIIdentificationWord = PUF_IMAGE_ID_WORD;

        bHTable->headerChecksum = bootHeader->ComputeWordChecksum(&bHTable->widthDetectionWord, bootHeader->GetBootHeaderSize() - sizeof(uint32_t) - sizeof(Versal_2vpSmapWidthTable));

        if (filePtr != NULL)
        {
            result = fwrite(pufPDITemp, 1, bootHeader->section->Length, filePtr);
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
void Versal_2vpBootImage::AppendImagesInSubsystems(void)
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
void Versal_2vpBootImage::BuildAndLink(Binary* cache)
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
            ImageBifOptions *imgOptions = new ImageBifOptions();
            /* If Subsystems are not specified in BIF - create one image header for PLM and other image header for subsytem and add all partitions to it */
            imgOptions->SetImageName("default_subsys");
            SubSysImageHeader* sub_sys_image = new Versal_2vpSubSysImageHeader(imgOptions);
            for (std::list<ImageHeader*>::iterator image = imageList.begin(); image != imageList.end(); image++)
            {
                if ((*image)->IsBootloader())
                {
                    imgOptions->SetImageName("pmc_subsys");
                    SubSysImageHeader* plm_header = new Versal_2vpSubSysImageHeader(imgOptions);
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
        ReplaceImages();
    }
    else
    {
        ReplaceImagesNoSubSys();
    }

    /* Build stage - versal_2vp PDI order:
       BH → IHT → IH → PHT → HB1 → AC(s) for HB1 → HB0 → AC(s) for HB0 → PLM/PMC → PLM loadable partitions
       
       1. Boot Header first
       2. imageHeaderTable creates partition list (needed for later builds)
       3. Meta Header (IHT, IH, PHT, HB1, MH AC)
       4. Bootloader partitions (HB0, AC(s), PLM/PMC) AFTER Meta Header
       5. Non-bootloader partitions (PLM loadable) last
    */
    bootHeader->Build(*this, *cache);
    
    /* imageHeaderTable->Build creates partition list and adds IHT to bi.headers 
       (NOT pushed to cache yet - that happens in partitionHeaderTable->Build) */
    imageHeaderTable->Build(*this, *cache);
    
    /* Build partition header table - pushes headers from bi.headers (IHT, IH, PHT), HB1, MH AC to cache */
    partitionHeaderTable->Build(*this, *cache);
    
    checksumTable->Build(*this, *cache);
    
    /* NOW build bootloader partitions (HB0, AC(s), PLM/PMC) AFTER Meta Header content */
    Versal_2vpPartitionHeaderTable* vphTable = dynamic_cast<Versal_2vpPartitionHeaderTable*>(partitionHeaderTable.get());
    if (vphTable != nullptr)
    {
        vphTable->BuildBootloaderPartitions(*this, *cache);
    }
    
    /* Build non-bootloader (PLM loadable) partitions last */
    if (vphTable != nullptr)
    {
        vphTable->BuildNonBootloaderPartitions(*this, *cache);
    }

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

/******************************************************************************/
uint64_t Versal_2vpBootImage::GetSecureChunkSize(bool isBootloader)
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
