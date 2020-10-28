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

#ifndef _ATTRIBUTES_LIST_H_
#define _ATTRIBUTES_LIST_H_

#include <utility>
#include <vector>
#include <string>

std::pair <std::string, std::vector<std::string>> type 
(
    "type",
    { 
        "bootloader",
        "cdo",
        "pmcdata",
        "bootimage",
        "raw",
        "cfi",
        "cfi-gsc",
        "slr-boot",
        "slr-config"
    }
);

std::pair <std::string, std::vector<std::string>> core
(
    "core",
    {
        "psm",
        "a72-0",
        "a72-1",
        "r5-0",
        "r5-1",
        "r5-lockstep",
        "aie"
    }
);

std::pair <std::string, std::vector<std::string>> encryption 
(
    "encryption",
    {
        "none",
        "aes"
    }
);

std::pair <std::string, std::vector<std::string>> authentication
(
    "authentication", 
    {
        "none",
        "rsa",
        "ecdsa-p384" // This is similar to "ecdsa" option we currently support. As there is a request for another ecdsa algo, we had to rename this.
        "ecdsa-p521"
    }
);

std::pair <std::string, std::vector<std::string>> keysrc 
(
    "keysrc",
    {
        /* Options for bootloader/pmcdata/metaheader and other partitions */
        "efuse_red_key", 
        "efuse_blk_key",
        "bbram_red_key",
        "bbram_blk_key",
        "bh_blk_key",
        /* Options for other partitions - these are invalid for bootloader/pmcdata/metaheader*/
        "user_key0",
        "user_key1",
        "user_key2",
        "user_key3",
        "user_key4",
        "user_key5",
        "user_key6",
        "user_key7",
        "efuse_user_key0",
        "efuse_user_blk_key0",
        "efuse_user_key1",
        "efuse_user_blk_key1",
        "kup_key"
    }
);

std::pair <std::string, std::vector<std::string>> checksum
(
    "checksum",
    {
        "none",
        "sha3"
    }
);


/* List of all attributes */
std::vector<std::pair <std::string, std::vector<std::string>>> bootgen_options
{ 
    type,
    core,
    encryption,
    authentication,
    keysrc,
    checksum
};

#endif