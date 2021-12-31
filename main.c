#include "9cc.h"

int main(int argc, char **argv) {
    if (argc != 2)
        error("Invalid argument!");

    user_input = argv[1];
    g_token = tokenize();
    program();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    for (t_Obj *func = g_functions; func; func = func->next) {
        gen_func_prologue(func);
        gen(func->body);
        gen_func_epilogue();
    }
    return 0;
}
