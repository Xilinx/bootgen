##############################################################################
# 
# Copyright 2015-2019 Xilinx, Inc.
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
# Building Bootgen for Host:
#    'make "LIBS=<openssl/lib/libssl.a> <openssl/lib/libcrypto.a> -ldl 
#           -lpthread" "INCLUDE_USER=-I<openssl/include>"'
#
# Building Bootgen for ARM:
#    'make "CROSS_COMPILER=aarch64-linux-gnu-g++" "LIBS=<opensslarm/lib/libssl.a> \
#           <opensslarm/lib/libcrypto.a> -ldl -lpthread" 
#           "INCLUDE_USER=-I<opensslarm/include>"'
#
##############################################################################
CROSS_COMPILER ?= g++

OBJ = o
CXXFLAGS ?= -std=c++0x -O -Wall -Wno-reorder -Wno-deprecated-declarations

EXEC = bootgen
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
INCLUDE_SYS = -I.
LIBS    = -lssl -lcrypto
RTLIBS  = 
OPTIONS_USER = 
endif

INCLUDE = $(INCLUDE_USER) $(INCLUDE_SYS)

OPTIONS = $(OPTIONS_USER) -g

all: $(EXEC)  $(RTLIBS)

OBJECTS = bif.tab.o \
bif.yy.o \
reginit.tab.o \
reginit.yy.o \
cmdoptions.tab.o \
cmdoptions.yy.o \
authentication.o \
authentication-zynq.o \
authentication-zynqmp.o \
authkeys.o \
binary.o \
binfile.o \
bitutils.o \
options.o \
bifoptions.o \
bootheader.o \
bootheader-zynq.o \
bootheader-zynqmp.o \
bootimage.o \
bootimage-zynq.o \
bootimage-zynqmp.o \
checksum.o \
elftools.o \
encryption.o \
encryptutils.o \
encryptionkeys.o\
encryption-zynq.o \
encryption-zynqmp.o \
hash.o \
imageheadertable.o \
imageheadertable-zynq.o \
imageheadertable-zynqmp.o \
Keccak-compact.o \
logger.o \
readimage.o \
readimage-zynq.o \
readimage-zynqmp.o \
verifyimage.o \
main.o \
mcsfile.o \
outputfile.o \
parsing.o \
partition.o \
partitionheadertable.o \
partitionheadertable-zynq.o \
partitionheadertable-zynqmp.o \
reginit.o

# List of all src to be compiled for bootgen.
EXECOBJS = *.cpp : *.o

# compile all object files
%.${OBJ} : %.cpp
	${CROSS_COMPILER} -c ${CXXFLAGS} $(OPTIONS) ${INCLUDE} $<

${EXEC}: $(OBJECTS)
	echo Building executable file: $@...
	${CROSS_COMPILER} $(CXXFLAGS) $(LDFLAGS) $(OPTIONS_USER) -o $@ $(OBJECTS) $(LIBS) 
		
execs: ${EXEC}

clean:
	echo
	rm -rf ${EXEC}
	rm -f $(OBJECTS)


