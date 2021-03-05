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

#ifndef _FILEUTILS_H_
#define _FILEUTILS_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <string>
#include <iostream>
#include <fstream>
#include <stdint.h>

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class ByteFile
{
public:
    ByteFile(const std::string& name)
    {
        std::ifstream fl(name.c_str(), std::ios::binary);
        if (!fl)
        {
            LOG_ERROR("Cannot read file - %s", name.c_str());
        }
        
        fl.seekg(0, std::ios::end);
        if(!fl)
        {
            LOG_ERROR("Cannot seek to end of file - %s", name.c_str());
        }
        if(fl.bad() || fl.fail())
        {
            LOG_ERROR("Cannot seek to end of file - %s", name.c_str());
        }
        if(! fl.good())
        {
            LOG_ERROR("Cannot seek to end of file - %s", name.c_str());
        }
        len = fl.tellg();
        bytes = new uint8_t[len];
        fl.seekg(0, std::ios::beg); 
        fl.read((char*)bytes, len);
        fl.close();
    }

    ~ByteFile()
    {
        if (bytes)
        {
            delete [] bytes;
        }
        bytes = 0;
    }

    uint8_t* bytes;
    size_t len;
};

/******************************************************************************/
class FileImport
{
public:
    bool LoadHexData(const std::string& fileName, uint8_t* dataArray, int count)
    {
        FILE* f;
        int datum, ret, nbytes;
        nbytes = 0;
        memset(dataArray, 0, count);
        f = fopen(fileName.c_str(), "r");

        if ((ret = fscanf(f, "%2X", &datum)) == EOF)
        {
            fclose(f);
            LOG_ERROR("File is empty - %s", fileName.c_str());
        }
        fclose(f);
        f = fopen(fileName.c_str(), "r");
        
        if(f)
        {
            while ((ret = fscanf(f, "%2X", &datum)) != EOF)
            {
                if (ret == 1)
                {
                    if (datum < 0 || datum > 255)
                    {
                        fclose(f);
                        LOG_ERROR("Bad hex character %x in file - %s", datum, fileName.c_str());
                    }
                    if (nbytes == count)
                    {
                        fclose(f);
                        return false;
                    }
                    dataArray[nbytes++] = datum;
                }
                else
                {
                    fclose(f);
                    LOG_ERROR("Unable to read hex characters from file - %s", fileName.c_str());
                }
            }
            if (nbytes < count)
            {
                fclose(f);
                return false;
            }
        }
        else
        {
            LOG_ERROR("Unable to open file - %s", fileName.c_str());
        }
        fclose(f);
        return true;
    }
};

class FileUtils
{
public:
    static uint32_t GetFileSize(std::string filename)
    {
        FILE *p_file = NULL;
        uint32_t size = 0;
        p_file = fopen(filename.c_str(), "rb");
        if (p_file)
        {
            fseek(p_file, 0, SEEK_END);
            size = ftell(p_file);
        }
        else
        {
            LOG_ERROR("Unable to open file - %s", filename.c_str());
        }
        fclose(p_file);
        return size;
    }
};
#endif
