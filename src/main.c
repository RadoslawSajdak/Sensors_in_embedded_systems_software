#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "tools.h"
#include "bmp280.h"
#include "sts3x_dis.h"
#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#endif

#include "debug_uart.h"

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void init_done(void);
static void I2C_Init(void);

GPIO_InitTypeDef GPIO_InitStruct = {0};
I2C_HandleTypeDef I2C_InitStruct = {0};

bmp280_data_s test_data = {0};
int main(void)
{
    float temporary_temperature = 0.0f;

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    I2C_Init();
    if(0 != sts3x_dis_init(&I2C_InitStruct, false, false)) while(1);
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
    debug_uart_init(115200);
    init_done();
    while(1)
    {
        HAL_Delay(300);
        bmp280_get_data(&test_data);
        debug_uart_printf("Temp1: %d Pressure1: %d\n", test_data.temperature, test_data.pressure);
        if(OK == sts3x_get_temperature(&temporary_temperature, REPEATABILITY_MEDIUM))
            debug_uart_printf("Temp2: %F\n", temporary_temperature);
    }
}

static void init_done(void)
{
    HAL_GPIO_WritePin(LED_port, LED_pin, GPIO_PIN_SET);
}

static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while(1);
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        while(1);
    }
    
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
