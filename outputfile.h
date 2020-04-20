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

#pragma once

#ifndef _OUTPUTFILE_H_
#define _OUTPUTFILE_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <string>
#include <fstream>
#include <stdint.h>

#include "binary.h"
#include "bootgenenum.h"
#include "logger.h"

/* Forward Class References */
class Options;
class BootImage;
class OutputStream;


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define NO_SPLIT_SLAVE_FILES 3


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class OutputFile 
{
public:
    OutputFile()
        : totalByteOutputCount(0)
        , fileIndex(0)
        , qspiDualMode(QspiMode::Unknown)
        , qspiSizeInBytes(0)
        , totalImageSize(0)
        , encrypted(false)
        , splitOffset(0)
        , mode(OutputMode::OUT_NORMAL)
    {
    }

    virtual ~OutputFile() {}

    static File::Type  GetType(const std::string& filename);
    static OutputFile* Factory(const std::string& filename);

    void Output(Options& options, Binary& binary);
    void OutputVersal(Options& options, Binary& binary);
    void CreateOutputFiles(Options& options);
    void ProcessSplitMode(Section& section, Options& options);
    void ProcessSplitSlaveBootMode(Section& section, Options& options);
    void ProcessBitstreamMode(Section& section);
    void WritePostscriptAndClose(void);

    void OutputInterface(Options & options, uint8_t*, uint32_t);
    void WriteBootHeaderToFile(std::string fileName, uint8_t* data, uint64_t size);

    std::string filename;
    QspiMode::Type qspiDualMode;
    uint32_t qspiSizeInBytes;
    uint32_t totalImageSize;
    bool encrypted;
    uint32_t splitOffset;
    uint8_t fileIndex;

private:
    std::string bootFile;
    std::string headersFile;
    std::string partitionFile;
    OutputMode::Type mode;

protected:
    Binary::Length_t totalByteOutputCount;

    virtual void Open() = 0;
    virtual void Append() = 0;
    virtual void WritePreamble() = 0;
    virtual void Write(Binary::Address_t start, Binary::Length_t length, const uint8_t* buffer) = 0;
    virtual void Fill(Binary::Address_t start, Binary::Address_t end, bool doFill, uint8_t fillByte) = 0;
    virtual void WritePostscript() = 0;
    virtual void Close() = 0;
protected:
    std::ofstream ofs;
    std::ofstream ofs1;
};

/******************************************************************************/
class BinFile : public OutputFile
{
public:
    BinFile() {}
    virtual ~BinFile() {}

    virtual void Open() ;
    virtual void Append();
    virtual void WritePreamble();
    virtual void Write(Binary::Address_t start, Binary::Length_t length, const uint8_t* buffer);
    virtual void Fill(Binary::Address_t start, Binary::Address_t end, bool doFill, uint8_t fillByte);
    virtual void WritePostscript();
    virtual void Close() ;
};

/******************************************************************************/
class McsFile : public OutputFile 
{
public:
    McsFile();
    virtual ~McsFile() {}

    virtual void Open() ;
    virtual void Append();
    virtual void WritePreamble();
    virtual void Write(Binary::Address_t start, Binary::Length_t length, const uint8_t* buffer);
    virtual void Fill(Binary::Address_t start, Binary::Address_t end, bool doFill, uint8_t fillByte);
    virtual void WritePostscript();
    virtual void Close() ;
private:
    void Write(uint32_t address, uint8_t command, const uint8_t* data, int len);
    Binary::Address_t pageAddress;
    Binary::Address_t filePageAddr[NO_SPLIT_SLAVE_FILES];
    uint32_t lineLength;
    uint32_t shiftLength;
    uint32_t maskByte;
    bool newFile;
    bool eofrWritten;
};

/******************************************************************************/
class PartitionOutput
{
public:
    PartitionOutput() {}
    virtual ~PartitionOutput() {}

    virtual void GeneratePartitionFiles(BootImage& bi, Binary& cache) = 0;
    virtual PartitionArch::Type Type(void) = 0;
};

/******************************************************************************/
class ZynqPartitionOutput : public PartitionOutput
{
public:
    ZynqPartitionOutput() {}
    ~ZynqPartitionOutput() {}

    void GeneratePartitionFiles(BootImage& bi, Binary& cache) { };
    
    PartitionArch::Type Type(void)
    {
        return PartitionArch::NON_FPGA;
    }
};

/******************************************************************************/
class ZynqMpPartitionOutput : public PartitionOutput
{
public:
    ZynqMpPartitionOutput() {}
    ~ZynqMpPartitionOutput() {}

    void GeneratePartitionFiles(BootImage& bi, Binary& cache) { };
    
    PartitionArch::Type Type(void)
    {
        return PartitionArch::NON_FPGA;
    }
};

/******************************************************************************/
class FpgaPartitionOutput : public PartitionOutput
{
public:
    FpgaPartitionOutput() {}
    ~FpgaPartitionOutput() {}

    void GeneratePartitionFiles(BootImage& bi, Binary& cache);

    PartitionArch::Type Type(void)
    {
        return PartitionArch::FPGA;
    }
};

/******************************************************************************/
class VersalPartitionOutput : public PartitionOutput
{
public:
    void GeneratePartitionFiles(BootImage& bi, Binary& cache) { };

    PartitionArch::Type Type(void)
    {
        return PartitionArch::VERSAL;
    }
};

#endif
