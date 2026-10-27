/* A Bison parser, made by GNU Bison 2.7.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++
   
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


/* First part of user declarations.  */
/* Line 279 of lalr1.cc  */
#line 55 "parser/bif.y"

#include <stdio.h>
#include "bootimage.h"
#include "authentication.h"
#include "encryption.h"
#include "checksum.h"
#include "bifscanner.h"
#include "parsing.h"
#include "fileutils.h"
#include "imageheadertable-versal.h"
#include "imageheadertable-spartanup.h"
#include "imageheadertable-versal_2ve_2vm.h"
#include "imageheadertable-versal_2vp.h"

// Parser uses raw pointers for compatibility with Bison semantics
// These are transferred to options lists which take ownership
BifOptions* currentBifOptions;
PartitionBifOptions* currentPartitionBifOptions ;
ImageBifOptions* currentImageBifOptions;
LmsKeyTypeParam lmsParams;

/* Block being parsed, so that a bison "syntax error" can be reported as an
   attribute that is not allowed in that block. */
static bool inImageScope = false;
static bool inPartitionScope = false;

// BIF Integrity verification - now stored in Options class

/* Line 279 of lalr1.cc  */
#line 67 "bisonflex/bif.tab.cpp"


#include "bif.tab.hpp"

/* User implementation prologue.  */

/* Line 285 of lalr1.cc  */
#line 75 "bisonflex/bif.tab.cpp"
/* Unqualified %code blocks.  */
/* Line 286 of lalr1.cc  */
#line 49 "parser/bif.y"

    static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner);
    #include "options.h"


/* Line 286 of lalr1.cc  */
#line 85 "bisonflex/bif.tab.cpp"


# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
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


/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* Enable debugging if requested.  */
#if YYDEBUG

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Type, Value, Location) YYUSE(Type)
# define YY_REDUCE_PRINT(Rule)        static_cast<void>(0)
# define YY_STACK_PRINT()             static_cast<void>(0)

#endif /* !YYDEBUG */

#define yyerrok		(yyerrstatus_ = 0)
#define yyclearin	(yychar = yyempty_)

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

/* Line 353 of lalr1.cc  */
#line 24 "parser/bif.y"
namespace BIF {
/* Line 353 of lalr1.cc  */
#line 181 "bisonflex/bif.tab.cpp"

  /// Build a parser object.
  BisonParser::BisonParser (BIF::FlexScanner& scanner_yyarg, Options& options_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      scanner (scanner_yyarg),
      options (options_yyarg)
  {
  }

  BisonParser::~BisonParser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  BisonParser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    std::ostream& yyo = debug_stream ();
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  BisonParser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif

  void
  BisonParser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
        case 91: /* WORD */
/* Line 455 of lalr1.cc  */
#line 120 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 251 "bisonflex/bif.tab.cpp"
        break;
      case 92: /* HEXWORD */
/* Line 455 of lalr1.cc  */
#line 120 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 258 "bisonflex/bif.tab.cpp"
        break;
      case 93: /* FILENAME */
/* Line 455 of lalr1.cc  */
#line 120 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 265 "bisonflex/bif.tab.cpp"
        break;
      case 94: /* QFILENAME */
/* Line 455 of lalr1.cc  */
#line 120 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 272 "bisonflex/bif.tab.cpp"
        break;
      case 149: /* HYBRID_AUTHVALUE */
/* Line 455 of lalr1.cc  */
#line 120 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 279 "bisonflex/bif.tab.cpp"
        break;
      case 269: /* filename */
/* Line 455 of lalr1.cc  */
#line 120 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 286 "bisonflex/bif.tab.cpp"
        break;

	default:
	  break;
      }
  }

  void
  BisonParser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
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
#endif

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
    /// Lookahead and lookahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    // State.
    int yyn;
    int yylen = 0;
    int yystate = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the lookahead.
    static semantic_type yyval_default;
    semantic_type yylval = yyval_default;
    /// Location of the lookahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location_type yyerror_range[3];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


/* User initialization code.  */
/* Line 545 of lalr1.cc  */
#line 31 "parser/bif.y"
{
       // Initialize the initial location.
       yylloc.begin.filename = yylloc.end.filename = &scanner.filename;
       inImageScope = false;
       inPartitionScope = false;
}
/* Line 545 of lalr1.cc  */
#line 389 "bisonflex/bif.tab.cpp"

    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;

    /* Accept?  */
    if (yystate == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without lookahead.  */
    yyn = yypact_[yystate];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    /* Read a lookahead token.  */
    if (yychar == yyempty_)
      {
        YYCDEBUG << "Reading a token: ";
        yychar = yylex (&yylval, &yylloc, scanner);
      }

    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yy_table_value_is_error_ (yyn))
	  goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Shift the lookahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted.  */
    yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    // Compute the default @$.
    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }

    // Perform the reduction.
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
          case 5:
/* Line 670 of lalr1.cc  */
#line 230 "parser/bif.y"
    { options.includeBifOptionsList.push_back((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 231 "parser/bif.y"
    { currentBifOptions = new BifOptions(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series(), (yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 7:
/* Line 670 of lalr1.cc  */
#line 233 "parser/bif.y"
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 247 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(), currentBifOptions->GetIdCode()); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 258 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 259 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 260 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 261 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 262 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 263 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 264 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 265 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 266 "parser/bif.y"
    { currentBifOptions->SetPPKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 267 "parser/bif.y"
    { currentBifOptions->SetPSKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 268 "parser/bif.y"
    { currentBifOptions->SetSPKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 269 "parser/bif.y"
    { currentBifOptions->SetSSKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 34:
/* Line 670 of lalr1.cc  */
#line 270 "parser/bif.y"
    { currentBifOptions->SetPKFileName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 271 "parser/bif.y"
    { currentBifOptions->SetSKFileName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 36:
/* Line 670 of lalr1.cc  */
#line 272 "parser/bif.y"
    { currentBifOptions->SetPKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 37:
/* Line 670 of lalr1.cc  */
#line 273 "parser/bif.y"
    { currentBifOptions->SetSKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 38:
/* Line 670 of lalr1.cc  */
#line 274 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 39:
/* Line 670 of lalr1.cc  */
#line 275 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 276 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.partitionRevokeId = (yysemantic_stack_[(3) - (3)].number);}
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 277 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkRevokeId = (yysemantic_stack_[(3) - (3)].number); 
                                                                                if(options.GetArchType() == Arch::VERSAL)
                                                                                {
                                                                                   LOG_ERROR("SPK_REVOKE_ID does not support for versal arch, instead please use revoke_id option.");
                                                                                }
                                                                                }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 284 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 285 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 287 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 295 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ihtOptionalDataInfo.push_back(std::pair<std::string, uint32_t>((yysemantic_stack_[(5) - (1)].string), (yysemantic_stack_[(5) - (5)].number))); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 302 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 304 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 306 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 55:
/* Line 670 of lalr1.cc  */
#line 310 "parser/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 56:
/* Line 670 of lalr1.cc  */
#line 311 "parser/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 312 "parser/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); 
                                                                                  options.SetDl9Series((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 314 "parser/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 59:
/* Line 670 of lalr1.cc  */
#line 315 "parser/bif.y"
    { currentBifOptions->SetPKFileName((yysemantic_stack_[(3) - (3)].string)); 
                                                                                  currentBifOptions->AddFiles(BIF::BisonParser::token::PK_FILE, (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 317 "parser/bif.y"
    { currentBifOptions->SetSKFileName((yysemantic_stack_[(3) - (3)].string)); 
                                                                                  currentBifOptions->AddFiles(BIF::BisonParser::token::SK_FILE, (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 319 "parser/bif.y"
    { currentBifOptions->SetPKFileName1((yysemantic_stack_[(3) - (3)].string)); 
                                                                                  currentBifOptions->AddFiles(BIF::BisonParser::token::PK_FILE1, (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 321 "parser/bif.y"
    { currentBifOptions->SetSKFileName1((yysemantic_stack_[(3) - (3)].string)); 
                                                                                  currentBifOptions->AddFiles(BIF::BisonParser::token::SK_FILE1, (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 323 "parser/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 324 "parser/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 325 "parser/bif.y"
    { currentBifOptions->SetPdiType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 326 "parser/bif.y"
    { currentBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number));}
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 333 "parser/bif.y"
    { currentImageBifOptions = new ImageBifOptions();
                                                                                  inImageScope = true; }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 335 "parser/bif.y"
    { inImageScope = false;
                                                                                  currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions);
                                                                                  if((options.archType == Arch::SPARTANUP) && (currentBifOptions->imageBifOptionList.size() > 1))
                                                                                  {
                                                                                      LOG_ERROR("'-arch spartanup' cannot accomodate more than one Sub-System.");
                                                                                  }
                                                                                }
    break;

  case 79:
/* Line 670 of lalr1.cc  */
#line 356 "parser/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 80:
/* Line 670 of lalr1.cc  */
#line 357 "parser/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 81:
/* Line 670 of lalr1.cc  */
#line 358 "parser/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 82:
/* Line 670 of lalr1.cc  */
#line 359 "parser/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 83:
/* Line 670 of lalr1.cc  */
#line 360 "parser/bif.y"
    { LOG_ERROR("BIF attribute error !!!\n\t This usage of 'init' is not supported. See 'bootgen -bif_help init' for usage details."); }
    break;

  case 84:
/* Line 670 of lalr1.cc  */
#line 361 "parser/bif.y"
    { LOG_ERROR("Copy to Memory feature with the attribute 'copy' is no more supported.\n\t   This can be duplicated with the option 'imagestore'. Please refer UG1283 for more details.");
                                                                                  currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 85:
/* Line 670 of lalr1.cc  */
#line 363 "parser/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 86:
/* Line 670 of lalr1.cc  */
#line 364 "parser/bif.y"
    { if ((yysemantic_stack_[(3) - (3)].number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("uid value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 87:
/* Line 670 of lalr1.cc  */
#line 369 "parser/bif.y"
    { if ((yysemantic_stack_[(3) - (3)].number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("parent_uid value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 88:
/* Line 670 of lalr1.cc  */
#line 374 "parser/bif.y"
    { if ((yysemantic_stack_[(3) - (3)].number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("function_id value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 89:
/* Line 670 of lalr1.cc  */
#line 379 "parser/bif.y"
    { if (options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP)
                                                                                  {
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr' is not supported for the specified architecture");
                                                                                  }
                                                                                  else if(options.GetArchType() == Arch::VERSAL && !options.IsVersalNetSeries())
                                                                                  {
                                                                                    currentImageBifOptions->SetPcrNumber((yysemantic_stack_[(3) - (3)].number), false);
                                                                                  }
                                                                                  else if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                  {
                                                                                    currentImageBifOptions->SetPcrNumber((yysemantic_stack_[(3) - (3)].number), true);
                                                                                  }
                                                                                  else if(options.GetArchType() == Arch::VERSALGEN2)
                                                                                  {
                                                                                    currentImageBifOptions->SetPcrNumber((yysemantic_stack_[(3) - (3)].number), options.IsVersalNetSeries());
                                                                                  }
                                                                                  else if(options.GetArchType() == Arch::VERSAL_2VP)
                                                                                  {
                                                                                    currentImageBifOptions->SetPcrNumber((yysemantic_stack_[(3) - (3)].number), false);
                                                                                  }
                                                                                }
    break;

  case 90:
/* Line 670 of lalr1.cc  */
#line 400 "parser/bif.y"
    { if (options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP)
                                                                                  {
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr measurement index' is not supported for the specified architecture");
                                                                                  }
                                                                                  else if(options.GetArchType() == Arch::VERSAL || options.GetArchType() == Arch::VERSALGEN2 || options.GetArchType() == Arch::VERSAL_2VP)
                                                                                  {
                                                                                    currentImageBifOptions->SetPcrMeasurementIndex((yysemantic_stack_[(3) - (3)].number));
                                                                                  }
                                                                                }
    break;

  case 96:
/* Line 670 of lalr1.cc  */
#line 420 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQ || options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported for the specified architecture.\n\t   Refer 'bootgen -bif_help' for more details");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 97:
/* Line 670 of lalr1.cc  */
#line 423 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 428 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                  if(options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); 
                                                                                  currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 433 "parser/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 448 "parser/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 449 "parser/bif.y"
    { currentBifOptions->SetBootDevice(BootDevice::IMAGESTORE); }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 450 "parser/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 463 "parser/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 464 "parser/bif.y"
    { currentBifOptions->SetAuthJtagSPKRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 465 "parser/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 466 "parser/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 467 "parser/bif.y"
    { currentBifOptions->SetAuthJtagSignatureFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 470 "parser/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 473 "parser/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 129:
/* Line 670 of lalr1.cc  */
#line 475 "parser/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 130:
/* Line 670 of lalr1.cc  */
#line 477 "parser/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 479 "parser/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 481 "parser/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 133:
/* Line 670 of lalr1.cc  */
#line 483 "parser/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 134:
/* Line 670 of lalr1.cc  */
#line 485 "parser/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 135:
/* Line 670 of lalr1.cc  */
#line 487 "parser/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 136:
/* Line 670 of lalr1.cc  */
#line 489 "parser/bif.y"
    {  currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));}
    break;

  case 137:
/* Line 670 of lalr1.cc  */
#line 491 "parser/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->SetSmapWidth((yysemantic_stack_[(3) - (3)].number));
                                                                                }
    break;

  case 138:
/* Line 670 of lalr1.cc  */
#line 495 "parser/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 496 "parser/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 497 "parser/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 498 "parser/bif.y"
    { currentBifOptions->SetPufRingOscilltorSwapConfigValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 499 "parser/bif.y"
    { currentBifOptions->SetDiceEnable(); }
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 502 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(),options.IsVersalNetSeries(),  currentBifOptions->GetIdCode());
                                                                                  inPartitionScope = true; }
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 506 "parser/bif.y"
    { inPartitionScope = false;
                                                                                  currentPartitionBifOptions->filename = FileUtils::ResolveInputPath((yysemantic_stack_[(5) - (5)].string), scanner.filename);
                                                                                  currentPartitionBifOptions->filelist.push_back(currentPartitionBifOptions->filename);
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 511 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode());
                                                                                  currentPartitionBifOptions->filename = FileUtils::ResolveInputPath((yysemantic_stack_[(1) - (1)].string), scanner.filename);
                                                                                  currentPartitionBifOptions->filelist.push_back(currentPartitionBifOptions->filename);
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 517 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode());
                                                                                  inPartitionScope = true; }
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 520 "parser/bif.y"
    { inPartitionScope = false; }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 532 "parser/bif.y"
    { currentPartitionBifOptions->filename = FileUtils::ResolveInputPath((yysemantic_stack_[(3) - (3)].string), scanner.filename);
                                                                                  currentPartitionBifOptions->filelist.push_back(currentPartitionBifOptions->filename);
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 535 "parser/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 536 "parser/bif.y"
    { currentPartitionBifOptions->imageStoreId = (yysemantic_stack_[(3) - (3)].number);
                                                                                  currentPartitionBifOptions->SetPartitionType(PartitionType::IMAGE_STORE_PDI); }
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 539 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 540 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 541 "parser/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  /* front() on an empty list is undefined. -efuseppkbits
                                                                                     often omits -o, and libstdc++ then builds a string
                                                                                     from a null pointer. Fall back to the BIF name. */
                                                                                  std::string sectionOutFile = "";
                                                                                  if (!options.GetOutputFileNames().empty())
                                                                                  {
                                                                                      sectionOutFile = options.GetOutputFileNames().front();
                                                                                  }
                                                                                  else if (!options.GetBifFilename().empty())
                                                                                  {
                                                                                      sectionOutFile = StringUtils::RemoveExtension(options.GetBifFilename()) + ".bin";
                                                                                  }
                                                                                  else
                                                                                  {
                                                                                      LOG_ERROR("Output file name is required to resolve BIF 'section'");
                                                                                  }
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(sectionOutFile, (yysemantic_stack_[(3) - (3)].string), currentImageBifOptions->GetImageType());
                                                                                  currentPartitionBifOptions->filelist.push_back(currentPartitionBifOptions->filename);
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 586 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] is supported only for ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 591 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 593 "parser/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 594 "parser/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 595 "parser/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 596 "parser/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 597 "parser/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 598 "parser/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 599 "parser/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 600 "parser/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 601 "parser/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 602 "parser/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 605 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 606 "parser/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 607 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 189:
/* Line 670 of lalr1.cc  */
#line 608 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 609 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 191:
/* Line 670 of lalr1.cc  */
#line 610 "parser/bif.y"
    { currentPartitionBifOptions->SetLockStepFlag();}
    break;

  case 192:
/* Line 670 of lalr1.cc  */
#line 613 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 193:
/* Line 670 of lalr1.cc  */
#line 614 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 194:
/* Line 670 of lalr1.cc  */
#line 617 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 195:
/* Line 670 of lalr1.cc  */
#line 618 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 196:
/* Line 670 of lalr1.cc  */
#line 619 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 197:
/* Line 670 of lalr1.cc  */
#line 621 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 198:
/* Line 670 of lalr1.cc  */
#line 622 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 623 "parser/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 200:
/* Line 670 of lalr1.cc  */
#line 624 "parser/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 201:
/* Line 670 of lalr1.cc  */
#line 625 "parser/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 202:
/* Line 670 of lalr1.cc  */
#line 626 "parser/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 203:
/* Line 670 of lalr1.cc  */
#line 627 "parser/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 204:
/* Line 670 of lalr1.cc  */
#line 628 "parser/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 205:
/* Line 670 of lalr1.cc  */
#line 629 "parser/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 206:
/* Line 670 of lalr1.cc  */
#line 630 "parser/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 207:
/* Line 670 of lalr1.cc  */
#line 631 "parser/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 208:
/* Line 670 of lalr1.cc  */
#line 632 "parser/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 209:
/* Line 670 of lalr1.cc  */
#line 633 "parser/bif.y"
    { currentPartitionBifOptions->ppkFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 210:
/* Line 670 of lalr1.cc  */
#line 634 "parser/bif.y"
    { currentPartitionBifOptions->pskFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 211:
/* Line 670 of lalr1.cc  */
#line 635 "parser/bif.y"
    { currentPartitionBifOptions->spkFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 212:
/* Line 670 of lalr1.cc  */
#line 636 "parser/bif.y"
    { currentPartitionBifOptions->sskFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 213:
/* Line 670 of lalr1.cc  */
#line 637 "parser/bif.y"
    { currentPartitionBifOptions->pkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 214:
/* Line 670 of lalr1.cc  */
#line 638 "parser/bif.y"
    { currentPartitionBifOptions->skFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 215:
/* Line 670 of lalr1.cc  */
#line 639 "parser/bif.y"
    { currentPartitionBifOptions->pkFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 216:
/* Line 670 of lalr1.cc  */
#line 640 "parser/bif.y"
    { currentPartitionBifOptions->skFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 217:
/* Line 670 of lalr1.cc  */
#line 641 "parser/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 218:
/* Line 670 of lalr1.cc  */
#line 642 "parser/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 219:
/* Line 670 of lalr1.cc  */
#line 643 "parser/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 221:
/* Line 670 of lalr1.cc  */
#line 645 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 222:
/* Line 670 of lalr1.cc  */
#line 646 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 223:
/* Line 670 of lalr1.cc  */
#line 647 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 224:
/* Line 670 of lalr1.cc  */
#line 648 "parser/bif.y"
    { currentPartitionBifOptions->SetSPKRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 225:
/* Line 670 of lalr1.cc  */
#line 649 "parser/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 226:
/* Line 670 of lalr1.cc  */
#line 650 "parser/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 227:
/* Line 670 of lalr1.cc  */
#line 651 "parser/bif.y"
    { currentPartitionBifOptions->SetClusterNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 228:
/* Line 670 of lalr1.cc  */
#line 652 "parser/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 229:
/* Line 670 of lalr1.cc  */
#line 653 "parser/bif.y"
    { currentPartitionBifOptions->SetDelayAuth(true); }
    break;

  case 230:
/* Line 670 of lalr1.cc  */
#line 654 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBootFlag(); }
    break;

  case 240:
/* Line 670 of lalr1.cc  */
#line 668 "parser/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 242:
/* Line 670 of lalr1.cc  */
#line 670 "parser/bif.y"
    { currentPartitionBifOptions->SetHybridAuthType((yysemantic_stack_[(1) - (1)].string)); (yyval.authvalue_t) = ::Authentication::RSA; }
    break;

  case 243:
/* Line 670 of lalr1.cc  */
#line 673 "parser/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 245:
/* Line 670 of lalr1.cc  */
#line 677 "parser/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 262:
/* Line 670 of lalr1.cc  */
#line 726 "parser/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 263:
/* Line 670 of lalr1.cc  */
#line 727 "parser/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 264:
/* Line 670 of lalr1.cc  */
#line 728 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), false); }
    break;

  case 265:
/* Line 670 of lalr1.cc  */
#line 729 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), true); }
    break;

  case 266:
/* Line 670 of lalr1.cc  */
#line 730 "parser/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 267:
/* Line 670 of lalr1.cc  */
#line 731 "parser/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 268:
/* Line 670 of lalr1.cc  */
#line 732 "parser/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 269:
/* Line 670 of lalr1.cc  */
#line 733 "parser/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 270:
/* Line 670 of lalr1.cc  */
#line 734 "parser/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 271:
/* Line 670 of lalr1.cc  */
#line 735 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmARegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 272:
/* Line 670 of lalr1.cc  */
#line 736 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 273:
/* Line 670 of lalr1.cc  */
#line 737 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmCRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 274:
/* Line 670 of lalr1.cc  */
#line 740 "parser/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 275:
/* Line 670 of lalr1.cc  */
#line 741 "parser/bif.y"
    { currentPartitionBifOptions->acFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 276:
/* Line 670 of lalr1.cc  */
#line 742 "parser/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 302:
/* Line 670 of lalr1.cc  */
#line 776 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 303:
/* Line 670 of lalr1.cc  */
#line 779 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 304:
/* Line 670 of lalr1.cc  */
#line 782 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 305:
/* Line 670 of lalr1.cc  */
#line 785 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 314:
/* Line 670 of lalr1.cc  */
#line 806 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP || options.GetArchType() == Arch::VERSAL)
                                                                   LOG_ERROR("BIF attribute error !!!\n\t\t'lms_key_params' is not supported with the mentioned -arch");
                                                                 lmsParams.lms_param = (yysemantic_stack_[(1) - (1)].string);
                                                                 currentBifOptions->SetLmsParameters(lmsParams) ;
                                                               }
    break;

  case 316:
/* Line 670 of lalr1.cc  */
#line 813 "parser/bif.y"
    { lmsParams.type = (yysemantic_stack_[(2) - (1)].authkeylevel_t);}
    break;

  case 319:
/* Line 670 of lalr1.cc  */
#line 817 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 323:
/* Line 670 of lalr1.cc  */
#line 827 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 324:
/* Line 670 of lalr1.cc  */
#line 830 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 329:
/* Line 670 of lalr1.cc  */
#line 842 "parser/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 331:
/* Line 670 of lalr1.cc  */
#line 849 "parser/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 332:
/* Line 670 of lalr1.cc  */
#line 850 "parser/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 334:
/* Line 670 of lalr1.cc  */
#line 855 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 335:
/* Line 670 of lalr1.cc  */
#line 856 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 336:
/* Line 670 of lalr1.cc  */
#line 857 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 338:
/* Line 670 of lalr1.cc  */
#line 861 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 339:
/* Line 670 of lalr1.cc  */
#line 862 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 341:
/* Line 670 of lalr1.cc  */
#line 866 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 342:
/* Line 670 of lalr1.cc  */
#line 867 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 344:
/* Line 670 of lalr1.cc  */
#line 871 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 346:
/* Line 670 of lalr1.cc  */
#line 875 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 348:
/* Line 670 of lalr1.cc  */
#line 880 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1799 "bisonflex/bif.tab.cpp"
      default:
        break;
      }

    /* User semantic actions sometimes alter yychar, and that requires
       that yytoken be updated with the new translation.  We take the
       approach of translating immediately before every use of yytoken.
       One alternative is translating here after every semantic action,
       but that translation would be missed if the semantic action
       invokes YYABORT, YYACCEPT, or YYERROR immediately after altering
       yychar.  In the case of YYABORT or YYACCEPT, an incorrect
       destructor might then be invoked immediately.  In the case of
       YYERROR, subsequent parser actions might lead to an incorrect
       destructor call or verbose syntax error message before the
       lookahead is translated.  */
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* Make sure we have latest lookahead translation.  See comments at
       user semantic actions for why this is necessary.  */
    yytoken = yytranslate_ (yychar);

    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	if (yychar == yyempty_)
	  yytoken = yyempty_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[1] = yylloc;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */
        if (yychar <= yyeof_)
          {
            /* Return failure if at end of input.  */
            if (yychar == yyeof_)
              YYABORT;
          }
        else
          {
            yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
            yychar = yyempty_;
          }
      }

    /* Else will try to reuse lookahead token after shifting the error
       token.  */
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

    yyerror_range[1] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
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

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	  YYABORT;

	yyerror_range[1] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    yyerror_range[2] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the lookahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyempty_)
      {
        /* Make sure we have latest lookahead translation.  See comments
           at user semantic actions for why this is necessary.  */
        yytoken = yytranslate_ (yychar);
        yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval,
                     &yylloc);
      }

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystate_stack_.height ())
      {
        yydestruct_ ("Cleanup: popping",
                     yystos_[yystate_stack_[0]],
                     &yysemantic_stack_[0],
                     &yylocation_stack_[0]);
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
        if (yychar != yyempty_)
          {
            /* Make sure we have latest lookahead translation.  See
               comments at user semantic actions for why this is
               necessary.  */
            yytoken = yytranslate_ (yychar);
            yydestruct_ (YY_NULL, yytoken, &yylval, &yylloc);
          }

        while (1 < yystate_stack_.height ())
          {
            yydestruct_ (YY_NULL,
                         yystos_[yystate_stack_[0]],
                         &yysemantic_stack_[0],
                         &yylocation_stack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  // Generate an error message.
  std::string
  BisonParser::yysyntax_error_ (int, int)
  {
    return YY_("syntax error");
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const short int BisonParser::yypact_ninf_ = -505;
  const short int
  BisonParser::yypact_[] =
  {
      -505,    33,   -26,  -505,    51,  -505,  -505,   -30,    54,  -505,
    -505,  -505,    38,  -505,  1196,  -505,  -505,    -1,   108,    36,
     114,   124,   131,   134,   113,   150,    14,   155,   157,   160,
     158,   167,   186,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,   184,   188,   192,   193,  -505,  -505,  -505,
    -505,  -505,   117,  -505,  -505,  -505,  -505,  -505,   194,  -505,
    -505,  -505,   893,   190,   191,   209,  -505,   210,   211,  1050,
      52,    -5,    42,   -51,   -51,   -51,   -51,  -505,   -51,  -505,
      14,  -505,  -505,   -30,  -505,    69,   126,    60,   -38,   -30,
     -30,   -30,   -30,  -505,   -30,  -505,  -505,  -505,  -505,  -505,
     222,   227,   228,   234,   236,   237,   247,   249,   250,   251,
     254,   256,   257,   258,   259,   260,  -505,  -505,   261,   262,
     264,   265,  -505,  -505,  -505,  -505,  -505,  -505,   266,   267,
     268,   269,   271,   272,   274,   285,   290,   291,  -505,   301,
    -505,   302,   303,   304,   305,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,   306,   307,   308,   312,   313,   314,   317,   321,
     323,   326,   328,   329,   330,   331,  -505,  -505,  -505,   348,
     579,   736,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,    42,   -51,    69,   221,    34,    40,   353,   350,   357,
    -505,   358,  -505,  -505,   359,   360,  -505,  -505,  -505,  -505,
     -51,   -51,   -51,  -505,  -505,   -95,  -112,  -103,   195,   197,
     196,   196,   196,   196,  -505,   196,  -505,  -505,   362,   364,
     366,   339,  -505,  -505,  -505,  -505,  -505,  -505,  -505,   361,
     368,   369,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
     372,    23,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
     371,   376,   377,   378,   380,   383,    15,  -505,   382,   385,
     388,   386,   391,   389,    91,     3,  -505,  -505,  -505,  -505,
    -505,   -61,   -64,   -72,   245,  1113,   -51,   238,   235,   239,
     240,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   231,   -51,
     -51,   -51,   -51,   -30,   -30,   311,   -30,   -51,   -51,   -51,
     -51,   -30,   -51,   -51,   -51,    69,   -30,   -30,   -30,   -30,
     -30,   -30,   -30,   -30,   -30,   -30,   -30,   -30,   -30,   -30,
    -505,   893,  -505,   893,  -505,  -505,  -505,   398,   196,  -505,
     126,   399,   401,   402,  -505,   400,   404,   405,   407,    52,
     -30,  1050,   -51,  -505,    -5,  -505,  -505,  -140,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   375,  -505,
     -30,   337,   410,   411,   412,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   422,   423,   425,   426,   427,   428,
     429,   430,   434,   435,  -505,  -505,   438,     8,   -51,   -51,
     -51,  -505,   126,  -505,   -51,   -51,   351,   -51,   -30,  -505,
      60,  -505,   275,   -51,   -51,  -505,  -505,   -17,  -505,   355,
    -505,   273,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,   196,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
     196,   196,   196,   196,   196,   196,   196,  -505,  -505,   196,
     196,  -505,   441,  -132,  -505,   196,  -505,  -505,  -505,  -505,
     196,   196,   196,   196,  -505,   196,   196,   196,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,   -51,  -505,   -51,   -51,   231,
      34,   289,    46,    40,  -505,  -505,  -505,   196,  -505,  -505,
    -505,  -505,  -505,   -95,   -95,  -112,  -112,  -103,   195,   197,
    -505,   451,   453,   457,   458,   459,   460,   461,   465,  -505,
    -505,   479,  -505,  -505,   172,  -505,  -505,  -505,  -505,   480,
     -61,   -64,   -72,   -51,   -51,   -51,   -30,    69,   -30,   -30,
     -30,   -30,   -30,   -30,   -30,   -30,   -30,   -30,   -30,   -30,
     -30,   -30,  -505,   339,  -505,   196,   196,   196,  -505,   196,
     196,  -505,   196,  -505,  -505,  -505,  -505,   196,   196,   355,
    -505,   482,   -51,    76,  -505,   196,   196,  -505,   400,  -505,
    -505,  -505,  -505,  -505,   407,    52,   -51,   -51,   -51,   -51,
     -51,   -51,   397,   -51,   226,  -505,   -51,  -505,  -505,  -505,
     196,   196,  -505,   481,  -129,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,   105,  -505,   309,  -137,  -505,   196,   196,   196,
     196,   196,   196,  -505,   196,  -505,   196,   -51,   110,  -505,
    -505,  -505,  -505,   310,  -136,  -505,  -505
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,     6,     4,     0,     0,   325,
     326,     5,     0,     8,     0,   146,     7,   143,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   283,   282,   234,   233,   235,   236,   237,
     238,   239,   231,   277,   285,   286,   287,   288,   289,   290,
     291,   292,   293,     0,     0,     0,     0,   279,   280,   281,
      12,    13,    11,    67,    15,    10,     9,    14,     0,   232,
     278,   145,     0,     0,     0,     0,    97,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    69,     0,   143,
      91,    92,    93,    49,    16,     0,     0,     0,     0,     0,
       0,     0,     0,    68,     0,   176,   185,   183,   184,   191,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   192,
       0,     0,     0,     0,     0,     0,   268,   269,     0,     0,
       0,     0,   177,   178,   179,   180,   181,   182,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   229,     0,
     230,     0,     0,     0,     0,   284,   299,   300,   187,   301,
     186,   298,   277,   286,   287,   288,   289,   290,   291,   292,
     293,   294,   295,   296,   297,   279,   248,   228,   225,     0,
     149,   148,   167,   163,   220,   164,   190,   165,   166,   188,
     189,     0,     0,     0,     0,     0,     0,     0,     0,   161,
      65,     0,   114,   258,     0,   111,   113,   102,   328,   327,
       0,     0,     0,   330,   333,   337,   340,   343,   345,   347,
      66,    56,    57,    58,    71,    55,    94,    95,     0,    46,
       0,    21,   249,    64,   142,   138,   139,   140,   134,     0,
       0,     0,   250,   251,   253,   254,   255,   256,   257,   252,
       0,   116,   127,   128,   136,   129,   130,   131,   133,   132,
       0,     0,     0,     0,     0,     0,   119,   314,     0,     0,
       0,     0,     0,   308,     0,     0,    59,    60,    61,    62,
      63,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     147,     0,   152,     0,   153,   101,   105,   172,   174,    96,
       0,     0,     0,     0,   305,   103,     0,     0,   104,     0,
       0,     0,     0,   100,     0,   331,   332,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   107,
      49,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    45,    43,     0,    18,     0,     0,
       0,    99,     0,   118,     0,     0,     0,     0,     0,   106,
       0,   121,     0,     0,     0,   316,   108,     0,   109,     0,
     110,   318,   240,   241,   242,   197,   243,   244,   198,   245,
     246,   199,   247,   200,   158,   159,   294,   295,   296,   297,
     157,   190,   270,   260,   202,   259,   201,   261,   203,   193,
     262,   263,   264,   265,   266,   267,   218,   307,   217,   223,
     224,   168,   170,   194,   169,   175,   274,   275,   160,   276,
     226,   227,   156,   155,   154,   271,   272,   273,   222,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   219,   150,   151,     0,    98,     0,     0,     0,
       0,     0,     0,     0,   221,   144,   162,   115,   112,   329,
     334,   335,   336,   338,   339,   341,   342,   344,   346,   348,
      70,     0,     0,     0,     0,     0,     0,     0,     0,    81,
      82,     0,    83,    72,    76,    75,    73,    74,    47,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    17,    21,    20,   141,   135,   137,   117,   122,
     123,   124,   125,   126,   120,   313,   310,   312,   311,     0,
     309,   315,     0,     0,   173,   302,   304,   303,   306,   322,
     319,   323,   324,   320,   321,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    78,     0,    25,    22,    42,
      40,    41,    44,    50,    52,    39,    23,    24,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    19,     0,   171,     0,     0,    85,    89,    90,    86,
      87,    88,    79,    80,    84,    77,    48,     0,     0,   317,
     196,   195,    51,     0,     0,    54,    53
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -505,  -505,  -505,  -505,  -505,  -505,  -505,  -505,  -388,  -505,
     119,  -505,  -156,  -505,  -505,  -505,   433,  -505,  -505,  -504,
    -505,   128,  -505,  -505,  -505,   138,  -505,  -237,  -251,  -505,
    -505,   -15,  -505,   -14,  -505,  -162,  -505,   146,   -75,  -505,
     -81,     7,  -505,  -505,   218,  -505,  -505,  -505,  -505,   -35,
     -33,   -36,  -505,   -70,  -187,  -505,  -505,  -505,  -505,  -505,
    -505,  -505,  -505,   -66,  -505,  -505,  -505,  -505,  -505,   495,
    -505,  -505,    10,    12,  -505,    95,  -505,  -411,   424,  -505,
      16,  -505,  -505,    22,  -505,  -173,  -250,  -209,   156,   161,
     159,   -83
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     6,     8,    14,    60,   231,   396,   397,
     228,   229,   612,   613,    61,    62,    63,   224,   368,   533,
     534,    64,    90,    65,   194,   204,   205,   250,   265,   266,
     251,    66,    79,    67,    72,   179,   180,   198,   181,   182,
     461,   336,   337,   464,   183,   184,   462,   185,    68,   425,
     428,   431,   433,   186,   233,   252,   253,   254,   255,   256,
     257,   258,   259,   206,   446,   444,   448,   187,   188,   189,
      70,   190,   345,   458,   272,   273,   576,   274,   421,   579,
     348,   590,   593,    71,   213,   214,   215,   216,   217,   218,
     219,   338
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       220,   221,   222,   223,   199,   225,   339,   420,   581,   564,
     200,    91,    92,   563,   403,   411,   207,    15,   332,   334,
     410,   201,    73,   429,    89,   372,   373,   374,   402,    11,
     605,   426,   367,     3,   422,   367,   367,   355,   356,    81,
     367,    13,    82,   367,   509,   208,   209,   651,   656,   375,
     376,   583,    74,   267,   648,     4,   260,   261,     7,   377,
     202,    12,   378,     9,    10,     5,   361,   362,   268,   269,
     270,   341,   342,   343,   344,   226,   227,   363,   364,   430,
     358,   359,   360,   346,   347,   234,   427,   423,   424,   268,
     269,   270,   235,   236,   237,   418,   419,    75,    76,    77,
     645,   260,   261,   496,   591,   592,   379,     9,    10,   649,
     419,   513,   514,    78,    80,   230,    87,   262,   263,   264,
      83,   276,   277,   278,   279,   335,   280,   210,   478,   357,
      84,   211,   212,   271,   238,   239,   240,    85,   241,   380,
      86,   381,   382,   383,   384,   385,   386,   387,   388,   389,
     390,   391,   392,   393,   515,   516,    88,   203,    93,   574,
      94,    96,   262,   263,   264,   568,    95,   394,   632,   493,
      97,   494,   208,   209,   271,   631,   395,   604,   242,   243,
     244,   245,   246,   247,   248,   510,   511,   512,   234,    98,
      99,   249,    24,   521,   100,   235,   236,   237,   101,   102,
     104,   191,   192,   442,   203,   176,   208,   209,   450,   451,
     452,   453,   454,   455,   456,   441,   459,   460,   463,   465,
     193,   195,   196,   232,   470,   471,   472,   473,   281,   475,
     476,   477,   340,   282,   283,   522,   523,   238,   239,   240,
     284,   241,   285,   286,   524,   525,   526,   521,   527,   528,
     529,   530,   531,   287,   210,   288,   289,   290,   211,   212,
     291,   634,   292,   293,   294,   295,   296,   297,   298,   507,
     299,   300,   301,   302,   303,   304,   199,   305,   306,   504,
     307,   242,   243,   244,   245,   246,   247,   248,   210,   522,
     523,   308,   211,   212,   249,   653,   309,   310,   524,   525,
     526,   532,   527,   528,   529,   530,   531,   311,   312,   313,
     314,   315,   316,   317,   318,   565,   566,   567,   319,   320,
     321,   569,   570,   322,   572,   466,   467,   323,   469,   324,
     577,   578,   325,   474,   326,   327,   328,   329,   479,   480,
     481,   482,   483,   484,   485,   486,   487,   488,   489,   490,
     491,   492,   330,   536,   537,   532,   372,   373,   374,   349,
     616,   350,   351,   353,   352,   354,   369,   398,   367,   365,
     366,   371,   505,   370,   399,   400,   401,   404,    15,   520,
     375,   376,   405,   406,   407,    89,   408,   409,   412,   415,
     377,   413,   230,   378,   414,   416,   521,   432,   417,   445,
     457,   443,   468,   495,   447,   497,   449,   498,   499,   500,
     501,   502,   503,   539,   585,   586,   540,   541,   542,   543,
     544,   545,   546,   547,   548,   549,   550,   551,   552,   553,
     573,   554,   555,   556,   557,   558,   559,   379,   522,   523,
     560,   561,   562,   571,   271,   575,   267,   524,   525,   526,
     582,   527,   528,   529,   530,   531,   589,   595,    26,   596,
     610,   611,   614,   597,   598,   599,   600,   601,     9,    10,
     380,   602,   381,   382,   383,   384,   385,   386,   387,   388,
     389,   390,   391,   392,   393,   603,   606,   419,   643,   538,
     647,   652,   508,   650,   655,   103,   535,   506,   394,   463,
     635,   633,   584,   440,   532,   607,   609,   395,   608,    69,
     588,   587,   580,   637,   638,   639,   640,   641,   642,   594,
     644,   517,   275,   646,     0,   636,   519,   518,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   614,   654,     0,     0,   615,     0,
     617,   618,   619,   620,   621,   622,   623,   624,   625,   626,
     627,   628,   629,   630,   331,     0,     0,     0,     0,     0,
       0,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,     0,   116,   117,     0,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,     0,   128,   129,     0,
     130,   131,     0,     0,   132,   133,   134,   135,   136,   137,
     138,   139,     0,   140,   141,   142,   143,     0,     0,   144,
     145,     0,     0,     0,   146,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   147,     0,     0,     0,     0,
       0,   148,     0,   149,     0,     0,   150,   151,   152,   153,
       0,     0,     0,     0,     0,     0,     0,   154,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    33,    34,   155,     0,     0,
       0,     0,     0,     0,   156,   157,   158,   159,   160,   161,
     162,    44,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,    58,    59,     0,     0,     0,
       0,     0,   176,     0,     0,     0,     0,     0,   177,     0,
       0,   333,     0,     0,     0,     0,     0,   178,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,     0,
     116,   117,     0,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,     0,   128,   129,     0,   130,   131,     0,
       0,   132,   133,   134,   135,   136,   137,   138,   139,     0,
     140,   141,   142,   143,     0,     0,   144,   145,     0,     0,
       0,   146,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   147,     0,     0,     0,     0,     0,   148,     0,
     149,     0,     0,   150,   151,   152,   153,     0,     0,     0,
       0,     0,     0,     0,   154,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    34,   155,     0,     0,     0,     0,     0,
       0,   156,   157,   158,   159,   160,   161,   162,    44,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,    58,    59,     0,     0,     0,     0,     0,   176,
       0,     0,     0,     0,     0,   177,     0,     0,     0,     0,
       0,     0,     0,     0,   178,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,     0,   116,   117,     0,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
       0,   128,   129,     0,   130,   131,     0,     0,   132,   133,
     134,   135,   136,   137,   138,   139,     0,   140,   141,   142,
     143,     0,     0,   144,   145,     0,     0,     0,   146,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   147,
       0,     0,     0,     0,     0,   148,     0,   149,     0,     0,
     150,   151,   152,   153,     0,     0,     0,     0,     0,     0,
       0,   154,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    33,
      34,   155,     0,     0,     0,     0,     0,     0,   156,   157,
     158,   159,   160,   161,   162,    44,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,    58,
      59,     0,     0,     0,     0,     0,   176,     0,     0,     0,
       0,     0,   177,     0,     0,     0,     0,     0,     0,     0,
       0,   178,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   197,   115,     0,   116,   117,     0,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,     0,   128,   129,
       0,   130,   131,     0,     0,   132,   133,   134,   135,   136,
     137,   138,   139,     0,   140,   141,     0,   143,     0,     0,
     144,   145,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   105,   106,   107,   108,   109,
       0,     0,   148,     0,     0,     0,     0,   150,   151,   152,
     153,     0,     0,     0,     0,     0,     0,     0,   154,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   132,   133,
     134,   135,   136,   137,     0,     0,    33,    34,   155,     0,
       0,     0,     0,     0,     0,   156,   157,   158,   159,   160,
     161,   162,    44,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,    58,    59,     0,    15,
      16,     0,     0,   176,     0,     0,    17,     0,     0,   177,
       0,     0,     0,     0,     0,     0,     0,    18,   178,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    33,
      34,   155,     0,     0,     0,     0,     0,    20,   434,   435,
     158,   159,   160,   161,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,   436,   437,   438,   439,    57,    58,
      59,     0,    21,    22,    23,     0,   176,     0,     0,     0,
       0,    24,    25,     0,     0,     0,     0,     0,     0,    26,
       0,    27,    28,     0,     0,     0,     0,     0,     0,     9,
      10,     0,     0,     0,    29,    30,     0,    31,     0,     0,
       0,    32,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    34,    35,    36,    37,    38,    39,    40,
      41,     0,     0,     0,     0,    42,     0,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        83,    84,    85,    86,    79,    88,   193,     4,   419,   397,
      80,    26,    26,     5,   251,   266,    82,     3,   180,   181,
       5,    26,    23,    95,    10,    17,    18,    19,     5,     7,
     534,    95,   172,     0,    95,   172,   172,   210,   211,     3,
     172,     3,     6,   172,   184,    96,    97,   184,   184,    41,
      42,   183,    53,    91,   183,    81,    41,    42,     7,    51,
      65,     7,    54,    93,    94,    91,   178,   179,   106,   107,
     108,    37,    38,    39,    40,    90,    90,   180,   181,   151,
     175,   176,   177,    43,    44,    62,   150,   148,   149,   106,
     107,   108,    69,    70,    71,     4,     5,    98,    99,   100,
     604,    41,    42,   340,    58,    59,    98,    93,    94,     4,
       5,   361,   362,   114,     6,    93,     3,   102,   103,   104,
       6,    99,   100,   101,   102,   191,   104,   178,   315,   212,
       6,   182,   183,   171,   111,   112,   113,     6,   115,   131,
       6,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   363,   364,     6,   162,     3,   410,
       3,     3,   102,   103,   104,   402,     6,   159,   579,   331,
       3,   333,    96,    97,   171,   563,   168,     5,   155,   156,
     157,   158,   159,   160,   161,   358,   359,   360,    62,     3,
       6,   168,    75,    21,     6,    69,    70,    71,     6,     6,
       6,    11,    11,   286,   162,   153,    96,    97,   291,   292,
     293,   294,   295,   296,   297,   285,   299,   300,   301,   302,
      11,    11,    11,   154,   307,   308,   309,   310,     6,   312,
     313,   314,    11,     6,     6,    63,    64,   111,   112,   113,
       6,   115,     6,     6,    72,    73,    74,    21,    76,    77,
      78,    79,    80,     6,   178,     6,     6,     6,   182,   183,
       6,   185,     6,     6,     6,     6,     6,     6,     6,   352,
       6,     6,     6,     6,     6,     6,   351,     6,     6,   349,
       6,   155,   156,   157,   158,   159,   160,   161,   178,    63,
      64,     6,   182,   183,   168,   185,     6,     6,    72,    73,
      74,   129,    76,    77,    78,    79,    80,     6,     6,     6,
       6,     6,     6,     6,     6,   398,   399,   400,     6,     6,
       6,   404,   405,     6,   407,   303,   304,     6,   306,     6,
     413,   414,     6,   311,     6,     6,     6,     6,   316,   317,
     318,   319,   320,   321,   322,   323,   324,   325,   326,   327,
     328,   329,     4,   368,   368,   129,    17,    18,    19,     6,
     547,    11,     5,     4,     6,     5,     4,     6,   172,   174,
     173,     5,   350,     9,     6,     6,     4,     6,     3,     4,
      41,    42,     6,     6,     6,    10,     6,     4,     6,     3,
      51,     6,   370,    54,     6,     4,    21,   152,     9,   164,
     169,   163,    91,     5,   165,     6,   166,     6,     6,     9,
       6,     6,     5,    76,   497,   498,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
     408,     6,     6,     6,     6,     6,     6,    98,    63,    64,
       6,     6,     4,    92,   171,   170,    91,    72,    73,    74,
       9,    76,    77,    78,    79,    80,   167,     6,    83,     6,
     543,   544,   545,     6,     6,     6,     6,     6,    93,    94,
     131,     6,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,     6,     6,     5,    91,   370,
       9,   647,   354,   184,   184,    62,   368,   351,   159,   582,
     583,   582,   495,   285,   129,   540,   542,   168,   541,    14,
     500,   499,   417,   596,   597,   598,   599,   600,   601,   503,
     603,   365,    98,   606,    -1,   595,   367,   366,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   647,   648,    -1,    -1,   546,    -1,
     548,   549,   550,   551,   552,   553,   554,   555,   556,   557,
     558,   559,   560,   561,     5,    -1,    -1,    -1,    -1,    -1,
      -1,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    -1,    24,    25,    -1,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    -1,    38,    39,    -1,
      41,    42,    -1,    -1,    45,    46,    47,    48,    49,    50,
      51,    52,    -1,    54,    55,    56,    57,    -1,    -1,    60,
      61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,
      -1,    82,    -1,    84,    -1,    -1,    87,    88,    89,    90,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   116,   117,   118,    -1,    -1,
      -1,    -1,    -1,    -1,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,    -1,    -1,    -1,
      -1,    -1,   153,    -1,    -1,    -1,    -1,    -1,   159,    -1,
      -1,     5,    -1,    -1,    -1,    -1,    -1,   168,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    -1,
      24,    25,    -1,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    -1,    38,    39,    -1,    41,    42,    -1,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    -1,
      54,    55,    56,    57,    -1,    -1,    60,    61,    -1,    -1,
      -1,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    82,    -1,
      84,    -1,    -1,    87,    88,    89,    90,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   116,   117,   118,    -1,    -1,    -1,    -1,    -1,
      -1,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,    -1,    -1,    -1,    -1,    -1,   153,
      -1,    -1,    -1,    -1,    -1,   159,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   168,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    -1,    24,    25,    -1,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      -1,    38,    39,    -1,    41,    42,    -1,    -1,    45,    46,
      47,    48,    49,    50,    51,    52,    -1,    54,    55,    56,
      57,    -1,    -1,    60,    61,    -1,    -1,    -1,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      -1,    -1,    -1,    -1,    -1,    82,    -1,    84,    -1,    -1,
      87,    88,    89,    90,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,
     117,   118,    -1,    -1,    -1,    -1,    -1,    -1,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,    -1,    -1,    -1,    -1,    -1,   153,    -1,    -1,    -1,
      -1,    -1,   159,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   168,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    -1,    24,    25,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    -1,    38,    39,
      -1,    41,    42,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    -1,    54,    55,    -1,    57,    -1,    -1,
      60,    61,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    12,    13,    14,    15,    16,
      -1,    -1,    82,    -1,    -1,    -1,    -1,    87,    88,    89,
      90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    45,    46,
      47,    48,    49,    50,    -1,    -1,   116,   117,   118,    -1,
      -1,    -1,    -1,    -1,    -1,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,    -1,     3,
       4,    -1,    -1,   153,    -1,    -1,    10,    -1,    -1,   159,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    21,   168,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,
     117,   118,    -1,    -1,    -1,    -1,    -1,    41,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,    -1,    66,    67,    68,    -1,   153,    -1,    -1,    -1,
      -1,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,    83,
      -1,    85,    86,    -1,    -1,    -1,    -1,    -1,    -1,    93,
      94,    -1,    -1,    -1,    98,    99,    -1,   101,    -1,    -1,
      -1,   105,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   116,   117,   118,   119,   120,   121,   122,   123,
     124,    -1,    -1,    -1,    -1,   129,    -1,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned short int
  BisonParser::yystos_[] =
  {
         0,   187,   188,     0,    81,    91,   189,     7,   190,    93,
      94,   269,     7,     3,   191,     3,     4,    10,    21,    23,
      41,    66,    67,    68,    75,    76,    83,    85,    86,    98,
      99,   101,   105,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   129,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     192,   200,   201,   202,   207,   209,   217,   219,   234,   255,
     256,   269,   220,    23,    53,    98,    99,   100,   114,   218,
       6,     3,     6,     6,     6,     6,     6,     3,     6,    10,
     208,   217,   219,     3,     3,     6,     3,     3,     3,     6,
       6,     6,     6,   202,     6,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    24,    25,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    38,    39,
      41,    42,    45,    46,    47,    48,    49,    50,    51,    52,
      54,    55,    56,    57,    60,    61,    65,    76,    82,    84,
      87,    88,    89,    90,    98,   118,   125,   126,   127,   128,
     129,   130,   131,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   153,   159,   168,   221,
     222,   224,   225,   230,   231,   233,   239,   253,   254,   255,
     257,    11,    11,    11,   210,    11,    11,    21,   223,   224,
     239,    26,    65,   162,   211,   212,   249,   249,    96,    97,
     178,   182,   183,   270,   271,   272,   273,   274,   275,   276,
     277,   277,   277,   277,   203,   277,   217,   219,   196,   197,
     269,   193,   154,   240,    62,    69,    70,    71,   111,   112,
     113,   115,   155,   156,   157,   158,   159,   160,   161,   168,
     213,   216,   241,   242,   243,   244,   245,   246,   247,   248,
      41,    42,   102,   103,   104,   214,   215,    91,   106,   107,
     108,   171,   260,   261,   263,   264,   269,   269,   269,   269,
     269,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       4,     5,   221,     5,   221,   249,   227,   228,   277,   240,
      11,    37,    38,    39,    40,   258,    43,    44,   266,     6,
      11,     5,     6,     4,     5,   271,   271,   277,   175,   176,
     177,   178,   179,   180,   181,   174,   173,   172,   204,     4,
       9,     5,    17,    18,    19,    41,    42,    51,    54,    98,
     131,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   159,   168,   194,   195,     6,     6,
       6,     4,     5,   213,     6,     6,     6,     6,     6,     4,
       5,   214,     6,     6,     6,     3,     4,     9,     4,     5,
       4,   264,    95,   148,   149,   235,    95,   150,   236,    95,
     151,   237,   152,   238,   125,   126,   141,   142,   143,   144,
     230,   239,   277,   163,   251,   164,   250,   165,   252,   166,
     277,   277,   277,   277,   277,   277,   277,   169,   259,   277,
     277,   226,   232,   277,   229,   277,   269,   269,    91,   269,
     277,   277,   277,   277,   269,   277,   277,   277,   240,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   269,   269,   221,   221,     5,   213,     6,     6,     6,
       9,     6,     6,     5,   239,   269,   223,   277,   211,   184,
     271,   271,   271,   272,   272,   273,   273,   274,   275,   276,
       4,    21,    63,    64,    72,    73,    74,    76,    77,    78,
      79,    80,   129,   205,   206,   207,   217,   219,   196,    76,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     4,     5,   194,   277,   277,   277,   213,   277,
     277,    92,   277,   269,   214,   170,   262,   277,   277,   265,
     261,   263,     9,   183,   227,   277,   277,   259,   258,   167,
     267,    58,    59,   268,   266,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     5,   205,     6,   235,   236,   237,
     277,   277,   198,   199,   277,   269,   240,   269,   269,   269,
     269,   269,   269,   269,   269,   269,   269,   269,   269,   269,
     269,   194,   263,   226,   185,   277,   239,   277,   277,   277,
     277,   277,   277,    91,   277,   205,   277,     9,   183,     4,
     184,   184,   198,   185,   277,   184,   184
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  BisonParser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414,
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned short int
  BisonParser::yyr1_[] =
  {
         0,   186,   187,   188,   188,   189,   190,   189,   191,   191,
     191,   191,   191,   191,   191,   191,   193,   192,   194,   194,
     194,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   196,   196,   197,   197,
     198,   198,   199,   199,   199,   200,   200,   200,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   201,   201,   203,
     202,   204,   204,   204,   204,   204,   205,   205,   205,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   207,   208,   208,   208,   208,   209,   210,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   211,   211,   212,   212,   212,   213,   213,   213,   214,
     214,   214,   215,   215,   215,   215,   215,   216,   216,   216,
     216,   216,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   216,   216,   218,   217,   217,   220,   219,   221,   221,
     221,   221,   221,   221,   222,   222,   222,   222,   222,   222,
     222,   223,   223,   224,   224,   224,   224,   224,   225,   225,
     226,   226,   227,   227,   228,   229,   230,   230,   230,   230,
     230,   230,   230,   230,   230,   230,   230,   230,   230,   230,
     230,   230,   231,   231,   232,   232,   232,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   234,   234,   234,   234,   234,   234,   234,   234,   234,
     235,   235,   235,   236,   236,   237,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   253,   253,   253,   253,   253,   253,   253,
     253,   253,   253,   253,   254,   254,   254,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   256,   256,   256,   256,
     256,   256,   256,   256,   256,   256,   256,   256,   257,   257,
     257,   257,   258,   258,   258,   258,   258,   259,   260,   260,
     261,   261,   261,   262,   263,   263,   265,   264,   264,   266,
     266,   266,   267,   268,   268,   269,   269,   270,   270,   270,
     271,   271,   271,   272,   272,   272,   272,   273,   273,   273,
     274,   274,   274,   275,   275,   276,   276,   277,   277
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     3,     0,     6,     0,     2,
       2,     2,     2,     2,     2,     2,     0,     5,     1,     3,
       2,     0,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     3,     1,     1,     3,     5,     0,
       1,     3,     1,     4,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     2,     0,
       5,     0,     2,     2,     2,     2,     1,     3,     2,     3,
       3,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     2,     1,     1,     2,     2,     4,     0,     5,     4,
       4,     4,     3,     4,     4,     4,     4,     4,     4,     4,
       4,     1,     3,     1,     1,     3,     1,     3,     2,     1,
       3,     2,     3,     3,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     3,     1,     1,
       1,     3,     1,     0,     5,     1,     0,     4,     1,     1,
       3,     3,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     1,     3,     1,     1,     1,     1,     1,     3,     3,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     4,     4,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     1,     3,     1,     1,     3,
       3,     3,     3,     1,     1,     3,     0,     5,     2,     3,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     2,     2,     1,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     1,     3,     1,     3,     1,     3
  };

#if YYDEBUG
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
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
  "PLDATA", "BOOTIMAGE", "UDF_BH", "INIT", "PMUFW_IMAGE", "AES_KEY_FILE",
  "FAMILY_KEY", "PPK_FILE", "PSK_FILE", "SPK_FILE", "SSK_FILE",
  "PPK_FILE1", "PSK_FILE1", "SPK_FILE1", "SSK_FILE1", "PK_FILE", "SK_FILE",
  "PK_FILE1", "SK_FILE1", "SPK_SIGNATURE_FILE", "BH_SIGNATURE_FILE",
  "HEADER_SIGNATURE_FILE", "AUTHVALUE", "HYBRID_AUTHVALUE", "ENCRVALUE",
  "CHECKSUMVALUE", "POWNERVALUE", "PTYPEVALUE", "KEY_SRC", "CORE",
  "BH_RSA", "AUTH_HASH", "INT_HASH", "PUFHD_LOC", "OPT_KEY", "AUTH_ONLY",
  "BOOT_DEVICE_TYPE", "DEST_DEVICE_TYPE", "DEST_CPU_TYPE",
  "EXCEPTION_LEVEL_TYPE", "TRUSTZONE_TYPE", "SPLITMODE", "DPA_CM",
  "SPKSELECT", "HASH_FUNCTION", "AUTH_KEY_LEVEL", "OR", "XOR", "AND",
  "MULT", "DIVIDE", "MODULO", "PLUS", "MINUS", "LSHIFT", "RSHIFT",
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
  "shift_expression", "and_expression", "xor_expression", "expression", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BisonParser::rhs_number_type
  BisonParser::yyrhs_[] =
  {
       187,     0,    -1,   188,    -1,    -1,   188,   189,    -1,    81,
       7,   269,    -1,    -1,    91,   190,     7,     3,   191,     4,
      -1,    -1,   191,   217,    -1,   191,   209,    -1,   191,   201,
      -1,   191,   192,    -1,   191,   200,    -1,   191,   219,    -1,
     191,   207,    -1,    -1,    86,     3,   193,   194,     4,    -1,
     195,    -1,   195,     5,   194,    -1,   195,   194,    -1,    -1,
      18,     6,   236,    -1,    98,     6,   240,    -1,   131,     6,
     269,    -1,    17,     6,   235,    -1,   133,     6,   269,    -1,
     134,     6,   269,    -1,   135,     6,   269,    -1,   136,     6,
     269,    -1,   137,     6,   269,    -1,   138,     6,   269,    -1,
     139,     6,   269,    -1,   140,     6,   269,    -1,   141,     6,
     269,    -1,   142,     6,   269,    -1,   143,     6,   269,    -1,
     144,     6,   269,    -1,   145,     6,   269,    -1,    54,     6,
     269,    -1,    41,     6,   277,    -1,    42,     6,   277,    -1,
      19,     6,   237,    -1,   168,    -1,    51,     6,   198,    -1,
     159,    -1,   197,    -1,   197,     9,   196,    -1,   269,     5,
      76,     6,   277,    -1,    -1,   199,    -1,   199,     9,   198,
      -1,   277,    -1,   277,   183,   277,   184,    -1,   277,   183,
     185,   184,    -1,    76,     6,   277,    -1,    66,     6,   277,
      -1,    67,     6,   277,    -1,    68,     6,   277,    -1,   141,
       6,   269,    -1,   142,     6,   269,    -1,   143,     6,   269,
      -1,   144,     6,   269,    -1,   234,     6,   269,    -1,    98,
       6,   240,    -1,    21,     6,   239,    -1,    41,     6,   277,
      -1,   202,    -1,   201,   202,    -1,    -1,    75,     3,   203,
     204,     4,    -1,    -1,   204,   205,    -1,   204,   217,    -1,
     204,   219,    -1,   204,   207,    -1,   206,    -1,   206,     5,
     205,    -1,   206,   205,    -1,    76,     6,   277,    -1,    77,
       6,    91,    -1,    78,    -1,    79,    -1,   129,    -1,    80,
       6,   277,    -1,    21,     6,   239,    -1,    72,     6,   277,
      -1,    73,     6,   277,    -1,    74,     6,   277,    -1,    63,
       6,   277,    -1,    64,     6,   277,    -1,    83,   208,    -1,
     217,    -1,   219,    -1,   208,   217,    -1,   208,   219,    -1,
      10,    98,    11,   240,    -1,    -1,    10,    99,   210,    11,
     213,    -1,    99,     3,   213,     4,    -1,    23,     3,   211,
       4,    -1,    10,    23,    11,   249,    -1,    23,     6,   249,
      -1,    10,   100,    11,   258,    -1,    10,   114,    11,   266,
      -1,    10,    53,    11,   227,    -1,   101,     3,   214,     4,
      -1,    85,     3,   196,     4,    -1,   105,     3,   260,     4,
      -1,   105,     3,   263,     4,    -1,   105,     3,   264,     4,
      -1,   212,    -1,   212,     5,   211,    -1,   249,    -1,    65,
      -1,    26,     6,   277,    -1,   216,    -1,   216,     5,   213,
      -1,   216,   213,    -1,   215,    -1,   215,     5,   214,    -1,
     215,   214,    -1,    41,     6,   277,    -1,    42,     6,   277,
      -1,   102,     6,    92,    -1,   103,     6,   277,    -1,   104,
       6,   269,    -1,   241,    -1,   242,    -1,   244,    -1,   245,
      -1,   246,    -1,   248,    -1,   247,    -1,   111,    -1,   113,
       6,   277,    -1,   243,    -1,   115,     6,   277,    -1,    69,
      -1,    70,    -1,    71,    -1,   112,     6,   277,    -1,    62,
      -1,    -1,    10,   218,   223,    11,   269,    -1,   269,    -1,
      -1,     3,   220,   221,     4,    -1,   224,    -1,   222,    -1,
     222,     5,   221,    -1,   224,     5,   221,    -1,   222,   221,
      -1,   224,   221,    -1,    84,     6,   269,    -1,    76,     6,
     277,    -1,    65,     6,   277,    -1,    21,     6,   230,    -1,
      21,     6,   125,    -1,    21,     6,   126,    -1,    56,     6,
      91,    -1,   224,    -1,   224,     5,   223,    -1,   230,    -1,
     233,    -1,   253,    -1,   254,    -1,   225,    -1,    51,     6,
     226,    -1,    52,     6,   229,    -1,   232,    -1,   232,     9,
     226,    -1,   228,    -1,   228,     5,   227,    -1,   277,    -1,
     277,    -1,    12,    -1,    45,    -1,    46,    -1,    47,    -1,
      48,    -1,    49,    -1,    50,    -1,    14,    -1,    15,    -1,
      13,    -1,   129,    -1,   127,    -1,   255,    -1,   257,    -1,
     239,    -1,    16,    -1,    28,    -1,    28,     6,   166,    -1,
     277,    -1,   277,   183,   277,   184,    -1,   277,   183,   185,
     184,    -1,    17,     6,   235,    -1,    18,     6,   236,    -1,
      19,     6,   237,    -1,    20,     6,   238,    -1,    25,     6,
     250,    -1,    24,     6,   251,    -1,    27,     6,   252,    -1,
     131,     6,   269,    -1,   133,     6,   269,    -1,   134,     6,
     269,    -1,   135,     6,   269,    -1,   136,     6,   269,    -1,
     137,     6,   269,    -1,   138,     6,   269,    -1,   139,     6,
     269,    -1,   140,     6,   269,    -1,   141,     6,   269,    -1,
     142,     6,   269,    -1,   143,     6,   269,    -1,   144,     6,
     269,    -1,    39,     6,   259,    -1,    38,     6,   277,    -1,
     145,     6,   269,    -1,   231,    -1,    21,     6,   239,    -1,
      98,     6,   240,    -1,    41,     6,   277,    -1,    42,     6,
     277,    -1,   168,    -1,    60,     6,   277,    -1,    61,     6,
     277,    -1,   159,    -1,    82,    -1,    87,    -1,   129,    -1,
     255,    -1,   119,    -1,   118,    -1,   120,    -1,   121,    -1,
     122,    -1,   123,    -1,   124,    -1,    95,    -1,   148,    -1,
     149,    -1,    95,    -1,   150,    -1,    95,    -1,   151,    -1,
     152,    -1,   153,    -1,   154,    -1,   155,    -1,   156,    -1,
     168,    -1,   157,    -1,   158,    -1,   159,    -1,   160,    -1,
     161,    -1,   162,    -1,   164,    -1,   163,    -1,   165,    -1,
      29,     6,   277,    -1,    30,     6,   277,    -1,    31,     6,
     277,    -1,    32,     6,   277,    -1,    33,     6,   277,    -1,
      34,     6,   277,    -1,    35,    -1,    36,    -1,    22,     6,
     277,    -1,    88,     6,   277,    -1,    89,     6,   277,    -1,
      90,     6,   277,    -1,    54,     6,   269,    -1,    55,     6,
     269,    -1,    57,     6,   269,    -1,   131,    -1,   256,    -1,
     145,    -1,   146,    -1,   147,    -1,   117,    -1,   116,    -1,
     118,    -1,   132,    -1,   133,    -1,   134,    -1,   135,    -1,
     136,    -1,   137,    -1,   138,    -1,   139,    -1,   140,    -1,
     141,    -1,   142,    -1,   143,    -1,   144,    -1,   130,    -1,
     125,    -1,   126,    -1,   128,    -1,    37,     6,   277,    -1,
      39,     6,   259,    -1,    38,     6,   277,    -1,    40,    -1,
     258,     9,   258,    -1,   169,    -1,   261,    -1,   261,     9,
     261,    -1,   106,     6,   262,    -1,   108,     6,   277,    -1,
     107,     6,   277,    -1,   170,    -1,    91,    -1,   263,     5,
     263,    -1,    -1,   171,     3,   265,   263,     4,    -1,   264,
     264,    -1,    43,     6,   267,    -1,    44,     6,   268,    -1,
     266,     5,   266,    -1,   167,    -1,    58,    -1,    59,    -1,
      93,    -1,    94,    -1,    97,    -1,    96,    -1,   183,   277,
     184,    -1,   270,    -1,   178,   271,    -1,   182,   271,    -1,
     271,    -1,   272,   175,   271,    -1,   272,   176,   271,    -1,
     272,   177,   271,    -1,   272,    -1,   273,   178,   272,    -1,
     273,   179,   272,    -1,   273,    -1,   274,   180,   273,    -1,
     274,   181,   273,    -1,   274,    -1,   275,   174,   274,    -1,
     275,    -1,   276,   173,   275,    -1,   276,    -1,   277,   172,
     276,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     5,     6,     9,    13,    14,    21,    22,
      25,    28,    31,    34,    37,    40,    43,    44,    50,    52,
      56,    59,    60,    64,    68,    72,    76,    80,    84,    88,
      92,    96,   100,   104,   108,   112,   116,   120,   124,   128,
     132,   136,   140,   144,   146,   150,   152,   154,   158,   164,
     165,   167,   171,   173,   178,   183,   187,   191,   195,   199,
     203,   207,   211,   215,   219,   223,   227,   231,   233,   236,
     237,   243,   244,   247,   250,   253,   256,   258,   262,   265,
     269,   273,   275,   277,   279,   283,   287,   291,   295,   299,
     303,   307,   310,   312,   314,   317,   320,   325,   326,   332,
     337,   342,   347,   351,   356,   361,   366,   371,   376,   381,
     386,   391,   393,   397,   399,   401,   405,   407,   411,   414,
     416,   420,   423,   427,   431,   435,   439,   443,   445,   447,
     449,   451,   453,   455,   457,   459,   463,   465,   469,   471,
     473,   475,   479,   481,   482,   488,   490,   491,   496,   498,
     500,   504,   508,   511,   514,   518,   522,   526,   530,   534,
     538,   542,   544,   548,   550,   552,   554,   556,   558,   562,
     566,   568,   572,   574,   578,   580,   582,   584,   586,   588,
     590,   592,   594,   596,   598,   600,   602,   604,   606,   608,
     610,   612,   614,   616,   620,   622,   627,   632,   636,   640,
     644,   648,   652,   656,   660,   664,   668,   672,   676,   680,
     684,   688,   692,   696,   700,   704,   708,   712,   716,   720,
     724,   726,   730,   734,   738,   742,   744,   748,   752,   754,
     756,   758,   760,   762,   764,   766,   768,   770,   772,   774,
     776,   778,   780,   782,   784,   786,   788,   790,   792,   794,
     796,   798,   800,   802,   804,   806,   808,   810,   812,   814,
     816,   818,   820,   824,   828,   832,   836,   840,   844,   846,
     848,   852,   856,   860,   864,   868,   872,   876,   878,   880,
     882,   884,   886,   888,   890,   892,   894,   896,   898,   900,
     902,   904,   906,   908,   910,   912,   914,   916,   918,   920,
     922,   924,   926,   930,   934,   938,   940,   944,   946,   948,
     952,   956,   960,   964,   966,   968,   972,   973,   979,   982,
     986,   990,   994,   996,   998,  1000,  1002,  1004,  1006,  1008,
    1012,  1014,  1017,  1020,  1022,  1026,  1030,  1034,  1036,  1040,
    1044,  1046,  1050,  1054,  1056,  1060,  1062,  1066,  1068
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   225,   225,   227,   228,   230,   231,   231,   237,   238,
     239,   240,   241,   242,   243,   244,   247,   247,   252,   253,
     254,   257,   258,   259,   260,   261,   262,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   274,   275,
     276,   277,   284,   285,   286,   287,   291,   292,   295,   296,
     298,   299,   302,   304,   306,   310,   311,   312,   314,   315,
     317,   319,   321,   323,   324,   325,   326,   329,   330,   333,
     333,   344,   345,   346,   347,   348,   351,   352,   353,   356,
     357,   358,   359,   360,   361,   363,   364,   369,   374,   379,
     400,   411,   414,   415,   416,   417,   420,   423,   423,   426,
     427,   428,   433,   434,   435,   436,   437,   438,   439,   440,
     441,   444,   445,   448,   449,   450,   453,   454,   455,   458,
     459,   460,   463,   464,   465,   466,   467,   470,   473,   475,
     477,   479,   481,   483,   485,   487,   489,   491,   495,   496,
     497,   498,   499,   502,   502,   511,   517,   517,   524,   525,
     526,   527,   528,   529,   532,   535,   536,   538,   539,   540,
     541,   563,   564,   567,   568,   569,   570,   571,   574,   575,
     578,   579,   582,   583,   586,   591,   593,   594,   595,   596,
     597,   598,   599,   600,   601,   602,   605,   606,   607,   608,
     609,   610,   613,   614,   617,   618,   619,   621,   622,   623,
     624,   625,   626,   627,   628,   629,   630,   631,   632,   633,
     634,   635,   636,   637,   638,   639,   640,   641,   642,   643,
     644,   645,   646,   647,   648,   649,   650,   651,   652,   653,
     654,   657,   658,   659,   660,   661,   662,   663,   664,   665,
     668,   669,   670,   673,   674,   677,   678,   681,   684,   687,
     690,   693,   696,   699,   702,   705,   708,   711,   714,   717,
     720,   723,   726,   727,   728,   729,   730,   731,   732,   733,
     734,   735,   736,   737,   740,   741,   742,   745,   746,   747,
     748,   749,   750,   751,   752,   753,   756,   757,   758,   759,
     760,   761,   762,   763,   764,   765,   766,   767,   770,   771,
     772,   773,   776,   779,   782,   785,   788,   791,   795,   796,
     798,   799,   800,   802,   806,   811,   813,   813,   815,   817,
     820,   821,   824,   827,   830,   836,   837,   840,   841,   842,
     848,   849,   850,   854,   855,   856,   857,   860,   861,   862,
     865,   866,   867,   870,   871,   874,   875,   879,   880
  };

  // Print the state stack on the debug stream.
  void
  BisonParser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  BisonParser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "):" << std::endl;
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
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
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 1343;
  const int BisonParser::yynnts_ = 92;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 186;

  const unsigned int BisonParser::yyuser_token_number_max_ = 440;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 24 "parser/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 3026 "bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 885 "parser/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        if (inImageScope && !inPartitionScope && (msg.find("syntax error") != std::string::npos))
        {
                Parsing::Error(loc, "BIF attribute error !!!\n\t   '" + std::string(scanner.YYText()) +
                                    "' is not supported at image level.\n\t   Refer 'bootgen -bif_help' for more details");
        }
        else
        {
                Parsing::Error(loc,msg);
        }
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
