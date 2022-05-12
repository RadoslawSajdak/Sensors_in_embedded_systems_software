#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "stdbool.h"
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
uint8_t a = 0, b = 0;
void test_callback(void)
{
    a++;
    HAL_GPIO_TogglePin(LED_port, LED_pin);
    debug_uart_printf("Hi\n");
}

void test_callback2(void)
{
    HAL_GPIO_TogglePin(LED_port, LED_pin);
    debug_uart_printf("Haj\n");
    b++;
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    debug_uart_init(115200);
    timers_init();
    init_done();
    /////////////
    timers_add_timer(1000,test_callback, true);
    timers_add_timer(3000,test_callback2, true);

    while(1)
    {
        if(a == 15) timers_stop_timer(test_callback);
        if(a == 10){ timers_add_timer(500, test_callback, true); a++;}
        if(b == 3) timers_stop_timer(test_callback2);
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