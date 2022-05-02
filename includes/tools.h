#ifndef __TOOLS_H__
#define __TOOLS_H__

#define LED_pin                     GPIO_PIN_8
#define LED_port                    GPIOA
#define TMS_SWDIO_Pin               GPIO_PIN_13
#define TMS_SWDIO_GPIO_Port         GPIOA
#define TCK_SWCLK_Pin               GPIO_PIN_14
#define TCK_SWCLK_GPIO_Port         GPIOA

#define DEBUG_UART_PORT             GPIOA
#define DEBUG_UART_PIN              GPIO_PIN_2


typedef enum {
    OK = 0,
    INIT_ERROR = 1,
    BUSY_ERROR = 2,
    TIMEOUT_ERROR = 3,
    ARGUMENT_ERROR = 4
} hub_retcode_t;


#endif
