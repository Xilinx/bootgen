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
 ** \file cmdoptions.tab.hpp
 ** Define the CO::parser class.
 */

/* C++ LALR(1) parser skeleton written by Akim Demaille.  */

#ifndef YY_YY_CMDOPTIONS_TAB_HPP_INCLUDED
# define YY_YY_CMDOPTIONS_TAB_HPP_INCLUDED

/* "%code requires" blocks.  */
/* Line 33 of lalr1.cc  */
#line 33 "../../s/cmdoptions.y"

	namespace CO {
		class BisonScanner;
		class FlexScanner;
		class Parser;
	}
	#include "bootimage.h"
	#include "help.h"
    #include "location.hh"


/* Line 33 of lalr1.cc  */
#line 59 "cmdoptions.tab.hpp"


#include <string>
#include <iostream>
#include "stack.hh"


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Line 33 of lalr1.cc  */
#line 23 "../../s/cmdoptions.y"
namespace CO {
/* Line 33 of lalr1.cc  */
#line 76 "cmdoptions.tab.hpp"

  /// A Bison parser.
  class BisonParser
  {
  public:
    /// Symbol semantic values.
#ifndef YYSTYPE
    union semantic_type
    {
/* Line 33 of lalr1.cc  */
#line 62 "../../s/cmdoptions.y"

    unsigned long number;
    char* cstring;


/* Line 33 of lalr1.cc  */
#line 94 "cmdoptions.tab.hpp"
    };
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef BIF::location location_type;
    /// Tokens.
    struct token
    {
      /* Tokens.  */
   enum yytokentype {
     _IMAGE = 258,
     _FILL = 259,
     _O_TOK = 260,
     I = 261,
     _H = 262,
     _DEBUG_TOK = 263,
     _LEGACY = 264,
     _NONBOOTING = 265,
     _PACKAGENAME = 266,
     _BIF_HELP = 267,
     _LOG = 268,
     ERROR = 269,
     WARNING = 270,
     INFO = 271,
     DEBUG = 272,
     TRACE = 273,
     _SPLIT = 274,
     _PROCESS_BITSTREAM = 275,
     MCS = 276,
     BIN = 277,
     _DUMP = 278,
     _ARCH = 279,
     ZYNQ = 280,
     ZYNQMP = 281,
     _R = 282,
     FPGA = 283,
     _DUAL_QSPI_MODE = 284,
     PARALLEL = 285,
     STACKED = 286,
     _W = 287,
     ON = 288,
     OFF = 289,
     _NOAUTHBLOCKS = 290,
     _ZYNQMPES1 = 291,
     _EFUSEPPKBITS = 292,
     _GENERATE_HASHES = 293,
     _PADIMAGEHEADER = 294,
     _SPKSIGNATURE = 295,
     _GENERATE_KEYS = 296,
     PEM = 297,
     RSA = 298,
     AUTH = 299,
     GREY = 300,
     METAL = 301,
     _ENCRYPT = 302,
     BBRAM = 303,
     EFUSE = 304,
     _P_TOK = 305,
     _READ = 306,
     READ_BH = 307,
     READ_IHT = 308,
     READ_IH = 309,
     READ_PHT = 310,
     READ_AC = 311,
     _VERIFY = 312,
     _VERIFYKDF = 313,
     _ZYNQMPENCRDUMP = 314,
     DECVALUE = 315,
     HEXVALUE = 316,
     IDENTIFIER = 317,
     FILENAME = 318,
     QFILENAME = 319,
     HEXSTRING = 320,
     EQUALS = 321,
     HMAC = 322,
     STARTCBC = 323,
     KEY0 = 324,
     HBIFHELP = 325,
     HARCH = 326,
     HIMAGE = 327,
     HFILL = 328,
     HO = 329,
     HP = 330,
     HW = 331,
     HEFUSEPPKBITS = 332,
     HGENHASHES = 333,
     HLEGACY = 334,
     HPADHDR = 335,
     H_SPKSIGN = 336,
     HDEBUG = 337,
     HPACKAGE = 338,
     HENCRYPT = 339,
     HGENKEYS = 340,
     HDQSPI = 341,
     HLOG = 342,
     HZYNQMPES1 = 343,
     HPROCESSBIT = 344,
     HNONBOOTING = 345,
     HENCRDUMP = 346,
     HVERIFY = 347,
     H_BIF_INIT = 348,
     H_BIF_UDFBH = 349,
     H_BIF_AES = 350,
     H_BIF_PPK = 351,
     H_BIF_PSK = 352,
     H_BIF_SPK = 353,
     H_BIF_SSK = 354,
     H_BIF_SPKSIGN = 355,
     H_BIF_HIVEC = 356,
     H_BIF_HDRSIGN = 357,
     H_BIF_BI = 358,
     H_BIF_BL = 359,
     H_BIF_PID = 360,
     H_BIF_ENCR = 361,
     H_BIF_AUTH = 362,
     H_BIF_CHKSM = 363,
     H_BIF_ELYHNDOFF = 364,
     H_BIF_BHSIGN = 365,
     H_BIF_POWNER = 366,
     H_BIF_PRESIGN = 367,
     H_BIF_UDF = 368,
     H_BIF_XIP = 369,
     H_BIF_ALIGN = 370,
     H_BIF_OFFSET = 371,
     H_BIF_RES = 372,
     H_BIF_LOAD = 373,
     H_BIF_TZ = 374,
     H_BIF_STARTUP = 375,
     H_BIF_KEYSRC = 376,
     H_BIF_FSBLCFG = 377,
     H_BIF_BOOTDEV = 378,
     H_BIF_DESTCPU = 379,
     H_BIF_DESTDEV = 380,
     H_BIF_EL = 381,
     H_SPLIT = 382,
     H_BIF_AUTHPARAM = 383,
     H_BIF_BHKEY = 384,
     H_BIF_PFW = 385,
     H_BIF_BLOCKS = 386,
     H_BIF_METAL = 387,
     H_BIF_BHIV = 388,
     H_BIF_AUTHBLOCKS = 389,
     H_BIF_BOOTVEC = 390,
     H_BIF_PUFDATA = 391
   };

    };
    /// Token type.
    typedef token::yytokentype token_type;

    /// Build a parser object.
    BisonParser (CO::FlexScanner& scanner_yyarg, Options& options_yyarg);
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
    typedef BIF::stack<state_type>    state_stack_type;
    /// Semantic value stack type.
    typedef BIF::stack<semantic_type> semantic_stack_type;
    /// location stack type.
    typedef BIF::stack<location_type> location_stack_type;

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
    static const signed char yypact_[];
    static const signed char yypact_ninf_;

    /// For a state, default reduction number.
    /// Unless\a  yytable_ specifies something else to do.
    /// Zero means the default is an error.
    static const unsigned char yydefact_[];

    static const signed char yypgoto_[];
    static const short int yydefgoto_[];

    /// What to do in a state.
    /// \a yytable_[yypact_[s]]: what to do in state \a s.
    /// - if positive, shift that token.
    /// - if negative, reduce the rule which number is the opposite.
    /// - if zero, do what YYDEFACT says.
    static const unsigned char yytable_[];
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
    CO::FlexScanner& scanner;
    Options& options;
  };
/* Line 33 of lalr1.cc  */
#line 23 "../../s/cmdoptions.y"
} // CO
/* Line 33 of lalr1.cc  */
#line 416 "cmdoptions.tab.hpp"



#endif /* !YY_YY_CMDOPTIONS_TAB_HPP_INCLUDED  */
