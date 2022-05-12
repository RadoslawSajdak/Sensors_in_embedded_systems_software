/***** Includes *****/
#include "timers.h"
#include "stdlib.h"

/***** Defines *****/
#define MAX_TIMERS                          10
#define MAX_SYSTICK_VALUE                   4294967295


typedef struct registered_callback
{
    timer_callback callback;
    uint32_t timeout;
    uint32_t ticks_at_start;
    bool repeat;
    struct registered_callback *next_element;
}registered_callback_s;

/***** Local variables *****/
static registered_callback_s *g_head = NULL;
static uint8_t g_registered_callbacks = 0;
static volatile uint32_t g_systick = 0;


/***** Global functions definitions *****/

hub_retcode_t timers_init(void)
{
    return (hub_retcode_t)SysTick_Config(SystemCoreClock/1000);
}

hub_retcode_t timers_add_timer(uint32_t timeout, timer_callback callback, bool repeat)
{
    if(timeout > MAX_SYSTICK_VALUE) return ARGUMENT_ERROR;
    if (g_registered_callbacks >= MAX_TIMERS ) return NO_TIMERS_ERROR;
    registered_callback_s *current = g_head;

    while (current != NULL)
    {
        if(current->callback == callback)
        {
            current->ticks_at_start = g_systick;
            current->timeout = timeout;
            current->repeat = repeat;

            return OK;
        }

        current = current->next_element;
    }

    //Not found, so add another
    current = g_head;
    

    if(current == NULL)
    {
        g_head = (registered_callback_s *)malloc(sizeof(registered_callback_s));
        g_head->callback = callback;
        g_head->timeout = timeout;
        g_head->ticks_at_start = g_systick;
        g_head->repeat = repeat;
        g_head->next_element = NULL;
    } else {

        while(current->next_element != NULL) current = current->next_element;

        current->next_element = (registered_callback_s *)malloc(sizeof(registered_callback_s));
        current->next_element->callback = callback;
        current->next_element->timeout = timeout;
        current->next_element->ticks_at_start = g_systick;
        g_head->repeat = repeat;
        current->next_element->next_element = NULL;
    }

    g_registered_callbacks++;

    return OK;
}

hub_retcode_t timers_stop_timer(timer_callback callback)
{
    if(callback == NULL) return ARGUMENT_ERROR;
    registered_callback_s *current = g_head, *temp = NULL;
    if(current->callback == callback)
    {
        temp = current->next_element;
        free(g_head);
        g_head = temp;
        g_registered_callbacks--;
        return OK;
    }
    while (current->next_element != NULL)
    {
        if(current->next_element->callback == callback)
        {
            temp = current->next_element;
            current->next_element = temp->next_element;
            free(temp);
            g_registered_callbacks--;
            return OK;
        }
        current = current->next_element;
    }
    return ARGUMENT_ERROR;
}

/***** Local functions definitions *****/



void SysTick_Handler(void)
{
    registered_callback_s *current = g_head;
    while (current != NULL)
    {
        if(g_systick - current->ticks_at_start > current->timeout)
        {
            current->callback();
            if(current->repeat) current->ticks_at_start = g_systick;
            else timers_stop_timer(current->callback);
        }
        current = current->next_element;
    }
    g_systick++;
}
