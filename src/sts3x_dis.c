#include "sts3x_dis.h"
#include "stdint.h"
#include "stm32f1xx.h"
#include "tools.h"


/***** Defines *****/
#define I2C_read_M(data_p, len) (ret = (HAL_I2C_Master_Receive(g_i2c_dev, device_address, data_p, len, 500)))
#define I2C_write_M(data_p) (ret = (HAL_I2C_Master_Transmit(g_i2c_dev, device_address, data_p, 2, 500)))

/***** Local variables *****/
static I2C_HandleTypeDef* g_i2c_dev = NULL;
static uint8_t device_address = 0x4A;
static uint8_t offset = 45;
static uint16_t multiplier = 175;
static uint8_t periodicEnabled = 0;

static const uint8_t crc_polynomial = 0x31;
static const uint8_t crc_initial = 0xFF;
static const uint8_t crc_final_xor = 0x00;


static uint8_t sts3x_crc(uint16_t input) {
    uint8_t crc = crc_initial;

    crc ^= (input & 0xFF00) >> 8;
    for (uint8_t bit = 8; bit > 0; --bit) {
        if (crc & 0x80) crc = (crc << 1) ^ crc_polynomial;
        else crc = (crc << 1);
    }
    crc ^= (input & 0xFF);
    for (uint8_t bit = 8; bit > 0; --bit) {
        if (crc & 0x80) crc = (crc << 1) ^ crc_polynomial;
        else crc = (crc << 1);
    }

    return crc ^ crc_final_xor;
};


/***** Global functions definitions *****/

enum retcode_t sts3x_dis_init(I2C_HandleTypeDef* hi2c1, uint8_t addr_pin, uint8_t farenheit)
{
    enum retcode_t ret = OK;
    g_i2c_dev = hi2c1;
    float f;

    if (addr_pin > 0) {
        device_address = 0x4B;
    }

    if (farenheit == 1) {
        offset = 49;
        multiplier = 315;
    }

    if (OK != HAL_I2C_Init(g_i2c_dev)) return ERROR; //Every function should be checked if it's possible
   
    return sts3x_get_temperature(&f, STS3X_CSOFF_REPEATABILITY_LOW); //We return success if we CONFIRM communication
}

enum retcode_t sts3x_get_temperature(float* value, uint16_t configuration)
{
    enum retcode_t ret = OK;
    uint8_t retryCount = 5;
    uint8_t responseBuf[3];

    if (configuration == STS3X_PERIODIC_READ && periodicEnabled != 1) return ERROR;
    if (OK != I2C_write_M(&configuration)) return ERROR;

    do {
        I2C_read_M(responseBuf, 3);
        retryCount--;
    } while (ret == HAL_ERROR && retryCount != 0);

    if (ret == HAL_OK) {
        uint16_t rawValue = responseBuf[0] << 8 + responseBuf[1];
        if (sts3x_crc(rawValue) != responseBuf[2]) return CRC_FAIL;
        *value = ((float) (multiplier * rawValue)) / 65535 - offset;
        return ret;
    } else {
        return ret;
    }
}

enum retcode_t sts3x_start_periodic_acquisition(uint16_t configuration) {
    enum retcode_t ret = OK;
    I2C_write_M(&configuration);
    return ret;
}

enum retcode_t sts3x_stop_periodic_acquisition() {
    enum retcode_t ret = OK;
    uint16_t command = 0x3093;
    I2C_write_M(&command);
    return ret;
}

enum retcode_t sts3x_soft_reset() {
    enum retcode_t ret = OK;
    uint16_t command = 0x30A2;
    I2C_write_M(&command);
    return ret;
}

enum retcode_t sts3x_set_heater_status(uint16_t heater_command) {
    enum retcode_t ret = OK;
    I2C_write_M(&heater_command);
    return ret;
}

enum retcode_t sts3x_get_status_register(STS3X_Status_Reg *statusRegister) {
    enum retcode_t ret = OK;
    uint16_t command = 0xF32D;
    uint8_t buf[3];

    if (OK != I2C_write_M(&command)) return ERROR;
    if (OK != I2C_read_M(&buf, 3)) return ERROR;
    
    if (sts3x_crc((uint16_t) ((buf[0] << 8) + buf[1])) != buf[2]) return CRC_FAIL;

    statusRegister->alertPendingStatus = buf[0] & (1 << 7);
    statusRegister->heaterStatus = buf[0] & (1 << 5);
    statusRegister->tTrackingAlert = buf[0] & (1 << 2);
    statusRegister->systemReset = buf[1] & (1 << 4);
    statusRegister->commandStatus = buf[1] & (1 << 1);
    statusRegister->checksumStatus = buf[1] & 1;

    return ret;
}

enum retcode_t sts3x_clear_status_register() {
    enum retcode_t ret = OK;
    uint16_t command = 0x3041;
    I2C_write_M(&command);
    return ret;
}
