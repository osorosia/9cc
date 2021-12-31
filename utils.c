#include "9cc.h"

void error(const char *fmt, ...) {
    va_list ap;

    fprintf(stderr, "***");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "***");
    fprintf(stderr, "\n");
    exit(1);
}

bool startswith(char *p, char *q) {
    return memcmp(p, q, strlen(q)) == 0;
}

bool is_alnum(char c) {
    return ('a' <= c && c <= 'z')
            || ('A' <= c && c <= 'Z')
            || ('0' <= c && c <= '9')
            || (c == '_');
}

