

#include "slh_dsa.h"
#include "slh_ctx.h"
#include "slh_adrs.h"
#include <assert.h>

//  === Internal

//  helper functions to compute "len = len1 + len2"

static inline uint32_t get_len1(const slh_param_t *prm)
{
    return ((8 * prm->n + prm->lg_w - 1) / prm->lg_w);
}

static inline uint32_t get_len2(const slh_param_t *prm)
{
#ifdef NDEBUG
    (void) prm;
#endif
    //  Appedix B:
    //  "When lg_w = 4 and 9 <= n <= 136, the value of len2 will be 3."
    assert(prm->lg_w == 4 && prm->n >= 9 && prm->n <= 136);
    return 3;
}
static inline uint32_t get_len(const slh_param_t *prm)
{
    return  get_len1(prm) + get_len2(prm);
}

//  Return signature size in bytes for parameter set *prm.
size_t slh_sig_sz(const slh_param_t *prm)
{
    return  (1 + prm->k*(1 + prm->a) + prm->h + prm->d * get_len(prm)) * prm->n;
}

//  === Compute the base 2**b representation of X.
//  Algorithm 3: base_2b(X, b, out_len)

static inline size_t base_2b(   uint32_t *v, const uint8_t *x,
                                uint32_t b, size_t v_len)
{
    size_t i, j;
    uint32_t l, t, m;

    j = 0;
    l = 0;
    t = 0;
    m = (1 << b) - 1;
    for (i = 0; i < v_len; i++) {
        while (l < b) {
            t = (t << 8) + x[j++];
            l += 8;
        }
        l -= b;
        v[i] = (t >> l) & m;
    }
    return j;
}

//  little bit faster version for b = 4

static inline size_t base_16(   uint32_t *v, const uint8_t *x, int v_len)
{
    int i, j, l, t;

    j = 0;
    for (i = 0; i < v_len - 2; i += 2) {
        t = x[j++];
        v[i]     = t >> 4;
        v[i + 1] = t & 0xF;
    }

    l = 0;
    t = 0;
    for (; i < v_len; i++) {
        while (l < 4) {
            t = (t << 8) + x[j++];
            l += 8;
        }
        l -= 4;
        v[i] = (t >> l) & 0xF;
    }
    return j;
}

//  === Chaining function used in WOTS+
//  Algorithm 4: chain(X, i, s, PK.seed, ADRS)
//  (see prm->chain)

//  === Generate a WOTS+ public key.
//  Algorithm 5: wots_PKgen(SK.seed, PK.seed, ADRS)
//  (see xmms_node)

//  === Generate a WOTS+ signature on an n-byte message.
//  Algorithm 6: wots_sign(M, SK.seed, PK.seed, ADRS)

//  (wots_csum is a shared helper function for algorithms 6 and 7)
static void wots_csum(uint32_t *vm, const uint8_t *m, const slh_param_t *prm)
{
    uint32_t csum, i, t;
    uint32_t len1, len2;
    uint8_t buf[4];

    len1 = get_len1(prm);
    len2 = get_len2(prm);

    //base_2b(vm, m, prm->lg_w, len1);
    base_16(vm, m, len1);

    csum = 0;
    t = (1 << prm->lg_w) - 1;
    for (i = 0; i < len1; i++) {
        csum += t - vm[i];
    }
    csum <<= (8 - ((len2 * prm->lg_w) & 7)) & 7;

    t = (len2 * prm->lg_w + 7) / 8;
    memset(buf, 0, sizeof(buf));
    slh_tobyte(buf, csum, t);

    //base_2b(&vm[len1], buf, prm->lg_w, len2);
    base_16(&vm[len1], buf, len2);
}

static size_t wots_sign(slh_ctx_t *ctx, uint8_t *sig, const uint8_t *m)
{
    const slh_param_t *prm = ctx->prm;
    uint32_t i, len;
    uint32_t vm[SLH_MAX_LEN];
    size_t n = prm->n;

    len = get_len(prm);
    wots_csum(vm, m, prm);

    for (i = 0; i < len; i++) {
        adrs_set_chain_address(ctx, i);
        prm->wots_chain(ctx, sig, vm[i]);
        sig += n;
    }
    return n * len;
}

//  === Compute a WOTS+ public key from a message and its signature.
//  Algorithm 7: wots_PKFromSig(sig, M, PK.seed, ADRS)

static void wots_pk_from_sig(   slh_ctx_t *ctx, uint8_t *pk,
                                const uint8_t *sig,
                                const uint8_t *m)
{
    const slh_param_t *prm = ctx->prm;
    uint32_t i, t, len;
    uint32_t vm[SLH_MAX_LEN];
    uint8_t tmp[SLH_MAX_LEN * SLH_MAX_N];
    size_t n = prm->n;
    size_t tmp_sz;

    wots_csum(vm, m, prm);

    len = get_len(prm);
    t = 15; // (1 << prm->lg_w) - 1;
    tmp_sz = 0;
    for (i = 0; i < len; i++) {
        adrs_set_chain_address(ctx, i);
        prm->chain( ctx, tmp + tmp_sz, sig + tmp_sz, vm[i], t - vm[i]);
        tmp_sz += n;
    }

    adrs_set_type_and_clear_not_kp(ctx, ADRS_WOTS_PK);
    prm->h_t(ctx, pk, tmp, tmp_sz);
}

//  === Compute the root of a Merkle subtree of WOTS+ public keys.
//  Algorithm 8: xmss_node(SK.seed, i, z, PK.seed, ADRS)

static void xmss_node(  slh_ctx_t *ctx, uint8_t *node,
                        uint32_t i, uint32_t z)
{
    const slh_param_t *prm = ctx->prm;
    uint32_t j, k;
    int p;
    uint8_t *h0, h[SLH_MAX_HP][SLH_MAX_N];
    uint8_t tmp[SLH_MAX_LEN * SLH_MAX_N];
    uint8_t *sk;
    size_t n = prm->n;
    size_t len = get_len(prm);

    p = -1;
    i <<= z;
    for (j = 0; j < (1u << z); j++) {

        adrs_set_key_pair_address(ctx, i);

        //  === Generate a WOTS+ public key.
        //  Algorithm 5: wots_PKgen(SK.seed, PK.seed, ADRS)
        sk  = tmp;
        for (k = 0; k < len; k++) {
            adrs_set_chain_address(ctx, k);
            prm->wots_chain(ctx, sk, 15);   //  w-1 =  (1 << prm->lg_w) - 1;
            sk += n;
        }
        adrs_set_type_and_clear_not_kp(ctx, ADRS_WOTS_PK);
        h0 = p >= 0 ? h[p] : node;
        p++;
        prm->h_t(ctx, h0, tmp, len * n);

        //  this xmss_node() implementation is non-recursive
        for (k = 0; (j >> k) & 1; k++) {
            adrs_set_type_and_clear(ctx, ADRS_TREE);
            adrs_set_tree_height(ctx, k + 1);
            adrs_set_tree_index(ctx, i >> (k + 1));
            p--;
            h0 = p >= 1 ? h[p - 1] : node;
            prm->h_h(ctx, h0, h0, h[p]);
        }
        i++;        //  advance index
    }
}

//  === Generate an XMSS signature.
//  Algorithm 9: xmss_sign(M, SK.seed, idx, PK.seed, ADRS)

static size_t xmss_sign(slh_ctx_t *ctx, uint8_t *sx, const uint8_t *m,
                        uint32_t idx)
{

    const slh_param_t *prm = ctx->prm;
    uint32_t j, k;
    uint8_t *auth;
    size_t sx_sz = 0;
    size_t n = prm->n;

    sx_sz = get_len(prm) * n;
    auth = sx + sx_sz;

    for (j = 0; j < prm->hp; j++) {
        k = (idx >> j) ^ 1;
        xmss_node(ctx, auth, k, j);
        auth += n;
    }
    sx_sz += prm->hp * n;

    adrs_set_type_and_clear_not_kp(ctx, ADRS_WOTS_HASH);
    adrs_set_key_pair_address(ctx, idx);
    wots_sign(ctx, sx, m);

    return sx_sz;
}

//  === Compute an XMSS public key from an XMSS signature.
//  Algorithm 10: xmss_PKFromSig(idx, SIGXMSS, M, PK.seed, ADRS)

static void xmss_pk_from_sig(   slh_ctx_t *ctx, uint8_t *root, uint32_t idx,
                                const uint8_t *sig, const uint8_t *m)
{

    const slh_param_t *prm = ctx->prm;
    uint32_t k;
    const uint8_t *auth;
    size_t n = prm->n;

    adrs_set_type_and_clear_not_kp(ctx, ADRS_WOTS_HASH);
    adrs_set_key_pair_address(ctx, idx);

    wots_pk_from_sig(ctx, root, sig, m);
    adrs_set_type_and_clear(ctx, ADRS_TREE);

    auth = sig + (get_len(prm) * n);

    for (k = 0; k < prm->hp; k++) {

        adrs_set_tree_height(ctx, k + 1);
        adrs_set_tree_index(ctx, idx >> (k + 1));

        if (((idx >> k) & 1) == 0) {
            prm->h_h(ctx, root, root, auth);
        } else {
            prm->h_h(ctx, root, auth, root);
        }
        auth += n;
    }
}


//  === Generate a hypertree signature.
//  Algorithm 11: ht_sign(M, SK.seed, PK.seed, idx_tree, idx_leaf )

static size_t ht_sign(  slh_ctx_t *ctx, uint8_t *sh, uint8_t *m,
                        uint64_t i_tree, uint32_t i_leaf)
{

    const slh_param_t *prm = ctx->prm;
    uint32_t j;
    size_t sx_sz;

    adrs_zero(ctx);
    adrs_set_tree_address(ctx, i_tree);
    sx_sz = xmss_sign(ctx, sh, m, i_leaf);

    for (j = 1; j < prm->d; j++) {
        xmss_pk_from_sig(ctx, m, i_leaf, sh, m);
        sh += sx_sz;

        i_leaf = i_tree & ((1 << prm->hp) - 1);
        i_tree >>= prm->hp;
        adrs_set_layer_address(ctx, j);
        adrs_set_tree_address(ctx, i_tree);
        xmss_sign( ctx, sh, m, i_leaf);
    }

    return sx_sz * prm->d;
}


//  === Verify a hypertree signature.
//  Algorithm 12: ht_verify(M, SIG_HT, PK.seed, idx_tree, idx_leaf, PK.root)

static bool ht_verify(  slh_ctx_t *ctx, const uint8_t *m,
                        const uint8_t *sig_ht,
                        uint64_t i_tree, uint32_t i_leaf)
{
    const slh_param_t *prm = ctx->prm;
    uint32_t i, j;
    uint8_t node[SLH_MAX_N];
    size_t st_sz;

    adrs_zero(ctx);
    adrs_set_tree_address(ctx, i_tree);

    xmss_pk_from_sig(ctx, node, i_leaf, sig_ht, m);

    st_sz = (prm->hp + get_len(prm)) * prm->n;
    for (j = 1; j < prm->d; j++) {
        i_leaf = i_tree & ((1 << prm->hp) - 1);
        i_tree >>= prm->hp;
        adrs_set_layer_address(ctx, j);
        adrs_set_tree_address(ctx, i_tree);
        sig_ht += st_sz;
        xmss_pk_from_sig(ctx, node, i_leaf, sig_ht, node);
    }

    uint8_t t;
    t = 0;
    for (i = 0; i < prm->n; i++) {
        t |= node[i] ^ ctx->pk_root[i];
    }
    return t == 0;
}

//  === Generate a FORS private-key value.
//  Algorithm 13: fors_SKgen(SK.seed, PK.seed, ADRS, idx)

//  ( see prm->fors_hash() )

//  === Compute the root of a Merkle subtree of FORS public values.
//  Algorithm 14: fors_node(SK.seed, i, z, PK.seed, ADRS)

static void fors_node(  slh_ctx_t *ctx, uint8_t *node,
                        uint32_t i, uint32_t z)
{
    const slh_param_t *prm = ctx->prm;
    uint8_t h[SLH_MAX_A][SLH_MAX_N], *h0;
    uint32_t j, k;
    int p;

    p = -1;
    i <<= z;
    for (j = 0; j < (1u << z); j++) {


        //  fors_SKgen() + hash
        adrs_set_tree_index(ctx, i);
        h0 = p >= 0 ? h[p] : node;
        p++;
        prm->fors_hash(ctx, h0, 1);

        //  this fors_node() implementation is non-recursive
        for (k = 0; (j >> k) & 1; k++) {
            adrs_set_tree_height(ctx, k + 1);
            adrs_set_tree_index(ctx, i >> (k + 1));
            p--;
            h0 = p > 0 ? h[p - 1] : node;
            prm->h_h(ctx, h0, h0, h[p]);
        }
        i++;        //  advance index
    }
}


//  === Generate a FORS signature.
//  Algorithm 15: fors_sign(md, SK.seed, PK.seed, ADRS)

static size_t fors_sign(slh_ctx_t *ctx, uint8_t *sf, const uint8_t *md)
{
    const slh_param_t *prm = ctx->prm;
    uint32_t i, j, s;
    uint32_t vi[SLH_MAX_K];
    size_t  n = prm->n;

    assert(SLH_MAX_K >= prm->k);
    base_2b(vi, md, prm->a, prm->k);

    for (i = 0; i < prm->k; i++) {

        //  fors_SKgen()
        adrs_set_tree_index(ctx, (i << prm->a) + vi[i]);
        prm->fors_hash(ctx, sf, 0);
        sf += n;

        for (j = 0; j < prm->a; j++) {
            s = (vi[i] >> j) ^ 1;
            fors_node(  ctx, sf, (i << (prm->a - j)) + s, j);
            sf += n;
        }
    }
    return n * prm->k * (1 + prm->a);
}

//  === Compute a FORS public key from a FORS signature.
//  Algorithm 16: fors_pkFromSig(SIGFORS , md, PK.seed, ADRS)

static void fors_pk_from_sig(   slh_ctx_t *ctx, uint8_t *pk,
                                const uint8_t *sf, const uint8_t *md)
{

    const slh_param_t *prm = ctx->prm;
    uint32_t i, j, idx;
    uint32_t vi[SLH_MAX_K];
    uint8_t root[SLH_MAX_K * SLH_MAX_N];
    uint8_t *node;
    size_t  n = prm->n;

    base_2b(vi, md, prm->a, prm->k);

    node = root;
    for (i = 0; i < prm->k; i++) {

        adrs_set_tree_height(ctx, 0);

        idx = (i << prm->a) + vi[i];
        adrs_set_tree_index(ctx, idx);

        prm->h_f(ctx, node, sf);
        sf += n;

        for (j = 0; j < prm->a; j++) {

            adrs_set_tree_height(ctx, j + 1);
            adrs_set_tree_index(ctx, idx >> (j + 1));

            if (((vi[i] >> j) & 1) == 0) {
                prm->h_h(ctx, node, node, sf);
            } else {
                prm->h_h(ctx, node, sf, node);
            }
            sf += n;
        }
        node += n;
    }

    adrs_set_type_and_clear_not_kp(ctx, ADRS_FORS_ROOTS);
    prm->h_t(ctx, pk, root, prm->k * n);
}

//  === Public API

//  Return standard identifier string for parameter set *prm, or NULL.
const char *slh_alg_id(const slh_param_t *prm)
{
    return prm->alg_id;
}

//  Return public (verification) key size in bytes for parameter set *prm.
size_t slh_pk_sz(const slh_param_t *prm)
{
    return 2 * prm->n;
}

//  Return private (signing) key size in bytes for parameter set *prm.
size_t slh_sk_sz(const slh_param_t *prm)
{
    return 4 * prm->n;
}

//  === Generate an SLH-DSA key pair.
//  Algorithm 17: slh_keygen()

int slh_keygen(uint8_t *pk, uint8_t *sk,
               int (*rbg)(uint8_t *x, size_t xlen), const slh_param_t *prm)
{

    slh_ctx_t   ctx;
    uint8_t     pk_root[SLH_MAX_N];
    size_t      n = prm->n;

    rbg(sk, 3 * n);                     //  SK.seed || SK.prf || PK.seed
    memcpy(pk, sk + 2 * n, n);          //  PK.seed
    memset(sk + 3 * n, 0x00, n);        //  PK.root not generated yet
    prm->mk_ctx(&ctx, NULL, sk, prm);   //  fill in partial

    adrs_zero(&ctx);
    adrs_set_layer_address(&ctx, prm->d - 1);
    xmss_node(&ctx, pk_root, 0, prm->hp);

    //  fill pk_root
    memcpy(sk + 3 * n, pk_root, n);
    memcpy(pk + n, pk_root, n);
    return 0;
}

//  === Generate an SLH-DSA signature.
//  Algorithm 18: slh_sign(M, SK)

//  (Shared helper function for algorithms 18 and 19.)

static void split_digest(uint64_t *i_tree, uint32_t *i_leaf,
                         const uint8_t *digest, const slh_param_t *prm)
{
    size_t      md_sz       = (prm->k * prm->a + 7) / 8;
    const uint8_t *pi_tree  = digest + md_sz;
    size_t      i_tree_sz   = (prm->h - prm->hp + 7) / 8;
    *i_tree     = slh_toint(pi_tree, i_tree_sz);
    size_t      i_leaf_sz   = (prm->hp + 7) / 8;
    const uint8_t *pi_leaf  = pi_tree + i_tree_sz;
    *i_leaf     = slh_toint(pi_leaf, i_leaf_sz);
    if ((prm->h - prm->hp) != 64) {
        *i_tree     &= (UINT64_C(1) << (prm->h - prm->hp)) - UINT64_C(1);
    }
    *i_leaf     &= (1 << prm->hp) - 1;
}

//  Core signing function that just takes in "digest" and an already
//  initialized secret key context. *sig points to signature after randomizer.
//  Returns the length of |SIG_FORS + SIG_HT| written at *sig.

size_t slh_do_sign( slh_ctx_t *ctx, uint8_t *sig, const uint8_t *digest)
{
    const uint8_t   *md = digest;
    uint64_t i_tree = 0;
    uint32_t i_leaf = 0;
    uint8_t pk_fors[SLH_MAX_N];
    size_t sig_sz;

    split_digest(&i_tree, &i_leaf, digest, ctx->prm);

    adrs_zero(ctx);
    adrs_set_tree_address(ctx, i_tree);
    adrs_set_type_and_clear_not_kp(ctx, ADRS_FORS_TREE);
    adrs_set_key_pair_address(ctx, i_leaf);

    //  SIG_FORS
    sig_sz  = fors_sign(ctx, sig, md);
    fors_pk_from_sig(ctx, pk_fors, sig, md);

    //  SIG_HT
    sig +=  sig_sz;
    sig_sz  += ht_sign(ctx, sig, pk_fors, i_tree, i_leaf);

    return sig_sz;
}

size_t slh_sign(uint8_t *sig, const uint8_t *m, size_t m_sz,
                const uint8_t *sk, int (*rbg)(uint8_t *x, size_t xlen),
                const slh_param_t *prm)
{
    slh_ctx_t   ctx;
    uint8_t opt_rand[SLH_MAX_N];
    uint8_t digest[SLH_MAX_M];

    //  set up secret key etc
    prm->mk_ctx(&ctx, NULL, sk, prm);

/*
#ifdef SLH_DETERMINISTIC
    printf("*** SLH_DETERMINISTIC enabled");
    memcpy(opt_rand, ctx.pk_seed, prm->n);
#else
    printf("*** SLH_DETERMINISTIC disabled");
    rbg(opt_rand, prm->n);
#endif
*/
    memcpy(opt_rand, ctx.pk_seed, prm->n);
    //printf("*** SLH_DETERMINISTIC enabled");

    //  randomized hashing; R
    uint8_t *r  = sig;
    size_t  sig_sz = prm->n;
    prm->prf_msg(&ctx, r, opt_rand, m, m_sz);
    prm->h_msg(&ctx, digest, r, m, m_sz);

    //  create FORS and HT signature parts
    sig_sz += slh_do_sign(&ctx, sig + sig_sz, digest);

    return sig_sz;
}

//  === Verify an SLH-DSA signature.
//  Algorithm 19: slh_verify(M, SIG, PK)

bool slh_verify(const uint8_t *m, size_t m_sz,
                const uint8_t *sig, const uint8_t *pk,
                const slh_param_t *prm)
{

    slh_ctx_t   ctx;
    uint8_t digest[SLH_MAX_M];
    uint8_t pk_fors[SLH_MAX_N];

    const uint8_t   *r          = sig;
    const uint8_t   *sig_fors   = sig + prm->n;
    const uint8_t   *sig_ht     = sig + ((1 + prm->k*(1 + prm->a)) * prm->n);

    prm->mk_ctx(&ctx, pk, NULL, prm);
    prm->h_msg(&ctx, digest, r, m, m_sz);

    const uint8_t   *md = digest;
    uint64_t        i_tree = 0;
    uint32_t        i_leaf = 0;
    split_digest(&i_tree, &i_leaf, digest, prm);

    adrs_zero(&ctx);
    adrs_set_tree_address(&ctx, i_tree);
    adrs_set_type_and_clear_not_kp(&ctx, ADRS_FORS_TREE);
    adrs_set_key_pair_address(&ctx, i_leaf);

    fors_pk_from_sig(&ctx, pk_fors, sig_fors, md);

    bool sig_ok = ht_verify(&ctx, pk_fors, sig_ht, i_tree, i_leaf);
    return sig_ok;
}

