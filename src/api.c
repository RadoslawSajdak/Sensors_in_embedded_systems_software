/***** Includes *****/
#include "api.h"
#include "debug_uart.h"
#include "api_uart.h"
#include "tools.h"
#include "string.h"
#include "timers.h"


/***** Defines *****/
#define MAX_MSG_LEN                     100
#define MESSAGE_BUFFER                  10 * MAX_MSG_LEN
#define WAIT_FOR_MSG_TIMEOUT_MS         1000

/***** Local Variables *****/
static bool                             g_msg_timeout = false;
static char                             g_last_msg[MAX_MSG_LEN] = {0};

/***** Local Functions Definitions *****/
hub_retcode_t           wait_for_message(char *message);
api_user_commands_t     parse_at_command(void);
void api_msg_timeout_handler(void);


/***** Public Functions *****/
void run_api(void)
{
    //uint8_t message [100] = {0};
    api_uart_init(115200);
    while(1)
    {
        while( OK != wait_for_message("AT+"));
        debug_uart_printf("GOT it! %s\n", g_last_msg);
        switch (parse_at_command())
        {
        case GET_TEMPERATURE_COMMAND:
            debug_uart_printf("Asked for temperature\n");
            break;
        case GET_PRESSURE_COMMAND:
            debug_uart_printf("Asked for pressure\n");
            break;
        
        default:
            break;
        }

        //HAL_Delay(500);

    }
}




/***** Local Functions *****/

hub_retcode_t wait_for_message(char *message)
{
    uint16_t counter = 0U;
    char buffer[MESSAGE_BUFFER] = {0};
    g_msg_timeout = false;
    timers_add_timer(WAIT_FOR_MSG_TIMEOUT_MS, api_msg_timeout_handler, false);
    while (!g_msg_timeout)
    {
        if(counter == MESSAGE_BUFFER) for(; counter > 0; counter--) buffer[counter] = 0;
        if( OK == api_uart_rx((uint8_t *)buffer + counter, 1))
            if(buffer[counter++] == '\r')
            {
                if( 0 == strncmp(buffer, message, strlen(message)))
                {
                    strncpy(g_last_msg, buffer + 3, strlen(buffer));   
                    return OK;
                }
                else{
                    memset(buffer, 0, counter);
                    counter = 0;
                }
            }
    }
    return TIMEOUT_ERROR;
}

api_user_commands_t parse_at_command(void)
{
    if( 0 == strncmp(g_last_msg, GET_TEMPERATURE, strlen(GET_TEMPERATURE))) return GET_TEMPERATURE_COMMAND;
    if( 0 == strncmp(g_last_msg, GET_PRESSURE, strlen(GET_PRESSURE))) return GET_PRESSURE_COMMAND;

    return ARGUMENT_ERROR;
}

void api_msg_timeout_handler(void)
{
    g_msg_timeout = true;
}