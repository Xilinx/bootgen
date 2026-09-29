#ifndef PQC_OPENSSL_H
#define PQC_OPENSSL_H

#include <stddef.h>
#include <stdint.h>
#include <openssl/opensslv.h>
#if OPENSSL_VERSION_NUMBER < 0x30500000L
#error Bootgen ML-DSA/SLH-DSA require OpenSSL 3.5 or newer
#endif

#ifdef __cplusplus
extern "C" {
#endif

int openssl_slhdsa_keygen(const char *algorithm,
                          const uint8_t *seed,
                          size_t seed_len,
                          uint8_t *public_key,
                          size_t public_key_len,
                          uint8_t *private_key,
                          size_t private_key_len);

size_t openssl_slhdsa_sign(const char *algorithm,
                           const uint8_t *private_key,
                           size_t private_key_len,
                           const uint8_t *message,
                           size_t message_len,
                           uint8_t *signature,
                           size_t signature_capacity);

int openssl_slhdsa_verify(const char *algorithm,
                          const uint8_t *public_key,
                          size_t public_key_len,
                          const uint8_t *message,
                          size_t message_len,
                          const uint8_t *signature,
                          size_t signature_len);

#ifdef __cplusplus
}
#endif

#endif
