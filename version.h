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

#ifndef RDI_VERSION
#define RDI_VERSION "2019.2"
#endif

#define PROGRAMNAME     "Xilinx Bootgen"
#define COPYRIGHT       "Copyright (c) 1995-2019 Xilinx, Inc. All rights reserved.\n"


/******************************************************************************
 help
******************************************************************************/
#define HELP            "\
------------------------------------------------------------------------------+\n\
                       COMMAND LINE OPTIONS                                   |\n\
-------------------------------+----------------------------------------------+\n\
 -arch              [options]  | Xilinx Architecture                          |\n\
                               | options: [zynq, zynqmp, fpga]                |\n\
-------------------------------+----------------------------------------------+\n\
 -image            <filename>  | Input Boot Image File (.bif)                 |\n\
-------------------------------+----------------------------------------------+\n\
 -o                <filename>  | Output filename in MCS/BIN format            |\n\
-------------------------------+----------------------------------------------+\n\
 -w                 [options]  | Overwrite mode                               |\n\
                               | options: [on, off]                           |\n\
-------------------------------+----------------------------------------------+\n\
 -encrypt           [options]  | AES Key storage in chip (Zynq only)          |\n\
                               | options: [bbram, efuse]                      |\n\
-------------------------------+----------------------------------------------+\n\
 -p                  <string>  | Part name                                    |\n\
-------------------------------+----------------------------------------------+\n\
 -efuseppkbits     <filename>  | Generate PPK hash for e-fuse                 |\n\
-------------------------------+----------------------------------------------+\n\
 -generate_hashes              | Generate SHA hashes (PKCS#1v1.5)             |\n\
-------------------------------+----------------------------------------------+\n\
 -spksignature     <filename>  | Generate SPK signature file                  |\n\
-------------------------------+----------------------------------------------+\n\
 -fill             <hex-byte>  | Fill byte for padding                        |\n\
-------------------------------+----------------------------------------------+\n\
 -split             [options]  | Split partitions to diff files               |\n\
                               | options: [bin, mcs]                          |\n\
-------------------------------+----------------------------------------------+\n\
 -padimageheader    [options]  | Pad header tables                            |\n\
                               | options: [0, 1]                              |\n\
-------------------------------+----------------------------------------------+\n\
 -process_bitstream [options]  | Outputs bitstream in bin/mcs format          |\n\
                               | options: [bin, mcs]                          |\n\
-------------------------------+----------------------------------------------+\n\
 -generate_keys     [options]  | Generate authentication keys                 |\n\
                               | options: [pem, rsa, obfuscatedkey]           |\n\
-------------------------------+----------------------------------------------+\n\
 -dual_qspi_mode    [options]  | Generate 2 output files for Dual QSPI        |\n\
                               | options: [parallel, stacked <size>]          |\n\
-------------------------------+----------------------------------------------+\n\
 -log               [options]  | Generate log file                            |\n\
                               | options: [error, warning, info, debug, trace]|\n\
-------------------------------+----------------------------------------------+\n\
 -zynqmpes1                    | Generate boot image for (1.0)ES1             |\n\
-------------------------------+----------------------------------------------|\n\
 -nonbooting                   | Generate an intermediate boot image          |\n\
-------------------------------+----------------------------------------------|\n\
 -encryption_dump              | Generate encryption log file                 |\n\
-------------------------------+----------------------------------------------+\n\
 -verify                       | Verify BootImage authentication              |\n\
-------------------------------+----------------------------------------------+\n\
 -h | -help                    | Print the help summary                       |\n\
-------------------------------+----------------------------------------------+\n\
 -bif_help                     | Print the BIF help summary                   |\n\
-------------------------------+----------------------------------------------+\n\
 Note     : For more info on bootgen options, use the command                 |\n\
            bootgen -help <option>                                            |\n\
 Example  : bootgen -help efuseppkbits                                        |\n\
------------------------------------------------------------------------------+\n"

/******************************************************************************
 bif_help
******************************************************************************/
#define BIF_HELP    "\
------------------------------------------------------------------------------+\n\
                          COMMON BIF ATTRIBUTES                               |\n\
------------------------------------------------------------------------------+\n\
 Zynq / ZynqMP Architectures:                                                 |\n\
----------------------+-------------------------------------------------------+\n\
 [init]               | Register Initialization File (.ini file)              |\n\
----------------------+-------------------------------------------------------+\n\
 [aeskeyfile]         | AES Encryption Key File (.nky file)                   |\n\
----------------------+-------------------------------------------------------+\n\
 [ppkfile]            | Primary Public Key (.pub, .txt, .pk1)                 |\n\
----------------------+-------------------------------------------------------+\n\
 [pskfile]            | Primary Secret Key (.pem, .txt, .pk1)                 |\n\
----------------------+-------------------------------------------------------+\n\
 [spkfile]            | Secondary Public Key (.pub, .txt, .pk1)               |\n\
----------------------+-------------------------------------------------------+\n\
 [sskfile]            | Secondary Secret Key (.pem, .txt, .pk1)               |\n\
----------------------+-------------------------------------------------------+\n\
 [spksignature]       | SPK Signature File (.sig file)                        |\n\
----------------------+-------------------------------------------------------+\n\
 [headersignature]    | Header Signature File (.sig file)                     |\n\
----------------------+-------------------------------------------------------+\n\
 [bootimage]          | Boot Image File (in Xilinx Boot Image format)         |\n\
----------------------+-------------------------------------------------------+\n\
 ZynqMP Architecture:                                                         |\n\
----------------------+-------------------------------------------------------+\n\
 [pmufw_image]        | PMU Firmware Image (.elf file)                        |\n\
----------------------+-------------------------------------------------------+\n\
 [auth_params]        | Authentication Parameters                             |\n\
----------------------+-------------------------------------------------------+\n\
 [keysrc_encryption]  | Encryption Key Source                                 |\n\
----------------------+-------------------------------------------------------+\n\
 [boot_device]        | Secondary Boot Device                                 |\n\
----------------------+-------------------------------------------------------+\n\
 [fsbl_config]        | FSBL Configuration Parameters                         |\n\
----------------------+-------------------------------------------------------+\n\
 [split]              | Splits the image                                      |\n\
------------------------------------------------------------------------------+\n\
 [puf_file]           | PUF Helper Data File                                  |\n\
------------------------------------------------------------------------------+\n\
 [udf_bh]             | User Defined Feild of boot header                     |\n\
------------------------------------------------------------------------------+\n\
 [bh_key_iv]          | IV for Obfuscated or Black AES Encryption Key         |\n\
------------------------------------------------------------------------------+\n\
 [bh_keyfile]         | Obfuscated or Black AES Encryption Key                |\n\
------------------------------------------------------------------------------+\n\
 [bootvectors]        | Vector table for XIP                                  |\n\
------------------------------------------------------------------------------+\n\
 [bhsignature]        | Boot Header Signature File (.sig file)                |\n\
------------------------------------------------------------------------------|\n\
 [familykey]          | Family Key                                            |\n\
----------------------+-------------------------------------------------------+\n\
                       PARTITION BIF ATTRIBUTES                               |\n\
------------------------------------------------------------------------------+\n\
 Zynq / ZynqMP Architectures:                                                 |\n\
----------------------+-------------------------------------------------------+\n\
 bootloader           | First Stage Boot Loader                               |\n\
----------------------+-------------------------------------------------------+\n\
 encryption           | Encryption for the partition                          |\n\
----------------------+-------------------------------------------------------+\n\
 authentication       | Authentication for the partition                      |\n\
----------------------+-------------------------------------------------------+\n\
 checksum             | Checksum for the partition                            |\n\
----------------------+-------------------------------------------------------+\n\
 presign              | Partition Signature (.sig file)                       |\n\
----------------------+-------------------------------------------------------+\n\
 udf_data             | User Defined Data in the Authentication Certificate   |\n\
----------------------+-------------------------------------------------------+\n\
 xip_mode             | eXecute In Place                                      |\n\
----------------------+-------------------------------------------------------+\n\
 partition_owner      | Owner of the partition                                |\n\
----------------------+-------------------------------------------------------+\n\
 alignment            | Alignment for the partition                           |\n\
----------------------+-------------------------------------------------------+\n\
 offset               | Offset of the partition in the boot image             |\n\
----------------------+-------------------------------------------------------+\n\
 reserve              | Reserve space for the partition in the boot image     |\n\
----------------------+-------------------------------------------------------+\n\
 load                 | Load address of the partition in memory               |\n\
----------------------+-------------------------------------------------------+\n\
 startup              | Executable address of the partition in memory         |\n\
----------------------+-------------------------------------------------------+\n\
 ZynqMP Architecture:                                                         |\n\
----------------------+-------------------------------------------------------+\n\
 destination_cpu      | Destination CPU for the partition                     |\n\
----------------------+-------------------------------------------------------+\n\
 destination_device   | Destination Device for the partition                  |\n\
----------------------+-------------------------------------------------------+\n\
 trustzone            | Enable or Disable Trust Zone                          |\n\
----------------------+-------------------------------------------------------+\n\
 early_handoff        | Enable Early Handoff                                  |\n\
----------------------+-------------------------------------------------------+\n\
 hivec                | Enable Hivec                                          |\n\
----------------------+-------------------------------------------------------+\n\
 exception_level      | Exception Level of the partition                      |\n\
----------------------+-------------------------------------------------------+\n\
 blocks               | Blocks for Key Rolling Encryption                     |\n\
----------------------+-------------------------------------------------------|\n\
 pid                  + Partition id/Number                                   |\n\
----------------------+-------------------------------------------------------+\n\
 Note     : For more info on bif parameters, use the command                  |\n\
            bootgen -bif_help <bif parameter name>                            |\n\
 Example  : bootgen -bif_help aeskeyfile                                      |\n\
------------------------------------------------------------------------------+\n"
