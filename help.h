/******************************************************************************
* Copyright 2015-2021 Xilinx, Inc.
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
             |  versal : Image is targeted for VERSAL ACAP architecture       |\n\
             |  fpga   : Image is targeted for other FPGA architectures       |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | bootgen -arch zynq -image test.bif -o boot.bin                 |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | boot.bin generated is a Zynq boot image                        |\n\
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
 ARGUMENTS   | Any of the bif attributes                                      |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | 1. bootgen -bif_help                                           |\n\
             | 2. bootgen -bif_help aeskeyfile                                |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Usage 1: All the bif file attributes are listed                |\n\
             | Usage 2: Detailed explanation of the attribute aeskeyfile with |\n\
             |          example                                               |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
dual_ospi_mode
******************************************************************************/
#define DOSPIHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | dual_ospi_mode                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option generates output files as per the configuration    |\n\
             | specified by the Dual OSPI mode                                |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -dual_ospi_mode [arguments]                                    |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | stacked <size> :                                               |\n\
             |     The OSPI configuration is Dual Stacked.                    |\n\
             |     Generates 2 output files, in sequential fashion.           |\n\
             |     <size> - Size of flash in MB (16/32/64/128)                |\n\
             | Both the flashes can then be programmed independently.         |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | bootgen -image test.bif -o -boot.bin                           |\n\
             |                      -dual_ospi_mode stacked 64                |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Suppose the output bootimage content is around 120MB           |\n\
             | Two files are generated boot_1.bin & boot_2.bin of             |\n\
             |          64MB & 56MB                                           |\n\
             |----------------------------------------------------------------|\n\
             | Suppose the output bootimage content is around 50MB            |\n\
             | Only one file is generated boot_1.bin of 50MB                  |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
dual_qspi_mode
******************************************************************************/
#define DQSPIHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | dual_qspi_mode                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
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
 USAGE       | 1. bootgen -image test.bif -o -boot.bin                        |\n\
             |                      -dual_qspi_mode parallel                  |\n\
             |----------------------------------------------------------------|\n\
             | 2. bootgen -image test.bif -o -boot.bin                        |\n\
             |                      -dual_qspi_mode stacked 64                |\n\
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
 dump
******************************************************************************/
#define DUMPHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | dump                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option is used to dump partitions, bootheader and SSIT    |\n\
             | slave PDIs as binary files.                                    |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -dump <filename> [arguments]                                   |\n\
             | The files are dumped from the PDI mentioned with <filename>    |\n\
------------------------------------------------------------------------------|\n\
 ARGUMENTS   | boot_files : Dumps all boot files(Boot header, PLM and PMC CDO)|\n\
             | plm        : Dumps the plm partition                           |\n\
             | pmc_cdo    : Dumps pmc_cdo partition                           |\n\
             |                                                                |\n\
             | Note : When no arguments are specifed, all the partitions are  |\n\
             |        dumped.                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -dump [arguments]                                              |\n\
             | This must used while generating the PDI, and the files are      |\n\
             | dumped along with the PDI                                      |\n\
------------------------------------------------------------------------------|\n\
 ARGUMENTS   | bh         : Dumps the Boot Header                             |\n\
             | slave_pdis : Dumps slave PDIs for SSIT devices                 |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | 1. bootgen -arch versal -dump test.pdi                         |\n\
             | 2. bootgen -arch versal -dump test.pdi boot_files              |\n\
             | 3. bootgen -arch versal -image test.bif -w on -o               |\n\
             |                          test.pdi -dump slave_pdis             |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | 1. The partitions in the PDI are dumped as binary files.       |\n\
             | 2. Dumps all boot files - Boot header, PLM and PMC CDO, as     |\n\
             |    binary files.                                               |\n\
             | 3. Slave PDIs for SSIT devices are dumped as binary files.     |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
dump_dir
******************************************************************************/
#define DUMPDIRHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | dump_dir                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option is used with dump option to dump partitions in the |\n\
             | directory specified here.                                      |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -dump_dir <path>                                               |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | bootgen -arch versal -dump <pdi> -dump_dir <path>              |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | The partitions in the PDI are dumped as binary files in the    |\n\
             | dump directory.                                                |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
efuseppkbits
******************************************************************************/
#define EFUSEPPKBITSHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | efuseppkbits                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option specifies the name of the efuse file to be written |\n\
             | to contain the PPK hash. This option generates a direct hash   |\n\
             | without any padding.                                           |\n\
             | Zynq - Hashing done by SHA2                                    |\n\
             | ZynqMP & Versal - Hashing done by SHA3                         |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -efuseppkbits <filename>                                       |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       |  bootgen -image test.bif -o boot.bin                           |\n\
             |          -efuseppkbits efusefile.txt                           |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | efusefile.txt file gets generated, which contains the hash of  |\n\
             | the PPK key.                                                   |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
encrypt
******************************************************************************/
#define ENCRYPTHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | encrypt                                                        |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, fpga                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option specifies how to do encryption, where the keys are |\n\
             | stored.                                                        |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -encrypt [key source args] <filename>                          |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | Key source arguments:                                          |\n\
             |   efuse : The AES key is stored in EFUSE                       |\n\
             |   bbram : The AES key is stored in BBRAM                       |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       |  bootgen -image test.bif -o boot.bin -encrypt efuse            |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | The .nky key file is passed through BIF file attribute         |\n\
             | [aeskeyfile]. Only source is specified through command line.   |\n\
-------------+----------------------------------------------------------------+\n\
 NOTE        | For other devices, key source is specified using BIF attribute |\n\
             | keysrc_encryption or keysrc                                    |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
encryption_dump
******************************************************************************/
#define ENCRDUMPHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | encryption_dump                                                |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp, versal                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Generates encryption log file, aes_log.txt                     |\n\
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
             | aes_log.txt generated has the details of AES Key/IV pairs used |\n\
             | for encrypting each block of data. It also logs the partition  |\n\
             | and the aes key file used to encrypt it.                       |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
fill
******************************************************************************/
#define FILLHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | fill                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
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
generate_hashes
******************************************************************************/
#define GENERATEHASHESHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | generate_hashes                                                |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option generates file containing padded hash              |\n\
             | Zynq   - SHA-2 with PKCS#1v1.5 padding scheme                  |\n\
             | ZynqMP - SHA-3 with PKCS#1v1.5 padding scheme                  |\n\
             | Versal - SHA-3 with PSS padding scheme                         |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -generate_hashes                                               |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | bootgen -image test.bif -generate_hashes                       |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | To generate a SPK hash                                         |\n\
             | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [spkfile]secondary.pub                                      |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"
/******************************************************************************
generate_keys
******************************************************************************/
#define GENKEYSHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | generate_keys                                                  |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
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
             |   ecdsa-p384 : ecdsap384 keys                                  |\n\
             |   ecdsa-p521 : ecdsap521 keys                                  |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | For authentication :                                           |\n\
             |         bootgen -image test.bif -generate_keys rsa             |\n\
             |         bootgen -image test.bif -generate_keys ecdsa-p384      |\n\
             |         bootgen -image test.bif -generate_keys ecdsa-p521      |\n\
             | For encryption :                                               |\n\
             |          bootgen -image test.bif -generate_keys obfuscatedkey  |\n\
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
             |     [aeskeyfile] aes.nky                                       |\n\
             |     [bh_key_iv] bhkeyiv.txt                                    |\n\
             |     [familykey] familykey.txt                                  |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n\
 NOTES       | PEM Key Format:                                                |\n\
             |   -----BEGIN RSA PRIVATE KEY-----                              |\n\
             |   MIIEpAIBAAKCAQEAmlJyPcZVXltASHrtm/YnOMxskf0k2RZrIajqymqZptnG |\n\
             |   kyBMalXaqmGb1kqGCgGZVvQt3FSRO3yXa....                        |\n\
             |   -----END RSA PRIVATE KEY-----                                |\n\
             |                                                                |\n\
             |   -----BEGIN EC PRIVATE KEY-----                               |\n\
             |  MIGkAgEBBDAqm8l04lfuEOFZI988uJ8b5UcP45hFIDk/OmsXItT3vk0SJM/   |\n\
             |  HILiz4ZPnphfDt4egBwYFK4EEACKhZANiAAQiLcPBptJyMq4J+1/6wkUIrYY  |\n\
             |  iTfiGTxRRZ7ZaejZQH/kcojjv6yqkHqkxt44f16nqwHCAkb4yb+2tDbqi9uMnf|\n\
             |  5roGFnDx+6xkXn7ZCeldcLF2gdajH08AiXjXKgxTM0=                   |\n\
             |    -----END EC PRIVATE KEY-----                                |\n\
             |                                                                |\n\
             |----------------------------------------------------------------|\n\
             | RSA Key Format:                                                |\n\
             |   N = c6b9a521234567890abc4d4567e99f1234567891235987564....    |\n\
             |   E = 10001                                                    |\n\
             |   D = 37c80c81234567890abcdef1234567890abcdef1234567790....    |\n\
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
             | test:                                                          |\n\
             | {                                                              |\n\
             |     [init] init_data.int                                       |\n\
             |     [bootloader] Fsbl.elf                                      |\n\
             |     Partition1.bit                                             |\n\
             |     Partition2.elf                                             |\n\
             | }                                                              |\n\
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
             |*warning : The warnings & error information is captured.        |\n\
             | info    : The general info and all the above info is captured. |\n\
             | debug   : The debugging info in case of errors is captured in  |\n\
             |           in detail and all the above info is captured.        |\n\
             | trace   : More detailed information is captured along with the |\n\
             |           above                                                |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       |  bootgen -image test.bif -o -boot.bin -log trace               |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | The detailed info is described while creating the bootimage.   |\n\
             | The file 'bootgen_log.txt' is generated.                       |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
nonbooting
******************************************************************************/
#define NONBOOTINGHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | nonbooting                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Used for generating an intermediate boot image.                |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -nonbooting                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | bootgen -arch zynq -image test.bif -o test.bin -nonbooting     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |  [ppkfile]primary.pub                                          |\n\
             |  [spkfile]secondary.pub                                        |\n\
             |  [spksignature]secondary.pub.sha256.sig                        |\n\
             |  [bootimage,authentication=rsa,                                |\n\
             |             presign=fsbl_0.elf.0.sha256.sig]fsbl_e.bin         |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | An intermediate image test.bin is generated as output even in  |\n\
             | the absence of secret key, which is actually needed to generate|\n\
             | an authenticated image.                                        |\n\
             | This intermediate image cannot be booted.                      |\n\
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
 USAGE       | bootgen -arch zynq -image test.bif -o boot.mcs                 |\n\
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
 USAGE       | bootgen -image test.bif -o boot.bin -p xc7z020clg484           |\n\
             |   -encrypt efuse                                               |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | If the key file is not present in the path specified in BIF    |\n\
             | file, then a new encryption key is generated in the same path  |\n\
             | and xc7z020clg484 is copied along side the 'Device' field in   |\n\
             | the nky file. The generated image is an encrypted image.       |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
padimageheader
******************************************************************************/
#define PADHDRHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | padimageheader                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option pads the Image Header Table and Partition Header   |\n\
             | Table to maximum partitions allowed, to force alignment of     |\n\
             | following partitions. This feature is enabled by default.      |\n\
             | Specifying a 0, disables this feature                          |\n\
             | Zynq   : Maximum Partitions - 14                               |\n\
             | ZynqMP : Maximum Partitions - 32                               |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -padimageheader [arguments]                                    |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | *1 : Pad the header tables to max partitions                   |\n\
             |  0 : Do not pad the header tables                              |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | bootgen -image test.bif -w on -o boot.bin -padimageheader 0    |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | boot.bin has the image header tables and partition header      |\n\
             | tables in actual and no extra tables are padded. If nothing is |\n\
             | specified or if '-padimageheader 1' is used, then total image  |\n\
             | header tables and partition header tables are padded           |\n\
             | to max partitions.                                             |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
post_process
******************************************************************************/
#define POSTPROCESSHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | post_process                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option is used for post processing of CDO files.          |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -post_process <mode>                                           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [bootgen -arch versal -image test.bif -o test.bin              |\n\
             |     -post_process 3]                                           |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   |  1, 2, 3, 4, cfi-write, cfi-dma-write, cfi-dma-write-keyhole,  |\n\
             |  iso-all                                                       |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
process_bitstream
******************************************************************************/
#define PROCESSBITHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | process_bitstream                                              |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp                                                   |\n\
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
 USAGE       | bootgen -arch zynq -image test.bif -process_bitstream bin      |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    system.bit                                                  |\n\
             | }                                                              |\n\
             | Output generated is bitstream in BIN format.                   |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
read
******************************************************************************/
#define READHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | read                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This options reads the bootimage and dumps the header tables in|\n\
             |  readable form.                                                |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -read [arguments] <bootimage>                                  |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | bh  : Reads boot header                                        |\n\
             | iht : Reads image header table                                 |\n\
             | ih  : Reads image headers                                      |\n\
             | pht : Reads partition header tables                            |\n\
             | ac  : Reads authentication certificates                        |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | 1. bootgen -image test.bif -o boot.bin -read                   |\n\
             | 2. bootgen -image test.bif -o boot.bin -read pht               |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | 1. Reads all the headers and dump on console                   |\n\
             | 2. Reads partition header tables and dump on console           |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
securedebugimage
******************************************************************************/
#define SECUREDEBUGHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | authenticatedjtag                                              |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option is used to enable JTAG during secure boot          |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -authenticatedjtag [arguments]                                 |\n\
-------------+----------------------------------------------------------------+\n\
 ARGUMENTS   | rsa   : Image created using RSA                                |\n\
             | ecdsa : Image created using ECDSA                              |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | bootgen -arch versal -image test.bif -authenticatedjtag rsa    |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 split
******************************************************************************/
#define SPLITHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | split                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
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
             | test:                                                          |\n\
             | {                                                              |\n\
             |     [bootloader] Fsbl.elf                                      |\n\
             |     Partition1                                                 |\n\
             |     Partition2                                                 |\n\
             | }                                                              |\n\
             | Output files generated are:                                    |\n\
             | 1. Bootheader + Image Headers + Partition Headers + Fsbl.elf   |\n\
             | 2. Partition1.bit                                              |\n\
             | 3. Partition2.elf                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
spksignature
******************************************************************************/
#define SPKSIGNHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | spksignature                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp                                                   |\n\
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
verify
******************************************************************************/
#define VERIFYHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | verify                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal, zynqmp                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option is used for verifying authentication of a bootimage|\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -verify <bootimage>                                            |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | bootgen -arch zynqmp -verify boot.bin                          |\n\
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
verify_kdf
******************************************************************************/
#define VERIFYKDFHELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | verify_kdf                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option is used to validate the Counter Mode KDF used in   |\n\
             | bootgen for generation of AES keys..                           |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -verify <bootimage>                                            |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | bootgen -arch zynqmp -verify_kdf testvector.txt                |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | The format of the testvector.txt file is as below.             |\n\
             | L = 256                                                        |\n\
             | KI = d54b6fd94f7cf98fd955517f937e9927f9536caebe148......       |\n\
             | FixedInputDataByteLen = 60                                     |\n\
             | FixedInputData = 94c4a0c69526196c1377cebf0a2ae0fb4b57797....   |\n\
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
 USAGE       | bootgen -image test.bif -w on -o boot.bin or                   |\n\
             | bootgen -image test.bif -w    -o boot.bin                      |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | If the file boot.bin already exists in the path, then it is    |\n\
             | overwritten.                                                   |\n\
             | Options '-w on' & '-w' are treated as same                     |\n\
             | If the '-w' option is not specified, the file will not be      |\n\
             | overwritten by default                                         |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 zynqmpes1
******************************************************************************/
#define ZYNQMPES1HELP "\
-------------+----------------------------------------------------------------+\n\
 OPTION      | zynqmpes1                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This option specifies that the image generated will be used    |\n\
             | on ES1(1.0).                                                   |\n\
             | This option makes a difference only when generating            |\n\
             | authenticated image, else ignored.                             |\n\
-------------+----------------------------------------------------------------+\n\
 SYNOPSIS    | -zynqmpes1                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | bootgen -image test.bif -o boot.bin -zynqmpes1                 |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | The authentication padding scheme will be as per ES1(1.0).     |\n\
             | The default padding scheme is for (2.0)ES2 and above.          |\n\
-------------+----------------------------------------------------------------+\n"




/******************************************************************************
 aarch32_mode
******************************************************************************/
#define H_BIF_AARCH32_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | aarch32_mode                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp, versal                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | To specify the partition is to be executed in 32-bit mode.     |\n\
             | Only valid for binary partitions.                              |\n\
             | For elf files, bootgen automatically detects the execution     |\n\
             | mode from elf header.                                          |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQMP:                                                        |\n\
             | [aarch32_mode] <partition>                                     |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { aarch32_mode, file=<partition> }                             |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [bootloader, destination_cpu=a53-0] zynqmp_fsbl.elf         |\n\
             |    [destination_cpu=a53-0, aarch32_mode] hello.bin             |\n\
             |    [destination_cpu=r5-0] hello_world.elf                      |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, aarch32_mode, file=apu.bin }                |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+"

/******************************************************************************
alignment
******************************************************************************/
#define H_BIF_ALIGN_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | alignment                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Sets the byte alignment. The partition will be padded to be    |\n\
             | aligned to a multiple of this value.                           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [alignment=<value>] <partition>                                |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { alignment=<value>, file=<partition> }                        |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             |  {                                                             |\n\
             |    [bootloader] fsbl.elf                                       |\n\
             |    [alignment=64] u-boot.elf                                   |\n\
             |  }                                                             |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, alignment=64, file=uboot.elf }              |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Note: Cannot be used with offset.                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
big_endian
******************************************************************************/
#define H_BIF_BIGENDIAN_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | big_endian                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp, versal                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | To specify the partition in big endian                         |\n\
             | Only valid for binary partitions.                              |\n\
             | For elf files, bootgen automatically detects the endianness    |\n\
             | from elf header.                                               |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQMP:                                                        |\n\
             | [big_endian] <partition>                                       |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { big_endian, file=<partition> }                               |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [bootloader, destination_cpu=a53-0] zynqmp_fsbl.elf         |\n\
             |    [destination_cpu=a53-0, big_endian] hello.bin               |\n\
             |    [destination_cpu=r5-0] hello_world.elf                      |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, big_endian, file=apu.bin }                  |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+"

/******************************************************************************
blocks
******************************************************************************/
#define H_BIF_BLOCKS_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | blocks                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp, versal                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specify block sizes for key-rolling feature in encrytion.      |\n\
             | Each module is encrypted using its own unique key. The initial |\n\
             | key is stored at the key source on the device, while keys for  |\n\
             | each successive module are encrypted (wrapped) in the previous |\n\
             | module.                                                        |\n\
             | <size> mentioned will be taken in Bytes. If X(*) is specified, |\n\
             | then all the remaining blocks would of the size 'X'.           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQMP:                                                        |\n\
             | [blocks = <size><num>;<size><num>;...;<size><*>] <partition>   |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { blocks = <size><num>;...;<size><*>, file=<partition> }       |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [keysrc_encryption] bbram_red_key                           |\n\
             |    [bootloader, encryption=aes, destination_cpu=a53-0,         |\n\
             |                 blocks=4096(2);1024;2048(2);4096(*),           |\n\
             |                        aeskeyfile=encr.nky] fsbl.elf           |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, encryption=aes,                        |\n\
             |        blocks==4096(2);1024;2048(2);4096(*),                   |\n\
             |        aeskeyfile=encr1.nky,file=plm.elf }                     |\n\
             |      { type=pmcdata, aeskeyfile=encr2.nky, file=pmc_cdo.bin }  |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | In the above example, the first two blocks are of 4096 bytes,  |\n\
             | second block is of 1024 bytes, the next two blocks are of      |\n\
             | 2048 bytes, the rest of the blocks are of 4096 bytes.          |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
bootimage
******************************************************************************/
#define H_BIF_BOOTIMAGE_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | bootimage                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This specifies that the following file specification is a      |\n\
             | bootimage that was created by Bootgen, being reused as input.  |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [bootimage] <partition>                                        |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { type=bootimage, file=<partition> }                           |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [bootimage] fsbl.bin                                        |\n\
             |    [bootimage] system.bin                                      |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootimage, file=boot.pdi }                         |\n\
             |      { core=a72-0, file=apu.elf }                              |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
bootloader
******************************************************************************/
#define H_BIF_BL_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | bootloader                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This specifies the partition is a bootloader (FSBL). This      |\n\
             | attribute is specified with along with other partition bif     |\n\
             | attributes.                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [bootloader] <partition>                                       |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { type=bootloader, file=<partition> }                          |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [bootloader] fsbl.elf                                       |\n\
             |    hello.elf                                                   |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, file=hello.elf }                            |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
bootvectors
******************************************************************************/
#define H_BIF_BOOTVEC_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | bootvectors                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specifies the vector table for XIP                             |\n\
             | By default, bootgen populates XIP elf vector table below -     |\n\
             |     - 0xEAFFFFFE: for Cortex R5 and Cortex A53 (32-bit)        |\n\
             |     - 0x14000000: for Cortex A53 (64-bit)                      |\n\
             | To specify a different vector table, bootvectors can be used.  |\n\
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
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
boot_device
******************************************************************************/
#define H_BIF_BOOTDEV_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | boot_device                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp, versal                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specifies the secondary boot device. Indicates the device on   |\n\
             | which the partition is present.                                |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQMP:                                                        |\n\
             | [boot_device] <options>                                        |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | boot_device { <options>, address=<address> }                   |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | ZYNQMP:                                                        |\n\
             | qspi32, qspi24, nand, sd0, sd1, sd-ls, mmc, usb, ethernet      |\n\
             | pcie, sata                                                     |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | qspi32, qspi24, nand, sd0, sd1, sd-ls, mmc, usb, ethernet,     |\n\
             | pcie, sata, ospi, smap, sbi, sd0-raw, sd1-raw, sd-ls-raw,      |\n\
             | mmc-raw, mmc0, mmc0-raw                                        |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [boot_device] sd0                                          |\n\
             |     [bootloader,destination_cpu=a53-0] fsbl.elf                |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    boot_device { qspi32, address=0x10000 }                     |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, file=hello.elf }                            |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

#define H_BIF_BOOTCONFIG_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | boot_config                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | These parameters are used to configure the bootimage           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | boot_config { <options> }                                      |\n\
-------------+---------------------+------------------------------------------+\n\
 OPTIONS     |       Options       |             Description                  |\n\
             +---------------------+------------------------------------------+\n\
             | bh_auth_enable      | Boot Header Authentication Enable:       |\n\
             |                     | Authentication of the bootimage will     |\n\
             |                     | be done excluding the verification of    |\n\
             |                     | PPK hash and SPK ID.                     |\n\
             +---------------------+------------------------------------------+\n\
             | a_hwrot             | Asymmetric Hardware Root of Trust        |\n\
             |                     | (A-HWRoT) Boot Mode :                    |\n\
             |                     | Bootgen checks aganist the design rules  |\n\
             |                     | for A-HWRoT Boot Mode.                   |\n\
             |                     | Valid only for Production PDIs           |\n\
             +---------------------+------------------------------------------+\n\
             | s_hwrot             | Symmetric Hardware Root of Trust         |\n\
             |                     | (S-HWRoT) Boot Mode :                    |\n\
             |                     | Bootgen checks aganist the design rules  |\n\
             |                     | for S-HWRoT Boot Mode.                   |\n\
             |                     | Valid only for Production PDIs           |\n\
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
             +---------------------+------------------------------------------+\n\
             | smap_width = <value>| 8, 16, 32                                |\n\
             |                     | Default is 32-bit                        |\n\
-------------+---------------------+------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------|\n\
             | bh_auth_enable_smap_width:                                     |\n\
             | {                                                              |\n\
             |    boot_config { bh_auth_enable, smap_width=16 }               |\n\
             |    pskfile = /path/to/primary0.pem                             |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |       name = pmc_subsys, id = 0x1c000001                       |\n\
             |       {                                                        |\n\
             |           id = 0x00010000,                                     |\n\
             |           type = bootloader,                                   |\n\
             |           authentication = rsa,                                |\n\
             |           sskfile = /path/to/secondary-key.pem,                |\n\
             |           file = /path/to/plm.elf                              |\n\
             |       }                                                        |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------|\n\
             | dpacm_enable:                                                  |\n\
             | {                                                              |\n\
             |    boot_config { dpacm_enable }                                |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |       {                                                        |\n\
             |          type = bootloader,                                    |\n\
             |          encryption = aes,  blocks = 4096(*),                  |\n\
             |          keysrc = bbram_red_key,  aeskeyfile = key1.nky,       |\n\
             |          file = plm.elf                                        |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          type = pmcdata, load = 0xf2000000,                    |\n\
             |          aeskeyfile = key2.nky,                                |\n\
             |          file = pmc_cdo.bin                                    |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          core = psm,                                           |\n\
             |          encryption = aes, blocks = 4096(*),                   |\n\
             |          keysrc = bbram_red_key, aeskeyfile = key3.nky,        |\n\
             |          file = psm.elf                                        |\n\
             |       }                                                        |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
checksum
******************************************************************************/
#define H_BIF_CHKSM_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | checksum                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This specifies the partition needs to be checksummed.          |\n\
             | Checksum Algorithms:                                           |\n\
             |   Zynq            : MD5                                        |\n\
             |   ZynqMP & Versal : SHA3                                       |\n\
             |----------------------------------------------------------------|\n\
             | Note:                                                          |\n\
             | 1. In Zynq, checksum opertations is not supported for          |\n\
             |    bootloaders.                                                |\n\
             | 2. Checksum operation is not supported with secure features    |\n\
             |    like authentication and encryption                          |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [checksum = <options>] <partition>                             |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { checksum = <options>, file=<partition> }                     |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | *none : no checksum operation                                  |\n\
             |  md5  : MD5 checksum operation (Zynq)                          |\n\
             |  sha3 : SHA3 checksum operation (ZynqMP & Versal)              |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader] fsbl.elf                                      |\n\
             |     [checksum=md5] hello.elf                                   |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------+\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, checksum=sha3, file=plm.elf }          |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
copy
******************************************************************************/
#define H_BIF_COPY_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | copy                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This attribute specifies that the image is to be copied to     |\n\
             | memory at specified address.                                   |\n\
             | This is a image specific attribute and needs to be defined at  |\n\
             | image level and not partition level.                           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | copy = <addr>                                                  |\n\
-------------+---------------------+------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    boot_device { qspi32, address=0x10000 }                     |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |    }                                                           |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image2, id = 0x1c000002, copy = 0x30000            |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, file=hello.elf }                            |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
core
******************************************************************************/
#define H_BIF_CORE_H "\
-------------+----------------------------------------------------------------+\n\
ATTRIBUTE    | core                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
DESCRIPTION  | Specifies which core will execute the partition                |\n\
-------------+----------------------------------------------------------------+\n\
USAGE        | { core = <options>, file = <partition> }                       |\n\
-------------+----------------------------------------------------------------+\n\
OPTIONS      | *a72-0                                                         |\n\
             |  a72-1                                                         |\n\
             |  r5-0                                                          |\n\
             |  r5-1                                                          |\n\
             |  psm                                                           |\n\
             |  aie                                                           |\n\
-------------+----------------------------------------------------------------+\n\
EXPLANATION  | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, file=hello.elf }                            |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
delay_handoff
******************************************************************************/
#define H_BIF_DELAY_HANDOFF_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | delay_handoff                                                  |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This attribute specifies that the handoff to the subsystem is  |\n\
             | delayed.                                                       |\n\
             | This is a image specific attribute and needs to be defined at  |\n\
             | image level and not partition level.                           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | delay_handoff                                                  |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    boot_device { qspi32, address=0x10000 }                     |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |    }                                                           |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image2, id = 0x1c000002, delay_handoff             |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, file=hello.elf }                            |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
delay_load
******************************************************************************/
#define H_BIF_DELAY_LOAD_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | delay_load                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This attribute specifies that the loading the subsystem is     |\n\
             | delayed.                                                       |\n\
             | This is a image specific attribute and needs to be defined at  |\n\
             | image level and not partition level.                           |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | delay_load                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    boot_device { qspi32, address=0x10000 }                     |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |    }                                                           |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image2, id = 0x1c000002, delay_load                |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, file=hello.elf }                            |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
destination_cpu
******************************************************************************/
#define H_BIF_DESTCPU_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | destination_cpu                                                |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specifies which core will execute the partition                |\n\
             |----------------------------------------------------------------|\n\
             | Note:                                                          |\n\
             | * FSBL can run either on A53-0 or R5-0 only.                   |\n\
             | * PMU loaded by FSBL: [destination_cpu=pmu] pmu.elf            |\n\
             |   In this flow, BootROM loads FSBL first, and then FSBL loads  |\n\
             |   PMU fw.                                                      |\n\
             | * PMU loaded by BootROM: [pmufw_image] pmu.elf                 |\n\
             |   In this flow, BootROM loads PMU first and then the FSBL      |\n\
             |   So PMU does the power management tasks, before the FSBL comes|\n\
             |   up.                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [destination_cpu=<options>] <partition>                        |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | *a53-0                                                         |\n\
             |  a53-1                                                         |\n\
             |  a53-2                                                         |\n\
             |  a53-3                                                         |\n\
             |  r5-0                                                          |\n\
             |  r5-1                                                          |\n\
             |  r5-lockstep                                                   |\n\
             |  pmu                                                           |\n\
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
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
destination_device
******************************************************************************/
#define H_BIF_DESTDEV_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | destination_device                                             |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specifies the destination of the partition                     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | [destination_device=<options>] <partition>                     |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | *ps   : The partition is targetted for PS (default)            |\n\
             |  pl   : The partition is targetted for PL, for bitstreams      |\n\
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
early_handoff
******************************************************************************/
#define H_BIF_ELYHNDOFF_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | early_handoff                                                  |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
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
exception_level
******************************************************************************/
#define H_BIF_EL_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | exception_level                                                |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp, versal                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Exception level for which the core should be configured        |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQMP:                                                        |\n\
             | [exception_level=<options>] <partition>                        |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { exception_level=<options>, file=<partition> }                |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     |  el-0                                                          |\n\
             |  el-1                                                          |\n\
             |  el-2                                                          |\n\
             | *el-3                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader, destination_cpu=a53-0] fsbl.elf               |\n\
             |     [destination_cpu=a53-0, exception_level=el-3] bl31.elf     |\n\
             |     [destination_cpu=a53-0, exception_level=el-2] u-boot.elf   |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, file=uboot.elf }                            |\n\
             |      { core=a72-0, exception_level=el-3, file=bl31.elf }       |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
file
******************************************************************************/
#define H_BIF_FILE_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | file                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specifies the file for creating the partition                  |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | { file = <path/to/file> }                                      |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.cdo, file=topology.cdo }     |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, file=hello.elf }                            |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | Multiple CDO files can be merged together into one partition   |\n\
             | by specifying multiple 'file' attributes as shown above for    |\n\
             | pmc_cdo.cdo and topology.cdo                                   |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
fsbl_config
******************************************************************************/
#define H_BIF_FSBLCFG_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | fsbl_config                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | These parameters are used to configure the bootimage           |\n\
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
             |   [fsbl_config] bh_auth_enable                                 |\n\
             |   [pskfile] primary.pem                                        |\n\
             |   [sskfile] secondary.pem                                      |\n\
             |   [bootloader,destination_cpu=a53-0,authentication=rsa]fsbl.elf|\n\
             | }                                                              |\n\
             |----------------------------------------------------------------|\n\
             | FSBL which should run on A53 in 64-bit mode, is authenticated  |\n\
             | using RSA with SHA3 hashing, in Boot Header authentication mode|\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
hivec
******************************************************************************/
#define H_BIF_HIVEC_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | hivec                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp, versal                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | To specify the location of Exception Vector Table as hivec.    |\n\
             | Default is taken as lovec.                                     |\n\
             | This is applicable with a53(32 bit) and r5 cores only.         |\n\
             | hivec: exception vector table at 0xFFFF0000.                   |\n\
             | lovec: exception vector table at 0x00000000.                   |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQMP:                                                        |\n\
             | [hivec] <partition>                                            |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { hivec, file=<partition> }                                    |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader, destination_cpu=a53_0] fsbl.elf               |\n\
             |     [destination_cpu=r5-0,hivec] app1.elf                      |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=r5-0, hivec, file=hello.elf }                      |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
init
******************************************************************************/
#define H_BIF_ID_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | id                                                             |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specifies the following IDs based on the place its defined:    |\n\
             |   * pdi id       - within outermost/pdi paranthesis            |\n\
             |   * image id     - within image paranthesis                    |\n\
             |   * partition id - within partition paranthesis                |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | id = <id>                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             |  test:                                                         |\n\
             |  {                                                             |\n\
             |      id = 0x2                            // PDI ID             |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = pmc_ss,                                        |\n\
             |          id = 0x1c000001                 // Image ID           |\n\
             |          {                                                     |\n\
             |              type = bootloader,                                |\n\
             |              id = 0x1,                   // Partition ID       |\n\
             |              file = plm.elf                                    |\n\
             |          }                                                     |\n\
             |          {                                                     |\n\
             |              type = pmcdata,                                   |\n\
             |              load = 0xf2000000,                                |\n\
             |              file = pmc_cdo.bin                                |\n\
             |          }                                                     |\n\
             |      }                                                         |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = psm_ss,                                        |\n\
             |          id = 0x1c000001                 // Image ID           |\n\
             |          {                                                     |\n\
             |              core = psm,                                       |\n\
             |              id = 0x3,                   // Partition ID       |\n\
             |              file = psm_fw.elf                                 |\n\
             |          }                                                     |\n\
             |      }                                                         |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = apu_ss,                                        |\n\
             |          id = 0x1c000002                 // Image ID           |\n\
             |          {                                                     |\n\
             |              type = cdo,                                       |\n\
             |              id = 0x4,                   // Partition ID       |\n\
             |              file = apu_cdo.bin                                |\n\
             |          }                                                     |\n\
             |          {                                                     |\n\
             |              core = a72-0,                                     |\n\
             |              id = 0x5,                   // Partition ID       |\n\
             |              file = a72_app.bin                                |\n\
             |          }                                                     |\n\
             |      }                                                         |\n\
             |  }                                                             |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
image
******************************************************************************/
#define H_BIF_IMAGE_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | image                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This attribute is used to define a subsytem/image              |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | image {  }                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             |  test:                                                         |\n\
             |  {                                                             |\n\
             |      id = 0x2                                                  |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = PMC_SS, id = 0x1c000000                        |\n\
             |          { id = 0x1, type = bootloader, file = plm.elf }       |\n\
             |          { id = 0x9, type = pmcdata, file = pmc_cdo.bin }      |\n\
             |      }                                                         |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = PL_SS, id = 0x1c000004                         |\n\
             |          { id = 0x3, type = cdo, file = system.rcdo }          |\n\
             |          { id = 0x4, file = bitstream.rnpi }                   |\n\
             |      }                                                         |\n\
             |  }                                                             |\n\
-------------+----------------------------------------------------------------|\n"

/******************************************************************************
 init
******************************************************************************/
#define H_BIF_INIT_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | init                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Register initialization block at the end of the bootloader,    |\n\
             | built by parsing the .int file specification. Maximum of 256   |\n\
             | address-value init pairs are allowed. The int files have       |\n\
             | a specific format.                                             |\n\
             |----------------------------------------------------------------|\n\
             | Sample .int file - test.int                                    |\n\
             |      .set. 0xF8000120 = 0x1F000200;                            |\n\
             |      .set. 0xF8000720 = 0x00000202;                            |\n\
             |      .set. 0xF800014C = 0x00000521;                            |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [init] <filename>                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | init = <filename>                                              |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------|\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [init] test.int                                             |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    init = reginit.int                                          |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
load
******************************************************************************/
#define H_BIF_LOAD_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | load                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Sets the load address for the partition in memory.             |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [load = <value>] <partition>                                   |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { load = <value>, file = <partition> }                         |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [bootloader] fsbl.elf                                     |\n\
             |      u-boot.elf                                                |\n\
             |      [load=0x3000000, offset=0x500000] uImage.bin              |\n\
             |      [load=0x2A00000, offset=0xa00000] devicetree.dtb          |\n\
             |      [load=0x2000000, offset=0xc00000] uramdisk.image.gz       |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, load=0xf2000000, file=pmc_cdo.bin }       |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, file=hello.elf }                            |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
metahdr
******************************************************************************/
#define H_BIF_METAHDR_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | metaheader                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This attribute is used to define encryption, authentication    |\n\
             | attributes for meta headers like keys, key sources etc.        |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | metaheader {  }                                                |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | test:                                                          |\n\
             |  {                                                             |\n\
             |      metaheader                                                |\n\
             |      {                                                         |\n\
             |          encryption = aes,                                     |\n\
             |          keysrc = bbram_red_key,                               |\n\
             |          aeskeyfile = headerkey.nky,                           |\n\
             |          authentication = rsa                                  |\n\
             |      }                                                         |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = pmc_subsys, id = 0x1c000001                    |\n\
             |          {                                                     |\n\
             |              type = bootloader,                                |\n\
             |              encryption = aes,                                 |\n\
             |              keysrc = bbram_red_key,                           |\n\
             |              aeskeyfile = key1.nky,                            |\n\
             |              blocks = 8192(*),                                 |\n\
             |              file = plm.elf                                    |\n\
             |          }                                                     |\n\
             |          {                                                     |\n\
             |              type = pmcdata,                                   |\n\
             |              load = 0xf2000000,                                |\n\
             |              aeskeyfile = key2.nky,                            |\n\
             |              file = pmc_cdo.bin                                |\n\
             |          }                                                     |\n\
             |      }                                                         |\n\
             |  }                                                             |\n\
-------------+----------------------------------------------------------------|\n"

/******************************************************************************
name
******************************************************************************/
#define H_BIF_NAME_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | name                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specifies the name of the image/subsystem                      |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | name = <name>                                                  |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             |  test:                                                         |\n\
             |  {                                                             |\n\
             |      id = 0x2                            // PDI ID             |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = pmc_ss,                  // Image name         |\n\
             |          id = 0x1c000001                 // Image ID           |\n\
             |          {                                                     |\n\
             |              type = bootloader,                                |\n\
             |              id = 0x1,                   // Partition ID       |\n\
             |              file = plm.elf                                    |\n\
             |          }                                                     |\n\
             |          {                                                     |\n\
             |              type = pmcdata,                                   |\n\
             |              load = 0xf2000000,                                |\n\
             |              file = pmc_cdo.bin                                |\n\
             |          }                                                     |\n\
             |      }                                                         |\n\
             |  }                                                             |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
 owner
******************************************************************************/
#define H_BIF_POWNER_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTES  | partition_owner, owner                                         |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Owner of the partition which is responsible to load the        |\n\
             | partition.                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [partition_owner = <options>] <filename>                       |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { owner = <options>, file=<filename> }                         |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | ZYNQ/ZYNQMP:                                                   |\n\
             | *fsbl    : FSBL loads this partition                           |\n\
             |  uboot   : UBOOT loads this partition                          |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | *plm     : PLM loads this partition                            |\n\
             |  non-plm : PLM ignores this partition, someone else loads      |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------|\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader] fsbl.elf                                      |\n\
             |     uboot.elf                                                  |\n\
             |     [partition_owner=uboot] hello.elf                          |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     image                                                      |\n\
             |     {                                                          |\n\
             |         name = pmc_subsys, id = 0x1c000001                     |\n\
             |         {                                                      |\n\
             |              id = 0x00010000,                                  |\n\
             |              type = bootloader,                                |\n\
             |              file = /path/to/plm.elf                           |\n\
             |         }                                                      |\n\
             |     }                                                          |\n\
             |     image                                                      |\n\
             |     {                                                          |\n\
             |         name = apu_subsys,  id = 0x1c000003                    |\n\
             |         {                                                      |\n\
             |              id = 0x00000000,                                  |\n\
             |              core = a72-0,                                     |\n\
             |              owner = non-plm,                                  |\n\
             |              file = /path/to/image.ub                          |\n\
             |         }                                                      |\n\
             |     }                                                          |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
offset
******************************************************************************/
#define H_BIF_OFFSET_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | offset                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Sets the absolute offset of the partition in the boot image.   |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [offset = <value>] <filename>                                  |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { offset = <value>, file=<filename> }                          |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [bootloader] fsbl.elf                                     |\n\
             |      u-boot.elf                                                |\n\
             |      [load=0x3000000, offset=0x500000] uImage.bin              |\n\
             |      [load=0x2A00000, offset=0xa00000] devicetree.dtb          |\n\
             |      [load=0x2000000, offset=0xc00000] uramdisk.image.gz       |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { offset=0x8000, file=data.bin }                          |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
parent_id
******************************************************************************/
#define H_BIF_PARENTID_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | parent_id                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specifies the ID for the parent PDI. This is used to identify  |\n\
             | the relationship between a partial PDI and its corresponding   |\n\
             | Boot PDI                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | parent_id = <id>                                               |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             |  test:                                                         |\n\
             |  {                                                             |\n\
             |      parent_id = 0x2                                           |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = psm_ss, id = 0x1c000001                        |\n\
             |          { core = psm, file = psm_fw.elf }                     |\n\
             |      }                                                         |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = apu_ss, id = 0x1c000002                        |\n\
             |          { type = cdo, file = ps_cdo.bin }                     |\n\
             |          { core = a72-0, file = a72_app.elf }                  |\n\
             |      }                                                         |\n\
             |  }                                                             |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
partition
******************************************************************************/
#define H_BIF_PARTITION_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | partition                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This attribute is used to define a partition                   |\n\
             | It is an optional attribute to make the BIF more readable      |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | partition {  }                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             |  test:                                                         |\n\
             |  {                                                             |\n\
             |      id = 0x2                                                  |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = WDI_FLAT, id = 0x1c000000                      |\n\
             |          partition                                             |\n\
             |          {                                                     |\n\
             |              id = 0x1                                          |\n\
             |              type = bootloader                                 |\n\
             |              file = plm.elf                                    |\n\
             |          }                                                     |\n\
             |          partition                                             |\n\
             |          {                                                     |\n\
             |              id = 0x9                                          |\n\
             |              type = pmcdata,                                   |\n\
             |              file = pmc_cdo.bin                                |\n\
             |          }                                                     |\n\
             |          partition                                             |\n\
             |          {                                                     |\n\
             |              id = 0x3                                          |\n\
             |              type = cdo                                        |\n\
             |              file = system.rcdo                                |\n\
             |          }                                                     |\n\
             |          partition                                             |\n\
             |          {                                                     |\n\
             |              id = 0x4                                          |\n\
             |              file = system.rnpi                                |\n\
             |          }                                                     |\n\
             |      }                                                         |\n\
             |  }                                                             |\n\
             |                                                                |\n\
             | The same BIF can be written in shorter format as:              |\n\
             |  test:                                                         |\n\
             |  {                                                             |\n\
             |      id = 0x2                                                  |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = WDI_FLAT, id = 0x1c000000                      |\n\
             |          { id = 0x1, type = bootloader, file = plm.elf }       |\n\
             |          { id = 0x9, type = pmcdata, file = pmc_cdo.bin }      |\n\
             |          { id = 0x3, type = cdo, file = system.rcdo }          |\n\
             |          { id = 0x4, file = system.rnpi }                      |\n\
             |      }                                                         |\n\
             |  }                                                             |\n\
             |                                                                |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
pid
******************************************************************************/
#define H_BIF_PID_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | pid                                                            |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
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
pmufw_image
******************************************************************************/
#define H_BIF_PFW_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | pmufw_image                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | PMU Firmware image to be loaded by BootROM, before loading the |\n\
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
reserve
******************************************************************************/
#define H_BIF_RES_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | reserve                                                        |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Reserves the memory and padded after the partition.            |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [reserve = <value>] <filename>                                 |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { reserve = <value>, file=<filename> }                         |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [bootloader] fsbl.elf                                     |\n\
             |      [reserve=0x1000] test.bin                                 |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { reserve=0x1000, file=data.bin }                         |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
slr
******************************************************************************/
#define H_BIF_SLR_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | slr                                                            |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This attribute is used to specify the SLR identity to partition|\n\
             | It is valid for SSIT devices                                   |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | { slr = <index> }                                              |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             |  master_pdi:                                                   |\n\
             |  {                                                             |\n\
             |    id=2                                                        |\n\
             |    /* PMC Subsystem for Master SLR */                          |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |        name=pmc_subsys, id=0x1c000001                          |\n\
             |        { id=0x1, type=bootloader, file=plm.elf }               |\n\
             |        { type=pmcdata, load=0xf2000000, file=pmc_cdo.cdo }     |\n\
             |    }                                                           |\n\
             |    /* Slave SLR Boot PDIs */                                   |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |        name=slr_boot_subsys, id=0x1c000000, type=slr-boot      |\n\
             |        { file=slr_boot.cdo }                                   |\n\
             |    }                                                           |\n\
             |    /* Other images/partitions of Master SLR */                 |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |        name=psm_subsys, id=0x1c000002                          |\n\
             |        { id=0x2, core=psm, file=psm.elf }                      |\n\
             |    }                                                           |\n\
             |    /* Slave SLR Config PDIs */                                 |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |        name=slr_config_subsys, id=0x1c000000, type=slr-config  |\n\
             |        /* Slave SLR config PDIs */                             |\n\
             |        { slr=1, file=slr1_config.pdi }                         |\n\
             |        { slr=2, file=slr2_config.pdi }                         |\n\
             |        { slr=3, file=slr3_config.pdi }                         |\n\
             |        /* Master SLR PL config partition */                    |\n\
             |        { slr=0, file=/path/to/system.cdo }                     |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
split
******************************************************************************/
#define H_BIF_SPLIT_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | split                                                          |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp                                                   |\n\
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
startup
******************************************************************************/
#define H_BIF_STARTUP_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | startup                                                        |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Sets the entry address for the partition, after it is loaded.  |\n\
             | This is ignored for partitions that do not execute.            |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [startup = <value>] <filename>                                 |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { startup = <value>, file = <filename> }                       |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [bootloader] fsbl.elf                                     |\n\
             |      [startup=0x1000000] app.bin                               |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, load=0x1000,startup=0x1000, file=hello.bin }|\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
trustzone
******************************************************************************/
#define H_BIF_TZ_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | trustzone                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp, versal                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Configures the core to be Trustzone secure or nonsecure        |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [trustzone = <options> ] <filename>                            |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { trustzone = <options>, file = <filename> }                   |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     |  secure                                                        |\n\
             |  nonsecure                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [bootloader, destination_cpu=a53-0] fsbl.elf               |\n\
             |     [exception_level=el-3, trustzone = secure] bl31.elf        |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |      name = image1, id = 0x1c000001                            |\n\
             |      { type=bootloader, file=plm.elf }                         |\n\
             |      { type=pmcdata, file=pmc_cdo.bin }                        |\n\
             |      { type=cdo, file=fpd_data.cdo }                           |\n\
             |      { core=psm, file=psm.elf }                                |\n\
             |      { core=a72-0, file=uboot.elf }                            |\n\
             |      { core=a72-0, exception_level=el-3, trustzone=secure,     |\n\
             |        file=bl31.elf }                                         |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
type
******************************************************************************/
#define H_BIF_TYPE_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | type                                                           |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specifies the type of partition                                |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | { type = <options>, file = <partition> }                       |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | bootloader                                                     |\n\
             | pmcdata                                                        |\n\
             | cdo                                                            |\n\
             | cfi                                                            |\n\
             | cfi-gsc                                                        |\n\
             | bootimage                                                      |\n\
             | slr-boot                                                       |\n\
             | slr-config                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          name = pmc_subsys, id = 0x1c000001                    |\n\
             |          {                                                     |\n\
             |               type = bootloader,                               |\n\
             |               file = plm.elf                                   |\n\
             |          }                                                     |\n\
             |          {                                                     |\n\
             |               type = pmcdata,                                  |\n\
             |               load = 0xf2000000,                               |\n\
             |               file = pmc_data.cdo                              |\n\
             |          }                                                     |\n\
             |      }                                                         |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
udf_bh
******************************************************************************/
#define H_BIF_UDFBH_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | udf_bh                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp                                                   |\n\
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
xip_mode
******************************************************************************/
#define H_BIF_XIP_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | xip_mode                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp                                                   |\n\
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
 aeskeyfile
******************************************************************************/
#define H_BIF_AES_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | aeskeyfile                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp, versal                                           |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | The path to the AES keyfile. The keyfile contains AES key used |\n\
             | to encrypt the partitions. The contents of the key file needs  |\n\
             | to be written to key source                                    |\n\
             | If the key file is not present in the path specified, a new key|\n\
             | is generated by bootgen, which is used for encryption.         |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/FPGA:                                                     |\n\
             | [aeskeyfile] <key filename>                                    |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             | [aeskeyfile = <keyfile name>] <partition>                      |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { aeskeyfile = <keyfile name>, file = <filename> }             |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQ:                                                          |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [keysrc_encryption] bbram_red_key                         |\n\
             |      [aeskeyfile] test.nky                                     |\n\
             |      [bootloader, encryption=aes] fsbl.elf                     |\n\
             |      [encryption=aes] hello.elf                                |\n\
             | }                                                              |\n\
             | The partitions fsbl.elf & hello.elf are encrypted using        |\n\
             | test.nky key file.                                             |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [keysrc_encryption] bbram_red_key                         |\n\
             |      [bootloader,encryption=aes,aeskeyfile=test.nky] fsbl.elf  |\n\
             |      [encryption=aes,aeskeyfile=test1.nky] hello.elf           |\n\
             |      [encryption=aes,aeskeyfile=test2.nky] app.elf             |\n\
             | }                                                              |\n\
             | * Each partition being encrypted needs its own aeskeyfile.     |\n\
             | * Key0, IV0 and Key Opt should be the same accross all nky     |\n\
             | files that will be used.                                       |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |       name = pmc_subsys, id = 0x1c000001                       |\n\
             |       {                                                        |\n\
             |          type = bootloader, encryption = aes,                  |\n\
             |          keysrc = bbram_red_key, aeskeyfile = key1.nky,        |\n\
             |          file = plm.elf                                        |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          type = pmcdata, load = 0xf2000000,                    |\n\
             |          aeskeyfile = key2.nky, file = pmc_cdo.bin             |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          type=cdo, encryption = aes,                           |\n\
             |          keysrc = efuse_red_key, aeskeyfile = key3.nky,        |\n\
             |          file=fpd_data.cdo                                     |\n\
             |       }                                                        |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+"

/******************************************************************************
bh_keyfile
******************************************************************************/
#define H_BIF_BHKEY_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | bh_keyfile                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp, versal                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | 256-bit obfuscated key to be stored in boot header             |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQMP:                                                        |\n\
             | [bh_keyfile] <key file path>                                   |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | bh_keyfile = <key file path>                                   |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [keysrc_encryption] bh_gry_key                              |\n\
             |    [bh_keyfile] obfuscated_key.txt                             |\n\
             |    [bh_key_iv] obfuscated_iv.txt                               |\n\
             |    [bootloader, encryption=aes, aeskeyfile = encr.nky,         |\n\
             |                          destination_cpu=a53-0]fsbl.elf        |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    bh_keyfile = bh_key1.txt                                    |\n\
             |    bh_kek_iv = blk_iv.txt                                      |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |       name = pmc_subsys, id = 0x1c000001                       |\n\
             |       {                                                        |\n\
             |          type = bootloader, encryption = aes,                  |\n\
             |          keysrc = bbram_red_key, aeskeyfile = key1.nky,        |\n\
             |          file = plm.elf                                        |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          type = pmcdata, load = 0xf2000000,                    |\n\
             |          aeskeyfile = key2.nky, file = pmc_cdo.bin             |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          type=cdo, encryption = aes,                           |\n\
             |          keysrc = bh_blk_key, aeskeyfile = key3.nky,           |\n\
             |          file=fpd_data.cdo                                     |\n\
             |       }                                                        |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
bh_key_iv
******************************************************************************/
#define H_BIF_BHIV_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | bh_key_iv                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Initialization vector used when decrypting the obfuscated key  |\n\
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
userkeys
******************************************************************************/
#define H_BIF_USERKEYS_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | userkeys                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
DESCRIPTION  | The path to the user keyfile. The keyfile contains user keys   |\n\
             | used to encrypt the partitions.                                |\n\
             | The size of user key can be 128 or 256 bits.                   |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | userkeys = <filename>                                          |\n\
-------------+----------------------------------------------------------------+\n\
 FILE FORMAT | user_key0 <userkey0 value>                                     |\n\
             | user_key1 <userkey1 value>                                     |\n\
             | user_key2 <userkey2 value>                                     |\n\
             | user_key3 <userkey3 value>                                     |\n\
             | user_key4 <userkey4 value>                                     |\n\
             | user_key5 <userkey5 value>                                     |\n\
             | user_key6 <userkey6 value>                                     |\n\
             | user_key7 <userkey7 value>                                     |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------|\n\
             | new_bif:                                                       |\n\
             | {                                                              |\n\
             |   userkeys = userkeyfile.txt                                   |\n\
             |   id_code = 0x04ca8093                                         |\n\
             |   extended_id_code = 0x01                                      |\n\
             |   id = 0x2                                                     |\n\
             |   image                                                        |\n\
             |   {                                                            |\n\
             |      name = pmc_subsys                                         |\n\
             |      id = 0x1c000001                                           |\n\
             |      partition                                                 |\n\
             |      {                                                         |\n\
             |        id = 0x01                                               |\n\
             |        type = bootloader                                       |\n\
             |        file = plm.elf                                          |\n\
             |      }                                                         |\n\
             |      partition                                                 |\n\
             |      {                                                         |\n\
             |        id = 0x09                                               |\n\
             |        type = pmcdata, load = 0xf2000000                       |\n\
             |        file = topology_xcvc1902.v2.cdo                         |\n\
             |        file = bd_70da_ps_pmc_0_pmc_data.cdo                    |\n\
             |      }                                                         |\n\
             |   }                                                            |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+"
 
/******************************************************************************
encryption
******************************************************************************/
#define H_BIF_ENCR_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | encryption                                                     |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | This specifies the partition needs to be encrypted.            |\n\
             | Encryption Algorithms                                          |\n\
             |  Zynq            : AES-CBC                                     |\n\
             |  ZynqMP & Versal : AES-GCM                                     |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP/FPGA:                                              |\n\
             | [encryption = <options>] <partition>                           |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | { encryption = <options>, file = <filename> }                  |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | *none : Partition not encrypted                                |\n\
             |  aes  : Partition encrypted using AES algorithm                |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |      [aeskeyfile] test.nky                                     |\n\
             |      [bootloader, encryption=aes] fsbl.elf                     |\n\
             |      [encryption=aes] hello.elf                                |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |       name = pmc_subsys, id = 0x1c000001                       |\n\
             |       {                                                        |\n\
             |          type = bootloader, encryption = aes,                  |\n\
             |          keysrc = bbram_red_key, aeskeyfile = key1.nky,        |\n\
             |          file = plm.elf                                        |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          type = pmcdata, load = 0xf2000000,                    |\n\
             |          aeskeyfile = key2.nky, file = pmc_cdo.bin             |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          type=cdo, encryption = aes,                           |\n\
             |          keysrc = efuse_red_key, aeskeyfile = key3.nky,        |\n\
             |          file=fpd_data.cdo                                     |\n\
             |       }                                                        |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+"

/******************************************************************************
familykey
******************************************************************************/
#define H_BIF_METAL_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | familykey                                                      |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | fpga, zynqmp                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Specify Family Key                                             |\n\
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
keysrc_encryption
******************************************************************************/
#define H_BIF_KEYSRCENCR_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | keysrc_encryption                                              |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp                                                   |\n\
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
keysrc
******************************************************************************/
#define H_BIF_KEYSRC_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | keysrc                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | versal                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Key source for encryption                                      |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | { keysrc = <options>, file = <filename> }                      |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | Key sources applicable for bootloader, metaheader & partitions:|\n\
             |      efuse_red_key                                             |\n\
             |      efuse_blk_key                                             |\n\
             |      bbram_red_key                                             |\n\
             |      bbram_blk_key                                             |\n\
             |      bh_blk_key                                                |\n\
             | Additional key sources applicable only for partitions:         |\n\
             |      user_key0                                                 |\n\
             |      user_key1                                                 |\n\
             |      user_key2                                                 |\n\
             |      user_key3                                                 |\n\
             |      user_key4                                                 |\n\
             |      user_key5                                                 |\n\
             |      user_key6                                                 |\n\
             |      user_key7                                                 |\n\
             |      efuse_user_key0                                           |\n\
             |      efuse_user_blk_key0                                       |\n\
             |      efuse_user_key1                                           |\n\
             |      efuse_user_blk_key1                                       |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |       name = pmc_subsys, id = 0x1c000001                       |\n\
             |       {                                                        |\n\
             |          type = bootloader, encryption = aes,                  |\n\
             |          keysrc = bbram_red_key, aeskeyfile = key1.nky,        |\n\
             |          file = plm.elf                                        |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          type = pmcdata, load = 0xf2000000,                    |\n\
             |          aeskeyfile = key2.nky, file = pmc_cdo.bin             |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          type=cdo, encryption = aes,                           |\n\
             |          keysrc = efuse_red_key, aeskeyfile = key3.nky,        |\n\
             |          file=fpd_data.cdo                                     |\n\
             |       }                                                        |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
dpacm_enable
******************************************************************************/
#define H_DPACM_ENABLE_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | dpacm_enable                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Sets the DPA Counter Measure bit in the Partition Header Table |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | { dpacm_enable }                                               |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------|\n\
             | dpacm_enable:                                                  |\n\
             |  {                                                             |\n\
             |      image                                                     |\n\
             |      {                                                         |\n\
             |          {                                                     |\n\
             |               type = bootloader,                               |\n\
             |               encryption = aes,  blocks = 4096(*),             |\n\
             |               keysrc = bbram_red_key,  aeskeyfile = key1.nky,  |\n\
             |               dpacm_enable,                                    |\n\
             |               file = plm.elf                                   |\n\
             |          }                                                     |\n\
             |          {                                                     |\n\
             |               type = pmcdata, load = 0xf2000000,               |\n\
             |               aeskeyfile = key2.nky,                           |\n\
             |               file = pmc_cdo.bin                               |\n\
             |          }                                                     |\n\
             |          {                                                     |\n\
             |               core = psm,                                      |\n\
             |               encryption = aes, blocks = 4096(*),              |\n\
             |               keysrc = bbram_red_key, aeskeyfile = key3.nky,   |\n\
             |               dpacm_enable,                                    |\n\
             |               file = psm.elf                                   |\n\
             |          }                                                     |\n\
             |      }                                                         |\n\
             |  }                                                             |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
puf_file
******************************************************************************/
#define H_BIF_PUFDATA_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | puf_file                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp, versal                                                 |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | PUF helper data file.                                          |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQMP:                                                        |\n\
             | [puf_file] <puf data file>                                     |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             |  puf_file = <puf data file>                                    |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    [fsbl_config] pufhd_bh                                      |\n\
             |    [puf_file] pufhelperdata.txt                                |\n\
             |    [bh_keyfile] black_key.txt                                  |\n\
             |    [bh_key_iv] bhkeyiv.txt                                     |\n\
             |    [bootloader,destination_cpu=a53-0,encryption=aes]fsbl.elf   |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------|\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    boot_config {puf4kmode}                                     |\n\
             |    puf_file = pufhd_file_4K.txt                                |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |       name = pmc_subsys, id = 0x1c000001                       |\n\
             |       {                                                        |\n\
             |          type = bootloader, encryption = aes,                  |\n\
             |          keysrc = bbram_red_key, aeskeyfile = key1.nky,        |\n\
             |          file = plm.elf                                        |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          type = pmcdata, load = 0xf2000000,                    |\n\
             |          aeskeyfile = key2.nky, file = pmc_cdo.bin             |\n\
             |       }                                                        |\n\
             |       {                                                        |\n\
             |          type=cdo, encryption = aes,                           |\n\
             |          keysrc = efuse_red_key, aeskeyfile = key3.nky,        |\n\
             |          file=fpd_data.cdo                                     |\n\
             |       }                                                        |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
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
             |     Zynq & FPGA  : RSA-2048                                    |\n\
             |     ZynqMP       : RSA-4096                                    |\n\
             |     Versal       : RSA-4096 & ECDSA                            |\n\
             | Note : p384 and p521 curves are supported for ECDSA            |\n\
-------------+----------------------------------------------------------------+\n\
 USAGE       | ZYNQ/ZYNQMP/FPGA:                                              |\n\
             | [authenication = <options>] <partition>                        |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | {authenication=<options>, file=<partition>}                    |\n\
-------------+----------------------------------------------------------------+\n\
 OPTIONS     | *none       : Partition not authenticated                      |\n\
             |  rsa        : Partition authenticated using RSA                |\n\
             |  ecdsa-p384 : Partition authenticated using ECDSA(P384 Curve)  |\n\
             |  ecdsa-p521 : Partition authenticated using ECDSA(P521 Curve)  |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF - test.bif                                          |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQMP:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |     [ppkfile] ppk.txt                                          |\n\
             |     [spkfile] spk.txt                                          |\n\
             |     [bootloader, authentication=rsa] fsbl.elf                  |\n\
             |     [authentication=rsa] hello.elf                             |\n\
             | }                                                              |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    boot_config {bh_auth_enable}                                |\n\
             |    pskfile = primary0.pem                                      |\n\
             |    sskfile = secondary0.pem                                    |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |       name = pmc_ss, id = 0x1c000001                           |\n\
             |       {type=bootloader, authentication=rsa, file=plm.elf}      |\n\
             |       {type=pmcdata, load=0xf2000000, file=pmc_cdo.bin}        |\n\
             |       {type=cdo, authentication=rsa, file=fpd_cdo.bin}         |\n\
             |       {core=psm, authentication=rsa, file=psm.elf}             |\n\
             |       {core=a72-0, authentication=rsa, file = apu.elf}         |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
-------------+----------------------------------------------------------------+\n"

/******************************************************************************
auth_params
******************************************************************************/
#define H_BIF_AUTHPARAM_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | auth_params                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Extra options for authentication                               |\n\
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
bhsignature
******************************************************************************/
#define H_BIF_BHSIGN_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | bhsignature                                                    |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
-------------+----------------------------------------------------------------+\n\
 DESCRIPTION | Imports Boot Header signature into authentication certificate. |\n\
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
headersignature
******************************************************************************/
#define H_BIF_HDRSIGN_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | headersignature                                                |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynqmp                                                         |\n\
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
 USAGE       | ZYNQ/ZYNQMP:                                                   |\n\
             | [ppkfile] <key filename>                                       |\n\
             | [pskfile] <key filename>                                       |\n\
             | [spkfile] <key filename>                                       |\n\
             | [sskfile] <key filename>                                       |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             | ppkfile = <filename>                                           |\n\
             | pskfile = <filename>                                           |\n\
             | spkfile = <filename>                                           |\n\
             | sskfile = <filename>                                           |\n\
-------------+----------------------------------------------------------------+\n\
 EXPLANATION | Sample BIF 1 - test.bif                                        |\n\
             +----------------------------------------------------------------+\n\
             | ZYNQ/ZYNQMP:                                                   |\n\
             +----------------------------------------------------------------|\n\
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
             |     bootloader, destination_cpu = a53-0, authentication = rsa, |\n\
             |     sskfile = secondary1.pem                                   |\n\
             |   ]fsbla53.elf                                                 |\n\
             |                                                                |\n\
             |  /* Partition-1 */                                             |\n\
             |   [                                                            |\n\
             |     destination_cpu = a53-0,                                   |\n\
             |     authentication = rsa                                       |\n\
             |   ] hello.elf                                                  |\n\
             |  }                                                             |\n\
             | For ZynqMP:                                                    |\n\
             |  - PPK/PSK cannot be specific to partition.                    |\n\
             |  - Each partition can have its own SPK/SSK.(Refer sample BIF 2)|\n\
             |    The SPKs can be differentiated using the spk_id.            |\n\
             |  - SPK/SSK outside the partition scope is mandatory for        |\n\
             |    authentication. These keys will be used to authenticate     |\n\
             |    headers and any other partition that does not have a        |\n\
             |    specific SPK/SSK.                                           |\n\
             +----------------------------------------------------------------+\n\
             | VERSAL:                                                        |\n\
             +----------------------------------------------------------------|\n\
             | all:                                                           |\n\
             | {                                                              |\n\
             |    boot_config {bh_auth_enable}                                |\n\
             |    image                                                       |\n\
             |    {                                                           |\n\
             |       name = pmc_ss, id = 0x1c000001                           |\n\
             |       { type=bootloader, authentication=rsa, file=plm.elf,     |\n\
             |         pskfile=primary0.pem, sskfile=secondary0.pem }         |\n\
             |       { type = pmcdata, load = 0xf2000000, file=pmc_cdo.bin }  |\n\
             |       { type=cdo, authentication=rsa, file=fpd_cdo.bin,        |\n\
             |         pskfile = primary1.pem, sskfile = secondary1.pem  }    |\n\
             |    }                                                           |\n\
             | }                                                              |\n\
             | For Versal:                                                    |\n\
             |   PPK/PSK can be specific to partition.                        |\n\
-------------+----------------------------------------------------------------+\n"


/******************************************************************************
presign
******************************************************************************/
#define H_BIF_PRESIGN_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | presign                                                        |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp                                                   |\n\
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
 spksignature
******************************************************************************/
#define H_BIF_SPKSIGN_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | spksignature                                                   |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp                                                   |\n\
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
 udf_data
******************************************************************************/
#define H_BIF_UDF_H "\
-------------+----------------------------------------------------------------+\n\
 ATTRIBUTE   | udf_data                                                       |\n\
-------------+----------------------------------------------------------------+\n\
 SUPPORTED   | zynq, zynqmp                                                   |\n\
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
#endif
