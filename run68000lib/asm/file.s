#NO_APP
	.file	"file.c"
	.text
	.align	2
	.globl	memcmp
	.type	memcmp, @function
memcmp:
	link %fp,#0
	movem.l #15360,-(%sp)
	move.l 8(%fp),%d2
	move.l 12(%fp),%d3
	bra .L2
.L6:
	move.l %d2,%a0
	move.l %a0,%d2
	addq.l #1,%d2
	move.b (%a0),%d1
	move.l %d3,%a0
	move.l %a0,%d3
	addq.l #1,%d3
	move.b (%a0),%d0
	cmp.b %d1,%d0
	beq .L2
	move.l %d2,%a0
	subq.l #1,%a0
	move.b (%a0),%d1
	move.l %d3,%a0
	subq.l #1,%a0
	move.b (%a0),%d0
	cmp.b %d1,%d0
	bls .L3
	moveq #-1,%d0
	bra .L5
.L3:
	moveq #1,%d0
	bra .L5
.L2:
	move.l 16(%fp),%d0
	move.l 20(%fp),%d1
	moveq #-1,%d4
	moveq #-1,%d5
	add.l %d1,%d5
	addx.l %d0,%d4
	move.l %d4,16(%fp)
	move.l %d5,20(%fp)
	move.l %d0,%d5
	or.l %d1,%d5
	bne .L6
	moveq #0,%d0
.L5:
	movem.l (%sp)+,#60
	unlk %fp
	rts
	.size	memcmp, .-memcmp
	.align	2
	.globl	read_bpb
	.type	read_bpb, @function
read_bpb:
	link %fp,#-512
	move.l %fp,%d0
	add.l #-512,%d0
	move.l %d0,-(%sp)
	clr.l -(%sp)
	jsr read_sector
	addq.l #8,%sp
	move.b -501(%fp),%d0
	clr.w %d1
	move.b %d0,%d1
	move.b -500(%fp),%d0
	move.b %d0,%d0
	and.w #255,%d0
	lsl.w #8,%d0
	or.w %d1,%d0
	move.l 8(%fp),%a0
	move.w %d0,(%a0)
	move.b -499(%fp),%d0
	move.l 8(%fp),%a0
	move.b %d0,2(%a0)
	move.b -498(%fp),%d0
	clr.w %d1
	move.b %d0,%d1
	move.b -497(%fp),%d0
	move.b %d0,%d0
	and.w #255,%d0
	lsl.w #8,%d0
	or.w %d1,%d0
	move.l 8(%fp),%a0
	move.w %d0,4(%a0)
	move.b -496(%fp),%d0
	move.l 8(%fp),%a0
	move.b %d0,6(%a0)
	move.b -495(%fp),%d0
	clr.w %d1
	move.b %d0,%d1
	move.b -494(%fp),%d0
	move.b %d0,%d0
	and.w #255,%d0
	lsl.w #8,%d0
	or.w %d1,%d0
	move.l 8(%fp),%a0
	move.w %d0,8(%a0)
	move.b -490(%fp),%d0
	clr.w %d1
	move.b %d0,%d1
	move.b -489(%fp),%d0
	move.b %d0,%d0
	and.w #255,%d0
	lsl.w #8,%d0
	or.w %d1,%d0
	move.l 8(%fp),%a0
	move.w %d0,10(%a0)
	nop
	unlk %fp
	rts
	.size	read_bpb, .-read_bpb
	.align	2
	.globl	format_filename
	.type	format_filename, @function
format_filename:
	link %fp,#-4
	move.l %d2,-(%sp)
	clr.l -4(%fp)
	bra .L9
.L13:
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	cmp.b #96,%d0
	ble .L10
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	cmp.b #122,%d0
	bgt .L10
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	add.b #-32,%d0
	move.b %d0,%d1
	bra .L11
.L10:
	move.l 8(%fp),%a0
	move.b (%a0),%d1
.L11:
	move.l -4(%fp),%d0
	move.l %d0,%d2
	addq.l #1,%d2
	move.l %d2,-4(%fp)
	move.l 12(%fp),%a0
	adda.l %d0,%a0
	move.b %d1,(%a0)
	addq.l #1,8(%fp)
.L9:
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	beq .L14
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	cmp.b #46,%d0
	beq .L14
	moveq #7,%d0
	cmp.l -4(%fp),%d0
	bge .L13
	bra .L14
.L15:
	move.l -4(%fp),%d0
	move.l %d0,%d1
	addq.l #1,%d1
	move.l %d1,-4(%fp)
	move.l 12(%fp),%a0
	adda.l %d0,%a0
	move.b #32,(%a0)
.L14:
	moveq #7,%d2
	cmp.l -4(%fp),%d2
	bge .L15
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	cmp.b #46,%d0
	bne .L17
	addq.l #1,8(%fp)
	bra .L17
.L21:
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	cmp.b #96,%d0
	ble .L18
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	cmp.b #122,%d0
	bgt .L18
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	add.b #-32,%d0
	move.b %d0,%d1
	bra .L19
.L18:
	move.l 8(%fp),%a0
	move.b (%a0),%d1
.L19:
	move.l -4(%fp),%d0
	move.l %d0,%d2
	addq.l #1,%d2
	move.l %d2,-4(%fp)
	move.l 12(%fp),%a0
	adda.l %d0,%a0
	move.b %d1,(%a0)
	addq.l #1,8(%fp)
.L17:
	move.l 8(%fp),%a0
	move.b (%a0),%d0
	beq .L22
	moveq #10,%d0
	cmp.l -4(%fp),%d0
	bge .L21
	bra .L22
.L23:
	move.l -4(%fp),%d0
	move.l %d0,%d1
	addq.l #1,%d1
	move.l %d1,-4(%fp)
	move.l 12(%fp),%a0
	adda.l %d0,%a0
	move.b #32,(%a0)
.L22:
	moveq #10,%d2
	cmp.l -4(%fp),%d2
	bge .L23
	nop
	nop
	move.l (%sp)+,%d2
	unlk %fp
	rts
	.size	format_filename, .-format_filename
	.align	2
	.globl	find_file_in_root
	.type	find_file_in_root, @function
find_file_in_root:
	link %fp,#-560
	move.l %d2,-(%sp)
	lea (-34,%fp),%a0
	move.l %a0,-(%sp)
	jsr read_bpb
	addq.l #4,%sp
	move.w -30(%fp),%d0
	moveq #0,%d2
	move.w %d0,%d2
	move.b -28(%fp),%d0
	move.b %d0,%d0
	and.l #255,%d0
	move.w -24(%fp),%d1
	move.w %d1,%d1
	and.l #65535,%d1
	move.l %d1,-(%sp)
	move.l %d0,-(%sp)
	jsr __mulsi3
	addq.l #8,%sp
	add.l %d2,%d0
	move.l %d0,-12(%fp)
	move.w -26(%fp),%d0
	move.w %d0,%d0
	and.l #65535,%d0
	move.l %d0,%d1
	lsl.l #5,%d1
	move.w -34(%fp),%d0
	move.w %d0,%d0
	and.l #65535,%d0
	subq.l #1,%d0
	add.l %d0,%d1
	move.w -34(%fp),%d0
	move.w %d0,%d0
	and.l #65535,%d0
	move.l %d0,-(%sp)
	move.l %d1,-(%sp)
	jsr __divsi3
	addq.l #8,%sp
	move.l %d0,-16(%fp)
	lea (-45,%fp),%a0
	move.l %a0,-(%sp)
	move.l 8(%fp),-(%sp)
	jsr format_filename
	addq.l #8,%sp
	clr.l -4(%fp)
	bra .L25
.L32:
	move.l -12(%fp),%d0
	add.l -4(%fp),%d0
	move.l %fp,%d1
	add.l #-557,%d1
	move.l %d1,-(%sp)
	move.l %d0,-(%sp)
	jsr read_sector
	addq.l #8,%sp
	clr.l -8(%fp)
	bra .L26
.L31:
	move.l %fp,%d0
	add.l #-557,%d0
	move.l %d0,%d1
	add.l -8(%fp),%d1
	move.l %d1,-20(%fp)
	move.l -20(%fp),%a0
	move.b (%a0),%d0
	bne .L27
	clr.w %d0
	bra .L33
.L27:
	move.l -20(%fp),%a0
	move.b (%a0),%d0
	cmp.b #-27,%d0
	beq .L34
	pea 11.w
	clr.l -(%sp)
	lea (-45,%fp),%a0
	move.l %a0,-(%sp)
	move.l -20(%fp),-(%sp)
	jsr memcmp
	lea (16,%sp),%sp
	tst.l %d0
	bne .L30
	move.w #26,%a0
	adda.l -20(%fp),%a0
	move.b (%a0),%d0
	clr.w %d1
	move.b %d0,%d1
	move.w #27,%a0
	adda.l -20(%fp),%a0
	move.b (%a0),%d0
	move.b %d0,%d0
	and.w #255,%d0
	lsl.w #8,%d0
	or.w %d1,%d0
	move.w %d0,-22(%fp)
	move.w -22(%fp),%d0
	bra .L33
.L34:
	nop
.L30:
	moveq #32,%d0
	add.l %d0,-8(%fp)
.L26:
	cmpi.l #511,-8(%fp)
	ble .L31
	addq.l #1,-4(%fp)
.L25:
	move.l -4(%fp),%d1
	cmp.l -16(%fp),%d1
	bcs .L32
	clr.w %d0
.L33:
	move.l -564(%fp),%d2
	unlk %fp
	rts
	.size	find_file_in_root, .-find_file_in_root
	.globl	__divsi3
	.globl	__mulsi3
	.ident	"GCC: (GNU) 16.1.0"
