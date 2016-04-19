/*
* Heather Bradfield
* Lab 10 Task 9
* 4/18/16
*/

/*

s-expressions calculator

program ::= program s-expr
s-expr ::= number
| symbol 
| (func)
| ( func s-expr ) 
| ( func s-expr s-expr ) 
| ( cond s_expr s_expr s_expr )
| ( ( let let_list ) s_expr )

let_list ::= let_elem | let_list let_elem
let_elem ::= ( type symbol s_expr ) //
type ::= | integer | real //
symbol ::= letter+
func ::= neg|abs|exp|sqrt|add|sub|mult|div|remainder|log|pow|max|min|exp2|cbrt|hypot|print|equal|smaller|larger|rand|read
letter ::= [a-zA-Z]
number ::= [ + | - ] digit+ [ . digit+ ] 
digit ::= [0-9]

*/

%{
#include "l10t9.h"
%}

%union
{
   double dval;
   char *sval;
   struct ast_node *astNode;
};

%token <sval> FUNC
%token <sval> COND
%token <dval> NUMBER
%token <sval> SYMBOL
%token <dval> TYPE
%token LET LPAREN RPAREN EOL QUIT

%type <astNode> s_expr
%type <astNode> let_elem
%type <astNode> let_list

%%

program:/* empty */ { 
                       printf("> ");
                    }
        | program s_expr EOL {
            // printf("yacc: program expr\n");

            typeReset();
            printResult(eval($2));
            freeNode($2);
            printf("\n> "); 
         }
        ;

s_expr:
        NUMBER { 
            //printf("yacc: NUMBER%lf", $1); 
            $$ = number($1); 
        }
        | SYMBOL {
            $$ = symbol($1);
        }
        | LPAREN FUNC RPAREN {
            //printf("yacc: LPAREN FUNC RPAREN\n");
            $$ = function($2, 0, 0);
        }
        | LPAREN FUNC s_expr RPAREN { 
            // printf("yacc: LPAREN FUNC expr RPAREN\n"); 
            $$ = function($2, $3, 0);  
            //printf("%s(%lf)", $2, $3);
        }
        | LPAREN FUNC s_expr s_expr RPAREN {
            // printf("LPAREN FUNC expr expr RPAREN\n"); 
            // $$ = calc($2, $3, $4); 
            $$ = function($2, $3, $4);
        }
        | LPAREN COND s_expr s_expr s_expr RPAREN {
            //printf("LPAREN COND s_expr s_expr s_expr RPAREN\n");
            //$$ = calc($2, $3, $4);
            $$ = condition($3, $4, $5);
        }
        | LPAREN LPAREN LET let_list RPAREN s_expr RPAREN {
            $$ = let($4, $6);
        }
        | QUIT { 
            //printf("QUIT\n"); 
            exit(0);
         }
        
         | error { 
            //printf("error\n"); 
            //printf("> ");
         }

        ;

let_elem:
         LPAREN TYPE SYMBOL s_expr RPAREN {
            $$ = let_elem($2, $3, $4);
         }
         | LPAREN SYMBOL s_expr RPAREN {
            $$ = let_elem(2, $2, $3);
            //$$ = let_elem($2, $3);
         }

let_list:
         let_elem {
            $$ = $1;
         }
         | let_list let_elem {
            $$ = let_list($1, $2);
         }
  
%%
