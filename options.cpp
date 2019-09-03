/******************************************************************************
* Copyright 2015-2019 Xilinx, Inc.
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
#include <iostream>
#include <sstream>
#include <string> 
#include "options.h"
#include "cmdoptionsscanner.h"
#include "logger.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
void Options::ParseArgs(int argc, const char * argv[])
{
    CO::FlexScanner scanner;
    CO::BisonParser parser(scanner,*this);
    std::string merged;

    for(int i=1;i<argc;i++)
    {
        if (i!=1) 
        {
            merged.append(" ");
        }
        merged.append(argv[i]);
    }
    
    /* No arguments, assume "-h" */
    if (argc == 1) 
    {
        merged.append("-h");
    }

    scanner.commandline = merged;
    std::stringstream s(merged);
    scanner.switch_streams(&s);

#if YYDEBUG && 0
    parser.set_debug_level( 1);
    parser.set_debug_stream(std::cout);
#endif

    int res = parser.parse();
    LOG_TRACE("Command Line parsing started");
    if(res)
    {
        LOG_ERROR("Command line parsing failed with code %d", res);
    }
    
    LOG_TRACE("Command: %s", merged.c_str());
    LOG_INFO("Command line parsing completed successfully");
}

/******************************************************************************/
void Options::SetBifFilename (std::string filename)
{
    std::ifstream f(filename.c_str());
    if (!f)
    {
        LOG_ERROR("Can't read file - %s", filename.c_str());
    }
    bifFileName = filename;
}

/******************************************************************************/
void Options::SetEncryptedKeySource (KeySource::Type type)
{
    if(archType == Arch::ZYNQ)
    {
        if( (type!=KeySource::BbramRedKey) && (type!=KeySource::EfuseRedKey) && (type!=KeySource::None) )
        {
            LOG_DEBUG(DEBUG_STAMP, "Key Source = %d", type);
            LOG_ERROR("BIF attribute error !!!\n\t\tUnknown option for 'keysrc_encryption' in BIF file\n\t\tZynq Architecture supports 'efuse' & 'bbram' key sources");
        }
    }
    cmdEncryptOptions->encryptedKeySource = type;
    LOG_TRACE("Setting Encryption Key Source as %d", type);
}

/******************************************************************************/
void Options::SetEncryptionKeyFile(std::string filename)
{
    cmdEncryptOptions->encryptionKeyFile = filename;
}

/******************************************************************************/
void Options::SetHmac(std::string val)
{
    cmdEncryptOptions->hmac = val;
}

/******************************************************************************/
void Options::SetKey0(std::string val)
{
    cmdEncryptOptions->key0 = val;
}

/******************************************************************************/
void Options::SetStartCbc(std::string val)
{
    cmdEncryptOptions->startCBC = val;
}

/******************************************************************************/
void Options::SetDoFill(bool flag)
{
    doFill = flag;
}

/******************************************************************************/
void Options::SetOutputFillByte(uint8_t byte)
{
    outputFillByte = byte;
}

/******************************************************************************/
void Options::SetSplitType(File::Type type)
{
    splitType = type;
}

/******************************************************************************/
void Options::SetProcessBitstreamType(File::Type type)
{
    processBitstream = type;
}

/******************************************************************************/
void Options::SetDualQspiMode(QspiMode::Type type)
{
    dualQspiMode = type;
}

/******************************************************************************/
void Options::SetQspiSize(uint16_t size)
{
    qspiSize = size;
}

/******************************************************************************/
void Options::SetBaseAddress(Binary::Address_t addr)
{
    baseAddress = addr;
}

/******************************************************************************/
void Options::SetDefaultAlignment(uint32_t val)
{
    defaultAlignment = val;
}

/******************************************************************************/
void Options::SetDevicePartName(std::string partname)
{
    devicePartName = partname;
}

/******************************************************************************/
void Options::SetKDFTestVectorFile(std::string filename)
{
    kdfTestVectorFile = filename;
}

/******************************************************************************/
void Options::SetDevicePackageName(std::string packageName)
{
    devicePackageName = packageName;
}

/******************************************************************************/
void Options::SetEfuseHashFileName(std::string filename)
{
    efuseHashFileName = filename;
}

/******************************************************************************/
void Options::SetSpkSigFileName(std::string filename)
{
    spkSignatureFileName = filename;
}

/******************************************************************************/
void Options::InsertOutputFileNames(std::string filename)
{
    outputFileNames.push_back(filename);
}

/******************************************************************************/
void Options::SetLogLevel(LogLevel::Type level)
{
    LOG_LEVEL(level);
}

/******************************************************************************/
void Options::SetArchType(Arch::Type type)
{
    archType = type;
}

/******************************************************************************/
void Options::SetGenerateHashes(bool flag)
{
    generateHashes = flag;
}

/******************************************************************************/
void Options::SetNoAuthBlocksFlag(bool flag)
{
    noauthblocks = flag;
}

/******************************************************************************/
void Options::SetOverwrite(bool flag)
{
    overwriteMode = flag;
}

/******************************************************************************/
void Options::SetLegacyFlag(bool flag)
{
    legacy = flag;
}

/******************************************************************************/
void Options::SetPadHeaderTable(bool flag)
{
    padImageHeaderTable = flag;
}

/******************************************************************************/
void Options::SetNonBootingFlag(bool flag)
{
    nonBooting = flag;
}

/******************************************************************************/
void Options::SetOutputMode(OutputMode::Type mode, File::Type fmt)
{
    outputMode = mode;
    outputFmt = fmt;
}

/******************************************************************************/
void Options::SetDumpOption(DumpOption::Type type)
{
    dumpOption = type;
}

/******************************************************************************/
void Options::SetVerifyImageOption(bool)
{
    verifyImage = true;
}

/******************************************************************************/
void Options::SetReadImageOption(ReadImageOption::Type type)
{
    readImageOption = type;
}

/******************************************************************************/
void Options::SetReadImageFile(std::string file)
{
    readImageFile = file;
}

/******************************************************************************/
OutputMode::Type  Options::GetOutputMode(void)
{
    return outputMode;
}

/******************************************************************************/
File::Type Options::GetOutputFormat(void)
{
    return outputFmt;
}

/******************************************************************************/
std::string Options::GetReadImageFile(void)
{
    return readImageFile;
}

/******************************************************************************/
ReadImageOption::Type Options::GetReadImageOption(void)
{
    return readImageOption;
}

/******************************************************************************/
bool Options::GetVerifyImageOption()
{
    return verifyImage;
}

/******************************************************************************/
std::string Options::GetBifFilename(void)
{
    return bifFileName;
}

//check-R chnage to always access key src from bif options
/******************************************************************************/
KeySource::Type Options::GetEncryptedKeySource(void)
{
    return cmdEncryptOptions->encryptedKeySource;
}

/******************************************************************************/
std::string Options::GetEncryptionKeyFile(void)
{
    return cmdEncryptOptions->encryptionKeyFile;
}

/******************************************************************************/
std::string Options::GetHmac(void)
{
    return cmdEncryptOptions->hmac;
}

/******************************************************************************/
std::string Options::GetKey0(void)
{
    return cmdEncryptOptions->key0;
}

/******************************************************************************/
std::string Options::GetStartCbc(void)
{
    return cmdEncryptOptions->startCBC;
}

/******************************************************************************/
std::string Options::GetFsblFilename(void)
{
    return fsblFilename;
}

/******************************************************************************/
bool Options::GetDoFill(void)
{
    return doFill;
}

/******************************************************************************/
uint8_t Options::GetOutputFillByte(void)
{
    return outputFillByte;
}

/******************************************************************************/
File::Type Options::GetSplitType(void)
{
    return splitType;
}

/******************************************************************************/
File::Type Options::GetProcessBitstreamType(void)
{
    return processBitstream;
}

/******************************************************************************/
QspiMode::Type Options::GetDualQspiMode(void)
{
    return dualQspiMode;
}

/******************************************************************************/
uint16_t Options::GetQspiSize(void)
{
    return qspiSize;
}

/******************************************************************************/
Binary::Address_t Options::GetBaseAddress(void)
{
    return baseAddress;
}

/******************************************************************************/
uint32_t Options::GetDefaultAlignment(void)
{
    return defaultAlignment;
}

/******************************************************************************/
std::string Options::GetDevicePartName(void)
{
    return devicePartName;
}

/******************************************************************************/
std::string Options::GetKDFTestVectorFile(void)
{
    return kdfTestVectorFile;
}

/******************************************************************************/
std::string Options::GetEfuseHashFileName(void)
{
    return efuseHashFileName;
}

/******************************************************************************/
std::string Options::GetDevicePackageName(void)
{
    return devicePackageName;
}

/******************************************************************************/
std::string Options::GetSpkSigFileName(void)
{
    return spkSignatureFileName;
}

/******************************************************************************/
std::list<std::string>& Options::GetOutputFileNames(void)
{
    return outputFileNames;
}

/******************************************************************************/
LogLevel::Type Options::GetLogLevel(void)
{
    return logLevel;
}

/******************************************************************************/
Arch::Type Options::GetArchType(void)
{
    return archType;
}

/******************************************************************************/
bool Options::DoGenerateHashes(void)
{
    return generateHashes;
}

/******************************************************************************/
bool Options::GetNoAuthBlocksFlag(void)
{
    return noauthblocks;
}

/******************************************************************************/
bool Options::DoOverwrite(void)
{
    return overwriteMode;
}

/******************************************************************************/
bool Options::GetLegacyFlag(void)
{
    return legacy;
}

/******************************************************************************/
bool Options::DoPadHeaderTable(void)
{
    return padImageHeaderTable;
}

/******************************************************************************/
bool Options::GetNonBootingFlag(void)
{
    return nonBooting;
}

/******************************************************************************/
void Options::SetAuthKeyGeneration(GenAuthKeys::Type type)
{
    genKey = type;
}

/******************************************************************************/
bool Options::GetGreyKeyGeneration(void)
{
    return generateGreyKey;
}

/******************************************************************************/
bool Options::GetMetalKeyGeneration(void)
{
    return generateMetalKey;
}

/******************************************************************************/
void Options::SetGreyKeyGeneration(bool flag)
{
    generateGreyKey = flag;
}

/******************************************************************************/
void Options::SetMetalKeyGeneration(bool flag)
{
    generateMetalKey = flag;
}

/******************************************************************************/
GenAuthKeys::Type Options::GetAuthKeyGeneration(void)
{
    return genKey;
}

/******************************************************************************/
void Options::SetZynqMpEncrDump(bool type)
{
    std::string filename = "aes_log.txt";
    zynqmpEncrDump = type;
    std::ofstream exists(filename);
    if (exists)
    {
        std::ofstream remove(filename);
    }
    aesLogFile.open(filename, std::fstream::app);
}

/******************************************************************************/
void Options::SetZynqmpes1Flag(bool type)
{
    zynqmpes1 = type;
}

/******************************************************************************/
bool Options::GetZynqMpEncrDump(void)
{
    return zynqmpEncrDump;
}

/******************************************************************************/
bool Options::GetZynqmpes1Flag(void)
{
    if (archType != Arch::ZYNQMP && zynqmpes1)
    {
        LOG_ERROR("'-zynqmpes1' option supported only for ZynqMp architecture, '-arch zynqmp'");
    }
    return zynqmpes1;
}

/******************************************************************************/
DumpOption::Type Options::GetDumpOption(void)
{
    return dumpOption;
}
