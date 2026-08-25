/******************************************************************************
* ML-DSA-87 C API
* 
* C-compatible interface to the ML-DSA cryptographic library.
* This library is compiled with C++20 and linked as a static library,
* allowing it to be used from C++14 code (like gradle builds).
*
* Pattern follows LMS library (lms-hash-sigs/hss.h)
******************************************************************************/

#ifndef ML_DSA_API_H
#define ML_DSA_API_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ML-DSA-87 Constants */
#define MLDSA87_SEED_LEN        32
#define MLDSA87_PUBKEY_LEN      2592
#define MLDSA87_SECKEY_LEN      4896
#define MLDSA87_SIG_LEN         4627

/**
 * Generate ML-DSA-87 keypair
 * 
 * @param seed      32-byte random seed
 * @param pubkey    Output: public key (2592 bytes)
 * @param seckey    Output: secret key (4896 bytes)
 */
void mldsa87_keygen(
    const uint8_t *seed,
    uint8_t *pubkey,
    uint8_t *seckey
);

/**
 * Sign a message with ML-DSA-87
 * 
 * @param rnd       32-byte random seed (zeros for deterministic)
 * @param seckey    Secret key (4896 bytes)
 * @param msg       Message to sign
 * @param msg_len   Message length
 * @param sig       Output: signature (4627 bytes)
 * @return          1 on success, 0 on failure
 */
int mldsa87_sign(
    const uint8_t *rnd,
    const uint8_t *seckey,
    const uint8_t *msg,
    size_t msg_len,
    uint8_t *sig
);

/**
 * Verify ML-DSA-87 signature
 * 
 * @param pubkey    Public key (2592 bytes)
 * @param msg       Message
 * @param msg_len   Message length
 * @param sig       Signature (4627 bytes)
 * @return          1 if valid, 0 if invalid
 */
int mldsa87_verify(
    const uint8_t *pubkey,
    const uint8_t *msg,
    size_t msg_len,
    const uint8_t *sig
);

/**
 * Generate random bytes (uses RandomShake CSPRNG)
 * 
 * @param out       Output buffer
 * @param len       Number of bytes to generate
 */
void mldsa_random_bytes(uint8_t *out, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* ML_DSA_API_H */
