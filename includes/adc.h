#ifndef __ADC_H__
#define __ADC_H__

#include "tools.h"

hub_retcode_t adc_init(ADC_HandleTypeDef *hadc, ADC_TypeDef *instance, uint32_t channel);
hub_retcode_t adc_get_sample(ADC_HandleTypeDef *hadc, uint16_t *value);

#endif  //__ADC_H__
