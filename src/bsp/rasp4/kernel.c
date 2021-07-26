#include "io.h"

void kernel_main(void) {
    uart_init();
    uart_writeText("Hello world!\n");
    while (1) uart_update();
}