* libgcc1 routines for 68000 w/o floating-point hardware.
*   Copyright (C) 1994, 1996, 1997, 1998 Free Software Foundation
* from https://elixir.bootlin.com/linux/v7.0.12/source/arch/m68k/lib/divsi3.S
        .text
        .globl __divsi3
__divsi3:
        move.l   4(sp),d0        ; dividend 
        move.l   8(sp),d1        ; divisor 

        move.l   #0,d2            ; sign accumulator 

        tst.l    d0
        bpl      L1
        neg.l    d0
        addq.l   #1,d2
L1:
        tst.l    d1
        bpl      L2
        neg.l    d1
        addq.l   #1,d2
L2:
        move.l   #0,d3            ; quotient 
        move.l   #31,d4           ; bit counter 

L3:
        lsl.l    #1,d0
        roxl     #1,d3
        cmp.l    d1,d3
        bcs      L4
        sub.l    d1,d3
        addq.l   #1,d0
L4:
        dbra    d4,L3

        tst.l    d2
        beq      L5
        neg.l    d0
L5:
        move.l   d0,4(sp)
        rts
