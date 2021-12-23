#include "9cc.h"

void	gen_lval(t_Node *node)
{
	printf("\tmov rax, rbp\n");
	printf("\tsub rax, %d\n", node->offset);
	printf("\tpush rax\n");
}

void	gen(t_Node *node)
{
	int		tag_num;
	t_Block	*block;
	t_Node	*args;
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
		block = node->block; 
		while (block)
		{
			gen(block->node);
			block = block->next;
			printf("\tpop rax\n");
		}
		return ;
	case ND_FUNC:
		args = node->args;
		args_count = 0;
		while (args)
		{
			args_count++;
			if (args_count > 6)
				break ;
			switch (args_count)
			{
			case 1:
				gen(args->lhs);
				printf("\tpop rax\n");
				printf("\tmov rdi, rax\n");
				break ;
			case 2:
				gen(args->lhs);
				printf("\tpop rax\n");
				printf("\tmov rsi, rax\n");
				break ;
			case 3:
				gen(args->lhs);
				printf("\tpop rax\n");
				printf("\tmov rdx, rax\n");
				break ;
			case 4:
				gen(args->lhs);
				printf("\tpop rax\n");
				printf("\tmov rcx, rax\n");
				break ;
			case 5:
				gen(args->lhs);
				printf("\tpop rax\n");
				printf("\tmov r8, rax\n");
				break ;
			case 6:
				gen(args->lhs);
				printf("\tpop rax\n");
				printf("\tmov r9, rax\n");
				break ;
			}
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
