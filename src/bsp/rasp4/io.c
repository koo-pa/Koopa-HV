#include "io.h"

enum {
    PERIPHERAL_BASE = 0xFE000000,
    GPFSEL0         = PERIPHERAL_BASE + 0x200000,
    GPSET0          = PERIPHERAL_BASE + 0x20001C,
    GPCLR0          = PERIPHERAL_BASE + 0x200028,
    GPPUPPDN0       = PERIPHERAL_BASE + 0x2000E4
};

enum {
    GPIO_MAX_PIN       = 53,
    GPIO_FUNCTION_OUT  = 1,
    GPIO_FUNCTION_ALT5 = 2,
    GPIO_FUNCTION_ALT3 = 7
};

enum {
    Pull_None = 0,
    Pull_Down = 1, // Are down and up the right way around?
    Pull_Up = 2
};

uint32_t gpio_call(uint32_t pin_number, uint32_t value, uint32_t base, uint32_t field_size, uint32_t field_max) {
    uint32_t field_mask = (1 << field_size) - 1;

    if (pin_number > field_max) return 0;
    if (value > field_mask) return 0;

    uint32_t num_fields = 32 / field_size;
    uint32_t reg = base + ((pin_number / num_fields) * 4);
    uint32_t shift = (pin_number % num_fields) * field_size;

    uint32_t curval = mmio_read32(reg);
    curval &= ~(field_mask << shift);
    curval |= value << shift;
    mmio_write32(reg, curval);

    return 1;
}

uint32_t gpio_set     (uint32_t pin_number, uint32_t value) { return gpio_call(pin_number, value, GPSET0, 1, GPIO_MAX_PIN); }
uint32_t gpio_clear   (uint32_t pin_number, uint32_t value) { return gpio_call(pin_number, value, GPCLR0, 1, GPIO_MAX_PIN); }
uint32_t gpio_pull    (uint32_t pin_number, uint32_t value) { return gpio_call(pin_number, value, GPPUPPDN0, 2, GPIO_MAX_PIN); }
uint32_t gpio_function(uint32_t pin_number, uint32_t value) { return gpio_call(pin_number, value, GPFSEL0, 3, GPIO_MAX_PIN); }

void gpio_useAsAlt3(uint32_t pin_number) {
    gpio_pull(pin_number, Pull_None);
    gpio_function(pin_number, GPIO_FUNCTION_ALT3);
}

void gpio_useAsAlt5(uint32_t pin_number) {
    gpio_pull(pin_number, Pull_None);
    gpio_function(pin_number, GPIO_FUNCTION_ALT5);
}

void gpio_initOutputPinWithPullNone(uint32_t pin_number) {
    gpio_pull(pin_number, Pull_None);
    gpio_function(pin_number, GPIO_FUNCTION_OUT);
}

void gpio_setPinOutputBool(uint32_t pin_number, uint32_t onOrOff) {
    if (onOrOff) {
        gpio_set(pin_number, 1);
    } else {
        gpio_clear(pin_number, 1);
    }
}

// UART

enum {
    AUX_BASE        = PERIPHERAL_BASE + 0x215000,
    AUX_IRQ         = AUX_BASE,
    AUX_ENABLES     = AUX_BASE + 4,
    AUX_MU_IO_REG   = AUX_BASE + 64,
    AUX_MU_IER_REG  = AUX_BASE + 68,
    AUX_MU_IIR_REG  = AUX_BASE + 72,
    AUX_MU_LCR_REG  = AUX_BASE + 76,
    AUX_MU_MCR_REG  = AUX_BASE + 80,
    AUX_MU_LSR_REG  = AUX_BASE + 84,
    AUX_MU_MSR_REG  = AUX_BASE + 88,
    AUX_MU_SCRATCH  = AUX_BASE + 92,
    AUX_MU_CNTL_REG = AUX_BASE + 96,
    AUX_MU_STAT_REG = AUX_BASE + 100,
    AUX_MU_BAUD_REG = AUX_BASE + 104,
    AUX_UART_CLOCK  = 500000000,
    UART_MAX_QUEUE  = 16 * 1024
};

#define AUX_MU_BAUD(baud) ((AUX_UART_CLOCK/(baud*8))-1)

uint8_t uart_output_queue[UART_MAX_QUEUE];
uint32_t uart_output_queue_write = 0;
uint32_t uart_output_queue_read = 0;

void uart_init() {
    mmio_write32(AUX_ENABLES, 1); //enable UART1
    mmio_write32(AUX_MU_IER_REG, 0);
    mmio_write32(AUX_MU_CNTL_REG, 0);
    mmio_write32(AUX_MU_LCR_REG, 3); //8 bits
    mmio_write32(AUX_MU_MCR_REG, 0);
    mmio_write32(AUX_MU_IER_REG, 0);
    mmio_write32(AUX_MU_IIR_REG, 0xC6); //disable interrupts
    mmio_write32(AUX_MU_BAUD_REG, AUX_MU_BAUD(115200));
    gpio_useAsAlt5(14);
    gpio_useAsAlt5(15);
    mmio_write32(AUX_MU_CNTL_REG, 3); //enable RX/TX
}

uint32_t uart_isOutputQueueEmpty() {
    return uart_output_queue_read == uart_output_queue_write;
}

uint32_t uart_isReadByteReady()  { return mmio_read32(AUX_MU_LSR_REG) & 0x01; }
uint32_t uart_isWriteByteReady() { return mmio_read32(AUX_MU_LSR_REG) & 0x20; }

uint8_t uart_readByte() {
    while (!uart_isReadByteReady());
    return (uint8_t)mmio_read32(AUX_MU_IO_REG);
}

void uart_writeByteBlockingActual(uint8_t ch) {
    while (!uart_isWriteByteReady());
    mmio_write32(AUX_MU_IO_REG, (uint32_t)ch);
}

void uart_loadOutputFifo() {

    while (!uart_isOutputQueueEmpty() && uart_isWriteByteReady()) {
        uart_writeByteBlockingActual(uart_output_queue[uart_output_queue_read]);
        uart_output_queue_read = (uart_output_queue_read + 1) & (UART_MAX_QUEUE - 1); // Don't overrun
    }
}

void uart_writeByteBlocking(uint8_t ch) {
    uint32_t next = (uart_output_queue_write + 1) & (UART_MAX_QUEUE - 1); // Don't overrun

    while (next == uart_output_queue_read) uart_loadOutputFifo();

    uart_output_queue[uart_output_queue_write] = ch;
    uart_output_queue_write = next;
}

void uart_writeText(char *buffer) {
    while (*buffer) {
        if (*buffer == '\n') uart_writeByteBlocking('\r');
        uart_writeByteBlocking(*buffer++);
    }
}

void uart_drainOutputQueue() {
    while (!uart_isOutputQueueEmpty()) uart_loadOutputFifo();
}

void uart_update() {
    uart_loadOutputFifo();

    if (uart_isReadByteReady()) {
        uint8_t ch = uart_readByte();
        if (ch == '\r') uart_writeText("\n"); else uart_writeByteBlocking(ch);
    }
}