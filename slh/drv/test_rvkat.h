//  test_rvkat.h
//  Markku-Juhani O. Saarinen <mjos@iki.fi> See LICENSE

//  === functions to facilitate simple algorithm self tests

#ifndef _TEST_RVKAT_H_
#define _TEST_RVKAT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "sio_generic.h"

//  === TESTING === used in tests

//  read a hex string of "maxbytes", return byte length
size_t rvkat_gethex(uint8_t *buf, size_t maxbytes, const char *str);

//  check "data" of "len" bytes against a hexadecimal test vector "ref"
int rvkat_chkhex(const char *lab, const void *data, size_t len,
                 const char *ref);

//  boolean return value check (integer -- print decimal)
int rvkat_chkret(const char *lab, int want, int have);

//  32-bit return value check (print hex)
int rvkat_chku32(const char *lab, uint32_t want, uint32_t have);

//  64-bit return value check (print hex)
int rvkat_chku64(const char *lab, uint64_t want, uint64_t have);

//  === DEBUG ==  informational (does not return ok=0/fail)

//  32-bit checksum: compatible with cksum(1)
uint32_t rvkat_cksum(const uint8_t *data, size_t data_sz);

//  print label, test id, number, len, and checksum
uint32_t rvkat_chk( const char *lab, const char *id, uint32_t num,
                    const uint8_t *data, size_t data_sz);

//  print information
void rvkat_info(const char *info);

//  print hexadecimal "data", length "len", with label "lab"
void rvkat_hex(const char *lab, const void *data, size_t len);

//  print a label and a single decimal number
void rvkat_dec(const char *lab, uint32_t x);

//  print a space ' ' and hexademical unsigned without a label
void rvkat_hexu32(uint32_t x);
void rvkat_hexu64(uint64_t x);

#ifdef __cplusplus
}
#endif

#endif  //   _TEST_RVKAT_H_
