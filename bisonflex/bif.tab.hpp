// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton interface for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

/**
 ** \file bisonflex/bif.tab.hpp
 ** Define the BIF::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

#ifndef YY_YY_BISONFLEX_BIF_TAB_HPP_INCLUDED
# define YY_YY_BISONFLEX_BIF_TAB_HPP_INCLUDED
// //                    "%code requires" blocks.
#line 37 "parser/bif.y" // lalr1.cc:377

       namespace BIF {
        class BisonScanner;
        class FlexScanner;
        class Parser;
    }
    #include "bootimage.h"
    #include "bifoptions.h"

#line 54 "bisonflex/bif.tab.hpp" // lalr1.cc:377


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>
# include "stack.hh"
# include "location.hh"


#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

#line 24 "parser/bif.y" // lalr1.cc:377
namespace BIF {
#line 126 "bisonflex/bif.tab.hpp" // lalr1.cc:377





  /// A Bison parser.
  class BisonParser
  {
  public:
#ifndef YYSTYPE
    /// Symbol semantic values.
    union semantic_type
    {
    #line 78 "parser/bif.y" // lalr1.cc:377

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
    HashFunction::Type      hashfunc_t;
    AuthKeyLevel::Type      authkeylevel_t;

#line 170 "bisonflex/bif.tab.hpp" // lalr1.cc:377
    };
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m);
      location_type location;
    };

    /// Tokens.
    struct token
    {
      enum yytokentype
      {
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
        SPK_REVOKE_ID = 297,
        SPLIT_MODE = 298,
        SPLIT_FMT = 299,
        BOOT = 300,
        USER = 301,
        STATIC = 302,
        NOAUTOSTART = 303,
        MULTIBOOT = 304,
        PROTECTED = 305,
        BLOCKS = 306,
        AUTHBLOCKS = 307,
        BOOTVECTORS = 308,
        PRESIGN = 309,
        AC = 310,
        BIF_SECTION = 311,
        UDF_DATA = 312,
        MCS = 313,
        BIN = 314,
        SLR_NUM = 315,
        CLUSTER_NUM = 316,
        DICE = 317,
        PCR_NUMBER = 318,
        PCR_MEASUREMENT_INDEX = 319,
        IMAGE_STORE = 320,
        PARENT_ID = 321,
        ID_CODE = 322,
        EXT_ID_CODE = 323,
        BYPASS_IDCODE_CHECK = 324,
        A_HWROT = 325,
        S_HWROT = 326,
        UNIQUE_ID = 327,
        PARENT_UNIQUE_ID = 328,
        FUNCTION_ID = 329,
        IMAGE = 330,
        ID = 331,
        NAME = 332,
        DELAY_HANDOFF = 333,
        DELAY_LOAD = 334,
        COPY = 335,
        INCLUDE = 336,
        DELAY_AUTH = 337,
        PARTITION = 338,
        PFILE = 339,
        OPTIONAL_DATA = 340,
        METAHEADER = 341,
        TCM_BOOT = 342,
        TCM_A_REGION = 343,
        TCM_B_REGION = 344,
        TCM_C_REGION = 345,
        WORD = 346,
        HEXWORD = 347,
        FILENAME = 348,
        QFILENAME = 349,
        NONE = 350,
        DECVALUE = 351,
        HEXVALUE = 352,
        KEYSRC_ENCRYPTION = 353,
        FSBL_CONFIG = 354,
        AUTH_PARAMS = 355,
        AUTHJTAG_CONFIG = 356,
        DEVICE_DNA = 357,
        JTAG_TIMEOUT = 358,
        AUTHJTAG_SIGN = 359,
        LMS_KEY_PARAMS = 360,
        LMS_HASH = 361,
        LMS_H = 362,
        LMS_W = 363,
        SHA256 = 364,
        SHAKE256 = 365,
        PUF4KMODE = 366,
        PUFROSWAP = 367,
        SHUTTER = 368,
        SPLIT = 369,
        SMAP_WIDTH = 370,
        PUF_HELPER_FILE = 371,
        BH_KEY_FILE = 372,
        BH_KEY_IV = 373,
        BH_KEK_IV = 374,
        BBRAM_KEK_IV = 375,
        EFUSE_KEK_IV = 376,
        EFUSE_USER_KEK0_IV = 377,
        EFUSE_USER_KEK1_IV = 378,
        USER_KEYS = 379,
        PMCDATA = 380,
        BOOTIMAGE = 381,
        UDF_BH = 382,
        INIT = 383,
        PMUFW_IMAGE = 384,
        AES_KEY_FILE = 385,
        FAMILY_KEY = 386,
        PPK_FILE = 387,
        PSK_FILE = 388,
        SPK_FILE = 389,
        SSK_FILE = 390,
        SPK_SIGNATURE_FILE = 391,
        BH_SIGNATURE_FILE = 392,
        HEADER_SIGNATURE_FILE = 393,
        AUTHVALUE = 394,
        ENCRVALUE = 395,
        CHECKSUMVALUE = 396,
        POWNERVALUE = 397,
        PTYPEVALUE = 398,
        KEY_SRC = 399,
        CORE = 400,
        BH_RSA = 401,
        AUTH_HASH = 402,
        INT_HASH = 403,
        PUFHD_LOC = 404,
        OPT_KEY = 405,
        AUTH_ONLY = 406,
        BOOT_DEVICE_TYPE = 407,
        DEST_DEVICE_TYPE = 408,
        DEST_CPU_TYPE = 409,
        EXCEPTION_LEVEL_TYPE = 410,
        TRUSTZONE_TYPE = 411,
        SPLITMODE = 412,
        DPA_CM = 413,
        SPKSELECT = 414,
        HASH_FUNCTION = 415,
        AUTH_KEY_LEVEL = 416,
        OR = 417,
        XOR = 418,
        AND = 419,
        MULT = 420,
        DIVIDE = 421,
        MODULO = 422,
        PLUS = 423,
        MINUS = 424,
        LSHIFT = 425,
        RSHIFT = 426,
        NEGATION = 427,
        LPAREN = 428,
        RPAREN = 429,
        ASTERISK = 430
      };
    };

    /// (External) token type, as returned by yylex.
    typedef token::yytokentype token_type;

    /// Symbol type: an internal symbol number.
    typedef int symbol_number_type;

    /// The symbol type number to denote an empty symbol.
    enum { empty_symbol = -2 };

    /// Internal symbol number for tokens (subsumed by symbol_number_type).
    typedef unsigned char token_number_type;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol type
    /// via type_get().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol ();

      /// Copy constructor.
      basic_symbol (const basic_symbol& other);

      /// Constructor for valueless symbols.
      basic_symbol (typename Base::kind_type t,
                    const location_type& l);

      /// Constructor for symbols with semantic value.
      basic_symbol (typename Base::kind_type t,
                    const semantic_type& v,
                    const location_type& l);

      /// Destroy the symbol.
      ~basic_symbol ();

      /// Destroy contents, and record that is empty.
      void clear ();

      /// Whether empty.
      bool empty () const;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      semantic_type value;

      /// The location.
      location_type location;

    private:
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& other);
    };

    /// Type access provider for token (enum) based symbols.
    struct by_type
    {
      /// Default constructor.
      by_type ();

      /// Copy constructor.
      by_type (const by_type& other);

      /// The symbol type as needed by the constructor.
      typedef token_type kind_type;

      /// Constructor from (external) token numbers.
      by_type (kind_type t);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol type from \a that.
      void move (by_type& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_number_type type_get () const;

      /// The token.
      token_type token () const;

      /// The symbol type.
      /// \a empty_symbol when empty.
      /// An int, not token_number_type, to be able to store empty_symbol.
      int type;
    };

    /// "External" symbols: returned by the scanner.
    typedef basic_symbol<by_type> symbol_type;


    /// Build a parser object.
    BisonParser (BIF::FlexScanner& scanner_yyarg, Options& options_yyarg);
    virtual ~BisonParser ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

  private:
    /// This class is not copyable.
    BisonParser (const BisonParser&);
    BisonParser& operator= (const BisonParser&);

    /// State numbers.
    typedef int state_type;

    /// Generate an error message.
    /// \param yystate   the state where the error occurred.
    /// \param yyla      the lookahead token.
    virtual std::string yysyntax_error_ (state_type yystate,
                                         const symbol_type& yyla) const;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue);

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue);

    static const short int yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token number \a t to a symbol number.
    static token_number_type yytranslate_ (int t);

    // Tables.
  // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
  // STATE-NUM.
  static const short int yypact_[];

  // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
  // Performed when YYTABLE does not specify something else to do.  Zero
  // means the default is an error.
  static const unsigned short int yydefact_[];

  // YYPGOTO[NTERM-NUM].
  static const short int yypgoto_[];

  // YYDEFGOTO[NTERM-NUM].
  static const short int yydefgoto_[];

  // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
  // positive, shift that token.  If negative, reduce the rule whose
  // number is the opposite.  If YYTABLE_NINF, syntax error.
  static const unsigned short int yytable_[];

  static const short int yycheck_[];

  // YYSTOS[STATE-NUM] -- The (internal number of the) accessing
  // symbol of state STATE-NUM.
  static const unsigned short int yystos_[];

  // YYR1[YYN] -- Symbol number of symbol that rule YYN derives.
  static const unsigned short int yyr1_[];

  // YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.
  static const unsigned char yyr2_[];


#if YYDEBUG
    /// For a symbol, its name in clear.
    static const char* const yytname_[];

  // YYRLINE[YYN] -- Source line where rule number YYN was defined.
  static const unsigned short int yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();

    // Debugging.
    int yydebug_;
    std::ostream* yycdebug_;

    /// \brief Display a symbol type, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state ();

      /// The symbol type as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s);

      /// Copy constructor.
      by_state (const by_state& other);

      /// Record that this symbol is empty.
      void clear ();

      /// Steal the symbol type from \a that.
      void move (by_state& that);

      /// The (internal) type number (corresponding to \a state).
      /// \a empty_symbol when empty.
      symbol_number_type type_get () const;

      /// The state number used to denote an empty symbol.
      enum { empty_state = -1 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, symbol_type& sym);
      /// Assignment, needed by push_back.
      stack_symbol_type& operator= (const stack_symbol_type& that);
    };

    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, stack_symbol_type& s);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, state_type s, symbol_type& sym);

    /// Pop \a n symbols the three stacks.
    void yypop_ (unsigned int n = 1);

    /// Constants.
    enum
    {
      yyeof_ = 0,
      yylast_ = 1081,     ///< Last index in yytable_.
      yynnts_ = 92,  ///< Number of nonterminal symbols.
      yyfinal_ = 3, ///< Termination state number.
      yyterror_ = 1,
      yyerrcode_ = 256,
      yyntokens_ = 176  ///< Number of tokens.
    };


    // User arguments.
    BIF::FlexScanner& scanner;
    Options& options;
  };


#line 24 "parser/bif.y" // lalr1.cc:377
} // BIF
#line 684 "bisonflex/bif.tab.hpp" // lalr1.cc:377




#endif // !YY_YY_BISONFLEX_BIF_TAB_HPP_INCLUDED
