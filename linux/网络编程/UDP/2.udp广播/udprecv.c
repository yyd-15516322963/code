#include "myhead.h"  //我自定义的头文件，里面包含了所有常用的头文件

/*
	udp接收信息
*/   


	
int main()
{
	int ret;
	pthread_t id;
	int udpsock; 
	char rbuf[100];

	//定义ipv4地址结构体变量存放需要绑定的ip和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;  //地址协议，IPV4
	bindaddr.sin_addr.s_addr=htonl(INADDR_ANY);    //自动匹配当前ubuntu的ip地址
	bindaddr.sin_port=htons(20000);  //要绑定的端口号
	
	//定义ipv4地址结构体变量保存对方的ip和端口号
	struct sockaddr_in otheraddr;
	bzero(&otheraddr,sizeof(otheraddr));
	int size=sizeof(otheraddr);
	
	//创建udp套接字--》买手机
	udpsock=socket(AF_INET,SOCK_DGRAM,0);
	if(udpsock==-1)
	{
		perror("买手机失败(创建udp套接字)失败!\n");
		return -1;
	}
	
	//设置端口复用
	int on=1; //非0即可,是个开关，开启端口复用功能
	setsockopt(udpsock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	
	//绑定ip地址和端口号
	ret=bind(udpsock,(struct sockaddr *)&bindaddr,sizeof(bindaddr));
	if(ret==-1)
	{
		perror("绑定ip和端口号失败了!\n");
		return -1;
	}
	
	//主程序接收信息
	while(1)
	{
		bzero(rbuf,100);
		//接收信息
		//recvfrom总有六个参数，前面四个参数跟recv一模一样的含义
		//第五个参数存放对方的ip和端口号   第六个参数是结构体大小，要求是指针
		//recvfrom源码会自动帮你把发生信息给你的那个主机的ip和端口号存放到第五个参数的地址中
		//写法1：NULL表示给我发送信息的那个主机的信息我不想要
		//recvfrom(udpsock,rbuf,100,0,NULL,NULL);
		//写法2：给我发送信息的那个主机的信息我想要保存
		recvfrom(udpsock,rbuf,100,0,(struct sockaddr *)&otheraddr,&size);
		printf("对方给我发送的信息是: %s 对方的ip是: %s,对方端口是:%hu\n",rbuf,inet_ntoa(otheraddr.sin_addr),ntohs(otheraddr.sin_port));
	}
	//挂机
	close(udpsock);
	return 0;
}