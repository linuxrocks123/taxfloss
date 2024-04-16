/*taxicality.y -- Yacc grammar file for (simplified) C++/Java interpreter*/
//%glr-parser
%define parse.error verbose

%{
#include "taxicality_config.hpp"

using std::list;

int yylex();
int yyerror(const char* e_str);

extern list<Declaration>* root;
%}

%union
{
    list<Declaration>* decl_list;
    Declaration* declaration;
    Expression* expression;
    list<If_Triple>* if_triple_list;
    If_Triple if_triple;
    Type type;
    Value value;
    Array* array;
    struct
    {
         Expression* first;
         Expression* second;
    } array_expression_pair;

    int token;
    string* identifier;
}

%precedence ELSE
%left OR
%left AND
%left EQUALS NOT_EQUALS
%left '<' LT_EQUALS GT_EQUALS '>'
%precedence TERNARY_LT_RULE
%left '+' '-'
%left '*' '/' '\\' TRUNCATE ROUNDUP
%right NOT UMINUS_RULE
%precedence IDENTIFIER
%precedence '.'

%token BOOLEAN
%token NUMBER

%token IF
%token ELIF
%token ASSERT

%token <value> LITERAL
%token <identifier> IDENTIFIER;
%token <identifier> REGEX;

%token UNINITDEC ":="
%token INITIALIZE "<-"
%token APPROX_INITIALIZE "<~"

%token ZEROES
%token RANGE
%token DELTA
%token TOLERANCE

%token OF
%token PATTERN

%token INTERACTIVE_HERALD

%type <decl_list> Program Decl_List
%type <declaration> Declaration
%type <type> Type
%type <expression> Expression Optional_Assert Identifier Array_2D_Entry
%type <if_triple_list> Maybe_Elifs Elif_List
%type <if_triple> Elif
%type <array> Array_1D_Body Array_2D_Row_List Array_2D_Entry_List
%type <array_expression_pair> Array_1D_Expression_Pair

%start Program

%%

Program:        Decl_List { root = $1; }
        |       INTERACTIVE_HERALD Decl_List { root = $2; }
        |       INTERACTIVE_HERALD Expression
                {
                    root = new list<Declaration>();
                    root->push_back(Declaration{"@",$2,false});
                };
                
Decl_List:      Decl_List Declaration { $1->push_back(*$2); $$ = $1; }
        |       Declaration
                {
                    list<Declaration>* dlist = new list<Declaration>;
                    dlist->push_back(*$1);
                    $$ = dlist;
                };

Declaration:    IDENTIFIER ":=" Type { $$ = new Declaration{*$1,$3}; }
        |       IDENTIFIER '=' Expression { $$ = new Declaration{*$1,$3,false}; }
        |       IDENTIFIER '~' Expression { $$ = new Declaration{*$1,$3,false}; $$->autorounding=true; }
        |       Identifier "<-" Expression { $$ = new Declaration{static_cast<Identifier_Expression*>($1)->identifier,$3,true}; }
        |       Identifier "<~" Expression { $$ = new Declaration{static_cast<Identifier_Expression*>($1)->identifier,$3,true}; $$->autorounding=true; }
        |       ASSERT '(' Expression ')' { $$ = new Declaration{"assert",$3,false}; }
        |       Identifier "<-" ZEROES Expression RANGE '[' Expression ',' Expression ']' DELTA Expression TOLERANCE Expression
                {
                    $$ = new Declaration{static_cast<Identifier_Expression*>($1)->identifier,$4,$7,$9,$12,$14};
                }
        |       Type IDENTIFIER OF PATTERN REGEX '=' Expression { $$ = new Declaration{*$2,$1,regex{*$5,std::regex_constants::egrep},$7}; };

Type:           BOOLEAN { $$ = Type::BOOLEAN; }
        |       NUMBER { $$ = Type::NUMBER; };

Expression:     IF '(' Expression ')' Optional_Assert Expression Maybe_Elifs ELSE Optional_Assert Expression
                {
                    $$ = new If_Expression{If_Triple{$3,$5,$6},std::move(*$7),$9,$10};
                }
        |       Expression '<' Identifier '<' Expression %prec TERNARY_LT_RULE
                {
                    $$ = new Arithmetic_Expression{$1,'<',$3,'<',$5};
                }
        |       Expression '<' Identifier LT_EQUALS Expression %prec TERNARY_LT_RULE
                {
                    $$ = new Arithmetic_Expression{$1,'<',$3,LT_EQUALS,$5};
                }
        |       Expression LT_EQUALS Identifier '<' Expression %prec TERNARY_LT_RULE
                {
                    $$ = new Arithmetic_Expression{$1,LT_EQUALS,$3,'<',$5};
                }
        |       Expression LT_EQUALS Identifier LT_EQUALS Expression %prec TERNARY_LT_RULE
                {
                    $$ = new Arithmetic_Expression{$1,LT_EQUALS,$3,LT_EQUALS,$5};
                }
        |       Expression '<' Expression { $$ = new Arithmetic_Expression{$1, '<', $3}; }
        |       Expression LT_EQUALS Expression { $$ = new Arithmetic_Expression{$1, LT_EQUALS, $3}; }
        |       Expression '>' Expression { $$ = new Arithmetic_Expression{$1, '>', $3}; }
        |       Expression GT_EQUALS Expression { $$ = new Arithmetic_Expression{$1, GT_EQUALS, $3}; }
        |       Expression EQUALS Expression { $$ = new Arithmetic_Expression{$1, EQUALS, $3}; }
        |       Expression NOT_EQUALS Expression { $$ = new Arithmetic_Expression{$1, NOT_EQUALS, $3}; }
        |       Expression AND Expression { $$ = new Arithmetic_Expression{$1, AND, $3}; }
        |       Expression OR Expression { $$ = new Arithmetic_Expression{$1, OR, $3}; }
        |       NOT Expression { $$ = new Arithmetic_Expression{$2,NOT,NULL}; }
        |       '-' Expression %prec UMINUS_RULE { $$ = new Arithmetic_Expression{new Value_Expression{Value(Number(0L))},'-',$2}; }
        |       Expression '+' Expression { $$ = new Arithmetic_Expression{$1, '+', $3}; }
        |       Expression '-' Expression { $$ = new Arithmetic_Expression{$1, '-', $3}; }
        |       Expression '*' Expression { $$ = new Arithmetic_Expression{$1, '*', $3}; }
        |       Expression '/' Expression { $$ = new Arithmetic_Expression{$1, '/', $3}; }
        |       Expression '\\' Expression { $$ = new Arithmetic_Expression{$1, '\\', $3}; }
        |       Expression TRUNCATE Expression { $$ = new Arithmetic_Expression{$1, TRUNCATE, $3}; }
        |       Expression ROUNDUP Expression { $$ = new Arithmetic_Expression{$1, ROUNDUP, $3}; }
        |       '[' IDENTIFIER ']' '{' Array_1D_Body '}'
                {
                    Value v{$5};
                    $5->row_identifier = *$2;
                    $$ = new Value_Expression{v};
                }
        |       '[' IDENTIFIER ']' '[' IDENTIFIER ']' '{' '|' Array_2D_Entry_List '|' Array_2D_Row_List '}'
                {
                    Value v{$11};
                    $11->column_headers = std::move($9->row_headers);
                    $11->column_headers.insert($11->column_headers.end(),$9->body.begin(),$9->body.end());
                    $11->row_identifier = *$2;
                    $11->column_identifier = *$5;
                    $$ = new Value_Expression{v};
                }
        |       '(' Expression ')' { $$ = $2; }
        |       Identifier '[' Expression ']' { $$ = new Array_Expression{$1,$3}; }
        |       Identifier '[' Expression ']' '[' Expression ']' { $$ = new Array_Expression{$1,$3,$6}; }
        |       Identifier { $$ = $1; }
        |       LITERAL { $$ = new Value_Expression{$1}; };

Maybe_Elifs:    Elif_List { $$ = $1; }
        | /*    empty*/ { $$ = new list<If_Triple>{}; };

Elif_List:      Elif_List Elif { $1->push_back($2); $$ = $1; }
        |       Elif { $$ = new list<If_Triple>{}; $$->push_back($1); };

Elif:           ELIF '(' Expression ')' Optional_Assert Expression { $$ = If_Triple{$3,$5,$6}; };

Optional_Assert: ASSERT '(' Expression ')' { $$ = $3; }
        | /*    empty*/ { $$ = NULL; };

Array_1D_Body:  Array_1D_Body ',' Array_1D_Expression_Pair
                {
                    $1->row_headers.push_back($3.first);
                    $1->body.push_back($3.second);
                    $$ = $1;
                }
        |       Array_1D_Expression_Pair
                {
                    Array* ary = new Array;
                    ary->row_headers.push_back($1.first);
                    ary->body.push_back($1.second);
                    $$ = ary;
                };

Array_1D_Expression_Pair: Expression ':' Expression { $$ = {$1,$3}; };

Array_2D_Row_List: Array_2D_Row_List '|' Array_2D_Entry_List
                {
                    $1->row_headers.push_back($3->row_headers.front());
                    $1->body.insert($1->body.end(),$3->body.begin(),$3->body.end());
                    $$ = $1;
                }
        |       Array_2D_Entry_List { $$ = $1; };

Array_2D_Entry_List: Array_2D_Entry_List Array_2D_Entry
                {
                    $1->body.push_back($2);
                    $$ = $1;
                }
        |       Array_2D_Entry
                {
                    Array* ary = new Array;
                    ary->row_headers.push_back($1);
                    $$ = ary;
                };

Array_2D_Entry: Expression '|' { $$ = $1; };

Identifier:     IDENTIFIER '.' IDENTIFIER { $$ = new Identifier_Expression{*$1+'.'+*$3}; }
        |       IDENTIFIER { $$ = new Identifier_Expression{*$1}; };
