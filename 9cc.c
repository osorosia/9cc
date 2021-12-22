#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef enum
{
	TK_RESERVED,
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

t_Token	*g_token;
char	*user_input;

void	error_at(char *loc, char *fmt, ...)
{
	va_list	ap;
	int		pos;

	pos = loc - user_input;
	va_start(ap, fmt);
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

void	error(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

int	expect_number(void)
{
	int	val;

	if (g_token->kind != TK_NUM)
		error_at(g_token->str, "Not numeric!");
	val = g_token->val;
	g_token = g_token->next;
	return (val);
}

t_Token	*new_token(t_TokenKind kind, t_Token *cur, char *str, int len)
{
	t_Token	*next;

	next = (t_Token *)calloc(1, sizeof(t_Token));
	next->kind = kind;
	next->str = str;
	next->len = len;
	cur->next = next;
	return (next);
}

bool	startswith(char *p, char *q)
{
	return (memcmp(p, q, strlen(q)) == 0);
}

t_Token	*tokenize(void)
{
	t_Token	head;
	t_Token	*cur; 
	char	*p;
	char	*q;

	head.next = NULL;
	cur = &head;
	p = user_input;
	while (*p)
	{
		if (isspace(*p))
		{
			p++;
			continue ;
		}
		if (startswith(p, "==") || startswith(p, "!=") 
				|| startswith(p, "<=") || startswith(p, ">="))
		{
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue ;
		}
		if (strchr("+-*/()<>", *p))
		{
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue ;
		}
		if (isdigit(*p))
		{
			cur = new_token(TK_NUM, cur, p, 0);
			q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - q;
			continue ;
		}
		error_at(p, "Invalid Token!");
	}
	new_token(TK_EOF, cur, p, 0);
	return (head.next);
}

bool	at_eof(void)
{
	return (g_token->kind == TK_EOF);
}

bool	consume(char *op)
{
	if (g_token->kind != TK_RESERVED
		|| g_token->len != strlen(op)
		|| memcmp(g_token->str, op, g_token->len))
		return (false);
	g_token = g_token->next;
	return (true);
}

void	expect(char *op)
{
	if (g_token->kind != TK_RESERVED
		|| g_token->len != strlen(op)
		|| memcmp(g_token->str, op, g_token->len))
		error_at(g_token->str, "expected '%c'!", op);
	g_token = g_token->next;
}

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
	ND_NUM,
}	t_NodeKind;

typedef struct s_Node	t_Node;

struct	s_Node
{
	t_NodeKind	kind;
	t_Node		*lhs;
	t_Node		*rhs;
	int			val;
};

t_Node	*new_node(t_NodeKind	kind, t_Node *lhs, t_Node *rhs)
{
	t_Node	*node;

	node = calloc(1, sizeof(t_Node));
	node->kind = kind;
	node->lhs = lhs;
	node->rhs = rhs;
	return (node);
}

t_Node	*new_node_num(int val)
{
	t_Node	*node;

	node = new_node(ND_NUM, NULL, NULL);
	node->val = val;
	return (node);
}

t_Node	*expr();
t_Node	*equality();
t_Node	*relational();
t_Node	*add();
t_Node	*mul();
t_Node	*unary();
t_Node	*primary();

t_Node	*expr()
{
	return (equality());
}

t_Node *equality()
{
	t_Node	*node;

	node = relational();
	for (;;)
	{
		if (consume("=="))
			node = new_node(ND_EQ, node, relational());
		else if (consume("!="))
			node = new_node(ND_NE, node, relational());
		else
			return (node);
	}
}

t_Node *relational()
{
	t_Node	*node;

	node = add();
	for (;;)
	{
		if (consume("<"))
			node = new_node(ND_LT, node, add());
		else if (consume("<="))
			node = new_node(ND_LE, node, add());
		else if (consume(">"))
			node = new_node(ND_LT, add(), node);
		else if (consume(">="))
			node = new_node(ND_LE, add(), node);
		else
			return (node);
	}
}

t_Node	*add()
{
	t_Node	*node;

	node = mul();
	for (;;)
	{
		if (consume("+"))
			node = new_node(ND_ADD, node, mul());
		else if (consume("-"))
			node = new_node(ND_SUB, node, mul());
		else
			return (node);
	}
}

t_Node	*mul()
{
	t_Node	*node;

	node = unary();
	for (;;)
	{
		if (consume("*"))
			node = new_node(ND_MUL, node, unary());
		else if (consume("/"))
			node = new_node(ND_DIV, node, unary());
		else
			return (node);
	}
}

t_Node	*unary()
{
	t_Node	*node;

	if (consume("+"))
		return (unary());
	if (consume("-"))
		return (new_node(ND_SUB, new_node_num(0), unary()));
	return (primary());
}

t_Node	*primary()
{
	t_Node	*node;

	if (consume("("))
	{
		node = expr();
		expect(")");
		return (node);
	}
	return (new_node_num(expect_number()));
}

void	gen(t_Node *node)
{
	if (node->kind == ND_NUM)
	{
		printf("\tpush %d\n", node->val);
		return ;
	}
	gen(node->lhs);
	gen(node->rhs);
	printf("\tpop rdi\n");
	printf("\tpop rax\n");
	switch (node->kind)
	{
	case ND_ADD:
		printf("\tadd rax, rdi\n");
		break ;
	case ND_SUB:
		printf("\tsub rax, rdi\n");
		break ;
	case ND_MUL:
		printf("\timul rax, rdi\n");
		break ;
	case ND_DIV:
		printf("\tcqo\n");
		printf("\tidiv rdi\n");
		break ;
	case ND_EQ:
		printf("\tcmp rax, rdi\n");
		printf("\tsete al\n");
		printf("\tmovzb rax, al\n");
		break ;
	case ND_NE:
		printf("\tcmp rax, rdi\n");
		printf("\tsetne al\n");
		printf("\tmovzb rax, al\n");
		break ;
	case ND_LT:
		printf("\tcmp rax, rdi\n");
		printf("\tsetl al\n");
		printf("\tmovzb rax, al\n");
		break ;
	case ND_LE:
		printf("\tcmp rax, rdi\n");
		printf("\tsetle al\n");
		printf("\tmovzb rax, al\n");
		break ;
	}
	printf("\tpush rax\n");
}

int main(int argc, char **argv)
{
	t_Node	*node;
	if (argc != 2)
		error("Invalid argument!");

	user_input = argv[1];
	g_token = tokenize();
	node = expr();
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	gen(node);
	printf("\tpop rax\n");
	printf("\tret\n");
	return (0);
}
