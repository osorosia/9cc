#include "9cc.h"

int	size_of(t_Type *ty)
{
	if (ty->ty == INT)
		return 4;
	if (ty->ty == PTR)
		return 8;
	error("invalid type!");
}

void	gen_lval(t_Node *node)
{
	if (node->kind == ND_LVAR)
	{	
		printf("\tmov rax, rbp\n");
		printf("\tsub rax, %d\n", node->offset);
		printf("\tpush rax\n");
		return ;
	}
	if (node->kind == ND_DEREF)
	{
		gen(node->lhs);
		return ;
	}
	error("not an lvalue");
}

void	gen_func_args(char *str, int offset)
{
	printf("\tmov rax, rbp\n");
	printf("\tsub rax, %d\n", offset);
	printf("\tmov [rax], %s\n", str);
}

void	swap(t_Node **l, t_Node **r)
{
	t_Node	*tmp;

	tmp = *l;
	*l = *r;
	*r = tmp;
}

void	gen(t_Node *node)
{
	t_Node	*block;
	t_Node	*args;
	t_Type	*ty;
	int		tag_num;
	int		args_count;

	if (!node)
		return ;
	switch (node->kind)
	{	
	case ND_NUM:
		printf("\tpush %d\n", node->val);
		return ;
	case ND_LVAR:
		gen_lval(node);
		printf("\tpop rax\n");
		printf("\tmov rax, [rax]\n");
		printf("\tpush rax\n");
		return ;
	case ND_ADDR:
		gen_lval(node->lhs);
		node->ty = (t_Type *)calloc(1, sizeof(t_Type));
		node->ty->ty = PTR;
		node->ty->ptr_to = node->lhs->ty;
		return ;
	case ND_DEREF:
		gen(node->lhs);
		printf("\tpop rax\n");
		printf("\tmov rax, [rax]\n");
		printf("\tpush rax\n");
		node->ty = node->lhs->ty->ptr_to;
		if (!node->ty)
			error("not pointer!");
		return ;
	case ND_SIZEOF:
		gen(node->lhs);
		printf("\tpop rax\n");
		printf("\tmov rax, %d\n",size_of(node->lhs->ty));
		printf("\tpush rax\n");
		node->ty = (t_Type *)calloc(1, sizeof(t_Type));
		node->ty->ty = INT;
		return ;
	case ND_ASSIGN:
		gen_lval(node->lhs);
		gen(node->rhs);
		printf("\tpop rdi\n");
		printf("\tpop rax\n");
		printf("\tmov [rax], rdi\n");
		printf("\tpush rdi\n");
		return ;
	case ND_RETURN:
		gen(node->lhs);
		printf("\tpop rax\n");
		printf("\tmov rsp, rbp\n");
		printf("\tpop rbp\n");
		printf("\tret\n");
		return ;
	case ND_IF:
		tag_num = g_tag_num;
		g_tag_num++;
		gen(node->cond);
		printf("\tpop rax\n");
		printf("\tcmp rax, 0\n");
		if (!node->els)
		{
			printf("\tje .Lend%d\n", tag_num);
			gen(node->then);
		}
		else
		{
			printf("\tje .Lelse%d\n", tag_num);
			gen(node->then);
			printf("\tjmp .Lend%d\n", tag_num);
			printf(".Lelse%d:\n", tag_num);
			gen(node->els);
		}
		printf(".Lend%d:\n", tag_num);
		return ;
	case ND_WHILE:
		tag_num = g_tag_num++;
		printf(".Lbegin%d:\n", tag_num);
		gen(node->cond);
		printf("\tpop rax\n");
		printf("\tcmp rax, 0\n");
		printf("\tje .Lend%d\n", tag_num);
		gen(node->then);
		printf("\tjmp .Lbegin%d\n", tag_num);
		printf(".Lend%d:\n", tag_num);
		return ;
	case ND_FOR:
		tag_num = g_tag_num++;
		gen(node->init);
		printf(".Lbegin%d:\n", tag_num);
		if (node->cond)
		{
			gen(node->cond);
			printf("\tpop rax\n");
			printf("\tcmp rax, 0\n");
			printf("\tje .Lend%d\n", tag_num);
		}
		gen(node->then);
		gen(node->update);
		printf("\tjmp .Lbegin%d\n", tag_num);
		printf(".Lend%d:\n", tag_num);
		return ;
	case ND_BLOCK:
		block = node->next; 
		while (block)
		{
			gen(block->body);
			block = block->next;
			printf("\tpop rax\n");
		}
		return ;
	case ND_CALL:
		args = node->args;
		args_count = 0;
		while (args)
		{
			args_count++;
			if (args_count > 6)
				break ;
			gen(args->lhs);
			printf("\tpop rax\n");
			if (args_count == 1) printf("\tmov rdi, rax\n");
			if (args_count == 2) printf("\tmov rsi, rax\n");
			if (args_count == 3) printf("\tmov rdx, rax\n");
			if (args_count == 4) printf("\tmov rcx, rax\n");
			if (args_count == 5) printf("\tmov r8, rax\n");
			if (args_count == 6) printf("\tmov r9, rax\n");
			args = args->args;
		}
		printf("\tcall %.*s\n", node->len, node->name);
		printf("\tpush rax\n");
		return ;
	}
	gen(node->lhs);
	gen(node->rhs);
	printf("\tpop rdi\n");
	printf("\tpop rax\n");
	switch (node->kind)
	{
	case ND_ADD:
		if (node->lhs->ty->ty == PTR)
			printf("\timul rdi, %d\n", size_of(node->lhs->ty->ptr_to));
		if (node->rhs->ty->ty == PTR)
			printf("\timul rax, %d\n", size_of(node->rhs->ty->ptr_to));
		printf("\tadd rax, rdi\n");
		if (node->lhs->ty->ty == PTR)
			swap(&node->lhs, &node->rhs);
		if (node->lhs->ty->ty == PTR)
			error("<pointer> + <pointer> is not defined!");
		node->ty = node->rhs->ty;
		break ;
	case ND_SUB:
		if (node->lhs->ty->ty == PTR)
			printf("\timul rdi, %d\n", size_of(node->lhs->ty->ptr_to));
		if (node->rhs->ty->ty == PTR)
			printf("\timul rax, %d\n", size_of(node->rhs->ty->ptr_to));
		printf("\tsub rax, rdi\n");
		if (node->lhs->ty->ty == PTR)
			swap(&node->lhs, &node->rhs);
		if (node->lhs->ty->ty == PTR)
			error("<pointer> - <pointer> is not defined!");
		node->ty = node->rhs->ty;
		break ;
	case ND_MUL:
		printf("\timul rax, rdi\n");
		if (node->lhs->ty->ty != PTR)
			swap(&node->lhs, &node->rhs);
		if (node->lhs->ty->ty == PTR)
			error("<pointer> * <***> is not defined!");
		node->ty = node->lhs->ty;
		break ;
	case ND_DIV:
		printf("\tcqo\n");
		printf("\tidiv rdi\n");
		if (node->lhs->ty->ty != PTR)
			swap(&node->lhs, &node->rhs);
		if (node->lhs->ty->ty == PTR)
			error("<pointer> / <***> is not defined!");
		node->ty = node->lhs->ty;
		break ;
	case ND_EQ:
		printf("\tcmp rax, rdi\n");
		printf("\tsete al\n");
		printf("\tmovzb rax, al\n");
		node->ty = (t_Type *)calloc(1, sizeof(t_Type));
		node->ty->ty = INT;
		break ;
	case ND_NE:
		printf("\tcmp rax, rdi\n");
		printf("\tsetne al\n");
		printf("\tmovzb rax, al\n");
		node->ty = (t_Type *)calloc(1, sizeof(t_Type));
		node->ty->ty = INT;
		break ;
	case ND_LT:
		printf("\tcmp rax, rdi\n");
		printf("\tsetl al\n");
		printf("\tmovzb rax, al\n");
		node->ty = (t_Type *)calloc(1, sizeof(t_Type));
		node->ty->ty = INT;
		break ;
	case ND_LE:
		printf("\tcmp rax, rdi\n");
		printf("\tsetle al\n");
		printf("\tmovzb rax, al\n");
		node->ty = (t_Type *)calloc(1, sizeof(t_Type));
		node->ty->ty = INT;
		break ;
	}
	printf("\tpush rax\n");
}
