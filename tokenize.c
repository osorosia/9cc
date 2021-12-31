#include "9cc.h"

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    int     pos;

    pos = loc - user_input;
    va_start(ap, fmt);
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

int expect_number(void) {
    t_Token *cur = g_token;

    if (g_token->kind != TK_NUM)
        error_at(g_token->str, "Not numeric!");
    g_token = g_token->next;
    return cur->val;
}

t_Token *new_token(t_TokenKind kind, t_Token *cur, char *str, int len) {
    t_Token *next = (t_Token *)calloc(1, sizeof(t_Token));

    next->kind = kind;
    next->str = str;
    next->len = len;
    cur->next = next;
    return next;
}

int is_reserved(char *p) {
    static char *kw[] = {
        "return", "if", "else", "while", "for",
        "int", "sizeof",
    };
    for (int i = 0; i < sizeof(kw) / sizeof(kw[0]); i++) {
        int len = strlen(kw[i]);
        if (startswith(p, kw[i]) && !is_alnum(p[len]))
            return len;
    }
    return 0;
}

t_Token *tokenize(void) {
    t_Token head;
    t_Token *cur = &head;
    char *p = user_input;

    head.next = NULL;
    while (*p) {
        if (isspace(*p)) {
            p++;
            continue ;
        }
        if (startswith(p, "==") || startswith(p, "!=") 
                || startswith(p, "<=") || startswith(p, ">=")) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue ;
        }
        if (strchr("+-*/&()<>;={},[]", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue ;
        }
        int len = is_reserved(p);
        if (len > 0) {
             cur = new_token(TK_RESERVED, cur, p, len);
             p += len;
             continue ;
        }
        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue ;
        }
        if (isalpha(*p) || *p == '_') {
            len = 0;
            while (is_alnum(p[len]))
                len++;
            cur = new_token(TK_IDENT, cur, p, len);
            p += cur->len;
            continue ;
        }
        error_at(p, "Invalid Token!");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

bool at_eof(void) {
    return g_token->kind == TK_EOF;
}

t_Token *consume_token(t_TokenKind kind) {
    t_Token *cur = g_token;

    if (g_token->kind != kind)
        return NULL;
    g_token = g_token->next;
    return cur;
}

t_Token *peek_token(t_TokenKind kind) {
    if (g_token->kind != kind)
        return NULL;
    return g_token;
}

bool consume(char *op) {
    if (g_token->kind != TK_RESERVED
            || g_token->len != strlen(op)
            || memcmp(g_token->str, op, g_token->len))
        return false;
    g_token = g_token->next;
    return true;
}

bool peek(char *op, int len) {
    t_Token *tok = g_token;

    while (len-- && tok->kind != TK_EOF)
        tok = tok->next;
    if (tok->kind != TK_RESERVED
            || tok->len != strlen(op)
            || memcmp(tok->str, op, tok->len))
        return false;
    return true;
}

void expect(char *op) {
    if (g_token->kind != TK_RESERVED
            || g_token->len != strlen(op)
            || memcmp(g_token->str, op, g_token->len))
        error_at(g_token->str, "expected '%s'!", op);
    g_token = g_token->next;
}
