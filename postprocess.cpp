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

#include "imageheadertable-versal.h"
#include "partitionheadertable-versal.h"
#include "bootimage-versal.h"
#include "bootimage.h"
extern "C" {
#include "cdo-npi.h"
};

bool VersalImageHeader::PostProcessCdo(const uint8_t* cdo_data, Binary::Length_t cdo_size) {
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
