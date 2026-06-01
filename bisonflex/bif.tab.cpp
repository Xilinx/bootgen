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
      case 259: /* filename */
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
    { currentBifOptions = new BifOptions(options.GetArchType(), options.IsVersalNetSeries(), options.IsDl9Series(), (yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 7:
/* Line 670 of lalr1.cc  */
#line 220 "parser/bif.y"
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 234 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(), currentBifOptions->GetIdCode()); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 245 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 246 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 247 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 248 "parser/bif.y"
    { currentBifOptions->SetMetaHeaderAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 249 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 250 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 251 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 252 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 253 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 254 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 255 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.partitionRevokeId = (yysemantic_stack_[(3) - (3)].number);}
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 256 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.spkRevokeId = (yysemantic_stack_[(3) - (3)].number); 
                                                                                if(options.GetArchType() == Arch::VERSAL)
                                                                                {
                                                                                   LOG_ERROR("SPK_REVOKE_ID does not support for versal arch, instead please use revoke_id option.");
                                                                                }
                                                                                }
    break;

  case 34:
/* Line 670 of lalr1.cc  */
#line 263 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 264 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 37:
/* Line 670 of lalr1.cc  */
#line 266 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 274 "parser/bif.y"
    { currentBifOptions->metaHdrAttributes.ihtOptionalDataInfo.push_back(std::pair<std::string, uint32_t>((yysemantic_stack_[(5) - (1)].string), (yysemantic_stack_[(5) - (5)].number))); }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 281 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 283 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 285 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 289 "parser/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 290 "parser/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 291 "parser/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); 
                                                                                  options.SetDl9Series((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 293 "parser/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 294 "parser/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 295 "parser/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 296 "parser/bif.y"
    { currentBifOptions->SetPdiType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 297 "parser/bif.y"
    { currentBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number));}
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 304 "parser/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 305 "parser/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions);
                                                                                  if((options.archType == Arch::SPARTANUP) && (currentBifOptions->imageBifOptionList.size() > 1))
                                                                                  {
                                                                                      LOG_ERROR("'-arch spartanup' cannot accomodate more than one Sub-System.");
                                                                                  }
                                                                                }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 324 "parser/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 67:
/* Line 670 of lalr1.cc  */
#line 325 "parser/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 68:
/* Line 670 of lalr1.cc  */
#line 326 "parser/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 327 "parser/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 328 "parser/bif.y"
    { LOG_ERROR("BIF attribute error !!!\n\t This usage of 'init' is not supported. See 'bootgen -bif_help init' for usage details."); }
    break;

  case 71:
/* Line 670 of lalr1.cc  */
#line 329 "parser/bif.y"
    { LOG_ERROR("Copy to Memory feature with the attribute 'copy' is no more supported.\n\t   This can be duplicated with the option 'imagestore'. Please refer UG1283 for more details.");
                                                                                  currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 72:
/* Line 670 of lalr1.cc  */
#line 331 "parser/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 332 "parser/bif.y"
    { if ((yysemantic_stack_[(3) - (3)].number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("uid value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 337 "parser/bif.y"
    { if ((yysemantic_stack_[(3) - (3)].number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("parent_uid value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 75:
/* Line 670 of lalr1.cc  */
#line 342 "parser/bif.y"
    { if ((yysemantic_stack_[(3) - (3)].number) == 0xFFFFFFFF)
                                                                                  {
                                                                                    LOG_WARNING("function_id value 0xFFFFFFFF is reserved and not allowed, it will be treated as unset");
                                                                                  }
                                                                                  currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 76:
/* Line 670 of lalr1.cc  */
#line 347 "parser/bif.y"
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

  case 77:
/* Line 670 of lalr1.cc  */
#line 364 "parser/bif.y"
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

  case 83:
/* Line 670 of lalr1.cc  */
#line 384 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQ || options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported for the specified architecture.\n\t   Refer 'bootgen -bif_help' for more details");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 84:
/* Line 670 of lalr1.cc  */
#line 387 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 88:
/* Line 670 of lalr1.cc  */
#line 392 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ)
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                  if(options.GetArchType() != Arch::ZYNQMP)
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); 
                                                                                  currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 89:
/* Line 670 of lalr1.cc  */
#line 397 "parser/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Refer 'bootgen -bif_help' for more details"); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 412 "parser/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 413 "parser/bif.y"
    { currentBifOptions->SetBootDevice(BootDevice::IMAGESTORE); }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 414 "parser/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 427 "parser/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 110:
/* Line 670 of lalr1.cc  */
#line 428 "parser/bif.y"
    { currentBifOptions->SetAuthJtagSPKRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 429 "parser/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 430 "parser/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 431 "parser/bif.y"
    { currentBifOptions->SetAuthJtagSignatureFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 434 "parser/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 437 "parser/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 439 "parser/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 117:
/* Line 670 of lalr1.cc  */
#line 441 "parser/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 118:
/* Line 670 of lalr1.cc  */
#line 443 "parser/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 119:
/* Line 670 of lalr1.cc  */
#line 445 "parser/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 120:
/* Line 670 of lalr1.cc  */
#line 447 "parser/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 449 "parser/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 451 "parser/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 453 "parser/bif.y"
    {  currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));}
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 455 "parser/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->SetSmapWidth((yysemantic_stack_[(3) - (3)].number));
                                                                                }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 459 "parser/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 460 "parser/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 461 "parser/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 462 "parser/bif.y"
    { currentBifOptions->SetPufRingOscilltorSwapConfigValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 129:
/* Line 670 of lalr1.cc  */
#line 463 "parser/bif.y"
    { currentBifOptions->SetDiceEnable(); }
    break;

  case 130:
/* Line 670 of lalr1.cc  */
#line 466 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(),options.IsVersalNetSeries(),  currentBifOptions->GetIdCode()); }
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 469 "parser/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 473 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 133:
/* Line 670 of lalr1.cc  */
#line 479 "parser/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries(),  currentBifOptions->GetIdCode()); }
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 493 "parser/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 496 "parser/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 497 "parser/bif.y"
    { currentPartitionBifOptions->imageStoreId = (yysemantic_stack_[(3) - (3)].number);
                                                                                  currentPartitionBifOptions->SetPartitionType(PartitionType::IMAGE_STORE_PDI); }
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 500 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 501 "parser/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string), currentImageBifOptions->GetImageType());
                                                                                  currentPartitionBifOptions->filelist.push_back(currentPartitionBifOptions->filename);
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 530 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] is supported only for ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 161:
/* Line 670 of lalr1.cc  */
#line 535 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 537 "parser/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 538 "parser/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 539 "parser/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 540 "parser/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 541 "parser/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 542 "parser/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 543 "parser/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 544 "parser/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 545 "parser/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 546 "parser/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 549 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 550 "parser/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 551 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 552 "parser/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 553 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 554 "parser/bif.y"
    { currentPartitionBifOptions->SetLockStepFlag();}
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 557 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 558 "parser/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 561 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 562 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 563 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 565 "parser/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 566 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 567 "parser/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 568 "parser/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 569 "parser/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 570 "parser/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 189:
/* Line 670 of lalr1.cc  */
#line 571 "parser/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 572 "parser/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 191:
/* Line 670 of lalr1.cc  */
#line 573 "parser/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 192:
/* Line 670 of lalr1.cc  */
#line 574 "parser/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 193:
/* Line 670 of lalr1.cc  */
#line 575 "parser/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 194:
/* Line 670 of lalr1.cc  */
#line 576 "parser/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 195:
/* Line 670 of lalr1.cc  */
#line 577 "parser/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 196:
/* Line 670 of lalr1.cc  */
#line 578 "parser/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 197:
/* Line 670 of lalr1.cc  */
#line 579 "parser/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 581 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 200:
/* Line 670 of lalr1.cc  */
#line 582 "parser/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 201:
/* Line 670 of lalr1.cc  */
#line 583 "parser/bif.y"
    { currentPartitionBifOptions->SetPartitionRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 202:
/* Line 670 of lalr1.cc  */
#line 584 "parser/bif.y"
    { currentPartitionBifOptions->SetSPKRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 203:
/* Line 670 of lalr1.cc  */
#line 585 "parser/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 204:
/* Line 670 of lalr1.cc  */
#line 586 "parser/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 205:
/* Line 670 of lalr1.cc  */
#line 587 "parser/bif.y"
    { currentPartitionBifOptions->SetClusterNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 206:
/* Line 670 of lalr1.cc  */
#line 588 "parser/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 207:
/* Line 670 of lalr1.cc  */
#line 589 "parser/bif.y"
    { currentPartitionBifOptions->SetDelayAuth(true); }
    break;

  case 208:
/* Line 670 of lalr1.cc  */
#line 590 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBootFlag(); }
    break;

  case 218:
/* Line 670 of lalr1.cc  */
#line 604 "parser/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 220:
/* Line 670 of lalr1.cc  */
#line 608 "parser/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 222:
/* Line 670 of lalr1.cc  */
#line 612 "parser/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 239:
/* Line 670 of lalr1.cc  */
#line 661 "parser/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 240:
/* Line 670 of lalr1.cc  */
#line 662 "parser/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 241:
/* Line 670 of lalr1.cc  */
#line 663 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), false); }
    break;

  case 242:
/* Line 670 of lalr1.cc  */
#line 664 "parser/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), true); }
    break;

  case 243:
/* Line 670 of lalr1.cc  */
#line 665 "parser/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 244:
/* Line 670 of lalr1.cc  */
#line 666 "parser/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 245:
/* Line 670 of lalr1.cc  */
#line 667 "parser/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 246:
/* Line 670 of lalr1.cc  */
#line 668 "parser/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 247:
/* Line 670 of lalr1.cc  */
#line 669 "parser/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 248:
/* Line 670 of lalr1.cc  */
#line 670 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmARegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 249:
/* Line 670 of lalr1.cc  */
#line 671 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmBRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 250:
/* Line 670 of lalr1.cc  */
#line 672 "parser/bif.y"
    { currentPartitionBifOptions->SetTcmCRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 251:
/* Line 670 of lalr1.cc  */
#line 675 "parser/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 252:
/* Line 670 of lalr1.cc  */
#line 676 "parser/bif.y"
    { currentPartitionBifOptions->acFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 253:
/* Line 670 of lalr1.cc  */
#line 677 "parser/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 270:
/* Line 670 of lalr1.cc  */
#line 702 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 271:
/* Line 670 of lalr1.cc  */
#line 705 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 272:
/* Line 670 of lalr1.cc  */
#line 708 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 273:
/* Line 670 of lalr1.cc  */
#line 711 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 282:
/* Line 670 of lalr1.cc  */
#line 732 "parser/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ || options.GetArchType() == Arch::ZYNQMP || options.GetArchType() == Arch::VERSAL)
                                                                   LOG_ERROR("BIF attribute error !!!\n\t\t'lms_key_params' is not supported with the mentioned -arch");
                                                                 lmsParams.lms_param = (yysemantic_stack_[(1) - (1)].string);
                                                                 currentBifOptions->SetLmsParameters(lmsParams) ;
                                                               }
    break;

  case 284:
/* Line 670 of lalr1.cc  */
#line 739 "parser/bif.y"
    { lmsParams.type = (yysemantic_stack_[(2) - (1)].authkeylevel_t);}
    break;

  case 287:
/* Line 670 of lalr1.cc  */
#line 743 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 291:
/* Line 670 of lalr1.cc  */
#line 753 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 292:
/* Line 670 of lalr1.cc  */
#line 756 "parser/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported with the mentioned -arch");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 297:
/* Line 670 of lalr1.cc  */
#line 768 "parser/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 299:
/* Line 670 of lalr1.cc  */
#line 775 "parser/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 300:
/* Line 670 of lalr1.cc  */
#line 776 "parser/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 302:
/* Line 670 of lalr1.cc  */
#line 781 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 303:
/* Line 670 of lalr1.cc  */
#line 782 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 304:
/* Line 670 of lalr1.cc  */
#line 783 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 306:
/* Line 670 of lalr1.cc  */
#line 787 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 307:
/* Line 670 of lalr1.cc  */
#line 788 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 309:
/* Line 670 of lalr1.cc  */
#line 792 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 310:
/* Line 670 of lalr1.cc  */
#line 793 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 312:
/* Line 670 of lalr1.cc  */
#line 797 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 314:
/* Line 670 of lalr1.cc  */
#line 801 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 316:
/* Line 670 of lalr1.cc  */
#line 806 "parser/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1614 "bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -463;
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

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
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

  /* YYPGOTO[NTERM-NUM].  */
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

  /* YYDEFGOTO[NTERM-NUM].  */
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

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
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

  /* YYCHECK.  */
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

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
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
       177,     0,    -1,   178,    -1,    -1,   178,   179,    -1,    81,
       7,   259,    -1,    -1,    91,   180,     7,     3,   181,     4,
      -1,    -1,   181,   207,    -1,   181,   199,    -1,   181,   191,
      -1,   181,   182,    -1,   181,   190,    -1,   181,   209,    -1,
     181,   197,    -1,    -1,    86,     3,   183,   184,     4,    -1,
     185,    -1,   185,     5,   184,    -1,   185,   184,    -1,    -1,
      18,     6,   226,    -1,    98,     6,   230,    -1,   130,     6,
     259,    -1,    17,     6,   225,    -1,   132,     6,   259,    -1,
     133,     6,   259,    -1,   134,     6,   259,    -1,   135,     6,
     259,    -1,   136,     6,   259,    -1,    54,     6,   259,    -1,
      41,     6,   267,    -1,    42,     6,   267,    -1,    19,     6,
     227,    -1,   158,    -1,    51,     6,   188,    -1,   149,    -1,
     187,    -1,   187,     9,   186,    -1,   259,     5,    76,     6,
     267,    -1,    -1,   189,    -1,   189,     9,   188,    -1,   267,
      -1,   267,   173,   267,   174,    -1,   267,   173,   175,   174,
      -1,    76,     6,   267,    -1,    66,     6,   267,    -1,    67,
       6,   267,    -1,    68,     6,   267,    -1,   224,     6,   259,
      -1,    98,     6,   230,    -1,    21,     6,   229,    -1,    41,
       6,   267,    -1,   192,    -1,   191,   192,    -1,    -1,    75,
       3,   193,   194,     4,    -1,    -1,   194,   195,    -1,   194,
     207,    -1,   194,   197,    -1,   196,    -1,   196,     5,   195,
      -1,   196,   195,    -1,    76,     6,   267,    -1,    77,     6,
      91,    -1,    78,    -1,    79,    -1,   128,    -1,    80,     6,
     267,    -1,    21,     6,   229,    -1,    72,     6,   267,    -1,
      73,     6,   267,    -1,    74,     6,   267,    -1,    63,     6,
     267,    -1,    64,     6,   267,    -1,    83,   198,    -1,   198,
      -1,    -1,   198,   207,    -1,   198,   209,    -1,    10,    98,
      11,   230,    -1,    -1,    10,    99,   200,    11,   203,    -1,
      99,     3,   203,     4,    -1,    23,     3,   201,     4,    -1,
      10,    23,    11,   239,    -1,    23,     6,   239,    -1,    10,
     100,    11,   248,    -1,    10,   114,    11,   256,    -1,    10,
      53,    11,   217,    -1,   101,     3,   204,     4,    -1,    85,
       3,   186,     4,    -1,   105,     3,   250,     4,    -1,   105,
       3,   253,     4,    -1,   105,     3,   254,     4,    -1,   202,
      -1,   202,     5,   201,    -1,   239,    -1,    65,    -1,    26,
       6,   267,    -1,   206,    -1,   206,     5,   203,    -1,   206,
     203,    -1,   205,    -1,   205,     5,   204,    -1,   205,   204,
      -1,    41,     6,   267,    -1,    42,     6,   267,    -1,   102,
       6,    92,    -1,   103,     6,   267,    -1,   104,     6,   259,
      -1,   231,    -1,   232,    -1,   234,    -1,   235,    -1,   236,
      -1,   238,    -1,   237,    -1,   111,    -1,   113,     6,   267,
      -1,   233,    -1,   115,     6,   267,    -1,    69,    -1,    70,
      -1,    71,    -1,   112,     6,   267,    -1,    62,    -1,    -1,
      10,   208,   213,    11,   259,    -1,   259,    -1,    -1,     3,
     210,   211,     4,    -1,   214,    -1,   212,    -1,   212,     5,
     211,    -1,   214,     5,   211,    -1,   212,   211,    -1,   214,
     211,    -1,    84,     6,   259,    -1,    76,     6,   267,    -1,
      65,     6,   267,    -1,    21,     6,   220,    -1,    21,     6,
     125,    -1,    56,     6,    91,    -1,   214,    -1,   214,     5,
     213,    -1,   220,    -1,   223,    -1,   243,    -1,   244,    -1,
     215,    -1,    51,     6,   216,    -1,    52,     6,   219,    -1,
     222,    -1,   222,     9,   216,    -1,   218,    -1,   218,     5,
     217,    -1,   267,    -1,   267,    -1,    12,    -1,    45,    -1,
      46,    -1,    47,    -1,    48,    -1,    49,    -1,    50,    -1,
      14,    -1,    15,    -1,    13,    -1,   128,    -1,   126,    -1,
     245,    -1,   247,    -1,   229,    -1,    16,    -1,    28,    -1,
      28,     6,   156,    -1,   267,    -1,   267,   173,   267,   174,
      -1,   267,   173,   175,   174,    -1,    17,     6,   225,    -1,
      18,     6,   226,    -1,    19,     6,   227,    -1,    20,     6,
     228,    -1,    25,     6,   240,    -1,    24,     6,   241,    -1,
      27,     6,   242,    -1,   130,     6,   259,    -1,   132,     6,
     259,    -1,   133,     6,   259,    -1,   134,     6,   259,    -1,
     135,     6,   259,    -1,    39,     6,   249,    -1,    38,     6,
     267,    -1,   136,     6,   259,    -1,   221,    -1,    21,     6,
     229,    -1,    98,     6,   230,    -1,    41,     6,   267,    -1,
      42,     6,   267,    -1,   158,    -1,    60,     6,   267,    -1,
      61,     6,   267,    -1,   149,    -1,    82,    -1,    87,    -1,
     128,    -1,   245,    -1,   119,    -1,   118,    -1,   120,    -1,
     121,    -1,   122,    -1,   123,    -1,   124,    -1,    95,    -1,
     139,    -1,    95,    -1,   140,    -1,    95,    -1,   141,    -1,
     142,    -1,   143,    -1,   144,    -1,   145,    -1,   146,    -1,
     158,    -1,   147,    -1,   148,    -1,   149,    -1,   150,    -1,
     151,    -1,   152,    -1,   154,    -1,   153,    -1,   155,    -1,
      29,     6,   267,    -1,    30,     6,   267,    -1,    31,     6,
     267,    -1,    32,     6,   267,    -1,    33,     6,   267,    -1,
      34,     6,   267,    -1,    35,    -1,    36,    -1,    22,     6,
     267,    -1,    88,     6,   267,    -1,    89,     6,   267,    -1,
      90,     6,   267,    -1,    54,     6,   259,    -1,    55,     6,
     259,    -1,    57,     6,   259,    -1,   130,    -1,   246,    -1,
     136,    -1,   137,    -1,   138,    -1,   117,    -1,   116,    -1,
     118,    -1,   131,    -1,   132,    -1,   133,    -1,   134,    -1,
     135,    -1,   129,    -1,   125,    -1,   127,    -1,    37,     6,
     267,    -1,    39,     6,   249,    -1,    38,     6,   267,    -1,
      40,    -1,   248,     9,   248,    -1,   159,    -1,   251,    -1,
     251,     9,   251,    -1,   106,     6,   252,    -1,   108,     6,
     267,    -1,   107,     6,   267,    -1,   160,    -1,    91,    -1,
     253,     5,   253,    -1,    -1,   161,     3,   255,   253,     4,
      -1,   254,   254,    -1,    43,     6,   257,    -1,    44,     6,
     258,    -1,   256,     5,   256,    -1,   157,    -1,    58,    -1,
      59,    -1,    93,    -1,    94,    -1,    97,    -1,    96,    -1,
     173,   267,   174,    -1,   260,    -1,   168,   261,    -1,   172,
     261,    -1,   261,    -1,   262,   165,   261,    -1,   262,   166,
     261,    -1,   262,   167,   261,    -1,   262,    -1,   263,   168,
     262,    -1,   263,   169,   262,    -1,   263,    -1,   264,   170,
     263,    -1,   264,   171,   263,    -1,   264,    -1,   265,   164,
     264,    -1,   265,    -1,   266,   163,   265,    -1,   266,    -1,
     267,   162,   266,    -1
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
     375,   379,   383,   387,   391,   393,   395,   397,   399,   401,
     403,   405,   407,   411,   413,   417,   419,   421,   423,   427,
     429,   430,   436,   438,   439,   444,   446,   448,   452,   456,
     459,   462,   466,   470,   474,   478,   482,   486,   488,   492,
     494,   496,   498,   500,   502,   506,   510,   512,   516,   518,
     522,   524,   526,   528,   530,   532,   534,   536,   538,   540,
     542,   544,   546,   548,   550,   552,   554,   556,   558,   560,
     564,   566,   571,   576,   580,   584,   588,   592,   596,   600,
     604,   608,   612,   616,   620,   624,   628,   632,   636,   638,
     642,   646,   650,   654,   656,   660,   664,   666,   668,   670,
     672,   674,   676,   678,   680,   682,   684,   686,   688,   690,
     692,   694,   696,   698,   700,   702,   704,   706,   708,   710,
     712,   714,   716,   718,   720,   722,   724,   726,   728,   730,
     734,   738,   742,   746,   750,   754,   756,   758,   762,   766,
     770,   774,   778,   782,   786,   788,   790,   792,   794,   796,
     798,   800,   802,   804,   806,   808,   810,   812,   814,   816,
     818,   822,   826,   830,   832,   836,   838,   840,   844,   848,
     852,   856,   858,   860,   864,   865,   871,   874,   878,   882,
     886,   888,   890,   892,   894,   896,   898,   900,   904,   906,
     909,   912,   914,   918,   922,   926,   928,   932,   936,   938,
     942,   946,   948,   952,   954,   958,   960
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
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
  const int BisonParser::yylast_ = 1081;
  const int BisonParser::yynnts_ = 92;
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
#line 2741 "bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 811 "parser/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
