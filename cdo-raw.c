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
#include <string.h>
#include <inttypes.h>
#include "cdo-binary.h"
#include "cdo-alloc.h"
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

static void copy_metadata(char * dst, size_t dstmax, char * src, size_t srclen) {
    size_t len = srclen;
    if (srclen >= dstmax) len = dstmax - 1;
    memcpy(dst, src, len);
    dst[len] = '\0';
}

CdoRawInfo * cdoraw_decode(void * data, size_t size) {
    CdoRawInfo * raw = NULL;
    size_t capacity = 0;
    size_t offset = 0;
    char * s = data;
    char * e = s + size;
    char * p = s;
    uint32_t words = 0;
    uint64_t bits = 0;
    skipline(s, e);
    if ((s-p == 28 && memcmp(p, "Xilinx ASCII NPI Deviceimage", s-p) == 0) ||
        (s-p == 16 && memcmp(p, "Xilinx ASCII CDO", s-p) == 0) ||
        (s-p == 32 && memcmp(p, "Xilinx ASCII PSAXIMM Deviceimage", s-p) == 0)) {
        raw = cdoraw_create(CdoRawCdo);
        words = 1;
    } else if ((s-p == 22 && memcmp(p, "Xilinx ASCII Bitstream", s-p) == 0) ||
               (s-p == 28 && memcmp(p, "Xilinx ASCII CFI Deviceimage", s-p) == 0)) {
        raw = cdoraw_create(CdoRawCfi);
        words = 4;
    } else if (s-p == 2) {
        raw = cdoraw_create(CdoRawUnknown);
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
            if (offset >= capacity) {
                capacity = capacity ? capacity * 2 : 1;
                raw->data = (uint32_t *)myrealloc(raw->data, capacity * sizeof *raw->data);
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
        if (s >= e) goto error;
        if (s-p >= 10 && memcmp(p, "Created by", 10) == 0) {
            p += 10;
            skipsp(p);
            while (p < s && isspc(s[-1])) s--;
            /*copy_metadata(raw->meta.creator, sizeof raw->meta.creator, p, s-p);*/
        } else if (s-p >= 12 && memcmp(p, "Design name:", 12) == 0) {
            p += 12;
            skipsp(p);
            while (p < s && isspc(s[-1])) s--;
            copy_metadata(raw->meta.design, sizeof raw->meta.design, p, s-p);
        } else if (s-p >= 13 && memcmp(p, "Architecture:", 13) == 0) {
            p += 13;
            skipsp(p);
            while (p < s && isspc(s[-1])) s--;
            copy_metadata(raw->meta.arch, sizeof raw->meta.arch, p, s-p);
        } else if (s-p >= 5 && memcmp(p, "Part:", 5) == 0) {
            p += 5;
            skipsp(p);
            while (p < s && isspc(s[-1])) s--;
            copy_metadata(raw->meta.part, sizeof raw->meta.part, p, s-p);
        } else if (s-p >= 5 && memcmp(p, "Date:", 5) == 0) {
            p += 5;
            skipsp(p);
            while (p < s && isspc(s[-1])) s--;
            /*copy_metadata(raw->meta.date, sizeof raw->meta.date, p, s-p);*/
        } else if (s-p >= 5 && memcmp(p, "Bits:", 5) == 0) {
            p += 5;
            skipsp(p);
            if (parse_u64(&p, &bits)) goto error;
            skipsp(p);
            if (p != s) goto error;
        } else {
            char * q = p;
            while (q < s && *q != ':') q++;
            if (q == s) {
                /* End of raw header */
                s = p;
                break;
            }
            fprintf(stderr, "warning: unexpected raw header: %.*s\n", (int)(s-p), p);
        }
        skipnewline(s, e);
    }
    while (s < e) {
        uint32_t value;
        p = s;
        skipline(s, e);
        if (s >= e) goto error;
        if (parse_hex32(&p, &value)) goto error;
        if (offset >= capacity) {
            capacity = capacity ? capacity * 2 : 1;
            raw->data = (uint32_t *)myrealloc(raw->data, capacity * sizeof *raw->data);
        }
        raw->data[offset++] = value;
        if ((offset % words) == 0) {
            if (p != s) goto error;
            skipnewline(s, e);
        }
    }
    if (s < e) goto error;
    raw->size = offset * 4;
    if ((uint64_t)raw->size * 8 != bits) {
        fprintf(stderr, "warning: invalid bits parameter in raw file %"PRIu64", expected %"PRIu64"\n", bits, (uint64_t)raw->size * 8);
    }
    return raw;

error:
    if (raw != NULL) free(raw);
    return NULL;
}

void cdoraw_encode(CdoRawInfo * raw, FILE * f) {
    const char hexstr[] = "0123456789abcdef";
    uint8_t * p = (uint8_t *)raw->data;
    size_t size = raw->size;
    size_t i;
    fprintf(f, "Xilinx ASCII NPI Deviceimage\n");
    fprintf(f, "Created by %s\n", raw->meta.creator ? raw->meta.creator : "CDO utility");
    fprintf(f, "Design name: \t%s\n", raw->meta.design ? raw->meta.design : "");
    fprintf(f, "Architecture:\t%s\n", raw->meta.arch ? raw->meta.arch : "");
    fprintf(f, "Part:        \t%s\n", raw->meta.part ? raw->meta.part : "");
    fprintf(f, "Date:        \t%s\n", raw->meta.date ? raw->meta.date : "");
    fprintf(f, "Bits:        \t%"PRIu64"\n", (uint64_t)size * 8);
    for (i = 0; i < size; i++) {
        uint8_t b = p[i];
        putc(hexstr[(b >> 4) & 15], f);
        putc(hexstr[b & 15], f);
        if ((i & 3) == 3) putc('\n', f);
    }
}

void cdoraw_encode_seq(FILE * f, CdoSequence * seq, uint32_t be) {
    CdoRawInfo * raw = cdoraw_create(CdoRawCdo);
    cdometa_extract(&raw->meta, seq);
    raw->data = cdoseq_to_binary(seq, &raw->size, be);
    cdoraw_encode(raw, f);
    cdoraw_delete(raw);
}

CdoRawInfo * cdoraw_create(CdoRawType type) {
    CdoRawInfo * raw = (CdoRawInfo *)myalloc_zero(sizeof * raw);
    raw->type = type;
    return raw;
}

void cdoraw_delete(CdoRawInfo * raw) {
    if (!raw->data_free_disable) {
        myfree(raw->data);
    }
    myfree(raw);
}

CdoRawInfo * decode_raw(void * data, size_t size) {
    CdoRawInfo * raw = cdoraw_decode(data, size);
    CdoRawInfo * newraw = NULL;
    if (raw == NULL) return NULL;
    newraw = (CdoRawInfo *)myalloc(sizeof * newraw + raw->size);
    *newraw = *raw;
    newraw->data = (uint32_t *)(newraw + 1);
    memcpy(newraw->data, raw->data, raw->size);
    newraw->data_free_disable = 1;
    cdoraw_delete(raw);
    return newraw;
}

void encode_raw(FILE * f, void * data, size_t size) {
    CdoRawInfo * raw = cdoraw_create(CdoRawCdo);
    raw->data = data;
    raw->size = size;
    raw->data_free_disable = 1;
    cdoraw_encode(raw, f);
    cdoraw_delete(raw);
}
