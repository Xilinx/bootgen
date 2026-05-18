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
#include <limits>
#include <iomanip>
#include <iostream>

#include "authentication.h"
#include "authkeys.h"
#include "bootgenexception.h"
#include "stringutils.h"
#include "binary.h"
#include "bootimage.h"
#include "string.h"
#include "options.h"



uint16_t AuthenticationContext::authKeyLength = 0;
// hashLength is now an instance variable, not static - no initialization needed here
bool AuthenticationContext::zynpmpVerEs1 = false;


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
void AuthenticationContext::SetPresignFile(const std::string& filename)
{
    presignFile = filename;
}

/******************************************************************************/
void AuthenticationContext::SetACFile(const std::string& filename)
{
    acFile = filename;
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
void AuthenticationContext::WritePaddedSHAFile(const uint8_t* buffer, size_t bufferLen, const std::string& hashfilename)
{
    std::string filename =  StringUtils::BaseName(hashfilename);
    std::ofstream f(filename.c_str(),std::ios_base::out|std::ios_base::binary);
    f.write((char*)buffer, bufferLen);
    f.close();

    if (f.fail())
    {
        LOG_ERROR("-generate_hashes failure !!!\n           Failed to write generated hash to the file: %s", filename.c_str());
    }
    LOG_TRACE("Hash file %s generated successfully", filename.c_str());
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
void AuthenticationContext::WriteHashFile(const uint8_t* shaBuf, const std::string& hashfilename, bool isHeader)
{
    std::string filename =  StringUtils::BaseName(hashfilename); 
    
    std::size_t size = 0;
    FILE* fp = fopen("allHashes.bin", "rb");
    if (!fp)
    {
        size = 0;
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        fclose(fp);
    }

    uint32_t hashIndex;
    if(!isHeader)
    {
        hashIndex = (size/(sizeof(uint32_t) + hashLength)) + 1;
    }
    else
    {
        hashIndex = 0;
    }
    
    std::ofstream f(filename.c_str(), std::ios_base::app | std::ios_base::binary);

    f.write(reinterpret_cast<const char*>(&hashIndex), sizeof(uint32_t));
    f.write((char*)shaBuf, hashLength);
    f.close();

    if (f.fail())
    {
        LOG_ERROR("Failed to write generated hashes to the file: %s", filename.c_str());
    }
    LOG_TRACE("All hash file %s generated successfully", filename.c_str());
}

/******************************************************************************/
void AuthenticationContext::GetPresign(const std::string& presignFilename, uint8_t* signature, uint32_t index) 
{
    std::string filename(presignFilename);
    std::string baseFile = StringUtils::BaseName(filename);

    if (index != 0)
    {
        size_t x = filename.find(".0.");
        if (x == std::string::npos) 
        {
            LOG_ERROR("Presign file %s does not have partition index (*.0.*)", baseFile.c_str());
        }
        // nudge the '0' to index number
        std::string sindex = std::to_string(index);
        filename.replace(x+1,1,sindex);
    }

    LOG_TRACE("Reading presign file - %s", filename.c_str());
    signature = new uint8_t[signatureLength];
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
void AuthenticationContext::GetAC(const std::string& acFilename, uint8_t* ac, uint32_t index)
{
    std::string filename(acFilename);
    std::string baseFile = StringUtils::BaseName(filename);

    if (index != 0)
    {
        size_t x = filename.find(".0.");
        if (x == std::string::npos)
        {
            LOG_ERROR("AC file %s does not have partition index (*.0.*)", baseFile.c_str());
        }
        // nudge the '0' to index number
        std::string sindex = std::to_string(index);
        filename.replace(x + 1, 1, sindex);
    }

    LOG_TRACE("Reading AC from - %s", filename.c_str());
    FILE* filePtr;
    filePtr = fopen(filename.c_str(), "rb");
    if (filePtr)
    {
        fseek(filePtr, 0, SEEK_END);
        long size = ftell(filePtr);
        fclose(filePtr);
        if (size == GetCertificateSize())
        {
            // read binary
            filePtr = fopen(filename.c_str(), "rb");
            long read_size = fread(ac, 1, GetCertificateSize(), filePtr);
            if (read_size != GetCertificateSize())
            {
                LOG_ERROR("Authentication Error !!!\n           AC file %s should be of %d bytes", baseFile.c_str(), GetCertificateSize());
            }
            fclose(filePtr);
        }
        else
        {
            LOG_ERROR("Authentication Error !!!\n           AC file %s should be of %d bytes", baseFile.c_str(), GetCertificateSize());
        }
    }
    else
    {
        LOG_ERROR("Failure opening AC file - %s", baseFile.c_str());
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
        auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
        auto spkSignatureTemp = std::make_unique<uint8_t[]>(signatureLength);
        if (!secondaryKey->Loaded)
        {
            ParseSPKeyFile(spkFile);
        }
        GenerateSPKHash(shaHashPadded.get());

        RearrangeEndianess(shaHashPadded.get(), signatureLength);
        authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)primaryKey.get(), spkSignatureTemp.get());
        RearrangeEndianess(spkSignatureTemp.get(), signatureLength);
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
        // Smart pointers automatically clean up!
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
        auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
        auto spkSignaturetmp = std::make_unique<uint8_t[]>(signatureLength);
        
        /* Calulate the SPK hash with PKCS padding */
        GenerateSPKHash(shaHashPadded.get());
        
        RearrangeEndianess(shaHashPadded.get(), signatureLength);
                
        /* Sign the SPK hash */
        authAlgorithm->CreateSignature(shaHashPadded.get(), (uint8_t*)primaryKey.get(), spkSignaturetmp.get());
        
        /* If SPK signature file is directly given in BIF file, 
           Sanity Check by cross verifying the calculated SPK signature */
        if (spkSignLoaded)
        {
            RearrangeEndianess(spkSignaturetmp.get(), signatureLength);
            if (memcmp(spksignature.get(), spkSignaturetmp.get(), signatureLength) != 0)
            {
                LOG_ERROR("Authentication Error !!!\n           Loaded SPK Signature does not match calculated SPK Signature");
            }        
        } 
        /* If SPK signature file is not given in BIF file, 
           Copy the calculated SPK signature */                
        else
        {           
            memcpy(spksignature.get(), spkSignaturetmp.get(), signatureLength);
            spkSignLoaded = true;
            RearrangeEndianess(spksignature.get(), signatureLength);
        }
        // Smart pointers automatically clean up!
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
    // Note: hashObj is a non-owning pointer passed from caller
    // Temporarily save our hash and use the provided one
    Hash* savedHash = hash;
    bool savedOwnsHash = ownsHash;
    hash = hashObj;  // Non-owning - we'll restore later
    ownsHash = false;
    
    hashLength = hash->GetHashLength();
    auto shaHashPadded = std::make_unique<uint8_t[]>(signatureLength);
    memset(shaHashPadded.get(), 0, signatureLength);

    /* Calculate the SPK hash */
    GenerateSPKHash(shaHashPadded.get());

    std::string hashfile = filename + hash->GetHashFileExtension();
    WritePaddedSHAFile(shaHashPadded.get(), hashfile);
    
    // Restore original hash
    hash = savedHash;
    ownsHash = savedOwnsHash;
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
    GetPresign(filename, spksignature.get(), 0);
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
    GetPresign(filename, bHsignature.get(), 0);
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

    section = this->AuthContext->CreateCertificate(bi, cache, dataSection, fsbl0);
    fsbl = fsbl0;
    isTableHeader = isTableHeader0;
}

/******************************************************************************/
void AuthenticationCertificate::Link(BootImage& bi, void* partition)
{
    // IMAGE_STORE: Pass partition pointer to AuthContext
    this->AuthContext->Link(bi, partition, this);
}

/******************************************************************************/
void AuthenticationCertificate::Link(BootImage& bi, Section* dataSection)
{
    /* Gather up all the sections that will be used to calculate the authentication hash */
    std::list<Section*> sections;
    std::unique_ptr<Section> headers = nullptr;
    ImageHeaderTable* iHT = bi.imageHeaderTable.get();
    
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
        headers = std::make_unique<Section>("Headers",size);
        headers->Address = iHT->section->Address; // not really needed, but useful for debug.
        memset(headers->Data.get(), bi.options.GetOutputFillByte(), headers->Length);
        
        Binary::Address_t start = sections.front()->Address;
        for(std::list<Section*>::iterator i = sections.begin(); i != sections.end(); i++)
        {
            Section& section(**i);
            int offset = section.Address - start;
            memcpy(headers->Data.get() + offset, section.Data.get(), section.Length);
        }
        /* Replace sections list with the combined new section */
        sections.clear();
        sections.push_back(headers.get());
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
    // headers automatically cleaned up by unique_ptr
}

/******************************************************************************/
Section* RSA2048AuthenticationCertificate::AttachBootHeaderToFsbl(BootImage& bi)
{
    /* If section is a bootloader section, prepend bootheader to this section */
    if (fsbl) 
    {
        BootHeader* bH = bi.bootHeader.get();
		
        size_t bHSecLen = bH->section->Length;
        size_t fsblSecLen = bi.partitionHeaderList.front()->partition->section->Length;
        auto bhfsbl = std::make_unique<Section>("Bhfsbl", (bHSecLen + fsblSecLen));
        bhfsbl->Address = bH->section->Address;
        if (bHSecLen != bH->GetBootHeaderSize() + bH->GetRegInitTableSize())
        {
            LOG_DEBUG(DEBUG_STAMP, "AC size is incorrect during Link - Section Length - %d, BootHeader Size - %d", bHSecLen, bH->GetBootHeaderSize() + bH->GetRegInitTableSize());
            LOG_ERROR("Authentication error !!!");	
        }
        memcpy(bhfsbl->Data.get(), bH->section->Data.get(), bHSecLen);
        memcpy(bhfsbl->Data.get() + bHSecLen, bi.partitionHeaderList.front()->partition->section->Data.get(), fsblSecLen);
        return bhfsbl.release(); // Transfer ownership to caller
    }
    else 
    {
        return NULL;
    }
}

/******************************************************************************/
void AuthenticationAlgorithm::RSA_Exponentiation(const uint8_t *base, const uint8_t* modular, const uint8_t *modular_ext, const uint8_t *exponent, uint8_t *result0)
{
#ifdef _MSC_VER
    /* On Windows/MSVC, direct BIGNUM struct field access (d, dmax, top, etc.)
       causes BN_MONT_CTX_set failure due to struct layout differences between GCC and MSVC.
       Use public OpenSSL API (BN_lebin2bn, BN_mod_exp_mont, BN_bn2lebinpad) instead. */
    uint16_t keyLength = AuthenticationContext::GetRsaKeyLength();

    BIGNUM *a = BN_lebin2bn(base, keyLength, NULL);
    BIGNUM *power = BN_lebin2bn(exponent, keyLength, NULL);
    BIGNUM *m = BN_lebin2bn(modular, keyLength, NULL);
    if (a == NULL || power == NULL || m == NULL)
    {
        BN_free(a);
        BN_free(power);
        BN_free(m);
        LOG_ERROR("Failed to create BIGNUMs for RSA exponentiation");
    }

    BN_CTX_Class ctxInst;
    BN_MONT_CTX_Class montClass(ctxInst);
    montClass.Set(m);

    auto sanityExtension = std::make_unique<uint8_t[]>(keyLength);
    Key::ComputeModulusExtension(modular, sanityExtension.get(), keyLength);
    int comp = memcmp(sanityExtension.get(), modular_ext, keyLength);

    if (comp)
    {
        LOG_ERROR("Internal Error : Montgomery Reduction is not same as externally calculated value.");
    }

    BIGNUM *result = BN_new();
    if (result == NULL)
    {
        BN_free(a);
        BN_free(power);
        BN_free(m);
        LOG_ERROR("Failed to allocate result BIGNUM");
    }

    int ret = BN_mod_exp_mont(result, a, power, m, ctxInst.ctx, montClass.mont);

    if (ret != 1)
    {
        BN_free(result);
        BN_free(a);
        BN_free(power);
        BN_free(m);
        LOG_DEBUG(DEBUG_STAMP, "Error in calculating Modulus Exponent");
        LOG_ERROR("Authentication Error !!!");
    }

    BN_bn2lebinpad(result, result0, keyLength);
    BN_free(result);
    BN_free(a);
    BN_free(power);
    BN_free(m);
#else
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
    auto sanityExtension = std::make_unique<uint8_t[]>(keyLength);
    montClass.GetModulusExtension(sanityExtension.get(), m, keyLength);
    int comp = memcmp(sanityExtension.get(), modular_ext, keyLength);

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
#endif
}

/******************************************************************************/
void RSAAuthenticationAlgorithm::CreateSignature(const uint8_t* base, uint8_t* primaryKey, uint8_t* result0)
{
    RSA_Exponentiation(base, ((Key*)primaryKey)->N.get(), ((Key*)primaryKey)->N_ext.get(), ((Key*)primaryKey)->D.get(), result0);
}

/******************************************************************************/
NoneAuthenticationContext::NoneAuthenticationContext()
{
    authAlgorithm = std::make_unique<NoneAuthenticationAlgorithm>();
}