#pragma once

#include <regex>

#include "Value.hpp"

using std::regex;
using std::regex_match;

struct Solver_Data
{
     Expression* to_zero;
     Expression* lower_bound;
     Expression* upper_bound;
     Expression* delta_min;
     Expression* epsilon_tol;
};

struct Regex_Data
{
     Type type;
     regex matcher;
     Expression* post_modifier_expression;
};

struct Declaration
{
     string identifier;
     enum { UNINITIALIZED, INITIALIZED, POST_INITIALIZED, SOLVER_INITIALIZED, REGEX_POST_MODIFIER} init_status;
     union
     {
          Type type;
          Expression* expression;
          Solver_Data* solver_data;
          Regex_Data* regex_data;
     } rhs;
     bool autorounding = false;

     Declaration(string identifier_, Type type_)
          : identifier(identifier_), init_status(UNINITIALIZED) { rhs.type = type_; }

     Declaration(string identifier_, Expression* expression_, bool post)
          : identifier(identifier_),
            init_status(post ? POST_INITIALIZED : INITIALIZED)
          {
               rhs.expression = expression_;
          }

     Declaration(string identifier_, Expression* z, Expression* l, Expression* u, Expression* d, Expression* e)
          : identifier(identifier_), init_status(SOLVER_INITIALIZED)
          {
               rhs.solver_data = new Solver_Data{z,l,u,d,e};
          }

     Declaration(string identifier_, Type type, const regex& matcher, Expression* r_exp)
          : identifier(identifier_), init_status(REGEX_POST_MODIFIER)
          {
               rhs.regex_data = new Regex_Data{type,matcher,r_exp};
          }
};
