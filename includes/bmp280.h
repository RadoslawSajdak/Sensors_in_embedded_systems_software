/****************************************
 *  This library is just an example.    *
 *  Here you should put simplified      *
 *  library description                 *
 *  Author:                             *
 *  Date:                               *
 *  Github profile:                     *
 ***************************************/
#ifndef __BMP280_H__
#define __BMP280_H__

/***** Includes *****/
#include "stdint.h"
#include "tools.h"
#include "bmp280_map.h"

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#endif

/***** Data types *****/
typedef struct 
{
    /* Temperature calibration */
    uint16_t dig_t1;
    int16_t dig_t2, dig_t3;

    /* Pressure calibration */
    uint16_t dig_p1;
    int16_t dig_p2, dig_p3, dig_p4, dig_p5, dig_p6, dig_p7, dig_p8, dig_p9, dig_p10;
} bmp280_calib_digit_s;

typedef struct 
{
    uint8_t osrs_temperature;       // temperature sampling
    uint8_t osrs_pressure;          // pressure sampling
    uint8_t standby;                // standby
    uint8_t mode;       
} bmp280_config_s ;

typedef struct
{
    int32_t temperature;
    int32_t pressure;
} bmp280_raw_data_s;

typedef struct
{
    int32_t temperature;
    uint32_t pressure;
} bmp280_data_s;


/****************************
 * @brief Initializing sensor. 
 * Must be called before anything else!
 * 
 * @param hspi1 pointer to spi device
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_init(SPI_TypeDef *SPI_nr, GPIO_TypeDef* CS_GPIO, uint16_t CS_GPIO_Pin);

/****************************
 * @brief Get calibration data 
 * Must be called once before reading data!
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_get_calib(void);

/****************************
 * @brief Software reset 
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_reset(void);

/****************************
 * @brief Get sensor config. 
 * 
 * @param conf pointer to config struct
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_get_config(bmp280_config_s *conf);

/****************************
 * @brief Set config
 * 
 * @param conf pointer to config struct
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_set_config(bmp280_config_s *conf);

/****************************
 * @brief Wait till sensor will be ready
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_ready(void);

/****************************
 * @brief Get raw sensor value
 * 
 * @param raw pointer to data struct
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_get_raw(bmp280_raw_data_s *raw);

/****************************
 * @brief Get compensated data
 * 
 * @param data pointer to data struct
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_get_data(bmp280_data_s *data);

/****************************
 * @brief Compensate raw data
 * 
 * @param raw pointer to input data struct
 * @param data pointer to output data struct
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_compensate(bmp280_raw_data_s *raw, bmp280_data_s *data);

#endif //__BMP280_H__
