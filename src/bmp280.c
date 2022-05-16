/***** Includes *****/
#include "bmp280.h"
#include "spi.h"
#include <string.h>
#include <stdint.h>

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#endif

/***** Defines *****/
#define BMP280_READ_M(reg, data, len)           (ret |= (spi_get_register(&bmp_spi_dev, bmp_cs_gpio, bmp_cs_pin, reg, data, len)))
#define BMP280_WRITE_M(reg, data)               (ret |= (spi_set_register(&bmp_spi_dev, bmp_cs_gpio, bmp_cs_pin, reg, data)))
#define CHECK_ERROR_M(error)                    if(OK != (ret |= error)) return ret;

/***** Local functions declatation *****/
hub_retcode_t bmp280_get_calib(void);
hub_retcode_t bmp280_get_raw(bmp280_raw_data_s *raw);
hub_retcode_t bmp280_compensate(bmp280_raw_data_s *raw, bmp280_data_s *data);

/***** Local variables *****/
static SPI_HandleTypeDef        bmp_spi_dev;
static GPIO_TypeDef             *bmp_cs_gpio = NULL;
static uint16_t                 bmp_cs_pin = 0;
static bmp280_config_s          config;
bmp280_calib_digit_s            calib;




hub_retcode_t bmp280_init(SPI_TypeDef *SPI_nr, GPIO_TypeDef* CS_GPIO, uint16_t CS_GPIO_Pin)
{   
    hub_retcode_t ret = OK;

    if (OK != spi_init(&bmp_spi_dev, SPI_nr)) return INIT_ERROR;

    spi_cs_init(CS_GPIO, CS_GPIO_Pin);
    bmp_cs_gpio = CS_GPIO;
    bmp_cs_pin = CS_GPIO_Pin;

    CHECK_ERROR_M(bmp280_get_calib());

    config.mode = BMP280_CTRL_SLEEP;
    config.osrs_pressure = BMP280_CTRL_OSRS_P08;
    config.osrs_temperature = BMP280_CTRL_OSRS_T08;
    CHECK_ERROR_M(bmp280_set_config(&config));

    return ret;
}

hub_retcode_t bmp280_reset(void)
{
    hub_retcode_t ret = OK;

    uint8_t data = BMP280_RESET_VAL;
    BMP280_WRITE_M(BMP280_REG_RESET, &data);

    return ret;
}

hub_retcode_t bmp280_get_config(bmp280_config_s *conf)
{
    hub_retcode_t ret = OK;

    uint8_t data[2] = {0,0};
    BMP280_READ_M(BMP280_REG_CTRL_MEAS, data, 2);
    conf->osrs_temperature = data[0] & BMP280_CTRL_OSRS_T_MASK;
    conf->osrs_pressure = data[0] & BMP280_CTRL_OSRS_P_MASK;
    conf->mode = data[0] & BMP280_CTRL_MODE_MASK;

    conf->standby = data[1] & BMP280_CONFIG_SB_MASK;
    
    return ret;
}

hub_retcode_t bmp280_set_config(bmp280_config_s *conf)
{
    hub_retcode_t ret = OK;

    config = *conf;
    uint8_t data = conf->osrs_temperature | conf->osrs_pressure | conf->mode;
    BMP280_WRITE_M(BMP280_REG_CTRL_MEAS, &data);
    CHECK_ERROR_M(ret);

    data = conf->standby;
    BMP280_WRITE_M(BMP280_REG_CONFIG, &data);

    return ret;
}

hub_retcode_t bmp280_ready(void)
{
    hub_retcode_t ret = OK;

    uint8_t data = 0xff;
    for(uint8_t i = 0; i < 100; i++){
        if(BMP280_STATUS_DONE == data || OK != ret) return ret;
        BMP280_READ_M(BMP280_REG_STATUS, &data, 1);
        HAL_Delay(200);
    }

    return TIMEOUT_ERROR;
}

hub_retcode_t bmp280_get_data(bmp280_data_s *data)
{
    hub_retcode_t ret = OK;

    bmp280_raw_data_s raw;
    ret |= bmp280_get_raw(&raw);
    ret |= bmp280_compensate(&raw, data);

    return ret;
}


/***** Local functions definitions *****/

hub_retcode_t bmp280_get_calib(void)
{
    hub_retcode_t ret = OK;

    // get calibration data
    uint8_t dig[26];
    BMP280_READ_M(BMP280_dig_T1, dig, BMP280_dig_T_size + BMP280_dig_P_size);
    CHECK_ERROR_M(ret);

    memcpy(&calib, dig, sizeof(bmp280_calib_digit_s));
    
    return OK;
}

hub_retcode_t bmp280_get_raw(bmp280_raw_data_s *raw)
{
    hub_retcode_t ret = OK;

    // if sampling data off it is error
    if(BMP280_CTRL_OSRS_P00 == config.osrs_pressure && BMP280_CTRL_OSRS_T00 == config.osrs_temperature) return NO_CONFIG_ERROR;
    
    if(BMP280_CTRL_SLEEP == config.mode) config.mode = BMP280_CTRL_FORCED;

    CHECK_ERROR_M(bmp280_set_config(&config));
    
    CHECK_ERROR_M(bmp280_ready());

    uint8_t data[6];
    BMP280_READ_M(BMP280_REG_PRESS_MSB, data, 6);
    raw->pressure = data[0] << 12   |
                    data[1] << 4    |
                    data[2] >> 4;

    raw->temperature =  data[3] << 12   |
                        data[4] << 4    |
                        data[5] >> 4;

    if(BMP280_CTRL_OSRS_P00 == config.osrs_pressure) return INCORRECT_PRESSURE_ERROR;
    return ret;
}

hub_retcode_t bmp280_compensate(bmp280_raw_data_s *raw, bmp280_data_s *data)
{
    int32_t var1t, var2t, tFine, millicelsius;
    int32_t adc_T = raw->temperature;

    var1t    = ((((adc_T>>3) - ((int32_t)calib.dig_t1<<1))) * ((int32_t)calib.dig_t2)) >> 11;
    var2t    = (((((adc_T>>4) - ((int32_t)calib.dig_t1)) * ((adc_T>>4) - ((int32_t)calib.dig_t1))) >> 12) * ((int32_t)calib.dig_t3))>>14;
    tFine   = var1t + var2t;
    millicelsius = (tFine * 50 + 1280) >> 8;
    data->temperature = millicelsius;

    int32_t adc_P = raw->pressure;
    int64_t var1, var2, p;
    var1 = ((int64_t)tFine) - 128000;
    var2 = var1 * var1 * (int64_t)calib.dig_p6;
    var2 = var2 + ((var1*(int64_t)calib.dig_p5)<<17);
    var2 = var2 + (((int64_t)calib.dig_p4)<<35);
    var1 = ((var1 * var1 * (int64_t)calib.dig_p3)>>8) + ((var1 * (int64_t)calib.dig_p2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)calib.dig_p1)>>33;
    if (var1 == 0)        return INCORRECT_PRESSURE_ERROR; // avoid exception caused by division by zero

    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((int64_t)calib.dig_p9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)calib.dig_p8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)calib.dig_p7)<<4);
    data->pressure = ((uint32_t)p/256);

    return OK;
}
