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

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include "ospi_rx_tuning.h"
#include "bootimage.h"
#include "bifoptions.h"
#include "options.h"
#include "logger.h"

/*
-------------------------------------------------------------------------------
*********************************************************   F U N C T I O N S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
bool ShouldEmbedOspiRxTuningPattern(BootImage& bi)
{
    /* Only the families whose PLM reserves section ID 0x05 take the pattern.
       This is an allowlist rather than an opt-out: a derived family can reach
       this code through Versal_2ve_2vmImageHeaderTable, and any future family
       doing the same must opt in deliberately. */
    if ((bi.options.archType != Arch::VERSAL) && (bi.options.archType != Arch::VERSALGEN2))
    {
        LOG_TRACE("Not embedding OSPI RX tuning pattern - architecture does not use it");
        return false;
    }

    /* Partial PDIs carry no bootloader and are never fetched from OSPI flash
       at boot, so the calibration pattern has no consumer there. */
    if (!bi.IsBootloaderFound())
    {
        LOG_TRACE("Not embedding OSPI RX tuning pattern - not a full PDI");
        return false;
    }

    /* HSM stage 1 - bootgen is only emitting hashes for offline signing. */
    if (bi.options.DoGenerateHashes())
    {
        LOG_TRACE("Not embedding OSPI RX tuning pattern - HSM stage 1");
        return false;
    }

    /* HSM stage 2 - the signatures were produced offline over a metaheader
       that bootgen must now reproduce exactly, so its contents must not
       change. The same applies when any partition is presigned. */
    if ((bi.bifOptions->metaHdrAttributes.presign != "") ||
        (bi.bifOptions->GetHeaderSignatureFile() != ""))
    {
        LOG_TRACE("Not embedding OSPI RX tuning pattern - HSM stage 2 (presigned metaheader)");
        return false;
    }

    for (std::list<PartitionBifOptions*>::iterator partitionBifOptions = bi.bifOptions->partitionBifOptionList.begin();
         partitionBifOptions != bi.bifOptions->partitionBifOptionList.end(); partitionBifOptions++)
    {
        if ((*partitionBifOptions != NULL) && ((*partitionBifOptions)->presignFile != ""))
        {
            LOG_TRACE("Not embedding OSPI RX tuning pattern - HSM stage 2 (presigned partition)");
            return false;
        }
    }

    return true;
}
