#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	单链表实现队列的逻辑功能
	重点:
	    1.用单链表实现队列的逻辑,写法不唯一
		  彭老师目前的写法是尾部当成队尾,头结点的下一个当成是队首
		2.入队和出队
		  入队:在队尾的后面入队,然后更新队尾
		  出队:从队首位置出队,队首固定是头结点的下一个
		  
*/
//定义结构体表示队列
struct queue
{
	//数据域-->队列需要存放什么数据,你就写什么数据
	int data;
	struct queue *queuetail;  //记录队尾位置,方便我入队
	//指针域
	struct queue *next;
};

//初始化队列
struct queue *queue_init()
{
	struct queue *head=malloc(sizeof(struct queue));
	head->queuetail=head;  //最开始队尾停在头结点的位置
	head->next=NULL;
	return head;
}

/*
	入队操作的函数
	参数: 把newdata入队到head代表的队列里面
*/
void in_queue(int newdata,struct queue *head)
{
	//准备新的节点
	struct queue *newnode=malloc(sizeof(struct queue));
	newnode->data=newdata;
	newnode->next=NULL;
	
	//把新的节点存放到queuetail的后面,然后更新queuetail即可
	head->queuetail->next=newnode;
	//更新queuetail
	head->queuetail=newnode;
	
}

//出队操作的函数
int out_queue(struct queue *head)
{
	//判断队列是否为空
	if(head->next==NULL)
	{
		printf("对不起,队列现在为空,无法出队新的数据!\n");
		return -1;
	}
	//备份队首元素的值
	int temp=head->next->data;
	
	//删除队首元素
	struct queue *p=head->next;  //p指向队首
	head->next=p->next;
	p->next=NULL;
	free(p);
	return temp;
}
int main()
{
	int result;
	//初始化队列 
	struct queue *myqueue=queue_init();
	
	//入队几个数据
	in_queue(15,myqueue);
	in_queue(16,myqueue);
	in_queue(17,myqueue);
	in_queue(18,myqueue);
	
	//出队
	while((result=out_queue(myqueue))!=-1)
	{
		printf("目前出队的数据是: %d\n",result);
	}
	
	return 0;
}

