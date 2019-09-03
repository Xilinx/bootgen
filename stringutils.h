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

#pragma once

#ifndef _STRINGUTILS_H_
#define _STRINGUTILS_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <string>
#include <cstdarg>
#include <algorithm> // for std::transform
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class StringUtils 
{
public:
    static bool EndsWith(const std::string& str, const std::string& suffix)
    {
        size_t i = str.rfind(suffix);
        return (i != std::string::npos) && (i == (str.length() - suffix.length()));
    }

    static std::string ChangeExtension(const std::string& source, const std::string& extension)
    {
        if (extension[0] != '.')
        {
            throw std::exception();
        }
        std::string s = source;
        size_t i = s.rfind(".");
        size_t len = s.length() - i;
        s.replace(i,len,extension);
        return s;
    }

    static std::string RemoveExtension(const std::string& source)
    {
        std::string s = source;
        return s.substr(0, s.find_last_of("."));
    }

    static std::string GetExtension(const std::string& source)
    {
        std::string s = source;
        size_t i = s.rfind(".");
        if (i == std::string::npos) return "";
        std::string x = s.substr(i);
        return x;
    }

    static void ToLower(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    }

    static std::string BaseName(const std::string& fullpath)
    {
        size_t i1 = fullpath.rfind("/");
        size_t i2 = fullpath.rfind("\\");
        std::string x;
        if (i1 != std::string::npos)
        {
            x = fullpath.substr(i1+1);
        }
        else if (i2 != std::string::npos)
        {
            x = fullpath.substr(i2+1);
        }
        else
        {
            x = fullpath;
        }
        return x;
    }

    static std::string Format(const std::string fmt, ...)
    {
        std::string str;
        va_list ap;
        va_start(ap, fmt);
        str = vFormat(fmt,ap);
        va_end(ap);
        return str;
    }

    static std::string vFormat(const std::string fmt, va_list ap)
    {
        int size = 300;
        std::string str;
        while (1) 
        {
            str.resize(size);
            int n = vsnprintf((char *)str.c_str(), size,fmt.c_str(), ap);
            if (n > -1 && n < size)
            {
                str.resize(n);
                return str;
            }
            if (n > -1)
                size = n + 1;
            else
                size *= 2;
        }
        return str;
    }

    static std::pair<std::string, std::string> Split_jsonString(const std::string& str) 
    {
        auto first = str.find_last_of('{');
        if (first == std::string::npos) return{};
        auto mid = str.find_first_of(':', first);
        if (mid == std::string::npos) return{};
        auto last = str.find_first_of('}', mid);
        if (last == std::string::npos) return{};
        return{ str.substr(first + 1, mid - first - 1), str.substr(mid + 1, last - mid - 1) };
    }

    static std::string Trim_quotes(const std::string& str) {
        auto first = str.find_first_of('"');
        if (first == std::string::npos) return{};
        auto last = str.find_first_of('"', first + 1);
        if (last == std::string::npos) return{};
        return str.substr(first + 1, last - first - 1);
    }

    static std::string FolderPath(const std::string& fullpath)
    {
        size_t i1 = fullpath.rfind("/");
        size_t i2 = fullpath.rfind("\\");
        std::string x;
        if (i1 != std::string::npos)
        {
            x = fullpath.substr(0, i1);
        }
        else if (i2 != std::string::npos)
        {
            x = fullpath.substr(0, i2);
        }
        else
        {
            x = "./";
        }
        return x;
    }
};
#endif
