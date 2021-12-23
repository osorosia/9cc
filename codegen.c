#include "9cc.h"

void	gen_lval(t_Node *node)
{
	printf("\tmov rax, rbp\n");
	printf("\tsub rax, %d\n", node->offset);
	printf("\tpush rax\n");
}

void	gen(t_Node *node)
{
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
		gen(node->cond);
		printf("\tpop rax\n");
		printf("\tcmp rax, 0\n");
		printf("\tje .Lend%d\n", g_tag_num);
		gen(node->then);
		printf(".Lend%d:\n", g_tag_num);
		g_tag_num++;
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
