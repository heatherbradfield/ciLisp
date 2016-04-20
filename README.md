# ci-lisp
## An interpreter for CiLisp written in C

This project was for my COMP232 class - Programming Language Design. It uses flex and bison for the grammar and parsing aspects. The project begins basically as a calculator and then each task adds additional functionality. 

This language uses s-expressions which are a special notation for operations (functions), in which the operator name and the operands are enclosed in parentheses, in prefix notation. This is common for Lisps. For example, "1+2" is noted as:

`(add 1 2)`

### Features of ciLisp:

#### Types
There are two types in ciLisp. They are `real` and `integer`. Integer is your typical integer and real behaves a double. Variables must be initialized and declared with desired types before they can be used.
Variables are coerced with the following rules:
  - Integers should be coerced into reals by adding a zero fraction part. Ex: `1 -> 1.0`
  - Reals should be coerced into integer variables by rounding to the closest integer (down or up). Ex: `1.49 -> 1` and `1.69 to 2`
  - The type of an expression should be integer if only expressions of type integer are used to compute it. Otherwise, it should be real.

The evaluated result will be printed as a real number (even if it is an integer), although the proper coercion takes place. Also, there are no non-number types in ciLisp.

#### Variables
Variables are declared using the `let` keyword. The scope in which they exist is the s-expression that immediately follows the let statement. You can create an arbitrary number of variables within one let statement.

`((let (integer myVar 10) (real yourVar 2.9) (add myVar yourVar))`

#### Conditionals
Conditionals are done using the `cond` keyword with the following syntax: (cond (condition) (do this if its true) (do this if its false))

`(cond (smaller 1 10) (print 1) (print 2))`

The `cond` function checks to see if the first expression evaluates to true (non-zero). If so, the second expression is evaluated and returned. Otherwise, the third s_expression is evaluated and returned. The conditional functions `equal`, `smaller`, and `larger` all return 1 if the condition holds and 0 otherwise.

#### Printing to stdout
The print function is invoked by using the following syntax:

`(print (hypot 3 4))`

#### Reading from stdin
The `read` function is a parameter-less function that reads a number from standard input and can be used to assign a variable.

`((let (real a (read))) (add a 5))`

#### Random Numbers
The `rand` is another parameter-less function that is used for generating a random number. The time of execution is used as the seed for the random number generator.

#### Math Library
The standard library includes the following math functions : `neg`, `abs`, `exp`, `sqrt`, `exp2`, `cbrt`, `hypot`, `add`, `sub`, `mult`, `div`, `remainder`, `log`, `pow`, `max`, `min`.

#### Static Scoping
ciLisp implements static scoping, which means that it looks for the most recently declared variable with a given name, then its parent scope, and so on. For example:

`((let (integer myVar 10) (real yourVar 5.5)) ((let (myVar 5)) (add myVar yourVar)))`

would yield 10.5. Also, the type of a variable is inherited if it was declared in a previous scope. So, in the above example, myVar did not need to be declared with a type, although it could have.

#### Grammar
ciLisp uses the following grammar:
```
program ::= | program s_expr EOL
s_expr ::= number
| symbol
| ( func s_expr )
| ( func s_expr s_expr )
| ( cond s_expr s_expr s_expr )
| ( ( let let_list ) s_expr )
let_section ::= ( let let_list )
let_list ::= let_elem | let_list let_elem
let_elem ::= ( type symbol s_expr )
type ::= | integer | real
symbol ::= letter+
letter ::= [a-zA-Z]
func ::= neg|abs|exp|sqrt|add|sub|mult|div|remainder|log|pow|max|min|exp2|cbrt|hypot|print|equal|smaller|larger
number ::= [+|-]?digit+[.number+]
digit ::= [0-9]
```
