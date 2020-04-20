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
#include <string>
#include <time.h>
#include <signal.h>

#include "bootimage.h"
#include "bootgenexception.h"
#include "options.h"
#include "bifoptions.h"
#include "stringutils.h"
#include "version.h"


#include "encryption-zynqmp.h"

#ifdef _WIN32
#include "openssl/ms/applink.c"
#endif
static const char* time_stamp = __TIME__;
static const char* date_stamp = __DATE__;


/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
static void DisplayBanner()
{
    char version[64], *year;
    strncpy(version, RDI_VERSION, sizeof version);
    version[sizeof version - 1] = '\0';
    year = strtok(version, ".");
    LOG_MSG("\n");
    LOG_MSG("****** %s v%s", PROGRAMNAME, RDI_VERSION);
    LOG_MSG("  **** Build date : %s-%s", date_stamp, time_stamp);
    LOG_MSG("    ** Copyright 1986-%s Xilinx, Inc. All Rights Reserved.\n", year ? year : "2018");
}

/******************************************************************************/
class BootGenApp
{
public:
    void Run(int argc, const char* argv[])
    {
        Options options;
        options.ParseArgs(argc, argv);
        options.ProcessVerifyKDF();
        options.ProcessReadImage();
        std::string bifFile = options.GetBifFilename();
        LOG_TRACE("BIF File: %s", bifFile.c_str());

        if (bifFile.length() > 0) 
        {
            BIF_File bif(bifFile);
            bif.Process(options);
            LOG_MSG("\n[INFO]   : Bootimage generated successfully\n");
        }
    }
};

/******************************************************************************/
int main(int argc, const char* argv[])
{
    try 
    {
        BootGenApp app;
        DisplayBanner();
        app.Run(argc,argv);
        return 0;
    }
    catch(std::exception& ex) 
    {
        std::cerr << "[ERROR]  : " << ex.what() << std::endl;
        return 1;
    }
    catch(const char* msg) 
    {
        std::cerr << "FATAL: Internal Assertion: " << msg << std::endl;
        return 2;
    }
    catch(...) 
    {
        std::cerr << "FATAL: Unknown Exception caught."<< std::endl;
        return 3;
    }
}
