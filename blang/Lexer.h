#pragma once
#include "Types.h"

class Lexer : public yyFlexLexer
{
public:

	using yyFlexLexer::yyFlexLexer;

	const Location &GetLocation() const { return m_loc; }

	int yylex() override;

private:

	Location m_loc;

	void Token(const char *str)
	{
		m_loc.first_line = m_loc.last_line;
		m_loc.first_column = m_loc.last_column;
		m_loc.first_pos = m_loc.last_pos;

		for (size_t i = 0; i < strlen(str); i++)
		{
			m_loc.last_pos++;
			m_loc.last_column++;
			if (str[i] == '\n')
			{
				m_loc.last_column = 0;
				m_loc.last_line++;
			}
		}
	}
};
