bootgen.git - Source code for Xilinx Bootgen

# Overview
This repository contains source code to build Bootgen for Xilinx Zynq and ZU+
SoCs. Code related to generating obfuscated key is not part of this repository.
This functionality will be made available in future.

This repository also doesn't include code to support FPGA encryption and
authentication. These features are only available as part of Bootgen shipped
with Vivado tools.

For more details about Bootgen, please refer to Xilinx UG1283.

# Build instructions
Bootgen can be built for Linux and Windows OS. It can also be cross-compiled
for Linux running on Xilinx processors.

The following 3rd party packages are required to build Bootgen:

* OpenSSL v1.1.1b libraries

   * Windows libraries are available at http://slproweb.com/products/Win32OpenSSL.html
   * Linux libraries are part of libssl-dev package

## Linux Builds

A Makefile is included along with the code to build Bootgen for Linux. This
Makefile creates an executable named bootgen, when run.

1. Install OpenSSL libraries.

2. In the bootgen directory, run

   make

3. If OpenSSL libraries are located at a custom path, you can link with these
   libraries as given below.

   Assuming OpenSSL libraries are located at /home/$USER/local/openssl,

   make "LIBS=/home/\$USER/local/openssl/lib/libssl.a /home/\$USER/local/openssl/lib/libcrypto.a -ldl -lpthread" \
        "INCLUDE_USER=-I/home/\$USER/local/openssl/include"

## Windows Builds

1. Download and install "Win64 OpenSSL v1.1.1b", from http://slproweb.com/products/Win32OpenSSL.html.

2. Use Microsoft Visual Studio 2015 to build Bootgen source code and link it
.   with OpenSSL libraries downloaded in previous step.

## Cross-compiling Bootgen

Bootgen can be cross-compiled to run on Xilinx processors.

1. Cross-compile openssl libraries.

2. Cross-compile Bootgen code and link against the libraries built above
   make "CROSS_COMPILER=aarch64-linux-gnu-g++" "LIBS=<opensslarm/lib/libssl.a> \
         <opensslarm/lib/libcrypto.a> -ldl -lpthread"
         "INCLUDE_USER=-I<opensslarm/include>"'

# OpenSSL License
Bootgen links with openssl libraries. Below is openssl copyright.
```
 Copyright (c) 1998-2019 The OpenSSL Project.  All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.

 3. All advertising materials mentioning features or use of this
    software must display the following acknowledgment:
    "This product includes software developed by the OpenSSL Project
    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"

 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
    endorse or promote products derived from this software without
    prior written permission. For written permission, please contact
    openssl-core@openssl.org.

 5. Products derived from this software may not be called "OpenSSL"
    nor may "OpenSSL" appear in their names without prior written
    permission of the OpenSSL Project.

 6. Redistributions of any form whatsoever must retain the following
    acknowledgment:
    "This product includes software developed by the OpenSSL Project
    for use in the OpenSSL Toolkit (http://www.openssl.org/)"

 THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
```

