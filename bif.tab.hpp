/* A Bison parser, made by GNU Bison 2.7.  */

/* Skeleton interface for Bison LALR(1) parsers in C++
   
      Copyright (C) 2002-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/**
 ** \file ../bisonflex/bif.tab.hpp
 ** Define the BIF::parser class.
 */

/* C++ LALR(1) parser skeleton written by Akim Demaille.  */

#ifndef YY_YY_BISONFLEX_BIF_TAB_HPP_INCLUDED
# define YY_YY_BISONFLEX_BIF_TAB_HPP_INCLUDED

/* "%code requires" blocks.  */
/* Line 33 of lalr1.cc  */
#line 37 "../s/bif.y"

       namespace BIF {
        class BisonScanner;
        class FlexScanner;
        class Parser;
    }
    #include "bootimage.h"
    #include "bifoptions.h"


/* Line 33 of lalr1.cc  */
#line 58 "../bisonflex/bif.tab.hpp"


#include <string>
#include <iostream>
#include "stack.hh"
#include "location.hh"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Line 33 of lalr1.cc  */
#line 24 "../s/bif.y"
namespace BIF {
/* Line 33 of lalr1.cc  */
#line 75 "../bisonflex/bif.tab.hpp"

  /// A Bison parser.
  class BisonParser
  {
  public:
    /// Symbol semantic values.
#ifndef YYSTYPE
    union semantic_type
    {
/* Line 33 of lalr1.cc  */
#line 73 "../s/bif.y"

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


/* Line 33 of lalr1.cc  */
#line 116 "../bisonflex/bif.tab.hpp"
    };
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;
    /// Tokens.
    struct token
    {
      /* Tokens.  */
   enum yytokentype {
     OBRACE = 258,
     EBRACE = 259,
     COMMA = 260,
     EQUAL = 261,
     COLON = 262,
     QUOTE = 263,
     SEMICOLON = 264,
     OBRACKET = 265,
     EBRACKET = 266,
     BOOTLOADER = 267,
     XIP_MODE = 268,
     EARLY_HANDOFF = 269,
     HIVEC = 270,
     LOCKSTEP = 271,
     AUTHENTICATION = 272,
     ENCRYPTION = 273,
     CHECKSUM = 274,
     PARTITION_OWNER = 275,
     PARTITION_TYPE = 276,
     PARTITION_NUM = 277,
     BOOT_DEVICE = 278,
     DEST_DEVICE = 279,
     DEST_CPU = 280,
     ADDRESS = 281,
     EXCEPTION_LEVEL = 282,
     TRUSTZONE = 283,
     ALIGNMENT = 284,
     OFFSET = 285,
     RESERVE_LEGACY = 286,
     RESERVE = 287,
     LOAD = 288,
     STARTUP = 289,
     BIGENDIAN = 290,
     A32_MODE = 291,
     PPK_SELECT = 292,
     SPK_ID = 293,
     SPK_SELECT = 294,
     HEADER_AUTH = 295,
     REVOKE_ID = 296,
     SPLIT_MODE = 297,
     SPLIT_FMT = 298,
     BOOT = 299,
     USER = 300,
     STATIC = 301,
     NOAUTOSTART = 302,
     MULTIBOOT = 303,
     PROTECTED = 304,
     BLOCKS = 305,
     AUTHBLOCKS = 306,
     BOOTVECTORS = 307,
     PRESIGN = 308,
     BIF_SECTION = 309,
     UDF_DATA = 310,
     MCS = 311,
     BIN = 312,
     SLR_NUM = 313,
     CLUSTER_NUM = 314,
     DICE = 315,
     PCR_NUMBER = 316,
     PCR_MEASUREMENT_INDEX = 317,
     IMAGE_STORE = 318,
     PARENT_ID = 319,
     ID_CODE = 320,
     EXT_ID_CODE = 321,
     BYPASS_IDCODE_CHECK = 322,
     A_HWROT = 323,
     S_HWROT = 324,
     UNIQUE_ID = 325,
     PARENT_UNIQUE_ID = 326,
     FUNCTION_ID = 327,
     IMAGE = 328,
     ID = 329,
     NAME = 330,
     DELAY_HANDOFF = 331,
     DELAY_LOAD = 332,
     COPY = 333,
     INCLUDE = 334,
     DELAY_AUTH = 335,
     PARTITION = 336,
     PFILE = 337,
     OPTIONAL_DATA = 338,
     METAHEADER = 339,
     TCM_BOOT = 340,
     TCM_A_REGION = 341,
     TCM_B_REGION = 342,
     TCM_C_REGION = 343,
     WORD = 344,
     HEXWORD = 345,
     FILENAME = 346,
     QFILENAME = 347,
     NONE = 348,
     DECVALUE = 349,
     HEXVALUE = 350,
     KEYSRC_ENCRYPTION = 351,
     FSBL_CONFIG = 352,
     AUTH_PARAMS = 353,
     AUTHJTAG_CONFIG = 354,
     DEVICE_DNA = 355,
     JTAG_TIMEOUT = 356,
     PUF4KMODE = 357,
     PUFROSWAP = 358,
     SHUTTER = 359,
     SPLIT = 360,
     SMAP_WIDTH = 361,
     PUF_HELPER_FILE = 362,
     BH_KEY_FILE = 363,
     BH_KEY_IV = 364,
     BH_KEK_IV = 365,
     BBRAM_KEK_IV = 366,
     EFUSE_KEK_IV = 367,
     EFUSE_USER_KEK0_IV = 368,
     EFUSE_USER_KEK1_IV = 369,
     USER_KEYS = 370,
     PMCDATA = 371,
     BOOTIMAGE = 372,
     UDF_BH = 373,
     INIT = 374,
     PMUFW_IMAGE = 375,
     AES_KEY_FILE = 376,
     FAMILY_KEY = 377,
     PPK_FILE = 378,
     PSK_FILE = 379,
     SPK_FILE = 380,
     SSK_FILE = 381,
     SPK_SIGNATURE_FILE = 382,
     BH_SIGNATURE_FILE = 383,
     HEADER_SIGNATURE_FILE = 384,
     AUTHVALUE = 385,
     ENCRVALUE = 386,
     CHECKSUMVALUE = 387,
     POWNERVALUE = 388,
     PTYPEVALUE = 389,
     KEY_SRC = 390,
     CORE = 391,
     BH_RSA = 392,
     AUTH_HASH = 393,
     INT_HASH = 394,
     PUFHD_LOC = 395,
     OPT_KEY = 396,
     AUTH_ONLY = 397,
     BOOT_DEVICE_TYPE = 398,
     DEST_DEVICE_TYPE = 399,
     DEST_CPU_TYPE = 400,
     EXCEPTION_LEVEL_TYPE = 401,
     TRUSTZONE_TYPE = 402,
     SPLITMODE = 403,
     DPA_CM = 404,
     SPKSELECT = 405,
     OR = 406,
     XOR = 407,
     AND = 408,
     MULT = 409,
     DIVIDE = 410,
     MODULO = 411,
     PLUS = 412,
     MINUS = 413,
     LSHIFT = 414,
     RSHIFT = 415,
     NEGATION = 416,
     LPAREN = 417,
     RPAREN = 418,
     ASTERISK = 419
   };

    };
    /// Token type.
    typedef token::yytokentype token_type;

    /// Build a parser object.
    BisonParser (BIF::FlexScanner& scanner_yyarg, Options& options_yyarg);
    virtual ~BisonParser ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

  private:
    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Generate an error message.
    /// \param state   the state where the error occurred.
    /// \param tok     the lookahead token.
    virtual std::string yysyntax_error_ (int yystate, int tok);

#if YYDEBUG
    /// \brief Report a symbol value on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_value_print_ (int yytype,
					 const semantic_type* yyvaluep,
					 const location_type* yylocationp);
    /// \brief Report a symbol on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_print_ (int yytype,
				   const semantic_type* yyvaluep,
				   const location_type* yylocationp);
#endif


    /// State numbers.
    typedef int state_type;
    /// State stack type.
    typedef stack<state_type>    state_stack_type;
    /// Semantic value stack type.
    typedef stack<semantic_type> semantic_stack_type;
    /// location stack type.
    typedef stack<location_type> location_stack_type;

    /// The state stack.
    state_stack_type yystate_stack_;
    /// The semantic value stack.
    semantic_stack_type yysemantic_stack_;
    /// The location stack.
    location_stack_type yylocation_stack_;

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    /// Internal symbol numbers.
    typedef unsigned char token_number_type;
    /* Tables.  */
    /// For a state, the index in \a yytable_ of its portion.
    static const short int yypact_[];
    static const short int yypact_ninf_;

    /// For a state, default reduction number.
    /// Unless\a  yytable_ specifies something else to do.
    /// Zero means the default is an error.
    static const unsigned short int yydefact_[];

    static const short int yypgoto_[];
    static const short int yydefgoto_[];

    /// What to do in a state.
    /// \a yytable_[yypact_[s]]: what to do in state \a s.
    /// - if positive, shift that token.
    /// - if negative, reduce the rule which number is the opposite.
    /// - if zero, do what YYDEFACT says.
    static const unsigned short int yytable_[];
    static const signed char yytable_ninf_;

    static const short int yycheck_[];

    /// For a state, its accessing symbol.
    static const unsigned char yystos_[];

    /// For a rule, its LHS.
    static const unsigned char yyr1_[];
    /// For a rule, its RHS length.
    static const unsigned char yyr2_[]; 

#if YYDEBUG
    /// For a symbol, its name in clear.
    static const char* const yytname_[];

    /// A type to store symbol numbers and -1.
    typedef short int rhs_number_type;
    /// A `-1'-separated list of the rules' RHS.
    static const rhs_number_type yyrhs_[];
    /// For each rule, the index of the first RHS symbol in \a yyrhs_.
    static const unsigned short int yyprhs_[];
    /// For each rule, its source line number.
    static const unsigned short int yyrline_[];
    /// For each scanner token number, its symbol number.
    static const unsigned short int yytoken_number_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    /* Debugging.  */
    int yydebug_;
    std::ostream* yycdebug_;
#endif

    /// Convert a scanner token number \a t to a symbol number.
    token_number_type yytranslate_ (int t);

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg        Why this token is reclaimed.
    ///                     If null, do not display the symbol, just free it.
    /// \param yytype       The symbol type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    inline void yydestruct_ (const char* yymsg,
			     int yytype,
			     semantic_type* yyvaluep,
			     location_type* yylocationp);

    /// Pop \a n symbols the three stacks.
    inline void yypop_ (unsigned int n = 1);

    /* Constants.  */
    static const int yyeof_;
    /* LAST_ -- Last index in TABLE_.  */
    static const int yylast_;
    static const int yynnts_;
    static const int yyempty_;
    static const int yyfinal_;
    static const int yyterror_;
    static const int yyerrcode_;
    static const int yyntokens_;
    static const unsigned int yyuser_token_number_max_;
    static const token_number_type yyundef_token_;

    /* User arguments.  */
    BIF::FlexScanner& scanner;
    Options& options;
  };
/* Line 33 of lalr1.cc  */
#line 24 "../s/bif.y"
} // BIF
/* Line 33 of lalr1.cc  */
#line 466 "../bisonflex/bif.tab.hpp"



#endif /* !YY_YY_BISONFLEX_BIF_TAB_HPP_INCLUDED  */
