# self-made-compiler-9cc

## Abstract
This is a classic C compiler implemented following the textbook available online:
https://www.sigbus.info/compilerbook#%E6%A9%9F%E6%A2%B0%E8%AA%9E%E3%81%A8%E3%82%A2%E3%82%BB%E3%83%B3%E3%83%96%E3%83%A9

The main goal of this project is to accomplish self-host.

## Scope of Coverage
Please kindly refer `test.sh` for detailed test cases.

## Progress According to the Textbook
☑ Step 1: "Single Integer" <br>
☑ Step 2: "Addition and Subtraction" <br>
☑ Step 3: "Simple Tokenizer" <br>
☑ Step 4: "Improved Error Messages" <br>
☑ Step 5: "Arithmetic Operations" <br>
☑ Step 6: "Unary Operators (`+`, `-`)" <br>
☑ Step 7: "Comparison Operators" <br>
☑ Step 8: "Refactor into Multiple Files and Introduce Makefile" <br>
☑ Step 9: "Single-Letter Local Variables" <br>
☑ Step 10: "Multi-Letter Local Variables" <br>
☑ Step 11: "`return` Statement" <br>
☑ Step 12: "`if`, `else`, `while`, `for`" <br>
☑ Step 13: "Code Blocks `{}`" <br>
☑ Step 14: "Function Calls" <br>
☑ Step 15: "Function Declarations" <br>
☑ Step 16: "Unary Operators (`&`, `*`)" <br>
☑ Step 17: "Introduce int Keyword" <br>
☑ Step 18: "Introduce Pointer Types" <br>
  - Define pointer types
  - Substitution via dereference

☑ Step 19: "Pointer Arithmetic (Addition/Subtraction)" <br>
☑ Step 20: "`sizeof` Operator" <br>
☑ Step 21: "Implement Arrays"
  - Array declarations
  - Implicit cast from array to pointer
  - Element access via dereference

☑ Step 22: "Index Access of Array"

## Additional Features
- Improved error messages (pinpoint the first error in the code).
- Simple type annotations:
  - Detects type mismatches in operations and function calls
  - Detects incorrect number of arguments in function calls
- Stack memory allocation based on type:
  - `int` → 4 bytes
  - `pointer` → 8 bytes
  - `array` → array_size × element_type_size

