#include "myhead.h" //我自定义的头文件，里面包含了所有常用的头文件

/*
	多个客户端连接服务器
*/

// 用户信息结构体
struct user_info
{
	char username[50];
	char password[50];
	struct user_info *next;
};

// 用户列表全局变量
struct user_info *user_list = NULL;
pthread_mutex_t user_mutex = PTHREAD_MUTEX_INITIALIZER;

// 加载用户信息从文件
int load_users_from_file(const char *filename)
{
	FILE *fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("用户文件不存在，将创建新文件\n");
		return 0;
	}

	char line[100];
	int count = 0;

	while (fgets(line, sizeof(line), fp) != NULL)
	{
		line[strcspn(line, "\n")] = '\0';

		char *username = strtok(line, "@");
		char *password = strtok(NULL, "@");

		if (username && password)
		{
			// 创建新用户节点
			struct user_info *new_user = malloc(sizeof(struct user_info));
			strncpy(new_user->username, username, sizeof(new_user->username) - 1);
			strncpy(new_user->password, password, sizeof(new_user->password) - 1);
			new_user->next = user_list;
			user_list = new_user;
			count++;
		}
	}

	fclose(fp);
	printf("从文件加载了 %d 个用户\n", count);
	return count;
}

// 保存用户信息到文件
int save_users_to_file(const char *filename)
{
	FILE *fp = fopen(filename, "w");
	if (fp == NULL)
	{
		perror("打开用户文件失败");
		return -1;
	}

	struct user_info *current = user_list;
	int count = 0;

	while (current != NULL)
	{
		fprintf(fp, "%s@%s\n", current->username, current->password);
		current = current->next;
		count++;
	}

	fclose(fp);
	printf("保存了 %d 个用户到文件\n", count);
	return count;
}

// 检查用户是否存在
int user_exists(const char *username)
{
	struct user_info *current = user_list;

	while (current != NULL)
	{
		if (strcmp(current->username, username) == 0)
		{
			return 1; // 用户存在
		}
		current = current->next;
	}

	return 0; // 用户不存在
}

// 添加新用户
int add_user(const char *username, const char *password)
{
	pthread_mutex_lock(&user_mutex);

	// 检查用户是否已存在
	if (user_exists(username))
	{
		pthread_mutex_unlock(&user_mutex);
		return 0; // 用户已存在
	}

	// 创建新用户节点
	struct user_info *new_user = malloc(sizeof(struct user_info));
	strncpy(new_user->username, username, sizeof(new_user->username) - 1);
	strncpy(new_user->password, password, sizeof(new_user->password) - 1);
	new_user->next = user_list;
	user_list = new_user;

	// 保存到文件
	save_users_to_file("./users.txt");

	pthread_mutex_unlock(&user_mutex);
	return 1; // 添加成功
}

// 验证用户登录
int verify_user(const char *username, const char *password)
{
	pthread_mutex_lock(&user_mutex);

	struct user_info *current = user_list;

	while (current != NULL)
	{
		if (strcmp(current->username, username) == 0 &&
			strcmp(current->password, password) == 0)
		{
			pthread_mutex_unlock(&user_mutex);
			return 1; // 验证成功
		}
		current = current->next;
	}

	pthread_mutex_unlock(&user_mutex);
	return 0; // 验证失败
}

// 发送响应给客户端
void send_response(int sock, const char *type, const char *status,
				   const char *username, const char *message)
{
	char response[256];
	snprintf(response, sizeof(response), "%s@%s@%s@%s",
			 type, status, username, message);
	send(sock, response, strlen(response), 0);
}

// 定义结构体存放连接成功的客户端信息
struct clientmsg
{
	// 数据域
	int sock;			 // 目前连接成功客户端套接字
	char ip[20];		 // 客户端ip地址
	unsigned short port; // 客户端端口号

	// 指针域
	struct clientmsg *next;
};

struct clientmsg *myhead; // 全局变量
int linkflag = 0;		  // 如果有新的客户端连接就改变这个标志位

// 初始化链表的头结点
struct clientmsg *client_init()
{
	struct clientmsg *head = malloc(sizeof(struct clientmsg));
	head->next = NULL;
	return head;
}

// 尾插某个节点到链表的尾部
int insert_tail(struct clientmsg *node, struct clientmsg *head)
{
	// 找到链表的尾部
	struct clientmsg *p = head;
	while (p->next != NULL)
		p = p->next;

	// 把节点插入到尾部
	p->next = node;
	return 0;
}

// 删除节点
int list_delete(char ipbuf[20], unsigned short delport, struct clientmsg *head)
{
	// 定义两个指针,一前一后遍历链表
	struct clientmsg *p = head->next;
	struct clientmsg *q = head;

	while (p != NULL)
	{
		if (strcmp(p->ip, ipbuf) == 0 && p->port == delport)
			break;

		// p和q往后挪动
		p = p->next;
		q = q->next;
	}

	// 删除p这个节点
	q->next = p->next;
	p->next = NULL;
	free(p);
}

char * find_user_password(const char * username)
{
	pthread_mutex_lock(&user_mutex);

	struct user_info * current = user_list;

	while (current != NULL)
	{
		if (strcmp(current->username, username) == 0)
		{
			pthread_mutex_unlock(&user_mutex);
			return current->password;  // 返回密码
		}
		current = current->next;
	}

	pthread_mutex_unlock(&user_mutex);
	return NULL;  // 用户不存在
}

// 线程的任务函数：专门负责接收某个客户端发送过来的信息
void *recv_client_msg(void *arg)
{
	int ret;
	struct clientmsg *p = (struct clientmsg *)arg;
	char clientbuf[2048];
	char rbuf[2048];
	while (1)
	{
		bzero(rbuf, 2048);
		// 接收客户端的信息
		ret = recv(p->sock, rbuf, 2048, 0);
		if (ret <= 0)
		{
			printf("客户端ip %s  端口号%hu断开了\n", p->ip, p->port);
			// 从单链表中删除这个断开的客户端
			list_delete(p->ip, p->port, myhead);
			// 退出当前线程
			pthread_exit(NULL);
		}

		printf("收到的原始信息: %s\n", rbuf);

		// 按照协议分类处理信息
		char *p1 = strtok(rbuf, "@");

		// 注册请求
		if (strcmp(p1, "reg") == 0)
		{
			char *username = strtok(NULL, "@");
			char *password = strtok(NULL, "@");

			if (username && password)
			{
				printf("收到注册请求: 用户名=%s, 密码=%s\n", username, password);

				// 尝试添加用户
				if (add_user(username, password))
				{
					// 注册成功
					send_response(p->sock, "reg_response", "success", username, "注册成功");
					printf("用户 %s 注册成功\n", username);
				}
				else
				{
					// 用户名已存在
					send_response(p->sock, "reg_response", "fail", username, "用户名已存在");
					printf("用户 %s 注册失败，用户名已存在\n", username);
				}
			}
		}
		// 登录请求
		else if (strcmp(p1, "login") == 0)
		{
			char *username = strtok(NULL, "@");
			char *password = strtok(NULL, "@");

			if (username && password)
			{
				printf("收到登录请求: 用户名=%s, 密码=%s\n", username, password);

				// 验证用户
				if (verify_user(username, password))
				{
					// 登录成功
					send_response(p->sock, "login_response", "success", username, "登录成功");
					printf("用户 %s 登录成功\n", username);
				}
				else
				{
					// 登录失败
					send_response(p->sock, "login_response", "fail", username, "用户名或密码错误");
					printf("用户 %s 登录失败\n", username);
				}
			}
		}
		// 找回密码请求
		else if (strcmp(p1, "forget") == 0)
		{
			char *username = strtok(NULL, "@");

			if (username)
			{
				printf("收到找回密码请求: 用户名=%s\n", username);

				// 查找用户密码
				char *password = find_user_password(username);
				if (password != NULL)
				{
					// 找到用户，返回密码
					send_response(p->sock, "forget_response", "success", username, password);
					printf("用户 %s 的密码是: %s\n", username, password);
				}
				else
				{
					// 用户不存在
					send_response(p->sock, "forget_response", "fail", username, "用户不存在");
					printf("用户 %s 不存在\n", username);
				}
			}
		}
		// 获取在线客户端列表
		else if (strcmp(p1, "getlist") == 0)
		{
			printf("客户端请求获取在线列表\n");
			
			bzero(clientbuf, 2048);
			// 遍历链表，拼接所有在线客户端信息
			struct clientmsg *temp = myhead;
			int first = 1;
			while (temp->next != NULL)
			{
				temp = temp->next;
				if(!first) {
					strcat(clientbuf, "@");
				}
				sprintf(clientbuf + strlen(clientbuf), "%s:%hu", temp->ip, temp->port);
				first = 0;
			}
			
			if(strlen(clientbuf) > 0) {
				send(p->sock, clientbuf, strlen(clientbuf), 0);
				printf("发送在线列表: %s\n", clientbuf);
			} else {
				send(p->sock, "empty", 5, 0);
				printf("当前没有在线客户端\n");
			}
		}
		// 文字聊天信息
		else if (strcmp(p1, "msg") == 0)
		{
			// 继续切割
			char *p2 = strtok(NULL, "@"); // 对方ip
			char *p3 = strtok(NULL, "@"); // 对方端口
			char *p4 = strtok(NULL, "@"); // 真实的信息
			
			printf("收到聊天消息: 发送方=%s:%hu, 接收方=%s:%s, 内容=%s\n", 
				   p->ip, p->port, p2, p3, p4);
			
			// 查找目标客户端
			struct clientmsg *temp = myhead;
			while (temp->next != NULL)
			{
				temp = temp->next;
				if (strcmp(temp->ip, p2) == 0 && temp->port == atoi(p3))
				{
					bzero(clientbuf, 2048);
					// 拼接字符串，包含发送方信息
					sprintf(clientbuf, "%s@%s@%hu@%s", p1, p->ip, p->port, p4);
					send(temp->sock, clientbuf, strlen(clientbuf), 0);
					printf("转发消息给 %s:%hu\n", temp->ip, temp->port);
					break;
				}
			}
		}
		// 文件消息
		else if (strcmp(p1, "file") == 0)
		{
			// 1. 切割基础信息
			char *p2 = strtok(NULL, "@"); // 对方ip
			char *p3 = strtok(NULL, "@"); // 对方端口
			char *p4 = strtok(NULL, "@"); // 文件大小 
			char *p5 = strtok(NULL, "@"); // 文件类型
			char *p6 = strtok(NULL, "@"); // 文件名
			long size = atol(p4);
			
			printf("收到文件消息: 发送方=%s:%hu, 接收方=%s:%s, 大小=%ld, 文件=%s\n", 
				   p->ip, p->port, p2, p3, size, p6);

			// 2. 找到目标客户端
			struct clientmsg *temp = myhead;
			struct clientmsg *target_client = NULL;
			while (temp->next != NULL)
			{
				temp = temp->next;
				if (strcmp(temp->ip, p2) == 0 && temp->port == atoi(p3))
				{
					target_client = temp;
					break;
				}
			}

			if (target_client != NULL)
			{
				// 3. 先给目标客户端发送文件基础信息
				bzero(clientbuf, 2048);
				sprintf(clientbuf, "%s@%s@%hu@%ld@%s@%s", p1, p->ip, p->port, size, p5, p6);
				send(target_client->sock, clientbuf, strlen(clientbuf), 0);
				
				printf("发送文件头信息给目标客户端: %s\n", clientbuf);

				// 4. 接收发送方客户端的文件二进制数据
				char *file_buf = malloc(size);
				if (file_buf == NULL)
				{
					perror("malloc file buf failed");
					continue;
				}
				
				long remaining = size;
				char *ptr = file_buf;
				int recv_len;
				int total_received = 0;
				
				while (remaining > 0)
				{
					recv_len = recv(p->sock, ptr, remaining, 0);
					if (recv_len <= 0)
					{
						perror("recv file data failed");
						free(file_buf);
						file_buf = NULL;
						break;
					}
					remaining -= recv_len;
					total_received += recv_len;
					ptr += recv_len;
					printf("接收到文件数据: %d字节, 已接收:%d字节, 剩余: %ld字节\n", 
						   recv_len, total_received, remaining);
				}

				// 5. 转发二进制数据给目标客户端
				if (file_buf != NULL && total_received == size)
				{
					int sent = send(target_client->sock, file_buf, size, 0);
					printf("转发文件数据给目标客户端: 发送了%d字节\n", sent);
					free(file_buf);
				}
				else
				{
					if (file_buf) free(file_buf);
					printf("文件接收不完整: 期望%ld字节, 实际收到%d字节\n", size, total_received);
				}
			}
			else
			{
				printf("目标客户端 %s:%s 不在线\n", p2, p3);
			}
		}
		// 表情包消息
		else if (strcmp(p1, "emoji") == 0)
		{
			// 1. 切割基础信息
			char *p2 = strtok(NULL, "@"); // 对方ip
			char *p3 = strtok(NULL, "@"); // 对方端口
			char *p4 = strtok(NULL, "@"); // 表情包大小
			long size = atoi(p4);
			printf("收到表情包消息: 发送方=%s:%hu, 接收方=%s:%s, 大小=%ld\n", 
				   p->ip, p->port, p2, p3, size);

			// 2. 找到目标客户端
			struct clientmsg *temp = myhead;
			struct clientmsg *target_client = NULL;
			while (temp->next != NULL)
			{
				temp = temp->next;
				if (strcmp(temp->ip, p2) == 0 && temp->port == atoi(p3))
				{
					target_client = temp;
					break;
				}
			}

			if (target_client != NULL)
			{
				// 3. 先给目标客户端发送表情包基础信息
				bzero(clientbuf, 2048);
				sprintf(clientbuf, "%s@%s@%hu@%ld", p1, p->ip, p->port, size);
				send(target_client->sock, clientbuf, strlen(clientbuf), 0);

				// 4. 接收发送方客户端的表情包二进制数据
				char *emoji_buf = malloc(size);
				if (emoji_buf == NULL)
				{
					perror("malloc emoji buf failed");
					continue;
				}
				long remaining = size;
				char *ptr = emoji_buf;
				int recv_len;
				while (remaining > 0)
				{
					recv_len = recv(p->sock, ptr, remaining, 0);
					if (recv_len <= 0)
					{
						perror("recv emoji data failed");
						free(emoji_buf);
						emoji_buf = NULL;
						break;
					}
					remaining -= recv_len;
					ptr += recv_len;
				}

				// 5. 转发二进制数据给目标客户端
				if (emoji_buf != NULL)
				{
					send(target_client->sock, emoji_buf, size, 0);
					free(emoji_buf);
				}
			}
			else
			{
				printf("目标客户端 %s:%s 不在线\n", p2, p3);
			}
		}
	}
}

void *send_msgto_client(void *arg)
{
	char ipbuf[20];
	char sbuf[100];
	unsigned short someport;
	while (1)
	{
		bzero(ipbuf, 20);
		bzero(sbuf, 100);
		printf("请输入你要跟哪个客户端聊天，输入这个客户端ip和端口号!\n");
		scanf("%s", ipbuf);
		scanf("%hu", &someport);
		printf("请输入要给这个客户端发生的信息!\n");
		scanf("%s", sbuf);
		// 把这个信息发送给对应的客户端
		// 遍历链表找到你选择的那个客户端
		struct clientmsg *p = myhead;
		while (p->next != NULL)
		{
			p = p->next;
			if (strcmp(p->ip, ipbuf) == 0 && p->port == someport)
				break;
		}
		send(p->sock, sbuf, strlen(sbuf), 0);
	}
}

// 线程任务函数：只要有客户端连接，都自动遍历链表打印
void *show_client(void *arg)
{
	while (1)
	{
		if (linkflag == 1)
		{
			system("clear"); // 清屏
			// 遍历当前这个链表，把目前在线的客户端信息打印一遍，方便我选择究竟要跟谁聊天
			struct clientmsg *p = myhead;
			while (p->next != NULL)
			{
				p = p->next;
				printf("目前在线的客户端%s  %hu\n", p->ip, p->port);
			}
			linkflag = 0;
		}
	}
}

int main()
{
	int ret;
	pthread_t id;
	pthread_t otherid;
	int tcpsock;
	int newsock;
	char rbuf[100];
	// 定义ipv4地址结构体变量存放客户端需要绑定的ip和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr, sizeof(bindaddr));
	bindaddr.sin_family = AF_INET;							// 地址协议，IPV4
	bindaddr.sin_addr.s_addr = inet_addr("192.168.11.150"); // 绑定服务器自己的ip地址
	bindaddr.sin_port = htons(30000);						// 服务器要绑定的端口号

	// 定义ipv4地址结构体变量保存连接成功的那个客户端信息
	struct sockaddr_in clientaddr;
	int size = sizeof(clientaddr);

	// 定义属性变量并初始化
	pthread_attr_t myattr;
	pthread_attr_init(&myattr);

	// 调用设置分离属性的函数
	pthread_attr_setdetachstate(&myattr, PTHREAD_CREATE_DETACHED); // 可分离

	// 初始化链表的头结点
	myhead = client_init();

	// 初始化用户管理系统
	load_users_from_file("./users.txt");

	// 初始化互斥锁
	pthread_mutex_init(&user_mutex, NULL);

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

	// 创建一个线程，专门负责遍历打印在线客户端(如果有新的客户端连接就打印)
	pthread_create(&otherid, NULL, show_client, NULL);

	// 创建一个线程：从键盘输入内容发送给对应的客户端
	pthread_create(&otherid, NULL, send_msgto_client, NULL);

	while (1)
	{
		// 接收客户端的连接请求，产生新的套接字
		newsock = accept(tcpsock, (struct sockaddr *)&clientaddr, &size);
		// printf("目前连接成功的那个客户端ip是: %s  端口号是: %hu\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));

		// 准备新的节点
		struct clientmsg *newnode = malloc(sizeof(struct clientmsg));
		newnode->sock = newsock;							 // 存放套接字
		strcpy(newnode->ip, inet_ntoa(clientaddr.sin_addr)); // 存放ip地址
		newnode->port = ntohs(clientaddr.sin_port);			 // 存放端口号
		newnode->next = NULL;

		// 客户端的信息尾插到链表
		insert_tail(newnode, myhead);

		// 标志位修改
		linkflag = 1;

		// 立马新建一个线程(分离的)，该线程专门负责接收这个客户端发送过来的信息
		pthread_create(&id, &myattr, recv_client_msg, newnode);
	}
	// 清理资源
	close(tcpsock);
	close(newsock);
	pthread_mutex_destroy(&user_mutex);
	return 0;
}