#include "myhead.h"

// 通用输入框焦点回调：用删除/重建替代显示/隐藏
void msg_textarea_focus_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta        = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        // 先删除旧的软键盘（如果存在）
        if(kb != NULL) {
            lv_obj_del(kb);
        }
        // 重新创建并关联当前输入框
        kb = lv_keyboard_create(page2);
        lv_obj_set_size(kb, 590, 200);
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

// 全局变量：保存表情包窗口对象
static lv_obj_t * menu_emoji = NULL;
static lv_obj_t * menu_file  = NULL;

// 表情包窗口关闭按钮回调
void emoji_close_btn_cb(lv_event_t * e)
{
    if(menu_emoji != NULL) {
        lv_obj_del(menu_emoji);
        menu_emoji = NULL;
    }
}

// 新增：显示表情包到聊天窗口
void update_chat_emoji(lv_obj_t * msg_container, const char * emoji_path, int is_self)
{
    if(msg_container == NULL || emoji_path == NULL) return;

    // 1. 匹配聊天窗口索引
    int win_idx = -1;
    for(int i = 0; i < 5; i++) {
        if(chat_windows[i].msg_container == msg_container) {
            win_idx = i;
            break;
        }
    }
    if(win_idx == -1) return;

    // 2. 创建图片控件
    lv_obj_t * emoji_img = lv_img_create(msg_container);
    lv_img_set_src(emoji_img, emoji_path); 
    lv_obj_set_size(emoji_img, 50, 50);    

    // 3. 样式与位置（核心修复）
    chat_window_t * win = &chat_windows[win_idx];
    int img_height      = lv_obj_get_height(emoji_img);
    int img_width       = lv_obj_get_width(emoji_img);
    int container_width = lv_obj_get_width(msg_container);
    int y_pos           = win->last_y - img_height - 80;

    if(is_self) {
        // ========== 修复2：表情包右对齐 ==========
        // 容器宽度 - 图片宽度 - 右边距(20px)
        int img_x = container_width - img_width - 20;
        lv_obj_set_x(emoji_img, img_x);
        lv_obj_set_y(emoji_img, y_pos);
        lv_obj_set_style_radius(emoji_img, 10, LV_STATE_DEFAULT);
        
        // ========== 修复3：删除错误的lv_obj_align调用 ==========
        // 原代码中lv_obj_align(emoji_img, msg_container, -100, 0); 会覆盖坐标，已删除
    } else {
        // 对方发送的表情包：左对齐
        lv_obj_set_x(emoji_img, 20);
        lv_obj_set_y(emoji_img, y_pos);
        lv_obj_set_style_radius(emoji_img, 10, LV_STATE_DEFAULT);
    }

    // 4. 更新窗口状态
    win->msg_count++;
    win->last_y = y_pos + img_height + 10; 
    lv_obj_scroll_to_y(msg_container, win->last_y - 20, LV_ANIM_ON);
    lv_refr_now(NULL);
}

// 表情包按钮1发送服务器回调
void emoji_btn1_cb(lv_event_t * e)
{
    lv_obj_t * emoji_btn = lv_event_get_target(e);
    // 表情包按钮 → 表情包窗口 → 消息容器 → 聊天页面
    lv_obj_t * emoji_win     = lv_obj_get_parent(emoji_btn);     // 表情包窗口
    lv_obj_t * msg_container = lv_obj_get_parent(emoji_win);     // 消息容器
    lv_obj_t * chat_tab      = lv_obj_get_parent(msg_container); // 聊天页面

    if(chat_tab == NULL) {
        printf("无法找到聊天页面\n");
        return;
    }

    // ========== 修改1：获取当前用户索引（与消息发送逻辑一致） ==========
    int user_idx = -1;
    if(chat_tab == tab1_1)
        user_idx = 0;
    else if(chat_tab == tab1_2)
        user_idx = 1;
    else if(chat_tab == tab1_3)
        user_idx = 2;
    else if(chat_tab == tab1_4)
        user_idx = 3;
    else if(chat_tab == tab1_5)
        user_idx = 4;
    if(user_idx < 0) return;

    // ========== 修改2：获取目标IP/端口（与消息发送逻辑一致） ==========
    char target_ip[20];         // 目标客户端IP
    unsigned short target_port; // 目标客户端端口
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    // ========== 修改3：表情包文件读取与大小获取 ==========
    // 表情包文件路径（可根据实际情况调整）
    // 打开表情包文件
    int fd = open("/emoji1.jpg", O_RDONLY);
    if(fd < 0) {
        perror("打开表情包文件失败");
        update_chat_emoji(msg_container, "表情包文件打开失败！", 0);
        return;
    }

    // 获取文件大小
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET); // 重置文件指针到开头
    if(file_size <= 0) {
        update_chat_emoji(msg_container, "表情包文件为空！", 0);
        close(fd);
        return;
    }

    // ========== 修改4：按服务器协议拼接表情包消息头 ==========
    char send_header[2048];
    snprintf(send_header, sizeof(send_header), "emoji@%s@%hu@%ld", target_ip, target_port, file_size);

    // ========== 修改5：发送消息头到服务器（与消息发送逻辑一致） ==========
    if(tcpsock > 0) {
        // 发送表情包消息头
        int header_len = send(tcpsock, send_header, strlen(send_header), 0);
        sleep(1);
        if(header_len <= 0) {
            update_chat_emoji(msg_container, "表情包消息头发送失败！", 0);
            close(fd);
            return;
        }

        // ========== 修改6：循环发送表情包文件数据（解决大数据传输问题） ==========
        char buf[1024];
        ssize_t ret, total_sent = 0;
        while(total_sent < file_size) {
            ret = read(fd, buf, sizeof(buf));
            if(ret <= 0) {
                perror("读取表情包文件失败");
                update_chat_emoji(msg_container, "读取表情包文件失败！", 0);
                close(fd);
                return;
            }

            // 发送读取到的文件数据
            ssize_t sent = send(tcpsock, buf, ret, 0);
            if(sent <= 0) {
                perror("发送表情包数据失败");
                update_chat_emoji(msg_container, "表情包发送失败！", 0);
                close(fd);
                return;
            }
            total_sent += sent;
        }

        // 关闭文件
        close(fd);

        // ========== 修改7：本地显示表情包发送成功（与消息发送逻辑一致） ==========
        update_chat_emoji(msg_container, "S:/emoji1.jpg", 1); // 显示实际图片
        printf("表情包发送成功 - 目标IP:%s, 端口:%d, 大小:%ld字节\n", target_ip, target_port, file_size);
    } else {
        // TCP未连接的错误提示
        update_chat_emoji(msg_container, "TCP未连接，表情包发送失败!", 0);
        close(fd);
    }

    // ========== 修改8：关闭表情包窗口（用户体验优化） ==========
    if(menu_emoji != NULL) {
        lv_obj_del(menu_emoji);
        menu_emoji = NULL;
    }
}

// 表情包按钮2发送服务器回调
void emoji_btn2_cb(lv_event_t * e)
{
    lv_obj_t * emoji_btn = lv_event_get_target(e);
    // 表情包按钮 → 表情包窗口 → 消息容器 → 聊天页面
    lv_obj_t * emoji_win     = lv_obj_get_parent(emoji_btn);     // 表情包窗口
    lv_obj_t * msg_container = lv_obj_get_parent(emoji_win);     // 消息容器
    lv_obj_t * chat_tab      = lv_obj_get_parent(msg_container); // 聊天页面

    if(chat_tab == NULL) {
        printf("无法找到聊天页面\n");
        return;
    }

    // ========== 修改1：获取当前用户索引（与消息发送逻辑一致） ==========
    int user_idx = -1;
    if(chat_tab == tab1_1)
        user_idx = 0;
    else if(chat_tab == tab1_2)
        user_idx = 1;
    else if(chat_tab == tab1_3)
        user_idx = 2;
    else if(chat_tab == tab1_4)
        user_idx = 3;
    else if(chat_tab == tab1_5)
        user_idx = 4;
    if(user_idx < 0) return;

    // ========== 修改2：获取目标IP/端口（与消息发送逻辑一致） ==========
    char target_ip[20];         // 目标客户端IP
    unsigned short target_port; // 目标客户端端口
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    // ========== 修改3：表情包文件读取与大小获取 ==========
    // 表情包文件路径（可根据实际情况调整）
    // 打开表情包文件
    int fd = open("/emoji2.jpg", O_RDONLY);
    if(fd < 0) {
        perror("打开表情包文件失败");
        update_chat_emoji(msg_container, "表情包文件打开失败！", 0);
        return;
    }

    // 获取文件大小
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET); // 重置文件指针到开头
    if(file_size <= 0) {
        update_chat_emoji(msg_container, "表情包文件为空！", 0);
        close(fd);
        return;
    }

    // ========== 修改4：按服务器协议拼接表情包消息头 ==========
    char send_header[2048];
    snprintf(send_header, sizeof(send_header), "emoji@%s@%hu@%ld", target_ip, target_port, file_size);

    // ========== 修改5：发送消息头到服务器（与消息发送逻辑一致） ==========
    if(tcpsock > 0) {
        // 发送表情包消息头
        int header_len = send(tcpsock, send_header, strlen(send_header), 0);
        sleep(1);
        if(header_len <= 0) {
            update_chat_emoji(msg_container, "表情包消息头发送失败！", 0);
            close(fd);
            return;
        }

        // ========== 修改6：循环发送表情包文件数据（解决大数据传输问题） ==========
        char buf[1024];
        ssize_t ret, total_sent = 0;
        while(total_sent < file_size) {
            ret = read(fd, buf, sizeof(buf));
            if(ret <= 0) {
                perror("读取表情包文件失败");
                update_chat_emoji(msg_container, "读取表情包文件失败！", 0);
                close(fd);
                return;
            }

            // 发送读取到的文件数据
            ssize_t sent = send(tcpsock, buf, ret, 0);
            if(sent <= 0) {
                perror("发送表情包数据失败");
                update_chat_emoji(msg_container, "表情包发送失败！", 0);
                close(fd);
                return;
            }
            total_sent += sent;
        }

        // 关闭文件
        close(fd);

        // ========== 修改7：本地显示表情包发送成功（与消息发送逻辑一致） ==========
        update_chat_emoji(msg_container, "S:/emoji2.jpg", 1); // 显示实际图片
        printf("表情包发送成功 - 目标IP:%s, 端口:%d, 大小:%ld字节\n", target_ip, target_port, file_size);
    } else {
        // TCP未连接的错误提示
        update_chat_emoji(msg_container, "TCP未连接，表情包发送失败!", 0);
        close(fd);
    }

    // ========== 修改8：关闭表情包窗口（用户体验优化） ==========
    if(menu_emoji != NULL) {
        lv_obj_del(menu_emoji);
        menu_emoji = NULL;
    }
}

// 表情包按钮3发送服务器回调
void emoji_btn3_cb(lv_event_t * e)
{
    lv_obj_t * emoji_btn = lv_event_get_target(e);
    // 表情包按钮 → 表情包窗口 → 消息容器 → 聊天页面
    lv_obj_t * emoji_win     = lv_obj_get_parent(emoji_btn);     // 表情包窗口
    lv_obj_t * msg_container = lv_obj_get_parent(emoji_win);     // 消息容器
    lv_obj_t * chat_tab      = lv_obj_get_parent(msg_container); // 聊天页面

    if(chat_tab == NULL) {
        printf("无法找到聊天页面\n");
        return;
    }

    // ========== 修改1：获取当前用户索引（与消息发送逻辑一致） ==========
    int user_idx = -1;
    if(chat_tab == tab1_1)
        user_idx = 0;
    else if(chat_tab == tab1_2)
        user_idx = 1;
    else if(chat_tab == tab1_3)
        user_idx = 2;
    else if(chat_tab == tab1_4)
        user_idx = 3;
    else if(chat_tab == tab1_5)
        user_idx = 4;
    if(user_idx < 0) return;

    // ========== 修改2：获取目标IP/端口（与消息发送逻辑一致） ==========
    char target_ip[20];         // 目标客户端IP
    unsigned short target_port; // 目标客户端端口
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    // ========== 修改3：表情包文件读取与大小获取 ==========
    // 表情包文件路径（可根据实际情况调整）
    // 打开表情包文件
    int fd = open("/emoji3.jpg", O_RDONLY);
    if(fd < 0) {
        perror("打开表情包文件失败");
        update_chat_emoji(msg_container, "表情包文件打开失败！", 0);
        return;
    }

    // 获取文件大小
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET); // 重置文件指针到开头
    if(file_size <= 0) {
        update_chat_emoji(msg_container, "表情包文件为空！", 0);
        close(fd);
        return;
    }

    // ========== 修改4：按服务器协议拼接表情包消息头 ==========
    char send_header[2048];
    snprintf(send_header, sizeof(send_header), "emoji@%s@%hu@%ld", target_ip, target_port, file_size);

    // ========== 修改5：发送消息头到服务器（与消息发送逻辑一致） ==========
    if(tcpsock > 0) {
        // 发送表情包消息头
        int header_len = send(tcpsock, send_header, strlen(send_header), 0);
        sleep(1);
        if(header_len <= 0) {
            update_chat_emoji(msg_container, "表情包消息头发送失败！", 0);
            close(fd);
            return;
        }

        // ========== 修改6：循环发送表情包文件数据（解决大数据传输问题） ==========
        char buf[1024];
        ssize_t ret, total_sent = 0;
        while(total_sent < file_size) {
            ret = read(fd, buf, sizeof(buf));
            if(ret <= 0) {
                perror("读取表情包文件失败");
                update_chat_emoji(msg_container, "读取表情包文件失败！", 0);
                close(fd);
                return;
            }

            // 发送读取到的文件数据
            ssize_t sent = send(tcpsock, buf, ret, 0);
            if(sent <= 0) {
                perror("发送表情包数据失败");
                update_chat_emoji(msg_container, "表情包发送失败！", 0);
                close(fd);
                return;
            }
            total_sent += sent;
        }

        // 关闭文件
        close(fd);

        // ========== 修改7：本地显示表情包发送成功（与消息发送逻辑一致） ==========
        update_chat_emoji(msg_container, "S:/emoji3.jpg", 1); // 显示实际图片
        printf("表情包发送成功 - 目标IP:%s, 端口:%d, 大小:%ld字节\n", target_ip, target_port, file_size);
    } else {
        // TCP未连接的错误提示
        update_chat_emoji(msg_container, "TCP未连接，表情包发送失败!", 0);
        close(fd);
    }

    // ========== 修改8：关闭表情包窗口（用户体验优化） ==========
    if(menu_emoji != NULL) {
        lv_obj_del(menu_emoji);
        menu_emoji = NULL;
    }
}

// 打开表情包按钮回调函数
void emoji_btn_cb(lv_event_t * e)
{
    lv_obj_t * emoji_btn = lv_event_get_target(e);
    // 获取输入容器（表情包按钮的父容器）
    lv_obj_t * input_container = lv_obj_get_parent(emoji_btn);
    // 获取聊天页面（输入容器的父容器）
    lv_obj_t * chat_tab = lv_obj_get_parent(input_container);
    // 获取聊天记录容器（聊天页面的第一个子元素）
    lv_obj_t * msg_container = lv_obj_get_child(chat_tab, 0);

    // 1. 先删除旧窗口（如果存在）
    if(menu_emoji != NULL) {
        lv_obj_del(menu_emoji);
        menu_emoji = NULL;
    }

    // 2. 创建表情包窗口（父对象改为聊天记录容器）
    menu_emoji = lv_obj_create(msg_container);
    // 设置窗口大小和位置（在聊天记录容器内居中偏上）
    lv_obj_set_size(menu_emoji, 300, 250);
    lv_obj_add_flag(menu_emoji, LV_OBJ_FLAG_FLOATING); // 让窗口脱离父容器的弹性布局
    lv_obj_set_pos(menu_emoji, 20, 80);

    // 3. 设置窗口样式
    lv_obj_set_style_bg_color(menu_emoji, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(menu_emoji, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(menu_emoji, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(menu_emoji, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(menu_emoji, 5, LV_STATE_DEFAULT); // 轻微圆角更美观

    // 4. 创建关闭按钮（右上角）
    lv_obj_t * close_btn = lv_btn_create(menu_emoji);
    lv_obj_set_size(close_btn, 25, 25);
    lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, -5, 5); // 右上角偏移5px

    // 关闭按钮样式
    lv_obj_set_style_bg_color(close_btn, lv_color_hex(0x00bfff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(close_btn, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(close_btn, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(close_btn, 0, LV_STATE_DEFAULT);

    // 关闭按钮文字
    lv_obj_t * close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, "X");
    lv_obj_set_style_text_color(close_label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(close_label, &lv_font_montserrat_12, LV_STATE_DEFAULT);
    lv_obj_center(close_label);

    // 绑定关闭按钮回调
    lv_obj_add_event_cb(close_btn, emoji_close_btn_cb, LV_EVENT_CLICKED, NULL);

    // 创建表情包按钮1
    lv_obj_t * emoji_btn1 = lv_btn_create(menu_emoji);
    lv_obj_set_size(emoji_btn1, 50, 50);
    lv_obj_set_pos(emoji_btn1, 20, 20);

    // 表情包图片
    lv_obj_t * emoji_img1 = lv_img_create(emoji_btn1);
    lv_img_set_src(emoji_img1, "S:/emoji1.jpg");
    lv_obj_set_size(emoji_img1, 50, 50);
    lv_obj_center(emoji_img1);

    // 表情包按钮1发送服务器回调
    lv_obj_add_event_cb(emoji_btn1, emoji_btn1_cb, LV_EVENT_CLICKED, NULL);

    // 创建表情包按钮2
    lv_obj_t * emoji_btn2 = lv_btn_create(menu_emoji);
    lv_obj_set_size(emoji_btn2, 50, 50);
    lv_obj_align_to(emoji_btn2, emoji_btn1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    // 表情包图片
    lv_obj_t * emoji_img2 = lv_img_create(emoji_btn2);
    lv_img_set_src(emoji_img2, "S:/emoji2.jpg");
    lv_obj_set_size(emoji_img2, 50, 50);
    lv_obj_center(emoji_img2);

    // 表情包按钮2发送服务器回调
    lv_obj_add_event_cb(emoji_btn2, emoji_btn2_cb, LV_EVENT_CLICKED, NULL);

    // 创建表情包按钮3
    lv_obj_t * emoji_btn3 = lv_btn_create(menu_emoji);
    lv_obj_set_size(emoji_btn3, 50, 50);
    lv_obj_align_to(emoji_btn3, emoji_btn2, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    // 表情包图片
    lv_obj_t * emoji_img3 = lv_img_create(emoji_btn3);
    lv_img_set_src(emoji_img3, "S:/emoji3.jpg");
    lv_obj_set_size(emoji_img3, 50, 50);
    lv_obj_center(emoji_img3);

    // 表情包按钮3发送服务器回调
    lv_obj_add_event_cb(emoji_btn3, emoji_btn3_cb, LV_EVENT_CLICKED, NULL);
}

// 文件窗口关闭按钮回调
void file_close_btn_cb(lv_event_t * e)
{
    if(menu_file != NULL) {
        lv_obj_del(menu_file);
        menu_file = NULL;
    }
}

// 文件按钮回调函数
void file_btn_cb(lv_event_t * e)
{
    lv_obj_t * file_btn = lv_event_get_target(e);
    // 获取输入容器（表情包按钮的父容器）
    lv_obj_t * input_container = lv_obj_get_parent(file_btn);
    // 获取聊天页面（输入容器的父容器）
    lv_obj_t * chat_tab = lv_obj_get_parent(input_container);
    // 获取聊天记录容器（聊天页面的第一个子元素）
    lv_obj_t * msg_container = lv_obj_get_child(chat_tab, 0);

    // 1. 先删除旧窗口（如果存在）
    if(menu_file != NULL) {
        lv_obj_del(menu_file);
        menu_file = NULL;
    }

    // 2. 创建表情包窗口（父对象改为聊天记录容器）
    menu_file = lv_obj_create(msg_container);
    // 设置窗口大小和位置（在聊天记录容器内居中偏上）
    lv_obj_set_size(menu_file, 300, 250);
    lv_obj_add_flag(menu_file, LV_OBJ_FLAG_FLOATING); // 让窗口脱离父容器的弹性布局
    lv_obj_set_pos(menu_file, 80, 80);

    // 3. 设置窗口样式
    lv_obj_set_style_bg_color(menu_file, lv_color_hex(0xffffff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(menu_file, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(menu_file, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_all(menu_file, 0, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(menu_file, 5, LV_STATE_DEFAULT); // 轻微圆角更美观

    // 4. 创建关闭按钮（右上角）
    lv_obj_t * close_btn = lv_btn_create(menu_file);
    lv_obj_set_size(close_btn, 25, 25);
    lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, -5, 5); // 右上角偏移5px
    // 关闭按钮样式
    lv_obj_set_style_bg_color(close_btn, lv_color_hex(0x00bfff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(close_btn, LV_OPA_COVER, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(close_btn, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT); // 圆形按钮
    lv_obj_set_style_border_width(close_btn, 0, LV_STATE_DEFAULT);

    // 关闭按钮文字
    lv_obj_t * close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, "X");
    lv_obj_set_style_text_color(close_label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(close_label, &lv_font_montserrat_12, LV_STATE_DEFAULT);
    lv_obj_center(close_label);

    // 绑定关闭按钮回调
    lv_obj_add_event_cb(close_btn, file_close_btn_cb, LV_EVENT_CLICKED, NULL);
}

// 消息队列全局变量定义
msg_queue_t msg_queue[10];
int msg_queue_idx = 0;
pthread_mutex_t msg_mutex;

// 5个用户信息初始化
user_info_t user_list[5] = {{"Kai", "kai124", "20031009", "guangzhou", "Whisper softly, live boldly.", "S:/zhou.jpg",
                             NULL, "192.168.11.150", 10001},
                            {"Leo", "leo123", "20031010", "beijing", "Speak less, mean more.", "S:/picture1.jpg", NULL,
                             "192.168.11.150", 10002},
                            {"Roy", "roy456", "20031011", "shanghai", "Calm mind, warm heart.", "S:/picture2.jpg", NULL,
                             "192.168.11.150", 10003},
                            {"Eli", "eli789", "20031012", "shenzhen", "Quiet but not silent.", "S:/picture3.jpg", NULL,
                             "192.168.11.150", 10004},
                            {"Ike", "ike000", "20031013", "hangzhou", "Gentle, but unbroken.", "S:/picture4.jpg", NULL,
                             "192.168.11.150", 10005}};

chat_window_t chat_windows[5] = {{NULL, 0, 350}, {NULL, 0, 350}, {NULL, 0, 350}, {NULL, 0, 350}, {NULL, 0, 350}};
// 修复：更新LVGL聊天界面消息
void update_chat_msg(lv_obj_t * msg_container, const char * msg, int is_self)
{
    if(msg_container == NULL || msg == NULL || strlen(msg) == 0) return;

    // 步骤1：匹配当前聊天窗口索引
    int win_idx = -1;
    for(int i = 0; i < 5; i++) {
        if(chat_windows[i].msg_container == msg_container) {
            win_idx = i;
            break;
        }
    }
    // 首次初始化聊天窗口
    if(win_idx == -1) {
        for(int i = 0; i < 5; i++) {
            if(chat_windows[i].msg_container == NULL) {
                chat_windows[i].msg_container = msg_container;
                chat_windows[i].msg_count     = 0;
                chat_windows[i].last_y        = 350;
                win_idx                       = i;
                break;
            }
        }
    }
    if(win_idx == -1) return; // 超出5个窗口限制

    // 步骤2：创建消息标签
    lv_obj_t * msg_label = lv_label_create(msg_container);
    // 截断超长消息
    char msg_buf[128] = {0};
    strncpy(msg_buf, msg, 127);
    lv_label_set_text(msg_label, msg_buf);

    // 启用标签的背景显示+自适应宽度
    lv_obj_set_style_bg_opa(msg_label, LV_OPA_COVER, LV_STATE_DEFAULT); // 背景不透明

    // 步骤3：样式设置
    static lv_style_t style_self, style_other;
    static bool style_inited = false;
    if(!style_inited) { // 仅初始化一次
        // 自己消息样式
        lv_style_init(&style_self);
        lv_style_set_text_font(&style_self, &lv_font_montserrat_14);
        lv_style_set_bg_color(&style_self, lv_color_hex(0x07C160));   // 绿色背景
        lv_style_set_text_color(&style_self, lv_color_hex(0x333333)); // 黑色文字
        lv_style_set_radius(&style_self, 10);                         // 圆角
        lv_style_set_pad_all(&style_self, 8);                         // 内边距

        lv_style_init(&style_other);
        lv_style_set_text_font(&style_other, &lv_font_montserrat_14);
        lv_style_set_bg_color(&style_other, lv_color_white());         // 白色背景
        lv_style_set_text_color(&style_other, lv_color_hex(0x333333)); // 黑色文字
        lv_style_set_radius(&style_other, 10);
        lv_style_set_pad_all(&style_other, 8);

        style_inited = true;
    }

    // 步骤5：计算消息坐标（核心修复：正确的右对齐逻辑）
    chat_window_t * win = &chat_windows[win_idx];
    int msg_height      = lv_obj_get_height(msg_label); 
    int msg_width       = lv_obj_get_width(msg_label);  // 获取标签实际宽度
    int container_width = lv_obj_get_width(msg_container); // 容器宽度
    int y_pos = win->last_y - msg_height - 50;

    // 步骤6：应用样式+设置位置（核心修复）
    if(is_self) {
        lv_obj_add_style(msg_label, &style_self, LV_STATE_DEFAULT);
        lv_obj_set_size(msg_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

        // ========== 修复1：正确计算右对齐X坐标 ==========
        // 容器宽度 - 消息宽度 - 右边距(20px) = 消息左边缘X坐标
        int label_x = container_width - msg_width - 20;
        // 防止X坐标为负数（消息过长时）
        label_x = label_x < 20 ? 20 : label_x;
        lv_obj_set_x(msg_label, label_x);
        lv_obj_set_y(msg_label, y_pos);
    } else {
        lv_obj_add_style(msg_label, &style_other, LV_STATE_DEFAULT);
        // 对方消息左对齐
        lv_obj_set_x(msg_label, 20);
        lv_obj_set_y(msg_label, y_pos);
    }

    // 步骤7：更新计数+滚动+刷新
    win->msg_count++;
    win->last_y = y_pos + msg_height + 10; 
    lv_obj_scroll_to_y(msg_container, win->last_y - 20, LV_ANIM_ON);
    lv_refr_now(NULL);
}

// 聊天消息发送回调
void chat_send_msg_cb(lv_event_t * e)
{
    // lv_obj_t * btn      = lv_event_get_target(e);
    lv_obj_t * textarea = (lv_obj_t *)lv_event_get_user_data(e);
    if(textarea == NULL) return;

    // 获取输入框文本
    const char * msg = lv_textarea_get_text(textarea);
    if(msg == NULL || strlen(msg) == 0) return;

    // ========== 关键修改1：绑定目标IP/端口 ==========
    // 获取聊天页面和消息容器
    lv_obj_t * chat_page     = lv_obj_get_parent(lv_obj_get_parent(textarea));
    lv_obj_t * msg_container = lv_obj_get_child(chat_page, NULL);
    if(msg_container == NULL) return;

    // 获取当前用户索引
    int user_idx = -1;
    if(chat_page == tab1_1)
        user_idx = 0;
    else if(chat_page == tab1_2)
        user_idx = 1;
    else if(chat_page == tab1_3)
        user_idx = 2;
    else if(chat_page == tab1_4)
        user_idx = 3;
    else if(chat_page == tab1_5)
        user_idx = 4;
    if(user_idx < 0) return;

    char target_ip[20];         // 目标客户端IP
    unsigned short target_port; // 目标客户端端口
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    // ========== 关键修改2：按服务器协议拼接消息 ==========
    char send_buf[2048];
    snprintf(send_buf, sizeof(send_buf), "msg@%s@%hu@%s", target_ip, target_port, msg);

    // 发送消息到服务器
    if(tcpsock > 0) {
        int send_len = send(tcpsock, send_buf, strlen(send_buf), 0);
        if(send_len > 0) {
            // 本地显示自己发送的消息
            update_chat_msg(msg_container, msg, 1);
            printf("发送成功：%s\n", send_buf);
        } else {
            update_chat_msg(msg_container, "发送失败！", 0);
        }
    } else {
        update_chat_msg(msg_container, "TCP未连接，发送失败!", 0);
    }

    // 清空输入框
    lv_textarea_set_text(textarea, "");
    lv_obj_scroll_to_y(msg_container, lv_obj_get_scroll_bottom(msg_container), LV_ANIM_ON);
}

// 原有函数：切换聊天标签页回调
void btn_send_cb(lv_event_t * e)
{
    if(tabview1 != NULL) {
        lv_tabview_set_act(tabview1, 0, LV_ANIM_ON);
    }

    lv_obj_t * btn      = lv_event_get_target(e);
    lv_obj_t * tab_page = lv_obj_get_parent(btn);
    int user_idx        = -1;
    if(tab_page == tab2_1)
        user_idx = 0;
    else if(tab_page == tab2_2)
        user_idx = 1;
    else if(tab_page == tab2_3)
        user_idx = 2;
    else if(tab_page == tab2_4)
        user_idx = 3;
    else if(tab_page == tab2_5)
        user_idx = 4;

    if(tabview2 != NULL && user_idx >= 0) {
        lv_tabview_set_act(tabview2, user_idx, LV_ANIM_ON);
    }
}

// 原有函数：创建微信风格聊天页面
void create_chat_page(lv_obj_t * chat_tab, const char * username)
{
    // 1. 聊天记录容器
    lv_obj_t * msg_container = lv_obj_create(chat_tab);
    lv_obj_set_size(msg_container, 580, 350);
    lv_obj_set_pos(msg_container, 0, 0);
    lv_obj_set_style_bg_color(msg_container, lv_color_hex(0xcccccc), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(msg_container, 0, 0);
    lv_obj_set_style_pad_all(msg_container, 0, 0);
    // lv_obj_set_scrollbar_mode(msg_container, LV_SCROLLBAR_MODE_AUTO);

    // ========== LVGL 8.3弹性布局 ==========
    // lv_obj_set_layout(msg_container, LV_LAYOUT_FLEX);
    // lv_obj_set_flex_flow(msg_container, LV_FLEX_FLOW_COLUMN);
    // lv_obj_set_flex_align(msg_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER);

    // 保存消息容器到用户结构体+初始化聊天窗口
    for(int i = 0; i < 5; i++) {
        if(strcmp(user_list[i].nickname, username) == 0) {
            user_list[i].msg_container    = msg_container;
            chat_windows[i].msg_container = msg_container;
            chat_windows[i].msg_count     = 0;
            chat_windows[i].last_y        = 350;
            break;
        }
    }

    // 欢迎消息（LVGL 8.3居中）
    lv_obj_t * welcome_label = lv_label_create(msg_container);
    lv_label_set_text_fmt(welcome_label, "Start chatting with %s.", username);
    lv_obj_set_style_text_font(welcome_label, &lv_font_montserrat_16, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(welcome_label, lv_color_hex(0x666666), LV_STATE_DEFAULT);
    lv_obj_center(welcome_label);

    // 2. 底部输入区容器
    lv_obj_t * input_container = lv_obj_create(chat_tab);
    lv_obj_set_size(input_container, 580, 120);
    lv_obj_set_pos(input_container, 0, 340);
    lv_obj_set_style_bg_color(input_container, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(input_container, 0, 0);
    lv_obj_set_style_pad_all(input_container, 0, 0);

    // 3. 消息输入框
    lv_obj_t * msg_textarea = lv_textarea_create(input_container);
    lv_obj_set_size(msg_textarea, 450, 60);
    lv_obj_set_pos(msg_textarea, 5, 50);
    lv_textarea_set_placeholder_text(msg_textarea, "Type a message...");
    lv_obj_set_style_text_font(msg_textarea, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(msg_textarea, lv_color_hex(0xDCDCDC), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(msg_textarea, 1, 0);
    lv_obj_set_style_radius(msg_textarea, 5, LV_STATE_DEFAULT);

    lv_obj_add_event_cb(msg_textarea, msg_textarea_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(msg_textarea, msg_textarea_focus_cb, LV_EVENT_DEFOCUSED, NULL);

    // 4. 发送按钮
    lv_obj_t * send_btn = lv_btn_create(input_container);
    lv_obj_set_size(send_btn, 100, 60);
    lv_obj_align_to(send_btn, msg_textarea, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_set_style_bg_color(send_btn, lv_color_hex(0x00bfff), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(send_btn, 255, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(send_btn, 5, LV_STATE_DEFAULT);

    lv_obj_t * send_label = lv_label_create(send_btn);
    lv_label_set_text(send_label, "Send");
    lv_obj_set_style_text_color(send_label, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_center(send_label);

    // 5.表情包按钮
    lv_obj_t * emoji_btn = lv_btn_create(input_container);
    lv_obj_set_size(emoji_btn, 30, 30);
    lv_obj_set_pos(emoji_btn, 20, 10);
    lv_obj_set_style_radius(emoji_btn, 0, LV_STATE_DEFAULT);

    lv_obj_t * emoji_title = lv_img_create(emoji_btn);
    lv_img_set_src(emoji_title, "S:/emoji.jpg");
    lv_obj_set_size(emoji_title, 30, 30);
    lv_obj_center(emoji_title);

    lv_obj_add_event_cb(emoji_btn, emoji_btn_cb, LV_EVENT_CLICKED, NULL);

    // 6.文件按钮
    lv_obj_t * file_btn = lv_btn_create(input_container);
    lv_obj_set_size(file_btn, 30, 30);
    lv_obj_align_to(file_btn, emoji_btn, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style_radius(file_btn, 0, LV_STATE_DEFAULT);

    lv_obj_t * file_title = lv_img_create(file_btn);
    lv_img_set_src(file_title, "S:/file.jpg");
    lv_obj_set_size(file_title, 30, 30);
    lv_obj_center(file_title);

    lv_obj_add_event_cb(file_btn, file_btn_cb, LV_EVENT_CLICKED, NULL);

    // 绑定回调
    lv_obj_add_event_cb(send_btn, chat_send_msg_cb, LV_EVENT_CLICKED, msg_textarea);
}

// 原有函数：登录页面2
void show_login_page2(void)
{
    page2 = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page2, 800, 480);
    lv_obj_set_style_pad_all(page2, 0, 0);
    lv_obj_set_style_border_width(page2, 0, 0);

    tabview1 = lv_tabview_create(page2, LV_DIR_LEFT, 60);
    lv_obj_set_size(tabview1, 800, 480);
    lv_obj_set_style_pad_all(page2, 0, 0);
    lv_obj_set_style_border_width(page2, 0, 0);
    tab1 = lv_tabview_add_tab(tabview1, "Chat Room");
    tab2 = lv_tabview_add_tab(tabview1, "Contact Person");
    tab3 = lv_tabview_add_tab(tabview1, "Friends Circle");
    tab4 = lv_tabview_add_tab(tabview1, " ");
    tab5 = lv_tabview_add_tab(tabview1, " ");
    tab6 = lv_tabview_add_tab(tabview1, " ");
    tab7 = lv_tabview_add_tab(tabview1, "More");
    lv_tabview_set_act(tabview1, 0, LV_ANIM_OFF);

    // 聊天室TabView
    tabview2 = lv_tabview_create(tab1, LV_DIR_LEFT, 150);
    lv_obj_set_size(tabview2, 800, 480);
    lv_obj_set_style_pad_all(tab1, 0, 0);
    lv_obj_set_style_border_width(tab1, 0, 0);
    tab1_1 = lv_tabview_add_tab(tabview2, user_list[0].nickname);
    tab1_2 = lv_tabview_add_tab(tabview2, user_list[1].nickname);
    tab1_3 = lv_tabview_add_tab(tabview2, user_list[2].nickname);
    tab1_4 = lv_tabview_add_tab(tabview2, user_list[3].nickname);
    tab1_5 = lv_tabview_add_tab(tabview2, user_list[4].nickname);
    tab1_6 = lv_tabview_add_tab(tabview2, " ");
    tab1_7 = lv_tabview_add_tab(tabview2, " ");
    lv_tabview_set_act(tabview2, 0, LV_ANIM_OFF);

    // 创建聊天页面
    lv_obj_t * chat_tabs[5] = {tab1_1, tab1_2, tab1_3, tab1_4, tab1_5};
    for(int i = 0; i < 5; i++) {
        create_chat_page(chat_tabs[i], user_list[i].nickname);
    }

    // 联系人TabView
    tabview3 = lv_tabview_create(tab2, LV_DIR_LEFT, 150);
    lv_obj_set_size(tabview3, 800, 480);
    lv_obj_set_style_pad_all(tab2, 0, 0);
    lv_obj_set_style_border_width(tab2, 0, 0);
    tab2_1 = lv_tabview_add_tab(tabview3, user_list[0].nickname);
    tab2_2 = lv_tabview_add_tab(tabview3, user_list[1].nickname);
    tab2_3 = lv_tabview_add_tab(tabview3, user_list[2].nickname);
    tab2_4 = lv_tabview_add_tab(tabview3, user_list[3].nickname);
    tab2_5 = lv_tabview_add_tab(tabview3, user_list[4].nickname);
    lv_tabview_set_act(tabview3, 0, LV_ANIM_OFF);

    // 循环创建用户详情页
    lv_obj_t * user_tabs[5] = {tab2_1, tab2_2, tab2_3, tab2_4, tab2_5};
    for(int i = 0; i < 5; i++) {
        // 1. 用户头像按钮
        lv_obj_t * btn_user = lv_btn_create(user_tabs[i]);
        lv_obj_set_size(btn_user, 100, 100);
        lv_obj_set_pos(btn_user, 150, 10);

        // 2. 头像图片
        lv_obj_t * img1 = lv_img_create(btn_user);
        lv_img_set_src(img1, user_list[i].img_path);
        lv_obj_set_size(img1, 100, 100);
        lv_obj_center(img1);

        // 3. 个人信息标签
        lv_obj_t * lb_c1 = lv_label_create(user_tabs[i]);
        lv_label_set_text_fmt(lb_c1, "Username: %s", user_list[i].username);
        lv_obj_align_to(lb_c1, btn_user, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
        lv_obj_set_style_text_font(lb_c1, &lv_font_montserrat_16, LV_STATE_DEFAULT);

        lv_obj_t * lb_c2 = lv_label_create(user_tabs[i]);
        lv_label_set_text_fmt(lb_c2, "Whisper Num: %s", user_list[i].whisper_num);
        lv_obj_align_to(lb_c2, btn_user, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
        lv_obj_set_style_text_font(lb_c2, &lv_font_montserrat_16, LV_STATE_DEFAULT);

        lv_obj_t * lb_c3 = lv_label_create(user_tabs[i]);
        lv_label_set_text_fmt(lb_c3, "region: %s", user_list[i].region);
        lv_obj_align_to(lb_c3, btn_user, LV_ALIGN_OUT_RIGHT_BOTTOM, 20, 0);
        lv_obj_set_style_text_font(lb_c3, &lv_font_montserrat_16, LV_STATE_DEFAULT);

        // 4. 个性签名
        lv_obj_t * lb_c4 = lv_label_create(user_tabs[i]);
        lv_label_set_text(lb_c4, "Personal signature");
        lv_obj_align_to(lb_c4, btn_user, LV_ALIGN_OUT_BOTTOM_LEFT, 10, 20);
        lv_obj_set_style_text_font(lb_c4, &lv_font_montserrat_12, LV_STATE_DEFAULT);

        lv_obj_t * lb_c5 = lv_label_create(user_tabs[i]);
        lv_label_set_text(lb_c5, user_list[i].signature);
        lv_obj_align_to(lb_c5, lb_c4, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
        lv_obj_set_style_text_font(lb_c5, &lv_font_montserrat_18, LV_STATE_DEFAULT);

        // 5. 发消息按钮
        lv_obj_t * btn_send = lv_btn_create(user_tabs[i]);
        lv_obj_set_size(btn_send, 100, 30);
        lv_obj_align_to(btn_send, btn_user, LV_ALIGN_OUT_BOTTOM_LEFT, 10, 200);
        lv_obj_set_style_bg_color(btn_send, lv_color_hex(0x00bfff), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(btn_send, 255, LV_STATE_DEFAULT);

        lv_obj_t * lb_send = lv_label_create(btn_send);
        lv_label_set_text(lb_send, "Send msg");
        lv_obj_set_style_text_color(lb_send, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_center(lb_send);

        lv_obj_add_event_cb(btn_send, btn_send_cb, LV_EVENT_CLICKED, NULL);

        // 6. 音视频按钮
        lv_obj_t * btn_video = lv_btn_create(user_tabs[i]);
        lv_obj_set_size(btn_video, 100, 30);
        lv_obj_align_to(btn_video, btn_send, LV_ALIGN_OUT_RIGHT_MID, 50, 0);
        lv_obj_set_style_bg_color(btn_video, lv_color_hex(0x00bfff), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(btn_video, 255, LV_STATE_DEFAULT);

        lv_obj_t * lb_video = lv_label_create(btn_video);
        lv_label_set_text(lb_video, "Video chat");
        lv_obj_set_style_text_color(lb_video, lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_center(lb_video);
    }
}

// 原有函数：自定义LVGL时钟
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}