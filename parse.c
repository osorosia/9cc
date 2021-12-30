#include "9cc.h"

t_Node *new_node(t_NodeKind    kind, t_Node *lhs, t_Node *rhs) {
    t_Node  *node;

    node = calloc(1, sizeof(t_Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return (node);
}

t_Node *new_node_num(int val) {
    t_Node  *node;
    t_Type  *ty;

    node = new_node(ND_NUM, NULL, NULL);
    node->val = val;
    ty = (t_Type *)calloc(1, sizeof(t_Type));
    ty->kind = TY_INT;
    node->ty = ty;
    return (node);
}

t_Obj *new_obj_func(t_Obj *cur) {
    t_Obj *next;

    next = (t_Obj *)calloc(1, sizeof(t_Obj));
    cur->next = next;
    return (next);
}

t_Node *new_node_if(t_NodeKind kind, t_Node *cond, t_Node *then, t_Node *els) {
    t_Node *node;

    node = new_node(kind, NULL, NULL);
    node->cond = cond;
    node->then = then;
    node->els = els;
    return (node);
}

t_Node *new_node_while(t_NodeKind kind, t_Node *cond, t_Node *then) {
    t_Node *node;

    node = new_node(kind, NULL, NULL);
    node->cond = cond;
    node->then = then;
    return (node);
}

t_Node *new_node_for(t_Node *init, t_Node *cond, t_Node *update, t_Node *then) {
    t_Node *node;

    node = new_node(ND_FOR, NULL, NULL);
    node->init = init;
    node->cond = cond;
    node->update = update;
    node->then = then;
    return (node);
}

t_Node *new_body(t_Node *cur_body, t_Node *node) {
    t_Node *next_body;

    next_body = (t_Node *)calloc(1, sizeof(t_Node));
    cur_body->next = next_body;
    next_body->body = node;
    return (next_body);
}

t_Obj *find_lvar(t_Token *token) {
    for (t_Obj *var = g_program->locals; var; var = var->next) {
        if (token->len == var->len 
            && !memcmp(token->str, var->name, token->len))
            return (var);
    }
    return (NULL);
}

t_Node    *new_node_ident(t_Token *token, t_Type *ty) {
    t_Node    *node;
    t_Obj    *lvar;

    node = (t_Node *)calloc(1, sizeof(t_Node));
    node->kind = ND_LVAR;
    lvar = find_lvar(token);
    if (lvar) {
        node->offset = lvar->offset;
        node->ty = lvar->ty;
    } else {
        lvar = (t_Obj *)calloc(1, sizeof(t_Obj));
        lvar->next = g_program->locals;
        lvar->name = token->str;
        lvar->len = token->len;
        lvar->offset = g_program->locals ? g_program->locals->offset + 8 * ty->array_size : 8 * ty->array_size;
        lvar->ty = ty;
        node->offset = lvar->offset;
        node->ty = ty;
        g_program->locals = lvar;
    }
    node->var = lvar;
    return (node);
}

t_Node *new_node_call(t_Token *token) {
    t_Node *node;

    node = (t_Node *)calloc(1, sizeof(t_Node));
    node->kind = ND_CALL;
    node->name = token->str;
    node->len = token->len;
    node->ty = (t_Type *)calloc(1, sizeof(t_Type));
    return (node);
}

t_Node *new_node_args(t_Node *args, t_Node *node) {
    t_Node  *next;

    next = new_node(ND_ARGS, node, NULL);
    args->args = next;
    return (next);
}

t_Type *new_type(t_TypeKind kind, t_Type *ptr_to, int array_size) {
    t_Type *ty = (t_Type *)calloc(1, sizeof(t_Type));
    ty->kind = kind;
    ty->ptr_to = ptr_to;
    ty->array_size = array_size;
    return ty;
}

void    program();
t_Node  *stmt();
t_Node  *expr();
t_Node  *assign();
t_Node  *equality();
t_Node  *relational();
t_Node  *add();
t_Node  *mul();
t_Node  *unary();
t_Node  *primary();
t_Type  *typ();

// program = (typ ident "(" (typ ident( "," typ ident)*)? ")" "{" stmt "}")*
void program() {
    t_Obj   head;
    t_Token *token;
    t_Node  *locals;
    t_Type  *ty;

    head.next = NULL;
    g_program = &head;
    while (!at_eof()) {
        g_program = new_obj_func(g_program);
        ty = typ();
        token = consume_token(TK_IDENT);
        if (!token)
            error("expected identifier!");
        g_program->name = token->str;
        g_program->len = token->len;
        g_program->ty = ty;
        expect("(");
        if (!peek(")", 0)) {
            ty = typ();
            token = consume_token(TK_IDENT);
            if (!token)
                error("expected identifier!");
            new_node_ident(token, ty);
            g_program->args_len++;
            while (consume(",")) {
                ty = typ();
                token = consume_token(TK_IDENT);
                if (!token)
                    error("expected identifier!");
                new_node_ident(token, ty);
                g_program->args_len++;
            }
        }
        expect(")");
        if (!peek("{", 0))
            error("expected '{' !");
        g_program->body = stmt();
    }
    g_program = head.next;    
}

// stmt = expr ";"
//      | "{" stmt* "}"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | "return" expr? ";"
//      | typ ident ("=" expr)? ";"
//      | typ ident "[" num "]" ";"
t_Node    *stmt() {
    // "{" stmt* "}"
    if (consume("{")) {
        t_Node head;
        head.next = NULL;
        t_Node *node_body = &head;
        while (!consume("}"))
            node_body = new_body(node_body, stmt());
        t_Node *node = new_node(ND_BLOCK, NULL, NULL);
        node->next = head.next;
        return node;
    }

    // "if" "(" expr ")" stmt ("else" stmt)?
    if (consume("if")) {
        expect("(");
        t_Node *node_cond = expr();
        expect(")");
        t_Node *node_then = stmt();
        t_Node *node_else = NULL;
        if (consume("else"))
            node_else = stmt();
        t_Node *node = new_node_if(ND_IF, node_cond, node_then, node_else);
        return node;
    }

    // "while" "(" expr ")" stmt
    if (consume("while")) {
        expect("(");
        t_Node *node_cond = expr();
        expect(")");
        t_Node *node_then = stmt();
        t_Node *node = new_node_while(ND_WHILE, node_cond, node_then);
        return node;
    }

    // "for" "(" expr? ";" expr? ";" expr? ")" stmt
    if (consume("for")) {
        expect("(");
        t_Node *node_init = NULL;
        t_Node *node_cond = NULL;
        t_Node *node_update = NULL;
        if (!peek(";", 0))
            node_init = expr();
        expect(";");
        if (!peek(";", 0))
            node_cond = expr();
        expect(";");
        if (!peek(")", 0))
            node_update = expr();
        expect(")");
        t_Node *node_then = stmt();
        t_Node *node = new_node_for(node_init, node_cond, node_update, node_then);
        return node;
    }

    // "return" expr? ";"
    if (consume("return")) {
        t_Node *node = new_node(ND_RETURN, expr(), NULL);
        expect(";");
        return node;
    }

    // typ ident ("=" expr)? ";"
    // typ ident "[" num "]" ";"
    if (peek("int", 0)) {
        t_Type *ty = typ();
        t_Token *token = peek_token(TK_IDENT);
        if (!token)
            error("expected identifier!");
        if (find_lvar(token))
            error("redefinition of '%.*s'!\n", token->len, token->str);
        t_Node *node = NULL;
        if (peek("=", 1)) {
            new_node_ident(token, ty);
            node = expr();
        } else {
            consume_token(TK_IDENT);
            if (consume("[")) {
                ty = new_type(TY_ARRAY, ty, expect_number());
                expect("]");
            }
            new_node_ident(token, ty);
        }
        expect(";");
        return node;
    }

    // expr ";"
    t_Node *node = expr();
    expect(";");
    return node;
}

// expr = assign
t_Node *expr() {
    return assign();
}

// assign = equality ("=" assign)?
t_Node *assign() {
    t_Node *node = equality();

    if (consume("="))
        node = new_node(ND_ASSIGN, node, assign());
    return node;
}

// equality = relational ("==" relational | "!=" relational)*
t_Node *equality() {
    t_Node *node = relational();

    for (;;) {
        if (consume("=="))
            node = new_node(ND_EQ, node, relational());
        else if (consume("!="))
            node = new_node(ND_NE, node, relational());
        else
            break;
    }
    return node;
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
t_Node *relational() {
    t_Node *node = add();
    
    for (;;) {
        if (consume("<"))
            node = new_node(ND_LT, node, add());
        else if (consume("<="))
            node = new_node(ND_LE, node, add());
        else if (consume(">"))
            node = new_node(ND_LT, add(), node);
        else if (consume(">="))
            node = new_node(ND_LE, add(), node);
        else
            break;
    }
    return node;
}

// add = mul ("+" mul | "-" mul)*
t_Node *add() {
    t_Node *node = mul();

    for (;;) {
        if (consume("+"))
            node = new_node(ND_ADD, node, mul());
        else if (consume("-"))
            node = new_node(ND_SUB, node, mul());
        else
            break;
    }
    return node;
}

// mul = unary ("*" unary | "/" unary)*
t_Node *mul() {
    t_Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary());
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary());
        else
            break;
    }
    return node;
}

// unary = ("+" | "-")? primary
//       | ("*" | "&" | "sizeof") unary
t_Node *unary() {
    if (consume("sizeof"))
        return (new_node(ND_SIZEOF, unary(), NULL));
    if (consume("*"))
        return (new_node(ND_DEREF, unary(), NULL));
    if (consume("&"))
        return (new_node(ND_ADDR, unary(), NULL));
    if (consume("+"))
        return (unary());
    if (consume("-"))
        return (new_node(ND_SUB, new_node_num(0), unary()));
    return (primary());
}

// primary = num
//         | (ident ( "(" (expr ( "," expr)*)? ")" )?
//         | "(" expr ")"
t_Node *primary() {
    t_Node    *node;
    t_Token   *token;
    t_Node    *args;
    t_Node    args_head;

    // "(" expr ")"
    if (consume("(")) {
        node = expr();
        expect(")");
        return (node);
    }
    // (ident ( "(" (expr ( "," expr)*)? ")" )?
    token = consume_token(TK_IDENT);
    if (token) {
        if (consume("(")) {
            node = new_node_call(token);
            if (consume(")"))
                return (node);
            args_head.args = NULL;
            args = &args_head;
            args = new_node_args(args, expr());
            while (consume(","))
                args = new_node_args(args, expr());
            expect(")");
            node->args = args_head.args;
            return (node);
        }
        if (!find_lvar(token))
            error("'%.*s' undeclared!", token->len, token->str);
        return new_node_ident(token, NULL);
    }
    // num
    return new_node_num(expect_number());
}

// typ = "int" "*"*
t_Type *typ() {
    t_Type *ty = new_type(TY_INT, NULL, 1);
    expect("int");
    while (consume("*"))
        ty = new_type(TY_PTR, ty, 1);
    return ty;
}
