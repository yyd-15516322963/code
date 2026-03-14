#include "myhead.h"

/*
	进程的退出与回收
		1.pid_t wait(int *stat_loc);
			  返回值:
				参数: stat_loc -->存放子进程退出时候的状态信息
								 子进程退出时候的状态信息包括: 退出值
														  子进程是正常退出还是异常退出
															   正常退出: 代码按照逻辑顺序一步步执行,然后顺理成章的调用exit结束
															   异常退出: 代码执行地好好的,突然被外界干扰弄死了
																	   我们可以通过kill命令给子进程发送杀死信号,把子进程弄死
																			kill  -9  子进程的ID号
																			9号信号就是杀死信号
																			kill -l   查看linux所有的信号
																	   如何获取子进程的ID号呢?
																			pid_t getpid(void) 返回值就是当前进程的ID号
														  子进程是异常退出,什么原因导致的(什么信号把子进程弄死的)
								 退出值仅仅只是状态信息的其中一部分
		   //结束子进程
		   exit(123);  //123是退出值

		   父进程现在回收子进程
		   int status;
		   wait(&status);
*/
int main()
{
	pid_t id;
	int status;
	// 创建一个新的进程 -->子进程
	id = fork();
	if (id == 0) // 子进程
	{
		int i;
		for (i = 0; i < 10; i++)
		{
			printf("子进程执行打印hello你好粤嵌,子进程的ID号是: %d!\n", getpid());
			sleep(1);
		}
		// 结束子进程
		exit(123); // exit参数的取值范围是0---254之间
	}

	// 父进程现在回收子进程
	pid_t pid = wait(&status); // wait函数会阻塞父进程,等待子进程退出,然后回收子进程
				   // 如果子进程一直都不退出,wait会一直阻塞父进程
	/*
		刚才老师说了: status存放的是子进程退出的状态信息,退出值123仅仅只是状态信息的一部分
		现在我故意让子进程异常退出(kill发送杀死信号),我想要判断子进程究竟是正常退出了,还是异常退出了
	*/
	printf("父进程已经成功回收了子进程的资源,status的值是: %d\n",pid);
	// if (WIFEXITED(status)) // 判断子进程正常退出
	// {
	// 	printf("子进程是正常退出的!\n");
	// 	printf("我回收的子进程退出值是: %d\n", WEXITSTATUS(status));
	// }
	// if (WIFSIGNALED(status)) // 判断子进程被信号弄死
	// {
	// 	printf("爹地,我死得好惨,我不是正常结束,有人在搞我!\n");
	// 	// 找出真凶
	// 	printf("放心,凶手我找到了,它是: %d\n", WTERMSIG(status));
	// }
	return 0;
}