#pragma once

#include <list>
#include <unordered_map>

#include "Declaration.hpp"
#include "Expression.hpp"
#include "Array_Expression.hpp"
#include "Arithmetic_Expression.hpp"
#include "Identifier_Expression.hpp"
#include "If_Expression.hpp"
#include "Value_Expression.hpp"
#include "Value.hpp"

#include <stack>

using std::list;
using std::stack;
using std::unordered_map;

extern list<Declaration>* root;
extern int current_line;

extern unordered_map<string,Value> symtab;
extern string current_file;
extern stack<unordered_map<string,Value>> local_symtabs;

extern int inject_herald;

string canonicalize(string identifier);
Value& get_symbol(string identifier);

string get_type(Type type);
string get_value(Value v);


#define YYDEBUG 1

#if DEBUG
#define YYDEBUG 1
#else
#define NDEBUG
#endif
