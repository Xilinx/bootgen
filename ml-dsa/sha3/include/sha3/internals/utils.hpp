#pragma once
#include "sha3/internals/force_inline.hpp"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <span>

// Utility ( or commonly used ) functions for SHA3 implementation
namespace sha3_utils {

// Given a byte array of length 8, this routine can be used for interpreting those 8 -bytes in little-endian order, as a 64 -bit unsigned integer.
static forceinline constexpr uint64_t
le_bytes_to_u64(std::span<const uint8_t, sizeof(uint64_t)> bytes)
{
  static_assert(std::endian::native == std::endian::little);

  return (static_cast<uint64_t>(bytes[7]) << 56u) | (static_cast<uint64_t>(bytes[6]) << 48u) | (static_cast<uint64_t>(bytes[5]) << 40u) |
         (static_cast<uint64_t>(bytes[4]) << 32u) | (static_cast<uint64_t>(bytes[3]) << 24u) | (static_cast<uint64_t>(bytes[2]) << 16u) |
         (static_cast<uint64_t>(bytes[1]) << 8u) | (static_cast<uint64_t>(bytes[0]) << 0u);
}

// Given a 64 -bit unsigned integer as input, this routine can be used for interpreting those 8 -bytes in little-endian byte order.
static forceinline constexpr void
u64_to_le_bytes(uint64_t word, std::span<uint8_t, sizeof(word)> bytes)
{
  static_assert(std::endian::native == std::endian::little);

  bytes[0] = static_cast<uint8_t>(word >> 0u);
  bytes[1] = static_cast<uint8_t>(word >> 8u);
  bytes[2] = static_cast<uint8_t>(word >> 16u);
  bytes[3] = static_cast<uint8_t>(word >> 24u);
  bytes[4] = static_cast<uint8_t>(word >> 32u);
  bytes[5] = static_cast<uint8_t>(word >> 40u);
  bytes[6] = static_cast<uint8_t>(word >> 48u);
  bytes[7] = static_cast<uint8_t>(word >> 56u);
}

}
