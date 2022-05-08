#include <stack>
#include <iostream>
#include <sstream>
#include <string.h>
#include "ExpressionParser.h"

using namespace std;

typedef struct _ParserStatus {
    int error_no;
    string error_msg;
} ParserStatus;

ParserStatus g_parserStatus;

enum Operator {
    POW,
    MULT, DIV,
    PLUS, MINUS,
    INVALID
};

static int precedence[] = {
    1,
    2, 2,
    3, 3,
    10000
};

void init_parser_status(void)
{
    memset(&g_parserStatus, 0, sizeof(ParserStatus));
}

int get_parser_error(void)
{
    return g_parserStatus.error_no;
}

string &get_parser_error_msg(void)
{
    return g_parserStatus.error_msg;
}

ExpressionParser::ExpressionParser(const std::string& str) : _ex(str.c_str()), _pos(0), _next(0)
{
    _next = _ex[0];
}

ExpressionParser::~ExpressionParser()
{
}

void ExpressionParser::consume()
{
    if (_next)
        _next = _ex[++_pos];
}

void ExpressionParser::skipSpace()
{
    while (isspace(_next)) 
        _next = _ex[++_pos];
}

bool ExpressionParser::parseUnaryOperator()
{
	char c;
	if (parseChar('-'))
		c = '-';
	else if (parseChar('+'))
		c = '+';
	else
		return 0;

	skipSpace();
	required("term", parseTerm());
	ExpressionPointer e = _operands.top();
	_operands.pop();
	ExpressionPointer p;
	if (c == '-')
		p = new USubtractExpression(e);
	else if (c == '+')
		p = new UAddExpression(e);
	_operands.push(p);
	return 1;
}

bool ExpressionParser::parseBinaryOperator()
{
	skipSpace();
	switch (_next)
	{
	case '+':
		pushOperator(PLUS);
		break;
	case '-':
		pushOperator(MINUS);
		break;
	case '*':
		pushOperator(MULT);
		break;
	case '/':
		pushOperator(DIV);
		break;
	case '^':
		pushOperator(POW);
		break;
	default:
		return 0;
	}
	consume();
	skipSpace();
	required("term", parseTerm());

	return 1;
}

bool ExpressionParser::parseNumber()
{

	bool isInt = 1;
	string num;
	if (!isdigit(_next)) {
		if (!parseChar('.')) {
			return 0;
		}
		else
		{
			num = num + '.';
			isInt = 0;
		}
	}

	while (isdigit(_next))
	{
		num = num + _next;
		consume();
	}

	if (isInt)
	{
		if (parseChar('.'))
		{
			isInt = 0;
			num = num + '.';
			while (isdigit(_next))
			{
				num = num + _next;
				consume();
			}
		}
	}

	if (parseChar('e') || parseChar('E'))
	{
		isInt = 0;
		num = num + 'e';
		if (_next == '+' || _next == '-')
		{
			num = num + _next;
			consume();
		}
		if (!isdigit(_next))
			syntaxError();
		while (isdigit(_next))
		{
			num = num + _next;
			consume();
		}
	}

	double p;
	const char* start = num.c_str();
	char* end;
	if (isInt)
		p = strtol(start, &end, 10);
	else
		p = strtod(start, &end);
	if (end - start != num.length())
		syntaxError();
	_operands.push(ExpressionPointer(new ConstExpression(p)));
	return 1;
}

bool ExpressionParser::parseTerm()
{
	if (parseChar('('))
	{
		skipSpace();
		required("expression", parseExpression());
		skipSpace();
		required(")", parseChar(')'));
		return 1;
	}

	if (parseUnaryOperator() || parseNumber() || parseCall())
		return 1;

	return 0;
}

bool ExpressionParser::parseExpression()
{
	_operators.push(INVALID);

	if (!parseTerm()) {
		return 0;
	}

	while (parseBinaryOperator()) {}

	if (g_parserStatus.error_no) {
		return 0;
	}

	while (_operators.top() != INVALID) {
		popOperator();
		if (g_parserStatus.error_no)
			return 0;
	}

	_operators.pop();

	return 1;
}

void ExpressionParser::pushOperator(int op)
{
	while (precedence[_operators.top()] <= precedence[op]) {
		popOperator();
	}
	_operators.push(op);
}

bool ExpressionParser::parseName()
{
	if (_next == 0 || isdigit(_next))
		return 0;
	string s;
	bool done = 0;
	while (!done)
	{
		switch (_next)
		{
		case 0:
		case '+':
		case '-':
		case '*':
		case '/':
		case '^':
		case '=':
		case '(':
		case ')':
		case ',':
		case '.':
			done = 1;
			break;
		default:
			done = isspace(_next);
			break;
		}
		if (!done)
		{
			s = s + _next;
			consume();
		}
	}
	if (s.length() == 0)
		return 0;
	_operands.push(ExpressionPointer(new VarExpression(s)));
	return 1;
}

bool ExpressionParser::parseLiteral(const char *s)
{
    int i = _pos;
    while (*s && *s == _ex[i++]) 
        ++s;
    if (*s)
        return 0;
    _next = _ex[_pos = i];
    return 1;
}

bool ExpressionParser::parseKeyword(const char *s)
{
    int i=_pos;
    while (*s && *s == _ex[i++]) 
        ++s;
    if (*s || isalnum(_ex[i]))
        return 0;
    _next = _ex[_pos = i];
    return 1;
}

bool ExpressionParser::parseChar(char c)
{
    if (_next != c)
        return 0;
    _next = _ex[++_pos];
    return 1;
}

ExpressionPointer ExpressionParser::buildParseTree()
{
    required("expression", parseExpression());
    required("end", _next == 0);
    if (g_parserStatus.error_no)
        return NULL;
    ExpressionPointer root = _operands.top();
    return root;
}

void ExpressionParser::syntaxError()
{
    ostringstream ostr;
    g_parserStatus.error_no++;
    ostr << "Syntax Error : " << _ex << " " << _pos << endl;
    g_parserStatus.error_msg += ostr.str();
    cout << "Syntax Error : " << _ex << " " << _pos << endl;
}

void ExpressionParser::required(const char *expected, bool found)
{
	if (!found) {
		syntaxError();
	}
}

bool ExpressionParser::parseCall()
{
    if (!parseName())
        return 0;

    skipSpace();
    if (parseChar('('))
    {
        vector<ExpressionPointer> args;

        skipSpace();
        if (!parseChar(')'))
        {
            skipSpace();
            required("expression", parseExpression());
            skipSpace();
            args.push_back(_operands.top());
            _operands.pop();

            while (parseChar(','))
            {
                skipSpace();
                required("expression", parseExpression());
                skipSpace();
                args.push_back(_operands.top());
                _operands.pop();
            }

            required(")", parseChar(')'));
        }

        ExpressionPointer nameExpression = _operands.top();
        _operands.pop();
        ExpressionPointer p = new FuncExpression(nameExpression->toString(), args);
        _operands.push(p);
    }

    return 1;
}


void ExpressionParser::popOperator()
{
    int top = _operators.top();

    ExpressionPointer ex2 = _operands.top();
    _operands.pop();
    ExpressionPointer ex1 = _operands.top();
    _operands.pop();

    ExpressionPointer p;
    switch (top)
    {
    case PLUS:
        p = new AddExpression(ex1,ex2);
        break;
    case MINUS:
        p = new SubtractExpression(ex1,ex2);
        break;
    case MULT:
        p = new MultExpression(ex1,ex2);
        break;
    case DIV:
        p = new DivExpression(ex1,ex2);
        break;
    case POW:
        p = new PowExpression(ex1,ex2);
        break;

    default:
		g_parserStatus.error_no++;
		g_parserStatus.error_msg += "ERROR: Unknown operator";
		cout << "ERROR: Found unknown operator: " << top << " during expression parsing\n";
		return;
	}
    _operands.push(p);
    _operators.pop();
}