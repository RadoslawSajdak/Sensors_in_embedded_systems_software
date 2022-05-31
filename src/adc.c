/***** Includes *****/
#include "adc.h"
#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
#ifdef STM32F4
#include "stm32f4xx_hal.h"
#endif



/***** Global functions definitions *****/
hub_retcode_t adc_init(ADC_HandleTypeDef *hadc, ADC_TypeDef *instance, uint32_t channel){

    ADC_ChannelConfTypeDef sConfig = {0};

    hadc->Instance = instance;
#ifdef STM32F4
    hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc->Init.Resolution = ADC_RESOLUTION_12B;
    hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc->Init.DMAContinuousRequests = DISABLE;
    hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
#endif
    hadc->Init.ScanConvMode = DISABLE;
    hadc->Init.ContinuousConvMode = DISABLE;
    hadc->Init.DiscontinuousConvMode = DISABLE;
    hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc->Init.NbrOfConversion = 1;
    
    if (HAL_ADC_Init(hadc) != HAL_OK)
    {
        return INIT_ERROR;
    }

    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_7CYCLES_5;
    if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
    {
        return INIT_ERROR;
    }
    return OK;
}

hub_retcode_t adc_get_sample(ADC_HandleTypeDef *hadc, uint16_t *value){
    HAL_ADC_Start(hadc);
    if (HAL_ADC_PollForConversion(hadc, 10) == HAL_OK) {
        *value = HAL_ADC_GetValue(hadc);
        return OK;
    }else{
        return TIMEOUT_ERROR;
    }
}