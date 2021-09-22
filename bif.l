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
/* This is the lexer/flexer input file, that tokenizes the BIF file */
/* Run the bif.y file first through bison to generate the bif.tab.h file, which is included herein */
/* Then run this file through flex to generate the bif.yy.c source code */


%{
 #include <stdio.h>
 #include "bifscanner.h"
 #include "bif.tab.hpp"
 #include "authentication.h"
 #include "encryption.h"
 typedef BIF::BisonParser::token tok;
%}

/* see http://www.lysator.liu.se/c/ANSI-C-grammar-l.html */

%option nodefault yyclass="FlexScanner" noyywrap c++ noyywrap nounput batch debug

%{
     # define YY_USER_ACTION  yylloc->columns (yyleng);
%}

%%

%{
   yylloc->step ();
%}

"/*"            { comment(); }
\/\/.*?\n       yylloc->lines (1); yylloc->step ();

bootloader                                                          return tok::BOOTLOADER;
presign                                                             return tok::PRESIGN;
udf_data                                                            return tok::UDF_DATA;
xip_mode                                                            return tok::XIP_MODE;
boot                                                                return tok::BOOT;
user                                                                return tok::USER;
static                                                              return tok::STATIC;
noautostart                                                         return tok::NOAUTOSTART;
multiboot                                                           return tok::MULTIBOOT;
protected                                                           return tok::PROTECTED;
alignment                                                           return tok::ALIGNMENT;
early_handoff                                                       return tok::EARLY_HANDOFF;
delay_handoff                                                       return tok::DELAY_HANDOFF;
delay_load                                                          return tok::DELAY_LOAD;
hivec                                                               return tok::HIVEC;
blocks                                                              return tok::BLOCKS;
authblocks                                                          return tok::AUTHBLOCKS;
big_endian                                                          return tok::BIGENDIAN;
aarch32_mode                                                        return tok::A32_MODE;
partition                                                           return tok::PARTITION;
image                                                               return tok::IMAGE;
metaheader                                                          return tok::METAHEADER;
section                                                             return tok::BIF_SECTION;

offset              yylval->number = tok::OFFSET;                   return tok::OFFSET;
reserve_legacy      yylval->number = tok::RESERVE_LEGACY;           return tok::RESERVE_LEGACY;
reserve             yylval->number = tok::RESERVE;                  return tok::RESERVE;
load                yylval->number = tok::LOAD;                     return tok::LOAD;
copy                yylval->number = tok::COPY;                     return tok::COPY;
startup             yylval->number = tok::STARTUP;                  return tok::STARTUP;
pid                 yylval->number = tok::PARTITION_NUM;            return tok::PARTITION_NUM;
init                yylval->number = tok::INIT;                     return tok::INIT;
udf_bh              yylval->number = tok::UDF_BH;                   return tok::UDF_BH;
aeskeyfile          yylval->number = tok::AES_KEY_FILE;             return tok::AES_KEY_FILE;
ppkfile             yylval->number = tok::PPK_FILE;                 return tok::PPK_FILE;
pskfile             yylval->number = tok::PSK_FILE;                 return tok::PSK_FILE;
spkfile             yylval->number = tok::SPK_FILE;                 return tok::SPK_FILE;
sskfile             yylval->number = tok::SSK_FILE;                 return tok::SSK_FILE;
spksignature        yylval->number = tok::SPK_SIGNATURE_FILE;       return tok::SPK_SIGNATURE_FILE;
bhsignature         yylval->number = tok::BH_SIGNATURE_FILE;        return tok::BH_SIGNATURE_FILE;
headersignature     yylval->number = tok::HEADER_SIGNATURE_FILE;    return tok::HEADER_SIGNATURE_FILE;
bootvectors         yylval->number = tok::BOOTVECTORS;              return tok::BOOTVECTORS;
bootimage           yylval->number = tok::BOOTIMAGE;                return tok::BOOTIMAGE;
parent_id           yylval->number = tok::PARENT_ID;                return tok::PARENT_ID; 
id_code             yylval->number = tok::ID_CODE;                  return tok::ID_CODE;
extended_id_code    yylval->number = tok::EXT_ID_CODE;              return tok::EXT_ID_CODE;
id                  yylval->number = tok::ID;                       return tok::ID; 
name                yylval->number = tok::NAME;                     return tok::NAME; 
file                yylval->number = tok::PFILE;                    return tok::PFILE;
bh_keyfile          yylval->number = tok::BH_KEY_FILE;              return tok::BH_KEY_FILE;
puf_file            yylval->number = tok::PUF_HELPER_FILE;          return tok::PUF_HELPER_FILE;
pmufw_image         yylval->number = tok::PMUFW_IMAGE;              return tok::PMUFW_IMAGE;
pmcdata             yylval->number = tok::PMCDATA;                  return tok::PMCDATA;
bh_key_iv           yylval->number = tok::BH_KEY_IV;                return tok::BH_KEY_IV;
uid                 yylval->number = tok::UNIQUE_ID;                return tok::UNIQUE_ID;
parent_uid          yylval->number = tok::PARENT_UNIQUE_ID;         return tok::PARENT_UNIQUE_ID;
function_id         yylval->number = tok::FUNCTION_ID;              return tok::FUNCTION_ID;

bbram_kek_iv        yylval->number = tok::BBRAM_KEK_IV;             return tok::BBRAM_KEK_IV;
bh_kek_iv           yylval->number = tok::BH_KEK_IV;                return tok::BH_KEK_IV;
efuse_kek_iv        yylval->number = tok::EFUSE_KEK_IV;             return tok::EFUSE_KEK_IV;
efuse_user_kek0_iv  yylval->number = tok::EFUSE_USER_KEK0_IV;       return tok::EFUSE_USER_KEK0_IV;
efuse_user_kek1_iv  yylval->number = tok::EFUSE_USER_KEK1_IV;       return tok::EFUSE_USER_KEK1_IV;
userkeys            yylval->number = tok::USER_KEYS;                return tok::USER_KEYS;

familykey           yylval->number = tok::FAMILY_KEY;               return tok::FAMILY_KEY;

encryption                                                          return tok::ENCRYPTION;
    none            yylval->number = 0;                             return tok::NONE;
    aes             yylval->number = Encryption::AES;               return tok::ENCRVALUE;

authentication                                                      return tok::AUTHENTICATION;
    rsa             yylval->number = Authentication::RSA;           return tok::AUTHVALUE;
    ecdsa           yylval->number = Authentication::ECDSA;         return tok::AUTHVALUE;
    ecdsa-p384      yylval->number = Authentication::ECDSA;         return tok::AUTHVALUE;
    ecdsa-p521      yylval->number = Authentication::ECDSAp521;     return tok::AUTHVALUE;

checksum                                                            return tok::CHECKSUM;
    md5             yylval->number = Checksum::MD5;                 return tok::CHECKSUMVALUE;
    sha2            yylval->number = Checksum::SHA2;                return tok::CHECKSUMVALUE;
    sha3            yylval->number = Checksum::SHA3;                return tok::CHECKSUMVALUE;

partition_owner                                                     return tok::PARTITION_OWNER;
owner                                                               return tok::PARTITION_OWNER;
    fsbl            yylval->number = PartitionOwner::FSBL;          return tok::POWNERVALUE ;
    uboot           yylval->number = PartitionOwner::UBOOT;         return tok::POWNERVALUE ;
    pmc             yylval->number = PartitionOwner::FSBL;          return tok::POWNERVALUE ;
    non-pmc         yylval->number = PartitionOwner::UBOOT;         return tok::POWNERVALUE ;

partition_type                                                      return tok::PARTITION_TYPE;
type                                                                return tok::PARTITION_TYPE;
    cdo             yylval->number = PartitionType::CONFIG_DATA_OBJ;return tok::PTYPEVALUE;
    raw             yylval->number = PartitionType::RAW;            return tok::PTYPEVALUE;
    cfi             yylval->number = PartitionType::CFI;            return tok::PTYPEVALUE;
    cfi-gsc         yylval->number = PartitionType::CFI_GSC;        return tok::PTYPEVALUE;
    slr-boot        yylval->number = PartitionType::SLR_BOOT;       return tok::PTYPEVALUE;
    slr-config      yylval->number = PartitionType::SLR_CONFIG;     return tok::PTYPEVALUE;

keysrc_encryption           yylval->number = tok::KEYSRC_ENCRYPTION;        return tok::KEYSRC_ENCRYPTION;
keysrc                      yylval->number = tok::KEYSRC_ENCRYPTION;        return tok::KEYSRC_ENCRYPTION;
    efuse                   yylval->number = KeySource::EfuseRedKey;        return tok::KEY_SRC;
    bbram                   yylval->number = KeySource::BbramRedKey;        return tok::KEY_SRC;
    efuse_red_key           yylval->number = KeySource::EfuseRedKey;        return tok::KEY_SRC;
    efuse_blk_key           yylval->number = KeySource::EfuseBlkKey;        return tok::KEY_SRC;
    efuse_gry_key           yylval->number = KeySource::EfuseGryKey;        return tok::KEY_SRC;
    bbram_red_key           yylval->number = KeySource::BbramRedKey;        return tok::KEY_SRC;
    bbram_blk_key           yylval->number = KeySource::BbramBlkKey;        return tok::KEY_SRC;
    bbram_gry_key           yylval->number = KeySource::BbramGryKey;        return tok::KEY_SRC;
    bh_gry_key              yylval->number = KeySource::BhGryKey;           return tok::KEY_SRC;
    bh_blk_key              yylval->number = KeySource::BhBlkKey;           return tok::KEY_SRC;
    user_key0               yylval->number = KeySource::UserKey0;           return tok::KEY_SRC;
    user_key1               yylval->number = KeySource::UserKey1;           return tok::KEY_SRC;
    user_key2               yylval->number = KeySource::UserKey2;           return tok::KEY_SRC;
    user_key3               yylval->number = KeySource::UserKey3;           return tok::KEY_SRC;
    user_key4               yylval->number = KeySource::UserKey4;           return tok::KEY_SRC;
    user_key5               yylval->number = KeySource::UserKey5;           return tok::KEY_SRC;
    user_key6               yylval->number = KeySource::UserKey6;           return tok::KEY_SRC;
    user_key7               yylval->number = KeySource::UserKey7;           return tok::KEY_SRC;
    efuse_user_key0         yylval->number = KeySource::EfuseUserKey0;      return tok::KEY_SRC;
    efuse_user_blk_key0     yylval->number = KeySource::EfuseUserBlkKey0;   return tok::KEY_SRC;
    efuse_user_gry_key0     yylval->number = KeySource::EfuseUserGryKey0;   return tok::KEY_SRC;
    efuse_user_key1         yylval->number = KeySource::EfuseUserKey1;      return tok::KEY_SRC;
    efuse_user_blk_key1     yylval->number = KeySource::EfuseUserBlkKey1;   return tok::KEY_SRC;
    efuse_user_gry_key1     yylval->number = KeySource::EfuseUserGryKey1;   return tok::KEY_SRC;
    kup_key                 yylval->number = KeySource::BhKupKey;           return tok::KEY_SRC;

fsbl_config             yylval->number = tok::FSBL_CONFIG;              return tok::FSBL_CONFIG;
boot_config             yylval->number = tok::FSBL_CONFIG;              return tok::FSBL_CONFIG;
image_config            yylval->number = tok::FSBL_CONFIG;              return tok::FSBL_CONFIG;
    r5_single           yylval->number = Core::R5Single;                return tok::CORE;
    a53_x32             yylval->number = Core::A53Singlex32;            return tok::CORE;
    a5x_x32             yylval->number = Core::A53Singlex32;            return tok::CORE;
    a53_x64             yylval->number = Core::A53Singlex64;            return tok::CORE;
    a5x_x64             yylval->number = Core::A53Singlex64;            return tok::CORE;
    r5_dual             yylval->number = Core::R5Dual;                  return tok::CORE;
    bh_auth_enable      yylval->number = BhRsa::BhRsaEnable;            return tok::BH_RSA;
    auth_sha2_enable    yylval->number = AuthHash::Sha2;                return tok::AUTH_HASH;
    pufhd_bh            yylval->number = PufHdLoc::PUFinBH;             return tok::PUFHD_LOC;
    auth_only           yylval->number = AuthOnly::Enabled;             return tok::AUTH_ONLY;
    opt_key             yylval->number = OptKey::OptKeyinSecHdr;        return tok::OPT_KEY;
    puf4kmode           yylval->number = PufMode::PUF4K;                return tok::PUF4KMODE;
    shutter             yylval->number = tok::SHUTTER;                  return tok::SHUTTER;
    dpacm_enable        yylval->number = DpaCM::DpaCMEnable;            return tok::DPA_CM;
    smap_width          yylval->number = tok::SMAP_WIDTH;               return tok::SMAP_WIDTH;
    bypass_idcode_check yylval->number = tok::BYPASS_IDCODE_CHECK;      return tok::BYPASS_IDCODE_CHECK;
    a_hwrot             yylval->number = tok::A_HWROT;                  return tok::A_HWROT;
    s_hwrot             yylval->number = tok::S_HWROT;                  return tok::S_HWROT;

boot_device                                                         return tok::BOOT_DEVICE;
    address         yylval->number = tok::ADDRESS;                  return tok::ADDRESS;
    qspi32          yylval->number = BootDevice::QSPI32;            return tok::BOOT_DEVICE_TYPE ;
    qspi24          yylval->number = BootDevice::QSPI24;            return tok::BOOT_DEVICE_TYPE ;
    nand            yylval->number = BootDevice::NAND;              return tok::BOOT_DEVICE_TYPE ;
    sd0             yylval->number = BootDevice::SD0;               return tok::BOOT_DEVICE_TYPE ;
    sd1             yylval->number = BootDevice::SD1;               return tok::BOOT_DEVICE_TYPE ;
    sd-ls           yylval->number = BootDevice::SDLS;              return tok::BOOT_DEVICE_TYPE ;
    mmc             yylval->number = BootDevice::MMC;               return tok::BOOT_DEVICE_TYPE ;
    usb             yylval->number = BootDevice::USB;               return tok::BOOT_DEVICE_TYPE ;
    ethernet        yylval->number = BootDevice::ETHERNET;          return tok::BOOT_DEVICE_TYPE ;
    pcie            yylval->number = BootDevice::PCIE;              return tok::BOOT_DEVICE_TYPE ;
    sata            yylval->number = BootDevice::SATA;              return tok::BOOT_DEVICE_TYPE ;
    ospi            yylval->number = BootDevice::OSPI;              return tok::BOOT_DEVICE_TYPE ;
    smap            yylval->number = BootDevice::SMAP;              return tok::BOOT_DEVICE_TYPE ;
    sbi             yylval->number = BootDevice::SBI;               return tok::BOOT_DEVICE_TYPE ;
    sd0-raw         yylval->number = BootDevice::SD0RAW;            return tok::BOOT_DEVICE_TYPE ;
    sd1-raw         yylval->number = BootDevice::SD1RAW;            return tok::BOOT_DEVICE_TYPE ;
    sd-ls-raw       yylval->number = BootDevice::SDLSRAW;           return tok::BOOT_DEVICE_TYPE ;
    mmc-raw         yylval->number = BootDevice::MMCRAW;            return tok::BOOT_DEVICE_TYPE ;
    mmc0            yylval->number = BootDevice::MMC0;              return tok::BOOT_DEVICE_TYPE ;
    mmc0-raw        yylval->number = BootDevice::MMC0RAW;           return tok::BOOT_DEVICE_TYPE ;

destination_cpu                                                     return tok::DEST_CPU;
core                                                                return tok::DEST_CPU;
    a53-0           yylval->number = DestinationCPU::A53_0;         return tok::DEST_CPU_TYPE;
    a72-0           yylval->number = DestinationCPU::A53_0;         return tok::DEST_CPU_TYPE;
    a5x-0           yylval->number = DestinationCPU::A53_0;         return tok::DEST_CPU_TYPE;
    a53-1           yylval->number = DestinationCPU::A53_1;         return tok::DEST_CPU_TYPE;
    a72-1           yylval->number = DestinationCPU::A53_1;         return tok::DEST_CPU_TYPE;
    a5x-1           yylval->number = DestinationCPU::A53_1;         return tok::DEST_CPU_TYPE;
    a53-2           yylval->number = DestinationCPU::A53_2;         return tok::DEST_CPU_TYPE;
    a5x-2           yylval->number = DestinationCPU::A53_2;         return tok::DEST_CPU_TYPE;
    a53-3           yylval->number = DestinationCPU::A53_3;         return tok::DEST_CPU_TYPE;
    a5x-3           yylval->number = DestinationCPU::A53_3;         return tok::DEST_CPU_TYPE;
    r5-0            yylval->number = DestinationCPU::R5_0;          return tok::DEST_CPU_TYPE;
    r5-1            yylval->number = DestinationCPU::R5_1;          return tok::DEST_CPU_TYPE;
    r5-lockstep     yylval->number = DestinationCPU::R5_lockstep;   return tok::DEST_CPU_TYPE;
    pmu             yylval->number = DestinationCPU::PMU;           return tok::DEST_CPU_TYPE;
    psm             yylval->number = DestinationCPU::PMU;           return tok::DEST_CPU_TYPE;
    aie             yylval->number = DestinationCPU::AIE;           return tok::DEST_CPU_TYPE;

destination_device                                                      return tok::DEST_DEVICE;
    ps              yylval->number = DestinationDevice::DEST_DEV_PS;    return tok::DEST_DEVICE_TYPE;
    pl              yylval->number = DestinationDevice::DEST_DEV_PL;    return tok::DEST_DEVICE_TYPE;
    pmufw           yylval->number = DestinationDevice::DEST_DEV_PMU;   return tok::DEST_DEVICE_TYPE;
    xip             yylval->number = DestinationDevice::DEST_DEV_XIP;   return tok::DEST_DEVICE_TYPE;

exception_level                                                     return tok::EXCEPTION_LEVEL;
    el-0            yylval->number = ExceptionLevel::EL0;           return tok::EXCEPTION_LEVEL_TYPE;
    el-1            yylval->number = ExceptionLevel::EL1;           return tok::EXCEPTION_LEVEL_TYPE;
    el-2            yylval->number = ExceptionLevel::EL2;           return tok::EXCEPTION_LEVEL_TYPE;
    el-3            yylval->number = ExceptionLevel::EL3;           return tok::EXCEPTION_LEVEL_TYPE;

trustzone                                                           return tok::TRUSTZONE;
    secure          yylval->number = TrustZone::Secure;             return tok::TRUSTZONE_TYPE;
    nonsecure       yylval->number = TrustZone::NonSecure;          return tok::TRUSTZONE_TYPE;

slr                 yylval->number = tok::SLR_NUM;                  return tok::SLR_NUM;

auth_params         yylval->number = tok::AUTH_PARAMS;              return tok::AUTH_PARAMS;
    ppk_select      yylval->number = tok::PPK_SELECT;               return tok::PPK_SELECT;
    spk_select      yylval->number = tok::SPK_SELECT;               return tok::SPK_SELECT;
        spk-efuse   yylval->number = SpkSelect::SPK_eFUSE;          return tok::SPKSELECT;
        user-efuse  yylval->number = SpkSelect::USER_eFUSE;         return tok::SPKSELECT;
    spk_id          yylval->number = tok::SPK_ID;                   return tok::SPK_ID; 
    header_auth                                                     return tok::HEADER_AUTH;

revoke_id           yylval->number = tok::REVOKE_ID;                return tok::REVOKE_ID;

split               yylval->number = tok::SPLIT;                    return tok::SPLIT;
    mode            yylval->number = tok::SPLIT_MODE;               return tok::SPLIT_MODE;
       slaveboot    yylval->number = SplitMode::SlaveMode;          return tok::SPLITMODE;
       normal       yylval->number = SplitMode::Normal;             return tok::SPLITMODE;
    fmt             yylval->number = tok::SPLIT_FMT;                return tok::SPLIT_FMT; 
       mcs                                                          return tok::MCS;
       bin                                                          return tok::BIN;

authjtag_config     yylval->number = tok::AUTHJTAG_CONFIG;          return tok::AUTHJTAG_CONFIG;
    device_dna      yylval->number = tok::DEVICE_DNA;               return tok::DEVICE_DNA;
    jtag_timeout    yylval->number = tok::JTAG_TIMEOUT;             return tok::JTAG_TIMEOUT;

[0-9]+              yylval->number=atoi(yytext);                    return tok::DECVALUE;
0[xX][0-9a-fA-F]+   yylval->number=strtoul(yytext+2,NULL,16);       return tok::HEXVALUE;
[a-fA-F0-9]+        yylval->string=strdup(yytext);                  return tok::HEXWORD;

:                                                                   return tok::COLON;
;                                                                   return tok::SEMICOLON;
=                                                                   return tok::EQUAL;
,                                                                   return tok::COMMA;
\[                                                                  return tok::OBRACKET;
\]                                                                  return tok::EBRACKET;
\{                                                                  return tok::OBRACE;
\}                                                                  return tok::EBRACE;
\(                                                                  return tok::LPAREN;
\)                                                                  return tok::RPAREN;
\*                                                                  return tok::ASTERISK;

\+                                                                  return tok::PLUS;
\-                                                                  return tok::MINUS;
\<\<                                                                return tok::LSHIFT;
\>\>                                                                return tok::RSHIFT;
\*                                                                  return tok::MULT;
\/                                                                  return tok::DIVIDE;
\%                                                                  return tok::MODULO;
\~                                                                  return tok::NEGATION;
\&                                                                  return tok::AND;
\|                                                                  return tok::OR;
\^                                                                  return tok::XOR;

[a-zA-Z_][a-zA-Z0-9_]*                          yylval->string=strdup(yytext);      return tok::WORD;
([a-zA-Z]:[\/\\])?[a-zA-Z0-9\/\\\+._-]+         yylval->string=strdup(yytext);      return tok::FILENAME;
\"([a-zA-Z]:[\/\\])?[a-zA-Z0-9\/\\\\(\)@!#$%^&+._ -]+\"    yylval->string=strdup(yytext+1); yylval->string[strlen(yytext)-2]=0; return tok::QFILENAME;

[ \t]+                  yylloc->step ();
[\n]+                   yylloc->lines (yyleng); yylloc->step ();
\r                     /* ignore (Linux only) */
.                      printf("WARNING: bad character! '%s' (0x%02X)\n",yytext,yytext[0]);
%%


void BIF::FlexScanner::comment()
{
    char c, c1;
loop:
    while ((c = yyinput()) != '*' && c != 0) 
    {
        // putchar(c);
        if (c == '\n') yylloc->lines (1);
    }

    if ((c1 = yyinput()) != '/' && c != 0)
    {
        unput(c1);
        goto loop;
    }
}
