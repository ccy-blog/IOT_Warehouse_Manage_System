#include "data_global.h"

extern char check[4];//外部引用声明
extern int fd;
extern pthread_mutex_t mutex_analysis;
extern pthread_cond_t cond_analysis;
extern pthread_cond_t cond_refresh;

e_info e_buf;//环境信息结构体
r_info r_buf;//货物信息结构体

void *pthread_analysis(void *arg)
{
	printf("pthread_analysis is OK!!\n");
	//对cheak进行分析 st:e 还是 str:r
	while(1)
	{//阻塞等待唤醒
		pthread_mutex_lock(&mutex_analysis);
		pthread_cond_wait(&cond_analysis,&mutex_analysis);
		pthread_mutex_unlock(&mutex_analysis);
		if(strncmp(check,"st:e",4) == 0)//说明是环境信息
		{
			//继续读取到环境信息结构体
			read(fd,&e_buf,sizeof(e_buf));
			printf("temp:%d.%d hum:%d.%d light:%d\n",
					e_buf.temp[0],e_buf.temp[1],
					e_buf.hum[0],e_buf.hum[1],
					e_buf.ill);
			//唤醒刷新共享内存线程
			pthread_cond_signal(&cond_refresh);
		}
		else if(strncmp(check,"st:r",4) == 0)//说明是货物信息
		{
			//继续读取到货物信息结构体
			read(fd,&r_buf,sizeof(r_buf));
		}
	}
}
