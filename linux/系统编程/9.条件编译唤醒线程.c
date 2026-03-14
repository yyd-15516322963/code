#include "myhead.h"

/*
    条件变量可以唤醒(解除阻塞)线程
         pthread_cond_signal(pthread_cond_t *cond);     唤醒某个线程
         pthread_cond_broadcast(pthread_cond_t *cond);  唤醒所有的线程
      1.条件变量唤醒线程的过程
              解除条件变量的阻塞，然后立马上锁
*/

// 定义全局变量记录当前车次剩余的票数
int ticketnum = 7;
// 定义互斥锁变量
pthread_mutex_t mutex;
// 定义条件变量
pthread_cond_t cond;

// 售票窗口1
void *win1(void *arg)
{
    while (1)
    {
        // 上锁
        pthread_mutex_lock(&mutex);
        // 判断票卖完了，阻塞线程，暂停售票
        if (ticketnum <= 0)
        {
            // 利用条件变量阻塞当前线程
            printf("窗口1暂停售票，阻塞在wait的位置了!\n");
            pthread_cond_wait(&cond, &mutex); // 先解锁(防止出现死锁的现象)，然后阻塞当前线程
        }
        ticketnum--;
        printf("售票窗口1卖出一张票，剩余票数是%d!\n", ticketnum);
        sleep(1);
        // 解锁
        pthread_mutex_unlock(&mutex);
        usleep(500); // 延时500微秒
    }
    pthread_exit(NULL);
}

// 售票窗口2
void *win2(void *arg)
{
    while (1)
    {
        // 上锁
        pthread_mutex_lock(&mutex);
        // 判断票卖完了，阻塞线程，暂停售票
        if (ticketnum <= 0)
        {
            // 利用条件变量阻塞当前线程
            printf("窗口2暂停售票，阻塞在wait的位置了!\n");
            pthread_cond_wait(&cond, &mutex);
        }
        ticketnum--;
        printf("售票窗口2卖出一张票，剩余票数是%d!\n", ticketnum);
        sleep(1);
        // 解锁
        pthread_mutex_unlock(&mutex);
        usleep(500); // 延时500微秒
    }
    pthread_exit(NULL);
}

// 售票窗口3
void *win3(void *arg)
{
    while (1)
    {
        // 上锁
        pthread_mutex_lock(&mutex);

        // 判断票卖完了，阻塞线程，暂停售票
        if (ticketnum <= 0)
        {
            // 利用条件变量阻塞当前线程
            printf("窗口3暂停售票，阻塞在wait的位置了!\n");
            pthread_cond_wait(&cond, &mutex);
        }
        ticketnum--;
        printf("售票窗口3卖出一张票，剩余票数是%d!\n", ticketnum);
        sleep(1);
        // 解锁
        pthread_mutex_unlock(&mutex);
        usleep(500); // 延时500微秒
    }
    pthread_exit(NULL);
}

// 售票窗口4
void *win4(void *arg)
{
    while (1)
    {
        // 上锁
        pthread_mutex_lock(&mutex);
        // 判断票卖完了，阻塞线程，暂停售票
        if (ticketnum <= 0)
        {
            // 利用条件变量阻塞当前线程
            printf("窗口4暂停售票，阻塞在wait的位置了!\n");
            pthread_cond_wait(&cond, &mutex);
        }
        ticketnum--;
        printf("售票窗口4卖出一张票，剩余票数是%d!\n", ticketnum);
        sleep(1);
        // 解锁
        pthread_mutex_unlock(&mutex);
        usleep(500); // 延时500微秒
    }
    pthread_exit(NULL);
}

// 售票窗口5
void *win5(void *arg)
{
    while (1)
    {
        // 上锁
        pthread_mutex_lock(&mutex);
        // 判断票卖完了，阻塞线程，暂停售票
        if (ticketnum <= 0)
        {
            // 利用条件变量阻塞当前线程
            printf("窗口5暂停售票，阻塞在wait的位置了!\n");
            pthread_cond_wait(&cond, &mutex);
        }
        ticketnum--;
        printf("售票窗口5卖出一张票，剩余票数是%d!\n", ticketnum);
        sleep(1);
        // 解锁
        pthread_mutex_unlock(&mutex);
        usleep(500); // 延时500微秒
    }
    pthread_exit(NULL);
}

int main() // 是个进程，叫做主线程
{
    pthread_t id1, id2, id3, id4, id5;

    // 初始化互斥锁
    pthread_mutex_init(&mutex, NULL);

    // 初始化条件变量
    pthread_cond_init(&cond, NULL);

    // 创建五个线程，代表五个售票窗口
    pthread_create(&id1, NULL, win1, NULL);
    pthread_create(&id2, NULL, win2, NULL);
    pthread_create(&id3, NULL, win3, NULL);
    pthread_create(&id4, NULL, win4, NULL);
    pthread_create(&id5, NULL, win5, NULL);

    // 主程序模拟铁道部--》放票出来
    while (1)
    {
        if (ticketnum <= 0)
        {
            printf("领导，票卖完了，您重新输入新的票数!\n");
            scanf("%d", &ticketnum);
            // 唤醒某个线程--》让这个线程开始工作售票
            if (ticketnum <= 5 && ticketnum >= 0)
                pthread_cond_signal(&cond);
            // 唤醒所有阻塞的线程--》让五个线程开始工作售票
            else if(ticketnum > 5)
                pthread_cond_broadcast(&cond);
        }
    }
    // 回收子线程
    pthread_join(id1, NULL);
    pthread_join(id2, NULL);
    pthread_join(id3, NULL);
    pthread_join(id4, NULL);
    pthread_join(id5, NULL);

    // 销毁互斥锁
    pthread_mutex_destroy(&mutex);

    return 0;
}