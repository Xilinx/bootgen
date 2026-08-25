#pragma once
#include "sha3/internals/keccak.hpp"
#include "sha3/internals/sponge.hpp"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits>

// SHAKE128 Extendable Output Function : Keccak[256](M || 1111, d)
namespace shake128 {

// Width of capacity portion of the sponge, in bits.
static constexpr size_t CAPACITY = 256;

// Width of rate portion of the sponge, in bits.
static constexpr size_t RATE = 1600 - CAPACITY;

// Domain separator bits, used for finalization.
static constexpr uint8_t DOM_SEP = 0b00001111;

// Bit-width of domain separator, starting from least significant bit.
static constexpr size_t DOM_SEP_BW = 4;

// SHAKE128 Extendable Output Function (Xof)
//
// See SHA3 extendable output function definition in section 6.2 of SHA3 specification
// https://dx.doi.org/10.6028/NIST.FIPS.202
struct shake128_t
{
private:
  uint64_t state[keccak::LANE_CNT]{};
  size_t offset = 0;
  alignas(4) bool finalized = false; // all message bytes absorbed ?
  size_t squeezable = 0;

public:
  forceinline constexpr shake128_t() = default;
  forceinline constexpr size_t squeezable_num_bytes() const { return squeezable; }

  // Given N -many bytes input message, this routine consumes those into keccak[256] sponge state.
  //
  // Note, this routine can be called arbitrary number of times, each time with arbitrary bytes of input message, until
  // keccak[256] state is finalized ( by calling routine with similar name ). Once the sponge is finalized, it can't
  // absorb any more message bytes.
  forceinline constexpr void absorb(std::span<const uint8_t> msg)
  {
    if (!finalized) {
      sponge::absorb<RATE>(state, offset, msg);
    }
  }

  // After consuming arbitrary many input bytes, this routine is invoked when no more input bytes remaining to be
  // consumed by keccak[256] state.
  //
  // Note, once this routine is called, calling absorb() or finalize() again, on same SHAKE128 object, doesn't do
  // anything. After finalization, one might intend to read arbitrary many bytes by squeezing sponge, which is done by
  // calling read() function, as many times required.
  forceinline constexpr void finalize()
  {
    if (!finalized) {
      sponge::finalize<DOM_SEP, DOM_SEP_BW, RATE>(state, offset);

      finalized = true;
      squeezable = RATE / std::numeric_limits<uint8_t>::digits;
    }
  }

  // After sponge state is finalized, arbitrary many output bytes can be squeezed by calling this function any number of
  // times required.
  forceinline constexpr void squeeze(std::span<uint8_t> dig)
  {
    if (finalized) {
      sponge::squeeze<RATE>(state, squeezable, dig);
    }
  }

  // Reset the internal state of the Shake128-Xof hasher, now it can again be used for another absorb->finalize->squeeze
  // cycle.
  forceinline constexpr void reset()
  {
    std::fill(std::begin(state), std::end(state), 0);
    offset = 0;
    finalized = false;
    squeezable = 0;
  }

  // Given that sponge is already finalized, this routine can be used for zeroizing first n -bytes of permutation state
  // s.t. n <= 200 and applying permutation.
  forceinline void ratchet(const size_t byte_len)
  {
    if (finalized) {
      const auto ratchetable_portion_byte_len = std::min(byte_len, keccak::STATE_BYTE_LEN);

      auto state_as_bytes = reinterpret_cast<uint8_t*>(state);
      std::memset(state_as_bytes, 0, ratchetable_portion_byte_len);

      keccak::permute(state);
    }
  }
};

}
