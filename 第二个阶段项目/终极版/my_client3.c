#include "myhead.h" //我自定义的头文件，里面包含了所有常用的头文件

/*
	设置套接字的属性--》套接字允许端口复用
*/
int tcpsock;

// 线程任务函数：接收服务器发送过来的信息
// 线程任务函数：接收服务器发送过来的信息
void *recv_servermsg(void *arg)
{
	char rbuf[2048];
	int ret;
	while (1)
	{
		bzero(rbuf, 2048);
		ret = recv(tcpsock, rbuf, 2048, 0);
		if (ret <= 0)
		{
			if (ret == 0)
				printf("服务器断开连接！\n");
			else
				perror("recv失败");
			close(tcpsock);
			tcpsock = -1;
			pthread_exit(NULL);
		}

		printf("收到服务器原始消息：%s\n", rbuf);

		char *msg_type = strtok(rbuf, "@");

		if (strcmp(msg_type, "msg") == 0)
		{
			char *ip_part = strtok(NULL, "@");
			char *port_part = strtok(NULL, "@");
			char *msg_content = strtok(NULL, "@");

			if (ip_part && port_part && msg_content)
			{
				unsigned short port = atoi(port_part);
				printf("解析到文本消息 - IP:%s, 端口:%d, 内容:%s\n", ip_part, port, msg_content);
			}
		}
		else if (strcmp(msg_type, "file") == 0)
		{ // 文件消息
			char *ip_part = strtok(NULL, "@");
			char *port_part = strtok(NULL, "@");
			char *size_part = strtok(NULL, "@");
			char *file_type = strtok(NULL, "@");
			char *filename = strtok(NULL, "@");

			if (!ip_part || !port_part || !size_part || !filename)
			{
				printf("文件消息格式错误\n");
				continue;
			}

			long file_size = atol(size_part);
			unsigned short port = atoi(port_part);
			printf("收到文件消息 - 来自:%s:%d, 大小:%ld, 文件名:%s\n",
				   ip_part, port, file_size, filename);

			// 接收文件数据
			char *buf = malloc(file_size);
			if (buf == NULL)
			{
				perror("分配文件缓冲区失败");
				continue;
			}

			long remainingbytes = file_size;
			char *ptr = buf;
			int total_received = 0;

			while (remainingbytes > 0)
			{
				int recv_size = (remainingbytes > 1024) ? 1024 : remainingbytes;
				ret = recv(tcpsock, ptr, recv_size, 0);
				if (ret <= 0)
				{
					perror("接收文件数据失败");
					free(buf);
					break;
				}

				total_received += ret;
				remainingbytes -= ret;
				ptr += ret;
				printf("已接收文件数据: %d字节, 已接收:%d字节, 剩余: %ld字节\n",
					   ret, total_received, remainingbytes);
			}

			if (total_received == file_size)
			{
				// 保存文件
				char save_path[256];
				snprintf(save_path, sizeof(save_path), "./recv_%s", filename);

				int fd = open(save_path, O_CREAT | O_TRUNC | O_RDWR, 0777);
				if (fd != -1)
				{
					write(fd, buf, file_size);
					close(fd);
					printf("文件保存成功: %s\n", save_path);
				}
				else
				{
					perror("保存文件失败");
				}
			}
			else
			{
				printf("文件接收不完整: 期望%ld字节, 实际收到%d字节\n", file_size, total_received);
			}

			free(buf);
		}
		else if (strcmp(msg_type, "emoji") == 0)
		{ // 表情包消息
			char *ip_part = strtok(NULL, "@");
			char *port_part = strtok(NULL, "@");
			char *size_part = strtok(NULL, "@");

			if (!ip_part || !port_part || !size_part)
			{
				printf("表情包消息格式错误\n");
				continue;
			}

			long size = atol(size_part);
			unsigned short port = atoi(port_part);
			printf("收到表情包消息 - 来自:%s:%d, 大小:%ld\n", ip_part, port, size);

			// 接收表情包数据
			char *buf = malloc(size);
			if (buf == NULL)
			{
				perror("分配表情包缓冲区失败");
				continue;
			}

			long remainingbytes = size;
			char *ptr = buf;
			int total_received = 0;

			while (remainingbytes > 0)
			{
				int recv_size = (remainingbytes > 1024) ? 1024 : remainingbytes;
				ret = recv(tcpsock, ptr, recv_size, 0);
				if (ret <= 0)
				{
					perror("接收表情包数据失败");
					free(buf);
					break;
				}

				total_received += ret;
				remainingbytes -= ret;
				ptr += ret;
				printf("已接收表情包数据: %d字节, 已接收:%d字节, 剩余: %ld字节\n",
					   ret, total_received, remainingbytes);
			}

			if (total_received == size)
			{
				// 保存表情包
				int fd = open("./recv_emoji.jpg", O_CREAT | O_TRUNC | O_RDWR, 0777);
				if (fd != -1)
				{
					write(fd, buf, size);
					close(fd);
					printf("表情包保存成功: recv_emoji.jpg\n");
				}
				else
				{
					perror("保存表情包失败");
				}
			}
			else
			{
				printf("表情包接收不完整: 期望%ld字节, 实际收到%d字节\n", size, total_received);
			}

			free(buf);
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
	bindaddr.sin_port = htons(10003);						// 客户端要绑定的端口号

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
			bzero(otherip, 20);
			bzero(sbuf, 100);
			bzero(allbuf, 2048);
			printf("请输入对方的ip!\n");
			scanf("%s", otherip);
			printf("请输入对方的端口号!\n");
			scanf("%hu", &otherport);
			printf("请输入要发送的文件路径名!\n");
			scanf("%s", sbuf);

			// 获取文件信息
			struct stat file_stat;
			if (stat(sbuf, &file_stat) == -1)
			{
				perror("获取文件信息失败");
				break;
			}

			// 检查文件大小
			if (file_stat.st_size <= 0)
			{
				printf("错误：文件大小为0或文件不存在！\n");
				break;
			}

			printf("文件大小：%ld字节\n", file_stat.st_size);

			// 提取文件名和文件扩展名
			char *filename = strrchr(sbuf, '/');
			if (filename == NULL)
				filename = sbuf;
			else
				filename++; // 跳过'/'

			char *file_ext = strrchr(filename, '.');
			if (file_ext == NULL)
				file_ext = "";

			printf("文件名：%s，文件类型：%s\n", filename, file_ext);

			// 按照制定的通信协议，拼接字符串
			// 格式：file@目标IP@目标端口@文件大小@文件类型@文件名
			snprintf(allbuf, sizeof(allbuf), "file@%s@%hu@%ld@%s@%s",
					 otherip, otherport, file_stat.st_size, file_ext, filename);

			// 发送文件消息头给服务器
			int header_len = send(tcpsock, allbuf, strlen(allbuf), 0);
			if (header_len <= 0)
			{
				perror("发送文件消息头失败");
				break;
			}
			printf("文件消息头发送成功，长度：%d\n", header_len);

			// 打开文件
			int fd = open(sbuf, O_RDONLY);
			if (fd == -1)
			{
				perror("打开文件失败");
				break;
			}

			// 分配缓冲区
			char *file_buf = malloc(file_stat.st_size);
			if (file_buf == NULL)
			{
				perror("分配文件缓冲区失败");
				close(fd);
				break;
			}

			// 读取完整文件数据
			ssize_t read_size = read(fd, file_buf, file_stat.st_size);
			close(fd);

			if (read_size != file_stat.st_size)
			{
				printf("读取文件不完整，期望%ld字节，实际读取%ld字节\n",
					   file_stat.st_size, (long)read_size);
				free(file_buf);
				break;
			}

			// 发送文件数据
			ssize_t total_sent = 0;
			ssize_t sent = 0;

			while (total_sent < file_stat.st_size)
			{
				// 计算要发送的数据块大小
				ssize_t to_send = file_stat.st_size - total_sent;
				if (to_send > 4096)
					to_send = 4096; // 每次最多发送4KB

				sent = send(tcpsock, file_buf + total_sent, to_send, 0);
				if (sent <= 0)
				{
					perror("发送文件数据失败");
					free(file_buf);
					break;
				}

				total_sent += sent;
				printf("已发送文件数据: %ld字节, 累计发送: %ld字节, 剩余: %ld字节\n",
					   (long)sent, (long)total_sent, (long)(file_stat.st_size - total_sent));
			}

			if (total_sent == file_stat.st_size)
			{
				printf("文件发送成功，大小：%ld字节\n", (long)file_stat.st_size);
			}
			else
			{
				printf("文件发送不完整，期望%ld字节，实际发送%ld字节\n",
					   (long)file_stat.st_size, (long)total_sent);
			}

			free(file_buf);
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
			int fd2;
			fd2 = open(sbuf, O_RDWR);
			if (fd2 == -1)
			{
				perror("打开表情包失败了!\n");
				return -1;
			}
			char buf[mystat.st_size];
			// 读取完整表情包数据
			read(fd2, buf, mystat.st_size);
			close(fd2);
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