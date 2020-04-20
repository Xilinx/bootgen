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

#ifndef _Keccak_compact_versal_h_
#define _Keccak_compact_versal_h_

#include "Keccak-compact-settings.h"

int Versalcrypto_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen, bool padding);

/*
** API with message queue (inspired from NIST's API)
*/

#endif
