/***** Includes *****/
#include "adc.h"
#include "mq2.h"
#include "timers.h"
#include "math.h"
#include <stdio.h>

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#endif


/***** Defines *****/
#define RL      5000        /* Total load resistance in kohm */
#define ROUT    3000        /* Voltage divider output resistance */

/***** Local consts *****/
const double LPG_GAS[2] = {-0.47, 1.291};
const double CO_GAS[2] = {-0.34, 1.502};
const double SMOKE_GAS[2] = {-0.44, 1.542};
const double* MQ2_GAS[3] = {LPG_GAS, CO_GAS, SMOKE_GAS};

typedef struct
{
    bool track_data;
    mq2_gas_t track_gas;
    mq2_callback_t callback;
    uint16_t treshold;

}mq2_config_s;

typedef struct
{
    uint32_t Ro;
    uint32_t Rs;
    uint16_t raw;

}mq2_data_s;

/***** Local functions declatation *****/
void mq2_update(void);
hub_retcode_t mq2_calib(void);
hub_retcode_t mq2_calculate_resistance(uint16_t voltage, uint32_t* resistance);

/***** Local variables *****/
static ADC_HandleTypeDef mq2_adc;
static mq2_config_s mq2_config;
static mq2_data_s mq2_data;


/***** Global functions definitions *****/
hub_retcode_t mq2_init(ADC_TypeDef *instance, uint32_t channel){

    hub_retcode_t ret = adc_init(&mq2_adc, instance, channel);
    ret |= mq2_calib();
    mq2_update();

    return ret;
}

hub_retcode_t mq2_track_data(uint32_t timeout, mq2_gas_t gas, uint16_t treshold, mq2_callback_t alarm){

    mq2_config.track_data = 1;
    mq2_config.track_gas = gas;
    mq2_config.callback = alarm;
    mq2_config.treshold = treshold;

    return timers_add_timer(timeout, mq2_update, 1);
}

hub_retcode_t mq2_stop_tracking(void)
{
    return timers_stop_timer(mq2_update);
}

uint16_t mq2_get_sample(void){
    
    return mq2_data.raw;
}

uint16_t mq2_get_ppm(mq2_gas_t gas){

    double ppm = pow(10, (log10(mq2_data.Rs/mq2_data.Ro)-MQ2_GAS[gas][1])/MQ2_GAS[gas][0]);     // DATASHEET
    return (uint16_t) ppm;
}


/***** Local functions definitions *****/
void mq2_update(void){
    
    adc_get_sample(&mq2_adc, &mq2_data.raw);
    //mq2_data.raw = 3649; //USU
    mq2_calculate_resistance(mq2_data.raw, &mq2_data.Rs);
    if(mq2_get_ppm(mq2_config.track_gas) > mq2_config.treshold && mq2_config.track_data){
        mq2_config.callback();
    }
}

hub_retcode_t mq2_calib(void){

    hub_retcode_t ret = OK;
    uint16_t out;
    do{
        ret = adc_get_sample(&mq2_adc, &out);
    }while(OK != ret);
    //out = 3649;                                     // comment out with hardware
    ret = mq2_calculate_resistance(out, &mq2_data.Ro);     
    
    return ret;
}

hub_retcode_t mq2_calculate_resistance(uint16_t voltage, uint32_t* resistance){

    if(RL * voltage > 6206 * ROUT || voltage == 0) {
        return ARGUMENT_ERROR;
    }
    *resistance = ((6206 * ROUT - RL * voltage)/voltage);  // RS=(VCC*R3-OUT(R3+R2))/VOUT  
    return OK;
}


