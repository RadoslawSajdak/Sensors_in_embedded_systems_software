#ifndef __TIMERS_H__
#define __TIMERS_H__
#include "stdint.h"
#include "tools.h"
#include "stdbool.h"

typedef void (*timer_callback)(void);

void timers_init(void);
hub_retcode_t timers_add_timer(uint32_t timeout, timer_callback callback, bool repeat);
hub_retcode_t timers_stop_timer(timer_callback callback);

#endif
