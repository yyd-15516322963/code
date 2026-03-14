#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/*
    1.实际一般都是添加()清晰表达你的计算过程
        int n=8;
            n<<2+1
    2.编程技巧
        #define N 20
        typedef int datatype;
*/
// 编程技巧
#define N 20
typedef int datatype;

// 定义结构体来表示顺序表
struct seqlist
{
    // 存放真实数据的数组
    datatype buf[N];
    // 记录最后有效成员下标
    int last;
};

// 封装函数-->实现顺序表的初始化
struct seqlist *list_init()
{
    // struct seqlist list;  //局部变量-->作用域只能在当前函数中使用-->其他的函数无法使用
    // 解决方法1: 定义全局变量struct seqlist list;
    // 解决方法2: 定义成指针,然后分配堆空间,返回该指针
    struct seqlist *list = malloc(sizeof(struct seqlist));
    // 清空数组
    bzero(list->buf, sizeof(list->buf));
    // 初始化last
    list->last = -1;
    printf("你初始化好的顺序表地址:  %p\n", list);
    return list;
}

// 尾插
void list_add_tail(datatype newdata, struct seqlist *somelist)
{
    // 判断顺序表是否满了
    if (somelist->last < N - 1)
    {
        // 把新的数据插入尾部
        somelist->buf[++(somelist->last)] = newdata;
    }
    else
    {
        printf("对不起，顺序表已满，无法存放!\n");
        return;
    }
}

/*
    删除
       1.如果deldata不存在,打印提醒
       2.如果存在deldata,并且有多个,你全部删除
       3.判断顺序表是否为空
*/
void list_remove(datatype deldata, struct seqlist *somelist)
{
    int i;
    int flag;
    // 遍历找到要删除的数据
    for (i = 0; i <= somelist->last; i++)
    {
        if (somelist->buf[i] == deldata)
        {
            flag = 1;
            for (int j = i; j < somelist->last; j++)
            {
                somelist->buf[j] = somelist->buf[j + 1];
            }
            i--;
            printf("没有这个元素 无法删除\n");
            somelist->last--;
        }
    }
}

// 修改元素
void list_update(datatype olddata, datatype newdata, struct seqlist *somelist)
{
    int i;
    // 遍历找到要删除的数据 
    for (i = 0; i <= somelist->last; i++)
    {
        if (somelist->buf[i] == olddata) 
        {
            somelist->buf[i] = newdata;
        }
    }
}

// 查询打印
void list_show(struct seqlist *somelist)
{
    int i;
    for (i = 0; i <= somelist->last; i++)
        // if (somelist->buf[i] != 0)
        // {
        printf("目前顺序表存放的是: %d\n", somelist->buf[i]);
    // }
}

// 销毁顺序表
void list_uninit(struct seqlist *somelist)
{
    free(somelist);
}

int main()
{
    // 初始化顺序表
    struct seqlist *mylist = list_init();
    printf("你初始化好的顺序表地址:  %p\n", mylist);

    // 尾插数据
    list_add_tail(10, mylist);
    list_add_tail(10, mylist);
    list_add_tail(56, mylist);
    list_add_tail(66, mylist);
    list_add_tail(89, mylist);
    list_add_tail(3, mylist);

    // 删除
    // list_remove(66, mylist);
    // list_remove(6, mylist);

    // // 修改
    // list_update(10, 20, mylist);

    // 遍历
    list_show(mylist);

    // 释放顺序表
    list_uninit(mylist);
    return 0;
}