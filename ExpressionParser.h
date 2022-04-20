#ifndef _EXPRESSION_PARSER_H
#define _EXPRESSION_PARSER_H

#include <string>
#include <stack>
#include "Expression.h"
#include "SmartPointer.h"

void init_parser_status(void);
int get_parser_error(void);
string& get_parser_error_msg(void);

class ExpressionParser;
typedef SmartPointer<ExpressionParser> ExpressionParserPointer;

class ExpressionParser : public ReferenceCount
{
public:
    ExpressionParser(const std::string& str);
    ~ExpressionParser();
    ExpressionPointer buildParseTree();
private:
    void syntaxError();
    void required(const char *expected, bool found);
    void consume();
    void skipSpace();
	bool parseUnaryOperator();
	bool parseBinaryOperator();
	bool parseNumber();
	bool parseTerm();
    bool parseExpression();
    bool parseName();
    bool parseCall();
    bool parseKeyword(const char *s);
    bool parseLiteral(const char *s);
    bool parseChar(char c);

    void pushOperator(int op);
    void popOperator();

    const char *_ex;
    int _pos;
    char _next;

    std::stack<int> _operators;
    std::stack<ExpressionPointer> _operands;
};
#endif