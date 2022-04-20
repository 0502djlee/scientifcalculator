#include "Expression.h"
#include "ExpressionParser.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <math.h>

typedef struct _ExpressionStatus {
	int error_no;
	string error_msg;

} ExpressionStatus;

using namespace std;

ExpressionStatus g_exStatus;

static inline void EXPR_ERR(const string& key)
{
	g_exStatus.error_no++;
	g_exStatus.error_msg += "ERROR: Can't find variable with name: " + key + "\n";
	cout << "ERROR: Variable: " << key << " cannot be found in the context. " << endl;
}

void init_ex_status(void)
{
	memset(&g_exStatus, 0, sizeof(ExpressionStatus));
}

int get_ex_error(void)
{
	return g_exStatus.error_no;
}

string& get_ex_error_msg(void)
{
	return g_exStatus.error_msg;
}

static void tokenize(const string& line, vector<string>& tokens, const string& delimiter)
{
	string::size_type lastPos = line.find_first_not_of(delimiter, 0);
	
	string::size_type pos = line.find_first_of(delimiter, lastPos);
	while (string::npos != pos || string::npos != lastPos)
	{
		tokens.push_back(line.substr(lastPos, pos - lastPos));
		lastPos = line.find_first_not_of(delimiter, pos);
		pos = line.find_first_of(delimiter, lastPos);
	}
}

Cntxt::Cntxt(void)
{
	_Parent = NULL;
}

Cntxt::~Cntxt(void)
{
}

void Cntxt::clear(void)
{
	_Cntxt.clear();
	_FuncMap.clear();
	_VarSet.clear();
	_Parent = NULL;
}

void Cntxt::setParent(CntxtPointer p)
{
	_Parent = p;
}

CntxtPointer Cntxt::getParent(void)
{
	return _Parent;
}

void Cntxt::addItem(const string& key, ExpressionPointer varObj)
{
	_Cntxt[key] = varObj;
}

void Cntxt::removeItem(const string& key)
{
	map<string, ExpressionPointer>::iterator iter = _Cntxt.find(key);
	if (iter != _Cntxt.end()) {
		_Cntxt.erase(iter);
		return;
	}
	cout << "ERROR: Can't find the variable named: " << key << endl;
	return;
}

ExpressionPointer Cntxt::getItem(const string& key)
{
	map<string, ExpressionPointer>::iterator iter = _Cntxt.find(key);

	if (iter == _Cntxt.end()) {
		return NULL;
	}

	return iter->second;
}

map<string, double> Cntxt::getItems(void)
{
	map<string, ExpressionPointer>::iterator iter;
	map<string, double> ret;

	for (iter = _Cntxt.begin(); iter != _Cntxt.end(); iter++) {
		ret[iter->first] = iter->second->eval(this);
	}

	return ret;
}

void Cntxt::addVar(const string& var)
{
	_VarSet.insert(var);
}

void Cntxt::removeVar(const string& var)
{
	_VarSet.erase(var);
}

string Cntxt::getVar(string& var)
{
	string ret;
	set<string>::iterator iter = _VarSet.find(var);

	if (iter != _VarSet.end()) {
		ret = *iter;
	}
	return ret;
}

void Cntxt::parseLine(const string& line)
{
	vector<string> tokens;
	tokenize(line, tokens, " =");
	int i;
	string var = "";
	string expression = "";

	bool isVarFound = false;
	i = 0;
	while (i < tokens.size()) {
		if (tokens[i] == "")
			continue;
		if (isVarFound) {
			expression = tokens[i];
			break;
		}
		else {
			var = tokens[i];
			isVarFound = true;
		}
		i++;
	}
	if (expression == "" || var == "") {
		cout << "ERROR: cannot parse line : " << line << endl;
		return;
	}

	ExpressionParserPointer parser = new ExpressionParser(expression);
	ExpressionPointer ex = parser->buildParseTree();
	addItem(var, ex);
}

void Cntxt::addToFuncMap(const string& name, UsrFuncPointer udf)
{
	_FuncMap[name] = udf;
}

void Cntxt::parseFuncLine(const string& line)
{
	vector<string> tokens;
	tokenize(line, tokens, "=");
	int i;
	string func = "";
	string expression = "";

	bool foundFunc = false;
	for (i = 0; i < tokens.size(); i++) {
		if (tokens[i] == "") {
			continue;
		}
		if (foundFunc) {
			expression = tokens[i];
			break;
		}
		else {
			func = tokens[i];
			foundFunc = true;
		}
	}
	if (func == "" || expression == "") {
		cout << "ERROR: Unable to parse the line: " << line << endl;
		return;
	}
	vector<string> func_tokens;
	tokenize(func, func_tokens, " (,)");
	string func_name;
	vector<string> func_args;
	bool foundName = false;
	i = 0;
	while (i < func_tokens.size()) {
		if (func_tokens[i] == "") {
			continue;
		}
		if (foundName) {
			func_args.push_back(func_tokens[i]);
		}
		else {
			func_name = func_tokens[i];
			foundName = true;
		}
		i++;
	}
	UsrFuncPointer udf = new UsrFunc(func_args, expression);
	addToFuncMap(func_name, udf);
}

UsrFuncPointer Cntxt::getFuncFromMap(const string& name)
{
	map<string, UsrFuncPointer>::iterator iter = _FuncMap.find(name);
	if (iter != _FuncMap.end())
	{
		return iter->second;
	}
	return NULL;
}

void Expression::setType(EType type)
{
	_Type = type;
}

EType Expression::getType(void)
{
	return _Type;
}

VarExpression::VarExpression(const string& var)
{
	init(var);
}

void VarExpression::init(const string& var)
{
	_Var = var;
}


double VarExpression::eval(CntxtPointer cntxt)
{
	CntxtPointer cur = cntxt;
	while (cur.get() != NULL) {
		ExpressionPointer var = cur->getItem(_Var);
		if (var.get() != NULL) {
			if (cur->getVar(_Var) != "") {
				g_exStatus.error_no++;
				g_exStatus.error_msg += "ERROR: The variable is self defining = " + var->toString() + "\n";
				return 0.0;
			}
			cur->addVar(_Var);
			double val = var->eval(cur);
			cur->removeVar(_Var);
			return val;
		}
		cur = cur->getParent();
	}
	EXPR_ERR(_Var);
	return 0.0;
}

string VarExpression::toString(void)
{
	return _Var;
}

ConstExpression::ConstExpression(double value)
{
	_Value = value;
}

double ConstExpression::eval(CntxtPointer cntxt)
{
	return _Value;
}

string ConstExpression::toString(void)
{
	stringstream s;
	s << _Value;
	return s.str();
}

UnaryExpression::UnaryExpression(ExpressionPointer ex)
{
	_Expression = ex;
}

UAddExpression::UAddExpression(ExpressionPointer ex) : UnaryExpression(ex)
{
}

double UAddExpression::eval(CntxtPointer cntxt)
{
	return _Expression->eval(cntxt);
}

string UAddExpression::toString(void)
{
	return _Expression->toString();
}

USubtractExpression::USubtractExpression(ExpressionPointer ex) : UnaryExpression(ex)
{
}

double USubtractExpression::eval(CntxtPointer cntxt)
{
	return -(_Expression->eval(cntxt));
}

string USubtractExpression::toString(void)
{
	return "(-" + _Expression->toString() + ")";
}

BinaryExpression::BinaryExpression(ExpressionPointer left, ExpressionPointer right)
{
	_Left = left;
	_Right = right;
}

AddExpression::AddExpression(ExpressionPointer left, ExpressionPointer right) : BinaryExpression(left, right)
{
}

double AddExpression::eval(CntxtPointer cntxt)
{
	return (_Left->eval(cntxt) + _Right->eval(cntxt));
}

string AddExpression::toString(void)
{
	return ("(" + _Left->toString() + "+" + _Right->toString() + ")");
}

SubtractExpression::SubtractExpression(ExpressionPointer left, ExpressionPointer right) : BinaryExpression(left, right)
{
}

double SubtractExpression::eval(CntxtPointer cntxt)
{
	return (_Left->eval(cntxt) - _Right->eval(cntxt));
}

string SubtractExpression::toString(void)
{
	return ("(" + _Left->toString() + "-" + _Right->toString() + ")");
}

MultExpression::MultExpression(ExpressionPointer left, ExpressionPointer right) : BinaryExpression(left, right)
{
}

double MultExpression::eval(CntxtPointer cntxt)
{
	return (_Left->eval(cntxt) *_Right->eval(cntxt));
}

string MultExpression::toString(void)
{
	return ("(" + _Left->toString() + "*" + _Right->toString() + ")");
}

DivExpression::DivExpression(ExpressionPointer left, ExpressionPointer right) : BinaryExpression(left, right)
{
}

double DivExpression::eval(CntxtPointer cntxt)
{
	return (_Left->eval(cntxt) / _Right->eval(cntxt));
}

string DivExpression::toString(void)
{
	return ("(" + _Left->toString() + "/" + _Right->toString() + ")");
}

PowExpression::PowExpression(ExpressionPointer left, ExpressionPointer right) : BinaryExpression(left, right)
{
}

double PowExpression::eval(CntxtPointer cntxt)
{
	return (pow(_Left->eval(cntxt), _Right->eval(cntxt)));
}

string PowExpression::toString(void)
{
	return ("(" + _Left->toString() + "^" + _Right->toString() + ")");
}

FuncExpression::FuncExpression(const string& func, const vector<ExpressionPointer>& args)
{
	_Args = args;
	_Func = func;
}

double FuncExpression::eval(CntxtPointer cntxt)
{
	double arg0;
	double arg1;
	double ret;
	if (_Func == "log") {
		ret = log(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "log10") {
		ret = log10(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "pow") {
		arg0 = _Args[0]->eval(cntxt);
		arg1 = _Args[1]->eval(cntxt);
		ret = pow(arg0, arg1);
		return ret;
	}
	else if (_Func == "exp") {
		ret = exp(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "sin") {
		ret = sin(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "cos") {
		ret = cos(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "tan") {
		ret = tan(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "asin") {
		ret = asin(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "acos") {
		ret = acos(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "atan") {
		ret = atan(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "atan2") {
		arg0 = _Args[0]->eval(cntxt);
		arg1 = _Args[1]->eval(cntxt);
		ret = atan2(arg0, arg1);
		return ret;
	}
	else if (_Func == "sinh") {
		ret = sinh(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "cosh") {
		ret = cosh(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "tanh") {
		ret = tanh(_Args[0]->eval(cntxt));
		return ret;
	}
	else if (_Func == "min") {
		arg0 = _Args[0]->eval(cntxt);
		arg1 = _Args[1]->eval(cntxt);
		ret = (arg0 < arg1) ? arg0 : arg1;
		return ret;
	}
	else if (_Func == "max") {
		arg0 = _Args[0]->eval(cntxt);
		arg1 = _Args[1]->eval(cntxt);
		ret = (arg0 > arg1) ? arg0 : arg1;
		return ret;
	}
	else if (_Func == "fact") {
		double doubNum = _Args[0]->eval(cntxt);
		if (doubNum < 0) {
			g_exStatus.error_no++;
			g_exStatus.error_msg += "ERROR: no implementation\n";
			cout << "ERROR: Negative number cannot use factorial: " << doubNum << endl;
			return 0.0;
		}
		else {
			int intNum = (int)(doubNum + 0.5);
			if (intNum == 0)
				return 1;

			int ret = 1;
			int i = intNum;
			while (i > 1) {
				ret = ret * i;
				i--;
			}
			return ret;
		}
	}
	else {
		UsrFuncPointer udf = cntxt->getFuncFromMap(_Func);
		if (udf.get()) {
			double val = 0.0;
			vector<string> udf_args = udf->getArgs();
			if (udf_args.size() != _Args.size()) {
				cout << "ERROR: Function arguments amount doesn't match for function " << _Func << endl;
				return 0.0;
			}

			ExpressionPointer func_expr = udf->getExpression();
			CntxtPointer new_context = new Cntxt;
			new_context->setParent(cntxt);
			int i;
			for (i = 0; i < _Args.size(); i++) {
				if (udf_args[i] != _Args[i]->toString())
					new_context->addItem(udf_args[i], _Args[i]);
			}
			val = func_expr->eval(new_context);
			return val;
		}
		else {
			g_exStatus.error_no++;
			g_exStatus.error_msg += "ERROR: no implementation\n";
		}
	}
	return 0;
}

string FuncExpression::toString(void)
{
	string ret = _Func + "(";
	int i = 0;
	while (i < _Args.size() - 1) {
		ret += _Args[i]->toString() + ", ";
		i++;
	}
	ret += _Args[i]->toString() + ")";
	return ret;
}

UsrFunc::UsrFunc(const vector<string>& args, const string& expression)
{
	_Args = args;
	_Expressionpression = expression;
	ExpressionParserPointer parser = new ExpressionParser(expression);
	_Expression = parser->buildParseTree();
}

vector<string>& UsrFunc::getArgs(void)
{
	return _Args;
}

ExpressionPointer UsrFunc::getExpression(void)
{
	return _Expression;
}