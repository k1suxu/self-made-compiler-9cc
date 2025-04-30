# self-made-compiler-9cc

## Abstract
Making a classic C compiler following the textbook (online):
https://www.sigbus.info/compilerbook#%E6%A9%9F%E6%A2%B0%E8%AA%9E%E3%81%A8%E3%82%A2%E3%82%BB%E3%83%B3%E3%83%96%E3%83%A9

## Progress According to the textbook

☑　Step. 1 "Single Integer"

☑　Step. 2 "Addition and Subtraction"

☑　Step. 3 "Simple Tokenizer"

☑　Step. 4 "Improved Error Message"

☑　Step. 5 "Arithmetic Operations"

☑　Step. 6 "Unary Operator (Add, Sub) (`+`, `-`)"

☑　Step. 7 "Comparison Operator"

☑　Step. 8 "Stop using single file (divide files into components), introduce Makefile"

☑　Step. 9 "Single-Letter Local Variable"

☑　Step. 10 "Multi-Letter Local Variable"

☑　Step. 11 "Return Statement"

☑　Step. 12 "`if`, `else`, `while`, `for`"

☑　Step. 13 "Code Block `{}`"

☑　Step. 14 "Call Function"

☑　Step. 15 "Declare Function"

☑　Step. 16 "Unary Operator (Address, Deref) (`&`, `*`)"

☑　Step. 17 "Introduce Type Keyword `int` "

☑　Step. 18 "Introduce Pointer Type"
  - define pointer type
  - substitution using pointer (deref)

☑　Step. 19 "Addition and Subtraction of Pointer"

☑　Step. 20 "`sizeof` Operator"

☑　Step. 21 "Implement Array Structure"
  - declaration of array
  - implicit casting (array → pointer)
  - element accessing via pointer operator (deref)



## Other Features
- Improved Error Message (can detect where the first bug in codes occured).
- Simple Type Annotation
  - Can detect type-mismatch of any operator and function call arguments
  - Can detect mismatch of the number of arguments of function call
- Allocate stack memory dependent on type (int→4, ptr→8, array→array_size*elem_type_size)








