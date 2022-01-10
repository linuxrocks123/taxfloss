#pragma once

#include "taxicality_config.hpp"
#include <unordered_set>

struct Executor : public Expression_Visitor
{
     void visit(list<Declaration>& root);
     void visit(Declaration& decl);

     void visit(If_Expression& if_expression);
     void visit(Arithmetic_Expression& arithmetic_expression);
     void visit(Array_Expression& array_expression);
     void visit(Identifier_Expression& ie);
     void visit(Value_Expression& ve);

     string lhs;
     Value retval;
     string last_id_lookup;
     string last_real_decl = "(no prior declaration in file)";

     struct uninitialized_value_exception
     {
          string identifier;
     };

     list<string> uninitializables;
     list<Declaration*> regex_post_modifiers;
};
