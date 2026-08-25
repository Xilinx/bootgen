# ML-DSA Library Makefile
# Builds libmldsa.a - C-compatible wrapper for the C++20 ML-DSA library
# Pattern follows lms-hash-sigs/Makefile

# Requires GCC 12 or newer. The compiler is taken from PATH, or from CXX passed
# by the parent make; no toolchain path is hardcoded here.
AR = ar

CXXFLAGS = -std=c++20 -O2 -Wall -fPIC

# ML-DSA header paths (relative to wrapper/ml-dsa-lib/)
ML_DSA_INCLUDE = ../../ml-dsa/include
SHA3_INCLUDE = ../../ml-dsa/sha3/include
RANDOMSHAKE_INCLUDE = ../../ml-dsa/RandomShake/include

INCLUDES = -I$(ML_DSA_INCLUDE) -I$(SHA3_INCLUDE) -I$(RANDOMSHAKE_INCLUDE) -I.

libmldsa.a: ml_dsa_api.o
	$(AR) rcs $@ $^

ml_dsa_api.o: ml_dsa_api.cpp ml_dsa_api.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c ml_dsa_api.cpp -o $@

clean:
	-rm -f *.o *.a
