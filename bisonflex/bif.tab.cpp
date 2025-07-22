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
#line 214 "parser/bif.y"
    { options.includeBifOptionsList.push_back((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 215 "parser/bif.y"
    { currentBifOptions = new BifOptions(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series(), (yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 7:
/* Line 670 of lalr1.cc  */
#line 217 "parser/bif.y"
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 231 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series()); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 242 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 243 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 244 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 245 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 246 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 247 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 248 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 249 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 250 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 251 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 252 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.partitionRevokeId = (yysemantic_stack_[(3) - (3)].number);}
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 253 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkRevokeId = (yysemantic_stack_[(3) - (3)].number); 
                                                                                if(options.GetArchType() == Arch::VERSAL)
                                                                                {
                                                                                   LOG_ERROR("SPK_REVOKE_ID does not support for versal arch, instead please use revoke_id option.");
                                                                                }
                                                                                }
    break;

  case 34:
/* Line 670 of lalr1.cc  */
#line 260 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 261 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 37:
/* Line 670 of lalr1.cc  */
#line 263 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 271 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ihtOptionalDataInfo.push_back(std::pair<std::string, uint32_t>((yysemantic_stack_[(5) - (1)].string), (yysemantic_stack_[(5) - (5)].number))); }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 278 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 280 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 282 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 286 "parser/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 287 "parser/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 288 "parser/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 289 "parser/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 290 "parser/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 291 "parser/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 292 "parser/bif.y"
    { currentBifOptions->SetPdiType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 293 "parser/bif.y"
    { currentBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number));}
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 300 "parser/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 301 "parser/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions);
                                                                                  if((options.archType == Arch::SPARTANUP) && (currentBifOptions->imageBifOptionList.size() > 1))
                                                                                  {
                                                                                      LOG_ERROR("'-arch spartanup' cannot accomodate more than one Sub-System.");
                                                                                  }
                                                                                }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 320 "parser/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 67:
/* Line 670 of lalr1.cc  */
#line 321 "parser/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 68:
/* Line 670 of lalr1.cc  */
#line 322 "parser/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 323 "parser/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 324 "parser/bif.y"
    { LOG_ERROR("BIF attribute error !!!\n\t This usage of 'init' is not supported. See 'bootgen -bif_help init' for usage details."); }
    break;

  case 71:
/* Line 670 of lalr1.cc  */
#line 325 "parser/bif.y"
    { LOG_ERROR("Copy to Memory feature with the attribute 'copy' is no more supported.\n\t   This can be duplicated with the option 'imagestore'. Please refer UG1283 for more details.");
                                                                                  currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 72:
/* Line 670 of lalr1.cc  */
#line 327 "parser/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 328 "parser/bif.y"
    { currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 329 "parser/bif.y"
    { currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 75:
/* Line 670 of lalr1.cc  */
#line 330 "parser/bif.y"
    { currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 76:
/* Line 670 of lalr1.cc  */
#line 331 "parser/bif.y"
    { if (options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP || (options.GetArchType() == Arch::VERSAL && !options.IsVersalNetSeries()))
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr' is not supported for the specified architecture");
                                                                                  currentImageBifOptions->SetPcrNumber((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 77:
/* Line 670 of lalr1.cc  */
#line 334 "parser/bif.y"
    { if (options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP || (options.GetArchType() == Arch::VERSAL && !options.IsVersalNetSeries()))
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr_mid' is not supported for the specified architecture");
                                                                                  currentImageBifOptions->SetPcrMeasurementIndex((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 83:
/* Line 670 of lalr1.cc  */
#line 348 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQ || options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported for the specified architecture.\n\t   Refer 'bootgen -bif_help' for more details");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 84:
/* Line 670 of lalr1.cc  */
#line 351 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 88:
/* Line 670 of lalr1.cc  */
#line 356 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                  if(options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); 
                                                                                  currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 89:
/* Line 670 of lalr1.cc  */
#line 361 "parser/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 376 "parser/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 377 "parser/bif.y"
    { currentBifOptions->SetBootDevice(BootDevice::IMAGESTORE); }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 378 "parser/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 391 "parser/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 110:
/* Line 670 of lalr1.cc  */
#line 392 "parser/bif.y"
    { currentBifOptions->SetAuthJtagSPKRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 393 "parser/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 394 "parser/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 397 "parser/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 400 "parser/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 402 "parser/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 404 "parser/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 117:
/* Line 670 of lalr1.cc  */
#line 406 "parser/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 118:
/* Line 670 of lalr1.cc  */
#line 408 "parser/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 119:
/* Line 670 of lalr1.cc  */
#line 410 "parser/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 120:
/* Line 670 of lalr1.cc  */
#line 412 "parser/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 414 "parser/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 416 "parser/bif.y"
    {  currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));}
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 418 "parser/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->SetSmapWidth((yysemantic_stack_[(3) - (3)].number));
                                                                                }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 422 "parser/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 423 "parser/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 424 "parser/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 425 "parser/bif.y"
    { currentBifOptions->SetPufRingOscilltorSwapConfigValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 426 "parser/bif.y"
    { currentBifOptions->SetDiceEnable(); }
    break;

  case 129:
/* Line 670 of lalr1.cc  */
#line 429 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(),options.IsVersalNetSeries(), options.IsDl9Series()); }
    break;

  case 130:
/* Line 670 of lalr1.cc  */
#line 432 "parser/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 436 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 442 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series()); }
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 456 "parser/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 459 "parser/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 460 "parser/bif.y"
    { currentPartitionBifOptions->imageStoreId = (yysemantic_stack_[(3) - (3)].number);
                                                                                  currentPartitionBifOptions->SetPartitionType(PartitionType::IMAGE_STORE_PDI); }
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 463 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 464 "parser/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string), currentImageBifOptions->GetImageType());
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 492 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] is supported only for ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 497 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 161:
/* Line 670 of lalr1.cc  */
#line 499 "parser/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 500 "parser/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 501 "parser/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 502 "parser/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 503 "parser/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 504 "parser/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 505 "parser/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 506 "parser/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 507 "parser/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 508 "parser/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 511 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 512 "parser/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 513 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 514 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 515 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 516 "parser/bif.y"
    { currentPartitionBifOptions->SetLockStepFlag();}
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 519 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 520 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 523 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 524 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 525 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 527 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 528 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 529 "parser/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 530 "parser/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 531 "parser/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 532 "parser/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 533 "parser/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 189:
/* Line 670 of lalr1.cc  */
#line 534 "parser/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 535 "parser/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 191:
/* Line 670 of lalr1.cc  */
#line 536 "parser/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 192:
/* Line 670 of lalr1.cc  */
#line 537 "parser/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 193:
/* Line 670 of lalr1.cc  */
#line 538 "parser/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 194:
/* Line 670 of lalr1.cc  */
#line 539 "parser/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 195:
/* Line 670 of lalr1.cc  */
#line 540 "parser/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 196:
/* Line 670 of lalr1.cc  */
#line 541 "parser/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 198:
/* Line 670 of lalr1.cc  */
#line 543 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 544 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 200:
/* Line 670 of lalr1.cc  */
#line 545 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 201:
/* Line 670 of lalr1.cc  */
#line 546 "parser/bif.y"
    { currentPartitionBifOptions->SetSPKRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 202:
/* Line 670 of lalr1.cc  */
#line 547 "parser/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 203:
/* Line 670 of lalr1.cc  */
#line 548 "parser/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 204:
/* Line 670 of lalr1.cc  */
#line 549 "parser/bif.y"
    { currentPartitionBifOptions->SetClusterNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 205:
/* Line 670 of lalr1.cc  */
#line 550 "parser/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 206:
/* Line 670 of lalr1.cc  */
#line 551 "parser/bif.y"
    { currentPartitionBifOptions->SetDelayAuth(true); }
    break;

  case 207:
/* Line 670 of lalr1.cc  */
#line 552 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBootFlag(); }
    break;

  case 216:
/* Line 670 of lalr1.cc  */
#line 565 "parser/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 218:
/* Line 670 of lalr1.cc  */
#line 569 "parser/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 220:
/* Line 670 of lalr1.cc  */
#line 573 "parser/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 237:
/* Line 670 of lalr1.cc  */
#line 622 "parser/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 238:
/* Line 670 of lalr1.cc  */
#line 623 "parser/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 239:
/* Line 670 of lalr1.cc  */
#line 624 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), false); }
    break;

  case 240:
/* Line 670 of lalr1.cc  */
#line 625 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), true); }
    break;

  case 241:
/* Line 670 of lalr1.cc  */
#line 626 "parser/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 242:
/* Line 670 of lalr1.cc  */
#line 627 "parser/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 243:
/* Line 670 of lalr1.cc  */
#line 628 "parser/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 244:
/* Line 670 of lalr1.cc  */
#line 629 "parser/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 245:
/* Line 670 of lalr1.cc  */
#line 630 "parser/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 246:
/* Line 670 of lalr1.cc  */
#line 631 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmARegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 247:
/* Line 670 of lalr1.cc  */
#line 632 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 248:
/* Line 670 of lalr1.cc  */
#line 633 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmCRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 249:
/* Line 670 of lalr1.cc  */
#line 636 "parser/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 250:
/* Line 670 of lalr1.cc  */
#line 637 "parser/bif.y"
    { currentPartitionBifOptions->acFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 251:
/* Line 670 of lalr1.cc  */
#line 638 "parser/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 268:
/* Line 670 of lalr1.cc  */
#line 663 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 269:
/* Line 670 of lalr1.cc  */
#line 666 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 270:
/* Line 670 of lalr1.cc  */
#line 669 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 271:
/* Line 670 of lalr1.cc  */
#line 672 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 280:
/* Line 670 of lalr1.cc  */
#line 693 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP || options.GetArchType() == Arch::VERSAL)
                                                                   LOG_ERROR("BIF attribute error !!!\n\t\t'lms_key_params' is not supported with the mentioned -arch");
                                                                 lmsParams.lms_param = (yysemantic_stack_[(1) - (1)].string);
                                                                 currentBifOptions->SetLmsParameters(lmsParams) ;
                                                               }
    break;

  case 282:
/* Line 670 of lalr1.cc  */
#line 700 "parser/bif.y"
    { lmsParams.type = (yysemantic_stack_[(2) - (1)].authkeylevel_t);}
    break;

  case 285:
/* Line 670 of lalr1.cc  */
#line 704 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 289:
/* Line 670 of lalr1.cc  */
#line 714 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 290:
/* Line 670 of lalr1.cc  */
#line 717 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 295:
/* Line 670 of lalr1.cc  */
#line 729 "parser/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 297:
/* Line 670 of lalr1.cc  */
#line 736 "parser/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 298:
/* Line 670 of lalr1.cc  */
#line 737 "parser/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 300:
/* Line 670 of lalr1.cc  */
#line 742 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 301:
/* Line 670 of lalr1.cc  */
#line 743 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 302:
/* Line 670 of lalr1.cc  */
#line 744 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 304:
/* Line 670 of lalr1.cc  */
#line 748 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 305:
/* Line 670 of lalr1.cc  */
#line 749 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 307:
/* Line 670 of lalr1.cc  */
#line 753 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 308:
/* Line 670 of lalr1.cc  */
#line 754 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 310:
/* Line 670 of lalr1.cc  */
#line 758 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 312:
/* Line 670 of lalr1.cc  */
#line 762 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 314:
/* Line 670 of lalr1.cc  */
#line 767 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1537 "bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -460;
  const short int
  BisonParser::yypact_[] =
  {
      -460,    21,    23,  -460,    56,  -460,  -460,   -11,    96,  -460,
    -460,  -460,   104,  -460,   884,  -460,  -460,   131,   124,    84,
     151,   158,   160,   163,   117,   170,  -460,   180,   196,   201,
     208,   218,   220,  -460,  -460,  -460,  -460,  -460,  -460,  -460,
    -460,  -460,  -460,  -460,  -460,  -460,  -460,  -460,  -460,  -460,
    -460,  -460,  -460,  -460,   153,  -460,  -460,     8,  -460,  -460,
    -460,   232,  -460,  -460,  -460,   641,   248,   250,   252,  -460,
     253,   265,   787,    97,    24,   135,   -43,   -43,   -43,   -43,
    -460,   -43,     8,   -11,  -460,   147,   922,   139,   204,  -460,
    -460,  -460,  -460,   -11,  -460,  -460,  -460,  -460,  -460,   288,
     291,   292,   293,   294,   301,   302,   307,   317,   318,   319,
     320,   335,   338,   340,   342,  -460,  -460,   345,   346,   348,
     353,  -460,  -460,  -460,  -460,  -460,  -460,   355,   359,   366,
     367,   368,   369,   370,   371,   372,   375,  -460,   376,  -460,
     378,   379,   380,   381,  -460,  -460,  -460,  -460,  -460,   385,
     391,   392,   399,   400,   404,  -460,  -460,  -460,   352,    10,
     495,  -460,  -460,  -460,  -460,  -460,  -460,  -460,  -460,  -460,
     135,   -43,   147,   401,   235,    79,   405,   406,   410,  -460,
     412,  -460,  -460,   415,   411,  -460,  -460,  -460,  -460,   -43,
     -43,   -43,  -460,  -460,    -4,     6,    16,   257,   259,   264,
     264,   264,   264,  -460,   264,   427,   423,   428,   183,  -460,
    -460,  -460,  -460,  -460,  -460,  -460,   429,   430,   433,  -460,
    -460,  -460,  -460,  -460,  -460,  -460,  -460,   436,   108,  -460,
    -460,  -460,  -460,  -460,  -460,  -460,  -460,   435,   438,   439,
     440,   443,    69,  -460,   442,   444,   451,   431,   445,   450,
     212,     3,  -460,   -26,   -48,   -62,   321,   354,   -43,   290,
     311,   306,   310,   -43,   -43,   -43,   -43,   -43,   -43,   -43,
     308,   -43,   -43,   -43,   -43,   -11,   -11,   377,   -11,   -43,
     -43,   -43,   -43,   -11,   -43,   -43,   -43,   147,   -11,   -11,
     -11,   -11,   -11,   -11,  -460,   641,  -460,   641,  -460,  -460,
    -460,   462,   264,  -460,   922,   466,   467,   468,  -460,   483,
     469,   470,   488,    97,   -11,   787,   -43,  -460,    24,  -460,
    -460,   -89,   -43,   -43,   -43,   -43,   -43,   -43,   -43,   -43,
     -43,   -43,   256,  -460,   -11,   421,   492,   512,   515,   526,
     529,   532,   533,   542,   547,   548,   551,   552,   553,   555,
    -460,  -460,   558,   114,   -43,   -43,   -43,  -460,   922,  -460,
     -43,   -43,   471,   -43,  -460,   139,  -460,   407,   -43,   -43,
    -460,  -460,    11,  -460,   473,  -460,   408,  -460,  -460,  -460,
    -460,  -460,  -460,  -460,  -460,  -460,  -460,  -460,  -460,  -460,
    -460,   264,  -460,  -460,  -460,  -460,  -460,  -460,  -460,   264,
     264,   264,   264,   264,   264,   264,  -460,  -460,   264,   264,
    -460,   556,   -93,  -460,   264,  -460,  -460,  -460,  -460,   264,
     264,   264,   264,  -460,   264,   264,   264,  -460,  -460,  -460,
    -460,  -460,  -460,  -460,  -460,  -460,   -43,  -460,   -43,   -43,
     308,   235,   413,   168,    79,  -460,  -460,  -460,   264,  -460,
    -460,  -460,  -460,  -460,    -4,    -4,     6,     6,    16,   257,
     259,  -460,   561,   564,   566,   567,   568,   569,   570,   572,
    -460,  -460,   574,  -460,  -460,   350,  -460,  -460,  -460,   575,
     -26,   -48,   -62,   -43,   -43,   -43,   -11,   147,   -11,   -11,
     -11,   -11,   -11,   -11,  -460,   183,  -460,   264,   264,   264,
    -460,   264,   264,  -460,   264,  -460,  -460,  -460,   264,   264,
     473,  -460,   581,   -43,   171,  -460,   264,   264,  -460,   483,
    -460,  -460,  -460,  -460,  -460,   488,    97,   -43,   -43,   -43,
     -43,   -43,   -43,   496,   -43,   316,  -460,   -43,  -460,  -460,
    -460,   264,   264,  -460,   579,   -84,  -460,  -460,  -460,  -460,
    -460,  -460,  -460,  -460,  -460,   228,  -460,   416,   -88,  -460,
     264,   264,   264,   264,   264,   264,  -460,   264,  -460,   264,
     -43,   186,  -460,  -460,  -460,  -460,   417,   -68,  -460,  -460
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,     6,     4,     0,     0,   291,
     292,     5,     0,     8,     0,   132,     7,   129,     0,     0,
       0,     0,     0,     0,     0,     0,    80,     0,     0,     0,
       0,     0,     0,   258,   257,   259,   210,   211,   212,   213,
     214,   215,   208,   252,   260,   261,   262,   263,   264,   254,
     255,   256,    12,    13,    11,    55,    15,    79,    10,     9,
      14,     0,   209,   253,   131,     0,     0,     0,     0,    84,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      57,     0,    78,    41,    16,     0,     0,     0,     0,    56,
     129,    81,    82,     0,   161,   170,   168,   169,   176,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   177,     0,
       0,     0,     0,     0,     0,   243,   244,     0,     0,     0,
       0,   162,   163,   164,   165,   166,   167,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   206,     0,   207,
       0,     0,     0,     0,   266,   172,   267,   171,   265,   252,
     261,   262,   263,   264,   254,   223,   205,   202,     0,   135,
     134,   152,   148,   197,   149,   175,   150,   151,   173,   174,
       0,     0,     0,     0,     0,     0,     0,     0,   146,    53,
       0,   101,   233,     0,    98,   100,    89,   294,   293,     0,
       0,     0,   296,   299,   303,   306,   309,   311,   313,    54,
      48,    49,    50,    59,    47,     0,    38,     0,    21,   224,
      52,   128,   124,   125,   126,   120,     0,     0,     0,   225,
     226,   228,   229,   230,   231,   232,   227,     0,   103,   113,
     114,   122,   115,   116,   117,   119,   118,     0,     0,     0,
       0,     0,   106,   280,     0,     0,     0,     0,     0,   274,
       0,     0,    51,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   133,     0,   138,     0,   139,    88,
      92,   157,   159,    83,     0,     0,     0,     0,   271,    90,
       0,     0,    91,     0,     0,     0,     0,    87,     0,   297,
     298,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    80,    94,    41,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      37,    35,     0,    18,     0,     0,     0,    86,     0,   105,
       0,     0,     0,     0,    93,     0,   108,     0,     0,     0,
     282,    95,     0,    96,     0,    97,   284,   216,   217,   182,
     218,   219,   183,   220,   221,   184,   222,   185,   144,   143,
     175,   245,   235,   187,   234,   186,   236,   188,   178,   237,
     238,   239,   240,   241,   242,   195,   273,   194,   200,   201,
     153,   155,   179,   154,   160,   249,   250,   145,   251,   203,
     204,   142,   141,   140,   246,   247,   248,   199,   189,   190,
     191,   192,   193,   196,   136,   137,     0,    85,     0,     0,
       0,     0,     0,     0,     0,   198,   130,   147,   102,    99,
     295,   300,   301,   302,   304,   305,   307,   308,   310,   312,
     314,    58,     0,     0,     0,     0,     0,     0,     0,     0,
      68,    69,     0,    70,    60,    63,    62,    61,    39,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    17,    21,    20,   127,   121,   123,
     104,   109,   110,   111,   112,   107,   279,   276,   278,   277,
       0,   275,   281,     0,     0,   158,   268,   270,   269,   272,
     288,   285,   289,   290,   286,   287,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    65,     0,    25,    22,
      34,    32,    33,    36,    42,    44,    31,    23,    24,    26,
      27,    28,    29,    30,    19,     0,   156,     0,     0,    72,
      76,    77,    73,    74,    75,    66,    67,    71,    64,    40,
       0,     0,   283,   181,   180,    43,     0,     0,    46,    45
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -460,  -460,  -460,  -460,  -460,  -460,  -460,  -460,  -344,  -460,
     258,  -460,    25,  -460,  -460,  -460,   537,  -460,  -460,  -459,
    -460,   262,   571,  -460,  -460,   278,  -460,  -209,  -206,  -460,
    -460,   -10,  -460,   584,  -460,  -147,  -460,   284,   -64,  -460,
      87,   165,  -460,  -460,   347,  -460,  -460,  -460,  -460,   122,
     125,   121,  -460,   -63,  -166,  -460,  -460,  -460,  -460,  -460,
    -460,  -460,  -460,   -61,  -460,  -460,  -460,  -460,  -460,   591,
    -460,  -460,   166,   169,  -460,   236,  -460,  -357,   525,  -460,
     172,  -460,  -460,    13,  -460,  -109,   -90,   -58,   285,   287,
     303,   -76
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     6,     8,    14,    52,   208,   352,   353,
     205,   206,   543,   544,    53,    54,    55,   203,   332,   474,
     475,    56,    57,    58,   173,   183,   184,   227,   241,   242,
     228,    91,    72,    92,    65,   158,   159,   177,   160,   161,
     410,   300,   301,   413,   162,   163,   411,   164,    61,   379,
     382,   385,   387,   165,   210,   229,   230,   231,   232,   233,
     234,   235,   236,   185,   395,   393,   397,   166,   167,   168,
      63,   169,   309,   407,   248,   249,   507,   250,   376,   510,
     312,   521,   524,    64,   192,   193,   194,   195,   196,   197,
     198,   302
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       199,   200,   201,   202,    59,   204,   303,   375,   178,   496,
     179,    15,   296,   298,   186,   295,   536,   512,    90,   359,
      11,     3,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   383,   105,   106,   366,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   380,   117,   118,
     180,   119,   120,   187,   188,   121,   122,   123,   124,   125,
     126,   127,   128,     7,   129,   130,   131,   132,   331,   377,
     133,   134,   331,   331,   365,   135,   568,   331,   384,   514,
     319,   320,     9,    10,   450,   574,   136,    74,   571,   181,
      75,   381,   137,   331,   138,   437,   207,   139,   140,   141,
     142,     9,    10,    12,     4,   579,   252,    13,   143,   299,
     237,   238,   378,   358,     5,   321,   244,   245,   246,   495,
      80,   427,   310,   311,   189,    33,    34,    35,   190,   191,
      73,   336,   337,   338,   144,   145,   146,   147,   148,   149,
      44,   150,   151,   152,   153,   154,    50,    51,   434,   500,
     435,   554,   155,   555,    66,   339,   340,    76,   156,   505,
     322,   323,   324,   247,    77,   341,    78,   157,   342,    79,
     211,   239,   240,   325,   326,   182,    81,   212,   213,   214,
     237,   238,   391,    83,    67,   327,   328,   399,   400,   401,
     402,   403,   404,   405,   390,   408,   409,   412,   414,    84,
     336,   337,   338,   419,   420,   421,   422,    85,   424,   425,
     426,    86,   343,   451,   452,   453,   373,   374,   215,   216,
     217,    87,   218,    88,   339,   340,   522,   523,    24,    68,
      69,    70,   572,   374,   341,   454,   455,   342,    93,   155,
     448,   239,   240,   344,    71,   345,   346,   347,   348,   349,
     445,   178,   219,   220,   221,   222,   223,   224,   225,   170,
     461,   171,   350,   172,   174,   226,    90,   187,   188,   456,
     457,   351,   305,   306,   307,   308,   175,   462,   497,   498,
     499,   343,   187,   188,   501,   502,   182,   504,   415,   416,
     209,   418,   508,   509,   253,   243,   423,   254,   255,   256,
     257,   428,   429,   430,   431,   432,   433,   258,   259,   244,
     245,   246,   344,   260,   345,   346,   347,   348,   349,   463,
     464,   547,   477,   261,   262,   263,   264,   446,   465,   466,
     467,   350,   468,   469,   470,   471,   472,   462,   189,    26,
     351,   265,   190,   191,   266,   557,   267,   207,   268,     9,
      10,   269,   270,   189,   271,   535,   294,   190,   191,   272,
     576,   273,   516,   517,   247,   274,    94,    95,    96,    97,
      98,   462,   275,   276,   277,   278,   279,   280,   281,   463,
     464,   282,   283,   473,   284,   285,   286,   287,   465,   466,
     467,   288,   468,   469,   470,   471,   472,   289,   290,   121,
     122,   123,   124,   125,   126,   291,   292,   541,   542,   545,
     293,   313,   304,   463,   464,   315,   318,   314,   316,   317,
     329,   330,   465,   466,   467,   331,   468,   469,   470,   471,
     472,   333,   334,   335,   370,   354,   355,   412,   558,   356,
     357,   360,   392,   473,   361,   362,   363,   364,   367,   371,
     368,   560,   561,   562,   563,   564,   565,   369,   567,   372,
     396,   569,   386,   559,   394,   398,   406,   436,   417,    33,
      34,    35,   438,   439,   440,   442,   443,   473,   388,   145,
     146,   147,   148,    43,    44,    45,    46,    47,    48,    49,
      50,    51,   441,   444,   545,   577,   155,   479,   480,   546,
     297,   548,   549,   550,   551,   552,   553,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   481,   105,
     106,   482,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   483,   117,   118,   484,   119,   120,   485,   486,
     121,   122,   123,   124,   125,   126,   127,   128,   487,   129,
     130,   131,   132,   488,   489,   133,   134,   490,   491,   492,
     135,   493,   494,   503,   243,   513,   506,   526,   247,   520,
     527,   136,   528,   529,   530,   531,   532,   137,   533,   138,
     534,   537,   139,   140,   141,   142,   374,   566,   570,   573,
     578,    89,   478,   143,   476,   575,   449,    82,    60,   447,
     556,   515,   538,   540,   389,    62,   539,   519,   511,   518,
      33,    34,    35,   251,   458,     0,   525,   459,     0,   144,
     145,   146,   147,   148,   149,    44,   150,   151,   152,   153,
     154,    50,    51,     0,   460,     0,     0,   155,     0,     0,
       0,     0,     0,   156,     0,     0,     0,     0,     0,     0,
       0,     0,   157,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,     0,   105,   106,     0,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,     0,   117,
     118,     0,   119,   120,     0,     0,   121,   122,   123,   124,
     125,   126,   127,   128,     0,   129,   130,   131,   132,     0,
       0,   133,   134,     0,     0,     0,   135,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   136,     0,     0,
       0,     0,     0,   137,     0,   138,     0,     0,   139,   140,
     141,   142,     0,     0,     0,     0,     0,     0,     0,   143,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    33,    34,    35,     0,
       0,     0,     0,     0,     0,   144,   145,   146,   147,   148,
     149,    44,   150,   151,   152,   153,   154,    50,    51,     0,
       0,     0,     0,   155,     0,     0,     0,     0,     0,   156,
       0,     0,     0,     0,     0,     0,     0,     0,   157,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   176,   104,
       0,   105,   106,     0,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,     0,   117,   118,     0,   119,   120,
       0,     0,   121,   122,   123,   124,   125,   126,   127,   128,
       0,   129,   130,     0,   132,     0,     0,   133,   134,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   137,
       0,     0,     0,     0,   139,   140,   141,   142,     0,     0,
       0,     0,     0,     0,     0,   143,     0,    15,    16,     0,
       0,     0,     0,     0,    17,     0,     0,     0,     0,     0,
       0,     0,    33,    34,    35,    18,     0,    19,     0,     0,
       0,   144,   145,   146,   147,   148,   149,    44,   150,   151,
     152,   153,   154,    50,    51,    20,     0,     0,     0,   155,
       0,     0,     0,     0,     0,   156,     0,     0,     0,     0,
       0,     0,     0,     0,   157,     0,     0,     0,     0,     0,
      21,    22,    23,     0,     0,     0,     0,     0,     0,    24,
      25,     0,     0,     0,     0,     0,     0,    26,     0,    27,
      28,     0,     0,     0,     0,     0,     0,     9,    10,     0,
       0,     0,    29,    30,   211,    31,     0,     0,    32,     0,
       0,   212,   213,   214,     0,     0,     0,     0,     0,    33,
      34,    35,    36,    37,    38,    39,    40,    41,     0,     0,
       0,    42,     0,    43,    44,    45,    46,    47,    48,    49,
      50,    51,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   215,   216,   217,     0,   218,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   219,   220,   221,   222,
     223,   224,   225,     0,     0,     0,     0,     0,     0,   226
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        76,    77,    78,    79,    14,    81,   172,     4,    72,   353,
      73,     3,   159,   160,    75,     5,   475,   374,    10,   228,
       7,     0,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    95,    24,    25,   242,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    95,    38,    39,
      26,    41,    42,    96,    97,    45,    46,    47,    48,    49,
      50,    51,    52,     7,    54,    55,    56,    57,   161,    95,
      60,    61,   161,   161,     5,    65,   535,   161,   140,   172,
     189,   190,    93,    94,   173,   173,    76,     3,   172,    65,
       6,   139,    82,   161,    84,   304,    83,    87,    88,    89,
      90,    93,    94,     7,    81,   173,    93,     3,    98,   170,
      41,    42,   138,     5,    91,   191,   105,   106,   107,     5,
       3,   287,    43,    44,   167,   115,   116,   117,   171,   172,
       6,    17,    18,    19,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   137,   295,   358,
     297,   495,   142,   510,    23,    41,    42,     6,   148,   365,
     164,   165,   166,   160,     6,    51,     6,   157,    54,     6,
      62,   102,   103,   167,   168,   151,     6,    69,    70,    71,
      41,    42,   258,     3,    53,   169,   170,   263,   264,   265,
     266,   267,   268,   269,   257,   271,   272,   273,   274,     3,
      17,    18,    19,   279,   280,   281,   282,     6,   284,   285,
     286,     3,    98,   322,   323,   324,     4,     5,   110,   111,
     112,     3,   114,     3,    41,    42,    58,    59,    75,    98,
      99,   100,     4,     5,    51,   325,   326,    54,     6,   142,
     316,   102,   103,   129,   113,   131,   132,   133,   134,   135,
     313,   315,   144,   145,   146,   147,   148,   149,   150,    11,
       4,    11,   148,    11,    11,   157,    10,    96,    97,   327,
     328,   157,    37,    38,    39,    40,    11,    21,   354,   355,
     356,    98,    96,    97,   360,   361,   151,   363,   275,   276,
     143,   278,   368,   369,     6,    91,   283,     6,     6,     6,
       6,   288,   289,   290,   291,   292,   293,     6,     6,   105,
     106,   107,   129,     6,   131,   132,   133,   134,   135,    63,
      64,   487,   332,     6,     6,     6,     6,   314,    72,    73,
      74,   148,    76,    77,    78,    79,    80,    21,   167,    83,
     157,     6,   171,   172,     6,   174,     6,   334,     6,    93,
      94,     6,     6,   167,     6,     5,     4,   171,   172,     6,
     174,     6,   438,   439,   160,     6,    12,    13,    14,    15,
      16,    21,     6,     6,     6,     6,     6,     6,     6,    63,
      64,     6,     6,   127,     6,     6,     6,     6,    72,    73,
      74,     6,    76,    77,    78,    79,    80,     6,     6,    45,
      46,    47,    48,    49,    50,     6,     6,   483,   484,   485,
       6,     6,    11,    63,    64,     5,     5,    11,     6,     4,
     163,   162,    72,    73,    74,   161,    76,    77,    78,    79,
      80,     4,     9,     5,     3,     6,     6,   513,   514,     6,
       4,     6,   152,   127,     6,     6,     6,     4,     6,     4,
       6,   527,   528,   529,   530,   531,   532,     6,   534,     9,
     154,   537,   141,   526,   153,   155,   158,     5,    91,   115,
     116,   117,     6,     6,     6,     6,     6,   127,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,     9,     5,   570,   571,   142,    76,     6,   486,
       5,   488,   489,   490,   491,   492,   493,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,     6,    24,
      25,     6,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,     6,    38,    39,     6,    41,    42,     6,     6,
      45,    46,    47,    48,    49,    50,    51,    52,     6,    54,
      55,    56,    57,     6,     6,    60,    61,     6,     6,     6,
      65,     6,     4,    92,    91,     9,   159,     6,   160,   156,
       6,    76,     6,     6,     6,     6,     6,    82,     6,    84,
       6,     6,    87,    88,    89,    90,     5,    91,     9,   173,
     173,    54,   334,    98,   332,   570,   318,    26,    14,   315,
     513,   436,   480,   482,   257,    14,   481,   441,   372,   440,
     115,   116,   117,    88,   329,    -1,   444,   330,    -1,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,    -1,   331,    -1,    -1,   142,    -1,    -1,
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
      86,    -1,    -1,    -1,    -1,    -1,    -1,    93,    94,    -1,
      -1,    -1,    98,    99,    62,   101,    -1,    -1,   104,    -1,
      -1,    69,    70,    71,    -1,    -1,    -1,    -1,    -1,   115,
     116,   117,   118,   119,   120,   121,   122,   123,    -1,    -1,
      -1,   127,    -1,   129,   130,   131,   132,   133,   134,   135,
     136,   137,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   110,   111,   112,    -1,   114,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   144,   145,   146,   147,
     148,   149,   150,    -1,    -1,    -1,    -1,    -1,    -1,   157
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned short int
  BisonParser::yystos_[] =
  {
         0,   176,   177,     0,    81,    91,   178,     7,   179,    93,
      94,   258,     7,     3,   180,     3,     4,    10,    21,    23,
      41,    66,    67,    68,    75,    76,    83,    85,    86,    98,
      99,   101,   104,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   127,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   181,   189,   190,   191,   196,   197,   198,   206,
     208,   223,   244,   245,   258,   209,    23,    53,    98,    99,
     100,   113,   207,     6,     3,     6,     6,     6,     6,     6,
       3,     6,   197,     3,     3,     6,     3,     3,     3,   191,
      10,   206,   208,     6,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    24,    25,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    38,    39,    41,
      42,    45,    46,    47,    48,    49,    50,    51,    52,    54,
      55,    56,    57,    60,    61,    65,    76,    82,    84,    87,
      88,    89,    90,    98,   124,   125,   126,   127,   128,   129,
     131,   132,   133,   134,   135,   142,   148,   157,   210,   211,
     213,   214,   219,   220,   222,   228,   242,   243,   244,   246,
      11,    11,    11,   199,    11,    11,    21,   212,   213,   228,
      26,    65,   151,   200,   201,   238,   238,    96,    97,   167,
     171,   172,   259,   260,   261,   262,   263,   264,   265,   266,
     266,   266,   266,   192,   266,   185,   186,   258,   182,   143,
     229,    62,    69,    70,    71,   110,   111,   112,   114,   144,
     145,   146,   147,   148,   149,   150,   157,   202,   205,   230,
     231,   232,   233,   234,   235,   236,   237,    41,    42,   102,
     103,   203,   204,    91,   105,   106,   107,   160,   249,   250,
     252,   253,   258,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     4,     5,   210,     5,   210,   238,
     216,   217,   266,   229,    11,    37,    38,    39,    40,   247,
      43,    44,   255,     6,    11,     5,     6,     4,     5,   260,
     260,   266,   164,   165,   166,   167,   168,   169,   170,   163,
     162,   161,   193,     4,     9,     5,    17,    18,    19,    41,
      42,    51,    54,    98,   129,   131,   132,   133,   134,   135,
     148,   157,   183,   184,     6,     6,     6,     4,     5,   202,
       6,     6,     6,     6,     4,     5,   203,     6,     6,     6,
       3,     4,     9,     4,     5,     4,   253,    95,   138,   224,
      95,   139,   225,    95,   140,   226,   141,   227,   124,   219,
     228,   266,   152,   240,   153,   239,   154,   241,   155,   266,
     266,   266,   266,   266,   266,   266,   158,   248,   266,   266,
     215,   221,   266,   218,   266,   258,   258,    91,   258,   266,
     266,   266,   266,   258,   266,   266,   266,   229,   258,   258,
     258,   258,   258,   258,   210,   210,     5,   202,     6,     6,
       6,     9,     6,     6,     5,   228,   258,   212,   266,   200,
     173,   260,   260,   260,   261,   261,   262,   262,   263,   264,
     265,     4,    21,    63,    64,    72,    73,    74,    76,    77,
      78,    79,    80,   127,   194,   195,   196,   206,   185,    76,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     4,     5,   183,   266,   266,   266,
     202,   266,   266,    92,   266,   203,   159,   251,   266,   266,
     254,   250,   252,     9,   172,   216,   266,   266,   248,   247,
     156,   256,    58,    59,   257,   255,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     5,   194,     6,   224,   225,
     226,   266,   266,   187,   188,   266,   258,   229,   258,   258,
     258,   258,   258,   258,   183,   252,   215,   174,   266,   228,
     266,   266,   266,   266,   266,   266,    91,   266,   194,   266,
       9,   172,     4,   173,   173,   187,   174,   266,   173,   173
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
         0,   175,   176,   177,   177,   178,   179,   178,   180,   180,
     180,   180,   180,   180,   180,   180,   182,   181,   183,   183,
     183,   184,   184,   184,   184,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   184,   184,   184,   184,   185,   185,
     186,   186,   187,   187,   188,   188,   188,   189,   189,   189,
     189,   189,   189,   189,   189,   190,   190,   192,   191,   193,
     193,   193,   193,   194,   194,   194,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   196,   196,
     197,   197,   197,   198,   199,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   198,   198,   200,   200,
     201,   201,   201,   202,   202,   202,   203,   203,   203,   204,
     204,   204,   204,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   207,
     206,   206,   209,   208,   210,   210,   210,   210,   210,   210,
     211,   211,   211,   211,   211,   211,   212,   212,   213,   213,
     213,   213,   213,   214,   214,   215,   215,   216,   216,   217,
     218,   219,   219,   219,   219,   219,   219,   219,   219,   219,
     219,   219,   219,   219,   219,   219,   219,   220,   220,   221,
     221,   221,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   222,   223,   223,
     223,   223,   223,   223,   223,   223,   224,   224,   225,   225,
     226,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   242,   242,
     242,   242,   242,   242,   242,   242,   242,   242,   242,   243,
     243,   243,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   245,   245,   245,   245,   246,   246,   246,   247,   247,
     247,   247,   247,   248,   249,   249,   250,   250,   250,   251,
     252,   252,   254,   253,   253,   255,   255,   255,   256,   257,
     257,   258,   258,   259,   259,   259,   260,   260,   260,   261,
     261,   261,   261,   262,   262,   262,   263,   263,   263,   264,
     264,   265,   265,   266,   266
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
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
       3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     3,     1,     1,     1,     3,     1,     0,
       5,     1,     0,     4,     1,     1,     3,     3,     2,     2,
       3,     3,     3,     3,     3,     3,     1,     3,     1,     1,
       1,     1,     1,     3,     3,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       4,     4,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     3,     3,
       3,     3,     1,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     3,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     1,     3,     1,     1,     3,     3,     3,     3,     1,
       1,     3,     0,     5,     2,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     2,     2,     1,
       3,     3,     3,     1,     3,     3,     1,     3,     3,     1,
       3,     1,     3,     1,     3
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
       176,     0,    -1,   177,    -1,    -1,   177,   178,    -1,    81,
       7,   258,    -1,    -1,    91,   179,     7,     3,   180,     4,
      -1,    -1,   180,   206,    -1,   180,   198,    -1,   180,   190,
      -1,   180,   181,    -1,   180,   189,    -1,   180,   208,    -1,
     180,   196,    -1,    -1,    86,     3,   182,   183,     4,    -1,
     184,    -1,   184,     5,   183,    -1,   184,   183,    -1,    -1,
      18,     6,   225,    -1,    98,     6,   229,    -1,   129,     6,
     258,    -1,    17,     6,   224,    -1,   131,     6,   258,    -1,
     132,     6,   258,    -1,   133,     6,   258,    -1,   134,     6,
     258,    -1,   135,     6,   258,    -1,    54,     6,   258,    -1,
      41,     6,   266,    -1,    42,     6,   266,    -1,    19,     6,
     226,    -1,   157,    -1,    51,     6,   187,    -1,   148,    -1,
     186,    -1,   186,     9,   185,    -1,   258,     5,    76,     6,
     266,    -1,    -1,   188,    -1,   188,     9,   187,    -1,   266,
      -1,   266,   172,   266,   173,    -1,   266,   172,   174,   173,
      -1,    76,     6,   266,    -1,    66,     6,   266,    -1,    67,
       6,   266,    -1,    68,     6,   266,    -1,   223,     6,   258,
      -1,    98,     6,   229,    -1,    21,     6,   228,    -1,    41,
       6,   266,    -1,   191,    -1,   190,   191,    -1,    -1,    75,
       3,   192,   193,     4,    -1,    -1,   193,   194,    -1,   193,
     206,    -1,   193,   196,    -1,   195,    -1,   195,     5,   194,
      -1,   195,   194,    -1,    76,     6,   266,    -1,    77,     6,
      91,    -1,    78,    -1,    79,    -1,   127,    -1,    80,     6,
     266,    -1,    21,     6,   228,    -1,    72,     6,   266,    -1,
      73,     6,   266,    -1,    74,     6,   266,    -1,    63,     6,
     266,    -1,    64,     6,   266,    -1,    83,   197,    -1,   197,
      -1,    -1,   197,   206,    -1,   197,   208,    -1,    10,    98,
      11,   229,    -1,    -1,    10,    99,   199,    11,   202,    -1,
      99,     3,   202,     4,    -1,    23,     3,   200,     4,    -1,
      10,    23,    11,   238,    -1,    23,     6,   238,    -1,    10,
     100,    11,   247,    -1,    10,   113,    11,   255,    -1,    10,
      53,    11,   216,    -1,   101,     3,   203,     4,    -1,    85,
       3,   185,     4,    -1,   104,     3,   249,     4,    -1,   104,
       3,   252,     4,    -1,   104,     3,   253,     4,    -1,   201,
      -1,   201,     5,   200,    -1,   238,    -1,    65,    -1,    26,
       6,   266,    -1,   205,    -1,   205,     5,   202,    -1,   205,
     202,    -1,   204,    -1,   204,     5,   203,    -1,   204,   203,
      -1,    41,     6,   266,    -1,    42,     6,   266,    -1,   102,
       6,    92,    -1,   103,     6,   266,    -1,   230,    -1,   231,
      -1,   233,    -1,   234,    -1,   235,    -1,   237,    -1,   236,
      -1,   110,    -1,   112,     6,   266,    -1,   232,    -1,   114,
       6,   266,    -1,    69,    -1,    70,    -1,    71,    -1,   111,
       6,   266,    -1,    62,    -1,    -1,    10,   207,   212,    11,
     258,    -1,   258,    -1,    -1,     3,   209,   210,     4,    -1,
     213,    -1,   211,    -1,   211,     5,   210,    -1,   213,     5,
     210,    -1,   211,   210,    -1,   213,   210,    -1,    84,     6,
     258,    -1,    76,     6,   266,    -1,    65,     6,   266,    -1,
      21,     6,   219,    -1,    21,     6,   124,    -1,    56,     6,
      91,    -1,   213,    -1,   213,     5,   212,    -1,   219,    -1,
     222,    -1,   242,    -1,   243,    -1,   214,    -1,    51,     6,
     215,    -1,    52,     6,   218,    -1,   221,    -1,   221,     9,
     215,    -1,   217,    -1,   217,     5,   216,    -1,   266,    -1,
     266,    -1,    12,    -1,    45,    -1,    46,    -1,    47,    -1,
      48,    -1,    49,    -1,    50,    -1,    14,    -1,    15,    -1,
      13,    -1,   127,    -1,   125,    -1,   244,    -1,   246,    -1,
     228,    -1,    16,    -1,    28,    -1,    28,     6,   155,    -1,
     266,    -1,   266,   172,   266,   173,    -1,   266,   172,   174,
     173,    -1,    17,     6,   224,    -1,    18,     6,   225,    -1,
      19,     6,   226,    -1,    20,     6,   227,    -1,    25,     6,
     239,    -1,    24,     6,   240,    -1,    27,     6,   241,    -1,
     129,     6,   258,    -1,   131,     6,   258,    -1,   132,     6,
     258,    -1,   133,     6,   258,    -1,   134,     6,   258,    -1,
      39,     6,   248,    -1,    38,     6,   266,    -1,   135,     6,
     258,    -1,   220,    -1,    21,     6,   228,    -1,    98,     6,
     229,    -1,    41,     6,   266,    -1,    42,     6,   266,    -1,
     157,    -1,    60,     6,   266,    -1,    61,     6,   266,    -1,
     148,    -1,    82,    -1,    87,    -1,   127,    -1,   244,    -1,
     118,    -1,   119,    -1,   120,    -1,   121,    -1,   122,    -1,
     123,    -1,    95,    -1,   138,    -1,    95,    -1,   139,    -1,
      95,    -1,   140,    -1,   141,    -1,   142,    -1,   143,    -1,
     144,    -1,   145,    -1,   157,    -1,   146,    -1,   147,    -1,
     148,    -1,   149,    -1,   150,    -1,   151,    -1,   153,    -1,
     152,    -1,   154,    -1,    29,     6,   266,    -1,    30,     6,
     266,    -1,    31,     6,   266,    -1,    32,     6,   266,    -1,
      33,     6,   266,    -1,    34,     6,   266,    -1,    35,    -1,
      36,    -1,    22,     6,   266,    -1,    88,     6,   266,    -1,
      89,     6,   266,    -1,    90,     6,   266,    -1,    54,     6,
     258,    -1,    55,     6,   258,    -1,    57,     6,   258,    -1,
     129,    -1,   245,    -1,   135,    -1,   136,    -1,   137,    -1,
     116,    -1,   115,    -1,   117,    -1,   130,    -1,   131,    -1,
     132,    -1,   133,    -1,   134,    -1,   128,    -1,   124,    -1,
     126,    -1,    37,     6,   266,    -1,    39,     6,   248,    -1,
      38,     6,   266,    -1,    40,    -1,   247,     9,   247,    -1,
     158,    -1,   250,    -1,   250,     9,   250,    -1,   105,     6,
     251,    -1,   107,     6,   266,    -1,   106,     6,   266,    -1,
     159,    -1,    91,    -1,   252,     5,   252,    -1,    -1,   160,
       3,   254,   252,     4,    -1,   253,   253,    -1,    43,     6,
     256,    -1,    44,     6,   257,    -1,   255,     5,   255,    -1,
     156,    -1,    58,    -1,    59,    -1,    93,    -1,    94,    -1,
      97,    -1,    96,    -1,   172,   266,   173,    -1,   259,    -1,
     167,   260,    -1,   171,   260,    -1,   260,    -1,   261,   164,
     260,    -1,   261,   165,   260,    -1,   261,   166,   260,    -1,
     261,    -1,   262,   167,   261,    -1,   262,   168,   261,    -1,
     262,    -1,   263,   169,   262,    -1,   263,   170,   262,    -1,
     263,    -1,   264,   163,   263,    -1,   264,    -1,   265,   162,
     264,    -1,   265,    -1,   266,   161,   265,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     5,     6,     9,    13,    14,    21,    22,
      25,    28,    31,    34,    37,    40,    43,    44,    50,    52,
      56,    59,    60,    64,    68,    72,    76,    80,    84,    88,
      92,    96,   100,   104,   108,   112,   114,   118,   120,   122,
     126,   132,   133,   135,   139,   141,   146,   151,   155,   159,
     163,   167,   171,   175,   179,   183,   185,   188,   189,   195,
     196,   199,   202,   205,   207,   211,   214,   218,   222,   224,
     226,   228,   232,   236,   240,   244,   248,   252,   256,   259,
     261,   262,   265,   268,   273,   274,   280,   285,   290,   295,
     299,   304,   309,   314,   319,   324,   329,   334,   339,   341,
     345,   347,   349,   353,   355,   359,   362,   364,   368,   371,
     375,   379,   383,   387,   389,   391,   393,   395,   397,   399,
     401,   403,   407,   409,   413,   415,   417,   419,   423,   425,
     426,   432,   434,   435,   440,   442,   444,   448,   452,   455,
     458,   462,   466,   470,   474,   478,   482,   484,   488,   490,
     492,   494,   496,   498,   502,   506,   508,   512,   514,   518,
     520,   522,   524,   526,   528,   530,   532,   534,   536,   538,
     540,   542,   544,   546,   548,   550,   552,   554,   556,   560,
     562,   567,   572,   576,   580,   584,   588,   592,   596,   600,
     604,   608,   612,   616,   620,   624,   628,   632,   634,   638,
     642,   646,   650,   652,   656,   660,   662,   664,   666,   668,
     670,   672,   674,   676,   678,   680,   682,   684,   686,   688,
     690,   692,   694,   696,   698,   700,   702,   704,   706,   708,
     710,   712,   714,   716,   718,   720,   722,   724,   728,   732,
     736,   740,   744,   748,   750,   752,   756,   760,   764,   768,
     772,   776,   780,   782,   784,   786,   788,   790,   792,   794,
     796,   798,   800,   802,   804,   806,   808,   810,   812,   816,
     820,   824,   826,   830,   832,   834,   838,   842,   846,   850,
     852,   854,   858,   859,   865,   868,   872,   876,   880,   882,
     884,   886,   888,   890,   892,   894,   898,   900,   903,   906,
     908,   912,   916,   920,   922,   926,   930,   932,   936,   940,
     942,   946,   948,   952,   954
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   208,   208,   210,   211,   214,   215,   215,   221,   222,
     223,   224,   225,   226,   227,   228,   231,   231,   236,   237,
     238,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,   260,   261,   262,   263,   267,   268,
     271,   272,   274,   275,   278,   280,   282,   286,   287,   288,
     289,   290,   291,   292,   293,   296,   297,   300,   300,   309,
     310,   311,   312,   315,   316,   317,   320,   321,   322,   323,
     324,   325,   327,   328,   329,   330,   331,   334,   339,   340,
     343,   344,   345,   348,   351,   351,   354,   355,   356,   361,
     362,   363,   364,   365,   366,   367,   368,   369,   372,   373,
     376,   377,   378,   381,   382,   383,   386,   387,   388,   391,
     392,   393,   394,   397,   400,   402,   404,   406,   408,   410,
     412,   414,   416,   418,   422,   423,   424,   425,   426,   429,
     429,   436,   442,   442,   448,   449,   450,   451,   452,   453,
     456,   459,   460,   462,   463,   464,   469,   470,   473,   474,
     475,   476,   477,   480,   481,   484,   485,   488,   489,   492,
     497,   499,   500,   501,   502,   503,   504,   505,   506,   507,
     508,   511,   512,   513,   514,   515,   516,   519,   520,   523,
     524,   525,   527,   528,   529,   530,   531,   532,   533,   534,
     535,   536,   537,   538,   539,   540,   541,   542,   543,   544,
     545,   546,   547,   548,   549,   550,   551,   552,   555,   556,
     557,   558,   559,   560,   561,   562,   565,   566,   569,   570,
     573,   574,   577,   580,   583,   586,   589,   592,   595,   598,
     601,   604,   607,   610,   613,   616,   619,   622,   623,   624,
     625,   626,   627,   628,   629,   630,   631,   632,   633,   636,
     637,   638,   641,   642,   643,   644,   645,   646,   647,   648,
     649,   652,   653,   654,   655,   658,   659,   660,   663,   666,
     669,   672,   675,   678,   682,   683,   685,   686,   687,   689,
     693,   698,   700,   700,   702,   704,   707,   708,   711,   714,
     717,   723,   724,   727,   728,   729,   735,   736,   737,   741,
     742,   743,   744,   747,   748,   749,   752,   753,   754,   757,
     758,   761,   762,   766,   767
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
  const int BisonParser::yylast_ = 1079;
  const int BisonParser::yynnts_ = 92;
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
#line 2657 "bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 772 "parser/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
