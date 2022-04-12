#include "stdint.h"
#include "example.h"
#include "stdio.h"
static I2C_HandleTypeDef			g_i2c_dev;

int main(void)
{
	uint8_t temperature = 0;
	sensor_init(&g_i2c_dev);

	while(1)
	{
		if(SUCCESS != sensor_get_byte(&temperature)) printf("Couldn't read data!\n");
		else printf("Received temperature %u\n", temperature);
		sleep(SEC_TO_MSEC(10));
	}
}
