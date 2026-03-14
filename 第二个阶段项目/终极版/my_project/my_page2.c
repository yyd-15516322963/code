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
    if(msg_container == NULL) {
        printf("错误：消息容器为空\n");
        return;
    }

    if(emoji_path == NULL) {
        printf("错误：表情包路径为空\n");
        return;
    }

    printf("开始显示表情包：%s (is_self=%d)\n", emoji_path, is_self);

    // 1. 匹配聊天窗口索引
    int win_idx = -1;
    for(int i = 0; i < 5; i++) {
        if(msg_windows[i].msg_container == msg_container) {
            win_idx = i;
            break;
        }
    }
    if(win_idx == -1) {
        printf("错误：找不到匹配的窗口索引\n");
        return;
    }

    // 2. 创建外层容器
    lv_obj_t * emoji_container = lv_obj_create(msg_container);
    lv_obj_remove_style_all(emoji_container);

    // 设置表情包容器样式
    lv_obj_set_size(emoji_container, 50, 50);
    lv_obj_set_style_radius(emoji_container, 10, 0);
    lv_obj_set_style_bg_opa(emoji_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(emoji_container, 0, 0);
    lv_obj_set_style_pad_all(emoji_container, 0, 0);

    // 3. 创建图片控件
    lv_obj_t * emoji_img = lv_img_create(emoji_container);
    lv_img_set_src(emoji_img, emoji_path);
    lv_obj_set_size(emoji_img, 50, 50);
    lv_obj_center(emoji_img);
    lv_obj_set_style_radius(emoji_img, 8, 0);
    lv_obj_set_style_border_width(emoji_img, 1, 0);
    lv_obj_set_style_border_color(emoji_img, lv_color_hex(0xCCCCCC), 0);

    // 4. 计算位置
    msg_window_t * win = &msg_windows[win_idx];
    int emoji_height   = 50;
    int y_pos          = win->last_y;

    lv_obj_set_y(emoji_container, y_pos);

    if(is_self) {
        // 自己发送：右对齐
        lv_obj_set_x(emoji_container, 560 - 50 - 20);
        lv_obj_set_style_bg_color(emoji_container, lv_color_hex(0x07C160), 0);
        lv_obj_set_style_bg_opa(emoji_container, LV_OPA_30, 0);
    } else {
        // 对方发送：左对齐
        lv_obj_set_x(emoji_container, 20);
        lv_obj_set_style_bg_color(emoji_container, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(emoji_container, LV_OPA_30, 0);
    }

    // 5. 更新容器高度和窗口状态
    int new_message_bottom   = y_pos + emoji_height;
    int new_container_height = new_message_bottom + 20;

    if(new_container_height > win->container_height) {
        win->container_height = new_container_height;
        lv_obj_set_height(msg_container, win->container_height);
    }

    win->msg_count++;
    win->last_y = y_pos + emoji_height + 10;

    printf("消息高度：%d, 位置Y：%d\n", emoji_height, y_pos);
    printf("更新消息计数：%d, 最后Y位置：%d\n", win->msg_count, win->last_y);

    // 6. 自动滚动到最新表情包
    int scroll_target = win->last_y - 350;
    if(scroll_target < 0) scroll_target = 0;

    if(win->scroll_container != NULL) {
        lv_obj_scroll_to_y(win->scroll_container, scroll_target, LV_ANIM_ON);
        printf("滚动到位置：%d\n", scroll_target);
    }

    printf("表情包显示完成\n");
}

// 添加一个辅助函数来获取当前激活的聊天页面
static lv_obj_t * get_active_chat_page(void)
{
    if(tabview2 == NULL) {
        printf("错误：tabview2 为空\n");
        return NULL;
    }
    // 获取当前激活的标签页索引
    uint16_t active_tab = lv_tabview_get_tab_act(tabview2);
    if(active_tab >= 5) { // 我们有5个聊天标签
        printf("错误：激活的标签页索引超出范围：%d\n", active_tab);
        return NULL;
    }

    // 根据索引返回对应的聊天页面
    switch(active_tab) {
        case 0: return tab1_1;
        case 1: return tab1_2;
        case 2: return tab1_3;
        case 3: return tab1_4;
        case 4: return tab1_5;
        default: return NULL;
    }
}

// 表情包按钮1发送服务器回调
void emoji_btn1_cb(lv_event_t * e)
{
    lv_obj_t * emoji_btn = lv_event_get_target(e);
    if(emoji_btn == NULL) {
        printf("错误：表情包按钮为空\n");
        return;
    }

    // 获取当前激活的聊天页面
    lv_obj_t * chat_tab = get_active_chat_page();
    if(chat_tab == NULL) {
        printf("错误：无法获取当前激活的聊天页面\n");
        return;
    }

    // 查找滚动容器
    lv_obj_t * scroll_container = lv_obj_get_child(chat_tab, 0);
    if(scroll_container == NULL) {
        printf("错误：找不到滚动容器\n");
        return;
    }

    // 查找内部消息容器
    lv_obj_t * msg_container = lv_obj_get_child(scroll_container, 0);
    if(msg_container == NULL) {
        printf("错误：找不到内部消息容器\n");
        return;
    }

    int user_idx = -1;
    if(chat_tab == tab1_1) {
        user_idx = 0;
    } else if(chat_tab == tab1_2) {
        user_idx = 1;
    } else if(chat_tab == tab1_3) {
        user_idx = 2;
    } else if(chat_tab == tab1_4) {
        user_idx = 3;
    } else if(chat_tab == tab1_5) {
        user_idx = 4;
    }

    if(user_idx < 0) {
        printf("错误：用户索引无效\n");
        return;
    }

    char target_ip[20];
    unsigned short target_port;
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    const char * emoji_path = "/emoji1.jpg";
    int fd                  = open(emoji_path, O_RDONLY);
    if(fd < 0) {
        perror("打开表情包文件失败");
        update_chat_msg(msg_container, "表情包文件打开失败！", 0);
        return;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if(file_size <= 0) {
        printf("错误：表情包文件为空\n");
        update_chat_msg(msg_container, "表情包文件为空！", 0);
        close(fd);
        return;
    }

    printf("表情包文件大小：%ld字节\n", file_size);

    char send_header[2048];
    snprintf(send_header, sizeof(send_header), "emoji@%s@%hu@%ld", target_ip, target_port, file_size);

    if(tcpsock > 0) {
        int header_len = send(tcpsock, send_header, strlen(send_header), 0);
        if(header_len <= 0) {
            perror("发送表情包消息头失败");
            update_chat_msg(msg_container, "表情包消息头发送失败！", 0);
            close(fd);
            return;
        }

        printf("表情包消息头发送成功，长度：%d\n", header_len);

        usleep(100000);

        char buf[1024];
        ssize_t ret, total_sent = 0;
        while(total_sent < file_size) {
            ret = read(fd, buf, sizeof(buf));
            if(ret <= 0) {
                perror("读取表情包文件失败");
                update_chat_msg(msg_container, "读取表情包文件失败！", 0);
                close(fd);
                return;
            }

            ssize_t sent = send(tcpsock, buf, ret, 0);
            if(sent <= 0) {
                perror("发送表情包数据失败");
                update_chat_msg(msg_container, "表情包发送失败！", 0);
                close(fd);
                return;
            }
            total_sent += sent;
        }

        close(fd);

        // 关键：显示表情包在右侧（自己发送）
        printf("表情包发送成功，大小：%ld字节，现在显示到聊天窗口\n", file_size);
        update_chat_emoji(msg_container, "S:/emoji1.jpg", 1);

    } else {
        printf("错误：TCP未连接\n");
        update_chat_msg(msg_container, "TCP未连接，表情包发送失败!", 0);
        close(fd);
    }

    if(menu_emoji != NULL) {
        lv_obj_del(menu_emoji);
        menu_emoji = NULL;
    }

    lv_refr_now(NULL);
}

// 表情包按钮2发送服务器回调
void emoji_btn2_cb(lv_event_t * e)
{
    lv_obj_t * emoji_btn = lv_event_get_target(e);
    if(emoji_btn == NULL) return;

    // 关键修复：不再通过父容器查找，直接获取当前激活的聊天页面
    lv_obj_t * chat_tab = get_active_chat_page();
    if(chat_tab == NULL) return;

    // 查找滚动容器（聊天页面的第一个子元素）
    lv_obj_t * scroll_container = lv_obj_get_child(chat_tab, 0);
    if(scroll_container == NULL) return;

    // 查找内部消息容器（滚动容器的第一个子元素）
    lv_obj_t * msg_container = lv_obj_get_child(scroll_container, 0);
    if(msg_container == NULL) return;

    int user_idx = -1;
    if(chat_tab == tab1_1) {
        user_idx = 0;
    } else if(chat_tab == tab1_2) {
        user_idx = 1;
    } else if(chat_tab == tab1_3) {
        user_idx = 2;
    } else if(chat_tab == tab1_4) {
        user_idx = 3;
    } else if(chat_tab == tab1_5) {
        user_idx = 4;
    }

    if(user_idx < 0) {
        // 备用方案：通过激活索引匹配
        if(tabview2 != NULL) {
            uint16_t active_tab = lv_tabview_get_tab_act(tabview2);
            if(active_tab >= 0 && active_tab < 5) {
                user_idx = active_tab;
            }
        }
    }

    if(user_idx < 0) return;

    char target_ip[20];
    unsigned short target_port;
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    const char * emoji_path = "/emoji2.jpg";
    int fd                  = open(emoji_path, O_RDONLY);
    if(fd < 0) {
        update_chat_msg(msg_container, "表情包文件打开失败！", 0);
        return;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if(file_size <= 0) {
        update_chat_msg(msg_container, "表情包文件为空！", 0);
        close(fd);
        return;
    }

    char send_header[2048];
    snprintf(send_header, sizeof(send_header), "emoji@%s@%hu@%ld", target_ip, target_port, file_size);

    if(tcpsock > 0) {
        int header_len = send(tcpsock, send_header, strlen(send_header), 0);
        if(header_len <= 0) {
            update_chat_msg(msg_container, "表情包消息头发送失败！", 0);
            close(fd);
            return;
        }

        usleep(100000);

        char buf[1024];
        ssize_t ret, total_sent = 0;
        while(total_sent < file_size) {
            ret = read(fd, buf, sizeof(buf));
            if(ret <= 0) {
                update_chat_msg(msg_container, "读取表情包文件失败！", 0);
                close(fd);
                return;
            }

            ssize_t sent = send(tcpsock, buf, ret, 0);
            if(sent <= 0) {
                update_chat_msg(msg_container, "表情包发送失败！", 0);
                close(fd);
                return;
            }
            total_sent += sent;
        }

        close(fd);
        update_chat_emoji(msg_container, "S:/emoji2.jpg", 1);

    } else {
        update_chat_msg(msg_container, "TCP未连接，表情包发送失败!", 0);
        close(fd);
    }

    if(menu_emoji != NULL) {
        lv_obj_del(menu_emoji);
        menu_emoji = NULL;
    }

    lv_refr_now(NULL);
}

// 表情包按钮3发送服务器回调
void emoji_btn3_cb(lv_event_t * e)
{
    lv_obj_t * emoji_btn = lv_event_get_target(e);
    if(emoji_btn == NULL) return;

    // 关键修复：不再通过父容器查找，直接获取当前激活的聊天页面
    lv_obj_t * chat_tab = get_active_chat_page();
    if(chat_tab == NULL) return;

    // 查找滚动容器（聊天页面的第一个子元素）
    lv_obj_t * scroll_container = lv_obj_get_child(chat_tab, 0);
    if(scroll_container == NULL) return;

    // 查找内部消息容器（滚动容器的第一个子元素）
    lv_obj_t * msg_container = lv_obj_get_child(scroll_container, 0);
    if(msg_container == NULL) return;

    int user_idx = -1;
    if(chat_tab == tab1_1) {
        user_idx = 0;
    } else if(chat_tab == tab1_2) {
        user_idx = 1;
    } else if(chat_tab == tab1_3) {
        user_idx = 2;
    } else if(chat_tab == tab1_4) {
        user_idx = 3;
    } else if(chat_tab == tab1_5) {
        user_idx = 4;
    }

    if(user_idx < 0) {
        // 备用方案：通过激活索引匹配
        if(tabview2 != NULL) {
            uint16_t active_tab = lv_tabview_get_tab_act(tabview2);
            if(active_tab >= 0 && active_tab < 5) {
                user_idx = active_tab;
            }
        }
    }

    if(user_idx < 0) return;

    char target_ip[20];
    unsigned short target_port;
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    const char * emoji_path = "/emoji3.jpg";
    int fd                  = open(emoji_path, O_RDONLY);
    if(fd < 0) {
        update_chat_msg(msg_container, "表情包文件打开失败！", 0);
        return;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if(file_size <= 0) {
        update_chat_msg(msg_container, "表情包文件为空！", 0);
        close(fd);
        return;
    }

    char send_header[2048];
    snprintf(send_header, sizeof(send_header), "emoji@%s@%hu@%ld", target_ip, target_port, file_size);

    if(tcpsock > 0) {
        int header_len = send(tcpsock, send_header, strlen(send_header), 0);
        if(header_len <= 0) {
            update_chat_msg(msg_container, "表情包消息头发送失败！", 0);
            close(fd);
            return;
        }

        usleep(100000);

        char buf[1024];
        ssize_t ret, total_sent = 0;
        while(total_sent < file_size) {
            ret = read(fd, buf, sizeof(buf));
            if(ret <= 0) {
                update_chat_msg(msg_container, "读取表情包文件失败！", 0);
                close(fd);
                return;
            }

            ssize_t sent = send(tcpsock, buf, ret, 0);
            if(sent <= 0) {
                update_chat_msg(msg_container, "表情包发送失败！", 0);
                close(fd);
                return;
            }
            total_sent += sent;
        }

        close(fd);
        update_chat_emoji(msg_container, "S:/emoji3.jpg", 1);

    } else {
        update_chat_msg(msg_container, "TCP未连接，表情包发送失败!", 0);
        close(fd);
    }

    if(menu_emoji != NULL) {
        lv_obj_del(menu_emoji);
        menu_emoji = NULL;
    }

    lv_refr_now(NULL);
}

void emoji_btn4_cb(lv_event_t * e)
{
    lv_obj_t * emoji_btn = lv_event_get_target(e);
    if(emoji_btn == NULL) return;

    // 关键修复：不再通过父容器查找，直接获取当前激活的聊天页面
    lv_obj_t * chat_tab = get_active_chat_page();
    if(chat_tab == NULL) return;

    // 查找滚动容器（聊天页面的第一个子元素）
    lv_obj_t * scroll_container = lv_obj_get_child(chat_tab, 0);
    if(scroll_container == NULL) return;

    // 查找内部消息容器（滚动容器的第一个子元素）
    lv_obj_t * msg_container = lv_obj_get_child(scroll_container, 0);
    if(msg_container == NULL) return;

    int user_idx = -1;
    if(chat_tab == tab1_1) {
        user_idx = 0;
    } else if(chat_tab == tab1_2) {
        user_idx = 1;
    } else if(chat_tab == tab1_3) {
        user_idx = 2;
    } else if(chat_tab == tab1_4) {
        user_idx = 3;
    } else if(chat_tab == tab1_5) {
        user_idx = 4;
    }

    if(user_idx < 0) {
        // 备用方案：通过激活索引匹配
        if(tabview2 != NULL) {
            uint16_t active_tab = lv_tabview_get_tab_act(tabview2);
            if(active_tab >= 0 && active_tab < 5) {
                user_idx = active_tab;
            }
        }
    }

    if(user_idx < 0) return;

    char target_ip[20];
    unsigned short target_port;
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    const char * emoji_path = "/emoji4.jpg";
    int fd                  = open(emoji_path, O_RDONLY);
    if(fd < 0) {
        update_chat_msg(msg_container, "表情包文件打开失败！", 0);
        return;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if(file_size <= 0) {
        update_chat_msg(msg_container, "表情包文件为空！", 0);
        close(fd);
        return;
    }

    char send_header[2048];
    snprintf(send_header, sizeof(send_header), "emoji@%s@%hu@%ld", target_ip, target_port, file_size);

    if(tcpsock > 0) {
        int header_len = send(tcpsock, send_header, strlen(send_header), 0);
        if(header_len <= 0) {
            update_chat_msg(msg_container, "表情包消息头发送失败！", 0);
            close(fd);
            return;
        }

        usleep(100000);

        char buf[1024];
        ssize_t ret, total_sent = 0;
        while(total_sent < file_size) {
            ret = read(fd, buf, sizeof(buf));
            if(ret <= 0) {
                update_chat_msg(msg_container, "读取表情包文件失败！", 0);
                close(fd);
                return;
            }

            ssize_t sent = send(tcpsock, buf, ret, 0);
            if(sent <= 0) {
                update_chat_msg(msg_container, "表情包发送失败！", 0);
                close(fd);
                return;
            }
            total_sent += sent;
        }

        close(fd);
        update_chat_emoji(msg_container, "S:/emoji4.jpg", 1);

    } else {
        update_chat_msg(msg_container, "TCP未连接，表情包发送失败!", 0);
        close(fd);
    }

    if(menu_emoji != NULL) {
        lv_obj_del(menu_emoji);
        menu_emoji = NULL;
    }

    lv_refr_now(NULL);
}

// 打开表情包按钮回调函数
void emoji_btn_cb(lv_event_t * e)
{
    lv_obj_t * emoji_btn = lv_event_get_target(e);
    if(emoji_btn == NULL) {
        printf("错误：表情包按钮为空\n");
        return;
    }

    // 使用新的容器查找逻辑
    // 表情包按钮 → 输入容器 → 聊天页面 → 滚动容器
    lv_obj_t * input_container = lv_obj_get_parent(emoji_btn);
    if(input_container == NULL) {
        printf("错误：找不到输入容器\n");
        return;
    }

    lv_obj_t * chat_tab = lv_obj_get_parent(input_container);
    if(chat_tab == NULL) {
        printf("错误：找不到聊天页面\n");
        return;
    }

    // 关键修复：查找滚动容器（聊天页面的第一个子元素），但不创建在内部消息容器中
    lv_obj_t * scroll_container = lv_obj_get_child(chat_tab, 0);
    if(scroll_container == NULL) {
        printf("错误：找不到滚动容器\n");
        return;
    }

    printf("找到滚动容器：%p，聊天页面：%p\n", scroll_container, chat_tab);

    // 1. 先删除旧窗口（如果存在）
    if(menu_emoji != NULL) {
        lv_obj_del(menu_emoji);
        menu_emoji = NULL;
    }

    // 2. 关键修复：创建表情包窗口（父对象改为聊天页面，而不是内部消息容器）
    menu_emoji = lv_obj_create(chat_tab);

    // 设置窗口大小和位置
    lv_obj_set_size(menu_emoji, 300, 250);
    lv_obj_add_flag(menu_emoji, LV_OBJ_FLAG_FLOATING);

    // 关键修复：将窗口定位在聊天页面的底部，输入框上方
    int window_x = 20;             // 左边距
    int window_y = 350 - 250 - 20; // 在消息区域底部，输入框上方，有10px间距

    lv_obj_set_pos(menu_emoji, window_x, window_y);

    // 3. 设置窗口样式
    lv_obj_set_style_bg_color(menu_emoji, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_opa(menu_emoji, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(menu_emoji, 1, 0);
    lv_obj_set_style_border_color(menu_emoji, lv_color_hex(0xDCDCDC), 0);
    lv_obj_set_style_pad_all(menu_emoji, 0, 0);
    lv_obj_set_style_radius(menu_emoji, 5, 0);

    // 添加阴影效果，使其看起来是浮动窗口
    lv_obj_set_style_shadow_width(menu_emoji, 10, 0);
    lv_obj_set_style_shadow_color(menu_emoji, lv_color_hex(0x666666), 0);
    lv_obj_set_style_shadow_ofs_x(menu_emoji, 0, 0);
    lv_obj_set_style_shadow_ofs_y(menu_emoji, 2, 0);

    // 4. 创建关闭按钮（右上角）
    lv_obj_t * close_btn = lv_btn_create(menu_emoji);
    lv_obj_set_size(close_btn, 25, 25);
    lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, -5, 5);

    lv_obj_set_style_bg_color(close_btn, lv_color_hex(0x00bfff), 0);
    lv_obj_set_style_bg_opa(close_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(close_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(close_btn, 0, 0);

    lv_obj_t * close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, "X");
    lv_obj_set_style_text_color(close_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(close_label, &lv_font_montserrat_12, 0);
    lv_obj_center(close_label);

    lv_obj_add_event_cb(close_btn, emoji_close_btn_cb, LV_EVENT_CLICKED, NULL);

    // 5. 创建标题栏
    lv_obj_t * title_label = lv_label_create(menu_emoji);
    lv_label_set_text(title_label, "MY EMOJI");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0x333333), 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 10, 15);

    // 6. 创建分隔线
    lv_obj_t * separator = lv_obj_create(menu_emoji);
    lv_obj_remove_style_all(separator);
    lv_obj_set_size(separator, 280, 1);
    lv_obj_align_to(separator, title_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_obj_set_style_bg_color(separator, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_style_bg_opa(separator, LV_OPA_COVER, 0);

    // 7. 创建表情包按钮容器
    lv_obj_t * emoji_container = lv_obj_create(menu_emoji);
    lv_obj_remove_style_all(emoji_container);
    lv_obj_set_size(emoji_container, 280, 170);
    lv_obj_align_to(emoji_container, separator, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_obj_set_style_bg_opa(emoji_container, LV_OPA_TRANSP, 0);

    // 8. 创建表情包按钮1
    lv_obj_t * emoji_btn1 = lv_btn_create(emoji_container);
    lv_obj_set_size(emoji_btn1, 50, 50);
    lv_obj_set_pos(emoji_btn1, 10, 10);
    lv_obj_set_style_radius(emoji_btn1, 5, 0);
    lv_obj_set_style_bg_color(emoji_btn1, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_bg_opa(emoji_btn1, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(emoji_btn1, 0, 0);

    lv_obj_t * emoji_img1 = lv_img_create(emoji_btn1);
    lv_img_set_src(emoji_img1, "S:/emoji1.jpg");
    lv_obj_set_size(emoji_img1, 50, 50);
    lv_obj_center(emoji_img1);

    lv_obj_add_event_cb(emoji_btn1, emoji_btn1_cb, LV_EVENT_CLICKED, NULL);

    // 9. 创建表情包按钮2
    lv_obj_t * emoji_btn2 = lv_btn_create(emoji_container);
    lv_obj_set_size(emoji_btn2, 50, 50);
    lv_obj_align_to(emoji_btn2, emoji_btn1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style_radius(emoji_btn2, 5, 0);
    lv_obj_set_style_bg_color(emoji_btn2, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_bg_opa(emoji_btn2, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(emoji_btn2, 0, 0);

    lv_obj_t * emoji_img2 = lv_img_create(emoji_btn2);
    lv_img_set_src(emoji_img2, "S:/emoji2.jpg");
    lv_obj_set_size(emoji_img2, 50, 50);
    lv_obj_center(emoji_img2);

    lv_obj_add_event_cb(emoji_btn2, emoji_btn2_cb, LV_EVENT_CLICKED, NULL);

    // 10. 创建表情包按钮3
    lv_obj_t * emoji_btn3 = lv_btn_create(emoji_container);
    lv_obj_set_size(emoji_btn3, 50, 50);
    lv_obj_align_to(emoji_btn3, emoji_btn2, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style_radius(emoji_btn3, 5, 0);
    lv_obj_set_style_bg_color(emoji_btn3, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_bg_opa(emoji_btn3, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(emoji_btn3, 0, 0);

    lv_obj_t * emoji_img3 = lv_img_create(emoji_btn3);
    lv_img_set_src(emoji_img3, "S:/emoji3.jpg");
    lv_obj_set_size(emoji_img3, 50, 50);
    lv_obj_center(emoji_img3);

    lv_obj_add_event_cb(emoji_btn3, emoji_btn3_cb, LV_EVENT_CLICKED, NULL);

    // 11. 可选：创建第二行表情包
    lv_obj_t * emoji_btn4 = lv_btn_create(emoji_container);
    lv_obj_set_size(emoji_btn4, 50, 50);
    lv_obj_align_to(emoji_btn4, emoji_btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_set_style_radius(emoji_btn4, 5, 0);
    lv_obj_set_style_bg_color(emoji_btn4, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_bg_opa(emoji_btn4, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(emoji_btn4, 0, 0);

    lv_obj_t * emoji_img4 = lv_img_create(emoji_btn4);
    lv_img_set_src(emoji_img4, "S:/emoji4.jpg");
    lv_obj_set_size(emoji_img4, 50, 50);
    lv_obj_center(emoji_img4);

    lv_obj_add_event_cb(emoji_btn4, emoji_btn4_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * emoji_btn5 = lv_btn_create(emoji_container);
    lv_obj_set_size(emoji_btn5, 50, 50);
    lv_obj_align_to(emoji_btn5, emoji_btn4, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style_radius(emoji_btn5, 5, 0);
    lv_obj_set_style_bg_color(emoji_btn5, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_bg_opa(emoji_btn5, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(emoji_btn5, 0, 0);

    lv_obj_t * emoji_img5 = lv_img_create(emoji_btn5);
    lv_img_set_src(emoji_img5, "S:/emoji3.jpg");
    lv_obj_set_size(emoji_img5, 50, 50);
    lv_obj_center(emoji_img5);

    lv_obj_add_event_cb(emoji_btn5, emoji_btn3_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * emoji_btn6 = lv_btn_create(emoji_container);
    lv_obj_set_size(emoji_btn6, 50, 50);
    lv_obj_align_to(emoji_btn6, emoji_btn3, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style_radius(emoji_btn6, 5, 0);
    lv_obj_set_style_bg_color(emoji_btn6, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_bg_opa(emoji_btn6, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(emoji_btn6, 0, 0);

    lv_obj_t * emoji_img6 = lv_img_create(emoji_btn6);
    lv_img_set_src(emoji_img6, "S:/emoji1.jpg");
    lv_obj_set_size(emoji_img6, 50, 50);
    lv_obj_center(emoji_img6);

    lv_obj_add_event_cb(emoji_btn6, emoji_btn1_cb, LV_EVENT_CLICKED, NULL);
}

// 文件窗口关闭按钮回调
void file_close_btn_cb(lv_event_t * e)
{
    if(menu_file != NULL) {
        lv_obj_del(menu_file);
        menu_file = NULL;
    }
}
void file_btn1_cb(lv_event_t * e)
{
    lv_obj_t * file_btn = lv_event_get_target(e);
    if(file_btn == NULL) {
        printf("错误：文件按钮为空\n");
        return;
    }

    // 获取当前激活的聊天页面
    lv_obj_t * chat_tab = get_active_chat_page();
    if(chat_tab == NULL) {
        printf("错误：无法获取当前激活的聊天页面\n");
        return;
    }

    // 查找滚动容器
    lv_obj_t * scroll_container = lv_obj_get_child(chat_tab, 0);
    if(scroll_container == NULL) {
        printf("错误：找不到滚动容器\n");
        return;
    }

    // 查找内部消息容器
    lv_obj_t * msg_container = lv_obj_get_child(scroll_container, 0);
    if(msg_container == NULL) {
        printf("错误：找不到内部消息容器\n");
        return;
    }

    int user_idx = -1;
    if(chat_tab == tab1_1) {
        user_idx = 0;
    } else if(chat_tab == tab1_2) {
        user_idx = 1;
    } else if(chat_tab == tab1_3) {
        user_idx = 2;
    } else if(chat_tab == tab1_4) {
        user_idx = 3;
    } else if(chat_tab == tab1_5) {
        user_idx = 4;
    }

    if(user_idx < 0) {
        printf("错误：用户索引无效\n");
        return;
    }

    char target_ip[20];
    unsigned short target_port;
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    const char * file_path = "/code.txt";
    int fd                 = open(file_path, O_RDONLY);
    if(fd < 0) {
        perror("打开文件失败");
        update_chat_msg(msg_container, "文件打开失败！", 0);
        return;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if(file_size <= 0) {
        printf("错误：文件为空\n");
        update_chat_msg(msg_container, "文件为空！", 0);
        close(fd);
        return;
    }

    printf("文件大小：%ld字节\n", file_size);

    // 获取文件扩展名
    const char * file_ext = strrchr(file_path, '.');
    if(file_ext == NULL) file_ext = "";

    char send_header[2048];
    // 格式：file@目标IP@目标端口@文件大小@文件类型@文件名
    const char * filename = strrchr(file_path, '/');
    if(filename == NULL)
        filename = file_path;
    else
        filename++;

    snprintf(send_header, sizeof(send_header), "file@%s@%hu@%ld@%s@%s", target_ip, target_port, file_size, file_ext,
             filename);

    if(tcpsock > 0) {
        int header_len = send(tcpsock, send_header, strlen(send_header), 0);
        if(header_len <= 0) {
            perror("发送文件消息头失败");
            update_chat_msg(msg_container, "文件消息头发送失败！", 0);
            close(fd);
            return;
        }

        printf("文件消息头发送成功，长度：%d\n", header_len);

        usleep(100000);

        char buf[1024];
        ssize_t ret, total_sent = 0;
        while(total_sent < file_size) {
            ret = read(fd, buf, sizeof(buf));
            if(ret <= 0) {
                perror("读取文件失败");
                update_chat_msg(msg_container, "读取文件失败！", 0);
                close(fd);
                return;
            }

            ssize_t sent = send(tcpsock, buf, ret, 0);
            if(sent <= 0) {
                perror("发送数据失败");
                update_chat_msg(msg_container, "文件发送失败！", 0);
                close(fd);
                return;
            }
            total_sent += sent;
        }

        close(fd);

        // 关键：显示文件在右侧（自己发送）
        printf("文件发送成功，大小：%ld字节，现在显示到聊天窗口\n", file_size);
        update_chat_file(msg_container, file_path, 1);

    } else {
        printf("错误：TCP未连接\n");
        update_chat_msg(msg_container, "TCP未连接，文件发送失败!", 0);
        close(fd);
    }

    if(menu_file != NULL) {
        lv_obj_del(menu_file);
        menu_file = NULL;
    }

    lv_refr_now(NULL);
}
// 文件按钮2回调函数
void file_btn2_cb(lv_event_t * e)
{
    lv_obj_t * file_btn = lv_event_get_target(e);
    if(file_btn == NULL) {
        printf("错误：文件按钮为空\n");
        return;
    }

    // 获取当前激活的聊天页面
    lv_obj_t * chat_tab = get_active_chat_page();
    if(chat_tab == NULL) {
        printf("错误：无法获取当前激活的聊天页面\n");
        return;
    }

    // 查找滚动容器
    lv_obj_t * scroll_container = lv_obj_get_child(chat_tab, 0);
    if(scroll_container == NULL) {
        printf("错误：找不到滚动容器\n");
        return;
    }

    // 查找内部消息容器
    lv_obj_t * msg_container = lv_obj_get_child(scroll_container, 0);
    if(msg_container == NULL) {
        printf("错误：找不到内部消息容器\n");
        return;
    }

    int user_idx = -1;
    if(chat_tab == tab1_1) {
        user_idx = 0;
    } else if(chat_tab == tab1_2) {
        user_idx = 1;
    } else if(chat_tab == tab1_3) {
        user_idx = 2;
    } else if(chat_tab == tab1_4) {
        user_idx = 3;
    } else if(chat_tab == tab1_5) {
        user_idx = 4;
    }

    if(user_idx < 0) {
        printf("错误：用户索引无效\n");
        return;
    }

    char target_ip[20];
    unsigned short target_port;
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    const char * file_path = "/code.docx";
    int fd                 = open(file_path, O_RDONLY);
    if(fd < 0) {
        perror("打开文件失败");
        update_chat_msg(msg_container, "文件打开失败！", 0);
        return;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if(file_size <= 0) {
        printf("错误：文件为空\n");
        update_chat_msg(msg_container, "文件为空！", 0);
        close(fd);
        return;
    }

    printf("文件大小：%ld字节\n", file_size);

    // 获取文件扩展名
    const char * file_ext = strrchr(file_path, '.');
    if(file_ext == NULL) file_ext = "";

    char send_header[2048];
    // 格式：file@目标IP@目标端口@文件大小@文件类型@文件名
    const char * filename = strrchr(file_path, '/');
    if(filename == NULL)
        filename = file_path;
    else
        filename++;

    snprintf(send_header, sizeof(send_header), "file@%s@%hu@%ld@%s@%s", target_ip, target_port, file_size, file_ext,
             filename);

    if(tcpsock > 0) {
        int header_len = send(tcpsock, send_header, strlen(send_header), 0);
        if(header_len <= 0) {
            perror("发送文件消息头失败");
            update_chat_msg(msg_container, "文件消息头发送失败！", 0);
            close(fd);
            return;
        }

        printf("文件消息头发送成功，长度：%d\n", header_len);

        usleep(100000);

        char buf[1024];
        ssize_t ret, total_sent = 0;
        while(total_sent < file_size) {
            ret = read(fd, buf, sizeof(buf));
            if(ret <= 0) {
                perror("读取文件失败");
                update_chat_msg(msg_container, "读取文件失败！", 0);
                close(fd);
                return;
            }

            ssize_t sent = send(tcpsock, buf, ret, 0);
            if(sent <= 0) {
                perror("发送数据失败");
                update_chat_msg(msg_container, "文件发送失败！", 0);
                close(fd);
                return;
            }
            total_sent += sent;
        }

        close(fd);

        // 关键：显示文件在右侧（自己发送）
        printf("文件发送成功，大小：%ld字节，现在显示到聊天窗口\n", file_size);
        update_chat_file(msg_container, file_path, 1);

    } else {
        printf("错误：TCP未连接\n");
        update_chat_msg(msg_container, "TCP未连接，文件发送失败!", 0);
        close(fd);
    }

    if(menu_file != NULL) {
        lv_obj_del(menu_file);
        menu_file = NULL;
    }

    lv_refr_now(NULL);
}

// 文件按钮3回调函数
void file_btn3_cb(lv_event_t * e)
{
    lv_obj_t * file_btn = lv_event_get_target(e);
    if(file_btn == NULL) {
        printf("错误：文件按钮为空\n");
        return;
    }

    // 获取当前激活的聊天页面
    lv_obj_t * chat_tab = get_active_chat_page();
    if(chat_tab == NULL) {
        printf("错误：无法获取当前激活的聊天页面\n");
        return;
    }

    // 查找滚动容器
    lv_obj_t * scroll_container = lv_obj_get_child(chat_tab, 0);
    if(scroll_container == NULL) {
        printf("错误：找不到滚动容器\n");
        return;
    }

    // 查找内部消息容器
    lv_obj_t * msg_container = lv_obj_get_child(scroll_container, 0);
    if(msg_container == NULL) {
        printf("错误：找不到内部消息容器\n");
        return;
    }

    int user_idx = -1;
    if(chat_tab == tab1_1) {
        user_idx = 0;
    } else if(chat_tab == tab1_2) {
        user_idx = 1;
    } else if(chat_tab == tab1_3) {
        user_idx = 2;
    } else if(chat_tab == tab1_4) {
        user_idx = 3;
    } else if(chat_tab == tab1_5) {
        user_idx = 4;
    }

    if(user_idx < 0) {
        printf("错误：用户索引无效\n");
        return;
    }

    char target_ip[20];
    unsigned short target_port;
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    const char * file_path = "/code.txt";
    int fd                 = open(file_path, O_RDONLY);
    if(fd < 0) {
        perror("打开文件失败");
        update_chat_msg(msg_container, "文件打开失败！", 0);
        return;
    }

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    if(file_size <= 0) {
        printf("错误：文件为空\n");
        update_chat_msg(msg_container, "文件为空！", 0);
        close(fd);
        return;
    }

    printf("文件大小：%ld字节\n", file_size);

    // 获取文件扩展名
    const char * file_ext = strrchr(file_path, '.');
    if(file_ext == NULL) file_ext = "";

    char send_header[2048];
    // 格式：file@目标IP@目标端口@文件大小@文件类型@文件名
    const char * filename = strrchr(file_path, '/');
    if(filename == NULL)
        filename = file_path;
    else
        filename++;

    snprintf(send_header, sizeof(send_header), "file@%s@%hu@%ld@%s@%s", target_ip, target_port, file_size, file_ext,
             filename);

    if(tcpsock > 0) {
        int header_len = send(tcpsock, send_header, strlen(send_header), 0);
        if(header_len <= 0) {
            perror("发送文件消息头失败");
            update_chat_msg(msg_container, "文件消息头发送失败！", 0);
            close(fd);
            return;
        }

        printf("文件消息头发送成功，长度：%d\n", header_len);

        usleep(100000);

        char buf[1024];
        ssize_t ret, total_sent = 0;
        while(total_sent < file_size) {
            ret = read(fd, buf, sizeof(buf));
            if(ret <= 0) {
                perror("读取文件失败");
                update_chat_msg(msg_container, "读取文件失败！", 0);
                close(fd);
                return;
            }

            ssize_t sent = send(tcpsock, buf, ret, 0);
            if(sent <= 0) {
                perror("发送数据失败");
                update_chat_msg(msg_container, "文件发送失败！", 0);
                close(fd);
                return;
            }
            total_sent += sent;
        }

        close(fd);

        // 关键：显示文件在右侧（自己发送）
        printf("文件发送成功，大小：%ld字节，现在显示到聊天窗口\n", file_size);
        update_chat_file(msg_container, file_path, 1);

    } else {
        printf("错误：TCP未连接\n");
        update_chat_msg(msg_container, "TCP未连接，文件发送失败!", 0);
        close(fd);
    }

    if(menu_file != NULL) {
        lv_obj_del(menu_file);
        menu_file = NULL;
    }

    lv_refr_now(NULL);
}

// 文件按钮回调函数
void file_btn_cb(lv_event_t * e)
{
    lv_obj_t * file_btn = lv_event_get_target(e);
    if(file_btn == NULL) {
        printf("错误：文件按钮为空\n");
        return;
    }

    // 使用新的容器查找逻辑
    // 文件按钮 → 输入容器 → 聊天页面 → 滚动容器
    lv_obj_t * input_container = lv_obj_get_parent(file_btn);
    if(input_container == NULL) {
        printf("错误：找不到输入容器\n");
        return;
    }

    lv_obj_t * chat_tab = lv_obj_get_parent(input_container);
    if(chat_tab == NULL) {
        printf("错误：找不到聊天页面\n");
        return;
    }

    // 关键修复：查找滚动容器（聊天页面的第一个子元素），但不创建在内部消息容器中
    lv_obj_t * scroll_container = lv_obj_get_child(chat_tab, 0);
    if(scroll_container == NULL) {
        printf("错误：找不到滚动容器\n");
        return;
    }

    printf("找到滚动容器：%p，聊天页面：%p\n", scroll_container, chat_tab);

    // 1. 先删除旧窗口（如果存在）
    if(menu_file != NULL) {
        lv_obj_del(menu_file);
        menu_file = NULL;
    }

    // 2. 关键修复：创建表情包窗口（父对象改为聊天页面，而不是内部消息容器）
    menu_file = lv_obj_create(chat_tab);

    // 设置窗口大小和位置
    lv_obj_set_size(menu_file, 300, 250);
    lv_obj_add_flag(menu_file, LV_OBJ_FLAG_FLOATING);

    // 关键修复：将窗口定位在聊天页面的底部，输入框上方
    int window_x = 50;             // 左边距
    int window_y = 350 - 250 - 20; // 在消息区域底部，输入框上方，有10px间距

    lv_obj_set_pos(menu_file, window_x, window_y);

    // 3. 设置窗口样式
    lv_obj_set_style_bg_color(menu_file, lv_color_hex(0xffffff), 0);
    lv_obj_set_style_bg_opa(menu_file, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(menu_file, 1, 0);
    lv_obj_set_style_border_color(menu_file, lv_color_hex(0xDCDCDC), 0);
    lv_obj_set_style_pad_all(menu_file, 0, 0);
    lv_obj_set_style_radius(menu_file, 5, 0);

    // 添加阴影效果，使其看起来是浮动窗口
    lv_obj_set_style_shadow_width(menu_file, 10, 0);
    lv_obj_set_style_shadow_color(menu_file, lv_color_hex(0x666666), 0);
    lv_obj_set_style_shadow_ofs_x(menu_file, 0, 0);
    lv_obj_set_style_shadow_ofs_y(menu_file, 2, 0);

    // 4. 创建关闭按钮（右上角）
    lv_obj_t * close_btn = lv_btn_create(menu_file);
    lv_obj_set_size(close_btn, 25, 25);
    lv_obj_align(close_btn, LV_ALIGN_TOP_RIGHT, -5, 5);

    lv_obj_set_style_bg_color(close_btn, lv_color_hex(0x00bfff), 0);
    lv_obj_set_style_bg_opa(close_btn, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(close_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(close_btn, 0, 0);

    lv_obj_t * close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, "X");
    lv_obj_set_style_text_color(close_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(close_label, &lv_font_montserrat_12, 0);
    lv_obj_center(close_label);

    lv_obj_add_event_cb(close_btn, file_close_btn_cb, LV_EVENT_CLICKED, NULL);

    // 5. 创建标题栏
    lv_obj_t * title_label = lv_label_create(menu_file);
    lv_label_set_text(title_label, "MY FILE");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0x333333), 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 10, 15);

    // 6. 创建分隔线
    lv_obj_t * separator = lv_obj_create(menu_file);
    lv_obj_remove_style_all(separator);
    lv_obj_set_size(separator, 280, 1);
    lv_obj_align_to(separator, title_label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_obj_set_style_bg_color(separator, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_style_bg_opa(separator, LV_OPA_COVER, 0);

    // 7. 创建文件按钮容器
    lv_obj_t * emoji_container = lv_obj_create(menu_file);
    lv_obj_remove_style_all(emoji_container);
    lv_obj_set_size(emoji_container, 280, 170);
    lv_obj_align_to(emoji_container, separator, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_obj_set_style_bg_opa(emoji_container, LV_OPA_TRANSP, 0);

    // 8. 创建文件按钮1
    lv_obj_t * file_btn1 = lv_btn_create(emoji_container);
    lv_obj_set_size(file_btn1, 200, 50);
    lv_obj_set_pos(file_btn1, 20, 10);
    lv_obj_set_style_radius(file_btn1, 5, 0);
    lv_obj_set_style_bg_color(file_btn1, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_bg_opa(file_btn1, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(file_btn1, 0, 0);

    lv_obj_t * file_img1 = lv_img_create(file_btn1);
    lv_img_set_src(file_img1, "S:/file1.jpg");
    lv_obj_set_size(file_img1, 200, 50);
    lv_obj_center(file_img1);

    lv_obj_add_event_cb(file_btn1, file_btn1_cb, LV_EVENT_CLICKED, NULL);

    // 9. 创建文件按钮2
    lv_obj_t * file_btn2 = lv_btn_create(emoji_container);
    lv_obj_set_size(file_btn2, 200, 50);
    lv_obj_align_to(file_btn2, file_btn1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_set_style_radius(file_btn2, 5, 0);
    lv_obj_set_style_bg_color(file_btn2, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_bg_opa(file_btn2, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(file_btn2, 0, 0);

    lv_obj_t * emoji_img2 = lv_img_create(file_btn2);
    lv_img_set_src(emoji_img2, "S:/file2.jpg");
    lv_obj_set_size(emoji_img2, 200, 50);
    lv_obj_center(emoji_img2);

    lv_obj_add_event_cb(file_btn2, file_btn2_cb, LV_EVENT_CLICKED, NULL);

    // 10. 创建文件按钮3
    lv_obj_t * file_btn3 = lv_btn_create(emoji_container);
    lv_obj_set_size(file_btn3, 200, 50);
    lv_obj_align_to(file_btn3, file_btn2, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_set_style_radius(file_btn3, 5, 0);
    lv_obj_set_style_bg_color(file_btn3, lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_bg_opa(file_btn3, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(file_btn3, 0, 0);

    lv_obj_t * emoji_img3 = lv_img_create(file_btn3);
    lv_img_set_src(emoji_img3, "S:/file1.jpg");
    lv_obj_set_size(emoji_img3, 200, 50);
    lv_obj_center(emoji_img3);

    lv_obj_add_event_cb(file_btn3, file_btn3_cb, LV_EVENT_CLICKED, NULL);
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

msg_window_t chat_windows[5] = {
    {NULL, 0, 20}, {NULL, 0, 20}, {NULL, 0, 20}, {NULL, 0, 20}, {NULL, 0, 20}}; // 修复：初始last_y改为20

// 新增：显示文件到聊天窗口
void update_chat_file(lv_obj_t * msg_container, const char * file_path, int is_self)
{
    if(msg_container == NULL) {
        printf("错误：消息容器为空\n");
        return;
    }

    if(file_path == NULL) {
        printf("错误：文件路径为空\n");
        return;
    }

    printf("开始显示文件：%s (is_self=%d)\n", file_path, is_self);

    // 1. 匹配聊天窗口索引
    int win_idx = -1;
    for(int i = 0; i < 5; i++) {
        if(msg_windows[i].msg_container == msg_container) {
            win_idx = i;
            break;
        }
    }
    if(win_idx == -1) {
        printf("错误：找不到匹配的窗口索引\n");
        return;
    }

    // 2. 创建外层容器
    lv_obj_t * file_container = lv_obj_create(msg_container);
    lv_obj_remove_style_all(file_container);

    // 设置文件容器样式
    lv_obj_set_size(file_container, 200, 50); 
    lv_obj_set_style_radius(file_container, 0, 0);
    lv_obj_set_style_bg_opa(file_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(file_container, 0, 0);
    lv_obj_set_style_pad_all(file_container, 0, 0);

    // 3. 创建背景容器
    lv_obj_t * bg_container = lv_obj_create(file_container);
    lv_obj_remove_style_all(bg_container);
    
    if(is_self) {
        lv_obj_set_style_bg_color(bg_container, lv_color_hex(0x07C160), 0);
        lv_obj_set_style_bg_opa(bg_container, LV_OPA_30, 0);
    } else {
        lv_obj_set_style_bg_color(bg_container, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_bg_opa(bg_container, LV_OPA_30, 0);
    }
    lv_obj_set_style_radius(bg_container, 0, 0);
    lv_obj_set_size(bg_container, 200, 50);
    lv_obj_set_style_border_width(bg_container, 1, 0);
    lv_obj_set_style_border_color(bg_container, lv_color_hex(0xCCCCCC), 0);

    // 4. 创建图片控件
    lv_obj_t * file_img = lv_img_create(file_container);
    
    // 根据文件类型选择不同的文件图片
    if(strstr(file_path, ".jpg") != NULL || strstr(file_path, ".jpeg") != NULL || 
       strstr(file_path, ".png") != NULL || strstr(file_path, ".bmp") != NULL) {
        // 图片文件
        lv_img_set_src(file_img, "S:/file1.jpg");
    } 
    else if(strstr(file_path, ".txt") != NULL || strstr(file_path, ".c") != NULL || 
            strstr(file_path, ".cpp") != NULL || strstr(file_path, ".h") != NULL) {
        // 文本文件
        lv_img_set_src(file_img, "S:/file1.jpg");  // 文本文件图标
    }
    else if(strstr(file_path, ".doc") != NULL || strstr(file_path, ".docx") != NULL) {
        // Word文档
        lv_img_set_src(file_img, "S:/file2.jpg");  // docx文件图标
    }
    
    lv_obj_set_size(file_img, 200, 50);
    lv_obj_center(file_img);
    lv_obj_set_style_radius(file_img, 6, 0);
    lv_obj_set_style_border_width(file_img, 1, 0);
    lv_obj_set_style_border_color(file_img, lv_color_hex(0xCCCCCC), 0);


    // 6. 计算位置
    msg_window_t * win = &msg_windows[win_idx];
    int file_height    = 60;
    int y_pos          = win->last_y;

    lv_obj_set_y(file_container, y_pos);

    if(is_self) {
        // 自己发送：右对齐
        lv_obj_set_x(file_container, 560 - 200 - 20);
    } else {
        // 对方发送：左对齐
        lv_obj_set_x(file_container, 20);
    }

    // 7. 更新容器高度和窗口状态
    int new_message_bottom   = y_pos + file_height;
    int new_container_height = new_message_bottom + 20;

    if(new_container_height > win->container_height) {
        win->container_height = new_container_height;
        lv_obj_set_height(msg_container, win->container_height);
    }

    win->msg_count++;
    win->last_y = y_pos + file_height + 10;

    printf("文件消息高度：%d, 位置Y：%d\n", file_height, y_pos);
    printf("更新文件消息计数：%d, 最后Y位置：%d\n", win->msg_count, win->last_y);

    // 8. 自动滚动到最新文件
    int scroll_target = win->last_y - 350;
    if(scroll_target < 0) scroll_target = 0;

    if(win->scroll_container != NULL) {
        lv_obj_scroll_to_y(win->scroll_container, scroll_target, LV_ANIM_ON);
        printf("滚动到位置：%d\n", scroll_target);
    }

    printf("文件显示完成\n");
}

// 修复：更新LVGL聊天界面消息
void update_chat_msg(lv_obj_t * msg_container, const char * msg, int is_self)
{
    if(msg_container == NULL) {
        printf("错误：消息容器为空\n");
        return;
    }

    if(msg == NULL || strlen(msg) == 0) {
        printf("错误：消息内容为空\n");
        return;
    }

    printf("开始显示消息：%s (is_self=%d)\n", msg, is_self);

    // 步骤1：匹配当前聊天窗口索引
    int win_idx = -1;
    for(int i = 0; i < 5; i++) {
        if(msg_windows[i].msg_container == msg_container) {
            win_idx = i;
            break;
        }
    }
    // 首次初始化聊天窗口
    if(win_idx == -1) {
        printf("首次初始化聊天窗口\n");
        for(int i = 0; i < 5; i++) {
            if(msg_windows[i].msg_container == NULL) {
                msg_windows[i].msg_container    = msg_container;
                msg_windows[i].msg_count        = 0;
                msg_windows[i].container_height = 350;
                msg_windows[i].last_y           = 20;
                win_idx                         = i;
                printf("初始化窗口索引：%d\n", i);
                break;
            }
        }
    }
    if(win_idx == -1) {
        printf("错误：找不到可用的窗口索引\n");
        return;
    }

    // 步骤2：创建消息气泡容器
    lv_obj_t * msg_bubble = lv_obj_create(msg_container);
    lv_obj_remove_style_all(msg_bubble);

    // 设置消息气泡基本样式
    lv_obj_set_style_pad_all(msg_bubble, 12, 0);
    lv_obj_set_style_pad_left(msg_bubble, 15, 0);
    lv_obj_set_style_pad_right(msg_bubble, 15, 0);
    lv_obj_set_style_radius(msg_bubble, 10, 0);
    lv_obj_set_style_border_width(msg_bubble, 0, 0);

    if(is_self) {
        // 自己消息样式（绿色，右对齐）
        lv_obj_set_style_bg_color(msg_bubble, lv_color_hex(0x07C160), 0);
        lv_obj_set_style_bg_opa(msg_bubble, LV_OPA_COVER, 0);
    } else {
        // 对方消息样式（浅灰色，左对齐）
        lv_obj_set_style_bg_color(msg_bubble, lv_color_hex(0xffffff), 0);
        lv_obj_set_style_bg_opa(msg_bubble, LV_OPA_COVER, 0);
    }

    // 步骤3：创建消息标签
    lv_obj_t * msg_label = lv_label_create(msg_bubble);
    char msg_buf[256]    = {0};
    strncpy(msg_buf, msg, 255);
    lv_label_set_text(msg_label, msg_buf);

    // 启用文本换行
    lv_label_set_long_mode(msg_label, LV_LABEL_LONG_WRAP);

    // 根据内容自适应宽度
    int text_length     = strlen(msg);
    int estimated_width = text_length * 8;
    int container_width = 560;                        // 内部消息容器宽度
    int max_width       = container_width * 60 / 100; // 最大宽度为容器60%

    if(estimated_width < 100) {
        lv_obj_set_width(msg_bubble, estimated_width + 30); // 短消息
    } else if(estimated_width < max_width) {
        lv_obj_set_width(msg_bubble, estimated_width);
    } else {
        lv_obj_set_width(msg_bubble, max_width); // 长消息限制宽度
    }

    lv_obj_set_height(msg_bubble, LV_SIZE_CONTENT);

    // 设置标签样式
    lv_obj_set_style_text_font(msg_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(msg_label, is_self ? lv_color_hex(0x000000) : lv_color_hex(0x000000), 0);
    lv_obj_set_width(msg_label, lv_pct(100));
    lv_obj_set_style_text_align(msg_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_style_pad_all(msg_label, 0, 0);

    // 步骤4：计算消息位置
    msg_window_t * win = &msg_windows[win_idx];
    lv_obj_update_layout(msg_bubble);
    int msg_height = lv_obj_get_height(msg_bubble);
    int y_pos      = win->last_y;

    printf("消息高度：%d, 位置Y：%d\n", msg_height, y_pos);

    // 设置消息气泡位置
    lv_obj_set_y(msg_bubble, y_pos);

    if(is_self) {
        // 右对齐
        int bubble_width = lv_obj_get_width(msg_bubble);
        lv_obj_set_x(msg_bubble, container_width - bubble_width - 20);
    } else {
        // 左对齐
        lv_obj_set_x(msg_bubble, 20);
    }

    // 步骤5：更新容器高度和窗口状态
    int new_message_bottom   = y_pos + msg_height;
    int new_container_height = new_message_bottom + 20; // 增加底部间距

    if(new_container_height > win->container_height) {
        win->container_height = new_container_height;
        lv_obj_set_height(msg_container, win->container_height);
        printf("更新容器高度到：%d\n", win->container_height);
    }

    win->msg_count++;
    win->last_y = y_pos + msg_height + 10; // 消息间距10px

    printf("更新消息计数：%d, 最后Y位置：%d\n", win->msg_count, win->last_y);

    // 步骤6：自动滚动到最新消息
    int scroll_target = win->last_y - 350; // 350是可视区域高度
    if(scroll_target < 0) scroll_target = 0;

    // 滚动外层滚动容器
    if(win->scroll_container != NULL) {
        lv_obj_scroll_to_y(win->scroll_container, scroll_target, LV_ANIM_ON);
        printf("滚动到位置：%d\n", scroll_target);
    } else {
        printf("警告：滚动容器为空\n");
    }

    // 强制刷新显示
    lv_refr_now(NULL);
    printf("消息显示完成\n");
}

// 聊天消息发送回调
void chat_send_msg_cb(lv_event_t * e)
{
    lv_obj_t * send_btn = lv_event_get_target(e);
    lv_obj_t * textarea = (lv_obj_t *)lv_event_get_user_data(e);

    if(textarea == NULL) {
        printf("错误：输入框为空\n");
        return;
    }

    // 获取输入框文本
    const char * msg = lv_textarea_get_text(textarea);
    if(msg == NULL || strlen(msg) == 0) {
        printf("错误：消息为空\n");
        return;
    }

    // 关键修复：获取正确的消息容器
    // 发送按钮 → 输入容器 → 聊天页面 → 滚动容器 → 内部消息容器
    lv_obj_t * input_container = lv_obj_get_parent(send_btn);
    if(input_container == NULL) {
        printf("错误：找不到输入容器\n");
        return;
    }

    lv_obj_t * chat_page = lv_obj_get_parent(input_container);
    if(chat_page == NULL) {
        printf("错误：找不到聊天页面\n");
        return;
    }

    // 查找滚动容器（聊天页面的第一个子元素）
    lv_obj_t * scroll_container = lv_obj_get_child(chat_page, 0);
    if(scroll_container == NULL) {
        printf("错误：找不到滚动容器\n");
        return;
    }

    // 查找内部消息容器（滚动容器的第一个子元素）
    lv_obj_t * msg_container = lv_obj_get_child(scroll_container, 0);
    if(msg_container == NULL) {
        printf("错误：找不到消息容器\n");
        return;
    }

    printf("找到消息容器：%p\n", msg_container);
    printf("消息内容：%s\n", msg);

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

    if(user_idx < 0) {
        printf("错误：用户索引无效\n");
        return;
    }

    printf("当前用户：%s\n", user_list[user_idx].nickname);

    // 获取目标IP/端口
    char target_ip[20];
    unsigned short target_port;
    strcpy(target_ip, user_list[user_idx].target_ip);
    target_port = user_list[user_idx].target_port;

    // 按服务器协议拼接消息
    char send_buf[2048];
    snprintf(send_buf, sizeof(send_buf), "msg@%s@%hu@%s", target_ip, target_port, msg);

    // 发送消息到服务器
    if(tcpsock > 0) {
        int send_len = send(tcpsock, send_buf, strlen(send_buf), 0);
        if(send_len > 0) {
            printf("发送成功：%s\n", send_buf);
            // 关键修复：本地显示自己发送的消息
            update_chat_msg(msg_container, msg, 1);
        } else {
            perror("发送失败");
            update_chat_msg(msg_container, "发送失败！", 0);
        }
    } else {
        printf("错误：TCP未连接\n");
        update_chat_msg(msg_container, "TCP未连接，发送失败!", 0);
    }

    // 清空输入框
    lv_textarea_set_text(textarea, "");

    // 强制刷新界面
    lv_refr_now(NULL);
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
    // 1. 创建外层滚动容器
    lv_obj_t * scroll_container = lv_obj_create(chat_tab);
    lv_obj_set_size(scroll_container, 580, 350);
    lv_obj_set_pos(scroll_container, 0, 0);
    lv_obj_set_style_bg_color(scroll_container, lv_color_hex(0xEDEDED), 0);
    lv_obj_set_style_border_width(scroll_container, 0, 0);
    lv_obj_set_style_pad_all(scroll_container, 0, 0);

    // 启用滚动
    lv_obj_set_scrollbar_mode(scroll_container, LV_SCROLLBAR_MODE_AUTO);
    lv_obj_set_scroll_dir(scroll_container, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(scroll_container, LV_SCROLL_SNAP_NONE);
    lv_obj_set_style_radius(scroll_container, 0, 0);

    // 2. 创建内部消息容器（在滚动容器内部）
    lv_obj_t * msg_container = lv_obj_create(scroll_container);
    lv_obj_remove_style_all(msg_container);
    lv_obj_set_size(msg_container, 560, 350); // 略小于滚动容器
    lv_obj_set_pos(msg_container, 0, 0);
    lv_obj_set_style_bg_color(msg_container, lv_color_hex(0xEDEDED), 0);
    lv_obj_set_style_border_width(msg_container, 0, 0);
    lv_obj_set_style_pad_all(msg_container, 0, 0);

    // 禁用内部容器的滚动
    lv_obj_set_scrollbar_mode(msg_container, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(msg_container, LV_OBJ_FLAG_SCROLLABLE);

    // 保存到用户结构体
    for(int i = 0; i < 5; i++) {
        if(strcmp(user_list[i].nickname, username) == 0) {
            user_list[i].msg_container = msg_container;
            // 初始化消息窗口状态
            msg_windows[i].msg_container    = msg_container;
            msg_windows[i].scroll_container = scroll_container;
            msg_windows[i].msg_count        = 0;
            msg_windows[i].container_height = 350;
            msg_windows[i].last_y           = 20;
            printf("初始化聊天窗口 %d: %s, 容器: %p\n", i, username, msg_container);
            break;
        }
    }

    // 3. 底部输入区容器（固定在聊天标签底部，不滚动）
    lv_obj_t * input_container = lv_obj_create(chat_tab);
    lv_obj_set_size(input_container, 580, 120);
    lv_obj_set_pos(input_container, 0, 350); // 固定在消息容器下方
    lv_obj_set_style_bg_color(input_container, lv_color_white(), 0);
    lv_obj_set_style_border_width(input_container, 0, 0);
    lv_obj_set_style_pad_all(input_container, 0, 0);
    lv_obj_set_style_border_color(input_container, lv_color_hex(0xDCDCDC), 0);
    lv_obj_set_style_border_width(input_container, 1, 0);
    lv_obj_set_style_border_side(input_container, LV_BORDER_SIDE_TOP, 0);

    // 4. 消息输入框
    lv_obj_t * msg_textarea = lv_textarea_create(input_container);
    lv_obj_set_size(msg_textarea, 450, 60);
    lv_obj_set_pos(msg_textarea, 5, 50);
    lv_textarea_set_placeholder_text(msg_textarea, "Type a message...");
    lv_obj_set_style_text_font(msg_textarea, &lv_font_montserrat_14, 0);
    lv_obj_set_style_border_color(msg_textarea, lv_color_hex(0xDCDCDC), 0);
    lv_obj_set_style_border_width(msg_textarea, 1, 0);
    lv_obj_set_style_radius(msg_textarea, 5, 0);

    lv_obj_add_event_cb(msg_textarea, msg_textarea_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(msg_textarea, msg_textarea_focus_cb, LV_EVENT_DEFOCUSED, NULL);

    // 5. 发送按钮
    lv_obj_t * send_btn = lv_btn_create(input_container);
    lv_obj_set_size(send_btn, 100, 60);
    lv_obj_align_to(send_btn, msg_textarea, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
    lv_obj_set_style_bg_color(send_btn, lv_color_hex(0x00bfff), 0);
    lv_obj_set_style_bg_opa(send_btn, 255, 0);
    lv_obj_set_style_radius(send_btn, 5, 0);

    lv_obj_t * send_label = lv_label_create(send_btn);
    lv_label_set_text(send_label, "Send");
    lv_obj_set_style_text_color(send_label, lv_color_white(), 0);
    lv_obj_center(send_label);

    // 绑定回调，传递正确的文本区域指针
    lv_obj_add_event_cb(send_btn, chat_send_msg_cb, LV_EVENT_CLICKED, msg_textarea);

    // 6. 表情包按钮
    lv_obj_t * emoji_btn = lv_btn_create(input_container);
    lv_obj_set_size(emoji_btn, 30, 30);
    lv_obj_set_pos(emoji_btn, 20, 10);
    lv_obj_set_style_radius(emoji_btn, 0, 0);

    lv_obj_t * emoji_title = lv_img_create(emoji_btn);
    lv_img_set_src(emoji_title, "S:/emoji.jpg");
    lv_obj_set_size(emoji_title, 30, 30);
    lv_obj_center(emoji_title);

    lv_obj_add_event_cb(emoji_btn, emoji_btn_cb, LV_EVENT_CLICKED, NULL);

    // 7. 文件按钮
    lv_obj_t * file_btn = lv_btn_create(input_container);
    lv_obj_set_size(file_btn, 30, 30);
    lv_obj_align_to(file_btn, emoji_btn, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style_radius(file_btn, 0, 0);

    lv_obj_t * file_title = lv_img_create(file_btn);
    lv_img_set_src(file_title, "S:/file.jpg");
    lv_obj_set_size(file_title, 30, 30);
    lv_obj_center(file_title);

    lv_obj_add_event_cb(file_btn, file_btn_cb, LV_EVENT_CLICKED, NULL);

    printf("聊天页面创建完成：%s\n", username);
}
void qr_btn_delete_cb(lv_event_t * e)
{
    lv_obj_t * target = lv_event_get_target(e);
    if(lv_obj_is_valid(target)) {
        lv_obj_del(target);
    }
}

void btn_qr_cb(lv_event_t * e)
{
    lv_obj_t * target     = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_PRESSING) {
        // 检查是否已创建
        static lv_obj_t * btn_qr1 = NULL;

        if(btn_qr1 == NULL || !lv_obj_is_valid(btn_qr1)) {
            // 创建放大二维码按钮
            btn_qr1 = lv_btn_create(lv_obj_get_parent(target));
            lv_obj_set_size(btn_qr1, 300, 300);
            lv_obj_set_pos(btn_qr1, 200, 80);
            lv_obj_set_style_radius(btn_qr1, 0, 0);
            lv_obj_set_style_bg_opa(btn_qr1, LV_OPA_0, 0);
            lv_obj_set_style_border_width(btn_qr1, 0, 0);

            // 创建放大图片
            lv_obj_t * img2 = lv_img_create(btn_qr1);
            lv_img_set_src(img2, "S:/qrcode1.jpg");
            lv_obj_set_size(img2, 300, 300);
            lv_obj_center(img2);

            // 添加释放事件删除
            lv_obj_add_event_cb(btn_qr1, qr_btn_delete_cb, LV_EVENT_RELEASED, NULL);
        }
    }
}

void btn_circle1_p1_cb(lv_event_t * e)
{
    lv_obj_t * target     = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_PRESSING) {
        // 检查是否已创建
        static lv_obj_t * btn_circle1_p1 = NULL;

        if(btn_circle1_p1 == NULL || !lv_obj_is_valid(btn_circle1_p1)) {
            // 创建放大二维码按钮
            btn_circle1_p1 = lv_btn_create(lv_obj_get_parent(target));
            lv_obj_set_size(btn_circle1_p1, 300, 300);
            lv_obj_set_pos(btn_circle1_p1, 200, 80);
            lv_obj_set_style_radius(btn_circle1_p1, 0, 0);
            lv_obj_set_style_bg_opa(btn_circle1_p1, LV_OPA_0, 0);
            lv_obj_set_style_border_width(btn_circle1_p1, 0, 0);

            // 创建放大图片
            lv_obj_t * img_circle1_p1 = lv_img_create(btn_circle1_p1);
            lv_img_set_src(img_circle1_p1, "S:/circle1_p1_b.jpg");
            lv_obj_set_size(img_circle1_p1, 300, 300);
            lv_obj_center(img_circle1_p1);

            // 添加释放事件删除
            lv_obj_add_event_cb(btn_circle1_p1, qr_btn_delete_cb, LV_EVENT_RELEASED, NULL);
        }
    }
}

void btn_circle1_p9_cb(lv_event_t * e)
{
    lv_obj_t * target     = lv_event_get_target(e);
    lv_event_code_t event = lv_event_get_code(e);

    if(event == LV_EVENT_PRESSING) {
        // 检查是否已创建
        static lv_obj_t * btn_circle1_p9 = NULL;

        if(btn_circle1_p9 == NULL || !lv_obj_is_valid(btn_circle1_p9)) {
            // 创建放大二维码按钮
            btn_circle1_p9 = lv_btn_create(lv_obj_get_parent(target));
            lv_obj_set_size(btn_circle1_p9, 300, 300);
            lv_obj_set_pos(btn_circle1_p9, 200, 80);
            lv_obj_set_style_radius(btn_circle1_p9, 0, 0);
            lv_obj_set_style_bg_opa(btn_circle1_p9, LV_OPA_0, 0);
            lv_obj_set_style_border_width(btn_circle1_p9, 0, 0);

            // 创建放大图片
            lv_obj_t * img_circle1_p3 = lv_img_create(btn_circle1_p9);
            lv_img_set_src(img_circle1_p3, "S:/circle1_p9_b.jpg");
            lv_obj_set_size(img_circle1_p3, 300, 300);
            lv_obj_center(img_circle1_p3);

            // 添加释放事件删除
            lv_obj_add_event_cb(btn_circle1_p9, qr_btn_delete_cb, LV_EVENT_RELEASED, NULL);
        }
    }
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
    tab7 = lv_tabview_add_tab(tabview1, "Mine");
    lv_tabview_set_act(tabview1, 0, LV_ANIM_OFF);

    // Chat Room
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

    // Contact Person
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

    // 朋友圈用户头像按钮
    lv_obj_t * btn_circle1 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle1, 50, 50);
    lv_obj_set_pos(btn_circle1, 150, 10);

    // 朋友圈头像图片
    lv_obj_t * img_circle1 = lv_img_create(btn_circle1);
    lv_img_set_src(img_circle1, "S:/friend1.jpg");
    lv_obj_set_size(img_circle1, 50, 50);
    lv_obj_center(img_circle1);

    // 朋友圈信息标签
    lv_obj_t * lb_circle1_c1 = lv_label_create(tab3);
    lv_label_set_text(lb_circle1_c1, "Leo");
    lv_obj_align_to(lb_circle1_c1, btn_circle1, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
    lv_obj_set_style_text_font(lb_circle1_c1, &lv_font_montserrat_20, LV_STATE_DEFAULT);

    // 朋友圈文案
    lv_obj_t * lb_circle1_c2 = lv_label_create(tab3);
    lv_label_set_text(lb_circle1_c2, "Life is made of small, ordinary moments that turn out to be extraordinary. Slow "
                                     "down, breathe, and enjoy every second of it.");
    lv_obj_set_size(lb_circle1_c2, 300, 60);
    lv_obj_align_to(lb_circle1_c2, lb_circle1_c1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_obj_set_style_text_font(lb_circle1_c2, &lv_font_montserrat_14, LV_STATE_DEFAULT);

    // 朋友圈图片1
    lv_obj_t * btn_circle1_p1 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle1_p1, 100, 100);
    lv_obj_align_to(btn_circle1_p1, lb_circle1_c2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_obj_t * img_circle1_p1 = lv_img_create(btn_circle1_p1);
    lv_img_set_src(img_circle1_p1, "S:circle1_p1.jpg");
    lv_obj_set_size(img_circle1_p1, 100, 100);
    lv_obj_center(img_circle1_p1);
    lv_obj_add_event_cb(btn_circle1_p1, btn_circle1_p1_cb, LV_EVENT_PRESSING, NULL);

    // 朋友圈图片2
    lv_obj_t * btn_circle1_p2 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle1_p2, 100, 100);
    lv_obj_align_to(btn_circle1_p2, btn_circle1_p1, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t * img_circle1_p2 = lv_img_create(btn_circle1_p2);
    lv_img_set_src(img_circle1_p2, "S:circle1_p2.jpg");
    lv_obj_set_size(img_circle1_p2, 100, 100);
    lv_obj_center(img_circle1_p2);
    // 朋友圈图片3
    lv_obj_t * btn_circle1_p3 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle1_p3, 100, 100);
    lv_obj_align_to(btn_circle1_p3, btn_circle1_p2, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);
    lv_obj_t * img_circle1_p3 = lv_img_create(btn_circle1_p3);
    lv_img_set_src(img_circle1_p3, "S:circle1_p3.jpg");
    lv_obj_set_size(img_circle1_p3, 100, 100);
    lv_obj_center(img_circle1_p3);
    
    // 朋友圈图片4
    lv_obj_t * btn_circle1_p4 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle1_p4, 100, 100);
    lv_obj_align_to(btn_circle1_p4, btn_circle1_p1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_t * img_circle1_p4 = lv_img_create(btn_circle1_p4);
    lv_img_set_src(img_circle1_p4, "S:circle1_p4.jpg");
    lv_obj_set_size(img_circle1_p4, 100, 100);
    lv_obj_center(img_circle1_p4);

    // 朋友圈图片5
    lv_obj_t * btn_circle1_p5 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle1_p5, 100, 100);
    lv_obj_align_to(btn_circle1_p5, btn_circle1_p4, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);
    lv_obj_t * img_circle1_p5 = lv_img_create(btn_circle1_p5);
    lv_img_set_src(img_circle1_p5, "S:circle1_p5.jpg");
    lv_obj_set_size(img_circle1_p5, 100, 100);
    lv_obj_center(img_circle1_p5);

    // 朋友圈图片6
    lv_obj_t * btn_circle1_p6 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle1_p6, 100, 100);
    lv_obj_align_to(btn_circle1_p6, btn_circle1_p5, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);
    lv_obj_t * img_circle1_p6 = lv_img_create(btn_circle1_p6);
    lv_img_set_src(img_circle1_p6, "S:circle1_p7.jpg");
    lv_obj_set_size(img_circle1_p6, 100, 100);
    lv_obj_center(img_circle1_p6);

    // 朋友圈图片7
    lv_obj_t * btn_circle1_p7 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle1_p7, 100, 100);
    lv_obj_align_to(btn_circle1_p7, btn_circle1_p4, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_t * img_circle1_p7 = lv_img_create(btn_circle1_p7);
    lv_img_set_src(img_circle1_p7, "S:circle1_p6.jpg");
    lv_obj_set_size(img_circle1_p7, 100, 100);
    lv_obj_center(img_circle1_p7);


    // 朋友圈图片8
    lv_obj_t * btn_circle1_p8 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle1_p8, 100, 100);
    lv_obj_align_to(btn_circle1_p8, btn_circle1_p7, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);
    lv_obj_t * img_circle1_p8 = lv_img_create(btn_circle1_p8);
    lv_img_set_src(img_circle1_p8, "S:circle1_p8.jpg");
    lv_obj_set_size(img_circle1_p8, 100, 100);
    lv_obj_center(img_circle1_p8);


    // 朋友圈图片9
    lv_obj_t * btn_circle1_p9 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle1_p9, 100, 100);
    lv_obj_align_to(btn_circle1_p9, btn_circle1_p8, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);
    lv_obj_t * img_circle1_p9 = lv_img_create(btn_circle1_p9);
    lv_img_set_src(img_circle1_p9, "S:circle1_p9.jpg");
    lv_obj_set_size(img_circle1_p9, 100, 100);
    lv_obj_center(img_circle1_p9);
    lv_obj_add_event_cb(btn_circle1_p9, btn_circle1_p9_cb, LV_EVENT_PRESSING, NULL);


    // 分割线
    lv_obj_t * circle_separator1 = lv_obj_create(tab3);
    lv_obj_remove_style_all(circle_separator1);
    lv_obj_set_size(circle_separator1, 400, 1);
    lv_obj_align_to(circle_separator1, img_circle1_p7, LV_ALIGN_OUT_BOTTOM_LEFT, -50, 50);
    lv_obj_set_style_bg_color(circle_separator1, lv_color_hex(0x696969), 0);
    lv_obj_set_style_bg_opa(circle_separator1, LV_OPA_COVER, 0);

    // 朋友圈用户头像按钮2
    lv_obj_t * btn_circle2 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle2, 50, 50);
    lv_obj_align_to(btn_circle2, circle_separator1, LV_ALIGN_OUT_BOTTOM_LEFT, 10, 50);

    // 朋友圈头像图片2
    lv_obj_t * img_circle2 = lv_img_create(btn_circle2);
    lv_img_set_src(img_circle2, "S:/friend2.jpg");
    lv_obj_set_size(img_circle2, 50, 50);
    lv_obj_center(img_circle2);

    // 朋友圈信息标签2
    lv_obj_t * lb_circle2_c1 = lv_label_create(tab3);
    lv_label_set_text(lb_circle2_c1, "Eli");
    lv_obj_align_to(lb_circle2_c1, btn_circle2, LV_ALIGN_OUT_RIGHT_TOP, 10, 0);
    lv_obj_set_style_text_font(lb_circle2_c1, &lv_font_montserrat_20, LV_STATE_DEFAULT);

    // 朋友圈文案
    lv_obj_t * lb_circle2_c2 = lv_label_create(tab3);
    lv_label_set_text(lb_circle2_c2, "Not every day is exciting, but every day is worth living. Find happiness in the "
                                     "little things, and keep your heart gentle.");
    lv_obj_set_size(lb_circle2_c2, 300, 50);
    lv_obj_align_to(lb_circle2_c2, lb_circle2_c1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_obj_set_style_text_font(lb_circle2_c2, &lv_font_montserrat_14, LV_STATE_DEFAULT);

    // 朋友圈图片1
    lv_obj_t * btn_circle2_p1 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle2_p1, 100, 100);
    lv_obj_align_to(btn_circle2_p1, lb_circle2_c2, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    lv_obj_t * img_circle2_p1 = lv_img_create(btn_circle2_p1);
    lv_img_set_src(img_circle2_p1, "S:circle2_p1.jpg");
    lv_obj_set_size(img_circle2_p1, 100, 100);
    lv_obj_center(img_circle2_p1);
    // 朋友圈图片2
    lv_obj_t * btn_circle2_p2 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle2_p2, 100, 100);
    lv_obj_align_to(btn_circle2_p2, btn_circle2_p1, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t * img_circle2_p2 = lv_img_create(btn_circle2_p2);
    lv_img_set_src(img_circle2_p2, "S:circle2_p2.jpg");
    lv_obj_set_size(img_circle2_p2, 100, 100);
    lv_obj_center(img_circle2_p2);
    // 朋友圈图片3
    lv_obj_t * btn_circle2_p3 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle2_p3, 100, 100);
    lv_obj_align_to(btn_circle2_p3, btn_circle2_p2, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);
    lv_obj_t * img_circle2_p3 = lv_img_create(btn_circle2_p3);
    lv_img_set_src(img_circle2_p3, "S:circle2_p3.jpg");
    lv_obj_set_size(img_circle2_p3, 100, 100);
    lv_obj_center(img_circle2_p3);
    // 朋友圈图片4
    lv_obj_t * btn_circle2_p4 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle2_p4, 100, 100);
    lv_obj_align_to(btn_circle2_p4, btn_circle2_p1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_t * img_circle2_p4 = lv_img_create(btn_circle2_p4);
    lv_img_set_src(img_circle2_p4, "S:circle2_p4.jpg");
    lv_obj_set_size(img_circle2_p4, 100, 100);
    lv_obj_center(img_circle2_p4);

    // 朋友圈图片5
    lv_obj_t * btn_circle2_p5 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle2_p5, 100, 100);
    lv_obj_align_to(btn_circle2_p5, btn_circle2_p4, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);
    lv_obj_t * img_circle2_p5 = lv_img_create(btn_circle2_p5);
    lv_img_set_src(img_circle2_p5, "S:circle2_p5.jpg");
    lv_obj_set_size(img_circle2_p5, 100, 100);
    lv_obj_center(img_circle2_p5);

    // 朋友圈图片6
    lv_obj_t * btn_circle2_p6 = lv_btn_create(tab3);
    lv_obj_set_size(btn_circle2_p6, 100, 100);
    lv_obj_align_to(btn_circle2_p6, btn_circle2_p5, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);
    lv_obj_t * img_circle2_p6 = lv_img_create(btn_circle2_p6);
    lv_img_set_src(img_circle2_p6, "S:circle2_p6.jpg");
    lv_obj_set_size(img_circle2_p6, 100, 100);
    lv_obj_center(img_circle2_p6);

    // Mine
    // 我的头像按钮
    lv_obj_t * btn_self = lv_btn_create(tab7);
    lv_obj_set_size(btn_self, 100, 100);
    lv_obj_set_pos(btn_self, 100, 10);

    lv_obj_t * img1 = lv_img_create(btn_self);
    lv_img_set_src(img1, "S:/myself.jpg");
    lv_obj_set_size(img1, 100, 100);
    lv_obj_center(img1);

    // 个人信息标签
    lv_obj_t * lb_c1 = lv_label_create(tab7);
    lv_label_set_text_fmt(lb_c1, "Username: %s", "yanyadong");
    lv_obj_align_to(lb_c1, btn_self, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    lv_obj_set_style_text_font(lb_c1, &lv_font_montserrat_16, LV_STATE_DEFAULT);

    lv_obj_t * lb_c2 = lv_label_create(tab7);
    lv_label_set_text_fmt(lb_c2, "Whisper Num: %s", "20031009");
    lv_obj_align_to(lb_c2, btn_self, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style_text_font(lb_c2, &lv_font_montserrat_16, LV_STATE_DEFAULT);

    lv_obj_t * lb_c3 = lv_label_create(tab7);
    lv_label_set_text_fmt(lb_c3, "region: %s", "guangzhou");
    lv_obj_align_to(lb_c3, btn_self, LV_ALIGN_OUT_RIGHT_BOTTOM, 20, 0);
    lv_obj_set_style_text_font(lb_c3, &lv_font_montserrat_16, LV_STATE_DEFAULT);

    // 二维码图片
    lv_obj_t * btn_qr = lv_btn_create(tab7);
    lv_obj_set_size(btn_qr, 20, 20);
    lv_obj_align_to(btn_qr, lb_c3, LV_ALIGN_OUT_RIGHT_MID, 30, 0);
    lv_obj_set_style_radius(btn_qr, 0, 0);

    lv_obj_t * img2 = lv_img_create(btn_qr);
    lv_img_set_src(img2, "S:/qrcode.jpg");
    lv_obj_set_size(img2, 20, 20);
    lv_obj_center(img2);

    // 改为PRESING事件
    lv_obj_add_event_cb(btn_qr, btn_qr_cb, LV_EVENT_PRESSING, NULL);

    // 分割线
    lv_obj_t * self_separator1 = lv_obj_create(tab7);
    lv_obj_remove_style_all(self_separator1);
    lv_obj_set_size(self_separator1, 350, 1);
    lv_obj_align_to(self_separator1, btn_self, LV_ALIGN_OUT_BOTTOM_LEFT, -5, 20);
    lv_obj_set_style_bg_color(self_separator1, lv_color_hex(0x696969), 0);
    lv_obj_set_style_bg_opa(self_separator1, LV_OPA_COVER, 0);

    // 个性签名
    lv_obj_t * lb_c4 = lv_label_create(tab7);
    lv_label_set_text(lb_c4, "Personal signature");
    lv_obj_set_size(lb_c4, 100, 50);
    lv_obj_align_to(lb_c4, self_separator1, LV_ALIGN_OUT_BOTTOM_LEFT, 15, 30);
    lv_obj_set_style_text_font(lb_c4, &lv_font_montserrat_16, LV_STATE_DEFAULT);

    lv_obj_t * lb_c5 = lv_label_create(tab7);
    lv_label_set_text(
        lb_c5,
        "I am learning to love the ordinary days, the quiet growth, and the gentle progress that no one else sees.");
    lv_obj_set_size(lb_c5, 230, 80);
    lv_obj_align_to(lb_c5, lb_c3, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 40);
    lv_obj_set_style_text_font(lb_c5, &lv_font_montserrat_14, LV_STATE_DEFAULT);

    // 分割线
    lv_obj_t * self_separator2 = lv_obj_create(tab7);
    lv_obj_remove_style_all(self_separator2);
    lv_obj_set_size(self_separator2, 350, 1);
    lv_obj_align_to(self_separator2, self_separator1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 100);
    lv_obj_set_style_bg_color(self_separator2, lv_color_hex(0x696969), 0);
    lv_obj_set_style_bg_opa(self_separator2, LV_OPA_COVER, 0);

    // 个性图片
    lv_obj_t * lb_c6 = lv_label_create(tab7);
    lv_label_set_text(lb_c6, "Friends Circle");
    lv_obj_set_size(lb_c6, 80, 30);
    lv_obj_align_to(lb_c6, self_separator2, LV_ALIGN_OUT_BOTTOM_LEFT, 20, 30);
    lv_obj_set_style_text_font(lb_c6, &lv_font_montserrat_16, LV_STATE_DEFAULT);

    lv_obj_t * img3 = lv_img_create(tab7);
    lv_img_set_src(img3, "S:/my_space1.jpg");
    lv_obj_set_size(img3, 50, 50);
    lv_obj_align_to(img3, lb_c5, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 25);

    lv_obj_t * img4 = lv_img_create(tab7);
    lv_img_set_src(img4, "S:/my_space2.jpg");
    lv_obj_set_size(img4, 50, 50);
    lv_obj_align_to(img4, img3, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

    lv_obj_t * img5 = lv_img_create(tab7);
    lv_img_set_src(img5, "S:/my_space3.jpg");
    lv_obj_set_size(img5, 50, 50);
    lv_obj_align_to(img5, img4, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

     lv_obj_t * img6 = lv_img_create(tab7);
    lv_img_set_src(img6, "S:/my_space4.jpg");
    lv_obj_set_size(img6, 50, 50);
    lv_obj_align_to(img6, img5, LV_ALIGN_OUT_RIGHT_MID, 5, 0);


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
