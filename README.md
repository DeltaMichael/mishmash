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

```
./run.sh my_program.msh
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
There is no main function, the program is contained in a DECLR-BEGIN-END block.
Variables are declared in the DECLR block and are initialized to 0.
The DECLR part is optional, i.e. you can have a BEGIN-END block that doesn't operate on variables.
Assignment is done with the walrus (`:=`) operator. `=` does an equality check like `==` in other languages.
`print` is a function in mashlib, which I sneakily implemented in C.

**Arithmetic operations implemented so far:**
- addition
- multiplication
- negation
- = and < comparison

**Arithmetic operations to be implemented:**
- subtraction
- division
- `>=`, `<=`, `>` comparison

**Logical operations implemented so far:**

**Logical operations to be implemented:**
- AND
- OR
- NOT
- XOR

**Logical blocks implemented so far:**
- main block

**Logical blocks to be implemented:**
- conditional
- while and for loops
- functions

**Types implemented:**
- int - 64-bit stack-allocated

**Types to be implemented:**
- bool
- float
- string

**Future assembly generation improvements**
- register management
