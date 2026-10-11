#!/bin/bash
/opt/m68k/bin/m68k-elf-gcc -S -m68000 file.c -o file.s1
sed -f jxx2bxx.sed file.s1 > file.s
../../bin/asm68000.exe file.s
