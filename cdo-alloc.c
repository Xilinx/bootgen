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
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include "cdo-alloc.h"

void * myalloc(size_t len) {
    void * p = malloc(len);
    if (p == NULL) {
        fprintf(stderr, "end of memory\n");
        exit(1);
    }
    return p;
}

void * myalloc_zero(size_t len) {
    void * p = myalloc(len);
    memset(p, 0, len);
    return p;
}

void * myrealloc(void * p, size_t len) {
    p = realloc(p, len);
    if (p == NULL) {
        fprintf(stderr, "end of memory\n");
        exit(1);
    }
    return p;
}

/**
 * @brief This function similar to strdup() which is duplicate input string
 * however, this will make sure the length of the output is divisible by 4
 * @param src string source 
 * @return char* string return
 */
char* my_strdup(const char* src) {

    if (src == NULL) return NULL;

    // Calculate the length of the original string
    size_t len = strlen(src) + 1;
    size_t padded_length = (len + 3) & ~3;  // Round up to nearest multiple of 4

    // Allocate memory for the new string with padding
    char* dup = (char*)myalloc_zero(padded_length);
    if (dup == NULL) return NULL;

    // Copy the string and pad with null characters
    strcpy(dup, src);

    return dup;
}

void myfree(void * p) {
    free(p);
}
