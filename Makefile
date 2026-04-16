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
#__________________________________________________________________________________________________________________________
# Create build directories (at Makefile parse time)
$(shell mkdir -p build/bin build/obj)

# Override the standard bison/flex installs
BISON_HOME = $(shell readlink -f /wrk/ptx_1/users/bootgen_libs/installs/bison-2.7)
FLEX_HOME = $(shell readlink -f /wrk/ptx_1/users/bootgen_libs/installs/flex-2.5.35)
M4_HOME = $(shell readlink -f /wrk/ptx_1/users/bootgen_libs/installs/m4-1.4.16)

OBJ = o
CFLAGS ?= -std=c++0x -Wall -Wno-reorder -Wno-deprecated-declarations
# Setup for alternate version of bison/flex
BISON = ${BISON_HOME}/bin/bison
BISON_PKGDATADIR = ${BISON_HOME}/share/bison
FLEX = ${FLEX_HOME}/bin/flex
M4 = ${M4_HOME}/bin/m4
export BISON FLEX M4 BISON_PKGDATADIR
BF_PATH = export BISON=${BISON}; export FLEX=${FLEX}; export M4=${M4}; export PATH=${BISON}:${FLEX}:${M4}:${PATH};

# Directory where code generation will take place
BF_GENDIR = bisonflex
#___________________________________________________________________________________________________________________________
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
# Add -isystem for OpenSSL to ensure system headers take precedence
# over GCC's include-fixed directory which may have old bn.h with struct definitions
INCLUDE_SYS = -I bisonflex -I common/include -I spartanup/include -I versal/include -I versal_2ve_2vm/include -I zynq/include -I zynqmp/include -I utils/include -I lms-hash-sigs -I win_include 
LIBS    = -lssl -lcrypto
RTLIBS  =
OPTIONS_USER =
# Detect if using Batonroot/devkits GCC which has include-fixed with old OpenSSL structs
BATONROOT_GCC := $(shell ${CXX} -print-search-dirs 2>/dev/null | grep -c batonroot)
ifneq ($(BATONROOT_GCC),0)
CXXFLAGS += -DXIL_BIGNUM_STRUCTS_DEFINED
CFLAGS += -DXIL_BIGNUM_STRUCTS_DEFINED
endif
endif

INCLUDE = $(INCLUDE_USER) $(INCLUDE_SYS)

OPTIONS = $(OPTIONS_USER) -lpthread 

all: $(EXEC) $(RTLIBS)

GRAMMAR = bif.tab.cpp bif.yy.cpp reginit.tab.cpp reginit.yy.cpp cmdoptions.tab.cpp cmdoptions.yy.cpp

%/.dir :
	mkdir -p $*
	touch $@

GFILES=$(shell echo $(GRAMMAR))  # Assuming GRAMMAR is a list of .cpp files
OBJECTS=$(patsubst %.cpp,${OBJDIR}/%.o,$(GFILES))

CXXFILES=$(shell find common/src spartanup/src versal/src versal_2ve_2vm/src zynq/src zynqmp/src utils/src -name "*.cpp")
OBJECTS+=$(patsubst %.cpp,build/obj/%.o,$(notdir $(CXXFILES)))

CFILES=$(shell find common/src spartanup/src versal/src versal_2ve_2vm/src zynq/src zynqmp/src utils/src -name "*.c")
OBJECTS+=$(patsubst %.c,build/obj/%.o,$(notdir $(CFILES)))
#-----------------------------------------FLEX&LEXER-FOR-DEV-----------------------------------------------
CPP_BISON = ${BF_GENDIR}/bif.yy.cpp ${BF_GENDIR}/cmdoptions.yy.cpp ${BF_GENDIR}/reginit.yy.cpp
CPP_FLEX = ${BF_GENDIR}/bif.tab.cpp ${BF_GENDIR}/cmdoptions.tab.cpp ${BF_GENDIR}/reginit.tab.cpp

${BF_GENDIR}/%.tab.cpp : parser/%.y ${BF_GENDIR}/%/.dir
	${BF_PATH} cd ${BF_GENDIR}/$*; ${BISON} --report=state --file-prefix=$* --output=$*.tab.cpp -d ../../$<
	sed 's/stack</BIF::stack</g' ${BF_GENDIR}/$*/$*.tab.hpp > ${BF_GENDIR}/$*.tab.hpp
	sed 's/slice</BIF::slice</g' ${BF_GENDIR}/$*/$*.tab.cpp > ${BF_GENDIR}/$*.tab.cpp


${BF_GENDIR}/bif.tab.cpp : parser/bif.y ${BF_GENDIR}/.dir \
								${BF_GENDIR}/cmdoptions.yy.cpp \
								${BF_GENDIR}/reginit.yy.cpp ${BF_GENDIR}/.dir
	${BF_PATH} ${BISON} --report=state --file-prefix=bif --output=$@ -d $<


${BF_GENDIR}/%.yy.cpp : parser/%.l 
	${BF_PATH} ${FLEX} --prefix=$* --outfile=$@ $<
#-----------------------------------------FLEX&LEXER-FOR-DEV-----------------------------------------------
${OBJDIR}/%.${OBJ} : bisonflex/%.cpp ${CPP_BISON} ${CPP_FLEX}
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
	rm -rf build/
	cd ${LMS_HASH_DIR} && $(MAKE) clean

dist_clean:
	rm -rf ${BF_GENDIR}

.PRECIOUS: %/.dir ${BF_GENDIR}/%.yy.cpp ${BF_GENDIR}/%.tab.cpp