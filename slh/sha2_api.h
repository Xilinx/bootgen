//  sha2_api.h
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === FIPS 180-4 (SHA-2) -- traditional "MD" type hash API interface.

#ifndef _SHA2_API_H_
#define _SHA2_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

//  === Single-call hash wrappers ===

//  SHA2-224: Compute 28-byte hash to "md" from "in" which has "mlen" bytes.
void sha2_224(uint8_t *md, const void *m, size_t mlen);

//  SHA2-256: Compute 32-byte hash to "md" from "in" which has "mlen" bytes.
void sha2_256(uint8_t *md, const void *m, size_t mlen);

//  SHA2-384: Compute 48-byte hash to "md" from "in" which has "mlen" bytes.
void sha2_384(uint8_t *md, const void *m, size_t mlen);

//  SHA2-512: Compute 64-byte hash to "md" from "in" which has "mlen" bytes.
void sha2_512(uint8_t *md, const void *m, size_t mlen);

//  === incremental interface ===

typedef struct {
    uint32_t s[8 + 24];
    size_t i, len;
} sha256_t;

typedef sha256_t sha224_t;

typedef struct {
    uint64_t s[8 + 24];
    size_t i, len;
} sha512_t;

typedef sha512_t sha384_t;

//  shaNNN_init(ctx): Initialize context for hashing.
void sha256_init(sha256_t *sha);
void sha224_init(sha256_t *sha);
void sha512_init(sha512_t *sha);
void sha384_init(sha512_t *sha);

//  shaNNN_update(ctx, m, mlen): Include "m" of "mlen" bytes in hash.
void sha256_update(sha256_t *sha, const uint8_t *m, size_t mlen);
#define sha224_update(sha, m, mlen) sha256_update(sha, m, mlen)
void sha512_update(sha512_t *sha, const uint8_t *m, size_t mlen);
#define sha384_update(sha, m, mlen) sha512_update(sha, m, mlen)

//  shaNNN_final(ctx, h): Finalize hash to "h", and clear the state.
void sha256_final_len(sha256_t *sha, uint8_t *h, size_t hlen);
#define sha256_final(sha, h) sha256_final_len(sha, h, 32)
#define sha224_final(sha, h) sha256_final_len(sha, h, 28)
void sha512_final_len(sha512_t *sha, uint8_t *h, size_t hlen);
#define sha512_final(sha, h) sha512_final_len(sha, h, 64)
#define sha384_final(sha, h) sha512_final_len(sha, h, 48)

//  final padding
void sha256_final_pad(sha256_t *sha);
void sha512_final_pad(sha512_t *sha);

//  copy state
void sha256_copy(sha256_t *dst, const sha256_t *src);
void sha512_copy(sha512_t *dst, const sha512_t *src);

//  compression functions
void sha256_compress(void *v);
void sha512_compress(void *v);

#ifdef __cplusplus
}
#endif

#endif  //  _SHA2_API_H_
