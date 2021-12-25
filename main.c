#include "9cc.h"

int main(int argc, char **argv)
{
	if (argc != 2)
		error("Invalid argument!");

	user_input = argv[1];
	g_token = tokenize();
	program();
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");
	while (g_program)
	{
		printf("%.*s:\n", g_program->len, g_program->name);
		printf("\tpush rbp\n");
		printf("\tmov rbp, rsp\n");
		printf("\tsub rsp, %d\n", g_program->locals ? g_program->locals->offset : 0);
		if (g_program->locals != NULL && g_program->locals->offset % 16 != 0)
			printf("\tsub rsp, 0x18\n");
		if (g_program->args_len >= 1) gen_func_args("rdi", 8);
		if (g_program->args_len >= 2) gen_func_args("rsi", 8 * 2);
		if (g_program->args_len >= 3) gen_func_args("rdx", 8 * 3);
		if (g_program->args_len >= 4) gen_func_args("rcx", 8 * 4);
		if (g_program->args_len >= 5) gen_func_args("r8", 8 * 5);
		if (g_program->args_len >= 6) gen_func_args("r9", 8 * 6);
		gen(g_program->body);
		g_program = g_program->next;
		printf("\tpop rax\n");
		printf("\tmov rsp, rbp\n");
		printf("\tpop rbp\n");
		printf("\tret\n");
	}
	return (0);
}
