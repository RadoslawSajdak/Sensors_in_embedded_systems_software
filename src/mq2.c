/***** Includes *****/
#include "mq2.h"
#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#include "stm32f429xx.h"
#endif


/***** Defines *****/

/***** Local functions declatation *****/


/***** Local variables *****/
static  ADC_HandleTypeDef mq2_adc;


/***** Global functions definitions *****/
hub_retcode_t mq2_init(ADC_TypeDef * instance, uint32_t channel){

    ADC_ChannelConfTypeDef sConfig = {0};

    mq2_adc.Instance = instance;
    mq2_adc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    mq2_adc.Init.Resolution = ADC_RESOLUTION_12B;
    mq2_adc.Init.ScanConvMode = DISABLE;
    mq2_adc.Init.ContinuousConvMode = DISABLE;
    mq2_adc.Init.DiscontinuousConvMode = DISABLE;
    mq2_adc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    mq2_adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    mq2_adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    mq2_adc.Init.NbrOfConversion = 1;
    mq2_adc.Init.DMAContinuousRequests = DISABLE;
    mq2_adc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&mq2_adc) != HAL_OK)
    {
        return INIT_ERROR;
    }

    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&mq2_adc, &sConfig) != HAL_OK)
    {
        return INIT_ERROR;
    }
    return OK;
}

hub_retcode_t mq2_get_sample(uint16_t *value){
    HAL_ADC_Start(&mq2_adc);
    if (HAL_ADC_PollForConversion(&mq2_adc, 10) == HAL_OK) {
        *value = HAL_ADC_GetValue(&mq2_adc);
        return OK;
    }else{
        return TIMEOUT_ERROR;
    }
}