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
echo OK
exit

echo vvvvvv TODO vvvvvv
assert 107 'int hoge(int a, int b);  int main() {int a = 1; a = 3; return hoge(a, 4);} int hoge(int a, int b){return a + b + 100;}'
assert 107 ' int main() {int a = 1; a = 3; return hoge(a, 4);} int hoge(int a, int b){return a + b + 100;}' 
# assert 3 'int main() {int a = 3; return a;}'
exit

assert 3 'main() {return three();}'
assert 3 'main() {return three();} hoge() {return 1;}'
assert 3 'hoge() {return 1;} main() {return three();}'
assert 2 'hoge(a) {return a + 1;} main() { return hoge(1); return three();}'
assert 4 'hoge(a, b) {return a + b + 1;} main() { return hoge(1, 2); return three();}'
assert 1 'hoge(a, b)
{
	if (a > 2)
	{
		return 1;
	}
	return a + 1 + b + 1;
}
main()
{
	hoge(1, 1);
	return 1;
}'
assert 8 '
fibo(i)c
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
main() 
{ 
	return fibo(6); 
}'

assert 3 '
main(){
  x = 3;
  z = 9;
  y = &x;
  return *y;
}'

assert 3 '
main(){
  x = 3;
  y = 5;
  z = &y + 8;
  return *z;
}'

echo OK

exit

assert 3 'return three();'
assert 3 'foo(); return 3;'
assert 3 'foo(); return 3;'
assert 3 'foo(); return 3;'
assert 3 'a = 1; foo(); return 3;'
assert 3 'a = 1; b = 1; foo(); return 3;'
assert 3 'a = 1; b = 1; c = 1; foo(); return 3;'
assert 3 'foo1(1); return 3;'
assert 3 'foo2(1,2); return 3;'
assert 3 'foo3(1,2,3); return 3;'
assert 3 'foo4(1,2,3,4); return 3;'
assert 3 'foo5(1,2,3,4,5); return 3;'
assert 3 'foo6(1,2,3,4,5,6); return 3;'

assert 1 'foo(); return 1;'
assert 1 'foo(); b = 1; return 1;'
assert 1 'foo(); b = 1; return 1;'
assert 1 'b = 1; foo(); return 1;'
assert 1 '{} return 1;'
assert 1 '{ return 1; }'

assert 10 "if (1) {
    a = 10;
    b = 2;
    return a;
} else {
    a = 1;
    b = 3;
    return b;
}"
assert 1 "{ return 1; }"

assert 42 "a = 0; for(;;) if ((a = a + 1) ==  42) return a; return 10;"
assert 42 "a = 0; for(i = 0; i < 42; i = i + 1) a = a + 1; return a;"
assert 42 "i = 0; while(i < 42) i = i + 1; return i;"
assert 42 "i = 0; while(1) if ((i = i + 1) == 42) return i;"
assert 21 "if(0) return 42; else return 21; return 10;"
assert 10 "a = 3; if(a < 0) return 10; else if(a < 2) return 20; else if(a < 4) return 10; else return 100;"
assert 10 'if (0) 2; else 10;'
assert 2 "a = 1;
b = 1;
if (a - b) a - b; else a + b;"
assert 42 "if(1) return 42; return 21;"
assert 42 "if(1) return 42; return 21;"
assert 42 "if(1==1) return 42; return 21;"
assert 42 "a=1; if(a) return 42; return 21;"
assert 42 "a=1; if(a=2) return 42; return 21;"
assert 2 "a=1; if(a=2) return a; return 21;"
assert 21 "if(1==0) return 42; return 21;"
assert 21 "if(0) return 42; return 21;"
assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4 "(3+5)/2;"
assert 8 "3+3*2-1;"
assert 18 "3+3*2-1+10;"
assert 10 "-10+20;"
assert 10 "--10;"
assert 10 "- -10;"
assert 10 "- - +10;"

assert 0 "0==1;"
assert 1 "42==42;"
assert 1 "0!=1;"
assert 0 "42!=42;"

assert 1 "0<1;"
assert 0 "1<1;"
assert 0 "2<1;"
assert 1 "0<=1;"
assert 1 "1<=1;"
assert 0 "2<=1;"

assert 1 "1>0;"
assert 0 "1>1;"
assert 0 "1>2;"
assert 1 "1>=0;"
assert 1 "1>=1;"
assert 0 "1>=2;"
assert 0 "1==1!=1;"
assert 1 "a = 1; a = a - 1; a == 0;"

assert 52 'a = 52;a;'
assert 1 "a = 1;
a;
"
assert 14 "a = 3;
b = 5 * 6 - 8;
a + b / 2;"

assert 6 "foo = 1;
bar = 2 + 3;
foo + bar;"
assert 2 "return 2;"
assert 2 "return 2;
return 1;"
assert 6 "foo = 1;
bar = 2 + 3;
return foo + bar;"
assert 42 "return 42; 21;"
assert 24 "yama = 57;
hon = 33; return yama = hon - 9;"
assert 57 "_a = 57; return _a;"
assert 1 "ABCD = 1; return ABCD;"
assert 42 "A0cd = 42; return A0cd;"
assert 10 "end_ = 10; return end_;"

assert_stdout OK 'foo(); return 1;'
assert_stdout OK 'foo(); return 2;'
assert_stdout OK 'foo(); return 3;'
assert_stdout OK 'a = 1; foo(); return 3;'
assert_stdout OK 'a = 1; b = 1; foo(); return 3;'
assert_stdout OK 'a = 1; b = 1; c = 1; foo(); return 3;'
assert_stdout 1,OK 'foo1(1); return 3;'
assert_stdout 1,2,OK 'foo2(1,2); return 3;'
assert_stdout 1,2,3,OK 'foo3(1,2,3); return 3;'
assert_stdout 1,2,3,4,OK 'foo4(1,2,3,4); return 3;'
assert_stdout 1,2,3,4,5,OK 'foo5(1,2,3,4,5); return 3;'
assert_stdout 1,2,3,4,5,6,OK 'foo6(1,2,3,4,5,6); return 3;'

echo OK
