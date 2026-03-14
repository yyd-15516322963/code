#include "myhead.h"  //我自定义的头文件，里面包含了所有常用的头文件

/*
	udp单向通信
	   1.流程
	     socket--》bind--》sendto或者recvfrom--》close
	   2.新知识点
          问题：绑定本地主机的ip，每个人的ip地址都不一样，需要修改代码，很麻烦
          解决方法：linux提供了一个宏定义INADDR_ANY，可以自动匹配本地主机任意一个ip地址	
          使用方法：htonl(INADDR_ANY)		  
*/   


	
int main()
{
	int ret;
	pthread_t id;
	int udpsock; 
	char sbuf[100];
	char ipbuf[20];
	unsigned short port;
	char rbuf[100];
	
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
	
	//主程序发送信息
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
		
		bzero(sbuf,100);
		bzero(ipbuf,20);
		
		printf("请输入你要发送给对方的信息!\n");
		scanf("%s",sbuf);
		printf("请输入你要把这个信息发给谁，对方的ip地址!\n");
		scanf("%s",ipbuf);
		printf("请输入对方的端口号!\n");
		scanf("%hu",&port);
		
		otheraddr.sin_addr.s_addr=inet_addr(ipbuf);    //对方的ip地址
		otheraddr.sin_port=htons(port);  //对方的端口号
		
		//发送给对方
		//sendto总有六个参数，前面四个参数跟send一模一样的含义
		//第五个参数表示对方的ip和端口号   第六个参数是结构体大小
		sendto(udpsock,sbuf,strlen(sbuf),0,(struct sockaddr *)&otheraddr,sizeof(otheraddr));

	}
	//挂机
	close(udpsock);
	return 0;
}