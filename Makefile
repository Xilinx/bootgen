##############################################################################
#
# Copyright 2015-2020 Xilinx, Inc.
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

ifneq ($(CROSS_COMPILE), )
 CXX	= $(CROSS_COMPILE)g++
 CC	= $(CROSS_COMPILE)gcc
else ifneq ($(CROSS_COMPILER), )
 CXX	= $(CROSS_COMPILER)
 CC	= $(subst g++,gcc,$(CROSS_COMPILER))
else
 # Native host build, use the supplied compiler
 CXX	= c++
 CC	= cc
endif

OBJ = o
CXX_NO_WARN ?=  -Wno-reorder -Wno-deprecated-declarations
CXXFLAGS ?= -std=c++0x -O -Wall
CFLAGS ?= -O -Wall

# A couple of steps to workaround gmake 3.81 issues
CLANGr = $(shell ${CXX} -dM -E - < /dev/null | grep __clang__)
CLANG = $(shell echo "$(CLANGr)" | sed "s/.*__clang__.*/yes/")

ifeq ($(CLANG), yes)
CXX_NO_WARN += -Wno-deprecated-register
else
GCCVERSIONGTEQ9 := $(shell expr `${CXX} -dumpversion | cut -f1 -d.` \>= 9)
ifeq "$(GCCVERSIONGTEQ9)" "1"
CXX_NO_WARN += -Wno-aligned-new -Wno-misleading-indentation -Wno-class-memaccess
endif
endif

CXXFLAGS += $(CXX_NO_WARN)

EXEC = bootgen
UNAME := $(shell uname)

ifeq ($(UNAME),$(filter $(UNAME),Linux FreeBSD))
INCLUDE_SYS = -I.
LIBS    = -lssl -lcrypto
RTLIBS  =
OPTIONS_USER =
endif

INCLUDE = $(INCLUDE_USER) $(INCLUDE_SYS)

OPTIONS = $(OPTIONS_USER)

DEPS_GEN = -MD -MP -MF ${@:.o=.d}

all: $(EXEC) $(RTLIBS)

OBJECTS = $(addsuffix .o, $(basename $(wildcard *.cpp)))
OBJECTS += $(addsuffix .o, $(basename $(wildcard *.c)))

DEPS = $(OBJECTS:.o=.d)

%.${OBJ} : %.cpp
	${CXX} -c ${CXXFLAGS} $(OPTIONS) $(DEPS_GEN) ${INCLUDE} $<

%.${OBJ} : %.c
	${CC} -c ${CFLAGS} $(OPTIONS) $(DEPS_GEN) ${INCLUDE} $<

${EXEC}: $(OBJECTS)
	echo Building executable file: $@...
	${CXX} $(CXXFLAGS) $(LDFLAGS) $(OPTIONS_USER) -o $@ $(OBJECTS) $(LIBS)

execs: ${EXEC}

clean:
	echo
	rm -rf ${EXEC}
	rm -f $(OBJECTS)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif
