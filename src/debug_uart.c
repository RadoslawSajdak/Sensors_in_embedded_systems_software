#include "stdarg.h"
#include "stdio.h"

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_conf.h"
#include "stm32f1xx_hal_usart.h"
#endif

#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_usart.h"
#endif

#include "debug_uart.h"

#define UART_TIMEOUT                500 //miliseconds
#define PRINTF_BUFFER_SIZE          512 //bytes

static UART_HandleTypeDef uart;

hub_retcode_t debug_uart_init(uint32_t baudrate)
{
    if(!(9600 == baudrate || 115200 == baudrate))
        return ARGUMENT_ERROR;

    /* Configure Pins for Debug UART */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DEBUG_UART_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(DEBUG_UART_PORT, &GPIO_InitStruct);

    /* Initialize UART */
    uart.Instance = USART2;
    uart.Init.BaudRate = baudrate;
    uart.Init.WordLength = UART_WORDLENGTH_8B;
    uart.Init.Parity = UART_PARITY_NONE;
    uart.Init.StopBits = UART_STOPBITS_1;
    uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart.Init.OverSampling = UART_OVERSAMPLING_16;
    uart.Init.Mode = UART_MODE_TX;

    HAL_UART_Init(&uart);
    return OK;
}

hub_retcode_t debug_uart_tx(uint8_t *data, uint16_t len)
{
    if (0 == len)
        return ARGUMENT_ERROR;
    
    return (hub_retcode_t)HAL_UART_Transmit(&uart, data, len, UART_TIMEOUT);
}

hub_retcode_t debug_uart_printf(const char *msg, ...)
{
    char buffer[PRINTF_BUFFER_SIZE] = {0};
    int wb = 0;
    hub_retcode_t error_code = OK;

    va_list args;
    va_start(args, msg);
    if( 0 >= (wb = vsnprintf(buffer, sizeof(buffer), msg, args))) error_code = ARGUMENT_ERROR;
    va_end(args);
    if(OK != error_code) return error_code;

    return debug_uart_tx((uint8_t *)buffer, wb);
}