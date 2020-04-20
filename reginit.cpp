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
#include <fstream>

#include "bootimage.h"
#include "reginitscanner.h"
#include "options.h"
#include "bifoptions.h"


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/*****************************************************************************/
void RegisterTable::Build(Options& options, RegisterInitTable* regtab0)
{
    regtab = regtab0;
    filename = options.bifOptions->GetRegInitFileName();
    
    if (filename != "")
    {
        LOG_INFO("Started RE parsing : %s", filename.c_str());
        
        RE::FlexScanner scanner;
        RE::BisonParser parser(scanner,options,*this);
        scanner.filename = filename;
        std::ifstream s(filename.c_str());
        if (!s) 
        {
            LOG_ERROR("Cannot read file - %s", filename.c_str());;
        }
        scanner.switch_streams(&s);
        parser.parse();

        LOG_INFO("Done RE parsing : %s. Added %d regiter pairs", filename.c_str(), count);
    }

    /* Fill the remainder of the area with NOPs. */
    while(count < MAX_REGISTER_INITS) 
    {
        Add(options, INVALID_REGISTER_ADDRESS,0);
    }

}

/******************************************************************************/
void RegisterTable::Add(Options& options, uint32_t address, uint32_t value)
{
    if (count >= MAX_REGISTER_INITS) 
    {
        LOG_ERROR("Too many register init pairs in %s", filename.c_str());
    }
    if (address != 0xFFFFFFFF) 
    {
        LOG_INFO("count [0x%8x], value = %d", address, value);
    }

    regtab->registerInitialization[count ].address = address;
    regtab->registerInitialization[count ].value   = value;
    count++;
}
