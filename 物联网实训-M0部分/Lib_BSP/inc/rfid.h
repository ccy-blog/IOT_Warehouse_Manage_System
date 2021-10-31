/*******************************************************************************
* File Name          : rfid.h
* Author             : Farsight Design Team
* Version            : V1.00
* Date               : 2011.06.20
* Description        : RFID program head
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RFID_H
#define __RFID_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "LPC11xx.h"

/* Exported types ------------------------------------------------------------*/
#define RFID_CS(x)		GPIOSetValue(PORT2, 7, x)
#define RFID_CLK(x)		GPIOSetValue(PORT0, 6, x)
#define RFID_MISO(x)	GPIOSetValue(PORT0, 8, x)
#define RFID_MOSI(x)	GPIOSetValue(PORT0, 9, x)
#define sto_no 2 //2号仓库环境信息
	 
struct sendGoods
{
	uint8_t head[3];	 //标识位st:
	uint8_t	type;	     //数据类型
	uint8_t snum;		 //仓库编号
	uint8_t io;		     //进出仓库 'I'  'O'
	uint8_t goodsno;	 //商品编号1
	uint8_t goodsnum;	 //商品数量5
};
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint8_t RFID_CheckSum(uint8_t *databuf);
uint8_t RFID_Operate(uint8_t *tbuf, uint8_t *rbuf);
int RFID_Test(uint8_t *databuf);
void RFID(struct sendGoods *sendsbuf);

/* External variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __RFID_H */

/**** End of File ****/
