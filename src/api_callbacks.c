#include "api_callbacks.h"
#include "api_uart.h"
#include "tools.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

sensor_choice_t                         g_chosen_sensor = DEFAULT_COMMANDS;
uint32_t                                g_command_parameters[MAX_COMMAND_PARAMETERS];
uint8_t                                 g_command_param_counter = 0;
static bmp280_data_s                    g_bmp_data = {0};
static uint32_t                         g_sts32_val = 0;
static measurement_config_t             g_sts32_config = REPEATABILITY_LOW;

hub_retcode_t api_send_response(const uint32_t value)
{
    char msg[20] = {0};
    sprintf(msg, "%lu", value);
    return api_uart_tx((uint8_t *)msg, strlen(msg));
}

void set_irq_pin(void)
{
    HAL_GPIO_WritePin(IRQ_GPIO, IRQ_PIN, 0);
}
void clear_irq_pin(void)
{
    HAL_GPIO_WritePin(IRQ_GPIO, IRQ_PIN, 1);
}

void api_menu_set_bmp(void)
{
    g_chosen_sensor = BMP_COMMANDS;
}
void api_menu_set_sts(void)
{
    g_chosen_sensor = STS_COMMANDS;
}
void api_menu_set_mq(void)
{
    g_chosen_sensor = MQ_COMMANDS;
}
void api_menu_set_default(void)
{
    g_chosen_sensor = DEFAULT_COMMANDS;
}

void api_bmp_get_data(void)
{
    memset(&g_bmp_data, 0, sizeof(bmp280_data_s));
    bmp280_get_data(&g_bmp_data);
    api_send_response(g_bmp_data.temperature);
}

void api_sts_get_temp(void)
{
    g_sts32_val = 0U;
    sts3x_get_temperature(&g_sts32_val, g_sts32_config);
    api_send_response(g_sts32_val);
}

void api_sts_set_repeatability_low(void)
{
    g_sts32_config = REPEATABILITY_LOW;
}

void api_sts_set_repeatability_medium(void)
{
    g_sts32_config = REPEATABILITY_MEDIUM;
}

void api_sts_set_repeatability_high(void)
{
    g_sts32_config = REPEATABILITY_HIGH;    
}

void api_sts_set_heater_on(void)
{
    sts3x_set_heater_status(STS3X_HEATER_ENABLE);
}

void api_sts_set_heater_off(void)
{
    sts3x_set_heater_status(STS3X_HEATER_DISABLE);
}

void api_sts_soft_reset(void)
{
    sts3x_soft_reset();
}

void api_mq2_get_sample(void)
{
    uint16_t temp = mq2_get_sample();
    clear_irq_pin();
    api_send_response((uint32_t)temp);
}

void api_mq2_get_LPG(void)
{
    uint16_t temp = mq2_get_ppm(LPG_MQ2);
    clear_irq_pin();
    api_send_response((uint32_t)temp);
}

void api_mq2_get_CO(void)
{
    uint16_t temp = mq2_get_ppm(CO_MQ2);
    clear_irq_pin();
    api_send_response((uint32_t)temp);
}

void api_mq2_get_SMOKE(void)
{
    uint16_t temp = mq2_get_ppm(SMOKE_MQ2);
    clear_irq_pin();
    api_send_response((uint32_t)temp);
}

void api_mq2_set_tracking(void)
{
    if(g_command_param_counter != 3)
    {
        api_send_response(1);
    }
    else
    {
        mq2_track_data(g_command_parameters[0], (mq2_gas_t)g_command_parameters[1], (uint16_t)g_command_parameters[2], set_irq_pin);
        api_send_response(0);
    }

    
}

void api_mq2_stop_tracking(void)
{
    clear_irq_pin();
    mq2_stop_tracking();
}