//  test_leak.c
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === Doodles for leakage assessment

#include "plat_local.h"
#include "slh_dsa.h"
#include "slh_ctx.h"
#include "slh_adrs.h"
#include "test_rvkat.h"
#include "kat_drbg.h"
#include "sloth_hal.h"
#include  <string.h>

//  test_trig.S
void test_trig(uint32_t x, volatile uint32_t *p);

//  my over-engineered trigger pulses in otherwise under-engineered sca setup
#define TRIG_SIGNAL_F   0x94A44891
#define TRIG_SIGNAL_R   0x92444891
#define TRIG_SIGNAL_END 0x4A924891

#define TRIG_PULSE(x) { \
        test_trig((x), (volatile uint32_t *) GPIO_OUT_ADDR); }

//  fake test drbg state
aes256_ctr_drbg_t sca_drbg;

//  SLH parameters
#define MAX_SIGN 50000

int test_leak()
{
    int fail = 0;
    int i;

    uint8_t digest[SLH_MAX_M];
    uint8_t sk[4 * 32] = { 0 };
    uint8_t sig[MAX_SIGN];
    slh_ctx_t   ctx;

    uint32_t trig = 0, cc = 0;

    uint8_t rando[32];
    const slh_param_t *prm = &slh_dsa_shake_128f;

    //  initialize
    aes256ctr_xof_init(&sca_drbg, sk);

    for (i = 0;; i++) {

        rvkat_dec("=== count", i);

        aes256ctr_xof(&sca_drbg, rando, sizeof(rando));
        aes256ctr_xof(&sca_drbg, sk, 4 * prm->n);

        if (rando[0] & 1) {
            //  fixed
            memset(sk, 0x00, prm->n);       //  SK.seed = 0
            trig = TRIG_SIGNAL_F;
        } else {
            //  keep it random
            trig = TRIG_SIGNAL_R;
        }

        //  random digest
        aes256ctr_xof(&sca_drbg, digest, prm->m);

        //  (re)create the context -- set up SK.seed key from plaintext
        prm->mk_ctx(&ctx, NULL, sk, prm);

        //  one invocation of PRF to randomize the masked secret key
        block_copy_32(ctx.adrs->u32, rando);
        prm->prf(&ctx, rando);
        adrs_zero(&ctx);

        //  go go
        cc = get_clk_ticks();
        TRIG_PULSE(trig);
        slh_do_sign(&ctx, sig, digest);
        TRIG_PULSE(TRIG_SIGNAL_END);
        cc = get_clk_ticks() - cc;

        sio_puts("[STAT] clk= ");
        sio_put_dec(cc);
        sio_puts(" trig=");
        rvkat_hexu32(trig);
        sio_putc('\n');
    }

    return fail;
}
