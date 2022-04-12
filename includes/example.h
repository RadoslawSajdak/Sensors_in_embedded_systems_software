/****************************************
 *  This library is just an example.    *
 *  Here you should put simplified      *
 *  library description                 *
 *  Author:                             *
 *  Date:                               *
 *  Github profile:                     *
 ***************************************/
#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__

#include "tools.h"

/****************************
 * @brief Initializing sensor and I2C. 
 * Must be called before anything else!
 * 
 * @param hi2c1 pointer to i2c device
 * 
 * @return 0 if succes, else error number
 * **************************/
retcode_t sensor_init(I2C_HandleTypeDef *hi2c1);

/****************************
 * @brief get one-byte data from sensor
 * 
 * @param value pointer to the place where the value will be stored
 * 
 * @return 0 if succes, else error number
 * **************************/
retcode_t sensor_get_byte(uint8_t *value);

#endif