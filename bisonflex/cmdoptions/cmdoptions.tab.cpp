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
#line 49 "../../parser/cmdoptions.y"

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
#line 45 "../../parser/cmdoptions.y"

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
#line 24 "../../parser/cmdoptions.y"
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
        case 85: /* IDENTIFIER */
/* Line 455 of lalr1.cc  */
#line 70 "../../parser/cmdoptions.y"
        { free(((*yyvaluep).cstring)); };
/* Line 455 of lalr1.cc  */
#line 232 "cmdoptions.tab.cpp"
        break;
      case 86: /* FILENAME */
/* Line 455 of lalr1.cc  */
#line 70 "../../parser/cmdoptions.y"
        { free(((*yyvaluep).cstring)); };
/* Line 455 of lalr1.cc  */
#line 239 "cmdoptions.tab.cpp"
        break;
      case 87: /* QFILENAME */
/* Line 455 of lalr1.cc  */
#line 70 "../../parser/cmdoptions.y"
        { free(((*yyvaluep).cstring)); };
/* Line 455 of lalr1.cc  */
#line 246 "cmdoptions.tab.cpp"
        break;
      case 88: /* HEXSTRING */
/* Line 455 of lalr1.cc  */
#line 70 "../../parser/cmdoptions.y"
        { free(((*yyvaluep).cstring)); };
/* Line 455 of lalr1.cc  */
#line 253 "cmdoptions.tab.cpp"
        break;
      case 198: /* filename */
/* Line 455 of lalr1.cc  */
#line 70 "../../parser/cmdoptions.y"
        { free(((*yyvaluep).cstring)); };
/* Line 455 of lalr1.cc  */
#line 260 "cmdoptions.tab.cpp"
        break;

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
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }

    // Perform the reduction.
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
          case 5:
/* Line 670 of lalr1.cc  */
#line 112 "../../parser/cmdoptions.y"
    { options.SetBifFilename((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 8:
/* Line 670 of lalr1.cc  */
#line 115 "../../parser/cmdoptions.y"
    { options.GetOutputFileNames().push_back((yysemantic_stack_[(3) - (3)].cstring)); }
    break;

  case 9:
/* Line 670 of lalr1.cc  */
#line 116 "../../parser/cmdoptions.y"
    { options.GetOutputFileNames().push_back((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 10:
/* Line 670 of lalr1.cc  */
#line 117 "../../parser/cmdoptions.y"
    { options.SetDevicePartName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 16:
/* Line 670 of lalr1.cc  */
#line 123 "../../parser/cmdoptions.y"
    { options.SetEfuseHashFileName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 17:
/* Line 670 of lalr1.cc  */
#line 124 "../../parser/cmdoptions.y"
    { options.SetEfusePufHashFileName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 18:
/* Line 670 of lalr1.cc  */
#line 125 "../../parser/cmdoptions.y"
    { options.SetGenerateHashes(true); }
    break;

  case 19:
/* Line 670 of lalr1.cc  */
#line 126 "../../parser/cmdoptions.y"
    { options.SetNonBootingFlag(true); }
    break;

  case 20:
/* Line 670 of lalr1.cc  */
#line 127 "../../parser/cmdoptions.y"
    { options.SetLegacyFlag(true); }
    break;

  case 21:
/* Line 670 of lalr1.cc  */
#line 128 "../../parser/cmdoptions.y"
    { options.SetPadHeaderTable(true); }
    break;

  case 22:
/* Line 670 of lalr1.cc  */
#line 129 "../../parser/cmdoptions.y"
    { options.SetPadHeaderTable((bool)(strcmp((yysemantic_stack_[(3) - (3)].cstring),"0"))); }
    break;

  case 23:
/* Line 670 of lalr1.cc  */
#line 130 "../../parser/cmdoptions.y"
    { options.SetPadHeaderTable((bool)(strcmp((yysemantic_stack_[(2) - (2)].cstring),"0"))); }
    break;

  case 27:
/* Line 670 of lalr1.cc  */
#line 134 "../../parser/cmdoptions.y"
    { options.SetSpkSigFileName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 28:
/* Line 670 of lalr1.cc  */
#line 135 "../../parser/cmdoptions.y"
    { options.SetDevicePackageName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 30:
/* Line 670 of lalr1.cc  */
#line 137 "../../parser/cmdoptions.y"
    { options.SetArchType(Arch::ZYNQMP); }
    break;

  case 31:
/* Line 670 of lalr1.cc  */
#line 138 "../../parser/cmdoptions.y"
    { options.SetNoAuthBlocksFlag(true); }
    break;

  case 33:
/* Line 670 of lalr1.cc  */
#line 140 "../../parser/cmdoptions.y"
    { LOG_ERROR("'-debug' option is no more supported. Please use '-log' option"); }
    break;

  case 35:
/* Line 670 of lalr1.cc  */
#line 142 "../../parser/cmdoptions.y"
    { options.SetZynqmpes1Flag(true); }
    break;

  case 36:
/* Line 670 of lalr1.cc  */
#line 143 "../../parser/cmdoptions.y"
    { LOG_ERROR("The option '-securedebugimage' is deprecated. Use '-authenticatedjtag' instead."); }
    break;

  case 41:
/* Line 670 of lalr1.cc  */
#line 148 "../../parser/cmdoptions.y"
    { options.SetDumpDirectory((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 42:
/* Line 670 of lalr1.cc  */
#line 149 "../../parser/cmdoptions.y"
    { options.SetPufOutputFileName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 43:
/* Line 670 of lalr1.cc  */
#line 150 "../../parser/cmdoptions.y"
    { options.SetKDFTestVectorFile((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 44:
/* Line 670 of lalr1.cc  */
#line 151 "../../parser/cmdoptions.y"
    { options.SetAuthOptimization();}
    break;

  case 45:
/* Line 670 of lalr1.cc  */
#line 152 "../../parser/cmdoptions.y"
    { options.SetOverlayCDOFileName((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 51:
/* Line 670 of lalr1.cc  */
#line 158 "../../parser/cmdoptions.y"
    { options.SetSplitType(File::MCS); }
    break;

  case 52:
/* Line 670 of lalr1.cc  */
#line 159 "../../parser/cmdoptions.y"
    { options.SetSplitType(File::BIN); }
    break;

  case 53:
/* Line 670 of lalr1.cc  */
#line 162 "../../parser/cmdoptions.y"
    { options.SetDoFill(true); }
    break;

  case 54:
/* Line 670 of lalr1.cc  */
#line 163 "../../parser/cmdoptions.y"
    { options.SetDoFill(true);
                                                      if ((yysemantic_stack_[(2) - (2)].number) >= 0 && (yysemantic_stack_[(2) - (2)].number) <= 255)
                                                        options.SetOutputFillByte((uint8_t)(yysemantic_stack_[(2) - (2)].number)); 
                                                      else 
                                                        LOG_ERROR("'-fill' - Fill byte must be 8 bits"); }
    break;

  case 55:
/* Line 670 of lalr1.cc  */
#line 170 "../../parser/cmdoptions.y"
    { options.SetOutType(File::MCS); }
    break;

  case 56:
/* Line 670 of lalr1.cc  */
#line 171 "../../parser/cmdoptions.y"
    { options.SetOutType(File::BIN); }
    break;

  case 57:
/* Line 670 of lalr1.cc  */
#line 174 "../../parser/cmdoptions.y"
    { ShowHelp(); exit(0); }
    break;

  case 58:
/* Line 670 of lalr1.cc  */
#line 175 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HBIFHELP); exit(0); }
    break;

  case 59:
/* Line 670 of lalr1.cc  */
#line 176 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HARCH); exit(0); }
    break;

  case 60:
/* Line 670 of lalr1.cc  */
#line 177 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HAUTHOPT); exit(0); }
    break;

  case 61:
/* Line 670 of lalr1.cc  */
#line 178 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HIMAGE); exit(0); }
    break;

  case 62:
/* Line 670 of lalr1.cc  */
#line 179 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HFILL); exit(0); }
    break;

  case 63:
/* Line 670 of lalr1.cc  */
#line 180 "../../parser/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPLIT,true); exit(0); }
    break;

  case 64:
/* Line 670 of lalr1.cc  */
#line 181 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HO); exit(0); }
    break;

  case 65:
/* Line 670 of lalr1.cc  */
#line 182 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HP); exit(0); }
    break;

  case 66:
/* Line 670 of lalr1.cc  */
#line 183 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HW); exit(0); }
    break;

  case 67:
/* Line 670 of lalr1.cc  */
#line 184 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HEFUSEPPKBITS); exit(0); }
    break;

  case 68:
/* Line 670 of lalr1.cc  */
#line 185 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HGENHASHES); exit(0); }
    break;

  case 69:
/* Line 670 of lalr1.cc  */
#line 186 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HLEGACY); exit(0); }
    break;

  case 70:
/* Line 670 of lalr1.cc  */
#line 187 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HPADHDR); exit(0); }
    break;

  case 71:
/* Line 670 of lalr1.cc  */
#line 188 "../../parser/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPKSIGN,true); exit(0); }
    break;

  case 72:
/* Line 670 of lalr1.cc  */
#line 189 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HPACKAGE); exit(0); }
    break;

  case 73:
/* Line 670 of lalr1.cc  */
#line 190 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HENCRYPT); exit(0); }
    break;

  case 74:
/* Line 670 of lalr1.cc  */
#line 191 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HGENKEYS); exit(0); }
    break;

  case 75:
/* Line 670 of lalr1.cc  */
#line 192 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HDQSPI); exit(0); }
    break;

  case 76:
/* Line 670 of lalr1.cc  */
#line 193 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HLOG); exit(0); }
    break;

  case 77:
/* Line 670 of lalr1.cc  */
#line 194 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HZYNQMPES1); exit(0); }
    break;

  case 78:
/* Line 670 of lalr1.cc  */
#line 195 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HPROCESSBIT); exit(0); }
    break;

  case 79:
/* Line 670 of lalr1.cc  */
#line 196 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HNONBOOTING); exit(0); }
    break;

  case 80:
/* Line 670 of lalr1.cc  */
#line 197 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HENCRDUMP); exit(0); }
    break;

  case 81:
/* Line 670 of lalr1.cc  */
#line 198 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HVERIFY); exit(0); }
    break;

  case 82:
/* Line 670 of lalr1.cc  */
#line 199 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HVERIFYKDF); exit(0); }
    break;

  case 83:
/* Line 670 of lalr1.cc  */
#line 200 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HREAD); exit(0); }
    break;

  case 84:
/* Line 670 of lalr1.cc  */
#line 201 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HSECUREDEBUG); exit(0); }
    break;

  case 85:
/* Line 670 of lalr1.cc  */
#line 202 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HDUMP); exit(0); }
    break;

  case 86:
/* Line 670 of lalr1.cc  */
#line 203 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HDUMPDIR); exit(0); }
    break;

  case 87:
/* Line 670 of lalr1.cc  */
#line 204 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HOVLCDO); exit(0); }
    break;

  case 88:
/* Line 670 of lalr1.cc  */
#line 205 "../../parser/cmdoptions.y"
    { ShowCmdHelp(CO::BisonParser::token::HOUTTYPE); exit(0); }
    break;

  case 89:
/* Line 670 of lalr1.cc  */
#line 208 "../../parser/cmdoptions.y"
    { ShowBifHelp(0); exit(0); }
    break;

  case 90:
/* Line 670 of lalr1.cc  */
#line 209 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_INIT); exit(0); }
    break;

  case 91:
/* Line 670 of lalr1.cc  */
#line 210 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_UDFBH); exit(0); }
    break;

  case 92:
/* Line 670 of lalr1.cc  */
#line 211 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AES); exit(0); }
    break;

  case 93:
/* Line 670 of lalr1.cc  */
#line 212 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PPK); exit(0); }
    break;

  case 94:
/* Line 670 of lalr1.cc  */
#line 213 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PSK); exit(0); }
    break;

  case 95:
/* Line 670 of lalr1.cc  */
#line 214 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_SPK); exit(0); }
    break;

  case 96:
/* Line 670 of lalr1.cc  */
#line 215 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_SSK); exit(0); }
    break;

  case 97:
/* Line 670 of lalr1.cc  */
#line 216 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_SPKSIGN); exit(0); }
    break;

  case 98:
/* Line 670 of lalr1.cc  */
#line 217 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_HDRSIGN); exit(0); }
    break;

  case 99:
/* Line 670 of lalr1.cc  */
#line 218 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTIMAGE); exit(0); }
    break;

  case 100:
/* Line 670 of lalr1.cc  */
#line 219 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BL); exit(0); }
    break;

  case 101:
/* Line 670 of lalr1.cc  */
#line 220 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_ENCR); exit(0); }
    break;

  case 102:
/* Line 670 of lalr1.cc  */
#line 221 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PID); exit(0); }
    break;

  case 103:
/* Line 670 of lalr1.cc  */
#line 222 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AUTH); exit(0); }
    break;

  case 104:
/* Line 670 of lalr1.cc  */
#line 223 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_TCMBOOT); exit(0); }
    break;

  case 105:
/* Line 670 of lalr1.cc  */
#line 224 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_OPTIONALDATA); exit(0); }
    break;

  case 106:
/* Line 670 of lalr1.cc  */
#line 225 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_CHKSM); exit(0); }
    break;

  case 107:
/* Line 670 of lalr1.cc  */
#line 226 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_POWNER); exit(0); }
    break;

  case 108:
/* Line 670 of lalr1.cc  */
#line 227 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PRESIGN); exit(0); }
    break;

  case 109:
/* Line 670 of lalr1.cc  */
#line 228 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_UDF); exit(0); }
    break;

  case 110:
/* Line 670 of lalr1.cc  */
#line 229 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_XIP); exit(0); }
    break;

  case 111:
/* Line 670 of lalr1.cc  */
#line 230 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_ALIGN); exit(0); }
    break;

  case 112:
/* Line 670 of lalr1.cc  */
#line 231 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_OFFSET); exit(0); }
    break;

  case 113:
/* Line 670 of lalr1.cc  */
#line 232 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_RES); exit(0); }
    break;

  case 114:
/* Line 670 of lalr1.cc  */
#line 233 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_LOAD); exit(0); }
    break;

  case 115:
/* Line 670 of lalr1.cc  */
#line 234 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_STARTUP); exit(0); }
    break;

  case 116:
/* Line 670 of lalr1.cc  */
#line 235 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_KEYSRC); exit(0); }
    break;

  case 117:
/* Line 670 of lalr1.cc  */
#line 236 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_FSBLCFG); exit(0); }
    break;

  case 118:
/* Line 670 of lalr1.cc  */
#line 237 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTDEV); exit(0); }
    break;

  case 119:
/* Line 670 of lalr1.cc  */
#line 238 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_DESTCPU); exit(0); }
    break;

  case 120:
/* Line 670 of lalr1.cc  */
#line 239 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_DESTDEV); exit(0); }
    break;

  case 121:
/* Line 670 of lalr1.cc  */
#line 240 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_EL); exit(0); }
    break;

  case 122:
/* Line 670 of lalr1.cc  */
#line 241 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_TZ); exit(0); }
    break;

  case 123:
/* Line 670 of lalr1.cc  */
#line 242 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AUTHPARAM); exit(0); }
    break;

  case 124:
/* Line 670 of lalr1.cc  */
#line 243 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BHKEY); exit(0); }
    break;

  case 125:
/* Line 670 of lalr1.cc  */
#line 244 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PFW); exit(0); }
    break;

  case 126:
/* Line 670 of lalr1.cc  */
#line 245 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BLOCKS); exit(0); }
    break;

  case 127:
/* Line 670 of lalr1.cc  */
#line 246 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BHIV); exit(0); }
    break;

  case 128:
/* Line 670 of lalr1.cc  */
#line 247 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_METAL); exit(0); }
    break;

  case 129:
/* Line 670 of lalr1.cc  */
#line 248 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_ELYHNDOFF); exit(0); }
    break;

  case 130:
/* Line 670 of lalr1.cc  */
#line 249 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_HIVEC); exit(0); }
    break;

  case 131:
/* Line 670 of lalr1.cc  */
#line 250 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BHSIGN); exit(0); }
    break;

  case 132:
/* Line 670 of lalr1.cc  */
#line 251 "../../parser/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPLIT,false); exit(0); }
    break;

  case 133:
/* Line 670 of lalr1.cc  */
#line 252 "../../parser/cmdoptions.y"
    { ShowCommonHelp(CO::BisonParser::token::H_SPKSIGN,false); exit(0); }
    break;

  case 134:
/* Line 670 of lalr1.cc  */
#line 253 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTVEC); exit(0); }
    break;

  case 135:
/* Line 670 of lalr1.cc  */
#line 254 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PUFDATA); exit(0); }
    break;

  case 136:
/* Line 670 of lalr1.cc  */
#line 255 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_AARCH32); exit(0); }
    break;

  case 137:
/* Line 670 of lalr1.cc  */
#line 256 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BIGENDIAN); exit(0); }
    break;

  case 138:
/* Line 670 of lalr1.cc  */
#line 257 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_BOOTCONFIG); exit(0); }
    break;

  case 139:
/* Line 670 of lalr1.cc  */
#line 258 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_COPY); exit(0); }
    break;

  case 140:
/* Line 670 of lalr1.cc  */
#line 259 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_CORE); exit(0); }
    break;

  case 141:
/* Line 670 of lalr1.cc  */
#line 260 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_DELAY_HANDOFF); exit(0); }
    break;

  case 142:
/* Line 670 of lalr1.cc  */
#line 261 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_DELAY_LOAD); exit(0); }
    break;

  case 143:
/* Line 670 of lalr1.cc  */
#line 262 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_FILE); exit(0); }
    break;

  case 144:
/* Line 670 of lalr1.cc  */
#line 263 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_ID); exit(0); }
    break;

  case 145:
/* Line 670 of lalr1.cc  */
#line 264 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_IMAGE); exit(0); }
    break;

  case 146:
/* Line 670 of lalr1.cc  */
#line 265 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_METAHDR); exit(0); }
    break;

  case 147:
/* Line 670 of lalr1.cc  */
#line 266 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_NAME); exit(0); }
    break;

  case 148:
/* Line 670 of lalr1.cc  */
#line 267 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PARENTID); exit(0); }
    break;

  case 149:
/* Line 670 of lalr1.cc  */
#line 268 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_PARTITION); exit(0); }
    break;

  case 150:
/* Line 670 of lalr1.cc  */
#line 269 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_SLR); exit(0); }
    break;

  case 151:
/* Line 670 of lalr1.cc  */
#line 270 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_TYPE); exit(0); }
    break;

  case 152:
/* Line 670 of lalr1.cc  */
#line 271 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_KEYSRCENCR); exit(0); }
    break;

  case 153:
/* Line 670 of lalr1.cc  */
#line 272 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_DPACM_ENABLE); exit(0); }
    break;

  case 154:
/* Line 670 of lalr1.cc  */
#line 273 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::H_BIF_USERKEYS); exit(0); }
    break;

  case 155:
/* Line 670 of lalr1.cc  */
#line 274 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::HVN_BIF_PCR); exit(0); }
    break;

  case 156:
/* Line 670 of lalr1.cc  */
#line 275 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::HVN_BIF_PCR_MINDEX); exit(0); }
    break;

  case 157:
/* Line 670 of lalr1.cc  */
#line 276 "../../parser/cmdoptions.y"
    { ShowBifHelp(CO::BisonParser::token::HV_BIF_IMAGESTORE); exit(0); }
    break;

  case 158:
/* Line 670 of lalr1.cc  */
#line 279 "../../parser/cmdoptions.y"
    { options.SetOverwrite(true); }
    break;

  case 159:
/* Line 670 of lalr1.cc  */
#line 280 "../../parser/cmdoptions.y"
    { options.SetOverwrite(true); }
    break;

  case 160:
/* Line 670 of lalr1.cc  */
#line 281 "../../parser/cmdoptions.y"
    { options.SetOverwrite(false); }
    break;

  case 162:
/* Line 670 of lalr1.cc  */
#line 285 "../../parser/cmdoptions.y"
    { options.SetLogLevel(LogLevel::ERROR); }
    break;

  case 163:
/* Line 670 of lalr1.cc  */
#line 286 "../../parser/cmdoptions.y"
    { options.SetLogLevel(LogLevel::WARNING); }
    break;

  case 164:
/* Line 670 of lalr1.cc  */
#line 287 "../../parser/cmdoptions.y"
    { options.SetLogLevel(LogLevel::INFO); }
    break;

  case 165:
/* Line 670 of lalr1.cc  */
#line 288 "../../parser/cmdoptions.y"
    { options.SetLogLevel(LogLevel::DEBUG); }
    break;

  case 166:
/* Line 670 of lalr1.cc  */
#line 289 "../../parser/cmdoptions.y"
    { options.SetLogLevel(LogLevel::TRACE); }
    break;

  case 167:
/* Line 670 of lalr1.cc  */
#line 292 "../../parser/cmdoptions.y"
    { options.SetEncryptedKeySource(KeySource::BbramRedKey); }
    break;

  case 168:
/* Line 670 of lalr1.cc  */
#line 293 "../../parser/cmdoptions.y"
    { options.SetEncryptedKeySource(KeySource::EfuseRedKey); }
    break;

  case 170:
/* Line 670 of lalr1.cc  */
#line 296 "../../parser/cmdoptions.y"
    { options.SetStartCbc((yysemantic_stack_[(3) - (3)].cstring));}
    break;

  case 172:
/* Line 670 of lalr1.cc  */
#line 297 "../../parser/cmdoptions.y"
    { options.SetKey0((yysemantic_stack_[(3) - (3)].cstring));}
    break;

  case 174:
/* Line 670 of lalr1.cc  */
#line 298 "../../parser/cmdoptions.y"
    { options.SetHmac((yysemantic_stack_[(3) - (3)].cstring));}
    break;

  case 176:
/* Line 670 of lalr1.cc  */
#line 299 "../../parser/cmdoptions.y"
    { options.SetEncryptionKeyFile((yysemantic_stack_[(1) - (1)].cstring));}
    break;

  case 178:
/* Line 670 of lalr1.cc  */
#line 303 "../../parser/cmdoptions.y"
    { options.SetProcessBitstreamType(File::MCS); }
    break;

  case 179:
/* Line 670 of lalr1.cc  */
#line 304 "../../parser/cmdoptions.y"
    { options.SetProcessBitstreamType(File::BIN); }
    break;

  case 180:
/* Line 670 of lalr1.cc  */
#line 307 "../../parser/cmdoptions.y"
    { options.SetDualQspiMode(QspiMode::PARALLEL_LQSPI); }
    break;

  case 181:
/* Line 670 of lalr1.cc  */
#line 308 "../../parser/cmdoptions.y"
    { options.SetDualQspiMode(QspiMode::STACKED); }
    break;

  case 182:
/* Line 670 of lalr1.cc  */
#line 311 "../../parser/cmdoptions.y"
    { LOG_ERROR("'parallel' option is not supported with '-dual_ospi_mode'."); }
    break;

  case 183:
/* Line 670 of lalr1.cc  */
#line 312 "../../parser/cmdoptions.y"
    { options.SetDualQspiMode(QspiMode::STACKED); }
    break;

  case 184:
/* Line 670 of lalr1.cc  */
#line 315 "../../parser/cmdoptions.y"
    { options.SetQspiSize((uint16_t)(yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 185:
/* Line 670 of lalr1.cc  */
#line 316 "../../parser/cmdoptions.y"
    { options.SetQspiSize((uint16_t)atoi((yysemantic_stack_[(1) - (1)].cstring))); }
    break;

  case 186:
/* Line 670 of lalr1.cc  */
#line 319 "../../parser/cmdoptions.y"
    { options.SetOspiSize((uint16_t)(yysemantic_stack_[(1) - (1)].number)); }
    break;

  case 187:
/* Line 670 of lalr1.cc  */
#line 320 "../../parser/cmdoptions.y"
    { options.SetOspiSize((uint16_t)atoi((yysemantic_stack_[(1) - (1)].cstring))); }
    break;

  case 188:
/* Line 670 of lalr1.cc  */
#line 323 "../../parser/cmdoptions.y"
    { options.SetArchType(Arch::ZYNQ); }
    break;

  case 189:
/* Line 670 of lalr1.cc  */
#line 324 "../../parser/cmdoptions.y"
    { options.SetArchType(Arch::ZYNQMP); }
    break;

  case 190:
/* Line 670 of lalr1.cc  */
#line 325 "../../parser/cmdoptions.y"
    { options.SetArchType(Arch::VERSAL); }
    break;

  case 191:
/* Line 670 of lalr1.cc  */
#line 326 "../../parser/cmdoptions.y"
    { options.SetArchType(Arch::FPGA); }
    break;

  case 192:
/* Line 670 of lalr1.cc  */
#line 327 "../../parser/cmdoptions.y"
    { options.SetArchType(Arch::VERSALNET); }
    break;

  case 193:
/* Line 670 of lalr1.cc  */
#line 328 "../../parser/cmdoptions.y"
    { options.SetArchType(Arch::VERSALGEN2); }
    break;

  case 194:
/* Line 670 of lalr1.cc  */
#line 329 "../../parser/cmdoptions.y"
    { options.SetArchType(Arch::SPARTANUPV2); }
    break;

  case 195:
/* Line 670 of lalr1.cc  */
#line 330 "../../parser/cmdoptions.y"
    { LOG_WARNING("usage of '-arch telluride' will be deprecated, Please use '-arch versal_2ve_2vm' instead.");
                                                      options.SetArchType(Arch::VERSALGEN2); }
    break;

  case 196:
/* Line 670 of lalr1.cc  */
#line 332 "../../parser/cmdoptions.y"
    { LOG_WARNING("usage of '-arch lassen' will be deprecated, Please use '-arch spartanup' instead."); 
                                                      options.SetArchType(Arch::SPARTANUP); }
    break;

  case 197:
/* Line 670 of lalr1.cc  */
#line 334 "../../parser/cmdoptions.y"
    { options.SetArchType(Arch::SPARTANUPV2); }
    break;

  case 199:
/* Line 670 of lalr1.cc  */
#line 338 "../../parser/cmdoptions.y"
    { options.SetGreyKeyGeneration(true); }
    break;

  case 200:
/* Line 670 of lalr1.cc  */
#line 339 "../../parser/cmdoptions.y"
    { options.SetMetalKeyGeneration(true); }
    break;

  case 201:
/* Line 670 of lalr1.cc  */
#line 340 "../../parser/cmdoptions.y"
    { options.SetAuthKeyGeneration(GenAuthKeys::PEM); }
    break;

  case 202:
/* Line 670 of lalr1.cc  */
#line 341 "../../parser/cmdoptions.y"
    { options.SetAuthKeyGeneration(GenAuthKeys::RSA); }
    break;

  case 203:
/* Line 670 of lalr1.cc  */
#line 342 "../../parser/cmdoptions.y"
    { options.SetAuthKeyGeneration(GenAuthKeys::ECDSA); }
    break;

  case 204:
/* Line 670 of lalr1.cc  */
#line 343 "../../parser/cmdoptions.y"
    { options.SetAuthKeyGeneration(GenAuthKeys::ECDSAP521); }
    break;

  case 205:
/* Line 670 of lalr1.cc  */
#line 344 "../../parser/cmdoptions.y"
    { options.SetAuthKeyGeneration(GenAuthKeys::LMS); }
    break;

  case 206:
/* Line 670 of lalr1.cc  */
#line 347 "../../parser/cmdoptions.y"
    { options.SetAuthKeyGeneration(GenAuthKeys::PEM); }
    break;

  case 207:
/* Line 670 of lalr1.cc  */
#line 348 "../../parser/cmdoptions.y"
    { options.SetAuthKeyGeneration(GenAuthKeys::RSA); }
    break;

  case 208:
/* Line 670 of lalr1.cc  */
#line 351 "../../parser/cmdoptions.y"
    { options.SetSecureDebugAuthType(Authentication::ECDSA);
                                                      options.SetSecureDebugImageFile("secureDebugImage-ecdsa.bin"); }
    break;

  case 209:
/* Line 670 of lalr1.cc  */
#line 353 "../../parser/cmdoptions.y"
    { options.SetSecureDebugAuthType(Authentication::RSA);
                                                      options.SetSecureDebugImageFile("secureDebugImage-rsa.bin"); }
    break;

  case 210:
/* Line 670 of lalr1.cc  */
#line 356 "../../parser/cmdoptions.y"
    { options.SetSecureDebugImageFile((yysemantic_stack_[(2) - (2)].cstring)); }
    break;

  case 212:
/* Line 670 of lalr1.cc  */
#line 359 "../../parser/cmdoptions.y"
    { options.SetSecureDebugAuthType(Authentication::ECDSA);
                                                      options.SetSecureDebugImageFile("authenticatedJtagImage-ecdsa.bin"); }
    break;

  case 213:
/* Line 670 of lalr1.cc  */
#line 361 "../../parser/cmdoptions.y"
    { options.SetSecureDebugAuthType(Authentication::RSA);
                                                      options.SetSecureDebugImageFile("authenticatedJtagImage-rsa.bin"); }
    break;

  case 214:
/* Line 670 of lalr1.cc  */
#line 365 "../../parser/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(1) - (1)].cstring));
                                                      options.SetVerifyImageOption(true); }
    break;

  case 215:
/* Line 670 of lalr1.cc  */
#line 368 "../../parser/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(1) - (1)].cstring));
                                                      options.SetReadImageOption(ReadImageOption::ALL); }
    break;

  case 216:
/* Line 670 of lalr1.cc  */
#line 370 "../../parser/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring)); }
    break;

  case 217:
/* Line 670 of lalr1.cc  */
#line 373 "../../parser/cmdoptions.y"
    { options.SetReadImageOption(ReadImageOption::BH); }
    break;

  case 218:
/* Line 670 of lalr1.cc  */
#line 374 "../../parser/cmdoptions.y"
    { options.SetReadImageOption(ReadImageOption::IHT); }
    break;

  case 219:
/* Line 670 of lalr1.cc  */
#line 375 "../../parser/cmdoptions.y"
    { options.SetReadImageOption(ReadImageOption::IH); }
    break;

  case 220:
/* Line 670 of lalr1.cc  */
#line 376 "../../parser/cmdoptions.y"
    { options.SetReadImageOption(ReadImageOption::PHT); }
    break;

  case 221:
/* Line 670 of lalr1.cc  */
#line 377 "../../parser/cmdoptions.y"
    { options.SetReadImageOption(ReadImageOption::AC); }
    break;

  case 222:
/* Line 670 of lalr1.cc  */
#line 380 "../../parser/cmdoptions.y"
    { options.SetDumpOption(DumpOption::BH); }
    break;

  case 223:
/* Line 670 of lalr1.cc  */
#line 381 "../../parser/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring));
                                                      options.SetDumpOption(DumpOption::BH); }
    break;

  case 224:
/* Line 670 of lalr1.cc  */
#line 383 "../../parser/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (2)].cstring));
                                                      options.SetDumpOption(DumpOption::BH); }
    break;

  case 225:
/* Line 670 of lalr1.cc  */
#line 385 "../../parser/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(1) - (1)].cstring));
                                                      options.SetDumpOption(DumpOption::PARTITIONS); }
    break;

  case 226:
/* Line 670 of lalr1.cc  */
#line 387 "../../parser/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring));
                                                      options.SetDumpOption(DumpOption::PLM); }
    break;

  case 227:
/* Line 670 of lalr1.cc  */
#line 389 "../../parser/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring));
                                                      options.SetDumpOption(DumpOption::PMC_CDO); }
    break;

  case 228:
/* Line 670 of lalr1.cc  */
#line 391 "../../parser/cmdoptions.y"
    { options.SetReadImageFile((yysemantic_stack_[(2) - (1)].cstring));
                                                      options.SetDumpOption(DumpOption::BOOT_FILES); }
    break;

  case 229:
/* Line 670 of lalr1.cc  */
#line 393 "../../parser/cmdoptions.y"
    { options.SetDumpOption(DumpOption::SLAVE_PDIS); }
    break;

  case 230:
/* Line 670 of lalr1.cc  */
#line 394 "../../parser/cmdoptions.y"
    { options.SetDumpOption(DumpOption::PUF_PDI); }
    break;

  case 231:
/* Line 670 of lalr1.cc  */
#line 397 "../../parser/cmdoptions.y"
    { options.SetEncryptionDump(true,"aes_log.txt"); }
    break;

  case 232:
/* Line 670 of lalr1.cc  */
#line 398 "../../parser/cmdoptions.y"
    { options.SetEncryptionDump(true,(yysemantic_stack_[(1) - (1)].cstring)); }
    break;


/* Line 670 of lalr1.cc  */
#line 1677 "cmdoptions.tab.cpp"
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
  const signed char BisonParser::yypact_ninf_ = -78;
  const short int
  BisonParser::yypact_[] =
  {
       -78,    20,   118,   -78,    92,   -53,    32,   120,   -78,   -78,
     -78,   -59,   -77,   141,     2,     8,    18,   121,    92,    92,
     211,   -78,    47,    93,    94,   -78,   -78,    92,    92,   -78,
      60,    92,   -58,    92,   -52,   -51,    89,   -50,    92,    92,
      92,   -78,    92,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,    92,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,    92,    17,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -70,   -78,   -78,   -69,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -47,   -78,   -78,   -78,   -78,   105,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,    92,
     -78,   -78,   -54,   -78,    57,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -44,    53,
     -78,   -78,   -78,   -78,   -78,   -78,    36,    64,    78,   -78,
      95,    97,    92,   -78,   101,   -78,   -78,   -78
  };

  /* YYDEFACT[S] -- default reduction number in state S.  Performed when
     YYTABLE doesn't specify something else to do.  Zero means the
     default is an error.  */
  const unsigned char
  BisonParser::yydefact_[] =
  {
         3,     0,     2,     1,     0,    53,     0,    57,    33,    20,
      19,     0,    89,   161,     0,     0,     0,     0,     0,     0,
       0,    30,     0,     0,   158,    31,    35,     0,     0,    18,
      21,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    44,   231,     4,     7,    48,    49,    50,    47,     5,
     177,    54,     0,     9,    58,    59,    61,    62,    64,    65,
      66,    67,    68,    69,    70,    71,    60,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    84,    83,    82,
      85,    86,    87,    88,    63,    13,    28,   133,    90,    91,
      92,    93,    94,    95,    96,    97,   130,    98,    99,   100,
     102,   101,   103,   106,   129,   131,   104,   105,   107,   108,
     109,   110,   111,   112,   113,   114,   122,   115,   116,   117,
     118,   119,   120,   121,   132,   123,   124,   125,   126,   128,
     127,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   149,   150,   151,   152,   148,
     153,   154,   155,   156,   157,    14,   162,   163,   164,   165,
     166,    12,    51,    52,     6,   178,   179,    24,    55,    56,
      46,   229,   230,   222,   225,    40,    41,    42,   188,   189,
     190,   191,   192,   195,   193,   196,   197,   194,    29,   180,
       0,    25,   182,     0,    26,   159,   160,    11,    45,    16,
      23,     0,    27,   201,   202,   204,     0,   199,   200,   205,
     203,    32,    17,   209,   208,    36,   213,   212,    37,   211,
     167,   168,   169,    10,   215,    38,   214,    39,    43,   232,
      34,     8,   224,   226,   227,   228,   223,   184,   185,   181,
     186,   187,   183,    22,   206,   207,   198,   210,     0,   171,
     217,   218,   219,   220,   221,   216,     0,     0,   173,   170,
       0,     0,   175,   172,     0,   176,    15,   174
  };

  /* YYPGOTO[NTERM-NUM].  */
  const signed char
  BisonParser::yypgoto_[] =
  {
       -78,   -78,   -78,   -78,    -6,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,   -78,
     -78,   -78,   -78,   -78
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  BisonParser::yydefgoto_[] =
  {
        -1,     1,     2,    43,    49,   164,    44,   170,    85,   155,
     197,   161,   222,   249,   258,   262,   266,    51,   167,   191,
     194,   239,   242,   188,   211,   246,   215,   218,   219,   227,
     225,   255,   175,   230
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If YYTABLE_NINF_, syntax error.  */
  const signed char BisonParser::yytable_ninf_ = -1;
  const unsigned short int
  BisonParser::yytable_[] =
  {
        53,   203,   204,   205,   206,   207,   208,   209,   213,   216,
     210,   174,   176,   177,   237,   240,   214,   217,   238,   241,
       3,   198,   199,   162,   163,   202,    86,   212,    87,   165,
     166,    50,   224,   226,   228,   223,   229,   248,    52,   168,
     169,   243,   233,   234,   235,   256,   231,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   236,   189,   190,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,   152,   153,   154,    45,    46,    47,
      48,     4,     5,     6,   259,     7,     8,     9,    10,    11,
      12,    13,   250,   251,   252,   253,   254,    14,    15,   192,
     193,    16,    17,   195,   196,   257,    18,    19,   200,   201,
      20,   171,   172,   260,    21,   156,   157,   158,   159,   160,
     220,   221,    22,    23,   244,   245,    24,   232,   261,    25,
      26,    27,    28,    29,    30,    31,    32,    45,    46,    47,
      48,     0,     0,   263,    33,    34,   264,    35,    36,   267,
       0,    37,    38,     0,     0,     0,   173,     0,    39,    40,
      41,    42,     0,     0,     0,     0,    45,    46,    47,    48,
       0,     0,     0,   247,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,   178,   179,   180,     0,   181,   182,
     183,   184,   185,   186,   187,     0,   265,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      84
  };

  /* YYCHECK.  */
  const short int
  BisonParser::yycheck_[] =
  {
         6,    59,    60,    61,    62,    63,    64,    65,    60,    60,
      68,    17,    18,    19,    84,    84,    68,    68,    88,    88,
       0,    27,    28,    21,    22,    31,    85,    33,   105,    21,
      22,    84,    38,    39,    40,    85,    42,    91,     6,    21,
      22,    88,    25,    26,    27,    89,    52,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,    75,    46,    47,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,    85,    86,    87,
      88,     3,     4,     5,    88,     7,     8,     9,    10,    11,
      12,    13,    75,    76,    77,    78,    79,    19,    20,    46,
      47,    23,    24,    49,    50,    92,    28,    29,    88,    89,
      32,    30,    31,    89,    36,    14,    15,    16,    17,    18,
      71,    72,    44,    45,    59,    60,    48,   173,    90,    51,
      52,    53,    54,    55,    56,    57,    58,    85,    86,    87,
      88,    -1,    -1,    88,    66,    67,    89,    69,    70,    88,
      -1,    73,    74,    -1,    -1,    -1,    75,    -1,    80,    81,
      82,    83,    -1,    -1,    -1,    -1,    85,    86,    87,    88,
      -1,    -1,    -1,   219,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,    33,    34,    35,    -1,    37,    38,
      39,    40,    41,    42,    43,    -1,   262,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     160
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  BisonParser::yystos_[] =
  {
         0,   195,   196,     0,     3,     4,     5,     7,     8,     9,
      10,    11,    12,    13,    19,    20,    23,    24,    28,    29,
      32,    36,    44,    45,    48,    51,    52,    53,    54,    55,
      56,    57,    58,    66,    67,    69,    70,    73,    74,    80,
      81,    82,    83,   197,   200,    85,    86,    87,    88,   198,
      84,   211,     6,   198,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   160,   202,    85,   105,   124,   125,
     126,   127,   128,   129,   130,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   203,    14,    15,    16,    17,
      18,   205,    21,    22,   199,    21,    22,   212,    21,    22,
     201,    30,    31,    75,   198,   226,   198,   198,    33,    34,
      35,    37,    38,    39,    40,    41,    42,    43,   217,    46,
      47,   213,    46,    47,   214,    49,    50,   204,   198,   198,
      88,    89,   198,    59,    60,    61,    62,    63,    64,    65,
      68,   218,   198,    60,    68,   220,    60,    68,   221,   222,
      71,    72,   206,    85,   198,   224,   198,   223,   198,   198,
     227,   198,   198,    25,    26,    27,    75,    84,    88,   215,
      84,    88,   216,    88,    59,    60,   219,   198,    91,   207,
      75,    76,    77,    78,    79,   225,    89,    92,   208,    88,
      89,    90,   209,    88,    89,   198,   210,    88
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
     425,   426,   427,   428,   429,   430,   431,   432,   433,   434,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   444,
     445,   446,   447,   448
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  BisonParser::yyr1_[] =
  {
         0,   194,   195,   196,   196,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   197,   197,   197,
     197,   197,   197,   197,   197,   197,   197,   198,   198,   198,
     198,   199,   199,   200,   200,   201,   201,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   202,
     202,   202,   202,   202,   202,   202,   202,   202,   202,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   203,   203,
     203,   203,   203,   203,   203,   203,   203,   203,   204,   204,
     204,   205,   205,   205,   205,   205,   205,   206,   206,   207,
     207,   208,   208,   209,   209,   210,   210,   211,   212,   212,
     213,   213,   214,   214,   215,   215,   216,   216,   217,   217,
     217,   217,   217,   217,   217,   217,   217,   217,   218,   218,
     218,   218,   218,   218,   218,   218,   219,   219,   220,   220,
     221,   221,   222,   222,   223,   224,   224,   225,   225,   225,
     225,   225,   226,   226,   226,   226,   226,   226,   226,   226,
     226,   227,   227
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  BisonParser::yyr2_[] =
  {
         0,     2,     1,     0,     2,     2,     2,     1,     3,     2,
       2,     2,     2,     2,     2,     6,     2,     2,     1,     1,
       1,     1,     3,     2,     2,     2,     2,     2,     2,     2,
       1,     1,     2,     1,     2,     1,     2,     2,     2,     2,
       2,     2,     2,     2,     1,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     0,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     1,
       1,     0,     1,     1,     1,     1,     1,     1,     1,     0,
       3,     0,     3,     0,     3,     0,     1,     1,     1,     1,
       1,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     2,     1,     1,     1,
       1,     1,     1,     2,     2,     1,     2,     2,     2,     1,
       1,     0,     1
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
  "_PROCESS_BITSTREAM", "MCS", "BIN", "_OUT_TYPE", "_DUMP", "DUMP_PLM",
  "DUMP_PMC_CDO", "DUMP_BOOT_FILES", "_DUMP_DIR", "_PUF",
  "DUMP_SLAVE_PDIS", "DUMP_PUF_PDI", "_ARCH", "ZYNQ", "ZYNQMP", "VERSAL",
  "_R", "FPGA", "VERSALNET", "TELLURIDE", "VERSAL_2VE_2VM", "LASSEN",
  "LASSEN_DL9", "SPARTANUP", "_DUAL_QSPI_MODE", "_DUAL_OSPI_MODE",
  "PARALLEL", "STACKED", "_W", "ON", "OFF", "_NOAUTHBLOCKS", "_ZYNQMPES1",
  "_OVERLAYCDO", "_EFUSEPPKBITS", "_GENERATE_HASHES", "_PADIMAGEHEADER",
  "_SPKSIGNATURE", "_GENERATE_KEYS", "PEM", "RSA", "ECDSAP521", "AUTH",
  "GREY", "METAL", "LMS", "_EFUSEPUFBITS", "_SECUREDEBUG", "ECDSA",
  "_AUTHJTAG", "_ENCRYPT", "BBRAM", "EFUSE", "_P_TOK", "_READ", "READ_BH",
  "READ_IHT", "READ_IH", "READ_PHT", "READ_AC", "_VERIFY", "_VERIFYKDF",
  "_AUTH_OPTIMIZATION", "_ZYNQMPENCRDUMP", "HEXVALUE", "IDENTIFIER",
  "FILENAME", "QFILENAME", "HEXSTRING", "EQUALS", "HMAC", "STARTCBC",
  "KEY0", "COMMA", "HBIFHELP", "HARCH", "HIMAGE", "HFILL", "HO", "HP",
  "HW", "HEFUSEPPKBITS", "HGENHASHES", "HLEGACY", "HPADHDR", "H_SPKSIGN",
  "HAUTHOPT", "HPACKAGE", "HENCRYPT", "HGENKEYS", "HDQSPI", "HLOG",
  "HZYNQMPES1", "HPROCESSBIT", "HNONBOOTING", "HENCRDUMP", "HVERIFY",
  "HSECUREDEBUG", "HREAD", "HVERIFYKDF", "HDUMP", "HDUMPDIR", "HOVLCDO",
  "HOUTTYPE", "H_BIF_INIT", "H_BIF_UDFBH", "H_BIF_AES", "H_BIF_PPK",
  "H_BIF_PSK", "H_BIF_SPK", "H_BIF_SSK", "H_BIF_SPKSIGN", "H_BIF_HIVEC",
  "H_BIF_HDRSIGN", "H_BIF_BOOTIMAGE", "H_BIF_BL", "H_BIF_PID",
  "H_BIF_ENCR", "H_BIF_AUTH", "H_BIF_CHKSM", "H_BIF_ELYHNDOFF",
  "H_BIF_BHSIGN", "H_BIF_TCMBOOT", "H_BIF_OPTIONALDATA", "H_BIF_POWNER",
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
  "H_BIF_PARENTID", "H_DPACM_ENABLE", "H_BIF_USERKEYS", "HVN_BIF_PCR",
  "HVN_BIF_PCR_MINDEX", "HV_BIF_IMAGESTORE", "$accept", "top",
  "option_list", "option", "filename", "memsplit", "filloption",
  "outputType", "helpoption", "bifhelpoption", "wopt", "loglevel",
  "keysource", "startcbc", "key0", "hmac", "keyfile", "number",
  "bitfile_type", "qpsi_mode", "opsi_mode", "qspi_size", "ospi_size",
  "archOptions", "key_type", "auth_key_options", "auth_type",
  "authJtagOptions", "authJtagType", "verifyImageOptions",
  "readImageOptions", "readType", "dumpOptions", "encrDumpOptions", YY_NULL
  };


  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const BisonParser::rhs_number_type
  BisonParser::yyrhs_[] =
  {
       195,     0,    -1,   196,    -1,    -1,   196,   197,    -1,     3,
     198,    -1,    19,   199,    -1,   200,    -1,     5,     6,   198,
      -1,     5,   198,    -1,    73,    85,    -1,    48,   204,    -1,
      13,   205,    -1,     7,   202,    -1,    12,   203,    -1,    70,
     206,   207,   208,   209,   210,    -1,    54,   198,    -1,    66,
     198,    -1,    55,    -1,    10,    -1,     9,    -1,    56,    -1,
      56,    89,    88,    -1,    56,    88,    -1,    20,   212,    -1,
      44,   213,    -1,    45,   214,    -1,    57,   198,    -1,    11,
      85,    -1,    32,   217,    -1,    36,    -1,    51,    -1,    58,
     218,    -1,     8,    -1,    83,   227,    -1,    52,    -1,    67,
     220,    -1,    69,   221,    -1,    74,   224,    -1,    80,   223,
      -1,    24,   226,    -1,    28,   198,    -1,    29,   198,    -1,
      81,   198,    -1,    82,    -1,    53,   198,    -1,    23,   201,
      -1,    88,    -1,    85,    -1,    86,    -1,    87,    -1,    21,
      -1,    22,    -1,     4,    -1,     4,   211,    -1,    21,    -1,
      22,    -1,    -1,    94,    -1,    95,    -1,   106,    -1,    96,
      -1,    97,    -1,   160,    -1,    98,    -1,    99,    -1,   100,
      -1,   101,    -1,   102,    -1,   103,    -1,   104,    -1,   105,
      -1,   107,    -1,   108,    -1,   109,    -1,   110,    -1,   111,
      -1,   112,    -1,   113,    -1,   114,    -1,   115,    -1,   116,
      -1,   119,    -1,   118,    -1,   117,    -1,   120,    -1,   121,
      -1,   122,    -1,   123,    -1,    -1,   124,    -1,   125,    -1,
     126,    -1,   127,    -1,   128,    -1,   129,    -1,   130,    -1,
     131,    -1,   133,    -1,   134,    -1,   135,    -1,   137,    -1,
     136,    -1,   138,    -1,   142,    -1,   143,    -1,   139,    -1,
     144,    -1,   145,    -1,   146,    -1,   147,    -1,   148,    -1,
     149,    -1,   150,    -1,   151,    -1,   153,    -1,   154,    -1,
     155,    -1,   156,    -1,   157,    -1,   158,    -1,   159,    -1,
     152,    -1,   161,    -1,   162,    -1,   163,    -1,   164,    -1,
     166,    -1,   165,    -1,   140,    -1,   132,    -1,   141,    -1,
     160,    -1,   105,    -1,   167,    -1,   168,    -1,   172,    -1,
     173,    -1,   174,    -1,   175,    -1,   176,    -1,   177,    -1,
     178,    -1,   179,    -1,   180,    -1,   181,    -1,   182,    -1,
     183,    -1,   188,    -1,   184,    -1,   185,    -1,   186,    -1,
     187,    -1,   189,    -1,   190,    -1,   191,    -1,   192,    -1,
     193,    -1,    -1,    49,    -1,    50,    -1,    -1,    14,    -1,
      15,    -1,    16,    -1,    17,    -1,    18,    -1,    71,    -1,
      72,    -1,    -1,    91,    89,    88,    -1,    -1,    92,    89,
      88,    -1,    -1,    90,    89,    88,    -1,    -1,   198,    -1,
      84,    -1,    21,    -1,    22,    -1,    46,    -1,    47,   215,
      -1,    46,    -1,    47,   216,    -1,    84,    -1,    88,    -1,
      84,    -1,    88,    -1,    33,    -1,    34,    -1,    35,    -1,
      37,    -1,    38,    -1,    40,    -1,    43,    -1,    39,    -1,
      41,    -1,    42,    -1,    62,   219,    -1,    63,    -1,    64,
      -1,    59,    -1,    60,    -1,    68,    -1,    61,    -1,    65,
      -1,    59,    -1,    60,    -1,    68,    -1,    60,    -1,   222,
     198,    -1,   222,    -1,    68,    -1,    60,    -1,   198,    -1,
     198,    -1,   198,   225,    -1,    75,    -1,    76,    -1,    77,
      -1,    78,    -1,    79,    -1,    75,    -1,   198,    75,    -1,
      75,   198,    -1,   198,    -1,   198,    25,    -1,   198,    26,
      -1,   198,    27,    -1,    30,    -1,    31,    -1,    -1,   198,
      -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  BisonParser::yyprhs_[] =
  {
         0,     0,     3,     5,     6,     9,    12,    15,    17,    21,
      24,    27,    30,    33,    36,    39,    46,    49,    52,    54,
      56,    58,    60,    64,    67,    70,    73,    76,    79,    82,
      85,    87,    89,    92,    94,    97,    99,   102,   105,   108,
     111,   114,   117,   120,   123,   125,   128,   131,   133,   135,
     137,   139,   141,   143,   145,   148,   150,   152,   153,   155,
     157,   159,   161,   163,   165,   167,   169,   171,   173,   175,
     177,   179,   181,   183,   185,   187,   189,   191,   193,   195,
     197,   199,   201,   203,   205,   207,   209,   211,   213,   215,
     216,   218,   220,   222,   224,   226,   228,   230,   232,   234,
     236,   238,   240,   242,   244,   246,   248,   250,   252,   254,
     256,   258,   260,   262,   264,   266,   268,   270,   272,   274,
     276,   278,   280,   282,   284,   286,   288,   290,   292,   294,
     296,   298,   300,   302,   304,   306,   308,   310,   312,   314,
     316,   318,   320,   322,   324,   326,   328,   330,   332,   334,
     336,   338,   340,   342,   344,   346,   348,   350,   352,   353,
     355,   357,   358,   360,   362,   364,   366,   368,   370,   372,
     373,   377,   378,   382,   383,   387,   388,   390,   392,   394,
     396,   398,   401,   403,   406,   408,   410,   412,   414,   416,
     418,   420,   422,   424,   426,   428,   430,   432,   434,   437,
     439,   441,   443,   445,   447,   449,   451,   453,   455,   457,
     459,   462,   464,   466,   468,   470,   472,   475,   477,   479,
     481,   483,   485,   487,   490,   493,   495,   498,   501,   504,
     506,   508,   509
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  BisonParser::yyrline_[] =
  {
         0,   106,   106,   108,   109,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   135,   136,
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   156,   156,   156,
     156,   158,   159,   162,   163,   170,   171,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   242,   243,   244,   245,   246,   247,   248,
     249,   250,   251,   252,   253,   254,   255,   256,   257,   258,
     259,   260,   261,   262,   263,   264,   265,   266,   267,   268,
     269,   270,   271,   272,   273,   274,   275,   276,   279,   280,
     281,   284,   285,   286,   287,   288,   289,   292,   293,   296,
     296,   297,   297,   298,   298,   299,   299,   301,   303,   304,
     307,   308,   311,   312,   315,   316,   319,   320,   323,   324,
     325,   326,   327,   328,   329,   330,   332,   334,   337,   338,
     339,   340,   341,   342,   343,   344,   347,   348,   351,   353,
     356,   357,   359,   361,   365,   368,   370,   373,   374,   375,
     376,   377,   380,   381,   383,   385,   387,   389,   391,   393,
     394,   397,   398
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
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int BisonParser::yyeof_ = 0;
  const int BisonParser::yylast_ = 280;
  const int BisonParser::yynnts_ = 34;
  const int BisonParser::yyempty_ = -2;
  const int BisonParser::yyfinal_ = 3;
  const int BisonParser::yyterror_ = 1;
  const int BisonParser::yyerrcode_ = 256;
  const int BisonParser::yyntokens_ = 194;

  const unsigned int BisonParser::yyuser_token_number_max_ = 448;
  const BisonParser::token_number_type BisonParser::yyundef_token_ = 2;

/* Line 1141 of lalr1.cc  */
#line 24 "../../parser/cmdoptions.y"
} // CO
/* Line 1141 of lalr1.cc  */
#line 2451 "cmdoptions.tab.cpp"
/* Line 1142 of lalr1.cc  */
#line 400 "../../parser/cmdoptions.y"



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

    case CO::BisonParser::token::HAUTHOPT:
        std::cout << ENABLEAUTHOPTHELP << std::endl;
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

    case CO::BisonParser::token::HOVLCDO:
        std::cout << OVERLAYCDO << std::endl;
        break;

    case CO::BisonParser::token::HOUTTYPE:
        std::cout << OUTTYPEHELP << std::endl;
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

    case CO::BisonParser::token::H_BIF_TCMBOOT:
        std::cout << H_BIF_TCMBOOT_H << std::endl;
        break;

    case CO::BisonParser::token::H_BIF_OPTIONALDATA:
        std::cout << H_BIF_OPTIONALDATA_H << std::endl;
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

    case CO::BisonParser::token::H_BIF_USERKEYS:
        std::cout << H_BIF_USERKEYS_H << std::endl;
        break;

    case CO::BisonParser::token::HVN_BIF_PCR_MINDEX:
        std::cout << HVN_BIF_PCR_MINDEX_H << std::endl;
        break;

    case CO::BisonParser::token::HVN_BIF_PCR:
        std::cout << HVN_BIF_PCR_H << std::endl;
        break;

    case CO::BisonParser::token::HV_BIF_IMAGESTORE:
        std::cout << HV_BIF_IMAGESTORE_H << std::endl;
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
