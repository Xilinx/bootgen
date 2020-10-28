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
#include <string.h>
#include "cmdoptionsscanner.h"
#include "version.h"
void ShowHelp();
void ShowCmdHelp(int);
void ShowBifHelp(int);
void ShowCommonHelp(int,bool);

/* Line 279 of lalr1.cc  */
#line 49 "cmdoptions.tab.cpp"


#include "cmdoptions.tab.hpp"

/* User implementation prologue.  */

/* Line 285 of lalr1.cc  */
#line 57 "cmdoptions.tab.cpp"
/* Unqualified %code blocks.  */
/* Line 286 of lalr1.cc  */
#line 44 "../../s/cmdoptions.y"

    static int yylex(CO::BisonParser::semantic_type * yylval, CO::BisonParser::location_type* loc, CO::FlexScanner &scanner);


/* Line 286 of lalr1.cc  */
#line 66 "cmdoptions.tab.cpp"


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
#line 162 "cmdoptions.tab.cpp"

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
#line 110 "../../s/cmdoptions.y"
    { options.SetBifFilename((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 8:
/* Line 670 of lalr1.cc  */
#line 113 "../../s/cmdoptions.y"
    { options.GetOutputFileNames().push_back((yysemantic_stack_[(3) - (3)].cstring)); }
    break;

  case 9:
/* Line 670 of lalr1.cc  */
#line 114 "../../s/cmdoptions.y"
    { options.GetOutputFileNames().push_back((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 10:
/* Line 670 of lalr1.cc  */
#line 115 "../../s/cmdoptions.y"
    { options.SetDevicePartName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 121 "../../s/cmdoptions.y"
    { options.SetEfuseHashFileName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 17:
/* Line 670 of lalr1.cc  */
#line 122 "../../s/cmdoptions.y"
    { options.SetGenerateHashes(true); }
    break;

  case 18:
/* Line 670 of lalr1.cc  */
#line 123 "../../s/cmdoptions.y"
    { options.SetNonBootingFlag(true); }
    break;

  case 19:
/* Line 670 of lalr1.cc  */
#line 124 "../../s/cmdoptions.y"
    { options.SetLegacyFlag(true); }
    break;

  case 20:
/* Line 670 of lalr1.cc  */
#line 125 "../../s/cmdoptions.y"
    { options.SetPadHeaderTable(true); }
    break;

  case 21:
/* Line 670 of lalr1.cc  */
#line 126 "../../s/cmdoptions.y"
    { options.SetPadHeaderTable((bool)(strcmp((yysemantic_stack_[(3) - (3)].cstring),"0"))); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 127 "../../s/cmdoptions.y"
    { options.SetPadHeaderTable((bool)(strcmp((yysemantic_stack_[(2) - (2)].cstring),"0"))); }
    break;

  case 26:
/* Line 670 of lalr1.cc  */
#line 131 "../../s/cmdoptions.y"
    { options.SetSpkSigFileName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 132 "../../s/cmdoptions.y"
    { options.SetDevicePackageName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 29:
/* Line 670 of lalr1.cc  */
#line 134 "../../s/cmdoptions.y"
    { options.SetArchType(Arch::ZYNQMP); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 135 "../../s/cmdoptions.y"
    { options.SetNoAuthBlocksFlag(true); }
    break;

  case 32:
/* Line 670 of lalr1.cc  */
#line 137 "../../s/cmdoptions.y"
    { LOG_ERROR("'-debug' option is no more supported. Please use '-log' option"); }
    break;

  case 34:
/* Line 670 of lalr1.cc  */
#line 139 "../../s/cmdoptions.y"
    { options.SetZynqmpes1Flag(true); }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 140 "../../s/cmdoptions.y"
    { LOG_WARNING("The option '-securedebugimage'is deprecated. Use '-authenticatedjtag' instead."); }
    break;

  case 40:
/* Line 670 of lalr1.cc  */
#line 145 "../../s/cmdoptions.y"
    { options.SetDumpDirectory((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 146 "../../s/cmdoptions.y"
    { options.SetKDFTestVectorFile((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 46:
/* Line 670 of lalr1.cc  */
#line 153 "../../s/cmdoptions.y"
    { options.SetSplitType(File::MCS); }
    break;

  case 47:
/* Line 670 of lalr1.cc  */
#line 154 "../../s/cmdoptions.y"
    { options.SetSplitType(File::BIN); }
    break;

  case 48:
/* Line 670 of lalr1.cc  */
#line 157 "../../s/cmdoptions.y"
    { options.SetDoFill(true); }
    break;

  case 49:
/* Line 670 of lalr1.cc  */
#line 158 "../../s/cmdoptions.y"
    { options.SetDoFill(true);
                                                      if ((yysemantic_stack_[(2) - (2)].number) >= 0 && (yysemantic_stack_[(2) - (2)].number) <= 255)
                                                        options.SetOutputFillByte((uint8_t)(yysemantic_stack_[(2) - (2)].number)); 
                                                      else 
                                                        LOG_ERROR("'-fill' - Fill byte must be 8 bits"); }
    break;

  case 50:
/* Line 670 of lalr1.cc  */
#line 165 "../../s/cmdoptions.y"
    { ShowHelp(); exit(0); }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 166 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HBIFHELP); exit(0); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 167 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HARCH); exit(0); }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 168 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HIMAGE); exit(0); }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 169 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HFILL); exit(0); }
    break;

  case 55:
/* Line 670 of lalr1.cc  */
#line 170 "../../s/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPLIT,true); exit(0); }
    break;

  case 56:
/* Line 670 of lalr1.cc  */
#line 171 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HO); exit(0); }
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 172 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HP); exit(0); }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 173 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HW); exit(0); }
    break;

  case 59:
/* Line 670 of lalr1.cc  */
#line 174 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HEFUSEPPKBITS); exit(0); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 175 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HGENHASHES); exit(0); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 176 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HLEGACY); exit(0); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 177 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HPADHDR); exit(0); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 178 "../../s/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPKSIGN,true); exit(0); }
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 179 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HPACKAGE); exit(0); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 180 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HENCRYPT); exit(0); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 181 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HGENKEYS); exit(0); }
    break;

  case 67:
/* Line 670 of lalr1.cc  */
#line 182 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HDQSPI); exit(0); }
    break;

  case 68:
/* Line 670 of lalr1.cc  */
#line 183 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HLOG); exit(0); }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 184 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HZYNQMPES1); exit(0); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 185 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HPROCESSBIT); exit(0); }
    break;

  case 71:
/* Line 670 of lalr1.cc  */
#line 186 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HNONBOOTING); exit(0); }
    break;

  case 72:
/* Line 670 of lalr1.cc  */
#line 187 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HENCRDUMP); exit(0); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 188 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HVERIFY); exit(0); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 189 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HVERIFYKDF); exit(0); }
    break;

  case 75:
/* Line 670 of lalr1.cc  */
#line 190 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HREAD); exit(0); }
    break;

  case 76:
/* Line 670 of lalr1.cc  */
#line 191 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HSECUREDEBUG); exit(0); }
    break;

  case 77:
/* Line 670 of lalr1.cc  */
#line 192 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HDUMP); exit(0); }
    break;

  case 78:
/* Line 670 of lalr1.cc  */
#line 193 "../../s/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HDUMPDIR); exit(0); }
    break;

  case 79:
/* Line 670 of lalr1.cc  */
#line 196 "../../s/cmdoptions.y"
    { ShowBifHelp(0); exit(0); }
    break;

  case 80:
/* Line 670 of lalr1.cc  */
#line 197 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_INIT); exit(0); }
    break;

  case 81:
/* Line 670 of lalr1.cc  */
#line 198 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_UDFBH); exit(0); }
    break;

  case 82:
/* Line 670 of lalr1.cc  */
#line 199 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AES); exit(0); }
    break;

  case 83:
/* Line 670 of lalr1.cc  */
#line 200 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PPK); exit(0); }
    break;

  case 84:
/* Line 670 of lalr1.cc  */
#line 201 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PSK); exit(0); }
    break;

  case 85:
/* Line 670 of lalr1.cc  */
#line 202 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_SPK); exit(0); }
    break;

  case 86:
/* Line 670 of lalr1.cc  */
#line 203 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_SSK); exit(0); }
    break;

  case 87:
/* Line 670 of lalr1.cc  */
#line 204 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_SPKSIGN); exit(0); }
    break;

  case 88:
/* Line 670 of lalr1.cc  */
#line 205 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_HDRSIGN); exit(0); }
    break;

  case 89:
/* Line 670 of lalr1.cc  */
#line 206 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTIMAGE); exit(0); }
    break;

  case 90:
/* Line 670 of lalr1.cc  */
#line 207 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BL); exit(0); }
    break;

  case 91:
/* Line 670 of lalr1.cc  */
#line 208 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_ENCR); exit(0); }
    break;

  case 92:
/* Line 670 of lalr1.cc  */
#line 209 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PID); exit(0); }
    break;

  case 93:
/* Line 670 of lalr1.cc  */
#line 210 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AUTH); exit(0); }
    break;

  case 94:
/* Line 670 of lalr1.cc  */
#line 211 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_CHKSM); exit(0); }
    break;

  case 95:
/* Line 670 of lalr1.cc  */
#line 212 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_POWNER); exit(0); }
    break;

  case 96:
/* Line 670 of lalr1.cc  */
#line 213 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PRESIGN); exit(0); }
    break;

  case 97:
/* Line 670 of lalr1.cc  */
#line 214 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_UDF); exit(0); }
    break;

  case 98:
/* Line 670 of lalr1.cc  */
#line 215 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_XIP); exit(0); }
    break;

  case 99:
/* Line 670 of lalr1.cc  */
#line 216 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_ALIGN); exit(0); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 217 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_OFFSET); exit(0); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 218 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_RES); exit(0); }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 219 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_LOAD); exit(0); }
    break;

  case 103:
/* Line 670 of lalr1.cc  */
#line 220 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_STARTUP); exit(0); }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 221 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_KEYSRC); exit(0); }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 222 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_FSBLCFG); exit(0); }
    break;

  case 106:
/* Line 670 of lalr1.cc  */
#line 223 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTDEV); exit(0); }
    break;

  case 107:
/* Line 670 of lalr1.cc  */
#line 224 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_DESTCPU); exit(0); }
    break;

  case 108:
/* Line 670 of lalr1.cc  */
#line 225 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_DESTDEV); exit(0); }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 226 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_EL); exit(0); }
    break;

  case 110:
/* Line 670 of lalr1.cc  */
#line 227 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_TZ); exit(0); }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 228 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AUTHPARAM); exit(0); }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 229 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BHKEY); exit(0); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 230 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PFW); exit(0); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 231 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BLOCKS); exit(0); }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 232 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BHIV); exit(0); }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 233 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_METAL); exit(0); }
    break;

  case 117:
/* Line 670 of lalr1.cc  */
#line 234 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_ELYHNDOFF); exit(0); }
    break;

  case 118:
/* Line 670 of lalr1.cc  */
#line 235 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_HIVEC); exit(0); }
    break;

  case 119:
/* Line 670 of lalr1.cc  */
#line 236 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BHSIGN); exit(0); }
    break;

  case 120:
/* Line 670 of lalr1.cc  */
#line 237 "../../s/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPLIT,false); exit(0); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 238 "../../s/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPKSIGN,false); exit(0); }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 239 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTVEC); exit(0); }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 240 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PUFDATA); exit(0); }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 241 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AARCH32); exit(0); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 242 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BIGENDIAN); exit(0); }
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 243 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTCONFIG); exit(0); }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 244 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_COPY); exit(0); }
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 245 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_CORE); exit(0); }
    break;

  case 129:
/* Line 670 of lalr1.cc  */
#line 246 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_DELAY_HANDOFF); exit(0); }
    break;

  case 130:
/* Line 670 of lalr1.cc  */
#line 247 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_DELAY_LOAD); exit(0); }
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 248 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_FILE); exit(0); }
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 249 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_ID); exit(0); }
    break;

  case 133:
/* Line 670 of lalr1.cc  */
#line 250 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_IMAGE); exit(0); }
    break;

  case 134:
/* Line 670 of lalr1.cc  */
#line 251 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_METAHDR); exit(0); }
    break;

  case 135:
/* Line 670 of lalr1.cc  */
#line 252 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_NAME); exit(0); }
    break;

  case 136:
/* Line 670 of lalr1.cc  */
#line 253 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PARENTID); exit(0); }
    break;

  case 137:
/* Line 670 of lalr1.cc  */
#line 254 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PARTITION); exit(0); }
    break;

  case 138:
/* Line 670 of lalr1.cc  */
#line 255 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_SLR); exit(0); }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 256 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_TYPE); exit(0); }
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 257 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_KEYSRCENCR); exit(0); }
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 258 "../../s/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_DPACM_ENABLE); exit(0); }
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 261 "../../s/cmdoptions.y"
    { options.SetOverwrite(true); }
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 262 "../../s/cmdoptions.y"
    { options.SetOverwrite(true); }
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 263 "../../s/cmdoptions.y"
    { options.SetOverwrite(false); }
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 267 "../../s/cmdoptions.y"
    { options.SetLogLevel(LogLevel::ERROR); }
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 268 "../../s/cmdoptions.y"
    { options.SetLogLevel(LogLevel::WARNING); }
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 269 "../../s/cmdoptions.y"
    { options.SetLogLevel(LogLevel::INFO); }
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 270 "../../s/cmdoptions.y"
    { options.SetLogLevel(LogLevel::DEBUG); }
    break;

  case 150:
/* Line 670 of lalr1.cc  */
#line 271 "../../s/cmdoptions.y"
    { options.SetLogLevel(LogLevel::TRACE); }
    break;

  case 151:
/* Line 670 of lalr1.cc  */
#line 274 "../../s/cmdoptions.y"
    { options.SetEncryptedKeySource(KeySource::BbramRedKey); }
    break;

  case 152:
/* Line 670 of lalr1.cc  */
#line 275 "../../s/cmdoptions.y"
    { options.SetEncryptedKeySource(KeySource::EfuseRedKey); }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 278 "../../s/cmdoptions.y"
    { options.SetStartCbc((yysemantic_stack_[(3) - (3)].cstring));}
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 279 "../../s/cmdoptions.y"
    { options.SetKey0((yysemantic_stack_[(3) - (3)].cstring));}
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 280 "../../s/cmdoptions.y"
    { options.SetHmac((yysemantic_stack_[(3) - (3)].cstring));}
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 281 "../../s/cmdoptions.y"
    { options.SetEncryptionKeyFile((yysemantic_stack_[(1) - (1)].cstring));}
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 285 "../../s/cmdoptions.y"
    { options.SetProcessBitstreamType(File::MCS); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 286 "../../s/cmdoptions.y"
    { options.SetProcessBitstreamType(File::BIN); }
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 289 "../../s/cmdoptions.y"
    { options.SetDualQspiMode(QspiMode::PARALLEL_LQSPI); }
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 290 "../../s/cmdoptions.y"
    { options.SetDualQspiMode(QspiMode::STACKED); }
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 293 "../../s/cmdoptions.y"
    { LOG_ERROR("'parallel' option is not supported with '-dual_ospi_mode'."); }
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 294 "../../s/cmdoptions.y"
    { options.SetDualQspiMode(QspiMode::STACKED); }
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 297 "../../s/cmdoptions.y"
    { options.SetQspiSize((uint16_t)(yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 169:
/* Line 670 of lalr1.cc  */
#line 298 "../../s/cmdoptions.y"
    { options.SetQspiSize((uint16_t)atoi((yysemantic_stack_[(1) - (1)].cstring))); }
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 301 "../../s/cmdoptions.y"
    { options.SetOspiSize((uint16_t)(yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 171:
/* Line 670 of lalr1.cc  */
#line 302 "../../s/cmdoptions.y"
    { options.SetOspiSize((uint16_t)atoi((yysemantic_stack_[(1) - (1)].cstring))); }
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 305 "../../s/cmdoptions.y"
    { options.SetArchType(Arch::ZYNQ); }
    break;

  case 173:
/* Line 670 of lalr1.cc  */
#line 306 "../../s/cmdoptions.y"
    { options.SetArchType(Arch::ZYNQMP); }
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 307 "../../s/cmdoptions.y"
    { options.SetArchType(Arch::VERSAL); }
    break;

  case 175:
/* Line 670 of lalr1.cc  */
#line 308 "../../s/cmdoptions.y"
    { options.SetArchType(Arch::FPGA); }
    break;

  case 177:
/* Line 670 of lalr1.cc  */
#line 312 "../../s/cmdoptions.y"
    { options.SetGreyKeyGeneration(true); }
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 313 "../../s/cmdoptions.y"
    { options.SetMetalKeyGeneration(true); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 314 "../../s/cmdoptions.y"
    { options.SetAuthKeyGeneration(GenAuthKeys::PEM); }
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 315 "../../s/cmdoptions.y"
    { options.SetAuthKeyGeneration(GenAuthKeys::RSA); }
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 318 "../../s/cmdoptions.y"
    { options.SetAuthKeyGeneration(GenAuthKeys::PEM); }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 319 "../../s/cmdoptions.y"
    { options.SetAuthKeyGeneration(GenAuthKeys::RSA); }
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 322 "../../s/cmdoptions.y"
    { options.SetSecureDebugAuthType(Authentication::ECDSA);
                                                      options.SetSecureDebugImageFile("secureDebugImage-ecdsa.bin"); }
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 324 "../../s/cmdoptions.y"
    { options.SetSecureDebugAuthType(Authentication::RSA);
                                                      options.SetSecureDebugImageFile("secureDebugImage-rsa.bin"); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 327 "../../s/cmdoptions.y"
    { options.SetSecureDebugImageFile((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 330 "../../s/cmdoptions.y"
    { options.SetSecureDebugAuthType(Authentication::ECDSA);
                                                      options.SetSecureDebugImageFile("authenticatedJtagImage-ecdsa.bin"); }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 332 "../../s/cmdoptions.y"
    { options.SetSecureDebugAuthType(Authentication::RSA);
                                                      options.SetSecureDebugImageFile("authenticatedJtagImage-rsa.bin"); }
    break;

  case 189:
/* Line 670 of lalr1.cc  */
#line 336 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(1) - (1)].cstring));
                                                      options.SetVerifyImageOption(true); }
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 339 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(1) - (1)].cstring));
                                                      options.SetReadImageOption(ReadImageOption::ALL); }
    break;

  case 191:
/* Line 670 of lalr1.cc  */
#line 341 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring)); }
    break;

  case 192:
/* Line 670 of lalr1.cc  */
#line 344 "../../s/cmdoptions.y"
    { options.SetReadImageOption(ReadImageOption::BH); }
    break;

  case 193:
/* Line 670 of lalr1.cc  */
#line 345 "../../s/cmdoptions.y"
    { options.SetReadImageOption(ReadImageOption::IHT); }
    break;

  case 194:
/* Line 670 of lalr1.cc  */
#line 346 "../../s/cmdoptions.y"
    { options.SetReadImageOption(ReadImageOption::IH); }
    break;

  case 195:
/* Line 670 of lalr1.cc  */
#line 347 "../../s/cmdoptions.y"
    { options.SetReadImageOption(ReadImageOption::PHT); }
    break;

  case 196:
/* Line 670 of lalr1.cc  */
#line 348 "../../s/cmdoptions.y"
    { options.SetReadImageOption(ReadImageOption::AC); }
    break;

  case 197:
/* Line 670 of lalr1.cc  */
#line 351 "../../s/cmdoptions.y"
    { options.SetDumpOption(DumpOption::BH); }
    break;

  case 198:
/* Line 670 of lalr1.cc  */
#line 352 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring));
                                                      options.SetDumpOption(DumpOption::BH); }
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 354 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (2)].cstring));
                                                      options.SetDumpOption(DumpOption::BH); }
    break;

  case 200:
/* Line 670 of lalr1.cc  */
#line 356 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(1) - (1)].cstring));
                                                      options.SetDumpOption(DumpOption::PARTITIONS); }
    break;

  case 201:
/* Line 670 of lalr1.cc  */
#line 358 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring));
                                                      options.SetDumpOption(DumpOption::PLM); }
    break;

  case 202:
/* Line 670 of lalr1.cc  */
#line 360 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring));
                                                      options.SetDumpOption(DumpOption::PMC_CDO); }
    break;

  case 203:
/* Line 670 of lalr1.cc  */
#line 362 "../../s/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring));
                                                      options.SetDumpOption(DumpOption::BOOT_FILES); }
    break;

  case 204:
/* Line 670 of lalr1.cc  */
#line 366 "../../s/cmdoptions.y"
    { options.SetEncryptionDump(true,"aes_log.txt"); }
    break;

  case 205:
/* Line 670 of lalr1.cc  */
#line 367 "../../s/cmdoptions.y"
    { options.SetEncryptionDump(true,(yysemantic_stack_[(1) - (1)].cstring)); }
    break;


/* Line 670 of lalr1.cc  */
#line 1484 "cmdoptions.tab.cpp"
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
  const signed char BisonParser::yypact_ninf_ = -73;
  const signed char
  BisonParser::yypact_[] =
  {
       -73,     4,   111,   -73,    33,   -69,    29,   101,   -73,   -73,
     -73,   -62,   -72,    22,    11,   104,   -66,   -57,   -13,   -73,
      96,    99,    88,   -73,   -73,    33,   -73,    60,    33,   123,
     -48,   -46,    93,   -51,    33,    33,    33,    33,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,    33,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,    33,    61,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -71,   -73,   -73,   -53,   -73,   -73,
     -73,   -73,   -73,   -73,   -56,   -73,   -73,   -73,   120,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,    33,   -73,
     -73,   -49,   -73,    91,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,    36,    53,   -73,
     -73,   -73,   -73,   -73,   -73,    62,    63,    65,   -73,    85,
      95,    33,   -73,    97,   -73,   -73,   -73
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,    48,     0,    50,    32,    19,
      18,     0,    79,   145,     0,     0,     0,     0,     0,    29,
       0,     0,   142,    30,    34,     0,    17,    20,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   204,     4,     7,
      43,    44,    45,    42,     5,   161,    49,     0,     9,    51,
      52,    53,    54,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    76,    75,    74,    77,    78,    55,    13,    27,   121,
      80,    81,    82,    83,    84,    85,    86,    87,   118,    88,
      89,    90,    92,    91,    93,    94,   117,   119,    95,    96,
      97,    98,    99,   100,   101,   102,   110,   103,   104,   105,
     106,   107,   108,   109,   120,   111,   112,   113,   114,   116,
     115,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   137,   138,   139,   140,   136,
     141,    14,   146,   147,   148,   149,   150,    12,    46,    47,
       6,   162,   163,    23,   197,   200,    39,    40,   172,   173,
     174,   175,    28,   164,     0,    24,   166,     0,    25,   143,
     144,    11,    16,    22,     0,    26,   179,   180,     0,   177,
     178,    31,   184,   183,    35,   188,   187,    36,   186,   151,
     152,   153,    10,   190,    37,   189,    38,    41,   205,    33,
       8,   199,   201,   202,   203,   198,   168,   169,   165,   170,
     171,   167,    21,   181,   182,   176,   185,     0,   155,   192,
     193,   194,   195,   196,   191,     0,     0,   157,   154,     0,
       0,   159,   156,     0,   160,    15,   158
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  BisonParser::yypgoto_[] =
  {
       -73,   -73,   -73,   -73,    -6,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,    38,    44,   150,    39,    77,   141,   171,
     147,   191,   218,   227,   231,   235,    46,   153,   165,   168,
     208,   211,   162,   181,   215,   184,   187,   188,   196,   194,
     224,   156,   199
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned char
  BisonParser::yytable_[] =
  {
        48,   182,   154,   185,     3,   206,   183,    45,   186,   207,
     155,    40,    41,    42,    43,    78,   158,   159,   160,   172,
     161,   157,   175,   209,   212,    79,   192,   210,   193,   195,
     197,   198,   148,   149,   217,    47,   142,   143,   144,   145,
     146,   200,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   202,   203,   204,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,    40,    41,    42,    43,
      40,    41,    42,    43,     4,     5,     6,   225,     7,     8,
       9,    10,    11,    12,    13,   151,   152,   169,   170,   205,
      14,    15,   163,   164,    16,   166,   167,   226,    17,    18,
     173,   174,   228,    19,   229,    20,    21,   230,   201,    22,
     189,   190,    23,    24,    25,    26,    27,    28,    29,   219,
     220,   221,   222,   223,    30,   232,    31,    32,   213,   214,
      33,   176,   177,   178,   179,   180,   233,   236,    34,     0,
       0,     0,   216,     0,    35,    36,    37,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,     0,    70,
      71,    72,    73,    74,    75,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   234,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    76
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
         6,    49,    68,    49,     0,    76,    54,    76,    54,    80,
      16,    77,    78,    79,    80,    77,    29,    30,    31,    25,
      33,    78,    28,    76,    80,    97,    77,    80,    34,    35,
      36,    37,    21,    22,    83,     6,    14,    15,    16,    17,
      18,    47,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   154,   155,   156,    24,    25,    26,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,    77,    78,    79,    80,
      77,    78,    79,    80,     3,     4,     5,    81,     7,     8,
       9,    10,    11,    12,    13,    21,    22,    39,    40,    68,
      19,    20,    36,    37,    23,    36,    37,    84,    27,    28,
      80,    81,    80,    32,    81,    34,    35,    82,   154,    38,
      57,    58,    41,    42,    43,    44,    45,    46,    47,    68,
      69,    70,    71,    72,    53,    80,    55,    56,    48,    49,
      59,    48,    49,    50,    51,    52,    81,    80,    67,    -1,
      -1,    -1,   188,    -1,    73,    74,    75,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,    -1,   108,
     109,   110,   111,   112,   113,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   231,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   148
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   179,   180,     0,     3,     4,     5,     7,     8,     9,
      10,    11,    12,    13,    19,    20,    23,    27,    28,    32,
      34,    35,    38,    41,    42,    43,    44,    45,    46,    47,
      53,    55,    56,    59,    67,    73,    74,    75,   181,   184,
      77,    78,    79,    80,   182,    76,   194,     6,   182,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     108,   109,   110,   111,   112,   113,   148,   185,    77,    97,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   186,    14,    15,    16,    17,    18,   188,    21,    22,
     183,    21,    22,   195,    68,   182,   209,    78,    29,    30,
      31,    33,   200,    36,    37,   196,    36,    37,   197,    39,
      40,   187,   182,    80,    81,   182,    48,    49,    50,    51,
      52,   201,    49,    54,   203,    49,    54,   204,   205,    57,
      58,   189,    77,   182,   207,   182,   206,   182,   182,   210,
     182,   182,    24,    25,    26,    68,    76,    80,   198,    76,
      80,   199,    80,    48,    49,   202,   182,    83,   190,    68,
      69,    70,    71,    72,   208,    81,    84,   191,    80,    81,
      82,   192,    80,    81,   182,   193,    80
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
     415,   416,   417,   418,   419,   420,   421,   422,   423,   424,
     425,   426,   427,   428,   429,   430,   431,   432
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   178,   179,   180,   180,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   182,   182,   182,   182,   183,   183,   184,   184,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   185,   185,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   187,   187,   187,   188,   188,   188,   188,   188,
     188,   189,   189,   190,   190,   191,   191,   192,   192,   193,
     193,   194,   195,   195,   196,   196,   197,   197,   198,   198,
     199,   199,   200,   200,   200,   200,   201,   201,   201,   201,
     201,   202,   202,   203,   203,   204,   204,   205,   205,   206,
     207,   207,   208,   208,   208,   208,   208,   209,   209,   209,
     209,   209,   209,   209,   210,   210
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     2,     2,     1,     3,     2,
       2,     2,     2,     2,     2,     6,     2,     1,     1,     1,
       1,     3,     2,     2,     2,     2,     2,     2,     2,     1,
       1,     2,     1,     2,     1,     2,     2,     2,     2,     2,
       2,     2,     1,     1,     1,     1,     1,     1,     1,     2,
       0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     1,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     0,     3,     0,     3,     0,     3,     0,
       1,     1,     1,     1,     1,     2,     1,     2,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     2,     2,     2,     0,     1
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
  "_PROCESS_BITSTREAM", "MCS", "BIN", "_DUMP", "DUMP_PLM", "DUMP_PMC_CDO",
  "DUMP_BOOT_FILES", "_DUMP_DIR", "_ARCH", "ZYNQ", "ZYNQMP", "VERSAL",
  "_R", "FPGA", "_DUAL_QSPI_MODE", "_DUAL_OSPI_MODE", "PARALLEL",
  "STACKED", "_W", "ON", "OFF", "_NOAUTHBLOCKS", "_ZYNQMPES1",
  "_EFUSEPPKBITS", "_GENERATE_HASHES", "_PADIMAGEHEADER", "_SPKSIGNATURE",
  "_GENERATE_KEYS", "PEM", "RSA", "AUTH", "GREY", "METAL", "_SECUREDEBUG",
  "ECDSA", "_AUTHJTAG", "_ENCRYPT", "BBRAM", "EFUSE", "_P_TOK",
  "_INTERFACE", "SMAPx8", "SMAPx16", "SMAPx32", "SPI", "BPIx8", "BPIx16",
  "_READ", "READ_BH", "READ_IHT", "READ_IH", "READ_PHT", "READ_AC",
  "_VERIFY", "_VERIFYKDF", "_ZYNQMPENCRDUMP", "HEXVALUE", "IDENTIFIER",
  "FILENAME", "QFILENAME", "HEXSTRING", "EQUALS", "HMAC", "STARTCBC",
  "KEY0", "COMMA", "HBIFHELP", "HARCH", "HIMAGE", "HFILL", "HO", "HP",
  "HW", "HEFUSEPPKBITS", "HGENHASHES", "HLEGACY", "HPADHDR", "H_SPKSIGN",
  "HPACKAGE", "HENCRYPT", "HGENKEYS", "HDQSPI", "HLOG", "HZYNQMPES1",
  "HPROCESSBIT", "HNONBOOTING", "HENCRDUMP", "HPOSTPROCESS", "HVERIFY",
  "HSECUREDEBUG", "HREAD", "HVERIFYKDF", "HDUMP", "HDUMPDIR", "H_BIF_INIT",
  "H_BIF_UDFBH", "H_BIF_AES", "H_BIF_PPK", "H_BIF_PSK", "H_BIF_SPK",
  "H_BIF_SSK", "H_BIF_SPKSIGN", "H_BIF_HIVEC", "H_BIF_HDRSIGN",
  "H_BIF_BOOTIMAGE", "H_BIF_BL", "H_BIF_PID", "H_BIF_ENCR", "H_BIF_AUTH",
  "H_BIF_CHKSM", "H_BIF_ELYHNDOFF", "H_BIF_BHSIGN", "H_BIF_POWNER",
  "H_BIF_PRESIGN", "H_BIF_UDF", "H_BIF_XIP", "H_BIF_ALIGN", "H_BIF_OFFSET",
  "H_BIF_RES", "H_BIF_LOAD", "H_BIF_TZ", "H_BIF_STARTUP", "H_BIF_KEYSRC",
  "H_BIF_FSBLCFG", "H_BIF_BOOTDEV", "H_BIF_DESTCPU", "H_BIF_DESTDEV",
  "H_BIF_EL", "H_SPLIT", "H_BIF_AUTHPARAM", "H_BIF_BHKEY", "H_BIF_PFW",
  "H_BIF_BLOCKS", "H_BIF_METAL", "H_BIF_BHIV", "H_BIF_BOOTVEC",
  "H_BIF_PUFDATA", "H_BIF_PTYPE", "H_BIF_IMAGECFG", "H_BIF_PMCCONFIG",
  "H_BIF_AARCH32", "H_BIF_BIGENDIAN", "H_BIF_BOOTCONFIG", "H_BIF_COPY",
  "H_BIF_CORE", "H_BIF_DELAY_HANDOFF", "H_BIF_DELAY_LOAD", "H_BIF_FILE",
  "H_BIF_ID", "H_BIF_IMAGE", "H_BIF_METAHDR", "H_BIF_NAME",
  "H_BIF_PARTITION", "H_BIF_SLR", "H_BIF_TYPE", "H_BIF_KEYSRCENCR",
  "H_BIF_PARENTID", "H_DPACM_ENABLE", "$accept", "top", "option_list",
  "option", "filename", "memsplit", "filloption", "helpoption",
  "bifhelpoption", "wopt", "loglevel", "keysource", "startcbc", "key0",
  "hmac", "keyfile", "number", "bitfile_type", "qpsi_mode", "opsi_mode",
  "qspi_size", "ospi_size", "archOptions", "key_type", "auth_key_options",
  "auth_type", "authJtagOptions", "authJtagType", "verifyImageOptions",
  "readImageOptions", "readType", "dumpOptions", "encrDumpOptions", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BisonParser::rhs_number_type
  BisonParser::yyrhs_[] =
  {
       179,     0,    -1,   180,    -1,    -1,   180,   181,    -1,     3,
     182,    -1,    19,   183,    -1,   184,    -1,     5,     6,   182,
      -1,     5,   182,    -1,    59,    77,    -1,    38,   187,    -1,
      13,   188,    -1,     7,   185,    -1,    12,   186,    -1,    56,
     189,   190,   191,   192,   193,    -1,    43,   182,    -1,    44,
      -1,    10,    -1,     9,    -1,    45,    -1,    45,    81,    80,
      -1,    45,    80,    -1,    20,   195,    -1,    34,   196,    -1,
      35,   197,    -1,    46,   182,    -1,    11,    77,    -1,    28,
     200,    -1,    32,    -1,    41,    -1,    47,   201,    -1,     8,
      -1,    75,   210,    -1,    42,    -1,    53,   203,    -1,    55,
     204,    -1,    67,   207,    -1,    73,   206,    -1,    23,   209,
      -1,    27,    78,    -1,    74,   182,    -1,    80,    -1,    77,
      -1,    78,    -1,    79,    -1,    21,    -1,    22,    -1,     4,
      -1,     4,   194,    -1,    -1,    86,    -1,    87,    -1,    88,
      -1,    89,    -1,   148,    -1,    90,    -1,    91,    -1,    92,
      -1,    93,    -1,    94,    -1,    95,    -1,    96,    -1,    97,
      -1,    98,    -1,    99,    -1,   100,    -1,   101,    -1,   102,
      -1,   103,    -1,   104,    -1,   105,    -1,   106,    -1,   108,
      -1,   111,    -1,   110,    -1,   109,    -1,   112,    -1,   113,
      -1,    -1,   114,    -1,   115,    -1,   116,    -1,   117,    -1,
     118,    -1,   119,    -1,   120,    -1,   121,    -1,   123,    -1,
     124,    -1,   125,    -1,   127,    -1,   126,    -1,   128,    -1,
     129,    -1,   132,    -1,   133,    -1,   134,    -1,   135,    -1,
     136,    -1,   137,    -1,   138,    -1,   139,    -1,   141,    -1,
     142,    -1,   143,    -1,   144,    -1,   145,    -1,   146,    -1,
     147,    -1,   140,    -1,   149,    -1,   150,    -1,   151,    -1,
     152,    -1,   154,    -1,   153,    -1,   130,    -1,   122,    -1,
     131,    -1,   148,    -1,    97,    -1,   155,    -1,   156,    -1,
     160,    -1,   161,    -1,   162,    -1,   163,    -1,   164,    -1,
     165,    -1,   166,    -1,   167,    -1,   168,    -1,   169,    -1,
     170,    -1,   171,    -1,   176,    -1,   172,    -1,   173,    -1,
     174,    -1,   175,    -1,   177,    -1,    -1,    39,    -1,    40,
      -1,    -1,    14,    -1,    15,    -1,    16,    -1,    17,    -1,
      18,    -1,    57,    -1,    58,    -1,    -1,    83,    81,    80,
      -1,    -1,    84,    81,    80,    -1,    -1,    82,    81,    80,
      -1,    -1,   182,    -1,    76,    -1,    21,    -1,    22,    -1,
      36,    -1,    37,   198,    -1,    36,    -1,    37,   199,    -1,
      76,    -1,    80,    -1,    76,    -1,    80,    -1,    29,    -1,
      30,    -1,    31,    -1,    33,    -1,    50,   202,    -1,    51,
      -1,    52,    -1,    48,    -1,    49,    -1,    48,    -1,    49,
      -1,    54,    -1,    49,    -1,   205,   182,    -1,   205,    -1,
      54,    -1,    49,    -1,   182,    -1,   182,    -1,   182,   208,
      -1,    68,    -1,    69,    -1,    70,    -1,    71,    -1,    72,
      -1,    68,    -1,   182,    68,    -1,    68,   182,    -1,   182,
      -1,   182,    24,    -1,   182,    25,    -1,   182,    26,    -1,
      -1,   182,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     5,     6,     9,    12,    15,    17,    21,
      24,    27,    30,    33,    36,    39,    46,    49,    51,    53,
      55,    57,    61,    64,    67,    70,    73,    76,    79,    82,
      84,    86,    89,    91,    94,    96,    99,   102,   105,   108,
     111,   114,   117,   119,   121,   123,   125,   127,   129,   131,
     134,   135,   137,   139,   141,   143,   145,   147,   149,   151,
     153,   155,   157,   159,   161,   163,   165,   167,   169,   171,
     173,   175,   177,   179,   181,   183,   185,   187,   189,   191,
     192,   194,   196,   198,   200,   202,   204,   206,   208,   210,
     212,   214,   216,   218,   220,   222,   224,   226,   228,   230,
     232,   234,   236,   238,   240,   242,   244,   246,   248,   250,
     252,   254,   256,   258,   260,   262,   264,   266,   268,   270,
     272,   274,   276,   278,   280,   282,   284,   286,   288,   290,
     292,   294,   296,   298,   300,   302,   304,   306,   308,   310,
     312,   314,   316,   317,   319,   321,   322,   324,   326,   328,
     330,   332,   334,   336,   337,   341,   342,   346,   347,   351,
     352,   354,   356,   358,   360,   362,   365,   367,   370,   372,
     374,   376,   378,   380,   382,   384,   386,   389,   391,   393,
     395,   397,   399,   401,   403,   405,   408,   410,   412,   414,
     416,   418,   421,   423,   425,   427,   429,   431,   433,   436,
     439,   441,   444,   447,   450,   451
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   104,   104,   106,   107,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   151,   151,   151,   151,   153,   154,   157,   158,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,   238,   239,   240,   241,   242,   243,   244,   245,   246,
     247,   248,   249,   250,   251,   252,   253,   254,   255,   256,
     257,   258,   261,   262,   263,   266,   267,   268,   269,   270,
     271,   274,   275,   278,   278,   279,   279,   280,   280,   281,
     281,   283,   285,   286,   289,   290,   293,   294,   297,   298,
     301,   302,   305,   306,   307,   308,   311,   312,   313,   314,
     315,   318,   319,   322,   324,   327,   328,   330,   332,   336,
     339,   341,   344,   345,   346,   347,   348,   351,   352,   354,
     356,   358,   360,   362,   366,   367
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
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 249;
  const int BisonParser::yynnts_ = 33;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 178;

  const unsigned int BisonParser::yyuser_token_number_max_ = 432;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 23 "../../s/cmdoptions.y"
} // CO
/* Line 1141 of lalr1.cc  */
#line 2215 "cmdoptions.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 370 "../../s/cmdoptions.y"



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

    case CO::BisonParser::token::HPOSTPROCESS: 
        std::cout << POSTPROCESSHELP << std::endl;
        break;
    
    case CO::BisonParser::token::HVERIFYKDF:
        std::cout << VERIFYKDFHELP << std::endl;
        break;

    case CO::BisonParser::token::HREAD:
        std::cout << READHELP << std::endl;
        break;

    case CO::BisonParser::token::HSECUREDEBUG:
        std::cout << SECUREDEBUGHELP << std::endl;
        break;

    case CO::BisonParser::token::HDUMP:
        std::cout << DUMPHELP << std::endl;
        break;
     
    case CO::BisonParser::token::HDUMPDIR:
        std::cout << DUMPDIRHELP << std::endl;
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

    case CO::BisonParser::token::H_BIF_BOOTIMAGE:
        std::cout << H_BIF_BOOTIMAGE_H << std::endl;
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

    case CO::BisonParser::token::H_BIF_BHSIGN:
        std::cout << H_BIF_BHSIGN_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_BOOTVEC:
        std::cout << H_BIF_BOOTVEC_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PUFDATA:
        std::cout << H_BIF_PUFDATA_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_AARCH32:
        std::cout << H_BIF_AARCH32_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_BIGENDIAN:
        std::cout << H_BIF_BIGENDIAN_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_BOOTCONFIG:
        std::cout << H_BIF_BOOTCONFIG_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_COPY:
        std::cout << H_BIF_COPY_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_CORE:
        std::cout << H_BIF_CORE_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_DELAY_HANDOFF:
        std::cout << H_BIF_DELAY_HANDOFF_H << std::endl;
        break;
        
        case CO::BisonParser::token::H_BIF_DELAY_LOAD:
        std::cout << H_BIF_DELAY_LOAD_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_FILE:
        std::cout << H_BIF_FILE_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_ID:
        std::cout << H_BIF_ID_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_IMAGE:
        std::cout << H_BIF_IMAGE_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_METAHDR:
        std::cout << H_BIF_METAHDR_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_NAME:
        std::cout << H_BIF_NAME_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_PARENTID:
        std::cout << H_BIF_PARENTID_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_PARTITION:
        std::cout << H_BIF_PARTITION_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_SLR:
        std::cout << H_BIF_SLR_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_TYPE:
        std::cout << H_BIF_TYPE_H << std::endl;
        break;
        
    case CO::BisonParser::token::H_BIF_KEYSRCENCR:
        std::cout << H_BIF_KEYSRCENCR_H << std::endl;
        break;

    case CO::BisonParser::token::H_DPACM_ENABLE:
        std::cout << H_DPACM_ENABLE_H << std::endl;
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
