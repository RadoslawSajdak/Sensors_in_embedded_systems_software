#ifndef __ADC_H__
#define __ADC_H__
#include "stdint.h"
#include "tools.h"

hub_retcode_t adc_init(void);
uint32_t adc_get_data(void);
#endif