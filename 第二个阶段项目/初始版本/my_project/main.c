#include "myhead.h"

// TCP全局变量定义
int tcpsock;

// 线程任务函数：接收服务器发送过来的信息
void * recv_servermsg(void * arg)
{
    struct clientmsg * p = (struct clientmsg *)arg;
    char rbuf[2048];
    int ret;
    while(1) {
        bzero(rbuf, sizeof(rbuf));
        // 接收服务器消息
        ret = recv(tcpsock, rbuf, sizeof(rbuf) - 1, 0); // 留1字节防越界
        if(ret <= 0) {
            if(ret == 0)
                printf("服务器断开连接！\n");
            else
                perror("recv失败");
            close(tcpsock);
            tcpsock = -1; // 标记断开
            pthread_exit(NULL);
        }

        printf("收到服务器：%s\n", rbuf);

        // ========== 关键修改：解析msg@IP@PORT@内容格式的消息 ==========
        char * p1 = strtok(rbuf, "@");
        if(strcmp(p1, "msg") == 0) { // 是标准的msg格式消息
            // 分割格式：msg@目标IP@目标端口@消息内容
            char * ip_part     = strtok(NULL, "@"); // 跳过msg@，取IP部分
            char * port_part   = strtok(NULL, "@"); // 取端口部分
            char * msg_content = strtok(NULL, "@"); // 取消息内容

            if(ip_part && port_part && msg_content) {
                unsigned short port = atoi(port_part);
                printf("解析到消息 - IP:%s, 端口:%d, 内容:%s\n", ip_part, port, msg_content);

                pthread_mutex_lock(&msg_mutex);
                // 放入队列，记录IP和端口用于匹配聊天窗口
                strcpy(msg_queue[msg_queue_idx].target_ip, ip_part);
                msg_queue[msg_queue_idx].target_port = port;
                strcpy(msg_queue[msg_queue_idx].msg, msg_content);
                msg_queue[msg_queue_idx].is_self = 0;
                msg_queue_idx                    = (msg_queue_idx + 1) % 10;
                pthread_mutex_unlock(&msg_mutex);
            }
        }
        // ========== 修改：优化表情包接收逻辑 ==========
        else if(strcmp(p1, "emoji") == 0) { // 说明客户端发过来是表情包
            // 继续切割
            char * ip_part   = strtok(NULL, "@"); // 对方ip
            char * port_part = strtok(NULL, "@"); // 对方端口
            char * size_part = strtok(NULL, "@"); // 表情包大小
            if(!ip_part || !port_part || !size_part) {
                printf("表情包消息格式错误\n");
                continue;
            }

            long size           = atol(size_part);
            unsigned short port = atoi(port_part);
            printf("客户端%s:%d发送过来的表情包大小是： %ld\n", ip_part, port, size);

            // ========== 修改：创建表情包接收缓冲区 ==========
            char * emoji_buf = (char *)malloc(size);
            if(!emoji_buf) {
                printf("表情包缓冲区分配失败\n");
                continue;
            }

            // 循环接收表情包数据
            ret                 = 0;
            long remainingbytes = size;      // 存放剩余要接收的字节数
            char * p5           = emoji_buf; // 指向数组的起始位置
            while(1) {
                printf("阻塞recv，剩余接收字节数是: %ld\n", remainingbytes);
                ret = recv(tcpsock, p5, 1024, 0); // 修改：每次接收1024字节更高效
                if(ret <= 0) {
                    perror("接收表情包数据失败");
                    free(emoji_buf);
                    break;
                }
                // 更新剩余字节数
                remainingbytes = remainingbytes - ret;
                // 更新p5
                p5 = p5 + ret;
                printf("接收到表情包数据字节数是: %d\n", ret);
                // 退出条件
                if(remainingbytes <= 0) break;
            }

            // ========== 修改：将表情包信息放入消息队列 ==========
            if(remainingbytes <= 0) {
                pthread_mutex_lock(&msg_mutex);
                // 记录表情包信息
                strcpy(msg_queue[msg_queue_idx].target_ip, ip_part);
                msg_queue[msg_queue_idx].target_port = port;
                strcpy(msg_queue[msg_queue_idx].msg, "[收到表情包]");
                msg_queue[msg_queue_idx].is_self = 0;
                msg_queue_idx                    = (msg_queue_idx + 1) % 10;
                pthread_mutex_unlock(&msg_mutex);

                // 可选：保存表情包文件到本地
                FILE * fp = fopen("recv_emoji.jpg", "wb");
                if(fp) {
                    fwrite(emoji_buf, 1, size, fp);
                    fclose(fp);
                }
            }

            // 释放缓冲区
            free(emoji_buf);
        } else if(strstr(rbuf, "发送的信息:") != NULL) { // 兼容原有格式
            // 切割出真实消息内容
            char * msg_start = strstr(rbuf, "发送的信息:");
            if(msg_start != NULL) {
                msg_start += strlen("发送的信息:"); // 跳过前缀
                if(strlen(msg_start) > 0) {
                    pthread_mutex_lock(&msg_mutex);

                    // 遍历user_list[0-4]，轮询分配消息到不同窗口
                    static int window_index = 0;
                    if(window_index > 4) window_index = 0;

                    // 自动获取当前轮询窗口的IP和端口
                    strcpy(msg_queue[msg_queue_idx].username, user_list[window_index].nickname);
                    strcpy(msg_queue[msg_queue_idx].msg, msg_start);
                    msg_queue[msg_queue_idx].is_self = 0;
                    strcpy(msg_queue[msg_queue_idx].target_ip, user_list[window_index].target_ip);
                    msg_queue[msg_queue_idx].target_port = user_list[window_index].target_port;

                    printf("自动分配消息到%s的窗口（IP:%s, 端口:%d）：%s\n", user_list[window_index].nickname,
                           user_list[window_index].target_ip, user_list[window_index].target_port, msg_start);

                    msg_queue_idx = (msg_queue_idx + 1) % 10;
                    window_index++;

                    pthread_mutex_unlock(&msg_mutex);
                }
            }
        } else if(strstr(rbuf, "@") != NULL) { // 是在线列表
            printf("在线列表：%s\n", rbuf);
        }
    }
}

// 主函数（原有框架，仅新增消息队列处理）
int main(void)
{
    // 初始化互斥锁
    pthread_mutex_init(&msg_mutex, NULL);

    // LVGL初始化（原有）
    lv_init();

    fbdev_init();
    static lv_color_t buf[DISP_BUF_SIZE];
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    disp_drv.hor_res  = 800;
    disp_drv.ver_res  = 480;
    lv_disp_drv_register(&disp_drv);

    evdev_init();
    static lv_indev_drv_t indev_drv_1;
    lv_indev_drv_init(&indev_drv_1);
    indev_drv_1.type    = LV_INDEV_TYPE_POINTER;
    indev_drv_1.read_cb = evdev_read;
    lv_indev_drv_register(&indev_drv_1);

    // ========== 修复：移除lv_tick_set_cb（低版本LVGL不支持） ==========
    // LVGL 7.x 无需显式设置tick回调，主循环调用lv_timer_handler()即可

    // 显示登录页面（原有）
    show_login_page2();

    // ====================== TCP客户端初始化（原有） ======================
    int ret;
    int n;
    char sbuf[100];
    char otherip[20];
    unsigned short otherport;
    char allbuf[2048];
    pthread_t id;

    // 绑定客户端IP和端口
    struct sockaddr_in bindaddr;
    bzero(&bindaddr, sizeof(bindaddr));
    bindaddr.sin_family      = AF_INET;
    bindaddr.sin_addr.s_addr = inet_addr("192.168.11.151");
    bindaddr.sin_port        = htons(10000);

    // 服务器地址配置
    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family      = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("192.168.11.150");
    serveraddr.sin_port        = htons(30000);

    // 创建TCP套接字
    tcpsock = socket(AF_INET, SOCK_STREAM, 0);
    if(tcpsock == -1) {
        perror("买手机失败(创建tcp套接字)失败!\n");
        return -1;
    }

    // 端口复用
    int on = 1;
    setsockopt(tcpsock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // 绑定IP和端口
    ret = bind(tcpsock, (struct sockaddr *)&bindaddr, sizeof(bindaddr));
    if(ret == -1) {
        perror("绑定ip和端口号失败了!\n");
        return -1;
    }

    // 连接服务器
    ret = connect(tcpsock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if(ret == -1) {
        perror("连接服务器失败了!\n");
        return -1;
    }

    // 创建接收线程
    pthread_create(&id, NULL, recv_servermsg, NULL);

    // ====================== 主循环 ======================
    while(1) {
        // 处理LVGL任务（低版本LVGL核心）
        lv_timer_handler();

        // 处理消息队列（更新LVGL界面）
        pthread_mutex_lock(&msg_mutex);
        if(msg_queue_idx > 0) {
            // 关键修复：遍历队列时使用临时索引，避免队列清空导致漏处理
            int current_idx = msg_queue_idx;
            msg_queue_idx   = 0; // 立即清空队列，防止重复处理

            // 主循环中处理消息队列部分
            for(int i = 0; i < current_idx; i++) {
                for(int j = 0; j < 5; j++) {
                    if(strcmp(user_list[j].target_ip, msg_queue[i].target_ip) == 0 &&
                       user_list[j].target_port == msg_queue[i].target_port) {
                        // 判断是否是表情包
                        if(msg_queue[i].is_emoji) {
                            // 显示表情包图片
                            update_chat_emoji(user_list[j].msg_container, msg_queue[i].msg, msg_queue[i].is_self);
                        } else {
                            // 显示文本消息
                            update_chat_msg(user_list[j].msg_container, msg_queue[i].msg, msg_queue[i].is_self);
                        }
                        printf("更新%s的聊天窗口：%s\n", user_list[j].nickname, msg_queue[i].msg);
                        lv_refr_now(NULL);
                        break;
                    }
                }
            }
        }
        pthread_mutex_unlock(&msg_mutex);

        usleep(5000);
    }

    // 清理资源
    close(tcpsock);
    pthread_mutex_destroy(&msg_mutex);

    return 0;
}