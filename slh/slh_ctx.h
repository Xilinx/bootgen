//  slh_ctx.h
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === Internal parameter definition structure.

#ifndef _SLH_CTX_H_
#define _SLH_CTX_H_

#include "slh_param.h"
#include "sha2_api.h"

//  some structural sizes
#define SLH_MAX_N   32
#define SLH_MAX_LEN (2 * SLH_MAX_N + 3)
#define SLH_MAX_K   35
#define SLH_MAX_M   49
#define SLH_MAX_HP  9
#define SLH_MAX_A   14

//  context
struct slh_ctx_s {
    const       slh_param_t *prm;
    uint8_t     sk_seed[SLH_MAX_N];
    uint8_t     sk_prf[SLH_MAX_N];
    uint8_t     pk_seed[SLH_MAX_N];
    uint8_t     pk_root[SLH_MAX_N];

#ifdef SLOTH
    volatile adrs_t *adrs;                  //  hardware
#else
    adrs_t      *adrs;                      //  regular pointer
#endif
    adrs_t      t_adrs;                     //  local ADRS buffer

    //  precomputed values
    sha256_t    sha256_pk_seed;
    sha512_t    sha512_pk_seed;
};

//  === Lower-level functions

//  Core signing function (of a randomized digest) with initialized context.
size_t slh_do_sign( slh_ctx_t *ctx,
                    uint8_t *sig, const uint8_t *digest);

//  _SLH_CTX_H_
#endif
