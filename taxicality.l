%{
#include "taxicality_config.hpp"
#include "taxicality.h"
#include "Value.hpp"
//#include "symtab.hpp"

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

%{
  if(inject_herald)
  {
       int to_inject = inject_herald;
       inject_herald = 0;
       return to_inject;
  }
%}

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

assert { return ASSERT; }
ZEROES { return ZEROES; }
RANGE { return RANGE; }
DELTA { return DELTA; }
TOLERANCE { return TOLERANCE; }

BOOLEAN { return BOOLEAN; }
NUMBER { return NUMBER; }

{integer}|{fpoint} { string to_convert = StringFunctions::replace(yytext,"$","");
            to_convert = StringFunctions::replace(to_convert,",","");
            yylval.value = Value(Number(to_convert));
            return LITERAL;
          }

 /*Assignment*/
:=  { return UNINITDEC; }
=   { return '='; }
\<- { return INITIALIZE; }

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

 /*Logic*/
and { return AND; }
or { return OR; }
not { return NOT; }

 /*Punctuation*/
\. { return '.'; }
\,  { return ','; }
:  { return ':'; }
\[ { return '['; }
\] { return ']'; }
\| { return '|'; }
\( { return '('; }
\) { return ')'; }
\{ { return '{'; }
\} { return '}'; }

{id} { yylval.identifier = new string{yytext};
       return IDENTIFIER;
     }

\" { char c = yyinput();
     while(c!='\"')
          if(c=='\n' || c=='\0')
          {
               yyerror("Lexer: unterminated comment literal.");
               break;
          }
          else
               c = yyinput();
   }
