#pragma once

#include "Array.hpp"
#include "Number.hpp"

enum struct Type { BOOLEAN, NUMBER, ARRAY, NONE };
struct Value
{
     Type type;
     union
     {
          bool boolean;
          Number number;
          Array* array;
     } value;

     Value(bool b) : type(Type::BOOLEAN) { value.boolean = b; }
     Value(Number n) : type(Type::NUMBER) { value.number = n; }
     Value(Array* a) : type(Type::ARRAY) { value.array = a; }
     Value(Type t) : type(t) { }

     Value() = default;
};
