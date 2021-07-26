#ifndef CHOCOHV_IO_H
#define CHOCOHV_IO_H

#include "util/utils.h"
#include "util/type.h"

void uart_init();
void uart_writeText(char *buffer);
void uart_loadOutputFifo();
unsigned char uart_readByte();
unsigned int uart_isReadByteReady();
void uart_writeByteBlocking(uint8_t ch);
void uart_update();

#endif //CHOCOHV_IO_H
