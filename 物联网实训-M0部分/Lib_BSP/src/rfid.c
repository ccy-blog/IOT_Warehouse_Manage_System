/*******************************************************************************
* File:    rfid.c 
* Author:  FarSight Design Team
* Version: V1.00
* Date:    2011.06.21
* Brief:   RFID program
*******************************************************************************/ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"
#include "gpio.h"
#include "ssp.h"
#include "oled2864.h"
#include "key.h"
#include "tools.h"
#include "rfid.h"
#include "menu.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//const uint8_t RFID_READ_MOD_TYPE_01[2] = {0x02, 0x01};
////const uint8_t RFID_READ_TYPE_19[2] = {0x02, 0x19};
//const uint8_t RFID_READ_CARD_20[2] = {0x02, 0x20};
////const uint8_t RFID_READ_DATA_BLOCK_21[10] =  {0x0a, 0x21, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
//const uint8_t RFID_WRITE_DATA_BLOCK_22[26] = {0x1a, 0x22, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
//                                              0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
//										      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}; 
//
   const uint8_t RFID_READ_MOD_TYPE_01[2] = {0x02, 0x01};
const uint8_t RFID_READ_TYPE_19[2] = {0x02, 0x19};
const uint8_t RFID_READ_CARD_20[2] = {0x02, 0x20};
const uint8_t RFID_READ_DATA_BLOCK_21[10] =  {0x0a, 0x21, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; 
const uint8_t RFID_WRITE_DATA_BLOCK_22_0[26] = {0x1a, 0x22, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                              0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
										      0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00}; 
const uint8_t RFID_WRITE_DATA_BLOCK_22_1[26] = {0x1a, 0x22, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                              0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
										      0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f}; 
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : RFID_CheckSum
* Description    : Init RFID control.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RFID(struct sendGoods *sendsbuf)
{ 
  uint16_t	key;
  int	i, num = 0;
  char buf[10]= {0};
  uint8_t strbuf[9] = {0} ;
   static uint8_t ch;
   static uint8_t goodsno = 0;
   static uint8_t NameFlag = 1;		   					//结束循环标志位，用在物品选完后对数量（num）操作上
   static uint8_t WFlag	= 1;		   					//结束循环标志位，用在货物物品操作上
   static Menu_TypeDef	 menu1;
	
			  menu1.max_numb = 4;						 //货物的种类
				menu1.numb = 1;
				menu1.title = "Choice your good        ";
				menu1.item[0] = "1.Apple";
				menu1.item[1] = "2.Banana";
				menu1.item[2] = "3.Orange";
				menu1.item[3] = "4.Grape";
       GPIOSetDir(PORT2, 8, 0);
	 //  OLED_DisStrLine(1, 0, "RFID");
	     
	if((GPIOGetValue(PORT2, 8)&0x100) == 0)              // 得到P2_8引脚的电平，如果为电平有刷卡
	{
	 OLED_DisStrLine(1, 0, "RFID");
	 OLED_ClearScreen();
	 Dis_Menu(menu1);
	while(WFlag)										 // 进入循环，通过上、下键来选择货品名称
  	{	
    key = KEY_Read();
				
	switch(key){
		 case KEY_UP:
			  if(menu1.numb > 1)
				 menu1.numb --;
			  else
				  menu1.numb = menu1.max_numb;
					Dis_Menu(menu1);
						delay_ms(500);
					break;

		 case KEY_DOWN:	    
				if(menu1.numb < menu1.max_numb)
					  menu1.numb ++;
				else
					 menu1.numb = 1;
				   	Dis_Menu(menu1);
					 delay_ms(500);
						break;

		 case KEY_SEL: 									 //	选中货品名称后，进入数量选择
		   goodsno =  menu1.numb;//	sendsbuf.goodsno =  menu1.numb;				 //结构体货品编号赋值
		   						  					   				
		    OLED_ClearScreen();	
			OLED_DisStrLine(0, 0, "Input number");				
	 		OLED_DisStrLine(2, 0, "num : 0");
			delay_ms(500); 
			while(NameFlag){							 //进入循环，通过左、右键来实现num的增减、减少
				key = 0;
				key = KEY_Read();
				 delay_ms(100);
				switch(key){
				case KEY_LEFT:
			 		 num++;				
				     snprintf(buf, 10, "num : %d  ",num);
					 OLED_DisStrLine(2, 0, (uint8_t *)buf);
				   	 break;
			 	case KEY_RIGHT:	    
					 num-- ;				
				 	 snprintf(buf, 10, "num : %d  ",num);
				 	 OLED_DisStrLine(2, 0, (uint8_t *)buf);
				 	 break;
		 	    case KEY_SEL:							//按确定键后，通过num的值来判定进库/出库信息
		   		  	 if(num >= 0 ){
						ch = 'I';//	sendsbuf.io = 'I';				//num 不小于 0 则进库 结构体 sendsbuf.io值为I
					 }else{
						ch = 'O';//sendsbuf.io = 'O';				//num  小于 0 则出库  结构体 sendsbuf.io值为O
						num *= -1;
					 } 	
		   			 //sendsbuf.goodsnum =  num;			//|num|值为货物数量值
					 NameFlag = 0;						//标志位至 0 跳出循环
		   			 break;
				}			
			}
				
			sendsbuf->head[0] = 's';						 //结构体其他部分赋值
			sendsbuf->head[1] = 't';
			sendsbuf->head[2] = ':';
			sendsbuf->type = 'r';
			sendsbuf->snum =  sto_no ;
			sendsbuf->io = ch;
			sendsbuf->goodsno = goodsno; 
			sendsbuf->goodsnum =  num;	
			memcpy(strbuf,sendsbuf , 8);				 //将结构体信息拷贝到strbuf中，变为字符串形式
			for(i = 0; i < 8; i++){						 //按字节发送
			   		ZigBee_PutChar(strbuf[i]);
			 }
			 WFlag = 0;									 //标志位至 0 跳出循环
		      break;				
			} 		 
  	    }	
	    NameFlag = 1;									 //跳出循环后 标志位至 1 方便下次
		WFlag = 1;
  } 	  	
}
													
													
													
uint8_t RFID_CheckSum(uint8_t *databuf)
{ 
  uint8_t	numb, chksum;
  uint32_t	i;

  numb = databuf[0]-1;
  for(i=0; i<databuf[0]; i++)
  {
    if(databuf[i] == 0xaa)
	  numb ++;
  }

  chksum = databuf[0];
  while(numb)
  {
    chksum += databuf[numb];
    numb --;
  }

  return chksum;
}
/*****************************************************************************
** Function name:		SSP_Send
**
** Descriptions:		Send a block of data to the SSP port, the 
**						first parameter is the buffer pointer, the 2nd 
**						parameter is the block length.
**
** parameters:			port #, buffer pointer, and the block length
** Returned value:		None
** 
*****************************************************************************/

static unsigned short SPI0_Send( unsigned char  portNum, unsigned char  buf )
{
 	
  
	if ( portNum == 0 )
	{
	  GPIOSetValue( PORT2, 7, 0 ); 
	  while (( !(LPC_SSP0->SR & SSPSR_TNF)||(LPC_SSP0->SR & SSPSR_BSY)) != 0 );
	  LPC_SSP0->DR = buf;
	  while ( LPC_SSP0->SR & SSPSR_BSY );
	      /* Wait until the Busy bit is cleared */
     while((LPC_SSP0->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE);
   GPIOSetValue( PORT2, 7, 1);
  }
  return	  LPC_SSP0->DR;
}


/*******************************************************************************
* Function Name  : RFID_Operate
* Description    : Output segment to LED.
* Input          : - numb : display number.
* Output         : None
* Return         : None
*******************************************************************************/


uint8_t RFID_Operate(uint8_t *tbuf, uint8_t *rbuf)
{
  uint8_t	chksum;
  uint32_t	i, j, rnumb;

  SPI_Init(0, 8, 128);
  delay_ms(1);
  RFID_CS(0);

  chksum = RFID_CheckSum(tbuf);

   SPI0_Send(0, 0xaa);
  delay_ms(1);

  i = 0;
  rbuf[i] =  SPI0_Send(0, 0xbb);
  i ++;
  delay_ms(1);

  for(j=0; j<tbuf[0]; j++)
  {
    rbuf[i] =  SPI0_Send(0, tbuf[j]);
	i ++;
    delay_ms(1);
  }

  rbuf[i] =  SPI0_Send(0, chksum);
  i ++;

  delay_ms(500);

  rbuf[0] = 1;

  // SPI0_Send(0, 0);
  delay_ms(1);
  
  if( SPI0_Send(0, 0) != 0xaa)
  {
    SPI_Init(0, 8, 2);
    return 0;
  }

  delay_ms(1);
  
  if( SPI0_Send(0, 0) != 0xbb)
  {
    SPI_Init(0, 8, 2);
    return 0;
  }
	
  delay_ms(1);
	 
  switch(tbuf[1])
  {
    case 0x01:
	  rnumb = 8 + 2 + 1;
	  break;

	case 0x20:
	  rnumb = 4 + 2 + 1;
	  break;

	case 0x21:
	  rnumb = 16 + 2 + 1;
	  break;

	case 0x22:
	  rnumb = 2 + 1;
	  break;

	default:
	  rnumb = 4 + 2 + 1;
	  break;
  }

  for(j=0, i=0; j<rnumb; j++, i++)
  {
    rbuf[i] =  SPI0_Send(0, 0);
	delay_ms(1);
  }

  RFID_CS(1);

  delay_ms(1);

  SPI_Init(0, 8, 2);

  return i;
}

/*******************************************************************************
* Function Name  : ZigBee_Test
* Description    : ZigBee test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int RFID_Test(uint8_t *rbuf)
{
//  Menu_TypeDef	menu;
  uint16_t	 x;
  //uint8_t	rbuf[36] = {0}, chksum;
  uint8_t chksum;
  int i;
  uint8_t buffer[20] = {0};
  uint32_t j;
  char		buf[8];
  //uint32_t	i, j;


 /*menu.max_numb = 2;
  menu.numb = 1;
  menu.title = "RFID";
  menu.item[0] = "1.Read Card";
  menu.item[1] = "2.Write Card";
  
  Dis_Menu(menu);

  delay_ms(250);
*/
  // use port2_8 as input event, ZigBee interrupt.
  GPIOSetDir(PORT2, 8, 0);
  // port2_8 interrupt. edge, single trigger, falling edges.
  //GPIOSetInterrupt(PORT2, 8, 0, 0, 0);
  //GPIOIntEnable(PORT2, 8);
  
 // while(menu.numb)
 // {
	if(!GPIOGetValue(PORT2, 8))
    { 
	  delay_ms(200);

     j= RFID_Operate((uint8_t *)RFID_READ_DATA_BLOCK_21, buffer);

     // printf("\r\n");	
   
    for(i=0, x=0; i<j; i++)
      {
		if(x <= 108)
		{
		  snprintf(buf, 4, "%02X ", buffer[i]);
		//  OLED_DisChar(2-1 + 5, x, 0,buffer[0]);
          x += 6;
          //OLED_DisChar(2-1 + 5, x, 0,buffer[1]);
          x += 6;
          //OLED_DisChar(2-1 + 5, x, 0, buffer[2]);
          x += 6;
        }
     //   printf("%02X ", rbuf[i]);
      }	  
	 
      chksum = RFID_CheckSum(buffer);

      if(chksum == buffer[buffer[0]+1]||chksum == buffer[buffer[0]])
	  {
		
		for (i=0;i<4;i++)
		{
			rbuf[i] = buffer[2+i];
		}
		rbuf[i] = '\0'; 
   //     OLED_DisStrLine(2-1 + 6, 0, "Ok     ");
        //printf("\r\nOk ");
		//send_data(rbuf);
		return 1;
      }
      else
      {
	//	OLED_DisStrLine(2-1 + 5, 0, "                     ");
      //  OLED_DisStrLine(2-1 + 6, 0, "No Card");
      //  printf("\r\nFail ");
		return 0;
      }
	  }

	  return 0;
	  
	 // delay_ms(200); 

   // key = KEY_Read();

  /*  switch(key)
	{
	  case KEY_UP:
	    if(menu.numb > 1)
		  menu.numb --;
		else
		  menu.numb = menu.max_numb;
		Dis_Menu(menu);
		break;

	  case KEY_DOWN:	    
		if(menu.numb < menu.max_numb)
		  menu.numb ++;
		else
		  menu.numb = 1;
		Dis_Menu(menu);
		break;

	  case KEY_SEL:
	    switch(menu.numb)
		{
		  case 1:
		    printf("\r\nRFID Read");		    
			j = RFID_Operate((uint8_t *)RFID_READ_DATA_BLOCK_21, rbuf);
	  
	        printf("\r\n");	
            for(i=0, x=0; i<j; i++)
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
              printf("%02X ", rbuf[i]);
            }

            chksum = RFID_CheckSum(rbuf);

            if(chksum == rbuf[rbuf[0]])
	        {
	          OLED_DisStrLine(2-1 + 6, 0, "Ok     ");
              printf("\r\nOk ");
	        }
            else
	        {
			  OLED_DisStrLine(2-1 + 5, 0, "                     ");
	          OLED_DisStrLine(2-1 + 6, 0, "No Card");
              printf("\r\nFail ");
	        }
		    break;

		  case 2:
		    printf("\r\nRFID Write");		    
			j = RFID_Operate((uint8_t *)RFID_WRITE_DATA_BLOCK_22, rbuf);
	  
	        printf("\r\n");	
            for(i=0, x=0; i<j; i++)
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
              printf("%02X ", rbuf[i]);
            }

            chksum = RFID_CheckSum(rbuf);

            if(chksum == rbuf[rbuf[0]])
	        {
	          OLED_DisStrLine(2-1 + 6, 0, "Ok  ");
              printf("\r\nOk ");
	        }
            else
	        {
			  OLED_DisStrLine(2-1 + 5, 0, "                     ");
	          OLED_DisStrLine(2-1 + 6, 0, "Fail");
              printf("\r\nFail ");
	        }
		    break;

		  default:
		    break;
		}
	    break;
	  
	  case KEY_ESC:	    	    
        menu.numb = 0;		
	    break;
		
	  default:
	    break; 
	}

	

	if((key!=KEY_NONE) && (menu.numb!=0))
	{
	  delay_ms(250);
	} */
  //}
}

/*******************************************************************************
* Function Name  : RFID_Test
* Description    : RFID Test.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*
void RFID_Test(void)
{
  uint8_t	rbuf[36], chksum;
  uint32_t	i, j;

  OLED_ClearScreen();
  OLED_DisStrLine(0, 0, "RFID");
  printf("\r\nRFID\r\n");

  while(KEY_Read() != KEY_ESC)
  {
    if(KEY_Read() == KEY_SEL)
	{
      j = RFID_Operate((uint8_t *)RFID_READ_DATA_BLOCK_21, rbuf);
	  
	  printf("\r\n");	
      for(i=0; i<j; i++)
      {
        printf("%02X ", rbuf[i]);
      }

      chksum = RFID_CheckSum(rbuf);

      if(chksum == rbuf[rbuf[0]])
	  {
	    OLED_DisStrLine(2-1 + 6, 0, "ok  ");
        printf("ok");
	  }
      else
	  {
	    OLED_DisStrLine(2-1 + 6, 0, "fail");
        printf("fail");
	  }
	}
  }
}
*/
	

/**** End of File ****/


