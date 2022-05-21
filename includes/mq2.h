#ifndef __MQ2_H__
#define __MQ2_H__

#include "tools.h"

typedef void (*mq2_callback_t)(void);

hub_retcode_t mq2_init(ADC_TypeDef *instance, uint32_t channel);
uint16_t mq2_get_sample();
hub_retcode_t mq2_track_data(uint32_t timeout, uint16_t treshold, mq2_callback_t alarm);

#endif  //__MQ2_H__
