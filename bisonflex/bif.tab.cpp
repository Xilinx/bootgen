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
#line 53 "parser/bif.y"

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
#ifndef SKIP_VERSAL_2VP_NATIVE
#include "imageheadertable-versal_2vp.h"
#endif

// Parser uses raw pointers for compatibility with Bison semantics
// These are transferred to options lists which take ownership
BifOptions* currentBifOptions;
PartitionBifOptions* currentPartitionBifOptions ;
ImageBifOptions* currentImageBifOptions;
LmsKeyTypeParam lmsParams;

/* Line 279 of lalr1.cc  */
#line 61 "bisonflex/bif.tab.cpp"


#include "bif.tab.hpp"

/* User implementation prologue.  */

/* Line 285 of lalr1.cc  */
#line 69 "bisonflex/bif.tab.cpp"
/* Unqualified %code blocks.  */
/* Line 286 of lalr1.cc  */
#line 47 "parser/bif.y"

    static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner);
    #include "options.h"


/* Line 286 of lalr1.cc  */
#line 79 "bisonflex/bif.tab.cpp"


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
#line 175 "bisonflex/bif.tab.cpp"

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
#line 112 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 245 "bisonflex/bif.tab.cpp"
        break;
      case 92: /* HEXWORD */
/* Line 455 of lalr1.cc  */
#line 112 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 252 "bisonflex/bif.tab.cpp"
        break;
      case 93: /* FILENAME */
/* Line 455 of lalr1.cc  */
#line 112 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 259 "bisonflex/bif.tab.cpp"
        break;
      case 94: /* QFILENAME */
/* Line 455 of lalr1.cc  */
#line 112 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 266 "bisonflex/bif.tab.cpp"
        break;
      case 148: /* HYBRID_AUTHVALUE */
/* Line 455 of lalr1.cc  */
#line 112 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 273 "bisonflex/bif.tab.cpp"
        break;
      case 268: /* filename */
/* Line 455 of lalr1.cc  */
#line 112 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 280 "bisonflex/bif.tab.cpp"
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
}
/* Line 545 of lalr1.cc  */
#line 381 "bisonflex/bif.tab.cpp"

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
#line 222 "parser/bif.y"
    { options.includeBifOptionsList.push_back((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 223 "parser/bif.y"
    { currentBifOptions = new BifOptions(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series(), (yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 7:
/* Line 670 of lalr1.cc  */
#line 225 "parser/bif.y"
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 239 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(), currentBifOptions->GetIdCode()); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 250 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 251 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 252 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 253 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 254 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 255 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 256 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 257 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 258 "parser/bif.y"
    { currentBifOptions->SetPPKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 259 "parser/bif.y"
    { currentBifOptions->SetPSKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 260 "parser/bif.y"
    { currentBifOptions->SetSPKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 261 "parser/bif.y"
    { currentBifOptions->SetSSKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 34:
/* Line 670 of lalr1.cc  */
#line 262 "parser/bif.y"
    { currentBifOptions->SetPKFileName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 263 "parser/bif.y"
    { currentBifOptions->SetSKFileName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 36:
/* Line 670 of lalr1.cc  */
#line 264 "parser/bif.y"
    { currentBifOptions->SetPKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 37:
/* Line 670 of lalr1.cc  */
#line 265 "parser/bif.y"
    { currentBifOptions->SetSKFileName1((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 38:
/* Line 670 of lalr1.cc  */
#line 266 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 39:
/* Line 670 of lalr1.cc  */
#line 267 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 268 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.partitionRevokeId = (yysemantic_stack_[(3) - (3)].number);}
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 269 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkRevokeId = (yysemantic_stack_[(3) - (3)].number); 
                                                                                if(options.GetArchType() == Arch::VERSAL)
                                                                                {
                                                                                   LOG_ERROR("SPK_REVOKE_ID does not support for versal arch, instead please use revoke_id option.");
                                                                                }
                                                                                }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 276 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 277 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 279 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 287 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ihtOptionalDataInfo.push_back(std::pair<std::string, uint32_t>((yysemantic_stack_[(5) - (1)].string), (yysemantic_stack_[(5) - (5)].number))); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 294 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 296 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 298 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 55:
/* Line 670 of lalr1.cc  */
#line 302 "parser/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 56:
/* Line 670 of lalr1.cc  */
#line 303 "parser/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 304 "parser/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); 
                                                                                  options.SetDl9Series((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 306 "parser/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 59:
/* Line 670 of lalr1.cc  */
#line 307 "parser/bif.y"
    { currentBifOptions->SetPKFileName((yysemantic_stack_[(3) - (3)].string)); 
                                                                                  currentBifOptions->AddFiles(BIF::BisonParser::token::PK_FILE, (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 309 "parser/bif.y"
    { currentBifOptions->SetSKFileName((yysemantic_stack_[(3) - (3)].string)); 
                                                                                  currentBifOptions->AddFiles(BIF::BisonParser::token::SK_FILE, (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 311 "parser/bif.y"
    { currentBifOptions->SetPKFileName1((yysemantic_stack_[(3) - (3)].string)); 
                                                                                  currentBifOptions->AddFiles(BIF::BisonParser::token::PK_FILE1, (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 313 "parser/bif.y"
    { currentBifOptions->SetSKFileName1((yysemantic_stack_[(3) - (3)].string)); 
                                                                                  currentBifOptions->AddFiles(BIF::BisonParser::token::SK_FILE1, (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 315 "parser/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 316 "parser/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 317 "parser/bif.y"
    { currentBifOptions->SetPdiType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 318 "parser/bif.y"
    { currentBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number));}
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 325 "parser/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 326 "parser/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions);
                                                                                  if((options.archType == Arch::SPARTANUP) && (currentBifOptions->imageBifOptionList.size() > 1))
                                                                                  {
                                                                                      LOG_ERROR("'-arch spartanup' cannot accomodate more than one Sub-System.");
                                                                                  }
                                                                                }
    break;

  case 78:
/* Line 670 of lalr1.cc  */
#line 345 "parser/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 79:
/* Line 670 of lalr1.cc  */
#line 346 "parser/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 80:
/* Line 670 of lalr1.cc  */
#line 347 "parser/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 81:
/* Line 670 of lalr1.cc  */
#line 348 "parser/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 82:
/* Line 670 of lalr1.cc  */
#line 349 "parser/bif.y"
    { LOG_ERROR("BIF attribute error !!!\n\t This usage of 'init' is not supported. See 'bootgen -bif_help init' for usage details."); }
    break;

  case 83:
/* Line 670 of lalr1.cc  */
#line 350 "parser/bif.y"
    { LOG_ERROR("Copy to Memory feature with the attribute 'copy' is no more supported.\n\t   This can be duplicated with the option 'imagestore'. Please refer UG1283 for more details.");
                                                                                  currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 84:
/* Line 670 of lalr1.cc  */
#line 352 "parser/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 85:
/* Line 670 of lalr1.cc  */
#line 353 "parser/bif.y"
    { if ((yysemantic_stack_[(3) - (3)].number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("uid value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 86:
/* Line 670 of lalr1.cc  */
#line 358 "parser/bif.y"
    { if ((yysemantic_stack_[(3) - (3)].number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("parent_uid value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 87:
/* Line 670 of lalr1.cc  */
#line 363 "parser/bif.y"
    { if ((yysemantic_stack_[(3) - (3)].number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("function_id value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 88:
/* Line 670 of lalr1.cc  */
#line 368 "parser/bif.y"
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

  case 89:
/* Line 670 of lalr1.cc  */
#line 389 "parser/bif.y"
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

  case 95:
/* Line 670 of lalr1.cc  */
#line 409 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQ || options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported for the specified architecture.\n\t   Refer 'bootgen -bif_help' for more details");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 96:
/* Line 670 of lalr1.cc  */
#line 412 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 417 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                  if(options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); 
                                                                                  currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 422 "parser/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 437 "parser/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 438 "parser/bif.y"
    { currentBifOptions->SetBootDevice(BootDevice::IMAGESTORE); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 439 "parser/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 452 "parser/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 453 "parser/bif.y"
    { currentBifOptions->SetAuthJtagSPKRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 454 "parser/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 455 "parser/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 456 "parser/bif.y"
    { currentBifOptions->SetAuthJtagSignatureFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 459 "parser/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 462 "parser/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 464 "parser/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 129:
/* Line 670 of lalr1.cc  */
#line 466 "parser/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 130:
/* Line 670 of lalr1.cc  */
#line 468 "parser/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 470 "parser/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 472 "parser/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 133:
/* Line 670 of lalr1.cc  */
#line 474 "parser/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 134:
/* Line 670 of lalr1.cc  */
#line 476 "parser/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 135:
/* Line 670 of lalr1.cc  */
#line 478 "parser/bif.y"
    {  currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));}
    break;

  case 136:
/* Line 670 of lalr1.cc  */
#line 480 "parser/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->SetSmapWidth((yysemantic_stack_[(3) - (3)].number));
                                                                                }
    break;

  case 137:
/* Line 670 of lalr1.cc  */
#line 484 "parser/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 138:
/* Line 670 of lalr1.cc  */
#line 485 "parser/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 486 "parser/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 487 "parser/bif.y"
    { currentBifOptions->SetPufRingOscilltorSwapConfigValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 488 "parser/bif.y"
    { currentBifOptions->SetDiceEnable(); }
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 491 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(),options.IsVersalNetSeries(),  currentBifOptions->GetIdCode()); }
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 494 "parser/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 498 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 504 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode()); }
    break;

  case 153:
/* Line 670 of lalr1.cc  */
#line 518 "parser/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 521 "parser/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 522 "parser/bif.y"
    { currentPartitionBifOptions->imageStoreId = (yysemantic_stack_[(3) - (3)].number);
                                                                                  currentPartitionBifOptions->SetPartitionType(PartitionType::IMAGE_STORE_PDI); }
    break;

  case 157:
/* Line 670 of lalr1.cc  */
#line 525 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 526 "parser/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string), currentImageBifOptions->GetImageType());
                                                                                  currentPartitionBifOptions->filelist.push_back(currentPartitionBifOptions->filename);
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 555 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] is supported only for ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 560 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 562 "parser/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 563 "parser/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 564 "parser/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 565 "parser/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 566 "parser/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 567 "parser/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 568 "parser/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 569 "parser/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 570 "parser/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 571 "parser/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 574 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 575 "parser/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 576 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 577 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 578 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 189:
/* Line 670 of lalr1.cc  */
#line 579 "parser/bif.y"
    { currentPartitionBifOptions->SetLockStepFlag();}
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 582 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 191:
/* Line 670 of lalr1.cc  */
#line 583 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 192:
/* Line 670 of lalr1.cc  */
#line 586 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 193:
/* Line 670 of lalr1.cc  */
#line 587 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 194:
/* Line 670 of lalr1.cc  */
#line 588 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 195:
/* Line 670 of lalr1.cc  */
#line 590 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 196:
/* Line 670 of lalr1.cc  */
#line 591 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 197:
/* Line 670 of lalr1.cc  */
#line 592 "parser/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 198:
/* Line 670 of lalr1.cc  */
#line 593 "parser/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 594 "parser/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 200:
/* Line 670 of lalr1.cc  */
#line 595 "parser/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 201:
/* Line 670 of lalr1.cc  */
#line 596 "parser/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 202:
/* Line 670 of lalr1.cc  */
#line 597 "parser/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 203:
/* Line 670 of lalr1.cc  */
#line 598 "parser/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 204:
/* Line 670 of lalr1.cc  */
#line 599 "parser/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 205:
/* Line 670 of lalr1.cc  */
#line 600 "parser/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 206:
/* Line 670 of lalr1.cc  */
#line 601 "parser/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 207:
/* Line 670 of lalr1.cc  */
#line 602 "parser/bif.y"
    { currentPartitionBifOptions->ppkFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 208:
/* Line 670 of lalr1.cc  */
#line 603 "parser/bif.y"
    { currentPartitionBifOptions->pskFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 209:
/* Line 670 of lalr1.cc  */
#line 604 "parser/bif.y"
    { currentPartitionBifOptions->spkFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 210:
/* Line 670 of lalr1.cc  */
#line 605 "parser/bif.y"
    { currentPartitionBifOptions->sskFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 211:
/* Line 670 of lalr1.cc  */
#line 606 "parser/bif.y"
    { currentPartitionBifOptions->pkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 212:
/* Line 670 of lalr1.cc  */
#line 607 "parser/bif.y"
    { currentPartitionBifOptions->skFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 213:
/* Line 670 of lalr1.cc  */
#line 608 "parser/bif.y"
    { currentPartitionBifOptions->pkFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 214:
/* Line 670 of lalr1.cc  */
#line 609 "parser/bif.y"
    { currentPartitionBifOptions->skFile1 = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 215:
/* Line 670 of lalr1.cc  */
#line 610 "parser/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 216:
/* Line 670 of lalr1.cc  */
#line 611 "parser/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 217:
/* Line 670 of lalr1.cc  */
#line 612 "parser/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 219:
/* Line 670 of lalr1.cc  */
#line 614 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 220:
/* Line 670 of lalr1.cc  */
#line 615 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 221:
/* Line 670 of lalr1.cc  */
#line 616 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 222:
/* Line 670 of lalr1.cc  */
#line 617 "parser/bif.y"
    { currentPartitionBifOptions->SetSPKRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 223:
/* Line 670 of lalr1.cc  */
#line 618 "parser/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 224:
/* Line 670 of lalr1.cc  */
#line 619 "parser/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 225:
/* Line 670 of lalr1.cc  */
#line 620 "parser/bif.y"
    { currentPartitionBifOptions->SetClusterNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 226:
/* Line 670 of lalr1.cc  */
#line 621 "parser/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 227:
/* Line 670 of lalr1.cc  */
#line 622 "parser/bif.y"
    { currentPartitionBifOptions->SetDelayAuth(true); }
    break;

  case 228:
/* Line 670 of lalr1.cc  */
#line 623 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBootFlag(); }
    break;

  case 238:
/* Line 670 of lalr1.cc  */
#line 637 "parser/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 240:
/* Line 670 of lalr1.cc  */
#line 639 "parser/bif.y"
    { currentPartitionBifOptions->SetHybridAuthType((yysemantic_stack_[(1) - (1)].string)); (yyval.authvalue_t) = ::Authentication::RSA; }
    break;

  case 241:
/* Line 670 of lalr1.cc  */
#line 642 "parser/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 243:
/* Line 670 of lalr1.cc  */
#line 646 "parser/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 260:
/* Line 670 of lalr1.cc  */
#line 695 "parser/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 261:
/* Line 670 of lalr1.cc  */
#line 696 "parser/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 262:
/* Line 670 of lalr1.cc  */
#line 697 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), false); }
    break;

  case 263:
/* Line 670 of lalr1.cc  */
#line 698 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), true); }
    break;

  case 264:
/* Line 670 of lalr1.cc  */
#line 699 "parser/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 265:
/* Line 670 of lalr1.cc  */
#line 700 "parser/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 266:
/* Line 670 of lalr1.cc  */
#line 701 "parser/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 267:
/* Line 670 of lalr1.cc  */
#line 702 "parser/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 268:
/* Line 670 of lalr1.cc  */
#line 703 "parser/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 269:
/* Line 670 of lalr1.cc  */
#line 704 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmARegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 270:
/* Line 670 of lalr1.cc  */
#line 705 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 271:
/* Line 670 of lalr1.cc  */
#line 706 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmCRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 272:
/* Line 670 of lalr1.cc  */
#line 709 "parser/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 273:
/* Line 670 of lalr1.cc  */
#line 710 "parser/bif.y"
    { currentPartitionBifOptions->acFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 274:
/* Line 670 of lalr1.cc  */
#line 711 "parser/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 299:
/* Line 670 of lalr1.cc  */
#line 744 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 300:
/* Line 670 of lalr1.cc  */
#line 747 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 301:
/* Line 670 of lalr1.cc  */
#line 750 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 302:
/* Line 670 of lalr1.cc  */
#line 753 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 311:
/* Line 670 of lalr1.cc  */
#line 774 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP || options.GetArchType() == Arch::VERSAL)
                                                                   LOG_ERROR("BIF attribute error !!!\n\t\t'lms_key_params' is not supported with the mentioned -arch");
                                                                 lmsParams.lms_param = (yysemantic_stack_[(1) - (1)].string);
                                                                 currentBifOptions->SetLmsParameters(lmsParams) ;
                                                               }
    break;

  case 313:
/* Line 670 of lalr1.cc  */
#line 781 "parser/bif.y"
    { lmsParams.type = (yysemantic_stack_[(2) - (1)].authkeylevel_t);}
    break;

  case 316:
/* Line 670 of lalr1.cc  */
#line 785 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 320:
/* Line 670 of lalr1.cc  */
#line 795 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 321:
/* Line 670 of lalr1.cc  */
#line 798 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 326:
/* Line 670 of lalr1.cc  */
#line 810 "parser/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 328:
/* Line 670 of lalr1.cc  */
#line 817 "parser/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 329:
/* Line 670 of lalr1.cc  */
#line 818 "parser/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 331:
/* Line 670 of lalr1.cc  */
#line 823 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 332:
/* Line 670 of lalr1.cc  */
#line 824 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 333:
/* Line 670 of lalr1.cc  */
#line 825 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 335:
/* Line 670 of lalr1.cc  */
#line 829 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 336:
/* Line 670 of lalr1.cc  */
#line 830 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 338:
/* Line 670 of lalr1.cc  */
#line 834 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 339:
/* Line 670 of lalr1.cc  */
#line 835 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 341:
/* Line 670 of lalr1.cc  */
#line 839 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 343:
/* Line 670 of lalr1.cc  */
#line 843 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 345:
/* Line 670 of lalr1.cc  */
#line 848 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1758 "bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -513;
  const short int
  BisonParser::yypact_[] =
  {
      -513,    26,   -70,  -513,    33,  -513,  -513,     1,    53,  -513,
    -513,  -513,    73,  -513,  1042,  -513,  -513,   148,    64,    84,
      83,    94,    96,   116,    89,   118,  -513,   138,   165,   166,
     173,   182,   196,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,   214,   219,   228,   232,  -513,  -513,  -513,
    -513,  -513,   105,  -513,  -513,    15,  -513,  -513,  -513,   236,
    -513,  -513,  -513,   730,   241,   250,   258,  -513,   261,   262,
     886,   122,    31,   126,    86,    86,    86,    86,  -513,    86,
      15,     1,  -513,   135,   389,   269,    32,     1,     1,     1,
       1,  -513,  -513,  -513,  -513,     1,  -513,  -513,  -513,  -513,
    -513,   271,   286,   287,   291,   297,   298,   300,   303,   309,
     310,   311,   314,   318,   320,   323,   325,  -513,  -513,   326,
     327,   328,   345,  -513,  -513,  -513,  -513,  -513,  -513,   346,
     347,   352,   353,   354,   355,   356,   357,   358,   370,  -513,
     372,  -513,   373,   374,   375,   376,  -513,  -513,  -513,  -513,
    -513,  -513,   377,   378,   379,   380,   381,   388,   390,   391,
     392,   393,   395,   396,   398,   399,  -513,  -513,  -513,   351,
      17,   574,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,   126,    86,   135,   401,   151,    67,   400,   402,   404,
    -513,   408,  -513,  -513,   411,   414,  -513,  -513,  -513,  -513,
      86,    86,    86,  -513,  -513,   -44,   -41,    24,   252,   254,
     257,   257,   257,   257,  -513,   257,   425,   421,   426,   936,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,   427,   428,   429,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,   432,   124,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,   431,   433,
     434,   435,   436,   439,    75,  -513,   438,   440,   442,   446,
     441,   423,   192,     4,  -513,  -513,  -513,  -513,  -513,   -34,
     -68,   -75,   299,  1074,    86,   276,   289,   290,   288,    86,
      86,    86,    86,    86,    86,    86,   293,    86,    86,    86,
      86,     1,     1,   371,     1,    86,    86,    86,    86,     1,
      86,    86,    86,   135,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,  -513,   730,
    -513,   730,  -513,  -513,  -513,   458,   257,  -513,   389,   459,
     460,   461,  -513,   455,   462,   463,   465,   122,     1,   886,
      86,  -513,    31,  -513,  -513,   -92,    86,    86,    86,    86,
      86,    86,    86,    86,    86,    86,   177,  -513,     1,   397,
     468,   469,   470,   471,   472,   474,   490,   491,   492,   493,
     497,   499,   500,   508,   511,   513,   515,   516,   517,   518,
     519,   520,  -513,  -513,   467,   349,    86,    86,    86,  -513,
     389,  -513,    86,    86,   437,    86,     1,  -513,   269,  -513,
     359,    86,    86,  -513,  -513,   111,  -513,   443,  -513,   360,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,   257,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,   257,   257,   257,
     257,   257,   257,   257,  -513,  -513,   257,   257,  -513,   522,
    -158,  -513,   257,  -513,  -513,  -513,  -513,   257,   257,   257,
     257,  -513,   257,   257,   257,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,    86,  -513,    86,    86,   293,   151,   361,   174,
      67,  -513,  -513,  -513,   257,  -513,  -513,  -513,  -513,  -513,
     -44,   -44,   -41,   -41,    24,   252,   254,  -513,   526,   527,
     529,   530,   531,   532,   533,   534,  -513,  -513,   535,  -513,
    -513,   344,  -513,  -513,  -513,   536,   -34,   -68,   -75,    86,
      86,    86,     1,   135,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,  -513,   936,
    -513,   257,   257,   257,  -513,   257,   257,  -513,   257,  -513,
    -513,  -513,  -513,   257,   257,   443,  -513,   545,    86,   193,
    -513,   257,   257,  -513,   455,  -513,  -513,  -513,  -513,  -513,
     465,   122,    86,    86,    86,    86,    86,    86,   464,    86,
     222,  -513,    86,  -513,  -513,  -513,   257,   257,  -513,   542,
    -128,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,   240,  -513,
     369,   -86,  -513,   257,   257,   257,   257,   257,   257,  -513,
     257,  -513,   257,    86,   211,  -513,  -513,  -513,  -513,   417,
     -85,  -513,  -513
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,     6,     4,     0,     0,   322,
     323,     5,     0,     8,     0,   145,     7,   142,     0,     0,
       0,     0,     0,     0,     0,     0,    92,     0,     0,     0,
       0,     0,     0,   281,   280,   232,   231,   233,   234,   235,
     236,   237,   229,   275,   283,   284,   285,   286,   287,   288,
     289,   290,   291,     0,     0,     0,     0,   277,   278,   279,
      12,    13,    11,    67,    15,    91,    10,     9,    14,     0,
     230,   276,   144,     0,     0,     0,     0,    96,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    69,     0,
      90,    49,    16,     0,     0,     0,     0,     0,     0,     0,
       0,    68,   142,    93,    94,     0,   174,   183,   181,   182,
     189,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     190,     0,     0,     0,     0,     0,     0,   266,   267,     0,
       0,     0,     0,   175,   176,   177,   178,   179,   180,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   227,
       0,   228,     0,     0,     0,     0,   282,   297,   185,   298,
     184,   296,   275,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,   295,   277,   246,   226,   223,     0,
     148,   147,   165,   161,   218,   162,   188,   163,   164,   186,
     187,     0,     0,     0,     0,     0,     0,     0,     0,   159,
      65,     0,   113,   256,     0,   110,   112,   101,   325,   324,
       0,     0,     0,   327,   330,   334,   337,   340,   342,   344,
      66,    56,    57,    58,    71,    55,     0,    46,     0,    21,
     247,    64,   141,   137,   138,   139,   133,     0,     0,     0,
     248,   249,   251,   252,   253,   254,   255,   250,     0,   115,
     126,   127,   135,   128,   129,   130,   132,   131,     0,     0,
       0,     0,     0,     0,   118,   311,     0,     0,     0,     0,
       0,   305,     0,     0,    59,    60,    61,    62,    63,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   146,     0,
     151,     0,   152,   100,   104,   170,   172,    95,     0,     0,
       0,     0,   302,   102,     0,     0,   103,     0,     0,     0,
       0,    99,     0,   328,   329,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    92,   106,    49,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    45,    43,     0,    18,     0,     0,     0,    98,
       0,   117,     0,     0,     0,     0,     0,   105,     0,   120,
       0,     0,     0,   313,   107,     0,   108,     0,   109,   315,
     238,   239,   240,   195,   241,   242,   196,   243,   244,   197,
     245,   198,   157,   292,   293,   294,   295,   156,   188,   268,
     258,   200,   257,   199,   259,   201,   191,   260,   261,   262,
     263,   264,   265,   216,   304,   215,   221,   222,   166,   168,
     192,   167,   173,   272,   273,   158,   274,   224,   225,   155,
     154,   153,   269,   270,   271,   220,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   217,
     149,   150,     0,    97,     0,     0,     0,     0,     0,     0,
       0,   219,   143,   160,   114,   111,   326,   331,   332,   333,
     335,   336,   338,   339,   341,   343,   345,    70,     0,     0,
       0,     0,     0,     0,     0,     0,    80,    81,     0,    82,
      72,    75,    74,    73,    47,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,    21,
      20,   140,   134,   136,   116,   121,   122,   123,   124,   125,
     119,   310,   307,   309,   308,     0,   306,   312,     0,     0,
     171,   299,   301,   300,   303,   319,   316,   320,   321,   317,
     318,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    77,     0,    25,    22,    42,    40,    41,    44,    50,
      52,    39,    23,    24,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    19,     0,   169,
       0,     0,    84,    88,    89,    85,    86,    87,    78,    79,
      83,    76,    48,     0,     0,   314,   194,   193,    51,     0,
       0,    54,    53
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -513,  -513,  -513,  -513,  -513,  -513,  -513,  -513,  -386,  -513,
     185,  -513,   -89,  -513,  -513,  -513,   495,  -513,  -513,  -512,
    -513,   195,   538,  -513,  -513,   206,  -513,  -235,  -241,  -513,
    -513,   -10,  -513,   548,  -513,  -165,  -513,   231,   -74,  -513,
       3,    90,  -513,  -513,   301,  -513,  -513,  -513,  -513,    47,
      48,    59,  -513,   -71,  -186,  -513,  -513,  -513,  -513,  -513,
    -513,  -513,  -513,   -66,  -513,  -513,  -513,  -513,  -513,   597,
    -513,  -513,   117,   121,  -513,   203,  -513,  -405,   537,  -513,
     127,  -513,  -513,    21,  -513,  -127,  -101,   -97,   273,   268,
     272,   -84
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     6,     8,    14,    60,   229,   394,   395,
     226,   227,   608,   609,    61,    62,    63,   224,   366,   530,
     531,    64,    65,    66,   194,   204,   205,   248,   263,   264,
     249,   103,    80,   104,    73,   179,   180,   198,   181,   182,
     458,   334,   335,   461,   183,   184,   459,   185,    69,   423,
     426,   429,   431,   186,   231,   250,   251,   252,   253,   254,
     255,   256,   257,   206,   443,   441,   445,   187,   188,   189,
      71,   190,   343,   455,   270,   271,   572,   272,   419,   575,
     346,   586,   589,    72,   213,   214,   215,   216,   217,   218,
     219,   336
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       220,   221,   222,   223,    67,   225,   199,   337,   418,   560,
     200,     4,   577,   365,   401,   330,   332,   207,    15,   601,
     427,     5,   329,   409,   579,   102,     3,   424,    11,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
       7,   117,   118,   365,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   644,   129,   130,   201,   131,   132,
      12,   420,   133,   134,   135,   136,   137,   138,   139,   140,
      81,   141,   142,   143,   144,   428,    13,   145,   146,   365,
     408,   425,   147,   353,   354,   365,   365,    82,   641,    84,
      83,   506,    88,   148,     9,    10,   202,   647,   652,   149,
      85,   150,    86,   493,   151,   152,   153,   154,     9,    10,
     344,   345,   228,   421,   422,   155,   258,   259,   274,   275,
     276,   277,    87,   265,    89,   333,   278,   475,   355,   400,
     356,   357,   358,    33,    34,   156,   359,   360,   266,   267,
     268,    91,   157,   158,   159,   160,   161,   162,    44,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,    58,    59,   490,   564,   491,   570,    92,   176,
     628,    74,    93,   627,   269,   177,    94,   260,   261,   262,
      24,   517,   208,   209,   178,    95,   232,   102,   339,   340,
     341,   342,   203,   233,   234,   235,   416,   417,   518,    96,
     439,    75,   269,   361,   362,   447,   448,   449,   450,   451,
     452,   453,   438,   456,   457,   460,   462,   266,   267,   268,
      97,   467,   468,   469,   470,    98,   472,   473,   474,   507,
     508,   509,   587,   588,    99,   236,   237,   238,   100,   239,
     519,   520,   105,   518,   645,   417,    76,    77,    78,   521,
     522,   523,   191,   524,   525,   526,   527,   528,   510,   511,
      26,   192,    79,   210,   512,   513,   504,   211,   212,   193,
       9,    10,   195,   196,   176,   199,   501,   279,   240,   241,
     242,   243,   244,   245,   246,   519,   520,   203,   230,   208,
     209,   247,   280,   281,   521,   522,   523,   282,   524,   525,
     526,   527,   528,   283,   284,   529,   285,   208,   209,   286,
     258,   259,   561,   562,   563,   287,   288,   289,   565,   566,
     290,   568,   463,   464,   291,   466,   292,   573,   574,   293,
     471,   294,   295,   296,   297,   476,   477,   478,   479,   480,
     481,   482,   483,   484,   485,   486,   487,   488,   489,   600,
     529,   298,   299,   300,   559,   328,   533,   612,   301,   302,
     303,   304,   305,   306,   307,   518,   370,   371,   372,   502,
     210,   260,   261,   262,   211,   212,   308,   630,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   318,   210,   228,
     373,   374,   211,   212,   319,   649,   320,   321,   322,   323,
     375,   324,   325,   376,   326,   327,   347,   519,   520,   349,
     581,   582,   338,   348,   350,   351,   521,   522,   523,   352,
     524,   525,   526,   527,   528,   363,   364,   569,   365,   367,
     368,   369,   415,   396,   397,   398,   399,   402,   440,   403,
     404,   405,   406,   407,   410,   414,   411,   377,   412,   413,
     430,   232,   442,   446,   444,   606,   607,   610,   233,   234,
     235,   454,   465,   492,   497,   494,   495,   496,   498,   499,
     500,   558,   529,   535,   536,   537,   538,   539,   540,   378,
     541,   379,   380,   381,   382,   383,   384,   385,   386,   387,
     388,   389,   390,   391,   460,   631,   542,   543,   544,   545,
     236,   237,   238,   546,   239,   547,   548,   392,   633,   634,
     635,   636,   637,   638,   549,   640,   393,   550,   642,   551,
     632,   552,   553,   554,   555,   556,   557,   585,   571,   567,
     269,   578,   591,   592,   265,   593,   594,   595,   596,   597,
     598,   599,   602,   240,   241,   242,   243,   244,   245,   246,
     417,   643,   646,   534,   648,   639,   247,   101,   505,   610,
     650,   532,    68,   611,    90,   613,   614,   615,   616,   617,
     618,   619,   620,   621,   622,   623,   624,   625,   626,   331,
     503,   629,   580,   603,   437,   604,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   605,   117,   118,
     651,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,    70,   129,   130,   584,   131,   132,   583,   576,   133,
     134,   135,   136,   137,   138,   139,   140,   590,   141,   142,
     143,   144,   515,   273,   145,   146,   514,   516,     0,   147,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     148,     0,     0,     0,     0,     0,   149,     0,   150,     0,
       0,   151,   152,   153,   154,     0,     0,     0,     0,     0,
       0,     0,   155,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    34,   156,     0,     0,     0,     0,     0,     0,   157,
     158,   159,   160,   161,   162,    44,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,    58,
      59,     0,     0,     0,     0,     0,   176,     0,     0,     0,
       0,     0,   177,     0,     0,     0,     0,     0,     0,     0,
       0,   178,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,     0,   117,   118,     0,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,     0,   129,   130,
       0,   131,   132,     0,     0,   133,   134,   135,   136,   137,
     138,   139,   140,     0,   141,   142,   143,   144,     0,     0,
     145,   146,     0,     0,     0,   147,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   148,     0,     0,     0,
       0,     0,   149,     0,   150,     0,     0,   151,   152,   153,
     154,     0,     0,     0,     0,     0,     0,     0,   155,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,    34,   156,     0,
       0,     0,     0,     0,     0,   157,   158,   159,   160,   161,
     162,    44,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,    58,    59,     0,     0,     0,
       0,     0,   176,     0,     0,     0,     0,     0,   177,     0,
       0,     0,     0,     0,     0,     0,     0,   178,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   197,   116,     0,
     117,   118,     0,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,     0,   129,   130,     0,   131,   132,     0,
       0,   133,   134,   135,   136,   137,   138,   139,   140,     0,
     141,   142,     0,   144,     0,     0,   145,   146,     0,     0,
       0,     0,     0,   370,   371,   372,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   149,     0,
       0,     0,     0,   151,   152,   153,   154,   373,   374,     0,
       0,     0,     0,     0,   155,     0,     0,   375,     0,     0,
     376,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    33,    34,   156,     0,     0,     0,     0,     0,
       0,   157,   158,   159,   160,   161,   162,    44,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,    58,    59,     0,   377,     0,     0,     0,   176,     0,
       0,     0,     0,     0,   177,    15,    16,     0,     0,     0,
       0,     0,    17,   178,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    18,     0,    19,   378,     0,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,   389,   390,
     391,     0,     0,    20,     0,     0,   106,   107,   108,   109,
     110,     0,     0,     0,   392,     0,     0,     0,     0,     0,
       0,     0,     0,   393,     0,     0,     0,     0,    21,    22,
      23,     0,     0,     0,     0,     0,     0,    24,    25,   133,
     134,   135,   136,   137,   138,    26,     0,    27,    28,     0,
       0,     0,     0,     0,     0,     9,    10,     0,     0,     0,
      29,    30,     0,    31,     0,     0,     0,    32,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    33,    34,
      35,    36,    37,    38,    39,    40,    41,     0,     0,     0,
      42,     0,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,     0,
      33,    34,   156,     0,     0,     0,     0,     0,     0,   432,
     158,   159,   160,   161,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,   433,   434,   435,   436,    57,    58,
      59,     0,     0,     0,     0,     0,   176
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        84,    85,    86,    87,    14,    89,    80,   193,     4,   395,
      81,    81,   417,   171,   249,   180,   181,    83,     3,   531,
      95,    91,     5,   264,   182,    10,     0,    95,     7,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
       7,    24,    25,   171,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,   182,    38,    39,    26,    41,    42,
       7,    95,    45,    46,    47,    48,    49,    50,    51,    52,
       6,    54,    55,    56,    57,   150,     3,    60,    61,   171,
       5,   149,    65,   210,   211,   171,   171,     3,   600,     6,
       6,   183,     3,    76,    93,    94,    65,   183,   183,    82,
       6,    84,     6,   338,    87,    88,    89,    90,    93,    94,
      43,    44,    91,   147,   148,    98,    41,    42,    97,    98,
      99,   100,     6,    91,     6,   191,   105,   313,   212,     5,
     174,   175,   176,   116,   117,   118,   177,   178,   106,   107,
     108,     3,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   329,   400,   331,   408,     3,   152,
     575,    23,     6,   559,   170,   158,     3,   102,   103,   104,
      75,     4,    96,    97,   167,     3,    62,    10,    37,    38,
      39,    40,   161,    69,    70,    71,     4,     5,    21,     3,
     284,    53,   170,   179,   180,   289,   290,   291,   292,   293,
     294,   295,   283,   297,   298,   299,   300,   106,   107,   108,
       6,   305,   306,   307,   308,     6,   310,   311,   312,   356,
     357,   358,    58,    59,     6,   111,   112,   113,     6,   115,
      63,    64,     6,    21,     4,     5,    98,    99,   100,    72,
      73,    74,    11,    76,    77,    78,    79,    80,   359,   360,
      83,    11,   114,   177,   361,   362,   350,   181,   182,    11,
      93,    94,    11,    11,   152,   349,   347,     6,   154,   155,
     156,   157,   158,   159,   160,    63,    64,   161,   153,    96,
      97,   167,     6,     6,    72,    73,    74,     6,    76,    77,
      78,    79,    80,     6,     6,   128,     6,    96,    97,     6,
      41,    42,   396,   397,   398,     6,     6,     6,   402,   403,
       6,   405,   301,   302,     6,   304,     6,   411,   412,     6,
     309,     6,     6,     6,     6,   314,   315,   316,   317,   318,
     319,   320,   321,   322,   323,   324,   325,   326,   327,     5,
     128,     6,     6,     6,     5,     4,   366,   543,     6,     6,
       6,     6,     6,     6,     6,    21,    17,    18,    19,   348,
     177,   102,   103,   104,   181,   182,     6,   184,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   177,   368,
      41,    42,   181,   182,     6,   184,     6,     6,     6,     6,
      51,     6,     6,    54,     6,     6,     6,    63,    64,     5,
     494,   495,    11,    11,     6,     4,    72,    73,    74,     5,
      76,    77,    78,    79,    80,   173,   172,   406,   171,     4,
       9,     5,     9,     6,     6,     6,     4,     6,   162,     6,
       6,     6,     6,     4,     6,     4,     6,    98,     6,     3,
     151,    62,   163,   165,   164,   539,   540,   541,    69,    70,
      71,   168,    91,     5,     9,     6,     6,     6,     6,     6,
       5,     4,   128,    76,     6,     6,     6,     6,     6,   130,
       6,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   578,   579,     6,     6,     6,     6,
     111,   112,   113,     6,   115,     6,     6,   158,   592,   593,
     594,   595,   596,   597,     6,   599,   167,     6,   602,     6,
     591,     6,     6,     6,     6,     6,     6,   166,   169,    92,
     170,     9,     6,     6,    91,     6,     6,     6,     6,     6,
       6,     6,     6,   154,   155,   156,   157,   158,   159,   160,
       5,     9,   183,   368,   643,    91,   167,    62,   352,   643,
     644,   366,    14,   542,    26,   544,   545,   546,   547,   548,
     549,   550,   551,   552,   553,   554,   555,   556,   557,     5,
     349,   578,   492,   536,   283,   537,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,   538,    24,    25,
     183,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    14,    38,    39,   497,    41,    42,   496,   415,    45,
      46,    47,    48,    49,    50,    51,    52,   500,    54,    55,
      56,    57,   364,    96,    60,    61,   363,   365,    -1,    65,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      76,    -1,    -1,    -1,    -1,    -1,    82,    -1,    84,    -1,
      -1,    87,    88,    89,    90,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     116,   117,   118,    -1,    -1,    -1,    -1,    -1,    -1,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,    -1,    -1,    -1,    -1,    -1,   152,    -1,    -1,    -1,
      -1,    -1,   158,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   167,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    -1,    24,    25,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    -1,    38,    39,
      -1,    41,    42,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    -1,    54,    55,    56,    57,    -1,    -1,
      60,    61,    -1,    -1,    -1,    65,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,    -1,
      -1,    -1,    82,    -1,    84,    -1,    -1,    87,    88,    89,
      90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   116,   117,   118,    -1,
      -1,    -1,    -1,    -1,    -1,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,    -1,    -1,    -1,
      -1,    -1,   152,    -1,    -1,    -1,    -1,    -1,   158,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   167,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    -1,
      24,    25,    -1,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    -1,    38,    39,    -1,    41,    42,    -1,
      -1,    45,    46,    47,    48,    49,    50,    51,    52,    -1,
      54,    55,    -1,    57,    -1,    -1,    60,    61,    -1,    -1,
      -1,    -1,    -1,    17,    18,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,
      -1,    -1,    -1,    87,    88,    89,    90,    41,    42,    -1,
      -1,    -1,    -1,    -1,    98,    -1,    -1,    51,    -1,    -1,
      54,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   116,   117,   118,    -1,    -1,    -1,    -1,    -1,
      -1,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,    -1,    98,    -1,    -1,    -1,   152,    -1,
      -1,    -1,    -1,    -1,   158,     3,     4,    -1,    -1,    -1,
      -1,    -1,    10,   167,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    21,    -1,    23,   130,    -1,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,    -1,    -1,    41,    -1,    -1,    12,    13,    14,    15,
      16,    -1,    -1,    -1,   158,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   167,    -1,    -1,    -1,    -1,    66,    67,
      68,    -1,    -1,    -1,    -1,    -1,    -1,    75,    76,    45,
      46,    47,    48,    49,    50,    83,    -1,    85,    86,    -1,
      -1,    -1,    -1,    -1,    -1,    93,    94,    -1,    -1,    -1,
      98,    99,    -1,   101,    -1,    -1,    -1,   105,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   116,   117,
     118,   119,   120,   121,   122,   123,   124,    -1,    -1,    -1,
     128,    -1,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,    -1,
     116,   117,   118,    -1,    -1,    -1,    -1,    -1,    -1,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,    -1,    -1,    -1,    -1,    -1,   152
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned short int
  BisonParser::yystos_[] =
  {
         0,   186,   187,     0,    81,    91,   188,     7,   189,    93,
      94,   268,     7,     3,   190,     3,     4,    10,    21,    23,
      41,    66,    67,    68,    75,    76,    83,    85,    86,    98,
      99,   101,   105,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   128,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     191,   199,   200,   201,   206,   207,   208,   216,   218,   233,
     254,   255,   268,   219,    23,    53,    98,    99,   100,   114,
     217,     6,     3,     6,     6,     6,     6,     6,     3,     6,
     207,     3,     3,     6,     3,     3,     3,     6,     6,     6,
       6,   201,    10,   216,   218,     6,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    24,    25,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    38,
      39,    41,    42,    45,    46,    47,    48,    49,    50,    51,
      52,    54,    55,    56,    57,    60,    61,    65,    76,    82,
      84,    87,    88,    89,    90,    98,   118,   125,   126,   127,
     128,   129,   130,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   152,   158,   167,   220,
     221,   223,   224,   229,   230,   232,   238,   252,   253,   254,
     256,    11,    11,    11,   209,    11,    11,    21,   222,   223,
     238,    26,    65,   161,   210,   211,   248,   248,    96,    97,
     177,   181,   182,   269,   270,   271,   272,   273,   274,   275,
     276,   276,   276,   276,   202,   276,   195,   196,   268,   192,
     153,   239,    62,    69,    70,    71,   111,   112,   113,   115,
     154,   155,   156,   157,   158,   159,   160,   167,   212,   215,
     240,   241,   242,   243,   244,   245,   246,   247,    41,    42,
     102,   103,   104,   213,   214,    91,   106,   107,   108,   170,
     259,   260,   262,   263,   268,   268,   268,   268,   268,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     4,     5,
     220,     5,   220,   248,   226,   227,   276,   239,    11,    37,
      38,    39,    40,   257,    43,    44,   265,     6,    11,     5,
       6,     4,     5,   270,   270,   276,   174,   175,   176,   177,
     178,   179,   180,   173,   172,   171,   203,     4,     9,     5,
      17,    18,    19,    41,    42,    51,    54,    98,   130,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   158,   167,   193,   194,     6,     6,     6,     4,
       5,   212,     6,     6,     6,     6,     6,     4,     5,   213,
       6,     6,     6,     3,     4,     9,     4,     5,     4,   263,
      95,   147,   148,   234,    95,   149,   235,    95,   150,   236,
     151,   237,   125,   140,   141,   142,   143,   229,   238,   276,
     162,   250,   163,   249,   164,   251,   165,   276,   276,   276,
     276,   276,   276,   276,   168,   258,   276,   276,   225,   231,
     276,   228,   276,   268,   268,    91,   268,   276,   276,   276,
     276,   268,   276,   276,   276,   239,   268,   268,   268,   268,
     268,   268,   268,   268,   268,   268,   268,   268,   268,   268,
     220,   220,     5,   212,     6,     6,     6,     9,     6,     6,
       5,   238,   268,   222,   276,   210,   183,   270,   270,   270,
     271,   271,   272,   272,   273,   274,   275,     4,    21,    63,
      64,    72,    73,    74,    76,    77,    78,    79,    80,   128,
     204,   205,   206,   216,   195,    76,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     4,     5,
     193,   276,   276,   276,   212,   276,   276,    92,   276,   268,
     213,   169,   261,   276,   276,   264,   260,   262,     9,   182,
     226,   276,   276,   258,   257,   166,   266,    58,    59,   267,
     265,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       5,   204,     6,   234,   235,   236,   276,   276,   197,   198,
     276,   268,   239,   268,   268,   268,   268,   268,   268,   268,
     268,   268,   268,   268,   268,   268,   268,   193,   262,   225,
     184,   276,   238,   276,   276,   276,   276,   276,   276,    91,
     276,   204,   276,     9,   182,     4,   183,   183,   197,   184,
     276,   183,   183
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
     435,   436,   437,   438,   439
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned short int
  BisonParser::yyr1_[] =
  {
         0,   185,   186,   187,   187,   188,   189,   188,   190,   190,
     190,   190,   190,   190,   190,   190,   192,   191,   193,   193,
     193,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   195,   195,   196,   196,
     197,   197,   198,   198,   198,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   200,   200,   202,
     201,   203,   203,   203,   203,   204,   204,   204,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     206,   206,   207,   207,   207,   208,   209,   208,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     210,   210,   211,   211,   211,   212,   212,   212,   213,   213,
     213,   214,   214,   214,   214,   214,   215,   215,   215,   215,
     215,   215,   215,   215,   215,   215,   215,   215,   215,   215,
     215,   215,   217,   216,   216,   219,   218,   220,   220,   220,
     220,   220,   220,   221,   221,   221,   221,   221,   221,   222,
     222,   223,   223,   223,   223,   223,   224,   224,   225,   225,
     226,   226,   227,   228,   229,   229,   229,   229,   229,   229,
     229,   229,   229,   229,   229,   229,   229,   229,   229,   229,
     230,   230,   231,   231,   231,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   233,
     233,   233,   233,   233,   233,   233,   233,   233,   234,   234,
     234,   235,   235,   236,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   245,   246,   247,   248,   249,   250,   251,
     252,   252,   252,   252,   252,   252,   252,   252,   252,   252,
     252,   252,   253,   253,   253,   254,   254,   254,   254,   254,
     254,   254,   254,   254,   255,   255,   255,   255,   255,   255,
     255,   255,   255,   255,   255,   255,   256,   256,   256,   257,
     257,   257,   257,   257,   258,   259,   259,   260,   260,   260,
     261,   262,   262,   264,   263,   263,   265,   265,   265,   266,
     267,   267,   268,   268,   269,   269,   269,   270,   270,   270,
     271,   271,   271,   271,   272,   272,   272,   273,   273,   273,
     274,   274,   275,   275,   276,   276
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
       5,     0,     2,     2,     2,     1,     3,     2,     3,     3,
       1,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       2,     1,     0,     2,     2,     4,     0,     5,     4,     4,
       4,     3,     4,     4,     4,     4,     4,     4,     4,     4,
       1,     3,     1,     1,     3,     1,     3,     2,     1,     3,
       2,     3,     3,     3,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     1,     1,     1,
       3,     1,     0,     5,     1,     0,     4,     1,     1,     3,
       3,     2,     2,     3,     3,     3,     3,     3,     3,     1,
       3,     1,     1,     1,     1,     1,     3,     3,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     4,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     3,
       3,     3,     3,     1,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     1,     3,     1,     1,     3,     3,     3,     3,
       1,     1,     3,     0,     5,     2,     3,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     2,     2,
       1,     3,     3,     3,     1,     3,     3,     1,     3,     3,
       1,     3,     1,     3,     1,     3
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
  "BOOTIMAGE", "UDF_BH", "INIT", "PMUFW_IMAGE", "AES_KEY_FILE",
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
       186,     0,    -1,   187,    -1,    -1,   187,   188,    -1,    81,
       7,   268,    -1,    -1,    91,   189,     7,     3,   190,     4,
      -1,    -1,   190,   216,    -1,   190,   208,    -1,   190,   200,
      -1,   190,   191,    -1,   190,   199,    -1,   190,   218,    -1,
     190,   206,    -1,    -1,    86,     3,   192,   193,     4,    -1,
     194,    -1,   194,     5,   193,    -1,   194,   193,    -1,    -1,
      18,     6,   235,    -1,    98,     6,   239,    -1,   130,     6,
     268,    -1,    17,     6,   234,    -1,   132,     6,   268,    -1,
     133,     6,   268,    -1,   134,     6,   268,    -1,   135,     6,
     268,    -1,   136,     6,   268,    -1,   137,     6,   268,    -1,
     138,     6,   268,    -1,   139,     6,   268,    -1,   140,     6,
     268,    -1,   141,     6,   268,    -1,   142,     6,   268,    -1,
     143,     6,   268,    -1,   144,     6,   268,    -1,    54,     6,
     268,    -1,    41,     6,   276,    -1,    42,     6,   276,    -1,
      19,     6,   236,    -1,   167,    -1,    51,     6,   197,    -1,
     158,    -1,   196,    -1,   196,     9,   195,    -1,   268,     5,
      76,     6,   276,    -1,    -1,   198,    -1,   198,     9,   197,
      -1,   276,    -1,   276,   182,   276,   183,    -1,   276,   182,
     184,   183,    -1,    76,     6,   276,    -1,    66,     6,   276,
      -1,    67,     6,   276,    -1,    68,     6,   276,    -1,   140,
       6,   268,    -1,   141,     6,   268,    -1,   142,     6,   268,
      -1,   143,     6,   268,    -1,   233,     6,   268,    -1,    98,
       6,   239,    -1,    21,     6,   238,    -1,    41,     6,   276,
      -1,   201,    -1,   200,   201,    -1,    -1,    75,     3,   202,
     203,     4,    -1,    -1,   203,   204,    -1,   203,   216,    -1,
     203,   206,    -1,   205,    -1,   205,     5,   204,    -1,   205,
     204,    -1,    76,     6,   276,    -1,    77,     6,    91,    -1,
      78,    -1,    79,    -1,   128,    -1,    80,     6,   276,    -1,
      21,     6,   238,    -1,    72,     6,   276,    -1,    73,     6,
     276,    -1,    74,     6,   276,    -1,    63,     6,   276,    -1,
      64,     6,   276,    -1,    83,   207,    -1,   207,    -1,    -1,
     207,   216,    -1,   207,   218,    -1,    10,    98,    11,   239,
      -1,    -1,    10,    99,   209,    11,   212,    -1,    99,     3,
     212,     4,    -1,    23,     3,   210,     4,    -1,    10,    23,
      11,   248,    -1,    23,     6,   248,    -1,    10,   100,    11,
     257,    -1,    10,   114,    11,   265,    -1,    10,    53,    11,
     226,    -1,   101,     3,   213,     4,    -1,    85,     3,   195,
       4,    -1,   105,     3,   259,     4,    -1,   105,     3,   262,
       4,    -1,   105,     3,   263,     4,    -1,   211,    -1,   211,
       5,   210,    -1,   248,    -1,    65,    -1,    26,     6,   276,
      -1,   215,    -1,   215,     5,   212,    -1,   215,   212,    -1,
     214,    -1,   214,     5,   213,    -1,   214,   213,    -1,    41,
       6,   276,    -1,    42,     6,   276,    -1,   102,     6,    92,
      -1,   103,     6,   276,    -1,   104,     6,   268,    -1,   240,
      -1,   241,    -1,   243,    -1,   244,    -1,   245,    -1,   247,
      -1,   246,    -1,   111,    -1,   113,     6,   276,    -1,   242,
      -1,   115,     6,   276,    -1,    69,    -1,    70,    -1,    71,
      -1,   112,     6,   276,    -1,    62,    -1,    -1,    10,   217,
     222,    11,   268,    -1,   268,    -1,    -1,     3,   219,   220,
       4,    -1,   223,    -1,   221,    -1,   221,     5,   220,    -1,
     223,     5,   220,    -1,   221,   220,    -1,   223,   220,    -1,
      84,     6,   268,    -1,    76,     6,   276,    -1,    65,     6,
     276,    -1,    21,     6,   229,    -1,    21,     6,   125,    -1,
      56,     6,    91,    -1,   223,    -1,   223,     5,   222,    -1,
     229,    -1,   232,    -1,   252,    -1,   253,    -1,   224,    -1,
      51,     6,   225,    -1,    52,     6,   228,    -1,   231,    -1,
     231,     9,   225,    -1,   227,    -1,   227,     5,   226,    -1,
     276,    -1,   276,    -1,    12,    -1,    45,    -1,    46,    -1,
      47,    -1,    48,    -1,    49,    -1,    50,    -1,    14,    -1,
      15,    -1,    13,    -1,   128,    -1,   126,    -1,   254,    -1,
     256,    -1,   238,    -1,    16,    -1,    28,    -1,    28,     6,
     165,    -1,   276,    -1,   276,   182,   276,   183,    -1,   276,
     182,   184,   183,    -1,    17,     6,   234,    -1,    18,     6,
     235,    -1,    19,     6,   236,    -1,    20,     6,   237,    -1,
      25,     6,   249,    -1,    24,     6,   250,    -1,    27,     6,
     251,    -1,   130,     6,   268,    -1,   132,     6,   268,    -1,
     133,     6,   268,    -1,   134,     6,   268,    -1,   135,     6,
     268,    -1,   136,     6,   268,    -1,   137,     6,   268,    -1,
     138,     6,   268,    -1,   139,     6,   268,    -1,   140,     6,
     268,    -1,   141,     6,   268,    -1,   142,     6,   268,    -1,
     143,     6,   268,    -1,    39,     6,   258,    -1,    38,     6,
     276,    -1,   144,     6,   268,    -1,   230,    -1,    21,     6,
     238,    -1,    98,     6,   239,    -1,    41,     6,   276,    -1,
      42,     6,   276,    -1,   167,    -1,    60,     6,   276,    -1,
      61,     6,   276,    -1,   158,    -1,    82,    -1,    87,    -1,
     128,    -1,   254,    -1,   119,    -1,   118,    -1,   120,    -1,
     121,    -1,   122,    -1,   123,    -1,   124,    -1,    95,    -1,
     147,    -1,   148,    -1,    95,    -1,   149,    -1,    95,    -1,
     150,    -1,   151,    -1,   152,    -1,   153,    -1,   154,    -1,
     155,    -1,   167,    -1,   156,    -1,   157,    -1,   158,    -1,
     159,    -1,   160,    -1,   161,    -1,   163,    -1,   162,    -1,
     164,    -1,    29,     6,   276,    -1,    30,     6,   276,    -1,
      31,     6,   276,    -1,    32,     6,   276,    -1,    33,     6,
     276,    -1,    34,     6,   276,    -1,    35,    -1,    36,    -1,
      22,     6,   276,    -1,    88,     6,   276,    -1,    89,     6,
     276,    -1,    90,     6,   276,    -1,    54,     6,   268,    -1,
      55,     6,   268,    -1,    57,     6,   268,    -1,   130,    -1,
     255,    -1,   144,    -1,   145,    -1,   146,    -1,   117,    -1,
     116,    -1,   118,    -1,   131,    -1,   132,    -1,   133,    -1,
     134,    -1,   135,    -1,   136,    -1,   137,    -1,   138,    -1,
     139,    -1,   140,    -1,   141,    -1,   142,    -1,   143,    -1,
     129,    -1,   125,    -1,   127,    -1,    37,     6,   276,    -1,
      39,     6,   258,    -1,    38,     6,   276,    -1,    40,    -1,
     257,     9,   257,    -1,   168,    -1,   260,    -1,   260,     9,
     260,    -1,   106,     6,   261,    -1,   108,     6,   276,    -1,
     107,     6,   276,    -1,   169,    -1,    91,    -1,   262,     5,
     262,    -1,    -1,   170,     3,   264,   262,     4,    -1,   263,
     263,    -1,    43,     6,   266,    -1,    44,     6,   267,    -1,
     265,     5,   265,    -1,   166,    -1,    58,    -1,    59,    -1,
      93,    -1,    94,    -1,    97,    -1,    96,    -1,   182,   276,
     183,    -1,   269,    -1,   177,   270,    -1,   181,   270,    -1,
     270,    -1,   271,   174,   270,    -1,   271,   175,   270,    -1,
     271,   176,   270,    -1,   271,    -1,   272,   177,   271,    -1,
     272,   178,   271,    -1,   272,    -1,   273,   179,   272,    -1,
     273,   180,   272,    -1,   273,    -1,   274,   173,   273,    -1,
     274,    -1,   275,   172,   274,    -1,   275,    -1,   276,   171,
     275,    -1
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
     237,   243,   244,   247,   250,   253,   255,   259,   262,   266,
     270,   272,   274,   276,   280,   284,   288,   292,   296,   300,
     304,   307,   309,   310,   313,   316,   321,   322,   328,   333,
     338,   343,   347,   352,   357,   362,   367,   372,   377,   382,
     387,   389,   393,   395,   397,   401,   403,   407,   410,   412,
     416,   419,   423,   427,   431,   435,   439,   441,   443,   445,
     447,   449,   451,   453,   455,   459,   461,   465,   467,   469,
     471,   475,   477,   478,   484,   486,   487,   492,   494,   496,
     500,   504,   507,   510,   514,   518,   522,   526,   530,   534,
     536,   540,   542,   544,   546,   548,   550,   554,   558,   560,
     564,   566,   570,   572,   574,   576,   578,   580,   582,   584,
     586,   588,   590,   592,   594,   596,   598,   600,   602,   604,
     606,   608,   612,   614,   619,   624,   628,   632,   636,   640,
     644,   648,   652,   656,   660,   664,   668,   672,   676,   680,
     684,   688,   692,   696,   700,   704,   708,   712,   716,   718,
     722,   726,   730,   734,   736,   740,   744,   746,   748,   750,
     752,   754,   756,   758,   760,   762,   764,   766,   768,   770,
     772,   774,   776,   778,   780,   782,   784,   786,   788,   790,
     792,   794,   796,   798,   800,   802,   804,   806,   808,   810,
     812,   816,   820,   824,   828,   832,   836,   838,   840,   844,
     848,   852,   856,   860,   864,   868,   870,   872,   874,   876,
     878,   880,   882,   884,   886,   888,   890,   892,   894,   896,
     898,   900,   902,   904,   906,   908,   910,   912,   914,   916,
     920,   924,   928,   930,   934,   936,   938,   942,   946,   950,
     954,   956,   958,   962,   963,   969,   972,   976,   980,   984,
     986,   988,   990,   992,   994,   996,   998,  1002,  1004,  1007,
    1010,  1012,  1016,  1020,  1024,  1026,  1030,  1034,  1036,  1040,
    1044,  1046,  1050,  1052,  1056,  1058
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   217,   217,   219,   220,   222,   223,   223,   229,   230,
     231,   232,   233,   234,   235,   236,   239,   239,   244,   245,
     246,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   266,   267,
     268,   269,   276,   277,   278,   279,   283,   284,   287,   288,
     290,   291,   294,   296,   298,   302,   303,   304,   306,   307,
     309,   311,   313,   315,   316,   317,   318,   321,   322,   325,
     325,   334,   335,   336,   337,   340,   341,   342,   345,   346,
     347,   348,   349,   350,   352,   353,   358,   363,   368,   389,
     400,   401,   404,   405,   406,   409,   412,   412,   415,   416,
     417,   422,   423,   424,   425,   426,   427,   428,   429,   430,
     433,   434,   437,   438,   439,   442,   443,   444,   447,   448,
     449,   452,   453,   454,   455,   456,   459,   462,   464,   466,
     468,   470,   472,   474,   476,   478,   480,   484,   485,   486,
     487,   488,   491,   491,   498,   504,   504,   510,   511,   512,
     513,   514,   515,   518,   521,   522,   524,   525,   526,   532,
     533,   536,   537,   538,   539,   540,   543,   544,   547,   548,
     551,   552,   555,   560,   562,   563,   564,   565,   566,   567,
     568,   569,   570,   571,   574,   575,   576,   577,   578,   579,
     582,   583,   586,   587,   588,   590,   591,   592,   593,   594,
     595,   596,   597,   598,   599,   600,   601,   602,   603,   604,
     605,   606,   607,   608,   609,   610,   611,   612,   613,   614,
     615,   616,   617,   618,   619,   620,   621,   622,   623,   626,
     627,   628,   629,   630,   631,   632,   633,   634,   637,   638,
     639,   642,   643,   646,   647,   650,   653,   656,   659,   662,
     665,   668,   671,   674,   677,   680,   683,   686,   689,   692,
     695,   696,   697,   698,   699,   700,   701,   702,   703,   704,
     705,   706,   709,   710,   711,   714,   715,   716,   717,   718,
     719,   720,   721,   722,   725,   726,   727,   728,   729,   730,
     731,   732,   733,   734,   735,   736,   739,   740,   741,   744,
     747,   750,   753,   756,   759,   763,   764,   766,   767,   768,
     770,   774,   779,   781,   781,   783,   785,   788,   789,   792,
     795,   798,   804,   805,   808,   809,   810,   816,   817,   818,
     822,   823,   824,   825,   828,   829,   830,   833,   834,   835,
     838,   839,   842,   843,   847,   848
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
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 1226;
  const int BisonParser::yynnts_ = 92;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 185;

  const unsigned int BisonParser::yyuser_token_number_max_ = 439;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 24 "parser/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 2959 "bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 853 "parser/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
