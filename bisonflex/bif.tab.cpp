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

// Parser uses raw pointers for compatibility with Bison semantics
// These are transferred to options lists which take ownership
BifOptions* currentBifOptions;
PartitionBifOptions* currentPartitionBifOptions ;
ImageBifOptions* currentImageBifOptions;
LmsKeyTypeParam lmsParams;

/* Line 279 of lalr1.cc  */
#line 58 "bisonflex/bif.tab.cpp"


#include "bif.tab.hpp"

/* User implementation prologue.  */

/* Line 285 of lalr1.cc  */
#line 66 "bisonflex/bif.tab.cpp"
/* Unqualified %code blocks.  */
/* Line 286 of lalr1.cc  */
#line 47 "parser/bif.y"

    static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner);
    #include "options.h"


/* Line 286 of lalr1.cc  */
#line 76 "bisonflex/bif.tab.cpp"


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
#line 172 "bisonflex/bif.tab.cpp"

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
#line 109 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 242 "bisonflex/bif.tab.cpp"
        break;
      case 92: /* HEXWORD */
/* Line 455 of lalr1.cc  */
#line 109 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 249 "bisonflex/bif.tab.cpp"
        break;
      case 93: /* FILENAME */
/* Line 455 of lalr1.cc  */
#line 109 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 256 "bisonflex/bif.tab.cpp"
        break;
      case 94: /* QFILENAME */
/* Line 455 of lalr1.cc  */
#line 109 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 263 "bisonflex/bif.tab.cpp"
        break;
      case 260: /* filename */
/* Line 455 of lalr1.cc  */
#line 109 "parser/bif.y"
        { free(((*yyvaluep).string)); };
/* Line 455 of lalr1.cc  */
#line 270 "bisonflex/bif.tab.cpp"
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
#line 371 "bisonflex/bif.tab.cpp"

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
#line 217 "parser/bif.y"
    { options.includeBifOptionsList.push_back((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 218 "parser/bif.y"
    { if (!includefound)
                                                                                      {currentBifOptions = new BifOptions(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series(), (yysemantic_stack_[(1) - (1)].string));}
                                                                                  else{
                                                                                    if (!currentBifOptions) {
                                                                                    currentBifOptions = new BifOptions(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series(), (yysemantic_stack_[(1) - (1)].string));
                                                                                    options.bifOptions = currentBifOptions;
                                                                                    options.bifOptionsList.push_back(currentBifOptions);}
                                                                                  } }
    break;

  case 7:
/* Line 670 of lalr1.cc  */
#line 227 "parser/bif.y"
    { if (!includefound){options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions);} }
    break;

  case 8:
/* Line 670 of lalr1.cc  */
#line 230 "parser/bif.y"
    { options.includeBifOptionsList.push_back((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 9:
/* Line 670 of lalr1.cc  */
#line 231 "parser/bif.y"
    { currentBifOptions = new BifOptions(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series(), (yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 10:
/* Line 670 of lalr1.cc  */
#line 233 "parser/bif.y"
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
    break;

  case 19:
/* Line 670 of lalr1.cc  */
#line 247 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(), currentBifOptions->GetIdCode()); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 258 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 259 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 260 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 261 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 262 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 263 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 264 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 265 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 266 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 34:
/* Line 670 of lalr1.cc  */
#line 267 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 268 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.partitionRevokeId = (yysemantic_stack_[(3) - (3)].number);}
    break;

  case 36:
/* Line 670 of lalr1.cc  */
#line 269 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkRevokeId = (yysemantic_stack_[(3) - (3)].number); 
                                                                                if(options.GetArchType() == Arch::VERSAL)
                                                                                {
                                                                                   LOG_ERROR("SPK_REVOKE_ID does not support for versal arch, instead please use revoke_id option.");
                                                                                }
                                                                                }
    break;

  case 37:
/* Line 670 of lalr1.cc  */
#line 276 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 38:
/* Line 670 of lalr1.cc  */
#line 277 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 279 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 287 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ihtOptionalDataInfo.push_back(std::pair<std::string, uint32_t>((yysemantic_stack_[(5) - (1)].string), (yysemantic_stack_[(5) - (5)].number))); }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 294 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 296 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 298 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 302 "parser/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 303 "parser/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 304 "parser/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); 
                                                                                  options.SetDl9Series((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 306 "parser/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 307 "parser/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 55:
/* Line 670 of lalr1.cc  */
#line 308 "parser/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 56:
/* Line 670 of lalr1.cc  */
#line 309 "parser/bif.y"
    { currentBifOptions->SetPdiType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 310 "parser/bif.y"
    { currentBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number));}
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 317 "parser/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 318 "parser/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions);
                                                                                  if((options.archType == Arch::SPARTANUP) && (currentBifOptions->imageBifOptionList.size() > 1))
                                                                                  {
                                                                                      LOG_ERROR("'-arch spartanup' cannot accomodate more than one Sub-System.");
                                                                                  }
                                                                                }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 337 "parser/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 338 "parser/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 71:
/* Line 670 of lalr1.cc  */
#line 339 "parser/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 72:
/* Line 670 of lalr1.cc  */
#line 340 "parser/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 341 "parser/bif.y"
    { LOG_ERROR("BIF attribute error !!!\n\t This usage of 'init' is not supported. See 'bootgen -bif_help init' for usage details."); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 342 "parser/bif.y"
    { LOG_ERROR("Copy to Memory feature with the attribute 'copy' is no more supported.\n\t   This can be duplicated with the option 'imagestore'. Please refer UG1283 for more details.");
                                                                                  currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 75:
/* Line 670 of lalr1.cc  */
#line 344 "parser/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 76:
/* Line 670 of lalr1.cc  */
#line 345 "parser/bif.y"
    { currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 77:
/* Line 670 of lalr1.cc  */
#line 346 "parser/bif.y"
    { currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 78:
/* Line 670 of lalr1.cc  */
#line 347 "parser/bif.y"
    { currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 79:
/* Line 670 of lalr1.cc  */
#line 348 "parser/bif.y"
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
                                                                                }
    break;

  case 80:
/* Line 670 of lalr1.cc  */
#line 361 "parser/bif.y"
    { if (options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP)
                                                                                  {
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr measurement index' is not supported for the specified architecture");
                                                                                  }
                                                                                  else if(options.GetArchType() == Arch::VERSAL || options.GetArchType() == Arch::VERSALGEN2)
                                                                                  {
                                                                                    currentImageBifOptions->SetPcrMeasurementIndex((yysemantic_stack_[(3) - (3)].number));
                                                                                  }
                                                                                }
    break;

  case 86:
/* Line 670 of lalr1.cc  */
#line 381 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQ || options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported for the specified architecture.\n\t   Refer 'bootgen -bif_help' for more details");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 87:
/* Line 670 of lalr1.cc  */
#line 384 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 91:
/* Line 670 of lalr1.cc  */
#line 389 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                  if(options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); 
                                                                                  currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 92:
/* Line 670 of lalr1.cc  */
#line 394 "parser/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); }
    break;

  case 103:
/* Line 670 of lalr1.cc  */
#line 409 "parser/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 410 "parser/bif.y"
    { currentBifOptions->SetBootDevice(BootDevice::IMAGESTORE); }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 411 "parser/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 424 "parser/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 425 "parser/bif.y"
    { currentBifOptions->SetAuthJtagSPKRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 426 "parser/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 427 "parser/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 428 "parser/bif.y"
    { currentBifOptions->SetAuthJtagSignatureFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 117:
/* Line 670 of lalr1.cc  */
#line 431 "parser/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 118:
/* Line 670 of lalr1.cc  */
#line 434 "parser/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 119:
/* Line 670 of lalr1.cc  */
#line 436 "parser/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 120:
/* Line 670 of lalr1.cc  */
#line 438 "parser/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 440 "parser/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 442 "parser/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 444 "parser/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 446 "parser/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 448 "parser/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 450 "parser/bif.y"
    {  currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));}
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 452 "parser/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->SetSmapWidth((yysemantic_stack_[(3) - (3)].number));
                                                                                }
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 456 "parser/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 129:
/* Line 670 of lalr1.cc  */
#line 457 "parser/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 130:
/* Line 670 of lalr1.cc  */
#line 458 "parser/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 459 "parser/bif.y"
    { currentBifOptions->SetPufRingOscilltorSwapConfigValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 460 "parser/bif.y"
    { currentBifOptions->SetDiceEnable(); }
    break;

  case 133:
/* Line 670 of lalr1.cc  */
#line 463 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(),options.IsVersalNetSeries(),  currentBifOptions->GetIdCode()); }
    break;

  case 134:
/* Line 670 of lalr1.cc  */
#line 466 "parser/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 135:
/* Line 670 of lalr1.cc  */
#line 470 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 136:
/* Line 670 of lalr1.cc  */
#line 476 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode()); }
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 490 "parser/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 493 "parser/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 494 "parser/bif.y"
    { currentPartitionBifOptions->imageStoreId = (yysemantic_stack_[(3) - (3)].number);
                                                                                  currentPartitionBifOptions->SetPartitionType(PartitionType::IMAGE_STORE_PDI); }
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 497 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 498 "parser/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string), currentImageBifOptions->GetImageType());
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 526 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] is supported only for ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 531 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 533 "parser/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 534 "parser/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 535 "parser/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 536 "parser/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 537 "parser/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 538 "parser/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 539 "parser/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 540 "parser/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 541 "parser/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 542 "parser/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 545 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 546 "parser/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 547 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 548 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 549 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 550 "parser/bif.y"
    { currentPartitionBifOptions->SetLockStepFlag();}
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 553 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 554 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 557 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 558 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 559 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 561 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 562 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 563 "parser/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 189:
/* Line 670 of lalr1.cc  */
#line 564 "parser/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 565 "parser/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 191:
/* Line 670 of lalr1.cc  */
#line 566 "parser/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 192:
/* Line 670 of lalr1.cc  */
#line 567 "parser/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 193:
/* Line 670 of lalr1.cc  */
#line 568 "parser/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 194:
/* Line 670 of lalr1.cc  */
#line 569 "parser/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 195:
/* Line 670 of lalr1.cc  */
#line 570 "parser/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 196:
/* Line 670 of lalr1.cc  */
#line 571 "parser/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 197:
/* Line 670 of lalr1.cc  */
#line 572 "parser/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 198:
/* Line 670 of lalr1.cc  */
#line 573 "parser/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 574 "parser/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 200:
/* Line 670 of lalr1.cc  */
#line 575 "parser/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 202:
/* Line 670 of lalr1.cc  */
#line 577 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 203:
/* Line 670 of lalr1.cc  */
#line 578 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 204:
/* Line 670 of lalr1.cc  */
#line 579 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 205:
/* Line 670 of lalr1.cc  */
#line 580 "parser/bif.y"
    { currentPartitionBifOptions->SetSPKRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 206:
/* Line 670 of lalr1.cc  */
#line 581 "parser/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 207:
/* Line 670 of lalr1.cc  */
#line 582 "parser/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 208:
/* Line 670 of lalr1.cc  */
#line 583 "parser/bif.y"
    { currentPartitionBifOptions->SetClusterNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 209:
/* Line 670 of lalr1.cc  */
#line 584 "parser/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 210:
/* Line 670 of lalr1.cc  */
#line 585 "parser/bif.y"
    { currentPartitionBifOptions->SetDelayAuth(true); }
    break;

  case 211:
/* Line 670 of lalr1.cc  */
#line 586 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBootFlag(); }
    break;

  case 221:
/* Line 670 of lalr1.cc  */
#line 600 "parser/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 223:
/* Line 670 of lalr1.cc  */
#line 604 "parser/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 225:
/* Line 670 of lalr1.cc  */
#line 608 "parser/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 242:
/* Line 670 of lalr1.cc  */
#line 657 "parser/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 243:
/* Line 670 of lalr1.cc  */
#line 658 "parser/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 244:
/* Line 670 of lalr1.cc  */
#line 659 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), false); }
    break;

  case 245:
/* Line 670 of lalr1.cc  */
#line 660 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), true); }
    break;

  case 246:
/* Line 670 of lalr1.cc  */
#line 661 "parser/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 247:
/* Line 670 of lalr1.cc  */
#line 662 "parser/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 248:
/* Line 670 of lalr1.cc  */
#line 663 "parser/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 249:
/* Line 670 of lalr1.cc  */
#line 664 "parser/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 250:
/* Line 670 of lalr1.cc  */
#line 665 "parser/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 251:
/* Line 670 of lalr1.cc  */
#line 666 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmARegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 252:
/* Line 670 of lalr1.cc  */
#line 667 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 253:
/* Line 670 of lalr1.cc  */
#line 668 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmCRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 254:
/* Line 670 of lalr1.cc  */
#line 671 "parser/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 255:
/* Line 670 of lalr1.cc  */
#line 672 "parser/bif.y"
    { currentPartitionBifOptions->acFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 256:
/* Line 670 of lalr1.cc  */
#line 673 "parser/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 273:
/* Line 670 of lalr1.cc  */
#line 698 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 274:
/* Line 670 of lalr1.cc  */
#line 701 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 275:
/* Line 670 of lalr1.cc  */
#line 704 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 276:
/* Line 670 of lalr1.cc  */
#line 707 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 285:
/* Line 670 of lalr1.cc  */
#line 728 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP || options.GetArchType() == Arch::VERSAL)
                                                                   LOG_ERROR("BIF attribute error !!!\n\t\t'lms_key_params' is not supported with the mentioned -arch");
                                                                 lmsParams.lms_param = (yysemantic_stack_[(1) - (1)].string);
                                                                 currentBifOptions->SetLmsParameters(lmsParams) ;
                                                               }
    break;

  case 287:
/* Line 670 of lalr1.cc  */
#line 735 "parser/bif.y"
    { lmsParams.type = (yysemantic_stack_[(2) - (1)].authkeylevel_t);}
    break;

  case 290:
/* Line 670 of lalr1.cc  */
#line 739 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 294:
/* Line 670 of lalr1.cc  */
#line 749 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 295:
/* Line 670 of lalr1.cc  */
#line 752 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 300:
/* Line 670 of lalr1.cc  */
#line 764 "parser/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 302:
/* Line 670 of lalr1.cc  */
#line 771 "parser/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 303:
/* Line 670 of lalr1.cc  */
#line 772 "parser/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 305:
/* Line 670 of lalr1.cc  */
#line 777 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 306:
/* Line 670 of lalr1.cc  */
#line 778 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 307:
/* Line 670 of lalr1.cc  */
#line 779 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 309:
/* Line 670 of lalr1.cc  */
#line 783 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 310:
/* Line 670 of lalr1.cc  */
#line 784 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 312:
/* Line 670 of lalr1.cc  */
#line 788 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 313:
/* Line 670 of lalr1.cc  */
#line 789 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 315:
/* Line 670 of lalr1.cc  */
#line 793 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 317:
/* Line 670 of lalr1.cc  */
#line 797 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 319:
/* Line 670 of lalr1.cc  */
#line 802 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1623 "bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -464;
  const short int
  BisonParser::yypact_[] =
  {
      -464,    14,    34,  -464,     9,  -464,  -464,    37,    15,    86,
    -464,  -464,  -464,    97,   123,  -464,  -464,   890,   964,  -464,
    -464,   150,   130,    95,   136,   164,   171,   182,   201,   209,
    -464,   214,   223,   217,   227,   233,   244,  -464,  -464,  -464,
    -464,  -464,  -464,  -464,  -464,  -464,  -464,  -464,  -464,  -464,
    -464,  -464,  -464,  -464,  -464,  -464,  -464,  -464,   176,  -464,
    -464,    92,  -464,  -464,  -464,   257,  -464,  -464,  -464,  -464,
     645,   264,   274,   277,  -464,   281,   283,   792,   154,    -5,
     127,   169,   169,   169,   169,  -464,   169,    92,    37,  -464,
     158,   997,   -17,    26,  -464,  -464,  -464,  -464,    37,  -464,
    -464,  -464,  -464,  -464,   293,   303,   305,   314,   318,   321,
     322,   324,   325,   326,   327,   328,   329,   330,   332,   333,
    -464,  -464,   334,   337,   338,   339,  -464,  -464,  -464,  -464,
    -464,  -464,   340,   345,   352,   353,   354,   355,   356,   361,
     362,   365,  -464,   366,  -464,   367,   368,   369,   370,  -464,
    -464,  -464,  -464,  -464,  -464,   371,   374,   376,   377,   378,
     379,  -464,  -464,  -464,   344,    23,   498,  -464,  -464,  -464,
    -464,  -464,  -464,  -464,  -464,  -464,   127,   169,   158,   375,
     206,    75,   381,   385,   383,  -464,   384,  -464,  -464,   393,
     394,  -464,  -464,  -464,  -464,   169,   169,   169,  -464,  -464,
     -43,   -24,     4,   234,   237,   239,   239,   239,   239,  -464,
     239,   398,   395,   402,   259,  -464,  -464,  -464,  -464,  -464,
    -464,  -464,   397,   403,   407,  -464,  -464,  -464,  -464,  -464,
    -464,  -464,  -464,   421,   109,  -464,  -464,  -464,  -464,  -464,
    -464,  -464,  -464,   420,   422,   423,   424,   425,   431,    25,
    -464,   438,   439,   440,   444,   445,   418,   178,     3,  -464,
     -49,   -64,   -78,   306,   988,   169,   298,   296,   308,   310,
     169,   169,   169,   169,   169,   169,   169,   302,   169,   169,
     169,   169,    37,    37,   373,    37,   169,   169,   169,   169,
      37,   169,   169,   169,   158,    37,    37,    37,    37,    37,
      37,  -464,   645,  -464,   645,  -464,  -464,  -464,   447,   239,
    -464,   997,   462,   463,   465,  -464,   464,   466,   468,   470,
     154,    37,   792,   169,  -464,    -5,  -464,  -464,   -80,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   169,   360,
    -464,    37,   400,   471,   472,   473,   474,   475,   476,   477,
     478,   479,   480,   481,   483,   484,   485,  -464,  -464,   488,
     220,   169,   169,   169,  -464,   997,  -464,   169,   169,   401,
     169,    37,  -464,   -17,  -464,   335,   169,   169,  -464,  -464,
     162,  -464,   405,  -464,   336,  -464,  -464,  -464,  -464,  -464,
    -464,  -464,  -464,  -464,  -464,  -464,  -464,  -464,  -464,   239,
    -464,  -464,  -464,  -464,  -464,  -464,  -464,   239,   239,   239,
     239,   239,   239,   239,  -464,  -464,   239,   239,  -464,   491,
    -144,  -464,   239,  -464,  -464,  -464,  -464,   239,   239,   239,
     239,  -464,   239,   239,   239,  -464,  -464,  -464,  -464,  -464,
    -464,  -464,  -464,  -464,   169,  -464,   169,   169,   302,   206,
     364,   143,    75,  -464,  -464,  -464,   239,  -464,  -464,  -464,
    -464,  -464,   -43,   -43,   -24,   -24,     4,   234,   237,  -464,
     495,   518,   529,   532,   535,   536,   545,   550,  -464,  -464,
     551,  -464,  -464,   342,  -464,  -464,  -464,   554,   -49,   -64,
     -78,   169,   169,   169,    37,   158,    37,    37,    37,    37,
      37,    37,  -464,   259,  -464,   239,   239,   239,  -464,   239,
     239,  -464,   239,  -464,  -464,  -464,  -464,   239,   239,   405,
    -464,   489,   169,    41,  -464,   239,   239,  -464,   464,  -464,
    -464,  -464,  -464,  -464,   470,   154,   169,   169,   169,   169,
     169,   169,   482,   169,   155,  -464,   169,  -464,  -464,  -464,
     239,   239,  -464,   552,   -65,  -464,  -464,  -464,  -464,  -464,
    -464,  -464,  -464,  -464,   268,  -464,   388,   -71,  -464,   239,
     239,   239,   239,   239,   239,  -464,   239,  -464,   239,   169,
     144,  -464,  -464,  -464,  -464,   390,   -70,  -464,  -464
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,     6,     4,     0,     0,     0,
     296,   297,     5,     0,     0,    11,    11,     0,     0,   136,
       7,   133,     0,     0,     0,     0,     0,     0,     0,     0,
      83,     0,     0,     0,     0,     0,     0,   263,   262,   215,
     214,   216,   217,   218,   219,   220,   212,   257,   265,   266,
     267,   268,   269,   259,   260,   261,    15,    16,    14,    58,
      18,    82,    13,    12,    17,     0,   213,   258,   135,    10,
       0,     0,     0,     0,    87,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    60,     0,    81,    44,    19,
       0,     0,     0,     0,    59,   133,    84,    85,     0,   165,
     174,   172,   173,   180,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   181,     0,     0,     0,     0,     0,     0,
     248,   249,     0,     0,     0,     0,   166,   167,   168,   169,
     170,   171,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   210,     0,   211,     0,     0,     0,     0,   264,
     271,   176,   272,   175,   270,   257,   266,   267,   268,   269,
     259,   228,   209,   206,     0,   139,   138,   156,   152,   201,
     153,   179,   154,   155,   177,   178,     0,     0,     0,     0,
       0,     0,     0,     0,   150,    56,     0,   104,   238,     0,
     101,   103,    92,   299,   298,     0,     0,     0,   301,   304,
     308,   311,   314,   316,   318,    57,    51,    52,    53,    62,
      50,     0,    41,     0,    24,   229,    55,   132,   128,   129,
     130,   124,     0,     0,     0,   230,   231,   233,   234,   235,
     236,   237,   232,     0,   106,   117,   118,   126,   119,   120,
     121,   123,   122,     0,     0,     0,     0,     0,     0,   109,
     285,     0,     0,     0,     0,     0,   279,     0,     0,    54,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   137,     0,   142,     0,   143,    91,    95,   161,   163,
      86,     0,     0,     0,     0,   276,    93,     0,     0,    94,
       0,     0,     0,     0,    90,     0,   302,   303,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    83,
      97,    44,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    40,    38,     0,
      21,     0,     0,     0,    89,     0,   108,     0,     0,     0,
       0,     0,    96,     0,   111,     0,     0,     0,   287,    98,
       0,    99,     0,   100,   289,   221,   222,   186,   223,   224,
     187,   225,   226,   188,   227,   189,   148,   147,   179,   250,
     240,   191,   239,   190,   241,   192,   182,   242,   243,   244,
     245,   246,   247,   199,   278,   198,   204,   205,   157,   159,
     183,   158,   164,   254,   255,   149,   256,   207,   208,   146,
     145,   144,   251,   252,   253,   203,   193,   194,   195,   196,
     197,   200,   140,   141,     0,    88,     0,     0,     0,     0,
       0,     0,     0,   202,   134,   151,   105,   102,   300,   305,
     306,   307,   309,   310,   312,   313,   315,   317,   319,    61,
       0,     0,     0,     0,     0,     0,     0,     0,    71,    72,
       0,    73,    63,    66,    65,    64,    42,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    20,    24,    23,   131,   125,   127,   107,   112,
     113,   114,   115,   116,   110,   284,   281,   283,   282,     0,
     280,   286,     0,     0,   162,   273,   275,   274,   277,   293,
     290,   294,   295,   291,   292,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    68,     0,    28,    25,    37,
      35,    36,    39,    45,    47,    34,    26,    27,    29,    30,
      31,    32,    33,    22,     0,   160,     0,     0,    75,    79,
      80,    76,    77,    78,    69,    70,    74,    67,    43,     0,
       0,   288,   185,   184,    46,     0,     0,    49,    48
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -464,  -464,  -464,  -464,  -464,  -464,   549,  -464,  -464,  -341,
    -464,   225,  -464,   -12,  -464,  -464,  -464,   510,  -464,  -464,
    -463,  -464,   230,   540,  -464,  -464,   246,  -464,  -222,  -238,
    -464,  -464,   -55,  -464,   -38,  -464,  -139,  -464,   250,   -69,
    -464,    53,   132,  -464,  -464,   313,  -464,  -464,  -464,  -464,
      90,    94,    89,  -464,   -68,  -174,  -464,  -464,  -464,  -464,
    -464,  -464,  -464,  -464,   -67,  -464,  -464,  -464,  -464,  -464,
     297,  -464,  -464,   135,   133,  -464,   210,  -464,  -373,   496,
    -464,   139,  -464,  -464,     8,  -464,  -162,   -10,    -9,   256,
     258,   255,   -81
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     6,     8,     9,    17,    56,   214,   359,
     360,   211,   212,   552,   553,    57,    58,    59,   209,   339,
     482,   483,    60,    61,    62,   179,   189,   190,   233,   248,
     249,   234,    63,    77,    64,    70,   164,   165,   183,   166,
     167,   418,   307,   308,   421,   168,   169,   419,   170,    65,
     387,   390,   393,   395,   171,   216,   235,   236,   237,   238,
     239,   240,   241,   242,   191,   403,   401,   405,   172,   173,
     174,    67,   175,   316,   415,   255,   256,   516,   257,   384,
     519,   319,   530,   533,    68,   198,   199,   200,   201,   202,
     203,   204,   309
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       205,   206,   207,   208,   310,   210,    96,   383,   184,   521,
     185,   374,   366,   192,     3,    12,     7,   391,   338,   504,
     545,   186,    13,    97,   243,   244,   303,   305,   302,   523,
     373,   388,    96,   326,   327,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   385,   110,   111,    97,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     187,   122,   123,   392,   124,   125,   243,   244,   126,   127,
     128,   129,   130,   131,   132,   133,   389,   134,   135,   136,
     137,   577,   338,   138,   139,   245,   246,   247,   140,   445,
     386,   338,   338,    14,   458,    19,   213,   338,    79,   141,
      15,    80,    95,   583,   588,   142,   259,   143,   580,   306,
     144,   145,   146,   147,   365,     4,   328,   250,   317,   318,
     435,   148,   329,   330,   331,     5,    16,   245,   246,   247,
      10,    11,   251,   252,   253,   514,    78,   193,   194,    37,
      38,   149,    81,   508,   332,   333,   564,   188,   150,   151,
     152,   153,   154,   155,    48,   156,   157,   158,   159,   160,
      54,    55,   563,   442,   254,   443,   161,   459,   460,   461,
      82,   217,   162,    71,   334,   335,   470,    83,   218,   219,
     220,   163,   381,   382,   399,    10,    11,   254,    84,   407,
     408,   409,   410,   411,   412,   413,   398,   416,   417,   420,
     422,   531,   532,    72,    85,   427,   428,   429,   430,   195,
     432,   433,   434,   196,   197,    86,   566,    88,   471,   472,
     221,   222,   223,    90,   224,   503,    89,   473,   474,   475,
      91,   476,   477,   478,   479,   480,    92,   343,   344,   345,
     193,   194,   456,   312,   313,   314,   315,    93,    73,    74,
      75,    28,   453,   184,   225,   226,   227,   228,   229,   230,
     231,   346,   347,    98,    76,   193,   194,   232,   251,   252,
     253,   348,   581,   382,   349,   176,   343,   344,   345,   188,
     505,   506,   507,   481,   485,   177,   509,   510,   178,   512,
     423,   424,   180,   426,   181,   517,   518,   161,   431,   260,
     346,   347,   215,   436,   437,   438,   439,   440,   441,   261,
     348,   262,   195,   349,    66,    66,   196,   197,   350,   585,
     263,   556,   462,   463,   264,   464,   465,   265,   266,   454,
     267,   268,   269,   270,   271,   272,   273,   195,   274,   275,
     276,   196,   197,   277,   278,   279,   280,   544,   301,   213,
     351,   281,   352,   353,   354,   355,   356,   350,   282,   283,
     284,   285,   286,   470,   469,   525,   526,   287,   288,   357,
      95,   289,   290,   291,   292,   293,   294,   295,   358,   513,
     296,   470,   297,   298,   299,   300,   311,   320,   322,   351,
     323,   352,   353,   354,   355,   356,   321,   324,   336,   325,
     337,   338,   340,   361,   341,   471,   472,   342,   357,   362,
     550,   551,   554,   363,   473,   474,   475,   358,   476,   477,
     478,   479,   480,   471,   472,   364,   367,   380,   368,   369,
     370,   371,   473,   474,   475,   372,   476,   477,   478,   479,
     480,   420,   567,    30,   375,   376,   377,   378,   394,   379,
     402,   400,   444,    10,    11,   569,   570,   571,   572,   573,
     574,   414,   576,   404,   425,   578,   406,   568,   446,   447,
     481,   448,   450,   449,   451,   452,   487,   488,   489,   490,
     491,   492,   493,   494,   495,   496,   497,   498,   481,   499,
     500,   501,   502,   511,   382,   515,   250,   254,   554,   586,
     522,   535,   555,   304,   557,   558,   559,   560,   561,   562,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   529,   110,   111,   536,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   537,   122,   123,   538,   124,
     125,   539,   540,   126,   127,   128,   129,   130,   131,   132,
     133,   541,   134,   135,   136,   137,   542,   543,   138,   139,
     546,   579,   582,   140,   587,    18,   486,   584,    94,   484,
      87,   457,   455,   575,   141,   565,   524,   397,   547,   549,
     142,   527,   143,   548,   528,   144,   145,   146,   147,   258,
     520,   534,   466,   468,     0,   467,   148,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    37,    38,   149,     0,     0,     0,
       0,     0,     0,   150,   151,   152,   153,   154,   155,    48,
     156,   157,   158,   159,   160,    54,    55,     0,     0,     0,
       0,   161,     0,     0,     0,     0,     0,   162,     0,     0,
       0,     0,     0,     0,     0,     0,   163,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,     0,   110,
     111,     0,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,     0,   122,   123,     0,   124,   125,     0,     0,
     126,   127,   128,   129,   130,   131,   132,   133,     0,   134,
     135,   136,   137,     0,     0,   138,   139,     0,     0,     0,
     140,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   141,     0,     0,     0,     0,     0,   142,     0,   143,
       0,     0,   144,   145,   146,   147,     0,     0,     0,     0,
       0,     0,     0,   148,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    37,    38,   149,     0,     0,     0,     0,     0,     0,
     150,   151,   152,   153,   154,   155,    48,   156,   157,   158,
     159,   160,    54,    55,     0,     0,     0,     0,   161,     0,
       0,     0,     0,     0,   162,     0,     0,     0,     0,     0,
       0,     0,     0,   163,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   182,   109,     0,   110,   111,     0,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,     0,
     122,   123,     0,   124,   125,     0,     0,   126,   127,   128,
     129,   130,   131,   132,   133,     0,   134,   135,     0,   137,
       0,     0,   138,   139,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   142,     0,     0,     0,     0,   144,
     145,   146,   147,     0,     0,     0,     0,     0,     0,     0,
     148,     0,     0,    19,    20,     0,     0,     0,     0,     0,
      21,     0,     0,     0,     0,     0,     0,     0,    37,    38,
     149,    22,     0,    23,     0,     0,     0,   150,   151,   152,
     153,   154,   155,    48,   156,   157,   158,   159,   160,    54,
      55,    24,     0,     0,     0,   161,     0,     0,     0,     0,
       0,   162,     0,     0,     0,     0,     0,     0,     0,     0,
     163,     0,     0,     0,     0,     0,    25,    26,    27,     0,
       0,     0,     0,     0,     0,    28,    29,    19,    69,     0,
       0,     0,     0,    30,    21,    31,    32,     0,     0,     0,
       0,     0,     0,    10,    11,    22,     0,    23,    33,    34,
       0,    35,     0,     0,     0,    36,     0,     0,     0,     0,
      99,   100,   101,   102,   103,    24,    37,    38,    39,    40,
      41,    42,    43,    44,    45,     0,     0,     0,    46,     0,
      47,    48,    49,    50,    51,    52,    53,    54,    55,     0,
      25,    26,    27,   126,   127,   128,   129,   130,   131,    28,
      29,     0,     0,     0,     0,     0,     0,    30,     0,    31,
      32,     0,     0,     0,     0,     0,     0,    10,    11,   217,
       0,     0,    33,    34,     0,    35,   218,   219,   220,    36,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      37,    38,    39,    40,    41,    42,    43,    44,    45,     0,
       0,     0,    46,     0,    47,    48,    49,    50,    51,    52,
      53,    54,    55,     0,    37,    38,   149,     0,   221,   222,
     223,     0,   224,   396,   151,   152,   153,   154,    47,    48,
      49,    50,    51,    52,    53,    54,    55,     0,     0,     0,
       0,   161,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   225,   226,   227,   228,   229,   230,   231,     0,
       0,     0,     0,     0,     0,   232
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        81,    82,    83,    84,   178,    86,    61,     4,    77,   382,
      78,   249,   234,    80,     0,     7,     7,    95,   162,   360,
     483,    26,     7,    61,    41,    42,   165,   166,     5,   173,
       5,    95,    87,   195,   196,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    95,    24,    25,    87,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      65,    38,    39,   141,    41,    42,    41,    42,    45,    46,
      47,    48,    49,    50,    51,    52,   140,    54,    55,    56,
      57,   544,   162,    60,    61,   102,   103,   104,    65,   311,
     139,   162,   162,     7,   174,     3,    88,   162,     3,    76,
       3,     6,    10,   174,   174,    82,    98,    84,   173,   176,
      87,    88,    89,    90,     5,    81,   197,    91,    43,    44,
     294,    98,   165,   166,   167,    91,     3,   102,   103,   104,
      93,    94,   106,   107,   108,   373,     6,    96,    97,   116,
     117,   118,     6,   365,   168,   169,   519,   152,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   503,   302,   161,   304,   143,   329,   330,   331,
       6,    62,   149,    23,   170,   171,    21,     6,    69,    70,
      71,   158,     4,     5,   265,    93,    94,   161,     6,   270,
     271,   272,   273,   274,   275,   276,   264,   278,   279,   280,
     281,    58,    59,    53,     3,   286,   287,   288,   289,   168,
     291,   292,   293,   172,   173,     6,   175,     3,    63,    64,
     111,   112,   113,     6,   115,     5,     3,    72,    73,    74,
       3,    76,    77,    78,    79,    80,     3,    17,    18,    19,
      96,    97,   323,    37,    38,    39,    40,     3,    98,    99,
     100,    75,   320,   322,   145,   146,   147,   148,   149,   150,
     151,    41,    42,     6,   114,    96,    97,   158,   106,   107,
     108,    51,     4,     5,    54,    11,    17,    18,    19,   152,
     361,   362,   363,   128,   339,    11,   367,   368,    11,   370,
     282,   283,    11,   285,    11,   376,   377,   143,   290,     6,
      41,    42,   144,   295,   296,   297,   298,   299,   300,     6,
      51,     6,   168,    54,    17,    18,   172,   173,    98,   175,
       6,   495,   332,   333,     6,   334,   335,     6,     6,   321,
       6,     6,     6,     6,     6,     6,     6,   168,     6,     6,
       6,   172,   173,     6,     6,     6,     6,     5,     4,   341,
     130,     6,   132,   133,   134,   135,   136,    98,     6,     6,
       6,     6,     6,    21,     4,   446,   447,     6,     6,   149,
      10,     6,     6,     6,     6,     6,     6,     6,   158,   371,
       6,    21,     6,     6,     6,     6,    11,     6,     5,   130,
       6,   132,   133,   134,   135,   136,    11,     4,   164,     5,
     163,   162,     4,     6,     9,    63,    64,     5,   149,     6,
     491,   492,   493,     6,    72,    73,    74,   158,    76,    77,
      78,    79,    80,    63,    64,     4,     6,     9,     6,     6,
       6,     6,    72,    73,    74,     4,    76,    77,    78,    79,
      80,   522,   523,    83,     6,     6,     6,     3,   142,     4,
     154,   153,     5,    93,    94,   536,   537,   538,   539,   540,
     541,   159,   543,   155,    91,   546,   156,   535,     6,     6,
     128,     6,     6,     9,     6,     5,    76,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,   128,     6,
       6,     6,     4,    92,     5,   160,    91,   161,   579,   580,
       9,     6,   494,     5,   496,   497,   498,   499,   500,   501,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,   157,    24,    25,     6,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,     6,    38,    39,     6,    41,
      42,     6,     6,    45,    46,    47,    48,    49,    50,    51,
      52,     6,    54,    55,    56,    57,     6,     6,    60,    61,
       6,     9,   174,    65,   174,    16,   341,   579,    58,   339,
      30,   325,   322,    91,    76,   522,   444,   264,   488,   490,
      82,   448,    84,   489,   449,    87,    88,    89,    90,    93,
     380,   452,   336,   338,    -1,   337,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   116,   117,   118,    -1,    -1,    -1,
      -1,    -1,    -1,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,    -1,    -1,    -1,
      -1,   143,    -1,    -1,    -1,    -1,    -1,   149,    -1,    -1,
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
      -1,    -1,    -1,    -1,    -1,    75,    76,     3,     4,    -1,
      -1,    -1,    -1,    83,    10,    85,    86,    -1,    -1,    -1,
      -1,    -1,    -1,    93,    94,    21,    -1,    23,    98,    99,
      -1,   101,    -1,    -1,    -1,   105,    -1,    -1,    -1,    -1,
      12,    13,    14,    15,    16,    41,   116,   117,   118,   119,
     120,   121,   122,   123,   124,    -1,    -1,    -1,   128,    -1,
     130,   131,   132,   133,   134,   135,   136,   137,   138,    -1,
      66,    67,    68,    45,    46,    47,    48,    49,    50,    75,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    83,    -1,    85,
      86,    -1,    -1,    -1,    -1,    -1,    -1,    93,    94,    62,
      -1,    -1,    98,    99,    -1,   101,    69,    70,    71,   105,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     116,   117,   118,   119,   120,   121,   122,   123,   124,    -1,
      -1,    -1,   128,    -1,   130,   131,   132,   133,   134,   135,
     136,   137,   138,    -1,   116,   117,   118,    -1,   111,   112,
     113,    -1,   115,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,    -1,    -1,    -1,
      -1,   143,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   145,   146,   147,   148,   149,   150,   151,    -1,
      -1,    -1,    -1,    -1,    -1,   158
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned short int
  BisonParser::yystos_[] =
  {
         0,   177,   178,     0,    81,    91,   179,     7,   180,   181,
      93,    94,   260,     7,     7,     3,     3,   182,   182,     3,
       4,    10,    21,    23,    41,    66,    67,    68,    75,    76,
      83,    85,    86,    98,    99,   101,   105,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   128,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   183,   191,   192,   193,
     198,   199,   200,   208,   210,   225,   246,   247,   260,     4,
     211,    23,    53,    98,    99,   100,   114,   209,     6,     3,
       6,     6,     6,     6,     6,     3,     6,   199,     3,     3,
       6,     3,     3,     3,   193,    10,   208,   210,     6,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      24,    25,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    38,    39,    41,    42,    45,    46,    47,    48,
      49,    50,    51,    52,    54,    55,    56,    57,    60,    61,
      65,    76,    82,    84,    87,    88,    89,    90,    98,   118,
     125,   126,   127,   128,   129,   130,   132,   133,   134,   135,
     136,   143,   149,   158,   212,   213,   215,   216,   221,   222,
     224,   230,   244,   245,   246,   248,    11,    11,    11,   201,
      11,    11,    21,   214,   215,   230,    26,    65,   152,   202,
     203,   240,   240,    96,    97,   168,   172,   173,   261,   262,
     263,   264,   265,   266,   267,   268,   268,   268,   268,   194,
     268,   187,   188,   260,   184,   144,   231,    62,    69,    70,
      71,   111,   112,   113,   115,   145,   146,   147,   148,   149,
     150,   151,   158,   204,   207,   232,   233,   234,   235,   236,
     237,   238,   239,    41,    42,   102,   103,   104,   205,   206,
      91,   106,   107,   108,   161,   251,   252,   254,   255,   260,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     4,     5,   212,     5,   212,   240,   218,   219,   268,
     231,    11,    37,    38,    39,    40,   249,    43,    44,   257,
       6,    11,     5,     6,     4,     5,   262,   262,   268,   165,
     166,   167,   168,   169,   170,   171,   164,   163,   162,   195,
       4,     9,     5,    17,    18,    19,    41,    42,    51,    54,
      98,   130,   132,   133,   134,   135,   136,   149,   158,   185,
     186,     6,     6,     6,     4,     5,   204,     6,     6,     6,
       6,     6,     4,     5,   205,     6,     6,     6,     3,     4,
       9,     4,     5,     4,   255,    95,   139,   226,    95,   140,
     227,    95,   141,   228,   142,   229,   125,   221,   230,   268,
     153,   242,   154,   241,   155,   243,   156,   268,   268,   268,
     268,   268,   268,   268,   159,   250,   268,   268,   217,   223,
     268,   220,   268,   260,   260,    91,   260,   268,   268,   268,
     268,   260,   268,   268,   268,   231,   260,   260,   260,   260,
     260,   260,   212,   212,     5,   204,     6,     6,     6,     9,
       6,     6,     5,   230,   260,   214,   268,   202,   174,   262,
     262,   262,   263,   263,   264,   264,   265,   266,   267,     4,
      21,    63,    64,    72,    73,    74,    76,    77,    78,    79,
      80,   128,   196,   197,   198,   208,   187,    76,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     4,     5,   185,   268,   268,   268,   204,   268,
     268,    92,   268,   260,   205,   160,   253,   268,   268,   256,
     252,   254,     9,   173,   218,   268,   268,   250,   249,   157,
     258,    58,    59,   259,   257,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     5,   196,     6,   226,   227,   228,
     268,   268,   189,   190,   268,   260,   231,   260,   260,   260,
     260,   260,   260,   185,   254,   217,   175,   268,   230,   268,
     268,   268,   268,   268,   268,    91,   268,   196,   268,     9,
     173,     4,   174,   174,   189,   175,   268,   174,   174
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
     425,   426,   427,   428,   429,   430
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned short int
  BisonParser::yyr1_[] =
  {
         0,   176,   177,   178,   178,   179,   180,   179,   179,   181,
     179,   182,   182,   182,   182,   182,   182,   182,   182,   184,
     183,   185,   185,   185,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   187,   187,   188,   188,   189,   189,   190,   190,   190,
     191,   191,   191,   191,   191,   191,   191,   191,   192,   192,
     194,   193,   195,   195,   195,   195,   196,   196,   196,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   198,   198,   199,   199,   199,   200,   201,   200,   200,
     200,   200,   200,   200,   200,   200,   200,   200,   200,   200,
     200,   202,   202,   203,   203,   203,   204,   204,   204,   205,
     205,   205,   206,   206,   206,   206,   206,   207,   207,   207,
     207,   207,   207,   207,   207,   207,   207,   207,   207,   207,
     207,   207,   207,   209,   208,   208,   211,   210,   212,   212,
     212,   212,   212,   212,   213,   213,   213,   213,   213,   213,
     214,   214,   215,   215,   215,   215,   215,   216,   216,   217,
     217,   218,   218,   219,   220,   221,   221,   221,   221,   221,
     221,   221,   221,   221,   221,   221,   221,   221,   221,   221,
     221,   222,   222,   223,   223,   223,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   224,   224,
     224,   224,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   226,   226,   227,   227,   228,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,   238,   239,   240,   241,
     242,   243,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   245,   245,   245,   246,   246,   246,
     246,   246,   246,   246,   246,   246,   247,   247,   247,   247,
     248,   248,   248,   249,   249,   249,   249,   249,   250,   251,
     251,   252,   252,   252,   253,   254,   254,   256,   255,   255,
     257,   257,   257,   258,   259,   259,   260,   260,   261,   261,
     261,   262,   262,   262,   263,   263,   263,   263,   264,   264,
     264,   265,   265,   265,   266,   266,   267,   267,   268,   268
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     3,     0,     6,     3,     0,
       6,     0,     2,     2,     2,     2,     2,     2,     2,     0,
       5,     1,     3,     2,     0,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     3,
       1,     1,     3,     5,     0,     1,     3,     1,     4,     4,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     2,
       0,     5,     0,     2,     2,     2,     1,     3,     2,     3,
       3,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     2,     1,     0,     2,     2,     4,     0,     5,     4,
       4,     4,     3,     4,     4,     4,     4,     4,     4,     4,
       4,     1,     3,     1,     1,     3,     1,     3,     2,     1,
       3,     2,     3,     3,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     3,     1,     1,
       1,     3,     1,     0,     5,     1,     0,     4,     1,     1,
       3,     3,     2,     2,     3,     3,     3,     3,     3,     3,
       1,     3,     1,     1,     1,     1,     1,     3,     3,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     4,     4,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     1,     3,     1,     1,
       3,     3,     3,     3,     1,     1,     3,     0,     5,     2,
       3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     2,     2,     1,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     1,     3,     1,     3,     1,     3
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
  "SPK_SIGNATURE_FILE", "BH_SIGNATURE_FILE", "HEADER_SIGNATURE_FILE",
  "AUTHVALUE", "ENCRVALUE", "CHECKSUMVALUE", "POWNERVALUE", "PTYPEVALUE",
  "KEY_SRC", "CORE", "BH_RSA", "AUTH_HASH", "INT_HASH", "PUFHD_LOC",
  "OPT_KEY", "AUTH_ONLY", "BOOT_DEVICE_TYPE", "DEST_DEVICE_TYPE",
  "DEST_CPU_TYPE", "EXCEPTION_LEVEL_TYPE", "TRUSTZONE_TYPE", "SPLITMODE",
  "DPA_CM", "SPKSELECT", "HASH_FUNCTION", "AUTH_KEY_LEVEL", "OR", "XOR",
  "AND", "MULT", "DIVIDE", "MODULO", "PLUS", "MINUS", "LSHIFT", "RSHIFT",
  "NEGATION", "LPAREN", "RPAREN", "ASTERISK", "$accept", "bif",
  "group_list", "bifoptions", "$@1", "$@2", "file_list", "metahdr_spec",
  "$@3", "metahdr_attr_list", "metahdr_attr", "optional_data",
  "optional_data_attr", "metahdr_blk", "metahdr_blk_attr", "new_pdi_spec",
  "image_spec", "image_list", "$@4", "image_content",
  "image_attributes_list", "image_attributes", "partition_spec",
  "partition_content", "other_spec", "$@5", "sec_boot_attr_list",
  "sec_boot_attr", "fsbl_attr_list", "authjtag_attr_list", "authjtag_attr",
  "fsbl_attr", "file_spec", "$@6", "new_file_spec", "$@7",
  "new_attribute_list", "new_attribute", "attribute_list", "attribute",
  "blocksattr_list", "blocksattr", "bootvectors_list", "bootvector",
  "authblockattr", "boolattr", "trustzone_type", "blockattr", "optattr",
  "other_file_attr", "authvalue", "encrvalue", "checkvalue", "pownervalue",
  "ptypevalue", "key_src", "core", "bh_rsa", "dpa_cm", "auth_hash",
  "int_hash", "pufhd_loc", "opt_key", "auth_only", "boot_device_type",
  "dest_cpu_type", "dest_device_type", "exception_level_type", "numattr",
  "fileattr", "key_file", "rsa_key_file", "other_files", "auth_parameters",
  "spkselect", "lms_params_list", "lms_params_attr", "hashfunc",
  "lms_params_string_list", "lms_params_string_type_list", "$@8",
  "split_options", "splitmode", "splitfmt", "filename", "number",
  "unary_expression", "multiplicative_expression", "additive_expression",
  "shift_expression", "and_expression", "xor_expression", "expression", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BisonParser::rhs_number_type
  BisonParser::yyrhs_[] =
  {
       177,     0,    -1,   178,    -1,    -1,   178,   179,    -1,    81,
       7,   260,    -1,    -1,    91,   180,     7,     3,   182,     4,
      -1,    81,     7,   260,    -1,    -1,    91,   181,     7,     3,
     182,     4,    -1,    -1,   182,   208,    -1,   182,   200,    -1,
     182,   192,    -1,   182,   183,    -1,   182,   191,    -1,   182,
     210,    -1,   182,   198,    -1,    -1,    86,     3,   184,   185,
       4,    -1,   186,    -1,   186,     5,   185,    -1,   186,   185,
      -1,    -1,    18,     6,   227,    -1,    98,     6,   231,    -1,
     130,     6,   260,    -1,    17,     6,   226,    -1,   132,     6,
     260,    -1,   133,     6,   260,    -1,   134,     6,   260,    -1,
     135,     6,   260,    -1,   136,     6,   260,    -1,    54,     6,
     260,    -1,    41,     6,   268,    -1,    42,     6,   268,    -1,
      19,     6,   228,    -1,   158,    -1,    51,     6,   189,    -1,
     149,    -1,   188,    -1,   188,     9,   187,    -1,   260,     5,
      76,     6,   268,    -1,    -1,   190,    -1,   190,     9,   189,
      -1,   268,    -1,   268,   173,   268,   174,    -1,   268,   173,
     175,   174,    -1,    76,     6,   268,    -1,    66,     6,   268,
      -1,    67,     6,   268,    -1,    68,     6,   268,    -1,   225,
       6,   260,    -1,    98,     6,   231,    -1,    21,     6,   230,
      -1,    41,     6,   268,    -1,   193,    -1,   192,   193,    -1,
      -1,    75,     3,   194,   195,     4,    -1,    -1,   195,   196,
      -1,   195,   208,    -1,   195,   198,    -1,   197,    -1,   197,
       5,   196,    -1,   197,   196,    -1,    76,     6,   268,    -1,
      77,     6,    91,    -1,    78,    -1,    79,    -1,   128,    -1,
      80,     6,   268,    -1,    21,     6,   230,    -1,    72,     6,
     268,    -1,    73,     6,   268,    -1,    74,     6,   268,    -1,
      63,     6,   268,    -1,    64,     6,   268,    -1,    83,   199,
      -1,   199,    -1,    -1,   199,   208,    -1,   199,   210,    -1,
      10,    98,    11,   231,    -1,    -1,    10,    99,   201,    11,
     204,    -1,    99,     3,   204,     4,    -1,    23,     3,   202,
       4,    -1,    10,    23,    11,   240,    -1,    23,     6,   240,
      -1,    10,   100,    11,   249,    -1,    10,   114,    11,   257,
      -1,    10,    53,    11,   218,    -1,   101,     3,   205,     4,
      -1,    85,     3,   187,     4,    -1,   105,     3,   251,     4,
      -1,   105,     3,   254,     4,    -1,   105,     3,   255,     4,
      -1,   203,    -1,   203,     5,   202,    -1,   240,    -1,    65,
      -1,    26,     6,   268,    -1,   207,    -1,   207,     5,   204,
      -1,   207,   204,    -1,   206,    -1,   206,     5,   205,    -1,
     206,   205,    -1,    41,     6,   268,    -1,    42,     6,   268,
      -1,   102,     6,    92,    -1,   103,     6,   268,    -1,   104,
       6,   260,    -1,   232,    -1,   233,    -1,   235,    -1,   236,
      -1,   237,    -1,   239,    -1,   238,    -1,   111,    -1,   113,
       6,   268,    -1,   234,    -1,   115,     6,   268,    -1,    69,
      -1,    70,    -1,    71,    -1,   112,     6,   268,    -1,    62,
      -1,    -1,    10,   209,   214,    11,   260,    -1,   260,    -1,
      -1,     3,   211,   212,     4,    -1,   215,    -1,   213,    -1,
     213,     5,   212,    -1,   215,     5,   212,    -1,   213,   212,
      -1,   215,   212,    -1,    84,     6,   260,    -1,    76,     6,
     268,    -1,    65,     6,   268,    -1,    21,     6,   221,    -1,
      21,     6,   125,    -1,    56,     6,    91,    -1,   215,    -1,
     215,     5,   214,    -1,   221,    -1,   224,    -1,   244,    -1,
     245,    -1,   216,    -1,    51,     6,   217,    -1,    52,     6,
     220,    -1,   223,    -1,   223,     9,   217,    -1,   219,    -1,
     219,     5,   218,    -1,   268,    -1,   268,    -1,    12,    -1,
      45,    -1,    46,    -1,    47,    -1,    48,    -1,    49,    -1,
      50,    -1,    14,    -1,    15,    -1,    13,    -1,   128,    -1,
     126,    -1,   246,    -1,   248,    -1,   230,    -1,    16,    -1,
      28,    -1,    28,     6,   156,    -1,   268,    -1,   268,   173,
     268,   174,    -1,   268,   173,   175,   174,    -1,    17,     6,
     226,    -1,    18,     6,   227,    -1,    19,     6,   228,    -1,
      20,     6,   229,    -1,    25,     6,   241,    -1,    24,     6,
     242,    -1,    27,     6,   243,    -1,   130,     6,   260,    -1,
     132,     6,   260,    -1,   133,     6,   260,    -1,   134,     6,
     260,    -1,   135,     6,   260,    -1,    39,     6,   250,    -1,
      38,     6,   268,    -1,   136,     6,   260,    -1,   222,    -1,
      21,     6,   230,    -1,    98,     6,   231,    -1,    41,     6,
     268,    -1,    42,     6,   268,    -1,   158,    -1,    60,     6,
     268,    -1,    61,     6,   268,    -1,   149,    -1,    82,    -1,
      87,    -1,   128,    -1,   246,    -1,   119,    -1,   118,    -1,
     120,    -1,   121,    -1,   122,    -1,   123,    -1,   124,    -1,
      95,    -1,   139,    -1,    95,    -1,   140,    -1,    95,    -1,
     141,    -1,   142,    -1,   143,    -1,   144,    -1,   145,    -1,
     146,    -1,   158,    -1,   147,    -1,   148,    -1,   149,    -1,
     150,    -1,   151,    -1,   152,    -1,   154,    -1,   153,    -1,
     155,    -1,    29,     6,   268,    -1,    30,     6,   268,    -1,
      31,     6,   268,    -1,    32,     6,   268,    -1,    33,     6,
     268,    -1,    34,     6,   268,    -1,    35,    -1,    36,    -1,
      22,     6,   268,    -1,    88,     6,   268,    -1,    89,     6,
     268,    -1,    90,     6,   268,    -1,    54,     6,   260,    -1,
      55,     6,   260,    -1,    57,     6,   260,    -1,   130,    -1,
     247,    -1,   136,    -1,   137,    -1,   138,    -1,   117,    -1,
     116,    -1,   118,    -1,   131,    -1,   132,    -1,   133,    -1,
     134,    -1,   135,    -1,   129,    -1,   125,    -1,   127,    -1,
      37,     6,   268,    -1,    39,     6,   250,    -1,    38,     6,
     268,    -1,    40,    -1,   249,     9,   249,    -1,   159,    -1,
     252,    -1,   252,     9,   252,    -1,   106,     6,   253,    -1,
     108,     6,   268,    -1,   107,     6,   268,    -1,   160,    -1,
      91,    -1,   254,     5,   254,    -1,    -1,   161,     3,   256,
     254,     4,    -1,   255,   255,    -1,    43,     6,   258,    -1,
      44,     6,   259,    -1,   257,     5,   257,    -1,   157,    -1,
      58,    -1,    59,    -1,    93,    -1,    94,    -1,    97,    -1,
      96,    -1,   173,   268,   174,    -1,   261,    -1,   168,   262,
      -1,   172,   262,    -1,   262,    -1,   263,   165,   262,    -1,
     263,   166,   262,    -1,   263,   167,   262,    -1,   263,    -1,
     264,   168,   263,    -1,   264,   169,   263,    -1,   264,    -1,
     265,   170,   264,    -1,   265,   171,   264,    -1,   265,    -1,
     266,   164,   265,    -1,   266,    -1,   267,   163,   266,    -1,
     267,    -1,   268,   162,   267,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     5,     6,     9,    13,    14,    21,    25,
      26,    33,    34,    37,    40,    43,    46,    49,    52,    55,
      56,    62,    64,    68,    71,    72,    76,    80,    84,    88,
      92,    96,   100,   104,   108,   112,   116,   120,   124,   126,
     130,   132,   134,   138,   144,   145,   147,   151,   153,   158,
     163,   167,   171,   175,   179,   183,   187,   191,   195,   197,
     200,   201,   207,   208,   211,   214,   217,   219,   223,   226,
     230,   234,   236,   238,   240,   244,   248,   252,   256,   260,
     264,   268,   271,   273,   274,   277,   280,   285,   286,   292,
     297,   302,   307,   311,   316,   321,   326,   331,   336,   341,
     346,   351,   353,   357,   359,   361,   365,   367,   371,   374,
     376,   380,   383,   387,   391,   395,   399,   403,   405,   407,
     409,   411,   413,   415,   417,   419,   423,   425,   429,   431,
     433,   435,   439,   441,   442,   448,   450,   451,   456,   458,
     460,   464,   468,   471,   474,   478,   482,   486,   490,   494,
     498,   500,   504,   506,   508,   510,   512,   514,   518,   522,
     524,   528,   530,   534,   536,   538,   540,   542,   544,   546,
     548,   550,   552,   554,   556,   558,   560,   562,   564,   566,
     568,   570,   572,   576,   578,   583,   588,   592,   596,   600,
     604,   608,   612,   616,   620,   624,   628,   632,   636,   640,
     644,   648,   650,   654,   658,   662,   666,   668,   672,   676,
     678,   680,   682,   684,   686,   688,   690,   692,   694,   696,
     698,   700,   702,   704,   706,   708,   710,   712,   714,   716,
     718,   720,   722,   724,   726,   728,   730,   732,   734,   736,
     738,   740,   742,   746,   750,   754,   758,   762,   766,   768,
     770,   774,   778,   782,   786,   790,   794,   798,   800,   802,
     804,   806,   808,   810,   812,   814,   816,   818,   820,   822,
     824,   826,   828,   830,   834,   838,   842,   844,   848,   850,
     852,   856,   860,   864,   868,   870,   872,   876,   877,   883,
     886,   890,   894,   898,   900,   902,   904,   906,   908,   910,
     912,   916,   918,   921,   924,   926,   930,   934,   938,   940,
     944,   948,   950,   954,   958,   960,   964,   966,   970,   972
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   212,   212,   214,   215,   217,   218,   218,   230,   231,
     231,   237,   238,   239,   240,   241,   242,   243,   244,   247,
     247,   252,   253,   254,   257,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   276,   277,   278,
     279,   283,   284,   287,   288,   290,   291,   294,   296,   298,
     302,   303,   304,   306,   307,   308,   309,   310,   313,   314,
     317,   317,   326,   327,   328,   329,   332,   333,   334,   337,
     338,   339,   340,   341,   342,   344,   345,   346,   347,   348,
     361,   372,   373,   376,   377,   378,   381,   384,   384,   387,
     388,   389,   394,   395,   396,   397,   398,   399,   400,   401,
     402,   405,   406,   409,   410,   411,   414,   415,   416,   419,
     420,   421,   424,   425,   426,   427,   428,   431,   434,   436,
     438,   440,   442,   444,   446,   448,   450,   452,   456,   457,
     458,   459,   460,   463,   463,   470,   476,   476,   482,   483,
     484,   485,   486,   487,   490,   493,   494,   496,   497,   498,
     503,   504,   507,   508,   509,   510,   511,   514,   515,   518,
     519,   522,   523,   526,   531,   533,   534,   535,   536,   537,
     538,   539,   540,   541,   542,   545,   546,   547,   548,   549,
     550,   553,   554,   557,   558,   559,   561,   562,   563,   564,
     565,   566,   567,   568,   569,   570,   571,   572,   573,   574,
     575,   576,   577,   578,   579,   580,   581,   582,   583,   584,
     585,   586,   589,   590,   591,   592,   593,   594,   595,   596,
     597,   600,   601,   604,   605,   608,   609,   612,   615,   618,
     621,   624,   627,   630,   633,   636,   639,   642,   645,   648,
     651,   654,   657,   658,   659,   660,   661,   662,   663,   664,
     665,   666,   667,   668,   671,   672,   673,   676,   677,   678,
     679,   680,   681,   682,   683,   684,   687,   688,   689,   690,
     693,   694,   695,   698,   701,   704,   707,   710,   713,   717,
     718,   720,   721,   722,   724,   728,   733,   735,   735,   737,
     739,   742,   743,   746,   749,   752,   758,   759,   762,   763,
     764,   770,   771,   772,   776,   777,   778,   779,   782,   783,
     784,   787,   788,   789,   792,   793,   796,   797,   801,   802
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
     175
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 1155;
  const int BisonParser::yynnts_ = 93;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 176;

  const unsigned int BisonParser::yyuser_token_number_max_ = 430;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 24 "parser/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 2765 "bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 807 "parser/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
