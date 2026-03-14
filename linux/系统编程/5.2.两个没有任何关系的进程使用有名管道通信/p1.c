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
        3.有名管道不能重复创建
          解决方法:
             方法1: 利用文件IO学过的函数access判断文件是否存在
			 方法2: system()帮你调用执行shell命令,或者可执行程序
			       system("madplay  /1.mp3");
				   system("mplayer /1.mp4");
			       system("clear");
				   system("rm /home/gec/myfifo");
		  
*/

int main()
{
	int ret;
	int fd;
	char sbuf[100];
	
	//判断有名管道是否已经存在
	if(access("/home/yyd/myfifo",F_OK)!=0)  //不存在
	{
		//第一步:创建有名管道
		ret=mkfifo("/home/yyd/myfifo",0777); //我要在家目录新建一个有名管道文件,名字叫做myfifo
		if(ret==-1)
		{
			perror("创建有名管道失败了!\n");
			return -1;
		}
	}
	
	
	//第二步:打开有名管道
	fd=open("/home/yyd/myfifo",O_RDWR);
	if(fd==-1)
	{
		perror("打开有名管道失败了!\n");
		return -1;
	}
	
	//第三步:往有名管道中写入数据,或者读取有名管道中的数据
	while(1)
	{
		bzero(sbuf,100);
		printf("请输入要发送给p2的信息!\n");
		scanf("%s",sbuf);
		//写入到有名管道中
		write(fd,sbuf,strlen(sbuf));
	}
	
	//第四步:关闭有名管道
	close(fd);
	return 0;
}