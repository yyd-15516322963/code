#include "myhead.h" //我自定义的头文件，里面包含了所有常用的头文件

/*
	多个客户端连接服务器
*/

// 接收参数：消息内容 + 目标IP + 目标端口（用结构体封装）
typedef struct
{
	char target_ip[20];
	unsigned short target_port;
	char msg[100];
} SendArgs;

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

void *send_msgto_client1(void *arg)
{
	int fd1;
	char buf1[100];
	int read_len = 0;

	struct clientmsg *client = (struct clientmsg *)arg;

	fd1 = open("./1.txt", O_RDWR);
	if (fd1 == -1)
	{
		perror("创建文件失败！\n");
	}
	read_len = read(fd1, buf1, sizeof(buf1) - 1);
	int send_len = send(client->sock, buf1, read_len, 0);
	if (send_len > 0)
	{
		printf("成功向客户端 %s:%hu 发送1.txt内容，长度：%d\n",
			   client->ip, client->port, send_len);
	}

	// 释放参数内存
	pthread_exit(NULL); // 线程正确退出
	close(fd1);
}

// 线程的任务函数：专门负责接收某个客户端发送过来的信息
void *recv_client_msg(void *arg)
{
	int ret;
	int fd1;
	char buf1[100];
	pthread_t otherid;
	struct clientmsg *p = (struct clientmsg *)arg;
	char rbuf[100];
	while (1)
	{
		bzero(rbuf, 100);
		// 接收客户端的信息
		ret = recv(p->sock, rbuf, 100, 0);
		if (ret == 0)
		{
			printf("客户端ip %s  端口号%hu断开了\n", p->ip, p->port);
			// 从单链表中删除这个断开的客户端
			list_delete(p->ip, p->port, myhead);
			// 退出当前线程
			pthread_exit(NULL);
		}
		printf("客户端ip %s  端口号%hu给我发送的信息是： %s\n", p->ip, p->port, rbuf);

		// 检查解析结果是否完整
		if (strcmp(rbuf, "get1.txt") == 0)
		{

			if (pthread_create(&otherid, NULL, send_msgto_client1, p) != 0)
			{
				perror("创建转发线程失败");
			}
			// 设置线程分离，避免资源泄漏
			pthread_detach(otherid);
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
		// 遍历当前这个链表，把目前在线的客户端信息打印一遍，方便我选择究竟要跟谁聊天
		struct clientmsg *p = myhead;
		while (p->next != NULL)
		{
			p = p->next;
			printf("目前在线的客户端%s  %hu\n", p->ip, p->port);
		}
		printf("请输入你要跟哪个客户端聊天，输入这个客户端ip和端口号!\n");
		scanf("%s", ipbuf);
		scanf("%hu", &someport);
		printf("请输入要给这个客户端发生的信息!\n");
		scanf("%s", sbuf);
		// 把这个信息发送给对应的客户端
		// 遍历链表找到你选择的那个客户端
		p = myhead;
		while (p->next != NULL)
		{
			p = p->next;
			if (strcmp(p->ip, ipbuf) == 0 && p->port == someport)
				break;
		}
		send(p->sock, sbuf, strlen(sbuf), 0);
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
	bindaddr.sin_family = AF_INET;				  // 地址协议，IPV4
	bindaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 绑定服务器自己的ip地址
	bindaddr.sin_port = htons(30000);			  // 服务器要绑定的端口号

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

		// 立马新建一个线程(分离的)，该线程专门负责接收这个客户端发送过来的信息
		pthread_create(&id, &myattr, recv_client_msg, newnode);
	}
	// 挂机
	close(tcpsock);
	close(newsock);
	return 0;
}