//  slh_dsa.h
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === FIPS 205 (ipd) Stateless Hash-Based Digital Signature Standard.

#ifndef _SLH_DSA_H_
#define _SLH_DSA_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct  slh_param_s slh_param_t;

//  === SLH-DSA parameter sets
extern const slh_param_t slh_dsa_sha2_128s;
extern const slh_param_t slh_dsa_shake_128s;
extern const slh_param_t slh_dsa_sha2_128f;
extern const slh_param_t slh_dsa_shake_128f;
extern const slh_param_t slh_dsa_sha2_192s;
extern const slh_param_t slh_dsa_shake_192s;
extern const slh_param_t slh_dsa_sha2_192f;
extern const slh_param_t slh_dsa_shake_192f;
extern const slh_param_t slh_dsa_sha2_256s;
extern const slh_param_t slh_dsa_shake_256s;
extern const slh_param_t slh_dsa_sha2_256f;
extern const slh_param_t slh_dsa_shake_256f;

//  === API

//  Return standard identifier string for parameter set *prm, or NULL.
const char *slh_alg_id(const slh_param_t *prm);

//  Return public (verification) key size in bytes for parameter set *prm.
size_t slh_pk_sz(const slh_param_t *prm);

//  Return private (signing) key size in bytes for parameter set *prm.
size_t slh_sk_sz(const slh_param_t *prm);

//  Return signature size in bytes for parameter set *prm.
size_t slh_sig_sz(const slh_param_t *prm);

//  Generate an SLH-DSA key pair.
int slh_keygen( uint8_t *pk, uint8_t *sk,
                int (*rbg)(uint8_t *x, size_t xlen),
                const slh_param_t *prm);

//  Generate a SLH-DSA signature.
size_t slh_sign(uint8_t *sig,
                const uint8_t *m, size_t m_sz,
                const uint8_t *sk,
                int (*rbg)(uint8_t *x, size_t xlen),
                const slh_param_t *prm);

//  Verify an SLH-DSA signature.
bool slh_verify(const uint8_t *m, size_t m_sz,
                const uint8_t *sig, const uint8_t *pk,
                const slh_param_t *prm);

#ifdef __cplusplus
}
#endif

//  _SLH_DSA_H_
#endif
