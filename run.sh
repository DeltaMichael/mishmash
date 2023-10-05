#!/bin/sh
BARE_PATH="${1%.*}"
./mishmash "${BARE_PATH}.msh" && as "${BARE_PATH}.s" -o "${BARE_PATH}.o" && ld "${BARE_PATH}.o" -o $BARE_PATH
exec $BARE_PATH
# LD_LIBRARY_PATH=./mashlib
