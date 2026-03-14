#include "myhead.h"

int main(int argc, char const *argv[])
{
    pthread_attr_t my_attr2;
    pthread_t tid1,tid2;

    pthread_creat(&tid1,NULL);

    pthread_attr_init(&my_attr2);
    pthread_attr_setdetachstate(&my_attr2,PTHREAD_CREATE_DETACHED);
    pthread_creat(&tid2,&my_attr2);
    pthread_attr_destory(&my_attr2);
    return 0;
}
