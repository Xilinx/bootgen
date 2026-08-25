//  sloth_sha2.c
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === SLotH: Accelerated functions for instantiation of SLH-DSA with SHA2

#ifdef SLOTH_SHA256

#include "slh_ctx.h"
#include "sha2_api.h"
#include "plat_local.h"
#include "sloth_hal.h"
#include "slh_adrs.h"
#include <string.h>

//  compression function instatiation for sha2_256.c

void sha256_compress(void *v)
{
    uint32_t *v32 = (uint32_t *) v;
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;
    int i;

    for (i = 0; i < 24; i++) {
        r32[i] = v32[i];
    }
    r32[S256_TRIG] = 0x01;                  //  start it
    S256_WAIT

    for (i = 0; i < 24; i++) {
        v32[i] = r32[i];
    }
}

//  compression function instatiation for sha2_512.c

#ifdef SLOTH_SHA512

void sha512_compress(void *v)
{
    uint32_t *v32 = (uint32_t *) v;
    volatile uint32_t   *r32 = (volatile uint32_t *) SHA512_BASE_ADDR;
    int i;

    for (i = 0; i < 48; i++) {
        r32[i] = v32[i];
    }
    r32[S512_TRIG] = 0x01;                  //  start it
    S512_WAIT

    for (i = 0; i < 48; i++) {
        v32[i] = r32[i];
    }
}

#endif

//  (Hmsg is not optimized as it is used only once.)

//  Cat 1: Hmsg(R, PK.seed, PK.root, M) =
//      MGF1-SHA-256(R || PK.seed || SHA-256(R ||PK.seed || PK.root || M), m)

static void sha2_256_h_msg( slh_ctx_t *ctx,
                            uint8_t *h,
                            const uint8_t *r,
                            const uint8_t *m, size_t m_sz)
{
    sha256_t sha2;
    uint8_t mgf[16 + 16 + 32 + 4];
    size_t  n = ctx->prm->n;

    //  MGF1-SHA-256(R || PK.seed || ..
    memcpy(mgf, r, n);
    memcpy(mgf + n, ctx->pk_seed, n);

    //  SHA-256(R || PK.seed || PK.root || M)
    sha256_init(&sha2);
    sha256_update(&sha2, r, n);
    sha256_update(&sha2, ctx->pk_seed, n);
    sha256_update(&sha2, ctx->pk_root, n);
    sha256_update(&sha2, m, m_sz);
    sha256_final(&sha2, mgf + 2 * n);

    size_t mgf_sz = 2 * n + 32 + 4;
    uint8_t *ctr = mgf + mgf_sz - 4;

    //  MGF1 counter mode
    for (size_t i = 0; i < ctx->prm->m; i += 32) {
        uint32_t c = i / 32;
        ctr[0] = c >> 24;
        ctr[1] = (c >> 16) & 0xFF;
        ctr[2] = (c >> 8) & 0xFF;
        ctr[3] = c & 0xFF;

        sha256_init(&sha2);
        sha256_update(&sha2, mgf, mgf_sz);
        if ((ctx->prm->m - i) >= 32) {
            sha256_final(&sha2, h + i);
        } else {
            sha256_final(&sha2, mgf);
            memcpy(h + i, mgf, ctx->prm->m - i);
        }
    }
}

//  Cat 3, 5: Hmsg(R, PK.seed, PK.root, M) =
//      MGF1-SHA-512(R || PK.seed || SHA-512(R || PK.seed || PK.root || M), m)

static void sha2_512_h_msg( slh_ctx_t *ctx,
                            uint8_t *h,
                            const uint8_t *r,
                            const uint8_t *m, size_t m_sz)
{
    sha512_t sha2;
    uint8_t mgf[32 + 32 + 64 + 4];
    size_t  n = ctx->prm->n;

    //  MGF1-SHA-512(R || PK.seed || ..
    memcpy(mgf, r, n);
    memcpy(mgf + n, ctx->pk_seed, n);

    //  SHA-512(R || PK.seed || PK.root || M)
    sha512_init(&sha2);
    sha512_update(&sha2, r, n);
    sha512_update(&sha2, ctx->pk_seed, n);
    sha512_update(&sha2, ctx->pk_root, n);
    sha512_update(&sha2, m, m_sz);
    sha512_final(&sha2, mgf + 2 * n);

    size_t mgf_sz = 2 * n + 64 + 4;
    uint8_t *ctr = mgf + mgf_sz - 4;

    //  MGF1 counter mode
    for (size_t i = 0; i < ctx->prm->m; i += 64) {
        uint32_t c = i / 64;
        ctr[0] = c >> 24;
        ctr[1] = (c >> 16) & 0xFF;
        ctr[2] = (c >> 8) & 0xFF;
        ctr[3] = c & 0xFF;

        sha512_init(&sha2);
        sha512_update(&sha2, mgf, mgf_sz);
        if ((ctx->prm->m - i) >= 64) {
            sha512_final(&sha2, h + i);
        } else {
            sha512_final(&sha2, mgf);
            memcpy(h + i, mgf, ctx->prm->m - i);
        }
    }
}

//  Cat 1, 3, 5: PRF(PK.seed, SK.seed, ADRS) =
//      Trunc_n(SHA-256(PK.seed || toByte(0, 64 − n) || ADRSc || SK.seed))

#include "test_rvkat.h"

static void sha256_prf_16(  slh_ctx_t *ctx, uint8_t *h)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;
    r32[S256_CHNS] = 0x00;
    r32[S256_TRIG] = 0x03;                  //  start it
    S256_WAIT

    block_copy_16(h, r32);
}

static void sha256_prf_24(  slh_ctx_t *ctx, uint8_t *h)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    block_copy_24(r32, ctx->sk_seed);

    r32[S256_CHNS] = 0x01;                  //  one hash
    r32[S256_TRIG] = 0x02;                  //  start it
    S256_WAIT

    block_copy_24(h, r32);
}

static void sha256_prf_32(  slh_ctx_t *ctx, uint8_t *h)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    block_copy_32(r32, ctx->sk_seed);

    r32[S256_CHNS] = 0x01;                  //  one hash
    r32[S256_TRIG] = 0x02;                  //  start it
    S256_WAIT

    block_copy_32(h, r32);
}

//  (PRFmsg is not optimized as it is used only once.)

//  Cat 1: PRFmsg(SK.prf, opt_rand, M) =
//      Trunc_n(HMAC-SHA-256(SK.prf, opt_rand || M))

static void sha256_prf_msg( slh_ctx_t *ctx,
                            uint8_t *h,
                            const uint8_t *opt_rand,
                            const uint8_t *m, size_t m_sz)
{
    unsigned i;
    sha256_t sha2;
    uint8_t pad[64], buf[32];
    size_t  n = ctx->prm->n;

    //  ipad
    memcpy(pad, ctx->sk_prf, n);
    for (i = 0; i < n; i++) {
        pad[i] ^= 0x36;
    }
    memset(pad + n, 0x36, 64 - n);

    sha256_init(&sha2);
    sha256_update(&sha2, pad, 64);
    sha256_update(&sha2, opt_rand, n);
    sha256_update(&sha2, m, m_sz);
    sha256_final(&sha2, buf);

    //  opad
    for (i = 0; i < 64; i++) {
        pad[i] ^= 0x36 ^ 0x5C;
    }

    sha256_init(&sha2);
    sha256_update(&sha2, pad, 64);
    sha256_update(&sha2, buf, 32);
    sha256_final_len(&sha2, h, n);
}

//  Cat 3, 5: PRFmsg(SK.prf, opt_rand, M) =
//      Trunc_n(HMAC-SHA-512(SK.prf, opt_rand || M))

static void sha512_prf_msg( slh_ctx_t *ctx,
                            uint8_t *h, const uint8_t *opt_rand,
                            const uint8_t *m, size_t m_sz)
{
    unsigned i;
    sha512_t sha2;
    uint8_t pad[128], buf[64];
    size_t  n = ctx->prm->n;

    //  ipad
    memcpy(pad, ctx->sk_prf, n);
    for (i = 0; i < n; i++) {
        pad[i] ^= 0x36;
    }
    memset(pad + n, 0x36, 128 - n);

    sha512_init(&sha2);
    sha512_update(&sha2, pad, 128);
    sha512_update(&sha2, opt_rand, n);
    sha512_update(&sha2, m, m_sz);
    sha512_final(&sha2, buf);

    //  opad
    for (i = 0; i < 128; i++) {
        pad[i] ^= 0x36 ^ 0x5C;
    }

    sha512_init(&sha2);
    sha512_update(&sha2, pad, 128);
    sha512_update(&sha2, buf, 64);
    sha512_final_len(&sha2, h, n);
}

//  Cat 1: T_l(PK.seed, ADRS, M1 ) =
//      Trunc_n(SHA-256(PK.seed || toByte(0, 64 − n) || ADRSc || Ml ))

static void sha256_tl_16(   slh_ctx_t *ctx,
                            uint8_t *h,
                            const uint8_t *m, size_t m_sz)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;
    volatile uint32_t *sr32 = &r32[S256_MSH2 + S256_MSGB];
    volatile uint8_t  *mr8  = (volatile uint8_t *)  &r32[S256_MSGB];
    const uint32_t *m32 = (const uint32_t *) m;
    const uint32_t rblk = 64;
    uint32_t i, j, bl;

    //  store total bit length
    bl = 8 * (rblk + 22 + m_sz);

    //  use f function padding feature
    block_copy_16(r32, m32);
    m += 16;
    m_sz -= 16;
    m32 += 4;

    r32[S256_CHNS] = 0x00;                  //  no hash
    r32[S256_TRIG] = 0x02;                  //  pad it
    S256_WAIT

    //  fill until end of block
    i = 22 + 16;
    for (j = i / 4; j < 16; j++) {
        sr32[j] = *m32++;
    }
    j = rblk - i;
    m += j;
    m_sz -= j;

    r32[S256_TRIG] = 0x01;                  //  compression
    S256_WAIT

    //  process full blocks

    j = m_sz / rblk;
    for (i = 0; i < j; i++) {

        *(sr32 - 1) = *(m32 - 1);
        block_copy_64(sr32, m32);
        m32 += 16;

        r32[S256_TRIG] = 0x01;              //  compression
        S256_WAIT
    }

    j = rblk * j;
    m += j;
    m_sz -= j;

    //  last partial block
    for (i = 0; i < m_sz; i++) {
        mr8[i] = m[i];
    }

    mr8[i++] = 0x80;
    if (i > rblk - 8) {
        while (i < rblk) {
            mr8[i++] = 0;
        }
        r32[S256_TRIG] = 0x01;              //  compression
        S256_WAIT
        i = 0;
    }

    //  encode length
    j = rblk;
    while (bl > 0) {
        mr8[--j] = bl & 0xFF;
        bl >>= 8;
    }
    //  clear stuff in between
    while (i < j) {
        mr8[i++] = 0;
    }

    r32[S256_TRIG] = 0x01;                  //  compression
    S256_WAIT

    block_copy_16(h, r32);
}

//  Cat 1: F(PK.seed, ADRS, M1 ) =
//      Trunc_n(SHA-256(PK.seed || toByte(0, 64 − n) || ADRSc || M1 ))

static void sha256_f_16(    slh_ctx_t *ctx,
                            uint8_t *h, const uint8_t *m1)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    block_copy_16(r32, m1);
    r32[S256_CHNS] = 0x01;                  //  one hash
    r32[S256_TRIG] = 0x02;                  //  start it
    S256_WAIT
    block_copy_16(h, r32);
}

static void sha256_f_24(    slh_ctx_t *ctx,
                            uint8_t *h, const uint8_t *m1)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    block_copy_24(r32, m1);
    r32[S256_CHNS] = 0x01;                  //  one hash
    r32[S256_TRIG] = 0x02;                  //  start it
    S256_WAIT

    block_copy_24(h, r32);
}

static void sha256_f_32(    slh_ctx_t *ctx,
                            uint8_t *h, const uint8_t *m1)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    block_copy_32(r32, m1);
    r32[S256_CHNS] = 0x01;                  //  one hash
    r32[S256_TRIG] = 0x02;                  //  start it
    S256_WAIT

    block_copy_32(h, r32);
}

//  Cat 1: H(PK.seed, ADRS, M2 ) =
//      Trunc_n(SHA-256(PK.seed || toByte(0, 64 − n) || ADRSc || M2 ))

static void sha256_h_16(    slh_ctx_t *ctx,
                            uint8_t *h,
                            const uint8_t *m1, const uint8_t *m2)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;
    volatile uint8_t  *mr8  = (volatile uint8_t *)  &r32[S256_MSGB];

    block_copy_16(r32, m1);
    r32[S256_CHNS] = 0x00;                  //  no hash
    r32[S256_TRIG] = 0x02;                  //  pad it
    S256_WAIT

    //  alignment with +2 shifting
    block_copy_16(&r32[S256_MSH2 + S256_MSGB + (22 + 16)/4], m2);

    //  new padding
    mr8[22 + 16 + 16] = 0x80;
    mr8[63] = 0xB0;                         //  8*(64+22+16+16) = 0x3B0 bits

    r32[S256_TRIG] = 0x01;                  //  hash it
    S256_WAIT

    block_copy_16(h, r32);
}

//  Cat 3, 5: Tl(PK.seed, ADRS, Ml ) =
//      Trunc_n(SHA-512(PK.seed || toByte(0, 128 − n) || ADRSc || Ml ))

static void sha512_tl( slh_ctx_t *ctx, uint8_t *h,
                        const uint8_t *m, size_t m_sz)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA512_BASE_ADDR;
    uint32_t *m32;
    volatile uint32_t *sr32 = &r32[ S512_MSGB + S512_MSH2 ];
    volatile uint8_t  *mr8  = (volatile uint8_t *)  &r32[S512_MSGB];
    const uint32_t rblk = 128;
    uint32_t i, j, bl, n = ctx->prm->n;

    //  store total bit length
    bl = 8 * (rblk + 22 + m_sz);

    //  64-byte first-block init
    block_copy_64(r32, &ctx->sha512_pk_seed.s);

    //  ADRSc
    adrsc_22(ctx, (uint8_t *) mr8);

    m32 = (uint32_t *) m;
    for (j = 22/4; j <= (rblk - 32) / 4; j += 8) {
        block_copy_32(sr32 + j, m32);
        m32 += 8;
    }
    while (j < rblk / 4) {
        sr32[j++] = *m32++;
    }

    j = rblk - 22;
    m += j;
    m_sz -= j;

    //  first block
    r32[S512_TRIG] = 0x01;                  //  compression
    S512_WAIT

    //  process full blocks
    j = m_sz / rblk;
    for (i = 0; i < j; i++) {

        *(sr32 - 1) = *(m32 - 1);
        block_copy_64(sr32, m32);
        block_copy_64(sr32 + 16, m32 + 16);
        m32 += 32;

        r32[S512_TRIG] = 0x01;              //  compression
        S512_WAIT
    }
    j = rblk * j;
    m += j;
    m_sz -= j;

    i = 0;

    //  final part
    *(sr32 - 1) = *(m32 - 1);
    j = m_sz / 4 - 1;
    for (i = 0; i < j; i++) {
        sr32[i] = m32[i];
    }
    j = 4 * j + 2;
    i = j;
    m += j;
    m_sz -= j;

    while (m_sz > 0) {
        mr8[i++] = *m++;
        m_sz--;
    }
    mr8[i++] = 0x80;                        //  padding

    if (i > rblk - 16) {
        while (i < rblk) {
            mr8[i++] = 0;
        }
        r32[S512_TRIG] = 0x01;              //  compression
        S512_WAIT
        i = 0;
    }

    while ((i & 3) != 0) {                  //  alignment
        mr8[i++] = 0;
    }
    j = i / 4 + S512_MSGB;
    while (j < S512_MEND - 1){
        r32[j++] = 0;
    }

    //  encode length
    r32[S512_MEND - 1] = rev8_be32(bl);

    r32[S512_TRIG] = 0x01;                  //  compression
    S512_WAIT

    block_copy_n(h, r32, n);
}

//  Cat 3, 5: H(PK.seed, ADRS, M2 ) =
//      Trunc_n(SHA-512(PK.seed || toByte(0, 128 − n) || ADRSc || M2 ))

static void sha512_h( slh_ctx_t *ctx, uint8_t *h,
                        const uint8_t *m1, const uint8_t *m2)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA512_BASE_ADDR;
    volatile uint32_t *sr32 = &r32[ S512_MSGB + S512_MSH2 ];
    volatile uint8_t  *mr8  = (volatile uint8_t *)  &r32[S512_MSGB];
    const uint32_t rblk = 128;
    uint32_t j, bl;
    uint32_t n = ctx->prm->n;
    uint32_t n4 = n / 4;

    //  store total bit length
    bl = 8 * (rblk + 22 + 2 * n);

    //  64-byte first-block lemn
    block_copy_64(r32, &ctx->sha512_pk_seed.s);

    //  ADRSc
    adrsc_22(ctx, (uint8_t *) mr8);

    //  m1 || m2
    j = (22 / 4);
    block_copy_n(sr32 + j, m1, n);
    j += n4;
    block_copy_n(sr32 + j, m2, n);
    j += n4;
    sr32[j++] = 0x80;

    //  fill in between
    while (j < (rblk - 6) / 4) {
        sr32[ j++ ] = 0;
    }

    //  encode length
    r32[S512_MEND - 1] = rev8_be32(bl);

    //  start it
    r32[S512_TRIG] = 0x01;      //  compression
    S512_WAIT

    block_copy_n(h, r32, n);
}

//  create a context

static void sha2_mk_ctx(slh_ctx_t *ctx,
                        const uint8_t *pk, const uint8_t *sk,
                        const slh_param_t *prm)
{
    ctx->prm = prm;
    size_t n = prm->n;

    ctx->prm = prm;     //  store fixed parameters

    if (sk != NULL) {
        memcpy( ctx->sk_seed,   sk,         n );
        memcpy( ctx->sk_prf,    sk + n,     n );
        memcpy( ctx->pk_seed,   sk + 2*n,   n );
        memcpy( ctx->pk_root,   sk + 3*n,   n );
    } else  if (pk != NULL) {
        memcpy( ctx->pk_seed,   pk,         n );
        memcpy( ctx->pk_root,   pk + n,     n );
    }

    //  eliminate the first compression function
    uint8_t buf[128-24];
    memset(buf, 0x00, sizeof(buf));
    sha256_init(&ctx->sha256_pk_seed);
    sha256_update(&ctx->sha256_pk_seed, ctx->pk_seed, n);
    sha256_update(&ctx->sha256_pk_seed, buf, 64 - n);

    if (n > 16) {
        sha512_init(&ctx->sha512_pk_seed);
        sha512_update(&ctx->sha512_pk_seed, ctx->pk_seed, n);
        sha512_update(&ctx->sha512_pk_seed, buf, 128 - n);
    }

    //  set up SLotH
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;
    r32[S256_SECN] = n;
    r32[S256_CHNS] = 0;
    block_copy_32(r32 + S256_SEED, &ctx->sha256_pk_seed);
    block_copy_32(r32 + S256_SKSD, &ctx->sk_seed);
    ctx->adrs = (volatile adrs_t *) &r32[S256_ADRS];
}

//  === Chaining function used in WOTS+
//  Algorithm 4: chain(X, i, s, PK.seed, ADRS)

static void sha256_chain_16(slh_ctx_t *ctx, uint8_t *tmp,
                            const uint8_t *x, uint32_t i, uint32_t s)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    //  nop?
    if (s == 0) {
        block_copy_16(tmp, x);
        return;
    }

    //  init
    block_copy_16(r32, x);

    //  adrs_set_hash_address(adrs, i);
    ctx->adrs->u8[31] = i;

    //  go
    r32[S256_CHNS] = s;
    r32[S256_TRIG] = 0x02;      //  start it
    S256_WAIT

    block_copy_16(tmp, r32);
}

static void sha256_chain_24(slh_ctx_t *ctx, uint8_t *tmp,
                            const uint8_t *x, uint32_t i, uint32_t s)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    //  nop?
    if (s == 0) {
        block_copy_24(tmp, x);
        return;
    }

    //  init
    block_copy_24(r32, x);

    //  adrs_set_hash_address(adrs, i);
    ctx->adrs->u8[31] = i;

    //  go
    r32[S256_CHNS] = s;
    r32[S256_TRIG] = 0x02;      //  start it
    S256_WAIT

    block_copy_24(tmp, r32);
}

static void sha256_chain_32(slh_ctx_t *ctx, uint8_t *tmp,
                            const uint8_t *x, uint32_t i, uint32_t s)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    //  nop?
    if (s == 0) {
        block_copy_32(tmp, x);
        return;
    }

    //  init
    block_copy_32(r32, x);

    //  adrs_set_hash_address(adrs, i);
    ctx->adrs->u8[31] = i;

    //  go
    r32[S256_CHNS] = s;
    r32[S256_TRIG] = 0x02;      //  start it
    S256_WAIT

    block_copy_32(tmp, r32);
}

//  Combination WOTS PRF + Chain

static void sha256_wots_chain_16( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    adrs_set_type(ctx, ADRS_WOTS_PRF);
    adrs_set_tree_index(ctx, 0);
    r32[S256_CHNS] = s;
    r32[S256_TRIG] = 0x03;
    S256_WAIT

    block_copy_16(tmp, r32);
}

static void sha256_wots_chain_24( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    adrs_set_type(ctx, ADRS_WOTS_PRF);
    adrs_set_tree_index(ctx, 0);
    r32[S256_CHNS] = s;
    r32[S256_TRIG] = 0x03;
    S256_WAIT

    block_copy_24(tmp, r32);
}

static void sha256_wots_chain_32( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    adrs_set_type(ctx, ADRS_WOTS_PRF);
    adrs_set_tree_index(ctx, 0);
    r32[S256_CHNS] = s;
    r32[S256_TRIG] = 0x03;
    S256_WAIT

    block_copy_32(tmp, r32);
}

//  PRF + optional F for FORS

static void sha256_fors_hash_16( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    adrs_set_type(ctx, ADRS_FORS_PRF);
    adrs_set_tree_height(ctx, 0);
    r32[S256_CHNS] = s;
    r32[S256_TRIG] = 0x03;
    S256_WAIT

    block_copy_16(tmp, r32);
}

static void sha256_fors_hash_24( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    adrs_set_type(ctx, ADRS_FORS_PRF);
    adrs_set_tree_height(ctx, 0);
    r32[S256_CHNS] = s;
    r32[S256_TRIG] = 0x03;
    S256_WAIT

    block_copy_24(tmp, r32);
}

static void sha256_fors_hash_32( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    volatile uint32_t *r32  = (volatile uint32_t *) SHA256_BASE_ADDR;

    adrs_set_type(ctx, ADRS_FORS_PRF);
    adrs_set_tree_height(ctx, 0);
    r32[S256_CHNS] = s;
    r32[S256_TRIG] = 0x03;
    S256_WAIT

    block_copy_32(tmp, r32);
}

//  10.2.   SLH-DSA Using SHA2 for Security Category 1

const slh_param_t slh_dsa_sha2_128s = { .alg_id ="SLH-DSA-SHA2-128s",
    .n= 16, .h= 63, .d= 7, .hp= 9, .a= 12, .k= 14, .lg_w= 4, .m= 30,
    .mk_ctx= sha2_mk_ctx, .chain= sha256_chain_16,
    .wots_chain= sha256_wots_chain_16, .fors_hash= sha256_fors_hash_16,
    .h_msg= sha2_256_h_msg, .prf= sha256_prf_16, .prf_msg= sha256_prf_msg,
    .h_f= sha256_f_16, .h_h= sha256_h_16, .h_t= sha256_tl_16
};

const slh_param_t slh_dsa_sha2_128f = { .alg_id ="SLH-DSA-SHA2-128f",
    .n= 16, .h= 66, .d= 22, .hp= 3, .a= 6, .k= 33, .lg_w= 4, .m= 34,
    .mk_ctx= sha2_mk_ctx, .chain= sha256_chain_16,
    .wots_chain= sha256_wots_chain_16, .fors_hash= sha256_fors_hash_16,
    .h_msg= sha2_256_h_msg, .prf= sha256_prf_16, .prf_msg= sha256_prf_msg,
    .h_f= sha256_f_16, .h_h= sha256_h_16, .h_t= sha256_tl_16
};

//  10.3.   SLH-DSA Using SHA2 for Security Categories 3 and 5

const slh_param_t slh_dsa_sha2_192s = { .alg_id ="SLH-DSA-SHA2-192s",
    .n= 24, .h= 63, .d= 7, .hp= 9, .a= 14, .k= 17, .lg_w= 4, .m= 39,
    .mk_ctx= sha2_mk_ctx, .chain= sha256_chain_24,
    .wots_chain= sha256_wots_chain_24, .fors_hash= sha256_fors_hash_24,
    .h_msg= sha2_512_h_msg, .prf= sha256_prf_24, .prf_msg= sha512_prf_msg,
    .h_f= sha256_f_24, .h_h= sha512_h, .h_t= sha512_tl
};

const slh_param_t slh_dsa_sha2_192f = { .alg_id ="SLH-DSA-SHA2-192f",
    .n= 24, .h= 66, .d= 22, .hp= 3, .a= 8, .k= 33, .lg_w= 4, .m= 42,
    .mk_ctx= sha2_mk_ctx, .chain= sha256_chain_24,
    .wots_chain= sha256_wots_chain_24, .fors_hash= sha256_fors_hash_24,
    .h_msg= sha2_512_h_msg, .prf= sha256_prf_24, .prf_msg= sha512_prf_msg,
    .h_f= sha256_f_24, .h_h= sha512_h, .h_t= sha512_tl
};

const slh_param_t slh_dsa_sha2_256s = { .alg_id ="SLH-DSA-SHA2-256s",
    .n= 32, .h= 64, .d= 8, .hp= 8, .a= 14, .k= 22, .lg_w= 4, .m= 47,
    .mk_ctx= sha2_mk_ctx, .chain= sha256_chain_32,
    .wots_chain= sha256_wots_chain_32, .fors_hash= sha256_fors_hash_32,
    .h_msg= sha2_512_h_msg, .prf= sha256_prf_32, .prf_msg= sha512_prf_msg,
    .h_f= sha256_f_32, .h_h= sha512_h, .h_t= sha512_tl
};

const slh_param_t slh_dsa_sha2_256f = { .alg_id ="SLH-DSA-SHA2-256f",
    .n= 32, .h= 68, .d= 17, .hp= 4, .a= 9, .k= 35, .lg_w= 4, .m= 49,
    .mk_ctx= sha2_mk_ctx, .chain= sha256_chain_32,
    .wots_chain= sha256_wots_chain_32, .fors_hash= sha256_fors_hash_32,
    .h_msg= sha2_512_h_msg, .prf= sha256_prf_32, .prf_msg= sha512_prf_msg,
    .h_f= sha256_f_32, .h_h= sha512_h, .h_t= sha512_tl
};

//  SLOTH_SHA256
#endif
