/*
* Heather Bradfield
* Lab 10 Task 9
* 4/15/16
*/

%option noyywrap

%{
    #include "l10t9.h"
%}

digit [0-9]
number (-)?{digit}+(\.{digit}+)?
int_type "integer"|"int"
letter [a-zA-Z]
symbol {letter}+
func "neg"|"abs"|"exp"|"sqrt"|"add"|"sub"|"mult"|"div"|"remainder"|"log"|"pow"|"max"|"min"|"exp2"|"cbrt"|"hypot"|"print"|"equal"|"smaller"|"larger"|"rand"|"read"

%%

{number} {
                yylval.dval = strtod(yytext, NULL);
                //printf("lex: NUMBER dval = %lf\n", yylval.dval);
                return NUMBER;
            }

"quit"      { return QUIT; }

"let"       { return LET; }

"cond"      { return COND; }

{int_type}  {
                yylval.dval = 0;
                return TYPE;
            }

"real"      {
                yylval.dval = 1;
                return TYPE;
            }

{func}      {
                yylval.sval = (char *) malloc(strlen(yytext)*sizeof(char));
                strcpy(yylval.sval, yytext);
                //printf("lex: FUNC sval = %s\n", yylval.sval);
                return FUNC;
            }

{symbol}    {
                yylval.sval = (char *) malloc(strlen(yytext) * sizeof(char));
                strcpy(yylval.sval, yytext);
                return SYMBOL;
            }

"("         { 
                //printf("lex: LPAREN\n"); 
                return LPAREN;
            }
")"         { 
                //printf("lex: RPAREN\n"); 
                return RPAREN;
            }
"\n"        {
                //printf("lex: RPAREN\n"); 
                return EOL;
            }

[ \t]       ; /* skip whitespace */

.           // yyerror("lex: invalid character");

%%