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

#pragma once

#ifndef _REGINITSCANNER_H_
#define _REGINITSCANNER_H_

/*
-------------------------------------------------------------------------------
***********************************************   H E A D E R   F I L E S   ***
-------------------------------------------------------------------------------
*/
// Only include FlexLexer.h if it hasn't been already included
#if ! defined(yyFlexLexerOnce)

#undef yyFlexLexer
#define yyFlexLexer reginitFlexLexer
#include <FlexLexer.h>
#endif

// Override the interface for yylex since we namespaced it
#undef YY_DECL
#define YY_DECL int RE::FlexScanner::yylex()

// Include Bison for types / tokens
#include "reginit.tab.hpp"


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/
/******************************************************************************/
namespace RE 
{
    class FlexScanner : public yyFlexLexer 
    {
        public:
            // save the pointer to yylval so we can change it, and invoke scanner
            int yylex(RE::BisonParser::semantic_type * lval, RE::BisonParser::location_type* loc) { 
                yylval = lval; 
                yylloc = loc;
                return yylex(); 
            }
            void comment();
            std::string filename;

        private:
            // Scanning function created by Flex; make this private to force usage
            // of the overloaded method so we can get a pointer to Bison's yylval
            int yylex();
            // point to yylval (provided by Bison in overloaded yylex)
            RE::BisonParser::semantic_type * yylval;
            RE::BisonParser::location_type * yylloc;
    };
}
#endif
