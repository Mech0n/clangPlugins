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
clang -cc1 -load libViewIf.so -plugin view-if-ast  tests/iftest2.c
```

Output :
```
➜  build git:(master) ✗ clang -cc1 -load libViewIf.so -plugin view-if-ast  ../tests/iftest2.c
../tests/iftest2.c:1:10: fatal error: 'stdio.h' file not found
#include <stdio.h>
         ^~~~~~~~~
Found an if statement:
Condition: 0x55555a3d8d68
Source Code:
if (num > 5) {
        printf("Number is greater than 5\n");
	return 1;
    } else {
        printf("Number is less than or equal to 5\n");


Found an if statement:
Condition: 0x55555a3d9298
Source Code:
if (num == 10) {
        printf("Number is equal to 10\n");


1 error generated.
```

### TODO

- [x] `ViewIf.cpp` : View `if` block when transforming AST to IR. (test case)
- [ ] `FindIfBoundaryAction.cpp` : find `if` block boundary when transforming AST to IR. (🚧)

### Q&A

1. `fatal error: 'stdio.h' file not found`

    [Answer](https://stackoverflow.com/questions/48369566/clang-stdio-h-file-not-found)




