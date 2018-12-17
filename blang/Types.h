#pragma once

#include <string>
#include <variant>
#include <map>
#include <cstring>

struct Location
{
	int timestamp = 0;
	int first_line = 0;
	int first_column = 0;
	int last_line = 0;
	int last_column = 0;
	int first_pos = 0;
	int last_pos = 0;
	char *text = nullptr;

	static Location Between(const Location &lhs, const Location &rhs)
	{
		Location loc;
		if (lhs.first_pos < rhs.first_pos)
		{
			loc = rhs;
			loc.first_line = lhs.first_line;
			loc.first_column = lhs.first_column;
			loc.first_pos = lhs.first_pos;
		}
		else
		{
			loc = lhs;
			loc.first_line = rhs.first_line;
			loc.first_column = rhs.first_column;
			loc.first_pos = rhs.first_pos;
		}

		return loc;
	}
};

enum class ExprType
{
	VALUE,
	IDENTIFIER,
	CALL,

	PLUS,
	MINUS,
	MUL,
	DIV,
	MOD,

	EQUAL,
	NOT_EQUAL,
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	AND,
	OR,
	NEGATE,
};

enum BreakType
{
	NONE,
	BREAK,
	RETURN,
};

enum class Type
{
	UNSIGNED,
	BOOL,
};

struct Void
{
	bool operator==(const Void &rhs) const
	{
		return true;
	}

	friend std::istream &operator>>(std::istream &stream, Void &)
	{
		throw std::logic_error("Right shift used on void");
	}

	friend std::ostream &operator<<(std::ostream &stream, const Void &)
	{
		throw std::logic_error("Left shift used on void");
	}
};

using Value = std::variant<Void, unsigned, bool>;

inline Value TypeToValue(Type type)
{
	switch (type)
	{
	case Type::UNSIGNED:
		return unsigned();
		break;
	case Type::BOOL:
		return bool();
		break;
	default:
		throw std::domain_error("Unknown variable type");
	}
}

struct Expression
{
	ExprType type;
	Value value;
	std::string name;
	std::vector<Expression*> arguments;
	Location location;

	Expression *left() const
	{
		return arguments.at(0);
	}

	Expression *right() const
	{
		return arguments.at(1);
	}

	Expression(const Location &loc, ExprType type = ExprType::VALUE, const std::vector<Expression *> &arguments = {})
		: location(loc), type(type), arguments(arguments)
	{
	}
};

struct Statement
{
	Statement *next = nullptr;
	Location location;

	virtual ~Statement() {}
};

struct AssignmentStatement : public Statement
{
	std::string variable;
	Expression *expression;

	AssignmentStatement(std::string var, Expression *e) : variable(var), expression(e)
	{
	}
};

struct ReadStatement : public Statement
{
	std::string variable;

	ReadStatement(std::string var) : variable(var)
	{
	}
};

struct WriteStatement : public Statement
{
	Expression *expression;

	WriteStatement(Expression *e) : expression(e)
	{
	}
};

struct ReturnStatement : public Statement
{
	Expression *expression;

	ReturnStatement(Expression *e) : expression(e)
	{
	}
};

struct BreakStatement : public Statement
{
};


struct IfStatement : public Statement
{
	Expression *condition;
	Statement *truebranch;
	Statement *falsebranch;

	IfStatement(Expression *condition, Statement *truebranch, Statement *falsebranch)
		: condition(condition), truebranch(truebranch), falsebranch(falsebranch)
	{
	}
};

struct WhileStatement : public Statement
{
	Expression *condition;
	Statement *statements;

	WhileStatement(Expression *condition, Statement *statements)
		: condition(condition), statements(statements)
	{
	}
};

struct ExprStatement : public Statement
{
	Expression *expr;

	ExprStatement(Expression *expr)
		: expr(expr)
	{
	}
};

struct Declaration
{
	Type type;
	std::string name;

	Declaration* next = nullptr;
};

struct Function
{
	std::string name;
	Type type;
	Declaration *parameters = nullptr;
	Declaration *declarations = nullptr;
	Statement *statements = nullptr;
};

struct Class
{
	std::string name;
	std::map<std::string, Declaration*> variables;
	std::map<std::string, Function*> functions;
	std::map<std::string, Class*> classes;

	void AddFunction(Function *f)
	{
		functions[f->name] = f;
	}

	void AddVariable(Declaration *var)
	{
		variables[var->name] = var;
	}

	void AddClass(Class *cl)
	{
		classes[cl->name] = cl;
	}
};
