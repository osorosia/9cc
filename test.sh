#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s foo.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert_vs_gcc() {
  input="$1"

  echo "$input" > tmp.c
  gcc -c tmp.c
  cc -o tmp tmp.o foo.o
  ./tmp
  expected="$?"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s foo.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert_stdout() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s foo.o
  ./tmp > tmp.stdout
  actual="$(cat tmp.stdout)"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}


assert 3 'int main() {int a = 3; return a;}'
assert 3 'int main() {int a; a = 3; return a;}'
assert 103 'int hoge(int a){return a + 100;}  int main() {return hoge(3);}'
assert 107 'int hoge(int a, int b){return a + b + 100;}  int main() {return hoge(3, 4);}'
assert 107 'int hoge(int a, int b){return a + b + 100;}  int main() {int a = 3;return hoge(a, 4);}'
assert 107 'int hoge(int a, int b){return a + b + 100;}  int main() {int a = 3; int b = a + 1; return hoge(a, b);}'
assert 107 'int hoge(int a, int b){return a + b + 100;}  int main() {int a = 1; a = 3; return hoge(a, 4);}'
assert 3 '
int main() {
  int x;
  int *y;
  y = &x;
  *y = 3;
  return x;
}'

assert 8 '
int fibo(int i)
{
	if (i <= 0)
	{
		return 0;
	} 
	if (i <= 2) 
	{
		return 1;
	} 
	return fibo(i - 1) + fibo(i - 2);
}
int main() 
{ 
	return fibo(6); 
}'

assert 4 '
int main() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  int *q;
  q = p + 2;
  return *q;
}
'

assert 2 '
int main() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  return *(p + 1);
}
'

assert 8 'int main() { int *p = alloc1(3, 5); return *p + *(p + 1); }'
assert 9 'int main() { int *p = alloc2(2, 7); return *p + *(p - 1); }'
assert 2 'int main() { int **p = alloc_ptr_ptr(2); return **p; }'

assert 4 'int main() {int x; int *y; return sizeof(x);}'
assert 8 'int main() {int x; int *y; return sizeof(y);}'
assert 4 'int main() {int x; int *y; return sizeof(x + 3);}'
assert 8 'int main() {int x; int *y; return sizeof(y + 3);}'
assert 4 'int main() {int x; int *y; return sizeof(*y);}'
assert 4 'int main() {int x; int *y; return sizeof(1);}'
assert 4 'int main() {int x; int *y; return sizeof(sizeof(1));}'

assert 4 'int main() {int x; int *y; return sizeof x;}'
assert 8 'int main() {int x; int *y; return sizeof y;}'
assert 4 'int main() {int x; int *y; return sizeof *y;}'
assert 4 'int main() {int x; int *y; return sizeof 1;}'
assert 4 'int main() {int x; int *y; return sizeof sizeof(1);}'

assert 3 '
int main() {
  int p[2];
  *p = 3;
  *(p + 1) = 5;
  return *p;
}
'

echo OK
exit

echo vvvvvv TODO vvvvvv
assert 107 'int hoge(int a, int b);  int main() {int a = 1; a = 3; return hoge(a, 4);} int hoge(int a, int b){return a + b + 100;}'
assert 107 ' int main() {int a = 1; a = 3; return hoge(a, 4);} int hoge(int a, int b){return a + b + 100;}' 
# assert 3 'int main() {int a = 3; return a;}'
assert 2 '
int *f() {
  int *p;
  alloc4(&p, 1, 2, 4, 8);
  return p;
}
int main() {
  return *(f() + 1);
}
'

exit
