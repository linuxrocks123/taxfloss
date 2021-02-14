#pragma once

#include "Expression.hpp"

struct Array_Expression : public Expression
{
     Expression* base;
     Expression* index;
     Expression* index2;

     Array_Expression(Expression* base_, Expression* index_)
          : base(base_), index(index_), index2(NULL) { }

     Array_Expression(Expression* base_, Expression* index_, Expression* index2_)
          : base(base_), index(index_), index2(index2_) { }
     
     void visit_with(Expression_Visitor& v) { v.visit(*this); }
};
