#pragma once
#include "Types.h"

class Source
{
public:
	Source(const std::string &filename);
	~Source();

	std::ifstream &Stream() { return m_stream; }
	void Error(const Location &loc, const std::string &message);

private:
	std::string m_filename;
	std::ifstream m_stream;
	std::ifstream m_errorStream;
};

