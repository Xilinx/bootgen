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

#ifndef _CMDOPTIONSSCANNER_H_
#define _CMDOPTIONSSCANNER_H_

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
#define YY_DECL int CO::FlexScanner::yylex()

// Include Bison for types / tokens
#include "cmdoptions.tab.hpp"
#include "options.h"
#include "parsing.h"


/*
-------------------------------------------------------------------------------
*********************************************************   C L A S S E S   ***
-------------------------------------------------------------------------------
*/

/******************************************************************************/
namespace CO 
{
    class FlexScanner : public yyFlexLexer 
    {
      public:
        // save the pointer to yylval so we can change it, and invoke scanner
        int yylex(CO::BisonParser::semantic_type * lval, CO::BisonParser::location_type* loc) 
        { 
            yylval = lval; 
            yylloc = loc;
            return yylex(); 
        }
    
        void comment();
        std::string commandline;
        void ReportError(const CO::BisonParser::location_type &loc, const std::string &msg) 
        {
            std::cerr << "ERROR: " << msg << "\n";
            Parsing::Error2(loc,commandline);
        }

      private:
        // Scanning function created by Flex; make this private to force usage
        // of the overloaded method so we can get a pointer to Bison's yylval
        int yylex();
    
        // point to yylval (provided by Bison in overloaded yylex)
        CO::BisonParser::semantic_type * yylval;
        CO::BisonParser::location_type * yylloc;
    };
}
#endif
