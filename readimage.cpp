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

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstring>

#include "readimage.h"

/*******************************************************************************/
void ReadImage::Separator()
{
#ifdef SEPARATOR
    std::cout << SEPARATOR << std::endl;
#else
    std::cout << std::endl;
#endif
}

/*******************************************************************************/
void ReadImage::DisplayImageDetails(ReadImageOption::Type type, DumpOption::Type dump, std::string path)
{
    ReadBinaryFile();
    switch (type)
    {
    case ReadImageOption::BH:
        DisplayBootHeader();
        break;

    case ReadImageOption::IHT:
        DisplayImageHeaderTable();
        break;

    case ReadImageOption::IH:
        DisplayImageHeaders();
        break;

    case ReadImageOption::PHT:
        DisplayPartitionHeaderTable();
        break;

    case ReadImageOption::AC:
        DisplayAuthenicationCertificates();
        break;

    default:
        DisplayBootHeader();
        DisplayImageHeaderTable();
        DisplayImageHeaders();
        DisplayPartitionHeaderTable();
        DisplayAuthenicationCertificates();
        break;
    }
    Separator();
}

/*******************************************************************************/
void ReadImage::DisplayIV(std::string name, uint32_t* ptr)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << name;
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << ptr[0] << std::setw(8) << std::hex << ptr[1] << std::setw(8) << std::hex << ptr[2] << std::endl;
}

/******************************************************************************/
void ReadImage::DisplayKey(std::string name, uint32_t* ptr)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << name;
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << ptr[0] << std::setw(8) << std::hex << ptr[1] << std::setw(8) << std::hex << ptr[2] << std::setw(8) << std::hex << ptr[3] << std::setw(8) << std::hex << ptr[4] << std::setw(8) << std::hex << ptr[5] << std::endl;
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << " ";
    std::cout << std::setfill('0') << std::setw(8) << std::hex << ptr[6] << std::setw(8) << std::hex << ptr[7] << std::endl;
}

/*******************************************************************************/
void ReadImage::DisplayValue(std::string name1, uint32_t value1, std::string name2, uint32_t value2)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << name1;
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << value1;
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << name2;
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << value2;
    std::cout << std::endl;
}

/*******************************************************************************/
void ReadImage::DisplayValue(std::string name1, uint32_t value1, std::string name2, std::string value2)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << name1;
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << value1;
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << name2;
    std::cout << std::left << value2;
    std::cout << std::endl;
}

/*******************************************************************************/
void ReadImage::DisplayValue(std::string name1, uint32_t value1)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << name1;
    std::cout << std::setfill('0') << "0x" << std::setw(8) << std::hex << value1;
    std::cout << std::endl;
}

/*******************************************************************************/
void ReadImage::DisplayLongValues(std::string name, uint8_t* ptr, uint32_t num_bytes)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << name;
    uint32_t index = 0;
    for (uint32_t count = 0; count < num_bytes; count++)
    {
        index++;
        uint32_t temp = ptr[count];
        //std::cout << std::setfill('0') << std::setw(2) << std::hex << ptr[count];
        std::cout << std::hex << std::setfill('0') << std::setw(2) << temp;
        if ((index % 24 == 0) && (index < num_bytes))
        {
            std::cout << std::endl;
            std::cout << std::setw(30);
            std::cout << std::setfill(' ');
            std::cout << std::right << " ";
        }
    }
    std::cout << std::endl;
}

/*******************************************************************************/
void ReadImage::DisplayAscii(std::string name1, std::string value1)
{
    std::cout << std::setw(30);
    std::cout << std::setfill(' ');
    std::cout << std::right << name1;
    std::cout << value1;
    std::cout << std::endl;
}

/******************************************************************************/
void ReadImage::DisplayAttributes(std::string name1, std::string value1, std::string name2, std::string value2)
{
    std::cout << std::setw(25);
    std::cout << std::setfill(' ');
    std::cout << std::right << name1;
    std::cout << std::setw(12);
    std::cout << std::left << value1;
    if (name2 != "")
    {
        std::cout << std::setw(15);
        std::cout << std::right << name2;
        std::cout << std::setw(15);
        std::cout << std::left << value2;
    }
    std::cout << std::endl;
}
