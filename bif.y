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

BifOptions* currentBifOptions;
PartitionBifOptions* currentPartitionBifOptions ;
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
    ExceptionLevel::Type    el_t;
    SplitMode::Type         splitmode_t;
    TrustZone::Type         trustzone_t;
    BifOptions*             bifoptions;
    PartitionBifOptions*    partitionBifOptions;
    SpkSelect::Type         spkselect_t;
}


%token                  OBRACE EBRACE
%token                  COMMA EQUAL COLON QUOTE SEMICOLON
%token                  OBRACKET EBRACKET
%token                  BOOTLOADER XIP_MODE EARLY_HANDOFF HIVEC
%token                  AUTHENTICATION ENCRYPTION CHECKSUM
%token                  PARTITION_OWNER PARTITION_TYPE PARTITION_NUM
%token                  BOOT_DEVICE DEST_DEVICE DEST_CPU
%token                  EXCEPTION_LEVEL TRUSTZONE 
%token                  ALIGNMENT OFFSET RESERVE LOAD STARTUP BIGENDIAN A32_MODE
%token                  PPK_SELECT SPK_ID SPK_SELECT HEADER_AUTH 
%token                  SPLIT_MODE SPLIT_FMT
%token                  BOOT USER STATIC NOAUTOSTART MULTIBOOT PROTECTED  
%token                  BLOCKS AUTHBLOCKS BOOTVECTORS
%token                  PRESIGN
%token                  UDF_DATA
%token                  MCS BIN
%token                  IMAGE ID NAME
%token                  PFILE

%token <string>         WORD
%token <string>         FILENAME QFILENAME

%token <number>         NONE 
%token <number>         DECVALUE HEXVALUE
%token <number>	        KEYSRC_ENCRYPTION FSBL_CONFIG AUTH_PARAMS 
%token <number>	        PUF4KMODE SHUTTER SPLIT
%token <number>	        PUF_HELPER_FILE BH_KEY_FILE BH_KEY_IV
%token <number>	        BOOTIMAGE UDF_BH INIT PMUFW_IMAGE
%token <number>         AES_KEY_FILE FAMILY_KEY 
%token <number>         PPK_FILE PSK_FILE SPK_FILE SSK_FILE 
%token <number>         SPK_SIGNATURE_FILE BH_SIGNATURE_FILE HEADER_SIGNATURE_FILE


%token <authvalue_t>    AUTHVALUE
%token <encrvalue_t>    ENCRVALUE
%token <checksumvalue_t> CHECKSUMVALUE
%token <powner_t>       POWNERVALUE
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
%type <number>          number key_file rsa_key_file other_spec other_files
%type <authvalue_t>     authvalue
%type <encrvalue_t>     encrvalue 
%type <checksumvalue_t> checkvalue 
%type <powner_t>        pownervalue
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

%%
bif             : group_list;

group_list      : /* empty */ 
                | group_list bifoptions  
                ;
               
bifoptions      : WORD                      { currentBifOptions = new BifOptions(options.GetArchType(),$1); }
                  COLON 
                  OBRACE file_list EBRACE   { options.bifOptions=currentBifOptions; }
                ;
                
file_list       : /* empty */
                | file_list file_spec
                | file_list other_spec
                ;

other_spec      : OBRACKET KEYSRC_ENCRYPTION EBRACKET key_src       { currentBifOptions->SetEncryptionKeySource($4); options.SetEncryptedKeySource($4); }
                | OBRACKET FSBL_CONFIG                              { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                        LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
                  EBRACKET fsbl_attr_list
                | OBRACKET BOOT_DEVICE EBRACKET boot_device_type    { if(options.GetArchType() != Arch::ZYNQMP) {
                                                                          LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); }
                                                                      currentBifOptions->SetBootDevice($4); 
                                                                    }
                | OBRACKET AUTH_PARAMS EBRACKET auth_parameters
                | OBRACKET SPLIT EBRACKET split_options
                | OBRACKET BOOTVECTORS EBRACKET bootvectors_list
                ;
                
fsbl_attr_list  : fsbl_attr
                | fsbl_attr COMMA fsbl_attr_list
                ;

fsbl_attr       : core          { currentBifOptions->SetCore($1);
                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }

                | bh_rsa        { currentBifOptions->SetBhRsa($1); }

                | auth_hash     { LOG_ERROR("Authentication using SHA2 is no more supported."); }

                | int_hash      { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }

                | pufhd_loc     { currentBifOptions->SetPufHdLoc($1); }

                | auth_only     { currentBifOptions->SetAuthOnly($1); }

                | opt_key       { currentBifOptions->SetOptKey($1); }

                | PUF4KMODE     { currentBifOptions->SetPufMode(PufMode::PUF4K); }

                | SHUTTER EQUAL expression
                                { currentBifOptions->SetShutterValue($3); }
                ;


file_spec       : OBRACKET      { currentPartitionBifOptions = new PartitionBifOptions();
                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
                   attribute_list EBRACKET 
                   filename     { currentPartitionBifOptions->filename = $5; 
                                  currentBifOptions->Add(currentPartitionBifOptions);
                                } 
                | filename      { currentPartitionBifOptions = new PartitionBifOptions();
                                  currentPartitionBifOptions->SetArchType(options.GetArchType());
                                  currentPartitionBifOptions->filename = $1; 
                                  currentBifOptions->Add(currentPartitionBifOptions);
                                };

attribute_list  : attribute 
                | attribute COMMA attribute_list
                ;
                
attribute       : boolattr 
                | optattr  
                | numattr 
                | fileattr
                | blocksattr_list
                ;

blocksattr_list : BLOCKS EQUAL blocksattr
                | AUTHBLOCKS EQUAL authblockattr
                ;       
                
blocksattr      : blockattr
                | blockattr SEMICOLON blocksattr
                ;     

bootvectors_list: bootvector
                | bootvector COMMA bootvectors_list
                ;

bootvector      : expression    { if(options.GetArchType() != Arch::ZYNQMP) 
                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] not supported in ZYNQ architecture");
                                  currentBifOptions->SetBootVectorArray($1); }
                ;

authblockattr   : expression    { currentPartitionBifOptions->SetAuthBlockAttr($1); }

boolattr        : BOOTLOADER    { currentPartitionBifOptions->bootloader = true;}
                | BOOT          { currentPartitionBifOptions->boot = true;}        
                | USER          { currentPartitionBifOptions->user = true;}        
                | STATIC        { currentPartitionBifOptions->Static = true;}     
                | NOAUTOSTART   { currentPartitionBifOptions->noautostart = true;} 
                | MULTIBOOT     { currentPartitionBifOptions->multiboot = true;}  
                | PROTECTED     { currentPartitionBifOptions->Protected = true;} 
                | EARLY_HANDOFF { currentPartitionBifOptions->SetEarlyHandoff(true); }
                | HIVEC         { currentPartitionBifOptions->SetHivec(true); }
                | XIP_MODE      { if(currentPartitionBifOptions->bootloader!=true) 
                                    LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                  currentBifOptions->SetXipMode();}  
                | INIT          { currentPartitionBifOptions->fileType = $1; }
                | BOOTIMAGE     { currentPartitionBifOptions->bootImage = true; }
                | key_file      { currentPartitionBifOptions->fileType = $1; }
                | other_files   { currentPartitionBifOptions->fileType = $1; }
                ;

trustzone_type  : TRUSTZONE                         { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
                | TRUSTZONE EQUAL TRUSTZONE_TYPE    { currentPartitionBifOptions->SetTrustZone($3); }
                ;

blockattr       : expression                            { currentPartitionBifOptions->SetEncryptionBlocks($1); }
                | expression LPAREN expression RPAREN   { currentPartitionBifOptions->SetEncryptionBlocks($1, $3); }
                | expression LPAREN ASTERISK RPAREN     { currentPartitionBifOptions->SetEncryptionBlocks($1, 0); }

optattr         : AUTHENTICATION        EQUAL authvalue             { currentPartitionBifOptions->SetAuthType($3); }
                | ENCRYPTION            EQUAL encrvalue             { currentPartitionBifOptions->SetEncryptType($3); }
                | CHECKSUM              EQUAL checkvalue            { currentPartitionBifOptions->SetChecksumType($3); }
                | PARTITION_OWNER       EQUAL pownervalue           { currentPartitionBifOptions->SetOwnerType($3); }
                | DEST_CPU              EQUAL dest_cpu_type         { currentPartitionBifOptions->SetDestCpu($3); }
                | DEST_DEVICE           EQUAL dest_device_type      { currentPartitionBifOptions->SetDestDevice($3);  }
                | EXCEPTION_LEVEL       EQUAL exception_level_type  { currentPartitionBifOptions->SetExceptionLevel($3); }
                | AES_KEY_FILE          EQUAL filename              { currentPartitionBifOptions->aesKeyFile = $3; }
                | SPK_FILE              EQUAL filename              { currentPartitionBifOptions->spkFile=($3); }
                | SSK_FILE              EQUAL filename              { currentPartitionBifOptions->sskFile=($3); }
                | SPK_SELECT            EQUAL spkselect             { currentPartitionBifOptions->spkSelect=($3); currentPartitionBifOptions->spkSelLocal = true; }
                | SPK_ID                EQUAL expression            { currentPartitionBifOptions->spkId=($3); currentPartitionBifOptions->spkIdLocal = true; }
                | SPK_SIGNATURE_FILE    EQUAL filename              { currentPartitionBifOptions->spkSignatureFile=($3); }
                | trustzone_type
                ;

authvalue       : NONE      { $$ = ::Authentication::None;}
                | AUTHVALUE 
                ;
                
encrvalue       : NONE      { $$ = ::Encryption::None;}
                | ENCRVALUE 
                ;
                
checkvalue      : NONE      { $$ = ::Checksum::None;}
                | CHECKSUMVALUE
                ;

pownervalue     : POWNERVALUE
                ;

key_src         : KEY_SRC
                ;
                
core            : CORE
                ;

bh_rsa          : BH_RSA
                ;

auth_hash       : AUTH_HASH
                ;

int_hash        : INT_HASH
                ;

pufhd_loc       : PUFHD_LOC
                ;

opt_key         : OPT_KEY
                ;

auth_only       : AUTH_ONLY
                ;

boot_device_type: BOOT_DEVICE_TYPE
                ;

dest_cpu_type   : DEST_CPU_TYPE
                ;

dest_device_type: DEST_DEVICE_TYPE
                ;
                
exception_level_type    : EXCEPTION_LEVEL_TYPE
                        ;
                
numattr         : ALIGNMENT EQUAL expression        { currentPartitionBifOptions->alignment= $3; }
                | OFFSET    EQUAL expression        { currentPartitionBifOptions->offset   = $3; }
                | RESERVE   EQUAL expression        { currentPartitionBifOptions->reserve  = $3; }
                | LOAD      EQUAL expression        { currentPartitionBifOptions->load     = $3; }
                | STARTUP   EQUAL expression        { currentPartitionBifOptions->startup  = $3; }
                | BIGENDIAN                         { currentPartitionBifOptions->bigEndian  = true; }
                | A32_MODE                          { currentPartitionBifOptions->a32Mode  = true; }
                | PARTITION_NUM EQUAL expression    { currentPartitionBifOptions->pid  = $3; }
                ;

fileattr        : PRESIGN   EQUAL filename          { currentPartitionBifOptions->presignFile = $3; }
                | UDF_DATA  EQUAL filename          { currentPartitionBifOptions->udfDataFile = $3; }
                ;

key_file        : AES_KEY_FILE
                | rsa_key_file
                | SPK_SIGNATURE_FILE
                | BH_SIGNATURE_FILE
                | HEADER_SIGNATURE_FILE
                | BH_KEY_FILE
                | PUF_HELPER_FILE
                | BH_KEY_IV
                | FAMILY_KEY
                ;

rsa_key_file    : PPK_FILE
                | PSK_FILE
                | SPK_FILE
                | SSK_FILE
                ;

other_files     : PMUFW_IMAGE 
                | UDF_BH
                ;

auth_parameters : PPK_SELECT EQUAL expression           { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetPPKSelection($3); }
                | SPK_SELECT EQUAL spkselect            { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetSPKSelection($3); }
                | SPK_ID EQUAL expression               { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetSpkId($3);  }
                | HEADER_AUTH                           { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetHeaderAuthentication(); }
                | auth_parameters SEMICOLON auth_parameters
                ;

spkselect	: SPKSELECT
                ;

split_options   : SPLIT_MODE EQUAL splitmode    { if(options.GetArchType() != Arch::ZYNQMP)
                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                  currentBifOptions->SetSplitMode($3); }
                | SPLIT_FMT EQUAL splitfmt
                | split_options COMMA split_options
                ;

splitmode       : SPLITMODE
                ;

splitfmt        : MCS           { if(options.GetArchType() != Arch::ZYNQMP)
                                    LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                  currentBifOptions->SetSplitFmt(File::MCS); } 
                | BIN           { if(options.GetArchType() != Arch::ZYNQMP)
                                    LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                  currentBifOptions->SetSplitFmt(File::BIN); } 
                ;


filename        : FILENAME 
                | QFILENAME
                ;

number          : HEXVALUE 
                | DECVALUE 
                | LPAREN expression RPAREN { $$ = $2; };





// highest precendence
unary_expression
        : number
        | PLUS     unary_expression  {$$ =  $2;     *options.debugstr << $$ << "    + " << $2 << std::endl;}
        | NEGATION unary_expression  {$$ = ~$2;     *options.debugstr << $$ << "    ~ " << $2 << std::endl;}
        ;

multiplicative_expression
        : unary_expression
        | multiplicative_expression MULT   unary_expression {$$ = $1 *  $3; *options.debugstr << $$ << " = " << $1  << " + " << $3 << std::endl;}
        | multiplicative_expression DIVIDE unary_expression {$$ = $1 /  $3; *options.debugstr << $$ << " = " << $1  << " / " << $3 << std::endl;}
        | multiplicative_expression MODULO unary_expression {$$ = $1 %  $3; *options.debugstr << $$ << " = " << $1  << " % " << $3 << std::endl;}
        ;

additive_expression
        : multiplicative_expression
        | additive_expression  PLUS multiplicative_expression {$$ = $1 +  $3;*options.debugstr << $$ << " = " << $1  << " + " << $3 << std::endl;}
        | additive_expression MINUS multiplicative_expression {$$ = $1 -  $3;*options.debugstr << $$ << " = " << $1  << " - " << $3 << std::endl;}
        ;

shift_expression
        : additive_expression
        | shift_expression LSHIFT additive_expression {$$ = $1 << $3;*options.debugstr << $$ << " = " << $1  << " << " << $3 << std::endl;}
        | shift_expression RSHIFT additive_expression {$$ = $1 >> $3;*options.debugstr << $$ << " = " << $1  << " >> " << $3 << std::endl;}
        ;
           
and_expression
        : shift_expression
        | and_expression AND shift_expression  {$$ = $1 &  $3;*options.debugstr << $$ << " = " << $1  << " & " << $3 << std::endl;}
        ;

xor_expression
        : and_expression
        | xor_expression XOR and_expression  {$$ = $1 ^  $3;*options.debugstr << $$ << " = " << $1  << " ^ " << $3 << std::endl;}
        ;

// lowest precendence
expression
        : xor_expression
        | expression OR xor_expression     {$$ = $1 |  $3;*options.debugstr << $$ << " = " << $1  << " | " << $3 << std::endl;}                 
        ;

%%
void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
