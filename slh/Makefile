# #	Makefile
# #	Markku-Juhani O. Saarinen <mjos@iki.fi>.  See LICENSE.	

# #	===	This is a PC-target makefile for the core SLH-DSA implementation.
# #		No hardware acceleration if used like this -- mainly useful for
# #		debug purposes.

# XBIN	?=	kat_test
# CSRC	=	$(wildcard *.c)
# CSRC	+=	../drv/kat_drbg.c
# OBJS	= 	$(CSRC:.c=.o)
# KATNUM	?=	1

# CC 		=	gcc
# CFLAGS	+=	-Wall -march=native -Ofast -DNDEBUG
# #CFLAGS	+=	-Wall -Wextra -Wshadow -fsanitize=address,undefined -g
# CFLAGS	+=	-I. -I../drv -DKATNUM=$(KATNUM)
# LDLIBS	+=

# $(XBIN):	$(OBJS)
# 	$(CC) $(LDFLAGS) $(CFLAGS) -o $(XBIN) $(OBJS) $(LDLIBS)

# %.o:	%.[cS]
# 	$(CC) $(CFLAGS) -c $^ -o $@

# test:	$(XBIN)
# 	./$(XBIN)	
# 	sha256sum *.rsp > t.log
# 	cat t.log ../kat/kat$(KATNUM)-sha256.txt | sort | uniq -c -w 64
# 	@echo "NOTE >2<- indicates test vector comparison match, 1 is a failure."
# clean:
# 	$(RM) -rf $(XBIN) $(OBJS) *.rsp *.req *.log







#
# Makefile for the SLH-DSA static library.
# Builds all .c files in this directory into a single archive.
#

# The name of the library to be created, matching the main Makefile.
LIB = libslhdsa.a

# Compiler and archiver tools. Using CC from the parent make environment.
CC ?= gcc
AR = ar

# Compiler flags.
# -O2 is a standard optimization level.
# -Wall enables common warnings.
# -fPIC is required for creating relocatable code for libraries.
CFLAGS ?= -O2 -Wall -fPIC
CFLAGS += -I. -Idrv

# Find all C source files in the current directory, excluding the test file.
CSRC = $(filter-out kat_test.c, $(wildcard *.c))

# Create a list of object files from the source files.
OBJS = $(CSRC:.c=.o)

# The default target: build the library.
all: $(LIB)

# Rule to create the static library.
# 'rcs' flags for 'ar': create the archive, replace older files, and create an index.
$(LIB): $(OBJS)
	$(AR) rcs $(LIB) $(OBJS)

# Generic rule to compile a .c file into a .o file.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	-rm -f *.o *.a