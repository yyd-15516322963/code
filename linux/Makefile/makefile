# 第二版本    
# TARGET = 2.作业递归读取目录文件
# SRCS = 2.作业递归读取目录文件.c
# CC = gcc

# $(TARGET):$(SRCS)
# 	$(CC) $(SRCS) -o $(TARGET)
# 	./$(TARGET)

# 第三版本
# TARGET = main
# SRC = $(wildcard *.c)
# CC = gcc

# $(TARGET):$(SRC)
# 	$(CC) $(SRC) -o $(TARGET)
# 	./$(TARGET)


# 第四版本
#生成中间.o文件

#定义变量存放目标
TARGET=main
#定义变量存放依赖
SRC=$(wildcard *.c)
#定义变量存放生成的.o文件的名字
#把$(SRC)变量里面存放的.c文件编译得到对应的.o文件
OBJ=$(patsubst %.c, %.o, $(SRC)) 
#定义变量存放编译器
CC=gcc

CFLAGS=-Wall -g -D_GNU_SOURCE

$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)
	

# 第五版本
#伪目标  没有依赖的目标就是伪目标
clean:
	rm -f $(TARGET) $(OBJ)
