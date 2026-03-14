#include "myhead.h"  //我自定义的头文件，里面包含了所有常用的头文件

/*
	设置套接字的属性--》套接字允许端口复用
*/   
int tcpsock;

//线程任务函数：接收服务器发送过来的信息
void *recv_servermsg(void *arg)
{
	char rbuf[100];
	int ret;
	while(1)
	{
		bzero(rbuf,100);
		//接收服务器发送过来的信息
		ret=recv(tcpsock,rbuf,100,0);
		if(ret==0)
		{
			printf("对方(服务器端)断开连接了!\n");	
		}
		//判断收到的信息是不是quit
		if(strcmp(rbuf,"quit")==0)
			exit(0);
		printf("服务器给我发送的信息是: %s\n",rbuf);
	}
}
int main()
{
	int ret;
	char sbuf[100];
	pthread_t id;
	//定义ipv4地址结构体变量存放客户端需要绑定的ip和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;  //地址协议，IPV4
	bindaddr.sin_addr.s_addr=htonl(INADDR_ANY);;    //绑定客户端自己的ip地址
	bindaddr.sin_port=htons(10000);  //客户端要绑定的端口号
	
	//定义ipv4地址结构体变量存放服务器的ip和端口号
	struct sockaddr_in serveraddr;
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;  //地址协议，IPV4
	serveraddr.sin_addr.s_addr=inet_addr("192.168.11.150");    //服务器的ip地址
	serveraddr.sin_port=htons(30000);  //服务器的端口号
	
	//创建tcp套接字--》买手机
	tcpsock=socket(AF_INET,SOCK_STREAM,0);
	if(tcpsock==-1)
	{
		perror("买手机失败(创建tcp套接字)失败!\n");
		return -1;
	}
	
	//设置端口复用
	int on=1; //非0即可,是个开关，开启端口复用功能
	setsockopt(tcpsock,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	
	//绑定ip地址和端口号
	ret=bind(tcpsock,(struct sockaddr *)&bindaddr,sizeof(bindaddr));
	if(ret==-1)
	{
		perror("绑定ip和端口号失败了!\n");
		return -1;
	}
	
	//连接服务器
	ret=connect(tcpsock,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	if(ret==-1)
	{
		perror("连接服务器失败了!\n");
		return -1;
	}
	
	//创建线程接收信息
	pthread_create(&id,NULL,recv_servermsg,NULL);
	
	while(1)
	{
		bzero(sbuf,100);
		printf("请输入要发送给服务器的信息\n");
		scanf("%s",sbuf);
		//发送给服务器
		//写法1：
		//write(tcpsock,sbuf,strlen(sbuf));
		//写法2：
		send(tcpsock,sbuf,strlen(sbuf),0);
		//判断是不是quit
		if(strcmp(sbuf,"quit")==0)
			break;
	}
	
	//挂机
	close(tcpsock);
	return 0;
}