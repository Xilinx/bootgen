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

#pragma once

#ifndef _OPTIONS_H_
#define _OPTIONS_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <vector>
#include <string>

#include "outputfile.h"
#include "logger.h"
#include "encryption.h"
#include "baseclass.h"

/* Forward Class References */
class BifOptions;



/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
struct Arch
{
    typedef enum 
    {
        ZYNQ,
        ZYNQMP,
        FPGA,
        VERSAL,
    } Type;
};

typedef struct
{
    KeySource::Type encryptedKeySource;
    std::string encryptionKeyFile;
    std::string hmac;
    std::string key0;
    std::string startCBC;
} CommndLineEncryptOptions;


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class Options
{
public:
    Options()
        : bifFileName("bootimage.bif")
        , doFill(false)
        , outputFillByte(0xFF)
        , splitType(File::Unknown)
        , baseAddress(0x00000000)
        , defaultAlignment(64)
        , generateHashes(false)
        , noauthblocks(false)
        , overwriteMode(false)
        , debugstr(new std::ostream(0))
        , legacy(false)
        , padImageHeaderTable(true)
        , dualQspiMode(QspiMode::Unknown)
        , nonBooting(false)
        , processBitstream(File::Unknown)
        , devicePackageName("")
        , archType(Arch::ZYNQ)
        , genKey(GenAuthKeys::None)
        , encryptionDump(false)
        , zynqmpes1(false)
        , generateGreyKey(false)
        , generateMetalKey(false)
        , outputMode(OutputMode::OUT_NORMAL)
        , totalHeadersSize(0)
        , bootheaderSize(0)
        , allHeaderSize(0)
        , bootloaderSize(0)
        , qspiSize(0)
        , logLevel(LogLevel::WARNING)
        , outputFmt(File::Unknown)
        , readImageOption(ReadImageOption::ALL)
        , readImageFile("")
        , verifyImage(false)
        , bifOptions(NULL)
        , secHdrIv(NULL)
        , secHdrIvPmcData(NULL)
        , secureDebugAuthType(Authentication::None)
        , secureDebugImageFilename("")
        , dumpOption(DumpOption::NONE)
        , dumpPath("")
        , deviceKeyStored(false)
    {
        cmdEncryptOptions = new CommndLineEncryptOptions();
    };
    
    ~Options()
    {
        if (debugstr != &std::cout)
        {
            delete debugstr;
        }
        if (cmdEncryptOptions)
        {
            delete cmdEncryptOptions;
        }
        if (secHdrIv)
        {
            delete secHdrIv;
        }
    }

    void ParseArgs(int argc,const char* argv[]);
    std::ostream* debugstr;
    void ProcessVerifyKDF(void);
    void ProcessReadImage(void);

    void SetBifFilename (std::string);
    void SetEncryptedKeySource (KeySource::Type);
    void SetEncryptionKeyFile (std::string);
    void SetHmac (std::string);
    void SetKey0 (std::string);
    void SetStartCbc (std::string);
    void SetOutputByteWidth (uint8_t);
    void SetDoFill (bool);
    void SetOutputFillByte (uint8_t);
    void SetSplitType (File::Type);
    void SetProcessBitstreamType (File::Type);
    void SetDualQspiMode (QspiMode::Type);
    void SetQspiSize (uint16_t);
    void SetOspiSize(uint16_t size);
    void SetBaseAddress (Binary::Address_t);
    void SetDefaultAlignment (uint32_t);
    void SetDevicePartName (std::string);
    void SetKDFTestVectorFile(std::string);
    void SetDevicePackageName (std::string);
    void SetEfuseHashFileName (std::string);
    void SetSpkSigFileName (std::string);
    void InsertOutputFileNames (std::string);
    void SetLogLevel (LogLevel::Type);
    void SetArchType (Arch::Type);
    void SetGenerateHashes (bool);
    void SetNoAuthBlocksFlag (bool);
    void SetOverwrite (bool);
    void SetLegacyFlag (bool);
    void SetPadHeaderTable (bool);
    void SetNonBootingFlag (bool);
    void SetAuthKeyGeneration (GenAuthKeys::Type);
    void SetGreyKeyGeneration (bool flag);
    void SetMetalKeyGeneration (bool flag);
    void SetEncryptionDump(bool, std::string filename);
    void CloseEncryptionDumpFile(void);
    void SetZynqmpes1Flag (bool);
    void SetOutputMode (OutputMode::Type, File::Type);
    void SetDumpOption(DumpOption::Type);
    void SetDumpDirectory(std::string);
    void SetVerifyImageOption(bool);
    void SetReadImageOption(ReadImageOption::Type);
    void SetReadImageFile(std::string);
    void SetSecureDebugAuthType(Authentication::Type type);
    void SetSecureDebugImageFile(std::string);
    void SetOverlayCDOFileName (std::string);
    
    std::string GetBifFilename (void);
    KeySource::Type GetEncryptedKeySource (void);
    std::string GetEncryptionKeyFile (void);
    std::string GetHmac (void);
    std::string GetKey0 (void);
    std::string GetStartCbc (void);
    std::string GetFsblFilename (void);
    uint8_t GetOutputByteWidth (void);
    bool GetDoFill (void);
    uint8_t GetOutputFillByte (void);
    File::Type GetSplitType (void);
    File::Type GetProcessBitstreamType (void);
    QspiMode::Type GetDualQspiMode (void);
    uint16_t GetQspiSize (void);
    Binary::Address_t GetBaseAddress (void);
    uint32_t GetDefaultAlignment (void);
    std::string GetDevicePartName (void);
    std::string GetKDFTestVectorFile(void);
    std::string GetDevicePackageName (void);
    std::string GetEfuseHashFileName (void);
    std::string GetSpkSigFileName (void);
    std::list<std::string>& GetOutputFileNames (void);
    LogLevel::Type GetLogLevel (void);
    Arch::Type GetArchType (void);
    bool DoGenerateHashes (void);
    bool GetNoAuthBlocksFlag (void);
    bool DoOverwrite (void);
    bool GetLegacyFlag (void);
    bool DoPadHeaderTable (void);
    bool GetNonBootingFlag (void);
    GenAuthKeys::Type GetAuthKeyGeneration (void);
    bool GetGreyKeyGeneration (void);
    bool GetMetalKeyGeneration (void);
    bool GetEncryptionDumpFlag (void);
    bool GetZynqmpes1Flag (void);
    OutputMode::Type GetOutputMode (void);
    File::Type GetOutputFormat (void);
    std::string GetReadImageFile(void);
    ReadImageOption::Type GetReadImageOption(void);
    bool GetVerifyImageOption();
    DumpOption::Type GetDumpOption(void);
    std::string GetDumpDirectory(void);
    Authentication::Type GetSecureDebugAuthType(void);
    std::string GetSecureDebugImageFile(void);
    std::string GetOverlayCDOFileName (void);

    uint32_t totalHeadersSize;
    uint32_t bootheaderSize;
    uint32_t allHeaderSize;
    uint32_t bootloaderSize;
    uint8_t *secHdrIv;
    uint8_t *secHdrIvPmcData;

//private:
    std::string bifFileName;
    std::string devicePartName;
    std::string efuseHashFileName;
    std::string spkSignatureFileName;
    std::string devicePackageName;
    std::string kdfTestVectorFile;
    std::list<std::string> outputFileNames;
    LogLevel::Type logLevel;
    QspiMode::Type dualQspiMode;
    GenAuthKeys::Type genKey;
    Arch::Type archType;
    OutputMode::Type outputMode;
    File::Type outputFmt;
    File::Type splitType;
    File::Type processBitstream;
    std::string readImageFile;
    ReadImageOption::Type readImageOption;
    bool verifyImage;
    DumpOption::Type dumpOption;
    std::string dumpPath;
    bool overwriteMode;
    bool doFill;
    bool legacy;
    bool padImageHeaderTable;
    bool generateHashes;
    bool nonBooting;
    bool encryptionDump;
    bool zynqmpes1;
    bool noauthblocks;
    bool generateGreyKey;
    bool generateMetalKey;
    bool deviceKeyStored;
    std::ofstream aesLogFile;
    std::string aesLogFilename;
    uint8_t outputFillByte;
    uint16_t qspiSize;
    uint32_t defaultAlignment;
    Binary::Address_t baseAddress;
    CommndLineEncryptOptions *cmdEncryptOptions;
    BifOptions* bifOptions;
    std::vector<BifOptions*> bifOptionsList;
    std::string fsblFilename;
    Authentication::Type secureDebugAuthType;
    std::string secureDebugImageFilename;
    std::string overlayCDOFile;
};

#endif
