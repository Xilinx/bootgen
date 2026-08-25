//  test_sio.c
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === sio_generic.h (serial) IO

#include "sio_generic.h"

#ifdef SLOTH

//  Bare metal interface for SLoth

#include "sloth_hal.h"

#ifndef SIO_TIMEOUT
#define SIO_TIMEOUT 1000
#endif

//  millisecond timeout for the SIO interface

static int test_soc_sio_tmo = SIO_TIMEOUT;

//  === Open/Close ===

//  Initialize the serial interface. Returns nonzero on failure.

int sio_init() { return 0; }

//  Close and free the serial interface.

void sio_close() { ; }

//  Set timeout in milliseconds. Set to -1 for blocking behavior.

void sio_timeout(int wait_ms) { test_soc_sio_tmo = wait_ms; }

//  === Core Functions ===

//  Read a single byte from serial interface. Timeout returns -1.

int sio_getc()
{
    int i = test_soc_sio_tmo;
    uint32_t t = get_clk_ticks();

    while (!get_uart_rxok()) {
        if (i == 0) return -1;

        asm volatile("wfi");

        if (i > 0 && (get_clk_ticks() - t) >= (SLOTH_CLK / 1000)) {
            t += SLOTH_CLK / 1000;
            i--;
        }
    }

    return get_uart_rx();
}

//  Read at most "count" bytes to "buf". Return number of bytes read.

size_t sio_read(void *buf, size_t count)
{
    int ch;
    size_t i;

    for (i = 0; i < count; i++) {
        ch = sio_getc();
        if (ch < 0) break;
        ((uint8_t *)buf)[i] = ch;
    }

    return i;
}

//  Send a single character.

void sio_putc(int ch)
{
    while (!get_uart_txok()) {
        asm volatile("wfi");
    }

    set_uart_tx(ch);
}

//  Try to send "count" bytes from "buf". Return number of bytes sent.

size_t sio_write(const void *buf, size_t count)
{
    size_t i;

    for (i = 0; i < count; i++) {
        sio_putc(((const uint8_t *)buf)[i]);
    }
    return i;
}

//  === Convenience Functions ===

//  send a string

void sio_puts(const char *s)
{
    while (*s != 0) sio_putc(*s++);
}

//  Hex number with "n" digits (n=0 for dynamic, n<0 for space indent).

void sio_put_hex(uint32_t x, int n)
{
    int i, c;

    if (n == 0) {
        i = 28;                             //  dynamic length
        c = 0;
    } else {
        if (n < 0) {
            n = -n;                         //  leading spaces
            c = ' ';
        } else {
            c = '0';                        //  leading zeros
        }
        i = 4 * (n - 1);                    //  yes, truncate upper half
    }

    while (i >= 4) {                        //  leading digits
        if ((x >> i) & 0xF) break;
        if (c) sio_putc(c);
        i -= 4;
    }

    while (i >= 0) {                        //  the rest
        c = (x >> i) & 0xF;
        c = c < 10 ? c + '0' : c + 'A' - 10;
        sio_putc(c);
        i -= 4;
    }
}

//  write a 32-bit decimal number (avoiding mul and div)

void sio_put_dec(uint32_t x)
{
    int i;
    char c, m;

    m = 0;
    while (x >= 1000000000) {               //  highest digit
        x -= 1000000000;
        m++;
    }
    if (m) sio_putc(m + '0');

    i = 0;

    while (1) {
        c = 0;
        while (x >= 100000000) {            //  get digit
            x -= 100000000;
            c++;
        }
        m |= c;
        c += '0';
        if (i == 8) {
            sio_putc(c);
            break;
        }
        if (m) sio_putc(c);
        x = ((x << 2) + x) << 1;            //  multiply by 10
        i++;
    }
}

#else

//  fancy standard library fallback.
#include <stdio.h>

int sio_init() { return 0; }
void sio_close() { return; }
void sio_timeout(int wait_ms) { (void) wait_ms; return; }
int sio_getc() { return getc(stdin); }
size_t sio_read(void *buf, size_t count) {
    return fread(buf, 1, count, stdin); }
void sio_putc(int ch) { fputc(ch, stdout); }
size_t sio_write(const void *buf, size_t count) {
    return fwrite(buf, 1, count, stdout); }
void sio_puts(const char *s) { fputs(s, stdout); }
void sio_put_hex(uint32_t x, int n) {
    if (n > 0) { fprintf(stdout, "%0*X", n, (unsigned) (x));
    } else if (n < 0) { fprintf(stdout, "%*X", n, (unsigned) (x));
    } else { fprintf(stdout, "%X", (unsigned) (x)); }
}
void sio_put_dec(uint32_t x) { fprintf(stdout, "%u", (unsigned) (x)); }

//  SLOTH
#endif
