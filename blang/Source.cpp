#include "pch.h"
#include "Source.h"


Source::Source(const std::string &filename) : m_filename(filename)
{
	m_stream.open(filename);
	m_errorStream.open(filename);

	if (!m_stream.is_open() || !m_errorStream.is_open())
	{
		throw std::runtime_error("Failed to open file");
	}
}


Source::~Source()
{
}

void Source::Error(const Location & loc, const std::string & message)
{
	/*std::cout << loc.first_line << ":" << loc.first_column << "  --  " << loc.last_line << ":" << loc.last_column << std::endl;
	std::cout << loc.first_pos << "  --  " << loc.last_pos << std::endl;*/

	std::cout << m_filename << ":" << loc.first_line << ":" << loc.first_column << ": " << message << std::endl;
	m_errorStream.seekg(0, m_errorStream.beg);
	std::string line;//(int(d_loc__.last_pos - d_loc__.first_pos), 'x');

	std::getline(m_errorStream, line);
	for (int i = 0; i < loc.first_line; i++)
		std::getline(m_errorStream, line);

	std::cout << line << std::endl;

	for (int i = 0; i < loc.first_column; i++)
		std::cout << ' ';

	int last_column = loc.first_line == loc.last_line ? loc.last_column : line.length();
	for (int i = loc.first_column; i < last_column; i++)
		std::cout << '^';

	std::cout << std::endl;
}