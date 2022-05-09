#ifndef __SPI_H__
#define __SPI_H__

/***** Includes *****/
#include "tools.h"


/***** Defines *****/
#define SPI_REG_R_M(reg)  ((reg) | 0x80)
#define SPI_REG_W_M(reg)  ((reg) & 0x7F)

/****************************
 * @brief Initializing spi. 
 * Must be called before anything else!
 * 
 * @param hspi1 pointer to spi device
 * @param SPI_nr SPI number
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus spi_init(SPI_HandleTypeDef *hspi1, SPI_TypeDef *SPI_nr);

/****************************
 * @brief Initializing spi cs pin. 
 * Must be called before anything else!
 * 
 * @param CS_GPIO pointer to GPIO
 * @param CS_GPIO_Pin pin number
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus spi_cs_init(GPIO_TypeDef* CS_GPIO, uint16_t CS_GPIO_Pin);

/****************************
 * @brief Write single byte to register
 * 
 * @param hspi1 pointer to spi device
 * @param CS_GPIO pointer to CS pin GPIO
 * @param CS_GPIO_Pin CS pin number
 * @param reg register address
 * @param data pointer to data buffer
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus spi_set_register(SPI_HandleTypeDef *hspi1, GPIO_TypeDef* CS_GPIO, uint16_t CS_GPIO_Pin, uint8_t reg, uint8_t *data);

/****************************
 * @brief Read multiple bytes from device
 * 
 * @param hspi1 pointer to spi device
 * @param CS_GPIO pointer to CS pin GPIO
 * @param CS_GPIO_Pin CS pin number
 * @param reg register address
 * @param data pointer to data buffer
 * @param len amount of bytes
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus spi_get_register(SPI_HandleTypeDef *hspi1, GPIO_TypeDef* CS_GPIO, uint16_t CS_GPIO_Pin, uint8_t reg, uint8_t *data, uint8_t bytes_num);


#endif //__SPI_H__
