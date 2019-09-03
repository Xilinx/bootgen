/******************************************************************************
* Copyright 2015-2019 Xilinx, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/

#ifndef _SYSTEMUTILS_H_
#define _SYSTEMUTILS_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
-------------------------------------------------------------------------------
*********************************************   P R E P R O C E S S O R S   ***
-------------------------------------------------------------------------------
*/
#define BITS_IN_BYTE        8
#define BITS_IN_A_WORD     32


/*
-------------------------------------------------------------------------------
***************************************************   S T R U C T U R E S   ***
-------------------------------------------------------------------------------
*/
struct UInt128
{
    uint64_t    upper;
    uint64_t    lower;
};
typedef struct UInt128 UInt128;

/* Read a big endian 16 bit word from a byte stream. */
#define ReadBigEndian16( addr ) \
    ( ( (UInt16)( *( (uint8_t *)(addr) + 0 ) ) << 8 ) | \
        (UInt16)( *( (uint8_t *)(addr) + 1 ) ) )

/* Write a big endian 16 bit word to a byte stream. */
#define WriteBigEndian16( addr, value ) \
    *( (uint8_t *)(addr) + 0 ) = ( ( (value) >> 8 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 1 ) =   ( (value) & 0xFF );

/* Read a big endian 32 bit word from a byte stream. */
#define ReadBigEndian32( addr ) \
    ( ( (uint32_t)( *( (uint8_t *)(addr) + 0 ) ) << 24 ) | \
      ( (uint32_t)( *( (uint8_t *)(addr) + 1 ) ) << 16 ) | \
      ( (uint32_t)( *( (uint8_t *)(addr) + 2 ) ) << 8 ) |  \
        (uint32_t)( *( (uint8_t *)(addr) + 3 ) ) )

/* Write a big endian 32 bit word to a byte stream. */
#define WriteBigEndian32( addr, value ) \
    *( (uint8_t *)(addr) + 0 ) = ( ( (value) >> 24 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 1 ) = ( ( (value) >> 16 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 2 ) = ( ( (value) >> 8 ) & 0xFF );  \
    *( (uint8_t *)(addr) + 3 ) =   ( (value) & 0xFF );

/* Read a big endian 64 bit word from a byte stream. */
#define ReadBigEndian64( addr ) \
    ( ( (uint64_t)( *( (uint8_t *)(addr) + 0 ) ) << 56 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 1 ) ) << 48 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 2 ) ) << 40 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 3 ) ) << 32 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 4 ) ) << 24 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 5 ) ) << 16 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 6 ) ) << 8 ) |  \
        (uint64_t)( *( (uint8_t *)(addr) + 7 ) ) )

/* Write a big endian 64 bit word to a byte stream. */
#define WriteBigEndian64( addr, value ) \
    *( (uint8_t *)(addr) + 0 ) = ( ( (uint64_t)(value) >> 56 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 1 ) = ( ( (uint64_t)(value) >> 48 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 2 ) = ( ( (uint64_t)(value) >> 40 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 3 ) = ( ( (uint64_t)(value) >> 32 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 4 ) = ( ( (uint64_t)(value) >> 24 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 5 ) = ( ( (uint64_t)(value) >> 16 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 6 ) = ( ( (uint64_t)(value) >> 8 ) & 0xFF );  \
    *( (uint8_t *)(addr) + 7 ) =   ( (uint64_t)(value) & 0xFF );

/* Read a little endian 16 bit word from a byte stream. */
#define ReadLittleEndian16( addr ) \
    ( ( (uint16_t)( *( (uint8_t *)(addr) + 1 ) ) << 8 ) | \
        (uint16_t)( *( (uint8_t *)(addr) + 0 ) ) )

/* Write a little endian 16 bit word to a byte stream. */
#define WriteLittleEndian16( addr, value ) \
    *( (uint8_t *)(addr) + 1 ) = ( ( (value) >> 8 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 0 ) =   ( (value) & 0xFF );

/* Read a little endian 32 bit word from a byte stream. */
#define ReadLittleEndian32( addr ) \
    ( ( (uint32_t)( *( (uint8_t *)(addr) + 3 ) ) << 24 ) | \
      ( (uint32_t)( *( (uint8_t *)(addr) + 2 ) ) << 16 ) | \
      ( (uint32_t)( *( (uint8_t *)(addr) + 1 ) ) << 8 ) |  \
        (uint32_t)( *( (uint8_t *)(addr) + 0 ) ) )

/* Write a little endian 32 bit word to a byte stream. */
#define WriteLittleEndian32( addr, value ) \
    *( (uint8_t *)(addr) + 3 ) = ( ( (value) >> 24 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 2 ) = ( ( (value) >> 16 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 1 ) = ( ( (value) >> 8 ) & 0xFF );  \
    *( (uint8_t *)(addr) + 0 ) =   ( (value) & 0xFF );

/* Read a little endian 64 bit word from a byte stream. */
#define ReadLittleEndian64( addr ) \
    ( ( (uint64_t)( *( (uint8_t *)(addr) + 7 ) ) << 56 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 6 ) ) << 48 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 5 ) ) << 40 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 4 ) ) << 32 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 3 ) ) << 24 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 2 ) ) << 16 ) | \
      ( (uint64_t)( *( (uint8_t *)(addr) + 1 ) ) << 8 ) |  \
        (uint64_t)( *( (uint8_t *)(addr) + 0 ) ) )

/* Write a little endian 64 bit word to a byte stream. */
#define WriteLittleEndian64( addr, value ) \
    *( (uint8_t *)(addr) + 7 ) = ( ( (uint64_t)(value) >> 56 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 6 ) = ( ( (uint64_t)(value) >> 48 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 5 ) = ( ( (uint64_t)(value) >> 40 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 4 ) = ( ( (uint64_t)(value) >> 32 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 3 ) = ( ( (uint64_t)(value) >> 24 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 2 ) = ( ( (uint64_t)(value) >> 16 ) & 0xFF ); \
    *( (uint8_t *)(addr) + 1 ) = ( ( (uint64_t)(value) >> 8 ) & 0xFF );  \
    *( (uint8_t *)(addr) + 0 ) =   ( (uint64_t)(value) & 0xFF );


/* Swap the bytes in a 16 bit word. */
#define SwapUInt16Bytes( data16 ) ( ( ( (data16) >> BITS_IN_BYTE ) & 0x00FF ) | \
                                    ( ( (data16) << BITS_IN_BYTE ) & 0xFF00 ) )

/* Swap the bytes in a 32 bit word. */
#define SwapUInt32Bytes( data32 ) ( ( ( (data32) >> ( BITS_IN_BYTE * 3 ) ) & 0x000000FF ) | \
                                    ( ( (data32) >> ( BITS_IN_BYTE * 1 ) ) & 0x0000FF00 ) | \
                                    ( ( (data32) << ( BITS_IN_BYTE * 1 ) ) & 0x00FF0000 ) | \
                                    ( ( (data32) << ( BITS_IN_BYTE * 3 ) ) & 0xFF000000 ) )

/* Swap the bytes in a 64 bit word. */
#define SwapUInt64Bytes( data64 ) ( ( ( (data64) >> ( BITS_IN_BYTE * 7 ) ) & (uint64_t)0x00000000000000FFll ) | \
                                    ( ( (data64) >> ( BITS_IN_BYTE * 5 ) ) & (uint64_t)0x000000000000FF00ll ) | \
                                    ( ( (data64) >> ( BITS_IN_BYTE * 3 ) ) & (uint64_t)0x0000000000FF0000ll ) | \
                                    ( ( (data64) >> ( BITS_IN_BYTE * 1 ) ) & (uint64_t)0x00000000FF000000ll ) | \
                                    ( ( (data64) << ( BITS_IN_BYTE * 1 ) ) & (uint64_t)0x000000FF00000000ll ) | \
                                    ( ( (data64) << ( BITS_IN_BYTE * 3 ) ) & (uint64_t)0x0000FF0000000000ll ) | \
                                    ( ( (data64) << ( BITS_IN_BYTE * 5 ) ) & (uint64_t)0x00FF000000000000ll ) | \
                                    ( ( (data64) << ( BITS_IN_BYTE * 7 ) ) & (uint64_t)0xFF00000000000000ll ) )


/* Trim it down to an even address boundary. */
#define RoundAddressDown( addr ) ( (uint64_t)(addr) & (uint64_t)~( sizeof( char * ) - 1 ) )

/* Increment it up to an even address boundary. */
#define RoundAddressUp( addr ) ( ( ( (uint64_t)(addr) ) + ( sizeof( char * ) - 1 ) ) & (uint64_t)~( sizeof( char * ) - 1 ) )

/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
class DataMover
{
public:
    DataMover() {};
    ~DataMover() {};

    virtual uint32_t ReadData32(const uint8_t* addr) = 0;
    virtual void WriteData32(const uint8_t* addr, uint32_t word) = 0;
};

/******************************************************************************/
class DataMoverLE : public DataMover
{
public:
    DataMoverLE() {};
    ~DataMoverLE() {};

    uint32_t ReadData32(const uint8_t* addr)
    {
        return ReadLittleEndian32(addr);
    }
    void WriteData32(const uint8_t* addr, uint32_t word)
    {
        WriteLittleEndian32(addr, word);
    }
};

/******************************************************************************/
class DataMoverBE : public DataMover
{
public:
    DataMoverBE() {};
    ~DataMoverBE() {};

    uint32_t ReadData32(const uint8_t* addr)
    {
        return ReadBigEndian32(addr);
    }

    void WriteData32(const uint8_t* addr, uint32_t word)
    {
        WriteBigEndian32(addr, word);
    }
};


#ifdef __cplusplus
}
#endif

#endif
