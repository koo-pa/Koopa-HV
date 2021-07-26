#include "mm.h"

.section ".text.boot"

.globl _start
_start:
    mrs	x0, mpidr_el1
    and	x0, x0, #0xFF		// Check processor id
    cbz	x0, _start_main		// Hang for all non-primary CPU
    b	proc_hang

proc_hang:
	b 	proc_hang

_start_main:
	adr	x0, bss_begin
	adr	x1, bss_end
	sub	x1, x1, x0
	bl 	memzero

    msr DAIFSet, #0xF

	mov	sp, #LOW_MEMORY
	bl	kernel_main
	b 	proc_hang		// should never come here

