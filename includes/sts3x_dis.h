/****************************************
 *  This library is just an example.    *
 *  Here you should put simplified      *
 *  library description                 *
 *  Author:    Miroslaw Wiacek          *
 *  Date:      May 2022                 *
 *  Github profile: @Soberat            *
 ***************************************/
#ifndef __STS3X_DIS_H__
#define __STS3X_DIS_H__

#include "stdint.h"
#include "tools.h"
#include "stm32f1xx.h"

// Defines for single shot measurements without clock streching
#define STS3X_CSOFF_REPEATABILITY_LOW 0x2416
#define STS3X_CSOFF_REPEATABILITY_MEDIUM 0x240B
#define STS3X_CSOFF_REPEATABILITY_HIGH 0x2400
#define STS3X_PERIODIC_READ 0xE000

#define STS3X_PERIODIC_05MPS_REP_LOW 0x202F
#define STS3X_PERIODIC_05MPS_REP_MEDIUM 0x2024
#define STS3X_PERIODIC_05MPS_REP_HIGH 0x2032

#define STS3X_PERIODIC_1MPS_REP_LOW 0x212D
#define STS3X_PERIODIC_1MPS_REP_MEDIUM 0x2126
#define STS3X_PERIODIC_1MPS_REP_HIGH 0x2130

#define STS3X_PERIODIC_2MPS_REP_LOW 0x222B
#define STS3X_PERIODIC_2MPS_REP_MEDIUM 0x2220
#define STS3X_PERIODIC_2MPS_REP_HIGH 0x2236

#define STS3X_PERIODIC_4MPS_REP_LOW 0x2329
#define STS3X_PERIODIC_4MPS_REP_MEDIUM 0x2322
#define STS3X_PERIODIC_4MPS_REP_HIGH 0x2334

#define STS3X_PERIODIC_10MPS_REP_LOW 0x272A
#define STS3X_PERIODIC_10MPS_REP_MEDIUM 0x2721
#define STS3X_PERIODIC_10MPS_REP_HIGH 0x2737

#define STS3X_HEATER_ENABLE 0x306D
#define STS3X_HEATER_DISABLE 0x3066

typedef struct {
	uint8_t checksumStatus;
	uint8_t commandStatus;
	uint8_t systemReset;
	uint8_t tTrackingAlert;
	uint8_t heaterStatus;
	uint8_t alertPendingStatus;
} STS3X_Status_Reg;

 /****************************
  * @brief Initializing sensor and I2C.
  * Must be called before anything else!
  *
  * @param hi2c1 pointer to i2c device
  * @param addr_pin state of ADDR pin on sensor, 0 is LOW, anything else is HIGH
  * @param farenheit - set this to 1 to report temperature in Farenheit instead of Celsius
  *
  * @return 0 if success, else error number
  * **************************/
enum retcode_t sts3x_dis_init(I2C_HandleTypeDef *hi2c1, uint8_t addrPin, uint8_t farenheit);

/****************************
 * @brief Get the temperature reported
 * by the sensor in units specified in sts3x_dis_init()
 *
 * @param value pointer to where measurement is to be written to
 * @param configuration - measurement configuration,
 *        one of STS3X_CSOFF_REPEATABILITY_ defines (or STS3X_PERIODIC_READ
 *        if periodic measurements were previously enabled)
 *
 * @return 0 if success, else error number
 * **************************/
enum retcode_t sts3x_get_temperature(float* value, uint16_t configuration);

enum retcode_t sts3x_start_periodic_acquisition(uint16_t configuration);

enum retcode_t sts3x_stop_periodic_acquisition();

enum retcode_t sts3x_soft_reset();

enum retcode_t sts3x_set_heater_status(uint16_t heaterCommand);

enum retcode_t sts3x_get_status_register(STS3X_Status_Reg* statusRegister);

enum retcode_t sts3x_clear_status_register();
#endif
