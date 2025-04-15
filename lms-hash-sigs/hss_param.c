#include <string.h>
#include "hss.h"
#include "hss_internal.h"
#include "endian.h"
#include "hss_zeroize.h"
#include "lm_common.h"

static struct map_structure {
    param_set_t public;
    unsigned char compressed;
} lm_map[] = {
    { LMS_SHA256_N32_H5, 0x01 },
    { LMS_SHA256_N32_H10,0x02 },
    { LMS_SHA256_N32_H15,0x03 },
    { LMS_SHA256_N32_H20,0x04 },
    { LMS_SHA256_N32_H25,0x05 },
    { LMS_SHA256_N24_H5, 0x06 },
    { LMS_SHA256_N24_H10,0x07 },
    { LMS_SHA256_N24_H15,0x08 },
    { LMS_SHA256_N24_H20,0x09 },
    { LMS_SHA256_N24_H25,0x0a },
    { LMS_SHAKE256_N32_H5, 0x0b },
    { LMS_SHAKE256_N32_H10,0x0c },
    { LMS_SHAKE256_N32_H15,0x0d },
    { LMS_SHAKE256_N32_H20,0x0e },
    { LMS_SHAKE256_N32_H25,0x0f },
    { LMS_SHAKE256_N24_H5, 0x10 },
    { LMS_SHAKE256_N24_H10,0x11 },
    { LMS_SHAKE256_N24_H15,0x12 },
    { LMS_SHAKE256_N24_H20,0x13 },
    { LMS_SHAKE256_N24_H25,0x14 },
    { 0, 0 }},
   ots_map[] = {
    { LMOTS_SHA256_N32_W1, 0x01 },
    { LMOTS_SHA256_N32_W2, 0x02 },
    { LMOTS_SHA256_N32_W4, 0x03 },
    { LMOTS_SHA256_N32_W8, 0x04 },
    { LMOTS_SHA256_N24_W1, 0x05 },
    { LMOTS_SHA256_N24_W2, 0x06 },
    { LMOTS_SHA256_N24_W4, 0x07 },
    { LMOTS_SHA256_N24_W8, 0x08 },
    { LMOTS_SHAKE256_N32_W1, 0x09 },
    { LMOTS_SHAKE256_N32_W2, 0x0a },
    { LMOTS_SHAKE256_N32_W4, 0x0b },
    { LMOTS_SHAKE256_N32_W8, 0x0c },
    { LMOTS_SHAKE256_N24_W1, 0x0d },
    { LMOTS_SHAKE256_N24_W2, 0x0e },
    { LMOTS_SHAKE256_N24_W4, 0x0f },
    { LMOTS_SHAKE256_N24_W8, 0x10 },
    { 0, 0 }};
static bool map(param_set_t *a,
                struct map_structure *map) {
    int i;
    for (i=0; map[i].public != 0; i++) {
        if (map[i].public == *a) {
            *a = map[i].compressed;
            return true;
        }
    }
    return false;
}
static bool unmap(param_set_t *a,
                struct map_structure *map) {
    int i;
    for (i=0; map[i].public != 0; i++) {
        if (map[i].compressed == *a) {
            *a = map[i].public;
            return true;
        }
    }
    return false;
}

/*
 * Convert a parameter set into the compressed version we use within a private
 * key.  This is the private key that'll end up being updated constantly, and
 * so we try to make it as small as possible
 */
bool hss_compress_param_set( unsigned char *compressed,
                   int levels, 
                   const param_set_t *lm_type,
                   const param_set_t *lm_ots_type,
                   size_t len_compressed ) {
    int i;

    for (i=0; i<levels; i++) {
        if (len_compressed == 0) return false;
        param_set_t a = *lm_type++;
        param_set_t b = *lm_ots_type++;

        /* Convert the official mappings to the compressed versions */
        if (!map( &a, lm_map ) ||
            !map( &b, ots_map )) return false;

        *compressed++ = a;
        *compressed++ = b;
        len_compressed -= 2;
    }

    while (len_compressed) {
        *compressed++ = PARM_SET_END;
        len_compressed--;
    }

    return true;
}

/*
 * This returns the parameter set for a given private key.
 * This is here to solve a chicken-and-egg problem: the hss_working_key
 * must be initialized to the same parameter set as the private key,
 * but (other than this function, or somehow remembering it) there's
 * no way to retreive the parameter set.
 *
 * read_private_key/context will read the private key (if read_private_key is
 * NULL, context is assumed to point to the private key)
 *
 * On success, *levels will be set to the number of levels, and lm_type[]
 * and lm_ots_type[] will be set to the lm/ots parameter sets
 *
 * On success, this returns true; on failure (can't read the private key, or
 * the private key is invalid), returns false 
 */
bool hss_get_parameter_set( unsigned *levels,
                           param_set_t lm_type[ MAX_HSS_LEVELS ],
                           param_set_t lm_ots_type[ MAX_HSS_LEVELS ],
                           bool (*read_private_key)(unsigned char *private_key,
                                       size_t len_private_key, void *context),
                           void *context,
                           struct hss_extra_info *info) {
    unsigned char private_key[ PRIVATE_KEY_LEN(MAX_SEED_LEN) ];
    bool success = false;
    enum hss_error_code error_code = hss_error_bad_private_key; /* Most of */
        /* the detected errors are 'what's in the private key does not */
        /* correspond to a supported parameter set' */

    if (read_private_key) {
        if (!read_private_key( private_key, PRIVATE_KEY_SEED, context )) {
            error_code = hss_error_private_key_read_failed;
            goto failed;
        }
    } else {
        if (!context) {
            error_code = hss_error_no_private_buffer;
            goto failed;
        }
        memcpy( private_key, context, PRIVATE_KEY_SEED );
    }

    /* Scan through the private key to recover the parameter sets */
    unsigned total_height = 0;
    unsigned level;
    for (level=0; level < MAX_HSS_LEVELS; level++) {
        unsigned char c = private_key[PRIVATE_KEY_PARAM_SET + 2*level];
        unsigned char d = private_key[PRIVATE_KEY_PARAM_SET + 2*level + 1];
        if (c == PARM_SET_END && d == PARM_SET_END) break;
            /* Decode this level's parameter set */
        param_set_t lm = c;
        param_set_t ots = d;
            /* Make sure both are supported */
            /* While we're here, add up the total Merkle height */

        /* How we unpack the parameter sets */
        if (!unmap( &lm, lm_map ) ||
            !unmap( &ots, ots_map )) goto failed;
            /* While we're here, add up the total Merkle height */
        switch (lm) {
        case LMS_SHA256_N32_H5:
        case LMS_SHA256_N24_H5:
        case LMS_SHAKE256_N32_H5:
        case LMS_SHAKE256_N24_H5: total_height += 5; break;
        case LMS_SHA256_N32_H10:
        case LMS_SHA256_N24_H10:
        case LMS_SHAKE256_N32_H10:
        case LMS_SHAKE256_N24_H10: total_height += 10; break;
        case LMS_SHA256_N32_H15:
        case LMS_SHA256_N24_H15:
        case LMS_SHAKE256_N32_H15:
        case LMS_SHAKE256_N24_H15: total_height += 15; break;
        case LMS_SHA256_N32_H20:
        case LMS_SHA256_N24_H20:
        case LMS_SHAKE256_N32_H20:
        case LMS_SHAKE256_N24_H20: total_height += 20; break;
        case LMS_SHA256_N32_H25:
        case LMS_SHA256_N24_H25:
        case LMS_SHAKE256_N32_H25:
        case LMS_SHAKE256_N24_H25: total_height += 25; break;
        default: goto failed;
        }
        lm_type[level] = lm;
        lm_ots_type[level] = ots;
    }

    if (level < MIN_HSS_LEVELS || level > MAX_HSS_LEVELS) goto failed;

    *levels = level;

    /* Make sure that the rest of the private key has PARM_SET_END */
    unsigned i;
    for (i = 2*(level+1); i<2*MAX_HSS_LEVELS; i++) {
        unsigned char c = private_key[PRIVATE_KEY_PARAM_SET + i];
        if (c != PARM_SET_END) goto failed;
    }

    /* One final check; make sure that the sequence number listed in the */
    /* private key is in range */

    if (total_height > 64) total_height = 64; /* (bounded by 2**64) */
    sequence_t max_count = ((sequence_t)2 << (total_height-1)) - 1;
        /* height-1 so we don't try to shift by 64, and hit U.B. */

        /* We use the count 0xffff..ffff to signify 'we've used up all our */
        /* signatures'.  Make sure that is above max_count, even for */
        /* parameter sets that can literally generate 2**64 signatures (by */
        /* letting them generate only 2**64-1) */
    if (total_height == 64) max_count--;
    sequence_t current_count = get_bigendian(
                 private_key + PRIVATE_KEY_INDEX, PRIVATE_KEY_INDEX_LEN );

    if (current_count > max_count) {
      	/* Private key expired */
	error_code = hss_error_private_key_expired;
        goto failed;
    }

    success = true;   /* It worked! */
    error_code = hss_error_none;
failed:
    if (info) info->error_code = error_code;
        /* There might be private keying material here */
    hss_zeroize( private_key, sizeof private_key );
    return success;
}

int get_level0_lm_hash_len( const unsigned char *private_key ) {
    /* Look up the compressed parameter set format */
    unsigned char c = private_key[PRIVATE_KEY_PARAM_SET];
    param_set_t lm = c;
    if (!unmap( &lm, lm_map )) return 0; 
    unsigned n;
    if (!lm_look_up_parameter_set(lm, 0, &n, 0)) return 0;
    return n;
}
