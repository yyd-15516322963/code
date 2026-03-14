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

#define ON 0
#define OFF 1

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

#define DISP_BUF_SIZE (800 * 480)

// 全局UI对象
lv_obj_t * page1;
lv_obj_t * page2;
lv_obj_t * sec_page1;
lv_obj_t * sec_page2;
lv_obj_t * bt_key;
lv_obj_t * bt_key2;
lv_obj_t * bt1;
lv_obj_t * bt2;
lv_obj_t * kb;
lv_obj_t * ta1;
lv_obj_t * ta2;
lv_obj_t * tab1;
lv_obj_t * tab2;
lv_obj_t * tab3; // Shopping Car页面
lv_obj_t * tab4;
lv_obj_t * tab2_1;
lv_obj_t * tab2_2;
lv_obj_t * tab2_3;
lv_obj_t * sec_bt1;
lv_obj_t * sec_bt2;
lv_obj_t * sec_ta1;
lv_obj_t * sec_ta2;
lv_obj_t * sec_kb1;
lv_obj_t * sec_kb2;
lv_obj_t * pay_ui;

// 管理员操作表单控件
lv_obj_t * admin_ta_id;            // 商品ID输入框
lv_obj_t * admin_ta_name;          // 商品名称输入框
lv_obj_t * admin_ta_price;         // 商品价格输入框
lv_obj_t * admin_ta_img;           // 图片路径输入框
lv_obj_t * admin_goods_list_label; // 商品列表展示标签


// ========== 商品数据结构重构（动态化） ==========
// 商品结构体
typedef struct
{
    int id;             // 商品唯一ID
    char name[1000];    // 商品名称（固定内存）
    char price[32];     // 商品价格（固定内存）
    char img_path[128]; // 商品图片路径（固定内存）
} goods_item_t;

// 购物车项（商品+数量）
typedef struct
{
    goods_item_t goods;
    int count;
} cart_item_t;


// 动态商品列表配置
#define MAX_GOODS_COUNT 50
#define MAX_CART_ITEMS 20
// 全局购物车（最大20个商品）

extern cart_item_t shopping_cart[];
extern int cart_item_count;
// 分离的商品列表：
// 1. purchase_goods_list: 购物页面的商品列表（只读，管理员操作不影响）
// 2. admin_goods_list: 管理员页面的商品列表（可编辑）
extern goods_item_t purchase_goods_list[]; // 购物页面商品列表
extern int purchase_goods_count;   // 购物页面商品数量

extern goods_item_t admin_goods_list[]; // 管理员页面商品列表
extern int admin_goods_count;   // 管理员页面商品数量


#endif 


