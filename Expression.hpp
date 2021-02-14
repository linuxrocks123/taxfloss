#pragma once

#include "Expression_Visitor.hpp"

struct Expression
{
     virtual void visit_with(Expression_Visitor& v) = 0;
};
