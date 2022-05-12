
/***** Includes *****/
#include "spi.h"
#include "stdint.h"

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#endif

/***** Defines *****/
#define SPI_send(hspi, data)            (ret |= (HAL_SPI_Transmit(hspi, data, 1, 100)))
#define SPI_recv(hspi, data)            (ret |= (HAL_SPI_Receive(hspi, data, 1, 100)))
#define CSB_low(CSB_GPIO, CSB_Pin)      HAL_GPIO_WritePin(CSB_GPIO, CSB_Pin, GPIO_PIN_RESET);
#define CSB_high(CSB_GPIO, CSB_Pin)     HAL_GPIO_WritePin(CSB_GPIO, CSB_Pin, GPIO_PIN_SET);




/***** Global functions definitions *****/

hub_retcode_t spi_init(SPI_HandleTypeDef *hspi1, SPI_TypeDef *SPI_nr)
{
    /* SPI1 parameter configuration*/
    hspi1->Instance = SPI_nr;
    hspi1->Init.Mode = SPI_MODE_MASTER;
    hspi1->Init.Direction = SPI_DIRECTION_2LINES;
    hspi1->Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1->Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1->Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1->Init.NSS = SPI_NSS_SOFT;
    hspi1->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1->Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1->Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1->Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(hspi1) != HAL_OK) return INIT_ERROR;

    return OK;
}

void spi_cs_init(GPIO_TypeDef* CS_GPIO, uint16_t CS_GPIO_Pin)
{
    /* CSB */
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(CS_GPIO, CS_GPIO_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : PC5 */
    GPIO_InitStruct.Pin = CS_GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(CS_GPIO, &GPIO_InitStruct);

}


hub_retcode_t spi_set_register(SPI_HandleTypeDef *hspi1, GPIO_TypeDef* CS_GPIO, uint16_t CS_GPIO_Pin, uint8_t reg, uint8_t *data)
{
    hub_retcode_t ret = OK;
    
    uint8_t buff = SPI_REG_W_M(reg);
    CSB_low(CS_GPIO, CS_GPIO_Pin);
    SPI_send(hspi1, &buff);
    SPI_send(hspi1, data);
    CSB_high(CS_GPIO, CS_GPIO_Pin);

    return ret;
}


hub_retcode_t spi_get_register(SPI_HandleTypeDef *hspi1, GPIO_TypeDef* CS_GPIO, uint16_t CS_GPIO_Pin, uint8_t reg, uint8_t *data, uint8_t bytes_num)
{
    hub_retcode_t ret = OK;

    uint8_t buff = SPI_REG_R_M(reg);
    CSB_low(CS_GPIO, CS_GPIO_Pin);
    SPI_send(hspi1, &buff);
    for(uint8_t i = 0; i < bytes_num && OK == ret; i++) SPI_recv(hspi1, &data[i]); 
    CSB_high(CS_GPIO, CS_GPIO_Pin);

    return ret;
}
