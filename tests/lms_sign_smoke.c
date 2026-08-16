#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lms-utils.h"

static void cleanup(void) {
    remove("lms-smoke.prv");
    remove("lms-smoke.pub");
    remove("lms-smoke.aux");
    remove("lms-smoke.prv.aux");
}

int main(void) {
    const char *key_base = "lms-smoke";
    const char *private_key = "lms-smoke.prv";
    const char *public_key = "lms-smoke.pub";
    const unsigned char message[] = "Bootgen LMS signing smoke test";
    int hash[] = { 2 };
    int height[] = { 5 };
    int width[] = { 2 };
    int result = EXIT_FAILURE;
    uint32_t error_code = 0;
    size_t signature_length;
    unsigned char *signature = NULL;

    cleanup();
    if (!LmsKeyGeneration(key_base, hash, height, width, 1)) {
        fprintf(stderr, "LMS key generation failed\n");
        goto done;
    }

    signature_length = GetLmsSignLength(private_key, false);
    if (signature_length == 0) {
        fprintf(stderr, "LMS signature length lookup failed\n");
        goto done;
    }

    signature = calloc(signature_length, 1);
    if (!signature) {
        fprintf(stderr, "LMS signature allocation failed\n");
        goto done;
    }

    if (!LmsSign(private_key, message, sizeof(message) - 1, signature,
                 &signature_length, &error_code)) {
        fprintf(stderr, "LMS signing failed: %u\n", error_code);
        goto done;
    }
    if (!LmsVerify(public_key, message, sizeof(message) - 1, signature,
                   signature_length)) {
        fprintf(stderr, "LMS signature verification failed\n");
        goto done;
    }

    signature[0] ^= 1;
    if (LmsVerify(public_key, message, sizeof(message) - 1, signature,
                  signature_length)) {
        fprintf(stderr, "Modified LMS signature unexpectedly verified\n");
        goto done;
    }

    result = EXIT_SUCCESS;

done:
    free(signature);
    cleanup();
    return result;
}
