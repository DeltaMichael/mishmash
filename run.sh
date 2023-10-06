#!/bin/sh
BARE_PATH="${1%.*}"
MASHLIB_DIR="/lib64"
MASHLIB_PATH=$MASHLIB_DIR"/mashlib.so"
./mishmash "${BARE_PATH}.msh" && as "${BARE_PATH}.s" -o "${BARE_PATH}.o"
ld --dynamic-linker=/lib64/ld-linux-x86-64.so.2 "${BARE_PATH}.o" $MASHLIB_PATH -o $BARE_PATH
# export LD_LIBRARY_PATH=$MASHLIB_PATH
exec $BARE_PATH
