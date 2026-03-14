#include "myhead.h"

/*
	引入条件变量
	   1.举例子--》火车站买票
	       创建五个线程--》模拟五个售票窗口
		   全局变量--》记录剩余票数
	   2.代码有问题
	         问题1：余票卖完了，五个线程应该暂停卖票
			 答案：使用我接下来讲解的条件变量即可解决问题
			      满足某个条件的时候(程序员依据自己的代码需要写条件判断，然后利用条件变量阻塞/唤醒线程)，可以用条件变量去阻塞或者唤醒(解除阻塞)线程
				       比如：彭老师现在这个例子，我希望ticketnum变成0的时候，阻塞五个线程(暂停售票)
				       条件变量可以阻塞线程
					   条件变量可以唤醒(解除阻塞)线程
			 
			 
			       如果放出新的票，五个线程应该唤醒，接着卖票
	    3.条件变量使用的思路
		     第一步：定义条件变量并初始化
			        pthread_cond_t cond;
					pthread_cond_init(&cond, NULL);
			 第二步：使用条件变量
    			    阻塞线程
					  pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
					         参数：cond --》你刚才初始化好的条件变量
							      mutex --》互斥锁？为什么条件变量需要跟互斥锁配合使用
								            答案：pthread_cond_wait这个函数做了两件事情，先解锁，然后阻塞当前线程

             第三步：销毁条件变量
			        pthread_cond_destroy(&cond);
*/ 

//定义全局变量记录当前车次剩余的票数
int ticketnum=10; 
//定义互斥锁变量
pthread_mutex_t mutex;
//定义条件变量
pthread_cond_t cond;

//售票窗口1
void *win1(void *arg)
{
	while(1)
	{
		//上锁
		pthread_mutex_lock(&mutex);
		ticketnum--;
		//判断票卖完了，阻塞线程，暂停售票
		if(ticketnum<=3)
		{
			//利用条件变量阻塞当前线程
			printf("窗口1暂停售票，阻塞在wait的位置了!\n");
			pthread_cond_wait(&cond,&mutex); //先解锁(防止出现死锁的现象)，然后阻塞当前线程
		}
		printf("售票窗口1卖出一张票，剩余票数是%d!\n",ticketnum);
		sleep(1);
		//解锁
		pthread_mutex_unlock(&mutex);
		usleep(500); //延时500微秒
	}	
	pthread_exit(NULL);
}

//售票窗口2
void *win2(void *arg)
{
	while(1)
	{
		//上锁
		pthread_mutex_lock(&mutex);
		ticketnum--;
		//判断票卖完了，阻塞线程，暂停售票
		if(ticketnum<=3)
		{
			//利用条件变量阻塞当前线程
			printf("窗口2暂停售票，阻塞在wait的位置了!\n");
			pthread_cond_wait(&cond,&mutex);
		}
		printf("售票窗口2卖出一张票，剩余票数是%d!\n",ticketnum);
		sleep(1);
		//解锁
		pthread_mutex_unlock(&mutex);
		usleep(500); //延时500微秒
	}	
	pthread_exit(NULL);
}

//售票窗口3
void *win3(void *arg)
{
	while(1)
	{
		//上锁
		pthread_mutex_lock(&mutex);
		ticketnum--;
		//判断票卖完了，阻塞线程，暂停售票
		if(ticketnum<=3)
		{
			//利用条件变量阻塞当前线程
			printf("窗口3暂停售票，阻塞在wait的位置了!\n");
			pthread_cond_wait(&cond,&mutex);
		}
		printf("售票窗口3卖出一张票，剩余票数是%d!\n",ticketnum);
		sleep(1);
		//解锁
		pthread_mutex_unlock(&mutex);
		usleep(500); //延时500微秒
	}	
	pthread_exit(NULL);
}

//售票窗口4
void *win4(void *arg)
{
	while(1)
	{
		//上锁
		pthread_mutex_lock(&mutex);
		ticketnum--;
		//判断票卖完了，阻塞线程，暂停售票
		if(ticketnum<=3)
		{
			//利用条件变量阻塞当前线程
			printf("窗口4暂停售票，阻塞在wait的位置了!\n");
			pthread_cond_wait(&cond,&mutex);
		}
		printf("售票窗口4卖出一张票，剩余票数是%d!\n",ticketnum);
		sleep(1);
		//解锁
		pthread_mutex_unlock(&mutex);
		usleep(500); //延时500微秒
	}	
	pthread_exit(NULL);
}

//售票窗口5
void *win5(void *arg)
{
	while(1)
	{
		//上锁
		pthread_mutex_lock(&mutex);
		ticketnum--;
		//判断票卖完了，阻塞线程，暂停售票
		if(ticketnum<=3)
		{
			//利用条件变量阻塞当前线程
			printf("窗口5暂停售票，阻塞在wait的位置了!\n");
			pthread_cond_wait(&cond,&mutex);
		}
		printf("售票窗口5卖出一张票，剩余票数是%d!\n",ticketnum);
		sleep(1);
		//解锁
		pthread_mutex_unlock(&mutex);
		usleep(500); //延时500微秒
	}	
	pthread_exit(NULL);
}

int main()   //是个进程，叫做主线程   
{
	pthread_t id1,id2,id3,id4,id5;
	
	//初始化互斥锁
	pthread_mutex_init(&mutex,NULL);
	
	//初始化条件变量
	pthread_cond_init(&cond,NULL);

	//创建五个线程，代表五个售票窗口
	pthread_create(&id1,NULL,win1,NULL);
	pthread_create(&id2,NULL,win2,NULL); 
	pthread_create(&id3,NULL,win3,NULL); 
	pthread_create(&id4,NULL,win4,NULL); 
	pthread_create(&id5,NULL,win5,NULL); 

	//回收子线程
	pthread_join(id1,NULL); 
	pthread_join(id2,NULL);
	pthread_join(id3,NULL); 
	pthread_join(id4,NULL);
	pthread_join(id5,NULL);
	
	//销毁互斥锁
	pthread_mutex_destroy(&mutex);

	return 0;
}