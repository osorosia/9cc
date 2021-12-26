# 9cc

## EBNF
```
program = ("int" ident "(" ("int" ident( "," "int" ident)*)? ")" "{" stmt "}")*
stmt    = expr ";"
        | "{" stmt* "}"
        | "if" "(" expr ")" stmt ("else" stmt)?
        | "while" "(" expr ")" stmt
        | "for" "(" expr? ";" expr? ";" expr? ")" stmt
        | "return" expr? ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
           | ("*" | "&") unary
primary = num
        | ident ( "(" (expr ( "," expr)*)? ")" )?
        | declaration
        | "(" expr ")"
```

## TODO
- 関数のプロトタイプ宣言に対応していない
- {int a = int b = 3; return a + b;} => 6 と正常動作してしまう（構文エラーにする）

