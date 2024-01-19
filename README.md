# Mish-Mash

Mish-mash is a programming languague that will probably never make sense to anyone. The point of this repo is to learn C, assemply and compiler design.

## Behind the name

My first CS teacher didn't speak much English and pronounced the "Type Mismatch" error from Turbo Pascal as "Type Mish-Mash". 13-year-old me found this extremely funny. Current me still chuckles a bit. The syntax is based on most of what I remember from Pascal, which is very little. I haven't looked at Pascal since then and I don't plan to start any time soon.

# HOW-TO

## Compile the compiler

```
make
```

## Compile and run a Mish-Mash program

./mishmash my_program.msh
./my_program

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

Then either move mashlib.so to /lib64 after building or set `LD_LIBRARY_PATH` to where the .so is located
