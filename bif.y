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
/* BIF (Boot Image Format)
 * Use bison to process this file into bif.tab.cpp
 */

%require "2.7"
%skeleton "lalr1.cc"
%defines
%define namespace "BIF"
%define parser_class_name "BisonParser"
%parse-param { BIF::FlexScanner& scanner }
%parse-param { Options& options}
%lex-param   { BIF::FlexScanner& scanner }
%locations
%initial-action
{
       // Initialize the initial location.
       @$.begin.filename = @$.end.filename = &scanner.filename;
};


%code requires {
       namespace BIF {
        class BisonScanner;
        class FlexScanner;
        class Parser;
    }
    #include "bootimage.h"
    #include "bifoptions.h"
}

%code {
    static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner);
    #include "options.h"
}


%{
#include <stdio.h>
#include "bootimage.h"
#include "authentication.h"
#include "encryption.h"
#include "checksum.h"
#include "bifscanner.h"
#include "parsing.h"
#include "imageheadertable-versal.h"

BifOptions* currentBifOptions;
PartitionBifOptions* currentPartitionBifOptions ;
ImageBifOptions* currentImageBifOptions;
%}


%start bif 


%union 
{
    uint32_t token;
    uint64_t number;
    char *string;
    Authentication::Type    authvalue_t;
    Encryption::Type        encrvalue_t;
    KeySource::Type         encrkeysrc_t;
    Core::Type              core_t;
    BhRsa::Type             bhrsa_t;
    AuthHash::Type          authhash_t;
    PufHdLoc::Type          pufhdloc_t;
    OptKey::Type            optkey_t;
    AuthOnly::Type          authonly_t;
    BootDevice::Type        bootdevice_t;
    DestinationDevice::Type destdevice_t;
    DestinationCPU::Type    destcpu_t;
    Checksum::Type          checksumvalue_t;
    PartitionOwner::Type    powner_t;
    PartitionType::Type     ptype_t;
    ExceptionLevel::Type    el_t;
    SplitMode::Type         splitmode_t;
    TrustZone::Type         trustzone_t;
    BifOptions*             bifoptions;
    PartitionBifOptions*    partitionBifOptions;
    DpaCM::Type             dpacm_t;
    SpkSelect::Type         spkselect_t;
}


%token                  OBRACE EBRACE
%token                  COMMA EQUAL COLON QUOTE SEMICOLON
%token                  OBRACKET EBRACKET
%token                  BOOTLOADER XIP_MODE EARLY_HANDOFF HIVEC
%token                  AUTHENTICATION ENCRYPTION CHECKSUM
%token                  PARTITION_OWNER PARTITION_TYPE PARTITION_NUM
%token                  BOOT_DEVICE DEST_DEVICE DEST_CPU ADDRESS
%token                  EXCEPTION_LEVEL TRUSTZONE 
%token                  ALIGNMENT OFFSET RESERVE_LEGACY RESERVE LOAD STARTUP BIGENDIAN A32_MODE
%token                  PPK_SELECT SPK_ID SPK_SELECT HEADER_AUTH REVOKE_ID
%token                  SPLIT_MODE SPLIT_FMT
%token                  BOOT USER STATIC NOAUTOSTART MULTIBOOT PROTECTED  
%token                  BLOCKS AUTHBLOCKS BOOTVECTORS
%token                  PRESIGN BIF_SECTION
%token                  UDF_DATA
%token                  MCS BIN
%token                  SLR_NUM
%token                  PARENT_ID ID_CODE EXT_ID_CODE BYPASS_IDCODE_CHECK A_HWROT S_HWROT UNIQUE_ID PARENT_UNIQUE_ID FUNCTION_ID
%token                  IMAGE ID NAME DELAY_HANDOFF DELAY_LOAD COPY
%token                  PARTITION PFILE
%token                  METAHEADER
%token <string>         WORD HEXWORD
%token <string>         FILENAME QFILENAME
%token <number>         NONE
%token <number>         DECVALUE HEXVALUE
%token <number>         KEYSRC_ENCRYPTION FSBL_CONFIG AUTH_PARAMS
%token <number>         AUTHJTAG_CONFIG DEVICE_DNA JTAG_TIMEOUT
%token <number>         PUF4KMODE SHUTTER SPLIT SMAP_WIDTH
%token <number>         PUF_HELPER_FILE BH_KEY_FILE BH_KEY_IV
%token <number>         BH_KEK_IV BBRAM_KEK_IV EFUSE_KEK_IV EFUSE_USER_KEK0_IV EFUSE_USER_KEK1_IV USER_KEYS
%token <number>         PMCDATA BOOTIMAGE UDF_BH INIT PMUFW_IMAGE
%token <number>         AES_KEY_FILE FAMILY_KEY
%token <number>         PPK_FILE PSK_FILE SPK_FILE SSK_FILE 
%token <number>         SPK_SIGNATURE_FILE BH_SIGNATURE_FILE HEADER_SIGNATURE_FILE
%token <authvalue_t>    AUTHVALUE
%token <encrvalue_t>    ENCRVALUE
%token <checksumvalue_t> CHECKSUMVALUE
%token <powner_t>       POWNERVALUE
%token <ptype_t>        PTYPEVALUE
%token <encrkeysrc_t>   KEY_SRC
%token <core_t>         CORE
%token <bhrsa_t>        BH_RSA
%token <authhash_t>     AUTH_HASH
%token <inthash_t>      INT_HASH
%token <pufhdloc_t>     PUFHD_LOC
%token <optkey_t>       OPT_KEY
%token <authonly_t>     AUTH_ONLY
%token <bootdevice_t>   BOOT_DEVICE_TYPE
%token <destdevice_t>   DEST_DEVICE_TYPE
%token <destcpu_t>      DEST_CPU_TYPE
%token <el_t>           EXCEPTION_LEVEL_TYPE
%token <trustzone_t>    TRUSTZONE_TYPE
%token <splitmode_t>    SPLITMODE
%token <dpacm_t>        DPA_CM
%token <spkselect_t>    SPKSELECT
%token                  OR XOR AND MULT DIVIDE MODULO PLUS MINUS LSHIFT RSHIFT
%left                   OR XOR AND MULT DIVIDE MODULO PLUS MINUS LSHIFT RSHIFT
%token                  NEGATION
%right                  NEGATION
%token                  LPAREN RPAREN ASTERISK
%right                  LPAREN
%left                   RPAREN
%left                   EQUAL



%type <string>          filename
%type <number>          number key_file rsa_key_file other_spec other_files other_file_attr
%type <authvalue_t>     authvalue
%type <encrvalue_t>     encrvalue 
%type <checksumvalue_t> checkvalue 
%type <powner_t>        pownervalue
%type <ptype_t>         ptypevalue
%type <encrkeysrc_t>    key_src
%type <core_t>          core
%type <bhrsa_t>         bh_rsa
%type <authhash_t>      auth_hash
%type <inthash_t>       int_hash
%type <pufhdloc_t>      pufhd_loc
%type <optkey_t>        opt_key
%type <authonly_t>      auth_only
%type <bootdevice_t>    boot_device_type
%type <destdevice_t>    dest_device_type
%type <destcpu_t>       dest_cpu_type
%type <el_t>            exception_level_type
%type <splitmode_t>     splitmode
%type <spkselect_t>     spkselect
%type <number>          expression  multiplicative_expression unary_expression additive_expression shift_expression
%type <number>          and_expression xor_expression
%type <bifoptions>      bifoptions
%type <dpacm_t>         dpa_cm



%%
bif                     :   group_list;

group_list              :   /* empty */ 
                        |   group_list bifoptions
                        ;
               
bifoptions              :   WORD                                                { currentBifOptions = new BifOptions(options.GetArchType(),$1); }
                            COLON 
                            OBRACE file_list EBRACE                             { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
                        ;
                        
file_list               :   /* empty */
                        |   file_list file_spec
                        |   file_list other_spec
                        |   file_list image_spec
                        |   file_list metahdr_spec
                        |   file_list new_pdi_spec
                        |   file_list new_file_spec
                        |   file_list partition_spec
                        ;

metahdr_spec            :   METAHEADER OBRACE                                    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType()); }
                            metahdr_attr_list EBRACE
                        ;

metahdr_attr_list       :  metahdr_attr
                        |   metahdr_attr COMMA metahdr_attr_list
                        |   metahdr_attr metahdr_attr_list
                        ;

metahdr_attr            :   /* empty */
                        |   ENCRYPTION EQUAL encrvalue                          { currentBifOptions->SetMetaHeaderEncryptType($3); }
                        |   KEYSRC_ENCRYPTION EQUAL key_src                     { currentBifOptions->SetMetaHeaderEncryptionKeySource($3); }
                        |   AES_KEY_FILE EQUAL filename                         { currentBifOptions->metaHdrAttributes.encrKeyFile = $3; }
                        |   AUTHENTICATION EQUAL authvalue                      { currentBifOptions->SetMetaHeaderAuthType($3); }
                        |   PPK_FILE EQUAL filename                             { currentBifOptions->metaHdrAttributes.ppk = $3; }
                        |   PSK_FILE EQUAL filename                             { currentBifOptions->metaHdrAttributes.psk = $3; }
                        |   SPK_FILE EQUAL filename                             { currentBifOptions->metaHdrAttributes.spk = $3; }
                        |   SSK_FILE EQUAL filename                             { currentBifOptions->metaHdrAttributes.ssk = $3; }
                        |   SPK_SIGNATURE_FILE EQUAL filename                   { currentBifOptions->metaHdrAttributes.spkSignature = $3; }
                        |   PRESIGN EQUAL filename                              { currentBifOptions->metaHdrAttributes.presign = $3; }
                        |   REVOKE_ID EQUAL expression                          { currentBifOptions->metaHdrAttributes.revokeId = $3; }
                        |   CHECKSUM EQUAL checkvalue                           { currentBifOptions->metaHdrAttributes.checksum = $3; }
                        |   DPA_CM                                              { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
                        |   BLOCKS EQUAL metahdr_blk
                        |   PUFHD_LOC                                           { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
                        ;

metahdr_blk             :   metahdr_blk_attr
                        |   metahdr_blk_attr SEMICOLON  metahdr_blk
                        ;

metahdr_blk_attr        :   expression                                          { currentPartitionBifOptions->SetEncryptionBlocks($1); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
                        |   expression LPAREN expression RPAREN                 { currentPartitionBifOptions->SetEncryptionBlocks($1, $3); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
                        |   expression LPAREN ASTERISK RPAREN                   { currentPartitionBifOptions->SetEncryptionBlocks($1, 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = $1; }
                        ;

new_pdi_spec            :   ID EQUAL expression                                 { currentBifOptions->SetPdiId($3); }
                        |   PARENT_ID EQUAL expression                          { currentBifOptions->SetParentId($3); }
                        |   ID_CODE EQUAL expression                            { currentBifOptions->SetIdCode($3); }
                        |   EXT_ID_CODE EQUAL expression                        { currentBifOptions->SetExtendedIdCode($3); }
                        |   other_file_attr EQUAL filename                      { currentBifOptions->AddFiles($1, $3); }
                        |   KEYSRC_ENCRYPTION EQUAL key_src                     { currentBifOptions->SetEncryptionKeySource($3); }
                        ;

image_spec              :   image_list
                        |   image_spec image_list
                        ;

image_list              :   IMAGE OBRACE                                        { currentImageBifOptions = new ImageBifOptions(); }
                            image_content EBRACE                                { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions); }
                        ;

image_content           :   /* empty */
                        |   image_content image_attributes_list
                        |   image_content file_spec
                        |   image_content partition_spec
                        ;

image_attributes_list   :   image_attributes
                        |   image_attributes COMMA image_attributes_list
                        |   image_attributes image_attributes_list
                        ;

image_attributes        :   ID EQUAL expression                                 { currentImageBifOptions->SetImageId($3); }
                        |   NAME EQUAL WORD                                     { currentImageBifOptions->SetImageName($3); }
                        |   DELAY_HANDOFF                                       { currentImageBifOptions->SetDelayHandoff(true); }
                        |   DELAY_LOAD                                          { currentImageBifOptions->SetDelayLoad(true); }
                        |   COPY EQUAL expression                               { currentImageBifOptions->SetMemCopyAddress($3); }
                        |   PARTITION_TYPE EQUAL ptypevalue                     { currentImageBifOptions->SetImageType($3); }
                        |   UNIQUE_ID EQUAL expression                          { currentImageBifOptions->SetUniqueId($3); }
                        |   PARENT_UNIQUE_ID EQUAL expression                   { currentImageBifOptions->SetParentUniqueId($3); }
                        |   FUNCTION_ID EQUAL expression                        { currentImageBifOptions->SetFunctionId($3); }

                        ;

partition_spec          :   PARTITION partition_content
                        |   partition_content
                        ;

partition_content       :   /* empty */
                        |   partition_content file_spec
                        |   partition_content new_file_spec
                        ;

other_spec              :   OBRACKET KEYSRC_ENCRYPTION EBRACKET key_src         { if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported in VERSAL architecture.\n\t   Please see 'bootgen -arch versal -bif_help keysrc'");
                                                                                  currentBifOptions->SetEncryptionKeySource($4); options.SetEncryptedKeySource($4); }
                        |   OBRACKET FSBL_CONFIG                                { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
                            EBRACKET fsbl_attr_list
                        |   FSBL_CONFIG OBRACE fsbl_attr_list EBRACE
                        |   BOOT_DEVICE OBRACE sec_boot_attr_list EBRACE
                        |   OBRACKET BOOT_DEVICE EBRACKET boot_device_type      { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                    if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); 
                                                                                    currentBifOptions->SetBootDevice($4); }
                        |   BOOT_DEVICE EQUAL boot_device_type                  { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); }
                        |   OBRACKET AUTH_PARAMS EBRACKET auth_parameters
                        |   OBRACKET SPLIT EBRACKET split_options
                        |   OBRACKET BOOTVECTORS EBRACKET bootvectors_list
                        |   AUTHJTAG_CONFIG OBRACE authjtag_attr_list EBRACE
                        ;

sec_boot_attr_list      :   sec_boot_attr
                        |   sec_boot_attr COMMA sec_boot_attr_list
                        ;

sec_boot_attr           :   boot_device_type                                    { currentBifOptions->SetBootDevice($1); }
                        |   ADDRESS EQUAL expression                            { currentBifOptions->SetBootDeviceAddress($3); }
                        ;

fsbl_attr_list          :   fsbl_attr
                        |   fsbl_attr COMMA fsbl_attr_list
                        ;

authjtag_attr_list      :   authjtag_attr
                        |   authjtag_attr COMMA authjtag_attr_list
                        |   authjtag_attr authjtag_attr_list
                        ;

authjtag_attr           :   REVOKE_ID EQUAL expression           { currentBifOptions->SetAuthJtagRevokeID($3); }
                        |   DEVICE_DNA EQUAL HEXWORD             { currentBifOptions->SetAuthJtagDeviceDna($3); }
                        |   JTAG_TIMEOUT EQUAL expression        { currentBifOptions->SetAuthJtagTimeOut($3); }
                        ;

fsbl_attr               :   core                                                { currentBifOptions->SetCore($1);
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }

                        |   bh_rsa                                              { currentBifOptions->SetBhRsa($1); }

                        |   auth_hash                                           { LOG_ERROR("Authentication using SHA2 is no more supported."); }

                        |   int_hash                                            { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }

                        |   pufhd_loc                                           { currentBifOptions->SetPufHdLoc($1); }

                        |   auth_only                                           { currentBifOptions->SetAuthOnly($1); }

                        |   opt_key                                             { currentBifOptions->SetOptKey($1); }

                        |   PUF4KMODE                                           { currentBifOptions->SetPufMode(PufMode::PUF4K); }

                        |   SHUTTER EQUAL expression                            { currentBifOptions->SetShutterValue($3); }

                        |   dpa_cm                                              {  if(options.GetArchType() != Arch::VERSAL)
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t\t'dpacm_enable' is supported only in VERSAL architecture");
                                                                                   if(options.GetArchType() == Arch::VERSAL)
                                                                                     LOG_WARNING("boot_config { dpacm_enable } will be deprecated. 'dpacm_enable' should be specified along with the partition. \n            See 'bootgen -bif_help dpacm_enable' for more info.");
                                                                                   currentBifOptions->SetDpaCM($1);
                                                                                }
                        |   SMAP_WIDTH EQUAL expression                         { if(($3 != 8) && ($3 !=16) && ($3 != 32) && ($3 != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->smapWidth = $3;
                                                                                }
                        |   BYPASS_IDCODE_CHECK                                 { currentBifOptions->SetBypassIdcodeFlag(true); }
                        |   A_HWROT                                             { currentBifOptions->SetAHwRoTFlag(true); }
                        |   S_HWROT                                             { currentBifOptions->SetSHwRoTFlag(true); }
                        ;

file_spec               :   OBRACKET                                            { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
                            attribute_list EBRACKET 
                            filename                                            { currentPartitionBifOptions->filename = $5;
                                                                                  currentPartitionBifOptions->filelist.push_back($5);
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
                        |   filename                                            { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType());
                                                                                  currentPartitionBifOptions->filename = $1; 
                                                                                  currentPartitionBifOptions->filelist.push_back($1);
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                };
new_file_spec           :   OBRACE                                              { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
                            new_attribute_list EBRACE
                        ;


new_attribute_list      :   attribute
                        |   new_attribute
                        |   new_attribute COMMA new_attribute_list
                        |   attribute COMMA new_attribute_list
                        |   new_attribute new_attribute_list
                        |   attribute new_attribute_list
                        ;

new_attribute           :   PFILE EQUAL filename                                { currentPartitionBifOptions->filename = $3;
                                                                                  currentPartitionBifOptions->filelist.push_back($3);
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
                        |   ID EQUAL expression                                 { currentPartitionBifOptions->partitionId = $3; }
                        |   PARTITION_TYPE EQUAL boolattr
                        |   PARTITION_TYPE EQUAL PMCDATA                        { currentPartitionBifOptions->fileType = $3; }
                        |   BIF_SECTION EQUAL WORD                                { currentPartitionBifOptions->bifSection = $3;
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), $3);
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
                        ;

attribute_list          :   attribute 
                        |   attribute COMMA attribute_list
                        ;

attribute               :   boolattr
                        |   optattr
                        |   numattr
                        |   fileattr
                        |   blocksattr_list
                        ;

blocksattr_list         :   BLOCKS EQUAL blocksattr
                        |   AUTHBLOCKS EQUAL authblockattr
                        ;

blocksattr              :   blockattr
                        |   blockattr SEMICOLON blocksattr
                        ;

bootvectors_list        :   bootvector
                        |   bootvector COMMA bootvectors_list
                        ;

bootvector              :   expression                                          { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] only supported in ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray($1); }
                        ;

authblockattr           :   expression                                          { currentPartitionBifOptions->SetAuthBlockAttr($1); }

boolattr                :   BOOTLOADER                                          { currentPartitionBifOptions->bootloader = true;}
                        |   BOOT                                                { currentPartitionBifOptions->boot = true;}
                        |   USER                                                { currentPartitionBifOptions->user = true;}
                        |   STATIC                                              { currentPartitionBifOptions->Static = true;}
                        |   NOAUTOSTART                                         { currentPartitionBifOptions->noautostart = true;}
                        |   MULTIBOOT                                           { currentPartitionBifOptions->multiboot = true;}
                        |   PROTECTED                                           { currentPartitionBifOptions->Protected = true;}
                        |   EARLY_HANDOFF                                       { currentPartitionBifOptions->SetEarlyHandoff(true); }
                        |   HIVEC                                               { currentPartitionBifOptions->SetHivec(true); }
                        |   XIP_MODE                                            { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
                        |   INIT                                                { currentPartitionBifOptions->fileType = $1; }
                        |   BOOTIMAGE                                           { currentPartitionBifOptions->bootImage = true; }
                        |   key_file                                            { currentPartitionBifOptions->fileType = $1; }
                        |   other_files                                         { currentPartitionBifOptions->fileType = $1; }
                        |   ptypevalue                                          { currentPartitionBifOptions->SetPartitionType($1); }
                        ;

trustzone_type          :   TRUSTZONE                                           { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
                        |   TRUSTZONE EQUAL TRUSTZONE_TYPE                      { currentPartitionBifOptions->SetTrustZone($3); }
                        ;

blockattr               :   expression                                          { currentPartitionBifOptions->SetEncryptionBlocks($1); }
                        |   expression LPAREN expression RPAREN                 { currentPartitionBifOptions->SetEncryptionBlocks($1, $3); }
                        |   expression LPAREN ASTERISK RPAREN                   { currentPartitionBifOptions->SetEncryptionBlocks($1, 0); }

optattr                 :   AUTHENTICATION EQUAL authvalue                      { currentPartitionBifOptions->SetAuthType($3); }
                        |   ENCRYPTION EQUAL encrvalue                          { currentPartitionBifOptions->SetEncryptType($3); }
                        |   CHECKSUM EQUAL checkvalue                           { currentPartitionBifOptions->SetChecksumType($3); }
                        |   PARTITION_OWNER EQUAL pownervalue                   { currentPartitionBifOptions->SetOwnerType($3); }
                        |   DEST_CPU EQUAL dest_cpu_type                        { currentPartitionBifOptions->SetDestCpu($3); }
                        |   DEST_DEVICE EQUAL dest_device_type                  { currentPartitionBifOptions->SetDestDevice($3);  }
                        |   EXCEPTION_LEVEL EQUAL exception_level_type          { currentPartitionBifOptions->SetExceptionLevel($3); }
                        |   AES_KEY_FILE EQUAL filename                         { currentPartitionBifOptions->SetAesKeyFile($3); }
                        |   PPK_FILE EQUAL filename                             { currentPartitionBifOptions->ppkFile = ($3); }
                        |   PSK_FILE EQUAL filename                             { currentPartitionBifOptions->pskFile = ($3); }
                        |   SPK_FILE EQUAL filename                             { currentPartitionBifOptions->spkFile = ($3); }
                        |   SSK_FILE EQUAL filename                             { currentPartitionBifOptions->sskFile = ($3); }
                        |   SPK_SELECT EQUAL spkselect                          { currentPartitionBifOptions->spkSelect =($3); currentPartitionBifOptions->spkSelLocal = true; }
                        |   SPK_ID EQUAL expression                             { currentPartitionBifOptions->SetSpkId($3); }
                        |   SPK_SIGNATURE_FILE EQUAL filename                   { currentPartitionBifOptions->spkSignatureFile = ($3); }
                        |   trustzone_type
                        |   PARTITION_TYPE EQUAL ptypevalue                     { currentPartitionBifOptions->SetPartitionType($3); }
                        |   KEYSRC_ENCRYPTION EQUAL key_src                     { currentPartitionBifOptions->SetEncryptionKeySource($3); }
                        |   REVOKE_ID EQUAL expression                          { currentPartitionBifOptions->SetRevokeId($3); }
                        |   DPA_CM                                              { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
                        |   SLR_NUM EQUAL expression                            { currentPartitionBifOptions->SetSlrNum($3); }
                        |   PUFHD_LOC                                           { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
                        ;

other_file_attr         :   INIT
                        |   key_file
                        |   BH_KEK_IV
                        |   BBRAM_KEK_IV
                        |   EFUSE_KEK_IV
                        |   EFUSE_USER_KEK0_IV
                        |   EFUSE_USER_KEK1_IV
                        |   USER_KEYS
                        ;

authvalue               :   NONE                                                { $$ = ::Authentication::None;}
                        |   AUTHVALUE 
                        ;

encrvalue               :   NONE                                                { $$ = ::Encryption::None;}
                        |   ENCRVALUE 
                        ;

checkvalue              :   NONE                                                { $$ = ::Checksum::None;}
                        |   CHECKSUMVALUE
                        ;

pownervalue             :   POWNERVALUE
                        ;

ptypevalue              :   PTYPEVALUE
                        ;

key_src                 :   KEY_SRC
                        ;
                        
core                    :   CORE
                        ;

bh_rsa                  :   BH_RSA
                        ;

dpa_cm                  :   DPA_CM
                        ;

auth_hash               :   AUTH_HASH
                        ;

int_hash                :   INT_HASH
                        ;

pufhd_loc               :   PUFHD_LOC
                        ;

opt_key                 :   OPT_KEY
                        ;

auth_only               :   AUTH_ONLY
                        ;

boot_device_type        :   BOOT_DEVICE_TYPE
                        ;

dest_cpu_type           :   DEST_CPU_TYPE
                        ;

dest_device_type        :   DEST_DEVICE_TYPE
                        ;

exception_level_type    :   EXCEPTION_LEVEL_TYPE
                        ;

numattr                 :   ALIGNMENT EQUAL expression                          { currentPartitionBifOptions->alignment = $3; }
                        |   OFFSET EQUAL expression                             { currentPartitionBifOptions->offset = $3; }
                        |   RESERVE_LEGACY EQUAL expression                     { currentPartitionBifOptions->SetReserveLength($3, false); }
                        |   RESERVE EQUAL expression                            { currentPartitionBifOptions->SetReserveLength($3, true); }
                        |   LOAD EQUAL expression                               { currentPartitionBifOptions->load = $3; }
                        |   STARTUP EQUAL expression                            { currentPartitionBifOptions->startup = $3; }
                        |   BIGENDIAN                                           { currentPartitionBifOptions->bigEndian = true; }
                        |   A32_MODE                                            { currentPartitionBifOptions->a32Mode = true; }
                        |   PARTITION_NUM EQUAL expression                      { currentPartitionBifOptions->pid = $3; }
                        ;

fileattr                :   PRESIGN EQUAL filename                              { currentPartitionBifOptions->presignFile = $3; }
                        |   UDF_DATA EQUAL filename                             { currentPartitionBifOptions->SetUdfDataFile($3); }
                        ;

key_file                :   AES_KEY_FILE
                        |   rsa_key_file
                        |   SPK_SIGNATURE_FILE
                        |   BH_SIGNATURE_FILE
                        |   HEADER_SIGNATURE_FILE
                        |   BH_KEY_FILE
                        |   PUF_HELPER_FILE
                        |   BH_KEY_IV
                        |   FAMILY_KEY
                        ;

rsa_key_file            :   PPK_FILE
                        |   PSK_FILE
                        |   SPK_FILE
                        |   SSK_FILE
                        ;

other_files             :   PMUFW_IMAGE 
                        |   PMCDATA
                        |   UDF_BH
                        ;

auth_parameters         :   PPK_SELECT EQUAL expression                         { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection($3); }
                        |   SPK_SELECT EQUAL spkselect                          { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection($3); }
                        |   SPK_ID EQUAL expression                             { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId($3);  }
                        |   HEADER_AUTH                                         { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
                        |   auth_parameters SEMICOLON auth_parameters
                        ;

spkselect               :   SPKSELECT
                        ;

split_options           :   SPLIT_MODE EQUAL splitmode                          { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitMode($3); }
                        |   SPLIT_FMT EQUAL splitfmt
                        |   split_options COMMA split_options
                        ;

splitmode               :   SPLITMODE
                        ;

splitfmt                :   MCS                                                 { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); } 
                        |   BIN                                                 { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); } 
                        ;


filename                :   FILENAME 
                        |   QFILENAME
                        ;

number                  :   HEXVALUE 
                        |   DECVALUE 
                        |   LPAREN expression RPAREN                            { $$ = $2; };




// highest precendence
unary_expression        :   number
                        |   PLUS unary_expression                               {$$ =  $2;     *options.debugstr << $$ << "    + " << $2 << std::endl;}
                        |   NEGATION unary_expression                           {$$ = ~$2;     *options.debugstr << $$ << "    ~ " << $2 << std::endl;}
                        ;

multiplicative_expression
                        :   unary_expression
                        |   multiplicative_expression MULT unary_expression     {$$ = $1 *  $3; *options.debugstr << $$ << " = " << $1  << " + " << $3 << std::endl;}
                        |   multiplicative_expression DIVIDE unary_expression   {$$ = $1 /  $3; *options.debugstr << $$ << " = " << $1  << " / " << $3 << std::endl;}
                        |   multiplicative_expression MODULO unary_expression   {$$ = $1 %  $3; *options.debugstr << $$ << " = " << $1  << " % " << $3 << std::endl;}
                        ;

additive_expression     :   multiplicative_expression
                        |   additive_expression PLUS multiplicative_expression  {$$ = $1 +  $3;*options.debugstr << $$ << " = " << $1  << " + " << $3 << std::endl;}
                        |   additive_expression MINUS multiplicative_expression {$$ = $1 -  $3;*options.debugstr << $$ << " = " << $1  << " - " << $3 << std::endl;}
                        ;

shift_expression        :   additive_expression
                        |   shift_expression LSHIFT additive_expression         {$$ = $1 << $3;*options.debugstr << $$ << " = " << $1  << " << " << $3 << std::endl;}
                        |   shift_expression RSHIFT additive_expression         {$$ = $1 >> $3;*options.debugstr << $$ << " = " << $1  << " >> " << $3 << std::endl;}
                        ;

and_expression          :   shift_expression
                        |   and_expression AND shift_expression                 {$$ = $1 &  $3;*options.debugstr << $$ << " = " << $1  << " & " << $3 << std::endl;}
                        ;

xor_expression          :   and_expression
                        |   xor_expression XOR and_expression                   {$$ = $1 ^  $3;*options.debugstr << $$ << " = " << $1  << " ^ " << $3 << std::endl;}
                        ;

// lowest precendence
expression              :   xor_expression
                        |   expression OR xor_expression                        {$$ = $1 |  $3;*options.debugstr << $$ << " = " << $1  << " | " << $3 << std::endl;}
                        ;



%%
void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
