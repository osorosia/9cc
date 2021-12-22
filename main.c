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
	printf("  push rbp\n");
	printf("  mov rbp, rsp\n");
	printf("  sub rsp, 208\n");	
	for (int i = 0; code[i]; i++)
	{	
		gen(code[i]);
		printf("\tpop rax\n");
	}
 	printf("  mov rsp, rbp\n");
 	printf("  pop rbp\n");
	printf("\tret\n");
	return (0);
}
