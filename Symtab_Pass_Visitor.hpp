#pragma once

#include "taxicality_config.hpp"

#include <set>

struct Symtab_Pass_Visitor : public Expression_Visitor
{
     void visit(const list<Declaration>& root);
     void visit(const Declaration& decl);

     void visit(If_Expression& if_expression);
     void visit(Arithmetic_Expression& arithmetic_expression);
     void visit(Array_Expression& array_expression);
     void visit(Identifier_Expression& ie);
     void visit(Value_Expression& ve);

     std::set<string> uninitialized_vars;
     std::set<string> postinitialized_vars;
     vector<string> error_msgs;
     vector<string> diagnostic_msgs;
};
