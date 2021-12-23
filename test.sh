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

assert 0 "for(i = 1; i < 100; i = i + 1)foo(); return 0;"
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




echo OK
