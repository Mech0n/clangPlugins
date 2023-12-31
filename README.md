# clang Plugins

🔨 Some tools for implement customization during compilation using clang.

### Build

```bash
mkdir build
cd build
CC=clang CXX=clang++ cmake cmake -DLLVM_DIR=/path/to/llvm/cmake  -DLT_LLVM_INSTALL_DIR=/path/to/llvm/ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make -j
```

### Run

Eg. Using `libViewIf.so`

```bash
# Only clang frontend:
clang -cc1 -load libViewIf.so -plugin view-if-ast  tests/iftest2.c
# or
clang -Xclang -add-plugin -fplugin=libViewIf.so -Xclang view-if-ast  ../tests/iftest2.c -o iftest2
# Using in repo
CC="clang -Xclang -add-plugin -fplugin=/path/to/libViewIf.so -Xclang view-if-ast"
CXX="clang++ -Xclang -add-plugin -fplugin=/path/to/libViewIf.so -Xclang view-if-ast"
```

Output :
```
➜  build git:(master) ✗ clang -cc1 -load libViewIf.so -plugin view-if-ast  ../tests/iftest2.c
../tests/iftest2.c:7:9: warning: implicitly declaring library function 'printf' with type 'int (const char *, ...)'
        printf("Number is greater than 5\n");
        ^
../tests/iftest2.c:7:9: note: include the header <stdio.h> or explicitly provide a declaration for 'printf'
Found an if statement:
Condition: 0x55555a3d8d68
Source Code:
if (num > 5) {
        printf("Number is greater than 5\n");
	return 1;
    } else {
        printf("Number is less than or equal to 5\n");


Found an if statement:
Condition: 0x55555a3d9658
Source Code:
if (num == 10) {
        printf("Number is equal to 10\n");


1 warning generated.
```

### TODO

- [x] `ViewIf.cpp` : View `if` block when transforming AST to IR. (test case)
- [x] `FindIfBoundaryAction.cpp` : find `if` block boundary when transforming AST to IR.
        - [x] Find the Boundary.
        - [x] Save into a `.ifi` file. (Finding a way not to use file but memory. 🚧)
 
### Q&A

1. ~~`fatal error: 'stdio.h' file not found`~~

    [Answer](https://clang.llvm.org/docs/FAQ.html#i-run-clang-cc1-and-get-weird-errors-about-missing-headers)




