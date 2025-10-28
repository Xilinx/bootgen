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

BifOptions* currentBifOptions;
PartitionBifOptions* currentPartitionBifOptions ;
ImageBifOptions* currentImageBifOptions;
LmsKeyTypeParam lmsParams;

/* Line 279 of lalr1.cc  */
#line 56 "bisonflex/bif.tab.cpp"


#include "bif.tab.hpp"

/* User implementation prologue.  */

/* Line 285 of lalr1.cc  */
#line 64 "bisonflex/bif.tab.cpp"
/* Unqualified %code blocks.  */
/* Line 286 of lalr1.cc  */
#line 47 "parser/bif.y"

    static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner);
    #include "options.h"


/* Line 286 of lalr1.cc  */
#line 74 "bisonflex/bif.tab.cpp"


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
#line 170 "bisonflex/bif.tab.cpp"

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
#line 334 "bisonflex/bif.tab.cpp"

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
#line 213 "parser/bif.y"
    { options.includeBifOptionsList.push_back((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 214 "parser/bif.y"
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
#line 223 "parser/bif.y"
    { if (!includefound){options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions);} }
    break;

  case 8:
/* Line 670 of lalr1.cc  */
#line 226 "parser/bif.y"
    { options.includeBifOptionsList.push_back((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 9:
/* Line 670 of lalr1.cc  */
#line 227 "parser/bif.y"
    { currentBifOptions = new BifOptions(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series(), (yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 10:
/* Line 670 of lalr1.cc  */
#line 229 "parser/bif.y"
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
    break;

  case 19:
/* Line 670 of lalr1.cc  */
#line 243 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(), currentBifOptions->GetIdCode()); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 254 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 255 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 256 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 257 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 258 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 259 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 260 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 261 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 262 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 34:
/* Line 670 of lalr1.cc  */
#line 263 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 264 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.partitionRevokeId = (yysemantic_stack_[(3) - (3)].number);}
    break;

  case 36:
/* Line 670 of lalr1.cc  */
#line 265 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkRevokeId = (yysemantic_stack_[(3) - (3)].number); 
                                                                                if(options.GetArchType() == Arch::VERSAL)
                                                                                {
                                                                                   LOG_ERROR("SPK_REVOKE_ID does not support for versal arch, instead please use revoke_id option.");
                                                                                }
                                                                                }
    break;

  case 37:
/* Line 670 of lalr1.cc  */
#line 272 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 38:
/* Line 670 of lalr1.cc  */
#line 273 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 275 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 283 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ihtOptionalDataInfo.push_back(std::pair<std::string, uint32_t>((yysemantic_stack_[(5) - (1)].string), (yysemantic_stack_[(5) - (5)].number))); }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 290 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 292 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 294 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 298 "parser/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 299 "parser/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 300 "parser/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); 
                                                                                  options.SetDl9Series((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 302 "parser/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 303 "parser/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 55:
/* Line 670 of lalr1.cc  */
#line 304 "parser/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 56:
/* Line 670 of lalr1.cc  */
#line 305 "parser/bif.y"
    { currentBifOptions->SetPdiType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 306 "parser/bif.y"
    { currentBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number));}
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 313 "parser/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 314 "parser/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions);
                                                                                  if((options.archType == Arch::SPARTANUP) && (currentBifOptions->imageBifOptionList.size() > 1))
                                                                                  {
                                                                                      LOG_ERROR("'-arch spartanup' cannot accomodate more than one Sub-System.");
                                                                                  }
                                                                                }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 333 "parser/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 334 "parser/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 71:
/* Line 670 of lalr1.cc  */
#line 335 "parser/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 72:
/* Line 670 of lalr1.cc  */
#line 336 "parser/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 337 "parser/bif.y"
    { LOG_ERROR("BIF attribute error !!!\n\t This usage of 'init' is not supported. See 'bootgen -bif_help init' for usage details."); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 338 "parser/bif.y"
    { LOG_ERROR("Copy to Memory feature with the attribute 'copy' is no more supported.\n\t   This can be duplicated with the option 'imagestore'. Please refer UG1283 for more details.");
                                                                                  currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 75:
/* Line 670 of lalr1.cc  */
#line 340 "parser/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 76:
/* Line 670 of lalr1.cc  */
#line 341 "parser/bif.y"
    { currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 77:
/* Line 670 of lalr1.cc  */
#line 342 "parser/bif.y"
    { currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 78:
/* Line 670 of lalr1.cc  */
#line 343 "parser/bif.y"
    { currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 79:
/* Line 670 of lalr1.cc  */
#line 344 "parser/bif.y"
    { if (options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP || (options.GetArchType() == Arch::VERSAL && !options.IsVersalNetSeries()))
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr' is not supported for the specified architecture");
                                                                                  currentImageBifOptions->SetPcrNumber((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 80:
/* Line 670 of lalr1.cc  */
#line 347 "parser/bif.y"
    { if (options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP || (options.GetArchType() == Arch::VERSAL && !options.IsVersalNetSeries()))
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr_mid' is not supported for the specified architecture");
                                                                                  currentImageBifOptions->SetPcrMeasurementIndex((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 86:
/* Line 670 of lalr1.cc  */
#line 361 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQ || options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported for the specified architecture.\n\t   Refer 'bootgen -bif_help' for more details");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 87:
/* Line 670 of lalr1.cc  */
#line 364 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 91:
/* Line 670 of lalr1.cc  */
#line 369 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                  if(options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); 
                                                                                  currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 92:
/* Line 670 of lalr1.cc  */
#line 374 "parser/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); }
    break;

  case 103:
/* Line 670 of lalr1.cc  */
#line 389 "parser/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 390 "parser/bif.y"
    { currentBifOptions->SetBootDevice(BootDevice::IMAGESTORE); }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 391 "parser/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 404 "parser/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 405 "parser/bif.y"
    { currentBifOptions->SetAuthJtagSPKRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 406 "parser/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 407 "parser/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 410 "parser/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 117:
/* Line 670 of lalr1.cc  */
#line 413 "parser/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 118:
/* Line 670 of lalr1.cc  */
#line 415 "parser/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 119:
/* Line 670 of lalr1.cc  */
#line 417 "parser/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 120:
/* Line 670 of lalr1.cc  */
#line 419 "parser/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 421 "parser/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 423 "parser/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 425 "parser/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 427 "parser/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 429 "parser/bif.y"
    {  currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));}
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 431 "parser/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->SetSmapWidth((yysemantic_stack_[(3) - (3)].number));
                                                                                }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 435 "parser/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 436 "parser/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 129:
/* Line 670 of lalr1.cc  */
#line 437 "parser/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 130:
/* Line 670 of lalr1.cc  */
#line 438 "parser/bif.y"
    { currentBifOptions->SetPufRingOscilltorSwapConfigValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 439 "parser/bif.y"
    { currentBifOptions->SetDiceEnable(); }
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 442 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(),options.IsVersalNetSeries(),  currentBifOptions->GetIdCode()); }
    break;

  case 133:
/* Line 670 of lalr1.cc  */
#line 445 "parser/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 134:
/* Line 670 of lalr1.cc  */
#line 449 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 135:
/* Line 670 of lalr1.cc  */
#line 455 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode()); }
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 469 "parser/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 472 "parser/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 473 "parser/bif.y"
    { currentPartitionBifOptions->imageStoreId = (yysemantic_stack_[(3) - (3)].number);
                                                                                  currentPartitionBifOptions->SetPartitionType(PartitionType::IMAGE_STORE_PDI); }
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 476 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 477 "parser/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string), currentImageBifOptions->GetImageType());
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 505 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] is supported only for ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 510 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 512 "parser/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 513 "parser/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 514 "parser/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 515 "parser/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 516 "parser/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 517 "parser/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 518 "parser/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 519 "parser/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 520 "parser/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 521 "parser/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 524 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 525 "parser/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 526 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 527 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 528 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 529 "parser/bif.y"
    { currentPartitionBifOptions->SetLockStepFlag();}
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 532 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 533 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 536 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 537 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 538 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 540 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 541 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 542 "parser/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 543 "parser/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 189:
/* Line 670 of lalr1.cc  */
#line 544 "parser/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 545 "parser/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 191:
/* Line 670 of lalr1.cc  */
#line 546 "parser/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 192:
/* Line 670 of lalr1.cc  */
#line 547 "parser/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 193:
/* Line 670 of lalr1.cc  */
#line 548 "parser/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 194:
/* Line 670 of lalr1.cc  */
#line 549 "parser/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 195:
/* Line 670 of lalr1.cc  */
#line 550 "parser/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 196:
/* Line 670 of lalr1.cc  */
#line 551 "parser/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 197:
/* Line 670 of lalr1.cc  */
#line 552 "parser/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 198:
/* Line 670 of lalr1.cc  */
#line 553 "parser/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 554 "parser/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 201:
/* Line 670 of lalr1.cc  */
#line 556 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 202:
/* Line 670 of lalr1.cc  */
#line 557 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 203:
/* Line 670 of lalr1.cc  */
#line 558 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 204:
/* Line 670 of lalr1.cc  */
#line 559 "parser/bif.y"
    { currentPartitionBifOptions->SetSPKRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 205:
/* Line 670 of lalr1.cc  */
#line 560 "parser/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 206:
/* Line 670 of lalr1.cc  */
#line 561 "parser/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 207:
/* Line 670 of lalr1.cc  */
#line 562 "parser/bif.y"
    { currentPartitionBifOptions->SetClusterNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 208:
/* Line 670 of lalr1.cc  */
#line 563 "parser/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 209:
/* Line 670 of lalr1.cc  */
#line 564 "parser/bif.y"
    { currentPartitionBifOptions->SetDelayAuth(true); }
    break;

  case 210:
/* Line 670 of lalr1.cc  */
#line 565 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBootFlag(); }
    break;

  case 219:
/* Line 670 of lalr1.cc  */
#line 578 "parser/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 221:
/* Line 670 of lalr1.cc  */
#line 582 "parser/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 223:
/* Line 670 of lalr1.cc  */
#line 586 "parser/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 240:
/* Line 670 of lalr1.cc  */
#line 635 "parser/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 241:
/* Line 670 of lalr1.cc  */
#line 636 "parser/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 242:
/* Line 670 of lalr1.cc  */
#line 637 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), false); }
    break;

  case 243:
/* Line 670 of lalr1.cc  */
#line 638 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), true); }
    break;

  case 244:
/* Line 670 of lalr1.cc  */
#line 639 "parser/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 245:
/* Line 670 of lalr1.cc  */
#line 640 "parser/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 246:
/* Line 670 of lalr1.cc  */
#line 641 "parser/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 247:
/* Line 670 of lalr1.cc  */
#line 642 "parser/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 248:
/* Line 670 of lalr1.cc  */
#line 643 "parser/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 249:
/* Line 670 of lalr1.cc  */
#line 644 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmARegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 250:
/* Line 670 of lalr1.cc  */
#line 645 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 251:
/* Line 670 of lalr1.cc  */
#line 646 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmCRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 252:
/* Line 670 of lalr1.cc  */
#line 649 "parser/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 253:
/* Line 670 of lalr1.cc  */
#line 650 "parser/bif.y"
    { currentPartitionBifOptions->acFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 254:
/* Line 670 of lalr1.cc  */
#line 651 "parser/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 271:
/* Line 670 of lalr1.cc  */
#line 676 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 272:
/* Line 670 of lalr1.cc  */
#line 679 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 273:
/* Line 670 of lalr1.cc  */
#line 682 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 274:
/* Line 670 of lalr1.cc  */
#line 685 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 283:
/* Line 670 of lalr1.cc  */
#line 706 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP || options.GetArchType() == Arch::VERSAL)
                                                                   LOG_ERROR("BIF attribute error !!!\n\t\t'lms_key_params' is not supported with the mentioned -arch");
                                                                 lmsParams.lms_param = (yysemantic_stack_[(1) - (1)].string);
                                                                 currentBifOptions->SetLmsParameters(lmsParams) ;
                                                               }
    break;

  case 285:
/* Line 670 of lalr1.cc  */
#line 713 "parser/bif.y"
    { lmsParams.type = (yysemantic_stack_[(2) - (1)].authkeylevel_t);}
    break;

  case 288:
/* Line 670 of lalr1.cc  */
#line 717 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 292:
/* Line 670 of lalr1.cc  */
#line 727 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 293:
/* Line 670 of lalr1.cc  */
#line 730 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 298:
/* Line 670 of lalr1.cc  */
#line 742 "parser/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 300:
/* Line 670 of lalr1.cc  */
#line 749 "parser/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 301:
/* Line 670 of lalr1.cc  */
#line 750 "parser/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 303:
/* Line 670 of lalr1.cc  */
#line 755 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 304:
/* Line 670 of lalr1.cc  */
#line 756 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 305:
/* Line 670 of lalr1.cc  */
#line 757 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 307:
/* Line 670 of lalr1.cc  */
#line 761 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 308:
/* Line 670 of lalr1.cc  */
#line 762 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 310:
/* Line 670 of lalr1.cc  */
#line 766 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 311:
/* Line 670 of lalr1.cc  */
#line 767 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 313:
/* Line 670 of lalr1.cc  */
#line 771 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 315:
/* Line 670 of lalr1.cc  */
#line 775 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 317:
/* Line 670 of lalr1.cc  */
#line 780 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1564 "bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -465;
  const short int
  BisonParser::yypact_[] =
  {
      -465,    14,    35,  -465,    74,  -465,  -465,     1,    94,   102,
    -465,  -465,  -465,   128,   131,  -465,  -465,   884,   969,  -465,
    -465,   213,   138,   126,   177,   222,   233,   241,   196,   242,
    -465,   246,   249,   248,   252,   254,   255,  -465,  -465,  -465,
    -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,
    -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,   192,  -465,
    -465,    79,  -465,  -465,  -465,   262,  -465,  -465,  -465,  -465,
     641,   259,   260,   265,  -465,   266,   270,   787,   141,    -5,
     135,    46,    46,    46,    46,  -465,    46,    79,     1,  -465,
     147,  1010,   -17,   109,  -465,  -465,  -465,  -465,     1,  -465,
    -465,  -465,  -465,  -465,   288,   289,   293,   302,   303,   308,
     309,   310,   311,   312,   314,   315,   317,   318,   322,   323,
    -465,  -465,   324,   326,   332,   333,  -465,  -465,  -465,  -465,
    -465,  -465,   334,   336,   337,   338,   339,   340,   342,   343,
     346,   349,  -465,   350,  -465,   352,   354,   355,   360,  -465,
    -465,  -465,  -465,  -465,   361,   363,   364,   365,   366,   370,
    -465,  -465,  -465,   378,    23,   495,  -465,  -465,  -465,  -465,
    -465,  -465,  -465,  -465,  -465,   135,    46,   147,   375,   193,
      93,   389,   390,   348,  -465,   396,  -465,  -465,   399,   400,
    -465,  -465,  -465,  -465,    46,    46,    46,  -465,  -465,   -41,
       2,     6,   243,   250,   253,   253,   253,   253,  -465,   253,
     411,   407,   412,   256,  -465,  -465,  -465,  -465,  -465,  -465,
    -465,   416,   423,   425,  -465,  -465,  -465,  -465,  -465,  -465,
    -465,  -465,   414,   115,  -465,  -465,  -465,  -465,  -465,  -465,
    -465,  -465,   426,   427,   428,   429,   432,    25,  -465,   433,
     436,   437,   441,   442,   439,   197,     3,  -465,   -46,   -49,
     -77,   304,  1011,    46,   297,   306,   296,   305,    46,    46,
      46,    46,    46,    46,    46,   299,    46,    46,    46,    46,
       1,     1,   371,     1,    46,    46,    46,    46,     1,    46,
      46,    46,   147,     1,     1,     1,     1,     1,     1,  -465,
     641,  -465,   641,  -465,  -465,  -465,   459,   253,  -465,  1010,
     460,   461,   462,  -465,   456,   463,   464,   466,   141,     1,
     787,    46,  -465,    -5,  -465,  -465,  -144,    46,    46,    46,
      46,    46,    46,    46,    46,    46,    46,   347,  -465,     1,
     397,   469,   470,   471,   472,   473,   474,   475,   476,   477,
     478,   479,   480,   481,   482,  -465,  -465,   468,   202,    46,
      46,    46,  -465,  1010,  -465,    46,    46,   398,    46,  -465,
     -17,  -465,   330,    46,    46,  -465,  -465,   117,  -465,   401,
    -465,   331,  -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,
    -465,  -465,  -465,  -465,  -465,  -465,   253,  -465,  -465,  -465,
    -465,  -465,  -465,  -465,   253,   253,   253,   253,   253,   253,
     253,  -465,  -465,   253,   253,  -465,   484,  -141,  -465,   253,
    -465,  -465,  -465,  -465,   253,   253,   253,   253,  -465,   253,
     253,   253,  -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,
    -465,    46,  -465,    46,    46,   299,   193,   341,   153,    93,
    -465,  -465,  -465,   253,  -465,  -465,  -465,  -465,  -465,   -41,
     -41,     2,     2,     6,   243,   250,  -465,   490,   492,   512,
     515,   526,   529,   532,   533,  -465,  -465,   542,  -465,  -465,
     320,  -465,  -465,  -465,   547,   -46,   -49,   -77,    46,    46,
      46,     1,   147,     1,     1,     1,     1,     1,     1,  -465,
     256,  -465,   253,   253,   253,  -465,   253,   253,  -465,   253,
    -465,  -465,  -465,   253,   253,   401,  -465,   549,    46,   -74,
    -465,   253,   253,  -465,   456,  -465,  -465,  -465,  -465,  -465,
     466,   141,    46,    46,    46,    46,    46,    46,   467,    46,
     301,  -465,    46,  -465,  -465,  -465,   253,   253,  -465,   548,
     -39,  -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,  -465,
     230,  -465,   386,   -59,  -465,   253,   253,   253,   253,   253,
     253,  -465,   253,  -465,   253,    46,     7,  -465,  -465,  -465,
    -465,   388,   -43,  -465,  -465
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,     6,     4,     0,     0,     0,
     294,   295,     5,     0,     0,    11,    11,     0,     0,   135,
       7,   132,     0,     0,     0,     0,     0,     0,     0,     0,
      83,     0,     0,     0,     0,     0,     0,   261,   260,   262,
     213,   214,   215,   216,   217,   218,   211,   255,   263,   264,
     265,   266,   267,   257,   258,   259,    15,    16,    14,    58,
      18,    82,    13,    12,    17,     0,   212,   256,   134,    10,
       0,     0,     0,     0,    87,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    60,     0,    81,    44,    19,
       0,     0,     0,     0,    59,   132,    84,    85,     0,   164,
     173,   171,   172,   179,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   180,     0,     0,     0,     0,     0,     0,
     246,   247,     0,     0,     0,     0,   165,   166,   167,   168,
     169,   170,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   209,     0,   210,     0,     0,     0,     0,   269,
     175,   270,   174,   268,   255,   264,   265,   266,   267,   257,
     226,   208,   205,     0,   138,   137,   155,   151,   200,   152,
     178,   153,   154,   176,   177,     0,     0,     0,     0,     0,
       0,     0,     0,   149,    56,     0,   104,   236,     0,   101,
     103,    92,   297,   296,     0,     0,     0,   299,   302,   306,
     309,   312,   314,   316,    57,    51,    52,    53,    62,    50,
       0,    41,     0,    24,   227,    55,   131,   127,   128,   129,
     123,     0,     0,     0,   228,   229,   231,   232,   233,   234,
     235,   230,     0,   106,   116,   117,   125,   118,   119,   120,
     122,   121,     0,     0,     0,     0,     0,   109,   283,     0,
       0,     0,     0,     0,   277,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   136,
       0,   141,     0,   142,    91,    95,   160,   162,    86,     0,
       0,     0,     0,   274,    93,     0,     0,    94,     0,     0,
       0,     0,    90,     0,   300,   301,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    83,    97,    44,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    40,    38,     0,    21,     0,
       0,     0,    89,     0,   108,     0,     0,     0,     0,    96,
       0,   111,     0,     0,     0,   285,    98,     0,    99,     0,
     100,   287,   219,   220,   185,   221,   222,   186,   223,   224,
     187,   225,   188,   147,   146,   178,   248,   238,   190,   237,
     189,   239,   191,   181,   240,   241,   242,   243,   244,   245,
     198,   276,   197,   203,   204,   156,   158,   182,   157,   163,
     252,   253,   148,   254,   206,   207,   145,   144,   143,   249,
     250,   251,   202,   192,   193,   194,   195,   196,   199,   139,
     140,     0,    88,     0,     0,     0,     0,     0,     0,     0,
     201,   133,   150,   105,   102,   298,   303,   304,   305,   307,
     308,   310,   311,   313,   315,   317,    61,     0,     0,     0,
       0,     0,     0,     0,     0,    71,    72,     0,    73,    63,
      66,    65,    64,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    20,
      24,    23,   130,   124,   126,   107,   112,   113,   114,   115,
     110,   282,   279,   281,   280,     0,   278,   284,     0,     0,
     161,   271,   273,   272,   275,   291,   288,   292,   293,   289,
     290,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    68,     0,    28,    25,    37,    35,    36,    39,    45,
      47,    34,    26,    27,    29,    30,    31,    32,    33,    22,
       0,   159,     0,     0,    75,    79,    80,    76,    77,    78,
      69,    70,    74,    67,    43,     0,     0,   286,   184,   183,
      46,     0,     0,    49,    48
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -465,  -465,  -465,  -465,  -465,  -465,   546,  -465,  -465,  -339,
    -465,   224,  -465,   -11,  -465,  -465,  -465,   507,  -465,  -465,
    -464,  -465,   229,   537,  -465,  -465,   245,  -465,  -222,  -235,
    -465,  -465,   -55,  -465,    30,  -465,  -138,  -465,   258,   -69,
    -465,    51,   129,  -465,  -465,   313,  -465,  -465,  -465,  -465,
      87,    88,    86,  -465,   -68,  -173,  -465,  -465,  -465,  -465,
    -465,  -465,  -465,  -465,   -67,  -465,  -465,  -465,  -465,  -465,
     220,  -465,  -465,   130,   136,  -465,   203,  -465,  -370,   493,
    -465,   139,  -465,  -465,     8,  -465,  -161,   -89,   -87,   257,
     261,   251,   -81
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     6,     8,     9,    17,    56,   213,   357,
     358,   210,   211,   548,   549,    57,    58,    59,   208,   337,
     479,   480,    60,    61,    62,   178,   188,   189,   232,   246,
     247,   233,    63,    77,    64,    70,   163,   164,   182,   165,
     166,   415,   305,   306,   418,   167,   168,   416,   169,    65,
     384,   387,   390,   392,   170,   215,   234,   235,   236,   237,
     238,   239,   240,   241,   190,   400,   398,   402,   171,   172,
     173,    67,   174,   314,   412,   253,   254,   512,   255,   381,
     515,   317,   526,   529,    68,   197,   198,   199,   200,   201,
     202,   203,   307
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       204,   205,   206,   207,   308,   209,    96,   380,   183,   517,
     184,   364,   371,   191,     3,    12,   541,   336,   388,   501,
     336,   185,   192,   193,   242,   243,   301,   303,   300,   455,
     370,   519,    96,   324,   325,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   385,   110,   111,   382,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     186,   122,   123,   389,   124,   125,   242,   243,   126,   127,
     128,   129,   130,   131,   132,   133,   573,   134,   135,   136,
     137,     7,    19,   138,   139,   244,   245,   442,   140,    95,
     386,    97,   383,   194,    10,    11,   212,   195,   196,   141,
     562,    13,   336,   192,   193,   142,   257,   143,   304,    14,
     144,   145,   146,   147,   579,   326,     4,    97,   336,   432,
     363,   148,   336,   327,   328,   329,     5,   244,   245,    79,
     584,    15,    80,   576,    16,   510,   315,   316,    37,    38,
      39,   505,   192,   193,    78,   560,   187,   149,   150,   151,
     152,   153,   154,    48,   155,   156,   157,   158,   159,    54,
      55,   559,   439,   252,   440,   160,   456,   457,   458,   330,
     331,   161,    10,    11,   194,   332,   333,   216,   195,   196,
     162,   581,   396,    81,   217,   218,   219,   404,   405,   406,
     407,   408,   409,   410,   395,   413,   414,   417,   419,    85,
     248,   378,   379,   424,   425,   426,   427,   500,   429,   430,
     431,   527,   528,   194,   249,   250,   251,   195,   196,   341,
     342,   343,   249,   250,   251,   220,   221,   222,    82,   223,
     310,   311,   312,   313,   577,   379,    71,    66,    66,    83,
     453,   459,   460,   344,   345,   461,   462,    84,    86,    88,
     450,   183,    89,   346,    90,    91,   347,    92,    93,   224,
     225,   226,   227,   228,   229,   230,    72,    28,    98,   252,
     175,   176,   231,   341,   342,   343,   177,   179,   502,   503,
     504,   180,   482,   160,   506,   507,   187,   509,   420,   421,
     214,   423,   513,   514,   258,   259,   428,   344,   345,   260,
     348,   433,   434,   435,   436,   437,   438,   346,   261,   262,
     347,    73,    74,    75,   263,   264,   265,   266,   267,   552,
     268,   269,   467,   270,   271,   540,    76,   451,   272,   273,
     274,   349,   275,   350,   351,   352,   353,   354,   276,   277,
     278,   467,   279,   280,   281,   282,   283,   212,   284,   285,
     355,   466,   286,   320,   348,   287,   288,    95,   289,   356,
     290,   291,   521,   522,   468,   469,   292,   293,   467,   294,
     295,   296,   297,   470,   471,   472,   298,   473,   474,   475,
     476,   477,   299,   468,   469,   349,   309,   350,   351,   352,
     353,   354,   470,   471,   472,   318,   473,   474,   475,   476,
     477,   319,   321,   322,   355,   323,   334,   546,   547,   550,
     468,   469,   335,   356,   336,   338,   339,   340,   362,   470,
     471,   472,   359,   473,   474,   475,   476,   477,   478,   360,
      30,   361,   365,   366,   367,   368,   369,   417,   563,   372,
      10,    11,   373,   374,   375,   391,   376,   478,   377,   397,
     401,   565,   566,   567,   568,   569,   570,   411,   572,   399,
     403,   574,   422,   564,   441,   446,   443,   444,   445,   447,
     448,   449,   499,   484,   478,   485,   486,   487,   488,   489,
     490,   491,   492,   493,   494,   495,   496,   497,   498,   511,
     508,   252,   248,   518,   550,   582,   531,   525,   532,   551,
     302,   553,   554,   555,   556,   557,   558,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   533,   110,
     111,   534,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   535,   122,   123,   536,   124,   125,   537,   538,
     126,   127,   128,   129,   130,   131,   132,   133,   539,   134,
     135,   136,   137,   542,   379,   138,   139,   575,   571,   578,
     140,   583,    18,   483,   580,    94,   481,    87,   454,   561,
     520,   141,   543,   545,   544,   394,   524,   142,   452,   143,
     516,   523,   144,   145,   146,   147,   256,   465,   530,     0,
       0,   463,     0,   148,     0,     0,   464,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      37,    38,    39,     0,     0,     0,     0,     0,     0,   149,
     150,   151,   152,   153,   154,    48,   155,   156,   157,   158,
     159,    54,    55,     0,     0,     0,     0,   160,     0,     0,
       0,     0,     0,   161,     0,     0,     0,     0,     0,     0,
       0,     0,   162,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,     0,   110,   111,     0,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,     0,   122,
     123,     0,   124,   125,     0,     0,   126,   127,   128,   129,
     130,   131,   132,   133,     0,   134,   135,   136,   137,     0,
       0,   138,   139,     0,     0,     0,   140,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   141,     0,     0,
       0,     0,     0,   142,     0,   143,     0,     0,   144,   145,
     146,   147,     0,     0,     0,     0,     0,     0,     0,   148,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    37,    38,    39,     0,
       0,     0,     0,     0,     0,   149,   150,   151,   152,   153,
     154,    48,   155,   156,   157,   158,   159,    54,    55,     0,
       0,     0,     0,   160,     0,     0,     0,     0,     0,   161,
       0,     0,     0,     0,     0,     0,     0,     0,   162,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   181,   109,
       0,   110,   111,     0,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,     0,   122,   123,     0,   124,   125,
       0,     0,   126,   127,   128,   129,   130,   131,   132,   133,
       0,   134,   135,     0,   137,     0,     0,   138,   139,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   142,
       0,     0,     0,     0,   144,   145,   146,   147,     0,     0,
       0,     0,     0,     0,     0,   148,     0,    19,    20,     0,
       0,     0,     0,     0,    21,     0,     0,     0,     0,     0,
       0,     0,    37,    38,    39,    22,     0,    23,     0,     0,
       0,   149,   150,   151,   152,   153,   154,    48,   155,   156,
     157,   158,   159,    54,    55,    24,     0,     0,     0,   160,
       0,     0,     0,     0,     0,   161,     0,     0,     0,     0,
       0,     0,     0,     0,   162,     0,     0,     0,     0,     0,
      25,    26,    27,     0,     0,     0,     0,     0,     0,    28,
      29,     0,     0,     0,     0,     0,     0,    30,     0,    31,
      32,     0,    19,    69,     0,     0,     0,    10,    11,    21,
       0,     0,    33,    34,     0,    35,     0,     0,    36,     0,
      22,     0,    23,     0,     0,     0,     0,     0,     0,    37,
      38,    39,    40,    41,    42,    43,    44,    45,     0,     0,
      24,    46,     0,    47,    48,    49,    50,    51,    52,    53,
      54,    55,     0,    99,   100,   101,   102,   103,     0,     0,
       0,     0,     0,     0,     0,    25,    26,    27,     0,     0,
       0,     0,     0,     0,    28,    29,     0,     0,     0,     0,
       0,     0,    30,     0,    31,    32,   126,   127,   128,   129,
     130,   131,    10,    11,     0,     0,     0,    33,    34,     0,
      35,     0,   216,    36,     0,     0,     0,     0,     0,   217,
     218,   219,     0,     0,    37,    38,    39,    40,    41,    42,
      43,    44,    45,     0,     0,     0,    46,     0,    47,    48,
      49,    50,    51,    52,    53,    54,    55,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     220,   221,   222,     0,   223,     0,    37,    38,    39,     0,
       0,     0,     0,     0,     0,   393,   150,   151,   152,   153,
      47,    48,    49,    50,    51,    52,    53,    54,    55,     0,
       0,     0,     0,   160,   224,   225,   226,   227,   228,   229,
     230,     0,     0,     0,     0,     0,     0,   231
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        81,    82,    83,    84,   177,    86,    61,     4,    77,   379,
      78,   233,   247,    80,     0,     7,   480,   161,    95,   358,
     161,    26,    96,    97,    41,    42,   164,   165,     5,   173,
       5,   172,    87,   194,   195,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    95,    24,    25,    95,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      65,    38,    39,   140,    41,    42,    41,    42,    45,    46,
      47,    48,    49,    50,    51,    52,   540,    54,    55,    56,
      57,     7,     3,    60,    61,   102,   103,   309,    65,    10,
     139,    61,   138,   167,    93,    94,    88,   171,   172,    76,
     174,     7,   161,    96,    97,    82,    98,    84,   175,     7,
      87,    88,    89,    90,   173,   196,    81,    87,   161,   292,
       5,    98,   161,   164,   165,   166,    91,   102,   103,     3,
     173,     3,     6,   172,     3,   370,    43,    44,   115,   116,
     117,   363,    96,    97,     6,   515,   151,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   500,   300,   160,   302,   142,   327,   328,   329,   167,
     168,   148,    93,    94,   167,   169,   170,    62,   171,   172,
     157,   174,   263,     6,    69,    70,    71,   268,   269,   270,
     271,   272,   273,   274,   262,   276,   277,   278,   279,     3,
      91,     4,     5,   284,   285,   286,   287,     5,   289,   290,
     291,    58,    59,   167,   105,   106,   107,   171,   172,    17,
      18,    19,   105,   106,   107,   110,   111,   112,     6,   114,
      37,    38,    39,    40,     4,     5,    23,    17,    18,     6,
     321,   330,   331,    41,    42,   332,   333,     6,     6,     3,
     318,   320,     3,    51,     6,     3,    54,     3,     3,   144,
     145,   146,   147,   148,   149,   150,    53,    75,     6,   160,
      11,    11,   157,    17,    18,    19,    11,    11,   359,   360,
     361,    11,   337,   142,   365,   366,   151,   368,   280,   281,
     143,   283,   373,   374,     6,     6,   288,    41,    42,     6,
      98,   293,   294,   295,   296,   297,   298,    51,     6,     6,
      54,    98,    99,   100,     6,     6,     6,     6,     6,   492,
       6,     6,    21,     6,     6,     5,   113,   319,     6,     6,
       6,   129,     6,   131,   132,   133,   134,   135,     6,     6,
       6,    21,     6,     6,     6,     6,     6,   339,     6,     6,
     148,     4,     6,     5,    98,     6,     6,    10,     6,   157,
       6,     6,   443,   444,    63,    64,     6,     6,    21,     6,
       6,     6,     6,    72,    73,    74,     6,    76,    77,    78,
      79,    80,     4,    63,    64,   129,    11,   131,   132,   133,
     134,   135,    72,    73,    74,     6,    76,    77,    78,    79,
      80,    11,     6,     4,   148,     5,   163,   488,   489,   490,
      63,    64,   162,   157,   161,     4,     9,     5,     4,    72,
      73,    74,     6,    76,    77,    78,    79,    80,   127,     6,
      83,     6,     6,     6,     6,     6,     4,   518,   519,     6,
      93,    94,     6,     6,     3,   141,     4,   127,     9,   152,
     154,   532,   533,   534,   535,   536,   537,   158,   539,   153,
     155,   542,    91,   531,     5,     9,     6,     6,     6,     6,
       6,     5,     4,    76,   127,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,   159,
      92,   160,    91,     9,   575,   576,     6,   156,     6,   491,
       5,   493,   494,   495,   496,   497,   498,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,     6,    24,
      25,     6,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     6,    38,    39,     6,    41,    42,     6,     6,
      45,    46,    47,    48,    49,    50,    51,    52,     6,    54,
      55,    56,    57,     6,     5,    60,    61,     9,    91,   173,
      65,   173,    16,   339,   575,    58,   337,    30,   323,   518,
     441,    76,   485,   487,   486,   262,   446,    82,   320,    84,
     377,   445,    87,    88,    89,    90,    93,   336,   449,    -1,
      -1,   334,    -1,    98,    -1,    -1,   335,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     115,   116,   117,    -1,    -1,    -1,    -1,    -1,    -1,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,    -1,    -1,    -1,    -1,   142,    -1,    -1,
      -1,    -1,    -1,   148,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   157,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    -1,    24,    25,    -1,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    -1,    38,
      39,    -1,    41,    42,    -1,    -1,    45,    46,    47,    48,
      49,    50,    51,    52,    -1,    54,    55,    56,    57,    -1,
      -1,    60,    61,    -1,    -1,    -1,    65,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,
      -1,    -1,    -1,    82,    -1,    84,    -1,    -1,    87,    88,
      89,    90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   115,   116,   117,    -1,
      -1,    -1,    -1,    -1,    -1,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,    -1,
      -1,    -1,    -1,   142,    -1,    -1,    -1,    -1,    -1,   148,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   157,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      -1,    24,    25,    -1,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    -1,    38,    39,    -1,    41,    42,
      -1,    -1,    45,    46,    47,    48,    49,    50,    51,    52,
      -1,    54,    55,    -1,    57,    -1,    -1,    60,    61,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,
      -1,    -1,    -1,    -1,    87,    88,    89,    90,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    98,    -1,     3,     4,    -1,
      -1,    -1,    -1,    -1,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   115,   116,   117,    21,    -1,    23,    -1,    -1,
      -1,   124,   125,   126,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,    41,    -1,    -1,    -1,   142,
      -1,    -1,    -1,    -1,    -1,   148,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   157,    -1,    -1,    -1,    -1,    -1,
      66,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    75,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    83,    -1,    85,
      86,    -1,     3,     4,    -1,    -1,    -1,    93,    94,    10,
      -1,    -1,    98,    99,    -1,   101,    -1,    -1,   104,    -1,
      21,    -1,    23,    -1,    -1,    -1,    -1,    -1,    -1,   115,
     116,   117,   118,   119,   120,   121,   122,   123,    -1,    -1,
      41,   127,    -1,   129,   130,   131,   132,   133,   134,   135,
     136,   137,    -1,    12,    13,    14,    15,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    66,    67,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    75,    76,    -1,    -1,    -1,    -1,
      -1,    -1,    83,    -1,    85,    86,    45,    46,    47,    48,
      49,    50,    93,    94,    -1,    -1,    -1,    98,    99,    -1,
     101,    -1,    62,   104,    -1,    -1,    -1,    -1,    -1,    69,
      70,    71,    -1,    -1,   115,   116,   117,   118,   119,   120,
     121,   122,   123,    -1,    -1,    -1,   127,    -1,   129,   130,
     131,   132,   133,   134,   135,   136,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     110,   111,   112,    -1,   114,    -1,   115,   116,   117,    -1,
      -1,    -1,    -1,    -1,    -1,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   136,   137,    -1,
      -1,    -1,    -1,   142,   144,   145,   146,   147,   148,   149,
     150,    -1,    -1,    -1,    -1,    -1,    -1,   157
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned short int
  BisonParser::yystos_[] =
  {
         0,   176,   177,     0,    81,    91,   178,     7,   179,   180,
      93,    94,   259,     7,     7,     3,     3,   181,   181,     3,
       4,    10,    21,    23,    41,    66,    67,    68,    75,    76,
      83,    85,    86,    98,    99,   101,   104,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   127,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   182,   190,   191,   192,
     197,   198,   199,   207,   209,   224,   245,   246,   259,     4,
     210,    23,    53,    98,    99,   100,   113,   208,     6,     3,
       6,     6,     6,     6,     6,     3,     6,   198,     3,     3,
       6,     3,     3,     3,   192,    10,   207,   209,     6,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      24,    25,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    38,    39,    41,    42,    45,    46,    47,    48,
      49,    50,    51,    52,    54,    55,    56,    57,    60,    61,
      65,    76,    82,    84,    87,    88,    89,    90,    98,   124,
     125,   126,   127,   128,   129,   131,   132,   133,   134,   135,
     142,   148,   157,   211,   212,   214,   215,   220,   221,   223,
     229,   243,   244,   245,   247,    11,    11,    11,   200,    11,
      11,    21,   213,   214,   229,    26,    65,   151,   201,   202,
     239,   239,    96,    97,   167,   171,   172,   260,   261,   262,
     263,   264,   265,   266,   267,   267,   267,   267,   193,   267,
     186,   187,   259,   183,   143,   230,    62,    69,    70,    71,
     110,   111,   112,   114,   144,   145,   146,   147,   148,   149,
     150,   157,   203,   206,   231,   232,   233,   234,   235,   236,
     237,   238,    41,    42,   102,   103,   204,   205,    91,   105,
     106,   107,   160,   250,   251,   253,   254,   259,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     4,
       5,   211,     5,   211,   239,   217,   218,   267,   230,    11,
      37,    38,    39,    40,   248,    43,    44,   256,     6,    11,
       5,     6,     4,     5,   261,   261,   267,   164,   165,   166,
     167,   168,   169,   170,   163,   162,   161,   194,     4,     9,
       5,    17,    18,    19,    41,    42,    51,    54,    98,   129,
     131,   132,   133,   134,   135,   148,   157,   184,   185,     6,
       6,     6,     4,     5,   203,     6,     6,     6,     6,     4,
       5,   204,     6,     6,     6,     3,     4,     9,     4,     5,
       4,   254,    95,   138,   225,    95,   139,   226,    95,   140,
     227,   141,   228,   124,   220,   229,   267,   152,   241,   153,
     240,   154,   242,   155,   267,   267,   267,   267,   267,   267,
     267,   158,   249,   267,   267,   216,   222,   267,   219,   267,
     259,   259,    91,   259,   267,   267,   267,   267,   259,   267,
     267,   267,   230,   259,   259,   259,   259,   259,   259,   211,
     211,     5,   203,     6,     6,     6,     9,     6,     6,     5,
     229,   259,   213,   267,   201,   173,   261,   261,   261,   262,
     262,   263,   263,   264,   265,   266,     4,    21,    63,    64,
      72,    73,    74,    76,    77,    78,    79,    80,   127,   195,
     196,   197,   207,   186,    76,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     4,
       5,   184,   267,   267,   267,   203,   267,   267,    92,   267,
     204,   159,   252,   267,   267,   255,   251,   253,     9,   172,
     217,   267,   267,   249,   248,   156,   257,    58,    59,   258,
     256,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       5,   195,     6,   225,   226,   227,   267,   267,   188,   189,
     267,   259,   230,   259,   259,   259,   259,   259,   259,   184,
     253,   216,   174,   267,   229,   267,   267,   267,   267,   267,
     267,    91,   267,   195,   267,     9,   172,     4,   173,   173,
     188,   174,   267,   173,   173
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
     425,   426,   427,   428,   429
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned short int
  BisonParser::yyr1_[] =
  {
         0,   175,   176,   177,   177,   178,   179,   178,   178,   180,
     178,   181,   181,   181,   181,   181,   181,   181,   181,   183,
     182,   184,   184,   184,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   186,   186,   187,   187,   188,   188,   189,   189,   189,
     190,   190,   190,   190,   190,   190,   190,   190,   191,   191,
     193,   192,   194,   194,   194,   194,   195,   195,   195,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   197,   197,   198,   198,   198,   199,   200,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     199,   201,   201,   202,   202,   202,   203,   203,   203,   204,
     204,   204,   205,   205,   205,   205,   206,   206,   206,   206,
     206,   206,   206,   206,   206,   206,   206,   206,   206,   206,
     206,   206,   208,   207,   207,   210,   209,   211,   211,   211,
     211,   211,   211,   212,   212,   212,   212,   212,   212,   213,
     213,   214,   214,   214,   214,   214,   215,   215,   216,   216,
     217,   217,   218,   219,   220,   220,   220,   220,   220,   220,
     220,   220,   220,   220,   220,   220,   220,   220,   220,   220,
     221,   221,   222,   222,   222,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   223,   223,   223,   223,   223,   223,   223,   223,   223,
     223,   224,   224,   224,   224,   224,   224,   224,   224,   225,
     225,   226,   226,   227,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   243,   243,   243,   243,   243,   243,   243,   243,   243,
     243,   243,   244,   244,   244,   245,   245,   245,   245,   245,
     245,   245,   245,   245,   246,   246,   246,   246,   247,   247,
     247,   248,   248,   248,   248,   248,   249,   250,   250,   251,
     251,   251,   252,   253,   253,   255,   254,   254,   256,   256,
     256,   257,   258,   258,   259,   259,   260,   260,   260,   261,
     261,   261,   262,   262,   262,   262,   263,   263,   263,   264,
     264,   264,   265,   265,   266,   266,   267,   267
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
       3,     2,     3,     3,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     1,     1,     1,
       3,     1,     0,     5,     1,     0,     4,     1,     1,     3,
       3,     2,     2,     3,     3,     3,     3,     3,     3,     1,
       3,     1,     1,     1,     1,     1,     3,     3,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     4,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     3,     3,     3,     3,     1,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     1,     3,     1,     1,     3,     3,
       3,     3,     1,     1,     3,     0,     5,     2,     3,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       2,     2,     1,     3,     3,     3,     1,     3,     3,     1,
       3,     3,     1,     3,     1,     3,     1,     3
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
  "DEVICE_DNA", "JTAG_TIMEOUT", "LMS_KEY_PARAMS", "LMS_HASH", "LMS_H",
  "LMS_W", "SHA256", "SHAKE256", "PUF4KMODE", "PUFROSWAP", "SHUTTER",
  "SPLIT", "SMAP_WIDTH", "PUF_HELPER_FILE", "BH_KEY_FILE", "BH_KEY_IV",
  "BH_KEK_IV", "BBRAM_KEK_IV", "EFUSE_KEK_IV", "EFUSE_USER_KEK0_IV",
  "EFUSE_USER_KEK1_IV", "USER_KEYS", "PMCDATA", "BOOTIMAGE", "UDF_BH",
  "INIT", "PMUFW_IMAGE", "AES_KEY_FILE", "FAMILY_KEY", "PPK_FILE",
  "PSK_FILE", "SPK_FILE", "SSK_FILE", "SPK_SIGNATURE_FILE",
  "BH_SIGNATURE_FILE", "HEADER_SIGNATURE_FILE", "AUTHVALUE", "ENCRVALUE",
  "CHECKSUMVALUE", "POWNERVALUE", "PTYPEVALUE", "KEY_SRC", "CORE",
  "BH_RSA", "AUTH_HASH", "INT_HASH", "PUFHD_LOC", "OPT_KEY", "AUTH_ONLY",
  "BOOT_DEVICE_TYPE", "DEST_DEVICE_TYPE", "DEST_CPU_TYPE",
  "EXCEPTION_LEVEL_TYPE", "TRUSTZONE_TYPE", "SPLITMODE", "DPA_CM",
  "SPKSELECT", "HASH_FUNCTION", "AUTH_KEY_LEVEL", "OR", "XOR", "AND",
  "MULT", "DIVIDE", "MODULO", "PLUS", "MINUS", "LSHIFT", "RSHIFT",
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
       176,     0,    -1,   177,    -1,    -1,   177,   178,    -1,    81,
       7,   259,    -1,    -1,    91,   179,     7,     3,   181,     4,
      -1,    81,     7,   259,    -1,    -1,    91,   180,     7,     3,
     181,     4,    -1,    -1,   181,   207,    -1,   181,   199,    -1,
     181,   191,    -1,   181,   182,    -1,   181,   190,    -1,   181,
     209,    -1,   181,   197,    -1,    -1,    86,     3,   183,   184,
       4,    -1,   185,    -1,   185,     5,   184,    -1,   185,   184,
      -1,    -1,    18,     6,   226,    -1,    98,     6,   230,    -1,
     129,     6,   259,    -1,    17,     6,   225,    -1,   131,     6,
     259,    -1,   132,     6,   259,    -1,   133,     6,   259,    -1,
     134,     6,   259,    -1,   135,     6,   259,    -1,    54,     6,
     259,    -1,    41,     6,   267,    -1,    42,     6,   267,    -1,
      19,     6,   227,    -1,   157,    -1,    51,     6,   188,    -1,
     148,    -1,   187,    -1,   187,     9,   186,    -1,   259,     5,
      76,     6,   267,    -1,    -1,   189,    -1,   189,     9,   188,
      -1,   267,    -1,   267,   172,   267,   173,    -1,   267,   172,
     174,   173,    -1,    76,     6,   267,    -1,    66,     6,   267,
      -1,    67,     6,   267,    -1,    68,     6,   267,    -1,   224,
       6,   259,    -1,    98,     6,   230,    -1,    21,     6,   229,
      -1,    41,     6,   267,    -1,   192,    -1,   191,   192,    -1,
      -1,    75,     3,   193,   194,     4,    -1,    -1,   194,   195,
      -1,   194,   207,    -1,   194,   197,    -1,   196,    -1,   196,
       5,   195,    -1,   196,   195,    -1,    76,     6,   267,    -1,
      77,     6,    91,    -1,    78,    -1,    79,    -1,   127,    -1,
      80,     6,   267,    -1,    21,     6,   229,    -1,    72,     6,
     267,    -1,    73,     6,   267,    -1,    74,     6,   267,    -1,
      63,     6,   267,    -1,    64,     6,   267,    -1,    83,   198,
      -1,   198,    -1,    -1,   198,   207,    -1,   198,   209,    -1,
      10,    98,    11,   230,    -1,    -1,    10,    99,   200,    11,
     203,    -1,    99,     3,   203,     4,    -1,    23,     3,   201,
       4,    -1,    10,    23,    11,   239,    -1,    23,     6,   239,
      -1,    10,   100,    11,   248,    -1,    10,   113,    11,   256,
      -1,    10,    53,    11,   217,    -1,   101,     3,   204,     4,
      -1,    85,     3,   186,     4,    -1,   104,     3,   250,     4,
      -1,   104,     3,   253,     4,    -1,   104,     3,   254,     4,
      -1,   202,    -1,   202,     5,   201,    -1,   239,    -1,    65,
      -1,    26,     6,   267,    -1,   206,    -1,   206,     5,   203,
      -1,   206,   203,    -1,   205,    -1,   205,     5,   204,    -1,
     205,   204,    -1,    41,     6,   267,    -1,    42,     6,   267,
      -1,   102,     6,    92,    -1,   103,     6,   267,    -1,   231,
      -1,   232,    -1,   234,    -1,   235,    -1,   236,    -1,   238,
      -1,   237,    -1,   110,    -1,   112,     6,   267,    -1,   233,
      -1,   114,     6,   267,    -1,    69,    -1,    70,    -1,    71,
      -1,   111,     6,   267,    -1,    62,    -1,    -1,    10,   208,
     213,    11,   259,    -1,   259,    -1,    -1,     3,   210,   211,
       4,    -1,   214,    -1,   212,    -1,   212,     5,   211,    -1,
     214,     5,   211,    -1,   212,   211,    -1,   214,   211,    -1,
      84,     6,   259,    -1,    76,     6,   267,    -1,    65,     6,
     267,    -1,    21,     6,   220,    -1,    21,     6,   124,    -1,
      56,     6,    91,    -1,   214,    -1,   214,     5,   213,    -1,
     220,    -1,   223,    -1,   243,    -1,   244,    -1,   215,    -1,
      51,     6,   216,    -1,    52,     6,   219,    -1,   222,    -1,
     222,     9,   216,    -1,   218,    -1,   218,     5,   217,    -1,
     267,    -1,   267,    -1,    12,    -1,    45,    -1,    46,    -1,
      47,    -1,    48,    -1,    49,    -1,    50,    -1,    14,    -1,
      15,    -1,    13,    -1,   127,    -1,   125,    -1,   245,    -1,
     247,    -1,   229,    -1,    16,    -1,    28,    -1,    28,     6,
     155,    -1,   267,    -1,   267,   172,   267,   173,    -1,   267,
     172,   174,   173,    -1,    17,     6,   225,    -1,    18,     6,
     226,    -1,    19,     6,   227,    -1,    20,     6,   228,    -1,
      25,     6,   240,    -1,    24,     6,   241,    -1,    27,     6,
     242,    -1,   129,     6,   259,    -1,   131,     6,   259,    -1,
     132,     6,   259,    -1,   133,     6,   259,    -1,   134,     6,
     259,    -1,    39,     6,   249,    -1,    38,     6,   267,    -1,
     135,     6,   259,    -1,   221,    -1,    21,     6,   229,    -1,
      98,     6,   230,    -1,    41,     6,   267,    -1,    42,     6,
     267,    -1,   157,    -1,    60,     6,   267,    -1,    61,     6,
     267,    -1,   148,    -1,    82,    -1,    87,    -1,   127,    -1,
     245,    -1,   118,    -1,   119,    -1,   120,    -1,   121,    -1,
     122,    -1,   123,    -1,    95,    -1,   138,    -1,    95,    -1,
     139,    -1,    95,    -1,   140,    -1,   141,    -1,   142,    -1,
     143,    -1,   144,    -1,   145,    -1,   157,    -1,   146,    -1,
     147,    -1,   148,    -1,   149,    -1,   150,    -1,   151,    -1,
     153,    -1,   152,    -1,   154,    -1,    29,     6,   267,    -1,
      30,     6,   267,    -1,    31,     6,   267,    -1,    32,     6,
     267,    -1,    33,     6,   267,    -1,    34,     6,   267,    -1,
      35,    -1,    36,    -1,    22,     6,   267,    -1,    88,     6,
     267,    -1,    89,     6,   267,    -1,    90,     6,   267,    -1,
      54,     6,   259,    -1,    55,     6,   259,    -1,    57,     6,
     259,    -1,   129,    -1,   246,    -1,   135,    -1,   136,    -1,
     137,    -1,   116,    -1,   115,    -1,   117,    -1,   130,    -1,
     131,    -1,   132,    -1,   133,    -1,   134,    -1,   128,    -1,
     124,    -1,   126,    -1,    37,     6,   267,    -1,    39,     6,
     249,    -1,    38,     6,   267,    -1,    40,    -1,   248,     9,
     248,    -1,   158,    -1,   251,    -1,   251,     9,   251,    -1,
     105,     6,   252,    -1,   107,     6,   267,    -1,   106,     6,
     267,    -1,   159,    -1,    91,    -1,   253,     5,   253,    -1,
      -1,   160,     3,   255,   253,     4,    -1,   254,   254,    -1,
      43,     6,   257,    -1,    44,     6,   258,    -1,   256,     5,
     256,    -1,   156,    -1,    58,    -1,    59,    -1,    93,    -1,
      94,    -1,    97,    -1,    96,    -1,   172,   267,   173,    -1,
     260,    -1,   167,   261,    -1,   171,   261,    -1,   261,    -1,
     262,   164,   261,    -1,   262,   165,   261,    -1,   262,   166,
     261,    -1,   262,    -1,   263,   167,   262,    -1,   263,   168,
     262,    -1,   263,    -1,   264,   169,   263,    -1,   264,   170,
     263,    -1,   264,    -1,   265,   163,   264,    -1,   265,    -1,
     266,   162,   265,    -1,   266,    -1,   267,   161,   266,    -1
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
     376,   380,   383,   387,   391,   395,   399,   401,   403,   405,
     407,   409,   411,   413,   415,   419,   421,   425,   427,   429,
     431,   435,   437,   438,   444,   446,   447,   452,   454,   456,
     460,   464,   467,   470,   474,   478,   482,   486,   490,   494,
     496,   500,   502,   504,   506,   508,   510,   514,   518,   520,
     524,   526,   530,   532,   534,   536,   538,   540,   542,   544,
     546,   548,   550,   552,   554,   556,   558,   560,   562,   564,
     566,   568,   572,   574,   579,   584,   588,   592,   596,   600,
     604,   608,   612,   616,   620,   624,   628,   632,   636,   640,
     644,   646,   650,   654,   658,   662,   664,   668,   672,   674,
     676,   678,   680,   682,   684,   686,   688,   690,   692,   694,
     696,   698,   700,   702,   704,   706,   708,   710,   712,   714,
     716,   718,   720,   722,   724,   726,   728,   730,   732,   734,
     736,   740,   744,   748,   752,   756,   760,   762,   764,   768,
     772,   776,   780,   784,   788,   792,   794,   796,   798,   800,
     802,   804,   806,   808,   810,   812,   814,   816,   818,   820,
     822,   824,   828,   832,   836,   838,   842,   844,   846,   850,
     854,   858,   862,   864,   866,   870,   871,   877,   880,   884,
     888,   892,   894,   896,   898,   900,   902,   904,   906,   910,
     912,   915,   918,   920,   924,   928,   932,   934,   938,   942,
     944,   948,   952,   954,   958,   960,   964,   966
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   208,   208,   210,   211,   213,   214,   214,   226,   227,
     227,   233,   234,   235,   236,   237,   238,   239,   240,   243,
     243,   248,   249,   250,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   272,   273,   274,
     275,   279,   280,   283,   284,   286,   287,   290,   292,   294,
     298,   299,   300,   302,   303,   304,   305,   306,   309,   310,
     313,   313,   322,   323,   324,   325,   328,   329,   330,   333,
     334,   335,   336,   337,   338,   340,   341,   342,   343,   344,
     347,   352,   353,   356,   357,   358,   361,   364,   364,   367,
     368,   369,   374,   375,   376,   377,   378,   379,   380,   381,
     382,   385,   386,   389,   390,   391,   394,   395,   396,   399,
     400,   401,   404,   405,   406,   407,   410,   413,   415,   417,
     419,   421,   423,   425,   427,   429,   431,   435,   436,   437,
     438,   439,   442,   442,   449,   455,   455,   461,   462,   463,
     464,   465,   466,   469,   472,   473,   475,   476,   477,   482,
     483,   486,   487,   488,   489,   490,   493,   494,   497,   498,
     501,   502,   505,   510,   512,   513,   514,   515,   516,   517,
     518,   519,   520,   521,   524,   525,   526,   527,   528,   529,
     532,   533,   536,   537,   538,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   553,   554,
     555,   556,   557,   558,   559,   560,   561,   562,   563,   564,
     565,   568,   569,   570,   571,   572,   573,   574,   575,   578,
     579,   582,   583,   586,   587,   590,   593,   596,   599,   602,
     605,   608,   611,   614,   617,   620,   623,   626,   629,   632,
     635,   636,   637,   638,   639,   640,   641,   642,   643,   644,
     645,   646,   649,   650,   651,   654,   655,   656,   657,   658,
     659,   660,   661,   662,   665,   666,   667,   668,   671,   672,
     673,   676,   679,   682,   685,   688,   691,   695,   696,   698,
     699,   700,   702,   706,   711,   713,   713,   715,   717,   720,
     721,   724,   727,   730,   736,   737,   740,   741,   742,   748,
     749,   750,   754,   755,   756,   757,   760,   761,   762,   765,
     766,   767,   770,   771,   774,   775,   779,   780
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
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 1167;
  const int BisonParser::yynnts_ = 93;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 175;

  const unsigned int BisonParser::yyuser_token_number_max_ = 429;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 24 "parser/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 2706 "bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 785 "parser/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
