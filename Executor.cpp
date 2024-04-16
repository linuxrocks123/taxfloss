#include "Executor.hpp"
#include "taxicality.h"
#include "interpreter.hpp"

#include <cassert>
#include <cmath>
#include <sstream>

void Executor::visit(list<Declaration>& root)
{
     uninitializables.clear();
     regex_post_modifiers.clear();
     for(auto& decl : root)
     {
          try
          {
               visit(decl);
          }
          catch(uninitialized_value_exception e)
          {
               uninitializables.push_back(current_file+":"+e.identifier);
          }
     }
}

void Executor::visit(Declaration& decl)
{
     lhs = canonicalize(decl.identifier);
     if(lhs!="assert")
          last_real_decl = lhs;
     
     if(decl.init_status==Declaration::INITIALIZED || decl.init_status==Declaration::POST_INITIALIZED)
     {
          try
          {
               decl.rhs.expression->visit_with(*this);
          }
          catch(std::range_error e)
          {
               throw "Number error in declaration of "+lhs+": "+e.what();
          }
          catch(std::invalid_argument e)
          {
               throw "Number error in declaration of "+lhs+": "+e.what();
          }

          //Regex Post-Modifier Processing
          if(decl.autorounding)
          {
               bool rounding_performed = false;
               for(Declaration* post_modifier : regex_post_modifiers)
                    if(retval.type==post_modifier->rhs.regex_data->type && regex_match(decl.identifier,post_modifier->rhs.regex_data->matcher))
                    {
                         unordered_map<string,Value> local_symtab;
                         local_symtab[post_modifier->identifier] = retval;
                         local_symtabs.push(local_symtab);
                         post_modifier->rhs.regex_data->post_modifier_expression->visit_with(*this);
                         local_symtabs.pop();
                         rounding_performed = true;
                         break;
                    }

               if(!rounding_performed)
                    throw "Variable "+lhs+" declared with autorounding, but no matching regex post-modifier was found.";
          }
          
          get_symbol(lhs) = retval;
     }
     else if(decl.init_status==Declaration::SOLVER_INITIALIZED)
     {
          //Check to see if we are the real or hypothetical Executor
          if(decl.rhs.solver_data->lower_bound) //real
          {
               //If we already exist, skip this
               if(get_symbol(lhs).type!=Type::NONE)
                    return;
               
               Number lower_bound, upper_bound, delta_min, epsilon_tol;
               try
               {
                    decl.rhs.solver_data->lower_bound->visit_with(*this);
                    if(retval.type!=Type::NUMBER)
                         throw "Invalid lower bound for "+lhs;
                    lower_bound = retval.value.number;

                    decl.rhs.solver_data->upper_bound->visit_with(*this);
                    if(retval.type!=Type::NUMBER)
                         throw "Invalid upper bound for "+lhs;
                    upper_bound = retval.value.number;

                    decl.rhs.solver_data->delta_min->visit_with(*this);
                    if(retval.type!=Type::NUMBER)
                         throw "Invalid delta minimum for "+lhs;
                    delta_min = retval.value.number;

                    decl.rhs.solver_data->epsilon_tol->visit_with(*this);
                    if(retval.type!=Type::NUMBER)
                         throw "Invalid epsilon tolerance for "+lhs;
                    epsilon_tol = retval.value.number;
               }               
               catch(std::range_error e)
               {
                    throw "Number error in declaration of "+lhs+": "+e.what();
               }
               catch(std::invalid_argument e)
               {
                    throw "Number error in declaration of "+lhs+": "+e.what();
               }

               Value_Expression* ve = new Value_Expression{Value{}};
               Value& cur_val = ve->value;
               cur_val.type = Type::NUMBER;
               
               Solver_Data* orig_solver = decl.rhs.solver_data;
               Solver_Data temp_solver = *orig_solver;
               temp_solver.lower_bound = NULL;
               temp_solver.upper_bound = ve;
               decl.rhs.solver_data = &temp_solver;

               unordered_map<string,Value> real_symtab = symtab;
               auto perform_hypothetical = [&](Number n)
                    {
                         symtab.clear(); 
                         cur_val.value.number = n;
                         string errors;
                         try
                         {
                              errors = execute();
                         }
                         catch(Value v)
                         {
                              if(v.type!=Type::NUMBER)
                                   throw "invalid type for solver zero expression in declaration for "+lhs;
                              symtab = real_symtab;
                              return v.value.number;
                         }
                         assert(errors!="");
                         throw errors;
                    };

               Number y_lower = perform_hypothetical(lower_bound);
               Number y_upper = perform_hypothetical(upper_bound);
               Number y_current;
               bool pd; //positive derivative
               int8_t precision = (int8_t)(round(-log10(stod(to_string(delta_min))))) + 2;

               if(abs(y_lower) - epsilon_tol <= 0)
               {
                    cur_val.value.number = y_lower;
                    goto coda;
               }
               else if(abs(y_upper) - epsilon_tol <= 0)
               {
                    cur_val.value.number = y_upper;
                    goto coda;
               }
               else if(y_lower > 0 && y_upper < 0)
                    pd = false;
               else if(y_lower < 0 && y_upper > 0)
                    pd = true;
               else
                    throw "invalid range for solver in declaration for "+lhs;

               do
               {
                    Number n = (lower_bound + upper_bound)/2;
                    n.round(precision);
                    y_current = perform_hypothetical(n);

                    if(y_current < 0 && pd || y_current > 0 && !pd)
                         lower_bound = n;
                    else
                         upper_bound = n;

                    if(upper_bound - lower_bound < delta_min)
                         throw "Solver could not find solution in declaration for "+lhs;
               } while(abs(y_current) - epsilon_tol > 0);
               
          coda:
               decl.rhs.solver_data = orig_solver;
               get_symbol(lhs) = cur_val;               
          }
          else //hypothetical
          {
               Declaration d {decl.identifier,decl.rhs.solver_data->upper_bound,true};
               visit(d);
               d.identifier = "solver expression for "+decl.identifier;
               d.rhs.expression = decl.rhs.solver_data->to_zero;
               visit(d);
               throw retval;
          }
     }
     else if(decl.init_status==Declaration::REGEX_POST_MODIFIER)
          regex_post_modifiers.push_back(&decl);

     if(lhs=="assert")
          if(retval.type!=Type::BOOLEAN)
               throw "Assertion statement following declaration of "+last_real_decl+" not of boolean type.";
          else if(!retval.value.boolean)
               throw "Assertion following declaration of "+last_real_decl+" failed.";
}

void Executor::visit(If_Expression& ie)
{
     static Value_Expression TRUTH = {Value{true}};
     list<If_Triple> all_branches = ie.elif_branch_list;
     all_branches.push_front(ie.primary_branch);
     all_branches.push_back(If_Triple{&TRUTH,ie.else_branch_assert,ie.else_branch_value});
     for(If_Triple& branch : all_branches)
     {
          
          branch.condition->visit_with(*this);
          if(retval.type!=Type::BOOLEAN)
               throw "Invalid type for if expression condition in declaration for "+lhs;
          if(retval.value.boolean)
          {
               if(branch.assertion)
                    branch.assertion->visit_with(*this);
               if(retval.type!=Type::BOOLEAN)
                    throw "Invalid type for if expression assertion in declaration for "+lhs;
               if(!retval.value.boolean)
                    throw "Assertion failed for if expression in declaration for "+lhs;
               branch.value->visit_with(*this);
               return;
          }
     }
}

void Executor::visit(Arithmetic_Expression& ae)
{
     Value to_return;
     auto assert_type = [&](Type t)
                             {
                                  if(to_return.type!=t || retval.type!=t)
                                       throw "Invalid type for arithmetic expression in declaration for "+lhs;
                             };

     ae.left_expression->visit_with(*this);
     to_return = retval;
     if(ae.right_expression)
          ae.right_expression->visit_with(*this);
     switch(ae.token)
     {
     case '<': assert_type(Type::NUMBER);
          to_return.type = Type::BOOLEAN;
          to_return.value.boolean = to_return.value.number < retval.value.number;
          break;
     case LT_EQUALS: assert_type(Type::NUMBER);
          to_return.type = Type::BOOLEAN;
          to_return.value.boolean = to_return.value.number <= retval.value.number;
          break;
     case '>': assert_type(Type::NUMBER);
          to_return.type = Type::BOOLEAN;
          to_return.value.boolean = to_return.value.number > retval.value.number;
          break;
     case GT_EQUALS: assert_type(Type::NUMBER);
          to_return.type = Type::BOOLEAN;
          to_return.value.boolean = to_return.value.number >= retval.value.number;
          break;
     case EQUALS: if(to_return.type!=retval.type)
               throw "Attempted to compare values of different types for equality in declaration for "+lhs;
          if(to_return.type==Type::ARRAY)
               throw "Attempted to compare two arrays for equality in declaration for "+lhs;
          if(to_return.type==Type::BOOLEAN)
               to_return.value.boolean = to_return.value.boolean == retval.value.boolean;
          else
               to_return.value.boolean = to_return.value.number == retval.value.number;
          to_return.type = Type::BOOLEAN;
          break;
     case NOT_EQUALS: if(to_return.type!=retval.type)
               throw "Attempted to compare values of different types for inequality in declaration for "+lhs;
          if(to_return.type==Type::ARRAY)
               throw "Attempted to compare two arrays for inequality in declaration for "+lhs;
          if(to_return.type==Type::BOOLEAN)
               to_return.value.boolean = to_return.value.boolean != retval.value.boolean;
          else
               to_return.value.boolean = to_return.value.number != retval.value.number;
          to_return.type = Type::BOOLEAN;
          break;
     case AND: assert_type(Type::BOOLEAN);
          to_return.type = Type::BOOLEAN;
          to_return.value.boolean = to_return.value.boolean && retval.value.boolean;
          break;
     case OR: assert_type(Type::BOOLEAN);
          to_return.type = Type::BOOLEAN;
          to_return.value.boolean = to_return.value.boolean || retval.value.boolean;
          break;
     case NOT: if(to_return.type!=Type::BOOLEAN)
               throw "NOT operator used with non-boolean in declaration for "+lhs;
          to_return.type = Type::BOOLEAN;
          to_return.value.boolean = !to_return.value.boolean;
          break;
     case '+': assert_type(Type::NUMBER);
          to_return.type = Type::NUMBER;
          to_return.value.number = to_return.value.number + retval.value.number;
          break;
     case '-': assert_type(Type::NUMBER);
          to_return.type = Type::NUMBER;
          to_return.value.number = to_return.value.number - retval.value.number;
          break;
     case '*': assert_type(Type::NUMBER);
          to_return.value.number = to_return.value.number * retval.value.number;
          break;
     case '/': assert_type(Type::NUMBER);
          to_return.type = Type::NUMBER;
          to_return.value.number = to_return.value.number / retval.value.number;
          break;
     case '\\': assert_type(Type::NUMBER);
          to_return.type = Type::NUMBER;
          to_return.value.number.round(retval.value.number);
          break;
     case TRUNCATE: assert_type(Type::NUMBER);
          to_return.type = Type::NUMBER;
          to_return.value.number.truncate(retval.value.number);
          break;
     case ROUNDUP: assert_type(Type::NUMBER);
          to_return.type = Type::NUMBER;
          to_return.value.number.roundup(retval.value.number);
          break;
     }
     if(ae.ternary_lt_token)
          if(to_return.value.boolean)
          {
               Arithmetic_Expression{ae.right_expression,ae.ternary_lt_token,ae.ternary_lt_expression}.visit_with(*this);
               to_return.value.boolean &= retval.value.boolean;
          }
     retval = to_return;
}

void Executor::visit(Array_Expression& ae)
{
     ae.base->visit_with(*this);
     if(retval.type!=Type::ARRAY)
          throw "Attempted to index non-array in declaration for "+lhs;
     Array* ary = retval.value.array;

     string ary_file;
     if(last_id_lookup.find('.')!=string::npos)
          ary_file = last_id_lookup.substr(0,last_id_lookup.find('.'));
     else
          ary_file = current_file;

     ae.index->visit_with(*this);
     if(retval.type!=Type::NUMBER)
          throw "Index not a number in declaration for "+lhs;
     Number index1 = retval.value.number;

     if(ary->column_identifier!="" && !ae.index2)
          throw "No second index for 2D array in declaration for "+lhs;
     if(ary->column_identifier=="" && ae.index2)
          throw "Second index for 1D array in declaration for "+lhs;

     Number index2;
     if(ary->column_identifier!="")
     {
          ae.index2->visit_with(*this);
          index2 = retval.value.number;
     }

     unordered_map<string,Value> local_symtab;
     local_symtab[ary->row_identifier] = index1;
     if(ary->column_identifier!="")
          local_symtab[ary->column_identifier] = index2;
     local_symtabs.push(local_symtab);

     string old_current_file = current_file;
     current_file = ary_file;

     struct Finally
     {
          const string& old_current_file;
          Finally(const string& old_current_file_) :
               old_current_file(old_current_file_) { }
          ~Finally()
               {
                    local_symtabs.pop();
                    current_file = old_current_file;
               }
     } finally(old_current_file);

     int d1 = 0;
     bool found = false;
     for(auto e : ary->row_headers)
     {
          e->visit_with(*this);
          if(retval.type==Type::NUMBER && retval.value.number==index1 || retval.type==Type::BOOLEAN && retval.value.boolean)
          {
               found = true;
               break;
          }
          d1++;
     }
     if(!found)
          throw "Invalid first index in array access in declaration for "+lhs;

     if(ary->column_identifier=="")
          (*ary)(d1)->visit_with(*this);
     else
     {
          int d2 = 0;
          found = false;
          for(auto e : ary->column_headers)
          {
               e->visit_with(*this);
               if(retval.type==Type::NUMBER && retval.value.number==index2 || retval.type==Type::BOOLEAN && retval.value.boolean)
               {
                    found = true;
                    break;
               }
               d2++;
          }
          if(!found)
               throw "Invalid second index in array access in declaration for "+lhs;
          (*ary)(d1,d2)->visit_with(*this);
     }
}

void Executor::visit(Identifier_Expression& ie)
{
     last_id_lookup = ie.identifier;
     retval = get_symbol(ie.identifier);
     if(retval.type==Type::NONE)
          throw uninitialized_value_exception{ie.identifier};
}

void Executor::visit(Value_Expression& ve)
{
     retval = ve.value;
}
