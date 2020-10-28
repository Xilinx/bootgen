/*
The Keccak sponge function, designed by Guido Bertoni, Joan Daemen,
Michaël Peeters and Gilles Van Assche. For more information, feedback or
questions, please refer to our website: http://keccak.noekeon.org/

Implementation by Ronny Van Keer,
hereby denoted as "the implementer".

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/


#include    <string.h>
#include "Keccak-compact-settings.h"
#include "Keccak-compact-versal.h"
#define cKeccakR_SizeInBytes    (cKeccakR / 8)
//#include "crypto_hash.h"
#ifndef crypto_hash_BYTES
#ifdef cKeccakFixedOutputLengthInBytes
#define crypto_hash_BYTES cKeccakFixedOutputLengthInBytes
#else
#define crypto_hash_BYTES cKeccakR_SizeInBytes
#endif
#endif
#if (crypto_hash_BYTES > cKeccakR_SizeInBytes)
#error "Full squeezing not yet implemented"
#endif

#define IS_BIG_ENDIAN      4321 /* byte 0 is most significant (mc68k) */
#define IS_LITTLE_ENDIAN   1234 /* byte 0 is least significant (i386) */
#define PLATFORM_BYTE_ORDER IS_LITTLE_ENDIAN /* WARNING: This implementation works on little-endian platform. Support for big-endinanness is implemented, but not tested. */

#if     (cKeccakB   == 1600)
typedef unsigned long long  UINT64;
typedef UINT64 tKeccakLane;
#define cKeccakNumberOfRounds   24
#elif   (cKeccakB   == 800)
typedef unsigned int        UINT32;
// WARNING: on 8-bit and 16-bit platforms, this should be replaced by:
//typedef unsigned long       UINT32;
typedef UINT32 tKeccakLane;
#define cKeccakNumberOfRounds   22
#elif   (cKeccakB   == 400)
typedef unsigned short      UINT16;
typedef UINT16 tKeccakLane;
#define cKeccakNumberOfRounds   20
#elif   (cKeccakB   == 200)
typedef unsigned char       UINT8;
typedef UINT8 tKeccakLane;
#define cKeccakNumberOfRounds   18
#else
#error  "Unsupported Keccak-f width"
#endif

typedef unsigned int tSmallUInt; /*INFO It could be more optimized to use "unsigned char" on an 8-bit CPU	*/


#define cKeccakLaneSizeInBits   (sizeof(tKeccakLane) * 8)

#define ROL(a, offset) (tKeccakLane)((((tKeccakLane)a) << ((offset) % cKeccakLaneSizeInBits)) ^ (((tKeccakLane)a) >> (cKeccakLaneSizeInBits-((offset) % cKeccakLaneSizeInBits))))

void KeccakF(tKeccakLane * state, const tKeccakLane *in, int laneCount);


int Versalcrypto_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen, bool padding)
{
    tKeccakLane state[5 * 5];

#if (crypto_hash_BYTES >= cKeccakR_SizeInBytes)
#define temp out
#else
    unsigned char 	temp[cKeccakR_SizeInBytes];
#endif

    memset(state, 0, sizeof(state));

    for ( /* empty */; inlen >= cKeccakR_SizeInBytes; inlen -= cKeccakR_SizeInBytes, in += cKeccakR_SizeInBytes)
    {
        KeccakF(state, (const tKeccakLane*)in, cKeccakR_SizeInBytes / sizeof(tKeccakLane));
    }

    if (padding)
    {
        /*Last data and padding*/
        memcpy(temp, in, (size_t)inlen);
        temp[inlen++] = 6;
        if (inlen < cKeccakR_SizeInBytes)
        {
            memset(temp + inlen, 0, cKeccakR_SizeInBytes - (size_t)inlen);
        }
        temp[cKeccakR_SizeInBytes - 1] |= 0x80;
        KeccakF(state, (const tKeccakLane*)temp, cKeccakR_SizeInBytes / sizeof(tKeccakLane));
    }

#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN) || (cKeccakB == 200)
    memcpy(out, state, crypto_hash_BYTES);
#else
    for (i = 0; i < (crypto_hash_BYTES / sizeof(tKeccakLane)); ++i)
    {
        tSmallUInt j;
        tKeccakLane t;
        t = state[i];
        for (j = 0; j < sizeof(tKeccakLane); ++j)
        {
            *(out++) = (unsigned char)t;
            t >>= 8;
        }
    }
#endif
#if (crypto_hash_BYTES >= cKeccakR_SizeInBytes)
#undef temp
#endif
    return (0);
}
