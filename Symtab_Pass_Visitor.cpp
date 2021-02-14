#include "Symtab_Pass_Visitor.hpp"

void Symtab_Pass_Visitor::visit(const list<Declaration>& root)
{
     for(const auto& decl : root)
          visit(decl);
}

void Symtab_Pass_Visitor::visit(const Declaration& decl)
{
     string lhs = canonicalize(decl.identifier);
     if(decl.init_status==Declaration::INITIALIZED)
          decl.rhs.expression->visit_with(*this);
     switch(decl.init_status)
     {
     case Declaration::UNINITIALIZED:
          if(symtab.count(lhs) && !postinitialized_vars.count(lhs))
               error_msgs.push_back("Variable "+decl.identifier+" redeclared");
          else if(!postinitialized_vars.count(lhs))
               uninitialized_vars.insert(lhs);
          else
               postinitialized_vars.erase(lhs);
          break;
     case Declaration::INITIALIZED:
          if(symtab.count(lhs) && lhs!="assert")
               error_msgs.push_back("Variable "+decl.identifier+" redeclared");
          get_symbol(lhs) = Value{Type::NONE};
          break;
     case Declaration::POST_INITIALIZED:
     case Declaration::SOLVER_INITIALIZED:
          if(symtab.count(lhs) || postinitialized_vars.count(lhs))
               error_msgs.push_back("Variable "+decl.identifier+" reinitialized");
          else if(!uninitialized_vars.count(lhs))
               postinitialized_vars.insert(lhs);
          else
               uninitialized_vars.erase(lhs);
          get_symbol(lhs) = Value{Type::NONE};
          break;
     }
}

void Symtab_Pass_Visitor::visit(If_Expression& ie)
{
     ie.primary_branch.condition->visit_with(*this);
     if(ie.primary_branch.assertion)
          ie.primary_branch.assertion->visit_with(*this);
     ie.primary_branch.value->visit_with(*this);
     for(auto& t : ie.elif_branch_list)
     {
          t.condition->visit_with(*this);
          if(t.assertion)
               t.assertion->visit_with(*this);
          t.value->visit_with(*this);
     }
     if(ie.else_branch_assert)
          ie.else_branch_assert->visit_with(*this);
     ie.else_branch_value->visit_with(*this);
}

void Symtab_Pass_Visitor::visit(Arithmetic_Expression& ae)
{
     ae.left_expression->visit_with(*this);
     if(ae.right_expression)
          ae.right_expression->visit_with(*this);
     if(ae.ternary_lt_token)
          ae.ternary_lt_expression->visit_with(*this);
}

void Symtab_Pass_Visitor::visit(Array_Expression& aexp)
{
     aexp.base->visit_with(*this);
     aexp.index->visit_with(*this);
     if(aexp.index2)
          aexp.index2->visit_with(*this);
}

void Symtab_Pass_Visitor::visit(Identifier_Expression& ie)
{
     string ieid = canonicalize(ie.identifier);
     if(!symtab.count(ieid) && (!local_symtabs.size() || !local_symtabs.top().count(ieid)))
          diagnostic_msgs.push_back("Variable "+ieid+" used before declaration");
}

void Symtab_Pass_Visitor::visit(Value_Expression& ve)
{
     if(ve.value.type!=Type::ARRAY)
          return;

     //Handle arrays
     Array* ary = ve.value.value.array;
     unordered_map<string,Value> local_symtab;
     local_symtab[ary->row_identifier];
     if(ary->column_identifier!="")
          local_symtab[ary->column_identifier];
     local_symtabs.push(local_symtab);

     for(auto e : ary->row_headers)
          e->visit_with(*this);
     for(auto e : ary->column_headers)
          e->visit_with(*this);
     for(auto e : ary->body)
          e->visit_with(*this);

     local_symtabs.pop();
}
