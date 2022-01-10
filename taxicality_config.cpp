#include "taxicality_config.hpp"

#include <iostream>
#include <sstream>
#include <stack>

using std::cout;
using std::endl;
using std::stack;

std::list<Declaration>* root;
int current_line = 1;

unordered_map<string,Value> symtab;
string current_file;
stack<unordered_map<string,Value>> local_symtabs;
int inject_herald = 0;

string canonicalize(string identifier)
{
     if(identifier=="assert")
          return identifier;
     if(local_symtabs.size())
     {
          unordered_map<string,Value>& local_symtab = local_symtabs.top();
          if(local_symtab.count(identifier))
               return identifier;
     }
     return identifier.find('.')!=string::npos ? identifier : (current_file+'.'+identifier);
}

Value& get_symbol(string identifier)
{
     Value none = Value{Type::NONE};
     string cid = canonicalize(identifier);
     if(cid.find('.')==string::npos && cid!="assert")
     {
          if(!local_symtabs.top().count(cid))
               local_symtabs.top()[cid] = none;
          return local_symtabs.top()[cid];
     }
     else
     {
          if(!symtab.count(cid))
               symtab[cid] = none;
          return symtab[cid];
     }
}

string get_type(Type type)
{
     switch(type)
     {
     case Type::BOOLEAN: return "BOOLEAN";
     case Type::NUMBER: return "NUMBER";
     case Type::ARRAY: return "ARRAY";
     default: return "unknown type";
     }
}

string get_value(Value v)
{
     ostringstream out;
     out << get_type(v.type) << ": ";
     switch(v.type)
     {
     case Type::BOOLEAN:
          if(v.value.boolean)
               out << "true";
          else
               out << "false";
          break;
     case Type::NUMBER:
          out << v.value.number;
          break;
     case Type::ARRAY:
          out << "(unimplemented)"; //TODO
     }

     return out.str();
}

extern "C" int yywrap()
{
     //For now
     return 1;
}

vector<string>* error_msgs;
int yyerror(const char* msg_)
{
     string msg = "Line "+to_string(current_line)+": "+msg_;
     if(error_msgs)
          error_msgs->push_back(msg);
     else
          cout << msg << endl;
     return 0;
}
