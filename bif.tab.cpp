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
    { options.bifOptions = currentBifOptions;
                                                                                  options.bifOptionsList.push_back(currentBifOptions); }
    break;

  case 15:
/* Line 670 of lalr1.cc  */
#line 217 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                   currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 21:
/* Line 670 of lalr1.cc  */
#line 228 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 229 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 230 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.encrKeyFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 231 "../s/bif.y"
    { currentBifOptions->SetMetaHeaderAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 232 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ppk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 233 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.psk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 234 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 235 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.ssk = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 236 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.spkSignature = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 237 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.presign = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 238 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.revokeId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 239 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.checksum = (yysemantic_stack_[(3) - (3)].checksumvalue_t); }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 240 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.dpaCM = DpaCM::DpaCMEnable; }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 242 "../s/bif.y"
    { currentBifOptions->metaHdrAttributes.pufHdLoc = PufHdLoc::PUFinBH; 
                                                                                  currentBifOptions->SetPufHdinBHFlag();}
    break;

  case 38:
/* Line 670 of lalr1.cc  */
#line 250 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 39:
/* Line 670 of lalr1.cc  */
#line 252 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); 
                                                                                  currentBifOptions->metaHdrAttributes.encrBlocks = currentPartitionBifOptions->GetEncryptionBlocks(); }
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 254 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); 
                                                                                  currentBifOptions->metaHdrAttributes.defEncrBlockSize = (yysemantic_stack_[(4) - (1)].number); }
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 258 "../s/bif.y"
    { currentBifOptions->SetPdiId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 259 "../s/bif.y"
    { currentBifOptions->SetParentId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 260 "../s/bif.y"
    { currentBifOptions->SetIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 261 "../s/bif.y"
    { currentBifOptions->SetExtendedIdCode((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 262 "../s/bif.y"
    { currentBifOptions->AddFiles((yysemantic_stack_[(3) - (1)].number), (yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 263 "../s/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 270 "../s/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 271 "../s/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions); }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 285 "../s/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 59:
/* Line 670 of lalr1.cc  */
#line 286 "../s/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 287 "../s/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 288 "../s/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 289 "../s/bif.y"
    { currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 290 "../s/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 291 "../s/bif.y"
    { currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 292 "../s/bif.y"
    { currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 293 "../s/bif.y"
    { currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 72:
/* Line 670 of lalr1.cc  */
#line 306 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported in VERSAL architecture.\n\t   Please see 'bootgen -arch versal -bif_help keysrc'");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 309 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 77:
/* Line 670 of lalr1.cc  */
#line 314 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                    if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); 
                                                                                    currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 78:
/* Line 670 of lalr1.cc  */
#line 319 "../s/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); }
    break;

  case 84:
/* Line 670 of lalr1.cc  */
#line 329 "../s/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 85:
/* Line 670 of lalr1.cc  */
#line 330 "../s/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 88:
/* Line 670 of lalr1.cc  */
#line 337 "../s/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 89:
/* Line 670 of lalr1.cc  */
#line 340 "../s/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 90:
/* Line 670 of lalr1.cc  */
#line 342 "../s/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 91:
/* Line 670 of lalr1.cc  */
#line 344 "../s/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 92:
/* Line 670 of lalr1.cc  */
#line 346 "../s/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 93:
/* Line 670 of lalr1.cc  */
#line 348 "../s/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 94:
/* Line 670 of lalr1.cc  */
#line 350 "../s/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 95:
/* Line 670 of lalr1.cc  */
#line 352 "../s/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 96:
/* Line 670 of lalr1.cc  */
#line 354 "../s/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 97:
/* Line 670 of lalr1.cc  */
#line 356 "../s/bif.y"
    {  if(options.GetArchType() != Arch::VERSAL)
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t\t'dpacm_enable' is supported only in VERSAL architecture");
                                                                                   if(options.GetArchType() == Arch::VERSAL)
                                                                                     LOG_WARNING("boot_config { dpacm_enable } will be deprecated. 'dpacm_enable' should be specified along with the partition. \n            See 'bootgen -bif_help dpacm_enable' for more info.");
                                                                                   currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));
                                                                                }
    break;

  case 98:
/* Line 670 of lalr1.cc  */
#line 362 "../s/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->smapWidth = (yysemantic_stack_[(3) - (3)].number);
                                                                                }
    break;

  case 99:
/* Line 670 of lalr1.cc  */
#line 366 "../s/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 367 "../s/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 368 "../s/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 371 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 103:
/* Line 670 of lalr1.cc  */
#line 374 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 378 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 384 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 398 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 401 "../s/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 403 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 117:
/* Line 670 of lalr1.cc  */
#line 404 "../s/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 432 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] only supported in ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 437 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 133:
/* Line 670 of lalr1.cc  */
#line 439 "../s/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 134:
/* Line 670 of lalr1.cc  */
#line 440 "../s/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 135:
/* Line 670 of lalr1.cc  */
#line 441 "../s/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 136:
/* Line 670 of lalr1.cc  */
#line 442 "../s/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 137:
/* Line 670 of lalr1.cc  */
#line 443 "../s/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 138:
/* Line 670 of lalr1.cc  */
#line 444 "../s/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 445 "../s/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 446 "../s/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 447 "../s/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 448 "../s/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 451 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 452 "../s/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 453 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 454 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 455 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 458 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 459 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 150:
/* Line 670 of lalr1.cc  */
#line 462 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 151:
/* Line 670 of lalr1.cc  */
#line 463 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 152:
/* Line 670 of lalr1.cc  */
#line 464 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 153:
/* Line 670 of lalr1.cc  */
#line 466 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 467 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 468 "../s/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 469 "../s/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 157:
/* Line 670 of lalr1.cc  */
#line 470 "../s/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 471 "../s/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 472 "../s/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 473 "../s/bif.y"
    { currentPartitionBifOptions->aesKeyFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 161:
/* Line 670 of lalr1.cc  */
#line 474 "../s/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 475 "../s/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 476 "../s/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 477 "../s/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 478 "../s/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 479 "../s/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 480 "../s/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 482 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 483 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 484 "../s/bif.y"
    { currentPartitionBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 485 "../s/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 486 "../s/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 487 "../s/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 499 "../s/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 503 "../s/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 507 "../s/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 203:
/* Line 670 of lalr1.cc  */
#line 556 "../s/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 204:
/* Line 670 of lalr1.cc  */
#line 557 "../s/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 205:
/* Line 670 of lalr1.cc  */
#line 558 "../s/bif.y"
    { currentPartitionBifOptions->reserve = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 206:
/* Line 670 of lalr1.cc  */
#line 559 "../s/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 207:
/* Line 670 of lalr1.cc  */
#line 560 "../s/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 208:
/* Line 670 of lalr1.cc  */
#line 561 "../s/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 209:
/* Line 670 of lalr1.cc  */
#line 562 "../s/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 210:
/* Line 670 of lalr1.cc  */
#line 563 "../s/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 211:
/* Line 670 of lalr1.cc  */
#line 566 "../s/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 212:
/* Line 670 of lalr1.cc  */
#line 567 "../s/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 229:
/* Line 670 of lalr1.cc  */
#line 592 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 230:
/* Line 670 of lalr1.cc  */
#line 595 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 231:
/* Line 670 of lalr1.cc  */
#line 598 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 232:
/* Line 670 of lalr1.cc  */
#line 601 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 235:
/* Line 670 of lalr1.cc  */
#line 610 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 239:
/* Line 670 of lalr1.cc  */
#line 620 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 240:
/* Line 670 of lalr1.cc  */
#line 623 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 245:
/* Line 670 of lalr1.cc  */
#line 635 "../s/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 247:
/* Line 670 of lalr1.cc  */
#line 642 "../s/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 248:
/* Line 670 of lalr1.cc  */
#line 643 "../s/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 250:
/* Line 670 of lalr1.cc  */
#line 648 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 251:
/* Line 670 of lalr1.cc  */
#line 649 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 252:
/* Line 670 of lalr1.cc  */
#line 650 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 254:
/* Line 670 of lalr1.cc  */
#line 654 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 255:
/* Line 670 of lalr1.cc  */
#line 655 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 257:
/* Line 670 of lalr1.cc  */
#line 659 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 258:
/* Line 670 of lalr1.cc  */
#line 660 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 260:
/* Line 670 of lalr1.cc  */
#line 664 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 262:
/* Line 670 of lalr1.cc  */
#line 668 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 264:
/* Line 670 of lalr1.cc  */
#line 673 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1351 "../bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -373;
  const short int
  BisonParser::yypact_[] =
  {
      -373,    17,    -7,  -373,  -373,  -373,    42,    69,  -373,   674,
    -373,  -373,   198,     8,    91,    95,   120,    72,   136,  -373,
     152,  -373,  -373,   159,   165,  -373,  -373,  -373,  -373,  -373,
    -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,
    -373,  -373,  -373,  -373,  -373,   104,  -373,  -373,     6,  -373,
    -373,  -373,   171,  -373,  -373,  -373,   498,   179,   182,   187,
    -373,   188,   189,   616,   -17,    79,   -57,   -57,   -57,  -373,
     -57,     6,  -373,    89,   232,  -373,  -373,  -373,  -373,    16,
    -373,  -373,  -373,  -373,   199,   200,   201,   202,   205,   206,
     208,   209,   210,   213,   216,   217,   218,   219,   220,  -373,
    -373,   221,   224,   225,  -373,  -373,  -373,  -373,  -373,  -373,
     229,   236,   238,   239,   240,   244,   249,   252,   253,  -373,
    -373,  -373,  -373,  -373,   254,   255,   256,   258,   266,   272,
    -373,  -373,  -373,   279,    14,   380,  -373,  -373,  -373,  -373,
    -373,  -373,  -373,  -373,  -373,    79,   -57,    89,   276,   110,
      59,   282,   278,   290,   291,  -373,   292,   293,  -373,  -373,
    -373,  -373,   -57,   -57,   -57,  -373,  -373,    25,   -29,   -10,
     185,   180,   172,   172,   172,  -373,   172,   170,  -373,  -373,
    -373,  -373,  -373,  -373,   309,   314,  -373,  -373,  -373,  -373,
    -373,  -373,  -373,  -373,   317,   318,  -373,  -373,  -373,  -373,
    -373,  -373,  -373,  -373,  -373,   -39,   -42,   -26,   211,   745,
     -57,   203,   197,   204,   235,   -57,   -57,   -57,   -57,   -57,
     -57,   214,   -57,   -57,   -57,    16,   265,    16,   -57,   -57,
      16,    89,    16,    16,    16,    16,    16,    16,  -373,   498,
    -373,   498,  -373,  -373,  -373,   323,   172,  -373,   232,   325,
     327,   337,  -373,   320,   339,   340,   342,   245,    16,   616,
     -57,  -373,   -17,  -373,  -373,  -119,   -57,   -57,   -57,   -57,
     -57,   -57,   -57,   -57,   -57,   -57,   237,   357,   358,   359,
     360,   361,   363,   364,   365,   368,   370,   371,   372,   373,
    -373,  -373,   376,   162,   -57,   -57,  -373,   232,  -373,  -373,
    -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,
    -373,  -373,   172,  -373,  -373,  -373,  -373,  -373,  -373,  -373,
     172,   172,   172,   172,   172,   172,  -373,  -373,   172,  -373,
     374,   -37,  -373,   172,  -373,  -373,  -373,   172,   172,  -373,
    -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,   -57,
    -373,   -57,   -57,   214,   110,   259,    86,    59,  -373,  -373,
    -373,   172,  -373,  -373,  -373,  -373,  -373,    25,    25,   -29,
     -29,   -10,   185,   180,  -373,   375,   378,   382,   383,   396,
     399,  -373,  -373,   409,  -373,    68,  -373,  -373,   -39,   -42,
     -26,   -57,   -57,    16,    89,    16,    16,    16,    16,    16,
      16,  -373,   170,  -373,   172,   172,  -373,   -57,   112,  -373,
     172,   172,  -373,   320,  -373,  -373,  -373,  -373,  -373,   342,
     245,   -57,   -57,   -57,   -57,   307,   -57,   169,  -373,  -373,
    -373,  -373,   172,  -373,   377,   -35,  -373,  -373,  -373,  -373,
    -373,  -373,  -373,  -373,  -373,  -373,   275,   -67,  -373,   172,
     172,   172,   172,  -373,   172,  -373,   -57,   231,  -373,  -373,
    -373,   277,   -52,  -373,  -373
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     5,     4,     0,     0,     7,     0,
     105,     6,   102,     0,     0,     0,     0,     0,     0,    69,
       0,   241,   242,     0,     0,   219,   218,   220,   177,   178,
     179,   180,   181,   175,   213,   221,   222,   223,   224,   225,
     215,   216,   217,    11,    12,    10,    47,    14,    68,     9,
       8,    13,     0,   176,   214,   104,     0,     0,     0,     0,
      73,     0,     0,     0,     0,     0,     0,     0,     0,    49,
       0,    67,    15,     0,     0,    48,   102,    70,    71,     0,
     133,   142,   140,   141,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   148,     0,     0,     0,     0,     0,   208,
     209,     0,     0,     0,   134,   135,   136,   137,   138,   139,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   227,
     144,   228,   143,   226,   213,   222,   223,   224,   225,   215,
     189,   174,   172,     0,   108,   107,   124,   120,   168,   121,
     147,   122,   123,   145,   146,     0,     0,     0,     0,     0,
       0,     0,     0,   118,     0,   199,     0,    82,    84,    78,
     244,   243,     0,     0,     0,   246,   249,   253,   256,   259,
     261,   263,    42,    43,    44,    51,    41,    20,   190,    46,
      99,   100,   101,    95,     0,     0,   191,   192,   194,   195,
     196,   197,   198,   193,     0,    86,    88,    89,    97,    90,
      91,    92,    94,    93,    45,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   106,     0,
     111,     0,   112,    77,    81,   129,   131,    72,     0,     0,
       0,     0,   232,    79,     0,     0,    80,     0,     0,     0,
       0,    76,     0,   247,   248,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    69,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      35,    33,     0,    17,     0,     0,    75,     0,   182,   183,
     153,   184,   185,   154,   186,   187,   155,   188,   156,   116,
     115,   147,   210,   201,   158,   200,   157,   202,   159,   149,
     203,   204,   205,   206,   207,   166,   234,   165,   171,   125,
     127,   150,   126,   132,   211,   117,   212,   173,   114,   113,
     170,   160,   161,   162,   163,   164,   167,   109,   110,     0,
      74,     0,     0,     0,     0,     0,     0,     0,   169,   103,
     119,    85,    83,   245,   250,   251,   252,   254,   255,   257,
     258,   260,   262,   264,    50,     0,     0,     0,     0,     0,
       0,    60,    61,     0,    52,    55,    54,    53,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    16,    20,    19,    96,    98,    87,     0,     0,   130,
     229,   231,   230,   233,   238,   235,   239,   240,   236,   237,
       0,     0,     0,     0,     0,     0,     0,     0,    57,    24,
      21,    32,    31,    34,    36,    38,    30,    22,    23,    25,
      26,    27,    28,    29,    18,   128,     0,     0,    63,    64,
      65,    66,    58,    59,    62,    56,     0,     0,   152,   151,
      37,     0,     0,    40,    39
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,  -278,  -373,
     -22,  -373,  -373,  -373,   385,  -373,  -373,  -372,  -373,   145,
     416,  -373,  -373,   175,  -373,  -223,  -373,    -6,  -373,   429,
    -373,  -114,  -373,   181,   -56,  -373,    32,    92,  -373,  -373,
     233,  -373,  -373,  -373,  -373,    55,    56,    54,  -373,  -203,
    -142,  -373,  -373,  -373,  -373,  -373,  -373,  -373,  -373,   -55,
    -373,  -373,  -373,  -373,  -373,   437,  -373,  -373,    94,    96,
      93,  -373,  -373,   -61,  -373,   -85,   -74,   -87,   178,   183,
     177,   -66
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     5,     6,     9,    43,   177,   292,   293,
     433,   434,    44,    45,    46,   175,   276,   384,   385,    47,
      48,    49,   148,   156,   157,   194,   195,    77,    63,    78,
      56,   133,   134,   152,   135,   136,   329,   244,   245,   332,
     137,   138,   330,   139,    52,   300,   303,   306,   308,   140,
     179,   196,   197,   198,   199,   200,   201,   202,   203,   158,
     316,   314,   318,   141,   142,   143,    54,   144,   253,   327,
     256,   415,   418,    55,   165,   166,   167,   168,   169,   170,
     171,   246
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       172,   173,   174,    50,   176,   247,   311,   153,   154,    10,
     159,    64,   275,   428,    65,   403,    76,     3,   204,   239,
     240,   242,   160,   161,   363,   350,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,   301,    90,    91,   298,
      92,    93,    94,    95,    96,    97,    98,    99,   100,     7,
     101,   102,   304,   103,   358,   455,   104,   105,   106,   107,
     108,   109,   110,   111,   275,   112,   113,   114,     4,   302,
     115,   299,     8,   427,   406,    69,   459,   263,   264,   275,
     162,   116,    21,    22,   163,   164,   305,   117,   375,   340,
     243,   464,    21,    22,   275,   118,   275,    66,   265,   254,
     255,    67,    25,    26,    27,   408,   155,   457,   269,   270,
     119,   120,   121,   122,   123,   124,    35,   125,   126,   127,
     128,   129,    41,    42,   444,   347,    68,   348,   130,   271,
     272,   376,   377,   378,   131,   379,   380,   381,   382,   383,
     416,   417,    70,   132,   312,   249,   250,   251,   252,   320,
     321,   322,   323,   324,   325,    72,   328,   331,   333,   266,
     267,   268,   337,   338,   334,    73,   336,   402,    74,   339,
      17,   341,   342,   343,   344,   345,   346,    79,   277,   278,
     279,   364,   365,   366,   369,   370,   277,   278,   279,   375,
     145,   160,   161,   146,   361,   367,   368,   359,   147,   149,
     150,   280,   155,   153,   178,   205,   206,   207,   208,   280,
     281,   209,   210,   282,   211,   212,   213,   448,   281,   214,
      57,   282,   215,   216,   217,   218,   219,   220,   404,   405,
     221,   222,   376,   377,   378,   223,   379,   380,   381,   382,
     383,   374,   224,   283,   225,   226,   227,    76,    58,   162,
     228,   283,   437,   163,   164,   229,   446,   375,   230,   231,
     232,   233,   234,   284,   235,   285,   286,   287,   288,   289,
     387,   284,   236,   285,   286,   287,   288,   289,   237,    59,
      60,    61,   290,   238,    62,   410,   411,   248,   257,   258,
     290,   291,   180,   181,   182,   259,   261,   260,   262,   291,
     376,   377,   378,   275,   379,   380,   381,   382,   383,    19,
     160,   161,   274,    21,    22,   294,   183,   184,   273,   185,
     295,   296,   315,   297,   307,   432,   435,   313,   349,   354,
     317,   351,   436,   352,   438,   439,   440,   441,   442,   443,
     335,   331,   447,   353,   326,   355,   356,   357,   186,   187,
     188,   189,   190,   191,   192,   449,   450,   451,   452,   130,
     454,   193,   319,   388,   389,   390,   391,   392,   162,   393,
     394,   395,   163,   164,   396,   461,   397,   398,   399,   400,
     401,   420,   453,   407,   421,   241,   456,   414,   422,   423,
     435,   462,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,   424,    90,    91,   425,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   426,   101,   102,   458,   103,
     463,   386,   104,   105,   106,   107,   108,   109,   110,   111,
      75,   112,   113,   114,   460,    71,   115,   362,    51,   445,
     360,   409,   310,   429,   431,   430,    53,   116,   413,   412,
     419,   371,   373,   117,     0,     0,     0,   372,     0,     0,
       0,   118,     0,     0,     0,     0,     0,     0,    25,    26,
      27,     0,     0,     0,     0,     0,   119,   120,   121,   122,
     123,   124,    35,   125,   126,   127,   128,   129,    41,    42,
       0,     0,     0,     0,   130,     0,     0,     0,     0,     0,
     131,     0,     0,     0,     0,     0,     0,     0,     0,   132,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
       0,    90,    91,     0,    92,    93,    94,    95,    96,    97,
      98,    99,   100,     0,   101,   102,     0,   103,     0,     0,
     104,   105,   106,   107,   108,   109,   110,   111,     0,   112,
     113,   114,     0,     0,   115,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   116,     0,     0,     0,     0,
       0,   117,     0,     0,     0,     0,     0,     0,     0,   118,
       0,     0,     0,     0,     0,     0,    25,    26,    27,     0,
       0,     0,     0,     0,   119,   120,   121,   122,   123,   124,
      35,   125,   126,   127,   128,   129,    41,    42,     0,     0,
       0,     0,   130,     0,     0,     0,     0,     0,   131,     0,
       0,     0,     0,     0,     0,     0,     0,   132,    80,    81,
      82,    83,    84,    85,    86,    87,   151,    89,     0,    90,
      91,     0,    92,    93,    94,    95,    96,    97,    98,    99,
     100,     0,   101,   102,     0,   103,     0,     0,   104,   105,
     106,   107,   108,   109,   110,   111,     0,   112,     0,   114,
       0,     0,   115,     0,     0,     0,     0,    10,    11,     0,
       0,     0,     0,     0,    12,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    13,   118,     0,     0,
       0,     0,     0,     0,    25,    26,    27,     0,     0,     0,
       0,     0,   119,   120,   121,   122,   123,   124,    35,   125,
     126,   127,   128,   129,    41,    42,     0,     0,     0,     0,
     130,    14,    15,    16,     0,     0,   131,     0,     0,     0,
      17,    18,     0,     0,     0,   132,    19,     0,    20,     0,
      21,    22,     0,     0,     0,    23,    24,    80,    81,    82,
      83,     0,    25,    26,    27,    28,    29,    30,    31,    32,
       0,     0,     0,    33,     0,    34,    35,    36,    37,    38,
      39,    40,    41,    42,     0,     0,     0,   104,   105,   106,
     107,   108,   109,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    25,    26,    27,     0,     0,     0,     0,
       0,   309,   120,   121,   122,   123,    34,    35,    36,    37,
      38,    39,    40,    41,    42,     0,     0,     0,     0,   130
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        66,    67,    68,     9,    70,   147,   209,    63,    25,     3,
      65,     3,   131,   385,     6,   293,    10,     0,    79,     5,
     134,   135,    79,    80,   143,   248,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    78,    23,    24,    78,
      26,    27,    28,    29,    30,    31,    32,    33,    34,     7,
      36,    37,    78,    39,   257,   427,    42,    43,    44,    45,
      46,    47,    48,    49,   131,    51,    52,    53,    75,   111,
      56,   110,     3,     5,   297,     3,   143,   162,   163,   131,
     137,    67,    76,    77,   141,   142,   112,    73,    20,   231,
     145,   143,    76,    77,   131,    81,   131,     6,   164,    40,
      41,     6,    88,    89,    90,   142,   123,   142,   137,   138,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   402,   239,     6,   241,   114,   139,
     140,    63,    64,    65,   120,    67,    68,    69,    70,    71,
      54,    55,     6,   129,   210,    35,    36,    37,    38,   215,
     216,   217,   218,   219,   220,     3,   222,   223,   224,   134,
     135,   136,   228,   229,   225,     6,   227,     5,     3,   230,
      66,   232,   233,   234,   235,   236,   237,     6,    16,    17,
      18,   266,   267,   268,   271,   272,    16,    17,    18,    20,
      11,    79,    80,    11,   260,   269,   270,   258,    11,    11,
      11,    39,   123,   259,   115,     6,     6,     6,     6,    39,
      48,     6,     6,    51,     6,     6,     6,   420,    48,     6,
      22,    51,     6,     6,     6,     6,     6,     6,   294,   295,
       6,     6,    63,    64,    65,     6,    67,    68,    69,    70,
      71,     4,     6,    81,     6,     6,     6,    10,    50,   137,
       6,    81,   394,   141,   142,     6,   144,    20,     6,     6,
       6,     6,     6,   101,     6,   103,   104,   105,   106,   107,
     276,   101,     6,   103,   104,   105,   106,   107,     6,    81,
      82,    83,   120,     4,    86,   351,   352,    11,     6,    11,
     120,   129,    60,    61,    62,     5,     4,     6,     5,   129,
      63,    64,    65,   131,    67,    68,    69,    70,    71,    72,
      79,    80,   132,    76,    77,     6,    84,    85,   133,    87,
       6,     4,   125,     5,   113,   391,   392,   124,     5,     9,
     126,     6,   393,     6,   395,   396,   397,   398,   399,   400,
      75,   407,   408,     6,   130,     6,     6,     5,   116,   117,
     118,   119,   120,   121,   122,   421,   422,   423,   424,   114,
     426,   129,   127,     6,     6,     6,     6,     6,   137,     6,
       6,     6,   141,   142,     6,   144,     6,     6,     6,     6,
       4,     6,    75,     9,     6,     5,     9,   128,     6,     6,
     456,   457,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,     6,    23,    24,     6,    26,    27,    28,    29,
      30,    31,    32,    33,    34,     6,    36,    37,   143,    39,
     143,   276,    42,    43,    44,    45,    46,    47,    48,    49,
      45,    51,    52,    53,   456,    19,    56,   262,     9,   407,
     259,   349,   209,   388,   390,   389,     9,    67,   354,   353,
     357,   273,   275,    73,    -1,    -1,    -1,   274,    -1,    -1,
      -1,    81,    -1,    -1,    -1,    -1,    -1,    -1,    88,    89,
      90,    -1,    -1,    -1,    -1,    -1,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
      -1,    -1,    -1,    -1,   114,    -1,    -1,    -1,    -1,    -1,
     120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   129,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    23,    24,    -1,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    -1,    36,    37,    -1,    39,    -1,    -1,
      42,    43,    44,    45,    46,    47,    48,    49,    -1,    51,
      52,    53,    -1,    -1,    56,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    67,    -1,    -1,    -1,    -1,
      -1,    73,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    81,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    90,    -1,
      -1,    -1,    -1,    -1,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,    -1,    -1,
      -1,    -1,   114,    -1,    -1,    -1,    -1,    -1,   120,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   129,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    -1,    23,
      24,    -1,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    -1,    36,    37,    -1,    39,    -1,    -1,    42,    43,
      44,    45,    46,    47,    48,    49,    -1,    51,    -1,    53,
      -1,    -1,    56,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,    -1,    -1,    -1,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    22,    81,    -1,    -1,
      -1,    -1,    -1,    -1,    88,    89,    90,    -1,    -1,    -1,
      -1,    -1,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,    -1,    -1,    -1,    -1,
     114,    57,    58,    59,    -1,    -1,   120,    -1,    -1,    -1,
      66,    67,    -1,    -1,    -1,   129,    72,    -1,    74,    -1,
      76,    77,    -1,    -1,    -1,    81,    82,    12,    13,    14,
      15,    -1,    88,    89,    90,    91,    92,    93,    94,    95,
      -1,    -1,    -1,    99,    -1,   101,   102,   103,   104,   105,
     106,   107,   108,   109,    -1,    -1,    -1,    42,    43,    44,
      45,    46,    47,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    88,    89,    90,    -1,    -1,    -1,    -1,
      -1,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,    -1,    -1,    -1,    -1,   114
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   146,   147,     0,    75,   148,   149,     7,     3,   150,
       3,     4,    10,    22,    57,    58,    59,    66,    67,    72,
      74,    76,    77,    81,    82,    88,    89,    90,    91,    92,
      93,    94,    95,    99,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   151,   157,   158,   159,   164,   165,   166,
     172,   174,   189,   210,   211,   218,   175,    22,    50,    81,
      82,    83,    86,   173,     3,     6,     6,     6,     6,     3,
       6,   165,     3,     6,     3,   159,    10,   172,   174,     6,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      23,    24,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    36,    37,    39,    42,    43,    44,    45,    46,    47,
      48,    49,    51,    52,    53,    56,    67,    73,    81,    96,
      97,    98,    99,   100,   101,   103,   104,   105,   106,   107,
     114,   120,   129,   176,   177,   179,   180,   185,   186,   188,
     194,   208,   209,   210,   212,    11,    11,    11,   167,    11,
      11,    20,   178,   179,    25,   123,   168,   169,   204,   204,
      79,    80,   137,   141,   142,   219,   220,   221,   222,   223,
     224,   225,   226,   226,   226,   160,   226,   152,   115,   195,
      60,    61,    62,    84,    85,    87,   116,   117,   118,   119,
     120,   121,   122,   129,   170,   171,   196,   197,   198,   199,
     200,   201,   202,   203,   218,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     4,     5,
     176,     5,   176,   204,   182,   183,   226,   195,    11,    35,
      36,    37,    38,   213,    40,    41,   215,     6,    11,     5,
       6,     4,     5,   220,   220,   226,   134,   135,   136,   137,
     138,   139,   140,   133,   132,   131,   161,    16,    17,    18,
      39,    48,    51,    81,   101,   103,   104,   105,   106,   107,
     120,   129,   153,   154,     6,     6,     4,     5,    78,   110,
     190,    78,   111,   191,    78,   112,   192,   113,   193,    96,
     185,   194,   226,   124,   206,   125,   205,   126,   207,   127,
     226,   226,   226,   226,   226,   226,   130,   214,   226,   181,
     187,   226,   184,   226,   218,    75,   218,   226,   226,   218,
     195,   218,   218,   218,   218,   218,   218,   176,   176,     5,
     170,     6,     6,     6,     9,     6,     6,     5,   194,   218,
     178,   226,   168,   143,   220,   220,   220,   221,   221,   222,
     222,   223,   224,   225,     4,    20,    63,    64,    65,    67,
      68,    69,    70,    71,   162,   163,   164,   172,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     4,     5,   153,   226,   226,   170,     9,   142,   182,
     226,   226,   214,   213,   128,   216,    54,    55,   217,   215,
       6,     6,     6,     6,     6,     6,     6,     5,   162,   190,
     191,   192,   226,   155,   156,   226,   218,   195,   218,   218,
     218,   218,   218,   218,   153,   181,   144,   226,   194,   226,
     226,   226,   226,    75,   226,   162,     9,   142,   143,   143,
     155,   144,   226,   143,   143
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
     395,   396,   397,   398,   399
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   145,   146,   147,   147,   149,   148,   150,   150,   150,
     150,   150,   150,   150,   150,   152,   151,   153,   153,   153,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   155,   155,   156,   156,
     156,   157,   157,   157,   157,   157,   157,   158,   158,   160,
     159,   161,   161,   161,   161,   162,   162,   162,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   164,   164,   165,
     165,   165,   166,   167,   166,   166,   166,   166,   166,   166,
     166,   166,   168,   168,   169,   169,   170,   170,   171,   171,
     171,   171,   171,   171,   171,   171,   171,   171,   171,   171,
     171,   171,   173,   172,   172,   175,   174,   176,   176,   176,
     176,   176,   176,   177,   177,   177,   177,   177,   178,   178,
     179,   179,   179,   179,   179,   180,   180,   181,   181,   182,
     182,   183,   184,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   186,   186,
     187,   187,   187,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188,
     188,   188,   188,   188,   188,   189,   189,   189,   189,   189,
     189,   189,   190,   190,   191,   191,   192,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   208,   208,   208,   208,   208,   208,
     208,   209,   209,   210,   210,   210,   210,   210,   210,   210,
     210,   210,   211,   211,   211,   211,   212,   212,   212,   213,
     213,   213,   213,   213,   214,   215,   215,   215,   216,   217,
     217,   218,   218,   219,   219,   219,   220,   220,   220,   221,
     221,   221,   221,   222,   222,   222,   223,   223,   223,   224,
     224,   225,   225,   226,   226
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
       4,     4,     1,     3,     1,     3,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     3,     1,     3,     1,
       1,     1,     0,     5,     1,     0,     4,     1,     1,     3,
       3,     2,     2,     3,     3,     3,     3,     3,     1,     3,
       1,     1,     1,     1,     1,     3,     3,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     4,     4,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     3,
       3,     3,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     3,     3,     3,     1,     1,
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
  "METAHEADER", "WORD", "FILENAME", "QFILENAME", "NONE", "DECVALUE",
  "HEXVALUE", "KEYSRC_ENCRYPTION", "FSBL_CONFIG", "AUTH_PARAMS",
  "PUF4KMODE", "SHUTTER", "SPLIT", "SMAP_WIDTH", "PUF_HELPER_FILE",
  "BH_KEY_FILE", "BH_KEY_IV", "BH_KEK_IV", "BBRAM_KEK_IV", "EFUSE_KEK_IV",
  "EFUSE_USER_KEK0_IV", "EFUSE_USER_KEK1_IV", "PMCDATA", "BOOTIMAGE",
  "UDF_BH", "INIT", "PMUFW_IMAGE", "AES_KEY_FILE", "FAMILY_KEY",
  "PPK_FILE", "PSK_FILE", "SPK_FILE", "SSK_FILE", "SPK_SIGNATURE_FILE",
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
       146,     0,    -1,   147,    -1,    -1,   147,   148,    -1,    -1,
      75,   149,     7,     3,   150,     4,    -1,    -1,   150,   172,
      -1,   150,   166,    -1,   150,   158,    -1,   150,   151,    -1,
     150,   157,    -1,   150,   174,    -1,   150,   164,    -1,    -1,
      74,     3,   152,   153,     4,    -1,   154,    -1,   154,     5,
     153,    -1,   154,   153,    -1,    -1,    17,     6,   191,    -1,
      81,     6,   195,    -1,   101,     6,   218,    -1,    16,     6,
     190,    -1,   103,     6,   218,    -1,   104,     6,   218,    -1,
     105,     6,   218,    -1,   106,     6,   218,    -1,   107,     6,
     218,    -1,    51,     6,   218,    -1,    39,     6,   226,    -1,
      18,     6,   192,    -1,   129,    -1,    48,     6,   155,    -1,
     120,    -1,   156,    -1,   156,     9,   155,    -1,   226,    -1,
     226,   142,   226,   143,    -1,   226,   142,   144,   143,    -1,
      67,     6,   226,    -1,    57,     6,   226,    -1,    58,     6,
     226,    -1,    59,     6,   226,    -1,   189,     6,   218,    -1,
      81,     6,   195,    -1,   159,    -1,   158,   159,    -1,    -1,
      66,     3,   160,   161,     4,    -1,    -1,   161,   162,    -1,
     161,   172,    -1,   161,   164,    -1,   163,    -1,   163,     5,
     162,    -1,   163,   162,    -1,    67,     6,   226,    -1,    68,
       6,    75,    -1,    69,    -1,    70,    -1,    71,     6,   226,
      -1,    20,     6,   194,    -1,    63,     6,   226,    -1,    64,
       6,   226,    -1,    65,     6,   226,    -1,    72,   165,    -1,
     165,    -1,    -1,   165,   172,    -1,   165,   174,    -1,    10,
      81,    11,   195,    -1,    -1,    10,    82,   167,    11,   170,
      -1,    82,     3,   170,     4,    -1,    22,     3,   168,     4,
      -1,    10,    22,    11,   204,    -1,    22,     6,   204,    -1,
      10,    83,    11,   213,    -1,    10,    86,    11,   215,    -1,
      10,    50,    11,   182,    -1,   169,    -1,   169,     5,   168,
      -1,   204,    -1,    25,     6,   226,    -1,   171,    -1,   171,
       5,   170,    -1,   196,    -1,   197,    -1,   199,    -1,   200,
      -1,   201,    -1,   203,    -1,   202,    -1,    84,    -1,    85,
       6,   226,    -1,   198,    -1,    87,     6,   226,    -1,    60,
      -1,    61,    -1,    62,    -1,    -1,    10,   173,   178,    11,
     218,    -1,   218,    -1,    -1,     3,   175,   176,     4,    -1,
     179,    -1,   177,    -1,   177,     5,   176,    -1,   179,     5,
     176,    -1,   177,   176,    -1,   179,   176,    -1,    73,     6,
     218,    -1,    67,     6,   226,    -1,    20,     6,   185,    -1,
      20,     6,    96,    -1,    52,     6,    75,    -1,   179,    -1,
     179,     5,   178,    -1,   185,    -1,   188,    -1,   208,    -1,
     209,    -1,   180,    -1,    48,     6,   181,    -1,    49,     6,
     184,    -1,   187,    -1,   187,     9,   181,    -1,   183,    -1,
     183,     5,   182,    -1,   226,    -1,   226,    -1,    12,    -1,
      42,    -1,    43,    -1,    44,    -1,    45,    -1,    46,    -1,
      47,    -1,    14,    -1,    15,    -1,    13,    -1,    99,    -1,
      97,    -1,   210,    -1,   212,    -1,   194,    -1,    27,    -1,
      27,     6,   127,    -1,   226,    -1,   226,   142,   226,   143,
      -1,   226,   142,   144,   143,    -1,    16,     6,   190,    -1,
      17,     6,   191,    -1,    18,     6,   192,    -1,    19,     6,
     193,    -1,    24,     6,   205,    -1,    23,     6,   206,    -1,
      26,     6,   207,    -1,   101,     6,   218,    -1,   103,     6,
     218,    -1,   104,     6,   218,    -1,   105,     6,   218,    -1,
     106,     6,   218,    -1,    37,     6,   214,    -1,    36,     6,
     226,    -1,   107,     6,   218,    -1,   186,    -1,    20,     6,
     194,    -1,    81,     6,   195,    -1,    39,     6,   226,    -1,
     129,    -1,    56,     6,   226,    -1,   120,    -1,    99,    -1,
     210,    -1,    91,    -1,    92,    -1,    93,    -1,    94,    -1,
      95,    -1,    78,    -1,   110,    -1,    78,    -1,   111,    -1,
      78,    -1,   112,    -1,   113,    -1,   114,    -1,   115,    -1,
     116,    -1,   117,    -1,   129,    -1,   118,    -1,   119,    -1,
     120,    -1,   121,    -1,   122,    -1,   123,    -1,   125,    -1,
     124,    -1,   126,    -1,    28,     6,   226,    -1,    29,     6,
     226,    -1,    30,     6,   226,    -1,    31,     6,   226,    -1,
      32,     6,   226,    -1,    33,    -1,    34,    -1,    21,     6,
     226,    -1,    51,     6,   218,    -1,    53,     6,   218,    -1,
     101,    -1,   211,    -1,   107,    -1,   108,    -1,   109,    -1,
      89,    -1,    88,    -1,    90,    -1,   102,    -1,   103,    -1,
     104,    -1,   105,    -1,   106,    -1,   100,    -1,    96,    -1,
      98,    -1,    35,     6,   226,    -1,    37,     6,   214,    -1,
      36,     6,   226,    -1,    38,    -1,   213,     9,   213,    -1,
     130,    -1,    40,     6,   216,    -1,    41,     6,   217,    -1,
     215,     5,   215,    -1,   128,    -1,    54,    -1,    55,    -1,
      76,    -1,    77,    -1,    80,    -1,    79,    -1,   142,   226,
     143,    -1,   219,    -1,   137,   220,    -1,   141,   220,    -1,
     220,    -1,   221,   134,   220,    -1,   221,   135,   220,    -1,
     221,   136,   220,    -1,   221,    -1,   222,   137,   221,    -1,
     222,   138,   221,    -1,   222,    -1,   223,   139,   222,    -1,
     223,   140,   222,    -1,   223,    -1,   224,   133,   223,    -1,
     224,    -1,   225,   132,   224,    -1,   225,    -1,   226,   131,
     225,    -1
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
     265,   270,   275,   277,   281,   283,   287,   289,   293,   295,
     297,   299,   301,   303,   305,   307,   309,   313,   315,   319,
     321,   323,   325,   326,   332,   334,   335,   340,   342,   344,
     348,   352,   355,   358,   362,   366,   370,   374,   378,   380,
     384,   386,   388,   390,   392,   394,   398,   402,   404,   408,
     410,   414,   416,   418,   420,   422,   424,   426,   428,   430,
     432,   434,   436,   438,   440,   442,   444,   446,   448,   450,
     454,   456,   461,   466,   470,   474,   478,   482,   486,   490,
     494,   498,   502,   506,   510,   514,   518,   522,   526,   528,
     532,   536,   540,   542,   546,   548,   550,   552,   554,   556,
     558,   560,   562,   564,   566,   568,   570,   572,   574,   576,
     578,   580,   582,   584,   586,   588,   590,   592,   594,   596,
     598,   600,   602,   604,   608,   612,   616,   620,   624,   626,
     628,   632,   636,   640,   642,   644,   646,   648,   650,   652,
     654,   656,   658,   660,   662,   664,   666,   668,   670,   672,
     676,   680,   684,   686,   690,   692,   696,   700,   704,   706,
     708,   710,   712,   714,   716,   718,   722,   724,   727,   730,
     732,   736,   740,   744,   746,   750,   754,   756,   760,   764,
     766,   770,   772,   776,   778
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   195,   195,   197,   198,   201,   201,   207,   208,   209,
     210,   211,   212,   213,   214,   217,   217,   222,   223,   224,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   246,   247,   250,   252,
     254,   258,   259,   260,   261,   262,   263,   266,   267,   270,
     270,   274,   275,   276,   277,   280,   281,   282,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   297,   298,   301,
     302,   303,   306,   309,   309,   312,   313,   314,   319,   320,
     321,   322,   325,   326,   329,   330,   333,   334,   337,   340,
     342,   344,   346,   348,   350,   352,   354,   356,   362,   366,
     367,   368,   371,   371,   378,   384,   384,   390,   391,   392,
     393,   394,   395,   398,   401,   402,   403,   404,   409,   410,
     413,   414,   415,   416,   417,   420,   421,   424,   425,   428,
     429,   432,   437,   439,   440,   441,   442,   443,   444,   445,
     446,   447,   448,   451,   452,   453,   454,   455,   458,   459,
     462,   463,   464,   466,   467,   468,   469,   470,   471,   472,
     473,   474,   475,   476,   477,   478,   479,   480,   481,   482,
     483,   484,   485,   486,   487,   490,   491,   492,   493,   494,
     495,   496,   499,   500,   503,   504,   507,   508,   511,   514,
     517,   520,   523,   526,   529,   532,   535,   538,   541,   544,
     547,   550,   553,   556,   557,   558,   559,   560,   561,   562,
     563,   566,   567,   570,   571,   572,   573,   574,   575,   576,
     577,   578,   581,   582,   583,   584,   587,   588,   589,   592,
     595,   598,   601,   604,   607,   610,   613,   614,   617,   620,
     623,   629,   630,   633,   634,   635,   641,   642,   643,   647,
     648,   649,   650,   653,   654,   655,   658,   659,   660,   663,
     664,   667,   668,   672,   673
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
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 859;
  const int BisonParser::yynnts_ = 82;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 145;

  const unsigned int BisonParser::yyuser_token_number_max_ = 399;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 23 "../s/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 2340 "../bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 678 "../s/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
