#include "myhead.h" //我自定义的头文件，里面包含了所有常用的头文件

/*
	tcp客户端的代码
		客户端双向通信
		1.分析客户端监测哪些文件描述符，监测它们的哪种状态改变
		  发送--》键盘输入--》监测键盘
		  接收--》套接字接收--》监测套接字
		2.没有学多路复用： tcp双向通信--》线程实现，创建线程
		  学多路复用： select监测键盘和套接字，分别判断处理发送和接收
*/

int main()
{
	int ret;
	char sbuf[100];
	char rbuf[100];
	int tcpsock;
	// 定义ipv4地址结构体变量存放客户端需要绑定的ip和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr, sizeof(bindaddr));
	bindaddr.sin_family = AF_INET;				  // 地址协议，IPV4
	bindaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 绑定客户端自己的ip地址
	bindaddr.sin_port = htons(10000);			  // 客户端要绑定的端口号

	// 定义ipv4地址结构体变量存放服务器的ip和端口号
	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;						   // 地址协议，IPV4
	serveraddr.sin_addr.s_addr = inet_addr("192.168.11.150"); // 服务器的ip地址
	serveraddr.sin_port = htons(30000);						   // 服务器的端口号

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

	// 连接服务器--》服务器就是我刚才配置购买的阿里云
	ret = connect(tcpsock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (ret == -1)
	{
		perror("连接服务器失败了!\n");
		return -1;
	}

	// 第一步：定义集合存放键盘和套接字的文件描述符
	fd_set myset;

	while (1)
	{
		// 一定要重复添加--》select每次监测会把状态没有改变的文件描述符删除，因此我有必要再添加一次
		FD_ZERO(&myset);		 // 清空集合
		FD_SET(0, &myset);		 // 把键盘添加进去
		FD_SET(tcpsock, &myset); // 把套接字添加进去

		// 调用select这个函数监测myset中的文件描述符状态改变
		ret = select(tcpsock + 1, &myset, NULL, NULL, NULL);
		
		if (ret > 0) // 表示select成功监测到文件描述符的状态改变
		{
			// 判断是不是键盘发生了状态改变，数据可读
			if (FD_ISSET(0, &myset) == 1)
			{
				bzero(sbuf, 100);
				// 读取键盘输入的内容
				scanf("%s", sbuf);
				// 立马发送给服务器
				send(tcpsock, sbuf, strlen(sbuf), 0);
			}
			// 判断是不是套接字发生了状态改变，数据可读
			if (FD_ISSET(tcpsock, &myset) == 1)
			{
				bzero(rbuf, 100);
				recv(tcpsock, rbuf, 100, 0);
				printf("服务器给我发送的信息是： %s\n", rbuf);
			}
		}
		else if (ret == 0) // 表示超时
		{
			perror("select超时了!\n");
			return -1;
		}
		else
		{
			perror("select调用失败了!\n");
			return -1;
		}
	}

	// 挂机
	close(tcpsock);
	return 0;
}