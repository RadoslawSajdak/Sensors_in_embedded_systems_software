#include "stdint.h"
#include "stdio.h"
#include "string.h"
#include "tools.h"
#include "bmp280.h"
#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#endif

#include "debug_uart.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

GPIO_InitTypeDef GPIO_InitStruct = {0};
SPI_TypeDef spi1;
GPIO_TypeDef spi_gpio;

bmp280_data_s test_data = {0};
int main(void)
{

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
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
    while(1)
    {
        HAL_Delay(300);
        HAL_GPIO_TogglePin(LED_port, LED_pin);
        bmp280_get_data(&test_data);
        debug_uart_printf("Temp: %d Pressure: %d\n", test_data.temperature, test_data.pressure);
    }
}



void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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



}
