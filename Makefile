##############################################################################
#
# Copyright 2015-2022 Xilinx, Inc.
# Copyright 2022-2023 Advanced Micro Devices, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
##############################################################################
#
# Building Bootgen for x86 Host:
#    'make "LIBS=<openssl/lib/libssl.a> <openssl/lib/libcrypto.a> -ldl
#           -lpthread" "INCLUDE_USER=-I<openssl/include>"'
#
# Building Bootgen for ARM v8:
#    'make "CROSS_COMPILE=aarch64-linux-gnu-" "LIBS=<opensslarm/lib/libssl.a> \
#           <opensslarm/lib/libcrypto.a> -ldl -lpthread"
#           "INCLUDE_USER=-I<opensslarm/include>"'
#
##############################################################################
# Create build directories (at Makefile parse time)
$(shell mkdir -p build/bin build/obj)

OBJ = o
CXXFLAGS ?= -std=c++14 -O -Wall -Wno-reorder -Wno-deprecated-declarations
CFLAGS ?= -O -Wall

CXX_VERSION := $(shell ${CXX} -dumpversion 2>/dev/null)
CXX_MAJOR   := $(firstword $(subst ., ,$(CXX_VERSION)))

# make defaults CC to "cc", which on most hosts is the system compiler even when
# a newer gcc is first on PATH. Prefer "gcc" so the C objects come from the same
# toolchain as the C++ ones, unless the caller supplied a compiler.
ifeq ($(origin CC),default)
CC = gcc
endif

# lms-hash-sigs defaults to -g, and a recent GCC then hands the assembler a
# --gdwarf-N option that binutils 2.30 rejects. Drop the debug flag rather than
# require a newer binutils, which would mean naming another toolchain path. Its
# Makefile uses a bare "CFLAGS ?=" with no include flags, so overriding is safe.
LMS_CFLAGS = -Wall -O0

ifeq ($(shell test "$(CXX_MAJOR)" -ge 9 2>/dev/null && echo ok),ok)
CXXFLAGS += -Wno-aligned-new -Wno-misleading-indentation -Wno-class-memaccess
endif

# The sources are C++14, so the host compiler is enough and no rpath is needed.
# Building with a toolchain newer than the host's is still supported: pass
# CXX_RUNTIME_RPATH=1 to record that toolchain's libstdc++ in the binary. Asking
# the compiler keeps this file free of hardcoded toolchain paths. Static linking
# is not usable here: the shipped libstdc++.a has compressed debug sections that
# older system linkers cannot read.
ifeq ($(CXX_RUNTIME_RPATH),1)
CXX_LIBDIR := $(patsubst %/,%,$(dir $(shell $(CXX) -print-file-name=libstdc++.so)))
ifneq ($(CXX_LIBDIR),)
LDFLAGS += -L$(CXX_LIBDIR) -Wl,-rpath,$(CXX_LIBDIR)
endif
endif

LMS_HASH_DIR = lms-hash-sigs
LMS_LIB = hss_lib_thread.a

# ML-DSA / SLH-DSA compatibility headers over the OpenSSL EVP implementation
ML_DSA_INC_DIR = wrapper/ml-dsa-lib
PQC_DIR = wrapper/pqc-openssl
SLH_DSA_DIR = slh

EXEC = build/bin/bootgen
OBJDIR = build/obj
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
# ml-dsa-lib provides ML-DSA compatibility API headers, compat/ provides std::span polyfill
INCLUDE_SYS = -I ${ML_DSA_INC_DIR}/compat -I ${ML_DSA_INC_DIR} -I bisonflex -I common/include -I spartanup/include -I versal/include -I versal_2ve_2vm/include -I versal_2vp/include -I zynq/include -I zynqmp/include -I utils/include -I lms-hash-sigs -I win_include -I ${SLH_DSA_DIR} -I ${PQC_DIR}
LIBS    = -lssl -lcrypto
RTLIBS  =
OPTIONS_USER =
endif

INCLUDE = $(INCLUDE_USER) $(INCLUDE_SYS)

OPTIONS = $(OPTIONS_USER) -lpthread 

all: $(EXEC) $(RTLIBS)

GRAMMAR = bif.tab.cpp bif.yy.cpp reginit.tab.cpp reginit.yy.cpp cmdoptions.tab.cpp cmdoptions.yy.cpp

GFILES=$(shell echo $(GRAMMAR))  # Assuming GRAMMAR is a list of .cpp files
OBJECTS=$(patsubst %.cpp,${OBJDIR}/%.o,$(GFILES))

CXXFILES=$(shell find common/src spartanup/src versal/src versal_2ve_2vm/src versal_2vp/src zynq/src zynqmp/src utils/src ${PQC_DIR} -name "*.cpp")
OBJECTS+=$(patsubst %.cpp,build/obj/%.o,$(notdir $(CXXFILES)))

CFILES=$(shell find common/src spartanup/src versal/src versal_2ve_2vm/src zynq/src zynqmp/src utils/src ${SLH_DSA_DIR} -name "*.c")
OBJECTS+=$(patsubst %.c,build/obj/%.o,$(notdir $(CFILES)))

${OBJDIR}/%.${OBJ} : bisonflex/%.cpp 
	${CXX} -c ${CXXFLAGS} $(OPTIONS) ${INCLUDE} $< -o $@

${OBJDIR}/%.${OBJ} : */src/%.cpp 
	${CXX} -c ${CXXFLAGS} $(OPTIONS) ${INCLUDE} $< -o $@

${OBJDIR}/%.${OBJ} : */src/%.c 
	${CC} -c ${CFLAGS} $(OPTIONS) ${INCLUDE} $< -o $@

${OBJDIR}/%.${OBJ} : ${PQC_DIR}/%.cpp
	${CXX} -c ${CXXFLAGS} $(OPTIONS) ${INCLUDE} $< -o $@

${OBJDIR}/%.${OBJ} : ${SLH_DSA_DIR}/%.c
	${CC} -c ${CFLAGS} $(OPTIONS) ${INCLUDE} $< -o $@


${EXEC}: $(OBJECTS)
	echo Building executable file: $@...
	cd ${LMS_HASH_DIR} && $(MAKE) CC="${CC}" CFLAGS="${LMS_CFLAGS}" ${LMS_LIB}
	${CXX} $(CXXFLAGS) $(LDFLAGS) $(OPTIONS_USER) -o $@ $(OBJECTS) ${LMS_HASH_DIR}/${LMS_LIB} $(OPTIONS)$(LIBS)

execs: ${EXEC}

clean:
	echo
	rm -rf ${EXEC}
	rm -f $(OBJECTS)
	cd ${LMS_HASH_DIR} && $(MAKE) clean
