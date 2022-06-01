#include "api_uart.h"
#define MAX_MSG_LEN                 100
#define UART_TIMEOUT                2000 //msec
#define UART_BUFFER                 2000

static UART_HandleTypeDef uart;

hub_retcode_t api_uart_init(uint32_t baudrate)
{
     if(!(9600 == baudrate || 115200 == baudrate))
        return ARGUMENT_ERROR;

    /* Configure Pins for API UART */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = API_UART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(API_UART_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = API_UART_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(API_UART_PORT, &GPIO_InitStruct);

    /* Initialize UART */
    uart.Instance = USART1;
    uart.Init.BaudRate = baudrate;
    uart.Init.WordLength = UART_WORDLENGTH_8B;
    uart.Init.Parity = UART_PARITY_NONE;
    uart.Init.StopBits = UART_STOPBITS_1;
    uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart.Init.OverSampling = UART_OVERSAMPLING_16;
    uart.Init.Mode = UART_MODE_TX_RX;

    HAL_UART_Init(&uart);
    return OK;
}

hub_retcode_t api_uart_tx(uint8_t *data, uint16_t len)
{
    if (0 == len)
        return ARGUMENT_ERROR;
    
    return (hub_retcode_t)HAL_UART_Transmit(&uart, data, len, UART_TIMEOUT);
}

hub_retcode_t api_uart_rx(uint8_t *data, uint16_t len)
{
    if (0 == len)
        return ARGUMENT_ERROR;

    return (hub_retcode_t)HAL_UART_Receive(&uart, data, len, UART_TIMEOUT);
}

