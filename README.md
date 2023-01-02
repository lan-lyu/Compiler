# LL_Complier
This is a SysY to RISC-V complier project for the course Compiler Principles of Peking University. 

An example SysY code[^1] is like this:
```
int fib(int n) {
  if (n <= 2) {
    return 1;
  } else {
    return fib(n - 1) + fib(n - 2);
  }
}

int main() {
  int input = getint();
  putint(fib(input));
  putch(10);
  return 0;
}
```
The corresponding RISC-V code is like this:
```
  .text
  .align  2

  .globl fib
fib:
  sw    ra, -4(sp)
  addi  sp, sp, -16
  li    t1, 2
  bgt   a0, t1, .l0
  li    a0, 1
  addi  sp, sp, 16
  lw    ra, -4(sp)
  ret
.l0:
  addi  s4, a0, -1
  sw    a0, 0(sp)
  mv    a0, s4
  call  fib
  mv    a3, a0
  lw    a0, 0(sp)
  addi  s4, a0, -2
  sw    a3, 0(sp)
  mv    a0, s4
  call  fib
  mv    s4, a0
  lw    a3, 0(sp)
  add   s4, a3, s4
  mv    a0, s4
  addi  sp, sp, 16
  lw    ra, -4(sp)
  ret

  .globl main
main:
  sw    ra, -4(sp)
  addi  sp, sp, -16
  call  getint
  call  fib
  call  putint
  li    a0, 10
  call  putch
  li    a0, 0
  addi  sp, sp, 16
  lw    ra, -4(sp)
  ret
```


[^1]: https://pku-minic.github.io/online-doc/#/


## Code Structure
The compiling process are splited into three stages:
-  stage 1: sysY --> eeyore
-  stage 2: eeyore --> tigger
-  stage 3: tigger --> RISCV

The file structure are shown as below, in each stage the structures are similar. Take stage 1 as an example, the folder is composed of 5 files, `source.lex` for lexical analysis, `source.yacc` for syntactic analysis, while the process of constructing and scanning the syntax tree through the data structures is defined in `parse.c` and `parse.h`.

```c++
.
├── Stage1 	     // sysY -> eeyore
│   ├── parse.c     	// Parse the generated syntax tree
│   ├── parse.h				// Definition of global data structure, key functions and symbol tables
│   ├── run.sh				// shell script for running
│   ├── source.l    	// for lexical analysis
│   └── source.y			// for syntactic analysis
├── Stage2			 // eeyore -> tigger
└── Stage3			 // tigger -> RISCV
		
3 directories, 15 files
```
