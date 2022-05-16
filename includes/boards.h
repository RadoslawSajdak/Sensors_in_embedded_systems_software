#ifndef __BOARDS_H__
#define __BOARDS_H__


#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#endif

void SystemClock_Config(void);

#endif //__BOARDS_H__
