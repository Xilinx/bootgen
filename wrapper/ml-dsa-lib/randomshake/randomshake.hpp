#pragma once
/******************************************************************************
* RandomShake Compatibility Header (C++14)
* 
* Provides the same API as the original C++20 randomshake library.
* Internally calls the pre-built libmldsa.a C functions.
******************************************************************************/

#include <array>
#include <cstdint>
#include <cstddef>
#include <cstring>

extern "C" {
#include "ml_dsa_api.h"
}

namespace randomshake {

// Compatibility template matching original randomshake API
template<size_t>
struct randomshake_t {
    // Generate random bytes into an array (matches original API)
    template<size_t N>
    void generate(std::array<uint8_t, N>& output) {
        mldsa_random_bytes(output.data(), N);
    }
    
    // Generate random bytes into a raw buffer
    void generate(uint8_t* output, size_t len) {
        mldsa_random_bytes(output, len);
    }
};

} // namespace randomshake
