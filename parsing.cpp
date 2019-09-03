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
#include <fstream>
#include "parsing.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
void Parsing::Error(const BIF::BisonParser::location_type &loc, const std::string &msg) 
{
    std::cerr << "\nERROR:BootGen - " << msg << "\n";
    std::cerr << "   Line #" << loc.begin.line << ", \"" << *loc.begin.filename << "\".\n";

    std::string linedata;
    {
        std::ifstream echo(loc.begin.filename->c_str());
        for(unsigned int line=0;line<loc.begin.line;line++) {
          std::getline(echo,linedata);
        }
    }
    Error2(loc,linedata);
}

/******************************************************************************/
void Parsing::Error2(const BIF::BisonParser::location_type &loc, const std::string& linedata)
{
    try 
    {
        int first;
        if (loc.begin.column > 10) 
        {
            std::cerr << "... ";
            first = loc.begin.column - 10;
        } 
        else 
        {
            first = 1;
        }
        int len = loc.end.column - loc.begin.column;
        if (len > 70)
        {
            len = 70;
        }
        std::cerr << linedata.substr(first-1,70) << "\n";
        unsigned int i;
        if (first != 1)
        {
            std::cerr << "    ";
        }
        for(i=first;i<loc.begin.column;i++)  std::cerr << (linedata[i-1]=='\t'?'\t':' ');
        for(;i<loc.end.column;i++) std::cerr << "^";
        std::cerr << "\n" << std::endl;
    }
    catch(...)
    {
        std::cerr << "Parsing problem at " << loc << std::endl;
        throw;
    }
}
