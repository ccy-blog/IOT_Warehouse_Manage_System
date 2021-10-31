#include "data_global.h"

extern int fd;

void *pthread_client_request(void *arg)
{
	struct msg buf;//消息
	printf("pthread_client_request is oK!!\n");
	//得到创建消息队列的key值
	key_t key = ftok("/app",'g');
	if(key == -1)
	{
		perror("ftok failed");
		exit(-1);
	}
	//创建一个消息队列
	int msgid = msgget(key,IPC_CREAT|0666);
	if(msgid == -1)
	{
		perror("msgget failed");
		exit(-1);
	}
	while(1)//接收发送过来的控制命令，并用ZigBee发送出去
	{	
		msgrcv(msgid,&buf,sizeof(buf),0,0);//接收消息
		write(fd,&buf.text[0],1);
	}

}
