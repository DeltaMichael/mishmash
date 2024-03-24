# Mish-Mash

Mish-mash is a programming languague that will probably never make sense to anyone. The point of this repo is to learn C, assemply and compiler design.

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

### Run a program

There is currently only support for single source files. main.c calls NASM and ld and outputs a binary which is runnable.

```
./mishmash -d my_program.msh
./my_program
```

## Compile test suite

```
make testmash
```

## Run test suite

```
./testmash
```

This will execute all the tests in the `test` directory.

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
- while and for loops

**Logical blocks to be implemented:**
- functions

**Types implemented:**
- int - 64-bit stack-allocated

**Types to be implemented:**
- bool
- float
- string

**Future assembly generation improvements**
- register management
- stack reclaiming

**Other tech debt**

Memory leaks when creating labels
Undefined behavior when freeing lists
