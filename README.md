# 9cc

## EBNF
```
program		= (typ ident "(" (typ ident( "," typ ident)*)? ")" "{" stmt "}")*
stmt		= expr ";"
			| "{" stmt* "}"
			| "if" "(" expr ")" stmt ("else" stmt)?
			| "while" "(" expr ")" stmt
			| "for" "(" expr? ";" expr? ";" expr? ")" stmt
			| "return" expr? ";"
			| typ ident ("=" expr)? ";"
expr		= assign
assign		= equality ("=" assign)?
equality	= relational ("==" relational | "!=" relational)*
relational	= add ("<" add | "<=" add | ">" add | ">=" add)*
add			= mul ("+" mul | "-" mul)*
mul			= unary ("*" unary | "/" unary)*
unary		= ("+" | "-")? primary
			| ("*" | "&") unary
primary		= num
			| ident ( "(" (expr ( "," expr)*)? ")" )?
			| "(" expr ")"
typ			= "int" "*"*
```

