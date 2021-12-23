#include "9cc.h"

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

t_Node	*new_node_if(t_NodeKind kind, t_Node *cond, t_Node *then, t_Node *els)
{
	t_Node	*node;

	node = new_node(kind, NULL, NULL);
	node->cond = cond;
	node->then = then;
	node->els = els;
	return (node);
}

t_Node	*new_node_while(t_NodeKind kind, t_Node *cond, t_Node *then)
{
	t_Node	*node;

	node = new_node(kind, NULL, NULL);
	node->cond = cond;
	node->then = then;
	return (node);
}

t_Node	*new_node_for(t_Node *init, t_Node *cond, t_Node *update, t_Node *then)
{
	t_Node	*node;

	node = new_node(ND_FOR, NULL, NULL);
	node->init = init;
	node->cond = cond;
	node->update = update;
	node->then = then;
	return (node);
}

t_Block	*new_block(t_Block *cur, t_Node *node)
{
	t_Block	*next;

	next = (t_Block *)calloc(1, sizeof(t_Block));
	next->node = node;
	next->next = NULL;
	cur->next = next;
	return (next);
}

t_Node	*stmt()
{
	t_Node	*node;
	t_Node	*node_init;
	t_Node	*node_update;
	t_Node	*node_cond;
	t_Node	*node_then;
	t_Node	*node_else;
	t_Block	*block;
	t_Block	block_head;

	if (consume("{"))
	{
		block_head.next = NULL;
		block = &block_head;
		while (!consume("}"))
			block = new_block(block, stmt());
		node = new_node(ND_BLOCK, NULL, NULL);
		node->block = block_head.next;
		return (node);
	}
	if (consume("if"))
	{
		expect("(");
		node_cond = expr();
		expect(")");
		node_then = stmt();
		node_else = NULL;
		if (consume("else"))
			node_else = stmt();
		node = new_node_if(ND_IF, node_cond, node_then, node_else);
		return (node);
	}
	if (consume("while"))
	{
		expect("(");
		node_cond = expr();
		expect(")");
		node_then = stmt();
		node = new_node_while(ND_WHILE, node_cond, node_then);
		return (node);
	}
	if (consume("for"))
	{
		expect("(");
		node_init = NULL;
		node_cond = NULL;
		node_update = NULL;
		if (!peek(";", 0))
			node_init = expr();
		expect(";");
		if (!peek(";", 0))
			node_cond = expr();
		expect(";");
		if (!peek(")", 0))
			node_update = expr();
		expect(")");
		node_then = stmt();
		node = new_node_for(node_init, node_cond, node_update, node_then);
		return (node);
	}
	if (consume("return"))
	{
		node = new_node(ND_RETURN, expr(), NULL);
		expect(";");
		return (node);
	}
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

t_Node	*new_node_func(t_Token *token)
{
	t_Node	*node;

	node = (t_Node *)calloc(1, sizeof(t_Node));
	node->kind = ND_FUNC;
	node->name = token->str;
	node->len = token->len;
	return (node);
}

t_Node	*new_node_args(t_Node *args, t_Node *node)
{
	t_Node	*next;

	next = new_node(ND_ARGS, node, NULL);
	args->args = next;
	return (next);
}

t_Node	*primary()
{
	t_Node	*node;
	t_Token	*token;
	t_Node	*args;
	t_Node	args_head;

	if (consume("("))
	{
		node = expr();
		expect(")");
		return (node);
	}
	token = consume_ident();
	if (token)
	{
		if (consume("("))
		{
			node = new_node_func(token);
			if (consume(")"))
				return (node);
			args_head.args = NULL;
			args = &args_head;
			args = new_node_args(args, expr());
			while (consume(","))
				args = new_node_args(args, expr());
			expect(")");
			node->args = args_head.args;
			return (node);
		}
		return (new_node_ident(token));
	}
	return (new_node_num(expect_number()));
}
