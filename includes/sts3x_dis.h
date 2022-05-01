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
#include "tools.h"
#include "stm32f1xx.h"

// Defines for single shot measurements without clock streching
#define STS3X_CSOFF_REPEATABILITY_LOW 0x2416
#define STS3X_CSOFF_REPEATABILITY_MEDIUM 0x240B
#define STS3X_CSOFF_REPEATABILITY_HIGH 0x2400
#define STS3X_PERIODIC_READ 0xE000

// Defines for periodic measurement configurations
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

/****************************
 * @brief Start the periodic temperature sampling
 * on the sensor, according to the configuration supplied.
 * This only tells the sensor to sample not on demand, but periodically.
 * To read the periodic data, @see sts3x_get_temperature
 *
 * @param configuration - measurement configuration,
 *        one of STS3X_PERIODIC_XMPS defines
 *
 * @return 0 if success, else error number
 * **************************/
enum retcode_t sts3x_start_periodic_acquisition(uint16_t configuration);

/****************************
 * @brief Stop the periodic temperature sampling.
 *
 * @return 0 if success, else error number
 * **************************/
enum retcode_t sts3x_stop_periodic_acquisition();

/****************************
 * @brief Trigger a soft reset on sensor.
 * @see Sensirion STS3X-DIS October 2019 V5 documentation, section 4.8
 *
 * @return 0 if success, else error number
 * **************************/
enum retcode_t sts3x_soft_reset();

/****************************
 * @brief Enable or disable the built-in sensor heater.
 * @see Sensirion STS3X-DIS October 2019 V5 documentation, section 4.9
 *
 * @param heaterCommand - command to enable/disable the heater,
 *        one of STS3X_HEATER_ defines.
 * @return 0 if success, else error number
 * **************************/
enum retcode_t sts3x_set_heater_status(uint16_t heaterCommand);

/****************************
 * @brief Fetch the sensor status register data.
 * @see Sensirion STS3X-DIS October 2019 V5 documentation, section 4.10, table 16
 *
 * @param statusRegister - pointer to STS3X_Status_Reg, to which the data will be written.
 * @see STS3X_Status_Reg struct definition
 * @return 0 if success, else error number
 * **************************/
enum retcode_t sts3x_get_status_register(STS3X_Status_Reg* statusRegister);

/****************************
 * @brief Clear the non-reserved status register bits.
 * @see Sensirion STS3X-DIS October 2019 V5 documentation, section 4.10, table 16
 *
 * @return 0 if success, else error number
 * **************************/
enum retcode_t sts3x_clear_status_register();
#endif
