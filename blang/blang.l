%option noyywrap c++ yylineno

%{
#include "pch.h"
#include "Lexer.h"
#include "Parserbase.h"

std::map<std::string, int> g_keywords = {
	{ "class",		Parser::CLASS },
	{ "unsigned",	Parser::UNSIGNED },
	{ "bool",		Parser::BOOL },
	{ "true",		Parser::TRUE },
	{ "false",		Parser::FALSE },
	{ "if",			Parser::IF },
	{ "else",		Parser::ELSE },
	{ "while",		Parser::WHILE },
	{ "cout",		Parser::COUT },
	{ "cin",		Parser::CIN },
	{ "return",		Parser::RETURN },
	{ "break",		Parser::BREAK },
};

#define YY_USER_ACTION Token(YYText());
		
%}

DIGIT   [0-9]
ALPHA	[A-Za-z]
WS  [ \t\r\n]

%%

{DIGIT}+    return Parser::NUMBER;
{ALPHA}({DIGIT}|{ALPHA}|_)* {
				auto it = g_keywords.find(YYText());
				if (it != g_keywords.end())
					return it->second;
				else
					return Parser::IDENTIFIER;
			}

"+"         return Parser::PLUS;
"-"         return Parser::MINUS;
"*"         return Parser::MUL;
"/"         return Parser::DIV;
"%"         return Parser::MOD;

"("         return Parser::OPEN;
")"         return Parser::CLOSE;
"="         return Parser::ASSIGN;

"=="        return Parser::EQUAL;
"!="		return Parser::NOT_EQUAL;
"<"         return Parser::LESS;
">"         return Parser::GREATER;
"<="        return Parser::LESS_EQUAL;
">="        return Parser::GREATER_EQUAL;

"&&"        return Parser::AND;
"||"        return Parser::OR;
"!"         return Parser::NEGATE;

">>"        return Parser::READ;
"<<"        return Parser::WRITE;

"{"        return Parser::BOPEN;
"}"        return Parser::BCLOSE;
";"			return Parser::SEMICOLON;
","			return Parser::COMMA;

"//".*      // comment

{WS}+       // feher szokozok: semmi teendo

.           {
                std::cerr << lineno() << ": Lexikalis hiba." << std::endl;
                exit(1);
            }

%%
