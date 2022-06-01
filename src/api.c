/***** Includes *****/
#include "api.h"
#include <stdio.h>
#include <string.h>
#include "debug_uart.h"
#include "api_uart.h"
#include "tools.h"
#include "timers.h"
#include "api_callbacks.h"


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
    MAX_COMMAND_CHOICE = 4
};


typedef void (*api_callback)(void);


typedef struct
{
    char command[20];
    api_callback callback;
}command_handlers_s;

/***** Local Functions Definitions *****/
hub_retcode_t           wait_for_message(char *message);
hub_retcode_t           parse_at_command(void);
static void             api_msg_timeout_handler(void);
static uint32_t         str_to_u32(char *num_as_string);


/***** Local Variables *****/
static bool                             g_msg_timeout = false;
static char                             g_last_msg[MAX_MSG_LEN] = {0};
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
                                                                                    {.command = "STOPTRACKING", .callback = api_mq2_stop_tracking},
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
    uint32_t len = 0;
    char buffer[MESSAGE_BUFFER] = {0};
    char *args_begin = NULL;
    while (1)
    {
        if(counter == MESSAGE_BUFFER) for(; counter > 0; counter--) buffer[counter] = 0;
        if( TIMEOUT_ERROR != api_uart_rx((uint8_t *)buffer + counter, 1))
            if(buffer[counter++] == '\r')
            {
                timers_add_timer(WAIT_FOR_MSG_TIMEOUT_MS, api_msg_timeout_handler, false);
                if( 0 == strncmp(buffer, message, strlen(message)))
                {
                    args_begin = memchr(buffer, ',', strlen(buffer));
                    if(NULL == args_begin)
                        strncpy(g_last_msg, buffer + 3, strlen(buffer));
                    else
                    {
                        len = (uint32_t)(args_begin - buffer - 1);
                        strncpy(g_last_msg, buffer + 3, len);
                    }
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

    for(uint8_t i = 0U; i < digits; i++)
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
