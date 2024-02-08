#!/bin/sh
BARE_PATH="${1%.*}"
MASHLIB_DIR="/lib64"
MASHLIB_PATH=$MASHLIB_DIR"/mashlib.so"
./mishmash -d "${BARE_PATH}.msh" && nasm -felf64 -g "${BARE_PATH}.asm"
ld --dynamic-linker=/lib64/ld-linux-x86-64.so.2 "${BARE_PATH}.o" $MASHLIB_PATH -o $BARE_PATH
export LD_LIBRARY_PATH=$MASHLIB_PATH
exec $BARE_PATH
