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
#include <iomanip>
#include <iostream>
#include <limits>
#include "string.h"

#include "authentication-zynq.h"
#include "bootgenexception.h"
#include "stringutils.h"
#include "binary.h"
#include "bootimage.h"
#include "options.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
void ZynqAuthenticationContext::CopyPartitionSignature(BootImage& bi,
                                                       std::list<Section*> sections,
                                                       uint8_t* signatureBlock,
                                                       Section* acSection)
{
    LOG_TRACE("Copying the partition signature into Authentication Certificate");

    uint8_t* shaHash = new uint8_t[hashLength];

    /* Calculate the length of the partiton that is to be hashed,Since bs is authenticated in chunks.
       This can be changed with authblock[default is 8].*/
    std::list<Section*>::iterator section = sections.begin();
    size_t hashSecLen = (*section)->Length;
    size_t authblocksize = authBlocks * 1024 * 1024;
    if (authblocksize != 0)
    {
        hashSecLen = authblocksize;
    }
    if (((*section)->Length) - (authblocksize * acIndex) < authblocksize && (authBlocks != 0))
    {
        hashSecLen = ((*section)->Length) - (authblocksize * acIndex);
    }

    uint8_t *partitionAc = new uint8_t[hashSecLen + (acSection->Length - signatureLength)];

    /* Update with Partition */
    memcpy(partitionAc, (*section)->Data + (authblocksize * acIndex), hashSecLen);

    /* Update with authentication certificate except the last 256 bytes, which is the partition signature, 
       that we are calculating now. Once calculated, the partition signature will sit there */
    memcpy(partitionAc + hashSecLen, acSection->Data, acSection->Length - signatureLength);

    uint32_t start = (*section)->Address;
    uint32_t end = (acSection->Address + acSection->Length) - signatureLength;
    LOG_TRACE("Hashing %s from 0x%x to 0x%x", acSection->Name.c_str(), start, end);
    LOG_DUMP_BYTES((*section)->Data + (authblocksize * acIndex), 32);
    LOG_OUT(" ... ");
    LOG_DUMP_BYTES((*section)->Data + (authblocksize * acIndex) + hashSecLen - hashLength, hashLength);
    LOG_OUT(" ... \n");

    /* Calculate the hash */
    /*  Partition signatures are used by only FSBL / XilSecure / XilFPGA except BL Sign - so partition
        hashes - always NIST except for bootloader
        Bootloader Sign is used by ROM - so Bootloader hash - always Keccak */
    hash->CalculateHash(!(*section)->isBootloader, partitionAc, hashSecLen + (acSection->Length - signatureLength), shaHash);

    LOG_TRACE("Hash of %s (LE):", acSection->Name.c_str());
    LOG_DUMP_BYTES(shaHash, hashLength);

    /* Create the PKCS padding for the hash */
    uint8_t* shaHashPadded = new uint8_t[signatureLength];
    CreatePadding(shaHashPadded, shaHash);

    if (bi.options.DoGenerateHashes())
    {
        std::string hashfilename = acSection->Name;
        WritePaddedSHAFile(shaHashPadded, hashfilename);
    }

    /* Sign the hash */
    authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)secondaryKey, signatureBlock);
    RearrangeEndianess(signatureBlock, signatureLength);
    LOG_TRACE("The partition signature is copied into Authentication Certificate");

    /* Delete the temporarily created arrays */
    delete[] shaHash;
    delete[] shaHashPadded;
    acIndex++;
}

/******************************************************************************/
ZynqAuthenticationContext::ZynqAuthenticationContext()
{
    signatureLength = RSA_SIGN_LENGTH_ZYNQ;
    SetAuthenticationKeyLength(RSA_2048_KEY_LENGTH);
    hashType = AuthHash::Sha2;
    spksignature = new uint8_t[signatureLength];
    spkSignLoaded = false;
    primaryKey = new Key2048("Primary Key");
    secondaryKey = new Key2048("Secondary Key");
    memset(spksignature, 0, signatureLength);
    memset(udf_data, 0, UDF_DATA_SIZE);

    authAlgorithm = new RSAAuthenticationAlgorithm();
    authCertificate = new RSA2048AuthenticationCertificate(this);
    certSize = sizeof(AuthCertificate2048Structure);
}

/******************************************************************************/
ZynqAuthenticationContext::ZynqAuthenticationContext(const AuthenticationContext* refAuthContext)
{
    signatureLength = RSA_SIGN_LENGTH_ZYNQ;
    SetAuthenticationKeyLength(RSA_2048_KEY_LENGTH);
    hashType = AuthHash::Sha2;
    primaryKey = new Key2048("Primary Key");
    secondaryKey = new Key2048("Secondary Key");
    spksignature = new uint8_t[signatureLength];
    spkSignLoaded = refAuthContext->spkSignLoaded;
    primaryKey = refAuthContext->primaryKey;
    secondaryKey = refAuthContext->secondaryKey;
    memcpy(spksignature, refAuthContext->spksignature, signatureLength);
    memcpy(udf_data, refAuthContext->udf_data, sizeof(udf_data));
    authAlgorithm = refAuthContext->authAlgorithm;
    authCertificate = refAuthContext->authCertificate;
    spkSignRequested = refAuthContext->spkSignRequested;
    presignFile = refAuthContext->presignFile;
    udfFile = refAuthContext->udfFile;
    certSize = sizeof(AuthCertificate2048Structure);
}

/******************************************************************************/
ZynqAuthenticationContext::ZynqAuthenticationContext(const AuthCertificate2048Structure* existingCert)
{
    signatureLength = RSA_SIGN_LENGTH_ZYNQ;
    SetAuthenticationKeyLength(RSA_2048_KEY_LENGTH);
    hashType = AuthHash::Sha2;
    spksignature = new uint8_t[signatureLength];
    spkSignLoaded = true;
    primaryKey = new Key2048("Primary Key");
    secondaryKey = new Key2048("Secondary Key");
    primaryKey->Import(&existingCert->acPpk, "Primary Key");
    secondaryKey->Import(&existingCert->acSpk, "Secondary Key");
    memcpy(spksignature, existingCert->acSpkSignature.Signature, signatureLength);
    memcpy(udf_data, existingCert->acUdf, UDF_DATA_SIZE);
    certSize = sizeof(AuthCertificate2048Structure);
    authAlgorithm = new RSAAuthenticationAlgorithm();
}

/******************************************************************************/
ZynqAuthenticationContext::~ZynqAuthenticationContext()
{
    if (spksignature != NULL)
    {
        delete spksignature;
    }

    if (primaryKey != NULL)
    {
        delete primaryKey;
    }

    if (secondaryKey != NULL)
    {
        delete secondaryKey;
    }
}

/******************************************************************************/
void ZynqAuthenticationContext::CreatePadding(uint8_t * signature, const uint8_t * hash)
{
    /* Padding scheme
        MSB-------------------------------------------------------- - LSB
        0x0 || 0x1 || 0xFF(for 202 bytes) || 0x0 || tPad || SHA256 Hash
    */

    uint8_t tPad[T_PAD_LENGTH] = { 0x30, 0x31, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01,
                                   0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 };
    uint8_t* sigPtr;
    uint32_t index;

    /* Signature array is padded in reverse way - Go to end of the array */
    sigPtr = signature + signatureLength;

    *--sigPtr = 0x00;
    *--sigPtr = 0x01;

    /* Fill up 0xFFs for 202 bytes
       202bytes = 256bytes (Total) - 19bytes (tPad) - 32bytes (Hash) - 1byte (0x0 MSB) - 
                          1byte (0x1 next to MSB) - 1byte (0x0 before tPad)
       total FFs = 256 - 19 - 32 - 1 - 1 - 1 = 202 */
    uint8_t totalFfs = signatureLength - sizeof(tPad) - hashLength - 1 - 1 - 1;
    for (index = 0; index < totalFfs; ++index)
    {
        *--sigPtr = 0xFF;
    }

    *--sigPtr = 0x00;
    for (index = 0; index < sizeof(tPad); ++index)
    {
        *--sigPtr = tPad[index];
    }
    for (index = 0; index < 32; ++index)
    {
        *--sigPtr = hash[index];
    }
}

/******************************************************************************/
Section* ZynqAuthenticationContext::CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection)
{
    LOG_INFO("Creating Authentication Certificate for section - %s", dataSection->Name.c_str());
    hash = bi.hash;
    hashLength = hash->GetHashLength();

    /* Partition size must be 64-byte aligned */
    if ((dataSection->Length & 0x3F) != 0)
    {
        LOG_ERROR("Authentication Error !!!\n           Partition %s's length %d bytes - is not 64-byte aligned - %s", dataSection->Name.c_str(), dataSection->Length);
    }
    if (!secondaryKey->Loaded)
    {
        LOG_ERROR("Authentication Error !!!\n           Secondary key must be specified in BIF file for %s", dataSection->Name.c_str());
    }
    if (!secondaryKey->isSecret)
    {
        if (presignFile != "")
        {
            // ok
        }
        else if (bi.options.GetNonBootingFlag() && (dataSection->Name.find("ImageHeaderTable") != std::string::npos))
        {
            // ok
        }
        else if (bi.options.DoGenerateHashes() || bi.options.GetNonBootingFlag())
        {
            static bool warningGiven = false;
            std::list<std::string> outFilename = bi.options.GetOutputFileNames();
            if (outFilename.size() > 0)
            {
                if (!warningGiven)
                {
                    LOG_WARNING("A secondary secret key (SSK) is needed to authenticate a boot image. Because the key provided is not secret, the bootimage will not be usable. However, the sha256 hash files will be generated for offline signing");
                    warningGiven = true;
                }
            }
        }
        else
        {
            LOG_ERROR("Authentication Error !!!\n           Secondary key must be secret (SSK) or partition must have [presign=xxx] attribute in BIF file for section %s", dataSection->Name.c_str());
        }
    }
    if (!(primaryKey->Loaded && primaryKey->isSecret) && !spkSignLoaded)
    {
        LOG_ERROR("Authentication Error !!!\n           Either PSK or SPK signature file must be specified in BIF file.");
    }

    Section* acSection = new Section(dataSection->Name + ".sha256", certSize);
    acSection->isCertificate = true;
    acSection->index = dataSection->index;
    cache.Sections.push_back(acSection);
    AuthCertificate2048Structure* authCert = (AuthCertificate2048Structure*)acSection->Data;
    LOG_TRACE("Creating new section for certificate - %s", acSection->Name.c_str());
    uint32_t x = sizeof(AuthCertificate2048Structure);
    if (x != certSize)
    {
        LOG_DEBUG(DEBUG_STAMP, "Bad Authentication Certificate Size");
        LOG_ERROR("Authentication Error !!!");
    }
    memset(authCert, 0, certSize);
    WriteLittleEndian32(&authCert->acHeader, AUTH_HDR_ZYNQ);
    WriteLittleEndian32(&authCert->acSize, certSize);

    if (udfFile != "")
    {
        LoadUdfData(udfFile, udf_data);
        memcpy(authCert->acUdf, udf_data, UDF_DATA_SIZE);
    }

    primaryKey->Export(&authCert->acPpk);
    secondaryKey->Export(&authCert->acSpk);
    CopySPKSignature(&authCert->acSpkSignature);
    return acSection;
}

/******************************************************************************/
void ZynqAuthenticationContext::Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert)
{
    ::AuthCertificate2048Structure* authCert = (AuthCertificate2048Structure*)cert->section->Data;
    uint8_t* signatureBlock = (uint8_t*)&authCert->acPartitionSignature;

    if (presignFile == "")
    {
        /* If the parition is not presigned / presign file not present */
        CopyPartitionSignature(bi, sections, signatureBlock, cert->section);
    }
    else
    {
        /* If the parition is presigned / presign file present */
        GetPresign(presignFile, signatureBlock, cert->section->index);
    }
}

/******************************************************************************/
void ZynqAuthenticationContext::GenerateSPKHash(uint8_t* shaHashPadded)
{
    LOG_TRACE("Calculating the SPK hash");
    ACKey2048 spkFull;
    secondaryKey->Export(&spkFull);
    uint8_t* shaHash = new uint8_t[hashLength];
    hash->CalculateHash(false, (uint8_t*)&spkFull, sizeof(spkFull), shaHash);
    CreatePadding(shaHashPadded, shaHash);
    delete[] shaHash;
}

/******************************************************************************/
void ZynqAuthenticationContext::CopySPKSignature(ACSignature2048* ptr)
{
    CreateSPKSignature();
    LOG_TRACE("Copying the SPK signature into the Authentication Certificate");
    memcpy(ptr, spksignature, signatureLength);
}

/******************************************************************************/
void ZynqAuthenticationContext::GeneratePPKHash(const std::string& filename)
{
    ACKey2048 ppkTemp;
    primaryKey->Export(&ppkTemp);

    hashLength = hash->GetHashLength();
    uint8_t* rsa_signature = new uint8_t[hashLength];
    hash->CalculateHash(false, (uint8_t*)&ppkTemp, sizeof(ACKey2048), rsa_signature);

    FILE* filePtr;
    if ((filePtr = fopen(filename.c_str(), "w")) == NULL)
    {
        LOG_ERROR("-efuseppkbits error !!!           Failure writing to hash file %s", StringUtils::BaseName(filename).c_str());
    }
    for (int index = 0; index < hashLength; index++)
    {
        fprintf(filePtr, "%02X", rsa_signature[index]);
    }
    fprintf(filePtr, "\r\n");

    fclose(filePtr);
    LOG_INFO("PPK Hash is written to file %s successfully", filename.c_str());
}

/******************************************************************************/
void ZynqAuthenticationContext::SetKeyLength(Authentication::Type type)
{
    if (type == Authentication::RSA)
    {
        AuthenticationContext::authKeyLength = RSA_2048_KEY_LENGTH;
    }
}
