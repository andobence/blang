%baseclass-preinclude "Types.h"
%lsp-needed
%ltype Location

%union {
	unsigned intvalue;
	const char *stringvalue;
	Expression *expr;
	Statement *statement;
	Type type;
	Declaration *declaration;
	Function *function;
	Class *classtype;
	std::vector<Expression*> *exprlist;
}

%left AND OR
%left EQUAL NOT_EQUAL
%left LESS GREATER LESS_EQUAL GREATER_EQUAL
%left PLUS MINUS
%left MUL DIV MOD
%left NEGATE
%left OPEN

%token CLOSE ASSIGN
%token READ WRITE
%token BOPEN BCLOSE SEMICOLON COMMA
%token <stringvalue> IDENTIFIER
%token TRUE FALSE IF ELSE WHILE COUT CIN RETURN BREAK
%token <type> UNSIGNED BOOL
%token <intvalue> NUMBER
%token <classtype> CLASS

%type <type> type
%type <expr> expr
%type <declaration> declaration declarations parameters parameterlist
%type <statement> statement statements assignment read write if while exprStatement return break
%type <function> function
%type <classtype> members
//%type <classtype> class
%type <exprlist> arguments argumentlist

%%

start: members { m_globalScope = $1; };

/*class:
	CLASS IDENTIFIER BOPEN members BCLOSE { $$ = $4; $$->name = $2; }
;*/
	
members:
	{ $$ = new Class(); std::cout << "new class " << $$ << std::endl; }
|
	function members { $$ = $2; $$->AddFunction($1); }
|
	declaration SEMICOLON members { $$ = $3; $$->AddVariable($1); }
/*|
	class members { $$ = $2; $$->AddClass($1); }*/
;

function:
	type IDENTIFIER OPEN parameterlist CLOSE BOPEN declarations statements BCLOSE
	{
		$$ = new Function{$2, $1, $4, $7, $8};
	}
;

parameterlist:
	{ $$ = nullptr; }
|
	parameters { $$ = $1; }
;

parameters:
	declaration { $$ = $1; }
|
	declaration COMMA parameters
	{
		$$ = $1;
		$$->next = $3;
		@@ = Location::Between(@1, @3);
	}
;

declarations:
	{ $$ = nullptr; }
|
	declaration SEMICOLON declarations
	{
		$$ = $1;
		$$->next = $3;
		@@ = Location::Between(@1, @3);
	}
;

declaration: type IDENTIFIER
	{
		$$ = new Declaration;
		$$->type = $1;
		$$->name = $2;
		@@ = Location::Between(@1, @2);
	}
;

type: UNSIGNED | BOOL;

statements:	
	{ $$ = nullptr; }
|
	statement statements	{ $$ = $1; $$->next = $2; }
;

statement:
	assignment
|
	read 
|
	write 
|
	exprStatement
|
	return
|
	break
|
	if
|
	while
;


assignment: IDENTIFIER ASSIGN expr SEMICOLON
	{
		$$ = new AssignmentStatement($1, $3);
		@@ = $$->location = Location::Between(@1, @4);
	}
;

read: CIN READ IDENTIFIER SEMICOLON
	{
		$$ = new ReadStatement($3);
		@@ = $$->location = Location::Between(@1, @4);
	}
;

write: COUT WRITE expr SEMICOLON
	{
		$$ = new WriteStatement($3);
		@@ = $$->location = Location::Between(@1, @4);
	}
;

exprStatement: expr SEMICOLON
	{
		$$ = new ExprStatement($1);
		@@ = $$->location = Location::Between(@1, @2);
	}
;

return: RETURN expr SEMICOLON
	{
		$$ = new ReturnStatement($2);
		@@ = $$->location = Location::Between(@1, @3);
	}
;

break: BREAK SEMICOLON
	{
		$$ = new BreakStatement;
		@@ = $$->location = Location::Between(@1, @2);
	}
;

if:
	IF OPEN expr CLOSE BOPEN statements BCLOSE ELSE BOPEN statements BCLOSE
	{
		$$ = new IfStatement($3, $6, $10);
		@@ = $$->location = Location::Between(@1, @11);
	}
|
	IF OPEN expr CLOSE BOPEN statements BCLOSE
	{
		$$ = new IfStatement($3, $6, nullptr);
		@@ = $$->location = Location::Between(@1, @7);
	}
;

while: WHILE OPEN expr CLOSE BOPEN statements BCLOSE
	{
		$$ = new WhileStatement($3, $6);
		@@ = $$->location = Location::Between(@1, @7);
	}
;

expr:
	FALSE {
		@@ = @1;
		$$ = new Expression(@@, ExprType::VALUE);
		$$->value = false;
	}
|
	TRUE {
		@@ = @1;
		$$ = new Expression(@@, ExprType::VALUE);
		$$->value = true;
	}
|
	NUMBER {
		@@ = @1;
		$$ = new Expression(@@, ExprType::VALUE);
		$$->value = $1;
	}
|
	IDENTIFIER OPEN argumentlist CLOSE {
		@@ = Location::Between(@1, @4);
		$$ = new Expression(@@, ExprType::CALL, *$3);
		$$->name = $1;

		delete $3;
	}
|
	IDENTIFIER {
		@@ = @1;
		$$ = new Expression(@@, ExprType::IDENTIFIER);
		$$->name = $1;
		/*LTYPE__ *bloc = &@1;
		std::cout << typeid(*bloc).name() << std::endl;
		std::cout << bloc->first_line << ":" << bloc->first_column << "  --  " << bloc->last_line << ":" << bloc->last_column << std::endl; */
	}
|
	expr PLUS expr			{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::PLUS, { $1, $3 }); }
|
	expr MINUS expr			{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::MINUS, { $1, $3 }); }
|
	expr MUL expr			{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::MUL, { $1, $3 }); }
|
	expr DIV expr			{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::DIV, { $1, $3 }); }
|
	expr MOD expr			{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::MOD, { $1, $3 }); }
|
	expr EQUAL expr			{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::EQUAL, { $1, $3 }); }
|
	expr NOT_EQUAL expr		{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::NOT_EQUAL, { $1, $3 }); }
|
	expr LESS expr			{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::LESS, { $1, $3 }); }
|
	expr LESS_EQUAL expr	{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::LESS_EQUAL, { $1, $3 }); }
|
	expr GREATER expr		{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::GREATER, { $1, $3 }); }
|
	expr GREATER_EQUAL expr	{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::GREATER_EQUAL, { $1, $3 }); }
|
	expr AND expr			{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::AND, { $1, $3 }); }
|
	expr OR expr			{ @@ = Location::Between(@1, @3); $$ = new Expression(@@, ExprType::OR, { $1, $3 });; }
|
	NEGATE expr				{ @@ = Location::Between(@1, @2); $$ = new Expression(@@, ExprType::NEGATE, { $2 }); }
|
	OPEN expr CLOSE			{ @@ = Location::Between(@1, @3); $$ = $2; $$->location = @@; }
;

argumentlist:
	{
		$$ = new std::vector<Expression*>;
	}
|
	arguments
	{
		$$ = new std::vector<Expression*>;
	}
;

arguments:
	expr
	{
		$$ = new std::vector<Expression*>;
		$$->push_back($1);
	}
|
	arguments COMMA expr
	{
		$$ = $1;
		$$->push_back($3);
	}
;
