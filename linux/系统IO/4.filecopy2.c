#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>  //read的头文件
#include <strings.h>
/*
	stat获取文件大小拷贝文件				
*/
#define N 10000

int main(int argc,char **argv) //  ./程序名  原文件路径  目标文件路径  
{
	int fd1,fd2;
	struct stat mystat;
	char sbuf[N];
	ssize_t ret;
	int i;
	
	//获取原文件
	stat(argv[1],&mystat);
	
	//打开原文件
	fd1=open("/mnt/d/code/share/1笔记.txt",O_RDWR);
	if(fd1==-1)
	{
		printf("打开原文件失败了\n");
		return -1;
	}
	
	//新建目标文件
	fd2=open("/mnt/d/code/share/2笔记.txt",O_CREAT|O_EXCL|O_RDWR,0777);
	if(fd2==-1)
	{
		printf("新建目标文件失败了\n");
		return -1;
	}
	
	//思路1: 一口气读取整个文件大小(一般适合小文件),然后写入到目标文件
	//依据原文件大小定义数组
	//char sbuf[mystat.st_size];  
	//read(fd1,sbuf,mystat.st_size);
	//写入到目标文件
	//write(fd2,sbuf,mystat.st_size);
	
	//思路2:超大文件,我建议大家采用循环分段拷贝
	int n1=(mystat.st_size)/N;
	int n2=(mystat.st_size)%N;
	
	for(i=0; i<n1; i++)
	{
		bzero(sbuf,N);
		//分段读取原文件内容
		read(fd1,sbuf,N);
		//写入到目标文件
		write(fd2,sbuf,N);
	}
	//把余数读取写入
	bzero(sbuf,N);
	//读取余数
	read(fd1,sbuf,n2);
	write(fd2,sbuf,n2);
		

	//关闭文件
	close(fd1);
	close(fd2);
	return 0;
}