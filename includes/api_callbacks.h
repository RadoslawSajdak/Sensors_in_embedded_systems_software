#ifndef __API_CALLBACKS_H__
#define __API_CALLBACKS_H__

#include "bmp280.h"
#include "mq2.h"
#include "sts3x_dis.h"

#define MAX_COMMAND_PARAMETERS          5

typedef enum{
    DEFAULT_COMMANDS = 0,
    BMP_COMMANDS = 1,
    STS_COMMANDS = 2,
    MQ_COMMANDS = 3
}sensor_choice_t;

extern sensor_choice_t                  g_chosen_sensor;
extern uint32_t                         g_command_parameters[MAX_COMMAND_PARAMETERS];
extern uint8_t                          g_command_param_counter;
/**** Rapers definitions *****/
hub_retcode_t api_send_response(const uint32_t value);
void api_menu_set_bmp(void);
void api_menu_set_sts(void);
void api_menu_set_mq(void);
void api_menu_set_default(void);

void api_bmp_get_temp(void);
void api_bmp_get_pressure(void);
void api_bmp_soft_reset(void);
void api_bmp_set_config(void);
void api_bmp_get_ready(void);

void api_sts_get_temp(void);
void api_sts_set_repeatability_low(void);
void api_sts_set_repeatability_medium(void);
void api_sts_set_repeatability_high(void);
void api_sts_set_heater_on(void);
void api_sts_set_heater_off(void);
void api_sts_soft_reset(void);

void api_mq2_get_sample(void);
void api_mq2_get_LPG(void);
void api_mq2_get_CO(void);
void api_mq2_get_SMOKE(void);
void api_mq2_set_tracking(void);
void api_mq2_stop_tracking(void);

#endif