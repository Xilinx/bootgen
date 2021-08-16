/******************************************************************************
* Copyright 2015-2020 Xilinx, Inc.
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
#define RDI_VERSION "2021.2"
#endif

#define PROGRAMNAME     "Xilinx Bootgen"
#define COPYRIGHT       "Copyright (c) 1995-2021 Xilinx, Inc. All rights reserved.\n"


/******************************************************************************
 help
******************************************************************************/
#define HELP            "\
------------------------------------------------------------------------------+\n\
                       COMMAND LINE OPTIONS                                   |\n\
------------------------------------------------------------------------------+\n\
 arch              - Xilinx Architecture                                      |\n\
 bif_help          - Print the BIF help summary                               |\n\
 dual_ospi_mode    - Generate 2 output files for Dual QSPI                    |\n\
 dual_qspi_mode    - Generate 2 output files for Dual QSPI                    |\n\
 dump              - Dumps components from PDI as bin files                   |\n\
 dump_dir          - Dumps components in specified directory                  |\n\
 efuseppkbits      - Generate PPK hash for e-fuse                             |\n\
 encrypt           - AES Key storage on device                                |\n\
 encryption_dump   - Generate encryption log file                             |\n\
 fill              - Fill byte for padding                                    |\n\
 generate_hashes   - Generate SHA hashes (PKCS#1v1.5)                         |\n\
 generate_keys     - Generate authentication keys                             |\n\
 help              - Print the help summary                                   |\n\
 image             - Input Boot Image File (.bif)                             |\n\
 log               - Generate log file                                        |\n\
 nonbooting        - Generate an intermediate boot image                      |\n\
 o                 - Output filename in MCS/BIN format                        |\n\
 p                 - Part name for generating nky file                        |\n\
 padimageheader    - Pad header tables                                        |\n\
 process_bitstream - Outputs bitstream in bin/mcs format                      |\n\
 read              - Dumps the header tables in human readable form           |\n\
 authenticatedjtag - Generate a Image to authenticate PSK and enable jtag     |\n\
 split             - Split partitions to diff files                           |\n\
 spksignature      - Generate SPK signature file                              |\n\
 verify            - Verify BootImage authentication                          |\n\
 verify_kdf        - Verify AES key generation                                |\n\
 w                 - Overwrite mode                                           |\n\
 zynqmpes1         - Generate boot image for ZynqMP ES1                       |\n\
------------------------------------------------------------------------------+\n\
 Note    : Some options are valid only for specific architectures.            |\n\
           Help for such options include a list of supported architectures.   |\n\
           If an option is supported for all architectures, this list is not  |\n\
           included in help                                                   |\n\
 Example : Supported architectures : zynqmp, versal                           |\n\
------------------------------------------------------------------------------+\n\
 For more information on bootgen options, use the command                     |\n\
           bootgen -help <option>                                             |\n\
 Example : bootgen -help efuseppkbits                                         |\n\
------------------------------------------------------------------------------+\n"

/******************************************************************************
 bif_help
******************************************************************************/
#define BIF_HELP    "\
------------------------------------------------------------------------------+\n\
 Attributes                                                                   |\n\
------------------------------------------------------------------------------+\n\
 aarch32_mode         - 32-bit mode execution for binary partitions           |\n\
 alignment            - Alignment for the partition                           |\n\
 big_endian           - Big endian partition                                  |\n\
 bootimage            - Boot Image File (in Xilinx Boot Image format)         |\n\
 bootloader           - First Stage Boot Loader                               |\n\
 bootvectors          - Vector table for XIP                                  |\n\
 boot_device          - Secondary Boot Device                                 |\n\
 boot_config          - Configuration for boot image                          |\n\
 checksum             - Checksum for the partition                            |\n\
 copy                 - Copy image from flash to memory address               |\n\
 core                 - CPU Core on which partition executes                  |\n\
 delay_handoff        - Handoff to the image is delayed                       |\n\
 delay_load           - Loading of the image is delayed                       |\n\
 destination_cpu      - CPU Core on which partition executes                  |\n\
 destination_device   - Destination Device for the partition                  |\n\
 early_handoff        - Enable Early Handoff                                  |\n\
 exception_level      - Exception Level of the partition                      |\n\
 file                 - Partition to be stitched to boot image                |\n\
 fsbl_config          - FSBL Configuration Parameters                         |\n\
 hivec                - Enable Hivec                                          |\n\
 id                   - ID for PDI, image, partition                          |\n\
 image                - To group partitions into an image                     |\n\
 init                 - Register Initialization File (.ini file)              |\n\
 load                 - Load address of the partition in memory               |\n\
 metaheader           - Configuration of Meta Header                          |\n\
 name                 - Name of the image                                     |\n\
 owner                - Owner of the partition                                |\n\
 offset               - Offset of the partition in the boot image             |\n\
 parent_id            - PDI ID of the parent PDI incase of partial PDI        |\n\
 partition            - Group all attributes related to partition             |\n\
 pid                  - Partition id/Number                                   |\n\
 pmcdata              - PMC CDO to be stitched to PLM                         |\n\
 pmufw_image          - PMU Firmware Image (.elf file)                        |\n\
 reserve              - Reserve space for the partition in the boot image     |\n\
 slr                  - SLR number                                            |\n\
 split                - Splits the image                                      |\n\
 startup              - Executable address of the partition in memory         |\n\
 trustzone            - Enable or Disable Trust Zone                          |\n\
 type                 - Type of partition                                     |\n\
 udf_bh               - User Defined Feild of boot header                     |\n\
 xip_mode             - eXecute In Place                                      |\n\
------------------------------------------------------------------------------+\n\
 Encryption attributes                                                        |\n\
------------------------------------------------------------------------------+\n\
 aeskeyfile           - AES Encryption Key File (.nky file)                   |\n\
 bbram_kek_iv         - IV for decrypting key in BBRAM KEK key                |\n\
 bh_key_iv            - IV for Obfuscated or Black AES Encryption Key         |\n\
 bh_keyfile           - Obfuscated or Black AES Encryption Key                |\n\
 bh_kek_iv            - IV for decrypting the Boot Header KEK                 |\n\
 efuse_kek_iv         - IV for decrypting the efuse KEK                       |\n\
 efuse_user_kek0_iv   - IV for decrypting the efuse User KEK0                 |\n\
 efuse_user_kek1_iv   - IV for decrypting the efuse User KEK1                 |\n\
 encryption           - Encryption for the partition                          |\n\
 familykey            - Family Key                                            |\n\
 keysrc               - Encryption Key Source                                 |\n\
 keysrc_encryption    - Encryption Key Source                                 |\n\
 puf_file             - PUF Helper Data File                                  |\n\
 blocks               - Blocks for Key Rolling Encryption                     |\n\
 dpacm_enable          - Enables DPA Counter Measure for Encryption           |\n\
------------------------------------------------------------------------------+\n\
 Authentication attributes                                                    |\n\
------------------------------------------------------------------------------+\n\
 authentication       - Authentication for the partition                      |\n\
 auth_params          - Authentication Parameters                             |\n\
 bhsignature          - Boot Header Signature File (.sig file)                |\n\
 headersignature      - Header Signature File (.sig file)                     |\n\
 ppkfile              - Primary Public Key (.pub, .txt, .pk1)                 |\n\
 presign              - Partition Signature (.sig file)                       |\n\
 pskfile              - Primary Secret Key (.pem, .txt, .pk1)                 |\n\
 revoke_id            - ID for revoking the keys                              |\n\
 spkfile              - Secondary Public Key (.pub, .txt, .pk1)               |\n\
 spksignature         - SPK Signature File (.sig file)                        |\n\
 sskfile              - Secondary Secret Key (.pem, .txt, .pk1)               |\n\
 udf_data             - User Defined Data in the Authentication Certificate   |\n\
------------------------------------------------------------------------------+\n\
 Note   :  Some attributes are valid only for specific architectures.         |\n\
           Help for such attributes include a list of supported architectures.|\n\
           If an attribute is supported for all architectures, this list is   |\n\
           not included in help                                               |\n\
 Example : Supported architectures : zynqmp, versal                           |\n\
------------------------------------------------------------------------------+\n\
 For more information on bootgen attributes, use the command                  |\n\
           bootgen -bif_help <option>                                         |\n\
 Example : bootgen -bif_help bootloader                                       |\n\
----------------------------------------------------------------------------- +\n"
