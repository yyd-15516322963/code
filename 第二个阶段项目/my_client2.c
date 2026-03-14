#include "myhead.h" //我自定义的头文件，里面包含了所有常用的头文件

/*
	设置套接字的属性--》套接字允许端口复用
*/
int tcpsock;

// 线程任务函数：接收服务器发送过来的信息
void *recv_servermsg(void *arg)
{
	char rbuf[2048];
	int ret;
	while (1)
	{
		bzero(rbuf, 2048);
		// 接收服务器发送过来的信息
		ret = recv(tcpsock, rbuf, 2048, 0);
		sleep(1);
		if (ret == 0)
		{
			printf("对方(服务器端)断开连接了!\n");
			exit(0);
		}
		// 判断收到的信息是不是quit
		if (strcmp(rbuf, "quit") == 0)
			exit(0);
		printf("收到的信息是: %s\n", rbuf);

		char *p1 = strtok(rbuf, "@");
		if (strcmp(p1, "msg") == 0) // 说明客户端发过来是文字聊天信息
		{
			// 继续切割
			char *p2 = strtok(NULL, "@"); // 对方ip
			char *p3 = strtok(NULL, "@"); // 对方端口
			char *p4 = strtok(NULL, "@"); // 真实的信息
			// 遍历链表，找到跟p2，p3对应的客户端，然后把信息转发出去即可
			printf("收到的信息是: %s\n", rbuf);
		}
		if (strcmp(p1, "file") == 0) // 说明客户端发过来是文件
		{
		}
		if (strcmp(p1, "emoji") == 0) // 说明客户端发过来是表情包
		{
			// 继续切割
			char *p2 = strtok(NULL, "@"); // 对方ip
			char *p3 = strtok(NULL, "@"); // 对方端口
			char *p4 = strtok(NULL, "@"); // 表情包大小
			long size = atol(p4);		  // 用atol更安全（long类型）
			printf("收到的表情包大小是： %ld\n", size);

			// 接收客户端接下来发送的表情包数据（动态分配缓冲区，避免栈溢出）
			char *buf = malloc(size);
			if (buf == NULL)
			{
				perror("malloc emoji buf failed");
				continue;
			}

			/* 循环接收表情包数据 */
			ret = 0;
			long remainingbytes = size; // 存放剩余要接收的字节数
			char *p5 = buf;				// 指向数组的起始位置
			while (1)
			{
				printf("阻塞recv，剩余接收字节数是: %ld\n", remainingbytes);
				ret = recv(tcpsock, p5, remainingbytes, 0); // 用remainingbytes而非固定500，提高效率
				if (ret <= 0)
				{
					perror("recv emoji data failed");
					free(buf);
					buf = NULL;
					break;
				}
				// 更新剩余字节数
				remainingbytes = remainingbytes - ret;
				// 更新p5
				p5 = p5 + ret;
				printf("接收到表情包数据字节数是: %d\n", ret);
				// 退出条件
				if (remainingbytes <= 0)
					break;
			}

			if (buf != NULL)
			{
				printf("服务器表情包文件接收完毕！\n");
				// 新建空白的jpeg文件，保存表情包数据
				int fd;
				fd = open("./new.jpeg", O_CREAT | O_TRUNC | O_RDWR, 0777);
				if (fd == -1)
				{
					perror("新建空白的jpeg文件失败了!\n");
					free(buf);
					continue;
				}
				// 写入到新建的文件中
				write(fd, buf, size);
				close(fd);
				free(buf); // 释放缓冲区
			}
		}
	}
}
int main()
{
	int ret;
	int n;
	char sbuf[100];
	char otherip[20];		  // 对方的ip
	unsigned short otherport; // 对方的端口
	char allbuf[2048];
	pthread_t id;
	// 定义ipv4地址结构体变量存放客户端需要绑定的ip和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr, sizeof(bindaddr));
	bindaddr.sin_family = AF_INET;							// 地址协议，IPV4
	bindaddr.sin_addr.s_addr = inet_addr("192.168.11.150"); // 绑定客户端自己的ip地址
	bindaddr.sin_port = htons(10002);						// 客户端要绑定的端口号

	// 定义ipv4地址结构体变量存放服务器的ip和端口号
	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;						  // 地址协议，IPV4
	serveraddr.sin_addr.s_addr = inet_addr("192.168.11.150"); // 服务器的ip地址
	serveraddr.sin_port = htons(30000);						  // 服务器的端口号

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

	// 连接服务器
	ret = connect(tcpsock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (ret == -1)
	{
		perror("连接服务器失败了!\n");
		return -1;
	}

	// 创建线程接收信息
	pthread_create(&id, NULL, recv_servermsg, NULL);

	while (1)
	{
		printf("请输入您的选择!\n");
		printf("1.获取服务器上所以在线的客户端信息！\n");
		printf("2.跟某个客户端单聊！\n");
		printf("3.跟某个客户端发送文件！\n");
		printf("4.跟某个客户端发送表情包！\n");
		scanf("%d", &n);
		switch (n)
		{
		case 1:
			send(tcpsock, "getlist", 7, 0);
			break;
		case 2:
			bzero(otherip, 20);
			bzero(sbuf, 100);
			bzero(allbuf, 2048);
			printf("请输入对方的ip!\n");
			scanf("%s", otherip);
			printf("请输入对方的端口号!\n");
			scanf("%hu", &otherport);
			printf("请输入要发送这个客户端的真实信息!\n");
			scanf("%s", sbuf);
			// 按照制定的通信协议，拼接字符串
			sprintf(allbuf, "msg@%s@%hu@%s", otherip, otherport, sbuf);
			// 发送给服务器
			send(tcpsock, allbuf, strlen(allbuf), 0);
			break;
		case 3:
			break;
		case 4:
			bzero(otherip, 20);
			bzero(sbuf, 100);
			bzero(allbuf, 2048);
			printf("请输入对方的ip!\n");
			scanf("%s", otherip);
			printf("请输入对方的端口号!\n");
			scanf("%hu", &otherport);
			printf("请输入要发送这个客户端的表情文件路径名!\n");
			scanf("%s", sbuf);
			// 获取表情包文件大小
			struct stat mystat;
			stat(sbuf, &mystat);
			// 按照制定的通信协议，拼接字符串
			sprintf(allbuf, "emoji@%s@%hu@%ld", otherip, otherport, mystat.st_size);
			// 发送给服务器
			send(tcpsock, allbuf, strlen(allbuf), 0);
			// 打开表情包文件，把表情包数据读取出来，发送给服务器
			int fd;
			fd = open(sbuf, O_RDWR);
			if (fd == -1)
			{
				perror("打开表情包失败了!\n");
				return -1;
			}
			char buf[mystat.st_size];
			// 读取完整表情包数据
			read(fd, buf, mystat.st_size);
			close(fd);
			// 发送整个表情包数据
			ret = send(tcpsock, buf, mystat.st_size, 0);
			printf("客户端发送的字节数是: %d\n", ret);
			break;
		}
	}

	// 挂机
	close(tcpsock);
	return 0;
}