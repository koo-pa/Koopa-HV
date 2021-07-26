.globl mmio_write32
mmio_write32:
	str w1,[x0]
	ret

.globl mmio_read32
mmio_read32:
	ldr w0,[x0]
	ret

.globl delay
delay:
	subs x0, x0, #1
	bne delay
	ret