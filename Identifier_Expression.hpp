#pragma once

#include "Expression.hpp"
#include <string>

struct Identifier_Expression : public Expression
{
     std::string identifier;

     Identifier_Expression(std::string id_) : identifier(id_) { }
     
     void visit_with(Expression_Visitor& v) { v.visit(*this); }
};
