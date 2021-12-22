#include "9cc.h"

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

bool	is_alphabet_lower(char c)
{
	return ('a' <= c && c <= 'z');
}

t_Token	*tokenize(void)
{
	t_Token	head;
	t_Token	*cur; 
	char	*p;
	char	*q;
	int		len;

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
		if (strchr("+-*/()<>;=", *p))
		{
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue ;
		}
		if (is_alphabet_lower(*p))
		{
			len = 0;
			while (is_alphabet_lower(p[len]))
				len++;
			cur = new_token(TK_IDENT, cur, p, len);
			p += cur->len;
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

t_Token	*consume_ident()
{
	t_Token	*cur_token;

	if (g_token->kind != TK_IDENT)
		return (NULL);
	cur_token = g_token;
	g_token = g_token->next;
	return (cur_token);
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
		error_at(g_token->str, "expected '%s'!", op);
	g_token = g_token->next;
}

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

void	program()
{
	int	i;

	i = 0;
	while (!at_eof())
		code[i++] = stmt();
	code[i] = NULL;	
}

t_Node	*stmt()
{
	t_Node	*node;

	node = expr();
	expect(";");
	return (node);
}

t_Node	*expr()
{
	return (assign());
}

t_Node	*assign()
{
	t_Node	*node;

	node = equality();
	if (consume("="))
		node = new_node(ND_ASSIGN, node, assign());
	return (node);
}

t_Node	*equality()
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

t_Node	*relational()
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

t_LVar	*find_lvar(t_Token *token)
{
	for (t_LVar *var = g_locals; var; var = var->next)
	{
		if (token->len == var->len 
			&& !memcmp(token->str, var->name, token->len))
			return (var);
	}
	return (NULL);
}

t_Node	*new_node_ident(t_Token *token)
{
	t_Node	*node;
	t_LVar	*lvar;

	node = (t_Node *)calloc(1, sizeof(t_Node));
	node->kind = ND_LVAR;
	lvar = find_lvar(token);
	if (lvar)
	{
		node->offset = lvar->offset;
	}
	else
	{
		lvar = (t_LVar *)calloc(1, sizeof(t_LVar));
		lvar->next = g_locals;
		lvar->name = token->str;
		lvar->len = token->len;
		lvar->offset = g_locals ? g_locals->offset + 8 : 8;
		node->offset = lvar->offset;
		g_locals = lvar;
	}
	return (node);
}

t_Node	*primary()
{
	t_Node	*node;
	t_Token	*token;

	if (consume("("))
	{
		node = expr();
		expect(")");
		return (node);
	}
	token = consume_ident();
	if (token)
		return (new_node_ident(token));
	return (new_node_num(expect_number()));
}
