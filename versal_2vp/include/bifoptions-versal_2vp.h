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

#ifndef _BIFOPTIONS_VERSAL_2VP_H_
#define _BIFOPTIONS_VERSAL_2VP_H_

/*
-------------------------------------------------------------------------------
************************************************** HEADER FILES AND DEPENDENCIES
-------------------------------------------------------------------------------
*/
#include <string>
#include <vector>
#include <stdint.h>
#include "authentication.h"  // For Authentication::Type enum

/* Forward declaration to avoid pulling common bifoptions.h into this header. */
class BifOptions;

/*
-------------------------------------------------------------------------------
************************************************** DEVICE-SPECIFIC KMD CONVERSION
-------------------------------------------------------------------------------
This header provides Versal 2VP (versal_2vp) device-specific conversion functions
for .kmd metadata file parsing. These functions map algorithm strings and
values to device-specific authentication header constants.

Architecture Isolation:
  - All AUTH_HDR_TELLURIDE_* macro dependencies are isolated here
  - All HYBRID_* macro dependencies are isolated here
  - Common code (bifoptions.cpp) calls these via function pointers or factory

Future Device Support:
  - Create similar headers: bifoptions-versal_3vp.h, bifoptions-spartanup.h
  - Implement same function signatures with device-specific constants
  - Register device-specific functions at runtime based on -arch flag
*/

namespace BifOptionsVersal2VP
{
    /**************************************************************************/
    /* Convert algorithm string to device-specific authentication header value*/
    /* Input:  Algorithm string (e.g., "rsa", "ecdsa-p384", "lms-shake256")  */
    /* Output: AUTH_HDR_TELLURIDE_* value for Versal 2VP ppkheader.Algorithm*/
    /**************************************************************************/
    uint32_t ConvertAlgorithmStringToHeaderValue(const std::string& algStr);

    /**************************************************************************/
    /* Convert hybrid algorithm string to device-specific hybrid bitmask      */
    /* Input:  Hybrid specification (e.g., "ecdsa-p384,lms-shake256")        */
    /* Output: HYBRID_* bitmask for Versal 2VP ppkheader.Hybrid field       */
    /**************************************************************************/
    uint32_t ConvertHybridStringToBitmask(const std::string& hybridStr);

    /**************************************************************************/
    /* Check if algorithm is classical (non-PQC) for Versal 2VP             */
    /* Input:  Algorithm value (AUTH_HDR_TELLURIDE_* constant)              */
    /* Output: true if RSA/ECDSA, false otherwise                           */
    /**************************************************************************/
    bool IsClassicalAlgorithm(uint32_t algorithm);

    /**************************************************************************/
    /* Check if algorithm is Post-Quantum (PQC) for Versal 2VP             */
    /* Input:  Algorithm value (AUTH_HDR_TELLURIDE_* constant)              */
    /* Output: true if LMS/HSS/ML-DSA/SLH-DSA, false otherwise             */
    /**************************************************************************/
    bool IsPQCAlgorithm(uint32_t algorithm);

    /**************************************************************************/
    /* Get human-readable algorithm name for Versal 2VP                     */
    /* Input:  Algorithm value (AUTH_HDR_TELLURIDE_* constant)              */
    /* Output: Algorithm name string (e.g., "RSA-4096", "ECDSA-P384")       */
    /**************************************************************************/
    std::string GetAlgorithmName(uint32_t algorithm);

    /**************************************************************************/
    /* Check if algorithm is present in hybrid bitmask for Versal 2VP       */
    /* Input:  Algorithm value and hybrid bitmask                            */
    /* Output: true if algorithm bit is set in hybrid bitmask                */
    /**************************************************************************/
    bool IsAlgorithmInHybridMask(uint32_t algorithm, uint32_t hybridMask);

    /**************************************************************************/
    /* Convert .kmd algorithm code to Authentication::Type enum             */
    /* Input:  Algorithm value from .kmd (AUTH_HDR_TELLURIDE_* constant)    */
    /* Output: Authentication::Type enum value                               */
    /**************************************************************************/
    Authentication::Type ConvertKmdAlgorithmToAuthType(uint32_t kmdAlgorithm);

    /**************************************************************************/
    /* Check if hybrid mask contains classical algorithms for Versal 2VP    */
    /* Input:  Hybrid bitmask value                                          */
    /* Output: true if mask contains any RSA/ECDSA bits, false otherwise    */
    /**************************************************************************/
    bool HybridMaskContainsClassical(uint32_t hybridMask);

    /**************************************************************************/
    /* Check if hybrid mask contains PQC algorithms for Versal 2VP          */
    /* Input:  Hybrid bitmask value                                          */
    /* Output: true if mask contains any LMS/HSS/ML-DSA/SLH-DSA bits        */
    /**************************************************************************/
    bool HybridMaskContainsPQC(uint32_t hybridMask);

    /**************************************************************************/
    /* Auto-detect (.kmd) keys and apply global->partition key inheritance   */
    /* Input:  List of BifOptions (main or included BIF)                     */
    /* Output: None; keys are resolved/inherited in place for multi-SPK      */
    /**************************************************************************/
    void AutoDetectAndInheritKeys(std::vector<BifOptions*>& bifOptionList);
}

#endif // _BIFOPTIONS_VERSAL_2VP_H_
