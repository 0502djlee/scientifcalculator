#include <iostream>
#include "ExpressionParser.h"

int main(void)
{
	CntxtPointer cntxt = new Cntxt;
	ExpressionPointer a = new ConstExpression(2);
	ExpressionPointer b = new ConstExpression(3);
	ExpressionPointer c = new VarExpression("b");
	cntxt->addItem("a", a);
	cntxt->addItem("b", b);
	cntxt->addItem("c", c);

	string func_name = "f";
	vector<string> args;
	args.push_back("a");
	args.push_back("b");
	args.push_back("c");
	string func_expression = "a *b *c";
	UsrFuncPointer udf = new UsrFunc(args, func_expression);
	cntxt->addToFuncMap(func_name, udf);

	func_name = "f1";
	vector<string> args1;
	args1.push_back("x");
	args1.push_back("y");
	args1.push_back("z");
	func_expression = "x +y +z";
	UsrFuncPointer udf1 = new UsrFunc(args1, func_expression);
	cntxt->addToFuncMap(func_name, udf1);

	string test_expression = "a^2+2*2+a*b+a*b+f(1, 1, 1)+f(1, 1, 1)";
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

	string test_expression1 = "f(a, b, c) + f1(b, a, c)";
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