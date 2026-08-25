#pragma once
#include "sha3/internals/sponge.hpp"
#include <cstdint>
#include <limits>

// SHA3-224 Hash Function : Keccak[448](M || 01, 224)
namespace sha3_224 {

// Bit length of SHA3-224 message digest.
static constexpr size_t DIGEST_BIT_LEN = 224;

// Byte length of SHA3-224 message digest.
static constexpr size_t DIGEST_LEN = DIGEST_BIT_LEN / 8;

// Width of capacity portion of the sponge, in bits.
static constexpr size_t CAPACITY = 2 * DIGEST_BIT_LEN;

// Width of rate portion of the sponge, in bits.
static constexpr size_t RATE = 1600 - CAPACITY;

// Domain separator bits, used for finalization.
static constexpr uint8_t DOM_SEP = 0b00000010;

// Bit-width of domain separator, starting from least significant bit.
static constexpr size_t DOM_SEP_BW = 2;

// Given arbitrary many input message bytes, this routine consumes it into keccak[448] sponge state and squeezes out 28
// -bytes digest.
//
// See SHA3 hash function definition in section 6.1 of SHA3 specification https://dx.doi.org/10.6028/NIST.FIPS.202.
struct sha3_224_t
{
private:
  uint64_t state[keccak::LANE_CNT]{};
  size_t offset = 0;
  alignas(4) bool finalized = false;
  alignas(4) bool squeezed = false;

public:
  // Constructor
  forceinline constexpr sha3_224_t() = default;

  // Given N(>=0) -bytes message as input, this routine can be invoked arbitrary many times ( until the sponge is
  // finalized ), each time absorbing arbitrary many message bytes into RATE portion of the sponge.
  forceinline constexpr void absorb(std::span<const uint8_t> msg)
  {
    if (!finalized) {
      sponge::absorb<RATE>(state, offset, msg);
    }
  }

  // Finalizes the sponge after all message bytes are absorbed into it, now it should be ready for squeezing message
  // digest bytes. Once finalized, you can't absorb any message bytes into sponge. After finalization, calling this
  // function again and again doesn't mutate anything.
  forceinline constexpr void finalize()
  {
    if (!finalized) {
      sponge::finalize<DOM_SEP, DOM_SEP_BW, RATE>(state, offset);
      finalized = true;
    }
  }

  // After sponge state is finalized, 28 message digest bytes can be squeezed by calling this function. Once digest
  // bytes are squeezed, calling this function again and again returns nothing.
  forceinline constexpr void digest(std::span<uint8_t, DIGEST_LEN> md)
  {
    if (finalized && !squeezed) {
      size_t squeezable = RATE / std::numeric_limits<uint8_t>::digits;
      sponge::squeeze<RATE>(state, squeezable, md);

      squeezed = true;
    }
  }

  // Reset the internal state of the SHA3-224 hasher, now it can again be used for another absorb->finalize->squeeze
  // cycle.
  forceinline constexpr void reset()
  {
    std::fill(std::begin(state), std::end(state), 0);
    offset = 0;
    finalized = false;
    squeezed = false;
  }
};

}
