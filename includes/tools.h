#ifndef __TOOLS_H__
#define __TOOLS_H__

/***** Boards *****/
#define STM32F4
//#define STM32F1

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#endif

#define LED_pin GPIO_PIN_14
#define LED_port GPIOG
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
#define SPI_CSB_Pin     GPIO_PIN_5
#define SPI_CSB_GPIO    GPIOC
#define UART_TX_Pin     GPIO_PIN_9
#define UART_TX_GPIO    GPIOA
#define UART_RX_Pin     GPIO_PIN_10
#define UART_RX_GPIO    GPIOA


#endif