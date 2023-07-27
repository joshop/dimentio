@ === void memset32(void *dst, u32 src, u32 wdn); =====================
    .section .iwram,"ax", %progbits
    .align  2
    .code   32
    .global memset32
    .type   memset32 STT_FUNC
memset32:
    and		r12, r2, #7
	movs	r2, r2, lsr #3
	beq		.Lres_set32
	push	{r4-r9}
	@ set 32byte chunks with 8fold xxmia
	mov		r3, r1
	mov		r4, r1
	mov		r5, r1
	mov		r6, r1
	mov		r7, r1
	mov		r8, r1
	mov		r9, r1
.Lmain_set32:
		stmia	r0!, {r1, r3-r9}
		subs	r2, r2, #1
		bhi		.Lmain_set32
	pop		{r4-r9}
	@ residual 0-7 words
.Lres_set32:
		subs	r12, r12, #1
		stmhsia	r0!, {r1}
		bhi		.Lres_set32
	bx	lr


@ === void memset16(void *dst, u16 src, u32 hwn); =====================
    .section .iwram,"ax", %progbits
    .align  2
    .code   16
    .global memset16
    .thumb_func
    .type   memset16 STT_FUNC
memset16:
    push	{r4, lr}
	@ under 6 hwords -> std set
	cmp		r2, #5
	bls		.Ltail_set16
	@ dst not word aligned: copy 1 hword and align
	lsl		r3, r0, #31
	bcc		.Lmain_set16
		strh	r1, [r0]
		add		r0, #2
		sub		r2, r2, #1
	@ Again, memset32 does the real work
.Lmain_set16:
	lsl		r4, r1, #16
	orr		r1, r4
	lsl		r4, r2, #31
	lsr		r2, r2, #1
	ldr		r3, =memset32
	bl		.Llong_bl
	@ NOTE: r0 is altered by memset32, but in exactly the right 
	@ way, so we can use is as is. r1 is now doubled though.
	lsr		r2, r4, #31
	beq		.Lend_set16
	lsr		r1, #16
.Ltail_set16:
	sub		r2, #1
	bcc		.Lend_set16		@ r2 was 0, bug out
	lsl		r2, r2, #1
.Lres_set16:
		strh	r1, [r0, r2]
		sub		r2, r2, #2
		bcs		.Lres_set16
.Lend_set16:
	pop		{r4}
	pop		{r3}
.Llong_bl:
	bx	r3

@ === void memcpy32(void *dst, const void *src, uint wdcount) IWRAM_CODE; =============
@ r0, r1: dst, src
@ r2: wdcount, then wdcount>>3
@ r3-r10: data buffer
@ r12: wdn&7
    .section .iwram,"ax", %progbits
    .align  2
    .code   32
    .global memcpy32
    .type   memcpy32 STT_FUNC
memcpy32:
    and     r12, r2, #7     @ r12= residual word count
    movs    r2, r2, lsr #3  @ r2=block count
    beq     .Lres_cpy32
    push    {r4-r10}
    @ Copy 32byte chunks with 8fold xxmia
    @ r2 in [1,inf>
.Lmain_cpy32:
        ldmia   r1!, {r3-r10}   
        stmia   r0!, {r3-r10}
        subs    r2, #1
        bne     .Lmain_cpy32
    pop     {r4-r10}
    @ And the residual 0-7 words. r12 in [0,7]
.Lres_cpy32:
        subs    r12, #1
        ldrcs   r3, [r1], #4
        strcs   r3, [r0], #4
        bcs     .Lres_cpy32
    bx  lr

@ === void memcpy16(void *dst, const void *src, uint hwcount); =============
@ Reglist:
@  r0, r1: dst, src
@  r2, r4: hwcount
@  r3: tmp and data buffer
    .text
    .align  2
    .code   16
    .thumb_func
    .global memcpy16
    .type   memcpy16 STT_FUNC
memcpy16:
    push    {r4, lr}
    @ (1) under 5 hwords -> std cpy
    cmp     r2, #5
    bls     .Ltail_cpy16
    @ (2) Unreconcilable alignment -> std cpy
    @ if (dst^src)&2 -> alignment impossible
    mov     r3, r0
    eor     r3, r1
    lsl     r3, #31         @ (dst^src), bit 1 into carry
    bcs     .Ltail_cpy16    @ (dst^src)&2 : must copy by halfword
    @ (3) src and dst have same alignment -> word align
    lsl     r3, r0, #31
    bcc     .Lmain_cpy16    @ ~src&2 : already word aligned
    @ Aligning is necessary: copy 1 hword and align
        ldrh    r3, [r1]
        strh    r3, [r0]
        add     r0, #2
        add     r1, #2
        sub     r2, #1
    @ (4) Right, and for the REAL work, we're gonna use memcpy32
.Lmain_cpy16:
    lsl     r4, r2, #31
    lsr     r2, r2, #1
    ldr     r3,=memcpy32
    bl      _call_via_r3
    @ (5) NOTE: r0,r1 are altered by memcpy32, but in exactly the right 
    @ way, so we can use them as is.
    lsr     r2, r4, #31
    beq     .Lend_cpy16
    @ (6) Copy residuals by halfword
.Ltail_cpy16:
    sub     r2, #1
    bcc     .Lend_cpy16     @ r2 was 0, bug out
    lsl     r2, r2, #1      @ r2 is offset (Yes, we're copying backward)
.Lres_cpy16:
        ldrh    r3, [r1, r2]
        strh    r3, [r0, r2]
        sub     r2, r2, #2
        bcs     .Lres_cpy16
.Lend_cpy16:
    pop     {r4}
    pop     {r3}
    bx  r3


@ void matmul(s32 *matrix, s32 *vectors);
@ matrix points to nine s32s.
@ vectors points to the last vector in the buffer.
@ r0-r8 used to hold the matrix.
@ r9,r10,r11 used for the vector itself.
@ r12,r14(lr) used to accumulate multiply results.
@ r13(sp) used to point to the vector list.
    .section .iwram,"ax", %progbits
    .align  2
    .code   32
    .global matmul
    .type   matmul STT_FUNC
matmul:
    @ backup old registers (all of them!)
    push    {r4-r11}
    push    {r14}
    @ disable interrupts, so the stack can be unsafe
    mov     r2, #0
    ldr     r3,=0x04000208
    str     r2,[r3]
    @ move stack to the vector list
    @ and store old SP past the start
    ldr     r2, =0x02010000
    sub     r2, r2, #12
    str     r13, [r2]
    mov     r13, r1
    @ load in the matrix (r0 last!)
    ldr     r1, [r0, #4]
    ldr     r2, [r0, #8]
    ldr     r3, [r0, #12]
    ldr     r4, [r0, #16]
    ldr     r5, [r0, #20]
    ldr     r6, [r0, #24]
    ldr     r7, [r0, #28]
    ldr     r8, [r0, #32]
    ldr     r0, [r0]
    @ decrement sp, point to first vector
    sub     r13, r13, #12
.Lmatmul_loop:
    @ load in the vector of the day
    ldr     r9, [r13]
    ldr     r10, [r13, #4]
    ldr     r11, [r13, #8]

    @ compute first entry of result
    @ r12 = lo, r14 = hi
    smull   r12, r14, r9, r0
    smlal   r12, r14, r10, r1
    smlal   r12, r14, r11, r2
    @ fixed point, so >> 16
    @ impl'd by >> lo 16 and << hi 16, or
    mov     r12, r12, lsr #16
    orr     r12, r12, r14, lsl #16
    @ store in first entry
    str     r12, [r13]

    @repeat for other two entries

    smull   r12, r14, r9, r3
    smlal   r12, r14, r10, r4
    smlal   r12, r14, r11, r5
    mov     r12, r12, lsr #16
    orr     r12, r12, r14, lsl #16
    str     r12, [r13, #4]

    smull   r12, r14, r9, r6
    smlal   r12, r14, r10, r7
    smlal   r12, r14, r11, r8
    mov     r12, r12, lsr #16
    orr     r12, r12, r14, lsl #16
    str     r12, [r13, #8]

    @ advance sp
    sub     r13, r13, #12


    @ have we hit the end of the line?
    @ if bit 0x00010000 is clear, we have
    tst     r13, #0x00010000
    bne     .Lmatmul_loop

    ldr     r0, [r13]
    mov     r13, r0
    @ reenable interrupts!
    mov     r2, #1
    ldr     r3,=0x04000208
    str     r2,[r3]
    pop     {r14}
    pop     {r4-r11}
    bx      lr

