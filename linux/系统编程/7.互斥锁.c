#include "myhead.h"

/*
	完整(上锁和解锁都有)的互斥锁操作写上去
*/ 

//定义全局变量表示目前商品的库存数量
int n=0; 
//定义互斥锁变量
pthread_mutex_t mymutex;

//生产部门 
void *task1(void *arg)
{
	while(1)
	{
		//上锁
		pthread_mutex_lock(&mymutex);
		n++;
		printf("生产部门现在生产一件商品，商品库存是%d!\n",n);
		sleep(1);
		//解锁
		pthread_mutex_unlock(&mymutex);
		//稍微延时一丢丢
		//由于线程1解锁之后存在两种可能性(可能性1：线程1自己把锁上了 可能性2：线程2抢到锁)
		//延时一丢丢为了排除可能性1
		usleep(500); //延时500微秒
	}	
	pthread_exit(NULL);
}

//销售部门 
void *task2(void *arg)
{
	while(1)
	{
		//上锁
		pthread_mutex_lock(&mymutex);
		n--;
		printf("销售部门现在卖出一件商品，商品库存是%d!\n",n);
		sleep(1);
		//解锁
		pthread_mutex_unlock(&mymutex);
		//稍微延时一丢丢
		//由于线程2解锁之后存在两种可能性(可能性1：线程2自己把锁上了 可能性2：线程1抢到锁)
		//延时一丢丢为了排除可能性1
		usleep(500); //延时500微秒
	}	
	pthread_exit(NULL);
}

int main()   //是个进程，叫做主线程   
{
	pthread_t id1,id2;
	
	//初始化互斥锁
	pthread_mutex_init(&mymutex,NULL);
	

	//创建两个线程，分别代表生产和销售
	//生产部门 
	pthread_create(&id1,NULL,task1,NULL);
	sleep(1); //耍点手段确保一定是生产部门先执行，先上锁
	//销售部门 
	pthread_create(&id2,NULL,task2,NULL); 

	//回收子线程
	pthread_join(id1,NULL); 
	pthread_join(id2,NULL);
	
	//销毁互斥锁
	pthread_destroy(&mymutex);
	
	return 0;
}