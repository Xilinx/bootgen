/******************************************************************************
* Copyright 2019-2021 Xilinx, Inc.
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

#ifndef D_cdo_overlay
#define D_cdo_overlay

#include "cdo-command.h"

typedef struct CdoOverlayInfo CdoOverlayInfo;
typedef struct CdoOverlayEntry CdoOverlayEntry;

struct CdoOverlayEntry {
    CdoCommand * start;
    CdoCommand * end;
    unsigned int level;
};

struct CdoOverlayInfo {
    CdoSequence * seq;
    size_t count;
    CdoOverlayEntry * list;
};

CdoOverlayInfo * cdooverlay_open(CdoSequence * seq);
void cdooverlay_close(CdoOverlayInfo * info);
int cdooverlay_apply(CdoSequence * seq, CdoOverlayInfo * ovl);

#endif /* D_cdo_overlay */
