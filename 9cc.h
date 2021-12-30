#ifndef _9CC_H_
#define _9CC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
}    t_TokenKind;

typedef struct s_Token  t_Token;
struct s_Token {
    t_TokenKind kind;
    t_Token     *next;
    int         val;
    char        *str;
    int         len;
};

typedef    enum {
    ND_ADD,    // +
    ND_SUB,    // -
    ND_MUL,    // *
    ND_DIV,    // /
    ND_ADDR,   // unary &
    ND_DEREF,  // unary *
    ND_SIZEOF, // "sizeof"
    ND_EQ,     // ==
    ND_NE,     // !=
    ND_LT,     // <
    ND_LE,     // <=
    ND_ASSIGN, // =
    ND_RETURN, // "return"
    ND_IF,     // "if"
    ND_ELSE,   // "else"
    ND_WHILE,  // "while"
    ND_FOR,    // "for"
    ND_LVAR,   // Variable
    ND_CALL,   // call Function 
    ND_ARGS,   // Arguments
    ND_BLOCK,  // "{}"
    ND_NUM,    // Number
}    t_NodeKind;

typedef struct s_Node   t_Node;
typedef struct s_Type   t_Type;
typedef struct s_Obj    t_Obj;

struct  s_Node {
    t_NodeKind  kind;
    t_Node      *next;
    t_Node      *lhs;
    t_Node      *rhs;

    // "if" or "for" statement
    t_Node      *init;
    t_Node      *cond;
    t_Node      *update;
    t_Node      *then;
    t_Node      *els;

    // Function call
    t_Node      *args;

    // Block or statement
    t_Node      *body;

    // Function or variable name and length
    char        *name;
    int         len;

    // Number
    int         val;

    // Stack offset
    int         offset;
    
    // Variable
    t_Obj       *var;
    t_Type      *ty;
};

struct s_Obj {
    t_Obj   *next;
    char    *name;
    int     len;
    t_Token *tok;
    t_Type  *ty;
    
    // Local variable
    int     offset;

    // Function
    t_Obj   *locals;
    t_Node  *body;
    int     args_len;
};

typedef enum {
    TY_INT,
    TY_PTR,
    TY_ARRAY,
} t_TypeKind;

struct s_Type {
    t_TypeKind  kind;
    t_Type      *ptr_to;
    int         array_size;
};

//
// tokenize.c
//
void    error(const char *fmt, ...);
bool    at_eof(void);
t_Token *consume_token(t_TokenKind kind);
t_Token *peek_token(t_TokenKind kind);
int     expect_number(void);
void    expect(char *op);    
bool    peek(char *op, int len);
bool    consume(char *op);
t_Token *tokenize(void);

//
// parse.c
//
void    program(void);

//
// codegen.c
//
void    gen(t_Node *node);
void    gen_func_args(char *str, int offset);

//
// utils.c
//
void    error(const char *fmt, ...);
bool    startswith(char *p, char *q);
bool    is_alnum(char c);

// 
// Gloval variable
// 
t_Token *g_token;
char    *user_input;
t_Obj   *g_program;
int     g_tag_num;

#endif
