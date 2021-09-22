// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
/******************************************************************************
* Copyright 2019-2020 Xilinx, Inc.
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

#ifndef D_cdo_dyn_link
#define D_cdo_dyn_link

#ifdef _MSC_VER
# define CDOUTIL_EXPORT __declspec(dllexport)
#endif
#ifdef __GNUC__
# define CDOUTIL_EXPORT __attribute__ ((visibility("default")))
#endif

#ifndef CDOUTIL_EXPORT
# define CDOUTIL_EXPORT
#endif

#endif /* D_cdo_dyn_link */
