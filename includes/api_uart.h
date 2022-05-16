#ifndef __API_UART_H__
#define __API_UART_H__

#include "tools.h"
#include "stdint.h"
#define MAX_MSG_LEN                 100

hub_retcode_t api_uart_init(uint32_t baudrate);

hub_retcode_t api_uart_tx(uint8_t *data, uint16_t len);
hub_retcode_t api_uart_rx(uint8_t *data, uint16_t len);

#endif