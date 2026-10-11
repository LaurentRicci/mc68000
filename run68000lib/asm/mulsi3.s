* libgcc1 routines for 68000 w/o floating-point hardware.
*   Copyright (C) 1994, 1996, 1997, 1998 Free Software Foundation, Inc.
* from https://github.com/torvalds/linux/blob/master/arch/m68k/lib/mulsi3.S

__mulsi3:
        move.w   4(sp),d0       ; x0 -> d0 
        mulu.w   10(sp),d0      ; x0*y1 

        move.w   6(sp),d1       ; x1 -> d1 
        mulu.w   8(sp),d1       ; x1*y0 

        add.w    d1,d0
        swap    d0
        clr.w    d0

        move.w   6(sp),d1       ; x1 -> d1 
        mulu.w   10(sp),d1      ; x1*y1 

        add.l    d1,d0
        rts
