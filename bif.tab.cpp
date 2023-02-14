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
#line 203 "../s/bif.y"
    { options.includeBifOptionsList.push_back((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 204 "../s/bif.y"
    { currentBifOptions = new BifOptions(options.GetArchType(),(yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 7:
/* Line 670 of lalr1.cc  */
#line 206 "../s/bif.y"
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 220 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries()); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 231 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 232 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t), options.IsVersalNetSeries()); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 233 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 234 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 235 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 236 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 237 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 238 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 239 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 240 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 241 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.revokeId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 242 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 34:
/* Line 670 of lalr1.cc  */
#line 243 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 36:
/* Line 670 of lalr1.cc  */
#line 245 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 39:
/* Line 670 of lalr1.cc  */
#line 253 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 255 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 257 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 261 "../s/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 262 "../s/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 263 "../s/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 264 "../s/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 265 "../s/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 266 "../s/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 267 "../s/bif.y"
    { currentBifOptions->SetPdiType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 274 "../s/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 275 "../s/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 289 "../s/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 290 "../s/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 291 "../s/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 292 "../s/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 293 "../s/bif.y"
    { currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 294 "../s/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 295 "../s/bif.y"
    { currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 67:
/* Line 670 of lalr1.cc  */
#line 296 "../s/bif.y"
    { currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 68:
/* Line 670 of lalr1.cc  */
#line 297 "../s/bif.y"
    { currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 298 "../s/bif.y"
    { if(!options.IsVersalNetSeries())
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr' is supported only for VERSAL NET architecture");
                                                                                  currentImageBifOptions->SetPcrNumber((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 301 "../s/bif.y"
    { if(!options.IsVersalNetSeries())
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t  'pcr_mid' is supported only for VERSAL NET architecture");
                                                                                  currentImageBifOptions->SetPcrMeasurementIndex((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 76:
/* Line 670 of lalr1.cc  */
#line 315 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported in VERSAL architecture.\n\t   Please see 'bootgen -arch versal -bif_help keysrc'");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 77:
/* Line 670 of lalr1.cc  */
#line 318 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 81:
/* Line 670 of lalr1.cc  */
#line 323 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                    if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); 
                                                                                    currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 82:
/* Line 670 of lalr1.cc  */
#line 328 "../s/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); }
    break;

  case 89:
/* Line 670 of lalr1.cc  */
#line 339 "../s/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 90:
/* Line 670 of lalr1.cc  */
#line 340 "../s/bif.y"
    { currentBifOptions->SetBootDevice(BootDevice::IMAGESTORE); }
    break;

  case 91:
/* Line 670 of lalr1.cc  */
#line 341 "../s/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 97:
/* Line 670 of lalr1.cc  */
#line 353 "../s/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 98:
/* Line 670 of lalr1.cc  */
#line 354 "../s/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 99:
/* Line 670 of lalr1.cc  */
#line 355 "../s/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 358 "../s/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 361 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                     LOG_ERROR("BIF attribute error !!! 'bh_auth_enable' is not supported with '-arch versalnet'.\n\t   Bootheader or eFuse authentication will be chosen based on eFuse bits.");
                                                                                  else
                                                                                     currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); 
                                                                                }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 367 "../s/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 103:
/* Line 670 of lalr1.cc  */
#line 369 "../s/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 371 "../s/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 373 "../s/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 106:
/* Line 670 of lalr1.cc  */
#line 375 "../s/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 107:
/* Line 670 of lalr1.cc  */
#line 377 "../s/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 108:
/* Line 670 of lalr1.cc  */
#line 379 "../s/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 381 "../s/bif.y"
    {  if(options.GetArchType() != Arch::VERSAL)
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t\t'dpacm_enable' is supported only in VERSAL architecture");
                                                                                   if(options.GetArchType() == Arch::VERSAL)
                                                                                     LOG_WARNING("boot_config { dpacm_enable } will be deprecated. 'dpacm_enable' should be specified along with the partition. \n            See 'bootgen -bif_help dpacm_enable' for more info.");
                                                                                   currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));
                                                                                }
    break;

  case 110:
/* Line 670 of lalr1.cc  */
#line 387 "../s/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->smapWidth = (yysemantic_stack_[(3) - (3)].number);
                                                                                }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 391 "../s/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 392 "../s/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 393 "../s/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 394 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                     currentBifOptions->SetPufRingOscilltorSwapConfigValue((yysemantic_stack_[(3) - (3)].number));
                                                                                  else
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t  'puf_ro_swap' is supported only in VersalNet architecture");
                                                                                 }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 399 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                     currentBifOptions->SetDiceEnable();
                                                                                  else
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t  'dice_enable' is supported only in VersalNet architecture");
                                                                                 }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 406 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(),options.IsVersalNetSeries()); }
    break;

  case 117:
/* Line 670 of lalr1.cc  */
#line 409 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 118:
/* Line 670 of lalr1.cc  */
#line 413 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 119:
/* Line 670 of lalr1.cc  */
#line 419 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries()); }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 433 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 436 "../s/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 129:
/* Line 670 of lalr1.cc  */
#line 437 "../s/bif.y"
    { currentPartitionBifOptions->imageStoreId = (yysemantic_stack_[(3) - (3)].number);
                                                                                  currentPartitionBifOptions->SetPartitionType(PartitionType::IMAGE_STORE_PDI); }
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 440 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 441 "../s/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string), currentImageBifOptions->GetImageType());
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 469 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] only supported in ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 474 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 476 "../s/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 477 "../s/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 150:
/* Line 670 of lalr1.cc  */
#line 478 "../s/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 151:
/* Line 670 of lalr1.cc  */
#line 479 "../s/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 152:
/* Line 670 of lalr1.cc  */
#line 480 "../s/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 153:
/* Line 670 of lalr1.cc  */
#line 481 "../s/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 482 "../s/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 483 "../s/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 484 "../s/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 157:
/* Line 670 of lalr1.cc  */
#line 485 "../s/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 488 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 489 "../s/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 490 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 161:
/* Line 670 of lalr1.cc  */
#line 491 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 492 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 493 "../s/bif.y"
    { currentPartitionBifOptions->SetLockStepFlag();}
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 496 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 497 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 500 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 501 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 502 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 504 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 505 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 506 "../s/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 507 "../s/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 508 "../s/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 509 "../s/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 510 "../s/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 511 "../s/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 512 "../s/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 513 "../s/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 514 "../s/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 515 "../s/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 516 "../s/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 517 "../s/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 518 "../s/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 520 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 521 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 522 "../s/bif.y"
    { currentPartitionBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 523 "../s/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 189:
/* Line 670 of lalr1.cc  */
#line 524 "../s/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 525 "../s/bif.y"
    { currentPartitionBifOptions->SetClusterNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 191:
/* Line 670 of lalr1.cc  */
#line 526 "../s/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 192:
/* Line 670 of lalr1.cc  */
#line 527 "../s/bif.y"
    { currentPartitionBifOptions->SetDelayAuth(true); }
    break;

  case 201:
/* Line 670 of lalr1.cc  */
#line 540 "../s/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 203:
/* Line 670 of lalr1.cc  */
#line 544 "../s/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 205:
/* Line 670 of lalr1.cc  */
#line 548 "../s/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 222:
/* Line 670 of lalr1.cc  */
#line 597 "../s/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 223:
/* Line 670 of lalr1.cc  */
#line 598 "../s/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 224:
/* Line 670 of lalr1.cc  */
#line 599 "../s/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), false); }
    break;

  case 225:
/* Line 670 of lalr1.cc  */
#line 600 "../s/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), true); }
    break;

  case 226:
/* Line 670 of lalr1.cc  */
#line 601 "../s/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 227:
/* Line 670 of lalr1.cc  */
#line 602 "../s/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 228:
/* Line 670 of lalr1.cc  */
#line 603 "../s/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 229:
/* Line 670 of lalr1.cc  */
#line 604 "../s/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 230:
/* Line 670 of lalr1.cc  */
#line 605 "../s/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 231:
/* Line 670 of lalr1.cc  */
#line 608 "../s/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 232:
/* Line 670 of lalr1.cc  */
#line 609 "../s/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 249:
/* Line 670 of lalr1.cc  */
#line 634 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 250:
/* Line 670 of lalr1.cc  */
#line 637 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 251:
/* Line 670 of lalr1.cc  */
#line 640 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 252:
/* Line 670 of lalr1.cc  */
#line 643 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 255:
/* Line 670 of lalr1.cc  */
#line 652 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 259:
/* Line 670 of lalr1.cc  */
#line 662 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 260:
/* Line 670 of lalr1.cc  */
#line 665 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 265:
/* Line 670 of lalr1.cc  */
#line 677 "../s/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 267:
/* Line 670 of lalr1.cc  */
#line 684 "../s/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 268:
/* Line 670 of lalr1.cc  */
#line 685 "../s/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 270:
/* Line 670 of lalr1.cc  */
#line 690 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 271:
/* Line 670 of lalr1.cc  */
#line 691 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 272:
/* Line 670 of lalr1.cc  */
#line 692 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 274:
/* Line 670 of lalr1.cc  */
#line 696 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 275:
/* Line 670 of lalr1.cc  */
#line 697 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 277:
/* Line 670 of lalr1.cc  */
#line 701 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 278:
/* Line 670 of lalr1.cc  */
#line 702 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 280:
/* Line 670 of lalr1.cc  */
#line 706 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 282:
/* Line 670 of lalr1.cc  */
#line 710 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 284:
/* Line 670 of lalr1.cc  */
#line 715 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1458 "../bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -391;
  const short int
  BisonParser::yypact_[] =
  {
      -391,    14,   -17,  -391,    61,  -391,  -391,     5,    70,  -391,
    -391,  -391,   104,  -391,   760,  -391,  -391,    92,   110,   146,
     125,   145,   154,   160,   171,  -391,   179,   192,   202,   217,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
    -391,    25,  -391,  -391,    60,  -391,  -391,  -391,   221,  -391,
    -391,  -391,   561,   225,   229,   230,  -391,   233,   238,   694,
     108,    20,   118,   -14,   -14,   -14,  -391,   -14,    60,  -391,
     128,   226,   -24,  -391,  -391,  -391,  -391,     5,  -391,  -391,
    -391,  -391,  -391,   259,   260,   261,   262,   263,   264,   265,
     266,   268,   269,   271,   272,   274,   275,   276,   277,  -391,
    -391,   278,   281,   282,  -391,  -391,  -391,  -391,  -391,  -391,
     283,   285,   286,   290,   291,   293,   299,   300,   301,  -391,
     309,   310,  -391,  -391,  -391,  -391,  -391,   312,   315,   316,
     325,   331,   332,  -391,  -391,  -391,   335,     6,   428,  -391,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,   118,   -14,
     128,   336,   117,    97,   340,   337,   344,  -391,   345,  -391,
    -391,   346,   347,  -391,  -391,  -391,  -391,   -14,   -14,   -14,
    -391,  -391,    46,    38,    64,   205,   207,   218,   218,   218,
    -391,   218,   182,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
     359,   360,   361,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
    -391,   364,   366,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
    -391,   363,   367,   368,   371,     8,  -391,   -40,   -30,   -45,
     244,   296,   -14,   239,   240,   241,   237,   -14,   -14,   -14,
     -14,   -14,   -14,   -14,   236,   -14,   -14,   -14,     5,   302,
       5,   -14,   -14,   -14,   -14,     5,   128,     5,     5,     5,
       5,     5,     5,  -391,   561,  -391,   561,  -391,  -391,  -391,
     378,   218,  -391,   226,   379,   381,   382,  -391,   375,   383,
     390,   396,   108,     5,   694,   -14,  -391,    20,  -391,  -391,
     -80,   -14,   -14,   -14,   -14,   -14,   -14,   -14,   -14,   -14,
     -14,   827,   397,   398,   399,   400,   415,   416,   417,   418,
     420,   421,   422,   425,   445,  -391,  -391,   450,    95,   -14,
     -14,   -14,  -391,   226,   -14,   380,   -14,  -391,   -24,  -391,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
    -391,  -391,  -391,  -391,   218,  -391,  -391,  -391,  -391,  -391,
    -391,  -391,   218,   218,   218,   218,   218,   218,   218,  -391,
    -391,   218,  -391,   459,   -62,  -391,   218,  -391,  -391,  -391,
     218,   218,   218,   218,  -391,  -391,  -391,  -391,  -391,  -391,
    -391,  -391,  -391,  -391,   -14,  -391,   -14,   -14,   236,   117,
     327,   169,    97,  -391,  -391,  -391,   218,  -391,  -391,  -391,
    -391,  -391,    46,    46,    38,    38,    64,   205,   207,  -391,
     465,   474,   478,   479,   482,   483,   484,   486,  -391,  -391,
     487,  -391,   132,  -391,  -391,   -40,   -30,   -45,   -14,   -14,
       5,   128,     5,     5,     5,     5,     5,     5,  -391,   182,
    -391,   218,   218,   218,  -391,   218,  -391,   218,  -391,   -14,
      72,  -391,   218,   218,  -391,   375,  -391,  -391,  -391,  -391,
    -391,   396,   108,   -14,   -14,   -14,   -14,   -14,   -14,   410,
     -14,   258,  -391,  -391,  -391,  -391,   218,  -391,   488,   -52,
    -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
     338,   -59,  -391,   218,   218,   218,   218,   218,   218,  -391,
     218,  -391,   -14,    86,  -391,  -391,  -391,   341,   -56,  -391,
    -391
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,     6,     4,     0,     0,   261,
     262,     5,     0,     8,     0,   119,     7,   116,     0,     0,
       0,     0,     0,     0,     0,    73,     0,     0,     0,     0,
     239,   238,   240,   195,   196,   197,   198,   199,   200,   193,
     233,   241,   242,   243,   244,   245,   235,   236,   237,    12,
      13,    11,    49,    15,    72,    10,     9,    14,     0,   194,
     234,   118,     0,     0,     0,     0,    77,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    51,     0,    71,    16,
       0,     0,     0,    50,   116,    74,    75,     0,   148,   157,
     155,   156,   163,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   164,     0,     0,     0,     0,     0,     0,   228,
     229,     0,     0,     0,   149,   150,   151,   152,   153,   154,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   192,
       0,     0,   247,   159,   248,   158,   246,   233,   242,   243,
     244,   245,   235,   208,   191,   188,     0,   122,   121,   139,
     135,   184,   136,   162,   137,   138,   160,   161,     0,     0,
       0,     0,     0,     0,     0,     0,   133,    48,     0,    90,
     218,     0,    87,    89,    82,   264,   263,     0,     0,     0,
     266,   269,   273,   276,   279,   281,   283,    43,    44,    45,
      53,    42,    21,   209,    47,   115,   111,   112,   113,   107,
       0,     0,     0,   210,   211,   213,   214,   215,   216,   217,
     212,     0,    92,   100,   101,   109,   102,   103,   104,   106,
     105,     0,     0,     0,     0,    94,    46,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   120,     0,   125,     0,   126,    81,    85,
     144,   146,    76,     0,     0,     0,     0,   252,    83,     0,
       0,    84,     0,     0,     0,     0,    80,     0,   267,   268,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    73,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,    34,     0,    18,     0,
       0,     0,    79,     0,     0,     0,     0,    86,     0,    96,
     201,   202,   169,   203,   204,   170,   205,   206,   171,   207,
     172,   131,   130,   162,   230,   220,   174,   219,   173,   221,
     175,   165,   222,   223,   224,   225,   226,   227,   182,   254,
     181,   187,   140,   142,   166,   141,   147,   231,   132,   232,
     189,   190,   129,   128,   127,   186,   176,   177,   178,   179,
     180,   183,   123,   124,     0,    78,     0,     0,     0,     0,
       0,     0,     0,   185,   117,   134,    91,    88,   265,   270,
     271,   272,   274,   275,   277,   278,   280,   282,   284,    52,
       0,     0,     0,     0,     0,     0,     0,     0,    62,    63,
       0,    54,    57,    56,    55,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    17,    21,
      20,   114,   108,   110,    93,    97,    98,    99,    95,     0,
       0,   145,   249,   251,   250,   253,   258,   255,   259,   260,
     256,   257,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    59,    25,    22,    33,    32,    35,    37,    39,
      31,    23,    24,    26,    27,    28,    29,    30,    19,   143,
       0,     0,    65,    69,    70,    66,    67,    68,    60,    61,
      64,    58,     0,     0,   168,   167,    38,     0,     0,    41,
      40
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -391,  -391,  -391,  -391,  -391,  -391,  -391,  -391,  -306,  -391,
      -7,  -391,  -391,  -391,   447,  -391,  -391,  -390,  -391,   199,
     476,  -391,  -391,   216,  -391,  -244,  -217,  -391,  -391,   -11,
    -391,   490,  -391,  -132,  -391,   222,   -63,  -391,    56,   123,
    -391,  -391,   280,  -391,  -391,  -391,  -391,    84,    87,    85,
    -391,   -60,  -155,  -391,  -391,  -391,  -391,  -391,  -391,  -391,
    -391,   -65,  -391,  -391,  -391,  -391,  -391,   500,  -391,  -391,
     126,   129,   124,  -391,  -391,     2,  -391,  -104,   -61,   -42,
     220,   223,   224,   -73
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     6,     8,    14,    49,   192,   317,   318,
     477,   478,    50,    51,    52,   190,   301,   421,   422,    53,
      54,    55,   161,   171,   172,   211,   224,   225,   212,    85,
      69,    86,    62,   146,   147,   165,   148,   149,   362,   269,
     270,   365,   150,   151,   363,   152,    58,   332,   335,   338,
     340,   153,   194,   213,   214,   215,   216,   217,   218,   219,
     220,   173,   348,   346,   350,   154,   155,   156,    60,   157,
     278,   360,   281,   457,   460,    61,   180,   181,   182,   183,
     184,   185,   186,   271
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       187,   188,   189,    56,   191,   272,   166,   174,   329,    11,
     167,   264,   440,   328,     3,   265,   267,   221,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,   385,
      99,   100,   472,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   336,   111,   112,   168,   113,   330,   221,
     114,   115,   116,   117,   118,   119,   120,   121,   333,   122,
     123,   124,     4,    15,   125,   126,   300,     5,     7,   127,
      84,   222,   223,   288,   289,   175,   176,    12,   398,   444,
     128,   501,   337,   169,   300,   331,   129,   300,   130,   226,
     300,     9,    10,   268,   300,   450,   334,   131,    23,   505,
     439,   375,   510,   222,   223,   503,   290,    13,    30,    31,
      32,   448,   302,   303,   304,    63,    70,   132,   133,   134,
     135,   136,   137,    41,   138,   139,   140,   141,   142,    47,
      48,    73,   382,   488,   383,   143,   305,   471,   177,   279,
     280,   144,   178,   179,    64,   306,     9,    10,   307,    71,
     145,    74,    72,   410,   274,   275,   276,   277,   170,   344,
      75,   175,   176,    76,   352,   353,   354,   355,   356,   357,
     358,   343,   361,   364,   366,   175,   176,    77,   370,   371,
     372,   373,    79,    65,    66,    67,   308,   399,   400,   401,
     294,   295,    68,   411,   412,   291,   292,   293,    80,   302,
     303,   304,   413,   414,   415,    81,   416,   417,   418,   419,
     420,   309,   396,   310,   311,   312,   313,   314,   296,   297,
      82,   166,   393,   305,   177,   458,   459,    87,   178,   179,
     315,   490,   306,   402,   403,   307,   158,   143,   177,   316,
     159,   160,   178,   179,   162,   507,   441,   442,   443,   163,
     367,   445,   369,   447,   404,   405,   170,   374,   193,   376,
     377,   378,   379,   380,   381,   227,   228,   229,   230,   231,
     232,   233,   234,   308,   235,   236,   481,   237,   238,   410,
     239,   240,   241,   242,   243,   394,   195,   244,   245,   246,
     424,   247,   248,   196,   197,   198,   249,   250,   309,   251,
     310,   311,   312,   313,   314,   252,   253,   254,    88,    89,
      90,    91,    92,   452,   453,   255,   256,   315,   257,   411,
     412,   258,   259,   199,   200,   201,   316,   202,   413,   414,
     415,   260,   416,   417,   418,   419,   420,   261,   262,   263,
     114,   115,   116,   117,   118,   119,   282,   273,   283,   284,
     286,   285,   287,   298,   299,   476,   479,   203,   204,   205,
     206,   207,   208,   209,   300,   319,   320,   321,   322,   324,
     210,   323,   339,   325,   326,   327,   364,   491,   345,   351,
     347,   359,   349,   384,   389,   386,   368,   387,   388,   390,
     493,   494,   495,   496,   497,   498,   391,   500,    30,    31,
      32,   392,   492,   425,   426,   427,   428,   341,   133,   134,
     135,   136,    40,    41,    42,    43,    44,    45,    46,    47,
      48,   429,   430,   431,   432,   143,   433,   434,   435,   479,
     508,   436,   480,   266,   482,   483,   484,   485,   486,   487,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,   437,    99,   100,   438,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   446,   111,   112,   449,   113,
     456,   462,   114,   115,   116,   117,   118,   119,   120,   121,
     463,   122,   123,   124,   464,   465,   125,   126,   466,   467,
     468,   127,   469,   470,   499,   506,   504,   502,    83,   509,
     423,    78,   128,   397,    57,   489,   395,   451,   129,   473,
     130,   342,   475,   474,    59,   455,   461,   454,   406,   131,
       0,     0,   407,     0,   408,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,     0,     0,     0,     0,   132,
     133,   134,   135,   136,   137,    41,   138,   139,   140,   141,
     142,    47,    48,     0,     0,     0,     0,   143,     0,     0,
       0,     0,     0,   144,     0,     0,     0,     0,     0,     0,
       0,     0,   145,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,     0,    99,   100,     0,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,     0,   111,
     112,     0,   113,     0,     0,   114,   115,   116,   117,   118,
     119,   120,   121,     0,   122,   123,   124,     0,     0,   125,
     126,     0,     0,     0,   127,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   128,     0,     0,     0,     0,
       0,   129,     0,   130,     0,     0,     0,     0,     0,     0,
       0,     0,   131,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    30,    31,    32,     0,     0,     0,     0,
       0,     0,   132,   133,   134,   135,   136,   137,    41,   138,
     139,   140,   141,   142,    47,    48,     0,     0,     0,     0,
     143,     0,     0,     0,     0,     0,   144,     0,     0,     0,
       0,     0,     0,     0,     0,   145,    88,    89,    90,    91,
      92,    93,    94,    95,    96,   164,    98,     0,    99,   100,
       0,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,     0,   111,   112,     0,   113,     0,     0,   114,   115,
     116,   117,   118,   119,   120,   121,     0,   122,     0,   124,
       0,     0,   125,   126,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    15,    16,     0,     0,     0,     0,     0,
      17,     0,     0,     0,   129,     0,     0,     0,     0,     0,
       0,    18,     0,    19,     0,   131,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    30,    31,    32,     0,
       0,     0,     0,     0,     0,   132,   133,   134,   135,   136,
     137,    41,   138,   139,   140,   141,   142,    47,    48,     0,
       0,     0,     0,   143,    20,    21,    22,     0,     0,   144,
       0,   409,     0,    23,    24,     0,     0,    84,   145,     0,
       0,    25,     0,    26,     0,     0,     9,    10,   410,     0,
       0,    27,    28,     0,    29,     0,     0,     0,     0,     0,
       0,     0,    30,    31,    32,    33,    34,    35,    36,    37,
      38,     0,     0,     0,    39,     0,    40,    41,    42,    43,
      44,    45,    46,    47,    48,     0,     0,     0,   411,   412,
       0,     0,     0,     0,     0,     0,     0,   413,   414,   415,
       0,   416,   417,   418,   419,   420,     0,     0,    25,     0,
       0,     0,     0,     9,    10
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        73,    74,    75,    14,    77,   160,    69,    72,   225,     7,
      70,     5,   318,     5,     0,   147,   148,    41,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,   273,
      24,    25,   422,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    88,    38,    39,    26,    41,    88,    41,
      44,    45,    46,    47,    48,    49,    50,    51,    88,    53,
      54,    55,    79,     3,    58,    59,   146,    84,     7,    63,
      10,    95,    96,   177,   178,    89,    90,     7,   158,   323,
      74,   471,   127,    63,   146,   125,    80,   146,    82,    87,
     146,    86,    87,   158,   146,   157,   126,    91,    73,   158,
       5,   256,   158,    95,    96,   157,   179,     3,   102,   103,
     104,   328,    17,    18,    19,    23,     6,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,     6,   264,   439,   266,   129,    41,     5,   152,    42,
      43,   135,   156,   157,    52,    50,    86,    87,    53,     3,
     144,     6,     6,    21,    37,    38,    39,    40,   138,   232,
       6,    89,    90,     3,   237,   238,   239,   240,   241,   242,
     243,   231,   245,   246,   247,    89,    90,     6,   251,   252,
     253,   254,     3,    91,    92,    93,    91,   291,   292,   293,
     152,   153,   100,    61,    62,   149,   150,   151,     6,    17,
      18,    19,    70,    71,    72,     3,    74,    75,    76,    77,
      78,   116,   285,   118,   119,   120,   121,   122,   154,   155,
       3,   284,   282,    41,   152,    56,    57,     6,   156,   157,
     135,   159,    50,   294,   295,    53,    11,   129,   152,   144,
      11,    11,   156,   157,    11,   159,   319,   320,   321,    11,
     248,   324,   250,   326,   296,   297,   138,   255,   130,   257,
     258,   259,   260,   261,   262,     6,     6,     6,     6,     6,
       6,     6,     6,    91,     6,     6,   431,     6,     6,    21,
       6,     6,     6,     6,     6,   283,    60,     6,     6,     6,
     301,     6,     6,    67,    68,    69,     6,     6,   116,     6,
     118,   119,   120,   121,   122,     6,     6,     6,    12,    13,
      14,    15,    16,   386,   387,     6,     6,   135,     6,    61,
      62,     6,     6,    97,    98,    99,   144,   101,    70,    71,
      72,     6,    74,    75,    76,    77,    78,     6,     6,     4,
      44,    45,    46,    47,    48,    49,     6,    11,    11,     5,
       4,     6,     5,   148,   147,   428,   429,   131,   132,   133,
     134,   135,   136,   137,   146,     6,     6,     6,     4,     6,
     144,     5,   128,     6,     6,     4,   449,   450,   139,   142,
     140,   145,   141,     5,     9,     6,    84,     6,     6,     6,
     463,   464,   465,   466,   467,   468,     6,   470,   102,   103,
     104,     5,   462,     6,     6,     6,     6,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,     6,     6,     6,     6,   129,     6,     6,     6,   502,
     503,     6,   430,     5,   432,   433,   434,   435,   436,   437,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,     6,    24,    25,     4,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    85,    38,    39,     9,    41,
     143,     6,    44,    45,    46,    47,    48,    49,    50,    51,
       6,    53,    54,    55,     6,     6,    58,    59,     6,     6,
       6,    63,     6,     6,    84,   502,   158,     9,    51,   158,
     301,    25,    74,   287,    14,   449,   284,   384,    80,   425,
      82,   231,   427,   426,    14,   389,   392,   388,   298,    91,
      -1,    -1,   299,    -1,   300,    -1,    -1,    -1,    -1,    -1,
     102,   103,   104,    -1,    -1,    -1,    -1,    -1,    -1,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,    -1,    -1,    -1,    -1,   129,    -1,    -1,
      -1,    -1,    -1,   135,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   144,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    -1,    24,    25,    -1,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    -1,    38,
      39,    -1,    41,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    51,    -1,    53,    54,    55,    -1,    -1,    58,
      59,    -1,    -1,    -1,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    74,    -1,    -1,    -1,    -1,
      -1,    80,    -1,    82,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   102,   103,   104,    -1,    -1,    -1,    -1,
      -1,    -1,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    -1,    -1,    -1,   135,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   144,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    -1,    24,    25,
      -1,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    -1,    38,    39,    -1,    41,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    -1,    53,    -1,    55,
      -1,    -1,    58,    59,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,    -1,    -1,    -1,
      10,    -1,    -1,    -1,    80,    -1,    -1,    -1,    -1,    -1,
      -1,    21,    -1,    23,    -1,    91,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   102,   103,   104,    -1,
      -1,    -1,    -1,    -1,    -1,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,    -1,
      -1,    -1,    -1,   129,    64,    65,    66,    -1,    -1,   135,
      -1,     4,    -1,    73,    74,    -1,    -1,    10,   144,    -1,
      -1,    81,    -1,    83,    -1,    -1,    86,    87,    21,    -1,
      -1,    91,    92,    -1,    94,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   102,   103,   104,   105,   106,   107,   108,   109,
     110,    -1,    -1,    -1,   114,    -1,   116,   117,   118,   119,
     120,   121,   122,   123,   124,    -1,    -1,    -1,    61,    62,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,
      -1,    74,    75,    76,    77,    78,    -1,    -1,    81,    -1,
      -1,    -1,    -1,    86,    87
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   161,   162,     0,    79,    84,   163,     7,   164,    86,
      87,   235,     7,     3,   165,     3,     4,    10,    21,    23,
      64,    65,    66,    73,    74,    81,    83,    91,    92,    94,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   114,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   166,
     172,   173,   174,   179,   180,   181,   189,   191,   206,   227,
     228,   235,   192,    23,    52,    91,    92,    93,   100,   190,
       6,     3,     6,     6,     6,     6,     3,     6,   180,     3,
       6,     3,     3,   174,    10,   189,   191,     6,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    24,
      25,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    38,    39,    41,    44,    45,    46,    47,    48,    49,
      50,    51,    53,    54,    55,    58,    59,    63,    74,    80,
      82,    91,   111,   112,   113,   114,   115,   116,   118,   119,
     120,   121,   122,   129,   135,   144,   193,   194,   196,   197,
     202,   203,   205,   211,   225,   226,   227,   229,    11,    11,
      11,   182,    11,    11,    21,   195,   196,   211,    26,    63,
     138,   183,   184,   221,   221,    89,    90,   152,   156,   157,
     236,   237,   238,   239,   240,   241,   242,   243,   243,   243,
     175,   243,   167,   130,   212,    60,    67,    68,    69,    97,
      98,    99,   101,   131,   132,   133,   134,   135,   136,   137,
     144,   185,   188,   213,   214,   215,   216,   217,   218,   219,
     220,    41,    95,    96,   186,   187,   235,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     4,     5,   193,     5,   193,   221,   199,
     200,   243,   212,    11,    37,    38,    39,    40,   230,    42,
      43,   232,     6,    11,     5,     6,     4,     5,   237,   237,
     243,   149,   150,   151,   152,   153,   154,   155,   148,   147,
     146,   176,    17,    18,    19,    41,    50,    53,    91,   116,
     118,   119,   120,   121,   122,   135,   144,   168,   169,     6,
       6,     6,     4,     5,     6,     6,     6,     4,     5,   186,
      88,   125,   207,    88,   126,   208,    88,   127,   209,   128,
     210,   111,   202,   211,   243,   139,   223,   140,   222,   141,
     224,   142,   243,   243,   243,   243,   243,   243,   243,   145,
     231,   243,   198,   204,   243,   201,   243,   235,    84,   235,
     243,   243,   243,   243,   235,   212,   235,   235,   235,   235,
     235,   235,   193,   193,     5,   185,     6,     6,     6,     9,
       6,     6,     5,   211,   235,   195,   243,   183,   158,   237,
     237,   237,   238,   238,   239,   239,   240,   241,   242,     4,
      21,    61,    62,    70,    71,    72,    74,    75,    76,    77,
      78,   177,   178,   179,   189,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     4,     5,
     168,   243,   243,   243,   185,   243,    85,   243,   186,     9,
     157,   199,   243,   243,   231,   230,   143,   233,    56,    57,
     234,   232,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     5,   177,   207,   208,   209,   243,   170,   171,   243,
     235,   212,   235,   235,   235,   235,   235,   235,   168,   198,
     159,   243,   211,   243,   243,   243,   243,   243,   243,    84,
     243,   177,     9,   157,   158,   158,   170,   159,   243,   158,
     158
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
     405,   406,   407,   408,   409,   410,   411,   412,   413,   414
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   160,   161,   162,   162,   163,   164,   163,   165,   165,
     165,   165,   165,   165,   165,   165,   167,   166,   168,   168,
     168,   169,   169,   169,   169,   169,   169,   169,   169,   169,
     169,   169,   169,   169,   169,   169,   169,   170,   170,   171,
     171,   171,   172,   172,   172,   172,   172,   172,   172,   173,
     173,   175,   174,   176,   176,   176,   176,   177,   177,   177,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   179,   179,   180,   180,   180,   181,   182,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   183,   183,   184,
     184,   184,   185,   185,   186,   186,   186,   187,   187,   187,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   190,   189,   189,   192,
     191,   193,   193,   193,   193,   193,   193,   194,   194,   194,
     194,   194,   194,   195,   195,   196,   196,   196,   196,   196,
     197,   197,   198,   198,   199,   199,   200,   201,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   203,   203,   204,   204,   204,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   205,   205,   205,   205,   205,   205,   205,
     205,   205,   205,   206,   206,   206,   206,   206,   206,   206,
     206,   207,   207,   208,   208,   209,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   225,   225,   225,   225,   225,   225,   225,
     225,   226,   226,   227,   227,   227,   227,   227,   227,   227,
     227,   227,   228,   228,   228,   228,   229,   229,   229,   230,
     230,   230,   230,   230,   231,   232,   232,   232,   233,   234,
     234,   235,   235,   236,   236,   236,   237,   237,   237,   238,
     238,   238,   238,   239,   239,   239,   240,   240,   240,   241,
     241,   242,   242,   243,   243
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     3,     0,     6,     0,     2,
       2,     2,     2,     2,     2,     2,     0,     5,     1,     3,
       2,     0,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     3,     1,     1,     3,     1,
       4,     4,     3,     3,     3,     3,     3,     3,     3,     1,
       2,     0,     5,     0,     2,     2,     2,     1,     3,     2,
       3,     3,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     2,     1,     0,     2,     2,     4,     0,     5,     4,
       4,     4,     3,     4,     4,     4,     4,     1,     3,     1,
       1,     3,     1,     3,     1,     3,     2,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     1,
       3,     1,     1,     1,     3,     1,     0,     5,     1,     0,
       4,     1,     1,     3,     3,     2,     2,     3,     3,     3,
       3,     3,     3,     1,     3,     1,     1,     1,     1,     1,
       3,     3,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     4,     4,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     3,     3,     3,     1,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     3,     3,     3,     1,     1,
       3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     1,     3,     1,     3,     3,     3,     1,     1,
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
  "SPK_ID", "SPK_SELECT", "HEADER_AUTH", "REVOKE_ID", "SPLIT_MODE",
  "SPLIT_FMT", "BOOT", "USER", "STATIC", "NOAUTOSTART", "MULTIBOOT",
  "PROTECTED", "BLOCKS", "AUTHBLOCKS", "BOOTVECTORS", "PRESIGN",
  "BIF_SECTION", "UDF_DATA", "MCS", "BIN", "SLR_NUM", "CLUSTER_NUM",
  "DICE", "PCR_NUMBER", "PCR_MEASUREMENT_INDEX", "IMAGE_STORE",
  "PARENT_ID", "ID_CODE", "EXT_ID_CODE", "BYPASS_IDCODE_CHECK", "A_HWROT",
  "S_HWROT", "UNIQUE_ID", "PARENT_UNIQUE_ID", "FUNCTION_ID", "IMAGE", "ID",
  "NAME", "DELAY_HANDOFF", "DELAY_LOAD", "COPY", "INCLUDE", "DELAY_AUTH",
  "PARTITION", "PFILE", "METAHEADER", "WORD", "HEXWORD", "FILENAME",
  "QFILENAME", "NONE", "DECVALUE", "HEXVALUE", "KEYSRC_ENCRYPTION",
  "FSBL_CONFIG", "AUTH_PARAMS", "AUTHJTAG_CONFIG", "DEVICE_DNA",
  "JTAG_TIMEOUT", "PUF4KMODE", "PUFROSWAP", "SHUTTER", "SPLIT",
  "SMAP_WIDTH", "PUF_HELPER_FILE", "BH_KEY_FILE", "BH_KEY_IV", "BH_KEK_IV",
  "BBRAM_KEK_IV", "EFUSE_KEK_IV", "EFUSE_USER_KEK0_IV",
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
  "metahdr_blk", "metahdr_blk_attr", "new_pdi_spec", "image_spec",
  "image_list", "$@3", "image_content", "image_attributes_list",
  "image_attributes", "partition_spec", "partition_content", "other_spec",
  "$@4", "sec_boot_attr_list", "sec_boot_attr", "fsbl_attr_list",
  "authjtag_attr_list", "authjtag_attr", "fsbl_attr", "file_spec", "$@5",
  "new_file_spec", "$@6", "new_attribute_list", "new_attribute",
  "attribute_list", "attribute", "blocksattr_list", "blocksattr",
  "bootvectors_list", "bootvector", "authblockattr", "boolattr",
  "trustzone_type", "blockattr", "optattr", "other_file_attr", "authvalue",
  "encrvalue", "checkvalue", "pownervalue", "ptypevalue", "key_src",
  "core", "bh_rsa", "dpa_cm", "auth_hash", "int_hash", "pufhd_loc",
  "opt_key", "auth_only", "boot_device_type", "dest_cpu_type",
  "dest_device_type", "exception_level_type", "numattr", "fileattr",
  "key_file", "rsa_key_file", "other_files", "auth_parameters",
  "spkselect", "split_options", "splitmode", "splitfmt", "filename",
  "number", "unary_expression", "multiplicative_expression",
  "additive_expression", "shift_expression", "and_expression",
  "xor_expression", "expression", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BisonParser::rhs_number_type
  BisonParser::yyrhs_[] =
  {
       161,     0,    -1,   162,    -1,    -1,   162,   163,    -1,    79,
       7,   235,    -1,    -1,    84,   164,     7,     3,   165,     4,
      -1,    -1,   165,   189,    -1,   165,   181,    -1,   165,   173,
      -1,   165,   166,    -1,   165,   172,    -1,   165,   191,    -1,
     165,   179,    -1,    -1,    83,     3,   167,   168,     4,    -1,
     169,    -1,   169,     5,   168,    -1,   169,   168,    -1,    -1,
      18,     6,   208,    -1,    91,     6,   212,    -1,   116,     6,
     235,    -1,    17,     6,   207,    -1,   118,     6,   235,    -1,
     119,     6,   235,    -1,   120,     6,   235,    -1,   121,     6,
     235,    -1,   122,     6,   235,    -1,    53,     6,   235,    -1,
      41,     6,   243,    -1,    19,     6,   209,    -1,   144,    -1,
      50,     6,   170,    -1,   135,    -1,   171,    -1,   171,     9,
     170,    -1,   243,    -1,   243,   157,   243,   158,    -1,   243,
     157,   159,   158,    -1,    74,     6,   243,    -1,    64,     6,
     243,    -1,    65,     6,   243,    -1,    66,     6,   243,    -1,
     206,     6,   235,    -1,    91,     6,   212,    -1,    21,     6,
     211,    -1,   174,    -1,   173,   174,    -1,    -1,    73,     3,
     175,   176,     4,    -1,    -1,   176,   177,    -1,   176,   189,
      -1,   176,   179,    -1,   178,    -1,   178,     5,   177,    -1,
     178,   177,    -1,    74,     6,   243,    -1,    75,     6,    84,
      -1,    76,    -1,    77,    -1,    78,     6,   243,    -1,    21,
       6,   211,    -1,    70,     6,   243,    -1,    71,     6,   243,
      -1,    72,     6,   243,    -1,    61,     6,   243,    -1,    62,
       6,   243,    -1,    81,   180,    -1,   180,    -1,    -1,   180,
     189,    -1,   180,   191,    -1,    10,    91,    11,   212,    -1,
      -1,    10,    92,   182,    11,   185,    -1,    92,     3,   185,
       4,    -1,    23,     3,   183,     4,    -1,    10,    23,    11,
     221,    -1,    23,     6,   221,    -1,    10,    93,    11,   230,
      -1,    10,   100,    11,   232,    -1,    10,    52,    11,   199,
      -1,    94,     3,   186,     4,    -1,   184,    -1,   184,     5,
     183,    -1,   221,    -1,    63,    -1,    26,     6,   243,    -1,
     188,    -1,   188,     5,   185,    -1,   187,    -1,   187,     5,
     186,    -1,   187,   186,    -1,    41,     6,   243,    -1,    95,
       6,    85,    -1,    96,     6,   243,    -1,   213,    -1,   214,
      -1,   216,    -1,   217,    -1,   218,    -1,   220,    -1,   219,
      -1,    97,    -1,    99,     6,   243,    -1,   215,    -1,   101,
       6,   243,    -1,    67,    -1,    68,    -1,    69,    -1,    98,
       6,   243,    -1,    60,    -1,    -1,    10,   190,   195,    11,
     235,    -1,   235,    -1,    -1,     3,   192,   193,     4,    -1,
     196,    -1,   194,    -1,   194,     5,   193,    -1,   196,     5,
     193,    -1,   194,   193,    -1,   196,   193,    -1,    82,     6,
     235,    -1,    74,     6,   243,    -1,    63,     6,   243,    -1,
      21,     6,   202,    -1,    21,     6,   111,    -1,    54,     6,
      84,    -1,   196,    -1,   196,     5,   195,    -1,   202,    -1,
     205,    -1,   225,    -1,   226,    -1,   197,    -1,    50,     6,
     198,    -1,    51,     6,   201,    -1,   204,    -1,   204,     9,
     198,    -1,   200,    -1,   200,     5,   199,    -1,   243,    -1,
     243,    -1,    12,    -1,    44,    -1,    45,    -1,    46,    -1,
      47,    -1,    48,    -1,    49,    -1,    14,    -1,    15,    -1,
      13,    -1,   114,    -1,   112,    -1,   227,    -1,   229,    -1,
     211,    -1,    16,    -1,    28,    -1,    28,     6,   142,    -1,
     243,    -1,   243,   157,   243,   158,    -1,   243,   157,   159,
     158,    -1,    17,     6,   207,    -1,    18,     6,   208,    -1,
      19,     6,   209,    -1,    20,     6,   210,    -1,    25,     6,
     222,    -1,    24,     6,   223,    -1,    27,     6,   224,    -1,
     116,     6,   235,    -1,   118,     6,   235,    -1,   119,     6,
     235,    -1,   120,     6,   235,    -1,   121,     6,   235,    -1,
      39,     6,   231,    -1,    38,     6,   243,    -1,   122,     6,
     235,    -1,   203,    -1,    21,     6,   211,    -1,    91,     6,
     212,    -1,    41,     6,   243,    -1,   144,    -1,    58,     6,
     243,    -1,    59,     6,   243,    -1,   135,    -1,    80,    -1,
     114,    -1,   227,    -1,   105,    -1,   106,    -1,   107,    -1,
     108,    -1,   109,    -1,   110,    -1,    88,    -1,   125,    -1,
      88,    -1,   126,    -1,    88,    -1,   127,    -1,   128,    -1,
     129,    -1,   130,    -1,   131,    -1,   132,    -1,   144,    -1,
     133,    -1,   134,    -1,   135,    -1,   136,    -1,   137,    -1,
     138,    -1,   140,    -1,   139,    -1,   141,    -1,    29,     6,
     243,    -1,    30,     6,   243,    -1,    31,     6,   243,    -1,
      32,     6,   243,    -1,    33,     6,   243,    -1,    34,     6,
     243,    -1,    35,    -1,    36,    -1,    22,     6,   243,    -1,
      53,     6,   235,    -1,    55,     6,   235,    -1,   116,    -1,
     228,    -1,   122,    -1,   123,    -1,   124,    -1,   103,    -1,
     102,    -1,   104,    -1,   117,    -1,   118,    -1,   119,    -1,
     120,    -1,   121,    -1,   115,    -1,   111,    -1,   113,    -1,
      37,     6,   243,    -1,    39,     6,   231,    -1,    38,     6,
     243,    -1,    40,    -1,   230,     9,   230,    -1,   145,    -1,
      42,     6,   233,    -1,    43,     6,   234,    -1,   232,     5,
     232,    -1,   143,    -1,    56,    -1,    57,    -1,    86,    -1,
      87,    -1,    90,    -1,    89,    -1,   157,   243,   158,    -1,
     236,    -1,   152,   237,    -1,   156,   237,    -1,   237,    -1,
     238,   149,   237,    -1,   238,   150,   237,    -1,   238,   151,
     237,    -1,   238,    -1,   239,   152,   238,    -1,   239,   153,
     238,    -1,   239,    -1,   240,   154,   239,    -1,   240,   155,
     239,    -1,   240,    -1,   241,   148,   240,    -1,   241,    -1,
     242,   147,   241,    -1,   242,    -1,   243,   146,   242,    -1
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
     124,   129,   134,   138,   142,   146,   150,   154,   158,   162,
     164,   167,   168,   174,   175,   178,   181,   184,   186,   190,
     193,   197,   201,   203,   205,   209,   213,   217,   221,   225,
     229,   233,   236,   238,   239,   242,   245,   250,   251,   257,
     262,   267,   272,   276,   281,   286,   291,   296,   298,   302,
     304,   306,   310,   312,   316,   318,   322,   325,   329,   333,
     337,   339,   341,   343,   345,   347,   349,   351,   353,   357,
     359,   363,   365,   367,   369,   373,   375,   376,   382,   384,
     385,   390,   392,   394,   398,   402,   405,   408,   412,   416,
     420,   424,   428,   432,   434,   438,   440,   442,   444,   446,
     448,   452,   456,   458,   462,   464,   468,   470,   472,   474,
     476,   478,   480,   482,   484,   486,   488,   490,   492,   494,
     496,   498,   500,   502,   504,   506,   510,   512,   517,   522,
     526,   530,   534,   538,   542,   546,   550,   554,   558,   562,
     566,   570,   574,   578,   582,   584,   588,   592,   596,   598,
     602,   606,   608,   610,   612,   614,   616,   618,   620,   622,
     624,   626,   628,   630,   632,   634,   636,   638,   640,   642,
     644,   646,   648,   650,   652,   654,   656,   658,   660,   662,
     664,   666,   668,   672,   676,   680,   684,   688,   692,   694,
     696,   700,   704,   708,   710,   712,   714,   716,   718,   720,
     722,   724,   726,   728,   730,   732,   734,   736,   738,   740,
     744,   748,   752,   754,   758,   760,   764,   768,   772,   774,
     776,   778,   780,   782,   784,   786,   790,   792,   795,   798,
     800,   804,   808,   812,   814,   818,   822,   824,   828,   832,
     834,   838,   840,   844,   846
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   197,   197,   199,   200,   203,   204,   204,   210,   211,
     212,   213,   214,   215,   216,   217,   220,   220,   225,   226,
     227,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   249,   250,   253,
     255,   257,   261,   262,   263,   264,   265,   266,   267,   270,
     271,   274,   274,   278,   279,   280,   281,   284,   285,   286,
     289,   290,   291,   292,   293,   294,   295,   296,   297,   298,
     301,   306,   307,   310,   311,   312,   315,   318,   318,   321,
     322,   323,   328,   329,   330,   331,   332,   335,   336,   339,
     340,   341,   344,   345,   348,   349,   350,   353,   354,   355,
     358,   361,   367,   369,   371,   373,   375,   377,   379,   381,
     387,   391,   392,   393,   394,   399,   406,   406,   413,   419,
     419,   425,   426,   427,   428,   429,   430,   433,   436,   437,
     439,   440,   441,   446,   447,   450,   451,   452,   453,   454,
     457,   458,   461,   462,   465,   466,   469,   474,   476,   477,
     478,   479,   480,   481,   482,   483,   484,   485,   488,   489,
     490,   491,   492,   493,   496,   497,   500,   501,   502,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   514,
     515,   516,   517,   518,   519,   520,   521,   522,   523,   524,
     525,   526,   527,   530,   531,   532,   533,   534,   535,   536,
     537,   540,   541,   544,   545,   548,   549,   552,   555,   558,
     561,   564,   567,   570,   573,   576,   579,   582,   585,   588,
     591,   594,   597,   598,   599,   600,   601,   602,   603,   604,
     605,   608,   609,   612,   613,   614,   615,   616,   617,   618,
     619,   620,   623,   624,   625,   626,   629,   630,   631,   634,
     637,   640,   643,   646,   649,   652,   655,   656,   659,   662,
     665,   671,   672,   675,   676,   677,   683,   684,   685,   689,
     690,   691,   692,   695,   696,   697,   700,   701,   702,   705,
     706,   709,   710,   714,   715
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
     155,   156,   157,   158,   159
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 914;
  const int BisonParser::yynnts_ = 84;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 160;

  const unsigned int BisonParser::yyuser_token_number_max_ = 414;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 24 "../s/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 2495 "../bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 720 "../s/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
