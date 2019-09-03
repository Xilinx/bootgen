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
#line 48 "../../s/cmdoptions.y"

#include <stdio.h>
#include "cmdoptionsscanner.h"
#include "version.h"
void ShowHelp();
void ShowCmdHelp(int);
void ShowBifHelp(int);
void ShowCommonHelp(int,bool);

/* Line 279 of lalr1.cc  */
#line 48 "cmdoptions.tab.cpp"


#include "cmdoptions.tab.hpp"

/* User implementation prologue.  */

/* Line 285 of lalr1.cc  */
#line 56 "cmdoptions.tab.cpp"
/* Unqualified %code blocks.  */
/* Line 286 of lalr1.cc  */
#line 44 "../../s/cmdoptions.y"

    static int yylex(CO::BisonParser::semantic_type * yylval, CO::BisonParser::location_type* loc, CO::FlexScanner &scanner);


/* Line 286 of lalr1.cc  */
#line 65 "cmdoptions.tab.cpp"


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
#line 23 "../../s/cmdoptions.y"
namespace CO {
/* Line 353 of lalr1.cc  */
#line 161 "cmdoptions.tab.cpp"

  /// Build a parser object.
  BisonParser::BisonParser (CO::FlexScanner& scanner_yyarg, Options& options_yyarg)
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
      BIF::slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }

    // Perform the reduction.
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
          case 5:
/* Line 670 of lalr1.cc  */
#line 105 "../../s/cmdoptions.y"
    { options.SetBifFilename((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 8:
/* Line 670 of lalr1.cc  */
#line 108 "../../s/cmdoptions.y"
    { options.GetOutputFileNames().push_back((yysemantic_stack_[(3) - (3)].cstring)); }
    break;

  case 9:
/* Line 670 of lalr1.cc  */
#line 109 "../../s/cmdoptions.y"
    { options.GetOutputFileNames().push_back((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 10:
/* Line 670 of lalr1.cc  */
#line 110 "../../s/cmdoptions.y"
    { options.SetDevicePartName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 116 "../../s/cmdoptions.y"
    { options.SetEfuseHashFileName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 17:
/* Line 670 of lalr1.cc  */
#line 117 "../../s/cmdoptions.y"
    { options.SetGenerateHashes(true); }
    break;

  case 18:
/* Line 670 of lalr1.cc  */
#line 118 "../../s/cmdoptions.y"
    { options.SetNonBootingFlag(true); }
    break;

  case 19:
/* Line 670 of lalr1.cc  */
#line 119 "../../s/cmdoptions.y"
    { options.SetLegacyFlag(true); }
    break;

  case 20:
/* Line 670 of lalr1.cc  */
#line 120 "../../s/cmdoptions.y"
    { options.SetPadHeaderTable(true); }
    break;

  case 21:
/* Line 670 of lalr1.cc  */
#line 121 "../../s/cmdoptions.y"
    { options.SetPadHeaderTable((bool)((yysemantic_stack_[(3) - (3)].number)!=0)); }
    break;

  case 24:
/* Line 670 of lalr1.cc  */
#line 124 "../../s/cmdoptions.y"
    { options.SetSpkSigFileName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 25:
/* Line 670 of lalr1.cc  */
#line 125 "../../s/cmdoptions.y"
    { options.SetDevicePackageName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 127 "../../s/cmdoptions.y"
    { options.SetArchType(Arch::ZYNQMP); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 128 "../../s/cmdoptions.y"
    { options.SetNoAuthBlocksFlag(true); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 130 "../../s/cmdoptions.y"
    { LOG_ERROR("'-debug' option is no more supported. Please use '-log' option"); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 131 "../../s/cmdoptions.y"
    { options.SetZynqMpEncrDump(true); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 132 "../../s/cmdoptions.y"
    { options.SetZynqmpes1Flag(true); }
    break;

  case 36:
/* Line 670 of lalr1.cc  */
#line 136 "../../s/cmdoptions.y"
    { options.SetKDFTestVectorFile((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 141 "../../s/cmdoptions.y"
    { options.SetSplitType(File::MCS); }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 142 "../../s/cmdoptions.y"
    { options.SetSplitType(File::BIN); }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 145 "../../s/cmdoptions.y"
    { options.SetDoFill(true); }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 146 "../../s/cmdoptions.y"
    {
                                  options.SetDoFill(true);  
                                  if ((yysemantic_stack_[(2) - (2)].number) >= 0 && (yysemantic_stack_[(2) - (2)].number) <=255)
                                    options.SetOutputFillByte((uint8_t)(yysemantic_stack_[(2) - (2)].number)); 
                                  else 
                                    LOG_ERROR("'-fill' - Fill byte must be 8 bits");
                                }
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 155 "../../s/cmdoptions.y"
    { ShowHelp(); exit(0); }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 156 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HBIFHELP); exit(0); }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 157 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HARCH); exit(0); }
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 158 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HIMAGE); exit(0); }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 159 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HFILL); exit(0); }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 160 "../../s/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPLIT,true); exit(0); }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 161 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HO); exit(0); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 162 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HP); exit(0); }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 163 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HW); exit(0); }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 164 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HEFUSEPPKBITS); exit(0); }
    break;

  case 55:
/* Line 670 of lalr1.cc  */
#line 165 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HGENHASHES); exit(0); }
    break;

  case 56:
/* Line 670 of lalr1.cc  */
#line 166 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HLEGACY); exit(0); }
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 167 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HPADHDR); exit(0); }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 168 "../../s/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPKSIGN,true); exit(0); }
    break;

  case 59:
/* Line 670 of lalr1.cc  */
#line 169 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HDEBUG); exit(0); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 170 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HPACKAGE); exit(0); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 171 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HENCRYPT); exit(0); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 172 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HGENKEYS); exit(0); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 173 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HDQSPI); exit(0); }
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 174 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HLOG); exit(0); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 175 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HZYNQMPES1); exit(0); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 176 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HPROCESSBIT); exit(0); }
    break;

  case 67:
/* Line 670 of lalr1.cc  */
#line 177 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HNONBOOTING); exit(0); }
    break;

  case 68:
/* Line 670 of lalr1.cc  */
#line 178 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HENCRDUMP); exit(0); }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 179 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HVERIFY); exit(0); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 182 "../../s/cmdoptions.y"
    { ShowBifHelp(0); exit(0); }
    break;

  case 71:
/* Line 670 of lalr1.cc  */
#line 183 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_INIT); exit(0); }
    break;

  case 72:
/* Line 670 of lalr1.cc  */
#line 184 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_UDFBH); exit(0); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 185 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AES); exit(0); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 186 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PPK); exit(0); }
    break;

  case 75:
/* Line 670 of lalr1.cc  */
#line 187 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PSK); exit(0); }
    break;

  case 76:
/* Line 670 of lalr1.cc  */
#line 188 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_SPK); exit(0); }
    break;

  case 77:
/* Line 670 of lalr1.cc  */
#line 189 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_SSK); exit(0); }
    break;

  case 78:
/* Line 670 of lalr1.cc  */
#line 190 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_SPKSIGN); exit(0); }
    break;

  case 79:
/* Line 670 of lalr1.cc  */
#line 191 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_HDRSIGN); exit(0); }
    break;

  case 80:
/* Line 670 of lalr1.cc  */
#line 192 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BI); exit(0); }
    break;

  case 81:
/* Line 670 of lalr1.cc  */
#line 193 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BL); exit(0); }
    break;

  case 82:
/* Line 670 of lalr1.cc  */
#line 194 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_ENCR); exit(0); }
    break;

  case 83:
/* Line 670 of lalr1.cc  */
#line 195 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PID); exit(0); }
    break;

  case 84:
/* Line 670 of lalr1.cc  */
#line 196 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AUTH); exit(0); }
    break;

  case 85:
/* Line 670 of lalr1.cc  */
#line 197 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_CHKSM); exit(0); }
    break;

  case 86:
/* Line 670 of lalr1.cc  */
#line 198 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_POWNER); exit(0); }
    break;

  case 87:
/* Line 670 of lalr1.cc  */
#line 199 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PRESIGN); exit(0); }
    break;

  case 88:
/* Line 670 of lalr1.cc  */
#line 200 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_UDF); exit(0); }
    break;

  case 89:
/* Line 670 of lalr1.cc  */
#line 201 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_XIP); exit(0); }
    break;

  case 90:
/* Line 670 of lalr1.cc  */
#line 202 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_ALIGN); exit(0); }
    break;

  case 91:
/* Line 670 of lalr1.cc  */
#line 203 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_OFFSET); exit(0); }
    break;

  case 92:
/* Line 670 of lalr1.cc  */
#line 204 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_RES); exit(0); }
    break;

  case 93:
/* Line 670 of lalr1.cc  */
#line 205 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_LOAD); exit(0); }
    break;

  case 94:
/* Line 670 of lalr1.cc  */
#line 206 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_STARTUP); exit(0); }
    break;

  case 95:
/* Line 670 of lalr1.cc  */
#line 207 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_KEYSRC); exit(0); }
    break;

  case 96:
/* Line 670 of lalr1.cc  */
#line 208 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_FSBLCFG); exit(0); }
    break;

  case 97:
/* Line 670 of lalr1.cc  */
#line 209 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTDEV); exit(0); }
    break;

  case 98:
/* Line 670 of lalr1.cc  */
#line 210 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_DESTCPU); exit(0); }
    break;

  case 99:
/* Line 670 of lalr1.cc  */
#line 211 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_DESTDEV); exit(0); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 212 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_EL); exit(0); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 213 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_TZ); exit(0); }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 214 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AUTHPARAM); exit(0); }
    break;

  case 103:
/* Line 670 of lalr1.cc  */
#line 215 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BHKEY); exit(0); }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 216 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PFW); exit(0); }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 217 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BLOCKS); exit(0); }
    break;

  case 106:
/* Line 670 of lalr1.cc  */
#line 218 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BHIV); exit(0); }
    break;

  case 107:
/* Line 670 of lalr1.cc  */
#line 219 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_METAL); exit(0); }
    break;

  case 108:
/* Line 670 of lalr1.cc  */
#line 220 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_ELYHNDOFF); exit(0); }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 221 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_HIVEC); exit(0); }
    break;

  case 110:
/* Line 670 of lalr1.cc  */
#line 222 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AUTHBLOCKS); exit(0); }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 223 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BHSIGN); exit(0); }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 224 "../../s/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPLIT,false); exit(0); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 225 "../../s/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPKSIGN,false); exit(0); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 226 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTVEC); exit(0); }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 227 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PUFDATA); exit(0); }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 230 "../../s/cmdoptions.y"
    {options.SetOverwrite(true);}
    break;

  case 117:
/* Line 670 of lalr1.cc  */
#line 231 "../../s/cmdoptions.y"
    {options.SetOverwrite(true);}
    break;

  case 118:
/* Line 670 of lalr1.cc  */
#line 232 "../../s/cmdoptions.y"
    {options.SetOverwrite(false);}
    break;

  case 120:
/* Line 670 of lalr1.cc  */
#line 236 "../../s/cmdoptions.y"
    {options.SetLogLevel(LogLevel::ERROR);}
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 237 "../../s/cmdoptions.y"
    {options.SetLogLevel(LogLevel::WARNING);}
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 238 "../../s/cmdoptions.y"
    {options.SetLogLevel(LogLevel::INFO);}
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 239 "../../s/cmdoptions.y"
    {options.SetLogLevel(LogLevel::DEBUG);}
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 240 "../../s/cmdoptions.y"
    {options.SetLogLevel(LogLevel::TRACE);}
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 243 "../../s/cmdoptions.y"
    {options.SetEncryptedKeySource(KeySource::BbramRedKey);}
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 244 "../../s/cmdoptions.y"
    {options.SetEncryptedKeySource(KeySource::EfuseRedKey);}
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 247 "../../s/cmdoptions.y"
    {options.SetStartCbc((yysemantic_stack_[(3) - (3)].cstring));}
    break;

  case 130:
/* Line 670 of lalr1.cc  */
#line 248 "../../s/cmdoptions.y"
    {options.SetKey0((yysemantic_stack_[(3) - (3)].cstring));}
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 249 "../../s/cmdoptions.y"
    {options.SetHmac((yysemantic_stack_[(3) - (3)].cstring));}
    break;

  case 134:
/* Line 670 of lalr1.cc  */
#line 250 "../../s/cmdoptions.y"
    {options.SetEncryptionKeyFile((yysemantic_stack_[(1) - (1)].cstring));}
    break;

  case 137:
/* Line 670 of lalr1.cc  */
#line 254 "../../s/cmdoptions.y"
    {options.SetProcessBitstreamType(File::MCS); }
    break;

  case 138:
/* Line 670 of lalr1.cc  */
#line 255 "../../s/cmdoptions.y"
    {options.SetProcessBitstreamType(File::BIN); }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 258 "../../s/cmdoptions.y"
    {options.SetDualQspiMode(QspiMode::PARALLEL_LQSPI); }
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 259 "../../s/cmdoptions.y"
    {options.SetDualQspiMode(QspiMode::STACKED); }
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 263 "../../s/cmdoptions.y"
    { 
                      if ((yysemantic_stack_[(1) - (1)].number)==16 || (yysemantic_stack_[(1) - (1)].number)==32 || (yysemantic_stack_[(1) - (1)].number)==64 || (yysemantic_stack_[(1) - (1)].number)==128) 
                          options.SetQspiSize((uint16_t)(yysemantic_stack_[(1) - (1)].number)); 
                      else 
                          LOG_ERROR("Supported QSPI sizes (in MB) for -dual_qspi_mode option are 16, 32, 64 & 128"); 
                  }
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 271 "../../s/cmdoptions.y"
    {options.SetArchType(Arch::ZYNQ); }
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 272 "../../s/cmdoptions.y"
    {options.SetArchType(Arch::ZYNQMP); }
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 273 "../../s/cmdoptions.y"
    {options.SetArchType(Arch::FPGA); }
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 277 "../../s/cmdoptions.y"
    {options.SetGreyKeyGeneration(true); }
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 278 "../../s/cmdoptions.y"
    {options.SetMetalKeyGeneration(true); }
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 279 "../../s/cmdoptions.y"
    {options.SetAuthKeyGeneration(GenAuthKeys::PEM); }
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 280 "../../s/cmdoptions.y"
    {options.SetAuthKeyGeneration(GenAuthKeys::RSA); }
    break;

  case 150:
/* Line 670 of lalr1.cc  */
#line 283 "../../s/cmdoptions.y"
    {options.SetAuthKeyGeneration(GenAuthKeys::PEM); }
    break;

  case 151:
/* Line 670 of lalr1.cc  */
#line 284 "../../s/cmdoptions.y"
    {options.SetAuthKeyGeneration(GenAuthKeys::RSA); }
    break;

  case 152:
/* Line 670 of lalr1.cc  */
#line 287 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(1) - (1)].cstring));
                                  options.SetVerifyImageOption(true);}
    break;

  case 153:
/* Line 670 of lalr1.cc  */
#line 290 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(1) - (1)].cstring));
                                              options.SetReadImageOption(ReadImageOption::ALL); }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 292 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring)); }
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 295 "../../s/cmdoptions.y"
    {options.SetReadImageOption(ReadImageOption::BH);}
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 296 "../../s/cmdoptions.y"
    {options.SetReadImageOption(ReadImageOption::IHT);}
    break;

  case 157:
/* Line 670 of lalr1.cc  */
#line 297 "../../s/cmdoptions.y"
    {options.SetReadImageOption(ReadImageOption::IH);}
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 298 "../../s/cmdoptions.y"
    {options.SetReadImageOption(ReadImageOption::PHT);}
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 299 "../../s/cmdoptions.y"
    {options.SetReadImageOption(ReadImageOption::AC);}
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 302 "../../s/cmdoptions.y"
    { options.SetDumpOption(DumpOption::BH); }
    break;

  case 161:
/* Line 670 of lalr1.cc  */
#line 303 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring));
                                      options.SetDumpOption(DumpOption::BH); }
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 305 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (2)].cstring));
                                      options.SetDumpOption(DumpOption::BH); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 307 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(1) - (1)].cstring));
                                      options.SetDumpOption(DumpOption::PARTITIONS); }
    break;


/* Line 670 of lalr1.cc  */
#line 1249 "cmdoptions.tab.cpp"
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
  const signed char BisonParser::yypact_ninf_ = -51;
  const signed char
  BisonParser::yypact_[] =
  {
       -51,    23,    90,   -51,   -30,   -39,    -5,    -9,   -51,   -51,
     -51,   -13,    86,   -11,    15,    18,   -50,   -17,   -51,    11,
      10,   -51,   -51,   -12,   -51,   -16,   -10,   -26,    -3,    -8,
     -30,   -30,   -30,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -30,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
     -30,     0,   -51,   -51,   -51,   -51,   -51,   -51,   -39,   -51,
     -51,   -51,   -51,   -51,    -4,   -51,   -51,   -51,     5,   -51,
     -51,   -51,   -51,   -51,    16,   -51,   -25,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,   -51,
      19,   -14,   -51,   -51,   -51,   -51,   -51,   -51,    21,    22,
      20,   -51,    24,    25,   -30,   -51,    27,   -51,   -51,   -51
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,    43,     0,    45,    30,    19,
      18,     0,    70,   119,     0,     0,     0,     0,    27,     0,
     116,    28,    32,     0,    17,    20,     0,     0,     0,     0,
       0,     0,     0,    31,     4,     7,    38,    39,    40,    37,
       5,   136,   135,    44,     0,     9,    46,    47,    48,    49,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    50,
      13,    25,   113,    71,    72,    73,    74,    75,    76,    77,
      78,   109,    79,    80,    81,    83,    82,    84,    85,   108,
     111,    86,    87,    88,    89,    90,    91,    92,    93,   101,
      94,    95,    96,    97,    98,    99,   100,   112,   102,   103,
     104,   105,   107,   106,   110,   114,   115,    14,   120,   121,
     122,   123,   124,    12,    41,    42,     6,   137,   138,    22,
     160,   163,    35,   142,   143,   144,    26,   139,     0,    23,
     117,   118,    11,    16,     0,    24,   148,   149,     0,   146,
     147,    29,   125,   126,   127,    10,   153,    33,   152,    34,
      36,     8,   162,   161,   141,   140,    21,   150,   151,   145,
       0,   129,   155,   156,   157,   158,   159,   154,     0,     0,
     131,   128,     0,     0,   133,   130,     0,   134,    15,   132
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  BisonParser::yypgoto_[] =
  {
       -51,   -51,   -51,   -51,    -6,   -51,   -51,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -48,   -51,   -51,   -51,
     -51,   -51,   -51,   -51,   -51,   -51,   -51
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,    34,    40,   126,    35,    70,   117,   142,
     123,   154,   171,   180,   184,   188,    43,   129,   139,   165,
     136,   151,   169,   159,   157,   177,   132
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned char
  BisonParser::yytable_[] =
  {
        45,    44,   130,   118,   119,   120,   121,   122,   133,   134,
     131,   135,    36,    37,    38,    39,   146,   147,   148,   149,
     150,    41,    42,     3,   156,   158,   160,   172,   173,   174,
     175,   176,    36,    37,    38,    39,   124,   125,   161,   127,
     128,   137,   138,   140,   141,   152,   153,   167,   168,    71,
     144,   143,   163,   145,   155,   179,   166,    36,    37,    38,
      39,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,   170,   178,   181,   183,   182,   185,
     164,   186,   189,     4,     5,     6,     0,     7,     8,     9,
      10,    11,    12,    13,     0,     0,     0,     0,     0,    14,
      15,     0,     0,    16,    17,     0,     0,    18,    69,    19,
       0,     0,    20,     0,   162,    21,    22,    23,    24,    25,
      26,    27,     0,     0,     0,     0,     0,    28,     0,     0,
      29,    30,     0,     0,     0,     0,     0,    31,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    72,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   187,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
         6,     6,    52,    14,    15,    16,    17,    18,    25,    26,
      16,    28,    62,    63,    64,    65,    42,    43,    44,    45,
      46,    60,    61,     0,    30,    31,    32,    52,    53,    54,
      55,    56,    62,    63,    64,    65,    21,    22,    44,    21,
      22,    30,    31,    33,    34,    48,    49,    42,    43,    62,
      66,    63,    52,    63,    62,    69,    60,    62,    63,    64,
      65,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    68,    66,    65,    67,    66,    65,
     138,    66,    65,     3,     4,     5,    -1,     7,     8,     9,
      10,    11,    12,    13,    -1,    -1,    -1,    -1,    -1,    19,
      20,    -1,    -1,    23,    24,    -1,    -1,    27,   127,    29,
      -1,    -1,    32,    -1,   130,    35,    36,    37,    38,    39,
      40,    41,    -1,    -1,    -1,    -1,    -1,    47,    -1,    -1,
      50,    51,    -1,    -1,    -1,    -1,    -1,    57,    58,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    81,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   184,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   138,   139,     0,     3,     4,     5,     7,     8,     9,
      10,    11,    12,    13,    19,    20,    23,    24,    27,    29,
      32,    35,    36,    37,    38,    39,    40,    41,    47,    50,
      51,    57,    58,    59,   140,   143,    62,    63,    64,    65,
     141,    60,    61,   153,     6,   141,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,   127,
     144,    62,    81,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   145,    14,    15,
      16,    17,    18,   147,    21,    22,   142,    21,    22,   154,
      52,   141,   163,    25,    26,    28,   157,    30,    31,   155,
      33,    34,   146,    63,    66,    63,    42,    43,    44,    45,
      46,   158,    48,    49,   148,    62,   141,   161,   141,   160,
     141,   141,   141,    52,   153,   156,    60,    42,    43,   159,
      68,   149,    52,    53,    54,    55,    56,   162,    66,    69,
     150,    65,    66,    67,   151,    65,    66,   141,   152,    65
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
     385,   386,   387,   388,   389,   390,   391
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   137,   138,   139,   139,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   140,   140,   140,
     140,   140,   140,   140,   140,   140,   140,   141,   141,   141,
     141,   142,   142,   143,   143,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     144,   144,   144,   144,   144,   144,   144,   144,   144,   144,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   146,   146,   146,   147,
     147,   147,   147,   147,   147,   148,   148,   149,   149,   150,
     150,   151,   151,   152,   152,   153,   153,   154,   154,   155,
     155,   156,   157,   157,   157,   158,   158,   158,   158,   158,
     159,   159,   160,   161,   161,   162,   162,   162,   162,   162,
     163,   163,   163,   163
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     2,     2,     1,     3,     2,
       2,     2,     2,     2,     2,     6,     2,     1,     1,     1,
       1,     3,     2,     2,     2,     2,     2,     1,     1,     2,
       1,     1,     1,     2,     2,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     2,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     0,     1,     1,     0,
       1,     1,     1,     1,     1,     1,     1,     0,     3,     0,
       3,     0,     3,     0,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     2,     2,     1
  };

#if YYDEBUG
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const BisonParser::yytname_[] =
  {
    "$end", "error", "$undefined", "_IMAGE", "_FILL", "_O_TOK", "I", "_H",
  "_DEBUG_TOK", "_LEGACY", "_NONBOOTING", "_PACKAGENAME", "_BIF_HELP",
  "_LOG", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE", "_SPLIT",
  "_PROCESS_BITSTREAM", "MCS", "BIN", "_DUMP", "_ARCH", "ZYNQ", "ZYNQMP",
  "_R", "FPGA", "_DUAL_QSPI_MODE", "PARALLEL", "STACKED", "_W", "ON",
  "OFF", "_NOAUTHBLOCKS", "_ZYNQMPES1", "_EFUSEPPKBITS",
  "_GENERATE_HASHES", "_PADIMAGEHEADER", "_SPKSIGNATURE", "_GENERATE_KEYS",
  "PEM", "RSA", "AUTH", "GREY", "METAL", "_ENCRYPT", "BBRAM", "EFUSE",
  "_P_TOK", "_READ", "READ_BH", "READ_IHT", "READ_IH", "READ_PHT",
  "READ_AC", "_VERIFY", "_VERIFYKDF", "_ZYNQMPENCRDUMP", "DECVALUE",
  "HEXVALUE", "IDENTIFIER", "FILENAME", "QFILENAME", "HEXSTRING", "EQUALS",
  "HMAC", "STARTCBC", "KEY0", "HBIFHELP", "HARCH", "HIMAGE", "HFILL", "HO",
  "HP", "HW", "HEFUSEPPKBITS", "HGENHASHES", "HLEGACY", "HPADHDR",
  "H_SPKSIGN", "HDEBUG", "HPACKAGE", "HENCRYPT", "HGENKEYS", "HDQSPI",
  "HLOG", "HZYNQMPES1", "HPROCESSBIT", "HNONBOOTING", "HENCRDUMP",
  "HVERIFY", "H_BIF_INIT", "H_BIF_UDFBH", "H_BIF_AES", "H_BIF_PPK",
  "H_BIF_PSK", "H_BIF_SPK", "H_BIF_SSK", "H_BIF_SPKSIGN", "H_BIF_HIVEC",
  "H_BIF_HDRSIGN", "H_BIF_BI", "H_BIF_BL", "H_BIF_PID", "H_BIF_ENCR",
  "H_BIF_AUTH", "H_BIF_CHKSM", "H_BIF_ELYHNDOFF", "H_BIF_BHSIGN",
  "H_BIF_POWNER", "H_BIF_PRESIGN", "H_BIF_UDF", "H_BIF_XIP", "H_BIF_ALIGN",
  "H_BIF_OFFSET", "H_BIF_RES", "H_BIF_LOAD", "H_BIF_TZ", "H_BIF_STARTUP",
  "H_BIF_KEYSRC", "H_BIF_FSBLCFG", "H_BIF_BOOTDEV", "H_BIF_DESTCPU",
  "H_BIF_DESTDEV", "H_BIF_EL", "H_SPLIT", "H_BIF_AUTHPARAM", "H_BIF_BHKEY",
  "H_BIF_PFW", "H_BIF_BLOCKS", "H_BIF_METAL", "H_BIF_BHIV",
  "H_BIF_AUTHBLOCKS", "H_BIF_BOOTVEC", "H_BIF_PUFDATA", "$accept", "top",
  "option_list", "option", "filename", "memsplit", "filloption",
  "helpoption", "bifhelpoption", "wopt", "loglevel", "keysource",
  "startcbc", "key0", "hmac", "keyfile", "number", "bitfile_type",
  "qpsi_mode", "qspi_size", "archOptions", "key_type", "auth_key_options",
  "verifyImageOptions", "readImageOptions", "readType", "dumpOptions", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BisonParser::rhs_number_type
  BisonParser::yyrhs_[] =
  {
       138,     0,    -1,   139,    -1,    -1,   139,   140,    -1,     3,
     141,    -1,    19,   142,    -1,   143,    -1,     5,     6,   141,
      -1,     5,   141,    -1,    50,    62,    -1,    32,   146,    -1,
      13,   147,    -1,     7,   144,    -1,    12,   145,    -1,    47,
     148,   149,   150,   151,   152,    -1,    37,    63,    -1,    38,
      -1,    10,    -1,     9,    -1,    39,    -1,    39,    66,    60,
      -1,    20,   154,    -1,    29,   155,    -1,    40,    63,    -1,
      11,    62,    -1,    24,   157,    -1,    27,    -1,    35,    -1,
      41,   158,    -1,     8,    -1,    59,    -1,    36,    -1,    51,
     161,    -1,    57,   160,    -1,    23,   163,    -1,    58,   141,
      -1,    65,    -1,    62,    -1,    63,    -1,    64,    -1,    21,
      -1,    22,    -1,     4,    -1,     4,   153,    -1,    -1,    70,
      -1,    71,    -1,    72,    -1,    73,    -1,   127,    -1,    74,
      -1,    75,    -1,    76,    -1,    77,    -1,    78,    -1,    79,
      -1,    80,    -1,    81,    -1,    82,    -1,    83,    -1,    84,
      -1,    85,    -1,    86,    -1,    87,    -1,    88,    -1,    89,
      -1,    90,    -1,    91,    -1,    92,    -1,    -1,    93,    -1,
      94,    -1,    95,    -1,    96,    -1,    97,    -1,    98,    -1,
      99,    -1,   100,    -1,   102,    -1,   103,    -1,   104,    -1,
     106,    -1,   105,    -1,   107,    -1,   108,    -1,   111,    -1,
     112,    -1,   113,    -1,   114,    -1,   115,    -1,   116,    -1,
     117,    -1,   118,    -1,   120,    -1,   121,    -1,   122,    -1,
     123,    -1,   124,    -1,   125,    -1,   126,    -1,   119,    -1,
     128,    -1,   129,    -1,   130,    -1,   131,    -1,   133,    -1,
     132,    -1,   109,    -1,   101,    -1,   134,    -1,   110,    -1,
     127,    -1,    81,    -1,   135,    -1,   136,    -1,    -1,    33,
      -1,    34,    -1,    -1,    14,    -1,    15,    -1,    16,    -1,
      17,    -1,    18,    -1,    48,    -1,    49,    -1,    -1,    68,
      66,    65,    -1,    -1,    69,    66,    65,    -1,    -1,    67,
      66,    65,    -1,    -1,   141,    -1,    61,    -1,    60,    -1,
      21,    -1,    22,    -1,    30,    -1,    31,   156,    -1,   153,
      -1,    25,    -1,    26,    -1,    28,    -1,    44,   159,    -1,
      45,    -1,    46,    -1,    42,    -1,    43,    -1,    42,    -1,
      43,    -1,   141,    -1,   141,    -1,   141,   162,    -1,    52,
      -1,    53,    -1,    54,    -1,    55,    -1,    56,    -1,    52,
      -1,   141,    52,    -1,    52,   141,    -1,   141,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     5,     6,     9,    12,    15,    17,    21,
      24,    27,    30,    33,    36,    39,    46,    49,    51,    53,
      55,    57,    61,    64,    67,    70,    73,    76,    78,    80,
      83,    85,    87,    89,    92,    95,    98,   101,   103,   105,
     107,   109,   111,   113,   115,   118,   119,   121,   123,   125,
     127,   129,   131,   133,   135,   137,   139,   141,   143,   145,
     147,   149,   151,   153,   155,   157,   159,   161,   163,   165,
     167,   168,   170,   172,   174,   176,   178,   180,   182,   184,
     186,   188,   190,   192,   194,   196,   198,   200,   202,   204,
     206,   208,   210,   212,   214,   216,   218,   220,   222,   224,
     226,   228,   230,   232,   234,   236,   238,   240,   242,   244,
     246,   248,   250,   252,   254,   256,   258,   259,   261,   263,
     264,   266,   268,   270,   272,   274,   276,   278,   279,   283,
     284,   288,   289,   293,   294,   296,   298,   300,   302,   304,
     306,   309,   311,   313,   315,   317,   320,   322,   324,   326,
     328,   330,   332,   334,   336,   339,   341,   343,   345,   347,
     349,   351,   354,   357
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,    99,    99,   101,   102,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   132,   133,   134,   135,   136,   139,   139,   139,
     139,   141,   142,   145,   146,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   230,   231,   232,   235,
     236,   237,   238,   239,   240,   243,   244,   247,   247,   248,
     248,   249,   249,   250,   250,   252,   252,   254,   255,   258,
     259,   262,   271,   272,   273,   276,   277,   278,   279,   280,
     283,   284,   287,   290,   292,   295,   296,   297,   298,   299,
     302,   303,   305,   307
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
     135,   136
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 222;
  const int BisonParser::yynnts_ = 27;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 137;

  const unsigned int BisonParser::yyuser_token_number_max_ = 391;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 23 "../../s/cmdoptions.y"
} // CO
/* Line 1141 of lalr1.cc  */
#line 1915 "cmdoptions.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 311 "../../s/cmdoptions.y"



//    int                outputByteWidth;
//    Binary::Address_t  baseAddress;
//    uint32_t           defaultAlignment;

void ShowHelp() 
{
   std::cout << HELP << std::endl;
}

void ShowCommonHelp(int a, bool cmdhelp)
{
    switch(a)
    {
        case CO::BisonParser::token::H_SPLIT:
            if(cmdhelp)
                std::cout << SPLITHELP << std::endl;
            else
                std::cout << H_BIF_SPLIT_H << std::endl;
            break;

        case CO::BisonParser::token::H_SPKSIGN:
            if(cmdhelp)
                std::cout << SPKSIGNHELP << std::endl;
            else
                std::cout << H_BIF_SPKSIGN_H << std::endl;
             break;

        case 0: 
             std::cout << HELP << std::endl;
             break;
                     
        default: LOG_ERROR("Unsupported Option"); 
             break;
    }
}


void ShowCmdHelp(int a)
{
    switch (a)
    {
    case CO::BisonParser::token::HARCH:
        std::cout << ARCHHELP << std::endl;
        break;

    case CO::BisonParser::token::HIMAGE:
        std::cout << IMAGEHELP << std::endl;
        break;

    case CO::BisonParser::token::HFILL:
        std::cout << FILLHELP << std::endl;
        break;

    case CO::BisonParser::token::HO:
        std::cout << OHELP << std::endl;
        break;

    case CO::BisonParser::token::HP:
        std::cout << PHELP << std::endl;
        break;

    case CO::BisonParser::token::HW:
        std::cout << WHELP << std::endl;
        break;

    case CO::BisonParser::token::HEFUSEPPKBITS:
        std::cout << EFUSEPPKBITSHELP << std::endl;
        break;

    case CO::BisonParser::token::HZYNQMPES1:
        std::cout << ZYNQMPES1HELP << std::endl;
        break;

    case CO::BisonParser::token::HGENHASHES:
        std::cout << GENERATEHASHESHELP << std::endl;
        break;

    case CO::BisonParser::token::HPADHDR:
        std::cout << PADHDRHELP << std::endl;
        break;

    case CO::BisonParser::token::HDEBUG:
        std::cout << DEBUGHELP << std::endl;
        break;

    case CO::BisonParser::token::HENCRYPT:
        std::cout << ENCRYPTHELP << std::endl;
        break;

    case CO::BisonParser::token::HGENKEYS:
        std::cout << GENKEYSHELP << std::endl;
        break;

    case CO::BisonParser::token::HDQSPI:
        std::cout << DQSPIHELP << std::endl;
        break;

    case CO::BisonParser::token::HLOG:
        std::cout << LOGHELP << std::endl;
        break;

    case CO::BisonParser::token::HBIFHELP:
        std::cout << BIFHELP << std::endl;
        break;

    case CO::BisonParser::token::HPROCESSBIT:
        std::cout << PROCESSBITHELP << std::endl;
        break;

    case CO::BisonParser::token::HNONBOOTING:
        std::cout << NONBOOTINGHELP << std::endl;
        break;

    case CO::BisonParser::token::HENCRDUMP:
        std::cout << ENCRDUMPHELP << std::endl;
        break;

    case CO::BisonParser::token::HVERIFY: 
        std::cout << VERIFYHELP << std::endl;
        break;

    case 0:
        std::cout << HELP << std::endl;
        break;

    default: LOG_ERROR("Unsupported Option");
        break;
    }
}

void ShowBifHelp(int a)
{
    switch (a)
    {
    case CO::BisonParser::token::H_BIF_INIT:
        std::cout << H_BIF_INIT_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_UDFBH:
        std::cout << H_BIF_UDFBH_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_AES:
        std::cout << H_BIF_AES_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PPK:
    case CO::BisonParser::token::H_BIF_PSK:
    case CO::BisonParser::token::H_BIF_SPK:
    case CO::BisonParser::token::H_BIF_SSK:
        std::cout << H_BIF_PPK_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_SPKSIGN:
        std::cout << H_BIF_SPKSIGN_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_HDRSIGN:
        std::cout << H_BIF_HDRSIGN_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BI:
        std::cout << H_BIF_BI_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BL:
        std::cout << H_BIF_BL_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_ENCR:
        std::cout << H_BIF_ENCR_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PID:
        std::cout << H_BIF_PID_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_AUTH:
        std::cout << H_BIF_AUTH_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_CHKSM:
        std::cout << H_BIF_CHKSM_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_POWNER:
        std::cout << H_BIF_POWNER_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PRESIGN:
        std::cout << H_BIF_PRESIGN_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_UDF:
        std::cout << H_BIF_UDF_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_XIP:
        std::cout << H_BIF_XIP_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_ALIGN:
        std::cout << H_BIF_ALIGN_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_OFFSET:
        std::cout << H_BIF_OFFSET_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_RES:
        std::cout << H_BIF_RES_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_LOAD:
        std::cout << H_BIF_LOAD_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_STARTUP:
        std::cout << H_BIF_STARTUP_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_KEYSRC:
        std::cout << H_BIF_KEYSRC_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_FSBLCFG:
        std::cout << H_BIF_FSBLCFG_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BOOTDEV:
        std::cout << H_BIF_BOOTDEV_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_DESTCPU:
        std::cout << H_BIF_DESTCPU_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_DESTDEV:
        std::cout << H_BIF_DESTDEV_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_EL:
        std::cout << H_BIF_EL_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_TZ:
        std::cout << H_BIF_TZ_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_AUTHPARAM:
        std::cout << H_BIF_AUTHPARAM_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BHKEY:
        std::cout << H_BIF_BHKEY_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PFW:
        std::cout << H_BIF_PFW_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BLOCKS:
        std::cout << H_BIF_BLOCKS_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BHIV:
        std::cout << H_BIF_BHIV_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_METAL:
        std::cout << H_BIF_METAL_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_ELYHNDOFF:
        std::cout << H_BIF_ELYHNDOFF_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_HIVEC:
        std::cout << H_BIF_HIVEC_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_AUTHBLOCKS:
        std::cout << H_BIF_AUTHBLOCKS_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BHSIGN:
        std::cout << H_BIF_BHSIGN_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BOOTVEC:
        std::cout << H_BIF_BOOTVEC_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PUFDATA:
        std::cout << H_BIF_PUFDATA_H << std::endl;
        break;

    case 0:
        std::cout << BIF_HELP << std::endl;
        break;

    default: LOG_ERROR("Unsupported Option");
        break;
    }
}

void CO::BisonParser::error(const CO::BisonParser::location_type &loc, const std::string &msg)
{
    scanner.ReportError(loc, msg);
}

static int yylex(CO::BisonParser::semantic_type * yylval, CO::BisonParser::location_type* loc, CO::FlexScanner &scanner)
{
    return scanner.yylex(yylval, loc);
}
