# 9cc

## EBNF
```
program = ident "(" (expr( "," expr)*)? ")" "{" stmt* "}" 
stmt    = expr ";"
        | "if" "(" expr ")" stmt ("else" stmt)?
        | "while" "(" expr ")" stmt
        | "for" "(" expr? ";" expr? ";" expr? ")" stmt
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary = num
        | ident ( "(" (expr ( "," expr)*)? ")" )?
        | "(" expr ")" 
```
