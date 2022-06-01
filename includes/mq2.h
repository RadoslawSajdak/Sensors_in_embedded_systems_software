#ifndef __MQ2_H__
#define __MQ2_H__

#include "tools.h"

typedef enum {
    LPG_MQ2 = 0,
    CO_MQ2 = 1,
    SMOKE_MQ2 = 2,
} mq2_gas_t;

typedef void (*mq2_callback_t)(void);

/****************************
 * @brief Initializing mq2. 
 * Must be called before anything else!
 * 
 * @param instance  \ adc connectd to sensor
 * @param channel   /
 * 
 * @return 0 if succes, else error number
 * **************************/
hub_retcode_t mq2_init(ADC_TypeDef *instance, uint32_t channel);

/****************************
 * @brief Get raw MQ2 sample
 * 
 * @return raw MQ2 ADC value
 * **************************/
uint16_t mq2_get_sample();

/****************************
 * @brief Init data tracking
 * 
 * @param timeout sample interval
 * @param gas select gas to track {LPG_MQ2, CO_MQ2, SMOKE_MQ2}
 * @param treshold selected gas max value before activation 
 * @param alarm callback to activate when higher than treshold
 * 
 * @return 0 if succes, else error number
 * **************************/
hub_retcode_t mq2_track_data(uint32_t timeout, mq2_gas_t gas, uint16_t treshold, mq2_callback_t alarm);

/****************************
 * @brief Stop tracking data
 * 
 * @return 0 if succes, else error number
 * **************************/
hub_retcode_t mq2_stop_tracking(void);

/****************************
 * @brief Get last sample in ppm
 * 
 * @param gas select gas {LPG_MQ2, CO_MQ2, SMOKE_MQ2}
 * 
 * @return gas ppm
 * **************************/
uint16_t mq2_get_ppm(mq2_gas_t gas);

#endif  //__MQ2_H__
