//  test_bench.c
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === Tests for SLotH: KAT, clock cycles, stack use

#include <string.h>
#include <stdlib.h>

#include "test_rvkat.h"
#include "slh_dsa.h"
#include "kat_drbg.h"
#include "sloth_hal.h"

//  how many tests (short cksums exist only for 10)

#ifndef KATNUM
#define KATNUM 10
#endif

//  maximum signature length

#define MAX_SIGN 50000

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

//  fake NIST DRBG state
aes256_ctr_drbg_t kat_drbg, iut_drbg;

//  for the callback interface
int iut_randombytes(uint8_t *x, size_t xlen)
{
    aes256ctr_xof(&iut_drbg, x, xlen);
    return 0;
}

//  creates NIST format KAT output

int kat_test(const slh_param_t *iut, int katnum)
{
    int fail = 0;

    uint8_t seed[48] = { 0 };
    uint8_t msg[33 * KATNUM] = { 0 };
    size_t  msg_sz= 0, pk_sz = 0, sk_sz = 0, sig_sz = 0, sm_sz = 0;

    uint8_t pk[2 * 32] = { 0 };
    uint8_t sk[4 * 32] = { 0 };
    uint8_t sm[MAX_SIGN + 33 * KATNUM];

    rvkat_info(slh_alg_id(iut));
    sio_puts("# SPHINCS+\n\n");

    //  initialize kat seed drbg
    for (int i = 0; i < 48; i++) {
        seed[i] = i;
    }
    aes256ctr_xof_init(&kat_drbg, seed);

    for (int count = 0; count < katnum; count++) {

        rvkat_dec("count", count);

        aes256ctr_xof(&kat_drbg, seed, 48);
        rvkat_hex("seed", seed, 48);

        msg_sz = (count + 1) * 33;
        rvkat_dec("mlen", msg_sz);

        aes256ctr_xof(&kat_drbg, msg, msg_sz);

        //  initialize target drbg
        aes256ctr_xof_init(&iut_drbg, seed);

        pk_sz = slh_pk_sz(iut);
        sk_sz = slh_sk_sz(iut);
        sig_sz = slh_sig_sz(iut);
        slh_keygen(pk, sk, &iut_randombytes, iut);

        rvkat_hex("pk", pk, pk_sz);
        rvkat_hex("sk", sk, sk_sz);

        sm_sz = slh_sign(sm, msg, msg_sz, sk, &iut_randombytes, iut);
        memcpy(sm + sm_sz, msg, msg_sz);
        sm_sz += msg_sz;
        rvkat_dec("smlen", sm_sz);

        rvkat_hex("sm", sm, sm_sz);

        if (!slh_verify(sm + sig_sz, msg_sz, sm, pk, iut)) {
            fail += rvkat_chku32("slh_verify()", true, false);
        }
#if 1
        //  flip random bit
        uint32_t xbit = ((uint32_t) seed[4]) +
                        (((uint32_t) seed[5]) <<  8) +
                        (((uint32_t) seed[6]) << 16) +
                        (((uint32_t) seed[7]) << 24);
        xbit %= (8 * sm_sz);
        sm[xbit >> 3] ^= 1 << (xbit & 7);
        if (slh_verify(sm + sig_sz, msg_sz, sm, pk, iut)) {
            fail++;
            rvkat_dec("[FAIL]\tslh_verify() flip bit", xbit);
        }
#endif
    }

    return fail;
}


//  stack measurement:
#define STACK_FILL_SIZE 9999
#define STACK_FILL_BYTE 0xEE

//  test_stack.S
void test_stack_fill(size_t siz, uint8_t fill);
size_t test_stack_probe(size_t siz, uint8_t fill);

//  print info
void clk_label(const char *alg, const char *lab, uint32_t cc, uint32_t stk)
{
    sio_puts("[CLK]\t");
    sio_puts(alg);
    sio_putc(' ');
    sio_put_dec(cc);
    sio_putc(' ');
    sio_puts(lab);
    sio_putc('\n');

    sio_puts("[STK]\t");
    sio_puts(alg);
    sio_putc(' ');
    sio_put_dec(stk);
    sio_putc(' ');
    sio_puts(lab);
    sio_putc('\n');
}

int chk_test(const slh_param_t *iut, const uint32_t cksum[][2], int katnum)
{
    int fail = 0;

    uint8_t seed[48] = { 0 };
    uint8_t msg[33 * KATNUM] = { 0 };
    size_t  msg_sz= 0, /*pk_sz = 0,*/ sk_sz = 0, sig_sz = 0, sm_sz = 0;

    uint8_t pk[2 * 32] = { 0 };
    uint8_t sk[4 * 32] = { 0 };
    uint8_t sm[MAX_SIGN + 33 * KATNUM];
    uint32_t cc, stk;
    bool    ok;

    rvkat_info(slh_alg_id(iut));

    //  initialize kat seed drbg
    for (int i = 0; i < 48; i++) {
        seed[i] = i;
    }
    aes256ctr_xof_init(&kat_drbg, seed);

    //pk_sz = slh_pk_sz(iut);
    sk_sz = slh_sk_sz(iut);
    sig_sz = slh_sig_sz(iut);

    for (int count = 0; count < katnum; count++) {

        rvkat_dec("[INFO]\tkat test count", count);

        //  create seed and the message
        aes256ctr_xof(&kat_drbg, seed, 48);
        msg_sz = (count + 1) * 33;
        aes256ctr_xof(&kat_drbg, msg, msg_sz);

        aes256ctr_xof_init(&iut_drbg, seed);

        //  KeyGen
        test_stack_fill(STACK_FILL_SIZE, STACK_FILL_BYTE);
        cc = get_clk_ticks();
        slh_keygen(pk, sk, &iut_randombytes, iut);
        cc = get_clk_ticks() - cc;
        stk = test_stack_probe(STACK_FILL_SIZE, STACK_FILL_BYTE);
        //  ---

        clk_label(slh_alg_id(iut), "slh_keygen()", cc, stk);

        if (count < 10) {
            fail += rvkat_chku32(   "sk", cksum[count][0],
                                    rvkat_cksum(sk, sk_sz));
        }

        //  Sign
        test_stack_fill(STACK_FILL_SIZE, STACK_FILL_BYTE);
        cc = get_clk_ticks();
        sm_sz = slh_sign(sm, msg, msg_sz, sk, &iut_randombytes, iut);
        cc = get_clk_ticks() - cc;
        stk = test_stack_probe(STACK_FILL_SIZE, STACK_FILL_BYTE);
        //  ---

        clk_label(slh_alg_id(iut), "slh_sign()", cc, stk);
        memcpy(sm + sm_sz, msg, msg_sz);
        sm_sz += msg_sz;

        if (count < 10) {
            fail += rvkat_chku32(   "sm", cksum[count][1],
                                    rvkat_cksum(sm, sm_sz));
        }

        //  Verify
        test_stack_fill(STACK_FILL_SIZE, STACK_FILL_BYTE);
        cc = get_clk_ticks();
        ok = slh_verify(sm + sig_sz, msg_sz, sm, pk, iut);
        cc = get_clk_ticks() - cc;
        stk = test_stack_probe(STACK_FILL_SIZE, STACK_FILL_BYTE);
        //  ---

        clk_label(slh_alg_id(iut), "slh_verify()", cc, stk);
        if (!ok) {
            fail++;
            rvkat_dec("[FAIL]\tslh_verify() returned", ok);
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
            rvkat_dec("[FAIL]\tslh_verify() flip bit", xbit);
        } else {
            rvkat_dec("[PASS]\tslh_verify() flip bit", xbit);
        }
    }

    return fail;
}

//  fake rng for speed and sca measurement

uint32_t fibo_a = 0x01234567, fibo_b = 0xDEADBEEF;

int clk_randombytes(uint8_t *x, size_t xlen)
{
    size_t i;
    uint32_t a, b;
    a = fibo_a;
    b = fibo_b;

    for (i = 0; i < xlen; i++) {
        x[i] = a >> 24;
        a += b;
        b += a;
    }
    fibo_a = a;
    fibo_b = b;

    return 0;
}

int clk_test(const slh_param_t *iut, int n)
{
    int fail = 0;

    uint8_t msg[2] = { 0, 0 };
    size_t  msg_sz= 2, /*pk_sz = 0, sk_sz = 0,*/ sig_sz = 0, sm_sz = 0;

    uint8_t pk[2 * 32] = { 0 };
    uint8_t sk[4 * 32] = { 0 };
    uint8_t sm[MAX_SIGN + 2];
    uint32_t cc, stk;
    bool    ok;

    rvkat_info(slh_alg_id(iut));

    //pk_sz = slh_pk_sz(iut);
    //sk_sz = slh_sk_sz(iut);
    sig_sz = slh_sig_sz(iut);

    for (int count = 0; count < n; count++) {

        fibo_a = 0x01234567 * count;
        fibo_b = 0xDEADBEEF;

        rvkat_dec("[INFO]\tclk test count", count);

        msg[0] = count >> 8;
        msg[1] = count & 0xFF;

        //  KeyGen
        test_stack_fill(STACK_FILL_SIZE, STACK_FILL_BYTE);
        cc = get_clk_ticks();
        slh_keygen(pk, sk, &clk_randombytes, iut);
        cc = get_clk_ticks() - cc;
        stk = test_stack_probe(STACK_FILL_SIZE, STACK_FILL_BYTE);
        //  ---

        clk_label(slh_alg_id(iut), "slh_keygen()", cc, stk);

        //  Sign
        test_stack_fill(STACK_FILL_SIZE, STACK_FILL_BYTE);
        cc = get_clk_ticks();
        sm_sz = slh_sign(sm, msg, msg_sz, sk, &clk_randombytes, iut);
        cc = get_clk_ticks() - cc;
        stk = test_stack_probe(STACK_FILL_SIZE, STACK_FILL_BYTE);
        //  ---

        clk_label(slh_alg_id(iut), "slh_sign()", cc, stk);

        memcpy(sm + sm_sz, msg, msg_sz);
        sm_sz += msg_sz;

        //  Verify
        test_stack_fill(STACK_FILL_SIZE, STACK_FILL_BYTE);
        cc = get_clk_ticks();
        ok = slh_verify(sm + sig_sz, msg_sz, sm, pk, iut);
        cc = get_clk_ticks() - cc;
        stk = test_stack_probe(STACK_FILL_SIZE, STACK_FILL_BYTE);
        //  ---

        clk_label(slh_alg_id(iut), "slh_verify()", cc, stk);
        if (!ok) {
            fail++;
            rvkat_dec("[FAIL]\tslh_verify() returned", ok);
        }
    }

    return fail;
}


/*
    These are 32-bit cksums pairs (sk, sm) matching the the data of of the
    NIST-format ".rsp" test vectors. For SLH-DSA, the Secret key sk is of
    size 4*n and also contains the public key. The signed message sm is
    actually a concatenation (sig || msg) of the signature with the message
    to be signed itself (derived from a deternistic generator). In those
    test vectors the message size mlen is 33*i = 33, 66, ..
*/

const uint32_t kat_sksm_cksum[12][10][2] =
{
    {   //  0:  SLH-DSA-SHAKE-128s
        { 0x7139D058, 0xCECD2289 }, { 0x04DB860A, 0x2B5D75C3 },
        { 0x750FC8A8, 0xB52CBF8B }, { 0x8A07C209, 0x4782CD29 },
        { 0x0D85477F, 0xB7184CC0 }, { 0x5B509D42, 0xDB98DC7A },
        { 0xA8F879DD, 0x993D0B80 }, { 0x0D0688C4, 0x04E4CFA5 },
        { 0x95651532, 0xE9A19793 }, { 0x32299412, 0xC9AE5CE9 }
    },

    {   //  1:  SLH-DSA-SHAKE-128f
        { 0xBCA6B2C3, 0xC03DA016 }, { 0x3364979A, 0x93E308F4 },
        { 0x32372854, 0x04EAED18 }, { 0x4B3A6954, 0xCCD470DE },
        { 0x29BF3EE0, 0xEA10D0B7 }, { 0xDA05B3CE, 0xEAC89C1A },
        { 0x2BB3B8F4, 0x799A2BD6 }, { 0x79E4405D, 0xDD39FA5B },
        { 0x81B427F3, 0x0658FA84 }, { 0x14BEBAF4, 0xEC07B74C }
    },

    {   //  2:  SLH-DSA-SHAKE-192s
        { 0x633A3551, 0x63309A27 }, { 0xD4913219, 0x5DF827D2 },
        { 0xBBAA287C, 0x1BA3669C }, { 0x3A8283C4, 0xD16E9C4D },
        { 0x6E8249F3, 0x12828060 }, { 0x96C8D773, 0x4375D105 },
        { 0x879CDE67, 0x0AFA6CC9 }, { 0xF50875D0, 0x2916E162 },
        { 0x1D2D26FC, 0xEC944449 }, { 0xD1061AC2, 0xD467A2FB }
    },

    {   //  3:  SLH-DSA-SHAKE-192f
        { 0x450D7842, 0x63F845FD }, { 0xB3B8FEC6, 0x8E30B463 },
        { 0x7F1D810C, 0xCFBBC3D4 }, { 0xA07B127C, 0x3C5F6691 },
        { 0xC91C9A30, 0xE413F536 }, { 0x8DEF2EB7, 0x7D96A366 },
        { 0x9E43038E, 0xEFB925EB }, { 0xC15F47FE, 0x2EA12900 },
        { 0x0087D725, 0x4DA1AFA5 }, { 0x9D069644, 0x0AE3415B }
    },

    {   //  4:  SLH-DSA-SHAKE-256s
        { 0x80885768, 0xD7342B60 }, { 0xB5CC8BBF, 0xB50288DB },
        { 0xFFC92D06, 0x12753DDE }, { 0x5B04A6B2, 0x71573961 },
        { 0xB4D63DEB, 0x9045ED9E }, { 0xE6C47AD3, 0xDD25A9EC },
        { 0xB121DFD5, 0xEB1107C9 }, { 0x8C854AD5, 0x00469DA0 },
        { 0x10A6557D, 0x21526DD6 }, { 0xF2D607E6, 0x9885950A }
    },

    {   //  5:  SLH-DSA-SHAKE-256f
        { 0xC33B72F6, 0x88D645E6 }, { 0xE362F046, 0xFCA8D620 },
        { 0x718D28D1, 0xA5F18386 }, { 0x0177710A, 0x1756E6AA },
        { 0x4B8D6A16, 0x2C0CADC7 }, { 0x3B385A25, 0xEC30B038 },
        { 0x0727DDE0, 0x817AED48 }, { 0xD037CFCC, 0x50042FFF },
        { 0x0D732E05, 0x9E902778 }, { 0x149B2F8A, 0xC72B3CFE }
    },

    {   //  6:  SLH-DSA-SHA2-128s
        { 0x8627D031, 0x7FFFF721 }, { 0x931A5BE2, 0xFC3C993B },
        { 0xF4C5E15C, 0xF2DCC265 }, { 0x929E3B7B, 0x5C667BD2 },
        { 0x7631B3C7, 0xD2142CC9 }, { 0x51FE8A25, 0xA9CF39DE },
        { 0x125A8C14, 0x573AC530 }, { 0xC532E8B8, 0x5E48EE43 },
        { 0x7C6FC89C, 0x67C74063 }, { 0x084C8A2E, 0x2623797E }
    },

    {   //  7:  SLH-DSA-SHA2-128f
        { 0xAC48BA77, 0xF3D144D9 }, { 0xAF4240D7, 0xE738E1C3 },
        { 0x01A80309, 0x98B3C248 }, { 0x7F9E7F3F, 0xC8C480C6 },
        { 0x9997E1FC, 0x73B0FCE7 }, { 0xDF396509, 0xDEAE146A },
        { 0x5718F320, 0xF7DC370F }, { 0x12F11542, 0x6F493BD6 },
        { 0x461E0E93, 0x56F7C2F6 }, { 0x89741C32, 0x77EF284C }
    },

    {   //  8:  SLH-DSA-SHA2-192s
        { 0xFB2714A9, 0x384767AC }, { 0xB5177020, 0x54A94CA7 },
        { 0xDBF4AD8E, 0x9229CC65 }, { 0x4345D49D, 0x1D5336CA },
        { 0x2F5024F5, 0x9801796B }, { 0x00AD5763, 0xC4220434 },
        { 0x87BEE287, 0xC70F855F }, { 0x8AF2610E, 0x956CB32D },
        { 0xD10A83AD, 0x1FB62038 }, { 0x8B599826, 0x3C33A14F }
    },

    {   //  9:  SLH-DSA-SHA2-192f
        { 0xE99D5038, 0x7A0A5E0B }, { 0xAF2AD49C, 0x2B1EB751 },
        { 0x2D890971, 0xED30B612 }, { 0x734A4AC8, 0x7A31DC65 },
        { 0xF4783313, 0x65150AF4 }, { 0x81633CCC, 0xAD5E4F2E },
        { 0x0B56464D, 0x59BD37D8 }, { 0x330F3A2E, 0xF4B39660 },
        { 0x2F3F1828, 0x732DE020 }, { 0x7783FB31, 0xADFE6529 }
    },

    {   //  10: SLH-DSA-SHA2-256s
        { 0x2C4DB31B, 0x26FA7A7F }, { 0xA53E25E6, 0xE751B203 },
        { 0xDB68CCF8, 0xFE0018F7 }, { 0x52314BD4, 0x84FFB529 },
        { 0x690565A1, 0xB0FC0AB3 }, { 0x184CC51C, 0xA4177149 },
        { 0xFEB8CAFA, 0xC337DE2A }, { 0x97E922D6, 0xC74D4F02 },
        { 0x8CBB0E91, 0x1EE05E0C }, { 0xE5406E5C, 0xBB498161 }
    },

    {   //  11: SLH-DSA-SHA2-256f
        { 0x71DB7EBA, 0x4473CB28 }, { 0x883CAD60, 0x3BF0DCB2 },
        { 0xFDE0BD44, 0xB0843413 }, { 0xC6BCF24F, 0x4B8B98B2 },
        { 0xD13AB845, 0xBDEB2868 }, { 0x234AA9CB, 0x7F6E0A73 },
        { 0x633010D5, 0x57238C89 }, { 0x28908504, 0xA35634B5 },
        { 0x7090B719, 0xBB4B1544 }, { 0xD4D29D09, 0xE785DC8F }
    },
};


//  this function is

int test_bench()
{
    int fail = 0;
    int iut_n = 0;

#ifdef SLOTH

#if 1
    //  SLH-DSA-SHAKE-128f for a quick test with KATs
    iut_n = 1;
    fail += chk_test(   test_iut[iut_n],
                        kat_sksm_cksum[iut_n], 1);
    return fail;
#endif

#if 0
    //  for clock cycle measurement
    for (iut_n = 0; test_iut[iut_n] != NULL; iut_n++) {
        fail += clk_test(   test_iut[iut_n], KATNUM);
    }
#endif

#if 0
    //  for the kat checksums
    for (iut_n = 0; test_iut[iut_n] != NULL; iut_n++) {
        fail += chk_test(test_iut[iut_n], kat_sksm_cksum[iut_n], KATNUM);
    }
#endif

//  SLOTH
#endif

    return fail;
}

