#include <stdio.h>
#include <string.h>

#include "LPC11xx.h"
#include "ssp.h"
#include "tools.h"
#include "oled2864.h"
#include "rfid.h"
#include "menu.h"
#include "gpio.h"
#include "myrfid.h"
#include "led_spk_fan.h"
#include "collect_data.h"
//extern volatile uint32_t cont;
//长度字、命令类型、密钥标志、块号、6字节密钥
//uint8_t tbuf[10] =  {0x0a, 0x21, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
uint8_t tbuf[10];
uint8_t key[10] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff,0xff, 0xff, 0xff, 0xff};
//uint8_t wbuf[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10};
//uint8_t wbuf[16] = {0x10,0x0f,0x0e,0x0d,0x0c,0x0b,0x0a,0x09,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00};
uint8_t wbuf[16] = {0x02,0x00,0x01,0x01,0x00,0x0b,0x00,0x18,0x00,0x10,0x0f,0x0c,0x0d,0x0e,0x05,0x07};

void Rfid_Init(void)
{
	SPI_Init(0, 8, 128);
	delay_ms(1);
	GPIOSetDir(PORT2, 8, 0);	  //拉低片选，使能芯片
//	RFID_CS(0);

	return;
}

//read rfid, save in rbuf, return value is the length
uint8_t Rfid_Read(uint8_t *rbuf,uint8_t choice)
{
	uint8_t	chksum;
	uint32_t	i, j, rnumb;

	uint8_t	tmp;

//	delay_ms(200); //原本有
	RFID_CS(0);

	switch(choice)
  {
	case 1:			   //读数据块（返回16字节的块内容）
		tbuf[0] = 0x0a;
		tbuf[1] = 0x21;
		tbuf[2] = 0x00;
		tbuf[3] = 0x01;
		for(tmp=0;tmp<6;tmp++)
		tbuf[4+tmp] =	key[0+tmp];//6字节密钥
		break;

	case 2:			  //写数据块（返回数据域为空的帧）
	  	tbuf[0] = 0x1a;
		tbuf[1] = 0x22;
		tbuf[2] = 0x00;
		tbuf[3] = 0x01;
		for(tmp=0;tmp<6;tmp++)
		tbuf[4+tmp] =	key[0+tmp];//6字节密钥
		
		for(tmp=0;tmp<16;tmp++)
		tbuf[10+tmp] = wbuf[0+tmp];//16字节数据
		break;

	case 3:			  //读头类型（返回8字节的模块型号）
	  	tbuf[0] = 0x02;
		tbuf[1] = 0x01;
	  	break;
	
	case 4:			  //读卡（返回4字节卡序列号）
	  	tbuf[0] = 0x02;
		tbuf[1] = 0x20;
	  	break;
	
	case 5:			   //读钱包（返回16字节的块内容）
		tbuf[0] = 0x0a;
		tbuf[1] = 0x24;
		tbuf[2] = 0x00;
		tbuf[3] = 0x01;
		for(tmp=0;tmp<6;tmp++)
		tbuf[4+tmp] =	key[0+tmp];//6字节密钥
		break;
	default:
	  	break;
  }

/*
	tbuf[0] = 0x0a;
	tbuf[1] = 0x21;
	tbuf[2] = 0x00;
	tbuf[3] = 0x01;
	for(tmp=0;tmp<6;tmp++)
	tbuf[4+tmp] =	key[0+tmp];//6字节密钥
*/

	chksum = RFID_CheckSum(tbuf);  //求出校验码,chksum=42(0x2A)
//	printf("%d\n",chksum);

	SPI_PutGet(0, 0xaa);		//发送0xaa
	delay_ms(1);

	i = 0;
	rbuf[i++] = SPI_PutGet(0, 0xbb);  //发送0xbb，并且接收数据0xAA
//	printf("%d\n",rbuf[0]);			  //rbuf[0]=0xAA
	delay_ms(1);

	for(j=0; j<tbuf[0]; j++)
	{
		rbuf[i++] = SPI_PutGet(0, tbuf[j]);	//i=1开始
    	delay_ms(1);
	}
	/*
	for(i=0;i<9;i++)
	{
		printf("%02X ",rbuf[i]);
	}
	printf("\n");
	*/
	rbuf[i] = SPI_PutGet(0, chksum); //i=9,chksum=42(0x2A)
//	printf("%02X\n",rbuf[9]);		 //rbuf={0x0a,0x21,0x00,0x01,0xff,0xff,0xff,0xff}
	i++;//原本没有					//i=10

	delay_ms(500);

	rbuf[0] = 1; //原本没有

	SPI_PutGet(0, 0);
	delay_ms(1);
  
	if(SPI_PutGet(0, 0) != 0xaa)   //不会执行
	{
    	SPI_Init(0, 8, 2);
    	return 0;
	}

	delay_ms(1);
  
	if(SPI_PutGet(0, 0) != 0xbb)  //不会执行
	{	//printf("hello");
    	SPI_Init(0, 8, 2);
    	return 0;
	}
	
	delay_ms(1);
/*
	if (tbuf[1]==0x21)
	{
		rnumb = 16+2+1;
	}
	else
	{
  		rnumb = 4+2+1;
	}
*/

switch(tbuf[1])
  {
    case 0x01:			  //读头类型（返回8字节的模块型号）
	  rnumb = 8 + 2 + 1;
	  break;

	case 0x20:			  //读卡（返回4字节卡序列号）
	  rnumb = 4 + 2 + 1;
	  break;

	case 0x21:			   //读数据块（返回16字节的块内容）
	  rnumb = 16 + 2 + 1;
	  break;

	case 0x22:			  //写数据块（返回数据域为空的帧）
	  rnumb = 2 + 1;
	  break;

	default:
	  rnumb = 4 + 2 + 1;
	  break;
  }

	for(j=0, i=0; j<rnumb; j++, i++)
	{
    	rbuf[i] = SPI_PutGet(0, 0);
		delay_ms(1);
	}
	//rbuf[2+16+1]={12 21 00 01 02 03 04 05 06 07 09 0A 0B 0C 0D 0E 0F 10 2B}
	RFID_CS(1);

	delay_ms(1);

	SPI_Init(0, 8, 2);

	return i;	
}


//operation of rfid
uint8_t Rfid_Operation(uint8_t snum, uint8_t *sbuf, uint8_t *rbuf)
{
	uint8_t	choice = 4;   //[choice:] 1:读卡中的块内容 2:写卡 3:读头类型 4:读卡系列号 5:读钱包

	uint8_t i,rlen=0,flen=0;
	char buf[64];
	

	Rfid_Init();
				
	rlen = Rfid_Read(rbuf,choice);		 //rlen=19,rbuf[19]={18,33,012345678910111213141516,43}	

	if(rlen == 3)
	{
		printf("write card ok\n");
		return rlen;
	}
			
	snprintf((char *)sbuf,64,"FrI%dR%d",snum,rbuf[2]); //snum=1,rbuf[2]= 0(卡号的第一位)
	

	for(i=3;i<rlen-1;i++)
	{
		snprintf(buf,64,"%d",rbuf[i]);	 //buf存的是卡号(第二位开始)
		strcat((char *)sbuf,buf);
	}
	
//	send_data(sbuf);
//	printf("%s\n",sbuf);	 		   //sbuf[27位]={仓库号+R+23位卡号}
	flen = strlen((char *)sbuf);			   //flen = 27	
	strcpy(buf,(char *)sbuf);
	snprintf((char *)sbuf,64,"S%d%sE",flen,buf);	   //flen=27（buf的长度）,buf[27位]={仓库号+R+23位卡号}

	return rlen;
}

void initSpeaker()
{
	LPC_IOCON->R_PIO1_1 = (LPC_IOCON->R_PIO1_1 & (~0x07)) | 0x01;
	GPIOSetDir(PORT1, 1, 1);			// Set PIO3_2 to output
}


void Speaker_OnOff(uint8_t value)
{
	 initSpeaker();
	//LPC_IOCON->R_PIO1_1 = (LPC_IOCON->R_PIO1_1 & (~0x07)) | 0x01;	//init speaker
	//GPIOSetDir(PORT1, 1, 1);			// Set PIO3_2 to output
	speaker_op(1);
	delay_ms(10);
	speaker_op(0);		// PIO3_2 output 1,	Speaker turn off
	

}

/*
void Speaker_On()
{
	initSpeaker();
	GPIOSetValue(PORT1, 1, 0);		//PIO3_2 output 0,	Speaker turn on 
	delay_ms(1);
	GPIOSetValue(PORT1, 1, 1);		// PIO3_2 output 1,	Speaker turn off
}
*/
 void Speaker_Error(uint8_t value)
{	int i;
	initSpeaker();
	for(i = 0;i < 5;i++)
	{
		GPIOSetValue(PORT1, 1, value);		//PIO3_2 output 0,	Speaker turn on 
//		delay_ms(10);
//		GPIOSetValue(PORT1, 1, 1);		// PIO3_2 output 1,	Speaker turn off
	}
}


//display the number on oled
void Rfid_Display(uint8_t len, uint8_t *rbuf)
{
	int i,x;
	char buf[8];

	if (0==len)
	{
		OLED_DisStrLine(2-1 + 5, 0, "                     ");
		OLED_DisStrLine(2-1 + 6, 0, "Fail");
	//	printf("fail") ;
		return;
	}
	else
	{
	for(i=0, x=0; i<len; i++)
    {
		if(x <= 108)
		{
		  snprintf(buf, 4, "%02X ", rbuf[i]);
		  OLED_DisChar(2-1 + 5, x, 0, buf[0]);
          x += 6;
          OLED_DisChar(2-1 + 5, x, 0, buf[1]);
          x += 6;
          OLED_DisChar(2-1 + 5, x, 0, buf[2]);
          x += 6;
        }
		OLED_DisStrLine(2-1 + 6, 0, "OK"); 
    }
	}
	return;
}
uint32_t Read_Rfidnum(uint8_t st_num,uint8_t *sbuf, uint8_t *rbuf)
{

        if(!GPIOGetValue(PORT2, 8))			//! 0(若有刷卡)
		{
			Rfid_Operation(st_num,sbuf,rbuf);
			delay_ms(100);
	   		
             return 1;
		}
        else
        {         
            return 0;
        } 
}
