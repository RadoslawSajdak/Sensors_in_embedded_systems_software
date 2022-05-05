/***** Includes *****/
#include "example.h"
#include "stdint.h"

/***** Defines *****/
#define I2C_read_M(data_p, len) (ret = (I2C_read(g_i2c_dev, data_p, len)))

/***** Local functions declatation *****/
static retcode_t get_data(uint8_t *array, uint8_t len);


/***** Local variables *****/

static uint8_t                  g_example_variable = 0;  //Ok
static I2C_HandleTypeDef        *g_i2c_dev = NULL;

----> uint8_t example_variable;               //Totally not ok

/***** Global functions definitions *****/

retcode_t sensor_init(I2C_HandleTypeDef *hi2c1)
{
    retcode_t ret = SUCCESS;
    g_i2c_dev = hi2c1;
    if(SUCCESS != I2C_init(g_i2c_dev)) return ERROR; //Every function should be checked if it's possible

    return I2C_write(hi2c1, "Hello", sizeof("Hello")); //We return success if we CONFIRM communication
}

retcode_t sensor_get_byte(uint8_t *value)
{
    return get_data(value, 1); //Get one byte
}

/***** Local functions definitions *****/
static retcode_t get_data(uint8_t *array, uint8_t len)
{
    retcode_t ret = SUCCESS;
    ret = I2C_write(g_i2c_dev, "GET_STH", sizeof("GET_STH"));
    ret = I2C_read(g_i2c_dev, array, len);

    I2C_read_M(array, len); // Just an example. Doing the same as above

    return ret;
}