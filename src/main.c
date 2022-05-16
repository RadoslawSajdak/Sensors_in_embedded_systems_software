#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "tools.h"
#include "bmp280.h"
#include "boards.h"
#include "debug_uart.h"
#include "timers.h"

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#endif


static void MX_GPIO_Init(void);
static void init_done(void);

GPIO_InitTypeDef GPIO_InitStruct = {0};

bmp280_data_s test_data = {0};
int main(void)
{
        
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    debug_uart_init(115200);
    timers_init();

    if( 0 != bmp280_init(SPI1, SPI_CSB_GPIO, SPI_CSB_Pin)) while(1);
    for(uint8_t i = 0; i < 6; i ++)
    {
        HAL_Delay(200);
        HAL_GPIO_TogglePin(LED_port, LED_pin);
    }
    HAL_Delay(2000);

    if( 0 != bmp280_get_data(&test_data)) while(1);
    for(uint8_t i = 0; i < 6; i ++)
    {
        HAL_Delay(200);
        HAL_GPIO_TogglePin(LED_port, LED_pin);
    }
    HAL_Delay(2000);
    if(test_data.temperature == 0 || test_data.pressure == 0) while(1);
    for(uint8_t i = 0; i < 6; i ++)
    {
        HAL_Delay(200);
        HAL_GPIO_TogglePin(LED_port, LED_pin);
    }
    HAL_Delay(2000);
    init_done();
    while(1)
    {
        HAL_Delay(300);
        bmp280_get_data(&test_data);
        //debug_uart_printf("Temp: %d Pressure: %d\n", test_data.temperature, test_data.pressure);
    }
}

static void init_done(void)
{
    HAL_GPIO_WritePin(LED_port, LED_pin, GPIO_PIN_SET);
}

static void MX_GPIO_Init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LED_port, LED_pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : LED_pin(PA8) */
    GPIO_InitStruct.Pin = LED_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_port, &GPIO_InitStruct);



}