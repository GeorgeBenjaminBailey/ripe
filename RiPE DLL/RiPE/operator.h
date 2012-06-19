#ifndef __RIPE_OPERATOR_H
#define __RIPE_OPERATOR_H

#include <map>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "../memreadwrite.h"
#include "script_variable.h"

namespace Script
{
    struct TokenEval
    {
        enum Type
        {
            VARIABLE,
            UINT,
            USHORT,
            UCHAR
        };
        Variable v;
        union
        {
            Variable       *vPtr;
            unsigned int   *iPtr;
            unsigned short *sPtr;
            unsigned char  *cPtr;
        };
        Type type;

        TokenEval()
        {
            v    = 0;
            vPtr = NULL;
            type = VARIABLE;
        }

        TokenEval(Variable var, void *ptr, Type typ = VARIABLE)
        {
            v    = var;
            vPtr = (Variable *)ptr;
            type = typ;
        }
    };

    // The pair consists of the same value twice.  The first time it's used
    // as a copy.  The second is as a reference.  This is to accommodate
    // functions that change the src / dst
    typedef void (*EvalExpression)( std::stack<TokenEval>&, std::vector<TokenEval>& );

    void EvalAddition( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalPostfixIncrement( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalPostfixDecrement( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalPrefixIncrement( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalPrefixDecrement( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalNegation( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    //TODO:  Fix this
    void EvalAddressOf( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalLogicalNot( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

//     void EvalBitwiseNot( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalDereference( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalMultiplication( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalDivision( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalModulus( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalConcatenation( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalSubtraction( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

//     void EvalShiftLeft( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

//     void EvalShiftRight( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

//     void EvalRotateLeft( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

//     void EvalRotateRight( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalLessThan( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalLessThanEqual( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalGreaterThan( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalGreaterThanEqual( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalIsEqual( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalIsNotEqual( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalAnd( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalXor( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalOr( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalLogicalAnd( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalLogicalOr( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalAssign( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalComma( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalLeftParentheses( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalRightParentheses( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalLeftBracket( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    void EvalRightBracket( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack );

    struct Token
    {
        enum Type
        {
            NONE     = 1<<0,
            OPERATOR = 1<<1,
            BRACE    = 1<<2,
            VARIABLE = 1<<3,
            DECIMAL  = 1<<4
        };
        Type type;
        int precedence;
        EvalExpression expression;
        Variable v;
        std::string name;

        Token()
        {
            type = NONE;
            precedence = 0;
        }

        Token(Type typ, const std::string &nam)
        {
            type = typ;
            name = nam;
        }

        Token(Type typ, int p, const std::string &nam)
        {
            type       = typ;
            name       = nam;
            precedence = p;
        }

        Token(Type typ, const std::string &nam, Variable var)
        {
            type = typ;
            name = nam;
            v    = var;
        }

        Token(Type typ, int p, EvalExpression expr)
        {
            type = typ;
            precedence = p;
            expression = expr;
        }
    };

    struct OperatorList
    {
        std::map<std::string, Token> opMap;

        OperatorList();
    };

    extern OperatorList oList;
}

#endif // __RIPE_OPERATOR_H
