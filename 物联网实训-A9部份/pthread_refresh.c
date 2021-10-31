#include "data_global.h"

extern e_info e_buf;
extern pthread_mutex_t mutex_refresh;
extern pthread_cond_t cond_refresh;

//刷新共享内存线程
void *pthread_refresh(void *arg)
{
	printf("333--pthread_refresh is ok!!\n");
	key_t key;//创建共享内存的key值
	int semid;//信号两的ID
	int shmid;//共享内存的ID 
	struct shm_addr *shm_buf = NULL;//保存共享内存的收地址

	if((key = ftok("/app", 'i')) < 0)
	{
		perror("ftok");
		exit(1);
	}
	
	if((semid  = semget(key, 1, IPC_CREAT|0666)) < 0)
	{
		perror("semget");
		exit(1);
	}
	//初始化信号量
	init_sem(semid,0,1);

	if((shmid = shmget(key, N, IPC_CREAT|0666 )) == -1)
	{
		perror("shmget");
		exit(1);
	}
	
	if((shm_buf = (struct shm_addr*)shmat(shmid, NULL, 0)) == (void*)-1)
	{
		perror("shmat");
		exit(1);
	}
	
	while(1)
	{
		//阻塞等待被唤醒
		pthread_mutex_lock(&mutex_refresh);
		pthread_cond_wait(&cond_refresh,&mutex_refresh);
		pthread_mutex_unlock(&mutex_refresh);

		sem_p(semid,0);
	
		shm_buf->rt_status.storage_no[STORAGE_NO].storage_status = 1;
		shm_buf->rt_status.storage_no[STORAGE_NO].led_status = 1;
		shm_buf->rt_status.storage_no[STORAGE_NO].buzzer_status = 1;
		shm_buf->rt_status.storage_no[STORAGE_NO].fan_status = 1;
		shm_buf->rt_status.storage_no[STORAGE_NO].seg_status = 1;
		shm_buf->rt_status.storage_no[STORAGE_NO].x = e_buf.x;
		shm_buf->rt_status.storage_no[STORAGE_NO].y = e_buf.y;
		shm_buf->rt_status.storage_no[STORAGE_NO].z = e_buf.z;
		shm_buf->rt_status.storage_no[STORAGE_NO].temperature = e_buf.temp[0] + dota_atof(e_buf.temp[1]);
		shm_buf->rt_status.storage_no[STORAGE_NO].temperatureMIN = 10;
		shm_buf->rt_status.storage_no[STORAGE_NO].temperatureMAX = 50;
		shm_buf->rt_status.storage_no[STORAGE_NO].humidity = e_buf.hum[0] + dota_atof(e_buf.hum[1]);
		shm_buf->rt_status.storage_no[STORAGE_NO].humidityMIN = 10;
		shm_buf->rt_status.storage_no[STORAGE_NO].humidityMAX = 50;
		shm_buf->rt_status.storage_no[STORAGE_NO].illumination = e_buf.ill;
		shm_buf->rt_status.storage_no[STORAGE_NO].illuminationMIN = 10;
		shm_buf->rt_status.storage_no[STORAGE_NO].illuminationMAX = 500;
		shm_buf->rt_status.storage_no[STORAGE_NO].battery = dota_adc(e_buf.bet);
		shm_buf->rt_status.storage_no[STORAGE_NO].adc = dota_adc(e_buf.adc);

		sem_v (semid, 0);
	}

}
