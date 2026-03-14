#include "myhead.h"

// TCP全局变量定义
int tcpsock;

// 线程任务函数：接收服务器发送过来的信息
// 线程任务函数：接收服务器发送过来的信息
void * recv_servermsg(void * arg)
{
    char rbuf[2048];
    int ret;
    while(1) {
        bzero(rbuf, 2048);
        ret = recv(tcpsock, rbuf, 2048, 0);
        usleep(1000);
        if(ret == 0) {
            printf("对方(服务器端)断开连接了!\n");
            exit(0);
        }
        if(ret < 0) {
            perror("recv失败");
            exit(0);
        }
        if(strcmp(rbuf, "quit") == 0) exit(0);
        printf("收到的原始信息是: %s\n", rbuf);

        char * p1 = strtok(rbuf, "@");
        
        pthread_mutex_lock(&msg_mutex);
        
        if(strcmp(p1, "msg") == 0) // 文字聊天信息
        {
            char * p2 = strtok(NULL, "@"); // 对方ip
            char * p3 = strtok(NULL, "@"); // 对方端口
            char * p4 = strtok(NULL, "@"); // 真实的信息

            printf("收到文本消息: 来自%s:%s, 内容: %s\n", p2, p3, p4);

            // 将消息放入消息队列
            if(p2 && p3 && p4) {
                strcpy(msg_queue[msg_queue_idx].target_ip, p2);
                msg_queue[msg_queue_idx].target_port = atoi(p3);
                strcpy(msg_queue[msg_queue_idx].msg, p4);
                msg_queue[msg_queue_idx].is_self  = 0;  // 对方发送的消息
                msg_queue[msg_queue_idx].is_emoji = 0;  // 不是表情包
                msg_queue[msg_queue_idx].is_file  = 0;  // 不是文件
                msg_queue_idx = (msg_queue_idx + 1) % 10;
                printf("文本消息放入队列，当前索引: %d\n", msg_queue_idx);
            }
        } 
        else if(strcmp(p1, "file") == 0) // 文件消息
        {
            char * p2 = strtok(NULL, "@"); // 对方ip
            char * p3 = strtok(NULL, "@"); // 对方端口
            char * p4 = strtok(NULL, "@"); // 文件大小
            char * p5 = strtok(NULL, "@"); // 文件类型
            char * p6 = strtok(NULL, "@"); // 文件名
            long size = atol(p4);

            printf("收到文件消息: 来自%s:%s, 大小: %ld字节, 类型: %s, 文件名: %s\n", p2, p3, size, p5, p6);

            // 接收文件数据
            char * buf = malloc(size);
            if(buf == NULL) {
                perror("malloc file buf failed");
                pthread_mutex_unlock(&msg_mutex);
                continue;
            }

            /* 循环接收文件数据 */
            ret                 = 0;
            long remainingbytes = size;
            char * ptr          = buf;
            int total_received  = 0;

            while(remainingbytes > 0) {
                int to_recv = (remainingbytes > 1024) ? 1024 : remainingbytes;
                ret         = recv(tcpsock, ptr, to_recv, 0);
                if(ret <= 0) {
                    perror("recv file data failed");
                    free(buf);
                    buf = NULL;
                    break;
                }
                remainingbytes -= ret;
                total_received += ret;
                ptr += ret;
                printf("接收到文件数据: %d字节, 已接收: %d字节, 剩余: %ld字节\n", ret, total_received, remainingbytes);
            }

            if(buf != NULL && total_received == size) {
                printf("文件接收完成，大小: %d字节\n", total_received);

                // 保存文件
                char filename[256];
                snprintf(filename, sizeof(filename), "./recv_%s", p6);

                int fd = open(filename, O_CREAT | O_TRUNC | O_RDWR, 0777);
                if(fd == -1) {
                    perror("创建文件失败");
                } else {
                    write(fd, buf, size);
                    close(fd);
                    printf("文件保存为: %s\n", filename);
                }

                // 将文件消息放入消息队列
                if(p2 && p3 && p6) {
                    char file_path[256];
                    snprintf(file_path, sizeof(file_path), "/%s", filename);  // 添加路径前缀
                    
                    strcpy(msg_queue[msg_queue_idx].target_ip, p2);
                    msg_queue[msg_queue_idx].target_port = atoi(p3);
                    strcpy(msg_queue[msg_queue_idx].msg, file_path);
                    msg_queue[msg_queue_idx].is_self  = 0;  // 对方发送的文件
                    msg_queue[msg_queue_idx].is_emoji = 0;  // 不是表情包
                    msg_queue[msg_queue_idx].is_file  = 1;  // 是文件
                    msg_queue_idx = (msg_queue_idx + 1) % 10;
                    printf("文件消息放入队列，当前索引: %d\n", msg_queue_idx);
                }

                free(buf);
            } else {
                if(buf) free(buf);
                printf("文件接收不完整，期望%ld字节，实际收到%d字节\n", size, total_received);
            }
        } 
        else if(strcmp(p1, "emoji") == 0) // 表情包消息
        {
            char * p2 = strtok(NULL, "@"); // 对方ip
            char * p3 = strtok(NULL, "@"); // 对方端口
            char * p4 = strtok(NULL, "@"); // 表情包大小
            long size = atol(p4);
            printf("收到的表情包大小是： %ld\n", size);

            // 接收表情包数据
            char * buf = malloc(size);
            if(buf == NULL) {
                perror("malloc emoji buf failed");
                pthread_mutex_unlock(&msg_mutex);
                continue;
            }

            /* 循环接收表情包数据 */
            ret                 = 0;
            long remainingbytes = size;
            char * ptr          = buf;
            int total_received  = 0;

            while(remainingbytes > 0) {
                int to_recv = (remainingbytes > 1024) ? 1024 : remainingbytes;
                ret         = recv(tcpsock, ptr, to_recv, 0);
                if(ret <= 0) {
                    perror("recv emoji data failed");
                    free(buf);
                    buf = NULL;
                    break;
                }
                remainingbytes -= ret;
                total_received += ret;
                ptr += ret;
                printf("接收到表情包数据: %d字节, 已接收: %d字节, 剩余: %ld字节\n", ret, total_received,
                       remainingbytes);
            }

            if(buf != NULL && total_received == size) {
                printf("服务器表情包文件接收完毕！\n");
                // 新建空白的jpeg文件，保存表情包数据
                int fd = open("./recv_emoji.jpg", O_CREAT | O_TRUNC | O_RDWR, 0777);
                if(fd == -1) {
                    perror("新建表情包文件失败");
                } else {
                    write(fd, buf, size);
                    close(fd);
                }
                
                // 将表情包消息放入消息队列
                if(p2 && p3) {
                    strcpy(msg_queue[msg_queue_idx].target_ip, p2);
                    msg_queue[msg_queue_idx].target_port = atoi(p3);
                    strcpy(msg_queue[msg_queue_idx].msg, "[表情包]");
                    msg_queue[msg_queue_idx].is_self  = 0;  // 对方发送的表情包
                    msg_queue[msg_queue_idx].is_emoji = 1;  // 是表情包
                    msg_queue[msg_queue_idx].is_file  = 0;  // 不是文件
                    msg_queue_idx = (msg_queue_idx + 1) % 10;
                    printf("表情包消息放入队列，当前索引: %d\n", msg_queue_idx);
                }
                
                free(buf);
            } else {
                if(buf) free(buf);
                printf("表情包接收不完整，期望%ld字节，实际收到%d字节\n", size, total_received);
            }
        } 
        else {
            printf("收到未知格式的消息: %s\n", p1);
        }
        
        pthread_mutex_unlock(&msg_mutex);
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


    // 显示登录页面
    show_login_page1();

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
        // 处理LVGL任务
        lv_timer_handler();

        // 处理消息队列
        pthread_mutex_lock(&msg_mutex);

        if(msg_queue_idx > 0) {
            int current_size = msg_queue_idx;

            for(int i = 0; i < current_size; i++) {
                lv_obj_t * msg_container = NULL;
                int window_idx           = -1;

                for(int j = 0; j < 5; j++) {
                    if(strcmp(user_list[j].target_ip, msg_queue[i].target_ip) == 0 &&
                       user_list[j].target_port == msg_queue[i].target_port) {
                        msg_container = user_list[j].msg_container;
                        window_idx    = j;
                        break;
                    }
                }

                if(msg_container != NULL && window_idx != -1) {
                    if(msg_queue[i].is_emoji) {
                        // 显示表情包
                        update_chat_emoji(msg_container, "S:/recv_emoji.jpg", msg_queue[i].is_self);
                    } else if(msg_queue[i].is_file) { // 处理文件消息
                        // 文件显示函数
                        printf("开始显示文件消息: %s (is_self=%d)\n", msg_queue[i].msg, msg_queue[i].is_self);
                        update_chat_file(msg_container, msg_queue[i].msg, msg_queue[i].is_self);
                    } else {
                        // 显示文本消息
                        printf("开始显示消息: %s (is_self=%d)\n", msg_queue[i].msg, msg_queue[i].is_self);
                        update_chat_msg(msg_container, msg_queue[i].msg, msg_queue[i].is_self);
                    }
                    printf("更新%s的聊天窗口：%s\n", user_list[window_idx].nickname, msg_queue[i].msg);
                }
            }

            msg_queue_idx = 0;
        }

        pthread_mutex_unlock(&msg_mutex);

        usleep(1000);
    }

    // 清理资源
    close(tcpsock);
    pthread_mutex_destroy(&msg_mutex);

    return 0;
}