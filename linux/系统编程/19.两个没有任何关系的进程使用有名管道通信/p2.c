#include "myhead.h"

/*
	有名管道实现没有任何血缘关系的进程间通信
	    p1发送(通过有名管道)信息给p2
		   int mkfifo(const char *pathname, mode_t mode);
                 返回值: 成功  0  失败  -1
				  形参:  pathname -->你要新建的有名管道路径名
				        mode -->表示有名管道的权限0777
		1.linux中的七种类型的文件,其中管道指的就是有名管道
		  共享文件夹无法创建有名管道-->原因:有名管道属于linux中第一种文件类型,windows系统不支持这种文件类型
		                               有名管道只能在纯粹的linux环境下创建使用
		2.有名管道只有一个文件描述符,既可以读,也可以写							   
		  
*/

int main()
{
	int ret;
	int fd;
	char rbuf[100];
	
	//判断有名管道是否已经存在
	if(access("/home/gec/myfifo",F_OK)!=0)  //不存在
	{
		//第一步:创建有名管道
		ret=mkfifo("/home/gec/myfifo",0777); //我要在家目录新建一个有名管道文件,名字叫做myfifo
		if(ret==-1)
		{
			perror("创建有名管道失败了!\n");
			return -1;
		}
	}
	
	//第二步:打开有名管道
	fd=open("/home/gec/myfifo",O_RDWR);
	if(fd==-1)
	{
		perror("打开有名管道失败了!\n");
		return -1;
	}
	
	//第三步:读取有名管道中的数据
	while(1)
	{
		bzero(rbuf,100);
		//读取有名管道中的数据
		read(fd,rbuf,100);
		printf("p2读取到的信息是: %s\n",rbuf);
	}
	
	//第四步:关闭有名管道
	close(fd);
	return 0;
}