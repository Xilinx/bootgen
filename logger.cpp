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
#include "logger.h"

Logger* Logger::logger = NULL;


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
Logger* Logger::Instance(void)
{
    if(logger == NULL)
    {
        logger = new Logger();
    }
    return logger;
}

/******************************************************************************/
void Logger::SetLogLevel(LogLevel::Type level)
{
    log_level = level;
    if(!outFile.is_open())
    {
        outFile.open("bootgen_log.txt");
    }
}

/******************************************************************************/
LogLevel::Type Logger::GetLogLevel(void)
{
    return log_level;
}

/******************************************************************************/
void Logger::Error(const char* format, ...)
{
    if(log_level < LogLevel::ERROR)
    {
        return;
    }
    va_list ap;
    va_start(ap, format);
    msg = StringUtils::vFormat(format,ap);
    va_end(ap);

    Logger::outFile << "[ERROR]  : " << msg.c_str() << std::endl;
    throw BootGenException(msg.c_str());
}

/******************************************************************************/
void Logger::Warning(const char* format, ...)
{
    if(log_level < LogLevel::WARNING)
    {
        return;
    }
    va_list ap;
    va_start(ap, format);
    msg = StringUtils::vFormat(format,ap);
    va_end(ap);

    std::cout << "[WARNING]: " << msg.c_str() << std::endl;
    Logger::outFile << "[WARNING]: " << msg.c_str() << std::endl;
}

/******************************************************************************/
void Logger::Info(const char* format, ...)
{
    if(log_level < LogLevel::INFO)
    {
        return;
    }
    va_list ap;
    va_start(ap, format);
    msg = StringUtils::vFormat(format,ap);
    va_end(ap);

    std::cout << "[INFO]   : " << msg.c_str() << std::endl;
    Logger::outFile << "[INFO]   : " << msg.c_str() << std::endl;
}

/******************************************************************************/
void Logger::Debug(std::string filename, int line, const char* format, ...)
{
    if(log_level < LogLevel::DEBUG)
    {
        return;
    };
    va_list ap;
    va_start(ap, format);
    msg = StringUtils::vFormat(format,ap);
    va_end(ap);

    std::cout << "[DEBUG]  : " << msg.c_str() << std::endl;
    std::cout << "           " << "FILE: " << StringUtils::BaseName(filename).c_str() << std::endl;
    std::cout << "           " << "LINE: " << line << std::endl;
    
    Logger::outFile << "[DEBUG]  : " << msg.c_str() << std::endl;
    Logger::outFile << "           " << "FILE: " << StringUtils::BaseName(filename).c_str() << std::endl;
    Logger::outFile << "           " << "LINE: " << line << std::endl;
}

/******************************************************************************/
void Logger::Trace(const char* format, ...)
{
    if(log_level < LogLevel::TRACE)
    {
        return;
    }
    va_list ap;
    va_start(ap, format);
    msg = StringUtils::vFormat(format,ap);
    va_end(ap);

    std::cout << "[TRACE]  : " << msg.c_str() << std::endl;
    Logger::outFile << "[TRACE]  : " << msg.c_str() << std::endl;
}

/******************************************************************************/
void Logger::DumpBinaryImage(Binary &cache) 
{
    if(log_level < LogLevel::INFO)
    {
        return;
    }
    std::cout << "           -- Dump of Binary Image ----\n";
    Logger::outFile << "           -- Dump of Binary Image ----\n";
    std::cout << "   Offset" << "      Length" << "     Reserve"<< "     Name\n";
    Logger::outFile << "   Offset" << "     Length" << "      Reserve"<< "     Name\n";
   
    for(SectionList::iterator i=cache.Sections.begin(); i != cache.Sections.end(); i++)
    {
        Section& sec(**i);
        std::cout << "   " << std::hex << std::setfill('0') << std::setw(8) << sec.Address 
            << "   " << std::hex << std::setfill('0') << std::setw(8) << sec.Length 
            << "   " << std::hex << std::setfill('0') << std::setw(8) << sec.Reserve
            << "   " << sec.Name << "" << std::endl;
        Logger::outFile  << " " << std::hex << std::setfill('0') << std::setw(8) << sec.Address 
            << "   " << std::hex << std::setfill('0') << std::setw(8) << sec.Length 
            << "   " << std::hex << std::setfill('0') << std::setw(8) << sec.Reserve
            << "   " << sec.Name << "" << std::endl;
    }
    std::cout << "           -- End of Dump" << std::endl;
    Logger::outFile << "           -- End of Dump" << std::endl;
}

/******************************************************************************/
void Logger::DumpBytes(uint8_t* ptr, int count) 
{
    if(log_level < LogLevel::TRACE)
    {
        return;
    }
    std::cout << "           ";
    Logger::outFile << "           ";

    for(int i=0;i<count;i++)
    {
            std::cout << std::setfill('0') << std::hex << std::setw(2) << (uint32_t)ptr[i];
            Logger::outFile << std::setfill('0') << std::hex << std::setw(2) << (uint32_t)ptr[i];
    }
    std::cout << std::endl;
    Logger::outFile << std::endl;
}

/******************************************************************************/
void Logger::Out(std::string stringout) 
{
    if(log_level < LogLevel::TRACE)
    {
        return;
    }
    std::cout << "           " << stringout;
    Logger::outFile << "           " << stringout;
}

/******************************************************************************/
void Logger::Message(const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    msg = StringUtils::vFormat(format, ap);
    va_end(ap);
    std::cout << msg.c_str() << std::endl;
    Logger::outFile << msg.c_str() << std::endl;
}
