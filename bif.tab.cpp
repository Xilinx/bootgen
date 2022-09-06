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
#line 52 "../s/bif.y"

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
#line 46 "../s/bif.y"

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
#line 23 "../s/bif.y"
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
#line 30 "../s/bif.y"
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
#line 202 "../s/bif.y"
    { options.includeBifOptionsList.push_back((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 203 "../s/bif.y"
    { currentBifOptions = new BifOptions(options.GetArchType(),(yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 7:
/* Line 670 of lalr1.cc  */
#line 205 "../s/bif.y"
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 219 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries()); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 230 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 231 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t), options.IsVersalNetSeries()); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 232 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.encrKeyFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 233 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 234 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 235 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 236 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 237 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 238 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 239 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 240 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.revokeId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 241 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 34:
/* Line 670 of lalr1.cc  */
#line 242 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 36:
/* Line 670 of lalr1.cc  */
#line 244 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 39:
/* Line 670 of lalr1.cc  */
#line 252 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 254 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 256 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 260 "../s/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 261 "../s/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 262 "../s/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 263 "../s/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 264 "../s/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 265 "../s/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 266 "../s/bif.y"
    { currentBifOptions->SetPdiType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 273 "../s/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 274 "../s/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 288 "../s/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 289 "../s/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 290 "../s/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 291 "../s/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 292 "../s/bif.y"
    { currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 293 "../s/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 294 "../s/bif.y"
    { currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 67:
/* Line 670 of lalr1.cc  */
#line 295 "../s/bif.y"
    { currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 68:
/* Line 670 of lalr1.cc  */
#line 296 "../s/bif.y"
    { currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 308 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported in VERSAL architecture.\n\t   Please see 'bootgen -arch versal -bif_help keysrc'");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 75:
/* Line 670 of lalr1.cc  */
#line 311 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 79:
/* Line 670 of lalr1.cc  */
#line 316 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                    if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); 
                                                                                    currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 80:
/* Line 670 of lalr1.cc  */
#line 321 "../s/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); }
    break;

  case 87:
/* Line 670 of lalr1.cc  */
#line 332 "../s/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 88:
/* Line 670 of lalr1.cc  */
#line 333 "../s/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 94:
/* Line 670 of lalr1.cc  */
#line 345 "../s/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 95:
/* Line 670 of lalr1.cc  */
#line 346 "../s/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 96:
/* Line 670 of lalr1.cc  */
#line 347 "../s/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 97:
/* Line 670 of lalr1.cc  */
#line 350 "../s/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 98:
/* Line 670 of lalr1.cc  */
#line 353 "../s/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 99:
/* Line 670 of lalr1.cc  */
#line 355 "../s/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 357 "../s/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 359 "../s/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 361 "../s/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 103:
/* Line 670 of lalr1.cc  */
#line 363 "../s/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 365 "../s/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 367 "../s/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 106:
/* Line 670 of lalr1.cc  */
#line 369 "../s/bif.y"
    {  if(options.GetArchType() != Arch::VERSAL)
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t\t'dpacm_enable' is supported only in VERSAL architecture");
                                                                                   if(options.GetArchType() == Arch::VERSAL)
                                                                                     LOG_WARNING("boot_config { dpacm_enable } will be deprecated. 'dpacm_enable' should be specified along with the partition. \n            See 'bootgen -bif_help dpacm_enable' for more info.");
                                                                                   currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));
                                                                                }
    break;

  case 107:
/* Line 670 of lalr1.cc  */
#line 375 "../s/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->smapWidth = (yysemantic_stack_[(3) - (3)].number);
                                                                                }
    break;

  case 108:
/* Line 670 of lalr1.cc  */
#line 379 "../s/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 380 "../s/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 110:
/* Line 670 of lalr1.cc  */
#line 381 "../s/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 382 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                     currentBifOptions->SetPufRingOscilltorSwapConfigValue((yysemantic_stack_[(3) - (3)].number));
                                                                                  else
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t  'puf_ro_swap' is supported only in VersalNet architecture");
                                                                                 }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 387 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL && options.IsVersalNetSeries())
                                                                                     currentBifOptions->SetDiceEnable();
                                                                                  else
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t  'dice_enable' is supported only in VersalNet architecture");
                                                                                 }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 394 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(),options.IsVersalNetSeries()); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 397 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 401 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 407 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType(), options.IsVersalNetSeries()); }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 421 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 424 "../s/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 426 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 427 "../s/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string), currentImageBifOptions->GetImageType());
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 455 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] only supported in ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 460 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 462 "../s/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 463 "../s/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 464 "../s/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 465 "../s/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 466 "../s/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 467 "../s/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 150:
/* Line 670 of lalr1.cc  */
#line 468 "../s/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 151:
/* Line 670 of lalr1.cc  */
#line 469 "../s/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 152:
/* Line 670 of lalr1.cc  */
#line 470 "../s/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 153:
/* Line 670 of lalr1.cc  */
#line 471 "../s/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 474 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 475 "../s/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 476 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 157:
/* Line 670 of lalr1.cc  */
#line 477 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 478 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 479 "../s/bif.y"
    { currentPartitionBifOptions->SetLockStepFlag();}
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 482 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 161:
/* Line 670 of lalr1.cc  */
#line 483 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 486 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 487 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 488 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 490 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 491 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 492 "../s/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 493 "../s/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 494 "../s/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 495 "../s/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 496 "../s/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 497 "../s/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 498 "../s/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 499 "../s/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 500 "../s/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 501 "../s/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 502 "../s/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 503 "../s/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 504 "../s/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 506 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 507 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 508 "../s/bif.y"
    { currentPartitionBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 509 "../s/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 510 "../s/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 511 "../s/bif.y"
    { currentPartitionBifOptions->SetClusterNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 512 "../s/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 513 "../s/bif.y"
    { currentPartitionBifOptions->SetDelayAuth(true); }
    break;

  case 197:
/* Line 670 of lalr1.cc  */
#line 526 "../s/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 530 "../s/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 201:
/* Line 670 of lalr1.cc  */
#line 534 "../s/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 218:
/* Line 670 of lalr1.cc  */
#line 583 "../s/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 219:
/* Line 670 of lalr1.cc  */
#line 584 "../s/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 220:
/* Line 670 of lalr1.cc  */
#line 585 "../s/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), false); }
    break;

  case 221:
/* Line 670 of lalr1.cc  */
#line 586 "../s/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), true); }
    break;

  case 222:
/* Line 670 of lalr1.cc  */
#line 587 "../s/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 223:
/* Line 670 of lalr1.cc  */
#line 588 "../s/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 224:
/* Line 670 of lalr1.cc  */
#line 589 "../s/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 225:
/* Line 670 of lalr1.cc  */
#line 590 "../s/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 226:
/* Line 670 of lalr1.cc  */
#line 591 "../s/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 227:
/* Line 670 of lalr1.cc  */
#line 594 "../s/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 228:
/* Line 670 of lalr1.cc  */
#line 595 "../s/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 245:
/* Line 670 of lalr1.cc  */
#line 620 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 246:
/* Line 670 of lalr1.cc  */
#line 623 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 247:
/* Line 670 of lalr1.cc  */
#line 626 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 248:
/* Line 670 of lalr1.cc  */
#line 629 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 251:
/* Line 670 of lalr1.cc  */
#line 638 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 255:
/* Line 670 of lalr1.cc  */
#line 648 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 256:
/* Line 670 of lalr1.cc  */
#line 651 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 261:
/* Line 670 of lalr1.cc  */
#line 663 "../s/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 263:
/* Line 670 of lalr1.cc  */
#line 670 "../s/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 264:
/* Line 670 of lalr1.cc  */
#line 671 "../s/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 266:
/* Line 670 of lalr1.cc  */
#line 676 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 267:
/* Line 670 of lalr1.cc  */
#line 677 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 268:
/* Line 670 of lalr1.cc  */
#line 678 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 270:
/* Line 670 of lalr1.cc  */
#line 682 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 271:
/* Line 670 of lalr1.cc  */
#line 683 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 273:
/* Line 670 of lalr1.cc  */
#line 687 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 274:
/* Line 670 of lalr1.cc  */
#line 688 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 276:
/* Line 670 of lalr1.cc  */
#line 692 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 278:
/* Line 670 of lalr1.cc  */
#line 696 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 280:
/* Line 670 of lalr1.cc  */
#line 701 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1425 "../bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -372;
  const short int
  BisonParser::yypact_[] =
  {
      -372,     9,    -4,  -372,    44,  -372,  -372,   106,    58,  -372,
    -372,  -372,    13,  -372,   746,  -372,  -372,   114,    68,   100,
      85,    89,    93,    75,   107,  -372,   136,   135,   162,   167,
    -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,
    -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,
    -372,   144,  -372,  -372,    61,  -372,  -372,  -372,   178,  -372,
    -372,  -372,   550,   209,   211,   220,  -372,   225,   226,   680,
     112,   -20,   105,   -11,   -11,   -11,  -372,   -11,    61,  -372,
     115,   214,    19,  -372,  -372,  -372,  -372,   106,  -372,  -372,
    -372,  -372,  -372,   237,   238,   240,   250,   251,   253,   255,
     257,   258,   261,   262,   265,   266,   269,   270,   271,  -372,
    -372,   276,   277,   278,  -372,  -372,  -372,  -372,  -372,  -372,
     279,   282,   288,   290,   296,   297,   298,   299,  -372,   301,
     305,  -372,  -372,  -372,  -372,  -372,   307,   311,   312,   315,
     316,   318,  -372,  -372,  -372,   321,     8,   420,  -372,  -372,
    -372,  -372,  -372,  -372,  -372,  -372,  -372,   105,   -11,   115,
     319,   157,   113,   320,   325,   332,  -372,   333,  -372,   334,
     335,  -372,  -372,  -372,  -372,   -11,   -11,   -11,  -372,  -372,
      59,    63,    57,   121,   197,   206,   206,   206,  -372,   206,
     182,  -372,  -372,  -372,  -372,  -372,  -372,  -372,   344,   345,
     346,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,   349,
     351,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,   348,
     352,   355,   353,    43,  -372,    76,   -35,   -51,   246,   826,
     -11,   229,   235,   236,   234,   -11,   -11,   -11,   -11,   -11,
     -11,   -11,   233,   -11,   -11,   -11,   106,   302,   106,   -11,
     -11,   -11,   106,   115,   106,   106,   106,   106,   106,   106,
    -372,   550,  -372,   550,  -372,  -372,  -372,   371,   206,  -372,
     214,   372,   373,   376,  -372,   368,   378,   379,   381,   112,
     106,   680,   -11,  -372,   -20,  -372,  -372,  -136,   -11,   -11,
     -11,   -11,   -11,   -11,   -11,   -11,   -11,   -11,    79,   382,
     383,   384,   385,   386,   387,   392,   394,   395,   396,   397,
     398,   399,  -372,  -372,   402,   174,   -11,   -11,   -11,  -372,
     214,   -11,   326,   -11,  -372,    19,  -372,  -372,  -372,  -372,
    -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,
    -372,   206,  -372,  -372,  -372,  -372,  -372,  -372,  -372,   206,
     206,   206,   206,   206,   206,   206,  -372,  -372,   206,  -372,
     400,   -50,  -372,   206,  -372,  -372,  -372,   206,   206,   206,
    -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,
     -11,  -372,   -11,   -11,   233,   157,   267,   161,   113,  -372,
    -372,  -372,   206,  -372,  -372,  -372,  -372,  -372,    59,    59,
      63,    63,    57,   121,   197,  -372,   404,   405,   406,   407,
     408,   409,  -372,  -372,   410,  -372,   260,  -372,  -372,    76,
     -35,   -51,   -11,   -11,   106,   115,   106,   106,   106,   106,
     106,   106,  -372,   182,  -372,   206,   206,   206,  -372,   206,
    -372,   206,  -372,   -11,    15,  -372,   206,   206,  -372,   368,
    -372,  -372,  -372,  -372,  -372,   381,   112,   -11,   -11,   -11,
     -11,   337,   -11,   295,  -372,  -372,  -372,  -372,   206,  -372,
     417,   -49,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,
    -372,  -372,   272,   -73,  -372,   206,   206,   206,   206,  -372,
     206,  -372,   -11,    72,  -372,  -372,  -372,   273,   -57,  -372,
    -372
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,     6,     4,     0,     0,   257,
     258,     5,     0,     8,     0,   116,     7,   113,     0,     0,
       0,     0,     0,     0,     0,    71,     0,     0,     0,     0,
     235,   234,   236,   191,   192,   193,   194,   195,   196,   189,
     229,   237,   238,   239,   240,   241,   231,   232,   233,    12,
      13,    11,    49,    15,    70,    10,     9,    14,     0,   190,
     230,   115,     0,     0,     0,     0,    75,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    51,     0,    69,    16,
       0,     0,     0,    50,   113,    72,    73,     0,   144,   153,
     151,   152,   159,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   160,     0,     0,     0,     0,     0,     0,   224,
     225,     0,     0,     0,   145,   146,   147,   148,   149,   150,
       0,     0,     0,     0,     0,     0,     0,     0,   188,     0,
       0,   243,   155,   244,   154,   242,   229,   238,   239,   240,
     241,   231,   204,   187,   184,     0,   119,   118,   135,   131,
     180,   132,   158,   133,   134,   156,   157,     0,     0,     0,
       0,     0,     0,     0,     0,   129,    48,     0,   214,     0,
      85,    87,    80,   260,   259,     0,     0,     0,   262,   265,
     269,   272,   275,   277,   279,    43,    44,    45,    53,    42,
      21,   205,    47,   112,   108,   109,   110,   104,     0,     0,
       0,   206,   207,   209,   210,   211,   212,   213,   208,     0,
      89,    97,    98,   106,    99,   100,   101,   103,   102,     0,
       0,     0,     0,    91,    46,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     117,     0,   122,     0,   123,    79,    83,   140,   142,    74,
       0,     0,     0,     0,   248,    81,     0,     0,    82,     0,
       0,     0,     0,    78,     0,   263,   264,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    71,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    36,    34,     0,    18,     0,     0,     0,    77,
       0,     0,     0,     0,    84,     0,    93,   197,   198,   165,
     199,   200,   166,   201,   202,   167,   203,   168,   127,   126,
     158,   226,   216,   170,   215,   169,   217,   171,   161,   218,
     219,   220,   221,   222,   223,   178,   250,   177,   183,   136,
     138,   162,   137,   143,   227,   128,   228,   185,   186,   125,
     124,   182,   172,   173,   174,   175,   176,   179,   120,   121,
       0,    76,     0,     0,     0,     0,     0,     0,     0,   181,
     114,   130,    88,    86,   261,   266,   267,   268,   270,   271,
     273,   274,   276,   278,   280,    52,     0,     0,     0,     0,
       0,     0,    62,    63,     0,    54,    57,    56,    55,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    17,    21,    20,   111,   105,   107,    90,    94,
      95,    96,    92,     0,     0,   141,   245,   247,   246,   249,
     254,   251,   255,   256,   252,   253,     0,     0,     0,     0,
       0,     0,     0,     0,    59,    25,    22,    33,    32,    35,
      37,    39,    31,    23,    24,    26,    27,    28,    29,    30,
      19,   139,     0,     0,    65,    66,    67,    68,    60,    61,
      64,    58,     0,     0,   164,   163,    38,     0,     0,    41,
      40
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -372,  -372,  -372,  -372,  -372,  -372,  -372,  -372,  -300,  -372,
     -61,  -372,  -372,  -372,   411,  -372,  -372,  -371,  -372,   145,
     421,  -372,  -372,   173,  -372,  -239,  -215,  -372,  -372,   -12,
    -372,   446,  -372,   -78,  -372,   191,   -65,  -372,    20,    96,
    -372,  -372,   248,  -372,  -372,  -372,  -372,    62,    60,    64,
    -372,   -69,  -156,  -372,  -372,  -372,  -372,  -372,  -372,  -372,
    -372,   -67,  -372,  -372,  -372,  -372,  -372,   468,  -372,  -372,
      98,   102,    99,  -372,  -372,    -7,  -372,  -158,   -62,   -60,
     189,   192,   193,   -63
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     6,     8,    14,    49,   190,   314,   315,
     469,   470,    50,    51,    52,   188,   298,   415,   416,    53,
      54,    55,   160,   169,   170,   209,   222,   223,   210,    85,
      69,    86,    62,   145,   146,   164,   147,   148,   359,   266,
     267,   362,   149,   150,   360,   151,    58,   329,   332,   335,
     337,   152,   192,   211,   212,   213,   214,   215,   216,   217,
     218,   171,   345,   343,   347,   153,   154,   155,    60,   156,
     275,   357,   278,   451,   454,    61,   178,   179,   180,   181,
     182,   183,   184,   268
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
        11,   166,    56,   269,   165,   172,   167,   297,   326,     3,
     185,   186,   187,   261,   189,   434,    13,   285,   286,   394,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,   381,    99,   100,   333,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   464,   111,   112,   325,   113,
     330,     7,   114,   115,   116,   117,   118,   119,   120,   121,
     219,   122,   123,   124,    15,    12,   125,   126,   262,   264,
     297,    84,     4,   334,    70,   173,   174,     5,    76,   127,
     224,   438,   495,   405,   219,   128,   297,   129,   331,    84,
     265,    73,   491,   297,   297,    74,   130,   371,   500,    75,
     406,   173,   174,    71,   444,   493,    72,    30,    31,    32,
     442,   220,   221,    77,   287,   168,   131,   132,   133,   134,
     135,   136,    41,   137,   138,   139,   140,   141,    47,    48,
     395,   396,   397,   480,   142,   220,   221,    63,   175,    79,
     143,    80,   176,   177,     9,    10,   407,   408,   409,   144,
     410,   411,   412,   413,   414,   276,   277,    25,   173,   174,
     340,   327,     9,    10,   175,    81,    64,   341,   176,   177,
      82,   482,   349,   350,   351,   352,   353,   354,   355,   433,
     358,   361,   363,   378,    87,   379,   367,   368,   369,     9,
      10,   299,   300,   301,   271,   272,   273,   274,   328,   299,
     300,   301,    65,    66,    67,   288,   289,   290,   293,   294,
     389,    68,   291,   292,    23,   302,   165,   452,   453,   392,
     157,   175,   158,   302,   303,   176,   177,   304,   497,   398,
     399,   159,   303,   400,   401,   304,   161,   162,   142,   364,
     168,   366,   191,   225,   226,   370,   227,   372,   373,   374,
     375,   376,   377,   435,   436,   437,   228,   229,   439,   230,
     441,   231,   305,   232,   233,   463,   295,   234,   235,   473,
     305,   236,   237,   390,   193,   238,   239,   240,   194,   195,
     196,   406,   241,   242,   243,   244,   418,   306,   245,   307,
     308,   309,   310,   311,   246,   306,   247,   307,   308,   309,
     310,   311,   248,   249,   250,   251,   312,   252,   197,   198,
     199,   253,   200,   254,   312,   313,   406,   255,   256,   446,
     447,   257,   258,   313,   259,   260,   279,   407,   408,   409,
     270,   410,   411,   412,   413,   414,   280,   281,   283,   282,
     284,   296,   201,   202,   203,   204,   205,   206,   207,   297,
     316,   317,   318,   319,   321,   208,   320,   324,   322,   468,
     471,   323,   407,   408,   409,   342,   410,   411,   412,   413,
     414,   336,   344,   348,   346,   356,   380,   385,   382,   383,
     361,   483,   384,   365,   386,   387,   388,   484,   419,   420,
     421,   422,   423,   424,   485,   486,   487,   488,   425,   490,
     426,   427,   428,   429,   430,   431,   432,   450,   440,   443,
     456,   457,   458,   459,   460,   461,   462,   472,   489,   474,
     475,   476,   477,   478,   479,   263,   492,   494,   499,   471,
     498,   496,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,   417,    99,   100,    78,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   393,   111,   112,
      57,   113,    83,   481,   114,   115,   116,   117,   118,   119,
     120,   121,   391,   122,   123,   124,   445,   339,   125,   126,
     466,   465,    59,   449,   402,   467,   448,   455,   403,     0,
     404,   127,     0,     0,     0,     0,     0,   128,     0,   129,
       0,     0,     0,     0,     0,     0,     0,     0,   130,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    30,
      31,    32,     0,     0,     0,     0,     0,     0,   131,   132,
     133,   134,   135,   136,    41,   137,   138,   139,   140,   141,
      47,    48,     0,     0,     0,     0,   142,     0,     0,     0,
       0,     0,   143,     0,     0,     0,     0,     0,     0,     0,
       0,   144,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,     0,    99,   100,     0,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,     0,   111,   112,
       0,   113,     0,     0,   114,   115,   116,   117,   118,   119,
     120,   121,     0,   122,   123,   124,     0,     0,   125,   126,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   127,     0,     0,     0,     0,     0,   128,     0,   129,
       0,     0,     0,     0,     0,     0,     0,     0,   130,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    30,
      31,    32,     0,     0,     0,     0,     0,     0,   131,   132,
     133,   134,   135,   136,    41,   137,   138,   139,   140,   141,
      47,    48,     0,     0,     0,     0,   142,     0,     0,     0,
       0,     0,   143,     0,     0,     0,     0,     0,     0,     0,
       0,   144,    88,    89,    90,    91,    92,    93,    94,    95,
      96,   163,    98,     0,    99,   100,     0,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,     0,   111,   112,
       0,   113,     0,     0,   114,   115,   116,   117,   118,   119,
     120,   121,     0,   122,     0,   124,     0,     0,   125,   126,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    15,
      16,     0,     0,     0,     0,     0,    17,   128,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    18,   130,    19,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    30,
      31,    32,     0,     0,     0,     0,     0,     0,   131,   132,
     133,   134,   135,   136,    41,   137,   138,   139,   140,   141,
      47,    48,     0,     0,     0,     0,   142,    20,    21,    22,
       0,     0,   143,     0,     0,     0,    23,    24,     0,     0,
       0,   144,     0,     0,    25,     0,    26,     0,     0,     9,
      10,     0,     0,     0,    27,    28,     0,    29,    88,    89,
      90,    91,    92,     0,     0,    30,    31,    32,    33,    34,
      35,    36,    37,    38,     0,     0,     0,    39,     0,    40,
      41,    42,    43,    44,    45,    46,    47,    48,     0,     0,
     114,   115,   116,   117,   118,   119,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    30,    31,    32,     0,     0,
       0,     0,     0,     0,   338,   132,   133,   134,   135,    40,
      41,    42,    43,    44,    45,    46,    47,    48,     0,     0,
       0,     0,   142
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
         7,    70,    14,   159,    69,    72,    26,   143,   223,     0,
      73,    74,    75,     5,    77,   315,     3,   175,   176,   155,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,   270,    24,    25,    85,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,   416,    38,    39,     5,    41,
      85,     7,    44,    45,    46,    47,    48,    49,    50,    51,
      41,    53,    54,    55,     3,     7,    58,    59,   146,   147,
     143,    10,    76,   124,     6,    86,    87,    81,     3,    71,
      87,   320,   155,     4,    41,    77,   143,    79,   123,    10,
     157,     6,   463,   143,   143,     6,    88,   253,   155,     6,
      21,    86,    87,     3,   154,   154,     6,    99,   100,   101,
     325,    92,    93,     6,   177,   135,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     288,   289,   290,   433,   126,    92,    93,    23,   149,     3,
     132,     6,   153,   154,    83,    84,    67,    68,    69,   141,
      71,    72,    73,    74,    75,    42,    43,    78,    86,    87,
     229,    85,    83,    84,   149,     3,    52,   230,   153,   154,
       3,   156,   235,   236,   237,   238,   239,   240,   241,     5,
     243,   244,   245,   261,     6,   263,   249,   250,   251,    83,
      84,    17,    18,    19,    37,    38,    39,    40,   122,    17,
      18,    19,    88,    89,    90,   146,   147,   148,   151,   152,
     279,    97,   149,   150,    70,    41,   281,    56,    57,   282,
      11,   149,    11,    41,    50,   153,   154,    53,   156,   291,
     292,    11,    50,   293,   294,    53,    11,    11,   126,   246,
     135,   248,   127,     6,     6,   252,     6,   254,   255,   256,
     257,   258,   259,   316,   317,   318,     6,     6,   321,     6,
     323,     6,    88,     6,     6,     5,   145,     6,     6,   425,
      88,     6,     6,   280,    60,     6,     6,     6,    64,    65,
      66,    21,     6,     6,     6,     6,   298,   113,     6,   115,
     116,   117,   118,   119,     6,   113,     6,   115,   116,   117,
     118,   119,     6,     6,     6,     6,   132,     6,    94,    95,
      96,     6,    98,     6,   132,   141,    21,     6,     6,   382,
     383,     6,     6,   141,     6,     4,     6,    67,    68,    69,
      11,    71,    72,    73,    74,    75,    11,     5,     4,     6,
       5,   144,   128,   129,   130,   131,   132,   133,   134,   143,
       6,     6,     6,     4,     6,   141,     5,     4,     6,   422,
     423,     6,    67,    68,    69,   136,    71,    72,    73,    74,
      75,   125,   137,   139,   138,   142,     5,     9,     6,     6,
     443,   444,     6,    81,     6,     6,     5,   456,     6,     6,
       6,     6,     6,     6,   457,   458,   459,   460,     6,   462,
       6,     6,     6,     6,     6,     6,     4,   140,    82,     9,
       6,     6,     6,     6,     6,     6,     6,   424,    81,   426,
     427,   428,   429,   430,   431,     5,     9,   155,   155,   492,
     493,   492,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,   298,    24,    25,    25,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,   284,    38,    39,
      14,    41,    51,   443,    44,    45,    46,    47,    48,    49,
      50,    51,   281,    53,    54,    55,   380,   229,    58,    59,
     420,   419,    14,   385,   295,   421,   384,   388,   296,    -1,
     297,    71,    -1,    -1,    -1,    -1,    -1,    77,    -1,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,
      -1,    -1,   132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   141,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    -1,    24,    25,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    -1,    38,    39,
      -1,    41,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    -1,    53,    54,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    71,    -1,    -1,    -1,    -1,    -1,    77,    -1,    79,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,
      -1,    -1,   132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   141,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    -1,    24,    25,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    -1,    38,    39,
      -1,    41,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    -1,    53,    -1,    55,    -1,    -1,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,    -1,    -1,    -1,    -1,    -1,    10,    77,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    21,    88,    23,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
     100,   101,    -1,    -1,    -1,    -1,    -1,    -1,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,    -1,    -1,    -1,    -1,   126,    61,    62,    63,
      -1,    -1,   132,    -1,    -1,    -1,    70,    71,    -1,    -1,
      -1,   141,    -1,    -1,    78,    -1,    80,    -1,    -1,    83,
      84,    -1,    -1,    -1,    88,    89,    -1,    91,    12,    13,
      14,    15,    16,    -1,    -1,    99,   100,   101,   102,   103,
     104,   105,   106,   107,    -1,    -1,    -1,   111,    -1,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,    -1,
      44,    45,    46,    47,    48,    49,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    99,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,    -1,    -1,
      -1,    -1,   126
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   158,   159,     0,    76,    81,   160,     7,   161,    83,
      84,   232,     7,     3,   162,     3,     4,    10,    21,    23,
      61,    62,    63,    70,    71,    78,    80,    88,    89,    91,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   111,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   163,
     169,   170,   171,   176,   177,   178,   186,   188,   203,   224,
     225,   232,   189,    23,    52,    88,    89,    90,    97,   187,
       6,     3,     6,     6,     6,     6,     3,     6,   177,     3,
       6,     3,     3,   171,    10,   186,   188,     6,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    24,
      25,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    38,    39,    41,    44,    45,    46,    47,    48,    49,
      50,    51,    53,    54,    55,    58,    59,    71,    77,    79,
      88,   108,   109,   110,   111,   112,   113,   115,   116,   117,
     118,   119,   126,   132,   141,   190,   191,   193,   194,   199,
     200,   202,   208,   222,   223,   224,   226,    11,    11,    11,
     179,    11,    11,    21,   192,   193,   208,    26,   135,   180,
     181,   218,   218,    86,    87,   149,   153,   154,   233,   234,
     235,   236,   237,   238,   239,   240,   240,   240,   172,   240,
     164,   127,   209,    60,    64,    65,    66,    94,    95,    96,
      98,   128,   129,   130,   131,   132,   133,   134,   141,   182,
     185,   210,   211,   212,   213,   214,   215,   216,   217,    41,
      92,    93,   183,   184,   232,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       4,     5,   190,     5,   190,   218,   196,   197,   240,   209,
      11,    37,    38,    39,    40,   227,    42,    43,   229,     6,
      11,     5,     6,     4,     5,   234,   234,   240,   146,   147,
     148,   149,   150,   151,   152,   145,   144,   143,   173,    17,
      18,    19,    41,    50,    53,    88,   113,   115,   116,   117,
     118,   119,   132,   141,   165,   166,     6,     6,     6,     4,
       5,     6,     6,     6,     4,     5,   183,    85,   122,   204,
      85,   123,   205,    85,   124,   206,   125,   207,   108,   199,
     208,   240,   136,   220,   137,   219,   138,   221,   139,   240,
     240,   240,   240,   240,   240,   240,   142,   228,   240,   195,
     201,   240,   198,   240,   232,    81,   232,   240,   240,   240,
     232,   209,   232,   232,   232,   232,   232,   232,   190,   190,
       5,   182,     6,     6,     6,     9,     6,     6,     5,   208,
     232,   192,   240,   180,   155,   234,   234,   234,   235,   235,
     236,   236,   237,   238,   239,     4,    21,    67,    68,    69,
      71,    72,    73,    74,    75,   174,   175,   176,   186,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     4,     5,   165,   240,   240,   240,   182,   240,
      82,   240,   183,     9,   154,   196,   240,   240,   228,   227,
     140,   230,    56,    57,   231,   229,     6,     6,     6,     6,
       6,     6,     6,     5,   174,   204,   205,   206,   240,   167,
     168,   240,   232,   209,   232,   232,   232,   232,   232,   232,
     165,   195,   156,   240,   208,   240,   240,   240,   240,    81,
     240,   174,     9,   154,   155,   155,   167,   156,   240,   155,
     155
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
     405,   406,   407,   408,   409,   410,   411
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   157,   158,   159,   159,   160,   161,   160,   162,   162,
     162,   162,   162,   162,   162,   162,   164,   163,   165,   165,
     165,   166,   166,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   167,   167,   168,
     168,   168,   169,   169,   169,   169,   169,   169,   169,   170,
     170,   172,   171,   173,   173,   173,   173,   174,   174,   174,
     175,   175,   175,   175,   175,   175,   175,   175,   175,   176,
     176,   177,   177,   177,   178,   179,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   180,   180,   181,   181,   182,
     182,   183,   183,   183,   184,   184,   184,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   187,   186,   186,   189,   188,   190,   190,
     190,   190,   190,   190,   191,   191,   191,   191,   191,   192,
     192,   193,   193,   193,   193,   193,   194,   194,   195,   195,
     196,   196,   197,   198,   199,   199,   199,   199,   199,   199,
     199,   199,   199,   199,   199,   199,   199,   199,   199,   199,
     200,   200,   201,   201,   201,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   203,
     203,   203,   203,   203,   203,   203,   203,   204,   204,   205,
     205,   206,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   222,
     222,   222,   222,   222,   222,   222,   222,   223,   223,   224,
     224,   224,   224,   224,   224,   224,   224,   224,   225,   225,
     225,   225,   226,   226,   226,   227,   227,   227,   227,   227,
     228,   229,   229,   229,   230,   231,   231,   232,   232,   233,
     233,   233,   234,   234,   234,   235,   235,   235,   235,   236,
     236,   236,   237,   237,   237,   238,   238,   239,   239,   240,
     240
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
       3,     3,     1,     1,     3,     3,     3,     3,     3,     2,
       1,     0,     2,     2,     4,     0,     5,     4,     4,     4,
       3,     4,     4,     4,     4,     1,     3,     1,     3,     1,
       3,     1,     3,     2,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     3,     1,     1,
       1,     3,     1,     0,     5,     1,     0,     4,     1,     1,
       3,     3,     2,     2,     3,     3,     3,     3,     3,     1,
       3,     1,     1,     1,     1,     1,     3,     3,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     4,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       1,     3,     3,     3,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     1,     1,     3,     3,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     3,     1,     3,
       1,     3,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     2,     2,     1,     3,     3,     3,     1,
       3,     3,     1,     3,     3,     1,     3,     1,     3,     1,
       3
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
  "DICE", "PARENT_ID", "ID_CODE", "EXT_ID_CODE", "BYPASS_IDCODE_CHECK",
  "A_HWROT", "S_HWROT", "UNIQUE_ID", "PARENT_UNIQUE_ID", "FUNCTION_ID",
  "IMAGE", "ID", "NAME", "DELAY_HANDOFF", "DELAY_LOAD", "COPY", "INCLUDE",
  "DELAY_AUTH", "PARTITION", "PFILE", "METAHEADER", "WORD", "HEXWORD",
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
       158,     0,    -1,   159,    -1,    -1,   159,   160,    -1,    76,
       7,   232,    -1,    -1,    81,   161,     7,     3,   162,     4,
      -1,    -1,   162,   186,    -1,   162,   178,    -1,   162,   170,
      -1,   162,   163,    -1,   162,   169,    -1,   162,   188,    -1,
     162,   176,    -1,    -1,    80,     3,   164,   165,     4,    -1,
     166,    -1,   166,     5,   165,    -1,   166,   165,    -1,    -1,
      18,     6,   205,    -1,    88,     6,   209,    -1,   113,     6,
     232,    -1,    17,     6,   204,    -1,   115,     6,   232,    -1,
     116,     6,   232,    -1,   117,     6,   232,    -1,   118,     6,
     232,    -1,   119,     6,   232,    -1,    53,     6,   232,    -1,
      41,     6,   240,    -1,    19,     6,   206,    -1,   141,    -1,
      50,     6,   167,    -1,   132,    -1,   168,    -1,   168,     9,
     167,    -1,   240,    -1,   240,   154,   240,   155,    -1,   240,
     154,   156,   155,    -1,    71,     6,   240,    -1,    61,     6,
     240,    -1,    62,     6,   240,    -1,    63,     6,   240,    -1,
     203,     6,   232,    -1,    88,     6,   209,    -1,    21,     6,
     208,    -1,   171,    -1,   170,   171,    -1,    -1,    70,     3,
     172,   173,     4,    -1,    -1,   173,   174,    -1,   173,   186,
      -1,   173,   176,    -1,   175,    -1,   175,     5,   174,    -1,
     175,   174,    -1,    71,     6,   240,    -1,    72,     6,    81,
      -1,    73,    -1,    74,    -1,    75,     6,   240,    -1,    21,
       6,   208,    -1,    67,     6,   240,    -1,    68,     6,   240,
      -1,    69,     6,   240,    -1,    78,   177,    -1,   177,    -1,
      -1,   177,   186,    -1,   177,   188,    -1,    10,    88,    11,
     209,    -1,    -1,    10,    89,   179,    11,   182,    -1,    89,
       3,   182,     4,    -1,    23,     3,   180,     4,    -1,    10,
      23,    11,   218,    -1,    23,     6,   218,    -1,    10,    90,
      11,   227,    -1,    10,    97,    11,   229,    -1,    10,    52,
      11,   196,    -1,    91,     3,   183,     4,    -1,   181,    -1,
     181,     5,   180,    -1,   218,    -1,    26,     6,   240,    -1,
     185,    -1,   185,     5,   182,    -1,   184,    -1,   184,     5,
     183,    -1,   184,   183,    -1,    41,     6,   240,    -1,    92,
       6,    82,    -1,    93,     6,   240,    -1,   210,    -1,   211,
      -1,   213,    -1,   214,    -1,   215,    -1,   217,    -1,   216,
      -1,    94,    -1,    96,     6,   240,    -1,   212,    -1,    98,
       6,   240,    -1,    64,    -1,    65,    -1,    66,    -1,    95,
       6,   240,    -1,    60,    -1,    -1,    10,   187,   192,    11,
     232,    -1,   232,    -1,    -1,     3,   189,   190,     4,    -1,
     193,    -1,   191,    -1,   191,     5,   190,    -1,   193,     5,
     190,    -1,   191,   190,    -1,   193,   190,    -1,    79,     6,
     232,    -1,    71,     6,   240,    -1,    21,     6,   199,    -1,
      21,     6,   108,    -1,    54,     6,    81,    -1,   193,    -1,
     193,     5,   192,    -1,   199,    -1,   202,    -1,   222,    -1,
     223,    -1,   194,    -1,    50,     6,   195,    -1,    51,     6,
     198,    -1,   201,    -1,   201,     9,   195,    -1,   197,    -1,
     197,     5,   196,    -1,   240,    -1,   240,    -1,    12,    -1,
      44,    -1,    45,    -1,    46,    -1,    47,    -1,    48,    -1,
      49,    -1,    14,    -1,    15,    -1,    13,    -1,   111,    -1,
     109,    -1,   224,    -1,   226,    -1,   208,    -1,    16,    -1,
      28,    -1,    28,     6,   139,    -1,   240,    -1,   240,   154,
     240,   155,    -1,   240,   154,   156,   155,    -1,    17,     6,
     204,    -1,    18,     6,   205,    -1,    19,     6,   206,    -1,
      20,     6,   207,    -1,    25,     6,   219,    -1,    24,     6,
     220,    -1,    27,     6,   221,    -1,   113,     6,   232,    -1,
     115,     6,   232,    -1,   116,     6,   232,    -1,   117,     6,
     232,    -1,   118,     6,   232,    -1,    39,     6,   228,    -1,
      38,     6,   240,    -1,   119,     6,   232,    -1,   200,    -1,
      21,     6,   208,    -1,    88,     6,   209,    -1,    41,     6,
     240,    -1,   141,    -1,    58,     6,   240,    -1,    59,     6,
     240,    -1,   132,    -1,    77,    -1,   111,    -1,   224,    -1,
     102,    -1,   103,    -1,   104,    -1,   105,    -1,   106,    -1,
     107,    -1,    85,    -1,   122,    -1,    85,    -1,   123,    -1,
      85,    -1,   124,    -1,   125,    -1,   126,    -1,   127,    -1,
     128,    -1,   129,    -1,   141,    -1,   130,    -1,   131,    -1,
     132,    -1,   133,    -1,   134,    -1,   135,    -1,   137,    -1,
     136,    -1,   138,    -1,    29,     6,   240,    -1,    30,     6,
     240,    -1,    31,     6,   240,    -1,    32,     6,   240,    -1,
      33,     6,   240,    -1,    34,     6,   240,    -1,    35,    -1,
      36,    -1,    22,     6,   240,    -1,    53,     6,   232,    -1,
      55,     6,   232,    -1,   113,    -1,   225,    -1,   119,    -1,
     120,    -1,   121,    -1,   100,    -1,    99,    -1,   101,    -1,
     114,    -1,   115,    -1,   116,    -1,   117,    -1,   118,    -1,
     112,    -1,   108,    -1,   110,    -1,    37,     6,   240,    -1,
      39,     6,   228,    -1,    38,     6,   240,    -1,    40,    -1,
     227,     9,   227,    -1,   142,    -1,    42,     6,   230,    -1,
      43,     6,   231,    -1,   229,     5,   229,    -1,   140,    -1,
      56,    -1,    57,    -1,    83,    -1,    84,    -1,    87,    -1,
      86,    -1,   154,   240,   155,    -1,   233,    -1,   149,   234,
      -1,   153,   234,    -1,   234,    -1,   235,   146,   234,    -1,
     235,   147,   234,    -1,   235,   148,   234,    -1,   235,    -1,
     236,   149,   235,    -1,   236,   150,   235,    -1,   236,    -1,
     237,   151,   236,    -1,   237,   152,   236,    -1,   237,    -1,
     238,   145,   237,    -1,   238,    -1,   239,   144,   238,    -1,
     239,    -1,   240,   143,   239,    -1
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
     228,   230,   231,   234,   237,   242,   243,   249,   254,   259,
     264,   268,   273,   278,   283,   288,   290,   294,   296,   300,
     302,   306,   308,   312,   315,   319,   323,   327,   329,   331,
     333,   335,   337,   339,   341,   343,   347,   349,   353,   355,
     357,   359,   363,   365,   366,   372,   374,   375,   380,   382,
     384,   388,   392,   395,   398,   402,   406,   410,   414,   418,
     420,   424,   426,   428,   430,   432,   434,   438,   442,   444,
     448,   450,   454,   456,   458,   460,   462,   464,   466,   468,
     470,   472,   474,   476,   478,   480,   482,   484,   486,   488,
     490,   492,   496,   498,   503,   508,   512,   516,   520,   524,
     528,   532,   536,   540,   544,   548,   552,   556,   560,   564,
     568,   570,   574,   578,   582,   584,   588,   592,   594,   596,
     598,   600,   602,   604,   606,   608,   610,   612,   614,   616,
     618,   620,   622,   624,   626,   628,   630,   632,   634,   636,
     638,   640,   642,   644,   646,   648,   650,   652,   654,   658,
     662,   666,   670,   674,   678,   680,   682,   686,   690,   694,
     696,   698,   700,   702,   704,   706,   708,   710,   712,   714,
     716,   718,   720,   722,   724,   726,   730,   734,   738,   740,
     744,   746,   750,   754,   758,   760,   762,   764,   766,   768,
     770,   772,   776,   778,   781,   784,   786,   790,   794,   798,
     800,   804,   808,   810,   814,   818,   820,   824,   826,   830,
     832
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   196,   196,   198,   199,   202,   203,   203,   209,   210,
     211,   212,   213,   214,   215,   216,   219,   219,   224,   225,
     226,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   244,   248,   249,   252,
     254,   256,   260,   261,   262,   263,   264,   265,   266,   269,
     270,   273,   273,   277,   278,   279,   280,   283,   284,   285,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   299,
     300,   303,   304,   305,   308,   311,   311,   314,   315,   316,
     321,   322,   323,   324,   325,   328,   329,   332,   333,   336,
     337,   340,   341,   342,   345,   346,   347,   350,   353,   355,
     357,   359,   361,   363,   365,   367,   369,   375,   379,   380,
     381,   382,   387,   394,   394,   401,   407,   407,   413,   414,
     415,   416,   417,   418,   421,   424,   425,   426,   427,   432,
     433,   436,   437,   438,   439,   440,   443,   444,   447,   448,
     451,   452,   455,   460,   462,   463,   464,   465,   466,   467,
     468,   469,   470,   471,   474,   475,   476,   477,   478,   479,
     482,   483,   486,   487,   488,   490,   491,   492,   493,   494,
     495,   496,   497,   498,   499,   500,   501,   502,   503,   504,
     505,   506,   507,   508,   509,   510,   511,   512,   513,   516,
     517,   518,   519,   520,   521,   522,   523,   526,   527,   530,
     531,   534,   535,   538,   541,   544,   547,   550,   553,   556,
     559,   562,   565,   568,   571,   574,   577,   580,   583,   584,
     585,   586,   587,   588,   589,   590,   591,   594,   595,   598,
     599,   600,   601,   602,   603,   604,   605,   606,   609,   610,
     611,   612,   615,   616,   617,   620,   623,   626,   629,   632,
     635,   638,   641,   642,   645,   648,   651,   657,   658,   661,
     662,   663,   669,   670,   671,   675,   676,   677,   678,   681,
     682,   683,   686,   687,   688,   691,   692,   695,   696,   700,
     701
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
     155,   156
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 952;
  const int BisonParser::yynnts_ = 84;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 157;

  const unsigned int BisonParser::yyuser_token_number_max_ = 411;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 23 "../s/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 2465 "../bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 706 "../s/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
