#ifndef __DEBUG_UART_H__
#define __DEBUG_UART_H__

#include "tools.h"
#include "stdint.h"

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#endif

hub_retcode_t debug_uart_init(uint32_t baudrate);
hub_retcode_t debug_uart_tx(uint8_t *data, uint16_t len);
hub_retcode_t debug_uart_printf(const char *msg, ...);

#endif