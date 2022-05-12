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
hub_retcode_t api_wait_for_message(char *message);
void api_msg_timeout_handler(void);


/***** Public Functions *****/
void run_api(void)
{
    //uint8_t message [100] = {0};
    api_uart_init(115200);
    while(1)
    {
        while( OK != api_wait_for_message("AT+"));

        debug_uart_printf("GOT it! %s\n", g_last_msg);

        //HAL_Delay(500);

    }
}




/***** Local Functions *****/

hub_retcode_t api_wait_for_message(char *message)
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

void api_msg_timeout_handler(void)
{
    g_msg_timeout = true;
}