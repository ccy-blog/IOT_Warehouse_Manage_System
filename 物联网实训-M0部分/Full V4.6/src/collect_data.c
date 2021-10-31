#include <stdio.h>
#include <string.h>
#include "LPC11xx.h"
#include "collect_data.h"
#include "led_spk_fan.h"
#include "temp_hum.h"
#include "light.h"
#include "i2c.h"
#include "adc.h"
#include "acc.h"
#include "spi_uart.h"
#include "gpio.h"
#include "oled2864.h"

void collect_data(uint8_t *temp, uint8_t *hum,uint8_t *light)
{
		uint8_t i;
		char buf[16];
		uint32_t light_data;

		i = Read_Temp_Hum(temp,hum)	;
		I2CInit(I2CMASTER, 0);
	 	light_init();
		light_enable();
		light_setRange(LIGHT_RANGE_4000);
		
		light_data = light_read();
		light[0] = (light_data >> 8);
		light[1] = (light_data & 0xff);
	   
		if(i != 0)
		{
		//	printf("temp:%d.%d------hum:%d.%d------light:%d\r\n",temp[0],temp[1],hum[0],hum[1],light[0]);
			OLED_ClearScreen();
			snprintf(buf, 16, "Temp: %d.%d", temp[0], temp[1]);
			OLED_DisStrLine(1, 0, (uint8_t *)buf);
			snprintf(buf, 16, "Hum: %d.%d", hum[0], hum[1]);
			OLED_DisStrLine(2, 0, (uint8_t *)buf);
			snprintf(buf, 20, "light %d    ", light_data);
            OLED_DisStrLine(3, 0, (uint8_t *)buf);
		}
}

void send_data(uint8_t *data, uint8_t len)
{
    uint8_t i=0;
    for(i=0;i<len ;i++)
    {
//		UART0_PutChar(data[i]);
        ZigBee_PutChar(data[i]); //一个一个发送
    }
}

uint32_t adc_ret(uint8_t *adc)
{
	uint32_t	adv;
 	float		vol;
	char buf[16];

	ADCInit(ADC_CLK);
	adv = ADCRead(0);
	vol = adv*3.7/1023;
	adv = vol*10;
	snprintf(buf, 21, "Voltage: %d.%dV", adv/10, adv%10);
	adc[0] = adv/10;
	adc[1] = adv%10;
//	OLED_DisStrLine(4, 0, (uint8_t *)buf);

	return 1;			
}

void acc_ret(uint8_t *buffer)
{
  char buf[16];

  int32_t	xoff = 0;
  int32_t	yoff = 0;
  int32_t	zoff = 0;
  int8_t	x = 0;
  int8_t	y = 0;
  int8_t	z = 0;
  
  I2CInit(I2CMASTER, 0);

  acc_init();
 
  acc_read(&x, &y, &z);
  xoff = 0-x;
  yoff = 0-y;
  zoff = 0-z;

    acc_read(&x, &y, &z);
    x = x+xoff;
    y = y+yoff;
    z = z+zoff;
  		
    //sprintf(buffer,"X%dY%dZ%d", x, y, z);
	buffer[0] = x;
	buffer[1] = y;
	buffer[2] = z;
  	
    snprintf(buf, 20, "Acc x: %d  ", x);
    OLED_DisStrLine(5, 0, (uint8_t *)buf);
	//printf("\r\nAcc x: %d,  ", x);

	snprintf(buf, 20, "Acc y: %d  ", y);
    OLED_DisStrLine(6, 0, (uint8_t *)buf);
	//printf("Acc y: %d,  ", y);

	snprintf(buf, 20, "Acc z: %d  ", z);
    OLED_DisStrLine(7, 0, (uint8_t *)buf);


	//printf("Acc z: %d. ", z);
  //ZigBee_PutString((uint8_t *)buf);
 // printf("Acc x:%2d  y:%2d  z:%2d  \r\n", x, y, z);	
 
}


