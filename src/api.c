#include "api.h"
#include "debug_uart.h"
#include "api_uart.h"
#include "tools.h"
#include "string.h"

#define MESSAGE_BUFFER                  1000


hub_retcode_t api_wait_for_message(char *message);


// Main loop
void run_api(void)
{
    //uint8_t message [100] = {0};
    api_uart_init(115200);
    while(1)
    {
        if( OK == api_wait_for_message("AT+HELLO"))
            debug_uart_printf("GOT it! \n");
        else
            debug_uart_printf("ERROR\n");
        //HAL_Delay(500);

    }
}

hub_retcode_t api_wait_for_message(char *message)
{
    uint16_t counter = 0U;
    char buffer[MESSAGE_BUFFER];

    while (1) //TODO: timer
    {
        if(counter == MESSAGE_BUFFER) for(; counter > 0; counter--) buffer[counter] = 0;
        if( OK == api_uart_rx((uint8_t *)buffer + counter, 1))
            if(buffer[counter++] == '\r')
            {
                if( 0 == strncmp(buffer, message, strlen(message))) return OK;
                else counter = 0;
            }
    }
    return TIMEOUT_ERROR;
}
