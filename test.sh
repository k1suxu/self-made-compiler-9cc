#!/bin/bash
echo "#include<stdio.h>
int foo() {
  return 10;
}
int hoge(int a, int b) {
  return a + b;
}" > tmpDecl.c
cc -c tmpDecl.c -o tmpDecl.o

assert_with() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s tmpDecl.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

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

make
assert 0 'main() {0;}'
assert 42 'main() { 42; }'
assert 21 'main() { 5+20-4; }'
assert 41 'main() {  12 + 34 - 5 ; }'
assert 41 'main() {  12 + 34 - 5;  }'
assert 47 'main() { 5+6*7; }'
assert 15 'main() { 5*(9-6); }'
assert 4 'main() { (3+5)/2; }'
assert 10 'main() { -(2+8)+20; }'
assert 10 'main() { +20-(13-3); }'
assert 10 'main() { -(-(-(2+8)+20)); }'

assert 0 'main() { 0==1; }'
assert 1 'main() { 42==42; }'
assert 1 'main() { 0!=1; }'
assert 0 'main() { 42!=42; }'

assert 1 'main() { 0<1; }'
assert 0 'main() { 1<1; }'
assert 0 'main() { 2<1; }'
assert 1 'main() { 0<=1; }'
assert 1 'main() { 1<=1; }'
assert 0 'main() { 2<=1; }'

assert 1 'main() { 1>0; }'
assert 0 'main() { 1>1; }'
assert 0 'main() { 1>2; }'
assert 1 'main() { 1>=0; }'
assert 1 'main() { 1>=1; }'
assert 0 'main() { 1>=2; }'
assert 2 'main() { (1==2)+(1!=2)+(1<=2); }'

assert 14 'main() {a = 3;
b = 5 * 6 - 8;
a + b / 2;}'

assert 6 'main() {foo = 1;
bar = 2 + 3;
foo + bar;}'
assert 1 'main() {fo_110o = 1;
_bAr = 2 + 3 * fo_110o;
Hoge_fo = -_bAr;
fo_110o + _bAr + Hoge_fo;
}'

assert 14 'main() {a = 3;
b = 5 * 6 - 8;
return a + b / 2;
}'
assert 5 'main() {return 5;
return 8;}'
assert 10 'main() {returnx = 10;
return returnx;}'
assert 10 'main() {x = 4;
x = x * 2 + 2;
return x;}'

# if test
assert 1 'main() {if(5+3-1) return 1; else return 0;}'
assert 0 'main() {if(5+3-8) return 1; else return 0;}'
assert 3 'main() { a = 3; b = 5;
if (2 * a - b != 0) return a; else return b; }'
assert 3 'main() {a = 3; b = 5;
if (2 * a - b) return a; else return b;}'
assert 5 'main() {a = 3; b = 5;
if ((2 * a - b) - 1) return a; else return b;}'
assert 1 'main() {if (1) return 1; else if (1) return 2; else return 3; }'
assert 1 'main() {if (1) return 1; else if (0) return 2; else return 3; }'
assert 2 'main() {if (0) return 1; else if (1) return 2; else return 3; }'
assert 3 'main() {if (0) return 1; else if (0) return 2; else return 3; }'
assert 3 'main() {if (0) {
return 1;
}else if (0) {
return 2;} else return 3;
}'

# while test
assert 16 'main() {i = 1;
while (i < 10) i = i + i;
return i;}'

# for test
assert 55 'main() {i = 0;
for (j = 0; j <= 10; j = j + 1) i = i + j;
return i;}'
assert 29 'main() {i = 0;
for (j = 1; j * 2 < 20; j = j * 2) i = i + j;
if (i == 15) i = i + 4;
for (j = 0; j < 5; j = j + 1) i = i + j;
return i;}'
assert 100 'main() {i = 0;
for (j = 0; j < 5; j = j + 1)
  for (k = 0; k < 5; k = k + 1)
    i = i + j * k;
return i;}'
assert 23 'main() {i = 0;
for (j = 1; j < 5; j = j + 1)
  for (k = 0; j * k < 5; k = k + 1)
    i = i + j * k;
return i;}'

# code-block test
assert 23 'main() {i = 0;
for (j = 1; j < 5; j = j + 1) {
  for (k = 0; j * k < 5; k = k + 1)
    i = i + j * k;
}
return i;}'
assert 23 'main() {i = 0;
for (j = 1; j < 5; j = j + 1) {
  for (k = 0; j * k < 5; k = k + 1) {
    i = i + j * k;
  }
}
return i;}'
assert 23 'main() {i = 0;
for (j = 1; j < 5; j = j + 1)
  for (k = 0; j * k < 5; k = k + 1) {
    i = i + j * k;
  }
return i;}'
assert 218 'main() {i = 0;
for (j = 0; j < 4; j = j + 1) {
  for (k = 0; k < 5; k = k + 1) {
    for (l = 0; l < 6; l = l + 1) {
      lo = 0;
      hi = 100;
      while (hi - lo > 1) {
        mid = (lo + hi) / 2;
        if (mid * mid > (i + j) * k + l) hi = mid;
        else lo = mid;
      }
      i = i + lo + 1;
    }
  }
}
return i;
}'

# function call test
assert_with 10 'main() {return foo();}'
assert_with 15 'main() {return hoge(5,10);}'
assert_with 30 'main() {return hoge(foo(),foo()) + foo();}'

# function call stack size test
assert_with 48 'main() {i = 0;
for (j = 0; j < 3; j = j + 1) {
  i = i + hoge(5,10);
}
k = 3;
i = i + k;
return i;}'

# function def test
assert 10 'hoge() {
  return 10;
}
main() {
  return hoge();
}'
assert 15 '
hoge(a, b) {
  return a + b;
}
main() {
  return hoge(5,10);
}'
assert 55 'fib(n) {
  if (n < 2) return n;
  return fib(n-1) + fib(n-2);
}
main() {
  return fib(10);
}'
assert 144 'fib(n) {
  if (n < 2) return n;
  return fib(n-1) + fib(n-2);
}
main() {
  return fib(12);
}'
assert 30 'foo() {
  return 10;
}
hoge(a, b) {
  return a + b;
}
main() {
  return hoge(foo(),foo()) + foo();
}'

# unary operators about address : & and *
assert 3 'main() {
x = 3;
y = &x;
return *y;}'
assert 3 'main() {
x = 3;
y = 5;
z = &y + 8;
return *z;}'



echo OK
