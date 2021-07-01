#!/bin/sh
BASE=$(basename $1 .c)
riscv64-elf-gcc -march=rv32i -mabi=ilp32 -nostdlib -fno-builtin -O5 $1 -o $BASE.elf
riscv64-elf-objcopy -O binary $BASE.elf $BASE.bin