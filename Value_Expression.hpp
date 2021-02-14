#pragma once

#include "Expression.hpp"
#include "Value.hpp"

struct Value_Expression : public Expression
{
     Value value;

     Value_Expression(Value value_) : value(value_) { }
     
     void visit_with(Expression_Visitor& v) { v.visit(*this); }
};
