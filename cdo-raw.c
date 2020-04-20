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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#include "cdo-raw.h"

#define isspc(c) (c == ' ' || c == '\t')
#define isnewline(c) (c == '\n' || c == '\r')
#define skipsp(s)                               \
    do {                                        \
        int c = *s;                             \
        while (isspc(c)) c = *++s;              \
    } while (0)
#define skipline(s, e)                          \
    do {                                        \
        int c;                                  \
        if (s >= e) break;                      \
        c = *s;                                 \
        if (isnewline(c)) break;                \
        s++;                                    \
    } while (1)
#define skipnewline(s, e)                       \
    do {                                        \
        int c;                                  \
        if (s >= e) break;                      \
        c = *s;                                 \
        if (c == '\r') {                        \
            s++;                                \
            if (s >= e) break;                  \
            c = *s;                             \
        }                                       \
        if (c == '\n') s++;                     \
    } while (0)

static uint32_t parse_u64(char ** sp, uint64_t * valuep) {
    char * s = *sp;
    skipsp(s);
    errno = 0;
    *valuep = (uint64_t)strtoull(s, sp, 0);
    if (s == *sp || errno != 0) return 1;
    return 0;
};

static uint32_t parse_hex32(char ** sp, uint32_t * valuep) {
    uint32_t value = 0;
    char * s = *sp;
    int i;
    for (i = 0; i < 8; i++) {
        int c = *s++;
        int v;
        if (c >= '0' && c <= '9') v = c - '0';
        else if (c >= 'a' && c <= 'f') v = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') v = c - 'A' + 10;
        else return 1;
        value = (value << 4) | v;
    }
    *valuep = value;
    *sp = s;
    return 0;
}

static uint32_t parse_hex8(char ** sp, uint32_t * valuep) {
    uint32_t value = 0;
    char * s = *sp;
    int i;
    for (i = 0; i < 2; i++) {
        int c = *s++;
        int v;
        if (c >= '0' && c <= '9') v = c - '0';
        else if (c >= 'a' && c <= 'f') v = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') v = c - 'A' + 10;
        else return 1;
        value = (value << 4) | v;
    }
    *valuep = value;
    *sp = s;
    return 0;
}

CdoRawInfo * decode_raw(void * data, size_t size) {
    CdoRawInfo * raw = NULL;
    CdoRawType type;
    size_t offset = 0;
    char * s = data;
    char * e = s + size;
    char * p = s;
    uint32_t words = 0;
    int invalid_size = 0;
    uint64_t bits = 0;
    skipline(s, e);
    if ((s-p == 28 && memcmp(p, "Xilinx ASCII NPI Deviceimage", s-p) == 0) ||
        (s-p == 32 && memcmp(p, "Xilinx ASCII PSAXIMM Deviceimage", s-p) == 0)) {
        type = CdoRawCdo;
        words = 1;
    } else if ((s-p == 22 && memcmp(p, "Xilinx ASCII Bitstream", s-p) == 0) ||
               (s-p == 28 && memcmp(p, "Xilinx ASCII CFI Deviceimage", s-p) == 0)) {
        type = CdoRawCfi;
        words = 4;
    } else if (s-p == 2) {
        raw = (CdoRawInfo *)malloc(sizeof * raw);
        if (raw == NULL) goto error;
        raw->type = CdoRawUnknown;
        raw->size = 4;
        s = p;
        while (s < e) {
            uint32_t value = 0;
            uint32_t i;
            for (i = 0; i < 4; i++) {
                uint32_t byte;
                if (s + 2 >= e) {
                    fprintf(stderr, "warning: last word is incomplete %.*s\n", (int)(e-s), s);
                    goto incomplete_word;
                }
                if (parse_hex8(&s, &byte)) goto error;
                value = (value << 8) | byte;
                if (!isnewline(*s)) goto error;
                skipnewline(s, e);
            }
            if (offset >= raw->size/4) {
                raw->size *= 2;
                raw = (CdoRawInfo *)realloc(raw, sizeof * raw + raw->size);
            }
            raw->data[offset++] = value;
        }
    incomplete_word:
        raw->size = offset * 4;
        if (s < e) goto error;
        return raw;
    } else {
        goto error;
    }
    skipnewline(s, e);
    while (s < e) {
        p = s;
        skipline(s, e);
        if (s-p >= 5 && memcmp(p, "Bits:", 5) == 0) {
            uint64_t bytes;
            if (s >= e) goto error;
            p += 5;
            while (p < s && isspc(*p)) p++;
            if (parse_u64(&p, &bits)) goto error;
            if ((bits & 7) != 0) goto error;
            bytes = bits / 8;
            if ((bytes/4 & (words - 1)) != 0) goto error;
            raw = (CdoRawInfo *)malloc(sizeof * raw + bytes);
            if (raw == NULL) goto error;
            raw->type = type;
            raw->size = bytes;
            skipnewline(s, e);
            break;
        }
        skipnewline(s, e);
    }
    while (s < e) {
        uint32_t value;
        if (offset >= raw->size/4) {
            raw->size *= 2;
            raw = (CdoRawInfo *)realloc(raw, sizeof * raw + raw->size);
            invalid_size = 1;
        }
        if (parse_hex32(&s, &value)) goto error;
        raw->data[offset++] = value;
        if ((offset % words) == 0) {
            if (!isnewline(*s)) goto error;
            skipnewline(s, e);
        }
    }
    if (s < e) goto error;
    if (offset != raw->size/4) invalid_size = 1;
    if (invalid_size) {
        raw->size = offset * 4;
        fprintf(stderr, "warning: invalid bits parameter in raw file %"PRIu64", expected %"PRIu64"\n", bits, (uint64_t)raw->size * 8);
    }
    return raw;

error:
    if (raw != NULL) free(raw);
    return NULL;
}

void encode_raw(FILE * f, void * data, size_t size) {
    const char hexstr[] = "0123456789abcdef";
    uint8_t * p = (uint8_t *)data;
    size_t i;
    fprintf(f, "Xilinx ASCII NPI Deviceimage\n");
    fprintf(f, "Created by CDO utility\n");
    fprintf(f, "Design name:  \n");
    fprintf(f, "Architecture: \n");
    fprintf(f, "Part:         \n");
    fprintf(f, "Date:         \n");
    fprintf(f, "Bits:         %"PRIu64"\n", (uint64_t)size * 8);
    for (i = 0; i < size; i++) {
        uint8_t b = p[i];
        putc(hexstr[(b >> 4) & 15], f);
        putc(hexstr[b & 15], f);
        if ((i & 3) == 3) putc('\n', f);
    }
}
