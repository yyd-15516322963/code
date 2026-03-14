#include "myhead.h" //我自定义的头文件，里面包含了所有常用的头文件

/*
	多个客户端连接服务器
*/
int newsock;

struct siglelist
{
	struct sockaddr_in clientaddr;
	struct siglelist *next;
};

struct siglelist *init()
{
	struct siglelist *headnode = malloc(sizeof(struct siglelist));
	headnode->clientaddr.sin_addr;
	headnode->clientaddr.sin_port = NULL;
	headnode->next = NULL;
	return headnode;
}
void list_tail(struct sockaddr_in clientaddr, struct siglelist *head)
{
	struct siglelist *newnode = malloc(sizeof(struct siglelist));
	newnode->next = NULL;

	struct siglelist *p = head;
	while (p->next != NULL)
	{
		p = p->next;
	}

	p->next = newnode;
}

void *recv_client(void *arg)
{

}

int main()
{
	int ret;
	pthread_t id;
	int tcpsock;
	char rbuf[100];
	// 定义ipv4地址结构体变量存放客户端需要绑定的ip和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr, sizeof(bindaddr));
	bindaddr.sin_family = AF_INET;						  // 地址协议，IPV4
	bindaddr.sin_addr.s_addr = inet_addr("192.168.11.4"); // 绑定服务器自己的ip地址
	bindaddr.sin_port = htons(30000);					  // 服务器要绑定的端口号

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

	while (1)
	{
		// 接收客户端的连接请求，产生新的套接字
		newsock = accept(tcpsock, (struct sockaddr *)&clientaddr, &size);
		printf("目前连接成功的那个客户端ip是: %s  端口号是: %hu\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 客户端的信息尾插到链表
		struct siglelist *myhead = init();

		list_tail(clientaddr,myhead);

		// 立马新建一个线程，该线程专门负责接收这个客户端发送过来的信息
		pthread_create(id, NULL, recv_client, NULL);
	}
	// 挂机
	close(tcpsock);
	close(newsock);
	return 0;
}