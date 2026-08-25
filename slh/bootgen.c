#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Required for rand()

#include "slh_dsa.h"

// A simple pseudo-random byte generator for demonstration purposes.
// For production, use a cryptographically secure random number generator.
int demo_randombytes(uint8_t *x, size_t xlen) {
    for (size_t i = 0; i < xlen; i++) {
        x[i] = (uint8_t)rand();
    }
    return 0;
}

static void print_hex(const char *label, const uint8_t *data, size_t len) {
    printf("%s (%zu bytes): ", label, len);
    for (size_t i = 0; i < len; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}

int dup_main() {
    // Select the SLH-DSA parameter set
    const slh_param_t *params = &slh_dsa_shake_256s; // Example parameter set

    // Define a sample message
    const char *message_str = "This is a test message for SLH-DSA signature generation.";
    const uint8_t *message = (const uint8_t *)message_str;
    size_t message_len = strlen(message_str);

    // Determine key and signature sizes
    size_t pk_sz = slh_pk_sz(params);
    size_t sk_sz = slh_sk_sz(params);
    size_t sig_sz = slh_sig_sz(params);

    // Allocate memory for keys and signature
    uint8_t pk[pk_sz];
    uint8_t sk[sk_sz];
    uint8_t signature[sig_sz];

    printf("Using SLH-DSA parameter set: %s\n", slh_alg_id(params));
    print_hex("Original Message", message, message_len);

    // 1. Generate SLH-DSA key pair
    if (slh_keygen(pk, sk, &demo_randombytes, params) != 0) {
        fprintf(stderr, "Error: SLH-DSA key generation failed.\n");
        return 1;
    }
    print_hex("Public Key", pk, pk_sz);
    print_hex("Secret Key", sk, sk_sz);

    // 2. Generate SLH-DSA signature
    size_t actual_sig_sz = slh_sign(signature, message, message_len, sk, &demo_randombytes, params);
    if (actual_sig_sz == 0) { // slh_sign returns 0 on error, actual size on success
        fprintf(stderr, "Error: SLH-DSA signature generation failed.\n");
        return 1;
    }
     // The actual_sig_sz should match sig_sz for fixed-size SLH-DSA signatures.
    if (actual_sig_sz != sig_sz) {
        fprintf(stderr, "Warning: Signature size mismatch. Expected %zu, Got %zu\n", sig_sz, actual_sig_sz);
        // For SLH-DSA, the signature size is fixed by the parameters.
        // If they don't match, it might indicate an issue or a misunderstanding of the API.
        // However, we'll proceed with the actual_sig_sz for printing.
    }
    print_hex("Signature", signature, actual_sig_sz);


    // 3. Verify the SLH-DSA signature (optional, but good for testing)
    bool is_valid = slh_verify(message, message_len, signature, pk, params);
    if (is_valid) {
        printf("Signature verification successful.\n");
    } else {
        printf("Signature verification failed.\n");
    }

    return 0;
}