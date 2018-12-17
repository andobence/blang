#include "pch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Parser.h"
#include "PseudoCodeGen.h"
#include "Interpreter.h"

using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cerr << "Egy parancssori argumentum kell!" << endl;
		return 1;
	}
	Source source(argv[1]);
	Parser parser(source);
	parser.parse();

	auto globalScope = parser.GetGlobalScope();

	if (globalScope)
		PseudoCodeGen().print(std::cout, *globalScope);

	Interpreter interpreter(source, globalScope);
	interpreter.Run();

	std::cout << "end" << std::endl;

	return 0;
}
