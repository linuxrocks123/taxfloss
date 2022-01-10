%{
#include "tuidl_config.hpp"
#include "tuidl.h"
#include "Value.hpp"

#include "StringFunctions.h"

int yyerror(const char* e_str);
%}

delim   [ \t]
ws      {delim}+
letter  [A-Za-z_]
digit   [0-9$]
id      {letter}({letter}|{digit})*
integer {digit}+
fpoint  {integer}+\.{integer}+

%%

{ws} { /*no code needed*/ }
\n { current_line++; }
;  { return EOF; }

if { return IF; }
elif { return ELIF; }
else { return ELSE; }

true { yylval.value = Value(true);
       return LITERAL;
     }

false { yylval.value = Value(false);
        return LITERAL;
      }

{integer}|{fpoint} { string to_convert = StringFunctions::replace(yytext,"$","");
            to_convert = StringFunctions::replace(to_convert,",","");
            yylval.value = Value(Number(to_convert));
            return LITERAL;
          }

 /*Assignment*/
=   { return '='; }
\+= { return PLUSEQUALS; }

 /*Computation*/
\\ { return '\\'; }
\\_ { return TRUNCATE; }
\\^ { return ROUNDUP; }
\+ { return '+'; }
-  { return '-'; }
\* { return '*'; }
\/ { return '/'; }

 /*Comparison*/
\< { return '<'; }
\<= { return LT_EQUALS; }
== { return EQUALS; }
\>= { return GT_EQUALS; }
\> { return '>'; }
!= { return NOT_EQUALS; }

 /*Keywords*/
dialog { return DIALOG; }
for { return FOR; }
form { return FORM; }
nonunique { return NONUNIQUE; }
stub { return STUB; }
unique { return UNIQUE; }


 /*Logic*/
and { return AND; }
or { return OR; }
not { return NOT; }

 /*Punctuation*/
\. { return '.'; }
:: { return DOUBLECOLON; }
\{ { return '{'; }
\} { return '}'; }

{id} { yylval.identifier = new string{yytext};
       return IDENTIFIER;
     }

\" { yylval.identifier = new string{};
     bool last_was_backslash = false;
     for(char c = yyinput(); !last_was_backslash || c!='\"'; c = yyinput())
     {
          if(c=='\n' && !last_was_backslash || c=='\0')
          {
               yyerror("Lexer: unterminated string literal.");
               break;
          }
          if(c!='\\' || last_was_backslash)
          {
               *(yylval.identifer) += c;
               last_was_backslash = false;
          }
          else
               last_was_backslash = true;
     }
     return STRING;
   }
