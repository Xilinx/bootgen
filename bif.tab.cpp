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

  case 105:
/* Line 670 of lalr1.cc  */
#line 366 "../s/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 106:
/* Line 670 of lalr1.cc  */
#line 367 "../s/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 107:
/* Line 670 of lalr1.cc  */
#line 368 "../s/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 108:
/* Line 670 of lalr1.cc  */
#line 371 "../s/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 374 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                     LOG_ERROR("BIF attribute error !!! 'bh_auth_enable' is not supported with '-arch versalnet'.\n\t   Bootheader or eFuse authentication will be chosen based on eFuse bits.");
                                                                                  else
                                                                                     currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); 
                                                                                }
    break;

  case 110:
/* Line 670 of lalr1.cc  */
#line 380 "../s/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 382 "../s/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 384 "../s/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 386 "../s/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 388 "../s/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 390 "../s/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 392 "../s/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 117:
/* Line 670 of lalr1.cc  */
#line 394 "../s/bif.y"
    {  if(options.GetArchType() != Arch::VERSAL)
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t\t'dpacm_enable' is supported only in VERSAL architecture");
                                                                                   if(options.GetArchType() == Arch::VERSAL)
                                                                                     LOG_WARNING("boot_config { dpacm_enable } will be deprecated. 'dpacm_enable' should be specified along with the partition. \n            See 'bootgen -bif_help dpacm_enable' for more info.");
                                                                                   currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));
                                                                                }
    break;

  case 118:
/* Line 670 of lalr1.cc  */
#line 400 "../s/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->SetSmapWidth((yysemantic_stack_[(3) - (3)].number));
                                                                                }
    break;

  case 119:
/* Line 670 of lalr1.cc  */
#line 404 "../s/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 120:
/* Line 670 of lalr1.cc  */
#line 405 "../s/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 406 "../s/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 407 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                     currentBifOptions->SetPufRingOscilltorSwapConfigValue((yysemantic_stack_[(3) - (3)].number));
                                                                                  else
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t  'puf_ro_swap' is supported only in VersalNet architecture");
                                                                                 }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 412 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                     currentBifOptions->SetDiceEnable();
                                                                                  else
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t  'dice_enable' is supported only in VersalNet architecture");
                                                                                 }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 419 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(),options.IsVersalNetSeries()); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 422 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 426 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 432 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries()); }
    break;

  case 135:
/* Line 670 of lalr1.cc  */
#line 446 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 136:
/* Line 670 of lalr1.cc  */
#line 449 "../s/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 137:
/* Line 670 of lalr1.cc  */
#line 450 "../s/bif.y"
    { currentPartitionBifOptions->imageStoreId = (yysemantic_stack_[(3) - (3)].number);
                                                                                  currentPartitionBifOptions->SetPartitionType(PartitionType::IMAGE_STORE_PDI); }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 453 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 454 "../s/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string), currentImageBifOptions->GetImageType());
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 482 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] only supported in ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 487 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 489 "../s/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 157:
/* Line 670 of lalr1.cc  */
#line 490 "../s/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 491 "../s/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 492 "../s/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 493 "../s/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 161:
/* Line 670 of lalr1.cc  */
#line 494 "../s/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 495 "../s/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 496 "../s/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 497 "../s/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 498 "../s/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 501 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 502 "../s/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 503 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 504 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 505 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 506 "../s/bif.y"
    { currentPartitionBifOptions->SetLockStepFlag();}
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 509 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 510 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 513 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 514 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 515 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 517 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 518 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 519 "../s/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 520 "../s/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 521 "../s/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 522 "../s/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 523 "../s/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 524 "../s/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 525 "../s/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 526 "../s/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 527 "../s/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 528 "../s/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 189:
/* Line 670 of lalr1.cc  */
#line 529 "../s/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 530 "../s/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 191:
/* Line 670 of lalr1.cc  */
#line 531 "../s/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 193:
/* Line 670 of lalr1.cc  */
#line 533 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 194:
/* Line 670 of lalr1.cc  */
#line 534 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 195:
/* Line 670 of lalr1.cc  */
#line 535 "../s/bif.y"
    { currentPartitionBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 196:
/* Line 670 of lalr1.cc  */
#line 536 "../s/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 197:
/* Line 670 of lalr1.cc  */
#line 537 "../s/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 198:
/* Line 670 of lalr1.cc  */
#line 538 "../s/bif.y"
    { currentPartitionBifOptions->SetClusterNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 539 "../s/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 200:
/* Line 670 of lalr1.cc  */
#line 540 "../s/bif.y"
    { currentPartitionBifOptions->SetDelayAuth(true); }
    break;

  case 201:
/* Line 670 of lalr1.cc  */
#line 541 "../s/bif.y"
    { currentPartitionBifOptions->SetTcmBootFlag(); }
    break;

  case 210:
/* Line 670 of lalr1.cc  */
#line 554 "../s/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 212:
/* Line 670 of lalr1.cc  */
#line 558 "../s/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 214:
/* Line 670 of lalr1.cc  */
#line 562 "../s/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 231:
/* Line 670 of lalr1.cc  */
#line 611 "../s/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 232:
/* Line 670 of lalr1.cc  */
#line 612 "../s/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 233:
/* Line 670 of lalr1.cc  */
#line 613 "../s/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), false); }
    break;

  case 234:
/* Line 670 of lalr1.cc  */
#line 614 "../s/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), true); }
    break;

  case 235:
/* Line 670 of lalr1.cc  */
#line 615 "../s/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 236:
/* Line 670 of lalr1.cc  */
#line 616 "../s/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 237:
/* Line 670 of lalr1.cc  */
#line 617 "../s/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 238:
/* Line 670 of lalr1.cc  */
#line 618 "../s/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 239:
/* Line 670 of lalr1.cc  */
#line 619 "../s/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 240:
/* Line 670 of lalr1.cc  */
#line 620 "../s/bif.y"
    { currentPartitionBifOptions->SetTcmARegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 241:
/* Line 670 of lalr1.cc  */
#line 621 "../s/bif.y"
    { currentPartitionBifOptions->SetTcmBRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 242:
/* Line 670 of lalr1.cc  */
#line 622 "../s/bif.y"
    { currentPartitionBifOptions->SetTcmCRegion((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 243:
/* Line 670 of lalr1.cc  */
#line 625 "../s/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 244:
/* Line 670 of lalr1.cc  */
#line 626 "../s/bif.y"
    { currentPartitionBifOptions->acFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 245:
/* Line 670 of lalr1.cc  */
#line 627 "../s/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 262:
/* Line 670 of lalr1.cc  */
#line 652 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 263:
/* Line 670 of lalr1.cc  */
#line 655 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 264:
/* Line 670 of lalr1.cc  */
#line 658 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 265:
/* Line 670 of lalr1.cc  */
#line 661 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 268:
/* Line 670 of lalr1.cc  */
#line 670 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 272:
/* Line 670 of lalr1.cc  */
#line 680 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 273:
/* Line 670 of lalr1.cc  */
#line 683 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 278:
/* Line 670 of lalr1.cc  */
#line 695 "../s/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 280:
/* Line 670 of lalr1.cc  */
#line 702 "../s/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 281:
/* Line 670 of lalr1.cc  */
#line 703 "../s/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 283:
/* Line 670 of lalr1.cc  */
#line 708 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 284:
/* Line 670 of lalr1.cc  */
#line 709 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 285:
/* Line 670 of lalr1.cc  */
#line 710 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 287:
/* Line 670 of lalr1.cc  */
#line 714 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 288:
/* Line 670 of lalr1.cc  */
#line 715 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 290:
/* Line 670 of lalr1.cc  */
#line 719 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 291:
/* Line 670 of lalr1.cc  */
#line 720 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 293:
/* Line 670 of lalr1.cc  */
#line 724 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 295:
/* Line 670 of lalr1.cc  */
#line 728 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 297:
/* Line 670 of lalr1.cc  */
#line 733 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1507 "../bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -388;
  const short int
  BisonParser::yypact_[] =
  {
      -388,    13,    42,  -388,    12,  -388,  -388,    32,    14,  -388,
    -388,  -388,    72,  -388,   811,  -388,  -388,   -11,    86,    97,
     113,   128,   145,   152,   160,   163,  -388,   164,   171,   180,
     186,   197,  -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,
    -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,
    -388,  -388,  -388,   135,  -388,  -388,   106,  -388,  -388,  -388,
     205,  -388,  -388,  -388,   600,   211,   215,   220,  -388,   223,
     225,   739,   114,    50,   108,   -79,   -79,   -79,   -79,  -388,
     -79,   106,    32,  -388,   122,   294,    29,  -388,  -388,  -388,
    -388,    32,  -388,  -388,  -388,  -388,  -388,   254,   255,   256,
     257,   261,   267,   268,   269,   271,   272,   273,   274,   281,
     282,   283,   284,  -388,  -388,   285,   286,   287,  -388,  -388,
    -388,  -388,  -388,  -388,   288,   291,   292,   296,   298,   300,
     303,   304,   305,   306,  -388,   307,  -388,   308,   309,   311,
     312,  -388,  -388,  -388,  -388,  -388,   313,   315,   323,   329,
     334,   335,  -388,  -388,  -388,   297,    18,   461,  -388,  -388,
    -388,  -388,  -388,  -388,  -388,  -388,  -388,   108,   -79,   122,
     289,   117,   -15,   336,   332,   339,  -388,   340,  -388,  -388,
     341,   344,  -388,  -388,  -388,  -388,   -79,   -79,   -79,  -388,
    -388,     5,    43,    44,   196,   199,   201,   201,   201,   201,
    -388,   201,   347,   350,   355,   206,  -388,  -388,  -388,  -388,
    -388,  -388,  -388,   359,   363,   369,  -388,  -388,  -388,  -388,
    -388,  -388,  -388,  -388,   357,   103,  -388,  -388,  -388,  -388,
    -388,  -388,  -388,  -388,   370,   372,   373,   376,    19,  -388,
     -50,   -36,   -39,   249,   900,   -79,   239,   240,   238,   241,
     -79,   -79,   -79,   -79,   -79,   -79,   -79,   242,   -79,   -79,
     -79,    32,    32,   301,    32,   -79,   -79,   -79,   -79,    32,
     -79,   -79,   -79,   122,    32,    32,    32,    32,    32,    32,
    -388,   600,  -388,   600,  -388,  -388,  -388,   385,   201,  -388,
     294,   386,   388,   394,  -388,   398,   407,   408,   417,   114,
      32,   739,   -79,  -388,    50,  -388,  -388,   -67,   -79,   -79,
     -79,   -79,   -79,   -79,   -79,   -79,   -79,   -79,   295,  -388,
      32,   349,   419,   421,   423,   424,   432,   433,   434,   435,
     436,   437,   439,   440,   441,  -388,  -388,   444,   198,   -79,
     -79,   -79,  -388,   294,  -388,   -79,   358,   -79,  -388,    29,
    -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,
    -388,  -388,  -388,  -388,  -388,   201,  -388,  -388,  -388,  -388,
    -388,  -388,  -388,   201,   201,   201,   201,   201,   201,   201,
    -388,  -388,   201,  -388,   442,  -141,  -388,   201,  -388,  -388,
    -388,  -388,   201,   201,   201,   201,  -388,   201,   201,   201,
    -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,   -79,
    -388,   -79,   -79,   242,   117,   310,   161,   -15,  -388,  -388,
    -388,   201,  -388,  -388,  -388,  -388,  -388,     5,     5,    43,
      43,    44,   196,   199,  -388,   446,   448,   449,   450,   454,
     455,   456,   457,  -388,  -388,   458,  -388,  -388,   333,  -388,
    -388,  -388,   478,   -50,   -36,   -39,   -79,   -79,    32,   122,
      32,    32,    32,    32,    32,    32,  -388,   206,  -388,   201,
     201,   201,  -388,   201,  -388,   201,  -388,   -79,    70,  -388,
     201,   201,  -388,   398,  -388,  -388,  -388,  -388,  -388,   417,
     114,   -79,   -79,   -79,   -79,   -79,   -79,   360,   -79,   880,
    -388,   -79,  -388,  -388,  -388,   201,  -388,   489,  -134,  -388,
    -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,   337,
     -41,  -388,   201,   201,   201,   201,   201,   201,  -388,   201,
    -388,   201,   -79,    92,  -388,  -388,  -388,   354,   -35,  -388,
    -388
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,     6,     4,     0,     0,   274,
     275,     5,     0,     8,     0,   127,     7,   124,     0,     0,
       0,     0,     0,     0,     0,     0,    79,     0,     0,     0,
       0,     0,   252,   251,   253,   204,   205,   206,   207,   208,
     209,   202,   246,   254,   255,   256,   257,   258,   248,   249,
     250,    12,    13,    11,    54,    15,    78,    10,     9,    14,
       0,   203,   247,   126,     0,     0,     0,     0,    83,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    56,
       0,    77,    40,    16,     0,     0,     0,    55,   124,    80,
      81,     0,   156,   165,   163,   164,   171,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   172,     0,     0,     0,
       0,     0,     0,   237,   238,     0,     0,     0,   157,   158,
     159,   160,   161,   162,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   200,     0,   201,     0,     0,     0,
       0,   260,   167,   261,   166,   259,   246,   255,   256,   257,
     258,   248,   217,   199,   196,     0,   130,   129,   147,   143,
     192,   144,   170,   145,   146,   168,   169,     0,     0,     0,
       0,     0,     0,     0,     0,   141,    52,     0,    97,   227,
       0,    94,    96,    88,   277,   276,     0,     0,     0,   279,
     282,   286,   289,   292,   294,   296,    53,    47,    48,    49,
      58,    46,     0,    37,     0,    21,   218,    51,   123,   119,
     120,   121,   115,     0,     0,     0,   219,   220,   222,   223,
     224,   225,   226,   221,     0,    99,   108,   109,   117,   110,
     111,   112,   114,   113,     0,     0,     0,     0,   102,    50,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     128,     0,   133,     0,   134,    87,    91,   152,   154,    82,
       0,     0,     0,     0,   265,    89,     0,     0,    90,     0,
       0,     0,     0,    86,     0,   280,   281,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    79,    93,
      40,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,    34,     0,    18,     0,
       0,     0,    85,     0,   101,     0,     0,     0,    92,     0,
     104,   210,   211,   177,   212,   213,   178,   214,   215,   179,
     216,   180,   139,   138,   170,   239,   229,   182,   228,   181,
     230,   183,   173,   231,   232,   233,   234,   235,   236,   190,
     267,   189,   195,   148,   150,   174,   149,   155,   243,   244,
     140,   245,   197,   198,   137,   136,   135,   240,   241,   242,
     194,   184,   185,   186,   187,   188,   191,   131,   132,     0,
      84,     0,     0,     0,     0,     0,     0,     0,   193,   125,
     142,    98,    95,   278,   283,   284,   285,   287,   288,   290,
     291,   293,   295,   297,    57,     0,     0,     0,     0,     0,
       0,     0,     0,    67,    68,     0,    69,    59,    62,    61,
      60,    38,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    17,    21,    20,   122,
     116,   118,   100,   105,   106,   107,   103,     0,     0,   153,
     262,   264,   263,   266,   271,   268,   272,   273,   269,   270,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      64,     0,    25,    22,    33,    32,    35,    41,    43,    31,
      23,    24,    26,    27,    28,    29,    30,    19,   151,     0,
       0,    71,    75,    76,    72,    73,    74,    65,    66,    70,
      63,    39,     0,     0,   176,   175,    42,     0,     0,    45,
      44
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -388,  -388,  -388,  -388,  -388,  -388,  -388,  -388,  -318,  -388,
     167,  -388,   -29,  -388,  -388,  -388,   451,  -388,  -388,  -387,
    -388,   195,   493,  -388,  -388,   218,  -388,  -210,  -231,  -388,
    -388,   -10,  -388,   509,  -388,  -131,  -388,   226,   -63,  -388,
      47,   119,  -388,  -388,   290,  -388,  -388,  -388,  -388,    73,
      75,    76,  -388,   -62,  -163,  -388,  -388,  -388,  -388,  -388,
    -388,  -388,  -388,   -65,  -388,  -388,  -388,  -388,  -388,   516,
    -388,  -388,   118,   120,   121,  -388,  -388,     7,  -388,   -96,
     -91,   -84,   222,   219,   224,   -75
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     6,     8,    14,    51,   205,   337,   338,
     202,   203,   506,   507,    52,    53,    54,   200,   318,   447,
     448,    55,    56,    57,   170,   180,   181,   224,   237,   238,
     225,    89,    71,    90,    64,   155,   156,   174,   157,   158,
     383,   286,   287,   386,   159,   160,   384,   161,    60,   353,
     356,   359,   361,   162,   207,   226,   227,   228,   229,   230,
     231,   232,   233,   182,   369,   367,   371,   163,   164,   165,
      62,   166,   295,   381,   298,   485,   488,    63,   189,   190,
     191,   192,   193,   194,   195,   288
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       196,   197,   198,   199,    58,   201,   289,   350,   175,   183,
     176,   317,    65,     3,    11,   344,   184,   185,   317,     7,
     468,    12,   478,   281,   349,   282,   284,   296,   297,   533,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,    66,   103,   104,   351,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   357,   115,   116,   354,   117,
     234,   500,   118,   119,   120,   121,   122,   123,   124,   125,
     234,   126,   127,   128,   129,    13,   177,   130,   131,   186,
     410,   352,   132,   187,   188,   317,    67,    68,    69,   204,
     305,   306,    72,   133,   358,    70,   355,   423,   239,   134,
      73,   135,   285,    74,   136,   137,   138,   139,   343,    15,
     400,   317,   530,   307,   178,   140,    88,   317,   476,    75,
     235,   236,     4,   535,     9,    10,    32,    33,    34,   540,
     235,   236,     5,   472,    76,   141,   142,   143,   144,   145,
     146,    43,   147,   148,   149,   150,   151,    49,    50,   517,
     407,    77,   408,   152,   291,   292,   293,   294,    78,   153,
     308,   309,   310,    79,   208,   184,   185,    82,   154,    80,
     365,   209,   210,   211,    83,   373,   374,   375,   376,   377,
     378,   379,   364,   382,   385,   387,    84,   184,   185,    85,
     392,   393,   394,   395,   179,   397,   398,   399,     9,    10,
      86,   311,   312,   467,   313,   314,   212,   213,   214,    24,
     215,    91,   424,   425,   426,   322,   323,   324,   486,   487,
     427,   428,   167,   322,   323,   324,   168,   421,   186,   429,
     430,   169,   187,   188,   171,   519,   172,   418,   175,   325,
     216,   217,   218,   219,   220,   221,   222,   325,   326,   152,
     186,   327,   179,   223,   187,   188,   326,   537,   206,   327,
     240,   241,   242,   243,   469,   470,   471,   244,   388,   389,
     473,   391,   475,   245,   246,   247,   396,   248,   249,   250,
     251,   401,   402,   403,   404,   405,   406,   252,   253,   254,
     255,   256,   257,   258,   259,   328,   510,   260,   261,   434,
     290,   280,   262,   328,   263,    88,   264,   419,   450,   265,
     266,   267,   268,   269,   270,   271,   435,   272,   273,   274,
     329,   275,   330,   331,   332,   333,   334,   204,   329,   276,
     330,   331,   332,   333,   334,   277,   480,   481,   499,   335,
     278,   279,   299,   300,   301,   303,   302,   335,   336,   304,
     315,   319,   316,   317,   435,   208,   336,   436,   437,   320,
     321,   342,   209,   210,   211,   339,   438,   439,   440,   340,
     441,   442,   443,   444,   445,   341,   345,    26,   346,   347,
     348,   505,   508,   360,   366,   370,   368,     9,    10,   372,
     409,   390,   411,   380,   412,   436,   437,   212,   213,   214,
     413,   215,   385,   520,   438,   439,   440,   414,   441,   442,
     443,   444,   445,   415,   416,   446,   522,   523,   524,   525,
     526,   527,   417,   529,   452,   453,   531,   454,   521,   455,
     456,   216,   217,   218,   219,   220,   221,   222,   457,   458,
     459,   460,   461,   462,   223,   463,   464,   465,   466,   474,
     528,   477,   490,   446,   491,   492,   493,   508,   538,   484,
     494,   495,   496,   497,   498,   509,   283,   511,   512,   513,
     514,   515,   516,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   501,   103,   104,   451,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   532,   115,
     116,   534,   117,   536,    87,   118,   119,   120,   121,   122,
     123,   124,   125,   449,   126,   127,   128,   129,   539,    81,
     130,   131,   422,    59,   518,   132,   502,   420,   479,   503,
      61,   504,   483,   482,   363,   432,   133,   431,   489,     0,
       0,   433,   134,     0,   135,     0,     0,   136,   137,   138,
     139,     0,     0,     0,     0,     0,     0,     0,   140,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    32,
      33,    34,     0,     0,     0,     0,     0,     0,   141,   142,
     143,   144,   145,   146,    43,   147,   148,   149,   150,   151,
      49,    50,     0,     0,     0,     0,   152,     0,     0,     0,
       0,     0,   153,     0,     0,     0,     0,     0,     0,     0,
       0,   154,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,     0,   103,   104,     0,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,     0,   115,   116,
       0,   117,     0,     0,   118,   119,   120,   121,   122,   123,
     124,   125,     0,   126,   127,   128,   129,     0,     0,   130,
     131,     0,     0,     0,   132,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   133,     0,     0,     0,     0,
       0,   134,     0,   135,     0,     0,   136,   137,   138,   139,
       0,     0,     0,     0,     0,     0,     0,   140,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    32,    33,
      34,     0,     0,     0,     0,     0,     0,   141,   142,   143,
     144,   145,   146,    43,   147,   148,   149,   150,   151,    49,
      50,     0,     0,     0,     0,   152,     0,     0,     0,     0,
       0,   153,     0,     0,     0,     0,     0,     0,     0,     0,
     154,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     173,   102,     0,   103,   104,     0,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,     0,   115,   116,     0,
     117,     0,     0,   118,   119,   120,   121,   122,   123,   124,
     125,     0,   126,   127,     0,   129,     0,     0,   130,   131,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    15,    16,     0,     0,     0,     0,
     134,    17,     0,     0,     0,   136,   137,   138,   139,     0,
       0,     0,    18,     0,    19,     0,   140,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    32,    33,    34,
       0,     0,    20,     0,     0,     0,   141,   142,   143,   144,
     145,   146,    43,   147,   148,   149,   150,   151,    49,    50,
       0,     0,     0,     0,   152,     0,    21,    22,    23,     0,
     153,     0,     0,     0,     0,    24,    25,     0,     0,   154,
       0,     0,     0,    26,     0,    27,    28,     0,     0,     0,
       0,   435,     0,     9,    10,     0,     0,     0,    29,    30,
       0,    31,    92,    93,    94,    95,    96,     0,     0,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     0,     0,
       0,    41,     0,    42,    43,    44,    45,    46,    47,    48,
      49,    50,   436,   437,   118,   119,   120,   121,   122,   123,
       0,   438,   439,   440,     0,   441,   442,   443,   444,   445,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     446,     0,     0,     0,     0,     0,     0,     0,    32,    33,
      34,     0,     0,     0,     0,     0,     0,   362,   142,   143,
     144,   145,    42,    43,    44,    45,    46,    47,    48,    49,
      50,     0,     0,     0,     0,   152
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        75,    76,    77,    78,    14,    80,   169,   238,    71,    74,
      72,   152,    23,     0,     7,   225,    95,    96,   152,     7,
     338,     7,   163,     5,     5,   156,   157,    42,    43,   163,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    52,    24,    25,    94,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    94,    38,    39,    94,    41,
      41,   448,    44,    45,    46,    47,    48,    49,    50,    51,
      41,    53,    54,    55,    56,     3,    26,    59,    60,   158,
     290,   131,    64,   162,   163,   152,    97,    98,    99,    82,
     186,   187,     6,    75,   133,   106,   132,   164,    91,    81,
       3,    83,   167,     6,    86,    87,    88,    89,     5,     3,
     273,   152,   499,   188,    64,    97,    10,   152,   349,     6,
     101,   102,    80,   164,    92,    93,   108,   109,   110,   164,
     101,   102,    90,   343,     6,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   467,
     281,     6,   283,   135,    37,    38,    39,    40,     6,   141,
     155,   156,   157,     3,    61,    95,    96,     3,   150,     6,
     245,    68,    69,    70,     3,   250,   251,   252,   253,   254,
     255,   256,   244,   258,   259,   260,     6,    95,    96,     3,
     265,   266,   267,   268,   144,   270,   271,   272,    92,    93,
       3,   158,   159,     5,   160,   161,   103,   104,   105,    74,
     107,     6,   308,   309,   310,    17,    18,    19,    57,    58,
     311,   312,    11,    17,    18,    19,    11,   302,   158,   313,
     314,    11,   162,   163,    11,   165,    11,   299,   301,    41,
     137,   138,   139,   140,   141,   142,   143,    41,    50,   135,
     158,    53,   144,   150,   162,   163,    50,   165,   136,    53,
       6,     6,     6,     6,   339,   340,   341,     6,   261,   262,
     345,   264,   347,     6,     6,     6,   269,     6,     6,     6,
       6,   274,   275,   276,   277,   278,   279,     6,     6,     6,
       6,     6,     6,     6,     6,    97,   459,     6,     6,     4,
      11,     4,     6,    97,     6,    10,     6,   300,   318,     6,
       6,     6,     6,     6,     6,     6,    21,     6,     6,     6,
     122,     6,   124,   125,   126,   127,   128,   320,   122,     6,
     124,   125,   126,   127,   128,     6,   411,   412,     5,   141,
       6,     6,     6,    11,     5,     4,     6,   141,   150,     5,
     154,     4,   153,   152,    21,    61,   150,    62,    63,     9,
       5,     4,    68,    69,    70,     6,    71,    72,    73,     6,
      75,    76,    77,    78,    79,     6,     6,    82,     6,     6,
       4,   456,   457,   134,   145,   147,   146,    92,    93,   148,
       5,    90,     6,   151,     6,    62,    63,   103,   104,   105,
       6,   107,   477,   478,    71,    72,    73,     9,    75,    76,
      77,    78,    79,     6,     6,   120,   491,   492,   493,   494,
     495,   496,     5,   498,    75,     6,   501,     6,   490,     6,
       6,   137,   138,   139,   140,   141,   142,   143,     6,     6,
       6,     6,     6,     6,   150,     6,     6,     6,     4,    91,
      90,     9,     6,   120,     6,     6,     6,   532,   533,   149,
       6,     6,     6,     6,     6,   458,     5,   460,   461,   462,
     463,   464,   465,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,     6,    24,    25,   320,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,     9,    38,
      39,   164,    41,   532,    53,    44,    45,    46,    47,    48,
      49,    50,    51,   318,    53,    54,    55,    56,   164,    26,
      59,    60,   304,    14,   477,    64,   453,   301,   409,   454,
      14,   455,   414,   413,   244,   316,    75,   315,   417,    -1,
      -1,   317,    81,    -1,    83,    -1,    -1,    86,    87,    88,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,   110,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,    -1,    -1,    -1,    -1,   135,    -1,    -1,    -1,
      -1,    -1,   141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   150,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    -1,    24,    25,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    -1,    38,    39,
      -1,    41,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    -1,    53,    54,    55,    56,    -1,    -1,    59,
      60,    -1,    -1,    -1,    64,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    75,    -1,    -1,    -1,    -1,
      -1,    81,    -1,    83,    -1,    -1,    86,    87,    88,    89,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,    -1,    -1,    -1,    -1,   135,    -1,    -1,    -1,    -1,
      -1,   141,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     150,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    -1,    24,    25,    -1,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    -1,    38,    39,    -1,
      41,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      51,    -1,    53,    54,    -1,    56,    -1,    -1,    59,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,    -1,    -1,    -1,    -1,
      81,    10,    -1,    -1,    -1,    86,    87,    88,    89,    -1,
      -1,    -1,    21,    -1,    23,    -1,    97,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,   110,
      -1,    -1,    41,    -1,    -1,    -1,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,    65,    66,    67,    -1,
     141,    -1,    -1,    -1,    -1,    74,    75,    -1,    -1,   150,
      -1,    -1,    -1,    82,    -1,    84,    85,    -1,    -1,    -1,
      -1,    21,    -1,    92,    93,    -1,    -1,    -1,    97,    98,
      -1,   100,    12,    13,    14,    15,    16,    -1,    -1,   108,
     109,   110,   111,   112,   113,   114,   115,   116,    -1,    -1,
      -1,   120,    -1,   122,   123,   124,   125,   126,   127,   128,
     129,   130,    62,    63,    44,    45,    46,    47,    48,    49,
      -1,    71,    72,    73,    -1,    75,    76,    77,    78,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,    -1,    -1,    -1,    -1,    -1,    -1,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,    -1,    -1,    -1,    -1,   135
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   167,   168,     0,    80,    90,   169,     7,   170,    92,
      93,   243,     7,     3,   171,     3,     4,    10,    21,    23,
      41,    65,    66,    67,    74,    75,    82,    84,    85,    97,
      98,   100,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   120,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   172,   180,   181,   182,   187,   188,   189,   197,   199,
     214,   235,   236,   243,   200,    23,    52,    97,    98,    99,
     106,   198,     6,     3,     6,     6,     6,     6,     6,     3,
       6,   188,     3,     3,     6,     3,     3,   182,    10,   197,
     199,     6,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    24,    25,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    38,    39,    41,    44,    45,
      46,    47,    48,    49,    50,    51,    53,    54,    55,    56,
      59,    60,    64,    75,    81,    83,    86,    87,    88,    89,
      97,   117,   118,   119,   120,   121,   122,   124,   125,   126,
     127,   128,   135,   141,   150,   201,   202,   204,   205,   210,
     211,   213,   219,   233,   234,   235,   237,    11,    11,    11,
     190,    11,    11,    21,   203,   204,   219,    26,    64,   144,
     191,   192,   229,   229,    95,    96,   158,   162,   163,   244,
     245,   246,   247,   248,   249,   250,   251,   251,   251,   251,
     183,   251,   176,   177,   243,   173,   136,   220,    61,    68,
      69,    70,   103,   104,   105,   107,   137,   138,   139,   140,
     141,   142,   143,   150,   193,   196,   221,   222,   223,   224,
     225,   226,   227,   228,    41,   101,   102,   194,   195,   243,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       4,     5,   201,     5,   201,   229,   207,   208,   251,   220,
      11,    37,    38,    39,    40,   238,    42,    43,   240,     6,
      11,     5,     6,     4,     5,   245,   245,   251,   155,   156,
     157,   158,   159,   160,   161,   154,   153,   152,   184,     4,
       9,     5,    17,    18,    19,    41,    50,    53,    97,   122,
     124,   125,   126,   127,   128,   141,   150,   174,   175,     6,
       6,     6,     4,     5,   193,     6,     6,     6,     4,     5,
     194,    94,   131,   215,    94,   132,   216,    94,   133,   217,
     134,   218,   117,   210,   219,   251,   145,   231,   146,   230,
     147,   232,   148,   251,   251,   251,   251,   251,   251,   251,
     151,   239,   251,   206,   212,   251,   209,   251,   243,   243,
      90,   243,   251,   251,   251,   251,   243,   251,   251,   251,
     220,   243,   243,   243,   243,   243,   243,   201,   201,     5,
     193,     6,     6,     6,     9,     6,     6,     5,   219,   243,
     203,   251,   191,   164,   245,   245,   245,   246,   246,   247,
     247,   248,   249,   250,     4,    21,    62,    63,    71,    72,
      73,    75,    76,    77,    78,    79,   120,   185,   186,   187,
     197,   176,    75,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     4,     5,   174,   251,
     251,   251,   193,   251,    91,   251,   194,     9,   163,   207,
     251,   251,   239,   238,   149,   241,    57,    58,   242,   240,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     5,
     185,     6,   215,   216,   217,   251,   178,   179,   251,   243,
     220,   243,   243,   243,   243,   243,   243,   174,   206,   165,
     251,   219,   251,   251,   251,   251,   251,   251,    90,   251,
     185,   251,     9,   163,   164,   164,   178,   165,   251,   164,
     164
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
     415,   416,   417,   418,   419,   420
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   166,   167,   168,   168,   169,   170,   169,   171,   171,
     171,   171,   171,   171,   171,   171,   173,   172,   174,   174,
     174,   175,   175,   175,   175,   175,   175,   175,   175,   175,
     175,   175,   175,   175,   175,   175,   175,   176,   176,   177,
     177,   178,   178,   179,   179,   179,   180,   180,   180,   180,
     180,   180,   180,   180,   181,   181,   183,   182,   184,   184,
     184,   184,   185,   185,   185,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   187,   187,   188,
     188,   188,   189,   190,   189,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   191,   191,   192,   192,   192,   193,
     193,   193,   194,   194,   194,   195,   195,   195,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   198,   197,   197,   200,   199,   201,
     201,   201,   201,   201,   201,   202,   202,   202,   202,   202,
     202,   203,   203,   204,   204,   204,   204,   204,   205,   205,
     206,   206,   207,   207,   208,   209,   210,   210,   210,   210,
     210,   210,   210,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   211,   211,   212,   212,   212,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   213,   213,   213,   213,   213,   213,   213,   213,
     213,   213,   214,   214,   214,   214,   214,   214,   214,   214,
     215,   215,   216,   216,   217,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   233,   233,   233,   233,   233,   233,   233,   233,
     233,   233,   233,   234,   234,   234,   235,   235,   235,   235,
     235,   235,   235,   235,   235,   236,   236,   236,   236,   237,
     237,   237,   238,   238,   238,   238,   238,   239,   240,   240,
     240,   241,   242,   242,   243,   243,   244,   244,   244,   245,
     245,   245,   246,   246,   246,   246,   247,   247,   247,   248,
     248,   248,   249,   249,   250,   250,   251,   251
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
       3,     2,     1,     3,     2,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     3,     1,
       1,     1,     3,     1,     0,     5,     1,     0,     4,     1,
       1,     3,     3,     2,     2,     3,     3,     3,     3,     3,
       3,     1,     3,     1,     1,     1,     1,     1,     3,     3,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     4,     4,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     3,     3,     3,     1,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     1,     3,     1,     3,     3,
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
  "SPK_ID", "SPK_SELECT", "HEADER_AUTH", "REVOKE_ID", "SPLIT_MODE",
  "SPLIT_FMT", "BOOT", "USER", "STATIC", "NOAUTOSTART", "MULTIBOOT",
  "PROTECTED", "BLOCKS", "AUTHBLOCKS", "BOOTVECTORS", "PRESIGN", "AC",
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
       167,     0,    -1,   168,    -1,    -1,   168,   169,    -1,    80,
       7,   243,    -1,    -1,    90,   170,     7,     3,   171,     4,
      -1,    -1,   171,   197,    -1,   171,   189,    -1,   171,   181,
      -1,   171,   172,    -1,   171,   180,    -1,   171,   199,    -1,
     171,   187,    -1,    -1,    85,     3,   173,   174,     4,    -1,
     175,    -1,   175,     5,   174,    -1,   175,   174,    -1,    -1,
      18,     6,   216,    -1,    97,     6,   220,    -1,   122,     6,
     243,    -1,    17,     6,   215,    -1,   124,     6,   243,    -1,
     125,     6,   243,    -1,   126,     6,   243,    -1,   127,     6,
     243,    -1,   128,     6,   243,    -1,    53,     6,   243,    -1,
      41,     6,   251,    -1,    19,     6,   217,    -1,   150,    -1,
      50,     6,   178,    -1,   141,    -1,   177,    -1,   177,     9,
     176,    -1,   243,     5,    75,     6,   251,    -1,    -1,   179,
      -1,   179,     9,   178,    -1,   251,    -1,   251,   163,   251,
     164,    -1,   251,   163,   165,   164,    -1,    75,     6,   251,
      -1,    65,     6,   251,    -1,    66,     6,   251,    -1,    67,
       6,   251,    -1,   214,     6,   243,    -1,    97,     6,   220,
      -1,    21,     6,   219,    -1,    41,     6,   251,    -1,   182,
      -1,   181,   182,    -1,    -1,    74,     3,   183,   184,     4,
      -1,    -1,   184,   185,    -1,   184,   197,    -1,   184,   187,
      -1,   186,    -1,   186,     5,   185,    -1,   186,   185,    -1,
      75,     6,   251,    -1,    76,     6,    90,    -1,    77,    -1,
      78,    -1,   120,    -1,    79,     6,   251,    -1,    21,     6,
     219,    -1,    71,     6,   251,    -1,    72,     6,   251,    -1,
      73,     6,   251,    -1,    62,     6,   251,    -1,    63,     6,
     251,    -1,    82,   188,    -1,   188,    -1,    -1,   188,   197,
      -1,   188,   199,    -1,    10,    97,    11,   220,    -1,    -1,
      10,    98,   190,    11,   193,    -1,    98,     3,   193,     4,
      -1,    23,     3,   191,     4,    -1,    10,    23,    11,   229,
      -1,    23,     6,   229,    -1,    10,    99,    11,   238,    -1,
      10,   106,    11,   240,    -1,    10,    52,    11,   207,    -1,
     100,     3,   194,     4,    -1,    84,     3,   176,     4,    -1,
     192,    -1,   192,     5,   191,    -1,   229,    -1,    64,    -1,
      26,     6,   251,    -1,   196,    -1,   196,     5,   193,    -1,
     196,   193,    -1,   195,    -1,   195,     5,   194,    -1,   195,
     194,    -1,    41,     6,   251,    -1,   101,     6,    91,    -1,
     102,     6,   251,    -1,   221,    -1,   222,    -1,   224,    -1,
     225,    -1,   226,    -1,   228,    -1,   227,    -1,   103,    -1,
     105,     6,   251,    -1,   223,    -1,   107,     6,   251,    -1,
      68,    -1,    69,    -1,    70,    -1,   104,     6,   251,    -1,
      61,    -1,    -1,    10,   198,   203,    11,   243,    -1,   243,
      -1,    -1,     3,   200,   201,     4,    -1,   204,    -1,   202,
      -1,   202,     5,   201,    -1,   204,     5,   201,    -1,   202,
     201,    -1,   204,   201,    -1,    83,     6,   243,    -1,    75,
       6,   251,    -1,    64,     6,   251,    -1,    21,     6,   210,
      -1,    21,     6,   117,    -1,    55,     6,    90,    -1,   204,
      -1,   204,     5,   203,    -1,   210,    -1,   213,    -1,   233,
      -1,   234,    -1,   205,    -1,    50,     6,   206,    -1,    51,
       6,   209,    -1,   212,    -1,   212,     9,   206,    -1,   208,
      -1,   208,     5,   207,    -1,   251,    -1,   251,    -1,    12,
      -1,    44,    -1,    45,    -1,    46,    -1,    47,    -1,    48,
      -1,    49,    -1,    14,    -1,    15,    -1,    13,    -1,   120,
      -1,   118,    -1,   235,    -1,   237,    -1,   219,    -1,    16,
      -1,    28,    -1,    28,     6,   148,    -1,   251,    -1,   251,
     163,   251,   164,    -1,   251,   163,   165,   164,    -1,    17,
       6,   215,    -1,    18,     6,   216,    -1,    19,     6,   217,
      -1,    20,     6,   218,    -1,    25,     6,   230,    -1,    24,
       6,   231,    -1,    27,     6,   232,    -1,   122,     6,   243,
      -1,   124,     6,   243,    -1,   125,     6,   243,    -1,   126,
       6,   243,    -1,   127,     6,   243,    -1,    39,     6,   239,
      -1,    38,     6,   251,    -1,   128,     6,   243,    -1,   211,
      -1,    21,     6,   219,    -1,    97,     6,   220,    -1,    41,
       6,   251,    -1,   150,    -1,    59,     6,   251,    -1,    60,
       6,   251,    -1,   141,    -1,    81,    -1,    86,    -1,   120,
      -1,   235,    -1,   111,    -1,   112,    -1,   113,    -1,   114,
      -1,   115,    -1,   116,    -1,    94,    -1,   131,    -1,    94,
      -1,   132,    -1,    94,    -1,   133,    -1,   134,    -1,   135,
      -1,   136,    -1,   137,    -1,   138,    -1,   150,    -1,   139,
      -1,   140,    -1,   141,    -1,   142,    -1,   143,    -1,   144,
      -1,   146,    -1,   145,    -1,   147,    -1,    29,     6,   251,
      -1,    30,     6,   251,    -1,    31,     6,   251,    -1,    32,
       6,   251,    -1,    33,     6,   251,    -1,    34,     6,   251,
      -1,    35,    -1,    36,    -1,    22,     6,   251,    -1,    87,
       6,   251,    -1,    88,     6,   251,    -1,    89,     6,   251,
      -1,    53,     6,   243,    -1,    54,     6,   243,    -1,    56,
       6,   243,    -1,   122,    -1,   236,    -1,   128,    -1,   129,
      -1,   130,    -1,   109,    -1,   108,    -1,   110,    -1,   123,
      -1,   124,    -1,   125,    -1,   126,    -1,   127,    -1,   121,
      -1,   117,    -1,   119,    -1,    37,     6,   251,    -1,    39,
       6,   239,    -1,    38,     6,   251,    -1,    40,    -1,   238,
       9,   238,    -1,   151,    -1,    42,     6,   241,    -1,    43,
       6,   242,    -1,   240,     5,   240,    -1,   149,    -1,    57,
      -1,    58,    -1,    92,    -1,    93,    -1,    96,    -1,    95,
      -1,   163,   251,   164,    -1,   244,    -1,   158,   245,    -1,
     162,   245,    -1,   245,    -1,   246,   155,   245,    -1,   246,
     156,   245,    -1,   246,   157,   245,    -1,   246,    -1,   247,
     158,   246,    -1,   247,   159,   246,    -1,   247,    -1,   248,
     160,   247,    -1,   248,   161,   247,    -1,   248,    -1,   249,
     154,   248,    -1,   249,    -1,   250,   153,   249,    -1,   250,
      -1,   251,   152,   250,    -1
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
     336,   340,   343,   345,   349,   352,   356,   360,   364,   366,
     368,   370,   372,   374,   376,   378,   380,   384,   386,   390,
     392,   394,   396,   400,   402,   403,   409,   411,   412,   417,
     419,   421,   425,   429,   432,   435,   439,   443,   447,   451,
     455,   459,   461,   465,   467,   469,   471,   473,   475,   479,
     483,   485,   489,   491,   495,   497,   499,   501,   503,   505,
     507,   509,   511,   513,   515,   517,   519,   521,   523,   525,
     527,   529,   531,   533,   537,   539,   544,   549,   553,   557,
     561,   565,   569,   573,   577,   581,   585,   589,   593,   597,
     601,   605,   609,   611,   615,   619,   623,   625,   629,   633,
     635,   637,   639,   641,   643,   645,   647,   649,   651,   653,
     655,   657,   659,   661,   663,   665,   667,   669,   671,   673,
     675,   677,   679,   681,   683,   685,   687,   689,   691,   693,
     695,   697,   701,   705,   709,   713,   717,   721,   723,   725,
     729,   733,   737,   741,   745,   749,   753,   755,   757,   759,
     761,   763,   765,   767,   769,   771,   773,   775,   777,   779,
     781,   783,   785,   789,   793,   797,   799,   803,   805,   809,
     813,   817,   819,   821,   823,   825,   827,   829,   831,   835,
     837,   840,   843,   845,   849,   853,   857,   859,   863,   867,
     869,   873,   877,   879,   883,   885,   889,   891
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
     357,   358,   361,   362,   363,   366,   367,   368,   371,   374,
     380,   382,   384,   386,   388,   390,   392,   394,   400,   404,
     405,   406,   407,   412,   419,   419,   426,   432,   432,   438,
     439,   440,   441,   442,   443,   446,   449,   450,   452,   453,
     454,   459,   460,   463,   464,   465,   466,   467,   470,   471,
     474,   475,   478,   479,   482,   487,   489,   490,   491,   492,
     493,   494,   495,   496,   497,   498,   501,   502,   503,   504,
     505,   506,   509,   510,   513,   514,   515,   517,   518,   519,
     520,   521,   522,   523,   524,   525,   526,   527,   528,   529,
     530,   531,   532,   533,   534,   535,   536,   537,   538,   539,
     540,   541,   544,   545,   546,   547,   548,   549,   550,   551,
     554,   555,   558,   559,   562,   563,   566,   569,   572,   575,
     578,   581,   584,   587,   590,   593,   596,   599,   602,   605,
     608,   611,   612,   613,   614,   615,   616,   617,   618,   619,
     620,   621,   622,   625,   626,   627,   630,   631,   632,   633,
     634,   635,   636,   637,   638,   641,   642,   643,   644,   647,
     648,   649,   652,   655,   658,   661,   664,   667,   670,   673,
     674,   677,   680,   683,   689,   690,   693,   694,   695,   701,
     702,   703,   707,   708,   709,   710,   713,   714,   715,   718,
     719,   720,   723,   724,   727,   728,   732,   733
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
     165
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 1035;
  const int BisonParser::yynnts_ = 86;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 166;

  const unsigned int BisonParser::yyuser_token_number_max_ = 420;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 24 "../s/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 2589 "../bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 738 "../s/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
