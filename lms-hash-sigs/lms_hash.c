#include <string.h>
#include "lms_hash.h"
#include "sha256.h"
#include "fips202.h"
#include "hss_zeroize.h"

#define ALLOW_VERBOSE 0  /* 1 -> we allow the dumping of intermediate */
                         /*      states.  Useful for debugging; horrid */
                         /*      for security */

/*
 * This is the file that implements the hashing APIs we use internally.
 * At the present, our parameter sets support only one hash function
 * (SHA-256, using full 256 bit output), however, that is likely to change
 * in the future
 */

#if ALLOW_VERBOSE
#include <stdio.h>
#include <stdbool.h>
/*
 * Debugging flag; if this is set, we chat about what we're hashing, and what
 * the result is it's useful when debugging; however we probably don't want to
 * do this if we're multithreaded...
 */
bool hss_verbose = false;
#endif

/*
 * This will hash the message, given the hash type. It assumes that the result
 * buffer is large enough for the hash
 */
void hss_hash_ctx(void *result, int hash_type, union hash_context *ctx,
          const void *message, size_t message_len) {
#if ALLOW_VERBOSE
    if (hss_verbose) {
        int i; for (i=0; i< message_len; i++) printf( " %02x%s", ((unsigned char*)message)[i], (i%16 == 15) ? "\n" : "" );
    }
#endif

    int output_len;
    switch (hash_type) {
    case HASH_SHA256: {
        SHA256_Init(&ctx->sha256);
        SHA256_Update(&ctx->sha256, message, message_len);
        SHA256_Final(result, &ctx->sha256);
	output_len = 32;
        break;
    }
    case HASH_SHA256_24: {
        unsigned char temp[SHA256_LEN];
        SHA256_Init(&ctx->sha256);
        SHA256_Update(&ctx->sha256, message, message_len);
        SHA256_Final(temp, &ctx->sha256);
        memcpy(result, temp, 24 );
        hss_zeroize(temp, sizeof temp);
	output_len = 24;
        break;
    }
    case HASH_SHAKE256: case HASH_SHAKE256_24:
        shake256_inc_init(ctx->shake256);
        shake256_inc_absorb(ctx->shake256, message, message_len);
        shake256_inc_finalize(ctx->shake256);
	output_len = hss_hash_length(hash_type);
        shake256_inc_squeeze(result, output_len, ctx->shake256);
	break;
    }
#if ALLOW_VERBOSE
    if (hss_verbose) {
        printf( " ->" );
        int i; for (i=0; i<output_len; i++) printf( " %02x", ((unsigned char *)result)[i] ); printf( "\n" );
    }
#endif
}

void hss_hash(void *result, int hash_type,
          const void *message, size_t message_len) {
    union hash_context ctx;
    hss_hash_ctx(result, hash_type, &ctx, message, message_len);
    hss_zeroize(&ctx, sizeof ctx);
}


/*
 * This provides an API to do incremental hashing.  We use it when hashing the
 * message; since we don't know how long it could be, we don't want to
 * allocate a buffer that's long enough for that, plus the decoration we add
 */
void hss_init_hash_context(int h, union hash_context *ctx) {
    switch (h) {
    case HASH_SHA256: case HASH_SHA256_24:
        SHA256_Init( &ctx->sha256 );
        break;
    case HASH_SHAKE256: case HASH_SHAKE256_24:
        shake256_inc_init(ctx->shake256);
        break;
    }
}

void hss_update_hash_context(int h, union hash_context *ctx,
                         const void *msg, size_t len_msg) {
#if ALLOW_VERBOSE
    if (hss_verbose) {
        int i; for (i=0; i<len_msg; i++) printf( " %02x", ((unsigned char*)msg)[i] );
    }
#endif
    switch (h) {
    case HASH_SHA256: case HASH_SHA256_24:
        SHA256_Update(&ctx->sha256, msg, len_msg);
        break;
    case HASH_SHAKE256: case HASH_SHAKE256_24:
        shake256_inc_absorb(ctx->shake256, msg, len_msg);
	break;
    }
}

void hss_finalize_hash_context(int h, union hash_context *ctx, void *buffer) {
    int output_len;
    switch (h) {
    case HASH_SHA256:
        SHA256_Final(buffer, &ctx->sha256);
	output_len = 32;
        break;
    case HASH_SHA256_24: {
        unsigned char temp[SHA256_LEN];
        SHA256_Final(temp, &ctx->sha256);
        memcpy(buffer, temp, 24);
        hss_zeroize(temp, sizeof temp);
	output_len = 24;
        break;
    }
    case HASH_SHAKE256:
	output_len = 32;
        shake256_inc_finalize(ctx->shake256);
        shake256_inc_squeeze(buffer, output_len, ctx->shake256);
        break;
    case HASH_SHAKE256_24:
	output_len = 24;
        shake256_inc_finalize(ctx->shake256);
        shake256_inc_squeeze(buffer, output_len, ctx->shake256);
        break;
    }
#if ALLOW_VERBOSE
    if (hss_verbose) {
        printf( " -->" );
        int i; for (i=0; i<output_len; i++) printf( " %02x", ((unsigned char*)buffer)[i] );
        printf( "\n" );
    }
#endif
}


unsigned hss_hash_length(int hash_type) {
    switch (hash_type) {
    case HASH_SHA256: case HASH_SHAKE256: return 32;
    case HASH_SHA256_24: case HASH_SHAKE256_24: return 24;
    }
    return 0;
}

unsigned hss_hash_blocksize(int hash_type) {
    switch (hash_type) {
    case HASH_SHA256: case HASH_SHA256_24:return 64;

        /* We call this function in order to do HMAC; doing HMAC on SHAKE */
        /* is not usual (KMAC is preferred), however we fill in a value */
        /* anyways */
    case HASH_SHAKE256: case HASH_SHAKE256_24: return 136;
    }
    return 0;
}

