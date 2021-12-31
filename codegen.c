#include "9cc.h"

int g_tag_num;

int size_of(t_Type *ty) {
    if (ty->kind == TY_INT)
        return 4;
    if (ty->kind == TY_PTR)
        return 8;
    error("invalid type!");
}

void gen_lval(t_Node *node) {
    if (node->ty && node->ty->kind == TY_ARRAY)
        node->ty->kind == TY_PTR;
    if (node->kind == ND_LVAR) {
        printf("    mov rax, rbp\n");
        printf("    sub rax, %d\n", node->offset);
        printf("    push rax\n");
        return ;
    }
    if (node->kind == ND_DEREF) {
        gen(node->lhs);
        return ;
    }
    error("not an lvalue");
}

void gen_func_args(char *str, int offset) {
    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", offset);
    printf("    mov [rax], %s\n", str);
}

void swap(t_Node **l, t_Node **r) {
    t_Node *tmp = *l;
    *l = *r;
    *r = tmp;
}

void    gen(t_Node *node) {
    t_Node  *block;
    t_Node  *args;
    t_Type  *ty;
    int     tag_num;
    int     args_count;

    if (!node)
        return ;
    switch (node->kind) {
    case ND_NUM: {
        printf("    push %d\n", node->val);
        return ;
    }
    case ND_LVAR: {
        gen_lval(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return ;
    }
    case ND_ADDR: {
        gen_lval(node->lhs);
        node->ty = (t_Type *)calloc(1, sizeof(t_Type));
        node->ty->kind = TY_PTR;
        node->ty->ptr_to = node->lhs->ty;
        return ;
    }
    case ND_DEREF: {
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        node->ty = node->lhs->ty->ptr_to;
        if (!node->ty)
            error("not pointer!");
        return ;
    }
    case ND_SIZEOF: {
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rax, %d\n",size_of(node->lhs->ty));
        printf("    push rax\n");
        node->ty = (t_Type *)calloc(1, sizeof(t_Type));
        node->ty->kind = TY_INT;
        return ;
    }
    case ND_ASSIGN: {
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return ;
    }
    case ND_RETURN: {
        gen(node->lhs);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return ;
    }
    case ND_IF: {
        tag_num = g_tag_num;
        g_tag_num++;
        gen(node->cond);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        if (!node->els) {
            printf("    je .Lend%d\n", tag_num);
            gen(node->then);
        } else {
            printf("    je .Lelse%d\n", tag_num);
            gen(node->then);
            printf("    jmp .Lend%d\n", tag_num);
            printf(".Lelse%d:\n", tag_num);
            gen(node->els);
        }
        printf(".Lend%d:\n", tag_num);
        return ;
    }
    case ND_WHILE: {
        tag_num = g_tag_num++;
        printf(".Lbegin%d:\n", tag_num);
        gen(node->cond);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", tag_num);
        gen(node->then);
        printf("    jmp .Lbegin%d\n", tag_num);
        printf(".Lend%d:\n", tag_num);
        return ;
    }
    case ND_FOR: {
        tag_num = g_tag_num++;
        gen(node->init);
        printf(".Lbegin%d:\n", tag_num);
        if (node->cond) {
            gen(node->cond);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend%d\n", tag_num);
            gen(node->then);
        }
        gen(node->update);
        printf("    jmp .Lbegin%d\n", tag_num);
        printf(".Lend%d:\n", tag_num);
        return ;
    }
    case ND_BLOCK: {
        block = node->next; 
        while (block) {
            gen(block->body);
            block = block->next;
            printf("    pop rax\n");
        }
        return ;
    }
    case ND_CALL: {
        args = node->args;
        args_count = 0;
        while (args) {
            args_count++;
            if (args_count > 6)
                break ;
            gen(args->lhs);
            printf("    pop rax\n");
            if (args_count == 1) printf("    mov rdi, rax\n");
            if (args_count == 2) printf("    mov rsi, rax\n");
            if (args_count == 3) printf("    mov rdx, rax\n");
            if (args_count == 4) printf("    mov rcx, rax\n");
            if (args_count == 5) printf("    mov r8, rax\n");
            if (args_count == 6) printf("    mov r9, rax\n");
            args = args->args;
        }
        printf("    call %.*s\n", node->len, node->name);
        printf("    push rax\n");
        return ;
    }
    }
    gen(node->lhs);
    gen(node->rhs);
    printf("    pop rdi\n");
    printf("    pop rax\n");
    switch (node->kind) {
    case ND_ADD: {
        if (node->lhs->ty->kind == TY_PTR)
            printf("    imul rdi, %d\n", size_of(node->lhs->ty->ptr_to));
        if (node->rhs->ty->kind == TY_PTR)
            printf("    imul rax, %d\n", size_of(node->rhs->ty->ptr_to));
        printf("    add rax, rdi\n");
        if (node->lhs->ty->kind == TY_PTR)
            swap(&node->lhs, &node->rhs);
        if (node->lhs->ty->kind == TY_PTR)
            error("<pointer> + <pointer> is not defined!");
        node->ty = node->rhs->ty;
        break ;
    }
    case ND_SUB: {
        if (node->lhs->ty->kind == TY_PTR)
            printf("    imul rdi, %d\n", size_of(node->lhs->ty->ptr_to));
        if (node->rhs->ty->kind == TY_PTR)
            printf("    imul rax, %d\n", size_of(node->rhs->ty->ptr_to));
        printf("    sub rax, rdi\n");
        if (node->lhs->ty->kind == TY_PTR)
            swap(&node->lhs, &node->rhs);
        if (node->lhs->ty->kind == TY_PTR)
            error("<pointer> - <pointer> is not defined!");
        node->ty = node->rhs->ty;
        break ;
    }
    case ND_MUL: {
        printf("    imul rax, rdi\n");
        if (node->lhs->ty->kind != TY_PTR)
            swap(&node->lhs, &node->rhs);
        if (node->lhs->ty->kind == TY_PTR)
            error("<pointer> * <***> is not defined!");
        node->ty = node->lhs->ty;
        break ;
    }
    case ND_DIV: {
        printf("    cqo\n");
        printf("    idiv rdi\n");
        if (node->lhs->ty->kind != TY_PTR)
            swap(&node->lhs, &node->rhs);
        if (node->lhs->ty->kind == TY_PTR)
            error("<pointer> / <***> is not defined!");
        node->ty = node->lhs->ty;
        break ;
    }
    case ND_EQ: {
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        node->ty = (t_Type *)calloc(1, sizeof(t_Type));
        node->ty->kind = TY_INT;
        break ;
    }
    case ND_NE: {
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        node->ty = (t_Type *)calloc(1, sizeof(t_Type));
        node->ty->kind = TY_INT;
        break ;
    }
    case ND_LT: {
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        node->ty = (t_Type *)calloc(1, sizeof(t_Type));
        node->ty->kind = TY_INT;
        break ;
    }
    case ND_LE: {
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        node->ty = (t_Type *)calloc(1, sizeof(t_Type));
        node->ty->kind = TY_INT;
        break ;
    }
    }
    printf("    push rax\n");
}
