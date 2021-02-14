#include "taxicality_config.hpp"
#include "Print_Visitor.hpp"
#include "Symtab_Pass_Visitor.hpp"
#include "Executor.hpp"

#include "taxicality.h"

#include <queue>

using std::queue;

extern FILE* yyin;
extern int yydebug;

void yyrestart(FILE* new_file);

int main(int argc, char** argv)
{
#if DEBUG
     yydebug = 1;
#endif

     Symtab_Pass_Visitor gen_syms;
     Executor executor;
     while(true)
     {
          yyrestart(stdin);
          inject_herald = INTERACTIVE_HERALD;
          int retval = yyparse();
          if(retval)
          {
               cout << "Parse failed." << endl;
               continue;
          }

          #if 0
          gen_syms.visit(*root);
          if(gen_syms.uninitialized_vars.size())
          {
               cout << "Uninitialized variables:" << endl;
               for(const auto& x : gen_syms.uninitialized_vars)
                    cout << x << endl;
               continue;
          }
          else if(gen_syms.error_msgs.size())
          {
               cout << "Errors:" << endl;
               for(const auto& x : gen_syms.error_msgs)
                    cout << x << endl;
               continue;
          }
          #endif

          try
          {
               executor.visit(*root);
          }
          catch(Executor::uninitialized_value_exception e)
          {
                    cout << "Uninitializable variable: " << e.identifier << endl;
                    continue;
          }
          catch(string e)
          {
               cout << e << endl;
               continue;
          }
          catch(const char* e)
          {
               cout << e << endl;
               continue;
          }

          string value = get_value(symtab["."+root->front().identifier]);
          value = value.substr(value.find(":")+2);
          cout << value << endl;
          symtab["._"] = symtab[".@"];
          symtab.erase(".@");
     }
     
     return 0;
}
