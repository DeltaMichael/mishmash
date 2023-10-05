#!/bin/sh

as print_num.s -o print_num.o
ld -fPIE -shared --dynamic-linker=/lib64/ld-linux-x86-64.so.2 print_num.o -o print_num.so
