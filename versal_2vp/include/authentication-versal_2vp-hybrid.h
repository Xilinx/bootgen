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
#pragma once

#ifndef _VERSAL_2VP_HYBRID_AUTHENTICATION_CONTEXT_H_
#define _VERSAL_2VP_HYBRID_AUTHENTICATION_CONTEXT_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "authentication-versal_2vp.h"
#include <vector>
#include <list>

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/

/* Forward class references */
class ImageHeader;
class BootImage;
class Binary;
class Section;
class AuthenticationCertificate;

/*
-------------------------------------------------------------------------------
***************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
class Versal_2vpHybridAuthenticationContext : public Versal_2vpAuthenticationContext
{
public:
    Versal_2vpHybridAuthenticationContext(std::vector<Authentication::Type> algorithms);
    Versal_2vpHybridAuthenticationContext(const AuthenticationContext* refAuthContext, 
                                         std::vector<Authentication::Type> algorithms);
    ~Versal_2vpHybridAuthenticationContext();

    // Override core methods to generate dual signatures for hashblock0
    void Link(BootImage& bi, std::list<Section*> sections, AuthenticationCertificate* cert) override;
    virtual Section* CreateCertificate(BootImage& bi, Binary& cache, Section* dataSection, bool isBootloader) override;
    
    // Hybrid-specific methods
    bool IsHybrid() const { return hybridAlgorithms.size() > 1; }
    std::vector<Authentication::Type> GetHybridAlgorithms() const { return hybridAlgorithms; }
    uint32_t GetHybridMask(Authentication::Type algType);
    const char* GetAlgorithmName(Authentication::Type algType);

    // Algorithm 2 key file accessors
    void SetPPKFile1(const std::string& file) { ppkFile1Path = file; }
    void SetPSKFile1(const std::string& file) { pskFile1Path = file; }
    void SetSPKFile1(const std::string& file) { spkFile1Path = file; }
    void SetSSKFile1(const std::string& file) { sskFile1Path = file; }
    std::string GetPPKFile1() const { return ppkFile1Path; }
    std::string GetPSKFile1() const { return pskFile1Path; }
    std::string GetSPKFile1() const { return spkFile1Path; }
    std::string GetSSKFile1() const { return sskFile1Path; }
    void SetLmsOnlySecondAlg(bool value) { lmsOnlySecondAlg = value; }
    bool GetLmsOnlySecondAlg() const { return lmsOnlySecondAlg; }

private:
    std::vector<Authentication::Type> hybridAlgorithms;
    
    // Deferred execution members for true isolation
    bool deferredExecution;
    Section* deferredHashBlockSection;
    uint8_t* deferredSignaturePtr;
    uint16_t deferredSigLength;
    Authentication::Type deferredAlgType;
    BootImage* deferredBi;
    
    // Isolated key storage for memory cleanup
    Key* isolatedPrimaryKey;
    Key* isolatedSecondaryKey;
    uint32_t hybridMask;
    
    // Key file paths for second algorithm
    std::string pskFile1Path;
    std::string sskFile1Path;
    std::string ppkFile1Path;
    std::string spkFile1Path;
    
    // LMS/HSS mode flags for hybrid mask calculation
    bool lmsOnlyFirstAlg;   // For first algorithm if it's LMS/HSS
    bool lmsOnlySecondAlg;  // For second algorithm if it's LMS/HSS
    
    // Hash context tracking for safe cleanup
    Hash* savedOriginalBootImageHash;
    
    // Helper methods
    void GenerateSecondSignature(BootImage& bi, std::list<Section*> sections, uint8_t* signatureBlock, uint16_t sigLength, Authentication::Type algType);
    void GenerateHashblock0SignatureForSecondAlgorithm(BootImage& bi, Section* hashBlockSection, uint8_t* signatureBlock, uint16_t sigLength, Authentication::Type algType);
    void LoadPublicKeyForAlgorithm(Key* targetKey, const std::string& keyFile, Authentication::Type algType, const std::string& keyName);
    void EnsureMLDSAPublicKeyForHybrid(Key* targetKey, const std::string& secretKeyFile, const std::string& keyName);
};

#endif // _VERSAL_2VP_HYBRID_AUTHENTICATION_CONTEXT_H_

