#include "pch.h"
#include "Interpreter.h"


Value Interpreter::Run()
{
	Context ctx;

	if (auto it = m_globalScope->functions.find("main"); it != m_globalScope->functions.end())
	{
		auto mainFunc = it->second;
		Eval(ctx, *mainFunc, {});
	}

	return {};
}

Value Interpreter::Eval(Context &ctx, const Expression &expr)
{
	switch (expr.type)
	{
	case ExprType::VALUE:
		return expr.value;

	case ExprType::IDENTIFIER:
		return ctx.variables.at(expr.name);

	case ExprType::CALL:
	{
		if (auto it = m_globalScope->functions.find(expr.name); it != m_globalScope->functions.end())
		{
			return Eval(ctx, *it->second, expr.arguments);
		}
		else
		{
			m_source.Error(expr.location, "Error: call of undefined function");
			throw std::domain_error("Call of undefined function");
		}
	}

	case ExprType::PLUS:
	{
		unsigned lhs = std::get<unsigned>(Eval(ctx, *expr.left()));
		unsigned rhs = std::get<unsigned>(Eval(ctx, *expr.right()));

		return lhs + rhs;
	}
	case ExprType::MINUS:
	{
		unsigned lhs = std::get<unsigned>(Eval(ctx, *expr.left()));
		unsigned rhs = std::get<unsigned>(Eval(ctx, *expr.right()));

		return lhs - rhs;
	}
	case ExprType::MUL:
	{
		unsigned lhs = std::get<unsigned>(Eval(ctx, *expr.left()));
		unsigned rhs = std::get<unsigned>(Eval(ctx, *expr.right()));

		return lhs * rhs;
	}
	case ExprType::DIV:
	{
		unsigned lhs = std::get<unsigned>(Eval(ctx, *expr.left()));
		unsigned rhs = std::get<unsigned>(Eval(ctx, *expr.right()));

		return lhs / rhs;
	}
	case ExprType::MOD:
	{
		unsigned lhs = std::get<unsigned>(Eval(ctx, *expr.left()));
		unsigned rhs = std::get<unsigned>(Eval(ctx, *expr.right()));

		return lhs % rhs;
	}
	case ExprType::LESS:
	{
		unsigned lhs = std::get<unsigned>(Eval(ctx, *expr.left()));
		unsigned rhs = std::get<unsigned>(Eval(ctx, *expr.right()));

		return lhs < rhs;
	}
	case ExprType::LESS_EQUAL:
	{
		unsigned lhs = std::get<unsigned>(Eval(ctx, *expr.left()));
		unsigned rhs = std::get<unsigned>(Eval(ctx, *expr.right()));

		return lhs <= rhs;
	}
	case ExprType::GREATER:
	{
		unsigned lhs = std::get<unsigned>(Eval(ctx, *expr.left()));
		unsigned rhs = std::get<unsigned>(Eval(ctx, *expr.right()));

		return lhs > rhs;
	}
	case ExprType::GREATER_EQUAL:
	{
		unsigned lhs = std::get<unsigned>(Eval(ctx, *expr.left()));
		unsigned rhs = std::get<unsigned>(Eval(ctx, *expr.right()));

		return lhs >= rhs;
	}
	case ExprType::EQUAL:
	{
		auto lhs = Eval(ctx, *expr.left());
		auto rhs = Eval(ctx, *expr.right());

		return lhs == rhs;
	}
	case ExprType::NOT_EQUAL:
	{
		auto lhs = Eval(ctx, *expr.left());
		auto rhs = Eval(ctx, *expr.right());

		return !(lhs == rhs);
	}
	case ExprType::AND:
	{
		bool lhs = std::get<bool>(Eval(ctx, *expr.left()));
		bool rhs = std::get<bool>(Eval(ctx, *expr.right()));

		return lhs && rhs;
	}
	case ExprType::OR:
	{
		bool lhs = std::get<bool>(Eval(ctx, *expr.left()));
		bool rhs = std::get<bool>(Eval(ctx, *expr.right()));

		return lhs || rhs;
	}
	case ExprType::NEGATE:
	{
		bool lhs = std::get<bool>(Eval(ctx, *expr.left()));

		return !lhs;
	}
	}

	m_source.Error(expr.location, "Error: unknown expression type");
	throw std::domain_error("Unknown expression type");
}

Value Interpreter::Eval(Context &ctx, const Function &func, const std::vector<Expression *> &arguments)
{
	Context scope;
	scope.returnValue = TypeToValue(func.type);

	if (func.declarations)
	{
		for (Declaration *current = func.declarations; current != nullptr; current = current->next)
		{
			scope.variables[current->name] = TypeToValue(current->type);
		}
	}

	for (Statement *current = func.statements; current != nullptr; current = current->next)
	{
		auto val = Eval(scope, *current);
		if (val == BREAK)
		{
			m_source.Error(scope.returnLoc, "Error: break statement not in a loop");
			throw std::logic_error("Break statement not in a loop");
		}
		else if (val == RETURN)
			break;
	}

	return scope.returnValue;
}

BreakType Interpreter::Eval(Context &ctx, const Statement &statement)
{
	if (auto s = dynamic_cast<const WriteStatement *>(&statement))
		return Eval(ctx, *s);
	else if (auto s = dynamic_cast<const ReadStatement *>(&statement))
		return Eval(ctx, *s);
	else if (auto s = dynamic_cast<const ExprStatement *>(&statement))
		return Eval(ctx, *s);
	else if (auto s = dynamic_cast<const ReturnStatement *>(&statement))
		return Eval(ctx, *s);
	else if (auto s = dynamic_cast<const BreakStatement *>(&statement))
		return Eval(ctx, *s);
	else if (auto s = dynamic_cast<const AssignmentStatement *>(&statement))
		return Eval(ctx, *s);
	else if (auto s = dynamic_cast<const IfStatement *>(&statement))
		return Eval(ctx, *s);
	else if (auto s = dynamic_cast<const WhileStatement *>(&statement))
		return Eval(ctx, *s);
	else
		throw std::domain_error("Unsupported statement type");
}

BreakType Interpreter::Eval(Context &ctx, const AssignmentStatement &statement)
{
	auto  val = Eval(ctx, *statement.expression);
	auto &var = ctx.variables.at(statement.variable);

	if (val.index() == var.index())
		ctx.variables.at(statement.variable) = val;
	else
	{
		m_source.Error(statement.expression->location, "Error: assignment of incompatible types");
		throw std::domain_error("Assignment of incompatible types");
	}

	return {};
}

BreakType Interpreter::Eval(Context &ctx, const ReadStatement &statement)
{
	std::cout << "Input: ";
	std::visit([](auto &var)
	{
		std::cin >> var;
	}, ctx.variables.at(statement.variable));

	return {};
}

BreakType Interpreter::Eval(Context &ctx, const WriteStatement &statement)
{
	std::cout << "Output: ";
	std::visit([](const auto &var)
	{
		std::cout << var << std::endl;
	}, Eval(ctx, *statement.expression));

	return {};
}

BreakType Interpreter::Eval(Context &ctx, const ReturnStatement &statement)
{
	auto val = Eval(ctx, *statement.expression);
	if (ctx.returnValue.index() != val.index())
	{
		m_source.Error(statement.expression->location, "Error: returning incompatible value");
		throw std::domain_error("Returning incompatible value");
	}

	ctx.returnValue = val;
	ctx.returnLoc = statement.location;
	return RETURN;
}

BreakType Interpreter::Eval(Context &ctx, const BreakStatement &statement)
{
	ctx.returnLoc = statement.location;
	return BreakType::BREAK;
}

BreakType Interpreter::Eval(Context &ctx, const IfStatement &statement)
{
	if (std::get<bool>(Eval(ctx, *statement.condition)))
	{
		for (Statement *current = statement.truebranch; current != nullptr; current = current->next)
		{
			auto val = Eval(ctx, *current);
			if (val)
				return val;
		}
	}
	else
	{
		for (Statement *current = statement.falsebranch; current != nullptr; current = current->next)
		{
			auto val = Eval(ctx, *current);
			if (val)
				return val;
		}
	}

	return {};
}

BreakType Interpreter::Eval(Context &ctx, const WhileStatement &statement)
{
	while (std::get<bool>(Eval(ctx, *statement.condition)))
	{
		for (Statement *current = statement.statements; current != nullptr; current = current->next)
		{
			auto val = Eval(ctx, *current);
			if (val == BREAK)
				return {};
			else if (val == RETURN)
				return RETURN;
		}
	}

	return {};
}

BreakType Interpreter::Eval(Context &ctx, const ExprStatement &statement)
{
	Eval(ctx, *statement.expr);

	return {};
}
