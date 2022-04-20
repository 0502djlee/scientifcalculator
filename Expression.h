#ifndef _EXPRESSION_H
#define _EXPRESSION_H

#include <vector>
#include <string>
#include <set>
#include <map>
#include "SmartPointer.h"

using namespace std;

void init_ex_status(void);
int get_ex_error(void);
string& get_ex_error_msg(void);

typedef enum {
	EXPR = 0,
	CONST_EXPR,
	VAR_EXPR,
	UNARY_EXPR,
	BIN_EXPR,
	FUNC_EXPR,
} EType;

class Expression;
class Cntxt;
class UsrFunc;
typedef SmartPointer<Expression> ExpressionPointer;
typedef SmartPointer<Cntxt> CntxtPointer;
typedef SmartPointer<UsrFunc> UsrFuncPointer;

class Cntxt : public ReferenceCount {
public:
	Cntxt(void);
	~Cntxt(void);
	void clear(void);
	void addItem(const string& key, ExpressionPointer varObj);
	void removeItem(const string& key);
	ExpressionPointer getItem(const string& key);
	map<string, double> getItems(void);

	void setParent(CntxtPointer p);
	CntxtPointer getParent(void);

	void parseLine(const string& line);
	void addToFuncMap(const string& name, UsrFuncPointer udf);
	void parseFuncLine(const string& line);
	UsrFuncPointer getFuncFromMap(const string& name);

	string getVar(string& var);
	void addVar(const string& var);
	void removeVar(const string& var);

private:
	set<string> _VarSet;
	map<string, ExpressionPointer> _Cntxt;
	map<string, UsrFuncPointer> _FuncMap;
	CntxtPointer _Parent;
};

class Expression : public ReferenceCount {
public:
	Expression(void) {};
	~Expression(void) {};
	virtual double eval(CntxtPointer cntxt) = 0;

	void setType(EType type);
	EType getType(void);

	virtual string toString(void) = 0;

protected:
	EType _Type;
};

class VarExpression : public Expression {
public:
	VarExpression(const string& var);
	~VarExpression(void) {};
	double eval(CntxtPointer cntxt);
	void init(const string& var);

	virtual string toString(void);

private:
	string _Var;
};

class ConstExpression : public Expression {
public:
	ConstExpression(double value);
	~ConstExpression(void) {};
	double eval(CntxtPointer cntxt);

	virtual string toString(void);

private:
	double _Value;
};

class UnaryExpression : public Expression {
public:
	UnaryExpression(ExpressionPointer ex);
	~UnaryExpression(void) {};
	double eval(CntxtPointer cntxt) = 0;

	virtual string toString(void) = 0;

protected:
	ExpressionPointer _Expression;
};

class UAddExpression : public UnaryExpression {
public:
	UAddExpression(ExpressionPointer ex);
	~UAddExpression(void) {};
	double eval(CntxtPointer cntxt);

	virtual string toString(void);
};

class USubtractExpression : public UnaryExpression {
public:
	USubtractExpression(ExpressionPointer ex);
	~USubtractExpression(void) {};
	double eval(CntxtPointer cntxt);

	virtual string toString(void);

};

class BinaryExpression : public Expression {
public:
	BinaryExpression(ExpressionPointer left, ExpressionPointer right);
	~BinaryExpression(void) {};
	double eval(CntxtPointer cntxt) = 0;

	virtual string toString(void) = 0;
protected:
	ExpressionPointer _Left;
	ExpressionPointer _Right;
};

class AddExpression : public BinaryExpression {
public:
	AddExpression(ExpressionPointer left, ExpressionPointer right);
	~AddExpression(void) {};
	double eval(CntxtPointer cntxt);
	virtual string toString(void);
};

class SubtractExpression : public BinaryExpression {
public:
	SubtractExpression(ExpressionPointer left, ExpressionPointer right);
	~SubtractExpression(void) {};
	double eval(CntxtPointer cntxt);
	virtual string toString(void);
};

class MultExpression : public BinaryExpression {
public:
	MultExpression(ExpressionPointer left, ExpressionPointer right);
	~MultExpression(void) {};
	double eval(CntxtPointer cntxt);
	virtual string toString(void);
};

class DivExpression : public BinaryExpression {
public:
	DivExpression(ExpressionPointer left, ExpressionPointer right);
	~DivExpression(void) {};
	double eval(CntxtPointer cntxt);
	virtual string toString(void);
};

class PowExpression : public BinaryExpression {
public:
	PowExpression(ExpressionPointer left, ExpressionPointer right);
	~PowExpression(void) {};
	double eval(CntxtPointer cntxt);
	virtual string toString(void);
};

class FuncExpression : public Expression {
public:
	FuncExpression(const string& func, const vector<ExpressionPointer>& args);
	~FuncExpression(void) {};
	double eval(CntxtPointer cntxt);
	virtual string toString(void);
private:
	string _Func;
	vector<ExpressionPointer> _Args;
};

class UsrFunc : public ReferenceCount {
public:
	UsrFunc(const vector<string>& args, const string& expression);
	~UsrFunc(void) {};
	vector<string>& getArgs(void);
	ExpressionPointer getExpression(void);
private:
	string _Expressionpression;
	vector<string> _Args;
	ExpressionPointer _Expression;
};

#endif