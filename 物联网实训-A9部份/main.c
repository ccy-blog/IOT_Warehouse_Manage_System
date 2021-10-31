#include "data_global.h"

extern void *pthread_transfer(void *arg);
extern void *pthread_analysis(void *arg);
extern void *pthread_refresh(void *arg);
extern void *pthread_client_request(void *arg);

pthread_mutex_t mutex_analysis;//分析线程的锁
pthread_cond_t cond_analysis; //分析线程唤醒条件
pthread_mutex_t mutex_refresh;//刷新线程的锁
pthread_cond_t cond_refresh; //刷新线程唤醒条件

int main(int argc, const char *argv[])
{
	pthread_t id_transfer;//接收线程ID 
	pthread_t id_analysis;//分析线程ID 
	pthread_t id_refresh;//刷新共享内存线程ID 
	pthread_t id_client_request;//接收控制命令请求

	//初始化锁和条件
	pthread_mutex_init(&mutex_analysis,NULL);
	pthread_cond_init(&cond_analysis,NULL);
	pthread_mutex_init(&mutex_refresh,NULL);
	pthread_cond_init(&cond_refresh,NULL);

	//创建一个M0接收数据线程 
	pthread_create(&id_transfer,NULL,pthread_transfer,NULL);
	//创建一个分析数据线程
	pthread_create(&id_analysis,NULL,pthread_analysis,NULL);
	//创建刷新共享内存线程
	pthread_create(&id_refresh,NULL,pthread_refresh,NULL);
	//创建接收控制命令的线程
	pthread_create(&id_client_request,NULL,pthread_client_request,NULL);
	
	//阻塞等待子线程结束
	pthread_join(id_transfer,NULL);//阻塞等待接收数据线程
	pthread_join(id_analysis,NULL);//阻塞等待分析数据线程
	pthread_join(id_refresh,NULL);//阻塞等待分析数据线程


	return 0;
}
