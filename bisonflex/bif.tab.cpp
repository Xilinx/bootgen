// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

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


// First part of user declarations.
#line 53 "parser/bif.y" // lalr1.cc:404

#include <stdio.h>
#include "bootimage.h"
#include "authentication.h"
#include "encryption.h"
#include "checksum.h"
#include "bifscanner.h"
#include "parsing.h"
#include "imageheadertable-versal.h"
#include "imageheadertable-spartanup.h"
#include "imageheadertable-versal_2ve_2vm.h"

// Parser uses raw pointers for compatibility with Bison semantics
// These are transferred to options lists which take ownership
BifOptions* currentBifOptions;
PartitionBifOptions* currentPartitionBifOptions ;
ImageBifOptions* currentImageBifOptions;
LmsKeyTypeParam lmsParams;

#line 56 "bisonflex/bif.tab.cpp" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "bif.tab.hpp"

// User implementation prologue.

#line 70 "bisonflex/bif.tab.cpp" // lalr1.cc:412
// Unqualified %code blocks.
#line 47 "parser/bif.y" // lalr1.cc:413

    static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner);
    #include "options.h"

#line 77 "bisonflex/bif.tab.cpp" // lalr1.cc:413


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 24 "parser/bif.y" // lalr1.cc:479
namespace BIF {
#line 163 "bisonflex/bif.tab.cpp" // lalr1.cc:479

  /// Build a parser object.
  BisonParser::BisonParser (BIF::FlexScanner& scanner_yyarg, Options& options_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg),
      options (options_yyarg)
  {}

  BisonParser::~BisonParser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  BisonParser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  BisonParser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  BisonParser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  BisonParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  BisonParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  BisonParser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  BisonParser::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  BisonParser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  BisonParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  BisonParser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  BisonParser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  BisonParser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  BisonParser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  BisonParser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  BisonParser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  BisonParser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  BisonParser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  BisonParser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  BisonParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  BisonParser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  BisonParser::symbol_number_type
  BisonParser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  BisonParser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  BisonParser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  BisonParser::stack_symbol_type&
  BisonParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  BisonParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.type_get ())
    {
            case 91: // WORD

#line 109 "parser/bif.y" // lalr1.cc:614
        { free((yysym.value.string)); }
#line 379 "bisonflex/bif.tab.cpp" // lalr1.cc:614
        break;

      case 92: // HEXWORD

#line 109 "parser/bif.y" // lalr1.cc:614
        { free((yysym.value.string)); }
#line 386 "bisonflex/bif.tab.cpp" // lalr1.cc:614
        break;

      case 93: // FILENAME

#line 109 "parser/bif.y" // lalr1.cc:614
        { free((yysym.value.string)); }
#line 393 "bisonflex/bif.tab.cpp" // lalr1.cc:614
        break;

      case 94: // QFILENAME

#line 109 "parser/bif.y" // lalr1.cc:614
        { free((yysym.value.string)); }
#line 400 "bisonflex/bif.tab.cpp" // lalr1.cc:614
        break;

      case 259: // filename

#line 109 "parser/bif.y" // lalr1.cc:614
        { free((yysym.value.string)); }
#line 407 "bisonflex/bif.tab.cpp" // lalr1.cc:614
        break;


      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
  BisonParser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  BisonParser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  BisonParser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  BisonParser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  BisonParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  BisonParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  BisonParser::debug_level_type
  BisonParser::debug_level () const
  {
    return yydebug_;
  }

  void
  BisonParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  inline BisonParser::state_type
  BisonParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  BisonParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  BisonParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  BisonParser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    // User initialization code.
    #line 31 "parser/bif.y" // lalr1.cc:741
{
       // Initialize the initial location.
       yyla.location.begin.filename = yyla.location.end.filename = &scanner.filename;
}

#line 545 "bisonflex/bif.tab.cpp" // lalr1.cc:741

    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location, scanner));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 5:
#line 217 "parser/bif.y" // lalr1.cc:859
    { options.includeBifOptionsList.push_back((yystack_[0].value.string)); }
#line 655 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 6:
#line 218 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions = new BifOptions(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series(), (yystack_[0].value.string)); }
#line 661 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 7:
#line 220 "parser/bif.y" // lalr1.cc:859
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
#line 668 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 16:
#line 234 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(), currentBifOptions->GetIdCode()); }
#line 675 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 22:
#line 245 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetMetaHeaderEncryptType((yystack_[0].value.encrvalue_t)); }
#line 681 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 23:
#line 246 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yystack_[0].value.encrkeysrc_t)); }
#line 687 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 24:
#line 247 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetMetaHeaderEncryptionKeyFile((yystack_[0].value.string)); }
#line 693 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 25:
#line 248 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetMetaHeaderAuthType((yystack_[0].value.authvalue_t)); }
#line 699 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 26:
#line 249 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.ppk = (yystack_[0].value.string); }
#line 705 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 27:
#line 250 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.psk = (yystack_[0].value.string); }
#line 711 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 28:
#line 251 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.spk = (yystack_[0].value.string); }
#line 717 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 29:
#line 252 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.ssk = (yystack_[0].value.string); }
#line 723 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 30:
#line 253 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.spkSignature = (yystack_[0].value.string); }
#line 729 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 31:
#line 254 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.presign = (yystack_[0].value.string); }
#line 735 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 32:
#line 255 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.partitionRevokeId = (yystack_[0].value.number);}
#line 741 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 33:
#line 256 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.spkRevokeId = (yystack_[0].value.number); 
                                                                                if(options.GetArchType() == Arch::VERSAL)
                                                                                {
                                                                                   LOG_ERROR("SPK_REVOKE_ID does not support for versal arch, instead please use revoke_id option.");
                                                                                }
                                                                                }
#line 752 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 34:
#line 263 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.checksum = (yystack_[0].value.checksumvalue_t); }
#line 758 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 35:
#line 264 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
#line 764 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 37:
#line 266 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
#line 771 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 40:
#line 274 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->metaHdrAttributes.ihtOptionalDataInfo.push_back(std::pair<std::string, uint32_t>((yystack_[4].value.string), (yystack_[0].value.number))); }
#line 777 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 44:
#line 281 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetEncryptionBlocks((yystack_[0].value.number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
#line 784 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 45:
#line 283 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetEncryptionBlocks((yystack_[3].value.number), (yystack_[1].value.number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
#line 791 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 46:
#line 285 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetEncryptionBlocks((yystack_[3].value.number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yystack_[3].value.number); }
#line 798 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 47:
#line 289 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetPdiId((yystack_[0].value.number)); }
#line 804 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 48:
#line 290 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetParentId((yystack_[0].value.number)); }
#line 810 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 49:
#line 291 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetIdCode((yystack_[0].value.number)); 
                                                                                  options.SetDl9Series((yystack_[0].value.number)); }
#line 817 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 50:
#line 293 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetExtendedIdCode((yystack_[0].value.number)); }
#line 823 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 51:
#line 294 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->AddFiles((yystack_[2].value.number), (yystack_[0].value.string)); }
#line 829 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 52:
#line 295 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetEncryptionKeySource((yystack_[0].value.encrkeysrc_t)); }
#line 835 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 53:
#line 296 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetPdiType((yystack_[0].value.ptype_t)); }
#line 841 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 54:
#line 297 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetRevokeId((yystack_[0].value.number));}
#line 847 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 57:
#line 304 "parser/bif.y" // lalr1.cc:859
    { currentImageBifOptions = new ImageBifOptions(); }
#line 853 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 58:
#line 305 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions);
                                                                                  if((options.archType == Arch::SPARTANUP) && (currentBifOptions->imageBifOptionList.size() > 1))
                                                                                  {
                                                                                      LOG_ERROR("'-arch spartanup' cannot accomodate more than one Sub-System.");
                                                                                  }
                                                                                }
#line 864 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 66:
#line 324 "parser/bif.y" // lalr1.cc:859
    { currentImageBifOptions->SetImageId((yystack_[0].value.number)); }
#line 870 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 67:
#line 325 "parser/bif.y" // lalr1.cc:859
    { currentImageBifOptions->SetImageName((yystack_[0].value.string)); }
#line 876 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 68:
#line 326 "parser/bif.y" // lalr1.cc:859
    { currentImageBifOptions->SetDelayHandoff(true); }
#line 882 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 69:
#line 327 "parser/bif.y" // lalr1.cc:859
    { currentImageBifOptions->SetDelayLoad(true); }
#line 888 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 70:
#line 328 "parser/bif.y" // lalr1.cc:859
    { LOG_ERROR("BIF attribute error !!!\n\t This usage of 'init' is not supported. See 'bootgen -bif_help init' for usage details."); }
#line 894 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 71:
#line 329 "parser/bif.y" // lalr1.cc:859
    { LOG_ERROR("Copy to Memory feature with the attribute 'copy' is no more supported.\n\t   This can be duplicated with the option 'imagestore'. Please refer UG1283 for more details.");
                                                                                  currentImageBifOptions->SetMemCopyAddress((yystack_[0].value.number)); }
#line 901 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 72:
#line 331 "parser/bif.y" // lalr1.cc:859
    { currentImageBifOptions->SetImageType((yystack_[0].value.ptype_t)); }
#line 907 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 73:
#line 332 "parser/bif.y" // lalr1.cc:859
    { if ((yystack_[0].value.number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("uid value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetUniqueId((yystack_[0].value.number)); }
#line 917 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 74:
#line 337 "parser/bif.y" // lalr1.cc:859
    { if ((yystack_[0].value.number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("parent_uid value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetParentUniqueId((yystack_[0].value.number)); }
#line 927 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 75:
#line 342 "parser/bif.y" // lalr1.cc:859
    { if ((yystack_[0].value.number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("function_id value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetFunctionId((yystack_[0].value.number)); }
#line 937 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 76:
#line 347 "parser/bif.y" // lalr1.cc:859
    { if (options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP)
                                                                                  {
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr' is not supported for the specified architecture");
                                                                                  }
                                                                                  else if(options.GetArchType() == Arch::VERSAL && !options.IsVersalNetSeries())
                                                                                  {
                                                                                    currentImageBifOptions->SetPcrNumber((yystack_[0].value.number), false);
                                                                                  }
                                                                                  else if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                  {
                                                                                    currentImageBifOptions->SetPcrNumber((yystack_[0].value.number), true);
                                                                                  }
                                                                                  else if(options.GetArchType() == Arch::VERSALGEN2)
                                                                                  {
                                                                                    currentImageBifOptions->SetPcrNumber((yystack_[0].value.number), options.IsVersalNetSeries());
                                                                                  }
                                                                                }
#line 959 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 77:
#line 364 "parser/bif.y" // lalr1.cc:859
    { if (options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP)
                                                                                  {
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr measurement index' is not supported for the specified architecture");
                                                                                  }
                                                                                  else if(options.GetArchType() == Arch::VERSAL || options.GetArchType() == Arch::VERSALGEN2)
                                                                                  {
                                                                                    currentImageBifOptions->SetPcrMeasurementIndex((yystack_[0].value.number));
                                                                                  }
                                                                                }
#line 973 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 83:
#line 384 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() != Arch::ZYNQ || options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported for the specified architecture.\n\t   Refer 'bootgen -bif_help' for more details");
                                                                                  currentBifOptions->SetEncryptionKeySource((yystack_[0].value.encrkeysrc_t)); options.SetEncryptedKeySource((yystack_[0].value.encrkeysrc_t)); }
#line 981 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 84:
#line 387 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
#line 988 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 88:
#line 392 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                  if(options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); 
                                                                                  currentBifOptions->SetBootDevice((yystack_[0].value.bootdevice_t)); }
#line 998 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 89:
#line 397 "parser/bif.y" // lalr1.cc:859
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); }
#line 1004 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 100:
#line 412 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetBootDevice((yystack_[0].value.bootdevice_t)); }
#line 1010 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 101:
#line 413 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetBootDevice(BootDevice::IMAGESTORE); }
#line 1016 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 102:
#line 414 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetBootDeviceAddress((yystack_[0].value.number)); }
#line 1022 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 109:
#line 427 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetAuthJtagRevokeID((yystack_[0].value.number)); }
#line 1028 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 110:
#line 428 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetAuthJtagSPKRevokeID((yystack_[0].value.number)); }
#line 1034 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 111:
#line 429 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetAuthJtagDeviceDna((yystack_[0].value.string)); }
#line 1040 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 112:
#line 430 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetAuthJtagTimeOut((yystack_[0].value.number)); }
#line 1046 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 113:
#line 431 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetAuthJtagSignatureFile((yystack_[0].value.string)); }
#line 1052 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 114:
#line 434 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetCore((yystack_[0].value.core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
#line 1059 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 115:
#line 437 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetBhRsa((yystack_[0].value.bhrsa_t)); }
#line 1065 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 116:
#line 439 "parser/bif.y" // lalr1.cc:859
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
#line 1071 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 117:
#line 441 "parser/bif.y" // lalr1.cc:859
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
#line 1077 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 118:
#line 443 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetPufHdLoc((yystack_[0].value.pufhdloc_t)); }
#line 1083 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 119:
#line 445 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetAuthOnly((yystack_[0].value.authonly_t)); }
#line 1089 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 120:
#line 447 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetOptKey((yystack_[0].value.optkey_t)); }
#line 1095 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 121:
#line 449 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
#line 1101 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 122:
#line 451 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetShutterValue((yystack_[0].value.number)); }
#line 1107 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 123:
#line 453 "parser/bif.y" // lalr1.cc:859
    {  currentBifOptions->SetDpaCM((yystack_[0].value.dpacm_t));}
#line 1113 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 124:
#line 455 "parser/bif.y" // lalr1.cc:859
    { if(((yystack_[0].value.number) != 8) && ((yystack_[0].value.number) !=16) && ((yystack_[0].value.number) != 32) && ((yystack_[0].value.number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->SetSmapWidth((yystack_[0].value.number));
                                                                                }
#line 1122 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 125:
#line 459 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetBypassIdcodeFlag(true); }
#line 1128 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 126:
#line 460 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetAHwRoTFlag(true); }
#line 1134 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 127:
#line 461 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetSHwRoTFlag(true); }
#line 1140 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 128:
#line 462 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetPufRingOscilltorSwapConfigValue((yystack_[0].value.number)); }
#line 1146 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 129:
#line 463 "parser/bif.y" // lalr1.cc:859
    { currentBifOptions->SetDiceEnable(); }
#line 1152 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 130:
#line 466 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(),options.IsVersalNetSeries(),  currentBifOptions->GetIdCode()); }
#line 1159 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 131:
#line 469 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->filename = (yystack_[0].value.string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yystack_[0].value.string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
#line 1168 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 132:
#line 473 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode());
                                                                                  currentPartitionBifOptions->filename = (yystack_[0].value.string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yystack_[0].value.string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
#line 1179 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 133:
#line 479 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode()); }
#line 1186 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 141:
#line 493 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->filename = (yystack_[0].value.string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yystack_[0].value.string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
#line 1194 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 142:
#line 496 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->partitionId = (yystack_[0].value.number); }
#line 1200 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 143:
#line 497 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->imageStoreId = (yystack_[0].value.number);
                                                                                  currentPartitionBifOptions->SetPartitionType(PartitionType::IMAGE_STORE_PDI); }
#line 1207 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 145:
#line 500 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->fileType = (yystack_[0].value.number); }
#line 1213 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 146:
#line 501 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->bifSection = (yystack_[0].value.string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yystack_[0].value.string), currentImageBifOptions->GetImageType());
                                                                                  currentPartitionBifOptions->filelist.push_back(currentPartitionBifOptions->filename);
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
#line 1222 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 160:
#line 530 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] is supported only for ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yystack_[0].value.number)); }
#line 1230 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 161:
#line 535 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetAuthBlockAttr((yystack_[0].value.number)); }
#line 1236 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 162:
#line 537 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->bootloader = true;}
#line 1242 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 163:
#line 538 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->boot = true;}
#line 1248 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 164:
#line 539 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->user = true;}
#line 1254 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 165:
#line 540 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->Static = true;}
#line 1260 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 166:
#line 541 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->noautostart = true;}
#line 1266 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 167:
#line 542 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->multiboot = true;}
#line 1272 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 168:
#line 543 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->Protected = true;}
#line 1278 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 169:
#line 544 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
#line 1284 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 170:
#line 545 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetHivec(true); }
#line 1290 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 171:
#line 546 "parser/bif.y" // lalr1.cc:859
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
#line 1298 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 172:
#line 549 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->fileType = (yystack_[0].value.number); }
#line 1304 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 173:
#line 550 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->bootImage = true; }
#line 1310 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 174:
#line 551 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->fileType = (yystack_[0].value.number); }
#line 1316 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 175:
#line 552 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->fileType = (yystack_[0].value.number); }
#line 1322 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 176:
#line 553 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetPartitionType((yystack_[0].value.ptype_t)); }
#line 1328 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 177:
#line 554 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetLockStepFlag();}
#line 1334 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 178:
#line 557 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
#line 1340 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 179:
#line 558 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetTrustZone((yystack_[0].value.trustzone_t)); }
#line 1346 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 180:
#line 561 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetEncryptionBlocks((yystack_[0].value.number)); }
#line 1352 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 181:
#line 562 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetEncryptionBlocks((yystack_[3].value.number), (yystack_[1].value.number)); }
#line 1358 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 182:
#line 563 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetEncryptionBlocks((yystack_[3].value.number), 0); }
#line 1364 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 183:
#line 565 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetAuthType((yystack_[0].value.authvalue_t)); }
#line 1370 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 184:
#line 566 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetEncryptType((yystack_[0].value.encrvalue_t)); }
#line 1376 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 185:
#line 567 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetChecksumType((yystack_[0].value.checksumvalue_t)); }
#line 1382 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 186:
#line 568 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetOwnerType((yystack_[0].value.powner_t)); }
#line 1388 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 187:
#line 569 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetDestCpu((yystack_[0].value.destcpu_t)); }
#line 1394 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 188:
#line 570 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetDestDevice((yystack_[0].value.destdevice_t));  }
#line 1400 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 189:
#line 571 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetExceptionLevel((yystack_[0].value.el_t)); }
#line 1406 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 190:
#line 572 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetAesKeyFile((yystack_[0].value.string)); }
#line 1412 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 191:
#line 573 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->ppkFile = ((yystack_[0].value.string)); }
#line 1418 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 192:
#line 574 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->pskFile = ((yystack_[0].value.string)); }
#line 1424 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 193:
#line 575 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->spkFile = ((yystack_[0].value.string)); }
#line 1430 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 194:
#line 576 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->sskFile = ((yystack_[0].value.string)); }
#line 1436 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 195:
#line 577 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->spkSelect =((yystack_[0].value.spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
#line 1442 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 196:
#line 578 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetSpkId((yystack_[0].value.number)); }
#line 1448 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 197:
#line 579 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->spkSignatureFile = ((yystack_[0].value.string)); }
#line 1454 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 199:
#line 581 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetPartitionType((yystack_[0].value.ptype_t)); }
#line 1460 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 200:
#line 582 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetEncryptionKeySource((yystack_[0].value.encrkeysrc_t)); }
#line 1466 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 201:
#line 583 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetPartitionRevokeId((yystack_[0].value.number)); }
#line 1472 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 202:
#line 584 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetSPKRevokeId((yystack_[0].value.number)); }
#line 1478 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 203:
#line 585 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
#line 1484 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 204:
#line 586 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetSlrNum((yystack_[0].value.number)); }
#line 1490 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 205:
#line 587 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetClusterNum((yystack_[0].value.number)); }
#line 1496 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 206:
#line 588 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
#line 1502 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 207:
#line 589 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetDelayAuth(true); }
#line 1508 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 208:
#line 590 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetTcmBootFlag(); }
#line 1514 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 218:
#line 604 "parser/bif.y" // lalr1.cc:859
    { (yylhs.value.authvalue_t) = ::Authentication::None;}
#line 1520 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 220:
#line 608 "parser/bif.y" // lalr1.cc:859
    { (yylhs.value.encrvalue_t) = ::Encryption::None;}
#line 1526 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 222:
#line 612 "parser/bif.y" // lalr1.cc:859
    { (yylhs.value.checksumvalue_t) = ::Checksum::None;}
#line 1532 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 239:
#line 661 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->alignment = (yystack_[0].value.number); }
#line 1538 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 240:
#line 662 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->offset = (yystack_[0].value.number); }
#line 1544 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 241:
#line 663 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetReserveLength((yystack_[0].value.number), false); }
#line 1550 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 242:
#line 664 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetReserveLength((yystack_[0].value.number), true); }
#line 1556 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 243:
#line 665 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->load = (yystack_[0].value.number); }
#line 1562 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 244:
#line 666 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->startup = (yystack_[0].value.number); }
#line 1568 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 245:
#line 667 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->bigEndian = true; }
#line 1574 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 246:
#line 668 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->a32Mode = true; }
#line 1580 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 247:
#line 669 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->pid = (yystack_[0].value.number); }
#line 1586 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 248:
#line 670 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetTcmARegion((yystack_[0].value.number)); }
#line 1592 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 249:
#line 671 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetTcmBRegion((yystack_[0].value.number)); }
#line 1598 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 250:
#line 672 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetTcmCRegion((yystack_[0].value.number)); }
#line 1604 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 251:
#line 675 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->presignFile = (yystack_[0].value.string); }
#line 1610 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 252:
#line 676 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->acFile = (yystack_[0].value.string); }
#line 1616 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 253:
#line 677 "parser/bif.y" // lalr1.cc:859
    { currentPartitionBifOptions->SetUdfDataFile((yystack_[0].value.string)); }
#line 1622 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 270:
#line 702 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yystack_[0].value.number)); }
#line 1630 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 271:
#line 705 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yystack_[0].value.spkselect_t)); }
#line 1638 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 272:
#line 708 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yystack_[0].value.number));  }
#line 1646 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 273:
#line 711 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
#line 1654 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 282:
#line 732 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP || options.GetArchType() == Arch::VERSAL)
                                                                   LOG_ERROR("BIF attribute error !!!\n\t\t'lms_key_params' is not supported with the mentioned -arch");
                                                                 lmsParams.lms_param = (yystack_[0].value.string);
                                                                 currentBifOptions->SetLmsParameters(lmsParams) ;
                                                               }
#line 1664 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 284:
#line 739 "parser/bif.y" // lalr1.cc:859
    { lmsParams.type = (yystack_[1].value.authkeylevel_t);}
#line 1670 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 287:
#line 743 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitMode((yystack_[0].value.splitmode_t)); }
#line 1678 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 291:
#line 753 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
#line 1686 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 292:
#line 756 "parser/bif.y" // lalr1.cc:859
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
#line 1694 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 297:
#line 768 "parser/bif.y" // lalr1.cc:859
    { (yylhs.value.number) = (yystack_[1].value.number); }
#line 1700 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 299:
#line 775 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) =  (yystack_[0].value.number);     *options.debugstr << (yylhs.value.number) << "    + " << (yystack_[0].value.number) << std::endl;}
#line 1706 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 300:
#line 776 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) = ~(yystack_[0].value.number);     *options.debugstr << (yylhs.value.number) << "    ~ " << (yystack_[0].value.number) << std::endl;}
#line 1712 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 302:
#line 781 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) = (yystack_[2].value.number) *  (yystack_[0].value.number); *options.debugstr << (yylhs.value.number) << " = " << (yystack_[2].value.number)  << " + " << (yystack_[0].value.number) << std::endl;}
#line 1718 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 303:
#line 782 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) = (yystack_[2].value.number) /  (yystack_[0].value.number); *options.debugstr << (yylhs.value.number) << " = " << (yystack_[2].value.number)  << " / " << (yystack_[0].value.number) << std::endl;}
#line 1724 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 304:
#line 783 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) = (yystack_[2].value.number) %  (yystack_[0].value.number); *options.debugstr << (yylhs.value.number) << " = " << (yystack_[2].value.number)  << " % " << (yystack_[0].value.number) << std::endl;}
#line 1730 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 306:
#line 787 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) = (yystack_[2].value.number) +  (yystack_[0].value.number);*options.debugstr << (yylhs.value.number) << " = " << (yystack_[2].value.number)  << " + " << (yystack_[0].value.number) << std::endl;}
#line 1736 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 307:
#line 788 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) = (yystack_[2].value.number) -  (yystack_[0].value.number);*options.debugstr << (yylhs.value.number) << " = " << (yystack_[2].value.number)  << " - " << (yystack_[0].value.number) << std::endl;}
#line 1742 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 309:
#line 792 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) = (yystack_[2].value.number) << (yystack_[0].value.number);*options.debugstr << (yylhs.value.number) << " = " << (yystack_[2].value.number)  << " << " << (yystack_[0].value.number) << std::endl;}
#line 1748 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 310:
#line 793 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) = (yystack_[2].value.number) >> (yystack_[0].value.number);*options.debugstr << (yylhs.value.number) << " = " << (yystack_[2].value.number)  << " >> " << (yystack_[0].value.number) << std::endl;}
#line 1754 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 312:
#line 797 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) = (yystack_[2].value.number) &  (yystack_[0].value.number);*options.debugstr << (yylhs.value.number) << " = " << (yystack_[2].value.number)  << " & " << (yystack_[0].value.number) << std::endl;}
#line 1760 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 314:
#line 801 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) = (yystack_[2].value.number) ^  (yystack_[0].value.number);*options.debugstr << (yylhs.value.number) << " = " << (yystack_[2].value.number)  << " ^ " << (yystack_[0].value.number) << std::endl;}
#line 1766 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;

  case 316:
#line 806 "parser/bif.y" // lalr1.cc:859
    {(yylhs.value.number) = (yystack_[2].value.number) |  (yystack_[0].value.number);*options.debugstr << (yylhs.value.number) << " = " << (yystack_[2].value.number)  << " | " << (yystack_[0].value.number) << std::endl;}
#line 1772 "bisonflex/bif.tab.cpp" // lalr1.cc:859
    break;


#line 1776 "bisonflex/bif.tab.cpp" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  BisonParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  BisonParser::yysyntax_error_ (state_type, const symbol_type&) const
  {
    return YY_("syntax error");
  }


  const short int BisonParser::yypact_ninf_ = -463;

  const signed char BisonParser::yytable_ninf_ = -1;

  const short int
  BisonParser::yypact_[] =
  {
    -463,    19,    12,  -463,    14,  -463,  -463,    18,    47,  -463,
    -463,  -463,    79,  -463,   890,  -463,  -463,   246,    78,   101,
      99,   103,   117,   144,   112,   149,  -463,   118,   157,   159,
     160,   167,   206,  -463,  -463,  -463,  -463,  -463,  -463,  -463,
    -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,
    -463,  -463,  -463,  -463,    97,  -463,  -463,     8,  -463,  -463,
    -463,   182,  -463,  -463,  -463,   645,   190,   212,   214,  -463,
     230,   235,   792,   120,    24,   116,   169,   169,   169,   169,
    -463,   169,     8,    18,  -463,   135,   923,    27,    67,  -463,
    -463,  -463,  -463,    18,  -463,  -463,  -463,  -463,  -463,   282,
     286,   288,   291,   307,   308,   310,   319,   324,   325,   326,
     327,   329,   330,   333,   334,  -463,  -463,   341,   342,   344,
     345,  -463,  -463,  -463,  -463,  -463,  -463,   346,   347,   348,
     349,   350,   352,   355,   356,   358,   361,  -463,   362,  -463,
     369,   370,   371,   372,  -463,  -463,  -463,  -463,  -463,  -463,
     376,   377,   378,   379,   380,   381,  -463,  -463,  -463,   384,
      10,   498,  -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,
    -463,   116,   169,   135,   387,   192,    75,   386,   388,   354,
    -463,   402,  -463,  -463,   405,   408,  -463,  -463,  -463,  -463,
     169,   169,   169,  -463,  -463,    16,    49,    64,   250,   252,
     256,   256,   256,   256,  -463,   256,   415,   411,   416,   185,
    -463,  -463,  -463,  -463,  -463,  -463,  -463,   417,   418,   422,
    -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,   430,   109,
    -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,   429,   431,
     432,   433,   434,   439,    83,  -463,   438,   442,   443,   419,
     446,   444,   239,     3,  -463,   -48,   -23,   -62,   309,   357,
     169,   299,   300,   306,   312,   169,   169,   169,   169,   169,
     169,   169,   311,   169,   169,   169,   169,    18,    18,   373,
      18,   169,   169,   169,   169,    18,   169,   169,   169,   135,
      18,    18,    18,    18,    18,    18,  -463,   645,  -463,   645,
    -463,  -463,  -463,   458,   256,  -463,   923,   460,   463,   465,
    -463,   469,   474,   490,   492,   120,    18,   792,   169,  -463,
      24,  -463,  -463,   -89,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   353,  -463,    18,   425,   515,   518,
     529,   532,   535,   536,   545,   550,   551,   554,   555,   556,
     558,   559,  -463,  -463,   562,   115,   169,   169,   169,  -463,
     923,  -463,   169,   169,   475,   169,    18,  -463,    27,  -463,
     409,   169,   169,  -463,  -463,   194,  -463,   477,  -463,   410,
    -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,
    -463,  -463,  -463,  -463,   256,  -463,  -463,  -463,  -463,  -463,
    -463,  -463,   256,   256,   256,   256,   256,   256,   256,  -463,
    -463,   256,   256,  -463,   561,   -99,  -463,   256,  -463,  -463,
    -463,  -463,   256,   256,   256,   256,  -463,   256,   256,   256,
    -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,   169,
    -463,   169,   169,   311,   192,   420,   226,    75,  -463,  -463,
    -463,   256,  -463,  -463,  -463,  -463,  -463,    16,    16,    49,
      49,    64,   250,   252,  -463,   566,   567,   569,   570,   572,
     573,   575,   577,  -463,  -463,   578,  -463,  -463,   317,  -463,
    -463,  -463,   583,   -48,   -23,   -62,   169,   169,   169,    18,
     135,    18,    18,    18,    18,    18,    18,  -463,   185,  -463,
     256,   256,   256,  -463,   256,   256,  -463,   256,  -463,  -463,
    -463,  -463,   256,   256,   477,  -463,   585,   169,    65,  -463,
     256,   256,  -463,   469,  -463,  -463,  -463,  -463,  -463,   492,
     120,   169,   169,   169,   169,   169,   169,   500,   169,   198,
    -463,   169,  -463,  -463,  -463,   256,   256,  -463,   584,   -86,
    -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,   305,
    -463,   421,   -84,  -463,   256,   256,   256,   256,   256,   256,
    -463,   256,  -463,   256,   169,   304,  -463,  -463,  -463,  -463,
     423,   -79,  -463,  -463
  };

  const unsigned short int
  BisonParser::yydefact_[] =
  {
       3,     0,     2,     1,     0,     6,     4,     0,     0,   293,
     294,     5,     0,     8,     0,   133,     7,   130,     0,     0,
       0,     0,     0,     0,     0,     0,    80,     0,     0,     0,
       0,     0,     0,   260,   259,   212,   211,   213,   214,   215,
     216,   217,   209,   254,   262,   263,   264,   265,   266,   256,
     257,   258,    12,    13,    11,    55,    15,    79,    10,     9,
      14,     0,   210,   255,   132,     0,     0,     0,     0,    84,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      57,     0,    78,    41,    16,     0,     0,     0,     0,    56,
     130,    81,    82,     0,   162,   171,   169,   170,   177,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   178,     0,
       0,     0,     0,     0,     0,   245,   246,     0,     0,     0,
       0,   163,   164,   165,   166,   167,   168,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   207,     0,   208,
       0,     0,     0,     0,   261,   268,   173,   269,   172,   267,
     254,   263,   264,   265,   266,   256,   225,   206,   203,     0,
     136,   135,   153,   149,   198,   150,   176,   151,   152,   174,
     175,     0,     0,     0,     0,     0,     0,     0,     0,   147,
      53,     0,   101,   235,     0,    98,   100,    89,   296,   295,
       0,     0,     0,   298,   301,   305,   308,   311,   313,   315,
      54,    48,    49,    50,    59,    47,     0,    38,     0,    21,
     226,    52,   129,   125,   126,   127,   121,     0,     0,     0,
     227,   228,   230,   231,   232,   233,   234,   229,     0,   103,
     114,   115,   123,   116,   117,   118,   120,   119,     0,     0,
       0,     0,     0,     0,   106,   282,     0,     0,     0,     0,
       0,   276,     0,     0,    51,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   134,     0,   139,     0,
     140,    88,    92,   158,   160,    83,     0,     0,     0,     0,
     273,    90,     0,     0,    91,     0,     0,     0,     0,    87,
       0,   299,   300,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    80,    94,    41,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    37,    35,     0,    18,     0,     0,     0,    86,
       0,   105,     0,     0,     0,     0,     0,    93,     0,   108,
       0,     0,     0,   284,    95,     0,    96,     0,    97,   286,
     218,   219,   183,   220,   221,   184,   222,   223,   185,   224,
     186,   145,   144,   176,   247,   237,   188,   236,   187,   238,
     189,   179,   239,   240,   241,   242,   243,   244,   196,   275,
     195,   201,   202,   154,   156,   180,   155,   161,   251,   252,
     146,   253,   204,   205,   143,   142,   141,   248,   249,   250,
     200,   190,   191,   192,   193,   194,   197,   137,   138,     0,
      85,     0,     0,     0,     0,     0,     0,     0,   199,   131,
     148,   102,    99,   297,   302,   303,   304,   306,   307,   309,
     310,   312,   314,   316,    58,     0,     0,     0,     0,     0,
       0,     0,     0,    68,    69,     0,    70,    60,    63,    62,
      61,    39,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    17,    21,    20,
     128,   122,   124,   104,   109,   110,   111,   112,   113,   107,
     281,   278,   280,   279,     0,   277,   283,     0,     0,   159,
     270,   272,   271,   274,   290,   287,   291,   292,   288,   289,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      65,     0,    25,    22,    34,    32,    33,    36,    42,    44,
      31,    23,    24,    26,    27,    28,    29,    30,    19,     0,
     157,     0,     0,    72,    76,    77,    73,    74,    75,    66,
      67,    71,    64,    40,     0,     0,   285,   182,   181,    43,
       0,     0,    46,    45
  };

  const short int
  BisonParser::yypgoto_[] =
  {
    -463,  -463,  -463,  -463,  -463,  -463,  -463,  -463,  -346,  -463,
     258,  -463,    25,  -463,  -463,  -463,   538,  -463,  -463,  -462,
    -463,   264,   574,  -463,  -463,   281,  -463,  -193,  -191,  -463,
    -463,   -10,  -463,   588,  -463,  -148,  -463,   287,   -64,  -463,
      86,   166,  -463,  -463,   351,  -463,  -463,  -463,  -463,   123,
     124,   122,  -463,   -63,  -167,  -463,  -463,  -463,  -463,  -463,
    -463,  -463,  -463,   -61,  -463,  -463,  -463,  -463,  -463,   595,
    -463,  -463,   168,   170,  -463,   236,  -463,  -360,   530,  -463,
     172,  -463,  -463,    13,  -463,  -110,   -16,    -2,   289,   285,
     313,   -76
  };

  const short int
  BisonParser::yydefgoto_[] =
  {
      -1,     1,     2,     6,     8,    14,    52,   209,   354,   355,
     206,   207,   547,   548,    53,    54,    55,   204,   334,   477,
     478,    56,    57,    58,   174,   184,   185,   228,   243,   244,
     229,    91,    72,    92,    65,   159,   160,   178,   161,   162,
     413,   302,   303,   416,   163,   164,   414,   165,    61,   382,
     385,   388,   390,   166,   211,   230,   231,   232,   233,   234,
     235,   236,   237,   186,   398,   396,   400,   167,   168,   169,
      63,   170,   311,   410,   250,   251,   511,   252,   379,   514,
     314,   525,   528,    64,   193,   194,   195,   196,   197,   198,
     199,   304
  };

  const unsigned short int
  BisonParser::yytable_[] =
  {
     200,   201,   202,   203,    59,   205,   305,   378,   179,   499,
     180,    15,   298,   300,   187,   297,   540,   516,    90,     3,
      11,     7,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   386,   105,   106,   361,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   380,   117,   118,
     181,   119,   120,   369,    12,   121,   122,   123,   124,   125,
     126,   127,   128,   333,   129,   130,   131,   132,   238,   239,
     133,   134,   383,   333,   518,   135,   333,   572,   333,   387,
     321,   322,    13,   333,    73,   453,   136,   575,   368,   182,
     578,   381,   137,     4,   138,   583,   208,   139,   140,   141,
     142,     9,    10,     5,    74,    76,   254,    75,   143,    77,
     301,     9,    10,   440,   360,    80,   323,   384,   312,   313,
     498,    83,   430,    78,   238,   239,    33,    34,   144,   240,
     241,   242,   338,   339,   340,   145,   146,   147,   148,   149,
     150,    44,   151,   152,   153,   154,   155,    50,    51,   437,
      79,   438,   558,   156,   559,    81,   341,   342,   245,   157,
      84,   188,   189,    86,   249,    85,   343,   503,   158,   344,
      87,   212,    24,   246,   247,   248,   183,   509,   213,   214,
     215,   324,   325,   326,   394,   240,   241,   242,    93,   402,
     403,   404,   405,   406,   407,   408,   393,   411,   412,   415,
     417,   171,   338,   339,   340,   422,   423,   424,   425,    88,
     427,   428,   429,   345,   454,   455,   456,   327,   328,   465,
     216,   217,   218,   172,   219,   173,   341,   342,   249,   307,
     308,   309,   310,   190,   329,   330,   343,   191,   192,   344,
     561,   175,   451,   376,   377,   346,   176,   347,   348,   349,
     350,   351,   448,   179,   220,   221,   222,   223,   224,   225,
     226,   466,   467,   156,   352,   188,   189,   227,   183,    66,
     468,   469,   470,   353,   471,   472,   473,   474,   475,   210,
     500,   501,   502,   345,   526,   527,   504,   505,   255,   507,
     418,   419,   256,   421,   257,   512,   513,   258,   426,    67,
     246,   247,   248,   431,   432,   433,   434,   435,   436,   576,
     377,   457,   458,   259,   260,   346,   261,   347,   348,   349,
     350,   351,   539,   551,   480,   262,   476,   459,   460,   449,
     263,   264,   265,   266,   352,   267,   268,   190,   465,   269,
     270,   191,   192,   353,    68,    69,    70,   271,   272,   208,
     273,   274,   275,   276,   277,   278,   279,   464,   280,   317,
      71,   281,   282,    90,   283,   520,   521,   284,   285,    94,
      95,    96,    97,    98,   465,   286,   287,   288,   289,   508,
     466,   467,   290,   291,   292,   293,   294,   295,   296,   468,
     469,   470,   315,   471,   472,   473,   474,   475,   306,   316,
     188,   189,   121,   122,   123,   124,   125,   126,   318,   319,
     545,   546,   549,   320,   331,   332,   466,   467,   333,   335,
     336,   337,   373,   356,   357,   468,   469,   470,   358,   471,
     472,   473,   474,   475,   359,   362,    26,   363,   364,   365,
     366,   415,   562,   367,   370,   476,     9,    10,   371,   372,
     374,   389,   395,   375,   397,   564,   565,   566,   567,   568,
     569,   399,   571,   439,   420,   573,   441,   563,   401,   442,
     409,   443,   190,    33,    34,   144,   191,   192,   444,   580,
     445,   476,   391,   146,   147,   148,   149,    43,    44,    45,
      46,    47,    48,    49,    50,    51,   446,   447,   549,   581,
     156,   482,   550,   299,   552,   553,   554,   555,   556,   557,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   483,   105,   106,   484,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   485,   117,   118,   486,   119,
     120,   487,   488,   121,   122,   123,   124,   125,   126,   127,
     128,   489,   129,   130,   131,   132,   490,   491,   133,   134,
     492,   493,   494,   135,   495,   496,   497,   506,   245,   510,
     517,   249,   530,   531,   136,   532,   533,   524,   534,   535,
     137,   536,   138,   537,   538,   139,   140,   141,   142,   541,
     377,   570,    89,   574,   481,   577,   143,   582,   479,   579,
      82,   452,    60,   560,   450,   519,   542,   544,   543,    62,
     392,   515,   523,   522,    33,    34,   144,   462,   253,   529,
     461,     0,     0,   145,   146,   147,   148,   149,   150,    44,
     151,   152,   153,   154,   155,    50,    51,     0,     0,     0,
       0,   156,     0,     0,     0,     0,   463,   157,     0,     0,
       0,     0,     0,     0,     0,     0,   158,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,     0,   105,
     106,     0,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,     0,   117,   118,     0,   119,   120,     0,     0,
     121,   122,   123,   124,   125,   126,   127,   128,     0,   129,
     130,   131,   132,     0,     0,   133,   134,     0,     0,     0,
     135,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   136,     0,     0,     0,     0,     0,   137,     0,   138,
       0,     0,   139,   140,   141,   142,     0,     0,     0,     0,
       0,     0,     0,   143,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    33,    34,   144,     0,     0,     0,     0,     0,     0,
     145,   146,   147,   148,   149,   150,    44,   151,   152,   153,
     154,   155,    50,    51,     0,     0,     0,     0,   156,     0,
       0,     0,     0,     0,   157,     0,     0,     0,     0,     0,
       0,     0,     0,   158,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   177,   104,     0,   105,   106,     0,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,     0,
     117,   118,     0,   119,   120,     0,     0,   121,   122,   123,
     124,   125,   126,   127,   128,     0,   129,   130,     0,   132,
       0,     0,   133,   134,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   137,     0,     0,     0,     0,   139,
     140,   141,   142,     0,     0,     0,     0,     0,     0,     0,
     143,     0,     0,    15,    16,     0,     0,     0,     0,     0,
      17,     0,     0,     0,     0,     0,     0,     0,    33,    34,
     144,    18,     0,    19,     0,     0,     0,   145,   146,   147,
     148,   149,   150,    44,   151,   152,   153,   154,   155,    50,
      51,    20,     0,     0,     0,   156,     0,     0,     0,     0,
       0,   157,     0,     0,     0,     0,     0,     0,     0,     0,
     158,     0,     0,     0,     0,     0,    21,    22,    23,     0,
       0,     0,     0,     0,     0,    24,    25,     0,     0,     0,
       0,     0,     0,    26,     0,    27,    28,     0,     0,     0,
       0,     0,     0,     9,    10,   212,     0,     0,    29,    30,
       0,    31,   213,   214,   215,    32,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,    34,    35,    36,
      37,    38,    39,    40,    41,     0,     0,     0,    42,     0,
      43,    44,    45,    46,    47,    48,    49,    50,    51,     0,
       0,     0,     0,     0,   216,   217,   218,     0,   219,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   220,   221,
     222,   223,   224,   225,   226,     0,     0,     0,     0,     0,
       0,   227
  };

  const short int
  BisonParser::yycheck_[] =
  {
      76,    77,    78,    79,    14,    81,   173,     4,    72,   355,
      73,     3,   160,   161,    75,     5,   478,   377,    10,     0,
       7,     7,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    95,    24,    25,   229,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    95,    38,    39,
      26,    41,    42,   244,     7,    45,    46,    47,    48,    49,
      50,    51,    52,   162,    54,    55,    56,    57,    41,    42,
      60,    61,    95,   162,   173,    65,   162,   539,   162,   141,
     190,   191,     3,   162,     6,   174,    76,   173,     5,    65,
     174,   139,    82,    81,    84,   174,    83,    87,    88,    89,
      90,    93,    94,    91,     3,     6,    93,     6,    98,     6,
     171,    93,    94,   306,     5,     3,   192,   140,    43,    44,
       5,     3,   289,     6,    41,    42,   116,   117,   118,   102,
     103,   104,    17,    18,    19,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   297,
       6,   299,   498,   143,   514,     6,    41,    42,    91,   149,
       3,    96,    97,     3,   161,     6,    51,   360,   158,    54,
       3,    62,    75,   106,   107,   108,   152,   368,    69,    70,
      71,   165,   166,   167,   260,   102,   103,   104,     6,   265,
     266,   267,   268,   269,   270,   271,   259,   273,   274,   275,
     276,    11,    17,    18,    19,   281,   282,   283,   284,     3,
     286,   287,   288,    98,   324,   325,   326,   168,   169,    21,
     111,   112,   113,    11,   115,    11,    41,    42,   161,    37,
      38,    39,    40,   168,   170,   171,    51,   172,   173,    54,
     175,    11,   318,     4,     5,   130,    11,   132,   133,   134,
     135,   136,   315,   317,   145,   146,   147,   148,   149,   150,
     151,    63,    64,   143,   149,    96,    97,   158,   152,    23,
      72,    73,    74,   158,    76,    77,    78,    79,    80,   144,
     356,   357,   358,    98,    58,    59,   362,   363,     6,   365,
     277,   278,     6,   280,     6,   371,   372,     6,   285,    53,
     106,   107,   108,   290,   291,   292,   293,   294,   295,     4,
       5,   327,   328,     6,     6,   130,     6,   132,   133,   134,
     135,   136,     5,   490,   334,     6,   128,   329,   330,   316,
       6,     6,     6,     6,   149,     6,     6,   168,    21,     6,
       6,   172,   173,   158,    98,    99,   100,     6,     6,   336,
       6,     6,     6,     6,     6,     6,     6,     4,     6,     5,
     114,     6,     6,    10,     6,   441,   442,     6,     6,    12,
      13,    14,    15,    16,    21,     6,     6,     6,     6,   366,
      63,    64,     6,     6,     6,     6,     6,     6,     4,    72,
      73,    74,     6,    76,    77,    78,    79,    80,    11,    11,
      96,    97,    45,    46,    47,    48,    49,    50,     6,     4,
     486,   487,   488,     5,   164,   163,    63,    64,   162,     4,
       9,     5,     3,     6,     6,    72,    73,    74,     6,    76,
      77,    78,    79,    80,     4,     6,    83,     6,     6,     6,
       6,   517,   518,     4,     6,   128,    93,    94,     6,     6,
       4,   142,   153,     9,   154,   531,   532,   533,   534,   535,
     536,   155,   538,     5,    91,   541,     6,   530,   156,     6,
     159,     6,   168,   116,   117,   118,   172,   173,     9,   175,
       6,   128,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,     6,     5,   574,   575,
     143,    76,   489,     5,   491,   492,   493,   494,   495,   496,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,     6,    24,    25,     6,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     6,    38,    39,     6,    41,
      42,     6,     6,    45,    46,    47,    48,    49,    50,    51,
      52,     6,    54,    55,    56,    57,     6,     6,    60,    61,
       6,     6,     6,    65,     6,     6,     4,    92,    91,   160,
       9,   161,     6,     6,    76,     6,     6,   157,     6,     6,
      82,     6,    84,     6,     6,    87,    88,    89,    90,     6,
       5,    91,    54,     9,   336,   174,    98,   174,   334,   574,
      26,   320,    14,   517,   317,   439,   483,   485,   484,    14,
     259,   375,   444,   443,   116,   117,   118,   332,    88,   447,
     331,    -1,    -1,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,    -1,    -1,    -1,
      -1,   143,    -1,    -1,    -1,    -1,   333,   149,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   158,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    -1,    24,
      25,    -1,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    -1,    38,    39,    -1,    41,    42,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    -1,    54,
      55,    56,    57,    -1,    -1,    60,    61,    -1,    -1,    -1,
      65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    82,    -1,    84,
      -1,    -1,    87,    88,    89,    90,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   116,   117,   118,    -1,    -1,    -1,    -1,    -1,    -1,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,    -1,    -1,    -1,    -1,   143,    -1,
      -1,    -1,    -1,    -1,   149,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   158,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    -1,    24,    25,    -1,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    -1,
      38,    39,    -1,    41,    42,    -1,    -1,    45,    46,    47,
      48,    49,    50,    51,    52,    -1,    54,    55,    -1,    57,
      -1,    -1,    60,    61,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    -1,    -1,    87,
      88,    89,    90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      98,    -1,    -1,     3,     4,    -1,    -1,    -1,    -1,    -1,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,   117,
     118,    21,    -1,    23,    -1,    -1,    -1,   125,   126,   127,
     128,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,    41,    -1,    -1,    -1,   143,    -1,    -1,    -1,    -1,
      -1,   149,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     158,    -1,    -1,    -1,    -1,    -1,    66,    67,    68,    -1,
      -1,    -1,    -1,    -1,    -1,    75,    76,    -1,    -1,    -1,
      -1,    -1,    -1,    83,    -1,    85,    86,    -1,    -1,    -1,
      -1,    -1,    -1,    93,    94,    62,    -1,    -1,    98,    99,
      -1,   101,    69,    70,    71,   105,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   116,   117,   118,   119,
     120,   121,   122,   123,   124,    -1,    -1,    -1,   128,    -1,
     130,   131,   132,   133,   134,   135,   136,   137,   138,    -1,
      -1,    -1,    -1,    -1,   111,   112,   113,    -1,   115,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   145,   146,
     147,   148,   149,   150,   151,    -1,    -1,    -1,    -1,    -1,
      -1,   158
  };

  const unsigned short int
  BisonParser::yystos_[] =
  {
       0,   177,   178,     0,    81,    91,   179,     7,   180,    93,
      94,   259,     7,     3,   181,     3,     4,    10,    21,    23,
      41,    66,    67,    68,    75,    76,    83,    85,    86,    98,
      99,   101,   105,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   128,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   182,   190,   191,   192,   197,   198,   199,   207,
     209,   224,   245,   246,   259,   210,    23,    53,    98,    99,
     100,   114,   208,     6,     3,     6,     6,     6,     6,     6,
       3,     6,   198,     3,     3,     6,     3,     3,     3,   192,
      10,   207,   209,     6,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    24,    25,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    38,    39,    41,
      42,    45,    46,    47,    48,    49,    50,    51,    52,    54,
      55,    56,    57,    60,    61,    65,    76,    82,    84,    87,
      88,    89,    90,    98,   118,   125,   126,   127,   128,   129,
     130,   132,   133,   134,   135,   136,   143,   149,   158,   211,
     212,   214,   215,   220,   221,   223,   229,   243,   244,   245,
     247,    11,    11,    11,   200,    11,    11,    21,   213,   214,
     229,    26,    65,   152,   201,   202,   239,   239,    96,    97,
     168,   172,   173,   260,   261,   262,   263,   264,   265,   266,
     267,   267,   267,   267,   193,   267,   186,   187,   259,   183,
     144,   230,    62,    69,    70,    71,   111,   112,   113,   115,
     145,   146,   147,   148,   149,   150,   151,   158,   203,   206,
     231,   232,   233,   234,   235,   236,   237,   238,    41,    42,
     102,   103,   104,   204,   205,    91,   106,   107,   108,   161,
     250,   251,   253,   254,   259,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     4,     5,   211,     5,
     211,   239,   217,   218,   267,   230,    11,    37,    38,    39,
      40,   248,    43,    44,   256,     6,    11,     5,     6,     4,
       5,   261,   261,   267,   165,   166,   167,   168,   169,   170,
     171,   164,   163,   162,   194,     4,     9,     5,    17,    18,
      19,    41,    42,    51,    54,    98,   130,   132,   133,   134,
     135,   136,   149,   158,   184,   185,     6,     6,     6,     4,
       5,   203,     6,     6,     6,     6,     6,     4,     5,   204,
       6,     6,     6,     3,     4,     9,     4,     5,     4,   254,
      95,   139,   225,    95,   140,   226,    95,   141,   227,   142,
     228,   125,   220,   229,   267,   153,   241,   154,   240,   155,
     242,   156,   267,   267,   267,   267,   267,   267,   267,   159,
     249,   267,   267,   216,   222,   267,   219,   267,   259,   259,
      91,   259,   267,   267,   267,   267,   259,   267,   267,   267,
     230,   259,   259,   259,   259,   259,   259,   211,   211,     5,
     203,     6,     6,     6,     9,     6,     6,     5,   229,   259,
     213,   267,   201,   174,   261,   261,   261,   262,   262,   263,
     263,   264,   265,   266,     4,    21,    63,    64,    72,    73,
      74,    76,    77,    78,    79,    80,   128,   195,   196,   197,
     207,   186,    76,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     4,     5,   184,
     267,   267,   267,   203,   267,   267,    92,   267,   259,   204,
     160,   252,   267,   267,   255,   251,   253,     9,   173,   217,
     267,   267,   249,   248,   157,   257,    58,    59,   258,   256,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     5,
     195,     6,   225,   226,   227,   267,   267,   188,   189,   267,
     259,   230,   259,   259,   259,   259,   259,   259,   184,   253,
     216,   175,   267,   229,   267,   267,   267,   267,   267,   267,
      91,   267,   195,   267,     9,   173,     4,   174,   174,   188,
     175,   267,   174,   174
  };

  const unsigned short int
  BisonParser::yyr1_[] =
  {
       0,   176,   177,   178,   178,   179,   180,   179,   181,   181,
     181,   181,   181,   181,   181,   181,   183,   182,   184,   184,
     184,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   186,   186,
     187,   187,   188,   188,   189,   189,   189,   190,   190,   190,
     190,   190,   190,   190,   190,   191,   191,   193,   192,   194,
     194,   194,   194,   195,   195,   195,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   197,   197,
     198,   198,   198,   199,   200,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   201,   201,
     202,   202,   202,   203,   203,   203,   204,   204,   204,   205,
     205,   205,   205,   205,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     208,   207,   207,   210,   209,   211,   211,   211,   211,   211,
     211,   212,   212,   212,   212,   212,   212,   213,   213,   214,
     214,   214,   214,   214,   215,   215,   216,   216,   217,   217,
     218,   219,   220,   220,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   221,   221,
     222,   222,   222,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   225,   225,
     226,   226,   227,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     243,   243,   243,   243,   243,   243,   243,   243,   243,   243,
     243,   244,   244,   244,   245,   245,   245,   245,   245,   245,
     245,   245,   245,   246,   246,   246,   246,   247,   247,   247,
     248,   248,   248,   248,   248,   249,   250,   250,   251,   251,
     251,   252,   253,   253,   255,   254,   254,   256,   256,   256,
     257,   258,   258,   259,   259,   260,   260,   260,   261,   261,
     261,   262,   262,   262,   262,   263,   263,   263,   264,   264,
     264,   265,   265,   266,   266,   267,   267
  };

  const unsigned char
  BisonParser::yyr2_[] =
  {
       0,     2,     1,     0,     2,     3,     0,     6,     0,     2,
       2,     2,     2,     2,     2,     2,     0,     5,     1,     3,
       2,     0,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     3,     1,     1,     3,
       5,     0,     1,     3,     1,     4,     4,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     2,     0,     5,     0,
       2,     2,     2,     1,     3,     2,     3,     3,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     2,     1,
       0,     2,     2,     4,     0,     5,     4,     4,     4,     3,
       4,     4,     4,     4,     4,     4,     4,     4,     1,     3,
       1,     1,     3,     1,     3,     2,     1,     3,     2,     3,
       3,     3,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     3,     1,     1,     1,     3,     1,
       0,     5,     1,     0,     4,     1,     1,     3,     3,     2,
       2,     3,     3,     3,     3,     3,     3,     1,     3,     1,
       1,     1,     1,     1,     3,     3,     1,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     4,     4,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     1,     1,     3,     3,     3,
       3,     3,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     1,     3,     1,     1,     3,     3,     3,
       3,     1,     1,     3,     0,     5,     2,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     2,
       2,     1,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     1,     3,     1,     3,     1,     3
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const BisonParser::yytname_[] =
  {
  "$end", "error", "$undefined", "OBRACE", "EBRACE", "COMMA", "EQUAL",
  "COLON", "QUOTE", "SEMICOLON", "OBRACKET", "EBRACKET", "BOOTLOADER",
  "XIP_MODE", "EARLY_HANDOFF", "HIVEC", "LOCKSTEP", "AUTHENTICATION",
  "ENCRYPTION", "CHECKSUM", "PARTITION_OWNER", "PARTITION_TYPE",
  "PARTITION_NUM", "BOOT_DEVICE", "DEST_DEVICE", "DEST_CPU", "ADDRESS",
  "EXCEPTION_LEVEL", "TRUSTZONE", "ALIGNMENT", "OFFSET", "RESERVE_LEGACY",
  "RESERVE", "LOAD", "STARTUP", "BIGENDIAN", "A32_MODE", "PPK_SELECT",
  "SPK_ID", "SPK_SELECT", "HEADER_AUTH", "REVOKE_ID", "SPK_REVOKE_ID",
  "SPLIT_MODE", "SPLIT_FMT", "BOOT", "USER", "STATIC", "NOAUTOSTART",
  "MULTIBOOT", "PROTECTED", "BLOCKS", "AUTHBLOCKS", "BOOTVECTORS",
  "PRESIGN", "AC", "BIF_SECTION", "UDF_DATA", "MCS", "BIN", "SLR_NUM",
  "CLUSTER_NUM", "DICE", "PCR_NUMBER", "PCR_MEASUREMENT_INDEX",
  "IMAGE_STORE", "PARENT_ID", "ID_CODE", "EXT_ID_CODE",
  "BYPASS_IDCODE_CHECK", "A_HWROT", "S_HWROT", "UNIQUE_ID",
  "PARENT_UNIQUE_ID", "FUNCTION_ID", "IMAGE", "ID", "NAME",
  "DELAY_HANDOFF", "DELAY_LOAD", "COPY", "INCLUDE", "DELAY_AUTH",
  "PARTITION", "PFILE", "OPTIONAL_DATA", "METAHEADER", "TCM_BOOT",
  "TCM_A_REGION", "TCM_B_REGION", "TCM_C_REGION", "WORD", "HEXWORD",
  "FILENAME", "QFILENAME", "NONE", "DECVALUE", "HEXVALUE",
  "KEYSRC_ENCRYPTION", "FSBL_CONFIG", "AUTH_PARAMS", "AUTHJTAG_CONFIG",
  "DEVICE_DNA", "JTAG_TIMEOUT", "AUTHJTAG_SIGN", "LMS_KEY_PARAMS",
  "LMS_HASH", "LMS_H", "LMS_W", "SHA256", "SHAKE256", "PUF4KMODE",
  "PUFROSWAP", "SHUTTER", "SPLIT", "SMAP_WIDTH", "PUF_HELPER_FILE",
  "BH_KEY_FILE", "BH_KEY_IV", "BH_KEK_IV", "BBRAM_KEK_IV", "EFUSE_KEK_IV",
  "EFUSE_USER_KEK0_IV", "EFUSE_USER_KEK1_IV", "USER_KEYS", "PMCDATA",
  "BOOTIMAGE", "UDF_BH", "INIT", "PMUFW_IMAGE", "AES_KEY_FILE",
  "FAMILY_KEY", "PPK_FILE", "PSK_FILE", "SPK_FILE", "SSK_FILE",
  "SPK_SIGNATURE_FILE", "BH_SIGNATURE_FILE", "HEADER_SIGNATURE_FILE",
  "AUTHVALUE", "ENCRVALUE", "CHECKSUMVALUE", "POWNERVALUE", "PTYPEVALUE",
  "KEY_SRC", "CORE", "BH_RSA", "AUTH_HASH", "INT_HASH", "PUFHD_LOC",
  "OPT_KEY", "AUTH_ONLY", "BOOT_DEVICE_TYPE", "DEST_DEVICE_TYPE",
  "DEST_CPU_TYPE", "EXCEPTION_LEVEL_TYPE", "TRUSTZONE_TYPE", "SPLITMODE",
  "DPA_CM", "SPKSELECT", "HASH_FUNCTION", "AUTH_KEY_LEVEL", "OR", "XOR",
  "AND", "MULT", "DIVIDE", "MODULO", "PLUS", "MINUS", "LSHIFT", "RSHIFT",
  "NEGATION", "LPAREN", "RPAREN", "ASTERISK", "$accept", "bif",
  "group_list", "bifoptions", "$@1", "file_list", "metahdr_spec", "$@2",
  "metahdr_attr_list", "metahdr_attr", "optional_data",
  "optional_data_attr", "metahdr_blk", "metahdr_blk_attr", "new_pdi_spec",
  "image_spec", "image_list", "$@3", "image_content",
  "image_attributes_list", "image_attributes", "partition_spec",
  "partition_content", "other_spec", "$@4", "sec_boot_attr_list",
  "sec_boot_attr", "fsbl_attr_list", "authjtag_attr_list", "authjtag_attr",
  "fsbl_attr", "file_spec", "$@5", "new_file_spec", "$@6",
  "new_attribute_list", "new_attribute", "attribute_list", "attribute",
  "blocksattr_list", "blocksattr", "bootvectors_list", "bootvector",
  "authblockattr", "boolattr", "trustzone_type", "blockattr", "optattr",
  "other_file_attr", "authvalue", "encrvalue", "checkvalue", "pownervalue",
  "ptypevalue", "key_src", "core", "bh_rsa", "dpa_cm", "auth_hash",
  "int_hash", "pufhd_loc", "opt_key", "auth_only", "boot_device_type",
  "dest_cpu_type", "dest_device_type", "exception_level_type", "numattr",
  "fileattr", "key_file", "rsa_key_file", "other_files", "auth_parameters",
  "spkselect", "lms_params_list", "lms_params_attr", "hashfunc",
  "lms_params_string_list", "lms_params_string_type_list", "$@7",
  "split_options", "splitmode", "splitfmt", "filename", "number",
  "unary_expression", "multiplicative_expression", "additive_expression",
  "shift_expression", "and_expression", "xor_expression", "expression", YY_NULLPTR
  };


  const unsigned short int
  BisonParser::yyrline_[] =
  {
       0,   212,   212,   214,   215,   217,   218,   218,   224,   225,
     226,   227,   228,   229,   230,   231,   234,   234,   239,   240,
     241,   244,   245,   246,   247,   248,   249,   250,   251,   252,
     253,   254,   255,   256,   263,   264,   265,   266,   270,   271,
     274,   275,   277,   278,   281,   283,   285,   289,   290,   291,
     293,   294,   295,   296,   297,   300,   301,   304,   304,   313,
     314,   315,   316,   319,   320,   321,   324,   325,   326,   327,
     328,   329,   331,   332,   337,   342,   347,   364,   375,   376,
     379,   380,   381,   384,   387,   387,   390,   391,   392,   397,
     398,   399,   400,   401,   402,   403,   404,   405,   408,   409,
     412,   413,   414,   417,   418,   419,   422,   423,   424,   427,
     428,   429,   430,   431,   434,   437,   439,   441,   443,   445,
     447,   449,   451,   453,   455,   459,   460,   461,   462,   463,
     466,   466,   473,   479,   479,   485,   486,   487,   488,   489,
     490,   493,   496,   497,   499,   500,   501,   507,   508,   511,
     512,   513,   514,   515,   518,   519,   522,   523,   526,   527,
     530,   535,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   549,   550,   551,   552,   553,   554,   557,   558,
     561,   562,   563,   565,   566,   567,   568,   569,   570,   571,
     572,   573,   574,   575,   576,   577,   578,   579,   580,   581,
     582,   583,   584,   585,   586,   587,   588,   589,   590,   593,
     594,   595,   596,   597,   598,   599,   600,   601,   604,   605,
     608,   609,   612,   613,   616,   619,   622,   625,   628,   631,
     634,   637,   640,   643,   646,   649,   652,   655,   658,   661,
     662,   663,   664,   665,   666,   667,   668,   669,   670,   671,
     672,   675,   676,   677,   680,   681,   682,   683,   684,   685,
     686,   687,   688,   691,   692,   693,   694,   697,   698,   699,
     702,   705,   708,   711,   714,   717,   721,   722,   724,   725,
     726,   728,   732,   737,   739,   739,   741,   743,   746,   747,
     750,   753,   756,   762,   763,   766,   767,   768,   774,   775,
     776,   780,   781,   782,   783,   786,   787,   788,   791,   792,
     793,   796,   797,   800,   801,   805,   806
  };

  // Print the state stack on the debug stream.
  void
  BisonParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  BisonParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
  BisonParser::token_number_type
  BisonParser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
     0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175
    };
    const unsigned int user_token_number_max_ = 430;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }

#line 24 "parser/bif.y" // lalr1.cc:1167
} // BIF
#line 2669 "bisonflex/bif.tab.cpp" // lalr1.cc:1167
#line 811 "parser/bif.y" // lalr1.cc:1168

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
