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

  case 47:
/* Line 670 of lalr1.cc  */
#line 265 "../s/bif.y"
    { currentBifOptions->SetPdiType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 272 "../s/bif.y"
    { currentImageBifOptions = new ImageBifOptions(); }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 273 "../s/bif.y"
    { currentBifOptions->imageBifOptionList.push_back(currentImageBifOptions); }
    break;

  case 59:
/* Line 670 of lalr1.cc  */
#line 287 "../s/bif.y"
    { currentImageBifOptions->SetImageId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 288 "../s/bif.y"
    { currentImageBifOptions->SetImageName((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 289 "../s/bif.y"
    { currentImageBifOptions->SetDelayHandoff(true); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 290 "../s/bif.y"
    { currentImageBifOptions->SetDelayLoad(true); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 291 "../s/bif.y"
    { currentImageBifOptions->SetMemCopyAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 292 "../s/bif.y"
    { currentImageBifOptions->SetImageType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 293 "../s/bif.y"
    { currentImageBifOptions->SetUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 294 "../s/bif.y"
    { currentImageBifOptions->SetParentUniqueId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 67:
/* Line 670 of lalr1.cc  */
#line 295 "../s/bif.y"
    { currentImageBifOptions->SetFunctionId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 308 "../s/bif.y"
    { if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_WARNING("BIF attribute error !!! [keysrc_encryption] not supported in VERSAL architecture.\n\t   Please see 'bootgen -arch versal -bif_help keysrc'");
                                                                                  currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 311 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 78:
/* Line 670 of lalr1.cc  */
#line 316 "../s/bif.y"
    { if(options.GetArchType() == Arch::ZYNQ) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); 
                                                                                    if(options.GetArchType() == Arch::VERSAL) 
                                                                                    LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); 
                                                                                    currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); }
    break;

  case 79:
/* Line 670 of lalr1.cc  */
#line 321 "../s/bif.y"
    { LOG_ERROR("This usage of boot_device is no more supported.\n\t   Please see 'bootgen -arch versal -bif_help boot_device'"); }
    break;

  case 86:
/* Line 670 of lalr1.cc  */
#line 332 "../s/bif.y"
    { currentBifOptions->SetBootDevice((yysemantic_stack_[(1) - (1)].bootdevice_t)); }
    break;

  case 87:
/* Line 670 of lalr1.cc  */
#line 333 "../s/bif.y"
    { currentBifOptions->SetBootDeviceAddress((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 93:
/* Line 670 of lalr1.cc  */
#line 345 "../s/bif.y"
    { currentBifOptions->SetAuthJtagRevokeID((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 94:
/* Line 670 of lalr1.cc  */
#line 346 "../s/bif.y"
    { currentBifOptions->SetAuthJtagDeviceDna((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 95:
/* Line 670 of lalr1.cc  */
#line 347 "../s/bif.y"
    { currentBifOptions->SetAuthJtagTimeOut((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 96:
/* Line 670 of lalr1.cc  */
#line 350 "../s/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                                                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 97:
/* Line 670 of lalr1.cc  */
#line 353 "../s/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 98:
/* Line 670 of lalr1.cc  */
#line 355 "../s/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 99:
/* Line 670 of lalr1.cc  */
#line 357 "../s/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 359 "../s/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 361 "../s/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 363 "../s/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 103:
/* Line 670 of lalr1.cc  */
#line 365 "../s/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 367 "../s/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 369 "../s/bif.y"
    {  if(options.GetArchType() != Arch::VERSAL)
                                                                                     LOG_ERROR("BIF attribute error !!!\n\t\t'dpacm_enable' is supported only in VERSAL architecture");
                                                                                   if(options.GetArchType() == Arch::VERSAL)
                                                                                     LOG_WARNING("boot_config { dpacm_enable } will be deprecated. 'dpacm_enable' should be specified along with the partition. \n            See 'bootgen -bif_help dpacm_enable' for more info.");
                                                                                   currentBifOptions->SetDpaCM((yysemantic_stack_[(1) - (1)].dpacm_t));
                                                                                }
    break;

  case 106:
/* Line 670 of lalr1.cc  */
#line 375 "../s/bif.y"
    { if(((yysemantic_stack_[(3) - (3)].number) != 8) && ((yysemantic_stack_[(3) - (3)].number) !=16) && ((yysemantic_stack_[(3) - (3)].number) != 32) && ((yysemantic_stack_[(3) - (3)].number) != 0))
                                                                                        LOG_ERROR("Invalid smap_width value in BIF. Valid values are 8, 16 and 32");
                                                                                  currentBifOptions->smapWidth = (yysemantic_stack_[(3) - (3)].number);
                                                                                }
    break;

  case 107:
/* Line 670 of lalr1.cc  */
#line 379 "../s/bif.y"
    { currentBifOptions->SetBypassIdcodeFlag(true); }
    break;

  case 108:
/* Line 670 of lalr1.cc  */
#line 380 "../s/bif.y"
    { currentBifOptions->SetAHwRoTFlag(true); }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 381 "../s/bif.y"
    { currentBifOptions->SetSHwRoTFlag(true); }
    break;

  case 110:
/* Line 670 of lalr1.cc  */
#line 384 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 387 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(5) - (5)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 391 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType());
                                                                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(1) - (1)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions);
                                                                                }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 397 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                                                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 411 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filelist.push_back((yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 414 "../s/bif.y"
    { currentPartitionBifOptions->partitionId = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 416 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 417 "../s/bif.y"
    { currentPartitionBifOptions->bifSection = (yysemantic_stack_[(3) - (3)].string);
                                                                                  currentPartitionBifOptions->filename = currentPartitionBifOptions->GetOutputFileFromBifSection(options.GetOutputFileNames().front(), (yysemantic_stack_[(3) - (3)].string));
                                                                                  currentBifOptions->Add(currentPartitionBifOptions, currentImageBifOptions); }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 445 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] only supported in ZYNQMP architecture");
                                                                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 450 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 452 "../s/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 453 "../s/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 454 "../s/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 455 "../s/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 456 "../s/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 457 "../s/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 458 "../s/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 459 "../s/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 460 "../s/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 150:
/* Line 670 of lalr1.cc  */
#line 461 "../s/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true)
                                                                                        LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                                                                  currentBifOptions->SetXipMode(); }
    break;

  case 151:
/* Line 670 of lalr1.cc  */
#line 464 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 152:
/* Line 670 of lalr1.cc  */
#line 465 "../s/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 153:
/* Line 670 of lalr1.cc  */
#line 466 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 467 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 468 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(1) - (1)].ptype_t)); }
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 471 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 157:
/* Line 670 of lalr1.cc  */
#line 472 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 475 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 476 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 477 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 161:
/* Line 670 of lalr1.cc  */
#line 479 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 480 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 481 "../s/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 482 "../s/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 483 "../s/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 484 "../s/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 485 "../s/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 486 "../s/bif.y"
    { currentPartitionBifOptions->SetAesKeyFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 487 "../s/bif.y"
    { currentPartitionBifOptions->ppkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 488 "../s/bif.y"
    { currentPartitionBifOptions->pskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 489 "../s/bif.y"
    { currentPartitionBifOptions->spkFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 490 "../s/bif.y"
    { currentPartitionBifOptions->sskFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 491 "../s/bif.y"
    { currentPartitionBifOptions->spkSelect =((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 492 "../s/bif.y"
    { currentPartitionBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 493 "../s/bif.y"
    { currentPartitionBifOptions->spkSignatureFile = ((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 495 "../s/bif.y"
    { currentPartitionBifOptions->SetPartitionType((yysemantic_stack_[(3) - (3)].ptype_t)); }
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 496 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionKeySource((yysemantic_stack_[(3) - (3)].encrkeysrc_t)); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 497 "../s/bif.y"
    { currentPartitionBifOptions->SetRevokeId((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 498 "../s/bif.y"
    { currentPartitionBifOptions->SetDpaCM(DpaCM::DpaCMEnable); }
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 499 "../s/bif.y"
    { currentPartitionBifOptions->SetSlrNum((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 500 "../s/bif.y"
    { currentPartitionBifOptions->SetPufHdLocation(PufHdLoc::PUFinBH); }
    break;

  case 191:
/* Line 670 of lalr1.cc  */
#line 513 "../s/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 193:
/* Line 670 of lalr1.cc  */
#line 517 "../s/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 195:
/* Line 670 of lalr1.cc  */
#line 521 "../s/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 212:
/* Line 670 of lalr1.cc  */
#line 570 "../s/bif.y"
    { currentPartitionBifOptions->alignment = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 213:
/* Line 670 of lalr1.cc  */
#line 571 "../s/bif.y"
    { currentPartitionBifOptions->offset = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 214:
/* Line 670 of lalr1.cc  */
#line 572 "../s/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), false); }
    break;

  case 215:
/* Line 670 of lalr1.cc  */
#line 573 "../s/bif.y"
    { currentPartitionBifOptions->SetReserveLength((yysemantic_stack_[(3) - (3)].number), true); }
    break;

  case 216:
/* Line 670 of lalr1.cc  */
#line 574 "../s/bif.y"
    { currentPartitionBifOptions->load = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 217:
/* Line 670 of lalr1.cc  */
#line 575 "../s/bif.y"
    { currentPartitionBifOptions->startup = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 218:
/* Line 670 of lalr1.cc  */
#line 576 "../s/bif.y"
    { currentPartitionBifOptions->bigEndian = true; }
    break;

  case 219:
/* Line 670 of lalr1.cc  */
#line 577 "../s/bif.y"
    { currentPartitionBifOptions->a32Mode = true; }
    break;

  case 220:
/* Line 670 of lalr1.cc  */
#line 578 "../s/bif.y"
    { currentPartitionBifOptions->pid = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 221:
/* Line 670 of lalr1.cc  */
#line 581 "../s/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 222:
/* Line 670 of lalr1.cc  */
#line 582 "../s/bif.y"
    { currentPartitionBifOptions->SetUdfDataFile((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 239:
/* Line 670 of lalr1.cc  */
#line 607 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 240:
/* Line 670 of lalr1.cc  */
#line 610 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 241:
/* Line 670 of lalr1.cc  */
#line 613 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_WARNING("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 242:
/* Line 670 of lalr1.cc  */
#line 616 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] is supported only in ZYNQMP architecture");
                                                                                  currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 245:
/* Line 670 of lalr1.cc  */
#line 625 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 249:
/* Line 670 of lalr1.cc  */
#line 635 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 250:
/* Line 670 of lalr1.cc  */
#line 638 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                                                      LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 255:
/* Line 670 of lalr1.cc  */
#line 650 "../s/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 257:
/* Line 670 of lalr1.cc  */
#line 657 "../s/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 258:
/* Line 670 of lalr1.cc  */
#line 658 "../s/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 260:
/* Line 670 of lalr1.cc  */
#line 663 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 261:
/* Line 670 of lalr1.cc  */
#line 664 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 262:
/* Line 670 of lalr1.cc  */
#line 665 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 264:
/* Line 670 of lalr1.cc  */
#line 669 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 265:
/* Line 670 of lalr1.cc  */
#line 670 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 267:
/* Line 670 of lalr1.cc  */
#line 674 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 268:
/* Line 670 of lalr1.cc  */
#line 675 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 270:
/* Line 670 of lalr1.cc  */
#line 679 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 272:
/* Line 670 of lalr1.cc  */
#line 683 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 274:
/* Line 670 of lalr1.cc  */
#line 688 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 1381 "../bisonflex/bif.tab.cpp"
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
  const short int BisonParser::yypact_ninf_ = -358;
  const short int
  BisonParser::yypact_[] =
  {
      -358,    12,   -11,  -358,  -358,  -358,    60,    79,  -358,   712,
    -358,  -358,   185,    83,   103,    85,   102,   104,   128,   139,
    -358,   140,  -358,  -358,   142,   153,   171,  -358,  -358,  -358,
    -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,
    -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,   109,  -358,
    -358,     8,  -358,  -358,  -358,   173,  -358,  -358,  -358,   524,
     191,   199,   200,  -358,   203,   207,   648,    99,    20,    92,
     134,   134,   134,  -358,   134,     8,  -358,   101,   169,    39,
    -358,  -358,  -358,  -358,    -4,  -358,  -358,  -358,  -358,   217,
     218,   219,   221,   222,   227,   228,   229,   233,   234,   236,
     239,   247,   248,   249,   250,  -358,  -358,   251,   256,   261,
    -358,  -358,  -358,  -358,  -358,  -358,   265,   267,   268,   269,
     272,   273,   274,   279,   292,  -358,  -358,  -358,  -358,  -358,
     293,   294,   299,   302,   303,   304,  -358,  -358,  -358,   307,
       9,   400,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,
    -358,    92,   134,   101,   296,   160,    58,   308,   305,   310,
    -358,   313,  -358,   318,   319,  -358,  -358,  -358,  -358,   134,
     134,   134,  -358,  -358,    28,     7,    42,   184,   187,   190,
     190,   190,  -358,   190,   177,  -358,  -358,  -358,  -358,  -358,
    -358,   322,   323,  -358,  -358,  -358,  -358,  -358,  -358,  -358,
    -358,   328,   329,  -358,  -358,  -358,  -358,  -358,  -358,  -358,
    -358,   330,   331,   332,   335,    10,  -358,   -46,   -49,   -67,
     216,   787,   134,   213,   214,   212,   223,   134,   134,   134,
     134,   134,   134,   134,   210,   134,   134,   134,    -4,   271,
      -4,   134,   134,    -4,   101,    -4,    -4,    -4,    -4,    -4,
      -4,  -358,   524,  -358,   524,  -358,  -358,  -358,   344,   190,
    -358,   169,   351,   352,   353,  -358,   354,   358,   362,   355,
      99,    -4,   648,   134,  -358,    20,  -358,  -358,   -73,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,    68,
     368,   373,   375,   376,   377,   387,   388,   389,   390,   391,
     392,   393,   394,  -358,  -358,   397,   137,   134,   134,  -358,
     169,   134,   325,   134,  -358,    39,  -358,  -358,  -358,  -358,
    -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,
    -358,   190,  -358,  -358,  -358,  -358,  -358,  -358,  -358,   190,
     190,   190,   190,   190,   190,   190,  -358,  -358,   190,  -358,
     395,   -77,  -358,   190,  -358,  -358,  -358,   190,   190,  -358,
    -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,   134,
    -358,   134,   134,   210,   160,   275,   145,    58,  -358,  -358,
    -358,   190,  -358,  -358,  -358,  -358,  -358,    28,    28,     7,
       7,    42,   184,   187,  -358,   401,   402,   416,   419,   430,
     433,  -358,  -358,   435,  -358,   301,  -358,  -358,   -46,   -49,
     -67,   134,   134,    -4,   101,    -4,    -4,    -4,    -4,    -4,
      -4,  -358,   177,  -358,   190,   190,  -358,   190,  -358,   190,
    -358,   134,   170,  -358,   190,   190,  -358,   354,  -358,  -358,
    -358,  -358,  -358,   355,    99,   134,   134,   134,   134,   327,
     134,   320,  -358,  -358,  -358,  -358,   190,  -358,   442,   -53,
    -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,
     257,   -64,  -358,   190,   190,   190,   190,  -358,   190,  -358,
     134,   183,  -358,  -358,  -358,   306,   -44,  -358,  -358
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned short int
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     5,     4,     0,     0,     7,     0,
     113,     6,   110,     0,     0,     0,     0,     0,     0,     0,
      70,     0,   251,   252,     0,     0,     0,   229,   228,   230,
     185,   186,   187,   188,   189,   190,   183,   223,   231,   232,
     233,   234,   235,   225,   226,   227,    11,    12,    10,    48,
      14,    69,     9,     8,    13,     0,   184,   224,   112,     0,
       0,     0,     0,    74,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    50,     0,    68,    15,     0,     0,     0,
      49,   110,    71,    72,     0,   141,   150,   148,   149,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   156,     0,
       0,     0,     0,     0,     0,   218,   219,     0,     0,     0,
     142,   143,   144,   145,   146,   147,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   237,   152,   238,   151,   236,
     223,   232,   233,   234,   235,   225,   198,   182,   180,     0,
     116,   115,   132,   128,   176,   129,   155,   130,   131,   153,
     154,     0,     0,     0,     0,     0,     0,     0,     0,   126,
      47,     0,   208,     0,    84,    86,    79,   254,   253,     0,
       0,     0,   256,   259,   263,   266,   269,   271,   273,    42,
      43,    44,    52,    41,    20,   199,    46,   107,   108,   109,
     103,     0,     0,   200,   201,   203,   204,   205,   206,   207,
     202,     0,    88,    96,    97,   105,    98,    99,   100,   102,
     101,     0,     0,     0,     0,    90,    45,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   114,     0,   119,     0,   120,    78,    82,   137,   139,
      73,     0,     0,     0,     0,   242,    80,     0,     0,    81,
       0,     0,     0,     0,    77,     0,   257,   258,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    70,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    35,    33,     0,    17,     0,     0,    76,
       0,     0,     0,     0,    83,     0,    92,   191,   192,   161,
     193,   194,   162,   195,   196,   163,   197,   164,   124,   123,
     155,   220,   210,   166,   209,   165,   211,   167,   157,   212,
     213,   214,   215,   216,   217,   174,   244,   173,   179,   133,
     135,   158,   134,   140,   221,   125,   222,   181,   122,   121,
     178,   168,   169,   170,   171,   172,   175,   117,   118,     0,
      75,     0,     0,     0,     0,     0,     0,     0,   177,   111,
     127,    87,    85,   255,   260,   261,   262,   264,   265,   267,
     268,   270,   272,   274,    51,     0,     0,     0,     0,     0,
       0,    61,    62,     0,    53,    56,    55,    54,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    16,    20,    19,   104,   106,    89,    93,    94,    95,
      91,     0,     0,   138,   239,   241,   240,   243,   248,   245,
     249,   250,   246,   247,     0,     0,     0,     0,     0,     0,
       0,     0,    58,    24,    21,    32,    31,    34,    36,    38,
      30,    22,    23,    25,    26,    27,    28,    29,    18,   136,
       0,     0,    64,    65,    66,    67,    59,    60,    63,    57,
       0,     0,   160,   159,    37,     0,     0,    40,    39
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yypgoto_[] =
  {
      -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -297,  -358,
     -38,  -358,  -358,  -358,   408,  -358,  -358,  -357,  -358,   172,
     438,  -358,  -358,   188,  -358,  -241,  -208,  -358,  -358,    -6,
    -358,   450,  -358,  -124,  -358,   192,   -60,  -358,    29,    93,
    -358,  -358,   244,  -358,  -358,  -358,  -358,    59,    57,    61,
    -358,   -57,  -148,  -358,  -358,  -358,  -358,  -358,  -358,  -358,
    -358,   -61,  -358,  -358,  -358,  -358,  -358,   460,  -358,  -358,
      96,   100,    95,  -358,  -358,   -65,  -358,   -89,   -78,   -76,
     189,   193,   194,   -70
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     5,     6,     9,    46,   184,   305,   306,
     457,   458,    47,    48,    49,   182,   289,   404,   405,    50,
      51,    52,   154,   163,   164,   201,   214,   215,   202,    82,
      66,    83,    59,   139,   140,   158,   141,   142,   349,   257,
     258,   352,   143,   144,   350,   145,    55,   319,   322,   325,
     327,   146,   186,   203,   204,   205,   206,   207,   208,   209,
     210,   165,   335,   333,   337,   147,   148,   149,    57,   150,
     266,   347,   269,   439,   442,    58,   172,   173,   174,   175,
     176,   177,   178,   259
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
       179,   180,   181,    53,   183,   260,   159,   316,   166,   423,
     160,    10,     3,   323,   252,   315,   253,   255,    81,   216,
     370,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,   320,    95,    96,   317,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   161,   107,   108,   452,   109,
     211,   324,   110,   111,   112,   113,   114,   115,   116,   117,
     288,   118,   119,   120,   288,     4,   121,     7,   321,   426,
     318,   432,   394,   288,    22,    23,   383,   122,    81,   211,
     276,   277,     8,   123,   288,   483,    22,    23,   395,    67,
     256,    70,   124,   288,   479,   481,   360,   212,   213,   267,
     268,   278,    27,    28,    29,   488,    68,   430,    71,    69,
      72,   125,   126,   127,   128,   129,   130,    38,   131,   132,
     133,   134,   135,    44,    45,   468,   212,   213,   367,   136,
     368,    73,   396,   397,   398,   137,   399,   400,   401,   402,
     403,    20,   422,    76,   138,    74,    22,    23,    77,   162,
     282,   283,   331,   290,   291,   292,    78,   339,   340,   341,
     342,   343,   344,   345,   330,   348,   351,   353,   279,   280,
     281,   357,   358,   354,    79,   356,    18,   293,   359,    84,
     361,   362,   363,   364,   365,   366,   294,   284,   285,   295,
     384,   385,   386,   290,   291,   292,   262,   263,   264,   265,
     440,   441,   151,   381,   387,   388,   379,    60,   389,   390,
     152,   153,   159,   378,   155,   167,   168,   293,   156,   136,
     296,   162,   185,   217,   218,   219,   294,   220,   221,   295,
     187,   188,   189,   222,   223,   224,    61,   424,   425,   225,
     226,   427,   227,   429,   297,   228,   298,   299,   300,   301,
     302,   167,   168,   229,   230,   231,   232,   233,   190,   191,
     296,   192,   234,   303,   167,   168,   461,   235,    62,    63,
      64,   236,   304,   237,   238,   239,    65,   169,   240,   241,
     242,   170,   171,   407,   297,   243,   298,   299,   300,   301,
     302,   193,   194,   195,   196,   197,   198,   199,   244,   245,
     246,   434,   435,   303,   200,   247,   451,   261,   248,   249,
     250,   251,   304,   169,   270,   272,   271,   170,   171,   273,
     470,   395,   274,   286,   275,   287,   169,   288,   307,   308,
     170,   171,   309,   485,   310,   326,   311,   312,   313,   314,
     395,   456,   459,   332,   336,   334,   346,   355,   460,   369,
     462,   463,   464,   465,   466,   467,   338,   371,   372,   373,
     377,   351,   471,   374,   375,   396,   397,   398,   376,   399,
     400,   401,   402,   403,   408,   473,   474,   475,   476,   409,
     478,   410,   411,   412,   396,   397,   398,   472,   399,   400,
     401,   402,   403,   413,   414,   415,   416,   417,   418,   419,
     420,   421,   428,   477,   431,   254,   482,   444,   445,   438,
     459,   486,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,   446,    95,    96,   447,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   448,   107,   108,   449,
     109,   450,   484,   110,   111,   112,   113,   114,   115,   116,
     117,   480,   118,   119,   120,   487,    80,   121,    75,    54,
     469,   406,   433,   382,   380,   329,   454,   453,   122,    56,
     437,   455,   443,   436,   123,   391,     0,     0,     0,     0,
     392,     0,   393,   124,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    27,    28,    29,     0,     0,     0,     0,
       0,     0,   125,   126,   127,   128,   129,   130,    38,   131,
     132,   133,   134,   135,    44,    45,     0,     0,     0,     0,
     136,     0,     0,     0,     0,     0,   137,     0,     0,     0,
       0,     0,     0,     0,     0,   138,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,     0,    95,    96,     0,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
       0,   107,   108,     0,   109,     0,     0,   110,   111,   112,
     113,   114,   115,   116,   117,     0,   118,   119,   120,     0,
       0,   121,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   122,     0,     0,     0,     0,     0,   123,     0,
       0,     0,     0,     0,     0,     0,     0,   124,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    27,    28,    29,
       0,     0,     0,     0,     0,     0,   125,   126,   127,   128,
     129,   130,    38,   131,   132,   133,   134,   135,    44,    45,
       0,     0,     0,     0,   136,     0,     0,     0,     0,     0,
     137,     0,     0,     0,     0,     0,     0,     0,     0,   138,
      85,    86,    87,    88,    89,    90,    91,    92,   157,    94,
       0,    95,    96,     0,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,     0,   107,   108,     0,   109,     0,
       0,   110,   111,   112,   113,   114,   115,   116,   117,     0,
     118,     0,   120,     0,     0,   121,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    10,    11,     0,     0,     0,
       0,     0,    12,     0,     0,     0,     0,     0,     0,     0,
       0,   124,    13,     0,    14,     0,     0,     0,     0,     0,
       0,    27,    28,    29,     0,     0,     0,     0,     0,     0,
     125,   126,   127,   128,   129,   130,    38,   131,   132,   133,
     134,   135,    44,    45,     0,     0,     0,     0,   136,     0,
      15,    16,    17,     0,   137,     0,     0,     0,     0,    18,
      19,     0,     0,   138,     0,    20,     0,    21,     0,     0,
      22,    23,     0,     0,     0,    24,    25,     0,    26,    85,
      86,    87,    88,     0,     0,    27,    28,    29,    30,    31,
      32,    33,    34,    35,     0,     0,     0,    36,     0,    37,
      38,    39,    40,    41,    42,    43,    44,    45,     0,     0,
     110,   111,   112,   113,   114,   115,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      27,    28,    29,     0,     0,     0,     0,     0,     0,   328,
     126,   127,   128,   129,    37,    38,    39,    40,    41,    42,
      43,    44,    45,     0,     0,     0,     0,   136
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        70,    71,    72,     9,    74,   153,    66,   215,    69,   306,
      67,     3,     0,    80,     5,     5,   140,   141,    10,    84,
     261,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    80,    23,    24,    80,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    25,    37,    38,   405,    40,
      40,   118,    43,    44,    45,    46,    47,    48,    49,    50,
     137,    52,    53,    54,   137,    76,    57,     7,   117,   310,
     116,   148,     4,   137,    78,    79,   149,    68,    10,    40,
     169,   170,     3,    74,   137,   149,    78,    79,    20,     6,
     151,     6,    83,   137,   451,   148,   244,    87,    88,    41,
      42,   171,    93,    94,    95,   149,     3,   315,     6,     6,
       6,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   422,    87,    88,   252,   120,
     254,     3,    64,    65,    66,   126,    68,    69,    70,    71,
      72,    73,     5,     3,   135,     6,    78,    79,     6,   129,
     143,   144,   222,    16,    17,    18,     3,   227,   228,   229,
     230,   231,   232,   233,   221,   235,   236,   237,   140,   141,
     142,   241,   242,   238,     3,   240,    67,    40,   243,     6,
     245,   246,   247,   248,   249,   250,    49,   145,   146,    52,
     279,   280,   281,    16,    17,    18,    36,    37,    38,    39,
      55,    56,    11,   273,   282,   283,   271,    22,   284,   285,
      11,    11,   272,   270,    11,    81,    82,    40,    11,   120,
      83,   129,   121,     6,     6,     6,    49,     6,     6,    52,
      61,    62,    63,     6,     6,     6,    51,   307,   308,     6,
       6,   311,     6,   313,   107,     6,   109,   110,   111,   112,
     113,    81,    82,     6,     6,     6,     6,     6,    89,    90,
      83,    92,     6,   126,    81,    82,   414,     6,    83,    84,
      85,     6,   135,     6,     6,     6,    91,   143,     6,     6,
       6,   147,   148,   289,   107,     6,   109,   110,   111,   112,
     113,   122,   123,   124,   125,   126,   127,   128,     6,     6,
       6,   371,   372,   126,   135,     6,     5,    11,     6,     6,
       6,     4,   135,   143,     6,     5,    11,   147,   148,     6,
     150,    20,     4,   139,     5,   138,   143,   137,     6,     6,
     147,   148,     4,   150,     5,   119,     6,     6,     6,     4,
      20,   411,   412,   130,   132,   131,   136,    76,   413,     5,
     415,   416,   417,   418,   419,   420,   133,     6,     6,     6,
       5,   431,   432,     9,     6,    64,    65,    66,     6,    68,
      69,    70,    71,    72,     6,   445,   446,   447,   448,     6,
     450,     6,     6,     6,    64,    65,    66,   444,    68,    69,
      70,    71,    72,     6,     6,     6,     6,     6,     6,     6,
       6,     4,    77,    76,     9,     5,   149,     6,     6,   134,
     480,   481,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,     6,    23,    24,     6,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,     6,    37,    38,     6,
      40,     6,   480,    43,    44,    45,    46,    47,    48,    49,
      50,     9,    52,    53,    54,   149,    48,    57,    20,     9,
     431,   289,   369,   275,   272,   221,   409,   408,    68,     9,
     374,   410,   377,   373,    74,   286,    -1,    -1,    -1,    -1,
     287,    -1,   288,    83,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    93,    94,    95,    -1,    -1,    -1,    -1,
      -1,    -1,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
     120,    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   135,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    -1,    23,    24,    -1,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      -1,    37,    38,    -1,    40,    -1,    -1,    43,    44,    45,
      46,    47,    48,    49,    50,    -1,    52,    53,    54,    -1,
      -1,    57,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    68,    -1,    -1,    -1,    -1,    -1,    74,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    83,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    93,    94,    95,
      -1,    -1,    -1,    -1,    -1,    -1,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
      -1,    -1,    -1,    -1,   120,    -1,    -1,    -1,    -1,    -1,
     126,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   135,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      -1,    23,    24,    -1,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    -1,    37,    38,    -1,    40,    -1,
      -1,    43,    44,    45,    46,    47,    48,    49,    50,    -1,
      52,    -1,    54,    -1,    -1,    57,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,    -1,
      -1,    -1,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    83,    20,    -1,    22,    -1,    -1,    -1,    -1,    -1,
      -1,    93,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,    -1,    -1,    -1,    -1,   120,    -1,
      58,    59,    60,    -1,   126,    -1,    -1,    -1,    -1,    67,
      68,    -1,    -1,   135,    -1,    73,    -1,    75,    -1,    -1,
      78,    79,    -1,    -1,    -1,    83,    84,    -1,    86,    12,
      13,    14,    15,    -1,    -1,    93,    94,    95,    96,    97,
      98,    99,   100,   101,    -1,    -1,    -1,   105,    -1,   107,
     108,   109,   110,   111,   112,   113,   114,   115,    -1,    -1,
      43,    44,    45,    46,    47,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      93,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,    -1,    -1,    -1,    -1,   120
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   152,   153,     0,    76,   154,   155,     7,     3,   156,
       3,     4,    10,    20,    22,    58,    59,    60,    67,    68,
      73,    75,    78,    79,    83,    84,    86,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   105,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   157,   163,   164,   165,
     170,   171,   172,   180,   182,   197,   218,   219,   226,   183,
      22,    51,    83,    84,    85,    91,   181,     6,     3,     6,
       6,     6,     6,     3,     6,   171,     3,     6,     3,     3,
     165,    10,   180,   182,     6,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    23,    24,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    37,    38,    40,
      43,    44,    45,    46,    47,    48,    49,    50,    52,    53,
      54,    57,    68,    74,    83,   102,   103,   104,   105,   106,
     107,   109,   110,   111,   112,   113,   120,   126,   135,   184,
     185,   187,   188,   193,   194,   196,   202,   216,   217,   218,
     220,    11,    11,    11,   173,    11,    11,    20,   186,   187,
     202,    25,   129,   174,   175,   212,   212,    81,    82,   143,
     147,   148,   227,   228,   229,   230,   231,   232,   233,   234,
     234,   234,   166,   234,   158,   121,   203,    61,    62,    63,
      89,    90,    92,   122,   123,   124,   125,   126,   127,   128,
     135,   176,   179,   204,   205,   206,   207,   208,   209,   210,
     211,    40,    87,    88,   177,   178,   226,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     4,     5,   184,     5,   184,   212,   190,   191,   234,
     203,    11,    36,    37,    38,    39,   221,    41,    42,   223,
       6,    11,     5,     6,     4,     5,   228,   228,   234,   140,
     141,   142,   143,   144,   145,   146,   139,   138,   137,   167,
      16,    17,    18,    40,    49,    52,    83,   107,   109,   110,
     111,   112,   113,   126,   135,   159,   160,     6,     6,     4,
       5,     6,     6,     6,     4,     5,   177,    80,   116,   198,
      80,   117,   199,    80,   118,   200,   119,   201,   102,   193,
     202,   234,   130,   214,   131,   213,   132,   215,   133,   234,
     234,   234,   234,   234,   234,   234,   136,   222,   234,   189,
     195,   234,   192,   234,   226,    76,   226,   234,   234,   226,
     203,   226,   226,   226,   226,   226,   226,   184,   184,     5,
     176,     6,     6,     6,     9,     6,     6,     5,   202,   226,
     186,   234,   174,   149,   228,   228,   228,   229,   229,   230,
     230,   231,   232,   233,     4,    20,    64,    65,    66,    68,
      69,    70,    71,    72,   168,   169,   170,   180,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     4,     5,   159,   234,   234,   176,   234,    77,   234,
     177,     9,   148,   190,   234,   234,   222,   221,   134,   224,
      55,    56,   225,   223,     6,     6,     6,     6,     6,     6,
       6,     5,   168,   198,   199,   200,   234,   161,   162,   234,
     226,   203,   226,   226,   226,   226,   226,   226,   159,   189,
     150,   234,   202,   234,   234,   234,   234,    76,   234,   168,
       9,   148,   149,   149,   161,   150,   234,   149,   149
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
     405
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   151,   152,   153,   153,   155,   154,   156,   156,   156,
     156,   156,   156,   156,   156,   158,   157,   159,   159,   159,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   161,   161,   162,   162,
     162,   163,   163,   163,   163,   163,   163,   163,   164,   164,
     166,   165,   167,   167,   167,   167,   168,   168,   168,   169,
     169,   169,   169,   169,   169,   169,   169,   169,   170,   170,
     171,   171,   171,   172,   173,   172,   172,   172,   172,   172,
     172,   172,   172,   172,   174,   174,   175,   175,   176,   176,
     177,   177,   177,   178,   178,   178,   179,   179,   179,   179,
     179,   179,   179,   179,   179,   179,   179,   179,   179,   179,
     181,   180,   180,   183,   182,   184,   184,   184,   184,   184,
     184,   185,   185,   185,   185,   185,   186,   186,   187,   187,
     187,   187,   187,   188,   188,   189,   189,   190,   190,   191,
     192,   193,   193,   193,   193,   193,   193,   193,   193,   193,
     193,   193,   193,   193,   193,   193,   194,   194,   195,   195,
     195,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   196,   196,   196,   196,   196,   196,   196,
     196,   196,   196,   197,   197,   197,   197,   197,   197,   197,
     197,   198,   198,   199,   199,   200,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   216,   216,   216,   216,   216,   216,   216,
     216,   217,   217,   218,   218,   218,   218,   218,   218,   218,
     218,   218,   219,   219,   219,   219,   220,   220,   220,   221,
     221,   221,   221,   221,   222,   223,   223,   223,   224,   225,
     225,   226,   226,   227,   227,   227,   228,   228,   228,   229,
     229,   229,   229,   230,   230,   230,   231,   231,   231,   232,
     232,   233,   233,   234,   234
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     0,     6,     0,     2,     2,
       2,     2,     2,     2,     2,     0,     5,     1,     3,     2,
       0,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     3,     1,     1,     3,     1,     4,
       4,     3,     3,     3,     3,     3,     3,     3,     1,     2,
       0,     5,     0,     2,     2,     2,     1,     3,     2,     3,
       3,     1,     1,     3,     3,     3,     3,     3,     2,     1,
       0,     2,     2,     4,     0,     5,     4,     4,     4,     3,
       4,     4,     4,     4,     1,     3,     1,     3,     1,     3,
       1,     3,     2,     3,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     1,     1,     1,
       0,     5,     1,     0,     4,     1,     1,     3,     3,     2,
       2,     3,     3,     3,     3,     3,     1,     3,     1,     1,
       1,     1,     1,     3,     3,     1,     3,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     4,
       4,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     3,     3,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
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
  "XIP_MODE", "EARLY_HANDOFF", "HIVEC", "AUTHENTICATION", "ENCRYPTION",
  "CHECKSUM", "PARTITION_OWNER", "PARTITION_TYPE", "PARTITION_NUM",
  "BOOT_DEVICE", "DEST_DEVICE", "DEST_CPU", "ADDRESS", "EXCEPTION_LEVEL",
  "TRUSTZONE", "ALIGNMENT", "OFFSET", "RESERVE_LEGACY", "RESERVE", "LOAD",
  "STARTUP", "BIGENDIAN", "A32_MODE", "PPK_SELECT", "SPK_ID", "SPK_SELECT",
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
       152,     0,    -1,   153,    -1,    -1,   153,   154,    -1,    -1,
      76,   155,     7,     3,   156,     4,    -1,    -1,   156,   180,
      -1,   156,   172,    -1,   156,   164,    -1,   156,   157,    -1,
     156,   163,    -1,   156,   182,    -1,   156,   170,    -1,    -1,
      75,     3,   158,   159,     4,    -1,   160,    -1,   160,     5,
     159,    -1,   160,   159,    -1,    -1,    17,     6,   199,    -1,
      83,     6,   203,    -1,   107,     6,   226,    -1,    16,     6,
     198,    -1,   109,     6,   226,    -1,   110,     6,   226,    -1,
     111,     6,   226,    -1,   112,     6,   226,    -1,   113,     6,
     226,    -1,    52,     6,   226,    -1,    40,     6,   234,    -1,
      18,     6,   200,    -1,   135,    -1,    49,     6,   161,    -1,
     126,    -1,   162,    -1,   162,     9,   161,    -1,   234,    -1,
     234,   148,   234,   149,    -1,   234,   148,   150,   149,    -1,
      68,     6,   234,    -1,    58,     6,   234,    -1,    59,     6,
     234,    -1,    60,     6,   234,    -1,   197,     6,   226,    -1,
      83,     6,   203,    -1,    20,     6,   202,    -1,   165,    -1,
     164,   165,    -1,    -1,    67,     3,   166,   167,     4,    -1,
      -1,   167,   168,    -1,   167,   180,    -1,   167,   170,    -1,
     169,    -1,   169,     5,   168,    -1,   169,   168,    -1,    68,
       6,   234,    -1,    69,     6,    76,    -1,    70,    -1,    71,
      -1,    72,     6,   234,    -1,    20,     6,   202,    -1,    64,
       6,   234,    -1,    65,     6,   234,    -1,    66,     6,   234,
      -1,    73,   171,    -1,   171,    -1,    -1,   171,   180,    -1,
     171,   182,    -1,    10,    83,    11,   203,    -1,    -1,    10,
      84,   173,    11,   176,    -1,    84,     3,   176,     4,    -1,
      22,     3,   174,     4,    -1,    10,    22,    11,   212,    -1,
      22,     6,   212,    -1,    10,    85,    11,   221,    -1,    10,
      91,    11,   223,    -1,    10,    51,    11,   190,    -1,    86,
       3,   177,     4,    -1,   175,    -1,   175,     5,   174,    -1,
     212,    -1,    25,     6,   234,    -1,   179,    -1,   179,     5,
     176,    -1,   178,    -1,   178,     5,   177,    -1,   178,   177,
      -1,    40,     6,   234,    -1,    87,     6,    77,    -1,    88,
       6,   234,    -1,   204,    -1,   205,    -1,   207,    -1,   208,
      -1,   209,    -1,   211,    -1,   210,    -1,    89,    -1,    90,
       6,   234,    -1,   206,    -1,    92,     6,   234,    -1,    61,
      -1,    62,    -1,    63,    -1,    -1,    10,   181,   186,    11,
     226,    -1,   226,    -1,    -1,     3,   183,   184,     4,    -1,
     187,    -1,   185,    -1,   185,     5,   184,    -1,   187,     5,
     184,    -1,   185,   184,    -1,   187,   184,    -1,    74,     6,
     226,    -1,    68,     6,   234,    -1,    20,     6,   193,    -1,
      20,     6,   102,    -1,    53,     6,    76,    -1,   187,    -1,
     187,     5,   186,    -1,   193,    -1,   196,    -1,   216,    -1,
     217,    -1,   188,    -1,    49,     6,   189,    -1,    50,     6,
     192,    -1,   195,    -1,   195,     9,   189,    -1,   191,    -1,
     191,     5,   190,    -1,   234,    -1,   234,    -1,    12,    -1,
      43,    -1,    44,    -1,    45,    -1,    46,    -1,    47,    -1,
      48,    -1,    14,    -1,    15,    -1,    13,    -1,   105,    -1,
     103,    -1,   218,    -1,   220,    -1,   202,    -1,    27,    -1,
      27,     6,   133,    -1,   234,    -1,   234,   148,   234,   149,
      -1,   234,   148,   150,   149,    -1,    16,     6,   198,    -1,
      17,     6,   199,    -1,    18,     6,   200,    -1,    19,     6,
     201,    -1,    24,     6,   213,    -1,    23,     6,   214,    -1,
      26,     6,   215,    -1,   107,     6,   226,    -1,   109,     6,
     226,    -1,   110,     6,   226,    -1,   111,     6,   226,    -1,
     112,     6,   226,    -1,    38,     6,   222,    -1,    37,     6,
     234,    -1,   113,     6,   226,    -1,   194,    -1,    20,     6,
     202,    -1,    83,     6,   203,    -1,    40,     6,   234,    -1,
     135,    -1,    57,     6,   234,    -1,   126,    -1,   105,    -1,
     218,    -1,    96,    -1,    97,    -1,    98,    -1,    99,    -1,
     100,    -1,   101,    -1,    80,    -1,   116,    -1,    80,    -1,
     117,    -1,    80,    -1,   118,    -1,   119,    -1,   120,    -1,
     121,    -1,   122,    -1,   123,    -1,   135,    -1,   124,    -1,
     125,    -1,   126,    -1,   127,    -1,   128,    -1,   129,    -1,
     131,    -1,   130,    -1,   132,    -1,    28,     6,   234,    -1,
      29,     6,   234,    -1,    30,     6,   234,    -1,    31,     6,
     234,    -1,    32,     6,   234,    -1,    33,     6,   234,    -1,
      34,    -1,    35,    -1,    21,     6,   234,    -1,    52,     6,
     226,    -1,    54,     6,   226,    -1,   107,    -1,   219,    -1,
     113,    -1,   114,    -1,   115,    -1,    94,    -1,    93,    -1,
      95,    -1,   108,    -1,   109,    -1,   110,    -1,   111,    -1,
     112,    -1,   106,    -1,   102,    -1,   104,    -1,    36,     6,
     234,    -1,    38,     6,   222,    -1,    37,     6,   234,    -1,
      39,    -1,   221,     9,   221,    -1,   136,    -1,    41,     6,
     224,    -1,    42,     6,   225,    -1,   223,     5,   223,    -1,
     134,    -1,    55,    -1,    56,    -1,    78,    -1,    79,    -1,
      82,    -1,    81,    -1,   148,   234,   149,    -1,   227,    -1,
     143,   228,    -1,   147,   228,    -1,   228,    -1,   229,   140,
     228,    -1,   229,   141,   228,    -1,   229,   142,   228,    -1,
     229,    -1,   230,   143,   229,    -1,   230,   144,   229,    -1,
     230,    -1,   231,   145,   230,    -1,   231,   146,   230,    -1,
     231,    -1,   232,   139,   231,    -1,   232,    -1,   233,   138,
     232,    -1,   233,    -1,   234,   137,   233,    -1
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
     125,   130,   134,   138,   142,   146,   150,   154,   158,   160,
     163,   164,   170,   171,   174,   177,   180,   182,   186,   189,
     193,   197,   199,   201,   205,   209,   213,   217,   221,   224,
     226,   227,   230,   233,   238,   239,   245,   250,   255,   260,
     264,   269,   274,   279,   284,   286,   290,   292,   296,   298,
     302,   304,   308,   311,   315,   319,   323,   325,   327,   329,
     331,   333,   335,   337,   339,   343,   345,   349,   351,   353,
     355,   356,   362,   364,   365,   370,   372,   374,   378,   382,
     385,   388,   392,   396,   400,   404,   408,   410,   414,   416,
     418,   420,   422,   424,   428,   432,   434,   438,   440,   444,
     446,   448,   450,   452,   454,   456,   458,   460,   462,   464,
     466,   468,   470,   472,   474,   476,   478,   480,   484,   486,
     491,   496,   500,   504,   508,   512,   516,   520,   524,   528,
     532,   536,   540,   544,   548,   552,   556,   558,   562,   566,
     570,   572,   576,   578,   580,   582,   584,   586,   588,   590,
     592,   594,   596,   598,   600,   602,   604,   606,   608,   610,
     612,   614,   616,   618,   620,   622,   624,   626,   628,   630,
     632,   634,   636,   640,   644,   648,   652,   656,   660,   662,
     664,   668,   672,   676,   678,   680,   682,   684,   686,   688,
     690,   692,   694,   696,   698,   700,   702,   704,   706,   708,
     712,   716,   720,   722,   726,   728,   732,   736,   740,   742,
     744,   746,   748,   750,   752,   754,   758,   760,   763,   766,
     768,   772,   776,   780,   782,   786,   790,   792,   796,   800,
     802,   806,   808,   812,   814
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   196,   196,   198,   199,   202,   202,   208,   209,   210,
     211,   212,   213,   214,   215,   218,   218,   223,   224,   225,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
     238,   239,   240,   241,   242,   243,   247,   248,   251,   253,
     255,   259,   260,   261,   262,   263,   264,   265,   268,   269,
     272,   272,   276,   277,   278,   279,   282,   283,   284,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   299,   300,
     303,   304,   305,   308,   311,   311,   314,   315,   316,   321,
     322,   323,   324,   325,   328,   329,   332,   333,   336,   337,
     340,   341,   342,   345,   346,   347,   350,   353,   355,   357,
     359,   361,   363,   365,   367,   369,   375,   379,   380,   381,
     384,   384,   391,   397,   397,   403,   404,   405,   406,   407,
     408,   411,   414,   415,   416,   417,   422,   423,   426,   427,
     428,   429,   430,   433,   434,   437,   438,   441,   442,   445,
     450,   452,   453,   454,   455,   456,   457,   458,   459,   460,
     461,   464,   465,   466,   467,   468,   471,   472,   475,   476,
     477,   479,   480,   481,   482,   483,   484,   485,   486,   487,
     488,   489,   490,   491,   492,   493,   494,   495,   496,   497,
     498,   499,   500,   503,   504,   505,   506,   507,   508,   509,
     510,   513,   514,   517,   518,   521,   522,   525,   528,   531,
     534,   537,   540,   543,   546,   549,   552,   555,   558,   561,
     564,   567,   570,   571,   572,   573,   574,   575,   576,   577,
     578,   581,   582,   585,   586,   587,   588,   589,   590,   591,
     592,   593,   596,   597,   598,   599,   602,   603,   604,   607,
     610,   613,   616,   619,   622,   625,   628,   629,   632,   635,
     638,   644,   645,   648,   649,   650,   656,   657,   658,   662,
     663,   664,   665,   668,   669,   670,   673,   674,   675,   678,
     679,   682,   683,   687,   688
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
     145,   146,   147,   148,   149,   150
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 907;
  const int BisonParser::yynnts_ = 84;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 151;

  const unsigned int BisonParser::yyuser_token_number_max_ = 405;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 23 "../s/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 2397 "../bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 693 "../s/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
