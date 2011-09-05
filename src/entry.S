/*
 * NiuBoot ASM code for the very beginning entry point
 *
 * Orson Zhai <city2008@gmail.com>
 *
 * Copyright 2011 Orson Zhai, Beijing, China.
 * Copyright 2011 CFFHH Open Embedded Org. 
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program  is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */


	.code 32
	.section ".reset", "ax"
	.global _start
_start:
	b init
	b .
	b .
	b .
	b .
	b .
	b .
	b .
init:
	/*stack pointer special for imx233 , shall be changed later to support multiple ARM core SoCs*/
	mov sp, #0x8000	

	bl clear_bss
	/*C entry point*/
    	bl main	 
	b .

	.pool

/*
 * This function clears out the .bss section.
 */
clear_bss:
	ldr	r0, =_bss_start
	ldr	r1, =_bss_end
	mov	r2, #0
1:	str	r2, [r0], #4
	cmp	r0, r1
	blo	1b
	mov	pc, lr

	.pool
/*
 * ARM EABI toolchain needs divide by zero handler to be implemented
 * externally.
 */
	.globl	__div0
__div0:
	mov	pc, lr
	.globl  raise
raise:
	mov	pc, lr
	.globl get_heap_start
get_heap_start:
	ldr	r0, =_heap_start
	mov	pc, lr
	.pool
