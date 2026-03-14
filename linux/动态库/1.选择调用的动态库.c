#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char const *argv[])
{
    void *hander = dlopen("/mnt/d/code/linux/动态库/liblcd.so",RTLD_LAZY);
    if(hander == NULL)
    {
        perror("打开动态库失败！");
        return -1;
    }

    void *fun = dlsym(hander, "open_lcd");

    void (*p)() = fun;
    p();

    dlclose(hander);

    return 0;
}
