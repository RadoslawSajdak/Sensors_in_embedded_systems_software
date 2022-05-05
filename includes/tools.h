#ifndef __TOOLS_H__
#define __TOOLS_H__

/***** Boards *****/

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#define LED_pin GPIO_PIN_8
#define LED_port GPIOA
#define SPI_CSB_Pin     GPIO_PIN_4
#define SPI_CSB_GPIO    GPIOA
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#define LED_pin GPIO_PIN_14
#define LED_port GPIOG
#define SPI_CSB_Pin     GPIO_PIN_5
#define SPI_CSB_GPIO    GPIOC
#endif


#define TMS_SWDIO_Pin GPIO_PIN_13
#define TMS_SWDIO_GPIO_Port GPIOA
#define TCK_SWCLK_Pin GPIO_PIN_14
#define TCK_SWCLK_GPIO_Port GPIOA


/***** Pinout *****/
#define SPI_SCK_Pin     GPIO_PIN_5
#define SPI_SCK_GPIO    GPIOA
#define SPI_MOSI_Pin    GPIO_PIN_7
#define SPI_MOSI_GPIO   GPIOA
#define SPI_MISO_Pin    GPIO_PIN_6
#define SPI_MISO_GPIO   GPIOA

#define UART_TX_Pin     GPIO_PIN_9
#define UART_TX_GPIO    GPIOA
#define UART_RX_Pin     GPIO_PIN_10
#define UART_RX_GPIO    GPIOA

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
