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
#include "tools.h"
#include "bmp280_map.h"

/***** Defines *****/
#define BMP280_SET_BIT(reg, bit)    reg |= bit 

/***** Custom types *****/
typedef int32_t     BMP280_S32_t;
typedef uint32_t    BMP280_U32_t;
typedef int64_t     BMP280_S64_t;
/***** Data types *****/
struct bmp280_calib_digit
{
    /* Temperature calibration */
    uint16_t dig_t1;
    int16_t dig_t2, dig_t3;

    /* Pressure calibration */
    uint16_t dig_p1;
    int16_t dig_p2, dig_p3, dig_p4, dig_p5, dig_p6, dig_p7, dig_p8, dig_p9, dig_p10;
};

struct bmp280_config
{
    uint8_t osrs_t;     // temperature sampling
    uint8_t osrs_p;     // pressure sampling
    uint8_t sb;         // standby
    uint8_t mode;       
};

struct bmp280_raw_data
{
    BMP280_S32_t temperature;
    BMP280_S32_t pressure;
};

struct bmp280_data
{
    BMP280_S32_t temperature;
    BMP280_U32_t pressure;
};


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
ErrorStatus bmp280_get_config(struct bmp280_config *conf);

/****************************
 * @brief Set config
 * 
 * @param conf pointer to config struct
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_set_config(struct bmp280_config *conf);

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
ErrorStatus bmp280_get_raw(struct bmp280_raw_data *raw);

/****************************
 * @brief Get compensated data
 * 
 * @param data pointer to data struct
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_get_data(struct bmp280_data *data);

/****************************
 * @brief Compensate raw data
 * 
 * @param raw pointer to input data struct
 * @param data pointer to output data struct
 * 
 * @return 0 if succes, else error number
 * **************************/
ErrorStatus bmp280_compensate(struct bmp280_raw_data *raw, struct bmp280_data *data);

#endif
