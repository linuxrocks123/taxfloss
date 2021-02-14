#pragma once

struct If_Expression;
struct Arithmetic_Expression;
struct Array_Expression;
struct Identifier_Expression;
struct Value_Expression;

struct Expression_Visitor
{
     virtual void visit(If_Expression& if_expression) = 0;
     virtual void visit(Arithmetic_Expression& arithmetic_expression) = 0;
     virtual void visit(Array_Expression& array_expression) = 0;
     virtual void visit(Identifier_Expression& identifier_expression) = 0;
     virtual void visit(Value_Expression& value_expression) = 0;
};
