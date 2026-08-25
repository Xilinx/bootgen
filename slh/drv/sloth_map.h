//  sloth_map.h
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === memory map

#ifndef _SLOTH_MAP_H_
#define _SLOTH_MAP_H_

#ifndef SLOTH_CLK
#define SLOTH_CLK           100000000
#endif

//  check that synchronized with: test_top.sv

//  128 kB main ram at 0
#define RAM_ADDR            0x00000000
#define MAIN_RAM            0x20000

//  uart
#define UART_TX_ADDR        0x10000000
#define UART_TXOK_ADDR      0x10000004
#define UART_RX_ADDR        0x10000008
#define UART_RXOK_ADDR      0x1000000C

//  cycle counter
#define GET_TICKS_ADDR      0x10000010

//  gpio
#define GPIO_IN_ADDR        0x10000014
#define GPIO_OUT_ADDR       0x10000018

//  === hash accelerators

//  see kecti3_sloth.v
#define KECTI3_BASE_ADDR    0x14000000
#define KTI3_MEMA   0
#define KTI3_MEMB   50
#define KTI3_MEMC   100
#define KTI3_ADRS   152
#define KTI3_SEED   160
#define KTI3_SKSA   168
#define KTI3_SKSB   176
#define KTI3_SKSC   184
#define KTI3_MTOP   192
#define KTI3_CTRL   240
#define KTI3_STAT   240
#define KTI3_TRIG   240
#define KTI3_STOP   241
#define KTI3_SECN   242
#define KTI3_CHNS   243

//  see keccak_sloth.v
#define KECCAK_BASE_ADDR    0x15000000
#define KECC_MEMA   0
#define KECC_ADRS   50
#define KECC_SEED   58
#define KECC_SKSD   66
#define KECC_MTOP   74
#define KECC_CTRL   120
#define KECC_STAT   120
#define KECC_TRIG   120
#define KECC_STOP   121
#define KECC_SECN   122
#define KECC_CHNS   123

//  see sha256_sloth.v
#define SHA256_BASE_ADDR    0x16000000
#define S256_HASH   0
#define S256_MSGB   8
#define S256_MEND   24
#define S256_SEED   24
#define S256_ADRS   32
#define S256_SKSD   40
#define S256_MTOP   48
#define S256_MSH2   64
#define S256_CTRL   120
#define S256_TRIG   120
#define S256_STAT   120
#define S256_SECN   122
#define S256_CHNS   123

//  see sha512_sloth.v
#define SHA512_BASE_ADDR    0x17000000
#define S512_HASH   0
#define S512_MSGB   16
#define S512_MEND   48
#define S512_MSH2   64
#define S512_CTRL   120
#define S512_TRIG   120
#define S512_STAT   120

//  _SLOTH_MAP_H_
#endif

