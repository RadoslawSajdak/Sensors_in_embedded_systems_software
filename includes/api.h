/****************************************
 *  This library is just an example.    *
 *  Here you should put simplified      *
 *  library description                 *
 *  Author:                             *
 *  Date:                               *
 *  Github profile:                     *
 ***************************************/
#ifndef __SENSOR_HUB_API_H__
#define __SENSOR_HUB_API_H__

/*---- EVERY COMMAND HAS AT+????? IN THE BEGGINING!!! -----*/
#define GET_TEMPERATURE                 "TEMPERATURE"
#define GET_PRESSURE                    "PRESSURE"

typedef enum{
    GET_TEMPERATURE_COMMAND = 0,
    GET_PRESSURE_COMMAND = 1
}api_user_commands_t;

void run_api(void);
#endif
