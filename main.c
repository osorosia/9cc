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
		printf("\tsub rsp, %d\n", g_locals ? g_locals->offset : 0);
		gen(g_program->body);
		g_program = g_program->next;
		printf("\tpop rax\n");
	}
 	printf("\tmov rsp, rbp\n");
 	printf("\tpop rbp\n");
	printf("\tret\n");
	return (0);
}
