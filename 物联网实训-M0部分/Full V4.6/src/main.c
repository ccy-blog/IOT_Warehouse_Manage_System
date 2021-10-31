/*******************************************************************************
* File:    main.c 
* Author:  Farsight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   Main program body
********************************************************************************
********************************************************************************
* History:
* 2011.05.10:  V1.00		   initial version
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "lpc11xx.h"
#include "clkconfig.h"
#include "gpio.h"
#include "uart.h"
#include "timer32.h"
#include "ssp.h"
#include "i2c.h"
#include "wdt.h"
#include "adc.h"
#include "rom_drivers.h"

#include "seg7led.h"
#include "oled2864.h" 
#include "light.h"
#include "acc.h"
#include "key.h"
#include "rfid.h"
#include "tools.h"
#include "spi_uart.h"
#include "temp_hum.h"
#include "collect_data.h"
#include "led_spk_fan.h"
#include "myrfid.h"
#include "menu.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* External global variables -------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//__align(4) uint32_t	iap_test=0;
//char buf[32];

/* External functions --------------------------------------------------------*/

extern void LED_Test(void);
extern void Speaker_Test(void);
extern void Fan_Test(void);
extern void ADC_Test(void);
extern void CAN_Test_ROM(void);
extern void CAN_Test(void);
extern volatile uint32_t period ;


#define  ID_NUM 2
#define ntype 1
#define itype 2
#define rtype 4
#define transhead    	0xaa
//学生自己写
#define LED_ON       	0xa1      
#define LED_OFF	     	0xa0
#define FAN_ON	     	0x81
#define FAN_OFF		 		0x80
#define LED7_ON		 		0xb8
#define LED7_OFF	 		0xbf
#define SPEAKER_ON	 	0x91
#define SPEAKER_OFF	 	0x90

//#define  ID_NUM 2
//#define ntype 1
//#define itype 2
//#define rtype 4
//#define transhead    0xaa
//#define LED_ON       '0'      
//#define LED_OFF	     '1'
//#define FAN_ON	     '2'
//#define FAN_OFF		 '3'
//#define LED7_ON		 '4'
//#define LED7_OFF	 '5'
//#define SPEAKER_ON	 '6'
//#define SPEAKER_OFF	 '7'
//#define ANSWER		'8'


struct sendEnv
{
	uint8_t head[3];	 //标识位st:
	uint8_t type;		 //数据类型e
	uint8_t snum;		 //仓库编号
	uint8_t temp[2];	 //温度
	uint8_t hum[2];		 //湿度
	uint8_t x;			 //三轴信息
	uint8_t y;			 
	uint8_t z;			 
	uint32_t ill;		 //光照
	uint32_t bet;		 //电池电量
	uint32_t adc; 		 //电位器信息
};
 	  




volatile uint32_t counter1 = 0;
volatile uint32_t counter2 = 0;
static uint8_t	cnt=0;



/****************************************************************************/

void SystemInitStart(void);
void CollectData(struct sendEnv *envMsg);
void M0SendToA9(struct sendEnv *envMsg);
void WaitTime(void);
void M0RecvFromA9(void);
void SysTick_Handler(void);

int main(void)
{	  	
	struct sendEnv envMsg;
	struct sendGoods sendsbuf;
	//初始化
	SystemInitStart();
//	WaitTime();
	while(1)
	{
		//M0发送----->A9
			RFID(&sendsbuf);
			if(counter1 > 2)
			{
				CollectData(&envMsg);//采集数据存入结构体中
				M0SendToA9(&envMsg);
				counter1 = 0;
			}
		//M0接收<-----A9
			M0RecvFromA9();
	}
}


void M0RecvFromA9()
{
	uint8_t buf;
	if(ZigBee_GetChar(&buf))
	{
				switch(buf)
				{
					case LED_ON: //开LED1
						GPIOSetValue(PORT3, 0, 0);
						break;
					case LED_OFF://关LED1
						GPIOSetValue(PORT3, 0, 1);
						break;
					case SPEAKER_ON:
						speaker_op(1);
						break;
					case SPEAKER_OFF:
						speaker_op(0);
						break;
					case LED7_ON:
						Seg7Led_Put(8);	
						break;
					case LED7_OFF:
						Seg7Led_Put(' ');
						break;
					case FAN_ON:
						GPIOSetValue(PORT0, 2, 0);
						break;
					case FAN_OFF:
						GPIOSetValue(PORT0, 2, 1);
						break;
					
				}
		}
}


void M0SendToA9(struct sendEnv *envMsg)
{
		uint8_t i = 0;
		uint8_t buf[24] = { 0 };
		envMsg->head[0] = 's';			 //仓库信息结构体赋值
		envMsg->head[1] = 't';
		envMsg->head[2] = ':';
		envMsg->type = 'e';
		envMsg->snum = sto_no;
		memset(buf,24,0);
		memcpy(buf,envMsg,24);
		for(i = 0; i < 24; i++) //一次发送一个字节
		{
			ZigBee_PutChar(buf[i]);
		}
//		delay_ms(500);
}

//倒计时界面
void WaitTime()
{
	char buf[20] = {0};
	int i = 0;
	for(i = 10;i >= 0;i--)
	{
  	  snprintf(buf,20,"time is %d ",i);
			OLED_ClearScreen();
      OLED_DisStrLine(0, 0, "Waiting for 10s ") ;
			OLED_DisStrLine(2, 0, (uint8_t *)buf) ;
			delay_ms(1000);
	}
}

//采集数据
void CollectData(struct sendEnv *envMsg)
{
	Temp_Hum_Collect(envMsg->temp, envMsg->hum);//采集温度
	Axis3_Collect(&envMsg->x,&envMsg->y,&envMsg->z);//采集三轴
	Light_Collect(&envMsg->ill);//采集光照值
	ADC_Collect(&envMsg->adc);
	Bet_Collect(&envMsg->bet);
//	delay_ms(300);
}

//初始化
void SystemInitStart()
{
		SystemInit();
  	GPIOInit();						// Enables clock for GPIO
  	CLKOUT_Setup(CLKOUTCLK_SRC_MAIN_CLK);	//set clock source :main clock
  	LPC_IOCON->PIO0_1 &= ~0x07;	
		LPC_IOCON->PIO0_1 |= 0x01;		/* CLK OUT */

		// LED
		GPIOSetDir(PORT3, 0, 1);			// Set PIO3_0 to output
		GPIOSetValue(PORT3, 0, 1);		// PIO3_0 output 1, close LED1

		GPIOSetDir(PORT3, 1, 1);			// Set PIO3_1 to output
		GPIOSetValue(PORT3, 1, 1);		// PIO1_1 output 1, close LED2

    // Fan
    GPIOSetDir(PORT0, 2, 1);			// Set PIO0_2 to output    
    GPIOSetValue(PORT0, 2, 1);		// PIO0_2 output 1, close FAN

    // Speaker
    LPC_IOCON->R_PIO1_1 &= ~0x07;
    LPC_IOCON->R_PIO1_1 |= 0x01;
    GPIOSetDir(PORT1, 1, 1);			// Set PIO3_2 to output
  	 
    // 7Segment LED
    Seg7Led_Init();
    Seg7Led_Put(' ');					// Output ' '

    SysTick_Config(48000);

    SPI_IOConfig(0);
    SPI_Init(0, 8, 2);
    OLED_Init_I();

    UARTInit(115200); 
    NVIC_SetPriority(UART_IRQn, 2);

    printf("\r\n\r\nFS_11C14 Main Menu\r\n");

    delay_ms(250);
    SPI_IOConfig(1);
    SPI_Init(1, 8, 2);

    SPI752_Init(1, 115200);
		WaitTime();
}	


void SysTick_Handler(void)
{
  static uint32_t	Tick=0;
  
  Tick ++;
  if(Tick == 500)
  {
    //GPIOSetValue(PORT3, 1, 0);		// PIO3_1 output 1, turn on LED2

//	Seg7Led_Put(cnt);

	cnt ++;
	cnt &= 0x1f;
  }
  else if(Tick >= 1000)
  {

  	counter1++;
	counter2++;
    //GPIOSetValue(PORT3, 1, 1);		// PIO3_1 output 1, turn off LED2

	Tick = 0;
  }
}
///////////////////////////////////////////////////////////

