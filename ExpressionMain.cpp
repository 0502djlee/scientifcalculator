#include <iostream>
#include "ExpressionParser.h"

int main(void)
{
	//test cases
	CntxtPointer cntxt = new Cntxt;
	ExpressionPointer a = new ConstExpression(4);
	ExpressionPointer b = new ConstExpression(1);
	ExpressionPointer c = new VarExpression("b");
	cntxt->addItem("a", a);
	cntxt->addItem("b", b);
	cntxt->addItem("c", c);

	string function_name = "function";
	vector<string> args;
	args.push_back("a");
	args.push_back("b");
	args.push_back("c");
	string function_expression = "a *b *c";
	UsrFuncPointer udf = new UsrFunc(args, function_expression);
	cntxt->addToFuncMap(function_name, udf);

	function_name = "function1";
	vector<string> args1;
	args1.push_back("x");
	args1.push_back("y");
	args1.push_back("z");
	function_expression = "x +y +z";
	UsrFuncPointer udf1 = new UsrFunc(args1, function_expression);
	cntxt->addToFuncMap(function_name, udf1);

	string test_expression = "a^2+2*2+a*b+a*b+function(1, 1, 1)+function(1, 1, 1)";
	init_parser_status();
	ExpressionParserPointer parser = new ExpressionParser(test_expression);
	ExpressionPointer ex = parser->buildParseTree();
	int ret = get_parser_error();
	//printf("%d\n", ret);
	if (ret) {
		cout << "Parsing Error: " << get_parser_error_msg() << endl;
		return 0;
	}
	init_ex_status();
	double val = ex->eval(cntxt);
	ret = get_ex_error();
	if (ret) {
		cout << "Expression Error: " << get_ex_error_msg() << endl;
		return 0;
	}
	cout << val << endl;
	cout << ex->toString() << endl;

	string test_expression1 = "function(a, b, c) + function1(b, a, c)";
	parser = new ExpressionParser(test_expression1);
	ex = parser->buildParseTree();
	ret = get_parser_error();
	//printf("%d\n", ret);
	if (ret) {
		cout << "Parsing Error: " << get_parser_error_msg() << endl;
		return 0;
	}
	init_ex_status();
	val = ex->eval(cntxt);
	ret = get_ex_error();
	if (ret) {
		cout << "Expression Error: " << get_ex_error_msg() << endl;
		return 0;
	}
	cout << val << endl; 
	cout << ex->toString() << endl;
	
	string test_expression2 = "1 + 2 * 3 + 4 / 5";
	parser = new ExpressionParser(test_expression2);
	ex = parser->buildParseTree();
	ret = get_parser_error();
	//printf("%d\n", ret);
	if (ret) {
		cout << "Parsing Error: " << get_parser_error_msg() << endl;
		return 0;
	}
	init_ex_status();
	val = ex->eval(cntxt);
	ret = get_ex_error();
	if (ret) {
		cout << "Expression Error: " << get_ex_error_msg() << endl;
		return 0;
	}
	cout << val << endl;
	cout << ex->toString() << endl;


	return 0;
}