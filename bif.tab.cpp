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
    { currentBifOptions = new BifOptions(options.GetArchType(),(yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 204 "../s/bif.y"
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
    break;

  case 15:
/* Line 670 of lalr1.cc  */
#line 218 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 21:
/* Line 670 of lalr1.cc  */
#line 229 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 230 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 231 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.encrKeyFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 232 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 233 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 234 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 235 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 236 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 237 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 238 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 239 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.revokeId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 240 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 241 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 243 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 38:
/* Line 670 of lalr1.cc  */
#line 251 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 39:
/* Line 670 of lalr1.cc  */
#line 253 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 255 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 259 "../s/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 260 "../s/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 261 "../s/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 262 "../s/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 263 "../s/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 264 "../s/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 271 "../s/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 272 "../s/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions); }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 286 "../s/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 59:
/* Line 670 of lalr1.cc  */
#line 287 "../s/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 288 "../s/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 289 "../s/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 290 "../s/bif.y"
    { currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 291 "../s/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 292 "../s/bif.y"
    { currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 293 "../s/bif.y"
    { currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 294 "../s/bif.y"
    { currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 72:
/* Line 670 of lalr1.cc  */
#line 307 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported in VERSAL architecture.\n\t   Please see 'bootgen -arch versal -bif_help keysrc'");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 310 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 77:
/* Line 670 of lalr1.cc  */
#line 315 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                    if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); 
                                                                                    currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 78:
/* Line 670 of lalr1.cc  */
#line 320 "../s/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); }
    break;

  case 85:
/* Line 670 of lalr1.cc  */
#line 331 "../s/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 86:
/* Line 670 of lalr1.cc  */
#line 332 "../s/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 92:
/* Line 670 of lalr1.cc  */
#line 344 "../s/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 93:
/* Line 670 of lalr1.cc  */
#line 345 "../s/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 94:
/* Line 670 of lalr1.cc  */
#line 346 "../s/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 95:
/* Line 670 of lalr1.cc  */
#line 349 "../s/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 96:
/* Line 670 of lalr1.cc  */
#line 352 "../s/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 97:
/* Line 670 of lalr1.cc  */
#line 354 "../s/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 98:
/* Line 670 of lalr1.cc  */
#line 356 "../s/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 99:
/* Line 670 of lalr1.cc  */
#line 358 "../s/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 360 "../s/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 362 "../s/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 364 "../s/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 103:
/* Line 670 of lalr1.cc  */
#line 366 "../s/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 368 "../s/bif.y"
    {  if(options.GetArchType() != Arch::VERSAL)
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t\t'dpacm_enable' is supported only in VERSAL architecture");
                                                                                   if(options.GetArchType() == Arch::VERSAL)
                                                                                     LOG_WARNING("boot_config { dpacm_enable } will be deprecated. 'dpacm_enable' should be specified along with the partition. \n            See 'bootgen -bif_help dpacm_enable' for more info.");
                                                                                   currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));
                                                                                }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 374 "../s/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->smapWidth = (yysemantic_stack_[(3) - (3)].number);
                                                                                }
    break;

  case 106:
/* Line 670 of lalr1.cc  */
#line 378 "../s/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 107:
/* Line 670 of lalr1.cc  */
#line 379 "../s/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 108:
/* Line 670 of lalr1.cc  */
#line 380 "../s/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 383 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 110:
/* Line 670 of lalr1.cc  */
#line 386 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 390 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 396 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 120:
/* Line 670 of lalr1.cc  */
#line 410 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 413 "../s/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 415 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 416 "../s/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 138:
/* Line 670 of lalr1.cc  */
#line 444 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] only supported in ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 449 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 451 "../s/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 452 "../s/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 453 "../s/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 454 "../s/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 455 "../s/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 456 "../s/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 457 "../s/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 458 "../s/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 459 "../s/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 460 "../s/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 150:
/* Line 670 of lalr1.cc  */
#line 463 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 151:
/* Line 670 of lalr1.cc  */
#line 464 "../s/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 152:
/* Line 670 of lalr1.cc  */
#line 465 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 153:
/* Line 670 of lalr1.cc  */
#line 466 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 467 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 470 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 471 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 157:
/* Line 670 of lalr1.cc  */
#line 474 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 475 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 476 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 478 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 161:
/* Line 670 of lalr1.cc  */
#line 479 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 480 "../s/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 481 "../s/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 482 "../s/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 483 "../s/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 484 "../s/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 485 "../s/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 486 "../s/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 487 "../s/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 488 "../s/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 489 "../s/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 490 "../s/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 491 "../s/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 492 "../s/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 494 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 495 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 496 "../s/bif.y"
    { currentPartitionBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 497 "../s/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 498 "../s/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 499 "../s/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 512 "../s/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 192:
/* Line 670 of lalr1.cc  */
#line 516 "../s/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 194:
/* Line 670 of lalr1.cc  */
#line 520 "../s/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 211:
/* Line 670 of lalr1.cc  */
#line 569 "../s/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 212:
/* Line 670 of lalr1.cc  */
#line 570 "../s/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 213:
/* Line 670 of lalr1.cc  */
#line 571 "../s/bif.y"
    { currentPartitionBifOptions->reserve = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 214:
/* Line 670 of lalr1.cc  */
#line 572 "../s/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 215:
/* Line 670 of lalr1.cc  */
#line 573 "../s/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 216:
/* Line 670 of lalr1.cc  */
#line 574 "../s/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 217:
/* Line 670 of lalr1.cc  */
#line 575 "../s/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 218:
/* Line 670 of lalr1.cc  */
#line 576 "../s/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 219:
/* Line 670 of lalr1.cc  */
#line 579 "../s/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 220:
/* Line 670 of lalr1.cc  */
#line 580 "../s/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 237:
/* Line 670 of lalr1.cc  */
#line 605 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 238:
/* Line 670 of lalr1.cc  */
#line 608 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 239:
/* Line 670 of lalr1.cc  */
#line 611 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 240:
/* Line 670 of lalr1.cc  */
#line 614 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 243:
/* Line 670 of lalr1.cc  */
#line 623 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 247:
/* Line 670 of lalr1.cc  */
#line 633 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 248:
/* Line 670 of lalr1.cc  */
#line 636 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 253:
/* Line 670 of lalr1.cc  */
#line 648 "../s/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 255:
/* Line 670 of lalr1.cc  */
#line 655 "../s/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 256:
/* Line 670 of lalr1.cc  */
#line 656 "../s/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 258:
/* Line 670 of lalr1.cc  */
#line 661 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 259:
/* Line 670 of lalr1.cc  */
#line 662 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 260:
/* Line 670 of lalr1.cc  */
#line 663 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 262:
/* Line 670 of lalr1.cc  */
#line 667 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 263:
/* Line 670 of lalr1.cc  */
#line 668 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 265:
/* Line 670 of lalr1.cc  */
#line 672 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 266:
/* Line 670 of lalr1.cc  */
#line 673 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 268:
/* Line 670 of lalr1.cc  */
#line 677 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 270:
/* Line 670 of lalr1.cc  */
#line 681 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 272:
/* Line 670 of lalr1.cc  */
#line 686 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1369 "../bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -362;
  const short int
  BisonParser::yypact_[] =
  {
      -362,    11,   -59,  -362,  -362,  -362,    61,    69,  -362,   705,
    -362,  -362,   123,    14,    73,    77,    84,    72,    94,  -362,
      89,  -362,  -362,   104,   151,   158,  -362,  -362,  -362,  -362,
    -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,
    -362,  -362,  -362,  -362,  -362,  -362,  -362,    99,  -362,  -362,
      64,  -362,  -362,  -362,   165,  -362,  -362,  -362,   519,   182,
     183,   190,  -362,   191,   193,   642,   -17,    80,    86,    86,
      86,  -362,    86,    64,  -362,    56,   732,   -16,  -362,  -362,
    -362,  -362,    19,  -362,  -362,  -362,  -362,   205,   206,   208,
     209,   210,   212,   214,   215,   223,   224,   225,   230,   231,
     233,   236,  -362,  -362,   242,   243,   246,  -362,  -362,  -362,
    -362,  -362,  -362,   248,   249,   250,   252,   253,   260,   261,
     265,   266,  -362,  -362,  -362,  -362,  -362,   267,   269,   270,
     279,   280,   281,  -362,  -362,  -362,   247,    13,   396,  -362,
    -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,    80,    86,
      56,   259,   188,    62,   286,   282,   289,   293,  -362,   291,
     295,  -362,  -362,  -362,  -362,    86,    86,    86,  -362,  -362,
      58,   -30,   -15,   163,   166,   168,   168,   168,  -362,   168,
     171,  -362,  -362,  -362,  -362,  -362,  -362,   296,   300,  -362,
    -362,  -362,  -362,  -362,  -362,  -362,  -362,   303,   306,  -362,
    -362,  -362,  -362,  -362,  -362,  -362,  -362,   311,   318,   319,
     322,    48,  -362,   -64,   -31,   -44,   221,   276,    86,   201,
     213,   222,   220,    86,    86,    86,    86,    86,    86,   219,
      86,    86,    86,    19,   284,    19,    86,    86,    19,    56,
      19,    19,    19,    19,    19,    19,  -362,   519,  -362,   519,
    -362,  -362,  -362,   335,   168,  -362,   732,   339,   349,   350,
    -362,   351,   355,   356,   358,   256,    19,   642,    86,  -362,
     -17,  -362,  -362,  -124,    86,    86,    86,    86,    86,    86,
      86,    86,    86,    86,   264,   359,   360,   361,   385,   386,
     387,   388,   390,   391,   392,   393,   394,   397,  -362,  -362,
     398,   135,    86,    86,  -362,   732,    86,   288,    86,  -362,
     -16,  -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,
    -362,  -362,  -362,  -362,  -362,  -362,   168,  -362,  -362,  -362,
    -362,  -362,  -362,  -362,   168,   168,   168,   168,   168,   168,
    -362,  -362,   168,  -362,   395,   -43,  -362,   168,  -362,  -362,
    -362,   168,   168,  -362,  -362,  -362,  -362,  -362,  -362,  -362,
    -362,  -362,  -362,    86,  -362,    86,    86,   219,   188,   272,
      82,    62,  -362,  -362,  -362,   168,  -362,  -362,  -362,  -362,
    -362,    58,    58,   -30,   -30,   -15,   163,   166,  -362,   412,
     415,   425,   428,   431,   440,  -362,  -362,   444,  -362,   245,
    -362,  -362,   -64,   -31,   -44,    86,    86,    19,    56,    19,
      19,    19,    19,    19,    19,  -362,   171,  -362,   168,   168,
    -362,   168,  -362,   168,  -362,    86,    -3,  -362,   168,   168,
    -362,   351,  -362,  -362,  -362,  -362,  -362,   358,   256,    86,
      86,    86,    86,   376,    86,   416,  -362,  -362,  -362,  -362,
     168,  -362,   445,   -38,  -362,  -362,  -362,  -362,  -362,  -362,
    -362,  -362,  -362,  -362,   305,   -60,  -362,   168,   168,   168,
     168,  -362,   168,  -362,    86,   115,  -362,  -362,  -362,   307,
     -47,  -362,  -362
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     5,     4,     0,     0,     7,     0,
     112,     6,   109,     0,     0,     0,     0,     0,     0,    69,
       0,   249,   250,     0,     0,     0,   227,   226,   228,   184,
     185,   186,   187,   188,   189,   182,   221,   229,   230,   231,
     232,   233,   223,   224,   225,    11,    12,    10,    47,    14,
      68,     9,     8,    13,     0,   183,   222,   111,     0,     0,
       0,     0,    73,     0,     0,     0,     0,     0,     0,     0,
       0,    49,     0,    67,    15,     0,     0,     0,    48,   109,
      70,    71,     0,   140,   149,   147,   148,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   155,     0,     0,     0,
       0,     0,   216,   217,     0,     0,     0,   141,   142,   143,
     144,   145,   146,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   235,   151,   236,   150,   234,   221,   230,   231,
     232,   233,   223,   197,   181,   179,     0,   115,   114,   131,
     127,   175,   128,   154,   129,   130,   152,   153,     0,     0,
       0,     0,     0,     0,     0,     0,   125,     0,   207,     0,
      83,    85,    78,   252,   251,     0,     0,     0,   254,   257,
     261,   264,   267,   269,   271,    42,    43,    44,    51,    41,
      20,   198,    46,   106,   107,   108,   102,     0,     0,   199,
     200,   202,   203,   204,   205,   206,   201,     0,    87,    95,
      96,   104,    97,    98,    99,   101,   100,     0,     0,     0,
       0,    89,    45,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   113,     0,   118,     0,
     119,    77,    81,   136,   138,    72,     0,     0,     0,     0,
     240,    79,     0,     0,    80,     0,     0,     0,     0,    76,
       0,   255,   256,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    69,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    35,    33,
       0,    17,     0,     0,    75,     0,     0,     0,     0,    82,
       0,    91,   190,   191,   160,   192,   193,   161,   194,   195,
     162,   196,   163,   123,   122,   154,   218,   209,   165,   208,
     164,   210,   166,   156,   211,   212,   213,   214,   215,   173,
     242,   172,   178,   132,   134,   157,   133,   139,   219,   124,
     220,   180,   121,   120,   177,   167,   168,   169,   170,   171,
     174,   116,   117,     0,    74,     0,     0,     0,     0,     0,
       0,     0,   176,   110,   126,    86,    84,   253,   258,   259,
     260,   262,   263,   265,   266,   268,   270,   272,    50,     0,
       0,     0,     0,     0,     0,    60,    61,     0,    52,    55,
      54,    53,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    16,    20,    19,   103,   105,
      88,    92,    93,    94,    90,     0,     0,   137,   237,   239,
     238,   241,   246,   243,   247,   248,   244,   245,     0,     0,
       0,     0,     0,     0,     0,     0,    57,    24,    21,    32,
      31,    34,    36,    38,    30,    22,    23,    25,    26,    27,
      28,    29,    18,   135,     0,     0,    63,    64,    65,    66,
      58,    59,    62,    56,     0,     0,   159,   158,    37,     0,
       0,    40,    39
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -362,  -362,  -362,  -362,  -362,  -362,  -362,  -362,  -288,  -362,
     -18,  -362,  -362,  -362,   410,  -362,  -362,  -361,  -362,   174,
     441,  -362,  -362,   189,  -362,  -242,  -202,  -362,  -362,    -6,
    -362,   452,  -362,  -116,  -362,   195,   -58,  -362,    39,   102,
    -362,  -362,   251,  -362,  -362,  -362,  -362,    65,    63,    66,
    -362,  -211,  -145,  -362,  -362,  -362,  -362,  -362,  -362,  -362,
    -362,   -57,  -362,  -362,  -362,  -362,  -362,   462,  -362,  -362,
     105,   107,   101,  -362,  -362,   -63,  -362,   -84,  -129,   -95,
     194,   200,   211,   -68
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     5,     6,     9,    45,   180,   300,   301,
     451,   452,    46,    47,    48,   178,   284,   398,   399,    49,
      50,    51,   151,   159,   160,   197,   210,   211,   198,    80,
      65,    81,    58,   136,   137,   155,   138,   139,   343,   252,
     253,   346,   140,   141,   344,   142,    54,   314,   317,   320,
     322,   143,   182,   199,   200,   201,   202,   203,   204,   205,
     206,   161,   330,   328,   332,   144,   145,   146,    56,   147,
     261,   341,   264,   433,   436,    57,   168,   169,   170,   171,
     172,   173,   174,   254
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       175,   176,   177,    52,   179,   255,   325,   156,   157,   311,
     162,     3,   283,   417,   364,   312,     4,    66,   247,   212,
      67,   248,   250,   207,   377,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,   318,    93,    94,   446,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   315,   104,
     105,   313,   106,   310,   372,   107,   108,   109,   110,   111,
     112,   113,   114,   420,   115,   116,   117,    10,     7,   118,
     208,   209,     8,   319,    79,    71,   283,   163,   164,    68,
     119,   271,   272,    69,   473,   316,   120,   207,   477,   283,
      70,   251,    74,   283,   354,   121,    21,    22,   283,   273,
      72,   482,   262,   263,   426,    26,    27,    28,   424,   475,
      75,   158,   277,   278,   122,   123,   124,   125,   126,   127,
      37,   128,   129,   130,   131,   132,    43,    44,   462,   279,
     280,   361,   133,   362,   208,   209,   434,   435,   134,   165,
     416,    21,    22,   166,   167,    59,   464,   135,   381,   382,
     326,   285,   286,   287,    76,   334,   335,   336,   337,   338,
     339,    77,   342,   345,   347,    17,   163,   164,   351,   352,
     348,    82,   350,    60,   288,   353,   181,   355,   356,   357,
     358,   359,   360,   289,   383,   384,   290,   285,   286,   287,
     378,   379,   380,   148,   149,   163,   164,   274,   275,   276,
     375,   150,   152,   373,   153,    61,    62,    63,   158,   156,
     288,   213,   214,    64,   215,   216,   217,   291,   218,   289,
     219,   220,   290,   257,   258,   259,   260,   466,   165,   221,
     222,   223,   166,   167,   418,   419,   224,   225,   421,   226,
     423,   292,   227,   293,   294,   295,   296,   297,   228,   229,
     445,   246,   230,   291,   231,   232,   233,   165,   234,   235,
     298,   166,   167,   455,   479,   389,   236,   237,   388,   299,
     256,   238,   239,   240,    79,   241,   242,   292,   401,   293,
     294,   295,   296,   297,   389,   243,   244,   245,    83,    84,
      85,    86,   265,   266,   267,   269,   298,   428,   429,   268,
     270,   281,   302,   282,   283,   299,   303,   304,   390,   391,
     392,   305,   393,   394,   395,   396,   397,   306,   107,   108,
     109,   110,   111,   112,   307,   308,   309,   390,   391,   392,
     327,   393,   394,   395,   396,   397,    19,   450,   453,   321,
     363,    21,    22,   329,   454,   365,   456,   457,   458,   459,
     460,   461,   333,   331,   340,   366,   367,   345,   465,   349,
     368,   369,   370,   371,   422,   402,   403,   404,    26,    27,
      28,   467,   468,   469,   470,   133,   472,   323,   123,   124,
     125,   126,    36,    37,    38,    39,    40,    41,    42,    43,
      44,   405,   406,   407,   408,   133,   409,   410,   411,   412,
     413,   249,   415,   414,   425,   432,   453,   480,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,   438,    93,
      94,   439,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   440,   104,   105,   441,   106,   389,   442,   107,   108,
     109,   110,   111,   112,   113,   114,   443,   115,   116,   117,
     444,   471,   118,   476,   474,   481,   478,    78,   400,   376,
      73,    53,   374,   119,   463,   427,   448,   447,   324,   120,
     449,    55,   437,   431,   430,   385,     0,     0,   121,   390,
     391,   392,   386,   393,   394,   395,   396,   397,    26,    27,
      28,     0,     0,     0,   387,     0,     0,   122,   123,   124,
     125,   126,   127,    37,   128,   129,   130,   131,   132,    43,
      44,     0,     0,     0,     0,   133,     0,     0,     0,     0,
       0,   134,     0,     0,     0,     0,     0,     0,     0,     0,
     135,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,     0,    93,    94,     0,    95,    96,    97,    98,    99,
     100,   101,   102,   103,     0,   104,   105,     0,   106,     0,
       0,   107,   108,   109,   110,   111,   112,   113,   114,     0,
     115,   116,   117,     0,     0,   118,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   119,     0,     0,     0,
       0,     0,   120,     0,     0,     0,     0,     0,     0,     0,
       0,   121,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    26,    27,    28,     0,     0,     0,     0,     0,     0,
     122,   123,   124,   125,   126,   127,    37,   128,   129,   130,
     131,   132,    43,    44,     0,     0,     0,     0,   133,     0,
       0,     0,     0,     0,   134,     0,     0,     0,     0,     0,
       0,     0,     0,   135,    83,    84,    85,    86,    87,    88,
      89,    90,   154,    92,     0,    93,    94,     0,    95,    96,
      97,    98,    99,   100,   101,   102,   103,     0,   104,   105,
       0,   106,     0,     0,   107,   108,   109,   110,   111,   112,
     113,   114,     0,   115,     0,   117,     0,     0,   118,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    10,    11,
       0,     0,     0,     0,     0,    12,     0,     0,     0,     0,
       0,     0,     0,     0,   121,     0,     0,    13,     0,     0,
       0,     0,     0,     0,    26,    27,    28,     0,     0,     0,
       0,     0,     0,   122,   123,   124,   125,   126,   127,    37,
     128,   129,   130,   131,   132,    43,    44,     0,     0,     0,
       0,   133,    14,    15,    16,     0,     0,   134,     0,     0,
       0,    17,    18,     0,     0,     0,   135,    19,     0,    20,
       0,     0,    21,    22,     0,     0,     0,    23,    24,     0,
      25,     0,   183,   184,   185,     0,     0,    26,    27,    28,
      29,    30,    31,    32,    33,    34,     0,     0,     0,    35,
       0,    36,    37,    38,    39,    40,    41,    42,    43,    44,
     186,   187,     0,   188,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   189,   190,   191,   192,   193,   194,   195,
       0,     0,     0,     0,     0,     0,   196
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        68,    69,    70,     9,    72,   150,   217,    65,    25,   211,
      67,     0,   136,   301,   256,    79,    75,     3,     5,    82,
       6,   137,   138,    39,   148,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    79,    23,    24,   399,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    79,    36,
      37,   115,    39,     5,   265,    42,    43,    44,    45,    46,
      47,    48,    49,   305,    51,    52,    53,     3,     7,    56,
      86,    87,     3,   117,    10,     3,   136,    80,    81,     6,
      67,   165,   166,     6,   445,   116,    73,    39,   148,   136,
       6,   148,     3,   136,   239,    82,    77,    78,   136,   167,
       6,   148,    40,    41,   147,    92,    93,    94,   310,   147,
       6,   128,   142,   143,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   416,   144,
     145,   247,   119,   249,    86,    87,    54,    55,   125,   142,
       5,    77,    78,   146,   147,    22,   149,   134,   277,   278,
     218,    16,    17,    18,     3,   223,   224,   225,   226,   227,
     228,     3,   230,   231,   232,    66,    80,    81,   236,   237,
     233,     6,   235,    50,    39,   238,   120,   240,   241,   242,
     243,   244,   245,    48,   279,   280,    51,    16,    17,    18,
     274,   275,   276,    11,    11,    80,    81,   139,   140,   141,
     268,    11,    11,   266,    11,    82,    83,    84,   128,   267,
      39,     6,     6,    90,     6,     6,     6,    82,     6,    48,
       6,     6,    51,    35,    36,    37,    38,   438,   142,     6,
       6,     6,   146,   147,   302,   303,     6,     6,   306,     6,
     308,   106,     6,   108,   109,   110,   111,   112,     6,     6,
       5,     4,     6,    82,     6,     6,     6,   142,     6,     6,
     125,   146,   147,   408,   149,    20,     6,     6,     4,   134,
      11,     6,     6,     6,    10,     6,     6,   106,   284,   108,
     109,   110,   111,   112,    20,     6,     6,     6,    12,    13,
      14,    15,     6,    11,     5,     4,   125,   365,   366,     6,
       5,   138,     6,   137,   136,   134,     6,     4,    63,    64,
      65,     5,    67,    68,    69,    70,    71,     6,    42,    43,
      44,    45,    46,    47,     6,     6,     4,    63,    64,    65,
     129,    67,    68,    69,    70,    71,    72,   405,   406,   118,
       5,    77,    78,   130,   407,     6,   409,   410,   411,   412,
     413,   414,   132,   131,   135,     6,     6,   425,   426,    75,
       9,     6,     6,     5,    76,     6,     6,     6,    92,    93,
      94,   439,   440,   441,   442,   119,   444,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,     6,     6,     6,     6,   119,     6,     6,     6,     6,
       6,     5,     4,     6,     9,   133,   474,   475,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,     6,    23,
      24,     6,    26,    27,    28,    29,    30,    31,    32,    33,
      34,     6,    36,    37,     6,    39,    20,     6,    42,    43,
      44,    45,    46,    47,    48,    49,     6,    51,    52,    53,
       6,    75,    56,   148,     9,   148,   474,    47,   284,   270,
      19,     9,   267,    67,   425,   363,   403,   402,   217,    73,
     404,     9,   371,   368,   367,   281,    -1,    -1,    82,    63,
      64,    65,   282,    67,    68,    69,    70,    71,    92,    93,
      94,    -1,    -1,    -1,   283,    -1,    -1,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,    -1,    -1,    -1,    -1,   119,    -1,    -1,    -1,    -1,
      -1,   125,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     134,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    -1,    23,    24,    -1,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    -1,    36,    37,    -1,    39,    -1,
      -1,    42,    43,    44,    45,    46,    47,    48,    49,    -1,
      51,    52,    53,    -1,    -1,    56,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    67,    -1,    -1,    -1,
      -1,    -1,    73,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    82,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    92,    93,    94,    -1,    -1,    -1,    -1,    -1,    -1,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,    -1,    -1,    -1,    -1,   119,    -1,
      -1,    -1,    -1,    -1,   125,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   134,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    -1,    23,    24,    -1,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    -1,    36,    37,
      -1,    39,    -1,    -1,    42,    43,    44,    45,    46,    47,
      48,    49,    -1,    51,    -1,    53,    -1,    -1,    56,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
      -1,    -1,    -1,    -1,    -1,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    22,    -1,    -1,
      -1,    -1,    -1,    -1,    92,    93,    94,    -1,    -1,    -1,
      -1,    -1,    -1,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,    -1,    -1,    -1,
      -1,   119,    57,    58,    59,    -1,    -1,   125,    -1,    -1,
      -1,    66,    67,    -1,    -1,    -1,   134,    72,    -1,    74,
      -1,    -1,    77,    78,    -1,    -1,    -1,    82,    83,    -1,
      85,    -1,    60,    61,    62,    -1,    -1,    92,    93,    94,
      95,    96,    97,    98,    99,   100,    -1,    -1,    -1,   104,
      -1,   106,   107,   108,   109,   110,   111,   112,   113,   114,
      88,    89,    -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   121,   122,   123,   124,   125,   126,   127,
      -1,    -1,    -1,    -1,    -1,    -1,   134
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   151,   152,     0,    75,   153,   154,     7,     3,   155,
       3,     4,    10,    22,    57,    58,    59,    66,    67,    72,
      74,    77,    78,    82,    83,    85,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   104,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   156,   162,   163,   164,   169,
     170,   171,   179,   181,   196,   217,   218,   225,   182,    22,
      50,    82,    83,    84,    90,   180,     3,     6,     6,     6,
       6,     3,     6,   170,     3,     6,     3,     3,   164,    10,
     179,   181,     6,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    23,    24,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    36,    37,    39,    42,    43,    44,
      45,    46,    47,    48,    49,    51,    52,    53,    56,    67,
      73,    82,   101,   102,   103,   104,   105,   106,   108,   109,
     110,   111,   112,   119,   125,   134,   183,   184,   186,   187,
     192,   193,   195,   201,   215,   216,   217,   219,    11,    11,
      11,   172,    11,    11,    20,   185,   186,    25,   128,   173,
     174,   211,   211,    80,    81,   142,   146,   147,   226,   227,
     228,   229,   230,   231,   232,   233,   233,   233,   165,   233,
     157,   120,   202,    60,    61,    62,    88,    89,    91,   121,
     122,   123,   124,   125,   126,   127,   134,   175,   178,   203,
     204,   205,   206,   207,   208,   209,   210,    39,    86,    87,
     176,   177,   225,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     4,     5,   183,     5,
     183,   211,   189,   190,   233,   202,    11,    35,    36,    37,
      38,   220,    40,    41,   222,     6,    11,     5,     6,     4,
       5,   227,   227,   233,   139,   140,   141,   142,   143,   144,
     145,   138,   137,   136,   166,    16,    17,    18,    39,    48,
      51,    82,   106,   108,   109,   110,   111,   112,   125,   134,
     158,   159,     6,     6,     4,     5,     6,     6,     6,     4,
       5,   176,    79,   115,   197,    79,   116,   198,    79,   117,
     199,   118,   200,   101,   192,   201,   233,   129,   213,   130,
     212,   131,   214,   132,   233,   233,   233,   233,   233,   233,
     135,   221,   233,   188,   194,   233,   191,   233,   225,    75,
     225,   233,   233,   225,   202,   225,   225,   225,   225,   225,
     225,   183,   183,     5,   175,     6,     6,     6,     9,     6,
       6,     5,   201,   225,   185,   233,   173,   148,   227,   227,
     227,   228,   228,   229,   229,   230,   231,   232,     4,    20,
      63,    64,    65,    67,    68,    69,    70,    71,   167,   168,
     169,   179,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     4,     5,   158,   233,   233,
     175,   233,    76,   233,   176,     9,   147,   189,   233,   233,
     221,   220,   133,   223,    54,    55,   224,   222,     6,     6,
       6,     6,     6,     6,     6,     5,   167,   197,   198,   199,
     233,   160,   161,   233,   225,   202,   225,   225,   225,   225,
     225,   225,   158,   188,   149,   233,   201,   233,   233,   233,
     233,    75,   233,   167,     9,   147,   148,   148,   160,   149,
     233,   148,   148
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
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   150,   151,   152,   152,   154,   153,   155,   155,   155,
     155,   155,   155,   155,   155,   157,   156,   158,   158,   158,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   160,   160,   161,   161,
     161,   162,   162,   162,   162,   162,   162,   163,   163,   165,
     164,   166,   166,   166,   166,   167,   167,   167,   168,   168,
     168,   168,   168,   168,   168,   168,   168,   169,   169,   170,
     170,   170,   171,   172,   171,   171,   171,   171,   171,   171,
     171,   171,   171,   173,   173,   174,   174,   175,   175,   176,
     176,   176,   177,   177,   177,   178,   178,   178,   178,   178,
     178,   178,   178,   178,   178,   178,   178,   178,   178,   180,
     179,   179,   182,   181,   183,   183,   183,   183,   183,   183,
     184,   184,   184,   184,   184,   185,   185,   186,   186,   186,
     186,   186,   187,   187,   188,   188,   189,   189,   190,   191,
     192,   192,   192,   192,   192,   192,   192,   192,   192,   192,
     192,   192,   192,   192,   192,   193,   193,   194,   194,   194,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   196,   196,   196,   196,   196,   196,   196,   196,
     197,   197,   198,   198,   199,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   215,   215,   215,   215,   215,   215,   215,   216,
     216,   217,   217,   217,   217,   217,   217,   217,   217,   217,
     218,   218,   218,   218,   219,   219,   219,   220,   220,   220,
     220,   220,   221,   222,   222,   222,   223,   224,   224,   225,
     225,   226,   226,   226,   227,   227,   227,   228,   228,   228,
     228,   229,   229,   229,   230,   230,   230,   231,   231,   232,
     232,   233,   233
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     0,     6,     0,     2,     2,
       2,     2,     2,     2,     2,     0,     5,     1,     3,     2,
       0,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     3,     1,     1,     3,     1,     4,
       4,     3,     3,     3,     3,     3,     3,     1,     2,     0,
       5,     0,     2,     2,     2,     1,     3,     2,     3,     3,
       1,     1,     3,     3,     3,     3,     3,     2,     1,     0,
       2,     2,     4,     0,     5,     4,     4,     4,     3,     4,
       4,     4,     4,     1,     3,     1,     3,     1,     3,     1,
       3,     2,     3,     3,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     3,     1,     1,     1,     0,
       5,     1,     0,     4,     1,     1,     3,     3,     2,     2,
       3,     3,     3,     3,     3,     1,     3,     1,     1,     1,
       1,     1,     3,     3,     1,     3,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     4,     4,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     3,     3,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     1,     1,     3,     3,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       1,     3,     1,     3,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     2,     2,     1,     3,     3,
       3,     1,     3,     3,     1,     3,     3,     1,     3,     1,
       3,     1,     3
  };

#if YYDEBUG
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const BisonParser::yytname_[] =
  {
    "$end", "error", "$undefined", "OBRACE", "EBRACE", "COMMA", "EQUAL",
  "COLON", "QUOTE", "SEMICOLON", "OBRACKET", "EBRACKET", "BOOTLOADER",
  "XIP_MODE", "EARLY_HANDOFF", "HIVEC", "AUTHENTICATION", "ENCRYPTION",
  "CHECKSUM", "PARTITION_OWNER", "PARTITION_TYPE", "PARTITION_NUM",
  "BOOT_DEVICE", "DEST_DEVICE", "DEST_CPU", "ADDRESS", "EXCEPTION_LEVEL",
  "TRUSTZONE", "ALIGNMENT", "OFFSET", "RESERVE", "LOAD", "STARTUP",
  "BIGENDIAN", "A32_MODE", "PPK_SELECT", "SPK_ID", "SPK_SELECT",
  "HEADER_AUTH", "REVOKE_ID", "SPLIT_MODE", "SPLIT_FMT", "BOOT", "USER",
  "STATIC", "NOAUTOSTART", "MULTIBOOT", "PROTECTED", "BLOCKS",
  "AUTHBLOCKS", "BOOTVECTORS", "PRESIGN", "BIF_SECTION", "UDF_DATA", "MCS",
  "BIN", "SLR_NUM", "PARENT_ID", "ID_CODE", "EXT_ID_CODE",
  "BYPASS_IDCODE_CHECK", "A_HWROT", "S_HWROT", "UNIQUE_ID",
  "PARENT_UNIQUE_ID", "FUNCTION_ID", "IMAGE", "ID", "NAME",
  "DELAY_HANDOFF", "DELAY_LOAD", "COPY", "PARTITION", "PFILE",
  "METAHEADER", "WORD", "HEXWORD", "FILENAME", "QFILENAME", "NONE",
  "DECVALUE", "HEXVALUE", "KEYSRC_ENCRYPTION", "FSBL_CONFIG",
  "AUTH_PARAMS", "AUTHJTAG_CONFIG", "DEVICE_DNA", "JTAG_TIMEOUT",
  "PUF4KMODE", "SHUTTER", "SPLIT", "SMAP_WIDTH", "PUF_HELPER_FILE",
  "BH_KEY_FILE", "BH_KEY_IV", "BH_KEK_IV", "BBRAM_KEK_IV", "EFUSE_KEK_IV",
  "EFUSE_USER_KEK0_IV", "EFUSE_USER_KEK1_IV", "USER_KEYS", "PMCDATA",
  "BOOTIMAGE", "UDF_BH", "INIT", "PMUFW_IMAGE", "AES_KEY_FILE",
  "FAMILY_KEY", "PPK_FILE", "PSK_FILE", "SPK_FILE", "SSK_FILE",
  "SPK_SIGNATURE_FILE", "BH_SIGNATURE_FILE", "HEADER_SIGNATURE_FILE",
  "AUTHVALUE", "ENCRVALUE", "CHECKSUMVALUE", "POWNERVALUE", "PTYPEVALUE",
  "KEY_SRC", "CORE", "BH_RSA", "AUTH_HASH", "INT_HASH", "PUFHD_LOC",
  "OPT_KEY", "AUTH_ONLY", "BOOT_DEVICE_TYPE", "DEST_DEVICE_TYPE",
  "DEST_CPU_TYPE", "EXCEPTION_LEVEL_TYPE", "TRUSTZONE_TYPE", "SPLITMODE",
  "DPA_CM", "SPKSELECT", "OR", "XOR", "AND", "MULT", "DIVIDE", "MODULO",
  "PLUS", "MINUS", "LSHIFT", "RSHIFT", "NEGATION", "LPAREN", "RPAREN",
  "ASTERISK", "$accept", "bif", "group_list", "bifoptions", "$@1",
  "file_list", "metahdr_spec", "$@2", "metahdr_attr_list", "metahdr_attr",
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
       151,     0,    -1,   152,    -1,    -1,   152,   153,    -1,    -1,
      75,   154,     7,     3,   155,     4,    -1,    -1,   155,   179,
      -1,   155,   171,    -1,   155,   163,    -1,   155,   156,    -1,
     155,   162,    -1,   155,   181,    -1,   155,   169,    -1,    -1,
      74,     3,   157,   158,     4,    -1,   159,    -1,   159,     5,
     158,    -1,   159,   158,    -1,    -1,    17,     6,   198,    -1,
      82,     6,   202,    -1,   106,     6,   225,    -1,    16,     6,
     197,    -1,   108,     6,   225,    -1,   109,     6,   225,    -1,
     110,     6,   225,    -1,   111,     6,   225,    -1,   112,     6,
     225,    -1,    51,     6,   225,    -1,    39,     6,   233,    -1,
      18,     6,   199,    -1,   134,    -1,    48,     6,   160,    -1,
     125,    -1,   161,    -1,   161,     9,   160,    -1,   233,    -1,
     233,   147,   233,   148,    -1,   233,   147,   149,   148,    -1,
      67,     6,   233,    -1,    57,     6,   233,    -1,    58,     6,
     233,    -1,    59,     6,   233,    -1,   196,     6,   225,    -1,
      82,     6,   202,    -1,   164,    -1,   163,   164,    -1,    -1,
      66,     3,   165,   166,     4,    -1,    -1,   166,   167,    -1,
     166,   179,    -1,   166,   169,    -1,   168,    -1,   168,     5,
     167,    -1,   168,   167,    -1,    67,     6,   233,    -1,    68,
       6,    75,    -1,    69,    -1,    70,    -1,    71,     6,   233,
      -1,    20,     6,   201,    -1,    63,     6,   233,    -1,    64,
       6,   233,    -1,    65,     6,   233,    -1,    72,   170,    -1,
     170,    -1,    -1,   170,   179,    -1,   170,   181,    -1,    10,
      82,    11,   202,    -1,    -1,    10,    83,   172,    11,   175,
      -1,    83,     3,   175,     4,    -1,    22,     3,   173,     4,
      -1,    10,    22,    11,   211,    -1,    22,     6,   211,    -1,
      10,    84,    11,   220,    -1,    10,    90,    11,   222,    -1,
      10,    50,    11,   189,    -1,    85,     3,   176,     4,    -1,
     174,    -1,   174,     5,   173,    -1,   211,    -1,    25,     6,
     233,    -1,   178,    -1,   178,     5,   175,    -1,   177,    -1,
     177,     5,   176,    -1,   177,   176,    -1,    39,     6,   233,
      -1,    86,     6,    76,    -1,    87,     6,   233,    -1,   203,
      -1,   204,    -1,   206,    -1,   207,    -1,   208,    -1,   210,
      -1,   209,    -1,    88,    -1,    89,     6,   233,    -1,   205,
      -1,    91,     6,   233,    -1,    60,    -1,    61,    -1,    62,
      -1,    -1,    10,   180,   185,    11,   225,    -1,   225,    -1,
      -1,     3,   182,   183,     4,    -1,   186,    -1,   184,    -1,
     184,     5,   183,    -1,   186,     5,   183,    -1,   184,   183,
      -1,   186,   183,    -1,    73,     6,   225,    -1,    67,     6,
     233,    -1,    20,     6,   192,    -1,    20,     6,   101,    -1,
      52,     6,    75,    -1,   186,    -1,   186,     5,   185,    -1,
     192,    -1,   195,    -1,   215,    -1,   216,    -1,   187,    -1,
      48,     6,   188,    -1,    49,     6,   191,    -1,   194,    -1,
     194,     9,   188,    -1,   190,    -1,   190,     5,   189,    -1,
     233,    -1,   233,    -1,    12,    -1,    42,    -1,    43,    -1,
      44,    -1,    45,    -1,    46,    -1,    47,    -1,    14,    -1,
      15,    -1,    13,    -1,   104,    -1,   102,    -1,   217,    -1,
     219,    -1,   201,    -1,    27,    -1,    27,     6,   132,    -1,
     233,    -1,   233,   147,   233,   148,    -1,   233,   147,   149,
     148,    -1,    16,     6,   197,    -1,    17,     6,   198,    -1,
      18,     6,   199,    -1,    19,     6,   200,    -1,    24,     6,
     212,    -1,    23,     6,   213,    -1,    26,     6,   214,    -1,
     106,     6,   225,    -1,   108,     6,   225,    -1,   109,     6,
     225,    -1,   110,     6,   225,    -1,   111,     6,   225,    -1,
      37,     6,   221,    -1,    36,     6,   233,    -1,   112,     6,
     225,    -1,   193,    -1,    20,     6,   201,    -1,    82,     6,
     202,    -1,    39,     6,   233,    -1,   134,    -1,    56,     6,
     233,    -1,   125,    -1,   104,    -1,   217,    -1,    95,    -1,
      96,    -1,    97,    -1,    98,    -1,    99,    -1,   100,    -1,
      79,    -1,   115,    -1,    79,    -1,   116,    -1,    79,    -1,
     117,    -1,   118,    -1,   119,    -1,   120,    -1,   121,    -1,
     122,    -1,   134,    -1,   123,    -1,   124,    -1,   125,    -1,
     126,    -1,   127,    -1,   128,    -1,   130,    -1,   129,    -1,
     131,    -1,    28,     6,   233,    -1,    29,     6,   233,    -1,
      30,     6,   233,    -1,    31,     6,   233,    -1,    32,     6,
     233,    -1,    33,    -1,    34,    -1,    21,     6,   233,    -1,
      51,     6,   225,    -1,    53,     6,   225,    -1,   106,    -1,
     218,    -1,   112,    -1,   113,    -1,   114,    -1,    93,    -1,
      92,    -1,    94,    -1,   107,    -1,   108,    -1,   109,    -1,
     110,    -1,   111,    -1,   105,    -1,   101,    -1,   103,    -1,
      35,     6,   233,    -1,    37,     6,   221,    -1,    36,     6,
     233,    -1,    38,    -1,   220,     9,   220,    -1,   135,    -1,
      40,     6,   223,    -1,    41,     6,   224,    -1,   222,     5,
     222,    -1,   133,    -1,    54,    -1,    55,    -1,    77,    -1,
      78,    -1,    81,    -1,    80,    -1,   147,   233,   148,    -1,
     226,    -1,   142,   227,    -1,   146,   227,    -1,   227,    -1,
     228,   139,   227,    -1,   228,   140,   227,    -1,   228,   141,
     227,    -1,   228,    -1,   229,   142,   228,    -1,   229,   143,
     228,    -1,   229,    -1,   230,   144,   229,    -1,   230,   145,
     229,    -1,   230,    -1,   231,   138,   230,    -1,   231,    -1,
     232,   137,   231,    -1,   232,    -1,   233,   136,   232,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     5,     6,     9,    10,    17,    18,    21,
      24,    27,    30,    33,    36,    39,    40,    46,    48,    52,
      55,    56,    60,    64,    68,    72,    76,    80,    84,    88,
      92,    96,   100,   104,   106,   110,   112,   114,   118,   120,
     125,   130,   134,   138,   142,   146,   150,   154,   156,   159,
     160,   166,   167,   170,   173,   176,   178,   182,   185,   189,
     193,   195,   197,   201,   205,   209,   213,   217,   220,   222,
     223,   226,   229,   234,   235,   241,   246,   251,   256,   260,
     265,   270,   275,   280,   282,   286,   288,   292,   294,   298,
     300,   304,   307,   311,   315,   319,   321,   323,   325,   327,
     329,   331,   333,   335,   339,   341,   345,   347,   349,   351,
     352,   358,   360,   361,   366,   368,   370,   374,   378,   381,
     384,   388,   392,   396,   400,   404,   406,   410,   412,   414,
     416,   418,   420,   424,   428,   430,   434,   436,   440,   442,
     444,   446,   448,   450,   452,   454,   456,   458,   460,   462,
     464,   466,   468,   470,   472,   474,   476,   480,   482,   487,
     492,   496,   500,   504,   508,   512,   516,   520,   524,   528,
     532,   536,   540,   544,   548,   552,   554,   558,   562,   566,
     568,   572,   574,   576,   578,   580,   582,   584,   586,   588,
     590,   592,   594,   596,   598,   600,   602,   604,   606,   608,
     610,   612,   614,   616,   618,   620,   622,   624,   626,   628,
     630,   632,   636,   640,   644,   648,   652,   654,   656,   660,
     664,   668,   670,   672,   674,   676,   678,   680,   682,   684,
     686,   688,   690,   692,   694,   696,   698,   700,   704,   708,
     712,   714,   718,   720,   724,   728,   732,   734,   736,   738,
     740,   742,   744,   746,   750,   752,   755,   758,   760,   764,
     768,   772,   774,   778,   782,   784,   788,   792,   794,   798,
     800,   804,   806
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   196,   196,   198,   199,   202,   202,   208,   209,   210,
     211,   212,   213,   214,   215,   218,   218,   223,   224,   225,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   247,   248,   251,   253,
     255,   259,   260,   261,   262,   263,   264,   267,   268,   271,
     271,   275,   276,   277,   278,   281,   282,   283,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   298,   299,   302,
     303,   304,   307,   310,   310,   313,   314,   315,   320,   321,
     322,   323,   324,   327,   328,   331,   332,   335,   336,   339,
     340,   341,   344,   345,   346,   349,   352,   354,   356,   358,
     360,   362,   364,   366,   368,   374,   378,   379,   380,   383,
     383,   390,   396,   396,   402,   403,   404,   405,   406,   407,
     410,   413,   414,   415,   416,   421,   422,   425,   426,   427,
     428,   429,   432,   433,   436,   437,   440,   441,   444,   449,
     451,   452,   453,   454,   455,   456,   457,   458,   459,   460,
     463,   464,   465,   466,   467,   470,   471,   474,   475,   476,
     478,   479,   480,   481,   482,   483,   484,   485,   486,   487,
     488,   489,   490,   491,   492,   493,   494,   495,   496,   497,
     498,   499,   502,   503,   504,   505,   506,   507,   508,   509,
     512,   513,   516,   517,   520,   521,   524,   527,   530,   533,
     536,   539,   542,   545,   548,   551,   554,   557,   560,   563,
     566,   569,   570,   571,   572,   573,   574,   575,   576,   579,
     580,   583,   584,   585,   586,   587,   588,   589,   590,   591,
     594,   595,   596,   597,   600,   601,   602,   605,   608,   611,
     614,   617,   620,   623,   626,   627,   630,   633,   636,   642,
     643,   646,   647,   648,   654,   655,   656,   660,   661,   662,
     663,   666,   667,   668,   671,   672,   673,   676,   677,   680,
     681,   685,   686
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
     145,   146,   147,   148,   149
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 866;
  const int BisonParser::yynnts_ = 84;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 150;

  const unsigned int BisonParser::yyuser_token_number_max_ = 404;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 23 "../s/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 2375 "../bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 691 "../s/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
