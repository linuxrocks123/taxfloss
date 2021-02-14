#pragma once

#include "Expression.hpp"
#include "MultiArray.hpp"

#include <list>
#include <string>
#include <vector>

struct Array
{
     std::list<Expression*> row_headers;
     std::string row_identifier;
     
     std::list<Expression*> column_headers;
     std::string column_identifier;
     
     std::vector<Expression*> body;

     MultiArray<Expression*,2>* d2_accessor = NULL;

     Expression* operator()(int single_d) { return body[single_d]; }
     
     //Note: must not change body after calling because keeps pointer to element inside vector
     Expression* operator()(int d1, int d2)
          {
               if(!d2_accessor)
                    d2_accessor = new MultiArray<Expression*,2>(&body[0],row_headers.size(),column_headers.size());
               return (*d2_accessor)[d1][d2];
          }

     ~Array() { delete d2_accessor; }
};
