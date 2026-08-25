//  sio_generic.h
//  Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.

//  === Generic Serial I/O interface -- both real and emulated interfaces.

#ifndef _SIO_GENERIC_H_
#define _SIO_GENERIC_H_

#include <stddef.h>
#include <stdint.h>

//  === Open/Close ===

//  Initialize the serial interface. Returns nonzero on failure.
int sio_init();

//  Close and free the serial interface.
void sio_close();

//  Set timeout in milliseconds. Set to -1 for blocking behavior.
void sio_timeout(int wait_ms);

//  === Core Functions ===

//  Read a single byte from serial interface. Timeout returns -1.
int sio_getc();

//  Read at most "count" bytes to "buf". Return number of bytes read.
size_t sio_read(void *buf, size_t count);

//  Send a single character.
void sio_putc(int ch);

//  Try to send "count" bytes from "buf". Return number of bytes sent.
size_t sio_write(const void *buf, size_t count);

//  === Convenience Functions ===

//  Output a null-terminated string to serial, no newline.
void sio_puts(const char *s);

//  Hex number with "n" digits (n=0 for dynamic, n<0 for space indent).
void sio_put_hex(uint32_t x, int n);

//  write a word-size decimal number
void sio_put_dec(uint32_t x);

#endif  //  _SIO_GENERIC_H_
