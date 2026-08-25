#pragma once
/******************************************************************************
* ML-DSA-87 Compatibility Header (C++14)
* 
* Provides the same API as the original C++20 ml_dsa_87 namespace.
* Internally calls the pre-built libmldsa.a C functions.
******************************************************************************/

#include <array>
#include <cstdint>
#include <cstddef>
#include <cstring>

extern "C" {
#include "ml_dsa_api.h"
}

namespace ml_dsa_87 {

// Constants matching the original ml-dsa library
static constexpr size_t KeygenSeedByteLen = MLDSA87_SEED_LEN;     // 32
static constexpr size_t PubKeyByteLen = MLDSA87_PUBKEY_LEN;       // 2592
static constexpr size_t SecKeyByteLen = MLDSA87_SECKEY_LEN;       // 4896
static constexpr size_t SigByteLen = MLDSA87_SIG_LEN;             // 4627
static constexpr size_t SigningSeedByteLen = MLDSA87_SEED_LEN;    // 32

// Keygen - matches original API with std::array
inline void keygen(
    const std::array<uint8_t, KeygenSeedByteLen>& seed,
    std::array<uint8_t, PubKeyByteLen>& pubkey,
    std::array<uint8_t, SecKeyByteLen>& seckey)
{
    mldsa87_keygen(seed.data(), pubkey.data(), seckey.data());
}

// Sign - matches original API (fixed-size message via std::array)
template<size_t N>
inline bool sign(
    const std::array<uint8_t, SigningSeedByteLen>& rnd,
    const std::array<uint8_t, SecKeyByteLen>& seckey,
    const std::array<uint8_t, N>& msg,
    const std::array<uint8_t, 0>&,  // empty context (ignored)
    std::array<uint8_t, SigByteLen>& sig)
{
    return mldsa87_sign(rnd.data(), seckey.data(), msg.data(), N, sig.data()) == 1;
}

// Verify - matches original API (fixed-size message via std::array)
template<size_t N>
inline bool verify(
    const std::array<uint8_t, PubKeyByteLen>& pubkey,
    const std::array<uint8_t, N>& msg,
    const std::array<uint8_t, 0>&,  // empty context (ignored)
    const std::array<uint8_t, SigByteLen>& sig)
{
    return mldsa87_verify(pubkey.data(), msg.data(), N, sig.data()) == 1;
}

// Sign - matches original API (variable-size message via std::span)
template<typename Span>
inline bool sign(
    const std::array<uint8_t, SigningSeedByteLen>& rnd,
    const std::array<uint8_t, SecKeyByteLen>& seckey,
    const Span& msg,
    const std::array<uint8_t, 0>&,  // empty context (ignored)
    std::array<uint8_t, SigByteLen>& sig)
{
    return mldsa87_sign(rnd.data(), seckey.data(), msg.data(), msg.size(), sig.data()) == 1;
}

// Verify - matches original API (variable-size message via std::span)
template<typename Span>
inline bool verify(
    const std::array<uint8_t, PubKeyByteLen>& pubkey,
    const Span& msg,
    const std::array<uint8_t, 0>&,  // empty context (ignored)
    const std::array<uint8_t, SigByteLen>& sig)
{
    return mldsa87_verify(pubkey.data(), msg.data(), msg.size(), sig.data()) == 1;
}

} // namespace ml_dsa_87
