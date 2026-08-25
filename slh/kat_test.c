//  kat_test.c
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === KAT Testing for SLH-DSA

#ifndef SLOTH

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "slh_dsa.h"
#include "kat_drbg.h"

#ifndef KATNUM
#define KATNUM 100
#endif

//  fake test drbg state
aes256_ctr_drbg_t kat_drbg, iut_drbg;

//  for the callback interface
int iut_randombytes(uint8_t *x, size_t xlen)
{
    aes256ctr_xof(&iut_drbg, x, xlen);
    return 0;
}

static void kat_hex(FILE *fh, const char *label,
                    const uint8_t *x, size_t xlen)
{
    size_t i;
    fprintf(fh, "%s = ", label);
    for (i = 0; i < xlen; i++) {
        fprintf(fh, "%02X", x[i]);
    }
    fprintf(fh, "\n");
}

int kat_test(const slh_param_t *iut, int katnum)
{
    int fail = 0;

    char fn[256];
    FILE *fh;

    uint8_t seed[48] = { 0 };
    uint8_t msg[33 * KATNUM] = { 0 };
    size_t msg_sz= 0, pk_sz = 0, sk_sz = 0, sig_sz = 0, sm_sz = 0;

    uint8_t pk[2 * 32] = { 0 };
    uint8_t sk[4 * 32] = { 0 };
    uint8_t sm[50000 + 33 * KATNUM];

    snprintf(fn, sizeof(fn), "%s-%d.rsp", slh_alg_id(iut), katnum);
    fh = fopen(fn, "w");
    if (fh == NULL) {
        perror(fn);
        fail++;
        return fail;
    }

    fprintf(fh, "# SPHINCS+\n\n");

    //  initialize kat seed drbg
    for (int i = 0; i < 48; i++) {
        seed[i] = i;
    }
    aes256ctr_xof_init(&kat_drbg, seed);

    pk_sz = slh_pk_sz(iut);
    assert(sizeof(pk) >= pk_sz);
    sk_sz = slh_sk_sz(iut);
    assert(sizeof(sk) >= sk_sz);
    sig_sz = slh_sig_sz(iut);

    for (int count = 0; count < katnum; count++) {

        printf("[KAT] (%d) %s\n", count, fn);
        fflush(stdout);

        fprintf(fh, "count = %d\n", count);

        aes256ctr_xof(&kat_drbg, seed, 48);
        kat_hex(fh, "seed", seed, 48);

        msg_sz = (count + 1) * 33;
        fprintf(fh, "mlen = %zu\n", msg_sz);
        assert(sizeof(sm) >= sig_sz + msg_sz);

        aes256ctr_xof(&kat_drbg, msg, msg_sz);
        kat_hex(fh, "msg", msg, msg_sz);

        //  initialize target drbg
        aes256ctr_xof_init(&iut_drbg, seed);
        slh_keygen(pk, sk, &iut_randombytes, iut);

        kat_hex(fh, "pk", pk, pk_sz);
        kat_hex(fh, "sk", sk, sk_sz);

        sm_sz = slh_sign(sm, msg, msg_sz, sk, &iut_randombytes, iut);

        memcpy(sm + sm_sz, msg, msg_sz);
        sm_sz += msg_sz;
        fprintf(fh, "smlen = %zu\n", sm_sz);

        kat_hex(fh, "sm", sm, sm_sz);
        fprintf(fh, "\n");
        assert(sm_sz == sig_sz + msg_sz);

        if (!slh_verify(sm + sig_sz, msg_sz, sm, pk, iut)) {
            fail++;
            fprintf(stderr, "[FAIL] slh_verify() fails.\n");
        }

        //  flip random bit
        uint32_t xbit = ((uint32_t) seed[4]) +
                        (((uint32_t) seed[5]) <<  8) +
                        (((uint32_t) seed[6]) << 16) +
                        (((uint32_t) seed[7]) << 24);
        xbit %= (8 * sm_sz);
        sm[xbit >> 3] ^= 1 << (xbit & 7);
        if (slh_verify(sm + sig_sz, msg_sz, sm, pk, iut)) {
            fail++;
            fprintf(stderr, "[FAIL] slh_verify() forgery bit= %u.\n", xbit);
        }
    }
    fclose(fh);

    return fail;
}

//  test targets

const slh_param_t *test_iut[] = {
    &slh_dsa_shake_128s,
    &slh_dsa_shake_128f,
    &slh_dsa_shake_192s,
    &slh_dsa_shake_192f,
    &slh_dsa_shake_256s,
    &slh_dsa_shake_256f,
    &slh_dsa_sha2_128s,
    &slh_dsa_sha2_128f,
    &slh_dsa_sha2_192s,
    &slh_dsa_sha2_192f,
    &slh_dsa_sha2_256s,
    &slh_dsa_sha2_256f,
    NULL
};

int main(int argc, char **argv)
{
    int fail = 0;
    int iut_n = 0;

    if  (argc == 2 &&
        (iut_n = atoi(argv[1])) >= 0 &&
        iut_n < 12) {
        fail += kat_test(test_iut[iut_n], 1);
    } else {
        for (iut_n = 0; test_iut[iut_n] != NULL; iut_n++) {
            fail += kat_test(test_iut[iut_n], KATNUM);
        }
    }

    printf("[INFO] test_slh_dsa() fail= %d\n", fail);

    return fail;
}

#endif
