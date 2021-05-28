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
#include <limits>
#include <iomanip>
#include <iostream>

#include "authentication.h"
#include "bootgenexception.h"
#include "stringutils.h"
#include "binary.h"
#include "bootimage.h"
#include "string.h"
#include "options.h"



uint16_t AuthenticationContext::authKeyLength = 0;
uint8_t AuthenticationContext::hashLength = 0;
bool AuthenticationContext::zynpmpVerEs1 = false;


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
void AuthenticationContext::SetPresignFile(const std::string& filename) 
{
    presignFile = filename;
}

/******************************************************************************/
void AuthenticationContext::SetUdfFile(const std::string& filename)
{
    udfFile = filename;
}

/******************************************************************************/
void AuthenticationContext::SetPSKeyFile(const std::string& filename)
{
    pskFile = filename;
}

/******************************************************************************/
void AuthenticationContext::ParsePSKeyFile(const std::string & filename)
{
    primaryKey->ParseSecret(filename);
}

/******************************************************************************/
void AuthenticationContext::SetPPKeyFile(const std::string& filename)
{
    ppkFile = filename;
}

/******************************************************************************/
void AuthenticationContext::ParsePPKeyFile(const std::string& filename)
{
    if (ppkFile != "")
    {
        primaryKey->ParsePublic(filename);
    }
}

/******************************************************************************/
void AuthenticationContext::SetSSKeyFile(const std::string& filename)
{
    sskFile = filename;
}

/******************************************************************************/
void AuthenticationContext::ParseSSKeyFile(const std::string& filename)
{
    if (sskFile != "")
    {
        secondaryKey->ParseSecret(filename);
    }
}

/******************************************************************************/
void AuthenticationContext::SetSPKeyFile(const std::string& filename)
{
    spkFile = filename;
}

/******************************************************************************/
void AuthenticationContext::ParseSPKeyFile(const std::string& filename)
{
    if (spkFile != "")
    {
        secondaryKey->ParsePublic(filename);
    }
}

/******************************************************************************/
void AuthenticationContext::SetPpkSelect(uint32_t ppk)
{
    ppkSelect = ppk;
}

/******************************************************************************/
void AuthenticationContext::SetSpkSelect(uint32_t spk)
{
    spkSelect = spk;
}

/******************************************************************************/
void AuthenticationContext::SetSpkIdentification(uint32_t spkid)
{
    spkIdentification = spkid;
}

/******************************************************************************/
void AuthenticationContext::SetHeaderAuthentication(uint32_t headerauth)
{
    isHeaderAuthentication = headerauth;
}

/******************************************************************************/
void AuthenticationContext::SetAuthenticationKeyLength(uint16_t keylen)
{
    AuthenticationContext::authKeyLength = keylen;
}

/******************************************************************************/
uint16_t AuthenticationContext::GetRsaKeyLength(void)
{
    return AuthenticationContext::authKeyLength;
}

/******************************************************************************/
void AuthenticationContext::SetSignatureLength(uint16_t length)
{
    signatureLength = length;
}

/******************************************************************************/
uint16_t AuthenticationContext::GetSignatureLength(void)
{
    return signatureLength;
}

/******************************************************************************/
size_t AuthenticationContext::GetauthBlocks(size_t authblocks)
{
    return authblocks;
}

/******************************************************************************/
void AuthenticationContext::SetZynpMpVerEs1Flag(bool isEs1)
{
    AuthenticationContext::zynpmpVerEs1 = isEs1;
}

/******************************************************************************/
bool AuthenticationContext::GetZynpMpVerEs1Flag(void)
{
    return AuthenticationContext::zynpmpVerEs1;
}

/******************************************************************************/
void AuthenticationContext::SetHashType(AuthHash::Type type)
{
    hashType = type;
}

/******************************************************************************/
void AuthenticationContext::SetFirstChunkSize(uint64_t size)
{
    firstChunkSize = size;
}

/******************************************************************************/
AuthHash::Type AuthenticationContext::GetHashType(void)
{
    return hashType;
}

/******************************************************************************/
void AuthenticationContext::WritePaddedSHAFile(const uint8_t* shaBuf, const std::string& hashfilename)
{
    std::string filename =  StringUtils::BaseName(hashfilename);
    std::ofstream f(filename.c_str(),std::ios_base::out|std::ios_base::binary);
    f.write((char*)shaBuf, signatureLength);
    f.close();

    if (f.fail())
    {
        LOG_ERROR("-generate_hashes failure !!!\n           Failed to write generated hash to the file: %s", filename.c_str());
    }
    LOG_TRACE("Hash file %s generated successfully", filename.c_str());
}

/******************************************************************************/
void AuthenticationContext::GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index) 
{
    std::string filename(presignFilename);
    std::string baseFile = StringUtils::BaseName(filename);

    if (index > 9) 
    {
        LOG_DEBUG(DEBUG_STAMP, "Partition index count %d too high",index);
        LOG_ERROR("Presign file should have proper index (0-9)");
    }

    if (index != 0)
    {
        size_t x = filename.find(".0.");
        if (x == std::string::npos) 
        {
            LOG_ERROR("Presign file %s does not have partition index (*.0.*)", baseFile.c_str());
        }
        // nudge the '0' to '1' ... '9'.
        filename[x+1] = (char)(filename[x+1] + index); 
    }

    LOG_TRACE("Reading presign file - %s", filename.c_str());
    FILE* filePtr;
    filePtr = fopen(filename.c_str(),"rb");
    if (filePtr) 
    {
        fseek(filePtr, 0, SEEK_END);
        long size = ftell(filePtr);
        fclose(filePtr);
        if (size == signatureLength)
        {
            // read binary
            filePtr = fopen(filename.c_str(),"rb");
            long read_size = fread(signature, 1, signatureLength, filePtr);
            if(read_size != signatureLength)
            {
                LOG_ERROR("Authentication Error !!!\n           Presign file %s should be of %d bytes", baseFile.c_str(), signatureLength);
            }
            fclose(filePtr);
        }
        else 
        {
            // read ascii
            filePtr = fopen(filename.c_str(),"r");
            for(int i=0; i<signatureLength; i++)
            {
                int x;
                if (fscanf(filePtr,"%2X",&x) != 1) 
                {
                    LOG_ERROR("Failure reading presign file - %s", baseFile.c_str());
                }
                signature[i] = x;
            }
            fclose(filePtr);
        }
    } 
    else       
    {
        LOG_ERROR("Failure opening presign file - %s", baseFile.c_str());
    }
}

/******************************************************************************/
void AuthenticationContext::LoadUdfData(const std::string& udfFilename, uint8_t* signature)
{
    std::string filename(udfFilename);
    std::string baseFile = StringUtils::BaseName(udfFilename);
    
    FILE* filePtr;
    int datum, ret, nbytes;
    nbytes = 0;
    filePtr = fopen(filename.c_str(),"r");    
    
    if(filePtr) 
    {
        while((ret = fscanf(filePtr, "%2X", &datum)) != EOF) 
        {
            if (ret == 1) 
            {
                if(datum < 0 || datum > 255) 
                {
                    LOG_ERROR("Bad hex conversion %x - %s", datum, baseFile.c_str());
                }

                if(nbytes == UDF_DATA_SIZE) 
                {
                    LOG_ERROR("More than %d bytes in the UDF Data file %s is not allowed.", UDF_DATA_SIZE, baseFile.c_str());                               
                } 
                signature[nbytes++] = datum;
            } 
            else
            {
                LOG_ERROR("Failure reading UDF file - %s", baseFile.c_str());
            }
        }    
    } 
    else
    {
        LOG_ERROR("Failure opening UDF file - %s", baseFile.c_str());       
    }

    fclose(filePtr);
    LOG_TRACE("UDF data is loaded successfully in the Authentication Certificate from file - %s", udfFilename.c_str());
}

/******************************************************************************/
void AuthenticationContext::ResizeIfNecessary(Section* section)
{
    uint32_t dataSizePadded = (section->Length + 63) & 0xFFFFFFC0; 
    section->IncreaseLengthAndPadTo(dataSizePadded, 0xFF);
}

/******************************************************************************/
void AuthenticationContext::GenerateSPKSignature(const std::string& filename)
{
    if (primaryKey->Loaded && primaryKey->isSecret) 
    {
        uint8_t* shaHashPadded = new uint8_t [signatureLength];
        uint8_t* spkSignatureTemp = new uint8_t [signatureLength];
        if (!secondaryKey->Loaded)
        {
            ParseSPKeyFile(spkFile);
        }
        GenerateSPKHash(shaHashPadded);

        RearrangeEndianess(shaHashPadded, signatureLength);
        authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)primaryKey, spkSignatureTemp);
        RearrangeEndianess(spkSignatureTemp, signatureLength);
        LOG_INFO("SPK Signature generated successfully");
        if (filename != "") 
        {
            FILE* filePtr;
            filePtr = fopen(filename.c_str(),"w");
            if (filePtr) 
            {
                for(uint32_t i=0; i<signatureLength; i++)
                {
                    fprintf(filePtr,"%02X",spkSignatureTemp[i]);
                    if ((i % 32) == 31)
                    {
                        fprintf(filePtr,"\n");
                    }
                }
                fclose(filePtr);
                LOG_INFO("SPK Signature file - %s generated successfully", filename.c_str());
            } 
            else 
            {
                LOG_ERROR("-spksignature error !!!           Failure writing the SPK signature file - %s", StringUtils::BaseName(filename).c_str());
            }
        }
        delete[] spkSignatureTemp;
        delete[] shaHashPadded;
    }
    else
    {
        LOG_ERROR("-spksignature error !!!           PSK missing, cannot sign SPK for generating SPK Signature");
    }
}

/******************************************************************************/
void AuthenticationContext::CreateSPKSignature(void)
{
    LOG_TRACE("Creating the SPK signature");

    /* SPK is signed with PSK (Primary Secret Key) - Check if PSK is given */
    if(primaryKey->Loaded && primaryKey->isSecret) 
    {
        uint8_t* shaHashPadded = new uint8_t [signatureLength];
        uint8_t* spkSignaturetmp = new uint8_t [signatureLength];
        
        /* Calulate the SPK hash with PKCS padding */
        GenerateSPKHash(shaHashPadded);
        
        RearrangeEndianess(shaHashPadded, signatureLength);
                
        /* Sign the SPK hash */
        authAlgorithm->CreateSignature(shaHashPadded, (uint8_t*)primaryKey, spkSignaturetmp);
        
        /* If SPK signature file is directly given in BIF file, 
           Sanity Check by cross verifying the calculated SPK signature */
        if (spkSignLoaded)
        {
            RearrangeEndianess(spkSignaturetmp, signatureLength);
            if (memcmp(spksignature, spkSignaturetmp, signatureLength) != 0)
            {
                LOG_ERROR("Authentication Error !!!\n           Loaded SPK Signature does not match calculated SPK Signature");
            }        
        } 
        /* If SPK signature file is not given in BIF file, 
           Copy the calculated SPK signature */                
        else
        {           
            memcpy(spksignature, spkSignaturetmp, signatureLength);
            spkSignLoaded = true;
            RearrangeEndianess(spksignature, signatureLength);
        }
        delete [] shaHashPadded;
        delete [] spkSignaturetmp;
    } 
    /* If SPK signature file or PSK is not given in BIF file, cannot get SPK signature for the auth certificate
       Throw error */
    else if(!spkSignLoaded) 
    {
        LOG_ERROR("Authentication Error !!!\n          PSK or SPK signature missing, must specify atleast one"); 
    }

    LOG_INFO("SPK signature created successfully");
}

/******************************************************************************/
void AuthenticationContext::GenerateSPKHashFile(const std::string& filename, Hash* hashObj)
{
    hash = hashObj;
    hashLength = hash->GetHashLength();
    uint8_t* shaHashPadded = new uint8_t [signatureLength];
    memset(shaHashPadded, 0, signatureLength);

    /* Calculate the SPK hash */
    GenerateSPKHash(shaHashPadded);

    std::string hashfile = filename + hash->GetHashFileExtension();
    WritePaddedSHAFile(shaHashPadded, hashfile);
    delete [] shaHashPadded;
}


/******************************************************************************/
void AuthenticationContext::SetSPKSignatureFile(const std::string& filename)
{
    spkSignFile = filename;
    if (spkSignFile != "")
    {
        ParseSPKSignatureFile(filename);
    }
}

/******************************************************************************/
void AuthenticationContext::ParseSPKSignatureFile(const std::string& filename)
{
    GetPresign(filename, spksignature, 0);
    spkSignLoaded = true;
    spkSignRequested = "";
}

/******************************************************************************/
void AuthenticationContext::SetBHSignatureFile(const std::string& filename)
{
    bhSignFile = filename;
    if (bhSignFile != "")
    {
        ParseBHSignatureFile(filename);
    }
}

/******************************************************************************/
void AuthenticationContext::ParseBHSignatureFile(const std::string& filename)
{
    GetPresign(filename, bHsignature, 0);
    bhSignLoaded = true;
}


/******************************************************************************/
AuthenticationCertificate::AuthenticationCertificate(AuthenticationContext* context)
    : AuthContext(context)
    , fsbl(false)
    , isTableHeader(false)
{
}

/******************************************************************************/
void AuthenticationCertificate::Build(BootImage& bi, Binary& cache, Section* dataSection, bool fsbl0, bool isTableHeader0)
{
    if (fsbl0 && (this->AuthContext->spkSelect != SpkSelect::SPK_eFUSE)) {
        LOG_ERROR("Bootloader partition can have only 'spk_select=spk-efuse'");
    }

    /* If User - eFuse, SPK_ID mandatory - b/n 0x1 - 0x100 - else ERROR */
    if (this->AuthContext->spkSelect == SpkSelect::USER_eFUSE)
    {
        if ((this->AuthContext->spkIdentification == 0) || (this->AuthContext->spkIdentification > 0x100))
        {
            LOG_ERROR("spk_id can only take values from 0x1 to 0x100, if spk_select=user-efuse");
        }
    }

    section = this->AuthContext->CreateCertificate(bi, cache, dataSection);
    fsbl = fsbl0;
    isTableHeader = isTableHeader0;
}

/******************************************************************************/
void AuthenticationCertificate::Link(BootImage& bi, Section* dataSection)
{
    /* Gather up all the sections that will be used to calculate the authentication hash */
    std::list<Section*> sections;
    Section* headers = NULL;
    ImageHeaderTable* iHT = bi.imageHeaderTable;
    
    if (isTableHeader) 
    {
        /* Header section */
        sections.push_back(iHT->section);
        
        /* Append all image and partition headers */
        for(std::list<ImageHeader*>::iterator img = bi.imageList.begin(); img!= bi.imageList.end(); img++) 
        {
            sections.push_back((*img)->section);
        }
        for(std::list<PartitionHeader*>::iterator part = bi.partitionHeaderList.begin(); part!= bi.partitionHeaderList.end(); part++) 
        {
            sections.push_back((*part)->section);
        }
        sections.push_back(bi.nullPartHeaderSection);
        
        /* Create one new combined section with all the appended sections above */
        size_t size = this->section->Address - iHT->section->Address;
        headers = new Section("Headers",size);
        headers->Address = iHT->section->Address; // not really needed, but useful for debug.
        memset(headers->Data, bi.options.GetOutputFillByte(), headers->Length);
        
        Binary::Address_t start = sections.front()->Address;
        for(SectionList::iterator i = sections.begin(); i != sections.end(); i++)
        {
            Section& section(**i);
            int offset = section.Address - start;
            memcpy(headers->Data + offset, section.Data, section.Length);
        }
        /* Replace sections list with the combined new section */
        sections.clear();
        sections.push_back(headers);
    } 
    else
    {
        /* Partition section */
        Section* bHSec = AttachBootHeaderToFsbl(bi);
        if(bHSec !=NULL)
        {
            sections.push_back(bHSec);
        }
        else
        {
            sections.push_back(dataSection);
        }
    }

    /* Link the certificate - pass for signing */
    this->AuthContext->Link(bi, sections, this);
    if(headers != NULL)
    {
        delete headers;
    }
}

/******************************************************************************/
Section* RSA2048AuthenticationCertificate::AttachBootHeaderToFsbl(BootImage& bi)
{
    /* If section is a bootloader section, prepend bootheader to this section */
    if (fsbl) 
    {
        BootHeader* bH = bi.bootHeader;
        Section* bhfsbl;
        size_t bHSecLen = bH->section->Length;
        size_t fsblSecLen = bi.partitionHeaderList.front()->partition->section->Length;
        bhfsbl = new Section("Bhfsbl", (bHSecLen + fsblSecLen));
        bhfsbl->Address = bH->section->Address;
        if (bHSecLen != bH->GetBootHeaderSize() + bH->GetRegInitTableSize())
        {
            LOG_DEBUG(DEBUG_STAMP, "AC size is incorrect during Link - Section Length - %d, BootHeader Size - %d", bHSecLen, bH->GetBootHeaderSize() + bH->GetRegInitTableSize());
            LOG_ERROR("Authentication error !!!");	
        }
        memcpy(bhfsbl->Data, bH->section->Data, bHSecLen);
        memcpy(bhfsbl->Data + bHSecLen, bi.partitionHeaderList.front()->partition->section->Data, fsblSecLen);
        return bhfsbl;
    }
    else 
    {
        return NULL;
    }
}

/******************************************************************************/
void AuthenticationAlgorithm::RSA_Exponentiation(const uint8_t *base, const uint8_t* modular, const uint8_t *modular_ext, const uint8_t *exponent, uint8_t *result0)
{
    BIGNUM result;
    BIGNUM a; // base
    BIGNUM power; // exponent
    BIGNUM m; // modulus
    BN_CTX_Class ctxInst;
    BN_MONT_CTX_Class montClass(ctxInst);

    uint16_t keyLength = AuthenticationContext::GetRsaKeyLength();

    a.d = (BN_ULONG*)base;
    a.dmax = keyLength / sizeof(BN_ULONG);
    a.top = keyLength / sizeof(BN_ULONG);
    a.flags = 0;
    a.neg = 0;

    power.d = (BN_ULONG*)exponent;
    power.dmax = keyLength / sizeof(BN_ULONG);
    power.top = keyLength / sizeof(BN_ULONG);
    power.flags = 0;
    power.neg = 0;

    m.d = (BN_ULONG*)modular;
    m.dmax = keyLength / sizeof(BN_ULONG);
    m.top = keyLength / sizeof(BN_ULONG);
    m.flags = 0;
    m.neg = 0;

    montClass.Set(m);
    uint8_t* sanityExtension = new uint8_t[keyLength];
    montClass.GetModulusExtension(sanityExtension, m, keyLength);
    int comp = memcmp(sanityExtension, modular_ext, keyLength);
    delete[] sanityExtension;

    if (comp)
    {
        LOG_ERROR("Internal Error : Montgomery Reduction is not same as externally calculated value.");
    }

    result.d = (BN_ULONG*)result0;
    result.dmax = keyLength / sizeof(BN_ULONG);
    result.flags = 0;
    result.neg = 0;
    result.top = 0;

    int ret = BN_mod_exp_mont(&result, &a, &power, &m, ctxInst.ctx, montClass.mont);

    if (ret != 1)
    {
        LOG_DEBUG(DEBUG_STAMP, "Error in calculating Modulus Exponent");
        LOG_ERROR("Authentication Error !!!");
    }
}

/******************************************************************************/
void RSAAuthenticationAlgorithm::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0)
{
    RSA_Exponentiation(base, ((Key*)primaryKey)->N, ((Key*)primaryKey)->N_ext, ((Key*)primaryKey)->D, result0);
}

/******************************************************************************/
NoneAuthenticationContext::NoneAuthenticationContext()
{
    authAlgorithm = new NoneAuthenticationAlgorithm();
}
