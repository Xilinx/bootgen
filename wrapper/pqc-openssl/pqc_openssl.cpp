#include "pqc_openssl.h"
#include "ml_dsa_api.h"

#include <openssl/opensslv.h>
#include <openssl/rand.h>

#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdint>

#if OPENSSL_VERSION_NUMBER >= 0x30500000L
#include <openssl/core_names.h>
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/params.h>
#include <memory>
#endif

#if OPENSSL_VERSION_NUMBER >= 0x30500000L
namespace {

using PkeyPtr = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
using PkeyCtxPtr = std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
using SignaturePtr = std::unique_ptr<EVP_SIGNATURE, decltype(&EVP_SIGNATURE_free)>;

PkeyPtr ImportKey(const char* algorithm,
                  const uint8_t* key_data,
                  size_t key_len,
                  bool private_key)
{
    PkeyPtr key(nullptr, EVP_PKEY_free);
    PkeyCtxPtr context(EVP_PKEY_CTX_new_from_name(nullptr, algorithm, nullptr),
                       EVP_PKEY_CTX_free);
    if (!context || EVP_PKEY_fromdata_init(context.get()) <= 0) {
        return key;
    }

    OSSL_PARAM parameters[] = {
        OSSL_PARAM_construct_octet_string(
            private_key ? OSSL_PKEY_PARAM_PRIV_KEY : OSSL_PKEY_PARAM_PUB_KEY,
            const_cast<uint8_t*>(key_data), key_len),
        OSSL_PARAM_construct_end()
    };
    EVP_PKEY* imported = nullptr;
    const int selection = private_key ? EVP_PKEY_KEYPAIR : EVP_PKEY_PUBLIC_KEY;
    if (EVP_PKEY_fromdata(context.get(), &imported, selection, parameters) <= 0) {
        return key;
    }
    return PkeyPtr(imported, EVP_PKEY_free);
}

bool ExportKey(EVP_PKEY* key,
               const char* parameter,
               uint8_t* output,
               size_t expected_len)
{
    size_t actual_len = 0;
    return EVP_PKEY_get_octet_string_param(
               key, parameter, output, expected_len, &actual_len) > 0 &&
           actual_len == expected_len;
}

bool GenerateKey(const char* algorithm,
                 const char* seed_parameter,
                 const uint8_t* seed,
                 size_t seed_len,
                 uint8_t* public_key,
                 size_t public_key_len,
                 uint8_t* private_key,
                 size_t private_key_len)
{
    PkeyCtxPtr context(EVP_PKEY_CTX_new_from_name(nullptr, algorithm, nullptr),
                       EVP_PKEY_CTX_free);
    if (!context || EVP_PKEY_keygen_init(context.get()) <= 0) {
        return false;
    }

    OSSL_PARAM parameters[] = {
        OSSL_PARAM_construct_octet_string(
            seed_parameter, const_cast<uint8_t*>(seed), seed_len),
        OSSL_PARAM_construct_end()
    };
    if (EVP_PKEY_CTX_set_params(context.get(), parameters) <= 0) {
        return false;
    }

    EVP_PKEY* generated = nullptr;
    if (EVP_PKEY_generate(context.get(), &generated) <= 0) {
        return false;
    }
    PkeyPtr key(generated, EVP_PKEY_free);
    return ExportKey(key.get(), OSSL_PKEY_PARAM_PUB_KEY,
                     public_key, public_key_len) &&
           ExportKey(key.get(), OSSL_PKEY_PARAM_PRIV_KEY,
                     private_key, private_key_len);
}

size_t Sign(EVP_PKEY* key,
            const char* algorithm,
            const OSSL_PARAM* parameters,
            const uint8_t* message,
            size_t message_len,
            uint8_t* signature,
            size_t signature_capacity)
{
    SignaturePtr implementation(
        EVP_SIGNATURE_fetch(nullptr, algorithm, nullptr), EVP_SIGNATURE_free);
    PkeyCtxPtr context(EVP_PKEY_CTX_new_from_pkey(nullptr, key, nullptr),
                       EVP_PKEY_CTX_free);
    if (!implementation || !context ||
        EVP_PKEY_sign_message_init(
            context.get(), implementation.get(), parameters) <= 0) {
        return 0;
    }

    size_t signature_len = signature_capacity;
    if (EVP_PKEY_sign(context.get(), signature, &signature_len,
                      message, message_len) <= 0 ||
        signature_len > signature_capacity) {
        return 0;
    }
    return signature_len;
}

bool Verify(EVP_PKEY* key,
            const char* algorithm,
            const OSSL_PARAM* parameters,
            const uint8_t* message,
            size_t message_len,
            const uint8_t* signature,
            size_t signature_len)
{
    SignaturePtr implementation(
        EVP_SIGNATURE_fetch(nullptr, algorithm, nullptr), EVP_SIGNATURE_free);
    PkeyCtxPtr context(EVP_PKEY_CTX_new_from_pkey(nullptr, key, nullptr),
                       EVP_PKEY_CTX_free);
    return implementation && context &&
           EVP_PKEY_verify_message_init(
               context.get(), implementation.get(), parameters) > 0 &&
           EVP_PKEY_verify(context.get(), signature, signature_len,
                           message, message_len) == 1;
}

} // namespace
#endif /* OPENSSL_VERSION_NUMBER >= 0x30500000L */

extern "C" {

int bootgen_pqc_openssl_available(void)
{
#if OPENSSL_VERSION_NUMBER < 0x30500000L
    return 0;
#else
    return OpenSSL_version_num() >= 0x30500000L ? 1 : 0;
#endif
}

void mldsa_random_bytes(uint8_t* out, size_t len)
{
    while (out && len != 0) {
        const size_t chunk = std::min(len, static_cast<size_t>(INT_MAX));
        if (RAND_bytes(out, static_cast<int>(chunk)) != 1) {
            return;
        }
        out += chunk;
        len -= chunk;
    }
}

#if OPENSSL_VERSION_NUMBER >= 0x30500000L

void mldsa87_keygen(const uint8_t* seed, uint8_t* pubkey, uint8_t* seckey)
{
    if (!seed || !pubkey || !seckey) {
        return;
    }
    GenerateKey("ML-DSA-87", OSSL_PKEY_PARAM_ML_DSA_SEED,
                seed, MLDSA87_SEED_LEN,
                pubkey, MLDSA87_PUBKEY_LEN,
                seckey, MLDSA87_SECKEY_LEN);
}

int mldsa87_sign(const uint8_t* rnd,
                 const uint8_t* seckey,
                 const uint8_t* msg,
                 size_t msg_len,
                 uint8_t* sig)
{
    if (!rnd || !seckey || (!msg && msg_len != 0) || !sig) {
        return 0;
    }
    PkeyPtr key = ImportKey(
        "ML-DSA-87", seckey, MLDSA87_SECKEY_LEN, true);
    if (!key) {
        return 0;
    }

    OSSL_PARAM parameters[] = {
        OSSL_PARAM_construct_octet_string(
            OSSL_SIGNATURE_PARAM_TEST_ENTROPY,
            const_cast<uint8_t*>(rnd), MLDSA87_SEED_LEN),
        OSSL_PARAM_construct_end()
    };
    return Sign(key.get(), "ML-DSA-87", parameters, msg, msg_len,
                sig, MLDSA87_SIG_LEN) == MLDSA87_SIG_LEN;
}

int mldsa87_verify(const uint8_t* pubkey,
                   const uint8_t* msg,
                   size_t msg_len,
                   const uint8_t* sig)
{
    if (!pubkey || (!msg && msg_len != 0) || !sig) {
        return 0;
    }
    PkeyPtr key = ImportKey(
        "ML-DSA-87", pubkey, MLDSA87_PUBKEY_LEN, false);
    if (!key) {
        return 0;
    }
    const OSSL_PARAM parameters[] = { OSSL_PARAM_construct_end() };
    return Verify(key.get(), "ML-DSA-87", parameters, msg, msg_len,
                  sig, MLDSA87_SIG_LEN) ? 1 : 0;
}

int openssl_slhdsa_keygen(const char* algorithm,
                          const uint8_t* seed,
                          size_t seed_len,
                          uint8_t* public_key,
                          size_t public_key_len,
                          uint8_t* private_key,
                          size_t private_key_len)
{
    if (!algorithm || !seed || !public_key || !private_key) {
        return 0;
    }
    return GenerateKey(algorithm, OSSL_PKEY_PARAM_SLH_DSA_SEED,
                       seed, seed_len,
                       public_key, public_key_len,
                       private_key, private_key_len) ? 1 : 0;
}

size_t openssl_slhdsa_sign(const char* algorithm,
                           const uint8_t* private_key,
                           size_t private_key_len,
                           const uint8_t* message,
                           size_t message_len,
                           uint8_t* signature,
                           size_t signature_capacity)
{
    if (!algorithm || !private_key || (!message && message_len != 0) ||
        !signature) {
        return 0;
    }
    PkeyPtr key = ImportKey(
        algorithm, private_key, private_key_len, true);
    if (!key) {
        return 0;
    }

    if (private_key_len == 0 || private_key_len % 4 != 0) {
        return 0;
    }
    const size_t security_parameter = private_key_len / 4;
    uint8_t* public_seed =
        const_cast<uint8_t*>(private_key + (2 * security_parameter));
    int deterministic = 1;
    int message_encoding = 0;
    OSSL_PARAM parameters[] = {
        OSSL_PARAM_construct_int(
            OSSL_SIGNATURE_PARAM_DETERMINISTIC, &deterministic),
        OSSL_PARAM_construct_octet_string(
            OSSL_SIGNATURE_PARAM_TEST_ENTROPY,
            public_seed, security_parameter),
        OSSL_PARAM_construct_int(
            OSSL_SIGNATURE_PARAM_MESSAGE_ENCODING, &message_encoding),
        OSSL_PARAM_construct_end()
    };
    return Sign(key.get(), algorithm, parameters, message, message_len,
                signature, signature_capacity);
}

int openssl_slhdsa_verify(const char* algorithm,
                          const uint8_t* public_key,
                          size_t public_key_len,
                          const uint8_t* message,
                          size_t message_len,
                          const uint8_t* signature,
                          size_t signature_len)
{
    if (!algorithm || !public_key || (!message && message_len != 0) ||
        !signature) {
        return 0;
    }
    PkeyPtr key = ImportKey(
        algorithm, public_key, public_key_len, false);
    if (!key) {
        return 0;
    }

    int message_encoding = 0;
    OSSL_PARAM parameters[] = {
        OSSL_PARAM_construct_int(
            OSSL_SIGNATURE_PARAM_MESSAGE_ENCODING, &message_encoding),
        OSSL_PARAM_construct_end()
    };
    return Verify(key.get(), algorithm, parameters, message, message_len,
                  signature, signature_len) ? 1 : 0;
}

#else /* OpenSSL < 3.5: compile, but PQC crypto is unavailable */

void mldsa87_keygen(const uint8_t* seed, uint8_t* pubkey, uint8_t* seckey)
{
    (void)seed;
    (void)pubkey;
    (void)seckey;
}

int mldsa87_sign(const uint8_t* rnd,
                 const uint8_t* seckey,
                 const uint8_t* msg,
                 size_t msg_len,
                 uint8_t* sig)
{
    (void)rnd;
    (void)seckey;
    (void)msg;
    (void)msg_len;
    (void)sig;
    return 0;
}

int mldsa87_verify(const uint8_t* pubkey,
                   const uint8_t* msg,
                   size_t msg_len,
                   const uint8_t* sig)
{
    (void)pubkey;
    (void)msg;
    (void)msg_len;
    (void)sig;
    return 0;
}

int openssl_slhdsa_keygen(const char* algorithm,
                          const uint8_t* seed,
                          size_t seed_len,
                          uint8_t* public_key,
                          size_t public_key_len,
                          uint8_t* private_key,
                          size_t private_key_len)
{
    (void)algorithm;
    (void)seed;
    (void)seed_len;
    (void)public_key;
    (void)public_key_len;
    (void)private_key;
    (void)private_key_len;
    return 0;
}

size_t openssl_slhdsa_sign(const char* algorithm,
                           const uint8_t* private_key,
                           size_t private_key_len,
                           const uint8_t* message,
                           size_t message_len,
                           uint8_t* signature,
                           size_t signature_capacity)
{
    (void)algorithm;
    (void)private_key;
    (void)private_key_len;
    (void)message;
    (void)message_len;
    (void)signature;
    (void)signature_capacity;
    return 0;
}

int openssl_slhdsa_verify(const char* algorithm,
                          const uint8_t* public_key,
                          size_t public_key_len,
                          const uint8_t* message,
                          size_t message_len,
                          const uint8_t* signature,
                          size_t signature_len)
{
    (void)algorithm;
    (void)public_key;
    (void)public_key_len;
    (void)message;
    (void)message_len;
    (void)signature;
    (void)signature_len;
    return 0;
}

#endif /* OPENSSL_VERSION_NUMBER >= 0x30500000L */

} // extern "C"
