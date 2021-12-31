#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2)
        error("Invalid argument!");

    user_input = argv[1];
    g_token = tokenize();
    program();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    while (g_functions) {
        gen_func_prologue(g_functions);
        gen(g_functions->body);
        gen_func_epilogue();
        g_functions = g_functions->next;
    }
    return 0;
}
