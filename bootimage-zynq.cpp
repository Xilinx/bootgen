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
#include "bootimage-zynq.h"
#include "bifoptions.h"
#include "encryption-zynq.h"
#include "authentication-zynq.h"

/* Forward Class Declaration */
class ZynqEncryptionContext;


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
ZynqBootImage::ZynqBootImage(Options& options, uint8_t index) : BootImage(options, index)
{
    options.SetPadHeaderTable(true);
    bootHeader = new ZynqBootHeader();
    imageHeaderTable = new ZynqImageHeaderTable();
    partitionHeaderTable = new PartitionHeaderTable();
    currentEncryptCtx = new ZynqEncryptionContext();
    currentAuthCtx = new ZynqAuthenticationContext();
    SetLegacyEncryptionFlag(true);
    partitionOutput = new ZynqPartitionOutput();
    hash = new HashSha2();
    currentAuthCtx->hash = hash;
    partitionHeaderList.clear();
    partitionHeaderTable->firstSection = NULL;
    importedBh = NULL;
};

/******************************************************************************/
void ZynqBootImage::ConfigureEncryptionContext(ImageHeader * image, Encryption::Type encryptType)
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
        image->SetEncryptContext(new ZynqEncryptionContext(this->currentEncryptCtx));
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
void ZynqBootImage::ConfigureAuthenticationContext(ImageHeader * image, Authentication::Type authType, PartitionBifOptions * partitionbifoptions)
{
    std::string spkSignFile = options.GetSpkSigFileName();
    switch (partitionbifoptions->authType)
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

        image->SetAuthContext(new ZynqAuthenticationContext(this->currentAuthCtx));
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
void ZynqBootImage::ConfigureChecksumContext(ImageHeader * image, Checksum::Type checksumType)
{
    switch (checksumType)
    {
    case Checksum::MD5:
        // For Zynq cases, Checksum is not supported for FSBL
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
void ZynqBootImage::ConfigureProcessingStages(ImageHeader* image, PartitionBifOptions* partitionbifoptions)
{
    ConfigureEncryptionContext(image, partitionbifoptions->encryptType);
    ConfigureAuthenticationContext(image, partitionbifoptions->authType, partitionbifoptions);
    ConfigureChecksumContext(image, partitionbifoptions->checksumType);
    image->SetPartOwner(partitionbifoptions->ownerType);
}

/******************************************************************************/
void ZynqBootImage::DetermineEncryptionDefaults()
{
    if (legacyEncryptionEnabled)
    {
        Encryption::Type defaultType = Encryption::None;
        if (assumeEncryption)
        {
            bool explicitlySet = false;

            /* Legacy behavior for BIF files that do not explicitly set [encryption=aes|none].
               loop through all the images. If the encryption context is set (at least once), 
               then this is the "new" functionality. If it is not set at all, this is legacy functionality. */
            for (std::list<ImageHeader*>::iterator image = imageList.begin(); image != imageList.end(); image++)
            {
                if ((*image)->GetEncryptContext() != NULL)
                {
                    explicitlySet = true;
                    break;
                }
            }

            if (explicitlySet)
            {
                defaultType = Encryption::None;
            }
            else
            {
                bool globalEncryptionRequested = ( options.GetEncryptedKeySource() != KeySource::None ||
                                                   options.GetHmac() != "" ||
                                                   options.GetKey0() != "" ||
                                                   options.GetStartCbc() != "" ||
                                                   options.GetEncryptionKeyFile() != "");
                if (globalEncryptionRequested)
                {
                    defaultType = Encryption::AES;
                }
                else
                {
                    defaultType = Encryption::None;
                }
            }
        }

        /* Apply the default to all images that were not explicitly specified */
        for (std::list<ImageHeader*>::iterator image = imageList.begin(); image != imageList.end(); image++)
        {
            if ((*image)->GetEncryptContext() == NULL)
            {
                switch (defaultType)
                {
                    case Encryption::AES:
                    {
                        ZynqEncryptionContext* aes = new ZynqEncryptionContext();
                        std::string keyFile = options.GetEncryptionKeyFile();
                        if (keyFile != "")
                            aes->SetAesFileName(keyFile);
                        else
                            aes->SetAesFileName(keyFile);
                        (*image)->SetEncryptContext(aes);
                    }
                    break;

                case Encryption::None:
                default:
                    (*image)->SetEncryptContext(new NoneEncryptionContext());
                    break;
                }
            }
            /* Set or verify the bootrom key source/encryption ONLY for the FSBL */
            if ((*image)->IsBootloader())
            {
                if ((*image)->GetEncryptContext()->Type() == Encryption::None)
                {
                    options.SetEncryptedKeySource(KeySource::None);
                }
                else
                {
                    if (options.GetEncryptedKeySource() == KeySource::None)
                    {
                        LOG_ERROR("A key source must be specified in order to have an encrypted partition. Please specify -encrypt efuse|bbram at the command line.");
                    }
                }
            }
        }
    }
}

/******************************************************************************/
void ZynqBootImage::ParseBootImage(PartitionBifOptions* it)
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

    importedBh = new ZynqBootHeader(src);
    if (importedBh->GetIdentificationWord() != HEADER_ID_WORD)
    {
        LOG_ERROR("File %s is not a bootimage. It is missing the header signature", baseFile.c_str());
    }

    if (importedBh->GetHeaderVersion() == BOOTHEADER_VERSION)
    {
        uint32_t calcChecksum = importedBh->CalcHeaderChecksum(importedBh->GetChecksumStartPtr(), 40);
        if (calcChecksum != importedBh->GetHeaderChecksum())
        {
            LOG_ERROR("File %s has a corrupted bootimage header. The checksum is incorrect", baseFile.c_str());
        }
    }

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
    ImageHeaderTable* imageHeaderTable = new ZynqImageHeaderTable(src);
    uint32_t offset = imageHeaderTable->GetFirstImageHeaderOffset() * sizeof(uint32_t);

    do
    {
        src.seekg(offset);
        ImageHeader* image = new ZynqImageHeader(src);

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

        /* Local key files/signatures within partition attributes have more priority than global key/signature files, 
           if both specified */
        image->SetSpkFile(options.bifOptions->GetSPKFileName());
        image->SetSskFile(options.bifOptions->GetSSKFileName());
        image->SetSpkSignFile(options.bifOptions->GetSPKSignFileName());

        // Local AES key file is mandatory when the partition is encrypted
        if (it->aesKeyFile != "")
        {
            LOG_ERROR("This usage of 'aeskeyfile' is not supported for ZYNQ. Please refer 'bootgen -bif_help aeskeyfile' for more info.");
        }
        else if ((options.GetEncryptionKeyFile() != "") && (image->IsBootloader()))
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
                    AuthenticationContext::SetAuthenticationKeyLength(RSA_2048_KEY_LENGTH);
                    AuthenticationContext* auth = new ZynqAuthenticationContext((AuthCertificate2048Structure*)cert);
                    AuthenticationCertificate* tempac;
                    tempac = new RSA2048AuthenticationCertificate(auth);
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
void ZynqBootImage::ValidateSecureAttributes(ImageHeader * image, BifOptions * bifoptions, PartitionBifOptions * partitionBifOptions)
{
    switch (partitionBifOptions->encryptType)
    {
    case Encryption::AES:
        if (XipMode)
        {
            LOG_ERROR("Encryption not supported in XIP Mode");
        }
        break;
    default:
        break;
    }

    switch (partitionBifOptions->authType)
    {
    case Authentication::RSA:
    {
        bifoptions->SetHeaderAC(true);
        if (XipMode)
        {
            LOG_ERROR("Authentication not supported in XIP Mode");
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
    }
    break;
    }

    switch (partitionBifOptions->checksumType)
    {
    case Checksum::MD5:
        if (partitionBifOptions->bootloader)
        {
            LOG_ERROR("Zynq FSBL does not support checksum attribute");
        }
        break;
    default:
        break;
    }
}

/******************************************************************************/
void ZynqBootImage::ParsePartitionDataToImage(BifOptions * bifoptions, PartitionBifOptions * partitionBifOptions)
{
    ImageHeader *image = new ZynqImageHeader(partitionBifOptions->filename);
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

    image->SetSpkFile(bifoptions->GetSPKFileName());
    image->SetSskFile(bifoptions->GetSSKFileName());
    image->SetSpkSignFile(bifoptions->GetSPKSignFileName());

    if (partitionBifOptions->aesKeyFile != "")
    {
        LOG_ERROR("This usage of 'aeskeyfile' is not supported for ZYNQ. Please refer 'bootgen -bif_help aeskeyfile' for more info.");
    }
    if (partitionBifOptions->aesKeyFile != "")
    {
        image->SetAesKeyFile(partitionBifOptions->aesKeyFile);
    }
    else if ((options.GetEncryptionKeyFile() != "") && (image->IsBootloader()))
    {
        image->SetAesKeyFile(options.GetEncryptionKeyFile());
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
    }

    ConfigureProcessingStages(image, partitionBifOptions);
    ValidateSecureAttributes(image, bifoptions, partitionBifOptions);

    imageList.push_back(image);
}

/******************************************************************************/
void ZynqBootImage::Add(BifOptions* bifoptions)
{
   if (bifoptions->GetAESKeyFileName() != "")
    {
        currentEncryptCtx->SetAesFileName(bifoptions->GetAESKeyFileName());
        options.SetEncryptionKeyFile(bifoptions->GetAESKeyFileName());
    }
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
            currentAuthCtx->ParseSPKeyFile(bifoptions->GetSPKFileName());
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
            currentAuthCtx->ParseSSKeyFile(bifoptions->GetSSKFileName());
        }
    }

    if (bifoptions->GetSPKSignFileName() != "")
    {
        LOG_TRACE("Parsing SPK Signature File");
        currentAuthCtx->SetSPKSignatureFile(bifoptions->GetSPKSignFileName());
    }

    XipMode = bifoptions->GetXipMode();

    if (options.GetAuthKeyGeneration() != GenAuthKeys::None)
    {
        LOG_WARNING("A bootimage cannot be generated on the go, with '-generate_keys'.\n           However, the requested keys will be generated.");
        return;
    }

    LOG_INFO("Parsing Partition Data to Image");
    for (std::list<PartitionBifOptions*>::iterator it = bifoptions->partitionBifOptionList.begin(); it != bifoptions->partitionBifOptionList.end(); it++)
    {
        if ((*it)->bootImage)
        {
            ParseBootImage((*it));
        }
        else
        {
            ParsePartitionDataToImage(bifOptions, *it);
        }
    }
}
