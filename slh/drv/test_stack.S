//  test_stack.S
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === Call fill() to fill the stack, and probe() to check fill level.

    .option nopic
    .attribute arch, "rv32i2p1_m2p0_c2p0"
    .text
    .align  1

//  C prototype:    void test_stack_fill(size_t siz, uint8_t fill);

    .globl  test_stack_fill
    .type   test_stack_fill, @function

test_stack_fill:
    beq     a0, zero, .fill_zero
    sub     a5, sp, a0
.fill_loop:
    sb      a1, 0(a5)
    addi    a5, a5, 1
    bne     a5, sp, .fill_loop
    li      a0, 0
.fill_zero:
    jr      ra
    .size   test_stack_fill, .-test_stack_fill


//  C prototype:    size_t test_stack_probe(size_t siz, uint8_t fill);

    .globl  test_stack_probe
    .type   test_stack_probe, @function

test_stack_probe:
    beq     a0, zero, .probe_zero
    sub     a5, sp, a0

.probe_loop:
    lbu     a2, 0(a5)
    bne     a1, a2, .probe_done
    addi    a5, a5, 1
    bne     a5, sp, .probe_loop

.probe_done:
    sub     a0, sp, a5
.probe_zero:
    jr      ra
    .size   test_stack_probe, .-test_stack_probe



