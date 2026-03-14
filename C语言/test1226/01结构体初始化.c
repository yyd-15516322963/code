#include <stdio.h>
#include <stdlib.h>

typedef struct dog
{
    int age;
    char *name;
    char *color;
} Dog;

int main(int argc, char const *argv[])
{
    Dog wangcai = {
        4,
        "旺财",
        "白色"};

    Dog d = {
        .color = "黑色",
        .age = 5,
        .name = "111",
    };

    Dog *p = &d;

    printf("%d \n", p->age);
    printf("%s \n", p->color);
    printf("%s \n", p->name);

    Dog *p2 = malloc(sizeof(Dog));
    p2->age = 3;
    p2->color = "黄色";
    p2->name = "333";

    printf("%d \n", p2->age);
    printf("%s \n", p2->color);
    printf("%s \n", p2->name);


    free(p2);

    p2 = NULL;

    return 0;
}
