#pragma once
#include "Types.h"

class PseudoCodeGen
{
public:
	PseudoCodeGen();
	~PseudoCodeGen();

	void print(std::ostream &stream, const AssignmentStatement &);
	void print(std::ostream &stream, const ReadStatement &);
	void print(std::ostream &stream, const WriteStatement &);
	void print(std::ostream &stream, const ReturnStatement &);
	void print(std::ostream &stream, const BreakStatement &);
	void print(std::ostream &stream, const IfStatement &);
	void print(std::ostream &stream, const WhileStatement &);
	void print(std::ostream &stream, const ExprStatement &);
	void print(std::ostream &stream, const Statement &);


	void print(std::ostream &stream, const Expression &);
	void print(std::ostream &stream, const Type &);
	void print(std::ostream &stream, const Declaration &);
	void print(std::ostream &stream, const Function &);
	void print(std::ostream &stream, const Class &);

	std::string tabs();

private:
	size_t m_ident = 0;
};

