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
/* This is the lexer/flexer input file, that tokenizes the register initialization file */
/* Run the cmdoptions.y file first through bison to generate the regidit.tab.hpp file, which is included herein */
/* Then run this file through flex to generate the cmdoptions.yy.c source code */


%{
 #include <stdio.h>
 #include "cmdoptionsscanner.h"
 #include "cmdoptions.tab.hpp"
 typedef CO::BisonParser::token tok;
%}

%option nodefault yyclass="FlexScanner" noyywrap c++ noyywrap nounput batch debug

%{
     # define YY_USER_ACTION  yylloc->columns (yyleng);
%}

%%

%{
   yylloc->step ();
%}



"/*"                        { comment(); }
\/\/.*?\n                /* ignore */;

0[xX][0-9A-Fa-f]+       yylval->number=strtoul(yytext+2,NULL,16); return tok::HEXVALUE;


\-image                     return tok::_IMAGE;
\-fill                      return tok::_FILL;
\-o                         return tok::_O_TOK;
\i                          return tok::I;
\-p                         return tok::_P_TOK;

\-w                         return tok::_W;
    on                      return tok::ON;
    off                     return tok::OFF;

\-log                       return tok::_LOG;
    error                   return tok::ERROR;
    warning                 return tok::WARNING;
    info                    return tok::INFO;
    debug                   return tok::DEBUG;
    trace                   return tok::TRACE;

\-bif_help                  return tok::_BIF_HELP;

\-encrypt                   return tok::_ENCRYPT;
    bbram                   return tok::BBRAM;
    efuse                   return tok::EFUSE;
    HMAC                    return tok::HMAC;
    hmac                    return tok::HMAC;
    StartCBC                return tok::STARTCBC;
    startcbc                return tok::STARTCBC;
    Key0                    return tok::KEY0;
    key0                    return tok::KEY0;

\-split                     return tok::_SPLIT;
\-process_bitstream         return tok::_PROCESS_BITSTREAM;
    mcs                     return tok::MCS;
    bin                     return tok::BIN;

\-dual_qspi_mode            return tok::_DUAL_QSPI_MODE;
\-dual_ospi_mode            return tok::_DUAL_OSPI_MODE;
    parallel                return tok::PARALLEL;
    stacked                 return tok::STACKED;

=                           return tok::EQUALS;
,                           return tok::COMMA;
\-debug                     return tok::_DEBUG_TOK;
\-dump                      return tok::_DUMP;
    plm                     return tok::DUMP_PLM;
    pmc_cdo                 return tok::DUMP_PMC_CDO;
    boot_files              return tok::DUMP_BOOT_FILES;
    slave_pdis              return tok::DUMP_SLAVE_PDIS;
\-dump_dir                  return tok::_DUMP_DIR;
\-legacy                    return tok::_LEGACY;
\-padimageheader            return tok::_PADIMAGEHEADER;
\-efuseppkbits              return tok::_EFUSEPPKBITS;
\-generate_hashes           return tok::_GENERATE_HASHES;

\-securedebugimage          return tok::_SECUREDEBUG;
\-authenticatedjtag         return tok::_AUTHJTAG;

\-generate_keys             return tok::_GENERATE_KEYS;
    auth                    return tok::AUTH;
    pem                     return tok::PEM;
    rsa                     return tok::RSA;
    obfuscatedkey           return tok::GREY;
    familykey               return tok::METAL;
    ecdsa                   return tok::ECDSA;
    ecdsa-p384              return tok::ECDSA;
    ecdsa-p521              return tok::ECDSAP521;

\-nonbooting                return tok::_NONBOOTING;
\-spksignature              return tok::_SPKSIGNATURE;
\-packagename               return tok::_PACKAGENAME;

\-arch                      return tok::_ARCH;
    zynq                    return tok::ZYNQ;
    zynqmp                  return tok::ZYNQMP;
    versal                  return tok::VERSAL;
    fpga                    return tok::FPGA;

\-r                         return tok::_R;

\-read                      return tok::_READ;
    bh                      return tok::READ_BH;
    iht                     return tok::READ_IHT;
    ih                      return tok::READ_IH;
    pht                     return tok::READ_PHT;
    ac                      return tok::READ_AC;

\-verify_kdf                return tok::_VERIFYKDF;
\-verify                    return tok::_VERIFY;
\-encryption_dump           return tok::_ZYNQMPENCRDUMP;
\-noauthblocks              return tok::_NOAUTHBLOCKS;
\-zynqmpes1                 return tok::_ZYNQMPES1;
\-overlay_cdo               return tok::_OVERLAYCDO;

\-h                         return tok::_H;
\-help                      return tok::_H;
bif_help                    return tok::HBIFHELP;
arch                        return tok::HARCH;
image                       return tok::HIMAGE;
fill                        return tok::HFILL;
o                           return tok::HO;
p                           return tok::HP;
w                           return tok::HW;
efuseppkbits                return tok::HEFUSEPPKBITS;
generate_hashes             return tok::HGENHASHES;
legacy                      return tok::HLEGACY;
padimageheader              return tok::HPADHDR;
spksignature                return tok::H_SPKSIGN;
packagename                 return tok::HPACKAGE;
encrypt                     return tok::HENCRYPT;
generate_keys               return tok::HGENKEYS;
dual_qspi_mode              return tok::HDQSPI;
log                         return tok::HLOG;
zynqmpes1                   return tok::HZYNQMPES1;
process_bitstream           return tok::HPROCESSBIT;
nonbooting                  return tok::HNONBOOTING;
encryption_dump             return tok::HENCRDUMP;
verify                      return tok::HVERIFY;
verify_kdf                  return tok::HVERIFYKDF;
securedebugimage            return tok::HSECUREDEBUG;
authenticatedjtag           return tok::HSECUREDEBUG;
read                        return tok::HREAD;
dump                        return tok::HDUMP;
dump_dir                    return tok::HDUMPDIR;

aarch32_mode                return tok::H_BIF_AARCH32;
big_endian                  return tok::H_BIF_BIGENDIAN;
boot_config                 return tok::H_BIF_BOOTCONFIG;
copy                        return tok::H_BIF_COPY;
core                        return tok::H_BIF_CORE;
delay_handoff               return tok::H_BIF_DELAY_HANDOFF;
delay_load                  return tok::H_BIF_DELAY_LOAD;
file                        return tok::H_BIF_FILE;
id                          return tok::H_BIF_ID;
image                       return tok::H_BIF_IMAGE;
metaheader                  return tok::H_BIF_METAHDR;
name                        return tok::H_BIF_NAME;
partition                   return tok::H_BIF_PARTITION;
parent_id                   return tok::H_BIF_PARENTID;
slr                         return tok::H_BIF_SLR;
type                        return tok::H_BIF_TYPE;
keysrc                      return tok::H_BIF_KEYSRC;
init                        return tok::H_BIF_INIT;
udf_bh                      return tok::H_BIF_UDFBH;
aeskeyfile                  return tok::H_BIF_AES;
ppkfile                     return tok::H_BIF_PPK;
pskfile                     return tok::H_BIF_PSK;
spkfile                     return tok::H_BIF_SPK;
sskfile                     return tok::H_BIF_SSK;
spksignature                return tok::H_BIF_SPKSIGN;
headersignature             return tok::H_BIF_HDRSIGN;
bootimage                   return tok::H_BIF_BOOTIMAGE;
bootloader                  return tok::H_BIF_BL;
encryption                  return tok::H_BIF_ENCR;
pid                         return tok::H_BIF_PID;
authentication              return tok::H_BIF_AUTH;
checksum                    return tok::H_BIF_CHKSM;
owner                       return tok::H_BIF_POWNER;
partition_owner             return tok::H_BIF_POWNER;
presign                     return tok::H_BIF_PRESIGN;
udf_data                    return tok::H_BIF_UDF;
xip_mode                    return tok::H_BIF_XIP;
alignment                   return tok::H_BIF_ALIGN;
offset                      return tok::H_BIF_OFFSET;
reserve                     return tok::H_BIF_RES;
load                        return tok::H_BIF_LOAD;
startup                     return tok::H_BIF_STARTUP;
keysrc_encryption           return tok::H_BIF_KEYSRCENCR;
fsbl_config                 return tok::H_BIF_FSBLCFG;
boot_device                 return tok::H_BIF_BOOTDEV;
destination_cpu             return tok::H_BIF_DESTCPU;
destination_device          return tok::H_BIF_DESTDEV;
exception_level             return tok::H_BIF_EL;
trustzone                   return tok::H_BIF_TZ;
auth_params                 return tok::H_BIF_AUTHPARAM;
bh_keyfile                  return tok::H_BIF_BHKEY;
pmufw_image                 return tok::H_BIF_PFW;
blocks                      return tok::H_BIF_BLOCKS;
bh_key_iv                   return tok::H_BIF_BHIV;
familykey                   return tok::H_BIF_METAL;
early_handoff               return tok::H_BIF_ELYHNDOFF;
hivec                       return tok::H_BIF_HIVEC;
bhsignature                 return tok::H_BIF_BHSIGN;
split                       return tok::H_SPLIT;
bootvectors                 return tok::H_BIF_BOOTVEC;
puf_file                    return tok::H_BIF_PUFDATA;
dpacm_enable                return tok::H_DPACM_ENABLE;
userkeys                    return tok::H_BIF_USERKEYS;



[a-fA-F0-9]+        yylval->cstring=strdup(yytext);         return tok::HEXSTRING;
[0-9]?[a-zA-Z0-9.,_-]+     yylval->cstring=strdup(yytext);  return tok::IDENTIFIER;
([a-zA-Z]:[\/\\])?[a-zA-Z0-9\/\\\\(\)@!#$%^&+._-]+          yylval->cstring=strdup(yytext);    return tok::FILENAME;
\"([a-zA-Z]:[\/\\])?[a-zA-Z0-9\/\\\\(\)@!#$%^&+._ -]+\"     yylval->cstring=strdup(yytext+1); yylval->cstring[strlen(yytext)-2]=0; return tok::QFILENAME;


[ \t]+                  yylloc->step ();
[\n]+                   yylloc->lines (yyleng); yylloc->step ();
.                       LOG_ERROR("Unknown option on command line: %s\n",yytext);
%%


void CO::FlexScanner::comment()
{
    char c, c1;
    loop:
        while ((c = yyinput()) != '*' && c != 0) 
        {
            // putchar(c);
        }

    if ((c1 = yyinput()) != '/' && c != 0)
    {
        unput(c1);
        goto loop;
    }
}
