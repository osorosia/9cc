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
        printf("%.*s:\n", g_functions->len, g_functions->name);
        printf("    push rbp\n");
        printf("    mov rbp, rsp\n");
        printf("    sub rsp, %d\n", g_functions->locals ? g_functions->locals->offset : 0);
        if (g_functions->locals != NULL && g_functions->locals->offset % 16 != 0)
            printf("    sub rsp, 0x18\n");
        else
            printf("    sub rsp, 0x10\n");
        if (g_functions->args_len >= 1) gen_func_args("rdi", 8);
        if (g_functions->args_len >= 2) gen_func_args("rsi", 8 * 2);
        if (g_functions->args_len >= 3) gen_func_args("rdx", 8 * 3);
        if (g_functions->args_len >= 4) gen_func_args("rcx", 8 * 4);
        if (g_functions->args_len >= 5) gen_func_args("r8", 8 * 5);
        if (g_functions->args_len >= 6) gen_func_args("r9", 8 * 6);
        gen(g_functions->body);
        g_functions = g_functions->next;
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
    }
    return 0;
}
