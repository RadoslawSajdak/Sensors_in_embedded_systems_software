#ifndef __TIMERS_H__
#define __TIMERS_H__
#include "stdint.h"
#include "tools.h"
#include "stdbool.h"

typedef void (*timer_callback)(void);

/****************************
 * @brief Configure SysTick for miliseconds. Must be callet before anything else
 * **************************/
hub_retcode_t timers_init(void);

/****************************
 * @brief Add timer with IRQ and user callback. Calling it again with the same callback will restart the timer
 * 
 * @param timeout Timeout in miliseconds
 * @param callback Pointer to the function defined by user. Callbacks should be as short as possible
 * @param repeat Indicator if timer should be periodically
 * 
 * @return 0 if succes, else error number
 * **************************/
hub_retcode_t timers_add_timer(uint32_t timeout, timer_callback callback, bool repeat);

/****************************
 * @brief Stop and remove timer from list. It's called on the end of non periodically timers
 * 
 * @param callback pointer to the user defined function
 * 
 * @return 0 if succes, else error number; It will raise ARGUMENT_ERROR if you try to stop
 * timer which is not run
 * **************************/
hub_retcode_t timers_stop_timer(timer_callback callback);


/****************************
 * @brief Redefinition of HAL Delay function. Used because of local SysTick Handler
 * 
 * @param delay Delay in miliseconds
 * **************************/
void timers_delay(uint32_t delay);

#endif
