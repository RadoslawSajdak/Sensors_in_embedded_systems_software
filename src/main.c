#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "tools.h"
#include "bmp280.h"
#include "sts3x_dis.h"
#include "boards.h"
#include "debug_uart.h"
#include "timers.h"
#include "api.h"
#include "mq2.h"

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#endif


static void MX_GPIO_Init(void);
static void init_done(void);
static void alarm();

GPIO_InitTypeDef GPIO_InitStruct = {0};

int main(void)
{

    SystemClock_Config();
    timers_init();
    HAL_Init();
    MX_GPIO_Init();
    if(OK != mq2_init(MQ2_INSTANCE, MQ2_CHANNEL)) while(1);
    mq2_track_data(200, LPG_MQ2, 2000, alarm);
    uint16_t adc_data;
    while(1)
    {
        adc_data = mq2_get_ppm(LPG_MQ2);
        printf("%d", adc_data);
        HAL_Delay(2000);
    }
    init_done();
    /////////////

    run_api();
    while(1)
    {
        ;
    }
}

static void alarm(){
  printf("pali sie pali");
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

    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;		// SCL, SDA
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}
