//  kat_drbg.h
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === AES-CTR for a a "NIST-compatible" PRNG API

#ifndef _KAT_DRBG_H_
#define _KAT_DRBG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_local.h"

//  shared random generator

typedef struct {
    uint8_t key[32];
    uint8_t ctr[16];
    uint32_t rk[60];
} aes256_ctr_drbg_t;

//  initalize with a 48-byte seed
void aes256ctr_xof_init(aes256_ctr_drbg_t *ctx, const uint8_t input48[48]);

//  produce output and update
void aes256ctr_xof(aes256_ctr_drbg_t *ctx, uint8_t *x, size_t xlen);

#ifdef __cplusplus
}
#endif

#endif  //  _KAT_DRBG_H_
