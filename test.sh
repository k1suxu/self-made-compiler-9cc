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
assert 0 'int main() {0;}'
assert 42 'int main() { 42; }'
assert 21 'int main() { 5+20-4; }'
assert 41 'int main() {  12 + 34 - 5 ; }'
assert 41 'int main() {  12 + 34 - 5;  }'
assert 47 'int main() { 5+6*7; }'
assert 15 'int main() { 5*(9-6); }'
assert 4 'int main() { (3+5)/2; }'
assert 10 'int main() { -(2+8)+20; }'
assert 10 'int main() { +20-(13-3); }'
assert 10 'int main() { -(-(-(2+8)+20)); }'

assert 0 'int main() { 0==1; }'
assert 1 'int main() { 42==42; }'
assert 1 'int main() { 0!=1; }'
assert 0 'int main() { 42!=42; }'

assert 1 'int main() { 0<1; }'
assert 0 'int main() { 1<1; }'
assert 0 'int main() { 2<1; }'
assert 1 'int main() { 0<=1; }'
assert 1 'int main() { 1<=1; }'
assert 0 'int main() { 2<=1; }'

assert 1 'int main() { 1>0; }'
assert 0 'int main() { 1>1; }'
assert 0 'int main() { 1>2; }'
assert 1 'int main() { 1>=0; }'
assert 1 'int main() { 1>=1; }'
assert 0 'int main() { 1>=2; }'
assert 2 'int main() { (1==2)+(1!=2)+(1<=2); }'

assert 14 'int main() {int a; a = 3;
int b; b = 5 * 6 - 8;
a + b / 2;}'

assert 6 'int main() {int foo; foo = 1;
int bar; bar = 2 + 3;
foo + bar;}'
assert 1 'int main() {int fo_110o; fo_110o = 1;
int _bAr; _bAr = 2 + 3 * fo_110o;
int Hoge_fo; Hoge_fo = -_bAr;
fo_110o + _bAr + Hoge_fo;
}'

assert 14 'int main() {int a; a = 3;
int b; b = 5 * 6 - 8;
return a + b / 2;
}'
assert 5 'int main() {return 5;
return 8;}'
assert 10 'int main() {int returnx; returnx = 10;
return returnx;}'
assert 10 'int main() {int x; x = 4;
x = x * 2 + 2;
return x;}'

# if test
assert 1 'int main() {if(5+3-1) return 1; else return 0;}'
assert 0 'int main() {if(5+3-8) return 1; else return 0;}'
assert 3 'int main() { int a; int b; a = 3; b = 5;
if (2 * a - b != 0) return a; else return b; }'
assert 3 'int main() {int a; int b; a = 3; b = 5;
if (2 * a - b) return a; else return b;}'
assert 5 'int main() {int a; int b; a = 3; b = 5;
if ((2 * a - b) - 1) return a; else return b;}'
assert 1 'int main() {if (1) return 1; else if (1) return 2; else return 3; }'
assert 1 'int main() {if (1) return 1; else if (0) return 2; else return 3; }'
assert 2 'int main() {if (0) return 1; else if (1) return 2; else return 3; }'
assert 3 'int main() {if (0) return 1; else if (0) return 2; else return 3; }'
assert 3 'int main() {if (0) {
return 1;
}else if (0) {
return 2;} else return 3;
}'

# while test
assert 16 'int main() {int i; i = 1;
while (i < 10) i = i + i;
return i;}'

# for test
assert 55 'int main() {int i; int j; i = 0;
for (j = 0; j <= 10; j = j + 1) i = i + j;
return i;}'
assert 29 'int main() {int i; int j; i = 0;
for (j = 1; j * 2 < 20; j = j * 2) i = i + j;
if (i == 15) i = i + 4;
for (j = 0; j < 5; j = j + 1) i = i + j;
return i;}'
assert 100 'int main() {int i; int j; int k; i = 0;
for (j = 0; j < 5; j = j + 1)
  for (k = 0; k < 5; k = k + 1)
    i = i + j * k;
return i;}'
assert 23 'int main() {int i; int j; int k; int l; i = 0;
for (j = 1; j < 5; j = j + 1)
  for (k = 0; j * k < 5; k = k + 1)
    i = i + j * k;
return i;}'

# code-block test
assert 23 'int main() {int i; int j; int k; i = 0;
for (j = 1; j < 5; j = j + 1) {
  for (k = 0; j * k < 5; k = k + 1)
    i = i + j * k;
}
return i;}'
assert 23 'int main() {int i; int j; int k; i = 0;
for (j = 1; j < 5; j = j + 1) {
  for (k = 0; j * k < 5; k = k + 1) {
    i = i + j * k;
  }
}
return i;}'
assert 23 'int main() {int i; int j; int k; i = 0;
for (j = 1; j < 5; j = j + 1)
  for (k = 0; j * k < 5; k = k + 1) {
    i = i + j * k;
  }
return i;}'
assert 218 'int main() {int i; int j; int k; i = 0;
for (j = 0; j < 4; j = j + 1) {
  for (k = 0; k < 5; k = k + 1) {
    int l;
    for (l = 0; l < 6; l = l + 1) {
      int lo; int hi;
      lo = 0;
      hi = 100;
      while (hi - lo > 1) {
        int mid;
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
assert_with 10 'int main() {return foo();}'
assert_with 15 'int main() {return hoge(5,10);}'
assert_with 30 'int main() {return hoge(foo(),foo()) + foo();}'

# function call stack size test
assert_with 48 'int main() {int i; int j; int k; i = 0;
for (j = 0; j < 3; j = j + 1) {
  i = i + hoge(5,10);
}
k = 3;
i = i + k;
return i;}'

# function def test
assert 10 'int hoge() {
  return 10;
}
int main() {
  return hoge();
}'
assert 15 '
int hoge(int a, int b) {
  return a + b;
}
int main() {
  return hoge(5,10);
}'
assert 55 'int fib(int n) {
  if (n < 2) return n;
  return fib(n-1) + fib(n-2);
}
int main() {
  return fib(10);
}'
assert 144 'int fib(int n) {
  if (n < 2) return n;
  return fib(n-1) + fib(n-2);
}
int main() {
  return fib(12);
}'
assert 30 'int foo() {
  return 10;
}
int hoge(int a, int b) {
  return a + b;
}
int main() {
  return hoge(foo(),foo()) + foo();
}'

# unary operators about address : & and *
assert 3 'int main() {
int x; int y;
x = 3;
y = &x;
return *y;}'
assert 3 'int main() {
int x; int y; int z;
x = 3;
y = 5;
z = &y + 8;
return *z;}'

# int-var-name test
assert 24 'int main() {
  int intx; int _int; int _int_; int _;
  intx = 3;
  _int = 5;
  _int_ = 7;
  _ = 9;
  return intx + _int + _int_ + _;
}'

# ptr-type test
assert 3 'int main() {
int x;
int *y;
y = &x;
*y = 3;
return x; }'
# ptr-ptr-type test
assert 3 'int main() {
int x;
int *y;
int **z;
x = 8;
y = &x;
z = &y;
**z = 3;
return x; }'
assert 3 'int main() {
int x;
int *y;
int **z;
int ***w;
x = 8;
y = &x;
z = &y;
w = &z;
***w = 3;
return x; }'
assert 218 'int main() {int i; int j; int k; i = 0;
int *i_ptr; int **i_ptr_ptr; int *j_ptr; int **j_ptr_ptr;
i_ptr = &i;
i_ptr_ptr = &i_ptr;
j_ptr = &j;
j_ptr_ptr = &j_ptr;
for (**j_ptr_ptr = 0; **j_ptr_ptr < 4; **j_ptr_ptr = **j_ptr_ptr + 1) {
  for (k = 0; k < 5; k = k + 1) {
    int l;
    int *l_ptr;
    l_ptr = &l;
    for (*l_ptr = 0; l < 6; *l_ptr = l + 1) {
      int lo; int hi;
      lo = 0;
      hi = 100;
      while (hi - lo > 1) {
        int mid;
        mid = (lo + hi) / 2;
        if (mid * mid > (*i_ptr + j) * k + *l_ptr) hi = mid;
        else lo = mid;
      }
      *i_ptr = **i_ptr_ptr + lo + 1;
    }
  }
}
return **i_ptr_ptr;
}'

# ptr return function test
# 規約違反ではない？？(ローカル変数のアドレスを返すのはよくない気もするが、これがコンパイル通るという記録のためにここに残しておく)
assert 3 'int *foo() {
  int x;
  x = 3;
  return &x;
}
int main() {
  int *y;
  y = foo();
  return *y;
}'



echo OK
