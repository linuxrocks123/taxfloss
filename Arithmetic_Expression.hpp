#pragma once

#include "Expression.hpp"

struct Arithmetic_Expression : public Expression
{
     Expression* left_expression;
     int token;
     Expression* right_expression;


     //For NUM1 < x < NUM2
     int ternary_lt_token; //zero iff this is not a ternary expression
     Expression* ternary_lt_expression;

     Arithmetic_Expression(Expression* left, int token_, Expression* right)
          : left_expression(left), token(token_), right_expression(right), ternary_lt_token(0), ternary_lt_expression(NULL) { }

     Arithmetic_Expression(Expression* left, int token_, Expression* right, int tern_tok, Expression* ternary)
          : left_expression(left), token(token_), right_expression(right), ternary_lt_token(tern_tok), ternary_lt_expression(ternary) { }
          
     
     void visit_with(Expression_Visitor& v) { v.visit(*this); }
};
