/*******************************************************************************
 *  A library to handle most of Sensirion STS3X-DIS sensor functions           *
 *  (clock streching is not supported, and neither is reset via general call)  *
 *  Based on Sensirion STS3X-DIS October 2019 Version 5 datasheet              *
 *                                                                             *
 *  Author:    Miroslaw Wiacek                                                 *
 *  Date:      May 2022                                                        *
 *  Github profile: @Soberat                                                   *
 ******************************************************************************/
#ifndef __STS3X_DIS_H__
#define __STS3X_DIS_H__

#include "stdint.h"
#include "stdbool.h"
#include "tools.h"
#include "stm32f1xx.h"

// Enum for heater configuration commands
typedef enum {
  STS3X_HEATER_ENABLE = 0x6D30,
  STS3X_HEATER_DISABLE = 0x6630
} sts3x_heater_config_t;

// Enum for single shot measurements without clock streching
typedef enum {
  REPEATABILITY_LOW = 0x1624,
  REPEATABILITY_MEDIUM = 0x0B24,
  REPEATABILITY_HIGH = 0x0024,
  PERIODIC_READ = 0x00E0
} measurement_config_t;

// Enum for periodic measurement configurations
typedef enum {
  REP_LOW_05MPS = 0x2F20,
  REP_MEDIUM_05MPS = 0x2420,
  REP_HIGH_05MPS = 0x3220,
  REP_LOW_1MPS = 0x2D21,
  REP_MEDIUM_1MPS = 0x2621,
  REP_HIGH_1MPS = 0x3021,
  REP_LOW_2MPS = 0x2B22,
  REP_MEDIUM_2MPS = 0x2022,
  REP_HIGH_2MPS = 0x3622,
  REP_LOW_4MPS = 0x2923,
  REP_MEDIUM_4MPS = 0x2223,
  REP_HIGH_4MPS = 0x3423,
  REP_LOW_10MPS = 0x2A27,
  REP_MEDIUM_10MPS = 0x2127,
  REP_HIGH_10MPS = 0x3727
} periodic_measurement_config_t;

// Any non-zero state of the member should be treated as HIGH.
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
  * @param addr_pin state of ADDR pin on sensor, false is LOW, true is HIGH
  * @param farenheit - set this to true to report temperature in Farenheit instead of Celsius
  *
  * @return 0 if success, else error number
  * **************************/
hub_retcode_t sts3x_dis_init(I2C_HandleTypeDef *hi2c1, bool addrPin, bool farenheit);

/****************************
 * @brief Get the temperature reported
 * by the sensor in units specified in sts3x_dis_init()
 *
 * @param value pointer to where measurement is to be written to
 * @param configuration - measurement configuration, member of 
 *         measurement_config_t enum (PERIODIC_READ only if periodic measurements were previously enabled)
 *
 * @return 0 if success, else error number
 * **************************/
hub_retcode_t sts3x_get_temperature(float* value, measurement_config_t configuration);

/****************************
 * @brief Start the periodic temperature sampling
 * on the sensor, according to the configuration supplied.
 * This only tells the sensor to sample not on demand, but periodically.
 * To read the periodic data, @see sts3x_get_temperature
 *
 * @param configuration - measurement configuration, member of periodic_measurement_config_t enum
 *
 * @return 0 if success, else error number
 * **************************/
hub_retcode_t sts3x_start_periodic_acquisition(periodic_measurement_config_t configuration);

/****************************
 * @brief Stop the periodic temperature sampling.
 *
 * @return 0 if success, else error number
 * **************************/
hub_retcode_t sts3x_stop_periodic_acquisition();

/****************************
 * @brief Trigger a soft reset on sensor.
 * @see Sensirion STS3X-DIS October 2019 V5 documentation, section 4.8
 *
 * @return 0 if success, else error number
 * **************************/
hub_retcode_t sts3x_soft_reset();

/****************************
 * @brief Enable or disable the built-in sensor heater.
 * @see Sensirion STS3X-DIS October 2019 V5 documentation, section 4.9
 *
 * @param heaterCommand - member of sts3x_heater_config_t enum
 * @return 0 if success, else error number
 * **************************/
hub_retcode_t sts3x_set_heater_status(sts3x_heater_config_t heaterCommand);

/****************************
 * @brief Fetch the sensor status register data.
 * @see Sensirion STS3X-DIS October 2019 V5 documentation, section 4.10, table 16
 *
 * @param statusRegister - pointer to STS3X_Status_Reg, to which the data will be written.
 * @see STS3X_Status_Reg struct definition
 * @return 0 if success, else error number
 * **************************/
hub_retcode_t sts3x_get_status_register(STS3X_Status_Reg* statusRegister);

/****************************
 * @brief Clear the non-reserved status register bits.
 * @see Sensirion STS3X-DIS October 2019 V5 documentation, section 4.10, table 16
 *
 * @return 0 if success, else error number
 * **************************/
hub_retcode_t sts3x_clear_status_register();
#endif
