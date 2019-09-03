/******************************************************************************
* Copyright 2015-2019 Xilinx, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/

#pragma once

#ifndef _HELP_H_
#define _HELP_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
#include <string>



/******************************************************************************
 arch
******************************************************************************/
#define ARCHHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | arch                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Xilinx family architecture for which the bootimage needs to    |\n\
             | be created                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -arch [arguments]                                              |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | *zynq   : Image is targeted for ZYNQ architecture              |\n\
             |  zynqmp : Image is targeted for ZYNQ MP SoC architecture       |\n\
             |  fpga   : Image is targeted for other FPGA architectures       |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -arch zynq -image test.bif -o boot.bin]               |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | boot.bin generated is a Zynq boot image                        |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 image
******************************************************************************/
#define IMAGEHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | image                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Input BIF file name                                            |\n\
             | BIF file specifies each component of the boot image in order of|\n\
             | boot and allows optional attributes to be specified to each    |\n\
             | image component. Each image component is usually mapped to a   |\n\
             | partition, but in some cases an image component can be mapped  |\n\
             | to more than one partition if the image component is not       |\n\
             | contiguous in memory                                           |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -image <bif filename>                                          |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -arch zynq -image test.bif -o boot.bin]               |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | test.bif is the input BIF file                                 |\n\
             | Sample BIF file is shown below                                 |\n\
             | the_ROM_image:                                                 |\n\
             | {                                                              |\n\
             |     [init] init_data.int                                       |\n\
             |     [bootloader] Fsbl.elf                                      |\n\
             |     Partition1.bit                                             |\n\
             |     Partition2.elf                                             |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 split
******************************************************************************/
#define SPLITHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | split                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option outputs each data partition with headers as a new  |\n\
             | file in MCS or BIN format.                                     |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -split [arguments]                                             |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | bin : Output files generated in bin format                     |\n\
             | mcs : Output files generated in mcs format                     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -arch zynq -image test.bif -split bin]                |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Consider the sample bif                                        |\n\
             | the_ROM_image:                                                 |\n\
             | {                                                              |\n\
             |     [bootloader] Fsbl.elf                                      |\n\
             |     Partition1.bit                                             |\n\
             |     Partition2.elf                                             |\n\
             | }                                                              |\n\
             | Output files generated are:                                    |\n\
             | 1. Bootheader + Image Headers + Partition Headers + Fsbl.elf   |\n\
             | 2. Partition1.bit                                              |\n\
             | 3. Partition2.elf                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 fill
******************************************************************************/
#define FILLHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | fill                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option specifies the byte to use for filling extra memory |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -fill <hex byte>                                               |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -arch zynq -image test.bif -fill 0xAB -o boot.bin]    |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Byte 0xAB is padded in the header tables instead of 0xFF when  |\n\
             | fill is specified.                                             |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 o
******************************************************************************/
#define OHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | o                                                              |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option specifies the name of the output image file with   |\n\
             | bin or mcs extension.                                          |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -o <filename>                                                  |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -arch zynq -image test.bif -o boot.mcs]               |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | The output image is generated with name boot.mcs               |\n\
             | The format of the output image is decided based on the file    |\n\
             | extension of the file given with '-o' option                   |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 p
******************************************************************************/
#define PHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | p                                                              |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option specifies the partname of the Xilinx chip.         |\n\
             | This is needed for generating a encryption key. It is copied   |\n\
             | verbatim to the *.nky file in the 'Device' line.               |\n\
             | This is applicable only with encryption is enabled.            |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -p <part name>                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -image test.bif -o boot.bin -p xc7z020clg484          |\n\
             |   -encrypt efuse]                                              |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | If the key file is not present in the path specified in BIF    |\n\
             | file, then a new encryption key is generated in the same path  |\n\
             | and xc7z020clg484 is copied along side the 'Device' field in   |\n\
             | the nky file. The generated image is an encrypted image.       |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 w
******************************************************************************/
#define WHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | w                                                              |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option specifies whether to overwrite an existing file or |\n\
             | not.                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -w [arguments]                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   |  on  : Overwrite the existing file                             |\n\
             | *off : Don't overwrite the existing file                       |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -image test.bif -w on -o boot.bin]  or                |\n\
             | [bootgen -image test.bif -w    -o boot.bin]                    |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | If the file boot.bin already exists in the path, then it is    |\n\
             | overwritten.                                                   |\n\
             | Options '-w on' & '-w' are treated as same                     |\n\
             | If the '-w' option is not specified, the file will not be      |\n\
             | overwritten by default                                         |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 efuseppkbits
******************************************************************************/
#define EFUSEPPKBITSHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | efuseppkbits                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option specifies the name of the efuse file to be written |\n\
             | to contain the PPK hash. This option generates a direct hash   |\n\
             | without any padding.                                           |\n\
             | Zynq   : Hashing done by SHA2                                  |\n\
             | ZynqMP : Hashing done by SHA3                                  |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -efuseppkbits <filename>                                       |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -image test.bif -o boot.bin                           |\n\
             |          -efuseppkbits efusefile.txt]                          |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | efusefile.txt file gets generated, which contains the hash of  |\n\
             | the PPK key.                                                   |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 zynqmpes1
******************************************************************************/
#define ZYNQMPES1HELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | zynqmpes1                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option specifies that the image generated will be used    |\n\
             | on ES1(1.0).                                                   |\n\
             | This option makes a difference only when generating            |\n\
             | authenticated image, else ignored.                             |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -zynqmpes1                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -image test.bif -o boot.bin -zynqmpes1 ]              |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | The authentication padding scheme will be as per ES1(1.0).     |\n\
             | The default padding scheme is for (2.0)ES2 and above.          |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 generate_hashes
******************************************************************************/
#define GENERATEHASHESHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | generate_hashes                                                |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option generates file containing PKCS#1v1.5 padded hash   |\n\
             |----------------------------------------------------------------|\n\
             | Zynq Format:                                                   |\n\
             |    SHA-2 (256 bytes)                                           |\n\
             |       +-----------+-----------+------+------+------+------+    |\n\
             |       | SHA2 Hash | T-Padding | 0x00 | 0xFF | 0x01 | 0x00 |    |\n\
             |       +-----------+-----------+------+------+------+------+    |\n\
             | Bytes:|    32*    |     19    |   1  |  202 |  1   |   1  |    |\n\
             |       +-----------+-----------+------+------+------+------+    |\n\
             |            * 32 byte hash is reversed.                         |\n\
             |----------------------------------------------------------------|\n\
             | ZynqMP Format:                                                 |\n\
             |    SHA-3 (384 bytes)                                           |\n\
             |       +------+------+------+------+-----------+-----------+    |\n\
             |       | 0x00 | 0x01 | 0xFF | 0x00 | T-Padding | SHA3 Hash |    |\n\
             |       +------+------+------+------+-----------+-----------+    |\n\
             | Bytes:|   1  |   1  |  314 |   1  |    19     |    48     |    |\n\
             |       +------+------+------+------+-----------+-----------+    |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -generate_hashes                                               |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -image test.bif -generate_hashes]                     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | To generate a SPK hash                                         |\n\
             | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [spkfile]secondary.pub                                      |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 padimageheader
******************************************************************************/
#define PADHDRHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | padimageheader                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option pads the Image Header Table and Partition Header   |\n\
             | Table to maximum partitions allowed, to force alignment of     |\n\
             | following partitions. This feature is enabled by default.      |\n\
             | Specifying a 0, disables this feature                          |\n\
             | Zynq   : Maximum Partitions - 14                               |\n\
             | ZynqMP : Maximum Partitions - 32 partitions                    |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -padimageheader=[arguments]                                    |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | *1 : Pad the header tables to max partitions                   |\n\
             |  0 : Do not pad the header tables                              |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -image test.bif -w on -o boot.bin -padimageheader=0]  |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | boot.bin has the image header tables and partition header      |\n\
             | tables in actual and no extra tables are padded. If nothing is |\n\
             | specified or -padimageheader=1, then total image header tables |\n\
             | and partition header tables are padded to max partitions.      |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 spksignature
******************************************************************************/
#define SPKSIGNHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | spksignature                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option is used to generate the SPK signature file. This   |\n\
             | option must be used only when [spkfile] & [pskfile] are        |\n\
             | specified in BIF                                               |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -spksignature <filename>                                       |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -image test.bif -w on -o boot.bin                     |\n\
             |                          -spksignature spksignfile.txt]        |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | The SPK signature file (spksignfile.txt) is generated.         |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 debug
******************************************************************************/
#define DEBUGHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | debug                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option is deprecated.                                     |\n\
             | Please refer -log option                                       |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 encrypt
******************************************************************************/
#define ENCRYPTHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | encrypt                                                        |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option specifies how to do encryption, where the keys are |\n\
             | stored.                                                        |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -encrypt [key source args] <filename>                          |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | Key source arguments:                                          |\n\
             |   efuse : The AES key is stored in EFUSE                       |\n\
             |   bbram : The AES key is stored in BBRAM                       |\n\
             |----------------------------------------------------------------|\n\
             | filename : Key file name in .nky format. IV, AES and HMAC keys |\n\
             |            are specified in the file itself.                   |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -image test.bif -o boot.bin -encrypt efuse]           |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | The .nky key file is passed through BIF file attribute         |\n\
             | [aeskeyfile]. Only source is specified through command line.   |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 generate_keys
******************************************************************************/
#define GENKEYSHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | generate_keys                                                  |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option generates keys.                                    |\n\
             | . Generates authentication keys in rsa/pem format.             |\n\
             | . Generate Obfuscated Key for encryption.                      |\n\
             | BIF File should have the paths of all the keys.                |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | For authentication :                                           |\n\
             |         -generate_keys [arguments]                             |\n\
             | For encryption :                                               |\n\
             |         -generate_keys obfuscatedkey                           |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | Format of authentication keys                                  |\n\
             |   rsa : RSA format keys                                        |\n\
             |   pem : PEM format keys                                        |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | For authentication :                                           |\n\
             |         [bootgen -image test.bif -generate_keys rsa]           |\n\
             | For encryption :                                               |\n\
             |         [bootgen -image test.bif -generate_keys obfuscatedkey] |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | For authentication :                                           |\n\
             | Contents of BIF file: test.bif                                 |\n\
             | image:                                                         |\n\
             | {                                                              |\n\
             |     [ppkfile] <path/ppkgenfile.txt>                            |\n\
             |     [pskfile] <path/pskgenfile.txt>                            |\n\
             |     [spkfile] <path/spkgenfile.txt>                            |\n\
             |     [sskfile] <path/sskgenfile.txt>                            |\n\
             | }                                                              |\n\
             | The key files are generated in the paths mentioned above       |\n\
             |----------------------------------------------------------------|\n\
             | For encryption :                                               |\n\
             | image:                                                         |\n\
             | {                                                              |\n\
             |     [keysrc_encryption] bbram_red_key                          |\n\
             |     [aeskeyfile] aes.nky                                       |\n\
             |     [bh_key_iv] bhkeyiv.txt                                    |\n\
             |     [familykey] familykey.txt                                  |\n\
-------------+----------------------------------------------------------------+\n\
 NOTES       | PEM Key Format:                                                |\n\
             |   -----BEGIN RSA PRIVATE KEY-----                              |\n\
             |   MIIEpAIBAAKCAQEAmlJyPcZVXltASHrtm/YnOMxskf0k2RZrIajqymqZptnG |\n\
             |   kyBMalXaqmGb1kqGCgGZVvQt3FSRO3yXa....                        |\n\
             |   -----END RSA PRIVATE KEY-----                                |\n\
             |----------------------------------------------------------------|\n\
             | RSA Key Format:                                                |\n\
             |   N = c6b9a521234567890abc4d4567e99f1234567891235987564....    |\n\
             |   E = 10001                                                    |\n\
             |   D = 37c80c81234567890abcdef1234567890abcdef1234567790....    |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 dual_qspi_mode
******************************************************************************/
#define DQSPIHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | dual_qspi_mode                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option generates output files as per the configuration    |\n\
             | specified by the Dual QSPI mode                                |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -dual_qspi_mode [arguments]                                    |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | parallel :                                                     |\n\
             |     The QSPI configuration is Dual Parallel                    |\n\
             |     Generates 2 output files, the actual output is written to  |\n\
             |     two different files in bit-by-bit fashion.                 |\n\
             | stacked <size> :                                               |\n\
             |     The QSPI configuration is Dual Stacked.                    |\n\
             |     Generates 2 output files, in sequential fashion.           |\n\
             |     <size> - Size of flash in MB (16/32/64/128)                |\n\
             | Both the flashes can then be programmed independently.         |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | 1. [bootgen -image test.bif -o -boot.bin                       |\n\
             |                      -dual_qspi_mode parallel]                 |\n\
             |----------------------------------------------------------------|\n\
             | 2. [bootgen -image test.bif -o -boot.bin                       |\n\
             |                      -dual_qspi_mode stacked 64]               |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Suppose the output bootimage content is around 120MB           |\n\
             | Usage 1: Two files are generated boot_1.bin & boot_2.bin of    |\n\
             |          60MB each                                             |\n\
             | Usage 2: Two files are generated boot_1.bin & boot_2.bin of    |\n\
             |          64MB & 56MB                                           |\n\
             |----------------------------------------------------------------|\n\
             | Suppose the output bootimage content is around 50MB            |\n\
             | Usage 1: Two files are generated boot_1.bin & boot_2.bin of    |\n\
             |          25MB each                                             |\n\
             | Usage 2: Only one file is generated boot_1.bin of 50MB         |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 log
******************************************************************************/
#define LOGHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | log                                                            |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This command generates log while generating the bootimage.     |\n\
             | There are various options for choosing the level of information|\n\
             | The information is displayed on the console as well as log file|\n\
             | named 'bootgen_log.txt' in the current working directory.      |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -log [arguments]                                               |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | error   : Only the error information is captured.              |\n\
             | warning : The warnings & error information is captured.        |\n\
             |*info    : The general info and all the above info is captured. |\n\
             | debug   : The debugging info in case of errors is captured in  |\n\
             |           in detail and all the above info is captured.        |\n\
             | trace   : More detailed information is captured along with the |\n\
             |           above                                                |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -image test.bif -o -boot.bin -log trace]              |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | The detailed info is described while creating the bootimage.   |\n\
             | The file 'bootgen_log.txt' is generated.                       |\n\
-------------+----------------------------------------------------------------+\n"


/******************************************************************************
 bif_help
******************************************************************************/
#define BIFHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | bif_help                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option is used to list all the supported BIF file         |\n\
             | attributes.                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -bif_help [arguments]                                          |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | Any of the bif keywords                                        |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | 1. [bootgen -bif_help]                                         |\n\
             | 2. [bootgen -bif_help aeskeyfile]                              |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Usage 1: All the bif file attributes are listed                |\n\
             | Usage 2: Detailed explanation of the attribute aeskeyfile with |\n\
             |          example                                               |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 process_bitstream
******************************************************************************/
#define PROCESSBITHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | process_bitstream                                              |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Processes only bitstream from the BIF, and output as an MCS or |\n\
             | a BIN file.                                                    |\n\
             | For example: If encryption is selected for bitstream in the BIF|\n\
             | file, the output is an encrypted bitstream.                    |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -process_bitstream [arguments]                                 |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | bin : Output in bin format                                     |\n\
             | mcs : Output in mcs format                                     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -arch zynq -image test.bif -process_bitstream bin]    |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    system.bit                                                  |\n\
             | }                                                              |\n\
             | Output generated is bitstream in BIN format.                   |\n\
-------------+----------------------------------------------------------------+\n"	

/******************************************************************************
 encryption_dump
******************************************************************************/
#define ENCRDUMPHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | *** Only for ZynqMP Architecture ***                           |\n\
             | encryption_dump                                                |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Generates Encryption log file, aes_log.txt                     |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -encryption_dump                                               |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -arch zynqmp -image test.bif -encryption_dump]        |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [keysrc_encryption] bbram_red_key                           |\n\
             |    [bootloader, encryption=aes, aeskeyfile=test.nky] fsbl.elf  |\n\
             |    [encryption=aes,aeskeyfile=test2.nky] hello.elf             |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | aes_log.txt generarted has the details of AES Key/IV pairs used|\n\
             | for encrypting each block of data. It also logs the partition  |\n\
             | and the aes key file used to encrypt it.                       |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 nonbooting
******************************************************************************/
#define NONBOOTINGHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | nonbooting                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Used for generating an intermediate boot image.                |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -nonbooting                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -arch zynq -image test.bif -o test.bin -nonbooting]   |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |  [ppkfile]primary.pub                                          |\n\
             |  [spkfile]secondary.pub                                        |\n\
             |  [spksignature]secondary.pub.sha256.sig                        |\n\
             |  [bootimage,authentication=rsa,                                |\n\
             |                    presign=fsbl_0.elf.0.sha256.sig]fsbl_e.bin  |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | An intermediate image test.bin is generated as output even in  |\n\
             | the absence of secret key, which is actually needed to generate|\n\
             | an authenticated image.                                        |\n\
             | This intermediate image cannot be booted.                      |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 verify
******************************************************************************/
#define VERIFYHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | verify                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Used for verifying authentication of a boot image.             |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -verify                                                        |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -arch zynqmp -verify boot.bin]                        |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | All the authentication certificates in a boot image will be    |\n\
             | verified againist the available partitions.                    |\n\
             | The verification process is done as below -                    |\n\
             | 1. Verify Header Authentication Certificate                    |\n\
             |      verify SPK Signature                                      |\n\
             |      verify Header Signature                                   |\n\
             | 2. Verify Bootloader Authentication Certificate                |\n\
             |      verify Boot Header Signature                              |\n\
             |      verify SPK Signature                                      |\n\
             |      verify Bootloader Signature                               |\n\
             | 3. Verifiy Partition Authentication Certificate                |\n\
             |      verify SPK Signature                                      |\n\
             |      verify Partition Signature                                |\n\
             |    This is repeated for all partitions in the given boot image.|\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 init
******************************************************************************/
#define H_BIF_INIT_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | init                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Register initialization block at the end of the bootloader,    |\n\
             | built by parsing the .int file specification. Maximum of 256   |\n\
             | address-value init pairs are allowed. The int files have       |\n\
             | a specific format.                                             |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [init] <filename>                                              |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [init] test.int                                             |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Sample .int file - test.int                                    |\n\
             |      .set. 0xF8000120 = 0x1F000200;                            |\n\
             |      .set. 0xF8000720 = 0x00000202;                            |\n\
             |      .set. 0xF800014C = 0x00000521;                            |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 aeskeyfile
******************************************************************************/
#define H_BIF_AES_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | aeskeyfile                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | The path to the AES keyfile. The keyfile contains AES key used |\n\
             | to encrypt the partitions. The contents of the key file needs  |\n\
             | to be written to efuse or bbram.                               |\n\
             | If the key file is not present in the path specified, a new key|\n\
             | is generated by bootgen, which is used for encryption.         |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       |  [aeskeyfile] <key filename>                                   |\n\
             |  [aeskeyfile = <keyfile name>] <partition>                     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | For Zynq only:                                                 |\n\
             | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [keysrc_encryption] bbram_red_key                         |\n\
             |      [aeskeyfile] test.nky                                     |\n\
             |      [bootloader, encryption=aes] fsbl.elf                     |\n\
             |      [encryption=aes] hello.elf                                |\n\
             | }                                                              |\n\
             | The partitions fsbl.elf & hello.elf are encrypted using        |\n\
             | test.nky key file.                                             |\n\
             |----------------------------------------------------------------|\n\
             | Sample key (.nky) file - test.nky                              |\n\
             |  Device       xc7z020clg484;                                   |\n\
             |  Key 0        8177B12032A7DEEE35D0F71A7FC399027BF....D608C58;  |\n\
             |  Key StartCBC 952FD2DF1DA543C46CDDE4F811506228;                |\n\
             |  Key HMAC     123177B12032A7DEEE35D0F71A7FC3990BF....127BD89;  |\n\
-------------+----------------------------------------------------------------+\n\
             | For ZynqMP only:                                               |\n\
             | * Each partition being encrypted needs its own aeskeyfile.     |\n\
             | * Key0, IV0 and Key Opt should be the same accross all nky     |\n\
             | files that will be used.                                       |\n\
             | * For cases where multiple partitions are generated for an elf |\n\
             | file, each partition can be encrypted using keys from a unique |\n\
             | key file. [Refer Example 2]                                    |\n\
             |----------------------------------------------------------------|\n\
             | Example 1:                                                     |\n\
             | The partition fsbl.elf is encrypted with keys in test.nky,     |\n\
             | hello.elf using keys in test1.nky & app.elf using keys in      |\n\
             | test2.nky.                                                     |\n\
             | Sample BIF - test_multiple.bif                                 |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [keysrc_encryption] bbram_red_key                         |\n\
             |      [bootloader,encryption=aes,aeskeyfile=test.nky] fsbl.elf  |\n\
             |      [encryption=aes,aeskeyfile=test1.nky] hello.elf           |\n\
             |      [encryption=aes,aeskeyfile=test2.nky] app.elf             |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Example 2:                                                     |\n\
             | Consider bootgen creates 3 partitions for hello.elf            |\n\
             |           - hello.elf.0, hello.elf.1 & hello.elf.2             |\n\
             |                                                                |\n\
             | Sample BIF - test_multiple.bif                                 |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [keysrc_encryption] bbram_red_key                         |\n\
             |      [bootloader,encryption=aes,aeskeyfile=test.nky] fsbl.elf  |\n\
             |      [encryption=aes,aeskeyfile=test1.nky] hello.elf           |\n\
             | }                                                              |\n\
             |                                                                |\n\
             | The partition fsbl.elf is encrypted with keys in test.nky.     |\n\
             | All hello.elf partitions are encrypted using keys in test1.nky.|\n\
             |                                                                |\n\
             | User can have unique key files for each hello partition by     |\n\
             | having key files named test1.1.nky & test1.2.nky in the same   |\n\
             | path as test1.nky.                                             |\n\
             | hello.elf.0 uses test1.nky                                     |\n\
             | hello.elf.1 uses test1.1.nky                                   |\n\
             | hello.elf.2 uses test1.2.nky                                   |\n\
             | If any of the key files (test1.1.nky or test1.2.nky) is not    |\n\
             | present, bootgen generated the key file.                       |\n\
-------------+----------------------------------------------------------------+"

/******************************************************************************
 ppkfile, pskfile, spkfile, sskfile
******************************************************************************/
#define H_BIF_PPK_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | ppkfile, pskfile, spkfile, sskfile                             |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | These keys are used to authenticate partitions in the bootimage|\n\
             | Xilinx SoCs use primary & secondary keys for authentication.   |\n\
             | The primary keys authenticate the secondary keys and the       |\n\
             | secondary keys authenticate the partitions.                    |\n\
             |    PPK - Primary Public Key                                    |\n\
             |    PSK - Primary Secret Key                                    |\n\
             |    SPK - Secondary Public Key                                  |\n\
             |    SSK - Secondary Secret Key                                  |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [ppkfile] <key filename>                                       |\n\
             | [pskfile] <key filename>                                       |\n\
             | [spkfile] <key filename>                                       |\n\
             | [sskfile] <key filename>                                       |\n\
             |----------------------------------------------------------------|\n\
             | For ZynqMP only :                                              |\n\
             |  - PPK/PSK cannot be specific to partition.                    |\n\
             |  - Each partition can have its own SPK/SSK.(Refer sample BIF 2)|\n\
             |    The SPKs can be differentiated using the spk_id.            |\n\
             |  - SPK/SSK outside the partition scope is mandatory for        |\n\
             |    authentication. These keys will be used to authenticate     |\n\
             |    headers and any other partition that does not have a        |\n\
             |    specific SPK/SSK.                                           |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF 1 - test.bif                                        |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |  [pskfile] primarykey.pem                                      |\n\
             |  [sskfile] secondarykey.pem                                    |\n\
             |  [bootloader, authentication=rsa] fsbl.elf                     |\n\
             |  [authentication=rsa] hello.elf                                |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Sample BIF 2 - test.bif                                        |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |   [pskfile] primary.pem                                        |\n\
             |   [sskfile] secondary0.pem                                     |\n\
             |                                                                |\n\
             |  /* FSBL (Partition-0) */                                      |\n\
             |   [                                                            |\n\
             |     bootloader,                                                |\n\
             |     destination_cpu = a53-0,                                   |\n\
             |     authentication = rsa,                                      |\n\
             |     sskfile = secondary1.pem                                   |\n\
             |   ]fsbla53.elf                                                 |\n\
             |                                                                |\n\
             |  /* Partition-1 */                                             |\n\
             |   [                                                            |\n\
             |     destination_cpu = a53-0,                                   |\n\
             |     authentication = rsa                                       |\n\
             |   ] hello.elf                                                  |\n\
             |  }                                                             |\n\
             |----------------------------------------------------------------|\n\
             | Sample key files:                                              |\n\
             | 1. OpenSSL Key Format: (.pem/.pub)                             |\n\
             |	  -----BEGIN RSA PRIVATE KEY-----                             |\n\
             |      MIIEpAIBAAKCAQEAxrmlJyPcZVXltASHrtm/YnOMxskf0k2RZrIajqy   |\n\
             |      kyBMalXaqmGb1kqGCgGZVvQt3FSRO3yXa....                     |\n\
             |    -----END RSA PRIVATE KEY-----                               |\n\
             |                                                                |\n\
             |    -----BEGIN PUBLIC KEY-----                                  |\n\
             |      MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxrmlJyPcZVm   |\n\
             |      YnOMxskf0k2RZrIajq....                                    |\n\
             |    -----END RSA PRIVATE KEY-----                               |\n\
             |                                                                |\n\
             | 2. Xilinx RSA Format:                                          |\n\
             |      N = c6b9a522354238902352302acfb3.......a21678123412       |\n\
             |      E = 10001                                                 |\n\
             |      D = 37c80c8c6b9a522354238f4a6cdf.......c6b9a5223542       |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 udf_bh
******************************************************************************/
#define H_BIF_UDFBH_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | udf_bh                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Imports a file of data to be copied to the User Defined Field  |\n\
             | of the Boot Header. The input user defined data is provided    |\n\
             | through a text file in the form of a hex string.               |\n\
             | Total no. of bytes in UDF in Xilinx SoCs:                      |\n\
             |    Zynq   : 76 bytes                                           |\n\
             |    ZynqMP : 40 bytes                                           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [udf_bh] <filename>                                            |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [udf_bh] test.txt                                           |\n\
             |    [bootloader] fsbl.elf                                       |\n\
             |    hello.elf                                                   |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Sample input file for udf_bh - test.txt                        |\n\
             | 123456789abcdef85072696e636530300301440408706d616c6c6164000508 |\n\
             | 266431530102030405060708090a0b0c0d0e0f101112131415161718191a1b |\n\
             | 1c1d1                                                          |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 spksignature
******************************************************************************/
#define H_BIF_SPKSIGN_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | spksignature                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Imports SPK signature into authentication certificate. This can|\n\
             | be used incase the user does't want to share the secret key PSK|\n\
             | The user can create a signature and provide it to Bootgen.     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [spksignature] <signature-file>                                |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [ppkfile] ppk.txt                                          |\n\
             |     [spkfile] spk.txt                                          |\n\
             |     [spksignature] spk.txt.sha256.sig                          |\n\
             |     [bootloader, authentication=rsa] fsbl.elf                  |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 headersignature
******************************************************************************/
#define H_BIF_HDRSIGN_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | headersignature                                                |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Imports Header signature into authentication certificate. This |\n\
             | can be used incase the user does't want to share the secret key|\n\
             | The user can create a signature and provide it to Bootgen.     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [headersignature] <signature-file>                             |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [ppkfile] ppk.txt                                          |\n\
             |     [spkfile] spk.txt                                          |\n\
             |     [headersignature] headers.sha256.sig                       |\n\
             |     [spksignature] spk.txt.sha256.sig                          |\n\
             |     [bootloader, authentication=rsa] fsbl.elf                  |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 bootimage
******************************************************************************/
#define H_BIF_BI_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | bootimage                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This specifies that the following file specification is a      |\n\
             | bootimage that was created by Bootgen, being reused as input.  |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootimage] <image created by bootgen>                         |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootimage] fsbl.bin                                       |\n\
             |     [bootimage] system.bin                                     |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | In the above example, the fsbl.bin & system.bin are images     |\n\
             | generated using bootgen.                                       |\n\
             |                                                                |\n\
             | Example fsbl.bin generation                                    |\n\
             |  image:                                                        |\n\
             |  {                                                             |\n\
             |    [keysrc_encryption] bbram_red_key                           |\n\
             |    [pskfile] primary.pem                                       |\n\
             |    [sskfile] secondary.pem                                     |\n\
             |    [bootloader, authentication=rsa, encryption=aes,            |\n\
             |                   aeskeyfile=aes.nky] fsbl.elf                 |\n\
             |  }                                                             |\n\
             |  Command: bootgen -image fsbl.bif -o fsbl.bin -encrypt efuse   |\n\
             |                                                                |\n\
             | Example system.bin generation                                  |\n\
             |  image:                                                        |\n\
             |  {                                                             |\n\
             |     [pskfile] primary.pem                                      |\n\
             |     [sskfile] secondary.pem                                    |\n\
             |     [bootloader, authentication=rsa] system.bit                |\n\
             |  }                                                             |\n\
             |  Command: bootgen -image system.bif -o system.bin              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 bootloader
******************************************************************************/
#define H_BIF_BL_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | bootloader                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This specifies the partition is a bootloader (FSBL). This      |\n\
             | attribute is specified with along with other partition bif     |\n\
             | attributes.                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootloader] <partition>                                       |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [bootloader] fsbl.elf                                       |\n\
             |    hello.elf                                                   |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 encryption
******************************************************************************/
#define H_BIF_ENCR_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | encryption                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This specifies the partition needs to be encrypted.            |\n\
             | Encryption Algorithms                                          |\n\
             |     Zynq   : AES-CBC                                           |\n\
             |     ZynqMP : AES-GCM                                           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [encryption = <options>] <partition>                           |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | *none : Partition not encrypted                                |\n\
             |  aes  : Partition encrypted using AES algorithm                |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [aeskeyfile] test.nky                                     |\n\
             |      [bootloader, encryption=aes] fsbl.elf                     |\n\
             |      [encryption=aes] hello.elf                                |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 pid
******************************************************************************/
#define H_BIF_PID_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | *** Only for ZynqMP Architecture ***                           |\n\
             | pid                                                            |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This is used to specify an id to which the partition is        |\n\
             | associated with.                                               |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [pid = <id>] <partition>                                       |\n\
             | <id> is an integer value, representing the partition number.   |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [keysrc_encryption] bbram_red_key                         |\n\
             |      [encryption=aes,aeskeyfile=test.nky,pid=1] hello.elf      |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | While creating an image bootgen by default assigns an id to    |\n\
             | every partition, which is in line with the order of partitions |\n\
             | given in the bif.                                              |\n\
             | To assign a different id to any partition, pid can be used.    |\n\
             | During encryption, the IV is incremented by this value to avoid|\n\
             | security vulneribilities.                                      |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 authentication
******************************************************************************/
#define H_BIF_AUTH_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | authentication                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This specifies the partition needs to be authenticated.        |\n\
             | Authentication Algorithms:                                     |\n\
             |     Zynq   : RSA-2048                                          |\n\
             |     ZynqMP : RSA-4096                                          |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [authenication = <options>] <partition>                        |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | *none : Partition not authenticated                            |\n\
             |  rsa  : Partition authenticated using RSA algorithm            |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [ppkfile] ppk.txt                                          |\n\
             |     [spkfile] spk.txt                                          |\n\
             |     [bootloader, authentication=rsa] fsbl.elf                  |\n\
             |     [authentication=rsa] hello.elf                             |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 checksum
******************************************************************************/
#define H_BIF_CHKSM_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | checksum                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This specifies the partition needs to be checksummed.          |\n\
             | Checksum Algorithms:                                           |\n\
             |     Zynq   : MD5                                               |\n\
             |     ZynqMP : SHA3                                              |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [checksum = <options>] <partition>                             |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | *none : no checksum operation                                  |\n\
             |  md5  : MD5 checksum operation (Zynq)                          |\n\
             |  sha3 : SHA3 checksum operation (ZynqMP)                       |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader] fsbl.elf                                      |\n\
             |     [checksum=md5] hello.elf                                   |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Note:                                                          |\n\
             | 1. In Zynq, checksum opertations is not supported for          |\n\
             |    bootloaders.                                                |\n\
             | 2. In ZynqMP, checksum operation is not supported along with   |\n\
             |    more secure features like authentication and encryption.    |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 partition_owner
******************************************************************************/
#define H_BIF_POWNER_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | partition_owner                                                |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Owner of the partition which is responsible to load the        |\n\
             | partition.                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [partition_owner = <options>] <partition>                      |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | *fsbl  : FSBL loads this partition                             |\n\
             |  uboot : u-boot loads this partition                           |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader] fsbl.elf                                      |\n\
             |     uboot.elf                                                  |\n\
             |     [partition_owner=uboot] hello.elf                          |\n\
             | }                                                              |\n\
             |                                                                |\n\
             | uboot.elf will be loaded by FSBL & hello.elf will be loaded by |\n\
             | u-boot                                                         |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 presign
******************************************************************************/
#define H_BIF_PRESIGN_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | presign                                                        |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Imports partition signature into partition authenticate        |\n\
             | certificate.                                                   |\n\
             | This can be used incase the user does't want to share the      |\n\
             | secret key(SSK). The user can create a signature and provide   |\n\
             | it to Bootgen.                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [presign = <signature-file>] <partition>                       |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [ppkfile] ppk.txt                                           |\n\
             |    [spkfile] spk.txt                                           |\n\
             |    [headersignature] headers.sha256.sig                        |\n\
             |    [spksignature] spk.txt.sha256.sig                           |\n\
             |    [bootloader, authentication=rsa, presign=fsbl.sig] fsbl.elf |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 udf_data
******************************************************************************/
#define H_BIF_UDF_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | udf_data                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Imports a file containing up to 56 bytes of data into User     |\n\
             | Defined Field of Authentication Certificate.                   |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | udf_data = <filename>                                          |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [pskfile]primary0.pem                                      |\n\
             |     [sskfile]secondary0.pem                                    |\n\
             |     [bootloader,destination_cpu=a53-0,authentication=rsa,      |\n\
             |                                    udf_data=udf.txt]fsbl.elf   |\n\
             |     [destination_cpu=a53-0, authentication=rsa] hello.elf      |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 xip_mode
******************************************************************************/
#define H_BIF_XIP_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | xip_mode                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Indicates 'eXecute In Place' for FSBL to be executed directly  |\n\
             | from QSPI flash.                                               |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [xip_mode] <partition>                                         |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader, xip_mode] fsbl.elf                            |\n\
             |     application.elf                                            |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Note: This attribute is only applicable for FSBL partition.    |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 alignment
******************************************************************************/
#define H_BIF_ALIGN_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | alignment                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Sets the byte alignment. The partition will be padded to be    |\n\
             | aligned to a multiple of this value.                           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [alignment=<value>] <partition>                                |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |                                                                |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Note: Cannot be used with offset.                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 offset
******************************************************************************/
#define H_BIF_OFFSET_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | offset                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Sets the absolute offset of the partition in the boot image.   |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [offset=<value>] <partition>                                   |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [bootloader] fsbl.elf                                     |\n\
             |      u-boot.elf                                                |\n\
             |      [load=0x3000000, offset=0x500000] uImage.bin              |\n\
             |      [load=0x2A00000, offset=0xa00000] devicetree.dtb          |\n\
             |      [load=0x2000000, offset=0xc00000] uramdisk.image.gz       |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 reserve
******************************************************************************/
#define H_BIF_RES_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | reserve                                                        |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Reserves the memory and padded after the partition.            |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [reserve=<value>] <partition>                                  |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [bootloader] fsbl.elf                                     |\n\
             |      [reserve=0x1000] test.bin                                 |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 load
******************************************************************************/
#define H_BIF_LOAD_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | load                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Sets the load address for the partition in memory.             |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [load=<value>] <partition>                                     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [bootloader] fsbl.elf                                     |\n\
             |      u-boot.elf                                                |\n\
             |      [load=0x3000000, offset=0x500000] uImage.bin              |\n\
             |      [load=0x2A00000, offset=0xa00000] devicetree.dtb          |\n\
             |      [load=0x2000000, offset=0xc00000] uramdisk.image.gz       |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 startup
******************************************************************************/
#define H_BIF_STARTUP_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | startup                                                        |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Sets the entry address for the partition, after it is loaded.  |\n\
             | This is ignored for partitions that do not execute.            |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [startup=<value>] <partition>                                  |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [bootloader] fsbl.elf                                     |\n\
             |      [startup=0x1000000] app.elf                               |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 keysrc_encryption
******************************************************************************/
#define H_BIF_KEYSRC_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | keysrc_encryption                                              |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Key source for encryption                                      |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [keysrc_encrption] <options>                                   |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | efuse_red_key  : Red Key stored in efuse                       |\n\
             | bbram_red_key  : Red Key stored in bbram                       |\n\
             | efuse_gry_key  : Grey (Obfuscated) Key stored in efuse         |\n\
             | bh_gry_key     : Grey (Obfuscated) Key stored in boot header   |\n\
             | bh_blk_key     : Black Key stored in boot header               |\n\
             | efuse_blk_key  : Black Key stored in efuse                     |\n\
             | kup_key        : User Key                                      |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [keysrc_encryption] bbram_red_key                           |\n\
             |    [bootloader, encryption=aes, aeskeyfile=encr.nky            |\n\
             |                         destination_cpu=a53-0]fsbl.elf         |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | FSBL is encrypted using the key encr.nky, which is stored in   |\n\
             | bbram in the device for decryption purpose.                    |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 fsbl_config
******************************************************************************/
#define H_BIF_FSBLCFG_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | fsbl_config                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | These parameters are used to configure the bootimage           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [fsbl_config] <options>                                        |\n\
-------------+---------------------+------------------------------------------+\n\
 OPTIONS     |       Options       |             Description                  |\n\
             +---------------------+------------------------------------------+\n\
             | bh_auth_enable      | Boot Header Authentication Enable:       |\n\
             |                     | RSA authentication of the bootimage will |\n\
             |                     | be done excluding the verification of    |\n\
             |                     | PPK hash and SPK ID.                     |\n\
             +---------------------+------------------------------------------+\n\
             | auth_only           | Boot image is only RSA signed.           |\n\
             |                     | FSBL should not be decrypted.            |\n\
             +---------------------+------------------------------------------+\n\
             | opt_key             | Optional key is used for block-0         |\n\
             |                     | decryption. Secure Header has the opt key|\n\
             +---------------------+------------------------------------------+\n\
             | pufhd_bh            | PUF helper data is stored in BH.         |\n\
             |                     | (Default is efuse)                       |\n\
             |                     | PUF helper data file is passed to bootgen|\n\
             |                     | using the option [puf_file].             |\n\
             +---------------------+------------------------------------------+\n\
             | puf4kmode           | PUF is tuned to use in 4k bit syndrome   |\n\
             |                     | configuration.                           |\n\
             |                     | (Default is 12k bit)                     |\n\
             +---------------------+------------------------------------------+\n\
             | shutter = <value>   | 32 bit PUF_SHUT register value to        |\n\
             |                     | configure PUF for shutter offset time    |\n\
             |                     | and shutter open time.                   |\n\
-------------+---------------------+------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             | [fsbl_config] bh_auth_enable                                   |\n\
             | [pskfile] primary.pem                                          |\n\
             | [sskfile] secondary.pem                                        |\n\
             | [bootloader,destination_cpu=a53-0,authentication=rsa] fsbl.elf |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | FSBL which should run on A53 in 64-bit mode, is authenticated  |\n\
             | using RSA with SHA3 hashing, in Boot Header authentication mode|\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 boot_device
******************************************************************************/
#define H_BIF_BOOTDEV_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | [boot_device]                                                  |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | Specifies the secondary boot device. Indicates the device on   |\n\
             | which the partition is present.                                |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [boot_device] <options>                                        |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | qspi32                                                         |\n\
             | qspi24                                                         |\n\
             | nand                                                           |\n\
             | sd0                                                            |\n\
             | sd1                                                            |\n\
             | sd-ls                                                          |\n\
             | mmc                                                            |\n\
             | usb                                                            |\n\
             | ethernet                                                       |\n\
             | pcie                                                           |\n\
             | sata                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [boot_device] sd0                                          |\n\
             |     [bootloader,destination_cpu=a53-0] fsbl.elf                |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"


/******************************************************************************
 destination_cpu
******************************************************************************/
#define H_BIF_DESTCPU_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | destination_cpu                                                |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | Specifies which core will execute the partition                |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [destination_cpu=<options>] <partition>                        |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | * a53-0                                                        |\n\
             |   a53-1                                                        |\n\
             |   a53-2                                                        |\n\
             |   a53-3                                                        |\n\
             |   r5-0                                                         |\n\
             |   r5-1                                                         |\n\
             |   r5-lockstep                                                  |\n\
             |   pmu                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader, destination_cpu=a53-0] fsbl.elf               |\n\
             |     [destination_cpu=r5-0] app.elf                             |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | This specifies that FSBL will be excuted on A53-0 core and     |\n\
             | application on R5-0 core.                                      |\n\
             |----------------------------------------------------------------|\n\
             | Note: FSBL can run either on A53-0 or R5-0 only.               |\n\
             |----------------------------------------------------------------|\n\
             | Note:                                                          |\n\
             | * PMU loaded by FSBL: [destination_cpu=pmu] pmu.elf            |\n\
             | In this flow, BootROM loads FSBL first, and then FSBL loads the|\n\
             | PMU fw.                                                        |\n\
             | * PMU loaded by BootROM: [pmufw_image] pmu.elf                 |\n\
             | In this flow, BootROM loads PMU first and then the FSBL        |\n\
             | So PMU does the power management tasks, before the FSBL comes  |\n\
             | up.                                                            |\n\
-------------+----------------------------------------------------------------+\n"    

/******************************************************************************
 destination_device
******************************************************************************/
#define H_BIF_DESTDEV_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | destination_device                                             |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | Specifies the destination of the partition                     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [destination_device=<options>] <partition>                     |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | * ps   : The partition is targetted for PS (default)           |\n\
             |   pl   : The partition is targetted for PL, for bitstreams     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader, destination_cpu=a53-0] fsbl.elf               |\n\
             |     [destination_device=pl] system.bit                         |\n\
             |     [destination_cpu=r5-1] app.elf                             |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 exception_level
******************************************************************************/
#define H_BIF_EL_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | exception_level                                                |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | Exception level for which the core should be configured        |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [exception_level=<options>] <partition>                        |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | el-0                                                           |\n\
             | el-1                                                           |\n\
             | el-2                                                           |\n\
             | el-3                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader, destination_cpu=a53-0] fsbl.elf               |\n\
             |     [destination_cpu=a53-0, exception_level=el-3] bl31.elf     |\n\
             |     [destination_cpu=a53-0, exception_level=el-2] u-boot.elf   |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 trustzone
******************************************************************************/
#define H_BIF_TZ_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | trustzone                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | Configures the core to be Trustzone secure or nonsecure        |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [trustzone = <options> ] <partition>                           |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     |  secure                                                        |\n\
             |  nonsecure                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader, destination_cpu=a53-0] fsbl.elf               |\n\
             |     [exception_level=el-3, trustzone = secure] bl31.elf        |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 auth_params
******************************************************************************/
#define H_BIF_AUTHPARAM_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | auth_params                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | Extra options for authentication                               |\n\
             | ppk_select   - 2 PPKs supported.                               |\n\
             | spk_select   - To differentiate spk and user efuses.           |\n\
             |                Default will be spk-efuse.                      |\n\
             | spk_id       - ID to differentiate SPKs.                       |\n\
             | header_auth  - To authenticate headers when no partition is    |\n\
             |                authenticated.                                  |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [auth_params] header_auth; ppk_select=<0/1>; spk_id=<value>;   |\n\
             |                      spk_select=<spk-efuse/user-efuse>         |\n\
             |----------------------------------------------------------------|\n\
             | Notes:                                                         |\n\
             |  - ppk_select is unique for the image.                         |\n\
             |  - Each partition can have its own spk_select and spk_id.      |\n\
             |    spk-efuse id is unique accross the image, but user-efuse id |\n\
             |    can vary between partitions.                                |\n\
             |  - spk_select/spk_id outside the partition scope will be used  |\n\
             |    for headers and any other partition that does not have      |\n\
             |    these specifications as partition attributes.               |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF 1 - test.bif                                        |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |   [auth_params] ppk_select=0;spk_select=user-efuse;spk_id=0x22 |\n\
             |   [pskfile]     primary.pem                                    |\n\
             |   [sskfile]     secondary.pem                                  |\n\
             |   [                                                            |\n\
             |     bootloader,                                                |\n\
             |     authentication = rsa                                       |\n\
             |   ] fsbl.elf                                                   |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Sample BIF 2 - test.bif                                        |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |   [auth_params] ppk_select=1; spk_select= user-efuse;          |\n\
             |                                spk_id=0x22; header_auth        |\n\
             |   [pskfile] primary.pem                                        |\n\
             |   [sskfile] secondary.pem                                      |\n\
             |   [destination_cpu=a53-0] test.elf                             |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |   [auth_params]  ppk_select=1;spk_select=user-efuse;spk_id=0x22|\n\
             |   [pskfile] primary.pem                                        |\n\
             |   [sskfile] secondary0.pem                                     |\n\
             |                                                                |\n\
             |  /* FSBL - Partition-0) */                                     |\n\
             |   [                                                            |\n\
             |     bootloader,                                                |\n\
             |     destination_cpu = a53-0,                                   |\n\
             |     authentication = rsa,                                      |\n\
             |     spk_id = 0x12345678,                                       |\n\
             |     spk_select = spk-efuse,                                    |\n\
             |     sskfile = secondary1.pem                                   |\n\
             |   ] fsbla53.elf                                                |\n\
             |                                                                |\n\
             |  /* Partition-1 */                                             |\n\
             |   [                                                            |\n\
             |     destination_cpu = a53-1,                                   |\n\
             |     authentication = rsa,                                      |\n\
             |     spk_id = 0x24,                                             |\n\
             |     spk_select = user-efuse,                                   |\n\
             |     sskfile = secondary2.pem                                   |\n\
             |   ] hello.elf                                                  |\n\
             |  }                                                             |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 bh_keyfile
******************************************************************************/
#define H_BIF_BHKEY_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | bh_keyfile                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | 256-bit obfuscated key to be stored in boot header             |\n\
             | This is only valid, when [keysrc_encryption] bh_gry_key        |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bh_keyfile] <key file path>                                   |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [keysrc_encryption] bh_gry_key                              |\n\
             |    [bh_keyfile] obfuscated_key.txt                             |\n\
             |    [bh_key_iv] obfuscated_iv.txt                               |\n\
             |    [bootloader, encryption=aes, aeskeyfile = encr.nky,         |\n\
             |                          destination_cpu=a53-0]fsbl.elf        |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 bh_key_iv
******************************************************************************/
#define H_BIF_BHIV_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | bh_key_iv                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | Initialization vector used when decrypting the obfuscated key  |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bh_key_iv] <iv file path>                                     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [keysrc_encryption] bh_gry_key                              |\n\
             |    [bh_keyfile] obfuscated_key.txt                             |\n\
             |    [bh_key_iv] obfuscated_iv.txt                               |\n\
             |    [bootloader, encryption=aes, aeskeyfile=encr.nky,           |\n\
             |                             destination_cpu=a53-0]fsbl.elf     |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 familykey
******************************************************************************/
#define H_BIF_METAL_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | familykey                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | Specify Family Key                                             |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [familykey] <key file path>                                    |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [aeskeyfile] encr.nky                                       |\n\
             |    [bh_key_iv] bh_iv.txt                                       |\n\
             |    [familykey] familykey.txt                                   |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 pmufw_image
******************************************************************************/
#define H_BIF_PFW_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | pmufw_image                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | PMU Firmware image to be loaded by BootROM, before loading the |\n\
             | FSBL.                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [pmufw_image] <PMU elf file>                                   |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [pmufw_image] pfw.elf                                      |\n\
             |     [bootloader, destination_cpu=a53-0] fsbl.elf               |\n\
             |     [destination_cpu=r5-1] app.elf                             |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Note:                                                          |\n\
             |----------------------------------------------------------------|\n\
             | PMU loaded by BootROM: [pmufw_image] pmu.elf                   |\n\
             | In this flow, BootROM loads PMU first and then the FSBL        |\n\
             | So PMU does the power management tasks, before the FSBL comes  |\n\
             | up.                                                            |\n\
             |----------------------------------------------------------------|\n\
             | PMU loaded by FSBL: [destination_cpu=pmu] pmu.elf              |\n\
             | In this flow, BootROM loads FSBL first, and then FSBL loads the|\n\
             | PMU fw.                                                        |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 blocks
******************************************************************************/
#define H_BIF_BLOCKS_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | blocks                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | Specify block sizes for key-rolling feature in encrytion.      |\n\
             | Each module is encrypted using its own unique key. The initial |\n\
             | key is stored at the key source on the device, while keys for  |\n\
             | each successive module are encrypted (wrapped) in the previous |\n\
             | module.                                                        |\n\
             | <size> mentioned will be taken in Bytes. If X(*) is specified, |\n\
             | then all the remaining blocks would of the size 'X'.           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [blocks = <size><num>;<size><num>;...;<size><*>] <partition>   |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [keysrc_encryption] bbram_red_key                           |\n\
             |    [bootloader, encryption=aes, destination_cpu=a53-0,         |\n\
             |                 blocks=4096(2);1024;2048(2);4096(*),           |\n\
             |                        aeskeyfile=encr.nky] fsbl.elf           |\n\
             | }                                                              |\n\
             | In the above example, the first two blocks are of 4096 bytes,  |\n\
             | second block is of 1024 bytes, the next two blocks are of      |\n\
             | 2048 bytes, the rest of the blocks are of 4096 bytes.          |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 early_handoff
******************************************************************************/
#define H_BIF_ELYHNDOFF_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | early_handoff                                                  |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This flag will ensure, the handoff to applications which are   |\n\
             | critical, immediatley after partition is loaded. Otherwise all |\n\
             | the partitions are loaded sequentially and handoff also happens|\n\
             | in sequenctial fashion                                         |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [early_handoff] <partition>                                    |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader, destination_cpu=a53_0] fsbl.elf               |\n\
             |     [destination_cpu=r5-0] app1.elf                            |\n\
             |     [destination_cpu=r5-1, early_handoff] app2.elf             |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Note: In the above scenario, the FSBL loads app1, then app2    |\n\
             | and immediately handsoff the control to app2 before app1.      |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 authblocks
******************************************************************************/
#define H_BIF_AUTHBLOCKS_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | authblocks                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture Bit Stream ***                |\n\
             | Specify block size for bit stream in authentication.           |\n\
             | The bitstream is authenticated in chunks of given size,default |\n\
             | will be 8MB. For any other partion, this is invalid.           |\n\
             | Supported sizes are 4,8,16,32,64. [All values are taken in MB] |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [authblocks = <size>] <partition>                              |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [ppkfile] ppk.txt                                          |\n\
             |     [spkfile] spk.txt                                          |\n\
             |     [bootloader, authentication=rsa] fsbl.elf                  |\n\
             |     [authentication=rsa, authblocks=4] bs.bit                  |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 hivec
******************************************************************************/
#define H_BIF_HIVEC_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | hivec                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | To specify the location of Exception Vector Table as hivec.    |\n\
             | Default is taken as lovec.                                     |\n\
             | This is applicable with a53(32 bit) and r5 cores only.         |\n\
             | hivec: exception vector table at 0xFFFF0000.                   |\n\
             | lovec: exception vector table at 0x00000000.                   |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [hivec] <partition>                                            |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader, destination_cpu=a53_0] fsbl.elf               |\n\
             |     [destination_cpu=r5-0,hivec] app1.elf                      |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 bhsignature
******************************************************************************/
#define H_BIF_BHSIGN_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | bhsignature                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | Imports Boot Header signature into authentication certificate. |\n\
             | This can be used incase the user does't want to share the      |\n\
             | secret key PSK.                                                |\n\
             | The user can create a signature and provide it to Bootgen.     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bhsignature] <signature-file>                                 |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [ppkfile] ppk.txt                                          |\n\
             |     [spkfile] spk.txt                                          |\n\
             |     [spksignature] spk.txt.sha384.sig                          |\n\
             |     [bhsignature] bootheader.sha384.sig                        |\n\
             |     [bootloader, authentication=rsa] fsbl.elf                  |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 bootvectors
******************************************************************************/
#define H_BIF_BOOTVEC_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | [bootvectors]                                                  |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specifies the vector table for XIP                             |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootvectors] <values>                                         |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootvectors] 0x14000000,0x14000000,0x14000000,0x14000000, |\n\
             |             0x14000000,0x14000000,0x14000000,0x14000000        |\n\
             |     [bootloader,destination_cpu=a53-0] fsbl.elf                |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | By default, bootgen populates XIP elf vector table below -     |\n\
             |     - 0xEAFFFFFE: for Cortex R5 and Cortex A53 (32-bit)        |\n\
             |     - 0x14000000: for Cortex A53 (64-bit)                      |\n\
             | To specify a different vector table, bootvectors can be used.  |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 split
******************************************************************************/
#define H_BIF_SPLIT_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | split                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Splits the image into parts based on mode.                     |\n\
             | Slaveboot mode splits as below.                                |\n\
             |        1. Boot Header + Bootloader                             |\n\
             |        2. Image and Partition Headers                          |\n\
             |        3. Rest of the partitions.                              |\n\
             | Normal mode splits as below.                                   |\n\
             |        1. Bootheader + Image Headers +                         |\n\
             |            Partition Headers + Bootloader                      |\n\
             |        2. Partiton1                                            |\n\
             |        3. Partition2 ...                                       |\n\
             | Slaveboot is supported only for ZynqMP, normal is suppoted     |\n\
             | for both Zynq and ZynqMP.                                      |\n\
             | Along with the split mode, output format can also be           |\n\
             | specified as bin or mcs.                                       |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [split] mode = <options>                                       |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | slaveboot                                                      |\n\
             | normal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [split] mode=slaveboot, fmt=bin                            |\n\
             |     [bootloader, destination_cpu=a53-0] fsbl.elf               |\n\
             |     [destination_device=pl] system.bit                         |\n\
             |     [destination_cpu=r5-1] app.elf                             |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Note: The option split mode normal is same as the command line |\n\
             | option split. This command line option will be deprecated soon.|\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 puf_file
******************************************************************************/
#define H_BIF_PUFDATA_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | puf_file                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | *** Only for ZynqMP Architecture ***                           |\n\
             | PUF helper data file.                                          |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [puf_file] <puf data file>                                     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [fsbl_config] pufhd_bh                                      |\n\
             |    [puf_file] pufhelperdata.txt                                |\n\
             |    [bh_keyfile] black_key.txt                                  |\n\
             |    [bh_key_iv] bhkeyiv.txt                                     |\n\
             |    [bootloader,destination_cpu=a53-0,encryption=aes]fsbl.elf   |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | PUF is used with black key as encryption key source.           |\n\
             | PUF helper data is of 1544 bytes.                              |\n\
             | 1536 bytes of PUF HD + 4 bytes of CHASH                        |\n\
             |                        + 3 bytes of AUX + 1 byte alignment     |\n\
-------------+----------------------------------------------------------------+\n"
#endif
