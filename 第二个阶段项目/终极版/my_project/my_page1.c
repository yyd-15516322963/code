#include "myhead.h"

// TCP套接字全局变量
int login_sock = -1;

// 连接到服务器的函数
int connect_to_server()
{
    int ret;
    // 定义服务器地址配置
    struct sockaddr_in serveraddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family      = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("192.168.11.150"); // 服务器IP
    serveraddr.sin_port        = htons(30000);                // 服务器端口

    // 创建TCP套接字
    login_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(login_sock == -1) {
        perror("创建套接字失败");
        return -1;
    }

    // 连接服务器
    ret = connect(login_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if(ret == -1) {
        perror("连接服务器失败");
        close(login_sock);
        login_sock = -1;
        return -1;
    }

    printf("连接到服务器成功\n");
    return 0;
}

// 发送注册信息到服务器
int send_register_to_server(const char * name, const char * passwd)
{
    if(login_sock <= 0) {
        if(connect_to_server() != 0) {
            return -1;
        }
    }

    char send_buf[256];
    // 注册消息格式：reg@用户名@密码
    snprintf(send_buf, sizeof(send_buf), "reg@%s@%s", name, passwd);

    int ret = send(login_sock, send_buf, strlen(send_buf), 0);
    if(ret <= 0) {
        perror("发送注册信息失败");
        close(login_sock);
        login_sock = -1;
        return -1;
    }

    printf("注册信息已发送到服务器\n");
    return 0;
}

// 发送登录信息到服务器
int send_login_to_server(const char * name, const char * passwd)
{
    if(login_sock <= 0) {
        if(connect_to_server() != 0) {
            return -1;
        }
    }

    char send_buf[256];
    // 登录消息格式：login@用户名@密码
    snprintf(send_buf, sizeof(send_buf), "login@%s@%s", name, passwd);

    int ret = send(login_sock, send_buf, strlen(send_buf), 0);
    if(ret <= 0) {
        perror("发送登录信息失败");
        close(login_sock);
        login_sock = -1;
        return -1;
    }

    printf("登录信息已发送到服务器\n");
    return 0;
}

// 全局变量用于线程间通信
pthread_mutex_t login_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t login_cond   = PTHREAD_COND_INITIALIZER;
int login_success           = 0;
char login_username[50]     = {0};

// 修改接收服务器响应的线程，添加找回密码响应处理
void * recv_login_response(void * arg)
{
    char rbuf[256];
    int ret;

    while(1) {
        bzero(rbuf, 256);
        ret = recv(login_sock, rbuf, 256, 0);
        if(ret <= 0) {
            printf("服务器断开连接\n");
            close(login_sock);
            login_sock = -1;
            break;
        }

        printf("收到服务器响应: %s\n", rbuf);

        // 解析响应
        char response[256];
        strncpy(response, rbuf, sizeof(response) - 1);

        char * response_type = strtok(response, "@");
        char * status        = strtok(NULL, "@");
        char * username      = strtok(NULL, "@");
        char * message       = strtok(NULL, "@");

        if(strcmp(response_type, "reg_response") == 0) {
            if(status && username && message) {
                if(strcmp(status, "success") == 0) {
                    // 注册成功
                    bt_msgbox_cb4(username);
                } else {
                    // 注册失败
                    bt_msgbox_cb3(username);
                }
            }
        } 
        else if(strcmp(response_type, "login_response") == 0) {
            if(status && username && message) {
                if(strcmp(status, "success") == 0) {
                    // 登录成功
                    printf("登录成功，准备切换到下一个页面\n");

                    // 设置登录成功标志
                    pthread_mutex_lock(&login_mutex);
                    login_success = 1;
                    strncpy(login_username, username, sizeof(login_username) - 1);
                    pthread_cond_signal(&login_cond);
                    pthread_mutex_unlock(&login_mutex);
                } else {
                    // 登录失败
                    printf("登录失败\n");
                    bt_msgbox_cb(NULL);
                }
            }
        }
        else if(strcmp(response_type, "forget_response") == 0) {
            if(status && username && message) {
                if(strcmp(status, "success") == 0) {
                    // 找回密码成功
                    char msg_buf[256];
                    snprintf(msg_buf, sizeof(msg_buf), "Address: %s\n Password: %s", username, message);
                    
                    const char * btns[] = {"", "", ""};
                    lv_obj_t * msgbox = lv_msgbox_create(page1, "Password recovery", msg_buf, btns, true);
                    lv_obj_set_pos(msgbox, 260, 80);
                    
                    printf("找回密码成功: 用户=%s, 密码=%s\n", username, message);
                } else {
                    // 找回密码失败
                    char msg_buf[256];
                    snprintf(msg_buf, sizeof(msg_buf), "用户 %s 不存在", username);
                    
                    const char * btns[] = {"", "", ""};
                    lv_obj_t * msgbox = lv_msgbox_create(page1, "msgbox", msg_buf, btns, true);
                    lv_obj_set_pos(msgbox, 260, 80);
                    
                    printf("找回密码失败: 用户 %s 不存在\n", username);
                }
            }
        }
    }

    return NULL;
}

// 通用输入框焦点/失焦事件回调：实现软键盘复用+失焦隐藏
void ta_focus_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta        = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        // 先删除旧的软键盘（如果存在）
        if(kb != NULL) {
            lv_obj_del(kb);
        }
        // 重新创建并关联当前输入框
        kb = lv_keyboard_create(page1);
        lv_obj_set_size(kb, 800, 200);
        lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 15);
        lv_keyboard_set_textarea(kb, ta);
    } else if(code == LV_EVENT_DEFOCUSED) {
        // 失焦时直接删除软键盘
        if(kb != NULL) {
            lv_obj_del(kb);
            kb = NULL;
        }
    }
}

// 空白处点击回调：同样用删除替代隐藏
void page1_click_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    if(obj == page1 && kb != NULL) {
        lv_obj_del(kb);
        kb = NULL;
    }
}
// 消息盒子
void bt_msgbox_cb(lv_event_t * e)
{
    const char * btns[] = {"", "", ""};
    lv_obj_t * msgbox   = lv_msgbox_create(page1, "Warning",
                                           "The password/account you entered is incorrect.Please click the 'reg' button "
                                             "to complete the registration process.",
                                           btns, true);
    lv_obj_set_pos(msgbox, 260, 80);
}

void bt_msgbox_cb2(lv_event_t * e)
{
    const char * btns[] = {"", "", ""};
    lv_obj_t * msgbox   = lv_msgbox_create(page1, "Warning", "Name or password is invalid.", btns, true);
    lv_obj_set_pos(msgbox, 260, 80);
}

void bt_msgbox_cb3(const char * name)
{
    char msg_buf[200];
    snprintf(msg_buf, sizeof(msg_buf), "Username %s is already registered!", name);

    const char * btns[] = {"", "", ""};
    lv_obj_t * msgbox   = lv_msgbox_create(page1, "Warning", msg_buf, btns, true);
    lv_obj_set_pos(msgbox, 260, 80);
}

void bt_msgbox_cb4(const char * name)
{
    char msg_buf[200];
    snprintf(msg_buf, sizeof(msg_buf), "Register success! Name: %s\n", name);

    const char * btns[] = {"", "", ""};
    lv_obj_t * msgbox   = lv_msgbox_create(page1, "Warning", msg_buf, btns, true);
    lv_obj_set_pos(msgbox, 260, 80);
}

// 简化版的登录按钮回调（使用轮询）
void bt_login_cb(lv_event_t * e)
{
    char name[50]   = {0};
    char passwd[50] = {0};
    strncpy(name, lv_textarea_get_text(ta1), sizeof(name) - 1);
    strncpy(passwd, lv_textarea_get_text(ta2), sizeof(passwd) - 1);

    if(strlen(name) == 0 || strlen(passwd) == 0) {
        return;
    }

    // 发送登录信息到服务器
    if(send_login_to_server(name, passwd) != 0) {
        printf("发送登录信息失败\n");
        return;
    }

    printf("已发送登录请求，等待服务器响应...\n");

    // 简单轮询等待服务器响应
    int timeout = 30; // 最大等待3秒（30 * 100ms）
    int success = 0;

    for(int i = 0; i < timeout; i++) {
        lv_timer_handler(); // 处理UI事件

        pthread_mutex_lock(&login_mutex);
        if(login_success) {
            success = 1;
            pthread_mutex_unlock(&login_mutex);
            break;
        }
        pthread_mutex_unlock(&login_mutex);

        usleep(100000); // 等待100ms
    }

    if(success) {
        printf("登录成功，切换到下一个页面\n");
        show_login_page2();
        lv_obj_del(page1);
    } else {
        printf("登录超时或失败\n");
        bt_msgbox_cb(NULL);
    }
}

// 注册按钮回调
void bt_regt_cb(lv_event_t * e)
{
    char name[50]   = {0};
    char passwd[50] = {0};
    strncpy(name, lv_textarea_get_text(ta1), sizeof(name) - 1);
    strncpy(passwd, lv_textarea_get_text(ta2), sizeof(passwd) - 1);

    if(strlen(name) == 0 || strlen(passwd) == 0) {
        LV_LOG_WARN("Name or password is empty");
        return;
    }

    // 发送注册信息到服务器
    if(send_register_to_server(name, passwd) == 0) {
        lv_textarea_set_text(ta1, "");
        lv_textarea_set_text(ta2, "");
    }
}

// 忘记密码按钮回调函数
void forget_btn_cb(lv_event_t * e)
{
    char name[50] = {0};
    strncpy(name, lv_textarea_get_text(ta1), sizeof(name) - 1);

    if(strlen(name) == 0) {
        // 弹出提示消息盒子
        const char * btns[] = {"", "", ""};
        lv_obj_t * msgbox = lv_msgbox_create(page1, "Instruction", 
                                             "Please enter your username in the account input box and then click 'Reset Password'.", 
                                             btns, true);
        lv_obj_set_pos(msgbox, 260, 80);
        return;
    }

    // 发送找回密码请求到服务器
    if(send_forget_password_to_server(name) != 0) {
        printf("发送找回密码请求失败\n");
    }
}

// 发送找回密码请求到服务器
int send_forget_password_to_server(const char * name)
{
    if(login_sock <= 0) {
        if(connect_to_server() != 0) {
            return -1;
        }
    }

    char send_buf[256];
    // 找回密码消息格式：forget@用户名
    snprintf(send_buf, sizeof(send_buf), "forget@%s", name);

    int ret = send(login_sock, send_buf, strlen(send_buf), 0);
    if(ret <= 0) {
        perror("发送找回密码请求失败");
        close(login_sock);
        login_sock = -1;
        return -1;
    }

    printf("找回密码请求已发送到服务器: 用户名=%s\n", name);
    return 0;
}

// 登录页面1
void show_login_page1()
{
    // 初始化互斥锁和条件变量
    pthread_mutex_init(&login_mutex, NULL);
    pthread_cond_init(&login_cond, NULL);
    login_success = 0;

    // 连接到服务器
    if(connect_to_server() != 0) {
        printf("无法连接到服务器\n");
        bt_msgbox_cb(NULL);
        return;
    }

    // 创建接收服务器响应的线程
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, recv_login_response, NULL);

    page1 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page1, 800, 480);
    lv_obj_set_pos(page1, 0, 0);

    // 给登录界面添加点击事件：点击空白处隐藏软键盘
    lv_obj_add_event_cb(page1, page1_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * img = lv_img_create(page1);
    LV_IMG_DECLARE(image1);
    lv_img_set_src(img, &image1);
    lv_obj_align_to(img, page1, LV_ALIGN_CENTER, 0, 0);

    // 登录/注册按钮
    bt1 = lv_btn_create(page1);
    bt2 = lv_btn_create(page1);
    lv_obj_set_size(bt1, 100, 30);
    lv_obj_set_pos(bt1, 275, 320);
    lv_obj_set_size(bt2, 100, 30);
    lv_obj_align_to(bt2, bt1, LV_ALIGN_OUT_RIGHT_MID, 60, 0);
    lv_obj_set_style_bg_color(bt1, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bt2, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(bt2, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bt1, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bt1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(bt1, lv_color_hex(0xCCCCCC), LV_STATE_DEFAULT);
    lv_obj_set_style_radius(bt2, 8, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(bt2, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(bt2, lv_color_hex(0xCCCCCC), LV_STATE_DEFAULT);

    // 按钮文字
    lv_obj_t * lb1 = lv_label_create(bt1);
    lv_label_set_text(lb1, "login");
    lv_obj_t * lb2 = lv_label_create(bt2);
    lv_label_set_text(lb2, "regt");
    lv_obj_set_style_text_color(lb1, lv_color_hex(0x000000), LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(lb2, lv_color_hex(0x000000), LV_STATE_DEFAULT);
    lv_obj_align_to(lb1, bt1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(lb2, bt2, LV_ALIGN_CENTER, 0, 0);


    // 标题
    lv_obj_t * lb_title = lv_label_create(page1);
    lv_label_set_text(lb_title, "Welcome to use Whisper");
    lv_obj_align(lb_title, LV_ALIGN_CENTER, 0, -200);
    lv_obj_set_style_text_font(lb_title, &lv_font_montserrat_36, LV_STATE_DEFAULT);

    lv_obj_t * lb_title1 = lv_label_create(page1);
    lv_label_set_text(lb_title1, "------------------LOGIN WITH EMAIL------------------");
    lv_obj_align_to(lb_title1, lb_title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_set_style_text_font(lb_title1, &lv_font_montserrat_12, LV_STATE_DEFAULT);

    // 账户/密码标签
    lv_obj_t * lb3 = lv_label_create(page1);
    lv_label_set_text(lb3, "Address");
    lv_obj_t * lb4 = lv_label_create(page1);
    lv_label_set_text(lb4, "Password");
    lv_obj_set_pos(lb3, 250, 90);
    lv_obj_set_pos(lb4, 250, 170);

    // 文本输入框
    ta1 = lv_textarea_create(page1);
    ta2 = lv_textarea_create(page1);
    lv_obj_set_size(ta1, 300, 50);
    lv_obj_set_pos(ta1, 250, 110);
    lv_obj_set_size(ta2, 300, 50);
    lv_obj_set_pos(ta2, 250, 190);
    lv_textarea_set_one_line(ta1, true);
    lv_textarea_set_one_line(ta2, true);
    lv_textarea_set_password_mode(ta2, true);
    lv_textarea_set_password_show_time(ta2, 1000);

    // 创建只显示文字的按钮忘记密码
    lv_obj_t * forget_btn = lv_btn_create(page1);
    lv_obj_remove_style_all(forget_btn); // 移除所有默认样式
    lv_obj_set_size(forget_btn, 120, 30);
    lv_obj_align_to(forget_btn, ta2, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 5);

    // 设置按钮为透明背景，无边框
    lv_obj_set_style_bg_opa(forget_btn, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(forget_btn, 0, 0);
    lv_obj_set_style_outline_width(forget_btn, 0, 0);
    lv_obj_set_style_shadow_width(forget_btn, 0, 0);

    // 添加内边距，便于点击
    lv_obj_set_style_pad_hor(forget_btn, 10, 0);
    lv_obj_set_style_pad_ver(forget_btn, 5, 0);

    // 创建标签
    lv_obj_t * forget_label = lv_label_create(forget_btn);
    lv_label_set_text(forget_label, "FORGET  PWD");
    lv_obj_center(forget_label);

    // 设置字体样式
    lv_obj_set_style_text_color(forget_label, lv_color_hex(0x000000), 0);
    lv_obj_set_style_text_font(forget_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_decor(forget_label, LV_TEXT_DECOR_UNDERLINE, 0);

    // 添加点击效果
    lv_obj_set_style_text_color(forget_label, lv_color_hex(0x003399), LV_STATE_PRESSED);
    lv_obj_set_style_text_color(forget_label, lv_color_hex(0x0066CC), LV_STATE_FOCUSED);
    lv_obj_set_style_bg_opa(forget_btn, LV_OPA_20, LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(forget_btn, lv_color_hex(0x0066CC), LV_STATE_PRESSED);

    // 添加事件
    lv_obj_add_event_cb(forget_btn, forget_btn_cb, LV_EVENT_CLICKED, NULL);

    // 绑定事件
    lv_obj_add_event_cb(ta1, ta_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(ta1, ta_focus_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(ta2, ta_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(ta2, ta_focus_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(bt1, bt_login_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(bt2, bt_regt_cb, LV_EVENT_CLICKED, NULL);
}