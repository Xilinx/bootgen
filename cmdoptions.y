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
/* command line options grammar
 * Use bison to process this file into cmdoptions.tab.cpp
 */

%require "2.7"
%skeleton "lalr1.cc"
%defines
%define namespace "CO"
%define parser_class_name "BisonParser"
%parse-param { CO::FlexScanner& scanner }
%parse-param { Options& options}
%lex-param   { CO::FlexScanner& scanner }
%locations


%define api.location.type "BIF::location"
 
%code requires {
	namespace CO {
		class BisonScanner;
		class FlexScanner;
		class Parser;
	}
	#include "bootimage.h"
	#include "help.h"
    #include "location.hh"
}

%code {
    static int yylex(CO::BisonParser::semantic_type * yylval, CO::BisonParser::location_type* loc, CO::FlexScanner &scanner);
}

%{
#include <stdio.h>
#include "cmdoptionsscanner.h"
#include "version.h"
void ShowHelp();
void ShowCmdHelp(int);
void ShowBifHelp(int);
void ShowCommonHelp(int,bool);
%}


%start top 

%union 
{
    unsigned long number;
    char* cstring;
}


%token _IMAGE _FILL _O_TOK I _H _DEBUG_TOK _LEGACY _NONBOOTING _PACKAGENAME _BIF_HELP
%token _LOG ERROR WARNING INFO DEBUG TRACE
%token _SPLIT _PROCESS_BITSTREAM MCS BIN
%token _DUMP
%token _ARCH ZYNQ ZYNQMP _R FPGA
%token _DUAL_QSPI_MODE PARALLEL STACKED
%token _W ON OFF
%token _NOAUTHBLOCKS _ZYNQMPES1
%token _EFUSEPPKBITS _GENERATE_HASHES _PADIMAGEHEADER _SPKSIGNATURE _GENERATE_KEYS PEM RSA AUTH GREY METAL
%token _ENCRYPT BBRAM EFUSE _P_TOK
%token _READ READ_BH READ_IHT READ_IH READ_PHT READ_AC
%token _VERIFY _VERIFYKDF
%token _ZYNQMPENCRDUMP
%token <number> DECVALUE HEXVALUE
%token <cstring> IDENTIFIER FILENAME QFILENAME HEXSTRING
%token EQUALS HMAC STARTCBC KEY0
%type <number> number
%type <cstring> filename

%token HBIFHELP HARCH HIMAGE HFILL HO HP HW HEFUSEPPKBITS HGENHASHES HLEGACY HPADHDR H_SPKSIGN HDEBUG
%token HPACKAGE HENCRYPT HGENKEYS HDQSPI HLOG HZYNQMPES1 HPROCESSBIT HNONBOOTING HENCRDUMP
%token HVERIFY

%token H_BIF_INIT H_BIF_UDFBH H_BIF_AES H_BIF_PPK H_BIF_PSK H_BIF_SPK H_BIF_SSK H_BIF_SPKSIGN H_BIF_HIVEC
%token H_BIF_HDRSIGN H_BIF_BI H_BIF_BL H_BIF_PID H_BIF_ENCR H_BIF_AUTH H_BIF_CHKSM H_BIF_ELYHNDOFF H_BIF_BHSIGN 
%token H_BIF_POWNER H_BIF_PRESIGN H_BIF_UDF H_BIF_XIP H_BIF_ALIGN H_BIF_OFFSET H_BIF_RES H_BIF_LOAD H_BIF_TZ
%token H_BIF_STARTUP H_BIF_KEYSRC H_BIF_FSBLCFG H_BIF_BOOTDEV H_BIF_DESTCPU H_BIF_DESTDEV H_BIF_EL H_SPLIT
%token H_BIF_AUTHPARAM H_BIF_BHKEY H_BIF_PFW H_BIF_BLOCKS H_BIF_METAL H_BIF_BHIV H_BIF_AUTHBLOCKS H_BIF_BOOTVEC
%token H_BIF_PUFDATA

%%
top             : option_list;

option_list     : /* empty */ 
                | option_list option
                ;

option          : _IMAGE filename                   { options.SetBifFilename($2); }
                | _SPLIT memsplit
                | filloption                                                       
                | _O_TOK I   filename               { options.GetOutputFileNames().push_back($3); }
                | _O_TOK     filename               { options.GetOutputFileNames().push_back($2); } /* superfluous i can be ommitted */
                | _P_TOK     IDENTIFIER             { options.SetDevicePartName($2); }
                | _W         wopt
                | _LOG       loglevel
                | _H         helpoption
                | _BIF_HELP  bifhelpoption
                | _ENCRYPT keysource startcbc key0 hmac keyfile
                | _EFUSEPPKBITS FILENAME            { options.SetEfuseHashFileName($2); }
                | _GENERATE_HASHES                  { options.SetGenerateHashes(true); }
                | _NONBOOTING                       { options.SetNonBootingFlag(true); }
                | _LEGACY                           { options.SetLegacyFlag(true); }
                | _PADIMAGEHEADER                   { options.SetPadHeaderTable(true); }
                | _PADIMAGEHEADER EQUALS DECVALUE   { options.SetPadHeaderTable((bool)($3!=0)); }
                | _PROCESS_BITSTREAM bitfile_type   
                | _DUAL_QSPI_MODE qpsi_mode
                | _SPKSIGNATURE	FILENAME            { options.SetSpkSigFileName($2); }
                | _PACKAGENAME IDENTIFIER           { options.SetDevicePackageName($2); }
                | _ARCH archOptions
                | _R                                { options.SetArchType(Arch::ZYNQMP); }
                | _NOAUTHBLOCKS                     { options.SetNoAuthBlocksFlag(true); }
                | _GENERATE_KEYS key_type
                | _DEBUG_TOK                        { LOG_ERROR("'-debug' option is no more supported. Please use '-log' option"); }
                | _ZYNQMPENCRDUMP                   { options.SetZynqMpEncrDump(true); }
                | _ZYNQMPES1                        { options.SetZynqmpes1Flag(true); }
                | _READ readImageOptions
                | _VERIFY verifyImageOptions
                | _DUMP dumpOptions
                | _VERIFYKDF filename               { options.SetKDFTestVectorFile($2); }
                ;
                                
filename        : HEXSTRING | IDENTIFIER | FILENAME | QFILENAME ;

memsplit        : MCS   { options.SetSplitType(File::MCS); } 
                | BIN   { options.SetSplitType(File::BIN); } 
                ;

filloption      : _FILL         { options.SetDoFill(true); } 
                | _FILL number  {
                                  options.SetDoFill(true);  
                                  if ($2 >= 0 && $2 <=255)
                                    options.SetOutputFillByte((uint8_t)$2); 
                                  else 
                                    LOG_ERROR("'-fill' - Fill byte must be 8 bits");
                                }   
                ;  
                        
helpoption      : /* empty */       { ShowHelp(); exit(0); }
                | HBIFHELP          { ShowCmdHelp(CO::BisonParser::token::HBIFHELP); exit(0); }
                | HARCH             { ShowCmdHelp(CO::BisonParser::token::HARCH); exit(0); }
                | HIMAGE            { ShowCmdHelp(CO::BisonParser::token::HIMAGE); exit(0); }
                | HFILL             { ShowCmdHelp(CO::BisonParser::token::HFILL); exit(0); }
                | H_SPLIT           { ShowCommonHelp(CO::BisonParser::token::H_SPLIT,true); exit(0); }
                | HO                { ShowCmdHelp(CO::BisonParser::token::HO); exit(0); }
                | HP                { ShowCmdHelp(CO::BisonParser::token::HP); exit(0); }
                | HW                { ShowCmdHelp(CO::BisonParser::token::HW); exit(0); }
                | HEFUSEPPKBITS     { ShowCmdHelp(CO::BisonParser::token::HEFUSEPPKBITS); exit(0); }
                | HGENHASHES        { ShowCmdHelp(CO::BisonParser::token::HGENHASHES); exit(0); }
                | HLEGACY           { ShowCmdHelp(CO::BisonParser::token::HLEGACY); exit(0); }
                | HPADHDR           { ShowCmdHelp(CO::BisonParser::token::HPADHDR); exit(0); }
                | H_SPKSIGN         { ShowCommonHelp(CO::BisonParser::token::H_SPKSIGN,true); exit(0); }
                | HDEBUG            { ShowCmdHelp(CO::BisonParser::token::HDEBUG); exit(0); }
                | HPACKAGE          { ShowCmdHelp(CO::BisonParser::token::HPACKAGE); exit(0); }
                | HENCRYPT          { ShowCmdHelp(CO::BisonParser::token::HENCRYPT); exit(0); }
                | HGENKEYS          { ShowCmdHelp(CO::BisonParser::token::HGENKEYS); exit(0); }
                | HDQSPI            { ShowCmdHelp(CO::BisonParser::token::HDQSPI); exit(0); }
                | HLOG              { ShowCmdHelp(CO::BisonParser::token::HLOG); exit(0); }
                | HZYNQMPES1        { ShowCmdHelp(CO::BisonParser::token::HZYNQMPES1); exit(0); }
                | HPROCESSBIT       { ShowCmdHelp(CO::BisonParser::token::HPROCESSBIT); exit(0); }
                | HNONBOOTING       { ShowCmdHelp(CO::BisonParser::token::HNONBOOTING); exit(0); }
                | HENCRDUMP         { ShowCmdHelp(CO::BisonParser::token::HENCRDUMP); exit(0); }
                | HVERIFY           { ShowCmdHelp(CO::BisonParser::token::HVERIFY); exit(0); }
                ;

bifhelpoption	: /* empty */       { ShowBifHelp(0); exit(0); }
                | H_BIF_INIT        { ShowBifHelp(CO::BisonParser::token::H_BIF_INIT); exit(0); }
                | H_BIF_UDFBH       { ShowBifHelp(CO::BisonParser::token::H_BIF_UDFBH); exit(0); }
                | H_BIF_AES         { ShowBifHelp(CO::BisonParser::token::H_BIF_AES); exit(0); }
                | H_BIF_PPK         { ShowBifHelp(CO::BisonParser::token::H_BIF_PPK); exit(0); }
                | H_BIF_PSK         { ShowBifHelp(CO::BisonParser::token::H_BIF_PSK); exit(0); }
                | H_BIF_SPK         { ShowBifHelp(CO::BisonParser::token::H_BIF_SPK); exit(0); }
                | H_BIF_SSK         { ShowBifHelp(CO::BisonParser::token::H_BIF_SSK); exit(0); }
                | H_BIF_SPKSIGN     { ShowBifHelp(CO::BisonParser::token::H_BIF_SPKSIGN); exit(0); }
                | H_BIF_HDRSIGN     { ShowBifHelp(CO::BisonParser::token::H_BIF_HDRSIGN); exit(0); }
                | H_BIF_BI          { ShowBifHelp(CO::BisonParser::token::H_BIF_BI); exit(0); }
                | H_BIF_BL          { ShowBifHelp(CO::BisonParser::token::H_BIF_BL); exit(0); }
                | H_BIF_ENCR        { ShowBifHelp(CO::BisonParser::token::H_BIF_ENCR); exit(0); }
                | H_BIF_PID         { ShowBifHelp(CO::BisonParser::token::H_BIF_PID); exit(0); }
                | H_BIF_AUTH        { ShowBifHelp(CO::BisonParser::token::H_BIF_AUTH); exit(0); }
                | H_BIF_CHKSM       { ShowBifHelp(CO::BisonParser::token::H_BIF_CHKSM); exit(0); }
                | H_BIF_POWNER      { ShowBifHelp(CO::BisonParser::token::H_BIF_POWNER); exit(0); }
                | H_BIF_PRESIGN     { ShowBifHelp(CO::BisonParser::token::H_BIF_PRESIGN); exit(0); }
                | H_BIF_UDF         { ShowBifHelp(CO::BisonParser::token::H_BIF_UDF); exit(0); }
                | H_BIF_XIP         { ShowBifHelp(CO::BisonParser::token::H_BIF_XIP); exit(0); }
                | H_BIF_ALIGN       { ShowBifHelp(CO::BisonParser::token::H_BIF_ALIGN); exit(0); }
                | H_BIF_OFFSET      { ShowBifHelp(CO::BisonParser::token::H_BIF_OFFSET); exit(0); }
                | H_BIF_RES         { ShowBifHelp(CO::BisonParser::token::H_BIF_RES); exit(0); }
                | H_BIF_LOAD        { ShowBifHelp(CO::BisonParser::token::H_BIF_LOAD); exit(0); }
                | H_BIF_STARTUP     { ShowBifHelp(CO::BisonParser::token::H_BIF_STARTUP); exit(0); }
                | H_BIF_KEYSRC      { ShowBifHelp(CO::BisonParser::token::H_BIF_KEYSRC); exit(0); }
                | H_BIF_FSBLCFG     { ShowBifHelp(CO::BisonParser::token::H_BIF_FSBLCFG); exit(0); }
                | H_BIF_BOOTDEV     { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTDEV); exit(0); }
                | H_BIF_DESTCPU     { ShowBifHelp(CO::BisonParser::token::H_BIF_DESTCPU); exit(0); }
                | H_BIF_DESTDEV     { ShowBifHelp(CO::BisonParser::token::H_BIF_DESTDEV); exit(0); }
                | H_BIF_EL          { ShowBifHelp(CO::BisonParser::token::H_BIF_EL); exit(0); }
                | H_BIF_TZ          { ShowBifHelp(CO::BisonParser::token::H_BIF_TZ); exit(0); }
                | H_BIF_AUTHPARAM   { ShowBifHelp(CO::BisonParser::token::H_BIF_AUTHPARAM); exit(0); }
                | H_BIF_BHKEY       { ShowBifHelp(CO::BisonParser::token::H_BIF_BHKEY); exit(0); }
                | H_BIF_PFW         { ShowBifHelp(CO::BisonParser::token::H_BIF_PFW); exit(0); }
                | H_BIF_BLOCKS      { ShowBifHelp(CO::BisonParser::token::H_BIF_BLOCKS); exit(0); }
                | H_BIF_BHIV        { ShowBifHelp(CO::BisonParser::token::H_BIF_BHIV); exit(0); }
                | H_BIF_METAL       { ShowBifHelp(CO::BisonParser::token::H_BIF_METAL); exit(0); }
                | H_BIF_ELYHNDOFF   { ShowBifHelp(CO::BisonParser::token::H_BIF_ELYHNDOFF); exit(0); }
                | H_BIF_HIVEC       { ShowBifHelp(CO::BisonParser::token::H_BIF_HIVEC); exit(0); }
                | H_BIF_AUTHBLOCKS  { ShowBifHelp(CO::BisonParser::token::H_BIF_AUTHBLOCKS); exit(0); }
                | H_BIF_BHSIGN      { ShowBifHelp(CO::BisonParser::token::H_BIF_BHSIGN); exit(0); }
                | H_SPLIT           { ShowCommonHelp(CO::BisonParser::token::H_SPLIT,false); exit(0); }
                | H_SPKSIGN         { ShowCommonHelp(CO::BisonParser::token::H_SPKSIGN,false); exit(0); }
                | H_BIF_BOOTVEC     { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTVEC); exit(0); }
                | H_BIF_PUFDATA     { ShowBifHelp(CO::BisonParser::token::H_BIF_PUFDATA); exit(0); }
                ;

wopt            : /* empty*/    {options.SetOverwrite(true);}
                | ON            {options.SetOverwrite(true);}
                | OFF           {options.SetOverwrite(false);} 
                ;

loglevel        : /* empty */
                | ERROR     {options.SetLogLevel(LogLevel::ERROR);}
                | WARNING   {options.SetLogLevel(LogLevel::WARNING);}
                | INFO      {options.SetLogLevel(LogLevel::INFO);}
                | DEBUG     {options.SetLogLevel(LogLevel::DEBUG);}
                | TRACE     {options.SetLogLevel(LogLevel::TRACE);}
                ;

keysource       : BBRAM     {options.SetEncryptedKeySource(KeySource::BbramRedKey);}
                | EFUSE     {options.SetEncryptedKeySource(KeySource::EfuseRedKey);}
                ;
                
startcbc        : /* empty*/ | STARTCBC EQUALS HEXSTRING {options.SetStartCbc($3);} ;
key0            : /* empty*/ | KEY0     EQUALS HEXSTRING {options.SetKey0($3);} ;
hmac            : /* empty*/ | HMAC     EQUALS HEXSTRING {options.SetHmac($3);} ;
keyfile         : /* empty*/ | filename                  {options.SetEncryptionKeyFile($1);} ;
                  
number          : HEXVALUE | DECVALUE ;

bitfile_type    : MCS       {options.SetProcessBitstreamType(File::MCS); }
                | BIN       {options.SetProcessBitstreamType(File::BIN); }
                ;

qpsi_mode       : PARALLEL              {options.SetDualQspiMode(QspiMode::PARALLEL_LQSPI); }
                | STACKED qspi_size     {options.SetDualQspiMode(QspiMode::STACKED); }
                ;

qspi_size       : number 
                  { 
                      if ($1==16 || $1==32 || $1==64 || $1==128) 
                          options.SetQspiSize((uint16_t)$1); 
                      else 
                          LOG_ERROR("Supported QSPI sizes (in MB) for -dual_qspi_mode option are 16, 32, 64 & 128"); 
                  }
                ;

archOptions     : ZYNQ      {options.SetArchType(Arch::ZYNQ); }
                | ZYNQMP    {options.SetArchType(Arch::ZYNQMP); }
                | FPGA      {options.SetArchType(Arch::FPGA); }
                ;

key_type        : AUTH      auth_key_options
                | GREY      {options.SetGreyKeyGeneration(true); }
                | METAL     {options.SetMetalKeyGeneration(true); }
                | PEM       {options.SetAuthKeyGeneration(GenAuthKeys::PEM); }
                | RSA       {options.SetAuthKeyGeneration(GenAuthKeys::RSA); }
                ;

auth_key_options: PEM       {options.SetAuthKeyGeneration(GenAuthKeys::PEM); }
                | RSA       {options.SetAuthKeyGeneration(GenAuthKeys::RSA); }
                ;

verifyImageOptions  : filename  { options.SetReadImageFile($1);
                                  options.SetVerifyImageOption(true);}

readImageOptions    : filename              { options.SetReadImageFile($1);
                                              options.SetReadImageOption(ReadImageOption::ALL); }
                    | filename readType     { options.SetReadImageFile($1); }


readType    : READ_BH               {options.SetReadImageOption(ReadImageOption::BH);}
            | READ_IHT              {options.SetReadImageOption(ReadImageOption::IHT);}
            | READ_IH               {options.SetReadImageOption(ReadImageOption::IH);}
            | READ_PHT              {options.SetReadImageOption(ReadImageOption::PHT);}
            | READ_AC               {options.SetReadImageOption(ReadImageOption::AC);}
            ;

dumpOptions : READ_BH               { options.SetDumpOption(DumpOption::BH); }
            | filename READ_BH      { options.SetReadImageFile($1);
                                      options.SetDumpOption(DumpOption::BH); }
            | READ_BH filename      { options.SetReadImageFile($2);
                                      options.SetDumpOption(DumpOption::BH); }
            | filename              { options.SetReadImageFile($1);
                                      options.SetDumpOption(DumpOption::PARTITIONS); }
            ;

%%


//    int                outputByteWidth;
//    Binary::Address_t  baseAddress;
//    uint32_t           defaultAlignment;

void ShowHelp() 
{
   std::cout << HELP << std::endl;
}

void ShowCommonHelp(int a, bool cmdhelp)
{
    switch(a)
    {
        case CO::BisonParser::token::H_SPLIT:
            if(cmdhelp)
                std::cout << SPLITHELP << std::endl;
            else
                std::cout << H_BIF_SPLIT_H << std::endl;
            break;

        case CO::BisonParser::token::H_SPKSIGN:
            if(cmdhelp)
                std::cout << SPKSIGNHELP << std::endl;
            else
                std::cout << H_BIF_SPKSIGN_H << std::endl;
             break;

        case 0: 
             std::cout << HELP << std::endl;
             break;
                     
        default: LOG_ERROR("Unsupported Option"); 
             break;
    }
}


void ShowCmdHelp(int a)
{
    switch (a)
    {
    case CO::BisonParser::token::HARCH:
        std::cout << ARCHHELP << std::endl;
        break;

    case CO::BisonParser::token::HIMAGE:
        std::cout << IMAGEHELP << std::endl;
        break;

    case CO::BisonParser::token::HFILL:
        std::cout << FILLHELP << std::endl;
        break;

    case CO::BisonParser::token::HO:
        std::cout << OHELP << std::endl;
        break;

    case CO::BisonParser::token::HP:
        std::cout << PHELP << std::endl;
        break;

    case CO::BisonParser::token::HW:
        std::cout << WHELP << std::endl;
        break;

    case CO::BisonParser::token::HEFUSEPPKBITS:
        std::cout << EFUSEPPKBITSHELP << std::endl;
        break;

    case CO::BisonParser::token::HZYNQMPES1:
        std::cout << ZYNQMPES1HELP << std::endl;
        break;

    case CO::BisonParser::token::HGENHASHES:
        std::cout << GENERATEHASHESHELP << std::endl;
        break;

    case CO::BisonParser::token::HPADHDR:
        std::cout << PADHDRHELP << std::endl;
        break;

    case CO::BisonParser::token::HDEBUG:
        std::cout << DEBUGHELP << std::endl;
        break;

    case CO::BisonParser::token::HENCRYPT:
        std::cout << ENCRYPTHELP << std::endl;
        break;

    case CO::BisonParser::token::HGENKEYS:
        std::cout << GENKEYSHELP << std::endl;
        break;

    case CO::BisonParser::token::HDQSPI:
        std::cout << DQSPIHELP << std::endl;
        break;

    case CO::BisonParser::token::HLOG:
        std::cout << LOGHELP << std::endl;
        break;

    case CO::BisonParser::token::HBIFHELP:
        std::cout << BIFHELP << std::endl;
        break;

    case CO::BisonParser::token::HPROCESSBIT:
        std::cout << PROCESSBITHELP << std::endl;
        break;

    case CO::BisonParser::token::HNONBOOTING:
        std::cout << NONBOOTINGHELP << std::endl;
        break;

    case CO::BisonParser::token::HENCRDUMP:
        std::cout << ENCRDUMPHELP << std::endl;
        break;

    case CO::BisonParser::token::HVERIFY: 
        std::cout << VERIFYHELP << std::endl;
        break;

    case 0:
        std::cout << HELP << std::endl;
        break;

    default: LOG_ERROR("Unsupported Option");
        break;
    }
}

void ShowBifHelp(int a)
{
    switch (a)
    {
    case CO::BisonParser::token::H_BIF_INIT:
        std::cout << H_BIF_INIT_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_UDFBH:
        std::cout << H_BIF_UDFBH_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_AES:
        std::cout << H_BIF_AES_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PPK:
    case CO::BisonParser::token::H_BIF_PSK:
    case CO::BisonParser::token::H_BIF_SPK:
    case CO::BisonParser::token::H_BIF_SSK:
        std::cout << H_BIF_PPK_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_SPKSIGN:
        std::cout << H_BIF_SPKSIGN_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_HDRSIGN:
        std::cout << H_BIF_HDRSIGN_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BI:
        std::cout << H_BIF_BI_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BL:
        std::cout << H_BIF_BL_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_ENCR:
        std::cout << H_BIF_ENCR_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PID:
        std::cout << H_BIF_PID_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_AUTH:
        std::cout << H_BIF_AUTH_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_CHKSM:
        std::cout << H_BIF_CHKSM_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_POWNER:
        std::cout << H_BIF_POWNER_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PRESIGN:
        std::cout << H_BIF_PRESIGN_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_UDF:
        std::cout << H_BIF_UDF_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_XIP:
        std::cout << H_BIF_XIP_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_ALIGN:
        std::cout << H_BIF_ALIGN_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_OFFSET:
        std::cout << H_BIF_OFFSET_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_RES:
        std::cout << H_BIF_RES_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_LOAD:
        std::cout << H_BIF_LOAD_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_STARTUP:
        std::cout << H_BIF_STARTUP_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_KEYSRC:
        std::cout << H_BIF_KEYSRC_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_FSBLCFG:
        std::cout << H_BIF_FSBLCFG_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BOOTDEV:
        std::cout << H_BIF_BOOTDEV_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_DESTCPU:
        std::cout << H_BIF_DESTCPU_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_DESTDEV:
        std::cout << H_BIF_DESTDEV_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_EL:
        std::cout << H_BIF_EL_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_TZ:
        std::cout << H_BIF_TZ_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_AUTHPARAM:
        std::cout << H_BIF_AUTHPARAM_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BHKEY:
        std::cout << H_BIF_BHKEY_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PFW:
        std::cout << H_BIF_PFW_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BLOCKS:
        std::cout << H_BIF_BLOCKS_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BHIV:
        std::cout << H_BIF_BHIV_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_METAL:
        std::cout << H_BIF_METAL_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_ELYHNDOFF:
        std::cout << H_BIF_ELYHNDOFF_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_HIVEC:
        std::cout << H_BIF_HIVEC_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_AUTHBLOCKS:
        std::cout << H_BIF_AUTHBLOCKS_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BHSIGN:
        std::cout << H_BIF_BHSIGN_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BOOTVEC:
        std::cout << H_BIF_BOOTVEC_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PUFDATA:
        std::cout << H_BIF_PUFDATA_H << std::endl;
        break;

    case 0:
        std::cout << BIF_HELP << std::endl;
        break;

    default: LOG_ERROR("Unsupported Option");
        break;
    }
}

void CO::BisonParser::error(const CO::BisonParser::location_type &loc, const std::string &msg)
{
    scanner.ReportError(loc, msg);
}

static int yylex(CO::BisonParser::semantic_type * yylval, CO::BisonParser::location_type* loc, CO::FlexScanner &scanner)
{
    return scanner.yylex(yylval, loc);
}
