#include "interpreter.hpp"
#include "taxicality_config.hpp"

#include "Executor.hpp"
#include "Symtab_Pass_Visitor.hpp"

#include <fstream>
#include <queue>

using namespace std;

extern int yydebug;
extern FILE* yyin;
extern int yyparse();

map<string,list<Declaration>*> ast_roots;

static string chop_extension(string x)
{
     return x.substr(0,x.find('.'));
}

static void dump_symtab()
{
     ofstream fout("core");
     fout << "Debugging symbol table:";
     for(auto& i : symtab)
          fout << i.first << ": " << get_value(i.second) << endl;
}

string parse_and_check(char** filenames)
{
#if DEBUG
     yydebug = 1;
#endif

     Symtab_Pass_Visitor gen_syms;
     string to_return = "";
     while(*filenames)
     {
          yyin = fopen(*filenames,"r");
          current_file = chop_extension(*filenames);
          int result = yyparse();
          if(result)
          {
               to_return = string{"Parsing file "}+*filenames+" failed.";
               return to_return;
          }
          
          //Print_Visitor print_me;
          //print_me.visit(*root);
          ast_roots[current_file] = root;
          gen_syms.visit(*root);

          filenames++;
     }

     if(gen_syms.uninitialized_vars.size() || gen_syms.postinitialized_vars.size())
     {
          to_return = "Uninitialized variables:";
          for(const auto& x : gen_syms.uninitialized_vars)
               to_return+="\n"+x;
          to_return+="\nVariables postinitialized without corresponding declaration:";
          for(const auto& x : gen_syms.postinitialized_vars)
               to_return+="\n"+x;
     }
     else if(gen_syms.error_msgs.size())
     {
          to_return = "Errors:";
          for(const auto& x : gen_syms.error_msgs)
               to_return+="\n"+x;
     }

     return to_return;
}

string execute()
{
     string to_return = "";     
     queue<pair<string,list<string>>> execute_queue;
     for(const auto& file : ast_roots)
          execute_queue.push(make_pair(file.first,list<string>{}));
     
     Executor executor;
     int death_is_coming = 0;
     while(execute_queue.size())
     {
          current_file = execute_queue.front().first;
          try
          {
               executor.visit(*ast_roots[current_file]);
          }
          catch(string e)
          {
               dump_symtab();
               return e;
          }
          catch(const char* e)
          {
               dump_symtab();
               return e;
          }
          if(executor.uninitializables.size() && executor.uninitializables.size()==execute_queue.front().second.size())
          {
               if(death_is_coming==execute_queue.size())
               {
                    to_return = "Uninitializable variables:";
                    while(execute_queue.size())
                    {
                         for(auto& x : execute_queue.front().second)
                              to_return+="\n"+x;
                         execute_queue.pop();
                    }
                    return to_return;
               }
               else
                    death_is_coming++;
          }
          else
               death_is_coming = 0;
          if(executor.uninitializables.size())
               execute_queue.push(make_pair(current_file,executor.uninitializables));
          execute_queue.pop();
     }
     
     return to_return;
}
