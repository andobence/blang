#include "pch.h"
#include "PseudoCodeGen.h"

PseudoCodeGen::PseudoCodeGen()
{
}

PseudoCodeGen::~PseudoCodeGen()
{
}

void PseudoCodeGen::print(std::ostream &stream, const AssignmentStatement &statement)
{
	stream << tabs() << "ASSIGN " << statement.variable << " <- ";
	print(stream, *statement.expression);
}

void PseudoCodeGen::print(std::ostream &stream, const ReadStatement &statement)
{
	stream << tabs() << "READ -> " << statement.variable;
}

void PseudoCodeGen::print(std::ostream &stream, const WriteStatement &statement)
{
	stream << tabs() << "WRITE <- ";
	print(stream, *statement.expression);
}

void PseudoCodeGen::print(std::ostream & stream, const ReturnStatement &statement)
{
	stream << tabs() << "RETURN <- ";
	print(stream, *statement.expression);
}

void PseudoCodeGen::print(std::ostream & stream, const BreakStatement &statement)
{
	stream << tabs() << "BREAK";
}

void PseudoCodeGen::print(std::ostream &stream, const IfStatement &statement)
{
	stream << tabs() << "IF ";
	print(stream, *statement.condition);
	stream << " THEN\n";

	for (Statement *current = statement.truebranch; current != nullptr; current = current->next)
	{
		m_ident++;
		print(stream, *current);
		m_ident--;
		stream << '\n';
	}
	if (statement.falsebranch)
	{
		stream << tabs() << "ELSE\n";
		for (Statement *current = statement.falsebranch; current != nullptr; current = current->next)
		{
			m_ident++;
			print(stream, *current);
			m_ident--;
			stream << '\n';
		}
	}
	stream << tabs() << "END";
}

void PseudoCodeGen::print(std::ostream &stream, const WhileStatement &statement)
{
	stream << tabs() << "WHILE ";
	print(stream, *statement.condition);
	stream << " DO\n";

	for (Statement *current = statement.statements; current != nullptr; current = current->next)
	{
		m_ident++;
		print(stream, *current);
		m_ident--;
		stream << '\n';
	}
	stream << tabs() << "END";
}

void PseudoCodeGen::print(std::ostream &stream, const ExprStatement &statement)
{
	stream << tabs();
	print(stream, *statement.expr);
}

void PseudoCodeGen::print(std::ostream &stream, const Statement &statement)
{
	if (auto s = dynamic_cast<const WriteStatement *>(&statement))
		print(stream, *s);
	else if (auto s = dynamic_cast<const ReadStatement *>(&statement))
		print(stream, *s);
	else if (auto s = dynamic_cast<const ExprStatement *>(&statement))
		print(stream, *s);
	else if (auto s = dynamic_cast<const ReturnStatement *>(&statement))
		print(stream, *s);
	else if (auto s = dynamic_cast<const BreakStatement *>(&statement))
		print(stream, *s);
	else if (auto s = dynamic_cast<const AssignmentStatement *>(&statement))
		print(stream, *s);
	else if (auto s = dynamic_cast<const IfStatement *>(&statement))
		print(stream, *s);
	else if (auto s = dynamic_cast<const WhileStatement *>(&statement))
		print(stream, *s);
	else
		throw std::domain_error("Unsupported statement type");
}

void PseudoCodeGen::print(std::ostream &stream, const Expression &expr)
{
	static std::map<ExprType, std::string> g_operatorSymbols = {
		{ExprType::PLUS, "+"},
		{ExprType::MINUS, "-"},
		{ExprType::MUL, "/"},
		{ExprType::DIV, "*"},
		{ExprType::MOD, "%"},
		{ExprType::EQUAL, "=="},
		{ExprType::NOT_EQUAL, "!="},
		{ExprType::LESS, "<"},
		{ExprType::LESS_EQUAL, "<="},
		{ExprType::GREATER, ">"},
		{ExprType::GREATER_EQUAL, ">="},
		{ExprType::AND, "&&"},
		{ExprType::OR, "||"},
	};

	switch (expr.type)
	{
	case ExprType::VALUE:
		if (std::holds_alternative<bool>(expr.value))
			stream << (std::get<bool>(expr.value) ? "true" : "false");

		else if (std::holds_alternative<unsigned>(expr.value))
			stream << std::get<unsigned>(expr.value);

		return;

	case ExprType::IDENTIFIER:
		stream << expr.name;
		break;

	case ExprType::CALL:
		stream << expr.name << "(";

		for (size_t i = 0; i < expr.arguments.size(); i++)
		{
			if (i != 0)
				stream << ", ";

			print(stream, *expr.arguments[i]);
		}

		stream << ")";
		break;

	case ExprType::PLUS:
	case ExprType::MINUS:
	case ExprType::MUL:
	case ExprType::DIV:
	case ExprType::MOD:
	case ExprType::EQUAL:
	case ExprType::NOT_EQUAL:
	case ExprType::LESS:
	case ExprType::LESS_EQUAL:
	case ExprType::GREATER:
	case ExprType::GREATER_EQUAL:
	case ExprType::AND:
	case ExprType::OR:
		stream << "(";
		print(stream, *expr.left());
		stream << " " << g_operatorSymbols.at(expr.type) << " ";
		print(stream, *expr.right());
		stream << ")";
		break;
	case ExprType::NEGATE:
		stream << "!";
		print(stream, *expr.left());
		stream << ")";
		break;
	}
}

void PseudoCodeGen::print(std::ostream &stream, const Type &type)
{
	switch (type)
	{
	case Type::UNSIGNED:
		stream << "unsigned";
		break;
	case Type::BOOL:
		stream << "bool";
		break;
	}
}

void PseudoCodeGen::print(std::ostream &stream, const Declaration &decl)
{
	stream << tabs() << decl.name << " AS ";
	print(stream, decl.type);
}

void PseudoCodeGen::print(std::ostream &stream, const Function &func)
{
	std::cout << tabs() << "FUNCTION " << func.name << '\n';
	if (func.parameters)
	{
		for (Declaration *current = func.parameters; current != nullptr; current = current->next)
		{
			stream << '\t';
			print(stream, *current);
			stream << '\n';
		}
	}
	if (func.declarations)
	{
		stream << tabs() << "VAR: \n";
		for (Declaration *current = func.declarations; current != nullptr; current = current->next)
		{
			stream << '\t';
			print(stream, *current);
			stream << '\n';
		}
	}
	stream << tabs() << "DO\n";
	for (Statement *current = func.statements; current != nullptr; current = current->next)
	{
		m_ident++;
		print(stream, *current);
		m_ident--;
		stream << '\n';
	}
}

void PseudoCodeGen::print(std::ostream &stream, const Class &cl)
{
	stream << tabs() << "CLASS " << cl.name << '\n';
	m_ident++;
	for (auto[name, var] : cl.variables)
	{
		print(stream, *var);
		stream << '\n';
	}
	for (auto[name, func] : cl.functions)
	{
		print(stream, *func);
		stream << '\n';
	}
	for (auto[name, cl] : cl.classes)
	{
		print(stream, *cl);
		stream << '\n';
	}
	m_ident--;
}

std::string PseudoCodeGen::tabs()
{
	return std::string(m_ident, '\t');
}