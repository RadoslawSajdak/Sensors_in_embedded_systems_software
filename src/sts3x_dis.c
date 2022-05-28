#include "sts3x_dis.h"
#include "stdint.h"
#include "stdbool.h"
#include "tools.h"
#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx.h"
#endif

/***** Defines *****/
#define I2C_read_M(data_p, len)  ((hub_retcode_t) (HAL_I2C_Master_Receive(g_i2c_dev, device_address, (uint8_t *)data_p, len, HAL_MAX_DELAY)))
#define I2C_write_M(data_p) ((hub_retcode_t) (HAL_I2C_Master_Transmit(g_i2c_dev, device_address, (uint8_t *)data_p, 2, HAL_MAX_DELAY)))

// Values for CRC calculation
#define STS3X_CRC_POLYNOMIAL 0x31
#define STS3X_CRC_INITIAL 0xFF
#define STS3X_CRC_FINAL_XOR 0x00
#define STS3X_FXP_MULTIPLIER 512

/***** Local variables *****/
static I2C_HandleTypeDef* g_i2c_dev = NULL;
static uint8_t device_address = 0x4A << 1;
static uint8_t offset = 45;
static uint16_t multiplier = 175;
static bool periodicEnabled = false;

static uint8_t sts3x_crc(uint16_t input);

/****************************
 * @brief Calculate the CRC8 for a 2-byte input,
 * according to the CRC parameters provided in the sensor datasheet.
 * @see Sensirion STS3X-DIS October 2019 V5 documentation, section 4.11
 * @example sts3x_crc(0xBEEF) = 0x92
 * 
 * @param input - 2 byte input, as all commands sent and received from the
 *        sensor are 2 bytes (actually 3 bytes, but the 3rd one is the CRC)
 * @return CRC8 for input data
 * **************************/
static uint8_t sts3x_crc(uint16_t input) {
    uint8_t crc = STS3X_CRC_INITIAL;

    crc ^= (input & 0xFF00) >> 8;
    for (uint8_t bit = 8; bit > 0; --bit) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ STS3X_CRC_POLYNOMIAL;
        }
        else {
            crc = (crc << 1);
        } 
    }
    crc ^= (input & 0xFF);
    for (uint8_t bit = 8; bit > 0; --bit) {
        if (crc & 0x80) {
            crc = (crc << 1) ^ STS3X_CRC_POLYNOMIAL;
        }
        else {
            crc = (crc << 1);
        } 
    }

    return crc ^ STS3X_CRC_FINAL_XOR;
};


/***** Global functions definitions *****/

hub_retcode_t sts3x_dis_init(I2C_HandleTypeDef *hi2c1, bool addrPin, bool farenheit)
{
    g_i2c_dev = hi2c1;
    uint32_t temp;

    // The device address depends on the state of ADDR pin
    if (addrPin) device_address = 0x4B << 1;

    // Change processing params if Farenheit output is enabled
    if (farenheit) {
        offset = 49;
        multiplier = 315;
    }

    //if (OK != (hub_retcode_t)HAL_I2C_Init(g_i2c_dev)) return INIT_ERROR;
   
    return sts3x_get_temperature(&temp, REPEATABILITY_LOW); // We return success if we CONFIRM communication
}

hub_retcode_t sts3x_get_temperature(uint32_t* value, measurement_config_t configuration) {
    hub_retcode_t ret = OK;
    uint8_t retryCount = 5;
    uint8_t responseBuf[3];
    uint8_t maxWaitTime = STS_REPEATABILITY_HIGH_WAITTIME;

    switch (configuration)
    {
    case REPEATABILITY_LOW:
        maxWaitTime = STS_REPEATABILITY_LOW_WAITTIME;
        break;
    case REPEATABILITY_MEDIUM:
        maxWaitTime = STS_REPEATABILITY_MEDIUM_WAITTIME;
        break;
    default:
        break;
    }

    // Check if periodic measurements were enabled if a periodic read was requested
    if (configuration == PERIODIC_READ && !periodicEnabled) return ERROR;
    if (OK != I2C_write_M((uint16_t*) &configuration)) return ERROR;
    // Sensor responds with NACK until the data is ready, which results in HAL_ERROR
    // (Retry count might have to be higher, maybe better with timer dependent on measurement repeatability)
    do {
        HAL_Delay(maxWaitTime);
        ret = I2C_read_M(responseBuf, 3);
        retryCount--;
    } while (ret != OK && retryCount != 0);


    // If an ACK was received, 2 value and 1 CRC bytes are written to responseBuf
    if (ret == OK) {
        uint16_t rawValue = (responseBuf[0] << 8) + responseBuf[1];
        if (sts3x_crc(rawValue) != responseBuf[2]) return CRC_ERROR;
        *value = (uint32_t) STS3X_FXP_MULTIPLIER * ((float) (multiplier * rawValue)) / 65535 - offset;
    }
    return ret;
}

hub_retcode_t sts3x_start_periodic_acquisition(periodic_measurement_config_t configuration)
{
    hub_retcode_t ret = OK;
    ret = I2C_write_M((uint16_t*) &configuration);
    if (ret == OK) periodicEnabled = true;
    return ret;
}

hub_retcode_t sts3x_stop_periodic_acquisition() {
    hub_retcode_t ret = OK;
    uint16_t command = 0x3093;
    ret = I2C_write_M(&command);
    if (ret == OK) periodicEnabled = false;
    return ret;
}

hub_retcode_t sts3x_soft_reset() {
    hub_retcode_t ret = OK;
    uint16_t command = 0x30A2;
    ret = I2C_write_M(&command);
    if (ret == OK) periodicEnabled = false;
    return ret;
}

hub_retcode_t sts3x_set_heater_status(sts3x_heater_config_t heaterCommand) {
    hub_retcode_t ret = OK;
    ret = I2C_write_M((uint16_t*) &heaterCommand);
    return ret;
}

hub_retcode_t sts3x_get_status_register(STS3X_Status_Reg* statusRegister) {
    hub_retcode_t ret = OK;
    uint16_t command = 0xF32D;
    uint8_t buf[3];

    if (OK != I2C_write_M(&command)) return ERROR;
    if (OK != I2C_read_M(buf, 3)) return ERROR;
    
    if (sts3x_crc((uint16_t) ((buf[0] << 8) + buf[1])) != buf[2]) return CRC_ERROR;

    // If write, read, and CRC check were correct, write the respective statuses to passed struct
    statusRegister->alertPendingStatus = buf[0] & (1 << 7);
    statusRegister->heaterStatus = buf[0] & (1 << 5);
    statusRegister->tTrackingAlert = buf[0] & (1 << 2);
    statusRegister->systemReset = buf[1] & (1 << 4);
    statusRegister->commandStatus = buf[1] & (1 << 1);
    statusRegister->checksumStatus = buf[1] & 1;

    return ret;
}

hub_retcode_t sts3x_clear_status_register() {
    hub_retcode_t ret = OK;
    uint16_t command = 0x3041;
    ret = I2C_write_M(&command);
    return ret;
}
