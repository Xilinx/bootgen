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
#include "bootimage-zynqmp.h"
#include "bifoptions.h"
#include "encryption-zynqmp.h"
#include "authentication-zynqmp.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
ZynqMpBootImage::ZynqMpBootImage(Options& options, uint8_t index) : BootImage(options, index)
{
    partitionHeaderList.clear();
    bootHeader = new ZynqMpBootHeader();
    imageHeaderTable = new ZynqMpImageHeaderTable();
    partitionHeaderTable = new PartitionHeaderTable();
    currentEncryptCtx = new ZynqMpEncryptionContext(); 
    currentAuthCtx = new ZynqMpAuthenticationContext();
    SetLegacyEncryptionFlag(true);
    partitionOutput = new ZynqMpPartitionOutput();
    hash = new HashSha3();
    currentAuthCtx->hash = hash;
    AuthenticationContext::SetZynpMpVerEs1Flag(options.GetZynqmpes1Flag());
    partitionHeaderTable->firstSection = NULL;
    importedBh = NULL;
}

/******************************************************************************/
void ZynqMpBootImage::ConfigureEncryptionContext(ImageHeader * image, Encryption::Type encryptType)
{
    switch (encryptType)
    {
    case Encryption::AES:
        if (image->GetAesKeyFile() != "")
        {
            std::ifstream keyFile(image->GetAesKeyFile());
            if (!keyFile.good())
            {
                image->SetAesKeyFileGeneration(true);
            }
        }
        image->SetEncryptContext(new ZynqMpEncryptionContext(this->currentEncryptCtx));
        break;

    case Encryption::None:
        image->ValidateEncryptionAttributes(*this);
        image->SetEncryptContext(new NoneEncryptionContext());
        break;

    default:
        image->SetEncryptContext(NULL);
        break;
    }
}

/******************************************************************************/
void ZynqMpBootImage::ConfigureAuthenticationContext(ImageHeader * image, Authentication::Type authType, PartitionBifOptions * partitionbifoptions)
{
    std::string spkSignFile = options.GetSpkSigFileName();
    switch (authType)
    {
    case Authentication::RSA:
    {
        image->SetAuthenticationType(Authentication::RSA);
        options.bifOptions->SetHeaderAC(true);
        this->currentAuthCtx->hashType = GetAuthHashAlgo();
        if (image->GetSpkFile() != "")
        {
            currentAuthCtx->spkFile = image->GetSpkFile();
        }
        if (image->GetSskFile() != "")
        {
            currentAuthCtx->sskFile = image->GetSskFile();
        }
        if (image->GetSpkSignFile() != "")
        {
            currentAuthCtx->spkSignFile = image->GetSpkSignFile();
        }
        if (image->GetBhSignFile() != "")
        {
            currentAuthCtx->bhSignFile = image->GetBhSignFile();
        }
        currentAuthCtx->spkIdentification = image->GetSpkId();
        currentAuthCtx->ppkSelect = image->GetPpkSelect();
        currentAuthCtx->spkSelect = image->GetSpkSelect();
        
        image->SetAuthContext(new ZynqMpAuthenticationContext(this->currentAuthCtx));
        AuthenticationContext* authCtx = image->GetAuthContext();
        authCtx->SetPresignFile(partitionbifoptions->presignFile);
        authCtx->SetUdfFile(partitionbifoptions->udfDataFile);

        if (spkSignFile != "")
        {
            currentAuthCtx->GenerateSPKSignature(spkSignFile);
            currentAuthCtx->spkSignRequested = spkSignFile;
        }

        std::string copyName = image->section->Name;
        StringUtils::ToLower(copyName);
        if ((StringUtils::EndsWith(copyName, ".bit") || StringUtils::EndsWith(copyName, ".rbt")))
        {
            image->SetAuthBlock(partitionbifoptions->authblockattr, options.GetNoAuthBlocksFlag());
        }
    }
    break;

    case Authentication::None:
    default:
    {
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
void ZynqMpBootImage::ConfigureChecksumContext(ImageHeader * image, Checksum::Type checksumType)
{
    switch (checksumType)
    {
    case Checksum::MD5:
        image->SetChecksumContext(new MD5ChecksumContext());
        break;

    case Checksum::SHA2:
        image->SetChecksumContext(new SHA2ChecksumContext());
        break;

    case Checksum::SHA3:
        image->SetChecksumContext(new SHA3ChecksumContext());
        break;

    case Checksum::None:
    default:
        image->SetChecksumContext(new NoneChecksumContext());
        break;
    }
}

/******************************************************************************/
void ZynqMpBootImage::ConfigureProcessingStages(ImageHeader* image, PartitionBifOptions* partitionbifoptions)
{
    ConfigureEncryptionContext(image, partitionbifoptions->encryptType);
    ConfigureAuthenticationContext(image, partitionbifoptions->authType, partitionbifoptions);
    ConfigureChecksumContext(image, partitionbifoptions->checksumType);
    image->SetPartOwner(partitionbifoptions->ownerType);
}

/******************************************************************************/
void ZynqMpBootImage::ParseBootImage(PartitionBifOptions* it)
{
    LOG_INFO("Importing BootImage...");
    std::string baseFile = StringUtils::BaseName(it->filename);

    if (StringUtils::GetExtension(baseFile) == ".mcs")
    {
        LOG_ERROR("Parsing mcs format file is not supported : %s", baseFile.c_str());
    }

    std::ifstream src(it->filename.c_str(), std::ios::binary);

    if (!src.good())
    {
        LOG_ERROR("Failure in reading bootimage file for import - %s ", baseFile.c_str());
    }

    importedBh = new ZynqMpBootHeader(src);
    if (importedBh->GetIdentificationWord() != HEADER_ID_WORD)
    {
        LOG_ERROR("File %s is not a bootimage. It is missing the header signature", baseFile.c_str());
    }
    
    if (importedBh->GetHeaderVersion() != 0xFFFFFFFF)
    {
        LOG_WARNING("This version of bootgen may not support the bootimage header in %s ", baseFile.c_str());
        LOG_WARNING("The version of the imported header is 0x%X, and this version of bootgen only supports 0x%X", importedBh->GetHeaderVersion(), BOOTHEADER_VERSION);
    }
    
    /* Authentication attribues to be appended to the attributes from stage2-Encrypted Image */
    uint32_t fsblattributes = importedBh->GetFsblAttributes();
    if (options.bifOptions->GetBhRsa() != BhRsa::BhRsaDisable)
    {
        fsblattributes |= options.bifOptions->GetBhRsa() << BH_RSA_BIT_SHIFT;
    }
    if (authHash != AuthHash::Sha3)
    {
        fsblattributes |= authHash << AUTH_HASH_BIT_SHIFT;
    }
    importedBh->SetFsblAttributes(fsblattributes);

    /* If imported header has a non-zero source offset, it must be a FSBL header, so it must be copied */
    static bool bHImported = false;
    if (!bHImported)
    {
        LOG_INFO("Copying bootheader from %s ", baseFile.c_str());

        SetAssumeEncryptionFlag(false);

        options.SetEncryptedKeySource(importedBh->GetEncryptionKeySource());

        bootHeader->Copy(importedBh);
        bHImported = true;
    }

    src.seekg(importedBh->GetImageHeaderByteOffset());

    ImageHeaderTable* imageHeaderTable = new ZynqMpImageHeaderTable(src);

    uint32_t offset = imageHeaderTable->GetFirstImageHeaderOffset() * sizeof(uint32_t);
    do
    {
        src.seekg(offset);
        ImageHeader* image = new ZynqMpImageHeader(src);

        if (image->GetPartitionHeaderList().size() > 0)
        {
            /* FsblLoadAddress not available in boot header for ZynqMp. ExecAddress available for both Zynq & ZynqMp */
            bool checkLoadAddrInBhAndPht = (image->GetPartitionHeaderList().front()->GetExecAddress() == importedBh->GetFsblExecAddress());
            bool isItBootloader = (checkLoadAddrInBhAndPht && (importedBh->GetSourceOffset() != 0));

            image->SetBootloader(isItBootloader);

            if (image->IsBootloader())
            {
                LOG_INFO("Found Bootloader in %s at offset 0x%x ", baseFile.c_str(), importedBh->GetSourceOffset());
            }
        }

        image->SetAlignment(it->alignment);
        image->SetOffset(it->offset);
        image->SetReserve(it->reserve, it->updateReserveInPh);
        image->SetLoad(it->load);
        image->SetStartup(it->startup);
        image->SetUserPartitionNum(it->pid);
        image->SetPpkSelect(bifOptions->GetPpkSelection());

        /* Local key files/signatures within partition attributes have more priority than global key/signature files, 
           if both specified */
        image->SetSpkFile(options.bifOptions->GetSPKFileName());
        if (it->spkFile != "")
        {
            image->SetSpkFile(it->spkFile);
        }
        image->SetSskFile(options.bifOptions->GetSSKFileName());
        if (it->sskFile != "")
        {
            image->SetSskFile(it->sskFile);
        }
        image->SetSpkSignFile(options.bifOptions->GetSPKSignFileName());
        if (it->spkSignatureFile != "")
        {
            image->SetSpkSignFile(it->spkSignatureFile);
        }
        image->SetSpkId(bifOptions->GetSpkId());
        if (it->spkIdLocal)
        {
            image->SetSpkId(it->spkId);
        }
        image->SetSpkSelect(bifOptions->GetSpkSelection());
        if (it->spkSelLocal)
        {
            image->SetSpkSelect(it->spkSelect);
        }
        image->SetBhSignFile(options.bifOptions->GetBHSignFileName());

        /* Local AES key file is mandatory when the partition is encrypted */
        char* aeskeyfileENV = getenv("AESKEYFILE");
        if (it->aesKeyFile != "")
        {
            image->SetAesKeyFile(it->aesKeyFile);
        }
        else if ((aeskeyfileENV != NULL) && (options.GetEncryptionKeyFile() != "") && (image->IsBootloader()))
        {
            image->SetAesKeyFile(options.GetEncryptionKeyFile());
        }
        else
        {
            /* If no key file found in partition specific attributes
               Generate aeskeyfile with partition_name.nky */
            std::string name = StringUtils::RemoveExtension(image->GetName()) + ".nky";
            image->SetAesKeyFile(name);
        }

        ConfigureProcessingStages(image, it);
        imageList.push_back(image);

        /* image is just temporary, we need to get a pointer back to the copied object and relink */
        imageList.back()->Relink();
        ImageHeader* newImage = imageList.back();

        /* Determine if we need to load in previous RSAAuthentication information */
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
                image->SetAuthenticationType(Authentication::RSA);
                options.bifOptions->SetHeaderAC(true);
                /* load in previous certificate data */
                for (int i = 0; i < acSize; i++)
                {
                    void *cert = (ph->partition->section->Data + ph->GetCertificateRelativeByteOffset());
                    AuthenticationContext::SetAuthenticationKeyLength(RSA_4096_KEY_LENGTH);
                    AuthenticationContext* auth = new ZynqMpAuthenticationContext((AuthCertificate4096Structure*)cert);
                    AuthenticationCertificate* tempac;
                    tempac = new RSA4096AuthenticationCertificate(auth);
                    auth->preSigned = true;
                    tempac->fsbl = true;
                    ph->ac.push_back(tempac);
                    newImage->SetAuthContext(auth);
                }
            }
        }
        offset = image->GetNextImageHeaderOffset();
    } while (offset != 0);
}

/******************************************************************************/
void ZynqMpBootImage::ValidateSecureAttributes(ImageHeader * image,BifOptions * bifoptions, PartitionBifOptions * partitionBifOptions)
{
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
        if (image->IsBootloader() && bifoptions->GetOptKey() == OptKey::OptKeyinSecHdr)
        {
            LOG_ERROR("Optional key cannot be used, if bootloader is not encrypted\n          Cannot use 'opt_key' in [fsbl_config] attribute when encryption is not used");
        }
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
        if (currentAuthCtx->spkSelect == SpkSelect::USER_eFUSE)
        {
            if ((currentAuthCtx->spkIdentification == 0) || (currentAuthCtx->spkIdentification > 0x100))
            {
                LOG_ERROR("spk_id can only take values from 0x1 to 0x100, if spk_select=user-efuse");
            }
        }

        std::string copyName = image->section->Name;
        StringUtils::ToLower(copyName);
        if ((StringUtils::EndsWith(copyName, ".bit") || StringUtils::EndsWith(copyName, ".rbt")))
        {
        }
        else if (partitionBifOptions->authblockattr != 0)
        {
            LOG_ERROR("'-authblocks' option supported only for bit stream");
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
        if (partitionBifOptions->udfDataFile != "")
        {
            LOG_ERROR("Cannot specify 'udf_data' attribute with Authentication is not enabled");
        }
        if (partitionBifOptions->authblockattr != 0)
        {
            LOG_ERROR("Cannot specify '-authblocks' attribute with Authentication NOT enabled");
        }
        if ((image->IsBootloader()) && (bifoptions->GetAuthOnly() == AuthOnly::Enabled))
        {
            LOG_ERROR("Bootloader must be authenticated if auth_only is used in [fsbl_config] attribute.");
        }
    }
    break;
    }
}

/******************************************************************************/
void ZynqMpBootImage::ParsePartitionDataToImage(BifOptions* bifoptions, PartitionBifOptions* partitionBifOptions)
{
    ImageHeader *image = new ZynqMpImageHeader(partitionBifOptions->filename);
    image->SetBootloader(partitionBifOptions->bootloader);
    image->SetAlignment(partitionBifOptions->alignment);
    image->SetOffset(partitionBifOptions->offset);
    image->SetReserve(partitionBifOptions->reserve, partitionBifOptions->updateReserveInPh);
    image->SetLoad(partitionBifOptions->load);
    image->SetStartup(partitionBifOptions->startup);
    image->SetUserPartitionNum(partitionBifOptions->pid);

    image->SetBootFlag(partitionBifOptions->boot);
    image->SetMultibootFlag(partitionBifOptions->multiboot);
    image->SetNoAutoStartFlag(partitionBifOptions->noautostart);
    image->SetProtectedFlag(partitionBifOptions->Protected);
    image->SetStaticFlag(partitionBifOptions->Static);
    image->SetUserFlag(partitionBifOptions->user);
    image->SetDestDevice(partitionBifOptions->destDeviceType);
    if (image->IsBootloader())
    {
        SetDestCpuFromCore(bifOptions->GetCore(), partitionBifOptions->destCPUType);
    }
    image->SetDestCpu(partitionBifOptions->destCPUType);
    image->SetExceptionLevel(partitionBifOptions->exceptionLevel);
    image->SetTrustZone(partitionBifOptions->trustzone);
    image->SetEarlyHandoff(partitionBifOptions->early_handoff);
    image->SetHivec(partitionBifOptions->hivec);
    image->SetA32ExecMode(partitionBifOptions->a32Mode);
    image->SetBigEndian(partitionBifOptions->bigEndian);
    image->SetBhSignFile(bifoptions->GetBHSignFileName());
    image->SetPpkSelect(bifOptions->GetPpkSelection());

    /* Local key files/signatures within partition attributes have more priority than global key/signature files,
    if both specified */
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
    image->SetSpkId(bifOptions->GetSpkId());
    if (partitionBifOptions->spkIdLocal)
    {
        image->SetSpkId(partitionBifOptions->spkId);
    }
    image->SetSpkSelect(bifOptions->GetSpkSelection());
    if (partitionBifOptions->spkSelLocal)
    {
        image->SetSpkSelect(partitionBifOptions->spkSelect);
    }

    /* Local AES key file is mandatory when the partition is encrypted */
    if (partitionBifOptions->aesKeyFile != "")
    {
        image->SetAesKeyFile(partitionBifOptions->aesKeyFile);
    }
    else
    {
        /* If no key file found in partition specific attributes - Generate aeskeyfile with partition_name.nky */
        std::string name = StringUtils::RemoveExtension(image->GetName()) + ".nky";
        image->SetAesKeyFile(name);
    }

    if (image->IsBootloader())
    {
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

    if (partitionBifOptions->encryptType == Encryption::AES)
    {
        std::vector<uint32_t> encrBlocks = partitionBifOptions->GetEncryptionBlocks();
        for (uint32_t itr = 0; itr < encrBlocks.size(); itr++)
        {
            image->InsertEncrBlocksList(encrBlocks[itr]);
        }
        image->SetDefaultEncrBlockSize(partitionBifOptions->GetDefaultEncryptionBlockSize());
    }
    if ((bifoptions->GetAuthOnly() == AuthOnly::Enabled) && (options.GetEncryptedKeySource() == KeySource::None))
    {
        LOG_ERROR("Key Source is mandatory when 'auth_only' attribute is used.");
    }
    ConfigureProcessingStages(image, partitionBifOptions);

    ValidateSecureAttributes(image,bifoptions, partitionBifOptions);
    imageList.push_back(image);
}

/******************************************************************************/
void ZynqMpBootImage::Add(BifOptions* bifoptions)
{
    //check-R filespec->Dump();
    if (bifoptions->GetAESKeyFileName() != "")
    {
        if (options.GetGreyKeyGeneration() )
        {
            currentEncryptCtx->SetAesFileName(bifoptions->GetAESKeyFileName());
            options.SetEncryptionKeyFile(bifoptions->GetAESKeyFileName());
        }
        else
        {
            LOG_ERROR("This usage of 'aeskeyfile' is not supported for ZYNQMP. \n           Please refer 'bootgen -bif_help aeskeyfile' for more info. \n           Use of the same .nky file across multiple partitions results in re-use of the key, which is not a good security practice.");
        }
    }
    currentEncryptCtx->SetMetalKeyFile(bifoptions->GetFamilyKeyFileName());
    currentAuthCtx->SetPpkSelect(bifoptions->GetPpkSelection());
    currentAuthCtx->SetSpkSelect(bifoptions->GetSpkSelection());
    currentAuthCtx->SetSpkIdentification(bifoptions->GetSpkId());
    currentAuthCtx->SetHeaderAuthentication(bifoptions->GetHeaderAC());
    if (options.GetAuthKeyGeneration() == GenAuthKeys::None)
    {
        if (bifoptions->GetPPKFileName() != "")
        {
            LOG_TRACE("Parsing PPK Key File");
            currentAuthCtx->SetPPKeyFile(bifoptions->GetPPKFileName());
            currentAuthCtx->ParsePPKeyFile(bifoptions->GetPPKFileName());
        }

        if (bifoptions->GetPSKFileName() != "")
        {
            LOG_TRACE("Parsing PSK Key File");
            currentAuthCtx->SetPSKeyFile(bifoptions->GetPSKFileName());
            currentAuthCtx->ParsePSKeyFile(bifoptions->GetPSKFileName());
        }

        if (bifoptions->GetSPKFileName() != "")
        {
            LOG_TRACE("Parsing SPK Key File");
            currentAuthCtx->SetSPKeyFile(bifoptions->GetSPKFileName());
            if (options.DoGenerateHashes())
            {
                LOG_INFO("Generating SPK Hash File");
                currentAuthCtx->GenerateSPKHashFile(bifoptions->GetSPKFileName(), hash);
            }
        }
        if (bifoptions->GetSSKFileName() != "")
        {
            LOG_TRACE("Parsing SSK Key File");
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

    if (options.GetAuthKeyGeneration() != GenAuthKeys::None)
    {
        LOG_WARNING("A bootimage cannot be generated on the go, with '-generate_keys'.\n           However, the requested keys will be generated.");
        return;
    }
    if (options.GetGreyKeyGeneration())
    {
        LOG_WARNING("A bootimage cannot be generated on the go, with '-generate_keys'.\n           However, the requested keys will be generated.\n           Do note to use the same Key0/IV0 pair in AES key file while generating a bootimage.");
        return;
    }

    LOG_INFO("Parsing Partition Data to Image");
    for (std::list<PartitionBifOptions*>::iterator itr = bifoptions->partitionBifOptionList.begin(); itr != bifoptions->partitionBifOptionList.end(); itr++)
    {
        if ((*itr)->bootImage)
        {
            ParseBootImage(*itr);
        }
        else
        {
            ParsePartitionDataToImage(bifOptions,*itr);
        }
    }
}

