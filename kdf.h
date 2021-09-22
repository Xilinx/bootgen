// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
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

/*****************************************************************************
*
* This source contains the implementation of the interface functions for secure
* library.
*
* MODIFICATION HISTORY:
*
* Ver  Who    Date        Changes
* ----- --- -------- -------------------------------------------------------
* 1.0  Ramya  08/27/2021  Initial release
*
******************************************************************************/

#pragma once

#ifndef _KDF_H_
#define _KDF_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <cmath>
#include <iostream>

#define VERSION                         0x00010000

#define BYTES_PER_AES_SEED              32
#define KDF_COUNTER_BYTES               4

/* Error Codes */
#define KDF_INPUT_FID_UNAVAILABLE       0x00000001
#define KDF_INPUT_SEED_UNAVAILABLE      0x00000002

#define KDF_CAVP_TESTFILE_UNAVAILABLE   0x00010001
#define KDF_CAVP_TESTFILE_IMPROPER      0x00010002
#define KDF_CAVP_KI_LENGTH_IMPROPER     0x00010003
#define KDF_CAVP_HEXKEY_IMPROPER        0x00010004
#define KDF_CAVP_VERIFY_KO_FAILED       0x00010005

/********************************************************************************/
class Kdf
{
public:
    Kdf()
        : key_out_length(0)
        , key_seed(NULL)
        , fixed_input_data_byte_length(0)
        , fixed_input_data(NULL)
        , verify_ko(NULL)
        , key_out(NULL)
        , kdf_log_file("")
      { };

    ~Kdf()
    {
        if (key_seed != NULL)
        {
            delete[] key_seed;
        }
        if (fixed_input_data != NULL)
        {
            delete[] fixed_input_data;
        }
        if (verify_ko != NULL)
        {
            delete[] verify_ko;
        }
        if(key_out != NULL)
        {
            delete[] key_out;
        }
    };

    uint32_t CounterModeKDF(uint32_t* k_in, uint32_t* fid, uint32_t fid_byte_length, uint32_t* ko_buf, uint32_t ko_bytes, bool reset_cntr = true);
    uint32_t CAVPonCounterModeKDF(std::string filename);
    void ResetKdfCounter();
    void SetKdfLogFilename(std::string filename);

    std::string GetKdfLogFilename(void);
    uint32_t GetVersion(void);

private:
    uint8_t* key_out;
    uint8_t* key_seed;
    uint32_t key_out_length;
    uint8_t* verify_ko;
    uint8_t* fixed_input_data;
    uint32_t fixed_input_data_byte_length;
    uint32_t version;
    uint8_t  kdf_counter[KDF_COUNTER_BYTES];
    std::string kdf_log_file;

    void KDF(uint32_t ko_bytes, bool reset_cntr = true);
    uint32_t ParseKDFTestVectorFile(std::string filename);
    bool IsKdfCounterMax();
};

#endif
