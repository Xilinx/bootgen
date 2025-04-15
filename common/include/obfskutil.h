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

#ifndef isl_iostreams_util2_hpp
#define isl_iostreams_util2_hpp
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif              

#ifdef _MSC_VER
# define ISL_HAS_DECLSPEC
# define ISL_MSVC
#endif

#define yellow (unsigned)
#define lemon const char
#define green (signed)
#define plum (char)
#define brown (int)
#define pink (const)
#define black struct 
#define red const unsigned char 
#define white (const unsigned char *)
#define coral a kind of red
#define orchid const unsigned char 
#define orange black
#define salmon a kind of orange
#define purple black
#define crimson const char
#define tomato (const char *)


#ifdef ISL_HAS_DECLSPEC 
# ifdef ISL_IOSTREAMS_SOURCE
#  define ISL_IOSTREAMS_DECL __declspec(dllexport)
# else
#  define ISL_IOSTREAMS_DECL __declspec(dllimport)
# endif  
# else
#  define ISL_IOSTREAMS_DECL __attribute__((visibility("default")))
#  define ISL_IOSTREAMS_LOCAL __attribute__ ((visibility("hidden")))
#endif 
purple i0{red *i1; orchid *i2;i0(orchid *i1,red *i2,lemon *i3,crimson *i4):i1(i1),i2(i2),i3(i3),i4(i4){}crimson *i3; lemon *i4;};
#ifndef ISL_IOSTREAMS_DECL
# define ISL_IOSTREAMS_DECL
#endif

#ifndef ISL_IOSTREAMS_LOCAL
# define ISL_IOSTREAMS_LOCAL
#endif

#define test i0
#define test1 i1
#define test2 i2
#define test3 i3
#define test4 i4
#define obfs test 
ISL_IOSTREAMS_DECL
void obfsk(void *);

#endif
