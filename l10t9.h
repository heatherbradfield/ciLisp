/*
* Heather Bradfield
* Lab 10 Task 9
* 4/15/16
*/

#ifndef __l10t9_h_
#define __l10t9_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "y.tab.h"

int yyparse(void);
int yylex(void);
void yyerror(char *);

typedef enum { NUM_TYPE, FUNC_TYPE, SYMBOL_TYPE, TABLE_TYPE, LET_TYPE, COND_TYPE } AST_NODE_TYPE;
typedef enum { INT = 0, REAL = 1, UNDECLARED = 2} DATA_TYPE;

typedef struct
{
    double value;
} NUMBER_AST_NODE;

typedef struct
{
   char *name;
   struct ast_node *op1;
   struct ast_node *op2;
} FUNCTION_AST_NODE;

typedef struct {
   char *name;   
} SYMBOL_AST_NODE;

typedef struct let_node {
   char *symbol;
   struct ast_node *symVal;
   struct ast_node *next; 
} LET_AST_NODE;

typedef struct {
   struct ast_node *expr;
   struct ast_node *let_list;
} TABLE_AST_NODE;

typedef struct
{
    struct ast_node *cond;
    struct ast_node *true_expr;
    struct ast_node *false_expr;
} COND_AST_NODE;

typedef struct ast_node
{
   AST_NODE_TYPE type;
   DATA_TYPE data_type;

   union
   {
      NUMBER_AST_NODE number;
      FUNCTION_AST_NODE function;
      SYMBOL_AST_NODE symbol;
      LET_AST_NODE let;
      TABLE_AST_NODE table;
      COND_AST_NODE condition;
   } data;

   struct ast_node *parent;
} AST_NODE;

void freeNode(AST_NODE *p);
void typeReset();

AST_NODE *number(double value);
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2);
AST_NODE *condition(AST_NODE* condition, AST_NODE* ifTrue, AST_NODE* ifFalse);
AST_NODE *symbol(char* name);
AST_NODE *let_elem(DATA_TYPE data_type, char *name, AST_NODE *symVal);
void add(AST_NODE *let_list, AST_NODE *list_elem);
AST_NODE *let_list(AST_NODE *let_elem, AST_NODE *let_list);

AST_NODE *let(AST_NODE *let_list, AST_NODE *expr);
void parent(AST_NODE *table, AST_NODE *let_list);

AST_NODE *findDeclared(AST_NODE *let_list, char *value);
AST_NODE *findDeclaredSymbol(AST_NODE *let_list, char *value);
AST_NODE *listSearch(AST_NODE *let_list, char *value); 
AST_NODE *getSymbol(AST_NODE *let_list, char *value);
DATA_TYPE getType();

void printResult(double result);
double eval(AST_NODE *ast);
double calc(char *func, double op1, double op2);

#endif
