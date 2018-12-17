#pragma once
#include "Types.h"
#include "Source.h"

class Interpreter
{
public:
	struct Context
	{
		std::map<std::string, Value> variables;
		Value returnValue;
		Location returnLoc;
	};

	Interpreter(Source &source, Class *globalScope) : m_source(source), m_globalScope(globalScope) { }
	~Interpreter() { }

	Value Run();

	Value Eval(Context &ctx, const Expression &);
	Value Eval(Context &ctx, const Function &, const std::vector<Expression *> &arguments);
	BreakType Eval(Context &ctx, const Statement &);
	BreakType Eval(Context &ctx, const AssignmentStatement &);
	BreakType Eval(Context &ctx, const ReadStatement &);
	BreakType Eval(Context &ctx, const WriteStatement &);
	BreakType Eval(Context &ctx, const ReturnStatement &);
	BreakType Eval(Context &ctx, const BreakStatement &);
	BreakType Eval(Context &ctx, const IfStatement &);
	BreakType Eval(Context &ctx, const WhileStatement &);
	BreakType Eval(Context &ctx, const ExprStatement &);

private:
	Source &m_source;
	Class *m_globalScope;
};

