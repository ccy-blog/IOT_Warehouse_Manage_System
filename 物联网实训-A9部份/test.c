#include <stdio.h>
#include <pthread.h>

int all = 100;//全局变量
pthread_mutex_t mutex;

void *fun1(void *arg)
{
	while(1)
	{
		//加锁
		pthread_mutex_lock(&mutex);
		all = 50;
		sleep(1);
		printf("all is %d\n",all);
		pthread_mutex_unlock(&mutex);
		//解锁
	}
}

void *fun2(void *arg)
{
	while(1)
	{
		//加锁
		pthread_mutex_lock(&mutex);
		all = 0;
		//解锁
		pthread_mutex_unlock(&mutex);
	}

}

int main(int argc, const char *argv[])
{
	pthread_t id1,id2;

	pthread_mutex_init(&mutex,NULL);
	pthread_create(&id1,NULL,fun1,NULL);
	pthread_create(&id2,NULL,fun2,NULL);
	pthread_join(id1,NULL);
	pthread_join(id2,NULL);
	return 0;
}
