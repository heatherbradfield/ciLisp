/*
* Heather Bradfield
* Lab 10 Task 9
* 4/15/16
*/

// ((let (int myA 1)) ((let (myA 5.5) (real myB 1.5)) (cond (smaller myA myB) (print myA) (print myB))))
// ((let (int a (mult 1 2))) ((let (int b 2)) (print (add a b))))
//((let (a (read)) (b (rand))) (print (add a b)))

#include "l10t9.h"
#define MIN 0
#define MAX 100 //for rand function

char *funcs[] = { "neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder",
 "log", "pow", "max", "min", "exp2", "cbrt", "hypot", "print", "equal", "smaller", "larger", "rand", "read"};

DATA_TYPE final = UNDECLARED;
DATA_TYPE current = UNDECLARED;
int printFlag = 0;

int main(void)
{
    yyparse();
    return 0;
}

void typeReset()
{
  final = UNDECLARED;
}

void yyerror(char *s)
{
    fprintf(stderr, "%s\n", s);
}

// find out which function it is
int resolveFunc(char *func)
{
   int i = 0;
   while (funcs[i][0] !='\0')
   {
      if (!strcmp(funcs[i], func))
         return i;
         
      i++;
   }
   yyerror("invalid function"); // paranoic -- should never happen
   return -1;
}

// free a node
void freeNode(AST_NODE *p)
{
    if (!p) return;
       
    if (p->type == FUNC_TYPE)
    {
      free(p->data.function.name);
      freeNode(p->data.function.op1);
      freeNode(p->data.function.op2);
    }
    else if (p->type == LET_TYPE)
    {
      freeNode(p->data.let.next);
    }
    else if (p->type == TABLE_TYPE)
    {
      freeNode(p->data.table.expr);
      freeNode(p->data.table.let_list);
    }
    free (p);
}

// create a node for a number
AST_NODE *number(double value)
{
    AST_NODE *p;    
    size_t nodeSize;

    // allocate space for the fixed size and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->parent = NULL;

    p->type = NUM_TYPE;
    p->data.number.value = value;

    return p;
}

// create a node for a function
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed size and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = FUNC_TYPE;
    p->data.function.name = funcName;
    p->data.function.op1 = op1;
    p->data.function.op2 = op2;
    if (op1 != NULL) p->data.function.op1->parent = p;
    if (op2 != NULL) p->data.function.op2->parent = p;

    return p;
}

// create a node for a condition
AST_NODE *condition(AST_NODE* condition, AST_NODE* ifTrue, AST_NODE* ifFalse)
{
  AST_NODE *p;
  size_t nodeSize;

  // allocate space for the fixed sie and the variable part (union)
  nodeSize = sizeof(AST_NODE) + sizeof(COND_AST_NODE);
  if ((p = malloc(nodeSize)) == NULL)
    yyerror("out of memory");

  p->type = COND_TYPE;
  p->data.condition.cond = condition;
  p->data.condition.true_expr = ifTrue;
  p->data.condition.false_expr = ifFalse;
  p->data.condition.true_expr->parent = p;
  p->data.condition.false_expr->parent = p;

  return p;
}

AST_NODE *symbol(char* name)
{
  AST_NODE *p;
  size_t nodeSize = sizeof(AST_NODE) + sizeof(SYMBOL_AST_NODE);
  if ((p = malloc(nodeSize)) == NULL)
    yyerror("out of memory");

  p->type = SYMBOL_TYPE;
  p->data.symbol.name = name;

  return p;
}

// create let elem 
AST_NODE *let_elem(DATA_TYPE data_type, char *name, AST_NODE *symVal)
{
  //printf("!\n");
  AST_NODE *p;

  size_t nodeSize = sizeof(AST_NODE) + sizeof(LET_AST_NODE);
  if ((p = malloc(nodeSize)) == NULL)
    yyerror("out of memory");

  p->type = LET_TYPE;
  p->data_type = data_type;
  //printf("%d\n",p->data_type);
  p->data.let.symbol = name;
  p->data.let.symVal = symVal;
  p->data.let.symVal->parent = p;
  p->data.let.next = NULL;

  return p;
}

// Add the let_elem to end of let_list 
void add(AST_NODE *let_list, AST_NODE *list_elem)
{
  AST_NODE *duplicate;

  if (let_list->data.let.next != NULL) add(let_list->data.let.next, list_elem);      
  else {   
    duplicate = findDeclared(let_list, let_list->data.let.symbol);
    if (duplicate == NULL) let_list->data.let.next = list_elem;    
    else { 
      fprintf(stderr, "ERROR: redeclaration of variable <%s> attempted\n", let_list->data.let.symbol);
      let_list->data.let.symVal = duplicate;
    }
  }       
}

// Create a new let_list 
AST_NODE *let_list(AST_NODE *let_list, AST_NODE *list_elem)
{
  add(let_list, list_elem);
  return let_list;
}

// Sets element(s) to table 
void parent(AST_NODE *table, AST_NODE *let_list)
{
  // initialize the parent with the symbol table,
  // then go through rest of the list
  if (let_list != NULL) let_list->parent = table;
  if (let_list->data.let.next != NULL) parent(table, let_list->data.let.next);
}

// Set parent node 
AST_NODE *newScope(AST_NODE *let_list, AST_NODE *expr)
{
  AST_NODE *symbolTable;

  if ((symbolTable = malloc(sizeof(TABLE_AST_NODE) + sizeof(AST_NODE))) == NULL)
      yyerror("out of memory");

  symbolTable->type = TABLE_TYPE;
  expr->parent = symbolTable;
  parent(symbolTable, let_list);

  symbolTable->data.table.expr = expr;
  symbolTable->data.table.let_list = let_list;

  return symbolTable;
}

AST_NODE *findDeclared(AST_NODE *let_list, char *value)
{
  if (let_list->data_type != UNDECLARED && strcmp(let_list->data.let.symbol, value) == 0)
  {
    current = let_list->data_type;
    return let_list->data.let.symVal;
  }
    
  else if (let_list->data.let.next != NULL) return findDeclared(let_list->data.let.next, value);

  return NULL;
}

AST_NODE *findDeclaredSymbol(AST_NODE *let_list, char *value)
{
  if (let_list->type == TABLE_TYPE)
  {
    AST_NODE *declared = findDeclared(let_list->data.table.let_list, value);
    if (declared != NULL) return declared;
  }

  // check other parents 
  if (let_list->parent != NULL) return findDeclaredSymbol(let_list->parent, value);  

  //fprintf(stderr, "ERROR: undeclared variable <%s> used\n", value);

  return NULL;
}

// Return the symbol value if found 
AST_NODE *listSearch(AST_NODE *let_list, char *value)
{
  if (strcmp(let_list->data.let.symbol, value) == 0)
  {
    final = let_list->data_type;
    return let_list->data.let.symVal;
  } 
  else if ( let_list->data.let.next != NULL) return listSearch(let_list->data.let.next, value);

  return NULL;
}

// Search for the symbol in the let list 
AST_NODE *getSymbol(AST_NODE *let_list, char *value)
{
  if (let_list->type == TABLE_TYPE)
  {
    AST_NODE *result = listSearch(let_list->data.table.let_list, value);
    if (result != NULL) return result;
  }

  // check other parents if result not found yet
  if (let_list->parent != NULL) return getSymbol(let_list->parent, value);  

  //fprintf(stderr, "ERROR: undeclared variable <%s> used\n", value);
  return NULL;
}

DATA_TYPE getType()
{
  return final;
}

void printResult(double result)
{
  if (printFlag == 1)
  {
    if (final == INT) printf("%d", (int)round(result));
    else printf("%.2f", result);
  }
  printFlag = 0;
}

double eval(AST_NODE *p)
{

  if (!p) return 0;

  int func;

  if (p->type == NUM_TYPE) { 
    return p->data.number.value;
  }
  else if (p->type == TABLE_TYPE) return eval(p->data.table.expr);
  else if (p->type == SYMBOL_TYPE)
  {
    AST_NODE *declared = findDeclaredSymbol(p, p->data.symbol.name);
    //if (!declared) printf("!\n");
    p->data_type = current;
    AST_NODE *result = getSymbol(p, p->data.symbol.name);
    if (result != NULL)
    {
      double value = eval(result);
      if (p->data_type == INT && ceilf(value) != value)
      {
        printf("WARNING: incompatible type assignment for variable <%s>\n", p->data.symbol.name);
      }
      return value;
    }
    else {
      fprintf(stderr, "ERROR: undeclared variable <%s> used\n", p->data.symbol.name);
      return 0;
    }
  }
  else if (p->type == COND_TYPE)
  {
    return eval(p->data.condition.cond) ? eval(p->data.condition.true_expr) : eval(p->data.condition.false_expr);
  }
  else
  {    
    func = resolveFunc(p->data.function.name);
    switch (func)
    {
      case 0: return -1 * eval(p->data.function.op1); break;
      case 1: return fabs(eval(p->data.function.op1)); break;
      case 2: return exp(eval(p->data.function.op1)); break;
      case 3: return sqrt(eval(p->data.function.op1)); break;
      case 4: return eval(p->data.function.op1) + eval(p->data.function.op2); break;
      case 5: return eval(p->data.function.op1) - eval(p->data.function.op2); break;
      case 6: return eval(p->data.function.op1) * eval(p->data.function.op2);; break;
      case 7:
        if (p->data.function.op2->data.number.value != 0)        
          return eval(p->data.function.op1) / eval(p->data.function.op2);
        else        
          yyerror("Division by zero\n");
          return 0;        
        break;
      case 8: return fmod(eval(p->data.function.op1), eval(p->data.function.op2)); break;
      case 9:
        if (p->data.function.op1->data.number.value == 10)        
          return log10(eval(p->data.function.op2));
        else if (p->data.function.op1->data.number.value == 2)        
          return log10(eval(p->data.function.op2))/log10(2);
        else
        {  
          yyerror("Base must be 2 or 10.");
          return 0;
        }
        break;
      case 10: return pow(eval(p->data.function.op1), eval(p->data.function.op2)); break;
      case 11: return fmax(eval(p->data.function.op1), eval(p->data.function.op2)); break;
      case 12: return fmin(eval(p->data.function.op1), eval(p->data.function.op2)); break;
      case 13: return exp2(eval(p->data.function.op1)); break;
      case 14: return cbrt(eval(p->data.function.op1)); break;
      case 15: return hypot(eval(p->data.function.op1), eval(p->data.function.op2)); break;
      case 16: printFlag = 1; return eval(p->data.function.op1);
      case 17: return eval(p->data.function.op1) == eval(p->data.function.op2) ? 1 : 0; break;
      case 18: return eval(p->data.function.op1) < eval(p->data.function.op2) ? 1 : 0; break;
      case 19: return eval(p->data.function.op1) > eval(p->data.function.op2) ? 1 : 0; break;
      case 20: p->data_type = REAL; return (double)(rand()%(MAX-MIN)+MIN); break;
      case 21: p->data_type = REAL; double readIn = 0.0; scanf("\n%lf", &readIn); getc(stdin); return readIn; break;
      default: return 0; break;
    }
  }
}