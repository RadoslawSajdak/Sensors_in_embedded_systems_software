/***** Includes *****/
#include "api.h"
#include <stdio.h>
#include <string.h>
#include "debug_uart.h"
#include "api_uart.h"
#include "tools.h"
#include "string.h"
#include "timers.h"
#include "bmp280.h"
#include "sts3x_dis.h"
#include "mq2.h"


/***** Defines *****/
enum {
    LPG_MAX_VALUE = 100,
    CO_MAX_VALUE = 100,
    SMOKE_MAX_VALUE = 100,
    MAX_MSG_LEN = 100,
    MAX_COMMAND_LEN = 20,
    MAX_COMMAND_NUMBER = 20,
    MESSAGE_BUFFER = 10 * MAX_MSG_LEN,
    WAIT_FOR_MSG_TIMEOUT_MS = 1000,
    MAX_COMMAND_CHOICE = 4,
    MAX_COMMAND_PARAMETERS = 5
};


typedef void (*api_callback)(void);

typedef enum{
    DEFAULT_COMMANDS = 0,
    BMP_COMMANDS = 1,
    STS_COMMANDS = 2,
    MQ_COMMANDS = 3
}sensor_choice_t;



typedef struct
{
    char command[20];
    api_callback callback;
}command_handlers_s;

/***** Local Functions Definitions *****/
hub_retcode_t           api_send_response(const uint32_t value);
hub_retcode_t           wait_for_message(char *message);
hub_retcode_t           parse_at_command(void);
static void             api_msg_timeout_handler(void);
static uint32_t         str_to_u32(char *num_as_string);

/**** Rapers definitions *****/
static void api_menu_set_bmp(void);
static void api_menu_set_sts(void);
static void api_menu_set_mq(void);
static void api_menu_set_default(void);
static void api_bmp_get_data(void);
static void api_sts_get_temp(void);
static void api_sts_set_repeatability_low(void);
static void api_sts_set_repeatability_medium(void);
static void api_sts_set_repeatability_high(void);
static void api_sts_set_heater_on(void);
static void api_sts_set_heater_off(void);
static void api_sts_soft_reset(void);
static void api_mq2_get_sample(void);
static void api_mq2_get_LPG(void);
static void api_mq2_get_CO(void);
static void api_mq2_get_SMOKE(void);
static void api_mq2_set_tracking(void);
static void api_mq2_clear_tracking(void);


static void set_irq_pin(void)
{
    HAL_GPIO_WritePin(IRQ_GPIO, IRQ_PIN, 0);
}
static void clear_irq_pin(void)
{
    HAL_GPIO_WritePin(IRQ_GPIO, IRQ_PIN, 1);
}


/***** Local Variables *****/
static bool                             g_msg_timeout = false;
static char                             g_last_msg[MAX_MSG_LEN] = {0};
static uint32_t                         g_command_parameters[MAX_COMMAND_PARAMETERS];
static uint8_t                          g_command_param_counter = 0;
static bmp280_data_s                    g_bmp_data = {0};
static uint32_t                         g_sts32_val = 0;
static measurement_config_t             g_sts32_config = REPEATABILITY_LOW;
static sensor_choice_t                  g_chosen_sensor = DEFAULT_COMMANDS;
static const command_handlers_s         __menu_command_list[MAX_COMMAND_NUMBER] = { {.command = "BMP", .callback = api_menu_set_bmp},
                                                                                    {.command = "STS", .callback = api_menu_set_sts},
                                                                                    {.command = "MQ", .callback = api_menu_set_mq},
                                                                                    {.command = "BACK", .callback = api_menu_set_default}};

static const command_handlers_s         __bmp_command_list[MAX_COMMAND_NUMBER] = {  {.command = "GETDATA", .callback = api_bmp_get_data},
                                                                                    {.command = "BACK", .callback = api_menu_set_default}};

static const command_handlers_s         __sts_command_list[MAX_COMMAND_NUMBER] = {  {.command = "GETDATA", .callback = api_sts_get_temp},
                                                                                    {.command = "SETREPL", .callback = api_sts_set_repeatability_low},
                                                                                    {.command = "SETREPM", .callback = api_sts_set_repeatability_medium},
                                                                                    {.command = "SETREPH", .callback = api_sts_set_repeatability_high},
                                                                                    {.command = "SETHEATON", .callback = api_sts_set_heater_on},
                                                                                    {.command = "SETHEATOFF", .callback = api_sts_set_heater_off},
                                                                                    {.command = "SRESET", .callback = api_sts_soft_reset},
                                                                                    {.command = "BACK", .callback = api_menu_set_default}};

static const command_handlers_s         __mq_command_list[MAX_COMMAND_NUMBER] = {   {.command = "GETSAMPLE", .callback = api_mq2_get_sample},
                                                                                    {.command = "GETLPG", .callback = api_mq2_get_LPG},
                                                                                    {.command = "GETCO", .callback = api_mq2_get_CO},
                                                                                    {.command = "GETSMOKE", .callback = api_mq2_get_SMOKE},
                                                                                    {.command = "SETTRACKING", .callback = api_mq2_set_tracking},
                                                                                    {.command = "STOPTRACKING", .callback = api_mq2_clear_tracking},
                                                                                    {.command = "BACK", .callback = api_menu_set_default}};;
static const command_handlers_s         *__sensor_choice[MAX_COMMAND_CHOICE] = {__menu_command_list, __bmp_command_list, __sts_command_list, __mq_command_list};

/***** Public Functions *****/

void run_api(void)
{
    api_uart_init(115200);
    while(1)
    {
        while( OK != wait_for_message("AT+"));
        if(g_last_msg != NULL)
        {
            debug_uart_printf("GOT a message! %s\n", g_last_msg);
            parse_at_command();
        }
    }
}




/***** Local Functions *****/
hub_retcode_t api_send_response(const uint32_t value)
{
    char msg[20] = {0};
    sprintf(msg, "%lu", value);
    return api_uart_tx((uint8_t *)msg, strlen(msg));
}

void parse_parameters(char *msg)
{
    char *next = msg;
    char *ptr = NULL;
    char param_tmp[4] = {0};
    uint32_t len = 0;
    g_command_param_counter = 0;

    for(size_t i = 0; i < 10; i++)
    {
        ptr = memchr(next, ',', strlen(next));
        if(NULL != ptr)
        {
            next = memchr(ptr + 1, ',', strlen(ptr));
            if(NULL != next)
            {
                len = (uint32_t)(next - ptr - 1);
                strncpy(param_tmp, ptr + 1, len);
            } else {
            if(strlen(ptr) > 0) strncpy(param_tmp, ptr + 1, strlen(ptr) - 2); //removing \r and null from last message
            }
        }
        else break;
        g_command_parameters[g_command_param_counter++] = str_to_u32(param_tmp);
        memset(param_tmp, 0, sizeof(param_tmp));

        if(NULL == next || g_command_param_counter >= MAX_COMMAND_PARAMETERS) break;
    }
}

hub_retcode_t wait_for_message(char *message)
{
    uint16_t counter = 0U;
    char buffer[MESSAGE_BUFFER] = {0};
    while (1)
    {
        if(counter == MESSAGE_BUFFER) for(; counter > 0; counter--) buffer[counter] = 0;
        if( TIMEOUT_ERROR != api_uart_rx((uint8_t *)buffer + counter, 1))
            if(buffer[counter++] == '\r')
            {
                timers_add_timer(WAIT_FOR_MSG_TIMEOUT_MS, api_msg_timeout_handler, false);
                if( 0 == strncmp(buffer, message, strlen(message)))
                {
                    strncpy(g_last_msg, buffer + 3, strlen(message));
                    parse_parameters(buffer + 3);
                    return OK;
                }
                else{
                    api_uart_tx((uint8_t *)"ERROR\r", 6);
                    memset(buffer, 0, counter);
                    memset(g_last_msg, 0, sizeof(g_last_msg));
                    counter = 0;
                }
            }
    }
    memset(g_last_msg, 0, sizeof(g_last_msg));
    return TIMEOUT_ERROR;
}

hub_retcode_t parse_at_command(void)
{
    const command_handlers_s *current_array = __sensor_choice[g_chosen_sensor];

    for(size_t i = 0; i < MAX_COMMAND_NUMBER && strlen(current_array[i].command) > 0; i++)
    {
        if(0 == strncmp(g_last_msg, current_array[i].command, strlen(current_array[i].command)))
        {
            api_uart_tx((uint8_t *)"OK\r", 3); 
            current_array[i].callback();
            return OK;
        }
    }
    api_uart_tx((uint8_t *)"UNKNOWN\r", 8); 

    return ARGUMENT_ERROR;
}

static uint32_t str_to_u32(char *num_as_string)
{
    uint8_t digits = strlen(num_as_string);
    uint32_t number = 0U;
    number += num_as_string[0] - 48;

    for(uint8_t i = 1U; i < digits; i++)
    {
        number *= 10;
        number += (num_as_string[i] - 48);
    }
    return number;
}

static void api_msg_timeout_handler(void)
{
    g_msg_timeout = true;
}


/***** Rapers *****/
static void api_menu_set_bmp(void)
{
    g_chosen_sensor = BMP_COMMANDS;
}
static void api_menu_set_sts(void)
{
    g_chosen_sensor = STS_COMMANDS;
}
static void api_menu_set_mq(void)
{
    g_chosen_sensor = MQ_COMMANDS;
}
static void api_menu_set_default(void)
{
    g_chosen_sensor = DEFAULT_COMMANDS;
}

static void api_bmp_get_data(void)
{
    memset(&g_bmp_data, 0, sizeof(bmp280_data_s));
    bmp280_get_data(&g_bmp_data);
    api_send_response(g_bmp_data.temperature);
}

static void api_sts_get_temp(void)
{
    g_sts32_val = 0U;
    sts3x_get_temperature(&g_sts32_val, g_sts32_config);
    api_send_response(g_sts32_val);
}

static void api_sts_set_repeatability_low(void)
{
    g_sts32_config = REPEATABILITY_LOW;
}

static void api_sts_set_repeatability_medium(void)
{
    g_sts32_config = REPEATABILITY_MEDIUM;
}

static void api_sts_set_repeatability_high(void)
{
    g_sts32_config = REPEATABILITY_HIGH;    
}

static void api_sts_set_heater_on(void)
{
    sts3x_set_heater_status(STS3X_HEATER_ENABLE);
}

static void api_sts_set_heater_off(void)
{
    sts3x_set_heater_status(STS3X_HEATER_DISABLE);
}

static void api_sts_soft_reset(void)
{
    sts3x_soft_reset();
}

static void api_mq2_get_sample(void)
{
    uint16_t temp = mq2_get_sample();
    clear_irq_pin();
    api_send_response((uint32_t)temp);
}

static void api_mq2_get_LPG(void)
{
    uint16_t temp = mq2_get_ppm(LPG_MQ2);
    clear_irq_pin();
    api_send_response((uint32_t)temp);
}

static void api_mq2_get_CO(void)
{
    uint16_t temp = mq2_get_ppm(CO_MQ2);
    clear_irq_pin();
    api_send_response((uint32_t)temp);
}

static void api_mq2_get_SMOKE(void)
{
    uint16_t temp = mq2_get_ppm(SMOKE_MQ2);
    clear_irq_pin();
    api_send_response((uint32_t)temp);
}

static void api_mq2_set_tracking(void)
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

static void api_mq2_clear_tracking(void)
{
    clear_irq_pin();
    mq2_stop_tracking();
}