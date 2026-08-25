/******************************************************************************
* Copyright 2025 Advanced Micro Devices, Inc.
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
****************************************************** DEVICE-SPECIFIC BIFOPTIONS
-------------------------------------------------------------------------------
This file contains Versal 2VP (versal_2vp) device-specific implementations for .kmd
metadata parsing and algorithm conversion functions.

These functions are isolated from common code to:
  1. Avoid polluting common/ directory with device-specific headers
  2. Allow future devices to provide their own implementations
  3. Keep device-specific constants (AUTH_HDR_*, HYBRID_*) localized
*/

#include "bifoptions-versal_2vp.h"
#include "authentication-versal_2vp.h"  // For AUTH_HDR_TELLURIDE_* and HYBRID_* macros
#include "bifoptions.h"                 // For BifOptions / PartitionBifOptions
#include "logger.h"
#include "stringutils.h"
#include "bootgenexception.h"
#include <algorithm>

// Helper function to trim whitespace from string
static std::string Trim(const std::string& str)
{
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

namespace BifOptionsVersal2VP
{
    /**************************************************************************/
    uint32_t ConvertAlgorithmStringToHeaderValue(const std::string& algStr)
    {
        std::string lower = algStr;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        // AUTH_HDR_TELLURIDE values from authentication-versal_2vp.h
        if (lower == "rsa" || lower == "rsa-4096" || lower == "rsa4096") 
            return AUTH_HDR_TELLURIDE_RSA_4096;
        
        if (lower == "ecdsa" || lower == "ecdsa-p384") 
            return AUTH_HDR_TELLURIDE_ECDSA_P384;
        
        if (lower == "ecdsa-p521" || lower == "ecdsap521") 
            return AUTH_HDR_TELLURIDE_ECDSA_P521;
        
        if (lower == "hss" || lower == "hss-sha256" || lower == "hss-sha2-256" || 
            lower == "hss-shake256" || lower == "hss-lms") 
            return AUTH_HDR_TELLURIDE_HSS_LMS;
        
        if (lower == "lms" || lower == "lms-sha256" || lower == "lms-sha2-256" || 
            lower == "lms-shake256") 
            return AUTH_HDR_TELLURIDE_LMS;
        
        if (lower == "mldsa" || lower == "ml-dsa" || lower == "ml-dsa87" || lower == "mldsa87") 
            return AUTH_HDR_TELLURIDE_ML_DSA87;
        
        if (lower == "slh" || lower == "slh-dsa" || lower == "slhdsa" || lower == "slh-shake256") 
            return AUTH_HDR_TELLURIDE_SLH_DSA;
        
        LOG_WARNING("Unknown algorithm string for Versal 2VP: '%s', returning NONSECURE (0x00)", algStr.c_str());
        return AUTH_HDR_TELLURIDE_NONSECURE;
    }

    /**************************************************************************/
    uint32_t ConvertHybridStringToBitmask(const std::string& hybridStr)
    {
        uint32_t bitmask = 0;
        
        if (hybridStr.empty()) {
            return 0;
        }
        
        // Split by comma and process each algorithm
        std::string temp = hybridStr;
        size_t pos = 0;
        std::string token;
        
        while ((pos = temp.find(',')) != std::string::npos) {
            token = temp.substr(0, pos);
            token = Trim(token);
            
            // Convert algorithm string to bitmask value
            std::string lower = token;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            
            if (lower == "rsa" || lower == "rsa-4096" || lower == "rsa4096") 
                bitmask |= HYBRID_RSA4096;
            else if (lower == "ecdsa" || lower == "ecdsa-p384") 
                bitmask |= HYBRID_ECDSA_P384;
            else if (lower == "ecdsa-p521" || lower == "ecdsap521") 
                bitmask |= HYBRID_ECDSA_P521;
            else if (lower == "hss" || lower == "hss-sha256" || lower == "hss-sha2-256" || 
                     lower == "hss-shake256" || lower == "hss-lms") 
                bitmask |= HYBRID_LMS_HSS;
            else if (lower == "lms" || lower == "lms-sha256" || lower == "lms-sha2-256" || 
                     lower == "lms-shake256") 
                bitmask |= HYBRID_LMS;
            else if (lower == "mldsa" || lower == "ml-dsa" || lower == "ml-dsa87" || lower == "mldsa87") 
                bitmask |= HYBRID_ML_DSA87;
            else if (lower == "slh" || lower == "slh-dsa" || lower == "slhdsa" || lower == "slh-shake256") 
                bitmask |= HYBRID_SLH_DSA_SHAKE_256S;
            else
                LOG_WARNING("Unknown hybrid algorithm for Versal 2VP: '%s'", token.c_str());
            
            temp.erase(0, pos + 1);
        }
        
        // Process last token (or only token if no comma)
        token = Trim(temp);
        if (!token.empty()) {
            std::string lower = token;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            
            if (lower == "rsa" || lower == "rsa-4096" || lower == "rsa4096") 
                bitmask |= HYBRID_RSA4096;
            else if (lower == "ecdsa" || lower == "ecdsa-p384") 
                bitmask |= HYBRID_ECDSA_P384;
            else if (lower == "ecdsa-p521" || lower == "ecdsap521") 
                bitmask |= HYBRID_ECDSA_P521;
            else if (lower == "hss" || lower == "hss-sha256" || lower == "hss-sha2-256" || 
                     lower == "hss-shake256" || lower == "hss-lms") 
                bitmask |= HYBRID_LMS_HSS;
            else if (lower == "lms" || lower == "lms-sha256" || lower == "lms-sha2-256" || 
                     lower == "lms-shake256") 
                bitmask |= HYBRID_LMS;
            else if (lower == "mldsa" || lower == "ml-dsa" || lower == "ml-dsa87" || lower == "mldsa87") 
                bitmask |= HYBRID_ML_DSA87;
            else if (lower == "slh" || lower == "slh-dsa" || lower == "slhdsa" || lower == "slh-shake256") 
                bitmask |= HYBRID_SLH_DSA_SHAKE_256S;
            else
                LOG_WARNING("Unknown hybrid algorithm for Versal 2VP: '%s'", token.c_str());
        }
        
        return bitmask;
    }

    /**************************************************************************/
    bool IsClassicalAlgorithm(uint32_t algorithm)
    {
        // Extract algorithm from [0:7] bits
        uint32_t alg = algorithm & 0xFF;
        
        // Classical algorithms: RSA-4096, ECDSA-P384, ECDSA-P521
        return (alg == AUTH_HDR_TELLURIDE_RSA_4096 || 
                alg == AUTH_HDR_TELLURIDE_ECDSA_P384 || 
                alg == AUTH_HDR_TELLURIDE_ECDSA_P521);
    }

    /**************************************************************************/
    bool IsPQCAlgorithm(uint32_t algorithm)
    {
        // Extract algorithm from [0:7] bits
        uint32_t alg = algorithm & 0xFF;
        
        // PQC algorithms: HSS-LMS, LMS, ML-DSA87, SLH-DSA
        return (alg == AUTH_HDR_TELLURIDE_HSS_LMS || 
                alg == AUTH_HDR_TELLURIDE_LMS || 
                alg == AUTH_HDR_TELLURIDE_ML_DSA87 || 
                alg == AUTH_HDR_TELLURIDE_SLH_DSA);
    }

    /**************************************************************************/
    std::string GetAlgorithmName(uint32_t algorithm)
    {
        uint32_t alg = algorithm & 0xFF;
        
        // Return names consistent with .kmd file algorithm strings
        switch (alg) {
            case AUTH_HDR_TELLURIDE_RSA_4096: return "RSA-4096";
            case AUTH_HDR_TELLURIDE_ECDSA_P384: return "ECDSA-P384";
            case AUTH_HDR_TELLURIDE_ECDSA_P521: return "ECDSA-P521";
            case AUTH_HDR_TELLURIDE_HSS_LMS: return "HSS-SHAKE256";
            case AUTH_HDR_TELLURIDE_LMS: return "LMS-SHAKE256";
            case AUTH_HDR_TELLURIDE_ML_DSA87: return "ML-DSA87";
            case AUTH_HDR_TELLURIDE_SLH_DSA: return "SLH-DSA-SHAKE256";
            default: return "Unknown";
        }
    }

    /**************************************************************************/
    bool IsAlgorithmInHybridMask(uint32_t algorithm, uint32_t hybridMask)
    {
        uint32_t alg = algorithm & 0xFF;
        
        // Map algorithm code to hybrid bitmask bits
        switch (alg) {
            case AUTH_HDR_TELLURIDE_RSA_4096:
                return (hybridMask & HYBRID_RSA4096) != 0;
            case AUTH_HDR_TELLURIDE_ECDSA_P384:
                return (hybridMask & HYBRID_ECDSA_P384) != 0;
            case AUTH_HDR_TELLURIDE_ECDSA_P521:
                return (hybridMask & HYBRID_ECDSA_P521) != 0;
            case AUTH_HDR_TELLURIDE_LMS:
                return (hybridMask & HYBRID_LMS) != 0;
            case AUTH_HDR_TELLURIDE_HSS_LMS:
                return (hybridMask & HYBRID_LMS_HSS) != 0;
            case AUTH_HDR_TELLURIDE_ML_DSA87:
                return (hybridMask & HYBRID_ML_DSA87) != 0;
            case AUTH_HDR_TELLURIDE_SLH_DSA:
                return (hybridMask & HYBRID_SLH_DSA_SHAKE_256S) != 0;
            default:
                return false;
        }
    }

    /**************************************************************************/
    Authentication::Type ConvertKmdAlgorithmToAuthType(uint32_t kmdAlgorithm)
    {
        uint32_t alg = kmdAlgorithm & 0xFF;
        
        // Map AUTH_HDR_TELLURIDE values to Authentication::Type enum
        switch (alg) {
            case AUTH_HDR_TELLURIDE_RSA_4096:
                return Authentication::RSA;
            case AUTH_HDR_TELLURIDE_ECDSA_P384:
                return Authentication::ECDSA;
            case AUTH_HDR_TELLURIDE_ECDSA_P521:
                return Authentication::ECDSAp521;
            case AUTH_HDR_TELLURIDE_HSS_LMS:
                return Authentication::HSS_SHAKE256;
            case AUTH_HDR_TELLURIDE_LMS:
                return Authentication::LMS_SHAKE256;
            case AUTH_HDR_TELLURIDE_ML_DSA87:
                return Authentication::MLDSA;
            case AUTH_HDR_TELLURIDE_SLH_DSA:
                return Authentication::SLH_SHAKE256;
            default:
                return Authentication::None;
        }
    }

    /**************************************************************************/
    /* Check if hybrid mask contains classical algorithms (RSA/ECDSA)       */
    /**************************************************************************/
    bool HybridMaskContainsClassical(uint32_t hybridMask)
    {
        return (hybridMask & (HYBRID_RSA4096 | HYBRID_ECDSA_P384 | HYBRID_ECDSA_P521)) != 0;
    }

    /**************************************************************************/
    /* Check if hybrid mask contains PQC algorithms (HSS/LMS/ML-DSA/SLH-DSA)*/
    /**************************************************************************/
    bool HybridMaskContainsPQC(uint32_t hybridMask)
    {
        return (hybridMask & (HYBRID_LMS_HSS | HYBRID_LMS | HYBRID_ML_DSA87 | HYBRID_SLH_DSA_SHAKE_256S)) != 0;
    }

    /**************************************************************************/
    /* Auto-detect (.kmd) keys and apply global->partition key inheritance.  */
    /**************************************************************************/
    void AutoDetectAndInheritKeys(std::vector<BifOptions*>& bifOptionList)
    {
        for (std::vector<BifOptions*>::iterator bifoptions = bifOptionList.begin();
             bifoptions != bifOptionList.end(); bifoptions++)
        {
            // Global-level auto-detection
            (*bifoptions)->AutoDetectAndPopulateKeys();

            // Partition-level auto-detection (with global key inheritance)
            for (std::list<PartitionBifOptions*>::iterator partitr = (*bifoptions)->partitionBifOptionList.begin();
                 partitr != (*bifoptions)->partitionBifOptionList.end(); partitr++)
            {
                // Check if partition has any local keys defined
                bool hasPartitionKeys = !(*partitr)->pkFile.empty() || !(*partitr)->skFile.empty() ||
                                       !(*partitr)->pkFile1.empty() || !(*partitr)->skFile1.empty();

                // Check if global has simplified keys
                bool hasGlobalKeys = !(*bifoptions)->GetPKFileName().empty() || !(*bifoptions)->GetSKFileName().empty() ||
                                    !(*bifoptions)->GetPKFileName1().empty() || !(*bifoptions)->GetSKFileName1().empty();

                if (!hasPartitionKeys && hasGlobalKeys) {
                    // Partition has NO keys, but global has keys.
                    // Use partition's authentication type to derive keys from global pkfile/skfile.
                    LOG_TRACE("Partition has no local keys - deriving from global keys using partition's authentication type");

                    // Inherit global keys first
                    if (!(*bifoptions)->GetPKFileName().empty()) {
                        (*partitr)->pkFile = (*bifoptions)->GetPKFileName();
                        LOG_TRACE("Partition inheriting global pkfile: %s", (*partitr)->pkFile.c_str());
                    }
                    if (!(*bifoptions)->GetSKFileName().empty()) {
                        (*partitr)->skFile = (*bifoptions)->GetSKFileName();
                        LOG_TRACE("Partition inheriting global skfile: %s", (*partitr)->skFile.c_str());
                    }
                    if (!(*bifoptions)->GetPKFileName1().empty()) {
                        (*partitr)->pkFile1 = (*bifoptions)->GetPKFileName1();
                        LOG_TRACE("Partition inheriting global pkfile1: %s", (*partitr)->pkFile1.c_str());
                    }
                    if (!(*bifoptions)->GetSKFileName1().empty()) {
                        (*partitr)->skFile1 = (*bifoptions)->GetSKFileName1();
                        LOG_TRACE("Partition inheriting global skfile1: %s", (*partitr)->skFile1.c_str());
                    }

                    // Now run partition auto-detection with partition's authentication type
                    (*partitr)->AutoDetectAndPopulateKeys();
                }
                else if (hasPartitionKeys) {
                    // Partition has its own keys - use partition-level detection directly.
                    LOG_TRACE("Partition has local keys - using partition-level auto-detection");
                    if (hasGlobalKeys) {
                        if ((*partitr)->pkFile.empty() && !(*bifoptions)->GetPKFileName().empty()) {
                            (*partitr)->pkFile = (*bifoptions)->GetPKFileName();
                            LOG_TRACE("Partition inheriting global pkfile: %s", (*partitr)->pkFile.c_str());
                        }
                        if ((*partitr)->skFile.empty() && !(*bifoptions)->GetSKFileName().empty()) {
                            (*partitr)->skFile = (*bifoptions)->GetSKFileName();
                            LOG_TRACE("Partition inheriting global skfile: %s", (*partitr)->skFile.c_str());
                        }
                        if ((*partitr)->pkFile1.empty() && !(*bifoptions)->GetPKFileName1().empty()) {
                            (*partitr)->pkFile1 = (*bifoptions)->GetPKFileName1();
                            LOG_TRACE("Partition inheriting global pkfile1: %s", (*partitr)->pkFile1.c_str());
                        }
                        if ((*partitr)->skFile1.empty() && !(*bifoptions)->GetSKFileName1().empty()) {
                            (*partitr)->skFile1 = (*bifoptions)->GetSKFileName1();
                            LOG_TRACE("Partition inheriting global skfile1: %s", (*partitr)->skFile1.c_str());
                        }
                    }
                    (*partitr)->AutoDetectAndPopulateKeys();
                }
                else {
                    // No partition keys and no global keys - nothing to detect
                    LOG_TRACE("No keys at partition or global level - skipping auto-detection");
                }
            }
        }
    }

} // namespace BifOptionsVersal2VP
