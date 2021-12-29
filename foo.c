#include <stdio.h>
#include <stdlib.h>
int foo() { printf("OK\n"); return 0;};
int foo1(int x1) { printf("%d,OK\n", x1); return 0;};
int foo2(int x1, int x2) { printf("%d,%d,OK\n", x1, x2); return 0;};
int foo3(int x1, int x2, int x3) { printf("%d,%d,%d,OK\n", x1, x2, x3); return 0;};
int foo4(int x1, int x2, int x3, int x4) { printf("%d,%d,%d,%d,OK\n", x1, x2, x3, x4); return 0;};
int foo5(int x1, int x2, int x3, int x4, int x5) { printf("%d,%d,%d,%d,%d,OK\n", x1, x2, x3, x4, x5); return 0;};
int foo6(int x1, int x2, int x3, int x4, int x5, int x6) { printf("%d,%d,%d,%d,%d,%d,OK\n", x1, x2, x3, x4, x5, x6); return 0;};

int three() {return 3;}

void alloc4(int **p, int a0, int a1, int a2, int a3)
{
    *p = (int *)malloc(sizeof(int) * 4);
    (*p)[0] = a0;
    (*p)[1] = a1;
    (*p)[2] = a2;
    (*p)[3] = a3;
}

int *alloc1(int x, int y) {
  static int arr[2];
  arr[0] = x;
  arr[1] = y;
  return arr;
}
int *alloc2(int x, int y) {
  static int arr[2];
  arr[0] = x;
  arr[1] = y;
  return arr + 1;
}
int **alloc_ptr_ptr(int x) {
  static int **p;
  static int *q;
  static int r;
  r = x;
  q = &r;
  p = &q;
  return p;
}
