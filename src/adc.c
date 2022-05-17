#include "adc.h"
#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif
static ADC_HandleTypeDef        adc_instance;
static ADC_ChannelConfTypeDef   adc_channel;


hub_retcode_t adc_init(void)
{
    __HAL_RCC_ADC1_CLK_ENABLE();

    adc_instance.Instance = ADC1;
    adc_instance.Init.ContinuousConvMode = ENABLE;
    adc_instance.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc_instance.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_instance.Init.ScanConvMode = ADC_SCAN_DISABLE;
    adc_instance.Init.NbrOfConversion = 1;
    adc_instance.Init.DiscontinuousConvMode = DISABLE;
    adc_instance.Init.NbrOfDiscConversion = 1;
    if(HAL_OK != HAL_ADC_Init(&adc_instance)) return INIT_ERROR;

    HAL_Delay(200);

    if(HAL_OK != HAL_ADCEx_Calibration_Start(&adc_instance)) return INIT_ERROR;

    adc_channel.Channel = ADC_CHANNEL_0;
	adc_channel.Rank = ADC_REGULAR_RANK_1;
	adc_channel.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
	if(HAL_OK != HAL_ADC_ConfigChannel(&adc_instance, &adc_channel)) return INIT_ERROR;

    if(HAL_OK != HAL_ADC_Start(&adc_instance)) return INIT_ERROR;
    return OK;
}

uint32_t adc_get_data(void)
{
    return HAL_ADC_GetValue(&adc_instance);
}