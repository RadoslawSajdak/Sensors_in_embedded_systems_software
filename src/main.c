#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "tools.h"
#include "bmp280.h"
#include "sts3x_dis.h"
#include "mq2.h"
#include "boards.h"
#include "debug_uart.h"
#include "timers.h"
#include "api.h"

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#endif


static void MX_GPIO_Init(void);
static void init_done(void);
static void I2C_Init(void);

GPIO_InitTypeDef GPIO_InitStruct = {0};
I2C_HandleTypeDef I2C_InitStruct = {0};

int main(void)
{
    
    SystemClock_Config();
    timers_init();
    HAL_Init();
    MX_GPIO_Init();
    I2C_Init();
    if(OK != sts3x_dis_init(&I2C_InitStruct, false, false)) while(1);
    if(OK != bmp280_init(SPI1, SPI_CSB_GPIO, SPI_CSB_Pin)) while(1);
    HAL_Delay(2000);
    if(OK != mq2_init(MQ2_INSTANCE, MQ2_CHANNEL)) while(1);
    debug_uart_init(115200);
    init_done();
    /////////////

    run_api();
    while(1)
    {
        ;
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

    /*Configure GPIO pins : INT_pin(PB12) */
    GPIO_InitStruct.Pin = IRQ_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(IRQ_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;		// SCL, SDA
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

static void I2C_Init(void)
{
    __HAL_RCC_I2C1_CLK_ENABLE();
    I2C_InitStruct.Instance             = I2C1;
	I2C_InitStruct.Init.ClockSpeed      = 100000;
	I2C_InitStruct.Init.DutyCycle       = I2C_DUTYCYCLE_2;
	I2C_InitStruct.Init.OwnAddress1     = 0xff;
	I2C_InitStruct.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	I2C_InitStruct.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2C_InitStruct.Init.OwnAddress2     = 0xff;
	I2C_InitStruct.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2C_InitStruct.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

    HAL_I2C_Init(&I2C_InitStruct);
}