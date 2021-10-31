#ifndef __DATA_GLOBAL__H__
#define __DATA_GLOBAL__H__

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <termios.h>
#include <syscall.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <semaphore.h>

#define SHMMAX 32 //共享内存大小
#define STORAGE_NO 2 //仓库编号
#define STORAGE_NUM 5//仓库总数量
#define GOODS_NUM 10 //货物种类
#define N 32

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

//设置环境信息最大值
struct setEnv 
{
	int temMAX; //10-50
	int temMIN; //10-50
	int humMAX; //10-50
	int humMIN; //10-50
	int illMAX; //10-500
	int illMIN; //10-500
};
// st:e
// st:r

//接收M0发送过来的货物信息
typedef struct goods_info 
{
//	uint8_t head[3];	 //标识位st:
//	uint8_t	type;	     //数据类型
	uint8_t snum;		 //仓库编号
	uint8_t io;		     //进出仓库
	uint8_t goodsno;	 //货物编号
	uint8_t goodsnum;	 //货物数量

}r_info;


struct storage_goods_info
{
	unsigned char goods_type;
	unsigned int goods_count;
};

//仓库整体信息包括环境信息和货物信息
struct storage_info
{
	unsigned char storage_status;				// 0:open 1:close
	unsigned char led_status;
	unsigned char buzzer_status;
	unsigned char fan_status;
	unsigned char seg_status;
	signed char x;
	signed char y;
	signed char z;
	char samplingTime[20];
	float temperature;
	float temperatureMIN;
	float temperatureMAX;
	float humidity;
	float humidityMIN;
	float humidityMAX;
	float illumination;
	float illuminationMIN;
	float illuminationMAX;
	float battery;
	float adc;
	float adcMIN;
	struct storage_goods_info goods_info[GOODS_NUM];
};



struct env_info_clien_addr
{
	struct storage_info storage_no[STORAGE_NUM];//多个仓库
};

//A9---cgi的共享内存结构体
struct shm_addr
{
	char cgi_status;
	char qt_status;
	struct env_info_clien_addr rt_status;//仓库所有信息
};





typedef struct env_info //环境结构体类型定义
{
//	uint8_t head[3];	 //标识位st:
//	uint8_t type;		 //数据类型
	uint8_t snum;		 //仓库编号
	uint8_t temp[2];	 //温度	
	uint8_t hum[2];		 //湿度
	uint8_t x;			 //三轴信息
	uint8_t y;			 
	uint8_t z;			 
	uint32_t ill;		 //光照
	uint32_t bet;		 //电池电量
	uint32_t adc; 		 //电位器信息
}e_info;

//消息队列消息结构体
struct msg
{
	long type;
	long msgtype;
	unsigned char text[N];
};
#endif
