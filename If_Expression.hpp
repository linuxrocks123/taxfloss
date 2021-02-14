#pragma once

#include "Expression.hpp"

struct If_Triple
{
     Expression* condition;
     Expression* assertion;
     Expression* value;
};     

struct If_Expression : public Expression
{
     If_Triple primary_branch;

     list<If_Triple> elif_branch_list;

     Expression* else_branch_assert;
     Expression* else_branch_value;

     If_Expression(If_Triple tpl, list<If_Triple> ebl, Expression* ass, Expression* val)
          : primary_branch(tpl), elif_branch_list(ebl), else_branch_assert(ass), else_branch_value(val) { }
     
     void visit_with(Expression_Visitor& v) { v.visit(*this); }
};
