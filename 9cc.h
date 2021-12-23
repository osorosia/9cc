#ifndef _9CC_H_
# define _9CC_H_

# include <stdio.h>
# include <stdlib.h>
# include <stdarg.h>
# include <stdbool.h>
# include <string.h>
# include <ctype.h>

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
	int			val;
	char		*str;
	int			len;
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
	ND_RETURN,
	ND_IF,
	ND_ELSE,
	ND_WHILE,
	ND_FOR,
	ND_LVAR,
	ND_NUM,
}	t_NodeKind;

typedef struct s_Node	t_Node;
struct	s_Node
{
	t_NodeKind	kind;
	t_Node		*lhs;
	t_Node		*rhs;
	t_Node		*cond;
	t_Node		*then;
	t_Node		*els;
	int			val;
	int			offset;
};

typedef struct s_LVar	t_LVar;
struct s_LVar
{
	t_LVar	*next;
	char	*name;
	int		len;
	int		offset;
};

//
// tokenize.c
//

void	error(const char *fmt, ...);
bool	at_eof(void);
t_Token	*consume_ident();
int		expect_number(void);
void	expect(char *op);
bool	consume(char *op);
t_Token	*tokenize(void);

//
// parse.c
//

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

//
// codegen.c
//

void	gen(t_Node *node);

//
// utils.c
//

void	error(const char *fmt, ...);
bool	startswith(char *p, char *q);
bool	is_alnum(char c);

t_Token	*g_token;
char	*user_input;
t_Node	*code[100];
t_LVar	*g_locals;
int		g_tag_num;

#endif
