//  slh_dsa.c

//  === FIPS 205 Stateless Hash-Based Digital Signature Standard.
//
//  The key, signature and digest sizes are derived from the FIPS 205
//  Section 11 parameter sets. All hashing, key generation, signing and
//  verification is performed by the OpenSSL SLH-DSA implementation.

#include "slh_dsa.h"
#include "pqc_openssl.h"

//  Largest security parameter n across all parameter sets.
#define SLH_MAX_N   32

//  === FIPS 205 parameter sets

struct slh_param_s {
    const char  *alg_id;    //  Algorithm name
    uint32_t    n;          //  Security level / hash size { 16,24,32 }.
    uint32_t    h;          //  Bits h used to select FORS key.
    uint32_t    d;          //  Number of hypertree layers d.
    uint32_t    hp;         //  Merkle tree height h' (XMSS).
    uint32_t    a;          //  String length t = 2**a (FORS).
    uint32_t    k;          //  Number of strings (FORS).
    uint32_t    lg_w;       //  Number of bits in chain index (WOTS+)
    uint32_t    m;          //  Length in bytes of message digest.
};

const slh_param_t slh_dsa_sha2_128s = {     "SLH-DSA-SHA2-128s",
    16, 63, 7, 9, 12, 14, 4, 30 };

const slh_param_t slh_dsa_shake_128s = {    "SLH-DSA-SHAKE-128s",
    16, 63, 7, 9, 12, 14, 4, 30 };

const slh_param_t slh_dsa_sha2_128f = {     "SLH-DSA-SHA2-128f",
    16, 66, 22, 3, 6, 33, 4, 34 };

const slh_param_t slh_dsa_shake_128f = {    "SLH-DSA-SHAKE-128f",
    16, 66, 22, 3, 6, 33, 4, 34 };

const slh_param_t slh_dsa_sha2_192s = {     "SLH-DSA-SHA2-192s",
    24, 63, 7, 9, 14, 17, 4, 39 };

const slh_param_t slh_dsa_shake_192s = {    "SLH-DSA-SHAKE-192s",
    24, 63, 7, 9, 14, 17, 4, 39 };

const slh_param_t slh_dsa_sha2_192f = {     "SLH-DSA-SHA2-192f",
    24, 66, 22, 3, 8, 33, 4, 42 };

const slh_param_t slh_dsa_shake_192f = {    "SLH-DSA-SHAKE-192f",
    24, 66, 22, 3, 8, 33, 4, 42 };

const slh_param_t slh_dsa_sha2_256s = {     "SLH-DSA-SHA2-256s",
    32, 64, 8, 8, 14, 22, 4, 47 };

const slh_param_t slh_dsa_shake_256s = {    "SLH-DSA-SHAKE-256s",
    32, 64, 8, 8, 14, 22, 4, 47 };

const slh_param_t slh_dsa_sha2_256f = {     "SLH-DSA-SHA2-256f",
    32, 68, 17, 4, 9, 35, 4, 49 };

const slh_param_t slh_dsa_shake_256f = {    "SLH-DSA-SHAKE-256f",
    32, 68, 17, 4, 9, 35, 4, 49 };

//  === Internal

//  helper functions to compute "len = len1 + len2"

static inline uint32_t get_len1(const slh_param_t *prm)
{
    return ((8 * prm->n + prm->lg_w - 1) / prm->lg_w);
}

static inline uint32_t get_len2(const slh_param_t *prm)
{
    (void) prm;
    //  Appedix B:
    //  "When lg_w = 4 and 9 <= n <= 136, the value of len2 will be 3."
    return 3;
}

static inline uint32_t get_len(const slh_param_t *prm)
{
    return  get_len1(prm) + get_len2(prm);
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

//  Return signature size in bytes for parameter set *prm.
size_t slh_sig_sz(const slh_param_t *prm)
{
    return  (1 + prm->k*(1 + prm->a) + prm->h + prm->d * get_len(prm)) * prm->n;
}

//  === Generate an SLH-DSA key pair.

int slh_keygen(uint8_t *pk, uint8_t *sk,
               int (*rbg)(uint8_t *x, size_t xlen), const slh_param_t *prm)
{
    uint8_t seed[3 * SLH_MAX_N];
    if (pk == NULL || sk == NULL || rbg == NULL || prm == NULL) {
        return -1;
    }
    const size_t seed_sz = 3 * prm->n;
    if (rbg(seed, seed_sz) != 0) {
        return -1;
    }

    return openssl_slhdsa_keygen(
               slh_alg_id(prm), seed, seed_sz,
               pk, slh_pk_sz(prm), sk, slh_sk_sz(prm)) == 1 ? 0 : -1;
}

//  === Generate an SLH-DSA signature.

size_t slh_sign(uint8_t *sig, const uint8_t *m, size_t m_sz,
                const uint8_t *sk, int (*rbg)(uint8_t *x, size_t xlen),
                const slh_param_t *prm)
{
    (void)rbg;
    if (sig == NULL || (m == NULL && m_sz != 0) ||
        sk == NULL || prm == NULL) {
        return 0;
    }
    return openssl_slhdsa_sign(
        slh_alg_id(prm), sk, slh_sk_sz(prm), m, m_sz,
        sig, slh_sig_sz(prm));
}

//  === Verify an SLH-DSA signature.

bool slh_verify(const uint8_t *m, size_t m_sz,
                const uint8_t *sig, const uint8_t *pk,
                const slh_param_t *prm)
{
    if ((m == NULL && m_sz != 0) || sig == NULL ||
        pk == NULL || prm == NULL) {
        return false;
    }
    return openssl_slhdsa_verify(
               slh_alg_id(prm), pk, slh_pk_sz(prm), m, m_sz,
               sig, slh_sig_sz(prm)) == 1;
}
