/******************************************************************************
* Copyright 2019-2022 Xilinx, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "cdo-upgrade.h"

uint32_t cdoseq_upgrade(CdoSequence * seq, uint32_t version) {
    LINK * l = seq->cmds.next;
    if (version < seq->version) {
        return 1;
    }
    while (l != &seq->cmds) {
        CdoCommand * cmd = all2cmds(l);
        l = l->next;
        switch (cmd->type) {
        case CdoCmdSection:
            cdocmd_free(cmd);
            break;
        default:
            break;
        }
    }
    seq->version = version;
    return 0;
}
