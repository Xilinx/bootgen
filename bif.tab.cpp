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
#line 201 "../s/bif.y"
    { currentBifOptions = new BifOptions(options.GetArchType(),(yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 203 "../s/bif.y"
    { options.bifOptions=currentBifOptions; }
    break;

  case 15:
/* Line 670 of lalr1.cc  */
#line 216 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 21:
/* Line 670 of lalr1.cc  */
#line 227 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.encrypt = (yysemantic_stack_[(3) - (3)].encrvalue_t); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 228 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 229 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.encrKeyFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 230 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.authenticate = (yysemantic_stack_[(3) - (3)].authvalue_t); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 231 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 232 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 233 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 234 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 235 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 236 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 237 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.revokeId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 238 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 239 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 241 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 38:
/* Line 670 of lalr1.cc  */
#line 249 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 39:
/* Line 670 of lalr1.cc  */
#line 251 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 253 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 257 "../s/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 258 "../s/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 259 "../s/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 260 "../s/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 261 "../s/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 262 "../s/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 269 "../s/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 270 "../s/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions); }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 284 "../s/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 59:
/* Line 670 of lalr1.cc  */
#line 285 "../s/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 286 "../s/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 287 "../s/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 288 "../s/bif.y"
    { currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 289 "../s/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 302 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[keysrc_encryption] not supported in VERSAL architecture.\n\t   Please see 'bootgen -arch versal -bif_help keysrc'");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 305 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 310 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                    if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); 
                                                                                    currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 75:
/* Line 670 of lalr1.cc  */
#line 315 "../s/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); }
    break;

  case 81:
/* Line 670 of lalr1.cc  */
#line 325 "../s/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 82:
/* Line 670 of lalr1.cc  */
#line 326 "../s/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 85:
/* Line 670 of lalr1.cc  */
#line 333 "../s/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 86:
/* Line 670 of lalr1.cc  */
#line 336 "../s/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 87:
/* Line 670 of lalr1.cc  */
#line 338 "../s/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 88:
/* Line 670 of lalr1.cc  */
#line 340 "../s/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 89:
/* Line 670 of lalr1.cc  */
#line 342 "../s/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 90:
/* Line 670 of lalr1.cc  */
#line 344 "../s/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 91:
/* Line 670 of lalr1.cc  */
#line 346 "../s/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 92:
/* Line 670 of lalr1.cc  */
#line 348 "../s/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 93:
/* Line 670 of lalr1.cc  */
#line 350 "../s/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 94:
/* Line 670 of lalr1.cc  */
#line 352 "../s/bif.y"
    {  if(options.GetArchType() != Arch::VERSAL)
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t\t'dpacm_enable' is supported only in VERSAL architecture");
                                                                                   if(options.GetArchType() == Arch::VERSAL)
                                                                                     LOG_WARNING("boot_config { dpacm_enable } will be deprecated. 'dpacm_enable' should be specified along with the partition. \n            See 'bootgen -bif_help dpacm_enable' for more info.");
                                                                                   currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));
                                                                                }
    break;

  case 95:
/* Line 670 of lalr1.cc  */
#line 358 "../s/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->smapWidth = (yysemantic_stack_[(3) - (3)].number);
                                                                                }
    break;

  case 96:
/* Line 670 of lalr1.cc  */
#line 362 "../s/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 97:
/* Line 670 of lalr1.cc  */
#line 365 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 98:
/* Line 670 of lalr1.cc  */
#line 368 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string); 
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 99:
/* Line 670 of lalr1.cc  */
#line 371 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 376 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 108:
/* Line 670 of lalr1.cc  */
#line 390 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string); 
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 392 "../s/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 394 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 420 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] only supported in ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 425 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 427 "../s/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 428 "../s/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 129:
/* Line 670 of lalr1.cc  */
#line 429 "../s/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 130:
/* Line 670 of lalr1.cc  */
#line 430 "../s/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 431 "../s/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 432 "../s/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 133:
/* Line 670 of lalr1.cc  */
#line 433 "../s/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 134:
/* Line 670 of lalr1.cc  */
#line 434 "../s/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 135:
/* Line 670 of lalr1.cc  */
#line 435 "../s/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 136:
/* Line 670 of lalr1.cc  */
#line 436 "../s/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 137:
/* Line 670 of lalr1.cc  */
#line 439 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 138:
/* Line 670 of lalr1.cc  */
#line 440 "../s/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 441 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 442 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 443 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 446 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 447 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 450 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 451 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 452 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 454 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 455 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 456 "../s/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 150:
/* Line 670 of lalr1.cc  */
#line 457 "../s/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 151:
/* Line 670 of lalr1.cc  */
#line 458 "../s/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 152:
/* Line 670 of lalr1.cc  */
#line 459 "../s/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 153:
/* Line 670 of lalr1.cc  */
#line 460 "../s/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 461 "../s/bif.y"
    { currentPartitionBifOptions->aesKeyFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 462 "../s/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 463 "../s/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 157:
/* Line 670 of lalr1.cc  */
#line 464 "../s/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 465 "../s/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 466 "../s/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 467 "../s/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 161:
/* Line 670 of lalr1.cc  */
#line 468 "../s/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 470 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 471 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 472 "../s/bif.y"
    { currentPartitionBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 473 "../s/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 474 "../s/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 475 "../s/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 487 "../s/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 491 "../s/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 495 "../s/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 197:
/* Line 670 of lalr1.cc  */
#line 544 "../s/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 198:
/* Line 670 of lalr1.cc  */
#line 545 "../s/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 546 "../s/bif.y"
    { currentPartitionBifOptions->reserve = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 200:
/* Line 670 of lalr1.cc  */
#line 547 "../s/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 201:
/* Line 670 of lalr1.cc  */
#line 548 "../s/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 202:
/* Line 670 of lalr1.cc  */
#line 549 "../s/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 203:
/* Line 670 of lalr1.cc  */
#line 550 "../s/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 204:
/* Line 670 of lalr1.cc  */
#line 551 "../s/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 205:
/* Line 670 of lalr1.cc  */
#line 554 "../s/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 206:
/* Line 670 of lalr1.cc  */
#line 555 "../s/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 223:
/* Line 670 of lalr1.cc  */
#line 580 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 224:
/* Line 670 of lalr1.cc  */
#line 583 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 225:
/* Line 670 of lalr1.cc  */
#line 586 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 226:
/* Line 670 of lalr1.cc  */
#line 589 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 229:
/* Line 670 of lalr1.cc  */
#line 598 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 233:
/* Line 670 of lalr1.cc  */
#line 608 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 234:
/* Line 670 of lalr1.cc  */
#line 611 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 239:
/* Line 670 of lalr1.cc  */
#line 623 "../s/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 241:
/* Line 670 of lalr1.cc  */
#line 630 "../s/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 242:
/* Line 670 of lalr1.cc  */
#line 631 "../s/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 244:
/* Line 670 of lalr1.cc  */
#line 636 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 245:
/* Line 670 of lalr1.cc  */
#line 637 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 246:
/* Line 670 of lalr1.cc  */
#line 638 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 248:
/* Line 670 of lalr1.cc  */
#line 642 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 249:
/* Line 670 of lalr1.cc  */
#line 643 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 251:
/* Line 670 of lalr1.cc  */
#line 647 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 252:
/* Line 670 of lalr1.cc  */
#line 648 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 254:
/* Line 670 of lalr1.cc  */
#line 652 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 256:
/* Line 670 of lalr1.cc  */
#line 656 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 258:
/* Line 670 of lalr1.cc  */
#line 661 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1309 "../bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -353;
  const short int
  BisonParser::yypact_[] =
  {
      -353,    29,   -33,  -353,  -353,  -353,    45,    69,  -353,   172,
    -353,  -353,   -10,    27,    75,    78,    81,    73,    83,  -353,
      96,  -353,  -353,   105,   118,  -353,  -353,  -353,  -353,  -353,
    -353,  -353,  -353,  -353,  -353,  -353,  -353,  -353,  -353,  -353,
    -353,  -353,  -353,  -353,  -353,    48,  -353,  -353,    12,  -353,
    -353,  -353,   146,  -353,  -353,  -353,   590,   154,   162,   163,
    -353,   168,   169,   702,   -11,    40,     0,     0,     0,  -353,
       0,    12,  -353,    54,   225,  -353,  -353,  -353,  -353,   -15,
    -353,  -353,  -353,  -353,   175,   177,   178,   179,   180,   181,
     182,   183,   185,   189,   190,   191,   192,   194,   195,  -353,
    -353,   196,   197,   198,  -353,  -353,  -353,  -353,  -353,  -353,
     199,   200,   201,   203,   204,   205,   213,   214,  -353,  -353,
    -353,  -353,  -353,   215,   220,   221,   228,   229,   230,  -353,
    -353,  -353,   227,   366,   478,  -353,  -353,  -353,  -353,  -353,
    -353,  -353,  -353,  -353,    40,     0,    54,   226,   102,    39,
     233,   235,   236,   238,  -353,   258,   259,  -353,  -353,  -353,
    -353,     0,     0,     0,  -353,  -353,     4,   -18,     9,   136,
     150,   153,   153,   153,  -353,   153,    -7,  -353,  -353,  -353,
    -353,   271,   273,  -353,  -353,  -353,  -353,  -353,  -353,  -353,
    -353,   278,   280,  -353,  -353,  -353,  -353,  -353,  -353,  -353,
    -353,  -353,   -27,   -34,   -37,   176,   775,     0,   170,   167,
     171,   166,     0,     0,     0,     0,     0,     0,   165,     0,
       0,     0,   -15,   -15,     0,     0,   -15,    54,   -15,   -15,
     -15,   -15,   -15,   -15,  -353,   590,  -353,   590,  -353,  -353,
    -353,   285,   153,  -353,   225,   286,   287,   288,  -353,   289,
     290,   291,   294,   187,   -15,   702,     0,  -353,   -11,  -353,
    -353,   -94,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   152,   295,   296,   299,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,  -353,  -353,   315,     3,
       0,     0,  -353,   225,  -353,  -353,  -353,  -353,  -353,  -353,
    -353,  -353,  -353,  -353,  -353,  -353,  -353,  -353,   153,  -353,
    -353,  -353,  -353,  -353,  -353,  -353,   153,   153,   153,   153,
     153,   153,  -353,  -353,   153,  -353,   311,   -86,  -353,   153,
    -353,  -353,   153,   153,  -353,  -353,  -353,  -353,  -353,  -353,
    -353,  -353,  -353,  -353,     0,  -353,     0,     0,   165,   102,
     202,    70,    39,  -353,  -353,  -353,   153,  -353,  -353,  -353,
    -353,  -353,     4,     4,   -18,   -18,     9,   136,   150,  -353,
     316,   325,   326,  -353,  -353,   336,  -353,   188,  -353,  -353,
     -27,   -34,   -37,     0,     0,   -15,    54,   -15,   -15,   -15,
     -15,   -15,   -15,  -353,    -7,  -353,   153,   153,  -353,     0,
     -26,  -353,   153,   153,  -353,   289,  -353,  -353,  -353,  -353,
    -353,   294,   187,     0,   231,     0,    -2,  -353,  -353,  -353,
    -353,   153,  -353,   312,   -40,  -353,  -353,  -353,  -353,  -353,
    -353,  -353,  -353,  -353,  -353,   206,   -91,  -353,   153,  -353,
     153,  -353,     0,   -16,  -353,  -353,  -353,   207,   -88,  -353,
    -353
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     5,     4,     0,     0,     7,     0,
     100,     6,    97,     0,     0,     0,     0,     0,     0,    66,
       0,   235,   236,     0,     0,   213,   212,   214,   171,   172,
     173,   174,   175,   169,   207,   215,   216,   217,   218,   219,
     209,   210,   211,    11,    12,    10,    47,    14,    65,     9,
       8,    13,     0,   170,   208,    99,     0,     0,     0,     0,
      70,     0,     0,     0,     0,     0,     0,     0,     0,    49,
       0,    64,    15,     0,     0,    48,    97,    67,    68,     0,
     127,   136,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   142,     0,     0,     0,     0,     0,   202,
     203,     0,     0,     0,   128,   129,   130,   131,   132,   133,
       0,     0,     0,     0,     0,     0,     0,     0,   221,   138,
     222,   137,   220,   207,   216,   217,   218,   219,   209,   183,
     168,   166,     0,   103,   102,   118,   114,   162,   115,   141,
     116,   117,   139,   140,     0,     0,     0,     0,     0,     0,
       0,     0,   112,     0,   193,     0,    79,    81,    75,   238,
     237,     0,     0,     0,   240,   243,   247,   250,   253,   255,
     257,    42,    43,    44,    51,    41,    20,   184,    46,    96,
      92,     0,     0,   185,   186,   188,   189,   190,   191,   192,
     187,     0,    83,    85,    86,    94,    87,    88,    89,    91,
      90,    45,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   101,     0,   106,     0,   107,    74,
      78,   123,   125,    69,     0,     0,     0,     0,   226,    76,
       0,     0,    77,     0,     0,     0,     0,    73,     0,   241,
     242,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    66,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    35,    33,     0,    17,
       0,     0,    72,     0,   176,   177,   147,   178,   179,   148,
     180,   181,   149,   182,   150,   111,   110,   141,   204,   195,
     152,   194,   151,   196,   153,   143,   197,   198,   199,   200,
     201,   160,   228,   159,   165,   119,   121,   144,   120,   126,
     205,   206,   167,   109,   108,   164,   154,   155,   156,   157,
     158,   161,   104,   105,     0,    71,     0,     0,     0,     0,
       0,     0,     0,   163,    98,   113,    82,    80,   239,   244,
     245,   246,   248,   249,   251,   252,   254,   256,   258,    50,
       0,     0,     0,    60,    61,     0,    52,    55,    54,    53,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    16,    20,    19,    93,    95,    84,     0,
       0,   124,   223,   225,   224,   227,   232,   229,   233,   234,
     230,   231,     0,     0,     0,     0,     0,    57,    24,    21,
      32,    31,    34,    36,    38,    30,    22,    23,    25,    26,
      27,    28,    29,    18,   122,     0,     0,    63,    58,    59,
      62,    56,     0,     0,   146,   145,    37,     0,     0,    40,
      39
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -353,  -353,  -353,  -353,  -353,  -353,  -353,  -353,  -276,  -353,
     -97,  -353,  -353,  -353,   313,  -353,  -353,  -352,  -353,    74,
     331,  -353,  -353,    93,  -353,  -218,  -353,    -6,  -353,   343,
    -353,  -110,  -353,    98,   -56,  -353,   -45,    11,  -353,  -353,
     151,  -353,  -353,  -353,  -353,   -24,   -22,   -21,  -353,  -200,
    -141,  -353,  -353,  -353,  -353,  -353,  -353,  -353,  -353,   -49,
    -353,  -353,  -353,  -353,  -353,   351,  -353,  -353,    13,    15,
      14,  -353,  -353,   -62,  -353,  -134,  -121,   -90,    95,    97,
      94,   -66
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     5,     6,     9,    43,   176,   288,   289,
     422,   423,    44,    45,    46,   174,   272,   376,   377,    47,
      48,    49,   147,   155,   156,   191,   192,    77,    63,    78,
      56,   132,   133,   151,   134,   135,   325,   240,   241,   328,
     136,   137,   326,   138,    52,   296,   299,   302,   304,   139,
     178,   193,   194,   195,   196,   197,   198,   199,   200,   157,
     312,   310,   314,   140,   141,   142,    54,   143,   249,   323,
     252,   407,   410,    55,   164,   165,   166,   167,   168,   169,
     170,   242
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       171,   172,   173,    50,   175,   243,   307,   152,   394,   273,
     274,   275,    57,   395,   153,    10,   158,   201,   370,   273,
     274,   275,    76,   236,   238,   417,   345,   259,   260,     3,
      64,   271,   276,    65,   271,   300,     4,   271,   297,   271,
      58,   277,   276,   358,   278,   294,   445,   159,   160,   450,
     400,   277,     7,   353,   278,    21,    22,   159,   160,   371,
     372,   373,   374,   375,   441,    59,    60,    61,   279,   301,
      62,   298,     8,   159,   160,   398,    69,   295,   279,   250,
     251,    66,    21,    22,    67,   271,   335,    68,   280,    70,
     281,   282,   283,   284,   285,   239,   443,   261,   280,    72,
     281,   282,   283,   284,   285,   161,   154,   286,    17,   162,
     163,    73,   435,   265,   266,   161,   287,   286,   433,   162,
     163,    74,   447,   408,   409,   342,   287,   343,   359,   360,
     361,   161,   262,   263,   264,   162,   163,   245,   246,   247,
     248,   308,   267,   268,   362,   363,   316,   317,   318,   319,
     320,   321,    79,   324,   327,   329,   369,   154,   332,   333,
     330,   331,    76,   177,   334,   144,   336,   337,   338,   339,
     340,   341,   370,   145,   146,    10,    11,   364,   365,   148,
     149,   202,    12,   203,   204,   205,   206,   207,   208,   209,
     356,   210,   354,   416,    13,   211,   212,   213,   214,   152,
     215,   216,   217,   218,   219,   220,   221,   222,   370,   223,
     224,   225,   437,   371,   372,   373,   374,   375,    19,   226,
     227,   228,    21,    22,   396,   397,   229,   230,    14,    15,
      16,   234,    17,    18,   231,   232,   233,   244,    19,   253,
      20,   255,    21,    22,   256,   426,   254,    23,    24,   371,
     372,   373,   374,   375,    25,    26,    27,    28,    29,    30,
      31,    32,   257,   269,   258,    33,   379,    34,    35,    36,
      37,    38,    39,    40,    41,    42,   270,   290,   271,   291,
     402,   403,   292,   303,   179,   293,   311,   315,   309,   322,
     344,   313,   346,   347,   348,   129,   350,   351,   349,   352,
     439,   380,   381,   180,   181,   382,   182,   383,   384,   385,
     386,   387,   388,   389,   390,   391,   392,   421,   424,   393,
     399,   442,   412,   425,   406,   427,   428,   429,   430,   431,
     432,   413,   414,   327,   436,   183,   184,   185,   186,   187,
     188,   189,   415,   444,   449,   446,   378,   438,   190,   440,
      71,   357,    51,   355,   434,   401,   418,   306,    75,   419,
      53,   420,   405,   404,   366,   368,   411,   367,     0,     0,
       0,   235,     0,     0,     0,     0,   424,   448,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,     0,    90,
      91,     0,    92,    93,    94,    95,    96,    97,    98,    99,
     100,     0,   101,   102,     0,   103,     0,     0,   104,   105,
     106,   107,   108,   109,   110,   111,     0,   112,   113,     0,
       0,   114,     0,     0,     0,     0,     0,   115,     0,     0,
       0,     0,     0,   116,     0,     0,     0,     0,     0,     0,
       0,   117,     0,     0,     0,     0,     0,     0,    25,    26,
      27,     0,     0,     0,     0,     0,   118,   119,   120,   121,
     122,   123,    35,   124,   125,   126,   127,   128,    41,    42,
       0,     0,     0,     0,   129,     0,     0,     0,     0,     0,
     130,     0,     0,   237,     0,     0,     0,     0,     0,   131,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
       0,    90,    91,     0,    92,    93,    94,    95,    96,    97,
      98,    99,   100,     0,   101,   102,     0,   103,     0,     0,
     104,   105,   106,   107,   108,   109,   110,   111,     0,   112,
     113,     0,     0,   114,     0,     0,     0,     0,     0,   115,
       0,     0,     0,     0,     0,   116,     0,     0,     0,     0,
       0,     0,     0,   117,     0,     0,     0,     0,     0,     0,
      25,    26,    27,     0,     0,     0,     0,     0,   118,   119,
     120,   121,   122,   123,    35,   124,   125,   126,   127,   128,
      41,    42,     0,     0,     0,     0,   129,     0,     0,     0,
       0,     0,   130,     0,     0,     0,     0,     0,     0,     0,
       0,   131,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,     0,    90,    91,     0,    92,    93,    94,    95,
      96,    97,    98,    99,   100,     0,   101,   102,     0,   103,
       0,     0,   104,   105,   106,   107,   108,   109,   110,   111,
       0,   112,   113,     0,     0,   114,     0,     0,     0,     0,
       0,   115,     0,     0,     0,     0,     0,   116,     0,     0,
       0,     0,     0,     0,     0,   117,     0,     0,     0,     0,
       0,     0,    25,    26,    27,     0,     0,     0,     0,     0,
     118,   119,   120,   121,   122,   123,    35,   124,   125,   126,
     127,   128,    41,    42,     0,     0,     0,     0,   129,     0,
       0,     0,     0,     0,   130,     0,     0,     0,     0,     0,
       0,     0,     0,   131,    80,    81,    82,    83,    84,    85,
      86,    87,   150,    89,     0,    90,    91,     0,    92,    93,
      94,    95,    96,    97,    98,    99,   100,     0,   101,   102,
       0,   103,     0,     0,   104,   105,   106,   107,   108,   109,
     110,   111,     0,   112,   113,     0,     0,   114,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   117,     0,     0,
       0,     0,     0,     0,    25,    26,    27,    80,    81,    82,
      83,     0,   118,   119,   120,   121,   122,   123,    35,   124,
     125,   126,   127,   128,    41,    42,     0,     0,     0,     0,
     129,     0,     0,     0,     0,     0,   130,   104,   105,   106,
     107,   108,   109,     0,     0,   131,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    25,    26,    27,
       0,     0,     0,     0,     0,   305,   119,   120,   121,   122,
      34,    35,    36,    37,    38,    39,    40,    41,    42,     0,
       0,     0,     0,   129
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        66,    67,    68,     9,    70,   146,   206,    63,     5,    16,
      17,    18,    22,   289,    25,     3,    65,    79,    20,    16,
      17,    18,    10,   133,   134,   377,   244,   161,   162,     0,
       3,   125,    39,     6,   125,    72,    69,   125,    72,   125,
      50,    48,    39,   137,    51,    72,   137,    73,    74,   137,
     136,    48,     7,   253,    51,    70,    71,    73,    74,    61,
      62,    63,    64,    65,   416,    75,    76,    77,    75,   106,
      80,   105,     3,    73,    74,   293,     3,   104,    75,    40,
      41,     6,    70,    71,     6,   125,   227,     6,    95,     6,
      97,    98,    99,   100,   101,   144,   136,   163,    95,     3,
      97,    98,    99,   100,   101,   131,   117,   114,    60,   135,
     136,     6,   138,   131,   132,   131,   123,   114,   394,   135,
     136,     3,   138,    53,    54,   235,   123,   237,   262,   263,
     264,   131,   128,   129,   130,   135,   136,    35,    36,    37,
      38,   207,   133,   134,   265,   266,   212,   213,   214,   215,
     216,   217,     6,   219,   220,   221,     4,   117,   224,   225,
     222,   223,    10,   109,   226,    11,   228,   229,   230,   231,
     232,   233,    20,    11,    11,     3,     4,   267,   268,    11,
      11,     6,    10,     6,     6,     6,     6,     6,     6,     6,
     256,     6,   254,     5,    22,     6,     6,     6,     6,   255,
       6,     6,     6,     6,     6,     6,     6,     6,    20,     6,
       6,     6,   412,    61,    62,    63,    64,    65,    66,     6,
       6,     6,    70,    71,   290,   291,     6,     6,    56,    57,
      58,     4,    60,    61,     6,     6,     6,    11,    66,     6,
      68,     5,    70,    71,     6,   386,    11,    75,    76,    61,
      62,    63,    64,    65,    82,    83,    84,    85,    86,    87,
      88,    89,     4,   127,     5,    93,   272,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   126,     6,   125,     6,
     346,   347,     4,   107,    59,     5,   119,   121,   118,   124,
       5,   120,     6,     6,     6,   108,     6,     6,     9,     5,
      69,     6,     6,    78,    79,     6,    81,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,   383,   384,     4,
       9,     9,     6,   385,   122,   387,   388,   389,   390,   391,
     392,     6,     6,   399,   400,   110,   111,   112,   113,   114,
     115,   116,     6,   137,   137,   442,   272,   413,   123,   415,
      19,   258,     9,   255,   399,   344,   380,   206,    45,   381,
       9,   382,   349,   348,   269,   271,   352,   270,    -1,    -1,
      -1,     5,    -1,    -1,    -1,    -1,   442,   443,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    -1,    23,
      24,    -1,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    -1,    36,    37,    -1,    39,    -1,    -1,    42,    43,
      44,    45,    46,    47,    48,    49,    -1,    51,    52,    -1,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    61,    -1,    -1,
      -1,    -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    75,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,
      84,    -1,    -1,    -1,    -1,    -1,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
      -1,    -1,    -1,    -1,   108,    -1,    -1,    -1,    -1,    -1,
     114,    -1,    -1,     5,    -1,    -1,    -1,    -1,    -1,   123,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    23,    24,    -1,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    -1,    39,    -1,    -1,
      42,    43,    44,    45,    46,    47,    48,    49,    -1,    51,
      52,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    61,
      -1,    -1,    -1,    -1,    -1,    67,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    75,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    83,    84,    -1,    -1,    -1,    -1,    -1,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,    -1,    -1,    -1,    -1,   108,    -1,    -1,    -1,
      -1,    -1,   114,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   123,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    -1,    23,    24,    -1,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    -1,    36,    37,    -1,    39,
      -1,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      -1,    51,    52,    -1,    -1,    55,    -1,    -1,    -1,    -1,
      -1,    61,    -1,    -1,    -1,    -1,    -1,    67,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    75,    -1,    -1,    -1,    -1,
      -1,    -1,    82,    83,    84,    -1,    -1,    -1,    -1,    -1,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,    -1,    -1,    -1,    -1,   108,    -1,
      -1,    -1,    -1,    -1,   114,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   123,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    -1,    23,    24,    -1,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    -1,    36,    37,
      -1,    39,    -1,    -1,    42,    43,    44,    45,    46,    47,
      48,    49,    -1,    51,    52,    -1,    -1,    55,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    83,    84,    12,    13,    14,
      15,    -1,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,    -1,    -1,    -1,    -1,
     108,    -1,    -1,    -1,    -1,    -1,   114,    42,    43,    44,
      45,    46,    47,    -1,    -1,   123,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,    84,
      -1,    -1,    -1,    -1,    -1,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,    -1,
      -1,    -1,    -1,   108
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   140,   141,     0,    69,   142,   143,     7,     3,   144,
       3,     4,    10,    22,    56,    57,    58,    60,    61,    66,
      68,    70,    71,    75,    76,    82,    83,    84,    85,    86,
      87,    88,    89,    93,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   145,   151,   152,   153,   158,   159,   160,
     166,   168,   183,   204,   205,   212,   169,    22,    50,    75,
      76,    77,    80,   167,     3,     6,     6,     6,     6,     3,
       6,   159,     3,     6,     3,   153,    10,   166,   168,     6,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      23,    24,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    36,    37,    39,    42,    43,    44,    45,    46,    47,
      48,    49,    51,    52,    55,    61,    67,    75,    90,    91,
      92,    93,    94,    95,    97,    98,    99,   100,   101,   108,
     114,   123,   170,   171,   173,   174,   179,   180,   182,   188,
     202,   203,   204,   206,    11,    11,    11,   161,    11,    11,
      20,   172,   173,    25,   117,   162,   163,   198,   198,    73,
      74,   131,   135,   136,   213,   214,   215,   216,   217,   218,
     219,   220,   220,   220,   154,   220,   146,   109,   189,    59,
      78,    79,    81,   110,   111,   112,   113,   114,   115,   116,
     123,   164,   165,   190,   191,   192,   193,   194,   195,   196,
     197,   212,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     4,     5,   170,     5,   170,   198,
     176,   177,   220,   189,    11,    35,    36,    37,    38,   207,
      40,    41,   209,     6,    11,     5,     6,     4,     5,   214,
     214,   220,   128,   129,   130,   131,   132,   133,   134,   127,
     126,   125,   155,    16,    17,    18,    39,    48,    51,    75,
      95,    97,    98,    99,   100,   101,   114,   123,   147,   148,
       6,     6,     4,     5,    72,   104,   184,    72,   105,   185,
      72,   106,   186,   107,   187,    90,   179,   188,   220,   118,
     200,   119,   199,   120,   201,   121,   220,   220,   220,   220,
     220,   220,   124,   208,   220,   175,   181,   220,   178,   220,
     212,   212,   220,   220,   212,   189,   212,   212,   212,   212,
     212,   212,   170,   170,     5,   164,     6,     6,     6,     9,
       6,     6,     5,   188,   212,   172,   220,   162,   137,   214,
     214,   214,   215,   215,   216,   216,   217,   218,   219,     4,
      20,    61,    62,    63,    64,    65,   156,   157,   158,   166,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     4,     5,   147,   220,   220,   164,     9,
     136,   176,   220,   220,   208,   207,   122,   210,    53,    54,
     211,   209,     6,     6,     6,     6,     5,   156,   184,   185,
     186,   220,   149,   150,   220,   212,   189,   212,   212,   212,
     212,   212,   212,   147,   175,   138,   220,   188,   220,    69,
     220,   156,     9,   136,   137,   137,   149,   138,   220,   137,
     137
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
     385,   386,   387,   388,   389,   390,   391,   392,   393
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   139,   140,   141,   141,   143,   142,   144,   144,   144,
     144,   144,   144,   144,   144,   146,   145,   147,   147,   147,
     148,   148,   148,   148,   148,   148,   148,   148,   148,   148,
     148,   148,   148,   148,   148,   148,   149,   149,   150,   150,
     150,   151,   151,   151,   151,   151,   151,   152,   152,   154,
     153,   155,   155,   155,   155,   156,   156,   156,   157,   157,
     157,   157,   157,   157,   158,   158,   159,   159,   159,   160,
     161,   160,   160,   160,   160,   160,   160,   160,   160,   162,
     162,   163,   163,   164,   164,   165,   165,   165,   165,   165,
     165,   165,   165,   165,   165,   165,   165,   167,   166,   166,
     169,   168,   170,   170,   170,   170,   170,   170,   171,   171,
     171,   171,   172,   172,   173,   173,   173,   173,   173,   174,
     174,   175,   175,   176,   176,   177,   178,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   180,   180,   181,   181,   181,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   182,   182,   182,   182,   182,   182,   183,
     183,   183,   183,   183,   183,   183,   184,   184,   185,   185,
     186,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   202,   202,
     202,   202,   202,   202,   202,   203,   203,   204,   204,   204,
     204,   204,   204,   204,   204,   204,   205,   205,   205,   205,
     206,   206,   206,   207,   207,   207,   207,   207,   208,   209,
     209,   209,   210,   211,   211,   212,   212,   213,   213,   213,
     214,   214,   214,   215,   215,   215,   215,   216,   216,   216,
     217,   217,   217,   218,   218,   219,   219,   220,   220
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
       1,     1,     3,     3,     2,     1,     0,     2,     2,     4,
       0,     5,     4,     4,     4,     3,     4,     4,     4,     1,
       3,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     3,     1,     0,     5,     1,
       0,     4,     1,     1,     3,     3,     2,     2,     3,     3,
       3,     3,     1,     3,     1,     1,     1,     1,     1,     3,
       3,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     1,     4,     4,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     3,     3,     3,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     3,     3,
       3,     3,     1,     1,     3,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     1,     3,     1,     3,
       3,     3,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     2,     2,     1,     3,     3,     3,     1,     3,     3,
       1,     3,     3,     1,     3,     1,     3,     1,     3
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
  "AUTHBLOCKS", "BOOTVECTORS", "PRESIGN", "UDF_DATA", "MCS", "BIN",
  "SLR_NUM", "PARENT_ID", "ID_CODE", "EXT_ID_CODE", "BYPASS_IDCODE_CHECK",
  "IMAGE", "ID", "NAME", "DELAY_HANDOFF", "DELAY_LOAD", "COPY",
  "PARTITION", "PFILE", "METAHEADER", "WORD", "FILENAME", "QFILENAME",
  "NONE", "DECVALUE", "HEXVALUE", "KEYSRC_ENCRYPTION", "FSBL_CONFIG",
  "AUTH_PARAMS", "PUF4KMODE", "SHUTTER", "SPLIT", "SMAP_WIDTH",
  "PUF_HELPER_FILE", "BH_KEY_FILE", "BH_KEY_IV", "BH_KEK_IV",
  "BBRAM_KEK_IV", "EFUSE_KEK_IV", "EFUSE_USER_KEK0_IV",
  "EFUSE_USER_KEK1_IV", "PMCDATA", "BOOTIMAGE", "UDF_BH", "INIT",
  "PMUFW_IMAGE", "AES_KEY_FILE", "FAMILY_KEY", "PPK_FILE", "PSK_FILE",
  "SPK_FILE", "SSK_FILE", "SPK_SIGNATURE_FILE", "BH_SIGNATURE_FILE",
  "HEADER_SIGNATURE_FILE", "AUTHVALUE", "ENCRVALUE", "CHECKSUMVALUE",
  "POWNERVALUE", "PTYPEVALUE", "KEY_SRC", "CORE", "BH_RSA", "AUTH_HASH",
  "INT_HASH", "PUFHD_LOC", "OPT_KEY", "AUTH_ONLY", "BOOT_DEVICE_TYPE",
  "DEST_DEVICE_TYPE", "DEST_CPU_TYPE", "EXCEPTION_LEVEL_TYPE",
  "TRUSTZONE_TYPE", "SPLITMODE", "DPA_CM", "SPKSELECT", "OR", "XOR", "AND",
  "MULT", "DIVIDE", "MODULO", "PLUS", "MINUS", "LSHIFT", "RSHIFT",
  "NEGATION", "LPAREN", "RPAREN", "ASTERISK", "$accept", "bif",
  "group_list", "bifoptions", "$@1", "file_list", "metahdr_spec", "$@2",
  "metahdr_attr_list", "metahdr_attr", "metahdr_blk", "metahdr_blk_attr",
  "new_pdi_spec", "image_spec", "image_list", "$@3", "image_content",
  "image_attributes_list", "image_attributes", "partition_spec",
  "partition_content", "other_spec", "$@4", "sec_boot_attr_list",
  "sec_boot_attr", "fsbl_attr_list", "fsbl_attr", "file_spec", "$@5",
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
       140,     0,    -1,   141,    -1,    -1,   141,   142,    -1,    -1,
      69,   143,     7,     3,   144,     4,    -1,    -1,   144,   166,
      -1,   144,   160,    -1,   144,   152,    -1,   144,   145,    -1,
     144,   151,    -1,   144,   168,    -1,   144,   158,    -1,    -1,
      68,     3,   146,   147,     4,    -1,   148,    -1,   148,     5,
     147,    -1,   148,   147,    -1,    -1,    17,     6,   185,    -1,
      75,     6,   189,    -1,    95,     6,   212,    -1,    16,     6,
     184,    -1,    97,     6,   212,    -1,    98,     6,   212,    -1,
      99,     6,   212,    -1,   100,     6,   212,    -1,   101,     6,
     212,    -1,    51,     6,   212,    -1,    39,     6,   220,    -1,
      18,     6,   186,    -1,   123,    -1,    48,     6,   149,    -1,
     114,    -1,   150,    -1,   150,     9,   149,    -1,   220,    -1,
     220,   136,   220,   137,    -1,   220,   136,   138,   137,    -1,
      61,     6,   220,    -1,    56,     6,   220,    -1,    57,     6,
     220,    -1,    58,     6,   220,    -1,   183,     6,   212,    -1,
      75,     6,   189,    -1,   153,    -1,   152,   153,    -1,    -1,
      60,     3,   154,   155,     4,    -1,    -1,   155,   156,    -1,
     155,   166,    -1,   155,   158,    -1,   157,    -1,   157,     5,
     156,    -1,   157,   156,    -1,    61,     6,   220,    -1,    62,
       6,    69,    -1,    63,    -1,    64,    -1,    65,     6,   220,
      -1,    20,     6,   188,    -1,    66,   159,    -1,   159,    -1,
      -1,   159,   166,    -1,   159,   168,    -1,    10,    75,    11,
     189,    -1,    -1,    10,    76,   161,    11,   164,    -1,    76,
       3,   164,     4,    -1,    22,     3,   162,     4,    -1,    10,
      22,    11,   198,    -1,    22,     6,   198,    -1,    10,    77,
      11,   207,    -1,    10,    80,    11,   209,    -1,    10,    50,
      11,   176,    -1,   163,    -1,   163,     5,   162,    -1,   198,
      -1,    25,     6,   220,    -1,   165,    -1,   165,     5,   164,
      -1,   190,    -1,   191,    -1,   193,    -1,   194,    -1,   195,
      -1,   197,    -1,   196,    -1,    78,    -1,    79,     6,   220,
      -1,   192,    -1,    81,     6,   220,    -1,    59,    -1,    -1,
      10,   167,   172,    11,   212,    -1,   212,    -1,    -1,     3,
     169,   170,     4,    -1,   173,    -1,   171,    -1,   171,     5,
     170,    -1,   173,     5,   170,    -1,   171,   170,    -1,   173,
     170,    -1,    67,     6,   212,    -1,    61,     6,   220,    -1,
      20,     6,   179,    -1,    20,     6,    90,    -1,   173,    -1,
     173,     5,   172,    -1,   179,    -1,   182,    -1,   202,    -1,
     203,    -1,   174,    -1,    48,     6,   175,    -1,    49,     6,
     178,    -1,   181,    -1,   181,     9,   175,    -1,   177,    -1,
     177,     5,   176,    -1,   220,    -1,   220,    -1,    12,    -1,
      42,    -1,    43,    -1,    44,    -1,    45,    -1,    46,    -1,
      47,    -1,    14,    -1,    15,    -1,    13,    -1,    93,    -1,
      91,    -1,   204,    -1,   206,    -1,   188,    -1,    27,    -1,
      27,     6,   121,    -1,   220,    -1,   220,   136,   220,   137,
      -1,   220,   136,   138,   137,    -1,    16,     6,   184,    -1,
      17,     6,   185,    -1,    18,     6,   186,    -1,    19,     6,
     187,    -1,    24,     6,   199,    -1,    23,     6,   200,    -1,
      26,     6,   201,    -1,    95,     6,   212,    -1,    97,     6,
     212,    -1,    98,     6,   212,    -1,    99,     6,   212,    -1,
     100,     6,   212,    -1,    37,     6,   208,    -1,    36,     6,
     220,    -1,   101,     6,   212,    -1,   180,    -1,    20,     6,
     188,    -1,    75,     6,   189,    -1,    39,     6,   220,    -1,
     123,    -1,    55,     6,   220,    -1,   114,    -1,    93,    -1,
     204,    -1,    85,    -1,    86,    -1,    87,    -1,    88,    -1,
      89,    -1,    72,    -1,   104,    -1,    72,    -1,   105,    -1,
      72,    -1,   106,    -1,   107,    -1,   108,    -1,   109,    -1,
     110,    -1,   111,    -1,   123,    -1,   112,    -1,   113,    -1,
     114,    -1,   115,    -1,   116,    -1,   117,    -1,   119,    -1,
     118,    -1,   120,    -1,    28,     6,   220,    -1,    29,     6,
     220,    -1,    30,     6,   220,    -1,    31,     6,   220,    -1,
      32,     6,   220,    -1,    33,    -1,    34,    -1,    21,     6,
     220,    -1,    51,     6,   212,    -1,    52,     6,   212,    -1,
      95,    -1,   205,    -1,   101,    -1,   102,    -1,   103,    -1,
      83,    -1,    82,    -1,    84,    -1,    96,    -1,    97,    -1,
      98,    -1,    99,    -1,   100,    -1,    94,    -1,    90,    -1,
      92,    -1,    35,     6,   220,    -1,    37,     6,   208,    -1,
      36,     6,   220,    -1,    38,    -1,   207,     9,   207,    -1,
     124,    -1,    40,     6,   210,    -1,    41,     6,   211,    -1,
     209,     5,   209,    -1,   122,    -1,    53,    -1,    54,    -1,
      70,    -1,    71,    -1,    74,    -1,    73,    -1,   136,   220,
     137,    -1,   213,    -1,   131,   214,    -1,   135,   214,    -1,
     214,    -1,   215,   128,   214,    -1,   215,   129,   214,    -1,
     215,   130,   214,    -1,   215,    -1,   216,   131,   215,    -1,
     216,   132,   215,    -1,   216,    -1,   217,   133,   216,    -1,
     217,   134,   216,    -1,   217,    -1,   218,   127,   217,    -1,
     218,    -1,   219,   126,   218,    -1,   219,    -1,   220,   125,
     219,    -1
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
     193,   195,   197,   201,   205,   208,   210,   211,   214,   217,
     222,   223,   229,   234,   239,   244,   248,   253,   258,   263,
     265,   269,   271,   275,   277,   281,   283,   285,   287,   289,
     291,   293,   295,   297,   301,   303,   307,   309,   310,   316,
     318,   319,   324,   326,   328,   332,   336,   339,   342,   346,
     350,   354,   358,   360,   364,   366,   368,   370,   372,   374,
     378,   382,   384,   388,   390,   394,   396,   398,   400,   402,
     404,   406,   408,   410,   412,   414,   416,   418,   420,   422,
     424,   426,   428,   430,   434,   436,   441,   446,   450,   454,
     458,   462,   466,   470,   474,   478,   482,   486,   490,   494,
     498,   502,   506,   508,   512,   516,   520,   522,   526,   528,
     530,   532,   534,   536,   538,   540,   542,   544,   546,   548,
     550,   552,   554,   556,   558,   560,   562,   564,   566,   568,
     570,   572,   574,   576,   578,   580,   582,   584,   588,   592,
     596,   600,   604,   606,   608,   612,   616,   620,   622,   624,
     626,   628,   630,   632,   634,   636,   638,   640,   642,   644,
     646,   648,   650,   652,   656,   660,   664,   666,   670,   672,
     676,   680,   684,   686,   688,   690,   692,   694,   696,   698,
     702,   704,   707,   710,   712,   716,   720,   724,   726,   730,
     734,   736,   740,   744,   746,   750,   752,   756,   758
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   195,   195,   197,   198,   201,   201,   206,   207,   208,
     209,   210,   211,   212,   213,   216,   216,   221,   222,   223,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   238,   239,   240,   241,   245,   246,   249,   251,
     253,   257,   258,   259,   260,   261,   262,   265,   266,   269,
     269,   273,   274,   275,   276,   279,   280,   281,   284,   285,
     286,   287,   288,   289,   293,   294,   297,   298,   299,   302,
     305,   305,   308,   309,   310,   315,   316,   317,   318,   321,
     322,   325,   326,   329,   330,   333,   336,   338,   340,   342,
     344,   346,   348,   350,   352,   358,   362,   365,   365,   371,
     376,   376,   382,   383,   384,   385,   386,   387,   390,   392,
     393,   394,   397,   398,   401,   402,   403,   404,   405,   408,
     409,   412,   413,   416,   417,   420,   425,   427,   428,   429,
     430,   431,   432,   433,   434,   435,   436,   439,   440,   441,
     442,   443,   446,   447,   450,   451,   452,   454,   455,   456,
     457,   458,   459,   460,   461,   462,   463,   464,   465,   466,
     467,   468,   469,   470,   471,   472,   473,   474,   475,   478,
     479,   480,   481,   482,   483,   484,   487,   488,   491,   492,
     495,   496,   499,   502,   505,   508,   511,   514,   517,   520,
     523,   526,   529,   532,   535,   538,   541,   544,   545,   546,
     547,   548,   549,   550,   551,   554,   555,   558,   559,   560,
     561,   562,   563,   564,   565,   566,   569,   570,   571,   572,
     575,   576,   577,   580,   583,   586,   589,   592,   595,   598,
     601,   602,   605,   608,   611,   617,   618,   621,   622,   623,
     629,   630,   631,   635,   636,   637,   638,   641,   642,   643,
     646,   647,   648,   651,   652,   655,   656,   660,   661
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
     135,   136,   137,   138
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 883;
  const int BisonParser::yynnts_ = 82;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 139;

  const unsigned int BisonParser::yyuser_token_number_max_ = 393;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 23 "../s/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 2293 "../bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 666 "../s/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
