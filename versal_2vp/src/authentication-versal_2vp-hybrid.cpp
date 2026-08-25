//it is working file for PLM and rom -hybrid.cpp file 

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
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "authentication-versal_2vp-hybrid.h"
#include "bootimage-versal_2vp.h"
#include "logger.h"
#include "Keccak-compact-versal_2vp.h"
#include "hash.h"
#include <cstdlib>  // For malloc/free to create memory barriers
#include <vector>   // For memory barrier vector

extern "C" {
#include "lms-utils.h"
#include "hss_verify.h"
};

// Include ML-DSA key type for dynamic casting
#include "authkeys-versal_2vp.h"

#include "lms.h"

// Forward declaration for function from authentication-versal_2vp.cpp
extern void PopulateBootloaderHashforBHandMetaheader(BootImage& bi, Section* bootloaderSection, Section* hashBlockSection, ImageHeader* imageHeader, Section* metaSection);

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
/* Helper function to populate PPK/SPK header from .kmd metadata             */
/* Maintains backward compatibility: uses .kmd if available, else defaults   */
/******************************************************************************/
static void PopulateHeaderFromKmdMetadata(
    HeaderAuthCert_versal_2vp* header,
    const KeyMetadata& kmdMetadata,
    uint32_t defaultAlgorithm,
    uint32_t defaultRevocationID,
    uint32_t defaultHybrid,
    uint32_t defaultAuthority,
    uint32_t defaultPermission)
{
    if (kmdMetadata.isValid) {
        // Use .kmd file metadata if available
        header->Algorithm = kmdMetadata.Algorithm;
        header->RevocationID = kmdMetadata.RevocationID;
        header->Hybrid = kmdMetadata.Hybrid;
        header->Authority = kmdMetadata.Authority;
        header->Permission = kmdMetadata.Permission;
        memset(header->Reserved, 0, sizeof(header->Reserved));
        
        LOG_TRACE("Populated header from .kmd metadata:");
        LOG_TRACE("  Algorithm=0x%02X, RevocationID=0x%08X, Hybrid=0x%08X, Authority=0x%08X, Permission=0x%08X",
                 header->Algorithm, header->RevocationID, header->Hybrid, header->Authority, header->Permission);
    } else {
        // Fallback to defaults (backward compatibility - no .kmd file)
        header->Algorithm = defaultAlgorithm;
        header->RevocationID = defaultRevocationID;
        header->Hybrid = defaultHybrid;
        header->Authority = defaultAuthority;
        header->Permission = defaultPermission;
        memset(header->Reserved, 0, sizeof(header->Reserved));
        
        LOG_TRACE("Populated header with defaults (no .kmd file):");
        LOG_TRACE("  Algorithm=0x%02X, RevocationID=0x%08X, Hybrid=0x%08X, Authority=0x%08X, Permission=0x%08X",
                 header->Algorithm, header->RevocationID, header->Hybrid, header->Authority, header->Permission);
    }
}

/******************************************************************************/
/* Helper function to check if algorithm is LMS type (LMS-SHA2-256 or LMS-SHAKE256) */
/* Note: HSS uses the same type (LMS_SHAKE256/LMS_SHA2_256) with lmsOnly=false flag */
/******************************************************************************/
static inline bool IsLmsAlgorithm(Authentication::Type algType)
{
    return (algType == Authentication::LMS_SHA2_256 || algType == Authentication::LMS_SHAKE256);
}

/******************************************************************************/
/* Helper function to check if algorithm is post-quantum (ML-DSA, SLH-DSA, LMS) */
/******************************************************************************/
static inline bool IsPqcAlgorithm(Authentication::Type algType)
{
    return (algType == Authentication::MLDSA || 
            algType == Authentication::SLH_SHAKE256 ||
            algType == Authentication::LMS_SHA2_256 || 
            algType == Authentication::LMS_SHAKE256);
}

/******************************************************************************/
/* Helper function to find PartitionBifOptions for a given section index     */
/* Returns nullptr if not found                                               */
/******************************************************************************/
static PartitionBifOptions* FindPartitionBifOptions(BootImage& bi, int sectionIndex)
{
    // Try image-based structure first
    size_t partIndex = 0;
    for (auto imgIt = bi.bifOptions->imageBifOptionList.begin(); 
         imgIt != bi.bifOptions->imageBifOptionList.end(); ++imgIt) {
        for (auto partIt = (*imgIt)->partitionBifOptionsList.begin(); 
             partIt != (*imgIt)->partitionBifOptionsList.end(); ++partIt, ++partIndex) {
            if (partIndex == static_cast<size_t>(sectionIndex)) {
                return *partIt;
            }
        }
    }
    
    // Fallback to legacy BIF format
    if (static_cast<size_t>(sectionIndex) < bi.bifOptions->partitionBifOptionList.size()) {
        auto partBifIt = bi.bifOptions->partitionBifOptionList.begin();
        std::advance(partBifIt, sectionIndex);
        if (partBifIt != bi.bifOptions->partitionBifOptionList.end()) {
            return *partBifIt;
        }
    }
    
    return nullptr;
}

/******************************************************************************/
/* Helper function to get key file name with partition-level priority        */
/* Checks partition-level first, falls back to global                        */
/******************************************************************************/
static std::string GetKeyFileWithPriority(
    PartitionBifOptions* partBifOpts,
    BifOptions* bifOptions,
    const std::string& partitionKey,
    std::string (BifOptions::*globalGetter)())
{
    if (partBifOpts && !partitionKey.empty()) {
        return partitionKey;
    }
    return (bifOptions->*globalGetter)();
}

/******************************************************************************/
/* Helper function to safely export a key with validation                     */
/* Returns true on success, false if key is not valid/loaded                  */
/******************************************************************************/
static bool SafeKeyExport(Key* key, uint8_t* destination, const char* keyName)
{
    if (!key) {
        LOG_ERROR("%s is null", keyName);
        return false;
    }
    if (!key->Loaded) {
        LOG_ERROR("%s not properly loaded", keyName);
        return false;
    }
    key->Export(destination);
    return true;
}

/******************************************************************************/
/* Helper function to print complete hashblock0 signature in hex format       */
/* Prints as continuous hex string for easy comparison                        */
/******************************************************************************/
static void PrintCompleteHashblock0Signature(uint8_t* signature, size_t sigLength, const char* algorithmName, int algorithmNumber)
{
    if (!signature || sigLength == 0) {
        LOG_TRACE("Algorithm %d (%s) Hashblock0 Signature: <empty or null>", algorithmNumber, algorithmName);
        return;
    }
    
    LOG_TRACE("Algorithm %d (%s) Hashblock0 Signature (%zu bytes):", algorithmNumber, algorithmName, sigLength);
    LOG_DUMP_BYTES(signature, sigLength);
}

/******************************************************************************/
/* Helper function to get signature length for an algorithm type              */
/******************************************************************************/
static uint32_t GetSignatureLengthForAlgorithm(
    Authentication::Type algType,
    const std::string& sskFile,
    bool lmsOnly)
{
    switch (algType) {
        case Authentication::RSA:
            return SIGN_LENGTH_VERSAL;
        case Authentication::ECDSA:
            return EC_P384_KEY_LENGTH * 2;
        case Authentication::ECDSAp521:
            return EC_P521_KEY_LENGTH2 * 2;
        case Authentication::MLDSA:
            return MLDSA_TOTAL_SIGN_LEN;
        case Authentication::SLH_SHAKE256:
            return SLHDSA_TOTAL_SIGN_LENGTH;
        case Authentication::LMS_SHA2_256:
        case Authentication::LMS_SHAKE256:
            // Note: HSS uses same type with lmsOnly=false flag, handled by lmsOnly parameter
            // Signature length must be calculated from key file - no fallback
            if (!sskFile.empty()) {
                size_t actualLen = GetLmsSignLength(sskFile.c_str(), lmsOnly);
                return actualLen + PADDING_16B(actualLen);
            }
            return 0;  // Key file required - will be recalculated when available
        default:
            return SIGN_LENGTH_VERSAL;
    }
}

/******************************************************************************/
/* Helper function to create a key object for a given algorithm type          */
/* Returns nullptr if algorithm type is not supported                          */
/******************************************************************************/
static Key* CreateKeyForAlgorithm(Authentication::Type algType, const char* keyName, bool lmsOnly = true)
{
    Key* key = nullptr;
    switch (algType) {
        case Authentication::RSA:
            key = new Key4096Sha3Padding_versal_2vp(keyName);
            break;
        case Authentication::ECDSA:
            key = new KeyECDSA_versal_2vp(keyName);
            break;
        case Authentication::ECDSAp521:
            key = new KeyECDSAp521_versal_2vp(keyName);
            break;
        case Authentication::MLDSA:
            key = new KeyMLDSA_versal_2vp(keyName);
            break;
        case Authentication::SLH_SHAKE256:
            key = new KeySLH_versal_2vp(keyName);
            break;
        case Authentication::LMS_SHA2_256:
        case Authentication::LMS_SHAKE256:
            key = new KeyLMS_versal_2vp(keyName);
            key->lmsOnly = lmsOnly;
            break;
        default:
            return nullptr;
    }
    if (key) {
        key->authType = algType;
    }
    return key;
}

/******************************************************************************/
/* Helper function to create an authentication algorithm instance              */
/* Returns nullptr if algorithm type is not supported                          */
/******************************************************************************/
static AuthenticationAlgorithm* CreateAuthAlgorithmInstance(Authentication::Type algType)
{
    switch (algType) {
        case Authentication::RSA:
            return new RSA4096Sha3PaddingHBAuthenticationAlgorithm_versal_2vp();
        case Authentication::ECDSA:
            return new ECDSAHBAuthenticationAlgorithm_versal_2vp();
        case Authentication::ECDSAp521:
            return new ECDSAP521HBAuthenticationAlgorithm_versal_2vp();
        case Authentication::LMS_SHA2_256:
        case Authentication::LMS_SHAKE256:
            return new LMSAuthenticationAlgorithm_versal_2vp(algType);
        case Authentication::MLDSA:
            return new MLDSAAuthenticationAlgorithm_versal_2vp(Authentication::MLDSA);
        case Authentication::SLH_SHAKE256:
            return new SLHAuthenticationAlgorithm_versal_2vp(Authentication::SLH_SHAKE256);
        default:
            return nullptr;
    }
}

/******************************************************************************/
Versal_2vpHybridAuthenticationContext::Versal_2vpHybridAuthenticationContext(std::vector<Authentication::Type> algorithms)
    : Versal_2vpAuthenticationContext(algorithms[0]), hybridAlgorithms(algorithms), 
      deferredExecution(false), deferredHashBlockSection(nullptr), deferredSignaturePtr(nullptr), 
      deferredSigLength(0), deferredAlgType(Authentication::None), deferredBi(nullptr),
      isolatedPrimaryKey(nullptr), isolatedSecondaryKey(nullptr),
      lmsOnlyFirstAlg(true), lmsOnlySecondAlg(true)  // Default to LMS (single-level)
{
    LOG_INFO("Creating hybrid authentication context: %s + %s", 
             GetAlgorithmName(algorithms[0]), GetAlgorithmName(algorithms[1]));
}

/******************************************************************************/
Versal_2vpHybridAuthenticationContext::Versal_2vpHybridAuthenticationContext(const AuthenticationContext* refAuthContext, 
                                                                           std::vector<Authentication::Type> algorithms)
    : Versal_2vpAuthenticationContext(refAuthContext, algorithms[0]), hybridAlgorithms(algorithms),
      deferredExecution(false), deferredHashBlockSection(nullptr), deferredSignaturePtr(nullptr), 
      deferredSigLength(0), deferredAlgType(Authentication::None), deferredBi(nullptr),
      isolatedPrimaryKey(nullptr), isolatedSecondaryKey(nullptr),
      lmsOnlyFirstAlg(true), lmsOnlySecondAlg(true)
{
    LOG_TRACE("Creating hybrid authentication context from reference: %s + %s", 
             GetAlgorithmName(algorithms[0]), GetAlgorithmName(algorithms[1]));
}

/******************************************************************************/
Versal_2vpHybridAuthenticationContext::~Versal_2vpHybridAuthenticationContext()
{
    if (primaryKey.get() == isolatedPrimaryKey) primaryKey.release();
    if (secondaryKey.get() == isolatedSecondaryKey) secondaryKey.release();
    isolatedPrimaryKey = nullptr;
    isolatedSecondaryKey = nullptr;
}

/******************************************************************************/
void Versal_2vpHybridAuthenticationContext::Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert)
{
    LOG_INFO("HYBRID SIGNING: Processing %s", sections.front()->Name.c_str());

    // Save the BootImage hash context for first algorithm's hashblock0 signature
    Hash* originalBootImageHash = bi.hash.get();
    AuthHash::Type originalHashType = hashType;
    uint8_t originalHashLength = hashLength;
    
    // Ensure correct keys for first algorithm are loaded
    if (!primaryKey || !secondaryKey || !primaryKey->Loaded || !secondaryKey->Loaded || 
        primaryKey->authType != hybridAlgorithms[0] || secondaryKey->authType != hybridAlgorithms[0]) {
        
        LOG_TRACE("Reloading keys for first algorithm: %s", GetAlgorithmName(hybridAlgorithms[0]));
        
        // Store original keys to avoid double-deletion
        Key* oldPrimaryKey = primaryKey.get();
        Key* oldSecondaryKey = secondaryKey.get();
        primaryKey.reset();
        secondaryKey.reset();
        
        // Create new key objects for first algorithm if needed
        if (!oldPrimaryKey || oldPrimaryKey->authType != hybridAlgorithms[0]) {
            if (oldPrimaryKey && ownsPrimaryKey) delete oldPrimaryKey;
            primaryKey.reset(CreateKeyForAlgorithm(hybridAlgorithms[0], "Primary Key", lmsOnly));
            ownsPrimaryKey = true;
        } else {
            primaryKey.reset(oldPrimaryKey);  // Reuse existing key if it's the right type
            ownsPrimaryKey = false;  // Don't own - we're reusing
        }
        
        if (!oldSecondaryKey || oldSecondaryKey->authType != hybridAlgorithms[0]) {
            if (oldSecondaryKey && ownsSecondaryKey) delete oldSecondaryKey;
            secondaryKey.reset(CreateKeyForAlgorithm(hybridAlgorithms[0], "Secondary Key", lmsOnly));
            ownsSecondaryKey = true;
        } else {
            secondaryKey.reset(oldSecondaryKey);  // Reuse existing key if it's the right type
            ownsSecondaryKey = false;
        }
        
        // Load keys for first algorithm
        try {
            if (!pskFile.empty()) primaryKey->ParseSecret(pskFile);
            if (!sskFile.empty()) secondaryKey->ParseSecret(sskFile);
            // For LMS, skip public key loading to preserve isSecret flag from ParseSecret
            if (!IsLmsAlgorithm(hybridAlgorithms[0])) {
                if (!ppkFile.empty()) LoadPublicKeyForAlgorithm(primaryKey.get(), ppkFile, hybridAlgorithms[0], "PPK_Link");
                if (!spkFile.empty()) LoadPublicKeyForAlgorithm(secondaryKey.get(), spkFile, hybridAlgorithms[0], "SPK_Link");
            }
            } catch (const std::exception& e) {
            LOG_ERROR("Failed to load keys for first algorithm: %s", e.what());
                return;
        }
        
        // Validate keys
        if (!primaryKey || !secondaryKey || !primaryKey->Loaded || !secondaryKey->Loaded) {
            LOG_ERROR("Keys not properly loaded for first algorithm");
            return;
        }
        LOG_TRACE("Keys loaded for first algorithm: %s", GetAlgorithmName(hybridAlgorithms[0]));
    }

    // Handle first certificate (primary algorithm)
    LOG_TRACE("Processing first certificate for algorithm: %s", GetAlgorithmName(hybridAlgorithms[0]));
    
    // Get hashblock section for potential reuse in second algorithm
    Versal_2vpBootImage& vbi = static_cast<Versal_2vpBootImage&>(bi);
    Section* hashBlockSection = vbi.bootloaderHashBlockSection;
    
    // For bootloader, handle hashblock0 signature in first certificate
    if (sections.front()->isBootloader && authAlgorithm->Type() != Authentication::None) {
        // For LMS, calculate total signature size (actual + padding)
        size_t hashblockSignSize;
        if (IsLmsAlgorithm(authAlgorithm->Type())) {
            size_t actualLmsSize = GetLmsSignLength(sskFile.c_str(), lmsOnly);
            hashblockSignSize = actualLmsSize + PADDING_16B(actualLmsSize);
        } else {
            hashblockSignSize = GetTotalHashBlockSignSize();
        }
        
        // Calculate where to write the hashblock0 signature in the first certificate
        size_t certSize = cert->section->Length - hashblockSignSize;
        uint8_t* signaturePtr = cert->section->Data.get() + certSize;
        if (hashBlockSection) {
            // Populate hashblock0 with all hashes
            Section* metaSection = nullptr;
            if (bi.imageHeaderTable && bi.imageHeaderTable->section)
                metaSection = bi.imageHeaderTable->section;

            // Find the bootloader partition header and image header
            PartitionHeader* bootloaderPH = nullptr;
            ImageHeader* bootloaderIH = nullptr;
            for (auto& ph : bi.partitionHeaderList) {
                if (ph->partition->section->isBootloader) {
                    bootloaderPH = ph;
                    bootloaderIH = ph->imageHeader;
                    break;
                }
            }
            
            if (bootloaderPH && bootloaderIH) {
                PopulateBootloaderHashforBHandMetaheader(bi, bootloaderPH->partition->section, hashBlockSection, bootloaderIH, metaSection);
            }

            // Sign hashblock0 with first algorithm
            AuthenticationAlgorithm* savedAlg = authAlgorithm.release();
            uint32_t savedSigLength = signatureLength;
            
            // Restore original BootImage hash context for first algorithm
            hash = originalBootImageHash;
            hashType = originalHashType;
            hashLength = originalHashLength;
            
            // Set up context for first algorithm
            authAlgorithm = GetAuthenticationAlgorithm(hybridAlgorithms[0]);
            signatureLength = GetSignatureLengthForAlgorithm(hybridAlgorithms[0], sskFile, lmsOnly);
            
            // Validate keys
            if (!primaryKey || !secondaryKey || !primaryKey->Loaded || !secondaryKey->Loaded) {
                LOG_ERROR("Keys not properly loaded for hashblock0 signing");
                return;
            }
            
            try {
                if (!authAlgorithm || !hash) {
                    LOG_ERROR("Authentication context not properly initialized");
                    return;
                }
                hashLength = hash->GetHashLength();
                
                // Save total signature length for LMS (CopyPartitionSignature may modify it)
                uint32_t savedTotalSigLength = signatureLength;
                
                std::list<Section*> hashSections;
                hashSections.push_back(hashBlockSection);
                CopyPartitionSignature(bi, hashSections, signaturePtr, cert->section);
                
                // Print complete hashblock0 signature for first algorithm
                PrintCompleteHashblock0Signature(signaturePtr, signatureLength, GetAlgorithmName(hybridAlgorithms[0]), 1);
                
                // Restore LMS signature length if needed
                if (IsLmsAlgorithm(hybridAlgorithms[0])) {
                    signatureLength = savedTotalSigLength;
                }
            } catch (const std::exception& e) {
                LOG_ERROR("Exception during hashblock0 signature generation: %s", e.what());
                return;
            }
            
            // Restore context
            authAlgorithm.reset(savedAlg);
            signatureLength = savedSigLength;
        }
    } else {
        // For non-bootloader partitions, handle normal partition signature
        if (presignFile == "") {
            if (sections.front()->Name == "Headers") {
                /* versal_2vp CORRECTION: Hash Block 1 signature goes ONLY in the AC section,
                   NOT in the Hash Block itself. Hash Block contains only hash entries. */
                
                // Calculate AC signature offset (signature is at end of AC)
                uint32_t totalSigLen1 = GetTotalHashBlockSignSize();
                size_t acSigOffset1 = cert->section->Length - totalSigLen1;
                
                LOG_INFO("=== Generating Hash Block 1 Signature (Alg1: %s) for AC1 ===", GetAlgorithmName(hybridAlgorithms[0]));
                LOG_INFO("  AC1 section: %s, address: 0x%08X, length: %zu", 
                         cert->section->Name.c_str(), cert->section->Address, cert->section->Length);
                LOG_INFO("  AC1 signature dest offset: 0x%zX (absolute PDI addr: 0x%lX)", 
                         acSigOffset1, (unsigned long)(cert->section->Address + acSigOffset1));
                LOG_INFO("  Signature length: %u bytes", signatureLength);
                
                // Generate signature directly into AC section (NOT into Hash Block)
                CopyPartitionSignature(bi, sections, cert->section->Data.get() + acSigOffset1, cert->section);
                
                LOG_INFO("  AC1 signature after generation (first 32 bytes at offset 0x%zX):", acSigOffset1);
                LOG_DUMP_BYTES(cert->section->Data.get() + acSigOffset1, 32);
                LOG_INFO("  AC1 signature generation complete - %u bytes written to AC", signatureLength);
                
                // Print complete HashBlock1 signature for AC1 with LOG_TRACE
                LOG_TRACE("=== HashBlock1 Signature (Hybrid - AC1/Alg1) - Complete Dump ===");
                LOG_TRACE("  Algorithm: %s", GetAlgorithmName(hybridAlgorithms[0]));
                LOG_TRACE("  AC1 Section: %s", cert->section->Name.c_str());
                LOG_TRACE("  AC1 Address: 0x%08X", cert->section->Address);
                LOG_TRACE("  Signature Offset in AC1: 0x%zX", acSigOffset1);
                LOG_TRACE("  Absolute PDI Address: 0x%08lX", (unsigned long)(cert->section->Address + acSigOffset1));
                LOG_TRACE("  Signature Length: %u bytes (with padding: %u bytes)", signatureLength, totalSigLen1);
                LOG_TRACE("  HashBlock1 Signature Data AC1 (complete %u bytes):", totalSigLen1);
                LOG_DUMP_BYTES(cert->section->Data.get() + acSigOffset1, totalSigLen1);
                LOG_TRACE("=== End HashBlock1 Signature (Hybrid - AC1) ===");
            } else {
                // For versal_2vp devices, do NOT copy signature into PLM section after hash block
                if (!(bi.options.GetArchType() == Arch::VERSAL_2VP && sections.front()->isBootloader)) {
                    CopyPartitionSignature(bi, sections, sections.front()->Data.get() + bi.hashBlockLength, cert->section);
                }
            }
        }
    }

    // Handle second certificate (secondary algorithm)
    if (hybridAlgorithms.size() >= 2) {
        // Find second certificate section
        // Note: "Headers" section during Link corresponds to "MetaHeader" section during Build
        std::string baseName = sections.front()->Name;
        if (baseName == "Headers") {
            baseName = "MetaHeader";
        }
        std::string cert2Name = baseName + ".auth2" + bi.hash->GetHashFileExtension();
        Section* cert2Section = nullptr;
        for (auto& section : bi.cache->Sections) {
            if (section->Name == cert2Name) {
                cert2Section = section.get();
                break;
            }
        }

        if (cert2Section) {
            LOG_INFO("Found cert2Section: Name=%s, Length=%zu, Data=%p", 
                     cert2Section->Name.c_str(), cert2Section->Length, cert2Section->Data.get());
            
            // Get partition BIF options for second algorithm
            PartitionBifOptions* partBifOpts = FindPartitionBifOptions(bi, currentDataSection->index);
            bool lmsOnlyForSize = partBifOpts ? partBifOpts->lmsOnly : true;
            
            // Calculate signature length for second algorithm
            uint16_t tempSigLength = 0;  // Initialize to 0, will be calculated from key file
            if (IsLmsAlgorithm(hybridAlgorithms[1])) {
                std::string tempSsk1 = (partBifOpts && !partBifOpts->sskFile1.empty()) 
                                        ? partBifOpts->sskFile1 : bi.bifOptions->GetSSKFileName1();
                if (!tempSsk1.empty()) {
                    size_t lmsSignLen = GetLmsSignLength(tempSsk1.c_str(), lmsOnlyForSize);
                    tempSigLength = (uint16_t)(lmsSignLen + PADDING_16B(lmsSignLen));
                }
                // If key file empty, tempSigLength stays 0 - will be calculated when key is loaded
            } else {
                tempSigLength = GetSignatureLengthForAlgorithm(hybridAlgorithms[1], "", lmsOnlyForSize);
            }

            // Calculate signature pointer based on algorithm type
            uint8_t* sigPtr2 = nullptr;
            if (IsLmsAlgorithm(hybridAlgorithms[1])) {
                // For LMS, calculate offset based on certificate structure (reuse partBifOpts)
                std::string tempPpk1 = (partBifOpts && !partBifOpts->ppkFile1.empty()) 
                                        ? partBifOpts->ppkFile1 : bi.bifOptions->GetPPKFileName1();
                std::string tempSpk1 = (partBifOpts && !partBifOpts->spkFile1.empty()) 
                                        ? partBifOpts->spkFile1 : bi.bifOptions->GetSPKFileName1();
                std::string tempPsk1 = (partBifOpts && !partBifOpts->pskFile1.empty()) 
                                        ? partBifOpts->pskFile1 : bi.bifOptions->GetPSKFileName1();
                
                // Calculate key sizes
                size_t ppkKeySize = !tempPpk1.empty() ? GetLmsPublicKeyLength(tempPpk1.c_str(), lmsOnlyForSize) : 60;
                size_t spkKeySize = !tempSpk1.empty() ? GetLmsPublicKeyLength(tempSpk1.c_str(), lmsOnlyForSize) : 60;
                size_t spkSigSize = !tempPsk1.empty() ? GetLmsSignLength(tempPsk1.c_str(), lmsOnlyForSize) : 2644;
                
                // Calculate offset to hashblock signature
                size_t offset = sizeof(HeaderAuthCert_versal_2vp)   // PPK header
                              + ppkKeySize + PADDING_16B(ppkKeySize)  // PPK key + alignment
                              + sizeof(HeaderAuthCert_versal_2vp)   // SPK header
                              + spkKeySize + PADDING_16B(spkKeySize)  // SPK key + alignment
                              + sizeof(uint32_t) + 12               // SPK sig length field + alignment
                              + spkSigSize + PADDING_16B(spkSigSize) // SPK signature + alignment
                              + sizeof(uint32_t) + 12;              // HashBlock sig length field + alignment
                
                sigPtr2 = cert2Section->Data.get() + offset;
                LOG_TRACE("LMS cert2 hashblock signature offset: %zu", offset);
            } else {
                // For other algorithms, use simple calculation (signature at end)
                size_t certSize2 = cert2Section->Length - tempSigLength;
                sigPtr2 = cert2Section->Data.get() + certSize2;
                LOG_INFO("Non-LMS cert2: Length=%zu, sigLength=%u, hashblock sig offset=%zu, sigPtr2=%p",
                         cert2Section->Length, tempSigLength, certSize2, sigPtr2);
            }

            // Generate signature for second certificate
            if (sections.front()->isBootloader && hashBlockSection) {
                // Store parameters for deferred execution (after first algorithm completes)
                deferredHashBlockSection = hashBlockSection;
                deferredSignaturePtr = sigPtr2;
                deferredSigLength = tempSigLength;
                deferredAlgType = hybridAlgorithms[1];
                deferredBi = &bi;
                deferredExecution = true;
                LOG_TRACE("Deferred second algorithm execution stored");
            } else if (sections.front()->Name == "Headers") {
                /* versal_2vp CORRECTION: Hash Block 1 signature goes ONLY in the AC2 section,
                   NOT in the Hash Block itself. Hash Block contains only hash entries. */
                
                // Calculate AC2 signature offset (signature is at end of AC)
                size_t acSigOffset2 = cert2Section->Length - tempSigLength;
                
                LOG_INFO("=== Generating Hash Block 1 Signature (Alg2: %s) for AC2 ===", GetAlgorithmName(hybridAlgorithms[1]));
                LOG_INFO("  AC2 section: %s, address: 0x%08X, length: %zu", 
                         cert2Section->Name.c_str(), cert2Section->Address, cert2Section->Length);
                LOG_INFO("  AC2 signature dest offset: 0x%zX (absolute PDI addr: 0x%lX)", 
                         acSigOffset2, (unsigned long)(cert2Section->Address + acSigOffset2));
                LOG_INFO("  Signature length: %u bytes", tempSigLength);
                
                // Generate signature directly into AC2 section (NOT into Hash Block)
                GenerateSecondSignature(bi, sections, cert2Section->Data.get() + acSigOffset2, tempSigLength, hybridAlgorithms[1]);
                
                LOG_INFO("  AC2 signature after generation (first 32 bytes at offset 0x%zX):", acSigOffset2);
                LOG_DUMP_BYTES(cert2Section->Data.get() + acSigOffset2, 32);
                LOG_INFO("  AC2 signature generation complete - %u bytes written to AC", tempSigLength);
                
                // Print complete HashBlock1 signature for AC2 with LOG_TRACE
                LOG_TRACE("=== HashBlock1 Signature (Hybrid - AC2/Alg2) - Complete Dump ===");
                LOG_TRACE("  Algorithm: %s", GetAlgorithmName(hybridAlgorithms[1]));
                LOG_TRACE("  AC2 Section: %s", cert2Section->Name.c_str());
                LOG_TRACE("  AC2 Address: 0x%08X", cert2Section->Address);
                LOG_TRACE("  Signature Offset in AC2: 0x%zX", acSigOffset2);
                LOG_TRACE("  Absolute PDI Address: 0x%08lX", (unsigned long)(cert2Section->Address + acSigOffset2));
                LOG_TRACE("  Signature Length: %u bytes", tempSigLength);
                LOG_TRACE("  HashBlock1 Signature Data AC2 (complete %u bytes):", tempSigLength);
                LOG_DUMP_BYTES(cert2Section->Data.get() + acSigOffset2, tempSigLength);
                LOG_TRACE("=== End HashBlock1 Signature (Hybrid - AC2) ===");
                
                // Print Hash Block 1 summary (hash entries only, NO signatures)
                LOG_INFO("=== Hash Block 1 Summary (Meta Header - Hybrid) ===");
                LOG_INFO("  Hash Data ONLY: Offset=0x0, Length=%u bytes", bi.imageHeaderTable->hashBlockSectionLength);
                LOG_INFO("  Total HashBlock Size: %zu bytes (hash entries only)", bi.imageHeaderTable->hashBlockSection->Length);
                LOG_INFO("  Signatures are in AC1 and AC2, NOT in Hash Block");
            } else {
                // Non-bootloader partitions use regular partition signature
                GenerateSecondSignature(bi, sections, sigPtr2, tempSigLength, hybridAlgorithms[1]);
            }

            LOG_TRACE("Hybrid signing complete for %s", sections.front()->Name.c_str());
        } else {
            LOG_ERROR("Second certificate section not found: %s", cert2Name.c_str());
        }
    }

    // Final logging for bootloader
    if (sections.front()->isBootloader) {
        LOG_TRACE("Bootloader hybrid certificates completed");
    }

    // Execute deferred second algorithm after first algorithm completes
    if (deferredExecution) {
        LOG_INFO("Executing deferred second algorithm: deferredSignaturePtr=%p, deferredSigLength=%u, algType=%s",
                 deferredSignaturePtr, deferredSigLength, GetAlgorithmName(deferredAlgType));
        GenerateHashblock0SignatureForSecondAlgorithm(*deferredBi, deferredHashBlockSection, deferredSignaturePtr, deferredSigLength, deferredAlgType);
        
        // Print complete hashblock0 signature for second algorithm
        PrintCompleteHashblock0Signature(deferredSignaturePtr, deferredSigLength, GetAlgorithmName(deferredAlgType), 2);
        
        deferredExecution = false;
    }
}

/******************************************************************************/
void Versal_2vpHybridAuthenticationContext::GenerateSecondSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, uint16_t sigLength, Authentication::Type algType)
{
    // Save current authentication context state
    AuthenticationAlgorithm* originalAlg = authAlgorithm.release();
    uint32_t originalSigLength = signatureLength;
    Key* originalPrimary = primaryKey.release();
    Key* originalSecondary = secondaryKey.release();
    std::string originalSskFile = sskFile;
    std::string originalPskFile = pskFile;
    std::string originalSpkFile = spkFile;
    std::string originalPpkFile = ppkFile;
    Hash* originalHash = hash;
    uint32_t originalHashLength = hashLength;
    AuthHash::Type originalHashType = hashType;
    
    // Temporarily switch to second algorithm context
    authAlgorithm = GetAuthenticationAlgorithm(algType);
    signatureLength = sigLength;
    
    // Set correct hash for the second algorithm
    switch (algType) {
        case Authentication::ECDSA:
        case Authentication::RSA:
        case Authentication::ECDSAp521:
            hash = new HashSha3();
            hashLength = hash->GetHashLength();
            hashType = AuthHash::Sha3;
            break;
        case Authentication::LMS_SHAKE256:
        case Authentication::HSS_SHAKE256:
        case Authentication::SLH_SHAKE256:
            hash = new HashShake256();
            hashLength = hash->GetHashLength();
            hashType = AuthHash::Shake256;
            break;
        case Authentication::LMS_SHA2_256:
            hash = new HashSha2();
            hashLength = hash->GetHashLength();
            hashType = AuthHash::Sha2;
            break;
        case Authentication::MLDSA:
            hash = new HashSha2_512();
            hashLength = hash->GetHashLength();
            hashType = AuthHash::Sha2_512;
            break;
        default:
            // Keep original hash for unknown algorithms
            break;
    }
    LOG_TRACE("GenerateSecondSignature: Switched hash for algorithm %d - hashLength=%d", algType, hashLength);
    
    // Create temporary key objects for the second algorithm if needed
    Key* tempPrimaryKey = nullptr;
    Key* tempSecondaryKey = nullptr;
    
    // Get partition BIF options (needed for lmsOnly flag)
    Section* currentDataSection = GetCurrentDataSection();
    PartitionBifOptions* genPartitionBifOptions = currentDataSection 
        ? FindPartitionBifOptions(bi, currentDataSection->index) : nullptr;
    bool lmsOnlyForSig = genPartitionBifOptions ? genPartitionBifOptions->lmsOnly : lmsOnly;
    
    try {
        if (algType != hybridAlgorithms[0]) {
            // Create key instances using helper function
            tempPrimaryKey = CreateKeyForAlgorithm(algType, "Primary Key", lmsOnlyForSig);
            tempSecondaryKey = CreateKeyForAlgorithm(algType, "Secondary Key", lmsOnlyForSig);
            if (!tempPrimaryKey || !tempSecondaryKey) {
                LOG_ERROR("Unsupported algorithm type for second signature: %d", algType);
                return;
            }
            
            // Get key files with partition-level priority
            std::string psk1 = (genPartitionBifOptions && !genPartitionBifOptions->pskFile1.empty()) 
                ? genPartitionBifOptions->pskFile1 : bi.bifOptions->GetPSKFileName1();
            std::string ssk1 = (genPartitionBifOptions && !genPartitionBifOptions->sskFile1.empty()) 
                ? genPartitionBifOptions->sskFile1 : bi.bifOptions->GetSSKFileName1();
            std::string ppk1 = (genPartitionBifOptions && !genPartitionBifOptions->ppkFile1.empty()) 
                ? genPartitionBifOptions->ppkFile1 : bi.bifOptions->GetPPKFileName1();
            std::string spk1 = (genPartitionBifOptions && !genPartitionBifOptions->spkFile1.empty()) 
                ? genPartitionBifOptions->spkFile1 : bi.bifOptions->GetSPKFileName1();
            
            // Load keys - MUST cast to Versal_2vpKey* for PQC algorithms since
            // Versal_2vpKey has mldsa_*/slhdsa_* members that shadow the base Key class
            LOG_TRACE("GenerateSecondSignature: Loading keys - psk1='%s', ssk1='%s', algType=%d", 
                     psk1.c_str(), ssk1.c_str(), algType);
            
            // Cast to Versal_2vpKey* for all operations (safe since CreateKeyForAlgorithm creates Versal_2vp keys)
            Versal_2vpKey* vpPrimaryKey = static_cast<Versal_2vpKey*>(tempPrimaryKey);
            Versal_2vpKey* vpSecondaryKey = static_cast<Versal_2vpKey*>(tempSecondaryKey);
            
            // Parse secret keys
            if (!psk1.empty()) {
                vpPrimaryKey->ParseSecret(psk1);
            }
            if (!ssk1.empty()) {
                vpSecondaryKey->ParseSecret(ssk1);
            }
            
            // Handle public key loading/extraction based on algorithm type
            if (!IsPqcAlgorithm(algType)) {
                // Classical algorithms (RSA, ECDSA): load public keys separately
                if (!ppk1.empty()) LoadPublicKeyForAlgorithm(tempPrimaryKey, ppk1, algType, "PPK1_Gen");
                if (!spk1.empty()) LoadPublicKeyForAlgorithm(tempSecondaryKey, spk1, algType, "SPK1_Gen");
            } else if (algType == Authentication::MLDSA) {
                // ML-DSA: public key CANNOT be extracted from private key!
                // ML-DSA secret key format: (ρ || K || tr || s_1 || s_2 || t_0) - 4896 bytes
                // ML-DSA public key format: (ρ || t_1) - 2592 bytes
                // Only ρ (32 bytes) is shared, NOT the full public key!
                // Must load public key from separate file (ppk1/spk1)
                LOG_TRACE("GenerateSecondSignature: ML-DSA key state after ParseSecret:");
                LOG_TRACE("  Primary: private=%p, public=%p", vpPrimaryKey->mldsa_private_key, vpPrimaryKey->mldsa_public_key);
                LOG_TRACE("  Secondary: private=%p, public=%p", vpSecondaryKey->mldsa_private_key, vpSecondaryKey->mldsa_public_key);
                
                // Load public keys from separate files since they cannot be extracted from private keys
                if (vpPrimaryKey->mldsa_public_key == nullptr && !ppk1.empty()) {
                    vpPrimaryKey->isSecret = false;
                    vpPrimaryKey->authType = Authentication::MLDSA;
                    vpPrimaryKey->ParseMldsaKey(ppk1);
                    LOG_TRACE("  Loaded ML-DSA primary public key from %s (%d bytes)", ppk1.c_str(), MLDSA_PUB_KEY_LENGTH);
                }
                if (vpSecondaryKey->mldsa_public_key == nullptr && !spk1.empty()) {
                    vpSecondaryKey->isSecret = false;
                    vpSecondaryKey->authType = Authentication::MLDSA;
                    vpSecondaryKey->ParseMldsaKey(spk1);
                    LOG_TRACE("  Loaded ML-DSA secondary public key from %s (%d bytes)", spk1.c_str(), MLDSA_PUB_KEY_LENGTH);
                }
                
                LOG_TRACE("GenerateSecondSignature: ML-DSA final key state:");
                LOG_TRACE("  Primary: private=%p, public=%p", vpPrimaryKey->mldsa_private_key, vpPrimaryKey->mldsa_public_key);
                LOG_TRACE("  Secondary: private=%p, public=%p", vpSecondaryKey->mldsa_private_key, vpSecondaryKey->mldsa_public_key);
            } else if (algType == Authentication::SLH_SHAKE256) {
                // SLH-DSA: ParseSLHdsaKey already extracts public key from private key
                // (public key is embedded at the end of private key and extracted during Parse)
                LOG_TRACE("GenerateSecondSignature: SLH-DSA key state after ParseSecret:");
                LOG_TRACE("  Primary: private=%p, public=%p", vpPrimaryKey->slhdsa_private_key, vpPrimaryKey->slhdsa_public_key);
                LOG_TRACE("  Secondary: private=%p, public=%p", vpSecondaryKey->slhdsa_private_key, vpSecondaryKey->slhdsa_public_key);
                // Note: SLH-DSA public key extraction is done in ParseSLHdsaKey, no additional action needed
            }
            // LMS/HSS: public keys handled separately through key files, no extraction needed
            
            primaryKey.reset(vpPrimaryKey);
            secondaryKey.reset(vpSecondaryKey);
            
            // Also update key file paths used by CopyPartitionSignature
            pskFile = psk1;
            sskFile = ssk1;
            ppkFile = ppk1;
            spkFile = spk1;
        }
        
        // Create a dummy certificate and set signature length
        Section dummyCert("dummy_cert", sigLength);
        
        // For the second algorithm, use the correct key file (ssk1) to calculate signature length
        std::string ssk1ForSize = (genPartitionBifOptions && !genPartitionBifOptions->sskFile1.empty()) 
            ? genPartitionBifOptions->sskFile1 : bi.bifOptions->GetSSKFileName1();
        
        // Calculate signature length for the second algorithm
        if (IsLmsAlgorithm(algType)) {
            // For LMS/HSS, calculate from the second algorithm's key file
            if (!ssk1ForSize.empty()) {
                size_t actualLmsLen = GetLmsSignLength(ssk1ForSize.c_str(), lmsOnlyForSig);
                signatureLength = actualLmsLen + PADDING_16B(actualLmsLen);
                LOG_TRACE("GenerateSecondSignature: LMS signature length from %s: %d", ssk1ForSize.c_str(), signatureLength);
            } else {
                LOG_ERROR("Cannot calculate LMS signature length - ssk1 file not provided");
                signatureLength = sigLength; // Use caller-provided length as fallback
            }
        } else {
            signatureLength = GetSignatureLengthForAlgorithm(algType, ssk1ForSize, lmsOnlyForSig);
        }
        
        // Use the existing infrastructure to generate the signature
        CopyPartitionSignature(bi, sections, signatureBlock, &dummyCert);
        
        LOG_TRACE("Generated second signature using %s algorithm", GetAlgorithmName(algType));
        
    } catch (const std::exception& e) {
        LOG_ERROR("Exception during second signature generation: %s", e.what());
    } catch (...) {
        LOG_ERROR("Unknown exception during second signature generation");
    }
    
    // Restore original keys (temp keys were owned by primaryKey/secondaryKey and are deleted by reset)
    primaryKey.reset(originalPrimary);
    secondaryKey.reset(originalSecondary);
    
    // Clean up temporary hash if created
    if (hash != originalHash && hash != nullptr) {
        delete hash;
    }
    
    // Restore original authentication context state
    authAlgorithm.reset(originalAlg);
    signatureLength = originalSigLength;
    sskFile = originalSskFile;
    pskFile = originalPskFile;
    spkFile = originalSpkFile;
    ppkFile = originalPpkFile;
    hash = originalHash;
    hashLength = originalHashLength;
    hashType = originalHashType;
}

void Versal_2vpHybridAuthenticationContext::GenerateHashblock0SignatureForSecondAlgorithm(BootImage& bi, Section* hashBlockSection, uint8_t* signatureBlock, uint16_t sigLength, Authentication::Type algType)
{
    LOG_TRACE("Generating hashblock0 signature for second algorithm: %s", GetAlgorithmName(algType));
    
    // Save current authentication context state
    AuthenticationAlgorithm* originalAlg = authAlgorithm.release();
    uint32_t originalSigLength = signatureLength;
    uint8_t originalHashLength = hashLength;
    Hash* originalHash = hash;
    Key* originalPrimary = primaryKey.release();
    Key* originalSecondary = secondaryKey.release();
    
    Hash* tempHashForSecondAlg = nullptr;
    
    try {
        // Set up authentication context for second algorithm
        authAlgorithm = GetAuthenticationAlgorithm(algType);
        signatureLength = sigLength;
        
        // Set correct hash context based on algorithm type
        switch (algType) {
            case Authentication::MLDSA:
            hashType = AuthHash::Sha2_512;
            tempHashForSecondAlg = new HashSha2_512();
                break;
            case Authentication::SLH_SHAKE256:
            case Authentication::LMS_SHAKE256:
            hashType = AuthHash::Shake256;
            tempHashForSecondAlg = new HashShake256();
                break;
            case Authentication::LMS_SHA2_256:
            hashType = AuthHash::Sha2;
            tempHashForSecondAlg = new HashSha2();
                break;
            default:
            hashType = bi.GetAuthHashAlgo();
            hash = bi.hash.get();
                break;
        }
        
        if (tempHashForSecondAlg) {
            hash = tempHashForSecondAlg;
        }
        hashLength = hash->GetHashLength();
        
        // Validate and use pre-loaded isolated keys
        if (!isolatedPrimaryKey || !isolatedSecondaryKey || 
            isolatedPrimaryKey->authType != algType || isolatedSecondaryKey->authType != algType ||
            !isolatedPrimaryKey->Loaded || !isolatedSecondaryKey->Loaded) {
            LOG_ERROR("Isolated keys not available for algorithm: %s", GetAlgorithmName(algType));
            throw std::runtime_error("Isolated keys not available for deferred execution");
        }
        
        primaryKey.reset(isolatedPrimaryKey);   // Borrow isolated key (do NOT take ownership - release before restore)
        secondaryKey.reset(isolatedSecondaryKey);
        
        // Save and update key file paths for second algorithm
        std::string originalPskFile = pskFile;
        std::string originalSskFile = sskFile;
        std::string originalPpkFile = ppkFile;
        std::string originalSpkFile = spkFile;
        
        if (!pskFile1Path.empty()) pskFile = pskFile1Path;
        if (!sskFile1Path.empty()) sskFile = sskFile1Path;
        if (!ppkFile1Path.empty()) ppkFile = ppkFile1Path;
        if (!spkFile1Path.empty()) spkFile = spkFile1Path;
        
        // For LMS, recalculate signature length with correct sskFile
        if (IsLmsAlgorithm(algType)) {
            if (!sskFile.empty()) {
                bool lmsOnlyForSecondAlg = isolatedSecondaryKey ? isolatedSecondaryKey->lmsOnly : true;
                size_t actualSignatureLength = GetLmsSignLength(sskFile.c_str(), lmsOnlyForSecondAlg);
                size_t oldSignatureLength = signatureLength;
                signatureLength = actualSignatureLength + PADDING_16B(actualSignatureLength);
                
                // Reallocate bHsignature buffer if needed
                if (signatureLength != oldSignatureLength || bHsignature == nullptr) {
                        bHsignature.reset(new uint8_t[signatureLength]);
                    }
                sigLength = signatureLength;
            } else {
                throw std::runtime_error("Cannot calculate LMS signature length without sskFile");
            }
        }
        
        // Generate signature using CopyPartitionSignature
        std::list<Section*> hashSections;
        hashSections.push_back(hashBlockSection);
        Section dummyCert("dummy_cert", signatureLength);
        
        LOG_INFO("GenerateHashblock0SignatureForSecondAlgorithm: Writing to signatureBlock=%p, sigLength=%u, algType=%s",
                 signatureBlock, sigLength, GetAlgorithmName(algType));
        
        CopyPartitionSignature(bi, hashSections, signatureBlock, &dummyCert);
        
        // Debug: Log first few bytes of generated signature
        LOG_INFO("Generated hashblock0 signature (first 16 bytes): %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
                 signatureBlock[0], signatureBlock[1], signatureBlock[2], signatureBlock[3],
                 signatureBlock[4], signatureBlock[5], signatureBlock[6], signatureBlock[7],
                 signatureBlock[8], signatureBlock[9], signatureBlock[10], signatureBlock[11],
                 signatureBlock[12], signatureBlock[13], signatureBlock[14], signatureBlock[15]);
        
        // Restore original keys and file paths (release isolated keys without deleting)
        primaryKey.release();
        secondaryKey.release();
        primaryKey.reset(originalPrimary);
        secondaryKey.reset(originalSecondary);
        pskFile = originalPskFile;
        sskFile = originalSskFile;
        ppkFile = originalPpkFile;
        spkFile = originalSpkFile;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Exception during second algorithm signature generation: %s", e.what());
    }
    
    // Clean up temporary objects
    if (tempHashForSecondAlg && hash == tempHashForSecondAlg) {
        delete tempHashForSecondAlg;
        tempHashForSecondAlg = nullptr;
    }
    if (authAlgorithm && authAlgorithm.get() != originalAlg) {
        authAlgorithm.reset();  // Delete the temporary algorithm
    }
    
    // Restore original authentication context
    authAlgorithm.reset(originalAlg);
    signatureLength = originalSigLength;
    hashLength = originalHashLength;
    hash = originalHash;
    primaryKey.release();  // Don't delete isolated keys
    secondaryKey.release();
    primaryKey.reset(originalPrimary);
    secondaryKey.reset(originalSecondary);
}

/******************************************************************************/
const char* Versal_2vpHybridAuthenticationContext::GetAlgorithmName(Authentication::Type algType)
{
    switch (algType) {
        case Authentication::RSA:         return "RSA4096";
        case Authentication::ECDSA:       return "ECDSA-P384";
        case Authentication::ECDSAp521:   return "ECDSA-P521";
        case Authentication::LMS_SHA2_256: return "LMS-SHA2-256";
        case Authentication::LMS_SHAKE256: return "LMS-SHAKE256";
        case Authentication::MLDSA:       return "ML-DSA-87";
        case Authentication::SLH_SHAKE256: return "SLH-SHAKE256";
        default:                          return "Unknown";
    }
}

/******************************************************************************/
uint32_t Versal_2vpHybridAuthenticationContext::GetHybridMask(Authentication::Type algType)
{
    switch (algType) {
        case Authentication::RSA:         return HYBRID_RSA4096;
        case Authentication::ECDSA:       return HYBRID_ECDSA_P384;
        case Authentication::ECDSAp521:   return HYBRID_ECDSA_P521;
        case Authentication::LMS_SHA2_256:
        case Authentication::LMS_SHAKE256:
            // For LMS: lmsOnly=true -> LMS (0x10), lmsOnly=false -> HSS (0x8)
            // Use first/second algorithm flag based on position
            return (algType == hybridAlgorithms[0]) 
                ? (lmsOnlyFirstAlg ? HYBRID_LMS : HYBRID_LMS_HSS)
                : (lmsOnlySecondAlg ? HYBRID_LMS : HYBRID_LMS_HSS);
        case Authentication::MLDSA:       return HYBRID_ML_DSA87;
        case Authentication::SLH_SHAKE256: return HYBRID_SLH_DSA_SHAKE_256S;
        default:                          return 0x00000000;
    }
}

/******************************************************************************/
Section* Versal_2vpHybridAuthenticationContext::CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection, bool isBootloader)
{
    LOG_INFO("Creating HYBRID certificates for section - %s", dataSection->Name.c_str());
    SetCurrentDataSection(dataSection);
    
    // Validate keys
    if (!primaryKey || !secondaryKey || !primaryKey->Loaded || !secondaryKey->Loaded) {
        LOG_ERROR("Keys not properly loaded for hybrid authentication");
        return nullptr;
    }

    // Get partition BIF options using helper function
    PartitionBifOptions* partBifOptionsForFlags = FindPartitionBifOptions(bi, dataSection->index);
    
    // Set lmsOnly flags for LMS algorithms based on PartitionBifOptions
    if (partBifOptionsForFlags) {
        if (IsLmsAlgorithm(hybridAlgorithms[0])) lmsOnlyFirstAlg = partBifOptionsForFlags->lmsOnly;
        if (IsLmsAlgorithm(hybridAlgorithms[1])) lmsOnlySecondAlg = partBifOptionsForFlags->lmsOnly;
    }
    
    // ============================================================================
    // Retrieve .kmd metadata from PartitionBifOptions for certificate population
    // ============================================================================
    // Retrieve .kmd metadata from PartitionBifOptions for certificate population
    KeyMetadata primaryMeta, secondaryMeta, primaryMeta1, secondaryMeta1;
    
    if (partBifOptionsForFlags) {
        primaryMeta = partBifOptionsForFlags->primaryMetadata;
        secondaryMeta = partBifOptionsForFlags->secondaryMetadata;
        primaryMeta1 = partBifOptionsForFlags->primaryMetadata1;
        secondaryMeta1 = partBifOptionsForFlags->secondaryMetadata1;
    }

    // For hybrid authentication, we need to create TWO certificates
    // Certificate 1: Using primary algorithm (algorithms[0])
    // Certificate 2: Using secondary algorithm (algorithms[1])

    // Create first certificate using primary algorithm - save original state
    uint32_t originalCertSize = certSize;
    uint32_t originalSigLength = signatureLength;
    uint8_t originalHashLength = hashLength;
    Hash* originalHash = hash;
    AuthHash::Type originalHashType = hashType;

    LOG_INFO("Setting up for primary algorithm: %s", GetAlgorithmName(hybridAlgorithms[0]));

    // Set up for primary algorithm
    authAlgorithm = GetAuthenticationAlgorithm(hybridAlgorithms[0]);
    if (!authAlgorithm) {
        LOG_ERROR("Failed to get authentication algorithm for primary algorithm");
        return nullptr;
    }
    
    SetKeyLength(hybridAlgorithms[0]);
    hashType = bi.GetAuthHashAlgo();
    certSize = GetCertificateSize();
    
    // For LMS/HSS, signature length must be calculated from key file (not a fixed value)
    // and spksignature buffer must be reallocated to the correct size
    if (IsLmsAlgorithm(hybridAlgorithms[0])) {
        bool lmsOnlyForSig = primaryKey ? primaryKey->lmsOnly : lmsOnly;
        signatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnlyForSig);
        if (signatureLength == 0) {
            LOG_ERROR("Failed to get LMS signature length from key file: %s", pskFile.c_str());
            return nullptr;
        }
        
        // Reallocate spksignature buffer to actual LMS signature size
        spksignature.reset(new uint8_t[signatureLength]);
        memset(spksignature.get(), 0, signatureLength);
        LOG_TRACE("LMS first algorithm: signatureLength=%d (lmsOnly=%d)", signatureLength, lmsOnlyForSig);
    } else {
        signatureLength = GetSignatureLength();
    }
    
    hash = bi.hash.get();  // Set hash from BootImage
    if (!hash) {
        LOG_ERROR("Hash object is null");
        return nullptr;
    }
    hashLength = hash->GetHashLength();
    std::string hashExtension = hash->GetHashFileExtension();

    LOG_INFO("Creating first certificate section with size: %u", certSize);

    // Create first certificate section
    std::string cert1Name = dataSection->Name;
    if (dataSection->index == 0) {
        cert1Name = GetCertificateName(cert1Name);
    }
    Section* cert1 = new Section(cert1Name + hashExtension, certSize);
    if (!cert1 || !cert1->Data) {
        LOG_ERROR("Failed to create first certificate section or allocate data");
        delete cert1;
        return nullptr;
    }
    cert1->isCertificate = true;
    cert1->index = dataSection->index;
    cache.Sections.push_back(std::unique_ptr<Section>(cert1));

    // Fill first certificate
    uint8_t* authCert1 = cert1->Data.get();
    memset(authCert1, 0, certSize);
    
    LOG_TRACE("Filling first certificate for algorithm: %s", GetAlgorithmName(hybridAlgorithms[0]));
    
    // Certificate filling logic for primary algorithm
    if (hybridAlgorithms[0] == Authentication::RSA) {
        AuthCertificate4096Sha3PaddingHBStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificate4096Sha3PaddingHBStructure_versal2vp*>(authCert1);
        
        // Fill PPK header
        PopulateHeaderFromKmdMetadata(&certStruct->ppkheader, primaryMeta, authAlgorithm->GetAuthHeader(),
            0x0, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x0);
        
        // Export PPK key
        if (!SafeKeyExport(primaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acPpk), "PPK (RSA)")) {
            return nullptr;
        }
        authAlgorithm->RearrangeEndianess(certStruct->acPpk.N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acPpk.N_extension, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acPpk.E, RSA_4096_E_SIZE);
        memset(certStruct->acPpkAlignment, 0, sizeof(certStruct->acPpkAlignment));
        
        // Fill SPK header
        PopulateHeaderFromKmdMetadata(&certStruct->spkheader, secondaryMeta, authAlgorithm->GetAuthHeader(),
            spkIdentification, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x1);
        
        // Export SPK key
        if (!SafeKeyExport(secondaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acSpk), "SPK (RSA)")) {
            return nullptr;
        }
        memset(certStruct->acSpkAlignment, 0, sizeof(certStruct->acSpkAlignment));
        authAlgorithm->RearrangeEndianess(certStruct->acSpk.N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acSpk.N_extension, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acSpk.E, RSA_4096_E_SIZE);
        
        // Copy SPK signature
            CopySPKSignature(bi, reinterpret_cast<uint8_t*>(&certStruct->acSpkSignature));
        
        // Set signature length fields
        certStruct->acSpkSignatureLength = RSA_4096_KEY_LENGTH;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));
        certStruct->acHashblockSignatureLength = RSA_4096_KEY_LENGTH;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }
    else if (hybridAlgorithms[0] == Authentication::ECDSA) {
        AuthCertificateECDSAHBStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificateECDSAHBStructure_versal2vp*>(authCert1);
        
        // Fill PPK header
        PopulateHeaderFromKmdMetadata(&certStruct->ppkheader, primaryMeta, authAlgorithm->GetAuthHeader(),
            0x0, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x0);
        
        // Export PPK key
        if (!SafeKeyExport(primaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acPpk), "PPK (ECDSA)")) {
            return nullptr;
        }
        
        // Fill SPK header
        PopulateHeaderFromKmdMetadata(&certStruct->spkheader, secondaryMeta, authAlgorithm->GetAuthHeader(),
            spkIdentification, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x1);
        
        // Export SPK key
        if (!SafeKeyExport(secondaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acSpk), "SPK (ECDSA)")) {
            return nullptr;
        }
        
        // Copy SPK signature
            CopySPKSignature(bi, reinterpret_cast<uint8_t*>(&certStruct->acSpkSignature));
        
        // Set signature length fields
        certStruct->acSpkSignatureLength = EC_P384_KEY_LENGTH * 2;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));
        certStruct->acHashblockSignatureLength = EC_P384_KEY_LENGTH * 2;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }
    else if (hybridAlgorithms[0] == Authentication::ECDSAp521) {
        AuthCertificateECDSAp521HBStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificateECDSAp521HBStructure_versal2vp*>(authCert1);
        
        // Fill PPK header
        PopulateHeaderFromKmdMetadata(&certStruct->ppkheader, primaryMeta, authAlgorithm->GetAuthHeader(),
            0x0, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x0);
        
        // Export PPK key
        if (!SafeKeyExport(primaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acPpk), "PPK (ECDSA-P521)")) {
            return nullptr;
        }
        memset(certStruct->acPpkAlignment, 0, sizeof(certStruct->acPpkAlignment));
        
        // Fill SPK header
        PopulateHeaderFromKmdMetadata(&certStruct->spkheader, secondaryMeta, authAlgorithm->GetAuthHeader(),
            spkIdentification, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x1);
        
        // Export SPK key
        if (!SafeKeyExport(secondaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acSpk), "SPK (ECDSA-P521)")) {
            return nullptr;
        }
        
        // Copy SPK signature
            CopySPKSignature(bi, reinterpret_cast<uint8_t*>(&certStruct->acSpkSignature));
        
        // Set signature length fields
        certStruct->acSpkSignatureLength = EC_P521_KEY_LENGTH2 * 2;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));
        certStruct->acHashblockSignatureLength = EC_P521_KEY_LENGTH2 * 2;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
        memset(certStruct->acSpkAlignment, 0, sizeof(certStruct->acSpkAlignment));
    }
    else if (IsLmsAlgorithm(hybridAlgorithms[0])) {
        // LMS dynamic structure: Header + PPK + alignment + Header + SPK + alignment + SPK signature
        uint8_t* certPtr = authCert1;
        
        // PPK header
        HeaderAuthCert_versal_2vp* ppkHeader = reinterpret_cast<HeaderAuthCert_versal_2vp*>(certPtr);
        PopulateHeaderFromKmdMetadata(ppkHeader, primaryMeta, 
            authAlgorithm->GetAuthHeader(lmsOnly, false, bi.options.IsDl9Series()),
            0x0, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x0);
        certPtr += sizeof(HeaderAuthCert_versal_2vp);
        
        // PPK key
        size_t ppkKeySize = GetLmsPublicKeyLength(ppkFile.c_str(), lmsOnly);
        if (!SafeKeyExport(primaryKey.get(), certPtr, "PPK (LMS)")) {
            return nullptr;
        }
        certPtr += ppkKeySize;
        
        // PPK alignment
        size_t ppkAlignment = PADDING_16B(ppkKeySize);
        memset(certPtr, 0, ppkAlignment);
        certPtr += ppkAlignment;
        
        // SPK header
        HeaderAuthCert_versal_2vp* spkHeader = reinterpret_cast<HeaderAuthCert_versal_2vp*>(certPtr);
        PopulateHeaderFromKmdMetadata(spkHeader, secondaryMeta,
            authAlgorithm->GetAuthHeader(lmsOnly, false, bi.options.IsDl9Series()),
            spkIdentification, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x1);
        certPtr += sizeof(HeaderAuthCert_versal_2vp);
        
        // SPK key
        size_t spkKeySize = GetLmsPublicKeyLength(spkFile.c_str(), lmsOnly);
        if (!SafeKeyExport(secondaryKey.get(), certPtr, "SPK (LMS)")) {
            return nullptr;
        }
        certPtr += spkKeySize;
        
        // SPK alignment
        size_t spkAlignment = PADDING_16B(spkKeySize);
        memset(certPtr, 0, spkAlignment);
        certPtr += spkAlignment;
        
        // Get signature lengths for LMS
        size_t actualSpkSignatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);
        size_t actualHashBlockSignatureLength = GetLmsSignLength(sskFile.c_str(), lmsOnly);
        size_t totalSpkSignatureLength = actualSpkSignatureLength + PADDING_16B(actualSpkSignatureLength);
        size_t totalHashBlockSignatureLength = actualHashBlockSignatureLength + PADDING_16B(actualHashBlockSignatureLength);
        
        // SPK signature length field (4 bytes + 12 bytes padding = 16 bytes aligned)
        WriteLittleEndian32(certPtr, totalSpkSignatureLength);
        certPtr += sizeof(uint32_t);
        memset(certPtr, 0, 12);
        certPtr += 12;
        
        // SPK signature
            CopySPKSignature(bi, certPtr);
        certPtr += actualSpkSignatureLength;
        
        // SPK signature alignment
        size_t spkSigPad = PADDING_16B(actualSpkSignatureLength);
        if (spkSigPad) {
            memset(certPtr, 0, spkSigPad);
            certPtr += spkSigPad;
        }
        
        // Hash block signature length field (4 bytes + 12 bytes padding)
        WriteLittleEndian32(certPtr, totalHashBlockSignatureLength);
        certPtr += sizeof(uint32_t);
        memset(certPtr, 0, 12);
        certPtr += 12;
    }
    else if (hybridAlgorithms[0] == Authentication::MLDSA) {
        if (certSize < sizeof(AuthCertificateMLDSAStructure_versal2vp)) {
            LOG_ERROR("Certificate size too small for ML-DSA structure");
            return nullptr;
        }
        
        AuthCertificateMLDSAStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificateMLDSAStructure_versal2vp*>(authCert1);
        
        // Fill PPK header
        memset(&certStruct->ppkheader, 0, sizeof(HeaderAuthCert_versal_2vp));
        PopulateHeaderFromKmdMetadata(&certStruct->ppkheader, primaryMeta, authAlgorithm->GetAuthHeader(),
            0x0, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x0);

        primaryKey->Export(certStruct->acPpk);

        // Fill SPK header
        memset(&certStruct->spkheader, 0, sizeof(HeaderAuthCert_versal_2vp));
        PopulateHeaderFromKmdMetadata(&certStruct->spkheader, secondaryMeta, authAlgorithm->GetAuthHeader(),
            spkIdentification, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x1);

        secondaryKey->Export(certStruct->acSpk);

        certStruct->acSpkSignatureLength = MLDSA_TOTAL_SIGN_LEN;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));

        // Copy SPK signature
        CopySPKSignature(bi, certStruct->acSpkSignature);
        
        // Set hashblock signature length (same as single signing)
        certStruct->acHashblockSignatureLength = MLDSA_TOTAL_SIGN_LEN;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }
    else if (hybridAlgorithms[0] == Authentication::SLH_SHAKE256) {
        if (certSize < sizeof(AuthCertificateSLHDSAStructure_versal2vp)) {
            LOG_ERROR("Certificate size too small for SLH-DSA structure");
            return nullptr;
        }
        
        AuthCertificateSLHDSAStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificateSLHDSAStructure_versal2vp*>(authCert1);
        
        // Fill PPK header
        memset(&certStruct->ppkheader, 0, sizeof(HeaderAuthCert_versal_2vp));
        PopulateHeaderFromKmdMetadata(&certStruct->ppkheader, primaryMeta, authAlgorithm->GetAuthHeader(),
            0x0, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x0);
        primaryKey->Export(certStruct->acPpk);

        // Fill SPK header
        memset(&certStruct->spkheader, 0, sizeof(HeaderAuthCert_versal_2vp));
        PopulateHeaderFromKmdMetadata(&certStruct->spkheader, secondaryMeta, authAlgorithm->GetAuthHeader(),
            spkIdentification, GetHybridMask(hybridAlgorithms[1]), 0x0, 0x1);
        secondaryKey->Export(certStruct->acSpk);

        // Set signature lengths
        certStruct->acSpkSignatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));
        CopySPKSignature(bi, certStruct->acSpkSignature);
        certStruct->acHashblockSignatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }

    LOG_TRACE("Created first certificate section: %s", cert1->Name.c_str());

    // Save the first algorithm before switching to second
    AuthenticationAlgorithm* tempOriginalAlg = authAlgorithm.release();

    // Set up second algorithm context
    LOG_TRACE("Setting up secondary algorithm: %s", GetAlgorithmName(hybridAlgorithms[1]));
    authAlgorithm = GetAuthenticationAlgorithm(hybridAlgorithms[1]);
    if (!authAlgorithm) {
        LOG_ERROR("Failed to get authentication algorithm for secondary algorithm");
        return nullptr;
    }
    SetKeyLength(hybridAlgorithms[1]);
    
    // Set up hash context for second algorithm
    Hash* tempHashForSecondAlg = nullptr;
    switch (hybridAlgorithms[1]) {
        case Authentication::MLDSA:
        hashType = AuthHash::Sha2_512;
        tempHashForSecondAlg = new HashSha2_512();
            break;
        case Authentication::SLH_SHAKE256:
        case Authentication::LMS_SHAKE256:
        hashType = AuthHash::Shake256;
        tempHashForSecondAlg = new HashShake256();
            break;
        case Authentication::LMS_SHA2_256:
        hashType = AuthHash::Sha2;
        tempHashForSecondAlg = new HashSha2();
            break;
        default:
            hashType = bi.GetAuthHashAlgo();
            break;
    }
    if (tempHashForSecondAlg) {
        hash = tempHashForSecondAlg;
    } else {
        hash = bi.hash.get();
    }
    
    if (!hash) {
        LOG_ERROR("Hash object is null for second algorithm");
        return nullptr;
    }
    hashLength = hash->GetHashLength();
    
    // Set certificate size based on algorithm type
    switch (hybridAlgorithms[1]) {
        case Authentication::MLDSA:
        certSize = sizeof(AuthCertificateMLDSAStructure_versal2vp);
            break;
        case Authentication::SLH_SHAKE256:
        certSize = sizeof(AuthCertificateSLHDSAStructure_versal2vp);
            break;
        case Authentication::RSA:
        certSize = sizeof(AuthCertificate4096Sha3PaddingHBStructure_versal2vp);
            break;
        case Authentication::ECDSA:
        certSize = sizeof(AuthCertificateECDSAHBStructure_versal2vp);
            break;
        case Authentication::ECDSAp521:
        certSize = sizeof(AuthCertificateECDSAp521HBStructure_versal2vp);
            break;
        case Authentication::LMS_SHA2_256:
        case Authentication::LMS_SHAKE256:
            certSize = 0;  // Deferred until after key loading
            lmsOnly = true;
            break;
        default:
        certSize = GetCertificateSize();
            break;
    }
    
    // Set signature length based on algorithm type
    switch (hybridAlgorithms[1]) {
        case Authentication::RSA:
        signatureLength = SIGN_LENGTH_VERSAL;
            break;
        case Authentication::ECDSA:
        signatureLength = EC_P384_KEY_LENGTH * 2;
            break;
        case Authentication::ECDSAp521:
        signatureLength = EC_P521_KEY_LENGTH2 * 2;
            break;
        case Authentication::LMS_SHA2_256:
        case Authentication::LMS_SHAKE256:
            signatureLength = 0;  // Will be calculated from key file at line 1583
            break;
        case Authentication::MLDSA:
        signatureLength = MLDSA_TOTAL_SIGN_LEN;
            break;
        case Authentication::SLH_SHAKE256:
        signatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
            break;
        default:
            break;
    }
    
    // Reallocate spksignature buffer
    if (signatureLength > 0) {
        spksignature.reset(new uint8_t[signatureLength]);
        memset(spksignature.get(), 0, signatureLength);
    }

    // Load second algorithm keys for deferred execution
    LOG_TRACE("Loading second algorithm (%s) keys", GetAlgorithmName(hybridAlgorithms[1]));
    
    // Get partition-level BIF options
    PartitionBifOptions* partitionBifOptions = FindPartitionBifOptions(bi, dataSection->index);
    
    // Get second algorithm key file names (partition-level priority)
    std::string psk1 = GetKeyFileWithPriority(partitionBifOptions, bi.bifOptions, 
        partitionBifOptions ? partitionBifOptions->pskFile1 : "", &BifOptions::GetPSKFileName1);
    std::string ssk1 = GetKeyFileWithPriority(partitionBifOptions, bi.bifOptions,
        partitionBifOptions ? partitionBifOptions->sskFile1 : "", &BifOptions::GetSSKFileName1);
    std::string ppk1 = GetKeyFileWithPriority(partitionBifOptions, bi.bifOptions,
        partitionBifOptions ? partitionBifOptions->ppkFile1 : "", &BifOptions::GetPPKFileName1);
    std::string spk1 = GetKeyFileWithPriority(partitionBifOptions, bi.bifOptions,
        partitionBifOptions ? partitionBifOptions->spkFile1 : "", &BifOptions::GetSPKFileName1);
    
    // Store key file paths for deferred execution
    this->pskFile1Path = psk1;
    this->sskFile1Path = ssk1;
    this->ppkFile1Path = ppk1;
    this->spkFile1Path = spk1;
    
    // Get lmsOnly flag from PartitionBifOptions
    bool lmsOnly = partitionBifOptions ? partitionBifOptions->lmsOnly : true;
    
    // Create isolated keys for second algorithm
    this->isolatedPrimaryKey = CreateKeyForAlgorithm(hybridAlgorithms[1], "Isolated-Primary", lmsOnly);
    this->isolatedSecondaryKey = CreateKeyForAlgorithm(hybridAlgorithms[1], "Isolated-Secondary", lmsOnly);
    
    // Load isolated keys (private first, then public preserving isSecret flag)
    try {
        if (!psk1.empty()) {
                this->isolatedPrimaryKey->ParseSecret(psk1);
        }
        if (!ssk1.empty()) {
                this->isolatedSecondaryKey->ParseSecret(ssk1);
        }
        
        if (!ppk1.empty()) {
            bool savedIsSecret = this->isolatedPrimaryKey->isSecret;
            this->isolatedPrimaryKey->ParsePublic(ppk1);
            this->isolatedPrimaryKey->isSecret = savedIsSecret;
        }
        if (!spk1.empty()) {
            bool savedIsSecret = this->isolatedSecondaryKey->isSecret;
            this->isolatedSecondaryKey->ParsePublic(spk1);
            this->isolatedSecondaryKey->isSecret = savedIsSecret;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load isolated %s keys: %s", GetAlgorithmName(hybridAlgorithms[1]), e.what());
        delete this->isolatedPrimaryKey;
        delete this->isolatedSecondaryKey;
        this->isolatedPrimaryKey = this->isolatedSecondaryKey = nullptr;
        return nullptr;
    } catch (...) {
        LOG_ERROR("Unknown exception loading isolated %s keys", GetAlgorithmName(hybridAlgorithms[1]));
        delete this->isolatedPrimaryKey;
        delete this->isolatedSecondaryKey;
        this->isolatedPrimaryKey = this->isolatedSecondaryKey = nullptr;
        return nullptr;
    }

    // Store references to use later when switching to second algorithm
    Key* tempPrimaryKey = this->isolatedPrimaryKey;
    Key* tempSecondaryKey = this->isolatedSecondaryKey;
    Key* originalPrimary = primaryKey.get();
    Key* originalSecondary = secondaryKey.get();
    
    // CRITICAL: Save original key file paths before switching
    std::string originalPskFile = pskFile;
    std::string originalSskFile = sskFile;
    std::string originalPpkFile = ppkFile;
    std::string originalSpkFile = spkFile;
    
    // Check if we need different keys for the second algorithm
    bool needNewKeys = (hybridAlgorithms[1] != hybridAlgorithms[0]);
    
    if (needNewKeys) {
        LOG_TRACE("Switching to isolated %s keys for second algorithm", GetAlgorithmName(hybridAlgorithms[1]));
        
        // Replace primaryKey/secondaryKey with isolated keys
        // Must release() first to avoid deleting the original key that originalPrimary points to
        if (tempPrimaryKey) {
            primaryKey.release();
            primaryKey.reset(tempPrimaryKey);
        }
        if (tempSecondaryKey) {
            secondaryKey.release();
            secondaryKey.reset(tempSecondaryKey);
        }
        
        // Update key file paths for second algorithm
        if (!psk1.empty()) pskFile = psk1;
        if (!ppk1.empty()) ppkFile = ppk1;
        if (!ssk1.empty()) sskFile = ssk1;
        if (!spk1.empty()) spkFile = spk1;
        
        // LMS-specific: recalculate signature length and reallocate buffer
        if (IsLmsAlgorithm(hybridAlgorithms[1])) {
            signatureLength = GetLmsSignLength(pskFile.c_str(), lmsOnly);
            if (signatureLength == 0) {
                LOG_ERROR("Failed to get LMS signature length from key file: %s", pskFile.c_str());
                return nullptr;
            }
            spksignature.reset(new uint8_t[signatureLength]);
            memset(spksignature.get(), 0, signatureLength);
        }
        
        // Update authAlgorithm for proper certificate size calculation
        authAlgorithm.reset(CreateAuthAlgorithmInstance(hybridAlgorithms[1]));
        if (!authAlgorithm) {
            LOG_ERROR("Failed to create algorithm instance for %s", GetAlgorithmName(hybridAlgorithms[1]));
                return nullptr;
            }
            certSize = GetCertificateSize();
    } else {
        // Create keys for second algorithm using helper
        tempPrimaryKey = CreateKeyForAlgorithm(hybridAlgorithms[1], "Primary", lmsOnly);
        tempSecondaryKey = CreateKeyForAlgorithm(hybridAlgorithms[1], "Secondary", lmsOnly);
    }

    // Create second certificate section
    std::string cert2Name = dataSection->Name + ".auth2";
    std::string fullCert2Name = cert2Name + hashExtension;
    LOG_INFO("Creating second certificate section: Name=%s, size=%u, algorithm=%s", 
             fullCert2Name.c_str(), certSize, GetAlgorithmName(hybridAlgorithms[1]));
    Section* cert2 = new Section(fullCert2Name, certSize);
    cert2->isCertificate = true;
    cert2->index = dataSection->index;
    cache.Sections.push_back(std::unique_ptr<Section>(cert2));
    LOG_INFO("Created cert2 section: Data=%p, pushed to cache", cert2->Data.get());

    // Fill second certificate
    uint8_t* authCert2 = cert2->Data.get();
    memset(authCert2, 0, certSize);
    
    LOG_TRACE("Filling second certificate for algorithm: %s", GetAlgorithmName(hybridAlgorithms[1]));
    
    // Helper lambda for cleanup on error
    auto cleanupAndReturn = [&]() -> Section* {
        if (needNewKeys) {
            primaryKey.release();  // Don't delete isolated keys
            secondaryKey.release();
            primaryKey.reset(originalPrimary);
            secondaryKey.reset(originalSecondary);
        }
        if (tempOriginalAlg) {
            authAlgorithm.reset(tempOriginalAlg);  // Restore original algorithm
        }
        if (tempHashForSecondAlg && hash == tempHashForSecondAlg) {
            delete tempHashForSecondAlg;
        }
        if (tempPrimaryKey && tempPrimaryKey != originalPrimary && tempPrimaryKey != isolatedPrimaryKey) delete tempPrimaryKey;
        if (tempSecondaryKey && tempSecondaryKey != originalSecondary && tempSecondaryKey != isolatedSecondaryKey) delete tempSecondaryKey;
        return nullptr;
    };
    
    if (hybridAlgorithms[1] == Authentication::RSA) {
        AuthCertificate4096Sha3PaddingHBStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificate4096Sha3PaddingHBStructure_versal2vp*>(authCert2);
        
        // Fill PPK header
        PopulateHeaderFromKmdMetadata(&certStruct->ppkheader, primaryMeta1,
            authAlgorithm->GetAuthHeader(), 0x0, GetHybridMask(hybridAlgorithms[0]), 0x0, 0x0);
        
        // Export PPK key with validation
        // Export PPK key with validation
        if (!SafeKeyExport(primaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acPpk), "PPK (RSA cert2)")) {
            return cleanupAndReturn();
        }
        authAlgorithm->RearrangeEndianess(certStruct->acPpk.N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acPpk.N_extension, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acPpk.E, RSA_4096_E_SIZE);
        memset(certStruct->acPpkAlignment, 0, sizeof(certStruct->acPpkAlignment));
        
        // Fill SPK header
        PopulateHeaderFromKmdMetadata(&certStruct->spkheader, secondaryMeta1,
            authAlgorithm->GetAuthHeader(), spkIdentification, GetHybridMask(hybridAlgorithms[0]),
            0x0, 0x1);
        
        // Export SPK key with validation
        if (!SafeKeyExport(secondaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acSpk), "SPK (RSA cert2)")) {
            return cleanupAndReturn();
        }
        memset(certStruct->acSpkAlignment, 0, sizeof(certStruct->acSpkAlignment));
        authAlgorithm->RearrangeEndianess(certStruct->acSpk.N, RSA_4096_N_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acSpk.N_extension, RSA_4096_N_EXT_SIZE);
        authAlgorithm->RearrangeEndianess(certStruct->acSpk.E, RSA_4096_E_SIZE);
        
        // Copy SPK signature
            CopySPKSignature(bi, reinterpret_cast<uint8_t*>(&certStruct->acSpkSignature));
        
        // Set signature length fields
        certStruct->acSpkSignatureLength = RSA_4096_KEY_LENGTH;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));
        certStruct->acHashblockSignatureLength = RSA_4096_KEY_LENGTH;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }
    else if (hybridAlgorithms[1] == Authentication::ECDSA) {
        AuthCertificateECDSAHBStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificateECDSAHBStructure_versal2vp*>(authCert2);
        
        // Fill PPK header and export key
        PopulateHeaderFromKmdMetadata(&certStruct->ppkheader, primaryMeta1,
            authAlgorithm->GetAuthHeader(), 0x0, GetHybridMask(hybridAlgorithms[0]), 0x0, 0x0);
        if (!SafeKeyExport(primaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acPpk), "PPK (ECDSA cert2)")) {
            return cleanupAndReturn();
        }
        
        // Fill SPK header and export key
        PopulateHeaderFromKmdMetadata(&certStruct->spkheader, secondaryMeta1,
            authAlgorithm->GetAuthHeader(), spkIdentification, GetHybridMask(hybridAlgorithms[0]),
            0x0, 0x1);
        if (!SafeKeyExport(secondaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acSpk), "SPK (ECDSA cert2)")) {
            return cleanupAndReturn();
        }
        
        // Copy SPK signature
            CopySPKSignature(bi, reinterpret_cast<uint8_t*>(&certStruct->acSpkSignature));
        
        // Set signature length fields
        certStruct->acSpkSignatureLength = EC_P384_KEY_LENGTH * 2;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));
        certStruct->acHashblockSignatureLength = EC_P384_KEY_LENGTH * 2;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }
    else if (hybridAlgorithms[1] == Authentication::ECDSAp521) {
        AuthCertificateECDSAp521HBStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificateECDSAp521HBStructure_versal2vp*>(authCert2);
        
        // Fill PPK header and export key
        PopulateHeaderFromKmdMetadata(&certStruct->ppkheader, primaryMeta1,
            authAlgorithm->GetAuthHeader(), 0x0, GetHybridMask(hybridAlgorithms[0]), 0x0, 0x0);
        if (!SafeKeyExport(primaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acPpk), "PPK (ECDSA-P521 cert2)")) {
            return cleanupAndReturn();
        }
        memset(certStruct->acPpkAlignment, 0, sizeof(certStruct->acPpkAlignment));
        
        // Fill SPK header and export key
        PopulateHeaderFromKmdMetadata(&certStruct->spkheader, secondaryMeta1,
            authAlgorithm->GetAuthHeader(), spkIdentification, GetHybridMask(hybridAlgorithms[0]),
            0x0, 0x1);
        if (!SafeKeyExport(secondaryKey.get(), reinterpret_cast<uint8_t*>(&certStruct->acSpk), "SPK (ECDSA-P521 cert2)")) {
            return cleanupAndReturn();
        }
        memset(certStruct->acSpkAlignment, 0, sizeof(certStruct->acSpkAlignment));
        
        // Copy SPK signature
            CopySPKSignature(bi, reinterpret_cast<uint8_t*>(&certStruct->acSpkSignature));
        
        // Set signature length fields
        certStruct->acSpkSignatureLength = EC_P521_KEY_LENGTH2 * 2;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));
        certStruct->acHashblockSignatureLength = EC_P521_KEY_LENGTH2 * 2;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }
    else if (hybridAlgorithms[1] == Authentication::MLDSA) {
        if (certSize < sizeof(AuthCertificateMLDSAStructure_versal2vp)) {
            LOG_ERROR("Certificate size too small for ML-DSA cert2");
            return cleanupAndReturn();
        }
        
        AuthCertificateMLDSAStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificateMLDSAStructure_versal2vp*>(authCert2);
        
        // Fill PPK header and export key
        PopulateHeaderFromKmdMetadata(&certStruct->ppkheader, primaryMeta1,
            authAlgorithm->GetAuthHeader(), 0x0, GetHybridMask(hybridAlgorithms[0]), 0x0, 0x0);
        if (!SafeKeyExport(primaryKey.get(), certStruct->acPpk, "PPK (ML-DSA cert2)")) {
            return cleanupAndReturn();
        }
        
        // Fill SPK header and export key
        PopulateHeaderFromKmdMetadata(&certStruct->spkheader, secondaryMeta1,
            authAlgorithm->GetAuthHeader(), spkIdentification, GetHybridMask(hybridAlgorithms[0]),
            0x0, 0x1);
        if (!SafeKeyExport(secondaryKey.get(), certStruct->acSpk, "SPK (ML-DSA cert2)")) {
            return cleanupAndReturn();
        }
        
        // Copy SPK signature
            CopySPKSignature(bi, certStruct->acSpkSignature);
        
        // Set signature length fields
        certStruct->acSpkSignatureLength = MLDSA_TOTAL_SIGN_LEN;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));
        certStruct->acHashblockSignatureLength = MLDSA_TOTAL_SIGN_LEN;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }
    else if (IsLmsAlgorithm(hybridAlgorithms[1])) {
        bool lmsOnlyForCert2 = isolatedSecondaryKey ? isolatedSecondaryKey->lmsOnly :
                               (secondaryKey ? secondaryKey->lmsOnly : true);
        
        // Use second algorithm's key file paths for LMS key/signature size calculations
        std::string ppkFilePath2 = !ppkFile1Path.empty() ? ppkFile1Path : ppkFile;
        std::string spkFilePath2 = !spkFile1Path.empty() ? spkFile1Path : spkFile;
        std::string pskFilePath2 = !pskFile1Path.empty() ? pskFile1Path : pskFile;
        std::string sskFilePath2 = !sskFile1Path.empty() ? sskFile1Path : sskFile;
        
        uint8_t* certPtr = authCert2;
        
        // PPK header
        HeaderAuthCert_versal_2vp* ppkHeader = reinterpret_cast<HeaderAuthCert_versal_2vp*>(certPtr);
        PopulateHeaderFromKmdMetadata(ppkHeader, primaryMeta1,
            authAlgorithm->GetAuthHeader(lmsOnlyForCert2, false, bi.options.IsDl9Series()),
            0x0, GetHybridMask(hybridAlgorithms[0]), 0x0, 0x0);
        certPtr += sizeof(HeaderAuthCert_versal_2vp);
        
        // PPK key - use second algorithm's key path
        size_t ppkKeySize = GetLmsPublicKeyLength(ppkFilePath2.c_str(), lmsOnlyForCert2);
        if (!SafeKeyExport(primaryKey.get(), certPtr, "PPK (LMS cert2)")) {
            return cleanupAndReturn();
        }
        certPtr += ppkKeySize;
        memset(certPtr, 0, PADDING_16B(ppkKeySize));
        certPtr += PADDING_16B(ppkKeySize);
        
        // SPK header
        HeaderAuthCert_versal_2vp* spkHeader = reinterpret_cast<HeaderAuthCert_versal_2vp*>(certPtr);
        PopulateHeaderFromKmdMetadata(spkHeader, secondaryMeta1,
            authAlgorithm->GetAuthHeader(lmsOnlyForCert2, false, bi.options.IsDl9Series()),
            spkIdentification, GetHybridMask(hybridAlgorithms[0]), 0x0, 0x1);
        certPtr += sizeof(HeaderAuthCert_versal_2vp);
        
        // SPK key - use second algorithm's key path
        size_t spkKeySize = GetLmsPublicKeyLength(spkFilePath2.c_str(), lmsOnlyForCert2);
        if (!SafeKeyExport(secondaryKey.get(), certPtr, "SPK (LMS cert2)")) {
            return cleanupAndReturn();
        }
        certPtr += spkKeySize;
        memset(certPtr, 0, PADDING_16B(spkKeySize));
        certPtr += PADDING_16B(spkKeySize);
        
        // Get signature lengths - use second algorithm's key paths
        size_t actualSpkSignatureLength = GetLmsSignLength(pskFilePath2.c_str(), lmsOnlyForCert2);
        size_t actualHashBlockSignatureLength = GetLmsSignLength(sskFilePath2.c_str(), lmsOnlyForCert2);
        size_t totalSpkSignatureLength = actualSpkSignatureLength + PADDING_16B(actualSpkSignatureLength);
        size_t totalHashBlockSignatureLength = actualHashBlockSignatureLength + PADDING_16B(actualHashBlockSignatureLength);
        
        // SPK signature length field (16-byte aligned)
        WriteLittleEndian32(certPtr, totalSpkSignatureLength);
        certPtr += sizeof(uint32_t);
        memset(certPtr, 0, 12);
        certPtr += 12;
        
        // SPK signature
            SetCurrentDataSection(dataSection);
            CopySPKSignature(bi, certPtr);
        certPtr += actualSpkSignatureLength;
        
        // SPK signature alignment
        size_t spkSigPad = PADDING_16B(actualSpkSignatureLength);
        if (spkSigPad) {
            memset(certPtr, 0, spkSigPad);
            certPtr += spkSigPad;
        }
        
        // Hash block signature length field (16-byte aligned)
        WriteLittleEndian32(certPtr, totalHashBlockSignatureLength);
        certPtr += sizeof(uint32_t);
        memset(certPtr, 0, 12);
        certPtr += 12;
    }
    else if (hybridAlgorithms[1] == Authentication::SLH_SHAKE256) {
        if (certSize < sizeof(AuthCertificateSLHDSAStructure_versal2vp)) {
            LOG_ERROR("Certificate size too small for SLH-DSA cert2");
            return cleanupAndReturn();
        }
        
        AuthCertificateSLHDSAStructure_versal2vp* certStruct =
            reinterpret_cast<AuthCertificateSLHDSAStructure_versal2vp*>(authCert2);
        
        // Fill PPK header and export key
        PopulateHeaderFromKmdMetadata(&certStruct->ppkheader, primaryMeta1,
            authAlgorithm->GetAuthHeader(), 0x0, GetHybridMask(hybridAlgorithms[0]), 0x0, 0x0);
        if (!SafeKeyExport(primaryKey.get(), certStruct->acPpk, "PPK (SLH-DSA cert2)")) {
            return cleanupAndReturn();
        }
        
        // Fill SPK header and export key
        PopulateHeaderFromKmdMetadata(&certStruct->spkheader, secondaryMeta1,
            authAlgorithm->GetAuthHeader(), spkIdentification, GetHybridMask(hybridAlgorithms[0]),
            0x0, 0x1);
        if (!SafeKeyExport(secondaryKey.get(), certStruct->acSpk, "SPK (SLH-DSA cert2)")) {
            return cleanupAndReturn();
        }
        
        // Copy SPK signature
            CopySPKSignature(bi, certStruct->acSpkSignature);
        
        // Set signature length fields
        certStruct->acSpkSignatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
        memset(certStruct->acSpkSignatureLengthAlignment, 0, sizeof(certStruct->acSpkSignatureLengthAlignment));
        certStruct->acHashblockSignatureLength = SLHDSA_TOTAL_SIGN_LENGTH;
        memset(certStruct->acHashblockSignatureLengthAlignment, 0, sizeof(certStruct->acHashblockSignatureLengthAlignment));
    }

    LOG_TRACE("Created second certificate section: %s", cert2->Name.c_str());

    // Validate cert2 after creation
    if (!cert2 || !cert2->Data || cert2->Name.empty()) {
        LOG_ERROR("Second certificate section is invalid");
        return cleanupAndReturn();
    }

    // Restore original algorithm settings
    certSize = originalCertSize;
    signatureLength = originalSigLength;
    hashLength = originalHashLength;
    
    // Restore original hash context - always restore since we set hash = bi.hash.get() early on
    if (tempHashForSecondAlg) {
        delete tempHashForSecondAlg;
        tempHashForSecondAlg = nullptr;
    }
    hash = originalHash;
    hashType = originalHashType;
    hashLength = originalHashLength;
    
    // Restore original algorithm if we switched it for second algorithm
    if (hybridAlgorithms[1] != hybridAlgorithms[0] && tempOriginalAlg && authAlgorithm) {
        LOG_TRACE("Restoring original algorithm context");
        authAlgorithm.reset(tempOriginalAlg);
    }

    // Restore original keys and clean up temporary keys
    if (needNewKeys) {
        // Restore original key pointers and file paths (release isolated keys without deleting)
        primaryKey.release();
        secondaryKey.release();
        primaryKey.reset(originalPrimary);
        secondaryKey.reset(originalSecondary);
        pskFile = originalPskFile;
        sskFile = originalSskFile;
        ppkFile = originalPpkFile;
        spkFile = originalSpkFile;

        // Clean up temporary keys (not isolated keys which are preserved for deferred execution)
        // Note: temp keys were owned by primaryKey/secondaryKey and are already deleted by reset()
        LOG_TRACE("Keys restored, isolated keys preserved for deferred execution");
    } else {
        // Clean up any temporary keys not referenced by current keys
        if (tempPrimaryKey && tempPrimaryKey != primaryKey.get()) {
            delete tempPrimaryKey;
        }
        if (tempSecondaryKey && tempSecondaryKey != secondaryKey.get()) {
            delete tempSecondaryKey;
        }
    }
    
    return cert1;
}

/******************************************************************************/
void Versal_2vpHybridAuthenticationContext::EnsureMLDSAPublicKeyForHybrid(Key* targetKey, const std::string& secretKeyFile, const std::string& keyName)
{
    if (!targetKey) {
        LOG_ERROR("Target key is null for %s", keyName.c_str());
        return;
    }
    
    // Already have public key
    if (targetKey->mldsa_public_key != nullptr) {
        return;
    }
    
    // Extract public key from private key (embedded in ML-DSA secret key)
    if (targetKey->mldsa_private_key != nullptr) {
        targetKey->mldsa_public_key = new uint8_t[MLDSA_PUB_KEY_LENGTH];
        memcpy(targetKey->mldsa_public_key, targetKey->mldsa_private_key, MLDSA_PUB_KEY_LENGTH);
        LOG_TRACE("Extracted ML-DSA public key for %s", keyName.c_str());
    } else {
        LOG_WARNING("ML-DSA private key not available for public key extraction - %s", keyName.c_str());
    }
}

/******************************************************************************/
void Versal_2vpHybridAuthenticationContext::LoadPublicKeyForAlgorithm(Key* targetKey, const std::string& keyFile, Authentication::Type algType, const std::string& keyName)
{
    if (!targetKey) {
        LOG_ERROR("Target key is null for %s", keyName.c_str());
        return;
    }
    
    LOG_TRACE("Loading public key for %s, algorithm: %s", keyName.c_str(), GetAlgorithmName(algType));
    
    // Helper lambda for safe temp key cleanup
    auto safeDelete = [](auto* key) { if (key) { try { delete key; } catch (...) {} } };
    
    switch (algType) {
        case Authentication::MLDSA:
        {
            KeyMLDSA_versal_2vp* tempKey = nullptr;
            try {
                tempKey = new KeyMLDSA_versal_2vp(keyName + "_Temp");
                tempKey->authType = Authentication::MLDSA;
                tempKey->ParsePublic(keyFile);
                
                if (tempKey->Loaded && tempKey->mldsa_public_key) {
                        delete[] targetKey->mldsa_public_key;
                    targetKey->mldsa_public_key = new uint8_t[MLDSA_PUB_KEY_LENGTH];
                    memcpy(targetKey->mldsa_public_key, tempKey->mldsa_public_key, MLDSA_PUB_KEY_LENGTH);
                } else {
                    LOG_ERROR("Failed to load %s public key for ML-DSA", keyName.c_str());
                }
                delete tempKey;
            } catch (const std::exception& e) {
                LOG_ERROR("Exception loading ML-DSA key %s: %s", keyName.c_str(), e.what());
                safeDelete(tempKey);
                    } catch (...) {
                LOG_ERROR("Unknown exception loading ML-DSA key %s", keyName.c_str());
                safeDelete(tempKey);
            }
            break;
        }
        
        case Authentication::LMS_SHA2_256:
        case Authentication::LMS_SHAKE256:
        {
            KeyLMS_versal_2vp* tempKey = nullptr;
            try {
                tempKey = new KeyLMS_versal_2vp(keyName + "_Temp");
                tempKey->authType = algType;
                tempKey->ParsePublic(keyFile);
                
                if (tempKey->Loaded) {
                    targetKey->Loaded = tempKey->Loaded;
                    targetKey->authType = tempKey->authType;
                } else {
                    LOG_ERROR("Failed to load %s public key for LMS", keyName.c_str());
                }
                delete tempKey;
            } catch (const std::exception& e) {
                LOG_ERROR("Exception loading LMS key %s: %s", keyName.c_str(), e.what());
                safeDelete(tempKey);
                    } catch (...) {
                LOG_ERROR("Unknown exception loading LMS key %s", keyName.c_str());
                safeDelete(tempKey);
            }
            break;
        }
        
        case Authentication::SLH_SHAKE256:
        {
            KeySLH_versal_2vp* tempKey = nullptr;
            try {
                tempKey = new KeySLH_versal_2vp(keyName + "_Temp");
                tempKey->authType = Authentication::SLH_SHAKE256;
                tempKey->ParsePublic(keyFile);
                
                if (tempKey->Loaded && tempKey->slhdsa_public_key) {
                        delete[] targetKey->slhdsa_public_key;
                    targetKey->slhdsa_public_key = new uint8_t[SLHDSA_PUB_KEY_LENGTH];
                    memcpy(targetKey->slhdsa_public_key, tempKey->slhdsa_public_key, SLHDSA_PUB_KEY_LENGTH);
                } else {
                    LOG_ERROR("Failed to load %s public key for SLH-DSA", keyName.c_str());
                }
                delete tempKey;
            } catch (const std::exception& e) {
                LOG_ERROR("Exception loading SLH-DSA key %s: %s", keyName.c_str(), e.what());
                safeDelete(tempKey);
                    } catch (...) {
                LOG_ERROR("Unknown exception loading SLH-DSA key %s", keyName.c_str());
                safeDelete(tempKey);
            }
            break;
        }
        
        default:
        {
            // Standard algorithms (RSA, ECDSA, ECDSAp521)
            try {
                targetKey->ParsePublic(keyFile);
                if (!targetKey->Loaded) {
                    LOG_ERROR("Failed to load %s public key for %s", keyName.c_str(), GetAlgorithmName(algType));
                }
            } catch (const std::exception& e) {
                LOG_ERROR("Exception loading %s key %s: %s", GetAlgorithmName(algType), keyName.c_str(), e.what());
            } catch (...) {
                LOG_ERROR("Unknown exception loading %s key %s", GetAlgorithmName(algType), keyName.c_str());
            }
            break;
        }
    }
}
