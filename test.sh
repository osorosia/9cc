#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}



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

echo OK
