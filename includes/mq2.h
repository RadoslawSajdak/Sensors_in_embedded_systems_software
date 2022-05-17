#ifndef __MQ2_H__
#define __MQ2_H__

#include "tools.h"

hub_retcode_t mq2_init(ADC_TypeDef *instance, uint32_t channel);
hub_retcode_t mq2_get_sample(uint16_t *value);

#endif  //__MQ2_H__
