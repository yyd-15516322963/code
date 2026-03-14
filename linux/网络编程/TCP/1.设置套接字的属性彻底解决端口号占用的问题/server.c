#include "myhead.h"  //我自定义的头文件，里面包含了所有常用的头文件

/*
	设置套接字的属性--》套接字允许端口复用
	setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
*/   
int newsock;  //思路1：newsock定义成全局变量，方便线程使用
              //思路2：通过把newsock传递参数给线程，也可以实现线程使用newsock
			  
int flag=1;  //标志位：控制主程序中的while死循环退出

//线程的任务函数：发送信息给客户端
void *send_msgtoclient(void *arg)
{
	char sbuf[100];
	while(1)
	{
		bzero(sbuf,100);
		printf("请输入要发送给客户端的信息!\n");
		scanf("%s",sbuf);
		//发送给客户端-->不要用错套接字
		send(newsock,sbuf,strlen(sbuf),0);
		//判断你输入是不是"quit"
		if(strcmp(sbuf,"quit")==0)
		{
			//思路1：野蛮粗暴
			//exit(0); //结束进程
			//思路2：
			flag=0; //修改标志位
			pthread_exit(NULL); //结束线程
			
		}
	}
}

int main()
{
	int ret;
	pthread_t id;
	int tcpsock;
	char rbuf[100];
	//定义ipv4地址结构体变量存放客户端需要绑定的ip和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;  //地址协议，IPV4
	bindaddr.sin_addr.s_addr=inet_addr("192.168.11.4");    //绑定服务器自己的ip地址
	bindaddr.sin_port=htons(30000);  //服务器要绑定的端口号
	
	//定义ipv4地址结构体变量保存连接成功的那个客户端信息
	struct sockaddr_in clientaddr;
	int size=sizeof(clientaddr);
	
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
	
	//监听
	ret=listen(tcpsock,5);
	if(ret==-1)
	{
		perror("监听失败了!\n");
		return -1;
	}
	
	//接收客户端的连接请求，产生新的套接字
	newsock=accept(tcpsock,(struct sockaddr *)&clientaddr,&size);
	printf("目前连接成功的那个客户端ip是: %s  端口号是: %hu\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
		
	//创建线程发送信息
	pthread_create(&id,NULL,send_msgtoclient,NULL);
	
	while(flag)
	{
		bzero(rbuf,100);
		//接收客户端发送过来的信息
		//错误示范：用错了套接字,服务器必须使用accept产生的新套接字才能跟客户端同学
		//recv(tcpsock,rbuf,100,0);
		//正确示范：
		ret=recv(newsock,rbuf,100,0);  //阻塞在这里
		if(ret==0)
		{
			printf("对方(客户端)断开连接了!\n");	
		}
		if(strcmp(rbuf,"quit")==0)
			break;
		printf("客户端给我发送过来的信息是: %s\n",rbuf);
		
	}
	printf("服务器准备退出了!\n");
	//挂机
	close(tcpsock);
	close(newsock);
	return 0;
}