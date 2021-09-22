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
#line 36 "../s/bif.y"

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
#line 23 "../s/bif.y"
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
#line 72 "../s/bif.y"

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
     AUTHENTICATION = 271,
     ENCRYPTION = 272,
     CHECKSUM = 273,
     PARTITION_OWNER = 274,
     PARTITION_TYPE = 275,
     PARTITION_NUM = 276,
     BOOT_DEVICE = 277,
     DEST_DEVICE = 278,
     DEST_CPU = 279,
     ADDRESS = 280,
     EXCEPTION_LEVEL = 281,
     TRUSTZONE = 282,
     ALIGNMENT = 283,
     OFFSET = 284,
     RESERVE_LEGACY = 285,
     RESERVE = 286,
     LOAD = 287,
     STARTUP = 288,
     BIGENDIAN = 289,
     A32_MODE = 290,
     PPK_SELECT = 291,
     SPK_ID = 292,
     SPK_SELECT = 293,
     HEADER_AUTH = 294,
     REVOKE_ID = 295,
     SPLIT_MODE = 296,
     SPLIT_FMT = 297,
     BOOT = 298,
     USER = 299,
     STATIC = 300,
     NOAUTOSTART = 301,
     MULTIBOOT = 302,
     PROTECTED = 303,
     BLOCKS = 304,
     AUTHBLOCKS = 305,
     BOOTVECTORS = 306,
     PRESIGN = 307,
     BIF_SECTION = 308,
     UDF_DATA = 309,
     MCS = 310,
     BIN = 311,
     SLR_NUM = 312,
     PARENT_ID = 313,
     ID_CODE = 314,
     EXT_ID_CODE = 315,
     BYPASS_IDCODE_CHECK = 316,
     A_HWROT = 317,
     S_HWROT = 318,
     UNIQUE_ID = 319,
     PARENT_UNIQUE_ID = 320,
     FUNCTION_ID = 321,
     IMAGE = 322,
     ID = 323,
     NAME = 324,
     DELAY_HANDOFF = 325,
     DELAY_LOAD = 326,
     COPY = 327,
     PARTITION = 328,
     PFILE = 329,
     METAHEADER = 330,
     WORD = 331,
     HEXWORD = 332,
     FILENAME = 333,
     QFILENAME = 334,
     NONE = 335,
     DECVALUE = 336,
     HEXVALUE = 337,
     KEYSRC_ENCRYPTION = 338,
     FSBL_CONFIG = 339,
     AUTH_PARAMS = 340,
     AUTHJTAG_CONFIG = 341,
     DEVICE_DNA = 342,
     JTAG_TIMEOUT = 343,
     PUF4KMODE = 344,
     SHUTTER = 345,
     SPLIT = 346,
     SMAP_WIDTH = 347,
     PUF_HELPER_FILE = 348,
     BH_KEY_FILE = 349,
     BH_KEY_IV = 350,
     BH_KEK_IV = 351,
     BBRAM_KEK_IV = 352,
     EFUSE_KEK_IV = 353,
     EFUSE_USER_KEK0_IV = 354,
     EFUSE_USER_KEK1_IV = 355,
     USER_KEYS = 356,
     PMCDATA = 357,
     BOOTIMAGE = 358,
     UDF_BH = 359,
     INIT = 360,
     PMUFW_IMAGE = 361,
     AES_KEY_FILE = 362,
     FAMILY_KEY = 363,
     PPK_FILE = 364,
     PSK_FILE = 365,
     SPK_FILE = 366,
     SSK_FILE = 367,
     SPK_SIGNATURE_FILE = 368,
     BH_SIGNATURE_FILE = 369,
     HEADER_SIGNATURE_FILE = 370,
     AUTHVALUE = 371,
     ENCRVALUE = 372,
     CHECKSUMVALUE = 373,
     POWNERVALUE = 374,
     PTYPEVALUE = 375,
     KEY_SRC = 376,
     CORE = 377,
     BH_RSA = 378,
     AUTH_HASH = 379,
     INT_HASH = 380,
     PUFHD_LOC = 381,
     OPT_KEY = 382,
     AUTH_ONLY = 383,
     BOOT_DEVICE_TYPE = 384,
     DEST_DEVICE_TYPE = 385,
     DEST_CPU_TYPE = 386,
     EXCEPTION_LEVEL_TYPE = 387,
     TRUSTZONE_TYPE = 388,
     SPLITMODE = 389,
     DPA_CM = 390,
     SPKSELECT = 391,
     OR = 392,
     XOR = 393,
     AND = 394,
     MULT = 395,
     DIVIDE = 396,
     MODULO = 397,
     PLUS = 398,
     MINUS = 399,
     LSHIFT = 400,
     RSHIFT = 401,
     NEGATION = 402,
     LPAREN = 403,
     RPAREN = 404,
     ASTERISK = 405
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
#line 23 "../s/bif.y"
} // BIF
/* Line 33 of lalr1.cc  */
#line 452 "../bisonflex/bif.tab.hpp"



#endif /* !YY_YY_BISONFLEX_BIF_TAB_HPP_INCLUDED  */
