#include "stdint.h"
#include "stdio.h"
#include "tools.h"
#include "stm32f1xx_hal.h"
#include "sts3x_dis.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void I2C_Init(void);

GPIO_InitTypeDef GPIO_InitStruct = {0};
I2C_HandleTypeDef I2C_InitStruct = {0};

int main(void)
{

    float test_temperature = 0.0f;

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    I2C_Init();
    if(0 != sts3x_dis_init(&I2C_InitStruct, false, false)) while(1);
    for(uint8_t i = 0; i < 6; i ++)
    {
        HAL_Delay(200);
        HAL_GPIO_TogglePin(LED_port, LED_pin);
    }
    HAL_Delay(2000);

    if(0 != sts3x_get_temperature(&test_temperature, REPEATABILITY_LOW)) while(1);
    for(uint8_t i = 0; i < 6; i ++)
    {
        HAL_Delay(200);
        HAL_GPIO_TogglePin(LED_port, LED_pin);
    }
    HAL_Delay(2000);

    while(1)
    {
        HAL_Delay(1000);
        HAL_GPIO_TogglePin(LED_port, LED_pin);
    }
}



void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1){};
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    while(1){};
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
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

static void I2C_Init(void)
{
  __HAL_RCC_I2C1_CLK_ENABLE();
  I2C_InitStruct.Instance             = I2C1;
	I2C_InitStruct.Init.ClockSpeed      = 400000;
	I2C_InitStruct.Init.DutyCycle       = I2C_DUTYCYCLE_2;
	I2C_InitStruct.Init.OwnAddress1     = 0xff;
	I2C_InitStruct.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	I2C_InitStruct.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2C_InitStruct.Init.OwnAddress2     = 0xff;
	I2C_InitStruct.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2C_InitStruct.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

  HAL_I2C_Init(&I2C_InitStruct);
}