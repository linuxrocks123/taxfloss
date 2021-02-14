#include "taxicality_config.hpp"
#include "Print_Visitor.hpp"
#include "Symtab_Pass_Visitor.hpp"
#include "interpreter.hpp"

#include <queue>

using std::queue;

extern FILE* yyin;
extern int yydebug;
extern int yyparse();

static string chop_extension(string x)
{
     return x.substr(0,x.find('.'));
}

int main(int argc, char** argv)
{
#if DEBUG
     yydebug = 1;
#endif

     string vary_symbol = argv[1];
     Number range_min{argv[2]};
     Number range_max{argv[3]};
     string print_symbol = argv[4];
     int delta = stoi(argv[5]);
     string errors = parse_and_check(argv + 6);
     if(errors!="")
     {
          cout << errors << endl;
          return 1;
     }
     
     cout << '#' << vary_symbol << ',' << print_symbol << ',' << "Marginal Rate\n";
     Number old_tax = 0;

     Value_Expression* to_modify = NULL;
     for(auto& entry : ast_roots)
          for(Declaration& decl : *(entry.second))
               if(decl.init_status==Declaration::POST_INITIALIZED && (decl.identifier==vary_symbol || entry.first+"."+decl.identifier==vary_symbol))
               {
                    to_modify = dynamic_cast<Value_Expression*>(decl.rhs.expression);
                    break;
               }

     if(to_modify==NULL)
     {
          cout << "No post-initialization for varied symbol " << vary_symbol << endl;
          return 6;
     }
     
     for(Number cur_val = range_min; cur_val <= range_max; cur_val += delta)
     {
          symtab.clear();
          to_modify->value = Value{cur_val};

          errors = execute();
          if(errors!="")
          {
               cout << errors << endl;
               return 2;
          }

          if(symtab.count(print_symbol))
          {
               Number to_print = (symtab[print_symbol].value.number-old_tax)/delta;
               to_print.round(3);
               cout << cur_val << ',' << symtab[print_symbol].value.number << ',' << to_print << endl;
          }
          else
          {
               cout << "Error: no such symbol in symbol table." << endl;
               return 3;
          }
          
          old_tax = symtab[print_symbol].value.number;
     }
     
     return 0;
}
