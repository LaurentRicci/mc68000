# --- debug directives---
    .file   "test.c"
    .ident  "GCC: (GNU) 16.1.0"
    .line   1
    .loc    1 1 0

# --- Sections ---
    .text
    .section .tfxt.startup
    .section .rodata
    .section .data
    .section .bss
    .section .note.GNU-stack,"",@progbits

# --- Alignments ---
    .align  2
    .p2align 2
    .even

# --- Symbols ---
    .globl  my_function
    .local  local_symbol
    .type   my_function, @function
    .size   my_function, .-my_function

my_function:
# --- local labels from GCC ---
.Ltmp0:
.Ltmp1:

# --- data ---
    .byte   0x12
    .word   0x1234
    .short  0x5678
    .long   0x12345678
    .ascii  "HELLO"
    .asciz  "WORLD"

# --- Allocation ---
    .comm   global_var,4,2
    .lcomm  local_bss,8

# --- Relocations (rare but possible) ---
    .long   other_symbol
    .long   .Ltmp0

# --- DWARF / CFI (if -g or unwind tables) ---
    .cfi_startproc
    .cfi_endproc

# --- end of fonction ---
    .size   my_function, .-my_function

# --- Other sections rarely used ---
    .section .text.hot
    .section .text.unlikely
    