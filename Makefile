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

GCCVERSIONGTEQ9 := $(shell expr `${CXX} -dumpversion | cut -f1 -d.` \>= 9)
ifeq "$(GCCVERSIONGTEQ9)" "1"
CXXFLAGS += -Wno-aligned-new -Wno-misleading-indentation -Wno-class-memaccess
endif

LMS_HASH_DIR = lms-hash-sigs
LMS_LIB = hss_lib_thread.a


EXEC = build/bin/bootgen
OBJDIR = build/obj
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
INCLUDE_SYS = -I bisonflex -I common/include -I spartanup/include -I versal/include -I versal_2ve_2vm/include -I zynq/include -I zynqmp/include -I utils/include -I lms-hash-sigs -I win_include 
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

CXXFILES=$(shell find common/src spartanup/src versal/src versal_2ve_2vm/src zynq/src zynqmp/src utils/src -name "*.cpp")
OBJECTS+=$(patsubst %.cpp,build/obj/%.o,$(notdir $(CXXFILES)))

CFILES=$(shell find common/src spartanup/src versal/src versal_2ve_2vm/src zynq/src zynqmp/src utils/src -name "*.c")
OBJECTS+=$(patsubst %.c,build/obj/%.o,$(notdir $(CFILES)))

${OBJDIR}/%.${OBJ} : bisonflex/%.cpp 
	${CXX} -c ${CXXFLAGS} $(OPTIONS) ${INCLUDE} $< -o $@

${OBJDIR}/%.${OBJ} : */src/%.cpp 
	${CXX} -c ${CXXFLAGS} $(OPTIONS) ${INCLUDE} $< -o $@

${OBJDIR}/%.${OBJ} : */src/%.c 
	${CC} -c ${CFLAGS} $(OPTIONS) ${INCLUDE} $< -o $@


${EXEC}: $(OBJECTS)
	echo Building executable file: $@...
	cd ${LMS_HASH_DIR} && $(MAKE) ${LMS_LIB}
	${CXX} $(CXXFLAGS) $(LDFLAGS) $(OPTIONS_USER) -o $@ $(OBJECTS) ${LMS_HASH_DIR}/${LMS_LIB}  $(OPTIONS)$(LIBS)

execs: ${EXEC}

clean:
	echo
	rm -rf ${EXEC}
	rm -f $(OBJECTS)
	cd ${LMS_HASH_DIR} && $(MAKE) clean