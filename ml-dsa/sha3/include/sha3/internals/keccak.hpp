#pragma once
#include "sha3/internals/force_inline.hpp"
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>

// Keccak-p[1600, 24] permutation
namespace keccak {

// Logarithmic base 2 of bit width of lane i.e. log2(LANE_BW)
static constexpr size_t L = 6;

// Bit width of each lane of Keccak-p[1600, 24] state
static constexpr size_t LANE_BW = 1ul << L;

// Bit length of Keccak-p[1600, 24] permutation state
static constexpr size_t STATE_BIT_LEN = 1600;

// Byte length of Keccak-p[1600, 24] permutation state
static constexpr size_t STATE_BYTE_LEN = STATE_BIT_LEN / std::numeric_limits<uint8_t>::digits;

// # -of lanes ( each of 64 -bit width ) in Keccak-p[1600, 24] state
static constexpr size_t LANE_CNT = STATE_BIT_LEN / LANE_BW;

// Keccak-p[b, nr] permutation to be applied `nr` ( = 24 ) rounds
// s.t. b = 1600, w = b/ 25, l = log2(w), nr = 12 + 2l
static constexpr size_t ROUNDS = 12 + 2 * L;

// Leftwards circular rotation offset of 25 lanes ( each lane is 64 -bit wide ) of state array, as provided in table 2
// below algorithm 2 in section 3.2.2 of https://dx.doi.org/10.6028/NIST.FIPS.202
//
// Note, following offsets are obtained by performing % 64 ( bit width of lane ) on offsets provided in above mentioned
// link
static constexpr size_t ROT[LANE_CNT]{ 0 % LANE_BW,   1 % LANE_BW,   190 % LANE_BW, 28 % LANE_BW, 91 % LANE_BW, 36 % LANE_BW,  300 % LANE_BW,
                                       6 % LANE_BW,   55 % LANE_BW,  276 % LANE_BW, 3 % LANE_BW,  10 % LANE_BW, 171 % LANE_BW, 153 % LANE_BW,
                                       231 % LANE_BW, 105 % LANE_BW, 45 % LANE_BW,  15 % LANE_BW, 21 % LANE_BW, 136 % LANE_BW, 210 % LANE_BW,
                                       66 % LANE_BW,  253 % LANE_BW, 120 % LANE_BW, 78 % LANE_BW };

// Precomputed table used for looking up source index during application of π step mapping function on keccak-[1600, 24]
// state
//
// print('to <= from')
// for y in range(5):
//    for x in range(5):
//        print(f'{y * 5 + x} <= {x * 5 + (x + 3 * y) % 5}')
//
// Table generated using above Python code snippet. See section 3.2.3 of the specification
// https://dx.doi.org/10.6028/NIST.FIPS.202
static constexpr size_t PERM[LANE_CNT]{ 0, 6, 12, 18, 24, 3, 9, 10, 16, 22, 1, 7, 13, 19, 20, 4, 5, 11, 17, 23, 2, 8, 14, 15, 21 };

// Computes single bit of Keccak-p[1600, 24] round constant ( at compile-time ), using binary LFSR, defined by primitive
// polynomial x^8 + x^6 + x^5 + x^4 + 1
//
// See algorithm 5 in section 3.2.5 of http://dx.doi.org/10.6028/NIST.FIPS.202
//
// Taken from https://github.com/itzmeanjan/elephant/blob/2a21c7e/include/keccak.hpp#L24-L59
static consteval bool
rc(const size_t t)
{
  // step 1 of algorithm 5
  if (t % 255 == 0) {
    return 1;
  }

  // step 2 of algorithm 5
  //
  // note, step 3.a of algorithm 5 is also being executed in this statement ( for first iteration, with i = 1 ) !
  uint16_t r = 0b10000000;

  // step 3 of algorithm 5
  for (size_t i = 1; i <= t % 255; i++) {
    const uint16_t b0 = r & 1;

    r = (r & 0b011111111) ^ ((((r >> 8) & 1) ^ b0) << 8);
    r = (r & 0b111101111) ^ ((((r >> 4) & 1) ^ b0) << 4);
    r = (r & 0b111110111) ^ ((((r >> 3) & 1) ^ b0) << 3);
    r = (r & 0b111111011) ^ ((((r >> 2) & 1) ^ b0) << 2);

    // step 3.f of algorithm 5
    //
    // note, this statement also executes step 3.a for upcoming iterations ( i.e. when i > 1 )
    r >>= 1;
  }

  return static_cast<bool>((r >> 7) & 1);
}

// Computes 64 -bit round constant ( at compile-time ), which is XOR-ed into very first lane ( = lane(0, 0) ) of
// Keccak-p[1600, 24] permutation state
//
// Taken from https://github.com/itzmeanjan/elephant/blob/2a21c7e/include/keccak.hpp#L61-L74
static consteval uint64_t
compute_rc(const size_t r_idx)
{
  uint64_t tmp = 0;

  for (size_t j = 0; j < (L + 1); j++) {
    const size_t boff = (1 << j) - 1;
    tmp |= static_cast<uint64_t>(rc(j + 7 * r_idx)) << boff;
  }

  return tmp;
}

// Compile-time evaluate Keccak-p[1600, 24] round constants.
static consteval std::array<uint64_t, LANE_CNT>
compute_rcs()
{
  std::array<uint64_t, LANE_CNT> res;
  for (size_t i = 0; i < LANE_CNT; i++) {
    res[i] = compute_rc(i);
  }

  return res;
}

// Round constants to be XORed with lane (0, 0) of keccak-p[1600, 24] permutation state, see section 3.2.5 of
// https://dx.doi.org/10.s6028/NIST.FIPS.202
static constexpr auto RC = compute_rcs();

// Keccak-p[1600, 24] round function, applying all five step mapping functions, updating state array. Note this
// implementation of round function applies four consecutive rounds in a single call i.e. if you invoke it to apply
// round `i`
//
// - it first applies round `i`
// - then round `i+1`
// - and then round `i+2`
// - and finally round `i+3`
//
// See section 3.3 of https://dx.doi.org/10.6028/NIST.FIPS.202.
// This implementation collects a lot of inspiration from https://github.com/bwesterb/armed-keccak.git.
static forceinline constexpr void
roundx4(uint64_t* const state, const size_t ridx)
{
  std::array<uint64_t, 5> bc{}, d{};
  uint64_t t = 0;

// Round ridx + 0
#if defined __clang__
  // Following https://clang.llvm.org/docs/LanguageExtensions.html#extensions-for-loop-hint-optimizations

#pragma clang loop unroll(enable)
#pragma clang loop vectorize(enable)
#pragma clang loop interleave(enable)
#elif defined __GNUG__
  // Following https://gcc.gnu.org/onlinedocs/gcc/Loop-Specific-Pragmas.html#Loop-Specific-Pragmas

#pragma GCC unroll 5
#pragma GCC ivdep
#endif
  for (size_t i = 0; i < 25; i += 5) {
    bc[0] ^= state[i + 0];
    bc[1] ^= state[i + 1];
    bc[2] ^= state[i + 2];
    bc[3] ^= state[i + 3];
    bc[4] ^= state[i + 4];
  }

  d[0] = bc[4] ^ std::rotl(bc[1], 1);
  d[1] = bc[0] ^ std::rotl(bc[2], 1);
  d[2] = bc[1] ^ std::rotl(bc[3], 1);
  d[3] = bc[2] ^ std::rotl(bc[4], 1);
  d[4] = bc[3] ^ std::rotl(bc[0], 1);

  bc[0] = state[0] ^ d[0];
  t = state[6] ^ d[1];
  bc[1] = std::rotl(t, ROT[6]);
  t = state[12] ^ d[2];
  bc[2] = std::rotl(t, ROT[12]);
  t = state[18] ^ d[3];
  bc[3] = std::rotl(t, ROT[18]);
  t = state[24] ^ d[4];
  bc[4] = std::rotl(t, ROT[24]);

  state[0] = bc[0] ^ (bc[2] & ~bc[1]) ^ RC[ridx];
  state[6] = bc[1] ^ (bc[3] & ~bc[2]);
  state[12] = bc[2] ^ (bc[4] & ~bc[3]);
  state[18] = bc[3] ^ (bc[0] & ~bc[4]);
  state[24] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[10] ^ d[0];
  bc[2] = std::rotl(t, ROT[10]);
  t = state[16] ^ d[1];
  bc[3] = std::rotl(t, ROT[16]);
  t = state[22] ^ d[2];
  bc[4] = std::rotl(t, ROT[22]);
  t = state[3] ^ d[3];
  bc[0] = std::rotl(t, ROT[3]);
  t = state[9] ^ d[4];
  bc[1] = std::rotl(t, ROT[9]);

  state[10] = bc[0] ^ (bc[2] & ~bc[1]);
  state[16] = bc[1] ^ (bc[3] & ~bc[2]);
  state[22] = bc[2] ^ (bc[4] & ~bc[3]);
  state[3] = bc[3] ^ (bc[0] & ~bc[4]);
  state[9] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[20] ^ d[0];
  bc[4] = std::rotl(t, ROT[20]);
  t = state[1] ^ d[1];
  bc[0] = std::rotl(t, ROT[1]);
  t = state[7] ^ d[2];
  bc[1] = std::rotl(t, ROT[7]);
  t = state[13] ^ d[3];
  bc[2] = std::rotl(t, ROT[13]);
  t = state[19] ^ d[4];
  bc[3] = std::rotl(t, ROT[19]);

  state[20] = bc[0] ^ (bc[2] & ~bc[1]);
  state[1] = bc[1] ^ (bc[3] & ~bc[2]);
  state[7] = bc[2] ^ (bc[4] & ~bc[3]);
  state[13] = bc[3] ^ (bc[0] & ~bc[4]);
  state[19] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[5] ^ d[0];
  bc[1] = std::rotl(t, ROT[5]);
  t = state[11] ^ d[1];
  bc[2] = std::rotl(t, ROT[11]);
  t = state[17] ^ d[2];
  bc[3] = std::rotl(t, ROT[17]);
  t = state[23] ^ d[3];
  bc[4] = std::rotl(t, ROT[23]);
  t = state[4] ^ d[4];
  bc[0] = std::rotl(t, ROT[4]);

  state[5] = bc[0] ^ (bc[2] & ~bc[1]);
  state[11] = bc[1] ^ (bc[3] & ~bc[2]);
  state[17] = bc[2] ^ (bc[4] & ~bc[3]);
  state[23] = bc[3] ^ (bc[0] & ~bc[4]);
  state[4] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[15] ^ d[0];
  bc[3] = std::rotl(t, ROT[15]);
  t = state[21] ^ d[1];
  bc[4] = std::rotl(t, ROT[21]);
  t = state[2] ^ d[2];
  bc[0] = std::rotl(t, ROT[2]);
  t = state[8] ^ d[3];
  bc[1] = std::rotl(t, ROT[8]);
  t = state[14] ^ d[4];
  bc[2] = std::rotl(t, ROT[14]);

  state[15] = bc[0] ^ (bc[2] & ~bc[1]);
  state[21] = bc[1] ^ (bc[3] & ~bc[2]);
  state[2] = bc[2] ^ (bc[4] & ~bc[3]);
  state[8] = bc[3] ^ (bc[0] & ~bc[4]);
  state[14] = bc[4] ^ (bc[1] & ~bc[0]);

  // Round ridx + 1
  std::fill(bc.begin(), bc.end(), 0x00);

#if defined __clang__
#pragma clang loop unroll(enable)
#pragma clang loop vectorize(enable)
#pragma clang loop interleave(enable)
#elif defined __GNUG__
#pragma GCC unroll 5
#pragma GCC ivdep
#endif
  for (size_t i = 0; i < 25; i += 5) {
    bc[0] ^= state[i + 0];
    bc[1] ^= state[i + 1];
    bc[2] ^= state[i + 2];
    bc[3] ^= state[i + 3];
    bc[4] ^= state[i + 4];
  }

  d[0] = bc[4] ^ std::rotl(bc[1], 1);
  d[1] = bc[0] ^ std::rotl(bc[2], 1);
  d[2] = bc[1] ^ std::rotl(bc[3], 1);
  d[3] = bc[2] ^ std::rotl(bc[4], 1);
  d[4] = bc[3] ^ std::rotl(bc[0], 1);

  bc[0] = state[0] ^ d[0];
  t = state[16] ^ d[1];
  bc[1] = std::rotl(t, ROT[6]);
  t = state[7] ^ d[2];
  bc[2] = std::rotl(t, ROT[12]);
  t = state[23] ^ d[3];
  bc[3] = std::rotl(t, ROT[18]);
  t = state[14] ^ d[4];
  bc[4] = std::rotl(t, ROT[24]);

  state[0] = bc[0] ^ (bc[2] & ~bc[1]) ^ RC[ridx + 1];
  state[16] = bc[1] ^ (bc[3] & ~bc[2]);
  state[7] = bc[2] ^ (bc[4] & ~bc[3]);
  state[23] = bc[3] ^ (bc[0] & ~bc[4]);
  state[14] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[20] ^ d[0];
  bc[2] = std::rotl(t, ROT[10]);
  t = state[11] ^ d[1];
  bc[3] = std::rotl(t, ROT[16]);
  t = state[2] ^ d[2];
  bc[4] = std::rotl(t, ROT[22]);
  t = state[18] ^ d[3];
  bc[0] = std::rotl(t, ROT[3]);
  t = state[9] ^ d[4];
  bc[1] = std::rotl(t, ROT[9]);

  state[20] = bc[0] ^ (bc[2] & ~bc[1]);
  state[11] = bc[1] ^ (bc[3] & ~bc[2]);
  state[2] = bc[2] ^ (bc[4] & ~bc[3]);
  state[18] = bc[3] ^ (bc[0] & ~bc[4]);
  state[9] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[15] ^ d[0];
  bc[4] = std::rotl(t, ROT[20]);
  t = state[6] ^ d[1];
  bc[0] = std::rotl(t, ROT[1]);
  t = state[22] ^ d[2];
  bc[1] = std::rotl(t, ROT[7]);
  t = state[13] ^ d[3];
  bc[2] = std::rotl(t, ROT[13]);
  t = state[4] ^ d[4];
  bc[3] = std::rotl(t, ROT[19]);

  state[15] = bc[0] ^ (bc[2] & ~bc[1]);
  state[6] = bc[1] ^ (bc[3] & ~bc[2]);
  state[22] = bc[2] ^ (bc[4] & ~bc[3]);
  state[13] = bc[3] ^ (bc[0] & ~bc[4]);
  state[4] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[10] ^ d[0];
  bc[1] = std::rotl(t, ROT[5]);
  t = state[1] ^ d[1];
  bc[2] = std::rotl(t, ROT[11]);
  t = state[17] ^ d[2];
  bc[3] = std::rotl(t, ROT[17]);
  t = state[8] ^ d[3];
  bc[4] = std::rotl(t, ROT[23]);
  t = state[24] ^ d[4];
  bc[0] = std::rotl(t, ROT[4]);

  state[10] = bc[0] ^ (bc[2] & ~bc[1]);
  state[1] = bc[1] ^ (bc[3] & ~bc[2]);
  state[17] = bc[2] ^ (bc[4] & ~bc[3]);
  state[8] = bc[3] ^ (bc[0] & ~bc[4]);
  state[24] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[5] ^ d[0];
  bc[3] = std::rotl(t, ROT[15]);
  t = state[21] ^ d[1];
  bc[4] = std::rotl(t, ROT[21]);
  t = state[12] ^ d[2];
  bc[0] = std::rotl(t, ROT[2]);
  t = state[3] ^ d[3];
  bc[1] = std::rotl(t, ROT[8]);
  t = state[19] ^ d[4];
  bc[2] = std::rotl(t, ROT[14]);

  state[5] = bc[0] ^ (bc[2] & ~bc[1]);
  state[21] = bc[1] ^ (bc[3] & ~bc[2]);
  state[12] = bc[2] ^ (bc[4] & ~bc[3]);
  state[3] = bc[3] ^ (bc[0] & ~bc[4]);
  state[19] = bc[4] ^ (bc[1] & ~bc[0]);

  // Round ridx + 2
  std::fill(bc.begin(), bc.end(), 0x00);

#if defined __clang__
#pragma clang loop unroll(enable)
#pragma clang loop vectorize(enable)
#pragma clang loop interleave(enable)
#elif defined __GNUG__
#pragma GCC unroll 5
#pragma GCC ivdep
#endif
  for (size_t i = 0; i < 25; i += 5) {
    bc[0] ^= state[i + 0];
    bc[1] ^= state[i + 1];
    bc[2] ^= state[i + 2];
    bc[3] ^= state[i + 3];
    bc[4] ^= state[i + 4];
  }

  d[0] = bc[4] ^ std::rotl(bc[1], 1);
  d[1] = bc[0] ^ std::rotl(bc[2], 1);
  d[2] = bc[1] ^ std::rotl(bc[3], 1);
  d[3] = bc[2] ^ std::rotl(bc[4], 1);
  d[4] = bc[3] ^ std::rotl(bc[0], 1);

  bc[0] = state[0] ^ d[0];
  t = state[11] ^ d[1];
  bc[1] = std::rotl(t, ROT[6]);
  t = state[22] ^ d[2];
  bc[2] = std::rotl(t, ROT[12]);
  t = state[8] ^ d[3];
  bc[3] = std::rotl(t, ROT[18]);
  t = state[19] ^ d[4];
  bc[4] = std::rotl(t, ROT[24]);

  state[0] = bc[0] ^ (bc[2] & ~bc[1]) ^ RC[ridx + 2];
  state[11] = bc[1] ^ (bc[3] & ~bc[2]);
  state[22] = bc[2] ^ (bc[4] & ~bc[3]);
  state[8] = bc[3] ^ (bc[0] & ~bc[4]);
  state[19] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[15] ^ d[0];
  bc[2] = std::rotl(t, ROT[10]);
  t = state[1] ^ d[1];
  bc[3] = std::rotl(t, ROT[16]);
  t = state[12] ^ d[2];
  bc[4] = std::rotl(t, ROT[22]);
  t = state[23] ^ d[3];
  bc[0] = std::rotl(t, ROT[3]);
  t = state[9] ^ d[4];
  bc[1] = std::rotl(t, ROT[9]);

  state[15] = bc[0] ^ (bc[2] & ~bc[1]);
  state[1] = bc[1] ^ (bc[3] & ~bc[2]);
  state[12] = bc[2] ^ (bc[4] & ~bc[3]);
  state[23] = bc[3] ^ (bc[0] & ~bc[4]);
  state[9] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[5] ^ d[0];
  bc[4] = std::rotl(t, ROT[20]);
  t = state[16] ^ d[1];
  bc[0] = std::rotl(t, ROT[1]);
  t = state[2] ^ d[2];
  bc[1] = std::rotl(t, ROT[7]);
  t = state[13] ^ d[3];
  bc[2] = std::rotl(t, ROT[13]);
  t = state[24] ^ d[4];
  bc[3] = std::rotl(t, ROT[19]);

  state[5] = bc[0] ^ (bc[2] & ~bc[1]);
  state[16] = bc[1] ^ (bc[3] & ~bc[2]);
  state[2] = bc[2] ^ (bc[4] & ~bc[3]);
  state[13] = bc[3] ^ (bc[0] & ~bc[4]);
  state[24] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[20] ^ d[0];
  bc[1] = std::rotl(t, ROT[5]);
  t = state[6] ^ d[1];
  bc[2] = std::rotl(t, ROT[11]);
  t = state[17] ^ d[2];
  bc[3] = std::rotl(t, ROT[17]);
  t = state[3] ^ d[3];
  bc[4] = std::rotl(t, ROT[23]);
  t = state[14] ^ d[4];
  bc[0] = std::rotl(t, ROT[4]);

  state[20] = bc[0] ^ (bc[2] & ~bc[1]);
  state[6] = bc[1] ^ (bc[3] & ~bc[2]);
  state[17] = bc[2] ^ (bc[4] & ~bc[3]);
  state[3] = bc[3] ^ (bc[0] & ~bc[4]);
  state[14] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[10] ^ d[0];
  bc[3] = std::rotl(t, ROT[15]);
  t = state[21] ^ d[1];
  bc[4] = std::rotl(t, ROT[21]);
  t = state[7] ^ d[2];
  bc[0] = std::rotl(t, ROT[2]);
  t = state[18] ^ d[3];
  bc[1] = std::rotl(t, ROT[8]);
  t = state[4] ^ d[4];
  bc[2] = std::rotl(t, ROT[14]);

  state[10] = bc[0] ^ (bc[2] & ~bc[1]);
  state[21] = bc[1] ^ (bc[3] & ~bc[2]);
  state[7] = bc[2] ^ (bc[4] & ~bc[3]);
  state[18] = bc[3] ^ (bc[0] & ~bc[4]);
  state[4] = bc[4] ^ (bc[1] & ~bc[0]);

  // Round ridx + 3
  std::fill(bc.begin(), bc.end(), 0x00);

#if defined __clang__
#pragma clang loop unroll(enable)
#pragma clang loop vectorize(enable)
#pragma clang loop interleave(enable)
#elif defined __GNUG__
#pragma GCC unroll 5
#pragma GCC ivdep
#endif
  for (size_t i = 0; i < 25; i += 5) {
    bc[0] ^= state[i + 0];
    bc[1] ^= state[i + 1];
    bc[2] ^= state[i + 2];
    bc[3] ^= state[i + 3];
    bc[4] ^= state[i + 4];
  }

  d[0] = bc[4] ^ std::rotl(bc[1], 1);
  d[1] = bc[0] ^ std::rotl(bc[2], 1);
  d[2] = bc[1] ^ std::rotl(bc[3], 1);
  d[3] = bc[2] ^ std::rotl(bc[4], 1);
  d[4] = bc[3] ^ std::rotl(bc[0], 1);

  bc[0] = state[0] ^ d[0];
  t = state[1] ^ d[1];
  bc[1] = std::rotl(t, ROT[6]);
  t = state[2] ^ d[2];
  bc[2] = std::rotl(t, ROT[12]);
  t = state[3] ^ d[3];
  bc[3] = std::rotl(t, ROT[18]);
  t = state[4] ^ d[4];
  bc[4] = std::rotl(t, ROT[24]);

  state[0] = bc[0] ^ (bc[2] & ~bc[1]) ^ RC[ridx + 3];
  state[1] = bc[1] ^ (bc[3] & ~bc[2]);
  state[2] = bc[2] ^ (bc[4] & ~bc[3]);
  state[3] = bc[3] ^ (bc[0] & ~bc[4]);
  state[4] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[5] ^ d[0];
  bc[2] = std::rotl(t, ROT[10]);
  t = state[6] ^ d[1];
  bc[3] = std::rotl(t, ROT[16]);
  t = state[7] ^ d[2];
  bc[4] = std::rotl(t, ROT[22]);
  t = state[8] ^ d[3];
  bc[0] = std::rotl(t, ROT[3]);
  t = state[9] ^ d[4];
  bc[1] = std::rotl(t, ROT[9]);

  state[5] = bc[0] ^ (bc[2] & ~bc[1]);
  state[6] = bc[1] ^ (bc[3] & ~bc[2]);
  state[7] = bc[2] ^ (bc[4] & ~bc[3]);
  state[8] = bc[3] ^ (bc[0] & ~bc[4]);
  state[9] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[10] ^ d[0];
  bc[4] = std::rotl(t, ROT[20]);
  t = state[11] ^ d[1];
  bc[0] = std::rotl(t, ROT[1]);
  t = state[12] ^ d[2];
  bc[1] = std::rotl(t, ROT[7]);
  t = state[13] ^ d[3];
  bc[2] = std::rotl(t, ROT[13]);
  t = state[14] ^ d[4];
  bc[3] = std::rotl(t, ROT[19]);

  state[10] = bc[0] ^ (bc[2] & ~bc[1]);
  state[11] = bc[1] ^ (bc[3] & ~bc[2]);
  state[12] = bc[2] ^ (bc[4] & ~bc[3]);
  state[13] = bc[3] ^ (bc[0] & ~bc[4]);
  state[14] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[15] ^ d[0];
  bc[1] = std::rotl(t, ROT[5]);
  t = state[16] ^ d[1];
  bc[2] = std::rotl(t, ROT[11]);
  t = state[17] ^ d[2];
  bc[3] = std::rotl(t, ROT[17]);
  t = state[18] ^ d[3];
  bc[4] = std::rotl(t, ROT[23]);
  t = state[19] ^ d[4];
  bc[0] = std::rotl(t, ROT[4]);

  state[15] = bc[0] ^ (bc[2] & ~bc[1]);
  state[16] = bc[1] ^ (bc[3] & ~bc[2]);
  state[17] = bc[2] ^ (bc[4] & ~bc[3]);
  state[18] = bc[3] ^ (bc[0] & ~bc[4]);
  state[19] = bc[4] ^ (bc[1] & ~bc[0]);

  t = state[20] ^ d[0];
  bc[3] = std::rotl(t, ROT[15]);
  t = state[21] ^ d[1];
  bc[4] = std::rotl(t, ROT[21]);
  t = state[22] ^ d[2];
  bc[0] = std::rotl(t, ROT[2]);
  t = state[23] ^ d[3];
  bc[1] = std::rotl(t, ROT[8]);
  t = state[24] ^ d[4];
  bc[2] = std::rotl(t, ROT[14]);

  state[20] = bc[0] ^ (bc[2] & ~bc[1]);
  state[21] = bc[1] ^ (bc[3] & ~bc[2]);
  state[22] = bc[2] ^ (bc[4] & ~bc[3]);
  state[23] = bc[3] ^ (bc[0] & ~bc[4]);
  state[24] = bc[4] ^ (bc[1] & ~bc[0]);
}

// Keccak-p[1600, 24] permutation, applying 24 rounds of permutation on state of dimension 5 x 5 x 64 ( = 1600 ) -bits,
// using algorithm 7 defined in section 3.3 of SHA3 specification https://dx.doi.org/10.6028/NIST.FIPS.202
forceinline constexpr void
permute(uint64_t state[LANE_CNT])
{
  constexpr auto STEP_BY = 4;
  static_assert(ROUNDS % STEP_BY == 0);

  for (size_t i = 0; i < ROUNDS; i += STEP_BY) {
    roundx4(state, i);
  }
}

}
