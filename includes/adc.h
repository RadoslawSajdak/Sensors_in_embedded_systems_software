#ifndef __ADC_H__
#define __ADC_H__

#include "tools.h"

/****************************
 * @brief Initializing adc. 
 * Must be called before anything else!
 * 
 * @param hadc pointer to adc device
 * @param instance
 * @param channel 
 * 
 * @return 0 if succes, else error number
 * **************************/
hub_retcode_t adc_init(ADC_HandleTypeDef *hadc, ADC_TypeDef *instance, uint32_t channel);

/****************************
 * @brief Get single sample from adc
 * 
 * @param hadc pointer to adc device
 * @param value pointer to adc output buffer
 * 
 * @return 0 if succes, else error number
 * **************************/
hub_retcode_t adc_get_sample(ADC_HandleTypeDef *hadc, uint16_t *value);

#endif  //__ADC_H__
