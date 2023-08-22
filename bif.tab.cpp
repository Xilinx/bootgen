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
#line 53 "../s/bif.y"

#include <stdio.h>
#include "bootimage.h"
#include "authentication.h"
#include "encryption.h"
#include "checksum.h"
#include "bifscanner.h"
#include "parsing.h"
#include "imageheadertable-versal.h"

BifOptions* currentBifOptions;
PartitionBifOptions* currentPartitionBifOptions ;
ImageBifOptions* currentImageBifOptions;

/* Line 279 of lalr1.cc  */
#line 53 "../bisonflex/bif.tab.cpp"


#include "bif.tab.hpp"

/* User implementation prologue.  */

/* Line 285 of lalr1.cc  */
#line 61 "../bisonflex/bif.tab.cpp"
/* Unqualified %code blocks.  */
/* Line 286 of lalr1.cc  */
#line 47 "../s/bif.y"

    static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner);
    #include "options.h"


/* Line 286 of lalr1.cc  */
#line 71 "../bisonflex/bif.tab.cpp"


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
#line 24 "../s/bif.y"
namespace BIF {
/* Line 353 of lalr1.cc  */
#line 167 "../bisonflex/bif.tab.cpp"

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
#line 31 "../s/bif.y"
{
       // Initialize the initial location.
       yylloc.begin.filename = yylloc.end.filename = &scanner.filename;
}
/* Line 545 of lalr1.cc  */
#line 331 "../bisonflex/bif.tab.cpp"

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
#line 204 "../s/bif.y"
    { options.includeBifOptionsList.push_back((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 205 "../s/bif.y"
    { currentBifOptions = new BifOptions(options.GetArchType(),(yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 7:
/* Line 670 of lalr1.cc  */
#line 207 "../s/bif.y"
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 221 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries()); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 232 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 233 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t), options.IsVersalNetSeries()); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 234 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 235 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 236 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 237 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 238 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 239 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 240 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 241 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 242 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.revokeId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 243 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 34:
/* Line 670 of lalr1.cc  */
#line 244 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 36:
/* Line 670 of lalr1.cc  */
#line 246 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 39:
/* Line 670 of lalr1.cc  */
#line 254 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ihtOptionalDataInfo.push_back(std::pair<std::string, uint32_t>((yysemantic_stack_[(5) - (1)].string), (yysemantic_stack_[(5) - (5)].number))); }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 261 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 263 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 265 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 269 "../s/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 270 "../s/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 271 "../s/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 272 "../s/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 273 "../s/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 274 "../s/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 275 "../s/bif.y"
    { currentBifOptions->SetPdiType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 276 "../s/bif.y"
    { currentBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number));}
    break;

  case 56:
/* Line 670 of lalr1.cc  */
#line 283 "../s/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 284 "../s/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 298 "../s/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 299 "../s/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 67:
/* Line 670 of lalr1.cc  */
#line 300 "../s/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 68:
/* Line 670 of lalr1.cc  */
#line 301 "../s/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 302 "../s/bif.y"
    { LOG_ERROR("BIF attribute error !!!\n\t This usage of 'init' is not supported. See 'bootgen -bif_help init' for usage details."); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 303 "../s/bif.y"
    { LOG_ERROR("Copy to Memory feature with the attribute 'copy' is no more supported.\n\t   This can be duplicated with the option 'imagestore'. Please refer UG1283 for more details.");
                                                                                  currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 71:
/* Line 670 of lalr1.cc  */
#line 305 "../s/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 72:
/* Line 670 of lalr1.cc  */
#line 306 "../s/bif.y"
    { currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 307 "../s/bif.y"
    { currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 308 "../s/bif.y"
    { currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 75:
/* Line 670 of lalr1.cc  */
#line 309 "../s/bif.y"
    { if(!options.IsVersalNetSeries())
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr' is supported only for VERSAL NET architecture");
                                                                                  currentImageBifOptions->SetPcrNumber((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 76:
/* Line 670 of lalr1.cc  */
#line 312 "../s/bif.y"
    { if(!options.IsVersalNetSeries())
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr_mid' is supported only for VERSAL NET architecture");
                                                                                  currentImageBifOptions->SetPcrMeasurementIndex((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 82:
/* Line 670 of lalr1.cc  */
#line 326 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported in VERSAL architecture.\n\t   Please see 'bootgen -arch versal -bif_help keysrc'");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 83:
/* Line 670 of lalr1.cc  */
#line 329 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 87:
/* Line 670 of lalr1.cc  */
#line 334 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                    if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); 
                                                                                    currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 88:
/* Line 670 of lalr1.cc  */
#line 339 "../s/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); }
    break;

  case 96:
/* Line 670 of lalr1.cc  */
#line 351 "../s/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 97:
/* Line 670 of lalr1.cc  */
#line 352 "../s/bif.y"
    { currentBifOptions->SetBootDevice(BootDevice::IMAGESTORE); }
    break;

  case 98:
/* Line 670 of lalr1.cc  */
#line 353 "../s/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 365 "../s/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 366 "../s/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 106:
/* Line 670 of lalr1.cc  */
#line 367 "../s/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 107:
/* Line 670 of lalr1.cc  */
#line 370 "../s/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 108:
/* Line 670 of lalr1.cc  */
#line 373 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                     LOG_ERROR("BIF attribute error !!! 'bh_auth_enable' is not supported with '-arch versalnet'.\n\t   Bootheader or eFuse authentication will be chosen based on eFuse bits.");
                                                                                  else
                                                                                     currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); 
                                                                                }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 379 "../s/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 110:
/* Line 670 of lalr1.cc  */
#line 381 "../s/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 383 "../s/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 385 "../s/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 387 "../s/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 389 "../s/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 391 "../s/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 393 "../s/bif.y"
    {  if(options.GetArchType() != Arch::VERSAL)
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t\t'dpacm_enable' is supported only in VERSAL architecture");
                                                                                   if(options.GetArchType() == Arch::VERSAL)
                                                                                     LOG_WARNING("boot_config { dpacm_enable } will be deprecated. 'dpacm_enable' should be specified along with the partition. \n            See 'bootgen -bif_help dpacm_enable' for more info.");
                                                                                   currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));
                                                                                }
    break;

  case 117:
/* Line 670 of lalr1.cc  */
#line 399 "../s/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->SetSmapWidth((yysemantic_stack_[(3) - (3)].number));
                                                                                }
    break;

  case 118:
/* Line 670 of lalr1.cc  */
#line 403 "../s/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 119:
/* Line 670 of lalr1.cc  */
#line 404 "../s/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 120:
/* Line 670 of lalr1.cc  */
#line 405 "../s/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 406 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                     currentBifOptions->SetPufRingOscilltorSwapConfigValue((yysemantic_stack_[(3) - (3)].number));
                                                                                  else
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t  'puf_ro_swap' is supported only in VersalNet architecture");
                                                                                 }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 411 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                     currentBifOptions->SetDiceEnable();
                                                                                  else
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t  'dice_enable' is supported only in VersalNet architecture");
                                                                                 }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 418 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(),options.IsVersalNetSeries()); }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 421 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 425 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 431 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries()); }
    break;

  case 134:
/* Line 670 of lalr1.cc  */
#line 445 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 135:
/* Line 670 of lalr1.cc  */
#line 448 "../s/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 136:
/* Line 670 of lalr1.cc  */
#line 449 "../s/bif.y"
    { currentPartitionBifOptions->imageStoreId = (yysemantic_stack_[(3) - (3)].number);
                                                                                  currentPartitionBifOptions->SetPartitionType(PartitionType::IMAGE_STORE_PDI); }
    break;

  case 138:
/* Line 670 of lalr1.cc  */
#line 452 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 453 "../s/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string), currentImageBifOptions->GetImageType());
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 153:
/* Line 670 of lalr1.cc  */
#line 481 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] only supported in ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 486 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 488 "../s/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 489 "../s/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 157:
/* Line 670 of lalr1.cc  */
#line 490 "../s/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 491 "../s/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 492 "../s/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 493 "../s/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 161:
/* Line 670 of lalr1.cc  */
#line 494 "../s/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 495 "../s/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 496 "../s/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 497 "../s/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 500 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 501 "../s/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 502 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 503 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 504 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 505 "../s/bif.y"
    { currentPartitionBifOptions->SetLockStepFlag();}
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 508 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 509 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 512 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 513 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 514 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 516 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 517 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 518 "../s/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 519 "../s/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 520 "../s/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 521 "../s/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 522 "../s/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 523 "../s/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 524 "../s/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 525 "../s/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 526 "../s/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 527 "../s/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 528 "../s/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 189:
/* Line 670 of lalr1.cc  */
#line 529 "../s/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 530 "../s/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 192:
/* Line 670 of lalr1.cc  */
#line 532 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 193:
/* Line 670 of lalr1.cc  */
#line 533 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 194:
/* Line 670 of lalr1.cc  */
#line 534 "../s/bif.y"
    { currentPartitionBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 195:
/* Line 670 of lalr1.cc  */
#line 535 "../s/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 196:
/* Line 670 of lalr1.cc  */
#line 536 "../s/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 197:
/* Line 670 of lalr1.cc  */
#line 537 "../s/bif.y"
    { currentPartitionBifOptions->SetClusterNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 198:
/* Line 670 of lalr1.cc  */
#line 538 "../s/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 539 "../s/bif.y"
    { currentPartitionBifOptions->SetDelayAuth(true); }
    break;

  case 200:
/* Line 670 of lalr1.cc  */
#line 540 "../s/bif.y"
    { currentPartitionBifOptions->SetTcmBootFlag(); }
    break;

  case 209:
/* Line 670 of lalr1.cc  */
#line 553 "../s/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 211:
/* Line 670 of lalr1.cc  */
#line 557 "../s/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 213:
/* Line 670 of lalr1.cc  */
#line 561 "../s/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 230:
/* Line 670 of lalr1.cc  */
#line 610 "../s/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 231:
/* Line 670 of lalr1.cc  */
#line 611 "../s/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 232:
/* Line 670 of lalr1.cc  */
#line 612 "../s/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), false); }
    break;

  case 233:
/* Line 670 of lalr1.cc  */
#line 613 "../s/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), true); }
    break;

  case 234:
/* Line 670 of lalr1.cc  */
#line 614 "../s/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 235:
/* Line 670 of lalr1.cc  */
#line 615 "../s/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 236:
/* Line 670 of lalr1.cc  */
#line 616 "../s/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 237:
/* Line 670 of lalr1.cc  */
#line 617 "../s/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 238:
/* Line 670 of lalr1.cc  */
#line 618 "../s/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 239:
/* Line 670 of lalr1.cc  */
#line 619 "../s/bif.y"
    { currentPartitionBifOptions->SetTcmARegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 240:
/* Line 670 of lalr1.cc  */
#line 620 "../s/bif.y"
    { currentPartitionBifOptions->SetTcmBRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 241:
/* Line 670 of lalr1.cc  */
#line 621 "../s/bif.y"
    { currentPartitionBifOptions->SetTcmCRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 242:
/* Line 670 of lalr1.cc  */
#line 624 "../s/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 243:
/* Line 670 of lalr1.cc  */
#line 625 "../s/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 260:
/* Line 670 of lalr1.cc  */
#line 650 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 261:
/* Line 670 of lalr1.cc  */
#line 653 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 262:
/* Line 670 of lalr1.cc  */
#line 656 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 263:
/* Line 670 of lalr1.cc  */
#line 659 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 266:
/* Line 670 of lalr1.cc  */
#line 668 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 270:
/* Line 670 of lalr1.cc  */
#line 678 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 271:
/* Line 670 of lalr1.cc  */
#line 681 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 276:
/* Line 670 of lalr1.cc  */
#line 693 "../s/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 278:
/* Line 670 of lalr1.cc  */
#line 700 "../s/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 279:
/* Line 670 of lalr1.cc  */
#line 701 "../s/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 281:
/* Line 670 of lalr1.cc  */
#line 706 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 282:
/* Line 670 of lalr1.cc  */
#line 707 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 283:
/* Line 670 of lalr1.cc  */
#line 708 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 285:
/* Line 670 of lalr1.cc  */
#line 712 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 286:
/* Line 670 of lalr1.cc  */
#line 713 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 288:
/* Line 670 of lalr1.cc  */
#line 717 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 289:
/* Line 670 of lalr1.cc  */
#line 718 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 291:
/* Line 670 of lalr1.cc  */
#line 722 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 293:
/* Line 670 of lalr1.cc  */
#line 726 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 295:
/* Line 670 of lalr1.cc  */
#line 731 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1501 "../bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -427;
  const short int
  BisonParser::yypact_[] =
  {
      -427,    33,    22,  -427,     0,  -427,  -427,    59,    65,  -427,
    -427,  -427,    83,  -427,   803,  -427,  -427,   179,    94,   107,
      96,    98,   102,   111,   134,   139,  -427,   165,   182,   186,
     198,   203,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,  -427,   132,  -427,  -427,    61,  -427,  -427,  -427,
     201,  -427,  -427,  -427,   594,   206,   222,   223,  -427,   226,
     227,   732,    78,    21,    97,    92,    92,    92,    92,  -427,
      92,    61,    59,  -427,   104,   836,   -26,  -427,  -427,  -427,
    -427,    59,  -427,  -427,  -427,  -427,  -427,   236,   239,   240,
     241,   242,   244,   245,   250,   251,   252,   253,   254,   255,
     259,   264,   266,  -427,  -427,   268,   279,   283,  -427,  -427,
    -427,  -427,  -427,  -427,   284,   285,   288,   289,   290,   291,
     293,   294,   295,  -427,   296,  -427,   299,   301,   302,   303,
    -427,  -427,  -427,  -427,  -427,   304,   305,   307,   313,   314,
     315,  -427,  -427,  -427,   326,     7,   456,  -427,  -427,  -427,
    -427,  -427,  -427,  -427,  -427,  -427,    97,    92,   104,   277,
      82,   112,   316,   330,   340,  -427,   341,  -427,  -427,   342,
     343,  -427,  -427,  -427,  -427,    92,    92,    92,  -427,  -427,
      11,     3,    38,   196,   209,   199,   199,   199,   199,  -427,
     199,   351,   353,   359,   202,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,   366,   369,   370,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,  -427,   361,   376,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,  -427,   377,   378,   379,   382,    58,  -427,   -25,
     -49,   -34,   249,   322,    92,   243,   246,   256,   257,    92,
      92,    92,    92,    92,    92,    92,   247,    92,    92,    92,
      59,   311,    59,    92,    92,    92,    92,    59,    92,    92,
      92,   104,    59,    59,    59,    59,    59,    59,  -427,   594,
    -427,   594,  -427,  -427,  -427,   398,   199,  -427,   836,   399,
     400,   401,  -427,   402,   403,   404,   413,    78,    59,   732,
      92,  -427,    21,  -427,  -427,   -84,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,   282,  -427,    59,   334,
     414,   415,   417,   419,   420,   421,   422,   426,   427,   428,
     429,   430,   446,  -427,  -427,   451,   191,    92,    92,    92,
    -427,   836,    92,   367,    92,  -427,   -26,  -427,  -427,  -427,
    -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,   199,  -427,  -427,  -427,  -427,  -427,  -427,  -427,
     199,   199,   199,   199,   199,   199,   199,  -427,  -427,   199,
    -427,   449,   -71,  -427,   199,  -427,  -427,  -427,   199,   199,
     199,   199,  -427,   199,   199,   199,  -427,  -427,  -427,  -427,
    -427,  -427,  -427,  -427,  -427,    92,  -427,    92,    92,   247,
      82,   331,   106,   112,  -427,  -427,  -427,   199,  -427,  -427,
    -427,  -427,  -427,    11,    11,     3,     3,    38,   196,   209,
    -427,   453,   476,   487,   490,   492,   493,   502,   506,  -427,
    -427,   507,  -427,  -427,   152,  -427,  -427,  -427,   510,   -25,
     -49,   -34,    92,    92,    59,   104,    59,    59,    59,    59,
      59,    59,  -427,   202,  -427,   199,   199,   199,  -427,   199,
    -427,   199,  -427,    92,   -18,  -427,   199,   199,  -427,   402,
    -427,  -427,  -427,  -427,  -427,   413,    78,    92,    92,    92,
      92,    92,    92,   431,    92,   318,  -427,    92,  -427,  -427,
    -427,   199,  -427,   508,   -55,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,  -427,  -427,  -427,   355,   -78,  -427,   199,   199,
     199,   199,   199,   199,  -427,   199,  -427,   199,    92,    54,
    -427,  -427,  -427,   358,   -73,  -427,  -427
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,     6,     4,     0,     0,   272,
     273,     5,     0,     8,     0,   126,     7,   123,     0,     0,
       0,     0,     0,     0,     0,     0,    79,     0,     0,     0,
       0,     0,   250,   249,   251,   203,   204,   205,   206,   207,
     208,   201,   244,   252,   253,   254,   255,   256,   246,   247,
     248,    12,    13,    11,    54,    15,    78,    10,     9,    14,
       0,   202,   245,   125,     0,     0,     0,     0,    83,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    56,
       0,    77,    40,    16,     0,     0,     0,    55,   123,    80,
      81,     0,   155,   164,   162,   163,   170,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   171,     0,     0,     0,
       0,     0,     0,   236,   237,     0,     0,     0,   156,   157,
     158,   159,   160,   161,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   199,     0,   200,     0,     0,     0,     0,
     258,   166,   259,   165,   257,   244,   253,   254,   255,   256,
     246,   216,   198,   195,     0,   129,   128,   146,   142,   191,
     143,   169,   144,   145,   167,   168,     0,     0,     0,     0,
       0,     0,     0,     0,   140,    52,     0,    97,   226,     0,
      94,    96,    88,   275,   274,     0,     0,     0,   277,   280,
     284,   287,   290,   292,   294,    53,    47,    48,    49,    58,
      46,     0,    37,     0,    21,   217,    51,   122,   118,   119,
     120,   114,     0,     0,     0,   218,   219,   221,   222,   223,
     224,   225,   220,     0,    99,   107,   108,   116,   109,   110,
     111,   113,   112,     0,     0,     0,     0,   101,    50,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   127,     0,
     132,     0,   133,    87,    91,   151,   153,    82,     0,     0,
       0,     0,   263,    89,     0,     0,    90,     0,     0,     0,
       0,    86,     0,   278,   279,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    79,    93,    40,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    36,    34,     0,    18,     0,     0,     0,
      85,     0,     0,     0,     0,    92,     0,   103,   209,   210,
     176,   211,   212,   177,   213,   214,   178,   215,   179,   138,
     137,   169,   238,   228,   181,   227,   180,   229,   182,   172,
     230,   231,   232,   233,   234,   235,   189,   265,   188,   194,
     147,   149,   173,   148,   154,   242,   139,   243,   196,   197,
     136,   135,   134,   239,   240,   241,   193,   183,   184,   185,
     186,   187,   190,   130,   131,     0,    84,     0,     0,     0,
       0,     0,     0,     0,   192,   124,   141,    98,    95,   276,
     281,   282,   283,   285,   286,   288,   289,   291,   293,   295,
      57,     0,     0,     0,     0,     0,     0,     0,     0,    67,
      68,     0,    69,    59,    62,    61,    60,    38,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    17,    21,    20,   121,   115,   117,   100,   104,
     105,   106,   102,     0,     0,   152,   260,   262,   261,   264,
     269,   266,   270,   271,   267,   268,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    64,     0,    25,    22,
      33,    32,    35,    41,    43,    31,    23,    24,    26,    27,
      28,    29,    30,    19,   150,     0,     0,    71,    75,    76,
      72,    73,    74,    65,    66,    70,    63,    39,     0,     0,
     175,   174,    42,     0,     0,    45,    44
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -427,  -427,  -427,  -427,  -427,  -427,  -427,  -427,  -325,  -427,
     204,  -427,    -5,  -427,  -427,  -427,   471,  -427,  -427,  -426,
    -427,   210,   499,  -427,  -427,   225,  -427,  -258,  -228,  -427,
    -427,   -10,  -427,   514,  -427,  -139,  -427,   230,   -63,  -427,
      60,   126,  -427,  -427,   292,  -427,  -427,  -427,  -427,    85,
      87,    81,  -427,   -62,  -162,  -427,  -427,  -427,  -427,  -427,
    -427,  -427,  -427,   -60,  -427,  -427,  -427,  -427,  -427,   525,
    -427,  -427,   130,   136,   133,  -427,  -427,     6,  -427,  -136,
    -110,  -108,   234,   235,   233,   -75
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     6,     8,    14,    51,   204,   335,   336,
     201,   202,   502,   503,    52,    53,    54,   199,   316,   443,
     444,    55,    56,    57,   169,   179,   180,   223,   236,   237,
     224,    89,    71,    90,    64,   154,   155,   173,   156,   157,
     380,   284,   285,   383,   158,   159,   381,   160,    60,   350,
     353,   356,   358,   161,   206,   225,   226,   227,   228,   229,
     230,   231,   232,   181,   366,   364,   368,   162,   163,   164,
      62,   165,   293,   378,   296,   481,   484,    63,   188,   189,
     190,   191,   192,   193,   194,   286
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       195,   196,   197,   198,    58,   200,   287,     7,   174,   347,
     175,   464,   279,    11,   182,   233,   280,   282,   496,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     406,   103,   104,     3,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   351,   115,   116,   176,   117,   303,
     304,   118,   119,   120,   121,   122,   123,   124,   125,   354,
     126,   127,   128,   346,    15,   129,   130,   315,   348,   526,
     131,    88,    12,   315,   234,   235,   183,   184,   315,   419,
     315,   132,   352,   468,   177,   531,    13,   133,   203,   134,
     536,   474,   135,   136,   137,   138,   315,   238,   355,   233,
      72,     4,    75,   139,    76,   349,   283,   529,    77,   396,
      73,     5,   305,    74,    32,    33,    34,    78,   472,   289,
     290,   291,   292,   140,   141,   142,   143,   144,   145,    43,
     146,   147,   148,   149,   150,    49,    50,    79,   513,   185,
     403,   151,   404,   186,   187,    80,   515,   152,   183,   184,
       9,    10,     9,    10,   294,   295,   153,   495,   234,   235,
     309,   310,   482,   483,   178,   306,   307,   308,    82,   362,
     420,   421,   422,   431,   370,   371,   372,   373,   374,   375,
     376,   361,   379,   382,   384,    83,   183,   184,   388,   389,
     390,   391,    84,   393,   394,   395,   463,   311,   312,   423,
     424,    85,    65,   425,   426,    24,    86,    91,   320,   321,
     322,   185,   151,   432,   433,   186,   187,   166,   533,   320,
     321,   322,   434,   435,   436,   417,   437,   438,   439,   440,
     441,    66,   323,   167,   168,   414,   174,   170,   171,   205,
     178,   324,   239,   323,   325,   240,   241,   242,   243,   185,
     244,   245,   324,   186,   187,   325,   246,   247,   248,   249,
     250,   251,   465,   466,   467,   252,   385,   469,   387,   471,
     253,   442,   254,   392,   255,    67,    68,    69,   397,   398,
     399,   400,   401,   402,    70,   256,   430,   326,   288,   257,
     258,   259,    88,   506,   260,   261,   262,   263,   326,   264,
     265,   266,   267,   431,   415,   268,   446,   269,   270,   271,
     272,   273,   327,   274,   328,   329,   330,   331,   332,   275,
     276,   277,   297,   327,   203,   328,   329,   330,   331,   332,
     278,   333,   476,   477,    92,    93,    94,    95,    96,   431,
     334,   298,   333,   432,   433,   299,   301,   300,   302,   313,
     315,   334,   434,   435,   436,   317,   437,   438,   439,   440,
     441,   314,   318,    26,   319,   340,   118,   119,   120,   121,
     122,   123,   337,     9,    10,   338,   339,   501,   504,   432,
     433,   341,   357,   342,   343,   344,   345,   363,   434,   435,
     436,   365,   437,   438,   439,   440,   441,   377,   382,   516,
     386,   442,   367,   405,   369,   407,   408,   409,   448,   411,
     412,   410,   518,   519,   520,   521,   522,   523,   413,   525,
     449,   450,   527,   451,   517,   452,   453,   454,   455,    32,
      33,    34,   456,   457,   458,   459,   460,   442,   359,   141,
     142,   143,   144,    42,    43,    44,    45,    46,    47,    48,
      49,    50,   461,   504,   534,   462,   151,   470,   473,   486,
     505,   281,   507,   508,   509,   510,   511,   512,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   480,
     103,   104,   487,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   488,   115,   116,   489,   117,   490,   491,
     118,   119,   120,   121,   122,   123,   124,   125,   492,   126,
     127,   128,   493,   494,   129,   130,   497,   528,   530,   131,
     524,   535,   447,   532,    87,    81,   445,   418,    59,   416,
     132,   475,   500,   514,   498,   360,   133,   499,   134,    61,
     479,   135,   136,   137,   138,   478,   485,   427,   429,   428,
       0,     0,   139,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    32,    33,    34,     0,     0,     0,     0,
       0,     0,   140,   141,   142,   143,   144,   145,    43,   146,
     147,   148,   149,   150,    49,    50,     0,     0,     0,     0,
     151,     0,     0,     0,     0,     0,   152,     0,     0,     0,
       0,     0,     0,     0,     0,   153,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,     0,   103,   104,
       0,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,     0,   115,   116,     0,   117,     0,     0,   118,   119,
     120,   121,   122,   123,   124,   125,     0,   126,   127,   128,
       0,     0,   129,   130,     0,     0,     0,   131,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   132,     0,
       0,     0,     0,     0,   133,     0,   134,     0,     0,   135,
     136,   137,   138,     0,     0,     0,     0,     0,     0,     0,
     139,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    32,    33,    34,     0,     0,     0,     0,     0,     0,
     140,   141,   142,   143,   144,   145,    43,   146,   147,   148,
     149,   150,    49,    50,     0,     0,     0,     0,   151,     0,
       0,     0,     0,     0,   152,     0,     0,     0,     0,     0,
       0,     0,     0,   153,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   172,   102,     0,   103,   104,     0,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,     0,
     115,   116,     0,   117,     0,     0,   118,   119,   120,   121,
     122,   123,   124,   125,     0,   126,     0,   128,     0,     0,
     129,   130,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    15,    16,     0,     0,
       0,     0,   133,    17,     0,     0,     0,   135,   136,   137,
     138,     0,     0,     0,    18,     0,    19,     0,   139,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    32,
      33,    34,     0,     0,    20,     0,     0,     0,   140,   141,
     142,   143,   144,   145,    43,   146,   147,   148,   149,   150,
      49,    50,     0,     0,     0,     0,   151,    21,    22,    23,
       0,     0,   152,     0,     0,     0,    24,    25,     0,     0,
       0,   153,     0,     0,    26,     0,    27,    28,     0,     0,
       0,     0,     0,     0,     9,    10,   207,     0,     0,    29,
      30,     0,    31,   208,   209,   210,     0,     0,     0,     0,
      32,    33,    34,    35,    36,    37,    38,    39,    40,     0,
       0,     0,    41,     0,    42,    43,    44,    45,    46,    47,
      48,    49,    50,     0,     0,     0,     0,     0,   211,   212,
     213,     0,   214,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   215,   216,   217,   218,   219,   220,   221,     0,
       0,     0,     0,     0,     0,   222
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        75,    76,    77,    78,    14,    80,   168,     7,    71,   237,
      72,   336,     5,     7,    74,    41,   155,   156,   444,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
     288,    24,    25,     0,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    93,    38,    39,    26,    41,   185,
     186,    44,    45,    46,    47,    48,    49,    50,    51,    93,
      53,    54,    55,     5,     3,    58,    59,   151,    93,   495,
      63,    10,     7,   151,   100,   101,    94,    95,   151,   163,
     151,    74,   131,   341,    63,   163,     3,    80,    82,    82,
     163,   162,    85,    86,    87,    88,   151,    91,   132,    41,
       6,    79,     6,    96,     6,   130,   166,   162,     6,   271,
       3,    89,   187,     6,   107,   108,   109,     6,   346,    37,
      38,    39,    40,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,     3,   463,   157,
     279,   134,   281,   161,   162,     6,   164,   140,    94,    95,
      91,    92,    91,    92,    42,    43,   149,     5,   100,   101,
     157,   158,    56,    57,   143,   154,   155,   156,     3,   244,
     306,   307,   308,    21,   249,   250,   251,   252,   253,   254,
     255,   243,   257,   258,   259,     3,    94,    95,   263,   264,
     265,   266,     6,   268,   269,   270,     5,   159,   160,   309,
     310,     3,    23,   311,   312,    73,     3,     6,    17,    18,
      19,   157,   134,    61,    62,   161,   162,    11,   164,    17,
      18,    19,    70,    71,    72,   300,    74,    75,    76,    77,
      78,    52,    41,    11,    11,   297,   299,    11,    11,   135,
     143,    50,     6,    41,    53,     6,     6,     6,     6,   157,
       6,     6,    50,   161,   162,    53,     6,     6,     6,     6,
       6,     6,   337,   338,   339,     6,   260,   342,   262,   344,
       6,   119,     6,   267,     6,    96,    97,    98,   272,   273,
     274,   275,   276,   277,   105,     6,     4,    96,    11,     6,
       6,     6,    10,   455,     6,     6,     6,     6,    96,     6,
       6,     6,     6,    21,   298,     6,   316,     6,     6,     6,
       6,     6,   121,     6,   123,   124,   125,   126,   127,     6,
       6,     6,     6,   121,   318,   123,   124,   125,   126,   127,
       4,   140,   407,   408,    12,    13,    14,    15,    16,    21,
     149,    11,   140,    61,    62,     5,     4,     6,     5,   153,
     151,   149,    70,    71,    72,     4,    74,    75,    76,    77,
      78,   152,     9,    81,     5,     4,    44,    45,    46,    47,
      48,    49,     6,    91,    92,     6,     6,   452,   453,    61,
      62,     5,   133,     6,     6,     6,     4,   144,    70,    71,
      72,   145,    74,    75,    76,    77,    78,   150,   473,   474,
      89,   119,   146,     5,   147,     6,     6,     6,    74,     6,
       6,     9,   487,   488,   489,   490,   491,   492,     5,   494,
       6,     6,   497,     6,   486,     6,     6,     6,     6,   107,
     108,   109,     6,     6,     6,     6,     6,   119,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,     6,   528,   529,     4,   134,    90,     9,     6,
     454,     5,   456,   457,   458,   459,   460,   461,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,   148,
      24,    25,     6,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,     6,    38,    39,     6,    41,     6,     6,
      44,    45,    46,    47,    48,    49,    50,    51,     6,    53,
      54,    55,     6,     6,    58,    59,     6,     9,   163,    63,
      89,   163,   318,   528,    53,    26,   316,   302,    14,   299,
      74,   405,   451,   473,   449,   243,    80,   450,    82,    14,
     410,    85,    86,    87,    88,   409,   413,   313,   315,   314,
      -1,    -1,    96,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   107,   108,   109,    -1,    -1,    -1,    -1,
      -1,    -1,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,    -1,    -1,    -1,    -1,
     134,    -1,    -1,    -1,    -1,    -1,   140,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   149,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    -1,    24,    25,
      -1,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    -1,    38,    39,    -1,    41,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    -1,    53,    54,    55,
      -1,    -1,    58,    59,    -1,    -1,    -1,    63,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    74,    -1,
      -1,    -1,    -1,    -1,    80,    -1,    82,    -1,    -1,    85,
      86,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      96,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   107,   108,   109,    -1,    -1,    -1,    -1,    -1,    -1,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    -1,    -1,    -1,    -1,   134,    -1,
      -1,    -1,    -1,    -1,   140,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   149,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    -1,    24,    25,    -1,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    -1,
      38,    39,    -1,    41,    -1,    -1,    44,    45,    46,    47,
      48,    49,    50,    51,    -1,    53,    -1,    55,    -1,    -1,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,
      -1,    -1,    80,    10,    -1,    -1,    -1,    85,    86,    87,
      88,    -1,    -1,    -1,    21,    -1,    23,    -1,    96,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   107,
     108,   109,    -1,    -1,    41,    -1,    -1,    -1,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,    -1,    -1,    -1,    -1,   134,    64,    65,    66,
      -1,    -1,   140,    -1,    -1,    -1,    73,    74,    -1,    -1,
      -1,   149,    -1,    -1,    81,    -1,    83,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    91,    92,    60,    -1,    -1,    96,
      97,    -1,    99,    67,    68,    69,    -1,    -1,    -1,    -1,
     107,   108,   109,   110,   111,   112,   113,   114,   115,    -1,
      -1,    -1,   119,    -1,   121,   122,   123,   124,   125,   126,
     127,   128,   129,    -1,    -1,    -1,    -1,    -1,   102,   103,
     104,    -1,   106,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   136,   137,   138,   139,   140,   141,   142,    -1,
      -1,    -1,    -1,    -1,    -1,   149
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   166,   167,     0,    79,    89,   168,     7,   169,    91,
      92,   242,     7,     3,   170,     3,     4,    10,    21,    23,
      41,    64,    65,    66,    73,    74,    81,    83,    84,    96,
      97,    99,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   119,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   171,   179,   180,   181,   186,   187,   188,   196,   198,
     213,   234,   235,   242,   199,    23,    52,    96,    97,    98,
     105,   197,     6,     3,     6,     6,     6,     6,     6,     3,
       6,   187,     3,     3,     6,     3,     3,   181,    10,   196,
     198,     6,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    24,    25,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    38,    39,    41,    44,    45,
      46,    47,    48,    49,    50,    51,    53,    54,    55,    58,
      59,    63,    74,    80,    82,    85,    86,    87,    88,    96,
     116,   117,   118,   119,   120,   121,   123,   124,   125,   126,
     127,   134,   140,   149,   200,   201,   203,   204,   209,   210,
     212,   218,   232,   233,   234,   236,    11,    11,    11,   189,
      11,    11,    21,   202,   203,   218,    26,    63,   143,   190,
     191,   228,   228,    94,    95,   157,   161,   162,   243,   244,
     245,   246,   247,   248,   249,   250,   250,   250,   250,   182,
     250,   175,   176,   242,   172,   135,   219,    60,    67,    68,
      69,   102,   103,   104,   106,   136,   137,   138,   139,   140,
     141,   142,   149,   192,   195,   220,   221,   222,   223,   224,
     225,   226,   227,    41,   100,   101,   193,   194,   242,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     4,     5,
     200,     5,   200,   228,   206,   207,   250,   219,    11,    37,
      38,    39,    40,   237,    42,    43,   239,     6,    11,     5,
       6,     4,     5,   244,   244,   250,   154,   155,   156,   157,
     158,   159,   160,   153,   152,   151,   183,     4,     9,     5,
      17,    18,    19,    41,    50,    53,    96,   121,   123,   124,
     125,   126,   127,   140,   149,   173,   174,     6,     6,     6,
       4,     5,     6,     6,     6,     4,     5,   193,    93,   130,
     214,    93,   131,   215,    93,   132,   216,   133,   217,   116,
     209,   218,   250,   144,   230,   145,   229,   146,   231,   147,
     250,   250,   250,   250,   250,   250,   250,   150,   238,   250,
     205,   211,   250,   208,   250,   242,    89,   242,   250,   250,
     250,   250,   242,   250,   250,   250,   219,   242,   242,   242,
     242,   242,   242,   200,   200,     5,   192,     6,     6,     6,
       9,     6,     6,     5,   218,   242,   202,   250,   190,   163,
     244,   244,   244,   245,   245,   246,   246,   247,   248,   249,
       4,    21,    61,    62,    70,    71,    72,    74,    75,    76,
      77,    78,   119,   184,   185,   186,   196,   175,    74,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     4,     5,   173,   250,   250,   250,   192,   250,
      90,   250,   193,     9,   162,   206,   250,   250,   238,   237,
     148,   240,    56,    57,   241,   239,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     5,   184,     6,   214,   215,
     216,   250,   177,   178,   250,   242,   219,   242,   242,   242,
     242,   242,   242,   173,   205,   164,   250,   218,   250,   250,
     250,   250,   250,   250,    89,   250,   184,   250,     9,   162,
     163,   163,   177,   164,   250,   163,   163
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
     415,   416,   417,   418,   419
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   165,   166,   167,   167,   168,   169,   168,   170,   170,
     170,   170,   170,   170,   170,   170,   172,   171,   173,   173,
     173,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   175,   175,   176,
     176,   177,   177,   178,   178,   178,   179,   179,   179,   179,
     179,   179,   179,   179,   180,   180,   182,   181,   183,   183,
     183,   183,   184,   184,   184,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   186,   186,   187,
     187,   187,   188,   189,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   190,   190,   191,   191,   191,   192,
     192,   193,   193,   193,   194,   194,   194,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   197,   196,   196,   199,   198,   200,   200,
     200,   200,   200,   200,   201,   201,   201,   201,   201,   201,
     202,   202,   203,   203,   203,   203,   203,   204,   204,   205,
     205,   206,   206,   207,   208,   209,   209,   209,   209,   209,
     209,   209,   209,   209,   209,   209,   209,   209,   209,   209,
     209,   210,   210,   211,   211,   211,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   212,   212,   212,   212,   212,   212,   212,   212,   212,
     212,   213,   213,   213,   213,   213,   213,   213,   213,   214,
     214,   215,   215,   216,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   232,   232,   232,   232,   232,   232,   232,   232,   232,
     232,   232,   233,   233,   234,   234,   234,   234,   234,   234,
     234,   234,   234,   235,   235,   235,   235,   236,   236,   236,
     237,   237,   237,   237,   237,   238,   239,   239,   239,   240,
     241,   241,   242,   242,   243,   243,   243,   244,   244,   244,
     245,   245,   245,   245,   246,   246,   246,   247,   247,   247,
     248,   248,   249,   249,   250,   250
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     3,     0,     6,     0,     2,
       2,     2,     2,     2,     2,     2,     0,     5,     1,     3,
       2,     0,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     3,     1,     1,     3,     5,
       0,     1,     3,     1,     4,     4,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     2,     0,     5,     0,     2,
       2,     2,     1,     3,     2,     3,     3,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     2,     1,     0,
       2,     2,     4,     0,     5,     4,     4,     4,     3,     4,
       4,     4,     4,     4,     1,     3,     1,     1,     3,     1,
       3,     1,     3,     2,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     3,     1,     1,
       1,     3,     1,     0,     5,     1,     0,     4,     1,     1,
       3,     3,     2,     2,     3,     3,     3,     3,     3,     3,
       1,     3,     1,     1,     1,     1,     1,     3,     3,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     4,     4,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     3,     3,     3,     1,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     3,     3,     3,     1,     1,     3,     3,
       3,     3,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     1,     3,     1,     3,     3,     3,     1,
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
  "SPK_ID", "SPK_SELECT", "HEADER_AUTH", "REVOKE_ID", "SPLIT_MODE",
  "SPLIT_FMT", "BOOT", "USER", "STATIC", "NOAUTOSTART", "MULTIBOOT",
  "PROTECTED", "BLOCKS", "AUTHBLOCKS", "BOOTVECTORS", "PRESIGN",
  "BIF_SECTION", "UDF_DATA", "MCS", "BIN", "SLR_NUM", "CLUSTER_NUM",
  "DICE", "PCR_NUMBER", "PCR_MEASUREMENT_INDEX", "IMAGE_STORE",
  "PARENT_ID", "ID_CODE", "EXT_ID_CODE", "BYPASS_IDCODE_CHECK", "A_HWROT",
  "S_HWROT", "UNIQUE_ID", "PARENT_UNIQUE_ID", "FUNCTION_ID", "IMAGE", "ID",
  "NAME", "DELAY_HANDOFF", "DELAY_LOAD", "COPY", "INCLUDE", "DELAY_AUTH",
  "PARTITION", "PFILE", "OPTIONAL_DATA", "METAHEADER", "TCM_BOOT",
  "TCM_A_REGION", "TCM_B_REGION", "TCM_C_REGION", "WORD", "HEXWORD",
  "FILENAME", "QFILENAME", "NONE", "DECVALUE", "HEXVALUE",
  "KEYSRC_ENCRYPTION", "FSBL_CONFIG", "AUTH_PARAMS", "AUTHJTAG_CONFIG",
  "DEVICE_DNA", "JTAG_TIMEOUT", "PUF4KMODE", "PUFROSWAP", "SHUTTER",
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
  "SPKSELECT", "OR", "XOR", "AND", "MULT", "DIVIDE", "MODULO", "PLUS",
  "MINUS", "LSHIFT", "RSHIFT", "NEGATION", "LPAREN", "RPAREN", "ASTERISK",
  "$accept", "bif", "group_list", "bifoptions", "$@1", "file_list",
  "metahdr_spec", "$@2", "metahdr_attr_list", "metahdr_attr",
  "optional_data", "optional_data_attr", "metahdr_blk", "metahdr_blk_attr",
  "new_pdi_spec", "image_spec", "image_list", "$@3", "image_content",
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
  "spkselect", "split_options", "splitmode", "splitfmt", "filename",
  "number", "unary_expression", "multiplicative_expression",
  "additive_expression", "shift_expression", "and_expression",
  "xor_expression", "expression", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BisonParser::rhs_number_type
  BisonParser::yyrhs_[] =
  {
       166,     0,    -1,   167,    -1,    -1,   167,   168,    -1,    79,
       7,   242,    -1,    -1,    89,   169,     7,     3,   170,     4,
      -1,    -1,   170,   196,    -1,   170,   188,    -1,   170,   180,
      -1,   170,   171,    -1,   170,   179,    -1,   170,   198,    -1,
     170,   186,    -1,    -1,    84,     3,   172,   173,     4,    -1,
     174,    -1,   174,     5,   173,    -1,   174,   173,    -1,    -1,
      18,     6,   215,    -1,    96,     6,   219,    -1,   121,     6,
     242,    -1,    17,     6,   214,    -1,   123,     6,   242,    -1,
     124,     6,   242,    -1,   125,     6,   242,    -1,   126,     6,
     242,    -1,   127,     6,   242,    -1,    53,     6,   242,    -1,
      41,     6,   250,    -1,    19,     6,   216,    -1,   149,    -1,
      50,     6,   177,    -1,   140,    -1,   176,    -1,   176,     9,
     175,    -1,   242,     5,    74,     6,   250,    -1,    -1,   178,
      -1,   178,     9,   177,    -1,   250,    -1,   250,   162,   250,
     163,    -1,   250,   162,   164,   163,    -1,    74,     6,   250,
      -1,    64,     6,   250,    -1,    65,     6,   250,    -1,    66,
       6,   250,    -1,   213,     6,   242,    -1,    96,     6,   219,
      -1,    21,     6,   218,    -1,    41,     6,   250,    -1,   181,
      -1,   180,   181,    -1,    -1,    73,     3,   182,   183,     4,
      -1,    -1,   183,   184,    -1,   183,   196,    -1,   183,   186,
      -1,   185,    -1,   185,     5,   184,    -1,   185,   184,    -1,
      74,     6,   250,    -1,    75,     6,    89,    -1,    76,    -1,
      77,    -1,   119,    -1,    78,     6,   250,    -1,    21,     6,
     218,    -1,    70,     6,   250,    -1,    71,     6,   250,    -1,
      72,     6,   250,    -1,    61,     6,   250,    -1,    62,     6,
     250,    -1,    81,   187,    -1,   187,    -1,    -1,   187,   196,
      -1,   187,   198,    -1,    10,    96,    11,   219,    -1,    -1,
      10,    97,   189,    11,   192,    -1,    97,     3,   192,     4,
      -1,    23,     3,   190,     4,    -1,    10,    23,    11,   228,
      -1,    23,     6,   228,    -1,    10,    98,    11,   237,    -1,
      10,   105,    11,   239,    -1,    10,    52,    11,   206,    -1,
      99,     3,   193,     4,    -1,    83,     3,   175,     4,    -1,
     191,    -1,   191,     5,   190,    -1,   228,    -1,    63,    -1,
      26,     6,   250,    -1,   195,    -1,   195,     5,   192,    -1,
     194,    -1,   194,     5,   193,    -1,   194,   193,    -1,    41,
       6,   250,    -1,   100,     6,    90,    -1,   101,     6,   250,
      -1,   220,    -1,   221,    -1,   223,    -1,   224,    -1,   225,
      -1,   227,    -1,   226,    -1,   102,    -1,   104,     6,   250,
      -1,   222,    -1,   106,     6,   250,    -1,    67,    -1,    68,
      -1,    69,    -1,   103,     6,   250,    -1,    60,    -1,    -1,
      10,   197,   202,    11,   242,    -1,   242,    -1,    -1,     3,
     199,   200,     4,    -1,   203,    -1,   201,    -1,   201,     5,
     200,    -1,   203,     5,   200,    -1,   201,   200,    -1,   203,
     200,    -1,    82,     6,   242,    -1,    74,     6,   250,    -1,
      63,     6,   250,    -1,    21,     6,   209,    -1,    21,     6,
     116,    -1,    54,     6,    89,    -1,   203,    -1,   203,     5,
     202,    -1,   209,    -1,   212,    -1,   232,    -1,   233,    -1,
     204,    -1,    50,     6,   205,    -1,    51,     6,   208,    -1,
     211,    -1,   211,     9,   205,    -1,   207,    -1,   207,     5,
     206,    -1,   250,    -1,   250,    -1,    12,    -1,    44,    -1,
      45,    -1,    46,    -1,    47,    -1,    48,    -1,    49,    -1,
      14,    -1,    15,    -1,    13,    -1,   119,    -1,   117,    -1,
     234,    -1,   236,    -1,   218,    -1,    16,    -1,    28,    -1,
      28,     6,   147,    -1,   250,    -1,   250,   162,   250,   163,
      -1,   250,   162,   164,   163,    -1,    17,     6,   214,    -1,
      18,     6,   215,    -1,    19,     6,   216,    -1,    20,     6,
     217,    -1,    25,     6,   229,    -1,    24,     6,   230,    -1,
      27,     6,   231,    -1,   121,     6,   242,    -1,   123,     6,
     242,    -1,   124,     6,   242,    -1,   125,     6,   242,    -1,
     126,     6,   242,    -1,    39,     6,   238,    -1,    38,     6,
     250,    -1,   127,     6,   242,    -1,   210,    -1,    21,     6,
     218,    -1,    96,     6,   219,    -1,    41,     6,   250,    -1,
     149,    -1,    58,     6,   250,    -1,    59,     6,   250,    -1,
     140,    -1,    80,    -1,    85,    -1,   119,    -1,   234,    -1,
     110,    -1,   111,    -1,   112,    -1,   113,    -1,   114,    -1,
     115,    -1,    93,    -1,   130,    -1,    93,    -1,   131,    -1,
      93,    -1,   132,    -1,   133,    -1,   134,    -1,   135,    -1,
     136,    -1,   137,    -1,   149,    -1,   138,    -1,   139,    -1,
     140,    -1,   141,    -1,   142,    -1,   143,    -1,   145,    -1,
     144,    -1,   146,    -1,    29,     6,   250,    -1,    30,     6,
     250,    -1,    31,     6,   250,    -1,    32,     6,   250,    -1,
      33,     6,   250,    -1,    34,     6,   250,    -1,    35,    -1,
      36,    -1,    22,     6,   250,    -1,    86,     6,   250,    -1,
      87,     6,   250,    -1,    88,     6,   250,    -1,    53,     6,
     242,    -1,    55,     6,   242,    -1,   121,    -1,   235,    -1,
     127,    -1,   128,    -1,   129,    -1,   108,    -1,   107,    -1,
     109,    -1,   122,    -1,   123,    -1,   124,    -1,   125,    -1,
     126,    -1,   120,    -1,   116,    -1,   118,    -1,    37,     6,
     250,    -1,    39,     6,   238,    -1,    38,     6,   250,    -1,
      40,    -1,   237,     9,   237,    -1,   150,    -1,    42,     6,
     240,    -1,    43,     6,   241,    -1,   239,     5,   239,    -1,
     148,    -1,    56,    -1,    57,    -1,    91,    -1,    92,    -1,
      95,    -1,    94,    -1,   162,   250,   163,    -1,   243,    -1,
     157,   244,    -1,   161,   244,    -1,   244,    -1,   245,   154,
     244,    -1,   245,   155,   244,    -1,   245,   156,   244,    -1,
     245,    -1,   246,   157,   245,    -1,   246,   158,   245,    -1,
     246,    -1,   247,   159,   246,    -1,   247,   160,   246,    -1,
     247,    -1,   248,   153,   247,    -1,   248,    -1,   249,   152,
     248,    -1,   249,    -1,   250,   151,   249,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     5,     6,     9,    13,    14,    21,    22,
      25,    28,    31,    34,    37,    40,    43,    44,    50,    52,
      56,    59,    60,    64,    68,    72,    76,    80,    84,    88,
      92,    96,   100,   104,   108,   110,   114,   116,   118,   122,
     128,   129,   131,   135,   137,   142,   147,   151,   155,   159,
     163,   167,   171,   175,   179,   181,   184,   185,   191,   192,
     195,   198,   201,   203,   207,   210,   214,   218,   220,   222,
     224,   228,   232,   236,   240,   244,   248,   252,   255,   257,
     258,   261,   264,   269,   270,   276,   281,   286,   291,   295,
     300,   305,   310,   315,   320,   322,   326,   328,   330,   334,
     336,   340,   342,   346,   349,   353,   357,   361,   363,   365,
     367,   369,   371,   373,   375,   377,   381,   383,   387,   389,
     391,   393,   397,   399,   400,   406,   408,   409,   414,   416,
     418,   422,   426,   429,   432,   436,   440,   444,   448,   452,
     456,   458,   462,   464,   466,   468,   470,   472,   476,   480,
     482,   486,   488,   492,   494,   496,   498,   500,   502,   504,
     506,   508,   510,   512,   514,   516,   518,   520,   522,   524,
     526,   528,   530,   534,   536,   541,   546,   550,   554,   558,
     562,   566,   570,   574,   578,   582,   586,   590,   594,   598,
     602,   606,   608,   612,   616,   620,   622,   626,   630,   632,
     634,   636,   638,   640,   642,   644,   646,   648,   650,   652,
     654,   656,   658,   660,   662,   664,   666,   668,   670,   672,
     674,   676,   678,   680,   682,   684,   686,   688,   690,   692,
     694,   698,   702,   706,   710,   714,   718,   720,   722,   726,
     730,   734,   738,   742,   746,   748,   750,   752,   754,   756,
     758,   760,   762,   764,   766,   768,   770,   772,   774,   776,
     778,   782,   786,   790,   792,   796,   798,   802,   806,   810,
     812,   814,   816,   818,   820,   822,   824,   828,   830,   833,
     836,   838,   842,   846,   850,   852,   856,   860,   862,   866,
     870,   872,   876,   878,   882,   884
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   198,   198,   200,   201,   204,   205,   205,   211,   212,
     213,   214,   215,   216,   217,   218,   221,   221,   226,   227,
     228,   231,   232,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   250,   251,   254,
     255,   257,   258,   261,   263,   265,   269,   270,   271,   272,
     273,   274,   275,   276,   279,   280,   283,   283,   287,   288,
     289,   290,   293,   294,   295,   298,   299,   300,   301,   302,
     303,   305,   306,   307,   308,   309,   312,   317,   318,   321,
     322,   323,   326,   329,   329,   332,   333,   334,   339,   340,
     341,   342,   343,   344,   347,   348,   351,   352,   353,   356,
     357,   360,   361,   362,   365,   366,   367,   370,   373,   379,
     381,   383,   385,   387,   389,   391,   393,   399,   403,   404,
     405,   406,   411,   418,   418,   425,   431,   431,   437,   438,
     439,   440,   441,   442,   445,   448,   449,   451,   452,   453,
     458,   459,   462,   463,   464,   465,   466,   469,   470,   473,
     474,   477,   478,   481,   486,   488,   489,   490,   491,   492,
     493,   494,   495,   496,   497,   500,   501,   502,   503,   504,
     505,   508,   509,   512,   513,   514,   516,   517,   518,   519,
     520,   521,   522,   523,   524,   525,   526,   527,   528,   529,
     530,   531,   532,   533,   534,   535,   536,   537,   538,   539,
     540,   543,   544,   545,   546,   547,   548,   549,   550,   553,
     554,   557,   558,   561,   562,   565,   568,   571,   574,   577,
     580,   583,   586,   589,   592,   595,   598,   601,   604,   607,
     610,   611,   612,   613,   614,   615,   616,   617,   618,   619,
     620,   621,   624,   625,   628,   629,   630,   631,   632,   633,
     634,   635,   636,   639,   640,   641,   642,   645,   646,   647,
     650,   653,   656,   659,   662,   665,   668,   671,   672,   675,
     678,   681,   687,   688,   691,   692,   693,   699,   700,   701,
     705,   706,   707,   708,   711,   712,   713,   716,   717,   718,
     721,   722,   725,   726,   730,   731
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
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 985;
  const int BisonParser::yynnts_ = 86;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 165;

  const unsigned int BisonParser::yyuser_token_number_max_ = 419;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 24 "../s/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 2568 "../bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 736 "../s/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
