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
	printf("main:\n");
	printf("\tpush rbp\n");
	printf("\tmov rbp, rsp\n");
	printf("\tsub rsp, %d\n", g_locals ? g_locals->offset : 0);
	for (int i = 0; code[i]; i++)
	{	
		gen(code[i]);
		printf("\tpop rax\n");
	}
 	printf("\tmov rsp, rbp\n");
 	printf("\tpop rbp\n");
	printf("\tret\n");
	return (0);
}
