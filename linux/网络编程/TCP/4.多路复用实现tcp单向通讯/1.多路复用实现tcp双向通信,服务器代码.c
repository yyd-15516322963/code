#include "myhead.h" //我自定义的头文件，里面包含了所有常用的头文件

/*
	服务器代码
		1.分析服务器
		  发送--》键盘输入--》监测键盘
		  接收--》新的套接字接收--》监测新的套接字
		2.select同时监测多个文件描述符，select会自动把状态没有改变的文件描述符删除，只保留状态改变的文件描述符
*/

int main()
{
	int ret;
	char sbuf[100];
	int newsock;
	int tcpsock;
	char rbuf[100];
	// 定义ipv4地址结构体变量存放客户端需要绑定的ip和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr, sizeof(bindaddr));
	bindaddr.sin_family = AF_INET;				  // 地址协议，IPV4
	bindaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 绑定服务器自己的ip地址
	bindaddr.sin_port = htons(30000);			  // 服务器要绑定的端口号

	// 定义ipv4地址结构体变量保存连接成功的那个客户端信息
	struct sockaddr_in clientaddr;
	int size = sizeof(clientaddr);

	// 创建tcp套接字--》买手机
	tcpsock = socket(AF_INET, SOCK_STREAM, 0);
	if (tcpsock == -1)
	{
		perror("买手机失败(创建tcp套接字)失败!\n");
		return -1;
	}

	// 设置端口复用
	int on = 1; // 非0即可,是个开关，开启端口复用功能
	setsockopt(tcpsock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	// 绑定ip地址和端口号
	ret = bind(tcpsock, (struct sockaddr *)&bindaddr, sizeof(bindaddr));
	if (ret == -1)
	{
		perror("绑定ip和端口号失败了!\n");
		return -1;
	}

	// 监听
	ret = listen(tcpsock, 5);
	if (ret == -1)
	{
		perror("监听失败了!\n");
		return -1;
	}

	// 接收客户端的连接请求，产生新的套接字
	newsock = accept(tcpsock, (struct sockaddr *)&clientaddr, &size);
	printf("目前连接成功的那个客户端ip是: %s  端口号是: %hu\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	// 定义文件描述符集合
	fd_set myset;

	while (1)
	{
		// 一定要重复添加--》select每次监测会把状态没有改变的文件描述符删除，因此我有必要再添加一次
		FD_ZERO(&myset);		 // 清空集合
		FD_SET(newsock, &myset); // 添加新的文件描述符
		FD_SET(0, &myset);		 // 添加键盘的文件描述符

		ret = select(newsock + 1, &myset, NULL, NULL, NULL);
		
		if (ret > 0)
		{
			if (FD_ISSET(newsock, &myset) == 1) // 说明newsock发生了读就绪(有数据可读)
			{
				bzero(rbuf, 100);
				recv(newsock, rbuf, 100, 0);
				printf("客户端给我发送的数据是： %s\n", rbuf);
			}
			if (FD_ISSET(0, &myset) == 1) // 说明键盘发生了读就绪(有数据可读)
			{
				bzero(sbuf, 100);
				scanf("%s", sbuf); // 读取键盘输入
				// 发送给客户端
				send(newsock, sbuf, strlen(sbuf), 0);
			}
		}
		else if (ret == 0)
		{
			perror("select超时了！\n");
			return -1;
		}
		else
		{
			perror("select监测失败了！\n");
			return -1;
		}
	}

	// 挂机
	close(tcpsock);
	close(newsock);
	return 0;
}