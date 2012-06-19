#include "operator.h"

namespace Script {
OperatorList oList;

void EvalAddition( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst+src, NULL));
}

void EvalPostfixIncrement( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    if( varStack.top().type == TokenEval::VARIABLE )
    {
        Variable *dst;
        dst = varStack.top().vPtr;
        varStack.pop();
        varStack.push(TokenEval(*dst, dst));
        *dst += 1;
    }
    else if( varStack.top().type == TokenEval::UCHAR )
    {
        unsigned char *dst;
        dst = (unsigned char *)varStack.top().vPtr;
        varStack.pop();
        varStack.push(TokenEval(*dst, dst, TokenEval::UCHAR));
        *dst += 1;
    }
}

void EvalPostfixDecrement( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    if( varStack.top().type == TokenEval::VARIABLE )
    {
        Variable *dst;
        dst = varStack.top().vPtr;
        varStack.pop();
        varStack.push(TokenEval(*dst, dst));
        *dst -= 1;
    }
    else if( varStack.top().type == TokenEval::UCHAR )
    {
        unsigned char *dst;
        dst = (unsigned char *)varStack.top().vPtr;
        varStack.pop();
        varStack.push(TokenEval(*dst, dst, TokenEval::UCHAR));
        *dst -= 1;
    }
}

void EvalPrefixIncrement( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    if( varStack.top().type == TokenEval::VARIABLE )
    {
        Variable *dst;
        dst = varStack.top().vPtr;
        varStack.pop();
        *dst += 1;
        varStack.push(TokenEval(*dst, dst));
    }
    else if( varStack.top().type == TokenEval::UCHAR )
    {
        unsigned char *dst;
        dst = (unsigned char *)varStack.top().vPtr;
        varStack.pop();
        *dst += 1;
        varStack.push(TokenEval(*dst, dst, TokenEval::UCHAR));
    }
}

void EvalPrefixDecrement( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    if( varStack.top().type == TokenEval::VARIABLE )
    {
        Variable *dst;
        dst = varStack.top().vPtr;
        varStack.pop();
        *dst -= 1;
        varStack.push(TokenEval(*dst, dst));
    }
    else if( varStack.top().type == TokenEval::UCHAR )
    {
        unsigned char *dst;
        dst = (unsigned char *)varStack.top().vPtr;
        varStack.pop();
        *dst -= 1;
        varStack.push(TokenEval(*dst, dst, TokenEval::UCHAR));
    }
}

void EvalNegation( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable dst;
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst*-1, NULL));
}

//TODO:  Fix this
void EvalAddressOf( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable *dst;
    dst = varStack.top().vPtr;
    varStack.pop();
    varStack.push(TokenEval(0, NULL));
}

void EvalLogicalNot( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable dst;
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(!dst, NULL));
}

//     void EvalBitwiseNot( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
//     {
//         Variable dst;
//         dst = varStack.top().first;
//         varStack.pop();
//         varStack.push(std::make_pair<Variable, Variable *>(~dst, NULL));
//     }

void EvalDereference( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable dst;
    dst = varStack.top().v;
    varStack.pop();
    dst = ReadAddress((LPCVOID)dst.AsValue());
    varStack.push(TokenEval(dst, NULL));
}

void EvalMultiplication( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst*src, NULL));
}

void EvalDivision( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst/src, NULL));
}

void EvalModulus( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst%src, NULL));
}

void EvalConcatenation( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst.operatorDot(src), NULL));
}

void EvalSubtraction( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst-src, NULL));
}

//     void EvalShiftLeft( std::stack<std::pair<Variable, Variable *> > &varStack, std::vector<TokenEval> &callStack )
//     {
//         Variable src, dst;
//         src = varStack.top().first;
//         varStack.pop();
//         dst = varStack.top().first;
//         varStack.pop();
//         varStack.push(std::make_pair<Variable, Variable *>(dst<<src, NULL));
//     }

//     void EvalShiftRight( std::stack<std::pair<Variable, Variable *> > &varStack, std::vector<TokenEval> &callStack )
//     {
//         Variable src, dst;
//         src = varStack.top().first;
//         varStack.pop();
//         dst = varStack.top().first;
//         varStack.pop();
//         varStack.push(std::make_pair<Variable, Variable *>(dst>>src, NULL));
//     }

//     void EvalRotateLeft( std::stack<std::pair<Variable, Variable *> > &varStack, std::vector<TokenEval> &callStack )
//     {
//         Variable src, dst;
//         src = varStack.top().first;
//         varStack.pop();
//         dst = varStack.top().first;
//         varStack.pop();
//         varStack.push(std::make_pair<Variable, Variable *>(dst.operatorRotateLeft(src), NULL));
//     }

//     void EvalRotateRight( std::stack<std::pair<Variable, Variable *> > &varStack, std::vector<TokenEval> &callStack )
//     {
//         Variable src, dst;
//         src = varStack.top().first;
//         varStack.pop();
//         dst = varStack.top().first;
//         varStack.pop();
//         varStack.push(std::make_pair<Variable, Variable *>(dst.operatorRotateRight(src), NULL));
//     }

void EvalLessThan( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst < src, NULL));
}

void EvalLessThanEqual( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst <= src, NULL));
}

void EvalGreaterThan( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst > src, NULL));
}

void EvalGreaterThanEqual( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst >= src, NULL));
}

void EvalIsEqual( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst == src, NULL));
}

void EvalIsNotEqual( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst != src, NULL));
}

void EvalAnd( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst & src, NULL));
}

void EvalXor( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst ^ src, NULL));
}

void EvalOr( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst | src, NULL));
}

void EvalLogicalAnd( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst && src, NULL));
}

void EvalLogicalOr( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src, dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().v;
    varStack.pop();
    varStack.push(TokenEval(dst || src, NULL));
}

void EvalAssign( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src;
    TokenEval dst;
    src = varStack.top().v;
    varStack.pop();
    dst  = varStack.top();
    varStack.pop();

    if( dst.vPtr != NULL )
    {
        if( dst.type == TokenEval::VARIABLE )
        {
            *(dst.vPtr) = src;
            varStack.push(TokenEval(*(dst.vPtr), dst.vPtr));
        }
        else if( dst.type == TokenEval::UCHAR )
        {
            unsigned char *dstChar;
            dstChar = (unsigned char *)dst.vPtr;
            *dstChar = src.AsValue();
            varStack.push(TokenEval(*dstChar, dstChar, TokenEval::UCHAR));
        }
    }
    else
    {
        WriteAddress( (LPVOID)dst.v.AsValue(), &src, 4 );
    }
}

void EvalComma( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    TokenEval param;
    param = varStack.top();
    varStack.pop();
    callStack.push_back(param);
}

void EvalLeftParentheses( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    // Not used
}

void EvalRightParentheses( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    // Not used
}

void EvalLeftBracket( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    // Not used
}

void EvalRightBracket( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    Variable src;
    Variable *dst;
    src = varStack.top().v;
    varStack.pop();
    dst = varStack.top().vPtr;
    varStack.pop();

    if( dst != NULL )
    {
        varStack.push( TokenEval(dst->operatorSplit(src.AsValue()), &(*dst)[src.AsValue()], 
            TokenEval::UCHAR) );
    }
    else
    {
        varStack.push(TokenEval(0, NULL));
    }
}

// Function stub that should never be called.  EvalReturn needs to be handled
// before it's called since this function can't terminate a function
void EvalReturn( std::stack<TokenEval> &varStack, std::vector<TokenEval> &callStack )
{
    // Not used
}

OperatorList::OperatorList()
{
    opMap["INC"]  = Token(Token::OPERATOR, 2, EvalPostfixIncrement);
    opMap["DEC"]  = Token(Token::OPERATOR, 2, EvalPostfixDecrement);
    opMap["++"] = Token(Token::OPERATOR, 3, EvalPrefixIncrement);
    opMap["--"] = Token(Token::OPERATOR, 3, EvalPrefixDecrement);
    opMap["NEG"] = Token(Token::OPERATOR, 3, EvalNegation);
    opMap["ADDRESS_OF"] = Token(Token::OPERATOR, 3, EvalAddressOf);
    opMap["!"] = Token(Token::OPERATOR, 3, EvalLogicalNot);
    //  opMap["~"] = Operator(Operator::OPERATOR, 3, EvalBitwiseNot);
    opMap["DEREF"] = Token(Token::OPERATOR, 3, EvalDereference);
    opMap["*"] = Token(Token::OPERATOR, 5, EvalMultiplication);
    opMap["/"] = Token(Token::OPERATOR, 5, EvalDivision);
    opMap["%"] = Token(Token::OPERATOR, 5, EvalModulus);
    // We want to perform subtraction first to prevent the following error:
    // a = 0 - 10 + 5
    opMap["."] = Token(Token::OPERATOR, 6, EvalConcatenation);
    opMap["+"] = Token(Token::OPERATOR, 6, EvalAddition);
    opMap["-"] = Token(Token::OPERATOR, 6, EvalSubtraction);
    //            opMap["<<"] = Operator(Operator::OPERATOR, 7, EvalShiftLeft);
    //            opMap[">>"] = Operator(Operator::OPERATOR, 7, EvalShiftRight);
    //            opMap["<<<"] = Operator(Operator::OPERATOR, 7, EvalRotateLeft);
    //            opMap[">>>"] = Operator(Operator::OPERATOR, 7, EvalRotateRight);
    opMap["<"] = Token(Token::OPERATOR, 8, EvalLessThan);
    opMap["<="] = Token(Token::OPERATOR, 8, EvalLessThanEqual);
    opMap[">"]  = Token(Token::OPERATOR, 8, EvalGreaterThan);
    opMap[">="] = Token(Token::OPERATOR, 8, EvalGreaterThanEqual);
    opMap["=="] = Token(Token::OPERATOR, 9, EvalIsEqual);
    opMap["!="] = Token(Token::OPERATOR, 9, EvalIsNotEqual);
    opMap["&"]  = Token(Token::OPERATOR, 10, EvalAnd);
    opMap["^"]  = Token(Token::OPERATOR, 11, EvalXor);
    opMap["|"]  = Token(Token::OPERATOR, 12, EvalOr);
    opMap["&&"] = Token(Token::OPERATOR, 13, EvalLogicalAnd);
    opMap["||"] = Token(Token::OPERATOR, 14, EvalLogicalOr);
    //?: ternary conditional
    opMap["="]  = Token(Token::OPERATOR, 16, EvalAssign);
    opMap[","]  = Token(Token::OPERATOR, 18, EvalComma);
    opMap["return"] = Token(Token::OPERATOR, 19, EvalReturn); // Has lowest precedence of operators
    opMap["("]  = Token(Token::BRACE, 0x7FFFFFFF, EvalLeftParentheses);
    opMap[")"]  = Token(Token::BRACE, 0x7FFFFFFF, EvalRightParentheses);
    opMap["BEGIN"] = Token(Token::BRACE, 0x7FFFFFFF, EvalLeftParentheses);
    opMap["END"]   = Token(Token::BRACE, 0x7FFFFFFF, EvalRightParentheses);
    opMap["["]  = Token(Token::BRACE, 0x7FFFFFFF, EvalLeftBracket);
    opMap["]"]  = Token(Token::BRACE, 0x7FFFFFFF, EvalRightBracket);
}

} // namespace Script
