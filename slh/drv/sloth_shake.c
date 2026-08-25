//  sloth_shake.c
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === SLotH: Accelerated functions for instantiation of SLH-DSA with SHAKE

#ifdef SLOTH_KECCAK

#include "slh_ctx.h"
#include "slh_adrs.h"
#include "sha3_api.h"
#include "sloth_hal.h"
#include <string.h>

//  compression function instatiation for sha3_api.c

void keccak_f1600(void *v)
{
    uint32_t *v32 = (uint32_t *) v;
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
    int i;

    for (i = 0; i < 50; i++) {              //  actually slow part 1
        r32[i] = v32[i];
    }
    r32[KECC_STOP] = 0x74;                  //  stop position
    r32[KECC_TRIG] = 0x01;                  //  start it
    KECC_WAIT

    for (i = 0; i < 50; i++) {              //  actually slow part 2
        v32[i] = r32[i];
    }
}

//  === 10.1.   SLH-DSA Using SHAKE

//  Hmsg(R, PK.seed, PK.root, M) = SHAKE256(R || PK.seed || PK.root || M, 8m)

static void shake_h_msg( slh_ctx_t *ctx,
                            uint8_t *h,
                            const uint8_t *r,
                            const uint8_t *m, size_t m_sz)
{
    sha3_ctx_t sha3;
    size_t  n = ctx->prm->n;

    shake256_init(&sha3);
    shake_update(&sha3, r, n);
    shake_update(&sha3, ctx->pk_seed, n);
    shake_update(&sha3, ctx->pk_root, n);
    shake_update(&sha3, m, m_sz);

    shake_out(&sha3, h, ctx->prm->m);
}

//  F(PK.seed, ADRS, M1 ) = SHAKE256(PK.seed || ADRS || M1, 8n)

static void shake_f_16( slh_ctx_t *ctx,
                            uint8_t *h,
                            const uint8_t *m1)
{
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    block_copy_16(r32, m1);

    r32[KECC_CHNS]  =   1;                  //  one iteration
    KECC_WAIT

    block_copy_16(h, (const void *) r32);
}

static void shake_f_24( slh_ctx_t *ctx,
                            uint8_t *h,
                            const uint8_t *m1)
{
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    block_copy_24(r32, m1);

    r32[KECC_CHNS]  =   1;                  //  one iteration
    KECC_WAIT

    block_copy_24(h, (const void *) r32);
}

static void shake_f_32( slh_ctx_t *ctx,
                            uint8_t *h,
                            const uint8_t *m1)
{
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    block_copy_32(r32, m1);

    r32[KECC_CHNS]  =   1;                  //  one iteration
    KECC_WAIT

    block_copy_32(h, (const void *) r32);
}

//  PRF(PK.seed, SK.seed, ADRS) = SHAKE256(PK.seed || ADRS || SK.seed, 8n)

static void shake_prf_16(slh_ctx_t *ctx, uint8_t *h)
{
#ifdef SLOTH_KECTI3
    //  Masked PRF
    volatile uint32_t   *r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;
    int i;

    //  copy address
    block_copy_32(&r32[KTI3_ADRS], ctx->adrs);
    r32[KTI3_CHNS]  =   0x40;
    KTI3_WAIT

    //  collapse the final result
    for (i = 0; i < 4; i++) {
        ((uint32_t *) h)[i] =
            r32[KTI3_MEMA + i] ^ r32[KTI3_MEMB + i] ^ r32[KTI3_MEMC + i];
    }
#else
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    //  PRF
    r32[KECC_CHNS]  =   0x40;
    KECC_WAIT
    block_copy_16(h, r32);
#endif
}

static void shake_prf_24(slh_ctx_t *ctx, uint8_t *h)
{
#ifdef SLOTH_KECTI3
    //  Masked PRF
    volatile uint32_t   *r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;
    int i;

    //  copy address
    block_copy_32(&r32[KTI3_ADRS], ctx->adrs);
    r32[KTI3_CHNS]  =   0x40;
    KTI3_WAIT

    //  collapse the final result
    for (i = 0; i < 6; i++) {
        ((uint32_t *) h)[i] =
            r32[KTI3_MEMA + i] ^ r32[KTI3_MEMB + i] ^ r32[KTI3_MEMC + i];
    }
#else
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    //  PRF
    r32[KECC_CHNS]  =   0x40;
    KECC_WAIT

    block_copy_24(h, r32);
#endif
}

static void shake_prf_32(slh_ctx_t *ctx, uint8_t *h)
{
#ifdef SLOTH_KECTI3
    //  Masked PRF
    volatile uint32_t   *r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;
    int i;

    //  copy address
    block_copy_32(&r32[KTI3_ADRS], ctx->adrs);
    r32[KTI3_CHNS]  =   0x40;
    KTI3_WAIT

    //  collapse the final result
    for (i = 0; i < 8; i++) {
        ((uint32_t *) h)[i] =
            r32[KTI3_MEMA + i] ^ r32[KTI3_MEMB + i] ^ r32[KTI3_MEMC + i];
    }
#else
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    //  PRF
    r32[KECC_CHNS]  =   0x40;
    KECC_WAIT

    block_copy_32(h, r32);
#endif
}


//  PRFmsg (SK.prf, opt_rand, M) = SHAKE256(SK.prf || opt_rand || M, 8n)

static void shake_prf_msg(  slh_ctx_t *ctx,
                                uint8_t *h,
                                const uint8_t *opt_rand,
                                const uint8_t *m, size_t m_sz)
{
    sha3_ctx_t sha3;
    size_t  n = ctx->prm->n;

    shake256_init(&sha3);
    shake_update(&sha3, ctx->sk_prf, n);
    shake_update(&sha3, opt_rand, n);
    shake_update(&sha3, m, m_sz);

    shake_out(&sha3, h, n);
}

//  T_l(PK.seed, ADRS, M ) = SHAKE256(PK.seed || ADRS || Ml, 8n)

static void shake_t( slh_ctx_t *ctx, uint8_t *h,
                        const uint8_t *m, size_t m_sz)
{
    const uint32_t rblk = (1600 - 2 * 256) / 32;    //  SHAKE256 block size
    const uint32_t *m32 = (const uint32_t *) m;
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
    size_t  i, j, n = ctx->prm->n;

    m_sz /= 4;

    for (i = 0; i < n / 4; i++) {
        r32[i] = *m32++;
    }
    m_sz -= i;
    r32[KECC_CHNS]  =   0x80;               //  generate the padding only
    KECC_WAIT

    i += 8 + n / 4;

    //  initial block (no need to xor)
    while (i < rblk && m_sz > 0) {
        r32[i++] = *m32++;
        m_sz--;
    }
    if (i >= rblk) {
        r32[KECC_TRIG] = 0x01;              //  absorb
        KECC_WAIT
        i = 0;
    }

    //  full blocks
    while (m_sz > rblk) {
        for (j = 0; j < rblk; j++) {
            r32[j] ^= m32[j];
        }
        r32[KECC_TRIG] = 0x01;              //  absorb
        KECC_WAIT
        m32 += rblk;
        m_sz -= rblk;
    }

    //  last part
    while (m_sz > 0) {
        r32[i++] ^= *m32++;
        if (i >= rblk) {
            r32[KECC_TRIG] = 0x01;          //  absorb
            KECC_WAIT
            i = 0;
        }
        m_sz--;
    }
    r32[i] ^= 0x1F;                         //  SHAKE256 padding
    r32[rblk - 1] ^= 1 << 31;
    r32[KECC_TRIG] = 0x01;                  //  squeeze
    KECC_WAIT

    for (i = 0; i < n / 4; i++) {
        ((uint32_t *) h)[i] = r32[i];
    }
}


//  H(PK.seed, ADRS, M2 ) = SHAKE256(PK.seed || ADRS || M2, 8n)

static void shake_h_16( slh_ctx_t *ctx, uint8_t *h,
                            const uint8_t *m1, const uint8_t *m2)
{
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    block_copy_16(r32, m1);

    r32[KECC_CHNS]  =   0x80;               //  generate the padding only
    KECC_WAIT

    block_copy_16(&r32[16], m2);            //  after PK_seed, ADRS, and m1
    r32[20]         =   0x1F;               //  shake padding

    r32[KECC_TRIG]  =   0x01;               //  start it
    KECC_WAIT

    block_copy_16(h, r32);
}

static void shake_h_24( slh_ctx_t *ctx, uint8_t *h,
                            const uint8_t *m1, const uint8_t *m2)
{
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    block_copy_24(r32, m1);

    r32[KECC_CHNS]  =   0x80;               //  generate the padding only
    KECC_WAIT

    block_copy_24(&r32[20], m2);            //  after PK_seed, ADRS, and m1
    r32[26]         =   0x1F;               //  shake padding

    r32[KECC_TRIG]  =   0x01;               //  start it
    KECC_WAIT

    block_copy_32(h, r32);
}

static void shake_h_32( slh_ctx_t *ctx, uint8_t *h,
                            const uint8_t *m1, const uint8_t *m2)
{
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    block_copy_32(r32, m1);

    r32[KECC_CHNS]  =   0x80;               //  generate the padding only
    KECC_WAIT
    //  (1 cycle only)

    block_copy_32(&r32[24], m2);            //  after PK_seed, ADRS, and m1
    r32[32]         =   0x1F;               //  shake padding

    r32[KECC_TRIG]  =   0x01;               //  start it
    KECC_WAIT

    block_copy_32(h, r32);
}

//  create a context

static void shake_mk_ctx(slh_ctx_t *ctx,
                         const uint8_t *pk, const uint8_t *sk,
                         const slh_param_t *prm)
{
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

    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    //  load keys in hardware
    r32[KECC_SECN]  =   n;
    for (int j = 0; j < n/4; j++) {
        r32[KECC_SEED + j] = ((uint32_t *) ctx->pk_seed)[j];
        r32[KECC_SKSD + j] = ((uint32_t *) ctx->sk_seed)[j];
    }
    ctx->adrs = (volatile adrs_t *) &r32[KECC_ADRS];

#ifdef SLOTH_KECTI3
    r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;
    r32[KTI3_SECN]  =   n;

    for (int j = 0; j < n/4; j++) {
        r32[KTI3_SEED + j] = ((uint32_t *) ctx->pk_seed)[j];

        //  secret key load; unfortunately the source is unmasked
        r32[KTI3_SKSA + j] = ((uint32_t *) ctx->sk_seed)[j];
        r32[KTI3_SKSB + j] = 0;             //  Share B
        r32[KTI3_SKSC + j] = 0;             //  Share C
    }
#endif
}

//  === Chaining function used in WOTS+
//  Algorithm 4: chain(X, i, s, PK.seed, ADRS)

static void shake_chain_16( slh_ctx_t *ctx, uint8_t *tmp,
                            const uint8_t *x, uint32_t i, uint32_t s)
{
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    if (s == 0) {                           //  no-op
        block_copy_16(tmp, x);
        return;
    }
    block_copy_16(r32, x);
    ctx->adrs->u8[31] = i;                  //  set_hash_address(i)
    r32[KECC_CHNS]  =   s;                  //  auto-chain ..
    KECC_WAIT

    block_copy_16(tmp, r32);
}

static void shake_chain_24( slh_ctx_t *ctx, uint8_t *tmp,
                            const uint8_t *x, uint32_t i, uint32_t s)
{
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    if (s == 0) {                           //  no-op
        block_copy_24(tmp, x);
        return;
    }
    block_copy_24(r32, x);
    ctx->adrs->u8[31] = i;                  //  set_hash_address(i)
    r32[KECC_CHNS]  =   s;                  //  auto-chain ..
    KECC_WAIT

    block_copy_24(tmp, r32);
}

static void shake_chain_32( slh_ctx_t *ctx, uint8_t *tmp,
                            const uint8_t *x, uint32_t i, uint32_t s)
{
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    if (s == 0) {                           //  no-op
        block_copy_32(tmp, x);
        return;
    }
    block_copy_32(r32, x);
    ctx->adrs->u8[31] = i;                  //  set_hash_address(i)
    r32[KECC_CHNS]  =   s;                  //  auto-chain ..
    KECC_WAIT

    block_copy_32(tmp, r32);
}

#ifdef SLOTH_KECTI3

//  collapse three shares together and write to destination

static inline void kecti3_collapse(void *d, int n)
{
    volatile uint32_t   *r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;
    int i;

    //  collapse the final result
    for (i = 0; i < (n / 4); i++) {
        ((uint32_t *) d)[i] =
            r32[KTI3_MEMA + i] ^ r32[KTI3_MEMB + i] ^ r32[KTI3_MEMC + i];
    }
}
#endif

//  Combination WOTS PRF + Chain

static void shake_wots_chain_16( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    //  PRF secret key
    adrs_set_type(ctx, ADRS_WOTS_PRF);
    adrs_set_tree_index(ctx, 0);

#ifdef SLOTH_KECTI3
    //  Masked PRF + Chain
    volatile uint32_t   *r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;

    block_copy_32(&r32[KTI3_ADRS], ctx->adrs);
    r32[KTI3_CHNS]  =   0x40 + s;
    KTI3_WAIT
    kecti3_collapse(tmp, 16);
#else
    //  Unmasked PRF + Chain
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
    r32[KECC_CHNS]  =   0x40 + s;
    KECC_WAIT
    block_copy_16(tmp, r32);
#endif
}

static void shake_wots_chain_24( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    //  PRF secret key
    adrs_set_type(ctx, ADRS_WOTS_PRF);
    adrs_set_tree_index(ctx, 0);

#ifdef SLOTH_KECTI3
    //  Masked PRF + Chain
    volatile uint32_t   *r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;

    //  copy address
    block_copy_32(&r32[KTI3_ADRS], ctx->adrs);
    r32[KTI3_CHNS]  =   0x40 + s;
    KTI3_WAIT

    kecti3_collapse(tmp, 24);
#else
    //  Unmasked PRF + Chain
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
    r32[KECC_CHNS]  =   0x40 + s;
    KECC_WAIT

    block_copy_24(tmp, r32);
#endif
}

static void shake_wots_chain_32( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    //  PRF secret key
    adrs_set_type(ctx, ADRS_WOTS_PRF);
    adrs_set_tree_index(ctx, 0);

#ifdef SLOTH_KECTI3
    //  Masked PRF + Chain
    volatile uint32_t   *r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;

    //  copy address
    block_copy_32(&r32[KTI3_ADRS], ctx->adrs);
    r32[KTI3_CHNS]  =   0x40 + s;
    KTI3_WAIT

    kecti3_collapse(tmp, 32);
#else
    //  Unmasked PRF + Chain
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;
    r32[KECC_CHNS]  =   0x40 + s;
    KECC_WAIT

    block_copy_32(tmp, r32);
#endif
}

//  Combination FORS PRF + F (if s == 1)

static void shake_fors_hash_16( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    adrs_set_type(ctx, ADRS_FORS_PRF);
    adrs_set_tree_height(ctx, 0);

#ifdef SLOTH_KECTI3
    //  Masked PRF + Chain
    volatile uint32_t   *r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;

    //  copy address
    block_copy_32(&r32[KTI3_ADRS], ctx->adrs);
    r32[KTI3_CHNS]  =   0x40 + s;
    KTI3_WAIT
    kecti3_collapse(tmp, 16);
    adrs_set_type(ctx, ADRS_FORS_TREE);
#else
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    //  Unmasked PRF + F
    r32[KECC_CHNS]  =   0x40 + s;
    KECC_WAIT
    block_copy_16(tmp, r32);
#endif
}

static void shake_fors_hash_24( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    adrs_set_type(ctx, ADRS_FORS_PRF);
    adrs_set_tree_height(ctx, 0);

#ifdef SLOTH_KECTI3
    //  Masked PRF + Chain
    volatile uint32_t   *r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;

    //  copy address
    block_copy_32(&r32[KTI3_ADRS], ctx->adrs);
    r32[KTI3_CHNS]  =   0x40 + s;
    KTI3_WAIT
    kecti3_collapse(tmp, 24);
    adrs_set_type(ctx, ADRS_FORS_TREE);
#else
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    //  Unmasked PRF + F
    r32[KECC_CHNS]  =   0x40 + s;
    KECC_WAIT
    block_copy_24(tmp, r32);
#endif
}

static void shake_fors_hash_32( slh_ctx_t *ctx, uint8_t *tmp, uint32_t s)
{
    adrs_set_type(ctx, ADRS_FORS_PRF);
    adrs_set_tree_height(ctx, 0);

#ifdef SLOTH_KECTI3
    //  Masked PRF + Chain
    volatile uint32_t   *r32 = (volatile uint32_t *) KECTI3_BASE_ADDR;

    //  copy address
    block_copy_32(&r32[KTI3_ADRS], ctx->adrs);
    r32[KTI3_CHNS]  =   0x40 + s;
    KTI3_WAIT
    kecti3_collapse(tmp, 32);
    adrs_set_type(ctx, ADRS_FORS_TREE);
#else
    volatile uint32_t   *r32 = (volatile uint32_t *) KECCAK_BASE_ADDR;

    //  Unmasked PRF + F
    r32[KECC_CHNS]  =   0x40 + s;
    KECC_WAIT
    block_copy_32(tmp, r32);
#endif
}

//  parameter sets

const slh_param_t slh_dsa_shake_128s = {    .alg_id ="SLH-DSA-SHAKE-128s",
    .n= 16, .h= 63, .d= 7, .hp= 9, .a= 12, .k= 14, .lg_w= 4, .m= 30,
    .mk_ctx= shake_mk_ctx, .chain= shake_chain_16,
    .wots_chain= shake_wots_chain_16, .fors_hash= shake_fors_hash_16,
    .h_msg= shake_h_msg, .prf= shake_prf_16, .prf_msg= shake_prf_msg,
    .h_f= shake_f_16, .h_h= shake_h_16, .h_t= shake_t
};

const slh_param_t slh_dsa_shake_128f = {    .alg_id ="SLH-DSA-SHAKE-128f",
    .n= 16, .h= 66, .d= 22, .hp= 3, .a= 6, .k= 33, .lg_w= 4, .m= 34,
    .mk_ctx= shake_mk_ctx, .chain= shake_chain_16,
    .wots_chain= shake_wots_chain_16, .fors_hash= shake_fors_hash_16,
    .h_msg= shake_h_msg, .prf= shake_prf_16, .prf_msg= shake_prf_msg,
    .h_f= shake_f_16, .h_h= shake_h_16, .h_t= shake_t
};

const slh_param_t slh_dsa_shake_192s = {    .alg_id ="SLH-DSA-SHAKE-192s",
    .n= 24, .h= 63, .d= 7, .hp= 9, .a= 14, .k= 17, .lg_w= 4, .m= 39,
    .mk_ctx= shake_mk_ctx, .chain= shake_chain_24,
    .wots_chain= shake_wots_chain_24, .fors_hash= shake_fors_hash_24,
    .h_msg= shake_h_msg, .prf= shake_prf_24, .prf_msg= shake_prf_msg,
    .h_f= shake_f_24, .h_h= shake_h_24, .h_t= shake_t
};

const slh_param_t slh_dsa_shake_192f = {    .alg_id ="SLH-DSA-SHAKE-192f",
    .n= 24, .h= 66, .d= 22, .hp= 3, .a= 8, .k= 33, .lg_w= 4, .m= 42,
    .mk_ctx= shake_mk_ctx, .chain= shake_chain_24,
    .wots_chain= shake_wots_chain_24, .fors_hash= shake_fors_hash_24,
    .h_msg= shake_h_msg, .prf= shake_prf_24, .prf_msg= shake_prf_msg,
    .h_f= shake_f_24, .h_h= shake_h_24, .h_t= shake_t
};

const slh_param_t slh_dsa_shake_256s = {    .alg_id ="SLH-DSA-SHAKE-256s",
    .n= 32, .h= 64, .d= 8, .hp= 8, .a= 14, .k= 22, .lg_w= 4, .m= 47,
    .mk_ctx= shake_mk_ctx, .chain= shake_chain_32,
    .wots_chain= shake_wots_chain_32, .fors_hash= shake_fors_hash_32,
    .h_msg= shake_h_msg, .prf= shake_prf_32, .prf_msg= shake_prf_msg,
    .h_f= shake_f_32, .h_h= shake_h_32, .h_t= shake_t
};

const slh_param_t slh_dsa_shake_256f = {    .alg_id ="SLH-DSA-SHAKE-256f",
    .n= 32, .h= 68, .d= 17, .hp= 4, .a= 9, .k= 35, .lg_w= 4, .m= 49,
    .mk_ctx= shake_mk_ctx, .chain= shake_chain_32,
    .wots_chain= shake_wots_chain_32, .fors_hash= shake_fors_hash_32,
    .h_msg= shake_h_msg, .prf= shake_prf_32, .prf_msg= shake_prf_msg,
    .h_f= shake_f_32, .h_h= shake_h_32, .h_t= shake_t
};

//  SLOTH_KECCAK
#endif
