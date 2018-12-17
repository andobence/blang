#include "pch.h"
#include "Parser.h"

void Parser::error(char const *msg)
{
	m_source.Error(d_loc__, "Syntax error");
	exit(1);
}

int Parser::lex()
{
	int ret = lexer.yylex();
	d_loc__ = lexer.GetLocation();

	/*std::cout << "TOKEN: " << lexer.YYText() << std::endl;
	std::cout << d_loc__.first_line << ":" << d_loc__.first_column << "  --  " << d_loc__.last_line << ":" << d_loc__.last_column << std::endl;
	std::cout << d_loc__.first_pos << "  --  " << d_loc__.last_pos << std::endl;*/
	if (ret == NUMBER)
	{
		d_val__.intvalue = atoi(lexer.YYText());
	}
	else if (ret == IDENTIFIER)
	{
		size_t len = std::strlen(lexer.YYText());
		auto id = new char[len + 1];
		std::copy(lexer.YYText(), lexer.YYText() + len + 1, id);
		d_val__.stringvalue = id;
	}
	else if (ret == UNSIGNED)
	{
		d_val__.type = Type::UNSIGNED;
	}
	else if (ret == BOOL)
	{
		d_val__.type = Type::BOOL;
	}
	return ret;
}

void Parser::print()
{
	print__(); // displays tokens if --print was specified
}

void Parser::exceptionHandler__(std::exception const &exc)
{
	throw; // re-implement to handle exceptions thrown by actions
}
