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

BifOptions* currentBifOptions;
PartitionBifOptions* currentPartitionBifOptions ;

/* Line 279 of lalr1.cc  */
#line 51 "../bisonflex/bif.tab.cpp"


#include "bif.tab.hpp"

/* User implementation prologue.  */

/* Line 285 of lalr1.cc  */
#line 59 "../bisonflex/bif.tab.cpp"
/* Unqualified %code blocks.  */
/* Line 286 of lalr1.cc  */
#line 46 "../s/bif.y"

    static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner);
    #include "options.h"


/* Line 286 of lalr1.cc  */
#line 69 "../bisonflex/bif.tab.cpp"


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
#line 165 "../bisonflex/bif.tab.cpp"

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
#line 329 "../bisonflex/bif.tab.cpp"

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
#line 194 "../s/bif.y"
    { currentBifOptions = new BifOptions(options.GetArchType(),(yysemantic_stack_[(1) - (1)].string)); }
    break;

  case 6:
/* Line 670 of lalr1.cc  */
#line 196 "../s/bif.y"
    { options.bifOptions=currentBifOptions; }
    break;

  case 10:
/* Line 670 of lalr1.cc  */
#line 204 "../s/bif.y"
    { currentBifOptions->SetEncryptionKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); options.SetEncryptedKeySource((yysemantic_stack_[(4) - (4)].encrkeysrc_t)); }
    break;

  case 11:
/* Line 670 of lalr1.cc  */
#line 205 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                                        LOG_ERROR("BIF attribute error !!!\n\t\t[fsbl_config] not supported in ZYNQ architecture"); }
    break;

  case 13:
/* Line 670 of lalr1.cc  */
#line 208 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) {
                                                                          LOG_ERROR("BIF attribute error !!!\n\t\t[bootdevice] not supported in ZYNQ architecture"); }
                                                                      currentBifOptions->SetBootDevice((yysemantic_stack_[(4) - (4)].bootdevice_t)); 
                                                                    }
    break;

  case 19:
/* Line 670 of lalr1.cc  */
#line 221 "../s/bif.y"
    { currentBifOptions->SetCore((yysemantic_stack_[(1) - (1)].core_t));
                                  LOG_WARNING("[fsbl_config] a53_x64 | a53_x32 | r5_single | r5_dual is no more supported. Use 'destination_cpu' attribute for bootloader partition"); }
    break;

  case 20:
/* Line 670 of lalr1.cc  */
#line 224 "../s/bif.y"
    { currentBifOptions->SetBhRsa((yysemantic_stack_[(1) - (1)].bhrsa_t)); }
    break;

  case 21:
/* Line 670 of lalr1.cc  */
#line 226 "../s/bif.y"
    { LOG_ERROR("Authentication using SHA2 is no more supported."); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 228 "../s/bif.y"
    { LOG_ERROR("[fsbl_config] bi_integrity_sha3 is no more supported. Use 'checksum' attribute of bootloader partition"); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 230 "../s/bif.y"
    { currentBifOptions->SetPufHdLoc((yysemantic_stack_[(1) - (1)].pufhdloc_t)); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 232 "../s/bif.y"
    { currentBifOptions->SetAuthOnly((yysemantic_stack_[(1) - (1)].authonly_t)); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 234 "../s/bif.y"
    { currentBifOptions->SetOptKey((yysemantic_stack_[(1) - (1)].optkey_t)); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 236 "../s/bif.y"
    { currentBifOptions->SetPufMode(PufMode::PUF4K); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 239 "../s/bif.y"
    { currentBifOptions->SetShutterValue((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 243 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                  currentPartitionBifOptions->SetArchType(options.GetArchType()); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 246 "../s/bif.y"
    { currentPartitionBifOptions->filename = (yysemantic_stack_[(5) - (5)].string); 
                                  currentBifOptions->Add(currentPartitionBifOptions);
                                }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 249 "../s/bif.y"
    { currentPartitionBifOptions = new PartitionBifOptions();
                                  currentPartitionBifOptions->SetArchType(options.GetArchType());
                                  currentPartitionBifOptions->filename = (yysemantic_stack_[(1) - (1)].string); 
                                  currentBifOptions->Add(currentPartitionBifOptions);
                                }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 278 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                    LOG_ERROR("BIF attribute error !!!\n\t\t[bootvectors] not supported in ZYNQ architecture");
                                  currentBifOptions->SetBootVectorArray((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 283 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthBlockAttr((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 285 "../s/bif.y"
    { currentPartitionBifOptions->bootloader = true;}
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 286 "../s/bif.y"
    { currentPartitionBifOptions->boot = true;}
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 287 "../s/bif.y"
    { currentPartitionBifOptions->user = true;}
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 288 "../s/bif.y"
    { currentPartitionBifOptions->Static = true;}
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 289 "../s/bif.y"
    { currentPartitionBifOptions->noautostart = true;}
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 290 "../s/bif.y"
    { currentPartitionBifOptions->multiboot = true;}
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 291 "../s/bif.y"
    { currentPartitionBifOptions->Protected = true;}
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 292 "../s/bif.y"
    { currentPartitionBifOptions->SetEarlyHandoff(true); }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 293 "../s/bif.y"
    { currentPartitionBifOptions->SetHivec(true); }
    break;

  case 55:
/* Line 670 of lalr1.cc  */
#line 294 "../s/bif.y"
    { if(currentPartitionBifOptions->bootloader!=true) 
                                    LOG_ERROR("XIP mode can be enabled only for bootloader"); 
                                  currentBifOptions->SetXipMode();}
    break;

  case 56:
/* Line 670 of lalr1.cc  */
#line 297 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 298 "../s/bif.y"
    { currentPartitionBifOptions->bootImage = true; }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 299 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 59:
/* Line 670 of lalr1.cc  */
#line 300 "../s/bif.y"
    { currentPartitionBifOptions->fileType = (yysemantic_stack_[(1) - (1)].number); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 303 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone(::TrustZone::Secure); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 304 "../s/bif.y"
    { currentPartitionBifOptions->SetTrustZone((yysemantic_stack_[(3) - (3)].trustzone_t)); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 307 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 308 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), (yysemantic_stack_[(4) - (3)].number)); }
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 309 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptionBlocks((yysemantic_stack_[(4) - (1)].number), 0); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 311 "../s/bif.y"
    { currentPartitionBifOptions->SetAuthType((yysemantic_stack_[(3) - (3)].authvalue_t)); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 312 "../s/bif.y"
    { currentPartitionBifOptions->SetEncryptType((yysemantic_stack_[(3) - (3)].encrvalue_t)); }
    break;

  case 67:
/* Line 670 of lalr1.cc  */
#line 313 "../s/bif.y"
    { currentPartitionBifOptions->SetChecksumType((yysemantic_stack_[(3) - (3)].checksumvalue_t)); }
    break;

  case 68:
/* Line 670 of lalr1.cc  */
#line 314 "../s/bif.y"
    { currentPartitionBifOptions->SetOwnerType((yysemantic_stack_[(3) - (3)].powner_t)); }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 315 "../s/bif.y"
    { currentPartitionBifOptions->SetDestCpu((yysemantic_stack_[(3) - (3)].destcpu_t)); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 316 "../s/bif.y"
    { currentPartitionBifOptions->SetDestDevice((yysemantic_stack_[(3) - (3)].destdevice_t));  }
    break;

  case 71:
/* Line 670 of lalr1.cc  */
#line 317 "../s/bif.y"
    { currentPartitionBifOptions->SetExceptionLevel((yysemantic_stack_[(3) - (3)].el_t)); }
    break;

  case 72:
/* Line 670 of lalr1.cc  */
#line 318 "../s/bif.y"
    { currentPartitionBifOptions->aesKeyFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 319 "../s/bif.y"
    { currentPartitionBifOptions->spkFile=((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 320 "../s/bif.y"
    { currentPartitionBifOptions->sskFile=((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 75:
/* Line 670 of lalr1.cc  */
#line 321 "../s/bif.y"
    { currentPartitionBifOptions->spkSelect=((yysemantic_stack_[(3) - (3)].spkselect_t)); currentPartitionBifOptions->spkSelLocal = true; }
    break;

  case 76:
/* Line 670 of lalr1.cc  */
#line 322 "../s/bif.y"
    { currentPartitionBifOptions->spkId=((yysemantic_stack_[(3) - (3)].number)); currentPartitionBifOptions->spkIdLocal = true; }
    break;

  case 77:
/* Line 670 of lalr1.cc  */
#line 323 "../s/bif.y"
    { currentPartitionBifOptions->spkSignatureFile=((yysemantic_stack_[(3) - (3)].string)); }
    break;

  case 79:
/* Line 670 of lalr1.cc  */
#line 327 "../s/bif.y"
    { (yyval.authvalue_t) = ::Authentication::None;}
    break;

  case 81:
/* Line 670 of lalr1.cc  */
#line 331 "../s/bif.y"
    { (yyval.encrvalue_t) = ::Encryption::None;}
    break;

  case 83:
/* Line 670 of lalr1.cc  */
#line 335 "../s/bif.y"
    { (yyval.checksumvalue_t) = ::Checksum::None;}
    break;

  case 98:
/* Line 670 of lalr1.cc  */
#line 378 "../s/bif.y"
    { currentPartitionBifOptions->alignment= (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 99:
/* Line 670 of lalr1.cc  */
#line 379 "../s/bif.y"
    { currentPartitionBifOptions->offset   = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 380 "../s/bif.y"
    { currentPartitionBifOptions->reserve  = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 381 "../s/bif.y"
    { currentPartitionBifOptions->load     = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 382 "../s/bif.y"
    { currentPartitionBifOptions->startup  = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 103:
/* Line 670 of lalr1.cc  */
#line 383 "../s/bif.y"
    { currentPartitionBifOptions->pid  = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 386 "../s/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 387 "../s/bif.y"
    { currentPartitionBifOptions->udfDataFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 411 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 414 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 417 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 420 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 429 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 439 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                    LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 442 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                    LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 137:
/* Line 670 of lalr1.cc  */
#line 454 "../s/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 463 "../s/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 464 "../s/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 469 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 470 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 471 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 476 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 477 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 482 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 150:
/* Line 670 of lalr1.cc  */
#line 483 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 152:
/* Line 670 of lalr1.cc  */
#line 488 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 493 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 499 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 971 "../bisonflex/bif.tab.cpp"
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
  const signed char BisonParser::yypact_ninf_ = -116;
  const short int
  BisonParser::yypact_[] =
  {
      -116,     5,   -43,  -116,  -116,  -116,    18,    45,  -116,   107,
    -116,    68,  -116,  -116,  -116,  -116,  -116,    39,    49,    63,
    -116,    73,   113,    28,   -16,    29,   -10,   116,    59,   -29,
    -116,  -116,  -116,  -116,   112,   122,   129,   130,   132,   134,
     144,   147,   148,   149,   150,   151,   152,   153,   154,   155,
    -116,  -116,  -116,  -116,  -116,  -116,   156,   159,   160,   161,
    -116,  -116,  -116,  -116,  -116,  -116,  -116,   162,  -116,  -116,
    -116,   163,   164,   165,  -116,  -116,   166,   167,  -116,  -116,
    -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,
    -116,    29,    29,    29,  -116,   168,  -116,  -116,    16,   -95,
     -24,    70,    72,    74,  -116,  -116,    55,   172,   173,   174,
    -116,   175,   176,   177,   180,   -56,   -47,   -57,    95,    29,
      90,    91,    89,    92,    29,    29,    29,    29,    29,    29,
      88,    29,    29,    56,    56,    56,    56,    56,    56,    56,
      28,  -116,  -116,   -99,    29,    29,    29,    29,    29,    29,
      29,    29,    29,    29,    29,  -116,   184,  -116,  -116,  -116,
    -116,  -116,  -116,  -116,  -116,   187,  -116,  -116,  -116,  -116,
    -116,  -116,  -116,    29,    29,    88,    59,    93,    66,   -29,
    -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,
    -116,    74,  -116,  -116,  -116,  -116,  -116,  -116,  -116,    74,
      74,    74,    74,    74,    74,  -116,  -116,  -116,   185,   -96,
    -116,    74,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,
    -116,  -116,  -116,  -116,  -116,    16,    16,   -95,   -95,   -24,
      70,    72,    29,    55,    74,    74,  -116,   175,  -116,  -116,
    -116,  -116,  -116,   180,    29,   -30,    74,  -116,  -116,    81,
     -98,  -116,  -116
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     5,     4,     0,     0,     7,     0,
       6,    28,   133,   134,     9,     8,    30,     0,     0,     0,
      11,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      46,    55,    53,    54,     0,     0,     0,     0,     0,     0,
       0,     0,    60,     0,     0,     0,     0,     0,     0,     0,
      47,    48,    49,    50,    51,    52,     0,     0,     0,     0,
     112,   111,   113,    57,   120,    56,   119,   106,   114,   115,
     116,   117,   118,   108,   109,   110,     0,    31,    37,    33,
      78,    34,    35,    36,    58,   107,    59,    94,    13,   136,
     135,     0,     0,     0,    16,    42,   138,   141,   145,   148,
     151,   153,   155,    44,    86,    10,     0,     0,     0,     0,
     124,    14,     0,     0,    15,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   139,   140,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    26,     0,    87,    88,    89,
      90,    91,    92,    93,    12,    17,    19,    20,    21,    22,
      23,    25,    24,     0,     0,     0,     0,     0,     0,     0,
      79,    80,    65,    81,    82,    66,    83,    84,    67,    85,
      68,   103,    96,    70,    95,    69,    97,    71,    61,    98,
      99,   100,   101,   102,    76,   126,    75,    38,    40,    62,
      39,    45,   104,   105,    72,    73,    74,    77,    29,    32,
     137,    43,   142,   143,   144,   146,   147,   149,   150,   152,
     154,   156,     0,     0,   121,   123,   122,   125,   130,   127,
     131,   132,   128,   129,     0,     0,    27,    18,    41,     0,
       0,    64,    63
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  BisonParser::yypgoto_[] =
  {
      -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,   -37,  -116,
    -116,  -116,    57,  -116,  -116,   -46,    58,  -116,  -116,  -116,
    -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,
    -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,
    -116,  -116,  -116,  -116,    23,    25,    22,  -116,  -116,  -115,
    -116,   -82,   -23,   -18,    51,    52,    50,   -93
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     5,     6,     9,    14,    27,   164,   165,
      15,    23,    76,    77,    78,   207,    94,    95,   210,    79,
      80,   208,    81,   182,   185,   188,   190,   105,   166,   167,
     168,   169,   170,   171,   172,    88,   195,   193,   197,    82,
      83,    84,    85,    86,   111,   206,   114,   239,   242,    16,
      96,    97,    98,    99,   100,   101,   102,   103
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned char
  BisonParser::yytable_[] =
  {
       143,   186,   180,   154,   154,     3,   154,   112,   113,   141,
     142,   183,     4,   148,   149,   220,   252,   245,   212,   213,
     214,   215,   216,   217,   218,     7,   191,   181,   187,    89,
      90,   199,   200,   201,   202,   203,   204,   184,   209,   211,
      30,    31,    32,    33,    34,    35,    36,    37,     8,    38,
      24,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      25,    48,    49,   222,   223,   224,    50,    51,    52,    53,
      54,    55,    56,    57,    26,    58,    59,   104,    91,    87,
     234,   235,    92,    93,    28,   249,   150,   151,    89,    90,
      17,   107,   108,   109,   110,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    10,    12,    13,    18,   240,   241,    11,   115,   155,
     156,   145,   146,   147,    29,   225,   226,   106,   116,    19,
      20,    21,   227,   228,    22,   117,   118,    91,   119,   246,
     120,    92,    93,   157,   158,   159,   160,   161,   162,   163,
     121,   209,   250,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,    12,    13,   132,   133,   134,   135,   136,
     137,   138,   140,   144,   152,   153,   154,   139,   173,   174,
     175,   189,   177,   178,   176,   179,   192,   196,   194,   205,
     232,   198,   233,   238,   244,   251,   247,   219,   248,   237,
     236,   243,   221,   229,   231,   230
  };

  /* YYCHECK.  */
  const unsigned char
  BisonParser::yycheck_[] =
  {
        93,    58,    58,   102,   102,     0,   102,    36,    37,    91,
      92,    58,    55,   108,   109,   114,   114,   113,   133,   134,
     135,   136,   137,   138,   139,     7,   119,    83,    85,    59,
      60,   124,   125,   126,   127,   128,   129,    84,   131,   132,
      12,    13,    14,    15,    16,    17,    18,    19,     3,    21,
      11,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      11,    33,    34,   145,   146,   147,    38,    39,    40,    41,
      42,    43,    44,    45,    11,    47,    48,    87,   108,    95,
     173,   174,   112,   113,    11,   115,   110,   111,    59,    60,
      22,    32,    33,    34,    35,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,     4,    56,    57,    46,    49,    50,    10,     6,    64,
      65,   105,   106,   107,    11,   148,   149,    11,     6,    61,
      62,    63,   150,   151,    66,     6,     6,   108,     6,   232,
       6,   112,   113,    88,    89,    90,    91,    92,    93,    94,
       6,   244,   245,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,    56,    57,     6,     6,     6,     6,     6,
       6,     6,     5,     5,   104,   103,   102,    11,     6,     6,
       6,    86,     6,     6,     9,     5,    96,    98,    97,   101,
       6,    99,     5,   100,     9,   114,   233,   140,   244,   176,
     175,   179,   144,   152,   154,   153
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   117,   118,     0,    55,   119,   120,     7,     3,   121,
       4,    10,    56,    57,   122,   126,   165,    22,    46,    61,
      62,    63,    66,   127,    11,    11,    11,   123,    11,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    21,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    33,    34,
      38,    39,    40,    41,    42,    43,    44,    45,    47,    48,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,   128,   129,   130,   135,
     136,   138,   155,   156,   157,   158,   159,    95,   151,    59,
      60,   108,   112,   113,   132,   133,   166,   167,   168,   169,
     170,   171,   172,   173,    87,   143,    11,    32,    33,    34,
      35,   160,    36,    37,   162,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,    11,
       5,   167,   167,   173,     5,   105,   106,   107,   108,   109,
     110,   111,   104,   103,   102,    64,    65,    88,    89,    90,
      91,    92,    93,    94,   124,   125,   144,   145,   146,   147,
     148,   149,   150,     6,     6,     6,     9,     6,     6,     5,
      58,    83,   139,    58,    84,   140,    58,    85,   141,    86,
     142,   173,    96,   153,    97,   152,    98,   154,    99,   173,
     173,   173,   173,   173,   173,   101,   161,   131,   137,   173,
     134,   173,   165,   165,   165,   165,   165,   165,   165,   128,
     114,   132,   167,   167,   167,   168,   168,   169,   169,   170,
     171,   172,     6,     5,   173,   173,   161,   160,   100,   163,
      49,    50,   164,   162,     9,   113,   173,   124,   131,   115,
     173,   114,   114
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
     365,   366,   367,   368,   369,   370
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   116,   117,   118,   118,   120,   119,   121,   121,   121,
     122,   123,   122,   122,   122,   122,   122,   124,   124,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   127,   126,
     126,   128,   128,   129,   129,   129,   129,   129,   130,   130,
     131,   131,   132,   132,   133,   134,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     136,   136,   137,   137,   137,   138,   138,   138,   138,   138,
     138,   138,   138,   138,   138,   138,   138,   138,   138,   139,
     139,   140,   140,   141,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   155,
     155,   155,   155,   155,   156,   156,   157,   157,   157,   157,
     157,   157,   157,   157,   157,   158,   158,   158,   158,   159,
     159,   160,   160,   160,   160,   160,   161,   162,   162,   162,
     163,   164,   164,   165,   165,   166,   166,   166,   167,   167,
     167,   168,   168,   168,   168,   169,   169,   169,   170,   170,
     170,   171,   171,   172,   172,   173,   173
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     0,     6,     0,     2,     2,
       4,     0,     5,     4,     4,     4,     4,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     0,     5,
       1,     1,     3,     1,     1,     1,     1,     1,     3,     3,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     1,     4,     4,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     3,     3,     3,     1,     3,     1,     3,     3,     3,
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
  "XIP_MODE", "EARLY_HANDOFF", "HIVEC", "AUTHENTICATION", "ENCRYPTION",
  "CHECKSUM", "PARTITION_OWNER", "PARTITION_TYPE", "PARTITION_NUM",
  "BOOT_DEVICE", "DEST_DEVICE", "DEST_CPU", "EXCEPTION_LEVEL", "TRUSTZONE",
  "ALIGNMENT", "OFFSET", "RESERVE", "LOAD", "STARTUP", "PPK_SELECT",
  "SPK_ID", "SPK_SELECT", "HEADER_AUTH", "SPLIT_MODE", "SPLIT_FMT", "BOOT",
  "USER", "STATIC", "NOAUTOSTART", "MULTIBOOT", "PROTECTED", "BLOCKS",
  "AUTHBLOCKS", "BOOTVECTORS", "PRESIGN", "UDF_DATA", "MCS", "BIN",
  "IMAGE", "ID", "NAME", "PFILE", "WORD", "FILENAME", "QFILENAME", "NONE",
  "DECVALUE", "HEXVALUE", "KEYSRC_ENCRYPTION", "FSBL_CONFIG",
  "AUTH_PARAMS", "PUF4KMODE", "SHUTTER", "SPLIT", "PUF_HELPER_FILE",
  "BH_KEY_FILE", "BH_KEY_IV", "BOOTIMAGE", "UDF_BH", "INIT", "PMUFW_IMAGE",
  "AES_KEY_FILE", "FAMILY_KEY", "PPK_FILE", "PSK_FILE", "SPK_FILE",
  "SSK_FILE", "SPK_SIGNATURE_FILE", "BH_SIGNATURE_FILE",
  "HEADER_SIGNATURE_FILE", "AUTHVALUE", "ENCRVALUE", "CHECKSUMVALUE",
  "POWNERVALUE", "KEY_SRC", "CORE", "BH_RSA", "AUTH_HASH", "INT_HASH",
  "PUFHD_LOC", "OPT_KEY", "AUTH_ONLY", "BOOT_DEVICE_TYPE",
  "DEST_DEVICE_TYPE", "DEST_CPU_TYPE", "EXCEPTION_LEVEL_TYPE",
  "TRUSTZONE_TYPE", "SPLITMODE", "SPKSELECT", "OR", "XOR", "AND", "MULT",
  "DIVIDE", "MODULO", "PLUS", "MINUS", "LSHIFT", "RSHIFT", "NEGATION",
  "LPAREN", "RPAREN", "ASTERISK", "$accept", "bif", "group_list",
  "bifoptions", "$@1", "file_list", "other_spec", "$@2", "fsbl_attr_list",
  "fsbl_attr", "file_spec", "$@3", "attribute_list", "attribute",
  "blocksattr_list", "blocksattr", "bootvectors_list", "bootvector",
  "authblockattr", "boolattr", "trustzone_type", "blockattr", "optattr",
  "authvalue", "encrvalue", "checkvalue", "pownervalue", "key_src", "core",
  "bh_rsa", "auth_hash", "int_hash", "pufhd_loc", "opt_key", "auth_only",
  "boot_device_type", "dest_cpu_type", "dest_device_type",
  "exception_level_type", "numattr", "fileattr", "key_file",
  "rsa_key_file", "other_files", "auth_parameters", "spkselect",
  "split_options", "splitmode", "splitfmt", "filename", "number",
  "unary_expression", "multiplicative_expression", "additive_expression",
  "shift_expression", "and_expression", "xor_expression", "expression", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BisonParser::rhs_number_type
  BisonParser::yyrhs_[] =
  {
       117,     0,    -1,   118,    -1,    -1,   118,   119,    -1,    -1,
      55,   120,     7,     3,   121,     4,    -1,    -1,   121,   126,
      -1,   121,   122,    -1,    10,    61,    11,   143,    -1,    -1,
      10,    62,   123,    11,   124,    -1,    10,    22,    11,   151,
      -1,    10,    63,    11,   160,    -1,    10,    66,    11,   162,
      -1,    10,    46,    11,   132,    -1,   125,    -1,   125,     5,
     124,    -1,   144,    -1,   145,    -1,   146,    -1,   147,    -1,
     148,    -1,   150,    -1,   149,    -1,    64,    -1,    65,     6,
     173,    -1,    -1,    10,   127,   128,    11,   165,    -1,   165,
      -1,   129,    -1,   129,     5,   128,    -1,   135,    -1,   138,
      -1,   155,    -1,   156,    -1,   130,    -1,    44,     6,   131,
      -1,    45,     6,   134,    -1,   137,    -1,   137,     9,   131,
      -1,   133,    -1,   133,     5,   132,    -1,   173,    -1,   173,
      -1,    12,    -1,    38,    -1,    39,    -1,    40,    -1,    41,
      -1,    42,    -1,    43,    -1,    14,    -1,    15,    -1,    13,
      -1,    72,    -1,    70,    -1,   157,    -1,   159,    -1,    26,
      -1,    26,     6,    99,    -1,   173,    -1,   173,   113,   173,
     114,    -1,   173,   113,   115,   114,    -1,    16,     6,   139,
      -1,    17,     6,   140,    -1,    18,     6,   141,    -1,    19,
       6,   142,    -1,    24,     6,   152,    -1,    23,     6,   153,
      -1,    25,     6,   154,    -1,    74,     6,   165,    -1,    78,
       6,   165,    -1,    79,     6,   165,    -1,    34,     6,   161,
      -1,    33,     6,   173,    -1,    80,     6,   165,    -1,   136,
      -1,    58,    -1,    83,    -1,    58,    -1,    84,    -1,    58,
      -1,    85,    -1,    86,    -1,    87,    -1,    88,    -1,    89,
      -1,    90,    -1,    91,    -1,    92,    -1,    93,    -1,    94,
      -1,    95,    -1,    97,    -1,    96,    -1,    98,    -1,    27,
       6,   173,    -1,    28,     6,   173,    -1,    29,     6,   173,
      -1,    30,     6,   173,    -1,    31,     6,   173,    -1,    21,
       6,   173,    -1,    47,     6,   165,    -1,    48,     6,   165,
      -1,    74,    -1,   158,    -1,    80,    -1,    81,    -1,    82,
      -1,    68,    -1,    67,    -1,    69,    -1,    75,    -1,    76,
      -1,    77,    -1,    78,    -1,    79,    -1,    73,    -1,    71,
      -1,    32,     6,   173,    -1,    34,     6,   161,    -1,    33,
       6,   173,    -1,    35,    -1,   160,     9,   160,    -1,   101,
      -1,    36,     6,   163,    -1,    37,     6,   164,    -1,   162,
       5,   162,    -1,   100,    -1,    49,    -1,    50,    -1,    56,
      -1,    57,    -1,    60,    -1,    59,    -1,   113,   173,   114,
      -1,   166,    -1,   108,   167,    -1,   112,   167,    -1,   167,
      -1,   168,   105,   167,    -1,   168,   106,   167,    -1,   168,
     107,   167,    -1,   168,    -1,   169,   108,   168,    -1,   169,
     109,   168,    -1,   169,    -1,   170,   110,   169,    -1,   170,
     111,   169,    -1,   170,    -1,   171,   104,   170,    -1,   171,
      -1,   172,   103,   171,    -1,   172,    -1,   173,   102,   172,
      -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     5,     6,     9,    10,    17,    18,    21,
      24,    29,    30,    36,    41,    46,    51,    56,    58,    62,
      64,    66,    68,    70,    72,    74,    76,    78,    82,    83,
      89,    91,    93,    97,    99,   101,   103,   105,   107,   111,
     115,   117,   121,   123,   127,   129,   131,   133,   135,   137,
     139,   141,   143,   145,   147,   149,   151,   153,   155,   157,
     159,   161,   165,   167,   172,   177,   181,   185,   189,   193,
     197,   201,   205,   209,   213,   217,   221,   225,   229,   231,
     233,   235,   237,   239,   241,   243,   245,   247,   249,   251,
     253,   255,   257,   259,   261,   263,   265,   267,   269,   273,
     277,   281,   285,   289,   293,   297,   301,   303,   305,   307,
     309,   311,   313,   315,   317,   319,   321,   323,   325,   327,
     329,   331,   335,   339,   343,   345,   349,   351,   355,   359,
     363,   365,   367,   369,   371,   373,   375,   377,   381,   383,
     386,   389,   391,   395,   399,   403,   405,   409,   413,   415,
     419,   423,   425,   429,   431,   435,   437
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   188,   188,   190,   191,   194,   194,   199,   200,   201,
     204,   205,   205,   208,   212,   213,   214,   217,   218,   221,
     224,   226,   228,   230,   232,   234,   236,   238,   243,   243,
     249,   255,   256,   259,   260,   261,   262,   263,   266,   267,
     270,   271,   274,   275,   278,   283,   285,   286,   287,   288,
     289,   290,   291,   292,   293,   294,   297,   298,   299,   300,
     303,   304,   307,   308,   309,   311,   312,   313,   314,   315,
     316,   317,   318,   319,   320,   321,   322,   323,   324,   327,
     328,   331,   332,   335,   336,   339,   342,   345,   348,   351,
     354,   357,   360,   363,   366,   369,   372,   375,   378,   379,
     380,   381,   382,   383,   386,   387,   390,   391,   392,   393,
     394,   395,   396,   397,   398,   401,   402,   403,   404,   407,
     408,   411,   414,   417,   420,   423,   426,   429,   432,   433,
     436,   439,   442,   448,   449,   452,   453,   454,   462,   463,
     464,   468,   469,   470,   471,   475,   476,   477,   481,   482,
     483,   487,   488,   492,   493,   498,   499
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
     115
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 205;
  const int BisonParser::yynnts_ = 58;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 116;

  const unsigned int BisonParser::yyuser_token_number_max_ = 370;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 23 "../s/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 1665 "../bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 502 "../s/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
