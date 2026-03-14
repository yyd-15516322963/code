#ifndef _MYHEAD_H_
#define _MYHEAD_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/mman.h>
#include <errno.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ON 0
#define OFF 1

#define DISP_BUF_SIZE (800 * 480)

// 全局UI对象
lv_obj_t * page1;
lv_obj_t * page2;
lv_obj_t * tab1;
lv_obj_t * tab2;
lv_obj_t * tab3;
lv_obj_t * tab4;
lv_obj_t * tab5;
lv_obj_t * tab6;
lv_obj_t * tab7;
lv_obj_t * tab1_1;
lv_obj_t * tab1_2;
lv_obj_t * tab1_3;
lv_obj_t * tab1_4;
lv_obj_t * tab1_5;
lv_obj_t * tab1_6;
lv_obj_t * tab1_7;
lv_obj_t * tab2_1;
lv_obj_t * tab2_2;
lv_obj_t * tab2_3;
lv_obj_t * tab2_4;
lv_obj_t * tab2_5;
lv_obj_t * bt1;
lv_obj_t * bt2;
lv_obj_t * kb;
lv_obj_t * ta1;
lv_obj_t * ta2;
lv_obj_t * tabview1;
lv_obj_t * tabview2;
lv_obj_t * tabview3;

// 定义结构体存放连接成功的客户端信息
struct clientmsg
{
    // 数据域
    int sock;            // 目前连接成功客户端套接字
    char ip[20];         // 客户端ip地址
    unsigned short port; // 客户端端口号

    // 指针域
    struct clientmsg * next;
};

// TCP全局变量声明（原有）
extern int tcpsock;

// 消息队列结构体（用于TCP接收线程与LVGL主线程通信）
typedef struct
{
    char username[32];          // 消息所属用户（Kai/Leo等）
    char msg[2048];             // 消息内容
    int is_self;                // 是否是自己发送的消息（1:是 0:否）
    int is_emoji;               // 新增：1=表情包，0=文本
    int is_file;            // 是否是文件（新增字段）
    char target_ip[20];         // 目标IP（用于匹配聊天窗口）
    unsigned short target_port; // 目标端口（用于匹配聊天窗口）
} msg_queue_t;

extern msg_queue_t msg_queue[10]; // 消息队列
extern int msg_queue_idx;         // 消息队列索引
extern pthread_mutex_t msg_mutex; // 消息队列互斥锁

// 用户信息结构体
typedef struct
{
    char * nickname;            // 标签页名称（Kai/Leo等）
    char * username;            // 用户名
    char * whisper_num;         // Whisper编号
    char * region;              // 地区
    char * signature;           // 个性签名
    char * img_path;            // 头像路径
    lv_obj_t * msg_container;   // 聊天消息容器
    char target_ip[20];         // 对应聊天对象IP
    unsigned short target_port; // 对应聊天对象端口
} user_info_t;

// // 新增：记录每个聊天窗口的消息数量（用于计算Y坐标）
// typedef struct
// {
//     lv_obj_t * msg_container; // 消息容器
//     int msg_count;            // 消息数量
//     int last_y;               // 最后一条消息的Y坐标
// } chat_window_t;
// extern chat_window_t chat_windows[5]; // 对应5个用户的聊天窗口

// 消息窗口管理结构体
typedef struct {
    lv_obj_t * msg_container;  // 内部消息容器
    lv_obj_t * scroll_container;  // 滚动容器
    int msg_count;             // 消息数量
    int container_height;      // 容器当前高度
    int last_y;                // 最后消息的Y坐标
} msg_window_t;

static msg_window_t msg_windows[5];  // 每个聊天窗口的状态

extern user_info_t user_list[5]; // 5个用户信息

// 函数声明
void update_chat_msg(lv_obj_t * msg_container, const char * msg, int is_self);
uint32_t custom_tick_get(void);
void show_login_page2(void);
void show_login_page1(void); 
void update_chat_emoji(lv_obj_t * msg_container, const char * emoji_path, int is_self);
void chat_send_msg_cb(lv_event_t * e);
void emoji_btn_cb(lv_event_t * e);
void file_btn_cb(lv_event_t * e);
void emoji_btn1_cb(lv_event_t * e);
void emoji_btn2_cb(lv_event_t * e);
void emoji_btn3_cb(lv_event_t * e);
void emoji_close_btn_cb(lv_event_t * e);
void file_close_btn_cb(lv_event_t * e);
void create_chat_page(lv_obj_t * chat_tab, const char * username);
void btn_send_cb(lv_event_t * e);

#endif