/******************************************************************************
* Copyright 2015-2022 Xilinx, Inc.
* Copyright 2022-2023 Advanced Micro Devices, Inc.
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

#include "imageheadertable-versal.h"
#include "partitionheadertable-versal.h"
#include "bootimage-versal.h"
#include "bootimage.h"
extern "C" {
#include "cdo-npi.h"
};

static VersalBootImage * curbi;

void VersalBootImage::PostProcessStart() {
    if (curbi != NULL) {
        LOG_WARNING("post processing of multiple concurrent BIF files not supported");
    }

    uint32_t verbose = 0;
    if (GET_LOG_LEVEL() == LogLevel::INFO ||
        GET_LOG_LEVEL() == LogLevel::DEBUG ||
        GET_LOG_LEVEL() == LogLevel::TRACE) {
        verbose = 1;
    }

    const char *s;
    if (postProcessMode != "")
    {
        s = postProcessMode.c_str();
    }
    else
    {
        s = getenv("BOOTGEN_POST_PROCESSING");
        if (s == NULL || strcmp(s, "") == 0 || strcmp(s, "0") == 0 || strcmp(s, "false") == 0) 
        {
            /* Default post processing more for Si */
            s = "1";
            if (s == NULL || strcmp(s, "") == 0)
                LOG_WARNING("BOOTGEN_POST_PROCESSING is not set, enabling default post processing mode 1");
            if (strcmp(s, "0") == 0 || strcmp(s, "false") == 0)
                LOG_WARNING("BOOTGEN_POST_PROCESSING is disabled, enabling default post processing mode 1");
        }
    }
    
    if (cdocmd_post_process_mode(s, verbose) == 0) 
    {
        LOG_INFO("BOOTGEN_POST_PROCESSING is disabled");
        return;
    }
    //LOG_WARNING("BOOTGEN_POST_PROCESSING is enabled, CDOs will be post processed");
    curbi = this;
    cdocmd_post_process_start();
}

bool VersalBootImage::IsPostProcessingEnabled() 
{
    return (curbi != NULL);
}

ImageHeader* VersalBootImage::PostProcessEnd() 
{
    if (curbi == NULL) return NULL;
    curbi = NULL;
    void * new_data = NULL;
    size_t new_size = 0;
    cdocmd_post_process_end(&new_data, &new_size);
    if (new_data == NULL) return NULL;

    /* Create a new VersalImageHeader object */
    ImageHeader *newImage = new VersalImageHeader((uint8_t *)new_data, new_size);
    newImage->SetPartitionType(PartitionType::CONFIG_DATA_OBJ);

    /* Create a file specification object, usually created while
    parsing BIF or GeneratePdi() API calls from WDI For timebeing,
    I assumed, we don't need enryption/authentication for this new
    image */
    PartitionBifOptions* it = new PartitionBifOptions();
    it->authType = Authentication::None;
    it->encryptType = Encryption::None;
    it->checksumType = Checksum::None;
    ConfigureProcessingStages(newImage, it);
    return newImage;
}

bool VersalImageHeader::PostProcessCdo(const uint8_t* cdo_data, Binary::Length_t cdo_size) 
{
    if (curbi == NULL) return false;
    void * new_data = NULL;
    size_t new_size = 0;
    if (cdocmd_post_process_cdo(cdo_data, cdo_size, &new_data, &new_size)) {
        LOG_ERROR("CDO post process error");
        return false;
    }
    if (new_data == NULL) return false;
    SetPartitionType(PartitionType::CONFIG_DATA_OBJ);
    PartitionHeader* hdr = new VersalPartitionHeader(this, 0);
    hdr->firstValidIndex = true;
    hdr->execAddress = 0;
    hdr->loadAddress = 0xFFFFFFFFFFFFFFFF;
    hdr->partitionSize = hdr->transferSize = new_size;
    hdr->partition = new VersalPartition(hdr, (uint8_t*)new_data, new_size);
    partitionHeaderList.push_back(hdr);
    return true;
}

bool VersalImageHeader::PostProcessCfi(const uint8_t* cdo_data, Binary::Length_t cdo_size) 
{
    if (curbi == NULL) return false;
    void * new_data = NULL;
    size_t new_size = 0;
    uint32_t is_gsc;
    if ((GetPartitionType() == PartitionType::CFI_GSC) || (GetPartitionType() == PartitionType::CFI_GSC_UNMASK)) 
    {
        is_gsc = 1;
    }
    else if (GetPartitionType() == PartitionType::CFI) 
    {
        is_gsc = 0;
    }
    else 
    {
        LOG_ERROR("unexpected CFI partition type - %s ", Filename.c_str());
        return false;
    }
    if (cdocmd_post_process_cfi(cdo_data, cdo_size, is_gsc, &new_data, &new_size)) 
    {
        LOG_ERROR("CFI to CDO post process error");
        return false;
    }
    if (new_data == NULL) 
        return false;

    SetPartitionType(PartitionType::CONFIG_DATA_OBJ);
    PartitionHeader* hdr = new VersalPartitionHeader(this, 0);
    hdr->firstValidIndex = true;
    hdr->execAddress = 0;
    hdr->loadAddress = 0xFFFFFFFFFFFFFFFF;
    hdr->partitionSize = hdr->transferSize = new_size;
    hdr->partition = new VersalPartition(hdr, (uint8_t*)new_data, new_size);
    partitionHeaderList.push_back(hdr);
    return true;
}
