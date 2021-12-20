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
};

t_Token	*g_token;

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
		error("Not numeric!");
	val = g_token->val;
	g_token = g_token->next;
	return (val);
}

t_Token	*new_token(t_TokenKind kind, t_Token *cur, char *str)
{
	t_Token	*next;

	next = (t_Token *)calloc(1, sizeof(t_Token));
	next->kind = kind;
	next->str = str;
	cur->next = next;
	return (next);
}

t_Token	*tokenize(char *p)
{
	t_Token	head;
	t_Token	*cur; 

	head.next = NULL;
	cur = &head;
	while (*p)
	{
		if (isspace(*p))
		{
			p++;
			continue ;
		}
		if (*p == '+' || *p == '-')
		{
			cur = new_token(TK_RESERVED, cur, p++);
			continue ;
		}
		if (isdigit(*p))
		{
			cur = new_token(TK_NUM, cur, p);
			cur->val = strtol(p, &p, 10);
			continue ;
		}
		error("Invalid Token!");
	}
	new_token(TK_EOF, cur, p);
	return (head.next);
}

bool	at_eof(void)
{
	return (g_token->kind != TK_EOF);
}

bool	consume(char op)
{
	if (g_token->kind != TK_RESERVED || g_token->str[0] != op)
		return (false);
	g_token = g_token->next;
	return (true);
}

void	expect(char op)
{
	if (g_token->kind != TK_RESERVED || g_token->str[0] != op)
		error("%c is Not %c!", g_token->str[0], op);
	g_token = g_token->next;
}

int main(int argc, char **argv)
{
	if (argc != 2)
		error("Invalid argument!");

	g_token = tokenize(argv[1]);
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	printf("main:\n");
	printf("\tmov rax, %d\n", expect_number());
	while (!at_eof())
	{
		if (consume('+'))
		{
			printf("\tadd rax, %d\n", expect_number());
			continue ;
		}
		expect('-');
		printf("\tsub rax, %d\n", expect_number());
	}
	printf("\tret\n");
	return (0);
}


