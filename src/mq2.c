/***** Includes *****/
#include "adc.h"
#include "mq2.h"
#include "timers.h"
#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#endif


/***** Defines *****/
#define RL      5       /* Total load resistance in kohm */
#define ROUT    3       /* Voltage divider output resistance */

typedef struct
{
    bool track_data;
    mq2_callback_t callback;
    uint16_t treshold;
    uint16_t Ro;

}mq2_config_s;

/***** Local functions declatation *****/
void mq2_update(void);

/***** Local variables *****/
static ADC_HandleTypeDef mq2_adc;
static uint16_t value;
static mq2_config_s mq2_config;

/***** Global functions definitions *****/
hub_retcode_t mq2_init(ADC_TypeDef *instance, uint32_t channel){

    return adc_init(&mq2_adc, instance, channel);
}

hub_retcode_t mq2_track_data(uint32_t timeout, uint16_t treshold, mq2_callback_t alarm){

    mq2_config.callback = alarm;
    mq2_config.treshold = treshold;
    mq2_config.track_data = 1;

    return timers_add_timer(timeout, mq2_update, 1);
}

uint16_t mq2_get_sample(void){
    
    return value;
}

void mq2_update(void){
    
    adc_get_sample(&mq2_adc, &value);
    if(value > mq2_config.treshold && mq2_config.track_data){
        mq2_config.callback();
    }
}

hub_retcode_t mq2_calib(void){

    hub_retcode_t ret = OK;
    uint16_t out;
    do{
        ret = adc_get_sample(&mq2_adc, &out);
    }while(OK != ret);
    mq2_config.Ro = (4095 * ROUT - RL * out) / out;
    
    return ret;
}