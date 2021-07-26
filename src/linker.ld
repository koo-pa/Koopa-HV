SECTIONS
{
    . = 0x80000;     /* Kernel load address for AArch64 */
	.text.boot : { *(.text.boot) }
	.text : { *(.text) }
	. = ALIGN(0x8);
	.data : { *(.data) }
	. = ALIGN(0x8);
	bss_begin = .;
	.bss : { *(.bss*) }
	bss_end = .;
}
