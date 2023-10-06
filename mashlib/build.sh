#!/bin/sh
gcc -shared -o mashlib.so -fPIC mashlib.c
# move .so to /lib64 after building or set LD_LIBRARY_PATH to where the .so is located
