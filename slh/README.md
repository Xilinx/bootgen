#   slh

2023-12-21  Markku-Juhani O. Saarinen  mjos@iki.fi

This is my portable C implementation of SLH-DSA ("Stateless Hash-Based Digital Signature Standard") as described in [FIPS 205 Initial Public Draft](https://doi.org/10.6028/NIST.FIPS.205.ipd) from August 2023. SLH-DSA is derived from the SPHINCS+ submission to the NIST PQC Competition, with minor modifications. This is code is functionally equivalent to my [slh-dsa-py](https://github.com/mjosaarinen/slh-dsa-py) Python implementation, but up to 10x faster.


##  Building and Running Known Answer Tests

The implementation in this directory includes the necessary SHA2 and SHAKE256 hash functions and hence has no external library dependencies. On a Linux system, you may usually just use `make` to build the executable `kat_test`.
```
make
```

With the Makefile defaults, only a single test vector is generated for each variant for a quick check:

```
$ ./kat_test
[KAT] (0) SLH-DSA-SHAKE-128s-1.rsp
[KAT] (0) SLH-DSA-SHAKE-128f-1.rsp
[KAT] (0) SLH-DSA-SHAKE-192s-1.rsp
[KAT] (0) SLH-DSA-SHAKE-192f-1.rsp
[KAT] (0) SLH-DSA-SHAKE-256s-1.rsp
[KAT] (0) SLH-DSA-SHAKE-256f-1.rsp
[KAT] (0) SLH-DSA-SHA2-128s-1.rsp
[KAT] (0) SLH-DSA-SHA2-128f-1.rsp
[KAT] (0) SLH-DSA-SHA2-192s-1.rsp
[KAT] (0) SLH-DSA-SHA2-192f-1.rsp
[KAT] (0) SLH-DSA-SHA2-256s-1.rsp
[KAT] (0) SLH-DSA-SHA2-256f-1.rsp
[INFO] test_slh_dsa() fail= 0
```
The [../kat](../kat) subdirectory also contains full text of the first vector of each response file. One way to check that the newly generated test vectors is to compare their hashes with the ones in the `kat` subdirectory (the "2" prefix means a matching hash is found.)

```
slh$ sha256sum *.rsp ../kat/*.rsp.1 | sort | uniq -c -w 64
      2 049ff342c967ee43f586052249ba3313297e6630c1095d2af869853a717f0acd  ../kat/sphincs-shake-256f-simple.rsp.1
      2 1aab628d922a489f6879e29b55e5a474c3b9d4c52b166a52e7504f69bf85057a  ../kat/sphincs-sha2-256f-simple.rsp.1
      2 2b23b59d31969dbb91fb5465d26637448c53658631d9c218664e2ad1a2cb69e0  ../kat/sphincs-sha2-256s-simple.rsp.1
      2 601be3028bf69f6c7aecfd709b95c9c778e2420b976967771203e2e824a86321  ../kat/sphincs-sha2-192s-simple.rsp.1
      2 6452ae18c387c7816d1396b0919c824479d0ab02f91010f59d26cd405fe309b4  ../kat/sphincs-shake-128f-simple.rsp.1
      2 7cf1ae6803f24d2fabcc2f98e77ab703fc223b6d5ddd5091658d03c13c3045b2  ../kat/sphincs-shake-192f-simple.rsp.1
      2 84644ef3f842f5939f48179bcbc1f8c0e494f5a76585c71a176547e6ef297517  ../kat/sphincs-shake-192s-simple.rsp.1
      2 a316908dec0998861b647f40213bf37cbfc2b3634b8a0a52e4503dba59bad441  ../kat/sphincs-sha2-128s-simple.rsp.1
      2 b399077b5c53daffd830df98dd3953b9db1903a151bc679adff314a93cf808d0  ../kat/sphincs-shake-256s-simple.rsp.1
      2 d36999edc06c0daeaae9e6346b4b5a5ea9503387fb343bfe869c5ccd0a90c51c  ../kat/sphincs-shake-128s-simple.rsp.1
      2 d3df72bb154abaef53cc5d0dd5b4fde46a797e90e30805fff7c3e31041ab4c67  ../kat/sphincs-sha2-128f-simple.rsp.1
      2 f525b6569082b4335593eab8257364a5754932b2a209fc194c09ffce12bfbb16  ../kat/sphincs-sha2-192f-simple.rsp.1
```

The longer known answer tests are quite big, so I include just SHA256 hashes for [1](kat/kat1-sha256.txt), [10](kat/kat10-sha256.txt), and [100](kat/kat100-sha256.txt) first ones.

##  Notes on the implementation

This self-contained implementation covers all 12 parameter sets contained in FIPS 205 IPD (Table 1, Section 10.) Note that NIST is standardizing only the "small simple" (s) and "fast simple" (f) variants using SHA2 and SHAKE hashes. The following table also includes the PQ Security Categories and byte sizes of the public keys, secret keys, and signatures with each variant.

| FIPS 205 Name      | Reference KAT Filename    | Cat | PK |  SK |   Sig |
|--------------------|---------------------------|-----|----|-----|-------|
| SLH-DSA-SHA2-128s  | sphincs-sha2-128s-simple  |  1  | 32 |  64 |  7856 |
| SLH-DSA-SHAKE-128s | sphincs-shake-128s-simple |  1  | 32 |  64 |  7856 |
| SLH-DSA-SHA2-128f  | sphincs-sha2-128f-simple  |  1  | 32 |  64 | 17088 |
| SLH-DSA-SHAKE-128f | sphincs-shake-128f-simple |  1  | 32 |  64 | 17088 |
| SLH-DSA-SHA2-192s  | sphincs-sha2-192s-simple  |  3  | 48 |  96 | 16224 |
| SLH-DSA-SHAKE-192s | sphincs-shake-192s-simple |  3  | 48 |  96 | 16224 |
| SLH-DSA-SHA2-192f  | sphincs-sha2-192f-simple  |  3  | 48 |  96 | 35664 |
| SLH-DSA-SHAKE-192f | sphincs-shake-192f-simple |  3  | 48 |  96 | 35664 |
| SLH-DSA-SHA2-256s  | sphincs-sha2-256s-simple  |  5  | 64 | 128 | 29792 |
| SLH-DSA-SHAKE-256s | sphincs-shake-256s-simple |  5  | 64 | 128 | 29792 |
| SLH-DSA-SHA2-256f  | sphincs-sha2-256f-simple  |  5  | 64 | 128 | 49856 |
| SLH-DSA-SHAKE-256f | sphincs-shake-256f-simple |  5  | 64 | 128 | 49856 |

The code output matches the Known Answer Tests of the reference implementation (["consistent-basew" branch, commit eccdc43](https://github.com/sphincs/sphincsplus/tree/consistent-basew).) That branch implements the changes in FIPS 205 IPD in relation to SPHINCS+ 3.1, which are documented in Section 1.3 of FIPS 205 IPD. Note that the naming in that implementation is still with "SPHINCS", whereas this implementation uses names from FIPS 205.

The source files `slh_*.?` are related to the implementation of SLH-DSA. The file `slh_dsa.h` provides an external API; note that this API only creates detached signatures. The key generation function `slh_keygen()` and the signature generation function `slh_sign()` expect an additional `rbg` parameter which is a callback to a `randombytes()` fuction. This allows the same code to be instantiated with real random number generators and deterministic ones for Known Answer Tests.

The instantiation for SLH-DSA-SHAKE parameters is in `slh_shake.c`; this uses my provided C implementations in `sha3_api.*` and `keccap.c`, but the API is similar to standard ones such as OpenSSL or in Kernel. Similarly, the SLH-DSA-SHA2 parameters are in `slh_sha2.c`.

The source files `kat_*.?` provide a Known Answer Test harness that creates matching test vector data to the NIST PQC harness and the reference implementation. These components have many components unrelated to SLH-DSA, such as the toy AES256-CTR implementation required for the "fake" random number generation in testing. Note that running the KAT generator test does not mean that the implementation is correct; this merely means that test vectors were successfully reproduced. One additionally needs to compare them with the reference "golden" values in the [kat](kat) subdirectory.

