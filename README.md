# Mish-Mash

![Green on Blue Logo-2](https://github.com/DeltaMichael/mishmash/assets/91800778/22099f9e-6bd4-4a88-98a2-7c64c2ef69bf)


Mish-mash is a programming languague that shows you all the internals. It started as me trying to learn the basics of interpreter/compiler design but I hope anyone can find it useful in their learning process.

## Behind the name

My first CS teacher didn't speak much English and pronounced the "Type Mismatch" error from Turbo Pascal as "Type Mish-Mash". 13-year-old me found this extremely funny. Current me still chuckles a bit. The syntax is based on most of what I remember from Pascal, which is very little. I haven't looked at Pascal since then and I don't plan to start any time soon.

# HOW-TO

## Prerequisites
- an x86_64 CPU
- build-essential, base-devel or equivalent toolchain to run gcc, make, etc.
- NASM

## Compile the compiler

```
make
```

## Compile and run a Mish-Mash program


### Compile to x86_64 NASM

```
./mishmash my_program.msh
```
### Compile to x86_64 NASM with debug output

The debug output shows all tokens, AST in reverse-Polish notation, three-address code and x86_64 assembly

```
./mishmash -d my_program.msh
```

```
----------------
---- TOKENS ----
----------------
line: 0 lexeme: DECLR type: DECLR
line: 1 lexeme: i type: IDENTIFIER
line: 1 lexeme: : type: COLON
line: 1 lexeme: int type: STATIC_TYPE
line: 1 lexeme: ; type: LINE_TERM
line: 2 lexeme: first type: IDENTIFIER
line: 2 lexeme: : type: COLON
line: 2 lexeme: int type: STATIC_TYPE
line: 2 lexeme: ; type: LINE_TERM
line: 3 lexeme: second type: IDENTIFIER
line: 3 lexeme: : type: COLON
line: 3 lexeme: int type: STATIC_TYPE
line: 3 lexeme: ; type: LINE_TERM
line: 4 lexeme: temp type: IDENTIFIER
line: 4 lexeme: : type: COLON
line: 4 lexeme: int type: STATIC_TYPE
line: 4 lexeme: ; type: LINE_TERM
line: 5 lexeme: BEGIN type: BEGIN
line: 6 lexeme: first type: IDENTIFIER
line: 6 lexeme: := type: ASSIGN
line: 6 lexeme: 0 type: INT_LITERAL
line: 6 lexeme: ; type: LINE_TERM
line: 7 lexeme: second type: IDENTIFIER
line: 7 lexeme: := type: ASSIGN
line: 7 lexeme: 1 type: INT_LITERAL
line: 7 lexeme: ; type: LINE_TERM
line: 8 lexeme: temp type: IDENTIFIER
line: 8 lexeme: := type: ASSIGN
line: 8 lexeme: second type: IDENTIFIER
line: 8 lexeme: ; type: LINE_TERM
line: 9 lexeme: while type: WHILE
line: 9 lexeme: ( type: LEFT_PAREN
line: 9 lexeme: i type: IDENTIFIER
line: 9 lexeme: < type: LESS_THAN
line: 9 lexeme: 10 type: INT_LITERAL
line: 9 lexeme: ) type: RIGHT_PAREN
line: 10 lexeme: BEGIN type: BEGIN
line: 11 lexeme: print type: PRINT_OP
line: 11 lexeme: temp type: IDENTIFIER
line: 11 lexeme: ; type: LINE_TERM
line: 12 lexeme: first type: IDENTIFIER
line: 12 lexeme: := type: ASSIGN
line: 12 lexeme: second type: IDENTIFIER
line: 12 lexeme: ; type: LINE_TERM
line: 13 lexeme: second type: IDENTIFIER
line: 13 lexeme: := type: ASSIGN
line: 13 lexeme: temp type: IDENTIFIER
line: 13 lexeme: ; type: LINE_TERM
line: 14 lexeme: temp type: IDENTIFIER
line: 14 lexeme: := type: ASSIGN
line: 14 lexeme: first type: IDENTIFIER
line: 14 lexeme: + type: PLUS
line: 14 lexeme: second type: IDENTIFIER
line: 14 lexeme: ; type: LINE_TERM
line: 15 lexeme: i type: IDENTIFIER
line: 15 lexeme: := type: ASSIGN
line: 15 lexeme: i type: IDENTIFIER
line: 15 lexeme: + type: PLUS
line: 15 lexeme: 1 type: INT_LITERAL
line: 15 lexeme: ; type: LINE_TERM
line: 16 lexeme: END type: END
line: 17 lexeme: END type: END
-------------
---- AST ----
-------------
BLOCK START 0
declare i of type int
declare first of type int
declare second of type int
declare temp of type int
first := 0
second := 1
temp := second

while (i 10 <)
BLOCK START 1
print temp
first := second
second := temp
temp := (first second +)
i := (i 1 +)
BLOCK END 1

BLOCK END 0
----------------------------
---- THREE-ADDRESS CODE ----
----------------------------
first := 0
second := 1
temp := second
S6:
t100 := i < 10
iffalse t100 goto E26
print temp
first := second
second := temp
t101 := first + second
temp := t101
t102 := i + 1
i := t102
goto S6
E26:
------------------
---- ASSEMBLY ----
------------------
extern print_num
global	_start
section	.text
_start:
	enter 32, 0
	mov qword [rbp - 16], 0
	mov qword [rbp - 24], 1
	mov qword rax, [rbp - 24]
	mov qword [rbp - 32], rax
S6:
	mov rax, 10
	mov qword rbx, [rbp - 8]
	;; --- compare ---
	cmp rbx, rax
	mov rax, 0
	mov rbx, 1
	;; --- extract lt ---
	cmovl rax, rbx
	;; --- iffalse goto ---
	test rax, rax
	jz E26
	mov qword rdi, [rbp - 32]
	call print_num
	mov qword rax, [rbp - 24]
	mov qword [rbp - 16], rax
	mov qword rax, [rbp - 32]
	mov qword [rbp - 24], rax
	;; --- add ---
	mov qword rax, [rbp - 16]
	;; --- add ---
	add rax, [rbp - 24]
	mov qword [rbp - 32], rax
	mov qword rax, [rbp - 8]
	;; --- add ---
	add rax, 1
	mov qword [rbp - 8], rax
	;; --- goto ---
	jmp S6
E26:
	leave
	mov rdi, 0
	mov rax, 60
	syscall
```

### Compile and run a program

There is currently only support for single source files. main.c calls NASM and ld and outputs a binary which is runnable.

```
./mishmash my_program.msh
./my_program
```

### Run a program in interpreter mode

```
./mishmash -i my_program.msh
```

## Compile test suite

```
make testmash
```

## Run test suite

Executes all the tests in the `test` directory.

### Comipler mode

```
./testmash
```

### Interpreter mode

```
./testmash -i
```

## Compile mashlib

```
cd ./mashlib
./build.sh
```

Then either move mashlib.so to `/lib64` or set `LD_LIBRARY_PATH` to where the .so is located

# Syntax

**Example program**

```
DECLR
    x: int;
    y: int;
    a: int;
    b: int;
    c: int;
BEGIN
    a := 2;
    b := 4;
    c := -8;
    x := a * -c + b * -c;
    print x;
    print c;
    y := a < b;
    print y;
    y := a = b;
    print y;
END
```

**Conditional statements**

```
DECLR
	x: int;
	c: int;
BEGIN
	c := -2;
	x := c / -1;
	x := 3;

	if (c = -2)
	BEGIN
		print c;
	END
	else BEGIN
		print x;
	END

	if (c = -3)
	BEGIN
		print c;
	END
	else BEGIN
		print x;
	END
END
```

**Loops**

```
// print the first 10 fibonacci numbers
DECLR
	i: int;
	first: int;
	second: int;
	temp: int;
BEGIN
	first := 0;
	second := 1;
	temp := second;
	while (i < 10)
	BEGIN
		print temp;
		first := second;
		second := temp;
		temp := first + second;
		i := i + 1;
	END
END
```

There is no main function, the program is contained in a DECLR-BEGIN-END block.
Variables are declared in the DECLR block and are initialized to 0.
The DECLR part is optional, i.e. you can have a BEGIN-END block that doesn't operate on variables.
Assignment is done with the walrus (`:=`) operator. `=` does an equality check like `==` in other languages.
`print` is a function in mashlib, which I sneakily implemented in C.

**Arithmetic operations implemented so far:**
- addition
- subtraction
- multiplication
- division
- negation
- comparison

**Arithmetic operations to be implemented:**
- != comparison

**Logical operations implemented so far:**

**Logical operations to be implemented:**
- AND
- OR
- NOT
- XOR

**Logical blocks implemented so far:**
- main block
- conditional
- while loops

**Logical blocks to be implemented:**
- functions
- for loops
- variable scope

**Types implemented:**
- int - 64-bit stack-allocated

**Types to be implemented:**
- bool
- float
- string

**Code generation pipeline stages**
- type checker
- the notorious "Mish-Mash" error

**Future assembly generation improvements**
- register management
- stack reclaiming

**Other tech debt**

Memory leaks when creating labels
Memory leaks in interpreter

~~Undefined behavior when freeing lists~~
