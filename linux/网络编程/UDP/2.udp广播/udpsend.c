#include "myhead.h"  //我自定义的头文件，里面包含了所有常用的头文件

/*
	udp广播发送代码 
*/   

int main()
{
	int ret;
	pthread_t id;
	int udpsock; 
	char sbuf[100];
	char ipbuf[20];
	unsigned short port;
	
	//定义ipv4地址结构体变量存放需要绑定的ip和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;  //地址协议，IPV4
	bindaddr.sin_addr.s_addr=htonl(INADDR_ANY);    //自动匹配当前ubuntu的ip地址
	bindaddr.sin_port=htons(30000);  //要绑定的端口号
	
	//定义ipv4地址结构体变量保存对方的ip和端口号
	struct sockaddr_in otheraddr;
	bzero(&otheraddr,sizeof(otheraddr));
	otheraddr.sin_family=AF_INET;  //地址协议，IPV4
	otheraddr.sin_addr.s_addr=inet_addr("192.168.11.255");   //8栋404课室广播专用地址
	otheraddr.sin_port=htons(20000);  //要绑定的端口号
	
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
	
	//设置udp套接字的属性为可以广播
	setsockopt(udpsock,SOL_SOCKET,SO_BROADCAST,&on,sizeof(on));
	
	//主程序发送广播信息--》发送给当前局域网中所有的主机
	while(1)
	{
		bzero(sbuf,100);
		printf("请输入要广播的信息!\n");
		scanf("%s",sbuf);
		//发送给局域网中所有的主机
		sendto(udpsock,sbuf,strlen(sbuf),0,(struct sockaddr *)&otheraddr,sizeof(otheraddr));
	}
	//挂机
	close(udpsock);
	return 0;
}