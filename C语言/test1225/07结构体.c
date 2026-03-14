#include <stdio.h>

typedef int *pint;
//ld 是long double 类型变量的别名, pld 是long double * 类型变量的别名
typedef long double ld, *pld;
// Swap 是 int(*)(int,int) 类型函数指针类型的别名
typedef int (*Swap)(int, int);

struct dog
{
    char *name;
    char color;
};

typedef struct student
{
    char *name;
    int age;
    int gender;
} Student;

// // 结构体可以匿名，但是用typedef必须给他设置别名
typedef struct
{
    char *name;
    int age;
    int gender;
} Teacher;

int main(int argc, char const *argv[])
{
    struct dog wangcai;
    Student stu;

    Teacher tea;
    return 0;
}
