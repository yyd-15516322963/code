#include "myhead.h"  //我自定义的头文件，里面包含了所有常用的头文件

/*
	http请求获取阿里云市场提供的天气预报信息
	   1.post请求，C语言代码格式如下
	          POST /路径  HTTP/1.1\r\n
              Host:网址\r\n              (注意网址前面的http:\\不要写，并且.com后面的路径不要写，不要漏掉\r\n)
              Content-Type:application/x-www-form-urlencoded\r\n   字符编码格式(URL格式)
              Content-Length:25\r\n   数据内容的长度(一定是真实长度，不能多不能少)
                                            比如：在下面这个例子中数据内容是expressNo=YT8796074334503
                                                       expressNo=YT8796074334503的总长度就是25个字节
              Authorization:APPCODE 验证码\r\n\r\n
              expressNo=YT8796074334503  你要发送的真实数据内容
	    总结：前面每一句话后面都必须有\r\n,只有最后一句没有
*/   

int main()
{
	int ret;
	char rbuf[30000]={0};
	char ipbuf[20]={0};
	int tcpsock;
	
	//定义ipv4地址结构体变量存放客户端需要绑定的ip和端口号
	struct sockaddr_in bindaddr;
	bzero(&bindaddr,sizeof(bindaddr));
	bindaddr.sin_family=AF_INET;  //地址协议，IPV4
	bindaddr.sin_addr.s_addr=htonl(INADDR_ANY);    //绑定客户端自己的ip地址
	bindaddr.sin_port=htons(20000);  //客户端要绑定的端口号
	
	//获取天气预报网站的ip地址
	struct hostent *result=gethostbyname("kzqexpress.market.alicloudapi.com");
	strcpy(ipbuf,inet_ntoa(*((struct in_addr *)((result->h_addr_list)[0]))));
	printf("天气预报网站对应的ip地址是: %s\n",ipbuf);
	
	//定义ipv4地址结构体变量存放服务器的ip和端口号
	struct sockaddr_in serveraddr;
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;  //地址协议，IPV4
	serveraddr.sin_addr.s_addr=inet_addr(ipbuf);    //天气预报网站的ip地址
	serveraddr.sin_port=htons(80);  //http请求，默认使用的端口号就是80
	
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
	
	//连接服务器--》由于我这个例子是要获取天气预报信息，因此我要连接就是天气预报网站对应的服务器
	ret=connect(tcpsock,(struct sockaddr *)&serveraddr,sizeof(serveraddr));
	if(ret==-1)
	{
		perror("连接服务器失败了!\n");
		return -1;
	}
	
	//发送请求给天气预报的服务器-->服务商要求发生post请求
	// https://kzqexpress.market.alicloudapi.com/api/express/query
	const char *postreq="POST /api/express/query HTTP/1.1\r\n"
	                    "Host:kzqexpress.market.alicloudapi.com\r\n"
						"Content-Type:application/x-www-form-urlencoded\r\n"
						"Content-Length:25\r\n"
						"Authorization:APPCODE 442b565c2b4743b38c410a5b74fcbbe7\r\n\r\n"
						"expressNo=YT7606790768919";
	send(tcpsock,postreq,strlen(postreq),0);
	
	//立马接受服务器返回的json格式数据
	recv(tcpsock,rbuf,30000,0);
	printf("杭州快证签公司服务器给我返回的快递物流信息如下: %s\n",rbuf);

	//挂机
	close(tcpsock);
	return 0;
}