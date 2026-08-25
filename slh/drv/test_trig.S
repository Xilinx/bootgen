//  test_trig.S
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === this is the "go" pulse signal, which is able to encode
//      little bit of data into the traces.

    .option nopic
//  .attribute arch, "rv32i2p0_m2p0_c2p0"
    .attribute unaligned_access, 0
    .text
    .balign 16
    .globl  test_trig
    .type   test_trig, @function

//  note: the three instructions should be 4+2+2 = 8 bytes total (c ext).

test_trig:
    .rept   32
    andi    a4,a0,1
    sw      a4,0(a1)
    srli    a0,a0,1
    .endr

    ret
    .size   test_trig, .-test_trig

