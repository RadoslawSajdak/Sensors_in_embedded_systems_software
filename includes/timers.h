#ifndef __TIMERS_H__
#define __TIMERS_H__
#include "stdint.h"
#ifdef STM32F1
    #include "stm32f1xx.h"
    #include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
    #include "stm32f4xx.h"
    #include "stm32f4xx_hal.h"
#endif
#include "tools.h"

typedef void (*timer_callback)(void);

void timers_init(void);
hub_retcode_t timers_add_timer(uint32_t timeout, timer_callback callback);

#endif
