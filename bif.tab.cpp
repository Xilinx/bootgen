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
    { currentPartitionBifOptions->bigEndian  = true; }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 384 "../s/bif.y"
    { currentPartitionBifOptions->a32Mode  = true; }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 385 "../s/bif.y"
    { currentPartitionBifOptions->pid  = (yysemantic_stack_[(3) - (3)].number); }
    break;

  case 106:
/* Line 670 of lalr1.cc  */
#line 388 "../s/bif.y"
    { currentPartitionBifOptions->presignFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 107:
/* Line 670 of lalr1.cc  */
#line 389 "../s/bif.y"
    { currentPartitionBifOptions->udfDataFile = (yysemantic_stack_[(3) - (3)].string); }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 413 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetPPKSelection((yysemantic_stack_[(3) - (3)].number)); }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 416 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetSPKSelection((yysemantic_stack_[(3) - (3)].spkselect_t)); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 419 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetSpkId((yysemantic_stack_[(3) - (3)].number));  }
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 422 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP) 
                                                            LOG_ERROR("BIF attribute error !!!\n\t\t[auth_params] not supported in ZYNQ architecture");
                                                          currentBifOptions->SetHeaderAuthentication(); }
    break;

  case 129:
/* Line 670 of lalr1.cc  */
#line 431 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                                    LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                                  currentBifOptions->SetSplitMode((yysemantic_stack_[(3) - (3)].splitmode_t)); }
    break;

  case 133:
/* Line 670 of lalr1.cc  */
#line 441 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                    LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                  currentBifOptions->SetSplitFmt(File::MCS); }
    break;

  case 134:
/* Line 670 of lalr1.cc  */
#line 444 "../s/bif.y"
    { if(options.GetArchType() != Arch::ZYNQMP)
                                    LOG_ERROR("BIF attribute error !!!\n\t\t[split] not supported in ZYNQ architecture");
                                  currentBifOptions->SetSplitFmt(File::BIN); }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 456 "../s/bif.y"
    { (yyval.number) = (yysemantic_stack_[(3) - (2)].number); }
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 465 "../s/bif.y"
    {(yyval.number) =  (yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    + " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 466 "../s/bif.y"
    {(yyval.number) = ~(yysemantic_stack_[(2) - (2)].number);     *options.debugstr << (yyval.number) << "    ~ " << (yysemantic_stack_[(2) - (2)].number) << std::endl;}
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 471 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) *  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 472 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) /  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " / " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 473 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) %  (yysemantic_stack_[(3) - (3)].number); *options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " % " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 478 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) +  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " + " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 479 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) -  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " - " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 151:
/* Line 670 of lalr1.cc  */
#line 484 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) << (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " << " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 152:
/* Line 670 of lalr1.cc  */
#line 485 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) >> (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " >> " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 490 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) &  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " & " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 495 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) ^  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " ^ " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 501 "../s/bif.y"
    {(yyval.number) = (yysemantic_stack_[(3) - (1)].number) |  (yysemantic_stack_[(3) - (3)].number);*options.debugstr << (yyval.number) << " = " << (yysemantic_stack_[(3) - (1)].number)  << " | " << (yysemantic_stack_[(3) - (3)].number) << std::endl;}
    break;


/* Line 670 of lalr1.cc  */
#line 983 "../bisonflex/bif.tab.cpp"
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
  const signed char BisonParser::yypact_ninf_ = -118;
  const short int
  BisonParser::yypact_[] =
  {
      -118,     5,   -46,  -118,  -118,  -118,     6,    12,  -118,    75,
    -118,    67,  -118,  -118,  -118,  -118,  -118,    14,    37,    39,
    -118,    51,    56,    28,   -21,    26,    -3,   102,   -27,    55,
    -118,  -118,  -118,  -118,   114,   122,   123,   138,   139,   140,
     141,   142,   143,   144,   147,   148,   149,   150,  -118,  -118,
     151,   152,  -118,  -118,  -118,  -118,  -118,  -118,   153,   154,
     162,   163,  -118,  -118,  -118,  -118,  -118,  -118,  -118,   164,
    -118,  -118,  -118,   165,   166,   167,  -118,  -118,   168,   169,
    -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,
    -118,  -118,  -118,    26,    26,    26,  -118,   170,  -118,  -118,
     -17,   -15,    10,    70,    72,    74,  -118,  -118,    71,   174,
     175,   176,  -118,   177,   178,   179,   182,   -48,   -59,   -57,
      95,    26,    90,    91,    89,    92,    26,    26,    26,    26,
      26,    26,    88,    26,    26,    58,    58,    58,    58,    58,
      58,    58,    28,  -118,  -118,  -102,    26,    26,    26,    26,
      26,    26,    26,    26,    26,    26,    26,  -118,   186,  -118,
    -118,  -118,  -118,  -118,  -118,  -118,  -118,   189,  -118,  -118,
    -118,  -118,  -118,  -118,  -118,    26,    26,    88,   -27,    93,
      73,    55,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,
    -118,  -118,  -118,    74,  -118,  -118,  -118,  -118,  -118,  -118,
    -118,    74,    74,    74,    74,    74,    74,  -118,  -118,  -118,
     187,   -98,  -118,    74,  -118,  -118,  -118,  -118,  -118,  -118,
    -118,  -118,  -118,  -118,  -118,  -118,  -118,   -17,   -17,   -15,
     -15,    10,    70,    72,    26,    71,    74,    74,  -118,   177,
    -118,  -118,  -118,  -118,  -118,   182,    26,     4,    74,  -118,
    -118,    81,  -100,  -118,  -118
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     5,     4,     0,     0,     7,     0,
       6,    28,   135,   136,     9,     8,    30,     0,     0,     0,
      11,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      46,    55,    53,    54,     0,     0,     0,     0,     0,     0,
       0,     0,    60,     0,     0,     0,     0,     0,   103,   104,
       0,     0,    47,    48,    49,    50,    51,    52,     0,     0,
       0,     0,   114,   113,   115,    57,   122,    56,   121,   108,
     116,   117,   118,   119,   120,   110,   111,   112,     0,    31,
      37,    33,    78,    34,    35,    36,    58,   109,    59,    94,
      13,   138,   137,     0,     0,     0,    16,    42,   140,   143,
     147,   150,   153,   155,   157,    44,    86,    10,     0,     0,
       0,     0,   126,    14,     0,     0,    15,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   141,   142,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,    87,
      88,    89,    90,    91,    92,    93,    12,    17,    19,    20,
      21,    22,    23,    25,    24,     0,     0,     0,     0,     0,
       0,     0,    79,    80,    65,    81,    82,    66,    83,    84,
      67,    85,    68,   105,    96,    70,    95,    69,    97,    71,
      61,    98,    99,   100,   101,   102,    76,   128,    75,    38,
      40,    62,    39,    45,   106,   107,    72,    73,    74,    77,
      29,    32,   139,    43,   144,   145,   146,   148,   149,   151,
     152,   154,   156,   158,     0,     0,   123,   125,   124,   127,
     132,   129,   133,   134,   130,   131,     0,     0,    27,    18,
      41,     0,     0,    64,    63
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  BisonParser::yypgoto_[] =
  {
      -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,   -37,  -118,
    -118,  -118,    57,  -118,  -118,   -45,    54,  -118,  -118,  -118,
    -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,
    -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,  -118,
    -118,  -118,  -118,  -118,    24,    27,    22,  -118,  -118,  -117,
    -118,   -65,   -24,   -10,    52,    50,    53,   -95
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,     5,     6,     9,    14,    27,   166,   167,
      15,    23,    78,    79,    80,   209,    96,    97,   212,    81,
      82,   210,    83,   184,   187,   190,   192,   107,   168,   169,
     170,   171,   172,   173,   174,    90,   197,   195,   199,    84,
      85,    86,    87,    88,   113,   208,   116,   241,   244,    16,
      98,    99,   100,   101,   102,   103,   104,   105
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned char
  BisonParser::yytable_[] =
  {
       145,   185,   156,   188,   156,     3,   156,   109,   110,   111,
     112,     4,   182,     7,   222,     8,   254,   247,   214,   215,
     216,   217,   218,   219,   220,    24,   193,   186,   143,   144,
     189,   201,   202,   203,   204,   205,   206,   183,   211,   213,
      30,    31,    32,    33,    34,    35,    36,    37,    25,    38,
      26,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    28,    50,    51,    91,    92,    29,    52,    53,
      54,    55,    56,    57,    58,    59,    89,    60,    61,    10,
     236,   237,   224,   225,   226,    11,   106,    91,    92,    17,
     147,   148,   149,   114,   115,   150,   151,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,   108,    93,    18,    12,    13,    94,    95,
     117,   251,   152,   153,   242,   243,   227,   228,   118,   119,
      19,    20,    21,    12,    13,    22,    93,   157,   158,   248,
      94,    95,   229,   230,   120,   121,   122,   123,   124,   125,
     126,   211,   252,   127,   128,   129,   130,   131,   132,   133,
     134,   159,   160,   161,   162,   163,   164,   165,   135,   136,
     137,   138,   139,   140,   142,   146,   154,   155,   156,   141,
     175,   176,   177,   191,   179,   180,   178,   181,   194,   198,
     196,   207,   234,   200,   235,   240,   246,   253,   249,   221,
     223,   250,   239,   245,   238,   232,   231,     0,     0,   233
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
        95,    60,   104,    60,   104,     0,   104,    34,    35,    36,
      37,    57,    60,     7,   116,     3,   116,   115,   135,   136,
     137,   138,   139,   140,   141,    11,   121,    86,    93,    94,
      87,   126,   127,   128,   129,   130,   131,    85,   133,   134,
      12,    13,    14,    15,    16,    17,    18,    19,    11,    21,
      11,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    11,    35,    36,    61,    62,    11,    40,    41,
      42,    43,    44,    45,    46,    47,    97,    49,    50,     4,
     175,   176,   147,   148,   149,    10,    89,    61,    62,    22,
     107,   108,   109,    38,    39,   110,   111,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    11,   110,    48,    58,    59,   114,   115,
       6,   117,   112,   113,    51,    52,   150,   151,     6,     6,
      63,    64,    65,    58,    59,    68,   110,    66,    67,   234,
     114,   115,   152,   153,     6,     6,     6,     6,     6,     6,
       6,   246,   247,     6,     6,     6,     6,     6,     6,     6,
       6,    90,    91,    92,    93,    94,    95,    96,     6,     6,
       6,     6,     6,     6,     5,     5,   106,   105,   104,    11,
       6,     6,     6,    88,     6,     6,     9,     5,    98,   100,
      99,   103,     6,   101,     5,   102,     9,   116,   235,   142,
     146,   246,   178,   181,   177,   155,   154,    -1,    -1,   156
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   119,   120,     0,    57,   121,   122,     7,     3,   123,
       4,    10,    58,    59,   124,   128,   167,    22,    48,    63,
      64,    65,    68,   129,    11,    11,    11,   125,    11,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    21,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      35,    36,    40,    41,    42,    43,    44,    45,    46,    47,
      49,    50,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,   130,   131,
     132,   137,   138,   140,   157,   158,   159,   160,   161,    97,
     153,    61,    62,   110,   114,   115,   134,   135,   168,   169,
     170,   171,   172,   173,   174,   175,    89,   145,    11,    34,
      35,    36,    37,   162,    38,    39,   164,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,     6,     6,     6,     6,     6,     6,     6,     6,     6,
       6,    11,     5,   169,   169,   175,     5,   107,   108,   109,
     110,   111,   112,   113,   106,   105,   104,    66,    67,    90,
      91,    92,    93,    94,    95,    96,   126,   127,   146,   147,
     148,   149,   150,   151,   152,     6,     6,     6,     9,     6,
       6,     5,    60,    85,   141,    60,    86,   142,    60,    87,
     143,    88,   144,   175,    98,   155,    99,   154,   100,   156,
     101,   175,   175,   175,   175,   175,   175,   103,   163,   133,
     139,   175,   136,   175,   167,   167,   167,   167,   167,   167,
     167,   130,   116,   134,   169,   169,   169,   170,   170,   171,
     171,   172,   173,   174,     6,     5,   175,   175,   163,   162,
     102,   165,    51,    52,   166,   164,     9,   115,   175,   126,
     133,   117,   175,   116,   116
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
     365,   366,   367,   368,   369,   370,   371,   372
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   118,   119,   120,   120,   122,   121,   123,   123,   123,
     124,   125,   124,   124,   124,   124,   124,   126,   126,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   129,   128,
     128,   130,   130,   131,   131,   131,   131,   131,   132,   132,
     133,   133,   134,   134,   135,   136,   137,   137,   137,   137,
     137,   137,   137,   137,   137,   137,   137,   137,   137,   137,
     138,   138,   139,   139,   139,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   141,
     141,   142,   142,   143,   143,   144,   145,   146,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   157,
     157,   157,   157,   157,   157,   157,   158,   158,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   160,   160,   160,
     160,   161,   161,   162,   162,   162,   162,   162,   163,   164,
     164,   164,   165,   166,   166,   167,   167,   168,   168,   168,
     169,   169,   169,   170,   170,   170,   170,   171,   171,   171,
     172,   172,   172,   173,   173,   174,   174,   175,   175
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
       3,     3,     3,     1,     1,     3,     3,     3,     1,     1,
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
  "BOOT_DEVICE", "DEST_DEVICE", "DEST_CPU", "EXCEPTION_LEVEL", "TRUSTZONE",
  "ALIGNMENT", "OFFSET", "RESERVE", "LOAD", "STARTUP", "BIGENDIAN",
  "A32_MODE", "PPK_SELECT", "SPK_ID", "SPK_SELECT", "HEADER_AUTH",
  "SPLIT_MODE", "SPLIT_FMT", "BOOT", "USER", "STATIC", "NOAUTOSTART",
  "MULTIBOOT", "PROTECTED", "BLOCKS", "AUTHBLOCKS", "BOOTVECTORS",
  "PRESIGN", "UDF_DATA", "MCS", "BIN", "IMAGE", "ID", "NAME", "PFILE",
  "WORD", "FILENAME", "QFILENAME", "NONE", "DECVALUE", "HEXVALUE",
  "KEYSRC_ENCRYPTION", "FSBL_CONFIG", "AUTH_PARAMS", "PUF4KMODE",
  "SHUTTER", "SPLIT", "PUF_HELPER_FILE", "BH_KEY_FILE", "BH_KEY_IV",
  "BOOTIMAGE", "UDF_BH", "INIT", "PMUFW_IMAGE", "AES_KEY_FILE",
  "FAMILY_KEY", "PPK_FILE", "PSK_FILE", "SPK_FILE", "SSK_FILE",
  "SPK_SIGNATURE_FILE", "BH_SIGNATURE_FILE", "HEADER_SIGNATURE_FILE",
  "AUTHVALUE", "ENCRVALUE", "CHECKSUMVALUE", "POWNERVALUE", "KEY_SRC",
  "CORE", "BH_RSA", "AUTH_HASH", "INT_HASH", "PUFHD_LOC", "OPT_KEY",
  "AUTH_ONLY", "BOOT_DEVICE_TYPE", "DEST_DEVICE_TYPE", "DEST_CPU_TYPE",
  "EXCEPTION_LEVEL_TYPE", "TRUSTZONE_TYPE", "SPLITMODE", "SPKSELECT", "OR",
  "XOR", "AND", "MULT", "DIVIDE", "MODULO", "PLUS", "MINUS", "LSHIFT",
  "RSHIFT", "NEGATION", "LPAREN", "RPAREN", "ASTERISK", "$accept", "bif",
  "group_list", "bifoptions", "$@1", "file_list", "other_spec", "$@2",
  "fsbl_attr_list", "fsbl_attr", "file_spec", "$@3", "attribute_list",
  "attribute", "blocksattr_list", "blocksattr", "bootvectors_list",
  "bootvector", "authblockattr", "boolattr", "trustzone_type", "blockattr",
  "optattr", "authvalue", "encrvalue", "checkvalue", "pownervalue",
  "key_src", "core", "bh_rsa", "auth_hash", "int_hash", "pufhd_loc",
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
       119,     0,    -1,   120,    -1,    -1,   120,   121,    -1,    -1,
      57,   122,     7,     3,   123,     4,    -1,    -1,   123,   128,
      -1,   123,   124,    -1,    10,    63,    11,   145,    -1,    -1,
      10,    64,   125,    11,   126,    -1,    10,    22,    11,   153,
      -1,    10,    65,    11,   162,    -1,    10,    68,    11,   164,
      -1,    10,    48,    11,   134,    -1,   127,    -1,   127,     5,
     126,    -1,   146,    -1,   147,    -1,   148,    -1,   149,    -1,
     150,    -1,   152,    -1,   151,    -1,    66,    -1,    67,     6,
     175,    -1,    -1,    10,   129,   130,    11,   167,    -1,   167,
      -1,   131,    -1,   131,     5,   130,    -1,   137,    -1,   140,
      -1,   157,    -1,   158,    -1,   132,    -1,    46,     6,   133,
      -1,    47,     6,   136,    -1,   139,    -1,   139,     9,   133,
      -1,   135,    -1,   135,     5,   134,    -1,   175,    -1,   175,
      -1,    12,    -1,    40,    -1,    41,    -1,    42,    -1,    43,
      -1,    44,    -1,    45,    -1,    14,    -1,    15,    -1,    13,
      -1,    74,    -1,    72,    -1,   159,    -1,   161,    -1,    26,
      -1,    26,     6,   101,    -1,   175,    -1,   175,   115,   175,
     116,    -1,   175,   115,   117,   116,    -1,    16,     6,   141,
      -1,    17,     6,   142,    -1,    18,     6,   143,    -1,    19,
       6,   144,    -1,    24,     6,   154,    -1,    23,     6,   155,
      -1,    25,     6,   156,    -1,    76,     6,   167,    -1,    80,
       6,   167,    -1,    81,     6,   167,    -1,    36,     6,   163,
      -1,    35,     6,   175,    -1,    82,     6,   167,    -1,   138,
      -1,    60,    -1,    85,    -1,    60,    -1,    86,    -1,    60,
      -1,    87,    -1,    88,    -1,    89,    -1,    90,    -1,    91,
      -1,    92,    -1,    93,    -1,    94,    -1,    95,    -1,    96,
      -1,    97,    -1,    99,    -1,    98,    -1,   100,    -1,    27,
       6,   175,    -1,    28,     6,   175,    -1,    29,     6,   175,
      -1,    30,     6,   175,    -1,    31,     6,   175,    -1,    32,
      -1,    33,    -1,    21,     6,   175,    -1,    49,     6,   167,
      -1,    50,     6,   167,    -1,    76,    -1,   160,    -1,    82,
      -1,    83,    -1,    84,    -1,    70,    -1,    69,    -1,    71,
      -1,    77,    -1,    78,    -1,    79,    -1,    80,    -1,    81,
      -1,    75,    -1,    73,    -1,    34,     6,   175,    -1,    36,
       6,   163,    -1,    35,     6,   175,    -1,    37,    -1,   162,
       9,   162,    -1,   103,    -1,    38,     6,   165,    -1,    39,
       6,   166,    -1,   164,     5,   164,    -1,   102,    -1,    51,
      -1,    52,    -1,    58,    -1,    59,    -1,    62,    -1,    61,
      -1,   115,   175,   116,    -1,   168,    -1,   110,   169,    -1,
     114,   169,    -1,   169,    -1,   170,   107,   169,    -1,   170,
     108,   169,    -1,   170,   109,   169,    -1,   170,    -1,   171,
     110,   170,    -1,   171,   111,   170,    -1,   171,    -1,   172,
     112,   171,    -1,   172,   113,   171,    -1,   172,    -1,   173,
     106,   172,    -1,   173,    -1,   174,   105,   173,    -1,   174,
      -1,   175,   104,   174,    -1
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
     277,   281,   285,   289,   291,   293,   297,   301,   305,   307,
     309,   311,   313,   315,   317,   319,   321,   323,   325,   327,
     329,   331,   333,   335,   339,   343,   347,   349,   353,   355,
     359,   363,   367,   369,   371,   373,   375,   377,   379,   381,
     385,   387,   390,   393,   395,   399,   403,   407,   409,   413,
     417,   419,   423,   427,   429,   433,   435,   439,   441
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
     380,   381,   382,   383,   384,   385,   388,   389,   392,   393,
     394,   395,   396,   397,   398,   399,   400,   403,   404,   405,
     406,   409,   410,   413,   416,   419,   422,   425,   428,   431,
     434,   435,   438,   441,   444,   450,   451,   454,   455,   456,
     464,   465,   466,   470,   471,   472,   473,   477,   478,   479,
     483,   484,   485,   489,   490,   494,   495,   500,   501
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
     115,   116,   117
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 209;
  const int BisonParser::yynnts_ = 58;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 118;

  const unsigned int BisonParser::yyuser_token_number_max_ = 372;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 23 "../s/bif.y"
} // BIF
/* Line 1141 of lalr1.cc  */
#line 1678 "../bisonflex/bif.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 504 "../s/bif.y"

void BIF::BisonParser::error(const BIF::BisonParser::location_type &loc, const std::string &msg) {
        Parsing::Error(loc,msg);
}

static int yylex(BIF::BisonParser::semantic_type * yylval, BIF::BisonParser::location_type* loc, BIF::FlexScanner &scanner) {
        return scanner.yylex(yylval,loc);
}
