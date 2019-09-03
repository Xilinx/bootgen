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

#ifndef _LOGGER_H_
#define _LOGGER_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <cstdarg>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "stringutils.h"
#include "bootgenenum.h"
#include "bootgenexception.h"
#include "binary.h"


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define DEBUG_STAMP     __FILE__,__LINE__
#define LOG_LEVEL(a)    Logger::Instance()->SetLogLevel(a)
#define LOG_ERROR       Logger::Instance()->Error
#define LOG_WARNING     Logger::Instance()->Warning
#define LOG_INFO        Logger::Instance()->Info
#define LOG_DEBUG       Logger::Instance()->Debug
#define LOG_TRACE       Logger::Instance()->Trace
#define LOG_DUMP_IMAGE  Logger::Instance()->DumpBinaryImage
#define LOG_DUMP_BYTES  Logger::Instance()->DumpBytes
#define LOG_OUT         Logger::Instance()->Out
#define LOG_MSG         Logger::Instance()->Message
#define GET_LOG_LEVEL   Logger::Instance()->GetLogLevel

struct LogLevel
{
    typedef enum
    {
        ERROR,
        WARNING,
        INFO,
        DEBUG,
        TRACE,
    } Type;
};


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class Logger
{
public:	
    static Logger* Instance(void);
    void Error(const char* format, ...);
    void Warning(const char* format, ...);
    void Info(const char* format, ...);
    void Debug(std::string filename, int line, const char* format, ...);
    void Trace(const char* format, ...);
    void DumpBinaryImage(Binary &cache);
    void DumpBytes(uint8_t* ptr, int count);
    void Out(std::string stringout);
    void Message(const char* format, ...);
    void SetLogLevel(LogLevel::Type level);
    LogLevel::Type GetLogLevel(void);

private:
    Logger() 
    {
        log_level = LogLevel::WARNING; 
    }
    ~Logger() { };
    std::string msg;
    std::ofstream outFile;
    LogLevel::Type log_level;
    static Logger* logger;
};

#endif
