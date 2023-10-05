#!/bin/sh

as mashlib.s -o mashlib.o
ld -fPIE -shared --dynamic-linker=/lib64/ld-linux-x86-64.so.2 mashlib.o -o mashlib.so
rm mashlib.o
