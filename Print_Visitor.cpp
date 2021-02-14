#include "Print_Visitor.hpp"
#include "taxicality_config.hpp"
#include "taxicality.h"

#include <iostream>

using std::cout;
using std::endl;

static string get_operator(int token)
{
     if(token < 128)
          return string{static_cast<char>(token)};
     else
          switch(token)
          {
          case TRUNCATE: return "\\\\";
          case LT_EQUALS: return "<=";
          case GT_EQUALS: return ">=";
          case EQUALS: return "==";
          case NOT_EQUALS: return "!=";
          case OR: return "or";
          case AND: return "and";
          case NOT: return "not";
          case UNINITDEC: return ":=";
          case INITIALIZE: return "<-";
          }
     
     return "(unimplemented operator)";
}

static int indent_level;
static void indent()
{
     for(int i=0; i<indent_level*4; i++)
          cout << ' ';
}

static void print_if_triple(const If_Triple& triple, Print_Visitor& visitor)
{
     indent(); cout << "Triple Condition:" << endl;
     if(triple.condition)
     {
          indent_level++;
          triple.condition->visit_with(visitor);
          indent_level--;
     }
     else
     {
          indent(); cout << "NULL";
     }
     
     indent(); cout << "Triple Assertion:" << endl;
     if(triple.assertion)
     {
          indent_level++;
          triple.assertion->visit_with(visitor);
          indent_level--;
     }
     else
     {
          indent(); cout << "NULL";
     }
     
     indent(); cout << "Triple Value:" << endl;
     if(triple.value)
     {
          indent_level++;
          triple.value->visit_with(visitor);
          indent_level--;
     }
     else
     {
          indent(); cout << "NULL";
     }
}

void Print_Visitor::visit(const list<Declaration>& root)
{
     for(const auto& decl : root)
     {
          visit(decl);
     }
}

void Print_Visitor::visit(const Declaration& decl)
{
     cout << decl.identifier;
     switch(decl.init_status)
     {
     case Declaration::UNINITIALIZED:
          cout << ": ";
          cout << get_type(decl.rhs.type);
          break;
     case Declaration::INITIALIZED:
          cout << " = ";
          decl.rhs.expression->visit_with(*this);
          break;
     case Declaration::POST_INITIALIZED:
          cout << " <- ";
          decl.rhs.expression->visit_with(*this);
          break;
     }
     cout << endl;
}

void Print_Visitor::visit(If_Expression& if_expression)
{
     indent_level++;
     
     indent(); cout << "If Expression:" << endl;

     indent_level++;
     
     indent(); cout << "If Branch:" << endl;
     print_if_triple(if_expression.primary_branch,*this);
     
     for(auto& elif_triple : if_expression.elif_branch_list)
     {
          indent(); cout << "Elif branch:" << endl;
          print_if_triple(elif_triple,*this);
     }
     
     indent(); cout << "Else Branch:" << endl;
     print_if_triple(If_Triple{NULL,
                    if_expression.else_branch_assert,
                    if_expression.else_branch_value},*this);
     
     indent_level-=2;
}

void Print_Visitor::visit(Arithmetic_Expression& ae)
{
     indent_level++;

     indent(); cout << "Arithmetic Expression:" << endl;

     indent_level++;

     indent(); cout << "Left Expression:" << endl;
     ae.left_expression->visit_with(*this);
     indent(); cout << "Operator: " << get_operator(ae.token) << endl;
     if(ae.right_expression)
     {
          indent(); cout << "Right Expression:" << endl;
          ae.right_expression->visit_with(*this);
     }

     if(ae.ternary_lt_token)
     {
          indent(); cout << "Ternary LT Token: " << get_operator(ae.ternary_lt_token) << endl;
          indent(); cout << "Ternary LT Expression:" << endl;
          ae.ternary_lt_expression->visit_with(*this);
     }

     indent_level-=2;
}

void Print_Visitor::visit(Identifier_Expression& ie)
{
     indent_level++;
     indent(); cout << "IDENTIFIER: " << ie.identifier << endl;
     indent_level--;
}

void Print_Visitor::visit(Value_Expression& ve)
{
     indent_level++;
     indent(); cout << "VALUE: " << get_value(ve.value) << endl;
     indent_level--;
}

void Print_Visitor::visit(Array_Expression& ae)
{
     indent_level++;
     indent(); cout << "Array Expression:" << endl;

     indent_level++;
     indent(); cout << "Array Base:" << endl;
     ae.base->visit_with(*this);

     indent(); cout << "Array Index:" << endl;
     ae.index->visit_with(*this);

     indent(); cout << "Second Array Index:";
     if(!ae.index2)
          cout << " NULL" << endl;
     else
          ae.index2->visit_with(*this);
}
