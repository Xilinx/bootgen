/******************************************************************************
* ML-DSA-87 C API Implementation
* 
* This file MUST be compiled with C++20 (GCC 10+).
* It wraps the ml-dsa C++20 library and exposes C-compatible functions.
******************************************************************************/

#include "ml_dsa_api.h"
#include <ml_dsa/ml_dsa_87.hpp>
#include <randomshake/randomshake.hpp>
#include <array>
#include <cstring>
#include <span>

extern "C" {

void mldsa87_keygen(
    const uint8_t *seed,
    uint8_t *pubkey,
    uint8_t *seckey)
{
    std::span<const uint8_t, ml_dsa_87::KeygenSeedByteLen> seed_span(seed, ml_dsa_87::KeygenSeedByteLen);
    std::span<uint8_t, ml_dsa_87::PubKeyByteLen> pubkey_span(pubkey, ml_dsa_87::PubKeyByteLen);
    std::span<uint8_t, ml_dsa_87::SecKeyByteLen> seckey_span(seckey, ml_dsa_87::SecKeyByteLen);
    
    ml_dsa_87::keygen(seed_span, pubkey_span, seckey_span);
}

int mldsa87_sign(
    const uint8_t *rnd,
    const uint8_t *seckey,
    const uint8_t *msg,
    size_t msg_len,
    uint8_t *sig)
{
    std::span<const uint8_t, ml_dsa_87::SigningSeedByteLen> rnd_span(rnd, ml_dsa_87::SigningSeedByteLen);
    std::span<const uint8_t, ml_dsa_87::SecKeyByteLen> seckey_span(seckey, ml_dsa_87::SecKeyByteLen);
    std::span<const uint8_t> msg_span(msg, msg_len);
    std::span<const uint8_t> ctx_span;  // empty context
    std::span<uint8_t, ml_dsa_87::SigByteLen> sig_span(sig, ml_dsa_87::SigByteLen);
    
    return ml_dsa_87::sign(rnd_span, seckey_span, msg_span, ctx_span, sig_span) ? 1 : 0;
}

int mldsa87_verify(
    const uint8_t *pubkey,
    const uint8_t *msg,
    size_t msg_len,
    const uint8_t *sig)
{
    std::span<const uint8_t, ml_dsa_87::PubKeyByteLen> pubkey_span(pubkey, ml_dsa_87::PubKeyByteLen);
    std::span<const uint8_t> msg_span(msg, msg_len);
    std::span<const uint8_t> ctx_span;  // empty context
    std::span<const uint8_t, ml_dsa_87::SigByteLen> sig_span(sig, ml_dsa_87::SigByteLen);
    
    return ml_dsa_87::verify(pubkey_span, msg_span, ctx_span, sig_span) ? 1 : 0;
}

void mldsa_random_bytes(uint8_t *out, size_t len)
{
    static randomshake::randomshake_t<128> csprng;
    
    // Generate in 32-byte chunks
    std::array<uint8_t, 32> chunk;
    while (len > 0) {
        csprng.generate(chunk);
        size_t copy_len = (len < 32) ? len : 32;
        std::memcpy(out, chunk.data(), copy_len);
        out += copy_len;
        len -= copy_len;
    }
}

} // extern "C"
