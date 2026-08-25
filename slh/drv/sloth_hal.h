//  sloth_hal.h
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === A minimal hardware abstraction layer

#ifndef _SLOTH_HAL_H_
#define _SLOTH_HAL_H_


#include <stdint.h>
#include <stddef.h>

#include "sio_generic.h"

#ifdef  SLOTH
#include "sloth_map.h"

//  wait for an operation

#define KTI3_WAIT { while (r32[KTI3_STAT] != 0) ; }
#define KECC_WAIT { while (r32[KECC_STAT] != 0) ; }
#define S256_WAIT { while (r32[S256_STAT] != 0) ; }
#define S512_WAIT { while (r32[S512_STAT] != 0) ; }

//  uart
#define set_uart_tx(x)  \
    {   *((volatile char *)UART_TX_ADDR) = (x); }
#define get_uart_txok() (*((volatile char *)UART_TXOK_ADDR))
#define get_uart_rx()   (*((volatile char *)UART_RX_ADDR))
#define get_uart_rxok() (*((volatile char *)UART_RXOK_ADDR))

//  get ticks
#define get_clk_ticks() (*((volatile uint32_t *)GET_TICKS_ADDR))

//  gpio
#define get_gpio_in(x)  (*((volatile uint32_t *)GPIO_IN_ADDR))
#define set_gpio_out(x) \
    {   *((volatile uint32_t *)GPIO_OUT_ADDR) = (x);    }

//  millisecond-resolution delay
void delayms(uint32_t ms);

#else
//  running in host
#define get_clk_ticks() 0
#endif

//  fixed-length word-wise block copy macros are faster than memcpy

static inline void block_copy_16(volatile void *dst, const volatile void *src)
{
    volatile uint32_t *d32 = (volatile uint32_t *) dst;
    volatile uint32_t *s32 = (volatile uint32_t *) src;

    d32[0] = s32[0];
    d32[1] = s32[1];
    d32[2] = s32[2];
    d32[3] = s32[3];
}

static inline void block_copy_24(volatile void *dst, const volatile void *src)
{
    volatile uint32_t *d32 = (volatile uint32_t *) dst;
    volatile uint32_t *s32 = (volatile uint32_t *) src;

    d32[0] = s32[0];
    d32[1] = s32[1];
    d32[2] = s32[2];
    d32[3] = s32[3];
    d32[4] = s32[4];
    d32[5] = s32[5];
}

static inline void block_copy_32(volatile void *dst, const volatile void *src)
{
    volatile uint32_t *d32 = (volatile uint32_t *) dst;
    volatile uint32_t *s32 = (volatile uint32_t *) src;

    d32[0] = s32[0];
    d32[1] = s32[1];
    d32[2] = s32[2];
    d32[3] = s32[3];
    d32[4] = s32[4];
    d32[5] = s32[5];
    d32[6] = s32[6];
    d32[7] = s32[7];
}

static inline void block_copy_n(volatile void *dst, const volatile void *src,
                                uint32_t n)
{
    volatile uint32_t *d32 = (volatile uint32_t *) dst;
    volatile uint32_t *s32 = (volatile uint32_t *) src;

    d32[0] = s32[0];
    d32[1] = s32[1];
    d32[2] = s32[2];
    d32[3] = s32[3];
    if (n == 16)
        return;
    d32[4] = s32[4];
    d32[5] = s32[5];
    if (n == 24)
        return;
    d32[6] = s32[6];
    d32[7] = s32[7];
}

static inline void block_copy_64(volatile void *dst, const volatile void *src)
{
    volatile uint32_t *d32 = (volatile uint32_t *) dst;
    volatile uint32_t *s32 = (volatile uint32_t *) src;

    block_copy_32( d32,     s32 );
    block_copy_32( d32 + 8, s32 + 8 );
}
//  _SLOTH_HAL_H_
#endif
