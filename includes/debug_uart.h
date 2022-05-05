#ifndef __DEBUG_UART_H__
#define __DEBUG_UART_H__

#include "tools.h"
#include "stdint.h"


hub_retcode_t debug_uart_init(uint32_t baudrate);
hub_retcode_t debug_uart_tx(uint8_t *data, uint16_t len);
hub_retcode_t debug_uart_printf(const char *msg, ...);

#endif