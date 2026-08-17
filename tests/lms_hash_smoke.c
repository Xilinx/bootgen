#include <stdio.h>
#include <string.h>

#include "lms_hash.h"

static int check_sha256(void) {
    static const unsigned char expected[SHA256_LEN] = {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
        0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
        0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
        0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
    };
    unsigned char digest[SHA256_LEN];
    union hash_context context;

    hss_hash(digest, HASH_SHA256, "abc", 3);
    if (memcmp(digest, expected, sizeof digest) != 0) return 0;

    hss_init_hash_context(HASH_SHA256, &context);
    hss_update_hash_context(HASH_SHA256, &context, "a", 1);
    hss_update_hash_context(HASH_SHA256, &context, "bc", 2);
    hss_finalize_hash_context(HASH_SHA256, &context, digest);
    return memcmp(digest, expected, sizeof digest) == 0;
}

static int check_shake256(void) {
    unsigned char one_shot[SHA256_LEN];
    unsigned char incremental[SHA256_LEN];
    union hash_context context;

    hss_hash(one_shot, HASH_SHAKE256, "abc", 3);
    hss_init_hash_context(HASH_SHAKE256, &context);
    hss_update_hash_context(HASH_SHAKE256, &context, "a", 1);
    hss_update_hash_context(HASH_SHAKE256, &context, "bc", 2);
    hss_finalize_hash_context(HASH_SHAKE256, &context, incremental);
    return memcmp(one_shot, incremental, sizeof one_shot) == 0;
}

int main(void) {
    if (!check_sha256()) {
        fprintf(stderr, "LMS SHA-256 hash result did not match the expected digest\n");
        return 1;
    }
    if (!check_shake256()) {
        fprintf(stderr, "LMS SHAKE-256 one-shot and incremental hashes differ\n");
        return 1;
    }
    return 0;
}
