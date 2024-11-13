Interpreter for the C@ language
C@ has three basic types of statements:
AssgStmt Assign the result of an expression to a variable.
PrintStmt Issue printing of (the result of) an expression to an output.
ConfigStmt Set a configuration. Only one option is available: changing the number base. A base can be either decimal, hexadecimal or binary. Affects subsequent printing statements.
Additionally, there are expressions:
MathExp A mathematical expression. Valid operands are numbers (integers) and variables. Valid operators are the basic arithmetic operators, +–*/. 
Additionally, parentheses are allowed to define scopes for nested expressions.

The grammar (in EBNF form [4]) for C@ is given by:
Stmt := ConfigStmt | AssgStmt | PrintStmt
ConfigStmt := "config" [ "dec" | "hex" | "bin" ]
AssgStmt := Variable "=" MathExp
PrintStmt := "print" MathExp
MathExp := SumExp
SumExp := ProductExp [ "+" ProductExp | "–" ProductExp ]*
ProductExp := PrimaryExp [ "*" PrimaryExp | "/" PrimaryExp ]*
PrimaryExp := Int | Variable | "(" MathExp ")"
Variable := [a-zA-Z][a-zA-Z0-9]*
Int := -?[0-9]+
