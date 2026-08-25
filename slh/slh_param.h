//  slh_param.h
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === Internal parameter definition structure.

#ifndef _SLH_PARAM_H_
#define _SLH_PARAM_H_

#include "plat_local.h"

//  address

typedef union {
    uint8_t u8[32];
    uint32_t u32[8];
} adrs_t;

//  Parameter sets
typedef struct slh_param_s  slh_param_t;
typedef struct slh_ctx_s    slh_ctx_t;

struct slh_param_s {
    const char  *alg_id;    //  Algorithm name

    //  core parameters
    uint32_t    n;          //  Security level / hash size { 16,24,32 }.
    uint32_t    h;          //  Bits h used to select FORS key.
    uint32_t    d;          //  Number of hypertree layers d.
    uint32_t    hp;         //  Merkle tree height h' (XMSS).
    uint32_t    a;          //  String length t = 2**a (FORS).
    uint32_t    k;          //  Number of strings (FORS).
    uint32_t    lg_w;       //  Number of bits in chain index (WOTS+)
    uint32_t    m;          //  Length in bytes of message digest.

    //  hash function instantation
    void (*mk_ctx)(slh_ctx_t *ctx,  const uint8_t *pk, const uint8_t *sk,
                                    const slh_param_t *prm);
    void (*chain)(slh_ctx_t *ctx,   uint8_t *tmp, const uint8_t *x,
                                    uint32_t i, uint32_t s);
    void (*wots_chain)(slh_ctx_t *ctx,  uint8_t *tmp, uint32_t s);
    void (*fors_hash)(slh_ctx_t *ctx,   uint8_t *tmp, uint32_t s);
    void (*h_msg)(slh_ctx_t *ctx,   uint8_t *h, const uint8_t *r,
                                    const uint8_t *m, size_t m_sz);
    void (*prf)(slh_ctx_t *ctx,     uint8_t *h);
    void (*prf_msg)(slh_ctx_t *ctx, uint8_t *h, const uint8_t *opt_rand,
                                    const uint8_t *m, size_t m_sz);
    void (*h_f)(slh_ctx_t *ctx, uint8_t *h, const uint8_t *m1);
    void (*h_h)(slh_ctx_t *ctx, uint8_t *h,
                                const uint8_t *m1, const uint8_t *m2);
    void (*h_t)(slh_ctx_t *ctx, uint8_t *h,
                                const uint8_t *m, size_t m_sz);
};

//  _SLH_PARAM_H_
#endif
