//  sha3_api.h
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === FIPS 202: SHA-3 hash and SHAKE eXtensible Output Functions (XOF)

#ifndef _SHA3_API_H_
#define _SHA3_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

typedef struct {                            //  state context
    union {
        uint8_t b[200];                     //  8-bit bytes
        uint64_t d[25];                     //  64-bit words
    } st;
    int pt, rsiz, mdlen;
} sha3_ctx_t;

//  compute a SHA-3 hash "md" of "mdlen" bytes from data in "in"
void *sha3(uint8_t *md, int mdlen, const void *in, size_t inlen);

//  incremental interfece
void sha3_init(sha3_ctx_t *c, int mdlen);  //   mdlen = hash output in bytes
void sha3_update(sha3_ctx_t *c, const void *data, size_t len);
void sha3_final(sha3_ctx_t *c, uint8_t *md);  // digest goes to md

//  SHAKE128 and SHAKE256 extensible-output functions
#define shake128_init(c) sha3_init(c, 16)
#define shake256_init(c) sha3_init(c, 32)
#define shake_update sha3_update

//  squeeze output (can call repeat)
void shake_out(sha3_ctx_t *c, uint8_t *out, size_t lem);

//  core permutation
void keccak_f1600(void *st);

#ifdef __cplusplus
}
#endif

#endif  //  _SHA3_API_H_
