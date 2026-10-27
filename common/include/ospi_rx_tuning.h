/******************************************************************************
* Copyright 2026 Advanced Micro Devices, Inc.
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

#ifndef _OSPI_RX_TUNING_H_
#define _OSPI_RX_TUNING_H_

#include <stdint.h>

class BootImage;

/*
-------------------------------------------------------------------------------
*****************************************************   C O N S T A N T S   ***
-------------------------------------------------------------------------------
*/

/* IHT optional data section ID reserved by PLM for the OSPI RX tuning pattern.
   Matches XILPDI_OSPI_RX_TUNING_DATA_ID in embeddedsw (SSW-10366). */
#define OSPI_RX_TUNING_SECTION_ID       0x05

#define OSPI_RX_TUNING_PATTERN_SIZE     128

/* Fixed read-path (RX) calibration sequence for OSPI DDR/PHY mode.
   The OSPI controller reads these bytes back from flash and compares them
   against the copy compiled into the driver to lock its sampling delay, so
   these bytes must stay in lockstep with phy_tuning_pattern[] in embeddedsw.
   They are not user-configurable - see VITIS-13894 / SSW-10366.
   sha256: e88e2b4d364369d3cfe4559de735cd2648705cd9b45599114661ab09b4f1c073 */
static const uint8_t OspiRxTuningPattern[OSPI_RX_TUNING_PATTERN_SIZE] =
{
    0xFE, 0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
    0x00, 0xFE, 0xFE, 0x01, 0x01, 0x01, 0x01, 0x00,
    0x00, 0xFE, 0xFE, 0x01, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x00, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x00, 0xFE, 0xFE, 0xFF, 0x01, 0x01,
    0x01, 0x01, 0x01, 0xFE, 0x00, 0xFE, 0xFE, 0x01,
    0x01, 0x01, 0x01, 0xFE, 0x00, 0xFE, 0xFE, 0x01,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0xFE,
    0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0xFE,
    0xFE, 0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
    0xFE, 0xFE, 0xFE, 0x01, 0x01, 0x01, 0x01, 0x00,
    0xFE, 0xFE, 0xFE, 0x01, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0xFE, 0xFE, 0xFE, 0xFF, 0x01, 0x01,
    0x01, 0x01, 0x01, 0xFE, 0xFE, 0xFE, 0xFE, 0x01,
    0x01, 0x01, 0x01, 0xFE, 0xFE, 0xFE, 0xFE, 0x01
};

/*
-------------------------------------------------------------------------------
*****************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/* True when the built-in pattern should be embedded in this image: full base
   PDIs only, excluding partial PDIs and both HSM signing stages. */
bool ShouldEmbedOspiRxTuningPattern(BootImage& bi);

#endif
