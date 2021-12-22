#ifndef _9CC_H_
# define _9CC_H_

# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <stdbool.h>
# include <string.h>
# include <ctype.h>

//
// parser.c
//
typedef enum
{
	TK_RESERVED,
	TK_IDENT,
	TK_NUM,
	TK_EOF,
}	t_TokenKind;

typedef struct s_Token	t_Token;

struct s_Token
{
	t_TokenKind	kind;
	t_Token		*next;
	int		val;
	char		*str;
	int		len;
};

typedef	enum
{
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_EQ,
	ND_NE,
	ND_LT,
	ND_LE,
	ND_ASSIGN,
	ND_LVAR,
	ND_NUM,
}	t_NodeKind;

typedef struct s_Node	t_Node;

struct	s_Node
{
	t_NodeKind	kind;
	t_Node		*lhs;
	t_Node		*rhs;
	int			val;
	int			offset;
};

void	error(const char *fmt, ...);
t_Token	*tokenize(void);
void	program(void);
t_Node	*stmt(void);
t_Node	*expr(void);
t_Node	*assign(void);
t_Node	*equality(void);
t_Node	*relational(void);
t_Node	*add(void);
t_Node	*mul(void);
t_Node	*unary(void);
t_Node	*primary(void);

t_Token	*g_token;
char	*user_input;
t_Node	*code[100];

//
// codegen.c
//
void	gen(t_Node *node);

#endif
