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

#ifndef _BOOTGENEXCEPTION_H_
#define _BOOTGENEXCEPTION_H_


/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdexcept>
#include <cstdarg>
#include "stringutils.h"


/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define BootGenException(a)      BootGenExceptionClass(__FILE__,__LINE__,a)
#define BootGenException1(a,b)   BootGenExceptionClass(__FILE__,__LINE__,a,b)
#define BootGenException2(a,b,c) BootGenExceptionClass(__FILE__,__LINE__,a,b,c)


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class BootGenExceptionClass : public std::exception 
{
public:
    BootGenExceptionClass(const char* filename0, int line0, const std::string& msg0 ) 
        : msg(msg0)
        , filename(filename0)
        , line(line0) 
    {
    }
    
    BootGenExceptionClass(const char* filename0, int line0, const char* format, ...) 
        : filename(filename0)
        , line(line0) 
    {
        va_list ap;
        va_start(ap, format);
        msg = StringUtils::vFormat(format,ap);
        va_end(ap);
    }
    
    ~BootGenExceptionClass() throw() 
    {
    }
    
    const char* what()  const throw() 
    { 
        return msg.c_str();
    }

private:
    std::string msg;
    std::string filename;
    int line;
};
#endif
