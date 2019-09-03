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
/* This is the lexer/flexer input file, that tokenizes the register initialization file */
/* Run the regedit.y file first through bison to generate the regidit.tab.hpp file, which is included herein */
/* Then run this file through flex to generate the reginit.yy.c source code */


%{
 #include <stdio.h>
 #include "reginitscanner.h"
 #include "reginit.tab.hpp"
 typedef RE::BisonParser::token tok;
 #include <iostream>
%}

%option nodefault yyclass="FlexScanner" noyywrap c++ noyywrap nounput batch debug

/*
 \.bitset\.    return tok::BITSET;
 \.bitclear\.  return tok::BITCLEAR;
 \.boot\.      return tok::BOOT;
 \.user\.      return tok::USER;
 */
 

%{
     # define YY_USER_ACTION  yylloc->columns (yyleng);
%}

%%

%{
   yylloc->step ();
%}

"/*"			        { comment(); }
\/\/.*?\n                /* ignore */;


[0-9]+                  yylval->number=atoi(yytext); return tok::DECVALUE;
0[xX][0-9a-fA-F]+       yylval->number=strtoul(yytext+2,NULL,16); return tok::HEXVALUE;

\.set\.	      return tok::SET;
\.or\.        return tok::OR;
\.xor\.       return tok::XOR;
\.and\.       return tok::AND;
	
\+                      return tok::PLUS;
\-                      return tok::MINUS;
\<\<                    return tok::LSHIFT;
\>\>                    return tok::RSHIFT;
\*                      return tok::MULT;
\/                      return tok::DIVIDE;
\%                      return tok::MODULO;
\~                      return tok::NEGATION;
\&                      return tok::AND;
\|                      return tok::OR;
\^                      return tok::XOR;
\(                      return tok::LPAREN;
\)                      return tok::RPAREN;
\=                      return tok::EQUAL;
;                       return tok::SEMICOLON;


[ \t]+                  yylloc->step ();
[\n]+                   yylloc->lines (yyleng); yylloc->step ();
.                       std::cerr << "WARNING: bad character '" << yytext << " at " << *yylloc << std::endl;
%%


void RE::FlexScanner::comment()
{
	char c, c1;
loop:
	while ((c = yyinput()) != '*' && c != 0) {
		// putchar(c);
	}

	if ((c1 = yyinput()) != '/' && c != 0)
	{
		unput(c1);
		goto loop;
	}
}
