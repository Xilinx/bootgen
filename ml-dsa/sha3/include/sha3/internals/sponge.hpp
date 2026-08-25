#pragma once
#include "sha3/internals/force_inline.hpp"
#include "sha3/internals/keccak.hpp"
#include "sha3/internals/utils.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <span>

// Keccak family of sponge functions
namespace sponge {

static constexpr size_t KECCAK_WORD_BYTE_LEN = keccak::LANE_BW / std::numeric_limits<uint8_t>::digits;

// Compile-time check to ensure that domain separator can only be 2/ 4 -bits wide.
//
// When used in context of extendable output functions ( SHAKE{128, 256} ) domain separator bits are 4 -bit wide.
//
// See section 6.{1, 2} of SHA3 specification https://dx.doi.org/10.6028/NIST.FIPS.202
constexpr bool
check_domain_separator(const size_t dom_sep_bit_len)
{
  return (dom_sep_bit_len == 2u) | (dom_sep_bit_len == 4u);
}

// Given `mlen` (>=0) -bytes message, this routine consumes it into Keccak[c] permutation state s.t. `offset` ( second parameter ) denotes how many bytes are
// already consumed into rate portion of the state.
//
// - `num_bits_in_rate` portion of sponge will have bitwidth of 1600 - c.
// - `offset` must ∈ [0, `num_bytes_in_rate`).
//
// This function implementation collects inspiration from https://github.com/itzmeanjan/turboshake/blob/e1a6b950/src/sponge.rs#L4-L56
template<size_t num_bits_in_rate>
static forceinline constexpr void
absorb(uint64_t state[keccak::LANE_CNT], size_t& offset, std::span<const uint8_t> msg)
{
  constexpr size_t num_bytes_in_rate = num_bits_in_rate / std::numeric_limits<uint8_t>::digits;

  std::array<uint8_t, num_bytes_in_rate> block{};
  auto block_span = std::span(block);

  size_t msg_offset = 0;
  while (msg_offset < msg.size()) {
    const size_t remaining_num_bytes = msg.size() - msg_offset;
    const size_t absorbable_num_bytes = std::min(remaining_num_bytes, num_bytes_in_rate - offset);
    const size_t effective_block_byte_len = offset + absorbable_num_bytes;
    const size_t padded_effective_block_byte_len = (effective_block_byte_len + (KECCAK_WORD_BYTE_LEN - 1)) & (-KECCAK_WORD_BYTE_LEN);
    const size_t padded_effective_block_begins_at = offset & (-KECCAK_WORD_BYTE_LEN);

    std::fill_n(block_span.subspan(padded_effective_block_begins_at).begin(), padded_effective_block_byte_len - padded_effective_block_begins_at, 0x00);
    std::copy_n(msg.subspan(msg_offset).begin(), absorbable_num_bytes, block_span.subspan(offset).begin());

    size_t state_word_index = padded_effective_block_begins_at / KECCAK_WORD_BYTE_LEN;
    for (size_t i = padded_effective_block_begins_at; i < padded_effective_block_byte_len; i += KECCAK_WORD_BYTE_LEN) {
      auto msg_chunk = std::span<const uint8_t, KECCAK_WORD_BYTE_LEN>(block_span.subspan(i, KECCAK_WORD_BYTE_LEN));
      auto msg_word = sha3_utils::le_bytes_to_u64(msg_chunk);

      state[state_word_index] ^= msg_word;
      state_word_index++;
    }

    offset += absorbable_num_bytes;
    msg_offset += absorbable_num_bytes;

    if (offset == num_bytes_in_rate) [[unlikely]] {
      keccak::permute(state);
      offset = 0;
    }
  }
}

// Given that N message bytes are already consumed into Keccak[c] permutation state, this routine finalizes sponge state and makes it ready for squeezing, by
// appending ( along with domain separation bits ) 10*1 padding bits to input message s.t. total absorbed message byte length becomes multiple of `rate/ 8`
// -bytes.
//
// - `num_bits_in_rate` portion of sponge will have bitwidth of 1600 - c.
// - `offset` must ∈ [0, `num_bytes_in_rate`)
//
// This function implementation collects some motivation from https://github.com/itzmeanjan/turboshake/blob/e1a6b950/src/sponge.rs#L58-L81
template<uint8_t domain_separator, size_t ds_bit_len, size_t num_bits_in_rate>
static forceinline constexpr void
finalize(uint64_t state[keccak::LANE_CNT], size_t& offset)
  requires(check_domain_separator(ds_bit_len))
{
  constexpr size_t num_bytes_in_rate = num_bits_in_rate / std::numeric_limits<uint8_t>::digits;
  constexpr size_t num_words_in_rate = num_bytes_in_rate / std::numeric_limits<uint8_t>::digits;

  const auto state_word_index = offset / KECCAK_WORD_BYTE_LEN;
  const auto byte_index_in_state_word = offset % KECCAK_WORD_BYTE_LEN;
  const auto shl_bit_offset = byte_index_in_state_word * std::numeric_limits<uint8_t>::digits;

  constexpr uint8_t mask = (1u << ds_bit_len) - 1u;
  constexpr uint8_t pad_byte = (1u << ds_bit_len) | (domain_separator & mask);

  state[state_word_index] ^= static_cast<uint64_t>(pad_byte) << shl_bit_offset;
  state[num_words_in_rate - 1] ^= UINT64_C(0x80) << 56;

  keccak::permute(state);
  offset = 0;
}

// Given that Keccak[c] permutation state is finalized, this routine can be invoked for squeezing `olen` -bytes out of rate portion of the state.
//
// - `num_bits_in_rate` portion of sponge will have bitwidth of 1600 - c.
// - `squeezable` denotes how many bytes can be squeezed without permutating the sponge state.
// - When `squeezable` becomes 0, state needs to be permutated again, after which `num_bytes_in_rate` can again be squeezed from rate portion of the state.
//
// This function implementation collects motivation from https://github.com/itzmeanjan/turboshake/blob/e1a6b950/src/sponge.rs#L83-L118
template<size_t num_bits_in_rate>
static forceinline constexpr void
squeeze(uint64_t state[keccak::LANE_CNT], size_t& squeezable, std::span<uint8_t> out)
{
  constexpr size_t num_bytes_in_rate = num_bits_in_rate / std::numeric_limits<uint8_t>::digits;

  std::array<uint8_t, num_bytes_in_rate> blk_bytes{};
  auto blk_bytes_span = std::span(blk_bytes);

  size_t out_offset = 0;
  while (out_offset < out.size()) {
    const size_t state_byte_offset = num_bytes_in_rate - squeezable;
    const size_t remaining_num_bytes = out.size() - out_offset;
    const size_t squeezable_num_bytes = std::min(remaining_num_bytes, squeezable);
    const size_t effective_block_byte_len = state_byte_offset + squeezable_num_bytes;
    const size_t padded_effective_block_byte_len = (effective_block_byte_len + (KECCAK_WORD_BYTE_LEN - 1)) & (-KECCAK_WORD_BYTE_LEN);
    const size_t padded_effective_block_begins_at = state_byte_offset & (-KECCAK_WORD_BYTE_LEN);

    size_t state_word_index = padded_effective_block_begins_at / KECCAK_WORD_BYTE_LEN;
    for (size_t i = padded_effective_block_begins_at; i < padded_effective_block_byte_len; i += KECCAK_WORD_BYTE_LEN) {
      auto chunk_bytes = std::span<uint8_t, KECCAK_WORD_BYTE_LEN>(blk_bytes_span.subspan(i, KECCAK_WORD_BYTE_LEN));
      sha3_utils::u64_to_le_bytes(state[state_word_index], chunk_bytes);

      state_word_index++;
    }

    std::copy_n(blk_bytes_span.subspan(state_byte_offset).begin(), squeezable_num_bytes, out.subspan(out_offset).begin());

    squeezable -= squeezable_num_bytes;
    out_offset += squeezable_num_bytes;

    if (squeezable == 0) [[unlikely]] {
      keccak::permute(state);
      squeezable = num_bytes_in_rate;
    }
  }
}

}
