/******************************************************************************
* Copyright 2015-2019 Xilinx, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/
/* register init grammar
 * Use bison to process this file into reginit.tab.cpp
 */

%require "2.4.1"
%skeleton "lalr1.cc"
%defines
%define namespace "RE"
%define parser_class_name "BisonParser"
%parse-param { RE::FlexScanner& scanner }
%parse-param { Options& options}
%parse-param { RegisterTable& regtab}
%lex-param   { RE::FlexScanner& scanner }
%locations
%initial-action
{
       // Initialize the initial location.
       @$.begin.filename = @$.end.filename = &scanner.filename;
};
  
%define api.location.type "BIF::location"
 
%code requires {
	namespace RE {
		class BisonScanner;
		class FlexScanner;
		class Parser;
	}
	#include "bootimage.h"
    #include "location.hh"
}

%code {
    static int yylex(RE::BisonParser::semantic_type * yylval, RE::BisonParser::location_type* loc, RE::FlexScanner &scanner);
}

%{
#include <stdio.h>
#include "reginitscanner.h"
#include "options.h"
#include "parsing.h"
%}


%start top 

%union 
{
    unsigned long number;
}

%token SET OR XOR AND MULT DIVIDE MODULO PLUS MINUS LSHIFT RSHIFT
%left  SET OR XOR AND MULT DIVIDE MODULO PLUS MINUS LSHIFT RSHIFT

%token  NEGATION
%right  NEGATION

%token LPAREN RPAREN EQUAL SEMICOLON
%right LPAREN
%left  RPAREN
%left  EQUAL



%token <number> DECVALUE HEXVALUE
%type<number>   expression number  multiplicative_expression unary_expression additive_expression shift_expression
%type<number> and_expression xor_expression


%%
top             : statements ;

statements      : /* empty */ | statements statement ;

statement       : SET expression EQUAL expression SEMICOLON { regtab.Add(options,$2,$4); } ;

number   : HEXVALUE | 
           DECVALUE |
           LPAREN expression RPAREN { $$ = $2; };

unary_expression
	: number
	| PLUS     unary_expression  {$$ =  $2;     *options.debugstr << $$ << "    + " << $2 << std::endl;}
	| NEGATION unary_expression  {$$ = ~$2;     *options.debugstr << $$ << "    ~ " << $2 << std::endl;}
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression MULT   unary_expression {$$ = $1 *  $3; *options.debugstr << $$ << " = " << $1  << " + " << $3 << std::endl;}
	| multiplicative_expression DIVIDE unary_expression {$$ = $1 /  $3; *options.debugstr << $$ << " = " << $1  << " / " << $3 << std::endl;}
	| multiplicative_expression MODULO unary_expression {$$ = $1 %  $3; *options.debugstr << $$ << " = " << $1  << " % " << $3 << std::endl;}
	;

additive_expression
	: multiplicative_expression
	| additive_expression  PLUS multiplicative_expression {$$ = $1 +  $3;*options.debugstr << $$ << " = " << $1  << " + " << $3 << std::endl;}
	| additive_expression MINUS multiplicative_expression {$$ = $1 -  $3;*options.debugstr << $$ << " = " << $1  << " - " << $3 << std::endl;}
	;

shift_expression
	: additive_expression
	| shift_expression LSHIFT additive_expression {$$ = $1 << $3;*options.debugstr << $$ << " = " << $1  << " << " << $3 << std::endl;}
	| shift_expression RSHIFT additive_expression {$$ = $1 >> $3;*options.debugstr << $$ << " = " << $1  << " >> " << $3 << std::endl;}
	;
	   
and_expression
	: shift_expression
	| and_expression AND shift_expression  {$$ = $1 &  $3;*options.debugstr << $$ << " = " << $1  << " & " << $3 << std::endl;}
	;

xor_expression
	: and_expression
	| xor_expression XOR and_expression  {$$ = $1 ^  $3;*options.debugstr << $$ << " = " << $1  << " ^ " << $3 << std::endl;}
	;

expression
	: xor_expression
	| expression OR xor_expression	     {$$ = $1 |  $3;*options.debugstr << $$ << " = " << $1  << " | " << $3 << std::endl;}                 
  ;                 
                  

%%


void RE::BisonParser::error(const RE::BisonParser::location_type &loc, const std::string &msg) {
	Parsing::Error(loc,msg);
}

static int yylex(RE::BisonParser::semantic_type * yylval, RE::BisonParser::location_type* loc, RE::FlexScanner &scanner) {
    return scanner.yylex(yylval, loc);
}
