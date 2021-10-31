#include "data_global.h"

float dota_adc (unsigned int ratio)
{
	return ((ratio * 3.3) / 1024);
}

float dota_atof (char unitl)
{
	if (unitl > 100)
	{
		return unitl / 1000.0;
	}
	else if (unitl > 10)
	{
		return unitl / 100.0;
	}
	else
	{
		return unitl / 10.0;
	}
}

//串口初始化
void serial_init_115200(int fd)
{
//	printf("b serial_init\n");
	struct termios options;
	tcgetattr(fd, &options);
	options.c_cflag |= ( CLOCAL | CREAD );
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CSTOPB; 
	options.c_iflag |= IGNPAR;
	options.c_iflag &= ~(ICRNL | IXON);
	options.c_oflag = 0;
	options.c_lflag = 0;

	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	tcsetattr(fd,TCSANOW,&options);
//	printf("a serial_init\n");
}
//==== gqrs start =======

void serial_init_9600(int fd)
{
	/*if(check_fd_gprs(fd) < 0)
	  {
	  printf("error\n");
	  return ;
	  }*/
	struct termios options; //termios函数簇提供一个常规的终端接口，用于控制非同步通信端口。
	tcgetattr(fd, &options); //取得当前串口的属性，并付给collect_fd这个设备
	options.c_cflag |= ( CLOCAL | CREAD); //clocal表示忽略modem控制线，cread表示打开接收者

	options.c_cflag &= ~CSIZE; //清空原有字符长度（csize表示原有字符长度掩码）
	options.c_cflag &= ~CRTSCTS; //启用RTS/CTS（硬件）流控制
	options.c_cflag |= CS8;      //设置字符长度掩码
	options.c_cflag &= ~CSTOPB;  //设置停止位为1个（cstopb表示设置两个停止位）
	options.c_iflag |= IGNPAR;  //忽略帧错误和奇偶校验错
	options.c_oflag = 0;        //设置输出模式标志
	options.c_lflag = 0;    	//设置本地模式标志

	cfsetispeed(&options, B9600);		//设置输入波特率
	cfsetospeed(&options, B9600);		//设置输出波特率
	tcsetattr(fd, TCSANOW, &options);	//把上面设置好的属性赋值给collect_fd这个设备，tcsanow表示马上生效
}

