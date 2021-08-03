#include "util/printf.h"
#include "bsp/rasp4/io.h"
void kernel_main(void) {
    uart_init();

    printf("Hello world!\n");
    printf("Hello world! %d\n", 100);
    printf("Hello world! 0x%x\n", 16);



    while (1) uart_update();
}