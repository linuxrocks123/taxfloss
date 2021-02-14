#include "taxicality_config.hpp"
#include "Print_Visitor.hpp"
#include "Symtab_Pass_Visitor.hpp"
#include "Executor.hpp"
#include "interpreter.hpp"

#include <queue>
#include <fstream>

using std::queue;

int main(int argc, char** argv)
{
     string print_symbol = argv[1];

     string errors = parse_and_check(argv + 2);
     if(errors!="")
     {
          cout << errors << endl;
          return 1;
     }

     errors = execute();
     if(errors!="")
     {
          cout << errors << endl;
          return 2;
     }
     
     if(print_symbol=="ALL")
          for(auto& i : symtab)
               cout << i.first << ": " << get_value(i.second) << endl;
     else if(symtab.count(print_symbol))
          cout << get_value(symtab[print_symbol]) << endl;
     else
     {
          cout << "Error: no such symbol in symbol table." << endl;
          return 4;
     }

     return 0;
}
