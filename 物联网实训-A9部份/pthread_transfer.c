#include "data_global.h"

char check[4];//用来保存先读取出的四个字节 head[3] type
int fd;
extern pthread_mutex_t mutex_analysis;
extern pthread_cond_t cond_analysis;

//接收M0数据线程的函数
void *pthread_transfer(void *arg)
{
	printf("11111-pthread_transfer is OK\n");

	//文件IO是针对于文件的打开 读 写 关闭操作
	//1.打开文件 O_RDWR以read write 可读可写方式打开文件
	//e_info buf;//用来保存接收到的数据
	fd = open("/dev/ttyUSB0",O_RDWR);
	if(fd == -1)
	{
		perror("open failed");//打开失败
		exit(-1);//结束程序
	}
	printf("open ttyUSB0 OK!!\n");
	//初始化串口
	serial_init_115200(fd);
	while(1)
	{
		pthread_mutex_lock(&mutex_analysis);//加锁
		//2.读取文件的内容
		read(fd,check,4);//先读取出前四个字节
		//3.打印输出读取到的内容
		printf("%c%c%c%c\n",check[0],check[1],check[2],check[3]);
//		printf("type:%c snum:%d light:%d x:%d y:%d z:%d\n",buf.type,buf.snum,
//				buf.ill,buf.x,buf.y,buf.z);
		pthread_mutex_unlock(&mutex_analysis);//解锁
		//条件唤醒分析数据线程
		pthread_cond_signal(&cond_analysis);
		sleep(1);
	}
	//4.关闭文件 
	close(fd);
}
